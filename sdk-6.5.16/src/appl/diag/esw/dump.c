/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * socdiag dump command
 */

#include <sal/appl/pci.h>
#include <sal/core/sync.h>
#include <shared/bsl.h>
#include <soc/counter.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/phyctrl.h>
#include <soc/l2x.h>
#include <soc/ll.h>
#include <soc/cm.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/triumph3.h>
#endif
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
#include <soc/td2_td2p.h>
#endif
#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/trident3.h>
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif

#if defined(ALPM_ENABLE)
#include <soc/alpm.h>
#endif

#include <bcm/l2.h>
#include <bcm/pkt.h>
#include <bcm/time.h>


#if defined(BCM_WARM_BOOT_SUPPORT_SW_DUMP) || !defined(BCM_SW_STATE_DUMP_DISABLE)
#include <bcm/mirror.h>
#include <bcm_int/esw/port.h> 
#include <bcm_int/esw/stack.h> 
#include <bcm_int/esw/trunk.h> 
#include <bcm_int/esw/vlan.h> 
#include <bcm_int/esw/l2.h> 
#include <bcm_int/esw/link.h> 
#include <bcm_int/esw/mcast.h> 
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/multicast.h> 
#include <bcm_int/esw/stat.h> 
#include <bcm_int/esw/stg.h> 
#include <bcm_int/esw/qos.h> 
#include <bcm_int/esw/mpls.h> 
#include <bcm/l3.h> 
#include <bcm_int/esw/subport.h> 
#include <bcm/mpls.h> 
#include <bcm/extender.h> 
#include <bcm_int/esw/failover.h> 
#include <bcm_int/esw/l3.h> 
#include <bcm_int/esw/ipmc.h> 
#include <bcm_int/esw/cosq.h> 
#include <bcm_int/common/time.h> 
#include <bcm_int/esw/trx.h> 
#include <bcm_int/esw/niv.h> 
#include <bcm_int/esw/extender.h> 
#include <bcm_int/esw/trill.h> 
#include <bcm_int/esw/oam.h> 
#include <bcm_int/esw/fcoe.h>
#include <bcm_int/esw/ecn.h> 
#include <bcm/pstats.h>
#include <bcm_int/esw/flow.h>
#if defined(INCLUDE_PSTATS)
#include <bcm_int/esw/pstats.h>
#endif /* INCLUDE_PSTATS */
#ifdef BCM_FIELD_SUPPORT
#include <bcm_int/esw/field.h>
#include <bcm/field.h>
#endif  /* BCM_FIELD_SUPPORT */
#endif /* defined(BCM_WARM_BOOT_SUPPORT_SW_DUMP) || !defined(BCM_SW_STATE_DUMP_DISABLE) */
#ifdef BCM_TRIUMPH3_SUPPORT
#include <bcm_int/esw/l2gre.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
#include <bcm_int/esw/vxlan.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef INCLUDE_REGEX
#include <bcm_int/regex_api.h>
#endif
#if defined(INCLUDE_FLOWTRACKER)
#include <bcm_int/esw/flowtracker.h>
#endif
#if defined(BCM_FLOWTRACKER_SUPPORT)
#include <bcm_int/esw/flowtracker/ft_group.h>
#endif
#if defined(ALPM_ENABLE)
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/alpm.h>
#endif
#endif

#ifdef BCM_IPFIX_SUPPORT
#include <bcm/ipfix.h>
#include <bcm_int/esw/ipfix.h>
#endif

#if defined(INCLUDE_BFD)
#include <bcm_int/esw/bfd.h> 
#endif

#if defined(BCM_COLLECTOR_SUPPORT)
#include <bcm_int/esw/collector.h>
#endif

#if defined(INCLUDE_TELEMETRY)
#include <bcm_int/esw/telemetry.h>
#endif

#include <appl/diag/system.h>
#include <appl/diag/decode.h>
#include <appl/diag/sysconf.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/bslfile.h>

#include <ibde.h>

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/format.h>
#endif
#ifndef BCM_SW_STATE_DUMP_DISABLE
#include <bcm_int/esw/udf.h>
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#define DUMP_PHY_COLS	4
#define DUMP_MW_COLS	4
#define DUMP_MH_COLS	8
#define DUMP_MB_COLS	16

#ifdef BCM_CMICM_SUPPORT
static void
_do_dump_cpureg(int unit)
{
    soc_reg_t       reg;
    int             idx;
    uint32          off, val;

    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (!SOC_REG_IS_VALID(unit, reg)) {
            continue;
        }
        if (!SOC_REG_IS_ENABLED(unit, reg)) {
             continue;
        }
        if (SOC_REG_INFO(unit, reg).regtype == soc_cpureg) {
            idx = 0;
            off = SOC_REG_BASE(unit, reg);
            if (soc_feature(unit, soc_feature_time_v3_no_bs)){
                if (off >= CMIC_BS0_CONFIG_OFFSET &&
                    off <= CMIC_BS1_INITIAL_CRC_OFFSET){
                    continue;
                }
            }
            if (off >= CMIC_MMU_COSLC_COUNT_ADDR_OFFSET &&
                    off < CMIC_TIM0_TIMER1LOAD_OFFSET) {
                    continue;
            }
            if (SOC_REG_IS_ARRAY(unit, reg)) {
                for (idx = 0; idx < SOC_REG_NUMELS(unit,reg); idx ++) {
                    off += idx*4;
                }
            }

            /*
             * These registers do not ack the CPU until the FSCHAN read cycle is completed.
             * Meaning you must activate a proper FSCHAN read command in order to read these registers.
             */
            switch (reg) {
                case CMIC_CMC0_FSCHAN_DATA32r            :
                case CMIC_CMC0_FSCHAN_DATA64_LOr         :
                case CMIC_FSCHAN_DATA32r                 :
                case CMIC_FSCHAN_DATA64_LOr              :
                case CMIC_CMC1_FSCHAN_DATA32r            :
                case CMIC_CMC1_FSCHAN_DATA64_LOr         :
                case CMIC_CMC2_FSCHAN_DATA32r            :
                case CMIC_CMC2_FSCHAN_DATA64_LOr         :
                    continue;
                case CMIC_BS0_CLK_CTRLr                  : 
                case CMIC_BS0_CONFIGr                    :
                case CMIC_BS0_HEARTBEAT_CTRLr            :
                case CMIC_BS0_HEARTBEAT_DOWN_DURATIONr   : 
                case CMIC_BS0_HEARTBEAT_UP_DURATIONr     : 
                case CMIC_BS0_INITIAL_CRCr               :
                case CMIC_BS0_INPUT_TIME_0r              : 
                case CMIC_BS0_INPUT_TIME_1r              :
                case  CMIC_BS0_INPUT_TIME_2r             :
                case  CMIC_BS0_OUTPUT_TIME_0r            : 
                case CMIC_BS0_OUTPUT_TIME_1r             : 
                case  CMIC_BS0_OUTPUT_TIME_2r		 :	 
                case  CMIC_BS1_CLK_CTRLr                 : 
                case  CMIC_BS1_CONFIGr                   : 
                case  CMIC_BS1_HEARTBEAT_CTRLr           : 
                case  CMIC_BS1_HEARTBEAT_DOWN_DURATIONr  :
                case  CMIC_BS1_HEARTBEAT_UP_DURATIONr    :
                case  CMIC_BS1_INITIAL_CRCr              :
                case  CMIC_BS1_INPUT_TIME_0r             :
                case  CMIC_BS1_INPUT_TIME_1r             :
                case  CMIC_BS1_INPUT_TIME_2r             :
                case  CMIC_BS1_OUTPUT_TIME_0r            :
                case  CMIC_BS1_OUTPUT_TIME_1r            :
                case  CMIC_BS1_OUTPUT_TIME_2r            :
                case  CMIC_BS_CAPTURE_CTRLr              :
                case  CMIC_BS_CAPTURE_FREE_RUN_TIME_0r   :
                case  CMIC_BS_CAPTURE_FREE_RUN_TIME_1r   :
                case  CMIC_BS_CAPTURE_STATUSr            :
                case  CMIC_BS_CAPTURE_SYNC_TIME_0r       :
                case  CMIC_BS_CAPTURE_SYNC_TIME_1r       :
                case CMIC_BS_CAPTURE_SYNT_TIME_0r        :
                case CMIC_BS_CAPTURE_SYNT_TIME_1r        :
                case  CMIC_BS_CLK_CTRLr                  :
                case CMIC_BS_CLK_CTRL_0r                 :
                case CMIC_BS_CLK_CTRL_1r                 :
                case CMIC_BS_CLK_TOGGLE_TIME_0r          :
                case CMIC_BS_CLK_TOGGLE_TIME_1r          : 
                case CMIC_BS_CLK_TOGGLE_TIME_2r          :
                case CMIC_BS_CONFIGr                     :
                case CMIC_BS_DRIFT_RATEr                 :
                case CMIC_BS_HEARTBEAT_CTRLr             :
                case CMIC_BS_HEARTBEAT_DOWN_DURATIONr    :
                case CMIC_BS_HEARTBEAT_UP_DURATIONr      :
                case CMIC_BS_INITIAL_CRCr                :
                case CMIC_BS_INPUT_TIME_0r               :
                case CMIC_BS_INPUT_TIME_1r               :
                case CMIC_BS_INPUT_TIME_2r               :
                case CMIC_BS_OFFSET_ADJUST_0r            : 
                case CMIC_BS_OFFSET_ADJUST_1r            :
                case CMIC_BS_OUTPUT_TIME_0r              :
                case CMIC_BS_OUTPUT_TIME_1r              :
                case CMIC_BS_OUTPUT_TIME_2r              :
                case CMIC_BS_REF_CLK_GEN_CTRLr           :
                    if (SOC_IS_MONTEREY(unit)) {             
                        continue;
                    } 
                default:
                    /* Do Nothing */
                    break;
            }
            val = soc_pci_read(unit, off);
            cli_out("0x%04x %s(%d): 0x%x\n", off,SOC_REG_NAME(unit, reg), idx,val);
        }
     }
}
#endif

/*
 * Dump all of the CMIC registers.
 */

static void
do_dump_pcim(int unit, uint32 off_start, uint32 count)
{
    uint32 off, val = 0, off32;
    char *pcireg_name=NULL;
    uint32 unused_reg_index=0;
#if defined (BCM_TRIUMPH3_SUPPORT) && defined (BCM_CMICM_SUPPORT)
    uint32 unavail_reg_index=0;
#endif /* BCM_TRIUMPH3_SUPPORT */

    if ((off_start & 3) != 0) {
        cli_out("dump_pcim ERROR: offset must be a multiple of 4\n");
        return;
    }

    for (off = off_start; count--; off += 4) {
		off32 = off;
#if defined(BCM_IPROC_SUPPORT)  && defined(IPROC_NO_ATL)
		off32 += SOC_DRIVER(unit)->cmicd_base;
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
#ifdef BCM_CMICM_SUPPORT
        if ((off32 >= CMIC_MMU_COSLC_COUNT_ADDR_OFFSET &&
             off32 < CMIC_TIM0_TIMER1LOAD_OFFSET) && SOC_IS_TRIUMPH3(unit)) {
             if (unavail_reg_index == 0) {
                 if (unused_reg_index) {
                     cli_out(" -- 0x%04x \n########\n",off32-4);
                     unused_reg_index = 0;
                 }
                 cli_out("\n########\n UNAVAILABLE REG OFFSETS 0x%04x",off32);
                 if (!count) {
                     cli_out("\n######## \n");
                 }
                 unavail_reg_index=off;
             } else {
                 if (!count) {
                     cli_out(" -- 0x%04x \n########\n",off32);
                 }
             }
             continue;
        }
        if (unavail_reg_index) {
            cli_out(" -- 0x%04x \n########\n",off32);
            unavail_reg_index=0;
        }
#endif
#endif /* BCM_TRIUMPH3_SUPPORT */
        pcireg_name=soc_pci_off2name(unit, off);
        if (!((sal_strlen(pcireg_name) == 0) ||
             (sal_strcmp(pcireg_name,"???") == 0))) {
            val = soc_pci_read(unit, off);
        }
        if ((sal_strlen(pcireg_name) == 0) || 
            (sal_strcmp(pcireg_name,"???") == 0)) {
            if (unused_reg_index == 0) {
                cli_out("\n########\n UNUSED/UNKNOWN REG OFFSETS 0x%04x",off32);
                unused_reg_index=off;
            }
        } else {
            if (unused_reg_index) {
                cli_out(" -- 0x%04x \n########\n",off32);
                unused_reg_index=0;
            }
            cli_out("0x%04x %s: 0x%x\n", off32, soc_pci_off2name(unit, off), val);
        }
    }
    if (unused_reg_index) {
        cli_out("\n######## \n");
    }
}

