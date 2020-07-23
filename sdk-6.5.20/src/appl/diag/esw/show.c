/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Show CLI commands
 *
 * Also contains:
 *	Check CLI command
 *	Clear CLI command
 */

#include <appl/diag/system.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/stat.h>
#include <bcm/error.h>
#include <bcm/init.h>
#include <bcm_int/esw_dispatch.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/mcm/driver.h>

#include <sal/appl/pci.h>

#if defined(VXWORKS)
#include <netShow.h>
#include <muxLib.h>
#endif /* VXWORKS */

#if defined(VXWORKS_NETWORK_STACK_6_5)
#include <net/utils/netstat.h>
#endif /* VXWORKS_NETWORK_STACK_6_5 */

#if defined(BCM_TRIDENT_SUPPORT)
#include <soc/trident.h>
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)
#include <soc/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_KATANA_SUPPORT)
#include <soc/katana.h>
#endif /* BCM_KATANA_SUPPORT */

#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif /* BCM_SABER2_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#include <bcm_int/esw/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <soc/td2_td2p.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_HELIX5_SUPPORT)
#include <soc/helix5.h>
#endif /* BCM_HELIX5_SUPPORT */

#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */

#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif /* BCM_APACHE_SUPPORT */

#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */

#if defined(BCM_HURRICANE2_SUPPORT)
#include <soc/hurricane2.h>
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#endif

#if defined(BCM_KATANA2_SUPPORT)
cmd_result_t
do_show_linkphy_config(int unit);
#endif /* BCM_KATANA2_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif

/*
 * Utility routine for show command
 */

cmd_result_t
do_show_memory(int unit, soc_mem_t mem, int copyno)
{
    int		entries;
    char	*ufname;
    int		blk;

    ufname = SOC_MEM_UFNAME(unit, mem);
    if (!soc_mem_is_sorted(unit, mem) &&
        !soc_mem_is_hashed(unit, mem) &&
        mem != VLAN_TABm) {
            cli_out("ERROR: %s table is not sorted or hashed\n",
                    ufname);
            return CMD_FAIL;
    }

    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != blk) {
            continue;
        }
        if ((entries = soc_mem_entries(unit, mem, blk)) < 0) {
            cli_out("Error reading number of entries: %s\n",
                    soc_errmsg(entries));

            return CMD_FAIL;
        }

        cli_out("%s.%s contains %d entries\n",
                ufname, SOC_BLOCK_NAME(unit, blk), entries);
    }

    return CMD_OK;
}

/*
 * Show something to a user.
 */

/*
 * *** ORDER ***
 *
 * The order of these tables must match the switch statements below.
 */
static parse_key_t show_arg[] = {
    "Pci",				/* 0 */
    "Counters",				/* 1 */
    "Errors",				/* 2 */
    "Interrupts",			/* 3 */
    "Chips",				/* 4 */
    "Statistics",			/* 5 */
    "MIB",				/* 6 */
    "TeMPmonitor",                      /* 7 */
    "MaterialProcess",                  /* 8 */
    "RingOSC",                          /* 9 */
    "PortMAP",                          /* 10 */
    "Rate",                             /* 11 */
    "Voltage",                          /* 12 */
    "LinkPhyCounter",                   /* 13 */
    "SubPortMap",                       /* 14 */
    "ICID",                             /* 15 */
    "Threads",                          /* 16 */
};

static int show_arg_cnt = PARSE_ENTRIES(show_arg);

static parse_key_t show_ctr_arg[] = {
    "Changed",				/* 0 */
    "Same",				/* 1 */
    "Zero",				/* 2 */
    "NonZero",				/* 3 */
    "Hex",				/* 4 */
    "Raw",				/* 5 */
    "All",				/* 6 */
    "ErDisc",				/* 7 */
    "MmuStatus",       			/* 8 */
};

static int show_ctr_arg_cnt = PARSE_ENTRIES(show_ctr_arg);

#define    GETTIME    sal_time_usecs()
#define    DCLTIME    sal_usecs_t
#define    FMTTIME    "%u pkt/sec"
#define    RATE(x)    ((x) * 1000000)
/*
 * Data structure for saving the previous value of counters so we can
 * tell which counters that have changed since last shown.
 */
typedef struct counter_time_val_s {
    DCLTIME   prev_time;
    uint64       val;
}counter_time_val_t;

STATIC counter_time_val_t *counter_time_val[SOC_MAX_NUM_DEVICES];

static int
do_show_threads(void *data, int tid, char *name)
{
    cli_out("%-8d %s\n", tid, name);
    return CMD_OK;
}

cmd_result_t
cmd_esw_show(int unit, args_t *a)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    soc_pbmp_t		pbmp;
    bcm_pbmp_t		bcm_pbmp;
    bcm_port_config_t   pcfg;
    const parse_key_t	*cmd;
    char		*c;
    soc_mem_t		mem;
    int			copyno;
    int			flags;
    soc_regaddrlist_t	alist;
    soc_reg_t		ctr_reg;
    soc_port_t		port, dport;
    int                 commachr = soc_property_get(unit, spn_DIAG_COMMA, ',');
    char                buf_val[32];
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    int  rv = 0;
#endif
#ifdef BCM_TRIDENT2PLUS_SUPPORT
     int i, count,  queue_mode, array_count = 0;
     int *ar_idx_array = NULL;
     soc_port_t *port_array = NULL;
#endif

    if (!(c = ARG_GET(a))) {		/* Nothing to do */
        return(CMD_USAGE);		/* Print usage line */
    }

#if defined(BCM_KATANA2_SUPPORT)
    if (!sal_strcasecmp(c, "linkphy")) {
        do_show_linkphy_config(unit);
        return CMD_OK;
    }
#endif

#if defined (VXWORKS) 
#if defined (VXWORKS_NETWORK_STACK_6_5)
    if (!sal_strcasecmp(c, "ip")) {
        netstat("-p ip -f inet");
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "icmp")) {
        netstat("-p icmp -f inet");
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "arp")) {
        arpShow();
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "udp")) {
        netstat("-p udp -f inet");
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "tcp")) {
        netstat("-p tcp -f inet");
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "mux")) {
        muxShow(NULL, 0);
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "routes")) {
        netstat("-a -r");
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "hosts")){
        hostShow();
        return CMD_OK;
    }
#else /* !VXWORKS_NETWORK_STACK_6_5 */
    if (!sal_strcasecmp(c, "ip")) {
        ipstatShow(FALSE);
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "icmp")) {
        icmpstatShow();
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "arp")) {
        arpShow();
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "udp")) {
        udpstatShow();
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "tcp")) {
        tcpstatShow();
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "mux")) {
#if defined(IDTRP334)
        cli_out("muxShow not available on this BSP\n");
#else
        muxShow(NULL, 0);
#endif
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "routes")) {
        routeShow();
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "hosts")){
        hostShow();
        return CMD_OK;
    }
#endif /* VXWORKS_NETWORK_STACK_6_5 */
#endif

