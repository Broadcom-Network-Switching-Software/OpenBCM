/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Register Tests
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/debug.h>
#include <bcm/link.h>
#include <soc/register.h>
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <bcm_int/control.h>
#ifdef BCM_TRIUMPH2_SUPPORT
#include <soc/triumph2.h>
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <soc/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
#include <soc/katana.h>
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#endif
#if defined (BCM_DFE_SUPPORT)
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/drv.h>
#include <soc/dnx/pll/pll.h>
#include <bcm_int/dnx/init/init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#endif
#if defined(BCM_DNXF_SUPPORT)
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif /*BCM_DFE_SUPPORT*/
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif
#ifdef BCM_PETRA_SUPPORT
#include <appl/dpp/regs_filter.h>
#endif
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_mgmt.h>
#endif


#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)


STATIC int rval_test_proc(int unit, soc_regaddrinfo_t *ainfo, void *data);
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
static int rval_test_proc_above_64(int unit, soc_regaddrinfo_t *ainfo, void *data);
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
extern int test_dnxc_init_test(int unit, args_t *a, void *p);
extern int test_dnxc_init_test_init(int unit, args_t *a, void **p);
extern int test_dnxc_init_test_done(int unit, void *p);
#if defined(BCM_DNX_SUPPORT)
extern const dnx_init_step_t dnx_init_deinit_seq[];
uint32 dnx_tr1_enable[(SOC_MAX_NUM_DEVICES+31)/32] = {0};
#endif
#endif
/*
 * this is a special marker that is used in soc_reg_iterate()
 * to indicate that no more variations of the current register
 * should be iterated over.
 */
#define SOC_E_IGNORE -6000
#define MAX_NOF_BROADCAST_BLOCKS 40
#define BRDC_PREFIX_LEN 5
#if defined (BCM_PETRA_SUPPORT)

int
reg_contain_one_of_the_fields(int unit,uint32 reg,uint32 *fields)
{

  int i;
  for (i=0;fields[i]!=NUM_SOC_FIELD;i++) {
      if (SOC_REG_FIELD_VALID(unit,reg,fields[i])) {
          return 1;
      }
  }
  return 0;
}

#endif

/*
 * reg_test
 *
 * Read/write/addressing tests of all SOC internal register R/W bits
 */
STATIC int
try_reg_value(struct reg_data *rd,
              soc_regaddrinfo_t *ainfo,
              char *regname,
              uint32 pattern,
              uint64 mask)
{
    uint64  pat64, rd64, wr64, rrd64, notmask;
    char    wr_str[20], mask_str[20], pat_str[20], rrd_str[20];
    int r, read_only_flag, write_only_flag;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8 rev_id;
    int unit = rd->unit;
#endif

    COMPILER_64_ZERO(pat64);
    COMPILER_64_ZERO(rd64);
    COMPILER_64_ZERO(wr64);
    COMPILER_64_ZERO(rrd64);
    COMPILER_64_ZERO(notmask);

    read_only_flag = SOC_REG_INFO(rd->unit, ainfo->reg).flags & SOC_REG_FLAG_RO ? 1 : 0;
    write_only_flag = SOC_REG_INFO(rd->unit, ainfo->reg).flags & SOC_REG_FLAG_WO ? 1 : 0;
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id == BCM56982_DEVICE_ID) {
         /* FOR 56982 SKU, some of the PMs are not active in pipes.
            The below code determines which PM does it belong to
            based on the pipe_stage numbering of the IDB_CA/OBM
            registers
          */
        soc_reg_t reg;
        soc_reg_info_t *regp;
        reg = ainfo->reg;
        regp = &(SOC_REG_INFO(unit, reg));
        if (regp->pipe_stage) {
            int pipe_stage = 0;
            int pm_num = 0, pm = 0;
            int acc_type = SOC_REG_ACC_TYPE(unit, reg);
            soc_info_t *si;
            si = &SOC_INFO(unit);
            pipe_stage = regp->pipe_stage;
            if (pipe_stage > 14 && pipe_stage < 23) {
                if (acc_type == 9) {
                    return SOC_E_NONE;
                }
                if (acc_type >=0 && acc_type < 8) {
                    if (pipe_stage > 14 && pipe_stage < 19) {
                        pm_num = pipe_stage - 15;
                    }

                    if (pipe_stage > 18 && pipe_stage < 23) {
                        pm_num = pipe_stage - 19;
                    }

                    pm = pm_num + (acc_type * 4);
                }

                if (si->active_pm_map[pm] != 1) {
                    return SOC_E_NONE;
                }
           }
        }
    }
#endif

    /* skip 64b registers in sim */
    if (SAL_BOOT_PLISIM) {
        if (!SOC_IS_XGS(rd->unit) && !SOC_IS_DNX(rd->unit) && SOC_REG_IS_64(rd->unit,ainfo->reg)) {
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META("Skipping 64 bit %s register in sim\n"),regname));
            return 0;
      }
    }
    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
        if ((write_only_flag == 0) && ((r = soc_anyreg_read(rd->unit, ainfo, &rd64)) < 0)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META("ERROR: read reg %s failed: %s\n"),
                       regname, soc_errmsg(r)));
            return -1;
        }
#ifdef BCM_APACHE_SUPPORT
        /* Skipping CFAP registers because we do
           constrained writes to these registers.
           27th bit of AVS_REG_PMB_SLAVE_AVS_PWD_ACC_CONTROL register
           is a DONE bit which needs to be cleared for every new run
         */
        if (SOC_IS_APACHE(rd->unit)) {
            if (sal_strncasecmp(regname, "CFAPCONFIG", 10) == 0) {
                COMPILER_64_SET(mask, 0, 0);
            } else if (sal_strncasecmp(regname, "CFAPFULLRESETPOINT", 18) == 0) {
                COMPILER_64_SET(mask, 0, 0);
            } else if (sal_strncasecmp(regname, "CFAPFULLSETPOINT", 16) == 0) {
                COMPILER_64_SET(mask, 0, 0);
            } else if (sal_strncasecmp(regname, "AVS_REG_PMB_SLAVE_AVS_PWD_ACC_CONTROL", 37) == 0){
                COMPILER_64_SET(mask, 0, 0);
            } else if (sal_strncasecmp(regname, "MMU_TDM_DEBUG", 13) == 0) {
                COMPILER_64_SET(mask, 0, 0);
            }

        }
#endif
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    }

    COMPILER_64_SET(pat64, pattern, pattern);
    COMPILER_64_AND(pat64, mask);

    notmask = mask;
    COMPILER_64_NOT(notmask);

    wr64 = rd64;
    COMPILER_64_AND(wr64, notmask);
    COMPILER_64_OR(wr64, pat64);

    format_uint64(wr_str, wr64);
    format_uint64(mask_str, mask);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Write %s: value %s mask %s\n"),
              regname, wr_str, mask_str));
    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
        if ((read_only_flag == 0) && ((r = soc_anyreg_write(rd->unit, ainfo, wr64)) < 0)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META("ERROR: write reg %s failed: %s wrote %s (mask %s)\n"),
                       regname, soc_errmsg(r), wr_str, mask_str));
            rd->error = r;
            return -1;
        }
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    }

    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
        if ((write_only_flag == 0) && ((r = soc_anyreg_read(rd->unit, ainfo, &rrd64)) < 0)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META("ERROR: reread reg %s failed: %s after wrote %s (mask %s)\n"),
                       regname, soc_errmsg(r), wr_str, mask_str));
            rd->error = r;
            return -1;
        }
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    }

    COMPILER_64_AND(rrd64, mask);
    format_uint64(rrd_str, rrd64);
    format_uint64(pat_str, pat64);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Read  %s: value %s expecting %s\n"),
              regname, rrd_str, pat_str));
    if (!(read_only_flag | write_only_flag) && COMPILER_64_NE(rrd64, pat64)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META("ERROR %s: wrote %s read %s (mask %s)\n"),
                   regname, pat_str, rrd_str, mask_str));
        rd->error = SOC_E_FAIL;
    }

    /* put the register back the way we found it */
    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
        if ((read_only_flag == 0) && ((r = soc_anyreg_write(rd->unit, ainfo, rd64)) < 0)) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META("ERROR: rewrite reg %s failed: %s\n"),
                       regname, soc_errmsg(r)));
            rd->error = r;
            return -1;        
        }
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    }

    return 0;
}

/**
 * since the nbih/nbil port macros block can be in reset mode 
 * we give the oportunity to skip them at tr1-8 
 * 
 * @author elem (16/12/2014)
 * 
 * @param blocks 
 * 
 * @return uint8 
 */
uint8 block_can_be_disabled(soc_block_type_t block)
{
    switch (block) {
        case SOC_BLK_KAPS:    
        case SOC_BLK_XLP:
        case SOC_BLK_CLP:
        case SOC_BLK_ILKN_PMH:
        case SOC_BLK_ILKN_PML:
        /** QUX */
        case SOC_BLK_MXQ:
           return 1; /* Skip these blocks can be in reset state*/
        default:
            break;
    }
    return 0;

}

uint8 blocks_can_be_disabled(soc_block_types_t blocks)
{
    soc_block_type_t block  = SOC_REG_FIRST_BLK_TYPE(blocks);
    return block_can_be_disabled(block);

}


#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
/*
 * Test a register above 64 bit
 * If reg_data.flag can control a minimal test
 */
STATIC int
try_reg_above_64_value(struct reg_data *rd,
                       soc_regaddrinfo_t *ainfo,
                       char *regname,
                       uint32 pattern,
                       soc_reg_above_64_val_t mask)
{
    char    wr_str[256], mask_str[256], pat_str[256], rrd_str[256];
    int r;
    soc_reg_above_64_val_t rd_val, pat, notmask, wr_val, rrd_val;

    /* skip 64b registers in sim */
    if (SAL_BOOT_PLISIM) {
        if (SOC_REG_IS_64(rd->unit,ainfo->reg)) {
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META("Skipping 64 bit %s register in sim\n"),regname));
            return 0;
      }
    }

    if ((r = soc_reg_above_64_get(rd->unit, ainfo->reg, (ainfo->port >= 0) ? ainfo->port : REG_PORT_ANY, ainfo->idx, rd_val)) < 0) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META("ERROR: read reg %s failed: %s\n"),
                   regname, soc_errmsg(r)));
        return -1;
    }

    SOC_REG_ABOVE_64_SET_PATTERN(pat, pattern);
    SOC_REG_ABOVE_64_AND(pat, mask);

    SOC_REG_ABOVE_64_COPY(notmask, mask);
    SOC_REG_ABOVE_64_NOT(notmask);

    SOC_REG_ABOVE_64_COPY(wr_val, rd_val);
    SOC_REG_ABOVE_64_AND(wr_val, notmask);
    SOC_REG_ABOVE_64_OR(wr_val, pat);

    format_long_integer(wr_str, wr_val, SOC_REG_ABOVE_64_MAX_SIZE_U32);
    format_long_integer(mask_str, mask, SOC_REG_ABOVE_64_MAX_SIZE_U32);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Write %s: value %s mask %s\n"),
              regname, wr_str, mask_str));

    if ((r = soc_reg_above_64_set(rd->unit, ainfo->reg, (ainfo->port >= 0) ? ainfo->port : REG_PORT_ANY, ainfo->idx, wr_val)) < 0) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META("ERROR: write reg %s failed: %s wrote %s (mask %s)\n"),
                       regname, soc_errmsg(r), wr_str, mask_str));
        rd->error = r;
        return -1;
    }

    if ((r = soc_reg_above_64_get(rd->unit, ainfo->reg, (ainfo->port >= 0) ? ainfo->port : REG_PORT_ANY, ainfo->idx, rrd_val)) < 0) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META("ERROR: reread reg %s failed: %s after wrote %s (mask %s)\n"),
                       regname, soc_errmsg(r), wr_str, mask_str));
        rd->error = r;
        return -1;
    }

    SOC_REG_ABOVE_64_AND(rrd_val, mask);
    format_long_integer(rrd_str, rrd_val, SOC_REG_ABOVE_64_MAX_SIZE_U32);
    format_long_integer(pat_str, pat, SOC_REG_ABOVE_64_MAX_SIZE_U32);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META("Read  %s: value %s expecting %s\n"),
              regname, rrd_str, pat_str));
    if (!SOC_REG_ABOVE_64_IS_EQUAL(rrd_val, pat)) {
         LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META("ERROR %s: wrote %s read %s (mask %s)\n"),
                       regname, pat_str, rrd_str, mask_str));
        rd->error = SOC_E_FAIL;
    }

    /* put the register back the way we found it */
    if ((r = soc_reg_above_64_set(rd->unit, ainfo->reg, (ainfo->port >= 0) ? ainfo->port : REG_PORT_ANY, ainfo->idx, rd_val)) < 0) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META("ERROR: rewrite reg %s failed: %s\n"),
                       regname, soc_errmsg(r)));
        rd->error = r;
        return -1;
    }

    return 0;
}
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */

/*
 * Test a register
 * If reg_data.flag can control a minimal test
 */
STATIC int
try_reg(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    struct reg_data    *rd = data;
    uint64        mask, mask2, mask3;
    uint32     temp_mask_hi, temp_mask_lo;
    char        regname[80];
    uint32      access_flag = 0;
#ifdef BCM_HAWKEYE_SUPPORT
    uint32              miscconfig;
    int                 meter = 0;
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_reg_t reg = ainfo->reg;
    int acc_type = SOC_REG_ACC_TYPE(unit, reg);
    uint16 dev_id;
    uint8  rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (dev_id == BCM56983_DEVICE_ID &&
        (acc_type == 2 || acc_type == 3 || acc_type == 4 || acc_type == 5)) {
        return SOC_E_IGNORE;
    }
#endif
    if (!SOC_REG_IS_VALID(unit, ainfo->reg)) {
        return SOC_E_IGNORE;        /* invalid register */
    }

    if (rd->flags & REGTEST_FLAG_ACCESS_ONLY) {
        access_flag = 1;
    }

    if ((SOC_REG_INFO(unit, ainfo->reg).flags &
        (SOC_REG_FLAG_RO | SOC_REG_FLAG_WO | SOC_REG_FLAG_INTERRUPT | SOC_REG_FLAG_GENERAL_COUNTER | SOC_REG_FLAG_SIGNAL)) &&
        !access_flag) {
        return SOC_E_IGNORE;        /* no testable bits */
    }

    if (SOC_REG_INFO(unit, ainfo->reg).regtype == soc_portreg && !SOC_PORT_VALID(unit, ainfo->port)) {
        return 0;            /* skip invalid ports */
    }

#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit) || SOC_IS_KATANA(unit)) {
        if (ainfo->reg == OAM_SEC_NS_COUNTER_64r) {
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit,
                                 "Skipping OAM_SEC_NS_COUNTER_64 register\n")));
            return 0;               /* skip OAM_SEC_NS_COUNTER_64 register */
        }
    }
#endif    
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        if ( !soc_feature(unit,soc_feature_ces)  && (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_CES)) ) {
            return 0;
        }
        if ( !soc_feature(unit,soc_feature_ddr3)  && (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_CI)) ) {
            return 0;
        }
    }
#endif

    /*
     * set mask to read-write bits fields
     * (those that are not marked untestable, reserved, read-only,
     * or write-only)
     */
    if (rd->flags & REGTEST_FLAG_MASK64) {
        mask = soc_reg64_datamask(unit, ainfo->reg, 0);
        if (!access_flag) {
            mask2 = soc_reg64_datamask(unit, ainfo->reg, SOCF_RES);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_RO);
            COMPILER_64_OR(mask2, mask3);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_SIG);
            COMPILER_64_OR(mask2, mask3);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_WO);
            COMPILER_64_OR(mask2, mask3);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_INTR);
            COMPILER_64_OR(mask2, mask3);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_W1TC);
            COMPILER_64_OR(mask2, mask3);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_COR);
            COMPILER_64_OR(mask2, mask3);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_PUNCH);
            COMPILER_64_OR(mask2, mask3);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_WVTC);
            COMPILER_64_OR(mask2, mask3);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_RWBW);
            COMPILER_64_OR(mask2, mask3);
        
            COMPILER_64_NOT(mask2);
            COMPILER_64_AND(mask, mask2);
       } 
    } else {
    
        volatile uint32    m32;

        m32 = soc_reg_datamask(unit, ainfo->reg, 0);
        if (!access_flag) {
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_RES);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_RO);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_WO);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_W1TC);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_COR);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_SIG);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_INTR);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_PUNCH);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_WVTC);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_RWBW);
        }
        COMPILER_64_SET(mask, 0, m32);
        
    }

   /* if (mask == 0) {
         return SOC_E_IGNORE;;
    }*/
    COMPILER_64_TO_32_HI(temp_mask_hi,mask);
    COMPILER_64_TO_32_LO(temp_mask_lo,mask);

    if ((temp_mask_hi == 0) && (temp_mask_lo == 0))  {
        return SOC_E_IGNORE;
    }
    
#ifdef BCM_HAWKEYE_SUPPORT
    if (SOC_IS_HAWKEYE(unit) && soc_feature(unit, soc_feature_eee)) {
        if (ainfo->reg == EEE_DELAY_ENTRY_TIMERr) {
        /* The register r/w test should skip bit20 for register 
         *     EEE_DELAY_ENTRY_TIMER.
         * Because bit20 is a constant value before H/W one second delay 
         *     for EEE feature.
         */
            COMPILER_64_SET(mask2, 0, ~0x100000);
            COMPILER_64_AND(mask, mask2);
        }
    }
#endif /* BCM_HAWKEYE_SUPPORT */

    if (COMPILER_64_IS_ZERO(mask)) {
    return SOC_E_IGNORE;        /* no testable bits */
    }

    /*
     * Check if this register is actually implemented in HW for the
     * specified port/cos. If so, the mask is adjusted for the
     * specified port/cos based on what is acutually in HW.
     */
    if (reg_mask_subset(unit, ainfo, &mask)) {
        /* Skip this register. Returning SOC_E_NONE, instead of 
         * SOC_E_IGNORE since we may not want to skip this register
         * all the time (only for certain ports/cos)
         */
        return SOC_E_NONE;
    }

    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
        soc_reg_sprint_addr(unit, regname, ainfo);
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    } 

