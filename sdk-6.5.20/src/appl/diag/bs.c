/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/error.h>
#include <bcm/time.h>
#include <shared/bsl.h>

#include <soc/drv.h>
#include <appl/diag/system.h>

#include <appl/diag/parse.h>

#ifdef NO_SAL_APPL
#include <string.h>
#else
#include <sal/appl/sal.h>
#endif

#ifdef BCM_CMICM_SUPPORT

#include <bcm_int/esw/port.h>
#include <bcm_int/common/mbox.h>
#include <bcm_int/common/time.h>
#include <bcm_int/common/time-mbox.h>
#include <include/soc/uc.h>

#if defined (BCM_MONTEREY_SUPPORT)
#define DUAL_BS
#endif

#if defined(INCLUDE_PTP)
extern int _bcm_ptp_running(int unit);
#endif /* INCLUDE_PTP */

#if defined(INCLUDE_GDPLL)
extern int _bcm_time_tod_dpll_chan_create(int unit, bcm_time_format_t tod_type);
#endif

/* A sentinel value to indicate that no valid value was passed in */
/* static int64 ILLEGAL_OFFSET_LL = COMPILER_64_INIT(0x7fffffff, 0xffffffff); */
static uint32 ILLEGAL_OFFSET = 0x7fffffff;

#define BS_NANOSECONDS_PER_SECOND (1000000000)

#define CLEAN_UP_AND_RETURN(_result)            \
    parse_arg_eq_done(&parse_table);            \
    return (_result);

#define _isprintable(_c) (((_c) > 32) && ((_c) < 127))

char cmd_broadsync_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: BroadSync <option> [args...]\n"
#else
#ifdef DUAL_BS
    "\n"
    "  BroadSync CONFig [Master] [BS1] [BitClock=<hz>] [HeartBeat=<hz>] [PhaseSec=<seconds>] [PhaseNSec]=<nanoseconds>\n"
    "                   [NtpSec=<seconds>] [NtpNSec]=<nanoseconds> [BSTimeSec=<seconds>] \n"
    "    Initialize or reconfigure.  Default is slave, if 'Master' is not specified\n"
    "    Default is BS0, if 'BS1' is not specified\n"
    "  BroadSync STATus [BS0|BS1]\n"
    "    Show DPLL lock state\n"
    "    Default is BS0, if 'BS0/''BS1' is not specified\n "
    "  BroadSync DEBUG [ON|OFF]\n"
    "    No option: print debug state.  ON/OFF: enable/disable debug output\n"
    "  BroadSync LOG [LogDebug] [LogPLL] [Vlan=<vlan>] [DestMAC=<mac>] [DestAddr=<addr>] [SrcMAC=<mac>] [SrcAddr=<addr>] [UDPPort=<udp port>]\n"
    "    Sets UDP logging state\n"
    "  BroadSync CLEANUP\n"
    "    Cleanup the interface\n"
#else
    "\n"
    "  BroadSync CONFig [Master] [BitClock=<hz>] [HeartBeat=<hz>] [Freq=<ppb offset>] [PhaseSec=<seconds>] [PhaseNSec]=<nanoseconds>\n"
    "                   [NtpSec=<seconds>] [NtpNSec]=<nanoseconds> [BSTimeSec=<seconds>] \n"
    "    Initialize or reconfigure.  Default is slave, if 'Master' is not specified\n"
    "  BroadSync STATus\n"
    "    Show DPLL lock state\n"
    "  BroadSync 1PPS [ON|OFF]\n"
    "    No option: print 1PPS state.  ON/OFF: enable/disable 1PPS output\n"
    "  BroadSync DEBUG [ON|OFF]\n"
    "    No option: print debug state.  ON/OFF: enable/disable debug output\n"
    "  BroadSync LOG [LogDebug] [LogPLL] [Vlan=<vlan>] [DestMAC=<mac>] [DestAddr=<addr>] [SrcMAC=<mac>] [SrcAddr=<addr>] [UDPPort=<udp port>]\n"
    "    Sets UDP logging state\n"
    "  BroadSync CLEANUP\n"
    "    Cleanup the interface\n"
#endif
#endif
    ;

typedef struct {
    bcm_time_if_t   bs_id;
    int             bs_initialized;
} _bcm_time_intf_info;