#if defined(BROADCOM_DEBUG)
    if (!sal_strcasecmp(c, "feature") || !sal_strcasecmp(c, "features")) {
        int		all;
        soc_feature_t	f;

        c = ARG_GET(a);
        if (c == NULL) {
            all = 0;
        } else {
            all = 1;
        }
        cli_out("Unit %d features:\n", unit);

        for (f = 0; f < soc_feature_count; f++) {
            if (soc_feature(unit, f)) {
                cli_out("\t%s\n", soc_feature_name[f]);
            } else if (all) {
                cli_out("\t[%s]\n", soc_feature_name[f]);
            }
        }
        return CMD_OK;
    }

    if (!sal_strcasecmp(c, "soc_is") || !sal_strcasecmp(c, "soc_is_chip")) {
#define SOC_IS_SHOW(unit, a)  if (a(unit)) { cli_out("\t%s\n", #a); }
        SOC_IS_SHOW(unit, SOC_IS_DRACO1)
        SOC_IS_SHOW(unit, SOC_IS_DRACO15)
        SOC_IS_SHOW(unit, SOC_IS_DRACO)
        SOC_IS_SHOW(unit, SOC_IS_HERCULES1)
        SOC_IS_SHOW(unit, SOC_IS_LYNX)
        SOC_IS_SHOW(unit, SOC_IS_TUCANA)
        SOC_IS_SHOW(unit, SOC_IS_XGS12_SWITCH)
        SOC_IS_SHOW(unit, SOC_IS_HERCULES15)
        SOC_IS_SHOW(unit, SOC_IS_HERCULES15)
        SOC_IS_SHOW(unit, SOC_IS_HERCULES)
        SOC_IS_SHOW(unit, SOC_IS_FIREBOLT)
        SOC_IS_SHOW(unit, SOC_IS_FB)
        SOC_IS_SHOW(unit, SOC_IS_FB_FX_HX)
        SOC_IS_SHOW(unit, SOC_IS_HB_GW)
        SOC_IS_SHOW(unit, SOC_IS_HBX)
        SOC_IS_SHOW(unit, SOC_IS_FBX)
        SOC_IS_SHOW(unit, SOC_IS_HURRICANE2)
        SOC_IS_SHOW(unit, SOC_IS_GREYHOUND)
        SOC_IS_SHOW(unit, SOC_IS_HELIX)
        SOC_IS_SHOW(unit, SOC_IS_HELIX1)
        SOC_IS_SHOW(unit, SOC_IS_HELIX15)
        SOC_IS_SHOW(unit, SOC_IS_FELIX)
        SOC_IS_SHOW(unit, SOC_IS_FELIX1)
        SOC_IS_SHOW(unit, SOC_IS_FELIX15)
        SOC_IS_SHOW(unit, SOC_IS_RAPTOR)
        SOC_IS_SHOW(unit, SOC_IS_RAVEN)
        SOC_IS_SHOW(unit, SOC_IS_HAWKEYE)
        SOC_IS_SHOW(unit, SOC_IS_FX_HX)
        SOC_IS_SHOW(unit, SOC_IS_GOLDWING)
        SOC_IS_SHOW(unit, SOC_IS_HUMV)
        SOC_IS_SHOW(unit, SOC_IS_BRADLEY)
        SOC_IS_SHOW(unit, SOC_IS_FIREBOLT2)
        SOC_IS_SHOW(unit, SOC_IS_TRIUMPH)
        SOC_IS_SHOW(unit, SOC_IS_ENDURO)
        SOC_IS_SHOW(unit, SOC_IS_HURRICANE)
        SOC_IS_SHOW(unit, SOC_IS_HURRICANEX)
        SOC_IS_SHOW(unit, SOC_IS_TR_VL)
        SOC_IS_SHOW(unit, SOC_IS_VALKYRIE)
        SOC_IS_SHOW(unit, SOC_IS_SCORPION)
        SOC_IS_SHOW(unit, SOC_IS_SC_CQ)
        SOC_IS_SHOW(unit, SOC_IS_CONQUEROR)
        SOC_IS_SHOW(unit, SOC_IS_TRIUMPH2)
        SOC_IS_SHOW(unit, SOC_IS_APOLLO)
        SOC_IS_SHOW(unit, SOC_IS_VALKYRIE2)
        SOC_IS_SHOW(unit, SOC_IS_TRIDENT)
        SOC_IS_SHOW(unit, SOC_IS_TITAN)
        SOC_IS_SHOW(unit, SOC_IS_TD_TT)
        SOC_IS_SHOW(unit, SOC_IS_SHADOW)
        SOC_IS_SHOW(unit, SOC_IS_TRIUMPH3)
        SOC_IS_SHOW(unit, SOC_IS_HELIX4)
        SOC_IS_SHOW(unit, SOC_IS_KATANA)
        SOC_IS_SHOW(unit, SOC_IS_KATANAX)
        SOC_IS_SHOW(unit, SOC_IS_KATANA2)
        SOC_IS_SHOW(unit, SOC_IS_TRIDENT2)
        SOC_IS_SHOW(unit, SOC_IS_TITAN2)
        SOC_IS_SHOW(unit, SOC_IS_TD2_TT2)
        SOC_IS_SHOW(unit, SOC_IS_TOMAHAWK)
        SOC_IS_SHOW(unit, SOC_IS_TOMAHAWK2)
        SOC_IS_SHOW(unit, SOC_IS_TOMAHAWK3)
        SOC_IS_SHOW(unit, SOC_IS_TRIDENT3X)
        SOC_IS_SHOW(unit, SOC_IS_TRX)
        SOC_IS_SHOW(unit, SOC_IS_XGS)
        SOC_IS_SHOW(unit, SOC_IS_XGS_FABRIC)
        SOC_IS_SHOW(unit, SOC_IS_XGS_SWITCH)
        SOC_IS_SHOW(unit, SOC_IS_XGS12_FABRIC)
        SOC_IS_SHOW(unit, SOC_IS_XGS3_SWITCH)
        SOC_IS_SHOW(unit, SOC_IS_XGS3_FABRIC)
        SOC_IS_SHOW(unit, SOC_IS_ARAD)
        SOC_IS_SHOW(unit, SOC_IS_ARADPLUS)
        SOC_IS_SHOW(unit, SOC_IS_ARDON)
        SOC_IS_SHOW(unit, SOC_IS_ARAD_A0)
        SOC_IS_SHOW(unit, SOC_IS_ARAD_B0)
        SOC_IS_SHOW(unit, SOC_IS_ARAD_B1)
        SOC_IS_SHOW(unit, SOC_IS_ARADPLUS_A0)
        SOC_IS_SHOW(unit, SOC_IS_ARAD_B0_AND_ABOVE)
        SOC_IS_SHOW(unit, SOC_IS_ARAD_B1_AND_BELOW)
        SOC_IS_SHOW(unit, SOC_IS_ARADPLUS_AND_BELOW)
        SOC_IS_SHOW(unit, SOC_IS_ACP)
        SOC_IS_SHOW(unit, SOC_IS_QMX)
        SOC_IS_SHOW(unit, SOC_IS_JERICHO)
        SOC_IS_SHOW(unit, SOC_IS_QMX_A0)
        SOC_IS_SHOW(unit, SOC_IS_JERICHO_A0)
        SOC_IS_SHOW(unit, SOC_IS_FE1600_A0)
        SOC_IS_SHOW(unit, SOC_IS_FE1600_B0)
        SOC_IS_SHOW(unit, SOC_IS_FE1600_B0_AND_ABOVE)
        SOC_IS_SHOW(unit, SOC_IS_FE1600)
        SOC_IS_SHOW(unit, SOC_IS_FE3200)
        SOC_IS_SHOW(unit, SOC_IS_FE3200_A0)
        SOC_IS_SHOW(unit, SOC_IS_DFE_TYPE)
        SOC_IS_SHOW(unit, SOC_IS_FE1600_REDUCED)
        SOC_IS_SHOW(unit, SOC_IS_BCM88754_A0)
        SOC_IS_SHOW(unit, SOC_IS_RELOADING)

        return CMD_OK;
    }

    if (!sal_strcasecmp(c, "param") || !sal_strcasecmp(c, "params")) {
        if (!(c = ARG_GET(a))) {
            /* Current unit */
            soc_chip_dump(unit, SOC_DRIVER(unit));
        } else {
            int chip, pcidev;
            chip = sal_ctoi(c, 0);
            if ((chip >= 0x5600 && chip <= 0x56ff) ||
                (chip >= 0xb000 && chip <= 0xbfff)) {
                /* search for driver for pci device id */
                pcidev = chip;
                for (chip = 0; chip < SOC_NUM_SUPPORTED_CHIPS; chip++) {
                    if (pcidev == soc_base_driver_table[chip]->pci_device) {
                        soc_chip_dump(-1, soc_base_driver_table[chip]);
                        return CMD_OK;
                    }
                }
                cli_out("Chip device %x not found\n", pcidev);
                return CMD_FAIL;
            }
            /* specific chip requested */
            if (chip >= SOC_NUM_SUPPORTED_CHIPS) {
                cli_out("Bad chip parameter:  %d.  Max is %d\n",
                        chip, SOC_NUM_SUPPORTED_CHIPS);
            } else if (!SOC_DRIVER_ACTIVE(chip)) {
                cli_out("Chip %d is not supported.\n", chip);
            } else {
                soc_chip_dump(-1, soc_base_driver_table[chip]);
            }
        }
        return CMD_OK;
    }
    if (!sal_strcasecmp(c, "unit") || !sal_strcasecmp(c, "units")) {
        int		u;
        soc_control_t	*usoc;

        c = ARG_GET(a);
        if (c != NULL) {	/* specific unit */
            u = sal_ctoi(c, 0);
            if (!SOC_UNIT_VALID(u)) {
                cli_out("Unit %d is not valid\n", u);
                return CMD_FAIL;
            }
            usoc = SOC_CONTROL(u);
            if (!(usoc->soc_flags & SOC_F_ATTACHED)) {
                cli_out("Unit %d (detached)\n", u);
                return CMD_OK;
            }
            cli_out("Unit %d chip %s%s\n",
                    u,
                    soc_dev_name(u),
                    u == unit ? " (current)" : "");
            soc_chip_dump(u, SOC_DRIVER(u));
        } else {		/* all units */
            for (u = 0; u < soc_ndev; u++) {
                if (!SOC_UNIT_VALID(SOC_NDEV_IDX2DEV(u))) {
                    continue;
                }
                usoc = SOC_CONTROL(SOC_NDEV_IDX2DEV(u));
                cli_out("Unit %d chip %s%s\n",
                        SOC_NDEV_IDX2DEV(u),
                        soc_dev_name(SOC_NDEV_IDX2DEV(u)),
                        SOC_NDEV_IDX2DEV(u) == unit ? " (current)" : "");
            }
        }
        return CMD_OK;
    }
