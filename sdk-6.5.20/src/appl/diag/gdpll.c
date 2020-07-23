/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    gdpll.c
 * Purpose: GDPLL diag shell commands
 */

#include <bcm/error.h>
#include <bcm/gdpll.h>
#include <bcm/time.h>
#include <bcm_int/esw/gdpll.h>
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#ifdef CPRIMOD_SUPPORT
#include <soc/cprimod/cprif_drv.h>
#endif
#include <soc/iproc.h>

#include <bcm/cpri.h>

#ifdef NO_SAL_APPL
#include <string.h>
#else
#include <sal/appl/sal.h>
#endif

#ifdef INCLUDE_GDPLL

#include <bcm_int/esw/port.h>
#include <include/soc/uc.h>
#if defined(UNIX) && !defined(__KERNEL__)
#include <time.h>
#define GDPLL_SAL_HAS_TIME  (1)
#endif

char cmd_gdpll_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: GDPLL <option> [args...]\n"
#else
    "\n"
    "gdpll chan create <config_filename>\n"
    "   Create the DPLL channel\n"
    "   UseCase-1A: gdpll chan create <dpll_1A_xxx.config> [SyncBS0HB/SyncBS1HB/SyncGPIO2]\n"
    "   UseCase-1G: gdpll chan create <dpll_1G_xxx.config> [SyncGPSToD]\n"
    "   Ensure to define use_case_flag in config as per the bellow table\n"
    "       use_case_flag:1 UseCase-1A\n"
    "       use_case_flag:2 UseCase-1E\n"
    "       use_case_flag:3 UseCase-1D\n"
    "       use_case_flag:4 UseCase-1G\n"
    "       use_case_flag:5 UseCase-2D\n"
    "       use_case_flag:6 UseCase-BS_Synthesizer\n"
    "       use_case_flag:7 UseCase-1N\n"
    "       use_case_flag:8 UseCase-1P\n"
    "       use_case_flag:9 UseCase-1D(opt3)\n"
    "gdpll flush\n"
    "   Flush GDPLL\n"
    "gdpll Rsvd1cbreg\n"
    "   Register callback for RSVD1 message\n"
    "gdpll Rsvd1cbunreg\n"
    "   Un-register callback for RSVD1 message\n"
    "gdpll debug <enable:0/1> <file_name/UDP> [DestMAC=<mac>] [DestAddr=<addr>] [SrcMAC=<mac>] [SrcAddr=<addr>] [UDPSrcPort=< port>] [UDPDestPort=< port>] [VLan=<vlan>] [TTL=<ttl>]"
    "   Set the GDPLL debug feature\n"
    "gdpll chan destroy <chan_num>\n"
    "   Destroy the DPLL channel\n"
    "gdpll chan enable <chan_num>\n"
    "   Enable the DPLL channel\n"
    "gdpll chan disable <chan_num>\n"
    "   Disable the the DPLL channel\n"
    "gdpll chan monitor <chan_num>\n"
    "   Monitor the channel status\n"
    "gdpll chan notify start/stop\n"
    "   Start/Stop asynchronous chan status reporting\n"
    "gdpll chan dump <chan_num>\n"
    "   Dump the channel debug info\n"
    "gdpll chan status <chan_num>\n"
    "   Get the status of DPLL channel\n"
    "   Bit-31  ChannelError status\n"
    "   Bit-30  LargeErrorIndicator\n"
    "   Bit-8   ChannelEnable Status\n"
    "   Bit-7:1 LossOfLockIndicator Count\n"
    "   Bit-0   Channel LockIndicator\n"
    "gdpll chan debug <chan_num> <debugMode>\n"
    "   Set the debug mode for a given channel\n"
    "     Mode:0 Channel debug logging Disbaled, closed-loop\n"
    "     Mode:1 Channel debug logging(record bellow, each 64b) in closed-loop\n"
    "     Mode:2 Channel debug logging(record bellow, each 64b) in open-loop\n"
    "            <Channel_num+ChannelState>, <FB-TS>, <REF-TS>, <PERROR>, <NCO>, <LoopFilter>, <LockDetFilter>, <NominalLoopFilter>\n"
    "     Mode:3 Channel debug logging(record bellow, each 64b) in open-loop\n"
    "            <FB-TS>\n"
    "gdpll offset <set/get> <chan_num> <offset_H> <offset_L>\n"
    "   offset_H/L - 32b/32b with LS:4b are sub-ns\n"
#endif
    ;

extern int _bcm_esw_get_timestamp(int unit, int timestamper, uint64 *pTs_52);
extern int _bcm_esw_timestamp_enable(int unit, int timestamper, int enable);
extern int _bcm_esw_set_timestamp(int unit, int timestamper, uint64 ts_52);
extern int _bcm_esw_gdpll_chan_debug_mode_get(int unit, int chan, bcm_gdpll_debug_mode_t *pDebugMode);
extern int _bcm_esw_gdpll_debug_get (int unit, int *pEnable);
extern int _bcm_esw_gdpll_debug_dump (int unit, dpll_dump_t *pChanDump);

static int debug_enable_flag = 0;
static int use_case_flag = 0;
static int disable_debug_log = 0;
static int port_out;

typedef struct gdpll_app_context_s {
    int chan_status[BCM_GDPLL_NUM_CHANNELS];
    int chan_mon_flag[BCM_GDPLL_NUM_CHANNELS];
} gdpll_app_context_t;
gdpll_app_context_t *pGdpllAppContext = NULL;

#ifndef NO_FILEIO
FILE * volatile fp = NULL;
#endif

#define GPS_ToD_DEMO

#ifdef GPS_ToD_DEMO
#define ToD_BUFFER_LENGTH   40
#define ToD_LOCATION        0x0127FF80  /* Monterey msg_base:0x0127f000, 4KB */
#define ToD_LOCATION_AVAIL  0x0127FFFC

#ifdef GDPLL_SAL_HAS_TIME
time_t time_to_epoch ( const struct tm *ltm, int utcdiff )
{
    const int mon_days [] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    long tyears, tdays, leaps, utc_hrs;
    int i;

    tyears = ltm->tm_year - 70 ; /* tm->tm_year is from 1900. */
    leaps = (tyears + 2) / 4; /* no of next two lines until year 2100. */
    /*i = (ltm->tm_year - 100) / 100; */
    /*leaps -= ( (i/4)*3 + i%4 ); */
    tdays = 0;
    for (i=0; i < ltm->tm_mon; i++)
        tdays += mon_days[i];

    tdays += ltm->tm_mday-1; /* days of month passed. */
    tdays = tdays + (tyears * 365) + leaps;
    utc_hrs = ltm->tm_hour + utcdiff; /* for your time zone. */
    return (tdays * 86400) + (utc_hrs * 3600) + (ltm->tm_min * 60) + ltm->tm_sec;
}

void str_to_tm ( char *mdate, char *mtime, struct tm* mtm )
{
    char *pstr;
    long year, month, day, hour, min, sec;

    year = strtol( mdate, &pstr, 10 );
    month = strtol( ++pstr, &pstr, 10 );
    day = strtol( ++pstr, &pstr, 10 );

    hour = strtol( mtime, &pstr, 10 );
    while( !isdigit(*pstr) )
        ++pstr;
    min = strtol( pstr, &pstr, 10 );
    while( !isdigit(*pstr) )
        ++pstr;
    sec = strtol( pstr, &pstr, 10 );
    mtm->tm_sec = sec;
    mtm->tm_min = min;
    mtm->tm_hour = hour;
    mtm->tm_mday = day;
    mtm->tm_mon = month - 1;
    mtm->tm_year = year - 1900;
}

time_t get_time(char *mydate, char *mytime)
{
    struct tm mtm;
    time_t mytcks;
    /* time_t ticks; */

    str_to_tm ( mydate, mytime, &mtm );
    mytcks = time_to_epoch ( &mtm, 5 );
    /* ticks = mktime ( &mtm ); */
#if 0
    cli_out ( " std func time : %s", asctime(localtime(&ticks)));
    cli_out ( " our func time : %s", asctime(localtime(&mytcks)));
    cli_out ( " stdlib func ticks : %ld \n", ticks );
    cli_out ( " our func ticks : %ld \n", mytcks );
#endif
    return mytcks;
}
#endif  /* GDPLL_SAL_HAS_TIME */
#endif  /* GPS_ToD_DEMO */

int gdpll_debug_cb(int unit, void *user_data, uint32 debug_buff, uint32 debug_buf_size)
{
    int rv  = BCM_E_NONE;

#ifndef NO_FILEIO
    int i;
    uint32 val;
    uint32 rd_addr = debug_buff;;

    /* Write the debug buffer to a file registered */
    if (fp) {
        for (i=0; i<debug_buf_size; i++, rd_addr+=4) {
            val = swap32(soc_cm_iproc_read(unit, rd_addr));
            if (sal_fwrite(&val,4, 1, fp) != 1) {
                cli_out("Error writing to file\n");
                rv = BCM_E_FAIL;
            }
        }
    }
#endif

    return rv;
}

bcm_mac_t dest_mac = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; /* default to broadcast */
bcm_mac_t src_mac = {0x00, 0x10, 0x18, 0x00, 0x00, 0x01};
int vlan = 1;
/* Simple IPv4 UDP header */
uint8       L3_header[] = {
    0x08, 0x00,                           /* ethertype: IPv4 */
    0x45, 0x00,                           /* version, hdr len, ECN */
    0x00, 0x00,                           /* length */
    0x00, 0x00,                           /* ident = 0 */
    0x00, 0x00, 0x40, 0x11,               /* flags = 0, fragment = 0, TTL = 64, type = UDP */
    0x00, 0x00,                           /* header checksum */
    192, 168, 100, 100,                   /* source IP */
    0xff, 0xff, 0xff, 0xff,               /* dest IP (broadcast) */
    0x44, 0x55, 0x44, 0x55,               /* src/dest UDP ports */
    0x00, 0x00, 0x00, 0x00,               /* length(0), checksum (0) */
};

void
gdpll_channel_monitor_thread(void *unit_vp)
{
    int count = 0;
    uint32 status = 0;
    int result;
    int unit = PTR_TO_INT(unit_vp);

    while (1) {
        sal_usleep(1000*1000);  /* Poll evenry second */
        for (count = 0; count < 8/*BCM_GDPLL_NUM_CHANNELS*/; count++) {
            /* Check enable status of the channel, 0x100 */
            if (pGdpllAppContext && pGdpllAppContext->chan_mon_flag[count])  {
                result = bcm_gdpll_chan_state_get(unit, count, &status);
                if (BCM_FAILURE(result)) {
                    continue;
                }

                if (status != pGdpllAppContext->chan_status[count]) {
                    cli_out("GDPLL DIAG: Channel:%d\n", count);
                    cli_out("    Current  State(%d): 0x%x\n", count, status);
                    cli_out("    Previous State(%d): 0x%x\n", count, pGdpllAppContext->chan_status[count]);
                    pGdpllAppContext->chan_status[count] = status;
                }
            }
        }
    }
}

int gdpll_debug_udp_cb(int unit, void *user_data, uint32 debug_buff, uint32 debug_buf_size)
{
    int rv  = BCM_E_NONE;


    const int         L3_header_len = sizeof(L3_header);
    uint32 len = (debug_buf_size * 4) + L3_header_len + BCM_IEEE_HDR_BYTES + 4; /* Pkt Hdr + L3 Hdr + CRC */
    uint32 rd_addr = debug_buff;
    bcm_pkt_t  *txPkt;
    uint8     *myData;
    uint32   *myPayload;
    int         packet_len;
    int         i;

    BCM_IF_ERROR_RETURN(bcm_pkt_alloc(unit, len, BCM_TX_CRC_REGEN | BCM_TX_ETHER, &txPkt));
    
    BCM_PKT_HDR_SMAC_SET(txPkt, src_mac);
    BCM_PKT_HDR_DMAC_SET(txPkt, dest_mac);
    BCM_PKT_HDR_TPID_SET(txPkt, 0x8100);
    BCM_PKT_HDR_VTAG_CONTROL_SET(txPkt, vlan);

    /* Get a pointer to the payload area of packet */
    if ((BCM_E_NONE ==
         bcm_pkt_byte_index(txPkt, BCM_IEEE_HDR_BYTES, &packet_len, NULL, &myData))
        && (myData != 0)) {
        /* Initialize payload (IPv4/UDP header) */
        for (i = 0; i < L3_header_len; i++) {
            myData[i] = L3_header[i];
        }
        myPayload = (uint32 *)&myData[L3_header_len];
        /* Initialize rest of packet (size less FCS) */
        for (i=0; i<debug_buf_size; i++, rd_addr+=4) {
            myPayload[i] = soc_cm_iproc_read(unit, rd_addr);
        }
    }
    rv = bcm_tx(unit, txPkt, NULL);
    bcm_pkt_free(unit, txPkt);
    return rv;
}

