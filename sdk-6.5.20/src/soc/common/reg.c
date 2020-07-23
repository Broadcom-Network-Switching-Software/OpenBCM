/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Register address and value manipulations.
 */


#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <sal/core/boot.h>

#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/register.h>

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#endif /* BCM_PETRA_SUPPORT */
#if defined(BCM_DFE_SUPPORT)
#include <soc/dfe/cmn/dfe_drv.h>
#endif /* BCM_DFE_SUPPORT */
#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#endif
#if defined(BCM_DNXF_SUPPORT)
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif /* BCM_DNXF_SUPPORT */
#ifdef DNX_TEST_CHIPS_SUPPORT
#include <soc/dpp/dnxtestchip.h>
#endif
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#endif
#if defined(BCM_GREYHOUND_SUPPORT)
#include <soc/greyhound.h>
#endif
#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#endif

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif /* CRASH_RECOVERY_SUPPORT */

#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif
#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#endif

#ifdef CANCUN_SUPPORT
#include <soc/esw/cancun.h>
#endif

#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif
#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif

#ifdef BCM_SAND_SUPPORT
#include <soc/sand/sand_aux_access.h>
#endif

#include <soc/dnxc/multithread_analyzer.h>

#ifdef BCM_DNX_SUPPORT
#define CDMAC_OFFSET_CNT    0x10000
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
#define CDMAC_OFFSET_CNT    0x10000
#define CDMAC0_STAGE_ID     1
#define CDMAC1_STAGE_ID     2
#endif

/*
 * Function:   soc_reg_datamask
 * Purpose:    Generate data mask for the fields in a register
 *             whose flags match the flags parameter
 * Returns:    The data mask
 *
 * Notes:  flags can be SOCF_RO, SOCF_WO, or zero (read/write)
 */
uint32
soc_reg_datamask(int unit, soc_reg_t reg, int flags)
{
    int              i, start, end;
    soc_field_info_t *fieldp;
    soc_reg_info_t   *regp;
    uint32           result, mask;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    regp = &(SOC_REG_INFO(unit, reg));

    result = 0;
    for (i = 0; i < (int)(regp->nFields); i++) {
        fieldp = &(regp->fields[i]);

        if ((fieldp->flags & flags) == flags) {
            start = fieldp->bp;
            if (start > 31) {
                continue;
            }
            end = fieldp->bp + fieldp->len;
            if (end < 32) {
                mask = (1 << end) - 1;
            } else {
                mask = -1;
            }
            result |= ((uint32)-1 << start) & mask;
        }
    }

    return result;
}

/*
 * Function:   soc_reg64_datamask
 * Purpose:    Generate data mask for the fields in a 64-bit register
 *             whose flags match the flags parameter
 * Returns:    The data mask
 *
 * Notes:  flags can be SOCF_RO, SOCF_WO, or zero (read/write)
 */
uint64
soc_reg64_datamask(int unit, soc_reg_t reg, int flags)
{
    int              i, start, end;
    soc_field_info_t *fieldp;
    soc_reg_info_t   *regp;
    uint64           mask, tmp, result;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    regp = &(SOC_REG_INFO(unit, reg));

    COMPILER_64_ZERO(result);

    for (i = 0; i < (int)(regp->nFields); i++) {
        fieldp = &(regp->fields[i]);

        if ((fieldp->flags & flags) == flags) {
            start = fieldp->bp;
            end = fieldp->bp + fieldp->len;
            COMPILER_64_SET(mask, 0, 1);
            COMPILER_64_SHL(mask, end);
            COMPILER_64_SUB_32(mask, 1);
            COMPILER_64_ZERO(tmp);
    /*    coverity[overflow_assign]    */
            COMPILER_64_SUB_32(tmp, 1);
            COMPILER_64_SHL(tmp, start);
            COMPILER_64_AND(tmp, mask);
            COMPILER_64_OR(result, tmp);
        }
    }

    return result;
}

/*
 * Function:   soc_reg_above_64_datamask
 * Purpose:    Generate data mask for the fields in above 64-bit register
 *             whose flags match the flags parameter
 *
 * Notes:  flags can be SOCF_RO, SOCF_WO, or zero (read/write)
 */
void
soc_reg_above_64_datamask(int unit, soc_reg_t reg, int flags, soc_reg_above_64_val_t datamask)
{
    int              i;
    soc_field_info_t *fieldp;
    soc_reg_info_t   *regp;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    regp = &(SOC_REG_INFO(unit, reg));

    SOC_REG_ABOVE_64_CLEAR(datamask);

    for (i = 0; i < (int)(regp->nFields); i++) {
        fieldp = &(regp->fields[i]);

        if ((fieldp->flags & flags) == flags) {
            SOC_REG_ABOVE_64_CREATE_MASK(datamask, fieldp->len, fieldp->bp);
        }
    }
}

/************************************************************************/
/* Routines for reading/writing SOC internal registers                        */
/************************************************************************/

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)

STATIC 
void _soc_snoop_reg(int unit, soc_block_t block, int acc, uint32 addr, 
                    uint32 flag, uint32 data_hi, uint32 data_lo) {
    soc_reg_info_t    *reg_info_p;
    soc_regaddrinfo_t ainfo;
    soc_reg_t         reg;

    if (bsl_check(bslLayerSoc, bslSourceTests, bslSeverityNormal, unit) == 0) {
        return;
    }
    soc_regaddrinfo_extended_get(unit, &ainfo, block, acc, addr);
    reg = (int)ainfo.reg;
    if (SOC_REG_IS_VALID(unit, reg)) {
        reg_info_p = &SOC_REG_INFO(unit, reg);
        /* (SOC_REG_SNOOP_READ & reg_info_p->snoop_flags))  */
        if (NULL != reg_info_p->snoop_cb) {
             if (reg_info_p->snoop_flags & flag) {
                 reg_info_p->snoop_cb(unit, reg,&ainfo, flag, data_hi,data_lo,
                                      reg_info_p->snoop_user_data);
             }
        }
    }
    return ;
}
#ifdef BROADCOM_DEBUG

void
_soc_reg_debug(int unit, int access_width, char *op_str,
               uint32 addr, uint32 data_hi, uint32 data_lo)
{
    soc_regaddrinfo_t ainfo;
    char              buf[80];

    ainfo.block = SOC_BLK_NONE;
    soc_regaddrinfo_get(unit, &ainfo, addr);

    if (!ainfo.valid || (int)ainfo.reg < 0) {
        sal_strncpy(buf, "??", sizeof(buf));
    } else {
        soc_reg_sprint_addr(unit, buf, &ainfo);
    }

    if (data_hi != 0) {
        LOG_VERBOSE(BSL_LS_SOC_REG,
                 (BSL_META_U(unit,
                             "soc_reg%d_%s unit %d: "
                             "%s[0x%x] data=0x%08x_%08x\n"),
                  access_width, op_str, unit,
                  buf, addr, data_hi, data_lo));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_REG,
                 (BSL_META_U(unit,
                             "soc_reg%d_%s unit %d: "
                             "%s[0x%x] data=0x%08x\n"),
                  access_width, op_str, unit,
                  buf, addr, data_lo));
    }
}

STATIC void
_soc_reg_extended_debug(int unit, int access_width, char *op_str,
                        soc_block_t block, int acc, uint32 addr, 
                        uint32 data_hi, uint32 data_lo)
{
    soc_regaddrinfo_t ainfo;
    char              buf[80];

    soc_regaddrinfo_extended_get(unit, &ainfo, block, acc, addr);

    if (!ainfo.valid || (int)ainfo.reg < 0) {
        sal_strncpy(buf, "??", sizeof(buf));
    } else {
        soc_reg_sprint_addr(unit, buf, &ainfo);
    }

    if (data_hi != 0) {
        LOG_VERBOSE(BSL_LS_SOC_REG,
                 (BSL_META_U(unit,
                             "soc_reg%d_%s unit %d: "
                             "%s[%d][0x%x] data=0x%08x_%08x\n"),
                  access_width, op_str, unit,
                  buf, block, addr, data_hi, data_lo));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_REG,
                 (BSL_META_U(unit,
                             "soc_reg%d_%s unit %d: "
                             "%s[%d][0x%x] data=0x%08x\n"),
                  access_width, op_str, unit,
                  buf, block, addr, data_lo));
    }
}

void
_soc_reg_above_64_debug(int unit, char *op_str, soc_block_t block, 
               uint32 addr, soc_reg_above_64_val_t data)
{
    soc_regaddrinfo_t ainfo;
    char              buf[80];
    int i, first_non_zero;

    soc_regaddrinfo_extended_get(unit, &ainfo, block, 0, addr);

    if (!ainfo.valid || (int)ainfo.reg < 0) {
        sal_strncpy(buf, "??", sizeof(buf));
    } else {
        soc_reg_sprint_addr(unit, buf, &ainfo);
    }

    LOG_VERBOSE(BSL_LS_SOC_REG,
             (BSL_META_U(unit,
                         "soc_reg_above_64_%s unit %d: "
                         "%s[0x%x] data="),
              op_str, unit, 
              buf, addr));

    first_non_zero = 0;
    for(i=SOC_REG_ABOVE_64_MAX_SIZE_U32-1 ; i>=0 ; i--) {
        if(0 == i) {
            LOG_VERBOSE(BSL_LS_SOC_REG,
                     (BSL_META_U(unit,
                                 "0x%08x\n"),data[i]));
        } else {
            if(data[i] != 0) {
                first_non_zero = 1;
            }

            if(1 == first_non_zero) {
                LOG_VERBOSE(BSL_LS_SOC_REG,
                         (BSL_META_U(unit,
                                     "0x%08x_"),data[i]));
            }
        }
    }
   
}

#endif /* BROADCOM_DEBUG */


#ifdef BCM_BIGMAC_SUPPORT

/* List of registers that need iterative read/write operations */
STATIC int
iterative_op_required(soc_reg_t reg)
{
    switch (reg) {
        case MAC_RXCTRLr:
        case MAC_RXMACSAr:
        case MAC_RXMAXSZr:
        case MAC_RXLSSCTRLr:
        case MAC_RXLSSSTATr:
        case MAC_RXSPARE0r:
        case IR64r:
        case IR127r:
        case IR255r:
        case IR511r:
        case IR1023r:
        case IR1518r:
        case IR2047r:
        case IR4095r:
        case IR9216r:
        case IR16383r:
        case IRMAXr:
        case IRPKTr:
        case IRFCSr:
        case IRUCr:
        case IRMCAr:
        case IRBCAr:
        case IRXPFr:
        case IRXPPr:
        case IRXUOr:
        case IRJBRr:
        case IROVRr:
        case IRXCFr:
        case IRFLRr:
        case IRPOKr:
        case IRMEGr:
        case IRMEBr:
        case IRBYTr:
        case IRUNDr:
        case IRFRGr:
        case IRERBYTr:
        case IRERPKTr:
        case IRJUNKr:
        case MAC_RXLLFCMSGCNTr:
        case MAC_RXLLFCMSGFLDSr:
            return TRUE;
            break;
        default:
            return FALSE;
            break;
    }
}

/*
 * Iterative read procedure for MAC registers on Hyperlite ports.
 */
STATIC int
soc_reg64_read_iterative(int unit, uint32 addr, soc_port_t port,
                         uint64 *data)
{
    int rv, i, diff;
    uint64 xgxs_stat;
    uint32 locked;
    sal_usecs_t t1 = 0, t2;
    soc_timeout_t to;
    for (i = 0; i < 100; i++) {
       /* Read PLL lock status */
       soc_timeout_init(&to, 25 * MILLISECOND_USEC, 0);
       do {
           t1 = sal_time_usecs();
           rv = READ_MAC_XGXS_STATr(unit, port, &xgxs_stat);
           locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                          TXPLL_LOCKf);
           if (locked || SOC_FAILURE(rv)) {
               break;
           }
       } while (!soc_timeout_check(&to));
       if (SOC_FAILURE(rv)) {
           return rv;
       }
       if (!locked) {
           continue;
       }
       /* Read register value */
       SOC_IF_ERROR_RETURN(soc_reg64_read(unit, addr, data));
       /* Read PLL lock status */
       SOC_IF_ERROR_RETURN(READ_MAC_XGXS_STATr(unit, port, &xgxs_stat));
       locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                      TXPLL_LOCKf);
       t2 = sal_time_usecs();
       diff = SAL_USECS_SUB(t2, t1);
       if (locked && (diff < 20 * MILLISECOND_USEC)) {
           return SOC_E_NONE;
       }
       LOG_VERBOSE(BSL_LS_SOC_COMMON,
                   (BSL_META_U(unit,
                               "soc_reg64_read_iterative: WARNING: "
                               "iteration %d PLL went out of lock"),
                    i));
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_reg64_read_iterative: "
                          "operation failed:\n"))); 
    return SOC_E_FAIL;    
}
#endif /* BCM_BIGMAC_SUPPORT */

/*
 * Read an internal 64-bit SOC register through S-Channel messaging buffer.
 */
int
_soc_reg64_get(int unit, soc_block_t block, int acc, uint32 addr, uint64 *reg)
{
    schan_msg_t schan_msg;
    int rv;
    uint32 allow_intr = 0;
    int data_byte_len;
    int opcode, err;

    /*
     * Write message to S-Channel.
     */
    schan_msg_clear(&schan_msg);

    data_byte_len = 8;
    soc_schan_header_cmd_set(unit, &schan_msg.header, READ_REGISTER_CMD_MSG,
                             block, 0, acc, data_byte_len, 0, 0);

    schan_msg.readcmd.address = addr;

    if(SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }
    /* Write header word + address DWORD, read header word + data DWORD */
    rv = soc_schan_op(unit, &schan_msg, 2, 3, allow_intr);
    if (SOC_FAILURE(rv)) {
#if defined(BCM_XGS_SUPPORT)
        int rv1, port = 0, index;
#endif /* BCM_XGS_SUPPORT */
        soc_regaddrinfo_t ainfo;

        if (!soc_feature(unit, soc_feature_ser_parity)) {
            return rv;
        }    
        soc_regaddrinfo_extended_get(unit, &ainfo, block, acc, addr);
        if (ainfo.reg != INVALIDr) {
            if (SOC_REG_IS_COUNTER(unit, ainfo.reg)) {
                COMPILER_64_SET(*reg, 0, 0);
                /* Force correct */
                if (!SOC_REG_RETURN_SER_ERROR(unit)) {
                    rv = SOC_E_NONE;
                }
            } 
#if defined(BCM_XGS_SUPPORT)
            else if (soc_feature(unit, soc_feature_regs_as_mem)) {
                if (SOC_REG_INFO(unit, ainfo.reg).regtype == soc_portreg) {
                    port = ainfo.port;
                } else if (SOC_REG_INFO(unit, ainfo.reg).regtype == soc_cosreg) {
                    port = ainfo.cos;
                }
                index = ainfo.idx != -1 ? ainfo.idx : 0;
                rv1 = soc_ser_reg_cache_get(unit, ainfo.reg, port, index, reg);
                if (rv1 != SOC_E_NONE) {
                    if (SOC_REG_IS_DYNAMIC(unit, ainfo.reg)) {
                        COMPILER_64_SET(*reg, 0, 0);
                    } else {
                        return rv;
                    }
                }
                /* Force correct */
                if (!SOC_REG_RETURN_SER_ERROR(unit)) {
                    rv = SOC_E_NONE;
                }
            }
#endif /* BCM_XGS_SUPPORT */
        } else {
            return rv;
        }
    }
    /* Check result */
    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
                                &err, NULL, NULL);;
    if (opcode != READ_REGISTER_ACK_MSG || err != 0) {
        {
            LOG_WARN(BSL_LS_SOC_SCHAN,
                (BSL_META_U(unit,
                    "soc_schan_op: operation failed: %s(%d)\n"), soc_errmsg(rv), rv));
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "_soc_reg64_get: "
                      "invalid S-Channel reply, expected READ_REG_ACK: got %d "
                      "block:%d address:0x%x\n"), opcode, block, addr));
            soc_schan_dump(unit, &schan_msg, 2);
            return SOC_E_INTERNAL;
        }
    }

#ifdef BROADCOM_DEBUG
    if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
        _soc_reg_extended_debug(unit, 64, "read", block, acc, addr,
                                schan_msg.readresp.data[1],
                                schan_msg.readresp.data[0]);
    }
#endif /* BROADCOM_DEBUG */
    _soc_snoop_reg(unit, block, acc, addr,SOC_REG_SNOOP_READ, 
                   schan_msg.readresp.data[1],schan_msg.readresp.data[0]);

    COMPILER_64_SET(*reg,
                    schan_msg.readresp.data[1],
                    schan_msg.readresp.data[0]);

    return SOC_E_NONE;
}

#ifdef BCM_BIGMAC_SUPPORT

/*
 * Iterative read procedure for MAC registers on Hyperlite ports.
 */
STATIC int
soc_reg64_get_iterative(int unit, soc_block_t block, int acc, uint32 addr,
                        soc_port_t port, uint64 *data)
{
    int rv, i, diff;
    uint64 xgxs_stat;
    uint32 locked;
    sal_usecs_t t1 = 0, t2;
    soc_timeout_t to;
    for (i = 0; i < 100; i++) {
       /* Read PLL lock status */
       soc_timeout_init(&to, 25 * MILLISECOND_USEC, 0);
       do {
           t1 = sal_time_usecs();
           rv = READ_MAC_XGXS_STATr(unit, port, &xgxs_stat);
           locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                          TXPLL_LOCKf);
           if (locked || SOC_FAILURE(rv)) {
               break;
           }
       } while (!soc_timeout_check(&to));
       if (SOC_FAILURE(rv)) {
           return rv;
       }
       if (!locked) {
           continue;
       }
       /* Read register value */
       SOC_IF_ERROR_RETURN(_soc_reg64_get(unit, block, acc, addr, data));
       /* Read PLL lock status */
       SOC_IF_ERROR_RETURN(READ_MAC_XGXS_STATr(unit, port, &xgxs_stat));
       locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                      TXPLL_LOCKf);
       t2 = sal_time_usecs();
       diff = SAL_USECS_SUB(t2, t1);
       if (locked && (diff < 20 * MILLISECOND_USEC)) {
           return SOC_E_NONE;
       }
       LOG_VERBOSE(BSL_LS_SOC_COMMON,
                   (BSL_META_U(unit,
                               "soc_reg64_get_iterative: WARNING: "
                               "iteration %d PLL went out of lock"),
                    i));
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_reg64_get_iterative: "
                          "operation failed:\n"))); 
    return SOC_E_FAIL;    
}

#endif /* BCM_BIGMAC_SUPPORT */

/*
 * Read an internal SOC register through S-Channel messaging buffer.
 * Checks if the register is 32 or 64 bits.
 */

int
soc_reg_read(int unit, soc_reg_t reg, uint32 addr, uint64 *data)
{
    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_REG, reg, FALSE));

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg_read(unit, reg, addr, data);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */

    if (SOC_REG_IS_ABOVE_64(unit, reg)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_reg_read: "
                              "Use soc_reg_above_64_get \n")));
        
        return SOC_E_FAIL;
    }
    
    if (SOC_REG_IS_64(unit, reg)) {
        soc_port_t port;
        soc_block_types_t regblktype = SOC_REG_INFO(unit, reg).block;
        int blk, pindex, bindex, block;
        pindex = (addr >> SOC_REGIDX_BP) & 0x3f;
        block = ((addr >> SOC_BLOCK_BP) & 0xf) |
                (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
        if (SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_PORT) 
#ifdef BCM_SAND_SUPPORT
           && !SOC_IS_SAND(unit)
#endif /* BCM_SAND_SUPPORT */
#ifdef BCM_BIGMAC_SUPPORT
           && iterative_op_required(reg)
#endif /* BCM_BIGMAC_SUPPORT */
            ) {
            PBMP_HYPLITE_ITER(unit, port) {
                blk = SOC_PORT_BLOCK(unit, port);
                bindex = SOC_PORT_BINDEX(unit, port);
                if ((SOC_BLOCK2SCH(unit, blk) == block) && (bindex == pindex)) {
                    break;
                }
            }         
            if (!IS_HYPLITE_PORT(unit, port)) {
                return soc_reg64_read(unit, addr, data);
            } 
#ifdef BCM_BIGMAC_SUPPORT
            else {   
                return soc_reg64_read_iterative(unit, addr, port, data);
            }
#endif /* BCM_BIGMAC_SUPPORT */           
        } else {
            return soc_reg64_read(unit, addr, data);
        }
    } else {
        uint32 data32;

        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &data32));
        COMPILER_64_SET(*data, 0, data32);
    }

    return SOC_E_NONE;
}

/*
 * Read an internal SOC register through S-Channel messaging buffer. 
 *  
 * block is cmic block id 
 */

int
soc_direct_memreg_get(int unit, int cmic_block, uint32 addr, uint32 dwc_read, int is_mem, uint32 *data)
{
    schan_msg_t schan_msg;
    uint32 i;
    uint32 allow_intr = 0;
    int data_byte_len;
    int opcode, err;

    /*
     * Write message to S-Channel.
     */
    schan_msg_clear(&schan_msg);

    soc_schan_header_cmd_set(unit, &schan_msg.header, (is_mem? READ_MEMORY_CMD_MSG : READ_REGISTER_CMD_MSG),
                             cmic_block, SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit)),
                             0, dwc_read * 4, 0, 0);

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    if(soc_feature(unit, soc_feature_cmicm) || soc_feature(unit, soc_feature_cmicx)) {
        schan_msg.readcmd.address = addr;
    } else
#endif
    {
        uint32 cmice_addr = addr;
        if (cmic_block >= 0) {
            cmice_addr |= ((cmic_block & 0xf) << SOC_BLOCK_BP) | 
                            (((cmic_block >> 4) & 0x3) << SOC_BLOCK_MSB_BP);
        }
        
        schan_msg.readcmd.address = cmice_addr;
    }

    if (SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /* Write header word + address DWORD, read header word + data DWORD */
    SOC_IF_ERROR_RETURN(soc_schan_op(unit, &schan_msg, 2, dwc_read+1, allow_intr));

    /* Check result */
    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL,
                                &data_byte_len, &err, NULL, NULL);
    if (opcode != (is_mem ? READ_MEMORY_ACK_MSG : READ_REGISTER_ACK_MSG) || err != 0) {
        {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "soc_direct_memreg_get(): block:%d address:%u "
                      "invalid S-Channel reply, expected %s and found %d err=%d\n"),
                      cmic_block, (unsigned)addr, (is_mem ? "READ_MEM_ACK" : "READ_REG_ACK"), opcode, err));
            soc_schan_dump(unit, &schan_msg, 2);
            return SOC_E_INTERNAL;
        }
    }

    for(i = 0; i < data_byte_len / 4; i++) {
        data[i] = schan_msg.readresp.data[i];
    }

    return SOC_E_NONE;
}

int
soc_direct_reg_get(int unit, int cmic_block, uint32 addr, uint32 dwc_read, uint32 *data)
{
    return soc_direct_memreg_get(unit, cmic_block, addr, dwc_read, 0, data);
}


void soc_direct_mem_set_cache_update(int unit, int cmic_block, uint32 addr, uint32 *entry_data)
{
    int rc;
    soc_mem_t mem;
    int blk;
    int index;
    unsigned array_index;

    /* find the matching block by cmic_block */
    for (blk = 0; ; ++blk)
    {
        if (SOC_BLOCK_TYPE(unit, blk) < 0)
        {
            return;
        }
        else if (SOC_BLOCK_INFO(unit, blk).cmic == cmic_block)
        {
            break;
        }
    }

    mem = soc_addr_to_mem_extended(unit, cmic_block, 0xff, addr);
    if(mem == INVALIDm)
    {
        return;
    }
    SOC_MEM_ALIAS_TO_ORIG(unit,mem);
    rc = soc_mem_addr_to_array_element_and_index(unit, mem, addr, &array_index, &index);
    if (rc != SOC_E_NONE)
    {
        return;
    }

    _soc_mem_write_cache_update(unit, mem, blk, 0, index, (int)array_index, entry_data, NULL, NULL, NULL);

    return;
}

int
soc_direct_memreg_set(int unit, int cmic_block, uint32 addr, uint32 dwc_write, int is_mem, uint32 *data)
{
    schan_msg_t schan_msg;
    int i;
    uint32 allow_intr = 0;

    if (is_mem)
    {
        soc_direct_mem_set_cache_update(unit, cmic_block, addr, data);
    }
    /*
     * Setup S-Channel command packet
     *
     * NOTE: the datalen field matters only for the Write Memory and
     * Write Register commands, where it is used only by the CMIC to
     * determine how much data to send, and is in units of bytes.
     */

    schan_msg_clear(&schan_msg);

    soc_schan_header_cmd_set(unit, &schan_msg.header, (is_mem ? WRITE_MEMORY_CMD_MSG : WRITE_REGISTER_CMD_MSG),
                             cmic_block, SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit)),
                             0, dwc_write * 4, 0, 0);

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    if(soc_feature(unit, soc_feature_cmicm) || soc_feature(unit, soc_feature_cmicx)) {
        schan_msg.writecmd.address = addr;
    } else
#endif
    {
        uint32 cmice_addr = addr;
        if (cmic_block >= 0) {
            cmice_addr |= ((cmic_block & 0xf) << SOC_BLOCK_BP) |
                         (((cmic_block >> 4) & 0x3) << SOC_BLOCK_MSB_BP);
        }
        
        schan_msg.readcmd.address = cmice_addr;
    }
    
    for(i=0 ; i<dwc_write ; i++)
      schan_msg.writecmd.data[i] = data[i];


    if(SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /* Write header word + address + data DWORD */
    /* Note: The hardware does not send WRITE_REGISTER_ACK_MSG. */
    
    

    return soc_schan_op(unit, &schan_msg, dwc_write+2, 0, allow_intr);
}

int
soc_direct_reg_set(int unit, int cmic_block, uint32 addr, uint32 dwc_write, uint32 *data)
{
#ifdef CRASH_RECOVERY_SUPPORT
    /*     Use crash recovery defined callback for access*/
    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.direct_reg_set)
            {
                return Hw_Log_List[unit].Access_cb.direct_reg_set(unit, cmic_block, addr, dwc_write, data);
            }
        }
    }

#endif /* CRASH_RECOVERY_SUPPORT */
    return soc_direct_memreg_set(unit, cmic_block, addr, dwc_write, 0, data);
}

/*
 * Read an internal SOC register through S-Channel messaging buffer.
 * Use soc_reg32_get() if you know the port number, index
 */