#endif

    /* Can only check memory this way if unit is attached */
    if (soc_attached(unit)) {
        if (!sal_strcasecmp(c, "sa") || !sal_strcasecmp(c, "sarl")) {
            cli_out("Software ARL table: \n");
#ifdef BCM_TRIUMPH3_SUPPORT
            if ((SOC_IS_TRIUMPH3(unit)) &&
                (soc_feature(unit, soc_feature_esm_support))) {
                int int_count = 0;
                int ext1_count = 0;
                int ext2_count = 0;
                if ((SOC_CONTROL(unit)->arlShadow == NULL) &&
                    (SOC_CONTROL(unit)->arlShadow_ext1 == NULL) &&
                    (SOC_CONTROL(unit)->arlShadow_ext2 == NULL)) {
                    cli_out("none\n");
                } else {
                    if (SOC_CONTROL(unit)->arlShadow != NULL) {
                        int_count = shr_avl_count(SOC_CONTROL(unit)->arlShadow);
                    }
                    if (SOC_CONTROL(unit)->arlShadow_ext1 != NULL) {
                        ext1_count = shr_avl_count(
                                     SOC_CONTROL(unit)->arlShadow_ext1);
                    }
                    if (SOC_CONTROL(unit)->arlShadow_ext2 != NULL) {
                        ext2_count = shr_avl_count(
                                     SOC_CONTROL(unit)->arlShadow_ext2);
                    }
                    cli_out("Entries on internal mem: %d\n"
                            "Entries on external mem: %d\n "
                            "Total %d entries \n",
                            int_count, ext1_count + ext2_count, 
                            int_count + ext1_count + ext2_count);
               }
            } else
#endif
            {
                if (SOC_CONTROL(unit)->arlShadow == NULL) {
                    cli_out("none\n");
                } else {
                    cli_out("%d entries\n",
                            shr_avl_count(SOC_CONTROL(unit)->arlShadow));
                }
            }
            return CMD_OK;
        }

        if (parse_memory_name(unit, &mem, c, &copyno, 0) >= 0) {
            return do_show_memory(unit, mem, copyno);
        }
    } else { /* show based on chip */
    }

    cmd = parse_lookup(c, show_arg, sizeof(show_arg[0]), show_arg_cnt);
    if (!cmd) {
        cli_out("%s: Error: Invalid option %s\n", ARG_CMD(a), c);
        return(CMD_FAIL);
    }

    switch(cmd - show_arg) {
#if !defined(BCM_ICS) && !defined(NO_SAL_APPL)
    case 0:				/* PCI */
        /* COVERITY: Intentional, stack use of 5168 bytes */
        /* coverity[stack_use_callee_max : FALSE] */
        /* coverity[stack_use_overflow : FALSE] */
        pci_print_all();
        break;
#endif
    case 1:				/* Counters */
        flags = 0;

        while ((c = ARG_GET(a)) != NULL) {
            cmd = parse_lookup(c,
                               show_ctr_arg, sizeof (show_ctr_arg[0]),
                               show_ctr_arg_cnt);

            switch (cmd - show_ctr_arg) {
            case 0:
                flags |= SHOW_CTR_CHANGED;
                break;
            case 1:
                flags |= SHOW_CTR_SAME;
                break;
            case 2:
                flags |= SHOW_CTR_Z;
                break;
            case 3:
                flags |= SHOW_CTR_NZ;
                break;
            case 4:
                flags |= SHOW_CTR_HEX;
                break;
            case 5:
                flags |= SHOW_CTR_RAW;
                break;
            case 6:
                flags |= (SHOW_CTR_CHANGED | SHOW_CTR_SAME |
                          SHOW_CTR_Z | SHOW_CTR_NZ);
                break;
            case 7:
                flags |= SHOW_CTR_ED;
                break;
            case 8:
                flags |= SHOW_CTR_MS;
                break;
            default:
                goto break_for;
            }
        }
    break_for:

        /*
         * Supply defaults
         */

        if ((flags & (SHOW_CTR_CHANGED | SHOW_CTR_SAME)) == 0) {
            flags |= SHOW_CTR_CHANGED;
        }

        if ((flags & (SHOW_CTR_Z | SHOW_CTR_NZ)) == 0) {
            flags |= SHOW_CTR_NZ;
        }

        if (c == NULL) {
            ctr_reg = INVALIDr;		/* All registers */
            SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        } else if (parse_pbmp(unit, c, &pbmp) >= 0) {
            ctr_reg = INVALIDr;		/* All registers, selected ports */
        } else {
            int		i;

            if (soc_regaddrlist_alloc(&alist) < 0) {
                cli_out("Could not allocate address list.  Memory error.\n");
                return CMD_FAIL;
            }

            if (parse_symbolic_reference(unit, &alist, c) < 0) {
                cli_out("Syntax error parsing \"%s\"\n", c);
                soc_regaddrlist_free(&alist);
                return CMD_FAIL;
            }

            ctr_reg = alist.ainfo[0].reg;

            if (!SOC_REG_IS_COUNTER(unit, ctr_reg)) {
                cli_out("%s: Register is not a counter: %s\n", ARG_CMD(a), c);
                soc_regaddrlist_free(&alist);
                return(CMD_FAIL);
            }

            SOC_PBMP_CLEAR(pbmp);

            for (i = 0; i < alist.count; i++) {
                SOC_PBMP_PORT_ADD(pbmp, alist.ainfo[i].port);
            }
            soc_regaddrlist_free(&alist);
        }

        if (soc_feature(unit, soc_feature_cpuport_stat_dma)) {
            pbmp_t temp_pbmp;
            SOC_PBMP_CLEAR(temp_pbmp);
            SOC_PBMP_OR(temp_pbmp, PBMP_PORT_ALL(unit));
            SOC_PBMP_OR(temp_pbmp, PBMP_CMIC(unit));
            SOC_PBMP_AND(pbmp, temp_pbmp);
        } else {
            SOC_PBMP_AND(pbmp, PBMP_PORT_ALL(unit));
        }
        do_show_counters(unit, ctr_reg, pbmp, flags);

        break;
    case 2:				/* Errors */
        cli_out("%s: Errors: SDRAM(%d) CFAP(%d) first-CELL(%d) MMU-SR(%d)\n",
                ARG_CMD(a),
                soc->stat.err_sdram, soc->stat.err_cfap,
                soc->stat.err_fcell, soc->stat.err_sr);
        cli_out("%s          Timeout-Schan(%d),MII(%d)\n",
                ARG_CMD(a), soc->stat.err_sc_tmo, soc->stat.err_mii_tmo);
        break;
    case 3:				/* Interrupts */
        cli_out("%s: Interrupts: Total(%d) Schan-total(%d),error(%d) "
                "Link(%d)\n",
                ARG_CMD(a), soc->stat.intr, soc->stat.intr_sc,
                soc->stat.intr_sce, soc->stat.intr_ls);
        cli_out("%s:             GBP-full(%d) PCI-fatal(%d),parity(%d)\n",
                ARG_CMD(a), soc->stat.intr_gbp, soc->stat.intr_pci_fe,
                soc->stat.intr_pci_pe);

        if (soc_feature(unit, soc_feature_tslam_dma)) {
            cli_out("%s:             Table-DMA(%d),TSlam-DMA(%d)\n",
                    ARG_CMD(a),
                    soc->stat.intr_tdma, soc->stat.intr_tslam);
        }
        if (soc_feature(unit, soc_feature_fifo_dma)) {
            cli_out("%s:             Ch0-fifo-DMA(%d),Ch1-fifo-DMA(%d)\n",
                    ARG_CMD(a),
                    soc->stat.intr_fifo_dma[0], soc->stat.intr_fifo_dma[1]);
            cli_out("%s:             Ch2-fifo-DMA(%d),Ch3-fifo-DMA(%d)\n",
                    ARG_CMD(a),
                    soc->stat.intr_fifo_dma[2], soc->stat.intr_fifo_dma[3]);
        }
        cli_out("%s:             I2C(%d),MII(%d),Stats(%d),"
                "Desc(%d),Chain(%d)\n",
                ARG_CMD(a),
                soc->stat.intr_i2c,
                soc->stat.intr_mii,
                soc->stat.intr_stats,
                soc->stat.intr_desc,
                soc->stat.intr_chain);
        break;
    case 4:				/* Chips */
        cli_out("Known chips:\n");
        soc_cm_display_known_devices();
        break;
    case 5:				/* Statistics */
    case 6:				/* MIB */
        if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
            cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
            return CMD_FAIL;
        }

        flags = 0;
        if ((c = ARG_GET(a)) != NULL) {		/* Ports specified? */
            if (0 != parse_bcm_pbmp(unit, c, &bcm_pbmp)) {
                cli_out("%s: Invalid ports: %s\n", ARG_CMD(a), c);
                return(CMD_FAIL);
            }
            if ((c = ARG_GET(a)) != NULL && sal_strcasecmp(c, "all") == 0) {
                    flags = 1;
            }
        } else {
            BCM_PBMP_ASSIGN(bcm_pbmp, pcfg.port);
        }

        DPORT_BCM_PBMP_ITER(unit, bcm_pbmp, dport, port) {
            bcm_stat_val_t s;
            char *sname;
            char *_stat_names[] = BCM_STAT_NAME_INITIALIZER;
            int rv;
            uint64 val;

            cli_out("%s: Statistics for Unit %d port %s\n",
                    ARG_CMD(a), unit, BCM_PORT_NAME(unit, port));

            bcm_stat_sync(unit);
       
            for (s = 0; s < snmpValCount; s++) {
                 sname = _stat_names[s];
                 if (!sname) {
                     continue;
                 }

                 rv = bcm_stat_get(unit, port, s, &val);;

                 if ((rv < 0) && (rv != BCM_E_UNAVAIL)) {
                     cli_out("%8s\t%s (stat %d): %s\n",
                             "-", sname, s, bcm_errmsg(rv));
                     continue;
                 }
                 if (rv == BCM_E_UNAVAIL) {
                     continue;
                 }

                 if (flags == 0 && COMPILER_64_IS_ZERO(val)) {
                     continue;
                 }

                 if (COMPILER_64_HI(val) == 0) {
                     cli_out("%8u\t%s (stat %d)\n",
                             COMPILER_64_LO(val), sname, s);
                 } else {
                        cli_out("0x%08x%08x\t%s (stat %d)\n",
                                COMPILER_64_HI(val),
                                COMPILER_64_LO(val),
                                sname, s);
                 }
             }
        }
        break; 
    case 7: 
                          /* TeMPmonitor */
        {
            int avg_cur, max_peak,cur,peak;
            int num_temp_out;
            int index;
            bcm_switch_temperature_monitor_t temp_out[20];
          
            avg_cur = 0; 
            max_peak = 0x80000000;
            BCM_IF_ERROR_RETURN(bcm_switch_temperature_monitor_get(unit,
                20,temp_out,&num_temp_out));

            if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                num_temp_out = num_temp_out -1;
            }

            for (index = 0; index < num_temp_out; index++) {
                cur  = temp_out[index].curr;
                peak = temp_out[index].peak;
                cli_out("temperature monitor %d: current=%3d.%d, peak=%3d.%d\n",
                        index, 
                        cur/10,
                        cur >=0? cur%10:(-cur)%10, /* remove minus sign */
                        peak/10,
                        peak >=0? peak%10:(-peak)%10); /*remove minus sign */
                avg_cur += cur;
                if (max_peak < peak) {
                    max_peak = peak;
                }
            }
            avg_cur = avg_cur/index;
            cli_out("average current temperature is %3d.%d\n",
                    avg_cur/10,avg_cur >= 0? avg_cur%10: (-avg_cur)%10);
            cli_out("maximum peak temperature is %d.%d\n", 
                    max_peak/10,max_peak >= 0? max_peak%10: (-max_peak)%10);

        }
        break;
    case 8:                             /* MaterialProcess */