/*
 * Dump all of the SOC register addresses, and if do_values is true,
 * read and dump their values along with the addresses.
 */

#define DREG_ADR_SEL_MASK     0xf       /* Low order 4 bits */
#define DREG_ADDR               0       /* Address only */
#define DREG_RVAL               1       /* Address and reset default value */
#define DREG_REGS               2       /* Address and real value */
#define DREG_DIFF               3       /* Addr & real value if != default */
#define DREG_CNTR               4       /* Address and real value if counter */

#define DREG_PORT_ALL -1
#define DREG_BLOCK_ALL -1

struct dreg_data {
    int unit;
    int dreg_select;
    int only_port;    /* Select which port/block.  -1 ==> all */
    int only_block;
};

extern void 
 reg_above_64_print(int unit, 
                    soc_regaddrinfo_t *ainfo, 
                    soc_reg_above_64_val_t val, 
                    uint32 flags,
                    char *fld_sep, 
                    int wrap);

static int
dreg(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    struct dreg_data *dd = data;
    uint32 value;
    uint64 val64, resetVal;
    char name[80];
    int is64, is_default, rv, isabove64=0;
    int no_match = FALSE;  /* If specific port/block requested, turns true */
    char rval_str[20];
    soc_reg_above_64_val_t    value_above_64, resval_above64;

    /* Filter (set no_match) on ports and blocks if selected. */
    if (dd->only_port != DREG_PORT_ALL) {
        /* Only print ports that match */
        if (ainfo->port != dd->only_port) {
            no_match = TRUE;
        }
    }

    if (dd->only_block != DREG_BLOCK_ALL) {
        /* Only print blocks that match */
        if (ainfo->block != dd->only_block) {
            no_match = TRUE;
        } else { /* Match found; undo no_match */
            no_match = FALSE;
        }
    }

    if (no_match) {
        return 0;
    }

    soc_reg_sprint_addr(unit, name, ainfo);

    if (dd->dreg_select == DREG_ADDR) {
	cli_out("0x%08x %s\n", ainfo->addr, name);
	return 0;
    }

    SOC_REG_RST_VAL_GET(unit, ainfo->reg, resetVal);
    format_uint64(rval_str, resetVal);

    if (dd->dreg_select == DREG_RVAL) {
	cli_out("0x%08x %s = 0x%s\n", ainfo->addr, name, rval_str);
	return 0;
    }

    if (SOC_REG_INFO(unit, ainfo->reg).flags & SOC_REG_FLAG_WO) {
	cli_out("0x%08x %s = Write Only\n", ainfo->addr, name);
	return 0;
    }

    if (SOC_REG_IS_SPECIAL(unit, ainfo->reg)) {
	cli_out("0x%08x %s = Requires special processing\n",
                ainfo->addr, name);
	return 0;
    }

    if (reg_mask_subset(unit, ainfo, NULL)) {
        /* Register does not exist on this port/cos */
        return 0;
    }

    is64 = SOC_REG_IS_64(unit, ainfo->reg);
    isabove64 = SOC_REG_IS_ABOVE_64(unit, ainfo->reg);

    if (is64) {
        if (ainfo->cos >= 0) {
  	        rv = soc_reg64_get(dd->unit, ainfo->reg, REG_PORT_ANY, ainfo->cos, 
                               &val64);
        } else {
	          rv = soc_reg64_get(dd->unit, ainfo->reg, ainfo->port, ainfo->idx, 
                               &val64);
    }
   	is_default = COMPILER_64_EQ(val64, resetVal);
    } else if (isabove64) {
        rv = soc_reg_above_64_get(
                        unit, ainfo->reg, ainfo->port, 
                        SOC_REG_IS_ARRAY(unit, ainfo->reg) ? 
                        ainfo->idx : 0, value_above_64);

        SOC_REG_ABOVE_64_RST_VAL_GET(unit, ainfo->reg, resval_above64);
        is_default = SOC_REG_ABOVE_64_IS_EQUAL(value_above_64, resval_above64);
    } else {
        if (ainfo->cos >= 0) {
	          rv = soc_reg32_get(dd->unit, ainfo->reg, REG_PORT_ANY, ainfo->cos, 
                               &value);
        } else {
	          rv = soc_reg32_get(dd->unit, ainfo->reg, ainfo->port, ainfo->idx, 
                               &value);
        }
        is_default = (value == COMPILER_64_LO(resetVal));
   }

   if (rv < 0) {
       cli_out("0x%08x %s = ERROR\n", ainfo->addr, name);
	     return 0;
   }
   if (dd->dreg_select == DREG_DIFF && is_default) {
       return 0;
   }
   if ((dd->dreg_select == DREG_CNTR)  &&
        (!(SOC_REG_INFO(unit, ainfo->reg).flags & SOC_REG_FLAG_COUNTER))) {
  	  return 0;
   }
   if (is64) {
       cli_out("0x%08x %s = 0x%08x%08x\n",
           ainfo->addr, name,
             COMPILER_64_HI(val64), COMPILER_64_LO(val64));
   } else if (isabove64) {
       cli_out("0x%08x %s = ", ainfo->addr, name);
       reg_above_64_print(unit, ainfo, value_above_64, REG_PRINT_HEX, ",", 62 );
   } else {
     	 cli_out("0x%08x %s = 0x%08x\n", ainfo->addr, name, value);
   }
   return 0;
}

static cmd_result_t
do_dump_soc(int unit, int dreg_select, int only_port, int only_block)
{
    struct dreg_data dd;

    dd.unit = unit;
    dd.dreg_select = dreg_select;
    dd.only_port = only_port;
    dd.only_block = only_block;

    (void) soc_reg_iterate(unit, dreg, &dd);

    return CMD_OK;
}

static void
_pci_print_config(int unit)
{
    uint32		data;
    int         cap_len, cap_base, next_cap_base, i;

    if ((soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) == 0) {
        return;
    }


    data = soc_cm_pci_conf_read(unit, PCI_CONF_VENDOR_ID);
    cli_out("%04x: %08x  DeviceID=%04x  VendorID=%04x\n",
            PCI_CONF_VENDOR_ID, data,
            (data & 0xffff0000) >> 16,
            (data & 0x0000ffff) >>  0);

    data = soc_cm_pci_conf_read(unit, PCI_CONF_COMMAND);
    cli_out("%04x: %08x  Status=%04x  Command=%04x\n",
            PCI_CONF_COMMAND, data,
            (data & 0xffff0000) >> 16,
            (data & 0x0000ffff) >>  0);
    cap_len = (data >> 16) & 0x10 ? 4 : 0;

    data = soc_cm_pci_conf_read(unit, PCI_CONF_REVISION_ID);
    cli_out("%04x: %08x  ClassCode=%06x  RevisionID=%02x\n",
            PCI_CONF_REVISION_ID, data,
            (data & 0xffffff00) >> 8,
            (data & 0x000000ff) >> 0);

    data = soc_cm_pci_conf_read(unit,  PCI_CONF_CACHE_LINE_SIZE);
    cli_out("%04x: %08x  BIST=%02x  HeaderType=%02x  "
            "LatencyTimer=%02x  CacheLineSize=%02x\n",
            PCI_CONF_CACHE_LINE_SIZE, data,
            (data & 0xff000000) >> 24,
            (data & 0x00ff0000) >> 16,
            (data & 0x0000ff00) >>  8,
            (data & 0x000000ff) >>  0);

    data = soc_cm_pci_conf_read(unit,  PCI_CONF_BAR0);
    cli_out("%04x: %08x  BaseAddress0=%08x\n",
            PCI_CONF_BAR0, data, data);

    data = soc_cm_pci_conf_read(unit,  PCI_CONF_BAR1);
    cli_out("%04x: %08x  BaseAddress1=%08x\n",
            PCI_CONF_BAR1, data, data);

    data = soc_cm_pci_conf_read(unit,  PCI_CONF_BAR2);
    cli_out("%04x: %08x  BaseAddress2=%08x\n",
            PCI_CONF_BAR2, data, data);

    data = soc_cm_pci_conf_read(unit,  PCI_CONF_BAR3);
    cli_out("%04x: %08x  BaseAddress3=%08x\n",
            PCI_CONF_BAR3, data, data);

    data = soc_cm_pci_conf_read(unit, PCI_CONF_BAR4);
    cli_out("%04x: %08x  BaseAddress4=%08x\n",
            PCI_CONF_BAR4, data, data);

    data = soc_cm_pci_conf_read(unit,  PCI_CONF_BAR5);
    cli_out("%04x: %08x  BaseAddress5=%08x\n",
            PCI_CONF_BAR5, data, data);

    data = soc_cm_pci_conf_read(unit,  PCI_CONF_CB_CIS_PTR);
    cli_out("%04x: %08x  CardbusCISPointer=%08x\n",
            PCI_CONF_CB_CIS_PTR, data, data);

    data = soc_cm_pci_conf_read(unit, PCI_CONF_SUBSYS_VENDOR_ID);
    cli_out("%04x: %08x  SubsystemID=%02x  SubsystemVendorID=%02x\n",
            PCI_CONF_SUBSYS_VENDOR_ID, data,
            (data & 0xffff0000) >> 16,
            (data & 0x0000ffff) >>  0);

    data = soc_cm_pci_conf_read(unit,  PCI_CONF_EXP_ROM);
    cli_out("%04x: %08x  ExpansionROMBaseAddress=%08x\n",
            PCI_CONF_EXP_ROM, data, data);

    data = soc_cm_pci_conf_read(unit, 0x34);
    cli_out("%04x: %08x  Reserved=%06x  CapabilitiesPointer=%02x\n",
            0x34, data,
            (data & 0xffffff00) >> 8,
            (data & 0x000000ff) >> 0);
    cap_base = cap_len ? data & 0xff : 0;

    data = soc_cm_pci_conf_read(unit, 0x38);
    cli_out("%04x: %08x  Reserved=%08x\n",
            0x38, data, data);

    data = soc_cm_pci_conf_read(unit, PCI_CONF_INTERRUPT_LINE);
    cli_out("%04x: %08x  Max_Lat=%02x  Min_Gnt=%02x  "
            "InterruptPin=%02x  InterruptLine=%02x\n",
            PCI_CONF_INTERRUPT_LINE, data,
            (data & 0xff000000) >> 24,
            (data & 0x00ff0000) >> 16,
            (data & 0x0000ff00) >>  8,
            (data & 0x000000ff) >>  0);

    data = soc_cm_pci_conf_read(unit,  0x40);
    cli_out("%04x: %08x  Reserved=%02x  "
            "RetryTimeoutValue=%02x  TRDYTimeoutValue=%02x\n",
            0x40, data,
            (data & 0xffff0000) >> 16,
            (data & 0x0000ff00) >>  8,
            (data & 0x000000ff) >>  0);

    data = soc_cm_pci_conf_read(unit, 0x44);
    cli_out("%04x: %08x  PLLConf=%01x\n",
            0x44, data,
            (data & 0x000000ff) >>  0);

    data = soc_cm_pci_conf_read(unit,0x48);
    cli_out("%04x: %08x  -\n",
            0x48, data);

    while (cap_base) {
        data = soc_cm_pci_conf_read(unit, cap_base);
        cli_out("%04x: %08x  CapabilityID=%02x "
                "CapabilitiesPointer=%02x ",
                cap_base, data, data & 0xff, (data >> 8) & 0xff);
        next_cap_base = (data >> 8) & 0xff;
        switch (data & 0xff) {
            case 0x01:
                cap_len = 2 * 4;
                cli_out("PWR-MGMT\n");
                break;
            case 0x03:
                cap_len = 2 * 4;
                cli_out("VPD\n");
                break;
            case 0x05:
                cap_len = 6 * 4; /* 3 to 6 DWORDS */
                cli_out("MSI\n");
                break;
            case 0x10:
                cap_len = 3 * 4;
                cli_out("PCIE\n");
                break;
            case 0x11:
                cap_len = 3 * 4;
                cli_out("MSI-X\n");
                break;
            default:
                break;
        }
        for (i = 4; i < cap_len; i += 4) {
            data = soc_cm_pci_conf_read(unit,cap_base + i);
            cli_out("%04x: %08x  -\n", cap_base + i, data);
        }
        cap_base = next_cap_base;
    }

#if defined(VXWORKS) && defined(INCLUDE_PCI)
    
    {
#ifdef IDTRP334
        extern void sysBusErrDisable(void);
        extern void sysBusErrEnable(void);
#endif

        /* HINT (R) HB4 PCI-PCI Bridge (21150 clone) */
#define HINT_HB4_VENDOR_ID    0x3388
#define HINT_HB4_DEVICE_ID    0x0022

        int BusNo, DevNo, FuncNo;
        unsigned short tmp;

#ifdef IDTRP334
        sysBusErrDisable();
#endif

        /*
         * HINTCORP HB4 PCI-PCI Bridge
         */
        if (pciFindDevice(HINT_HB4_VENDOR_ID,
                    HINT_HB4_DEVICE_ID,
                    0,
                    &BusNo, &DevNo, &FuncNo) != ERROR) {

            cli_out("-------------------------------------\n");
            cli_out("HB4 PCI-PCI Bridge Status Registers  \n");
            cli_out("-------------------------------------\n");

            /* Dump the status registers */
            pciConfigInWord(BusNo,DevNo,FuncNo, 0x06, &tmp);
            cli_out("Primary Status (%xh):   0x%x\n", 0x06, tmp);
            pciConfigInWord(BusNo,DevNo,FuncNo, 0x1e, &tmp);
            cli_out("Secondary Status (%xh): 0x%x\n", 0x1e, tmp);
            pciConfigInWord(BusNo,DevNo,FuncNo, 0x3e, &tmp);
            cli_out("Bridge Control (%xh):   0x%x\n", 0x3e, tmp);
            pciConfigInWord(BusNo,DevNo,FuncNo, 0x6a, &tmp);
            cli_out("P_SERR Status (%xh):    0x%x\n", 0x6a, tmp);
        }

#ifdef IDTRP334
        sysBusErrEnable();
#endif
    }
#endif
}