int
_soc_reg32_get(int unit, soc_block_t block, int acc, uint32 addr, uint32 *data)
{
    schan_msg_t schan_msg;
    int rv;
    uint32 allow_intr = 0;
    int data_byte_len;
    int opcode, err;
#ifdef BCM_HELIX5_SUPPORT
    soc_info_t *si = &SOC_INFO(unit);
#endif

    /*
     * Write message to S-Channel.
     */
    schan_msg_clear(&schan_msg);

    data_byte_len = 4;
    soc_schan_header_cmd_set(unit, &schan_msg.header, READ_REGISTER_CMD_MSG,
                             block, 0, acc, data_byte_len, 0, 0);

    schan_msg.readcmd.address = addr;

    if(SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /* Write header word + address DWORD, read header word + data DWORD */
    rv = soc_schan_op(unit, &schan_msg, 2, 2, allow_intr);
    if (SOC_FAILURE(rv)) {
#if defined(BCM_XGS_SUPPORT)
        int rv1, port = 0, index;
#endif /* BCM_XGS_SUPPORT */
        soc_regaddrinfo_t ainfo;

        if (!soc_feature(unit, soc_feature_ser_parity)) {
            return rv;
        }
        soc_regaddrinfo_extended_get(unit, &ainfo, block, acc, addr);
        if (ainfo.reg != INVALIDr) {
            if (SOC_REG_IS_COUNTER(unit, ainfo.reg)) {
                *data = 0;
                /* Force correct */
                if (!SOC_REG_RETURN_SER_ERROR(unit)) {
                    rv = SOC_E_NONE;
                }
            } 
#if defined(BCM_XGS_SUPPORT)
            else if (soc_feature(unit, soc_feature_regs_as_mem)) {
                if (SOC_REG_INFO(unit, ainfo.reg).regtype == soc_portreg) {
                    port = ainfo.port;
                } else if (SOC_REG_INFO(unit, ainfo.reg).regtype == soc_cosreg) {
                    port = ainfo.cos;
                }
                index = ainfo.idx != -1 ? ainfo.idx : 0;
                rv1 = soc_ser_reg32_cache_get(unit, ainfo.reg, port, index, data);
                if (rv1 != SOC_E_NONE) {
                    if (SOC_REG_IS_DYNAMIC(unit, ainfo.reg)) {
                        *data = 0;
                    } else {
                        return rv;
                    }
                }
                /* Force correct */
                if (!SOC_REG_RETURN_SER_ERROR(unit)) {
                    rv = SOC_E_NONE;
                }
            }
#endif /* BCM_XGS_SUPPORT */
        } else {
            return rv;
        }
    }

    /* Check result */
    soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL, NULL,
                                &err, NULL, NULL);
    if (opcode != READ_REGISTER_ACK_MSG || err != 0) {
        {
            LOG_WARN(BSL_LS_SOC_SCHAN,
                (BSL_META_U(unit,
                    "soc_schan_op: operation failed: %s(%d)\n"), soc_errmsg(rv), rv));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "_soc_reg32_get: "
                              "invalid S-Channel reply, expected READ_REG_ACK(%d) and found %d err=%d\n"),
                              READ_REGISTER_ACK_MSG, opcode, err));
            soc_schan_dump(unit, &schan_msg, 2);
            return SOC_E_INTERNAL;
        }
    }

    *data = schan_msg.readresp.data[0];

#ifdef BCM_HELIX5_SUPPORT
    /* For HX5 A WAR is needed when reading 'CHIP_CONFIGr', since the fields
       PMD_PLL_CTRL_REFLK_DIV2f/PMD_PLL_CTRL_REFLK_DIV4f are swapped (Write
       into the register is fine) */
    if (si->hx5_chip_config_war_enable) {
        uint32 pll_ctrl_ref_clk_div2 = 0, pll_ctrl_ref_clk_div4 = 0;
        if (addr == si->hx5_chip_config_address) {
            pll_ctrl_ref_clk_div2 = soc_reg_field_get(unit, CHIP_CONFIGr,
                                    *data, PMD_PLL_CTRL_REFCLK_DIV2f);
            pll_ctrl_ref_clk_div4 = soc_reg_field_get(unit, CHIP_CONFIGr,
                                    *data, PMD_PLL_CTRL_REFCLK_DIV4f);
            soc_reg_field_set(unit, CHIP_CONFIGr, data, PMD_PLL_CTRL_REFCLK_DIV2f,
                                    pll_ctrl_ref_clk_div4);
            soc_reg_field_set(unit, CHIP_CONFIGr, data, PMD_PLL_CTRL_REFCLK_DIV4f,
                                    pll_ctrl_ref_clk_div2);
        }
    }
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BROADCOM_DEBUG
    if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
        _soc_reg_extended_debug(unit, 32, "read", block, acc, addr, 0, *data);
    }
#endif /* BROADCOM_DEBUG */
    _soc_snoop_reg(unit, block, acc, addr,SOC_REG_SNOOP_READ, 0,*data);

    return SOC_E_NONE;
}

/*
 * Read an internal SOC register through S-Channel messaging buffer.
 * Checks if the register is 32 or 64 bits.
 */

int
soc_reg_get(int unit, soc_reg_t reg, int port, int index, uint64 *data)
{
    uint32 addr;
    int block;
    int pindex = port; 
    int rv = SOC_E_NONE;
    uint8 acc_type = 0;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_REG, reg, FALSE));

#ifdef CRASH_RECOVERY_SUPPORT
    /*     Use crash recovery defined callback for access*/
    if (SOC_IS_DONE_INIT(unit) && BCM_UNIT_DO_HW_READ_WRITE(unit) && Hw_Log_List[unit].Access_cb.soc_reg_get) {
        rv = Hw_Log_List[unit].Access_cb.soc_reg_get(unit, reg, port, index, data);
    } else
#endif /* CRASH_RECOVERY_SUPPORT */
   /* Use user defined callback for access */
    if (SOC_INFO(unit).reg_access.reg64_get) {
        rv = SOC_INFO(unit).reg_access.reg64_get(unit, reg, port, index, data);
    } else

    if (SOC_REG_IS_ABOVE_64(unit, reg)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_reg_get: "
                              "Use soc_reg_above_64_get \n")));
        
        return SOC_E_FAIL;
    } else

#ifdef CANCUN_SUPPORT
    if (SOC_REG_IS_CCH(unit, reg) && !(soc_property_get(unit,
                              "skip_cancun_cch_reg_check", 0) ? TRUE : FALSE) &&
        SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
             rv = soc_cancun_pseudo_reg_get(unit, reg, data);
    } else
#endif

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        rv = soc_dnxtestchip_reg_get(unit, reg, port, data);
    } else
#endif /* DNX_TEST_CHIPS_SUPPORT */
    {
        addr = soc_reg_addr_get(unit, reg, port, index, SOC_REG_ADDR_OPTION_NONE, &block, &acc_type);
        if (SOC_REG_IS_64(unit, reg)) {
            soc_port_t _port;
            soc_block_types_t regblktype = SOC_REG_INFO(unit, reg).block;
            int blk, bindex;
            if (!soc_feature(unit, soc_feature_new_sbus_format)) {
                rv = soc_reg_read(unit, reg, addr, data);
            }

            else if (SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_PORT) 
#ifdef BCM_SAND_SUPPORT
               && !SOC_IS_SAND(unit)
#endif /* BCM_SAND_SUPPORT */
#ifdef BCM_BIGMAC_SUPPORT
               && iterative_op_required(reg)
#endif /* BCM_BIGMAC_SUPPORT */
                ) {
                PBMP_HYPLITE_ITER(unit, _port) {
                    blk = SOC_PORT_BLOCK(unit, _port);
                    bindex = SOC_PORT_BINDEX(unit, _port);
                    if ((SOC_BLOCK2SCH(unit, blk) == block) && (bindex == pindex)) {
                        break;
                    }
                }         
                if (!IS_HYPLITE_PORT(unit, port)) {
                    rv = _soc_reg64_get(unit, block, acc_type, addr, data);
                }  
#ifdef BCM_BIGMAC_SUPPORT
                else
                {   
                    rv = soc_reg64_get_iterative(unit, block, acc_type, addr, port, data);
                }
#endif            
            } else {
                rv = _soc_reg64_get(unit, block, acc_type, addr, data);
            }
        } else {
            uint32 data32;
            if (!soc_feature(unit, soc_feature_new_sbus_format)) {
                rv = soc_reg32_read(unit, addr, &data32);
            } else {
                rv = _soc_reg32_get(unit, block, acc_type, addr, &data32);
            }
            if (rv == SOC_E_NONE) {
                COMPILER_64_SET(*data, 0, data32);
            }
        }
    }

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
          "soc_reg_get failed for %s\n"), SOC_REG_NAME(unit, reg)));
    }
    return rv;
}

/*
 * Read an internal SOC register through S-Channel messaging buffer.
 * Handle register at any size 
 */
 
int
soc_reg_above_64_get(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    uint32 addr;
    int block;
    uint8 at; 
    uint64 data64;
    int rc;
    int reg_size;
    
    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }
    
    SOC_REG_ABOVE_64_CLEAR(data);
    
#ifdef CRASH_RECOVERY_SUPPORT

    /*     Use crash recovery defined callback for access*/
    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.reg_above64_get)
            {
                return Hw_Log_List[unit].Access_cb.reg_above64_get(unit, reg, port, index, data);
            }
        }
    }

#endif /* CRASH_RECOVERY_SUPPORT */

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg_above_64_get(unit, reg, port, data);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */

    /* Use user defined callback for access */
    if(SOC_INFO(unit).reg_access.reg_above64_get) {
        return SOC_INFO(unit).reg_access.reg_above64_get(unit, reg, port, index, data);
    }

    if (SOC_REG_IS_ABOVE_64(unit, reg)) 
    {
        reg_size = SOC_REG_ABOVE_64_INFO(unit, reg).size;
        addr = soc_reg_addr_get(unit, reg, port, index,
                                SOC_REG_ADDR_OPTION_NONE, &block, &at);
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            block = ((addr >> SOC_BLOCK_BP) & 0xf) | (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
        }
        rc = soc_direct_reg_get(unit, block, addr, reg_size, data);

#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_above_64_debug(unit, "get", block, addr, data);
        }
#endif /* BROADCOM_DEBUG */

        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
              "soc_reg_above_64_get failed for %s\n"), SOC_REG_NAME(unit, reg)));
        }
        return rc;

    } 
    else if (SOC_REG_IS_64(unit, reg)) {
        COMPILER_64_SET(data64, data[1], data[0]);
        rc = soc_reg_get(unit, reg, port, index, &data64);
        data[0] = COMPILER_64_LO(data64);
        data[1] = COMPILER_64_HI(data64);
        return rc;
    } 
    else {
        rc = soc_reg_get(unit, reg, port, index, &data64);
        data[0] = COMPILER_64_LO(data64);
        return rc;
    }
}

/*
 * Read an internal SOC register through S-Channel messaging buffer.
 * Uses sbusv1 format, before CMICm
 * SHOULD not be called from other files, use soc_reg32_get() instead
 */

int
soc_reg32_read(int unit,
               uint32 addr,
               uint32 *data)
{
    schan_msg_t schan_msg;
    int rv;
    uint32 allow_intr = 0;
    int dst_blk, src_blk, data_byte_len;
    int opcode, err;

#ifdef BCM_CMICM_SUPPORT
    uint32 fsdata = 0;
    int cmc = SOC_PCI_CMC(unit);
#endif

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg32_read(unit, addr, data);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */

#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm) &&
        (NULL != SOC_CONTROL(unit)->fschanMutex)) {
        FSCHAN_LOCK(unit);
        soc_pci_write(unit, CMIC_CMCx_FSCHAN_ADDRESS_OFFSET(cmc), addr);
        fsdata = soc_pci_read(unit, CMIC_CMCx_FSCHAN_DATA32_OFFSET(cmc));
        FSCHAN_UNLOCK(unit);
        *data = fsdata;
    } else
#endif
    {
        /*
         * Write message to S-Channel.
         */
        schan_msg_clear(&schan_msg);

        dst_blk = ((addr >> SOC_BLOCK_BP) & 0xf) | 
            (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
        {
            src_blk = SOC_IS_SHADOW(unit) ?
                0 : SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
            data_byte_len = SOC_IS_XGS12_FABRIC(unit) ? 8 : 4;
        }
        soc_schan_header_cmd_set(unit, &schan_msg.header,
                                 READ_REGISTER_CMD_MSG, dst_blk, src_blk, 0,
                                 data_byte_len, 0, 0);

        schan_msg.readcmd.address = addr;

        if(SOC_IS_SAND(unit)) {
            allow_intr = 1;
        }

        /* Write header word + address DWORD, read header word + data DWORD */
        rv = soc_schan_op(unit, &schan_msg, 2, 2, allow_intr);
        if (SOC_FAILURE(rv)) {
#if defined(BCM_XGS_SUPPORT)
            int rv1, port = 0, index;
#endif /* BCM_XGS_SUPPORT */
            soc_regaddrinfo_t ainfo;

            if (!soc_feature(unit, soc_feature_ser_parity)) {
                return rv;
            }
            soc_regaddrinfo_get(unit, &ainfo, addr);            
            if (ainfo.reg != INVALIDr) {
                if (SOC_REG_IS_COUNTER(unit, ainfo.reg)) {
                    *data = 0;
                    /* Force correct */
                    if (!SOC_REG_RETURN_SER_ERROR(unit)) {
                        rv = SOC_E_NONE;
                    }
                } 
#if defined(BCM_XGS_SUPPORT)
                else if (soc_feature(unit, soc_feature_regs_as_mem)) {
                    if (SOC_REG_INFO(unit, ainfo.reg).regtype == soc_portreg) {
                        port = ainfo.port;
                    } else if (SOC_REG_INFO(unit, ainfo.reg).regtype == soc_cosreg) {
                        port = ainfo.cos;
                    }
                    index = ainfo.idx != -1 ? ainfo.idx : 0;
                    rv1 = soc_ser_reg32_cache_get(unit, ainfo.reg, port, index, data);
                    if (rv1 != SOC_E_NONE) {
                        if (SOC_REG_IS_DYNAMIC(unit, ainfo.reg)) {
                            *data = 0;
                        } else {
                            return rv;
                        }
                    }
                    /* Force correct */
                    if (!SOC_REG_RETURN_SER_ERROR(unit)) {
                        rv = SOC_E_NONE;
                    }
                }
#endif /* BCM_XGS_SUPPORT */
            } else {
                return rv;
            }
        }

        /* Check result */
        soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL,
                                    NULL, &err, NULL, NULL);
        if (!SOC_FAILURE(rv) && 
            (opcode != READ_REGISTER_ACK_MSG || err != 0)) {
            LOG_WARN(BSL_LS_SOC_SCHAN,
                (BSL_META_U(unit,
                    "soc_schan_op: operation failed: %s(%d)\n"), soc_errmsg(rv), rv));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_reg32_read: "
                                  "invalid S-Channel reply, expected READ_REG_ACK:\n")));
            soc_schan_dump(unit, &schan_msg, 2);
            return SOC_E_INTERNAL;
        }
        *data = schan_msg.readresp.data[0];
    }
#ifdef BROADCOM_DEBUG
    if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
        _soc_reg_debug(unit, 32, "read", addr, 0, *data);
    }
#endif /* BROADCOM_DEBUG */
    _soc_snoop_reg(unit, 0, 0, addr,SOC_REG_SNOOP_READ, 0, *data);
    return SOC_E_NONE;
}

/*
 * Read an internal SOC register through S-Channel messaging buffer.
 */

int
soc_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32 *data)
{
    uint32 addr;
    int block = 0;
    uint8 acc_type = 0;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_REG, reg, FALSE));

    if (SOC_REG_IS_ABOVE_32(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s is > 32 bit, "
                                    "but called with soc_reg32_get\n"),
                   soc_reg_name[reg]));
#endif
    }
    assert(!SOC_REG_IS_ABOVE_32(unit, reg));

    /*     Use crash recovery defined callback for access*/
#ifdef CRASH_RECOVERY_SUPPORT
    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.reg32_get)
            {
                return Hw_Log_List[unit].Access_cb.reg32_get(unit, reg, port, index, data);
            }
        }
    }

#endif /* CRASH_RECOVERY_SUPPORT */

    /* Use user defined callback for access */
    if(SOC_INFO(unit).reg_access.reg32_get) {
        return SOC_INFO(unit).reg_access.reg32_get(unit, reg, port, index, data);
    }

#ifdef CANCUN_SUPPORT
    if (SOC_REG_IS_CCH(unit, reg) && !(soc_property_get(unit,
                              "skip_cancun_cch_reg_check", 0) ? TRUE : FALSE)) {
        uint64 rval64;
        int rv;

        if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
            rv = soc_cancun_pseudo_reg_get(unit, reg, &rval64);
            if(rv == SOC_E_NONE) {
                *data = COMPILER_64_LO(rval64);
            }
            return rv;
        }
    }
#endif

    addr = soc_reg_addr_get(unit, reg, port, index,
                            SOC_REG_ADDR_OPTION_NONE, &block, &acc_type);

    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
        return soc_reg32_read(unit, addr, data);
    }
    return _soc_reg32_get(unit, block, acc_type, addr, data);
}

/*
 * Read an internal 64-bit SOC register through S-Channel messaging buffer.
 */
int
soc_reg64_read(int unit,
               uint32 addr,
               uint64 *reg)
{
    schan_msg_t schan_msg;
    int rv;
    uint32 allow_intr = 0;
    int dst_blk, src_blk, data_byte_len;
    int opcode, err;

#ifdef BCM_CMICM_SUPPORT
    uint32 fsdatal = 0, fsdatah = 0;
    int cmc = SOC_PCI_CMC(unit);
#endif

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg64_read(unit, addr, reg);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */

#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm) &&
        (NULL != SOC_CONTROL(unit)->fschanMutex)) {
        FSCHAN_LOCK(unit);
        soc_pci_write(unit, CMIC_CMCx_FSCHAN_ADDRESS_OFFSET(cmc), addr);
        fsdatal = soc_pci_read(unit, CMIC_CMCx_FSCHAN_DATA64_LO_OFFSET(cmc));
        fsdatah = soc_pci_read(unit, CMIC_CMCx_FSCHAN_DATA64_HI_OFFSET(cmc));
        FSCHAN_UNLOCK(unit);
        COMPILER_64_SET(*reg, fsdatah, fsdatal);
    } else
#endif
    {
        /*
         * Write message to S-Channel.
         */
        schan_msg_clear(&schan_msg);

        dst_blk = ((addr >> SOC_BLOCK_BP) & 0xf) | 
            (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
        {
            src_blk = SOC_IS_SHADOW(unit) ?
                0 : SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
            data_byte_len = 8;
        }
        soc_schan_header_cmd_set(unit, &schan_msg.header,
                                 READ_REGISTER_CMD_MSG, dst_blk, src_blk, 0,
                                 data_byte_len, 0, 0);

        schan_msg.readcmd.address = addr;

        if(SOC_IS_SAND(unit)) {
            allow_intr = 1;
        }

        /* Write header word + address DWORD, read header word + data DWORD */
        rv = soc_schan_op(unit, &schan_msg, 2, 3, allow_intr);
        if (SOC_FAILURE(rv)) {
#if defined(BCM_XGS_SUPPORT)
            int rv1, port = 0, index;
#endif /* BCM_XGS_SUPPORT */
            soc_regaddrinfo_t ainfo;

            if (!soc_feature(unit, soc_feature_ser_parity)) {
                return rv;
            }    
            soc_regaddrinfo_get(unit, &ainfo, addr);            
            if (ainfo.reg != INVALIDr) {
                if (SOC_REG_IS_COUNTER(unit, ainfo.reg)) {
                    COMPILER_64_SET(*reg, 0, 0);
                    /* Force correct */
                    if (!SOC_REG_RETURN_SER_ERROR(unit)) {
                        rv = SOC_E_NONE;
                    }
                } 
#if defined(BCM_XGS_SUPPORT)
                else if (soc_feature(unit, soc_feature_regs_as_mem)) {
                    if (SOC_REG_INFO(unit, ainfo.reg).regtype == soc_portreg) {
                        port = ainfo.port;
                    } else if (SOC_REG_INFO(unit, ainfo.reg).regtype == soc_cosreg) {
                        port = ainfo.cos;
                    }
                    index = ainfo.idx != -1 ? ainfo.idx : 0;
                    rv1 = soc_ser_reg_cache_get(unit, ainfo.reg, port, index, reg);
                    if (rv1 != SOC_E_NONE) {
                        if (SOC_REG_IS_DYNAMIC(unit, ainfo.reg)) {
                            COMPILER_64_SET(*reg, 0, 0);
                        } else {
                            return rv;
                        }
                    }
                    /* Force correct */
                    if (!SOC_REG_RETURN_SER_ERROR(unit)) {
                        rv = SOC_E_NONE;
                    }
                }
#endif /* BCM_XGS_SUPPORT */
            } else {
                return rv;
            }
        }

        /* Check result */
        soc_schan_header_status_get(unit, &schan_msg.header, &opcode, NULL,
                                    NULL, &err, NULL, NULL);
        if (opcode != READ_REGISTER_ACK_MSG || err != 0) {
            LOG_WARN(BSL_LS_SOC_SCHAN,
                (BSL_META_U(unit,
                    "soc_schan_op: operation failed: %s(%d)\n"), soc_errmsg(rv), rv));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_reg64_read: "
                                  "invalid S-Channel reply, expected READ_REG_ACK:\n")));
            soc_schan_dump(unit, &schan_msg, 2);
            return SOC_E_INTERNAL;
        }
        COMPILER_64_SET(*reg, schan_msg.readresp.data[1],
                        schan_msg.readresp.data[0]);
    }

#ifdef BROADCOM_DEBUG
    if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
        _soc_reg_debug(unit, 64, "read", addr,
                       schan_msg.readresp.data[1],
                       schan_msg.readresp.data[0]);
    }
#endif /* BROADCOM_DEBUG */
    _soc_snoop_reg(unit, 0, 0, addr,SOC_REG_SNOOP_READ, 
                   schan_msg.readresp.data[1], schan_msg.readresp.data[0]);

    return SOC_E_NONE;
}

/*
 * Read an internal 64-bit SOC register through S-Channel messaging buffer.
 */
int
soc_reg64_get(int unit, soc_reg_t reg, int port, int index, uint64 *data)
{
    uint32 addr;
    int block = 0;
    uint8 acc_type = 0;

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_REG, reg, FALSE));

#ifdef CANCUN_SUPPORT
    if (SOC_REG_IS_CCH(unit, reg) && !(soc_property_get(unit,
                              "skip_cancun_cch_reg_check", 0) ? TRUE : FALSE)) {
        if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
            return(soc_cancun_pseudo_reg_get(unit, reg, data));
        }
    }
#endif

#ifdef CRASH_RECOVERY_SUPPORT
    /*     Use crash recovery defined callback for access*/
    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.reg64_get)
            {
                return Hw_Log_List[unit].Access_cb.reg64_get(unit, reg, port, index, data);
            }
        }
    }
#endif /* CRASH_RECOVERY_SUPPORT */

    /* Use user defined callback for access */
    if(SOC_INFO(unit).reg_access.reg64_get) {
        return SOC_INFO(unit).reg_access.reg64_get(unit, reg, port, index, data);
    }

    addr = soc_reg_addr_get(unit, reg, port, index,
                            SOC_REG_ADDR_OPTION_NONE, &block, &acc_type);
    assert(SOC_REG_IS_64(unit, reg));
    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
        return soc_reg64_read(unit, addr, data);
    }
    return _soc_reg64_get(unit, block, acc_type, addr, data);
}

/*
 * Read an internal SOC register through S-Channel messaging buffer
 * with Raw Port Number.
 */
int
soc_reg_rawport_get(int unit, soc_reg_t reg, int port, int index, uint64 *data)
{
    uint32 addr;
    int block = 0;
    uint8 acc_type;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    if ((REG_PORT_ANY != port) &&
        (port & (SOC_REG_ADDR_INSTANCE_MASK | SOC_REG_ADDR_BLOCK_ID_MASK | 
                SOC_REG_ADDR_SCHAN_ID_MASK | SOC_REG_ADDR_PHY_ACC_MASK))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "This function is only for Raw Port Numbers \n")));
        return SOC_E_FAIL;
    }
#ifdef CRASH_RECOVERY_SUPPORT
    /* Don't handle Special Accesses */
    if((SOC_INFO(unit).reg_access.reg64_get) || /* User defined */
       (SOC_REG_IS_ABOVE_64(unit, reg)) ||
       (SOC_IS_DONE_INIT(unit) && BCM_UNIT_DO_HW_READ_WRITE(unit) &&
        Hw_Log_List[unit].Access_cb.soc_reg_get)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Use soc_reg_get \n")));
        return SOC_E_FAIL;
    }
#else
    if((SOC_INFO(unit).reg_access.reg64_get) || /* User defined */
       (SOC_REG_IS_ABOVE_64(unit, reg))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Use soc_reg_get \n")));
        return SOC_E_FAIL;
    }
#endif /* CRASH_RECOVERY_SUPPORT */

    addr = soc_reg_addr_get(unit, reg, port, index,
                            SOC_REG_ADDR_OPTION_PRESERVE_PORT,
                            &block, &acc_type);

    if (SOC_REG_IS_64(unit, reg)) {
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            return soc_reg_read(unit, reg, addr, data);
        } else {
            return _soc_reg64_get(unit, block, acc_type, addr, data);
        }
    } else {
        uint32 data32;
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &data32));
        } else {
            SOC_IF_ERROR_RETURN
                (_soc_reg32_get(unit, block, acc_type, addr, &data32));
        }
        COMPILER_64_SET(*data, 0, data32);
    }
    return SOC_E_NONE;
}

int
soc_reg32_rawport_get(int unit, soc_reg_t reg, int port, int index, uint32 *data)
{
    int rv;
    uint64 d64;

    rv = soc_reg_rawport_get(unit, reg, port, index, &d64);
    *data = COMPILER_64_LO(d64);
    return rv;
}

#ifdef BCM_BIGMAC_SUPPORT
/*
 * Iterative write procedure for MAC registers on Hyperlite ports.
 */
STATIC int
soc_reg64_write_iterative(int unit, uint32 addr, soc_port_t port,
                          uint64 data)
{
    int rv, i, diff;
    uint64 xgxs_stat;
    uint32 locked;
    sal_usecs_t t1 = 0, t2;
    soc_timeout_t to;
    for (i = 0; i < 100; i++) {
       /* Read PLL lock status */
       soc_timeout_init(&to, 25 * MILLISECOND_USEC, 0);
       do {
           t1 = sal_time_usecs();
           rv = READ_MAC_XGXS_STATr(unit, port, &xgxs_stat);
           locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                          TXPLL_LOCKf);
           if (locked || SOC_FAILURE(rv)) {
               break;
           }
       } while (!soc_timeout_check(&to));
       if (SOC_FAILURE(rv)) {
           return rv;
       }
       if (!locked) {
           continue;
       }
       /* Write register value */
       SOC_IF_ERROR_RETURN(soc_reg64_write(unit, addr, data));
       /* Read PLL lock status */
       SOC_IF_ERROR_RETURN(READ_MAC_XGXS_STATr(unit, port, &xgxs_stat));
       locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                      TXPLL_LOCKf);
       t2 = sal_time_usecs();
       diff = SAL_USECS_SUB(t2, t1);
       if (locked && (diff < 20 * MILLISECOND_USEC)) {
           return SOC_E_NONE;
       }
       LOG_VERBOSE(BSL_LS_SOC_COMMON,
                   (BSL_META_U(unit,
                               "soc_reg64_write_iterative: WARNING: "
                               "iteration %d PLL went out of lock"),
                    i));
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_reg64_write_iterative: "
                          "operation failed:\n"))); 
    return SOC_E_FAIL;    
}