#ifdef BCM_MONTEREY_SUPPORT
        if (SOC_IS_MONTEREY(unit)) {
            (void)soc_monterey_show_material_process(unit);;
        } else
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            (void)soc_apache_show_material_process(unit);;
        } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit)) {
            (void)soc_trident2_show_material_process(unit);;
        } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) {
            (void)soc_trident_show_material_process(unit);
        }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_HURRICANE2_SUPPORT
	if (SOC_IS_HURRICANE2(unit)) {
	    (void)soc_hu2_show_material_process(unit);
	} 
#endif /* BCM_HURRICANE2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
	if (SOC_IS_KATANA2(unit)) {
	    (void)soc_kt2_show_material_process(unit);
	} 
#endif /* BCM_KATANA2_SUPPORT */
        break;
    case 9:                             /* RingOSC */
#ifdef BCM_MONTEREY_SUPPORT
        if (SOC_IS_MONTEREY(unit)) {
            (void)soc_monterey_show_ring_osc(unit);
        } else
#endif /*  BCM_MONTEREY_SUPPORT  */
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            (void)soc_apache_show_ring_osc(unit);
        } else
#endif /*  BCM_APACHE_SUPPORT  */
#ifdef BCM_TRIDENT2_SUPPORT
        if (SOC_IS_TD2_TT2(unit)) {
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit)) {
                (void)soc_tomahawk_show_ring_osc(unit);
            } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3X(unit)) {
                (void)soc_trident3_show_ring_osc(unit);
            } else
#endif /* BCM_TRIDENT3_SUPPORT */
            {
                (void)soc_trident2_show_ring_osc(unit);
            }
        } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            (void)soc_trident_show_ring_osc(unit);
        }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANA(unit)) {
            (void)soc_katana_show_ring_osc(unit);
        }
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            (void)soc_kt2_show_ring_osc(unit);
        }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_HURRICANE2_SUPPORT
        if (SOC_IS_HURRICANE2(unit)) {
            (void)soc_hu2_show_ring_osc(unit);;
        }
#endif /* BCM_HURRICANE2_SUPPORT */
        break;
    case 10:                            /* PortMAP */
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            if (!soc_feature(unit, soc_feature_two_ingress_pipes) &&
                !soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
                cli_out("             pm(pgw)  logical  "
                       "physical   idb lanes   speed_max   mmu   ucq/Numq    mcq/Numq\n");
            } else {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                if (SOC_IS_TRIDENT2PLUS(unit)) {
                     ar_idx_array = sal_alloc(sizeof(int) * 2960,
                         "cosq hw index when show counter");
                     if (NULL == ar_idx_array) {
                         return BCM_E_MEMORY;
                     }
                     port_array = sal_alloc(sizeof(soc_port_t) * 2960,
                         "port array of mmu queues when show counter");
                     if (NULL == port_array) {
                         sal_free(ar_idx_array);
                         return BCM_E_MEMORY;
                     }
                     cli_out("             pipe   logical  "
                       "physical    mmu   ucast_Qbase/Numq  mcast_Qbase/Numq"
                       "    endpoint_queue/Numq    service_queue/Numq\n");
                     _bcm_td2_cosq_hw_idx_get(unit, 2960, port_array,
                                              ar_idx_array, &array_count);
                } else
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    cli_out("             pipe   logical  "
                       "physical  MMU(local)  MMU(chip)   ucast_Qbase/Numq  mcast_Qbase/Numq\n");
                } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
                {
                    cli_out("             pipe   logical  "
                       "physical    idb mmu   ucast_Qbase/Numq  mcast_Qbase/Numq  half-pipe\n");
                }
            }
            PBMP_ALL_ITER(unit, port) {
                int phy_port, mmu_port, idb_port, cosq, numq, uc_cosq, uc_numq, pipe;
                int num_lanes, speed_max, pm, pgw, half_pipe;
                pipe = SOC_INFO(unit).port_pipe[port];
                pm = SOC_INFO(unit).port_serdes[port];
                pgw = SOC_INFO(unit).port_group[port];
                num_lanes = SOC_INFO(unit).port_num_lanes[port];
                speed_max = SOC_INFO(unit).port_speed_max[port];
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                idb_port = SOC_INFO(unit).port_l2i_mapping[port];
                mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
                cosq = SOC_INFO(unit).port_cosq_base[port];
                numq = SOC_INFO(unit).port_num_cosq[port];
                uc_cosq = SOC_INFO(unit).port_uc_cosq_base[port];
                uc_numq = SOC_INFO(unit).port_num_uc_cosq[port];
                half_pipe = SOC_INFO(unit).port_half_pipe[port];
#ifdef BCM_TRIDENT2_SUPPORT
#ifdef BCM_MONTEREY_SUPPORT
                if (SOC_IS_MONTEREY(unit)) {
                    cosq = soc_monterey_logical_qnum_hw_qnum(unit, port, cosq, 0);
                    uc_cosq = soc_monterey_logical_qnum_hw_qnum(unit, port,
                                                              uc_cosq, 1);
                } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit)) {
                    cosq = soc_apache_logical_qnum_hw_qnum(unit, port, cosq, 0);
                    uc_cosq = soc_apache_logical_qnum_hw_qnum(unit, port,
                                                              uc_cosq, 1);
                } else