void print_ts(int unit)
{
    uint32 l, u;
    READ_NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr(unit, &u);
    READ_NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr(unit, &l);

    cli_out(" u:0x%x, l:0x%x\n", u, l);
}

STATIC char * bcm_gdpll_chan_erro_str[] = {
    "None",
    "HW,TS-Overload",
    "HW,TS-PPM-violation",
    "HW,TS-Timeout",
    "SW,DPLL-Large-Error",
};

/* Channel status change notification callback */
int bcm_gdpll_cb_notification(int unit, void *user_data, uint32 *cb_data, int cb_type)
{
    int rv  = BCM_E_NONE;
    bcm_gdpll_chan_state_t *pChan_state;

    if (bcmGdpllCbChanState == cb_type) {
        pChan_state = (bcm_gdpll_chan_state_t *)cb_data;
        cli_out("\r [**STATE_CHANGE**]Chan-%d State: En-%d Lock-%d Err-%d(%s)\n", pChan_state->chan,
            (pChan_state->state & BCM_GDPLL_CHAN_STATE_ENABLE) ? 1:0,
            (pChan_state->state & BCM_GDPLL_CHAN_STATE_LOCK) ? 1:0,
            pChan_state->error_code, bcm_gdpll_chan_erro_str[pChan_state->error_code]);
    }

    return rv;
}

/*
 * RSVD1 Callback function for sanity test
 */
static int rsvd1count = 0;
static int bcn_update = 0;
static int ts_update = 0;
static uint64 ts52b;
static int64  gdpll_offset;
static int flag_rsvd1_if_ts;
static int flag_rsvd1_rf_ts;
static uint64_t rsvd1_if_ts;
static uint64_t rsvd1_rf_ts;
static int handle_rsvd1rf, handle_rsvd1if;
static int rsvd1_msg_type;

int _bcm_gdpll_enable_rsvd1_events(int unit, int enable)
{
    int rv = BCM_E_NONE;

    bcm_time_interface_t intf;
    bcm_time_capture_t event_config;

    intf.id = 0;

    event_config.flags = BCM_TIME_CAPTURE_RSVD1RF;
    event_config.ts_counter = 1;
    rv = bcm_time_capture_enable_set (unit, intf.id, &event_config, enable, &handle_rsvd1rf);
    if (BCM_FAILURE(rv)) {
        cli_out("bcm_time_capture_enable_set(RSVD1RF) returned FAIL");
    }

    event_config.flags = BCM_TIME_CAPTURE_RSVD1IF;
    event_config.ts_counter = 1;
    rv = bcm_time_capture_enable_set (unit, intf.id, &event_config, enable, &handle_rsvd1if);
    if (BCM_FAILURE(rv)) {
        cli_out("bcm_time_capture_enable_set(RSVD1IF) returned FAIL");
    }
    return rv;
}

int rsvd1_timestamp_cb(int unit, void *user_data, uint32 *cb_data_ptr, bcm_time_capture_cb_type_t status)
{
    uint32 event_id, upper, lower, cb_data_index=0;
    uint8 *cb_data = (void *)cb_data_ptr;
    uint64_t cur_ts=0;
    int rv = BCM_E_NONE;

    uint16 num_timestamps = (cb_data[cb_data_index+1] << 8) | cb_data[cb_data_index];
    static uint64 N1 = 0ULL;

    cli_out("num_timestamps: %d\n", num_timestamps);

    cb_data_index += 2;

    while(num_timestamps--) {

        event_id = cb_data[cb_data_index]
            | (cb_data[cb_data_index+1] << 8);

        cb_data_index += 2;

        lower = cb_data[cb_data_index]
            | (cb_data[cb_data_index+1]<<8)
            | (cb_data[cb_data_index+2]<<16)
            | (cb_data[cb_data_index+3]<<24);

        cb_data_index += 4;

        upper = cb_data[cb_data_index]
            | (cb_data[cb_data_index+1]<<8)
            | (cb_data[cb_data_index+1]<<16);

        cb_data_index += 3;

        u64_H(cur_ts) = upper;
        u64_L(cur_ts) = lower;
        cli_out(" Event: %d, Timestamp: %x%08x cur_ts[0x%llx] N1[0x%llx]\n", event_id, upper, lower,
                   (long long unsigned int)cur_ts, (long long unsigned int)N1);

        if (event_id == handle_rsvd1if) {
            cli_out(" IF: rsvd1_if_ts[%llx] rsvd1_rf_ts[%llx]\n",
                (long long unsigned int)rsvd1_if_ts, (long long unsigned int)rsvd1_rf_ts);
            bcm_cpri_rsvd1_msg_t rsvd1_msg = {0,0,0};
            rv = bcm_cpri_rsvd1_msg_get(unit, &rsvd1_msg);
            if (BCM_FAILURE(rv)) {
                cli_out("bcm_cpri_rsvd1_msg_get() returned %d FAIL \n", rv);
                return rv;
            }
            cli_out(" RSVD1 status: 0x%x 0x%x 0x%x\n",
                    rsvd1_msg.rsvd1_msg0, rsvd1_msg.rsvd1_msg1, rsvd1_msg.rsvd1_msg2);

            if (rsvd1_msg.rsvd1_msg0 == rsvd1_msg_type) {
                /* Filter for the required msg type that contains N1 */
                u64_H(rsvd1_if_ts) = upper;
                u64_L(rsvd1_if_ts) = lower;

                u64_H(N1) = (rsvd1_msg.rsvd1_msg1 >> 24);
                u64_L(N1) = ((rsvd1_msg.rsvd1_msg2 >> 24) | (rsvd1_msg.rsvd1_msg1 << 8));
                uint64 mask_bit;
                u64_H(mask_bit) = 0;
                u64_L(mask_bit) = 0x1FFFFFF;
                COMPILER_64_AND(N1, mask_bit);
                COMPILER_64_UMUL_32(N1, 10000000);
                u64_H(mask_bit) = 0xFFFF;
                u64_L(mask_bit) = 0xFFFFFFFF;
                COMPILER_64_AND(N1, mask_bit);
                COMPILER_64_SHL(N1, 4);

                flag_rsvd1_if_ts = 1;
            }
        } else if (event_id == handle_rsvd1rf) {
            cli_out(" RF: rsvd1_if_ts[%llx] rsvd1_rf_ts[%llx]\n",
                (long long unsigned int)rsvd1_if_ts, (long long unsigned int)rsvd1_rf_ts);
            if ((flag_rsvd1_if_ts) && (cur_ts > rsvd1_if_ts)) {
                flag_rsvd1_rf_ts = 1;
            }
            u64_H(rsvd1_rf_ts) = upper;
            u64_L(rsvd1_rf_ts) = lower;
        }

        if(flag_rsvd1_rf_ts && flag_rsvd1_if_ts) {
            int64_t t3_t1=0;

            /* disable RSVD1 events */
            rv = _bcm_gdpll_enable_rsvd1_events(unit, 0);
            if (BCM_FAILURE(rv)) {
                cli_out("_bcm_gdpll_enable_rsvd1_events(0) returned %d FAIL \n", rv);
                return rv;
            }

            COMPILER_64_COPY(t3_t1, rsvd1_rf_ts);
            COMPILER_64_SUB_64(t3_t1, rsvd1_if_ts);
            COMPILER_64_COPY(gdpll_offset, t3_t1);

            cli_out(" ts pair : rsvd1_if_ts[%llx] rsvd1_rf_ts[%llx] \
                        \n\t\t\t ts52b[%llx] GdpllOffset[%llx] N1[%llx]\n",
                       (long long unsigned int)rsvd1_if_ts, (long long unsigned int)rsvd1_rf_ts,
                       (long long unsigned int)ts52b, (long long signed int)gdpll_offset,
                       (long long unsigned int)N1);

            /* back-up N1 in ts52b which will be used for initializing phase-counter */
            COMPILER_64_COPY(ts52b, N1);
            _bcm_esw_set_timestamp(unit, 1, ts52b);
            ts_update = 0;
        }

    }
    return BCM_E_NONE;
}

int bcm_gdpll_cb_rsvd1(int unit, void *user_data, uint32 *cb_data, int cb_type)
{
    int rv  = BCM_E_NONE;

    /* cli_out(" ######## bcm_gdpll_cb_rsvd1\n"); */

    /* Following if statement is to sanity test the RSVD1 intf */
    if (rsvd1count) {
        rsvd1count--;
        cli_out(" RSVD1 stat0:0x%x\n", *cb_data); cb_data++;
        cli_out(" RSVD1 stat1:0x%x\n", *cb_data); cb_data++;
        cli_out(" RSVD1 stat2:0x%x\n", *cb_data);

        if (rsvd1count == 0) {
            rv = bcm_gdpll_cb_unregister(unit, bcmGdpllCbRsvd1);
            if (BCM_FAILURE(rv)) {
                return CMD_FAIL;
            }
        }
    }

    if (bcn_update || ts_update) {
        uint32 /*rsvd1_stat0,*/ rsvd1_stat1, rsvd1_stat2;
        uint64 N1;
        /*uint32 N2;*/

        /*rsvd1_stat0 = *cb_data; */
        cb_data++;

        rsvd1_stat1 = *cb_data;
        cb_data++;

        rsvd1_stat2 = *cb_data;
        /* cli_out(" RP1 stat1:0x%x\n", rsvd1_stat1); */
        /* cli_out(" RP1 stat2:0x%x\n", rsvd1_stat2); */

        /*
         * 1. Read the timecode and generate the 52-bit ref-TS using
         *    "((1e9*N + M) mod 2^48) << 4"
         * 2. Feed this Timecode directly to Input array location
         */
        /*N2 = rsvd1_stat2 & 0xFFFFFF; Lower 24 bits */
        u64_H(N1) = rsvd1_stat1 >> 24;
        u64_L(N1) = rsvd1_stat2 >> 24 | (rsvd1_stat1<<8);

        if (bcn_update) {
            
            soc_iproc_setreg(unit, 0x0326e03c/*soc_reg_addr(unit, BRCM_RESERVED_IPROC_19, REG_PORT_ANY, 0)*/, u64_H(N1));
            soc_iproc_setreg(unit, 0x0326e038/*soc_reg_addr(unit, BRCM_RESERVED_IPROC_18, REG_PORT_ANY, 0)*/, u64_L(N1));
            soc_iproc_setreg(unit, 0x0326e034/*soc_reg_addr(unit, BRCM_RESERVED_IPROC_17, REG_PORT_ANY, 0)*/, 0);
            soc_iproc_setreg(unit, 0x0326e030/*soc_reg_addr(unit, BRCM_RESERVED_IPROC_16, REG_PORT_ANY, 0)*/, 0);

            /* WRITE_BRCM_RESERVED_IPROC_19r(unit, u64_H(N1));
               WRITE_BRCM_RESERVED_IPROC_18r(unit, u64_L(N1));
               WRITE_BRCM_RESERVED_IPROC_17r(unit, 0);
               WRITE_BRCM_RESERVED_IPROC_16r(unit, 0); */

            bcn_update = 0;
            cli_out("### BCN counter updated with first message\n");
        }

        if (ts_update) {
            uint64 mask_bit;

            /* cli_out("TS Before: "); print_ts(unit); */

            /* ts52b = (N1 & 0x1FFFFFF)*10000000;  //+ 0.59604648328 * N2; */
            u64_H(mask_bit) = 0;
            u64_L(mask_bit) = 0x1FFFFFF;
            COMPILER_64_AND(N1, mask_bit);
            COMPILER_64_UMUL_32(N1, 10000000);

            /* ts52b = ts52b & 0xFFFFFFFFFFFF; */
            u64_H(mask_bit) = 0xFFFF;
            u64_L(mask_bit) = 0xFFFFFFFF;
            COMPILER_64_AND(N1, mask_bit);

            /* ts52b = ts52b << 4; */
            COMPILER_64_SHL(N1, 4);

            /* Copy the timestamper for phase_counter update */
            COMPILER_64_COPY(ts52b, N1);

            /* Update the timestamper */
            
            _bcm_esw_set_timestamp(unit, 1, ts52b);

            ts_update = 0;
            cli_out("### TS counter updated with first message TS: u:0x%x l:0x%x\n", u64_H(ts52b), u64_L(ts52b));
        }

        /* Un-register the callback */
        rv = bcm_gdpll_cb_unregister(unit, bcmGdpllCbRsvd1);
        if (BCM_FAILURE(rv)) {
            return CMD_FAIL;
        }
    }

    /* cli_out("TS: "); print_ts(unit); */
    return rv;
}