#endif /* BCM_BIGMAC_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
/*
 * Write an internal 64-bit TH3 SOC register through S-Channel messaging buffer.
 */
int
_soc_th3_reg64_set(int unit, soc_block_t block, int acc, uint32 addr, uint64 data)
{
    schan_msg_t schan_msg;
    uint32 allow_intr = 0;

    /*
     * Setup S-Channel command packet
     *
     * NOTE: the datalen field matters only for the Write Memory and
     * Write Register commands, where it is used only by the CMIC to
     * determine how much data to send, and is in units of bytes.
     */

    schan_msg_clear(&schan_msg);

    soc_schan_header_cmd_set(unit, &schan_msg.header, WRITE_REGISTER_CMD_MSG,
                             block, 0, acc, 8, 0, 0);

    schan_msg.writecmd.address = addr;
    schan_msg.writecmd.data[0] = COMPILER_64_LO(data);
    schan_msg.writecmd.data[1] = COMPILER_64_HI(data);

#ifdef BROADCOM_DEBUG
    if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
        _soc_reg_extended_debug(unit, 64, "write", block, acc, addr,
                                schan_msg.writecmd.data[1],
                                schan_msg.writecmd.data[0]);
    }
#endif /* BROADCOM_DEBUG */
    _soc_snoop_reg(unit, block, acc, addr,SOC_REG_SNOOP_WRITE, 
                   schan_msg.writecmd.data[1],schan_msg.writecmd.data[0]);

    if(SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /* Write header word + address + 2*data DWORD */
    /* Note: The hardware does not send WRITE_REGISTER_ACK_MSG. */
    
    

    return soc_schan_op(unit, &schan_msg, 4, 0, allow_intr);
    

}
#endif

/*
 * Write an internal 64-bit SOC register through S-Channel messaging buffer.
 */
int
_soc_reg64_set(int unit, soc_block_t block, int acc, uint32 addr, uint64 data)
{
    schan_msg_t schan_msg;
    int rv = 0;
    uint32 allow_intr = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8 rev_id;
    int j;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    if ((acc == 9 || acc == 14) && (dev_id == BCM56983_DEVICE_ID)) {

        for (j = 0; j< 4; j++) {
            switch(j) {
               case 0 : acc = 0;
                        break;
               case 1 : acc = 1;
                        break;
               case 2 : acc = 6;
                        break;
               case 3 : acc = 7;
                        break;
            }
            rv = _soc_th3_reg64_set(unit, block, acc, addr, data);
            if ( rv < 0 ) {
                return rv;
            }
        }

    } else if (acc == 15 && (dev_id == BCM56983_DEVICE_ID)) {
        for (j = 0; j < 2; j++) {
            switch(j) {
                case 0 : acc = 0;
                         break;
                case 1 : acc = 6;
                         break;
            }
            rv = _soc_th3_reg64_set(unit, block, acc, addr, data);
            if ( rv < 0 ) {
                return rv;
            }
        }
    } else if (acc == 16 && (dev_id == BCM56983_DEVICE_ID)) {
        acc = 0;
        rv = _soc_th3_reg64_set(unit, block, acc, addr, data);
        if ( rv < 0 ) {
            return rv;
        }
    } else
#endif
    {
        /*
         * Setup S-Channel command packet
         *
         * NOTE: the datalen field matters only for the Write Memory and
         * Write Register commands, where it is used only by the CMIC to
         * determine how much data to send, and is in units of bytes.
         */
    
        schan_msg_clear(&schan_msg);
    
        soc_schan_header_cmd_set(unit, &schan_msg.header, WRITE_REGISTER_CMD_MSG,
                                 block, 0, acc, 8, 0, 0);
    
        schan_msg.writecmd.address = addr;
        schan_msg.writecmd.data[0] = COMPILER_64_LO(data);
        schan_msg.writecmd.data[1] = COMPILER_64_HI(data);
    
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_extended_debug(unit, 64, "write", block, acc, addr,
                                    schan_msg.writecmd.data[1],
                                    schan_msg.writecmd.data[0]);
        }
#endif /* BROADCOM_DEBUG */
        _soc_snoop_reg(unit, block, acc, addr,SOC_REG_SNOOP_WRITE, 
                       schan_msg.writecmd.data[1],schan_msg.writecmd.data[0]);
    
        if(SOC_IS_SAND(unit)) {
            allow_intr = 1;
        }
    
        /* Write header word + address + 2*data DWORD */
        /* Note: The hardware does not send WRITE_REGISTER_ACK_MSG. */
        
        
    
        rv =  soc_schan_op(unit, &schan_msg, 4, 0, allow_intr);
        if (SOC_FAILURE(rv)) {
            LOG_WARN(BSL_LS_SOC_SCHAN,
                (BSL_META_U(unit,
                    "soc_schan_op: operation failed: %s(%d)\n"), soc_errmsg(rv), rv));
        }
    }
    return rv;
}

void
soc_reg_watch_set(int unit, int value)
{
    SOC_CONTROL(unit)->soc_reg_watch = value;
    LOG_CLI((BSL_META("regwatch delta %s\n"),
             value ? "on" : "off"));
}

#ifdef BCM_BIGMAC_SUPPORT

/*
 * Iterative write procedure for MAC registers on Hyperlite ports.
 */
STATIC int
soc_reg64_set_iterative(int unit, soc_block_t block, int acc, uint32 addr,
                        soc_port_t port, uint64 data)
{
    int rv, i, diff;
    uint64 xgxs_stat;
    uint32 locked;
    sal_usecs_t t1 = 0, t2;
    soc_timeout_t to;
    for (i = 0; i < 100; i++) {
       /* Read PLL lock status */
       soc_timeout_init(&to, 25 * MILLISECOND_USEC, 0);
       do {
           t1 = sal_time_usecs();
           rv = READ_MAC_XGXS_STATr(unit, port, &xgxs_stat);
           locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                          TXPLL_LOCKf);
           if (locked || SOC_FAILURE(rv)) {
               break;
           }
       } while (!soc_timeout_check(&to));
       if (SOC_FAILURE(rv)) {
           return rv;
       }
       if (!locked) {
           continue;
       }
       /* Write register value */
       SOC_IF_ERROR_RETURN(_soc_reg64_set(unit, block, acc, addr, data));
       /* Read PLL lock status */
       SOC_IF_ERROR_RETURN(READ_MAC_XGXS_STATr(unit, port, &xgxs_stat));
       locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat, 
                                      TXPLL_LOCKf);
       t2 = sal_time_usecs();
       diff = SAL_USECS_SUB(t2, t1);
       if (locked && (diff < 20 * MILLISECOND_USEC)) {
           return SOC_E_NONE;
       }
       LOG_VERBOSE(BSL_LS_SOC_COMMON,
                   (BSL_META_U(unit,
                               "soc_reg64_set_iterative: WARNING: "
                               "iteration %d PLL went out of lock"),
                    i));
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_reg64_set_iterative: "
                          "operation failed:\n"))); 
    return SOC_E_FAIL;    
}

#endif /* BCM_BIGMAC_SUPPORT */

/*
 * Write an internal SOC register through S-Channel messaging buffer.
 * Checks if the register is 32 or 64 bits.
 */

int
soc_reg_write(int unit, soc_reg_t reg, uint32 addr, uint64 data)
{

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_REG, reg, TRUE));

    if (SOC_REG_IS_ABOVE_64(unit, reg)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_reg_write: "
                              "Use soc_reg_above_64_set \n")));
        
        return SOC_E_FAIL;
    }

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg_write(unit, reg, addr, data);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */

    if (SOC_REG_IS_64(unit, reg)) {
        soc_port_t port;
        soc_block_types_t regblktype = SOC_REG_INFO(unit, reg).block;
        int blk, pindex, bindex, block;
        pindex = (addr >> SOC_REGIDX_BP) & 0x3f;
        block = ((addr >> SOC_BLOCK_BP) & 0xf) |
                (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
        if (SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_PORT) 
#ifdef BCM_SAND_SUPPORT
           && !SOC_IS_SAND(unit)
#endif /* BCM_SAND_SUPPORT */
#ifdef BCM_BIGMAC_SUPPORT
           && iterative_op_required(reg)
#endif /* BCM_BIGMAC_SUPPORT */
            ) {
            PBMP_HYPLITE_ITER(unit, port) {
                blk = SOC_PORT_BLOCK(unit, port);
                bindex = SOC_PORT_BINDEX(unit, port);
                if ((SOC_BLOCK2SCH(unit, blk) == block) && (bindex == pindex)) {
                    break;
                }
            }         
            if (!IS_HYPLITE_PORT(unit, port)) {
                return soc_reg64_write(unit, addr, data);
            } 
#ifdef BCM_BIGMAC_SUPPORT
            else {   
                return soc_reg64_write_iterative(unit, addr, port, data);
            }
#endif /* BCM_BIGMAC_SUPPORT */           
        } else {
            return soc_reg64_write(unit, addr, data);
        }
    } else {
        if (COMPILER_64_HI(data)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "soc_reg_write: WARNING: "
                                 "write to 32-bit reg %s with hi order data, 0x%x\n"),
                      SOC_REG_NAME(unit, reg),
                      COMPILER_64_HI(data)));
        }
        SOC_IF_ERROR_RETURN(soc_reg32_write(unit, addr,
                                            COMPILER_64_LO(data)));
    }

    return SOC_E_NONE;
}

/*
 * Write an internal SOC register through S-Channel messaging buffer.
 */
int
_soc_reg32_set(int unit, soc_block_t block, int acc, uint32 addr, uint32 data)
{
    schan_msg_t schan_msg;
    uint32 allow_intr=0;

    /*
     * Setup S-Channel command packet
     *
     * NOTE: the datalen field matters only for the Write Memory and
     * Write Register commands, where it is used only by the CMIC to
     * determine how much data to send, and is in units of bytes.
     */

    schan_msg_clear(&schan_msg);

    soc_schan_header_cmd_set(unit, &schan_msg.header, WRITE_REGISTER_CMD_MSG,
                             block, 0, acc, 4, 0, 0);

    schan_msg.writecmd.address = addr;
    schan_msg.writecmd.data[0] = data;

#ifdef BROADCOM_DEBUG
    if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
        _soc_reg_extended_debug(unit, 32, "write", block, acc, addr, 0, data);
    }
#endif /* BROADCOM_DEBUG */
    _soc_snoop_reg(unit, block, acc, addr,SOC_REG_SNOOP_WRITE, 0,data);

    if(SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /* Write header word + address + data DWORD */
    /* Note: The hardware does not send WRITE_REGISTER_ACK_MSG. */
    
    

    return soc_schan_op(unit, &schan_msg, 3, 0, allow_intr);
}

/*
 * Write an internal SOC register through S-Channel messaging buffer.
 * Checks if the register is 32 or 64 bits.
 */

int
soc_reg_set(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    uint32 addr;
    int block;
    soc_reg_access_info_t access_info;
    int rv;
    int pindex = port;
    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_REG, reg, TRUE));

    if (SOC_REG_IS_ABOVE_64(unit, reg)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_reg_set: "
                              "Use soc_reg_above_64_set \n")));
        
        return SOC_E_FAIL;
    }

    /* if reloading, don't write to register */
    if (SOC_IS_RELOADING(unit))
    {
        return SOC_E_NONE;
    }

#ifdef CRASH_RECOVERY_SUPPORT

    /*     Use crash recovery defined callback for access*/
    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.soc_reg_set)
            {
                return Hw_Log_List[unit].Access_cb.soc_reg_set(unit, reg, port, index, data);
            }
        }
    }

#endif /* CRASH_RECOVERY_SUPPORT */    

    /* Use user defined callback for access */
    if(SOC_INFO(unit).reg_access.reg64_set) {
        return SOC_INFO(unit).reg_access.reg64_set(unit, reg, port, index, data);
    }

#ifdef CANCUN_SUPPORT
    if (SOC_REG_IS_CCH(unit, reg) && !(soc_property_get(unit,
                              "skip_cancun_cch_reg_check", 0) ? TRUE : FALSE)) {
        soc_cancun_cch_reg_set(unit, reg, index, data);

        if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
            return(soc_cancun_pseudo_reg_set(unit, reg, data));
        }
    }
#endif

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg_set(unit, reg, port, data);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */
    rv = soc_reg_xaddr_get(unit, reg, port, index,
      SOC_REG_ADDR_OPTION_WRITE, &access_info);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
          "soc_reg_set: failed to get register address")));
        return rv;
    }
    addr = access_info.offset;
    block = access_info.blk_list[0];
    if (SOC_REG_IS_64(unit, reg)) {
        soc_port_t _port;
        soc_block_types_t regblktype = SOC_REG_INFO(unit, reg).block;
        int blk, bindex;

#if defined(BCM_XGS_SUPPORT)
        if (soc_feature(unit, soc_feature_regs_as_mem)) {
            (void)soc_ser_reg_cache_set(unit, reg, port, index, data);
        }
#endif /* BCM_XGS_SUPPORT */
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
           return soc_reg_write(unit, reg, addr, data);
        }
        if (SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_PORT) 
#ifdef BCM_SAND_SUPPORT
           && !SOC_IS_SAND(unit)
#endif /* BCM_SAND_SUPPORT */
#ifdef BCM_BIGMAC_SUPPORT
           && iterative_op_required(reg)
#endif /* BCM_BIGMAC_SUPPORT */
            ) {
            PBMP_HYPLITE_ITER(unit, _port) {
                blk = SOC_PORT_BLOCK(unit, _port);
                bindex = SOC_PORT_BINDEX(unit, _port);
                if ((SOC_BLOCK2SCH(unit, blk) == block) && (bindex == pindex)) {
                    break;
                }
            }         
            if (!IS_HYPLITE_PORT(unit, port)) {
                return _soc_reg64_set(unit, block, access_info.acc_type, addr, data);
            } 
#ifdef BCM_BIGMAC_SUPPORT
            else {   
                return soc_reg64_set_iterative(unit, block, access_info.acc_type, addr,
                                               port, data);
            }
#endif /* BCM_BIGMAC_SUPPORT */           
        } else {
            for (block = 0; block < access_info.num_blks && rv == SOC_E_NONE; ++block) {
                rv = _soc_reg64_set(unit, access_info.blk_list[block], access_info.acc_type, addr, data);
            }
        }
    } else {
        uint32 data32;
        if (COMPILER_64_HI(data)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "soc_reg_set: WARNING: "
                                 "write to 32-bit reg %s with hi order data, 0x%x\n"),
                      SOC_REG_NAME(unit, reg),
                      COMPILER_64_HI(data)));
        }
        data32 = COMPILER_64_LO(data);
#if defined(BCM_XGS_SUPPORT)
        if (soc_feature(unit, soc_feature_regs_as_mem)) {
            (void)soc_ser_reg32_cache_set(unit, reg, port, index, data32);
        }
#endif /* BCM_XGS_SUPPORT */
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            return soc_reg32_write(unit, addr, data32);
        }
        for (block = 0; block < access_info.num_blks && rv == SOC_E_NONE; ++block) {
            rv = _soc_reg32_set(unit, access_info.blk_list[block], access_info.acc_type, addr, data32);
        }
    }

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
          "soc_reg_set failed for %s\n"), SOC_REG_NAME(unit, reg)));
    }
    return rv;
}

/*
 * Write the data to all instances of a 32 bit register
 * This function will work properly only for registers having one block type.
 */
int
soc_reg32_set_all_instances(int unit, soc_reg_t reg, int array_index, uint32 data)
{
    uint32 nof_instances = 0;
    int instance = 0;
    int rv = SOC_E_NONE, out_rv = 0;
    if (soc_feature(unit,soc_feature_single_block_type_per_register)) {
        nof_instances = SOC_REG_BLOCK_NOF_INSTANCES(unit, reg);
        for (instance = 0; instance < nof_instances; instance++)
        {
            rv = soc_reg32_set(unit, reg, instance, array_index, data);
            if (rv != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "soc_reg_set_all_instances failed for %s\n"), SOC_REG_NAME(unit, reg)));
                out_rv = rv;
            }
        }
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "soc_reg_set_all_instaces: ERROR: "
                                 "Not supported for this device \n")));
        out_rv = SOC_E_FAIL;
    }
    return out_rv;
}

/*
 * Write the data to all instances of a 64 bit register
 * This function will work properly only for registers having one block type.
 */
int
soc_reg64_set_all_instances(int unit, soc_reg_t reg, int array_index, uint64 data)
{
    uint32 nof_instances = 0;
    int instance = 0;
    int rv = SOC_E_NONE, out_rv = 0;
    if (soc_feature(unit,soc_feature_single_block_type_per_register)) {
        nof_instances = SOC_REG_BLOCK_NOF_INSTANCES(unit, reg);
        for (instance = 0; instance < nof_instances; instance++)
        {
            rv = soc_reg64_set(unit, reg, instance, array_index, data);
            if (rv != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "soc_reg_set_all_instances failed for %s\n"), SOC_REG_NAME(unit, reg)));
                out_rv = rv;
            }
        }
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "soc_reg_set_all_instaces: ERROR: "
                                 "Not supported for this device \n")));
        out_rv = SOC_E_FAIL;
    }
    return out_rv;
}

/*
 * Write the data to all instances of a register above 64
 * This function will work properly only for registers having one block type.
 */
int
soc_reg_above_64_set_all_instances(int unit, soc_reg_t reg, int array_index, soc_reg_above_64_val_t data)
{
    uint32 nof_instances = 0;
    int instance = 0;
    int out_rv = 0;
    int rv = SOC_E_NONE;
    if (soc_feature(unit,soc_feature_single_block_type_per_register)) {
        nof_instances = SOC_REG_BLOCK_NOF_INSTANCES(unit, reg);
        for (instance = 0; instance < nof_instances; instance++)
        {
            rv = soc_reg_above_64_set(unit, reg, instance, array_index, data);
            if (rv != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "soc_reg_set_all_instances failed for %s\n"), SOC_REG_NAME(unit, reg)));
                out_rv = rv;
            }
        }
    }
    else
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "soc_reg_set_all_instaces: ERROR: "
                                 "Not supported for this device \n")));
        out_rv = SOC_E_FAIL;
    }
    return out_rv;
}

/* Write to h/w - do not update reg cache */
int
soc_reg_set_nocache(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    uint32 addr;
    int block, rv;
    int pindex = port;
    soc_reg_access_info_t access_info;
    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_REG, reg, TRUE));

    if (SOC_REG_IS_ABOVE_64(unit, reg)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_reg_set: "
                              "Use soc_reg_above_64_set \n")));
        
        return SOC_E_FAIL;
    }

    /* if reloading, don't write to register */
    if (SOC_IS_RELOADING(unit))
    {
        return SOC_E_NONE;
    }
    
#ifdef CRASH_RECOVERY_SUPPORT
/*     Use crash recovery defined callback for access*/
    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.soc_reg_set_nocache)
            {
                return Hw_Log_List[unit].Access_cb.soc_reg_set_nocache(unit, reg, port, index, data);
            }
        }
    }
#endif /* CRASH_RECOVERY_SUPPORT */

#ifdef CANCUN_SUPPORT
    if (SOC_REG_IS_CCH(unit, reg) && !(soc_property_get(unit,
                              "skip_cancun_cch_reg_check", 0) ? TRUE : FALSE)) {
        SOC_IF_ERROR_RETURN(soc_cancun_cch_reg_set(unit, reg, index, data));

        if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
            return(soc_cancun_pseudo_reg_set(unit, reg, data));
        }
    }
#endif

    rv = soc_reg_xaddr_get(unit, reg, port, index,
      SOC_REG_ADDR_OPTION_WRITE, &access_info);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
          "soc_reg_set_nocache: failed to get register address")));
        return rv;
    }
    addr = access_info.offset;
    block = access_info.blk_list[0];

    if (SOC_REG_IS_64(unit, reg)) {
        soc_port_t _port;
        soc_block_types_t regblktype = SOC_REG_INFO(unit, reg).block;
        int blk, bindex;

        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
           return soc_reg_write(unit, reg, addr, data);
        }
        if (SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_PORT) 
#ifdef BCM_SAND_SUPPORT
           && !SOC_IS_SAND(unit)
#endif /* BCM_SAND_SUPPORT */
#ifdef BCM_BIGMAC_SUPPORT
           && iterative_op_required(reg)
#endif /* BCM_BIGMAC_SUPPORT */
            ) {
            PBMP_HYPLITE_ITER(unit, _port) {
                blk = SOC_PORT_BLOCK(unit, _port);
                bindex = SOC_PORT_BINDEX(unit, _port);
                if ((SOC_BLOCK2SCH(unit, blk) == block) && (bindex == pindex)) {
                    break;
                }
            }         
            if (!IS_HYPLITE_PORT(unit, port)) {
                return _soc_reg64_set(unit, block, access_info.acc_type, addr, data);
            } 
#ifdef BCM_BIGMAC_SUPPORT
            else {   
                return soc_reg64_set_iterative(unit, block, access_info.acc_type, addr,
                                               port, data);
            }
#endif /* BCM_BIGMAC_SUPPORT */           
        } else {
            for (block = 0; block < access_info.num_blks && rv == SOC_E_NONE; ++block) {
                rv = _soc_reg64_set(unit, access_info.blk_list[block], access_info.acc_type, addr, data);
            }
        }
    } else {
        uint32 data32;
        if (COMPILER_64_HI(data)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "soc_reg_set: WARNING: "
                                 "write to 32-bit reg %s with hi order data, 0x%x\n"),
                      SOC_REG_NAME(unit, reg),
                      COMPILER_64_HI(data)));
        }
        data32 = COMPILER_64_LO(data);
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            return soc_reg32_write(unit, addr, data32);
        }
        for (block = 0; block < access_info.num_blks && rv == SOC_E_NONE; ++block) {
            rv = _soc_reg32_set(unit, access_info.blk_list[block], access_info.acc_type, addr, data32);
        }
    }

    return rv;
}

/*
 * Write an internal SOC register through S-Channel messaging buffer.
 * Handle register at any size 
 */

int
soc_reg_above_64_set(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    uint32 addr;
    int i, rv, reg_size;
    uint64 data64;
    soc_reg_access_info_t access_info;

    /* if reloading, don't write to register */
    if (SOC_IS_RELOADING(unit))
    {
        return SOC_E_NONE;
    }    
    
    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }
                      
    /*     Use crash recovery defined callback for access*/
#ifdef CRASH_RECOVERY_SUPPORT

    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.reg_above64_set)
            {
                return Hw_Log_List[unit].Access_cb.reg_above64_set(unit, reg, port, index, data);
            }
        }
    }

#endif /* CRASH_RECOVERY_SUPPORT */

    /* Use user defined callback for access */
    if(SOC_INFO(unit).reg_access.reg_above64_set) {
        return SOC_INFO(unit).reg_access.reg_above64_set(unit, reg, port, index, data);
    }

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg_above_64_set(unit, reg, port, data);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */
    
    if (SOC_REG_IS_ABOVE_64(unit, reg)) 
    {
        reg_size = SOC_REG_ABOVE_64_INFO(unit, reg).size;
        rv = soc_reg_xaddr_get(unit, reg, port, index,
          SOC_REG_ADDR_OPTION_WRITE, &access_info);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
              "soc_reg64_set: failed to get register address")));
            return rv;
        }
        addr = access_info.offset;
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            access_info.blk_list[0] = ((addr >> SOC_BLOCK_BP) & 0xf) | (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
            access_info.num_blks = 1;
        }

        for (i = 0; i < access_info.num_blks && rv == SOC_E_NONE; ++i) {
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
                _soc_reg_above_64_debug(unit, "set", access_info.blk_list[i], addr, data);
        }
#endif /* BROADCOM_DEBUG */

            rv = soc_direct_reg_set(unit, access_info.blk_list[i], addr, reg_size, data);
        }
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
              "soc_reg_above_64_set failed for %s\n"), SOC_REG_NAME(unit, reg)));
        }
        return rv;
    } 
    else if (SOC_REG_IS_64(unit, reg)) {
        COMPILER_64_SET(data64, data[1], data[0]);
        return soc_reg_set(unit, reg, port, index, data64);
    } 
    else {
        COMPILER_64_SET(data64, 0, data[0]);
        return soc_reg_set(unit, reg, port, index, data64);
    }
}

/*
 * Write an internal SOC register through S-Channel messaging buffer.
 */

int
soc_reg32_write(int unit,
                uint32 addr,
                uint32 data)
{
    schan_msg_t schan_msg;
    uint32 allow_intr=0;
    int dst_blk, src_blk, data_byte_len;
#ifdef BCM_CMICM_SUPPORT
    int cmc = SOC_PCI_CMC(unit);
#endif

#ifdef BROADCOM_DEBUG
    if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
        _soc_reg_debug(unit, 32, "write", addr, 0, data);
    }
#endif /* BROADCOM_DEBUG */
    _soc_snoop_reg(unit, 0, 0, addr,SOC_REG_SNOOP_WRITE, 0, data);

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg32_write(unit, addr, data);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */

#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm) &&
        (NULL != SOC_CONTROL(unit)->fschanMutex)) {
        FSCHAN_LOCK(unit);
        soc_pci_write(unit, CMIC_CMCx_FSCHAN_ADDRESS_OFFSET(cmc), addr);
        soc_pci_write(unit, CMIC_CMCx_FSCHAN_DATA32_OFFSET(cmc), data);
        fschan_wait_idle(unit);
        FSCHAN_UNLOCK(unit);
        return SOC_E_NONE;
    }
#endif /* CMICM */
    /*
     * Setup S-Channel command packet
     *
     * NOTE: the datalen field matters only for the Write Memory and
     * Write Register commands, where it is used only by the CMIC to
     * determine how much data to send, and is in units of bytes.
     */

    schan_msg_clear(&schan_msg);

    dst_blk = ((addr >> SOC_BLOCK_BP) & 0xf) | 
        (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
    data_byte_len = SOC_IS_XGS12_FABRIC(unit) ? 8 : 4;
    {
        src_blk = SOC_IS_SHADOW(unit) ?
            0 : SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    }
    soc_schan_header_cmd_set(unit, &schan_msg.header, WRITE_REGISTER_CMD_MSG,
                             dst_blk, src_blk, 0, data_byte_len, 0, 0);

    schan_msg.writecmd.address = addr;
    schan_msg.writecmd.data[0] = data;

    if(SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /* Write header word + address + data DWORD */
    /* Note: The hardware does not send WRITE_REGISTER_ACK_MSG. */
    
    

    return soc_schan_op(unit, &schan_msg, 3, 0, allow_intr);

}

/*
 * Write an internal SOC register through S-Channel messaging buffer.
 */

int
soc_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32 data)
{
    uint32 addr;
    int i, rv = 0;
    soc_reg_access_info_t access_info;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int j=0, acc=0;
    uint16 dev_id;
    uint8 rev_id;
    soc_reg_info_t *regp;
#endif
    /* if reloading, don't write to register */
    if (SOC_IS_RELOADING(unit))
    {
        return SOC_E_NONE;
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    regp = &(SOC_REG_INFO(unit, reg));
    soc_cm_get_id (unit, &dev_id, &rev_id);
#endif

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_REG, reg, TRUE));