/*
 * Dump registers, tables, or an address space.
 */

static cmd_result_t
do_dump_registers(int unit, regtype_entry_t *rt, args_t *a)
{
    int		i;
    sal_vaddr_t vaddr;
    uint32  t1 = 0, t2 = 0;
    pbmp_t	pbmp;
    soc_port_t	port, dport;
    int		rv = CMD_OK;
    uint32	flags = DREG_REGS;
    int		dump_port = DREG_PORT_ALL;
    int		dump_block = DREG_BLOCK_ALL;
    char	*an_arg;
    char	*count;
#ifdef BCM_CMICX_SUPPORT
    uint32 min_reg_offset = 0, max_reg_offset = 0;
#endif

    an_arg = ARG_GET(a);
    count = ARG_GET(a);

    /* PCI config space does not take an offset */
    switch (rt->type) {
    case soc_pci_cfg_reg:
        _pci_print_config(unit);
        break;
    case soc_cpureg:
        if (an_arg) {
            if (parse_cmic_regname(unit, an_arg, &t1) < 0) {
                cli_out("ERROR: unknown CMIC register name: %s\n", an_arg);
                rv = CMD_FAIL;
                goto done;
            }
            t2 = count ? parse_integer(count) : 1;
        } else {
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm)) {
                if (soc_feature(unit, soc_feature_cpureg_dump)){
                    _do_dump_cpureg(unit);
                    break;
                }
            }
#endif

#ifdef BCM_CMICM_SUPPORT
            if(soc_feature(unit, soc_feature_cmicm)) {
                t1 = CMICM_OFFSET_MIN;
                t2 = (CMICM_OFFSET_MAX - CMICM_OFFSET_MIN) / 4 + 1;
            } else
#endif
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                soc_cmicx_reg_offset_get(&min_reg_offset, &max_reg_offset);
                t1 = min_reg_offset;
                t2 = (max_reg_offset - min_reg_offset) / 4 + 1;
            } else
#endif
            {
                t1 = CMIC_OFFSET_MIN;
                t2 = (CMIC_OFFSET_MAX - CMIC_OFFSET_MIN) / 4 + 1;
            }
        }

        do_dump_pcim(unit, t1, t2);
        break;
#ifdef BCM_CMICM_SUPPORT
    case soc_mcsreg:
        if (!an_arg) {
            cli_out("Dumping MCS memory requires address and "
                    "optional count\n");
            rv = CMD_FAIL;
            goto done;
        }
        t1 = parse_integer(an_arg) & ~3;
        t2 = count ? parse_integer(count) : 1;
        for (i = 0; i < (int)t2; i++, t1 += 4) {
            if ((i % DUMP_MW_COLS) == 0)
            cli_out("%08x: ", t1);
            cli_out("%08x%c",
                    soc_pci_mcs_read(unit, t1),
                    ((i + 1) % DUMP_MW_COLS) == 0 ? '\n' : ' ');
        }
        if (i % DUMP_MW_COLS)
            cli_out("\n");
        break;
#endif
    case soc_iprocreg:
        goto done;
        break;
    case soc_schan_reg:
    case soc_genreg:
    case soc_portreg:
    case soc_cosreg:

        while (an_arg) {
            if (sal_strcasecmp(an_arg, "addr") == 0) {
                flags = DREG_ADDR;
            } else if (sal_strcasecmp(an_arg, "rval") == 0) {
                flags = DREG_RVAL;
            } else if (sal_strcasecmp(an_arg, "diff") == 0) {
                flags = DREG_DIFF;
            } else if (sal_strcasecmp(an_arg, "counter") == 0) {
                flags = DREG_CNTR;
            } else if (sal_strcasecmp(an_arg, "port") == 0) {
                an_arg = ARG_GET(a);
                dump_port = an_arg ? parse_integer(an_arg) : 0;
            } else if (sal_strcasecmp(an_arg, "block") == 0) {
                an_arg = ARG_GET(a);
                dump_block = an_arg ? parse_integer(an_arg) : 0;
            } else {
                cli_out("ERROR: unrecognized argument to DUMP SOC: %s\n",
                        an_arg);
                return CMD_FAIL;
            }
            if (count != NULL) {
                an_arg = count;
                count = NULL;
            } else {
                an_arg = ARG_GET(a);
            }
	}
        rv = do_dump_soc(unit, flags, dump_port, dump_block);
	break;

    case soc_phy_reg:
	if (an_arg) {
	    if (parse_pbmp(unit, an_arg, &pbmp)) {
		cli_out("Error: Invalid port identifier: %s\n", an_arg);
		rv = CMD_FAIL;
		break;
	    }
	} else {
	    pbmp = PBMP_PORT_ALL(unit);
	}
	SOC_PBMP_AND(pbmp, PBMP_PORT_ALL(unit));
        /* coverity[overrun-local] */
        DPORT_SOC_PBMP_ITER(unit, pbmp, dport, port) {
	    uint8	phy_id = port_to_phyaddr(unit, port);
	    uint16	phy_data, phy_reg;
	    cli_out("\nPort %d (Phy ID %d)", port + 1, phy_id);
	    for (phy_reg = PHY_MIN_REG; phy_reg <= PHY_MAX_REG; phy_reg++) {
		rv = soc_miim_read(unit, phy_id, phy_reg, &phy_data);
		if (rv < 0) {
		    cli_out("Error: Port %d: cmic_read_miim failed: %s\n",
                            port + 1, soc_errmsg(rv));
		    rv = CMD_FAIL;
		    goto done;
		}
		cli_out("%s\t0x%02x: 0x%04x",
                        ((phy_reg % DUMP_PHY_COLS) == 0) ? "\n" : "",
                        phy_reg, phy_data);
	    }
	    cli_out("\n");
	}
	break;

    case soc_hostmem_w:
	if (!an_arg) {
	    cli_out("Dumping memory requires address and optional count\n");
	    rv = CMD_FAIL;
	    goto done;
	}
        vaddr = parse_address(an_arg) & ~3;
	t2 = count ? parse_integer(count) : 1;
	for (i = 0; i < (int)t2; i++, vaddr += 4) {
            uint32 *memptr = INT_TO_PTR(vaddr);
	    if ((i % DUMP_MW_COLS) == 0) {
            cli_out("%p: ", (void *)memptr);
            }
	    cli_out("%08x%c", *memptr,
                    ((i + 1) % DUMP_MW_COLS) == 0 ? '\n' : ' ');
	}
	if (i % DUMP_MW_COLS) {
	    cli_out("\n");
        }
	break;
    case soc_hostmem_h:
	if (!an_arg) {
	    cli_out("Dumping memory requires address and optional count\n");
	    rv = CMD_FAIL;
	    goto done;
	}
        vaddr = parse_address(an_arg) & ~1;
	t2 = count ? parse_integer(count) : 1;
	for (i = 0; i < (int)t2; i++, vaddr += 2) {
            uint16 *memptr = INT_TO_PTR(vaddr);
	    if ((i % DUMP_MH_COLS) == 0) {
            cli_out("%p: ", (void *)memptr);
            }
	    cli_out("%04x%c", *memptr,
                    ((i + 1) % DUMP_MH_COLS) == 0 ? '\n' : ' ');
	}
	if (i % DUMP_MH_COLS) {
	    cli_out("\n");
        }
	break;
    case soc_hostmem_b:
	if (!an_arg) {
	    cli_out("Dumping memory requires address and optional count\n");
	    rv = CMD_FAIL;
	    goto done;
	}
        vaddr = parse_address(an_arg);
        t2 = count ? parse_integer(count) : 1;
        for (i = 0; i < (int)t2; i++, vaddr += 1) {
            uint8 *memptr = INT_TO_PTR(vaddr);
            if ((i % DUMP_MB_COLS) == 0) {
                cli_out("%p: ", (void *)memptr);
            }
            cli_out("%02x%c", *memptr,
                    ((i + 1) % DUMP_MB_COLS) == 0 ? '\n' : ' ');
        }
	if (i % DUMP_MB_COLS) {
	    cli_out("\n");
    }
	break;
    default:
        cli_out("Dumping register type %s is not yet implemented.\n",
                rt->name);
        rv = CMD_FAIL;
	break;
    }

 done:
    return rv;
}

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC INLINE int
_mem_tr3_mem_reduced(int unit, soc_mem_t mem, uint32 copyno) 
{
    switch (mem) {
        case PORT_EHG_RX_TUNNEL_DATAm:
        case PORT_EHG_RX_TUNNEL_MASKm:
        case PORT_EHG_TX_TUNNEL_DATAm:
            if ((SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_XLPORT) ||
                (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_XWPORT)) {
                return TRUE;
            } else {
                return FALSE;
            }

        default:
            return FALSE;
    }
}
#endif

#define DUMP_TABLE_RAW       0x01
#define DUMP_TABLE_HEX       0x02
#define DUMP_TABLE_ALL       0x04
#define DUMP_TABLE_CHANGED   0x08
#define DUMP_TABLE_EXT       0x10
#define DUMP_TABLE_PIPE_X    0x20
#define DUMP_TABLE_PIPE_Y    0x40
#define DUMP_TABLE_PIPE_0    0x80
#define DUMP_TABLE_PIPE_1    0x100
#define DUMP_TABLE_PIPE_2    0x200
#define DUMP_TABLE_PIPE_3    0x400
#define DUMP_TABLE_NO_CACHE  0x800
#define DUMP_TABLE_PHY_INDEX 0x1000
#define DUMP_TABLE_SIMPLIFIED 0x2000
#define DUMP_TABLE_FORMAT 0x4000
#define DUMP_TABLE_PIPE_4 0x8000
#define DUMP_TABLE_PIPE_5 0x10000
#define DUMP_TABLE_PIPE_6 0x20000
#define DUMP_TABLE_PIPE_7 0x40000
#define DUMP_TABLE_ALL_DMA_THRESHOLD 8
#define DUMP_TABLE_ITER_BEFORE_YIELD 8