#ifdef BCM_HAWKEYE_SUPPORT
    if (SOC_IS_HAWKEYE(unit) && (ainfo->reg != MISCCONFIGr)) {
        if (READ_MISCCONFIGr(unit, &miscconfig) < 0) {
            test_error(unit, "Miscconfig read failed\n");
             return SOC_E_IGNORE;
        }
        
        meter = soc_reg_field_get(unit, MISCCONFIGr, 
                              miscconfig, METERING_CLK_ENf);
    
        if(meter){
            soc_reg_field_set(unit, MISCCONFIGr, 
                              &miscconfig, METERING_CLK_ENf, 0);
    
            if (WRITE_MISCCONFIGr(unit, miscconfig) < 0) {
                test_error(unit, "Miscconfig setting failed\n");
                 return SOC_E_IGNORE;
            }
        }
    } 
#endif

    /*
     * minimal test
     * just Fs and 5s
     * only do first instance of each register
     * (only first port, cos, array index, and/or block)
     */
    if (rd->flags & REGTEST_FLAG_MINIMAL) {
    if (try_reg_value(rd, ainfo, regname, 0xffffffff, mask) < 0) {
        return SOC_E_IGNORE;
    }

    if (try_reg_value(rd, ainfo, regname, 0x55555555, mask) < 0) {
        return SOC_E_IGNORE;
    }
    return SOC_E_IGNORE;    /* skip other than first instance */
    }

    /*
     * full test
     */
    if (try_reg_value(rd, ainfo, regname, 0x00000000, mask) < 0) {
    return SOC_E_IGNORE;
    }

    if (try_reg_value(rd, ainfo, regname, 0xffffffff, mask) < 0) {
    return SOC_E_IGNORE;
    }

    if (try_reg_value(rd, ainfo, regname, 0x55555555, mask) < 0) {
    return SOC_E_IGNORE;
    }

    if (try_reg_value(rd, ainfo, regname, 0xaaaaaaaa, mask) < 0) {
    return SOC_E_IGNORE;
    }

#ifdef BCM_HAWKEYE_SUPPORT
     if (SOC_IS_HAWKEYE(unit) && (ainfo->reg != MISCCONFIGr)) {
        if (WRITE_MISCCONFIGr(unit, miscconfig) < 0) {
            test_error(unit, "Miscconfig setting failed\n");
            return SOC_E_IGNORE;
        }
    }
#endif

    return 0;
}

#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
STATIC int
try_reg_above_64(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    struct reg_data    *rd = data;
    char        regname[80];
    soc_reg_above_64_val_t mask, mask2, mask3;
    uint32      access_flag = 0;

    if (!SOC_REG_IS_VALID(unit, ainfo->reg)) {
        return SOC_E_IGNORE;        /* invalid register */
    }

    if (rd->flags & REGTEST_FLAG_ACCESS_ONLY) {
        access_flag = 1;
    }

    if ((SOC_REG_INFO(unit, ainfo->reg).flags &
        (SOC_REG_FLAG_RO | SOC_REG_FLAG_WO | SOC_REG_FLAG_INTERRUPT | SOC_REG_FLAG_GENERAL_COUNTER | SOC_REG_FLAG_SIGNAL)) &&
       !access_flag) {
        return SOC_E_IGNORE;        /* no testable bits */
    }

    if(SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
        if(SOC_REG_ABOVE_64_INFO(unit, ainfo->reg).size + 2 > CMIC_SCHAN_WORDS(unit)) {
            return SOC_E_IGNORE;                /* size + header larget than CMIC buffer */
        }
    }

    if (SOC_REG_INFO(unit, ainfo->reg).regtype == soc_portreg &&
        !SOC_PORT_VALID(unit, ainfo->port)) {
            return 0;            /* skip invalid ports */
    }  

    /*
     * set mask to read-write bits fields
     * (those that are not marked untestable, reserved, read-only,
     * or write-only)
     */
     soc_reg_above_64_datamask(unit, ainfo->reg, 0, mask);
    if (SOC_REG_ABOVE_64_IS_ZERO(mask)) {
        return SOC_E_IGNORE;        /* no testable bits */
    }

    if(SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
        if(SOC_REG_ABOVE_64_INFO(unit, ainfo->reg).size + 2 > CMIC_SCHAN_WORDS(unit)) {
            return SOC_E_IGNORE;        /* size + header larget than CMIC buffer */
        }
    }

    soc_reg_sprint_addr(unit, regname, ainfo);

    soc_reg_above_64_datamask(unit, ainfo->reg, 0, mask);
    if (!access_flag) {
        soc_reg_above_64_datamask(unit, ainfo->reg, SOCF_RES, mask2);
        soc_reg_above_64_datamask(unit, ainfo->reg, SOCF_RO, mask3);
        SOC_REG_ABOVE_64_OR(mask2, mask3);
        soc_reg_above_64_datamask(unit, ainfo->reg, SOCF_SIG, mask3);
        SOC_REG_ABOVE_64_OR(mask2, mask3);
        soc_reg_above_64_datamask(unit, ainfo->reg, SOCF_WO,mask3);
        SOC_REG_ABOVE_64_OR(mask2, mask3);
        soc_reg_above_64_datamask(unit, ainfo->reg, SOCF_INTR,mask3);
        SOC_REG_ABOVE_64_OR(mask2, mask3);
        soc_reg_above_64_datamask(unit, ainfo->reg, SOCF_WVTC,mask3);
        SOC_REG_ABOVE_64_OR(mask2, mask3);
          
        SOC_REG_ABOVE_64_NOT(mask2);
        SOC_REG_ABOVE_64_AND(mask, mask2);
    } 

    if (SOC_REG_ABOVE_64_IS_ZERO(mask)  == TRUE)  {
        return SOC_E_IGNORE;
    }

    /*
     * minimal test
     * just Fs and 5s
     * only do first instance of each register
     * (only first port, cos, array index, and/or block)
     */
    if (rd->flags & REGTEST_FLAG_MINIMAL) {
        if (try_reg_above_64_value(rd, ainfo, regname, 0xffffffff, mask) < 0) {
            return SOC_E_IGNORE;
        }
    
        if (try_reg_above_64_value(rd, ainfo, regname, 0x55555555, mask) < 0) {
            return SOC_E_IGNORE;
        }
        
        return SOC_E_IGNORE;    /* skip other than first instance */
    }

    /*
     * full test
     */
    if (try_reg_above_64_value(rd, ainfo, regname, 0x00000000, mask) < 0) {
        return SOC_E_IGNORE;
    }

    if (try_reg_above_64_value(rd, ainfo, regname, 0xffffffff, mask) < 0) {
        return SOC_E_IGNORE;
    }

    if (try_reg_above_64_value(rd, ainfo, regname, 0x55555555, mask) < 0) {
        return SOC_E_IGNORE;
    }

    if (try_reg_above_64_value(rd, ainfo, regname, 0xaaaaaaaa, mask) < 0) {
        return SOC_E_IGNORE;
    }

    return 0;
}
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */

STATIC int
try_reg_dispatch(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
#if defined (BCM_PETRA_SUPPORT)
    reg_data_t    *rd = data;
#endif /*BCM_PETRA_SUPPORT*/

#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8  rev_id;
#endif

#if defined (BCM_DFE_SUPPORT)
    if(SOC_IS_DFE(unit)) { 
        int rv;
        int is_filtered = 0;
        
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_test_reg_filter, (unit, ainfo->reg, &is_filtered));
        if (rv != SOC_E_NONE) {
            return rv;
        }
        if (is_filtered) {
            return SOC_E_IGNORE;
        }
    }
#endif /*BCM_DFE_SUPPORT*/
#if defined (BCM_PETRA_SUPPORT)
        if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            switch(ainfo->reg) {
                case ECI_REG_0001r:
                case EGQ_INDIRECT_COMMANDr:
                        return SOC_E_IGNORE;
                default:
                    break;
            }
    }
        if(SOC_IS_JERICHO(unit)) {
            /*
            * we skip on the following common regs after checking with noam halevi from the following reasons
            * 1. XXX_ECC_INTERRUPT_REGISTER_TEST because that the test mask is vary from block to block biut since the register
            * taken from common we cant know the actual size
            *
            * 2. XXX_INDIRECT_COMMAND   since INDIRECT_COMMAND_COUNT change by the HW we cant count on what we wrote
            *
            * 3. XXX_INDIRECT_COMMAND_WR_DATA since length vary from block to block and its width is like the widest mem in a block  
            *  we cant actually know how match bits is realy active for this register while its definition came from common
            *  and its 640 bits
 */
            uint32 disallowed_fields[] = {INDIRECT_COMMAND_COUNTf,INDIRECT_COMMAND_WR_DATAf, INTERRUPT_REGISTER_TESTf,ECC_INTERRUPT_REGISTER_TESTf,NUM_SOC_FIELD};
            if (ainfo->reg < rd->start_from || ainfo->reg >rd->start_from + rd->count) {
                return 1;
            }
            if (reg_contain_one_of_the_fields(unit,ainfo->reg,disallowed_fields)) {
                return 1;
            }
            if (!(rd->flags & REGTEST_FLAG_INC_PORT_MACROS)) {
                soc_block_types_t regblktype = SOC_REG_INFO(unit, ainfo->reg).block; 
                if (blocks_can_be_disabled(regblktype)) {
                    return 1;
                }

            }

        switch(ainfo->reg) {

#if defined(PLISIM)
        /* these global registers vary from block to block, making the expected value wrong */

        /* Writing to this register migth kill the core clock */
        case ECI_OGER_1000r:
        /* some blocks don't have memories and therfore they don't have these common registers  */
        case NBIH_ENABLE_DYNAMIC_MEMORY_ACCESSr:
        case NBIL_ENABLE_DYNAMIC_MEMORY_ACCESSr:
        case NBIH_INDIRECT_COMMAND_ADDRESSr:
        case NBIL_INDIRECT_COMMAND_ADDRESSr:
        case NBIH_INDIRECT_COMMAND_DATA_INCREMENTr:
        case NBIL_INDIRECT_COMMAND_DATA_INCREMENTr:
        case NBIH_INDIRECT_COMMAND_WIDE_MEMr:
        case NBIL_INDIRECT_COMMAND_WIDE_MEMr:
        case NBIH_INDIRECT_FORCE_BUBBLEr:
        case NBIL_INDIRECT_FORCE_BUBBLEr:

        /* Unknown errors */
        case CLPORT_SGNDET_EARLYCRSr:
        case MACSEC_PROG_TX_CRCr:
        case MAC_PFC_CTRLr:
        case MAC_PFC_REFRESH_CTRLr:
        case SFD_OFFSETr:

        /* additional regs r/w test at tr 3 failed on emulation*/
        case IHB_INTERRUPT_MASK_REGISTERr:
        case ILKN_PMH_ECC_INTERRUPT_REGISTER_TESTr:
        case ILKN_PMH_INDIRECT_COMMAND_WR_DATAr:
        case ILKN_PMH_INDIRECT_COMMANDr:
        case ILKN_PMH_INTERRUPT_REGISTER_TESTr:
        case ILKN_PML_ECC_INTERRUPT_REGISTER_TESTr:
        case ILKN_PML_INDIRECT_COMMAND_WR_DATAr:
        case ILKN_PML_INDIRECT_COMMANDr:
        case ILKN_PML_INTERRUPT_REGISTER_TESTr:
        case ILKN_SLE_RX_CFGr:
        case ILKN_SLE_RX_DEBUG_0r:
        case ILKN_SLE_RX_ERRINS_0r:
        case ILKN_SLE_RX_LANEr:
        case ILKN_SLE_RX_STATS_WT_ERR_HIGHr:
        case ILKN_SLE_RX_STATS_WT_ERR_LOWr:
        case ILKN_SLE_RX_STATS_WT_PARITYr:
        case ILKN_SLE_RX_STATS_WT_PKT_LOWr:
        case ILKN_SLE_TX_CFGr:
        case ILKN_SLE_TX_DEBUG_0r:
        case ILKN_SLE_TX_ERRINS_0r:
        case ILKN_SLE_TX_LANEr:
        case ILKN_SLE_TX_STATS_WT_ERR_HIGHr:
        case ILKN_SLE_TX_STATS_WT_ERR_LOWr:
        case ILKN_SLE_TX_STATS_WT_PARITYr:
        case ILKN_SLE_TX_STATS_WT_PKT_LOWr:

        case ILKN_SLE_RX_AFIFO_WMr:
        case ILKN_SLE_RX_BURSTr:
        case ILKN_SLE_RX_CAL_ACCr:
        case ILKN_SLE_RX_CAL_INBAND_DYNr:
        case ILKN_SLE_RX_CAL_INBANDr:
        case ILKN_SLE_RX_CAL_OUTBAND_DYNr:
        case ILKN_SLE_RX_CAL_OUTBANDr:
        case ILKN_SLE_RX_CAL_WTr:
        case ILKN_SLE_RX_CTLr:
        case ILKN_SLE_RX_DEBUG_1r:
        case ILKN_SLE_RX_DEBUG_2r:
        case ILKN_SLE_RX_DEBUG_3r:
        case ILKN_SLE_RX_DEBUG_HOLD_0r:
        case ILKN_SLE_RX_DEBUG_HOLD_1r:
        case ILKN_SLE_RX_DEBUG_HOLD_2r:
        case ILKN_SLE_RX_DEBUG_HOLD_3r:
        case ILKN_SLE_RX_ERRINS_1r:
        case ILKN_SLE_RX_ERRINS_2r:
        case ILKN_SLE_RX_ERRINS_3r:
        case ILKN_SLE_RX_EXT_INT_2_ND_MASKr:
        case ILKN_SLE_RX_EXT_INT_FORCEr:
        case ILKN_SLE_RX_EXT_INT_MASKr:
        case ILKN_SLE_RX_FCOB_RETRANSMIT_SLOT_DYr:
        case ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr:
        case ILKN_SLE_RX_INT_2_ND_MASKr:
        case ILKN_SLE_RX_INT_FORCEr:
        case ILKN_SLE_RX_INT_MASKr:
        case ILKN_SLE_RX_LANE_2r:
        case ILKN_SLE_RX_METAFRAMEr:
        case ILKN_SLE_RX_REMAP_LANE_14_10r:
        case ILKN_SLE_RX_REMAP_LANE_19_15r:
        case ILKN_SLE_RX_REMAP_LANE_24_20r:
        case ILKN_SLE_RX_REMAP_LANE_29_25r:
        case ILKN_SLE_RX_REMAP_LANE_34_30r:
        case ILKN_SLE_RX_REMAP_LANE_39_35r:
        case ILKN_SLE_RX_REMAP_LANE_44_40r:
        case ILKN_SLE_RX_REMAP_LANE_47_45r:
        case ILKN_SLE_RX_REMAP_LANE_4_0r:
        case ILKN_SLE_RX_REMAP_LANE_9_5r:
        case ILKN_SLE_RX_RETRANSMIT_CONFIGr:
        case ILKN_SLE_RX_RETRANSMIT_TIME_CONFIG_2r:
        case ILKN_SLE_RX_RETRANSMIT_TIME_CONFIGr:
        case ILKN_SLE_RX_SEGMENT_ENABLEr:
        case ILKN_SLE_RX_SERDES_TEST_CNTLr:
        case ILKN_SLE_RX_SERDES_TEST_PATTERNAr:
        case ILKN_SLE_RX_SERDES_TEST_PATTERNBr:
        case ILKN_SLE_RX_SERDES_TEST_PATTERNCr:
        case ILKN_SLE_RX_STATS_ACCr:
        case ILKN_SLE_RX_STATS_WT_BYTE_LOWr:
        case ILKN_SLE_TX_AFIFO_WMr:
        case ILKN_SLE_TX_BURSTr:
        case ILKN_SLE_TX_CAL_ACCr:
        case ILKN_SLE_TX_CAL_INBAND_DYNr:
        case ILKN_SLE_TX_CAL_INBANDr:
        case ILKN_SLE_TX_CAL_OUTBAND_DYNr:
        case ILKN_SLE_TX_CAL_OUTBANDr:
        case ILKN_SLE_TX_CAL_WTr:
        case ILKN_SLE_TX_CTLr:
        case ILKN_SLE_TX_DEBUG_1r:
        case ILKN_SLE_TX_DEBUG_2r:
        case ILKN_SLE_TX_DEBUG_3r:
        case ILKN_SLE_TX_DEBUG_HOLD_0r:
        case ILKN_SLE_TX_DEBUG_HOLD_1r:
        case ILKN_SLE_TX_DEBUG_HOLD_2r:
        case ILKN_SLE_TX_DEBUG_HOLD_3r:
        case ILKN_SLE_TX_ERRINS_1r:
        case ILKN_SLE_TX_ERRINS_2r:
        case ILKN_SLE_TX_ERRINS_3r:
        case ILKN_SLE_TX_FCOB_RETRANSMIT_SLOT_DYr:
        case ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr:
        case ILKN_SLE_TX_FIFO_CFGr:
        case ILKN_SLE_TX_INT_2_ND_MASKr:
        case ILKN_SLE_TX_INT_FORCEr:
        case ILKN_SLE_TX_INT_MASKr:
        case ILKN_SLE_TX_LANE_2r:
        case ILKN_SLE_TX_METAFRAMEr:
        case ILKN_SLE_TX_RATE_1r:
        case ILKN_SLE_TX_REMAP_LANE_14_10r:
        case ILKN_SLE_TX_REMAP_LANE_19_15r:
        case ILKN_SLE_TX_REMAP_LANE_24_20r:
        case ILKN_SLE_TX_REMAP_LANE_29_25r:
        case ILKN_SLE_TX_REMAP_LANE_34_30r:
        case ILKN_SLE_TX_REMAP_LANE_39_35r:
        case ILKN_SLE_TX_REMAP_LANE_44_40r:
        case ILKN_SLE_TX_REMAP_LANE_47_45r:
        case ILKN_SLE_TX_REMAP_LANE_4_0r:
        case ILKN_SLE_TX_REMAP_LANE_9_5r:
        case ILKN_SLE_TX_RETRANSMIT_CONFIGr:
        case ILKN_SLE_TX_SEGMENT_ENABLEr:
        case ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr:
        case ILKN_SLE_TX_SERDES_TEST_CNTLr:
        case ILKN_SLE_TX_SERDES_TEST_PATTERNAr:
        case ILKN_SLE_TX_SERDES_TEST_PATTERNBr:
        case ILKN_SLE_TX_SERDES_TEST_PATTERNCr:
        case ILKN_SLE_TX_STATS_ACCr:
        case ILKN_SLE_TX_STATS_WT_BYTE_LOWr:


        case CFC_INTERRUPT_MASK_REGISTERr: /*tr3 emulation  reg=CFC_INTERRUPT_MASK_REGISTERr,value=0x55501,expected0x55555*/ 


#endif
        /*Above registers only failed on emulation. Current only below registers failed on Jericho. Others should be tested.*/
        case EDB_PAR_ERR_INITIATEr:
        case MESH_TOPOLOGY_GLOBAL_MEM_OPTIONSr:
        case MESH_TOPOLOGY_RESERVED_MTCPr:

            return SOC_E_IGNORE;
        default:
            break;
        }
        if (SOC_IS_JERICHO_PLUS_A0(unit)) {
            switch(ainfo->reg) {

                case EDB_INDIRECT_WR_MASKr:
                case EGQ_INDIRECT_WR_MASKr:
                case IHB_INDIRECT_WR_MASKr:
                case KAPS_BBS_INDIRECT_WR_MASKr:
                case PPDB_A_INDIRECT_WR_MASKr:
                case PPDB_B_INDIRECT_WR_MASKr:

                
                case IHP_REG_00A4r:
                case IHP_REG_00A6r:

                
                case IHP_ECC_ERR_1B_MONITOR_MEM_MASKr:
                case IHP_ECC_ERR_2B_MONITOR_MEM_MASKr:

                    return SOC_E_IGNORE;
                default:
                    break;
            }
        }


        }