#ifdef CRASH_RECOVERY_SUPPORT
    /*     Use crash recovery defined callback for access*/

    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.reg32_set)
            {
                return Hw_Log_List[unit].Access_cb.reg32_set(unit, reg, port, index, data);
            }
        }
    }
#endif /* CRASH_RECOVERY_SUPPORT */


    /* Use user defined callback for access */
    if(SOC_INFO(unit).reg_access.reg32_set) {
        return SOC_INFO(unit).reg_access.reg32_set(unit, reg, port, index, data);
    }

#ifdef CANCUN_SUPPORT
    if (SOC_REG_IS_CCH(unit, reg) && !(soc_property_get(unit,
                              "skip_cancun_cch_reg_check", 0) ? TRUE : FALSE)) {
        uint64 data64;
        COMPILER_64_SET(data64, 0, data);
        SOC_IF_ERROR_RETURN(
                soc_cancun_cch_reg_set(unit, reg, index, data64));

        if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
            return(soc_cancun_pseudo_reg_set(unit, reg, data64));
        }
    }
#endif

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg32_set(unit, reg, port, data);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */

    rv = soc_reg_xaddr_get(unit, reg, port, index,
      SOC_REG_ADDR_OPTION_WRITE, &access_info);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
          "soc_reg32_set: failed to get register address")));
        return rv;
    }
    addr = access_info.offset;
    if (SOC_REG_IS_ABOVE_32(unit, reg)) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is not 32 bit\n"), soc_reg_name[reg]));
#endif
#endif

    }
    assert(!SOC_REG_IS_ABOVE_32(unit, reg));
#if defined(BCM_XGS_SUPPORT)
    if (soc_feature(unit, soc_feature_regs_as_mem)) {
        (void)soc_ser_reg32_cache_set(unit, reg, port, index, data);
    }
#endif /* BCM_XGS_SUPPORT */
    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
        return soc_reg32_write(unit, addr, data);
    }

    for (i = 0; i < access_info.num_blks && rv == SOC_E_NONE; ++i) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* For 56983 Lower die SKU, we make sure DATA_SPLIT and DUPL writes get
           converted into 4 unique writes since the OTP settings are not
           available
        */
        if ((access_info.acc_type == 9 || access_info.acc_type == 14)  &&
                            (dev_id == BCM56983_DEVICE_ID)) {
           for (j = 0; j < 4; j++) {
               switch(j) {
               case 0 : acc = 0;
                        break;
               case 1 : acc = 1;
                        break;
               case 2 : acc = 6;
                        break;
               case 3 : acc = 7;
                        break;
               }
               rv = _soc_reg32_set(unit, access_info.blk_list[i], acc, addr, data);
           }
        } else if (access_info.acc_type == 16  && (dev_id == BCM56983_DEVICE_ID)) {
            acc = 0;
            rv = _soc_reg32_set(unit, access_info.blk_list[i], acc, addr, data);
        } else if (access_info.acc_type == 15  && (dev_id == BCM56983_DEVICE_ID)) {
            for (j = 0; j < 2; j++) {
                switch(j) {
                case 0 : acc = 0;
                         break;
                case 1 : acc = 6;
                         break;
                }
                rv = _soc_reg32_set(unit, access_info.blk_list[i], acc, addr, data);
            }
        } else if ((dev_id == BCM56982_DEVICE_ID) &&
                                (regp->pipe_stage > 14 && regp->pipe_stage < 23)
                        && (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_EPIPE))) {
                    /* For 56982 SKU, we have certain registers which belong to
                       PMs and since this SKU does not have all the PMs active
                       in the pipes, we determine which PM the register
                       belongs to and skip the access if necessary.
                       When default registers are being programmed
                       (e.g IDB_CA0_BUFFER_CONFIGr) which does not have
                       the pipe info, we program all the valid instances
                       of this register in each pipe
                    */
                    int pipe = 0;
                    int pipe_stage= 0;
                    int pm_num = 0, pm = 0;
                    soc_info_t *si;
                    si = &SOC_INFO(unit);
                    pipe_stage = regp->pipe_stage;
                        /* We do this special programming in all pipes only
                           for unique/duplicate acc_type
                         */
                        if (access_info.acc_type == 9) {
                            soc_reg_t temp_reg;
                            temp_reg = reg;
                            if (pipe_stage > 14 && pipe_stage < 19) {
                                pm_num = pipe_stage - 15;
                            }

                            if (pipe_stage > 18 && pipe_stage < 23) {
                                pm_num = pipe_stage - 19;
                            }

                            for (pipe = 0; pipe < 8; pipe++) {
                                pm = pm_num + (pipe * 4);
                                temp_reg = reg + 1 + pipe;
                                if (si->active_pm_map[pm] == 1) {
                                    rv = soc_reg_xaddr_get(unit, temp_reg,
                                                           port, index,
                                                           SOC_REG_ADDR_OPTION_WRITE,
                                                           &access_info);
                                    if (rv != SOC_E_NONE) {
                                       LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                            "soc_reg32_set: failed to get register address")));
                                       return rv;
                                    }
                                    addr = access_info.offset;
                                    rv = _soc_reg32_set(unit, access_info.blk_list[i],
                                                        pipe, addr, data);
                                }
                            }
                            continue;
                        } else {
                            /* Below code checks if the register is valid for
                               that particular PM in the pipe. If it is not,
                               skip it.
                             */
                             if (access_info.acc_type < 8) {
                                 if (pipe_stage > 14 && pipe_stage < 19) {
                                     pm_num = pipe_stage - 15;
                                 }

                                 if (pipe_stage > 18 && pipe_stage < 23) {
                                    pm_num = pipe_stage - 19;
                                 }

                                 pm = pm_num + (access_info.acc_type * 4);
                             }

                             if (si->active_pm_map[pm] != 1) {
                                char reg_name[80];
                                sal_sprintf(reg_name, "%s",
                                                SOC_REG_NAME(unit, reg));
                                cli_out("Register %s does not exist in"
                                                " TH3 56982 SKU\n",reg_name);
                                continue;

                            } else {
                                   rv = _soc_reg32_set(unit, access_info.blk_list[i], access_info.acc_type, addr, data);
                            }
                        }

        } else
#endif
        {
           rv = _soc_reg32_set(unit, access_info.blk_list[i], access_info.acc_type, addr, data);
        }
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "soc_reg32_set failed for %s failed\n"), SOC_REG_NAME(unit, reg)));
        }
    }
    return rv;
}

/*
 * Write an internal 64-bit SOC register through S-Channel messaging buffer.
 */
int
soc_reg64_write(int unit,
                uint32 addr,
                uint64 data)
{
    schan_msg_t schan_msg;
    uint32 allow_intr=0;
    int dst_blk, src_blk;
#ifdef BCM_CMICM_SUPPORT
    int cmc = SOC_PCI_CMC(unit);
#endif

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg64_write(unit, addr, data);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */

#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm) &&
        (NULL != SOC_CONTROL(unit)->fschanMutex)) {
        FSCHAN_LOCK(unit);
        soc_pci_write(unit, CMIC_CMCx_FSCHAN_ADDRESS_OFFSET(cmc), addr);
        /* coverity[result_independent_of_operands] */
       SOC_IF_ERROR_RETURN(soc_pci_write(unit, 
                                          CMIC_CMCx_FSCHAN_DATA64_HI_OFFSET(cmc), 
                                          COMPILER_64_HI(data)));
        SOC_IF_ERROR_RETURN(soc_pci_write(unit, 
                                          CMIC_CMCx_FSCHAN_DATA64_LO_OFFSET(cmc), 
                                          COMPILER_64_LO(data)));

        fschan_wait_idle(unit);
        FSCHAN_UNLOCK(unit);
        return SOC_E_NONE;
    }
#endif /* CMICM */
    /*
     * Setup S-Channel command packet
     *
     * NOTE: the datalen field matters only for the Write Memory and
     * Write Register commands, where it is used only by the CMIC to
     * determine how much data to send, and is in units of bytes.
     */

    schan_msg_clear(&schan_msg);

    dst_blk = ((addr >> SOC_BLOCK_BP) & 0xf) | 
        (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
    {
        src_blk = SOC_IS_SHADOW(unit) ?
            0 : SOC_BLOCK2SCH(unit, CMIC_BLOCK(unit));
    }
    soc_schan_header_cmd_set(unit, &schan_msg.header, WRITE_REGISTER_CMD_MSG,
                             dst_blk, src_blk, 0, 8, 0, 0);

    schan_msg.writecmd.address = addr;
    schan_msg.writecmd.data[0] = COMPILER_64_LO(data);
    schan_msg.writecmd.data[1] = COMPILER_64_HI(data);

#ifdef BROADCOM_DEBUG
    if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
        _soc_reg_debug(unit, 64, "write", addr,
                       schan_msg.writecmd.data[1],
                       schan_msg.writecmd.data[0]);
    }
#endif /* BROADCOM_DEBUG */
    _soc_snoop_reg(unit, 0, 0, addr,SOC_REG_SNOOP_WRITE, 
                   schan_msg.writecmd.data[1],schan_msg.writecmd.data[0]);

    if(SOC_IS_SAND(unit)) {
        allow_intr = 1;
    }

    /* Write header word + address + 2*data DWORD */
    /* Note: The hardware does not send WRITE_REGISTER_ACK_MSG. */
    
    

    return soc_schan_op(unit, &schan_msg, 4, 0, allow_intr);

}

/*
 * Write an internal 64-bit SOC register through S-Channel messaging buffer.
 */
int
soc_reg64_set(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    uint32 addr;
    int i, rv;
    soc_reg_access_info_t access_info;
#ifdef CRASH_RECOVERY_SUPPORT

    /*     Use crash recovery defined callback for access*/
    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.reg64_set)
            {
                return Hw_Log_List[unit].Access_cb.reg64_set(unit, reg, port, index, data);
            }
        }
    }

#endif /* CRASH_RECOVERY_SUPPORT */

    DNXC_MTA(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_REG, reg, TRUE));

#ifdef CANCUN_SUPPORT
    if (SOC_REG_IS_CCH(unit, reg) && !(soc_property_get(unit,
                              "skip_cancun_cch_reg_check", 0) ? TRUE : FALSE)) {
        soc_cancun_cch_reg_set(unit, reg, index, data);

        if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CCH)) {
            return(soc_cancun_pseudo_reg_set(unit, reg, data));
        }
    }
#endif
    
    /* Use user defined callback for access */
    if(SOC_INFO(unit).reg_access.reg64_set) {
        return SOC_INFO(unit).reg_access.reg64_set(unit, reg, port, index, data);
    }

    rv = soc_reg_xaddr_get(unit, reg, port, index,
      SOC_REG_ADDR_OPTION_WRITE, &access_info);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
          "soc_reg64_set: failed to get register address")));
        return rv;
    }
    addr = access_info.offset;
    assert(SOC_REG_IS_64(unit, reg));
#if defined(BCM_XGS_SUPPORT)
    if (soc_feature(unit, soc_feature_regs_as_mem)) {
        (void)soc_ser_reg_cache_set(unit, reg, port, index, data);
    }
#endif /* BCM_XGS_SUPPORT */
    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
        return soc_reg64_write(unit, addr, data);
    }
    for (i = 0; i < access_info.num_blks && rv == SOC_E_NONE; ++i) {
        rv = _soc_reg64_set(unit, access_info.blk_list[i], access_info.acc_type, addr, data);
    }
    return rv;
}

/*
 * Write an internal SOC register through S-Channel messaging buffer
 * with Raw Port Number.
 */
int
soc_reg_rawport_set(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    uint32 addr;
    int i, rv;
    soc_reg_access_info_t access_info;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    if ((REG_PORT_ANY != port) &&
        (port & (SOC_REG_ADDR_INSTANCE_MASK | SOC_REG_ADDR_BLOCK_ID_MASK | 
                SOC_REG_ADDR_SCHAN_ID_MASK | SOC_REG_ADDR_PHY_ACC_MASK))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "This function is only for Raw Port Numbers \n")));
        return SOC_E_FAIL;
    }

#ifdef CRASH_RECOVERY_SUPPORT
/* Don't handle Special Accesses */
    if((SOC_INFO(unit).reg_access.reg64_set) || /* User defined */
       (SOC_REG_IS_ABOVE_64(unit, reg)) ||
       (SOC_IS_DONE_INIT(unit) && BCM_UNIT_DO_HW_READ_WRITE(unit) &&
        Hw_Log_List[unit].Access_cb.soc_reg_set)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Use soc_reg_set \n")));
        return SOC_E_FAIL;
    }
#else
    /* Don't handle Special Accesses */
    if((SOC_INFO(unit).reg_access.reg64_set) || /* User defined */
       (SOC_REG_IS_ABOVE_64(unit, reg))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Use soc_reg_set \n")));
        return SOC_E_FAIL;
    }
#endif /* CRASH_RECOVERY_SUPPORT */

    rv = soc_reg_xaddr_get(unit, reg, port, index, SOC_REG_ADDR_OPTION_WRITE |
      SOC_REG_ADDR_OPTION_PRESERVE_PORT, &access_info);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
          "soc_reg_rawport_set: failed to get register address")));
        return rv;
    }
    addr = access_info.offset;
    if (SOC_REG_IS_64(unit, reg)) {
#if defined(BCM_XGS_SUPPORT)
        if (soc_feature(unit, soc_feature_regs_as_mem)) {
            (void)soc_ser_reg_cache_set(unit, reg, port, index, data);
        }
#endif /* BCM_XGS_SUPPORT */
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            return soc_reg_write(unit, reg, addr, data);
        } else {
            for (i = 0; i < access_info.num_blks && rv == SOC_E_NONE; ++i) {
                rv = _soc_reg64_set(unit, access_info.blk_list[i], access_info.acc_type, addr, data);
            }
        }
    } else {
        uint32 data32;
        if (COMPILER_64_HI(data)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "WARNING: "
                                 "write to 32-bit reg %s with hi order data, 0x%x\n"),
                      SOC_REG_NAME(unit, reg),
                      COMPILER_64_HI(data)));
        }
        data32 = COMPILER_64_LO(data);
#if defined(BCM_XGS_SUPPORT)
        if (soc_feature(unit, soc_feature_regs_as_mem)) {
            (void)soc_ser_reg32_cache_set(unit, reg, port, index, data32);
        }
#endif /* BCM_XGS_SUPPORT */

        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            return soc_reg32_write(unit, addr, data32);
        } else {
            for (i = 0; i < access_info.num_blks && rv == SOC_E_NONE; ++i) {
                rv = _soc_reg32_set(unit, access_info.blk_list[i], access_info.acc_type, addr, data32);
        }
    }
    }
    return rv;
}

int
soc_reg32_rawport_set(int unit, soc_reg_t reg, int port, int index, uint32 data)
{
    uint64 d64;

    COMPILER_64_SET(d64, 0, data);
    return soc_reg_rawport_set(unit, reg, port, index, d64);
}

/*
 * Write internal register for a group of ports.
 * The specified register must be a port reg (type soc_portreg).
 */

int
soc_reg_write_ports(int unit,
                    soc_reg_t reg,
                    pbmp_t pbmp,
                    uint32 value)
{
    soc_port_t        port;
    soc_block_t       ptype; 
    soc_block_types_t rtype;

    /* assert(reg is a port register) */
    if (!SOC_REG_IS_VALID(unit, reg) ||
        SOC_REG_INFO(unit, reg).regtype != soc_portreg) {
        return SOC_E_UNAVAIL;
    }

    rtype = SOC_REG_INFO(unit, reg).block;

    /*
     * each port block type must match one of the register block types
     * or the register block type can be the MMU
     */
    PBMP_ITER(pbmp, port) {
        ptype = SOC_PORT_TYPE(unit, port);
        if (SOC_BLOCK_IN_LIST(unit, rtype, ptype) || SOC_BLOCK_IN_LIST(unit, rtype, SOC_BLK_MMU)) {
            if (soc_feature(unit, soc_feature_new_sbus_format)) {
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg,
                                                  port, 0, value));
            } else {
#if defined(BCM_XGS_SUPPORT)
                if (soc_feature(unit, soc_feature_regs_as_mem)) {
                    (void)soc_ser_reg32_cache_set(unit, reg, port, 0, value);
                }
#endif /* BCM_XGS_SUPPORT */
                SOC_IF_ERROR_RETURN(soc_reg32_write(unit,
                                    soc_reg_addr(unit, reg, port, 0), value));
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Write internal register for a block or group of blocks.
 * The specified register must be a generic reg (type soc_genreg).
 *
 * This routine will write to all possible blocks for the given
 * register.
 */
int
soc_reg64_write_all_blocks(int unit,
                           soc_reg_t reg,
                           uint64 value)
{
    int               blk, port;
    soc_block_types_t rtype;

    /* assert(reg is not a port or cos register) */
    if (!SOC_REG_IS_VALID(unit, reg) ||
        SOC_REG_INFO(unit, reg).regtype != soc_genreg) {
        return SOC_E_UNAVAIL;
    }

    rtype = SOC_REG_INFO(unit, reg).block;

    SOC_BLOCKS_ITER(unit, blk, rtype) {
        port = SOC_BLOCK_PORT(unit, blk);
#if defined(BCM_XGS_SUPPORT)
        if (soc_feature(unit, soc_feature_regs_as_mem)) {
            (void)soc_ser_reg_cache_set(unit, reg, port, 0, value);
        }
#endif /* BCM_XGS_SUPPORT */        
        if (soc_feature(unit, soc_feature_new_sbus_format)) {
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, value));
        } else {
            SOC_IF_ERROR_RETURN(soc_reg_write(unit, reg,
                                soc_reg_addr(unit, reg, port, 0), value));
        }
    }
    return SOC_E_NONE;
}

/*
 * Write internal register for a block or group of blocks.
 * The specified register must be a generic reg (type soc_genreg).
 *
 * This routine will write to all possible blocks for the given
 * register.
 */
int
soc_reg_write_all_blocks(int unit,
                         soc_reg_t reg,
                         uint32 value)
{
    uint64 val64;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    COMPILER_64_SET(val64, 0, value);
    return soc_reg64_write_all_blocks(unit, reg, val64);
}

/*
 * Read a general register from any block that has a copy
 */
int
soc_reg64_read_any_block(int unit,
                         soc_reg_t reg,
                         uint64 *datap)
{
    int               blk, port;
    soc_block_types_t rtype;
#ifdef CRASH_RECOVERY_SUPPORT
    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
          LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "soc_reg64_read_any_block: WARNING: "
                                 "HW Log feature in unexpected function!!!\n")));
        }
    }
#endif /* CRASH_RECOVERY_SUPPORT */

    /* assert(reg is not a port or cos register) */
    if (!SOC_REG_IS_VALID(unit, reg) ||
        SOC_REG_INFO(unit, reg).regtype != soc_genreg) {
        return SOC_E_UNAVAIL;
    }

    rtype = SOC_REG_INFO(unit, reg).block;
    SOC_BLOCKS_ITER(unit, blk, rtype) {
        port = SOC_BLOCK_PORT(unit, blk);
        if (soc_feature(unit, soc_feature_new_sbus_format)) {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, datap));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_read(unit, reg, soc_reg_addr(unit, reg, port, 0),
                              datap));
        }
        break;
    }
    return SOC_E_NONE;
}

/*
 * Read a general register from any block that has a copy
 */
int
soc_reg_read_any_block(int unit,
                     soc_reg_t reg,
                     uint32 *datap)
{
    uint64 val64;

    SOC_IF_ERROR_RETURN(soc_reg64_read_any_block(unit, reg, &val64));
    COMPILER_64_TO_32_LO(*datap, val64);

    return SOC_E_NONE;
}

/****************************************************************
 * Register field manipulation functions
 */

/* Define a macro so the assertion printout is informative. */
#define        REG_FIELD_IS_VALID        finfop

/*
 * Function:     soc_reg_field_length
 * Purpose:      Return the length of a register field in bits.
 *               Value is 0 if field is not found.
 * Returns:      bits in field
 */
int
soc_reg_field_length(int unit, soc_reg_t reg, soc_field_t field)
{
    soc_field_info_t *finfop;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return 0;
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
        return 0;
    }
    return finfop->len;
}

/*
 * Function:     soc_reg_field_valid
 * Purpose:      Determine if a field in a register is valid.
 * Returns:      Returns TRUE  if field is found.
 *               Returns FALSE if field is not found.
 */
int
soc_reg_field_valid(int unit, soc_reg_t reg, soc_field_t field)
{
    soc_field_info_t *finfop;

    if (!SOC_REG_IS_VALID(unit, reg)) {
        return FALSE;
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    return (finfop != NULL);
}


/*
 * Function:     soc_reg_field_get
 * Purpose:      Get the value of a field from a register
 * Parameters:
 * Returns:      Value of field
 */
uint32
soc_reg_field_get(int unit, soc_reg_t reg, uint32 regval, soc_field_t field)
{
    soc_field_info_t *finfop;
    uint32           val;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);

    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    val = regval >> finfop->bp;
    if (finfop->len < 32) {
        return val & ((1 << finfop->len) - 1);
    } else {
        return val;
    }
}

/*
 * Function:     soc_ftmh_cfg_get
 * Purpose:      Get the value of FTMH_LB_KEY_EXT_ENf, FTMH_STACKING_EXT_ENABLEf into the given int pointers
 */
int
soc_ftmh_cfg_get(int unit, int * p_cfg_ftmh_lb_key_ext_en, int * p_cfg_ftmh_stacking_ext_enable)
{
        uint32
            reg_val32;
        int
            rv = SOC_E_NONE;

        rv = READ_ECI_GLOBALFr(unit, &reg_val32);
        *p_cfg_ftmh_lb_key_ext_en = soc_reg_field_get(unit, ECI_GLOBALFr, reg_val32, FTMH_LB_KEY_EXT_ENf);
        *p_cfg_ftmh_stacking_ext_enable = soc_reg_field_get(unit, ECI_GLOBALFr, reg_val32, FTMH_STACKING_EXT_ENABLEf);

        return rv;
}

/*
 * Function:     soc_reg64_field_get
 * Purpose:      Get the value of a field from a 64-bit register
 * Parameters:
 * Returns:      Value of field (64 bits)
 */
uint64
soc_reg64_field_get(int unit, soc_reg_t reg, uint64 regval, soc_field_t field)
{
    soc_field_info_t *finfop;
    uint64           mask;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    COMPILER_64_MASK_CREATE(mask, finfop->len, 0);
    COMPILER_64_SHR(regval, finfop->bp);
    COMPILER_64_AND(regval, mask);

    return regval;
}

/* 
 * Function:     soc_reg64_field32_get
 * Purpose:      Get the value of a field from a 64-bit register
 * Parameters:
 * Returns:      Value of field (32 bits)
 */
uint32
soc_reg64_field32_get(int unit, soc_reg_t reg, uint64 regval,
                      soc_field_t field)
{
    soc_field_info_t *finfop;
    uint32           val32;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    COMPILER_64_SHR(regval, finfop->bp);
    COMPILER_64_TO_32_LO(val32, regval);
    if (finfop->len < 32) {
        return val32 & ((1 << finfop->len) - 1);
    } else {
        return val32;
    }
}

/*
 * Function:     soc_reg_above_64_field_get
 * Purpose:      Get the value of a field from a register
 * Parameters:
 * Returns:      Value of field
 */
void
soc_reg_above_64_field_get(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval, 
                           soc_field_t field, soc_reg_above_64_val_t field_val)
{
    soc_field_info_t *finfop;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);

    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }
    
    
    SOC_REG_ABOVE_64_CLEAR(field_val);
    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    SHR_BITCOPY_RANGE(field_val, 0, regval, finfop->bp, finfop->len);
    
}
/*
 * Function:     soc_reg_above_64_field_read
 * Purpose:      Read a register of any size, and get the value of a field sized up to any size.
 * Parameters:
 * Returns:      Value of field (32 bits)
 */
int
soc_reg_above_64_field_read(int unit, soc_reg_t reg, soc_port_t port, int index, soc_field_t field, soc_reg_above_64_val_t out_field_val)
{   
    int rc = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    SOC_REG_ABOVE_64_CLEAR(data);
    rc = soc_reg_above_64_get(unit, reg, port, index, data);
    if (rc != SOC_E_NONE){
        return rc;
    }
    soc_reg_above_64_field_get(unit, reg, data, field, out_field_val);
    return rc;
}
/* 
 * Function:     soc_reg_above_64_field32_get
 * Purpose:      Get the value of a field sized up to 32 bit from a register of any size
 * Parameters:
 * Returns:      Value of field (32 bits)
 */
uint32
soc_reg_above_64_field32_get(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval, 
                           soc_field_t field)
{
    soc_field_info_t *finfop;
    uint32 field_val = 0;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);

    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    } else if (finfop->len > 32) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s has a size of %u bits "
                                    "which is greater than 32\n"),
                   soc_reg_name[reg], soc_fieldnames[field],
                   (unsigned)finfop->len));
#endif
        assert(0);
    } else {

        SHR_BITCOPY_RANGE(&field_val, 0, regval, finfop->bp, finfop->len); /* get the field value */

    }
    return field_val;
}
/*
 * Function:     soc_reg_above_64_field32_read
 * Purpose:      Read a register of any size, and get the value of a field sized up to 32 bit from it.
 * Parameters:
 * Returns:      Value of field (32 bits)
 */

int
soc_reg_above_64_field32_read(int unit, soc_reg_t reg, soc_port_t port, int index, soc_field_t field, uint32* out_field_val)
{   
    int rc = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    SOC_REG_ABOVE_64_CLEAR(data);
    rc = soc_reg_above_64_get(unit, reg, port, index, data);
    if (rc != SOC_E_NONE){
        return rc;
    }
    *out_field_val = soc_reg_above_64_field32_get(unit,reg, data, field);
    return rc;
}
/* 
 * Function:     soc_reg_above_64_field64_get
 * Purpose:      Get the value of a field sized up to 32 bit from a register of any size
 * Parameters:
 * Returns:      Value of field (32 bits)
 */

uint64
soc_reg_above_64_field64_get(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval, 
                           soc_field_t field)
{
    soc_field_info_t *finfop;
    uint64 fieldval;
    
    COMPILER_64_ZERO(fieldval);
    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    /* coverity[var_compare_op] */
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    } else if (finfop->len > 64) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s has a size of %u bits "
                                    "which is greater than 32\n"),
                   soc_reg_name[reg], soc_fieldnames[field],
                   (unsigned)finfop->len));