#if defined(BCM_TRIUMPH3_SUPPORT) || defined (BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT)
STATIC INLINE int
_mem_tr3_skippable(int unit, soc_mem_t mem, uint32 copyno) 
{
    if (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_IBOD) {
        return TRUE;
    }

/* Cannot dump the memory due to crash with pop command */
    switch (mem) {
        case ING_SER_FIFOm:
        case ING_SER_FIFO_Xm:
        case ING_SER_FIFO_Ym:
        case ISM_SER_FIFOm:
        case EGR_SER_FIFOm:
        case PORT_WC_UCMEM_DATAm:
#ifdef BCM_TOMAHAWK_SUPPORT
        case CENTRAL_CTR_EVICTION_FIFOm:
        case ING_SER_FIFO_PIPE0m:
        case ING_SER_FIFO_PIPE1m:
        case ING_SER_FIFO_PIPE2m:
        case ING_SER_FIFO_PIPE3m:
        case EGR_SER_FIFO_PIPE0m:
        case EGR_SER_FIFO_PIPE1m:
        case EGR_SER_FIFO_PIPE2m:
        case EGR_SER_FIFO_PIPE3m:
        case L2_MGMT_SER_FIFOm:
        case MMU_GCFG_MEM_FAIL_ADDR_64m:
        case MMU_SCFG_MEM_FAIL_ADDR_64_SC0m:
        case MMU_SCFG_MEM_FAIL_ADDR_64_SC1m:
        case MMU_XCFG_MEM_FAIL_ADDR_64_XPE0m:
        case MMU_XCFG_MEM_FAIL_ADDR_64_XPE1m:
        case MMU_XCFG_MEM_FAIL_ADDR_64_XPE2m:
        case MMU_XCFG_MEM_FAIL_ADDR_64_XPE3m:
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
        case MMU_SEDCFG_MEM_FAIL_ADDR_64m:
        case MMU_SEDCFG_MEM_FAIL_ADDR_64_SC0m:
        case MMU_SEDCFG_MEM_FAIL_ADDR_64_SC1m:
#endif /* BCM_TOMAHAWK2_SUPPORT */
            return TRUE;

        default:
            return FALSE;
    }
}
#endif

/*
 * Utility function to determine whether
 * to skip DMA and use PIO for certain mems
 */
STATIC uint8
soc_mem_dma_skip(int unit, soc_mem_t mem) {
    switch (mem) {
        case L2_MOD_FIFOm:
            return TRUE;

        default:
            return FALSE;
    }
}
#if defined(BCM_TOMAHAWK_SUPPORT)
#define IS_IFP_TCAM(unit, base_mem) ((SOC_IS_TOMAHAWKX(unit))? \
                                      (base_mem == IFP_TCAMm || base_mem == IFP_TCAM_WIDEm):0)
#else
#define IS_IFP_TCAM(unit, base_mem) 0
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
STATIC int
read_and_clear_learn_cache_entries(int unit, soc_mem_t mem, int copyno, int index, int count, int flags, char *field_names, int *sts)
{
    int rv;
    int rv1 = SOC_E_NONE;
    void *buffer;
    int num_bytes;
    uint16 dev_id;
    uint8 rev_id;
    int i;
    int entry_dw;
    char *buff;
    char lineprefix[256];

    if (!SOC_IS_TOMAHAWK3(unit)) {
        return CMD_FAIL;
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Proceed only if revision is A0 */
    if (rev_id != BCM56980_A0_REV_ID) {
        *sts = 1;
        return CMD_FAIL;
    }

    assert(copyno >= 0);

    num_bytes = soc_mem_entry_bytes(unit, mem);

    buffer = soc_cm_salloc(unit, num_bytes * count, "Dump L2 lrn cache buffer");
    if (buffer == NULL) {
        cli_out("ERROR: Dump buffer memory allocation for %d bytes failed\n",
                num_bytes * count);
        return CMD_FAIL;
    }

    buff = (char *)soc_cm_salloc(unit, num_bytes * count, "zero buffer");
    if (buff == NULL) {
        cli_out("read_and_clear_learn_cache_entries:"
                " Cannot allocate memory, %u bytes\n", num_bytes * count);
        rv = CMD_FAIL;
        goto done;
    }

    /* Zero out the whole memory range */
    sal_memset(buff, 0x0, num_bytes * count);

    entry_dw = soc_mem_entry_words(unit, mem);

    /* Read memory in to temp buffer, and clear memory at the same time,
     * while holding the lock
     */
    soc_mem_lock(unit, mem);
    rv = soc_mem_read_range(unit, mem, copyno, index, index + count - 1,
                            buffer);
    if (rv == SOC_E_NONE) {

        /*rv1 = soc_mem_clear(unit, mem, MEM_BLOCK_ALL, FALSE); */
        rv1 = soc_mem_write_range(unit, mem, copyno, index, index + count - 1, (void *)buff);
    }
    soc_mem_unlock(unit, mem);

    /* Free zero buffer */
    if (buff != NULL) {
        soc_cm_sfree(unit, buff);
        buff = NULL;
    }

    if (SOC_FAILURE(rv)) {
        cli_out("ERROR: soc_mem_read_range failed for %s failed\n",
                SOC_MEM_NAME(unit, mem));
        rv = CMD_FAIL;
        goto done;
    }

    if (SOC_FAILURE(rv1)) {
        cli_out("ERROR: soc_mem_clear failed for %s failed, error %d\n",
                SOC_MEM_NAME(unit, mem), rv1);
        rv = CMD_FAIL;
        goto done;
    }

    for (i = index; i < (index + count); i++) {
        uint32 *entry;
        int j;

        /* Point to the next entry in the buffer */
        entry = (uint32 *)((uint8 *)buffer + i * num_bytes);

        if (!(flags & DUMP_TABLE_ALL)) {
            if (soc_mem_field32_get(unit, mem, entry, VALIDf) == 0) {
                continue;
            }
        }

        if (flags & DUMP_TABLE_HEX) {
            for (j = 0; j < entry_dw; j++) {
                cli_out("%08x\n", entry[j]);
            }
        } else if (flags & DUMP_TABLE_CHANGED) {
            sal_sprintf(lineprefix, "%s.%s[%d]: ",
                        SOC_MEM_UFNAME(unit, mem),
                        SOC_BLOCK_NAME(unit, copyno),
                        i);
            if (field_names == NULL) {
                soc_mem_entry_dump_if_changed(unit, mem, (void *)entry, lineprefix);
            } else {
                soc_mem_entry_dump_if_changed_fields(unit, mem, (void *)entry,
                                                     lineprefix, field_names);
            }
        } else {
            cli_out("%s.%s[%d]: ",
                    SOC_MEM_UFNAME(unit, mem),
                    SOC_BLOCK_NAME(unit, copyno),
                    i);

            if (flags & DUMP_TABLE_RAW) {
                for (j = 0; j < entry_dw; j++) {
                    cli_out("0x%08x ", entry[j]);
                }
                cli_out("\n");
            } else {
                if (field_names == NULL) {
                    soc_mem_entry_dump(unit, mem, (void *)entry, BSL_LSS_CLI);
                } else {
                    soc_mem_entry_dump_fields(unit, mem, (void *)entry, field_names);
                }
            }
        }

         /* Yield CPU so other processes are not starved */
         if (((i - index + 1) % DUMP_TABLE_ITER_BEFORE_YIELD) == 0) {
             sal_thread_yield();
         }
    }

    rv = CMD_OK;

done:
    if (buffer != NULL) {
        soc_cm_sfree(unit, buffer);
        buffer = NULL;
    }

    return rv;
}
#endif /* BCM_TOMAHAWK3_SUPPORT */

cmd_result_t
do_dump_table(int unit, soc_mem_t mem,
              int copyno, int index, int count, int flags, char *field_names,
              int enable_mem_mode_check)
{
    int       k, i;
    uint32    entry[SOC_MAX_MEM_WORDS];
    char      lineprefix[256];
    int       entry_dw;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int       base_valid_count;
#endif
    int       rv = CMD_FAIL, rv1;
#if defined(BCM_TRIUMPH3_SUPPORT)
    int       tr3_special_case = (copyno >= 0) && SOC_IS_TRIUMPH3(unit) 
                                  && _mem_tr3_mem_reduced(unit, mem, copyno);
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    int       check_view = FALSE;
    soc_mem_t view = mem;
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    int       pipe_no;
    soc_mem_t base_mem;
    uint8     dual_mode = FALSE;
    int       check_lpm_view = FALSE;
    soc_mem_t lpm_view = INVALIDm;
    int       pair_lpm = 0;
#endif
    uint32    *mem_buffer = NULL;
    uint8     do_dma = FALSE;
    uint32    read_flags = SOC_MEM_NO_FLAGS;

    assert(copyno >= 0);

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_TOMAHAWK_SUPPORT)
    if ((SOC_IS_TRIUMPH3(unit) || (SOC_IS_TD2_TT2(unit))) &&
        _mem_tr3_skippable(unit, mem, copyno)){
        return CMD_OK;
    }
#endif
        
    entry_dw = soc_mem_entry_words(unit, mem);

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit) &&
        (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
         mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
         mem == L3_DEFIP_ALPM_IPV6_128m)) {
        check_view = TRUE;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) && (flags & DUMP_TABLE_PHY_INDEX)) {
        if (soc_feature(unit, soc_feature_l3_defip_map) &&
            (mem == L3_DEFIPm ||
#ifdef BCM_TOMAHAWK3_SUPPORT
             mem == L3_DEFIP_LEVEL1m ||
#endif
             mem == L3_DEFIP_ONLYm ||
             mem == L3_DEFIP_PAIR_128m ||
#ifdef BCM_TOMAHAWK3_SUPPORT
             mem == L3_DEFIP_PAIR_LEVEL1m ||
#endif
             mem == L3_DEFIP_PAIR_128_ONLYm)) {
            check_lpm_view = TRUE;
            lpm_view = mem;
            pair_lpm = (mem == L3_DEFIP_PAIR_128m ||
#ifdef BCM_TOMAHAWK3_SUPPORT
                        mem == L3_DEFIP_PAIR_LEVEL1m ||
#endif
                        mem == L3_DEFIP_PAIR_128_ONLYm) ? 1 : 0;
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
        /*
         * Dual mode mems need special handling per index,
         * so cannot DMA, except IFP_TCAMm and IFP_TCAM_WIDEm.
         */
    if (SOC_IS_TOMAHAWKX(unit)) {
        dual_mode = soc_th_mem_is_dual_mode(unit, mem, &base_mem, &pipe_no);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit) &&
        ((mem >= L2_LEARN_CACHEm) &&
         (mem <= L2_LEARN_CACHE_PIPE7m))) {
            if (soc_property_get(unit, spn_L2XLRN_CLEAR_ON_READ, 0)) {
                int ret_sts = 0;

                /* For rev A0, this function will display mem contents.
                 * For other revs, we proceed with the original code below
                 * this 'if'
                 */
                if (read_and_clear_learn_cache_entries(unit, mem, copyno,
                        index, count, flags, field_names, &ret_sts) ==
                        CMD_OK) {
                    return CMD_OK;
                } else {
                    /* If ret_sts is 0, a problem like mem alloc failed
                     * occured. If ret_sts is 1, device is not rev A0 -
                     * proceed with display processing below this 'if-else'
                     */
                    if (ret_sts == 0) {
                        return CMD_FAIL;
                    }
                }
            }
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    /*
     * Attempt to do a DMA read of table for d socmem for all
     * memories that don't need additional per index processing
     */
    if (soc_feature(unit, soc_feature_dump_socmem_uses_dma) &&
        (0 == (flags ^ DUMP_TABLE_ALL))) {
        do_dma = TRUE;
        /* Skip DMA if per index processing is needed */
        if (
#if defined(BCM_TRIUMPH3_SUPPORT)
                tr3_special_case ||
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
                check_view ||
#endif
                soc_mem_dma_skip(unit, mem)) {
            do_dma = FALSE;
        }
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) {
            if (dual_mode) {
                do_dma = FALSE;
            }
        }
#endif
        /*
         * Use threshold to determine whether to perform
         * DMA or fall back to PIO
         */
        if (count < DUMP_TABLE_ALL_DMA_THRESHOLD) {
            do_dma = FALSE;
        }

        if (TRUE == do_dma || IS_IFP_TCAM(unit, base_mem)) {
            do_dma = FALSE;
            if ((soc_mem_index_valid(unit, mem, index) &&
                 soc_mem_index_valid(unit, mem, index + count - 1)) ||
                 (flags & DUMP_TABLE_PHY_INDEX) ||
                    (SOC_DEFIP_HOLE_RANGE_CHECK(unit, mem, index))) {

                mem_buffer = soc_cm_salloc(unit, count * 4 * entry_dw,
                                           "dump socmem dma buffer");
                if (NULL != mem_buffer) {
                    do_dma = TRUE;
                }
            }
        }
        if (TRUE == do_dma)
        {
#ifdef BCM_TOMAHAWK_SUPPORT
            if (IS_IFP_TCAM(unit, base_mem) &&
                enable_mem_mode_check &&
                dual_mode == TRUE) {
                rv1 = soc_th_ifp_tcam_range_dma_read(unit, mem, copyno,
                                                     index, index + count - 1,
                                                     mem_buffer);
            } else
#endif
            {
                rv1 = soc_mem_read_range(unit, mem, copyno, index,
                                         index + count - 1, mem_buffer);
            }
            if (SOC_E_NONE != rv1) {
                /* Fallback to PIO on DMA failure */
                do_dma = FALSE;
            }
        }
    }

    for (k = index; k < index + count; k++) {
        if (!soc_mem_index_valid(unit, mem, k)) {
            if (!(SOC_DEFIP_HOLE_RANGE_CHECK(unit, mem, k))) {
                if (!(flags & DUMP_TABLE_PHY_INDEX)) {
                    continue;
                }
            }
        }
        sal_memset(entry, 0x00, SOC_MAX_MEM_WORDS * sizeof(uint32));
#if defined(BCM_TOMAHAWK_SUPPORT)
            /*
             * Dual mode mems need to judge whether index is valid or not
             */
        if (SOC_IS_TOMAHAWKX(unit)) {
            if (enable_mem_mode_check && dual_mode == TRUE) {
                if (TRUE != soc_th_dual_tcam_index_valid(unit, mem, k)) {
                    continue;
                }
            }
        }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (tr3_special_case) {
            if (k >= 16) {
                break;
            }
            i = soc_mem_read(unit, mem, copyno, k, entry);
        } else 
#endif
        {
#ifdef BCM_TRIDENT2_SUPPORT
            if (check_view) {
                if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
                    view = _soc_trident2_alpm_bkt_view_get(unit, k);
                } 
#if defined(ALPM_ENABLE)
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
                else {
                    view = soc_alpm_cmn_bkt_view_get(unit, k);
                }
#endif
#endif
                if (!(view == INVALIDm || mem == view)) {
                    continue;
                }
            }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
            if (check_lpm_view) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    lpm_view = _soc_th3_lpm_view_get(unit, k, pair_lpm);
                } else
#endif
                if (SOC_IS_TOMAHAWKX(unit)) {
                    lpm_view = _soc_th_lpm_view_get(unit, k, pair_lpm);
                }
                if (mem != lpm_view) {
                    continue;
                }
            }