uint32 gdpll_config(bcm_gdpll_chan_t *pGdpllChan, char *pStr, char *pNumStr)
{
    uint32 num;

    if (!isint(pNumStr)) {
        cli_out("### gdpll_config: Error: config number for str:%s\n", pStr);
        return -1;
    }
    num = parse_integer(pNumStr);

    /* Reference */
    if (strcmp(pStr, "event_ref_input_event") == 0)
            pGdpllChan->event_ref.input_event = num;
    else if (strcmp(pStr, "event_ref_port") == 0)
            pGdpllChan->event_ref.port = num;
    else if (strcmp(pStr, "event_ref_port_event_type") == 0)
            pGdpllChan->event_ref.port_event_type = num;
     else if (strcmp(pStr, "event_ref_event_divisor") == 0)
            pGdpllChan->event_ref.event_divisor = num;
     else if (strcmp(pStr, "event_ref_event_dest") == 0)
            pGdpllChan->event_ref.event_dest = num;
    else if (strcmp(pStr, "event_ref_ts_counter") == 0)
            pGdpllChan->event_ref.ts_counter = num;
    else if (strcmp(pStr, "event_ref_ppm_check_enable") == 0)
            pGdpllChan->event_ref.ppm_check_enable = num;

    /* Feedback */
    else if (strcmp(pStr, "event_fb_input_event") == 0)
        pGdpllChan->event_fb.input_event = num;
    else if (strcmp(pStr, "event_fb_port") == 0)
        pGdpllChan->event_fb.port = num;
    else if (strcmp(pStr, "event_fb_port_event_type") == 0)
        pGdpllChan->event_fb.port_event_type = num;
    else if (strcmp(pStr, "event_fb_event_divisor") == 0)
        pGdpllChan->event_fb.event_divisor = num;
    else if (strcmp(pStr, "event_fb_event_dest") == 0)
        pGdpllChan->event_fb.event_dest = num;
    else if (strcmp(pStr, "event_fb_ts_counter") == 0)
        pGdpllChan->event_fb.ts_counter = num;
    else if (strcmp(pStr, "event_fb_ppm_check_enable") == 0)
        pGdpllChan->event_fb.ppm_check_enable = num;

    /* Channel specifics */
    else if (strcmp(pStr, "chan_prio") == 0)
        pGdpllChan->chan_prio = num;
    else if (strcmp(pStr, "out_event") == 0)
        pGdpllChan->out_event = num;
    else if (strcmp(pStr, "out_port") == 0)
        pGdpllChan->port = num;
    else if (strcmp(pStr, "ts_pair_dest") == 0)
        pGdpllChan->ts_pair_dest = num;

    /* DPLL config */
    else if (strcmp(pStr, "dpll_config_k1[0]") == 0)
        pGdpllChan->chan_dpll_config.k1[0] = num;
    else if (strcmp(pStr, "dpll_config_k1[1]") == 0)
        pGdpllChan->chan_dpll_config.k1[1] = num;
    else if (strcmp(pStr, "dpll_config_k1[2]") == 0)
        pGdpllChan->chan_dpll_config.k1[2] = num;

    else if (strcmp(pStr, "dpll_config_k1Shift[0]") == 0)
        pGdpllChan->chan_dpll_config.k1Shift[0] = num;
    else if (strcmp(pStr, "dpll_config_k1Shift[1]") == 0)
        pGdpllChan->chan_dpll_config.k1Shift[1] = num;
    else if (strcmp(pStr, "dpll_config_k1Shift[2]") == 0)
        pGdpllChan->chan_dpll_config.k1Shift[2] = num;

    else if (strcmp(pStr, "dpll_config_k1k2[0]") == 0)
        pGdpllChan->chan_dpll_config.k1k2[0] = num;
    else if (strcmp(pStr, "dpll_config_k1k2[1]") == 0)
        pGdpllChan->chan_dpll_config.k1k2[1] = num;
    else if (strcmp(pStr, "dpll_config_k1k2[2]") == 0)
        pGdpllChan->chan_dpll_config.k1k2[2] = num;

    else if (strcmp(pStr, "dpll_config_k1k2Shift[0]") == 0)
        pGdpllChan->chan_dpll_config.k1k2Shift[0] = num;
    else if (strcmp(pStr, "dpll_config_k1k2Shift[1]") == 0)
        pGdpllChan->chan_dpll_config.k1k2Shift[1] = num;
    else if (strcmp(pStr, "dpll_config_k1k2Shift[2]") == 0)
        pGdpllChan->chan_dpll_config.k1k2Shift[2] = num;

    else if (strcmp(pStr, "dpll_config_lockDetThres[0]") == 0)
        pGdpllChan->chan_dpll_config.lockDetThres[0] = num;
    else if (strcmp(pStr, "dpll_config_lockDetThres[1]") == 0)
        pGdpllChan->chan_dpll_config.lockDetThres[1] = num;
    else if (strcmp(pStr, "dpll_config_lockDetThres[2]") == 0)
        pGdpllChan->chan_dpll_config.lockDetThres[2] = num;

    else if (strcmp(pStr, "dpll_config_lockIndicatorThresholdLo") == 0)
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo = num;
    else if (strcmp(pStr, "dpll_config_lockIndicatorThresholdHi") == 0)
        pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi = num;

    else if (strcmp(pStr, "dpll_config_dwell_count[0]") == 0)
        pGdpllChan->chan_dpll_config.dwell_count[0] = num;
    else if (strcmp(pStr, "dpll_config_dwell_count[1]") == 0)
        pGdpllChan->chan_dpll_config.dwell_count[1] = num;
    else if (strcmp(pStr, "dpll_config_dwell_count[2]") == 0)
        pGdpllChan->chan_dpll_config.dwell_count[2] = num;

    else if (strcmp(pStr, "dpll_config_phase_error_limiter_thres_H[0]") == 0)
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = num;
    else if (strcmp(pStr, "dpll_config_phase_error_limiter_thres_L[0]") == 0)
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = num;
    else if (strcmp(pStr, "dpll_config_phase_error_limiter_thres_H[1]") == 0)
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = num;
    else if (strcmp(pStr, "dpll_config_phase_error_limiter_thres_L[1]") == 0)
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = num;
    else if (strcmp(pStr, "dpll_config_phase_error_limiter_thres_H[2]") == 0)
        u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = num;
    else if (strcmp(pStr, "dpll_config_phase_error_limiter_thres_L[2]") == 0)
        u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = num;

    else if (strcmp(pStr, "dpll_config_nco_bits") == 0)
        pGdpllChan->chan_dpll_config.nco_bits = num;
    else if (strcmp(pStr, "dpll_config_phase_counter_ref") == 0)
        pGdpllChan->chan_dpll_config.phase_counter_ref = num;
    else if (strcmp(pStr, "dpll_config_dpll_num_states") == 0)
        pGdpllChan->chan_dpll_config.dpll_num_states = num;

    else if (strcmp(pStr, "dpll_config_idump_mod[0]") == 0)
        pGdpllChan->chan_dpll_config.idump_mod[0] = num;
    else if (strcmp(pStr, "dpll_config_idump_mod[1]") == 0)
        pGdpllChan->chan_dpll_config.idump_mod[1] = num;
    else if (strcmp(pStr, "dpll_config_idump_mod[2]") == 0)
        pGdpllChan->chan_dpll_config.idump_mod[2] = num;

    else if (strcmp(pStr, "dpll_config_phase_error_shift") == 0)
        pGdpllChan->chan_dpll_config.phase_error_shift = num;
    else if (strcmp(pStr, "dpll_config_nominal_loop_filter_H") == 0)
        u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter) = num;
    else if (strcmp(pStr, "dpll_config_nominal_loop_filter_L") == 0)
        u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter) = num;
    else if (strcmp(pStr, "dpll_config_invert_phase_error") == 0)
        pGdpllChan->chan_dpll_config.invert_phase_error = num;
    else if (strcmp(pStr, "dpll_config_norm_phase_error") == 0)
        pGdpllChan->chan_dpll_config.norm_phase_error = num;
    else if (strcmp(pStr, "dpll_config_norm_phase_thres0") == 0)
        pGdpllChan->chan_dpll_config.phase_error_thres0 = num;
    else if (strcmp(pStr, "dpll_config_norm_phase_thres1") == 0)
        pGdpllChan->chan_dpll_config.phase_error_thres1 = num;
    else if (strcmp(pStr, "holdover_filter_coeff") == 0)
        pGdpllChan->chan_dpll_config.holdover_filter_coeff = num;

    /* DPLL State */
    else if (strcmp(pStr, "dpll_state_dpll_state") == 0)
        pGdpllChan->chan_dpll_state.dpll_state = num;
    else if (strcmp(pStr, "dpll_state_loop_filter_H") == 0)
        u64_H(pGdpllChan->chan_dpll_state.loop_filter) = num;
    else if (strcmp(pStr, "dpll_state_loop_filter_L") == 0)
        u64_L(pGdpllChan->chan_dpll_state.loop_filter) = num;
    else if (strcmp(pStr, "dpll_state_dwell_counter") == 0)
        pGdpllChan->chan_dpll_state.dwell_counter = num;
    else if (strcmp(pStr, "dpll_state_lockDetFilter") == 0)
        pGdpllChan->chan_dpll_state.lockDetFilter = num;
    else if (strcmp(pStr, "dpll_state_offset_H") == 0)
        u64_H(pGdpllChan->chan_dpll_state.offset) = num;
    else if (strcmp(pStr, "dpll_state_offset_L") == 0)
        u64_L(pGdpllChan->chan_dpll_state.offset) = num;
    else if (strcmp(pStr, "dpll_state_init_flag") == 0)
        pGdpllChan->chan_dpll_state.init_flag = num;
    else if (strcmp(pStr, "dpll_state_init_offset_flag") == 0)
        pGdpllChan->chan_dpll_state.init_offset_flag = num;
    else if (strcmp(pStr, "dpll_state_phase_counter_H") == 0)
        u64_H(pGdpllChan->chan_dpll_state.phase_counter) = num;
    else if (strcmp(pStr, "dpll_state_phase_counter_L") == 0)
        u64_L(pGdpllChan->chan_dpll_state.phase_counter) = num;
    else if (strcmp(pStr, "dpll_state_phaseCounterDelta_H") == 0)
        u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta) = num;
    else if (strcmp(pStr, "dpll_state_phaseCounterDelta_L") == 0)
        u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta) = num;
    else if (strcmp(pStr, "dpll_state_phaseCounterN") == 0)
        pGdpllChan->chan_dpll_state.phaseCounterN = num;
    else if (strcmp(pStr, "dpll_state_phaseCounterM") == 0)
        pGdpllChan->chan_dpll_state.phaseCounterM = num;
    else if (strcmp(pStr, "dpll_state_phaseCounterFrac") == 0)
        pGdpllChan->chan_dpll_state.phaseCounterFrac = num;
    else if (strcmp(pStr, "dpll_state_idumpCounter") == 0)
        pGdpllChan->chan_dpll_state.idumpCounter = num;
    else if (strcmp(pStr, "dpll_state_accumPhaseError_H") == 0)
        u64_H(pGdpllChan->chan_dpll_state.accumPhaseError) = num;
    else if (strcmp(pStr, "dpll_state_accumPhaseError_L") == 0)
        u64_L(pGdpllChan->chan_dpll_state.accumPhaseError) = num;
    else if (strcmp(pStr, "use_case_flag") == 0)
        use_case_flag = num;
    else if (strcmp(pStr, "disable_debug_log") == 0)
        disable_debug_log = num;
    else
        cli_out("### gdpll_config: ERROR: config str:%s\n", pStr);

    return 0;
}

static int
_bcm_future_cpri_enable(int unit, int port, uint64 offset)
{
    int rv  = BCM_E_NONE;

#ifdef CPRIMOD_SUPPORT
    rv = cprimod_tx_framer_tgen_enable(unit, port, 0/*enable*/);
    if (BCM_FAILURE(rv)) {
        cli_out("CPRI TGEN_ENABLE setting 0 failed %s\n", bcm_errmsg(rv));
        return CMD_FAIL;
    }

    rv = cprimod_tx_framer_tgen_offset_set(unit, port, offset);
    if (BCM_FAILURE(rv)) {
        cli_out("CPRI TGEN_OFFSET setting failed %s\n", bcm_errmsg(rv));
        return CMD_FAIL;
    }

    rv = cprimod_tx_framer_tgen_enable(unit, port, 1/*enable*/);
    if (BCM_FAILURE(rv)) {
        cli_out("CPRI TGEN_ENABLE setting 1 failed %s\n", bcm_errmsg(rv));
        return CMD_FAIL;
    }
#endif

    return rv;
}