#endif
        assert(0);
    } else {
        uint32 low = 0, hi = 0;
        if (finfop->len > 32) {
            SHR_BITCOPY_RANGE(&low, 0, regval, finfop->bp, 32); /* get the field value lsb word */
            SHR_BITCOPY_RANGE(&hi, 0, regval, finfop->bp + 32, finfop->len - 32); /* get the field value msb word */
        } else {
            SHR_BITCOPY_RANGE(&low, 0, regval, finfop->bp, finfop->len); /* get the field value */
        }
        COMPILER_64_SET(fieldval, hi, low);
    }
    return fieldval;
}
/*
 * Function:     soc_reg_above_64_field64_read
 * Purpose:      Read a register of any size, and get the value of a field sized up to 64 bit from it.
 * Parameters:
 * Returns:      Value of field (32 bits)
 */
int
soc_reg_above_64_field64_read(int unit, soc_reg_t reg, soc_port_t port, int index, soc_field_t field, uint64* out_field_val)
{   
    int rc = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    SOC_REG_ABOVE_64_CLEAR(data);
    rc = soc_reg_above_64_get(unit, reg, port, index, data);
    if (rc != SOC_E_NONE){
        return rc;
    }
    *out_field_val = soc_reg_above_64_field64_get(unit,reg, data, field);
    return rc;
}

/* Define a macro so the assertion printout is informative. */
#define VALUE_TOO_BIG_FOR_FIELD                ((value & ~mask) != 0)

/*
 * Function:     soc_reg_field_validate
 * Purpose:      Validate the value of a register's field.
 * Parameters:
 * Returns:      SOC_E_XXX
 */
int
soc_reg_field_validate(int unit, soc_reg_t reg, soc_field_t field, uint32 value)
{
    soc_field_info_t *finfop;
    uint32           mask;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
                   
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    if (finfop->len < 32) {
      mask = (1 << finfop->len) - 1; 
      if  (VALUE_TOO_BIG_FOR_FIELD) {
            return SOC_E_PARAM;
      }
     }
    
    return SOC_E_NONE;
}

/*
 * Function:     soc_reg_signed_field_mask
 * Purpose:      Chops high bits of signed value to fit in register field
 * Parameters:
 * Returns:      SOC_E_XXX
 */
int
soc_reg_signed_field_mask(int unit, soc_reg_t reg, soc_field_t field, int32 value_in, uint32 *value_out)
{
    soc_field_info_t *finfop;
    int32            max_valid;
    int32            min_valid;
    uint32           mask;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);

    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    if (finfop->len < 32) {
        mask = (1 << finfop->len) - 1;

        *value_out = (((uint32)value_in) & mask);

        max_valid = (1 << (finfop->len-1)) - 1;
        min_valid = -max_valid;

        if (value_in > max_valid || value_in < min_valid) {
            return SOC_E_PARAM;
        }
    } else {
        *value_out = (uint32) value_in;
    }

    return SOC_E_NONE;
}

/*
 * Function:     soc_reg_unsigned_field_mask
 * Purpose:      Chops high bits of unsigned value to fit in register field
 * Parameters:
 * Returns:      SOC_E_XXX
 */
int
soc_reg_unsigned_field_mask(int unit, soc_reg_t reg, soc_field_t field, int32 value_in, uint32 *value_out)
{
    soc_field_info_t *finfop;
    uint32            max_valid;
    uint32            min_valid;
    uint32            mask;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);

    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    if (finfop->len < 32) {
        mask = (1 << finfop->len) - 1;

        *value_out = (((uint32)value_in) & mask);

        max_valid = (1 << finfop->len) - 1;
        min_valid = 0;

        if (value_in > max_valid || value_in < min_valid) {
            return SOC_E_PARAM;
        }
    } else {
        *value_out = (uint32) value_in;
    }

    return SOC_E_NONE;
}

/*
 * Function:     soc_reg_set_field_if_exists
 * Purpose:      Set the value of a register's field if it exists.
 * Parameters:
 * Returns:      void
 */
void
soc_reg_set_field_if_exists(int unit, soc_reg_t reg, uint32 *regval,
                  soc_field_t field, uint32 value)
{
    soc_field_info_t *finfop;
    uint32           mask;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
        return;
    }

    if (finfop->len < 32) {
        mask = (1 << finfop->len) - 1;
        if (VALUE_TOO_BIG_FOR_FIELD) {
#if !defined(SOC_NO_NAMES)
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "reg %s field %s is too big\n"),
                       soc_reg_name[reg], soc_fieldnames[field]));
#endif
            assert(!VALUE_TOO_BIG_FOR_FIELD);
        }
    } else {
        mask = -1;
    }

    *regval = (*regval & ~(mask << finfop->bp)) | value << finfop->bp;
}

/*
 * Function:     soc_reg_field_set
 * Purpose:      Set the value of a register's field.
 * Parameters:
 * Returns:      void
 */
void
soc_reg_field_set(int unit, soc_reg_t reg, uint32 *regval,
                  soc_field_t field, uint32 value)
{
    soc_field_info_t *finfop;
    uint32           mask;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    if (finfop->len < 32) {
        mask = (1 << finfop->len) - 1;
        if (VALUE_TOO_BIG_FOR_FIELD) {
#if !defined(SOC_NO_NAMES)
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "reg %s field %s is too big\n"),
                       soc_reg_name[reg], soc_fieldnames[field]));
#endif
            assert(!VALUE_TOO_BIG_FOR_FIELD);
        }
    } else {
        mask = -1;
    }

    *regval = (*regval & ~(mask << finfop->bp)) | value << finfop->bp;
}

/*
 * Function:     soc_reg_field_set
 * Purpose:      Set the value of a register's field.
 * Parameters:
 * Returns:      void
 */
void
soc_reg_above_64_field_set(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval,
                            soc_field_t field, CONST soc_reg_above_64_val_t value)
{
    soc_field_info_t *finfop;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }
    { /* Check if the field value will fit into the field: Verify that value's bits that do not fit in the field are all zeroes. */

        /*
         * COVERITY
         *
         * assert validates the input for NULL
         */
        /* coverity[var_deref_op : FALSE] */
        unsigned msb_bits = finfop->len % 32; /* Bits in msb word of field value */
        unsigned idx      = finfop->len / 32; /* word Iteration index, starts with msb word of field. */

        if (msb_bits) { /* if the msb vaule word has left over bits unused */
            assert (!(value[idx] & (((uint32)0xffffffff) << msb_bits))); /* verify the left over bits are zeros */
            ++idx;
        }
        for (; idx < SOC_REG_ABOVE_64_MAX_SIZE_U32; ++idx) { /* verify the remaining words are zeros */
            assert (!(value[idx]));
        }
    }
    
    SHR_BITCOPY_RANGE(regval, finfop->bp, value, 0, finfop->len);

}

/* 
 * Function:     soc_reg_above_64_field32_set
 * Purpose:      Set the value of a register's field; field must be <= 32 bits, any register size supported
 * Parameters:
 * Returns:      void
 */
void
soc_reg_above_64_field32_set(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval,
                            soc_field_t field, uint32 value)
{
    soc_field_info_t *finfop;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    /* Check if the field value will fit into the field: Verify that value's bits that do not fit in the field are all zeroes. */
    } else if (finfop->len > 32) {
        SHR_BITCLR_RANGE(regval, finfop->bp + 32, finfop->len - 32);
        SHR_BITCOPY_RANGE(regval, finfop->bp, &value, 0, 32);
    } else {
        if (finfop->len < 32 && value >= (((uint32)1) << finfop->len)) {
#if !defined(SOC_NO_NAMES)
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "reg %s field %s is too small "
                                        "for value 0x%lx\n"),
                       soc_reg_name[reg], soc_fieldnames[field],
                       (unsigned long)value));
#endif
            assert (0);
        }
        SHR_BITCOPY_RANGE(regval, finfop->bp, &value, 0, finfop->len);
    }

}


/* 
 * Function:     soc_reg_above_64_field64_set
 * Purpose:      Set the value of a register's field; field must be <= 64 bits, any register size supported
 * Parameters:
 * Returns:      void
 */
void
soc_reg_above_64_field64_set(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval,
                            soc_field_t field, uint64 value)
{
    soc_field_info_t *finfop;
    uint32 value32;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    /* Check if the field value will fit into the field: Verify that value's bits that do not fit in the field are all zeroes. */
    } else if (finfop->len > 64) {
        SHR_BITCLR_RANGE(regval, finfop->bp + 64, finfop->len - 64);

        value32 = COMPILER_64_LO(value);
        SHR_BITCOPY_RANGE(regval, finfop->bp, &value32, 0, 32);

        value32 = COMPILER_64_HI(value);
        SHR_BITCOPY_RANGE(regval, finfop->bp + 32, &value32, 0, 32);

    } else if (finfop->len <= 32 ) {
        if (finfop->len < 32 && COMPILER_64_LO(value) >= (((uint32)1) << finfop->len)) {
#if !defined(SOC_NO_NAMES)
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "reg %s field %s is too small "
                                        "for value 0x%lx\n"),
                       soc_reg_name[reg], soc_fieldnames[field],
                       (unsigned long)COMPILER_64_LO(value)));
#endif
            assert (0);
        }
        value32 = COMPILER_64_LO(value);
        SHR_BITCOPY_RANGE(regval, finfop->bp, &value32, 0, finfop->len);
    } else { /*32<field lengh<=64*/
        if (finfop->len < 64 && COMPILER_64_HI(value) >= (((uint32)1) << (finfop->len - 32))) {
#if !defined(SOC_NO_NAMES)
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "reg %s field %s is too small "
                                        "for value 0x%lx\n"),
                       soc_reg_name[reg], soc_fieldnames[field],
                       (unsigned long)COMPILER_64_HI(value)));
#endif
            assert (0);
        }
        value32 = COMPILER_64_LO(value);
        SHR_BITCOPY_RANGE(regval, finfop->bp, &value32, 0, 32);

        value32 = COMPILER_64_HI(value);
        SHR_BITCOPY_RANGE(regval, finfop->bp + 32, &value32, 0, finfop->len - 32);

    }

}
#define        VALUE_TOO_BIG_FOR_FIELD64(mask)        (!COMPILER_64_IS_ZERO(mask))

/*
 * Function:     soc_reg64_field_validate
 * Purpose:      Validate the value of a register's field.
 * Parameters:
 * Returns:      SOC_E_XXX
 */
int
soc_reg64_field_validate(int unit, soc_reg_t reg, soc_field_t field, uint64 value)
{
    soc_field_info_t *finfop;
    uint64           mask;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
                   
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    if (finfop->len < 64) {
        COMPILER_64_ZERO(mask);
        COMPILER_64_ADD_32(mask, 1);
        COMPILER_64_SHL(mask, finfop->len);
        COMPILER_64_SUB_32(mask, 1);
        COMPILER_64_NOT(mask);
        COMPILER_64_AND(mask, value);
        if(VALUE_TOO_BIG_FOR_FIELD64(mask))
          return SOC_E_PARAM;

    } 
    
    return SOC_E_NONE;
}

/*
 * Function:     soc_reg64_field_set
 * Purpose:      Set the value of a register's field.
 * Parameters:
 * Returns:      void
 */
void
soc_reg64_field_set(int unit, soc_reg_t reg, uint64 *regval,
                    soc_field_t field, uint64 value)
{
    soc_field_info_t *finfop;
    uint64           mask, tmp;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    if (finfop->len < 64) {
        COMPILER_64_SET(mask, 0, 1);
        COMPILER_64_SHL(mask, finfop->len);
        COMPILER_64_SUB_32(mask, 1);
#ifndef NDEBUG
        /* assert(!VALUE_TOO_BIG_FOR_FIELD); */
        tmp = mask;
        COMPILER_64_NOT(tmp);
        COMPILER_64_AND(tmp, value);
        assert(!VALUE_TOO_BIG_FOR_FIELD64(tmp));
#endif
    } else {
        COMPILER_64_SET(mask, -1, -1);
    }

    /* *regval = (*regval & ~(mask << finfop->bp)) | value << finfop->bp; */
    tmp = mask;
    COMPILER_64_SHL(tmp, finfop->bp);
    COMPILER_64_NOT(tmp);
    COMPILER_64_AND(*regval, tmp);
    COMPILER_64_SHL(value, finfop->bp);
    COMPILER_64_OR(*regval, value);
}

/* 
 * Function:     soc_reg64_field32_set
 * Purpose:      Set the value of a register's field; field must be < 32 bits
 * Parameters:
 * Returns:      void
 */
void
soc_reg64_field32_set(int unit, soc_reg_t reg, uint64 *regval,
                      soc_field_t field, uint32 value)
{
    soc_field_info_t *finfop;
    uint64           mask, tmp;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    SOC_FIND_FIELD(field,
                   SOC_REG_INFO(unit, reg).fields,
                   SOC_REG_INFO(unit, reg).nFields,
                   finfop);
    if (finfop == NULL) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    /*
     * COVERITY
     *
     * assert validates the input for NULL
     */
    /* coverity[var_deref_op : FALSE] */
    if (finfop->len < 64) {
        COMPILER_64_SET(mask, 0, 1);
        COMPILER_64_SHL(mask, finfop->len);
        COMPILER_64_SUB_32(mask, 1);
    } else {
        COMPILER_64_SET(mask, -1, -1);
    }

    /* Mask value to fit in field.  Needed if passed-in value was signed. */
    value &= COMPILER_64_LO(mask);

    /* *regval = (*regval & ~(mask << finfop->bp)) | value << finfop->bp; */
    COMPILER_64_SHL(mask, finfop->bp);
    COMPILER_64_NOT(mask);
    COMPILER_64_AND(*regval, mask);
    if (value != 0) {
        COMPILER_64_SET(tmp, 0, value);
        COMPILER_64_SHL(tmp, finfop->bp);
        COMPILER_64_OR(*regval, tmp);
    }
}

/*
 * Function:    soc_reg_addr
 * Purpose:     calculate the address of a register
 * Parameters:
 *              unit  switch unit
 *              reg   register number
 *              port  port number or REG_PORT_ANY
 *              index array index (or cos number)
 * Returns:     register address suitable for soc_reg_read and friends
 * Notes:       the block number to access is determined by the register
 *              and the port number
 *
 * cpureg       00SSSSSS 00000000 0000RRRR RRRRRRRR
 * genreg       00SSSSSS BBBB1000 0000RRRR RRRRRRRR
 * portreg      00SSSSSS BBBB00PP PPPPRRRR RRRRRRRR
 * cosreg       00SSSSSS BBBB01CC CCCCRRRR RRRRRRRR
 *
 * all regs of bcm88230
 *              00000000 00001000 0000RRRR RRRRRRRR
 *
 * where        B+ is the 4 bit block number
 *              P+ is the 6 bit port number (within a block or chip wide)
 *              C+ is the 6 bit class of service
 *              R+ is the 12 bit register number
 *              S+ is the 6 bit Pipe stage
 */
uint32
soc_reg_addr(int unit, soc_reg_t reg, int port, int index)
{
    uint32            base;   /* base address from reg_info */
    int               block = -1;  /* block number */
    int               pindex = -1; /* register port/cos field */
    int               gransh; /* index granularity shift */
    soc_block_types_t regblktype;
    soc_block_t       portblktype;
    int               phy_port;
    int               instance_mask = 0;
    int               instance = -1;
    

#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_reg_t reg_excep_list[]={MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr,
                                MMU_RQE_REPL_PORT_AGG_MAPr};
    int reg_index, num_regs, add_exception = 0;
#endif
    
    if (SOC_CONTROL(unit)->soc_reg_watch) {
        if (SOC_CONTROL(unit)->prev_reg != reg) {
            LOG_CLI((BSL_META_U(unit, "%d:%s REG: %s\n"),
                       unit, BSL_FUNC, SOC_REG_NAME(unit, reg)));
            SOC_CONTROL(unit)->prev_reg = reg;
        }
    }

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

#ifdef DNX_TEST_CHIPS_SUPPORT
    if (SOC_IS_DNX_TEST_DEVICE(unit)
#if defined(PLISIM)
        && !SAL_BOOT_PLISIM
#endif
        ) {
        return soc_dnxtestchip_reg_addr(unit, reg, port, index);
    }
#endif /* DNX_TEST_CHIPS_SUPPORT */

#define SOC_REG_ADDR_INVALID_PORT 0 /* for asserts */

    {
        portblktype = SOC_BLK_PORT;
    }

    regblktype = SOC_REG_INFO(unit, reg).block;
    if(REG_PORT_ANY != port) {
        instance_mask = port & SOC_REG_ADDR_INSTANCE_MASK;
        port &= (~SOC_REG_ADDR_INSTANCE_MASK);
    }
    

    if(!instance_mask) {
        if (port >= 0) {
            if (SOC_BLOCK_IN_LIST(unit, regblktype, portblktype)) {
                assert(SOC_PORT_VALID(unit, port));
                if (soc_feature(unit, soc_feature_logical_port_num)) {
                    /*
                     * COVERITY
                     *
                     * assert validates the port
                     */
                    /* coverity[overrun-local : FALSE] */
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                } else {
                    phy_port = port;
                }
                block = SOC_PORT_BLOCK(unit, phy_port);
                pindex = SOC_PORT_BINDEX(unit, phy_port);
            } else {
                block = pindex = -1; /* multiple non-port block */
            }
        } else if (port == REG_PORT_ANY) {
            block = pindex = -1;
            if(soc_portreg == SOC_REG_INFO(unit, reg).regtype ) {
                PBMP_ALL_ITER(unit, port) { /* try enabled ports */
                    if (soc_feature(unit, soc_feature_logical_port_num)) {
                        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                    } else {
                        phy_port = port;
                    }
                    block = SOC_PORT_BLOCK(unit, phy_port);
                    pindex = SOC_PORT_BINDEX(unit, phy_port);
                    if (SOC_BLOCK_IN_LIST(unit, regblktype, portblktype)) { /* match reg type */
                        if (SOC_BLOCK_IS_TYPE(unit, block, regblktype)) {
                            break;
                        }
                        block = -1;
                    } else { /* match any port */
                        break;
                    }
                }
                if (block < 0) {
                    assert(SOC_REG_ADDR_INVALID_PORT); /* invalid port */
                }
            }
        } else {
        port &= ~SOC_REG_ADDR_INSTANCE_MASK;
        block = pindex = -1;
        }
    }

        if (REG_PORT_ANY == port ||instance_mask || !SOC_BLOCK_IN_LIST(unit, regblktype, portblktype)) {
            switch (SOC_REG_FIRST_BLK_TYPE(regblktype)) {
            case SOC_BLK_ARL:
                block = ARL_BLOCK(unit);
                break;
            case SOC_BLK_IPIPE:
                block = IPIPE_BLOCK(unit);
                break;
            case SOC_BLK_IPIPE_HI:
                block = IPIPE_HI_BLOCK(unit);
                break;
            case SOC_BLK_EPIPE:
                block = EPIPE_BLOCK(unit);
                break;
            case SOC_BLK_EPIPE_HI:
                block = EPIPE_HI_BLOCK(unit);
                break;
            case SOC_BLK_IGR:
                block = IGR_BLOCK(unit);
                break;
            case SOC_BLK_EGR:
                block = EGR_BLOCK(unit);
                break;
            case SOC_BLK_BSE:
                block = BSE_BLOCK(unit);
                break;
            case SOC_BLK_CSE:
                block = CSE_BLOCK(unit);
                break;
            case SOC_BLK_HSE:
                block = HSE_BLOCK(unit);
                break;
            case SOC_BLK_BSAFE:
                block = BSAFE_BLOCK(unit);
                break;
            case SOC_BLK_OTPC:
                instance = 0;
                block = OTPC_BLOCK(unit, instance);
                break;
            case SOC_BLK_MMU:
                block = MMU_BLOCK(unit);
                break;
            case SOC_BLK_MCU:
                block = MCU_BLOCK(unit);
                break;
            case SOC_BLK_CMIC:
                block = CMIC_BLOCK(unit);
                break;
            case SOC_BLK_IPROC:
                block = IPROC_BLOCK(unit);
                break;
            case SOC_BLK_CRYPTO:
                block = CRYPTO_BLOCK(unit);
                break;
            case SOC_BLK_ESM:
                block = ESM_BLOCK(unit);
                break;
            case SOC_BLK_PORT_GROUP4:
                block = PG4_BLOCK(unit, port);
                break;
            case SOC_BLK_PORT_GROUP5:
                block = PG5_BLOCK(unit, port);
                break;
            case SOC_BLK_TOP:
                block = TOP_BLOCK(unit);
                break;
            case SOC_BLK_LLS:
                block = LLS_BLOCK(unit);
                break;
            case SOC_BLK_CES:
                block = CES_BLOCK(unit);
                break;
            case SOC_BLK_CI:
                if (port >= 3) {
                    assert(SOC_REG_ADDR_INVALID_PORT); /* invalid instance */
                } else {
                    block = CI_BLOCK(unit, port);
                }
                break;
            case SOC_BLK_IL:
                if (SOC_IS_SHADOW(unit)) {
                    if (port == 9) {
                        block = IL0_BLOCK(unit);
                    } else if (port == 13) {
                        block = IL1_BLOCK(unit);
                    }
                    pindex = 0;
                }
                break;
            case SOC_BLK_MS_ISEC:
                if (SOC_IS_SHADOW(unit)) {
                    if (port >= 1 && port <= 4) {
                        block = MS_ISEC0_BLOCK(unit);
                        pindex = port - 1;
                    } else {
                        block = MS_ISEC1_BLOCK(unit);
                        pindex = port - 5;
                    }
                }
                break;
            case SOC_BLK_MS_ESEC:
                if (SOC_IS_SHADOW(unit)) {
                    if (port >= 1 && port <= 4) {
                        block = MS_ESEC0_BLOCK(unit);
                        pindex = port - 1;
                    } else {
                        block = MS_ESEC1_BLOCK(unit);
                        pindex = port - 5;
                    }
                }
                break;
            case SOC_BLK_IECELL:
#if defined(BCM_METROLITE_SUPPORT)
                if (SOC_IS_METROLITE(unit)) {
                    soc_ml_iecell_port_reg_blk_idx_get(unit, port,
                        SOC_BLK_IECELL, &block, &pindex);
                } else
#endif
#if defined (BCM_SABER2_SUPPORT)
                if (SOC_IS_SABER2(unit)) {
                    soc_sb2_iecell_port_reg_blk_idx_get(unit, port,
                        SOC_BLK_IECELL, &block, &pindex);
                } 
#endif
                break;
            case SOC_BLK_TXLP:
#if defined(BCM_KATANA2_SUPPORT)
                if (SOC_IS_KATANA2(unit)) {
                    soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                        SOC_BLK_TXLP, &block, &pindex);
                }
#endif
                break;
            case SOC_BLK_RXLP:
#if defined(BCM_KATANA2_SUPPORT)
                if (SOC_IS_KATANA2(unit)) {
                    soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                        SOC_BLK_RXLP, &block, &pindex);
                }
#endif
                break;
            case SOC_BLK_OAMP:
#if defined(BCM_SABER2_SUPPORT) || defined (BCM_METROLITE_SUPPORT)
                block = OAMP_BLOCK(unit);
#endif
                break;

            case SOC_BLK_CW:
                if (SOC_IS_SHADOW(unit)) {
                    block = CW_BLOCK(unit);
                }
                break;
            case SOC_BLK_ECI:
                block = ECI_BLOCK(unit);
                break;
            case SOC_BLK_OCCG:
                block = OCCG_BLOCK(unit);
                break;
            case SOC_BLK_DCH:
                if(REG_PORT_ANY != port)
                    block = DCH_BLOCK(unit, port);
                else
                    block = DCH_BLOCK(unit, 0);
                break;
            case SOC_BLK_DCL:
                if(REG_PORT_ANY != port)
                    block = DCL_BLOCK(unit, port);
                else
                    block = DCL_BLOCK(unit, 0);
                break;
            case SOC_BLK_DCMA:
                if(REG_PORT_ANY != port)
                    block = DCMA_BLOCK(unit, port);
                else
                    block = DCMA_BLOCK(unit, 0);
                break;
            case SOC_BLK_DCMB:
                if(REG_PORT_ANY != port)
                    block = DCMB_BLOCK(unit, port);
                else
                    block = DCMB_BLOCK(unit, 0);
                break;
            case SOC_BLK_DCMC:
                block = DCMC_BLOCK(unit);
                break;
            case SOC_BLK_CCS:
                if(REG_PORT_ANY != port)
                    block = CCS_BLOCK(unit, port);
                else
                    block = CCS_BLOCK(unit, 0);
                break;
            case SOC_BLK_RTP:
                block = RTP_BLOCK(unit);
                break;
            case SOC_BLK_MESH_TOPOLOGY:
                block = MESH_TOPOLOGY_BLOCK(unit);
                break;
            case SOC_BLK_FMAC:
                if(REG_PORT_ANY != port)
                    block = FMAC_BLOCK(unit, port);
                else
                    block = FMAC_BLOCK(unit, 0);
                break;
            case SOC_BLK_IPSEC_SPU_WRAPPER_TOP:
                if(REG_PORT_ANY != port)
                    block = IPSEC_SPU_WRAPPER_TOP_BLOCK(unit, port);
                else
                    block = IPSEC_SPU_WRAPPER_TOP_BLOCK(unit, 0);
                break;
            case SOC_BLK_FSRD:
                if(REG_PORT_ANY != port)
                    block = FSRD_BLOCK(unit, port);
                else
                    block = FSRD_BLOCK(unit, 0);
                break;
            case SOC_BLK_HBC:
                if(REG_PORT_ANY != port)
                    block = HBC_BLOCK(unit, port);
                else
                    block = HBC_BLOCK(unit, 0);
                break;
            case SOC_BLK_BRDC_FMACH:
                block = BRDC_FMACH_BLOCK(unit);
                break;
            case SOC_BLK_BRDC_FMACL:
                block = BRDC_FMACL_BLOCK(unit);
                break;
            case SOC_BLK_BRDC_FSRD:
                block = BRDC_FSRD_BLOCK(unit);
                break;
            case SOC_BLK_BRDC_HBC:
                block = BRDC_HBC_BLOCK(unit);
                break;
            case SOC_BLK_MXQ:
                if(REG_PORT_ANY != port)
                    block = MXQ_BLOCK(unit, port);
                else
                    block = MXQ_BLOCK(unit, 0);
                break;
            case SOC_BLK_PLL:
                block = PLL_BLOCK(unit);
                break;
            case SOC_BLK_NIF:
                block = NIF_BLOCK(unit);
                break;
            default:
                    block = -1; /* unknown non-port block */
                    break;
            }
        }

    assert(block >= 0); /* block must be valid */

    /* determine final block, pindex, and index */
    gransh = 0;
    switch (SOC_REG_INFO(unit, reg).regtype) {
    case soc_cpureg:
    case soc_mcsreg:
    case soc_iprocreg:
        block = -1;
        pindex = 0;
        gransh = 2; /* 4 byte granularity */
        break;
    case soc_portreg:
        if (!SOC_BLOCK_IN_LIST(unit, regblktype, portblktype) &&
            !(SOC_IS_SHADOW(unit) &&
             (SOC_BLOCK_IS(regblktype, SOC_BLK_MS_ISEC) || 
              SOC_BLOCK_IS(regblktype, SOC_BLK_MS_ESEC)))
#if defined(BCM_KATANA2_SUPPORT)
              && !(SOC_IS_KATANA2(unit) &&
             (SOC_BLOCK_IS(regblktype, SOC_BLK_TXLP) || 
              SOC_BLOCK_IS(regblktype, SOC_BLK_RXLP)))
#endif
/* Need to check : SABER2 */
#if defined(BCM_SABER2_SUPPORT)
              && !(SOC_IS_SABER2(unit) &&
             (SOC_BLOCK_IS(regblktype, SOC_BLK_IECELL)))
#endif
/* Need to check : FIRELIGHT */
#if defined(BCM_FIRELIGHT_SUPPORT)
              && !(SOC_IS_GREYHOUND2(unit) &&
             (SOC_BLOCK_IS(regblktype, SOC_BLK_MACSEC)))
#endif
            ) {
            if (soc_feature(unit, soc_feature_logical_port_num) &&
                block == MMU_BLOCK(unit)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                /*MMU register with device port indexing*/
                num_regs = sizeof(reg_excep_list) / sizeof(soc_reg_t);
                for (reg_index = 0;reg_index < num_regs; reg_index++) {
                    if (reg_excep_list[reg_index] == reg) {
                        add_exception = 1;
                        break;
                    }
                }
                if (add_exception) {
                            pindex = port;
                } else
#endif
                {
                /* coverity[negative_returns : FALSE] */
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                pindex = SOC_INFO(unit).port_p2m_mapping[phy_port];
                }
            } else {
                pindex = port;
            }
        }
        break;
    case soc_cosreg:
        assert(index >= 0 && index < NUM_COS(unit));
        pindex = index;
        index = 0;
        break;
    case soc_customreg:
    case soc_genreg:
        pindex = 0;
        break;
    default:
        assert(0); /* unknown register type */
        break;
    }

    /* put together address: base|block|pindex + index */
    base = SOC_REG_INFO(unit, reg).offset;
    LOG_VERBOSE(BSL_LS_SOC_REG,
                (BSL_META_U(unit,
                            "base: %x "), base));
        
    if (block >= 0) {
        base |= ((SOC_BLOCK2OFFSET(unit, block) & 0xf) << SOC_BLOCK_BP) |
                 (((SOC_BLOCK2OFFSET(unit, block) >> 4) & 0x3) <<
                 SOC_BLOCK_MSB_BP);
    }
    
    if (pindex) {
        base |= pindex << SOC_REGIDX_BP;
    }
    
    if (SOC_REG_IS_ARRAY(unit, reg)) {
        assert(index >= SOC_REG_INFO(unit, reg).first_array_index && index < SOC_REG_NUMELS(unit, reg) + SOC_REG_INFO(unit, reg).first_array_index);
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit) && block == OAMP_BLOCK(unit)) {
            base += (index - SOC_REG_INFO(unit, reg).first_array_index)*(SOC_REG_ELEM_SKIP(unit, reg) << 8);
        } else