#endif /* BCM_APACHE_SUPPORT */
                if (SOC_IS_TD2_TT2(unit) && !SOC_IS_TOMAHAWKX(unit) && !SOC_IS_TRIDENT3X(unit)) {
                    cosq = soc_td2_logical_qnum_hw_qnum(unit, port, cosq, 0);
                    uc_cosq = soc_td2_logical_qnum_hw_qnum(unit, port, uc_cosq,
                                                           1);
                }
#endif /* BCM_TRIDENT2_SUPPORT */
                if (!soc_feature(unit, soc_feature_two_ingress_pipes) &&
                    !soc_feature(unit, soc_feature_multi_pipe_mapped_ports)) {
                    cli_out("  %8s   %2d(%d)     %3d       %3d       %3d "
                            "%2d       %6d   %3d    %4d/%-4d   %4d/%-4d\n",
                            SOC_INFO(unit).port_name[port], (pm < 0 ? 0: pm),
                            (pgw < 0 ? 0 : pgw), port, phy_port, idb_port,
                            num_lanes, speed_max, mmu_port, uc_cosq, uc_numq,
                            cosq, numq);
                } else {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
                    if (SOC_IS_TRIDENT2PLUS(unit)) {
                        cli_out("  %8s    %3d   %3d       %3d       "
                                "%3d      %4d/%-4d            %4d/%-4d",
                                SOC_INFO(unit).port_name[port], pipe, port,
                                phy_port, mmu_port, uc_cosq, uc_numq, cosq, numq);
                        rv = bcm_esw_port_control_get(unit, port,
                                bcmPortControlCustomerQueuing, &queue_mode);
                        if (BCM_FAILURE(rv)) {
                            sal_free(ar_idx_array);
                            sal_free(port_array);
                            return rv;
                        }
                        if (queue_mode == 4 || queue_mode == 3) {
                            if (queue_mode == 4) {
                                cli_out("          ");
                            } else if (queue_mode == 3) {
                                cli_out("                                   ");
                            }
                            count = 0;
                            for (i = 0; i < array_count; i++) {
                                if (port_array[i] == port) {
                                    cli_out("%4d, ", ar_idx_array[i]);
                                    count++;
                                }
                            }
                            if (count) {
                                cli_out("/%-4d", count);
                            }
                        }
                        cli_out("\n");
                    } else
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
                    if (SOC_IS_TOMAHAWK3(unit)) {
                        mmu_port = SOC_TH3_MMU_CHIP_PORT(unit,port);
                        cli_out("  %8s    %3d   %3d       %3d       %3d "
                            "        %3d      %4d/%-4d            %4d/%-4d\n",
                            SOC_INFO(unit).port_name[port], pipe, port,
                            phy_port, idb_port, mmu_port, uc_cosq, uc_numq,
                            cosq, numq);
                    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */

                    {
                        cli_out("  %8s    %3d   %3d       %3d       %3d "
                                "%3d      %4d/%-4d            %4d/%-4d      %3d\n",
                                SOC_INFO(unit).port_name[port], pipe, port,
                                phy_port, idb_port, mmu_port, uc_cosq, uc_numq,
                                cosq, numq, half_pipe);
                    }
                }
            }
#ifdef BCM_TRIDENT2PLUS_SUPPORT
           if (ar_idx_array != NULL) {
               sal_free(ar_idx_array);
           }
           if (port_array != NULL) {
               sal_free(port_array);
           }
#endif
        }
        break;
    case 11:  /* Ingress Traffic rate */
        if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
            cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
            return CMD_FAIL;
        }

        flags = 0;
        if (counter_time_val[unit] == NULL) {

	    counter_time_val[unit] = sal_alloc(SOC_MAX_NUM_PORTS * 
                                           sizeof (counter_time_val_t), 
                                           "save_ctrs_time_val");

	    if (counter_time_val[unit] == NULL) {
            cli_out("Failed to allocate the counter_time_val\n");
	        return (CMD_FAIL);
	    }

	    sal_memset(counter_time_val[unit], 0, 
                   SOC_MAX_NUM_PORTS * 
                   sizeof (counter_time_val_t));
        }

        if ((c = ARG_GET(a)) != NULL) {     /* Ports specified? */
            if (!sal_strcasecmp(c, "CaptureOff")) {
                if (counter_time_val[unit] != NULL) {
                    sal_free(counter_time_val[unit]);
                    return (CMD_OK);  
                }
            } else if (0 != parse_bcm_pbmp(unit, c, &bcm_pbmp)) {
                cli_out("%s: Invalid ports: %s\n", ARG_CMD(a), c);
                return(CMD_FAIL);
            } else if (sal_strcasecmp(c, "all") == 0) {
                flags = 1;
            }
        } else {
            BCM_PBMP_ASSIGN(bcm_pbmp, pcfg.port);
        }

        DPORT_BCM_PBMP_ITER(unit, bcm_pbmp, dport, port) {
            bcm_stat_val_t s;
            char *sname;
            bcm_stat_val_t _stat[] = {
                                       snmpIfInUcastPkts,
                                       snmpIfInBroadcastPkts,
                                       snmpIfInMulticastPkts
                                     };
            char *_stat_names[] = BCM_STAT_NAME_INITIALIZER;
            int rv;
            uint64 val;
            uint64 rate;
            uint64 tmp;
            uint64 ctr_old, ctr_new, ctr_diff;
#if !defined(COMPILER_HAS_LONGLONG) || defined(__KERNEL__)
            uint32 ctr_diff32;
            uint32 rate32;
#endif
            DCLTIME curr_time, diff;
            COMPILER_64_ZERO(val);
            COMPILER_64_ZERO(rate);

            for (s = 0; s < (sizeof(_stat)/sizeof(_stat[0])); s++) {
                 sname = _stat_names[_stat[s]];
                 if (!sname) {
                     continue;
                 }


                 rv = bcm_stat_sync_get(unit, port, _stat[s], &tmp);

                 if (rv < 0) {
                     cli_out("%8s\t%s (stat %d): %s\n",
                             "-", sname, s, bcm_errmsg(rv));
                   continue;
                 }

                 COMPILER_64_ADD_64(val, tmp);
             }

             curr_time = GETTIME;
             diff = curr_time - counter_time_val[unit][port].prev_time;
             COMPILER_64_SET(ctr_new, COMPILER_64_HI(val), COMPILER_64_LO(val));
             ctr_old = counter_time_val[unit][port].val;
             COMPILER_64_DELTA(ctr_diff, ctr_old, ctr_new); 


#if !defined(COMPILER_HAS_LONGLONG) || defined(__KERNEL__)
             COMPILER_64_TO_32_LO(ctr_diff32, ctr_diff);
             rate32 = _shr_div_exp10(ctr_diff32, diff, 6);
             COMPILER_64_SET(rate, 0, rate32);
#else
             rate = ((ctr_diff * 1000000))/diff;
#endif

             counter_time_val[unit][port].prev_time = curr_time;
             counter_time_val[unit][port].val = val; 

             if (flags == 0 && COMPILER_64_IS_ZERO(rate)) {
                 continue;
             }

             sal_memset(buf_val, 0, sizeof(buf_val));
             format_uint64_decimal(buf_val, rate, commachr); 
             cli_out("Ingress traffic Rate[%s] = %s pkts/sec\n", 
                     BCM_PORT_NAME(unit, port), buf_val); 

        }
        break; 
    case 12:   /* Voltages */