#endif
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)) {
		if (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "AVS_") != NULL) {
			return 1; /* Skip AVS registers */
		}

        switch(ainfo->reg) {

        case ECI_INDIRECT_COMMANDr:
        case FSRD_INDIRECT_COMMANDr:
        case RTP_INDIRECT_COMMANDr:
        /*SBUS last in chain*/
        case BRDC_CCH_SBUS_BROADCAST_IDr:
        case BRDC_DCML_SBUS_BROADCAST_IDr:
        case BRDC_LCM_SBUS_BROADCAST_IDr:
        case BRDC_QRH_SBUS_BROADCAST_IDr:
        case FMAC_SBUS_BROADCAST_IDr:
        case MESH_TOPOLOGY_SBUS_BROADCAST_IDr:
        /*the following registers are filtered temporarily- the length of these registers should be amended according to the block they are in
        (need to be the longest line of a register that is RW instead of always 640 bit)*/
        case MCT_INDIRECT_COMMANDr:
        case QRH_INDIRECT_COMMANDr:
        case BRDC_DCML_INDIRECT_COMMANDr:
        case BRDC_QRH_INDIRECT_COMMANDr:
        case ECI_ECIC_BLOCKS_RESETr:
        case ECI_MISC_PLL_0_CONFIGr:
        case ECI_MISC_PLL_1_CONFIGr:
        case ECI_MISC_PLL_2_CONFIGr:
        case ECI_MISC_PLL_3_CONFIGr:
        case FMAC_ECC_ERR_1B_MONITOR_MEM_MASKr:
        case FMAC_ECC_ERR_2B_MONITOR_MEM_MASKr:
        case FMAC_ECC_INTERRUPT_REGISTER_MASKr:
        case FMAC_FMAL_GENERAL_CONFIGURATIONr:
        case FMAC_FMAL_RX_GENERAL_CONFIGURATIONr:
        case FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr:
        case FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr:
        case FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr:
        case FMAC_FMAL_TX_GENERAL_CONFIGURATIONr:
        case FMAC_FPS_CONFIGURATION_RX_SYNCr:
        case FMAC_INTERRUPT_MASK_REGISTERr:
        case FMAC_LEAKY_BUCKET_CONTROL_REGISTERr:
        case FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr:
        case FMAC_PAR_ERR_MEM_MASKr:
        case FMAC_RECEIVE_RESET_REGISTERr:
        case FMAC_FE_GLOBAL_GENERAL_CFG_1r:
        case FMAC_RSF_CONFIGURATIONr:
        case FMAC_TX_LANE_SWAP_0r:
        case FMAC_TX_LANE_SWAP_1r:
        case FMAC_TX_LANE_SWAP_2r:
        case FMAC_TX_LANE_SWAP_3r:
        case DCML_INDIRECT_COMMANDr:
               return 1; /* Skip these registers */
            default:
                break;
		}
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
		if (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "AN_") != NULL) {
			return 1; /* Skip AN registers */
		}
		if (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "BRDC_") != NULL) {
			return 1; /* Skip broadcast registers */
		}
        if ((dnx_data_dev_init_general_access_only_get(unit) == 1) && (sal_strstr(SOC_REG_NAME(unit, ainfo->reg), "CDPORT_") != NULL)) {
            return 1;
        }
        switch(ainfo->reg) {

            /*SBUS last in chain*/
            case BDM_SBUS_BROADCAST_IDr:
            case CDUM_SBUS_BROADCAST_IDr:
            case CDU_SBUS_BROADCAST_IDr:
            case CFC_SBUS_BROADCAST_IDr:
            case CGM_SBUS_BROADCAST_IDr:
            case CLU_SBUS_BROADCAST_IDr:
            case CLUP_SBUS_BROADCAST_IDr:
            case CRPS_SBUS_BROADCAST_IDr:
            case DDHA_SBUS_BROADCAST_IDr:
            case DDHB_SBUS_BROADCAST_IDr:
            case DDP_SBUS_BROADCAST_IDr:
            case DHC_SBUS_BROADCAST_IDr:
            case DQM_SBUS_BROADCAST_IDr:
            case ECGM_SBUS_BROADCAST_IDr:
            case ECI_SBUS_BROADCAST_IDr:
            case EDB_SBUS_BROADCAST_IDr:
            case EPNI_SBUS_BROADCAST_IDr:
            case EPRE_SBUS_BROADCAST_IDr:
            case EPS_SBUS_BROADCAST_IDr:
            case ERPP_SBUS_BROADCAST_IDr:
            case ETPPA_SBUS_BROADCAST_IDr:
            case ETPPB_SBUS_BROADCAST_IDr:
            case ETPPC_SBUS_BROADCAST_IDr:
            case EVNT_SBUS_BROADCAST_IDr:
            case FCR_SBUS_BROADCAST_IDr:
            case FCT_SBUS_BROADCAST_IDr:
            case FDA_SBUS_BROADCAST_IDr:
            case FDR_SBUS_BROADCAST_IDr:
            case FDTL_SBUS_BROADCAST_IDr:
            case FDT_SBUS_BROADCAST_IDr:
            case FMAC_SBUS_BROADCAST_IDr:
            case FQP_SBUS_BROADCAST_IDr:
            case FSRD_SBUS_BROADCAST_IDr:
            case HBC_SBUS_BROADCAST_IDr:
            case HBMC_SBUS_BROADCAST_IDr:
            case ILE_SBUS_BROADCAST_IDr:
            case IPPA_SBUS_BROADCAST_IDr:
            case IPPB_SBUS_BROADCAST_IDr:
            case IPPC_SBUS_BROADCAST_IDr:
            case IPPD_SBUS_BROADCAST_IDr:
            case IPPE_SBUS_BROADCAST_IDr:
            case IPPF_SBUS_BROADCAST_IDr:
            case IPS_SBUS_BROADCAST_IDr:
            case IPT_SBUS_BROADCAST_IDr:
            case IQM_SBUS_BROADCAST_IDr:
            case IRE_SBUS_BROADCAST_IDr:
            case ITPPD_SBUS_BROADCAST_IDr:
            case ITPP_SBUS_BROADCAST_IDr:
            case KAPS_SBUS_BROADCAST_IDr:
            case MACT_SBUS_BROADCAST_IDr:
            case MCP_SBUS_BROADCAST_IDr:
            case MDB_SBUS_BROADCAST_IDr:
            case MESH_TOPOLOGY_SBUS_BROADCAST_IDr:
            case MRPS_SBUS_BROADCAST_IDr:
            case MTM_SBUS_BROADCAST_IDr:
            case NMG_SBUS_BROADCAST_IDr:
            case OAMP_SBUS_BROADCAST_IDr:
            case OCB_SBUS_BROADCAST_IDr:
            case OLP_SBUS_BROADCAST_IDr:
            case PDM_SBUS_BROADCAST_IDr:
            case PEM_SBUS_BROADCAST_IDr:
            case PQP_SBUS_BROADCAST_IDr:
            case RQP_SBUS_BROADCAST_IDr:
            case RTP_SBUS_BROADCAST_IDr:
            case SCH_SBUS_BROADCAST_IDr:
            case SIF_SBUS_BROADCAST_IDr:
            case SPB_SBUS_BROADCAST_IDr:
            case SQM_SBUS_BROADCAST_IDr:
            case TCAM_SBUS_BROADCAST_IDr:
            case TDU_SBUS_BROADCAST_IDr:
            /*suspects */
            case CLUP_GTIMER_CONFIGURATIONr:
            case CLU_GTIMER_CONFIGURATIONr:
            case CLU_INDIRECT_COMMANDr:
            case ECI_ECIC_MDIO_CONFIGr:
            case ECI_ECIC_MISC_RESETr:
            case ECI_BLOCKS_POWER_DOWNr:
            case ECI_BLOCKS_SBUS_RESETr:
            case ECI_BLOCKS_SOFT_INITr:
            case ECI_BLOCKS_SOFT_RESETr:
            case ECI_TOP_LEVEL_SAMPLING_CFGr:
            case ECI_TAP_CPU_INTERFACE_COMMANDr:
            case ECI_TAP_CONTROLr:
            case ECI_TIME_RATIOr:
            /* Skipping indirrect command registers */
            case CDU_INDIRECT_COMMANDr:
            case CDUM_INDIRECT_COMMANDr:
            case CFC_INDIRECT_COMMANDr:
            case CGM_INDIRECT_COMMANDr:
            case CRPS_INDIRECT_COMMANDr:
            case DCC_INDIRECT_COMMANDr:
            case DDHA_INDIRECT_COMMANDr:
            case DDHB_INDIRECT_COMMANDr:
            case DHC_INDIRECT_COMMANDr:
            case DPC_INDIRECT_COMMANDr:
            case DQM_INDIRECT_COMMANDr:
            case ECGM_INDIRECT_COMMANDr:
            case ECI_INDIRECT_COMMANDr:
            case EDB_INDIRECT_COMMANDr:
            case EPNI_INDIRECT_COMMANDr:
            case EPS_INDIRECT_COMMANDr:
            case ERPP_INDIRECT_COMMANDr:
            case ESB_INDIRECT_COMMANDr:
            case ETPPA_INDIRECT_COMMANDr:
            case ETPPB_INDIRECT_COMMANDr:
            case ETPPC_INDIRECT_COMMANDr:
            case EVNT_INDIRECT_COMMANDr:
            case FCR_INDIRECT_COMMANDr:
            case FCT_INDIRECT_COMMANDr:
            case FDR_INDIRECT_COMMANDr:
            case FDT_INDIRECT_COMMANDr:
            case FEU_INDIRECT_COMMANDr:
            case FQP_INDIRECT_COMMANDr:
            case FSRD_INDIRECT_COMMANDr:
            case HBC_INDIRECT_COMMANDr:
            case HBMC_INDIRECT_COMMANDr:
            case ILE_INDIRECT_COMMANDr:
            case ILU_INDIRECT_COMMANDr:
            case IPPA_INDIRECT_COMMANDr:
            case IPPB_INDIRECT_COMMANDr:
            case IPPC_INDIRECT_COMMANDr:
            case IPPD_INDIRECT_COMMANDr:
            case IPPE_INDIRECT_COMMANDr:
            case IPPF_INDIRECT_COMMANDr:
            case IPS_INDIRECT_COMMANDr:
            case IPT_INDIRECT_COMMANDr:
            case IQM_INDIRECT_COMMANDr:
            case IRE_INDIRECT_COMMANDr:
            case ITPP_INDIRECT_COMMANDr:
            case ITPPD_INDIRECT_COMMANDr:
            case KAPS_INDIRECT_COMMANDr:
            case MACT_INDIRECT_COMMANDr:
            case MCP_INDIRECT_COMMANDr:
            case MDB_INDIRECT_COMMANDr:
            case MRPS_INDIRECT_COMMANDr:
            case MTM_INDIRECT_COMMANDr:
            case OAMP_INDIRECT_COMMANDr:
            case OCB_INDIRECT_COMMANDr:
            case OLP_INDIRECT_COMMANDr:
            case PEM_INDIRECT_COMMANDr:
            case PQP_INDIRECT_COMMANDr:
            case RQP_INDIRECT_COMMANDr:
            case RTP_INDIRECT_COMMANDr:
            case SCH_INDIRECT_COMMANDr:
            case SPB_INDIRECT_COMMANDr:
            case SQM_INDIRECT_COMMANDr:
            case TCAM_INDIRECT_COMMANDr:
            /* Skipping Gtimer registers */
            case BDM_GTIMER_CONFIGURATIONr:
            case CDUM_GTIMER_CONFIGURATIONr:
            case CDU_GTIMER_CONFIGURATIONr:
            case CFC_GTIMER_CONFIGURATIONr:
            case CGM_GTIMER_CONFIGURATIONr:
            case CRPS_GTIMER_CONFIGURATIONr:
            case DDHA_GTIMER_CONFIGURATIONr:
            case DDHB_GTIMER_CONFIGURATIONr:
            case DDP_GTIMER_CONFIGURATIONr:
            case DCC_GTIMER_CONFIGURATIONr:
            case DHC_GTIMER_CONFIGURATIONr:
            case DQM_GTIMER_CONFIGURATIONr:
            case ECGM_GTIMER_CONFIGURATIONr:
            case ECI_GTIMER_CONFIGURATIONr:
            case EDB_GTIMER_CONFIGURATIONr:
            case EPNI_GTIMER_CONFIGURATIONr:
            case EPRE_GTIMER_CONFIGURATIONr:
            case EPS_GTIMER_CONFIGURATIONr:
            case ERPP_GTIMER_CONFIGURATIONr:
            case ESB_GTIMER_CONFIGURATIONr:
            case ETPPA_GTIMER_CONFIGURATIONr:
            case ETPPB_GTIMER_CONFIGURATIONr:
            case ETPPC_GTIMER_CONFIGURATIONr:
            case EVNT_GTIMER_CONFIGURATIONr:
            case FCR_GTIMER_CONFIGURATIONr:
            case FCT_GTIMER_CONFIGURATIONr:
            case FDA_GTIMER_CONFIGURATIONr:
            case FDR_GTIMER_CONFIGURATIONr:
            case FDTL_GTIMER_CONFIGURATIONr:
            case FDT_GTIMER_CONFIGURATIONr:
            case FEU_GTIMER_CONFIGURATIONr:
            case FMAC_GTIMER_CONFIGURATIONr:
            case FPRD_GTIMER_CONFIGURATIONr:
            case FSRD_GTIMER_CONFIGURATIONr:
            case FQP_GTIMER_CONFIGURATIONr:
            case HBC_GTIMER_CONFIGURATIONr:
            case ILE_GTIMER_CONFIGURATIONr:
            case ILU_GTIMER_CONFIGURATIONr:
            case IPPA_GTIMER_CONFIGURATIONr:
            case IPPB_GTIMER_CONFIGURATIONr:
            case IPPC_GTIMER_CONFIGURATIONr:
            case IPPC_SPECIAL_CLOCK_CONTROLSr:
            case IPPD_GTIMER_CONFIGURATIONr:
            case IPPD_SPECIAL_CLOCK_CONTROLSr:
            case IPPE_GTIMER_CONFIGURATIONr:
            case IPPF_GTIMER_CONFIGURATIONr:
            case IPS_GTIMER_CONFIGURATIONr:
            case IPT_GTIMER_CONFIGURATIONr:
            case IQM_GTIMER_CONFIGURATIONr:
            case IRE_GTIMER_CONFIGURATIONr:
            case ITPPD_GTIMER_CONFIGURATIONr:
            case ITPP_GTIMER_CONFIGURATIONr:
            case KAPS_GTIMER_CONFIGURATIONr:
            case MACT_GTIMER_CONFIGURATIONr:
            case MCP_GTIMER_CONFIGURATIONr:
            case MDB_GTIMER_CONFIGURATIONr:
            case MESH_TOPOLOGY_GTIMER_CONFIGURATIONr:
            case MRPS_GTIMER_CONFIGURATIONr:
            case MTM_GTIMER_CONFIGURATIONr:
            case NMG_GTIMER_CONFIGURATIONr:
            case OAMP_GTIMER_CONFIGURATIONr:
            case OCB_GTIMER_CONFIGURATIONr:
            case OLP_GTIMER_CONFIGURATIONr:
            case PDM_GTIMER_CONFIGURATIONr:
            case PEM_GTIMER_CONFIGURATIONr:
            case PQP_GTIMER_CONFIGURATIONr:
            case RQP_GTIMER_CONFIGURATIONr:
            case RTP_GTIMER_CONFIGURATIONr:
            case SCH_GTIMER_CONFIGURATIONr:
            case SIF_GTIMER_CONFIGURATIONr:
            case SPB_GTIMER_CONFIGURATIONr:
            case SQM_GTIMER_CONFIGURATIONr:
            case TCAM_GTIMER_CONFIGURATIONr:
            case TDU_GTIMER_CONFIGURATIONr:
            /* Read only register and driven by another registers */
            case MDB_ARM_KAPS_GLOBAL_EVENTr:
            case MDB_ARM_KAPS_IBC_COMMAND_WORDr:
            case MDB_ARM_KAPS_IBC_FIFO_CORRECTABLE_STATUSr:
            case MDB_ARM_KAPS_IBC_FIFO_UNCORRECTABLE_STATUSr:
            case MDB_ARM_KAPS_IBC_QUEUE_STATUSr:
            case MDB_ARM_KAPS_IBC_RESPONSE_DATAr:
            case MDB_ARM_KAPS_MEMORY_A_ERRORr:
            case MDB_ARM_KAPS_MEMORY_B_ERRORr:
            case MDB_ARM_KAPS_REVISIONr:
            case MDB_ARM_KAPS_R_5_AXI_ERROR_STATUS_0r:
            case MDB_ARM_KAPS_R_5_AXI_ERROR_STATUS_1r:
            case MDB_ARM_KAPS_R_5_CORE_MEM_CONTROLr:
            case MDB_ARM_KAPS_R_5_DAP_APB_CTRLr:
            case MDB_ARM_KAPS_R_5_DAP_APB_RDATAr:
            case MDB_ARM_KAPS_R_5_DEBUG_STATUSr:
            case MDB_ARM_KAPS_R_5_DEBUG_STATUS_EVENT_BUS_0r:
            case MDB_ARM_KAPS_R_5_DEBUG_STATUS_EVENT_BUS_1r:
            case MDB_ARM_KAPS_R_5_ECC_ERR_RD_TCM_Ar:
            case MDB_ARM_KAPS_R_5_ECC_ERR_RD_TCM_B_0r:
            case MDB_ARM_KAPS_R_5_ECC_ERR_RD_TCM_B_1r:
            case MDB_ARM_KAPS_R_5_ECC_ERR_WR_TCM_Ar:
            case MDB_ARM_KAPS_R_5_ECC_ERR_WR_TCM_B_0r:
            case MDB_ARM_KAPS_R_5_ECC_ERR_WR_TCM_B_1r:
            case MDB_ARM_KAPS_R_5_FIFO_MON_EVENTr:
            case MDB_ARM_KAPS_SEARCH_0_A_ERRORr:
            case MDB_ARM_KAPS_SEARCH_0_B_ERRORr:
            case MDB_ARM_KAPS_SEARCH_1_A_ERRORr:
            case MDB_ARM_KAPS_SEARCH_1_B_ERRORr:
            case MDB_ARM_KAPS_STATUS_CFIFOr:
            case MDB_ARM_KAPS_STATUS_RFIFOr:
            case MDB_ARM_REG_0000CF00r:
            case MDB_ARM_REG_0000EF00r:
            case HBMC_HBM_RESET_CONTROLr:
            case ILKN_X4_CONTROL0_ILKN_CONTROL0r:
            case CDPORT_INTR_STATUSr:
            case CDPORT_LINKSTATUS_DOWNr:
            case CDPORT_TSC_PLL_LOCK_STATUSr:
            case MAIN0_ECC_1B_ERR_INTERRUPT_ENr:
            case MAIN0_ECC_2B_ERR_INTERRUPT_ENr:
            case MAIN0_ECC_CORRUPT_CONTROL_0r:
            case MAIN0_ECC_CORRUPT_CONTROL_1r:
            case MAIN0_ECC_DISABLE_CONTROLr:
            case MAIN0_SETUPr:
            case MAIN0_TICK_CONTROL_0r:
            case MAIN0_TICK_CONTROL_1r:
            case MAIN0_TM_CONTROLr:
            case PKTGEN0_PCS_SEEDA0r:
            case PKTGEN0_PCS_SEEDA1r:
            case PKTGEN0_PCS_SEEDA2r:
            case PKTGEN0_PCS_SEEDA3r:
            case PKTGEN0_PCS_SEEDB0r:
            case PKTGEN0_PCS_SEEDB1r:
            case PKTGEN0_PCS_SEEDB2r:
            case PKTGEN0_PCS_SEEDB3r:
            case PKTGEN0_PKTGENCTRL1r:
            case PKTGEN0_PRTPCONTROLr:
            case PKTGEN1_ERRORMASK0r:
            case PKTGEN1_ERRORMASK1r:
            case PKTGEN1_ERRORMASK2r:
            case PKTGEN1_ERRORMASK3r:
            case PKTGEN1_ERRORMASK4r:
            case PKTGEN1_GLASTEST_CONTROLr:
            case PMD_X1_CONTROLr:
            case PMD_X1_FCLK_PERIODr:
            case PMD_X1_PM_TIMER_OFFSETr:
            case PMD_X4_CONTROLr:
            case PMD_X4_MODEr:
            case PMD_X4_OVERRIDEr:
            case PMD_X4_RX_FIXED_LATENCYr:
            case PMD_X4_TX_FIXED_LATENCYr:
            case PMD_X4_UI_VALUE_HIr:
            case PMD_X4_UI_VALUE_LOr:
            case RX_X1_CONTROL0_RS_FEC_CONFIGr:
            case RX_X1_CONTROL0_RX_LANE_SWAPr:
            case RX_X4_CONTROL0_DECODE_CONTROL_0r:
            case RX_X4_CONTROL0_PMA_CONTROL_0r:
            case RX_X4_CONTROL0_RS_FEC_RX_CONTROL_0r:
            case RX_X4_CONTROL0_RS_FEC_TIMERr:
            case RX_X4_CONTROL0_RX_TS_CONTROLr:
            case RX_X4_FEC_CONTROL_FEC_0r:
            case RX_X4_FEC_CONTROL_FEC_1r:
            case RX_X4_FEC_CONTROL_FEC_2r:
            case RX_X4_STATUS1_RL_MODE_SW_CONTROLr:
            case SC_X1_CONTROL_PIPELINE_RESET_COUNTr:
            case SC_X1_CONTROL_TX_RESET_COUNTr:
            case SC_X4_CONTROL_CONTROLr:
            case SC_X4_CONTROL_SPARE0r:
            case SC_X4_CONTROL_SW_SPARE1r:
            case SYNCE_X4_FRACTIONAL_DIVr:
            case SYNCE_X4_INTEGER_DIVr:
            case TX_X1_CONTROL0_GLAS_TPMA_CONTROLr:
            case TX_X1_CONTROL0_TX_LANE_SWAPr:
            case TX_X4_CONTROL0_MISCr:
            case TX_X4_CONTROL0_RS_SYMBOLr:
            case TX_X4_CONTROL0_TX_TS_CONTROLr:
            case TX_X4_CONTROL0_ERROR_CONTROLr:
            /** Q2A */
            case CLPORT_INTR_STATUSr:
#ifdef ADAPTER_SERVER_MODE 
            case CDPORT_FAULT_LINK_STATUSr:
            case CDPORT_FLOW_CONTROL_CONFIGr:
            case CDPORT_GENERAL_SPARE0_REGr:
            case CDPORT_GENERAL_SPARE1_REGr:
            case CDPORT_GENERAL_SPARE2_REGr:
            case CDPORT_GENERAL_SPARE3_REGr:
            case CDPORT_INTR_ENABLEr:
            case CDPORT_LAG_FAILOVER_CONFIGr:
            case CDPORT_LED_CONTROLr:
            case CDPORT_MAC_CONTROLr:
            case CDPORT_MODE_REGr:
            case CDPORT_SBUS_CONTROLr:
            case CDPORT_SPARE0_REGr:
            case CDPORT_SPARE1_REGr:
            case CDPORT_SPARE2_REGr:
            case CDPORT_SPARE3_REGr:
            case CDPORT_SW_FLOW_CONTROLr:
            case CDPORT_TSC_MEM_CTRLr:
            case CDPORT_XGXS0_CTRL_REGr:
            case CLPORT_CNTMAXSIZEr:
            case CLPORT_CONFIGr:
            case CLPORT_ECC_CONTROLr:
            case CLPORT_EEE_CLOCK_GATEr:
            case CLPORT_EEE_CORE0_CLOCK_GATE_COUNTERr:
            case CLPORT_EEE_COUNTER_MODEr:
            case CLPORT_EEE_DURATION_TIMER_PULSEr:
            case CLPORT_ENABLE_REGr:
            case CLPORT_FAULT_LINK_STATUSr:
            case CLPORT_FLOW_CONTROL_CONFIGr:
            case CLPORT_FORCE_DOUBLE_BIT_ERRORr:
            case CLPORT_FORCE_SINGLE_BIT_ERRORr:
            case CLPORT_INTR_ENABLEr:
            case CLPORT_LAG_FAILOVER_CONFIGr:
            case CLPORT_LED_CHAIN_CONFIGr:
            case CLPORT_LINKSTATUS_DOWN_CLEARr:
            case CLPORT_MAC_CONTROLr:
            case CLPORT_MAC_RSV_MASKr:
            case CLPORT_MIB_MIN_FRAG_SIZEr:
            case CLPORT_MIB_PROG_RANGE_CNTR0_CONFIGr:
            case CLPORT_MIB_PROG_RANGE_CNTR1_CONFIGr:
            case CLPORT_MIB_PROG_RANGE_CNTR2_CONFIGr:
            case CLPORT_MIB_PROG_RANGE_CNTR3_CONFIGr:
            case CLPORT_MIB_RESETr:
            case CLPORT_MIB_RSC0_ECC_STATUSr:
            case CLPORT_MIB_RSC1_ECC_STATUSr:
            case CLPORT_MIB_RSC_RAM_CONTROLr:
            case CLPORT_MIB_TSC0_ECC_STATUSr:
            case CLPORT_MIB_TSC1_ECC_STATUSr:
            case CLPORT_MIB_TSC_RAM_CONTROLr:
            case CLPORT_MODE_REGr:
            case CLPORT_PMD_PLL0_CTRL_CONFIGr:
            case CLPORT_PMD_PLL1_CTRL_CONFIGr:
            case CLPORT_PMD_PLL_CTRL_CONFIGr:
            case CLPORT_POWER_SAVEr:
            case CLPORT_SPARE0_REGr:
            case CLPORT_SW_FLOW_CONTROLr:
            case CLPORT_TS_TIMER_31_0_REGr:
            case CLPORT_TS_TIMER_47_32_REGr:
            case CLPORT_TXPI_CSR_CTRL1r:
            case CLPORT_TXPI_CSR_OVER_ENr:
            case CLPORT_TXPI_CSR_OVER_VAL_0_HIGHr:
            case CLPORT_TXPI_CSR_OVER_VAL_0_LOWr:
            case CLPORT_TXPI_CSR_OVER_VAL_1_HIGHr:
            case CLPORT_TXPI_CSR_OVER_VAL_1_LOWr:
            case CLPORT_TXPI_CSR_OVER_VAL_2_HIGHr:
            case CLPORT_TXPI_CSR_OVER_VAL_2_LOWr:
            case CLPORT_TXPI_CSR_OVER_VAL_3_HIGHr:
            case CLPORT_TXPI_CSR_OVER_VAL_3_LOWr:
            case CLPORT_TXPI_LANE_SELECTIONr:
            case CLPORT_WC_UCMEM_CTRLr:
            case CLPORT_XGXS0_CTRL_REGr:
            case CLPORT_XGXS_COUNTER_MODEr:
            case ECI_BS_PLL_CONFIGURATIONSr:
            case ECI_CDU_INSTRUMENTATION_CTRLr:
            case ECI_CGM_INST_CTRLr:
            case ECI_CRPS_INSTRUMENTATION_CTRLr:
            case ECI_ECI_FIFO_CONFIGr:
            case ECI_ECI_INTERRUPTS_MASKr:
            case ECI_ECI_REGISTER_0r:
            case ECI_FAB_1_PLL_CONFIGURATIONSr:
            case ECI_FAB_PLL_CONFIGURATIONSr:
            case ECI_FIFO_DMA_SELr:
            case ECI_NIF_0_PLL_CONFIGURATIONSr:
            case ECI_NIF_1_PLL_CONFIGURATIONSr:
            case ECI_PP_INFOr:
            case ECI_PRM_PLL_CONTROL_STATUSr:
            case ECI_PVT_MON_A_CONTROL_REGr:
            case ECI_PVT_MON_B_CONTROL_REGr:
            case ECI_PVT_MON_C_CONTROL_REGr:
            case ECI_PVT_MON_D_CONTROL_REGr:
            case ECI_PVT_MON_E_CONTROL_REGr:
            case ECI_RESERVED_19r:
            case ECI_RESERVED_20r:
            case ECI_RESERVED_21r:
            case ECI_RESERVED_22r:
            case ECI_RESERVED_35r:
            case ECI_RESERVED_36r:
            case ECI_RESERVED_47r:
            case ECI_SCRATCH_PAD_1r:
            case ECI_TS_PLL_CONFIGURATIONSr:
            case MESH_TOPOLOGY_AUTO_DOC_NAME_16r:
            case MESH_TOPOLOGY_AUTO_DOC_NAME_18r:
            case MESH_TOPOLOGY_AUTO_DOC_NAME_20r:
            case MESH_TOPOLOGY_AUTO_DOC_NAME_22r:
            case MESH_TOPOLOGY_AUTO_DOC_NAME_26r:
            case MESH_TOPOLOGY_AUTO_DOC_NAME_30r:
            case MESH_TOPOLOGY_AUTO_DOC_NAME_34r:
            case MESH_TOPOLOGY_AUTO_DOC_NAME_109r:
            case MESH_TOPOLOGY_AUTO_DOC_NAME_110r:
            case MESH_TOPOLOGY_SPECIAL_CLOCK_CONTROLSr:
#endif
            /* Q2A skip list Flexe config register - should not access */
            case FASIC_GTIMER_CONFIGURATIONr:
            case FASIC_INDIRECT_COMMANDr:
            case FLEXEWP_GTIMER_CONFIGURATIONr:
            case FLEXEWP_INDIRECT_COMMANDr:
            case FSAR_GTIMER_CONFIGURATIONr:
            case FSAR_INDIRECT_COMMANDr:
            case FSCL_GTIMER_CONFIGURATIONr:
            case FSCL_INDIRECT_COMMANDr:

            /* J2C skip - those control register are making access to other fail */
            case CLPORT_SBUS_CONTROLr:
            case CLPORT_SOFT_RESETr:
            case CLPORT_TSC_PLL_LOCK_STATUSr:


            /* Q2A new eventor and OAMP registers and fields */
            case EVNT_ENABLE_DYNAMIC_MEMORY_ACCESSr:
            case OAMP_TXM_DISABLE_DROP_FIXr:

            /* Q2A CLPORT SKIP LIST - INVESTIGATED */

            case CLPORT_GENERAL_SPARE1_REGr:
            case CLPORT_GENERAL_SPARE2_REGr:
            case CLPORT_GENERAL_SPARE3_REGr:
            case CLPORT_GENERAL_SPARE6_REGr:
            case CLPORT_GENERAL_SPARE7_REGr:
            case CLPORT_PORT_SPARE0r:
            case CLPORT_PORT_SPARE1r:
            case CLPORT_PORT_SPARE2r:
            case CLPORT_PORT_SPARE3r:
            case CLPORT_SGNDET_EARLYCRSr:
               return 1; /* Skip these registers */
            default:
                break;
        }
    }