int _bcm_time_interface_traverse_cb(int unit, bcm_time_interface_t *intf, void *user_data)
{
    _bcm_time_intf_info *bstime_intf_info = (_bcm_time_intf_info*)user_data;
    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
        if (intf->flags & BCM_TIME_WITH_ID) {
            if(bstime_intf_info->bs_id == intf->id) { 
                bstime_intf_info->bs_initialized = 1;
            } else {
                return (BCM_E_BADID);
            }
        } else {
            bstime_intf_info->bs_id = intf->id;
            bstime_intf_info->bs_initialized = 1;
        }
    }
    return (BCM_E_NONE);
}

cmd_result_t cmd_broadsync(int unit, args_t *args)
{
    char *arg_string_p = NULL;
    int result;
    _bcm_time_intf_info bstime_intf_info;

    int arg_is_master = 0, arg_nophase = 0;

#ifdef DUAL_BS
    int arg_is_bs1=0;
    char *arg_str_bs_id;
#endif

    /* Check to verify Broadsync Firmware is present */
    int uc;
    char *version = NULL;
    int bs_fw = 0;

    for(uc=0; uc<SOC_INFO(unit).num_ucs; uc++) {
        if (!soc_uc_in_reset(unit, uc)) {
            version = soc_uc_firmware_version(unit, uc);
            if (version) {
#ifdef NO_SAL_APPL
                if (strstr((char *)version,(char *)"BS"))
#else
                if (sal_strcasestr((char *)version,(char *)"BS"))
#endif
                {
                    bs_fw = 1;
                }

                soc_cm_sfree(unit, version);
                if (1 == bs_fw) {
                    break;
                }
            }
        }
    }
    if (bs_fw == 0) {
        cli_out("Failed: Broadsync firmware not loaded\n");
        return CMD_FAIL;
    }

    arg_string_p = ARG_GET(args);

    if (arg_string_p == NULL) {
        return CMD_USAGE;
    }

    if (!sh_check_attached(ARG_CMD(args), unit)) {
        return CMD_FAIL;
    }

    bstime_intf_info.bs_id = 0;
#ifdef DUAL_BS
    arg_str_bs_id = (args->a_argc > args->a_arg)?args->a_argv[args->a_arg]:NULL;
    if (arg_str_bs_id == NULL) {
        bstime_intf_info.bs_id = 0;
    } else if (parse_cmp("bs1", arg_str_bs_id, 0)){
        bstime_intf_info.bs_id = 1;
    }
#endif
    bstime_intf_info.bs_initialized = 0;

    bcm_time_interface_traverse(unit, _bcm_time_interface_traverse_cb,
                                (void*)&bstime_intf_info);
    cli_out("\n BS%d. bs_initialized[%d]\n", bstime_intf_info.bs_id, bstime_intf_info.bs_initialized);

    if (parse_cmp("CONFig", arg_string_p, 0)) {
        parse_table_t parse_table;
        int n_args = 0;
        bcm_time_interface_t intf;
        int32 freq = ILLEGAL_OFFSET;
        int32 phase_sec = ILLEGAL_OFFSET;
        int32 phase_nsec = ILLEGAL_OFFSET;
        uint32 bs_time_sec = 0;
        uint32 ntp_sec = 0;
        uint32 ntp_nsec = 0;
        int def_bitclock_hz = 10000000;
        int def_heartbeat_hz = 4000;

        parse_table_init(unit, &parse_table);

#ifdef DUAL_BS
        parse_table_add(&parse_table, "BS1", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &arg_is_bs1, NULL);
#endif
        parse_table_add(&parse_table, "Master", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &arg_is_master, NULL);
        parse_table_add(&parse_table, "BitClock", PQ_INT, (void*)(size_t)def_bitclock_hz, &intf.bitclock_hz, NULL);
        parse_table_add(&parse_table, "HeartBeat", PQ_INT, (void*)(size_t)def_heartbeat_hz, &intf.heartbeat_hz, NULL);
        parse_table_add(&parse_table, "Freq", PQ_DFL | PQ_INT, NULL, &freq, NULL);
        parse_table_add(&parse_table, "PhaseSec", PQ_DFL | PQ_INT, NULL, &phase_sec, NULL);
        parse_table_add(&parse_table, "PhaseNSec", PQ_DFL | PQ_INT, NULL, &phase_nsec, NULL);
        parse_table_add(&parse_table, "NoPhase", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &arg_nophase, NULL);
        parse_table_add(&parse_table, "NtpSec", PQ_DFL | PQ_INT, NULL, &ntp_sec, NULL);
        parse_table_add(&parse_table, "NtpNSec", PQ_DFL | PQ_INT, NULL, &ntp_nsec, NULL);
        parse_table_add(&parse_table, "BSTimeSec", PQ_DFL | PQ_INT, NULL, &bs_time_sec, NULL);
        /* When PTP is running, "NoPhase" can be used to indicate that the servo phase should be ignored
         * i.e. that the BroadSync time should be the timestamper time, not PTP time, by clearing REF_CLOCK
         */
        n_args = parse_arg_eq(args, &parse_table);

        if ((n_args < 0) || (ARG_CNT(args) > 0)) {
            cli_out("Invalid option: %s\n", ARG_CUR(args));
            CLEAN_UP_AND_RETURN(CMD_USAGE);
        }

#ifdef DUAL_BS
        cli_out("\n interface id:%d ", arg_is_bs1);
        if (arg_is_bs1) {
            bstime_intf_info.bs_id = arg_is_bs1;
            bstime_intf_info.bs_initialized = 0;

            bcm_time_interface_traverse(unit, _bcm_time_interface_traverse_cb, (void*)&bstime_intf_info);
            cli_out("\n traverse BS%d. \n\t bs_initialized[%d]", bstime_intf_info.bs_id, bstime_intf_info.bs_initialized);
        }
#endif

        if (bstime_intf_info.bs_initialized) {
            intf.id = bstime_intf_info.bs_id;
            intf.flags = BCM_TIME_OFFSET | BCM_TIME_DRIFT |
                BCM_TIME_REF_CLOCK | BCM_TIME_HEARTBEAT;
            bcm_time_interface_get(unit, &intf);

            intf.flags = BCM_TIME_WITH_ID | BCM_TIME_REPLACE |
                BCM_TIME_ENABLE | BCM_TIME_SYNC_STAMPER |
                BCM_TIME_REF_CLOCK | BCM_TIME_HEARTBEAT;
        } else {
#if defined(INCLUDE_PTP)
             if (_bcm_ptp_running(unit) != BCM_E_NONE) {
#endif /* INCLUDE_PTP */
                /* Check for BS firmware on the unit */
                result = _bcm_mbox_comm_init(unit, MOS_MSG_MBOX_APPL_BS);
                if (BCM_FAILURE(result)) {
                    cli_out("Broadsync MBox Communication FAILED!!\n");
                    return CMD_FAIL;
                }
#if defined(INCLUDE_PTP)
            }
#endif /* INCLUDE_PTP */

            intf.flags = BCM_TIME_ENABLE | BCM_TIME_SYNC_STAMPER |
                BCM_TIME_REF_CLOCK | BCM_TIME_HEARTBEAT;

#ifdef DUAL_BS
            intf.id = arg_is_bs1;                           /* Time Interface Identifier */
            intf.flags |= BCM_TIME_WITH_ID;
#else
            intf.id = 0;                                    /* Time Interface Identifier */
#endif
            intf.drift.isnegative = 0;                      /* Drift amount per 1 Sec */
            COMPILER_64_ZERO(intf.drift.seconds);
            intf.drift.nanoseconds = 0;
            intf.offset.isnegative = 0;                     /* Offset */
            COMPILER_64_ZERO(intf.offset.seconds);
            intf.offset.nanoseconds = 0;
            intf.accuracy.isnegative = 0;                   /* Accuracy */
            COMPILER_64_ZERO(intf.accuracy.seconds);
            intf.accuracy.nanoseconds = 0;
            intf.clk_resolution = 1;                        /* Reference clock resolution in nsecs */
        }

        intf.flags &= ~(BCM_TIME_DRIFT);
        intf.flags &= ~(BCM_TIME_OFFSET);
        intf.flags &= ~(BCM_TIME_BS_TIME);


        if (!arg_is_master) {
            intf.flags |= BCM_TIME_INPUT;
        }

        if (arg_nophase) {
            intf.flags &= ~BCM_TIME_REF_CLOCK;
        }

        if (freq != ILLEGAL_OFFSET) {
            intf.flags |= BCM_TIME_DRIFT;
            intf.drift.isnegative = (freq < 0) ? 1 : 0;
            COMPILER_64_ZERO(intf.drift.seconds);
            freq = (freq < 0) ? (-freq) : (freq);
            intf.drift.nanoseconds = (uint32) freq;
        }

        if ((phase_sec != ILLEGAL_OFFSET) || (phase_nsec != ILLEGAL_OFFSET)) {
            /* Valid seconds or nanoseconds component of phase offset. */
            intf.flags |= BCM_TIME_OFFSET;

            phase_sec = (phase_sec == ILLEGAL_OFFSET) ? 0 : phase_sec;
            phase_nsec = (phase_nsec == ILLEGAL_OFFSET) ? 0 : phase_nsec; 

            /* Discard integer seconds, if any,  from nanoseconds term. */
            while (phase_nsec >= BS_NANOSECONDS_PER_SECOND) {
                phase_nsec -= BS_NANOSECONDS_PER_SECOND;
            } 
            while (phase_nsec <= -BS_NANOSECONDS_PER_SECOND) {
                phase_nsec += BS_NANOSECONDS_PER_SECOND;
            } 

            /* Reconcile different signs in seconds and nanoseconds terms. */
            if (phase_sec > 0 && phase_nsec < 0) {
                --phase_sec;
                phase_nsec += BS_NANOSECONDS_PER_SECOND;
            } else if (phase_sec < 0 && phase_nsec > 0) {
                ++phase_sec;
                phase_nsec -= BS_NANOSECONDS_PER_SECOND;
            }

            intf.offset.isnegative = ((phase_sec < 0) || (phase_nsec < 0)) ? 1 : 0;
            phase_sec = (intf.offset.isnegative) ? (-phase_sec) : (phase_sec);
            phase_nsec = (intf.offset.isnegative) ? (-phase_nsec) : (phase_nsec);

            COMPILER_64_SET(intf.offset.seconds, 0, ((uint32)phase_sec));
            intf.offset.nanoseconds = (uint32) phase_nsec;
        } else {
            /* Invalid seconds and nanoseconds component of phase offset. */
            intf.flags &= ~(BCM_TIME_OFFSET);
        }

        if (bs_time_sec != 0) {
            /* Valid seconds or nanoseconds component */
            intf.flags |= BCM_TIME_BS_TIME;

            intf.bs_time.isnegative = 0;
            COMPILER_64_SET(intf.bs_time.seconds, 0, bs_time_sec);
            intf.bs_time.nanoseconds = 0;
        } else {
            /* Invalid seconds and nanoseconds component . */
            intf.flags &= ~(BCM_TIME_BS_TIME);
        }

        if (ntp_sec != 0) {
            intf.flags |= BCM_TIME_NTP_OFFSET;
            
            while (ntp_nsec >= BS_NANOSECONDS_PER_SECOND) {
                ntp_nsec -= BS_NANOSECONDS_PER_SECOND;
            } 

            /*ntp offset is always positive*/
            intf.ntp_offset.isnegative = 0;
            COMPILER_64_SET(intf.ntp_offset.seconds, 0, ntp_sec);
            intf.ntp_offset.nanoseconds = ntp_nsec;
        } else {
            intf.flags &= ~(BCM_TIME_NTP_OFFSET);
        }

        result = bcm_time_interface_add(unit, &intf);
        if (BCM_FAILURE(result)) {
            cli_out("Command failed. %s\n", bcm_errmsg(result));
            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }
                    
        CLEAN_UP_AND_RETURN(CMD_OK);

    } else if (parse_cmp("reattach", arg_string_p, 0)) {

        if (!bstime_intf_info.bs_initialized) {
            cli_out("BroadSync not initialized on unit %d\n", unit);
            return CMD_FAIL;
        }

        /* Reattach BS firmware on the unit */
        result = _bcm_mbox_comm_init(unit, MOS_MSG_MBOX_APPL_BS);
        if (BCM_FAILURE(result)) {
            cli_out("Broadsync reattach FAILED\n");
            return CMD_FAIL;
        }

        cli_out("BroadSync FW reattached on unit %d\n", unit);

    } else if (parse_cmp("STATus", arg_string_p, 0)) {

#ifdef DUAL_BS
        bcm_time_interface_t intf;

        sal_memset(&intf, 0, sizeof(bcm_time_interface_t));

        arg_string_p = ARG_GET(args);

        if (arg_string_p == NULL) {
            intf.id = 0;
        } else if (parse_cmp("bs1", arg_string_p, 0)) {
            intf.id = 1;
        }
        result = bcm_time_interface_get(unit, &intf);
        if (BCM_FAILURE(result)) {
            cli_out("Broadsync i/f get failed with rv%d\n", result);
            return CMD_FAIL;
        } else {
            cli_out("Status = %d\n", intf.status);
        }
#else
        int status = 0;

        if (!bstime_intf_info.bs_initialized) {
            cli_out("BroadSync not initialized on unit %d\n", unit);
            return CMD_FAIL;
        }

        arg_string_p = ARG_GET(args);

        if (arg_string_p != NULL) {
            return CMD_USAGE;
        }

        _bcm_time_bs_status_get(unit, &status);

        cli_out("Status = %d\n", status);
#endif
    } else if (parse_cmp("wb", arg_string_p, 0)) {

        /* Send the message to BS-FW about warmboot */
        cli_out("Warmboot prep on BroadSync FW for unit %d\n",
                unit);

        if (!bstime_intf_info.bs_initialized) {
            cli_out("BroadSync not initialized on unit %d\n", unit);
            return CMD_FAIL;
        }

        arg_string_p = ARG_GET(args);
        if (NULL != arg_string_p) {
            return CMD_USAGE;
        }

        if (BCM_E_NONE != _bcm_mbox_bs_wb_prep(unit)) {
            cli_out("BroadSync-FW warmboot prep failed!! \n");
            return CMD_FAIL;
        }
#if !defined(MONTEREY)
    } else if (parse_cmp("1PPS", arg_string_p, 0)) {
        if (!bstime_intf_info.bs_initialized) {
            cli_out("BroadSync not initialized on unit %d\n", unit);
            return CMD_FAIL;
        }

        arg_string_p = ARG_GET(args);

        if (arg_string_p == NULL) {
            int enabled;

            if (BCM_FAILURE(bcm_time_heartbeat_enable_get(unit, bstime_intf_info.bs_id, &enabled))) {
                cli_out("Error getting heartbeat (1pps) enable status\n");
                return CMD_FAIL;
            } else {
                cli_out("1PPS is %s\n", (enabled) ? "Enabled" : "Disabled");
                return CMD_OK;
            }
        }

        if (parse_cmp(arg_string_p, "ON", 0)) {
            result = bcm_time_heartbeat_enable_set(unit, bstime_intf_info.bs_id, 1);
        } else if (parse_cmp(arg_string_p, "OFF", 0)) {
            result = bcm_time_heartbeat_enable_set(unit, bstime_intf_info.bs_id, 0);
        } else {
            return CMD_USAGE;
        }

        if (BCM_FAILURE(result)) {
            cli_out("Command failed. %s\n", bcm_errmsg(result));
            return CMD_FAIL;
        }
#endif /* !defined(MONTEREY) */

    } else if (parse_cmp("DEBUG", arg_string_p, 0)) {

#ifdef DUAL_BS
        if (!bstime_intf_info.bs_initialized) {
            /* check if BS1 is initialized in case if BS0 is not initialized */
            bstime_intf_info.bs_id = 1;
            bstime_intf_info.bs_initialized = 0;

            bcm_time_interface_traverse(unit, _bcm_time_interface_traverse_cb,
                                (void*)&bstime_intf_info);
        }
#endif

        if (!bstime_intf_info.bs_initialized) {
            cli_out("BroadSync not initialized on unit %d\n", unit);
            return CMD_FAIL;
        }

        arg_string_p = ARG_GET(args);

        if (arg_string_p == NULL) {
            uint32 flags;
            if (BCM_FAILURE(_bcm_mbox_debug_flag_get(&flags))) {
                cli_out("Error getting debug status\n");
            } else {
                cli_out("Debug output is %s\n", (flags) ? "On" : "Off");
                return CMD_OK;
            }
        }

        if (parse_cmp("ON", arg_string_p, 0)) {
            _bcm_mbox_debug_flag_set(0xff);
        } else if (parse_cmp(arg_string_p, "OFF", 0)) {
            _bcm_mbox_debug_flag_set(0);
        } else {
            return CMD_USAGE;
        }

    } else if (parse_cmp("LOG", arg_string_p, 0)) {
        parse_table_t parse_table;
        int arg_log_debug = 0;
        int arg_log_pll = 0;
        bcm_mac_t dest_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; /* default to broadcast */
        bcm_mac_t src_mac = {0x00, 0x10, 0x18, 0x00, 0x00, 0x01};
        bcm_ip_t dest_ip = 0xffffffff; /* default to broadcast */
        bcm_ip_t src_ip = (192L << 24) + (168L << 16) +(0L << 8) + 90;
        int udp_port = 0x4455;
        int tpid = 0x8100;
        int vlan = 1;
        int ttl = 1;
        uint64 udp_log_mask = COMPILER_64_INIT(0,0);
        uint64 log_debug_bit = COMPILER_64_INIT(0,0x01);
        uint64 log_pll_bit = COMPILER_64_INIT(0,0x40);

        int rv = 0;

        if (!bstime_intf_info.bs_initialized) {
            cli_out("BroadSync not initialized on unit %d\n", unit);
            return CMD_FAIL;
        }

        parse_table_init(unit, &parse_table);

        parse_table_add(&parse_table, "LogDebug", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &arg_log_debug, NULL);
        parse_table_add(&parse_table, "LogPLL", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &arg_log_pll, NULL);
        parse_table_add(&parse_table, "DestMAC", PQ_DFL | PQ_MAC, NULL, dest_mac, NULL);
        parse_table_add(&parse_table, "DestAddr", PQ_DFL | PQ_IP, NULL, &dest_ip, NULL);
        parse_table_add(&parse_table, "SrcMAC", PQ_DFL | PQ_MAC, NULL, src_mac, NULL);
        parse_table_add(&parse_table, "SrcAddr", PQ_DFL | PQ_IP, NULL, &src_ip, NULL);
        parse_table_add(&parse_table, "UDPPort", PQ_DFL | PQ_INT, NULL, &udp_port, NULL);
        parse_table_add(&parse_table, "Vlan", PQ_DFL | PQ_INT, NULL, &vlan, NULL);

        if (parse_arg_eq(args, &parse_table) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(args), ARG_CUR(args));
        }

        if (arg_log_debug) {
            COMPILER_64_OR(udp_log_mask, log_debug_bit);
        }
        if (arg_log_pll) {
            COMPILER_64_OR(udp_log_mask, log_pll_bit);
        }

        rv = _bcm_time_bs_log_configure(unit, 0xff, udp_log_mask,
                                        src_mac, dest_mac, tpid, vlan, ttl,
                                        src_ip, dest_ip, udp_port);
        
        if (rv != BCM_E_NONE) {
            cli_out("Failed setting log configuration: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else if (parse_cmp("CLEANUP", arg_string_p, 0)) {
#ifdef DUAL_BS
        arg_string_p = ARG_GET(args);

        if (arg_string_p == NULL) {
            bstime_intf_info.bs_id = 0;
        } else if (parse_cmp("bs0", arg_string_p, 0)) {
            bstime_intf_info.bs_id = 0;
        } else if (parse_cmp("bs1", arg_string_p, 0)) {
            bstime_intf_info.bs_id = 1;
        }
        bstime_intf_info.bs_initialized = 0;

        bcm_time_interface_traverse(unit, _bcm_time_interface_traverse_cb, (void*)&bstime_intf_info);
#endif

        if (bstime_intf_info.bs_initialized) {
            cli_out("Broadsync cleanup: bs%d\n", bstime_intf_info.bs_id);
            result = bcm_time_interface_delete(unit, bstime_intf_info.bs_id);
            if (BCM_FAILURE(result)) {
                cli_out("Command failed. %s\n", bcm_errmsg(result));
                return CMD_FAIL;
            }
            return CMD_OK;
        }
#if defined(INCLUDE_GDPLL)
    } else if (parse_cmp("CHAN", arg_string_p, 0)) {
        arg_string_p = ARG_GET(args);
        bcm_time_format_t tod_type;

        if (arg_string_p == NULL) {
            return CMD_FAIL;
        } else if (parse_cmp("NTP", arg_string_p, 0)) {
            tod_type = bcmTimeFormatNTP;
        } else if (parse_cmp("PTP", arg_string_p, 0)) {
            tod_type = bcmTimeFormatPTP;
        } else {
            return CMD_FAIL;
        }
        if (bstime_intf_info.bs_initialized) {
            _bcm_time_tod_dpll_chan_create(unit, tod_type);
        }
#endif
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif
