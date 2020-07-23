/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains Time Interface definitions internal to the BCM library.
 */

#ifndef _BCM_INT_COMMON_TIME_H
#define _BCM_INT_COMMON_TIME_H

#include <sal/core/sync.h>

#ifdef INCLUDE_GDPLL
#include <sal/core/thread.h>
#endif

/* Time module internal defines */
#define TIME_EPOCH_MAX          0x7FFF       /* Max epoch value allowed in HW */
#define TIME_SEC_MAX            0xFFFFFFFF   /* HW supports only 32 bits second */
#define TIME_NANOSEC_MAX        1000000000   /* Max nanosec value is 10^9 */
#define TIME_DRIFT_DENOMINATOR  1000000000   /* Drift denominator = 1 second */
#define TIME_DRIFT_MAX       TIME_DRIFT_DENOMINATOR / 8

#define TIME_ACCURACY_CLK_MAX        19     /* How many accuracy values we can have */

#define TIME_MODE_INPUT              0x0    /* Time interface will recieve the time as input(Slave)*/
#define TIME_MODE_OUTPUT             0x1    /* Time interface will output the time (Master)*/

/* HELIX4 and KATANA2 capture mode bits */
#define IPROC_TIME_CAPTURE_MODE_DISABLE    0x0     /* Disable HW time capture */
#define IPROC_TIME_CAPTURE_MODE_IMMEDIATE  0x1     /* Capture time immediately on CPU request */
#define IPROC_TIME_CAPTURE_MODE_HB_BS_0    0x2     /* Capture time on rising edge of heartbeat of BroadSync 0 */
#define IPROC_TIME_CAPTURE_MODE_HB_BS_1    0x4     /* Capture time on rising edge of heartbeat of BroadSync 1 */
#define IPROC_TIME_CAPTURE_MODE_GPIO_0     0x8     /* Capture time on GPIO event */
#define IPROC_TIME_CAPTURE_MODE_GPIO_1     0x10
#define IPROC_TIME_CAPTURE_MODE_GPIO_2     0x20
#define IPROC_TIME_CAPTURE_MODE_GPIO_3     0x40
#define IPROC_TIME_CAPTURE_MODE_GPIO_4     0x80
#define IPROC_TIME_CAPTURE_MODE_GPIO_5     0x100
#define IPROC_TIME_CAPTURE_MODE_SYNCE_1    0x200   /* Capture on rising edge of SyncE clk_x event */
#define IPROC_TIME_CAPTURE_MODE_SYNCE_2    0x400
#define IPROC_TIME_CAPTURE_MODE_SYNCE_3    0x800
#define IPROC_TIME_CAPTURE_MODE_SYNCE_4    0x1000
#define IPROC_TIME_CAPTURE_MODE_SYNCE_5    0x2000
#define IPROC_TIME_CAPTURE_MODE_IP_DM_0    0x4000  /* Capture on rising edge of IP_DM_x event */
#define IPROC_TIME_CAPTURE_MODE_IP_DM_1    0x8000
#define IPROC_TIME_CAPTURE_MODE_TS1        0x10000
#define IPROC_TIME_CAPTURE_MODE_CLK_BS_0   0x20000 /* Capture on rising edge of Broadsync 0 clock event */
#define IPROC_TIME_CAPTURE_MODE_CLK_BS_1   0x40000 /* Capture on rising edge of Broadsync 1 clock event */

#define TIME_CAPTURE_MODE_DISABLE    0x0    /* Disable HW time capture */
#define TIME_CAPTURE_MODE_IMMEDIATE  0x1    /* Capture time immediately on CPU request */
#define TIME_CAPTURE_MODE_HEARTBEAT  0x2    /* Capture time on rising edge of heartbeat */
#define TIME_CAPTURE_MODE_GPIO_0     0x4    /* Capture time on GPIO event */
#define TIME_CAPTURE_MODE_GPIO_1     0x8
#define TIME_CAPTURE_MODE_GPIO_2     0x10
#define TIME_CAPTURE_MODE_GPIO_3     0x20
#define TIME_CAPTURE_MODE_GPIO_4     0x40
#define TIME_CAPTURE_MODE_GPIO_5     0x80
#define TIME_CAPTURE_MODE_L1_CLOCK_PRIMARY   0x100 /* Capture on L1 Clock */
#define TIME_CAPTURE_MODE_L1_CLOCK_SECONDARY 0x200 
#define TIME_CAPTURE_MODE_LCPLL      0x400  /* Capture on LCPLL */
#define TIME_CAPTURE_MODE_IP_DM_0    0x800
#define TIME_CAPTURE_MODE_IP_DM_1    0x1000