#define ToD_TYPE_NTP    0x1
#define ToD_TYPE_PTP    0x2

static int
_bcm_program_ptpntptod(int unit, bcm_time_spec_t ts, uint32 tod_type)
{
    int rv  = BCM_E_NONE;
    uint32 regval;
    uint32 regval_sec;
    uint64 frac_sec;
    uint64 nanosec, div;
    uint64 val_64b;

    /*  regval_sec = new_time.u48_sec; */
    regval_sec = COMPILER_64_LO(ts.seconds);
    regval_sec = ((regval_sec & 1) << 31) | (regval_sec >> 1);

    if (tod_type & ToD_TYPE_NTP) {

        /* Program the initial NTP-ToD frequency control */
        WRITE_NTP_TIME_FREQ_CONTROLr(unit, 0x44B82FA1);

        /* NTP timestamps are 64 bit, Q32 seconds, i.e seconds * 2^32, so bit 0 == 1/2^32 of a second */
        /* Convert from nanoseconds to those fractions of a second */
        /*  frac_sec = (((uint64_t)new_time.u32_Nsec) << 32) / ONE_BILLION; */
        COMPILER_64_SET(div, 0, 1000000000);    /* ONE_BILLION */
        COMPILER_64_SET(nanosec, 0, ts.nanoseconds);
        COMPILER_64_SHL(nanosec, 32);
        COMPILER_64_UDIV_64(nanosec, div);
        COMPILER_64_COPY(frac_sec, nanosec);

        /* NTP time is stored in seconds and fractional seconds */
        WRITE_NTP_TIME_SECr(unit, regval_sec);

        COMPILER_64_COPY(val_64b, frac_sec);
        COMPILER_64_SHR(val_64b, 6);
        WRITE_NTP_TIME_FRAC_SEC_UPPERr(unit, COMPILER_64_LO(val_64b));

        COMPILER_64_COPY(val_64b, frac_sec);
        COMPILER_64_SHL(val_64b, 26);
        WRITE_NTP_TIME_FRAC_SEC_LOWERr(unit, COMPILER_64_LO(val_64b));

        READ_NTP_TIME_CONTROLr(unit, &regval);
        regval |= 0x1f;    /* Set load-enable bits, |B04...B00|. */

        WRITE_NTP_TIME_CONTROLr(unit, regval);

        regval &= ~(0x1f); /* Clear load-enable bits, |B04...B00|. */
        regval |= 0x20;    /* Set count-enable bit, |B05|. */
        WRITE_NTP_TIME_CONTROLr(unit, regval);
        /* cli_out("### TS After NTP write: "); print_ts(unit); */
    }

    if (tod_type & ToD_TYPE_PTP) {

        /* Program the initial PTP-ToD frequency control */
        WRITE_IEEE1588_TIME_FREQ_CONTROLr(unit, 0x10000000);

        /* 1588 time is stored in seconds and nanoseconds */
        WRITE_IEEE1588_TIME_SECr(unit, regval_sec);
        WRITE_IEEE1588_TIME_FRAC_SEC_UPPERr(unit, ts.nanoseconds >> 6);
        WRITE_IEEE1588_TIME_FRAC_SEC_LOWERr(unit, ts.nanoseconds << 26);

        READ_IEEE1588_TIME_CONTROLr(unit, &regval);
        regval |= 0x1f;    /* Set load-enable bits, |B04...B00|. */
        WRITE_IEEE1588_TIME_CONTROLr(unit, regval);
        regval &= ~(0x1f); /* Clear load-enable bits, |B04...B00|. */
        regval |= 0x20;    /* Set count-enable bit, |B05|. */
        WRITE_IEEE1588_TIME_CONTROLr(unit, regval);
        /* cli_out("### TS After PTP write: "); print_ts(unit); */
    }

    return rv;
}

/* Enable this macro rx.c DO-NOT have the timestamp conversion logic implemented */
/* #define RX_TS_CONVERTION */

STATIC bcm_rx_t
roe_pkt_cb_handler(int unit, bcm_pkt_t *info, void *cookie)
{
    uint64 T2 = 0;  /* RX packet timestamp */
    uint64 C = 0;   /* For CPRI-TX gate opening */
    uint32 delta = 3000 * 16; /* 3uSec jitter buffer, ns */
    uint32 future_offset = 10 * 1000000 * 16; /* 20mSec, ns */

#ifdef RX_TS_CONVERTION
    uint64 T1 = 0;  /* Current timestamper value */
    int timestamper = 1;    
    uint32 val32;

    u64_L(T2) = info->rx_timestamp;

    _bcm_esw_get_timestamp(unit, timestamper, &T1);

    if (COMPILER_64_BITTEST(T1, 35) == COMPILER_64_BITTEST(T2, 31)) {
        /* If T1[35] == T2[31], then set T2[47..32] = T1[51..36],  T2 = T2 << 4 */
        val32 = u64_H(T1);
        val32  = val32 >> 4;
        val32 &= 0xFFFF;

        u64_H(T2) &= ~0xFFFF;
        u64_H(T2) |= val32;
        COMPILER_64_SHL(T2, 4);
    } else if ((COMPILER_64_BITTEST(T1, 35) == 1) && (COMPILER_64_BITTEST(T2, 31) == 0)) {
        /* If T1[35] == 1 and T2[31] == 0, then set T2[47..32] = T1[51..36],  T2 = T2 << 4 */
        val32 = u64_H(T1);
        val32  = val32 >> 4;
        val32 &= 0xFFFF;

        u64_H(T2) &= ~0xFFFF;
        u64_H(T2) |= val32;
        COMPILER_64_SHL(T2, 4);
    } else if ((COMPILER_64_BITTEST(T1, 35) == 0) && (COMPILER_64_BITTEST(T2, 31) == 1)) {
        /* If T1[35] == 0 and T2[31] == 1, then set T2[47..32] = T1[51..36] - 1,  T2= T2 << 4  */
        val32  = u64_H(T1);
        val32  = val32 >> 4;
        val32 -= 1;
        val32 &= 0xFFFF;

        u64_H(T2) &= ~0xFFFF;
        u64_H(T2) |= val32;
        COMPILER_64_SHL(T2, 4);
    }
#else
    u64_L(T2) = info->rx_timestamp;
    u64_H(T2) = info->rx_timestamp_upper;
#endif

    /* C = T2 + 160,000,000 + Delta */
    COMPILER_64_ADD_32(C, T2);
    COMPILER_64_ADD_32(C, future_offset);
    COMPILER_64_ADD_32(C, delta);

    _bcm_future_cpri_enable(unit, port_out, C);

#ifdef RX_TS_CONVERTION
    cli_out(" ### \n T1(H/L):0x%x/0x%x \n T2(32b):0x%x \n T2_computed(H/L): 0x%x/0x%x \n future_time(H/L): 0x%x/0x%x\n\n",
        u64_H(T1), u64_L(T1), info->rx_timestamp, u64_H(T2), u64_L(T2), u64_H(C), u64_L(C));
#else
    cli_out(" ### \n T2(pkt-ts):0x%x/0x%x \n future_time(H/L): 0x%x/0x%x\n\n",
        info->rx_timestamp_upper, info->rx_timestamp, u64_H(C), u64_L(C));
#endif

    /* Un-register the cb handler */
    bcm_rx_unregister(unit, roe_pkt_cb_handler, 101);
    return BCM_RX_HANDLED;
}

void chan_debug_dump(int unit, int chan)
{
    dpll_dump_t chan_dump;
    uint64 diff1, diff2;

    /* Get the channel dump */
    chan_dump.chan = chan;
    if (BCM_E_NONE != _bcm_esw_gdpll_debug_dump(unit, &chan_dump)) {
        cli_out("***** TIMEOUT: M7 is NOT Getting triggered for chan-%d *****\n", chan);
    } else {
        COMPILER_64_COPY(diff1, chan_dump.debug_dump[1].feedback);
        COMPILER_64_SUB_64(diff1, chan_dump.debug_dump[0].feedback);

        COMPILER_64_COPY(diff2, chan_dump.debug_dump[1].reference);
        COMPILER_64_SUB_64(diff2, chan_dump.debug_dump[0].reference);

        cli_out("Ch# Feedback[FB]   Refer[REF]     PhaseError        (b2b FB-Diff)    (b2b REF-Diff)\n");
        cli_out("%3d %05x_%08x %05x_%08x %08x_%08x\n", chan_dump.chan,
                 u64_H(chan_dump.debug_dump[0].feedback), u64_L(chan_dump.debug_dump[0].feedback),
                 u64_H(chan_dump.debug_dump[0].reference), u64_L(chan_dump.debug_dump[0].reference),
                 u64_H(chan_dump.debug_dump[0].perror), u64_L(chan_dump.debug_dump[0].perror));
        cli_out("%3d %05x_%08x %05x_%08x %08x_%08x (%05x_%08x) (%05x_%08x)\n", chan_dump.chan,
                 u64_H(chan_dump.debug_dump[1].feedback), u64_L(chan_dump.debug_dump[1].feedback),
                 u64_H(chan_dump.debug_dump[1].reference), u64_L(chan_dump.debug_dump[1].reference),
                 u64_H(chan_dump.debug_dump[1].perror), u64_L(chan_dump.debug_dump[1].perror),
                 u64_H(diff1), u64_L(diff1), u64_H(diff2), u64_L(diff2));
    }
}

int chan_status_get(int unit, int chan)
{
    int result;
    uint32 status = 0;
    int error_code;

    result = bcm_gdpll_chan_state_get(unit, chan, &status);
    if (BCM_FAILURE(result)) {
        return result;
    }

    if (status & 0x80000000) {
        error_code=3;
    } else if (status & 0x40000000) {
        error_code=4;
    } else {
        error_code=0;
    }

    cli_out(" Status   (%3d): 0x%x  [En-%d Lock-%d Err-%d(%s)]\n",
            chan, status, status&0x100?1:0, status&0x1?1:0,
            error_code, bcm_gdpll_chan_erro_str[error_code]);
    return result;
}

cmd_result_t cmd_gdpll(int unit, args_t *args)
{
    char *arg_string_p = NULL;
    int result = BCM_E_NONE;
    char *c, *filename;
    int chan;
    int enable;
    char line[300];
    int i;
    bcm_gdpll_chan_t    gdpll_chan;
    uint32 poll_period_uSec = 30*60*1000000;   /* Poll for 30mins */
    uint32 rval;
    uint32 poll_interval_uSec = 0;
    uint32 pps_detect = 0;
    soc_field_t field = 0;
    uint32 alloc_sz = 0;
    sal_thread_t thread_id;
    uint32 regVal1;
    uint32 field1, field2, /*field3,*/ field4;

    
    uint32 flags = BCM_GDPLL_CONF_DPLL |
                           BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                           BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                           BCM_GDPLL_CHAN_OUTPUT_CONF;
    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));

    arg_string_p = ARG_GET(args);

    if (arg_string_p == NULL) {
        return CMD_USAGE;
    }

    if (!sh_check_attached(ARG_CMD(args), unit)) {
        return CMD_FAIL;
    }

    if (pGdpllAppContext == NULL) {
        alloc_sz = sizeof(gdpll_app_context_t);
        pGdpllAppContext = sal_alloc(alloc_sz, "GDPLL module");
        if (NULL != pGdpllAppContext) {
            sal_memset(pGdpllAppContext, 0, alloc_sz);
            thread_id = sal_thread_create("GDPLL Monitor", SAL_THREAD_STKSZ, 100,
                            gdpll_channel_monitor_thread, INT_TO_PTR(unit));

            if (thread_id == SAL_THREAD_ERROR) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "GDPLL DIAG: Channel monitor thread create failed\n")));
                sal_free(pGdpllAppContext);
                pGdpllAppContext = NULL;
            }
        }
    }

    if (parse_cmp("Flush", arg_string_p, 0)) {
        result = bcm_gdpll_flush(unit);
        if (BCM_FAILURE(result)) {
            cli_out("Command failed. %s\n", bcm_errmsg(result));
            return CMD_FAIL;
        }
    } else if (parse_cmp("Rsvd1cbreg", arg_string_p, 0)) {
        rsvd1count = 2;
        result = bcm_gdpll_cb_register(unit, bcmGdpllCbRsvd1, bcm_gdpll_cb_rsvd1, NULL);
        if (BCM_FAILURE(result)) {
            return CMD_FAIL;
        }
    } else if (parse_cmp("Rsvd1cbunreg", arg_string_p, 0)) {
        result = bcm_gdpll_cb_unregister(unit, bcmGdpllCbRsvd1);
        if (BCM_FAILURE(result)) {
            return CMD_FAIL;
        }
    } else if (parse_cmp("Debug", arg_string_p, 0)) {
        c = ARG_GET(args);
        if (!isint(c)) {
            cli_out("%s: Error: Channel number \n", ARG_CMD(args));
            return(CMD_USAGE);
        }
        enable = parse_integer(c);

        if (!enable) {
            /* Disable GDPLL debug feature so that there wont be the callback */
            result = bcm_gdpll_debug (unit, 0);
            if (BCM_FAILURE(result)) {
                cli_out("Command failed. %s\n", bcm_errmsg(result));
                return CMD_FAIL;
            }

            /* Introduce delay for the pending wrights */
            sal_usleep(100 * 1000);

            /* Close the file pointer */
            if (fp) {
#ifndef NO_FILEIO
                sal_fclose((FILE *)fp);
                cli_out("### gdpll debug file close\n");
#endif
                fp = NULL;
            }

            debug_enable_flag = 0;
            return CMD_OK;
        }

        if (enable && debug_enable_flag) {
            /* If already enabled, user needs to disable before re-enabling */
            cli_out("Already enabled.. \n");
            return CMD_FAIL;
        }

        /* Read the user file name */
        if (enable) {
            c = ARG_GET(args);
            filename = c;
            if (filename == NULL) {
                cli_out("%s: Error: No file specified\n", ARG_CMD(args));
                return(CMD_USAGE);
            }

            if (strcmp(filename, "UDP") == 0) {
                cli_out("Sending out as UDP packet\n");

                int ttl = L3_header[10];
                bcm_ip_t src_ip = soc_ntohl_load(&L3_header[14]);
                bcm_ip_t dest_ip = soc_ntohl_load(&L3_header[18]);
                int udp_src_port = soc_ntohs_load(&L3_header[22]);
                int udp_dst_port = soc_ntohs_load(&L3_header[24]);

                parse_table_t pt;
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "DestMAC", PQ_DFL | PQ_MAC, NULL, dest_mac, NULL);
                parse_table_add(&pt, "DestAddr", PQ_DFL | PQ_IP, NULL, &dest_ip, NULL);
                parse_table_add(&pt, "SrcMAC", PQ_DFL | PQ_MAC, NULL, src_mac, NULL);
                parse_table_add(&pt, "SrcAddr", PQ_DFL | PQ_IP, NULL, &src_ip, NULL);
                parse_table_add(&pt, "UDPSrcPort", PQ_DFL | PQ_INT, NULL, &udp_src_port, NULL);
                parse_table_add(&pt, "UDPDestPort", PQ_DFL | PQ_INT, NULL, &udp_dst_port, NULL);
                parse_table_add(&pt, "Vlan", PQ_DFL | PQ_INT, NULL, &vlan, NULL);
                parse_table_add(&pt, "TTL", PQ_DFL | PQ_INT, NULL, &ttl, NULL);
                if (parse_arg_eq(args, &pt) < 0) {
                    cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(args), ARG_CUR(args));
                    parse_arg_eq_done(&pt);
                    return(CMD_FAIL);
                }
                parse_arg_eq_done(&pt);

                L3_header[10] = ttl & 0xff;
                soc_htonl_store(&L3_header[14], src_ip);
                soc_htonl_store(&L3_header[18], dest_ip);
                soc_htons_store(&L3_header[22], udp_src_port);
                soc_htons_store(&L3_header[24], udp_dst_port);

                /* Register the callback function */
                result = bcm_gdpll_debug_cb_register (unit, NULL, gdpll_debug_udp_cb);
                if (BCM_FAILURE(result)) {
                    cli_out("Command failed. %s\n", bcm_errmsg(result));
                    return CMD_FAIL;
                }
            }
#ifndef NO_FILEIO
            else {
                /* Open the file */
                fp = sal_fopen(filename, "w+");
                if (!fp) {
                    cli_out("%s: Error: Unable to open file: %s\n",
                                           ARG_CMD(args), filename);
                    return CMD_FAIL;
                }
                /* Register the callback function */
                result = bcm_gdpll_debug_cb_register (unit, NULL, gdpll_debug_cb);
                if (BCM_FAILURE(result)) {
                    cli_out("Command failed. %s\n", bcm_errmsg(result));
                    return CMD_FAIL;
                }
            }