#ifdef BCM_SABER2_SUPPORT
        if (SOC_IS_SABER2(unit)) {
            (void)soc_sb2_show_voltage(unit);
        } else
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            (void)soc_kt2_show_voltage(unit);
        }
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (SOC_IS_TD2P_TT2P(unit)) {
            SOC_IF_ERROR_RETURN(soc_td2p_show_voltage(unit));
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
        if (SOC_IS_MONTEREY(unit)) {
            SOC_IF_ERROR_RETURN(soc_monterey_show_voltage(unit));
        }
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            SOC_IF_ERROR_RETURN(soc_apache_show_voltage(unit));
        }
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            SOC_IF_ERROR_RETURN(soc_tomahawk_show_voltage(unit));
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            SOC_IF_ERROR_RETURN(soc_hurricane4_show_voltage(unit));
        }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit)) {
            SOC_IF_ERROR_RETURN(soc_helix5_show_voltage(unit));
        }
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
        if (SOC_IS_FIREBOLT6(unit)) {
            SOC_IF_ERROR_RETURN(soc_firebolt6_show_voltage(unit));
        }
#endif /* BCM_HELIX5_SUPPORT */


        break;
    case 13:
#ifdef BCM_KATANA2_SUPPORT
        if ((soc_feature(unit, soc_feature_linkphy_coe) &&
                    SOC_INFO(unit).linkphy_enabled)) {
            int sid = -1;
            uint32 flag = 0;
            if ((c = ARG_GET(a)) != NULL) {
                if (!sal_strcasecmp(c, "StreamID")) {
                    flag = 1;
                    sid = ((c = ARG_GET(a)) != NULL) ? parse_integer(c) : -1;
                } else {
                    cli_out("Invalid argument\n");
                    return(CMD_FAIL);
                }
            }
            rv = bcm_kt2_subport_port_stat_show(unit, flag, sid);
            if(SOC_FAILURE(rv)) {
                if (rv == BCM_E_INIT) {    
                    cli_out("LinkPHY counter is not initialized\n");
                }
                return(CMD_FAIL);
            }
        }
#endif /* BCM_KATANA2_SUPPORT */
        break;
    case 14:
#ifdef BCM_FIREBOLT6_SUPPORT
      if (SOC_IS_FIREBOLT6(unit)) {
         (void) soc_fb6_show_subport_map(unit);
      }
#endif
        break;
    case 15:     /* ICID */
#ifdef BCM_HELIX5_SUPPORT
        if (soc_feature(unit, soc_feature_chip_icid_info)) {
            (void) soc_helix5_show_icid(unit);
        }
#endif
        break;
    case 16:     /* Threads */
        cli_out("TID      Name\n");
        sal_thread_traverse(do_show_threads, INT_TO_PTR(unit));
        do_show_threads(INT_TO_PTR(unit), sal_thread_id_get(), "*Main");
        break;
    default:
        return(CMD_FAIL);
    }

    return(CMD_OK);
}

/*
 * Clear something for a user.
 */

cmd_result_t
cmd_esw_clear(int unit, args_t *a)
{
    char *parm = ARG_GET(a);
    soc_mem_t mem;
    soc_pbmp_t pbmp;
    bcm_pbmp_t bcm_pbmp;
    int force_all = 0;
    int r, copyno;
    bcm_port_config_t   pcfg;
    int			flags;
    char		*c;
    soc_port_t  port, dport;
    int  rv = CMD_OK;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (!parm) {
	return CMD_USAGE;
    }

    if (!sal_strcasecmp(parm, "all")) {
	force_all = 1;
	parm = ARG_GET(a);
    }

    if (!parm) {
    return CMD_USAGE;
    }


    if (!sal_strcasecmp(parm, "counters") ||
	!sal_strcasecmp(parm, "c")) {
	uint64		val;

	if ((parm = ARG_GET(a)) == NULL) {
            if (soc_feature(unit, soc_feature_cpuport_stat_dma)) {
                SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
                if ((soc_feature(unit, soc_feature_linkphy_coe) &&
                            SOC_INFO(unit).linkphy_enabled) ||
                        (soc_feature(unit, soc_feature_subtag_coe) &&
                         SOC_INFO(unit).subtag_enabled)) {
                    SOC_PBMP_CLEAR(pbmp);
                    SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
                    SOC_PBMP_PORT_ADD(pbmp, CMIC_PORT(unit));
                    SOC_PBMP_PORT_ADD(pbmp, LB_PORT(unit));
                }

            } else {
                SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
            }
	} else if (parse_pbmp(unit, parm, &pbmp) < 0) {
	    cli_out("%s: Invalid port bitmap: %s\n", ARG_CMD(a), parm);
	    return CMD_FAIL;
	}
        /* Remove the lb ports from the bitmap */
        if (soc_feature(unit, soc_feature_internal_loopback)) {
            SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
        }

	if ((r = soc_counter_set32_by_port(unit, pbmp, 0)) < 0) {
	    cli_out("ERROR: Clear counters failed: %s\n", soc_errmsg(r));
	    return CMD_FAIL;
	}

	/*
	 * Clear the diagnostics' copy of the counters so 'show
	 * counters' knows they're clear.
	 */

	COMPILER_64_ZERO(val);

	counter_val_set_by_port(unit, pbmp, val);
	return CMD_OK;
    }

    if (!sal_strcasecmp(parm, "stats")) {
	int		rv = CMD_OK;
	soc_port_t	port, dport;
        bcm_pbmp_t      bcm_pbmp;
        bcm_port_config_t pcfg;

        if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
            cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
            return CMD_FAIL;
        }

	if ((parm = ARG_GET(a)) == NULL) {
            if (soc_feature(unit, soc_feature_cpuport_stat_dma)) {
                SOC_PBMP_ASSIGN(pbmp, pcfg.all);
            } else {
                SOC_PBMP_ASSIGN(pbmp, pcfg.port);
            }
	} else if (parse_bcm_pbmp(unit, parm, &bcm_pbmp) < 0) {
	    cli_out("%s: Invalid port bitmap: %s\n", ARG_CMD(a), parm);
	    return CMD_FAIL;
	}

    /*    coverity[uninit_use : FALSE]    */
        DPORT_BCM_PBMP_ITER(unit, bcm_pbmp, dport, port) {
	    int	rv;

	    if ((rv = bcm_stat_clear(unit, port)) != BCM_E_NONE) {
		cli_out("%s: Unit %d Port %d failed to clear stats: %s\n",
                        ARG_CMD(a), unit, port, bcm_errmsg(rv));
		rv = CMD_FAIL;
	    }
	}
	return(rv);
    }

    if (!sal_strcasecmp(parm, "snmpstats")) {
        if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
            cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
            return CMD_FAIL;
        }

        flags = 0;
        if ((c = ARG_GET(a)) != NULL) {		/* Ports specified? */
            if (0 != parse_bcm_pbmp(unit, c, &bcm_pbmp)) {
                cli_out("%s: Invalid ports: %s\n", ARG_CMD(a), c);
                return(CMD_FAIL);
            }
            if ((c = ARG_GET(a)) != NULL && sal_strcasecmp(c, "all") == 0) {
                    flags = 1;
            }
        } else {
            BCM_PBMP_ASSIGN(bcm_pbmp, pcfg.port);
        }

        DPORT_BCM_PBMP_ITER(unit, bcm_pbmp, dport, port) {
            bcm_stat_val_t s;
            char *sname;
            char *_stat_names[] = BCM_STAT_NAME_INITIALIZER;

            cli_out("%s: Statistics for Unit %d port %s\n",
                   ARG_CMD(a), unit, BCM_PORT_NAME(unit, port));

            for (s = 0; s < snmpValCount; s++) {
                 sname = _stat_names[s];
                 if (!sname) {
                     continue;
                 }

                 rv = bcm_stat_clear_single(unit, port, s);

                 if (rv < 0) {
                     cli_out("%8s\t%s (stat %d): %s\n",
                           "-", sname, s, bcm_errmsg(rv));
                     continue;
                 }

                 if (flags == 0) {
                     continue;
                 }

             }
        }
	    return(rv);
    }
       
    if (!sal_strcasecmp(parm, "dev")) {
        int rv;
        rv = bcm_clear(unit);
        if (rv < 0) {
            cli_out("%s ERROR: Unit %d.  bcm_clear returned %d: %s\n",
                    ARG_CMD(a), unit, rv, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (!sal_strcasecmp(parm, "sa") || !sal_strcasecmp(parm, "sarl")) {
	if (SOC_CONTROL(unit)->arlShadow != NULL) {
	    sal_mutex_take(SOC_CONTROL(unit)->arlShadowMutex,
			   sal_mutex_FOREVER);
	    shr_avl_delete_all(SOC_CONTROL(unit)->arlShadow);
	    sal_mutex_give(SOC_CONTROL(unit)->arlShadowMutex);
	}
	return CMD_OK;
    }

    do {
	if (parse_memory_name(unit, &mem, parm, &copyno, 0) < 0) {
	    cli_out("ERROR: unknown table \"%s\"\n", parm);
	    return CMD_FAIL;
	}

	if ((r = soc_mem_clear(unit, mem, copyno, force_all)) < 0) {
	    cli_out("ERROR: clear table %s failed: %s\n",
                    SOC_MEM_UFNAME(unit, mem), soc_errmsg(r));
	    return CMD_FAIL;
	}
    } while ((parm = ARG_GET(a)) != NULL);

    return CMD_OK;
}

/*
 * Check something for a user.
 */

char check_usage[] =
    "Parameters: check [SYNC] <TABLE>\n\t"
    "Verifies consistency of a memory table (sorted tables only).\n\t"
    "If the SYNC flag is used, checks that all copies are equal.\n";

cmd_result_t
cmd_check(int unit, args_t *a)
{
    char *parm = ARG_GET(a);
    soc_mem_t mem;
    int blk, copyno;
    char *ufname;
    int index, min, max, entries, entry_bytes;
    uint32 previous[SOC_MAX_MEM_WORDS], current[SOC_MAX_MEM_WORDS];
    uint32 *entry_null;
    int r, rv = CMD_OK;
    int sync = 0;
    int port_counts[SOC_MAX_NUM_PORTS];
    int iters, blkfirst;

    if (!sh_check_attached(ARG_CMD(a), unit))
	return CMD_FAIL;

    if (parm != NULL && sal_strcasecmp(parm, "sync") == 0) {
	sync = 1;
	parm = ARG_GET(a);
    }

    if (parm == NULL) {
	return CMD_USAGE;
    }

    if (parse_memory_name(unit, &mem, parm, &copyno, 0) < 0) {
	cli_out("ERROR: unknown table \"%s\"\n", parm);
	return CMD_FAIL;
    }

    ufname = SOC_MEM_UFNAME(unit, mem);
    min = soc_mem_index_min(unit, mem);
    max = soc_mem_index_max(unit, mem);
    entry_bytes = 4 * soc_mem_entry_words(unit, mem);
    entry_null = soc_mem_entry_null(unit, mem);

    if (!soc_mem_is_sorted(unit, mem)) {
	goto skip_sort_check;
    }

    sal_memset(port_counts, 0, sizeof(port_counts));

    iters = 0;
    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
	if (copyno != COPYNO_ALL && copyno != blk) {
	    continue;
	}
	iters += 1;
	entries = soc_mem_entries(unit, mem, blk);

    for (index = min; index <= max; index++) {
        if ((r = soc_mem_read(unit, mem, blk, index, current)) < 0) {
            cli_out("ERROR: failed reading %s.%s[%d]: %s\n",
                    ufname, SOC_BLOCK_NAME(unit, blk),
                    index, soc_errmsg(r));
            return CMD_FAIL;
        }

        if (sal_memcmp(current, entry_null, entry_bytes) == 0)
            break;	/* Found null key */

        if (index > min &&
            soc_mem_compare_key(unit, mem, current, previous) <= 0) {
                cli_out("ERROR: non-increasing key at %s.%s[%d]\n",
                        ufname, SOC_BLOCK_NAME(unit, blk), index);
                rv = CMD_FAIL;
        }

        sal_memcpy(previous, current, entry_bytes);
    }

	/* Make sure entry count matches number of entries found */

	if (index != min + entries) {
	    cli_out("ERROR: last non-null key was found at %s.%s[%d]\n",
                    ufname, SOC_BLOCK_NAME(unit, blk), index - 1);
	    cli_out("ERROR: but was expected to be at %s.%s[%d]\n",
                    ufname, SOC_BLOCK_NAME(unit, blk), min + entries);
	    cli_out("ERROR: based on a table entry count of %d\n",
                    entries);
	    rv = CMD_FAIL;
	}

	/* Make sure remaining entries in table are null */

	for (; index <= max; index++) {
	    if ((r = soc_mem_read(unit, mem, blk, index, current)) < 0) {
		cli_out("ERROR: failed reading %s.%s[%d]: %s\n",
                        ufname, SOC_BLOCK_NAME(unit, blk),
                        index, soc_errmsg(r));
		return CMD_FAIL;
	    }

	    if (sal_memcmp(current, entry_null, entry_bytes) != 0) {
		cli_out("ERROR: entry at %s.%s[%d] should contain null key\n",
                        ufname, SOC_BLOCK_NAME(unit, blk), index);
		rv = CMD_FAIL;
	    }
	}
    }

    /*
     * Optionally check that all copies of a table have identical contents.
     */

 skip_sort_check:

    if (sync) {
        for (index = min; index <= max; index++) {
            iters = 0;
            blkfirst = 0;
            SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                if (copyno != COPYNO_ALL && copyno != blk) {
                    continue;
                }
                if (++iters == 1) {
                    blkfirst = blk;
                }
                if ((r = soc_mem_read(unit, mem, blk, index,
                    iters == 1 ? previous : current) < 0)) {
                    cli_out("ERROR: failed reading %s.%s[%d]: %s\n",
                            ufname, SOC_BLOCK_NAME(unit, blk),
                            index, soc_errmsg(r));
                    return CMD_FAIL;
                }

                if (iters > 1) {
                    if (sal_memcmp(current, previous, entry_bytes) != 0) {
                        cli_out("ERROR: %s.%s[%d] does not match %s.%s[%d]\n",
                                ufname, SOC_BLOCK_NAME(unit, blk), index,
                                ufname, SOC_BLOCK_NAME(unit, blkfirst), index);
                        rv = CMD_FAIL;
                    }
                }
            }
        }
    }

    return rv;
}


#if defined(BCM_KATANA2_SUPPORT)
/*
 * data structure to store the hw linkphy information
 */
typedef struct linkphy_info_s {
    int     port;
    int     ppp;
    uint32  ext_stream_id;     /* external stream id ingressing a port */
    uint32  adj_ext_stream_id; /* base adjusted port external stream Id */
    uint16  blk_int_stream_id; /* block internal stream id range 0 - 127 */
    uint16  dev_int_stream_id; /* device internal stream id range 0 - 255 */
    uint32  rxlp_stream_active;
    uint32  txlp_ext_stream_id;
    uint32  txlp_stream_active;
}linkphy_info_t; 

/*
 * Function:    _linkphy_print_header
 * Purpose:     Print header string for output.
 * Parameters:  None.
 * Returns:     Nothing
 */
void
_linkphy_print_header(void)
{
    cli_out("+----+---+-----+-----+-----+-----+-----+-----+-----+\n");
    cli_out("|    |   |Rxlp |Port |Blk  |Dev  |Rxlp |Txlp |Txlp |\n");
    cli_out("|    |   |Ext  |Ext  |Int  |Int  |Actv |Ext  |Actv |\n");
    cli_out("|port|ppp|StrId|StrId|StrId|StrId|StrId|StrId|StrId|\n");
    cli_out("+----+---+-----+-----+-----+-----+-----+-----+-----+\n");
}

/*
 * Function:    _linkphy_print_seperator
 * Purpose:     Print header string for output.
 * Parameters:  None.
 * Returns:     Nothing
 */
void
_linkphy_print_separator(void)
{
    cli_out("+----+---+-----+-----+-----+-----+-----+-----+-----+\n");
}

/*
 * Function:    _linkphy_print_summary
 * Purpose:     Print summary values
 * Parameters:  Summary values
 * Returns:     Nothing
 */
void
_linkphy_print_summary(int unit, linkphy_info_t info)
{
    cli_out("|%4d|%3d|%5d|%5d|%5d|%5d|%5d|%5d|%5d|\n",
            info.port, info.ppp, info.ext_stream_id, 
            info.adj_ext_stream_id, info.blk_int_stream_id,
            info.dev_int_stream_id, info.rxlp_stream_active,
            info.txlp_ext_stream_id, info.txlp_stream_active);
}