#define TIME_HEARTBEAT_HZ_CLK       125000000   /* 125 MHz */
#define TIME_HEARTBEAT_HZ_MAX       8000        /* 8 KHz */
#define TIME_HEARTBEAT_HZ_MIN       1           /* 1 Hz */
#define TIME_HEARTBEAT_NS_HZ        1000000000  /* 10^9 ns in 1 HZ */


 /* Full cycle BS clock frequencies values in nanoseconds */
#define TIME_BS_FREQUENCY_1000NS     1000      
#define TIME_BS_FREQUENCY_1024NS     1024
#define TIME_BS_FREQUENCY_1544NS     1544
#define TIME_BS_FREQUENCY_2000NS     2000
#define TIME_BS_FREQUENCY_2048NS     2048

#define TIME_BS_FREQUENCIES_NUM         5       /* Total number of supported frequencies */

#define TIME_TS_CLK_FREQUENCY_40NS  40    /* 25Mhz TS_CLK */

#define TIME_INTERFACE_ID_MAX(unit)  NUM_TIME_INTERFACE(unit) - 1

#ifdef INCLUDE_GDPLL
#define _BCM_TIME_BS_EVENT_TS_COUNTER_REPROG 0x01
#endif

typedef enum _bcm_time_message_type_e {
    _BCM_TIME_FREQ_OFFSET_SET,
    _BCM_TIME_FREQ_OFFSET_GET,
    _BCM_TIME_PHASE_OFFSET_SET,
    _BCM_TIME_PHASE_OFFSET_GET,
    _BCM_TIME_DEBUG_1PPS_SET,
    _BCM_TIME_DEBUG_1PPS_GET,
    _BCM_TIME_STATUS_GET,
    _BCM_TIME_LOG_SET,
    _BCM_TIME_LOG_GET,
    _BCM_TIME_NTP_OFFSET_SET,
    _BCM_TIME_NTP_OFFSET_GET,
    _BCM_TIME_MAX_MESSAGE_NUM,
    _BCM_TIME_BS_TIME_GET,
    _BCM_TIME_BS_TIME_SET
} _bcm_time_message_type_t;


/* User call back management structure */
typedef struct _bcm_time_user_cb_s {
    bcm_time_heartbeat_cb  heartbeat_cb;    /* Callback function on hearbeat */
    void                   *user_data;      /* User data provided */
} _bcm_time_user_cb_t, *_bcm_time_user_cb_p;

/* Time interface managment structure. */
typedef struct _bcm_time_interface_config_s {
    bcm_time_interface_t    time_interface; /* Time Interface structure */
    bcm_time_capture_t      time_capture;   /* Time capture structure   */
    int                     ref_count;      /* Reference count.         */
    _bcm_time_user_cb_p     user_cb;        /* User call back info      */
#ifdef INCLUDE_GDPLL
    int                     dpll_chan;
    sal_thread_t            bs_thread;
#endif /* INCLUDE_GDPLL */
} _bcm_time_interface_config_t, *_bcm_time_interface_config_p;
                         

/* Module control structure. */
typedef struct _bcm_time_config_s {
    _bcm_time_interface_config_p    intf_arr;       /* Time Interface config array */
    int                             intf_count;     /* Time interfaces size.       */
    sal_mutex_t                     mutex;          /* Protection mutex.           */
#ifdef INCLUDE_GDPLL
    int                             ntp_dpll_chan;
    int                             ptp_dpll_chan;
#endif /* INCLUDE_GDPLL */
} _bcm_time_config_t, *_bcm_time_config_p;


/* Verify that time module is initialized */
#define TIME_INIT(unit) (NULL != _bcm_time_config[unit])
/* Time control configuration per unit */
#define TIME_CONFIG(unit) (_bcm_time_config[unit])
/* Time interface configuration */
#define TIME_INTERFACE_CONFIG(unit, idx) ((TIME_CONFIG(unit))->intf_arr[idx])
/* Time interface  */
#define TIME_INTERFACE(unit, idx) &TIME_INTERFACE_CONFIG(unit,idx).time_interface
/* Time capture */
#define TIME_CAPTURE(unit, idx) &TIME_INTERFACE_CONFIG(unit, idx).time_capture

/* Time interface configuration reference count */
#define TIME_INTERFACE_CONFIG_REF_COUNT(unit, idx) (TIME_INTERFACE_CONFIG(unit, idx).ref_count)

/* Time module lock */
#define TIME_LOCK(unit) sal_mutex_take(TIME_CONFIG(unit)->mutex, sal_mutex_FOREVER)
/* Time module unlock */
#define TIME_UNLOCK(unit)   sal_mutex_give(TIME_CONFIG(unit)->mutex)

#ifdef INCLUDE_GDPLL

#define BCM_TIME_DPLL_CHAN_INVALID        (0xFFFFFFFF)

#define TIME_DPLL_CHAN(unit, id) TIME_INTERFACE_CONFIG(unit, id).dpll_chan

#define IS_TIME_DPLL_CHAN_VALID(unit, id) (TIME_DPLL_CHAN(unit,id) != BCM_TIME_DPLL_CHAN_INVALID)

#define TIME_DPLL_CHAN_NTP(unit) (TIME_CONFIG(unit)->ntp_dpll_chan)
#define TIME_DPLL_CHAN_PTP(unit) (TIME_CONFIG(unit)->ptp_dpll_chan)

#define UPDATE_DPLL_CHAN_FOR_TOD(unit, time_fmt, chan) \
            ( (time_fmt == bcmTimeFormatNTP) ? \
                (TIME_DPLL_CHAN_NTP(unit)= chan) : \
                (TIME_DPLL_CHAN_PTP(unit)= chan) )

#endif /* INCLUDE_GDPLL */

#define TIME_NANOSEC_2_SEC_ROUND(ns)    ((ns > (TIME_NANOSEC_MAX / 2)) ? 1: 0)

#define TIME_ACCURACY_UNKNOWN               0
#define TIME_ACCURACY_INFINITE              TIME_NANOSEC_MAX
#define TIME_ACCURACY_UNKNOWN_IDX           18
#define TIME_ACCURACY_LOW_HW_VAL            32
#define TIME_ACCURACY_HIGH_HW_VAL           49
#define TIME_ACCURACY_UNKNOWN_HW_VAL        254
 
#define TIME_ACCURACY_HW_2_SW_IDX(val)     ((val == TIME_ACCURACY_UNKNOWN_HW_VAL) ? TIME_ACCURACY_UNKNOWN_IDX : val - TIME_ACCURACY_LOW_HW_VAL)
#define TIME_ACCURACY_SW_IDX_2_HW(idx)     ((idx == TIME_ACCURACY_UNKNOWN_IDX) ? TIME_ACCURACY_UNKNOWN_HW_VAL : idx + TIME_ACCURACY_LOW_HW_VAL)


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_time_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#ifdef BCM_WARM_BOOT_SUPPORT
extern int _bcm_time_scache_sync(int unit);
#endif

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
#define BCM_TIME_READ_NS_REGr(unit, reg, idx, rvp) \
    soc_iproc_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rvp);
#define BCM_TIME_WRITE_NS_REGr(unit, reg, idx, rv) \
    soc_iproc_setreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rv)
#define TS_GPIO_COUNT               6

typedef enum _bcm_time_capture_event_e {
    _bcmTimeCaptureEventCpu = 0,          /* CPU generated input event type */
    _bcmTimeCaptureEventBS0HB = 1,        /* Broadsync0 heartbeat input event type */
    _bcmTimeCaptureEventBS1HB = 2,        /* Broadsync1 heartbeat input event type */
    _bcmTimeCaptureEventGPIO0 = 3,        /* GPIO0 input event type */
    _bcmTimeCaptureEventGPIO1 = 4,        /* GPIO1 input event type */
    _bcmTimeCaptureEventGPIO2 = 5,        /* GPIO2 input event type */
    _bcmTimeCaptureEventGPIO3 = 6,        /* GPIO3 input event type */
    _bcmTimeCaptureEventGPIO4 = 7,        /* GPIO4 input event type */
    _bcmTimeCaptureEventGPIO5 = 8,        /* GPIO5 input event type */
    _bcmTimeCaptureEventIPDM0 = 9,        /* IPDM0 input event type */
    _bcmTimeCaptureEventIPDM1 = 10,       /* IPDM1 input event type */
    _bcmTimeCaptureEventTS1TS = 11,       /* TS1 counter timestamp event type */
    _bcmTimeCaptureEventBS0PLL = 12,      /* Broadsync0 PLL input event type */
    _bcmTimeCaptureEventBS1PLL = 13,      /* Broadsync1 PLL input event type */
    _bcmTimeCaptureEventRSVD1IF = 14,     /* RSVD1 interface input event type */
    _bcmTimeCaptureEventRSVD1RF = 15,     /* BCN counter input event type */
    _bcmTimeCaptureEventBS0ConvTC = 16,   /* Broadsync0 converted timecode input
                                           type */
    _bcmTimeCaptureEventBS1ConvTC = 17,   /* Broadsync1 converted timecode input
                                           type */
    _bcmTimeCaptureEventLCPLL0 = 18,      /* Serdes LCPLL0 input event type */
    _bcmTimeCaptureEventLCPLL1 = 19,      /* Serdes LCPLL1 input event type */
    _bcmTimeCaptureEventLCPLL2 = 20,      /* Serdes LCPLL2 input event type */
    _bcmTimeCaptureEventLCPLL3 = 21,      /* Serdes LCPLL3 input event type */
    _bcmTimeCaptureEventPORT = 22,        /* Input event from port type */
    _bcmTimeCaptureEventR5 = 23,          /* Input event from R5 core */
    _bcmTimeCaptureEventMax = 24          /* Input event max value */
} _bcm_time_capture_event_t;