#endif
        }

        /* Set enable for GDPLL debug feature */
        result = bcm_gdpll_debug (unit, enable);
        if (BCM_FAILURE(result)) {
            cli_out("Command failed. %s\n", bcm_errmsg(result));
            return CMD_FAIL;
        }

        debug_enable_flag = enable ? 1:0;

    } else if (parse_cmp("Offset", arg_string_p, 0)) {
        arg_string_p = ARG_GET(args);
        if (arg_string_p == NULL) {
            return CMD_USAGE;
        }

        if (parse_cmp("Set", arg_string_p, 0)) {
            uint64 Offset;
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            chan = parse_integer(c);

            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Offset_H \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            u64_H(Offset) = parse_integer(c);

            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Offset_L \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            u64_L(Offset) = parse_integer(c);

            bcm_gdpll_offset_set(unit, chan, Offset);

        } else if (parse_cmp("Get", arg_string_p, 0)) {
            uint64 Offset;
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            chan = parse_integer(c);

            bcm_gdpll_offset_get(unit, chan, &Offset);
            cli_out("### GDPLL DIAG: chan:%d offset(H/L):0x%x/0x%x\n", chan, u64_H(Offset), u64_L(Offset));

        } else {
            cli_out("Error: Invalid Option for Offset\n");
            return(CMD_USAGE);
        }
    } else if (parse_cmp("Chan", arg_string_p, 0)) {
        arg_string_p = ARG_GET(args);
        if (arg_string_p == NULL) {
            return CMD_USAGE;
        }

        if (parse_cmp("Destroy", arg_string_p, 0)) {
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            chan = parse_integer(c);

            result = bcm_gdpll_chan_destroy(unit, chan);
            if (BCM_FAILURE(result)) {
                cli_out("Channel destroy failed with error: %s\n", bcm_errmsg(result));
                return CMD_FAIL;
            }
        } else if (parse_cmp("Sanity", arg_string_p, 0)) {
            dpll_dump_t chan_dump;
            uint64 diff1;

            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            chan = parse_integer(c);
            if (chan >= BCM_GDPLL_NUM_CHANNELS || chan < 0) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }

            chan_dump.chan = chan;

            /* Get ref, fb, phaseCnt fields */
            READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal1);
            field1 = soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal1, ENABLEf);
            field2 = soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal1, REFERENCE_IDf);
            /*field3 = soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal1, FEEDBACK_IDf);*/
            field4 = soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal1, PHASECOUNTERREFf);

            /* Check if channel is already enabled */
            if (field1) {
                cli_out("Error: Channel is already enabled\n");
                return CMD_FAIL;
            }

            /* Set open loop and reset at the end */
            bcm_gdpll_chan_debug_enable(unit, chan, 2/*bcmGdpllDebugMode2*/, 1);

            /* Set phase counter-ref for M7 to get triggers */
            soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal1, PHASECOUNTERREFf, 1);
            WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal1);

            /* Sanity check FB */
            bcm_gdpll_chan_enable(unit, chan, 1);
            cli_out("########### Feedback Event ##############\n");
            if (BCM_E_NONE != _bcm_esw_gdpll_debug_dump(unit, &chan_dump)) {
                cli_out("ERROR !! FB Events are not happening\n");
            } else {
                COMPILER_64_COPY(diff1, chan_dump.debug_dump[1].feedback);
                COMPILER_64_SUB_64(diff1, chan_dump.debug_dump[0].feedback);

                cli_out("Ch# FB-TS:         (Diff)\n");
                cli_out("%3d %05x_%08x\n", chan_dump.chan,
                    u64_H(chan_dump.debug_dump[0].feedback), u64_L(chan_dump.debug_dump[0].feedback));
                cli_out("%3d %05x_%08x (%05x_%08x)\n", chan_dump.chan,
                    u64_H(chan_dump.debug_dump[1].feedback), u64_L(chan_dump.debug_dump[1].feedback),
                    u64_H(diff1), u64_L(diff1));
            }
            cli_out("#########################\n");
            bcm_gdpll_chan_enable(unit, chan, 0);

            /* Sanity check REF */
            if (!field4) {
                soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal1, FEEDBACK_IDf, field2);
                WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal1);

                bcm_gdpll_chan_enable(unit, chan, 1);
                cli_out("########### Reference Event ##############\n");
                if (BCM_E_NONE != _bcm_esw_gdpll_debug_dump(unit, &chan_dump)) {
                    cli_out("ERROR !! REF Events are not happening\n");
                } else {
                    COMPILER_64_COPY(diff1, chan_dump.debug_dump[1].feedback);
                    COMPILER_64_SUB_64(diff1, chan_dump.debug_dump[0].feedback);

                    cli_out("Ch# REF-TS:         (Diff)\n");
                    cli_out("%3d %05x_%08x\n", chan_dump.chan,
                        u64_H(chan_dump.debug_dump[0].feedback), u64_L(chan_dump.debug_dump[0].feedback));
                    cli_out("%3d %05x_%08x (%05x_%08x)\n", chan_dump.chan,
                        u64_H(chan_dump.debug_dump[1].feedback), u64_L(chan_dump.debug_dump[1].feedback),
                        u64_H(diff1), u64_L(diff1));
                }
                cli_out("#########################\n");
                bcm_gdpll_chan_enable(unit, chan, 0);
            }

            /* Disable the debug mode and destroy the channel */
            bcm_gdpll_chan_debug_enable(unit, chan, 0/*bcmGdpllDebugMode2*/, 1);
            bcm_gdpll_chan_destroy(unit, chan);

            cli_out("***** INFO: Please ensure to re-create the channel *****\n");

        } else if (parse_cmp("Enable", arg_string_p, 0)) {
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            chan = parse_integer(c);

            result = bcm_gdpll_chan_enable(unit, chan, 1);
            if (BCM_FAILURE(result)) {
                cli_out("Command failed. %s\n", bcm_errmsg(result));
                return CMD_FAIL;
            }

            /*cli_out("After Enable TS: "); print_ts(unit); */
        } else if (parse_cmp("Disable", arg_string_p, 0)) {
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }

            chan = parse_integer(c);
            result = bcm_gdpll_chan_enable(unit, chan, 0);
            if (BCM_FAILURE(result)) {
                cli_out("Command failed. %s\n", bcm_errmsg(result));
                return CMD_FAIL;
            }

        } else if (parse_cmp("Status", arg_string_p, 0)) {
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }

            chan = parse_integer(c);
            result = chan_status_get(unit, chan);
            if (BCM_FAILURE(result)) {
                cli_out("Command failed. %s\n", bcm_errmsg(result));
                return CMD_FAIL;
            }
        } else if (parse_cmp("Debug", arg_string_p, 0)) {
            uint32 debug_mode;

            /* Read the channel number */
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            chan = parse_integer(c);

            /* Read the debug mode */
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Debug mode \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            debug_mode = parse_integer(c);

            result = bcm_gdpll_chan_debug_enable(unit, chan, debug_mode, 1);
            if (BCM_FAILURE(result)) {
                cli_out("Command failed. %s\n", bcm_errmsg(result));
                return CMD_FAIL;
            }

        } else if (parse_cmp("Monitor", arg_string_p, 0)) {
            /* Read the channel number */
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            chan = parse_integer(c);

            if (pGdpllAppContext && (chan>=0) && (chan < BCM_GDPLL_NUM_CHANNELS)) {
                pGdpllAppContext->chan_mon_flag[chan] = 1;
            }

        } else if (parse_cmp("Notify", arg_string_p, 0)) {
            arg_string_p = ARG_GET(args);
            if (arg_string_p == NULL) {
                return CMD_USAGE;
            }

            if (parse_cmp("Start", arg_string_p, 0)) {
                result = bcm_gdpll_cb_register(unit, bcmGdpllCbChanState, bcm_gdpll_cb_notification, NULL);
                if (BCM_FAILURE(result)) {
                    cli_out("Error: Notification already started\n");
                    return CMD_FAIL;
                }
            } else if (parse_cmp("Stop", arg_string_p, 0)) {
                result = bcm_gdpll_cb_unregister(unit, bcmGdpllCbChanState);
                if (BCM_FAILURE(result)) {
                    return CMD_FAIL;
                }
            } else {
                return CMD_USAGE;
            }
        } else if (parse_cmp("Dump", arg_string_p, 0)) {
            uint32 pair_avail;
            bcm_gdpll_debug_mode_t chan_debug_mode;

            /* Read the channel number */
            c = ARG_GET(args);
            if (!isint(c)) {
                cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                return(CMD_USAGE);
            }
            chan = parse_integer(c);
            chan_debug_dump(unit, chan);

            if (chan < 32) {
                READ_NS_GDPLL_IA_TS_PAIR_AVAILABLE0r(unit, &pair_avail);
                pair_avail = pair_avail & (1 << chan);
                WRITE_NS_GDPLL_IA_TS_PAIR_AVAILABLE0r(unit, pair_avail);
            } else if (chan < 64) {
                READ_NS_GDPLL_IA_TS_PAIR_AVAILABLE1r(unit, &pair_avail);
                pair_avail = pair_avail & (1 << (chan-32));
                WRITE_NS_GDPLL_IA_TS_PAIR_AVAILABLE1r(unit, pair_avail);
            } else if (chan < 96) {
                READ_NS_GDPLL_IA_TS_PAIR_AVAILABLE2r(unit, &pair_avail);
                pair_avail = pair_avail & (1 << (chan-64));
                WRITE_NS_GDPLL_IA_TS_PAIR_AVAILABLE2r(unit, pair_avail);
            } else if (chan < 128) {
                READ_NS_GDPLL_IA_TS_PAIR_AVAILABLE3r(unit, &pair_avail);
                pair_avail = pair_avail & (1 << (chan-96));
                WRITE_NS_GDPLL_IA_TS_PAIR_AVAILABLE3r(unit, pair_avail);
            } else if (chan < 0 || chan >=128) {
                cli_out("Error: Wrong chan number %d\n", chan);
                return(CMD_USAGE);
            }

            _bcm_esw_gdpll_chan_debug_mode_get(unit, chan, &chan_debug_mode);
            _bcm_esw_gdpll_debug_get (unit, &enable);

            /* Check for phaseCounter Ref */
            READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal1);
            field1 = soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal1, PHASECOUNTERREFf);

            /* Dump the debug info */
            if((chan>=0) && (chan < BCM_GDPLL_NUM_CHANNELS)) {
                /* Check the status */
                cli_out("\n");
                chan_status_get(unit, chan);

                cli_out(" PairAvail(%3d): %d\n", chan, pair_avail?1:0);
                cli_out(" PhaCntRef(%3d): %d\n", chan, field1);
                cli_out(" DebugMode(%3d): %d(%s)  (EnLoging:%d)\n",
                    chan, (int)chan_debug_mode,
                    ((chan_debug_mode==1)||(chan_debug_mode==0))?"ClosedLoop":"OpenLoop",
                    enable);
                cli_out("************************************\n");
            }
        } else if (parse_cmp("Create", arg_string_p, 0)) {
            
            cli_out("### chan create flags:0x%x\n", flags);

#ifndef NO_FILEIO
            FILE * volatile fptr = NULL;
#endif
            c = ARG_GET(args);
            filename = c;
            if (filename == NULL) {
                cli_out("%s: Error: No file specified\n", ARG_CMD(args));
                return(CMD_USAGE);
            }

#ifndef NO_FILEIO
            /* Open the file */
            fptr = sal_fopen(filename, "r");
            if (!fptr) {
                cli_out("%s: Error: Unable to open file: %s\n",
                                       ARG_CMD(args), filename);
                return CMD_FAIL;
            }
#endif

            use_case_flag = 0;
            disable_debug_log = 0;

            /* Read the config from a file line by line */
            while (sal_fgets(line, sizeof(line), fptr)) {
                for (i=0; i < sizeof(line); i++) {
                    if (line[i] == '=') {
                        line[strlen(line)-1] = '\0';
                        line[i] = '\0';
                        gdpll_config(&gdpll_chan, line, line+i+1);
                        break;
                     }
                }
            }

            if (use_case_flag == 1)
            {
                uint64 TS;
                uint64 mask_bit;
                uint64 offset;
#ifdef CPRIMOD_SUPPORT
                uint32 cpriPort;
#endif
                /* cli_out("######## use_case_1A\n"); */
                cli_out("### GDPLL DIAG: use_case 1A config\n");

                /* If the computation of future time is taken care by caller of this diag, then
                 * the next arguments will be phase_counter_H and phase_counter_L
                 * Ex: SV-team test framework needs this.
                 */
                c = ARG_GET(args);
                if ((c != NULL) && isint(c)) {
                    u64_H(gdpll_chan.chan_dpll_state.phase_counter) = parse_integer(c);

                    c = ARG_GET(args);
                    if (!isint(c)) {
                        cli_out("%s: Error: Improper phase_counter_L\n", ARG_CMD(args));
                        return(CMD_USAGE);
                    }
                    u64_L(gdpll_chan.chan_dpll_state.phase_counter) = parse_integer(c);

                    cli_out("### Use-case-1a: phase_counter values from user H/L: %d / %d\n",
                        u64_H(gdpll_chan.chan_dpll_state.phase_counter),
                        u64_L(gdpll_chan.chan_dpll_state.phase_counter));
                } else {

                    poll_period_uSec = 30*60*1000000;   /* Poll for 30mins */
                    poll_interval_uSec = 0;  pps_detect = 0;
                    u64_H(mask_bit) = 0xFFFF;
                    u64_L(mask_bit) = 0xFFFFFFFF;

                    if ((c != NULL) && parse_cmp("SyncBS0HB", c, 0)) {
                        cli_out("   Note: 1A locking in close to input pulse on BS0HB\n");
                        field = BS0_HEARTBEATf;
                        poll_interval_uSec = 1; /* Poll for every 1uSec */

                    } else if ((c != NULL) && parse_cmp("SyncBS1HB", c, 0)) {
                        cli_out("   Note: 1A locking in close to input pulse on BS1HB\n");
                        field = BS1_HEARTBEATf;
                        poll_interval_uSec = 1; /* Poll for every 1uSec */

                    } else if ((c != NULL) && parse_cmp("SyncGPIO2", c, 0)) {
                        cli_out("   Note: 1A locking in close to input pulse on GPIO2\n");
                        field = TS_GPIO_2f;
                        poll_interval_uSec = 1; /* Poll for every 1uSec */

                    } else if (c != NULL) {
                        cli_out("%s: Error: Improper arg for 1A\n", ARG_CMD(args));
                        return(CMD_USAGE);
                    }

                    /* If the Sync is expected for (1G+1A) case */
                    if (poll_interval_uSec) {

                        cli_out("   Note: This logic is to test 1A+1G in sync\n");
                        cli_out("         Expectation is ensure 1G is locked and try enabling 1A\n");
                        cli_out("         close to 1PPS-In pulse of 1G\n");

                        /* Clear if any flag is already set */
                        SOC_IF_ERROR_RETURN(READ_NS_MISC_EVENT_STATUSr(unit, &rval));
                        soc_reg_field_set(unit, NS_MISC_EVENT_STATUSr, &rval, field, 1);
                        SOC_IF_ERROR_RETURN(WRITE_NS_MISC_EVENT_STATUSr(unit, rval));

                        /* Poll on the next 1PPS event */
                        while(poll_period_uSec > 0) {
                            SOC_IF_ERROR_RETURN(READ_NS_MISC_EVENT_STATUSr(unit, &rval));
                            if (soc_reg_field_get(unit, NS_MISC_EVENT_STATUSr, rval, field)) {
                                pps_detect = 1;
                                break;
                            }

                            sal_usleep(poll_interval_uSec);
                            poll_period_uSec -= poll_interval_uSec;
                        }
                    }

                    _bcm_esw_get_timestamp(unit, gdpll_chan.event_fb.ts_counter, &TS);

                    /*
                     * Setup procedure for 1A
                     * 1. Read current 52b Timestamp counter, say T
                     * 2. Compute C as under
                     *    R = floor((T >> 4) / 1e7)
                     *    C = (R*1e7 + 1e9) mod 2^48
                     * 3. Initialize phase counter to "C << 4"
                     */
                    u64_H(mask_bit) = 0;
                    u64_L(mask_bit) = 10000000;

                    COMPILER_64_SHR(TS, 4);
                    COMPILER_64_UDIV_64(TS, mask_bit);
                    COMPILER_64_UMUL_32(TS, 10000000);
                    COMPILER_64_ADD_32(TS, 1000000000);

                    u64_H(mask_bit) = 0xFFFF;
                    u64_L(mask_bit) = 0xFFFFFFFF;
                    COMPILER_64_AND(TS, mask_bit);
                    COMPILER_64_SHL(TS, 4);

                    COMPILER_64_COPY(gdpll_chan.chan_dpll_state.phase_counter, TS);
                    COMPILER_64_COPY(offset, TS);

#ifdef CPRIMOD_SUPPORT
                    cpriPort = gdpll_chan.event_fb.port;
                    COMPILER_64_ADD_32(offset, 1066656);    /* i.e, 66667 << 4 */
                    _bcm_future_cpri_enable(unit, cpriPort, offset);
#endif
                }
            } else if (use_case_flag == 2) {

                /* use-case 1E */
                cli_out("### GDPLL DIAG: use_case 1E config\n");
                bcn_update = 1;
                result = bcm_gdpll_cb_register(unit, bcmGdpllCbRsvd1, bcm_gdpll_cb_rsvd1, NULL);
                if (BCM_FAILURE(result)) {
                    return CMD_FAIL;
                }
            } else if (use_case_flag == 3) {
                /* use-case 1D */
                uint32 busy_wait = 10;  /* uSec wait per cycle */
                uint32 wait_period = 20*1000/busy_wait;   /* 20mSec */

                cli_out("### GDPLL DIAG: use_case 1D(Option2) config\n");

                /* Flag for callback function to update TS */
                ts_update = 1;
                result = bcm_gdpll_cb_register(unit, bcmGdpllCbRsvd1, bcm_gdpll_cb_rsvd1, NULL);
                if (BCM_FAILURE(result)) {
                    return CMD_FAIL;
                }

                while (ts_update) {
                    sal_usleep(busy_wait);
                    wait_period--;

                    if (wait_period == 0) {
                        cli_out("### Error !! TS counter not updated with latest RP1\n");
                        return CMD_FAIL;
                    }
                }

                /* Set the phase_counter */
                u64_H(gdpll_chan.chan_dpll_state.phase_counter) = u64_H(ts52b);
                u64_L(gdpll_chan.chan_dpll_state.phase_counter) = u64_L(ts52b);

                cli_out("### phase_counter : u:0x%x l:0x%x\n",
                    u64_H(gdpll_chan.chan_dpll_state.phase_counter),
                    u64_L(gdpll_chan.chan_dpll_state.phase_counter));

            } else if (use_case_flag == 4) {
                /* use-case 1g */
                uint32 ts_gpio = 0;
                uint64 ts_reset = 0;
                uint64 mask_bit;
                int    gpsToD = 0;
                uint32 pToD = ToD_LOCATION;
                char tod_buffer[ToD_BUFFER_LENGTH];
                uint32 val, val1, i;
                char mydate[12];
                char mytime[10];
#if defined(GDPLL_SAL_HAS_TIME)
                uint32_t/*time_t*/ time;
#endif

                tod_buffer[0] = '\0';
                pps_detect = 0;

                cli_out("### GDPLL DIAG: use_case 1G config\n");

                c = ARG_GET(args);
                if ((c != NULL) && parse_cmp("SyncGPSToD", c, 0)) {
                    cli_out("### GDPLL DIAG: This option is only for DEMO\n");
                    cli_out("    Setup:\n");
                    cli_out("    1. Ensure that the R5 FW is loaded, that acts as the GPS source\n");
                    cli_out("    2. Host polls the ToD from GPS to be programmed to TS counter\n");

                    /* Sanity check if the ToD is coming ??? */
                    sal_usleep(1000*1000*2);    /* To check the ToD count incrementing */

                    val = soc_cm_iproc_read(unit, ToD_LOCATION_AVAIL);
                    sal_usleep(1000*1000*3);    /* Wait for 5 sec to check the ToD count */
                    val1 = soc_cm_iproc_read(unit, ToD_LOCATION_AVAIL);
                    if (val1 != val) {
                        cli_out("### GDPLL DIAG: GPS-ToD Available\n");
                        gpsToD = 1;
                    } else {
                        cli_out("### GDPLL DIAG: Error, ToD NOT Available. Please check R5-FW val:%d val1:%d\n", val, val1);
                        return CMD_FAIL;
                    }
                } else if (c != NULL) {
                    cli_out("### GDPLL DIAG: Invalid command\n");
                    return CMD_FAIL;
                }

                /* Disable the timestamper */
                _bcm_esw_timestamp_enable(unit, gdpll_chan.event_fb.ts_counter, 0);

                u64_H(ts_reset) = 0; u64_L(ts_reset) = 0;
                _bcm_esw_set_timestamp(unit, gdpll_chan.event_fb.ts_counter, ts_reset);

                poll_period_uSec = 30*60*1000000;   /* Poll for 30mins */
                poll_interval_uSec = 1;

                if ((gdpll_chan.event_fb.input_event == bcmGdpllInputEventBS0HB) ||
                    (gdpll_chan.event_fb.input_event == bcmGdpllInputEventBS1HB)) {
                    cli_out("  1PPS input on BS%d-HB configured in INPUT-Mode !!!\n",
                            gdpll_chan.event_fb.input_event-bcmGdpllInputEventBS0HB);
#if 0
                    
                    /* Poll for event */
                    if (BCM_E_NONE ==
                        bcm_gdpll_input_event_poll(unit, gdpll_chan.event_fb.input_event, poll_period_uSec)) {
                        pps_detect = 1;
                    }
#else
                    /* Set BS Input-Mode and Enable-Capture */
                    if (gdpll_chan.event_fb.input_event == bcmGdpllInputEventBS0HB) {
                        SOC_IF_ERROR_RETURN(READ_NS_BS0_BS_CONFIGr(unit, &rval));
                        soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &rval, MODEf, 0);
                        SOC_IF_ERROR_RETURN(WRITE_NS_BS0_BS_CONFIGr(unit, rval));

                        SOC_IF_ERROR_RETURN(READ_NS_MISC_CLK_EVENT_CTRLr(unit, &rval));
                        rval = rval | 0x2;
                        SOC_IF_ERROR_RETURN(WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, rval));

                    } else {
                        SOC_IF_ERROR_RETURN(READ_NS_BS1_BS_CONFIGr(unit, &rval));
                        soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &rval, MODEf, 0);
                        SOC_IF_ERROR_RETURN(WRITE_NS_BS1_BS_CONFIGr(unit, rval));

                        SOC_IF_ERROR_RETURN(READ_NS_MISC_CLK_EVENT_CTRLr(unit, &rval));
                        rval = rval | 0x4;
                        SOC_IF_ERROR_RETURN(WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, rval));
                    }

                    poll_period_uSec = 30*60*1000000;   /* Poll for 30mins */
                    poll_interval_uSec = 1; /* Poll for every 1uSec */

                    /* Clear if any flag is already set */
                    SOC_IF_ERROR_RETURN(READ_NS_MISC_EVENT_STATUSr(unit, &rval));
                    soc_reg_field_set(unit, NS_MISC_EVENT_STATUSr, &rval,
                        gdpll_chan.event_fb.input_event == bcmGdpllInputEventBS0HB ? BS0_HEARTBEATf: BS1_HEARTBEATf,
                        1);
                    SOC_IF_ERROR_RETURN(WRITE_NS_MISC_EVENT_STATUSr(unit, rval));

                    /* Poll on the next 1PPS event */
                    while(poll_period_uSec > 0) {
                        SOC_IF_ERROR_RETURN(READ_NS_MISC_EVENT_STATUSr(unit, &rval));
                        if (((gdpll_chan.event_fb.input_event == bcmGdpllInputEventBS0HB) &&
                             soc_reg_field_get(unit, NS_MISC_EVENT_STATUSr, rval, BS0_HEARTBEATf)) ||
                            ((gdpll_chan.event_fb.input_event == bcmGdpllInputEventBS1HB) &&
                             soc_reg_field_get(unit, NS_MISC_EVENT_STATUSr, rval, BS1_HEARTBEATf))) {

                            pps_detect = 1;
                            break;
                        }
                        sal_usleep(poll_interval_uSec);
                        poll_period_uSec -= poll_interval_uSec;
                    }