#endif /* BCM_TOMAHAWK_SUPPORT */
            if (flags & DUMP_TABLE_PHY_INDEX) {
                read_flags = SOC_MEM_DONT_MAP_INDEX;
            }
            if (flags & DUMP_TABLE_PIPE_X) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_X, k, 
                                             entry);
            } else if (flags & DUMP_TABLE_PIPE_Y) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_Y, k, 
                                             entry);
            } else if (flags & DUMP_TABLE_PIPE_0) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_0, k,
                                             entry);
            } else if (flags & DUMP_TABLE_PIPE_1) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_1, k,
                                             entry);
            } else if (flags & DUMP_TABLE_PIPE_2) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_2, k,
                                             entry);
            } else if (flags & DUMP_TABLE_PIPE_3) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_3, k,
                                             entry);
            } else if (flags & DUMP_TABLE_PIPE_4) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_4, k,
                                             entry);
            } else if (flags & DUMP_TABLE_PIPE_5) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_5, k,
                                             entry);
            } else if (flags & DUMP_TABLE_PIPE_6) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_6, k,
                                             entry);
            } else if (flags & DUMP_TABLE_PIPE_7) {
                i = soc_mem_pipe_select_read(unit, read_flags, mem, copyno,
                                             _SOC_MEM_ADDR_ACC_TYPE_PIPE_7, k,
                                             entry);
            } else if (flags & DUMP_TABLE_NO_CACHE) {
                i = soc_mem_read_no_cache(unit, read_flags, mem, 0, copyno, k, entry);
            } else {
                if (soc_feature(unit, soc_feature_dump_socmem_uses_dma) &&
                    do_dma) {
                    sal_memcpy(entry,
                               (uint32*)&mem_buffer[entry_dw * (k - index)],
                               4 * entry_dw);
                    i = SOC_E_NONE;
                } else {
                    i = soc_mem_read_physical_index(unit, read_flags, mem,
                                                    copyno, k, entry);
                }
            }
        }

        if (i < 0) {
            cli_out("Read ERROR: table %s.%s[%d]: %s\n",
                    SOC_MEM_UFNAME(unit, mem),
                    SOC_BLOCK_NAME(unit, copyno), k, soc_errmsg(i));
            goto done;
        }

        if (!(flags & DUMP_TABLE_ALL)) {
            int    validf;

            validf = -1;
#if defined(BCM_TRX_SUPPORT)
            if (soc_mem_field_length(unit, mem, ACTIVEf) > 0) {
                validf = ACTIVEf;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
            }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
            if (mem == EXT_L2_ENTRY_TCAMm) {
                validf = FREEf;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 1) {
                    continue;
                }
            }
#endif /* BCM_TRIUMPH_SUPPORT */

            if (soc_mem_field_length(unit, mem, VALIDf) > 0) {
                validf = VALIDf;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
            }
#ifdef BCM_TOMAHAWK3_SUPPORT
            base_valid_count = 0;
            if (soc_mem_field_length(unit, mem, BASE_VALIDf) > 0) {
                validf = BASE_VALIDf;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
            }
            if (soc_mem_field_length(unit, mem, BASE_VALID_0f) > 0) {
                validf = BASE_VALID_0f;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    base_valid_count++;
                }
            }
            if (soc_mem_field_length(unit, mem, BASE_VALID_1f) > 0) {
                validf = BASE_VALID_1f;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    base_valid_count++;
                }
            }
            if (soc_mem_field_length(unit, mem, BASE_VALID_2f) > 0) {
                validf = BASE_VALID_2f;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    base_valid_count++;
                }
            }
            if (soc_mem_field_length(unit, mem, BASE_VALID_3f) > 0) {
                validf = BASE_VALID_3f;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    base_valid_count++;
                }
            }
            if (base_valid_count == 4) {
                continue;
            }
#endif
#ifdef BCM_TRIDENT2_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_MEM_IS_VALID(unit, L3_DEFIP_LEVEL1m) &&
                soc_mem_field_length(unit, mem, LWR_VALID0f) > 0 &&
                soc_mem_field_length(unit, mem, LWR_VALID1f) > 0 &&
                soc_mem_field_length(unit, mem, UPR_VALID0f) > 0 &&
                soc_mem_field_length(unit, mem, UPR_VALID1f) > 0) {
                if (soc_mem_field32_get(unit, mem, entry, LWR_VALID0f) == 0 &&
                    soc_mem_field32_get(unit, mem, entry, LWR_VALID1f) == 0 &&
                    soc_mem_field32_get(unit, mem, entry, UPR_VALID0f) == 0 &&
                    soc_mem_field32_get(unit, mem, entry, UPR_VALID1f) == 0) {
                    continue;
                }
            } else
#endif

            if (soc_mem_field_length(unit, mem, VALID0_LWRf) > 0 &&
                soc_mem_field_length(unit, mem, VALID1_LWRf) > 0 &&
                soc_mem_field_length(unit, mem, VALID0_UPRf) > 0 &&
                soc_mem_field_length(unit, mem, VALID1_UPRf) > 0) {
                if (soc_mem_field32_get(unit, mem, entry, VALID0_LWRf) == 0 &&
                    soc_mem_field32_get(unit, mem, entry, VALID1_LWRf) == 0 &&
                    soc_mem_field32_get(unit, mem, entry, VALID0_UPRf) == 0 &&
                    soc_mem_field32_get(unit, mem, entry, VALID1_UPRf) == 0) {
                    continue;
                }
            }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                if (soc_mem_field_length(unit, mem, BASE_VALIDf) > 0) {
                    if ((soc_mem_field32_get(unit, mem, entry, BASE_VALIDf) != 1) ||
                        (soc_mem_field32_get(unit, mem, entry, KEY_TYPEf) != 0)) {
                        continue;
                    }
                }
                /* Double tables have 2 base valids, quad tables 4.This check covers both */
                if (soc_mem_field_length(unit, mem, BASE_VALID_0f) > 0 &&
                    soc_mem_field_length(unit, mem, BASE_VALID_1f) > 0) {
                    if (soc_mem_field_length(unit, mem, BASE_VALID_2f) > 0 &&
                        soc_mem_field_length(unit, mem, BASE_VALID_3f) > 0) {
                        if (mem == L3_ENTRY_QUADm) {
                            if (soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 1 ||
                                   (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 2) ||
                                   (soc_mem_field32_get(unit, mem, entry, BASE_VALID_2f) != 2) ||
                                   (soc_mem_field32_get(unit, mem, entry, BASE_VALID_3f) != 7)) {
                                continue;
                            }
                        }
                    } else {
                        if (mem == L3_ENTRY_DOUBLEm) {
                            if (soc_mem_field32_get(unit, mem, entry, KEY_TYPEf) == 2) {
                                if ((soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 1) ||
                                    (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 2)) {
                                    continue;
                                }
                            } else if (soc_mem_field32_get(unit, mem, entry, KEY_TYPEf) == 4) {
                                if ((soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 1) ||
                                    (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 7)) {
                                    continue;
                                }
                            } else {
                                continue;
                            }

                        }
                    }
                }
            }
#endif

#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                if (soc_mem_field_length(unit, mem, BASE_VALIDf) > 0) {
                    switch (mem) {
                        case FT_KEY_SINGLEm:
                            if ((soc_mem_field32_get(unit, mem, entry, BASE_VALIDf) != 1) &&
                                    (soc_mem_field32_get(unit, mem, entry, BASE_VALIDf) != 2)) {
                                continue;
                            }
                            break;
                        default:
                            if (soc_mem_field32_get(unit, mem, entry, BASE_VALIDf) != 1 ) {
                                continue;
                            }
                            break;
                    }
                }
                /* Double tables have 2 base valids, quad tables 4.This check covers both */
                if (soc_mem_field_length(unit, mem, BASE_VALID_0f) > 0 &&
                    soc_mem_field_length(unit, mem, BASE_VALID_1f) > 0) {
                    if (soc_mem_field_length(unit, mem, BASE_VALID_2f) > 0 &&
                        soc_mem_field_length(unit, mem, BASE_VALID_3f) > 0) {
                        if ((mem == L3_ENTRY_QUADm) &&
                            (soc_mem_field32_get(unit, mem, entry, KEY_TYPEf) == 2)) {
                            /* V6 Extended entry*/
                            if ((soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 3) ||
                                (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 4) ||
                                (soc_mem_field32_get(unit, mem, entry, BASE_VALID_2f) != 7) ||
                                (soc_mem_field32_get(unit, mem, entry, BASE_VALID_3f) != 7)) {
                                continue;
                            }
                        } else if (soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 5 ||
                                   (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 6 &&
                                    soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 7) ||
                                   (soc_mem_field32_get(unit, mem, entry, BASE_VALID_2f) != 6 &&
                                    soc_mem_field32_get(unit, mem, entry, BASE_VALID_2f) != 7) ||
                                   (soc_mem_field32_get(unit, mem, entry, BASE_VALID_3f) != 6 &&
                                    soc_mem_field32_get(unit, mem, entry, BASE_VALID_3f) != 7)) {
                            continue;
                        }
                    } else {
                        switch (mem) {
                            case FT_KEY_DOUBLEm:
                                if ((soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 1) ||
                                    (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 2)) {
                                    continue;
                                }
                                break;
                            case L3_ENTRY_DOUBLEm:
                                /* V4 extended view */
                                if ((soc_mem_field32_get(unit, mem, entry, DATA_TYPEf) == 1)) {
                                    if ((soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 1) ||
                                        (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 7)) {
                                        continue;
                                    }
                                } else if ((soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 3) ||
                                           (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 4) ||
                                           (soc_mem_field32_get(unit, mem, entry, DATA_TYPEf) == 0)) {
                                    continue;
                                }
                                break;
                            default:
                                if (soc_mem_field32_get(unit, mem, entry, BASE_VALID_0f) != 3 ||
                                    (soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 4 &&
                                     soc_mem_field32_get(unit, mem, entry, BASE_VALID_1f) != 7)) {
                                    continue;
                                }
                                break;
                        }
                    }
                }
            }
