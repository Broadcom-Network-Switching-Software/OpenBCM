/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ptp.c
 * Purpose: IEEE1588 (PTP) Support
 */

#include <sal/core/dpc.h>
#include <sal/core/time.h>
#include <appl/diag/system.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <soc/drv.h>
#include <bcm/time.h>
#include <bcm/stack.h>

#if defined(UNIX) && !defined(__KERNEL__)
#include <time.h>
#include <stdlib.h>
#define PTP_SAL_HAS_TIME (1)
#endif

#if defined(INCLUDE_PTP)

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm_int/common/PTP_feature.h>
#include <appl/diag/bslenable.h>
#include <soc/iproc.h>
#include <bcm_int/common/time.h>

#ifdef BCM_ESMC_EXTDPLL_SUPPORT
#include <bcm_int/common/esmc.h>
#endif

#include <bcm/trunk.h>

#if defined(BCM_KSCPU_SUPPORT) && !defined(PTP_KEYSTONE_STACK)
#define PTP_KEYSTONE_STACK 
#endif

#define LOCAL_DEBUGBUFSIZE (2048)

static char local_debugbuf[LOCAL_DEBUGBUFSIZE];
static int local_head = 0;

static char output_debugbuf[LOCAL_DEBUGBUFSIZE * 2];
static int local_tail = 0;

bcm_tdpll_input_clock_ql_change_cb_data_t ql_change_data;

#define BCM_TOD_ETHERTYPE (0x5006)

#ifndef __KERNEL__

STATIC char *bcm_ptp_clock_accuracy_str[] = {
"bcmPTPClockAccuracy25ns",
"bcmPTPClockAccuracy100ns",
"bcmPTPClockAccuracy250ns",
"bcmPTPClockAccuracy1us",
"bcmPTPClockAccuracy2500ns",
"bcmPTPClockAccuracy10us",
"bcmPTPClockAccuracy25us",
"bcmPTPClockAccuracy100us",
"bcmPTPClockAccuracy250us",
"bcmPTPClockAccuracy1ms",
"bcmPTPClockAccuracy2500us",
"bcmPTPClockAccuracy10ms",
"bcmPTPClockAccuracy25ms",
"bcmPTPClockAccuracy100ms",
"bcmPTPClockAccuracy250ms",
"bcmPTPClockAccuracy1s",
"bcmPTPClockAccuracy10s",
"bcmPTPClockAccuracyL10s",
"bcmPTPClockAccuracyUnknown",
"bcmPTPClockAccuracyReserved",
};

typedef struct {
    int synced;
    sal_time_t host_time;
    bcm_ptp_timestamp_t ptp_time;
} diagshell_sync_t;

static diagshell_sync_t diagshell_sync;

STATIC void diag_arp_callback(int unit, bcm_ptp_stack_id_t stack_id, int protocol, int src_addr_offset, int payload_offset, int msg_len, uint8 *msg);

STATIC int diag_event_callback(
    int unit,
    bcm_ptp_stack_id_t stack_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_cb_type_t type,
    bcm_ptp_cb_msg_t *msg,
    void *user_data);

STATIC int diag_management_callback(
    int unit,
    bcm_ptp_stack_id_t stack_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_cb_type_t type,
    bcm_ptp_cb_msg_t *msg,
    void *user_data);

STATIC bcm_ptp_callback_t diag_signaling_arbiter(
    int unit,
    bcm_ptp_cb_signaling_arbiter_msg_t *amsg,
    void * flags);
#define ARBITER_DENY_FLAG (0x00000001)

STATIC int diag_ctdev_alarm_callback(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_ctdev_alarm_data_t *ctdev_alarm_data);

/* T-DPLL support. */
STATIC int diag_tdpll_input_clock_monitor_callback(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_cb_data_t *cb_data);

STATIC int diag_tdpll_input_clock_callback(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_cb_data_t *cb_data);

STATIC int diag_tdpll_input_clock_selector_callback(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_selector_cb_data_t *cb_data);

STATIC int diag_tdpll_input_clock_ql_change_callback(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_ql_change_cb_data_t *cb_data);

STATIC const char* diag_ieee1588_warn_reason_description(_bcm_ptp_ieee1588_warn_reason_t reason);
#ifdef BCM_PTP_EXT_SERVO_SUPPORT
STATIC const char* diag_servo_state_description(int state);
#else
STATIC const char* diag_servo_state_description(bcm_ptp_fll_state_t state);
#endif
STATIC const char* diag_servo_lock_status_description(bcm_ptp_servo_lock_status_t state);
STATIC const char* diag_pps_in_state_description(_bcm_ptp_pps_in_state_t state);

STATIC const char* diag_telecom_QL_description(
    const bcm_ptp_telecom_g8265_quality_level_t QL);
STATIC const char* diag_telecom_pktmaster_state_description(
    const bcm_ptp_telecom_g8265_pktmaster_state_t state);
STATIC void diag_telecom_pktmaster_printout(
    bcm_ptp_telecom_g8265_pktmaster_t *pktmaster, uint8 flags);

STATIC uint32 diag_debug_mask[BCM_MAX_NUM_UNITS][PTP_MAX_STACKS_PER_UNIT] = {{0}};
STATIC uint64 diag_log_mask[BCM_MAX_NUM_UNITS][PTP_MAX_STACKS_PER_UNIT] = {{COMPILER_64_INIT(0,0)}};

bcm_mac_t log_dst_mac_addr = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
bcm_mac_t log_src_mac_addr = {0x00, 0x10, 0x18, 0x00, 0x00, 0x01}; 
bcm_ip_t  log_dst_ip_addr = 0xffffffff;
bcm_ip_t  log_src_ip_addr = (192L << 24) + (168L << 16) +(0L << 8) + 90;
uint32  log_port = 0x4455;
uint32  log_tpid = 0x8100;
uint32  log_vid = 1;
uint32   log_ttl = 64;

extern void format_uint64_decimal(char *buf, uint64 n, int comma);
STATIC char *ptp_clock_accuracy_get(bcm_ptp_clock_accuracy_t *clock_accuracy, char *clock_accuracy_str);
#endif

STATIC const char* diag_port_state_description(_bcm_ptp_port_state_t state);
void ptp_print_addr(bcm_ptp_clock_port_address_t addr, char *addrstr);
void ptp_print_clockid(bcm_ptp_clock_identity_t id, char *idstr);


#if defined(BCM_TRIDENT2_SUPPORT)
int bcm_esw_ptp_map_input_synce_clock(int logical_l1_clk_port);
#endif

/*
 * Verbose diagnostic shell / CLI information.
 */
int verboseCLI = 0, verboseEvents = 2;

#define PTP_VERBOSE(msg)                                         \
    do {                                                         \
        if (verboseCLI) {                                        \
            cli_out("%s", msg);         \
        }                                                        \
    } while (0)

#define PTP_CLI_RESULT_RETURN(__cmd_result__)    \
    do {                                         \
      switch (__cmd_result__) {                  \
      case CMD_OK:                               \
          PTP_VERBOSE("PASSED\n");               \
          return __cmd_result__;                 \
      case CMD_FAIL:                             \
          PTP_VERBOSE("FAILED\n");               \
          return __cmd_result__;                 \
      case CMD_USAGE:                            \
          PTP_VERBOSE("INVALID SYNTAX\n");       \
          return __cmd_result__;                 \
      case CMD_NFND:                             \
          PTP_VERBOSE("NOT FOUND\n");            \
          return __cmd_result__;                 \
      case CMD_EXIT:                             \
          PTP_VERBOSE("EXIT SHELL\n");           \
          return __cmd_result__;                 \
      case CMD_INTR:                             \
            PTP_VERBOSE("INTERRUPTED\n");        \
            return __cmd_result__;               \
      case CMD_NOTIMPL:                          \
          PTP_VERBOSE("NOT IMPLEMENTED\n");      \
          return __cmd_result__;                 \
      default:                                   \
          PTP_VERBOSE("UNKNOWN\n");              \
          return CMD_FAIL;                       \
      }                                          \
  } while (0)

#define PTP_IF_ERROR_RETURN(op)                             \
    do {                                                    \
      int __rv__;                                           \
        if ((__rv__ = (op)) < 0) {                          \
            ptp_printf("Error: %s\n", bcm_errmsg(__rv__));  \
            PTP_CLI_RESULT_RETURN(CMD_FAIL);                \
        }                                                   \
  } while (0)

void bcm_ptp_debug_configure(int severity) {
    bslmgmt_set(BSL_L_BCM, BSL_S_PTP, severity);
}

void
ptp_printf(const char *fmt, ...)
{
    int ret;
    va_list args;
    char buf[512];
    int i;

    va_start(args, fmt);
    ret = sal_vsprintf(buf, fmt, args);

    for (i = 0; i < ret; ++i) {
        local_debugbuf[local_head++] = buf[i];
        if (local_head >= LOCAL_DEBUGBUFSIZE) local_head = 0;
    }

    va_end(args);
}

void
ptp_print_buf(const char *hdr, const uint8* buf, int len)
{
    while (len > 0) {
        int i;

        ptp_printf("%s:", hdr);
        for (i = 0; i < 16 && len > 0; ++i) {
            ptp_printf(" %02x", (unsigned) *buf);
            ++buf;
            --len;
        }
        ptp_printf("\n");
    }
}

/* Print clock port network address */
void
ptp_print_addr(bcm_ptp_clock_port_address_t addr, char *addrstr)
{
    int i;
    int num;
    char pstr[4] = {'\0'};

    addrstr[0] = '\0';
    switch (addr.addr_type) {
    case bcmPTPUDPIPv4:
        sal_sprintf(addrstr, "%d.%d.%d.%d",
            addr.address[0], addr.address[1], addr.address[2], addr.address[3]);
        num = 0;
        break;
    case bcmPTPUDPIPv6:
        num = 15;
        break;
    case bcmPTPIEEE8023:
        num = 5;
        break;
    default:
        num = 0;
        break;
    }

    if (0 == num) {
        return;
    }

    /* for L2 & IPv6, num > 0, so we print like this: */
    for (i = 0; i < num; ++i) {
        sal_sprintf(pstr, "%02x:", addr.address[i]);
        sal_strcat(addrstr, pstr);
    }
    sal_sprintf(pstr, "%02x", addr.address[num]);
    sal_strcat(addrstr, pstr);
}

/* Print clock ID */
void
ptp_print_clockid(bcm_ptp_clock_identity_t id, char *idstr)
{
    int i;
    char pstr[4] = {'\0'};

    idstr[0] = '\0';
    for (i = 0; i < 7; ++i) {
        sal_sprintf(pstr, "%02x:", id[i]);
        sal_strcat(idstr, pstr);
    }
    sal_sprintf(pstr, "%02x", id[7]);
    sal_strcat(idstr, pstr);
}

/* Mechanism to output information on a event/management callback.  bsl_printf / ptp_printf depending on preference */
#define ptp_cb_printf  ptp_printf

#define PTP_ERROR_CB(errmsg)      ptp_cb_printf("callback() FAILED %s\n", \
                                                errmsg)

#define PTP_ERROR_FUNC_CB(func)   ptp_cb_printf("callback() FAILED "    \
                                                func " returned %d : %s\n", \
                                                rv, bcm_errmsg(rv))

#define PTP_WARN_CB(warnmsg)      ptp_cb_printf("callback() failed %s\n", \
                                                warnmsg)

#define PTP_WARN_FUNC_CB(func)    ptp_cb_printf("callback() failed "    \
                                                func " returned %d : %s\n", \
                                                rv, bcm_errmsg(rv))

#define PTP_VERBOSE_CB(msg)                                      \
    do {                                                         \
        if (verboseCLI) {                                        \
            ptp_cb_printf("callback() ");                        \
            ptp_cb_printf msg;                                   \
        }                                                        \
    } while (0)


/* Dump anything new in the debug buffers.
 *  owner:         The owner if this is being called as a DPC.
 *  time_as_ptr:   Recurrence time if a follow-up DPC to this func should be scheduled.
 *  unit_as_ptr:   unit
 */
STATIC void
output_current_debug(void* owner, void* time_as_ptr, void *unit_as_ptr, void *unused_2, void* unused_3)
{
    int callback_time = (int)(size_t)time_as_ptr;
    int unit = (int)(size_t)unit_as_ptr;
    bcm_ptp_stack_id_t stack_id;

    uint32 head, size;
    int out_idx = 0;

    /* output the local debug first */
    while (local_tail != local_head) {
        char c = local_debugbuf[local_tail++];

        if (c) {
            output_debugbuf[out_idx++] = c;
        }

        if (local_tail == LOCAL_DEBUGBUFSIZE) {
            local_tail = 0;
        }
    }

    for (stack_id = 0; stack_id < PTP_MAX_STACKS_PER_UNIT; ++stack_id) {
        /* Ensure that PTP is up & initialized before proceeding */
        if (_bcm_common_ptp_unit_array[unit].memstate == PTP_MEMSTATE_INITIALIZED &&
            _bcm_common_ptp_unit_array[unit].stack_array[stack_id].memstate == PTP_MEMSTATE_INITIALIZED) {

            _bcm_ptp_info_t *ptp_info_p = &_bcm_common_ptp_info[unit];
            _bcm_ptp_stack_info_t *stack_p = &ptp_info_p->stack_info[stack_id];

            /* Katana-style shared-mem output */
            if (SOC_HAS_PTP_INTERNAL_STACK_SUPPORT(unit)) {
                /* head is written in externally, so will be in network byte order
                 * size is read externally, so is also network byte order
                 * tail is local, so we'll keep it in local endianness
                 */
                head = soc_htonl(stack_p->int_state.log->head);
                size = soc_htonl(stack_p->int_state.log->size);

                while (stack_p->int_state.log_tail != head) {
                    char c = stack_p->int_state.log->buf[stack_p->int_state.log_tail++];
                    if (c) {
                        output_debugbuf[out_idx++] = c;
                    }
                    if (stack_p->int_state.log_tail == size) {
                        stack_p->int_state.log_tail = 0;
                    }
                }
            } else {

#if defined(PTP_KEYSTONE_STACK)
                #define DEBUG_WINDOW_SIZE (1024)
                #define DEBUG_OFFSET_MASK (DEBUG_WINDOW_SIZE - 1)
                static uint32 tail[PTP_MAX_STACKS_PER_UNIT] = {0};
                const uint32 debug_head_idx_addr = 0x190005a8;
                const uint32 debug_window_addr = 0x190005ac;
                void *cookie = stack_p->ext_info.cookie;
                uint32 head;

                /* get the current head of the debug buffer (written by ToP) */
                stack_p->ext_info.read_fn(cookie, debug_head_idx_addr, &head);
                head &= DEBUG_OFFSET_MASK;

                /* walk our tail up to the current head */
                while (tail[stack_id] != head) {
                    uint32 wordval;
                    uint32 addr = debug_window_addr + tail[stack_id];
                    /* read a byte at "addr", by reading the word that the byte is in, then shifting */
                    stack_p->ext_info.read_fn(cookie, addr & 0xfffffffc, &wordval);
                    output_debugbuf[out_idx++] = wordval >> (24 - (addr & 3) * 8);
                    tail[stack_id] = ((tail[stack_id] + 1) & DEBUG_OFFSET_MASK);
                }
#endif

            }
        }

        if (out_idx) {
            output_debugbuf[out_idx] = 0;
            /* If UDP logging of debug is enabled, do not print logging on console */
#ifndef __KERNEL__
            if ((COMPILER_64_LO(diag_log_mask[unit][stack_id]) & 1) == 0) {
                cli_out("%s", output_debugbuf);
            }
#else
            cli_out("%s", output_debugbuf);
#endif
        }
    }

    sal_dpc_cancel(INT_TO_PTR(&output_current_debug));
    if (callback_time) {
        sal_dpc_time(callback_time, &output_current_debug, 0, time_as_ptr, unit_as_ptr, 0, 0);
    }
}


static cmd_result_t
ptp_show_clock(int unit, bcm_ptp_stack_id_t stack_id, int clock_num)
{
    bcm_ptp_clock_info_t        clock_info;
    bcm_ptp_default_dataset_t   def_data_set;
    bcm_ptp_current_dataset_t   cur_data_set;
    bcm_ptp_parent_dataset_t    par_data_set;
    bcm_ptp_time_properties_t   tp_data_set;
    cmd_result_t                rv;

    if (BCM_FAILURE(rv = bcm_ptp_clock_get(unit,0,clock_num,&clock_info))) {
        cli_out("\nbcm_ptp_clock_get() Error... (FCN RETURN= %d)\n", rv);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    /* retrieve clock datasets */
    if (BCM_FAILURE(rv = bcm_ptp_clock_default_dataset_get(unit, stack_id, clock_num, &def_data_set)))
    {
        cli_out("\nbcm_ptp_clock_default_dataset_get() Error... (FCN RETURN= %d)\n", rv);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    if (BCM_FAILURE(rv = bcm_ptp_clock_current_dataset_get(unit, stack_id, clock_num, &cur_data_set))) {
        cli_out("\nbcm_ptp_clock_current_dataset_get() Error... (FCN RETURN= %d)\n", rv);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    if (BCM_FAILURE(rv = bcm_ptp_clock_parent_dataset_get(unit, stack_id, clock_num, &par_data_set))) {
        cli_out("\nbcm_ptp_clock_parent_dataset_get() Error... (FCN RETURN= %d)\n", rv);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    if (BCM_FAILURE(rv = bcm_ptp_clock_time_properties_get(unit, stack_id, clock_num, &tp_data_set))) {
        cli_out("\nbcm_ptp_clock_time_properties_get() Error... (FCN RETURN= %d)\n", rv);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    if (BCM_FAILURE(rv = bcm_ptp_clock_get(unit, stack_id, clock_num, &clock_info))) {
        cli_out("\nbcm_ptp_clock_get() Error... (FCN RETURN= %d)\n", rv);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    cli_out("\nClock Instance DataSets\n\n");

    /* Default Dataset */
    cli_out("   defaultDS.twoStepFlag                                    : %s\n",
            def_data_set.flags & BCM_PTP_DATASET_TWOSTEP_ONLY ? "TRUE" : "FALSE");

    cli_out("   defaultDS.clockIdentity                                  : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
            def_data_set.clock_identity[0], def_data_set.clock_identity[1],
            def_data_set.clock_identity[2], def_data_set.clock_identity[3],
            def_data_set.clock_identity[4], def_data_set.clock_identity[5],
            def_data_set.clock_identity[6], def_data_set.clock_identity[7]);

    cli_out("   defaultDS.numberPorts                                    : %u\n", def_data_set.number_ports);

    cli_out("   defaultDS.clockQuality.clockClass                        : %u\n",
            def_data_set.clock_quality.clock_class);
    cli_out("   defaultDS.clockQuality.clockAccuracy                     : %u\n",
            def_data_set.clock_quality.clock_accuracy);
    cli_out("   defaultDS.clockQuality.offsetScaledLogVariance           : %u\n",
            def_data_set.clock_quality.offset_scaled_log_variance);

    cli_out("   defaultDS.priority1                                      : %u\n", def_data_set.priority1);
    cli_out("   defaultDS.priority2                                      : %u\n", def_data_set.priority2);
    cli_out("   defaultDS.localpriority                                  : %u\n", def_data_set.local_priority);
    cli_out("   defaultDS.maxStepsRemoved                                : %u\n", def_data_set.max_steps_removed);
    cli_out("   defaultDS.domainNumber                                   : %u\n", def_data_set.domain_number);

    cli_out("   defaultDS.slaveOnly                                      : %s\n",
            def_data_set.flags & BCM_PTP_DATASET_SLAVE_ONLY ? "TRUE" : "FALSE");

    cli_out("\n");

    /* Current Dataset */
    cli_out("   currentDS.stepsRemoved                                   : %u\n", cur_data_set.steps_removed);
#ifdef COMPILER_HAS_LONGLONG
    {
        int64 decval;
        int64 decfrac;

        decval =  (long long)cur_data_set.offset_from_master/(long long)PTP_CURRENT_DATASET_OFFSET_FROM_MASTER_SCALEFACTOR;
        decfrac = cur_data_set.offset_from_master - (decval * PTP_CURRENT_DATASET_OFFSET_FROM_MASTER_SCALEFACTOR);
        decfrac = (decfrac >= 0) ? decfrac : -decfrac;

        cli_out("   currentDS.offsetFromMaster                               : %lld (scaled), %lld.%lld (unscaled ns)\n",
                    (long long)cur_data_set.offset_from_master, (long long)decval, (long long)decfrac);
    }
    cli_out("   currentDS.meanPathDelay                                  : %lld\n", (long long)cur_data_set.mean_path_delay);
#endif

    cli_out("\n");

    /* Parent Dataset */

    cli_out("   parentDS.parentPortIdentity.clockIdentity                : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
            par_data_set.parent_port_identity.clock_identity[0],
            par_data_set.parent_port_identity.clock_identity[1],
            par_data_set.parent_port_identity.clock_identity[2],
            par_data_set.parent_port_identity.clock_identity[3],
            par_data_set.parent_port_identity.clock_identity[4],
            par_data_set.parent_port_identity.clock_identity[5],
            par_data_set.parent_port_identity.clock_identity[6],
            par_data_set.parent_port_identity.clock_identity[7]);

    cli_out("   parentDS.parentPortIdentity.portNumber                   : %u\n",
            par_data_set.parent_port_identity.port_number);
    cli_out("   parentDS.parentStats                                     : %s\n",
            par_data_set.ps & 0x01 ? "TRUE" : "FALSE");
    cli_out("   parentDS.observedParentOffsetScaledLogVariance           : %u\n",
            par_data_set.observed_parent_offset_scaled_log_variance);
    cli_out("   parentDS.observedParentClockPhaseChangeRate              : %u\n",
            par_data_set.observed_parent_clock_phase_change_rate);

    cli_out("   parentDS.grandmasterIdentity                             : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
            par_data_set.grandmaster_identity[0], par_data_set.grandmaster_identity[1],
            par_data_set.grandmaster_identity[2], par_data_set.grandmaster_identity[3],
            par_data_set.grandmaster_identity[4], par_data_set.grandmaster_identity[5],
            par_data_set.grandmaster_identity[6], par_data_set.grandmaster_identity[7]);

    cli_out("   parentDS.grandmasterClockQuality.clockClass              : %u\n",
            par_data_set.grandmaster_clock_quality.clock_class);
    cli_out("   parentDS.grandmasterClockQuality.clockAccuracy           : %u\n",
            par_data_set.grandmaster_clock_quality.clock_accuracy);
    cli_out("   parentDS.grandmasterClockQuality.offsetScaledLogVariance : %u\n",
            par_data_set.grandmaster_clock_quality.offset_scaled_log_variance);

    cli_out("   parentDS.grandmasterPriority1                            : %u\n",
            par_data_set.grandmaster_priority1);
    cli_out("   parentDS.grandmasterPriority2                            : %u\n",
            par_data_set.grandmaster_priority2);

    cli_out("\n");

    /* Time Properties Dataset */

    cli_out("   timePropertiesDS.currentUtcOffset                        : %d\n",
            tp_data_set.utc_info.utc_offset);
    cli_out("   timePropertiesDS.currentUtcOffsetValid                   : %s\n",
            tp_data_set.utc_info.utc_valid ? "TRUE" : "FALSE");
    cli_out("   timePropertiesDS.leap59                                  : %s\n",
            tp_data_set.utc_info.leap59 ? "TRUE" : "FALSE");
    cli_out("   timePropertiesDS.leap61                                  : %s\n",
            tp_data_set.utc_info.leap61 ? "TRUE" : "FALSE");
    cli_out("   timePropertiesDS.timeTraceable                           : %s\n",
            tp_data_set.trace_info.time_traceable ? "TRUE" : "FALSE");
    cli_out("   timePropertiesDS.frequencyTraceable                      : %s\n",
            tp_data_set.trace_info.frequency_traceable ? "TRUE" : "FALSE");
    cli_out("   timePropertiesDS.ptpTimescale                            : %s\n",
            tp_data_set.timescale_info.ptp_timescale ? "TRUE" : "FALSE");
    cli_out("   timeProperitesDS.timeSource                              : 0x%02x\n",
            tp_data_set.timescale_info.time_source);

    cli_out("   timeProperitesDS.masterSyncUncertain                     : %s\n",
            (tp_data_set.ptp_pkt_flags & (1<<BCM_PTP_PKT_FLAG_SYNC_UNCERTAIN))
            ? "TRUE" :  "FALSE");

    cli_out("\nClock Instance Parameters\n\n");

    cli_out("   Clock Num                           : %u\n", clock_info.clock_num);

    cli_out("   Clock Identity                      : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
            clock_info.clock_identity[0],
            clock_info.clock_identity[1],
            clock_info.clock_identity[2],
            clock_info.clock_identity[3],
            clock_info.clock_identity[4],
            clock_info.clock_identity[5],
            clock_info.clock_identity[6],
            clock_info.clock_identity[7]);

    cli_out("   Clock Type                          : %u", clock_info.type);
    if (clock_info.type== bcmPTPClockTypeOrdinary) {
        cli_out(" (Ordinary)\n");
    } else if (clock_info.type== bcmPTPClockTypeBoundary){
        cli_out(" (Boundary)\n");
    } else if (clock_info.type== bcmPTPClockTypeTransparent){
        cli_out(" (Transparent)\n");
    } else {
        cli_out(" (Unknown)\n");
    }

    cli_out("\n");
    cli_out("   TC Primary Domain                   : %u\n", clock_info.tc_primary_domain);
    cli_out("   TC Delay Mechanism                  : %u\n", clock_info.tc_delay_mechanism);
    cli_out("\n");
    cli_out("   Announce Receipt Timeout (MINIMUM)  : %u\n", clock_info.announce_receipt_timeout_minimum);
    cli_out("   Announce Receipt Timeout (DEFAULT)  : %u\n", clock_info.announce_receipt_timeout_default);
    cli_out("   Announce Receipt Timeout (MAXIMUM)  : %u\n", clock_info.announce_receipt_timeout_maximum);
    cli_out("\n");
    cli_out("   Log Announce Interval (MINIMUM)     : %d\n", clock_info.log_announce_interval_minimum);
    cli_out("   Log Announce Interval (DEFAULT)     : %d\n", clock_info.log_announce_interval_default);
    cli_out("   Log Announce Interval (MAXIMUM)     : %d\n", clock_info.log_announce_interval_maximum);
    cli_out("\n");
    cli_out("   Log Sync Interval (MINIMUM)         : %d\n", clock_info.log_sync_interval_minimum);
    cli_out("   Log Sync Interval (DEFAULT)         : %d\n", clock_info.log_sync_interval_default);
    cli_out("   Log Sync Interval (MAXIMUM)         : %d\n", clock_info.log_sync_interval_maximum);
    cli_out("\n");
    cli_out("   Log Min Delay Req Interval (MINIMUM): %d\n", clock_info.log_min_delay_req_interval_minimum);
    cli_out("   Log Min Delay Req Interval (DEFAULT): %d\n", clock_info.log_min_delay_req_interval_default);
    cli_out("   Log Min Delay Req Interval (MAXIMUM): %d\n", clock_info.log_min_delay_req_interval_maximum);
    cli_out("\n");
    cli_out("   Domain # (MINIMUM)                  : %u\n", clock_info.domain_number_minimum);
    cli_out("   Domain # (DEFAULT)                  : %u\n", clock_info.domain_number_default);
    cli_out("   Domain # (MAXIMUM)                  : %u\n", clock_info.domain_number_maximum);
    cli_out("\n");
    cli_out("   Priority 1 (MINIMUM)                : %u\n", clock_info.priority1_minimum);
    cli_out("   Priority 1 (DEFAULT)                : %u\n", clock_info.priority1_default);
    cli_out("   Priority 1 (MAXIMUM)                : %u\n", clock_info.priority1_maximum);
    cli_out("\n");
    cli_out("   Priority 2 (MINIMUM)                : %u\n", clock_info.priority2_minimum);
    cli_out("   Priority 2 (DEFAULT)                : %u\n", clock_info.priority2_default);
    cli_out("   Priority 2 (MAXIMUM)                : %u\n", clock_info.priority2_maximum);
    cli_out("\n");
    cli_out("   Flags                               : 0x%08x\n", clock_info.flags);
    cli_out("\n");
    cli_out("   LocalPriority                       : %u\n", clock_info.local_priority);
    cli_out("\n");
    
    /* cli_out("   # Virtual Interfaces                : %u\n", clock_info.number_virtual_interfaces); */
    PTP_CLI_RESULT_RETURN(CMD_OK);
}

STATIC const char*
diag_tod_format_description(
   bcm_ptp_tod_format_t tod_fmt )
{
    switch (tod_fmt) {
    case bcmPTPTODFormatString:
        return "bcmPTPTODFormatString";

    case bcmPTPTODFormatUBlox:
        return "bcmPTPTODFormatUBlox";

    case bcmPTPTODFormatChinaTcom:
        return "bcmPTPTODFormatChinaTcom";

    case bcmPTPTODFormatBCM:
        return "bcmPTPTODFormatBCM";

    case bcmPTPTODFormatBCMTS:
        return "bcmPTPTODFormatBCMTS";

    case bcmPTPTODFormatG8271:
        return "bcmPTPTODFormatG8271";

    default:
        return "<Unknown>";
    }
}

STATIC cmd_result_t
ptp_show_port(int unit, bcm_ptp_stack_id_t stack_id, int clock_num, int port_number)
{
    bcm_ptp_port_dataset_t port_data_set;
    bcm_ptp_clock_port_info_t port_data_api;
#ifndef __KERNEL__
    _bcm_ptp_clock_description_t description;
#endif
    cmd_result_t rv;
    char uint64_decimal_string[27];

    /* Retrieve comprehensive port configuration information to use in report. */
    if (BCM_FAILURE(rv = bcm_ptp_clock_port_info_get(unit, stack_id, clock_num,
                                    port_number, &port_data_api))) {
        cli_out("\nbcm_ptp_clock_port_info_get() Error... (FCN RETURN= %d)\n", rv);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    if (BCM_FAILURE(rv = bcm_ptp_clock_port_dataset_get(unit, stack_id, clock_num, port_number, &port_data_set))) {
        cli_out("\nbcm_ptp_clock_port_dataset_get(%d, %d, %d, %d, &dataset) Error... (FCN RETURN= %d)\n",
                unit, stack_id, clock_num, port_number, rv);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    cli_out("\nClock Port %d DataSets\n\n", port_number);

    cli_out("   portDS.portIdentity                : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
            port_data_set.port_identity.clock_identity[0],
            port_data_set.port_identity.clock_identity[1],
            port_data_set.port_identity.clock_identity[2],
            port_data_set.port_identity.clock_identity[3],
            port_data_set.port_identity.clock_identity[4],
            port_data_set.port_identity.clock_identity[5],
            port_data_set.port_identity.clock_identity[6],
            port_data_set.port_identity.clock_identity[7],
            port_data_set.port_identity.port_number);

    cli_out("   portDS.portState                   : %s\n", diag_port_state_description(port_data_set.port_state));
    cli_out("   portDS.logMinDelayReqInterval      : %d\n", port_data_set.log_min_delay_req_interval);
    format_uint64_decimal(uint64_decimal_string, port_data_set.peer_mean_path_delay, ',');
    cli_out("   portDS.peerMeanPathDelay           : %s\n", uint64_decimal_string);
    cli_out("   portDS.logAnnounceInterval         : %d\n", port_data_set.log_announce_interval);
    cli_out("   portDS.announceReceiptTimeout      : %d\n", port_data_set.announce_receipt_timeout);
    cli_out("   portDS.logSyncInterval             : %d\n", port_data_set.log_sync_interval);
    cli_out("   portDS.delayMechanism              : %u", port_data_set.delay_mechanism);
    if (port_data_set.delay_mechanism == bcmPTPDelayMechanismEnd2End) {
        cli_out(" (End-to-End)\n");
    } else if (port_data_set.delay_mechanism == bcmPTPDelayMechanismPeer2Peer) {
        cli_out(" (Peer-to-Peer)\n");
    } else if (port_data_set.delay_mechanism == bcmPTPDelayMechansimDisabled) {
        cli_out(" (Disabled)\n");
    } else {
        cli_out(" (Unknown)\n");
    }
    cli_out("   portDS.logMinPDelayReqInterval     : %d\n", port_data_set.log_min_pdelay_req_interval);
    cli_out("   portDS.versionNumber               : %d\n", port_data_set.version_number);
    cli_out("   portDS.localPriority               : %d\n", port_data_set.local_priority);
    cli_out("   portDS.notSlave                    : %s\n", port_data_set.not_slave ? "TRUE" : "FALSE");
    cli_out("   portDS.SF                          : %s\n", (port_data_set.signal_fail>0) ? "TRUE" : "FALSE");

    cli_out("\nClock Port %d Parameters\n\n", port_number);

    cli_out("   Port Address Protocol              : %u",port_data_api.port_address.addr_type);
    if (port_data_api.port_address.addr_type == bcmPTPIEEE8023) {
        cli_out(" (Ethernet Layer 2)\n");
    } else if (port_data_api.port_address.addr_type == bcmPTPUDPIPv4) {
        cli_out(" (Ethernet/UDP/IPv4)\n");
    } else if (port_data_api.port_address.addr_type == bcmPTPUDPIPv6) {
        cli_out(" (Ethernet/UDP/IPv6)\n");
    } else {
        cli_out(" (Unknown)\n");
    }

    if (port_data_api.port_address.addr_type == bcmPTPUDPIPv4) {
        cli_out("   Port IP Address (IPv4)             : %u.%u.%u.%u\n",
                port_data_api.port_address.address[0], port_data_api.port_address.address[1],
                port_data_api.port_address.address[2], port_data_api.port_address.address[3]);
    } else if (port_data_api.port_address.addr_type == bcmPTPUDPIPv6) {
        cli_out("   Port IP Address (IPv6)             : %02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                "%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
                port_data_api.port_address.address[0],  port_data_api.port_address.address[1],
                port_data_api.port_address.address[2],  port_data_api.port_address.address[3],
                port_data_api.port_address.address[4],  port_data_api.port_address.address[5],
                port_data_api.port_address.address[6],  port_data_api.port_address.address[7],
                port_data_api.port_address.address[8],  port_data_api.port_address.address[9],
                port_data_api.port_address.address[10], port_data_api.port_address.address[11],
                port_data_api.port_address.address[12], port_data_api.port_address.address[13],
                port_data_api.port_address.address[14], port_data_api.port_address.address[15]);
    }

    cli_out("   Port MAC Address                   : %02x:%02x:%02x:%02x:%02x:%02x\n",
            port_data_api.mac[0], port_data_api.mac[1], port_data_api.mac[2],
            port_data_api.mac[3], port_data_api.mac[4], port_data_api.mac[5]);

    if (port_data_api.multicast_tx_enable) {
        cli_out("   Multicast L2 Header                : |%02x:%02x:%02x:%02x:%02x:%02x|%02x:"
                "%02x:%02x:%02x:%02x:%02x|%02x:%02x:%02x:%02x|%02x:%02x|\n",
                port_data_api.multicast_l2[0],  port_data_api.multicast_l2[1],
                port_data_api.multicast_l2[2],  port_data_api.multicast_l2[3],
                port_data_api.multicast_l2[4],  port_data_api.multicast_l2[5],
                port_data_api.multicast_l2[6],  port_data_api.multicast_l2[7],
                port_data_api.multicast_l2[8],  port_data_api.multicast_l2[9],
                port_data_api.multicast_l2[10], port_data_api.multicast_l2[11],
                port_data_api.multicast_l2[12], port_data_api.multicast_l2[13],
                port_data_api.multicast_l2[14], port_data_api.multicast_l2[15],
                port_data_api.multicast_l2[16], port_data_api.multicast_l2[17]);
        cli_out("   Multicast Pdelay L2 Header         : |%02x:%02x:%02x:%02x:%02x:%02x|%02x:"
                "%02x:%02x:%02x:%02x:%02x|%02x:%02x:%02x:%02x|%02x:%02x|\n",
                port_data_api.multicast_pdelay_l2[0],  port_data_api.multicast_pdelay_l2[1],
                port_data_api.multicast_pdelay_l2[2],  port_data_api.multicast_pdelay_l2[3],
                port_data_api.multicast_pdelay_l2[4],  port_data_api.multicast_pdelay_l2[5],
                port_data_api.multicast_pdelay_l2[6],  port_data_api.multicast_pdelay_l2[7],
                port_data_api.multicast_pdelay_l2[8],  port_data_api.multicast_pdelay_l2[9],
                port_data_api.multicast_pdelay_l2[10], port_data_api.multicast_pdelay_l2[11],
                port_data_api.multicast_pdelay_l2[12], port_data_api.multicast_pdelay_l2[13],
                port_data_api.multicast_pdelay_l2[14], port_data_api.multicast_pdelay_l2[15],
                port_data_api.multicast_pdelay_l2[16], port_data_api.multicast_pdelay_l2[17]);
    } else {
        cli_out("   Multicast Disabled\n");
    }

    cli_out("   Port Type                          : %u", port_data_api.port_type);
    if (port_data_api.port_type == bcmPTPPortTypeStandard) {
        cli_out(" (Standard)\n");
    } else if (port_data_api.port_type == bcmPTPPortTypeMasterOnly) {
        cli_out(" (Master-only)\n");
    } else if (port_data_api.port_type == bcmPTPPortTypeSlaveOnly) {
        cli_out(" (Slave-only)\n");
    } else if (port_data_api.port_type == bcmPTPPortTypeVirtual) {
        cli_out(" (Virtual)\n");
    } else if (port_data_api.port_type == bcmPTPPortTypeCustomizedVirtual) {
        cli_out(" (Customized Virtual)\n");
    } else {
        cli_out(" (Unknown)\n");
    }

    cli_out("   Rx Timestamp Mechanism             : 0x%02x", port_data_api.rx_timestamp_mechanism);
    if (port_data_api.rx_timestamp_mechanism == bcmPTPToPTimestamps) {
        cli_out(" (ToP)\n");
    } else if (port_data_api.rx_timestamp_mechanism == bcmPTPRCPUTimestamps) {
        cli_out(" (RCPU)\n");
    } else if (port_data_api.rx_timestamp_mechanism == bcmPTPPhyCorrectionTimestamps) {
        cli_out(" (PHY Correction)\n");
    } else if (port_data_api.rx_timestamp_mechanism == bcmPTPMac32CorrectionTimestamps) {
        cli_out(" (Mac32 Correction)\n");
    } else if (port_data_api.rx_timestamp_mechanism == bcmPTPMac48CorrectionTimestamps) {
        cli_out(" (Mac48 Correction)\n");
    } else {
        cli_out(" (Unknown)\n");
    }

    cli_out("   Rx Packets VLAN                    : 0x%04x\n", port_data_api.rx_packets_vlan);
    cli_out("   Rx Packets Port Mask (High 32 Bits): 0x%08x\n", port_data_api.rx_packets_port_mask_high32);
    cli_out("   Rx Packets Port Mask (Low 32 Bits) : 0x%08x\n", port_data_api.rx_packets_port_mask_low32);
    cli_out("   Rx Packets Criteria Mask           : 0x%02x\n", port_data_api.rx_packets_criteria_mask);
    cli_out("   localPriority                      : %d\n", port_data_api.local_priority);
    
    if ((port_data_api.port_address.addr_type == bcmPTPUDPIPv4) ||
       (port_data_api.port_address.addr_type == bcmPTPUDPIPv6)) {
        cli_out("   TTL                                : %d\n", port_data_api.ttl);
        cli_out("   DSCP                               : %d\n", port_data_api.ip_dscp);
    }

    if (port_data_api.port_type == bcmPTPPortTypeVirtual ||
		port_data_api.port_type == bcmPTPPortTypeCustomizedVirtual) {
        cli_out("   Virtual port configuration\n");
        cli_out("      Clock class                      : %d\n", port_data_api.vport_info.clock_class);
        cli_out("      Clock accuracy                   : %d\n", port_data_api.vport_info.clock_accuracy);
        cli_out("      OffsetScaledLogVariance          : %d\n", port_data_api.vport_info.offset_scaled_log_variance);
        cli_out("      Steps Removed                    : %d\n", port_data_api.vport_info.steps_removed);
        cli_out("      GM priority2                     : %d\n", port_data_api.vport_info.GM_prio2);
        cli_out("      TOD offset sec                   : %d\n", port_data_api.vport_info.tod_offset_sec);
        cli_out("      TOD offset nsec                  : %d\n", port_data_api.vport_info.tod_offset_ns);
        cli_out("      TOD format                       : %s\n", diag_tod_format_description(port_data_api.vport_info.format));
        if (bcmPTPTODFormatString == port_data_api.vport_info.format) {
            cli_out("      TOD format string                : %s\n", port_data_api.vport_info.format_str);
            cli_out("      TOD mask string                  : %s\n", port_data_api.vport_info.mask_str);
        }
        cli_out("      TOD Ether type                   : 0x%04x\n", port_data_api.vport_info.ether_type);
        cli_out("      1PPS GPIO#                       : %d\n", port_data_api.vport_info.onepps_in_gpio);
        if(PTP_UC_FEATURE_CHECK(PTP_VP_TOD_IN_SERIAL)) {
            if (port_data_api.vport_info.tod_source == bcmPTPTODSourceNone) {
                cli_out("      TOD source#                      : %s\n","None");
            } else if (port_data_api.vport_info.tod_source == bcmPTPTODSourceEthernet)
                cli_out("      TOD source#                      : %s\n","Ethernet");
            else {
                if (port_data_api.vport_info.tod_source == bcmPTPTODSourceSerial)
                    cli_out("      TOD source#                      : %s\n","UartNum0");
                else if (port_data_api.vport_info.tod_source == bcmPTPTODSourceSerial1)
                    cli_out("      TOD source#                      : %s\n","UartNum1");
                else if (port_data_api.vport_info.tod_source == bcmPTPTODSourceSerial2)
                    cli_out("      TOD source#                      : %s\n","UartNum2");
                else if (port_data_api.vport_info.tod_source == bcmPTPTODSourceSerial3)
                    cli_out("      TOD source#                      : %s\n","UartNum3");
                cli_out("      Baud Rate#                       : %d\n", port_data_api.vport_info.tod_baud_rate);
                if(PTP_UC_FEATURE_CHECK(PTP_TOD_UART_PARITY)) {
                    cli_out("      UART Parity#                     : %d\n", port_data_api.vport_info.tod_parity);
                }
            }
        }
        if(PTP_UC_FEATURE_CHECK(PTP_TOD_TS_SYNC_ENABLE)) {
            cli_out("      G.8271 timestamper sync#         : %d\n", port_data_api.vport_info.ts_sync_enable);
        }
    }
#ifndef __KERNEL__
    if (BCM_FAILURE(rv = _bcm_ptp_clock_description_get(unit, stack_id, clock_num,
                                    port_number, &description))) {
        cli_out("\n_bcm_ptp_clock_description_get() Error... (FCN RETURN= %d)\n", rv);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    cli_out("\nClock Description\n");
    _bcm_ptp_dump_hex(description.data, description.size, 3);
    sal_free(description.data);
    cli_out("\n");
#endif
    PTP_CLI_RESULT_RETURN(CMD_OK);
}


STATIC cmd_result_t
ptp_show_peers(int unit, bcm_ptp_stack_id_t stack_id, int clock_num)
{
    PTP_CLI_RESULT_RETURN(CMD_OK);
}


#ifndef __KERNEL__
STATIC char *ptp_clock_accuracy_get(bcm_ptp_clock_accuracy_t *clock_accuracy, char *clock_accuracy_str)
{
    int len = 0;
    if (*clock_accuracy >= bcmPTPClockAccuracy25ns &&
        *clock_accuracy <= bcmPTPClockAccuracyL10s) {
            len = sal_strlen(bcm_ptp_clock_accuracy_str[(int)(*clock_accuracy- bcmPTPClockAccuracy25ns)]);
            sal_strncpy(clock_accuracy_str, 
                    bcm_ptp_clock_accuracy_str[(int)(*clock_accuracy 
                    - bcmPTPClockAccuracy25ns)], len);
        if (len)
            clock_accuracy_str[len] = '\0';
    }
    else {
        len = sal_strlen("bcmPTPClockAccuracyUnknown/bcmPTPClockAccuracyReserved");
        sal_strncpy(clock_accuracy_str, 
                "bcmPTPClockAccuracyUnknown/bcmPTPClockAccuracyReserved", len);
        if (len)
            clock_accuracy_str[len] = '\0';
    }
    return (clock_accuracy_str);
}

/*********************************************************************
* FUNCTION:
*     Parse Logical System Port used internally to indicate
*     either lag index and lag member index, or System Physical Port.
*     The Logical System Port structure is defined as following:
*     +-------------------------------------------------------------------------+
*     | Field              Size | Bit/s | Meaning                               |
*     +-------------------------------------------------------------------------+
*     | IS_LAG              1   |  15   | Indicates that the port is a LAG port |
*     +-------------------------------------------------------------------------+
*     | If (IS_LAG)             |       |                                       |
*     +-------------------------------------------------------------------------+
*     | Reserved            1   |  14   | Indicates that the port is a LAG port |
*     +-------------------------------------------------------------------------+
*     | LAG_PORT_MEMBER_ID 14-N | 13:N  | Identify a member physical port       |
*     |                         |       | (a LAG Port unique value)             |
*     +-------------------------------------------------------------------------+
*     | LAG_PORT_ID         N   | N-1:0 | The system-level LAG ID               |
*     +-------------------------------------------------------------------------+
*     | Else                    |       |                                       |
*     +-------------------------------------------------------------------------+
*     | PHYSICAL_PORT_ID    15  | 14:0  | System-level unique physical port ID  |
*     +-------------------------------------------------------------------------+
*     | End if                  |       |                                       |
*     +-------------------------------------------------------------------------+
*
* Reference:
*     arad_ports_logical_sys_id_parse
 */

STATIC int
ptp_parse_sysport_to_modport(int unit, uint32 sys_port_aggr, int *trunk_id, int *mod_id, int *port)
{
    int rv = BCM_E_NONE;

#if defined(BCM_PETRA_SUPPORT)
    uint32 reg32_val;
    int lag_mode =0, nof_lag_id_bits = 0, trunk_id_mask = 0;
    bcm_gport_t gport_sysport = 0;
    bcm_gport_t gport_modport = 0;
#endif

    *trunk_id = -1;
    *mod_id = 0;
    *port = 0;

    if (!sys_port_aggr)return (rv);

#if defined(BCM_PETRA_SUPPORT)

#define IS_LAG_BIT_POS  15
    /* first check if incoming FTMH.system_port_aggregate is lag */
    if (sys_port_aggr & (1<<IS_LAG_BIT_POS)) {
      /* lag case */
      READ_ECI_GLOBAL_GENERAL_CFG_3r(unit, &reg32_val);
      lag_mode = soc_reg_field_get(unit, ECI_GLOBAL_GENERAL_CFG_3r, reg32_val, LAG_MODEf);
      /* maximum 10 bits for lag id */
      nof_lag_id_bits =  10 - lag_mode;

      /* lag id comprises of bit pos 0..(nof_lag_id_bits - 1)*/
      trunk_id_mask = (1<<nof_lag_id_bits) - 1;

      *trunk_id = sys_port_aggr & trunk_id_mask;

       /** Map SPA to its corresponding sysport gport */
       bcm_trunk_spa_to_system_phys_port_map_get(unit, 0, sys_port_aggr, &gport_sysport);

    } else {
        *trunk_id = -1;
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport_sysport, sys_port_aggr);
    }

    /** Get modport gport from sysport gport */
    if(bcm_stk_sysport_gport_get(unit,gport_sysport,&gport_modport) != BCM_E_NONE){
       return (rv);
    }

    *mod_id  = BCM_GPORT_MODPORT_MODID_GET(gport_modport);
    *port = BCM_GPORT_MODPORT_PORT_GET(gport_modport);
#endif

    return rv;
}

STATIC cmd_result_t
ptp_show_foreign_master_dataset(int unit, bcm_ptp_stack_id_t stack_id, int clock_num, int port_num)
{
    bcm_ptp_foreign_master_dataset_t dataset;
    unsigned index;
    char clock_accuracy_str[64];

    char idstr[32] = {'\0'};
    char addrstr[64] = {'\0'};

    BCM_IF_ERROR_RETURN(
        bcm_ptp_foreign_master_dataset_get(unit, stack_id, clock_num, port_num, &dataset));

    cli_out("Number of Foreign Masters: %d\n", dataset.num_foreign_masters);
    if (dataset.num_foreign_masters) {
        cli_out("Current Foreign Master   : %d\n", dataset.current_best_master);
    } else {
        cli_out("Current Foreign Master   :\n");
    }
    for (index = 0; index < dataset.num_foreign_masters; index++) {
        cli_out("Foreign Master #%d\n", index);

        ptp_print_clockid(
            dataset.foreign_master[index].port_identity.clock_identity, idstr);
        cli_out("    Port ID   : %s:%04x\n", idstr,
                dataset.foreign_master[index].port_identity.port_number);

        ptp_print_addr(dataset.foreign_master[index].address, addrstr);
        cli_out("    Address   : %s\n", addrstr);

        cli_out("    clockClass: %u\n", dataset.foreign_master[index].clockClass);
        cli_out("    priority1 : %u\n", dataset.foreign_master[index].grandmasterPriority1);
        cli_out("    priority2 : %u\n", dataset.foreign_master[index].grandmasterPriority2);

        {
            int syncSec = dataset.foreign_master[index].ms_since_sync / 1000;
            int syncMS = dataset.foreign_master[index].ms_since_sync - 1000 * syncSec;

            int syncTSSec = dataset.foreign_master[index].ms_since_sync_ts / 1000;
            int syncTSMS = dataset.foreign_master[index].ms_since_sync_ts - 1000 * syncTSSec;

            int delRespSec = dataset.foreign_master[index].ms_since_del_resp / 1000;
            int delRespMS = dataset.foreign_master[index].ms_since_del_resp - 1000 * delRespSec;

            if (dataset.foreign_master[index].ms_since_del_resp == ((uint32)-1)) {
                /*
                 * One-way operation.
                 * Delay request and delay response messages are not exchanged.
                 * Firmware overwrites time since last delayResp with all-ones.
                 */
                cli_out("    Time since:   Sync: %d.%03d     Sync/Followup: %d.%03d    DelayResp: N/A\n",
                        syncSec, syncMS, syncTSSec, syncTSMS);
            } else {
                cli_out("    Time since:   Sync: %d.%03d     Sync/Followup: %d.%03d    DelayResp: %d.%03d sec\n",
                        syncSec, syncMS, syncTSSec, syncTSMS, delRespSec, delRespMS);
            }

            cli_out("    Announces : %u (per FOREIGN_MASTER_TIME_WINDOW)\n",
                    dataset.foreign_master[index].announce_messages);

#ifdef COMPILER_HAS_LONGLONG
            if (COMPILER_64_LO(dataset.foreign_master[index].pdv_scaled_allan_var) == ((uint32)-1) &&
                COMPILER_64_HI(dataset.foreign_master[index].pdv_scaled_allan_var) == ((uint32)-1)) {
                cli_out("    MTSD Scaled AVAR: N/A\n");
            } else {
                cli_out("    MTSD Scaled AVAR: %llu (nsec-sq)\n",
                        (long long unsigned)dataset.foreign_master[index].pdv_scaled_allan_var);
            }
#endif /* COMPILER_HAS_LONGLONG */

        }

        cli_out("    Offset scaled log variance: %u\n", 
                dataset.foreign_master[index].offset_scaled_log_variance);
        cli_out("    Clock accuracy: 0x%02x %s\n", 
                dataset.foreign_master[index].clock_accuracy,
                ptp_clock_accuracy_get(&(dataset.foreign_master[index].clock_accuracy), clock_accuracy_str));
        cli_out("    Steps removed: %u\n", dataset.foreign_master[index].steps_removed);
        cli_out("    Grandmaster clockIdentity: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
            dataset.foreign_master[index].grandmaster_identity[0], dataset.foreign_master[index].grandmaster_identity[1],
            dataset.foreign_master[index].grandmaster_identity[2], dataset.foreign_master[index].grandmaster_identity[3],
            dataset.foreign_master[index].grandmaster_identity[4], dataset.foreign_master[index].grandmaster_identity[5],
            dataset.foreign_master[index].grandmaster_identity[6], dataset.foreign_master[index].grandmaster_identity[7]);

        {
            int port = dataset.foreign_master[index].phy_port;
            if (SOC_IS_QUX(unit) || SOC_IS_QAX(unit) || SOC_IS_QMX(unit)) {
                int trunk_id=0, mod_id=0, port_id=0;
                ptp_parse_sysport_to_modport(unit, port, &trunk_id, &mod_id, &port_id);
                cli_out("    LagId Number: %d\n", trunk_id);
                cli_out("    Module Number: %u\n", mod_id);
                cli_out("    Physical Port number: %u\n", port_id);
            } else {
                cli_out("    Module Number: %u\n",
                            BCM_GPORT_MODPORT_MODID_GET(port));
                cli_out("    Physical Port number: %u\n",
                            BCM_GPORT_MODPORT_PORT_GET(port));
            }
            cli_out("    Port number: %u\n", port_num);
        }
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}
#endif

static int cur_clock_num = 0;   /* for now: per-clock operations happen on the last created clock */
static int cur_stack_id = 0;    /*  and per-stack operations on the last created stack            */
static int cur_port_num  = 1;

typedef struct {
    char *str;
    cmd_result_t (*func)(int unit, args_t *args);
    const char* help;
    const char* help_ext;  /* To prevent strings > 255 chars */
} subcommand_t;

static subcommand_t *subcommands_list;  /* Null terminated list */

/* PTP Stack */
#define PTP_STACK_USAGE \
    "PTP Stack <subcommand>\n" \
    "\t PTP Stack Create TS0TS1CombinedModeEn=<0/1>  Creates a PTP Stack\n" \
    "\t PTP Stack Perfomance           Prints ToP CPU and Memory Info to debug\n"

STATIC cmd_result_t
cmd_ptp_stack(int unit, args_t *a)
{
    int rv;
    const char * arg;
    parse_table_t pt;
    int arg_ts0_ts1_combined_mode_en= 0;
    bcm_ptp_stack_info_t stack_info;

    arg = ARG_GET(a);
    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }
    if (parse_cmp("Create", arg, 0)) {
#if defined(PTP_KEYSTONE_STACK)
        bcm_ptp_external_stack_info_t external = {
            0x0002, 0,
            0x0800,0x3150,0x8100,
            3,0,0x5001,
            {0xaa,0xbb,0xcc,0xdd,0xee,0xff},
            {0x00,0x1b,0xe9,0x58,0xb0,0x6d},
            {0x10,0x04,0x9f,0x00,0x01,0x01},
            (192<<24)+(168<<16)+(0<<8)+98,
            (192<<24)+(168<<16)+(1<<8)+(1<<0)
            ,24
        };
        bcm_mac_t switch_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
#endif
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "TS0TS1CombinedModeEn", PQ_INT, (void*)0, &arg_ts0_ts1_combined_mode_en, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }

        if (arg_ts0_ts1_combined_mode_en > 1) {
            parse_arg_eq_done(&pt);
            cli_out("\n Error in setting ts0 ts1 combined mode %d\n", arg_ts0_ts1_combined_mode_en);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }


        stack_info.id = 0;
        stack_info.flags = 0;
        if (arg_ts0_ts1_combined_mode_en) {
            stack_info.flags |= BCM_PTP_STACK_TIMESTAMPER_COMBINED_MODE;
        }

#if defined(PTP_KEYSTONE_STACK)
        stack_info.flags = BCM_PTP_STACK_EXTERNAL_TOP;
        stack_info.ext_stack_info = &external;
#endif
        parse_arg_eq_done(&pt);

        rv = bcm_ptp_stack_create(unit, &stack_info);

        if (rv == BCM_E_NONE) {
            cur_stack_id = stack_info.id;
            PTP_CLI_RESULT_RETURN(CMD_OK);
        } else {
            cli_out("\nbcm_ptp_stack_create() Error... (FCN RETURN= %d)\n", rv);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }

#if defined(PTP_KEYSTONE_STACK)
        _bcm_ptp_rcpu_configuration_set(unit, cur_stack_id, 0x5000, 0x3150, 0x8100, 1234, 23, 0x5001, switch_mac);
#endif

#ifndef __KERNEL__
    } else if (parse_cmp("Destroy", arg, 0)) {
        rv = bcm_ptp_stack_destroy(unit, 0);
        if (rv == BCM_E_NONE) {
            PTP_CLI_RESULT_RETURN(CMD_OK);
        } else {
            cli_out("\nbcm_ptp_stack_create() Error... (FCN RETURN= %d)\n", rv);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }
    } else if (parse_cmp("Performance", arg, 0)) {
        PTP_IF_ERROR_RETURN(_bcm_ptp_show_system_info(unit, 0, 0));  /* Fixed at stack_id = 0 for now */
        PTP_CLI_RESULT_RETURN(CMD_OK);
#endif
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }
}


/* PTP Clock */
#define PTP_CLOCK_USAGE \
    "PTP Clock <SubCommand> [...]\n"                                          \
    "\t PTP Clock Create NumPorts=<num_ports> ID=<clock_id> P1=<priority1>\n" \
    "\t                  SlaveOnly=<y/N> OneWay=<y/N> TELecom=<y/N>\n"        \
    "\t                  DELayREQuestALT=<y/N>  TRaceability=<y/N>\n"         \
    "\t                  PROFile=<0=8265.1, 1=8275.1, 2=8275.2> OffsetScaledLogVar=<> P2=<> HoldoverInspecTime=<sec>\n"\
    "\t                  TargetedDelReqRespTxEn=<0/1> UntaggedDelReqRespTxEn=<0/1>\n "\
    "\t PTP Clock Set TRACEability=<NONE|FREQ|TIME|FREQ-TIME|NO-OVERRIDE>\n"              \
    "\t PTP Clock Set PKTflags=<SYNC-UNCERTAIN>\n"              \
    "\t PTP Clock Show\n"

STATIC cmd_result_t
cmd_ptp_clock(int unit, args_t *a)
{
    parse_table_t pt;
    char * arg;

    int invalid_value = 0x7fffffff;

    int rv = BCM_E_NONE;
    int num_ports;
    int priority1 = PTP_CLOCK_PRESETS_PRIORITY1_DEFAULT;
    int arg_slaveOnly = 0;
    int arg_oneWay = 0;
    int arg_telecomProfile = 0;
    int arg_delreqAlt = 0;
    int arg_syncOAM = 0;
    int arg_domain = -1;
    int arg_max_unicast_masters = -1;
    int arg_max_acceptable_masters = -1;
    int arg_max_unicast_slaves = -1;
    int arg_max_multicast_slave_stats = -1;
    int arg_traceability = -1;
    int arg_profile = 0;
    int arg_local_priority = 128;
    int arg_max_steps_removed= 255;
    int arg_offscal_logvar = -1;
    int arg_holdover_inspec_time = 0;
    int priority2 = -1;

    bcm_ptp_clock_identity_t clock_id;
    char *clock_string;
    int clock_id_byte = 0;
    char *token;
    char *rem;
    char *tokstr = NULL;
    bcm_ptp_trace_t traceability;
    int configured = 0;

    char *arg_trace_str[6] = {"NONE", "FREQ", "TIME", "FREQ-TIME", "NO-OVERRIDE", 0};
    int arg_trace_index = invalid_value;

    char *arg_pktflags_str[2] = {"SYNC-UNCERTAIN", 0};
    int arg_pktflags_index = invalid_value;

    int arg_targeted_delreq_delresp_tx_en = 0, arg_untagged_delreq_delresp_tx_en = 0;

    arg = ARG_GET(a);
    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }
    if (parse_cmp("Create", arg, 0)) {
        bcm_ptp_clock_info_t clock_info = {0, 0, {1,2,3,4,5,6,7,8}, bcmPTPClockTypeBoundary, 2,
                                           248, 0, 1, 128, 64, 0, 0, 0, 1,
                                           PTP_CLOCK_PRESETS_ANNOUNCE_RECEIPT_TIMEOUT_MINIMUM,
                                           PTP_CLOCK_PRESETS_ANNOUNCE_RECEIPT_TIMEOUT_DEFAULT,
                                           PTP_CLOCK_PRESETS_ANNOUNCE_RECEIPT_TIMEOUT_MAXIMUM,
                                           PTP_CLOCK_PRESETS_LOG_ANNOUNCE_INTERVAL_MINIMUM,
                                           PTP_CLOCK_PRESETS_LOG_ANNOUNCE_INTERVAL_DEFAULT,
                                           PTP_CLOCK_PRESETS_LOG_ANNOUNCE_INTERVAL_MAXIMUM,
                                           PTP_CLOCK_PRESETS_LOG_SYNC_INTERVAL_MINIMUM,
                                           PTP_CLOCK_PRESETS_LOG_SYNC_INTERVAL_DEFAULT,
                                           PTP_CLOCK_PRESETS_LOG_SYNC_INTERVAL_MAXIMUM,
                                           PTP_CLOCK_PRESETS_LOG_MIN_DELAY_REQ_INTERVAL_MINIMUM,
                                           PTP_CLOCK_PRESETS_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT,
                                           PTP_CLOCK_PRESETS_LOG_MIN_DELAY_REQ_INTERVAL_MAXIMUM,
                                           PTP_CLOCK_PRESETS_DOMAIN_NUMBER_MINIMUM,
                                           PTP_CLOCK_PRESETS_DOMAIN_NUMBER_DEFAULT,
                                           PTP_CLOCK_PRESETS_DOMAIN_NUMBER_MAXIMUM,
                                           PTP_CLOCK_PRESETS_PRIORITY1_MINIMUM,
                                           PTP_CLOCK_PRESETS_PRIORITY1_DEFAULT,
                                           PTP_CLOCK_PRESETS_PRIORITY1_MAXIMUM,
                                           PTP_CLOCK_PRESETS_PRIORITY2_MINIMUM,
                                           PTP_CLOCK_PRESETS_PRIORITY2_DEFAULT,
                                           PTP_CLOCK_PRESETS_PRIORITY2_MAXIMUM,
                                           0, 128,
                                           PTP_CLOCK_PRESETS_TELECOM_MAX_STEPS_REMOVED_DEFAULT};

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "NumPorts", PQ_INT, (void*)1, &num_ports, NULL);
        parse_table_add(&pt, "ID", PQ_STRING, (void*)"11:22:33:ff:fe:44:55:66",
                        &clock_string, NULL);
        parse_table_add(&pt, "P1", PQ_INT, (void*)PTP_CLOCK_PRESETS_PRIORITY1_DEFAULT,
                        &priority1, NULL);
        parse_table_add(&pt, "SlaveOnly", PQ_BOOL, (void*)0, &arg_slaveOnly, NULL);
        parse_table_add(&pt, "OneWay", PQ_BOOL, (void*)0, &arg_oneWay, NULL);
        parse_table_add(&pt, "TELecom", PQ_BOOL, (void*)0, &arg_telecomProfile, NULL);
        parse_table_add(&pt, "DELayREQuestALT", PQ_BOOL, (void*)0, &arg_delreqAlt, NULL);
        parse_table_add(&pt, "SYNCOAM", PQ_BOOL, (void*)0, &arg_syncOAM, NULL);
        parse_table_add(&pt, "DOMain", PQ_INT, (void*)-1, &arg_domain, NULL);
        parse_table_add(&pt, "MaxUnicastMasters", PQ_INT, (void*)-1, &arg_max_unicast_masters, NULL);
        parse_table_add(&pt, "MaxAcceptableMasters", PQ_INT, (void*)-1, &arg_max_acceptable_masters, NULL);
        parse_table_add(&pt, "MaxUnicastSlaves", PQ_INT, (void*)-1, &arg_max_unicast_slaves, NULL);
        parse_table_add(&pt, "MaxMulticastSlaveStats", PQ_INT, (void*)-1, &arg_max_multicast_slave_stats, NULL);
        parse_table_add(&pt, "TRaceability", PQ_BOOL, (void*)-1, &arg_traceability, NULL);
        parse_table_add(&pt, "PROFile", PQ_INT, (void*)0, &arg_profile, NULL);
        parse_table_add(&pt, "LocalPriority", PQ_INT, (void*)128, &arg_local_priority, NULL);
        parse_table_add(&pt, "MaxStepsRemoved", PQ_INT, (void*)255, &arg_max_steps_removed, NULL);
        parse_table_add(&pt, "OffsetScaledLogVar", PQ_INT, (void*)-1, &arg_offscal_logvar, NULL);
        parse_table_add(&pt, "HoldoverInspecTime", PQ_INT, (void*)0, &arg_holdover_inspec_time, NULL);
        parse_table_add(&pt, "P2", PQ_INT, (void*)-1, &priority2, NULL);
        parse_table_add(&pt, "TargetedDelReqRespTxEn", PQ_INT, (void*)0, &arg_targeted_delreq_delresp_tx_en, NULL);
        parse_table_add(&pt, "UntaggedDelReqRespTxEn", PQ_INT, (void*)0, &arg_untagged_delreq_delresp_tx_en, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }

        token = sal_strtok_r(clock_string, ":", &tokstr);
        while ((token != NULL) && (clock_id_byte < 8)) {
            clock_id[clock_id_byte++] = strtol(token, &rem, 16);
            token = sal_strtok_r(NULL, ":", &tokstr);
        }
        if (clock_id_byte != 8) {
            cli_out("\nbcm_ptp_clock_create() Error... ID format\n");
            parse_arg_eq_done(&pt);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }

        if (arg_profile >= e_PtpProfType_None) {
            cli_out("\n Error in setting profile type %d\n", arg_profile);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }
        if ((1 == arg_telecomProfile) && (e_PtpProfType_8265p1 != arg_profile)) {
            /* Clock can either be configured for frequency or phase profile */
            cli_out("\n Conflicting profile types while configuring clock\n");
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }
        cli_out("\n profile type %d\n", arg_profile);

        clock_info.num_ports = num_ports;
        if (num_ports > 1) {
            clock_info.type = bcmPTPClockTypeBoundary;
        } else {
            clock_info.type = bcmPTPClockTypeOrdinary;
        }

        memcpy(clock_info.clock_identity, clock_id, sizeof(clock_id));
        clock_info.priority1 = priority1;
        clock_info.slaveonly = arg_slaveOnly;

        if (e_PtpProfType_8275p1 == arg_profile) {
            /* local_priority shall be in the range 1-255 */
            if ((arg_local_priority > 0) && (arg_local_priority < 256)) {
                clock_info.local_priority = (uint8)arg_local_priority;
            } else {
                cli_out("\n Error in setting local priority  %d\n", arg_local_priority);
                PTP_CLI_RESULT_RETURN(CMD_FAIL);
            }
            /* max_steps_removed shall be in the range 1-255 */
            if ((arg_max_steps_removed > 0) && (arg_max_steps_removed < 256)) {
                clock_info.max_steps_removed = (uint8)arg_max_steps_removed;
            } else {
                cli_out("\n Error in setting maxStepsRemoved %d\n", arg_max_steps_removed);
                PTP_CLI_RESULT_RETURN(CMD_FAIL);
            }
        }

        if (e_PtpProfType_8275p2 == arg_profile) {
            /* local_priority shall be in the range 1-255 */
            if ((arg_local_priority > 0) && (arg_local_priority < 256)) {
                clock_info.local_priority = (uint8)arg_local_priority;
            } else {
                cli_out("\n Error in setting local priority  %d\n", arg_local_priority);
                PTP_CLI_RESULT_RETURN(CMD_FAIL);
            }
        }

        if (e_PtpProfType_8275p1 == arg_profile || e_PtpProfType_8275p2 == arg_profile) {
            clock_info.holdover_in_spec_secs = arg_holdover_inspec_time;
        }

        clock_info.flags = 0;

        /* By default: fixed interval delay requests */
        clock_info.flags |= (1u << PTP_CLOCK_FLAGS_FIXED_DELREQ);

        if (1 == arg_oneWay) {
            clock_info.flags |= (1u << PTP_CLOCK_FLAGS_ONEWAY_BIT);
        }

        if (1 == arg_delreqAlt) {
            clock_info.flags |= (1u << PTP_CLOCK_FLAGS_DELREQ_ALTMASTER_BIT);
        }

        if (1 == arg_telecomProfile) {
            clock_info.flags |= (1u << PTP_CLOCK_FLAGS_TELECOM_PROFILE_BIT);

            /* Enforce telecom profile compliant attributes and profile ranges. */
            clock_info.type = bcmPTPClockTypeOrdinary;
            clock_info.num_ports = 1;

            if (1 == arg_slaveOnly) {
                clock_info.clock_class = 255;
            }
            /* Domain can be overridden below, but telecom has a different default */
            clock_info.domain_number =
                PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_DEFAULT;

            clock_info.log_announce_interval_minimum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_ANNOUNCE_INTERVAL_MINIMUM;
            clock_info.log_announce_interval_default =
                PTP_CLOCK_PRESETS_TELECOM_LOG_ANNOUNCE_INTERVAL_DEFAULT;
            clock_info.log_announce_interval_maximum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_ANNOUNCE_INTERVAL_MAXIMUM;

            clock_info.log_sync_interval_minimum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_SYNC_INTERVAL_MINIMUM;
            clock_info.log_sync_interval_default =
                PTP_CLOCK_PRESETS_TELECOM_LOG_SYNC_INTERVAL_DEFAULT;
            clock_info.log_sync_interval_maximum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_SYNC_INTERVAL_MAXIMUM;

            clock_info.log_min_delay_req_interval_minimum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_MIN_DELAY_REQ_INTERVAL_MINIMUM;
            clock_info.log_min_delay_req_interval_default =
                PTP_CLOCK_PRESETS_TELECOM_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT;
            clock_info.log_min_delay_req_interval_maximum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_MIN_DELAY_REQ_INTERVAL_MAXIMUM;

            clock_info.domain_number_minimum =
                PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_MINIMUM;
            clock_info.domain_number_default =
                PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_DEFAULT;
            clock_info.domain_number_maximum =
                PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_MAXIMUM;
        }

        if (arg_syncOAM) {
            clock_info.flags |= (1u << PTP_CLOCK_FLAGS_SYNCOAM_BIT);
        }

        if (arg_profile) {
            /* Bits#8-11:i identifies the profile type to configure the clock
               0: Frequency profile (8265.1) : handled earlier in TELecom arg 
               1: Phase profile (8275.1)
            */
            clock_info.flags |= ((arg_profile & 0x0F) << PTP_CLOCK_FLAGS_PROFILE_TYPE_START_BIT);
            /* parameters as required by the 8275.1 telecom phase profile */

            if ( 0x01 == (arg_profile & 0x0F) ) {
                /* Domain can be overridden below, but telecom profile for phase has a different default */
                clock_info.domain_number = PTP_CLOCK_PRESETS_TELECOM_PHASE_DOMAIN_NUMBER_DEFAULT;

                clock_info.log_announce_interval_minimum =
                    PTP_CLOCK_PRESETS_TELECOM_LOG_ANNOUNCE_INTERVAL_MINIMUM;
                clock_info.log_announce_interval_default =
                    PTP_CLOCK_PRESETS_TELECOM_PHASE_LOG_ANNOUNCE_INTERVAL_DEFAULT;
                clock_info.log_announce_interval_maximum =
                    PTP_CLOCK_PRESETS_TELECOM_LOG_ANNOUNCE_INTERVAL_MAXIMUM;

                clock_info.log_sync_interval_minimum =
                    PTP_CLOCK_PRESETS_TELECOM_LOG_SYNC_INTERVAL_MINIMUM;
                clock_info.log_sync_interval_default =
                    PTP_CLOCK_PRESETS_TELECOM_PHASE_LOG_SYNC_INTERVAL_DEFAULT;
                clock_info.log_sync_interval_maximum =
                    PTP_CLOCK_PRESETS_TELECOM_LOG_SYNC_INTERVAL_MAXIMUM;

                clock_info.log_min_delay_req_interval_minimum =
                    PTP_CLOCK_PRESETS_TELECOM_LOG_MIN_DELAY_REQ_INTERVAL_MINIMUM;
                clock_info.log_min_delay_req_interval_default =
                    PTP_CLOCK_PRESETS_TELECOM_PHASE_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT;
                clock_info.log_min_delay_req_interval_maximum =
                    PTP_CLOCK_PRESETS_TELECOM_LOG_MIN_DELAY_REQ_INTERVAL_MAXIMUM;

                clock_info.domain_number_minimum =
                    PTP_CLOCK_PRESETS_TELECOM_PHASE_DOMAIN_NUMBER_MINIMUM;
                clock_info.domain_number_default =
                    PTP_CLOCK_PRESETS_TELECOM_PHASE_DOMAIN_NUMBER_DEFAULT;
                clock_info.domain_number_maximum =
                    PTP_CLOCK_PRESETS_TELECOM_PHASE_DOMAIN_NUMBER_MAXIMUM;

                if (clock_info.slaveonly) {
                    /* For TSC, priority2 shall be 255 */
                    clock_info.priority2 = PTP_CLOCK_PRESETS_TELECOM_PHASE_PRIORITY2_TSC_DEFAULT;
                }

                if (clock_info.type == bcmPTPClockTypeBoundary) {
                    clock_info.priority2 = PTP_CLOCK_PRESETS_TELECOM_PHASE_PRIORITY2_TBC_DEFAULT;
                    /* For G.8275.1, user can configure offsetScaledLogVariance as per below.
                       defaultDS.clockQuality.offsetScaledLogVariance:
                       - 0x4E5D for a T-GM connected to a PRTC in locked-mode (i.e. PRTC traceable to GNSS)
                       - 0xFFFF for a T-GM not connected to a PRTC in locked-mode
                       - 0xFFFF for a T-BC, all the time */
                    clock_info.scaled_log_variance = 0xFFFF;
                }
            } else if ( 0x02 == (arg_profile & 0x0F) ) {
                /* Range Configuration for G.8275.2 Profile based on Table A.1 */
                /* Domain can be overridden below, but telecom profile for phase has a different default */
                clock_info.domain_number_minimum = PTP_CLOCK_PRESETS_G8275P2_DOMAIN_NUMBER_MINIMUM;
                clock_info.domain_number_default = PTP_CLOCK_PRESETS_G8275P2_DOMAIN_NUMBER_DEFAULT;
                clock_info.domain_number_maximum = PTP_CLOCK_PRESETS_G8275P2_DOMAIN_NUMBER_MAXIMUM;

                clock_info.priority1_minimum = PTP_CLOCK_PRESETS_G8275P2_PRIORITY1_MINIMUM;
                clock_info.priority1_default = PTP_CLOCK_PRESETS_G8275P2_PRIORITY1_DEFAULT;
                clock_info.priority1_maximum = PTP_CLOCK_PRESETS_G8275P2_PRIORITY1_MAXIMUM;

                clock_info.priority2_minimum = PTP_CLOCK_PRESETS_G8275P2_PRIORITY2_MINIMUM;
                clock_info.priority2_default = PTP_CLOCK_PRESETS_G8275P2_PRIORITY2_DEFAULT;
                clock_info.priority2_maximum = PTP_CLOCK_PRESETS_G8275P2_PRIORITY2_MAXIMUM;

                /* Range Configuration for G.8275.2 Profile based on Section A.3.4 */
                clock_info.log_announce_interval_minimum =
                    PTP_CLOCK_PRESETS_G8275P2_LOG_ANNOUNCE_INTERVAL_MINIMUM;
                clock_info.log_announce_interval_default =
                    PTP_CLOCK_PRESETS_G8275P2_LOG_ANNOUNCE_INTERVAL_DEFAULT;
                clock_info.log_announce_interval_maximum =
                    PTP_CLOCK_PRESETS_G8275P2_LOG_ANNOUNCE_INTERVAL_MAXIMUM;

                clock_info.log_sync_interval_minimum =
                    PTP_CLOCK_PRESETS_G8275P2_LOG_SYNC_INTERVAL_MINIMUM;
                clock_info.log_sync_interval_default =
                    PTP_CLOCK_PRESETS_G8275P2_LOG_SYNC_INTERVAL_DEFAULT;
                clock_info.log_sync_interval_maximum =
                    PTP_CLOCK_PRESETS_G8275P2_LOG_SYNC_INTERVAL_MAXIMUM;

                clock_info.log_min_delay_req_interval_minimum =
                    PTP_CLOCK_PRESETS_G8275P2_LOG_MIN_DELAY_REQ_INTERVAL_MINIMUM;
                clock_info.log_min_delay_req_interval_default =
                    PTP_CLOCK_PRESETS_G8275P2_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT;
                clock_info.log_min_delay_req_interval_maximum =
                    PTP_CLOCK_PRESETS_G8275P2_LOG_MIN_DELAY_REQ_INTERVAL_MAXIMUM;

                clock_info.domain_number = PTP_CLOCK_PRESETS_G8275P2_DOMAIN_NUMBER_DEFAULT;
                clock_info.priority1 = PTP_CLOCK_PRESETS_G8275P2_PRIORITY1_DEFAULT;
                clock_info.priority2 = PTP_CLOCK_PRESETS_G8275P2_PRIORITY2_DEFAULT;
                /*
                 *  For G.8275.1, user can configure offsetScaledLogVariance as per below.
                 *   defaultDS.clockQuality.offsetScaledLogVariance:
                 *   0x4B32 for a T-GM connected to an ePRTC in locked-mode
                 *   0x4E5D for a T-GM connected to a PRTC in locked-mode
                 *   0xFFFF for a T-GM not connected to a PRTC in locked-mode
                 *   0xFFFF for a T-BC-P not connected to a GNSS in locked mode on a virtual PTP port
                 */
                clock_info.scaled_log_variance = 0xFFFF;
                clock_info.announce_receipt_timeout_default =
                                PTP_CLOCK_PRESETS_G8275P2_ANNOUNCE_RECEIPT_TIMEOUT_DEFAULT;

                /* Override values needed for G8275P2 Profile as per A.1 */
                if (1 == arg_slaveOnly) {
                    clock_info.clock_class = 255;
                    clock_info.priority2 = PTP_CLOCK_PRESETS_G8275P2_PRIORITY2_TSC_DEFAULT;
                } else {
                    clock_info.clock_class = 248;
                }

            }
        } /* if(arg_profile) */

        if (arg_targeted_delreq_delresp_tx_en) {
            SHR_BITSET(&clock_info.flags, PTP_CLOCK_FLAGS_TARGETED_DELREQ_DELRESP_TX_BIT);
        } else {
            SHR_BITCLR(&clock_info.flags, PTP_CLOCK_FLAGS_TARGETED_DELREQ_DELRESP_TX_BIT);
        }
        if (arg_untagged_delreq_delresp_tx_en) {
            SHR_BITSET(&clock_info.flags, PTP_CLOCK_FLAGS_UNTAGGED_DELREQ_DELRESP_TX_BIT);
        } else {
            SHR_BITCLR(&clock_info.flags, PTP_CLOCK_FLAGS_UNTAGGED_DELREQ_DELRESP_TX_BIT);
        }

        parse_arg_eq_done(&pt);

        if (priority2 >= 0 ) {
            /* p2 is explicitly configured - override */
            clock_info.priority2 = priority2;
        }
        if (arg_offscal_logvar >= 0) {
			/* To work with IXIA, offset scaled log var needs to be set to 0 */
            clock_info.scaled_log_variance = (uint16)arg_offscal_logvar;
	    }

        if (arg_domain >= 0) {
            clock_info.domain_number = arg_domain;
        }

        if (arg_max_unicast_masters >= 0) {
            _bcm_ptp_set_max_unicast_masters(unit, cur_stack_id, arg_max_unicast_masters);
        }

        if (arg_max_acceptable_masters >= 0) {
            _bcm_ptp_set_max_acceptable_masters(unit, cur_stack_id, arg_max_acceptable_masters);
        }

        if (arg_max_unicast_slaves >= 0) {
            _bcm_ptp_set_max_unicast_slaves(unit, cur_stack_id, arg_max_unicast_slaves);
        }

        if (arg_max_multicast_slave_stats >= 0) {
            _bcm_ptp_set_max_multicast_slave_stats(unit, cur_stack_id, arg_max_multicast_slave_stats);
        }
        
        rv = bcm_ptp_clock_create(unit, cur_stack_id, &clock_info);
        if (rv == BCM_E_NONE) {
            cur_clock_num = clock_info.clock_num;

            if (1 == arg_traceability) {
               traceability.frequency_traceable = 1;
               traceability.time_traceable = 1;
               configured = 1;
            } else if (0 == arg_traceability) {
               traceability.frequency_traceable = 0;
               traceability.time_traceable = 0;
               configured = 1;
            } 

            if (configured == 1 && e_PtpProfType_8275p2 != arg_profile) {
               rv = bcm_ptp_clock_traceability_set(unit, cur_stack_id, cur_clock_num, &traceability);
               if (rv != BCM_E_NONE) {
                  cli_out("\nbcm_ptp_clock_traceability_set() Error... (FCN RETURN= %d)\n", rv);
                  PTP_CLI_RESULT_RETURN(CMD_FAIL);
               }
            }
            if (e_PtpProfType_8275p2 == arg_profile) {
               rv = bcm_ptp_unicast_request_duration_min_set(unit, cur_stack_id, cur_clock_num, 0,
                                                             BCM_PTP_SIGNAL_G8275P2_REQUEST_UC_MINIMUM_DURATION_SEC);
               if (rv != BCM_E_NONE) {
                  cli_out("\nbcm_ptp_unicast_request_duration_min_set() Error... (FCN RETURN= %d)\n", rv);
                  PTP_CLI_RESULT_RETURN(CMD_FAIL);
               }
               rv = bcm_ptp_unicast_request_duration_max_set(unit, cur_stack_id, cur_clock_num, 0,
                                                             BCM_PTP_SIGNAL_G8275P2_REQUEST_UC_MAXIMUM_DURATION_SEC);
               if (rv != BCM_E_NONE) {
                  cli_out("\nbcm_ptp_unicast_request_duration_max_set() Error... (FCN RETURN= %d)\n", rv);
                  PTP_CLI_RESULT_RETURN(CMD_FAIL);
               }
               rv = bcm_ptp_unicast_request_duration_set(unit, cur_stack_id, cur_clock_num, 0,
                                                             BCM_PTP_SIGNAL_G8275P2_REQUEST_UC_DEFAULT_DURATION_SEC);
               if (rv != BCM_E_NONE) {
                  cli_out("\nbcm_ptp_unicast_request_duration_set() Error... (FCN RETURN= %d)\n", rv);
                  PTP_CLI_RESULT_RETURN(CMD_FAIL);
               }
            }
            PTP_CLI_RESULT_RETURN(CMD_OK);
        }
        else {
            cli_out("\nbcm_ptp_clock_create() Error... (FCN RETURN= %d)\n", rv);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }
    } else if (parse_cmp(arg, "Set", 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "TRACEability", PQ_MULTI | PQ_DFL, 0, &arg_trace_index, arg_trace_str);
        parse_table_add(&pt, "PKTflags", PQ_MULTI | PQ_DFL, 0, &arg_pktflags_index, arg_pktflags_str);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        if (arg_trace_index != invalid_value) {
            switch (arg_trace_index) {
            case 0:
                /* NONE: Frequency-Traceable FALSE, Time-Traceable FALSE */
                traceability.frequency_traceable = 0;
                traceability.time_traceable = 0;
                break;
            case 1:
                /* FREQ: Frequency-Traceable TRUE , Time-Traceable FALSE */
                traceability.frequency_traceable = 1;
                traceability.time_traceable = 0;
                break;
            case 2:
                /* TIME: Frequency-Traceable FALSE, Time-Traceable TRUE  */
                traceability.frequency_traceable = 0;
                traceability.time_traceable = 1;
                break;
            case 3:
                /* FREQ+TIME: Frequency-Traceable TRUE , Time-Traceable TRUE  */
                traceability.frequency_traceable = 1;
                traceability.time_traceable = 1;
                break;
            case 4:
                /* No OverRide, Follow the GM Traceability option  */
                traceability.frequency_traceable = BCM_PTP_TRACEABILITY_NO_OVERRIDE;
                traceability.time_traceable = BCM_PTP_TRACEABILITY_NO_OVERRIDE;
                break;
            default:
                cli_out("PTP CLOCK SET ... : Invalid traceability configuration.\n");
                PTP_CLI_RESULT_RETURN(CMD_USAGE);
            }

            PTP_IF_ERROR_RETURN(bcm_ptp_clock_traceability_set(unit, cur_stack_id, cur_clock_num, &traceability));
        }

        if (arg_pktflags_index != invalid_value) {
            switch (arg_pktflags_index) {
            case 0:
                /* SYNC-UNCERTAIN */
                {
                    uint32 mask=0;
                    uint32 value=0;

                    mask |= (1 << BCM_PTP_PKT_FLAG_SYNC_UNCERTAIN);
                    value |= (1 << BCM_PTP_PKT_FLAG_SYNC_UNCERTAIN);
                    bcm_ptp_pkt_flags_override_set(0,0,0, mask, value);
                }

                break;
           default:
                cli_out("PTP CLOCK SET ... : Invalid packet flags configuration.\n");
                PTP_CLI_RESULT_RETURN(CMD_USAGE);
            }

        }


        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp(arg, "Show", 0)) {
        return ptp_show_clock(unit, cur_stack_id, cur_clock_num);
    } else if (parse_cmp(arg, "Peers", 0)) {
        return ptp_show_peers(unit, cur_stack_id, cur_clock_num);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }
}


/* PTP Port */
#define PTP_PORT_USAGE \
    "PTP Port <SubCommand> [port_num] [...]\n\t" \
    " PTP Port Configure [port_num] MAC=<macaddr> IP=<ipaddr> IPv6=<ip6addr> MultiCast=<Y/n> WILDcard=<y/N>\n\t" \
    "                    Prio=<prio> logAnnounceInterval=<intv> logSyncInterval=<intv> Vlan=<tag>\n\t"           \
    "                    logDelayrequestInterval=<intv> TimestampMechanism=<ToP|RCPU|PHY|Unimac32|Unimac48>\n\t" \
    "                    TTL=<ttl> DSCP=<ip_dscp>\n\t" \
    "                    rxMapCriteria=<rx_match_criteria_mask> RxPortMaskL=<match_rx_pbmp_1-31> RxPortMaskH=<match_rx_pbmp_32-63>\n\t" \
    " PTP Port Show [port_num]\n" \
    " PTP Port Dropcounter [port_num] Config ENable=<y/N>\n" \
    " PTP Port Dropcounter [port_num] Show\n"

STATIC cmd_result_t
cmd_ptp_port(int unit, args_t *a)
{
    int rv;
    char * arg;
    uint8 l2_multicast_mac[6]        = {0x01, 0x1B, 0x19, 0x00, 0x00, 0x00};
    uint8 l2_multicast_pdelay_mac[6] = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e};

    uint8 ipv4_multicast_mac[6]        = {0x01, 0x00, 0x5e, 0x00, 0x01, 0x81};
    uint8 ipv4_multicast_pdelay_mac[6] = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x6b};

    uint8 ipv6_multicast_mac[6]        = {0x33, 0x33, 0x00, 0x00, 0x01, 0x81};
    uint8 ipv6_multicast_pdelay_mac[6] = {0x33, 0x33, 0x00, 0x00, 0x00, 0x6b};


    arg = ARG_GET(a);
    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    if (parse_cmp("Configure", arg, 0)) {
        bcm_ptp_clock_info_t        clock_info;
        int port = 0;
        int vlan = 1;
        int prio = 0;
        int16 vlantag;
        bcm_ip_t ip;
        bcm_ip6_t ip6 = {0};
        uint8 blank[16] = {0};
        int profile_type;
        int multicast = 1, wildcard = 0;
        int port_type=1, local_prio=128;
        int ttl=0, ip_dscp=0;

        bcm_ptp_clock_port_info_t port_info = {{bcmPTPUDPIPv4, {192,168,0,55}},
                                               {0, 0, 0, 0, 0, 0},
                                               0, {0}, 0, {0},
                                               1, bcmPTPPortTypeStandard,
                                               3, 1, -5, -5,
                                               bcmPTPDelayMechanismEnd2End, bcmPTPMac32CorrectionTimestamps,
                                               0, 0, 0xffffffff, 1, 128,
                                               {0, 0, 0, 0, 0, 0, 0, bcmPTPTODFormatString, {""}, {""}, 0x5006, 0, bcmPTPTODSourceEthernet, 0, 0},
                                               64, 56
        };

        char *arg_timestamp_mechanism = 0;

        int vp_clock_class = 0;
        int vp_clock_accur = 0;
        int vp_scaled_var = 0;
        int vp_steps_rem = 0;
        int vp_gm_priority2 = 0;
        int vp_one_pps_in_gpio=0;
        int vp_ts_sync_enable = 0;
        char * blank_str = "";
        char *format= 0;
        char *parity = 0;
        const char *fmt_str= 0, *mask_str=0;
        char *custom_fmt_str = 0, *custom_mask_str = 0;
        int tod_offset_sec=0;
        int tod_offset_ns=0;
        int ethertype=0;
        int uartNum;
        uint32 baudRate;
        int rxMapCriteria=0;
        uint32 rxPortMaskL32=0xFFFFFFFF;
        uint32 rxPortMaskH32=0;

        /* NMEA ZDA                  "$GPZDA,HHMMSS.00,DD,MM,YYYY,zz,zz*CC"*/
        const char * nmea_mask_str = "      x      xxxx  x  x    xxxxxxxxx";
        const char * nmea_fmt_str  = "$GPZDA %2H%2M%2S    %d %m %Y";

        /* ISO                       "YYYY-MM-DDTHH:MM:SS.000Z"         "*/
        const char * iso_mask_str  = "                   xxxxx";
        const char * iso_fmt_str   = "%Y-%m-%dT%H:%M:%S";

        /* NTP                       "  YY JJ HH:MM:SS.000 S"            */
        const char * ntp_mask_str  = "xx         x  x  xxxx";
        const char * ntp_fmt_str   = "  %y %j %H %M %S      ";


        parse_table_t pt;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MultiCast", PQ_BOOL, (void*)1, &multicast, NULL);
        parse_table_add(&pt, "WILDcard", PQ_BOOL, (void*)0, &wildcard, NULL);
        parse_table_add(&pt, "Vlan", PQ_INT, (void*)1, &vlan, NULL);
        parse_table_add(&pt, "Prio", PQ_INT, (void*)0, &prio, NULL);
        parse_table_add(&pt, "MAC", PQ_MAC | PQ_DFL, (void*)0, port_info.mac, NULL);
        parse_table_add(&pt, "IP", PQ_IP, (void*)0, &ip, NULL);
        parse_table_add(&pt, "IPv6", PQ_IP6 | PQ_DFL, (void*)0, &ip6, NULL);
        parse_table_add(&pt, "logAnnounceInterval", PQ_INT | PQ_DFL, (void*)0, &port_info.log_announce_interval, NULL);
        parse_table_add(&pt, "logSyncInterval", PQ_INT | PQ_DFL, (void*)0, &port_info.log_sync_interval, NULL);
        parse_table_add(&pt, "logDelayrequestInterval", PQ_INT | PQ_DFL, (void*)0, &port_info.log_min_delay_req_interval, NULL);
        parse_table_add(&pt, "TimestampMechanism", PQ_STRING, (void*)"", &arg_timestamp_mechanism, NULL);
        parse_table_add(&pt, "PortType", PQ_INT, (void*)1, &port_type, NULL);
        parse_table_add(&pt, "LocalPriority", PQ_INT, (void*)128, &local_prio, NULL);
        parse_table_add(&pt, "TTL", PQ_INT, (void*)64, &ttl, NULL);
        parse_table_add(&pt, "DSCP", PQ_INT, (void*)56, &ip_dscp, NULL);
        parse_table_add(&pt, "FormatType", PQ_STRING, blank_str, &format, NULL);
        parse_table_add(&pt, "CustomFormatStr", PQ_STRING, blank_str, &custom_fmt_str, NULL);
        parse_table_add(&pt, "CustomMaskStr", PQ_STRING, blank_str, &custom_mask_str, NULL);
        parse_table_add(&pt, "EtherType", PQ_INT, (void*)BCM_TOD_ETHERTYPE, &ethertype, NULL);
        parse_table_add(&pt, "OffsetSec", PQ_INT, (void*)0, &tod_offset_sec, NULL);
        parse_table_add(&pt, "OffsetNS", PQ_INT, (void*)0, &tod_offset_ns, NULL);
        parse_table_add(&pt, "VirtualPortClockClass", PQ_INT, (void*)0, &vp_clock_class, NULL);
        parse_table_add(&pt, "VirtualPortClockAccuracy", PQ_INT, (void*)0, &vp_clock_accur, NULL);
        parse_table_add(&pt, "VirtualPortScaledVar", PQ_INT, (void*)0, &vp_scaled_var, NULL);
        parse_table_add(&pt, "VirtualPortStepsRemoved", PQ_INT, (void*)0, &vp_steps_rem, NULL);
        parse_table_add(&pt, "VirtualPortGmPrio2", PQ_INT, (void*)0, &vp_gm_priority2, NULL);
        parse_table_add(&pt, "VirtualPort1PPSIn", PQ_INT, (void*)0, &vp_one_pps_in_gpio, NULL);
        if(PTP_UC_FEATURE_CHECK(PTP_VP_TOD_IN_SERIAL)) {
            parse_table_add(&pt, "VirtualPortUART", PQ_INT, (void*)-1, &uartNum, NULL);
            parse_table_add(&pt, "VirtualPortBaudRate", PQ_INT, (void*)0, &baudRate, NULL);
            if(PTP_UC_FEATURE_CHECK(PTP_TOD_UART_PARITY)) {
                parse_table_add(&pt, "VirtualPortUartParity", PQ_STRING, (void*)"NONE", &parity, NULL);
            }
        }
        if(PTP_UC_FEATURE_CHECK(PTP_TOD_TS_SYNC_ENABLE)) {
            parse_table_add(&pt, "VirtualPortEnableTimestamperSync", PQ_INT, (void*)0, &vp_ts_sync_enable, NULL);
        }
        parse_table_add(&pt, "RxMapCriteria", PQ_INT, (void*)0, &rxMapCriteria, NULL);
        parse_table_add(&pt, "RxPortMaskL", PQ_INT, (void*)0xFFFFFFFF, &rxPortMaskL32, NULL);
        parse_table_add(&pt, "RxPortMaskH", PQ_INT, (void*)0, &rxPortMaskH32, NULL);

        arg = ARG_GET(a);
        if (!arg) {
            parse_arg_eq_done(&pt);
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

        port = parse_integer(arg);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }

        if (sal_memcmp(port_info.mac, blank, 6) == 0) {
            /* no valid MAC was given */
            parse_arg_eq_done(&pt);
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }


        vlantag = (int16) (vlan + (prio << 13));
        if (vlantag != 0)
        {
            port_info.rx_packets_vlan = vlantag;
        }

        if (ip != 0) {
            port_info.port_address.address[0] = (uint8)(ip >> 24);
            port_info.port_address.address[1] = (uint8)(ip >> 16);
            port_info.port_address.address[2] = (uint8)(ip >> 8);
            port_info.port_address.address[3] = (uint8)(ip);

            memcpy(port_info.multicast_l2, ipv4_multicast_mac, 6);
            memcpy(port_info.multicast_l2 + 6, port_info.mac, 6);
            port_info.multicast_l2[12] = 0x81;
            port_info.multicast_l2[13] = 0x00;
            port_info.multicast_l2[14] = (uint8)(vlantag >> 8);
            port_info.multicast_l2[15] = (uint8)(vlantag);
            port_info.multicast_l2[16] = 0x08;
            port_info.multicast_l2[17] = 0x00;
            port_info.multicast_l2_size = 18;

            memcpy(port_info.multicast_pdelay_l2, ipv4_multicast_pdelay_mac, 6);
            memcpy(port_info.multicast_pdelay_l2 + 6, port_info.mac, 6);
            port_info.multicast_pdelay_l2[12] = 0x81;
            port_info.multicast_pdelay_l2[13] = 0x00;
            port_info.multicast_pdelay_l2[14] = (uint8)(vlantag >> 8);
            port_info.multicast_pdelay_l2[15] = (uint8)(vlantag);
            port_info.multicast_pdelay_l2[16] = 0x08;
            port_info.multicast_pdelay_l2[17] = 0x00;
            port_info.multicast_pdelay_l2_size = 18;
            port_info.ttl = (uint8)(ttl);
            port_info.ip_dscp = (uint8)(ip_dscp);

        } else if (sal_memcmp(ip6, blank, 16) != 0) {
            sal_memcpy(port_info.port_address.address, ip6, 16);
            port_info.port_address.addr_type = bcmPTPUDPIPv6;

            memcpy(port_info.multicast_l2, ipv6_multicast_mac, 6);
            memcpy(port_info.multicast_l2 + 6, port_info.mac, 6);
            port_info.multicast_l2[12] = 0x81;
            port_info.multicast_l2[13] = 0x00;
            port_info.multicast_l2[14] = (uint8)(vlantag >> 8);
            port_info.multicast_l2[15] = (uint8)(vlantag);
            port_info.multicast_l2[16] = 0x86;
            port_info.multicast_l2[17] = 0xdd;
            port_info.multicast_l2_size = 18;

            memcpy(port_info.multicast_pdelay_l2, ipv6_multicast_pdelay_mac, 6);
            memcpy(port_info.multicast_pdelay_l2 + 6, port_info.mac, 6);
            port_info.multicast_pdelay_l2[12] = 0x81;
            port_info.multicast_pdelay_l2[13] = 0x00;
            port_info.multicast_pdelay_l2[14] = (uint8)(vlantag >> 8);
            port_info.multicast_pdelay_l2[15] = (uint8)(vlantag);
            port_info.multicast_pdelay_l2[16] = 0x86;
            port_info.multicast_pdelay_l2[17] = 0xdd;
            port_info.multicast_pdelay_l2_size = 18;
            port_info.ttl = (uint8)(ttl);
            port_info.ip_dscp = (uint8)(ip_dscp);

        } else {
            sal_memcpy(port_info.port_address.address, port_info.mac, 6);
            port_info.port_address.addr_type = bcmPTPIEEE8023;

            memcpy(port_info.multicast_l2, l2_multicast_mac, 6);
            memcpy(port_info.multicast_l2 + 6, port_info.mac, 6);
            port_info.multicast_l2[12] = 0x81;
            port_info.multicast_l2[13] = 0x00;
            port_info.multicast_l2[14] = (uint8)(vlantag >> 8);
            port_info.multicast_l2[15] = (uint8)(vlantag);
            port_info.multicast_l2[16] = 0x88;
            port_info.multicast_l2[17] = 0xf7;
            port_info.multicast_l2_size = 18;

            memcpy(port_info.multicast_pdelay_l2, l2_multicast_pdelay_mac, 6);
            memcpy(port_info.multicast_pdelay_l2 + 6, port_info.mac, 6);
            port_info.multicast_pdelay_l2[12] = 0x81;
            port_info.multicast_pdelay_l2[13] = 0x00;
            port_info.multicast_pdelay_l2[14] = (uint8)(vlantag >> 8);
            port_info.multicast_pdelay_l2[15] = (uint8)(vlantag);
            port_info.multicast_pdelay_l2[16] = 0x88;
            port_info.multicast_pdelay_l2[17] = 0xf7;
            port_info.multicast_pdelay_l2_size = 18;
            port_info.ttl = 0;
            port_info.ip_dscp = 0;
        }

#if defined(PTP_KEYSTONE_STACK)
        port_info.rx_timestamp_mechanism = bcmPTPToPTimestamps;
#else
        port_info.rx_timestamp_mechanism = bcmPTPMac32CorrectionTimestamps;
#endif
        if (arg_timestamp_mechanism) {
            if (parse_cmp("ToP", arg_timestamp_mechanism, 0)) {
                port_info.rx_timestamp_mechanism = bcmPTPToPTimestamps;
            } else if (parse_cmp("RCPU", arg_timestamp_mechanism, 0)) {
                port_info.rx_timestamp_mechanism = bcmPTPRCPUTimestamps;
            } else if (parse_cmp("PHY", arg_timestamp_mechanism, 0)) {
                port_info.rx_timestamp_mechanism = bcmPTPPhyCorrectionTimestamps;
            } else if (parse_cmp("Unimac32", arg_timestamp_mechanism, 0)) {
                port_info.rx_timestamp_mechanism = bcmPTPMac32CorrectionTimestamps;
            } else if (parse_cmp("Unimac48", arg_timestamp_mechanism, 0)) {
                port_info.rx_timestamp_mechanism = bcmPTPMac48CorrectionTimestamps;
                if (SOC_IS_TRIDENT2PLUS(unit)) {
                    cli_out("\n TD2PLUS : PTP application do not support 48 bit time-stamping \n");
                    PTP_CLI_RESULT_RETURN(CMD_FAIL);
                }
            }
        }

        port_info.multicast_tx_enable =  multicast ? 1 : 0;

        port_info.rx_packets_port_mask_low32 = rxPortMaskL32;
        port_info.rx_packets_port_mask_high32 = rxPortMaskH32;

        if (rxMapCriteria == 0) {
            port_info.rx_packets_criteria_mask = 0;


            if (multicast) {
                port_info.rx_packets_criteria_mask |= BCM_PTP_RXMAP_ACCEPT_MULTICAST;
            }

            if (wildcard) {
                port_info.rx_packets_criteria_mask |= BCM_PTP_RXMAP_MATCH_ANY_ADDR;
            }

            if (port_info.rx_packets_vlan != 1) {
#if defined(PTP_KEYSTONE_STACK)
                /* TPID 0x8100 is by default configured as Inner TPID for Keystone */
                port_info.rx_packets_criteria_mask |= BCM_PTP_RXMAP_MATCH_INNER_VLAN;
#else
                port_info.rx_packets_criteria_mask |= BCM_PTP_RXMAP_MATCH_OUTER_VLAN;
#endif
            }
        } else {
            port_info.rx_packets_criteria_mask = rxMapCriteria;
        }

        if (BCM_FAILURE(rv = bcm_ptp_clock_get(unit,cur_stack_id,cur_clock_num,&clock_info))) {
            cli_out("\nbcm_ptp_clock_get() Error... (FCN RETURN= %d)\n", rv);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }

        profile_type = ((clock_info.flags >> 8) & 0x0F);
        if (profile_type == e_PtpProfType_8275p2) {
            port_info.announce_receipt_timeout = 3;
        }

        port_info.port_type = (bcm_ptp_port_type_t)port_type;

        /* local_priority shall be in the range 1-255 */
        if ((local_prio > 0) && (local_prio < 256)) {
            port_info.local_priority = (uint8)local_prio;
        } else {
            cli_out("\n Error in setting local priority  %d\n", local_prio);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }

        if (port_type == bcmPTPPortTypeVirtual ||
			port_type == bcmPTPPortTypeCustomizedVirtual) {

            port_info.vport_info.clock_class = (uint8) vp_clock_class;
            port_info.vport_info.clock_accuracy = (bcm_ptp_clock_accuracy_t)vp_clock_accur;
            port_info.vport_info.offset_scaled_log_variance = (uint16) vp_scaled_var;
            port_info.vport_info.steps_removed = (uint16) vp_steps_rem;
            port_info.vport_info.GM_prio2 = (uint8) vp_gm_priority2;
            port_info.vport_info.onepps_in_gpio = (uint8) vp_one_pps_in_gpio;
            if(PTP_UC_FEATURE_CHECK(PTP_VP_TOD_IN_SERIAL)) {
                switch (uartNum) {
                    case -1:
                        port_info.vport_info.tod_source = bcmPTPTODSourceEthernet;
                        break;
                    case 0:
                        port_info.vport_info.tod_source = bcmPTPTODSourceSerial;
                        break;
                    case 1:
                        port_info.vport_info.tod_source = bcmPTPTODSourceSerial1;
                        break;
                    case 2:
                        port_info.vport_info.tod_source = bcmPTPTODSourceSerial2;
                        break;
                    case 3:
                        port_info.vport_info.tod_source = bcmPTPTODSourceSerial3;
                        break;
                    default:
                        port_info.vport_info.tod_source = bcmPTPTODSourceSerial;
                        break;
                }
                port_info.vport_info.tod_baud_rate = (uint32) baudRate;

                if(PTP_UC_FEATURE_CHECK(PTP_TOD_UART_PARITY)) {
                    if (parity) {
                        if (parse_cmp("NONE", parity, 0)) {
                            port_info.vport_info.tod_parity = bcmPTPTODUartParityNone;
                        } else if (parse_cmp("ODD", parity, 0)) {
                            port_info.vport_info.tod_parity = bcmPTPTODUartParityOdd;
                        } else if (parse_cmp("EVEN", parity, 0)) {
                            port_info.vport_info.tod_parity = bcmPTPTODUartParityEven;
                        }
                    } else {
                        port_info.vport_info.tod_parity = bcmPTPTODUartParityNone;
                    }
                }
            }
                cli_out("\nToD virtual tod_parity %d\n", port_info.vport_info.tod_parity);
            port_info.vport_info.ether_type = (uint16)ethertype;
            port_info.vport_info.tod_offset_sec = tod_offset_sec;
            port_info.vport_info.tod_offset_ns = tod_offset_ns;

            if (parse_cmp("", format, 0)) {
                cli_out("\n ToD Format needs to be specified for virtual port %s", format);
                PTP_CLI_RESULT_RETURN(CMD_FAIL);
            } else {
                cli_out("\n ToD Format specified for virtual port [%s]", format);
            }

            if (parse_cmp("NMEAzda", format, 0)) {
                fmt_str = nmea_fmt_str;
                mask_str = nmea_mask_str;
            } else if (parse_cmp("ISO8601", format, 0)) {
                fmt_str = iso_fmt_str;
                mask_str = iso_mask_str;
            } else if (parse_cmp("NTP", format, 0)) {
                fmt_str = ntp_fmt_str;
                mask_str = ntp_mask_str;
            } else if (parse_cmp("CUSTom", format, 0)) {
                fmt_str = custom_fmt_str;
                mask_str = custom_mask_str;
            }
            if (fmt_str) {
                cli_out("\n fmt_str[%s] \n", fmt_str);
                port_info.vport_info.format = bcmPTPTODFormatString;
                sal_strncpy((char *)port_info.vport_info.format_str,
				            fmt_str, BCM_PTP_MAX_TOD_FORMAT_STRING - 1);
                sal_strncpy((char *)port_info.vport_info.mask_str,
				            mask_str, BCM_PTP_MAX_TOD_FORMAT_STRING - 1);
            } else if (parse_cmp("UBlox", format, 0)) {
                port_info.vport_info.format = bcmPTPTODFormatUBlox;
            } else if (parse_cmp("ChinaMobile", format, 0)) {
                port_info.vport_info.format = bcmPTPTODFormatChinaTcom; /* China Mobile/Telecom use same enumerator. */
                sal_sprintf((char *)port_info.vport_info.format_str, "CM");
            } else if (parse_cmp("ChinaTelecom", format, 0)) {
                port_info.vport_info.format = bcmPTPTODFormatChinaTcom; /* China Mobile/Telecom use same enumerator. */
                sal_sprintf((char *)port_info.vport_info.format_str, "CT");
            } else if (parse_cmp("G8271", format, 0)) {
                port_info.vport_info.format = bcmPTPTODFormatG8271;
                sal_sprintf((char *)port_info.vport_info.format_str, "CM"); /* String is same as ChinaMobile */
                if(PTP_UC_FEATURE_CHECK(PTP_TOD_TS_SYNC_ENABLE)) {
                    port_info.vport_info.ts_sync_enable = vp_ts_sync_enable;
                }
            } else if (parse_cmp("BCM", format, 0)) {
                /* BCMTS is to be used only with redundant setup on stand-by TC. Hence, not considered */
                port_info.vport_info.format = bcmPTPTODFormatBCM;
            } else {
                cli_out("\n ToD Format[%s] specified for virtual port is invalid ", format);
            }
        }
        parse_arg_eq_done(&pt);

        rv = bcm_ptp_clock_port_configure(unit, cur_stack_id, cur_clock_num, port, &port_info);

        cur_port_num = port;

        if (rv) {
            if (rv == BCM_E_PARAM)
               cli_out("Failed: invalid parameters\n");
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        } else {
            PTP_CLI_RESULT_RETURN(CMD_OK);
        }

    } else if (parse_cmp(arg, "Show", 0)) {
        int port = 1;
        arg = ARG_GET(a);
        if (arg) {
            port = parse_integer(arg);

            if (port <= 0) {
                cli_out("Port %s is invalid.  Port numbering starts at 1.\n", arg);
                PTP_CLI_RESULT_RETURN(CMD_FAIL);
            }
        }

        BCM_IF_ERROR_RETURN(ptp_show_port(unit, cur_stack_id, cur_clock_num, port));
#ifndef __KERNEL__
        BCM_IF_ERROR_RETURN(ptp_show_foreign_master_dataset(unit, cur_stack_id, cur_clock_num, port));
#endif

        PTP_CLI_RESULT_RETURN(CMD_OK);
    }  
    else if (parse_cmp(arg, "Dropcounter", 0)) {

        int port = 1;
        char *arg_port_drop_counter_enabled = NULL;

        arg = ARG_GET(a);
        if (!arg) {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

        port = parse_integer(arg);

        arg = ARG_GET(a);

        if (parse_cmp("Show", arg, 0)) {
            bcm_ptp_clock_port_packet_drop_counters_t per_port_packet_drop_counters;
            memset(&per_port_packet_drop_counters, 0x0, sizeof(per_port_packet_drop_counters));
            BCM_IF_ERROR_RETURN(
                bcm_ptp_clock_port_drop_counters_get(unit, cur_stack_id, cur_clock_num, port, 
                &per_port_packet_drop_counters));
            
            cli_out("  Domain mismatch:                                 %u\n", per_port_packet_drop_counters.domain_mismatch);
            cli_out("  Memory alloc failure:                            %u\n", per_port_packet_drop_counters.memory_alloc_failure);
            cli_out("  Pkt len too short:                               %u\n", per_port_packet_drop_counters.pkt_len_too_short);
            cli_out("  Pkt rcvd on bad port state:                      %u\n", per_port_packet_drop_counters.pkt_rcvd_on_bad_port_state);
            cli_out("  Ptp profile mismatch:                            %u\n", per_port_packet_drop_counters.ptp_profile_mismatch);
            cli_out("  Clock port mismatch:                             %u\n", per_port_packet_drop_counters.clock_port_mismatch);
            cli_out("  Ptp packet parsing failure:                      %u\n", per_port_packet_drop_counters.ptp_packet_parsing_failure);

            cli_out("  Unicast delreq from unknown slave:               %u\n", per_port_packet_drop_counters.unicast_delreq_from_unknown_slave);
            cli_out("  Announce rcvd on master only port:               %u\n", per_port_packet_drop_counters.announce_rcvd_on_master_only_port);
            cli_out("  Announce rcvd with invalid steps removed field:  %u\n", per_port_packet_drop_counters.announce_rcvd_with_invalid_steps_removed_field);
            cli_out("  Announce from unknown unicast master:            %u\n", per_port_packet_drop_counters.announce_from_unknown_unicast_master);
            cli_out("  Announce rcvd on gps timesource ptp port:        %u\n", per_port_packet_drop_counters.announce_rcvd_on_gps_timesource_ptp_port);

            cli_out("  Sync rcvd invalid:                               %u\n", per_port_packet_drop_counters.sync_rcvd_invalid);
            cli_out("  Sync rcvd on ptp port without master:            %u\n", per_port_packet_drop_counters.sync_rcvd_on_ptp_port_without_master);

        } else if (parse_cmp("Config", arg, 0)) {
            parse_table_t pt;
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "ENable", PQ_STRING, (void *)"N", &arg_port_drop_counter_enabled, NULL);
 
            if (parse_arg_eq(a, &pt) < 0) {
               cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
            }

            if (parse_cmp("y", arg_port_drop_counter_enabled, 0)) {
                bcm_ptp_clock_port_drop_counters_enable_set(unit, cur_stack_id, cur_clock_num, port, 1);
            } else {
                bcm_ptp_clock_port_drop_counters_enable_set(unit, cur_stack_id, cur_clock_num, port, 0);
            }
            parse_arg_eq_done(&pt);
        }
        else {
            cli_out("  Unexpected argument '%s'\n", arg);
            cli_out("%s", PTP_PORT_USAGE);
        }
        PTP_CLI_RESULT_RETURN(CMD_OK);
    }
    else {
        cli_out("  Unexpected argument '%s'\n", arg);
        cli_out("%s", PTP_PORT_USAGE);

        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }
}

/* PTP Time */
#define PTP_TIME_USAGE \
    "PTP Time                              Set or get the PTP time \n\t"    \
    " PTP Time Set TimeSec=<timeSec> [TimeNS=<timeNS>] \n\t"                \
    " PTP Time Get\n"

STATIC cmd_result_t
cmd_ptp_time(int unit, args_t *a)
{
    int rv;
    const char * arg;
    bcm_ptp_timestamp_t time;
    char uint64_decimal_string[27];

    arg = ARG_GET(a);
    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    time.nanoseconds = 0;
    COMPILER_64_SET(time.seconds, 0, 0);
    if (parse_cmp("Get", arg, 0)) {
        rv = bcm_ptp_clock_time_get(unit, cur_stack_id, cur_clock_num, &time);
        format_uint64_decimal(uint64_decimal_string, time.seconds, ',');
        cli_out("Time: %s.%09u\n", uint64_decimal_string, (unsigned)time.nanoseconds);
        if (rv) {
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        } else {
            PTP_CLI_RESULT_RETURN(CMD_OK);
        }
    } else if (parse_cmp("Set", arg, 0)) {        
        int n_args = 0;
        parse_table_t pt;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "TimeNS", PQ_INT, 0, &time.nanoseconds, NULL);
        parse_table_add(&pt, "TimeSec", PQ_DFL|PQ_INT64, 0, &time.seconds, NULL);
        n_args = parse_arg_eq(a, &pt);
        if (n_args == 1 || n_args == 2) {
            parse_arg_eq_done(&pt);
            PTP_IF_ERROR_RETURN(bcm_ptp_clock_time_set(unit, cur_stack_id, cur_clock_num, &time));
            PTP_CLI_RESULT_RETURN(CMD_OK);
        }
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}



/* PTP Channels */
/* We divide the string because pedantic compiler doesn't allow strings longer then 509 characters */
#define PTP_CHANNELS_USAGE_1 \
    "PTP CHannels                          Configure sources of time \n\t"                                   \
    " PTP CHannels Set Num=<num_channels> PrimaryType=<ch1_type> PrimarySource=<ch1_source>\n\t"             \
    "                  PrimaryFrequency=<ch1_freq> PrimaryFrequencyEnabled=<ch1_freq_en>\n\t"                \
    "                  PrimaryFrequencyPriority=<ch1_freqprio>\n\t"                                          \
    "                  PrimaryTimeEnabled=<ch1_timeenabled> PrimaryTimePriority=<ch1_timeprio>\n\t"          \
    "                  BackupTimeInputSynce=<ch1_input_synce>  \n\t"
#define PTP_CHANNELS_USAGE_2 \
    "                  [BackupType=<ch2_type>] [BackupSource=<ch2_source>] \n\t"                             \
    "                  [BackupFrequency=<ch2_freq>] [BackupFrequencyEnabled=<ch2_freq_en>\n\t"               \
    "                  [BackupFrequencyPriority=<ch2_freqprio>]\n\t"                                         \
    "                  [BackupTimeEnabled=<ch2_time_en> [BackupTimePriority=<ch2_timeprio>]\n\t"             \
    "                  [BackupSynceInputType=<0/1> [NoL1MuxSel=<0/1>\n\t"                                                      \
    " PTP CHannels Get\n\t"                                                                                  \
    " PTP CHannels STatus\n"

#define PTP_CHANNELS_USAGE_EXAMPLES \
    " Example usage of PTP Channels:\n\t"                                                 \
    "   Configure to use PTP only:\n\t"                                                   \
    "      ptp chan set Num=1 PT=0 \n\t"                                                  \
    "   Configure to use PTP + 1PPS (from TS_SYNC0) , priorities 2 & 1 respectively:\n\t"  \
    "      ptp chan set Num=2 PT=0 PFP=2 BT=6 BF=1 BFP=1 \n"

STATIC cmd_result_t
cmd_ptp_channels(int unit, args_t *a)
{
    cmd_result_t our_result = CMD_OK;
    int rv = BCM_E_NONE;
    const char * arg;
    static bcm_ptp_channel_t channels[3];
    static int num_channels;
    int i;
    int show = 0;
    char *pri_src = 0, *bkp_src = 0;

    arg = ARG_GET(a);
    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    if (parse_cmp("Set", arg, 0)) {
        parse_table_t pt;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Num", PQ_INT, (void*)1, &num_channels, NULL);
        parse_table_add(&pt, "PrimaryType", PQ_INT, 0, &channels[0].type, NULL);
        parse_table_add(&pt, "PrimarySource", PQ_STRING, 0, &pri_src, NULL);
        parse_table_add(&pt, "PrimaryFrequency", PQ_INT, (void*)1000, &channels[0].frequency, NULL);
        parse_table_add(&pt, "PrimaryFrequencyPriority", PQ_INT, (void*)1, &channels[0].freq_priority, NULL);
        parse_table_add(&pt, "PrimaryFrequencyEnabled", PQ_INT, (void*)1, &channels[0].freq_enabled, NULL);
        parse_table_add(&pt, "PrimaryTimePriority", PQ_INT, (void*)1, &channels[0].time_prio, NULL);
        parse_table_add(&pt, "PrimaryTimeEnabled", PQ_INT, (void*)1, &channels[0].time_enabled, NULL);
        channels[0].synce_config_flags = 0;

        parse_table_add(&pt, "BackupType", PQ_INT, 0, &channels[1].type, NULL);
        parse_table_add(&pt, "BackupSource", PQ_STRING, 0, &bkp_src, NULL);
        parse_table_add(&pt, "BackupFrequency", PQ_INT, (void*)1, &channels[1].frequency, NULL);
        parse_table_add(&pt, "BackupFrequencyPriority", PQ_INT, (void*)2, &channels[1].freq_priority, NULL);
        parse_table_add(&pt, "BackupFrequencyEnabled", PQ_INT, (void*)1, &channels[1].freq_enabled, NULL);
        parse_table_add(&pt, "BackupTimePriority", PQ_INT, (void*)2, &channels[1].time_prio, NULL);
        parse_table_add(&pt, "BackupTimeEnabled", PQ_INT, (void*)0, &channels[1].time_enabled, NULL);
        parse_table_add(&pt, "BackupTimeInputSynce", PQ_INT, (void*)2, &channels[1].synce_input_port, NULL);
        parse_table_add(&pt, "BackupSynceInputType", PQ_INT, (void*)0, &channels[1].synce_input_clk_type, NULL);
        parse_table_add(&pt, "NoL1MuxSel", PQ_INT, (void*)0, &channels[1].synce_config_flags, NULL);

        parse_table_add(&pt, "ThirdType", PQ_INT, 0, &channels[2].type, NULL);
        parse_table_add(&pt, "ThirdSource", PQ_INT, 0, &channels[2].source, NULL);
        parse_table_add(&pt, "ThirdFrequency", PQ_INT, (void*)1, &channels[2].frequency, NULL);
        parse_table_add(&pt, "ThirdFrequencyPriority", PQ_INT, (void*)2, &channels[2].freq_priority, NULL);
        parse_table_add(&pt, "ThirdFrequencyEnabled", PQ_INT, (void*)1, &channels[2].freq_enabled, NULL);
        parse_table_add(&pt, "ThirdTimePriority", PQ_INT, (void*)2, &channels[2].time_prio, NULL);
        parse_table_add(&pt, "ThirdTimeEnabled", PQ_INT, (void*)0, &channels[2].time_enabled, NULL);
        channels[2].synce_config_flags = 0;

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return(CMD_FAIL);
        }

        if (pri_src && pri_src[0] != 0) {
            if (isint(pri_src)) {
                channels[0].source = parse_integer(pri_src);
            }
#if (defined(BCM_ESW_SUPPORT) && !defined(__KERNEL__))
            else {
                soc_pbmp_t pbm;
                if (parse_pbmp(unit, pri_src, &pbm) != 0) {
                    cli_out("Unable to parse source pbm '%s'\n", pri_src);
                    our_result = CMD_FAIL;
                } else if (soc_switch_sync_mux_from_pbm(unit, pbm,
                        &channels[0].source) != 0) {
                    cli_out("Unable to get mux setting for pbm '%s'\n", pri_src);
                    our_result = CMD_FAIL;
                }
            }
#endif /* ESW_SUPPORT */
        } else {
            channels[0].source = 0;
        }

        if (bkp_src && bkp_src[0] != 0) {
            if (isint(bkp_src)) {
                channels[1].source = parse_integer(bkp_src);
            }
#if (defined(BCM_ESW_SUPPORT) && !defined(__KERNEL__))
            else {
                soc_pbmp_t pbm;
                if (parse_pbmp(unit, bkp_src, &pbm) != 0) {
                    cli_out("Unable to parse source pbm '%s'\n", bkp_src);
                    our_result = CMD_FAIL;
                } else if (soc_switch_sync_mux_from_pbm(unit, pbm,
                        &channels[1].source) != 0) {
                    cli_out("Unable to get mux setting for pbm '%s'\n", bkp_src);
                    our_result = CMD_FAIL;
                }
            }
#endif /* ESW_SUPPORT */
        } else {
            channels[1].source = 0;
        }

        parse_arg_eq_done(&pt);

        if (our_result != CMD_OK) {
            return our_result;
        }

#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TRIDENT2(unit)) 
        {
           /* Handle the logical to physical port mapping on Trident 2 */
           if ((channels[1].synce_input_port < 0) || (channels[1].synce_input_port > 131)) {
              return BCM_E_PARAM;
           }
           else if ((channels[1].synce_input_port >= 0) && (channels[1].synce_input_port <= 128)) {
              channels[1].synce_input_port = bcm_esw_ptp_map_input_synce_clock(channels[1].synce_input_port);
           }
        }
#endif
        rv = bcm_ptp_input_channels_set(unit, cur_stack_id, cur_clock_num, num_channels, channels);
        show = 1;
        if (rv == BCM_E_UNAVAIL) {
            cli_out("Channel configuration NOT supported for the servo type\n");
        }

    } else if (parse_cmp("Get", arg, 0)) {
        num_channels = 3;
        rv = bcm_ptp_input_channels_get(unit, cur_stack_id, cur_clock_num, &num_channels, channels);
        if (rv == BCM_E_UNAVAIL) {
            cli_out("Channel configuration NOT supported for the servo type\n");
        }
        show = 1;
    } else if (parse_cmp("STatus", arg, 0)) {
        _bcm_ptp_channel_status_t channel_status[3];
        int i;

        num_channels = 3;
        rv = _bcm_common_ptp_input_channels_status_get(unit, cur_stack_id, cur_clock_num, &num_channels, channel_status);
        if (rv == BCM_E_UNAVAIL) {
            cli_out("Channel configuration NOT supported for the servo type\n");
        }
        if (rv == BCM_E_NONE) {
            if (num_channels == 0) {
                cli_out("No channels returned\n");
            }
            for (i = 0; i < num_channels; ++i) {
                cli_out("  channel %d:    Frequency       Time\n", i);
                cli_out("---------------------------------------\n");
                cli_out("       status  %8s     %8s\n",
                        (channel_status[i].freq_status == 0) ? "OK" :
                        (channel_status[i].freq_status == 1) ? "Fault" :
                        (channel_status[i].freq_status == 2) ? "Disabled" : "?",
                        (channel_status[i].time_status == 0) ? "OK" :
                        (channel_status[i].time_status == 1) ? "Fault" :
                        (channel_status[i].time_status == 2) ? "Disabled" : "?");
                cli_out("       weight     %3d             %3d\n",
                        channel_status[i].freq_weight, channel_status[i].time_weight);
                cli_out("       QL         %3d             %3d\n",
                        channel_status[i].freq_QL, channel_status[i].time_QL);
                cli_out("---------------------------------------\n");
                cli_out("   QL %s read externally.   Fault status: %08x\n",
                        channel_status[i].ql_read_externally ? "is" : "is not",
                        (unsigned)channel_status[i].fault_map);
                cli_out("\n");
            }
        }
    } else if (parse_cmp("Help", arg, 0) || parse_cmp("?", arg, 0)) {
        cli_out(PTP_CHANNELS_USAGE_1);
        cli_out(PTP_CHANNELS_USAGE_2);
        cli_out(PTP_CHANNELS_USAGE_EXAMPLES);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    if (show && (rv == BCM_E_NONE)) {
        cli_out("Configured channels : %d\n",num_channels);
        for (i = 0; i < num_channels; i++) {
            cli_out("channel[%d]: type=%d, source=%d, frequency=%d, tod_index=%d, freq_prio=%d\n", i,
                    channels[i].type, channels[i].source, channels[i].frequency,
                    channels[i].tod_index,channels[i].freq_priority);

            cli_out("            time_enabled=%c, freq_enabled=%c, time_prio=%d freq_assumed_QL=%d\n",
                    channels[i].time_enabled ? 'y' : 'n', channels[i].freq_enabled ? 'y' : 'n',
                    channels[i].time_prio,channels[i].freq_assumed_QL);

            cli_out("            time_assumed_ql=%d, assumed_ql_enabled=%c\n",
                    channels[i].time_assumed_QL, channels[i].assumed_QL_enabled ? 'y' : 'n');

            cli_out("            l1_clock_input_port=%d l1_clock_input_type=%d ",
                    channels[i].synce_input_port, (int)channels[i].synce_input_clk_type);
            cli_out("            NoL1MuxSel=%08x\n",
                    (channels[i].synce_config_flags & (1<<BCM_PTP_CHANNEL_SYNCE_CONFIG_NO_L1MUX_SELECT)));
        }
    }
    if (rv) {
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_OK);
    }
}


/* PTP Signal */
#define PTP_SIGNAL_USAGE \
    "PTP SIGnals                           Configure output signals \n\t"                      \
    " PTP SIGnals Set Pin=<pin#> Frequency=<Hz> PhaseLock[=y/n] Width=<ns> Offset=<ns> OutputSynce=<output_synce> \n\t"    \
    "       ex. \"ptp signals set p=1 freq=1 pl=y width=100000000\" for 1PPS on GPIO pin=1 \n\t" \
    " PTP SIGnals Get\n"

STATIC cmd_result_t
cmd_ptp_signals(int unit, args_t *a)
{
    int rv;
    const char * arg;

    static bcm_ptp_signal_output_t signal;
    int signal_id;
    int signal_count;
    int i;

    arg = ARG_GET(a);
    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    if (parse_cmp("Get", arg, 0)) {
        bcm_ptp_signal_output_t sig_arr[PTP_MAX_OUTPUT_SIGNALS];
        rv = bcm_ptp_signal_output_get(unit, cur_stack_id, cur_clock_num, &signal_count, sig_arr);
        cli_out("Configured signals : %d\n",signal_count);
        for (i = 0; i < PTP_MAX_OUTPUT_SIGNALS; i++) {
            if (sig_arr[i].frequency)
            {
                cli_out("Signal[%d]: pin=%d, frequency=%d, phaselock=%c, width=%d, offset=%d, outputsynce=%d\n",
                        i,sig_arr[i].pin,sig_arr[i].frequency,
                        sig_arr[i].phase_lock ? 'y' : 'n',
                        sig_arr[i].pulse_width_ns, sig_arr[i].pulse_offset_ns, sig_arr[i].synce_output_port);
            }
        }
        if (rv) {
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        } else {
            PTP_CLI_RESULT_RETURN(CMD_OK);
        }
    } else if (parse_cmp("Set", arg, 0)) {
        parse_table_t pt;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Pin", PQ_INT, (void*)1, &signal.pin, NULL);
        parse_table_add(&pt, "Frequency", PQ_INT, (void*)1, &signal.frequency, NULL);
        parse_table_add(&pt, "PhaseLock", PQ_BOOL, (void*)1, &signal.phase_lock, NULL);
        parse_table_add(&pt, "Width", PQ_INT, (void*)1000, &signal.pulse_width_ns, NULL);
        parse_table_add(&pt, "Offset", PQ_INT, 0, &signal.pulse_offset_ns, NULL);
        parse_table_add(&pt, "OutputSynce", PQ_INT, (void*)0, &signal.synce_output_port, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        rv = bcm_ptp_signal_output_set(unit, cur_stack_id, cur_clock_num, &signal_id, &signal);

        if (rv) {
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        } else {
            PTP_CLI_RESULT_RETURN(CMD_OK);
        }
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}

/* Configures PHY port to use 4KHz SYNC_IN0, with framesync on SYNC_IN1 */
STATIC int configure_phy_sync(int unit, bcm_port_t port, uint64 ref_phase)
{
    int rv;
    bcm_port_phy_timesync_config_t config;
    uint64 load_control;
    uint64 localTime_phy;

    sal_memset(&config, 0, sizeof(config));

    config.validity_mask = BCM_PORT_PHY_TIMESYNC_VALID_FLAGS | BCM_PORT_PHY_TIMESYNC_VALID_GMODE |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_SYNC_MODE | BCM_PORT_PHY_TIMESYNC_VALID_RX_SYNC_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_TX_DELAY_REQUEST_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_RX_DELAY_REQUEST_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_FRAMESYNC_MODE | BCM_PORT_PHY_TIMESYNC_VALID_SYNCOUT_MODE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_REF_PHASE_DELTA |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K1 | BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K2 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_K3 |
        BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_DPLL_LOOP_FILTER;

    config.flags = BCM_PORT_PHY_TIMESYNC_ENABLE |
                 BCM_PORT_PHY_TIMESYNC_L2_ENABLE |
                 BCM_PORT_PHY_TIMESYNC_IP4_ENABLE |
                 BCM_PORT_PHY_TIMESYNC_IP6_ENABLE |
                 BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;

    config.gmode = bcmPortPhyTimesyncModeCpu;
    config.framesync.mode = bcmPortPhyTimesyncFramesyncSyncin1;
    config.syncout.mode = bcmPortPhyTimesyncSyncoutDisable;
    config.tx_sync_mode = bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_sync_mode = bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;
    config.tx_delay_request_mode = bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField;
    config.rx_delay_request_mode = bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField;
    /* for 4KHz SyncIn */
    config.phy_1588_dpll_ref_phase = ref_phase;
    config.phy_1588_dpll_ref_phase_delta = 250000;
    config.phy_1588_dpll_k1 = 0x2b;
    config.phy_1588_dpll_k2 = 0x26;
    config.phy_1588_dpll_k3 = 0;

    /* Initial value for the loop filter: 0x8000 0000 0000 0000 */
    COMPILER_64_SET(config.phy_1588_dpll_loop_filter, 0x80000000, 0);

    rv = bcm_port_phy_timesync_config_set(unit, port, &config);

    if (rv != BCM_E_NONE) {
       cli_out("bcm_port_phy_timesync_config_set failed with error  u=%d p=%d %s\n", unit, port, bcm_errmsg(rv));
       return rv;
    }

    /* use ref_phase as local time for now */
    localTime_phy = _bcm_ptp_llu_div(ref_phase, 16);
    rv = bcm_port_control_phy_timesync_set(unit, port, bcmPortControlPhyTimesyncLocalTime,
                                           /*ref_phase*/localTime_phy);

    if (rv != BCM_E_NONE) {
       cli_out("bcm_port_control_phy_timesync_set failed with error  u=%d p=%d %s\n", unit, port, bcm_errmsg(rv));
       return rv;
    }

    COMPILER_64_SET(load_control, 0,
                    BCM_PORT_PHY_TIMESYNC_TN_LOAD |
                    BCM_PORT_PHY_TIMESYNC_TIMECODE_LOAD |
                    BCM_PORT_PHY_TIMESYNC_SYNCOUT_LOAD |
                    BCM_PORT_PHY_TIMESYNC_LOCAL_TIME_LOAD |
                    BCM_PORT_PHY_TIMESYNC_NCO_DIVIDER_LOAD |
                    BCM_PORT_PHY_TIMESYNC_NCO_ADDEND_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_LOOP_FILTER_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_REF_PHASE_DELTA_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_K3_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_K2_LOAD |
                    BCM_PORT_PHY_TIMESYNC_DPLL_K1_LOAD );

    rv = bcm_port_control_phy_timesync_set(unit, port, bcmPortControlPhyTimesyncLoadControl, load_control);

    if (rv != BCM_E_NONE) {
       cli_out("bcm_port_control_phy_timesync_set failed with error  u=%d p=%d %s\n", unit, port, bcm_errmsg(rv));
       return rv;
    }

    return BCM_E_NONE;
}

STATIC int bcm_ptp_external_phy_sync_user_func(
    int unit, int stack_id, int clock_num,
    bcm_pbmp_t  pbm, /* external phy port bitmap e.g ge0-ge3*/
    uint64 ref_phase /* reference phase to configure */
    )
{

    int rv = BCM_E_NONE;
    soc_port_t p, dport;
    bcm_ptp_port_identity_t portid;

     if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, cur_stack_id, cur_clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_ptp_clock_port_identity_get(unit, cur_stack_id,
            cur_clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    /* coverity[overrun-local] */
    DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
        if ((rv = configure_phy_sync(unit, p, ref_phase)) != BCM_E_NONE) {
            return rv;
        }
    }

    return rv;
}

#define PTP_SYNCPHY_USAGE \
    "PTP SyncPhy                           Synchronize 1588-aware PHYs \n\t"                      \
    " PTP SyncPhy PBM=<port bitmap> [FrameSyncPin=<pin#>] [SyncPin=<pin#>] [UsePHYSYNCcb=<1/0>]\n\t"  \
    "       defaults are FrameSyncPin=3, SyncPin=-1 (-1 corresponds to BroadSync Heartbeat),UsePHYSYNCcb=0 \n"

STATIC cmd_result_t
cmd_ptp_syncphy(int unit, args_t *a)
{
    int rv;

    int freq_pin = -1;  /* -1 implies BroadSync Heartbeat */
    int UsePHYSYNCcb= 0; /* For testing 1-> use new PHY SYNC API and callback */
    int framesync_pin = 3;
    char *pbm_str = 0;
    pbmp_t pbm;
    soc_port_t p, dport;
    bcm_ptp_sync_phy_input_t sync_input;
    bcm_ptp_bs_time_info_t bshblocalTime ;
    uint64 physyncTime_nsec;
    uint8 physync_reqd = 0;
    int tmp = 0, tmp1= 0;

    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS;
    bcm_ptp_port_identity_t portid;

    bcm_ptp_external_phy_config_t extphy_config;

    parse_table_t pt;
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "SyncPin", PQ_INT, (void*)-1, &freq_pin, NULL);
    parse_table_add(&pt, "FrameSyncPin", PQ_INT, (void*)3, &framesync_pin, NULL);
    parse_table_add(&pt, "PBM", PQ_STRING, (void*)"", &pbm_str, NULL);
    parse_table_add(&pt, "UsePHYSYNCcb",PQ_INT,(void*)0,&UsePHYSYNCcb,NULL); /* added for testing PHY SYNC callback  */
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }

    rv = parse_bcm_pbmp(unit, pbm_str, &pbm);

    parse_arg_eq_done(&pt);  /* free argument string */

    if (rv < 0) {
        cli_out("Invalid port bitmap\n");
        cli_out("%s", PTP_SYNCPHY_USAGE);
        return CMD_FAIL;
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, cur_stack_id, cur_clock_num,
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        PTP_ERROR_FUNC("_bcm_ptp_function_precheck()");
        return rv;
    }

    if (BCM_FAILURE(rv = bcm_ptp_clock_port_identity_get(unit, cur_stack_id,
            cur_clock_num, PTP_IEEE1588_ALL_PORTS, &portid))) {
        PTP_ERROR_FUNC("bcm_common_ptp_clock_port_identity_get()");
        return rv;
    }

    if (UsePHYSYNCcb) {

        extphy_config.framesync_pin = framesync_pin;
        extphy_config.freq_pin = freq_pin;
        /* Adding 1.5 sec */
        COMPILER_64_ZERO(extphy_config.cpu_latency_nsec);
        COMPILER_64_ADD_32(extphy_config.cpu_latency_nsec, 1000000000);
        COMPILER_64_ADD_32(extphy_config.cpu_latency_nsec, 500000000);
        extphy_config.pbm = pbm;
        extphy_config.ext_physync_func = bcm_ptp_external_phy_sync_user_func;

        rv = bcm_ptp_external_phy_synchronize(unit, cur_stack_id, cur_clock_num, &extphy_config);
        return rv;
    }

    for (tmp = 0; tmp  <5; tmp++) {

        cli_out("PHY synchronization iteration..: %d\n", tmp);

        /* NEW API implemented to query the BSHB0/1 time when TDPLL is running */

        rv = bcm_ptp_bs_time_info_get(unit,cur_stack_id,cur_clock_num,&bshblocalTime);

        if (rv) {
            cli_out("Issue with bcm_ptp_sync_phyTime API response from TDPLL fw.....Error code:%d\n",rv);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }


        /* Perform initial framesync, so that pin is pulled low */
        sal_memset(&sync_input, 0, sizeof(bcm_ptp_sync_phy_input_t));
        sync_input.framesync_pin = framesync_pin;
        sync_input.freq_pin= freq_pin;

        /*
         * PHY synchronization.
         * Calculate reference phase to program PHYs.
         * Reference phase based on local time (i.e. when stack is queried for
         * PHY synchronization state) plus sufficient delta to guarantee stack
         * can receive and process PHY framesync at BSHB which corresponds to
         * programmed phase.
         */
        if(freq_pin == -2) { /*BSHB1 timestamp*/
            physyncTime_nsec = bshblocalTime.bshb1_time;
        } else {
            physyncTime_nsec = bshblocalTime.bshb0_time;
        }
        /* Adding 1.5 sec */
        COMPILER_64_ADD_32(physyncTime_nsec, 1000000000);
        COMPILER_64_ADD_32(physyncTime_nsec, 500000000);

        /* For 54240 PHY , REF_PHASE = {REF_PHASE_2(15:0),REF_PHASE_1(15:0)_,REF_PHASE_0(15:0)} 
         * Note : REF_PHASE is 48bits unlike LOCAL_TIME which is 44bits 
         */
        /* COMPILER_64_ZERO(sync_input.reference_time);*/
        sync_input.reference_time = physyncTime_nsec;

        /* Perform initial framesync, so that pin is pulled low */
        rv = bcm_ptp_sync_phy(unit, cur_stack_id, cur_clock_num, sync_input);
        if (rv) {
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }

        /* delay 1ms for the framesync to happen */
        sal_usleep(1000);

        /* coverity[overrun-local] */
        DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
            if (configure_phy_sync(unit, p, physyncTime_nsec) != BCM_E_NONE) {
                PTP_CLI_RESULT_RETURN(CMD_FAIL);
            }
        }

        /* Perform the actual framesync so that programmed PHY values are loaded */
        rv = bcm_ptp_sync_phy(unit, cur_stack_id, cur_clock_num, sync_input);

#ifdef COMPILER_HAS_LONGLONG
       /*Debug logs added here to avoid any latency */
        if(-2 == freq_pin) { /*BSHB1 timestamp*/
            cli_out("****BS1 selected for SYNC_IN0 ,BS1 HB Time:%lld\n",(long long)bshblocalTime.bshb1_time);
            cli_out("REF_PHASE Configured with value : %lld\n",(long long)physyncTime_nsec);
        } else { /*default BSHB0 timestamp */
            cli_out("****BS0 selected for SYNC_IN0 ,BS0 HB Time:%lld\n",(long long)bshblocalTime.bshb0_time);
           cli_out("REF_PHASE Configured with value : %lld\n",(long long)physyncTime_nsec);
        }
#endif

        for (tmp1=0; tmp1<10; tmp1++) {
            /* Query stack to determine whether PHY synchronization is required. */
            if (BCM_FAILURE(rv = _bcm_ptp_management_message_send(unit, cur_stack_id,
                    cur_clock_num, &portid, PTP_MGMTMSG_GET, PTP_MGMTMSG_ID_PHYSYNC_STATE,
                    0, 0, resp, &resp_len))) {
                PTP_ERROR_FUNC("_bcm_ptp_management_message_send()");
                return rv;
            }

            /*
             * Parse response.
             *    Octet 0...5  : Custom management message key/identifier.
             *                   BCM<null><null><null>.
             *    Octet 6      : PHY synchronization required Boolean.
             *    Octet 7      : Reserved.
             *    Octet 8...15 : Stack local time (sec).
             *    Octet 16...19: stack local time (nsec).
             */
            physync_reqd = resp[6];

           /* PHY synchronization. */
            if (physync_reqd == 3) {
                /* PHY synchronization is not required. */
                cli_out("PHY synchronization complete or not required %d\n", physync_reqd);
                PTP_CLI_RESULT_RETURN(CMD_OK);
            }
            else {
                cli_out("PHY synchronization state %d\n", physync_reqd);
                sal_sleep(1);
                continue;
            }
        }
    }

    cli_out("PHY synchronization failed ......\n");
    if (rv) {
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_OK);
    }

}


#ifndef __KERNEL__

#define PTP_TODOUT_USAGE_1\
    "PTP ToDOut [EtherNet] <setting> [OffsetNS=<offsetNS>] [OffsetSec=<offsetSec>]\n"    \
    "           [Delay=<delayNS>] [CustomFormat=<format_string>] [PPSJitter=<nsec>]\n"   \
    "           [DestMac=<dmac>] [SrcMac=<smac>] [VLAN=<vid>] [EtherType=<etype>] [BaudRate=<baudrate>]\n"  \
    "           [ToDOutPin=<pin>] EnableTimestamperSync=[0/1] [UartParity=<parity>]\n"   \
    "    where <setting> = | Off | NMEAzda | ISO8601 | NTP | UBlox |\n"       \
    "                      | ChinaMobile | ChinaTelecom | BCM | BCMTS | CUSTom | G8271\n"        \
    "    G8271 supports Serail and Ethernet. Diag shell sample code for ToD over Ethernet only\n"\
    "    supported baudrates are 300|600|1200|1800|2400|4800|9600|19200|38400|57600|115200|230400 \n"\
    "    supported uart parity NONE|ODD|EVEN \n"\
    "    Examples:\n"

#define PTP_TODOUT_USAGE_2\
    "       PTP ToDOut ISO8601 Delay=50000000 OffsetSec=1 BaudRate=9600 UartParity=NONE\n" \
    "            Configure UART output with ISO format, delayed from 1PPS edge by 50ms,\n" \
    "            and outputting the time associated with the following 1PPS edge\n" \
    "       PTP ToDOut EtherNet BCM DestMac=00:00:01:00:00:01 SrcMac=00:00:02:00:00:02 VLAN=1 EtherType=0x5006\n" \
    "            Configure Ethernet output with Broadcom binary format, with specified L2 information\n" \
    "       PTP ToDOut EtherNet off\n" \
    "            Disable Ethernet TOD output\n"

STATIC cmd_result_t
cmd_ptp_tod_out(int unit, args_t *a)
{
    parse_table_t pt;
    const char *format = ARG_GET(a);
    char *custom_format_str = 0;
    char *parity = 0;
    bcm_ptp_tod_output_t tod = {0};
    bcm_ptp_tod_source_t source = bcmPTPTODSourceSerial;
    int output_id = 1;
    int pps_out_pin = 1;
    int ts_sync_enable = 0;

    int vlan, ethertype;
    int uartNum;

    const char * nmea_str = "$GPZDA,%H%M%S.00,%d,%m,%Y,00,00*\\*\\r\\n";
    const char * iso_str = "%Y-%m-%dT%H:%M:%SZ\\r\\n";
    const char * ntp_str = "  %y %j %H:%M:%S.000  S\\r\\n";
    const char * format_str = 0;

    int arg_pps_jitter;

    if (format && parse_cmp("EtherNet", format, 0)) {
        source = bcmPTPTODSourceEthernet;
        output_id = 2;
        format = ARG_GET(a);
    }

    if (!format) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    tod.source = source;
    tod.frequency = 1;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "OffsetNS", PQ_INT, (void*)0, &tod.tod_offset_ns, NULL);
    parse_table_add(&pt, "OffsetSec", PQ_INT, (void*)0, &tod.tod_offset_src, NULL);
    parse_table_add(&pt, "Delay", PQ_INT, (void*)0, &tod.tod_delay_ns, NULL);
    parse_table_add(&pt, "CustomFormat", PQ_STRING, (void*)"", &custom_format_str, NULL);
    parse_table_add(&pt, "PPSJitter", PQ_INT, (void*)0, &arg_pps_jitter, NULL);
    parse_table_add(&pt, "DestMac", PQ_MAC | PQ_DFL, (void*)0, &tod.peer_address.raw_l2_header[0], NULL);
    parse_table_add(&pt, "SrcMac", PQ_MAC | PQ_DFL, (void*)0, &tod.peer_address.raw_l2_header[6], NULL);
    parse_table_add(&pt, "VLAN", PQ_INT, (void*)1, &vlan, NULL);
    parse_table_add(&pt, "EtherType", PQ_INT, (void*)BCM_TOD_ETHERTYPE, &ethertype, NULL);
    parse_table_add(&pt, "BaudRate", PQ_INT, (void*)0, &tod.tod_baud_rate, NULL);
    parse_table_add(&pt, "UartNum", PQ_INT, (void*)0, &uartNum, NULL);
    parse_table_add(&pt, "UartParity", PQ_STRING, (void*)"NONE", &parity, NULL);
    parse_table_add(&pt, "ToDOutPin", PQ_INT, (void*)1, &pps_out_pin, NULL);
    parse_table_add(&pt, "EnableTimestamperSync", PQ_INT, (void*)0, &ts_sync_enable, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }

    cli_out("ptp_tod_out UartNum:%d\n", uartNum);
    if ((uartNum == 0) && (bcmPTPTODSourceEthernet != source))
        tod.source = bcmPTPTODSourceSerial;
    else if ((uartNum == 1) && (bcmPTPTODSourceEthernet != source))
        tod.source = bcmPTPTODSourceSerial1;
    else if ((uartNum == 2) && (bcmPTPTODSourceEthernet != source))
        tod.source = bcmPTPTODSourceSerial2;
    else if ((uartNum == 3) && (bcmPTPTODSourceEthernet != source))
        tod.source = bcmPTPTODSourceSerial3;

    cli_out("ptp_tod_out tod.source=%d\n", tod.source);

    if(PTP_UC_FEATURE_CHECK(PTP_TOD_UART_PARITY)) {
        if (parity) {
            if (parse_cmp("NONE", parity, 0)) {
                tod.tod_parity = bcmPTPTODUartParityNone;
            } else if (parse_cmp("ODD", parity, 0)) {
                tod.tod_parity = bcmPTPTODUartParityOdd;
            } else if (parse_cmp("EVEN", parity, 0)) {
                tod.tod_parity = bcmPTPTODUartParityEven;
            }
        } else {
            tod.tod_parity = bcmPTPTODUartParityNone;
        }
        cli_out("ptp_tod_out tod.tod_parity =%d\n", tod.tod_parity);
    }

    if (parse_cmp("NMEAzda", format, 0)) {
        format_str = nmea_str;
    } else if (parse_cmp("ISO8601", format, 0)) {
        format_str = iso_str;
    } else if (parse_cmp("NTP", format, 0)) {
        format_str = ntp_str;
    } else if (parse_cmp("CUSTom", format, 0)) {
        format_str = custom_format_str;
    }

    if (source == bcmPTPTODSourceEthernet) {
        /* insert a vlan tag after the dest/src MACs */
        _bcm_ptp_uint16_write(tod.peer_address.raw_l2_header + 12, 0x8100);
        _bcm_ptp_uint16_write(tod.peer_address.raw_l2_header + 14, vlan);
        _bcm_ptp_uint16_write(tod.peer_address.raw_l2_header + 16, ethertype);
        tod.peer_address.raw_l2_header_length = 18;
    }

    tod.pps_out_pin = pps_out_pin;

    if (format_str) {
        /* we've got the format string, so use it */
        tod.format = bcmPTPTODFormatString;
        sal_strncpy((char *)tod.format_str, format_str, BCM_PTP_MAX_TOD_FORMAT_STRING - 1);
        tod.format_str_len = sal_strlen(format_str);
        if (tod.format_str_len > BCM_PTP_MAX_TOD_FORMAT_STRING) {
            tod.format_str_len = BCM_PTP_MAX_TOD_FORMAT_STRING;
        }
    } else if (parse_cmp("UBlox", format, 0)) {
        tod.format = bcmPTPTODFormatUBlox;
        tod.format_str[0] = 0x00; /* UBlox flags. */
        tod.format_str_len = 1;
    } else if (parse_cmp("ChinaMobile", format, 0)) {
        tod.format = bcmPTPTODFormatChinaTcom;
        sal_sprintf((char *)tod.format_str, "CM %05u", (uint16)arg_pps_jitter);
        tod.format_str_len = 8;
    } else if (parse_cmp("ChinaTelecom", format, 0)) {
        tod.format = bcmPTPTODFormatChinaTcom;
        sal_sprintf((char *)tod.format_str, "CT %05u", (uint16)arg_pps_jitter);
        tod.format_str_len = 8;
    } else if (parse_cmp("BCMTS", format, 0)) {
        tod.format = bcmPTPTODFormatBCMTS;
    } else if (parse_cmp("BCM", format, 0)) {
        tod.format = bcmPTPTODFormatBCM;
    } else if (parse_cmp("G8271", format, 0)) {
        tod.format = bcmPTPTODFormatG8271;
        tod.ts_sync_enable = ts_sync_enable;
    } else if (parse_cmp("Off", format, 0)) {
        PTP_IF_ERROR_RETURN(bcm_ptp_tod_output_remove(unit, cur_stack_id, cur_clock_num, tod.source));
        PTP_CLI_RESULT_RETURN(CMD_OK);
    } else {
        parse_arg_eq_done(&pt);  /* will free 'custom_format_str' */
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    parse_arg_eq_done(&pt);  /* will free 'custom_format_str' */

    PTP_IF_ERROR_RETURN(bcm_ptp_tod_output_set(unit, cur_stack_id, cur_clock_num, &output_id, &tod));

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/* PTP ToDIn */
#define PTP_TODIN_USAGE_1 \
    "PTP ToDIn [EtherNet] <setting> [OffsetNS=<offsetNS>] [OffsetSec=<offsetSec>]\n"             \
    "           [CustomFormat=<format_string>] [CustomMask=<mask_string>]\n"          \
    "           [DestMac=<dmac>] [SrcMac=<smac>] [VLAN=<vid>] [EtherType=<etype>] \n" \
    "           [EnableTimestamperSync=<0/1>] [UartParity=<parity>]\n"   \
    "    where <setting> = | Off | NMEAzda | ISO8601 | NTP | UBlox |\n"               \
    "                      | ChinaMobile | ChinaTelecom | BCM | BCMTS | CUSTom | G8271\n"   \
    "    G8271 supports Serail and Ethernet. Diag shell sample code for ToD over Ethernet only\n"\
    "    Examples:\n"

#define PTP_TODIN_USAGE_2\
    "       PTP ToDIn ISO8601 OffsetSec=1 OffsetNS=0 BaudRate=9600 UartParity=NONE\n" \
    "            Configure ToD input via UART with ISO format,\n" \
    "       PTP ToDIn EtherNet BCM DestMac=00:00:01:00:00:01 SrcMac=00:00:02:00:00:02 VLAN=1 EtherType=0x5006\n" \
    "            Configure ToD input via Ethernet with Broadcom binary format, with specified L2 information\n" \
    "       PTP ToDIn off\n" \
    "            Disable ToD input\n"

STATIC cmd_result_t
cmd_ptp_tod_in(int unit, args_t *a)
{
    parse_table_t pt;
    const char *format = ARG_GET(a);
    char *custom_format_str = 0;
    char *custom_mask_str = 0;
    char *parity = 0;
    bcm_ptp_tod_input_t tod = {0};       /* initialize to all zero */

    /* NMEA ZDA                  "$GPZDA,HHMMSS.00,DD,MM,YYYY,zz,zz*CC"*/
    const char * nmea_mask_str = "      x      xxxx  x  x    xxxxxxxxx";
    const char * nmea_fmt_str  = "$GPZDA %2H%2M%2S    %d %m %Y";

    /* ISO                       "YYYY-MM-DDTHH:MM:SS.000Z"         "*/
    const char * iso_mask_str  = "                   xxxxx";
    const char * iso_fmt_str   = "%Y-%m-%dT%H:%M:%S";

    /* NTP                       "  YY JJ HH:MM:SS.000 S"            */
    const char * ntp_mask_str  = "xx         x  x  xxxx";
    const char * ntp_fmt_str   = "  %y %j %H %M %S      ";

    const char * format_str = 0;
    const char * mask_str = 0;
 
    int vlan, ethertype;
    int uartNum;

    char * blank_str = "";
    int i, num_sources = 1;
    int ts_sync_enable = 0;

    /* By default the source is set as Serial. */
    tod.source = bcmPTPTODSourceSerial;

    if (format && parse_cmp("EtherNet", format, 0)) {
        tod.source = bcmPTPTODSourceEthernet;
        format = ARG_GET(a);
    }

    if (format && parse_cmp("Off", format, 0)) {
        tod.source = bcmPTPTODSourceNone;
    }

    if (!format) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "OffsetSec", PQ_INT, (void*)0, &tod.tod_offset_sec, NULL);
    parse_table_add(&pt, "OffsetNS", PQ_INT, (void*)0, &tod.tod_offset_ns, NULL);
    parse_table_add(&pt, "CustomFormat", PQ_STRING, blank_str, &custom_format_str, NULL);
    parse_table_add(&pt, "CustomMask", PQ_STRING, blank_str, &custom_mask_str, NULL);
    parse_table_add(&pt, "DestMac", PQ_MAC | PQ_DFL, (void*)0, &tod.peer_address.raw_l2_header[0], NULL);
    parse_table_add(&pt, "SrcMac", PQ_MAC | PQ_DFL, (void*)0, &tod.peer_address.raw_l2_header[6], NULL);
    parse_table_add(&pt, "VLAN", PQ_INT, (void*)0, &vlan, NULL);
    parse_table_add(&pt, "EtherType", PQ_INT, (void*)BCM_TOD_ETHERTYPE, &ethertype, NULL);
    parse_table_add(&pt, "BaudRate", PQ_INT, (void*)0, &tod.tod_baud_rate, NULL);
    parse_table_add(&pt, "UartNum", PQ_INT, (void*)1, &uartNum, NULL);
    parse_table_add(&pt, "UartParity", PQ_STRING, (void*)"NONE", &parity, NULL);
    parse_table_add(&pt, "EnableTimestamperSync", PQ_INT, (void*)0, &ts_sync_enable, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }

    cli_out("ptp_tod_in UartNum:%d\n", uartNum);

    if (tod.source == bcmPTPTODSourceSerial) {
        if (uartNum == 0)
            tod.source = bcmPTPTODSourceSerial;
        else if (uartNum == 1)
            tod.source = bcmPTPTODSourceSerial1;
        else if (uartNum == 2)
            tod.source = bcmPTPTODSourceSerial2;
        else if (uartNum == 3)
            tod.source = bcmPTPTODSourceSerial3;
    }

    if(PTP_UC_FEATURE_CHECK(PTP_TOD_UART_PARITY)) {
        if (parity) {
            if (parse_cmp("NONE", parity, 0)) {
                tod.tod_parity = bcmPTPTODUartParityNone;
            } else if (parse_cmp("ODD", parity, 0)) {
                tod.tod_parity = bcmPTPTODUartParityOdd;
            } else if (parse_cmp("EVEN", parity, 0)) {
                tod.tod_parity = bcmPTPTODUartParityEven;
            }
        } else {
            tod.tod_parity = bcmPTPTODUartParityNone;
        }
        cli_out("ptp_tod_in tod.tod_parity =%d\n", tod.tod_parity);
    }

    if (parse_cmp("NMEAzda", format, 0)) {
        format_str = nmea_fmt_str;
        mask_str = nmea_mask_str;
    } else if (parse_cmp("ISO8601", format, 0)) {
        format_str = iso_fmt_str;
        mask_str = iso_mask_str;
    } else if (parse_cmp("NTP", format, 0)) {
        format_str = ntp_fmt_str;
        mask_str = ntp_mask_str;
    } else if (parse_cmp("CUSTom", format, 0)) {
        format_str = custom_format_str;
        mask_str = custom_mask_str;
    }

    /* Look for a non-zero byte in the SMAC/DMAC, to indicate that we were given ethernet info */
    for (i = 0; i < 2 * sizeof(bcm_mac_t); ++i) {
        if (tod.peer_address.raw_l2_header[i] != 0) {
            tod.peer_address.raw_l2_header_length = 18;

            if (vlan) {
                /* insert a vlan tag after the dest/src MACs */
                _bcm_ptp_uint16_write(tod.peer_address.raw_l2_header + 12, 0x8100);
                _bcm_ptp_uint16_write(tod.peer_address.raw_l2_header + 14, vlan);
            }
            _bcm_ptp_uint16_write(tod.peer_address.raw_l2_header + 16, ethertype);
            break;
        }
    }

    if ( (tod.source == bcmPTPTODSourceEthernet) && 
		 (tod.peer_address.raw_l2_header_length != 18)) {
        cli_out(" Invalid Ethernet header configuration \n");
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    if (format_str) {
        /* we've got the format string, so use it */
        tod.format = bcmPTPTODFormatString;
        sal_strncpy((char *)tod.format_str, format_str, BCM_PTP_MAX_TOD_FORMAT_STRING - 1);
        sal_strncpy((char *)tod.mask_str, mask_str, BCM_PTP_MAX_TOD_FORMAT_STRING - 1);
    } else if (parse_cmp("UBlox", format, 0)) {
        tod.format = bcmPTPTODFormatUBlox;
    } else if (parse_cmp("ChinaMobile", format, 0)) {
        tod.format = bcmPTPTODFormatChinaTcom; /* China Mobile/Telecom use same enumerator. */
    } else if (parse_cmp("ChinaTelecom", format, 0)) {
        tod.format = bcmPTPTODFormatChinaTcom; /* China Mobile/Telecom use same enumerator. */
    } else if (parse_cmp("BCMTS", format, 0)) {
        tod.format = bcmPTPTODFormatBCMTS;
    } else if (parse_cmp("BCM", format, 0)) {
        tod.format = bcmPTPTODFormatBCM;
    } else if (parse_cmp("G8271", format, 0)) {
        tod.format = bcmPTPTODFormatG8271;
        tod.ts_sync_enable = ts_sync_enable;
    } else if (parse_cmp("Off", format, 0)) {
        tod.source = bcmPTPTODSourceNone;
    } else {
        parse_arg_eq_done(&pt);  /* will free 'custom_format_str' and 'custom_mask_str' */
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    parse_arg_eq_done(&pt);  /* will free 'custom_format_str' and 'custom_mask_str' */

    cli_out("ptp_tod_in tod.source=%d\n", tod.source);

    PTP_IF_ERROR_RETURN(bcm_ptp_tod_input_sources_set(unit, cur_stack_id, cur_clock_num, num_sources, &tod));

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/* PTP Debug */
#define PTP_DEBUG_USAGE \
"PTP DeBug [[+/-]<option> ...] \n\t" \
   " Set or clear debug options, +option adds to list of output, \n\t"     \
   " -option removes from list, no +/- toggles current state. \n\t"        \
   " If no options given, a current list is printed out.\n\t"              \
   " Note: Debug output is disabled until the first use of this command\n"


/* Utility function: */
STATIC cmd_result_t
diag_set_debug(int unit, bcm_ptp_stack_id_t stack_id, int clock_num, uint32 debug_mask, uint64 log_mask)
{

    PTP_IF_ERROR_RETURN(_bcm_ptp_system_log_configure(unit, stack_id, clock_num,
                                                      diag_debug_mask[unit][stack_id],
                                                      diag_log_mask[unit][stack_id],
                                                      log_src_mac_addr, log_dst_mac_addr,
                                                      log_tpid, log_vid, log_ttl, log_src_ip_addr, log_dst_ip_addr, log_port));

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


STATIC cmd_result_t
cmd_ptp_debug(int unit, args_t *a)
{
    const char * arg;
    int stack_id = 0;
    int clock_num = 0;

    int vlan_id, port_num, tpid, ttl, show_param;
    bcm_ip_t dst_ip = 0, src_ip = 0;
    bcm_mac_t blank_mac = {0};
    bcm_mac_t dst_mac = {0};
    bcm_mac_t src_mac = {0};
    
    parse_pm_t opt_list[] = {
        { "@SYs_All",          0x0000000f },
        { "SYs_Info",          0x00000001 },
        { "SYs_Warning",       0x00000002 },
        { "SYs_Error",         0x00000004 },
        { "SYs_Critical",      0x00000008 },
        { "@STack_All",        0x000000f0 },
        { "STack_Info",        0x00000010 },
        { "STack_Warning",     0x00000020 },
        { "STack_Error",       0x00000040 },
        { "STack_Critical",    0x00000080 },
        { "Servo_Timestamps",  0x00000100 },
        { "Servo_Diagnostics", 0x00000200 },
        { "Servo_Output",      0x00000400 },
#ifdef BCM_PTP_EXT_SERVO_SUPPORT
        { "Extservo_SyncErr",  0x00010000 },
        { "Extservo_SyncWarn", 0x00020000 },
        { "Extservo_SyncAnal", 0x00040000 },
        { "Extservo_Err",      0x00080000 },
        { "Extservo_Warn",     0x00100000 },
        { "Extservo_Debug",    0x00200000 },
        { "Extservo_Trace",    0x00400000 },
        { "Extservo_TimeStamps",  0x00800000 },
        { "Extservo_NonStackall", 0x00FF0000 },
#endif
        { "Servo_Channels",    0x01000000 },
        { 0, 0}
    };

    parse_table_t pt;
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DMAC", PQ_MAC, (void*)0, dst_mac, NULL);
    parse_table_add(&pt, "SMAC", PQ_MAC, (void*)0, src_mac, NULL);
    parse_table_add(&pt, "TPid", PQ_INT, (void*)0, &tpid, NULL);
    parse_table_add(&pt, "Vlan", PQ_INT, (void*)0, &vlan_id, NULL);
    parse_table_add(&pt, "DstIP", PQ_IP, (void*)0, &dst_ip, NULL);
    parse_table_add(&pt, "SrcIP", PQ_IP, (void*)0, &src_ip, NULL);
    parse_table_add(&pt, "TTL", PQ_INT, (void*)0, &ttl, NULL);
    parse_table_add(&pt, "PORTnum", PQ_INT, (void*)0, &port_num, NULL);
    parse_table_add(&pt, "SHOWparam", PQ_BOOL, (void*)0, &show_param, NULL);

    sal_dpc_cancel(INT_TO_PTR(&output_current_debug));
    output_current_debug(INT_TO_PTR(&output_current_debug), INT_TO_PTR(1000), INT_TO_PTR(0), 0, 0);

    /* In future, when multiple stacks/clocks are in use, ARG_CUR() */
    /*   can be used to peek for a "Stack=" or "Clock=" argument    */

    if (ARG_CNT(a) == 0) {
        cli_out("Debugging is enabled for:\n\n");
        parse_mask_format(55, opt_list, diag_debug_mask[unit][stack_id]);
        cli_out("Debugging is disabled for:\n\n");
        parse_mask_format(55, opt_list, ~diag_debug_mask[unit][stack_id]);
        cli_out("\n");
        cli_out("Other permissible options:  SYs_All, STack_All\n");
        PTP_CLI_RESULT_RETURN(CMD_OK);
    }

    arg = ARG_CUR(a);
    while (arg) {
        /* extract debug mask */
        if (arg[0] == '0' && arg[1] == 'x') {
            diag_debug_mask[unit][stack_id] = parse_integer((char*)arg);
        } else if (parse_mask(arg, opt_list, &diag_debug_mask[unit][stack_id])) {
            /* Not a mask arg.  Break from 'while' so other args are parsed */
            break;
        }
        ARG_NEXT(a);
        arg = ARG_CUR(a);
    }

    if (ARG_CNT(a)) {
        /* extract parameters to configure log header */
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        } else {

            if (sal_memcmp(dst_mac, blank_mac, 6)) {
                sal_memcpy(log_dst_mac_addr, dst_mac, 6);
            }
            if (sal_memcmp(src_mac, blank_mac, 6)) {
                sal_memcpy(log_src_mac_addr, src_mac, 6);
            }
            if (dst_ip) {
                log_dst_ip_addr = dst_ip;
            }
            if (src_ip) {
                log_src_ip_addr = src_ip;
            }
            if (port_num) {
                log_port = port_num;
            }
            if (tpid) {
                log_tpid = tpid;
            }
            if (vlan_id) {
                log_vid = vlan_id;
            }
            if (ttl) {
                log_ttl = ttl;
            }

            if (show_param) {
                cli_out("\n ----- Header parameters for UDP log ----- ");
                cli_out("\n Destination mac           : 0x%02x:%02x:%02x:%02x:%02x:%02x",
                           log_dst_mac_addr[0], log_dst_mac_addr[1], log_dst_mac_addr[2],
                           log_dst_mac_addr[3], log_dst_mac_addr[4], log_dst_mac_addr[5]);
                cli_out("\n source mac                : 0x%02x:%02x:%02x:%02x:%02x:%02x",
                           log_src_mac_addr[0], log_src_mac_addr[1], log_src_mac_addr[2],
                           log_src_mac_addr[3], log_src_mac_addr[4], log_src_mac_addr[5]);
                cli_out("\n Destination IP            : 0x%08x", log_dst_ip_addr);
                cli_out("\n source IP                 : 0x%08x", log_src_ip_addr);
                cli_out("\n tpid                      : 0x%04x", log_tpid);
                cli_out("\n vlan id                   : 0x%04x", log_vid);
                cli_out("\n TTL                       : %u", log_ttl);
                cli_out("\n port number               : %u", log_port);
                cli_out("\n --------------------------------------- \n");
            }
        }
    }

    return diag_set_debug(unit, stack_id, clock_num, diag_debug_mask[unit][stack_id], diag_log_mask[unit][stack_id]);
}


/* PTP Log */
#define PTP_LOG_USAGE \
"PTP Log [[+/-]<option> ...] \n\t" \
   " Set or clear UDP logging options, +option adds to list of output, \n\t"     \
   " -option removes from list, no +/- toggles current state. \n\t"        \
   " If no options given, a current list is printed out.\n"



STATIC cmd_result_t
cmd_ptp_log(int unit, args_t *a)
{
    const char * arg;
    int stack_id = 0;
    int clock_num = 0;
    uint32 upper_mask = COMPILER_64_HI(diag_log_mask[unit][stack_id]);
    uint32 lower_mask = COMPILER_64_LO(diag_log_mask[unit][stack_id]);

    parse_pm_t lower_opt_list[] = {
        { "@Log_All",         0xffffffff },
        { "Log_Debug",        0x00000001 },
        { "Log_PDV",          0x00000002 },
        { "Log_Timestamps",   0x00000004 },
        { "Log_PKT_In",       0x00000008 },
        { "Log_PKT_Out",      0x00000010 },
        { "Log_Gpio_DPLL",    0x00000020 },
        { "Log_Synth_DPLL",   0x00000040 },
        { "Log_SyncE_DPLL",   0x00000080 },
        { "Log_CTDEV",        0x00000100 },
        { "Log_FreqPhase",    0x00000200 },
        /* ... */

        { "Log_Scratch",      0x00000000 },
        { 0, 0}
    };
    parse_pm_t upper_opt_list[] = {
        { "@Log_All",         0xffffffff },
        { "Log_Debug",        0x00000000 },
        { "Log_PDV",          0x00000000 },
        { "Log_Timestamps",   0x00000000 },
        { "Log_PKT_In",       0x00000000 },
        { "Log_PKT_Out",      0x00000000 },
        { "Log_Gpio_DPLL",    0x00000000 },
        { "Log_Synth_DPLL",   0x00000000 },
        { "Log_SyncE_DPLL",   0x00000000 },
        { "Log_CTDEV",        0x00000000 },
        { "Log_FreqPhase",    0x00000000 },
        /* ... */
        { "Log_Scratch",      0x80000000 },
        { 0, 0}
    };

    /* In future, when multiple stacks/clocks are in use, ARG_CUR() */
    /*   can be used to peek for a "Stack=" or "Clock=" argument    */

    if (ARG_CNT(a) == 0) {
        cli_out("Logging is enabled for:\n\n");
        parse_mask_format(50, lower_opt_list, lower_mask);
        parse_mask_format(50, upper_opt_list, upper_mask);
        cli_out("Debugging is disabled for:\n\n");
        parse_mask_format(50, lower_opt_list, ~lower_mask);
        parse_mask_format(50, upper_opt_list, ~upper_mask);
        cli_out("\n");
        cli_out("Other permissible options:  Log_All\n");
        PTP_CLI_RESULT_RETURN(CMD_OK);
    }

    while ((arg = ARG_GET(a)) != NULL) {
        if (parse_mask(arg, lower_opt_list, &lower_mask) ||
            parse_mask(arg, upper_opt_list, &upper_mask)) {
            cli_out("debug: unknown option: %s\n", arg);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }
    }

    COMPILER_64_SET(diag_log_mask[unit][stack_id], upper_mask, lower_mask);

    return diag_set_debug(unit, stack_id, clock_num, diag_debug_mask[unit][stack_id], diag_log_mask[unit][stack_id]);
}


/* PTP Servo */

/* We divide the string because pedantic compiler doesn't allow strings longer then 509 characters */
#define PTP_SERVO_SET_USAGE \
    "\t PTP SerVO SET Frequency Correction=<corr_ppt> Offset=<offset_ppt>\n" \
    "\t PTP SerVO SET PhaseHoldover [UseFixed=<y/n>] [DeltaOffset=<offset>] [FixedOffset=<offset>] [SLEWrate=<slew>] [SYNCoffset=<offset>]\n"

#define PTP_SERVO_USAGE_1 \
    "PTP SerVO <subcommand> [<stack_id>] [<clock_num>] [...]\n"         \
    "\t PTP SerVO SHOW                 Basic servo status\n"            \
    "\t PTP SerVO SHOW IPDV            Shows IPDV metrics\n"            \
    "\t PTP SerVO SHOW PERFormance     Shows performance metrics\n"     \
    "\t PTP SerVO SHOW PhaseHoldover\n" \
    "\t PTP SerVO SHOW Frequency\n" \
    "\t PTP SerVO CONFig OSCillator_type=\"OCXO\"/\"MINIocxo\" [SERVo=\"EXTernal\"/\"BroadCoM\"]\n" \
    "\t                    [TRANSport_mode=<transport>] [FreqCorr=<offset_ppt>] [FreqCorrTS=<offset_sec>]\n"
#define PTP_SERVO_USAGE_2 \
    "\t                    [PhaseOnly=N/y] [FreeRun=N/y]\n" \
    "\t       Set/show servo configuration.  <transport>: ETHernet, SLOWethernet, HIGHJITter\n" \
    "\t PTP SerVO CLEAR PERFormance    Clears performance metrics\n"    \
    PTP_SERVO_SET_USAGE
#define PTP_SERVO_USAGE_3 \
    "\t PTP SerVO IPDVCONFig [ObservationInterval=<intv>] [Threshold=<thresh>] [PacingFactor=<pf>]\n" \
    "\t       Set/show IPDV configuration\n"                                                       \
    "\t PTP SerVO TESTmode <n>         Configures servo for test.\n"

STATIC cmd_result_t
cmd_ptp_servo_status_get(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;

    bcm_ptp_servo_status_t status;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Stack_id", PQ_INT, (void*)PTP_STACK_ID_DEFAULT, &arg_stack_id, NULL);
    parse_table_add(&pt, "Clock_num", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT, &arg_clock_num, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }

    PTP_IF_ERROR_RETURN(bcm_ptp_servo_status_get(unit, arg_stack_id, arg_clock_num, &status));

    cli_out("\n");
    cli_out("PTP Servo Status\n");
    cli_out("Unit  : %d\n", unit);
    cli_out("Stack : %d\n", arg_stack_id);
    cli_out("Clock : %d\n", arg_clock_num);
    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("\t Servo State          : %u", (unsigned)status.servo_state);
#ifdef BCM_PTP_EXT_SERVO_SUPPORT
    cli_out(" (%s)\n", diag_servo_state_description(status.servo_state));
#else
    switch (status.servo_state) {
    case bcmPTPFLLStateAcquiring :
        cli_out(" (Acquiring Lock)\n");
        break;
    case bcmPTPFLLStateWarmup :
        cli_out(" (Warmup)\n");
        break;
    case bcmPTPFLLStateFastLoop :
        cli_out(" (Fast Loop)\n");
        break;
    case bcmPTPFLLStateNormal :
        cli_out(" (Normal Loop)\n");
        break;
    case bcmPTPFLLStateBridge :
        cli_out(" (Bridge)\n");
        break;
    case bcmPTPFLLStateHoldover :
        cli_out(" (Holdover)\n");
        break;
    default :
        cli_out(" (Unknown)\n");
    }
#endif
    cli_out("\t Servo State Duration : %u (sec)\n", status.servo_state_dur);
    cli_out("\t Lock Status        : %u (%s)\n", (unsigned)status.lock_status,
        diag_servo_lock_status_description(status.lock_status));
    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("\n");

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


STATIC cmd_result_t
cmd_ptp_servo_ipdv_perf_get(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;

    _bcm_ptp_ipdv_performance_t perf;
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Stack_id", PQ_INT, (void*)PTP_STACK_ID_DEFAULT, &arg_stack_id, NULL);
    parse_table_add(&pt, "Clock_num", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT, &arg_clock_num, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }

    PTP_IF_ERROR_RETURN(_bcm_ptp_servo_ipdv_performance_get(unit, arg_stack_id, arg_clock_num, &perf));

    cli_out("\n");
    cli_out("PTP Servo IPDV Performance Data / Metrics\n");
    cli_out("Unit  : %d\n", unit);
    cli_out("Stack : %d\n", arg_stack_id);
    cli_out("Clock : %d\n", arg_clock_num);
    cli_out("--------------------------------------------------------------------------------\n");

#ifdef COMPILER_HAS_LONGLONG
    {
        uint64 decval;
        unsigned decfrac;

        decval = perf.fwd_pct / (uint64)PTP_SERVO_FWD_IPDVPCT_FIXEDPOINT_SCALEFACTOR;
        decfrac = perf.fwd_pct - (decval * (uint64)PTP_SERVO_FWD_IPDVPCT_FIXEDPOINT_SCALEFACTOR);
        cli_out("\t Forward IPDV %% Below Threshold : %llu.%03u\n", (long long unsigned)decval, decfrac);

        decval = perf.fwd_max / (uint64)PTP_SERVO_FWD_IPDVMAX_FIXEDPOINT_SCALEFACTOR;
        decfrac = perf.fwd_max - (decval * (uint64)PTP_SERVO_FWD_IPDVMAX_FIXEDPOINT_SCALEFACTOR);
        cli_out("\t Forward Maximum IPDV           : %llu.%03u (usec)\n", (long long unsigned)decval, decfrac);

        decval = perf.fwd_jitter / (uint64)PTP_SERVO_FWD_INTERPKT_JITTER_FIXEDPOINT_SCALEFACTOR;
        decfrac = perf.fwd_jitter - (decval * (uint64)PTP_SERVO_FWD_INTERPKT_JITTER_FIXEDPOINT_SCALEFACTOR);
        cli_out("\t Forward Interpacket Jitter     : %llu.%03u (usec)\n", (long long unsigned)decval, decfrac);

        decval = perf.rev_pct / (uint64)PTP_SERVO_REV_IPDVPCT_FIXEDPOINT_SCALEFACTOR;
        decfrac = perf.rev_pct - (decval * (uint64)PTP_SERVO_REV_IPDVPCT_FIXEDPOINT_SCALEFACTOR);
        cli_out("\t Reverse IPDV %% Below Threshold : %llu.%03u\n", (long long unsigned)decval, decfrac);

        decval = perf.rev_max / (uint64)PTP_SERVO_REV_IPDVMAX_FIXEDPOINT_SCALEFACTOR;
        decfrac = perf.rev_max - (decval * (uint64)PTP_SERVO_REV_IPDVMAX_FIXEDPOINT_SCALEFACTOR);
        cli_out("\t Reverse Maximum IPDV           : %llu.%03u (usec)\n", (long long unsigned)decval, decfrac);

        decval = perf.rev_jitter / (uint64)PTP_SERVO_REV_INTERPKT_JITTER_FIXEDPOINT_SCALEFACTOR;
        decfrac = perf.rev_jitter - (decval * (uint64)PTP_SERVO_REV_INTERPKT_JITTER_FIXEDPOINT_SCALEFACTOR);
        cli_out("\t Reverse Interpacket Jitter     : %llu.%03u (usec)\n", (long long unsigned)decval, decfrac);
    }
#endif /* COMPILER_HAS_LONGLONG */
    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("\n");

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


STATIC cmd_result_t
cmd_ptp_servo_perf_get(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;
    bcm_ptp_servo_config_t config;
    _bcm_ptp_servo_performance_t perf;

    PTP_IF_ERROR_RETURN(bcm_ptp_servo_configuration_get(unit, arg_stack_id, arg_clock_num, &config));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Stack_id", PQ_INT, (void*)PTP_STACK_ID_DEFAULT, &arg_stack_id, NULL);
    parse_table_add(&pt, "Clock_num", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT, &arg_clock_num, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }

    PTP_IF_ERROR_RETURN(_bcm_ptp_servo_performance_get(unit, arg_stack_id, arg_clock_num, &perf));

    cli_out("\n");
    cli_out("PTP Servo Performance Data / Metrics\n");
    cli_out("Unit  : %d\n", unit);
    cli_out("Stack : %d\n", arg_stack_id);
    cli_out("Clock : %d\n", arg_clock_num);
    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("\t Servo State                      : %u", (unsigned)perf.status.servo_state);
#ifdef BCM_PTP_EXT_SERVO_SUPPORT
    cli_out(" (%s)\n", diag_servo_state_description(perf.status.servo_state));
#else
    switch (perf.status.servo_state) {
    case bcmPTPFLLStateAcquiring :
        cli_out(" (Acquiring Lock)\n");
        break;
    case bcmPTPFLLStateWarmup :
        cli_out(" (Warmup)\n");
        break;
    case bcmPTPFLLStateFastLoop :
        cli_out(" (Fast Loop)\n");
        break;
    case bcmPTPFLLStateNormal :
        cli_out(" (Normal Loop)\n");
        break;
    case bcmPTPFLLStateBridge :
        cli_out(" (Bridge)\n");
        break;
    case bcmPTPFLLStateHoldover :
        cli_out(" (Holdover)\n");
        break;
    default :
        cli_out(" (Unknown)\n");
    }
#endif
    cli_out("\t Servo State Duration             : %u (sec)\n", perf.status.servo_state_dur);

    cli_out("\n");

#ifdef COMPILER_HAS_LONGLONG
    {
        int64 decval;
        unsigned decfrac;
#ifndef BCM_PTP_EXT_SERVO_SUPPORT
        uint64 decval_;

        if (config.servo == bcmPTPServoTypeExt)
        {
            decval_ = perf.fwd_weight /
                (uint64)PTP_SERVO_FWD_FLOW_WEIGHT_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.fwd_weight -
                (decval_ * (uint64)PTP_SERVO_FWD_FLOW_WEIGHT_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Forward Flow Weight            : %llu.%03u\n", (long long unsigned)decval_, decfrac);

            cli_out("\t Forward Flow Transient-Free    : %u (900 sec Window)\n", perf.fwd_trans_free_900);
            cli_out("\t Forward Flow Transient-Free    : %u (3600 sec Window)\n", perf.fwd_trans_free_3600);

            decval_ = perf.fwd_pct /
                (uint64)PTP_SERVO_FWD_FLOW_TRANSACTIONS_PCT_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.fwd_pct -
                (decval_ * (uint64)PTP_SERVO_FWD_FLOW_TRANSACTIONS_PCT_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Forward Flow Transactions Used : %llu.%03u (%%)\n", (long long unsigned)decval_, decfrac);

            if (perf.fwd_min_Tdev == (uint64) -1) {
                cli_out("\t Forward Flow Oper. Min TDEV    : n/a (nsec)\n");
            } else {
                decval_ = perf.fwd_min_Tdev /
                    (uint64)PTP_SERVO_FWD_MIN_TDEV_FIXEDPOINT_SCALEFACTOR;
                decfrac = perf.fwd_min_Tdev -
                    (decval_ * (uint64)PTP_SERVO_FWD_MIN_TDEV_FIXEDPOINT_SCALEFACTOR);
                cli_out("\t Forward Flow Oper. Min TDEV    : %llu.%03u (nsec)\n", (long long unsigned)decval_, decfrac);
            }
            decval_ = perf.fwd_Mafie /
                (uint64)PTP_SERVO_FWD_MAFIE_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.fwd_Mafie -
                (decval_ * (uint64)PTP_SERVO_FWD_MAFIE_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Forward Mafie                  : %llu.%03u\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.fwd_min_cluster_width /
                (uint64)PTP_SERVO_FWD_MIN_CLUSTER_WIDTH_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.fwd_min_cluster_width -
                (decval_ * (uint64)PTP_SERVO_FWD_MIN_CLUSTER_WIDTH_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Forward Flow Min Cluster Width : %llu.%03u (nsec)\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.fwd_mode_width /
                (uint64)PTP_SERVO_FWD_MODE_WIDTH_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.fwd_mode_width -
                (decval_ * (uint64)PTP_SERVO_FWD_MODE_WIDTH_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Forward Flow Mode Width        : %llu.%03u (nsec)\n", (long long unsigned)decval_, decfrac);

            cli_out("\n");

            decval_ = perf.rev_weight /
                (uint64)PTP_SERVO_REV_FLOW_WEIGHT_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.rev_weight -
                (decval_ * (uint64)PTP_SERVO_REV_FLOW_WEIGHT_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Reverse Flow Weight            : %llu.%03u\n", (long long unsigned)decval_, decfrac);

            cli_out("\t Reverse Flow Transient-Free    : %u (900 sec Window)\n", perf.rev_trans_free_900);
            cli_out("\t Reverse Flow Transient-Free    : %u (3600 sec Window)\n", perf.rev_trans_free_3600);

            decval_ = perf.rev_pct /
                (uint64)PTP_SERVO_REV_FLOW_TRANSACTIONS_PCT_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.rev_pct -
                (decval_ * (uint64)PTP_SERVO_REV_FLOW_TRANSACTIONS_PCT_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Reverse Flow Transactions Used : %llu.%03u (%%)\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.rev_min_Tdev /
                (uint64)PTP_SERVO_REV_MIN_TDEV_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.rev_min_Tdev -
                (decval_ * (uint64)PTP_SERVO_REV_MIN_TDEV_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Reverse Flow Oper. Min TDEV    : %llu.%03u (nsec)\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.rev_Mafie /
                (uint64)PTP_SERVO_REV_MAFIE_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.rev_Mafie -
                (decval_ * (uint64)PTP_SERVO_REV_MAFIE_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Reverse Mafie                  : %llu.%03u\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.rev_min_cluster_width /
                (uint64)PTP_SERVO_REV_MIN_CLUSTER_WIDTH_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.rev_min_cluster_width -
                (decval_ * (uint64)PTP_SERVO_REV_MIN_CLUSTER_WIDTH_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Reverse Flow Min Cluster Width : %llu.%03u (nsec)\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.rev_mode_width /
                (uint64)PTP_SERVO_REV_MODE_WIDTH_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.rev_mode_width -
                (decval_ * (uint64)PTP_SERVO_REV_MODE_WIDTH_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Reverse Flow Mode Width        : %llu.%03u (nsec)\n", (long long unsigned)decval_, decfrac);

            cli_out("\n");
        }
#endif
        decval = perf.freq_correction /
            (int64)PTP_SERVO_FREQ_CORRECTION_FIXEDPOINT_SCALEFACTOR;
        decfrac = (decval >= 0) ?
            (perf.freq_correction -
             (decval * (int64)PTP_SERVO_FREQ_CORRECTION_FIXEDPOINT_SCALEFACTOR)) :
            ((decval * (int64)PTP_SERVO_FREQ_CORRECTION_FIXEDPOINT_SCALEFACTOR) -
             perf.freq_correction);
        cli_out("\t Frequency Correction           : %lld.%03u (ppb)\n", (long long)decval, decfrac);

        decval = perf.phase_correction /
            (int64)PTP_SERVO_PHASE_CORRECTION_FIXEDPOINT_SCALEFACTOR;
        decfrac = (decval >= 0) ?
            (perf.phase_correction -
             (decval * (int64)PTP_SERVO_PHASE_CORRECTION_FIXEDPOINT_SCALEFACTOR)) :
            ((decval * (int64)PTP_SERVO_PHASE_CORRECTION_FIXEDPOINT_SCALEFACTOR) -
             perf.phase_correction);
        cli_out("\t Phase Correction               : %lld.%03u (nsec)\n", (long long)decval, decfrac);

        cli_out("\n");

#ifndef BCM_PTP_EXT_SERVO_SUPPORT
        if (config.servo == bcmPTPServoTypeExt)
        {
            if (perf.tdev_estimate == (uint64) -1) {
                cli_out("\t Output TDEV Estimate           : n/a (nsec)\n");
            } else {
                decval_ = perf.tdev_estimate /
                    (uint64)PTP_SERVO_TDEV_ESTIMATE_FIXEDPOINT_SCALEFACTOR;
                decfrac = perf.tdev_estimate -
                    (decval_ * (uint64)PTP_SERVO_TDEV_ESTIMATE_FIXEDPOINT_SCALEFACTOR);
                cli_out("\t Output TDEV Estimate           : %llu.%03u (nsec)\n", (long long unsigned)decval_, decfrac);
            }

            if (perf.mdev_estimate == (uint64) -1) {
                cli_out("\t Output MDEV Estimate           : n/a (ppb)\n");
            } else {
                decval_ = perf.mdev_estimate /
                    (uint64)PTP_SERVO_MDEV_ESTIMATE_FIXEDPOINT_SCALEFACTOR;
                decfrac = perf.mdev_estimate -
                    (decval_ * (uint64)PTP_SERVO_MDEV_ESTIMATE_FIXEDPOINT_SCALEFACTOR);
                cli_out("\t Output MDEV Estimate           : %llu.%03u (ppb)\n", (long long unsigned)decval_, decfrac);
            }

            cli_out("\n");
        }
        decval = perf.residual_phase_error /
            (int64)PTP_SERVO_RESIDUAL_PHASE_ERROR_FIXEDPOINT_SCALEFACTOR;
        decfrac = (decval >= 0) ?
            (perf.residual_phase_error -
             (decval * (int64)PTP_SERVO_RESIDUAL_PHASE_ERROR_FIXEDPOINT_SCALEFACTOR)) :
            ((decval * (int64)PTP_SERVO_RESIDUAL_PHASE_ERROR_FIXEDPOINT_SCALEFACTOR) -
             perf.residual_phase_error);
        cli_out("\t Residual Phase Error           : %lld.%03u (nsec)\n", (long long)decval, decfrac);

        decval_ = (int64)perf.min_round_trip_delay /
            (int64)PTP_SERVO_MIN_ROUNDTRIP_DELAY_FIXEDPOINT_SCALEFACTOR;
        decfrac = (int64)perf.min_round_trip_delay -
            (decval_ * (int64)PTP_SERVO_MIN_ROUNDTRIP_DELAY_FIXEDPOINT_SCALEFACTOR);
        cli_out("\t Min. Roundtrip Delay           : %lld.%03u (nsec)\n", (long long)decval_, decfrac);
#endif

        cli_out("\n");

        cli_out("\t Sync Packet Rate               : %u (pkts/sec)\n", perf.fwd_pkt_rate);
        cli_out("\t Delay Packet Rate              : %u (pkts/sec)\n", perf.rev_pkt_rate);

        cli_out("\n");

#ifndef BCM_PTP_EXT_SERVO_SUPPORT
        if (config.servo == bcmPTPServoTypeExt)
        {
            decval_ = perf.ipdv_data.fwd_pct /
                (uint64)PTP_SERVO_FWD_IPDVPCT_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.ipdv_data.fwd_pct -
                (decval_ * (uint64)PTP_SERVO_FWD_IPDVPCT_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Forward IPDV %% Below Threshold : %llu.%03u\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.ipdv_data.fwd_max /
                (uint64)PTP_SERVO_FWD_IPDVMAX_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.ipdv_data.fwd_max -
                (decval_ * (uint64)PTP_SERVO_FWD_IPDVMAX_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Forward Maximum IPDV           : %llu.%03u (usec)\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.ipdv_data.fwd_jitter / (uint64)PTP_SERVO_FWD_INTERPKT_JITTER_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.ipdv_data.fwd_jitter -
                (decval_ * (uint64)PTP_SERVO_FWD_INTERPKT_JITTER_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Forward Interpacket Jitter     : %llu.%03u (usec)\n", (long long unsigned)decval_, decfrac);

            cli_out("\n");

            decval_ = perf.ipdv_data.rev_pct / (uint64)PTP_SERVO_REV_IPDVPCT_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.ipdv_data.rev_pct - (decval_ * (uint64)PTP_SERVO_REV_IPDVPCT_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Reverse IPDV %% Below Threshold : %llu.%03u\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.ipdv_data.rev_max / (uint64)PTP_SERVO_REV_IPDVMAX_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.ipdv_data.rev_max - (decval_ * (uint64)PTP_SERVO_REV_IPDVMAX_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Reverse Maximum IPDV           : %llu.%03u (usec)\n", (long long unsigned)decval_, decfrac);

            decval_ = perf.ipdv_data.rev_jitter / (uint64)PTP_SERVO_REV_INTERPKT_JITTER_FIXEDPOINT_SCALEFACTOR;
            decfrac = perf.ipdv_data.rev_jitter - (decval_ * (uint64)PTP_SERVO_REV_INTERPKT_JITTER_FIXEDPOINT_SCALEFACTOR);
            cli_out("\t Reverse Interpacket Jitter     : %llu.%03u (usec)\n", (long long unsigned)decval_, decfrac);
        }
#endif
    }
#endif /* COMPILER_HAS_LONGLONG */
    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("\n");

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

STATIC cmd_result_t
cmd_ptp_servo_perf_clear(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Stack_id", PQ_INT, (void*)PTP_STACK_ID_DEFAULT, &arg_stack_id, NULL);
    parse_table_add(&pt, "Clock_num", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT, &arg_clock_num, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }

    PTP_IF_ERROR_RETURN(_bcm_ptp_servo_performance_data_clear(unit, arg_stack_id, arg_clock_num));

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


STATIC cmd_result_t
cmd_ptp_servo_ipdv_config_show(int unit, bcm_ptp_stack_id_t stack_id, int clock_num)
{
    _bcm_ptp_ipdv_config_t config;

    PTP_IF_ERROR_RETURN(_bcm_ptp_servo_ipdv_configuration_get(unit, stack_id, clock_num, &config));

    cli_out("\n");
    cli_out("PTP Servo IPDV Configuration\n");
    cli_out("Unit  : %d\n", unit);
    cli_out("Stack : %d\n", stack_id);
    cli_out("Clock : %d\n", clock_num);
    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("\t Observation Interval : %u\n", config.observation_interval);
    cli_out("\t Threshold            : %d (nsec)\n", config.threshold);
    cli_out("\t Pacing Factor        : %u\n", config.pacing_factor);
    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("\n");

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


STATIC cmd_result_t
cmd_ptp_servo_ipdv_config(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;
    int invalid_value = 0x7fffffff;
    int arg_obs_interval = 1;
    int arg_threshold = 50;
    int arg_pacing_factor = 16;
    int perform_set = 0;

    _bcm_ptp_ipdv_config_t config;
    sal_memset(&config, 0, sizeof(_bcm_ptp_ipdv_config_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Stack_id", PQ_INT, (void*)PTP_STACK_ID_DEFAULT, &arg_stack_id, NULL);
    parse_table_add(&pt, "Clock_num", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT, &arg_clock_num, NULL);
    parse_table_add(&pt, "ObservationInterval", PQ_INT, (void*)(size_t)invalid_value, &arg_obs_interval, NULL);
    parse_table_add(&pt, "Threshold", PQ_INT, (void*)(size_t)invalid_value, &arg_threshold, NULL);
    parse_table_add(&pt, "PacingFactor", PQ_INT, (void*)(size_t)invalid_value, &arg_pacing_factor, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }
    parse_arg_eq_done(&pt);

    if (arg_obs_interval != invalid_value) {
        config.observation_interval = (uint16)arg_obs_interval;
        perform_set = 1;
    }
    if (arg_threshold != invalid_value) {
        config.threshold = (int32)arg_threshold;
        perform_set = 1;
    }
    if (arg_pacing_factor != invalid_value) {
        config.pacing_factor = (uint16)arg_pacing_factor;
        perform_set = 1;
    }

    if (perform_set) {
        PTP_IF_ERROR_RETURN(_bcm_ptp_servo_ipdv_configuration_set(unit, arg_stack_id, arg_clock_num, config));
    }

    return cmd_ptp_servo_ipdv_config_show(unit, arg_stack_id, arg_clock_num);
}


STATIC cmd_result_t
cmd_ptp_servo_phase_holdover_get(int unit, args_t *a)
{
    bcm_ptp_phase_holdover_state_t state;

    PTP_IF_ERROR_RETURN(_bcm_ptp_phase_holdover_get(unit, PTP_STACK_ID_DEFAULT, PTP_CLOCK_NUMBER_DEFAULT, &state));

#ifdef COMPILER_HAS_LONGLONG
    cli_out("\n");
    cli_out("PTP Servo Phase Holdover State:\n");
    cli_out("  Reported Phase Offset: %12lld ns\n", (long long)state.reported_phase_offset);
    cli_out("  Delta Phase Offset   : %12lld ns\n", (long long)state.delta_phase_offset);
    cli_out("  Fixed Phase Offset   : %12lld ns\n", (long long)state.fixed_phase_offset);
    cli_out("  Using Fixed Offset   : %12s\n", (state.use_fixed_phase_offset) ? "Yes" : "No");
    cli_out("  Phase Slew Rate      : %12d ppb\n", (int)state.phase_slew_rate_ppb);
    cli_out("  Sync Phase Offset    : %12u ns\n", (unsigned)state.sync_phase_offset_ns);
#endif /* COMPILER_HAS_LONGLONG */

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

STATIC cmd_result_t
cmd_ptp_servo_phase_holdover_set(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_ptp_phase_holdover_state_t state;

    PTP_IF_ERROR_RETURN(_bcm_ptp_phase_holdover_get(unit, PTP_STACK_ID_DEFAULT, PTP_CLOCK_NUMBER_DEFAULT, &state));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "UseFixed", PQ_BOOL, NULL, &state.use_fixed_phase_offset, NULL);
    parse_table_add(&pt, "DeltaOffset", PQ_DFL|PQ_INT64, NULL, &state.delta_phase_offset, NULL);
    parse_table_add(&pt, "FixedOffset", PQ_DFL|PQ_INT64, NULL, &state.fixed_phase_offset, NULL);
    parse_table_add(&pt, "SLEWrate", PQ_INT, NULL, &state.phase_slew_rate_ppb, NULL);
    parse_table_add(&pt, "SYNCoffset", PQ_INT, NULL, &state.sync_phase_offset_ns, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }
    parse_arg_eq_done(&pt);

    PTP_IF_ERROR_RETURN(_bcm_ptp_phase_holdover_set(unit, PTP_STACK_ID_DEFAULT, PTP_CLOCK_NUMBER_DEFAULT, &state));

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

STATIC cmd_result_t
cmd_ptp_servo_freq_corr_get(int unit, args_t *a)
{
    int32 corr = 0, offset = 0;

    PTP_IF_ERROR_RETURN(_bcm_ptp_freq_corr_get(unit, PTP_STACK_ID_DEFAULT, PTP_CLOCK_NUMBER_DEFAULT, &corr, &offset));

    cli_out("  Servo Frequency Correction: %d ppt   Offset: %d ppt\n",
            corr, offset);

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

STATIC cmd_result_t
cmd_ptp_servo_freq_corr_set(int unit, args_t *a)
{
    parse_table_t pt;
    int32 corr = 0, offset = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Correction", PQ_INT, NULL, &corr, NULL);
    parse_table_add(&pt, "Offset", PQ_INT, NULL, &offset, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        cli_out(PTP_SERVO_SET_USAGE);
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    PTP_IF_ERROR_RETURN(_bcm_ptp_freq_corr_set(unit, PTP_STACK_ID_DEFAULT, PTP_CLOCK_NUMBER_DEFAULT, corr, offset));

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

STATIC cmd_result_t
cmd_ptp_servo_config_show(int unit, bcm_ptp_stack_id_t stack_id, int clock_num)
{
    bcm_ptp_servo_config_t config;
    int decval;
    unsigned decfrac;
    char uint64_decimal_string[27];

    PTP_IF_ERROR_RETURN(bcm_ptp_servo_configuration_get(unit, stack_id, clock_num, &config));

    cli_out("\n");
    cli_out("PTP Servo Configuration\n");
    cli_out("Unit  : %d\n", unit);
    cli_out("Stack : %d\n", stack_id);
    cli_out("Clock : %d\n", clock_num);
    cli_out("--------------------------------------------------------------------------------\n");

    cli_out("\t Servo Type                   : ");
    switch (config.servo) {
    case bcmPTPServoTypeExt:
        cli_out("External, ");
        break;
    case bcmPTPServoTypeBCM:
        cli_out("Broadcom, ");
        break;
    default:
        cli_out(" <%02d>     ", (config.servo));
        break;
    }
    if (config.flags & BCM_PTP_SERVO_PHASE_ONLY) {
        cli_out(" Phase only\n");
    } else {
        cli_out(" Freq + Phase\n");
    }

    cli_out("\t Phase holdover mode          : ");
    if (config.flags & BCM_PTP_SERVO_PHASE_HOLDOVER) {
        cli_out("Enabled\n");
    } else {
        cli_out("Not enabled\n");
    }

    if (config.flags & BCM_PTP_SERVO_LOCK_SWITCH_FREQ) {
        cli_out("\t TOP frequency assumed locked to switch freq\n");
    }
    if (config.flags & BCM_PTP_SERVO_IGNORE_FREQ) {
        cli_out("\t Servo frequency corrections being ignored (debug)\n");
    }

    cli_out("\t Oscillator Type              : %u", (unsigned)config.osc_type);
    switch (config.osc_type) {
    case bcmPTPOscTypeRB:
       cli_out(" (Rubidium)\n");
       break;
    case bcmPTPOscTypeDOCXO:
       cli_out(" (Double oven OCXO)\n");
       break;
    case bcmPTPOscTypeOCXO:
       cli_out(" (OCXO)\n");
       break;
    case bcmPTPOscTypeMiniOCXO:
       cli_out(" (Mini OCXO)\n");
       break;
    case bcmPTPOscTypeTCXO:
       cli_out(" (TCXO)\n");
       break;
    default :
       cli_out(" (Unknown)\n");
    }
    cli_out("\t PTP Transport Mode           : %u", (unsigned)config.transport_mode);
    switch (config.transport_mode) {
    case bcmPTPTransportModeEthernet :
       cli_out(" (Ethernet)\n");
       break;
    case bcmPTPTransportModeDSL:
       cli_out(" (DSL)\n");
       break;
    case bcmPTPTransportModeMicrowave:
       cli_out(" (Microwave)\n");
       break;
    case bcmPTPTransportModeSONET:
       cli_out(" (SONET)\n");
       break;
    case bcmPTPTransportModeSlowEthernet:
       cli_out(" (Slow Ethernet)\n");
       break;
    case bcmPTPTransportModeHighJitter:
       cli_out(" (High Jitter)\n");
       break;
    default :
       cli_out(" (Unknown)\n");
    }
    cli_out("\t Phase Mode                   : %u\n", config.ptp_phase_mode);

    decval = config.freq_corr /
             PTP_SERVO_OCXO_FREQ_OFFSET_FIXEDPOINT_SCALEFACTOR;
    decfrac = (decval >= 0) ?
        (config.freq_corr -
         (decval * PTP_SERVO_OCXO_FREQ_OFFSET_FIXEDPOINT_SCALEFACTOR)) :
        ((decval * PTP_SERVO_OCXO_FREQ_OFFSET_FIXEDPOINT_SCALEFACTOR) -
         config.freq_corr);
    cli_out("\t Freq. Correction             : %d (%d.%03u ppb)\n", config.freq_corr, decval, decfrac);
    format_uint64_decimal(uint64_decimal_string, config.freq_corr_time.seconds, ',');
    cli_out("\t Freq. Correction Timestamp   : %s.%09u sec.\n",
            uint64_decimal_string, config.freq_corr_time.nanoseconds);

    cli_out("\t Bridge Time                  : %u\n", config.bridge_time);
    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("\n");

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


STATIC cmd_result_t
cmd_ptp_servo_config(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;
    int invalid_value = 0x7fffffff;
    int arg_freq_corr = 0;
    int arg_freq_corr_ts = 0;
    int arg_phase_only = 0;
    int arg_phaseholdover_mode = 0;
    int arg_free_run = 0;
    int arg_lock_freq = 0;
    int got_set_data = 0;
    char * servo_type_str[3] = {"EXTernal", "BroadCoM", 0};
    char * osc_type_str[6] = {"RuBidium", "DOCXO", "OCXO", "MINIocxo", "TCXO", 0};
    char * transport_mode_str[7] = {"ETHernet", "DSL", "MicroWave", "SONET", "SLOWethernet", "HIGHJITter", 0};
    int arg_freq_only = 0;

    bcm_ptp_servo_config_t config;

    config.osc_type = invalid_value;
    config.transport_mode = bcmPTPTransportModeEthernet;
    config.ptp_phase_mode = 1;
    config.freq_corr = 0;
    COMPILER_64_SET(config.freq_corr_time.seconds, 0, 0);
    config.freq_corr_time.nanoseconds = 0;
    config.servo = 0;
    config.flags = 0;
    config.bridge_time = 300;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Stack_id", PQ_INT, (void*)PTP_STACK_ID_DEFAULT, &arg_stack_id, NULL);
    parse_table_add(&pt, "Clock_num", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT, &arg_clock_num, NULL);
    parse_table_add(&pt, "SERVo", PQ_MULTI | PQ_DFL, 0, &config.servo, servo_type_str);
    parse_table_add(&pt, "OSCillator_type", PQ_MULTI | PQ_DFL, 0, &config.osc_type, osc_type_str);
    parse_table_add(&pt, "TRANSport_mode", PQ_MULTI | PQ_DFL, 0, &config.transport_mode, transport_mode_str);
    parse_table_add(&pt, "FreqCorr", PQ_INT, (void*)(size_t)invalid_value, &arg_freq_corr, NULL);
    parse_table_add(&pt, "FreqCorrTS", PQ_INT, (void*)(size_t)invalid_value, &arg_freq_corr_ts, NULL);
    parse_table_add(&pt, "PhaseOnly", PQ_BOOL, (void*)(size_t)invalid_value, &arg_phase_only, NULL);
    parse_table_add(&pt, "FreeRun", PQ_BOOL, (void*)(size_t)invalid_value, &arg_free_run, NULL);
    parse_table_add(&pt, "LockFreq", PQ_BOOL, (void*)(size_t)invalid_value, &arg_lock_freq, NULL);
    parse_table_add(&pt, "PhaseHoldovermode", PQ_BOOL, (void*)(size_t)invalid_value, &arg_phaseholdover_mode, NULL);
    parse_table_add(&pt, "FreqOnly", PQ_BOOL, (void*)(size_t)invalid_value, &arg_freq_only, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }

    parse_arg_eq_done(&pt);

    got_set_data = (config.osc_type != invalid_value || arg_freq_corr != invalid_value || arg_freq_corr_ts != invalid_value ||
                    arg_free_run != invalid_value || arg_lock_freq != invalid_value || (arg_phaseholdover_mode != invalid_value));

    if (got_set_data) {
        if (config.osc_type == invalid_value) {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }
        if (arg_freq_corr != invalid_value) {
            config.freq_corr = arg_freq_corr;
        }
        if (arg_freq_corr_ts != invalid_value) {
            COMPILER_64_SET(config.freq_corr_time.seconds, 0, arg_freq_corr_ts);
        }
        if (arg_free_run == 1) {
            config.flags |= BCM_PTP_SERVO_IGNORE_FREQ;
        }
        if (arg_lock_freq == 1) {
            config.flags |= BCM_PTP_SERVO_LOCK_SWITCH_FREQ;
        }
        if (arg_phase_only == 1) {
            config.flags |= BCM_PTP_SERVO_PHASE_ONLY;
        }
        if (arg_phaseholdover_mode == 1) {
            config.flags |= BCM_PTP_SERVO_PHASE_HOLDOVER;
        }
        if (arg_freq_only == 1) {
            config.flags |= BCM_PTP_SERVO_FREQUENCY_ONLY;
        }
    }

    if (got_set_data) {
        PTP_IF_ERROR_RETURN(bcm_ptp_servo_configuration_set(unit, arg_stack_id, arg_clock_num, &config));
    }

    return cmd_ptp_servo_config_show(unit, arg_stack_id, arg_clock_num);
}

STATIC cmd_result_t
cmd_ptp_servo_test_mode(int unit, args_t *a)
{
    int mode;
    bcm_ptp_stack_id_t stack_id = 0;
    int clock_num = 0;
    char * arg = ARG_GET(a);
    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }
    mode = parse_integer(arg);
    diag_debug_mask[unit][stack_id] &= 0x0fffffff;
    diag_debug_mask[unit][stack_id] |= (mode << 28);

    return diag_set_debug(unit, stack_id, clock_num, diag_debug_mask[unit][stack_id], diag_log_mask[unit][stack_id]);
}

STATIC cmd_result_t
cmd_ptp_servo(int unit, args_t *a)
{
    const char *arg = ARG_GET(a);

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("SHow", arg, 0)) {
        arg = ARG_GET(a);
        if (!arg) {  /* PTP SERVO SHOW */
            return cmd_ptp_servo_status_get(unit, a);
        } else if (parse_cmp("IPDV", arg, 0)) {  /* PTP SERVO SHOW IPDV */
            return cmd_ptp_servo_ipdv_perf_get(unit, a);
        } else if (parse_cmp("PERFormance", arg, 0)) {  /* PTP SERVO SHOW PERFormance */
            return cmd_ptp_servo_perf_get(unit, a);
        } else if (parse_cmp("PhaseHoldover", arg, 0)) {  /* PTP SERVO SHOW PhaseHoldover */
            return cmd_ptp_servo_phase_holdover_get(unit, a);
        } else if (parse_cmp("Frequency", arg, 0)) {  /* PTP SERVO SHOW Frequency */
            return cmd_ptp_servo_freq_corr_get(unit, a);
#ifdef BCM_PTP_EXT_SERVO_SUPPORT
        } else if (parse_cmp("MboxStats", arg, 0)) {  /* PTP SERVO SHOW PERFormance */
            _bcm_ptp_dump_mbox_tsevent_stats();
            PTP_CLI_RESULT_RETURN(CMD_OK);
#endif
        } else {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }
    } else if (parse_cmp("CLeaR", arg, 0)) {
        arg = ARG_GET(a);
        if (!arg) {  /* PTP SERVO CLEAR */
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        } else if (parse_cmp("PERFormance", arg, 0)) {  /* PTP SERVO CLEAR PERFormance */
            return cmd_ptp_servo_perf_clear(unit, a);
        } else {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }
    } else if (parse_cmp("SET", arg, 0)) {
        arg = ARG_GET(a);
        if (!arg) {  /* PTP SERVO SET */
            cli_out(PTP_SERVO_SET_USAGE);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        } else if (parse_cmp("Frequency", arg, 0)) {  /* PTP SERVO SET FREQUENCY */
            return cmd_ptp_servo_freq_corr_set(unit, a);
        } else if (parse_cmp("PhaseHoldover", arg, 0)) {
            return cmd_ptp_servo_phase_holdover_set(unit, a);
        } else {
            cli_out(PTP_SERVO_SET_USAGE);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }
    } else if (parse_cmp("CONFig", arg, 0)) {
        return cmd_ptp_servo_config(unit, a);
    } else if (parse_cmp("IPDVCONFig", arg, 0)) {
        return cmd_ptp_servo_ipdv_config(unit, a);
    } else if (parse_cmp("TESTmode", arg, 0)) {
        return cmd_ptp_servo_test_mode(unit, a);
    }

    cli_out(PTP_SERVO_USAGE_1);
    cli_out(PTP_SERVO_USAGE_2);
    cli_out(PTP_SERVO_USAGE_3);
    PTP_CLI_RESULT_RETURN(CMD_FAIL);
}

void _ptp_find_number_of_ptp_ports(int unit, bcm_ptp_stack_id_t stack_id, int clock_num, int *num_ptp_ports)
{
    int rv=0;
    bcm_ptp_clock_info_t clock_info;

    if (BCM_FAILURE(rv = bcm_ptp_clock_get(unit, stack_id, clock_num, &clock_info))) {
        cli_out("\nbcm_ptp_clock_get() Error... (FCN RETURN= %d)\n", rv);
    }
    *num_ptp_ports = clock_info.num_ports;
}

bcm_error_t _bcm_ptp_find_slave_port (int unit, bcm_ptp_stack_id_t stack_id, int clock_num, int *ptp_slave_port) {

    bcm_ptp_port_dataset_t port_data_set;
    int i;
    int rv;
    int num_ptp_ports = 0;

    _ptp_find_number_of_ptp_ports(unit, stack_id, clock_num, &num_ptp_ports);

    for(i=1; i <= num_ptp_ports; i++) {
        if (BCM_FAILURE(rv = bcm_ptp_clock_port_dataset_get(unit, stack_id, clock_num, i, &port_data_set))) {
            cli_out("failed to get ptp_clock_port_dataset rv = %d\n", rv);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }

        if(port_data_set.port_state > _bcm_ptp_state_passive) {
            *ptp_slave_port = i;
            break;
        }
    }

    return BCM_E_NONE;
}

/* PTP Master */
#define PTP_MASTER_USAGE_1 \
    "PTP MaSTer <subcommand> [STATic[=y/n]] [Port=<port_num>] [...]\n"      \
    "\t PTP MaSTer ADD [IP|IPv6=<ip>] [MAC=<mac>]\n"                        \
    "\t                [logAnnounceInterval=<value>]\n"                     \
    "\t                [logSyncInterval=<value>]\n"                         \
    "\t                [logDelayreqInterval=<value>]\n"
#define PTP_MASTER_USAGE_2 \
    "\t                [AnnounceSERVice=y/n]\n"                             \
    "\t                [SyncSERVice=y/n]\n"                                 \
    "\t                [DelaySERVice=y/n]\n"                                \
    "\t                [SIGnalingDURation=<sec>]\n"                         \
    "\t PTP MaSTer ReMove [IP|IPv6=<ip>]\n"                                 \
    "\t PTP MaSTer SHow\n"                                                  \
    "\t PTP MaSTer SHow DetaiL\n"                                           \

STATIC cmd_result_t
cmd_ptp_master(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);
    int i;

    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;
    int arg_port_num = PTP_CLOCK_PORT_NUMBER_DEFAULT;
    bcm_mac_t arg_master_mac = {0};
    bcm_ip_t arg_master_ipv4 = 0;
    bcm_ip6_t arg_master_ipv6 = {0};
    int arg_logAnnounceInterval = PTP_CLOCK_PRESETS_LOG_ANNOUNCE_INTERVAL_DEFAULT;
    int arg_logSyncInterval = PTP_CLOCK_PRESETS_LOG_SYNC_INTERVAL_DEFAULT;
    int arg_logMinDelayReqInterval = PTP_CLOCK_PRESETS_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT;
    int arg_announce_service = 1;
    int arg_sync_service = 1;
    int arg_delay_service = 1;
    int arg_durationField = -1;
    int arg_static_master = 0;

    bcm_ptp_clock_port_info_t port_data;
    bcm_ptp_clock_unicast_master_t master;
    uint8 blank[16] = {0};

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Stack", PQ_INT, (void*)PTP_STACK_ID_DEFAULT,
                    &arg_stack_id, NULL);
    parse_table_add(&pt, "Clock", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT,
                    &arg_clock_num, NULL);
    parse_table_add(&pt, "Port", PQ_INT, (void*)PTP_CLOCK_PORT_NUMBER_DEFAULT,
                    &arg_port_num, NULL);
    parse_table_add(&pt, "MAC", PQ_MAC, (void*)0, arg_master_mac, NULL);
    parse_table_add(&pt, "IP", PQ_IP, (void*)0, &arg_master_ipv4, NULL);
    parse_table_add(&pt, "IPv6", PQ_IP6, (void*)0, &arg_master_ipv6, NULL);
    parse_table_add(&pt, "logAnnounceInterval", PQ_INT,
                    (void*)PTP_CLOCK_PRESETS_LOG_ANNOUNCE_INTERVAL_DEFAULT,
                    &arg_logAnnounceInterval, NULL);
    parse_table_add(&pt, "logSyncInterval", PQ_INT,
                    (void*)PTP_CLOCK_PRESETS_LOG_SYNC_INTERVAL_DEFAULT,
                    &arg_logSyncInterval, NULL);
    parse_table_add(&pt, "logDelayreqInterval", PQ_INT,
                    (void*)PTP_CLOCK_PRESETS_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT,
                    &arg_logMinDelayReqInterval, NULL);
    parse_table_add(&pt, "AnnounceSERVice", PQ_BOOL,
                    (void*)1, &arg_announce_service, NULL);
    parse_table_add(&pt, "SyncSERVice", PQ_BOOL,
                    (void*)1, &arg_sync_service, NULL);
    parse_table_add(&pt, "DelaySERVice", PQ_BOOL,
                    (void*)1, &arg_delay_service, NULL);
    parse_table_add(&pt, "SIGnalingDURation", PQ_INT, (void*)0, &arg_durationField, NULL);
    parse_table_add(&pt, "STATic", PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &arg_static_master, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }

    /* Get local port information. */
    PTP_IF_ERROR_RETURN(bcm_ptp_clock_port_info_get(unit, arg_stack_id, arg_clock_num,
        arg_port_num, &port_data));

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("ADD", arg, 0)) {
        if (arg_master_ipv4 == 0 && memcmp(arg_master_ipv6, blank, sizeof(bcm_ip6_t)) == 0) {
            /* no IPv4 or IPv6 address was given */
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

        /* Define master. */
        sal_memset(master.address.raw_l2_header, 0, BCM_PTP_MAX_L2_HEADER_LENGTH);
        i = 0;
        memcpy(master.address.raw_l2_header, arg_master_mac, sizeof(bcm_mac_t));
        i += sizeof(bcm_mac_t);
        memcpy(master.address.raw_l2_header + i, port_data.mac, sizeof(bcm_mac_t));
        i += sizeof(bcm_mac_t);
        if (port_data.rx_packets_vlan != 0)
        {
            master.address.raw_l2_header[i++] = 0x81;
            master.address.raw_l2_header[i++] = 0x00;
            master.address.raw_l2_header[i++] = (uint8)(port_data.rx_packets_vlan >> 8);
            master.address.raw_l2_header[i++] = (uint8)(port_data.rx_packets_vlan);
        }
        if (arg_master_ipv4) {
            /* IPv4 master. */
            master.address.raw_l2_header[i++] = 0x08;
            master.address.raw_l2_header[i++] = 0x00;
        } else {
            /* IPv6 master. */
            master.address.raw_l2_header[i++] = 0x86;
            master.address.raw_l2_header[i++] = 0xdd;
        }
        master.address.raw_l2_header_length = i;

        if (arg_master_ipv4) {
            /* IPv4 master. */
            master.address.addr_type = bcmPTPUDPIPv4;
            master.address.ipv4_addr = arg_master_ipv4;
            sal_memset(master.address.ipv6_addr, 0, sizeof(bcm_ip6_t));
        } else {
            /* IPv6 master. */
            master.address.addr_type = bcmPTPUDPIPv6;
            master.address.ipv4_addr = 0;
            sal_memcpy(master.address.ipv6_addr, arg_master_ipv6, sizeof(bcm_ip6_t));
        }

        master.log_sync_interval = arg_logSyncInterval;
        master.log_min_delay_request_interval = arg_logMinDelayReqInterval;

        if (arg_static_master) {
            /* Static master. */
            PTP_IF_ERROR_RETURN(bcm_ptp_static_unicast_master_add(unit,
                arg_stack_id, arg_clock_num, arg_port_num, &master));
        } else {
            /* Signaled master. */
            int logQueryRequest = 1;
            uint32 flags = 0;
            uint32 durationField;

            if (1 == arg_announce_service) {
                flags |= (1u << PTP_SIGNALING_ANNOUNCE_SERVICE_BIT);
            }
            if (1 == arg_sync_service) {
                flags |= (1u << PTP_SIGNALING_SYNC_SERVICE_BIT);
            }
            if (1 == arg_delay_service) {
                flags |= (1u << PTP_SIGNALING_DELAYRESP_SERVICE_BIT);
            }

            if (arg_durationField > 0) {
                /*
                 * Set global REQUEST_UNICAST_TRANSMISSION TLV durationField temporarily to apply
                 * value to new signaled master.
                 */
                PTP_IF_ERROR_RETURN(bcm_ptp_unicast_request_duration_get(unit,
                arg_stack_id, arg_clock_num, arg_port_num, &durationField));
                PTP_IF_ERROR_RETURN(bcm_ptp_unicast_request_duration_set(unit,
                arg_stack_id, arg_clock_num, arg_port_num, arg_durationField));
            }

            master.log_announce_interval = arg_logAnnounceInterval;
            master.log_query_interval = logQueryRequest;
            PTP_IF_ERROR_RETURN(bcm_ptp_signaled_unicast_master_add(unit,
                arg_stack_id, arg_clock_num, arg_port_num, &master, flags));

            if ((arg_durationField > 0) && (arg_durationField != durationField)) {
                /* Restore default global REQUEST_UNICAST_TRANSMISSION TLV durationField. */
                PTP_IF_ERROR_RETURN(bcm_ptp_unicast_request_duration_set(unit,
                arg_stack_id, arg_clock_num, arg_port_num, durationField));
            }
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("ReMove", arg, 0)) {
        /* Define attributes relevant to master removal. */
        if (arg_master_ipv4 == 0 && memcmp(arg_master_ipv6, blank, sizeof(bcm_ip6_t)) == 0) {
            /* no IPv4 or IPv6 address was given */
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

        if (arg_master_ipv4) {
            /* IPv4 master. */
            master.address.addr_type = bcmPTPUDPIPv4;
            master.address.ipv4_addr = arg_master_ipv4;
            sal_memset(master.address.ipv6_addr, 0, sizeof(bcm_ip6_t));
        } else {
            /* IPv6 master. */
            master.address.addr_type = bcmPTPUDPIPv6;
            master.address.ipv4_addr = 0;
            sal_memcpy(master.address.ipv6_addr, arg_master_ipv6, sizeof(bcm_ip6_t));
        }

        if (arg_static_master) {
            /* Static master. */
            PTP_IF_ERROR_RETURN(bcm_ptp_static_unicast_master_remove(unit,
                arg_stack_id, arg_clock_num, arg_port_num, &master.address));
        } else {
            /* Signaled master. */
            PTP_IF_ERROR_RETURN(bcm_ptp_signaled_unicast_master_remove(unit,
                arg_stack_id, arg_clock_num, arg_port_num, &master.address));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("SHow", arg, 0)) {
        int num_masters = BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES;
        bcm_ptp_clock_peer_address_t masters[BCM_PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES];
        int port = PTP_IEEE1588_ALL_PORTS;
        arg = ARG_GET(a);

        if (arg && parse_cmp("DetaiL", arg, 0)) {
            arg = ARG_GET(a);
            if (arg) {
                port = parse_integer((char*)arg);
            } else {
                /* If no clock port provided retrieve the current foreign master */
                int rv = _bcm_ptp_find_slave_port(unit, cur_stack_id, cur_clock_num, &port);
                if ((rv < 0)||(!port)) {
                    port = 1;
                }
            }
            cli_out("\n");
#ifndef __KERNEL__
            BCM_IF_ERROR_RETURN(ptp_show_foreign_master_dataset(unit, cur_stack_id, cur_clock_num, port));
#endif
            cli_out("\n");
            PTP_CLI_RESULT_RETURN(CMD_OK);
        }

        PTP_IF_ERROR_RETURN(bcm_ptp_static_unicast_master_list(unit, arg_stack_id,
            arg_clock_num, arg_port_num, PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES, &num_masters, masters));

        cli_out("\n");
        cli_out("All PTP Unicast Masters.\n");
        cli_out("Unit: %d   Stack: %d   Clock: %d   Port: %d\n",
                unit, arg_stack_id, arg_clock_num, arg_port_num);
        cli_out("----------------------------------------------------------------"
                "----------------\n");
        cli_out("Entry   IP\n");
        cli_out("----------------------------------------------------------------"
                "----------------\n");
        for (i = 0; i < num_masters; ++i) {
            cli_out("%03d     ", i);
            switch (masters[i].addr_type) {
            case bcmPTPUDPIPv4:
                cli_out("%d.%d.%d.%d\n",
                        (masters[i].ipv4_addr >> 24) & 0xff,
                        (masters[i].ipv4_addr >> 16) & 0xff,
                        (masters[i].ipv4_addr >> 8) & 0xff,
                        (masters[i].ipv4_addr) & 0xff);
                break;

            case bcmPTPUDPIPv6:
                cli_out("%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                        "%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
                        masters[i].ipv6_addr[0],
                        masters[i].ipv6_addr[1],
                        masters[i].ipv6_addr[2],
                        masters[i].ipv6_addr[3],
                        masters[i].ipv6_addr[4],
                        masters[i].ipv6_addr[5],
                        masters[i].ipv6_addr[6],
                        masters[i].ipv6_addr[7],
                        masters[i].ipv6_addr[8],
                        masters[i].ipv6_addr[9],
                        masters[i].ipv6_addr[10],
                        masters[i].ipv6_addr[11],
                        masters[i].ipv6_addr[12],
                        masters[i].ipv6_addr[13],
                        masters[i].ipv6_addr[14],
                        masters[i].ipv6_addr[15]);
                break;

            default:
                cli_out("Unknown\n");
            }
        }
        cli_out("\n");

        PTP_CLI_RESULT_RETURN(CMD_OK);

    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}


/* PTP Slave */
#define PTP_SLAVE_USAGE \
    "PTP SLaVe <subcommand> [Port=<port_num>] [...]\n"                                  \
    "\t PTP SLaVe ADD [IP|IPv6=<ip>] [MAC=<mac>]\n"                                     \
    "\t               [announceReceiptTimeout=<value>] [logAnnounceInterval=<value>]\n" \
    "\t               [logSyncInterval=<value>] [logminDelayreqInterval=<value>]\n"     \
    "\t PTP SLaVe ReMove [IP|IPv6=<ip>]\n"                                              \
    "\t PTP SLaVe SHow [STATic[=y/n]]\n"                                                \

STATIC cmd_result_t
cmd_ptp_slave(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);
    int i;

    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;
    int arg_port_num = PTP_CLOCK_PORT_NUMBER_DEFAULT;
    bcm_mac_t arg_slave_mac = {0};
    bcm_ip_t arg_slave_ipv4 = 0;
    bcm_ip6_t arg_slave_ipv6 = {0};
    int arg_announceReceiptTimeout = PTP_CLOCK_PRESETS_ANNOUNCE_RECEIPT_TIMEOUT_DEFAULT;
    int arg_logAnnounceInterval = PTP_CLOCK_PRESETS_LOG_ANNOUNCE_INTERVAL_DEFAULT;
    int arg_logSyncInterval = PTP_CLOCK_PRESETS_LOG_SYNC_INTERVAL_DEFAULT;
    int arg_logMinDelayReqInterval = PTP_CLOCK_PRESETS_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT;
    int arg_static_slave = 1;

    bcm_ptp_clock_port_info_t port_data;
    bcm_ptp_clock_peer_t slave;
    uint8 blank[16] = {0};

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Stack", PQ_INT, (void*)PTP_STACK_ID_DEFAULT,
                    &arg_stack_id, NULL);
    parse_table_add(&pt, "Clock", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT,
                    &arg_clock_num, NULL);
    parse_table_add(&pt, "Port", PQ_INT, (void*)PTP_CLOCK_PORT_NUMBER_DEFAULT,
                    &arg_port_num, NULL);
    parse_table_add(&pt, "MAC", PQ_MAC, (void*)0, arg_slave_mac, NULL);
    parse_table_add(&pt, "IP",   PQ_IP  | PQ_DFL, (void*)0, &arg_slave_ipv4, NULL);
    parse_table_add(&pt, "IPv6", PQ_IP6 | PQ_DFL, (void*)0, &arg_slave_ipv6, NULL);
    parse_table_add(&pt, "announceReceiptTimeout", PQ_INT,
                    (void*)PTP_CLOCK_PRESETS_ANNOUNCE_RECEIPT_TIMEOUT_DEFAULT,
                    &arg_announceReceiptTimeout, NULL);
    parse_table_add(&pt, "logAnnounceInterval", PQ_INT,
                    (void*)PTP_CLOCK_PRESETS_LOG_ANNOUNCE_INTERVAL_DEFAULT,
                    &arg_logAnnounceInterval, NULL);
    parse_table_add(&pt, "logSyncInterval", PQ_INT,
                    (void*)PTP_CLOCK_PRESETS_LOG_SYNC_INTERVAL_DEFAULT,
                    &arg_logSyncInterval, NULL);
    parse_table_add(&pt, "logminDelayreqInterval", PQ_INT,
                    (void*)PTP_CLOCK_PRESETS_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT,
                    &arg_logMinDelayReqInterval, NULL);
    parse_table_add(&pt, "STATic", PQ_BOOL | PQ_NO_EQ_OPT, (void*)0, &arg_static_slave, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }
    parse_arg_eq_done(&pt);

    /* Get local port information. */
    PTP_IF_ERROR_RETURN(bcm_ptp_clock_port_info_get(unit, arg_stack_id, arg_clock_num,
        arg_port_num, &port_data));

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("ADD", arg, 0)) {
        if (arg_slave_ipv4 == 0 && memcmp(arg_slave_ipv6, blank, sizeof(bcm_ip6_t)) == 0) {
            /* no IPv4 or IPv6 address was given */
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

        /* Define slave. */
        sal_memset(slave.clock_identity, 0, sizeof(bcm_ptp_clock_identity_t));
        slave.remote_port_number = (uint16)PTP_CLOCK_PORT_NUMBER_DEFAULT;
        slave.local_port_number = (uint16)arg_port_num;

        sal_memset(slave.peer_address.raw_l2_header, 0, BCM_PTP_MAX_L2_HEADER_LENGTH);
        i = 0;
        memcpy(slave.peer_address.raw_l2_header, arg_slave_mac, sizeof(bcm_mac_t));
        i += sizeof(bcm_mac_t);
        memcpy(slave.peer_address.raw_l2_header + i, port_data.mac, sizeof(bcm_mac_t));
        i += sizeof(bcm_mac_t);
        if (port_data.rx_packets_vlan != 0){
            slave.peer_address.raw_l2_header[i++] = 0x81;
            slave.peer_address.raw_l2_header[i++] = 0x00;
            slave.peer_address.raw_l2_header[i++] = (uint8)(port_data.rx_packets_vlan >> 8);
            slave.peer_address.raw_l2_header[i++] = (uint8)(port_data.rx_packets_vlan);
        }
        if (arg_slave_ipv4) {
            /* IPv4 slave. */
            slave.peer_address.raw_l2_header[i++] = 0x08;
            slave.peer_address.raw_l2_header[i++] = 0x00;
        } else {
            /* IPv6 slave. */
            slave.peer_address.raw_l2_header[i++] = 0x86;
            slave.peer_address.raw_l2_header[i++] = 0xdd;
        }
        slave.peer_address.raw_l2_header_length = i;

        if (arg_slave_ipv4) {
            /* IPv4 slave. */
            slave.peer_address.addr_type = bcmPTPUDPIPv4;
            slave.peer_address.ipv4_addr = arg_slave_ipv4;
            sal_memset(slave.peer_address.ipv6_addr, 0, sizeof(bcm_ip6_t));
        } else {
            /* IPv6 slave. */
            slave.peer_address.addr_type = bcmPTPUDPIPv6;
            slave.peer_address.ipv4_addr = 0;
            sal_memcpy(slave.peer_address.ipv6_addr, arg_slave_ipv6, sizeof(bcm_ip6_t));
        }

        slave.announce_receive_timeout = arg_announceReceiptTimeout;
        slave.log_announce_interval = arg_logAnnounceInterval;
        slave.log_sync_interval = arg_logSyncInterval;
        slave.log_delay_request_interval = arg_logMinDelayReqInterval;
        slave.log_peer_delay_request_interval = arg_logMinDelayReqInterval;

#if defined(PTP_KEYSTONE_STACK)
        slave.tx_timestamp_mech = bcmPTPToPTimestamps;
#else
        slave.tx_timestamp_mech = port_data.rx_timestamp_mechanism;
#endif
        slave.delay_mechanism = bcmPTPDelayMechanismEnd2End;

        PTP_IF_ERROR_RETURN(bcm_ptp_static_unicast_slave_add(unit, arg_stack_id,
            arg_clock_num, arg_port_num, &slave));

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("ReMove", arg, 0)) {
        /* Define attributes relevant to slave removal. */
        if (arg_slave_ipv4 == 0 && memcmp(arg_slave_ipv6, blank, sizeof(bcm_ip6_t)) == 0) {
            /* no IPv4 or IPv6 address was given */
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

        if (arg_slave_ipv4) {
            /* IPv4 slave. */
            slave.peer_address.addr_type = bcmPTPUDPIPv4;
            slave.peer_address.ipv4_addr = arg_slave_ipv4;
            sal_memset(slave.peer_address.ipv6_addr, 0, sizeof(bcm_ip6_t));
        } else {
            /* IPv6 slave. */
            slave.peer_address.addr_type = bcmPTPUDPIPv6;
            slave.peer_address.ipv4_addr = 0;
            sal_memcpy(slave.peer_address.ipv6_addr, arg_slave_ipv6, sizeof(bcm_ip6_t));
        }

        PTP_IF_ERROR_RETURN(bcm_ptp_static_unicast_slave_remove(unit, arg_stack_id,
            arg_clock_num, arg_port_num, &slave));

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("SHow", arg, 0)) {
        int num_slaves;
        bcm_ptp_clock_peer_t *slaves = sal_alloc(sizeof(bcm_ptp_clock_peer_t) * PTP_MAX_UNICAST_SLAVE_TABLE_ENTRIES, "ptpdiag");
        if (!slaves) {
            cli_out("No memory for slave list\n");
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }

        if (arg_static_slave) {
            int rv = bcm_ptp_static_unicast_slave_list(unit, arg_stack_id,
                arg_clock_num, arg_port_num, PTP_MAX_UNICAST_SLAVE_TABLE_ENTRIES, 
                &num_slaves, slaves);
            if (rv < 0) {
                sal_free(slaves);
                PTP_IF_ERROR_RETURN(rv);
            }
        } else {
            int rv = bcm_ptp_signaled_unicast_slave_list(unit, arg_stack_id,
                arg_clock_num, arg_port_num, PTP_MAX_UNICAST_SLAVE_TABLE_ENTRIES,
                &num_slaves, slaves);
            if (rv < 0) {
                sal_free(slaves);
                PTP_IF_ERROR_RETURN(rv);
            }
        }

        cli_out("\n");
        if (arg_static_slave) {
            cli_out("PTP Static Unicast Slaves.\n");
        } else {
            cli_out("PTP Signaled Unicast Slaves.\n");
        }
        cli_out("Unit: %d   Stack: %d   Clock: %d   Port: %d\n",
                unit, arg_stack_id, arg_clock_num, arg_port_num);
        cli_out("----------------------------------------------------------------"
                "----------------\n");
        cli_out("Entry   Clock Identity             IP\n");
        cli_out("----------------------------------------------------------------"
                "----------------\n");
        for (i = 0; i < num_slaves; ++i) {
            cli_out("%03d     ", i);

            cli_out("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x   ",
                    slaves[i].clock_identity[0], slaves[i].clock_identity[1], slaves[i].clock_identity[2],
                    slaves[i].clock_identity[3], slaves[i].clock_identity[4], slaves[i].clock_identity[5],
                    slaves[i].clock_identity[6], slaves[i].clock_identity[7]);

            switch (slaves[i].peer_address.addr_type) {
            case bcmPTPUDPIPv4:
                cli_out("%d.%d.%d.%d\n",
                        (slaves[i].peer_address.ipv4_addr >> 24) & 0xff,
                        (slaves[i].peer_address.ipv4_addr >> 16) & 0xff,
                        (slaves[i].peer_address.ipv4_addr >> 8) & 0xff,
                        (slaves[i].peer_address.ipv4_addr) & 0xff);
                break;

            case bcmPTPUDPIPv6:
                cli_out("%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                        "%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
                        slaves[i].peer_address.ipv6_addr[0],
                        slaves[i].peer_address.ipv6_addr[1],
                        slaves[i].peer_address.ipv6_addr[2],
                        slaves[i].peer_address.ipv6_addr[3],
                        slaves[i].peer_address.ipv6_addr[4],
                        slaves[i].peer_address.ipv6_addr[5],
                        slaves[i].peer_address.ipv6_addr[6],
                        slaves[i].peer_address.ipv6_addr[7],
                        slaves[i].peer_address.ipv6_addr[8],
                        slaves[i].peer_address.ipv6_addr[9],
                        slaves[i].peer_address.ipv6_addr[10],
                        slaves[i].peer_address.ipv6_addr[11],
                        slaves[i].peer_address.ipv6_addr[12],
                        slaves[i].peer_address.ipv6_addr[13],
                        slaves[i].peer_address.ipv6_addr[14],
                        slaves[i].peer_address.ipv6_addr[15]);
                break;

            default:
                cli_out("Unknown\n");
            }

            cli_out("        logAnnounce: %+2d   logSync: %+2d   ",
                    slaves[i].log_announce_interval, slaves[i].log_sync_interval);

            switch (slaves[i].delay_mechanism) {
            case bcmPTPDelayMechanismEnd2End:
                cli_out("logDelay: %+2d (E2E)\n", slaves[i].log_delay_request_interval);
                break;
            case bcmPTPDelayMechanismPeer2Peer:
                cli_out("logPDelay: %+2d (P2P)\n", slaves[i].log_peer_delay_request_interval);
                break;
            default:
                cli_out("logDelay: ? (Unknown)\n");
            }
        }
        cli_out("\n");

        sal_free(slaves);
        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}

/* PTP Peer Dataset */
#define PTP_PEER_DATASET_GET_USAGE \
    "PTP PeerDataset [Port=<port_num>]\n"                  \
    "\t Get PTP peer dataset.\n"

STATIC cmd_result_t
cmd_ptp_peer_dataset_get(int unit, args_t *a)
{
    int rv;

    bcm_ptp_peer_entry_t *peers;
    int num_peers;
    int max_num_peers;

    unsigned index;
    int port_num;
    parse_table_t pt;
    char uint64_decimal_string[27];

    max_num_peers = PTP_MAX_UNICAST_SLAVE_TABLE_ENTRIES +
                    PTP_MAX_MULTICAST_SLAVE_STATS_ENTRIES +
                    PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES;

    peers = sal_alloc(sizeof(bcm_ptp_peer_entry_t) * max_num_peers, "ptpdiag_peers");
    if (!peers) {
        cli_out("No memory for peer dataset list.\n");
        PTP_CLI_RESULT_RETURN(CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Port", PQ_INT, (void*) PTP_IEEE1588_ALL_PORTS, &port_num, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }
    parse_arg_eq_done(&pt);

    rv = bcm_ptp_peer_dataset_get(0, PTP_STACK_ID_DEFAULT, 
             PTP_CLOCK_NUMBER_DEFAULT, port_num, max_num_peers, peers, &num_peers);
    if (BCM_FAILURE(rv)) {
        sal_free(peers);
        PTP_IF_ERROR_RETURN(rv);
    }

    cli_out("Number of Peers: %d\n", num_peers);
    for (index = 0; index < num_peers; index++) {
        cli_out("Peer Number: %d\n", index);
        cli_out("\t Port Number : %d\n", peers[index].local_port_number);
        cli_out("\t Clock ID    : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
                peers[index].clock_identity[0],
                peers[index].clock_identity[1],
                peers[index].clock_identity[2],
                peers[index].clock_identity[3],
                peers[index].clock_identity[4],
                peers[index].clock_identity[5],
                peers[index].clock_identity[6],
                peers[index].clock_identity[7]);

        switch (peers[index].port_address.addr_type) {
        case bcmPTPUDPIPv4:
            cli_out("\t IPv4        : %d.%d.%d.%d\n",
                    peers[index].port_address.address[0], peers[index].port_address.address[1],
                    peers[index].port_address.address[2], peers[index].port_address.address[3]);
            break;
        case bcmPTPUDPIPv6:
            {
                int i;

                cli_out("\t IPv6        : %02x%02x", peers[index].port_address.address[0], peers[index].port_address.address[1]);
                for (i = 2; i < 16; i += 2) {
                    cli_out(":%02x%02x", peers[index].port_address.address[i], peers[index].port_address.address[i + 1]);
                }
                cli_out("\n");
            }
            break;
        case bcmPTPIEEE8023:
            cli_out("\t L2: %02x:%02x:%02x:%02x:%02x:%02x\n",
                    peers[index].port_address.address[0], peers[index].port_address.address[1],
                    peers[index].port_address.address[2], peers[index].port_address.address[3],
                    peers[index].port_address.address[4], peers[index].port_address.address[5]);
            break;
        default:
            cli_out("\t ERROR.  Unexpected address type (%d)\n", peers[index].port_address.addr_type);
            break;
        }
        
        format_uint64_decimal(uint64_decimal_string, peers[index].rx_announces, ',');
        cli_out("\t RxAnn    : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].rx_syncs, ',');
        cli_out("\t RxSync   : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].rx_followups, ',');
        cli_out("\t RxFlwup  : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].rejected, ',');
        cli_out("\t RxRej    : %s\n", uint64_decimal_string);

        format_uint64_decimal(uint64_decimal_string, peers[index].rx_delayreqs, ',');
        cli_out("\t RxDlyRq  : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].rx_delayresps, ',');
        cli_out("\t RxDlyRsp : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].rx_mgmts, ',');
        cli_out("\t RxMgmt   : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].rx_signals, ',');
        cli_out("\t RxSig    : %s\n", uint64_decimal_string);

        format_uint64_decimal(uint64_decimal_string, peers[index].tx_announces, ',');
        cli_out("\t TxAnn    : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].tx_syncs, ',');
        cli_out("\t TxSync   : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].tx_followups, ',');
        cli_out("\t TxFlwup  : %s\n", uint64_decimal_string);

        format_uint64_decimal(uint64_decimal_string, peers[index].tx_delayreqs, ',');
        cli_out("\t TxDlyRq  : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].tx_delayresps, ',');
        cli_out("\t TxDlyRsp : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].tx_mgmts, ',');
        cli_out("\t TxMgmt   : %s\n", uint64_decimal_string);
        format_uint64_decimal(uint64_decimal_string, peers[index].tx_signals, ',');
        cli_out("\t TxSig    : %s\n", uint64_decimal_string);
        {
            int port = peers[index].phy_port;
            if (SOC_IS_QUX(unit) || SOC_IS_QAX(unit) || SOC_IS_QMX(unit)) {
                int trunk_id=0, mod_id=0, port_id=0;
                ptp_parse_sysport_to_modport(unit, port, &trunk_id, &mod_id, &port_id);
                cli_out("\t LagId    : %d\n", trunk_id);
                cli_out("\t Mod Num  : %d\n", mod_id);
                cli_out("\t Phy Port : %d\n", port_id);
            } else {
                cli_out("\t Mod Num  : %u\n", BCM_GPORT_MODPORT_MODID_GET(port));
                cli_out("\t Phy Port : %u\n", BCM_GPORT_MODPORT_PORT_GET(port));
            }
        }
    }

    sal_free(peers);
    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/* PTP Counters */
#define PTP_PACKET_COUNTERS_GET_USAGE \
    "PTP CounTeRs CLeaR [Port=<port_num>] [CounterBitMaP=<counter_bitmap>]\n"      \
    "\t Get or Clear PTP packet counters.\n"

STATIC cmd_result_t
cmd_ptp_packet_counters_get(int unit, args_t *a)
{
    parse_table_t pt;
    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;
    int arg_port_num = BCM_PTP_IEEE1588_ALL_PORTS;
    int arg_ctr_bitmap = BCM_PTP_PKT_COUNTER_ALL;

    bcm_ptp_packet_counters_t counters;
    int i;
    _bcm_ptp_clock_cache_t *clock;
    int num_ports, ports_per_column;
    const char *arg = ARG_GET(a);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Stack", PQ_INT, (void*)PTP_STACK_ID_DEFAULT, &arg_stack_id, NULL);
    parse_table_add(&pt, "Clock", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT, &arg_clock_num, NULL);
    parse_table_add(&pt, "Port", PQ_INT, (void*)-1, &arg_port_num, NULL);
    parse_table_add(&pt, "CounterBitMaP", PQ_INT, (void*)-1, &arg_ctr_bitmap, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
    }

    if (arg && parse_cmp("CLeaR", arg, 0)) {
        if ((arg_port_num == -1) && (arg_ctr_bitmap == -1)) {
			/* ptp ctr clear */
			arg_ctr_bitmap = BCM_PTP_PKT_COUNTER_ALL;
        } else if ((arg_port_num != -1) && (arg_ctr_bitmap == -1)) {
			/* ptp ctr clear port=<> */
			arg_ctr_bitmap = BCM_PTP_PKT_COUNTER_PORT_SPECIFIC;
        } else {
			/* ptp ctr clear CounterBitMaP=< > */
			/* ptp ctr clear Port=< > CounterBitMaP=< > */
            arg_ctr_bitmap &= BCM_PTP_PKT_COUNTER_ALL; /* Mask unused bits */
            if (arg_ctr_bitmap == BCM_PTP_PKT_COUNTER_ALL ||
                arg_ctr_bitmap <= BCM_PTP_PKT_COUNTER_COMMON) {
                /* To clear any/all common counters or all counters, port_num shall be 0xFFFF */
                arg_port_num = PTP_IEEE1588_ALL_PORTS;
            }
        }
        cli_out("Unit[%d]  Stack[%d]  Clock[%d]  Port[%d]  CounterBitmap[0x%04X]\n", unit, arg_stack_id, arg_clock_num, arg_port_num, arg_ctr_bitmap);
        PTP_IF_ERROR_RETURN(bcm_ptp_packet_counters_clear(unit, arg_stack_id, arg_clock_num, arg_port_num, arg_ctr_bitmap));
    } else {
      PTP_IF_ERROR_RETURN(bcm_ptp_packet_counters_get(unit, arg_stack_id, arg_clock_num, &counters));

      clock = &_bcm_common_ptp_unit_array[unit].stack_array[arg_stack_id].clock_array[arg_clock_num];
      num_ports = clock->clock_info.num_ports;
      if (num_ports > 6) {
        ports_per_column = (num_ports + 1) / 2;  /* round up */
      } else {
        ports_per_column = num_ports;
      }

      cli_out("Unit: %d   Stack: %d   Clock: %d\n", unit, arg_stack_id, arg_clock_num);
      cli_out("--------------------------------------------------------------------------------\n");
      cli_out("  Tx: %-9u    Rx: %-9u   Disc: %-9u  RCPU: %-9u\n",
              counters.packets_transmitted, counters.packets_received,
              counters.packets_discarded, counters.rcpu_encap_packets_received);
      cli_out("IPv4: %-9u  IPv6: %-9u  L2PTP: %-9u   UDP: %-9u\n",
              counters.ipv4_packets_received, counters.ipv6_packets_received,
              counters.l2_ptp_packets_received, counters.udp_ptp_packets_received);
      cli_out("ESTx: %-9u  ESRx: %-9u  RxOvf: %-9u\n",
              counters.enduro_sync_packets_transmitted, counters.enduro_sync_packets_received,
              counters.rx_queue_overflows);
      cli_out("\n");

      for (i = 0; i < ports_per_column; i += 1) {
          cli_out("   [Port %2d]  Tx: %-9u   Rx: %-9u   Disc: %-9u", i+1,
                  counters.port_packets_transmitted[i], counters.port_packets_received[i],
                  counters.port_packets_discarded[i]);
          if (i + ports_per_column < num_ports) {
              cli_out("   [Port %2d]  Tx: %-9u   Rx: %-9u   Disc: %-9u\n", i+1+ports_per_column,
                    counters.port_packets_transmitted[i+ports_per_column],
                    counters.port_packets_received[i+ports_per_column],
                    counters.port_packets_discarded[i+ports_per_column]);
          } else {
              cli_out("\n");
          }
      }
      cli_out("--------------------------------------------------------------------------------\n");
      cli_out("\n");
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


/* PTP REGISTER ARP/SIGNAL/EVENT/... */
#define PTP_REGISTER_USAGE \
    "PTP REGister [Signal] [Management] [Events] [ARP]\n"                   \
    "\t Register a diag-shell handler for callbacks on signaling messages, management messages, clock events, or ARPing.\n"

STATIC cmd_result_t
cmd_ptp_register(int unit, args_t *a)
{
    const char *arg = ARG_GET(a);

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    while (arg) {
        if (parse_cmp("ARP", arg, 0)) {
            _bcm_ptp_arp_callback = &diag_arp_callback;
            PTP_IF_ERROR_RETURN(_bcm_ptp_tunnel_arp_set(unit, PTP_STACK_ID_DEFAULT, 1));
        } else if (parse_cmp("Signal", arg, 0)) {
            PTP_IF_ERROR_RETURN(bcm_ptp_signaling_arbiter_register(
               unit, PTP_STACK_ID_DEFAULT, PTP_CLOCK_NUMBER_DEFAULT,
               diag_signaling_arbiter, 0));
        } else if (parse_cmp("SignalDeny", arg, 0)) {
            PTP_IF_ERROR_RETURN(bcm_ptp_signaling_arbiter_register(
                unit, PTP_STACK_ID_DEFAULT, PTP_CLOCK_NUMBER_DEFAULT,
                diag_signaling_arbiter, INT_TO_PTR(ARBITER_DENY_FLAG)));
        } else if (parse_cmp("Management", arg, 0)) {
            bcm_ptp_cb_types_t cb_mask = {{0}};
            SHR_BITSET(cb_mask.w, bcmPTPCallbackTypeManagement);
            PTP_IF_ERROR_RETURN(bcm_ptp_cb_register(unit, cb_mask, (bcm_ptp_cb)diag_management_callback, 0));
        } else if (parse_cmp("Events", arg, 0)) {
            bcm_ptp_cb_types_t cb_mask = {{0}};
            SHR_BITSET(cb_mask.w, bcmPTPCallbackTypeEvent);
            PTP_IF_ERROR_RETURN(bcm_ptp_cb_register(unit, cb_mask, (bcm_ptp_cb)diag_event_callback, 0));
        } else {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }
        arg = ARG_GET(a);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


/* PTP TELecom */
/* We divide the string because pedantic compiler doesn't allow strings longer then 509 characters */
#define PTP_TELECOM_USAGE_1 \
    "PTP TELecom <subcommand> [...]\n"                                               \
    "\t PTP TELecom CLOCK CREATE  [ID=<xx:xx:xx:xx:xx:xx:xx:xx>]\n"                  \
    "\t                           [QL=<ITU-T G.781 QL>] [DOMain=<value>]\n"          \
    "\t                           [SlaveOnly=y/n] [OneWay=y/n]\n"                    \
    "\t PTP TELecom PORT CONFig   [IP=<ip>] [MAC=<mac>]\n"                           \
    "\t                           [VLAN=<value>] [PRIO=<value>]\n"                   \
    "\t                           [TimeStamp=<|ToP|RCPU|PHY|Unimac32|Unimac48|>]\n"  \
    "\t PTP TELecom SLaVe CONFig  [ENable=<|DISable|OPTionI|OPTionII|OPTionIII|>]\n" \
    "\t                           "
#define PTP_TELECOM_USAGE_2 \
                                 "[SyncReceiptTimeout=<msec>]\n"                     \
    "\t                           [DelayReceiptTimeout=<msec>]\n"                    \
    "\t                           [VARianceTHRESHold=<nsec_sq>]\n"                   \
    "\t PTP TELecom SLaVe SHow\n"                                                    \
    "\t PTP TELecom MaSTer CONFig [IP=<ip>] [PRIOrity=<value>]\n"                    \
    "\t                           [OVeRride=y/n] [LoCKout=y/n]\n"                    \
    "\t                           [NONREVersion=y/n]\n"                              \
    "\t                           [WAIT_to_restore=<sec>]\n"                         \
    "\t PTP TELecom MaSTer SHow   [DeTaiLs=y/n]\n"                                   \
    "\t PTP TELecom MaSTer BeST   [DeTaiLs=y/n]\n"                                   \
    "\t PTP TELecom OPTions       [VERBose=y/n]\n"

STATIC cmd_result_t
cmd_ptp_telecom(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);
    int i;

    bcm_ptp_stack_id_t arg_stack_id = PTP_STACK_ID_DEFAULT;
    int arg_clock_num = PTP_CLOCK_NUMBER_DEFAULT;

    char *arg_clockIdentity;
    char *id_token;
    char *id_rem;

    char *arg_QL;
    int arg_domainNumber = PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_DEFAULT;
    int arg_slaveOnly = 0;
    int arg_oneWay = 0;

    bcm_ip_t arg_port_ip = (192 << 24) + (168 << 16) + 31;
    bcm_mac_t arg_port_mac = {0};
    int arg_port_vlan = 1;
    int arg_port_prio = 0;
    char *arg_timestamp_mechanism = 0;

    char *arg_telecom_enable;
    int arg_syncReceiptTimeout = PTP_TELECOM_SYNC_RECEIPT_TIMEOUT_MSEC_DEFAULT;
    int arg_delayReceiptTimeout = PTP_TELECOM_DELAYRESP_RECEIPT_TIMEOUT_MSEC_DEFAULT;
    uint64 arg_varianceThreshold;

    bcm_ip_t arg_pktmaster_ipv4 = 0;
    int arg_pktmaster_priority = 0;
    int arg_pktmaster_override = 0;
    int arg_pktmaster_lockout = 0;
    int arg_pktmaster_non_reversion = PTP_TELECOM_NON_REVERSION_MODE_DEFAULT;
    uint64 arg_pktmaster_wait_sec;

    int arg_details = 0;
    int arg_verbose = 1;

    bcm_ptp_telecom_g8265_network_option_t network_option;
    bcm_ptp_telecom_g8265_quality_level_t QL;
    bcm_ptp_clock_port_address_t address;

    uint32 slaveOnly;
    uint32 receiptTimeout;
    bcm_ptp_telecom_g8265_pktstats_t thresholds;

    int num_masters;
    int best_master;
    bcm_ptp_telecom_g8265_pktmaster_t pktmaster[PTP_TELECOM_MAX_NUMBER_PKTMASTERS];
    char *tokstr =NULL;
    char uint64_decimal_string[27];

    COMPILER_64_SET(arg_pktmaster_wait_sec, 0, PTP_TELECOM_WAIT_TO_RESTORE_SEC_DEFAULT);

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("CLOCK", arg, 0)) {
        bcm_ptp_clock_info_t clockInfo;
        clockInfo.flags = (1u << PTP_CLOCK_FLAGS_TELECOM_PROFILE_BIT);
        clockInfo.clock_num = PTP_CLOCK_NUMBER_DEFAULT;
        clockInfo.clock_identity[0] = 0x00;
        clockInfo.clock_identity[1] = 0x00;
        clockInfo.clock_identity[2] = 0x00;
        clockInfo.clock_identity[3] = 0xff;
        clockInfo.clock_identity[4] = 0xfe;
        clockInfo.clock_identity[5] = 0x00;
        clockInfo.clock_identity[6] = 0x00;
        clockInfo.clock_identity[7] = 0x00;
        clockInfo.type = bcmPTPClockTypeOrdinary;
        clockInfo.num_ports = 1;
        clockInfo.clock_class = 108; /* QL-PROV, ITU-T G.781 network option II clockClass. */
        clockInfo.domain_number = PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_DEFAULT;
        clockInfo.scaled_log_variance = 1;
        clockInfo.priority1 = PTP_CLOCK_PRESETS_PRIORITY1_DEFAULT;
        clockInfo.priority2 = PTP_CLOCK_PRESETS_PRIORITY2_DEFAULT;
        clockInfo.slaveonly = arg_slaveOnly;
        clockInfo.twostep = 1;
        clockInfo.tc_primary_domain = PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_DEFAULT;
        clockInfo.tc_delay_mechanism = bcmPTPDelayMechanismEnd2End;
        clockInfo.announce_receipt_timeout_minimum =
                PTP_CLOCK_PRESETS_ANNOUNCE_RECEIPT_TIMEOUT_MINIMUM;
        clockInfo.announce_receipt_timeout_default =
                PTP_CLOCK_PRESETS_ANNOUNCE_RECEIPT_TIMEOUT_DEFAULT;
        clockInfo.announce_receipt_timeout_maximum =
                PTP_CLOCK_PRESETS_ANNOUNCE_RECEIPT_TIMEOUT_MAXIMUM;
        clockInfo.log_announce_interval_minimum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_ANNOUNCE_INTERVAL_MINIMUM;
        clockInfo.log_announce_interval_default =
                PTP_CLOCK_PRESETS_TELECOM_LOG_ANNOUNCE_INTERVAL_DEFAULT;
        clockInfo.log_announce_interval_maximum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_ANNOUNCE_INTERVAL_MAXIMUM;
        clockInfo.log_sync_interval_minimum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_SYNC_INTERVAL_MINIMUM;
        clockInfo.log_sync_interval_default =
                PTP_CLOCK_PRESETS_TELECOM_LOG_SYNC_INTERVAL_DEFAULT;
        clockInfo.log_sync_interval_maximum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_SYNC_INTERVAL_MAXIMUM;
        clockInfo.log_min_delay_req_interval_minimum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_MIN_DELAY_REQ_INTERVAL_MINIMUM;
        clockInfo.log_min_delay_req_interval_default =
                PTP_CLOCK_PRESETS_TELECOM_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT;
        clockInfo.log_min_delay_req_interval_maximum =
                PTP_CLOCK_PRESETS_TELECOM_LOG_MIN_DELAY_REQ_INTERVAL_MAXIMUM;
        clockInfo.domain_number_minimum =
                PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_MINIMUM;
        clockInfo.domain_number_default =
                PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_DEFAULT;
        clockInfo.domain_number_maximum =
                PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_MAXIMUM;
        clockInfo.priority1_minimum = PTP_CLOCK_PRESETS_PRIORITY1_MINIMUM;
        clockInfo.priority1_default = PTP_CLOCK_PRESETS_PRIORITY1_DEFAULT;
        clockInfo.priority1_maximum = PTP_CLOCK_PRESETS_PRIORITY1_MAXIMUM;
        clockInfo.priority2_minimum = PTP_CLOCK_PRESETS_PRIORITY2_MINIMUM;
        clockInfo.priority2_default = PTP_CLOCK_PRESETS_PRIORITY2_DEFAULT;
        clockInfo.priority2_maximum = PTP_CLOCK_PRESETS_PRIORITY2_MAXIMUM;
        clockInfo.number_virtual_interfaces = 0;

        arg = ARG_GET(a);
        if (!arg) {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        } else if (parse_cmp("CREATE", arg, 0)) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Stack", PQ_INT, (void*)PTP_STACK_ID_DEFAULT,
                            &arg_stack_id, NULL);
            parse_table_add(&pt, "Clock", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT,
                            &arg_clock_num, NULL);
            parse_table_add(&pt, "ID", PQ_STRING, (void*)"00:00:00:ff:fe:00:00:00",
                            &arg_clockIdentity, NULL);
            parse_table_add(&pt, "QL", PQ_STRING, (void*)"QL-PROV",
                            &arg_QL, NULL);
            parse_table_add(&pt, "DOMain", PQ_INT, (void*)PTP_CLOCK_PRESETS_TELECOM_DOMAIN_NUMBER_DEFAULT,
                            &arg_domainNumber, NULL);
            parse_table_add(&pt, "SlaveOnly", PQ_BOOL, (void*)0, 
                            &arg_slaveOnly, NULL);
            parse_table_add(&pt, "OneWay", PQ_BOOL, (void*)0,
                            &arg_oneWay, NULL);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
            }

            if (1 == arg_slaveOnly) {
                QL = bcm_ptp_telecom_g8265_ql_na_slv;
            } else if (arg_QL) {
                if (parse_cmp("QL-PRC", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_I_prc;
                } else if (parse_cmp("QL-SSU-A", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_I_ssua;
                } else if (parse_cmp("QL-SSU-B", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_I_ssub;
                } else if (parse_cmp("QL-SEC", arg_QL, 0)) {
                    
                    QL = bcm_ptp_telecom_g8265_ql_I_sec;
                    /*
                    switch (network_option) {
                    case bcm_ptp_telecom_g8265_network_option_I:
                        QL = bcm_ptp_telecom_g8265_ql_I_sec;
                        break;
                    case bcm_ptp_telecom_g8265_network_option_III:
                        QL = bcm_ptp_telecom_g8265_ql_III_sec;
                        break;
                    default:
                        QL = bcm_ptp_telecom_g8265_ql_invalid;
                    }
                     */
                } else if (parse_cmp("QL-DNU", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_I_dnu;
                } else if (parse_cmp("QL-PRS", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_II_prs;
                } else if (parse_cmp("QL-STU", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_II_stu;
                } else if (parse_cmp("QL-ST2", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_II_st2;
                } else if (parse_cmp("QL-TNC", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_II_tnc;
                } else if (parse_cmp("QL-ST3E", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_II_st3e;
                } else if (parse_cmp("QL-ST3", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_II_st3;
                } else if (parse_cmp("QL-SMC", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_II_smc;
                } else if (parse_cmp("QL-PROV", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_II_prov;
                } else if (parse_cmp("QL-DUS", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_II_dus;
                } else if (parse_cmp("QL-UNK", arg_QL, 0)) {
                    QL = bcm_ptp_telecom_g8265_ql_III_unk;
                } else {
                    QL = bcm_ptp_telecom_g8265_ql_invalid;
                }
            } else {
                QL = bcm_ptp_telecom_g8265_ql_invalid;
            }

            /*
             * Create telecom-compliant PTP clock (OC).
             * Set configurable members of clock info.
             */
            if (1 == arg_oneWay) {
                clockInfo.flags |= (1u << PTP_CLOCK_FLAGS_ONEWAY_BIT);
            }

            clockInfo.clock_num = arg_clock_num;

            id_token = sal_strtok_r(arg_clockIdentity, ":", &tokstr);
            i = 0;
            while ((id_token != NULL) && (i < BCM_PTP_CLOCK_EUID_IEEE1588_SIZE)) {
                clockInfo.clock_identity[i++] = strtol(id_token, &id_rem, 16);
                id_token = sal_strtok_r(NULL, ":", &tokstr);
            }
            if (i != BCM_PTP_CLOCK_EUID_IEEE1588_SIZE) {
                cli_out("Error: Invalid clockIdentity format\n");
                cli_out("\n");
                parse_arg_eq_done(&pt);
                PTP_CLI_RESULT_RETURN(CMD_FAIL);
            }
            parse_arg_eq_done(&pt);

            PTP_IF_ERROR_RETURN(_bcm_ptp_telecom_g8265_map_QL_clockClass(QL, &clockInfo.clock_class));
            clockInfo.domain_number = arg_domainNumber;

            clockInfo.slaveonly = arg_slaveOnly;

            PTP_IF_ERROR_RETURN(bcm_ptp_clock_create(unit, arg_stack_id, &clockInfo));
            
            PTP_CLI_RESULT_RETURN(CMD_OK);

        } else {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

    } else if (parse_cmp("PORT", arg, 0)) {
        
        bcm_ptp_clock_port_info_t portInfo;

        portInfo.port_address.addr_type = bcmPTPUDPIPv4;
        sal_memset(portInfo.port_address.address, 0, sizeof(portInfo.port_address.address));
        sal_memset(portInfo.mac, 0, sizeof(portInfo.mac));
        portInfo.multicast_l2_size = 0;
        sal_memset(portInfo.multicast_l2, 0, sizeof(portInfo.multicast_l2));
        portInfo.multicast_pdelay_l2_size = 0;
        sal_memset(portInfo.multicast_pdelay_l2, 0, sizeof(portInfo.multicast_pdelay_l2));
        portInfo.multicast_tx_enable = 0;
        portInfo.port_type = bcmPTPPortTypeStandard;
        portInfo.announce_receipt_timeout =
                PTP_CLOCK_PRESETS_ANNOUNCE_RECEIPT_TIMEOUT_DEFAULT;
        portInfo.log_announce_interval =
                PTP_CLOCK_PRESETS_TELECOM_LOG_ANNOUNCE_INTERVAL_DEFAULT;
        portInfo.log_sync_interval =
                PTP_CLOCK_PRESETS_TELECOM_LOG_SYNC_INTERVAL_DEFAULT;
        portInfo.log_min_delay_req_interval =
                PTP_CLOCK_PRESETS_TELECOM_LOG_MIN_DELAY_REQ_INTERVAL_DEFAULT;
        portInfo.delay_mechanism = bcmPTPDelayMechanismEnd2End;
        portInfo.rx_timestamp_mechanism = bcmPTPMac32CorrectionTimestamps;
        portInfo.rx_packets_vlan = (arg_port_vlan + (arg_port_prio << 13));
        portInfo.rx_packets_port_mask_high32 = 0;
        portInfo.rx_packets_port_mask_low32 = 0;
        portInfo.rx_packets_criteria_mask = 0;

        arg = ARG_GET(a);
        if (!arg) {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        } else if (parse_cmp("CONFig", arg, 0)) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Stack", PQ_INT, (void*)PTP_STACK_ID_DEFAULT,
                            &arg_stack_id, NULL);
            parse_table_add(&pt, "Clock", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT,
                            &arg_clock_num, NULL);
            parse_table_add(&pt, "IP", PQ_IP, (void*)(size_t)arg_port_ip, 
                            &arg_port_ip, NULL);
            parse_table_add(&pt, "MAC", PQ_MAC, (void*)arg_port_mac,
                            &arg_port_mac, NULL);
            parse_table_add(&pt, "VLAN", PQ_INT, (void*)1,
                            &arg_port_vlan, NULL);
            parse_table_add(&pt, "PRIO", PQ_INT, (void*)0,
                            &arg_port_prio, NULL);
            parse_table_add(&pt, "TimeStamp", PQ_STRING, (void*)"",
                            &arg_timestamp_mechanism, NULL);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
            }

            /* Configure PTP port. */
            portInfo.port_address.address[0] = (uint8)(arg_port_ip >> 24);
            portInfo.port_address.address[1] = (uint8)(arg_port_ip >> 16);
            portInfo.port_address.address[2] = (uint8)(arg_port_ip >> 8);
            portInfo.port_address.address[3] = (uint8)(arg_port_ip);
            
            sal_memcpy(portInfo.mac, arg_port_mac, sizeof(bcm_mac_t));
            portInfo.rx_packets_vlan = (arg_port_vlan + (arg_port_prio << 13));

#if defined(PTP_KEYSTONE_STACK)
            portInfo.rx_timestamp_mechanism = bcmPTPToPTimestamps;
#else
            portInfo.rx_timestamp_mechanism = bcmPTPMac32CorrectionTimestamps;
#endif

            if (arg_timestamp_mechanism) {
                if (parse_cmp("ToP", arg_timestamp_mechanism, 0)) {
                    portInfo.rx_timestamp_mechanism = bcmPTPToPTimestamps;
                } else if (parse_cmp("RCPU", arg_timestamp_mechanism, 0)) {
                    portInfo.rx_timestamp_mechanism = bcmPTPRCPUTimestamps;
                } else if (parse_cmp("PHY", arg_timestamp_mechanism, 0)) {
                    portInfo.rx_timestamp_mechanism = bcmPTPPhyCorrectionTimestamps;
                } else if (parse_cmp("Unimac32", arg_timestamp_mechanism, 0)) {
                    portInfo.rx_timestamp_mechanism = bcmPTPMac32CorrectionTimestamps;
                } else if (parse_cmp("Unimac48", arg_timestamp_mechanism, 0)) {
                    portInfo.rx_timestamp_mechanism = bcmPTPMac48CorrectionTimestamps;
                } 

            }
            parse_arg_eq_done(&pt);

            PTP_IF_ERROR_RETURN(bcm_ptp_clock_port_configure(unit, arg_stack_id, 
                arg_clock_num, PTP_CLOCK_PORT_NUMBER_DEFAULT, &portInfo));

            PTP_CLI_RESULT_RETURN(CMD_OK);

        } else {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

    } else if (parse_cmp("SLaVe", arg, 0)) {
        /* Pre-check(s). */
        PTP_IF_ERROR_RETURN(bcm_ptp_clock_slaveonly_get(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, &slaveOnly));
        if (0 == slaveOnly) {
            cli_out("PTP TELECOM SLAVE commands applicable to slaveOnly clock.\n");
            cli_out("\n");
            PTP_IF_ERROR_RETURN(BCM_E_UNAVAIL);
        }

        arg = ARG_GET(a);
        if (!arg) {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        } else if (parse_cmp("CONFig", arg, 0)) {
            /* Defaults. */
            COMPILER_64_SET(arg_varianceThreshold, 0, PTP_TELECOM_PDV_SCALED_ALLAN_VARIANCE_THRESHOLD_NSECSQ_DEFAULT);

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Stack", PQ_INT, (void*)PTP_STACK_ID_DEFAULT,
                            &arg_stack_id, NULL);
            parse_table_add(&pt, "Clock", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT,
                            &arg_clock_num, NULL);
            parse_table_add(&pt, "ENable", PQ_STRING, (void*)"DISable",
                            &arg_telecom_enable, NULL);
            parse_table_add(&pt, "SyncReceiptTimeout", PQ_INT,
                            (void*)PTP_TELECOM_SYNC_RECEIPT_TIMEOUT_MSEC_DEFAULT,
                            &arg_syncReceiptTimeout, NULL);
            parse_table_add(&pt, "DelayReceiptTimeout", PQ_INT,
                            (void*)PTP_TELECOM_DELAYRESP_RECEIPT_TIMEOUT_MSEC_DEFAULT,
                            &arg_delayReceiptTimeout, NULL);
            parse_table_add(&pt, "VARianceTHRESHold", PQ_DFL|PQ_INT64,
                            (void*)PTP_TELECOM_PDV_SCALED_ALLAN_VARIANCE_THRESHOLD_NSECSQ_DEFAULT,
                            &arg_varianceThreshold, NULL);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
            }

            if (parse_cmp("DISable", arg_telecom_enable, 0)) {
                PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_network_option_set(unit,
                    arg_stack_id, arg_clock_num, bcm_ptp_telecom_g8265_network_option_disable));
                PTP_CLI_RESULT_RETURN(CMD_OK);
            } else if (parse_cmp("OPTionI", arg_telecom_enable, 0)) {
                network_option = bcm_ptp_telecom_g8265_network_option_I;
            } else if (parse_cmp("OPTionII", arg_telecom_enable, 0)) {
                network_option = bcm_ptp_telecom_g8265_network_option_II;
            } else if (parse_cmp("OPTionIII", arg_telecom_enable, 0)) {
                network_option = bcm_ptp_telecom_g8265_network_option_III;
            } else {
                parse_arg_eq_done(&pt);
                PTP_CLI_RESULT_RETURN(CMD_USAGE);
            }
            parse_arg_eq_done(&pt);

            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_network_option_set(unit,
                arg_stack_id, arg_clock_num, network_option));
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_quality_level_set(unit,
                arg_stack_id, arg_clock_num, bcm_ptp_telecom_g8265_ql_na_slv));

            /* Packet slave configuration settings. */
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_receipt_timeout_set(unit,
                arg_stack_id, arg_clock_num, bcmPTP_MESSAGE_TYPE_SYNC, (uint32)arg_syncReceiptTimeout));
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_receipt_timeout_set(unit,
                arg_stack_id, arg_clock_num, bcmPTP_MESSAGE_TYPE_DELAY_RESP, (uint32)arg_delayReceiptTimeout));

            COMPILER_64_SET(thresholds.pdv_scaled_allan_var,
                COMPILER_64_HI(arg_varianceThreshold),
                COMPILER_64_LO(arg_varianceThreshold));

            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_pktstats_thresholds_set(unit,
                arg_stack_id, arg_clock_num, thresholds));

            PTP_CLI_RESULT_RETURN(CMD_OK);

        } else if (parse_cmp("SHow", arg, 0)) {
            cli_out("\n");
            cli_out("Telecom Profile Slave Configuration (Control Parameters).\n");
            cli_out("Unit: %d   Stack: %d   Clock: %d\n",
                    unit, arg_stack_id, arg_clock_num);
            cli_out("-----------------------------------------------------------"
                    "---------------------\n");
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_network_option_get(unit, arg_stack_id,
                arg_clock_num, &network_option));
            switch (network_option) {
            case (bcm_ptp_telecom_g8265_network_option_I):
                cli_out("ITU-T G.781 network option : Option I\n");
                break;
            case (bcm_ptp_telecom_g8265_network_option_II):
                cli_out("ITU-T G.781 network option : Option II\n");
                break;
            case (bcm_ptp_telecom_g8265_network_option_III):
                cli_out("ITU-T G.781 network option : Option III\n");
                break;
            default:
                cli_out("ITU-T G.781 network option : <Invalid>\n");
            }
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_receipt_timeout_get(unit, arg_stack_id,
                arg_clock_num, bcmPTP_MESSAGE_TYPE_SYNC, &receiptTimeout));
            cli_out("Sync Timeout               : %u (msec)\n", receiptTimeout);
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_receipt_timeout_get(unit, arg_stack_id,
                arg_clock_num, bcmPTP_MESSAGE_TYPE_DELAY_RESP, &receiptTimeout));
            cli_out("Delay_Resp Timeout         : %u (msec)\n", receiptTimeout);
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_pktstats_thresholds_get(unit, arg_stack_id,
                arg_clock_num, &thresholds));
            format_uint64_decimal(uint64_decimal_string, thresholds.pdv_scaled_allan_var, ',');
            cli_out("Scaled AVAR Threshold      : %s (nsec-sq)\n", uint64_decimal_string);

            PTP_CLI_RESULT_RETURN(CMD_OK);

        } else {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

    } else if (parse_cmp("MaSTer", arg, 0)) {
        /* Pre-check(s). */
        PTP_IF_ERROR_RETURN(bcm_ptp_clock_slaveonly_get(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, &slaveOnly));
        if (0 == slaveOnly) {
            cli_out("PTP TELECOM MASTER commands applicable to slaveOnly clock.\n");
            cli_out("\n");
            PTP_IF_ERROR_RETURN(BCM_E_UNAVAIL);
        }

        arg = ARG_GET(a);
        if (!arg) {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        } else if (parse_cmp("CONFig", arg, 0)) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Stack", PQ_INT, (void*)PTP_STACK_ID_DEFAULT,
                            &arg_stack_id, NULL);
            parse_table_add(&pt, "Clock", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT,
                            &arg_clock_num, NULL);
            parse_table_add(&pt, "IP",   PQ_IP  | PQ_DFL, (void*)0, &arg_pktmaster_ipv4, NULL);
            parse_table_add(&pt, "PRIOrity", PQ_INT, (void*)0, &arg_pktmaster_priority, NULL);
            parse_table_add(&pt, "OVeRride", PQ_BOOL, (void*)0, &arg_pktmaster_override, NULL);
            parse_table_add(&pt, "LoCKout", PQ_BOOL, (void*)0, &arg_pktmaster_lockout, NULL);
            parse_table_add(&pt, "NONREVersion", PQ_BOOL,
                            (void*)PTP_TELECOM_NON_REVERSION_MODE_DEFAULT,
                            &arg_pktmaster_non_reversion, NULL);
            parse_table_add(&pt, "WAIT_to_restore", PQ_DFL|PQ_INT64,
                            (void*)PTP_TELECOM_WAIT_TO_RESTORE_SEC_DEFAULT,
                            &arg_pktmaster_wait_sec, NULL);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
            }
            parse_arg_eq_done(&pt);

            address.addr_type = bcmPTPUDPIPv4;
            sal_memset(address.address, 0, BCM_PTP_MAX_NETW_ADDR_SIZE);
            address.address[0] = ((arg_pktmaster_ipv4 >> 24) & 0xff);
            address.address[1] = ((arg_pktmaster_ipv4 >> 16) & 0xff);
            address.address[2] = ((arg_pktmaster_ipv4 >> 8) & 0xff);
            address.address[3] = (arg_pktmaster_ipv4 & 0xff);

            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_packet_master_priority_set(
                unit, arg_stack_id, arg_clock_num, (uint16)arg_pktmaster_priority,
                &address));
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_packet_master_priority_override(
                unit, arg_stack_id, arg_clock_num, (uint8)arg_pktmaster_override,
                &address));
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_packet_master_lockout_set(
                unit, arg_stack_id, arg_clock_num, (uint8)arg_pktmaster_lockout,
                &address));
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_packet_master_non_reversion_set(
                unit, arg_stack_id, arg_clock_num, (uint8)arg_pktmaster_non_reversion,
                &address));
            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_packet_master_wait_duration_set(
                unit, arg_stack_id, arg_clock_num, arg_pktmaster_wait_sec,
                &address));

            PTP_CLI_RESULT_RETURN(CMD_OK);

        } else if (parse_cmp("SHow", arg, 0)) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Stack", PQ_INT, (void*)PTP_STACK_ID_DEFAULT,
                            &arg_stack_id, NULL);
            parse_table_add(&pt, "Clock", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT,
                            &arg_clock_num, NULL);
            parse_table_add(&pt, "DeTaiLs", PQ_BOOL, (void*)0, &arg_details, NULL);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
            }
            parse_arg_eq_done(&pt);

            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_packet_master_list(unit,
                arg_stack_id, arg_clock_num, PTP_TELECOM_MAX_NUMBER_PKTMASTERS,
                &num_masters, &best_master, pktmaster));

            cli_out("\n");
            cli_out("Telecom Profile Packet Masters.\n");
            cli_out("Unit: %d   Stack: %d   Clock: %d\n",
                    unit, arg_stack_id, arg_clock_num);
            cli_out("-----------------------------------------------------------"
                    "---------------------\n");
            cli_out("Number Masters  : %u\n", num_masters);
            if (num_masters) {
                cli_out("Best Master     : %u\n", best_master);
            } else {
                cli_out("Best Master     :\n");
            }
            for (i = 0; i < num_masters; ++i) {
                cli_out("------------------------------------- [%02d] ----------"
                        "---------------------------\n", i);
                diag_telecom_pktmaster_printout(&pktmaster[i], (uint8)arg_details);
            }
            cli_out("-----------------------------------------------------------"
                    "---------------------\n");

            PTP_CLI_RESULT_RETURN(CMD_OK);

        } else if (parse_cmp("BeST", arg, 0)) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Stack", PQ_INT, (void*)PTP_STACK_ID_DEFAULT,
                            &arg_stack_id, NULL);
            parse_table_add(&pt, "Clock", PQ_INT, (void*)PTP_CLOCK_NUMBER_DEFAULT,
                            &arg_clock_num, NULL);
            parse_table_add(&pt, "DeTaiLs", PQ_BOOL, (void*)0, &arg_details, NULL);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
            }
            parse_arg_eq_done(&pt);

            PTP_IF_ERROR_RETURN(bcm_ptp_telecom_g8265_packet_master_best_get(unit,
                arg_stack_id, arg_clock_num, pktmaster));

            cli_out("\n");
            cli_out("Telecom Profile Best Packet Master.\n");
            cli_out("Unit: %d   Stack: %d   Clock: %d\n",
                    unit, arg_stack_id, arg_clock_num);
            cli_out("-----------------------------------------------------------"
                    "---------------------\n");
            diag_telecom_pktmaster_printout(pktmaster, (uint8)arg_details);
            cli_out("-----------------------------------------------------------"
                    "---------------------\n");

            PTP_CLI_RESULT_RETURN(CMD_OK);

        } else {
            PTP_CLI_RESULT_RETURN(CMD_USAGE);
        }

    } else if (parse_cmp("OPTions", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "VERBose", PQ_BOOL, (void*)1, &arg_verbose, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);
 
        /* Set telecom profile verbose level (ON/OFF only at present). */
        if (arg_verbose) {
            PTP_IF_ERROR_RETURN(_bcm_ptp_telecom_g8265_verbose_level_set((uint32)-1));
        } else {
            PTP_IF_ERROR_RETURN(_bcm_ptp_telecom_g8265_verbose_level_set(0));
        }
 
        PTP_CLI_RESULT_RETURN(CMD_OK);

    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}


/* PTP CTDEV */
#define PTP_CTDEV_USAGE                                                            \
    "PTP CTDEV <subcommand> [...]\n"                                               \
    "\t PTP CTDEV OPTions [ENable=y/n] [VERBose=y/n]\n"                            \
    "\t                   [ALPHA_Numerator=<value>] [ALPHA_Denominator=<value>]\n"

STATIC cmd_result_t
cmd_ptp_ctdev(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);
    int arg_enable;
    int arg_verbose;
    int arg_alpha_numerator;
    int arg_alpha_denominator;

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("OPTions", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "ENable", PQ_BOOL, (void*)1, &arg_enable, NULL);
        parse_table_add(&pt, "VERBose", PQ_BOOL, (void*)1, &arg_verbose, NULL);
        parse_table_add(&pt, "ALPHA_Numerator", PQ_INT, (void*)63, &arg_alpha_numerator, NULL);
        parse_table_add(&pt, "ALPHA_Denominator", PQ_INT, (void*)64, &arg_alpha_denominator, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        PTP_IF_ERROR_RETURN(bcm_ptp_ctdev_enable_set(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, arg_enable, 0));
        PTP_IF_ERROR_RETURN(bcm_ptp_ctdev_verbose_set(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, arg_verbose));
        PTP_IF_ERROR_RETURN(bcm_ptp_ctdev_alpha_set(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, arg_alpha_numerator, arg_alpha_denominator));

        if (arg_enable) {
            /* Register reference alarm callback function. */
            PTP_IF_ERROR_RETURN(bcm_ptp_ctdev_alarm_callback_register(unit, PTP_STACK_ID_DEFAULT,
                PTP_CLOCK_NUMBER_DEFAULT, diag_ctdev_alarm_callback));
        } else {
            /* Unregister reference alarm callback function. */
            PTP_IF_ERROR_RETURN(bcm_ptp_ctdev_alarm_callback_unregister(unit, PTP_STACK_ID_DEFAULT,
                PTP_CLOCK_NUMBER_DEFAULT));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}


/* PTP MODULAR ... */
#define PTP_MODULAR_USAGE                                                              \
    "PTP MODULAR <subcommand> [...]\n"                                                 \
    "\t PTP MODULAR GET\n"                                                             \
    "\t PTP MODULAR SET [ENable=y/n] [VERBose=y/n]\n"                                  \
    "\t                 [PHYTS=y/n] [FrameSyncPIN=<pin>] [PBM=<port bitmap>]\n"

STATIC cmd_result_t
cmd_ptp_modular(int unit, args_t *a)
{
    int rv;

    parse_table_t pt;
    const char *arg = ARG_GET(a);
    int arg_enable;
    uint32 flags;
    int arg_verbose;
    int arg_phyts;
    int arg_framesync_pin;
    char *arg_pbmp_str = 0;
    bcm_pbmp_t arg_pbmp;

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("GET", arg, 0)) {
        PTP_IF_ERROR_RETURN(bcm_ptp_modular_enable_get(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, &arg_enable, &flags));
        PTP_IF_ERROR_RETURN(bcm_ptp_modular_verbose_get(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, &arg_verbose));
        PTP_IF_ERROR_RETURN(bcm_ptp_modular_phyts_get(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, &arg_phyts, &arg_framesync_pin));

        cli_out("\n");
        cli_out("Modular System (Control Parameters and Options).\n");
        cli_out("Unit: %d   Stack: %d   Clock: %d\n",
                unit, PTP_STACK_ID_DEFAULT, PTP_CLOCK_NUMBER_DEFAULT);
        cli_out("-----------------------------------------------------------"
                "---------------------\n");
        cli_out("Enable : %s\n", arg_enable ? "Y":"N");
        cli_out("Verbose: %s\n", arg_verbose ? "Y":"N");
        cli_out("PHY TS : %s\n", arg_phyts ? "Y":"N");

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("SET", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "ENable", PQ_BOOL, (void*)1, &arg_enable, NULL);
        parse_table_add(&pt, "VERBose", PQ_BOOL, (void*)1, &arg_verbose, NULL);
        parse_table_add(&pt, "PHYTS", PQ_BOOL, (void*)0, &arg_phyts, NULL);
        parse_table_add(&pt, "FrameSyncPIN", PQ_INT, (void*)3, &arg_framesync_pin, NULL);
        parse_table_add(&pt, "PBM", PQ_STRING, (void*)"", &arg_pbmp_str, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }

        rv = parse_bcm_pbmp(unit, arg_pbmp_str, &arg_pbmp);
        parse_arg_eq_done(&pt);

        if (BCM_E_NONE == rv) {
            /* Valid port bitmap. Update port bitmap for PHY synchronization. */
            PTP_IF_ERROR_RETURN(bcm_ptp_modular_portbitmap_set(unit, PTP_STACK_ID_DEFAULT,
                PTP_CLOCK_NUMBER_DEFAULT, arg_pbmp));
        }

        PTP_IF_ERROR_RETURN(bcm_ptp_modular_enable_set(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, arg_enable, 0));
        PTP_IF_ERROR_RETURN(bcm_ptp_modular_verbose_set(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, arg_verbose));
        PTP_IF_ERROR_RETURN(bcm_ptp_modular_phyts_set(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, arg_phyts, arg_framesync_pin));

        PTP_CLI_RESULT_RETURN(CMD_OK);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}

FILE * volatile tsfifo_fp = NULL;

int timestamp_cb(int unit, void *user_data, uint32 *cb_data_ptr, bcm_time_capture_cb_type_t status)
{
    /* Sample handling of retreived timestamp FIFO entries */
    uint16 num_entries = 0;

    num_entries = *cb_data_ptr;

    if (tsfifo_fp) {
        if (sal_fwrite(cb_data_ptr, 4, 1, tsfifo_fp) != 1) {
            cli_out("\nError writing to file\n");
        }
        num_entries = soc_ntohl(*cb_data_ptr);
        /* cli_out("\n num_entries[%d : %08x]", num_entries, *cb_data_ptr);*/
        cb_data_ptr++;
        while(num_entries-- > 0) {
            if (sal_fwrite(cb_data_ptr, 4, 1, tsfifo_fp) != 1) {
                cli_out("\n Error writing to file\n");
            }
            /* cli_out("\n word_1[%08x]", *cb_data_ptr); */
            cb_data_ptr++;
            if (sal_fwrite(cb_data_ptr, 4, 1, tsfifo_fp) != 1) {
                cli_out("\nError writing to file\n");
            }
            /* cli_out("\t word_2[%08x]\n", *cb_data_ptr); */
            cb_data_ptr++;
        }
    }
    return BCM_E_NONE;
}

STATIC int
bcm_time_ts_fifo_capture_start(int unit)
{
    int rv = 0;
    bcm_time_capture_cb_type_t cb_type;

    if(!tsfifo_fp) {
        tsfifo_fp = sal_fopen("Test/ts_fifo.log", "w+");
        if (!tsfifo_fp) {
            cli_out("\n Error: Unable to open file\n");
            return BCM_E_FAIL;
        } else {
            cli_out("\n File opened \n");
        }
    }
    cb_type = bcmTimeCapturetFill;
    rv = bcm_time_capture_cb_register (unit, 0, cb_type, timestamp_cb, NULL);
    return rv;
}

STATIC int
bcm_time_ts_fifo_capture_stop(int unit)
{
    int rv = 0;
    bcm_time_capture_cb_type_t cb_type;

    cb_type = bcmTimeCapturetFill;
    rv = bcm_time_capture_cb_unregister (unit, 0, cb_type);

    if (tsfifo_fp) {
        sal_fclose((FILE *)tsfifo_fp);
        cli_out("\n Closing file \n");
        tsfifo_fp = NULL;
    }
    return rv;
}

/* PTP TsFifo */
#define PTP_TSFIFO_USAGE \
    "PTP TsFifo <subcommand> [...]\n"       \
    "\t PTP TsFifo TimeInit\n" \
    "\t PTP TsFifo CaptureSTART\n" \
    "\t PTP TsFifo CaptureSTOP\n"

STATIC cmd_result_t
cmd_ptp_ts_fifo(int unit, args_t *a)
{
    int rv=0;
    const char *arg = ARG_GET(a);

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("TimeInit", arg, 0)) {
        rv = bcm_time_init(unit);
        cli_out("\n bcm_time_init returned [%d]", rv);
        PTP_CLI_RESULT_RETURN(CMD_OK);
    } else if (parse_cmp("CaptureSTART", arg, 0)) {
        bcm_time_ts_fifo_capture_start(unit);
        PTP_CLI_RESULT_RETURN(CMD_OK);
    } else if (parse_cmp("CaptureSTOP", arg, 0)) {
        bcm_time_ts_fifo_capture_stop(unit);
        PTP_CLI_RESULT_RETURN(CMD_OK);
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}

int ts_offset_cb(int unit, void *user_data, uint32 *cb_data_ptr, bcm_time_capture_cb_type_t status)
{

#if defined (BCM_MONTEREY_SUPPORT)
    uint32 regval_upper, regval_lower;

    if (*cb_data_ptr == 0){
        BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_UPPERr, 0, &regval_upper);
        regval_upper &= 0xFFFF;
        BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_UPPERr, 0, regval_upper);
        BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr, 0, &regval_upper);
        BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr, 0, &regval_lower);
    } else {
        BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_UPPERr, 0, &regval_upper);
        regval_upper &= 0xFFFF;
        BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_UPPERr, 0, 0);
        BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr, 0, &regval_upper);
        BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr, 0, &regval_lower);
    }

    cli_out("Offset got applied for TS%d counter now. Current TS: 0x%X%X\n", *cb_data_ptr, regval_upper, regval_lower);
#endif /* BCM_MONTEREY_SUPPORT */
    return BCM_E_NONE;
}

int tsOffsetCalled = 0;
/* PTP TsOffset */
#define PTP_TSOFFSET_USAGE \
    "PTP TsOffset <subcommand> [...]\n"       \
    "\t PTP TsOffset SET [Offset=<TS offset>] [Interval=<update interval] [TsCounter=0/1]\n"
STATIC cmd_result_t
cmd_ptp_ts_offset(int unit, args_t *a)
{
    parse_table_t pt;
    int64 arg_ts_offset;
    uint64 arg_ts_update_intv;
    int arg_ts_counter;
    const char *arg = ARG_GET(a);

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("SET", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Offset", PQ_DFL|PQ_INT64, (void*)1, &arg_ts_offset, NULL);
        parse_table_add(&pt, "Interval", PQ_DFL|PQ_INT64, (void*)1, &arg_ts_update_intv, NULL);
        parse_table_add(&pt, "TsCounter", PQ_INT, (void*)0, &arg_ts_counter, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }

        parse_arg_eq_done(&pt);
        if (tsOffsetCalled == 0){
            /* First time this is called. Register the callbacks*/
            tsOffsetCalled = 1;
            PTP_IF_ERROR_RETURN(bcm_time_capture_cb_register(unit, 0,
                    bcmTimeCaptureTS0CntOffsetUpdated, ts_offset_cb, NULL));

            PTP_IF_ERROR_RETURN(bcm_time_capture_cb_register(unit, 0,
                            bcmTimeCaptureTS1CntOffsetUpdated, ts_offset_cb, NULL));
        }

        PTP_IF_ERROR_RETURN(bcm_time_ts_offset_set(unit, arg_ts_offset,
                    arg_ts_update_intv, arg_ts_counter));
        PTP_CLI_RESULT_RETURN(CMD_OK);
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}

/* PTP DIAGshell */
#define PTP_DIAGSHELL_USAGE \
    "PTP DIAGshell <subcommand> [...]\n"       \
    "\t PTP DIAGshell OPTions [VERBose=y/n]\n" \
    "\t PTP DIAGshell SYNC\n"

STATIC cmd_result_t
cmd_ptp_diagshell(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("OPTions", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "VERBose", PQ_BOOL, INT_TO_PTR(verboseCLI), &verboseCLI, NULL);
        parse_table_add(&pt, "VerboseEvents", PQ_INT, INT_TO_PTR(verboseEvents), &verboseEvents, NULL);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);
        PTP_CLI_RESULT_RETURN(CMD_OK);
    } else if (parse_cmp("SYNC", arg, 0)) {
        PTP_IF_ERROR_RETURN(bcm_ptp_clock_time_get(unit, cur_stack_id, cur_clock_num, &diagshell_sync.ptp_time));
        diagshell_sync.host_time = sal_time();
        diagshell_sync.synced = 1;

        PTP_CLI_RESULT_RETURN(CMD_OK);
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}


/* TDPLL INSTANCE ... */
/* We divide the string because pedantic compiler doesn't allow strings longer then 509 characters */
#define TDPLL_INSTANCE_USAGE_1 \
    "TDPLL INSTance <subcommand> [...]\n"   \
    "\t TDPLL INSTance [show] [<index>]\n"  \
    "\t    I/i: Enabled/Disabled Input\n"   \
    "\t    O/o: Enabled/disabled output\n"  \
    "\t      *: Selected\n"
#define TDPLL_INSTANCE_USAGE_2 \
    "\t TDPLL INSTance Get <index>\n"                                              \
    "\t TDPLL INSTance Set <index> INputMaSK=<hex> OUTputMaSK=<hex>\n"             \
    "\t                    PHaseMODE=<|NONE|PBO|PHaseSLEW|> PHaseOFFSET=<value>\n" \
    "\t                    DPLLBandWidth=<bandwidth>\n"                            \
    "\t                where <bandwidth> = | 0.01Hz | 0.05Hz | 0.1Hz | 0.5Hz |\n"  \
    "\t                                    | 1Hz    | 5Hz    | 10Hz  |\n"


STATIC cmd_result_t
tdpll_instance_show(int unit, int low_idx, int high_idx)
{
    bcm_tdpll_dpll_bindings_t bindings;
    bcm_tdpll_dpll_bandwidth_t bandwidth;
    bcm_tdpll_dpll_phase_control_t phase_control;
    uint32 input_mask;
    uint32 output_mask;

    int dpll_index, best_input, num_dplls, i;
    int dpll_refs[TDPLL_DPLL_INSTANCE_NUM_MAX];

    uint32 bwhz_dec = 0;
    uint32 bwhz_frac = 0;

#if defined (BCM_APACHE_SUPPORT)
    int num_io = 13, ptp_io_num = 8, alt_ptp_input_idx = 9;
    /* mappings from our print index to input or output bit index. 1000->none */
    const int input_idx[15] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 1000, 1000, 1000, 1000, 1000, 1000};
    const int output_idx[15] = {5, 6, 7, 8, 9, 10, 1000, 1000, 4, 0, 1, 2, 3, 11, 12};

    const char *padding[15] = {" ", " ", " ", " ", " ", " ",  /* GPIO 0-5 */
                                  " ", " ", "  ",                /* SE1, SE2, 1588 */
                                  " ", " ", "  ", "    ", "    ", "    "};/* BS0, BS1, SLCPLL/XGPLL0, MLCPLL/XGPLL1, XGPLL2, XGPLL3 */

    if (SOC_IS_APACHE(unit)) {
        cli_out("#  GP0  GP1  GP2  GP3  GP4  GP5  SE1  SE2  1588  BS0  BS1  XGPLL0  XGPLL1  XGPLL2  XGPLL3  BW   PBO\n");
    } else {
        cli_out("#  GP0  GP1  GP2  GP3  GP4  GP5  SE1  SE2  1588  BS0  BS1   S-PLL   M-PLL  BW   PBO\n");
    }
#elif defined (BCM_QAX_SUPPORT)
	int num_io = 13, ptp_io_num = 8, alt_ptp_input_idx = 9;
	 /* mappings from our print index to input or output bit index. 1000->none */
	 const int input_idx[14] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 1000, 1000, 1000, 1000, 1000};
	 const int output_idx[14] = {5, 6, 7, 8, 9, 10, 1000, 1000, 4, 0, 1, 2, 3, 11};

	 const char *padding[14] = {"  ", "   ", "  ", " ", " ", " ",  /* GPIO 0-5 */
								   " ", " ", " ",				  /* SE1, SE2, 1588 */
								   " ", " ", "    ", "    ", "    "}; 	  /* BS0, BS1, PML0, PML1, PMH */

	 if (SOC_IS_QUX(unit)) {
		 cli_out("#  TSGP0  TSGP1  GP0  GP1   -    -   SE1  SE2  1588  BS0  BS1  PMLPLL  PMXPLL  BW   PBO\n");
	 } else if (SOC_IS_QAX(unit)) {
		 cli_out("#  GP0  GP1  GP2  GP3  GP4  GP5  SE1  SE2  1588  BS0	BS1  PML0PLL  PML1PLL  PMHPLL  BW   PBO\n");
	 } else {
		 cli_out("#  GP0  GP1  GP2  GP3  GP4  GP5  SE1  SE2  1588  BS0	BS1   S-PLL   M-PLL  BW   PBO\n");
	 }


#else
    const int num_io = 13, ptp_io_num = 8, alt_ptp_input_idx = 9;
    /* mappings from our print index to input or output bit index. 1000->none */
    const int input_idx[13] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 1000, 1000, 1000, 1000};
    const int output_idx[13] = {5, 6, 7, 8, 9, 10, 1000, 1000, 4, 0, 1, 2, 3};
    const char *padding[13] = {" ", " ", " ", " ", " ", " ",  /* GPIO 0-5 */
                               " ", " ", "  ",                /* SE1, SE2, 1588 */
                               " ", " ", " ", " "};       /* BS0, BS1, SLCPLL, MLCPLL */

    cli_out("#  GP0  GP1  GP2  GP3  GP4  GP5  SE1  SE2  1588  BS0  BS1 S-PLL M-PLL  BW  PBO\n");
#endif

    PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_reference_get(
        unit, 0, TDPLL_DPLL_INSTANCE_NUM_MAX, dpll_refs, &num_dplls));

    for (dpll_index = low_idx; dpll_index < high_idx; ++dpll_index) {
        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_bindings_get(unit, 0, dpll_index, &bindings));

        
        input_mask = bindings.input_clocks[0];
        
        output_mask = bindings.output_clocks[0];

        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_bandwidth_get(unit, 0, dpll_index, &bandwidth));
        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_phase_control_get(unit, 0, dpll_index, &phase_control));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_best_get(unit, 0, dpll_index, &best_input));

        cli_out("%d:", dpll_index);

#if defined (BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            num_io = 15;
        }
#elif defined (BCM_QAX_SUPPORT)
       if (SOC_IS_QUX(unit)) {
         /* retain num_io as default i.e., 13 */
       } else if (SOC_IS_QAX(unit)) {
            num_io = 14;
        }
#endif
        for (i = 0; i < num_io; ++i) {
            const char input_char[3]  = {'-', 'i', 'I'};
            const char output_char[3] = {'-', 'o', 'O'};
            const char reference_char[4] = {' ', '+', '*', '*'};
            int is_best = (input_idx[i] == best_input);
            int is_ref = (input_idx[i] == dpll_refs[dpll_index]);
            int reference_idx;
            int enabled, input_status = 0, output_status = 0;

            /* Calculate input and output status */
            if ( (input_idx[i] != 1000) && (input_mask & (1 << input_idx[i]))) {
                int enabled = 0;
                PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_enable_get(unit, 0, input_idx[i], &enabled));
                input_status = 1 + enabled;
            } else {
                input_status = 0;
            }
            if (i == ptp_io_num) {
                /* For the 1588 column, override the 1588-Freq with 1588-Phase if in use */
                /* Not ideal, but the "tdpll input" command can be used to disambiguate */
                if (input_mask & (1 << alt_ptp_input_idx)) {
                    int alt_enabled = 0;
                    PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_enable_get(
                        unit, 0, alt_ptp_input_idx, &alt_enabled));
                    if (input_status < 1 + alt_enabled) {
                        input_status = 1 + alt_enabled;
                    }

                    if (alt_ptp_input_idx == best_input) {
                        is_best = 1;
                    }
                    if (alt_ptp_input_idx == dpll_refs[dpll_index]) {
                        is_ref = 1;
                    }
                }
            }
            reference_idx = is_ref * 2 + is_best;  /* '*' if active reference, '+' if best but not active */
            if ((output_idx[i] != 1000) && (output_mask & (1 << output_idx[i]))) {
                PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_enable_get(unit, 0, output_idx[i], &enabled));
                output_status = 1 + enabled;
            } else {
                output_status = 0;
            }
            /* Display input and output status with possible '/' */
            if (input_status) {
                if (output_status) {
                    cli_out("%s%c/%c%c", padding[i], input_char[input_status], output_char[output_status],
                            reference_char[reference_idx]);
                } else {
                    cli_out("%s %c%c ", padding[i], input_char[input_status], reference_char[reference_idx]);
                }
            } else {
                cli_out("%s %c%c ", padding[i], output_char[output_status], reference_char[reference_idx]);
            }
        }

        /* Bandwidth printout as x.yz. (i.e. one-hundredth Hz precision). */
        switch (bandwidth.units) {
        case bcm_tdpll_dpll_bandwidth_mHz:
            bwhz_dec = bandwidth.value / 1000;
            bwhz_frac =  ((bandwidth.value % 1000) + 5)/10;
            break;
        case bcm_tdpll_dpll_bandwidth_Hz:
            bwhz_dec = bandwidth.value;
            bwhz_frac = 0;
            break;
        case bcm_tdpll_dpll_bandwidth_kHz:
            bwhz_dec = bandwidth.value * 1000;
            bwhz_frac = 0;
            break;
        default:
            bwhz_dec = 0;
            bwhz_frac = 0;
            break;
        }
        cli_out("%3u.%02u", (unsigned)bwhz_dec, (unsigned)bwhz_frac);

        switch (phase_control.mode) {
        case bcm_tdpll_dpll_phase_mode_none:
            cli_out(" Off ");
            break;
        case bcm_tdpll_dpll_phase_mode_pbo:
            cli_out("  On ");
            break;
        case bcm_tdpll_dpll_phase_mode_pboslew:
            cli_out(" Slew");
            break;
        }
        cli_out("\n");
    }
    cli_out("\n");

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


STATIC cmd_result_t
cmd_tdpll_instance(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    int invalid_value = 0x7fffffff;

    int arg_dpll_index = -1;

    int arg_input_mask = 0;
    int arg_output_mask = 0;

    char *arg_phase_mode_str[4] = {"NONE", "PBO", "PHaseSLEW", 0};
    int arg_phase_mode_index = invalid_value;
    int arg_phase_offset = 0;
    bcm_tdpll_dpll_phase_control_t arg_phase_control;
    char **input_str = NULL;

    char *arg_bandwidth_str[8] = {"0.01Hz", "0.05Hz", "0.1Hz", "0.5Hz", "1Hz", "4Hz", "5Hz", "10Hz"};
    int arg_bandwidth_index = invalid_value;
    bcm_tdpll_dpll_bandwidth_t arg_dpll_bandwidth_array[8] =
        {{10, bcm_tdpll_dpll_bandwidth_mHz},
         {50, bcm_tdpll_dpll_bandwidth_mHz},
         {100, bcm_tdpll_dpll_bandwidth_mHz},
         {500, bcm_tdpll_dpll_bandwidth_mHz},
         {1, bcm_tdpll_dpll_bandwidth_Hz},
         {4, bcm_tdpll_dpll_bandwidth_Hz},
         {5, bcm_tdpll_dpll_bandwidth_Hz},
         {10, bcm_tdpll_dpll_bandwidth_Hz}};

    char *input_str_common[TDPLL_INPUT_CLOCK_NUM_MAX] =
        {"GPIO0 ", "GPIO1 ", "GPIO2 ", "GPIO3 ", "GPIO4 ", "GPIO5 ",
         "SyncE1", "SyncE2", "1588-Freq", "1588-Phase"};

#if defined (BCM_QAX_SUPPORT)
    char *input_str_qux[TDPLL_INPUT_CLOCK_NUM_MAX] =
        {"TSGPIO0 ", "TSGPIO1 ", "GPIO0 ", "GPIO1 ", " - ", " - ",
         "SyncE1", "SyncE2", "1588-Freq", "1588-Phase"};
#endif

#if defined (BCM_APACHE_SUPPORT) ||  defined (BCM_MONTEREY_SUPPORT)
    char *output_str_ap[TDPLL_OUTPUT_CLOCK_NUM_MAX] =
        {"BroadSync0    ", "BroadSync1    ", "XgPll0        ", "XgPll1        ",
         "1588-Hybrid   ", "GPIO0", "GPIO1", "GPIO2", "GPIO3", "GPIO4", "GPIO5", "XgPll2        ", "XgPll3        "};

    char *output_str_mo[TDPLL_OUTPUT_CLOCK_NUM_MAX] =
        {"BroadSync0    ", "BroadSync1    ", "SyncE1        ", "SyncE2        ", "SyncE3        ", "SyncE4        ",
         "1588-Hybrid   ", "GPIO0", "GPIO1", "GPIO2", "GPIO3", "GPIO4", "GPIO5"};
#endif
#if defined (BCM_QAX_SUPPORT)
    char *output_str_qax[TDPLL_OUTPUT_CLOCK_NUM_MAX] =
	{"BroadSync0	", "BroadSync1	  ", "Pml0Pll		", "Pml1Pll		  ",
	 "1588-Hybrid	", "GPIO0", "GPIO1", "GPIO2", "GPIO3", "GPIO4", "GPIO5", "PmhPll		"};
    char *output_str_qux[TDPLL_OUTPUT_CLOCK_NUM_MAX] =
	{"BroadSync0	", "BroadSync1	  ", "PmlPll		", "PmxPll		  ",
	 "1588-Hybrid	", "TSGPIO0", "TSGPIO1", "GPIO0", "GPIO1", " - ", " - "};
#endif
    char *output_str[TDPLL_OUTPUT_CLOCK_NUM_MAX] =
        {"BroadSync0    ", "BroadSync1    ", "SerDesLCPLL   ", "MasterLCPLL   ",
         "1588-Hybrid   ", "GPIO0", "GPIO1", "GPIO2", "GPIO3", "GPIO4", "GPIO5"};

    bcm_tdpll_dpll_bindings_t arg_bindings;
    bcm_tdpll_dpll_bandwidth_t arg_dpll_bandwidth = {1, bcm_tdpll_dpll_bandwidth_Hz};

    uint32 input_mask;
    uint32 output_mask;
    int start_index = 0, end_index = TDPLL_DPLL_INSTANCE_NUM_MAX;

    input_str = input_str_common;
#if defined (BCM_QAX_SUPPORT)
    if (SOC_IS_QUX(unit)) {
        input_str = input_str_qux;
    }
#endif

#if defined (BCM_APACHE_SUPPORT) || defined (BCM_QAX_SUPPORT)
    if (SOC_IS_QUX(unit)) {
        end_index = TDPLL_DPLL_INSTANCE_NUM_MAX - 2;
    } else if (SOC_IS_QAX(unit)) {
        end_index = TDPLL_DPLL_INSTANCE_NUM_MAX - 1;
    } else if (SOC_IS_APACHE(unit)) {
        /* Retain end_index */
    } else {
        end_index = TDPLL_DPLL_INSTANCE_NUM_MAX - 2;
    } 
#else 
    end_index = TDPLL_DPLL_INSTANCE_NUM_MAX - 2;

#endif

    if (arg) {
        char *dpll_index_str = ARG_GET(a);
        if (dpll_index_str) {
            if (!isint(dpll_index_str)) {
                cli_out("%s INSTance show/Get/Set: Invalid DPLL index '%s'\n", ARG_CMD(a), dpll_index_str);
                cli_out(TDPLL_INSTANCE_USAGE_1);
                cli_out(TDPLL_INSTANCE_USAGE_2);
                return CMD_FAIL;
            }
            arg_dpll_index = parse_integer(dpll_index_str);
        }
    }

    if (!arg || parse_cmp("show", arg, 0)) {
        if (arg_dpll_index >= 0) {
            start_index = arg_dpll_index;
            end_index = arg_dpll_index + 1;
        }

        return tdpll_instance_show(unit, start_index, end_index);
    } else if (parse_cmp("Get", arg, 0)) {
        if (arg_dpll_index < 0) {
            cli_out("%s INSTance Get: No DPLL index specified\n", ARG_CMD(a));
            return(CMD_FAIL);
        }

        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_bindings_get(
            unit, 0, arg_dpll_index, &arg_bindings));
        
        input_mask = arg_bindings.input_clocks[0];
        
        output_mask = arg_bindings.output_clocks[0];

        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_bandwidth_get(
            unit, 0, arg_dpll_index, &arg_dpll_bandwidth));

        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_phase_control_get(
            unit, 0, arg_dpll_index, &arg_phase_control));

        cli_out("\n");
        cli_out("T-DPLL DPLL Instance Configuration.\n");
        cli_out("Unit: %d   Stack: %d\n",
                unit, 0);
        cli_out("-----------------------------------------------------------"
                "---------------------\n");
        cli_out("DPLL       : %d\n", arg_dpll_index);
        cli_out("\n");
        cli_out("Inputs     : 0x%08x\n", input_mask);
        cli_out("             | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n"
                "             | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n"
                "             | %s [%s] | %s [%s] |\n",
                input_str[0], input_mask & (1 << 0) ? "x":" ",
                input_str[1], input_mask & (1 << 1) ? "x":" ",
                input_str[2], input_mask & (1 << 2) ? "x":" ",
                input_str[3], input_mask & (1 << 3) ? "x":" ",
                input_str[4], input_mask & (1 << 4) ? "x":" ",
                input_str[5], input_mask & (1 << 5) ? "x":" ",
                input_str[6], input_mask & (1 << 6) ? "x":" ",
                input_str[7], input_mask & (1 << 7) ? "x":" ",
                input_str[8], input_mask & (1 << 8) ? "x":" ",
                input_str[9], input_mask & (1 << 9) ? "x":" ");
        cli_out("\n");
        cli_out("Outputs    : 0x%08x\n", output_mask);
#if defined (BCM_APACHE_SUPPORT) || defined (BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            cli_out("             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n",
                    output_str_mo[0], output_mask & (1 << 0) ? "x":" ",
                    output_str_mo[1], output_mask & (1 << 1) ? "x":" ",
                    output_str_mo[2], output_mask & (1 << 2) ? "x":" ",
                    output_str_mo[3], output_mask & (1 << 3) ? "x":" ",
                    output_str_mo[4], output_mask & (1 << 4) ? "x":" ",
                    output_str_mo[5], output_mask & (1 << 5) ? "x":" ",
                    output_str_mo[6], output_mask & (1 << 6) ? "x":" ",
                    output_str_mo[7], output_mask & (1 << 7) ? "x":" ",
                    output_str_mo[8], output_mask & (1 << 8) ? "x":" ",
                    output_str_mo[9], output_mask & (1 << 9) ? "x":" ",
                    output_str_mo[10], output_mask & (1 << 10) ? "x":" ",
                    output_str_mo[11], output_mask & (1 << 11) ? "x":" ",
                    output_str_mo[12], output_mask & (1 << 12) ? "x":" ");
         } else if (SOC_IS_APACHE(unit)) {
            cli_out("             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n",
                    output_str_ap[0], output_mask & (1 << 0) ? "x":" ",
                    output_str_ap[1], output_mask & (1 << 1) ? "x":" ",
                    output_str_ap[2], output_mask & (1 << 2) ? "x":" ",
                    output_str_ap[3], output_mask & (1 << 3) ? "x":" ",
                    output_str_ap[4], output_mask & (1 << 4) ? "x":" ",
                    output_str_ap[5], output_mask & (1 << 5) ? "x":" ",
                    output_str_ap[6], output_mask & (1 << 6) ? "x":" ",
                    output_str_ap[7], output_mask & (1 << 7) ? "x":" ",
                    output_str_ap[8], output_mask & (1 << 8) ? "x":" ",
                    output_str_ap[9], output_mask & (1 << 9) ? "x":" ",
                    output_str_ap[10], output_mask & (1 << 10) ? "x":" ",
                    output_str_ap[11], output_mask & (1 << 11) ? "x":" ",
                    output_str_ap[12], output_mask & (1 << 12) ? "x":" ");
         } else
#endif
#if defined(BCM_QAX_SUPPORT)
        if (SOC_IS_QUX(unit)) {
            cli_out("             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n",
                    output_str_qux[0], output_mask & (1 << 0) ? "x":" ",
                    output_str_qux[1], output_mask & (1 << 1) ? "x":" ",
                    output_str_qux[2], output_mask & (1 << 2) ? "x":" ",
                    output_str_qux[3], output_mask & (1 << 3) ? "x":" ",
                    output_str_qux[4], output_mask & (1 << 4) ? "x":" ",
                    output_str_qux[5], output_mask & (1 << 5) ? "x":" ",
                    output_str_qux[6], output_mask & (1 << 6) ? "x":" ",
                    output_str_qux[7], output_mask & (1 << 7) ? "x":" ",
                    output_str_qux[8], output_mask & (1 << 8) ? "x":" ",
                    output_str_qux[9], output_mask & (1 << 9) ? "x":" ",
                    output_str_qux[10], output_mask & (1 << 10) ? "x":" ");
        } else if (SOC_IS_QAX(unit)) {
            cli_out("             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n",
                    output_str_qax[0], output_mask & (1 << 0) ? "x":" ",
                    output_str_qax[1], output_mask & (1 << 1) ? "x":" ",
                    output_str_qax[2], output_mask & (1 << 2) ? "x":" ",
                    output_str_qax[3], output_mask & (1 << 3) ? "x":" ",
                    output_str_qax[4], output_mask & (1 << 4) ? "x":" ",
                    output_str_qax[5], output_mask & (1 << 5) ? "x":" ",
                    output_str_qax[6], output_mask & (1 << 6) ? "x":" ",
                    output_str_qax[7], output_mask & (1 << 7) ? "x":" ",
                    output_str_qax[8], output_mask & (1 << 8) ? "x":" ",
                    output_str_qax[9], output_mask & (1 << 9) ? "x":" ",
                    output_str_qax[10], output_mask & (1 << 10) ? "x":" ",
                    output_str_qax[11], output_mask & (1 << 11) ? "x":" ");
        } else
#endif
        {
            cli_out("             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] |\n"
                    "             | %s [%s] | %s [%s] | %s [%s] | %s [%s] |\n",
                    output_str[0], output_mask & (1 << 0) ? "x":" ",
                    output_str[1], output_mask & (1 << 1) ? "x":" ",
                    output_str[2], output_mask & (1 << 2) ? "x":" ",
                    output_str[3], output_mask & (1 << 3) ? "x":" ",
                    output_str[4], output_mask & (1 << 4) ? "x":" ",
                    output_str[5], output_mask & (1 << 5) ? "x":" ",
                    output_str[6], output_mask & (1 << 6) ? "x":" ",
                    output_str[7], output_mask & (1 << 7) ? "x":" ",
                    output_str[8], output_mask & (1 << 8) ? "x":" ",
                    output_str[9], output_mask & (1 << 9) ? "x":" ",
                    output_str[10], output_mask & (1 << 10) ? "x":" ");
        }

        cli_out("\n");
        cli_out("Phase Mode : %s (%d)  %d ns\n",
                arg_phase_control.mode == bcm_tdpll_dpll_phase_mode_none ?
                "None" : (arg_phase_control.mode == bcm_tdpll_dpll_phase_mode_pbo ?
                "PBO" : (arg_phase_control.mode == bcm_tdpll_dpll_phase_mode_pboslew ?
                "PBO + Phase Slew" : "Unknown")), arg_phase_control.mode, arg_phase_control.offset_ns);
        cli_out("Bandwidth  : %d %s\n",
                arg_dpll_bandwidth.value,
                arg_dpll_bandwidth.units == bcm_tdpll_dpll_bandwidth_mHz ?
                "mHz" : (arg_dpll_bandwidth.units == bcm_tdpll_dpll_bandwidth_Hz ?
                "Hz" : (arg_dpll_bandwidth.units == bcm_tdpll_dpll_bandwidth_kHz ?
                "kHz" : "Unknown")));
        cli_out("\n");

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("Set", arg, 0)) {
        if (arg_dpll_index < 0) {
            cli_out("%s INSTance Set: No DPLL index specified\n", ARG_CMD(a));
            return(CMD_FAIL);
        }
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "INputMaSK", PQ_HEX, (void*)(size_t)invalid_value,
            &arg_input_mask, NULL);
        parse_table_add(&pt, "OUTputMaSK", PQ_HEX, (void*)(size_t)invalid_value,
            &arg_output_mask, NULL);
        parse_table_add(&pt, "DPLLBandWidth", PQ_MULTI | PQ_DFL, 0,
            &arg_bandwidth_index, arg_bandwidth_str);
        parse_table_add(&pt, "PHaseMODE", PQ_MULTI | PQ_DFL, 0,
            &arg_phase_mode_index, arg_phase_mode_str);
        parse_table_add(&pt, "PHaseOFFSET", PQ_INT, (void*)0, &arg_phase_offset, NULL);
        
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_bindings_get(
            unit, 0, arg_dpll_index, &arg_bindings));

        if (arg_input_mask != invalid_value) {
            
            arg_bindings.input_clocks[0] = (uint32)arg_input_mask;
/*arg_bindings.input_clocks[1] = 0;
  arg_bindings.input_clocks[2] = 0;*/
        }

        if (arg_output_mask != invalid_value) {
            
            arg_bindings.output_clocks[0] = (uint32)arg_output_mask;
/*arg_bindings.output_clocks[1] = 0;
  arg_bindings.output_clocks[2] = 0;*/
        }

        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_bindings_set(
            unit, 0, arg_dpll_index, &arg_bindings));

        if (arg_bandwidth_index != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_bandwidth_set(
                unit, 0, arg_dpll_index, &arg_dpll_bandwidth_array[arg_bandwidth_index]));
        }

        if (arg_phase_mode_index != invalid_value) {
            arg_phase_control.mode = (bcm_tdpll_dpll_phase_mode_t)arg_phase_mode_index;
            arg_phase_control.offset_ns = arg_phase_offset;
            PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_phase_control_set(unit, 0,
                arg_dpll_index, &arg_phase_control));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);
    } else if (parse_cmp("HELP", arg, 0)) {
        cli_out(TDPLL_INSTANCE_USAGE_1);
        cli_out(TDPLL_INSTANCE_USAGE_2);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/* TDPLL INPUTCLK ... */
/* We divide the string because pedantic compiler doesn't allow strings longer then 509 characters */
#if defined (BCM_QAX_SUPPORT)
#define TDPLL_INPUTCLK_USAGE_1\
    "TDPLL INputclk <subcommand> [...]\n"                                             \
    "\t TDPLL INputclk [show] [<index>]\n"                                            \
    "\t TDPLL INputclk Get <index>\n"                                                 \
    "\t                where <index> = | TSGPIO0  | TSGPIO1  | GPIO0  | GPIO1  |\n"   \
    "\t                                | - | - | SyncE1 | SyncE2 |\n"       \
    "\t                                | 1588-Freq | 1588-Phase |\n"                  \
    "\t TDPLL INputclk Set <index> L1MUX=<mux> L1PORT=<port> MAC=<mac>\n"             \
    "\t                    FREQuency=<value> TSFREQuency=<value> ENable=<y/n>\n"      \
    "\t                    QL=<QL> PRIOrity=<value> LOCKout=<y/n>\n"
#else
#define TDPLL_INPUTCLK_USAGE_1 \
    "TDPLL INputclk <subcommand> [...]\n"                                             \
    "\t TDPLL INputclk [show] [<index>]\n"                                            \
    "\t TDPLL INputclk Get <index>\n"                                                 \
    "\t                where <index> = | GPIO0  | GPIO1  | GPIO2  | GPIO3  |\n"       \
    "\t                                | GPIO4  | GPIO5  | SyncE1 | SyncE2 |\n"       \
    "\t                                | 1588-Freq | 1588-Phase |\n"                  \
    "\t TDPLL INputclk Set <index> L1MUX=<mux> L1PORT=<port> MAC=<mac>\n"             \
    "\t                    FREQuency=<value> TSFREQuency=<value> ENable=<y/n>\n"      \
    "\t                    QL=<QL> PRIOrity=<value> LOCKout=<y/n>\n"
#endif
#define TDPLL_INPUTCLK_USAGE_2 \
    "\t                where <QL> = | QL-PRC  | QL-SSU-A | QL-SSU-B | (Option I)\n"   \
    "\t                             | QL-SEC  | QL-DNU   |            (Option I)\n"   \
    "\t                             | QL-STU  | QL-PRS   | QL-TNC   | (Option II)\n"  \
    "\t                             | QL-ST2  | QL-ST3   | QL-SMC   | (Option II)\n"  \
    "\t                             | QL-ST3E | QL-PROV  | QL-DUS   | (Option II)\n"  \
    "\t                             | QL-UNK  | QL-SEC   |            (Option III)\n"


STATIC cmd_result_t
tdpll_inputclk_show(int unit, int low_idx, int high_idx)
{
    int inputclk_idx;
    int dpll_idx;

#if !defined(BCM_QAX_SUPPORT)
    const char *input_name[TDPLL_INPUT_CLOCK_NUM_MAX] =
        {"GPIO0 ","GPIO1 ","GPIO2 ","GPIO3 ","GPIO4 ","GPIO5 ",
         "syncE1","syncE2","1588-F", "1588-P"};
#else
    const char *input_name[TDPLL_INPUT_CLOCK_NUM_MAX] =
        {"TSGPIO0 ","TSGPIO1 ","GPIO0 ","GPIO1 "," - "," -  ",
         "syncE1","syncE2","1588-F", "1588-P"};
#endif

    cli_out("ID     En?   Offset   Valid    QL     Prio   LkO Mux   DPLLS\n");
    /* add sample & ts frequencies, maybe MAC, on second line?  */

    for (inputclk_idx = low_idx; inputclk_idx < high_idx; ++inputclk_idx) {
        int freq_error_ppb;
        int enabled, dpll_en, best_clock;
        bcm_tdpll_input_clock_l1mux_t l1mux;
        int valid, soft_warn, hard_accept, hard_reject, priority, lockout;
        int dpll_refs[TDPLL_DPLL_INSTANCE_NUM_MAX];
        int num_dplls;
        bcm_esmc_quality_level_t ql;

        cli_out("%s ", input_name[inputclk_idx]);

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_enable_get(
            unit, 0, inputclk_idx, &enabled));

        cli_out(" %c", enabled ? 'Y' : 'N');

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_frequency_error_get(
            unit, 0, inputclk_idx,
            &freq_error_ppb));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_valid_get(
            unit, 0, inputclk_idx, &valid));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_threshold_state_get(
            unit, 0, inputclk_idx,
                bcm_tdpll_input_clock_monitor_type_soft_warn, &soft_warn));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_threshold_state_get(
            unit, 0, inputclk_idx,
            bcm_tdpll_input_clock_monitor_type_hard_accept, &hard_accept));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_threshold_state_get(
            unit, 0, inputclk_idx,
            bcm_tdpll_input_clock_monitor_type_hard_reject, &hard_reject));

        if (!valid && freq_error_ppb == -65249626) {
            cli_out("       ---   ");
        } else {
            cli_out("%10d   ", freq_error_ppb);
        }

        if (valid) {
            cli_out("  %c   ", (soft_warn) ? 'W' : 'Y');
        } else {
            cli_out("  %c   ", (hard_reject) ? 'R' : 'I');
        }

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_ql_get(
            unit, 0, inputclk_idx, &ql));

        cli_out("%8s",
                ql == bcm_esmc_g781_I_ql_prc ? "QL-PRC" :
                (ql == bcm_esmc_g781_I_ql_ssua ? "QL-SSU-A" :
                (ql == bcm_esmc_g781_I_ql_ssub ? "QL-SSU-B" :
                (ql == bcm_esmc_g781_I_ql_sec ? "QL-SEC" :
                (ql == bcm_esmc_g781_I_ql_dnu ? "QL-DNU" :
                (ql == bcm_esmc_g781_II_ql_stu ? "QL-STU" :
                (ql == bcm_esmc_g781_II_ql_prs ? "QL-PRS" :
                (ql == bcm_esmc_g781_II_ql_tnc ? "QL-TNC" :
                (ql == bcm_esmc_g781_II_ql_st2 ? "QL-ST2" :
                (ql == bcm_esmc_g781_II_ql_st3 ? "QL-ST3" :
                (ql == bcm_esmc_g781_II_ql_smc ? "QL-SMC" :
                (ql == bcm_esmc_g781_II_ql_st3e ? "QL-ST3E" :
                (ql == bcm_esmc_g781_II_ql_prov ? "QL-PROV" :
                (ql == bcm_esmc_g781_II_ql_dus ? "QL-DUS" :
                (ql == bcm_esmc_g781_III_ql_unk ? "QL-UNK" :
                (ql == bcm_esmc_g781_III_ql_sec ? "QL-SEC" : "Unknown"
                ))))))))))))))));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_priority_get(
            unit, 0, inputclk_idx, &priority));
        cli_out("%5d", priority);

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_lockout_get(
            unit, 0, inputclk_idx, &lockout));
        cli_out("     %c", lockout ? 'Y' : 'N');

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_l1mux_get(
            unit, 0, inputclk_idx, &l1mux));
        if (l1mux.port >= 0) {
            cli_out("  %d.%-2d  ", l1mux.index, l1mux.port);
        } else {
            cli_out("   --   ");
        }

        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_reference_get(
            unit, 0, TDPLL_DPLL_INSTANCE_NUM_MAX, dpll_refs, &num_dplls));

        for (dpll_idx = 0; dpll_idx < TDPLL_DPLL_INSTANCE_NUM_MAX; ++dpll_idx) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_dpll_use_get(
                unit, 0, inputclk_idx, dpll_idx, &dpll_en));
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_best_get(
                unit, 0, dpll_idx, &best_clock));

            cli_out("%c",
                    (!dpll_en)                            ? '-' : /* not on DPLL list of inputs */
                    (dpll_refs[dpll_idx] == inputclk_idx) ? '*' : /* Current ref for DPLL */
                    (best_clock == inputclk_idx)          ? '+' : /* best ref for DPLL, but non-revert */
                    (enabled)                             ? 'x' : /* a backup for the DPLL */
                    '.'); /* on DPLL list, not enabled */

        }
        cli_out("\n");

    }

    return CMD_OK;
}

#if defined(BCM_MONTEREY_SUPPORT)
int get_input_clock_index(int arg_clock_index, int arg_l1mux_port, int arg_l1mux_index)
{
    int clock_index = arg_clock_index;

    /* "GPIO0", "GPIO1", "GPIO2", "GPIO3", "GPIO4", "GPIO5", "SyncE0", "SyncE1",..., "SyncE64", "1588-Freq", "1588-Phase"  */
    if ((clock_index == 6) && (arg_l1mux_port > 0) && (arg_l1mux_port <= 64)) {
        /* syncE: 6 -> 69 */
        if (BCM_TDPLL_INPUT_CLOCK_NUM_SYNCE == 2) {
            clock_index = clock_index + arg_l1mux_index;
        } else {
            
            clock_index = clock_index + (arg_l1mux_port-1);
        }
    } else if (clock_index == 7) {
        /* 1588-Freq:70 */
        clock_index = (BCM_TDPLL_INPUT_CLOCK_NUM_SYNCE + 6);    /*70;*/
    } else if (clock_index == 8) {
        /* 1588-Phase:71 */
        clock_index = (BCM_TDPLL_INPUT_CLOCK_NUM_SYNCE + 7);   /*71;*/
    } else {
        cli_out("### get_input_clock_index: Error !! Invalid clock_index or port\n");
    }
cli_out("### get_input_clock_index: ip:clock_index-%d op:clock_index-%d\n", arg_clock_index, clock_index);

    return clock_index;
}
#endif


#if defined (BCM_MONTEREY_SUPPORT)
int bcm_esw_ptp_map_synce_clock_port(int in_port){
    switch (in_port) {
    case 21:
        cli_out("Returning 22\n");
        return 22;
    case 25:
        cli_out("Returning 28\n");
        return 28;
    default:
        cli_out("Returning %d\n", in_port);
        return in_port;
    }
}
#endif

STATIC cmd_result_t
cmd_tdpll_inputclk(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    int invalid_value = 0x7fffffff;

#if defined(BCM_MONTEREY_SUPPORT)
    char *arg_index_str[9+1] =
        {"GPIO0","GPIO1","GPIO2","GPIO3","GPIO4","GPIO5",
         "SyncE","1588-Freq", "1588-Phase", 0};
#elif !defined(BCM_QAX_SUPPORT)
    char *arg_index_str[TDPLL_INPUT_CLOCK_NUM_MAX+1] =
        {"GPIO0","GPIO1","GPIO2","GPIO3","GPIO4","GPIO5",
         "SyncE1","SyncE2","1588-Freq", "1588-Phase", 0};
#else
    char *arg_index_str[TDPLL_INPUT_CLOCK_NUM_MAX+1] =
        {"TSGPIO0","TSGPIO1","GPIO0","GPIO1"," - "," - ",
         "SyncE1","SyncE2","1588-Freq", "1588-Phase", 0};
#endif
    int arg_clock_index = -1;  /* signal that none was provided */
    int start_index = 0, end_index = TDPLL_INPUT_CLOCK_NUM_MAX;

    bcm_tdpll_input_clock_l1mux_t arg_l1mux;
    int arg_l1mux_index = 0;
    int arg_l1mux_port = 0;

    bcm_mac_t arg_mac = {0x00, 0x10, 0x18, 0x00, 0x00, 0x00};
    bcm_mac_t zero_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    int arg_clk_en = 0;
    int arg_dpll0_en = 0;
    int arg_dpll1_en = 0;
    int arg_dpll2_en = 0;
    int arg_dpll3_en = 0;
    int arg_dpll4_en = 0;

    int arg_clock_frequency = 0;
    int arg_tsevent_frequency = 4000;
    uint32 arg_clock_frequency_rv;
    uint32 arg_tsevent_frequency_rv;
    int arg_tsevent_quotient_rv = 0;

    char *arg_ql_str[16] = {"QL-PRC", "QL-SSU-A", "QL-SSU-B", "QL-SEC", "QL-DNU",
                            "QL-STU", "QL-PRS", "QL-TNC", "QL-ST2", "QL-ST3",
                            "QL-SMC", "QL-ST3E", "QL-PROV", "QL-DUS", "QL-UNK", 0};

    int arg_ql_index = invalid_value;
    bcm_esmc_quality_level_t arg_ql;

    int arg_priority = 0;
    int arg_lockout = 0;
    int arg_valid = 0;

    int arg_over_softwarn = 0;
    int arg_under_hardaccept = 0;
    int arg_over_hardreject = 0;

    int arg_freq_error_ppb = 0;

    if (arg) {
        char *clock_index_str = ARG_GET(a);
        if (clock_index_str) {
            arg_clock_index = 0;
            while (arg_index_str[arg_clock_index]) {
                if (parse_cmp(arg_index_str[arg_clock_index], clock_index_str, 0)) {
                    break;
                }
                ++arg_clock_index;
            }
            if (!arg_index_str[arg_clock_index]) {
                cli_out("%s INputclk show/Get/Set: Invalid Clock index '%s'\n", ARG_CMD(a), clock_index_str);
                cli_out(TDPLL_INPUTCLK_USAGE_1);
                cli_out(TDPLL_INPUTCLK_USAGE_2);
                return CMD_FAIL;
            }
        }
    }

    if (!arg || parse_cmp("show", arg, 0)) {
        if (arg_clock_index >= 0) {
            start_index = arg_clock_index;
            end_index = arg_clock_index + 1;
        }

        return tdpll_inputclk_show(unit, start_index, end_index);
    } else if (parse_cmp("Get", arg, 0)) {
        if (arg_clock_index < 0) {
            cli_out("Inputclk index required\n");
            return CMD_USAGE;
        }

#if defined(BCM_MONTEREY_SUPPORT)
        if (arg_clock_index == 6) {
            char *l1Mux = ARG_GET(a);
            if (!isint(l1Mux)) {
                cli_out("Error!! Invalid port\n");
                return CMD_FAIL;
            }
            arg_l1mux_index = parse_integer(l1Mux);
            if (arg_l1mux_index != 0 && arg_l1mux_index != 1) {
                cli_out("Error!! Unsupported syncE\n");
                return CMD_FAIL;
            }
        }

        
        arg_clock_index = get_input_clock_index(arg_clock_index, 1/*arg_l1mux_index*/, arg_l1mux_index);
#endif
        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_l1mux_get(
            unit, 0, arg_clock_index, &arg_l1mux));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_mac_get(
            unit, 0, arg_clock_index, &arg_mac));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_enable_get(
            unit, 0, arg_clock_index, &arg_clk_en));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_dpll_use_get(
            unit, 0, arg_clock_index, 0, &arg_dpll0_en));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_dpll_use_get(
            unit, 0, arg_clock_index, 1, &arg_dpll1_en));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_dpll_use_get(
            unit, 0, arg_clock_index, 2, &arg_dpll2_en));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_dpll_use_get(
            unit, PTP_STACK_ID_DEFAULT, arg_clock_index, 3, &arg_dpll3_en));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_dpll_use_get(
            unit, PTP_STACK_ID_DEFAULT, arg_clock_index, 4, &arg_dpll4_en));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_frequency_get(
            unit, 0, arg_clock_index,
            &arg_clock_frequency_rv, &arg_tsevent_frequency_rv));
        arg_tsevent_quotient_rv = arg_tsevent_frequency_rv ?
            (arg_clock_frequency_rv + (arg_tsevent_frequency_rv >> 1))/arg_tsevent_frequency_rv : -1;

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_ql_get(
            unit, 0, arg_clock_index, &arg_ql));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_priority_get(
            unit, 0, arg_clock_index, &arg_priority));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_lockout_get(
            unit, 0, arg_clock_index, &arg_lockout));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_valid_get(
            unit, 0, arg_clock_index, &arg_valid));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_threshold_state_get(
            unit, 0, arg_clock_index,
                bcm_tdpll_input_clock_monitor_type_soft_warn, &arg_over_softwarn));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_threshold_state_get(
            unit, 0, arg_clock_index,
            bcm_tdpll_input_clock_monitor_type_hard_accept, &arg_under_hardaccept));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_threshold_state_get(
            unit, 0, arg_clock_index,
            bcm_tdpll_input_clock_monitor_type_hard_reject, &arg_over_hardreject));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_frequency_error_get(
            unit, 0, arg_clock_index,
            &arg_freq_error_ppb));

        cli_out("\n");
        cli_out("T-DPLL Input Clock Data and Metadata.\n");
        cli_out("Unit: %d   Stack: %d\n",
                unit, 0);
        cli_out("-----------------------------------------------------------"
                "---------------------\n");
#if defined(BCM_MONTEREY_SUPPORT)
        arg_clock_index = (arg_clock_index == 7)? 6: arg_clock_index;
#endif
        cli_out("Index    : %d (%s)   L1 MUX: %d.%d\n", arg_clock_index,
                (arg_clock_index >= 0 && arg_clock_index < TDPLL_INPUT_CLOCK_NUM_MAX) ?
                arg_index_str[arg_clock_index] : "Unknown", arg_l1mux.index, arg_l1mux.port);
        cli_out("MAC      : %02x:%02x:%02x:%02x:%02x:%02x\n",
                arg_mac[0], arg_mac[1], arg_mac[2], arg_mac[3], arg_mac[4], arg_mac[5]);
        cli_out("\n");
        cli_out("Enable   : [%s]\n", arg_clk_en ? "x":" ");
        cli_out("\n");
        cli_out("Clock f  : %u Hz TS f: %u Hz (Q: %d)\n", (unsigned)arg_clock_frequency_rv,
                (unsigned)arg_tsevent_frequency_rv, arg_tsevent_quotient_rv);
        cli_out("\n");
        cli_out("QL       : %s\n",
                arg_ql == bcm_esmc_g781_I_ql_prc ? "QL-PRC" :
                (arg_ql == bcm_esmc_g781_I_ql_ssua ? "QL-SSU-A" :
                (arg_ql == bcm_esmc_g781_I_ql_ssub ? "QL-SSU-B" :
                (arg_ql == bcm_esmc_g781_I_ql_sec ? "QL-SEC" :
                (arg_ql == bcm_esmc_g781_I_ql_dnu ? "QL-DNU" :
                (arg_ql == bcm_esmc_g781_II_ql_stu ? "QL-STU" :
                (arg_ql == bcm_esmc_g781_II_ql_prs ? "QL-PRS" :
                (arg_ql == bcm_esmc_g781_II_ql_tnc ? "QL-TNC" :
                (arg_ql == bcm_esmc_g781_II_ql_st2 ? "QL-ST2" :
                (arg_ql == bcm_esmc_g781_II_ql_st3 ? "QL-ST3" :
                (arg_ql == bcm_esmc_g781_II_ql_smc ? "QL-SMC" :
                (arg_ql == bcm_esmc_g781_II_ql_st3e ? "QL-ST3E" :
                (arg_ql == bcm_esmc_g781_II_ql_prov ? "QL-PROV" :
                (arg_ql == bcm_esmc_g781_II_ql_dus ? "QL-DUS" :
                (arg_ql == bcm_esmc_g781_III_ql_unk ? "QL-UNK" :
                (arg_ql == bcm_esmc_g781_III_ql_sec ? "QL-SEC" : "Unknown"
                ))))))))))))))));
        cli_out("Priority : %u\n", (unsigned)arg_priority);
        cli_out("Lockout  : [%s]\n", arg_lockout ? "x":" ");
        cli_out("\n");
        cli_out("Valid    : [%s]\n", arg_valid ? "x":" ");
        cli_out("| Over Soft-Warn [%s] | Under Hard-Accept [%s] | Over Hard-Reject [%s] |\n",
                arg_over_softwarn ? "x":" ", arg_under_hardaccept ? "x":" ", arg_over_hardreject ? "x":" ");
        cli_out("f Error  : %d ppb\n", arg_freq_error_ppb);
        cli_out("\n");
        cli_out("DPLLs    : | DPLL 0 [%s] | DPLL 1 [%s] | DPLL 2 [%s] | DPLL 3 [%s] | DPLL 4 [%s] |\n",
                arg_dpll0_en ? "x":" ", arg_dpll1_en ? "x":" ",
                arg_dpll2_en ? "x":" ", arg_dpll3_en ? "x":" ",
                arg_dpll4_en ? "x":" ");
        cli_out("\n");

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("Set", arg, 0)) {
        if (arg_clock_index < 0) {
            cli_out("Inputclk index required\n");
            return CMD_USAGE;
        }

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "L1MUX", PQ_INT, (void*)(size_t)invalid_value,
            &arg_l1mux_index, NULL);
        parse_table_add(&pt, "L1PORT", PQ_INT, (void*)(size_t)invalid_value,
            &arg_l1mux_port, NULL);
        parse_table_add(&pt, "MAC", PQ_MAC, (void*)0, arg_mac, NULL);
        parse_table_add(&pt, "ENable", PQ_BOOL, (void*)(size_t)invalid_value,
            &arg_clk_en, NULL);
        parse_table_add(&pt, "FREQuency", PQ_INT, (void*)(size_t)invalid_value,
            &arg_clock_frequency, NULL);
        parse_table_add(&pt, "TSFREQuency", PQ_INT | PQ_DFL, (void*)0,
            &arg_tsevent_frequency, NULL);
        parse_table_add(&pt, "QL", PQ_MULTI | PQ_DFL, 0, &arg_ql_index, arg_ql_str);
        parse_table_add(&pt, "PRIOrity", PQ_INT, (void*)(size_t)invalid_value,
            &arg_priority, NULL);
        parse_table_add(&pt, "LOCKout", PQ_BOOL, (void*)(size_t)invalid_value,
            &arg_lockout, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

#if defined(BCM_MONTEREY_SUPPORT)
/*        arg_l1mux_port = bcm_esw_ptp_map_synce_clock_port(arg_l1mux_port);*/
        arg_clock_index = get_input_clock_index(arg_clock_index, arg_l1mux_port, arg_l1mux_index);
#endif

        if (arg_l1mux_index != invalid_value && arg_l1mux_port != invalid_value) {
            arg_l1mux.index = arg_l1mux_index;
            arg_l1mux.port = arg_l1mux_port;
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_l1mux_set(
                unit, 0, arg_clock_index, &arg_l1mux));
        }

        if (sal_memcmp(arg_mac, zero_mac, sizeof(bcm_mac_t))) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_mac_set(
                unit, 0, arg_clock_index, &arg_mac));
        }

        if (arg_clock_frequency != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_frequency_set(
                unit, 0,
                arg_clock_index, (uint32)arg_clock_frequency, (uint32)arg_tsevent_frequency));
        }

        if (arg_clk_en != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_enable_set(
                unit, 0,
                arg_clock_index, arg_clk_en));
        }

        if (arg_ql_index != invalid_value) {
            switch (arg_ql_index) {
            case 0:
                arg_ql = bcm_esmc_g781_I_ql_prc;
                break;
            case 1:
                arg_ql = bcm_esmc_g781_I_ql_ssua;
                break;
            case 2:
                arg_ql = bcm_esmc_g781_I_ql_ssub;
                break;
            case 3:
                /* NB: Implement support to distinguish Option I/III QL-SEC. */
                arg_ql = bcm_esmc_g781_I_ql_sec;
                break;
            case 4:
                arg_ql = bcm_esmc_g781_I_ql_dnu;
                break;
            case 5:
                arg_ql = bcm_esmc_g781_II_ql_stu;
                break;
            case 6:
                arg_ql = bcm_esmc_g781_II_ql_prs;
                break;
            case 7:
                arg_ql = bcm_esmc_g781_II_ql_tnc;
                break;
            case 8:
                arg_ql = bcm_esmc_g781_II_ql_st2;
                break;
            case 9:
                arg_ql = bcm_esmc_g781_II_ql_st3;
                break;
            case 10:
                arg_ql = bcm_esmc_g781_II_ql_smc;
                break;
            case 11:
                arg_ql = bcm_esmc_g781_II_ql_st3e;
                break;
            case 12:
                arg_ql = bcm_esmc_g781_II_ql_prov;
                break;
            case 13:
                arg_ql = bcm_esmc_g781_II_ql_dus;
                break;
            case 14:
                arg_ql = bcm_esmc_g781_III_ql_unk;
                break;
            default:
                arg_ql = bcm_esmc_ql_unresolvable;
            }

            if (bcm_esmc_ql_unresolvable == arg_ql) {
                cli_out("TDPLL INPUTCLK SET ... : Unresolvable QL.\n");
            } else {
                PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_ql_set(
                    unit, 0, arg_clock_index, arg_ql));
            }
        }

        if (arg_priority != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_priority_set(
                unit, 0,
                arg_clock_index, arg_priority));
        }

        if (arg_lockout != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_lockout_set(
                unit, 0,
                arg_clock_index, arg_lockout));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("HELP", arg, 0)) {
        cli_out(TDPLL_INPUTCLK_USAGE_1);
        cli_out(TDPLL_INPUTCLK_USAGE_2);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/* TDPLL OUTPUTCLK ... */

#if defined (BCM_APACHE_SUPPORT)
#define TDPLL_OUTPUTCLK_USAGE_AP \
    "TDPLL OUTputclk <subcommand> [...]\n"                                   \
    "\t TDPLL OUTputclk Get <index>\n"                                 \
    "\t                 where <index> = | BroadSync0    | BroadSync1    |\n" \
    "\t                                 | XgPll0        | XgPll1        |\n" \
    "\t                                 | 1588-Hybrid   | GPIO0 | GPIO1 |\n" \
    "\t                                 | GPIO2 | GPIO3 | GPIO4 | GPIO5 |\n" \
	"\t                                 | XgPll2        | XgPll3        |\n" \
    "\t TDPLL OUTputclk Set <index> ENable=<y/n>\n"                    \
    "\t                     FREQuency=<value> TSFREQuency= <value>\n"        \
    "\t                     DerivFREQuency=<value>\n"
#elif defined (BCM_QAX_SUPPORT)
#define TDPLL_OUTPUTCLK_USAGE_QAX \
    "TDPLL OUTputclk <subcommand> [...]\n"                                   \
    "\t TDPLL OUTputclk Get <index>\n"                                 \
    "\t                 where <index> = | BroadSync0    | BroadSync1    |\n" \
    "\t                                 | Pml0Pll       | Pml1Pll       |\n" \
    "\t                                 | 1588-Hybrid   | GPIO0 | GPIO1 |\n" \
    "\t                                 | GPIO2 | GPIO3 | - | - | PmhPll\n" \
    "\t TDPLL OUTputclk Set <index> ENable=<y/n>\n"                    \
    "\t                     FREQuency=<value> TSFREQuency= <value>\n"        \
    "\t                     DerivFREQuency=<value>\n"
#define TDPLL_OUTPUTCLK_USAGE_QUX \
    "TDPLL OUTputclk <subcommand> [...]\n"                                   \
    "\t TDPLL OUTputclk Get <index>\n"                                 \
    "\t                 where <index> = | BroadSync0    | BroadSync1    |\n" \
    "\t                                 | PmlPll        | PmxPll       |\n" \
    "\t                                 | 1588-Hybrid   | TSGPIO0 | TSGPIO1 |\n" \
    "\t                                 | GPIO0 | GPIO1 | - | - |\n" \
    "\t TDPLL OUTputclk Set <index> ENable=<y/n>\n"                    \
    "\t                     FREQuency=<value> TSFREQuency= <value>\n"        \
    "\t                     DerivFREQuency=<value>\n"
#endif

#if defined (BCM_APACHE_SUPPORT)
#define TDPLL_OUTPUTCLK_USAGE TDPLL_OUTPUTCLK_USAGE_AP
#elif defined (BCM_QAX_SUPPORT) && defined (BCM_QUX_SUPPORT)
#define TDPLL_OUTPUTCLK_USAGE TDPLL_OUTPUTCLK_USAGE_QUX
#elif defined (BCM_QAX_SUPPORT) && !defined (BCM_QUX_SUPPORT)
#define TDPLL_OUTPUTCLK_USAGE TDPLL_OUTPUTCLK_USAGE_QAX
#else
#define TDPLL_OUTPUTCLK_USAGE \
    "TDPLL OUTputclk <subcommand> [...]\n"                                   \
    "\t TDPLL OUTputclk Get <index>\n"                                 \
    "\t                 where <index> = | BroadSync0    | BroadSync1    |\n" \
    "\t                                 | SerDesLCPLL   | MasterLCPLL   |\n" \
    "\t                                 | 1588-Hybrid   | GPIO0 | GPIO1 |\n" \
    "\t                                 | GPIO2 | GPIO3 | GPIO4 | GPIO5 |\n" \
    "\t TDPLL OUTputclk Set <index> ENable=<y/n>\n"                    \
    "\t                     FREQuency=<value> TSFREQuency= <value>\n"        \
    "\t                     DerivFREQuency=<value>\n"

#endif

STATIC cmd_result_t
cmd_tdpll_outputclk(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    int invalid_value = 0x7fffffff;

    char *arg_index_str[TDPLL_OUTPUT_CLOCK_NUM_MAX+1] =
        {"BroadSync0", "BroadSync1", "SerDesLCPLL", "MasterLCPLL", "1588-Hybrid",
         "GPIO0", "GPIO1", "GPIO2", "GPIO3", "GPIO4", "GPIO5", 0};
#if defined(BCM_MONTEREY_SUPPORT)
    char *arg_index_str_ap[TDPLL_OUTPUT_CLOCK_NUM_MAX+1] =
           {"BroadSync0", "BroadSync1", "SyncE1", "SyncE2", "SyncE3", "SyncE4", "1588-Hybrid",
            "GPIO0", "GPIO1", "GPIO2", "GPIO3", "GPIO4", "GPIO5", 0};
#elif defined (BCM_APACHE_SUPPORT)
    char *arg_index_str_ap[TDPLL_OUTPUT_CLOCK_NUM_MAX+1] =
           {"BroadSync0", "BroadSync1", "XgPll0", "XgPll1", "1588-Hybrid",
            "GPIO0", "GPIO1", "GPIO2", "GPIO3", "GPIO4", "GPIO5", "XgPll2", "XgPll3", 0};
#elif defined (BCM_QAX_SUPPORT)
    char *arg_index_str_qax[TDPLL_OUTPUT_CLOCK_NUM_MAX+1] =
        {"BroadSync0", "BroadSync1", "Pml0Pll", "Pml1Pll", "1588-Hybrid",
         "TSGPIO0", "TSGPIO1", "GPIO0", "GPIO2", " - ", " - ", "PmhPll", 0};
    char *arg_index_str_qux[TDPLL_OUTPUT_CLOCK_NUM_MAX+1] =
        {"BroadSync0", "BroadSync1", "PmlPll", "PmxPll", "1588-Hybrid",
         "TSGPIO0", "TSGPIO1", "GPIO0", "GPIO2", " - ", " - ", 0};
#endif

    int arg_clock_index = 0;
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
    int rv = BCM_E_NONE;
/*    int arg_l1mux_port = 0; */
    bcm_pbmp_t arg_pbmp;
    char *arg_pbmp_str = 0;
    char *arg_bandwidth_str[8] = {"0.01Hz", "0.05Hz", "0.1Hz", "0.5Hz", "1Hz", "4Hz", "5Hz", "10Hz"};
    int arg_bandwidth_index = invalid_value;

    bcm_tdpll_dpll_bandwidth_t arg_dpll_bandwidth_array[8] =
        {{10, bcm_tdpll_dpll_bandwidth_mHz},
         {50, bcm_tdpll_dpll_bandwidth_mHz},
         {100, bcm_tdpll_dpll_bandwidth_mHz},
         {500, bcm_tdpll_dpll_bandwidth_mHz},
         {1, bcm_tdpll_dpll_bandwidth_Hz},
         {4, bcm_tdpll_dpll_bandwidth_Hz},
         {5, bcm_tdpll_dpll_bandwidth_Hz},
         {10, bcm_tdpll_dpll_bandwidth_Hz}};
#endif

    int arg_clk_en = 0;

    int arg_synth_frequency;
    int arg_tsevent_frequency = 4000;
    int arg_deriv_frequency;

    uint32 arg_synth_frequency_rv;
    uint32 arg_tsevent_frequency_rv;
    uint32 arg_deriv_frequency_rv;

    int arg_tsevent_quotient_rv;
    int arg_deriv_quotient_rv;

    if (arg) {
        char *clock_index_str = ARG_GET(a);
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
        if (clock_index_str) {
            arg_clock_index = 0;
            while (arg_index_str_ap[arg_clock_index]) {
                if (parse_cmp(arg_index_str_ap[arg_clock_index], clock_index_str, 0)) {
                    break;
                }
                ++arg_clock_index;
            }
            if (!arg_index_str_ap[arg_clock_index]) {
                cli_out("%s OUTputclk Get/Set: Invalid Clock index '%s'\n", ARG_CMD(a), clock_index_str);
                cli_out(TDPLL_OUTPUTCLK_USAGE_AP);
                return CMD_FAIL;
            }

            cli_out("### output clock index:%d\n", arg_clock_index);
        }
#elif defined (BCM_APACHE_SUPPORT)
        if (clock_index_str && SOC_IS_APACHE(unit)) {
            arg_clock_index = 0;
            while (arg_index_str_ap[arg_clock_index]) {
                if (parse_cmp(arg_index_str_ap[arg_clock_index], clock_index_str, 0)) {
                    break;
                }
                ++arg_clock_index;
            }
            if (!arg_index_str_ap[arg_clock_index]) {
                cli_out("%s OUTputclk Get/Set: Invalid Clock index '%s'\n", ARG_CMD(a), clock_index_str);
                cli_out(TDPLL_OUTPUTCLK_USAGE_AP);
                return CMD_FAIL;
            }
        } else if (clock_index_str) {
            arg_clock_index = 0;
            while (arg_index_str[arg_clock_index]) {
                if (parse_cmp(arg_index_str[arg_clock_index], clock_index_str, 0)) {
                    break;
                }
                ++arg_clock_index;
            }
            if (!arg_index_str[arg_clock_index]) {
                cli_out("%s OUTputclk Get/Set: Invalid Clock index '%s'\n", ARG_CMD(a), clock_index_str);
                cli_out(TDPLL_OUTPUTCLK_USAGE);
                return CMD_FAIL;
            }
        }
#elif defined (BCM_QAX_SUPPORT)
    if (clock_index_str && SOC_IS_QUX(unit)) {
        arg_clock_index = 0;
        while (arg_index_str_qux[arg_clock_index]) {
            if (parse_cmp(arg_index_str_qux[arg_clock_index], clock_index_str, 0)) {
                break;
            }
            ++arg_clock_index;
        }
        if (!arg_index_str_qux[arg_clock_index]) {
            cli_out("%s OUTputclk Get/Set: Invalid Clock index '%s'\n", ARG_CMD(a), clock_index_str);
            cli_out(TDPLL_OUTPUTCLK_USAGE_QUX);
            return CMD_FAIL;
       }
    } else if (clock_index_str && SOC_IS_QAX(unit)) {
		arg_clock_index = 0;
		while (arg_index_str_qax[arg_clock_index]) {
	        	if (parse_cmp(arg_index_str_qax[arg_clock_index], clock_index_str, 0)) {
				break;
			}
			++arg_clock_index;
		}
		if (!arg_index_str_qax[arg_clock_index]) {
			cli_out("%s OUTputclk Get/Set: Invalid Clock index '%s'\n", ARG_CMD(a), clock_index_str);
			cli_out(TDPLL_OUTPUTCLK_USAGE_QAX);
			return CMD_FAIL;
		}
	} else if (clock_index_str) {
		arg_clock_index = 0;
		while (arg_index_str[arg_clock_index]) {
			if (parse_cmp(arg_index_str[arg_clock_index], clock_index_str, 0)) {
				break;
			}
			++arg_clock_index;
		}
		if (!arg_index_str[arg_clock_index]) {
			cli_out("%s OUTputclk Get/Set: Invalid Clock index '%s'\n", ARG_CMD(a), clock_index_str);
			cli_out(TDPLL_OUTPUTCLK_USAGE);
			return CMD_FAIL;
		}
	}

#else
        if (clock_index_str) {
            arg_clock_index = 0;
            while (arg_index_str[arg_clock_index]) {
                if (parse_cmp(arg_index_str[arg_clock_index], clock_index_str, 0)) {
                    break;
                }
                ++arg_clock_index;
            }
            if (!arg_index_str[arg_clock_index]) {
                cli_out("%s OUTputclk Get/Set: Invalid Clock index '%s'\n", ARG_CMD(a), clock_index_str);
                cli_out(TDPLL_OUTPUTCLK_USAGE);
                return CMD_FAIL;
            }
        }
#endif
    }

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("Get", arg, 0)) {
        PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_enable_get(
            unit, 0, arg_clock_index, &arg_clk_en));

        PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_synth_frequency_get(
            unit, 0, arg_clock_index,
            &arg_synth_frequency_rv, &arg_tsevent_frequency_rv));
        arg_tsevent_quotient_rv = arg_tsevent_frequency_rv ?
            (arg_synth_frequency_rv + (arg_tsevent_frequency_rv >> 1))/arg_tsevent_frequency_rv : -1;

        PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_deriv_frequency_get(
            unit, 0, arg_clock_index,
            &arg_deriv_frequency_rv));
        arg_deriv_quotient_rv = arg_deriv_frequency_rv ?
            (arg_synth_frequency_rv + (arg_deriv_frequency_rv >> 1))/arg_deriv_frequency_rv : -1;

        cli_out("\n");
        cli_out("T-DPLL Output Clock Data and Metadata.\n");
        cli_out("Unit: %d   Stack: %d\n",
                unit, 0);
        cli_out("-----------------------------------------------------------"
                "---------------------\n");
#if defined (BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            cli_out("Index      : %d (%s)\n", arg_clock_index,
                    /* coverity[check_after_sink : FALSE] */
                    (arg_clock_index >= 0 && arg_clock_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str_ap[arg_clock_index] : "Unknown");
        } else {
            cli_out("Index      : %d (%s)\n", arg_clock_index,
                    /* coverity[check_after_sink : FALSE] */
                    (arg_clock_index >= 0 && arg_clock_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str[arg_clock_index] : "Unknown");
        }
#elif defined (BCM_QAX_SUPPORT)
        if (SOC_IS_QUX(unit)) {
            cli_out("Index      : %d (%s)\n", arg_clock_index,
                    (arg_clock_index >= 0 && arg_clock_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str_qux[arg_clock_index] : "Unknown");
        } else if (SOC_IS_QAX(unit)) {
            cli_out("Index      : %d (%s)\n", arg_clock_index,
                    (arg_clock_index >= 0 && arg_clock_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str_qax[arg_clock_index] : "Unknown");
        }else {
            cli_out("Index      : %d (%s)\n", arg_clock_index,
                    (arg_clock_index >= 0 && arg_clock_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str[arg_clock_index] : "Unknown");
        }
#else
        cli_out("Index      : %d (%s)\n", arg_clock_index,
                (arg_clock_index >= 0 && arg_clock_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                arg_index_str[arg_clock_index] : "Unknown");
#endif
        cli_out("\n");
        cli_out("Enable     : [%s]\n", arg_clk_en ? "x":" ");
        cli_out("\n");        
        cli_out("Synth f    : %u Hz TS f: %u Hz (Q: %d)\n", (unsigned)arg_synth_frequency_rv,
                (unsigned)arg_tsevent_frequency_rv, arg_tsevent_quotient_rv);
        cli_out("Deriv f    : %u Hz (Q: %d)\n",
                (unsigned)arg_deriv_frequency_rv, arg_deriv_quotient_rv);
        cli_out("\n");

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("Set", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "ENable", PQ_BOOL, (void*)(size_t)invalid_value,
            &arg_clk_en, NULL);
        parse_table_add(&pt, "FREQuency", PQ_INT, (void*)(size_t)invalid_value,
            &arg_synth_frequency, NULL);
        parse_table_add(&pt, "TSFREQuency", PQ_INT | PQ_DFL, (void*)0,
            &arg_tsevent_frequency, NULL);
        parse_table_add(&pt, "DerivFREQuency", PQ_INT, (void*)(size_t)invalid_value,
            &arg_deriv_frequency, NULL);
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
        parse_table_add(&pt, "PBM", PQ_STRING, (void*)"", &arg_pbmp_str, NULL);
        parse_table_add(&pt, "DPLLBandWidth", PQ_MULTI | PQ_DFL, 0,
            &arg_bandwidth_index, arg_bandwidth_str);
#endif

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
        /* PBMP is valid only for synce clock */
        if ((arg_clock_index >= 2) && (arg_clock_index <= 5)) {
            rv = parse_bcm_pbmp(unit, arg_pbmp_str, &arg_pbmp);
            if (BCM_FAILURE(rv)) {
                cli_out("Invalid port bitmap\n");
                return CMD_FAIL;
            }
        }
#endif
        parse_arg_eq_done(&pt);

        if (arg_synth_frequency != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_synth_frequency_set(
                unit, 0, arg_clock_index,
                (uint32)arg_synth_frequency, (uint32)arg_tsevent_frequency));
        }

        if (arg_deriv_frequency != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_deriv_frequency_set(
                unit, 0, arg_clock_index,
                (uint32)arg_deriv_frequency));
        }

#if defined(BCM_MONTEREY_SUPPORT) && defined(INCLUDE_GDPLL)
        /* Default set the bw to 4Hz */
        arg_bandwidth_index = (arg_bandwidth_index == invalid_value) ? 5: arg_bandwidth_index;
        PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_create(unit, 0/*stack_id*/, arg_clock_index,
            arg_pbmp, &arg_dpll_bandwidth_array[arg_bandwidth_index]));
#endif
        if (arg_clk_en != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_enable_set(
                unit, 0, arg_clock_index, arg_clk_en));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("HELP", arg, 0)) {
#if defined (BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            cli_out(TDPLL_OUTPUTCLK_USAGE_AP);
        } else {
            cli_out(TDPLL_OUTPUTCLK_USAGE);
        }
#elif defined (BCM_QAX_SUPPORT)
        if (SOC_IS_QUX(unit)) {
            cli_out(TDPLL_OUTPUTCLK_USAGE_QUX);
        } else if (SOC_IS_QAX(unit)) {
            cli_out(TDPLL_OUTPUTCLK_USAGE_QAX);
        }else {
            cli_out(TDPLL_OUTPUTCLK_USAGE);
	}
#else
        cli_out(TDPLL_OUTPUTCLK_USAGE);
#endif
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/* TDPLL MONITOR ... */
#define TDPLL_MONITOR_USAGE \
    "TDPLL MONitor <subcommand> [...]\n"                                   \
    "\t TDPLL MONitor ConTRoL CallBackENable=<y/n>\n"                      \
    "\t TDPLL MONitor Get\n"                                               \
    "\t TDPLL MONitor Set INTERVAL=<value-sec>\n"                          \
    "\t                   SoFTWaRN=<value-ppb>\n"                          \
    "\t                   HaRDACCept=<value-ppb> HaRDREJect=<value-ppb>\n"

STATIC cmd_result_t
cmd_tdpll_monitor(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    int arg_callback_en;

    int invalid_value = 0x7fffffff;
    int arg_monitor_interval = 0;
    int arg_softwarn_ppb = 0;
    int arg_hardaccept_ppb = 0;
    int arg_hardreject_ppb = 0;

    uint32 arg_monitor_interval_rv;
    uint32 arg_softwarn_ppb_rv;
    uint32 arg_hardaccept_ppb_rv;
    uint32 arg_hardreject_ppb_rv;

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("ConTRoL", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "CallBackENable", PQ_BOOL, (void*)0, &arg_callback_en, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        if (arg_callback_en) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_callback_register(
                unit, 0, diag_tdpll_input_clock_monitor_callback));
            cli_out("Registering diag_tdpll_input_clock_callback for type bcmTdpllCallbackTypeMonitor\n");
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_callback_register(
                unit, 0, bcmTdpllCallbackTypeMonitor, diag_tdpll_input_clock_callback));
        } else {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_callback_unregister(
                unit, 0));
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_callback_unregister(
                unit, 0, bcmTdpllCallbackTypeMonitor));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("Get", arg, 0)) {
        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_interval_get(
            unit, 0, &arg_monitor_interval_rv));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_threshold_get(
            unit, 0,
            bcm_tdpll_input_clock_monitor_type_soft_warn, &arg_softwarn_ppb_rv));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_threshold_get(
            unit, 0,
            bcm_tdpll_input_clock_monitor_type_hard_accept, &arg_hardaccept_ppb_rv));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_threshold_get(
            unit, 0,
            bcm_tdpll_input_clock_monitor_type_hard_reject, &arg_hardreject_ppb_rv));

        cli_out("\n");
        cli_out("T-DPLL Input Clock Monitoring Parameters and Thresholds.\n");
        cli_out("Unit: %d   Stack: %d\n",
                unit, PTP_STACK_ID_DEFAULT);
        cli_out("-----------------------------------------------------------"
                "---------------------\n");
        cli_out("Interval   : %u sec\n", (unsigned)arg_monitor_interval_rv);
        cli_out("Soft-Warn  : %u ppb\n", (unsigned)arg_softwarn_ppb_rv);
        cli_out("Hard-Accept: %u ppb\n", (unsigned)arg_hardaccept_ppb_rv);
        cli_out("Hard-Reject: %u ppb\n", (unsigned)arg_hardreject_ppb_rv);
        cli_out("\n");

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("Set", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "INTERVAL", PQ_INT, (void*)(size_t)invalid_value, &arg_monitor_interval, NULL);
        parse_table_add(&pt, "SoFTWaRN", PQ_INT, (void*)(size_t)invalid_value, &arg_softwarn_ppb, NULL);
        parse_table_add(&pt, "HaRDACCept", PQ_INT, (void*)(size_t)invalid_value, &arg_hardaccept_ppb, NULL);
        parse_table_add(&pt, "HaRDREJect", PQ_INT, (void*)(size_t)invalid_value, &arg_hardreject_ppb, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        if (arg_monitor_interval != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_interval_set(
                unit, 0, (uint32)arg_monitor_interval));
        }
        if (arg_softwarn_ppb != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_threshold_set(
                unit, 0,
                bcm_tdpll_input_clock_monitor_type_soft_warn, (uint32)arg_softwarn_ppb));
        }
        if (arg_hardaccept_ppb != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_threshold_set(
                unit, 0,
                bcm_tdpll_input_clock_monitor_type_hard_accept, (uint32)arg_hardaccept_ppb));
        }
        if (arg_hardreject_ppb != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_monitor_threshold_set(
                unit, 0,
                bcm_tdpll_input_clock_monitor_type_hard_reject, (uint32)arg_hardreject_ppb));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("HELP", arg, 0)) {
        cli_out(TDPLL_MONITOR_USAGE);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/* TDPLL SELECT ... */
#define TDPLL_SELECT_USAGE \
    "TDPLL SELect <subcommand> [...]\n"                                           \
    "\t TDPLL SELect ConTRoL CallBackENable=<y/n>\n"                              \
    "\t TDPLL SELect Get <dpll_index>\n"                                          \
    "\t TDPLL SELect Set <dpll_index> QLENabled=<y/n>\n"

STATIC cmd_result_t
cmd_tdpll_select(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    int arg_callback_en;

    int invalid_value = 0x7fffffff;

    char *dpll_index_str;
    int arg_dpll_index = 0;

    char *arg_reference_str[TDPLL_INPUT_CLOCK_NUM_MAX+1] =
        {"GPIO0","GPIO1","GPIO2","GPIO3","GPIO4","GPIO5",
         "SyncE1","SyncE2","1588-Freq", "1588-Phase", 0};

    int arg_ql_enabled = 0;
    int arg_best_clock = 0;

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("ConTRoL", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "CallBackENable", PQ_BOOL, (void*)0, &arg_callback_en, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        if (arg_callback_en) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_selector_callback_register(
                unit, 0, diag_tdpll_input_clock_selector_callback));
            cli_out("Registering diag_tdpll_input_clock_callback for type bcmTdpllCallbackTypeSelector \n");
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_callback_register(
                unit, 0, bcmTdpllCallbackTypeSelector, diag_tdpll_input_clock_callback));
        } else {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_selector_callback_unregister(
                unit, 0));
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_callback_unregister(
                unit, 0, bcmTdpllCallbackTypeSelector));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    }

    dpll_index_str = ARG_GET(a);
    if (dpll_index_str) {
        if (!isint(dpll_index_str)) {
            cli_out("%s SELect Get/Set: Invalid DPLL index '%s'\n", ARG_CMD(a), dpll_index_str);
            cli_out(TDPLL_SELECT_USAGE);
            return CMD_FAIL;
        }
        arg_dpll_index = parse_integer(dpll_index_str);
    }

    if (parse_cmp("Get", arg, 0)) {
        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_ql_enabled_get(
            unit, 0,
            arg_dpll_index, &arg_ql_enabled));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_best_get(
            unit, 0,
            arg_dpll_index, &arg_best_clock));

        cli_out("\n");
        cli_out("T-DPLL Reference Selection Configuration and State Data.\n");
        cli_out("Unit: %d   Stack: %d\n",
                unit, 0);
        cli_out("-----------------------------------------------------------"
                "---------------------\n");
        cli_out("DPLL  : %d\n", arg_dpll_index);
        cli_out("Modes : QL-Enabled [%s]\n", arg_ql_enabled ? "x":" ");
        cli_out("Best  : %d (%s)\n", arg_best_clock,
                (arg_best_clock >= 0 && arg_best_clock < TDPLL_INPUT_CLOCK_NUM_MAX) ?
                arg_reference_str[arg_best_clock] : "Unknown");
        cli_out("\n");

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("Set", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "QLENabled", PQ_BOOL, (void*)(size_t)invalid_value, &arg_ql_enabled, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        if (arg_ql_enabled != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_ql_enabled_set(
                unit, 0,
                arg_dpll_index, arg_ql_enabled));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("HELP", arg, 0)) {
        cli_out(TDPLL_SELECT_USAGE);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/* TDPLL SELECT ... */
#define TDPLL_NOTIFICATION_USAGE \
    "TDPLL NOTIfication ConTRoL CallBackENable=<y/n>\n"

STATIC cmd_result_t
cmd_tdpll_notification(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    int arg_callback_en;

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("ConTRoL", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "CallBackENable", PQ_BOOL, (void*)0, &arg_callback_en, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        if (arg_callback_en) {
            cli_out("Registering diag_tdpll_input_clock_callback for type bcmTdpllCallbackTypeNotification \n");
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_callback_register(
                unit, 0, bcmTdpllCallbackTypeNotification, diag_tdpll_input_clock_callback));
        } else {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_callback_unregister(
                unit, 0, bcmTdpllCallbackTypeNotification));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}
/* TDPLL SWITCH ... */
#define TDPLL_SWITCH_USAGE \
    "TDPLL SWitch <subcommand> [...]\n"                    \
    "\t TDPLL SWitch ConTRoL ENable=<y/n>\n"               \
    "\t TDPLL SWitch Get <dpll_index>\n"                  \
    "\t TDPLL SWitch Set <dpll_index> REVertive=<y/n>\n"

STATIC cmd_result_t
cmd_tdpll_switch(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    int invalid_value = 0x7fffffff;
    int arg_dpc_en = 0;

    char *dpll_index_str;
    int arg_dpll_index = 0;

    char *arg_reference_str[TDPLL_INPUT_CLOCK_NUM_MAX] =
        {"GPIO0","GPIO1","GPIO2","GPIO3","GPIO4","GPIO5",
         "SyncE1","SyncE2","1588-Freq","1588-Phase"};

    int arg_revertive = 0;

    int arg_best_clock = 0;
    int arg_dpll_ref[TDPLL_DPLL_INSTANCE_NUM_MAX];
    int arg_num_dpll;

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("ConTRoL", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "ENable", PQ_BOOL, (void*)(size_t)invalid_value, &arg_dpc_en, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        if (invalid_value != arg_dpc_en) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_control(unit, 0, arg_dpc_en));
        }
        PTP_CLI_RESULT_RETURN(CMD_OK);

    }

    dpll_index_str = ARG_GET(a);
    if (dpll_index_str) {
        if (!isint(dpll_index_str)) {
            cli_out("%s SWitch Get/Set: Invalid DPLL index '%s'\n", ARG_CMD(a), dpll_index_str);
            cli_out(TDPLL_SWITCH_USAGE);
            return CMD_FAIL;
        }
        arg_dpll_index = parse_integer(dpll_index_str);
    }

    if (parse_cmp("Get", arg, 0)) {
        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_revertive_get(
            unit, 0,
            arg_dpll_index, &arg_revertive));

        PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_best_get(
            unit, 0,
            arg_dpll_index, &arg_best_clock));

        PTP_IF_ERROR_RETURN(bcm_tdpll_dpll_reference_get(
            unit, 0,
            TDPLL_DPLL_INSTANCE_NUM_MAX, arg_dpll_ref, &arg_num_dpll));

        cli_out("\n");
        cli_out("T-DPLL Reference Switching Configuration and State Data.\n");
        cli_out("Unit: %d   Stack: %d\n",
                unit, 0);
        cli_out("-----------------------------------------------------------"
                "---------------------\n");
        cli_out("DPLL  : %d\n", arg_dpll_index);
        cli_out("Modes : | Revertive [%s] |\n",
                arg_revertive ? "x":" ");
        cli_out("Ref.  : %d %s (%d %s)\n",
                (arg_dpll_index >= 0 && arg_dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX) ?
                arg_dpll_ref[arg_dpll_index] : -1,
                (arg_dpll_index >= 0 && arg_dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX) &&
                (arg_dpll_ref[arg_dpll_index] >= 0) &&
                (arg_dpll_ref[arg_dpll_index] < TDPLL_INPUT_CLOCK_NUM_MAX) ?
                arg_reference_str[arg_dpll_ref[arg_dpll_index]] : "Unknown",
                arg_best_clock,
                (arg_dpll_index >= 0 && arg_dpll_index < TDPLL_DPLL_INSTANCE_NUM_MAX) &&
                (arg_best_clock >= 0) &&
                (arg_best_clock < TDPLL_INPUT_CLOCK_NUM_MAX) ?
                arg_reference_str[arg_best_clock] : "Unknown");
        cli_out("\n");

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("Set", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "REVertive", PQ_BOOL, (void*)(size_t)invalid_value, &arg_revertive, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        if (arg_revertive != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_revertive_set(
                unit, 0,
                arg_dpll_index, arg_revertive));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("HELP", arg, 0)) {
        cli_out(TDPLL_SWITCH_USAGE);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/* TDPLL HOLDOVER ... */
/* We divide the string because pedantic compiler doesn't allow strings longer then 509 characters */
#define TDPLL_HOLDOVER_USAGE_1_AP                                                        \
    "TDPLL HOLDover <subcommand> [...]\n"                                           \
    "\t TDPLL HOLDover Get <output>\n"                                         \
    "\t                where <output> = | BroadSync0  | BroadSync1  |\n"             \
    "\t                                 | XgPll0      | XgPll1      |\n"             \
    "\t                                 | 1588-Hybrid | XgPll2      | XgPll3 \n"
#define TDPLL_HOLDOVER_USAGE_1_QAX                                              \
    "TDPLL HOLDover <subcommand> [...]\n"					\
    "\t TDPLL HOLDover Get <output>\n"				                \
    "\t                where <output> = | BroadSync0  | BroadSync1	|\n"    \
    "\t					| Pml0Pll     | Pml1Pll		|\n"    \
    "\t 				| 1588-Hybrid | PmhPll          \n"
#define TDPLL_HOLDOVER_USAGE_1_QUX                                              \
    "TDPLL HOLDover <subcommand> [...]\n"					\
    "\t TDPLL HOLDover Get <output>\n"				                \
    "\t                where <output> = | BroadSync0  | BroadSync1	|\n"    \
    "\t					| PmlPll        | PmxPll		|\n"    \
    "\t 				| 1588-Hybrid        \n"

#define TDPLL_HOLDOVER_USAGE_2                                                        \
    "\t TDPLL HOLDover Set <ouput> [FREQuency=<freqppt>] [Mode=<mode>]\n"     \
    "\t                where <mode> = | INSTantaneous | ONEsecond   | MANual |\n"   \
    "\t                               | FASTaverage   | SLOWaverage |\n"            \
    "\t TDPLL HOLDover RESET <output>\n"

#if defined (BCM_APACHE_SUPPORT)
#define TDPLL_HOLDOVER_USAGE_1 TDPLL_HOLDOVER_USAGE_1_AP
#elif defined (BCM_QAX_SUPPORT) && defined (BCM_QUX_SUPPORT)
#define TDPLL_HOLDOVER_USAGE_1 TDPLL_HOLDOVER_USAGE_1_QUX
#elif defined (BCM_QAX_SUPPORT) && !defined (BCM_QUX_SUPPORT)
#define TDPLL_HOLDOVER_USAGE_1 TDPLL_HOLDOVER_USAGE_1_QAX
#else
#define TDPLL_HOLDOVER_USAGE_1                                                        \
    "TDPLL HOLDover <subcommand> [...]\n"                                           \
    "\t TDPLL HOLDover Get <output>\n"                                         \
    "\t                where <output> = | BroadSync0  | BroadSync1  |\n"             \
    "\t                                 | SerDesLCPLL | MasterLCPLL |\n"             \
    "\t                                 | 1588-Hybrid |              \n"
#endif

STATIC cmd_result_t
cmd_tdpll_holdover(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    bcm_tdpll_holdover_data_t arg_hdata;
    bcm_tdpll_holdover_mode_t arg_hmode = bcm_tdpll_holdover_mode_slow_average;

    char *arg_index_str[TDPLL_OUTPUT_CLOCK_NUM_MAX+1] =
        {"BroadSync0", "BroadSync1", "SerDesLCPLL", "MasterLCPLL", "1588-Hybrid", 0};
#if defined(BCM_APACHE_SUPPORT)
		/* Initializing with dummy string for GPIO clocks to retain same indices for XGPLL2/3 */
    char *arg_index_str_ap[TDPLL_OUTPUT_CLOCK_NUM_MAX+1] =
        {"BroadSync0", "BroadSync1", "XgPll0", "XgPll1", "1588-Hybrid", "dummy1",  "dummy2",
		    "dummy3", "dummy4", "dummy5", "dummy6","XgPll2", "XgPll3", 0};
#elif defined(BCM_QAX_SUPPORT)
    char *arg_index_str_qux[TDPLL_OUTPUT_CLOCK_NUM_MAX+1] =
        {"BroadSync0", "BroadSync1", "PmlPll", "PmxPll", "1588-Hybrid", "dummy1",  "dummy2",
                    "dummy3", "dummy4", "dummy5", "dummy6",  0};
    char *arg_index_str_qax[TDPLL_OUTPUT_CLOCK_NUM_MAX+1] =
        {"BroadSync0", "BroadSync1", "Pml0Pll", "Pml1Pll", "1588-Hybrid", "dummy1",  "dummy2",
                    "dummy3", "dummy4", "dummy5", "dummy6","PmhPll",  0};
   
#endif
    int arg_index = 0;
    int arg_freqppt;

    int invalid_value = 0x7fffffff;
    char *arg_mode_str[6] = {"INSTantaneous", "ONEsecond", "MANual", "FASTaverage", "SLOWaverage", 0};

    if (arg) {
        char *clock_index_str = ARG_GET(a);
#if defined (BCM_APACHE_SUPPORT)
        if (clock_index_str && SOC_IS_APACHE(unit)) {
            arg_index = 0;
            while (arg_index_str_ap[arg_index]) {
                if (parse_cmp(arg_index_str_ap[arg_index], clock_index_str, 0)) {
                    break;
                }
                ++arg_index;
            }
            if (!arg_index_str_ap[arg_index]) {
                cli_out("%s HOLDover Get/Set: Invalid Output '%s'\n", ARG_CMD(a), clock_index_str);
                cli_out(TDPLL_HOLDOVER_USAGE_1_AP);
                cli_out(TDPLL_HOLDOVER_USAGE_2);
                return CMD_FAIL;
            }
        } else
#elif defined (BCM_QAX_SUPPORT)
        if (clock_index_str && SOC_IS_QUX(unit)) {
            arg_index = 0;
            while (arg_index_str_qux[arg_index]) {
                if (parse_cmp(arg_index_str_qux[arg_index], clock_index_str, 0)) {
                    break;
                }
                ++arg_index;
            }
            if (!arg_index_str_qux[arg_index]) {
                cli_out("%s HOLDover Get/Set: Invalid Output '%s'\n", ARG_CMD(a), clock_index_str);
                cli_out(TDPLL_HOLDOVER_USAGE_1_QUX);
                cli_out(TDPLL_HOLDOVER_USAGE_2);
                return CMD_FAIL;
            }
        } else if (clock_index_str && SOC_IS_QAX(unit)) {
            arg_index = 0;
            while (arg_index_str_qax[arg_index]) {
                if (parse_cmp(arg_index_str_qax[arg_index], clock_index_str, 0)) {
                    break;
                }
                ++arg_index;
            }
            if (!arg_index_str_qax[arg_index]) {
                cli_out("%s HOLDover Get/Set: Invalid Output '%s'\n", ARG_CMD(a), clock_index_str);
                cli_out(TDPLL_HOLDOVER_USAGE_1_QAX);
                cli_out(TDPLL_HOLDOVER_USAGE_2);
                return CMD_FAIL;
            }
        } else

#endif
        if (clock_index_str) {
            arg_index = 0;
            while (arg_index_str[arg_index]) {
                if (parse_cmp(arg_index_str[arg_index], clock_index_str, 0)) {
                    break;
                }
                ++arg_index;
            }
            if (!arg_index_str[arg_index]) {
                cli_out("%s HOLDover Get/Set: Invalid Output '%s'\n", ARG_CMD(a), clock_index_str);
                cli_out(TDPLL_HOLDOVER_USAGE_1);
                cli_out(TDPLL_HOLDOVER_USAGE_2);
                return CMD_FAIL;
            }
        }
    }

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("Get", arg, 0)) {
        PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_holdover_data_get(unit, 0,
            arg_index, &arg_hdata));

        cli_out("\n");
        cli_out("T-DPLL Holdover State and Configuration Data.\n");
        cli_out("Unit: %d   Stack: %d\n",
                unit, 0);
        cli_out("-----------------------------------------------------------"
                "---------------------\n");
#if defined (BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            cli_out("Index      : %d (%s)\n", arg_index,
                    /* coverity[check_after_sink : FALSE] */
                    (arg_index >= 0 && arg_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str_ap[arg_index] : "Unknown");
        } else {
            cli_out("Index      : %d (%s)\n", arg_index,
                    /* coverity[check_after_sink : FALSE] */
                    (arg_index >= 0 && arg_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str[arg_index] : "Unknown");
        }
#elif defined (BCM_QAX_SUPPORT)
        if (SOC_IS_QUX(unit)) {
            cli_out("Index      : %d (%s)\n", arg_index,
                    (arg_index >= 0 && arg_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str_qux[arg_index] : "Unknown");
        } else if (SOC_IS_QAX(unit)) {
            cli_out("Index      : %d (%s)\n", arg_index,
                    (arg_index >= 0 && arg_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str_qax[arg_index] : "Unknown");
        } else {
            cli_out("Index      : %d (%s)\n", arg_index,
                    (arg_index >= 0 && arg_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                    arg_index_str[arg_index] : "Unknown");
        }

#else
        cli_out("Index      : %d (%s)\n", arg_index,
                (arg_index >= 0 && arg_index < TDPLL_OUTPUT_CLOCK_NUM_MAX) ?
                arg_index_str[arg_index] : "Unknown");
#endif
        cli_out("Mode       : | [%s] Instantaneous | [%s] 1sec | [%s] Manual | [%s] Fast-Avg | [%s] Slow-Avg |\n",
                arg_hdata.mode == bcm_tdpll_holdover_mode_instantaneous ? "x" : " ",
                arg_hdata.mode == bcm_tdpll_holdover_mode_avg1s ? "x" : " ",
                arg_hdata.mode == bcm_tdpll_holdover_mode_manual ? "x" : " ",
                arg_hdata.mode == bcm_tdpll_holdover_mode_fast_average ? "x" : " ",
                arg_hdata.mode == bcm_tdpll_holdover_mode_slow_average ? "x" : " ");
        cli_out("f (Instantaneous): %d ppt\n", arg_hdata.freq_instantaneous);
        cli_out("f (1sec Average) : %d ppt\n", arg_hdata.freq_avg1s);
        cli_out("f (Manual)       : %d ppt\n", arg_hdata.freq_manual);
        cli_out("f (Fast Average) : %d ppt [%s] Valid\n", arg_hdata.freq_fast_average,
                arg_hdata.freq_fast_average_valid ? "x" : " ");
        cli_out("f (Slow Average) : %d ppt [%s] Valid\n", arg_hdata.freq_slow_average,
                arg_hdata.freq_slow_average_valid ? "x" : " ");
        cli_out("\n");

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("Set", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "FREQuency", PQ_INT, (void*)(size_t)invalid_value, &arg_freqppt, NULL);
        parse_table_add(&pt, "Mode", PQ_MULTI | PQ_DFL, 0, &arg_hmode, arg_mode_str);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_holdover_mode_set(unit, 0, arg_index, arg_hmode));

        if (arg_freqppt != invalid_value) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_holdover_frequency_set(unit, 0, arg_index, arg_freqppt));
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("RESET", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "INDeX", PQ_MULTI | PQ_DFL, 0, &arg_index, &arg_index_str);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }
        parse_arg_eq_done(&pt);

        PTP_IF_ERROR_RETURN(bcm_tdpll_output_clock_holdover_reset(unit, 0, arg_index));
        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("HELP", arg, 0)) {
#if defined (BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            cli_out(TDPLL_HOLDOVER_USAGE_1_AP);
        }
        else {
            cli_out(TDPLL_HOLDOVER_USAGE_1);
        }
#elif defined (BCM_QAX_SUPPORT)
        if (SOC_IS_QUX(unit)) {
            cli_out(TDPLL_HOLDOVER_USAGE_1_QUX);
        } else if (SOC_IS_QAX(unit)) {
            cli_out(TDPLL_HOLDOVER_USAGE_1_QAX);
        } else {
            cli_out(TDPLL_HOLDOVER_USAGE_1);
        }
#else
        cli_out(TDPLL_HOLDOVER_USAGE_1);
#endif
        cli_out(TDPLL_HOLDOVER_USAGE_2);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}


/* TDPLL ESMC ... */
/* We divide the string because pedantic compiler doesn't allow strings longer then 509 characters */
#define TDPLL_ESMC_USAGE_1 \
    "TDPLL ESMC <subcommand> [...]\n"                                             \
    "\t TDPLL ESMC Get <dpll_index>\n"                                            \
    "\t TDPLL ESMC Set <dpll_index> MAC=<mac> QL=<QL>\n"                          \
    "\t                HoldoverQL=<QL> G781=<OPTionI|OPTionII|OPTionIII>\n"       \
    "\t                RXENable=<y/n> TXENable=<y/n>\n"                           \
    "\t                RXPortBitMaP=<port bitmap> TXPortBitMaP=<port bitmap>\n"   \
    "\t                G8264ESSMCode=<essm_code>\n"   \
    "\t TDPLL ESMC Register <dpll_index> G8273PhaseTransientEnable=<y/n>\n"       \
    "\t            where <QL> = | QL-PRC  | QL-SSU-A | QL-SSU-B | (Option I)\n"
#define TDPLL_ESMC_USAGE_2 \
    "\t                         | QL-SEC  | QL-DNU   |            (Option I)\n"   \
    "\t                         | QL-STU  | QL-PRS   | QL-TNC   | (Option II)\n"  \
    "\t                         | QL-ST2  | QL-ST3   | QL-SMC   | (Option II)\n"  \
    "\t                         | QL-ST3E | QL-PROV  | QL-DUS   | (Option II)\n"  \
    "\t                         | QL-UNK  | QL-SEC   |            (Option III)\n" \
    "\t            NOTE: G781 and  RXENable are global attributes, which apply\n" \
    "\t                  to all T-DPLL instances.\n"                              
#define TDPLL_ESMC_USAGE_3 \
    "\t            where <essm_code> = \n"                                       \
    "\t                 <0 - bcmEsmcEssmCodeNone> | <0xff - bcmEsmcEssmCodeEEC>\n"

STATIC cmd_result_t
cmd_tdpll_esmc(int unit, args_t *a)
{
    parse_table_t pt;
    const char *arg = ARG_GET(a);

    int invalid_value = 0x7fffffff;

    char *dpll_index_str;
    int arg_dpll_index = 0;
    int arg_rx_enable = invalid_value;
    int arg_tx_enable = invalid_value;

    char *arg_rx_pbmp_str = 0;
    char *arg_tx_pbmp_str = 0;
    bcm_pbmp_t arg_rx_pbmp;
    bcm_pbmp_t arg_tx_pbmp;

    char *rxbufp = NULL;
    char *txbufp = NULL;
    int rxbufp_len;
    int txbufp_len;

    int rxrv;
    int txrv;

    bcm_mac_t arg_mac = {0x00, 0x10, 0x18, 0x00, 0x00, 0x00};
    bcm_mac_t zero_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    char *arg_g781_str[4] = {"OPTionI", "OPTionII", "OPTionIII", 0};
    int arg_g781_index = invalid_value;
    bcm_esmc_network_option_t arg_g781_option;

    char *arg_ql_str[16] = {"QL-PRC", "QL-SSU-A", "QL-SSU-B", "QL-SEC", "QL-DNU",
                            "QL-STU", "QL-PRS", "QL-TNC", "QL-ST2", "QL-ST3",
                            "QL-SMC", "QL-ST3E", "QL-PROV", "QL-DUS", "QL-UNK", 0};

    int arg_ql_index = invalid_value;
    int arg_holdover_ql_index = invalid_value;

    bcm_esmc_quality_level_t arg_ql;
    bcm_esmc_quality_level_t arg_holdover_ql;
    int g8273_phase_transient_enable = 0;
    bcm_esmc_essm_code_t essm_code = bcmEsmcEssmCodeNone;

    dpll_index_str = ARG_GET(a);
    if (dpll_index_str) {
        if (!isint(dpll_index_str)) {
            cli_out("%s ESMC Get/Set: Invalid DPLL index '%s'\n", ARG_CMD(a), dpll_index_str);
            cli_out(TDPLL_ESMC_USAGE_1);
            cli_out(TDPLL_ESMC_USAGE_2);
            cli_out(TDPLL_ESMC_USAGE_3);
            return CMD_FAIL;
        }
        arg_dpll_index = parse_integer(dpll_index_str);
    }

    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    } else if (parse_cmp("Get", arg, 0)) {
        PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_mac_get(
            unit, 0, arg_dpll_index, &arg_mac));

        PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_ql_get(
            unit, 0, arg_dpll_index, &arg_ql));

#ifndef BCM_ESMC_EXTDPLL_SUPPORT
        PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_holdover_ql_get(
            unit, 0, arg_dpll_index, &arg_holdover_ql));
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */

        PTP_IF_ERROR_RETURN(bcm_esmc_g781_option_get(
            unit, 0, &arg_g781_option));

        PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_rx_enable_get(
            unit, 0, &arg_rx_enable));

        PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_tx_enable_get(
            unit, 0, arg_dpll_index, &arg_tx_enable));

        PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_rx_portbitmap_get(
            unit, 0, arg_dpll_index, &arg_rx_pbmp));

        PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_tx_portbitmap_get(
            unit, 0, arg_dpll_index, &arg_tx_pbmp));

        rxbufp_len = FORMAT_PBMP_MAX * sizeof(*rxbufp);
        rxbufp = sal_alloc(rxbufp_len, "Rx ESMC portbitmap print buffer");
        if (NULL == rxbufp) {
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }
        format_bcm_pbmp(unit, rxbufp, rxbufp_len, arg_rx_pbmp);

        txbufp_len = FORMAT_PBMP_MAX * sizeof(*txbufp);
        txbufp = sal_alloc(txbufp_len, "Tx ESMC portbitmap print buffer");
        if (NULL == txbufp) {
            sal_free(rxbufp);
            PTP_CLI_RESULT_RETURN(CMD_FAIL);
        }
        format_bcm_pbmp(unit, txbufp, txbufp_len, arg_tx_pbmp);

        cli_out("\n");
        cli_out("T-DPLL Ethernet Synchronization Messaging Channel (ESMC).\n");
        cli_out("Unit: %d   Stack: %d\n",
                unit, 0);
        cli_out("-----------------------------------------------------------"
                "---------------------\n");
        cli_out("DPLL        : %d\n", arg_dpll_index);
        cli_out("MAC         : %02x:%02x:%02x:%02x:%02x:%02x\n",
                arg_mac[0], arg_mac[1], arg_mac[2], arg_mac[3], arg_mac[4], arg_mac[5]);
        cli_out("\n");

        cli_out("QL          : %s\n",
                arg_ql == bcm_esmc_g781_I_ql_prc ? "QL-PRC" :
                (arg_ql == bcm_esmc_g781_I_ql_ssua ? "QL-SSU-A" :
                (arg_ql == bcm_esmc_g781_I_ql_ssub ? "QL-SSU-B" :
                (arg_ql == bcm_esmc_g781_I_ql_sec ? "QL-SEC" :
                (arg_ql == bcm_esmc_g781_I_ql_dnu ? "QL-DNU" :
                (arg_ql == bcm_esmc_g781_II_ql_stu ? "QL-STU" :
                (arg_ql == bcm_esmc_g781_II_ql_prs ? "QL-PRS" :
                (arg_ql == bcm_esmc_g781_II_ql_tnc ? "QL-TNC" :
                (arg_ql == bcm_esmc_g781_II_ql_st2 ? "QL-ST2" :
                (arg_ql == bcm_esmc_g781_II_ql_st3 ? "QL-ST3" :
                (arg_ql == bcm_esmc_g781_II_ql_smc ? "QL-SMC" :
                (arg_ql == bcm_esmc_g781_II_ql_st3e ? "QL-ST3E" :
                (arg_ql == bcm_esmc_g781_II_ql_prov ? "QL-PROV" :
                (arg_ql == bcm_esmc_g781_II_ql_dus ? "QL-DUS" :
                (arg_ql == bcm_esmc_g781_III_ql_unk ? "QL-UNK" :
                (arg_ql == bcm_esmc_g781_III_ql_sec ? "QL-SEC" : "Unknown"
                ))))))))))))))));

#ifndef BCM_ESMC_EXTDPLL_SUPPORT
        cli_out("Holdover QL : %s\n",
                arg_holdover_ql == bcm_esmc_g781_I_ql_prc ? "QL-PRC" :
                (arg_holdover_ql == bcm_esmc_g781_I_ql_ssua ? "QL-SSU-A" :
                (arg_holdover_ql == bcm_esmc_g781_I_ql_ssub ? "QL-SSU-B" :
                (arg_holdover_ql == bcm_esmc_g781_I_ql_sec ? "QL-SEC" :
                (arg_holdover_ql == bcm_esmc_g781_I_ql_dnu ? "QL-DNU" :
                (arg_holdover_ql == bcm_esmc_g781_II_ql_stu ? "QL-STU" :
                (arg_holdover_ql == bcm_esmc_g781_II_ql_prs ? "QL-PRS" :
                (arg_holdover_ql == bcm_esmc_g781_II_ql_tnc ? "QL-TNC" :
                (arg_holdover_ql == bcm_esmc_g781_II_ql_st2 ? "QL-ST2" :
                (arg_holdover_ql == bcm_esmc_g781_II_ql_st3 ? "QL-ST3" :
                (arg_holdover_ql == bcm_esmc_g781_II_ql_smc ? "QL-SMC" :
                (arg_holdover_ql == bcm_esmc_g781_II_ql_st3e ? "QL-ST3E" :
                (arg_holdover_ql == bcm_esmc_g781_II_ql_prov ? "QL-PROV" :
                (arg_holdover_ql == bcm_esmc_g781_II_ql_dus ? "QL-DUS" :
                (arg_holdover_ql == bcm_esmc_g781_III_ql_unk ? "QL-UNK" :
                (arg_holdover_ql == bcm_esmc_g781_III_ql_sec ? "QL-SEC" : "Unknown"
                ))))))))))))))));
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */

        cli_out("ITU-T G.781 : %s (%d)\n",
                arg_g781_option == bcm_esmc_network_option_g781_I ? "Option I" :
                (arg_g781_option == bcm_esmc_network_option_g781_II ? "Option II" :
                (arg_g781_option == bcm_esmc_network_option_g781_III ? "Option III" : "Unknown")),
                (int)arg_g781_option);
        cli_out("\n");

        cli_out("Rx Enable   : [%s] (%s)\n", arg_rx_enable ? "x":" ", rxbufp);
        cli_out("Tx Enable   : [%s] (%s)\n", arg_tx_enable ? "x":" ", txbufp);

        cli_out("\n");

        bcm_tdpll_esmc_essm_code_get( unit, 0, arg_dpll_index, &essm_code);
        cli_out("G.8264 ESSM code : ");

        switch(essm_code) {
            case bcmEsmcEssmCodeNone:
                cli_out("bcmEsmcEssmCodeNone\n");
                break;
            case bcmEsmcEssmCodeEEC:
                cli_out("bcmEsmcEssmCodeEEC\n");
                break;
            case bcmEsmcEssmCodePRTC:
                cli_out("bcmEsmcEssmCodePRTC\n");
                break;
            case bcmEsmcEssmCodeEPRTC:
                cli_out("bcmEsmcEssmCodeEPRTC\n");
                break;
            case bcmEsmcEssmCodeEEEC:
                cli_out("bcmEsmcEssmCodeEEEC\n");
                break;
        }

        sal_free(rxbufp);
        sal_free(txbufp);
        PTP_CLI_RESULT_RETURN(CMD_OK);

    } else if (parse_cmp("Set", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MAC", PQ_MAC, (void*)0, arg_mac, NULL);
        parse_table_add(&pt, "QL", PQ_MULTI | PQ_DFL, 0, &arg_ql_index, arg_ql_str);
        parse_table_add(&pt, "HoldoverQL", PQ_MULTI | PQ_DFL, 0, &arg_holdover_ql_index, arg_ql_str);
        parse_table_add(&pt, "G781", PQ_MULTI | PQ_DFL, 0, &arg_g781_index, arg_g781_str);
        parse_table_add(&pt, "RXENable", PQ_BOOL, (void*)(size_t)invalid_value, &arg_rx_enable, NULL);
        parse_table_add(&pt, "TXENable", PQ_BOOL, (void*)(size_t)invalid_value, &arg_tx_enable, NULL);
        parse_table_add(&pt, "RXPortBitMaP", PQ_STRING, (void*)"", &arg_rx_pbmp_str, NULL);
        parse_table_add(&pt, "TXPortBitMaP", PQ_STRING, (void*)"", &arg_tx_pbmp_str, NULL);
        parse_table_add(&pt, "G8264ESSMCode", PQ_INT, (void*)0, &essm_code, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }


        rxrv = parse_bcm_pbmp(unit, arg_rx_pbmp_str, &arg_rx_pbmp);
        txrv = parse_bcm_pbmp(unit, arg_tx_pbmp_str, &arg_tx_pbmp);

        parse_arg_eq_done(&pt);

        if (sal_memcmp(arg_mac, zero_mac, sizeof(bcm_mac_t))) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_mac_set(
                unit, 0, arg_dpll_index, &arg_mac));
        }

        if (invalid_value == arg_g781_index)
        {
            PTP_IF_ERROR_RETURN(bcm_esmc_g781_option_get(
                unit, 0, &arg_g781_option));
        } else {
            arg_g781_option = (bcm_esmc_network_option_t)(arg_g781_index + 1);
            PTP_IF_ERROR_RETURN(bcm_esmc_g781_option_set(
                unit, 0, arg_g781_option));
        }

        if (invalid_value != arg_ql_index) {
            arg_ql = bcm_esmc_ql_unresolvable;
            switch (arg_ql_index) {
            case 0:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_I_ql_prc;
                }
                break;
            case 1:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_I_ql_ssua;
                }
                break;
            case 2:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_I_ql_ssub;
                }
                break;
            case 3:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_I_ql_sec;
                } else if (bcm_esmc_network_option_g781_III == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_III_ql_sec;
                }
                break;
            case 4:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_I_ql_dnu;
                }
                break;
            case 5:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_II_ql_stu;
                }
                break;
            case 6:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_II_ql_prs;
                }
                break;
            case 7:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_II_ql_tnc;
                }
                break;
            case 8:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_II_ql_st2;
                }
                break;
            case 9:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_II_ql_st3;
                }
                break;
            case 10:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_II_ql_smc;
                }
                break;
            case 11:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_II_ql_st3e;
                }
                break;
            case 12:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_II_ql_prov;
                }
                break;
            case 13:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_II_ql_dus;
                }
                break;
            case 14:
                if (bcm_esmc_network_option_g781_III == arg_g781_option) {
                    arg_ql = bcm_esmc_g781_III_ql_unk;
                }
                break;
            default:
                arg_ql = bcm_esmc_ql_unresolvable;
            }

            if (bcm_esmc_ql_unresolvable == arg_ql) {
                cli_out("TDPLL ESMC SET ... : Incompatible QL and ITU-T G.781 networking option.\n");
            } else {
                PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_ql_set(
                    unit, 0, arg_dpll_index, arg_ql));
            }
           
        }

        if (invalid_value != arg_holdover_ql_index) {
            arg_holdover_ql = bcm_esmc_ql_unresolvable;
            switch (arg_holdover_ql_index) {
            case 0:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_I_ql_prc;
                }
                break;
            case 1:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_I_ql_ssua;
                }
                break;
            case 2:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_I_ql_ssub;
                }
                break;
            case 3:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_I_ql_sec;
                } else if (bcm_esmc_network_option_g781_III == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_III_ql_sec;
                }
                break;
            case 4:
                if (bcm_esmc_network_option_g781_I == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_I_ql_dnu;
                }
                break;
            case 5:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_II_ql_stu;
                }
                break;
            case 6:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_II_ql_prs;
                }
                break;
            case 7:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_II_ql_tnc;
                }
                break;
            case 8:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_II_ql_st2;
                }
                break;
            case 9:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_II_ql_st3;
                }
                break;
            case 10:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_II_ql_smc;
                }
                break;
            case 11:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_II_ql_st3e;
                }
                break;
            case 12:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_II_ql_prov;
                }
                break;
            case 13:
                if (bcm_esmc_network_option_g781_II == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_II_ql_dus;
                }
                break;
            case 14:
                if (bcm_esmc_network_option_g781_III == arg_g781_option) {
                    arg_holdover_ql = bcm_esmc_g781_III_ql_unk;
                }
                break;
            default:
                arg_holdover_ql = bcm_esmc_ql_unresolvable;
            }

            if (bcm_esmc_ql_unresolvable == arg_holdover_ql) {
                cli_out("TDPLL ESMC SET ... : Incompatible Holdover QL and ITU-T G.781 networking option.\n");
            } else {
                PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_holdover_ql_set(
                    unit, 0, arg_dpll_index, arg_holdover_ql));
            }
        }

        if (invalid_value != arg_rx_enable) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_rx_enable_set(
                unit, 0, arg_rx_enable));
            if (arg_rx_enable) {
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
                PTP_IF_ERROR_RETURN(bcm_extdpll_esmc_event_cb_register(unit, 0));
#else
                PTP_IF_ERROR_RETURN(bcm_esmc_rx_callback_register(unit, 0,
                    bcm_tdpll_esmc_rx_state_machine));
#endif
            } else {
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
                PTP_IF_ERROR_RETURN(bcm_extdpll_esmc_event_cb_unregister(unit, 0));
#else
                PTP_IF_ERROR_RETURN(bcm_esmc_rx_callback_unregister(unit, 0));
#endif
            }
        }

        if (invalid_value != arg_tx_enable) {
            PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_tx_enable_set(
                unit, 0, arg_dpll_index, arg_tx_enable));
        }

        if (BCM_SUCCESS(rxrv)) {
            /* Valid port bitmap for ESMC Rx. */
            PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_rx_portbitmap_set(unit, 0,
                arg_dpll_index, arg_rx_pbmp));
        }

        if (BCM_SUCCESS(txrv)) {
            /* Valid port bitmap for ESMC Tx. */
            PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_tx_portbitmap_set(unit, 0,
                arg_dpll_index, arg_tx_pbmp));
        }

        switch(essm_code) {
            case bcmEsmcEssmCodeNone:
            case bcmEsmcEssmCodeEEC:
                PTP_IF_ERROR_RETURN(bcm_tdpll_esmc_essm_code_set(unit, 0,
                    arg_dpll_index, essm_code));
                break;
            case bcmEsmcEssmCodePRTC:
            case bcmEsmcEssmCodeEPRTC:
            case bcmEsmcEssmCodeEEEC:
            default:
                cli_out("Unsupported G.8264 essm code - %02x\n", (unsigned)essm_code);
                break;
        }

        PTP_CLI_RESULT_RETURN(CMD_OK);
    } else if (parse_cmp("Register", arg, 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "G8273PhaseTransientEnable", PQ_BOOL, (void*)0, &g8273_phase_transient_enable, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Invalid option %s\n", ARG_CMD(a), ARG_CUR(a));
        }

        parse_arg_eq_done(&pt);

        if (g8273_phase_transient_enable) {
            cli_out("Registering TDPLL phase transient callback..\n");
            PTP_IF_ERROR_RETURN(bcm_tdpll_input_clock_ql_change_callback_register(
                unit, 0, diag_tdpll_input_clock_ql_change_callback));
        } else {
            cli_out("Un-registering TDPLL phase transient callback..\n");
            bcm_tdpll_input_clock_ql_change_callback_unregister(unit, 0);
        }
    } else if (parse_cmp("HELP", arg, 0)) {
        cli_out(TDPLL_ESMC_USAGE_1);
        cli_out(TDPLL_ESMC_USAGE_2);
        cli_out(TDPLL_ESMC_USAGE_3);
    } else {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

#endif /* __KERNEL__ */


#define PTP_HELP_USAGE \
    "PTP HELP <subcommand>\n" \
    "\t \"PTP HELP LIST\" prints list of subcommands\n"

STATIC cmd_result_t
cmd_ptp_help(int unit, args_t *a)
{
    const char * arg;
    subcommand_t *cmd;

    arg = ARG_GET(a);
    if (arg) {
        cmd = subcommands_list;
        while (cmd->str) {
            if (parse_cmp(cmd->str, arg, 0)) {
                if (cmd->help) {
                    cli_out("%s", cmd->help);
                }
                if (cmd->help_ext) {
                    cli_out("%s", cmd->help_ext);
                }
                PTP_CLI_RESULT_RETURN(CMD_OK);
            }
            ++cmd;
        }
    }

    /* No valid argument */
    cli_out("PTP HELP <subcommand> with <subcommand> from:\n");

    cmd = subcommands_list;
    while (cmd->str) {
        const char* first_cmd = (cmd++)->str;
        const char* second_cmd = "";
        if (cmd->str) {
            second_cmd = (cmd++)->str;
        }
        cli_out("  %20s%20s\n", first_cmd, second_cmd);
    }

    PTP_CLI_RESULT_RETURN(CMD_OK);
}

/*************************************************************************
 * Main PTP command
 */

char cmd_ptp_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "  PTP <option> [args...]\n"
#else /* COMPILER_STRING_CONST_LIMIT */
    "  " PTP_STACK_USAGE "\n"
    "  " PTP_CLOCK_USAGE "\n"
    "  " PTP_PORT_USAGE "\n"
    "  " PTP_TIME_USAGE "\n"
    "  " PTP_CHANNELS_USAGE_1  PTP_CHANNELS_USAGE_2 "\n"
    "  " PTP_SIGNAL_USAGE "\n"
    "  " PTP_SYNCPHY_USAGE "\n"
#ifndef __KERNEL__
    "  " PTP_TODOUT_USAGE_1 PTP_TODOUT_USAGE_2 "\n"
    "  " PTP_TODIN_USAGE_1 PTP_TODIN_USAGE_2"\n"
    "  " PTP_SERVO_USAGE_1 PTP_SERVO_USAGE_2 PTP_SERVO_USAGE_3 "\n"
    "  " PTP_MASTER_USAGE_1 PTP_MASTER_USAGE_2 "\n"
    "  " PTP_SLAVE_USAGE "\n"
    "  " PTP_PEER_DATASET_GET_USAGE "\n"
    "  " PTP_PACKET_COUNTERS_GET_USAGE "\n"
    "  " PTP_DEBUG_USAGE "\n"
    "  " PTP_LOG_USAGE "\n"
    "  " PTP_REGISTER_USAGE "\n"
    "  " PTP_TELECOM_USAGE_1 PTP_TELECOM_USAGE_2 "\n"
    "  " PTP_CTDEV_USAGE "\n"
    "  " PTP_MODULAR_USAGE "\n"
    "  " PTP_DIAGSHELL_USAGE "\n"
#endif /* __KERNEL__ */
    "  " PTP_HELP_USAGE "\n"
#endif /* COMPILER_STRING_CONST_LIMIT */
    ;


static subcommand_t subcommands[] = {
    {"Stack", cmd_ptp_stack, PTP_STACK_USAGE, 0},
    {"Clock", cmd_ptp_clock, PTP_CLOCK_USAGE, 0},
    {"Port", cmd_ptp_port, PTP_PORT_USAGE, 0},
    {"Time", cmd_ptp_time, PTP_TIME_USAGE, 0},
    {"CHannels", cmd_ptp_channels, PTP_CHANNELS_USAGE_1, PTP_CHANNELS_USAGE_2},
    {"SIGnals", cmd_ptp_signals, PTP_SIGNAL_USAGE, 0},
    {"SyncPhy", cmd_ptp_syncphy, PTP_SYNCPHY_USAGE, 0},
#ifndef __KERNEL__
    {"ToDOut", cmd_ptp_tod_out, PTP_TODOUT_USAGE_1, PTP_TODOUT_USAGE_2},
    {"ToDIn", cmd_ptp_tod_in, PTP_TODIN_USAGE_1, PTP_TODIN_USAGE_2},
    {"SerVO", cmd_ptp_servo, PTP_SERVO_USAGE_1, PTP_SERVO_USAGE_2},
    {"MaSTer", cmd_ptp_master, PTP_MASTER_USAGE_1, PTP_MASTER_USAGE_2},
    {"SLaVe", cmd_ptp_slave, PTP_SLAVE_USAGE, 0},
    {"PeerDataset", cmd_ptp_peer_dataset_get, PTP_PEER_DATASET_GET_USAGE, 0},
    {"CounTeRs", cmd_ptp_packet_counters_get, PTP_PACKET_COUNTERS_GET_USAGE, 0},
    {"DeBug", cmd_ptp_debug, PTP_DEBUG_USAGE, 0},
    {"Log", cmd_ptp_log, PTP_LOG_USAGE, 0},
    {"REGister", cmd_ptp_register, PTP_REGISTER_USAGE, 0},
    {"TELecom", cmd_ptp_telecom, PTP_TELECOM_USAGE_1, PTP_TELECOM_USAGE_2},
    {"CTDEV", cmd_ptp_ctdev, PTP_CTDEV_USAGE, 0},
    {"MODULAR", cmd_ptp_modular, PTP_MODULAR_USAGE, 0},
    {"DIAGshell", cmd_ptp_diagshell, PTP_DIAGSHELL_USAGE, 0},
    {"TsFifo", cmd_ptp_ts_fifo, PTP_TSFIFO_USAGE, 0},
    {"TsOffset", cmd_ptp_ts_offset, PTP_TSOFFSET_USAGE, 0},
#endif /* __KERNEL__ */
    {"HELP", cmd_ptp_help, PTP_HELP_USAGE, 0},
    {0, 0, 0, 0}
};

static subcommand_t *subcommands_list = subcommands;

cmd_result_t
cmd_ptp(int unit, args_t *a)
{
    char *arg;
    subcommand_t *cmd;

    arg = ARG_GET(a);
    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    cmd = subcommands;
    while (cmd->str) {
        if (parse_cmp(cmd->str, arg, 0)) {
            /* Peek at next argument.  If it is help, we deal with it here */
            if (ARG_CUR(a) && parse_cmp("HELP", ARG_CUR(a), 0)) {
                arg = ARG_GET(a);  /* peel off the "HELP" to avoid warning */

                /* Help for subcommand */
                if (cmd->help) {
                    cli_out("%s", cmd->help);
                }
                if (cmd->help_ext) {
                    cli_out("%s", cmd->help_ext);
                }
                PTP_CLI_RESULT_RETURN(CMD_OK);
            } else {
                /* Execute the subcommand */
                return (*cmd->func)(unit, a);
            }
        }
        ++cmd;
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}

/* T-DPLL command gateway. */
char cmd_tdpll_usage[] = "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  TDPLL <option> [args...]\n"
#else /* COMPILER_STRING_CONST_LIMIT */
#ifndef __KERNEL__
    "  " TDPLL_INSTANCE_USAGE_1 TDPLL_INSTANCE_USAGE_2 "\n"
    "  " TDPLL_INPUTCLK_USAGE_1 TDPLL_INPUTCLK_USAGE_2 "\n"
    "  " TDPLL_OUTPUTCLK_USAGE "\n"
    "  " TDPLL_MONITOR_USAGE "\n"
    "  " TDPLL_SELECT_USAGE "\n"
    "  " TDPLL_SWITCH_USAGE "\n"
    "  " TDPLL_HOLDOVER_USAGE_1 TDPLL_HOLDOVER_USAGE_2 "\n"
    "  " TDPLL_ESMC_USAGE_1 TDPLL_ESMC_USAGE_2 TDPLL_ESMC_USAGE_3"\n"
#endif /* __KERNEL__ */
#endif /* COMPILER_STRING_CONST_LIMIT */
    ;

cmd_result_t
cmd_tdpll(int unit, args_t *a)
{
    subcommand_t subcommands[] = {
#ifndef __KERNEL__
        {"INSTance", cmd_tdpll_instance},
        {"INputclk", cmd_tdpll_inputclk},
        {"OUTputclk", cmd_tdpll_outputclk},
        {"MONitor", cmd_tdpll_monitor},
        {"SELect", cmd_tdpll_select},
        {"NOTIfication", cmd_tdpll_notification},
        {"SWitch", cmd_tdpll_switch},
        {"ESMC", cmd_tdpll_esmc},
        {"HOLDover", cmd_tdpll_holdover},
#endif /* __KERNEL__ */
    };

    char *arg;

    int i;

    arg = ARG_GET(a);
    if (!arg) {
        PTP_CLI_RESULT_RETURN(CMD_USAGE);
    }

    for (i = 0; i < sizeof(subcommands) / sizeof(subcommands[0]); ++i) {
        if (parse_cmp(subcommands[i].str, arg, 0)) {
            return (*subcommands[i].func)(unit, a);
        }
    }

    PTP_CLI_RESULT_RETURN(CMD_USAGE);
}

#ifndef __KERNEL__

/*****************************************************************************
 * Minimal ARP handler, for bare-bones use of Diag Shell with no other L3
 * framework in place.
 */

#define ARP_IPV4_LEN (28)
#define ARP_REQUEST  (1)
#define ARP_RESPONSE (2)
#define OPCODE_OFFSET (6)
#define PROTOCOL_OFFSET (2)
#define SENDER_IPV4_MAC_OFFSET (8)
#define SENDER_IPV4_ADDR_OFFSET (14)
#define TARGET_IPV4_MAC_OFFSET (18)
#define TARGET_IPV4_ADDR_OFFSET (24)
#define PROTOCOL_IPV4 (0x0800)
#define ETHERTYPE_ARP (0x0806)
#define ETHERTYPE_IPV6 (0x86dd)

static int
diag_icmpv6_checksum_set(uint8 *packet);
static uint16
diag_ipv6_checksum(uint8* packet, int packet_len);


STATIC void
diag_arp_callback(int unit, bcm_ptp_stack_id_t stack_id, int protocol,
                  int src_addr_offset, int payload_offset, int msg_len, uint8 *msg)
{
    int len;
    int required_len;
    int rv;

    switch (protocol) {
    case ETHERTYPE_ARP: {
        uint8 *arp = msg + payload_offset;
        uint16 opcode = _bcm_ptp_uint16_read(arp + OPCODE_OFFSET);
        uint16 arp_protocol = _bcm_ptp_uint16_read(arp + PROTOCOL_OFFSET);

        required_len = ARP_IPV4_LEN;
        len = msg_len - payload_offset;

        if (len < required_len) {
            PTP_VERBOSE_CB(("ARP too short: %d vs %d\n", len, required_len));
            return;
        }

        if (opcode == ARP_REQUEST) {
            if (arp_protocol == PROTOCOL_IPV4) {
                uint32 addr = _bcm_ptp_uint32_read(arp + TARGET_IPV4_ADDR_OFFSET);
                int clock_num, port_idx;
                for (clock_num = 0; clock_num < PTP_MAX_STACKS_PER_UNIT; ++clock_num) {
                    _bcm_ptp_clock_cache_t *clock =
                        &_bcm_common_ptp_unit_array[unit].stack_array[stack_id].clock_array[clock_num];

                    if (!clock->in_use) {
                        continue;
                    }

                    for (port_idx = 0; port_idx < clock->clock_info.num_ports; ++port_idx) {
                        if (clock->port_info[port_idx].port_address.addr_type == bcmPTPUDPIPv4) {
                            bcm_ip_t port_ip = _bcm_ptp_uint32_read(clock->port_info[port_idx].port_address.address);
                            if (port_ip == addr) {
                                /* Move incoming src to outgoing dest */
                                sal_memcpy(msg, msg+6, 6);
                                /* put in "our" MAC as src MAC */
                                sal_memcpy(msg+6, clock->port_info[port_idx].mac, 6);

                                _bcm_ptp_uint16_write(arp + OPCODE_OFFSET, ARP_RESPONSE);

                                /* sender's MAC/IP -> target MAC/IP */
                                sal_memcpy(arp + TARGET_IPV4_MAC_OFFSET, arp + SENDER_IPV4_MAC_OFFSET, 10);

                                /* sender MAC */
                                sal_memcpy(arp + SENDER_IPV4_MAC_OFFSET, clock->port_info[port_idx].mac, 6);

                                _bcm_ptp_uint32_write(arp + SENDER_IPV4_ADDR_OFFSET, port_ip);   /* sender IP */

                                PTP_VERBOSE_CB(("Responding to ARP request for IP %08x\n", port_ip));
                                
                                if (BCM_FAILURE(rv = _bcm_ptp_tunnel_message_to_world(unit, stack_id, clock_num, msg_len, msg, 1))) {
                                    PTP_VERBOSE_CB(("Failed responding to ARP request for IP %08x\n", port_ip));
                                }

                                return;
                            }
                        }
                    }
                }
            }
        }
        break;
    }

    case ETHERTYPE_IPV6: {
        if (msg[payload_offset] == 135) {  /* IPv6 ICMP type: Neighbor Solicitation */
            uint8 *tgt_addr = msg + payload_offset + 8;  /* Offset to target address in NS packet */
            int dest_addr_offset = src_addr_offset + 16;
            int icmp_offset = payload_offset;
            int ipv6_offset = src_addr_offset - 8;

            int clock_num, port_idx;
            for (clock_num = 0; clock_num < PTP_MAX_STACKS_PER_UNIT; ++clock_num) {
                _bcm_ptp_clock_cache_t *clock =
                    &_bcm_common_ptp_unit_array[unit].stack_array[stack_id].clock_array[clock_num];

                if (!clock->in_use) {
                    continue;
                }

                for (port_idx = 0; port_idx < clock->clock_info.num_ports; ++port_idx) {
                    if (clock->port_info[port_idx].port_address.addr_type == bcmPTPUDPIPv6) {
                        if (sal_memcmp(tgt_addr, clock->port_info[port_idx].port_address.address, 16) == 0) {
                            uint8 na[256];

                            int na_len = icmp_offset + 32; /* 32 bytes for NA payload (incl 8 of option) */
                            uint8 unspecified[16] = {0};  /* the IPv6 unspecified address */
                            uint8 all_nodes[16] = {0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
                            int flags = 0;

                            PTP_VERBOSE_CB(("Got NS for our IP\n"));

                            if (msg_len > sizeof(na)) {
                                PTP_VERBOSE_CB(("NS Too Big, dropping\n"));
                                break;
                            }
                            /* start with a copy of the NS packet */
                            sal_memcpy(na, msg, msg_len);

                            /* L2 addresses */
                            sal_memcpy(na, msg+6, 6);  /* copy src MAC on NS to dst MAC on NA */
                            sal_memcpy(na+6, clock->port_info[port_idx].mac, 6); /* copy port's MAC to src MAC on NA */

                            /* L3 addresses */

                            /* copy port's addr to src addr */
                            sal_memcpy(na + src_addr_offset, clock->port_info[port_idx].port_address.address, 16);

                            if (sal_memcmp(msg + src_addr_offset, unspecified, 16) == 0) {
                                /* sender used the "unspecified" address, so send response to "all_nodes" */
                                sal_memcpy(na + dest_addr_offset, all_nodes, 16);
                                flags = 0x40;  /* set only the R (response) bit in flags */
                            } else {
                                /* copy target's addr to dest addr */
                                sal_memcpy(na + dest_addr_offset, msg + src_addr_offset, 16);
                                flags = 0x60;  /* set R (response) and O (override) bits in flags */
                            }
                            na[icmp_offset] = 136;  /* NA */
                            na[icmp_offset + 4] = flags;

                            /* Option: include our link-layer address */
                            na[icmp_offset + 24] = 2;  /* type 2: target link layer address */
                            na[icmp_offset + 25] = 1;  /* length 1 (i.e. 1 set of 8 octets  */
                            sal_memcpy(na + icmp_offset + 26, clock->port_info[port_idx].mac, 6);

                            /* set length */
                            na[ipv6_offset + 4] = 0;
                            na[ipv6_offset + 5] = 32; /* 24 bytes + 8 bytes option */

                            diag_icmpv6_checksum_set(na + ipv6_offset);

                            if (BCM_FAILURE(rv = _bcm_ptp_tunnel_message_to_world(unit, stack_id, clock_num, na_len, na, 1))) {
                                PTP_VERBOSE_CB(("Failed responding to NS\n"));
                            }
                            break;
                        }
                    }
                }
            }
        }

        return;
      }
    }
}

#define BCMPTP_ND_NEIGHBOR_ADVERTISEMENT_ICMPV6_SIZE_OCTETS        (32)
#define BCMPTP_ND_NEIGHBOR_ADVERTISEMENT_IPV6_SIZE_OCTETS          (72)

#define BCMPTP_ND_IPV6_PSEUDO_HEADER_SIZE_OCTETS                   (40)
#define BCMPTP_ND_CHECKSUM_PACKET_SIZE_OCTETS (BCMPTP_ND_IPV6_PSEUDO_HEADER_SIZE_OCTETS + \
                                               BCMPTP_ND_NEIGHBOR_ADVERTISEMENT_ICMPV6_SIZE_OCTETS)

#define BCMPTP_ND_IPV6_NEXT_HEADER_OFFSET_OCTETS                   (6)
#define BCMPTP_ND_IPV6_SOURCE_IP_ADDRESS_OFFSET_OCTETS             (8)
#define BCMPTP_ND_IPV6_DESTINATION_IP_ADDRESS_OFFSET_OCTETS        (24)

#define BCMPTP_ND_ICMPV6_OFFSET_OCTETS                             (40)
#define BCMPTP_ND_ICMPV6_CHECKSUM_OFFSET_OCTETS                    (42)
#define BCMPTP_ND_ICMPV6_TARGET_OFFSET_OCTETS                      (48)
#define BCMPTP_ND_ICMPV6_OPTION_DATA_OFFSET                        (66)

#define BCMPTP_ND_NEXT_HEADER_ICMPV6                               (0x3a)
#define BCMPTP_ND_ICMPV6_TYPE_NEIGHBOR_SOLICITATION                (0x87)
#define BCMPTP_ND_ICMPV6_TYPE_NEIGHBOR_ADVERTISEMENT               (0x88)



#define BCMPTP_IPV6_ADDR_SIZE_BYTES (16)

/*
 * Function:
 *      _bcmptp_icmpv6_checksum_set()
 * Purpose:
 *      Set checksum for an ICMPv6 Neighbor Advertisement message.
 * Parameters:
 *      packet - (IN/OUT) ICMPv6 Neighbor Advertisement message.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
static int
diag_icmpv6_checksum_set(uint8 *packet)
{
    int i;
    uint16 checksum;
    uint8 pseudo_plus_icmpv6[BCMPTP_ND_CHECKSUM_PACKET_SIZE_OCTETS] = {0};

    /* Zero existing checksum. */
    sal_memset(packet + BCMPTP_ND_ICMPV6_CHECKSUM_OFFSET_OCTETS, 0,
               sizeof(uint16));

    /*
     * Make packet for checksum.
     * | IPv6 pseudo-header | ICMPv6 |.
     */

    /*
     * Insert IPv6 pseudo-header.
     * Ref. RFC 2460 - Internet Protocol, Version 6 (IPv6) Specification,
     *      Sect. 8.1, Upper-Layer Checksums.
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *  |                                                               |
     *  +                                                               +
     *  |                                                               |
     *  +                         Source Address                        +
     *  |                                                               |
     *  +                                                               +
     *  |                                                               |
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *  |                                                               |
     *  +                                                               +
     *  |                                                               |
     *  +                      Destination Address                      +
     *  |                                                               |
     *  +                                                               +
     *  |                                                               |
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *  |                   Upper-Layer Packet Length                   |
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *  |                      zero                     |  Next Header  |
     *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     */
     sal_memcpy(pseudo_plus_icmpv6,
               packet + BCMPTP_ND_IPV6_SOURCE_IP_ADDRESS_OFFSET_OCTETS,
               BCMPTP_IPV6_ADDR_SIZE_BYTES);

    i = BCMPTP_IPV6_ADDR_SIZE_BYTES;
    sal_memcpy(pseudo_plus_icmpv6 + i,
               packet + BCMPTP_ND_IPV6_DESTINATION_IP_ADDRESS_OFFSET_OCTETS,
               BCMPTP_IPV6_ADDR_SIZE_BYTES);

    i += BCMPTP_IPV6_ADDR_SIZE_BYTES;
    _bcm_ptp_uint32_write(pseudo_plus_icmpv6 + i,
                         BCMPTP_ND_NEIGHBOR_ADVERTISEMENT_ICMPV6_SIZE_OCTETS);

    i += sizeof(uint32);
    _bcm_ptp_uint32_write(pseudo_plus_icmpv6 + i, BCMPTP_ND_NEXT_HEADER_ICMPV6);

    /* Insert ICMPv6. */
    i += sizeof(uint32);
    sal_memcpy(pseudo_plus_icmpv6 + i,
               packet + BCMPTP_ND_ICMPV6_OFFSET_OCTETS,
               BCMPTP_ND_NEIGHBOR_ADVERTISEMENT_ICMPV6_SIZE_OCTETS);

    /* Calculate checksum. */
    checksum = diag_ipv6_checksum(pseudo_plus_icmpv6,
                                  BCMPTP_ND_CHECKSUM_PACKET_SIZE_OCTETS);

    /* Set checksum. */
    _bcm_ptp_uint16_write(packet + BCMPTP_ND_ICMPV6_CHECKSUM_OFFSET_OCTETS, checksum);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmptp_ipv6_checksum()
 * Purpose:
 *      Calculate IPv6 checksum.
 * Parameters:
 *      packet     - (IN/OUT) Packet.
 *      packet_len - (IN) Packet length (octets).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 *      Ref. RFC 2460 - Internet Protocol, Version 6 (IPv6) Specification,
 *      Sect. 8.1, Upper-Layer Checksums.
 */
static uint16
diag_ipv6_checksum(uint8* packet, int packet_len)
{
    uint32 checksum = 0;

    while (packet_len > 1)
    {
        /* Assemble words from consecutive octets and add to sum. */
        checksum += (uint16)packet[0] << 8 | packet[1];

        /*
         * Increment pointer in packet.
         * Decrement remaining size.
         */
        packet += sizeof(uint16);
        packet_len -= sizeof(uint16);
    }

    /* Process odd octet. */
    if (packet_len == 1) {
        checksum += (uint16)packet[0] << 8;
    }

    /* Carry(ies). */
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0x0000ffff);
    }

    /*
     * Result.
     * NOTE : Do not return zero checksum
     *        (zero value set to 0xffff).
     */
    if (checksum != 0xffff) {
        return ~checksum;
    } else {
        return checksum;
    }
}

static const char *diag_ptp_state_change_reason_str[] = {
    "Startup, instance creation",
    "Port initialization",
    "Fault detected",
    "BMCA state transition",
    "Enable port management message",
    "Disable port management message",
    "Network interface re-initialization",
    "Timestamp difference, master-to-slave",
    "Unknown"
};

/*
 * Function:
 *      diag_event_callback
 * Purpose:
 *      Default event callback handler.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      clock_port - (IN) PTP port number.
 *      type       - (IN) Callback function type.
 *      msg        - (IN) Callback message data.
 *      user_data  - (IN) Callback user data.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
diag_event_callback(
    int unit,
    bcm_ptp_stack_id_t stack_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_cb_type_t type,
    bcm_ptp_cb_msg_t *msg,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    uint16 event_type;
    bcm_ptp_protocol_t ucm_protocol;
    int eventID;
    sal_time_t host_time = sal_time();
    char time_str[32];
    int print_time = 0;
#if defined(PTP_SAL_HAS_TIME)
    time_t host_time_as_time_t;
    struct tm *timestruct;
#endif

    if (diagshell_sync.synced) {
#if defined(LONGS_ARE_64BITS)
        COMPILER_64_SUB_64(host_time, diagshell_sync.host_time);
        COMPILER_64_ADD_64(host_time, diagshell_sync.ptp_time.seconds);
#else
        host_time -= diagshell_sync.host_time;
        host_time += COMPILER_64_LO(diagshell_sync.ptp_time.seconds);
#endif
    }

#if defined(PTP_SAL_HAS_TIME)
    if (verboseEvents) {
        host_time_as_time_t = (time_t)host_time;
        timestruct = sal_localtime(&host_time_as_time_t);
        if (verboseEvents == 2) {
            print_time = sal_strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S ", timestruct);
        } else if (verboseEvents == 1) {
            print_time = sal_strftime(time_str, sizeof(time_str), "%H:%M:%S ", timestruct);
        }
    }
#endif

    if (!print_time) {
        sal_sprintf(time_str, "%u", (unsigned)host_time);
    }

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id, clock_num,
            clock_port))) {
        PTP_ERROR_FUNC_CB("_bcm_ptp_function_precheck()");
        return rv;
    }

    /* Extract event type. */
    event_type = _bcm_ptp_uint16_read(msg->data);

    /* Move cursor to beginning of event data. */
    msg->data = msg->data + sizeof(uint16);

    switch ((_bcm_ptp_event_t)event_type) {
    case _bcm_ptp_state_change_event:
        if (verboseEvents == 2) {
            int code = msg->data[15];
            if (code < _bcm_ptp_state_change_reason_startup ||
                code > _bcm_ptp_state_change_reason_last) {
                code = _bcm_ptp_state_change_reason_last;
            }

            ptp_cb_printf("Event %s: STATE CHANGE\n", time_str);
            /*
             * Event message data.
             *    Octet 0      : Clock instance.
             *    Octet 1...2  : Port number.
             *    Octet 3...12 : Port identity.
             *    Octet 13     : Port state.
             *    Octet 14     : Prior port state.
             *    Octet 15     : Port state change reason.
             */
            ptp_cb_printf("   Instance     : %d\n", msg->data[0]);
            ptp_cb_printf("   Port Number  : %d\n", _bcm_ptp_uint16_read(msg->data+1));
            ptp_cb_printf("   Port Identity: "
                       "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
                       msg->data[3], msg->data[4], msg->data[5], msg->data[6],
                       msg->data[7], msg->data[8], msg->data[9], msg->data[10],
                       _bcm_ptp_uint16_read(msg->data + 3 +
                                            sizeof(bcm_ptp_clock_identity_t)));
            ptp_cb_printf("----------------------------------------"
                       "----------------------------------------\n");
            ptp_cb_printf("   Old Port State: %d (%s)\n", msg->data[14],
                       diag_port_state_description(msg->data[14]));
            ptp_cb_printf("   New Port State: %d (%s)\n", msg->data[13],
                       diag_port_state_description(msg->data[13]));

            ptp_cb_printf("   Reason        : %u (%s).\n",
                          msg->data[15], diag_ptp_state_change_reason_str[code]);
        } else if (verboseEvents == 1) {
            int code = msg->data[15];
            if (code < _bcm_ptp_state_change_reason_startup ||
                code > _bcm_ptp_state_change_reason_last) {
                code = _bcm_ptp_state_change_reason_last;
            }

            ptp_cb_printf("Event %s: STATE CHANGE: Port %d: %s -> %s [%s]\n",
                          time_str,
                          _bcm_ptp_uint16_read(msg->data+1),
                          diag_port_state_description(msg->data[14]),
                          diag_port_state_description(msg->data[13]),
                          diag_ptp_state_change_reason_str[code]);
        }
        break;

    case _bcm_ptp_master_change_event:
        if (verboseEvents == 2) {
            ptp_cb_printf("Event %s: MASTER CHANGE\n", time_str);
            /*
             * Event message data.
             *    Octet 0       : Clock instance.
             *    Octet 1...2   : Port number.
             *    Octet 3...12  : Port identity.
             *    Octet 13...22 : New master port identity.
             *    Octet 23      : New master is-unicast Boolean.
             *    Octet 24      : New master network protocol (unicast master).
             *    Octet 25...26 : New master port address length (unicast master).
             *    Octet 27...42 : New master port address (unicast master).
             *    Octet 43...52 : Old master port identity.
             *    Octet 53      : Old master is-unicast Boolean.
             *    Octet 54      : Old master network protocol (unicast master).
             *    Octet 55...56 : Old master port address length (unicast master).
             *    Octet 57...72 : Old master port address (unicast master).
             *    Octet 73...74 : new master's module number of card on which foreign master is seen.
             *    Octet 75...76 : new master's physical port number of card on which foreign master is seen.
             *    Octet 77...78 : old master's module number of card on which foreign master is seen.
             *    Octet 79...80 : old master's physical port number of card on which foreign master is seen.
             */
            ptp_cb_printf("   Instance     : %d\n", msg->data[0]);
            ptp_cb_printf("   Port Number  : %d\n", _bcm_ptp_uint16_read(msg->data+1));
            ptp_cb_printf("   Port Identity: "
                       "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
                       msg->data[3], msg->data[4], msg->data[5], msg->data[6],
                       msg->data[7], msg->data[8], msg->data[9], msg->data[10],
                       _bcm_ptp_uint16_read(msg->data + 3 +
                                            sizeof(bcm_ptp_clock_identity_t)));
            ptp_cb_printf("----------------------------------------"
                       "----------------------------------------\n");
            ptp_cb_printf("   New Master Properties\n");
            ptp_cb_printf("   Port Identity     : "
                       "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
                       msg->data[13], msg->data[14], msg->data[15], msg->data[16],
                       msg->data[17], msg->data[18], msg->data[19], msg->data[20],
                       _bcm_ptp_uint16_read(msg->data + 13 +
                                            sizeof(bcm_ptp_clock_identity_t)));
            ptp_cb_printf("   Unicast or Multicast Master    : %s\n",
                (msg->data[23]) ? "Unicast" : "Multicast");

            ucm_protocol = msg->data[24];

            ptp_cb_printf("   Master Protocol: %u", ucm_protocol);
            if (ucm_protocol == bcmPTPIEEE8023) {
                ptp_cb_printf(" (Ethernet Layer 2)\n");
                ptp_cb_printf("   Multicast Master Address : "
                           "%02x:%02x:%02x:%02x:%02x:%02x (MAC)\n",
                           msg->data[27], msg->data[28], msg->data[29], msg->data[30],
                           msg->data[31], msg->data[32]);
            } else if (ucm_protocol == bcmPTPUDPIPv4) {
                ptp_cb_printf(" (Ethernet/UDP/IPv4)\n");
                ptp_cb_printf("   UC Master Address : %u.%u.%u.%u (IPv4)\n",
                           msg->data[27], msg->data[28], msg->data[29], msg->data[30]);
            } else if (ucm_protocol == bcmPTPUDPIPv6){
                ptp_cb_printf(" (Ethernet/UDP/IPv6)\n");
                ptp_cb_printf("   UC Master Address : "
                           "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                           "%02x%02x:%02x%02x:%02x%02x:%02x%02x (IPv6)\n",
                           msg->data[27], msg->data[28], msg->data[29], msg->data[30],
                           msg->data[31], msg->data[32], msg->data[33], msg->data[34],
                           msg->data[35], msg->data[36], msg->data[37], msg->data[38],
                           msg->data[39], msg->data[40], msg->data[41], msg->data[42]);
            } else {
                ptp_cb_printf(" (Unknown)\n");
            }

            ptp_cb_printf("\n");
            ptp_cb_printf("   Old Master Properties\n");
            ptp_cb_printf("   Port Identity     : "
                       "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
                       msg->data[43], msg->data[44], msg->data[45], msg->data[46],
                       msg->data[47], msg->data[48], msg->data[49], msg->data[50],
                       _bcm_ptp_uint16_read(msg->data + 43 +
                                            sizeof(bcm_ptp_clock_identity_t)));
            ptp_cb_printf("   Unicast or Mutlicast  Master    : %s\n",
                (msg->data[53]) ? "Unicast" : "Multicast");

            ucm_protocol = msg->data[54];

            ptp_cb_printf("   Master Protocol: %u", ucm_protocol);
            if (ucm_protocol == bcmPTPIEEE8023) {
                ptp_cb_printf(" (Ethernet Layer 2)\n");
                ptp_cb_printf("   Multicast Master Address : "
                           "%02x:%02x:%02x:%02x:%02x:%02x (MAC)\n",
                           msg->data[57], msg->data[58], msg->data[59], msg->data[60],
                           msg->data[61], msg->data[62]);
            } else if (ucm_protocol == bcmPTPUDPIPv4) {
                ptp_cb_printf(" (Ethernet/UDP/IPv4)\n");
                ptp_cb_printf("   UC Master Address : %u.%u.%u.%u (IPv4)\n",
                           msg->data[57], msg->data[58], msg->data[59], msg->data[60]);
            } else if (ucm_protocol == bcmPTPUDPIPv6) {
                ptp_cb_printf(" (Ethernet/UDP/IPv6)\n");
                ptp_cb_printf("   UC Master Address : "
                           "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                           "%02x%02x:%02x%02x:%02x%02x:%02x%02x (IPv6)\n",
                           msg->data[57], msg->data[58], msg->data[59], msg->data[60],
                           msg->data[61], msg->data[62], msg->data[63], msg->data[64],
                           msg->data[65], msg->data[66], msg->data[67], msg->data[68],
                           msg->data[69], msg->data[70], msg->data[71], msg->data[72]);
            } else {
                ptp_cb_printf(" (Unknown)\n");
            }

            {
                int port = _bcm_ptp_uint16_read(msg->data + 75);
                if (SOC_IS_QUX(unit) || SOC_IS_QAX(unit) || SOC_IS_QMX(unit)) {
                    int trunk_id=0, mod_id=0, port_id=0;
                    ptp_parse_sysport_to_modport(unit, port, &trunk_id, &mod_id, &port_id);
                    ptp_cb_printf("   New master's LagId Number: %d\n", trunk_id);
                    ptp_cb_printf("   New master's module Number: %d\n", mod_id);
                    ptp_cb_printf("   New master's Physical Port Number: %d\n", port_id);
                } else {
                    ptp_cb_printf("   New master's module Number: %d\n", _bcm_ptp_uint16_read(msg->data + 73));
                    ptp_cb_printf("   New master's Physical Port Number: %d\n", _bcm_ptp_uint16_read(msg->data + 75));
                }

                port = _bcm_ptp_uint16_read(msg->data + 79);
                if (SOC_IS_QUX(unit) || SOC_IS_QAX(unit) || SOC_IS_QMX(unit)) {
                    int trunk_id=0, mod_id=0, port_id=0;
                    ptp_parse_sysport_to_modport(unit, port, &trunk_id, &mod_id, &port_id);
                    ptp_cb_printf("   Old master's LagId Number: %d\n", trunk_id);
                    ptp_cb_printf("   Old master's module Number: %d\n", mod_id);
                    ptp_cb_printf("   Old master's Physical Port Number: %d\n", port_id);
                } else {
                    ptp_cb_printf("   Old master's module Number: %d\n", _bcm_ptp_uint16_read(msg->data + 77));
                    ptp_cb_printf("   Old master's Physical Port Number: %d\n", _bcm_ptp_uint16_read(msg->data + 79));
                }
            }

        } else if (verboseEvents == 1) {
            char port_addr[32];   /* needs space for 8 * 3 + 4 + null */
            char addr[50] = "??";   /* IPv6 -> 16 * 3 + null */

            sal_sprintf(port_addr,
                        "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x",
                        msg->data[13], msg->data[14], msg->data[15], msg->data[16],
                        msg->data[17], msg->data[18], msg->data[19], msg->data[20],
                        _bcm_ptp_uint16_read(msg->data + 13 +
                                             sizeof(bcm_ptp_clock_identity_t)));
            ucm_protocol = msg->data[24];
            if (ucm_protocol == bcmPTPIEEE8023) {
                sal_sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x",
                            msg->data[27], msg->data[28], msg->data[29], msg->data[30],
                            msg->data[31], msg->data[32]);
            } else if (ucm_protocol == bcmPTPUDPIPv4) {
                sal_sprintf(addr, "%u.%u.%u.%u",
                            msg->data[27], msg->data[28], msg->data[29], msg->data[30]);
            } else if (ucm_protocol == bcmPTPUDPIPv6){
                sal_sprintf(addr, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                            "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                            msg->data[27], msg->data[28], msg->data[29], msg->data[30],
                            msg->data[31], msg->data[32], msg->data[33], msg->data[34],
                            msg->data[35], msg->data[36], msg->data[37], msg->data[38],
                            msg->data[39], msg->data[40], msg->data[41], msg->data[42]);
            }
            ptp_cb_printf("Event %s: MASTER CHANGE %s [%s]\n", time_str, addr, port_addr);
        }
        break;

    case _bcm_ptp_master_avail_event:
    case _bcm_ptp_master_unavail_event:
        if (verboseEvents == 2) {
            ptp_cb_printf("Event %s: MASTER %sAVAILABLE\n", time_str,
                          (event_type == _bcm_ptp_master_unavail_event) ? "UN" : "");
            /*
             * Event message data.
             *    Octet 0       : Clock instance.
             *    Octet 1...2   : Port number.
             *    Octet 3...12  : Port identity.
             *    Octet 13...22 : Foreign master port identity.
             *    Octet 23      : Foreign master is-acceptable Boolean.
             *    Octet 24      : Foreign master is-unicast Boolean.
             *    Octet 25      : Foreign master network protocol (unicast foreign master).
             *    Octet 26...27 : Foreign master port address length (unicast foreign master).
             *    Octet 28...43 : Foreign master port address (unicast foreign master).
             *    Octet 44...45: module number of card on which foreign master is seen.
             *    Octet 46...47: physical port number of card on which Foreign master is seen.
             */
            ptp_cb_printf("   Instance     : %d\n", msg->data[0]);
            ptp_cb_printf("   Port Number  : %d\n", _bcm_ptp_uint16_read(msg->data+1));
            ptp_cb_printf("   Port Identity: "
                       "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
                       msg->data[3], msg->data[4], msg->data[5], msg->data[6],
                       msg->data[7], msg->data[8], msg->data[9], msg->data[10],
                       _bcm_ptp_uint16_read(msg->data + 3 +
                                            sizeof(bcm_ptp_clock_identity_t)));
            ptp_cb_printf("----------------------------------------"
                       "----------------------------------------\n");
            ptp_cb_printf("   Foreign Master Properties\n");
            ptp_cb_printf("   Port Identity     : "
                       "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
                       msg->data[13], msg->data[14], msg->data[15], msg->data[16],
                       msg->data[17], msg->data[18], msg->data[19], msg->data[20],
                       _bcm_ptp_uint16_read(msg->data + 13 +
                                            sizeof(bcm_ptp_clock_identity_t)));
            ptp_cb_printf("   Acceptable Master : %u\n", msg->data[23]);
            ptp_cb_printf("   Unicast or Multicast Master    : %s\n",
                (msg->data[24]) ?  "Unicast" : "Multicast");

            ucm_protocol = msg->data[25];

            ptp_cb_printf("   Master Protocol: %u", ucm_protocol);
            if (ucm_protocol == bcmPTPIEEE8023) {
                ptp_cb_printf(" (Ethernet Layer 2)\n");
                ptp_cb_printf("   Multicast Master Address : "
                    "%02x:%02x:%02x:%02x:%02x:%02x (MAC)\n",
                    msg->data[28], msg->data[29], msg->data[30], msg->data[31],
                    msg->data[32], msg->data[33]);
            } else if (ucm_protocol == bcmPTPUDPIPv4) {
                ptp_cb_printf(" (Ethernet/UDP/IPv4)\n");
                ptp_cb_printf("   UC Master Address : %u.%u.%u.%u (IPv4)\n",
                           msg->data[28], msg->data[29], msg->data[30], msg->data[31]);
            } else if (ucm_protocol== bcmPTPUDPIPv6) {
                ptp_cb_printf(" (Ethernet/UDP/IPv6)\n");
                ptp_cb_printf("   UC Master Address : "
                           "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                           "%02x%02x:%02x%02x:%02x%02x:%02x%02x (IPv6)\n",
                           msg->data[28], msg->data[29], msg->data[30], msg->data[31],
                           msg->data[32], msg->data[33], msg->data[34], msg->data[35],
                           msg->data[36], msg->data[37], msg->data[38], msg->data[39],
                           msg->data[40], msg->data[41], msg->data[42], msg->data[43]);
            } else {
                ptp_cb_printf(" (Unknown)\n");
            }

            {
                int port = _bcm_ptp_uint16_read(msg->data + 46);
                if (SOC_IS_QUX(unit) || SOC_IS_QAX(unit) || SOC_IS_QMX(unit)) {
                    int trunk_id=0, mod_id=0, port_id=0;
                    ptp_parse_sysport_to_modport(unit, port, &trunk_id, &mod_id, &port_id);
                    ptp_cb_printf("   LagId Number: %d\n", trunk_id);
                    ptp_cb_printf("   Module Number: %d\n", mod_id);
                    ptp_cb_printf("   Physical Port Number: %d\n", port_id);
                } else {
                    ptp_cb_printf("   Module Number: %d\n", _bcm_ptp_uint16_read(msg->data + 44));
                    ptp_cb_printf("   Physical Port Number: %d\n", _bcm_ptp_uint16_read(msg->data + 46));
                }
            }

        } else if (verboseEvents == 1) {
            char port_addr[32];   /* needs space for 8 * 3 + 4 + null */
            char addr[50] = "??";   /* IPv6 -> 16 * 3 + null */

            sal_sprintf(port_addr,
                        "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x",
                        msg->data[13], msg->data[14], msg->data[15], msg->data[16],
                        msg->data[17], msg->data[18], msg->data[19], msg->data[20],
                        _bcm_ptp_uint16_read(msg->data + 13 +
                                             sizeof(bcm_ptp_clock_identity_t)));

            ucm_protocol = msg->data[25];
            if (ucm_protocol == bcmPTPIEEE8023) {
                sal_sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x",
                            msg->data[28], msg->data[29], msg->data[30], msg->data[31],
                            msg->data[32], msg->data[33]);
            } else if (ucm_protocol == bcmPTPUDPIPv4) {
                sal_sprintf(addr, "%u.%u.%u.%u",
                            msg->data[28], msg->data[29], msg->data[30], msg->data[31]);
            } else if (ucm_protocol == bcmPTPUDPIPv6){
                sal_sprintf(addr, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                            "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                            msg->data[28], msg->data[29], msg->data[30], msg->data[31],
                            msg->data[32], msg->data[33], msg->data[34], msg->data[35],
                            msg->data[36], msg->data[37], msg->data[38], msg->data[39],
                            msg->data[40], msg->data[41], msg->data[42], msg->data[43]);
            }

            ptp_cb_printf("Event %s: MASTER CHANGE %s [%s]\n", time_str, addr, port_addr);
        }
        break;

    case _bcm_ptp_slave_avail_event:
    case _bcm_ptp_slave_unavail_event:
        if (verboseEvents == 2) {
            ptp_cb_printf("Event %s: SLAVE %sAVAILABLE\n", time_str,
                          (event_type == _bcm_ptp_slave_unavail_event) ? "UN" : "");
            /*
             * Event message data.
             *    Octet 0       : Clock instance.
             *    Octet 1...2   : Port number.
             *    Octet 3...12  : Clock ID.
             *    Octet 25     : Foreign slave network protocol (unicast foreign master).
             *    Octet 26...27: Foreign slave port address length (unicast foreign master).
             *    Octet 28...43: Foreign slave port address (unicast foreign master).
             */

            ptp_cb_printf("   Instance     : %d\n", msg->data[0]);
            ptp_cb_printf("   Port Number  : %d\n", _bcm_ptp_uint16_read(msg->data+1));
            ptp_cb_printf("   Clock Identity: "
                       "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n",
                       msg->data[3], msg->data[4], msg->data[5], msg->data[6],
                       msg->data[7], msg->data[8], msg->data[9], msg->data[10]);
            eventID = msg->data[24];
            ptp_cb_printf("   Event ID: %u", eventID);
            ucm_protocol = msg->data[25];
            ptp_cb_printf("   Protocol: %u", ucm_protocol);
            if (ucm_protocol == bcmPTPIEEE8023) {
                  ptp_cb_printf(" (Ethernet Layer 2)\n");
                  ptp_cb_printf("   Slave Address: "
                       "%02x:%02x:%02x:%02x:%02x:%02x\n",
                       msg->data[28], msg->data[29], msg->data[30], msg->data[31],
                       msg->data[32], msg->data[33]);
            } else if (ucm_protocol == bcmPTPUDPIPv4) {
                ptp_cb_printf(" (Ethernet/UDP/IPv4)\n");
                ptp_cb_printf("   Slave Address : %u.%u.%u.%u (IPv4)\n",
                msg->data[28], msg->data[29], msg->data[30], msg->data[31]);
            } else if (ucm_protocol== bcmPTPUDPIPv6) {
                ptp_cb_printf(" (Ethernet/UDP/IPv6)\n");
                ptp_cb_printf("   Slave Address : "
                               "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                               "%02x%02x:%02x%02x:%02x%02x:%02x%02x (IPv6)\n",
                               msg->data[28], msg->data[29], msg->data[30], msg->data[31],
                               msg->data[32], msg->data[33], msg->data[34], msg->data[35],
                               msg->data[36], msg->data[37], msg->data[38], msg->data[39],
                               msg->data[40], msg->data[41], msg->data[42], msg->data[43]);
             } else {
                ptp_cb_printf(" (Unknown)\n");
             }

             {
                int port = _bcm_ptp_uint16_read(msg->data + 46);
                if (SOC_IS_QUX(unit) || SOC_IS_QAX(unit) || SOC_IS_QMX(unit)) {
                    int trunk_id=0, mod_id=0, port_id=0;
                    ptp_parse_sysport_to_modport(unit, port, &trunk_id, &mod_id, &port_id);
                    ptp_cb_printf("   LagId Number: %d\n", trunk_id);
                    ptp_cb_printf("   Module Number: %d\n", mod_id);
                    ptp_cb_printf("   Physical Port Number: %d\n", port_id);
                } else {
                    ptp_cb_printf("   Module Number: %d\n", _bcm_ptp_uint16_read(msg->data + 44));
                    ptp_cb_printf("   Physical Port Number: %d\n", _bcm_ptp_uint16_read(msg->data + 46)); 
                }
             }

        }

        break;

    case _bcm_ptp_top_oom_event:
        if (verboseEvents > 0) {
            

            ptp_cb_printf("Event %s: ToP OUT-OF-MEMORY\n", time_str);
            /*
             * Event message data.
             *    Octet 0...3 : Minimum free memory (bytes).
             *    Octet 4...7 : Free ordinary blocks (bytes).
             */
            ptp_cb_printf("   Min. Free Memory: %u (bytes)\n",
                       _bcm_ptp_uint32_read(msg->data));
            ptp_cb_printf("   Ord. Blocks Free: %u (bytes)\n",
                       _bcm_ptp_uint32_read(msg->data + sizeof(uint32)));
        }
        break;

    case _bcm_ptp_top_watchdog_event:
        break;

    case _bcm_ptp_top_ready_event:
        if (verboseEvents > 0) {
            ptp_cb_printf("Event %s: ToP READY\n", time_str);
        }
        break;

    case _bcm_ptp_top_misc_event:
        if (verboseEvents > 0) {
            ptp_cb_printf("Event %s: ToP MISC\n", time_str);
            /*
             * Event message data.
             *    Octet 0       : Reason code
             */
            switch (msg->data[0])
            {
                case _bcm_ptp_top_misc_reason_anncFlagChange:
                    ptp_cb_printf("   Reason Code: %d\n", msg->data[0]);
                    ptp_cb_printf("   PrevFlags  : 0x%x\n", _bcm_ptp_uint32_read(msg->data+1));
                    ptp_cb_printf("   NewFlags   : 0x%x\n", _bcm_ptp_uint32_read(msg->data+5));
                default:
                    break;
            }
        }
        break;

    case _bcm_ptp_top_tod_avail_event:
        if (verboseEvents > 0) {
            ptp_cb_printf("Event %s: ToP ToD Available\n", time_str);
        }
        break;

    case _bcm_ptp_top_tod_unavail_event:
        if (verboseEvents > 0) {
            ptp_cb_printf("Event %s: ToP ToD Unavailable\n", time_str);
        }
        break;

    case _bcm_ptp_ieee1588_warn_event:
        if (verboseEvents == 2) {
            ptp_cb_printf("Event %s: IEEE Std. 1588-2008 WARNING\n", time_str);
            /*
             * Event message data.
             *    Octet 0      : Clock instance.
             *    Octet 1...2  : Port number.
             *    Octet 3...12 : Port identity.
             *    Octet 13     : IEEE Std. 1588-2008 warning reason code.
             *    Octet 14...N : Reason-dependent message data.
             */
            ptp_cb_printf("   Instance     : %d\n", msg->data[0]);
            ptp_cb_printf("   Port Number  : %d\n", _bcm_ptp_uint16_read(msg->data+1));
            ptp_cb_printf("   Port Identity: "
                       "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
                       msg->data[3], msg->data[4], msg->data[5], msg->data[6],
                       msg->data[7], msg->data[8], msg->data[9], msg->data[10],
                       _bcm_ptp_uint16_read(msg->data + 3 +
                                            sizeof(bcm_ptp_clock_identity_t)));
            ptp_cb_printf("----------------------------------------"
                       "----------------------------------------\n");
            ptp_cb_printf("   Warning Reason: %u (%s)\n", msg->data[13],
                       diag_ieee1588_warn_reason_description(msg->data[13]));
            ptp_cb_printf("\n");

            switch (msg->data[13]) {
            case _bcm_ptp_ieee1588_warn_reason_logAnnounceInterval:
                /*
                 * Non-uniform logAnnounceInterval in a PTP domain.
                 *    Octet 0...13  : IEEE Std. 1588-2008 warning common data.
                 *    Octet 14...23 : Foreign master port identity.
                 *    Octet 24      : Foreign master is-acceptable Boolean.
                 *    Octet 25      : Foreign master is-unicast Boolean.
                 *    Octet 26      : Foreign master network protocol (unicast foreign master).
                 *    Octet 27...28 : Foreign master port address length (unicast foreign master).
                 *    Octet 29...44 : Foreign master port address (unicast foreign master).
                 *    Octet 45      : Foreign master logAnnounceInterval.
                 */
                ptp_cb_printf("   Foreign Master Properties\n");
                ptp_cb_printf("   Port Identity      : "
                           "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
                           msg->data[14], msg->data[15], msg->data[16], msg->data[17],
                           msg->data[18], msg->data[19], msg->data[20], msg->data[21],
                           _bcm_ptp_uint16_read(msg->data + 14 +
                                                sizeof(bcm_ptp_clock_identity_t)));
                ptp_cb_printf("   Acceptable Master  : %u\n", msg->data[24]);
                ptp_cb_printf("   Unicast Master     : %u\n", msg->data[25]);

                if (msg->data[25]) {
                    ucm_protocol = msg->data[26];

                   ptp_cb_printf("   UC Master Protocol: %u", ucm_protocol);
                   if (ucm_protocol == bcmPTPIEEE8023) {
                        ptp_cb_printf(" (Ethernet Layer 2)\n");
                    } else if (ucm_protocol == bcmPTPUDPIPv4) {
                        ptp_cb_printf(" (Ethernet/UDP/IPv4)\n");
                        ptp_cb_printf("   UC Master Address  : %u.%u.%u.%u (IPv4)\n",
                                   msg->data[29], msg->data[30], msg->data[31], msg->data[32]);
                    } else if (ucm_protocol == bcmPTPUDPIPv6) {
                        ptp_cb_printf(" (Ethernet/UDP/IPv6)\n");
                        ptp_cb_printf("   UC Master Address  : "
                                   "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                                   "%02x%02x:%02x%02x:%02x%02x:%02x%02x (IPv6)\n",
                                   msg->data[29], msg->data[30], msg->data[31], msg->data[32],
                                   msg->data[33], msg->data[34], msg->data[35], msg->data[36],
                                   msg->data[37], msg->data[38], msg->data[39], msg->data[40],
                                   msg->data[41], msg->data[42], msg->data[43], msg->data[44]);
                    } else {
                        ptp_cb_printf("   UC Master Protocol : %u (Unknown)\n", ucm_protocol);
                    }
                }
                ptp_cb_printf("   logAnnounceInterval: %d\n", (int8)msg->data[45]);
                break;

            default:
              ;
            }
        } else if (verboseEvents == 1) {
            ptp_cb_printf("Event %s: Warning: %s", time_str,
                          diag_ieee1588_warn_reason_description(msg->data[13]));
        }
        break;

    case _bcm_ptp_servo_state_event:
        if (verboseEvents == 2) {
            ptp_cb_printf("Event %s: SERVO STATE\n", time_str);
            ptp_cb_printf("   Instance  : %d\n", msg->data[0]);
            ptp_cb_printf("   Old State : %d (%s)\n", msg->data[2],
                          diag_servo_state_description(msg->data[2]));
            ptp_cb_printf("   New State : %d (%s)\n", msg->data[1],
                          diag_servo_state_description(msg->data[1]));
        } else if (verboseEvents == 1) {
            ptp_cb_printf("Event %s: SERVO STATE %s -> %s\n",
                          time_str,
                          diag_servo_state_description(msg->data[2]),
                          diag_servo_state_description(msg->data[1]));
        }
        break;

    case _bcm_ptp_pps_in_state_event:
        if (verboseEvents == 2) {
            ptp_cb_printf("Event %s: PPS-IN STATE\n", time_str);
            ptp_cb_printf("   State : %d (%s)\n", msg->data[0],
                          diag_pps_in_state_description(msg->data[0]));
        } else if (verboseEvents == 1) {
            ptp_cb_printf("Event %s: PPS-IN STATE : %s\n", time_str,
                          diag_pps_in_state_description(msg->data[0]));
        }
        break;

    case _bcm_ptp_servo_log_event:
        {
            /* unsigned time_sec_hi = _bcm_ptp_uint32_read(msg->data+1); */
            unsigned time_sec_lo = _bcm_ptp_uint32_read(msg->data+5);
            unsigned event_id = _bcm_ptp_uint16_read(msg->data+9);
            unsigned severity_num = msg->data[11];
            unsigned state_num = msg->data[12];
            unsigned aux = _bcm_ptp_uint32_read(msg->data+13);
            char *servo_msg = (char*)msg->data+17;
            const char *state[4] = {"CLR", "SET", "TRANS", "OTHER"};
            const char *severity[8] = {"EMRG", "ALRT", "CRIT", "ERR", "WARN", "NOTC", "INFO", "DBG"};

            if (verboseEvents == 2) {
                ptp_cb_printf("Event %s: SERVO LOG\n", time_str);
                ptp_cb_printf("    %u: %u %s %s (%u)\n", time_sec_lo, event_id,
                              severity[severity_num], state[state_num], aux);
                ptp_cb_printf("      <%s>\n", servo_msg);
            } else if (verboseEvents == 1) {
                ptp_cb_printf("Event %s: SERVO %s %s <%s> (%d)\n", time_str,
                              severity[severity_num], state[state_num], servo_msg, aux);
            }

        }
        break;

    case _bcm_ptp_servo_lock_event:
        if (verboseEvents == 2) {
            ptp_cb_printf("Event %s: SERVO LOCK STATUS\n", time_str);
            ptp_cb_printf("   Instance  : %d\n", msg->data[0]);
            ptp_cb_printf("   Old Status : %d (%s)\n", msg->data[2],
                          diag_servo_lock_status_description(msg->data[2]));
            ptp_cb_printf("   New Status : %d (%s)\n", msg->data[1],
                          diag_servo_lock_status_description(msg->data[1]));
        } else if (verboseEvents == 1) {
            ptp_cb_printf("Event %s: SERVO LOCK STATUS %s -> %s\n",
                          time_str,
                          diag_servo_lock_status_description(msg->data[2]),
                          diag_servo_lock_status_description(msg->data[1]));
        }
        break;

    case _bcm_ptp_synce_loss_event:
#ifdef BCM_ESMC_EXTDPLL_SUPPORT
        if (verboseEvents == 2) {
            ptp_cb_printf("Event %s: SYNCE LOSS \n", time_str);
            ptp_cb_printf("   Port  : %d\n", msg->data[0]);
        }
#else
        /* Ignore if not using ext DPLL */
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */
        break;

    case _bcm_ptp_ts1_offset_event:
        /*Nothing to do for now*/
        break;

#ifdef BCM_PTP_EXT_SERVO_SUPPORT
    case _bcm_ptp_qual_timer_exp_event:
        {
            bcm_ptp_clock_port_address_t address;
            address.addr_type = msg->data[0];

            ptp_cb_printf("Event %s: QUAL_TIMER_EXPIRY\n", time_str);
            ptp_cb_printf("   Master Protocol: %u", msg->data[0]);

            if(address.addr_type == bcmPTPUDPIPv4) {
                sal_memcpy(address.address, &msg->data[1], PTP_IPV4_ADDR_SIZE_BYTES);
                ptp_cb_printf(" (Ethernet/UDP/IPv4)\n");
                ptp_cb_printf("   Master Address : %u.%u.%u.%u (IPv4)\n",
                           msg->data[1], msg->data[2], msg->data[3], msg->data[4]);
            } else if(address.addr_type == bcmPTPUDPIPv6) {
                sal_memcpy(address.address, &msg->data[1], PTP_IPV6_ADDR_SIZE_BYTES);
                ptp_cb_printf(" (Ethernet/UDP/IPv6)\n");
                ptp_cb_printf("   Master Address  : "
                        "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                        "%02x%02x:%02x%02x:%02x%02x:%02x%02x (IPv6)\n",
                        msg->data[1], msg->data[2], msg->data[3], msg->data[4],
                        msg->data[5], msg->data[6], msg->data[7], msg->data[8],
                        msg->data[9], msg->data[10], msg->data[11], msg->data[12],
                        msg->data[13], msg->data[14], msg->data[15], msg->data[16]);
            } else if(address.addr_type == bcmPTPIEEE8023) {
                ptp_cb_printf(" (Ethernet Layer 2)\n");
                ptp_cb_printf("   Master Address : "
                    "%02x:%02x:%02x:%02x:%02x:%02x (MAC)\n",
                    msg->data[1], msg->data[2], msg->data[3], msg->data[4],
                    msg->data[5], msg->data[6]);
                sal_memcpy(address.address, &msg->data[1], PTP_MAC_ADDR_SIZE_BYTES);
            }

            if (BCM_FAILURE(rv = bcm_ptp_telecom_g8265_packet_master_lockout_set(unit,
                                    stack_id, clock_num, 1, &address))) {
                PTP_ERROR_FUNC_CB("bcm_ptp_telecom_g8265_packet_master_lockout_set()");
                return rv;
            }

        }
        break;
#endif /* BCM_PTP_EXT_SERVO_SUPPORT */

    default:
        ptp_cb_printf("Unexpected event %s:, type %d\n", time_str, event_type);

        break;
    }
    /* Rewind cursor. */
    msg->data = msg->data - sizeof(uint16);

    return rv;
}

/*
 * Function:
 *      diag_management_callback
 * Purpose:
 *      Default forwarded (tunneled) PTP management message callback handler.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      stack_id   - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      clock_port - (IN) PTP port number.
 *      type       - (IN) Callback function type.
 *      msg        - (IN) Callback message data.
 *      user_data  - (IN) Callback user data.
 * Returns:
 *      BCM_E_XXX (if failure)
 *      bcmPTPCallbackAccept (if success)
 * Notes:
 */
STATIC int
diag_management_callback(
    int unit,
    bcm_ptp_stack_id_t stack_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_cb_type_t type,
    bcm_ptp_cb_msg_t *msg,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, stack_id, clock_num,
            clock_port))) {
        PTP_ERROR_FUNC_CB("_bcm_ptp_function_precheck()");
        return rv;
    }

    ptp_cb_printf("Management callback (Unit = %d, PTP Stack = %d)\n", unit, stack_id);

    return bcmPTPCallbackAccept;
}


/*
 * Function:
 *      diag_signaling_arbiter()
 * Purpose:
 *      Reference implementation to accept/reject an incoming PTP signaling message.
 * Parameters:
 *      unit                 - (IN) Unit number.
 *      amsg                 - (IN/OUT) Arbiter message data.
 *      user_data            - (IN) User data.
 * Returns:
 *      bcmPTPCallbackAccept - Criteria met; accept PTP signaling message.
 *      bcmPTPCallbackReject - Criteria not met; reject PTP signaling message.
 * Note:
 *      Reference implementation:
 *      Filter message based on PTP domain, interval, and duration.
 *      Set Tx timestamp mechanism to the default for the platform.
 */
bcm_ptp_callback_t diag_signaling_arbiter(
    int unit,
    bcm_ptp_cb_signaling_arbiter_msg_t *amsg,
    void * user_data)
{
    uint32 durationField_min;
    uint32 durationField_max;
    bcm_ptp_clock_port_info_t port_data;


    /*
     * Reject unicast negotiation requests if arbiter deny-all
     * mechanism is enabled or local clock is slave-only clock.
     */
    if ((PTR_TO_INT(user_data) & ARBITER_DENY_FLAG) ||
        (amsg->clock_info->slaveonly)) {
        return bcmPTPCallbackReject;
    }

    /*
     * Check logInterMessagePeriod against values stored in clock instance.
     * Reference implementation: Reject PTP signaling messages that request
     *                           intervals outside PTP profile range.
     */
    switch (*(amsg->messageType)) {
    case (bcmPTP_MESSAGE_TYPE_ANNOUNCE):
        if ((*(amsg->logInterMessagePeriod) < amsg->clock_info->log_announce_interval_minimum) ||
            (*(amsg->logInterMessagePeriod) > amsg->clock_info->log_announce_interval_maximum)) {

            PTP_VERBOSE_CB(("%s() failed %s\n", FUNCTION_NAME(), "Log announce interval outside PTP profile range.\n"));
            return bcmPTPCallbackReject;
        }
        break;

    case (bcmPTP_MESSAGE_TYPE_SYNC):
        if ((*(amsg->logInterMessagePeriod) < amsg->clock_info->log_sync_interval_minimum) ||
            (*(amsg->logInterMessagePeriod) > amsg->clock_info->log_sync_interval_maximum)) {

            PTP_VERBOSE_CB(("%s() failed %s\n", FUNCTION_NAME(), "Log sync interval outside PTP profile range.\n"));
            return bcmPTPCallbackReject;
        }
        break;

    case (bcmPTP_MESSAGE_TYPE_DELAY_RESP):
        if ((*(amsg->logInterMessagePeriod) < amsg->clock_info->log_min_delay_req_interval_minimum) ||
            (*(amsg->logInterMessagePeriod) > amsg->clock_info->log_min_delay_req_interval_maximum)) {

            PTP_VERBOSE_CB(("%s() failed %s\n", FUNCTION_NAME(), "Log delay interval outside PTP profile range.\n"));
            return bcmPTPCallbackReject;
        }
        break;

    default:
        return bcmPTPCallbackReject;
    }

    /*
     * Check durationField against PTP profile range.
     * Reference implementation: Reject PTP signaling messages that request
     *                           a duration outside PTP profile range.
     */
    if (BCM_FAILURE(bcm_ptp_unicast_request_duration_min_get(unit,
            amsg->ptp_id, amsg->clock_num, amsg->port_num, &durationField_min)) ||
        BCM_FAILURE(bcm_ptp_unicast_request_duration_max_get(unit,
            amsg->ptp_id, amsg->clock_num, amsg->port_num, &durationField_max)) ||
        *(amsg->durationField) < durationField_min || *(amsg->durationField) > durationField_max) {

        PTP_VERBOSE_CB(("%s() failed %s\n", FUNCTION_NAME(), "durationField outside PTP profile range.\n"));
        return bcmPTPCallbackReject;
    }

    if (BCM_FAILURE(bcm_ptp_clock_port_info_get(unit, amsg->ptp_id, amsg->clock_num,
        	   amsg->port_num, &port_data))) {
        PTP_VERBOSE_CB(("%s() failed %s\n", FUNCTION_NAME(), "failed to get port data.\n"));
        return bcmPTPCallbackReject;
    }

#if defined(PTP_KEYSTONE_STACK)
    amsg->slave->tx_timestamp_mech = bcmPTPToPTimestamps;
#else
    amsg->slave->tx_timestamp_mech = port_data.rx_timestamp_mechanism;
#endif

    /* Accept the default L2 header, which is simply the incoming L2 header with the MAC fields reversed */

    return bcmPTPCallbackAccept;
}


/*
 * Function:
 *      diag_ctdev_alarm_callback
 * Purpose:
 *      C-TDEV alarm callback example.
 *      Use ITU-T G.823 SEC TDEV mask to characterize pass/fail.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      ptp_id     - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      alarm_data - (IN) C-TDEV alarm data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
STATIC int
diag_ctdev_alarm_callback(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_ctdev_alarm_data_t *ctdev_alarm_data)
{
    int i;
    uint32 g823_mask_psec[BCM_PTP_CTDEV_NUM_TIMESCALES_MAX] = {0};
    int g823_mask_exceed[BCM_PTP_CTDEV_NUM_TIMESCALES_MAX] = {0};
    char uint64_decimal_string[27];

    for (i = 0; i < ctdev_alarm_data->num_tau; ++i) {
        /* ITU-T G.823 SEC TDEV mask criteria. */
        g823_mask_psec[i] = _bcm_ptp_ctdev_g823_mask(ctdev_alarm_data->tau_sec[i]);
        if (COMPILER_64_HI(ctdev_alarm_data->tdev_psec[i]) ||
            COMPILER_64_LO(ctdev_alarm_data->tdev_psec[i]) > g823_mask_psec[i]) {
            g823_mask_exceed[i] = 1;
        }
    }

    /*
     * Display formatted C-TDEV(tau) and pass/fail classification based on
     * ITU-T G.823 SEC TDEV mask.
     */
    cli_out("C-TDEV psec @T=%u: ", (unsigned)sal_time());
    for(i = 0; i < 11; ++i) {
        format_uint64_decimal(uint64_decimal_string, ctdev_alarm_data->tdev_psec[i], ',');
        cli_out("| (%d) %s - %s ", ctdev_alarm_data->tau_sec[i], uint64_decimal_string, g823_mask_exceed[i] ? "F":"P");
    }
    cli_out("\n");
     
    return BCM_E_NONE;
}


/*
 * Function:
 *      diag_tdpll_input_clock_monitor_callback()
 * Purpose:
 *      T-DPLL input clock monitoring callback example.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 *      clock_num - (IN) PTP clock number.
 *      cb_data   - (IN) Input clock monitor callback data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
STATIC int
diag_tdpll_input_clock_monitor_callback(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_cb_data_t *cb_data)
{
    cli_out("T-DPLL Input Clock Monitoring Event\n");
    cli_out("Unit: %d   Stack: %d\n", unit, stack_id);
    cli_out("-----------------------------------------------------------"
            "---------------------\n");
    cli_out("Index : %d\n", cb_data->index);
    cli_out("Event : <%s> %s\n",
            cb_data->monitor_value ? "SET":"CLEAR",
            cb_data->monitor_type == bcm_tdpll_input_clock_monitor_type_soft_warn ?
            "Over Soft-Warn" :
            (cb_data->monitor_type == bcm_tdpll_input_clock_monitor_type_hard_accept ?
            "Under Hard-Accept" :
            (cb_data->monitor_type == bcm_tdpll_input_clock_monitor_type_hard_reject ?
            "Over Hard-Reject":"Unknown")));
    cli_out("\n");

    return BCM_E_NONE;
}

/*
 * Function:
 *      diag_tdpll_input_clock_selector_callback()
 * Purpose:
 *      T-DPLL input clock reference selection callback example.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      cb_data  - (IN) Input clock reference selection callback data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
STATIC int
diag_tdpll_input_clock_selector_callback(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_selector_cb_data_t *cb_data)
{
    cli_out("T-DPLL Reference Selection Event\n");
    cli_out("Unit: %d   Stack: %d\n", unit, stack_id);
    cli_out("-----------------------------------------------------------"
            "---------------------\n");
    cli_out("DPLL : %d\n", cb_data->dpll_index);
    cli_out("Ref.  : %d (%d)\n", cb_data->selected_clock, cb_data->prior_selected_clock);
    cli_out("\n");

    return BCM_E_NONE;
}

/*
 * Function:
 *      diag_tdpll_input_clock_notification_callback()
 * Purpose:
 *      T-DPLL input clock notification callback example.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 *      cb_data   - (IN) Input clock callback data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
STATIC int
diag_tdpll_input_clock_notification_callback(
    int unit,
    int stack_id,
    void *cb_data)
{

    bcm_tdpll_input_clock_ref_change_cb_data_t *noti_cb_data;
    noti_cb_data = (bcm_tdpll_input_clock_ref_change_cb_data_t *)cb_data;

    cli_out("T-DPLL Input Clock Notification Event\n");
    cli_out("Unit: %d   Stack: %d\n", unit, stack_id);
    cli_out("-----------------------------------------------------------\n");

    switch (noti_cb_data->noti_type) {
    case bcmTdpllNotificationTypeRefchange:
        cli_out("Notification event: %s\n", "bcmTdpllNotificationTypeRefchange");
        cli_out("DPLL : %d\n", noti_cb_data->dpll_index);
        cli_out("Ref.  : %d (%d)\n", noti_cb_data->selected_clock, noti_cb_data->prior_selected_clock);
        break;
    default:
        cli_out("Invalid notification type[%d]\n", noti_cb_data->noti_type);
        break;
    }

    cli_out("\n");
    return BCM_E_NONE;
}

/*
 * Function:
 *      diag_tdpll_input_clock_callback()
 * Purpose:
 *      T-DPLL input clock callback example.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      stack_id  - (IN) Stack identifier index.
 *      cb_data   - (IN) Input clock callback data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
STATIC int
diag_tdpll_input_clock_callback(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_cb_data_t *cb_data)
{
    switch (cb_data->callback_type) {
    case bcmTdpllCallbackTypeMonitor:
         cli_out("diag_tdpll_input_clock_callback for callback type [bcmTdpllCallbackTypeMonitor] \n");
            diag_tdpll_input_clock_monitor_callback(unit, stack_id, 
               (bcm_tdpll_input_clock_monitor_cb_data_t *)cb_data->cb_info);
         break;

    case bcmTdpllCallbackTypeSelector:
         cli_out("diag_tdpll_input_clock_callback for callback type [bcmTdpllCallbackTypeSelector] \n");
            diag_tdpll_input_clock_selector_callback(unit, stack_id, 
               (bcm_tdpll_input_clock_selector_cb_data_t *)cb_data->cb_info);
         break;

    case bcmTdpllCallbackTypeNotification:
         cli_out("diag_tdpll_input_clock_callback for callback type [bcmTdpllCallbackTypeNotification] \n");
            diag_tdpll_input_clock_notification_callback(unit, stack_id, cb_data->cb_info);
         break;

    default:
         cli_out("diag_tdpll_input_clock_callback for invalid callback type [%d] \n", cb_data->callback_type);
         break;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      diag_tdpll_input_clock_ql_change_callback()
 * Purpose:
 *      ESMC Ql change for input clock reference callback example.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      stack_id - (IN) Stack identifier index.
 *      cb_data  - (IN) Input clock ESMC QL callback data.
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
STATIC int
diag_tdpll_input_clock_ql_change_callback(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_ql_change_cb_data_t *cb_data)
{
    cli_out("ESMC QL Change\n");
    cli_out("Unit: %d   Stack: %d\n", unit, stack_id);
    cli_out("-----------------------------------------------------------"
            "---------------------\n");
    cli_out("input_clk_index : %d\n", cb_data->input_clk_index);
    cli_out("QL  : %d (%d)\n", cb_data->current_ql, cb_data->prior_ql);
    cli_out("\n");
    /* Simple implementation of Store the state change in DB */
    /* Recommended to use Queue for communication between threads */
    ql_change_data.input_clk_index = cb_data->input_clk_index;
    ql_change_data.current_ql = cb_data->current_ql;
    ql_change_data.prior_ql = cb_data->prior_ql;

     if (ql_change_data.current_ql != ql_change_data.prior_ql) {
        bcm_esmc_network_option_t network_option;
        bcm_esmc_g781_option_get(0, 0, &network_option);
        if (network_option == bcm_esmc_network_option_g781_I) {
           cli_out("Network Option  : %d\n", network_option);
            if (ql_change_data.current_ql >= bcm_esmc_g781_I_ql_sec) {
                cli_out("Disable the Clock  : %d\n", ql_change_data.input_clk_index);
                sal_dpc((sal_dpc_fn_t)bcm_tdpll_input_clock_enable_set,
                                INT_TO_PTR(unit),
                                INT_TO_PTR(stack_id),
                                INT_TO_PTR(ql_change_data.input_clk_index),
                                INT_TO_PTR(FALSE), 0);
            } else {
                cli_out("Enable the Clock  : %d\n", ql_change_data.input_clk_index);
                sal_dpc((sal_dpc_fn_t)bcm_tdpll_input_clock_enable_set,
                    INT_TO_PTR(unit),
                    INT_TO_PTR(stack_id),
                    INT_TO_PTR(ql_change_data.input_clk_index),
                    INT_TO_PTR(TRUE), 0);
            }
        }
        ql_change_data.prior_ql = ql_change_data.current_ql;
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *      diag_port_state_description
 * Purpose:
 *      Interpret PTP port state values.
 * Parameters:
 *      state - (IN) PTP port state code.
 * Returns:
 *      Port state description
 * Notes:
 *      Ref. IEEE Std. 1588-2008, Chapter 8.2.5.3.1, Table 8.
 */
STATIC const char*
diag_port_state_description(_bcm_ptp_port_state_t state)
{
    switch(state) {
    case _bcm_ptp_state_initializing:
        return "Init";

    case _bcm_ptp_state_faulty:
        return "Faulty";

    case _bcm_ptp_state_disabled:
        return "Disabled";

    case _bcm_ptp_state_listening:
        return "Listening";

    case _bcm_ptp_state_pre_master:
        return "Pre-Master";

    case _bcm_ptp_state_master:
        return "Master";

    case _bcm_ptp_state_passive:
        return "Passive";

    case _bcm_ptp_state_uncalibrated:
        return "Uncalibrated";

    case _bcm_ptp_state_slave:
        return "Slave";

    default:
        return "<invalid>";
    }
}

/*
 * Function:
 *      diag_ieee1588_warn_reason_description
 * Purpose:
 *      Interpret IEEE Std. 1588-2008 warning values.
 * Parameters:
 *      reason - (IN) IEEE Std. 1588-2008 warning code.
 * Returns:
 *      Warning reason.
 * Notes:
 *      Function provides an implementation specific set of warnings
 *      relevant to the IEEE Std. 1588-2008.
 */
STATIC const char*
diag_ieee1588_warn_reason_description(
    _bcm_ptp_ieee1588_warn_reason_t reason)
{
   switch(reason) {
   case _bcm_ptp_ieee1588_warn_reason_logAnnounceInterval:
       return "Non-uniform logAnnounceInterval in PTP domain";

   default:
       return "<invalid>";
   }
}

/*
 * Function:
 *      diag_servo_state_description
 * Purpose:
 *      Interpret servo state values.
 * Parameters:
 *      state - (IN) Servo state code.
 * Returns:
 *      Servo state description
 * Notes:
 */
#ifdef BCM_PTP_EXT_SERVO_SUPPORT
STATIC const char*
diag_servo_state_description(
    int state)
{
    switch (state) {
    case bcmPtpServoStateAcquiring:
        return "Acquiring Lock";

    case bcmPtpServoStateWarmup:
        return "Warmup";

    case bcmPtpServoStateFastLoop:
        return "Fast Loop";

    case bcmPtpServoStateNormal:
        return "Normal Loop";

    case bcmPtpServoStateBridge:
        return "Bridge";

    case bcmPtpServoStateHoldover:
        return "Holdover";

    case bcmPtpServoStateInitial:
        return "Initial";

    case bcmPtpServoStateUnqualified:
        return "Unqualified";

    case bcmPtpServoStateFreqLocked:
        return "Frequency Locked";

    case bcmPtpServoStateTimeLocked:
        return "Time Locked";

    case bcmPtpServoStateHoldoverInSpec:
        return "Holdover-in-spec";

    case bcmPtpServoStateHoldoverOutOfSpec:
        return "Holdover-out-of-spec";

    case bcmPtpServoStateFreerun:
        return "Free run";

    default:
        return "<Unknown>";
    }
}

#else
STATIC const char*
diag_servo_state_description(
    bcm_ptp_fll_state_t state)
{
    switch (state) {
    case bcmPTPFLLStateAcquiring:
        return "Acquiring Lock";

    case bcmPTPFLLStateWarmup:
        return "Warmup";

    case bcmPTPFLLStateFastLoop:
        return "Fast Loop";

    case bcmPTPFLLStateNormal:
        return "Normal Loop";

    case bcmPTPFLLStateBridge:
        return "Bridge";

    case bcmPTPFLLStateHoldover:
        return "Holdover";

    default:
        return "<Unknown>";
    }
}
#endif

/*
 * Function:
 *      diag_servo_lock_status_description
 * Purpose:
 *      Interpret servo lock status values.
 * Parameters:
 *      state - (IN) Servo lock status code.
 * Returns:
 *      Servo lock status description
 * Notes:
 */
STATIC const char*
diag_servo_lock_status_description(
    bcm_ptp_servo_lock_status_t state)
{
    switch (state) {
    case bcmPTPServoStatusUnlocked:
        return "Unlocked";

    case bcmPTPServoStatusFreqLocked:
        return "Frequency Locked";

    case bcmPTPServoStatusPhaseLocked:
        return "Phase Locked";

    case bcmPTPServoStatusFreqPhaseLocked:
        return "Freq-Phase Locked";
    
    default:
        return "<Unknown>";
    }
}

/*
 * Function:
 *      diag_pps_in_state_description
 * Purpose:
 *      Interpret PPS-in state values.
 * Parameters:
 *      state - (IN) PPS-in state code.
 * Returns:
 *      PPS-in state description
 * Notes:
 */
STATIC const char*
diag_pps_in_state_description(
    _bcm_ptp_pps_in_state_t state)
{
    switch (state) {
    case _bcm_ptp_pps_in_state_missing:
        return "No PPS IN";

    case _bcm_ptp_pps_in_state_active_missing_tod:
        return "PPS IN, but no valid ToD";

    case _bcm_ptp_pps_in_state_valid:
        return "PPS IN with valid ToD";

    default:
        return "<Unknown>";
    }
}

/*
 * Function:
 *      diag_telecom_QL_description
 * Purpose:
 *      Interpret telecom profile QL values.
 * Parameters:
 *      QL - (IN) Telecom profile quality level (QL) value.
 * Returns:
 *      ITU-T G.781 QL description.
 * Notes:
 */
STATIC const char*
diag_telecom_QL_description(
    const bcm_ptp_telecom_g8265_quality_level_t QL)
{
    switch (QL) {
    case bcm_ptp_telecom_g8265_ql_I_prc:
        return "QL-PRC";
    case bcm_ptp_telecom_g8265_ql_I_ssua:
        return "QL-SSU-A";
    case bcm_ptp_telecom_g8265_ql_I_ssub:
        return "QL-SSU-B";
    case bcm_ptp_telecom_g8265_ql_I_sec:
        return "QL-SEC";
    case bcm_ptp_telecom_g8265_ql_I_dnu:
        return "QL-DNU";
    case bcm_ptp_telecom_g8265_ql_II_prs:
        return "QL-PRS";
    case bcm_ptp_telecom_g8265_ql_II_stu:
        return "QL-STU";
    case bcm_ptp_telecom_g8265_ql_II_st2:
        return "QL-ST2";
    case bcm_ptp_telecom_g8265_ql_II_tnc:
        return "QL-TNC";
    case bcm_ptp_telecom_g8265_ql_II_st3e:
        return "QL-ST3E";
    case bcm_ptp_telecom_g8265_ql_II_st3:
        return "QL-ST3";
    case bcm_ptp_telecom_g8265_ql_II_smc:
        return "QL-SMC";
    case bcm_ptp_telecom_g8265_ql_II_prov:
        return "QL-PROV";
    case bcm_ptp_telecom_g8265_ql_II_dus:
        return "QL-DUS";
    case bcm_ptp_telecom_g8265_ql_III_unk:
        return "QL-UNK";
    case bcm_ptp_telecom_g8265_ql_III_sec:
        return "QL-SEC";
    case bcm_ptp_telecom_g8265_ql_na_slv:
        return "N/A";
    default:
        return "<Invalid>";
    }
}

/*
 * Function:
 *      diag_telecom_pktmaster_state_description
 * Purpose:
 *      Interpret telecom profile packet master state.
 * Parameters:
 *      state - (IN) Telecom profile packet master state.
 * Returns:
 *      Packet master state description.
 * Notes:
 */
STATIC const char*
diag_telecom_pktmaster_state_description(
    const bcm_ptp_telecom_g8265_pktmaster_state_t state)
{
    switch (state) {
    case bcm_ptp_telecom_g8265_pktmaster_state_unused:
        return "Unused";
    case bcm_ptp_telecom_g8265_pktmaster_state_init:
        return "Initialized";
    case bcm_ptp_telecom_g8265_pktmaster_state_valid:
        return "Valid";
    default:
        return "<Invalid>";
    }
}

/*
 * Function:
 *      diag_telecom_pktmaster_printout
 * Purpose:
 *      Print telecom profile packet master attributes.
 * Parameters:
 *      pktmaster - (IN) Telecom profile packet master.
 *      flags     - (IN) Printout control flags.
 * Returns:
 *      None.
 * Notes:
 */
STATIC void
diag_telecom_pktmaster_printout(
    bcm_ptp_telecom_g8265_pktmaster_t *pktmaster,
    uint8 flags)
{
    sal_time_t dt, dt_p, dt_q;
    sal_time_t t = _bcm_ptp_monotonic_time();
    uint64 t64;

    if (bcmPTPUDPIPv4 == pktmaster->port_address.addr_type) {
        cli_out("IP (IPv4)       : %u.%u.%u.%u\n",
                pktmaster->port_address.address[0], pktmaster->port_address.address[1],
                pktmaster->port_address.address[2], pktmaster->port_address.address[3]);
    } else if (bcmPTPUDPIPv6 == pktmaster->port_address.addr_type) {
        cli_out("IP (IPv6)       : %02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                "%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
                pktmaster->port_address.address[0],  pktmaster->port_address.address[1],
                pktmaster->port_address.address[2],  pktmaster->port_address.address[3],
                pktmaster->port_address.address[4],  pktmaster->port_address.address[5],
                pktmaster->port_address.address[6],  pktmaster->port_address.address[7],
                pktmaster->port_address.address[8],  pktmaster->port_address.address[9],
                pktmaster->port_address.address[10], pktmaster->port_address.address[11],
                pktmaster->port_address.address[12], pktmaster->port_address.address[13],
                pktmaster->port_address.address[14], pktmaster->port_address.address[15]);
    }

    if (0 == flags) {
        return;
    }

    cli_out("portIdentity    : %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%04x\n",
            pktmaster->port_identity.clock_identity[0], pktmaster->port_identity.clock_identity[1],
            pktmaster->port_identity.clock_identity[2], pktmaster->port_identity.clock_identity[3],
            pktmaster->port_identity.clock_identity[4], pktmaster->port_identity.clock_identity[5],
            pktmaster->port_identity.clock_identity[6], pktmaster->port_identity.clock_identity[7],
            pktmaster->port_identity.port_number);

    cli_out("\n");
    cli_out("State           : %s\n",
            diag_telecom_pktmaster_state_description(pktmaster->state));

    cli_out("\n");
    cli_out("ITU-T G.781 QL  : %s\n", diag_telecom_QL_description(pktmaster->quality_level));
    cli_out("Priority        : %u\n", pktmaster->priority.value);
    if (pktmaster->priority.override) {
        cli_out("   Override     : [x]\n");
    } else {
        cli_out("   Override     : [ ]\n");
    }
    cli_out("   GM priority1 : %u\n", pktmaster->grandmaster_priority1);
    cli_out("   GM priority2 : %u\n", pktmaster->grandmaster_priority2);

    cli_out("\n");
    if (pktmaster->selector.lockout) {
        cli_out("Lockout         : [x]\n");
    } else {
        cli_out("Lockout         : [ ]\n");
    }
    if (pktmaster->selector.non_reversion) {
        cli_out("Non-Reversion   : [x]\n");
    } else {
        cli_out("Non-Reversion   : [ ]\n");
    }
    if (pktmaster->selector.wait_to_restore) {
        if (pktmaster->selector.non_reversion) {
            cli_out("Wait-to-Restore : [x] (Inf. sec)\n");
        } else {
            COMPILER_64_SET(t64, 0, t);
            COMPILER_64_SUB_64(t64, pktmaster->ptsf.timestamp);
            if(COMPILER_64_LT(t64, pktmaster->selector.wait_sec)){
                t64 = pktmaster->ptsf.timestamp;
                COMPILER_64_ADD_64(t64, pktmaster->selector.wait_sec);
                COMPILER_64_SUB_32(t64, t);
                dt_p = COMPILER_64_LO(t64);
            } else {
                dt_p = 0;
            }
            COMPILER_64_SET(t64, 0, t);
            COMPILER_64_SUB_64(t64, pktmaster->hql.timestamp);
            if(COMPILER_64_LT(t64, pktmaster->selector.wait_sec)){
                t64 = pktmaster->hql.timestamp;
                COMPILER_64_ADD_64(t64, pktmaster->selector.wait_sec);
                COMPILER_64_SUB_32(t64, t);
                dt_q = COMPILER_64_LO(t64);
            } else {
                dt_q = 0;
            }
            dt = (dt_p > dt_q) ? (dt_p):(dt_q);
            cli_out("Wait-to-Restore : [x] (%u sec)\n", (uint32)dt);
        }
    } else {
        cli_out("Wait-to-Restore : [ ]\n");
    }

    cli_out("\n");
    if (pktmaster->ptsf.loss_announce || pktmaster->ptsf.loss_sync ||
        pktmaster->ptsf.unusable) {
        cli_out("PTSF            : [x]\n");
    } else {
        cli_out("PTSF            : [ ]\n");
    }
    if (pktmaster->ptsf.loss_announce) {
        cli_out("                : [x]lossAnnounce   ");
    } else {
        cli_out("                : [ ]lossAnnounce   ");
    }
    if (pktmaster->ptsf.loss_sync) {
        cli_out("[x]lossSync         ");
    } else {
        cli_out("[ ]lossSync         ");
    }
    if (pktmaster->ptsf.unusable) {
        cli_out("[x]unusable\n");
    } else {
        cli_out("[ ]unusable\n");
    }
    if (pktmaster->ptsf.loss_timing_sync) {
        cli_out("                : [x]lossTimingSync ");
    } else {
        cli_out("                : [ ]lossTimingSync ");
    }
    if (pktmaster->ptsf.loss_timing_sync_ts) {
        cli_out("[x]lossTimingSyncTS ");
    } else {
        cli_out("[ ]lossTimingSyncTS ");
    }
    if (pktmaster->ptsf.loss_timing_delay) {
        cli_out("[x]lossTimingDelay\n");
    } else {
        cli_out("[ ]lossTimingDelay\n");
    }

    cli_out("\n");
    if (pktmaster->hql.degrade_ql) {
        cli_out("QL Degraded     : [x]\n");
    } else {
        cli_out("QL Degraded     : [ ]\n");
    }

#ifdef COMPILER_HAS_LONGLONG
    cli_out("\n");
    if (COMPILER_64_LO(pktmaster->pktstats.pdv_scaled_allan_var) == ((uint32)-1) &&
        COMPILER_64_HI(pktmaster->pktstats.pdv_scaled_allan_var) == ((uint32)-1)) {
        cli_out("MTSD Scaled AVAR: N/A\n");
    } else {
        cli_out("MTSD Scaled AVAR: %llu (nsec-sq)\n",
                (long long unsigned)pktmaster->pktstats.pdv_scaled_allan_var);
    }
#endif /* COMPILER_HAS_LONGLONG */
}


#endif /* __KERNEL__ */

#if defined(PTP_KEYSTONE_STACK)

#ifndef NO_FILEIO

uint32 ptp_ks_ihex_ext_addr; /* Extended Address */

/*
 * returs -1 if error
 * returns number of bytes. (0 if this record doesn't contain any data).
 */
STATIC int
ptp_parse_ihex_record(int unit, char *line, uint32 *off)
{
    int count;
    uint32 address;

    switch (line[8]) {      /* Record Type */
        case '0' :      /* Data Record */
            count = (xdigit2i(line[1]) << 4) |
                    (xdigit2i(line[2]));

            address = (xdigit2i(line[3]) << 12) |
                      (xdigit2i(line[4]) << 8) |
                      (xdigit2i(line[5]) << 4) |
                      (xdigit2i(line[6]));

            *off = ptp_ks_ihex_ext_addr + address;
            return count;
            break; /* Not Reachable */
        case '4' :      /* Extended Linear Address Record */
            address = (xdigit2i(line[9]) << 12) |
                      (xdigit2i(line[10]) << 8) |
                      (xdigit2i(line[11]) << 4) |
                      (xdigit2i(line[12]));

            ptp_ks_ihex_ext_addr = (address << 16);
            cli_out("Exteded Linear Address 0x%x\n", ptp_ks_ihex_ext_addr);
            return 0;
            break; /* Not Reachable */
        default :       /* We don't parse all other records */
            cli_out("Unsupported Record\n");
            return 0;
    }
    return -1; /* Why are we here? */ /* Not Reachable */
}

STATIC int
ptp_parse_srec_record (int unit, char *line, uint32 *off)
{
    int count;
    uint32 address;

    switch (line[1]) {      /* Record Type */
        case '0':           /* Header record - ignore */
            return 0;
            break; /* Not Reachable */

        case '1' :          /* Data Record with 2 byte address */
            count = (xdigit2i(line[2]) << 4) |
                    (xdigit2i(line[3]));
            count -= 3; /* 2 address + 1 checksum */

            address = (xdigit2i(line[4]) << 12) |
                      (xdigit2i(line[5]) << 8) |
                      (xdigit2i(line[6]) << 4) |
                      (xdigit2i(line[7]));

            *off = address;
            return count;
            break; /* Not Reachable */

        case '2' :          /* Data Record with 3 byte address */
            count = (xdigit2i(line[2]) << 4) |
                    (xdigit2i(line[3]));
            count -= 4; /* 3 address + 1 checksum */

            address = (xdigit2i(line[4]) << 20) |
                      (xdigit2i(line[5]) << 16) |
                      (xdigit2i(line[6]) << 12) |
                      (xdigit2i(line[7]) << 8) |
                      (xdigit2i(line[8]) << 4) |
                      (xdigit2i(line[9]));

            *off = address;
            return count;
            break; /* Not Reachable */

        case '3' :          /* Data Record with 4 byte address */
            count = (xdigit2i(line[2]) << 4) |
                    (xdigit2i(line[3]));
            count -= 5; /* 4 address + 1 checksum */

            address = (xdigit2i(line[4]) << 28) |
                      (xdigit2i(line[5]) << 24) |
                      (xdigit2i(line[6]) << 20) |
                      (xdigit2i(line[7]) << 16) |
                      (xdigit2i(line[8]) << 12) |
                      (xdigit2i(line[9]) << 8) |
                      (xdigit2i(line[10]) << 4) |
                      (xdigit2i(line[11]));

            *off = address;
            return count;
            break; /* Not Reachable */

        case '5':         /* Record count - ignore */
        case '6':         /* End of block - ignore */
        case '7':         /* End of block - ignore */
        case '8':         /* End of block - ignore */
        case '9':         /* End of block - ignore */
            return 0;
            break; /* Not Reachable */

        default :       /* We don't parse all other records */
            cli_out("Unsupported Record S%c\n", line[1]);
            return 0;
    }
    return -1; /* Why are we here? */ /* Not Reachable */
}


/*
 * It is assumed that by the time this routine is called
 * the record is already validated. So, blindly get data
 */
STATIC void
ptp_get_rec_data (char *line, int count, uint8 *dat)
{
    int i, datpos = 9;  /* 9 is valid for ihex */

    if (!count) {
        return;
    }

    if (line[0] != ':') {
        /* not ihex */
        switch (line[1]) {
        case '1':  /* S1 record */
            datpos = 8;
            break;
        case '2':  /* S2 record */
            datpos = 10;
            break;
        case '3':  /* S3 record */
            datpos = 12;
            break;
        default:
            cli_out("Unexpected record type: '%c'\n", line[1]);
            break;
        }
    }

    /*1 count = 2 hex chars */
    for(i = 0; i < count; i++) {
        *(dat + i) = ((xdigit2i(line[datpos + (i * 2)])) << 4) |
                    (xdigit2i(line[datpos + (i * 2) + 1]));
    }
}

STATIC cmd_result_t
ptp_file_load(int unit, FILE *fp, int ksnum) {
    uint32 addr=0;
    int rv = 0;
    char input[256], *cp = NULL;
    unsigned char data[256];
    uint32 phys_addr;

    ptp_ks_ihex_ext_addr = 0; /* Until an extended addr record is met.. */

    while (NULL != (cp = fgets(input, sizeof(input) - 1, fp))) {
        if (input[0] == 'S') {
            rv = ptp_parse_srec_record(unit, cp, &addr);
        } else if (input[0] == ':') {
            rv = ptp_parse_ihex_record(unit, cp, &addr);
        } else {
            cli_out("unknown Record Type\n");
            rv = -1;
        }

        if (-1 == rv) {
            return (CMD_FAIL);
        }

        if (rv % 4) {
            cli_out("record Not Multiple of 4\n");
            return (CMD_FAIL);
        }
        ptp_get_rec_data (cp, rv, data);

        /* for BCM53903, physical memory has top two addr bits == 0 */
        /* virt addr depends on cached status */

        phys_addr = (addr & 0x1fffffff);
        _bcm_ptp_write_pcishared_uint8_aligned_array(ksnum, phys_addr, data, rv);
    }

    return(CMD_OK);
}


STATIC cmd_result_t
ptp_file_load_bin(int unit, FILE *fp, int ksnum) {
    uint32 addr = 0x19000000;  /* base physical RAM address */
    unsigned char data[256];
    unsigned data_len;

    while (0 != (data_len = (unsigned) fread(data, 1, 256, fp))) {
        _bcm_ptp_write_pcishared_uint8_aligned_array(ksnum, addr, data, data_len);
        addr += data_len;
    }

    return(CMD_OK);
}

#endif


char cmd_topload_usage[] =
    "Parameters: <ksnum> <file.srec>\n\t\t"
#ifndef COMPILER_STRING_CONST_LIMIT
    "Load the MCS memory area from <file.srec>.\n\t"
    "ksnum = BCM53903 number to be loaded.\n"
#endif
    ;

cmd_result_t
cmd_topload(int unit, args_t *a)
/*
 * Function: 	cmd_topload
 * Purpose:	Load a file into ToP processor for 1588
 * Parameters:	unit - unit
 *		a - args, each of the files to be displayed.
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    cmd_result_t rv = CMD_OK;
    char *c , *filename;
    int ksnum;

#ifndef NO_FILEIO
    int resetks = 1; /* reset the TOP before loading */
    int startks = 0; /* don't start TOP after load: let stack start do that */

#ifndef NO_CTRL_C
    jmp_buf ctrl_c;
#endif
    FILE * volatile fp = NULL;
#endif

    if (ARG_CNT(a) < 2) {
        return(CMD_USAGE);
    }

    c = ARG_GET(a);
    if (!isint(c)) {
        cli_out("%s: Error: BCM53903 number not specified\n", ARG_CMD(a));
        return(CMD_USAGE);
    }

    ksnum = parse_integer(c);
    if (ksnum > 1) { 
        cli_out("Invalid uProcessor number: %d\n",ksnum);
        return(CMD_FAIL);
    }


    c = ARG_GET(a);
    filename = c;
    if (filename == NULL) {
        cli_out("%s: Error: No file specified\n", ARG_CMD(a));
        return(CMD_USAGE);
    }

#ifdef NO_FILEIO
    cli_out("no filesystem\n");
#else
#ifndef NO_CTRL_C
    if (!setjmp(ctrl_c)) {
        sh_push_ctrl_c(&ctrl_c);
#endif

        /* Reset the ToP before load */
        if (resetks) {
            _bcm_ptp_ext_stack_reset(ksnum);
        }

        fp = sal_fopen(filename, "r");
        if (!fp) {
            cli_out("%s: Error: Unable to open file: %s\n",
                    ARG_CMD(a), filename);
            rv = CMD_FAIL;
#ifndef NO_CTRL_C
            sh_pop_ctrl_c();
#endif
            return(rv);
        } else {
            if (sal_strlen(filename) > 4 &&
                sal_strcmp(filename + sal_strlen(filename) - 4, ".bin") == 0) {
                rv = ptp_file_load_bin(unit, fp, ksnum);
            } else {
                /* interpret as srec / ihex depending on file contents */
                rv = ptp_file_load(unit, fp, ksnum);
            }
            sal_fclose((FILE *)fp);
            fp = NULL;
        }

        if (startks) {
            _bcm_ptp_ext_stack_start(ksnum);
        }

#ifndef NO_CTRL_C
    } else if (fp) {
        sal_fclose((FILE *)fp);
        fp = NULL;
        rv = CMD_INTR;
    }

    sh_pop_ctrl_c();
#endif
#endif /* NO_FILEIO */
    sal_usleep(10000);

    return(rv);
}

char cmd_topinfo_usage[] =
    "Print information about ToP Firmware\n";

cmd_result_t
cmd_topinfo(int unit, args_t *a)
/*
 * Function:    cmd_topinfo
 * Purpose: Get information about TOP FW
 * Returns: CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    _bcm_ptp_ext_fw_info_t info;
    uint32 boot_status, fault_status;

    _bcm_ptp_ext_fw_info_get(unit, 0, &info);
    _bcm_ptp_ext_stack_fault_status_get(0, &boot_status, &fault_status);

    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("PTP Stack Information\n");
    cli_out("--------------------------------------------------------------------------------\n");
    cli_out("    Firmware Version       : %s\n", info.firmware_version);
    cli_out("    Servo Version          : %s\n", info.servo_version);
    cli_out("\n");
    cli_out("    PTP Timer Frequency    : %u Hz\n", info.ptp_stack_timer_hz);
    cli_out("\n");
    cli_out("    Max. # PTP Clocks      : %u\n", info.ptp_clocks_max);
    cli_out("    Max. # PTP Ports       : %u/clock\n", info.ptp_ports_per_clock_max);
    cli_out("\n");
    /*     cli_out("    Max. # Unicast Masters : %u\n", info.unicast_masters_max); */
    /*     cli_out("    Max. # Unicast Slaves  : %u\n", info.unicast_slaves_max); */
    cli_out("    Max. # Foreign Master  : %u/port\n", info.foreign_master_dataset_entries_max);
    cli_out("           Dataset Entries\n");
    cli_out("    Boot Status: %08x  Fault Status: %08x\n", boot_status, fault_status);
    cli_out("--------------------------------------------------------------------------------\n");

    return CMD_OK;
}

#endif  /* defined(PTP_KEYSTONE_STACK) */

#if defined(BCM_TRIDENT2_SUPPORT)
/*
 * Function:
 *      bcm_esw_ptp_map_input_synce_clock
 * Purpose:
 *      Map PTP input L1 clock from logical port to physical port.
 * Parameters:
 *      logical_l1_clk_port - (IN) Logical PTP input L1 clock port.
 * Returns:
 *      int - Physical PTP input L1 clock port.
 * Notes:
*/
int
bcm_esw_ptp_map_input_synce_clock(
   int logical_l1_clk_port)
{
   int port = 0;
   int n = logical_l1_clk_port / 4; 
   int lane = logical_l1_clk_port % 4;

   /************************************************************************************************* 
   For Trident 2, there are 132 (4 for each 32 TSCs plus 4 LCPLLs) ports on the L1 recovery mux.
   The SyncE clocks recovered from the 32 TSCs are multiplexed to the first 128 ports on the mux.
   Since these port numbers may not be the same as the front port numbers, an addtional parameter, 
   BackupTimeInputSynce, is added to the "ptp chan set" command to specify the SyncE input port on 
   the L1 clock recovery mux. To get the SyncE input port number, user needs to use "phy info" to
   indentify the front panel port which carries the input SyncE clock is mapped at lane x (x=0, 1, 
   2, 3) on TSC n (n= 0, 1, ..., 31). The BackupTimeInputSynce needs to be input with value of 
   n * 4 + x. 

   In the current Trident 2 H/W design, the TSC L1_recovery_clock[3:0] is from physical lane instead 
   of logic port as calculated above and expected by the TD2 top level. To resolve this issue, the 
   following mapping is added.

   For even TSC (TSC0 ...)
     i. link_status[3] <-> recovery_clock[3]
     ii. link_status[2] <-> recovery_clock[0]
     iii. link_status[1] <-> recovery_clock[1]
     iv. link_status[0] <-> recovery_clock[2]
   For odd TSC (TSC1 ...)
     i. Link_status[3] <-> recovery_clock[2]
     ii. Link_status[2] <-> recovery_clock[1]
     iii. Link_status[1] <-> recovery_clock[0]
     iv. Link_status[0] <-> recovery_clock[3]

   Note: The above mapping is based on Broadcom Trident 2 SVK. If customer has different board level 
   design, the mapping needs to be adjusted.
   
   The LCPLLs are multiplexed to ports 128 - 132. No mapping is needed for LCPLLs ports. 
 */
   if ((logical_l1_clk_port >= 128) && (logical_l1_clk_port <= 131)) 
   {
      return logical_l1_clk_port;
   }

   if ((n / 2) == 0) 
   {
      /* even TCS */
      if (lane == 0) 
         port = 2;
      else if (lane == 1) 
         port = 1;
      else if (lane == 2) 
         port = 0;
      else if (lane == 3) 
         port = 3;
   }
   else
   {
      /* odd TCS */
      if (lane == 0) 
         port = 3;
      else if (lane == 1) 
         port = 0;
      else if (lane == 2) 
         port = 1;
      else if (lane == 3) 
         port = 2;
   }

   return (n * 4 + port);
}
#endif /* BCM_TRIDENT2_SUPPORT */

#endif  /* defined(INCLUDE_PTP) */