#endif


#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)

#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) 
    {
        switch (ainfo->reg) {
        case OAMP_INDIRECT_COMMAND_WR_DATAr:
             return SOC_E_IGNORE;
        default:
             break;
        }
    }
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        switch (ainfo->reg) {
        /* CONFIG_ERRORf: HW write */
        /* MIB_RSC_DATA_HI_LVMf and MIB_RSC_DATA_LO_LVMf: Reserved */
        case XLPORT_MIB_RSC_RAM_CONTROLr:
        case TOP_CORE_PLL0_CTRL_0r:
        case TOP_CORE_PLL0_CTRL_4r:
        case TOP_CORE_PLL0_CTRL_6r:
        case TOP_CORE_PLL0_CTRL_8r:
        case TOP_CORE_PLL0_CTRL_9r:
            return SOC_E_IGNORE;
        default:
            break;
        }
    }
#endif /* BCM_TOMAHAWK2_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        soc_cm_get_id(unit, &dev_id, &rev_id);
        if (dev_id == BCM56982_DEVICE_ID) {
            switch (ainfo->reg) {
                /* These regs belong to PMs in pipes which are not active
                    so need to skip them
                 */
                case IDB_CA0_HW_STATUS_2r:
                case IDB_CA0_HW_STATUS_2_PIPE0r:
                case IDB_CA0_HW_STATUS_2_PIPE2r:
                case IDB_CA0_HW_STATUS_2_PIPE4r:
                case IDB_CA0_HW_STATUS_2_PIPE6r:
                case IDB_CA1_HW_STATUS_2r:
                case IDB_CA1_HW_STATUS_2_PIPE0r:
                case IDB_CA1_HW_STATUS_2_PIPE4r:
                case IDB_CA2_HW_STATUS_2_PIPE3r:
                case IDB_CA2_HW_STATUS_2_PIPE7r:
                case IDB_CA3_HW_STATUS_2_PIPE1r:
                case IDB_CA3_HW_STATUS_2_PIPE3r:
                case IDB_CA3_HW_STATUS_2_PIPE5r:
                case IDB_CA3_HW_STATUS_2_PIPE7r:
                    return 1;
                default:
                    break;
            }
        }

        switch(ainfo->reg) {
            /* This reg changes its states after de-assertion of
               reset so skipping this reg */
            case MMU_RQE_INFOTBL_FP_INITr:
            /* Reserved fields in this reg */
            case XLPORT_MIB_RSC_RAM_CONTROLr:
            /* These regs have hw_write fields */
            case MMU_RQE_QUEUE_SNAPSHOT_ENr:
            case IDB_OBM_MONITOR_CONFIGr:
            case IDB_OBM_MONITOR_CONFIG_PIPE0r:
            case IDB_OBM_MONITOR_CONFIG_PIPE1r:
            case IDB_OBM_MONITOR_CONFIG_PIPE2r:
            case IDB_OBM_MONITOR_CONFIG_PIPE3r:
            case IDB_OBM_MONITOR_CONFIG_PIPE4r:
            case IDB_OBM_MONITOR_CONFIG_PIPE5r:
            case IDB_OBM_MONITOR_CONFIG_PIPE6r:
            case IDB_OBM_MONITOR_CONFIG_PIPE7r:
            /* This reg can pertain SW values only when
               MMU_INTFO_XPORT_BKP_HW_UPDATE_DIS reg is programmed.
               So, skippping it. */
            case MMU_INTFO_TO_XPORT_BKPr:
            /* It has DONE field which is readonly . It gets set during the
               write operation and it timesout as the DONEf is still set */
            case EGR_HW_RESET_CONTROL_1r:
            case EGR_HW_RESET_CONTROL_1_PIPE0r:
            case EGR_HW_RESET_CONTROL_1_PIPE1r:
            case EGR_HW_RESET_CONTROL_1_PIPE2r:
            case EGR_HW_RESET_CONTROL_1_PIPE3r:
            case EGR_HW_RESET_CONTROL_1_PIPE4r:
            case EGR_HW_RESET_CONTROL_1_PIPE5r:
            case EGR_HW_RESET_CONTROL_1_PIPE6r:
            case EGR_HW_RESET_CONTROL_1_PIPE7r:
            case CDMAC_RX_LSS_STATUSr:
            case DLB_ECMP_HW_RESET_CONTROLr:
                return SOC_E_IGNORE;
            default:
                break;
        }
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_HURRICANE3_SUPPORT
    if (soc_feature(unit, soc_feature_wh2)) {
        /* Skip CHIP_SBUS_CFG.pmq2 */
        if (ainfo->reg == CHIP_SBUS_CFGr) {
            if (SOC_BLOCK_NUMBER(unit, ainfo->block) == 2) {
                return SOC_E_IGNORE;
            }
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if(SOC_IS_HURRICANE4(unit)) {
        switch(ainfo->reg) {
            /* This reg changes its states after control register change
               resetting control register */
            case IDB_CA_PM0_48_CA_HW_STATUS_0r:
            case IDB_CA_PM0_48_CA_HW_STATUS_1r:
                soc_reg32_set(unit, IDB_CA_PM0_48_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM0_48_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case IDB_CA_PM1_48_CA_HW_STATUS_0r:
            case IDB_CA_PM1_48_CA_HW_STATUS_1r:
                soc_reg32_set(unit, IDB_CA_PM1_48_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM1_48_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case IDB_CA_PM2_48_CA_HW_STATUS_0r:
            case IDB_CA_PM2_48_CA_HW_STATUS_1r:
                soc_reg32_set(unit, IDB_CA_PM2_48_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM2_48_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case IDB_CA_PM0_Q_CA_HW_STATUS0r:
            case IDB_CA_PM0_Q_CA_HW_STATUS1r:
            case IDB_CA_PM0_Q_CA_HW_STATUS2r:
            case IDB_CA_PM0_Q_CA_HW_STATUS3r:
                soc_reg32_set(unit, IDB_CA_PM0_Q_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM0_Q_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;


            case IDB_CA_PM1_Q_CA_HW_STATUS0r:
            case IDB_CA_PM1_Q_CA_HW_STATUS1r:
            case IDB_CA_PM1_Q_CA_HW_STATUS2r:
            case IDB_CA_PM1_Q_CA_HW_STATUS3r:
                soc_reg32_set(unit, IDB_CA_PM1_Q_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM1_Q_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case IDB_CA_PM2_Q_CA_HW_STATUS0r:
            case IDB_CA_PM2_Q_CA_HW_STATUS1r:
            case IDB_CA_PM2_Q_CA_HW_STATUS2r:
            case IDB_CA_PM2_Q_CA_HW_STATUS3r:
                soc_reg32_set(unit, IDB_CA_PM2_Q_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM2_Q_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case IDB_CA_PM3_Q_CA_HW_STATUS0r:
            case IDB_CA_PM3_Q_CA_HW_STATUS1r:
            case IDB_CA_PM3_Q_CA_HW_STATUS2r:
            case IDB_CA_PM3_Q_CA_HW_STATUS3r:
                soc_reg32_set(unit, IDB_CA_PM3_Q_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM3_Q_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case IDB_CA_PM4_Q_CA_HW_STATUS0r:
            case IDB_CA_PM4_Q_CA_HW_STATUS1r:
            case IDB_CA_PM4_Q_CA_HW_STATUS2r:
            case IDB_CA_PM4_Q_CA_HW_STATUS3r:
                soc_reg32_set(unit, IDB_CA_PM4_Q_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM4_Q_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case IDB_CA_PM5_Q_CA_HW_STATUS0r:
            case IDB_CA_PM5_Q_CA_HW_STATUS1r:
            case IDB_CA_PM5_Q_CA_HW_STATUS2r:
            case IDB_CA_PM5_Q_CA_HW_STATUS3r:
                soc_reg32_set(unit, IDB_CA_PM5_Q_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM5_Q_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case IDB_CA_PM6_Q_CA_HW_STATUS0r:
            case IDB_CA_PM6_Q_CA_HW_STATUS1r:
            case IDB_CA_PM6_Q_CA_HW_STATUS2r:
            case IDB_CA_PM6_Q_CA_HW_STATUS3r:
                soc_reg32_set(unit, IDB_CA_PM6_Q_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM6_Q_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case IDB_CA_PM7_Q_CA_HW_STATUS0r:
            case IDB_CA_PM7_Q_CA_HW_STATUS1r:
            case IDB_CA_PM7_Q_CA_HW_STATUS2r:
            case IDB_CA_PM7_Q_CA_HW_STATUS3r:
                soc_reg32_set(unit, IDB_CA_PM7_Q_CA_CONTROL_0r, REG_PORT_ANY, 0, 0);
                soc_reg32_set(unit, IDB_CA_PM7_Q_CA_CONTROL_1r, REG_PORT_ANY, 0, 0);
                break;

            case XLMAC_CLEAR_ECC_STATUSr:
            case XLMAC_CLEAR_FIFO_STATUSr:
            case XLMAC_CLEAR_RX_LSS_STATUSr:
            case XLMAC_CTRLr:
            case XLMAC_E2ECC_DATA_HDR_0r:
            case XLMAC_E2ECC_DATA_HDR_1r:
            case XLMAC_E2ECC_MODULE_HDR_0r:
            case XLMAC_E2ECC_MODULE_HDR_1r:
            case XLMAC_E2EFC_DATA_HDR_0r:
            case XLMAC_E2EFC_DATA_HDR_1r:
            case XLMAC_E2EFC_MODULE_HDR_0r:
            case XLMAC_E2EFC_MODULE_HDR_1r:
            case XLMAC_E2E_CTRLr:
            case XLMAC_ECC_CTRLr:
            case XLMAC_ECC_FORCE_DOUBLE_BIT_ERRr:
            case XLMAC_ECC_FORCE_SINGLE_BIT_ERRr:
            case XLMAC_EEE_1_SEC_LINK_STATUS_TIMERr:
            case XLMAC_EEE_CTRLr:
            case XLMAC_EEE_TIMERSr:
            case XLMAC_GMII_EEE_CTRLr:
            case XLMAC_HIGIG_HDR_0r:
            case XLMAC_HIGIG_HDR_1r:
            case XLMAC_INTR_ENABLEr:
            case XLMAC_LLFC_CTRLr:
            case XLMAC_MEM_CTRLr:
            case XLMAC_MODEr:
            case XLMAC_PAUSE_CTRLr:
            case XLMAC_PFC_CTRLr:
            case XLMAC_PFC_DAr:
            case XLMAC_PFC_OPCODEr:
            case XLMAC_PFC_TYPEr:
            case XLMAC_RX_CTRLr:
            case XLMAC_RX_LLFC_MSG_FIELDSr:
            case XLMAC_RX_LSS_CTRLr:
            case XLMAC_RX_MAC_SAr:
            case XLMAC_RX_MAX_SIZEr:
            case XLMAC_RX_VLAN_TAGr:
            case XLMAC_SPARE0r:
            case XLMAC_SPARE1r:
            case XLMAC_TIMESTAMP_ADJUSTr:
            case XLMAC_TIMESTAMP_BYTE_ADJUSTr:
            case XLMAC_TX_CRC_CORRUPT_CTRLr:
            case XLMAC_TX_CTRLr:
            case XLMAC_TX_LLFC_MSG_FIELDSr:
            case XLMAC_TX_MAC_SAr:
                /* Skip these registers for QSGMII port */
                if (SOC_PORT_VALID(unit, ainfo->port) &&
                    IS_QSGMII_PORT(unit, ainfo->port)) {
                    return SOC_E_IGNORE;
                }
                break;
            case SP_GLB_MIB_CTRLr:
                return SOC_E_IGNORE;
            default:
                break;
        }
        if (_soc_hr4_reg_skip(unit, ainfo->reg)) {
            return SOC_E_IGNORE;
        }
    }
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_FIREBOLT6_SUPPORT
    if(SOC_IS_FIREBOLT6(unit)) {
        if (_soc_fb6_reg_skip(unit, ainfo->reg)) {
            return SOC_E_IGNORE;
        }
    }
#endif

    if(SOC_BLOCK_IS(SOC_REG_INFO(unit, ainfo->reg).block, SOC_BLK_CCH)) {
        /* skip test for CANCUN CCH pseudo registers */
        return SOC_E_IGNORE;
    }
    if(SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
        return try_reg_above_64(unit, ainfo, data);
    } else 
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    { /* To work on 64 AND '32' bits regs */
        return try_reg(unit, ainfo, data);
    }
}
STATIC int
rval_test_proc_dispatch(int unit, soc_regaddrinfo_t *ainfo, void *data)
{

#ifdef BCM_TRIDENT3_SUPPORT
    if(SOC_BLOCK_IS(SOC_REG_INFO(unit, ainfo->reg).block, SOC_BLK_CCH)) {
        /* skip test for CCH pseudo registers */
        return 0;
    }
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
    if(SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
        return rval_test_proc_above_64(unit, ainfo, data);
    } else 
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    { /* To work on 64 AND '32' bits regs */
        return rval_test_proc(unit, ainfo, data);
    }
}

#ifdef BCM_SAND_SUPPORT
/*
 * dDisable interrupts and counters for test (tr 3)
 */
int
disable_intr_and_count_reg_test(int unit, args_t *a, void *pa)
{
    int rv = 0;
    if (SOC_IS_DNXF(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "access_only", 0))
    {
        return rv;
    }
    else
    {
        bcm_switch_event_t switch_event;
        bcm_switch_event_control_t type;
        bcm_switch_service_config_t config;
        uint32 value;
        switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
        type.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
        type.index = 0;
        type.action = bcmSwitchEventMask;
        value = 1;
        rv = bcm_switch_event_control_set(unit, switch_event, type, value);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_event_control_set\n");
            return rv;
        }
        bcm_switch_service_config_t_init(&config);
        config.enabled = bcmServiceStateDisabled;
        rv = bcm_switch_service_set(unit, bcmServiceCounterCollection, &config);
        if (rv < 0)
        {
            printf("Error, bcm_switch_service_set\n");
        }
    }
    return 0;
}
#endif

/*
 * Register read/write test (tr 3)
 */
int
reg_test(int unit, args_t *a, void *pa)
{
    struct reg_data rd;
    int r=0;
    int rv = 0;
    char *s;
#if defined(BCM_HAWKEYE_SUPPORT) || defined (BCM_ESW_SUPPORT) /* DPPCOMPILEENABLE */
    uint32 tem;
#endif /* BCM_HAWKEYE_SUPPORT || BCM_ESW_SUPPORT  || DPPCOMPILEENABE */
#if defined(BCM_HURRICANE3_SUPPORT)
    int port;
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    int cosq, idx;
#endif /* BCM_HURRICANE3_SUPPORT */

    COMPILER_REFERENCE(pa);

    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Register read/write test\n")));

    rd.unit = unit;
    rd.error = SOC_E_NONE;
    rd.flags = 0;
    rd.start_from = 0;
    rd.count = NUM_SOC_REG;

    if (a)
    {
        while ((s = ARG_GET(a)) != NULL) {
            if (sal_strcasecmp(s, "mini") == 0 ||
                sal_strcasecmp(s, "minimal") == 0) {
                rd.flags |= REGTEST_FLAG_MINIMAL;
                continue;
            }
            if (sal_strcasecmp(s, "mask64") == 0 ||
                sal_strcasecmp(s, "datamask64") == 0) {
                rd.flags |= REGTEST_FLAG_MASK64;
                continue;
            }
            if (sal_strcasecmp(s, "IncPm") == 0) {
                rd.flags |= REGTEST_FLAG_INC_PORT_MACROS;
                continue;
            }
            if (sal_strcasecmp(s, "StartFrom") == 0) {
                char *sf = ARG_GET(a);
                rd.start_from  = sal_ctoi(sf,0);
                continue;
            }
            if (sal_strcasecmp(s, "RegsNo") == 0) {
                char *sf = ARG_GET(a);
                rd.count  = sal_ctoi(sf,0);
                continue;
            }
            if (sal_strcasecmp(s, "AccessOnly") == 0) {
                rd.flags |= REGTEST_FLAG_ACCESS_ONLY;
                continue;
            }
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit,
                                 "WARNING: unknown argument '%s' ignored\n"), s));
        }
    }

    if (BCM_UNIT_VALID(unit)) {
        rv = bcm_linkscan_enable_set(unit, 0); /* disable linkscan */
        if(rv != SOC_E_UNAVAIL) { /* if unavail - no need to disable */
            BCM_IF_ERROR_RETURN(rv);
        }
    }


#if defined(BCM_PETRA_SUPPORT) && defined(BCM_JERICHO_SUPPORT)
    if(!SAL_BOOT_PLISIM && SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        jer_mgmt_pvt_monitor_correction_disable(unit);
    }
#endif
    {
#if defined (BCM_PETRA_SUPPORT)   
        if (SOC_IS_ARAD(unit)) {
            soc_counter_stop(unit);
            if ((r = soc_dpp_device_reset(unit, SOC_DPP_RESET_MODE_REG_ACCESS, SOC_DPP_RESET_ACTION_INOUT_RESET)) < 0) {
                LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ERROR: Unable to reinit unit %d: %s\n"), unit, soc_errmsg(r)));
                goto done;
            }
        } else
#endif
#if defined(BCM_SAND_SUPPORT)
        if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
            soc_counter_stop(unit);
            sal_sleep(1);
            if (SOC_IS_DNXF(unit)) {
                if ((r = soc_device_reset(unit, SOC_SAND_RESET_MODE_REG_ACCESS, SOC_SAND_RESET_ACTION_INOUT_RESET)) < 0) {
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                              (BSL_META_U(unit,
                                          "ERROR: Unable to reinit unit %d: %s\n"), unit, soc_errmsg(r)));
                    rv = BCM_E_FAIL;
                    goto done;
                }
            sal_sleep(1);
            }
        } else
#endif
#ifdef BCM_DFE_SUPPORT
        if (SOC_IS_DFE(unit))
        {
            r = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_blocks_reset, (unit, 0 , NULL));
            if (r != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Unable to reinit unit %d: %s\n"), unit, soc_errmsg(r)));
                goto done;
            }
        } else 
#endif
        {
            if(!(SOC_IS_DNXF(unit) || SOC_IS_DNX(unit))) {
                if ((r = soc_reset_init(unit)) < 0) {
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                              (BSL_META_U(unit,
                                          "ERROR: Unable to reset unit %d: %s\n"),
                               unit, soc_errmsg(r)));
                    goto done;
                }
            }
        }
    }
    
    if (SOC_IS_HB_GW(unit) && soc_feature(unit, soc_feature_bigmac_rxcnt_bug)) {
        /*
         * We need to wait for auto-negotiation to complete to ensure
         * that the BigMAC Rx counters will respond correctly.
         */
        sal_usleep(500000);
    }

    /* When doing E2EFC register/memory read/write tests, it is strongly recommended to set this parity genrate 
     *  enable bit to 0. This will prevent hardware automatically overwritting the ECC and
     *  parity field in E2EFC registers/memories contents and failing the tests.
     */
#if defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) /* DPPCOMPILEENABLE */
    if (soc_reg_field_valid(unit, MISCCONFIGr, E2EFC_PARITY_GEN_ENf)) {
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &tem));
        soc_reg_field_set(unit, MISCCONFIGr, &tem, E2EFC_PARITY_GEN_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, tem));
    } else if (soc_reg_field_valid(unit, MISCCONFIGr, PARITY_CHECK_ENf)) {
        /* If there is no E2EFC_PARITY_GEN_EN field, 
         * try to disable PARITY_ENABLE_EN 
         */
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &tem));
        soc_reg_field_set(unit, MISCCONFIGr, &tem, PARITY_CHECK_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, tem)); 
    }