#endif
#ifdef BCM_FIREBOLT_SUPPORT
            if (soc_mem_field_length(unit, mem, VALID0f) > 0) {
                validf = VALID0f;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    if (soc_mem_field_length(unit, mem, VALID1f) > 0) {
                        validf = VALID1f;
                        if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                            continue;
                        }
                    }
                }
            }

            if (soc_mem_field_length(unit, mem, VALID_0f) > 0) {
                validf = VALID_0f;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
            }

            if (soc_mem_field_length(unit, mem, VALID_1f) > 0) {
                validf = VALID_1f;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
            }

            if (soc_mem_field_length(unit, mem, VALID_2f) > 0) {
                validf = VALID_2f;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
            }

            if (soc_mem_field_length(unit, mem, VALID_3f) > 0) {
                validf = VALID_3f;
                if (soc_mem_field32_get(unit, mem, entry, validf) == 0) {
                    continue;
                }
            }
#endif /* BCM_FIREBOLT_SUPPORT */
        }

        if (flags & DUMP_TABLE_HEX) {
            for (i = 0; i < entry_dw; i++) {
                cli_out("%08x\n", entry[i]);
            }
        } else if (flags & DUMP_TABLE_CHANGED) {
            sal_sprintf(lineprefix, "%s.%s[%d]: ",
                        SOC_MEM_UFNAME(unit, mem),
                        SOC_BLOCK_NAME(unit, copyno),
                        k);
            if (field_names == NULL) {
                soc_mem_entry_dump_if_changed(unit, mem, entry, lineprefix);
            } else {
                soc_mem_entry_dump_if_changed_fields(unit, mem, entry, 
                                                     lineprefix, field_names);
            }
        } else {
            cli_out("%s.%s[%d]: ",
                    SOC_MEM_UFNAME(unit, mem),
                    SOC_BLOCK_NAME(unit, copyno),
                    k);

            if (flags & DUMP_TABLE_RAW) {
                for (i = 0; i < entry_dw; i++) {
                    cli_out("0x%08x ", entry[i]);
                }
                cli_out("\n");
            } else {
                if (field_names == NULL) {
                    soc_mem_entry_dump(unit, mem, entry, BSL_LSS_CLI);
                } else {
                    soc_mem_entry_dump_fields(unit, mem, entry,field_names);
                }
            }
        }
#ifdef ALPM_ENABLE
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (soc_feature(unit, soc_feature_alpm2) && mem == L3_DEFIP_ALPM_RAWm) {
            int8 fmt = 0;
            char *fmt_pfx[] = {
                "  NA:",
                "  V4:",
                "  V6_64:",
                "  V6_128:",
                "  V4_1:",
                "  V6_64_1:",
                "  V6_128:"
            };

#if defined(BCM_TRIDENT3_SUPPORT)
            fmt = _soc_trident3_alpm2_bnk_fmt_get(unit, k);
#endif
            th_alpm_table_ent_dump(unit, 1, mem, entry, copyno, k, fmt,
                                   -1, flags, fmt_pfx[fmt]);
        }
#endif
#endif
        /* Yield CPU so other processes are not starved */
        if (((k - index + 1) % DUMP_TABLE_ITER_BEFORE_YIELD ) == 0) {
            sal_thread_yield();
        }
    }

    rv = CMD_OK;

 done:
    if (NULL != mem_buffer) {
        soc_cm_sfree(unit, mem_buffer);
        mem_buffer = NULL;
    }
    return rv;
}
#ifndef BCM_SW_STATE_DUMP_DISABLE
STATIC cmd_result_t 
do_dump_sw(int unit, args_t *a) 
{ 
    char *c; 
    int  dump_all = FALSE; 
        
    if ((c = ARG_GET(a)) == NULL) { 
        return CMD_USAGE; 
    } 
     
    if (!sal_strcasecmp(c, "all")) { 
        dump_all = TRUE; 
    }
#ifndef BCM_SW_STATE_DUMP_DISABLE
    if (dump_all || !sal_strcasecmp(c, "cosq")) {
        _bcm_cosq_sw_dump(unit);
    }
    if (dump_all || !sal_strcasecmp(c, "mcast")) {
        _bcm_mcast_sw_dump(unit);
    }
#endif /* BCM_SW_STATE_DUMP_DISABLE */ 
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP 
    if (dump_all || !sal_strcasecmp(c, "stack")) { 
        _bcm_stk_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "time")) { 
        _bcm_time_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "mirror")) { 
        _bcm_mirror_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "stat")) { 
        _bcm_stat_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "qos")) { 
        _bcm_esw_qos_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "fcoe")) { 
        _bcm_esw_fcoe_sw_dump(unit); 
    } 
    if (dump_all || !sal_strcasecmp(c, "ecn")) {
        _bcm_esw_ecn_sw_dump(unit);
    }
#if defined(INCLUDE_PSTATS)
    if (dump_all || !sal_strcasecmp(c, "pstats")) {
        _bcm_esw_pstats_sw_dump(unit);
    }
#endif /* INCLUDE_PSTATS */
#endif  /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
#ifdef BCM_FIELD_SUPPORT
    if (dump_all || !sal_strcasecmp(c, "fp")) {
        _bcm_esw_field_udf_show(unit, "FP UDF");
    }
    if (dump_all || !sal_strcasecmp(c, "ifp") || !sal_strcasecmp(c, "fp")) {
        _bcm_esw_field_range_show(unit, "IFP RANGE", _BCM_FIELD_STAGE_INGRESS);
        _bcm_esw_field_show(unit, "IFP ", _BCM_FIELD_STAGE_INGRESS);
    }
    if (dump_all || !sal_strcasecmp(c, "efp") || !sal_strcasecmp(c, "fp")) {
        _bcm_esw_field_show(unit, "EFP ", _BCM_FIELD_STAGE_EGRESS);
    }
    if (dump_all || !sal_strcasecmp(c, "vfp") || !sal_strcasecmp(c, "fp")) {
        _bcm_esw_field_show(unit, "VFP ", _BCM_FIELD_STAGE_LOOKUP);
    }
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (dump_all || !sal_strcasecmp(c, "fp_class") || !sal_strcasecmp(c, "fp")) {
        _bcm_esw_field_show(unit, "CLASS ", _BCM_FIELD_STAGE_CLASS);
    }
    if (dump_all || !sal_strcasecmp(c, "fp_em") || !sal_strcasecmp(c, "fp")) {
        _bcm_esw_field_show(unit, "EXACT_MATCH ", _BCM_FIELD_STAGE_EXACTMATCH);
    }
#endif  /* BCM_TOMAHAWK_SUPPORT */
#endif  /* BCM_FIELD_SUPPORT */

    if (dump_all || !sal_strcasecmp(c, "stg")) {
        _bcm_stg_sw_dump(unit);
    }
    if (dump_all || !sal_strcasecmp(c, "link")) {
        _bcm_link_sw_dump(unit);
    }
    if (dump_all || !sal_strcasecmp(c, "port")) {
        _bcm_port_sw_dump(unit);
    }
    if (dump_all || !sal_strcasecmp(c, "trunk")) {
        _bcm_trunk_sw_dump(unit);
    }
#ifdef INCLUDE_L3 
#if defined(BCM_FIREBOLT_SUPPORT)
    if (dump_all || !sal_strcasecmp(c, "l3")) {
        _bcm_l3_sw_dump(unit);
    }
#endif
    if (dump_all || !sal_strcasecmp(c, "niv")) { 
        _bcm_niv_sw_dump(unit); 
    } 
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover)) {
        if (dump_all || !sal_strcasecmp(c, "failover")) { 
            _bcm_tr2_failover_sw_dump(unit); 
        } 
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
    if (soc_feature(unit, soc_feature_trill)) {
        if (dump_all || !sal_strcasecmp(c, "trill")) { 
            _bcm_td_trill_sw_dump(unit); 
        } 
    }
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_vxlan) ||
        soc_feature(unit, soc_feature_vxlan_lite)) {
        if (dump_all || !sal_strcasecmp(c, "vxlan")) { 
            _bcm_vxlan_sw_dump(unit); 
        } 
    }
#endif /* BCM_TRIDENT2_SUPPORT || BCM_GREYHOUND2_SUPPORT */

    if (dump_all || !sal_strcasecmp(c, "extender")) {
        _bcm_extender_sw_dump(unit);
    }
#endif  /* INCLUDE_L3 */ 
    if (dump_all || !sal_strcasecmp(c, "l2")) {
        _bcm_l2_sw_dump(unit);
    }

    if (dump_all || !sal_strcasecmp(c, "vlan")) {
        _bcm_vlan_sw_dump(unit);
    }
#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (dump_all || !sal_strcasecmp(c, "mpls")) {
        _bcm_mpls_sw_dump(unit);
    }
#endif

    if (soc_feature(unit, soc_feature_flex_port)) {
        if (dump_all || !sal_strcasecmp(c, "flexport")) {
            char *pipe_str = NULL;
            char *speed_str = NULL;
            int pipe=0, speed_id=0;

            if ((pipe_str = ARG_GET(a)) != NULL) {
                if (isint(pipe_str)) {
                    pipe = parse_integer(pipe_str);
                }
                if ((speed_str = ARG_GET(a)) != NULL) {
                    if (isint(speed_str)) {
                        speed_id = parse_integer(speed_str);
                    }
                    _bcm_esw_flexport_sw_pipe_speed_group_dump(unit,
                                                               pipe,
                                                               speed_id);
                }
            } else {
        _bcm_esw_flexport_sw_dump(unit);
        }
    }
    }

#if defined(INCLUDE_FLOWTRACKER)
    if (soc_feature(unit, soc_feature_uc_flowtracker_learn) ||
        soc_feature(unit, soc_feature_uc_flowtracker_export) ||
        soc_feature(unit, soc_feature_mv2_style_flowtracker)) {
        if (dump_all || !sal_strcasecmp(c, "flowtracker")) {
            _bcm_esw_flowtracker_sw_dump(unit);
        }
    }
#endif /* INCLUDE_FLOWTRACKER */

#if defined(BCM_FLOWTRACKER_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        if (dump_all || !sal_strcasecmp(c, "flowtracker")) {
             bcmi_esw_flowtracker_sw_dump(unit);
        }
    }
#endif /* INCLUDE_FLOWTRACKER */
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit,soc_feature_flex_stat_compaction_support)) {
        if (dump_all || !sal_strcasecmp(c, "ctrpoolinfo")) {
            _bcm_stat_flexctr_pool_info_sw_dump(unit);
        }
    }
#endif

#if defined(BCM_COLLECTOR_SUPPORT)
    if (soc_feature(unit, soc_feature_collector)) {
        if (dump_all || !sal_strcasecmp(c, "collector")) {
            _bcm_esw_collector_sw_dump(unit);
        }
    }