#endif
                } else if((gdpll_chan.event_fb.input_event == bcmGdpllInputEventGPIO0) ||
                          (gdpll_chan.event_fb.input_event == bcmGdpllInputEventGPIO1) ||
                          (gdpll_chan.event_fb.input_event == bcmGdpllInputEventGPIO2) ||
                          (gdpll_chan.event_fb.input_event == bcmGdpllInputEventGPIO3) ||
                          (gdpll_chan.event_fb.input_event == bcmGdpllInputEventGPIO4) ||
                          (gdpll_chan.event_fb.input_event == bcmGdpllInputEventGPIO5)) {

                    /* Find the input TS_GPIO */
                    ts_gpio = gdpll_chan.event_fb.input_event - bcmGdpllInputEventGPIO0;
                    if (ts_gpio < 0 || ts_gpio > 5) {
                        cli_out("Error: Wrong GPIO input %d !!!\n", gdpll_chan.event_fb.input_event);
                        return CMD_FAIL;
                    }
                    cli_out("  1PPS input on TS_GPIO%d !!!\n", ts_gpio);

                    /* Poll for event */
                    if (BCM_E_NONE ==
                        bcm_gdpll_input_event_poll(unit, gdpll_chan.event_fb.input_event, poll_period_uSec)) {
                        pps_detect = 1;
                    }
                }

                if (!pps_detect) {
                    cli_out("Error: No 1PPS signal detected !!!\n");
                    return CMD_FAIL;
                }

                if (gpsToD) {
                    /* ToD read from R5 is in NMEA format, "$GPZDA,055559.00,15,10,2018" */
                    pToD = ToD_LOCATION;

                    for (i=0; i < ToD_BUFFER_LENGTH; i+=4, pToD+=4) {
                        val = soc_cm_iproc_read(unit, pToD);
                        tod_buffer[i] = 0xFF & val;
                        tod_buffer[i+1] = 0xFF & (val>>8);
                        tod_buffer[i+2] = 0xFF & (val>>16);
                        tod_buffer[i+3] = 0xFF & (val>>24);
                    }

                    /*cli_out("### GDPLL DIAG: ToD- %s\n", tod_buffer);*/
                    if (tod_buffer[0] == '$') {
                        /* ToD read from R5, NMEA format is "$GPZDA,055559.00,15,10,2018" */

                        /* Date in YYYY-MM-DD format */
                        mydate[0]=tod_buffer[23]; mydate[1]=tod_buffer[24]; mydate[2]=tod_buffer[25]; mydate[3]=tod_buffer[26];
                        mydate[4]='-';
                        mydate[5]=tod_buffer[20]; mydate[6]=tod_buffer[21];
                        mydate[7]='-';
                        mydate[8]=tod_buffer[17]; mydate[9]=tod_buffer[18];
                        mydate[10]='\0';

                        /* Time in HH:MM:SS format */
                        mytime[0]= tod_buffer[7]; mytime[1]= tod_buffer[8];
                        mytime[2]=':';
                        mytime[3]= tod_buffer[9]; mytime[4]= tod_buffer[10];
                        mytime[5]=':';
                        mytime[6]= tod_buffer[11]; mytime[7]= tod_buffer[12];
                        mytime[8]='\0';

                        cli_out("### GDPLL DIAG: ToD-format: $GPZDA,055559.00,15,10,2018\n");

                    } else if (tod_buffer[0] == '#') {
                        /* ToD read from R5, format is custom format, "#51,10242018,130840" */
                        int start;

                        /* Date in YYYY-MM-DD format */
                        start = 8;
                        mydate[0]=tod_buffer[start]; mydate[1]=tod_buffer[start+1]; mydate[2]=tod_buffer[start+2]; mydate[3]=tod_buffer[start+3];
                        mydate[4]='-';
                        start = 4;
                        mydate[5]=tod_buffer[start]; mydate[6]=tod_buffer[start+1];
                        mydate[7]='-';
                        mydate[8]=tod_buffer[start+2]; mydate[9]=tod_buffer[start+3];
                        mydate[10]='\0';

                        /* Time in HH:MM:SS format */
                        start = 13;
                            mytime[0]= tod_buffer[start]; mytime[1]= tod_buffer[start+1];
                        mytime[2]=':';
                        mytime[3]= tod_buffer[start+2]; mytime[4]= tod_buffer[start+3];
                        mytime[5]=':';
                        mytime[6]= tod_buffer[start+4]; mytime[7]= tod_buffer[start+5];
                        mytime[8]='\0';

                        cli_out("### GDPLL DIAG: ToD-format: #51,10242018,130840\n");
                    }
                    cli_out("### GDPLL DIAG: myTime:%s myDate:%s\n", mytime, mydate);

#if defined(GDPLL_SAL_HAS_TIME)
                    time = (uint32_t)get_time(mydate, mytime);

                    /* Conver time to 52b timestamp format and program to TS counter here
                     * Assume a as a Linux epoch timestamp
                     * int64 b =  (a* 1000000000LL) & 0xffffffffffffLL;   - convert to 48 bit nanoseconds
                     *       b = b << 4;   - Convert to 52 bit Monterey format
                     */

                    /* 48b mask */
                    u64_H(mask_bit) = 0xFFFF;
                    u64_L(mask_bit) = 0xFFFFFFFF;

                    u64_H(ts_reset) = 0; u64_L(ts_reset) = time;
                    COMPILER_64_UMUL_32(ts_reset, 1000000000);
                    COMPILER_64_AND(ts_reset, mask_bit);
                    COMPILER_64_SHL(ts_reset, 4);

                    /* Program TS counter and phase-counter */
                    _bcm_esw_set_timestamp(unit, gdpll_chan.event_fb.ts_counter, ts_reset);
                    COMPILER_64_COPY(gdpll_chan.chan_dpll_state.phase_counter, ts_reset);
                    cli_out("### GDPLL DIAG: GPS tick:%d,  phase_counter: 0x%x 0x%x\n",
                            time, u64_H(ts_reset), u64_L(ts_reset));
#else
                    cli_out("### GDPLL DIAG: Error!! No TIMEconversion functions for %s %s\n", mydate, mytime);
                    return CMD_FAIL;
#endif
                }

                /* Enable the timestamper */
                _bcm_esw_timestamp_enable(unit, gdpll_chan.event_fb.ts_counter, 1);

            } else if (use_case_flag == 5) {
                /* use-case 2d */
                int rv  = BCM_E_NONE;

                cli_out("### GDPLL DIAG: use_case 2D config\n");

                port_out = gdpll_chan.port;
                if ((rv = bcm_rx_register(unit, "GdpllROEFrame", roe_pkt_cb_handler,
                                   101/*0*/, NULL,  /*BCM_RCO_F_INTR |*/ BCM_RCO_F_ALL_COS)) < 0)
                {
                    cli_out("### GDPLL: bcm_rx_register failed: %s\n", bcm_errmsg(rv));
                    return(CMD_USAGE);
                }
                cli_out("### GDPLL: bcm_rx_register Success\n");
            } else if (use_case_flag == 6) {

                /* use-case: BS synthesizer */
                uint64 T, V, val_64b;
                uint64 mask_bit;
                int rv  = BCM_E_NONE;
                bcm_time_ts_counter_t counter;

                cli_out("### GDPLL DIAG: BS Synthesizer config\n");

                /* Get the current 52b timestamper value */
                counter.ts_counter=gdpll_chan.event_fb.ts_counter;
                rv = bcm_time_ts_counter_get(unit, &counter);
                if (rv != BCM_E_NONE) {
                    cli_out("### Error in bcm_time_ts_counter_get\n");
                    return CMD_FAIL;
                }
                COMPILER_64_COPY(T, counter.ts_counter_ns);

                /* V = floor(T / 16,000,000,000) */
                COMPILER_64_COPY(V, T);
                COMPILER_64_SET(val_64b, 0x3, 0xB9ACA000);  /* 16,000,000,000 = 0x3_B9AC_A000 */
                COMPILER_64_UDIV_64(V, val_64b);

                /* M = ((V+1)*16,000,000,000 - 1,600,000) Mod 2^52 */
                COMPILER_64_ADD_32(V, 2);
                COMPILER_64_UMUL_32(V, 1000000000);
                COMPILER_64_SHL(V, 4);
                if(gdpll_chan.event_fb.event_divisor == 2500) {
                    COMPILER_64_SUB_32(V, 4000000); /* For 4Khz DPLL rate */
                } else {
                    COMPILER_64_SUB_32(V, 1600000);
                }

                /* 52b mask for Mod 2^52 */
                u64_H(mask_bit) = 0xFFFFF;
                u64_L(mask_bit) = 0xFFFFFFFF;

                COMPILER_64_AND(V, mask_bit);
                COMPILER_64_COPY(gdpll_chan.chan_dpll_state.phase_counter, V);

                LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                                " BS synth usecase : CurrentTS[%llu/0x%llx] phase_counter@[%llu/0x%llx]\n"),
                     (long long unsigned int)counter.ts_counter_ns,
                     (long long unsigned int)counter.ts_counter_ns,
                     (long long unsigned int)gdpll_chan.chan_dpll_state.phase_counter,
                     (long long unsigned int)gdpll_chan.chan_dpll_state.phase_counter));

                /* Minimize the initial phase error
                 * if ((phase_counter - TS) > 1mSec) then sleep(phase_counter - TS - 1mSec)
                 */
                COMPILER_64_SET(val_64b, 0, 1000000);  /* 1mSec = 1000000nSec */
                /* If sync_mode is enabled, consider the start_delay */
                uint32 regVal = 0, bc_high=0, bc_low=0, hb_down=0;
                int sync_mode_enabled = 0;

                if (gdpll_chan.event_fb.input_event == bcmGdpllInputEventBS0PLL) {
                    READ_NS_REGr(unit, NS_BROADSYNC_SYNC_MODEr, 0, &regVal);
                    sync_mode_enabled = soc_reg_field_get(unit, NS_BROADSYNC_SYNC_MODEr, regVal, SYNC_MODEf);

                    READ_NS_REGr(unit, NS_BS0_BS_CLK_CTRLr, 0, &regVal);
                    bc_high = soc_reg_field_get(unit, NS_BS0_BS_CLK_CTRLr, regVal, HIGH_DURATIONf);

                    READ_NS_REGr(unit, NS_BS0_BS_CLK_CTRLr, 0, &regVal);
                    bc_low = soc_reg_field_get(unit, NS_BS0_BS_CLK_CTRLr, regVal, LOW_DURATIONf);

                    READ_NS_REGr(unit, NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr, 0, &regVal);
                    hb_down = soc_reg_field_get(unit, NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr, regVal, DOWN_DURATIONf);

                } else if (gdpll_chan.event_fb.input_event == bcmGdpllInputEventBS1PLL) {
                    READ_NS_REGr(unit, NS_COMMON_CTRLr, 0, &regVal);
                    sync_mode_enabled = soc_reg_field_get(unit, NS_COMMON_CTRLr, regVal, RESERVEDf);
                    sync_mode_enabled = ((sync_mode_enabled>>26) & 0x1);

                    READ_NS_REGr(unit, NS_BS1_BS_CLK_CTRLr, 0, &regVal);
                    bc_high = soc_reg_field_get(unit, NS_BS1_BS_CLK_CTRLr, regVal, HIGH_DURATIONf);

                    READ_NS_REGr(unit, NS_BS1_BS_CLK_CTRLr, 0, &regVal);
                    bc_low = soc_reg_field_get(unit, NS_BS1_BS_CLK_CTRLr, regVal, LOW_DURATIONf);

                    READ_NS_REGr(unit, NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr, 0, &regVal);
                    hb_down = soc_reg_field_get(unit, NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr, regVal, DOWN_DURATIONf);
                }
                if (sync_mode_enabled) {
                    uint32 delay = (( ((hb_down-1)*(bc_low+bc_high))+bc_low)*50);
                    COMPILER_64_ADD_32(val_64b, delay);
                    LOG_VERBOSE(BSL_LS_BCM_PTP,
                                    (BSL_META_U(unit,
                                    " sync_mode_enabled[%d] delay[%u]\n"), sync_mode_enabled, delay));
                }
                COMPILER_64_SHR(V, 4);
                COMPILER_64_SHR(T, 4);
                COMPILER_64_SUB_64(V, T);

                if (COMPILER_64_GT(V, val_64b)) {
                    LOG_VERBOSE(BSL_LS_BCM_PTP,
                        (BSL_META_U(unit,
                                    "Available headroom[%llu nsec] \n"), (long long unsigned int)u64_L(V)));
                    COMPILER_64_SUB_64(V, val_64b);
                    COMPILER_64_UDIV_32(V, 1000);
                    LOG_VERBOSE(BSL_LS_BCM_PTP,
                        (BSL_META_U(unit,
                                    "wait for %u usec \n"), (unsigned)u64_L(V)));
                    sal_usleep(u64_L(V));
                } else {
                    LOG_VERBOSE(BSL_LS_BCM_PTP,
                        (BSL_META_U(unit,
                                    "Insufficient headroom[%llu / 0x%llx] \n"),
                                    (long long unsigned int)u64_L(V),
                                    (long long unsigned int)u64_L(V)));
                }

           } else if ((use_case_flag == 7) || (use_case_flag == 8)) {

                /* use-case: 1N/1P */
                uint64 secs, div, nano_secs;
                bcm_time_spec_t ts;
                bcm_time_ts_counter_t counter;
                int prog_phase_counter = 0;

                cli_out("### GDPLL DIAG: use_case 1N config\n");

                poll_period_uSec = 30*60*1000000;   /* Poll for 30mins */
                poll_interval_uSec = 100000; /* Poll for every 100000uSec=100mSec */

                counter.ts_counter = gdpll_chan.event_fb.ts_counter;
                while(poll_period_uSec > 0) {

                    BCM_IF_ERROR_RETURN(bcm_time_ts_counter_get(unit, &counter));

                    /* MO: Remove sub-ns portion */
                    COMPILER_64_SHR(counter.ts_counter_ns, 4);

                    /* TODO - FIXME !!
                     * NTP/PTP ToD counter will get the first ONE_SEC set after 500mSec/537mSec respectively, after enabling.
                     * This is verified by reading the present TS counter value(say T1) before programming 0 nanosec to
                     * NTP/PTP ToD and read the fisrt 1PPS event(i.e, ONE_SEC) of NTP/PTP ToD. With this its observed that the
                     * timestamp is (T1+500mSec/537mSec).
                     * To compensate this, start the TS counter prior by 500mSec/(1000-537=463)mSec
                     */
                    if (use_case_flag == 7) {
                        COMPILER_64_SUB_32(counter.ts_counter_ns, 500000000);
                    } else if (use_case_flag == 8) {
                        COMPILER_64_SUB_32(counter.ts_counter_ns, 463000000);
                    }

                    /* Get seconds from the timestamp value */
                    COMPILER_64_COPY(secs, counter.ts_counter_ns);
                    COMPILER_64_SET(div, 0, 1000000000);
                    COMPILER_64_UDIV_64(secs, div);
                    COMPILER_64_SET(ts.seconds, 0, COMPILER_64_LO(secs));

                    /* Get nanoseconds from the timestamp value */
                    COMPILER_64_COPY(nano_secs, counter.ts_counter_ns);
                    COMPILER_64_UMUL_32(secs, 1000000000);
                    COMPILER_64_SUB_64(nano_secs, secs);
                    ts.nanoseconds = COMPILER_64_LO(nano_secs);

                    /* Bound check for the DPLL setup */
                    if ((ts.nanoseconds < 800000000) && (ts.nanoseconds > 600000000)) {
                        prog_phase_counter = 1;
                        break;
                    }

                    sal_usleep(poll_interval_uSec);
                    poll_period_uSec -= poll_interval_uSec;
                }

                if (!prog_phase_counter) {
                    cli_out("Error: 1n: Could not program phase counter !!!\n");
                    return CMD_FAIL;
                }

#ifdef NTP_PTP_DEBUG
                /*
                 * Enable this, for testing/ensuring the 1st ON_SEC pulse after
                 * 500mSec/537mSec for NTP/PTP ToD counter respectively as explained above
                 */
                ts.nanoseconds = 0; /* ### Just for testing */
                cli_out("### 1N/1P: ts.seconds: 0x%x 0x%x\n", COMPILER_64_HI(ts.seconds), COMPILER_64_LO(ts.seconds));
                cli_out("### 1N/1P: ts.nanoseconds: 0x%x\n", ts.nanoseconds);
#endif
                if (use_case_flag == 7) {
                    /* Program the NTP counter */
                    _bcm_program_ptpntptod(unit, ts, ToD_TYPE_NTP);
                } else if (use_case_flag == 8) {
                    /* Program the PTP counter */
                    _bcm_program_ptpntptod(unit, ts, ToD_TYPE_PTP);
                }

                /* Program the phase counter per spec */
                COMPILER_64_ADD_32(ts.seconds, 1);
                COMPILER_64_UMUL_32(ts.seconds, 1000000000);
                COMPILER_64_SHL(ts.seconds, 4);
                COMPILER_64_COPY(gdpll_chan.chan_dpll_state.phase_counter, ts.seconds);
                /* cli_out("### 1N: phase_counter: 0x%x 0x%x\n", COMPILER_64_HI(gdpll_chan.chan_dpll_state.phase_counter),
                    COMPILER_64_LO(gdpll_chan.chan_dpll_state.phase_counter)); */
           } else if (use_case_flag == 9) {
               /* use-case 1D option3*/
               uint32 busy_wait = 10;  /* uSec wait per cycle */
               uint32 wait_period = 50*1000/busy_wait;   /* 50 msec */
               bcm_time_capture_cb_type_t cb_type = bcmTimeCaptureAll;
               bcm_time_interface_t intf;
               intf.id = 0;
               u64_H(ts52b) = 0;
               u64_L(ts52b) = 0;
               u64_H(gdpll_offset) = 0;
               u64_L(gdpll_offset) = 0;
               u64_H(rsvd1_if_ts) = 0;
               u64_L(rsvd1_if_ts) = 0;
               u64_H(rsvd1_rf_ts) = 0;
               u64_L(rsvd1_rf_ts) = 0;
               flag_rsvd1_if_ts=0;
               flag_rsvd1_rf_ts=0;

               cli_out("### GDPLL DIAG: use_case 1D(Option3) config\n");

               parse_table_t pt;
               parse_table_init(unit, &pt);
               parse_table_add(&pt, "Rsvd1Type", PQ_INT, (void*)8, &rsvd1_msg_type, NULL);
               if (parse_arg_eq(args, &pt) < 0) {
                   cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(args), ARG_CUR(args));
                   parse_arg_eq_done(&pt);
                   return(CMD_FAIL);
               }
               parse_arg_eq_done(&pt);
               cli_out(" \t\t rsvd1_msg_type[%d] \n", rsvd1_msg_type);

               /* enable RSVD1 events */
               result = _bcm_gdpll_enable_rsvd1_events(unit, 1);
               if (BCM_FAILURE(result)) {
                   cli_out("_bcm_gdpll_enable_rsvd1_events returned %d FAIL \n", result);
                   return CMD_FAIL;
               }

               /* enable callback to retrieve FIFO TS */
               result = bcm_time_capture_cb_register(unit, intf.id, cb_type, &rsvd1_timestamp_cb, NULL);
               if (BCM_FAILURE(result)) {
                   cli_out("bcm_time_capture_cb_register returned %d FAIL \n", result);
                   return CMD_FAIL;
               }
               cli_out("bcm_time_capture_cb_register returned %d SUCCESS \n", result);

               /* Flag for callback function to update TS */
               ts_update = 1;

               while (ts_update) {
                   sal_usleep(busy_wait);
                   wait_period--;

                   if (wait_period == 0) {
                       cli_out("### Error !! TS counter not updated with latest RSVD1\n");
                       return CMD_FAIL;
                   }
               }

               /* Set the phase_counter */
               u64_H(gdpll_chan.chan_dpll_state.phase_counter) = u64_H(ts52b);
               u64_L(gdpll_chan.chan_dpll_state.phase_counter) = u64_L(ts52b);

               u64_H(gdpll_chan.chan_dpll_state.offset) = u64_H(gdpll_offset);
               u64_L(gdpll_chan.chan_dpll_state.offset) = u64_L(gdpll_offset);

               cli_out("### phase_counter : u:0x%x l:0x%x\n",
                       u64_H(gdpll_chan.chan_dpll_state.phase_counter),
                       u64_L(gdpll_chan.chan_dpll_state.phase_counter));

               cli_out("### offset : u:0x%x l:0x%x\n",
                       u64_H(gdpll_chan.chan_dpll_state.offset),
                       u64_L(gdpll_chan.chan_dpll_state.offset));

           }

            if (flags & BCM_GDPLL_CHAN_ALLOC) {
                chan = 0xFF;
            } else {
                /* User should have passed the channel number */
                c = ARG_GET(args);
                if (!isint(c)) {
                    cli_out("%s: Error: Channel number \n", ARG_CMD(args));
                    return(CMD_USAGE);
                }
                chan = parse_integer(c);
            }

            result = bcm_gdpll_chan_create (unit, flags, &gdpll_chan, &chan);
            if (BCM_FAILURE(result)) {
                cli_out("Command failed. %s\n", bcm_errmsg(result));
                return CMD_FAIL;
            }
            cli_out("Channel: %d\n", chan);

        } else {
            cli_out("%s: Error: Channel command\n", ARG_CMD(args));
            return(CMD_USAGE);
        }
    } else {
        cli_out("%s: Error: GDPLL command\n", ARG_CMD(args));
        return(CMD_USAGE);
    }

    return CMD_OK;
}
#endif