#endif     /* DPPCOMPILEENABLE */

    /* When doing REMOTE_PFC_STATE register read/write tests, it is recommended to set
     * the REMOTE_PFC_XOFF_TC_TIME_OUT register to zero.
     * It will prevent the REMOTE_PFC_STATE register was modified by HW,
     * once the timeout event is happened.
     */
#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_REG_IS_VALID(unit, REMOTE_PFC_STATEr)) {
        cosq = NUM_COS(unit);
        PBMP_ALL_ITER(unit, port) {
            for (idx = 0; idx < cosq; idx++) {
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, 
                    REMOTE_PFC_XOFF_TC_TIME_OUTr,port, idx, 0));
            }
        }
    }
#endif /* BCM_HURRICANE3_SUPPORT */
   
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        soc_port_t port;
        soc_port_t port_max = SOC_INFO(unit).cpu_hg_index;

        /* Initialize the MMU port mapping so the backpressure
         * registers work properly.
         */
        for (port = 0; port < port_max; port++) {
            SOC_IF_ERROR_RETURN
                (WRITE_MMU_TO_PHY_PORT_MAPPINGr(unit, port, port));
        }

        SOC_IF_ERROR_RETURN
            (WRITE_MMU_TO_PHY_PORT_MAPPINGr(unit, 0, 59));
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_TO_PHY_PORT_MAPPINGr(unit, 59, 0));

        /* Turn off the background MMU processes */
        if ((rv = _soc_triumph3_mem_parity_control(unit, INVALIDm,
                                           SOC_BLOCK_ALL, FALSE)) < 0) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "ERROR: Unable to stop HW updates on unit %d: %s\n"),
                       unit, soc_errmsg(r)));
            goto done;
        }
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */ 
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        /* Turn off the background h/w updates, enable cpu access */
        if ((rv = soc_td2_reg_cpu_write_control(unit, TRUE)) < 0) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "ERROR: Unable to stop HW updates on unit %d: %s\n"),
                       unit, soc_errmsg(r)));
            goto done;
        }
    } else
#endif /* BCM_APACHE_SUPPORT */
    {
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
            /* Turn off the background h/w updates, enable cpu access */
            if ((rv = soc_tomahawk_reg_cpu_write_control(unit, TRUE)) < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Unable to stop HW updates on unit %d: %s\n"),
                           unit, soc_errmsg(r)));
                goto done;
            }
        } 
#endif 
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {

             if ((rv = soc_misc_init(unit)) < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit,
                        "ERROR: Unable to stop HW updates on unit %d: %s\n"),
                        unit, soc_errmsg(r)));
                goto done;
             }

            /* Turn off the background h/w updates, enable cpu access */
            if ((rv = soc_tomahawk3_reg_cpu_write_control(unit, TRUE)) < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Unable to stop HW updates on unit %d: %s\n"),
                           unit, soc_errmsg(r)));
                goto done;
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                /* Turn off the background h/w updates, enable cpu access */
                if ((rv = soc_trident3_reg_cpu_write_control(unit, TRUE)) < 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "ERROR: Unable to stop HW updates on unit %d: %s\n"),
                               unit, soc_errmsg(r)));
                    goto done;
                }
            }
#endif /* BCM_TRIDENT3_SUPPORT */

    }

    /*
     * If try_reg returns -1, there was a register access failure rather
     * than a bit error.
     *
     * If try_reg returns 0, then rd.error is -1 if there was a bit
     * error.
     */
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IARB_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IPARS_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IVLAN_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ISW1_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ISW2_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_DEBUG_CONFIGr, 9,
                                    IL_TREX2_DEBUG_LOCKf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_DEBUG_CONFIGr, 13,
                                    IL_TREX2_DEBUG_LOCKf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_THD_1_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_THD_0_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_PORT_THD_0_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_PORT_THD_1_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_CFG_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_PORT_CFG_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G0_BUCKET_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G0_CONFIG_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G1_BUCKET_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G1_CONFIG_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G2_BUCKET_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G2_CONFIG_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G3_BUCKET_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G3_CONFIG_ENABLE_ECCf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_CPU_PKT_ENABLE_ECCf, 0));
    }