#endif /* BCM_COLLECTOR_SUPPORT */

#if defined(INCLUDE_TELEMETRY)
    if (soc_feature(unit, soc_feature_telemetry)) {
        if (dump_all || !sal_strcasecmp(c, "telemetry")) {
            _bcm_esw_telemetry_sw_dump(unit);
        }
    }
#endif /* INCLUDE_TELEMETRY */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
#ifdef INCLUDE_L3
#if defined(BCM_FIREBOLT_SUPPORT)
    if (soc_feature(unit, soc_feature_ip_mcast)) {
        if (dump_all || !sal_strcasecmp(c, "ipmc")) {
            _bcm_ipmc_sw_dump(unit);
        }
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
    if (dump_all || !sal_strcasecmp(c, "extlpm")) { 
        _bcm_tr_ext_lpm_sw_dump(unit); 
    } 
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_ENDURO_SUPPORT)
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_ENDURO_SUPPORT */
#endif  /* INCLUDE_L3 */ 
    if (dump_all || !sal_strcasecmp(c, "multicast")) { 
        _bcm_multicast_sw_dump(unit); 
    } 
    
#if defined(BCM_TRX_SUPPORT)
    if (dump_all || !sal_strcasecmp(c, "common")) { 
        _bcm_common_sw_dump(unit); 
    } 
#ifdef INCLUDE_L3 
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        if (dump_all || !sal_strcasecmp(c, "subport")) {
            _bcm_kt2_subport_sw_dump(unit);
        }
    }
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_subport_enhanced)) {
        if (dump_all || !sal_strcasecmp(c, "subport")) { 
            _bcm_tr2_subport_sw_dump(unit); 
        }     
    } else
#endif /* BCM_TRIUMPH2_SUPPORT */
    {
        if (soc_feature(unit, soc_feature_subport)) {
            if (dump_all || !sal_strcasecmp(c, "subport")) {
                _bcm_tr_subport_sw_dump(unit);
            }
        }
    }
#endif  /* INCLUDE_L3 */ 
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_IPFIX_SUPPORT
    /*
     * Routine to dump ipfix s/w data structure info.
     */
    if (dump_all || !sal_strcasecmp(c, "ipfix")) {
        _bcm_ipfix_sw_dump(unit);
    }
#endif /* !BCM_IPFIX_SUPPORT */
#if defined(INCLUDE_BFD)
    if (soc_feature(unit, soc_feature_bfd)) {
        /*
         * Routine to dump bfd end point  info.
         */
        if (dump_all || !sal_strcasecmp(c, "bfd")) {
            _bcm_bfd_sw_dump(unit);
        }
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_fp_based_oam))
    {
        /*
         * Routine to dump oam s/w data structure info.
         */
        if (dump_all || !sal_strcasecmp(c, "oam")) {
            _bcm_oam_sw_dump(unit);
        }
    }
#endif
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) \
    || defined(BCM_VALKYRIE2_SUPPORT)
    if (soc_feature(unit, soc_feature_oam)) {
        /*
         * Routine to dump oam s/w data structure info.
         */
        if (dump_all || !sal_strcasecmp(c, "oam")) {
            _bcm_oam_sw_dump(unit);
        }
    }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_REGEX)
    if (soc_feature(unit, soc_feature_regex)) {
        if (dump_all || !sal_strcasecmp(c, "regex")) {
            _bcm_regex_sw_dump(unit);
        }
    }
#endif /* !BCM_IPFIX_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_l2gre)) {
        if (dump_all || !sal_strcasecmp(c, "l2gre")) { 
            _bcm_l2gre_sw_dump(unit); 
        } 
    }
#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_flex_flow)) {
        if (dump_all || !sal_strcasecmp(c, "flow")) {
            bcmi_esw_flow_sw_dump(unit);
        }
    }
#endif /*BCM_TRIDENT3_SUPPORT && INCLUDE_L3 */
#endif  /*  BCM_WARM_BOOT_SUPPORT_SW_DUMP */

    if (dump_all || !sal_strcasecmp(c, "udf")) {
       int idx = -1, copyno;
       soc_mem_t mem = INVALIDm;   
       char *mem_name = NULL;
       char *idx_str = NULL;

       if ((mem_name = ARG_GET(a)) != NULL) {
           if (parse_memory_name(unit, &mem, mem_name, &copyno, 0) < 0) {
              cli_out("ERROR: unknown table \"%s\"\n", mem_name);
              goto done;
           }

           if (!SOC_MEM_IS_VALID(unit, mem)) {
               cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
               goto done;
           }

           if ((idx_str = ARG_GET(a)) != NULL) { 
              idx = parse_memory_index(unit, mem, idx_str);
              if (idx == -1) {
                 goto done;
              }
           } else {  
              cli_out("ERROR: Memory index not mentioned\n");
              goto done;
           }
       
           _bcm_esw_udf_sw_mem_dump(unit, mem, idx);
           goto done;
       }

       _bcm_esw_udf_sw_dump(unit);
    }

done:
    return CMD_OK; 
} 
#endif  /*  BCM_SW_STATE_DUMP_DISABLE */ 

STATIC INLINE int
bypass_dump_table(int unit, soc_mem_t mem, uint32 copyno, int flags) 
{
    if (!(flags & DUMP_TABLE_EXT)) {
        /*
         * Bypass dumping MMU memories.
         */
        if (SOC_BLOCK_TYPE(unit, copyno) == SOC_BLK_MMU) {
            return TRUE;
        }
    }

    if (flags & DUMP_TABLE_SIMPLIFIED) {
        /*
         * Bypass dumping these memories while in simplified mode.
         */
        switch (mem) {
        case SER_MEMORYm:
        case CLPORT_WC_UCMEM_DATAm:
        case XLPORT_WC_UCMEM_DATAm:
            return TRUE;
        default:
            break;
        }
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (mem == CDPORT_TSC_UCMEM_DATAm) {
            return TRUE;
        }
    }
#endif

    return FALSE;
}

int
mem_dump_iter_callback(int unit, soc_mem_t mem, void *data)
{
    uint32     index_min, count, copyno;
    int        rv = SOC_E_NONE;
    int        flags = PTR_TO_INT(data);

    if (!SOC_MEM_IS_VALID(unit, mem) ||
        (soc_mem_flags(unit, mem) & SOC_MEM_FLAG_DEBUG)) {
        return rv;
    }

    index_min = soc_mem_index_min(unit, mem);
    count = soc_mem_index_count(unit, mem);

    SOC_MEM_BLOCK_ITER(unit, mem, copyno) {
        if (bypass_dump_table(unit, mem, copyno, flags)) {
            continue;
        }

        if ((do_dump_table(unit, mem, copyno, index_min, 
                           count, flags, NULL, 1)) != CMD_OK) {
            rv = SOC_E_INTERNAL;
            break;
        }
    }

    return rv;
}

static cmd_result_t
do_dump_memories(int unit, args_t *a)
{
    char	*an_arg;
    int          flags = DUMP_TABLE_ALL, rv = CMD_OK;

    an_arg = ARG_GET(a);

    while (an_arg) {
        if (sal_strcasecmp(an_arg, "diff") == 0) {
            flags = DUMP_TABLE_CHANGED;
        } else if (sal_strcasecmp(an_arg, "ext") == 0) {
            flags = DUMP_TABLE_EXT;
        } else if (sal_strcasecmp(an_arg, "simpl") == 0) {
            flags |= DUMP_TABLE_SIMPLIFIED;
        } else {
            cli_out("ERROR: unrecognized argument to DUMP SOC: %s\n",
                    an_arg);
            return CMD_FAIL;
        }
        an_arg = ARG_GET(a);
    }

    if ((soc_mem_iterate(unit, 
                         mem_dump_iter_callback, INT_TO_PTR(flags))) < 0) {
        rv = CMD_FAIL;
    }

    return rv;
}


cmd_result_t
cmd_esw_dump(int unit, args_t *a)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    regtype_entry_t *rt;
    soc_mem_t mem;
    char *arg1, *arg2, *arg3;
    volatile int flags = 0;
    int copyno;
    volatile int rv = CMD_FAIL;
    parse_table_t pt;
    volatile char *fname = "";
    int append = FALSE;
    volatile int console_was_on = 0, console_disabled = 0, pushed_ctrl_c = 0;
    jmp_buf	ctrl_c;
    char *temp_field_name = NULL ,*field_names = NULL;
    uint8 offset = 0;
    int enable_mem_mode_check = 1;
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    char *buf;
    char *tmp;
    soc_format_t format;
    uint32 format_val[SOC_MAX_MEM_WORDS];
    int i = 0;