#endif
        {
        base += (index - SOC_REG_INFO(unit, reg).first_array_index)*SOC_REG_ELEM_SKIP(unit, reg);
        }
    } else if (index && SOC_REG_ARRAY(unit, reg)) {
        assert(index >= 0 && index < SOC_REG_NUMELS(unit, reg));
        if (index && SOC_REG_ARRAY2(unit, reg)) {
            base += ((index*2) << gransh);
        } else if (index && SOC_REG_ARRAY4(unit, reg)) {
            base += ((index * 4) << gransh);
        } else {
            base += (index << gransh);
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_REG,
                (BSL_META_U(unit,
                            "addr: %x, block: %d, index: %d, pindex: %d, gransh: %d\n"),
                 base, block, index, pindex, gransh));
    return base;
}

/*
 * Function:    soc_reg_xaddr_get
 * Purpose:     calculate the address of a register
 * Parameters:
 *     unit       - SOC unit number
 *     reg        - Register number
 *     port       - Port number or REG_PORT_ANY or SOC_CORE_ALL for DNX blockswith an instance per core
 *     index      - Array index (or cos number)
 *     options    - Flag to indicate special handling to calculate
 *                  the HW register address: SOC_REG_ADDR_OPTION_xxx
 *     access_info- (OUT) Register access info: address, schan block IDs, access type
 *
 * Returns:     success
 * Notes:       the block number/s to access is determined by the register
 *              and the port number
 */
int soc_reg_xaddr_get(int unit, soc_reg_t reg, int port, int index,
                      uint32 options, soc_reg_access_info_t *access_info)
{
    uint32            base;        /* base address from reg_info */
    int               block = -1;  /* block number */
    int               pindex = -1; /* register port/cos field */
    int               gransh;      /* index granularity shift */
    soc_block_types_t regblktype;
    soc_block_t       portblktype;
    uint32            phy_port = 0;
    int               instance = -1, i;
    int               instance_mask = 0, block_id_mask = 0, schan_id_mask = 0, phy_acc_mask = 0;
    int               port_num_blktype;
    int               block_core = 0;
    int               is_write;
    int               preserve_port;
    int               *soc_blocks = NULL;
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_reg_t reg_excep_list[]={MMU_CRB_DEVICE_PORT_TO_MMU_PORT_MAPPINGr,
                                MMU_RQE_REPL_PORT_AGG_MAPr};
    int reg_index, num_regs, add_exception = 0;
    int               stage_id;
#endif
    
    COMPILER_REFERENCE(is_write);

    if (SOC_CONTROL(unit)->soc_reg_watch) {
        if (SOC_CONTROL(unit)->prev_reg != reg) {
            LOG_CLI((BSL_META_U(unit, "%d:%s REG: %s\n"),
                       unit, BSL_FUNC, SOC_REG_NAME(unit, reg)));
            SOC_CONTROL(unit)->prev_reg = reg;
        }
    }


    if (!soc_feature(unit, soc_feature_new_sbus_format)) {
        access_info->offset = soc_reg_addr(unit, reg, port, index);
        access_info->num_blks = 0;
        return SOC_E_NONE;
    }
    access_info->num_blks = 1;
    
    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

#define SOC_REG_ADDR_INVALID_PORT 0 /* for asserts */

    access_info->acc_type = SOC_REG_ACC_TYPE(unit, reg);

    portblktype = SOC_BLK_PORT;
    port_num_blktype = SOC_DRIVER(unit)->port_num_blktype > 1 ?
        SOC_DRIVER(unit)->port_num_blktype : 1;

    access_info->blk_list[0] = 0;/* not really needed, just to avoid coverity defect */

    /* Get options */
    is_write = options & SOC_REG_ADDR_OPTION_WRITE;
    preserve_port = options & SOC_REG_ADDR_OPTION_PRESERVE_PORT;

    if ((REG_PORT_ANY != port) && (SOC_CORE_ALL != port)) {
        instance_mask = port & SOC_REG_ADDR_INSTANCE_MASK;
        block_id_mask = port & SOC_REG_ADDR_BLOCK_ID_MASK;
        schan_id_mask = port & SOC_REG_ADDR_SCHAN_ID_MASK;
        phy_acc_mask = port & SOC_REG_ADDR_PHY_ACC_MASK;
        port &= (~ (SOC_REG_ADDR_INSTANCE_MASK | SOC_REG_ADDR_BLOCK_ID_MASK 
                    | SOC_REG_ADDR_SCHAN_ID_MASK | SOC_REG_ADDR_PHY_ACC_MASK));
    }

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    /* Use core broadcast writes when possible for more efficient writes */
    if (SOC_IS_ARAD(unit) || SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        instance = block_core = (port == SOC_CORE_ALL || port == REG_PORT_ANY) ?
                     (is_write ? SOC_CORE_ALL : 0) : port;
    }
#endif /* BCM_PETRA_SUPPORT || BCM_DNX_SUPPORT */
    
    regblktype = SOC_REG_INFO(unit, reg).block;

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit) && (soc_customreg == SOC_REG_INFO(unit,reg).regtype)
        && (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_ILE)
        && (port != SOC_CORE_ALL) && (port != SOC_BLOCK_ALL)) {
        /* for custom register, ILKN, the instance is meaningless */
        instance = block_core = 0;
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
       /* Since TH3 has the dummy blocks CDMAC defined in the regsfile, we need
          to assign them to the CDPORT block */
        if (regblktype[0] == SOC_BLK_CDMAC) {
            regblktype[0] = SOC_BLK_CDPORT;
        }
    }
#endif

    if (!block_id_mask && !schan_id_mask && !instance_mask && port >= 0) {
        if (SOC_BLOCK_IN_LIST(unit, regblktype, portblktype)) {
            if (preserve_port || phy_acc_mask) {
                phy_port = port;

#ifdef BCM_JERICHO_SUPPORT
                if (SOC_IS_JERICHO(unit)) {
                    /* translate phy port to phy port with qsgmii offset */
                    SOC_IF_ERROR_RETURN(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_add, (unit, phy_port, &phy_port)));
                }
#endif /* BCM_JERICHO_SUPPORT */

            } else {
                assert(SOC_PORT_VALID(unit, port)); 
                if (soc_feature(unit, soc_feature_logical_port_num)) {
                    /*
                     * COVERITY
                     *
                     * assert validates the port
                     */
                    /* coverity[overrun-local : FALSE] */ 
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                } else {
                    phy_port = port;
                }
            }
            for (i = 0; i < port_num_blktype; i++) {
#ifdef BCM_KATANA2_SUPPORT
                /* Override port blocks with Linkphy Blocks.. */
                if(SOC_IS_KATANA2(unit) &&
                    (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_TXLP) ) {
                    soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                        SOC_BLK_TXLP, &block, &pindex);
                    break;
                } else if(SOC_IS_KATANA2(unit) &&
                    (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_RXLP) ) {
                    soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                        SOC_BLK_RXLP, &block, &pindex);
                    break;
#ifdef BCM_METROLITE_SUPPORT
                } else if((SOC_IS_METROLITE(unit)) &&
                    (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_IECELL)) {
                    soc_ml_iecell_port_reg_blk_idx_get(unit, port,
                        SOC_BLK_IECELL, &block, &pindex);
                    break;
#endif
#if defined (BCM_SABER2_SUPPORT)
                } else if((SOC_IS_SABER2(unit)) &&
                    (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_IECELL)) {
                    soc_sb2_iecell_port_reg_blk_idx_get(unit, port,
                        SOC_BLK_IECELL, &block, &pindex);
                    break;
#endif
                }
#endif
#ifdef BCM_GREYHOUND_SUPPORT
                if(SOC_IS_GREYHOUND(unit) &&
                    (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_XLPORT) ) {
                    if (soc_greyhound_pgw_reg_blk_index_get(unit, reg, port, NULL,
                        &block, &pindex, 0) > 0){
                        break;
                    }                        
                }
#endif
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit) &&
                    (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_MACSEC)) {
                    soc_fl_macsec_port_reg_blk_idx_get(unit, phy_port,
                        SOC_BLK_MACSEC, &block, &pindex);
                    break;
               }
#endif
                block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);

#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit)) {
                    SOC_IF_ERROR_RETURN(soc_apache_port_reg_blk_index_get
                                (unit, port, SOC_REG_FIRST_BLK_TYPE(regblktype), &block));
                }
#endif
                if (block < 0) {
                    break;
                }
                if (SOC_BLOCK_IN_LIST(unit, regblktype,
                                      SOC_BLOCK_TYPE(unit, block))) {
                    pindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                    break;
                }
                
                else if (SOC_IS_JERICHO(unit) && SOC_PORT_IDX_BLOCK(unit, phy_port, i + 1) == -1) {
                    break;
                }
                else if (SOC_IS_DNX(unit)) {
                    if( ((regblktype[0] == SOC_BLK_CDMAC) && (SOC_BLOCK_TYPE(unit, block) == SOC_BLK_CDPORT)) ||
                        ((regblktype[0] == SOC_BLK_CLMAC) && (SOC_BLOCK_TYPE(unit, block) == SOC_BLK_CLPORT))) {
                        /*
                         * DNX only: get pindex, in case of CDMAC.
                         * SOC_PORT_IDX_BLOCK returns CDPORT, therefore check type of block is CDPORT.
                         */
                        pindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                        break;
                    }
                }
            }
        } else {
            block = pindex = -1; /* multiple non-port block */
        }
    } else if (port == REG_PORT_ANY) {
        block = pindex = -1;
        if (SOC_BLOCK_IN_LIST(unit, regblktype, portblktype)) {
            PBMP_ALL_ITER(unit, port) { /* try enabled ports */
                if (soc_feature(unit, soc_feature_logical_port_num)) {
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                } else {
                    phy_port = port;
                }
                for (i = 0; i < port_num_blktype; i++) {
                    block = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
                    if (block < 0) {
                        break;
                    }
                    if (SOC_BLOCK_IN_LIST
                        (unit, regblktype, SOC_BLOCK_TYPE(unit, block))) {
                        pindex = SOC_PORT_IDX_BINDEX(unit, phy_port,
                                                     i);
                        break;
                    }
                }
                if (i == port_num_blktype) {
                    continue;
                }
                if (block >= 0) {
                    break;
                }
            }
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(unit) &&
                    (SOC_REG_FIRST_BLK_TYPE(regblktype) == SOC_BLK_MACSEC)) {
                    soc_fl_macsec_port_reg_blk_idx_get(unit, REG_PORT_ANY,
                        SOC_BLK_MACSEC, &block, &pindex);
               }
#endif
            if (block < 0) {
                assert(SOC_REG_ADDR_INVALID_PORT); /* invalid port */
            }
        } else { /* match any port */
            if (!SOC_IS_SAND(unit)) {
                PBMP_ALL_ITER(unit, port) { /* try enabled ports */
                    break;
                }
            }
        }
    } else if (port != SOC_CORE_ALL){
        port &= ~(SOC_REG_ADDR_INSTANCE_MASK | SOC_REG_ADDR_BLOCK_ID_MASK | SOC_REG_ADDR_SCHAN_ID_MASK);
        instance = port;
        block = pindex = -1;
    }
    
#if defined (BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        if (((SOC_BLK_RXLP == SOC_REG_FIRST_BLK_TYPE(regblktype)) || 
             (SOC_BLK_TXLP == SOC_REG_FIRST_BLK_TYPE(regblktype))) &&
            (soc_portreg != SOC_REG_INFO(unit, reg).regtype)) {
            instance_mask = 1;
        }
    }
#endif
#if defined (BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        if ((SOC_BLK_IECELL == SOC_REG_FIRST_BLK_TYPE(regblktype)) &&
            (soc_portreg != SOC_REG_INFO(unit, reg).regtype)) {
            instance_mask = 1;
        }
    }
#endif

    if ((!block_id_mask) && (!schan_id_mask) && (REG_PORT_ANY == port || instance_mask || !SOC_BLOCK_IN_LIST(unit, regblktype, portblktype))) {
        int blkport = port;
        if (port == REG_PORT_ANY || port == SOC_CORE_ALL) {
            blkport = 0;
        }
        switch (SOC_REG_FIRST_BLK_TYPE(regblktype)) {
        case SOC_BLK_ARL:
            block = ARL_BLOCK(unit);
            break;
        case SOC_BLK_IPIPE:
            block = IPIPE_BLOCK(unit);
            break;
        case SOC_BLK_IPIPE_HI:
            block = IPIPE_HI_BLOCK(unit);
            break;
        case SOC_BLK_EPIPE:
            block = EPIPE_BLOCK(unit);
            break;
        case SOC_BLK_EPIPE_HI:
            block = EPIPE_HI_BLOCK(unit);
            break;
        case SOC_BLK_IGR:
            block = IGR_BLOCK(unit);
            break;
        case SOC_BLK_EGR:
            block = EGR_BLOCK(unit);
            break;
        case SOC_BLK_BSE:
            block = BSE_BLOCK(unit);
            break;
        case SOC_BLK_CSE:
            block = CSE_BLOCK(unit);
            break;
        case SOC_BLK_HSE:
            block = HSE_BLOCK(unit);
            break;
        case SOC_BLK_SYS:
            break;
        case SOC_BLK_BSAFE:
            block = BSAFE_BLOCK(unit);
            break;
        case SOC_BLK_OTPC:
            block = OTPC_BLOCK(unit, blkport);
            break;
        case SOC_BLK_MMU:
            block = MMU_BLOCK(unit);
            break;
        case SOC_BLK_MMU_SED:
            block = MMU_SED_BLOCK(unit);
            break;
        case SOC_BLK_MMU_GLB:
            block = MMU_GLB_BLOCK(unit);
            break;
        case SOC_BLK_MMU_XPE:
            block = MMU_XPE_BLOCK(unit);
            break;
        case SOC_BLK_MMU_SC:
            block = MMU_SC_BLOCK(unit);
            break;
        case SOC_BLK_MCU:
            block = MCU_BLOCK(unit);
            break;
        case SOC_BLK_CMIC:
            block = CMIC_BLOCK(unit);
            break;
        case SOC_BLK_IPROC:
            block = IPROC_BLOCK(unit);
            break;
        case SOC_BLK_CRYPTO:
            block = CRYPTO_BLOCK(unit);
            break;
        case SOC_BLK_ESM:
            block = ESM_BLOCK(unit);
            break;
        case SOC_BLK_PORT_GROUP4:
            block = PG4_BLOCK(unit, port);
            break;
        case SOC_BLK_PORT_GROUP5:
            block = PG5_BLOCK(unit, port);
            break;
        case SOC_BLK_TOP:
            block = TOP_BLOCK(unit);
            break;
        case SOC_BLK_SER:
            block = SER_BLOCK(unit);
            break;
        case SOC_BLK_AVS:
            block = AVS_BLOCK(unit);
            break;
        case SOC_BLK_AXP:
            block = AXP_BLOCK(unit);
            break;
        case SOC_BLK_ISM:
            block = ISM_BLOCK(unit);
            break;
        case SOC_BLK_ETU:
            block = ETU_BLOCK(unit);
            break;
        case SOC_BLK_ETU_WRAP:
            block = ETU_WRAP_BLOCK(unit);
            break;
        case SOC_BLK_IBOD:
            block = IBOD_BLOCK(unit);
            break;
        case SOC_BLK_LLS:
            block = LLS_BLOCK(unit);
            break;
        case SOC_BLK_CES:
            block = CES_BLOCK(unit);
            break;
        case SOC_BLK_PGW_CL:
            if (instance_mask) {
                instance = port;
            } else{
                /* coverity[overrun-local : FALSE] */
                instance = SOC_INFO(unit).port_group[port];
            }
            block = PGW_CL_BLOCK(unit, instance);
            break;
        case SOC_BLK_PMQPORT:
        case SOC_BLK_PMQ:
            /* coverity[overrun-local : FALSE] */
            block = PMQ_BLOCK(unit, blkport);
            break;
#ifdef BCM_HURRICANE4_SUPPORT
        case SOC_BLK_QTGPORT:
            block = QTGPORT_BLOCK(unit, blkport);
            break;
#endif /* BCM_HURRICANE4_SUPPORT */
        case SOC_BLK_PGW_GE:
            block = PGW_GE_BLOCK(unit, instance);
            break;
        case SOC_BLK_IL:
            if (SOC_IS_SHADOW(unit)) {
                if (port == 9) {
                    block = IL0_BLOCK(unit);
                } else if (port == 13) {
                    block = IL1_BLOCK(unit);
                }
                pindex = 0;
            }
            break;
        case SOC_BLK_MS_ISEC:
            if (SOC_IS_SHADOW(unit)) {
                if (port >= 1 && port <= 4) {
                    block = MS_ISEC0_BLOCK(unit);
                    pindex = port - 1;
                } else {
                    block = MS_ISEC1_BLOCK(unit);
                    pindex = port - 5;
                }
            }
            break;
        case SOC_BLK_MS_ESEC:
            if (SOC_IS_SHADOW(unit)) {
                if (port >= 1 && port <= 4) {
                    block = MS_ESEC0_BLOCK(unit);
                    pindex = port - 1;
                } else {
                    block = MS_ESEC1_BLOCK(unit);
                    pindex = port - 5;
                }
            }
            break;
        case SOC_BLK_CW:
            if (SOC_IS_SHADOW(unit)) {
                block = CW_BLOCK(unit);
            }
            break;
        case SOC_BLK_CM:
            break;
        case SOC_BLK_CO:
            break;
        case SOC_BLK_CI:
            if (SOC_IS_KATANA2(unit)) {
                block = CI_BLOCK(unit, port);
            }
            break;
        case SOC_BLK_CX:
            break;
        case SOC_BLK_LRA:
            break;
        case SOC_BLK_LRB:
            break;
        case SOC_BLK_OC:
            break;
        case SOC_BLK_PB:
            break;
        case SOC_BLK_PD:
            break;
        case SOC_BLK_PP:
            break;
        case SOC_BLK_PR:
            break;
        case SOC_BLK_PT:
            break;
        case SOC_BLK_QM:
            break;
        case SOC_BLK_RC:
            break;
        case SOC_BLK_TMA:
            break;
        case SOC_BLK_TMB:
            break;
        case SOC_BLK_TM_QE:
            break;
        case SOC_BLK_TP:
            break;
#if defined(BCM_KATANA2_SUPPORT)
        case SOC_BLK_TXLP:
            if (SOC_IS_KATANA2(unit)) {
                soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                    SOC_BLK_TXLP, &block, &pindex);
            }
            break;
        case SOC_BLK_RXLP:
            if (SOC_IS_KATANA2(unit)) {
                soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                    SOC_BLK_RXLP, &block, &pindex);
            }
            break;
#endif
        case SOC_BLK_IECELL:
#if defined(BCM_METROLITE_SUPPORT)
            if (SOC_IS_METROLITE(unit)) {
                soc_ml_iecell_port_reg_blk_idx_get(unit, port,
                    SOC_BLK_IECELL, &block, &pindex);
            }
#endif
#if defined (BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit)) {
                soc_sb2_iecell_port_reg_blk_idx_get(unit, port,
                    SOC_BLK_IECELL, &block, &pindex);
            }
#endif
            break;
            /* DPP */
        case SOC_BLK_CFC:
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) && block_core == SOC_CORE_ALL) {
                block = SOC_CORE_ALL;
                soc_blocks = SOC_INFO(unit).cfc_block;
            } else
#endif
            {
        	block = CFC_BLOCK(unit, blkport);
            }
            break;
        case SOC_BLK_OCB:
        	block = OCB_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ocb_blocks;
            break;
        case SOC_BLK_CRPS:
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) && block_core == SOC_CORE_ALL) {
                block = SOC_CORE_ALL;
                soc_blocks = SOC_INFO(unit).crps_blocks;
            } else
#endif
            {
            block = CRPS_BLOCK(unit, blkport);
            }
            break;
        case SOC_BLK_EPRE:
        	block = EPRE_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).epre_blocks;
            break;
        case SOC_BLK_IPPF:
        	block = IPPF_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ippf_blocks;
            break;
        case SOC_BLK_MDB_ARM:
        	block = MDB_ARM_BLOCK(unit);
            break;
        case SOC_BLK_CDPORT:
        	block = CDPORT_BLOCK(unit, blkport);
            break;
#ifdef BCM_DNX_SUPPORT
        case SOC_BLK_CLPORT:
            block = CLPORT_BLOCK(unit, blkport);
            break;
#endif
        case SOC_BLK_CDMAC:
            block = CDMAC_BLOCK(unit, blkport);
            break;
        case SOC_BLK_FDTL:
            block = FDTL_BLOCK(unit, blkport);
            break;
        case SOC_BLK_ECI:
            block = ECI_BLOCK(unit);
            break;
        case SOC_BLK_EGQ:
            block = EGQ_BLOCK(unit, block_core);
            break;
        case SOC_BLK_FCR:
            block = FCR_BLOCK(unit);
            break;
        case SOC_BLK_FCT:
            block = FCT_BLOCK(unit);
            break;
        case SOC_BLK_FDR:
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) && block_core == SOC_CORE_ALL) {
                block = SOC_CORE_ALL;
                soc_blocks = SOC_INFO(unit).fdr_blocks;
            } else
#endif
            {
            block = FDR_BLOCK(unit, blkport);
            }
            break;
        case SOC_BLK_FDA:
            block = FDA_BLOCK(unit, blkport);
            break;
        case SOC_BLK_FDT:
            block = FDT_BLOCK(unit, blkport);
            break;
        case SOC_BLK_MESH_TOPOLOGY:
            block = MESH_TOPOLOGY_BLOCK(unit);
            break;
        case SOC_BLK_IDR:
            block = IDR_BLOCK(unit);
            break;
        case SOC_BLK_IHB:
            block = IHB_BLOCK(unit, block_core);
            break;
        case SOC_BLK_IHP:
            block = IHP_BLOCK(unit, block_core);
            break;
        case SOC_BLK_IPS:
            block = IPS_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ips_blocks;
            break;
        case SOC_BLK_IPT:
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) && block_core == SOC_CORE_ALL) {
                block = SOC_CORE_ALL;
                soc_blocks = SOC_INFO(unit).ipt_blocks;
            } else
#endif
            {
            block = IPT_BLOCK(unit, blkport);
            }
            break;
        case SOC_BLK_IQM:
            block = IQM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).iqm_blocks;
            break;
        case SOC_BLK_PQP:
            block = PQP_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).pqp_blocks;
            break;
        case SOC_BLK_KAPS:
            block = KAPS_BLOCK(unit, blkport);
            break;
        case SOC_BLK_KAPS_BBS:
            block = KAPS_BBS_BLOCK(unit,instance);
            break;
        case SOC_BLK_ILB:
            block = ILB_BLOCK(unit);
            break;
        case SOC_BLK_IEP:
            block = IEP_BLOCK(unit);
            break;
        case SOC_BLK_IMP:
            block = IMP_BLOCK(unit);
            break;
        case SOC_BLK_SPB:
            block = SPB_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).spb_blocks;
            break;
        case SOC_BLK_ITE:
            block = ITE_BLOCK(unit);
            break;
        case SOC_BLK_DDP:
#ifdef BCM_DNX_SUPPORT
            if (block_core == SOC_CORE_ALL && SOC_IS_DNX(unit)) {
                block = SOC_CORE_ALL;
                soc_blocks = SOC_INFO(unit).ddp_blocks;
            } else