/*
 * Function:    do_show_linkphy_config
 * Purpose:     retrieves all the hardware information 
 *              each linkphy port. 
 * Parameters:  Summary values
 * Returns:     Failure/Success
 */
cmd_result_t
do_show_linkphy_config(int unit)
{
    int port = -1, ppp = -1, rv = BCM_E_NONE;
    int lp_block = 0, lp_index = 0, copyno = 0;
    int idx_min = 0, idx_max = 0, idx = 0;
    int reg_hi, stream_bit = 0;
    soc_mem_t mem, port_mem;
    soc_reg_t reg = INVALIDr;
    uint32 regval = 0, valid = 0;
    uint64 rval64, data;
    int field_word = 0, bit_pos = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 fldbuf[4] = { 0 };
    rxlp_internal_stream_map_port_0_entry_t rxlp_int_stream_map_port;
    txlp_port_stream_bitmap_table_entry_t txlp_port_stream_bitmap;
    txlp_int2ext_stream_map_table_entry_t txlp_int2ext_stream_map;
    device_stream_id_to_pp_port_map_entry_t dev_sid_to_pp_entry;
    uint16 blk_int_stream_id = 0, dev_int_stream_id = 0;
    uint32 port_ext_stream_id_base = 0, adj_ext_stream_id = 0;
    uint32 ext_stream_id = 0, txlp_ext_stream_id = 0;
    uint32 int_stream_id_base = 0;
    uint32 rxlp_stream_active = 0, txlp_stream_active = 0;
    linkphy_info_t  info;

    /* check if LinkPhy is enabled on any port */
    if (BCM_PBMP_IS_NULL(si->linkphy_pbm)) {
        cli_out("Linkphy is not enabled on any port\n");
        return CMD_FAIL;
    }

    sal_memset(&info, 0, sizeof(linkphy_info_t));

    _linkphy_print_header();
    BCM_PBMP_ITER(si->linkphy_pbm, port) {    
        
        /* get the block and index for the LinkPHY memory */
        rv = soc_kt2_linkphy_port_blk_idx_get(unit, port, &lp_block, &lp_index);
        if (rv < 0) {
            cli_out("LinkPHY block, index get failed for port %d\n", port);
            return CMD_FAIL;
        }

        /* 
         * read RXLP internal stream map table for external to block internal
         * stream id.
         */

        port_mem = RXLP_INTERNAL_STREAM_MAP_PORT_0m;

        if (!SOC_MEM_FIELD_VALID(unit, port_mem, INTERNAL_STREAM_IDf)) {
            continue;
        }
        sal_memset(&rxlp_int_stream_map_port, 0, 
            sizeof(rxlp_internal_stream_map_port_0_entry_t));

        /*
        * Internal stream id = 
        * RXLP_INTERNAL_STREAM_MAP_PORTn.rxlp0/1 [Internal Stream ID];
        */
        switch (lp_index) {
            case 0:
                port_mem = RXLP_INTERNAL_STREAM_MAP_PORT_0m;
                break;
            case 1:
                port_mem = RXLP_INTERNAL_STREAM_MAP_PORT_1m;
                break;
            case 2:
                port_mem = RXLP_INTERNAL_STREAM_MAP_PORT_2m;
                break;
            case 3:
                port_mem = RXLP_INTERNAL_STREAM_MAP_PORT_3m;
                break;
            default:
                return CMD_FAIL;
        };

        idx_min = soc_mem_index_min(unit, port_mem);
        idx_max = soc_mem_index_max(unit, port_mem);

        for (idx = idx_min; idx <= idx_max; idx++) {

            copyno = SOC_MEM_BLOCK_ANY(unit, port_mem) + lp_block;
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, port_mem, copyno, idx,
                                             &rxlp_int_stream_map_port));
            valid = soc_mem_field32_get(unit, port_mem,
                        (uint32 *)&rxlp_int_stream_map_port, VALIDf);
            if (!valid) {
                continue;
            }

            blk_int_stream_id = soc_mem_field32_get(unit, port_mem,
            (uint32 *)&rxlp_int_stream_map_port, INTERNAL_STREAM_IDf);
            adj_ext_stream_id = idx;
            reg = RXLP_PORT_STREAM_ID_BASEr;
            if (SOC_REG_PORT_VALID(unit, reg, port)) {
                SOC_IF_ERROR_RETURN(
                    READ_RXLP_PORT_STREAM_ID_BASEr(unit, port, &regval));
                port_ext_stream_id_base = soc_reg_field_get(unit, reg,
                                                            regval, BASEf);
            }
            ext_stream_id = adj_ext_stream_id + port_ext_stream_id_base;

            reg = RXLP_INT_STREAM_ID_BASEr;
            if (SOC_REG_PORT_VALID(unit, reg, port)) {
                SOC_IF_ERROR_RETURN(
                        READ_RXLP_INT_STREAM_ID_BASEr(unit, port, &regval));
                int_stream_id_base = soc_reg_field_get(unit, reg,
                                                       regval, BASEf);
            }
            dev_int_stream_id = blk_int_stream_id + int_stream_id_base;

            reg_hi = (int ) (blk_int_stream_id / 64);
            stream_bit = (int ) blk_int_stream_id % 64;

            reg = (reg_hi) ? RXLP_PORT_ACTIVE_STREAM_BITMAP_HIr :
                             RXLP_PORT_ACTIVE_STREAM_BITMAP_LOr;
            if (SOC_REG_PORT_VALID(unit, reg, port)) {
                SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
                data = soc_reg64_field_get(unit, reg,
                                           rval64, ACTIVE_STREAM_BITMAPf);
                if (COMPILER_64_BITTEST(data, stream_bit)) {
                    rxlp_stream_active = 1;
                }
            }

            mem = TXLP_PORT_STREAM_BITMAP_TABLEm;
            if (SOC_MEM_FIELD_VALID(unit, mem, STREAM_BITMAPf)) {

                copyno = SOC_MEM_BLOCK_ANY(unit, mem) + lp_block;
                SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, copyno, lp_index,
                    &txlp_port_stream_bitmap));
                soc_mem_field_get(unit, mem, (uint32 *)&txlp_port_stream_bitmap,
                         STREAM_BITMAPf, fldbuf);

                field_word = blk_int_stream_id / 32;
                bit_pos  = blk_int_stream_id % 32;
                if (fldbuf[field_word] & (1 << bit_pos)) {
                    txlp_stream_active = 1;
                }
            }
            mem = TXLP_INT2EXT_STREAM_MAP_TABLEm;
            /* get the block and index for the LinkPHY memory */
            sal_memset(&txlp_int2ext_stream_map, 0,
                    sizeof(txlp_int2ext_stream_map_table_entry_t));
            copyno = SOC_MEM_BLOCK_ANY(unit, mem) + lp_block;
            (void) soc_mem_read(unit, mem, copyno, blk_int_stream_id,
                                &txlp_int2ext_stream_map);
            valid = soc_mem_field32_get(unit, mem,
                                    (uint32 *)&txlp_int2ext_stream_map,
                                    VALIDf);
            if (valid) {
                txlp_ext_stream_id = soc_mem_field32_get(unit, mem,
                                       (uint32 *)&txlp_int2ext_stream_map,
                                       EXTERNAL_SIDf);
            }

            /* get the pp_port from dev stream id */
            mem = DEVICE_STREAM_ID_TO_PP_PORT_MAPm;
            sal_memset(&dev_sid_to_pp_entry, 0,
                    sizeof(dev_sid_to_pp_entry));
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
            (void) soc_mem_read(unit, mem, copyno, dev_int_stream_id,
                    &dev_sid_to_pp_entry);
            ppp = soc_mem_field32_get(unit, mem,
                                     (uint32 *)&dev_sid_to_pp_entry,
                                     PP_PORTf);
            info.port = port;
            info.ppp = ppp;
            info.ext_stream_id = ext_stream_id;
            info.adj_ext_stream_id = adj_ext_stream_id;
            info.blk_int_stream_id = blk_int_stream_id;
            info.dev_int_stream_id = dev_int_stream_id;
            info.rxlp_stream_active = rxlp_stream_active;
            info.txlp_ext_stream_id = txlp_ext_stream_id;
            info.txlp_stream_active = txlp_stream_active;

            _linkphy_print_summary(unit, info);
        } /* for */
        _linkphy_print_separator();
    }
    _linkphy_print_separator();

    return CMD_OK;
}
#endif /* BCM_KATANA2_SUPPORT */