typedef enum _bcm_time_capture_dest_e {
    _bcmTimeCaptureDestCPU = 0,   /* Event forward to CPU FIFO */
    _bcmTimeCaptureDestM7 = 1,    /* Event forward to GDPLL IA */
    _bcmTimeCaptureDestALL = 2    /* Event forward to both */
} _bcm_time_capture_dest_t;

typedef struct _bcm_time_capture_input_s {
    _bcm_time_capture_event_t input_event; /* Input event type for the channel */
    bcm_port_t port;                    /* Event input from the port */
    bcm_time_port_event_t port_event_type; /* Event input type from the port */
    uint32 event_divisor;               /* Divisor value for an input event */
    _bcm_time_capture_dest_t event_dest;  /* Input event forwarding destination
                                           type */
    int ts_counter;                     /* 0: TS0, 1:TS1 */
    int ppm_check_enable;               /* PPM check enable flag, boolean */
} _bcm_time_capture_input_t;

/* Internal datastructure for event destination config */
typedef struct _bcm_time_capture_dest_cfg_s {
    _bcm_time_capture_dest_t event_dest;
    int                 ts_counter;       /* 0: TS0     1:TS1*/
    int                 ppm_check_enable; /* PPM check enable */
} _bcm_time_capture_dest_cfg_t;

/* NS Interrupts */
#define INTR_TS_FIFO_NOT_EMPTY      (1<<0)  /* TS_FIFO fill */
#define INTR_TS_FIFO_OVERFLOW       (1<<1)  /* TS_FIFO overflow */
#define INTR_NS_TIME_CAPTURE  (INTR_TS_FIFO_NOT_EMPTY |   \
                                INTR_TS_FIFO_OVERFLOW)
#define INTR_TS0_CNT_OFFSET_UPDATED (1<<2)  /* TS0_CNT_OFFSET_UPDATED */
#define INTR_TS1_CNT_OFFSET_UPDATED (1<<3)  /* TS1_CNT_OFFSET_UPDATED */

#define BCM_TIME_CAPTURE_NUM_INPUT_EVENTS       256
#define BCM_TIME_CAPTURE_IA_START_TXPI_TXSOF    128
#define BCM_TIME_CAPTURE_IA_START_MISC          192
#define BCM_TIME_CAPTURE_IA_START_R5            214

#define BCM_TIME_CAPTURE_MISC_EVENT_EN_CPU     (1<<0)
#define BCM_TIME_CAPTURE_MISC_EVENT_EN_BS0HB   (1<<1)
#define BCM_TIME_CAPTURE_MISC_EVENT_EN_BS1HB   (1<<2)
#define BCM_TIME_CAPTURE_MISC_EVENT_EN_IPDM0   (1<<3)
#define BCM_TIME_CAPTURE_MISC_EVENT_EN_IPDM1   (1<<4)
#define BCM_TIME_CAPTURE_MISC_EVENT_EN_TS1     (1<<5)
#define BCM_TIME_CAPTURE_MISC_EVENT_EN_RSVD1IF   (1<<6)
#define BCM_TIME_CAPTURE_MISC_EVENT_EN_RSVD1RF   (1<<7)
#define BCM_TIME_CAPTURE_MISC_EVENT_EN_BS0CONV (1<<8)
#define BCM_TIME_CAPTURE_MISC_EVENT_EN_BS1CONV (1<<9)
#define BCM_TIME_CAPTURE_NUM_PORTS         64         /* NS supports 64 ports */
#define BCM_TIME_CAPTURE_NUM_CPRI_PORTS    32         /* NS supports 32 CPRI ports */

#define SOC_TIME_NS_INTR_POS       (1<<24)

#define BCM_TIME_CAPTURE_HANDLE_INVALID        (0xFFFFFFFF)
#define BCM_TIME_CAPTURE_MAX_EVENT_ID          (215)
#define BCM_TIME_CAPTURE_EVENT_STATE_NOT_IN_USE    0x0
#define BCM_TIME_CAPTURE_EVENT_STATE_IN_USE        0x1

#define BCM_TIME_CAPTURE_MAX_FIFO_DATA_SIZE    (2574)

#elif defined(BCM_PETRA_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)

#define BCM_TIME_CAPTURE_MAX_FIFO_DATA_SIZE    (2574)

#endif /* BCM_TIME_NANOSYNC_SUPPORT */

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
#define BCM_TIME_TS_CAPTURE_BUF_SIZE           (400000)
#elif defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
#define BCM_TIME_TS_CAPTURE_BUF_SIZE           (200000)
#endif /* BCM_TIMESYNC_TIME_CAPTURE_SUPPORT */

#endif /* _BCM_INT_COMMON_TIME_H */