#endif
    {
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2P_TT2P(unit)) {
            /* Turn off the background h/w updates, enable cpu access */
            if ((rv = soc_td2_reg_cpu_write_control (unit, TRUE)) < 0) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Unable to stop HW updates : %s\n"),
                           soc_errmsg(r)));
                goto done;
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */
    }
    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
        if (soc_reg_iterate(unit, try_reg_dispatch, &rd) < 0) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(unit,
                                 "Continuing test.\n")));
            rv = 0;
        } else {
            rv = rd.error;
        }
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    } 

#if defined (BCM_ESW_SUPPORT)
    /* Re-Enable Parity Gen */
    if (soc_reg_field_valid(unit, MISCCONFIGr, E2EFC_PARITY_GEN_ENf)) {
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &tem));
        soc_reg_field_set(unit, MISCCONFIGr, &tem, E2EFC_PARITY_GEN_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, tem));
    } else if (soc_reg_field_valid(unit, MISCCONFIGr, PARITY_CHECK_ENf)) {
        /* Re-enable PARITY_ENABLE */
        if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
            SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &tem));
            soc_reg_field_set(unit, MISCCONFIGr, &tem, PARITY_CHECK_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, tem)); 
        }
    }
#endif
   
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        if ((rv = _soc_triumph3_mem_parity_control(unit, INVALIDm,
                                           SOC_BLOCK_ALL, TRUE)) < 0) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "ERROR: Unable to restart HW updates on unit %d: %s\n"),
                       unit, soc_errmsg(r)));
            goto done;
        }
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        /* Turn on the background h/w updates, enable cpu access */
        if ((rv = soc_td2_reg_cpu_write_control(unit, FALSE)) < 0) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "ERROR: Unable to restart HW updates on unit %d: %s\n"),
                       unit, soc_errmsg(r)));
            goto done;
        }
    } else
#endif /* BCM_APACHE_SUPPORT */
    {
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TOMAHAWK3(unit)) {
            /* Turn off the background h/w updates, enable cpu access */
            if ((rv = soc_tomahawk_reg_cpu_write_control(unit, FALSE)) < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Unable to stop HW updates on unit %d: %s\n"),
                           unit, soc_errmsg(r)));
                goto done;
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */ 
    }

done:
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IARB_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IPARS_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IVLAN_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ISW1_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, ISW2_REGS_DEBUGr, REG_PORT_ANY,
                                    DEBUGf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_DEBUG_CONFIGr, 9,
                                    IL_TREX2_DEBUG_LOCKf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IL_DEBUG_CONFIGr, 13,
                                    IL_TREX2_DEBUG_LOCKf, 0));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_THD_1_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_THD_0_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_PORT_THD_0_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_PORT_THD_1_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_CFG_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    WRED_PORT_CFG_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G0_BUCKET_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G0_CONFIG_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G1_BUCKET_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G1_CONFIG_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G2_BUCKET_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G2_CONFIG_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G3_BUCKET_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_SHAPE_G3_CONFIG_ENABLE_ECCf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MMU_ECC_DEBUG1r, REG_PORT_ANY,
                                    MTRO_CPU_PKT_ENABLE_ECCf, 1));
    }
#endif
/** Perform tr 141 after TR 3 is finished */
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    if (SOC_IS_DNX(unit) || (SOC_IS_DNXF(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "access_only", 0))))
    {
        test_dnxc_init_test_init(unit, a, pa);
        test_dnxc_init_test(unit, a, pa);
        test_dnxc_init_test_done(unit, pa);
    }
#endif
    if (rv < 0) {
    test_error(unit, "Register read/write test failed\n");
    return rv;
    }

    return rv;
}

#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
/*
 * rval_test
 *
 * Reset SOC and compare reset values of all SOC registers with regsfile
 */

STATIC int
rval_test_proc_above_64(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    struct reg_data *rd = data;
    char            buf[80];
    soc_reg_above_64_val_t          rmsk, rval, rrd_val;
    int             r;
    char    wr_str[256], mask_str[256], rval_str[256], rrd_str[256];
    soc_reg_t       reg;

    reg = ainfo->reg;

   

    SOC_REG_ABOVE_64_RST_VAL_GET(unit, reg, rval);
    SOC_REG_ABOVE_64_RST_MSK_GET(unit, reg, rmsk);
    SOC_REG_ABOVE_64_AND(rval, rmsk);

    if (rval_test_skip_reg(unit, ainfo, rd)) {
        /* soc_reg_sprint_addr(unit, buf, ainfo);
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit,
                                 "Skipping register %s\n"), buf)); */
        return 0;
    }

    if (SOC_REG_ABOVE_64_IS_ZERO(rmsk)) {
        return 0;   /* No reset value */
    }

    if(SOC_REG_IS_ABOVE_64(unit, ainfo->reg)) {
        if(SOC_REG_ABOVE_64_INFO(unit, ainfo->reg).size + 2 > CMIC_SCHAN_WORDS(unit)) {
            return SOC_E_IGNORE;  /* size + header larget than CMIC buffer */
            }
    }

    soc_reg_sprint_addr(unit, buf, ainfo);

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)) {
        if ((r = soc_reg_above_64_get(rd->unit, ainfo->reg, (ainfo->port >= 0) ? ainfo->port : REG_PORT_ANY,  ainfo->idx, rrd_val)) < 0) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                              "ERROR: reread reg %s failed: %s after wrote %s (mask %s)\n"),
                               buf, soc_errmsg(r), wr_str, mask_str));
            rd->error = SOC_E_FAIL;
        }
    } else
#endif
    if ((r = soc_reg_above_64_get(rd->unit, ainfo->reg, (ainfo->port >= 0) ? ainfo->port : REG_PORT_ANY, (SOC_REG_ARRAY(unit, reg) || SOC_REG_IS_ARRAY(unit, reg)) ? SOC_REG_INFO(unit, reg).first_array_index : 0, rrd_val)) < 0) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "ERROR: reread reg %s failed: %s after wrote %s (mask %s)\n"),
                   buf, soc_errmsg(r), wr_str, mask_str));
        rd->error = SOC_E_FAIL; 
    }

  
    SOC_REG_ABOVE_64_AND(rrd_val, rmsk);
   
    format_long_integer(rrd_str, rrd_val, SOC_REG_ABOVE_64_MAX_SIZE_U32);
    format_long_integer(rval_str, rval, SOC_REG_ABOVE_64_MAX_SIZE_U32);
    format_long_integer(mask_str, rmsk, SOC_REG_ABOVE_64_MAX_SIZE_U32);


    if (!SOC_REG_ABOVE_64_IS_EQUAL(rrd_val, rval)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "ERROR %s: default %s read %s (mask %s)\n"),
                   buf, rval_str, rrd_str, mask_str));
        rd->error = SOC_E_FAIL;
    }

    return 0;
}
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */

/*
 * rval_test
 *
 * Reset SOC and compare reset values of all SOC registers with regsfile
 */

STATIC int
rval_test_proc(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    struct reg_data *rd = data;
    char            buf[80];
    uint64          value, rmsk, rval, chk;
    char            val_str[20], rmsk_str[20], rval_str[20];
    int             r;
    uint64          mask2,mask3;
    uint32          temp_mask_hi, temp_mask_lo;
    soc_reg_t       reg = ainfo->reg;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8 rev_id;
    int acc_type = SOC_REG_ACC_TYPE(unit, reg);
    soc_cm_get_id(unit, &dev_id, &rev_id);


    if (dev_id == BCM56982_DEVICE_ID) {
         /* FOR 56982 SKU, some of the PMs are not active in pipes.
            The below code determines which PM does it belong to
            based on the pipe_stage numbering of the IDB_CA/OBM
            registers
          */
        soc_reg_info_t *regp;
        regp = &(SOC_REG_INFO(unit, reg));
        if (regp->pipe_stage) {
            int pipe_stage = 0;
            int pm_num = 0, pm = 0;
            soc_info_t *si;
            si = &SOC_INFO(unit);
            pipe_stage = regp->pipe_stage;
            if (pipe_stage > 14 && pipe_stage < 23) {
                if (acc_type == 9) {
                    return SOC_E_NONE;
                }
                if (acc_type >=0 && acc_type < 8) {
                    if (pipe_stage > 14 && pipe_stage < 19) {
                        pm_num = pipe_stage - 15;
                    }

                    if (pipe_stage > 18 && pipe_stage < 23) {
                        pm_num = pipe_stage - 19;
                    }

                    pm = pm_num + (acc_type * 4);
                }

                if (si->active_pm_map[pm] != 1) {
                    return SOC_E_NONE;
                }
            }
        }
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (dev_id == BCM56983_DEVICE_ID &&
        (acc_type == 2 || acc_type == 3 || acc_type == 4 || acc_type == 5)) {
        return 0;
    }
#endif
    /* NOTE: This is experimental */
#ifdef  BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && 
        (reg == EGR_OUTER_TPIDr || reg == ING_MPLS_TPIDr ||
         reg == ING_OUTER_TPIDr)) {
        reg = reg+ainfo->idx+1;
    }
#endif
    SOC_REG_RST_MSK_GET(unit, reg, rmsk);

    if (rval_test_skip_reg(unit, ainfo, rd)) {
        /* soc_reg_sprint_addr(unit, buf, ainfo);
         LOG_WARN(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "Skipping register %s\n"), buf));*/

        return 0;
    }

#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
        if (ainfo->reg == OAM_SEC_NS_COUNTER_64r) {
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit,
                                 "Skipping OAM_SEC_NS_COUNTER_64 register\n")));
            return 0;                /* skip OAM_SEC_NS_COUNTER_64 register */
        }
    }
#endif    

#ifdef BCM_HAWKEYE_SUPPORT
    if (SOC_IS_HAWKEYE(unit)) {
        if (ainfo->reg == MAC_MODEr) {
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit,
                                 "Skipping MAC_MODE register\n")));
            return 0;                /* skip MAC_MODE register */
        }
    }
#endif    
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
            if ( !soc_feature(unit,soc_feature_ces)  && (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_CES)) ) {
                return 0;
            }
            if ( !soc_feature(unit,soc_feature_ddr3)  && (SOC_REG_BLOCK_IS(unit, ainfo->reg, SOC_BLK_CI)) ) {
                return 0;
            }
        }
#endif    

#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        if (ainfo->reg == TOP_XG_PLL0_CTRL_3r) {
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit,
                                 "Skipping TOP_XG_PLL0_CTRL_3 register\n"))); 
            return 0;
        }
    }
#endif
    
    if (SAL_BOOT_PLISIM) {
        if (!SOC_IS_XGS(rd->unit) && SOC_REG_IS_64(rd->unit,ainfo->reg)) {
            {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
                soc_reg_sprint_addr(unit, buf, ainfo);
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
            }
            LOG_WARN(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit,
                                 "Skipping 64 bit %s register in sim\n"),buf));
            return 0;
      }
    }

    if (SOC_IS_SAND(unit)) { /* NOTE: Without this check tr 1 breaks for all XGS chips */
        if (rd->flags & REGTEST_FLAG_MASK64) {
            rmsk = soc_reg64_datamask(unit, ainfo->reg, 0);
            mask2 = soc_reg64_datamask(unit, ainfo->reg, SOCF_IGNORE_DEFAULT_TEST);
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_WO);

            COMPILER_64_OR(mask2, mask3); 

        /*    mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_INTR);
            COMPILER_64_OR(mask2, mask3);

            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_COR);
            COMPILER_64_OR(mask2, mask3); */
            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_W1TC);
            COMPILER_64_OR(mask2, mask3);

            mask3 = soc_reg64_datamask(unit, ainfo->reg, SOCF_SIG);
            COMPILER_64_OR(mask2, mask3);
            
            COMPILER_64_NOT(mask2);
            COMPILER_64_AND(rmsk, mask2);
            
            
        } else {
        
            volatile uint32 m32;
    
            m32 = soc_reg_datamask(unit, ainfo->reg, 0);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_IGNORE_DEFAULT_TEST);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_WO);
            m32 &= ~soc_reg_datamask(unit, ainfo->reg, SOCF_SIG);

            COMPILER_64_SET(rmsk, 0, m32);
            
        }

        COMPILER_64_TO_32_HI(temp_mask_hi,rmsk);
        COMPILER_64_TO_32_LO(temp_mask_lo,rmsk);
    
        if ((temp_mask_hi == 0) && (temp_mask_lo == 0))  {
            return 0;
        }
    }

    /*
     * Check if this register is actually implemented in HW for the
     * specified port/cos. If so, the mask is adjusted for the
     * specified port/cos based on what is acutually in HW.
     */
    if (reg_mask_subset(unit, ainfo, &rmsk)) {
        return 0;
    }

    if (COMPILER_64_IS_ZERO(rmsk)) {
        return 0;   /* No reset value */
    }

    SOC_REG_RST_VAL_GET(unit, reg, rval);
    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
        soc_reg_sprint_addr(unit, buf, ainfo);
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    }

    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
        if ((r = soc_anyreg_read(rd->unit, ainfo, &value)) < 0) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "ERROR: read reg %s (0x%x) failed: %s\n"),
                       buf, ainfo->addr, soc_errmsg(r)));
            rd->error = r;
        return -1;        }
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    }

    format_uint64(val_str, value);
    format_uint64(rmsk_str, rmsk);
    format_uint64(rval_str, rval);
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Read %s: reset mask %s, reset value %s, read %s\n"),
              buf, rmsk_str, rval_str, val_str));

    /* Check reset value is correct */
    COMPILER_64_ZERO(chk);
    COMPILER_64_ADD_64(chk, value);
    COMPILER_64_XOR(chk, rval);
    COMPILER_64_AND(chk, rmsk);

    if (!COMPILER_64_IS_ZERO(chk)) {
        COMPILER_64_AND(rval, rmsk);
        format_uint64(rval_str, rval);
        COMPILER_64_AND(value, rmsk);
        format_uint64(val_str, value);
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "ERROR: %s: expected %s, got %s, reset mask %s\n"),
                   buf, rval_str, val_str, rmsk_str));
        rd->error = SOC_E_FAIL;
    }

    return 0;
}

#ifdef BCM_DNX_SUPPORT
static int tr1_init_callback(int unit) {
    reg_data_t rd;
    int rv = 0;
    rd.unit = unit;
    rd.error = SOC_E_NONE;
    rd.flags = 0;
    rd.start_from = 0;
    rd.count = NUM_SOC_REG;
    if (soc_reg_iterate(unit, rval_test_proc_dispatch, &rd) != 0) {
        rv = rd.error;
        if (rv < 0)
        {
            test_error(unit, "Register reset value test failed\n");
        }
    }
    return rv;
}
#endif /* BCM_DNX_SUPPORT */

/*
 * Register reset value test (tr 1)
 */
int
rval_test(int unit, args_t *a, void *pa)
{
    struct reg_data rd;
    int r, rv = -1;
    char *s;

    COMPILER_REFERENCE(pa);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Register reset value test\n")));

    rd.unit = unit;
    rd.error = SOC_E_NONE;
    rd.flags = 0;
    rd.start_from = 0;
    rd.count = NUM_SOC_REG;
    while ((s = ARG_GET(a)) != NULL) {
        if (sal_strcasecmp(s, "IncPm") == 0) {
            rd.flags |= REGTEST_FLAG_INC_PORT_MACROS;
            continue;
        }
        if (sal_strcasecmp(s, "StartFrom") == 0) {
            char *sf = ARG_GET(a);
            rd.start_from  = sal_ctoi(sf,0);
            continue;
        }
        if (sal_strcasecmp(s, "RegsNo") == 0) {
            char *sf = ARG_GET(a);
            rd.count  = sal_ctoi(sf,0);
            continue;
        }
        LOG_WARN(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit,
                             "WARNING: unknown argument '%s' ignored\n"), s));
    }

    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }
  /*  if (!SOC_IS_ARAD(unit)) {*/
        if (BCM_UNIT_VALID(unit)) {
            rv = bcm_linkscan_enable_set(unit, 0); /* disable linkscan */
            if(rv != SOC_E_UNAVAIL) { /* if unavail - no need to disable */
                BCM_IF_ERROR_RETURN(rv);
            }
        }
   /* } */

#if defined(BCM_PETRA_SUPPORT) && defined(BCM_JERICHO_SUPPORT)
    if(!SAL_BOOT_PLISIM && SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        jer_mgmt_pvt_monitor_correction_disable(unit);
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        soc_triumph2_pipe_mem_clear(unit);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
    {
#if defined(BCM_DFE_SUPPORT)
        if (SOC_IS_FE1600(unit))
        {
            soc_counter_stop(unit);
            sal_sleep(1);            
            r = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_blocks_reset, (unit, 0 , NULL));
            if (r != SOC_E_NONE)
            {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Unable to reinit unit %d: %s\n"), unit, soc_errmsg(r)));
                goto done;
            }
            sal_sleep(1);
        } else 
#endif
#if defined(BCM_DFE_SUPPORT) || defined(BCM_PETRA_SUPPORT)
        if (SOC_IS_DFE(unit) || SOC_IS_ARAD(unit))
        {
            soc_counter_stop(unit);
            sal_sleep(1);            
            if ((r = soc_device_reset(unit, SOC_DCMN_RESET_MODE_REG_ACCESS, SOC_DCMN_RESET_ACTION_INOUT_RESET)) < 0) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Unable to reinit unit %d: %s\n"), unit, soc_errmsg(r)));
                rv = BCM_E_FAIL;
                goto done;
            }
            sal_sleep(1);

        } else 
#endif
#if defined(BCM_SAND_SUPPORT)
        if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
            soc_counter_stop(unit);
            sal_sleep(1);
            if (SOC_IS_DNXF(unit)){
                if ((r = soc_device_reset(unit, SOC_SAND_RESET_MODE_REG_ACCESS, SOC_SAND_RESET_ACTION_INOUT_RESET)) < 0) {
                    LOG_ERROR(BSL_LS_APPL_COMMON,
                              (BSL_META_U(unit,
                                          "ERROR: Unable to reinit unit %d: %s\n"), unit, soc_errmsg(r)));
                    rv = BCM_E_FAIL;
                    goto done;
                }
                sal_sleep(1);
            }
#ifdef BCM_DNX_SUPPORT
           if (SOC_IS_DNX(unit)) {
                dnx_startup_test_functions[unit] = tr1_init_callback;
                goto done;
            }
#endif
        } else