#endif
            {
            block = DDP_BLOCK(unit, blkport);
            }
            break;
        case SOC_BLK_TXQ:
            block = TXQ_BLOCK(unit);
            break;
        case SOC_BLK_TAR:
            block = TAR_BLOCK(unit, blkport);
            break;
        case SOC_BLK_PTS:
            block = PTS_BLOCK(unit);
            break;
        case SOC_BLK_SQM:
            block = SQM_BLOCK(unit, blkport);
            break;
        case SOC_BLK_IPSEC:
            block = IPSEC_BLOCK(unit);
            break;
        case SOC_BLK_IPSEC_SPU_WRAPPER_TOP:
            block = IPSEC_SPU_WRAPPER_TOP_BLOCK(unit, instance);
            break;
        case SOC_BLK_DQM:
            block = DQM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).dqm_blocks;
            break;
        case SOC_BLK_ECGM:
            block = ECGM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ecgm_blocks;
            break;
        case SOC_BLK_IDB:
            block = IDB_BLOCK(unit);
            break;
        case SOC_BLK_PEM:
            block = PEM_BLOCK(unit, blkport);
            break;

        case SOC_BLK_IRE:
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) && block_core == SOC_CORE_ALL) {
                block = SOC_CORE_ALL;
                soc_blocks = SOC_INFO(unit).ire_blocks;
            } else
#endif
            {
            block = IRE_BLOCK(unit, blkport);
            }
            break;
        case SOC_BLK_IRR:
            block = IRR_BLOCK(unit);
            break;
        case SOC_BLK_FMAC:
            block = FMAC_BLOCK(unit, blkport);
            break;
        case SOC_BLK_XLP:
            block = XLP_BLOCK(unit, blkport);
            break;
        case SOC_BLK_CLP:
            block = CLP_BLOCK(unit, blkport);
            break;
        case SOC_BLK_NBI:
            block = NBI_BLOCK(unit);
            break;
        case SOC_BLK_CGM:
            block = CGM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).cgm_blocks;
            break;
        case SOC_BLK_OAMP:
            block = OAMP_BLOCK(unit);
            break;
        case SOC_BLK_OLP:
            block = OLP_BLOCK(unit, blkport);
            break;
        case SOC_BLK_SIF:
            block = SIF_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).sif_blocks;
            break;
        case SOC_BLK_MCP:
            block = MCP_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).mcp_blocks;
            break;
        case SOC_BLK_ITPP:
            block = ITPP_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).itpp_blocks;
            break;
        case SOC_BLK_ITPPD:
            block = ITPPD_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).itppd_blocks;
            break;
        case SOC_BLK_PDM:
            block = PDM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).pdm_blocks;
            break;
        case SOC_BLK_BDM:
            block = BDM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).bdm_blocks;
            break;
        case SOC_BLK_CDU:
            block = CDU_BLOCK(unit, blkport);
            break;
        case SOC_BLK_CDUM:
            block = CDUM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).cdum_blocks;
            break;
        case SOC_BLK_EPS:
            block = EPS_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).eps_blocks;
            break;

        case SOC_BLK_DDHA:
            block = DDHA_BLOCK(unit, blkport);
            break;

        case SOC_BLK_DDHB:
            block = DDHB_BLOCK(unit, blkport);
            break;

        case SOC_BLK_DHC:
            block = DHC_BLOCK(unit, blkport);
            break;

        case SOC_BLK_DMU:
            block = DMU_BLOCK(unit, blkport);
            break;

        case SOC_BLK_ETPPC:
            block = ETPPC_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).etppc_blocks;
            break;

        case SOC_BLK_EVNT:
            block = EVNT_BLOCK(unit, blkport);
            break;

        case SOC_BLK_HBC:
            block = HBC_BLOCK(unit, blkport);
            break;

        case SOC_BLK_ILE:
            block = ILE_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ile_blocks;
            break;
        case SOC_BLK_CLU:
            block = CLU_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).clu_blocks;
            break;
        case SOC_BLK_CLUP:
            block = CLUP_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).clup_blocks;
            break;
        case SOC_BLK_CDB:
            block = CDB_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).cdb_blocks;
            break;
        case SOC_BLK_CDBM:
            block = CDBM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).cdbm_blocks;
            break;
        case SOC_BLK_CDPM:
            block = CDPM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).cdpm_blocks;
            break;
        case SOC_BLK_OCBM:
            block = OCBM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ocbm_blocks;
            break;
        case SOC_BLK_MSD:
            block = MSD_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).msd_blocks;
            break;
        case SOC_BLK_MSS:
            block = MSS_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).mss_blocks;
            break;
        case SOC_BLK_CLMAC:
            block = CLMAC_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).clmac_blocks;
            break;
        case SOC_BLK_ESB:
            block = ESB_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).esb_blocks;
            break;
        case SOC_BLK_ILU:
            block = ILU_BLOCK(unit, blkport);
            break;
        case SOC_BLK_NMG:
            block = NMG_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).nmg_blocks;
            break;

        case SOC_BLK_IPPE:
            block = IPPE_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ippe_blocks;
            break;

        case SOC_BLK_TCAM:
            block = TCAM_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).tcam_blocks;
            break;

        case SOC_BLK_TDU:
            block = TDU_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).tdu_blocks;
            break;

        case SOC_BLK_MTM:
            block = MTM_BLOCK(unit, blkport);
            break;
        case SOC_BLK_HBM:
            block = HBM_BLOCK(unit, blkport);
            break;
        case SOC_BLK_RQP:
            block = RQP_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).rqp_blocks;
            break;
        case SOC_BLK_FQP:
            block = FQP_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).fqp_blocks;
            break;
        case SOC_BLK_HBMC:
            block = HBMC_BLOCK(unit, blkport);
            break;
        case SOC_BLK_MDB:
            block = MDB_BLOCK(unit);
            break;
        case SOC_BLK_HRC:
            block = HRC_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).hrc_blocks;
            break;
        case SOC_BLK_ERPP:
            block = ERPP_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).erpp_blocks;
            break;
        case SOC_BLK_ETPPA:
            block = ETPPA_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).etppa_blocks;
            break;
        case SOC_BLK_ETPPB:
            block = ETPPB_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).etppb_blocks;
            break;
        case SOC_BLK_MACT:
            block = MACT_BLOCK(unit, blkport);
            break;
        case SOC_BLK_IPPA:
            block = IPPA_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ippa_blocks;
            break;
        case SOC_BLK_IPPB:
            block = IPPB_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ippb_blocks;
            break;
        case SOC_BLK_IPPC:
            block = IPPC_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ippc_blocks;
            break;
        case SOC_BLK_IPPD:
            block = IPPD_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).ippd_blocks;
            break;
        case SOC_BLK_FSRD:
            block = FSRD_BLOCK(unit, blkport);
            break;
        case SOC_BLK_RTP:
            block = RTP_BLOCK(unit);
            break;
        case SOC_BLK_SCH:
            block = SCH_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).sch_blocks;
            break;
        case SOC_BLK_EPNI:
            block = EPNI_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).epni_blocks;
            break;
        case SOC_BLK_DRCA:
            block = DRCA_BLOCK(unit);
            break;
        case SOC_BLK_DRCB:
            block = DRCB_BLOCK(unit);
            break;
        case SOC_BLK_DRCC:
            block = DRCC_BLOCK(unit);
            break;
        case SOC_BLK_DRCD:
            block = DRCD_BLOCK(unit);
            break;
        case SOC_BLK_DRCE:
            block = DRCE_BLOCK(unit);
            break;
        case SOC_BLK_DRCF:
            block = DRCF_BLOCK(unit);
            break;
        case SOC_BLK_DRCG:
            block = DRCG_BLOCK(unit);
            break;
        case SOC_BLK_DRCH:
            block = DRCH_BLOCK(unit);
            break;
        case SOC_BLK_EDB:
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) && block_core == SOC_CORE_ALL) {
                block = SOC_CORE_ALL;
                soc_blocks = SOC_INFO(unit).edb_blocks;
            } else
#endif
            {
            block = EDB_BLOCK(unit, blkport);
            }
            break;
        case SOC_BLK_ILKN_PMH:
            block = ILKN_PMH_BLOCK(unit);
            break;
        case SOC_BLK_IPST:
            block = IPST_BLOCK(unit);
            break;
        case SOC_BLK_IQMT:
            block = IQMT_BLOCK(unit);
            break;
        case SOC_BLK_PPDB_A:
            block = PPDB_A_BLOCK(unit);
            break;
        case SOC_BLK_PPDB_B:
            block = PPDB_B_BLOCK(unit);
            break;
        case SOC_BLK_ILKN_PML:
            block = ILKN_PML_BLOCK(unit, blkport);
            break;
        case SOC_BLK_MRPS:
            block = MRPS_BLOCK(unit, instance);
            soc_blocks = SOC_INFO(unit).mrps_blocks;
            break;
        case SOC_BLK_MTRPS_EM:
            block = MTRPS_EM_BLOCK(unit, instance);
            break;
        case SOC_BLK_NBIL:
            block = NBIL_BLOCK(unit, blkport);
            break;
        case SOC_BLK_NBIH:
            block = NBIH_BLOCK(unit);
            break;
        case SOC_BLK_DRCBROADCAST:
            block = DRCBROADCAST_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_FSRD:
            block = BRDC_FSRD_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_FMAC:
            block = BRDC_FMAC_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_HBC:
            block = BRDC_HBC_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_CCH:
            block = BRDC_CCH_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_CGM:
            block = BRDC_CGM_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_EGQ:
            block = BRDC_EGQ_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_EPNI:
            block = BRDC_EPNI_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_IHB:
            block = BRDC_IHB_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_IHP:
            block = BRDC_IHP_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_IPS:
            block = BRDC_IPS_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_IQM:
            block = BRDC_IQM_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_SCH:
            block = BRDC_SCH_BLOCK(unit);
            break;
#ifdef DNX_TEST_CHIPS_SUPPORT
        case SOC_BLK_PRM:
            block = PRM_BLOCK(unit);
            break;
        case SOC_BLK_BLH:
            block = BLH_BLOCK(unit, blkport);
            break;
        case SOC_BLK_AM_TOP:
            block = AM_TOP_BLOCK(unit);
            break;
#endif /* DNX_TEST_CHIPS_SUPPORT */
        /* DFE  blocks*/
        case SOC_BLK_DCH:
            block=DCH_BLOCK(unit,blkport);
            break;
        case SOC_BLK_DCL:
            block=DCL_BLOCK(unit,blkport);
            break;
        case SOC_BLK_OCCG:
            block=OCCG_BLOCK(unit);
            break;
        case SOC_BLK_DCM:
            block=DCM_BLOCK(unit,blkport);
            break;
        case SOC_BLK_DCMC:
            block = DCMC_BLOCK(unit);
            break;
        case SOC_BLK_CCS:
            block=CCS_BLOCK(unit,blkport);
            break;
        case SOC_BLK_BRDC_FMAC_AC:
            block=BRDC_FMAC_AC_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_FMAC_BD:
            block=BRDC_FMAC_BD_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_DCH:
            block=BRDC_DCH_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_DCL:
            block=BRDC_DCL_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_DCM:
            block=BRDC_DCM_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_CCS:
            block=BRDC_CCS_BLOCK(unit);
            break;
        case SOC_BLK_DCML:
            block=DCML_BLOCK(unit, blkport);
            break;
        case SOC_BLK_MCT:
            block=MCT_BLOCK(unit);
            break;
        case SOC_BLK_QRH:
            block=QRH_BLOCK(unit, blkport);
            break;
        case SOC_BLK_CCH:
            block=CCH_BLOCK(unit, blkport);
            break;
        case SOC_BLK_LCM:
            block=LCM_BLOCK(unit, blkport);
            break;
        case SOC_BLK_BRDC_DCML:
            block=BRDC_DCML_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_QRH:
            block=BRDC_QRH_BLOCK(unit);
            break;
        case SOC_BLK_BRDC_LCM:
            block=BRDC_LCM_BLOCK(unit);
            break;
        case SOC_BLK_GPORT:
            block=GPORT_BLOCK(unit, blkport);
            break;
        case SOC_BLK_MXQ:
            block=MXQ_BLOCK(unit, blkport);
            break;
        case SOC_BLK_PLL:
            block=PLL_BLOCK(unit);
            break;
        case SOC_BLK_NIF:
            block=NIF_BLOCK(unit);
            break;
        case SOC_BLK_MMU_ITM:
            block = MMU_ITM_BLOCK(unit);
            break;
        case SOC_BLK_MMU_EB:
            block = MMU_EB_BLOCK(unit);
            break;
        case SOC_BLK_CEV:
            block = CEV_BLOCK(unit);
            break;
        case SOC_BLK_MACSEC :
            if (SOC_IS_DNX(unit))
            {
                block = MACSEC_BLOCK(unit, blkport);
            }
            else
            {
                block = MACSEC_BLOCK(unit,0);
            }
            break;    
        case SOC_BLK_TAF:
            block = TAF_BLOCK(unit);
            break;
        case SOC_BLK_DCC:
            block = DCC_BLOCK(unit, blkport);
            break;
        case SOC_BLK_DPC:
            block = DPC_BLOCK(unit, blkport);
            break;
        case SOC_BLK_FASIC:
            block = FASIC_BLOCK(unit);
            break;
        case SOC_BLK_FEU:
            block = FEU_BLOCK(unit);
            break;
        case SOC_BLK_FLEXEWP:
            block = FLEXEWP_BLOCK(unit);
            break;
        case SOC_BLK_FPRD:
            block = FPRD_BLOCK(unit);
            break;
        case SOC_BLK_FSAR:
            block = FSAR_BLOCK(unit);
            break;
        case SOC_BLK_FSCL:
            block = FSCL_BLOCK(unit);
            break;
        case SOC_BLK_ALIGNER:
            block = ALIGNER_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).aligner_blocks;
            break;
        case SOC_BLK_MSU:
            block = MSU_BLOCK(unit, blkport);
            break;
        case SOC_BLK_PMU:
            block = PMU_BLOCK(unit, blkport);
            break;
        case SOC_BLK_NBU:
            block = NBU_BLOCK(unit, blkport);
            break;
        case SOC_BLK_DDHX:
            block = DDHX_BLOCK(unit, blkport);
            break;
        case SOC_BLK_TCMX:
            block = TCMX_BLOCK(unit, blkport);
            break;
        case SOC_BLK_MKX:
            block = MKX_BLOCK(unit, blkport);
            break;
        case SOC_BLK_MDBA:
            block = MDBA_BLOCK(unit);
            break;
        case SOC_BLK_MDBB:
            block = MDBB_BLOCK(unit);
            break;
        case SOC_BLK_MDBK:
            block = MDBK_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).mdbk_blocks;
            break;
        case SOC_BLK_MGU:
            block = MGU_BLOCK(unit);
            break;
        case SOC_BLK_DDHC:
            block = DDHC_BLOCK(unit, blkport);
            break;
        case SOC_BLK_DDHAB:
            block = DDHAB_BLOCK(unit, blkport);
            break;
        case SOC_BLK_MACTMNG:
            block = MACTMNG_BLOCK(unit);
            break;
        case SOC_BLK_FDTM:
            block = FDTM_BLOCK(unit);
            break;
        case SOC_BLK_EPPS:
            block = EPPS_BLOCK(unit, block_core);
            soc_blocks = SOC_INFO(unit).epps_blocks;
            break;
        case SOC_BLK_CPX:
            block = CPX_BLOCK(unit);
            break;
        case SOC_BLK_SAT:
            block = SAT_BLOCK(unit);
            break;
        case SOC_BLK_FDTS:
            block = FDTS_BLOCK(unit);
            break;
        default:
            block = -1; /* unknown non-port block */
            break;
        }
    } else if(block_id_mask) {
        block = port;
    }

    if(!schan_id_mask) {
        if(block < 0) {
            assert(block == SOC_CORE_ALL); /* block must be valid */
        }
    }

    /* determine final block, pindex, and index */
    gransh = 0;
    switch (SOC_REG_INFO(unit, reg).regtype) {
    case soc_cpureg:
    case soc_mcsreg:
    case soc_iprocreg:
        block = -1;
        pindex = 0;
        gransh = 2; /* 4 byte granularity */
        break;
    case soc_ppportreg:
    case soc_portreg:
        if (!SOC_BLOCK_IN_LIST(unit, regblktype, portblktype) && 
            !(SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_MS_ISEC)) && 
            !(SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_MS_ESEC)) &&
            !(SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_TXLP)) && 
            !(SOC_BLOCK_IN_LIST(unit, regblktype, SOC_BLK_RXLP))) {
            if (soc_feature(unit, soc_feature_logical_port_num) &&
                (block == MMU_BLOCK(unit) ||
                 block == MMU_SED_BLOCK(unit) ||
                 block == MMU_GLB_BLOCK(unit) ||
                 block == MMU_XPE_BLOCK(unit) ||
                 block == MMU_SED_BLOCK(unit) ||
                 block == MMU_ITM_BLOCK(unit) ||
                 block == MMU_EB_BLOCK(unit) ||
                 block == MMU_SC_BLOCK(unit))) {
                if (preserve_port) {
                    phy_port = port;
                    pindex = port;
                } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
                    /*MMU register with device port indexing*/
                    num_regs = sizeof(reg_excep_list) / sizeof(soc_reg_t);
                    for (reg_index = 0;reg_index < num_regs; reg_index++) {
                        if (reg_excep_list[reg_index] == reg) {
                            add_exception = 1;
                            break;
                        }
                    }
                    if (add_exception) {
                                pindex = port;
                    } else

#endif
                    {
                    /* coverity[overrun-local : FALSE] */
                    /* coverity[negative_returns : FALSE] */
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                    pindex = SOC_INFO(unit).port_p2m_mapping[phy_port];
                    if (pindex < 0) {
                        pindex = SOC_INFO(unit).max_port_p2m_mapping[phy_port];
                    }
                    assert(pindex >= 0);
                }
                }
#ifdef BCM_TRIUMPH3_SUPPORT
                /* We do not want any more of these exceptions in the code */
                if (SOC_IS_TRIUMPH3(unit) && (reg == MMU_INTFO_CONGST_STr)) {
                    pindex = port;
                }
#endif
            } else {
                pindex = port;
#if defined(BCM_METROLITE_SUPPORT)              
                METROLITE_GET_REG_THDI_PORT(unit, port, reg, pindex)
#endif
            }
            gransh = 8;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Since there are two CDMAC0 and CDMAC1, we need to determine which
           stage does this port belong to for the per port registers.
           For CDPORT registers, the stage is 0 and CDMAC_0 registers, it is 1
           while for CDMAC_1 it is 2 */
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CDPORT)) {
               if ((SOC_REG_INFO(unit, reg).offset & 0x000F0000) ==
                                CDMAC_OFFSET_CNT) {
                   stage_id = ((((phy_port - 1) >> 2) & 1) == 0) ?
                                      CDMAC0_STAGE_ID : CDMAC1_STAGE_ID;
                   if (stage_id == CDMAC0_STAGE_ID) {
                        pindex = pindex | 0x04000000;
                        break;
                   } else {
                        pindex = pindex & 3;
                        pindex = pindex | 0x08000000;
                        break;
                   }
               }
            }
        }
#endif
#ifdef BCM_DNX_SUPPORT
        /* Since there are two CDMAC0 and CDMAC1, we need to determine which
           stage does this port belong to for the per port registers.
           For CDPORT registers, the stage is 0 and CDMAC_0 registers, it is 1
           while for CDMAC_1 it is 2 */
        if (SOC_IS_DNX(unit) && SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CDMAC) &&
           (SOC_REG_INFO(unit, reg).offset & 0x000F0000) == CDMAC_OFFSET_CNT) {
            if (soc_feature(unit,soc_feature_single_instance_cdmac)) {
                pindex = pindex | 0x04000000;
            } else {
                if ((phy_port & 4) == 0) { /* is it CDMAC0_STAGE_ID */
                    pindex = pindex | 0x04000000;
                } else {
                    pindex = pindex & 3;
                    pindex = pindex | 0x08000000;
                }
            }
        }
#endif

        break;
    case soc_cosreg:
        assert(index >= 0 && index < NUM_COS(unit));
        pindex = index;
        index = 0;
        break;
    case soc_pipereg:
    case soc_xpereg:
    case soc_itmreg:
    case soc_ebreg:
    case soc_slicereg:
    case soc_layerreg:
        gransh = 8;
        pindex = port;
        break;
    case soc_customreg:
    case soc_genreg:
        gransh = 8;
        pindex = 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
        /* Since there are two CDMAC0 and CDMAC1, we need to determine which
         * CDMAC/stage id does this register belong to.
         * For CDMAC_0 registers, it is 1, while for CDMAC_1 it is 2 */
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CDPORT)) {
                if ((SOC_REG_INFO(unit, reg).offset & 0x000F0000) ==
                                CDMAC_OFFSET_CNT) {
                    stage_id = ((((phy_port - 1) >> 2) & 1) == 0) ?
                                      CDMAC0_STAGE_ID : CDMAC1_STAGE_ID;
                    if (stage_id == CDMAC0_STAGE_ID) {
                        pindex = pindex | 0x04000000;
                        break;
                    } else {
                        pindex = pindex | 0x08000000;
                        break;
                    }
                }
            }
        }
#endif
        if (SOC_IS_GREYHOUND2(unit) &&
            (block == TAF_BLOCK(unit))) {
            /* Base address decision of GH2 TAF block */
            gransh = 0;
            pindex = index;
            index = 0;
        } else {
            gransh = 8;
            pindex = 0;
#ifdef BCM_DNX_SUPPORT
            /* Since there are two CDMAC0 and CDMAC1, we need to determine which
             * CDMAC/stage id does this register belong to.
             * For CDMAC_0 registers, it is 1, while for CDMAC_1 it is 2 */
            if (SOC_IS_DNX(unit) && SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CDMAC)) {
                if ((SOC_REG_INFO(unit, reg).offset & 0x000F0000) == CDMAC_OFFSET_CNT) {

                    if (soc_feature(unit,soc_feature_single_instance_cdmac)) {
                        pindex = 0x04000000;
                    } 
                    else if (instance_mask)
                    {
                       /* use the block instance for selecting CDMAC stage */
                       pindex = ((port & 1) == 0) ? 0x04000000 : 0x08000000;
                    }
                    else
                    {
                        if ((phy_port & 4) == 0) { /* is it CDMAC0_STAGE_ID */
                            pindex = pindex | 0x04000000;
                        } else {
                            pindex = pindex | 0x08000000;
                        }
                    }
                }
            }
#endif
        }
        break;
    default:
        assert(0); /* unknown register type */
        break;
    }

    /* put together address: base|block|pindex + index */
    base = SOC_REG_INFO(unit, reg).offset;

    /* Base address decision of GH2 TAF block */
    if (SOC_IS_GREYHOUND2(unit) &&
        (SOC_REG_INFO(unit, reg).regtype == soc_genreg) &&
        (block == TAF_BLOCK(unit)) &&
        (SOC_REG_NUMELS(unit, reg) == 128)) {
        base &= ~(1 << SOC_RT_BP);
    }

    LOG_VERBOSE(BSL_LS_SOC_REG,
                (BSL_META_U(unit,
                            "base: %x "), base));

    if ((port == REG_PORT_ANY || port == SOC_CORE_ALL) && (soc_feature(unit,soc_feature_reg_port_any_set_all_instance))) {
        uint32 nof_instance;
        int first_instance;
        unsigned en_blks_num;

        assert(soc_feature(unit,soc_feature_single_block_type_per_register));

        nof_instance = SOC_REG_BLOCK_NOF_INSTANCES(unit, reg);
        first_instance = SOC_REG_BLOCK_FIRST_INSTANCES(unit, reg);

        en_blks_num = 0;
        /* looping over all the instances of register's block */
        for (instance = 0; instance < nof_instance; instance++) {
            if (SOC_INFO(unit).block_valid[first_instance+instance]) {
                access_info->blk_list[en_blks_num] = SOC_BLOCK_INFO(unit, first_instance+instance).cmic;
                en_blks_num++;
            }
        }
        access_info->num_blks = en_blks_num;
    } else if(schan_id_mask) {
        access_info->blk_list[0] = port;
    } else if (block >= 0) {
        access_info->blk_list[0] = SOC_BLOCK_INFO(unit, block).cmic;
    } else if (block == SOC_CORE_ALL) {
        if (SOC_IS_DNX(unit)) {
            /* Fill the output s-channel block ID list */
#ifdef BCM_DNX_SUPPORT
            /* Get the number of active cores */
            access_info->num_blks = dnx_data_device.general.nof_cores_get(unit);
            for (block_core = 0; block_core < access_info->num_blks; ++block_core) { 
                access_info->blk_list[block_core] = soc_blocks != NULL ? SOC_BLOCK_INFO(unit, soc_blocks[block_core]).cmic : -1;
            }
#endif /* BCM_DNX_SUPPORT */
        } else {
            access_info->blk_list[0] = soc_blocks != NULL ? SOC_BLOCK_INFO(unit, soc_blocks[0]).cmic : -1;
    }
    
    } 
    if (pindex != -1) {
        base |= pindex;
    }
    
    if (SOC_REG_IS_ARRAY(unit, reg)) {
        assert(index >= SOC_REG_INFO(unit, reg).first_array_index && index < SOC_REG_NUMELS(unit, reg) + SOC_REG_INFO(unit, reg).first_array_index);
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit) && block == OAMP_BLOCK(unit)) {
            base += (index - SOC_REG_INFO(unit, reg).first_array_index)*(SOC_REG_ELEM_SKIP(unit, reg) << 8);
        } else
#endif
        {
        base += (index - SOC_REG_INFO(unit, reg).first_array_index)*SOC_REG_ELEM_SKIP(unit, reg);
        }
    }  else if (index && SOC_REG_ARRAY(unit, reg)) {
        if (index && SOC_REG_ARRAY2(unit, reg)) {
            assert(index >= 0 && index < 2 * SOC_REG_NUMELS(unit, reg));
            base += ((index*2) << gransh);
        } else if (index && SOC_REG_ARRAY4(unit, reg)) {
            assert(index >= 0 && index < 4 * SOC_REG_NUMELS(unit, reg));
            base += ((index * 4) << gransh);
        } else {
            assert(index >= 0 && index < SOC_REG_NUMELS(unit, reg));
            base += (index << gransh);
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_REG,
                (BSL_META_U(unit,
                            "addr new: %x, block: %d, index: %d, pindex: %d, gransh: %d\n"),
                 base, access_info->blk_list[0], index, pindex, gransh));
    access_info->offset = base;
    return SOC_E_NONE;
}


/*
 * Function:    soc_reg_addr_get
 * Purpose:     calculate the address of a register
 * Parameters:
 *     unit       - SOC unit number
 *     reg        - Register number
 *     port       - Port number or REG_PORT_ANY
 *     index      - Array index (or cos number)
 *     options    - Flag to indicate special handling to calculate
 *                  the HW register address: SOC_REG_ADDR_OPTION_xxx
 *     blk        - (OUT) ...
 *     acc_type   - (OUT) Register access type
 *
 * Returns:     register address suitable for soc_reg_get and friends
 * Notes:       the block number to access is determined by the register
 *              and the port number
 *
 * cpureg       00SSSSSS 00000000 0000RRRR RRRRRRRR
 * genreg       00SSSSSS BBBB1000 0000RRRR RRRRRRRR
 * portreg      00SSSSSS BBBB00PP PPPPRRRR RRRRRRRR
 * cosreg       00SSSSSS BBBB01CC CCCCRRRR RRRRRRRR
 *
 * all regs of bcm88230
 *              00000000 00001000 0000RRRR RRRRRRRR
 *
 * where        B+ is the 4 bit block number
 *              P+ is the 6 bit port number (within a block or chip wide)
 *              C+ is the 6 bit class of service
 *              R+ is the 12 bit register number
 *              S+ is the 6 bit Pipe stage
 */