#endif /* BCM_TRIDENT3_SUPPORT */

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "File", PQ_STRING, 0, &fname, 0);
    parse_table_add(&pt, "Append", PQ_BOOL, 0, &append, FALSE);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	goto done;
    }

    if (parse_arg_eq(a, &pt) < 0) {
	rv = CMD_USAGE;
	goto done;
    }

    console_was_on = bslcons_is_enabled();

    if (fname[0] != 0) {
	/*
	 * Catch control-C in case if using file output option.
	 */

#ifndef NO_CTRL_C
	if (setjmp(ctrl_c)) {
	    rv = CMD_INTR;
	    goto done;
	}
#endif

	sh_push_ctrl_c(&ctrl_c);

	pushed_ctrl_c = TRUE;



	if (bslfile_is_enabled()) {
	    cli_out("%s: Can't dump to file while logging is enabled\n",
                    ARG_CMD(a));
	    rv = CMD_FAIL;
	    goto done;
	}

	if (bslfile_open((char *)fname, append) < 0) {
	    cli_out("%s: Could not start log file\n", ARG_CMD(a));
	    rv = CMD_FAIL;
	    goto done;
	}

	bslcons_enable(FALSE);

	console_disabled = 1;
    }

    arg1 = ARG_GET(a);

    for (;;) {
         if (arg1 != NULL && !sal_strcasecmp(arg1, "raw")) {
             flags |= DUMP_TABLE_RAW;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "hex")) {
             flags |= DUMP_TABLE_HEX;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "all")) {
             flags |= DUMP_TABLE_ALL;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "chg")) {
             flags |= DUMP_TABLE_CHANGED;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe_x")) {
             flags |= DUMP_TABLE_PIPE_X;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe_y")) {
             flags |= DUMP_TABLE_PIPE_Y;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe0")) {
             flags |= DUMP_TABLE_PIPE_0;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe1")) {
             flags |= DUMP_TABLE_PIPE_1;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe2")) {
             flags |= DUMP_TABLE_PIPE_2;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe3")) {
             flags |= DUMP_TABLE_PIPE_3;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe4")) {
             flags |= DUMP_TABLE_PIPE_4;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe5")) {
             flags |= DUMP_TABLE_PIPE_5;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe6")) {
             flags |= DUMP_TABLE_PIPE_6;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "pipe7")) {
             flags |= DUMP_TABLE_PIPE_7;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "nocache")) {
             flags |= DUMP_TABLE_NO_CACHE;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "nocheck_mem_mode")) {
             enable_mem_mode_check = 0;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "idx_phy")){
             flags |= DUMP_TABLE_PHY_INDEX;
             arg1 = ARG_GET(a);
         } else if (arg1 != NULL && !sal_strcasecmp(arg1, "format")){
             flags |= DUMP_TABLE_FORMAT;
             arg1 = ARG_GET(a);
         } else {
             break;
         }
    }
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit)) {
        if (flags  & DUMP_TABLE_FORMAT) {
            sal_memset(format_val, 0,
                (sizeof(uint32) * SOC_MAX_MEM_WORDS));
            buf = ARG_GET(a);
            if (buf != NULL) {
                int base = 10;
                int done = 0;
                tmp = buf;

                tmp += (*tmp == '-');
                if (*tmp == '0') {
                    tmp++;
                    if (*tmp == 'x' || *tmp == 'X') {
                        base = 16;
                        tmp++;
                    } else if (*tmp == 'b' || *tmp == 'B') {
                        base = 2;
                        tmp++;
                    } else {
                        base = 8;
                    }
                }

                do {
                    int chs = 0;
                    uint32 n;
                    for (n = 0; ((base != 16 || chs < 8) &&
                         ((*tmp >= 'a' && *tmp < 'a' + base - 10) ||
                          (*tmp >= 'A' && *tmp < 'A' + base - 10) ||
                          (*tmp >= '0' && *tmp <= '9'))); tmp++) {
                        n = n * base + ((*tmp <= '9' ? *tmp : *tmp + 9) & 15);
                        chs ++;
                    }
                    if (base != 16 || chs < 8) {
                        done = 1;
                    }
                    format_val[i++] = n;
                } while ((*tmp != '\0') && (i < SOC_MAX_MEM_WORDS) && !done);

                if (base == 16 && i > 1) {
                    uint32 temp, t;
                    for (t = 0; t < (i / 2); t++) {
                        temp = format_val[t];
                        format_val[t] = format_val[i - t - 1];
                        format_val[i - t - 1] = temp;
                    }
                }
            }
            if (parse_format_name(unit, arg1, &format) == -1) {
                cli_out ("Invalid format.\n");
                rv = CMD_FAIL;
                goto done;
           }
           soc_format_entry_dump(unit, format, (uint32 *) &format_val);
           rv = CMD_OK;
           goto done;
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    if ((flags & DUMP_TABLE_PIPE_X) || (flags & DUMP_TABLE_PIPE_Y)) {
        if (NUM_PIPE(unit) != 2) {
            cli_out("Number of chip is %d, try to dump pipe0...3\n", NUM_PIPE(unit));
            rv = CMD_FAIL;
            goto done;
        }
    }
    if ((flags & DUMP_TABLE_PIPE_7) || (flags & DUMP_TABLE_PIPE_6) ||
        (flags & DUMP_TABLE_PIPE_5) || (flags & DUMP_TABLE_PIPE_4) ||
        (flags & DUMP_TABLE_PIPE_3) || (flags & DUMP_TABLE_PIPE_2) ||
        (flags & DUMP_TABLE_PIPE_1) || (flags & DUMP_TABLE_PIPE_0)){
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3X(unit)) {
            if (flags & (DUMP_TABLE_PIPE_2|DUMP_TABLE_PIPE_3|
                         DUMP_TABLE_PIPE_4|DUMP_TABLE_PIPE_5|
                         DUMP_TABLE_PIPE_6|DUMP_TABLE_PIPE_7)) {
                cli_out("Number of chip is %d, try to dump pipe_0/pipe_1\n", NUM_PIPE(unit));
                rv = CMD_FAIL;
                goto done;
            }
        } else
#endif
        if ((NUM_PIPE(unit) != 4) && (NUM_PIPE(unit) != 8)) {
            cli_out("Number of chip is %d, try to dump pipe_x/pipe_y\n", NUM_PIPE(unit));
            rv = CMD_FAIL;
            goto done;
        }
    }

    if (arg1 == NULL) {
	rv = CMD_USAGE;
	goto done;
    }

    /* See if dumping internal ARL/L2 shadow copy */

    if (!sal_strcasecmp(arg1, "sarl") || !sal_strcasecmp(arg1, "sa")) {
	if (soc->arlShadow == NULL) {
	    cli_out("No software ARL shadow table\n");
	    rv = CMD_FAIL;
	    goto done;
	}

	sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);

#ifdef	BCM_XGS_SWITCH_SUPPORT
	if (soc_feature(unit, soc_feature_arl_hashed)) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit)) {
                cli_out("Entries on Internal memory:\n");
                shr_avl_traverse(soc->arlShadow, soc_tr3_l2_entry_dump, NULL);
                if (soc_feature(unit, soc_feature_esm_support)) {
                    cli_out("Entries on External memory:\n");
                    shr_avl_traverse(soc->arlShadow_ext1, soc_tr3_ext_l2_1_entry_dump, NULL);
                    shr_avl_traverse(soc->arlShadow_ext2, soc_tr3_ext_l2_2_entry_dump, NULL);
                }
            } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
	        shr_avl_traverse(soc->arlShadow, soc_l2x_entry_dump, NULL);
            }
	}
#endif	/* BCM_XGS_SWITCH_SUPPORT */

	sal_mutex_give(soc->arlShadowMutex);

	rv = CMD_OK;
	goto done;
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Dump L2 learn shadow table */
    if (!sal_strcasecmp(arg1, "l2lrn")) {
        if (!SOC_IS_TOMAHAWK3(unit)) {
            cli_out("Command supported on Tomahawk3 only\n");
            rv = CMD_FAIL;
            goto done;
        }

        if (soc->l2x_lrn_shadow == NULL) {
            cli_out("No learn shadow table\n");
            rv = CMD_FAIL;
            goto done;
        }

	sal_mutex_take(soc->l2x_lrn_shadow_mutex, sal_mutex_FOREVER);

#ifdef	BCM_XGS_SWITCH_SUPPORT
	if (soc_feature(unit, soc_feature_arl_hashed)) {
	    shr_avl_traverse(soc->l2x_lrn_shadow, soc_th3_lrn_shadow_show, NULL);
	}
#endif	/* BCM_XGS_SWITCH_SUPPORT */

	sal_mutex_give(soc->l2x_lrn_shadow_mutex);

	rv = CMD_OK;
	goto done;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    /* See if dumping a DV */
    if (!sal_strcasecmp(arg1, "dv")) {
        arg2 = ARG_GET(a);
	if (!arg2 || !isint(arg2)) {
	    rv = CMD_USAGE;
	    goto done;
	}
	soc_dma_dump_dv(unit, " ", (void *)parse_address(arg2));
	rv = CMD_OK;
	goto done;
    }

    /* See if dumping a packet */
    if (!sal_strcasecmp(arg1, "pkt")) {
        arg2 = ARG_GET(a);
	if (!arg2 || !isint(arg2)) {
	    rv = CMD_USAGE;
	    goto done;
	}
#ifdef	BROADCOM_DEBUG
        arg3 = ARG_GET(a);
        arg3 = arg3 ? arg3 : "0000";
	bcm_pkt_dump(unit,
		     INT_TO_PTR(parse_address(arg2)),
		     parse_integer(arg3));
	rv = CMD_OK;
#else
	cli_out("cannot dump pkt in non-BROADCOM_DEBUG compilation\n");
	rv = CMD_FAIL;
#endif	/* BROADCOM_DEBUG */
	goto done;
    }

    /* See if dumping a memory table */

    if (parse_memory_name(unit, &mem, arg1, &copyno, 0) >= 0) {
	int index, count;

        arg2 = ARG_GET(a);
        arg3 = ARG_GET(a);
	if (!SOC_MEM_IS_VALID(unit, mem)) {
	    cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
	    goto done;
	}

#if defined(BCM_HAWKEYE_SUPPORT)
    if (SOC_IS_HAWKEYE(unit) && (soc_mem_index_max(unit, mem) <= 0)) {
	    cli_out("Error: Memory %s not valid for chip %s.\n",
                SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
	    goto done;
    }
#endif /* BCM_HAWKEYE_SUPPORT */

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && arg1 != NULL) {
#ifdef NO_SAL_APPL
        if (strstr((char *)arg1,(char *)"flowset")) {
#else
        if (sal_strcasestr((char *)arg1,(char *)"flowset")) {
#endif
            if (soc_mem_is_shared_mem(unit, mem)) {
#ifdef NO_SAL_APPL
                if (strstr((char *)arg1,(char *)"rh_")) {
#else
                if (sal_strcasestr((char *)arg1,(char *)"rh_")) {
#endif
                    cli_out("Error:  Memory %s cannot be dumped for chip %s with DLB mode selected.\n",
                             SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
                    goto done;
                }
#ifdef NO_SAL_APPL
                if (strstr((char *)arg1,(char *)"dlb_")) {
#else
                if (sal_strcasestr((char *)arg1,(char *)"dlb_")) {
#endif
                    cli_out("Error:  Memory %s cannot be dumped for chip %s in RH mode selected.\n",
                            SOC_MEM_UFNAME(unit, mem), SOC_UNIT_NAME(unit));
                    goto done;
                }
            }
        }
    }
#endif /* BCM_TRIDENT3_SUPPORT */

	if (copyno == COPYNO_ALL) {
	    copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        }
	if ((arg2) && (isint(arg2))) {
             index = parse_memory_index(unit, mem, arg2);

             if (!soc_mem_index_valid(unit, mem, index)) {
                if (!(SOC_DEFIP_HOLE_RANGE_CHECK(unit, mem, index))) {
                    if (!(flags & DUMP_TABLE_PHY_INDEX)) {
                        goto done;
                    }
                }
             }
	     if ((arg3) && (isint(arg3))) {
                 count = parse_integer(arg3);
             } else {
                 count = 1;
             }
	} else {
	    index = soc_mem_index_min(unit, mem);
	    if (soc_mem_is_sorted(unit, mem) &&
		!(flags & DUMP_TABLE_ALL)) {
		count = soc_mem_entries(unit, mem, copyno) +
                    SOC_DEFIP_HOLE_OFFSET(unit, mem);
	    } else {
		count = soc_mem_index_max(unit, mem) - index + 1 +
                    SOC_DEFIP_HOLE_OFFSET(unit, mem);
	    }

        if (flags & DUMP_TABLE_PHY_INDEX) {
            if (soc_feature(unit, soc_feature_l3_defip_map) &&
                (mem == L3_DEFIPm ||
                 mem == L3_DEFIP_ONLYm ||
                 mem == L3_DEFIP_DATA_ONLYm)) {
                 index = 0;
                 count = soc_mem_index_count(unit, L3_DEFIPm);
                 count += soc_mem_index_count(unit, L3_DEFIP_PAIR_128m) * 2;
            }
            if (soc_feature(unit, soc_feature_l3_defip_map) &&
                (mem == L3_DEFIP_PAIR_128m ||
                 mem == L3_DEFIP_PAIR_128_ONLYm ||
                 mem == L3_DEFIP_PAIR_128_DATA_ONLYm)) {
                 index = 0;
                 count = soc_mem_index_count(unit, L3_DEFIPm) >> 1;
                 count += soc_mem_index_count(unit, L3_DEFIP_PAIR_128m);
            }
        }
	}
        if ((arg2) && (!isint(arg2))) {
             ARG_PREV(a);
        }
        if ((arg3) && (!isint(arg3))) {
             ARG_PREV(a);
        }
        field_names = NULL;
        while ((temp_field_name = ARG_GET(a)) != NULL) {
                if (field_names == NULL) {
                    if ((field_names = sal_alloc(ARGS_BUFFER, "reg_set")) == 
                         NULL) {
                         return (CMD_FAIL);
                    }
                    sal_strncpy(field_names, ",", 2);
                }
                for (offset=0 ; offset < sal_strlen(temp_field_name);offset++) {
                     temp_field_name[offset] = toupper((int)
                                                       temp_field_name[offset]);
                } 
                sal_strncat(field_names, temp_field_name, 
                            sal_strlen(temp_field_name));
                sal_strncat(field_names, ",", 1);
        }
        rv = do_dump_table(unit, mem, copyno, index, count, flags, field_names,
                           enable_mem_mode_check);
        if (field_names != NULL) {
            sal_free(field_names);
        }
	goto done;
    }

    if (!sal_strcasecmp(arg1, "socmem")) {
        rv = do_dump_memories(unit, a);
        goto done;
    }

    /*
     * See if dumping a register type
     */
    if ((rt = regtype_lookup_name(arg1)) != NULL) {
        rv = do_dump_registers(unit, rt, a);
        goto done;
    }

#ifndef BCM_SW_STATE_DUMP_DISABLE
    /* 
     * SW data structures dump 
     */ 
    if (!sal_strcasecmp(arg1, "sw")) { 
        rv = do_dump_sw(unit, a); 
        goto done; 
    } 
#endif 

    cli_out("Unknown option or memory to dump "
            "(use 'help dump' for more info)\n");

    rv = CMD_FAIL;

 done:

    if (fname[0] != 0) {
	bslfile_close();
    }

    if (console_disabled && console_was_on) {
	bslcons_enable(TRUE);
    }

    if (pushed_ctrl_c) {
	sh_pop_ctrl_c();
    }

    parse_arg_eq_done(&pt);
    return rv;
}