#endif
        {
#if defined(BCM_PETRA_SUPPORT)
            if (!SOC_IS_ARAD(unit)) 
#endif
            {
                {
                    if ((r = soc_reset_init(unit)) < 0) {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit,
                                              "ERROR: Unable to reset unit %d: %s\n"),
                                   unit, soc_errmsg(r)));
                        goto done;
                    }
                }
            }
        }
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) 
        || SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)
        || SOC_IS_HURRICANE(unit)) {
        /* reset the register XQPORT_XGXS_NEWCTL_REG to 0x0 */
        soc_port_t port;
        PBMP_PORT_ITER(unit, port) {
            switch(port) {
            case 26:
            case 27:
            case 28:
            case 29:
                if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
                    SOC_IF_ERROR_RETURN(WRITE_XQPORT_XGXS_NEWCTL_REGr(
                                                       unit, port, 0x0));
                } else {
                    SOC_IF_ERROR_RETURN(WRITE_XPORT_XGXS_NEWCTL_REGr(
                                                       unit, port, 0x0));
                }
                break;
            case 30:
            case 34:
            case 38:
            case 42:
            case 46:
            case 50:
                SOC_IF_ERROR_RETURN(WRITE_XQPORT_XGXS_NEWCTL_REGr(unit, port, 0x0));
                break;
            default:
                break;
            }
        }
    }
#endif
    sal_usleep(10000);

    {
#if defined (BCM_ESW_SUPPORT) || defined (BCM_SAND_SUPPORT)
        if (soc_reg_iterate(unit, rval_test_proc_dispatch, &rd) < 0) {
            goto done;
        }
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */
    }

    rv = rd.error;

 done:
    if (rv < 0) {
        test_error(unit, "Register reset value test failed\n");
    }
    {
#ifdef BCM_PETRA_SUPPORT
        if (SOC_IS_ARAD(unit))
        {
            if ((r = soc_dpp_device_reset(unit, SOC_DPP_RESET_MODE_REG_ACCESS,SOC_DPP_RESET_ACTION_INOUT_RESET)) < 0) {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "ERROR: Unable to reinit unit %d: %s\n"), unit, soc_errmsg(r)));
            
            }
        } else 
#endif
#if defined (BCM_DFE_SUPPORT) || defined(BCM_DNXF_SUPPORT)
        if (SOC_IS_DFE(unit) || SOC_IS_DNXF(unit))
        {
            
            LOG_WARN(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "Warning: Run 'tr 141' in order to reset unit %d\n"), unit));
            
        } else 
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit) && (dnx_data_dev_init_general_access_only_get(unit) == 0))
        {
            LOG_WARN(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "Warning: Run 'tr 141' in order to reset unit %d\n"), unit));
            test_dnxc_init_test_init(unit, a, pa);
            test_dnxc_init_test(unit, a, pa);
            test_dnxc_init_test_done(unit, pa);
            dnx_startup_test_functions[unit] = NULL;
        } else if (SOC_IS_DNX(unit) && (dnx_data_dev_init_general_access_only_get(unit) == 1))
        {
            tr1_init_callback(unit);
        } else
#endif
        if ((r = soc_reset_init(unit)) < 0) {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit,
                                  "ERROR: Unable to reset unit %d: %s\n"),
                       unit, soc_errmsg(r)));
        }
    }

    return rv;
}


#endif /* BCM_ESW_SUPPORT */

#if defined(BCM_DFE_SUPPORT)

#define BRDC_BLOCKS_TEST_MAX_BLOCKS     (10)
/*
 * Function:
 *      brdc_blk_test_info_get
 * Purpose:
 *      Returns necessary info on device broadcast blocks
 * Parameters:
 *      unit                        - (IN)  Unit number.
 *      max_size                    - (IN)  max number of broadcast blocks
 *      brdc_info                   - (OUT) structure which holds the required info about each broadcast block
 *      actual_size                 - (OUT) number of broadcast blocks
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */

STATIC int
brdc_blk_test_info_get(int unit, int max_size, soc_reg_brdc_block_info_t *brdc_info, int *actual_size)
{
    int rv = BCM_E_UNAVAIL;

    *actual_size = 0;

#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit))
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_test_brdc_blk_info_get, (unit, max_size, brdc_info, actual_size));
        if (rv < 0)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "ERROR: unit %d : %s\n"),
                                unit, soc_errmsg(rv)));
            return rv;
        }
    }
#endif

    return rv;
}

/*
 * Function:
 *      brdc_blk_test_reg_filter
 * Purpose:
 *      Special registers should not be tested in broadcast block test
 * Parameters:
 *      unit                        - (IN)  Unit number.
 *      reg                         - (IN)  relevant reg
 *      is_filtered                 - (OUT) if 1 - do not test this reg
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */

STATIC int 
brdc_blk_test_reg_filter(int unit, soc_reg_t reg, int *is_filter)
{
    int rv = BCM_E_NONE;

    /*filter read only, interrupt registers, counters and signal*/
    if (SOC_REG_INFO(unit, reg).flags &
            (SOC_REG_FLAG_RO | SOC_REG_FLAG_INTERRUPT | SOC_REG_FLAG_GENERAL_COUNTER | SOC_REG_FLAG_SIGNAL)) {
            *is_filter = 1;
            return rv;
    }

    /*additional filetr per device*/
#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit))
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_test_brdc_blk_filter, (unit, reg, is_filter));
        if (rv < 0)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                        (BSL_META_U(unit, "ERROR: unit %d register %d : %s\n"), unit, reg, soc_errmsg(rv)));
            return rv;
        }
        return rv;
    }
#endif

    return rv;
}
/*
 * Function:
 *      brdc_blk_test_reg_addr_get
 * Purpose:
 *      Reading a register value of any kind: 32, 64, above 64.
 *      Giving the addr and the schan block number.
 *  
 * Parameters:
 *      unit                        - (IN)  Unit number
 *      reg                         - (IN)  relevant register of the broadcast block
 *      acc_type                    - (IN)  access type
 *      addr                        - (IN)  register address
 *      block                       - (IN)  schan block number to read from
 *      data                        - (OUT) register value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
brdc_blk_test_reg_addr_get(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t *data)
{
    int reg_size;
    uint64 data64;
    uint32 data32;
    int rv;

    SOC_REG_ABOVE_64_CLEAR(*data);

    if (SOC_REG_IS_ABOVE_64(unit, reg)) 
    {
        reg_size = SOC_REG_ABOVE_64_INFO(unit, reg).size;
        return soc_direct_reg_get(unit, block, addr, reg_size, *data);
    }  else if (SOC_REG_IS_64(unit, reg)) {
        
        rv =  _soc_reg64_get(unit, block, acc_type, addr, &data64);
        SOC_REG_ABOVE_64_WORD_SET(*data, COMPILER_64_LO(data64), 0);
        SOC_REG_ABOVE_64_WORD_SET(*data, COMPILER_64_HI(data64), 1);
        return rv;
    }  else {
        rv = _soc_reg32_get(unit, block, acc_type, addr, &data32);
        SOC_REG_ABOVE_64_WORD_SET(*data, data32 , 0);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      brdc_blk_test
 * Purpose:
 *      Main Broadcast Block test function -
 *      This test role is to set a specific register  in broadcast block,
 *      And to make sure that all the blocks that controlled by the broadcast block changed.
 *  
 * Parameters:
 *      unit                        - (IN)  Unit number.
 *      a                           - (IN)  test args - not used
 *      pa                          - (IN)  not used
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
brdc_blk_test_dfe(int unit, args_t *a, void *pa)
{
    int rv = BCM_E_NONE;
    soc_block_t brdc_block;
    soc_reg_t reg;
    soc_reg_above_64_val_t reg_above_64, reg_above_64_get;
    int block_dummy;
    uint8 acc_type;
    uint32 addr;
    int index, blk_instance;
    soc_reg_brdc_block_info_t *brdc_blk_info = NULL;
    int count, test_block;
    int is_filter;
    int blk_index;
    int result = BCM_E_NONE;
    char *reg_name;
#if defined(SOC_NO_NAMES) 
    char buffer[15];
#endif

    /*write value is fixed to 1*/
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_WORD_SET(reg_above_64, 1, 0);

    brdc_blk_info = sal_alloc(sizeof(*brdc_blk_info) * BRDC_BLOCKS_TEST_MAX_BLOCKS, "brdc_blk_test.brdc_blk_info");
    if(brdc_blk_info == NULL) {
        cli_out("Memory allocation failure\n");
        return CMD_FAIL;
    }
    rv = brdc_blk_test_info_get(unit, BRDC_BLOCKS_TEST_MAX_BLOCKS ,brdc_blk_info, &count);
    if (rv < 0)
    {
        LOG_ERROR(BSL_LS_APPL_TESTS,
                    (BSL_META_U(unit, "brdc_blk_test: ERROR: unit %d : %s\n"), unit, soc_errmsg(rv)));
        sal_free(brdc_blk_info);
        return rv;
    }

    for (test_block = 0; test_block < count; test_block++)
    {

        brdc_block = brdc_blk_info[test_block].blk_type;

        /*Iterate over all relevant block registers (brdc_block)*/
        for (reg = 0; reg < NUM_SOC_REG; reg++) 
        {
            if (!SOC_REG_IS_VALID(unit, reg)) 
            {
                continue;
            }
     
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, reg).block, brdc_block)) 
            {
#if defined(SOC_NO_NAMES)
                sal_sprintf(buffer, "%d", reg);
                reg_name = buffer;
#else
                reg_name = SOC_REG_NAME(unit, reg);
#endif
                /*register filter*/
                rv = brdc_blk_test_reg_filter(unit, reg, &is_filter);
                if (rv < 0)
                {
                    LOG_ERROR(BSL_LS_APPL_TESTS,
                                (BSL_META_U(unit, "brdc_blk_test: ERROR: unit %d, register %s : %s\n"), unit, reg_name, soc_errmsg(rv)));
                    sal_free(brdc_blk_info);
                    return rv;
                }
                if (is_filter)
                {
                    LOG_VERBOSE(BSL_LS_APPL_TESTS,
                        (BSL_META_U(unit, "brdc_blk_test: Filtering unit %d register %s\n"), unit, reg_name));
                    continue;
                }

                LOG_VERBOSE(BSL_LS_APPL_TESTS,
                                (BSL_META_U(unit, "brdc_blk_test: Testing unit %d register %s \n"), unit, reg_name));
                
                /*Iterate over all possible indexes*/
                for (index = 0; index < SOC_REG_INFO(unit, reg).numels; index++)
                {
                    /*write to broadcast block*/
                    rv = soc_reg_above_64_set(unit, reg, 0, index, reg_above_64);
                    if (rv < 0)
                    {
                        LOG_ERROR(BSL_LS_APPL_TESTS,
                                    (BSL_META_U(unit, "brdc_blk_test: ERROR: unit %d, register %s : %s\n"), unit, reg_name, soc_errmsg(rv)));
                        sal_free(brdc_blk_info);
                        return rv;
                    }

                    /*read all relevant blocks and make sure equals*/
                    addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, index,
                                            SOC_REG_ADDR_OPTION_NONE,
                                            &block_dummy, &acc_type);
                    for (blk_index = 0; brdc_blk_info[test_block].blk_ids[blk_index] != -1 ; blk_index++)
                    {
                        blk_instance = brdc_blk_info[test_block].blk_ids[blk_index]; /*schan number of the relevant blk*/

                        SOC_REG_ABOVE_64_CLEAR(reg_above_64_get);

                         rv = brdc_blk_test_reg_addr_get(unit, reg, acc_type, addr, SOC_BLOCK2SCH(unit, blk_instance), &reg_above_64_get);
                         if (rv < 0)
                         {
                             LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR: unit %d, register %s : %s\n"), unit, reg_name, soc_errmsg(rv)));
                             result = BCM_E_FAIL;
                             continue;
                         }

                         if (!SOC_REG_ABOVE_64_IS_EQUAL(reg_above_64_get, reg_above_64))
                         {
                             LOG_ERROR(BSL_LS_APPL_TESTS,
                                            (BSL_META_U(unit, "brdc_blk_test: ERROR: unit %d, register %s : %s\n"), unit, reg_name, soc_errmsg(BCM_E_FAIL)));
                             result = BCM_E_FAIL;
                             continue;
                         }
                    } /*blk controled by the brdc blk iteration*/

                } /*index iteration*/
            }

        } /*reg iteration*/

    } /*brdc blk iteration*/

    sal_free(brdc_blk_info);
    return result;
}

#endif /*defined(BCM_DFE_SUPPORT) */

#if defined(BCM_SAND_SUPPORT)

/*
 * Function:
 *      test_brdc_mem_filter
 * Purpose:
 *      Exclude list for memories very similar to TR 6 and TR 7.
 *  
 * Parameters:
 *      unit                        - (IN)  Unit number
 *      mem                         - (IN)  relevant memory of the broadcast block
 *      is_filtered                 - (OUT) Shows if the memory is in the exclude list
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

STATIC int
test_brdc_mem_filter(int unit, soc_mem_t mem, int *is_filtered)
{
    int rv = BCM_E_NONE;

    *is_filtered = 0;

    switch(mem)
    {
        case BRDC_QRH_FFLBm:
        case BRDC_QRH_MCLBTm:
        case BRDC_QRH_MCSFFm:
        case BRDC_QRH_MNOLm:
        case FSRD_FSRD_PROM_MEMm:
        case QRH_FFLBm:
        case QRH_MCLBTm:
        case QRH_MCSFFm:
        case QRH_MNOLm:
        case FSRD_FSRD_WL_EXT_MEMm:
		case IPS_CRBALTHm:
            *is_filtered = 1;
            break;       /* Skip these tables */
        default:
            break;
    }
    return rv;
}

/*
 * Function:
 *      brdc_blk_test
 * Purpose:
 *      Main Broadcast Block test function -
 *      This test role is to set a specific register  in broadcast block,
 *      And to make sure that all the blocks that controlled by the broadcast block changed.
 *  
 * Parameters:
 *      unit                        - (IN)  Unit number.
 *      a                           - (IN)  test args - not used
 *      pa                          - (IN)  not used
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

int
brdc_blk_test(int unit, args_t *a, void *pa)
{
    int rv = BCM_E_NONE;
    soc_block_t blk;
    soc_block_t none_brdc_block_type;
    soc_reg_t reg;
    soc_reg_t brdc_reg;

    soc_mem_t mem;
    soc_mem_t brdc_mem;

    soc_reg_above_64_val_t reg_above_64, reg_above_64_get;
    soc_block_types_t regblktype;
    char *s;
    int block_dummy;
    uint8 acc_type;
    uint32 addr =  0;
    int delayed_errors = 0;
    int reg_size = 0, reg_size_word = 0;
    int mem_size = 0, mem_size_word = 0;
    int instance;
    int reg_tested = 0, mem_tested = 0;
    int data;
    int field;
    int result = BCM_E_NONE;
    int is_filter = 0;
    int brdc_count = 0;
    int max_number_of_blocks = 0;
    int i;
    soc_block_t brdc_block[MAX_NOF_BROADCAST_BLOCKS]; /* The soc IDs of the broadcast blocks */
    int blk_first_instance = 0;
    /* FOR BRDC set and get regular block */
    char *reg_name = NULL;
    char *mem_name = NULL;

    /* broadcast block name: SOC_BLOCK_NAME(unit,blk)+BRDC_PREFIX_LEN */
    /* write value is fixed to 1 with setting value 1 in one word only */
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    SOC_REG_ABOVE_64_WORD_SET(reg_above_64, 1, 0);
    /* Calling the old function for old FE's */
    if ((s = ARG_GET(a)) != NULL)
    {
        result = BCM_E_FAIL;
        LOG_WARN(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit,
                             "WARNING: unknown argument '%s'\n Please run it without arguments.\n"), s));
        return result;
    }
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        return brdc_blk_test_dfe(unit, a, pa);
    }
#endif
    /* Loop over all the blocks to find all of the BRDC blocks */
    for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
        max_number_of_blocks++;
        if (sal_strncmp(SOC_BLOCK_NAME(unit,blk), "BRDC_", BRDC_PREFIX_LEN) == 0) {
            if (!SOC_BLOCK_IS_BROADCAST(unit, blk)) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: Found block is not BRDC blocks: %d : %s\n"), 
                                        brdc_count, soc_errmsg(rv)));
                delayed_errors++;
                continue;
            }
            /* verify SOC_BLOCK_IS_BROADCAST(unit, blk) */
            if (brdc_count >= MAX_NOF_BROADCAST_BLOCKS) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Reached the max number of BRDC blocks: %d : %s\n"), 
                                        brdc_count, soc_errmsg(rv)));
                delayed_errors++;
                break;
                /* Print error if reaching MAX_NOF_BROADCAST_BLOCKS */
            }
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) && (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0)
                && (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_BRDC_HBC))
            {
                continue;
            }
            else
            {
#endif
                brdc_block[brdc_count] = blk;
                brdc_count++;
#ifdef BCM_DNX_SUPPORT
            }