uint32
soc_reg_addr_get(int unit, soc_reg_t reg, int port, int index, uint32 options,
                 int *blk, uint8 *acc_type)
{
    soc_reg_access_info_t access_info;

    if (soc_reg_xaddr_get(unit, reg, port, index, options, &access_info) == SOC_E_NONE) {
        if (access_info.num_blks > 0) {
            *blk = access_info.blk_list[0]; /* (single) s-channel block ID */
            *acc_type = access_info.acc_type; /* access type */
        }
        return access_info.offset; /* register offset/address in its block */
    }
    LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
      "soc_reg_addr_get: failed to get register address")));
    return -1; /* invalid address */
}

int
soc_regaddrlist_alloc(soc_regaddrlist_t *addrlist)
{
    if ((addrlist->ainfo = sal_alloc(_SOC_MAX_REGLIST *
                sizeof(soc_regaddrinfo_t), "regaddrlist")) == NULL) {
        return SOC_E_MEMORY;
    }
    addrlist->count = 0;
    memset(addrlist->ainfo, 0, sizeof(soc_regaddrinfo_t));

    return SOC_E_NONE;
}

int
soc_regaddrlist_free(soc_regaddrlist_t *addrlist)
{
    if (addrlist->ainfo) {
        sal_free(addrlist->ainfo);
    }

    return SOC_E_NONE;
}

/*
 * Function:   soc_reg_fields32_modify
 * Purpose:    Modify the value of a fields in a register.
 * Parameters:
 *       unit         - (IN) SOC unit number.
 *       reg          - (IN) Register.
 *       port         - (IN) Port number.
 *       field_count  - (IN) Number of fields to modify.
 *       fields       - (IN) Modified fields array.
 *       values       - (IN) New value for each member of fields array.
 * Returns:
 *       BCM_E_XXX
 */
int
soc_reg_fields32_modify(int unit, soc_reg_t reg, soc_port_t port,
                        int field_count, soc_field_t *fields, uint32 *values)
{
    uint64 data64;      /* Current 64 bit register data.  */
    uint64 odata64;     /* Original 64 bit register data. */
    uint32 data32;      /* Current 32 bit register data.  */
    uint32 odata32;     /* Original 32 bit register data. */
    uint32 reg_addr;    /* Register address.              */
    int idx;            /* Iteration index.               */
    uint32 max_val;     /* Max value to fit the field     */
    int field_len;      /* Bit length of the field        */

    /* Check that register is a valid one for this unit. */
    if (!SOC_REG_IS_VALID(unit, reg)) {
        return SOC_E_PARAM;
    }

    if ((NULL == fields) || (NULL == values)) {
        return SOC_E_PARAM;
    }

    /*  Fields & values sanity check. */
    for (idx = 0; idx < field_count; idx++) {

        /* Make sure field is present in register. */
        if (!soc_reg_field_valid(unit, reg, fields[idx])) {
            return SOC_E_PARAM;
        }
        /* Make sure value can fit into field */
        field_len = soc_reg_field_length(unit, reg, fields[idx]);
        max_val = (field_len < 32) ? ((1 << field_len) - 1) : 0xffffffff;
        if (values[idx] > max_val) {
            return SOC_E_PARAM;
        }
    }

    if (soc_feature(unit, soc_feature_new_sbus_format)) {
        if (SOC_REG_IS_64(unit, reg)) {

            /* Read current register value. */
            SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &data64));
            odata64 = data64;

            /* Update fields with new values. */
            for (idx = 0; idx < field_count; idx ++) {
                soc_reg64_field32_set(unit, reg, &data64, fields[idx], values[idx]);
            }
            if (COMPILER_64_NE(data64, odata64)) {
                /* Write new register value back to hw. */
                SOC_IF_ERROR_RETURN(soc_reg64_set(unit, reg, port, 0, data64));
            }
        } else {
            if (soc_cpureg == SOC_REG_TYPE(unit,  reg)) {
                reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, port);
                /* Read PCI register value. */
                SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, reg_addr, &data32));
#ifdef BCM_IPROC_SUPPORT
            } else if (soc_iprocreg == SOC_REG_TYPE(unit,  reg)) {
                reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, port);
                SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit, reg_addr, &data32));
#endif
            } else {
                reg_addr = 0;  /* Compiler warning defense. */
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &data32));
            }
            odata32 = data32;

            for (idx = 0; idx < field_count; idx ++) {
                soc_reg_field_set(unit, reg, &data32, fields[idx], values[idx]);
            }
            if (data32 != odata32) {
                /* Write new register value back to hw. */
                if (soc_cpureg == SOC_REG_TYPE(unit,  reg)) {
                    SOC_IF_ERROR_RETURN(soc_pci_write(unit, reg_addr, data32));
#ifdef BCM_IPROC_SUPPORT
                } else if (soc_iprocreg == SOC_REG_TYPE(unit,  reg)) {
                    SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit, reg_addr, data32));
#endif
                } else {
                    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, data32));
                }
            }
        }
    } else {
        /* Calculate register address. */
        reg_addr = soc_reg_addr(unit, reg, port, 0);

        if (SOC_REG_IS_64(unit, reg)) {

            /* Read current register value. */
            SOC_IF_ERROR_RETURN(soc_reg64_read(unit, reg_addr, &data64));
            odata64 = data64;

            /* Update fields with new values. */
            for (idx = 0; idx < field_count; idx ++) {
                soc_reg64_field32_set(unit, reg, &data64, fields[idx], values[idx]);
            }
            if (COMPILER_64_NE(data64, odata64)) {
#if defined(BCM_XGS_SUPPORT)
                if (soc_feature(unit, soc_feature_regs_as_mem)) {
                    (void)soc_ser_reg_cache_set(unit, reg, port, 0, data64);
                }
#endif /* BCM_XGS_SUPPORT */                
                /* Write new register value back to hw. */
                SOC_IF_ERROR_RETURN(soc_reg64_write(unit, reg_addr, data64));
            }
        } else {
            if (soc_cpureg == SOC_REG_TYPE(unit,  reg)) {
                reg_addr = soc_reg_addr(unit, reg, REG_PORT_ANY, port);
                /* Read PCI register value. */
                /* coverity[result_independent_of_operands] */
                SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, reg_addr, &data32));
            } else {
                SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &data32));
            }

            odata32 = data32;

            for (idx = 0; idx < field_count; idx ++) {
                soc_reg_field_set(unit, reg, &data32, fields[idx], values[idx]);
            }
            if (data32 != odata32) {
                /* Write new register value back to hw. */
                if (soc_cpureg == SOC_REG_TYPE(unit,  reg)) {
                    /* coverity[result_independent_of_operands] */
                    SOC_IF_ERROR_RETURN(soc_pci_write(unit, reg_addr, data32));
                } else {
#if defined(BCM_XGS_SUPPORT)
                    if (soc_feature(unit, soc_feature_regs_as_mem)) {
                        (void)soc_ser_reg32_cache_set(unit, reg, port, 0, data32);
                    }
#endif /* BCM_XGS_SUPPORT */
                    SOC_IF_ERROR_RETURN(soc_reg32_write(unit, reg_addr, data32));
                }
            }
        }
    }
    return (SOC_E_NONE);
}

/*
 * Function:   soc_reg_field32_modify
 * Purpose:    Modify the value of a field in a register.
 * Parameters:
 *       unit  - (IN) SOC unit number.
 *       reg   - (IN) Register.
 *       port  - (IN) Port number.
 *       field - (IN) Modified field.
 *       value - (IN) New field value.
 * Returns:
 *       SOC_E_XXX
 */
int
soc_reg_field32_modify(int unit, soc_reg_t reg, soc_port_t port, 
                       soc_field_t field, uint32 value)
{
    return soc_reg_fields32_modify(unit, reg, port, 1, &field, &value);
}

int
soc_reg_port_valid(int unit, soc_reg_t reg, soc_port_t port) {
    if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, SOC_BLK_XLPORT) &&
        SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, SOC_BLK_MXQPORT)) {
        /* This register is valid for both port blocks */
        if (!IS_XL_PORT(unit, port) && !IS_MXQ_PORT(unit, port)) {
           return FALSE;
        }
    } else if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_XLPORT) &&
               !IS_XL_PORT(unit, port)) {
            return FALSE;
    } else if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_MXQPORT) &&
               !IS_MXQ_PORT(unit, port)) {
            return FALSE;
    }
    return TRUE;
}


/*
 * Function:   soc_reg_above_64_field32_modify
 * Purpose:    Modify the value of a 32 bit field in any size register.
 * Parameters:
 *       unit  - (IN) SOC unit number.
 *       reg   - (IN) Register.
 *       port  - (IN) Port number.
 *       index - (IN) instance index
 *       field - (IN) Modified field.
 *       value - (IN) New field value.
 * Returns:
 *       SOC_E_XXX
 */
int
soc_reg_above_64_field32_modify(int unit, soc_reg_t reg, soc_port_t port, 
                       int index, soc_field_t field, uint32 value)
{
    int rc;
    soc_reg_above_64_val_t data;
    SOC_REG_ABOVE_64_CLEAR(data);
    rc = soc_reg_above_64_get(unit, reg, port, index, data);
    if (rc != SOC_E_NONE){
        return rc;
    }
    soc_reg_above_64_field32_set(unit, reg, data, field, value);
    rc = soc_reg_above_64_set(unit, reg, port, index, data);
    if (rc != SOC_E_NONE){
        return rc;
    }
    return SOC_E_NONE;
}

/*
 * Function:   soc_reg_above_64_field32_modify
 * Purpose:    Modify the value of a 32 bit field in any size register.
 * Parameters:
 *       unit  - (IN) SOC unit number.
 *       reg   - (IN) Register.
 *       port  - (IN) Port number.
 *       index - (IN) instance index
 *       field - (IN) Modified field.
 *       value - (IN) New field value.
 * Returns:
 *       SOC_E_XXX
 */
int
soc_reg_above_64_field64_modify(int unit, soc_reg_t reg, soc_port_t port, 
                       int index, soc_field_t field, uint64 value)
{
    int rc;
    soc_reg_above_64_val_t 
        data;
    SOC_REG_ABOVE_64_CLEAR(data);
    rc = soc_reg_above_64_get(unit, reg, port, index, data);
    if (rc != SOC_E_NONE){
        return rc;
    }
    soc_reg_above_64_field64_set(unit, reg, data, field, value);
    rc = soc_reg_above_64_set(unit, reg, port, index, data);
    if (rc != SOC_E_NONE){
        return rc;
    }
    return SOC_E_NONE;
}

/*
 * Function:    soc_reg_port_idx_valid
 * Purpose:     Determine if a register of a given 
 *                   index and port is valid.
 * Returns:      Returns TRUE  if register is valid.
 *               Returns FALSE if register is not valid.
 */
int
soc_reg_port_idx_valid(int unit, soc_reg_t reg, soc_port_t port, int idx)
{
    soc_numelport_set_t *numelports;
    uint32 *portslist;
    int i, numellist_idx, indx;


    if (!SOC_REG_IS_VALID(unit, reg)) {
        return FALSE;
    }

    /* idx is -1 means "any/all indexes..". so, check for Index 0 */
    indx = (idx == -1) ? 0 : idx;

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        
        if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_TXLP)) {
            if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                SOC_BLK_TXLP, NULL, NULL) != SOC_E_NONE) {
                /* Not a TXLP Port */
                return FALSE;
            }
        } else if (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_RXLP)) {
            if (soc_kt2_linkphy_port_reg_blk_idx_get(unit, port,
                SOC_BLK_RXLP, NULL, NULL) != SOC_E_NONE) {
                /* Not a RXLP Port */
                return FALSE;
            }
#ifdef BCM_METROLITE_SUPPORT
        } else if (SOC_IS_METROLITE(unit) && (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_IECELL))) {
            if (soc_ml_iecell_port_reg_blk_idx_get(unit, port,
                SOC_BLK_IECELL, NULL, NULL) != SOC_E_NONE) {
                /* Not a IECELL Port */
                return FALSE;
            }
#endif
#if defined (BCM_SABER2_SUPPORT)
        } else if (SOC_IS_SABER2(unit) && (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_IECELL))) {
            if (soc_sb2_iecell_port_reg_blk_idx_get(unit, port,
                SOC_BLK_IECELL, NULL, NULL) != SOC_E_NONE) {
                /* Not a IECELL Port */
                return FALSE;
            }
#endif
        }
    }
#endif

    numellist_idx = SOC_REG_NUMELPORTLIST_IDX(unit, reg);
    if (numellist_idx == -1) {
     /* No PORTLIST or NUMEL_PERPORT */
#ifdef BCM_SABER2_SUPPORT
        if (SOC_IS_SABER2(unit)) {
            return soc_reg_port_valid(unit, reg, port);
        }
#endif
        return TRUE;
    }

#ifdef BCM_GREYHOUND2_SUPPORT
    /* GREYHOUND2 should use MMU port rather than logical port while checking PERPort MMU register */
    if(SOC_IS_GREYHOUND2(unit) && (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_MMU))) {
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            port = SOC_INFO(unit).port_l2p_mapping[port];
            port = SOC_INFO(unit).port_p2m_mapping[port];
        }
    }
#endif

    numelports = soc_numelports_list[numellist_idx];
    i=0;
    while (numelports[i].f_idx != -1) {
        if ((indx >= numelports[i].f_idx) && (indx <= numelports[i].l_idx)) {
            portslist = soc_ports_list[numelports[i].pl_idx];
            if (portslist[port /32] & (1 << (port % 32))) {
                return TRUE;
            } else {
                return FALSE;
            }
        }
        i++;
    }
    /* If idx is not found in the numel list, then it is just PORTLIST, not NUMELS_PERPORT */
    portslist = soc_ports_list[numelports[0].pl_idx];
    if (portslist[port /32] & (1 << (port % 32))) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*
 * Function:   soc_reg_egress_cell_count_get
 * Purpose:    Retrieves the number of egress cells for a <port, cos> pair.
 * Parameters:
 *       unit  - (IN) SOC unit number.
 *       port  - (IN) Port number.
 *       cos   - (IN) COS queue.
 *       data  - (OUT) Cell count.
 * Returns:
 *       SOC_E_XXX
 */
int
soc_reg_egress_cell_count_get(int unit, soc_port_t port, int cos, uint32 *data)
{
    if (!SOC_PORT_VALID(unit, port) || cos < 0 || cos >= NUM_COS(unit)) {
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(READ_COSLCCOUNTr(unit, port, cos, data));
    return SOC_E_NONE;
}

#ifdef BCM_CMICM_SUPPORT

soc_cmicm_reg_t cmicm_regs[] = CMICM_REG_INIT;

STATIC soc_cmicm_reg_t
*soc_cmicm_srch (uint32 addr) {
    int start = 0;
    int end = NUM_CMICM_REGS - 1;
    int mid = (start + end) >> 1;
    while (start <= end && cmicm_regs[mid].addr != addr) {
        if (cmicm_regs[mid].addr > addr) {
            end = mid - 1;
        } else {
            start = mid + 1;
        }
        mid = (start + end) >> 1;
    }
    return (cmicm_regs[mid].addr == addr) ? &(cmicm_regs[mid]) : NULL;
}

soc_cmicm_reg_t
*soc_cmicm_reg_get (uint32 idx) {
    return &(cmicm_regs[idx]);
}

soc_reg_t
soc_cmicm_addr_reg (uint32 addr) {
    soc_cmicm_reg_t *cmreg = soc_cmicm_srch(addr);
    return (cmreg == NULL)?INVALIDr:cmreg->reg;
}

char *
soc_cmicm_addr_name (uint32 addr) {
    soc_cmicm_reg_t *cmreg = soc_cmicm_srch(addr);
    return (cmreg == NULL)?"???":cmreg->name;
}
#endif 


uint32
soc_pci_mcs_read(int unit, uint32 addr) {

#ifdef BCM_CMICM_SUPPORT
    uint32 page = (addr & 0xffff8000);
    uint32 off = (addr & 0x00007fff);
    uint32 data = 0;

    if (soc_feature(unit, soc_feature_cmicm)) {
        soc_pci_write(unit, CMIC_PIO_MCS_ACCESS_PAGE_OFFSET, page);

        /* Read back to ensure write is complete */
        page = soc_pci_read(unit, CMIC_PIO_MCS_ACCESS_PAGE_OFFSET);

        /* Hardcoded for now.. use reg addr if & when available in regfile */
        data = soc_pci_read(unit, (0x38000 + off));
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_debug(unit, 32, "read", addr, 0, data);
        }
#endif /* BROADCOM_DEBUG */
        /* _soc_snoop_reg(unit, 0, 0, addr,SOC_REG_SNOOP_READ,0,data); */
        return data;
    }
#endif /* BCM_CMICM_SUPPORT */

    return 0;
}

int
soc_pci_mcs_getreg(int unit, uint32 addr, uint32 *data_ptr) {
    *data_ptr = soc_pci_mcs_read(unit, addr);
    return SOC_E_NONE;
}

int
soc_pci_mcs_write(int unit, uint32 addr, uint32 data) {
#ifdef BCM_CMICM_SUPPORT
    uint32 page = (addr & 0xffff8000);
    uint32 off = (addr & 0x00007fff);

    if (soc_feature(unit, soc_feature_cmicm)) {
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_debug(unit, 32, "write", addr, 0, data);
        }
#endif /* BROADCOM_DEBUG */
        /* _soc_snoop_reg(unit, 0, 0, addr,SOC_REG_SNOOP_WRITE,0,data); */

        soc_pci_write(unit, CMIC_PIO_MCS_ACCESS_PAGE_OFFSET, page);

        /* Read back to ensure write is complete */
        page = soc_pci_read(unit, CMIC_PIO_MCS_ACCESS_PAGE_OFFSET);

        /* Hardcoded for now.. use reg addr if & when available in regfile */
        return soc_pci_write(unit, (0x38000 + off), data);
    }
#endif /* BCM_CMICM_SUPPORT */

    return 0;
}

/* Get register length in bytes */
int
soc_reg_bytes(int unit, soc_reg_t reg) {

    int              bits = 0;
    soc_reg_info_t   *regp;
    int              i, bytes;
    soc_field_info_t *fieldp;

     if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif /* !SOC_NO_NAMES */
        assert(SOC_REG_IS_VALID(unit, reg));
    }
    regp = &(SOC_REG_INFO(unit, reg));

    for (i = 0; i < (int)(regp->nFields); i++) {
        fieldp = &(regp->fields[i]);
        bits = ((fieldp->len + fieldp->bp) > bits)? 
            (fieldp->len + fieldp->bp): bits;
    }
    bytes = BITS2BYTES(bits);
    return bytes;
}

/* Get register length in bits */
int
soc_reg_bits(int unit, soc_reg_t reg) {

    int              bits = 0;
    soc_reg_info_t   *regp;
    int              i;
    soc_field_info_t *fieldp;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if !defined(SOC_NO_NAMES)
        LOG_CLI((BSL_META_U(unit,
                            "reg %s is invalid\n"), soc_reg_name[reg]));
#endif /* !SOC_NO_NAMES */
        return 0;
    }
    regp = &(SOC_REG_INFO(unit, reg));

    for (i = 0; i < (int)(regp->nFields); i++) {
        fieldp = &(regp->fields[i]);
        bits = ((fieldp->len + fieldp->bp) > bits)? 
            (fieldp->len + fieldp->bp): bits;
    }
    
    return bits;
}

/* 
 * Function:     
 *     soc_reg_snoop_register 
 * Purpose:
 *      Registers a snooping call back for specific memory.
 *      Call back will be called on Read or Write operations
 *      on the register according to specified flags
 * Parameters:          
 *      unit         -  (IN) BCM device number.
 *      reg          -  (IN) Register to register a call back for.
 *      flags        -  (IN) SOC_REGS_SNOOP_XXX flags.
 *      snoop_cv     -  (IN) User provided call back, NULL for unregister
 *      user_data    -  (IN) user provided data to be passed to call back function
 * Returns:
 *      None
 */     
void 
soc_reg_snoop_register(int unit, soc_reg_t reg, uint32 flags,
                      soc_reg_snoop_cb_t snoop_cb, void *user_data)
{  
    soc_reg_info_t      *reg_info_p;
        
    if (!SOC_REG_IS_VALID(unit, reg)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    reg_info_p = &SOC_REG_INFO(unit, reg);

    assert(NULL != snoop_cb);

    reg_info_p->snoop_cb = snoop_cb;
    reg_info_p->snoop_user_data = user_data;
    reg_info_p->snoop_flags = flags;

    return;
}

/*
 * Function:
 *     soc_reg_snoop_unregister
 * Purpose:
 *      Unregisters a snooping call back for specific register.
 *      this function will not fail even if call back was not previously
 *      registered.
 * Parameters:
 *      unit         -  (IN) BCM device number.
 *      reg          -  (IN) Register to register a call back for.
 * Returns:
 *      None
 */
void
soc_reg_snoop_unregister(int unit, soc_reg_t reg)
{  
    soc_reg_info_t      *reg_info_p;

    if (!SOC_REG_IS_VALID(unit, reg)) {
#if defined(BCM_ESW_SUPPORT) && !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "reg %s is invalid\n"), soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    reg_info_p = &SOC_REG_INFO(unit, reg);

    reg_info_p->snoop_cb = NULL;
    reg_info_p->snoop_user_data = NULL;
    reg_info_p->snoop_flags = 0;

    return;
}

/*
 * This function allows registration of user defined callbacks for
 * registers and memories operations.
 */
int 
soc_reg_access_func_register(int unit, soc_reg_access_t* reg_access) 
{
    SOC_INFO(unit).reg_access = *reg_access;

    return SOC_E_NONE;
}


int
soc_custom_reg32_get(int unit, soc_reg_t reg, int port, int index, uint32 *data)
{
    soc_reg32_get_f custom_reg32_get = SOC_INFO(unit).custom_reg_access.custom_reg32_get;

    if (!custom_reg32_get) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "custom function not defined\n")));
        return SOC_E_FAIL;
    }

    return custom_reg32_get(unit, reg, port, index, data);
}

int
soc_custom_reg32_set(int unit, soc_reg_t reg, int port, int index, uint32 data)
{
    soc_reg32_set_f custom_reg32_set = SOC_INFO(unit).custom_reg_access.custom_reg32_set;

    if (!custom_reg32_set) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "custom function not defined\n")));
        return SOC_E_FAIL;
    }

    return custom_reg32_set(unit, reg, port, index, data);
}

int
soc_custom_reg64_get(int unit, soc_reg_t reg, int port, int index, uint64 *data)
{
    soc_reg64_get_f custom_reg64_get = SOC_INFO(unit).custom_reg_access.custom_reg64_get;

    if (!custom_reg64_get) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "custom function not defined\n")));
        return SOC_E_FAIL;
    }

    return custom_reg64_get(unit, reg, port, index, data);
}

int
soc_custom_reg64_set(int unit, soc_reg_t reg, int port, int index, uint64 data)
{
    soc_reg64_set_f custom_reg64_set = SOC_INFO(unit).custom_reg_access.custom_reg64_set;

    if (!custom_reg64_set) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "custom function not defined\n")));
        return SOC_E_FAIL;
    }

    return custom_reg64_set(unit, reg, port, index, data);
}

int
soc_custom_reg_above_64_get(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    soc_reg_above64_get_f custom_reg_above64_get = SOC_INFO(unit).custom_reg_access.custom_reg_above64_get;

    if (!custom_reg_above64_get) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "custom function not defined\n")));
        return SOC_E_FAIL;
    }

    return custom_reg_above64_get(unit, reg, port, index, data);
}

int
soc_custom_reg_above_64_set(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data)
{
    soc_reg_above64_set_f custom_reg_above64_set = SOC_INFO(unit).custom_reg_access.custom_reg_above64_set;

    if (!custom_reg_above64_set) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "custom function not defined\n")));
        return SOC_E_FAIL;
    }

    return custom_reg_above64_set(unit, reg, port, index, data);
}

 
/* 
 * Function:     
 *     soc_reg_field_acc_mode_get 
 * Purpose:
 *      indicate if a register or a field is readonly/writeonly. 
 * Parameters:          
 *      unit         -  (IN) BCM device number.
 *      reg          -  (IN) Register 
 *      field        -  (IN) field, if invalid then check readonly/writeonly on the register
 *      is_read_only  -  (OUT) read only indication for the register or the field
 *      is_write_only -  (OUT) write only indication for the register or the field
 * Returns:
 *      None
 */  
void
soc_reg_field_acc_mode_get(
    int unit,
    soc_reg_t reg,
    soc_field_t field,
    uint32 *is_read_only,
    uint32 *is_write_only)
{
    soc_field_info_t *finfop;

    *is_read_only = 0;
    *is_write_only = 0;

    if (!SOC_REG_IS_VALID(unit, reg))
    {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s is invalid\n"),
                   soc_reg_name[reg]));
#endif
        assert(SOC_REG_IS_VALID(unit, reg));
    }

    if (field == INVALIDf)
    {
        if (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_RO)
        {
            *is_read_only = 1;
        }
        return;
    }

    SOC_FIND_FIELD(field, SOC_REG_INFO(unit, reg).fields, SOC_REG_INFO(unit, reg).nFields, finfop);
    if (finfop == NULL)
    {
#if !defined(SOC_NO_NAMES)
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "reg %s field %s is invalid\n"),
                   soc_reg_name[reg], soc_fieldnames[field]));
#endif
        assert(finfop);
    }

    if (finfop->flags & SOCF_RO)
    {
        *is_read_only = 1;
    }

    if (finfop->flags & SOCF_WO)
    {
        *is_write_only = 1;
    }

}

/*
 * Function:
 *     soc_is_reg_flexe_core
 * Purpose:
 *      indicate if a register is customer and for flexe core(not ILKN register).
 * Parameters:
 *      unit         -  (IN) BCM device number.
 *      reg          -  (IN) Register
 * Returns:
 *      TRUE or FALSE
 */
int soc_is_reg_flexe_core(int unit, soc_reg_t reg)
{
    int rc = 0;
    soc_block_types_t regblktype;

    if (SOC_REG_IS_VALID(unit, reg)) {
        if (soc_customreg == SOC_REG_INFO(unit, reg).regtype) {
            regblktype = SOC_REG_INFO(unit, reg).block;
            if (*regblktype == SOC_BLK_FSCL) {
                rc = 1;
            }
        }
    }

    return rc;
}
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)*/