#endif

        } else if (SOC_BLOCK_IS_BROADCAST(unit, blk)) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: Block: %d is marked as a broadcast block but is not named BRDC_ : %s\n"), 
                                        blk, soc_errmsg(rv)));
            delayed_errors++;
        }
    }

    /* Looping that data with 0 and 1 to test both values */
    for(data = 0 ; data < 2; data++) {
        /* Test each broadcast block */
        for (i = 0; i < brdc_count; i++) {
            blk_first_instance = SOC_BLOCK_BROADCAST_MEMBER(unit, brdc_block[i], 0);
            reg_tested = 0;
            mem_tested = 0;
            none_brdc_block_type = SOC_BLOCK_INFO(unit, blk_first_instance).type;
            for (reg = 0; reg < NUM_SOC_REG; reg++) {
                /*Get a good register for testing */
                if (!SOC_REG_IS_VALID(unit, reg) || !SOC_REG_IS_ENABLED(unit, reg) || (SOC_REG_INFO(unit,reg).flags &
                        (SOC_REG_FLAG_RO | SOC_REG_FLAG_WO | SOC_REG_FLAG_ARRAY | SOC_REG_FLAG_INTERRUPT | SOC_REG_FLAG_GENERAL_COUNTER | SOC_REG_FLAG_SIGNAL))) {
                    continue;
                }

                regblktype = SOC_REG_INFO(unit, reg).block;
                /* We found a suitable register in the block for testing. Test it. */
                if (SOC_BLOCK_IS(regblktype, none_brdc_block_type)) {
                /* Check the size of  the register by counting the fields and storing the number in reg_size */
                    reg_size = 0;
                    for (field = 0; field < (int)(SOC_REG_INFO(unit, reg).nFields); field++)
                    {
                        reg_size += soc_reg_field_length(unit, reg, SOC_REG_INFO(unit, reg).fields->field);
                    } /*getting the size of register*/

                    if (SOC_REG_IS_ABOVE_64(unit, reg)) {
                        reg_size_word = SOC_REG_ABOVE_64_INFO(unit, reg).size;
                    } else if (SOC_REG_IS_64(unit, reg)) {
                        reg_size_word = 2;
                    } else {
                        reg_size_word = 1;
                    }
                    SOC_REG_ABOVE_64_WORD_SET(reg_above_64, data, 0);
                    addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                                    SOC_REG_ADDR_OPTION_NONE,
                                                    &block_dummy, &acc_type);
                    blk = SOC_BLOCK_BROADCAST_MEMBER(unit, brdc_block[i], 0);
                    rv = soc_direct_reg_set(unit,SOC_BLOCK2SCH(unit, brdc_block[i]), addr, reg_size_word, reg_above_64);
                    LOG_INFO(BSL_LS_APPL_TESTS,
                            (BSL_META("Write using the ADDR reg %s: value %u \n"),
                                SOC_REG_NAME(unit,reg), *reg_above_64));
                    if (rv < 0)
                    {
                         LOG_ERROR(BSL_LS_APPL_TESTS,
                                    (BSL_META_U(unit, "brdc_blk_test: ERROR ADDR set: unit %d, register %d : %s\n"), unit, reg, soc_errmsg(rv)));
                         result = BCM_E_FAIL;
                    }
                    for (instance = 0; instance < SOC_BLOCK_BROADCAST_SIZE(unit, brdc_block[i]) ; ++instance) {
                        /* Read all relevant blocks and make sure equals */
                        blk = SOC_BLOCK_BROADCAST_MEMBER(unit, brdc_block[i], instance);
                        if (!SOC_INFO(unit).block_valid[blk])
                        {
                            LOG_INFO(BSL_LS_APPL_TESTS,
                                (BSL_META("blk %d (instance=%d) %s is not valid, skip it \n"),
                                blk, instance, SOC_BLOCK_NAME(unit,blk)));
                            continue;
                        }

                        rv = soc_reg_above_64_get(unit, reg, instance, 0, reg_above_64_get);
                        LOG_INFO(BSL_LS_APPL_TESTS,
                            (BSL_META("Read reg %s instance %d : value %u \n"),
                             SOC_REG_NAME(unit,reg), instance, *reg_above_64_get));
                        if (rv < 0)
                        {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Getting: unit %d, register %d : %s\n"), unit, reg, soc_errmsg(rv)));
                            result = BCM_E_FAIL;
                            continue;
                        }
                        /* Checking if the written data is the same as the one that we read */
                        if (!SHR_BITEQ_RANGE(reg_above_64_get, reg_above_64, 0, reg_size)) /*Compare only the amount of bits in the register */
                        {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Comparing: unit %d, register %d : %s\n"), unit, reg, soc_errmsg(BCM_E_FAIL)));
                            result = BCM_E_FAIL;
                            continue;
                        }
                        /* Getting the address of the register for another testing */
                        addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                                SOC_REG_ADDR_OPTION_NONE,
                                                &block_dummy, &acc_type);
                        /* Another way for reading the register is by the Address and the block */
                        rv = soc_direct_reg_get(unit, SOC_BLOCK2SCH(unit, blk), addr, reg_size_word, reg_above_64_get);
                        LOG_INFO(BSL_LS_APPL_TESTS,
                            (BSL_META("Read addr 0x%x: value %u \n"),
                             addr, *reg_above_64_get));
                        if (rv < 0)
                        {
                             LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR ADDR: unit %d, register %d : %s\n"), unit, reg, soc_errmsg(rv)));
                             result = BCM_E_FAIL;
                             continue;
                        }
                        /* Checking if the written data is the same as the one that we read */
                        if (!SHR_BITEQ_RANGE(reg_above_64_get, reg_above_64, 0, reg_size)) /*Compare only the amount of bits in the register */
                        {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Comparing ADDR: unit %d, register %d : %s\n"), unit, reg, soc_errmsg(BCM_E_FAIL)));
                            result = BCM_E_FAIL;
                            continue;
                        }

                    }
                    reg_name = sal_alloc(sizeof(char)*100,"brdc_blk_test.reg_name");
                    sal_memset(reg_name, 0, sizeof(char)*100);
                    sal_strncpy_s(reg_name,"BRDC_", sizeof(char)*100);
                    sal_strncat(reg_name, SOC_REG_NAME(unit,reg), sizeof(char)*100);
                    for (brdc_reg = 0; brdc_reg < NUM_SOC_REG; brdc_reg++) {
                        if (sal_strcmp(reg_name, SOC_REG_NAME(unit, brdc_reg)) == 0) {
                            rv = soc_reg_above_64_set(unit, brdc_reg, REG_PORT_ANY, 0 , reg_above_64);
                            if (rv < 0)
                            {
                                LOG_INFO(BSL_LS_APPL_TESTS,
                                            (BSL_META_U(unit, "brdc_blk_test: Register does not exist with BRDC_<reg_name>: unit %d, register %d\n"), unit, reg));
                            }
                            LOG_INFO(BSL_LS_APPL_TESTS,
                                (BSL_META("Write to BRDC reg %s: value %u \n"),
                                 reg_name, *reg_above_64));
                            for (instance = 0; instance < SOC_BLOCK_BROADCAST_SIZE(unit, brdc_block[i]) ; ++instance) {
                                blk = SOC_BLOCK_BROADCAST_MEMBER(unit, brdc_block[i], instance);
                                if (!SOC_INFO(unit).block_valid[blk])
                                {
                                    LOG_INFO(BSL_LS_APPL_TESTS,
                                        (BSL_META("blk %d (instance=%d) %s is not valid, skip it \n"),
                                        blk, instance, SOC_BLOCK_NAME(unit,blk)));
                                    continue;
                                }

                                rv = soc_reg_above_64_get(unit, reg, instance, 0, reg_above_64_get);
                                LOG_INFO(BSL_LS_APPL_TESTS,
                                    (BSL_META("Read reg %s instance %d : value %u \n"),
                                     SOC_REG_NAME(unit,reg), instance, *reg_above_64_get));
                                if (rv < 0)
                                {
                                    LOG_ERROR(BSL_LS_APPL_TESTS,
                                                (BSL_META_U(unit, "brdc_blk_test: ERROR Getting: unit %d, register %d : %s\n"), unit, reg, soc_errmsg(rv)));
                                    result = BCM_E_FAIL;
                                    continue;
                                }
                                /* Checking if the written data is the same as the one that we read */
                                if (!SHR_BITEQ_RANGE(reg_above_64_get, reg_above_64, 0, reg_size)) /*Compare only the amount of bits in the register */
                                {
                                    LOG_ERROR(BSL_LS_APPL_TESTS,
                                                (BSL_META_U(unit, "brdc_blk_test: ERROR Comparing: unit %d, register %d : %s\n"), unit, reg, soc_errmsg(BCM_E_FAIL)));
                                    result = BCM_E_FAIL;
                                    continue;
                                }
                            }            
                        }
                    }
                    sal_free(reg_name);
                    reg_tested++;
                    break;
                }
            }
            if (reg_tested == 0) {
                LOG_ERROR(BSL_LS_APPL_TESTS,
                                                (BSL_META_U(unit, "brdc_blk_test: ERROR NO registers were tested : %s\n"), soc_errmsg(BCM_E_FAIL)));
                delayed_errors++;
            }
            /* TO BE DONE: Add a memory DMA test */
            /* Testing BRDC memories if they exist */
            for (mem = 0; mem < NUM_SOC_MEM; mem++) {
                if (!SOC_MEM_IS_VALID(unit, mem)) {
                    continue;
                }
                if (soc_mem_index_count(unit, mem) == 0) {
                    continue;
                }
                if (!SOC_MEM_IS_ENABLED(unit, mem)) {
                    continue;
                }
                if ((SOC_MEM_INFO(unit,mem).flags &
                    (SOC_MEM_FLAG_READONLY | SOC_MEM_FLAG_WRITEONLY | SOC_MEM_FLAG_SIGNAL))) {
                    continue;
                }
                test_brdc_mem_filter(unit, mem, &is_filter);
                if (is_filter > 0) {
                    continue;
                }

                blk =  SOC_MEM_BLOCK_MIN(unit, mem);
                /* Checked if the memory is in a BRDC block */
                if(blk == blk_first_instance) {
                    SOC_REG_ABOVE_64_WORD_SET(reg_above_64, data, 0);
                    /*Getting the size of the memory*/
                    for (field = 0; field < (int)(SOC_MEM_INFO(unit, mem).nFields); field++)
                    {
                        mem_size += soc_mem_field_length(unit, mem, SOC_MEM_INFO(unit, mem).fields->field);
                    }
                    mem_size_word = BITS2WORDS(SOC_MEM_WORDS(unit, mem));
                    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, reg_above_64);
                    LOG_INFO(BSL_LS_APPL_TESTS,
                        (BSL_META("Write mem %s: value %u \n"),
                         SOC_MEM_NAME(unit, mem), *reg_above_64));
                    if (rv < 0)
                    {
                         LOG_ERROR(BSL_LS_APPL_TESTS,
                                    (BSL_META_U(unit, "brdc_blk_test: ERROR ADDR: unit %d, memory %s : %s\n"), unit, SOC_MEM_NAME(unit, mem), soc_errmsg(rv)));
                         result = BCM_E_FAIL;
                    }
                    /* Looping all over the instances to preform read and write to a memory */
                    for (instance = 0; instance < SOC_BLOCK_BROADCAST_SIZE(unit, brdc_block[i]); instance++) {
                        blk = SOC_BLOCK_BROADCAST_MEMBER(unit, brdc_block[i], instance);
                        if (!SOC_INFO(unit).block_valid[blk])
                        {
                            LOG_INFO(BSL_LS_APPL_TESTS,
                                (BSL_META("blk %d (instance=%d) %s is not valid, skip it \n"),
                                blk, instance, SOC_BLOCK_NAME(unit,blk)));
                            continue;
                        }
                        
                        rv = soc_mem_read(unit, mem, instance, 0, reg_above_64_get);
                        LOG_INFO(BSL_LS_APPL_TESTS,
                            (BSL_META("Read mem %s instance %d : value %u \n"),
                             SOC_MEM_NAME(unit, mem), instance, *reg_above_64_get));
                        if (rv < 0)
                        {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Reading: unit %d, memory %d : %s\n"), unit, mem, soc_errmsg(rv)));
                            return rv;
                        }
                        /* Comparing the data from the read with the one from the write */
                        if (!SHR_BITEQ_RANGE(reg_above_64_get, reg_above_64, 0, mem_size)) /*Compare only the amount of bits in the memory */
                        {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Comparing: unit %d, memory %s : %s\n"), unit, SOC_MEM_NAME(unit, mem), soc_errmsg(BCM_E_FAIL)));
                            result = BCM_E_FAIL;
                        }
                        addr = soc_mem_addr_get(unit, mem, 0, SOC_MEM_BLOCK_ANY(unit, mem), instance, &acc_type);
                        /* Another way for reading the register is by the Address and the block */
                        rv = soc_direct_memreg_get(unit, SOC_BLOCK2SCH(unit, blk), addr, mem_size_word, 1, reg_above_64_get);
                        LOG_INFO(BSL_LS_APPL_TESTS,
                            (BSL_META("Read Memory addr 0x%x: value %u \n"),
                             addr, *reg_above_64_get));
                        if (rv < 0)
                        {
                             LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR ADDR: unit %d, memory %s : %s\n"), unit, SOC_MEM_NAME(unit, mem), soc_errmsg(rv)));
                             result = BCM_E_FAIL;
                        }
                        if (!SHR_BITEQ_RANGE(reg_above_64_get, reg_above_64, 0, mem_size)) /*Compare only the amount of bits in the memory */
                        {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Comparing: unit %d, memory %s : %s\n"), unit, SOC_MEM_NAME(unit, mem), soc_errmsg(BCM_E_FAIL)));
                            result = BCM_E_FAIL;
                        }
                    }
                    rv = soc_direct_memreg_set(unit, SOC_BLOCK2SCH(unit, brdc_block[i]), addr, mem_size_word, 1, reg_above_64);
                    LOG_INFO(BSL_LS_APPL_TESTS,
                        (BSL_META("Write to ADDR of a Memory addr 0x%x: value %u \n"),
                         addr, *reg_above_64));
                    if (rv < 0)
                    {
                         LOG_ERROR(BSL_LS_APPL_TESTS,
                                    (BSL_META_U(unit, "brdc_blk_test: ERROR ADDR: unit %d, memory %s : %s\n"), unit, SOC_MEM_NAME(unit, mem), soc_errmsg(rv)));
                         result = BCM_E_FAIL;
                    }
                    /* Looping all over the instances to preform read and write to a memory */
                    for (instance = 0; instance < SOC_BLOCK_BROADCAST_SIZE(unit, brdc_block[i]); instance++) {
                        blk = SOC_BLOCK_BROADCAST_MEMBER(unit, brdc_block[i], instance);
                        if (!SOC_INFO(unit).block_valid[blk])
                        {
                            LOG_INFO(BSL_LS_APPL_TESTS,
                                (BSL_META("blk %d (instance=%d) %s is not valid, skip it \n"),
                                blk, instance, SOC_BLOCK_NAME(unit,blk)));
                            continue;
                        }

                        rv = soc_mem_read(unit, mem, instance, 0, reg_above_64_get);
                        LOG_INFO(BSL_LS_APPL_TESTS,
                            (BSL_META("Read mem %s instance %d : value %u \n"),
                             SOC_MEM_NAME(unit, mem), instance, *reg_above_64_get));
                        if (rv < 0)
                        {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Reading: unit %d, memory %d : %s\n"), unit, mem, soc_errmsg(rv)));
                            return rv;
                        }
                        /* Comparing the data from the read with the one from the write */
                        if (!SHR_BITEQ_RANGE(reg_above_64_get, reg_above_64, 0, mem_size)) /*Compare only the amount of bits in the memory */
                        {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Comparing: unit %d, memory %s : %s\n"), unit, SOC_MEM_NAME(unit, mem), soc_errmsg(BCM_E_FAIL)));
                            result = BCM_E_FAIL;
                        }
                        addr = soc_mem_addr_get(unit, mem, 0, SOC_MEM_BLOCK_ANY(unit, mem), instance, &acc_type);
                        /* Another way for reading the register is by the Address and the block */
                        rv = soc_direct_memreg_get(unit, SOC_BLOCK2SCH(unit, blk), addr, mem_size_word, 1, reg_above_64_get);
                        LOG_INFO(BSL_LS_APPL_TESTS,
                            (BSL_META("Read Memory addr 0x%x: value %u \n"),
                             addr, *reg_above_64_get));
                        if (rv < 0)
                        {
                             LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR ADDR: unit %d, memory %s : %s\n"), unit, SOC_MEM_NAME(unit, mem), soc_errmsg(rv)));
                             result = BCM_E_FAIL;
                        }
                        if (!SHR_BITEQ_RANGE(reg_above_64_get, reg_above_64, 0, mem_size)) /*Compare only the amount of bits in the memory */
                        {
                            LOG_ERROR(BSL_LS_APPL_TESTS,
                                        (BSL_META_U(unit, "brdc_blk_test: ERROR Comparing: unit %d, memory %s : %s\n"), unit, SOC_MEM_NAME(unit, mem), soc_errmsg(BCM_E_FAIL)));
                            result = BCM_E_FAIL;
                        }
                    }
                    mem_name = sal_alloc(sizeof(char)*100,"brdc_blk_test.mem_name");
                    sal_memset(mem_name, 0, sizeof(char)*100);
                    sal_strncpy_s(mem_name,"BRDC_", sizeof(char)*100);
                    sal_strncat(mem_name, SOC_MEM_NAME(unit, mem), sizeof(char)*100);
                    for (brdc_mem = 0; brdc_mem < NUM_SOC_MEM; brdc_mem++) {
                        if (sal_strcmp(mem_name, SOC_MEM_NAME(unit, brdc_mem)) == 0) {
                            rv = soc_mem_write(unit, brdc_mem, MEM_BLOCK_ALL, 0, reg_above_64);
                            if (rv < 0)
                            {
                                LOG_INFO(BSL_LS_APPL_TESTS,
                                            (BSL_META_U(unit, "brdc_blk_test: Memory does not exist with BRDC_<mem_name>: unit %d, memory %d\n"), unit, brdc_mem));
                            } else {
                                LOG_INFO(BSL_LS_APPL_TESTS,
                                    (BSL_META("Write to BRDC mem %s: value %u \n"),
                                     mem_name, *reg_above_64));
                                for (instance = 0; instance < SOC_BLOCK_BROADCAST_SIZE(unit, brdc_block[i]); instance++) {
                                    rv = soc_mem_read(unit, mem, instance, 0, reg_above_64_get);
                                    if (rv < 0)
                                    {
                                        LOG_ERROR(BSL_LS_APPL_TESTS,
                                                    (BSL_META_U(unit, "brdc_blk_test: ERROR Reading: unit %d, memory %d : %s\n"), unit, mem, soc_errmsg(rv)));
                                        sal_free(mem_name);
                                        return rv;
                                    }
                                    LOG_INFO(BSL_LS_APPL_TESTS,
                                        (BSL_META("Read mem %s instance %d : value %u \n"),
                                         SOC_MEM_NAME(unit, mem), instance, *reg_above_64_get));
                                    /* Comparing the data from the read with the one from the write */
                                    if (!SHR_BITEQ_RANGE(reg_above_64_get, reg_above_64, 0, mem_size)) /*Compare only the amount of bits in the memory */
                                    {
                                        LOG_ERROR(BSL_LS_APPL_TESTS,
                                                    (BSL_META_U(unit, "brdc_blk_test: ERROR Comparing: unit %d, memory %s : %s\n"), unit, SOC_MEM_NAME(unit, mem), soc_errmsg(BCM_E_FAIL)));
                                        result = BCM_E_FAIL;
                                    }
                                }
                            }
                        }
                    }
                    sal_free(mem_name);
                    mem_tested++;
                    break;
                }
            }
            if (mem_tested == 0) {
                LOG_INFO(BSL_LS_APPL_TESTS,
                                    (BSL_META("Warning no memories were found and tested ! \n")));
            }
        }
    }
    if (delayed_errors > 0) {
        LOG_ERROR(BSL_LS_APPL_TESTS,
                    (BSL_META_U(unit, "brdc_blk_test: |The number of the delayed_errors is: %d  : %s \n"), delayed_errors, soc_errmsg(BCM_E_FAIL)));
        result = BCM_E_FAIL;
    }
    return result;
}

#endif

