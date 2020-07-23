/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       time.c
 *
 * Remarks:    Broadcom StrataSwitch Time BroadSync API
 *
 * Functions:
 *      Private Interface functions
 *
 *      _bcm_esw_time_hw_clear
 *      _bcm_esw_time_deinit
 *      _bcm_esw_time_reinit
 *      _bcm_esw_time_interface_id_validate
 *      _bcm_esw_time_interface_input_validate
 *      _bcm_esw_time_interface_allocate_id
 *      _bcm_esw_time_interface_heartbeat_install
 *      _bcm_esw_time_interface_accuracy_time2hw
 *      _bcm_esw_time_interface_drift_install
 *      _bcm_esw_time_interface_offset_install
 *      _bcm_esw_time_interface_ref_clock_install
 *      _bcm_esw_time_interface_install
 *      _bcm_esw_time_interface_dual_bs_install
 *      _bcm_time_interface_free
 *      _bcm_esw_time_accuracy_parse
 *      _bcm_esw_time_input_parse
 *      _bcm_esw_time_capture_counter_read
 *      _bcm_esw_time_capture_get
 *      _bcm_esw_time_interface_offset_get
 *      _bcm_esw_time_interface_drift_get
 *      _bcm_esw_time_interface_ref_clock_get
 *      _bcm_esw_time_interface_accuracy_get
 *      _bcm_esw_time_interface_get
 *      _bcm_esw_time_interface_dual_bs_get
 *      _bcm_esw_time_hw_interrupt_dflt
 *      _bcm_esw_time_hw_interrupt
 *      _bcm_esw_time_trigger_to_timestamp_mode
 *      _bcm_esw_time_trigger_from_timestamp_mode
 *      _bcm_esw_time_bs_init
 *      _bcm_time_sw_dump
 *
 *      Public Interface functions
 *
 *      bcm_esw_time_init
 *      bcm_esw_time_deinit
 *      bcm_esw_time_interface_add
 *      bcm_esw_time_interface_delete
 *      bcm_esw_time_interface_get
 *      bcm_esw_time_interface_delete_all
 *      bcm_esw_time_interface_traverse
 *      bcm_esw_time_capture_get
 *      bcm_esw_time_heartbeat_enable_set
 *      bcm_esw_time_heartbeat_enable_get
 *      bcm_esw_time_trigger_enable_set
 *      bcm_esw_time_trigger_enable_get
 *      bcm_esw_time_heartbeat_register
 *      bcm_esw_time_heartbeat_unregister
 */

#include <shared/util.h>
#include <shared/bsl.h>
#include <bcm/time.h>
#include <bcm/error.h>

#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/iproc.h>
#include <soc/drv.h>
#include <soc/cmicm.h>

#include <bcm/port.h>

#include <bcm_int/common/time.h>
#include <bcm_int/common/time-mbox.h>
#include <bcm_int/common/mbox.h>

#include <bcm/gdpll.h>
#include <bcm_int/esw/gdpll.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/common/ptp.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
#include <soc/phy/phyctrl.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_HELIX5_SUPPORT)
#include <soc/helix5.h>
#endif /* BCM_HELIX5_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT */
#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#endif /* BCM_FIRELIGHT_SUPPORT */
#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_internal.h>
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif
#include <bcm_int/esw/switch.h>

#ifdef BCM_ESMC_EXTDPLL_SUPPORT
#include <bcm_int/common/esmc.h>
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */

#define BROAD_SYNC_TIME_CAPTURE_TIMEOUT      (10) /* useconds */
#define BROAD_SYNC_OUTPUT_TOGGLE_TIME_DELAY  (3)  /* seconds */

#define SYNT_TIME_SECONDS(unit, id) \
        TIME_INTERFACE_CONFIG(unit, id).time_capture.syntonous.seconds
#define SYNT_TIME_NANOSECONDS(unit, id) \
        TIME_INTERFACE_CONFIG(unit, id).time_capture.syntonous.nanoseconds

#if defined(BCM_HITLESS_RESET_SUPPORT)
#define HITLESS_REG(cmicd_reg_write)           \
    do {                                       \
        /* Double-writes per CMICD-110 WAR. */ \
        cmicd_reg_write;                       \
        cmicd_reg_write;                       \
    } while (0)

#else
#define HITLESS_REG(cmicd_reg_write)           \
    do {                                       \
        cmicd_reg_write;                       \
    } while (0)

#endif /* defined(BCM_HITLESS_RESET_SUPPORT) */

#if (defined(BCM_TIME_V3_SUPPORT) ||                                    \
     defined(BCM_KATANA2_SUPPORT) || defined(BCM_HELIX4_SUPPORT) ||      \
     defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
     defined(BCM_SABER2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) ||      \
     defined(BCM_HURRICANE3_SUPPORT) || defined(BCM_MONTEREY_SUPPORT) || \
     defined(BCM_FIRELIGHT_SUPPORT))
    #define BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC (1)
#endif

#define SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)      \
    ((soc_feature(unit, soc_feature_time_v3)) ||              \
     (SOC_IS_KATANA2(unit)) || (SOC_IS_HELIX4(unit)) ||       \
     (SOC_IS_HURRICANE2(unit)) || (SOC_IS_GREYHOUND(unit)) || \
     (SOC_IS_SABER2(unit)) || (SOC_IS_TOMAHAWKX(unit)) ||      \
     (SOC_IS_HURRICANE3(unit)) || (SOC_IS_GREYHOUND2(unit)) || (SOC_IS_MONTEREY(unit)) || \
     (SOC_IS_FIRELIGHT(unit)))

#if (defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC) || \
     defined(BCM_KATANA_SUPPORT) ||             \
     defined(BCM_TRIUMPH3_SUPPORT) ||           \
     defined(BCM_TRIDENT2_SUPPORT))
#define BCM_TIME_MANAGED_BROADSYNC (1)
#endif

#define SOC_HAS_MANAGED_BROADSYNC(unit)                  \
    (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit) || \
     SOC_IS_KATANA(unit) ||                              \
     SOC_IS_TRIUMPH3(unit) ||                            \
     SOC_IS_TD2_TT2(unit))

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION  BCM_WB_VERSION_1_0
#endif  /* BCM_WARM_BOOT_SUPPORT */

#define IS_BROADSYNC0(x) (x == 0)
#define IS_BROADSYNC1(x) (x == 1)

/****************************************************************************/
/*                      GLOBAL VARIABLES                                    */
/****************************************************************************/
bcm_time_spec_t tod;
int64 hwts;

/****************************************************************************/
/*                      LOCAL VARIABLES DECLARATION                         */
/****************************************************************************/
#if defined(BCM_GREYHOUND2_SUPPORT)
/* 8 TSC (TSC0~6, TSCF0) */
static const int gh2_tsc_phy_port[] = {58, 62, 66, 70, 74, 78, 82, 86};
#endif /* defined(BCM_GREYHOUND2_SUPPORT) */

static _bcm_time_config_p _bcm_time_config[BCM_MAX_NUM_UNITS] = {NULL};

static bcm_time_spec_t _bcm_time_accuracy_arr[TIME_ACCURACY_CLK_MAX] = {
      {0, COMPILER_64_INIT(0,0),  25},        /* HW value = 32, accuracy up tp 25 nanosec */
      {0, COMPILER_64_INIT(0,0),  100},       /* HW value = 33, accuracy up to 100 nanosec */
      {0, COMPILER_64_INIT(0,0),  250},       /* HW value = 34, accuracy up to 250 nanosec */
      {0, COMPILER_64_INIT(0,0),  1000},      /* HW value = 35, accuracy up to 1 microsec */
      {0, COMPILER_64_INIT(0,0),  2500},      /* HW value = 36, accuracy up to 2.5 microsec */
      {0, COMPILER_64_INIT(0,0),  10000},     /* HW value = 37, accuracy up to 10 microsec */
      {0, COMPILER_64_INIT(0,0),  25000},     /* HW value = 38, accuracy up to 25 microsec */
      {0, COMPILER_64_INIT(0,0),  100000},    /* HW value = 39, accuracy up to 100 microsec */
      {0, COMPILER_64_INIT(0,0),  250000},    /* HW value = 40, accuracy up to 250 microsec */
      {0, COMPILER_64_INIT(0,0),  1000000},   /* HW value = 41, accuracy up to 1 milisec */
      {0, COMPILER_64_INIT(0,0),  2500000},   /* HW value = 42, accuracy up to 2.5 milisec */
      {0, COMPILER_64_INIT(0,0),  10000000},  /* HW value = 43, accuracy up to 10 milisec */
      {0, COMPILER_64_INIT(0,0),  25000000},  /* HW value = 44, accuracy up to 25 milisec */
      {0, COMPILER_64_INIT(0,0),  100000000}, /* HW value = 45, accuracy up to 100 milisec */
      {0, COMPILER_64_INIT(0,0),  250000000}, /* HW value = 46, accuracy up to 250 milisec */
      {0, COMPILER_64_INIT(0,1),  0},         /* HW value = 47, accuracy up to 1 sec */
      {0, COMPILER_64_INIT(0,10), 0},        /* HW value = 48, accuracy up to 10 sec */
      /* HW value = 49, accuracy greater than 10 sec */
      {0, COMPILER_64_INIT(0,TIME_ACCURACY_INFINITE), TIME_ACCURACY_INFINITE},
      /* HW value = 254 accuracy unknown */
      {0, COMPILER_64_INIT(0,TIME_ACCURACY_UNKNOWN), TIME_ACCURACY_UNKNOWN}
};

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
STATIC
soc_reg_t  _bcm_time_gpio_regs[TS_GPIO_COUNT] = {
    NS_TIMESYNC_GPIO_0_CTRLr,
    NS_TIMESYNC_GPIO_1_CTRLr,
    NS_TIMESYNC_GPIO_2_CTRLr,
    NS_TIMESYNC_GPIO_3_CTRLr,
    NS_TIMESYNC_GPIO_4_CTRLr,
    NS_TIMESYNC_GPIO_5_CTRLr};

STATIC
soc_reg_t  _bcm_time_gpio_iregs[TS_GPIO_COUNT] = {
    IPROC_NS_TIMESYNC_GPIO_0_CTRLr,
    IPROC_NS_TIMESYNC_GPIO_1_CTRLr,
    IPROC_NS_TIMESYNC_GPIO_2_CTRLr,
    IPROC_NS_TIMESYNC_GPIO_3_CTRLr,
    IPROC_NS_TIMESYNC_GPIO_4_CTRLr,
    IPROC_NS_TIMESYNC_GPIO_5_CTRLr};

STATIC
soc_reg_t _bcm_time_bs_pll_regs[2] = {
    NS_BROADSYNC0_CLK_EVENT_CTRLr,
    NS_BROADSYNC1_CLK_EVENT_CTRLr};

STATIC
soc_reg_t _bcm_time_bs_pll_iregs[2] = {
    IPROC_NS_BROADSYNC0_CLK_EVENT_CTRLr,
    IPROC_NS_BROADSYNC1_CLK_EVENT_CTRLr};

STATIC
soc_reg_t __bcm_time_mapper_port_enable_regs[4] = { NS_TIMESYNC_MAPPER_PORT_ENABLE_0r,
                                         NS_TIMESYNC_MAPPER_PORT_ENABLE_1r,
                                         NS_TIMESYNC_MAPPER_PORT_ENABLE_2r,
                                         NS_TIMESYNC_MAPPER_PORT_ENABLE_3r
                                       };

STATIC
soc_reg_t _bcm_time_mapper_port_enable[32] = { PORT0_TS_ENABLEf, PORT1_TS_ENABLEf,
                                     PORT2_TS_ENABLEf, PORT3_TS_ENABLEf,
                                     PORT4_TS_ENABLEf, PORT5_TS_ENABLEf,
                                     PORT6_TS_ENABLEf, PORT7_TS_ENABLEf,

                                     PORT8_TS_ENABLEf, PORT9_TS_ENABLEf,
                                     PORT10_TS_ENABLEf, PORT11_TS_ENABLEf,
                                     PORT12_TS_ENABLEf, PORT13_TS_ENABLEf,
                                     PORT14_TS_ENABLEf, PORT15_TS_ENABLEf,

                                     PORT16_TS_ENABLEf, PORT17_TS_ENABLEf,
                                     PORT18_TS_ENABLEf, PORT19_TS_ENABLEf,
                                     PORT20_TS_ENABLEf, PORT21_TS_ENABLEf,
                                     PORT22_TS_ENABLEf, PORT23_TS_ENABLEf,

                                     PORT24_TS_ENABLEf, PORT25_TS_ENABLEf,
                                     PORT26_TS_ENABLEf, PORT27_TS_ENABLEf,
                                     PORT28_TS_ENABLEf, PORT29_TS_ENABLEf,
                                     PORT30_TS_ENABLEf, PORT31_TS_ENABLEf
                                  };


STATIC
uint8 _bcm_time_capture_event_state[BCM_TIME_CAPTURE_MAX_EVENT_ID];
STATIC uint8    tsfifo_flag = 0;

STATIC const int
_bcm_time_capture_event_supported_mo[_bcmTimeCaptureEventMax] = {
    TRUE, /* _bcmTimeCaptureEventCpu = 0 */
    TRUE, /* _bcmTimeCaptureEventBS0HB */
    TRUE, /* _bcmTimeCaptureEventBS1HB */
    TRUE, /* _bcmTimeCaptureEventGPIO0 */
    TRUE, /* _bcmTimeCaptureEventGPIO1 */
    TRUE, /* _bcmTimeCaptureEventGPIO2 */
    TRUE, /* _bcmTimeCaptureEventGPIO3 */
    TRUE, /* _bcmTimeCaptureEventGPIO4 */
    TRUE, /* _bcmTimeCaptureEventGPIO5 */
    TRUE, /* _bcmTimeCaptureEventIPDM0 */
    TRUE, /* _bcmTimeCaptureEventIPDM1 */
    TRUE, /* _bcmTimeCaptureEventTS1TS */
    TRUE, /* _bcmTimeCaptureEventBS0PLL */
    TRUE, /* _bcmTimeCaptureEventBS1PLL */
    TRUE, /* _bcmTimeCaptureEventRSVD1IF */
    TRUE, /* _bcmTimeCaptureEventRSVD1RF */
    TRUE, /* _bcmTimeCaptureEventBS0ConvTC */
    TRUE, /* _bcmTimeCaptureEventBS1ConvTC */
    TRUE, /* _bcmTimeCaptureEventLCPLL0 */
    TRUE, /* _bcmTimeCaptureEventLCPLL1 */
    TRUE, /* _bcmTimeCaptureEventLCPLL2 */
    TRUE, /* _bcmTimeCaptureEventLCPLL3 */
    TRUE, /* _bcmTimeCaptureEventPORT */
    TRUE, /* _bcmTimeCaptureEventR5 */
};

#if defined(BCM_TOMAHAWK3_SUPPORT)
STATIC const int
_bcm_time_capture_event_supported_th3[_bcmTimeCaptureEventMax] = {
    TRUE,  /* _bcmTimeCaptureEventCpu */
    TRUE,  /* _bcmTimeCaptureEventBS0HB */
    TRUE,  /* _bcmTimeCaptureEventBS1HB */
    TRUE,  /* _bcmTimeCaptureEventGPIO0 */
    TRUE,  /* _bcmTimeCaptureEventGPIO1 */
    TRUE,  /* _bcmTimeCaptureEventGPIO2 */
    TRUE,  /* _bcmTimeCaptureEventGPIO3 */
    TRUE,  /* _bcmTimeCaptureEventGPIO4 */
    TRUE,  /* _bcmTimeCaptureEventGPIO5 */
    TRUE,  /* _bcmTimeCaptureEventIPDM0 */
    TRUE,  /* _bcmTimeCaptureEventIPDM1 */
    TRUE,  /* _bcmTimeCaptureEventTS1TS */
    TRUE,  /* _bcmTimeCaptureEventBS0PLL */
    TRUE,  /* _bcmTimeCaptureEventBS1PLL */
    FALSE, /* _bcmTimeCaptureEventRSVD1IF */
    FALSE, /* _bcmTimeCaptureEventRSVD1RF */
    TRUE,  /* _bcmTimeCaptureEventBS0ConvTC */
    TRUE,  /* _bcmTimeCaptureEventBS1ConvTC */
    TRUE,  /* _bcmTimeCaptureEventLCPLL0 */
    TRUE,  /* _bcmTimeCaptureEventLCPLL1 */
    TRUE,  /* _bcmTimeCaptureEventLCPLL2 */
    TRUE,  /* _bcmTimeCaptureEventLCPLL3 */
    FALSE, /* _bcmTimeCaptureEventPORT */
    FALSE, /* _bcmTimeCaptureEventR5 */
};
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_HURRICANE4_SUPPORT)
STATIC const int
_bcm_time_capture_event_supported_hr4[_bcmTimeCaptureEventMax] = {
    TRUE,  /* _bcmTimeCaptureEventCpu */
    TRUE,  /* _bcmTimeCaptureEventBS0HB */
    TRUE,  /* _bcmTimeCaptureEventBS1HB */
    TRUE,  /* _bcmTimeCaptureEventGPIO0 */
    TRUE,  /* _bcmTimeCaptureEventGPIO1 */
    TRUE,  /* _bcmTimeCaptureEventGPIO2 */
    TRUE,  /* _bcmTimeCaptureEventGPIO3 */
    TRUE,  /* _bcmTimeCaptureEventGPIO4 */
    TRUE,  /* _bcmTimeCaptureEventGPIO5 */
    TRUE,  /* _bcmTimeCaptureEventIPDM0 */
    TRUE,  /* _bcmTimeCaptureEventIPDM1 */
    TRUE,  /* _bcmTimeCaptureEventTS1TS */
    TRUE,  /* _bcmTimeCaptureEventBS0PLL */
    TRUE,  /* _bcmTimeCaptureEventBS1PLL */
    FALSE, /* _bcmTimeCaptureEventRSVD1IF */
    FALSE, /* _bcmTimeCaptureEventRSVD1RF */
    TRUE,  /* _bcmTimeCaptureEventBS0ConvTC */
    TRUE,  /* _bcmTimeCaptureEventBS1ConvTC */
    FALSE, /* _bcmTimeCaptureEventLCPLL0 */
    FALSE, /* _bcmTimeCaptureEventLCPLL1 */
    FALSE, /* _bcmTimeCaptureEventLCPLL2 */
    FALSE, /* _bcmTimeCaptureEventLCPLL3 */
    FALSE, /* _bcmTimeCaptureEventPORT */
    FALSE, /* _bcmTimeCaptureEventR5 */
};
#endif /* BCM_HURRICANE4_SUPPORT */

#if defined(BCM_FIREBOLT6_SUPPORT)
STATIC const int
_bcm_time_capture_event_supported_fb6[_bcmTimeCaptureEventMax] = {
    TRUE,  /* _bcmTimeCaptureEventCpu */
    TRUE,  /* _bcmTimeCaptureEventBS0HB */
    TRUE,  /* _bcmTimeCaptureEventBS1HB */
    TRUE,  /* _bcmTimeCaptureEventGPIO0 */
    TRUE,  /* _bcmTimeCaptureEventGPIO1 */
    TRUE,  /* _bcmTimeCaptureEventGPIO2 */
    TRUE,  /* _bcmTimeCaptureEventGPIO3 */
    TRUE,  /* _bcmTimeCaptureEventGPIO4 */
    TRUE,  /* _bcmTimeCaptureEventGPIO5 */
    TRUE,  /* _bcmTimeCaptureEventIPDM0 */
    TRUE,  /* _bcmTimeCaptureEventIPDM1 */
    TRUE,  /* _bcmTimeCaptureEventTS1TS */
    TRUE,  /* _bcmTimeCaptureEventBS0PLL */
    TRUE,  /* _bcmTimeCaptureEventBS1PLL */
    FALSE, /* _bcmTimeCaptureEventRSVD1IF */
    FALSE, /* _bcmTimeCaptureEventRSVD1RF */
    TRUE,  /* _bcmTimeCaptureEventBS0ConvTC */
    TRUE,  /* _bcmTimeCaptureEventBS1ConvTC */
    FALSE, /* _bcmTimeCaptureEventLCPLL0 */
    FALSE, /* _bcmTimeCaptureEventLCPLL1 */
    FALSE, /* _bcmTimeCaptureEventLCPLL2 */
    FALSE, /* _bcmTimeCaptureEventLCPLL3 */
    FALSE, /* _bcmTimeCaptureEventPORT */
    FALSE, /* _bcmTimeCaptureEventR5 */
};
#endif /* BCM_FIREBOLT6_SUPPORT */

#if defined(BCM_FIRELIGHT_SUPPORT)
STATIC const int
_bcm_time_capture_event_supported_fl[_bcmTimeCaptureEventMax] = {
    TRUE,  /* _bcmTimeCaptureEventCpu */
    TRUE,  /* _bcmTimeCaptureEventBS0HB */
    TRUE,  /* _bcmTimeCaptureEventBS1HB */
    TRUE,  /* _bcmTimeCaptureEventGPIO0 */
    TRUE,  /* _bcmTimeCaptureEventGPIO1 */
    TRUE,  /* _bcmTimeCaptureEventGPIO2 */
    TRUE,  /* _bcmTimeCaptureEventGPIO3 */
    TRUE,  /* _bcmTimeCaptureEventGPIO4 */
    TRUE,  /* _bcmTimeCaptureEventGPIO5 */
    TRUE,  /* _bcmTimeCaptureEventIPDM0 */
    TRUE,  /* _bcmTimeCaptureEventIPDM1 */
    TRUE,  /* _bcmTimeCaptureEventTS1TS */
    TRUE,  /* _bcmTimeCaptureEventBS0PLL */
    TRUE,  /* _bcmTimeCaptureEventBS1PLL */
    FALSE, /* _bcmTimeCaptureEventRSVD1IF */
    FALSE, /* _bcmTimeCaptureEventRSVD1RF */
    TRUE,  /* _bcmTimeCaptureEventBS0ConvTC */
    TRUE,  /* _bcmTimeCaptureEventBS1ConvTC */
    TRUE,  /* _bcmTimeCaptureEventLCPLL0 */
    FALSE, /* _bcmTimeCaptureEventLCPLL1 */
    FALSE, /* _bcmTimeCaptureEventLCPLL2 */
    FALSE, /* _bcmTimeCaptureEventLCPLL3 */
    FALSE, /* _bcmTimeCaptureEventPORT */
    FALSE, /* _bcmTimeCaptureEventR5 */
};
#endif /* BCM_FIRELIGHT_SUPPORT */

#endif /* BCM_TIME_NANOSYNC_SUPPORT */

/****************************************************************************/
/*                     Internal functions implementation                    */
/****************************************************************************/

STATIC int
_bcm_esw_time_capture_get (int unit, bcm_time_if_t id, bcm_time_capture_t *time, uint32 flags);
STATIC int
_bcm_esw_time_interface_offset_get(int unit, bcm_time_if_t id, bcm_time_spec_t *offset);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HELIX4_SUPPORT) || \
    defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
    defined(BCM_SABER2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
STATIC int
_bcm_esw_time_bs_init(int unit, bcm_time_interface_t *intf);
STATIC int
_bcm_esw_time_hw_clear(int unit, bcm_time_if_t intf_id);
STATIC int
_bcm_esw_time_hitless_reset(int unit, bcm_time_if_t intf_id);
#endif

#if defined(INCLUDE_GDPLL)
extern void gdpll_intr(int unit, uint32 int_status, uint32 int_enable);
#endif

STATIC int
_bcm_esw_time_synce_clock_source_squelch_set(int unit, bcm_time_synce_clock_src_type_t clk_src,
                            int squelch);
STATIC int
_bcm_esw_time_synce_clock_source_squelch_get(int unit, bcm_time_synce_clock_src_type_t clk_src,
                            int *squelch);

STATIC int
_bcm_esw_time_synce_clock_source_frequency_set(int unit, bcm_time_synce_clock_source_config_t *clk_src_config,
                            int frequency);
STATIC int
_bcm_esw_time_synce_clock_source_frequency_get(int unit, bcm_time_synce_clock_source_config_t *clk_src_config,
                            int *frequency);

#ifdef INCLUDE_GDPLL
int _bcm_bs_dpll_chan_create(int unit, bcm_time_interface_t *intf);
int _bcm_bs_dpll_chan_destroy(int unit, int bs_id);
#endif

typedef enum bcmi_time_port_mode_e {
    bcmi_time_port_mode_single = 0,
    bcmi_time_port_mode_dual = 1,
    bcmi_time_port_mode_tri = 2,
    bcmi_time_port_mode_quad = 3,
    bcmi_time_port_mode_invalid = 4
} bcmi_time_port_mode_t;

#ifdef PORTMOD_SUPPORT
/* Get port mode by giving logical port. */
STATIC void
bcmi_esw_time_port_mode_get(
    int unit,
    phymod_dispatch_type_t dispatch_type,
    int lport,
    bcmi_time_port_mode_t *port_mode)
{
    if (port_mode == NULL) {
        return;
    }

    *port_mode = bcmi_time_port_mode_invalid;
#ifdef PHYMOD_TSCF16_GEN3_SUPPORT
    if (dispatch_type == phymodDispatchTypeTscf16_gen3) {
#if defined(BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_FIRELIGHT(unit)) {
            int mode;

            soc_firelight_port_mode_get(unit, lport, &mode);
            if (mode == SOC_FL_PORT_MODE_QUAD) {
                /* 4 lane for single mode. */
                *port_mode = bcmi_time_port_mode_single;
            } else if (mode == SOC_FL_PORT_MODE_DUAL) {
                *port_mode = bcmi_time_port_mode_dual;
            } else if (mode == SOC_FL_PORT_MODE_SINGLE) {
                /* single lane for quad mode. */
                *port_mode = bcmi_time_port_mode_quad;
            } else if (mode == SOC_FL_PORT_MODE_TRI_023 ||
                       mode == SOC_FL_PORT_MODE_TRI_012) {
                *port_mode = bcmi_time_port_mode_tri;
            }
        }
#endif /* BCM_FIRELIGHT_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
        if (SOC_IS_FIREBOLT6(unit)) {
            int mode;

            soc_firebolt6_port_mode_get(unit, lport, &mode);
            if (mode == SOC_FB6_PORT_MODE_QUAD) {
                /* 4 lane for single mode. */
                *port_mode = bcmi_time_port_mode_single;
            } else if (mode == SOC_FB6_PORT_MODE_DUAL) {
                *port_mode = bcmi_time_port_mode_dual;
            } else if (mode == SOC_FB6_PORT_MODE_SINGLE) {
                /* single lane for quad mode. */
                *port_mode = bcmi_time_port_mode_quad;
            } else if (mode == SOC_FB6_PORT_MODE_TRI_023 ||
                       mode == SOC_FB6_PORT_MODE_TRI_012) {
                *port_mode = bcmi_time_port_mode_tri;
            }
        }
#endif /* BCM_FIREBOLT6_SUPPORT */
    }
#endif /* PHYMOD_TSCF16_GEN3_SUPPORT */
}
#endif /* PORTMOD_SUPPORT */

/* Check if lport is in gmii mode or not, and get the mode. */
STATIC int
bcmi_esw_time_port_is_gmii_mode(
    int unit,
    int lport,
    int *gmii_mode)
{
    int mode = 0;

#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5(unit)) {
        soc_helix5_port_qsgmii_mode_get(unit, lport, &mode);
    }
#endif /* BCM_HELIX5_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
    if (SOC_IS_HURRICANE4(unit)) {
        soc_hurricane4_port_qsgmii_mode_get(unit, lport, &mode);
    }
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        soc_fl_port_qsgmii_mode_get(unit, lport, &mode);
    }
#endif /* BCM_FIRELIGHT_SUPPORT */
    if (gmii_mode) {
        *gmii_mode = mode;
    }
    return (mode) ? TRUE : FALSE;
}

/* Get miscellaneous events start and end. */
STATIC int
bcmi_time_capture_event_misc_events_range(
    int unit,
    int *misc_start,
    int *misc_end)
{
    int start, end;
#if defined(BCM_HURRICANE4_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT)
    if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        /*
         * NS_TIMESYNC_TS_EVENT_FWD_CFG - 36 Numels
         * no NS_TIMESYNC_MAPPER_FWD_CFG.
         *
         * We have 36 numels of NS_TIMESYNC_TS_EVENT_FWD_CFG and no
         * NS_TIMESYNC_MAPPER_FWD_CFG, the miscellaneous events start
         * from index 0 per table 7 in uArch.
         */
        start = 0;
        end = 36;
    } else
#endif /* BCM_HURRICANE4_SUPPORT || BCM_FIREBOLT6_SUPPORT */
    {
        /*
         * NS_TIMESYNC_TS_EVENT_FWD_CFG - 150 Numels
         * NS_TIMESYNC_MAPPER_FWD_CFG - 128 Numels
         *
         * Since we have 150 numels of NS_TIMESYNC_TS_EVENT_FWD_CFG,
         * the miscellaneous events starts from index 128 per table 6.4
         * in uArch.
         */
        start = 128;
        end = 150;
    }
    if (misc_start) {
        *misc_start = start;
    }
    if (misc_end) {
        *misc_end = end;
    }
    return BCM_E_NONE;
}

STATIC int _bcm_esw_time_ts_offset_reset(int unit, int ts_counter);
/*
 * Function:
 *    _bcm_esw_time_hw_clear
 * Purpose:
 *    Internal routine used to clear all HW registers and table to default values
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf_id        - (IN) Time interface identifier
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_hw_clear(int unit, bcm_time_if_t intf_id)
{
    uint32 regval;
#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    /*****************************************************************************/
    /*   Perform KATANA2, HELIX4, HURRICANE2, GREYHOUND, SABER2 IPROC HW configuration   */
    /*****************************************************************************/

    /* Reset Config register */
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        if (!soc_feature(unit, soc_feature_time_v3_no_bs)) {
#if defined(BCM_MONTEREY_SUPPORT)
            if(SOC_IS_MONTEREY(unit)) {
                if(IS_BROADSYNC0(intf_id)) {
                    READ_NS_BS0_BS_CONFIGr(unit, &regval);
                    soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
                    soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                    soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                    HITLESS_REG(WRITE_NS_BS0_BS_CONFIGr(unit, regval));

                    READ_NS_BS0_BS_OUTPUT_TIME_0r(unit, &regval);
                    soc_reg_field_set(unit, NS_BS0_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
                    soc_reg_field_set(unit, NS_BS0_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
                    soc_reg_field_set(unit, NS_BS0_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
                    WRITE_NS_BS0_BS_OUTPUT_TIME_0r(unit, regval);

                    /* Reset Clock Control */
                    READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
                    soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 0);
                    HITLESS_REG(WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval));

                    /* Reset HeartBeat */
                    READ_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
                    soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
                    HITLESS_REG(WRITE_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval));
                } else if(IS_BROADSYNC1(intf_id)) {
                    READ_NS_BS1_BS_CONFIGr(unit, &regval);
                    soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
                    soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                    soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                    HITLESS_REG(WRITE_NS_BS1_BS_CONFIGr(unit, regval));

                    READ_NS_BS1_BS_OUTPUT_TIME_0r(unit, &regval);
                    soc_reg_field_set(unit, NS_BS1_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
                    soc_reg_field_set(unit, NS_BS1_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
                    soc_reg_field_set(unit, NS_BS1_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
                    WRITE_NS_BS1_BS_OUTPUT_TIME_0r(unit, regval);

                    READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
                    soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 0);
                    HITLESS_REG(WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval));

                    READ_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
                    soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
                    HITLESS_REG(WRITE_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval));
                }
            } else 
#endif /* BCM_MONTEREY_SUPPORT */
            if(SOC_IS_TOMAHAWK3(unit) ||
               SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                READ_NS_BS0_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                HITLESS_REG(WRITE_NS_BS0_BS_CONFIGr(unit, regval));

                READ_NS_BS1_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                HITLESS_REG(WRITE_NS_BS1_BS_CONFIGr(unit, regval));

                READ_NS_BS0_BS_OUTPUT_TIME_0r(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
                soc_reg_field_set(unit, NS_BS0_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
                soc_reg_field_set(unit, NS_BS0_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
                WRITE_NS_BS0_BS_OUTPUT_TIME_0r(unit, regval);

                READ_NS_BS1_BS_OUTPUT_TIME_0r(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
                soc_reg_field_set(unit, NS_BS1_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
                soc_reg_field_set(unit, NS_BS1_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
                WRITE_NS_BS1_BS_OUTPUT_TIME_0r(unit, regval);

                /* Reset Clock Control */
                READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 0);
                HITLESS_REG(WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval));

                READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 0);
                HITLESS_REG(WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval));

                /* Reset HeartBeat */
                READ_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
                HITLESS_REG(WRITE_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval));

                READ_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
                HITLESS_REG(WRITE_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval));
            } else if (SOC_IS_FIRELIGHT(unit)) {
                READ_IPROC_NS_BS0_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval, MODEf,
                                  TIME_MODE_OUTPUT);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                                  BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                                  BS_CLK_OUTPUT_ENABLEf, 0);
                HITLESS_REG(WRITE_IPROC_NS_BS0_BS_CONFIGr(unit, regval));

                READ_IPROC_NS_BS1_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval, MODEf,
                                  TIME_MODE_OUTPUT);
                soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval,
                                  BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval,
                                  BS_CLK_OUTPUT_ENABLEf, 0);
                HITLESS_REG(WRITE_IPROC_NS_BS1_BS_CONFIGr(unit, regval));

                READ_IPROC_NS_BS0_BS_OUTPUT_TIME_0r(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r, &regval,
                                  ACCURACYf, 0);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r, &regval,
                                  LOCKf, 0);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r, &regval,
                                  EPOCHf, 0);
                WRITE_IPROC_NS_BS0_BS_OUTPUT_TIME_0r(unit, regval);

                READ_IPROC_NS_BS1_BS_OUTPUT_TIME_0r(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_BS1_BS_OUTPUT_TIME_0r, &regval,
                                  ACCURACYf, 0);
                soc_reg_field_set(unit, IPROC_NS_BS1_BS_OUTPUT_TIME_0r, &regval,
                                  LOCKf, 0);
                soc_reg_field_set(unit, IPROC_NS_BS1_BS_OUTPUT_TIME_0r, &regval,
                                  EPOCHf, 0);
                WRITE_IPROC_NS_BS1_BS_OUTPUT_TIME_0r(unit, regval);

                /* Reset Clock Control */
                READ_IPROC_NS_BS0_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CLK_CTRLr, &regval,
                                  ENABLEf, 0);
                HITLESS_REG(WRITE_IPROC_NS_BS0_BS_CLK_CTRLr(unit, regval));

                READ_IPROC_NS_BS1_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_BS1_BS_CLK_CTRLr, &regval,
                                  ENABLEf, 0);
                HITLESS_REG(WRITE_IPROC_NS_BS1_BS_CLK_CTRLr(unit, regval));

                /* Reset HeartBeat */
                READ_IPROC_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_HEARTBEAT_CTRLr, &regval,
                                  ENABLEf, 0);
                HITLESS_REG(WRITE_IPROC_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval));

                READ_IPROC_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_BS1_BS_HEARTBEAT_CTRLr, &regval,
                                  ENABLEf, 0);
                HITLESS_REG(WRITE_IPROC_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval));
            } else {
                READ_CMIC_BS0_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
                soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                HITLESS_REG(WRITE_CMIC_BS0_CONFIGr(unit, regval));

                READ_CMIC_BS1_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
                soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                HITLESS_REG(WRITE_CMIC_BS1_CONFIGr(unit, regval));

                READ_CMIC_BS0_OUTPUT_TIME_0r(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS0_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
                soc_reg_field_set(unit, CMIC_BS0_OUTPUT_TIME_0r, &regval, LOCKf, 0);
                soc_reg_field_set(unit, CMIC_BS0_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
                WRITE_CMIC_BS0_OUTPUT_TIME_0r(unit, regval);

                READ_CMIC_BS1_OUTPUT_TIME_0r(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS1_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
                soc_reg_field_set(unit, CMIC_BS1_OUTPUT_TIME_0r, &regval, LOCKf, 0);
                soc_reg_field_set(unit, CMIC_BS1_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
                WRITE_CMIC_BS1_OUTPUT_TIME_0r(unit, regval);

                /* Reset Clock Control */
                READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 0);
                HITLESS_REG(WRITE_CMIC_BS0_CLK_CTRLr(unit, regval));

                READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 0);
                HITLESS_REG(WRITE_CMIC_BS1_CLK_CTRLr(unit, regval));

                /* Reset HeartBeat */
                READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
                HITLESS_REG(WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, regval));

                READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
                HITLESS_REG(WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, regval));
            }
        }

#if defined(BCM_MONTEREY_SUPPORT)
        /* Reset Capture */
        if (SOC_IS_MONTEREY(unit)) {
            if(IS_BROADSYNC0(intf_id)) {
                if (SOC_REG_IS_VALID(unit, NS_ROE_TS_INT_ENABLEr)) {
                    READ_NS_ROE_TS_INT_ENABLEr(unit, &regval);
                    soc_reg_field_set(unit, NS_ROE_TS_INT_ENABLEr, &regval, INTENf, 0);
                    WRITE_NS_ROE_TS_INT_ENABLEr(unit, regval);
                }

                READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
                if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr, TIME_CAPTURE_ENABLEf)) {
                    soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, TIME_CAPTURE_ENABLEf, 0);
                }
                if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr,
                            CPU_FIFO1_CAPTURE_ENABLEf)) {
                    soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval,
                            CPU_FIFO1_CAPTURE_ENABLEf, 0);
                }
                if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr,
                            CPU_FIFO2_CAPTURE_ENABLEf)) {
                    soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval,
                            CPU_FIFO2_CAPTURE_ENABLEf, 0);
                }
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_TX_HB_STATUS_ENABLEf, 0);
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_RX_HB_STATUS_ENABLEf, 0);
                WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);

                READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, ENABLEf, TIME_CAPTURE_MODE_DISABLE);
                WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
            } else if(IS_BROADSYNC1(intf_id)) {
                READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS1_TX_HB_STATUS_ENABLEf, 0);
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS1_RX_HB_STATUS_ENABLEf, 0);
                WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
            }
        } else
#endif /* BCM_MONTEREY_SUPPORT */
        if (SOC_IS_TOMAHAWK3(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            if (SOC_REG_IS_VALID(unit, NS_ROE_TS_INT_ENABLEr)) {
                READ_NS_ROE_TS_INT_ENABLEr(unit, &regval);
                soc_reg_field_set(unit, NS_ROE_TS_INT_ENABLEr, &regval, INTENf, 0);
                WRITE_NS_ROE_TS_INT_ENABLEr(unit, regval);
            }

            READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr, TIME_CAPTURE_ENABLEf)) {
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, TIME_CAPTURE_ENABLEf, 0);
            }
            if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr,
                                    CPU_FIFO1_CAPTURE_ENABLEf)) {
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval,
                                  CPU_FIFO1_CAPTURE_ENABLEf, 0);
            }
            if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr,
                                    CPU_FIFO2_CAPTURE_ENABLEf)) {
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval,
                                  CPU_FIFO2_CAPTURE_ENABLEf, 0);
            }
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_RX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS1_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS1_RX_HB_STATUS_ENABLEf, 0);
            WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);

            READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, ENABLEf, TIME_CAPTURE_MODE_DISABLE);
            WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
        } else if (SOC_IS_FIRELIGHT(unit)) {
            READ_IPROC_NS_ROE_TS_INT_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_ROE_TS_INT_ENABLEr, &regval,
                              INTENf, 0);
            WRITE_IPROC_NS_ROE_TS_INT_ENABLEr(unit, regval);

            READ_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
            TIME_CAPTURE_ENABLEf, 0);
            soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
            BS0_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
            BS0_RX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
            BS1_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
            BS1_RX_HB_STATUS_ENABLEf, 0);
            WRITE_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, regval);

            READ_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
            ENABLEf, TIME_CAPTURE_MODE_DISABLE);
            WRITE_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
        } else
#ifdef BCM_TRIDENT3_SUPPORT
         if (soc_feature(unit, soc_feature_timesync_block_iproc)) {
            soc_iproc_getreg(unit, soc_reg_addr(unit,
                CMIC_TIMESYNC_INTERRUPT_ENABLEr, REG_PORT_ANY, 0), &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_INTERRUPT_ENABLEr, &regval,
                              INT_ENABLEf, 0);
            soc_iproc_setreg(unit, soc_reg_addr(unit,
                CMIC_TIMESYNC_INTERRUPT_ENABLEr, REG_PORT_ANY, 0), regval);

            soc_iproc_getreg(unit, soc_reg_addr(unit,
                CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, REG_PORT_ANY, 0), &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr,
                              &regval, TIME_CAPTURE_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr,
                              &regval, BSYNC0_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr,
                              &regval, BSYNC0_RX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr,
                              &regval, BSYNC1_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr,
                              &regval, BSYNC1_RX_HB_STATUS_ENABLEf, 0);
            soc_iproc_setreg(unit, soc_reg_addr(unit,
                CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, REG_PORT_ANY, 0), regval);

            soc_iproc_getreg(unit, soc_reg_addr(unit,
                CMIC_TIMESYNC_TIME_CAPTURE_MODEr, REG_PORT_ANY, 0), &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &regval,
                              TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_DISABLE);
            soc_iproc_setreg(unit, soc_reg_addr(unit,
                CMIC_TIMESYNC_TIME_CAPTURE_MODEr, REG_PORT_ANY, 0), regval);

        } else
#endif
        {
            READ_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_INTERRUPT_ENABLEr, &regval, INT_ENABLEf, 0);
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, regval);

            READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, TIME_CAPTURE_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_RX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC1_TX_HB_STATUS_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC1_RX_HB_STATUS_ENABLEf, 0);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);

            READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &regval, TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_DISABLE);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, regval);
        }

    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */

    /*****************************************************************************/
    /*        Non-KATANA2, HELIX4, HURRICANE2, GREYHOUND HW configuration        */
    /*****************************************************************************/
    {
        if (SOC_REG_IS_VALID(unit, CMIC_BS_DRIFT_RATEr)) {
            /* Reset Drift Rate  */
            READ_CMIC_BS_DRIFT_RATEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_DRIFT_RATEr, &regval, SIGNf, 0);
            soc_reg_field_set(unit, CMIC_BS_DRIFT_RATEr, &regval, FRAC_NSf, 0);
            WRITE_CMIC_BS_DRIFT_RATEr(unit, regval);
        }

        if (SOC_REG_IS_VALID(unit, CMIC_BS_OFFSET_ADJUST_0r)) {
            /* Reset Offset Adjust */
            READ_CMIC_BS_OFFSET_ADJUST_0r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_0r, &regval, SECONDf, 0);
            WRITE_CMIC_BS_OFFSET_ADJUST_0r(unit, regval);
            READ_CMIC_BS_OFFSET_ADJUST_1r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_1r, &regval, SIGN_BITf, 0);
            soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_1r, &regval, NSf, 0);
            WRITE_CMIC_BS_OFFSET_ADJUST_1r(unit, regval);
        }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_KATANA(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
            READ_CMIC_BS_CONFIGr(unit, &regval);
            /* Configure Broadsync in output mode */
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            WRITE_CMIC_BS_CONFIGr(unit, regval);

            READ_CMIC_BS_OUTPUT_TIME_0r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_OUTPUT_TIME_0r, &regval, ACCURACYf, 0);
            soc_reg_field_set(unit, CMIC_BS_OUTPUT_TIME_0r, &regval, LOCKf, 0);
            soc_reg_field_set(unit, CMIC_BS_OUTPUT_TIME_0r, &regval, EPOCHf, 0);
            WRITE_CMIC_BS_OUTPUT_TIME_0r(unit, regval);
        } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
        {
            READ_CMIC_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, TIME_CODE_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, LOCKf, 0);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ACCURACYf, TIME_ACCURACY_UNKNOWN_HW_VAL);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, EPOCHf, 0);
            WRITE_CMIC_BS_CONFIGr(unit, regval);
        }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_KATANA(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
            READ_CMIC_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS_CLK_CTRLr(unit, regval);
        } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
        {
            READ_CMIC_BS_CLK_CTRL_0r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_0r, &regval, ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_0r, &regval, NSf, 0);
            WRITE_CMIC_BS_CLK_CTRL_0r(unit, regval);
        }

        if (SOC_REG_IS_VALID(unit, CMIC_BS_CLK_CTRL_1r)) {
            READ_CMIC_BS_CLK_CTRL_1r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_1r, &regval, FRAC_NSf, 0);
            WRITE_CMIC_BS_CLK_CTRL_1r(unit, regval);
        }

        /* Reset Clock Toggle  */
        if (SOC_REG_IS_VALID(unit, CMIC_BS_CLK_TOGGLE_TIME_0r)) {
            READ_CMIC_BS_CLK_TOGGLE_TIME_0r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_TOGGLE_TIME_0r, &regval, SECf, 0);
            WRITE_CMIC_BS_CLK_TOGGLE_TIME_0r(unit, regval);
        }

        if (SOC_REG_IS_VALID(unit, CMIC_BS_CLK_TOGGLE_TIME_1r)) {
            READ_CMIC_BS_CLK_TOGGLE_TIME_1r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_TOGGLE_TIME_1r, &regval, NSf, 0);
            WRITE_CMIC_BS_CLK_TOGGLE_TIME_1r(unit, regval);
        }

        if (SOC_REG_IS_VALID(unit, CMIC_BS_CLK_TOGGLE_TIME_2r)) {
            READ_CMIC_BS_CLK_TOGGLE_TIME_2r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_TOGGLE_TIME_2r, &regval, FRAC_NSf, 0);
            WRITE_CMIC_BS_CLK_TOGGLE_TIME_2r(unit, regval);
        }

        READ_CMIC_BS_HEARTBEAT_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
        if (!(SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
              SOC_IS_TD2_TT2(unit))) {
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, THRESHOLDf, 0);
        }
        WRITE_CMIC_BS_HEARTBEAT_CTRLr(unit, regval);

#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
        if (SOC_IS_KATANA(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
            READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval,
                              INT_ENABLEf, 0);
            if(SOC_IS_TD2P_TT2P(unit)) {
                soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval,
                                TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_DISABLE);
            } else {
                /* Configure the HW to capture time on every heartbeat */
                soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval,
                                TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_HEARTBEAT);
            }
            WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
        } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
        {
            READ_CMIC_BS_CAPTURE_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, INT_ENf, 0);
            soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                              TIME_CAPTURE_MODE_DISABLE);
            WRITE_CMIC_BS_CAPTURE_CTRLr(unit, regval);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_esw_time_hitless_reset
 * Purpose:
 *    Internal routine used to check if hitless reset is present in
 *    the hardware, and is active.
 *    If present, hitless reset preserves the BroadSync operation
 *    and registers through a chip reset.
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf_id        - (IN) Time interface identifier
 * Returns:
 *    BCM_E_NONE if hitless reset is active
 *    BCM_E_UNAVAIL if it is not.
 */
int _bcm_esw_time_hitless_reset(int unit, bcm_time_if_t intf_id)
{
#if defined(BCM_HITLESS_RESET_SUPPORT)
    if (SOC_IS_KATANA2(unit) || SOC_IS_SABER2(unit)) {
        /* if BS0 has its output enabled, this indicates that
         * drv.c determined that hitless reset was in effect
         * and so did not clear the output.
         */
        uint32 regval;
        READ_CMIC_BS0_CONFIGr(unit, &regval);
        if (soc_reg_field_get(unit, CMIC_BS0_CONFIGr,
                              regval, BS_CLK_OUTPUT_ENABLEf) != 0) {
            return BCM_E_NONE;
        }
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    _bcm_esw_time_deinit
 * Purpose:
 *    Internal routine used to free time software module
 *    control structures.
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    time_cfg_pptr  - (IN) Pointer to pointer to time config structure.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_deinit(int unit, _bcm_time_config_p *time_cfg_pptr)
{
    int                 idx;
    _bcm_time_config_p  time_cfg_ptr;
    soc_control_t       *soc = SOC_CONTROL(unit);

    /* Sanity checks. */
    if (NULL == time_cfg_pptr) {
        return (BCM_E_PARAM);
    }

    time_cfg_ptr = *time_cfg_pptr;
    /* If time config was not allocated we are done. */
    if (NULL == time_cfg_ptr) {
        return (BCM_E_NONE);
    }

    /* Free time interface */
    if (NULL != time_cfg_ptr->intf_arr) {
        for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
            if (NULL !=  time_cfg_ptr->intf_arr[idx].user_cb) {
                sal_free(time_cfg_ptr->intf_arr[idx].user_cb);
            }
        }
        sal_free(time_cfg_ptr->intf_arr);
    }

    /* Destroy protection mutex. */
    if (NULL != time_cfg_ptr->mutex) {
        sal_mutex_destroy(time_cfg_ptr->mutex);
    }

    /* If any registered function - deregister */
    soc->time_call_ref_count = 0;
    soc->soc_time_callout = NULL;

    /* Free module configuration structue. */
    sal_free(time_cfg_ptr);
    *time_cfg_pptr = NULL;
    return (BCM_E_NONE);
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *    _bcm_time_scache_alloc
 * Purpose:
 *    Internal routine used to allocate scache memory for TIME module
 * Parameters:
 *    unit           - (IN) BCM device number.
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_time_scache_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *time_scache;
    int alloc_size = 0;

    alloc_size = ( sizeof(bcm_time_interface_t) + sizeof(int)) * NUM_TIME_INTERFACE(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TIME, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 1,
               alloc_size, &time_scache, BCM_WB_DEFAULT_VERSION, NULL));
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_time_scache_sync
 * Purpose:
 *    Routine to sync the time module to scache
 * Parameters:
 *    unit           - (IN) BCM device number.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_time_scache_sync(int unit)
{
    int         rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8 *time_scache;
    int alloc_size = 0;
    int idx;

    alloc_size = ( sizeof(bcm_time_interface_t) + sizeof(int)) * NUM_TIME_INTERFACE(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TIME, 0);
    rv = _bcm_esw_scache_ptr_get(unit,
                    scache_handle,
                    0,
                    alloc_size,
                    &time_scache,
                    BCM_WB_DEFAULT_VERSION,
                    NULL
                    );

    if (!BCM_FAILURE(rv)) {
        for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
            sal_memcpy(time_scache, (TIME_INTERFACE(unit, idx)),
                    sizeof(bcm_time_interface_t));
            time_scache += sizeof(bcm_time_interface_t);
            sal_memcpy(time_scache, &TIME_INTERFACE_CONFIG_REF_COUNT(unit, idx),
                    sizeof(int));
            time_scache += sizeof(int);
        }
    }

    return rv;
}

/*
 * Function:
 *    _bcm_esw_time_reinit
 * Purpose:
 *    Internal routine used to reinitialize time module based on HW settings
 *    during Warm boot
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf_id        - (IN) Time interface identifier
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_reinit(int unit, bcm_time_if_t intf_id)
{
    soc_scache_handle_t scache_handle;
    uint16      recovered_ver = 0;
    uint8       *time_scache;
    int         rv = BCM_E_NONE;

    if(SOC_WARM_BOOT(unit)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TIME, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                    &time_scache, BCM_WB_DEFAULT_VERSION,
                                    &recovered_ver);
        if (!BCM_FAILURE(rv)) {
            time_scache += sizeof(bcm_time_interface_t) * intf_id;
            sal_memcpy((TIME_INTERFACE(unit, intf_id)), time_scache,
                    sizeof(bcm_time_interface_t));
            time_scache += sizeof(bcm_time_interface_t);
            time_scache += sizeof(int) * intf_id;
            sal_memcpy(&(TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf_id)), time_scache,
                    sizeof(int));
            time_scache += sizeof(int);

#if defined(BCM_TIME_MANAGED_BROADSYNC)
            if ((TIME_INTERFACE(unit, intf_id))->flags & BCM_TIME_SYNC_STAMPER) {
                /* Broadsync was initialised. Hence reattach */
                (void)_bcm_mbox_comm_init(unit, MOS_MSG_MBOX_APPL_BS);
            }
#endif
        }
    }

    if (!soc_feature(unit, soc_feature_time_v3_no_bs)) {
        int hb_enable;

        /* Read the status of heartbeat interrupt */
        BCM_IF_ERROR_RETURN(
            bcm_esw_time_heartbeat_enable_get(unit, intf_id, &hb_enable));

        /* If heartbeat was enabled, restore handling functionality */
        if (hb_enable) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_time_heartbeat_enable_set(unit, intf_id, hb_enable));
        }
    }

    return (BCM_E_NONE);
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *    _bcm_esw_time_interface_id_validate
 * Purpose:
 *    Internal routine used to validate interface identifier
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to validate
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_id_validate(int unit, bcm_time_if_t id)
{
    if (0 == TIME_INIT(unit)) {
        return (BCM_E_INIT);
    }
    if (id < 0 || id > TIME_INTERFACE_ID_MAX(unit)) {
        return (BCM_E_PARAM);
    }
    if (NULL == TIME_INTERFACE(unit, id)) {
        return (BCM_E_NOT_FOUND);
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *    _bcm_esw_time_interface_input_validate
 * Purpose:
 *    Internal routine used to validate interface input
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf           - (IN) Interface to validate
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_input_validate(int unit, bcm_time_interface_t *intf)
{
    /* Sanity checks. */
    if (NULL == intf) {
        return (BCM_E_PARAM);
    }
    if (intf->flags & BCM_TIME_WITH_ID) {
        if (intf->id < 0 || intf->id > TIME_INTERFACE_ID_MAX(unit) ) {
            return (BCM_E_PARAM);
        }
    }

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (soc_feature(unit, soc_feature_time_v3_no_bs) &&
        (intf->flags & BCM_TIME_SYNC_STAMPER)) {
        return BCM_E_UNAVAIL;
    }
#endif

#if defined(BCM_TIME_MANAGED_BROADSYNC)
    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
        if (intf->heartbeat_hz <= 0 || intf->bitclock_hz <= 0) {
            return (BCM_E_PARAM);
        }
    }
#endif

#if defined(BCM_TIME_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_time_v3) &&
        !SOC_IS_HURRICANE2(unit) && !SOC_IS_GREYHOUND(unit) &&
        !SOC_IS_TOMAHAWKX(unit) && !SOC_IS_HURRICANE3(unit) &&
        !SOC_IS_GREYHOUND2(unit) && !SOC_IS_FIRELIGHT(unit) &&
        !SOC_IS_TRIDENT3(unit) && !SOC_IS_HELIX5(unit) &&
        !(intf->flags & BCM_TIME_SYNC_STAMPER)) {
        /*
         * Drift/Offset is not supported by 3rd generation time architecture
         * NB: Hurricane2 (56150) enables v3 time feature. BroadSync FW APIs
         *     support time offset functionality. !SOC_IS_HURRICANE2() logic
         *     added to conditional to enable phase offset on HR2 similar to
         *     other platforms.
         */
        if (intf->flags & BCM_TIME_DRIFT || intf->flags & BCM_TIME_OFFSET) {
            return BCM_E_UNAVAIL;
        }
    }
#endif /* defined(BCM_TIME_V3_SUPPORT) */

    if (intf->flags & BCM_TIME_DRIFT) {
        if (intf->drift.nanoseconds > TIME_DRIFT_MAX) {
            return BCM_E_PARAM;
        }
    }

    if (intf->flags & BCM_TIME_OFFSET) {
        if (intf->offset.nanoseconds > TIME_NANOSEC_MAX) {
            return BCM_E_PARAM;
}
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *    _bcm_esw_time_interface_allocate_id
 * Purpose:
 *    Internal routine used to allocate time interface id
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (OUT) Interface id to be allocated
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_allocate_id(int unit, bcm_time_if_t *id)
{
    int                              idx;  /* Time interfaces iteration index.*/
    _bcm_time_interface_config_p     intf; /* Time interface description.     */

    /* Input parameters check. */
    if (NULL == id) {
        return (BCM_E_PARAM);
    }

    /* Find & allocate time interface. */
    for (idx = 0; idx < TIME_CONFIG(unit)->intf_count; idx++) {
        intf = TIME_CONFIG(unit)->intf_arr + idx;
        if (intf->ref_count) {  /* In use interface */
            continue;
        }
        intf->ref_count++;  /* If founf mark interface as in use */
        *id = intf->time_interface.id; /* Assign ID */
        return (BCM_E_NONE);
    }

    /* No available interfaces */
    return (BCM_E_FULL);
}

/*
 * Function:
 *    _bcm_esw_time_interface_heartbeat_install
 * Purpose:
 *    Internal routine used to install interface heartbeat rate into a HW
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_heartbeat_install(int unit, bcm_time_if_t id)
{
    uint32  regval;
    uint32  hb_hz;  /* Number of heartbeats in HZ */
    uint32  ns_hp;  /* Number of nanosec per half clock cycle*/
    uint32  threshold; /* Heartbeat threshold */

    bcm_time_interface_t    *intf;

    intf = TIME_INTERFACE(unit, id);

    hb_hz = (intf->heartbeat_hz > TIME_HEARTBEAT_HZ_MAX) ?
        TIME_HEARTBEAT_HZ_MAX : intf->heartbeat_hz;
    hb_hz = (intf->heartbeat_hz < TIME_HEARTBEAT_HZ_MIN) ?
        TIME_HEARTBEAT_HZ_MIN : hb_hz;

    threshold = intf->bitclock_hz / hb_hz;

    if (threshold < 100) {
        /* BitClock must have a frequency at least 100 times that of heartbeat. */
        return BCM_E_PARAM;
    }

    /* Calculate nanoseconds per half period of cycle */
    ns_hp = TIME_HEARTBEAT_NS_HZ / intf->bitclock_hz / 2;

    /* program the half period of broadsync clock */
#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        uint32  interval = 0;
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

            /* enable Heartbeat generation */
            READ_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval);

            /* Install heartbeat up and down interval */
            READ_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, threshold);
            WRITE_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, threshold);
            WRITE_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            READ_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, threshold);
            WRITE_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, threshold);
            WRITE_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            /* Enable heartbeat output */
            READ_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval);

            READ_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval);

            /*
             * KT-1381 :
             *  TS_GPIO_LOW = (UP_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
             *  TS_GPIO_HIGH = (DOWN_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
             */
            threshold = (threshold * 4) + SOC_TIMESYNC_PLL_CLOCK_NS(unit);

            /* Install GPIO timesync trigger interval */
            /* GPIO_EVENT_1 */
            READ_NS_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr(unit, interval);

            READ_NS_TIMESYNC_GPIO_0_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_0_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_0_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_2 */
            READ_NS_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr(unit, interval);

            READ_NS_TIMESYNC_GPIO_1_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_1_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_1_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_3 */
            READ_NS_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr(unit, interval);

            READ_NS_TIMESYNC_GPIO_2_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_2_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_2_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_4 */
            READ_NS_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr(unit, interval);

            READ_NS_TIMESYNC_GPIO_3_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_3_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_3_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_5 */
            READ_NS_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr(unit, interval);

            READ_NS_TIMESYNC_GPIO_4_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_4_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_4_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_6 */
            READ_NS_TIMESYNC_GPIO_5_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_5_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_5_DOWN_EVENT_CTRLr(unit, interval);

            READ_NS_TIMESYNC_GPIO_5_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, NS_TIMESYNC_GPIO_5_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_NS_TIMESYNC_GPIO_5_UP_EVENT_CTRLr(unit, interval);
        } else if (SOC_IS_FIRELIGHT(unit)) {
            READ_IPROC_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf,
                              1);
            WRITE_IPROC_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_IPROC_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf,
                              1);
            WRITE_IPROC_NS_BS1_BS_CLK_CTRLr(unit, regval);

            /* enable Heartbeat generation */
            READ_IPROC_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_HEARTBEAT_CTRLr, &regval,
                              ENABLEf, 1);
            WRITE_IPROC_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval);

            /* Install heartbeat up and down interval */
            READ_IPROC_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_HEARTBEAT_UP_DURATIONr,
                              &regval, UP_DURATIONf, threshold);
            WRITE_IPROC_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_IPROC_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr,
                              &regval, DOWN_DURATIONf, threshold);
            WRITE_IPROC_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            READ_IPROC_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_HEARTBEAT_UP_DURATIONr,
                              &regval, UP_DURATIONf, threshold);
            WRITE_IPROC_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_IPROC_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr,
                              &regval, DOWN_DURATIONf, threshold);
            WRITE_IPROC_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            /* Enable heartbeat output */
            READ_IPROC_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_HEARTBEAT_CTRLr, &regval,
                              ENABLEf, 1);
            WRITE_IPROC_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval);

            READ_IPROC_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_HEARTBEAT_CTRLr, &regval,
                              ENABLEf, 1);
            WRITE_IPROC_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval);

            /*
             * KT-1381 :
             *  TS_GPIO_LOW = (UP_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
             *  TS_GPIO_HIGH = (DOWN_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
             */
            threshold = (threshold * 4) + SOC_TIMESYNC_PLL_CLOCK_NS(unit);

            /* Install GPIO timesync trigger interval */
            /* GPIO_EVENT_1 */
            READ_IPROC_NS_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr(unit, interval);

            READ_IPROC_NS_TIMESYNC_GPIO_0_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_0_UP_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_0_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_2 */
            READ_IPROC_NS_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr(unit, interval);

            READ_IPROC_NS_TIMESYNC_GPIO_1_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_1_UP_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_1_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_3 */
            READ_IPROC_NS_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr(unit, interval);

            READ_IPROC_NS_TIMESYNC_GPIO_2_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_2_UP_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_2_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_4 */
            READ_IPROC_NS_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr(unit, interval);

            READ_IPROC_NS_TIMESYNC_GPIO_3_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_3_UP_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_3_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_5 */
            READ_IPROC_NS_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr(unit, interval);

            READ_IPROC_NS_TIMESYNC_GPIO_4_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_4_UP_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_4_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_6 */
            READ_IPROC_NS_TIMESYNC_GPIO_5_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_5_DOWN_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_5_DOWN_EVENT_CTRLr(unit, interval);

            READ_IPROC_NS_TIMESYNC_GPIO_5_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_GPIO_5_UP_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_IPROC_NS_TIMESYNC_GPIO_5_UP_EVENT_CTRLr(unit, interval);
        } else {
            READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

            READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

            /* enable Heartbeat generation */
            READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, regval);

            /* Install heartbeat up and down interval */
            READ_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, threshold);
            WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, threshold);
            WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            READ_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, threshold);
            WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, threshold);
            WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            /* Enable heartbeat output */
            READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, regval);

            READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, regval);

            /*
             * KT-1381 :
             *  TS_GPIO_LOW = (UP_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
             *  TS_GPIO_HIGH = (DOWN_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
             */
            threshold = (threshold * 4) + SOC_TIMESYNC_PLL_CLOCK_NS(unit);

            /* Install GPIO timesync trigger interval */
            /* GPIO_EVENT_1 */
            READ_CMIC_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_0_DOWN_EVENT_CTRLr(unit, interval);

            READ_CMIC_TIMESYNC_GPIO_0_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_0_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_0_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_2 */
            READ_CMIC_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_1_DOWN_EVENT_CTRLr(unit, interval);

            READ_CMIC_TIMESYNC_GPIO_1_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_1_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_1_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_3 */
            READ_CMIC_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_2_DOWN_EVENT_CTRLr(unit, interval);

            READ_CMIC_TIMESYNC_GPIO_2_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_2_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_2_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_4 */
            READ_CMIC_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_3_DOWN_EVENT_CTRLr(unit, interval);

            READ_CMIC_TIMESYNC_GPIO_3_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_3_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_3_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_5 */
            READ_CMIC_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_4_DOWN_EVENT_CTRLr(unit, interval);

            READ_CMIC_TIMESYNC_GPIO_4_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_GPIO_4_UP_EVENT_CTRLr, &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TIMESYNC_GPIO_4_UP_EVENT_CTRLr(unit, interval);
        }

    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
    {
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
            SOC_IS_TD2_TT2(unit)) {

            READ_CMIC_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

            READ_CMIC_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS_HEARTBEAT_CTRLr(unit, regval);
        } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
        {
            READ_CMIC_BS_CLK_CTRL_0r(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_0r, &regval, ENABLEf, 1);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_0r, &regval, NSf, ns_hp);
            WRITE_CMIC_BS_CLK_CTRL_0r(unit, regval);

            READ_CMIC_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval,
                              THRESHOLDf, threshold);
            WRITE_CMIC_BS_HEARTBEAT_CTRLr(unit, regval);
        }

#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
        /* Install heartbeat and external trigger clock interval */
        if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
            SOC_IS_TD2_TT2(unit)) {
            uint32  interval = 0;

            /* Install heartbeat up and down interval */
            READ_CMIC_BS_HEARTBEAT_UP_DURATIONr(unit, &interval);
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_UP_DURATIONr,
                              &interval, UP_DURATIONf, threshold);
            WRITE_CMIC_BS_HEARTBEAT_UP_DURATIONr(unit, interval);

            READ_CMIC_BS_HEARTBEAT_DOWN_DURATIONr(unit, &interval);
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_DOWN_DURATIONr,
                              &interval, DOWN_DURATIONf, threshold);
            WRITE_CMIC_BS_HEARTBEAT_DOWN_DURATIONr(unit, interval);

            /* Install divisor for LCPLL trigger interval */
            READ_CMIC_TS_LCPLL_CLK_COUNT_CTRLr(unit, &interval);
            soc_reg_field_set(unit,
                              CMIC_TS_LCPLL_CLK_COUNT_CTRLr,
                              &interval, DIVISORf, threshold);
            WRITE_CMIC_TS_LCPLL_CLK_COUNT_CTRLr(unit, interval);

            /* Install divisor for Primary L1 trigger interval */
            READ_CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr(unit, &interval);
            soc_reg_field_set(unit,
                              CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr,
                              &interval, DIVISORf, threshold);
            WRITE_CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr(unit, interval);

            /* Install divisor for backup L1 trigger interval */
            READ_CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr(unit, &interval);
            soc_reg_field_set(unit,
                              CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr,
                              &interval, DIVISORf, threshold);
            WRITE_CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr(unit, interval);

            /*
             * KT-1381 :
             *  TS_GPIO_LOW =
                   (UP_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
             *  TS_GPIO_HIGH =
                   (DOWN_EVENT_INTERVAL x 4) + 1 TS_REF_CLK_time_period
             */
            threshold = (threshold * 4) + SOC_TIMESYNC_PLL_CLOCK_NS(unit);

            /* Install GPIO timesync trigger interval */
            /* GPIO_EVENT_1 */
            READ_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr, &interval,
                              INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, interval);
            READ_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_1_UP_EVENT_CTRLr, &interval,
                              INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_2 */
            READ_CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr, &interval,
                              INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr(unit, interval);
            READ_CMIC_TS_GPIO_2_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_2_UP_EVENT_CTRLr, &interval,
                              INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_2_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_3 */
            READ_CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr, &interval,
                              INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr(unit, interval);
            READ_CMIC_TS_GPIO_3_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_3_UP_EVENT_CTRLr, &interval,
                              INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_3_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_4 */
            READ_CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr, &interval,
                              INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr(unit, interval);
            READ_CMIC_TS_GPIO_4_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit, CMIC_TS_GPIO_4_UP_EVENT_CTRLr, &interval,
                              INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_4_UP_EVENT_CTRLr(unit, interval);

            /* GPIO_EVENT_5 */
            READ_CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit,
                              CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr(unit, interval);
            READ_CMIC_TS_GPIO_5_UP_EVENT_CTRLr(unit, &interval);
            soc_reg_field_set(unit,
                              CMIC_TS_GPIO_5_UP_EVENT_CTRLr,
                              &interval, INTERVAL_LENGTHf, threshold);
            WRITE_CMIC_TS_GPIO_5_UP_EVENT_CTRLr(unit, interval);
        }
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *    _bcm_esw_time_interface_accuracy_time2hw
 * Purpose:
 *    Internal routine used to compute HW accuracy value from interface
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id
 *    accuracy       - (OUT) HW value to be programmed
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_accuracy_time2hw(int unit, bcm_time_if_t id,
                                         uint32 *accuracy)
{
    int                     idx;    /* accuracy itterator */
    bcm_time_interface_t    *intf;

    if (NULL == accuracy) {
        return BCM_E_PARAM;
    }

    intf = TIME_INTERFACE(unit, id);

    /* Find the right accuracy */
    for (idx = 0; idx < TIME_ACCURACY_CLK_MAX; idx++) {
        if (intf->accuracy.nanoseconds <= _bcm_time_accuracy_arr[idx].nanoseconds &&
             COMPILER_64_LO(intf->accuracy.seconds) <= COMPILER_64_LO(_bcm_time_accuracy_arr[idx].seconds) ) {
            break;
        }
    }
    /* if no match - return error */
    if (idx == TIME_ACCURACY_CLK_MAX) {
        return BCM_E_NOT_FOUND;
    }

    /* Return the correct HW value */

    *accuracy = TIME_ACCURACY_SW_IDX_2_HW(idx);

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_esw_time_interface_drift_install
 * Purpose:
 *    Internal routine used to install interface drift into a HW
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_drift_install(int unit, bcm_time_if_t id)
{
    uint32 regval, hw_val, sign;
    bcm_time_interface_t    *intf;

#if defined(BCM_TIME_V3_SUPPORT)
    /* Not supported in 3rd generation time arch */
    if (soc_feature(unit, soc_feature_time_v3)) {
        return BCM_E_UNAVAIL;
    }
#endif /* defined(BCM_TIME_V3_SUPPORT) */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    /* Katana does not support clock drift */
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */

    intf = TIME_INTERFACE(unit, id);
    sign = intf->drift.isnegative;

    /* Requested drift value should not be more then 1/8 of drift denominator */
    if (intf->drift.nanoseconds <= TIME_DRIFT_MAX) {
        hw_val = 8 * intf->drift.nanoseconds;
    } else {
        return (BCM_E_PARAM);
    }
    READ_CMIC_BS_DRIFT_RATEr(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_DRIFT_RATEr, &regval, SIGNf, sign);
    soc_reg_field_set(unit, CMIC_BS_DRIFT_RATEr, &regval, FRAC_NSf, hw_val);
    WRITE_CMIC_BS_DRIFT_RATEr(unit, regval);

    return (BCM_E_NONE);
}

/*
 * Function:
 *    _bcm_esw_time_interface_offset_install
 * Purpose:
 *    Internal routine used to install interface offset into a HW
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_offset_install(int unit, bcm_time_if_t id)
{
    uint32 regval, sign, hw_val;
    bcm_time_interface_t    *intf;

#if defined(BCM_TIME_V3_SUPPORT)
    /* Not supported in 3rd generation time arch */
    if (soc_feature(unit, soc_feature_time_v3)) {
        return BCM_E_UNAVAIL;
    }
#endif /* defined(BCM_TIME_V3_SUPPORT) */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
    /* Katana does not support clock offset */
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit) || SOC_IS_HELIX4(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HELIX4_SUPPORT) */

    intf = TIME_INTERFACE(unit, id);

    /* Negative value if local clock is faster and need negative adjustment */
    sign = intf->offset.isnegative;
    /* Write second's values into the HW */
    READ_CMIC_BS_OFFSET_ADJUST_0r(unit, &regval);
    hw_val = COMPILER_64_LO(intf->offset.seconds);
    soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_0r, &regval, SECONDf, hw_val);
    WRITE_CMIC_BS_OFFSET_ADJUST_0r(unit, regval);
    /* Write sign and nansecond's values into the HW */
    READ_CMIC_BS_OFFSET_ADJUST_1r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_1r, &regval,
                      SIGN_BITf, sign);
    soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_1r, &regval, NSf,
                      intf->offset.nanoseconds);
    WRITE_CMIC_BS_OFFSET_ADJUST_1r(unit, regval);

    /* If necessary install the epoch */
    if (COMPILER_64_HI(intf->offset.seconds) > 0) {
        READ_CMIC_BS_CONFIGr(unit, &regval);
        hw_val = COMPILER_64_HI(intf->offset.seconds);
        if (hw_val > TIME_EPOCH_MAX) {
            hw_val = TIME_EPOCH_MAX;
        }
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, EPOCHf, hw_val);
        WRITE_CMIC_BS_CONFIGr(unit, regval);
    }

    return (BCM_E_NONE);
}

#if defined(BCM_TIME_MANAGED_BROADSYNC)
/*
 * Function:
 *    _bcm_esw_time_interface_ref_clock_install
 * Purpose:
 *    Internal routine to install timesync clock divisor to
 *    enable broadsync reference clock.
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_ref_clock_install(int unit, bcm_time_if_t id)
{
    uint32 regval, ndiv, hdiv, ref_clk;
    bcm_time_interface_t    *intf;

    intf = TIME_INTERFACE(unit, id);

    /* Validate and calculate ts_clk divisor to generate reference clock */
    if (intf->clk_resolution <= 0) {
        return BCM_E_PARAM;
    }

    /* Maximum ts_clk frequency is of 25Mhz(40ns) */
    ref_clk = (intf->clk_resolution > TIME_TS_CLK_FREQUENCY_40NS) ?
              TIME_TS_CLK_FREQUENCY_40NS : intf->clk_resolution;

    /* Divisor is half period for reference clock */
    ndiv = TIME_TS_CLK_FREQUENCY_40NS / ref_clk;

    /* Divisor is ceiling of half period */
    hdiv = (ndiv + 1)/2 ? ndiv : 1;

    /* Enable Broadsync reference clock */
#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit)) {
            READ_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr, &regval,
                              ENABLEf, 1);
            soc_reg_field_set(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr, &regval,
                              DIVISORf, hdiv);
            WRITE_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);
        } else if (SOC_IS_FIRELIGHT(unit)) {
            READ_IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr,
                              &regval, ENABLEf, 1);
            soc_reg_field_set(unit, IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr,
                              &regval, DIVISORf, hdiv);
            WRITE_IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);
        } else {
            READ_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr, &regval,
                              ENABLEf, 1);
            soc_reg_field_set(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr, &regval,
                              DIVISORf, hdiv);
            WRITE_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);
        }
    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
    {
        READ_CMIC_BS_REF_CLK_GEN_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_BS_REF_CLK_GEN_CTRLr, &regval,
                          ENABLEf, 1);
        soc_reg_field_set(unit, CMIC_BS_REF_CLK_GEN_CTRLr, &regval,
                          DIVISORf, hdiv);
        WRITE_CMIC_BS_REF_CLK_GEN_CTRLr(unit, regval);
    }

    return (BCM_E_NONE);
}
#endif /* BCM_TIME_MANAGED_BROADSYNC */

/*
 * Function:
 *    _bcm_esw_time_interface_install
 * Purpose:
 *    Internal routine used to install interface settings into a HW
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf_id        - (IN) Interface id to be installed into a HW
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_install(int unit, bcm_time_if_t intf_id)
{
    bcm_time_interface_t    *intf;  /* Time interface */
    uint32                  regval; /* For register read and write operations */
    uint32                  hw_val; /* Value to program into a HW */
    uint32                  second, diff, delay = BROAD_SYNC_OUTPUT_TOGGLE_TIME_DELAY;
    int                     enable = 0;
    soc_reg_t               reg;
    bcm_time_spec_t         toggle_time;

    if (NULL == TIME_INTERFACE(unit, intf_id)) {
        return BCM_E_PARAM;
    }

    intf = TIME_INTERFACE(unit, intf_id);

    READ_CMIC_BS_CONFIGr(unit, &regval);

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        if ((intf->flags & BCM_TIME_ENABLE)) {
            /* Enable all three broadsync pins:
             *     Bit clock, HeartBeat and Timecode
             */
            soc_reg_field_set(
                    unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 1);
            soc_reg_field_set(
                    unit, CMIC_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 1);
            soc_reg_field_set(
                    unit, CMIC_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 1);
        } else {
            soc_reg_field_set(
                    unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(
                    unit, CMIC_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
        }

        if ((intf->flags & BCM_TIME_LOCKED)) {
            soc_reg_field32_modify(
                    unit, CMIC_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 1);
        } else {
            soc_reg_field32_modify(
                    unit, CMIC_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 0);
        }

    } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
    {
        if (intf->flags & BCM_TIME_ENABLE) {
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ENABLEf, 1);
            soc_reg_field_set(
                unit, CMIC_BS_CONFIGr, &regval, TIME_CODE_ENABLEf, 1);
        } else {
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ENABLEf, 0);
            soc_reg_field_set(
                unit, CMIC_BS_CONFIGr, &regval, TIME_CODE_ENABLEf, 0);
        }

        if ((intf->flags & BCM_TIME_LOCKED)) {
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, LOCKf, 1);
        } else {
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, LOCKf, 0);
        }
    }

    if (intf->flags & BCM_TIME_INPUT) {
        /* Set Broadsync in Input mode */
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf,
                          TIME_MODE_INPUT);
    } else {
        /* Configure Accuracy, offset and drift for broadsync output mode */
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf,
                          TIME_MODE_OUTPUT);
        if (intf->flags & BCM_TIME_ACCURACY) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_interface_accuracy_time2hw(unit, intf_id, &hw_val));
            if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
                SOC_IS_TD2_TT2(unit)) {
                soc_reg_field32_modify(unit, CMIC_BS_OUTPUT_TIME_0r,
                                       REG_PORT_ANY, ACCURACYf, hw_val);
            } else {
                soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ACCURACYf, hw_val);
            }
        }
        if (intf->flags & BCM_TIME_HEARTBEAT) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_interface_heartbeat_install(unit, intf_id));

            if (!(intf->flags & BCM_TIME_OFFSET)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_time_capture_get(unit, intf_id, TIME_CAPTURE(unit, intf_id), 0));
                if (!(SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
                      SOC_IS_TD2_TT2(unit))) {
                    second = COMPILER_64_LO(SYNT_TIME_SECONDS(unit, intf_id));
                    second += delay;
                    WRITE_CMIC_BS_CLK_TOGGLE_TIME_0r(unit, second);
                    WRITE_CMIC_BS_CLK_TOGGLE_TIME_1r(
                                unit, SYNT_TIME_NANOSECONDS(unit, intf_id));
                }
            }
        }
    }
    WRITE_CMIC_BS_CONFIGr(unit, regval);

    if (intf->flags & BCM_TIME_OFFSET) {
        if(SOC_REG_IS_VALID(unit, CMIC_BS_CLK_CTRL_0r)) {
            reg   =   CMIC_BS_CLK_CTRL_0r;
            READ_CMIC_BS_CLK_CTRL_0r(unit, &regval);
        } else {
            reg   =   CMIC_BS_CLK_CTRLr;
            READ_CMIC_BS_CLK_CTRLr(unit, &regval);
        }

        /* Stop broadsync output if already enabled */
        enable = soc_reg_field_get(unit, reg, regval, ENABLEf);
        if (1 == enable) {
            soc_reg_field_set(unit, reg, &regval, ENABLEf, 0);
            if (SOC_REG_IS_VALID(unit, CMIC_BS_CLK_CTRL_0r)) {
                WRITE_CMIC_BS_CLK_CTRL_0r(unit, regval);
            } else {
                WRITE_CMIC_BS_CLK_CTRLr(unit, regval);
            }
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_capture_get(unit, intf_id, TIME_CAPTURE(unit, intf_id), 0));
        }

        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_offset_install(unit, intf_id));

        if (intf->offset.isnegative) {
            second = COMPILER_64_LO(SYNT_TIME_SECONDS(unit, intf_id)) -
                     COMPILER_64_LO(intf->offset.seconds);

            if (intf->offset.nanoseconds <= SYNT_TIME_NANOSECONDS(unit, intf_id)) {
                toggle_time.nanoseconds = SYNT_TIME_NANOSECONDS(unit, intf_id) -
                                          intf->offset.nanoseconds;
            } else {
                /* Wrapped */
                diff = intf->offset.nanoseconds - SYNT_TIME_NANOSECONDS(unit, intf_id);
                toggle_time.nanoseconds = TIME_NANOSEC_MAX - diff;
                second--;
            }
            COMPILER_64_SET(toggle_time.seconds, 0, second);
        } else {
            second = COMPILER_64_LO(SYNT_TIME_SECONDS(unit, intf_id)) +
                     COMPILER_64_LO(intf->offset.seconds);
            toggle_time.nanoseconds =
                SYNT_TIME_NANOSECONDS(unit, intf_id) + intf->offset.nanoseconds;
            if (toggle_time.nanoseconds >= TIME_NANOSEC_MAX) {
                toggle_time.nanoseconds -= TIME_NANOSEC_MAX;
                second++;
            }
            COMPILER_64_SET(toggle_time.seconds, 0, second);
        }

        if (1 == enable) {
            /* Reenable broadsync output */
            COMPILER_64_TO_32_LO(second, toggle_time.seconds);
            second += delay;
            if (SOC_REG_IS_VALID(unit, CMIC_BS_CLK_TOGGLE_TIME_0r)) {
                WRITE_CMIC_BS_CLK_TOGGLE_TIME_0r(unit, second);
            }

            if (SOC_REG_IS_VALID(unit, CMIC_BS_CLK_TOGGLE_TIME_1r)) {
                WRITE_CMIC_BS_CLK_TOGGLE_TIME_1r(unit, toggle_time.nanoseconds);
            }

            if (SOC_REG_IS_VALID(unit, CMIC_BS_CLK_TOGGLE_TIME_1r)) {
                READ_CMIC_BS_CLK_CTRL_0r(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_0r, &regval, ENABLEf, 1);
                WRITE_CMIC_BS_CLK_CTRL_0r(unit, regval);
            } else {
                READ_CMIC_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_BS_CLK_CTRLr(unit, regval);
            }
        }
    }

    if (intf->flags & BCM_TIME_DRIFT) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_drift_install(unit, intf_id));
    }

    return (BCM_E_NONE);
}

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
STATIC int
_bcm_esw_time_interface_dual_bs_install(int unit, bcm_time_if_t intf_id)
{
    bcm_time_interface_t    *intf;  /* Time interface */
    uint32                  regval; /* For register read and write operations */
    uint32                  hw_val; /* Value to program into a HW */

    /* Feature unavailable on devices without BroadSync support. */
    if (soc_feature(unit, soc_feature_time_v3_no_bs)) {
        return BCM_E_UNAVAIL;
    }

    if (NULL == TIME_INTERFACE(unit, intf_id)) {
        return BCM_E_PARAM;
    }

    intf = TIME_INTERFACE(unit, intf_id);

    if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
        SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        READ_NS_BS0_BS_CONFIGr(unit, &regval);

        if ((intf->flags & BCM_TIME_LOCKED)) {
            soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 1);
        } else {
            soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 0);
        }

        if (intf->flags & BCM_TIME_INPUT) {
            /* Set Broadsync in Input mode */
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, TIME_MODE_INPUT);

            /* ensure all output controls are disabled in input mode */
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
        } else {
            /* Configure Accuracy, offset and drift for broadsync output mode */
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);

            if ((intf->flags & BCM_TIME_ENABLE)) {
                /* Enable all three broadsync pins: Bit clock, HeartBeat and Timecode */
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 1);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 1);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 1);
            } else {
                /* ensure all output controls are disabled */
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
            }

            if (intf->flags & BCM_TIME_ACCURACY) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_time_interface_accuracy_time2hw(unit, intf_id, &hw_val));
                soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r, REG_PORT_ANY, ACCURACYf, hw_val);
            }

            if (intf->flags & BCM_TIME_HEARTBEAT) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_time_interface_heartbeat_install(unit, intf_id));
            }
        }

        WRITE_NS_BS0_BS_CONFIGr(unit, regval);

    } else if (SOC_IS_FIRELIGHT(unit)) {
        READ_IPROC_NS_BS0_BS_CONFIGr(unit, &regval);

        if ((intf->flags & BCM_TIME_LOCKED)) {
            soc_reg_field32_modify(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r,
                                   REG_PORT_ANY, LOCKf, 1);
        } else {
            soc_reg_field32_modify(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r,
                                   REG_PORT_ANY, LOCKf, 0);
        }

        if (intf->flags & BCM_TIME_INPUT) {
            /* Set Broadsync in Input mode */
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval, MODEf,
                              TIME_MODE_INPUT);

            /* ensure all output controls are disabled in input mode */
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                              BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                              BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                              BS_HB_OUTPUT_ENABLEf, 0);
        } else {
            /* Configure Accuracy, offset and drift for broadsync output mode */
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval, MODEf,
                              TIME_MODE_OUTPUT);

            if ((intf->flags & BCM_TIME_ENABLE)) {
                /* Enable all three broadsync pins: Bit clock, HeartBeat and Timecode */
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                                  BS_CLK_OUTPUT_ENABLEf, 1);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                                  BS_TC_OUTPUT_ENABLEf, 1);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                                  BS_HB_OUTPUT_ENABLEf, 1);
            } else {
                /* ensure all output controls are disabled */
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                                  BS_CLK_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                                  BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                                  BS_HB_OUTPUT_ENABLEf, 0);
            }

            if (intf->flags & BCM_TIME_ACCURACY) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_time_interface_accuracy_time2hw(unit, intf_id, &hw_val));
                soc_reg_field32_modify(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r,
                                       REG_PORT_ANY, ACCURACYf, hw_val);
            }

            if (intf->flags & BCM_TIME_HEARTBEAT) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_time_interface_heartbeat_install(unit, intf_id));
            }
        }

        WRITE_IPROC_NS_BS0_BS_CONFIGr(unit, regval);
    } else {

        READ_CMIC_BS0_CONFIGr(unit, &regval);

        if ((intf->flags & BCM_TIME_LOCKED)) {
            soc_reg_field32_modify(unit, CMIC_BS0_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 1);
        } else {
            soc_reg_field32_modify(unit, CMIC_BS0_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, 0);
        }

        if (intf->flags & BCM_TIME_INPUT) {
            /* Set Broadsync in Input mode */
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, TIME_MODE_INPUT);

            /* ensure all output controls are disabled in input mode */
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
        } else {
            /* Configure Accuracy, offset and drift for broadsync output mode */
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);

            if ((intf->flags & BCM_TIME_ENABLE)) {
                /* Enable all three broadsync pins: Bit clock, HeartBeat and Timecode */
                soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 1);
                soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 1);
                soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 1);
            } else {
                /* ensure all output controls are disabled */
                soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, 0);
                soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, 0);
            }

            if (intf->flags & BCM_TIME_ACCURACY) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_time_interface_accuracy_time2hw(unit, intf_id, &hw_val));
                soc_reg_field32_modify(unit, CMIC_BS0_OUTPUT_TIME_0r, REG_PORT_ANY, ACCURACYf, hw_val);
            }

            if (intf->flags & BCM_TIME_HEARTBEAT) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_time_interface_heartbeat_install(unit, intf_id));
            }
        }

        WRITE_CMIC_BS0_CONFIGr(unit, regval);
    }
    return (BCM_E_NONE);
}
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */


/*
 * Function:
 *     _bcm_time_interface_free
 * Purpose:
 *     Free time interface.
 * Parameters:
 *      unit            - (IN) BCM device number.
 *      intf_id         - (IN) time interface id.
 * Returns:
 *      BCM_X_XXX
 */

STATIC int
_bcm_time_interface_free(int unit, bcm_time_if_t intf_id)
{
    _bcm_time_interface_config_p  intf_cfg; /* Time interface config.*/

    intf_cfg = &TIME_INTERFACE_CONFIG(unit, intf_id);

    if (intf_cfg->ref_count > 0) {
        intf_cfg->ref_count--;
    }

    if (0 == intf_cfg->ref_count) {
        sal_memset(&intf_cfg->time_interface, 0, sizeof(bcm_time_interface_t));
        intf_cfg->time_interface.id = intf_id;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_time_accuracy_parse
 * Purpose:
 *      Internal routine to parse accuracy hw value into bcm_time_spec_t format
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      accuracy    - (IN) Accuracy HW value
 *      time        - (OUT) bcm_time_spec_t structure to contain accuracy
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_time_accuracy_parse(int unit, uint32 accuracy, bcm_time_spec_t *time)
{
    if (accuracy < TIME_ACCURACY_LOW_HW_VAL ||
        (accuracy > TIME_ACCURACY_HIGH_HW_VAL &&
         accuracy != TIME_ACCURACY_UNKNOWN_HW_VAL)) {
        return (BCM_E_PARAM);
    }

    time->isnegative =
        _bcm_time_accuracy_arr[TIME_ACCURACY_HW_2_SW_IDX(accuracy)].isnegative;
    time->nanoseconds =
        _bcm_time_accuracy_arr[TIME_ACCURACY_HW_2_SW_IDX(accuracy)].nanoseconds;
    time->seconds =
        _bcm_time_accuracy_arr[TIME_ACCURACY_HW_2_SW_IDX(accuracy)].seconds;
    time->isnegative = 0;   /* Can't be negative */

    return (BCM_E_NONE);
}


/*
 * Function:
 *      _bcm_esw_time_input_parse
 * Purpose:
 *      Internal routine to parse input time information stored in 3 registeres
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      data0   - (IN) Data stored in register 0 conrain input time information
 *      data1   - (IN) Data stored in register 1 conrain input time information
 *      data2   - (IN) Data stored in register 2 conrain input time information
 *      time    - (OUT) Structure to contain input time information
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_time_input_parse(int unit, uint32 data0, uint32 data1, uint32 data2,
                          bcm_time_capture_t *time)
{
    uint32 accuracy, sec_hi, sec_lo;

    if (data0 >> 31) {
        time->flags |= BCM_TIME_CAPTURE_LOCKED;
    }
    time->received.isnegative = 0;
    sec_hi = ((data0 << 1) >> 1);
    sec_lo = (data1 >> 14);
    COMPILER_64_SET(time->received.seconds, sec_hi, sec_lo);
    time->received.nanoseconds = (data1 << 16);
    time->received.nanoseconds |= (data2 >> 8);

    accuracy = ((data2 << 24) >> 24);

    return _bcm_esw_time_accuracy_parse(unit,accuracy,
                                        &(time->received_accuracy));
}

/*
 * Function:
 *      _bcm_esw_time_capture_counter_read
 * Purpose:
 *      Internal routine to read HW clocks
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time interface identifier
 *      time    - (OUT) Structure to contain HW clocks values
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_time_capture_counter_read(int unit, bcm_time_if_t id,
                                   bcm_time_capture_t *time)
{
    uint32                  regval;
    bcm_time_interface_t    *intf;

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        uint32 regval2, event_id, lower;
        uint32 upper;
        sal_memset(time, 0, sizeof(bcm_time_capture_t));

        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            /* Read timestamp from FIFO */
            uint32 regval1, regval3;
            soc_field_t tsfifo_not_empty;
            soc_reg_t tsfifo_ts[4];

            if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                /*
                 * There are two FIFOs on HR4/FB6.
                 * By default, the configuration is map all events to FIFO1.
                 * No R5 on HR4, so no FW needs the FIFO.
                 * SDK will reads from FIFIO1.
                 */
                tsfifo_not_empty = TS_FIFO1_NOT_EMPTYf;
                tsfifo_ts[0] = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_0r;
                tsfifo_ts[1] = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1r;
                tsfifo_ts[2] = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_2r;
                tsfifo_ts[3] = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r;
            } else {
                tsfifo_not_empty = TS_FIFO_NOT_EMPTYf;
                tsfifo_ts[0] = NS_TIMESYNC_INPUT_TIME_FIFO_TS_0r;
                tsfifo_ts[1] = NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r;
                tsfifo_ts[2] = NS_TIMESYNC_INPUT_TIME_FIFO_TS_2r;
                tsfifo_ts[3] = NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r;
            }

            READ_NS_TS_INT_STATUSr(unit, &regval);
            if (0 == soc_reg_field_get(unit, NS_TS_INT_STATUSr,
                                       regval, tsfifo_not_empty)) {
                return (BCM_E_EMPTY);
            }

            /* Must read LOWER regiser first */
            /* Read to this register POPs the FIFO */
            if (SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_0r(unit, &regval);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1r(unit, &regval1);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_2r(unit, &regval2);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r(unit, &regval3);
            } else {
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_0r(unit, &regval);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r(unit, &regval1);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_2r(unit, &regval2);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r(unit, &regval3);
            }

             /* If it's not valid, something is wrong (since FIFO is not empty) */
            if (0 == soc_reg_field_get(unit, tsfifo_ts[3], regval3, TS_VALIDf)) {
                return (BCM_E_FAIL);
            }
            /* Check capture event id */
            event_id = soc_reg_field_get(unit, tsfifo_ts[3], regval3, TS_EVENT_IDf);
            
            if (event_id == 0) {
                time->flags |= BCM_TIME_CAPTURE_IMMEDIATE;
            }
            /* Calculate seconds/nanoseconds from the timestamp value */
            lower = soc_reg_field_get(unit, tsfifo_ts[0], regval, TS0_Lf);

            upper = soc_reg_field_get(unit, tsfifo_ts[1], regval1, TS0_Uf);
            /* Discard 4bits from lower*/
            lower = (lower >> 4);
            lower = (lower & 0x0FFFFFFF) | ((upper & 0xF) << 28);
            upper = (upper >> 4);

        } else if (SOC_IS_FIRELIGHT(unit)) {
            /* Read timestamp from FIFO */
            uint32 regval1, regval3;
            READ_IPROC_NS_TS_INT_STATUSr(unit, &regval);
            if (0 == soc_reg_field_get(unit, IPROC_NS_TS_INT_STATUSr,
                                       regval, TS_FIFO_NOT_EMPTYf)) {
                return (BCM_E_EMPTY);
            }

            /* Must read LOWER regiser first */
            /* Read to this register POPs the FIFO */
            READ_IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_0r(unit, &regval);
            READ_IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r(unit, &regval1);
            READ_IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_2r(unit, &regval2);
            READ_IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r(unit, &regval3);

             /* If it's not valid, something is wrong (since FIFO is not empty) */
            if (0 == soc_reg_field_get(
                            unit, IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r,
                            regval3, TS_VALIDf)) {
                return (BCM_E_FAIL);
            }
            /* Check capture event id */
            event_id = soc_reg_field_get(
                            unit, IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r,
                            regval3, TS_EVENT_IDf);
            
            if (event_id == 0) {
                time->flags |= BCM_TIME_CAPTURE_IMMEDIATE;
            }
            /* Calculate seconds/nanoseconds from the timestamp value */
            lower = soc_reg_field_get(
                            unit, IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_0r,
                            regval, TS0_Lf);

            upper = soc_reg_field_get(
                            unit, IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r,
                            regval1, TS0_Uf);
            /* Discard 4bits from lower*/
            lower = (lower >> 4);
            lower = (lower & 0x0FFFFFFF) | ((upper & 0xF) << 28);
            upper = (upper >> 4);

        } else {
            /* Read timestamp from FIFO */
            READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &regval);
            if (0 == soc_reg_field_get(unit, CMIC_TIMESYNC_CAPTURE_STATUS_1r,
                                       regval, FIFO_DEPTHf)) {
                return (BCM_E_EMPTY);
            }

            /* Must read LOWER regiser first */
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(unit, &regval2);

            /* Then read UPPER register */
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(unit, &regval);
             /* If it's not valid, something is wrong (since FIFO is not empty) */
            if (0 == soc_reg_field_get(
                            unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr, regval,
                            VALIDf)) {
                return (BCM_E_FAIL);
            }
            /* Check capture event id */
            event_id = soc_reg_field_get(
                            unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                            regval, EVENT_IDf);
            if (event_id == 0) {
                time->flags |= BCM_TIME_CAPTURE_IMMEDIATE;
            }
            /* Calculate seconds/nanoseconds from the timestamp value */
            lower = soc_reg_field_get(
                            unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr,
                            regval2, TIMESTAMPf);

            upper = soc_reg_field_get(
                            unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                            regval, TIMESTAMPf);

        }

        if (soc_feature(unit, soc_feature_timesync_timestampingmode)) {
            /* 48-bit timestamping supported */
            uint64 time_val, secs, nano_secs;
            uint64 div;
            /* Get the 48-bit timestamp value */
            COMPILER_64_SET(time_val, upper, lower);

            /* Get seconds from the timestamp value */
            secs = time_val;
            COMPILER_64_SET(div, 0, 1000000000);
            COMPILER_64_UDIV_64(secs, div);
            time->synchronous.seconds = secs;

            /* Get nanoseconds from the timestamp value */
            nano_secs = time_val;
            COMPILER_64_UMUL_32(secs, 1000000000);
            COMPILER_64_SUB_64(nano_secs, secs);
            time->synchronous.nanoseconds = COMPILER_64_LO(nano_secs);
        } else {
            COMPILER_64_SET(time->synchronous.seconds, 0, lower / 1000000000);
            time->synchronous.nanoseconds = lower % 1000000000;
        }

        /* For backward compatibility */
        time->free = time->synchronous;
        time->syntonous = time->synchronous;

    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_KATANA(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        uint32 event_id;

        /* Check FIFO depth */
        sal_memset(time, 0, sizeof(bcm_time_capture_t));
        READ_CMIC_TS_CAPTURE_STATUSr(unit, &regval);
        if (0 == soc_reg_field_get(unit, CMIC_TS_CAPTURE_STATUSr,
                                   regval, FIFO_DEPTHf)) {
            return (BCM_E_EMPTY);
        }

        /* Read timestamp from FIFO */
        READ_CMIC_TS_INPUT_TIME_FIFO_TSr(unit, &regval);
        time->synchronous.nanoseconds =  soc_reg_field_get(unit,
                                          CMIC_TS_INPUT_TIME_FIFO_TSr, regval,
                                          TIMESTAMPf);

        /* If it's not valid, something is wrong (since FIFO is not empty) */
        READ_CMIC_TS_INPUT_TIME_FIFO_IDr(unit, &regval);
        if (0 == soc_reg_field_get(
                        unit, CMIC_TS_INPUT_TIME_FIFO_IDr, regval,
                        VALIDf)) {
            return (BCM_E_FAIL);
        }

        /* Check capture event id */
        event_id = soc_reg_field_get(
                        unit, CMIC_TS_INPUT_TIME_FIFO_IDr,
                        regval, EVENT_IDf);

        if (event_id == 0) {
            time->flags |= BCM_TIME_CAPTURE_IMMEDIATE;
        }

        /* For backward compatibility */
        time->free = time->synchronous;
        time->syntonous = time->synchronous;

    } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
    {
        /* Read free running time capture */
        READ_CMIC_BS_CAPTURE_FREE_RUN_TIME_0r(unit, &regval);
        COMPILER_64_SET(time->free.seconds, 0,
            soc_reg_field_get(unit, CMIC_BS_CAPTURE_FREE_RUN_TIME_0r,
                              regval, SECONDf));
        READ_CMIC_BS_CAPTURE_FREE_RUN_TIME_1r(unit, &regval);
        time->free.nanoseconds =
         soc_reg_field_get(unit, CMIC_BS_CAPTURE_FREE_RUN_TIME_1r, regval, NSf);
        /* Read syntonous time capture */
        READ_CMIC_BS_CAPTURE_SYNT_TIME_0r(unit, &regval);
        COMPILER_64_SET(time->syntonous.seconds, 0,
         soc_reg_field_get(unit, CMIC_BS_CAPTURE_SYNT_TIME_0r, regval, SECONDf));
        READ_CMIC_BS_CAPTURE_SYNT_TIME_1r(unit, &regval);
        time->syntonous.nanoseconds =
            soc_reg_field_get(unit, CMIC_BS_CAPTURE_SYNT_TIME_1r, regval, NSf);
        /* Read synchronous time capture */
        READ_CMIC_BS_CAPTURE_SYNC_TIME_0r(unit, &regval);
        COMPILER_64_SET(time->synchronous.seconds, 0,
         soc_reg_field_get(unit, CMIC_BS_CAPTURE_SYNC_TIME_0r, regval, SECONDf));
        READ_CMIC_BS_CAPTURE_SYNC_TIME_1r(unit, &regval);
        time->synchronous.nanoseconds =
            soc_reg_field_get(unit, CMIC_BS_CAPTURE_SYNC_TIME_1r, regval, NSf);
    }

    time->free.isnegative = time->synchronous.isnegative =
        time->syntonous.isnegative = 0;

    /* If interface is in input mode read time provided by the master */
    intf = TIME_INTERFACE(unit, id);
    if (intf->flags & BCM_TIME_INPUT) {
        uint32  data0 = 0, data1 = 0, data2 = 0;

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
        if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
            if (!soc_feature(unit, soc_feature_time_v3_no_bs)) {
                if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit)  ||
                    SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                    READ_NS_BS0_BS_INPUT_TIME_2r(unit, &regval);
                    if (soc_reg_field_get(unit, NS_BS0_BS_INPUT_TIME_2r, regval,
                                          CHECKSUM_ERRORf)) {
                        return BCM_E_INTERNAL;
                    }
                    data2 = soc_reg_field_get(
                                unit, NS_BS0_BS_INPUT_TIME_2r, regval, DATAf);
                    READ_NS_BS0_BS_INPUT_TIME_1r(unit, &regval);
                    data1 = soc_reg_field_get(
                                unit, NS_BS0_BS_INPUT_TIME_1r, regval, DATAf);
                    READ_NS_BS0_BS_INPUT_TIME_0r(unit, &regval);
                    data0 = soc_reg_field_get(
                                unit, NS_BS0_BS_INPUT_TIME_0r, regval, DATAf);
                } else if (SOC_IS_FIRELIGHT(unit)) {
                    READ_IPROC_NS_BS0_BS_INPUT_TIME_2r(unit, &regval);
                    if (soc_reg_field_get(unit, IPROC_NS_BS0_BS_INPUT_TIME_2r,
                                          regval, CHECKSUM_ERRORf)) {
                        return BCM_E_INTERNAL;
                    }
                    data2 = soc_reg_field_get(unit,
                                              IPROC_NS_BS0_BS_INPUT_TIME_2r,
                                              regval, DATAf);
                    READ_NS_BS0_BS_INPUT_TIME_1r(unit, &regval);
                    data1 = soc_reg_field_get(unit,
                                              IPROC_NS_BS0_BS_INPUT_TIME_1r,
                                              regval, DATAf);
                    READ_IPROC_NS_BS0_BS_INPUT_TIME_0r(unit, &regval);
                    data0 = soc_reg_field_get(unit,
                                              IPROC_NS_BS0_BS_INPUT_TIME_0r,
                                              regval, DATAf);
                } else {
                    READ_CMIC_BS0_INPUT_TIME_2r(unit, &regval);
                    if (soc_reg_field_get(unit, CMIC_BS0_INPUT_TIME_2r, regval,
                                          CHECKSUM_ERRORf)) {
                        return BCM_E_INTERNAL;
                    }
                    data2 = soc_reg_field_get(
                                unit, CMIC_BS0_INPUT_TIME_2r, regval, DATAf);
                    READ_CMIC_BS0_INPUT_TIME_1r(unit, &regval);
                    data1 = soc_reg_field_get(
                                unit, CMIC_BS0_INPUT_TIME_1r, regval, DATAf);
                    READ_CMIC_BS0_INPUT_TIME_0r(unit, &regval);
                    data0 = soc_reg_field_get(
                                unit, CMIC_BS0_INPUT_TIME_0r, regval, DATAf);
                }
            }
        } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
        {
            READ_CMIC_BS_INPUT_TIME_2r(unit, &regval);
            if (soc_reg_field_get(unit, CMIC_BS_INPUT_TIME_2r, regval,
                                  CHECKSUM_ERRORf)) {
                return BCM_E_INTERNAL;
            }
            data2 = soc_reg_field_get(unit, CMIC_BS_INPUT_TIME_2r, regval, DATAf);
            READ_CMIC_BS_INPUT_TIME_1r(unit, &regval);
            data1 = soc_reg_field_get(unit, CMIC_BS_INPUT_TIME_1r, regval, DATAf);
            READ_CMIC_BS_INPUT_TIME_0r(unit, &regval);
            data0 = soc_reg_field_get(unit, CMIC_BS_INPUT_TIME_0r, regval, DATAf);
        }

        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_input_parse(unit, data0, data1, data2, time));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_time_capture_get
 * Purpose:
 *      Internal routine to read HW clocks
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time interface identifier
 *      time    - (OUT) Structure to contain HW clocks values
 *      flags   - (IN) Flags BCM_TIME_CAPTURE_
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_time_capture_get (int unit, bcm_time_if_t id, bcm_time_capture_t *time, uint32 flags)
{
    uint32          regval, orgval; /* To keep register value */
    int             hw_complete;    /* HW read completion indicator*/
    soc_timeout_t   timeout;        /* Timeout in case of HW error */
#if defined(BCM_GREYHOUND_SUPPORT)
    uint32 lower, upper;
#endif

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        /* Read capture mode */
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            uint32 lower, upper;
            READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            orgval = soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr,
                                       regval, ENABLEf);
            if (flags == BCM_TIME_CAPTURE_IMMEDIATE || orgval == TIME_CAPTURE_MODE_DISABLE ||
                   orgval == TIME_CAPTURE_MODE_IMMEDIATE) {
                /* do an immediate capture */
                READ_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr(unit, &regval);
                upper = regval;
                READ_NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr(unit, &regval);
                lower = regval;
                /* Discard 4bits from lower*/
                lower = (lower >> 4);
                lower = (lower & 0x0FFFFFFF) | ((upper & 0xF) << 28);
                upper = (upper >> 4);

                if (soc_feature(unit, soc_feature_timesync_timestampingmode)) {
                    /* 48-bit timestamping supported */
                    uint64 time_val, secs, nano_secs;
                    uint64 div;
                    /* Get the 48-bit timestamp value */
                    COMPILER_64_SET(time_val, upper, lower);
                    /* Get seconds from the timestamp value */
                    secs = time_val;
                    COMPILER_64_SET(div, 0, 1000000000);
                    COMPILER_64_UDIV_64(secs, div);
                    time->synchronous.seconds = secs;

                    /* Get nanoseconds from the timestamp value */
                    nano_secs = time_val;
                    COMPILER_64_UMUL_32(secs, 1000000000);
                    COMPILER_64_SUB_64(nano_secs, secs);
                    time->synchronous.nanoseconds = COMPILER_64_LO(nano_secs);
                } else {
                    COMPILER_64_SET(time->synchronous.seconds, 0, lower / 1000000000);
                    time->synchronous.nanoseconds = lower % 1000000000;
                }

                /* For backward compatibility */
                time->free = time->synchronous;
                time->syntonous = time->synchronous;
                time->flags |= BCM_TIME_CAPTURE_IMMEDIATE;
                return BCM_E_NONE;
            } else {

                /* If any trigger condition is set, get the timestamp from FIFO */
                return _bcm_esw_time_capture_counter_read(unit, id, time);
            }
        } else if (SOC_IS_FIRELIGHT(unit)) {
            uint32 lower, upper;
            READ_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            orgval = soc_reg_field_get(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr,
                                       regval, ENABLEf);
            if (flags == BCM_TIME_CAPTURE_IMMEDIATE ||
                orgval == TIME_CAPTURE_MODE_DISABLE ||
                orgval == TIME_CAPTURE_MODE_IMMEDIATE) {
                /* do an immediate capture */
                READ_IPROC_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr(unit,
                                                                   &regval);
                upper = regval;
                READ_IPROC_NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr(unit,
                                                                   &regval);
                lower = regval;
                /* Discard 4bits from lower*/
                lower = (lower >> 4);
                lower = (lower & 0x0FFFFFFF) | ((upper & 0xF) << 28);
                upper = (upper >> 4);

                if (soc_feature(unit, soc_feature_timesync_timestampingmode)) {
                    /* 48-bit timestamping supported */
                    uint64 time_val, secs, nano_secs;
                    uint64 div;
                    /* Get the 48-bit timestamp value */
                    COMPILER_64_SET(time_val, upper, lower);
                    /* Get seconds from the timestamp value */
                    secs = time_val;
                    COMPILER_64_SET(div, 0, 1000000000);
                    COMPILER_64_UDIV_64(secs, div);
                    time->synchronous.seconds = secs;

                    /* Get nanoseconds from the timestamp value */
                    nano_secs = time_val;
                    COMPILER_64_UMUL_32(secs, 1000000000);
                    COMPILER_64_SUB_64(nano_secs, secs);
                    time->synchronous.nanoseconds = COMPILER_64_LO(nano_secs);
                } else {
                    COMPILER_64_SET(time->synchronous.seconds, 0, lower / 1000000000);
                    time->synchronous.nanoseconds = lower % 1000000000;
                }

                /* For backward compatibility */
                time->free = time->synchronous;
                time->syntonous = time->synchronous;
                time->flags |= BCM_TIME_CAPTURE_IMMEDIATE;
                return BCM_E_NONE;
            } else {

                /* If any trigger condition is set, get the timestamp from FIFO */
                return _bcm_esw_time_capture_counter_read(unit, id, time);
            }
        }
#if defined(BCM_GREYHOUND_SUPPORT)
        if (flags == BCM_TIME_CAPTURE_IMMEDIATE) {
            /* do an immediate capture */
            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, &regval);
            lower = regval;
            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, &regval);
            upper = regval;
            if (soc_feature(unit, soc_feature_timesync_timestampingmode)) {
                /* 48-bit timestamping supported */
                uint64 time_val, secs, nano_secs;
                uint64 div;
                /* Get the 48-bit timestamp value */
                COMPILER_64_SET(time_val, upper, lower);
                /* Get seconds from the timestamp value */
                secs = time_val;
                COMPILER_64_SET(div, 0, 1000000000);
                COMPILER_64_UDIV_64(secs, div);
                time->synchronous.seconds = secs;

                /* Get nanoseconds from the timestamp value */
                nano_secs = time_val;
                COMPILER_64_UMUL_32(secs, 1000000000);
                COMPILER_64_SUB_64(nano_secs, secs);
                time->synchronous.nanoseconds = COMPILER_64_LO(nano_secs);
            } else {
                COMPILER_64_SET(time->synchronous.seconds, 0, lower / 1000000000);
                time->synchronous.nanoseconds = lower % 1000000000;
            }

            /* For backward compatibility */
            time->free = time->synchronous;
            time->syntonous = time->synchronous;
            return BCM_E_NONE;
        }
#endif
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
        orgval = soc_reg_field_get(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr,
                                   regval, TIME_CAPTURE_MODEf);

        /* If any trigger condition is set, get the timestamp from FIFO */
        if ((orgval != TIME_CAPTURE_MODE_DISABLE) &&
            (orgval != TIME_CAPTURE_MODE_IMMEDIATE)) {
            return _bcm_esw_time_capture_counter_read(unit, id, time);
        }
        /* Make sure timestamp capture is enabled */
        READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
        if (0 == soc_reg_field_get(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr,
                                   regval, TIME_CAPTURE_ENABLEf)) {
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr,
                            &regval, TIME_CAPTURE_ENABLEf, 1 );
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);
        }

        /* Clear the FIFO if not empty */
        for(;;) {
            READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &regval);
            if (0 == soc_reg_field_get(unit, CMIC_TIMESYNC_CAPTURE_STATUS_1r,
                                       regval, FIFO_DEPTHf)) {
                break;
            }
            /* There is still something in the FIFO, pop it */
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(unit, &regval);
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(unit, &regval);
        }

        /* Make sure the capture status is cleared */
        READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &regval);
        if (regval != 0) {
            soc_reg_field_set(unit, CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1r,
                              &regval, TIME_CAPTURE_COMPLETE_CLRf, 1);
            soc_reg_field_set(unit, CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1r,
                              &regval, FIFO_WRITE_COMPLETE_CLRf, 1);
            WRITE_CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1r(unit, regval);
        }

        /* Program HW to capture time immediately */
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &regval,
                          TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_IMMEDIATE);
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, regval);

        /* Wait for HW time capture completion */
        hw_complete = 0;
        soc_timeout_init(&timeout, BROAD_SYNC_TIME_CAPTURE_TIMEOUT, 0);
        while (!hw_complete) {
            READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &regval);
            hw_complete = soc_reg_field_get(unit, CMIC_TIMESYNC_CAPTURE_STATUS_1r,
                                            regval, TIME_CAPTURE_COMPLETEf);
            if (soc_timeout_check(&timeout)) {
                return (BCM_E_TIMEOUT);
            }
        }

        /* Program HW to disable time capture */
        READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &regval,
                            TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_DISABLE);
        WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, regval);

        /* Read the HW time */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_capture_counter_read(unit, id, time));

    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
    if (SOC_IS_KATANA(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        /* Read capture mode */
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        /* If any trigger condition is set, get the timestamp from FIFO */
        if (TIME_CAPTURE_MODE_DISABLE != soc_reg_field_get(unit,
                   CMIC_TS_TIME_CAPTURE_CTRLr, regval, TIME_CAPTURE_MODEf) ) {
            return _bcm_esw_time_capture_counter_read(unit, id, time);
        }

        /* if different than interrupt remember original capture mode */
        orgval = regval;

        /* Program HW to disable time capture */
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval,
                            TIME_CAPTURE_MODEf, TIME_CAPTURE_MODE_DISABLE);
        if (orgval != regval) {
            WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
        }

        /* Make sure timestamp capture is enabled */
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        if (0 == soc_reg_field_get(unit, CMIC_TS_TIME_CAPTURE_CTRLr,
                                   regval, TIME_CAPTURE_ENABLEf)) {
            soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr,
                            &regval, TIME_CAPTURE_ENABLEf, 1 );
            WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
        }

        /* Clear the FIFO if not empty */
        for(;;) {
            READ_CMIC_TS_CAPTURE_STATUSr(unit, &regval);
            if (0 == soc_reg_field_get(unit, CMIC_TS_CAPTURE_STATUSr,
                                       regval, FIFO_DEPTHf)) {
                break;
            }
            /* There is still something in the FIFO, pop it */
            READ_CMIC_TS_INPUT_TIME_FIFO_TSr(unit, &regval);
        }

        /* Make sure the capture status is cleared */
        READ_CMIC_TS_CAPTURE_STATUSr(unit, &regval);
        if (regval != 0) {
            soc_reg_field_set(unit, CMIC_TS_CAPTURE_STATUS_CLRr,
                              &regval, TIME_CAPTURE_COMPLETE_CLRf, 1);
            soc_reg_field_set(unit, CMIC_TS_CAPTURE_STATUS_CLRr,
                              &regval, FIFO_WRITE_COMPLETE_CLRf, 1);
            WRITE_CMIC_TS_CAPTURE_STATUS_CLRr(unit, regval);
        }

        /* Program HW to capture time immediately */
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval,
                        TIME_CAPTURE_MODEf,TIME_CAPTURE_MODE_IMMEDIATE);
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);

        /* Wait for HW time capture completion */
        hw_complete = 0;
        soc_timeout_init(&timeout, BROAD_SYNC_TIME_CAPTURE_TIMEOUT, 0);

        while (!hw_complete) {
            READ_CMIC_TS_CAPTURE_STATUSr(unit, &regval);
            hw_complete = soc_reg_field_get(unit, CMIC_TS_CAPTURE_STATUSr,
                                            regval, TIME_CAPTURE_COMPLETEf);
            if (soc_timeout_check(&timeout)) {
                return (BCM_E_TIMEOUT);
            }
        }

        /* Read the HW time */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_capture_counter_read(unit, id, time));

        /* Program HW to original time capture value */
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, orgval);

    } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
    {
        /* Read capture mode */
        READ_CMIC_BS_CAPTURE_CTRLr(unit, &regval);
        /* If in interrupt mode, return time captured on last interrupt */
        if (TIME_CAPTURE_MODE_HEARTBEAT == soc_reg_field_get(unit,
                            CMIC_BS_CAPTURE_CTRLr, regval, TIME_CAPTURE_MODEf)) {
            return _bcm_esw_time_capture_counter_read(unit, id, time);
        }
        /* if different than interrupt remember original capture mode */
        orgval = regval;

        /* Program HW to disable time capture */
        soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                          TIME_CAPTURE_MODE_DISABLE);
        if (orgval != regval) {
            WRITE_CMIC_BS_CAPTURE_CTRLr(unit, regval);
        }

        /* Program HW to capture time immediately */
        soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                          TIME_CAPTURE_MODE_IMMEDIATE);
        WRITE_CMIC_BS_CAPTURE_CTRLr(unit, regval);

        /* Wait for HW time capture completion */
        hw_complete = 0;
        soc_timeout_init(&timeout, BROAD_SYNC_TIME_CAPTURE_TIMEOUT, 0);

        while (!hw_complete) {
            READ_CMIC_BS_CAPTURE_STATUSr(unit, &regval);
            hw_complete = soc_reg_field_get(unit, CMIC_BS_CAPTURE_STATUSr, regval,
                                            TIME_CAPTURE_COMPLETEf);
            if (soc_timeout_check(&timeout)) {
                return (BCM_E_TIMEOUT);
            }
        }
        /* Read the HW time */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_capture_counter_read(unit, id, time));

        /* Program HW to original time capture value */
        WRITE_CMIC_BS_CAPTURE_CTRLr(unit, orgval);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_interface_offset_get
 * Purpose:
 *      Internal routine to read HW offset value and convert it into
 *      bcm_time_spec_t structure
 * Parameters:
 *      unit    -  (IN) StrataSwitch Unit #.
 *      id      -  (IN) Time interface identifier
 *      offset  - (OUT) Time interface  offset
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_offset_get(int unit, bcm_time_if_t id,
                                   bcm_time_spec_t *offset)
{
    uint32 regval, sec, epoch;

#if defined(BCM_TIME_V3_SUPPORT)
    /* Not supported in 3rd generation time arch */
    if (soc_feature(unit, soc_feature_time_v3)) {
        return BCM_E_UNAVAIL;
    }
#endif /* defined(BCM_TIME_V3_SUPPORT) */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    /* Katana does not support clock offset */
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */

    READ_CMIC_BS_OFFSET_ADJUST_0r(unit, &regval);
    sec = soc_reg_field_get(unit, CMIC_BS_OFFSET_ADJUST_0r, regval, SECONDf);
    READ_CMIC_BS_OFFSET_ADJUST_1r(unit, &regval);
    offset->nanoseconds= soc_reg_field_get(unit, CMIC_BS_OFFSET_ADJUST_1r,
                                           regval, NSf);
    offset->isnegative = soc_reg_field_get(unit, CMIC_BS_OFFSET_ADJUST_1r,
                                           regval, SIGN_BITf);
    /* If the epoch was installed read it */
    READ_CMIC_BS_CONFIGr(unit, &regval);
    epoch = soc_reg_field_get(unit,CMIC_BS_CONFIGr, regval, EPOCHf);

    COMPILER_64_SET(offset->seconds, epoch, sec);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_time_interface_drift_get
 * Purpose:
 *      Internal routine to read HW drift value and convert it into
 *      bcm_time_spec_t structure
 * Parameters:
 *      unit    -  (IN) StrataSwitch Unit #.
 *      id      -  (IN) Time interface identifier
 *      drift   - (OUT) Time interface  drift
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_drift_get(int unit, bcm_time_if_t id,
                                  bcm_time_spec_t *drift)
{
    uint32 regval, val, ns;

#if defined(BCM_TIME_V3_SUPPORT)
    /* Not supported in 3rd generation time arch */
    if (soc_feature(unit, soc_feature_time_v3)) {
        return BCM_E_UNAVAIL;
    }
#endif /* defined(BCM_TIME_V3_SUPPORT) */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    /* Katana does not support clock drift */
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */


    READ_CMIC_BS_DRIFT_RATEr(unit, &regval);
    val = soc_reg_field_get(unit, CMIC_BS_DRIFT_RATEr, regval, FRAC_NSf);

    ns = (val / 8);

    drift->nanoseconds = (ns > TIME_DRIFT_MAX) ? TIME_DRIFT_MAX : ns;

    drift->isnegative = soc_reg_field_get(unit, CMIC_BS_DRIFT_RATEr,
                                          regval, SIGNf);
    return (BCM_E_NONE);
}

#if defined(BCM_TIME_MANAGED_BROADSYNC)
/*
 * Function:
 *      _bcm_esw_time_interface_ref_clock_get
 * Purpose:
 *      Internal routine to read ref clock divisor
 * Parameters:
 *      unit    -  (IN) StrataSwitch Unit #.
 *      id      -  (IN) Time interface identifier
 *      divisor - (OUT) Time interface  ref clock resolution
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_ref_clock_get(int unit, bcm_time_if_t id,
                                  int *clk_resolution)
{
    uint32 regval, val, hdiv = 0;

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            READ_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            val = soc_reg_field_get(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr,
                                    regval, ENABLEf);
            hdiv = soc_reg_field_get(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr,
                                     regval, DIVISORf);
        } else if (SOC_IS_FIRELIGHT(unit)) {
            READ_IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            val = soc_reg_field_get(unit, IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr,
                                    regval, ENABLEf);
            hdiv = soc_reg_field_get(unit, IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr,
                                     regval, DIVISORf);
        } else {
            READ_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            val = soc_reg_field_get(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr,
                                    regval, ENABLEf);
            hdiv = soc_reg_field_get(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr,
                                     regval, DIVISORf);
        }
    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
    {
        READ_CMIC_BS_REF_CLK_GEN_CTRLr(unit, &regval);
        val = soc_reg_field_get(unit, CMIC_BS_REF_CLK_GEN_CTRLr, regval,
                                ENABLEf);
        hdiv = soc_reg_field_get(unit, CMIC_BS_REF_CLK_GEN_CTRLr, regval,
                                 DIVISORf);
    }

    if (val) {
        /* Divisor is half period for reference clock */
        *clk_resolution = TIME_TS_CLK_FREQUENCY_40NS / (hdiv * 2);
    } else {
        *clk_resolution = 0;
    }

    return (BCM_E_NONE);
}
#endif /*BCM_TIME_MANAGED_BROADSYNC */

/*
 * Function:
 *      _bcm_esw_time_interface_accuracy_get
 * Purpose:
 *      Internal routine to read HW accuracy value and convert it into
 *      bcm_time_spec_t structure
 * Parameters:
 *      unit        -  (IN) StrataSwitch Unit #.
 *      id          -  (IN) Time interface identifier
 *      accuracy    - (OUT) Time interface  accuracy
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_accuracy_get(int unit, bcm_time_if_t id,
                                     bcm_time_spec_t *accuracy)
{
    uint32  regval, val;

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            READ_NS_BS0_BS_OUTPUT_TIME_0r(unit, &regval);
            val = soc_reg_field_get(unit, NS_BS0_BS_OUTPUT_TIME_0r, regval, ACCURACYf);
        } else if (SOC_IS_FIRELIGHT(unit)) {
            READ_IPROC_NS_BS0_BS_OUTPUT_TIME_0r(unit, &regval);
            val = soc_reg_field_get(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r,
                                    regval, ACCURACYf);
        } else {
            READ_CMIC_BS0_OUTPUT_TIME_0r(unit, &regval);
            val = soc_reg_field_get(unit, CMIC_BS0_OUTPUT_TIME_0r, regval, ACCURACYf);
        }
    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        READ_CMIC_BS_OUTPUT_TIME_0r(unit, &regval);
        val = soc_reg_field_get(unit, CMIC_BS_OUTPUT_TIME_0r, regval, ACCURACYf);
    } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
    {
        READ_CMIC_BS_CONFIGr(unit, &regval);
        val = soc_reg_field_get(unit, CMIC_BS_CONFIGr, regval, ACCURACYf);
    }

    return _bcm_esw_time_accuracy_parse(unit, val, accuracy);
}

/*
 * Function:
 *      _bcm_esw_time_interface_get
 * Purpose:
 *      Internal routine to get a time sync interface by id
 * Parameters:
 *      unit -  (IN) StrataSwitch Unit #.
 *      id   -  (IN) Time interface identifier
 *      intf - (IN/OUT) Time Sync Interface to get
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_time_interface_get(int unit, bcm_time_if_t id, bcm_time_interface_t *intf)
{
    uint32                  regval;
    bcm_time_interface_t    *intf_ptr;
    uint32                  orig_flags;

    intf_ptr = TIME_INTERFACE(unit, id);
    orig_flags = intf_ptr->flags;

    intf_ptr->flags = intf->flags;
    intf_ptr->id = id;
    if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
        SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        READ_NS_BS0_BS_CONFIGr(unit, &regval);
        /* Update output flags */
        if (TIME_MODE_INPUT == soc_reg_field_get(unit, NS_BS0_BS_CONFIGr,
                                                 regval, MODEf)) {
            intf_ptr->flags |= BCM_TIME_INPUT;
        } else {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }
    } else if (SOC_IS_FIRELIGHT(unit)) {
        READ_IPROC_NS_BS0_BS_CONFIGr(unit, &regval);
        /* Update output flags */
        if (TIME_MODE_INPUT == soc_reg_field_get(unit, IPROC_NS_BS0_BS_CONFIGr,
                                                 regval, MODEf)) {
            intf_ptr->flags |= BCM_TIME_INPUT;
        } else {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }
    } else {
        READ_CMIC_BS_CONFIGr(unit, &regval);

        /* Update output flags */
        if (TIME_MODE_INPUT == soc_reg_field_get(unit, CMIC_BS_CONFIGr,
                                                 regval, MODEf)) {
            intf_ptr->flags |= BCM_TIME_INPUT;
        } else {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }
    }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
       if (soc_reg_field_get(unit, CMIC_BS_CONFIGr,
                             regval, BS_CLK_OUTPUT_ENABLEf)) {
            intf_ptr->flags |= BCM_TIME_ENABLE;
        } else {
            intf_ptr->flags &= ~BCM_TIME_ENABLE;
        }

        READ_CMIC_BS_OUTPUT_TIME_0r(unit, &regval);
        if (soc_reg_field_get(unit, CMIC_BS_OUTPUT_TIME_0r,
                              regval, LOCKf)) {
            intf_ptr->flags |= BCM_TIME_LOCKED;
        } else {
            intf_ptr->flags &= ~BCM_TIME_LOCKED;
        }
    } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
    {
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            if (soc_reg_field_get(unit, NS_BS0_BS_CONFIGr, regval, ENABLEf)) {
                intf_ptr->flags |= BCM_TIME_ENABLE;
            } else {
                intf_ptr->flags &= ~BCM_TIME_ENABLE;
            }
            if (soc_reg_field_get(unit, NS_BS0_BS_OUTPUT_TIME_0r, regval, LOCKf)) {
                intf_ptr->flags |= BCM_TIME_LOCKED;
            } else {
                intf_ptr->flags &= ~BCM_TIME_LOCKED;
            }
        } else if (SOC_IS_FIRELIGHT(unit)) {
            if (soc_reg_field_get(unit, IPROC_NS_BS0_BS_CONFIGr, regval,
                                  ENABLEf)) {
                intf_ptr->flags |= BCM_TIME_ENABLE;
            } else {
                intf_ptr->flags &= ~BCM_TIME_ENABLE;
            }
            if (soc_reg_field_get(unit, IPROC_NS_BS0_BS_CONFIGr, regval,
                                  LOCKf)) {
                intf_ptr->flags |= BCM_TIME_LOCKED;
            } else {
                intf_ptr->flags &= ~BCM_TIME_LOCKED;
            }
        } else {
            if (soc_reg_field_get(unit, CMIC_BS_CONFIGr, regval, ENABLEf)) {
                intf_ptr->flags |= BCM_TIME_ENABLE;
            } else {
                intf_ptr->flags &= ~BCM_TIME_ENABLE;
            }
            if (soc_reg_field_get(unit, CMIC_BS_CONFIGr, regval, LOCKf)) {
                intf_ptr->flags |= BCM_TIME_LOCKED;
            } else {
                intf_ptr->flags &= ~BCM_TIME_LOCKED;
            }
        }
    }

#if defined(INCLUDE_PTP)
    if ((orig_flags & BCM_TIME_SYNC_STAMPER) &&
            (_bcm_ptp_running(unit) == BCM_E_NONE)) {
        intf_ptr->flags = orig_flags;
        *intf = *intf_ptr;
        return BCM_E_NONE;
    }
#endif /* INCLUDE_PTP */

    if (intf->flags & BCM_TIME_ACCURACY) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_accuracy_get(unit, id,
                                                 &(intf_ptr->accuracy)));
    }

    if (intf->flags & BCM_TIME_OFFSET) {
        if (orig_flags & BCM_TIME_SYNC_STAMPER) {
            
        } else {
            /* Get value from hardware */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_interface_offset_get(unit, id, &(intf_ptr->offset)));
        }
    }

    if (intf->flags & BCM_TIME_DRIFT) {
        if (orig_flags & BCM_TIME_SYNC_STAMPER) {
            
        } else {
            /* Get value from hardware */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_interface_drift_get(unit, id, &(intf_ptr->drift)));
        }
    }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        if (intf->flags & BCM_TIME_REF_CLOCK) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_interface_ref_clock_get(unit, id,
                                            &(intf_ptr->clk_resolution)));
        }
    }
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */

    if (orig_flags & BCM_TIME_SYNC_STAMPER) {
        intf_ptr->flags = orig_flags;
    }

    *intf = *(TIME_INTERFACE(unit, id));

    return (BCM_E_NONE);
}

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
STATIC int
_bcm_esw_time_interface_dual_bs_get(int unit, bcm_time_if_t id, bcm_time_interface_t *intf)
{
    uint32                  regval;
    bcm_time_interface_t    *intf_ptr;
    uint32                  orig_flags;

    /* Feature unavailable on devices without BroadSync support. */
    if (soc_feature(unit, soc_feature_time_v3_no_bs)) {
        return BCM_E_UNAVAIL;
    }

    intf_ptr = TIME_INTERFACE(unit, id);
    orig_flags = intf_ptr->flags;
    intf_ptr->flags = intf->flags;
    intf_ptr->id = id;
    if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
        SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
        READ_NS_BS0_BS_CONFIGr(unit, &regval);

        /* Update output flags */
        if (TIME_MODE_INPUT == soc_reg_field_get(unit, NS_BS0_BS_CONFIGr,
                                                 regval, MODEf)) {
            intf_ptr->flags |= BCM_TIME_INPUT;
        } else {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }

       if (soc_reg_field_get(unit, NS_BS0_BS_CONFIGr,
                             regval, BS_CLK_OUTPUT_ENABLEf)) {
            intf_ptr->flags |= BCM_TIME_ENABLE;
        } else {
            intf_ptr->flags &= ~BCM_TIME_ENABLE;
        }

        READ_NS_BS0_BS_OUTPUT_TIME_0r(unit, &regval);
        if (soc_reg_field_get(unit, NS_BS0_BS_OUTPUT_TIME_0r,
                              regval, LOCKf)) {
            intf_ptr->flags |= BCM_TIME_LOCKED;
        } else {
            intf_ptr->flags &= ~BCM_TIME_LOCKED;
        }
    } else if (SOC_IS_FIRELIGHT(unit)) {
        READ_IPROC_NS_BS0_BS_CONFIGr(unit, &regval);

        /* Update output flags */
        if (TIME_MODE_INPUT == soc_reg_field_get(unit, IPROC_NS_BS0_BS_CONFIGr,
                                                 regval, MODEf)) {
            intf_ptr->flags |= BCM_TIME_INPUT;
        } else {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }

       if (soc_reg_field_get(unit, IPROC_NS_BS0_BS_CONFIGr,
                             regval, BS_CLK_OUTPUT_ENABLEf)) {
            intf_ptr->flags |= BCM_TIME_ENABLE;
        } else {
            intf_ptr->flags &= ~BCM_TIME_ENABLE;
        }

        READ_IPROC_NS_BS0_BS_OUTPUT_TIME_0r(unit, &regval);
        if (soc_reg_field_get(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r,
                              regval, LOCKf)) {
            intf_ptr->flags |= BCM_TIME_LOCKED;
        } else {
            intf_ptr->flags &= ~BCM_TIME_LOCKED;
        }
    } else {
        READ_CMIC_BS0_CONFIGr(unit, &regval);

        /* Update output flags */
        if (TIME_MODE_INPUT == soc_reg_field_get(unit, CMIC_BS0_CONFIGr,
                                                 regval, MODEf)) {
            intf_ptr->flags |= BCM_TIME_INPUT;
        } else {
            intf_ptr->flags &= ~BCM_TIME_INPUT;
        }

       if (soc_reg_field_get(unit, CMIC_BS0_CONFIGr,
                             regval, BS_CLK_OUTPUT_ENABLEf)) {
            intf_ptr->flags |= BCM_TIME_ENABLE;
        } else {
            intf_ptr->flags &= ~BCM_TIME_ENABLE;
        }

        READ_CMIC_BS0_OUTPUT_TIME_0r(unit, &regval);
        if (soc_reg_field_get(unit, CMIC_BS0_OUTPUT_TIME_0r,
                              regval, LOCKf)) {
            intf_ptr->flags |= BCM_TIME_LOCKED;
        } else {
            intf_ptr->flags &= ~BCM_TIME_LOCKED;
        }
    }


#if defined(INCLUDE_PTP)
    if (_bcm_ptp_running(unit) == BCM_E_NONE) {
        intf_ptr->flags = orig_flags;
        *intf = *intf_ptr;
        return BCM_E_NONE;
    }
#endif /* INCLUDE_PTP */

    if (intf->flags & BCM_TIME_ACCURACY) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_accuracy_get(unit, id,
                                                 &(intf_ptr->accuracy)));
    }

    if (intf->flags & BCM_TIME_OFFSET) {
        if (orig_flags & BCM_TIME_SYNC_STAMPER) {
            
        } else {
            /* Get value from hardware */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_interface_offset_get(unit, id, &(intf_ptr->offset)));
        }
    }

    if (intf->flags & BCM_TIME_DRIFT) {
        if (orig_flags & BCM_TIME_SYNC_STAMPER) {
            
        } else {
            /* Get value from hardware */
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_interface_drift_get(unit, id, &(intf_ptr->drift)));
        }
    }

    if (intf->flags & BCM_TIME_REF_CLOCK) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_ref_clock_get(unit, id,
                                        &(intf_ptr->clk_resolution)));
    }

    if (orig_flags & BCM_TIME_SYNC_STAMPER) {
        intf_ptr->flags = orig_flags;
    }

    *intf = *(TIME_INTERFACE(unit, id));

    return (BCM_E_NONE);
}
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */

/*
 * Function:
 *      _bcm_esw_time_hw_interrupt_dflt
 * Purpose:
 *      Default handler for broadsync heartbeat interrupt
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      Nothing
 */

STATIC void
_bcm_esw_time_hw_interrupt_dflt(int unit)
{
    uint32  regval;

    /* Due to HW constrains we need to reinable the interrupt on every rising */
    /* edge and update the capture mode */

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    uint32  intr, mode;

    if (soc_feature(unit, soc_feature_time_v3)) {
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit) || SOC_IS_FIRELIGHT(unit)) {
            
            return;
        } else {
            /* Read original values */
            READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &mode);
            READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
            READ_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, &intr);

            /* Disable all */
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, 0);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, 0);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, 0);

            /* Enable with the original settings */
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, intr);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);
            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, mode);
        }

    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, 0);
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
    } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
    {
        READ_CMIC_BS_CAPTURE_CTRLr(unit, &regval);
        WRITE_CMIC_BS_CAPTURE_CTRLr(unit, 0);
        WRITE_CMIC_BS_CAPTURE_CTRLr(unit, regval);
    }
    return;
}

/*
 * Function:
 *      _bcm_esw_time_hw_interrupt
 * Purpose:
 *      Handles broadsync heartbeat interrupt
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_esw_time_hw_interrupt(int unit)
{
    void                    *u_data;
    bcm_time_heartbeat_cb   u_cb;
    int                     idx;    /* interface itterator */
    bcm_time_capture_t      time;

    for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
        if (NULL != TIME_INTERFACE(unit, idx) &&
            NULL != TIME_INTERFACE_CONFIG(unit,idx).user_cb) {
            u_cb = TIME_INTERFACE_CONFIG(unit,idx).user_cb->heartbeat_cb;
            u_data = TIME_INTERFACE_CONFIG(unit,idx).user_cb->user_data;
            _bcm_esw_time_capture_counter_read(unit, idx, &time);
            if (NULL != u_cb) {
                u_cb(unit, idx, &time, u_data);
            }
        }
    }

    _bcm_esw_time_hw_interrupt_dflt(unit);

    return;
}

/****************************************************************************/
/*                      API functions implementation                        */
/****************************************************************************/

/*
 * Function:
 *      bcm_esw_time_init
 * Purpose:
 *      Initialize time module
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_init (int unit)
{
    _bcm_time_config_p      time_cfg_ptr;   /* Pointer to Time module config */
    bcm_time_interface_t    *intf;          /* Time interfaces iterator.     */
    int                     alloc_sz;       /* Memory allocation size.       */
    int                     idx;            /* Time interface array iterator.*/
    int                     rv;             /* Return Value.                 */
    soc_control_t *soc = SOC_CONTROL(unit); /* Soc control structure.        */
#if defined(BCM_TIME_NANOSYNC_SUPPORT)
    soc_reg_t               ns_reg;
    uint32_t                regVal = 0;
    uint32_t                eventId;
    int                     misc_start;     /* Miscellaneous start iterator. */
    int                     misc_end;       /* Miscellaneous end iterator.   */
#endif /* BCM_TIME_NANOSYNC_SUPPORT */

    /* Check if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* If already initialized then deinitialize time module */
    if (TIME_INIT(unit)) {
        _bcm_esw_time_deinit(unit, &TIME_CONFIG(unit));
    }

    /* Allocate time config structure. */
    alloc_sz = sizeof(_bcm_time_config_t);
    time_cfg_ptr = sal_alloc(alloc_sz, "Time module");
    if (NULL == time_cfg_ptr) {
        return (BCM_E_MEMORY);
    }
    sal_memset(time_cfg_ptr, 0, alloc_sz);

    /* Currently only one interface per unit */
    time_cfg_ptr->intf_count = NUM_TIME_INTERFACE(unit);

    /* Allocate memory for all time interfaces, supported */
    alloc_sz = time_cfg_ptr->intf_count * sizeof(_bcm_time_interface_config_t);
    time_cfg_ptr->intf_arr = sal_alloc(alloc_sz, "Time Interfaces");
    if (NULL == time_cfg_ptr->intf_arr) {
        _bcm_esw_time_deinit(unit, &time_cfg_ptr);
        return (BCM_E_MEMORY);
    }
    sal_memset(time_cfg_ptr->intf_arr, 0, alloc_sz);
    for (idx = 0; idx < time_cfg_ptr->intf_count; idx++) {
        intf = &time_cfg_ptr->intf_arr[idx].time_interface;
        intf->id = idx;
    }

    /* For each time interface allocate memory for tuser_cb */
    alloc_sz = sizeof(_bcm_time_user_cb_t);
    for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
        time_cfg_ptr->intf_arr[idx].user_cb =
            sal_alloc(alloc_sz, "Time Interface User Callback");
        if (NULL == time_cfg_ptr->intf_arr[idx].user_cb) {
            _bcm_esw_time_deinit(unit,  &time_cfg_ptr);
            return (BCM_E_MEMORY);
        }
        sal_memset(time_cfg_ptr->intf_arr[idx].user_cb, 0, alloc_sz);
#ifdef INCLUDE_GDPLL
        time_cfg_ptr->intf_arr[idx].dpll_chan = BCM_TIME_DPLL_CHAN_INVALID;
        time_cfg_ptr->intf_arr[idx].bs_thread = SAL_THREAD_ERROR;
#endif /* INCLUDE_GDPLL */
    }

    /* Interrupt handling function initialization */
    soc->time_call_ref_count = 0;
    soc->soc_time_callout = NULL;

    /* Create protection mutex. */
    time_cfg_ptr->mutex = sal_mutex_create("Time mutex");
    if (NULL == time_cfg_ptr->mutex) {
        _bcm_esw_time_deinit(unit, &time_cfg_ptr);
        return (BCM_E_MEMORY);
    }

    sal_mutex_take(time_cfg_ptr->mutex, sal_mutex_FOREVER);

    TIME_CONFIG(unit) = time_cfg_ptr;

#ifdef INCLUDE_GDPLL
    TIME_DPLL_CHAN_NTP(unit) = BCM_TIME_DPLL_CHAN_INVALID;
    TIME_DPLL_CHAN_PTP(unit) = BCM_TIME_DPLL_CHAN_INVALID;
#endif /* INCLUDE_GDPLL */

#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_time_scache_alloc(unit);
#endif

    /* Clear memories/registers on Cold boot only */
    if (!SOC_WARM_BOOT(unit) &&
        _bcm_esw_time_hitless_reset(unit, idx) != BCM_E_NONE) {
    for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
        rv  = _bcm_esw_time_hw_clear(unit, idx);
        if (BCM_FAILURE(rv)) {
            TIME_UNLOCK(unit);
            _bcm_esw_time_deinit(unit, &time_cfg_ptr);
            TIME_CONFIG(unit) = NULL;
            return (BCM_E_MEMORY);
        }
    }
    } else {
        /* If Warm boot reinitialize module based on HW state */
#ifdef BCM_WARM_BOOT_SUPPORT
        for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
            rv = _bcm_esw_time_reinit(unit, idx);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                _bcm_esw_time_deinit(unit, &time_cfg_ptr);
                TIME_CONFIG(unit) = NULL;
                return (rv);
            }
        }

#endif /* BCM_WARM_BOOT_SUPPORT */
    }

    if ((soc_feature(unit, soc_feature_ptp_cf_sw_update)) &&
        (soc_property_get(unit, spn_PTP_CF_SW_UPDATE, 0))) {
        /* Enable 1588 indication in Higig extension header */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, HG_EH_CONTROLr, REG_PORT_ANY,
                                    EH_1588_INDICATION_ENABLEf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_HG_EH_CONTROL_64r, REG_PORT_ANY,
                                 EH_1588_INDICATION_ENABLEf, 1));
    }

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_nanosync)) {
        /*
         * Set the event ids for Nanosync, both miscellaneous and port events
         *
         * EventId 0 to 191 for port events for 64-ports
         * Events could be RXCDR/RXSOF, ROE, TXSOF/TXPI
         * 0, 2, 4...126 -> RXCDR/RXSOF
         * 1, 3, 5...127 -> ROE
         * 128, 129, 130..191 -> TXSOF/TXPI
         */

        /* Set the Miscellaneous event id's */
        eventId = BCM_TIME_CAPTURE_IA_START_MISC;
        if (SOC_REG_IS_VALID(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr)) {
            ns_reg = NS_TIMESYNC_TS_EVENT_FWD_CFGr;
        } else {
            ns_reg = IPROC_NS_TIMESYNC_TS_EVENT_FWD_CFGr;
        }

        BCM_IF_ERROR_RETURN(
            bcmi_time_capture_event_misc_events_range(
                unit, &misc_start, &misc_end));

        for (idx = misc_start; idx < misc_end; idx++) {
            BCM_TIME_READ_NS_REGr(unit, ns_reg, idx, &regVal);
            soc_reg_field_set(unit, ns_reg, &regVal,
                              EVENT_IDf, eventId);
            BCM_TIME_WRITE_NS_REGr(unit, ns_reg, idx, regVal);
            eventId++;
        }

        
    }
    /* Check availability of TS FIFO to host */
    tsfifo_flag = soc_property_get(unit, "tsfifo_available_flag", 1);
#endif /* BCM_TIME_NANOSYNC_SUPPORT */

#if defined(BCM_TIMESYNC_LIVE_WIRE_TOD_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_live_wire_tod)) {
        /* The MSB nibble is the increment value in nanosecond */
        BCM_TIME_READ_NS_REGr(unit, NS_IEEE1588_TIME_FREQ_CONTROL_UPPERr, 0, &regVal);
        soc_reg_field_set(unit, NS_IEEE1588_TIME_FREQ_CONTROL_UPPERr, &regVal,
                          VALf, 0x2000);
        BCM_TIME_WRITE_NS_REGr(unit, NS_IEEE1588_TIME_FREQ_CONTROL_UPPERr, 0, regVal);

        /*
         * NS_IEEE1588_TIME_FREQ_CONTROL_UPPER[15:12]
         * says by how many nanoseconds the counter needs to be incremented
         * every clock tick.
         * 2ns is the clock period of ts_clk - on which the counter ticks.
         */
        BCM_TIME_READ_NS_REGr(unit, NS_IEEE1588_TIME_CONTROL_LOADr, 0, &regVal);
        soc_reg_field_set(unit, NS_IEEE1588_TIME_CONTROL_LOADr, &regVal,
                          LOAD_TYPEf, 0x0);
        soc_reg_field_set(unit, NS_IEEE1588_TIME_CONTROL_LOADr, &regVal,
                          ENABLEf, 0x1);
        BCM_TIME_WRITE_NS_REGr(unit, NS_IEEE1588_TIME_CONTROL_LOADr, 0, regVal);

        /* Enable TOD's counter */
        BCM_TIME_READ_NS_REGr(unit, NS_IEEE1588_TIME_CONTROLr, 0, &regVal);
        soc_reg_field_set(unit, NS_IEEE1588_TIME_CONTROLr, &regVal,
                          COUNTER_ENABLEf, 0x1);
        BCM_TIME_WRITE_NS_REGr(unit, NS_IEEE1588_TIME_CONTROLr, 0, regVal);
    }
#endif /* BCM_TIMESYNC_LIVE_WIRE_TOD_SUPPORT */

    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_deinit
 * Purpose:
 *      Uninitialize time module
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_deinit (int unit)
{
    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    if (0 == TIME_INIT(unit)) {
        return (BCM_E_INIT);
    }

    return _bcm_esw_time_deinit(unit, &TIME_CONFIG(unit));
}

static int _bcm_time_interface_traverse_cb(int unit,
                            bcm_time_interface_t *intf, void *user_data)
{
    uint8 *ptr = (uint8 *)user_data;
    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
        *ptr = 1;
        return BCM_E_NONE;
     } else {
        *ptr = 0;
        return BCM_E_NOT_FOUND;
     }
}

/*
 * Function:
 *      bcm_esw_time_bs_log_config_set
 * Purpose:
 *      Configure Broadsync logging
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_bs_log_configure_set (int unit, bcm_time_bs_log_cfg_t bs_log_cfg)
{
    uint8 bs_initialized=0;

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    bcm_esw_time_interface_traverse(unit, _bcm_time_interface_traverse_cb,
                                        (void*)&bs_initialized);
    if (!bs_initialized) {
        return BCM_E_INIT;
    }

    _bcm_time_bs_debug(bs_log_cfg.debug_mask);

    return _bcm_time_bs_log_configure(unit, bs_log_cfg.debug_mask,
                    bs_log_cfg.udp_log_mask, bs_log_cfg.src_mac,
                    bs_log_cfg.dest_mac, bs_log_cfg.tpid, bs_log_cfg.vid,
                    bs_log_cfg.ttl, bs_log_cfg.src_addr, bs_log_cfg.dest_addr,
                    bs_log_cfg.udp_port);
}

/*
 * Function:
 *      bcm_esw_time_bs_log_config_get
 * Purpose:
 *      Configure Broadsync logging
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_bs_log_configure_get (int unit, bcm_time_bs_log_cfg_t *bs_log_cfg)
{
    uint8 bs_initialized=0;

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    bcm_esw_time_interface_traverse(unit, _bcm_time_interface_traverse_cb,
                                        (void*)&bs_initialized);
    if (!bs_initialized) {
        return BCM_E_INIT;
    }

    return _bcm_time_bs_log_configure_get(unit, bs_log_cfg);

}

/*
 * Function:
 *      bcm_esw_time_interface_add
 * Purpose:
 *      Adding a time sync interface to a specified unit
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      intf - (IN/OUT) Time Sync Interface
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_interface_add (int unit, bcm_time_interface_t *intf)
{
    int rv = BCM_E_NONE; /* Return value */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_HELIX4_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || \
    defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_SABER2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || \
    defined(BCM_FIRELIGHT_SUPPORT)
    int replacing = 0;
    int reset_hardware = 0;
#endif

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* Check if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_input_validate(unit, intf));

    if (TIME_CONFIG(unit) == NULL) {
        return (BCM_E_INIT);
    }

    TIME_LOCK(unit);
    if (intf->flags & BCM_TIME_WITH_ID) {
        /* If interface already been in use */
        if (0 != TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id)) {
            if (0 == (intf->flags & BCM_TIME_REPLACE)) {
                TIME_UNLOCK(unit);
                return BCM_E_EXISTS;
            }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || \
    defined(BCM_HELIX4_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || \
    defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_SABER2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || \
    defined(BCM_FIRELIGHT_SUPPORT)
            replacing = 1;
#endif
        } else {
            TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id) = 1;
        }
    } else {
        rv = _bcm_esw_time_interface_allocate_id(unit, &(intf->id));
        if (BCM_FAILURE(rv)) {
            TIME_UNLOCK(unit);
            return rv;
        }
    }

#if defined(BCM_TIME_MANAGED_BROADSYNC)
    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
#if defined(INCLUDE_PTP)
        if (_bcm_ptp_running(unit) == BCM_E_NONE) {
            /* Unit is running PTP, so let PTP firmware know if timecodes should be output */
            int tc_output_enable = 0;
            int locked = 0;
            int include_servo_offset = 0;
            int needs_restart = 1;
            /* if either enable or "ref_clock" is changed, we'll need to stop and/or restart */
            uint32 restart_mask = (BCM_TIME_ENABLE | BCM_TIME_INPUT | BCM_TIME_REF_CLOCK);
            unsigned two_cycle_delay_usec = 500;  /* two cycles @ 4KHz */

            if ((intf->flags & BCM_TIME_ENABLE) &&
                !(intf->flags & BCM_TIME_INPUT)) {
                tc_output_enable = 1;
            }
            if (intf->flags & BCM_TIME_REF_CLOCK) {
                include_servo_offset = 1;
            }
            if (intf->flags & BCM_TIME_LOCKED) {
                locked = 1;
            }

            if (replacing) {
                /* clear 'needs_restart' if the only change is to the LOCK status */
                needs_restart = ((intf->flags & restart_mask) !=
                                 ((TIME_INTERFACE(unit, intf->id))->flags & restart_mask));
            }

#if defined(BCM_CMICM_SUPPORT)
            if (soc_feature(unit, soc_feature_cmicm) && /* CMICM, not iProc */
                !soc_feature(unit, soc_feature_iproc)) {
                if (needs_restart) {
                    soc_reg_field32_modify(unit, CMIC_BS_CONFIGr, REG_PORT_ANY, BS_TC_OUTPUT_ENABLEf, 0);
                    /* use "lock" field as a signal on startup that the servo offset should be included */
                    soc_reg_field32_modify(unit, CMIC_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, include_servo_offset);

                    sal_usleep(two_cycle_delay_usec);

                    soc_reg_field32_modify(unit, CMIC_BS_CONFIGr, REG_PORT_ANY, BS_TC_OUTPUT_ENABLEf, tc_output_enable);

                    sal_usleep(two_cycle_delay_usec);
                }
                soc_reg_field32_modify(unit, CMIC_BS_OUTPUT_TIME_0r, REG_PORT_ANY, LOCKf, locked);
            } else
#endif /* BCM_CMICM_SUPPORT */
#if defined (BCM_IPROC_SUPPORT)
            {
                if (soc_feature(unit, soc_feature_iproc)) {  /* IPROC/CMICD */
                    if (needs_restart) {
                        soc_reg_field32_modify(unit, CMIC_BS0_CONFIGr,
                                               REG_PORT_ANY,
                                               BS_TC_OUTPUT_ENABLEf, 0);
                        /* use "lock" field as a signal on startup that the servo offset should be included */
                        soc_reg_field32_modify(unit, CMIC_BS0_OUTPUT_TIME_0r,
                                               REG_PORT_ANY,
                                               LOCKf, include_servo_offset);

                        sal_usleep(two_cycle_delay_usec);

                        soc_reg_field32_modify(unit, CMIC_BS0_CONFIGr,
                                               REG_PORT_ANY,
                                               BS_TC_OUTPUT_ENABLEf, tc_output_enable);

                        sal_usleep(two_cycle_delay_usec);
                    }
                    soc_reg_field32_modify(unit, CMIC_BS0_OUTPUT_TIME_0r,
                                           REG_PORT_ANY,
                                           LOCKf, locked);
                }
            }
#endif /* BCM_IPROC_SUPPORT */
#if defined(BCM_TIME_NANOSYNC_SUPPORT)
        {
            /* IPROC/CMICD */
            if (soc_feature(unit, soc_feature_timesync_nanosync)) {
                if (SOC_IS_FIRELIGHT(unit)) {
                    if (needs_restart) {
                        soc_reg_field32_modify(unit, IPROC_NS_BS0_BS_CONFIGr,
                                               REG_PORT_ANY,
                                               BS_TC_OUTPUT_ENABLEf, 0);
                        /*
                         * use "lock" field as a signal on startup that the servo
                         * offset should be included
                         */
                        soc_reg_field32_modify(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r,
                                               REG_PORT_ANY,
                                               LOCKf, include_servo_offset);

                        sal_usleep(two_cycle_delay_usec);

                        soc_reg_field32_modify(unit, IPROC_NS_BS0_BS_CONFIGr,
                                               REG_PORT_ANY,
                                               BS_TC_OUTPUT_ENABLEf, tc_output_enable);

                        sal_usleep(two_cycle_delay_usec);
                    }
                    soc_reg_field32_modify(unit, IPROC_NS_BS0_BS_OUTPUT_TIME_0r,
                                           REG_PORT_ANY,
                                           LOCKf, locked);
                } else {
                    if (needs_restart) {
                        soc_reg_field32_modify(unit, NS_BS0_BS_CONFIGr,
                                               REG_PORT_ANY,
                                               BS_TC_OUTPUT_ENABLEf, 0);
                        /*
                         * use "lock" field as a signal on startup that the servo
                         * offset should be included
                         */
                        soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r,
                                               REG_PORT_ANY,
                                               LOCKf, include_servo_offset);

                        sal_usleep(two_cycle_delay_usec);

                        soc_reg_field32_modify(unit, NS_BS0_BS_CONFIGr,
                                               REG_PORT_ANY,
                                               BS_TC_OUTPUT_ENABLEf, tc_output_enable);

                        sal_usleep(two_cycle_delay_usec);
                    }
                    soc_reg_field32_modify(unit, NS_BS0_BS_OUTPUT_TIME_0r,
                                           REG_PORT_ANY,
                                           LOCKf, locked);
                }
            }
        }
#endif /* BCM_TIME_NANOSYNC_SUPPORT */
            /* copy the used flags */
            (TIME_INTERFACE(unit, intf->id))->flags &= ~(BCM_TIME_ENABLE | BCM_TIME_INPUT | BCM_TIME_REF_CLOCK | BCM_TIME_LOCKED);
            (TIME_INTERFACE(unit, intf->id))->flags |=
                intf->flags & (BCM_TIME_ENABLE | BCM_TIME_INPUT | BCM_TIME_REF_CLOCK | BCM_TIME_LOCKED);

            TIME_UNLOCK(unit);
            return (BCM_E_NONE);
        }
#endif /* INCLUDE_PTP */

        /* Set time interface configuration. */
        if (replacing) {
            if (((TIME_INTERFACE(unit, intf->id))->flags &
                 (BCM_TIME_ENABLE | BCM_TIME_INPUT)) !=
                (intf->flags & (BCM_TIME_ENABLE | BCM_TIME_INPUT))) {
                /* either master or enabled state changed, so copy those bits in flags */
                (TIME_INTERFACE(unit, intf->id))->flags &= ~(BCM_TIME_ENABLE | BCM_TIME_INPUT);
                (TIME_INTERFACE(unit, intf->id))->flags |=
                    intf->flags & (BCM_TIME_ENABLE | BCM_TIME_INPUT);
                /* either master or enabled state changed, so re-init */
                reset_hardware = 1;
            }
            if (intf->flags & BCM_TIME_DRIFT) {
                (TIME_INTERFACE(unit, intf->id))->drift = intf->drift;
            }
            if (intf->flags & BCM_TIME_OFFSET) {
                (TIME_INTERFACE(unit, intf->id))->offset = intf->offset;
            }
            if (intf->flags & BCM_TIME_ACCURACY) {
                (TIME_INTERFACE(unit, intf->id))->accuracy = intf->accuracy;
            }
            if (intf->flags & BCM_TIME_HEARTBEAT) {
                if ((TIME_INTERFACE(unit, intf->id))->heartbeat_hz != intf->heartbeat_hz) {
                    (TIME_INTERFACE(unit, intf->id))->heartbeat_hz = intf->heartbeat_hz;
                    /* Also use heartbeat flag to indicate whether to replace bitclock freq */
                    (TIME_INTERFACE(unit, intf->id))->bitclock_hz = intf->bitclock_hz;
                    reset_hardware = 1;
                }
            }
            if (intf->flags & BCM_TIME_BS_TIME) {
                (TIME_INTERFACE(unit, intf->id))->bs_time = intf->bs_time;
            }
        } else {
            /* Is new, so just copy wholesale */
            *(TIME_INTERFACE(unit, intf->id)) = *intf;
            reset_hardware = 1;
        }

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
#if defined(BCM_TIME_NANOSYNC_SUPPORT)
        if (soc_feature(unit, soc_feature_timesync_nanosync)) {
            uint32 regval;
            uint32 ts_common_ctrl_en;
            /* default to enable common control */
            ts_common_ctrl_en = soc_property_get(unit, spn_TS_COUNTER_COMBINED_MODE, 1);
            LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "common ctrl mode enabled[%d]\n"), ts_common_ctrl_en));
            if (SOC_IS_FIRELIGHT(unit)) {
                /* in case of broadsync, enable common control for both counters */
                READ_IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr,
                                  &regval, ENABLE_COMMON_CONTROLf, ts_common_ctrl_en);
                WRITE_IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);
            } else {
                /* in case of broadsync, enable common control for both counters */
                READ_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
                soc_reg_field_set(unit, NS_TIMESYNC_COUNTER_CONFIG_SELECTr, &regval,
                                  ENABLE_COMMON_CONTROLf, ts_common_ctrl_en);
                WRITE_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);
            }
        } else
#endif /* BCM_TIME_NANOSYNC_SUPPORT */
        if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
            uint32 regval;
            /* in case of broadsync, enable common control for both counters */
            READ_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr, &regval,
                              ENABLE_COMMON_CONTROLf, 1);
            WRITE_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);
        }
#endif /* defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC) */


        if (reset_hardware) {
            /* Enable/Setup TIME API BroadSync on the unit */
            rv = _bcm_esw_time_bs_init(unit, TIME_INTERFACE(unit, intf->id));
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                return rv;
            }
        }

        if (!replacing) {
            /* Enable BS firmware on the unit */
            rv = _bcm_mbox_comm_init(unit, MOS_MSG_MBOX_APPL_BS);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                return rv;
            }
        }

#ifdef INCLUDE_GDPLL
        if (!replacing) {
            _bcm_bs_dpll_chan_create(unit, TIME_INTERFACE(unit, intf->id));
        }
#endif

        /* Set BroadSync time */
        if (intf->flags & BCM_TIME_BS_TIME) {
            rv = _bcm_time_bs_time_set(unit, intf->bs_time);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                return rv;
            }
        }

        /* Set specified offsets */
        if (intf->flags & BCM_TIME_DRIFT) {
            rv = _bcm_time_bs_frequency_offset_set(unit, intf->drift);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                return rv;
            }
        }

        if (intf->flags & BCM_TIME_OFFSET) {
            rv = _bcm_time_bs_phase_offset_set(unit, intf->offset);
            if (BCM_FAILURE(rv)) {
                TIME_UNLOCK(unit);
                return rv;
            }
        }

        TIME_UNLOCK(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
        SOC_CONTROL_LOCK(unit);
        SOC_CONTROL(unit)->scache_dirty = 1;
        SOC_CONTROL_UNLOCK(unit);
#endif

        return (BCM_E_NONE);
    }

    if (SOC_HAS_MANAGED_BROADSYNC(unit)) {
        uint32          regval = 0;

        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            /* Enable Timestamp Generation */
            READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &regval);
            /* 500 Mhz NCO - 2.000 ns */
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &regval, UINCf,
                                            0x2000);
            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, regval);

            READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &regval, LINCf, 0);
            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

            READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
            WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

        } else if (SOC_IS_FIRELIGHT(unit)) {
            /* Enable Timestamp Generation */
            READ_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &regval);
            /* 500 Mhz NCO - 2.000 ns */
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr,
                              &regval, UINCf, 0x4000);
            WRITE_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, regval);

            READ_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr,
                              &regval, LINCf, 0);
            WRITE_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

            READ_IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr,
                              &regval, ENABLEf, 1);
            WRITE_IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);
        } else
#if defined (BCM_IPROC_SUPPORT)
        if (soc_feature(unit, soc_feature_iproc)) {
            /* Enable Timestamp Generation */
            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, &regval);
            /* 250 Mhz - 4.000 ns */
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr, &regval, FRACf,
                              ((soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0)
                                == 12800000 ) ? 0x40842108 : 0x40000000));
            WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, regval);

            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr, &regval, NSf, 0);
            WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, regval);

            READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
            WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);
        } else
#endif
        {
            /* Enable Timestamp Generation */
            READ_CMIC_TS_FREQ_CTRL_LOWERr(unit, &regval);
            /* 250 Mhz - 4.000 ns */
            soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_LOWERr, &regval, FRACf,
                              ((soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0)
                                == 12800000 ) ? 0x40842108 : 0x40000000));
            WRITE_CMIC_TS_FREQ_CTRL_LOWERr(unit, regval);

            READ_CMIC_TS_FREQ_CTRL_UPPERr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_UPPERr, &regval, NSf, 0);
            soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_UPPERr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_FREQ_CTRL_UPPERr(unit, regval);
        }

        if (intf->flags & BCM_TIME_REF_CLOCK) {
            rv = _bcm_esw_time_interface_ref_clock_install(unit, intf->id);
            if (BCM_FAILURE(rv)) {
                TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id) -= 1;
                TIME_UNLOCK(unit);
                return rv;
            }
        }
    }
#endif /* BCM_TIME_MANAGED_BROADSYNC */

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        uint32 regval;

        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            /* Enable common control for both counters */
            READ_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_COUNTER_CONFIG_SELECTr, &regval,
                              ENABLE_COMMON_CONTROLf, 1);
            WRITE_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);

            /* Enable Timestamp Generation */
            READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &regval);

            /* TH3 1GHz input to iProc 2.000 ns */
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &regval, UINCf,
                                            0x2000);
            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, regval);

            READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &regval, LINCf, 0);
            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

            READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
            WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);
        } else if (SOC_IS_FIRELIGHT(unit)) {
            /* Enable common control for both counters */
            READ_IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr,
                              &regval, ENABLE_COMMON_CONTROLf, 1);
            WRITE_IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);

            /* Enable Timestamp Generation */
            READ_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &regval);

            /* TH3 1GHz input to iProc 2.000 ns */
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr,
                              &regval, UINCf, 0x4000);
            WRITE_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, regval);

            READ_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr,
                              &regval, LINCf, 0);
            WRITE_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

            READ_IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr,
                              &regval, ENABLEf, 1);
            WRITE_IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);
        } else {
            uint32 offsethi = 0, offsetlo = 0;
            /* Enable common control for both counters */
            READ_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr, &regval,
                              ENABLE_COMMON_CONTROLf, 1);
            WRITE_CMIC_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, regval);

            /* 250 Mhz - 4.000 ns */
            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr, &regval,
                              FRACf,
                              ((soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0)
                              == 12800000 ) ? 0x40842108 : 0x40000000));
            WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, regval);

            if (intf->flags & BCM_TIME_OFFSET) {
                uint64 nanosec;
                /* need consider nanoseconds also */
                COMPILER_64_COPY(nanosec, intf->offset.seconds);
                COMPILER_64_UMUL_32(nanosec, 1000000000);
                COMPILER_64_ADD_32(nanosec, intf->offset.nanoseconds);
                offsethi = COMPILER_64_HI(nanosec) & 0xffff;
                offsetlo = COMPILER_64_LO(nanosec);
            }

            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr, &regval,
                              NSf, offsethi);
            WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, regval);

            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr, &regval,
                              NSf, offsetlo);
            WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, regval);

            /* Enable Timestamp Generation */
            READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &regval,
                              ENABLEf, 1);
            WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);
        }
        if (intf->flags & BCM_TIME_REF_CLOCK) {
            rv = _bcm_esw_time_interface_ref_clock_install(unit, intf->id);
            if (BCM_FAILURE(rv)) {
                TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id) -= 1;
                TIME_UNLOCK(unit);
                return rv;
            }
        }
    }
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */

    /* Set time interface configuration. */
    *(TIME_INTERFACE(unit, intf->id)) = *intf;

    /* Install the interface into the HW */
#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        if (!soc_feature(unit, soc_feature_time_v3_no_bs)) {
            rv = _bcm_esw_time_interface_dual_bs_install(unit, intf->id);
        }
    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
    {
        rv = _bcm_esw_time_interface_install(unit, intf->id);
    }

    if (BCM_FAILURE(rv)) {
        TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id) -= 1;
        TIME_UNLOCK(unit);
        return rv;
    }

    TIME_UNLOCK(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_interface_delete
 * Purpose:
 *      Deleting a time sync interface from unit
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      intf_id - (IN) Time Sync Interface id to remove
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_interface_delete (int unit, bcm_time_if_t intf_id)
{
    int rv;

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Initialization check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, intf_id));

    TIME_LOCK(unit);

    /* If interface still in use return an Error */
    if (1 < TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf_id)) {
        TIME_UNLOCK(unit);
        return (BCM_E_BUSY);
    }

#ifdef INCLUDE_GDPLL
    rv = _bcm_bs_dpll_chan_destroy(unit, intf_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(0, "### Error in destroy dpll chan ### \n")));
    }
#endif

    /* Free the interface */
    rv = _bcm_time_interface_free(unit, intf_id);
    if (BCM_FAILURE(rv)) {
        TIME_UNLOCK(unit);
        return (rv);
    }

    rv = _bcm_esw_time_hw_clear(unit, intf_id);

#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    TIME_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_time_interface_get
 * Purpose:
 *      Get a time sync interface on a specified unit
 * Parameters:
 *      unit -  (IN) StrataSwitch Unit #.
 *      intf - (IN/OUT) Time Sync Interface to get
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_interface_get (int unit, bcm_time_interface_t *intf)
{
    int rv;
#ifdef INCLUDE_GDPLL
    int dpll_chan;
    uint32 dpll_state;
#endif
    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Validation checks */
    if (NULL == intf) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, intf->id));

    TIME_LOCK(unit);
#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        rv = _bcm_esw_time_interface_dual_bs_get(unit, intf->id, intf);
    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
    {
        rv = _bcm_esw_time_interface_get(unit, intf->id, intf);
    }

    if (intf->flags & BCM_TIME_SYNC_STAMPER) {
        if (!soc_feature(unit, soc_feature_time_v3_no_bs)) {
            if(SOC_IS_MONTEREY(unit)) {
#ifdef INCLUDE_GDPLL
                dpll_chan = TIME_INTERFACE_CONFIG(unit, intf->id).dpll_chan;
                if (IS_TIME_DPLL_CHAN_VALID(unit, intf->id)) {
                    BCM_IF_ERROR_RETURN(bcm_esw_gdpll_chan_state_get(unit, dpll_chan, &dpll_state));
                    if (dpll_state & 0x1) {
                        intf->status = bcmBsTimeSourceLocked;
                    } else {
                        intf->status = bcmBsTimeSourceNotLocked;
                    }
                }
                LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            " intf->status[%d] dpll_state[0x%08x] \n"), intf->status, dpll_state));
#endif /* INCLUDE_GDPLL */
            } else {
                BCM_IF_ERROR_RETURN(_bcm_time_bs_status_get(unit,
                                 (int *)(&(intf->status))));
            }
        }
    }
    TIME_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *      bcm_esw_time_interface_delete_all
 * Purpose:
 *      Deleting all time sync interfaces on a unit
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_interface_delete_all (int unit)
{
    bcm_time_if_t   intf;

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Initialization check */
    if (0 == TIME_INIT(unit)) {
        return BCM_E_INIT;
    }

    for (intf = 0; intf < NUM_TIME_INTERFACE(unit); intf++ ) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_time_interface_delete(unit, intf));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_interface_traverse
 * Purpose:
 *      Itterates over all time sync interfaces and calls given callback
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      cb          - (IN) Call back function
 *      user_data   - (IN) void pointer to store any user information
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_interface_traverse (int unit, bcm_time_interface_traverse_cb cb,
                                 void *user_data)
{
    bcm_time_if_t   intf;
    int rv = BCM_E_NONE;

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Initialization check. */
    if (0 == TIME_INIT(unit)) {
        return BCM_E_INIT;
    }

    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    for (intf = 0; intf < NUM_TIME_INTERFACE(unit); intf++ ) {
        if (NULL != TIME_INTERFACE(unit, intf)) {
            rv = (cb(unit, TIME_INTERFACE(unit, intf), user_data));
            if (rv == BCM_E_NONE) {
                LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "traverse found i/f idx[%d] \n"), intf ));
                break;
            }
        }
    }
    return (rv);
}

/*
 * Function:
 *      bcm_esw_time_capture_get
 * Purpose:
 *      Gets a time captured by HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time interface identifier
 *      time    - (OUT) Structure to contain HW clocks values
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_capture_get (int unit, bcm_time_if_t id, bcm_time_capture_t *time)
{
    int rv;   /* Operation return status. */

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Initialization check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

    if (NULL == time) {
        return (BCM_E_PARAM);
    }
    if (NULL == TIME_CAPTURE(unit, id)) {
        return (BCM_E_NOT_FOUND);
    }

    TIME_LOCK(unit);
    rv = _bcm_esw_time_capture_get(unit, id, TIME_CAPTURE(unit, id), time->flags);
    if (BCM_FAILURE(rv)) {
        TIME_UNLOCK(unit);
        return (rv);
    }

    *time = *(TIME_CAPTURE(unit, id));

    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_heartbeat_enable_set
 * Purpose:
 *      Enables/Disables interrupt handling for each heartbeat provided by a
 *      HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time Sync Interface Id
 *      enable  - (IN) Enable/Disable parameter
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_heartbeat_enable_set (int unit, bcm_time_if_t id, int enable)
{
    uint32          regval;
    soc_control_t   *soc = SOC_CONTROL(unit);

    /* Check if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Feature unavailable on devices without BroadSync support. */
    if (soc_feature(unit, soc_feature_time_v3_no_bs)) {
        return BCM_E_UNAVAIL;
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

    TIME_LOCK(unit);

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || \
    defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if ((TIME_INTERFACE(unit, id))->flags & BCM_TIME_SYNC_STAMPER) {
        int rv = _bcm_time_bs_debug_1pps_set(unit, enable);
        if (BCM_FAILURE(rv)) {
            TIME_UNLOCK(unit);
            return rv;
        }

        TIME_UNLOCK(unit);
        return BCM_E_NONE;
    }
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) ||
          defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) ||
          defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) */

    /* Install heartbeat and external trigger clock interval */
#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, ENABLEf,
                (enable) ? IPROC_TIME_CAPTURE_MODE_HB_BS_0 : IPROC_TIME_CAPTURE_MODE_DISABLE);
            WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);

            if (enable) {
                /* Configure the HW to raise interrupt on every heartbeat */
                READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, ENABLEf, 1);
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_TX_HB_STATUS_ENABLEf, 1);
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_RX_HB_STATUS_ENABLEf, 1);
                WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
            } else {
                /* Leave TIME_CAPTURE_ENABLEf for other trigger sources */
                READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_RX_HB_STATUS_ENABLEf, 0);
                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regval, BS0_TX_HB_STATUS_ENABLEf, 0);
                WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
            }
        } else if (SOC_IS_FIRELIGHT(unit)) {
            READ_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
                              ENABLEf,
                              (enable) ? IPROC_TIME_CAPTURE_MODE_HB_BS_0 :
                              IPROC_TIME_CAPTURE_MODE_DISABLE);
            WRITE_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, regval);

            if (enable) {
                /* Configure the HW to raise interrupt on every heartbeat */
                READ_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
                                  ENABLEf, 1);
                soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
                                  BS0_TX_HB_STATUS_ENABLEf, 1);
                soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
                                  BS0_RX_HB_STATUS_ENABLEf, 1);
                WRITE_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
            } else {
                /* Leave TIME_CAPTURE_ENABLEf for other trigger sources */
                READ_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
                                  BS0_RX_HB_STATUS_ENABLEf, 0);
                soc_reg_field_set(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr, &regval,
                                  BS0_TX_HB_STATUS_ENABLEf, 0);
                WRITE_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, regval);
            }
        } else {
            READ_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_INTERRUPT_ENABLEr, &regval, INT_ENABLEf,
                (enable) ? IPROC_TIME_CAPTURE_MODE_HB_BS_0 : IPROC_TIME_CAPTURE_MODE_DISABLE);
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, regval);

            if (enable) {
                /* Configure the HW to raise interrupt on every heartbeat */
                READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, TIME_CAPTURE_ENABLEf, 1);
                soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_TX_HB_STATUS_ENABLEf, 1);
                soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_RX_HB_STATUS_ENABLEf, 1);
                WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);
            } else {
                /* Leave TIME_CAPTURE_ENABLEf for other trigger sources */
                READ_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_RX_HB_STATUS_ENABLEf, 0);
                soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr, &regval, BSYNC0_TX_HB_STATUS_ENABLEf, 0);
                WRITE_CMIC_TIMESYNC_TIME_CAPTURE_CONTROLr(unit, regval);
            }

            /* Configure the HW to capture time on every heartbeat */
            READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TIME_CAPTURE_MODEr, &regval, TIME_CAPTURE_MODEf,
                (enable) ? IPROC_TIME_CAPTURE_MODE_HB_BS_0 : IPROC_TIME_CAPTURE_MODE_DISABLE);

            WRITE_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, regval);

            /* set interrupt enables to match the mode */
            WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, regval);
        }


    } else

#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
    if (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        /* Configure the HW to raise interrupt on every heartbeat */
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, INT_ENABLEf,
                          (enable) ? TIME_CAPTURE_MODE_HEARTBEAT : 0);
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, TIME_CAPTURE_ENABLEf, 1);
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, TX_HB_STATUS_ENABLEf, 1);
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, RX_HB_STATUS_ENABLEf, 1);

        /* Configure the HW to capture time on every heartbeat */
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                          (enable) ? TIME_CAPTURE_MODE_HEARTBEAT : TIME_CAPTURE_MODE_DISABLE);
        WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);
    } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
    {
        READ_CMIC_BS_CAPTURE_CTRLr(unit, &regval);
        /* Configure the HW to give interrupt on every heartbeat */
        soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, INT_ENf, (enable) ? 1: 0);
        /* Configure the HW to capture time on every heartbeat */
        soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                          (enable) ? TIME_CAPTURE_MODE_HEARTBEAT : TIME_CAPTURE_MODE_DISABLE);
        WRITE_CMIC_BS_CAPTURE_CTRLr(unit, regval);
    }

    TIME_UNLOCK(unit);

    /* Enable/disable broadsync interrupt */
    if (enable) {
        soc_intr_enable(unit, IRQ_BROADSYNC_INTR);
        if (!soc->time_call_ref_count) {
            soc->soc_time_callout = _bcm_esw_time_hw_interrupt_dflt;
        }

    } else {
        soc_intr_disable(unit, IRQ_BROADSYNC_INTR);
        if (!soc->time_call_ref_count) {
            soc->soc_time_callout = NULL;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_heartbeat_enable_get
 * Purpose:
 *      Gets interrupt handling status for each heartbeat provided by a
 *      HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time Sync Interface Id
 *      enable  - (OUT) Enable status of interrupt handling
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_heartbeat_enable_get (int unit, bcm_time_if_t id, int *enable)
{
    uint32      regval;

    /* Check if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Feature unavailable on devices without BroadSync support. */
    if (soc_feature(unit, soc_feature_time_v3_no_bs)) {
        return BCM_E_UNAVAIL;
    }

    /* Param validation check. */
    if (enable == NULL) {
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

    /* Read HW Configuration */
#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) ||
            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
            READ_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            *enable = soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr, regval, ENABLEf) &
                    soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr, regval, BS0_TX_HB_STATUS_ENABLEf) &
                    soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr, regval, BS0_RX_HB_STATUS_ENABLEf);
        } else if (SOC_IS_FIRELIGHT(unit)) {
            READ_IPROC_NS_MISC_CLK_EVENT_CTRLr(unit, &regval);
            *enable = soc_reg_field_get(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr,
                                        regval, ENABLEf) &
                      soc_reg_field_get(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr,
                                        regval, BS0_TX_HB_STATUS_ENABLEf) &
                      soc_reg_field_get(unit, IPROC_NS_MISC_CLK_EVENT_CTRLr,
                                        regval, BS0_RX_HB_STATUS_ENABLEf);
        } else {
            READ_CMIC_TIMESYNC_TIME_CAPTURE_MODEr(unit, &regval);
            *enable = (regval & TIME_CAPTURE_MODE_HEARTBEAT)? 1 : 0;
        }
    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
    if (SOC_IS_KATANA(unit) || SOC_IS_TRIUMPH3(unit) ||
        SOC_IS_TD2_TT2(unit)) {
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);
        *enable = soc_reg_field_get(unit, CMIC_TS_TIME_CAPTURE_CTRLr,
                                    regval, INT_ENABLEf);
        *enable = (*enable ==  TIME_CAPTURE_MODE_HEARTBEAT) ? 1 : 0;
    } else
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */
    {
        READ_CMIC_BS_CAPTURE_CTRLr(unit, &regval);
        *enable = soc_reg_field_get(unit, CMIC_BS_CAPTURE_CTRLr, regval, INT_ENf);
    }

    return (BCM_E_NONE);
}


#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
/*
 * Function:
 *      _bcm_esw_time_trigger_to_timestamp_mode
 * Purpose:
 *      Convert capture mode flags to time control capture modes
 *
 * Parameters:
 *      unit       - (IN) mode_flags
 *
 * Returns:
 *      Timesync captude modes
 * Notes:
 */
STATIC uint32
_bcm_esw_time_trigger_to_timestamp_mode(int unit, uint32 mode_flags)
{
    uint32 capture_mode = 0;
    uint32 regval;
    int    bit_cnt;

    /* processing on user mode flags reduces the dependencies on
     * hardware bit maps, byte shift of mode flags
     * should have been efficient
     */
    for (bit_cnt = 0; bit_cnt < sizeof(uint32); bit_cnt++)
    {
        switch (mode_flags & (1 << bit_cnt))
        {
        case BCM_TIME_CAPTURE_GPIO_0:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_0;
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_1_UP_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_1_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_1_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_1_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_1_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_1_CTRLr(unit, regval);
            }
            break;

        case BCM_TIME_CAPTURE_GPIO_1:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_1;
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_1_DOWN_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_1_UP_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_1_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_1_UP_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_1_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_1_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_1_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_1_CTRLr(unit, regval);
            }
            break;

        case BCM_TIME_CAPTURE_GPIO_2:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_2;
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_2_DOWN_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_2_UP_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_2_UP_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_2_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_2_UP_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_2_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_2_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_2_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_2_CTRLr(unit, regval);
            }
            break;

        case BCM_TIME_CAPTURE_GPIO_3:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_3;
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_3_DOWN_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_3_UP_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_3_UP_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_3_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_3_UP_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_3_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_3_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_3_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_3_CTRLr(unit, regval);
            }
            break;

        case BCM_TIME_CAPTURE_GPIO_4:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_4;
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_4_DOWN_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_4_UP_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_4_UP_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_4_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_4_UP_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_4_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_4_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_4_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_4_CTRLr(unit, regval);
            }
            break;

        case BCM_TIME_CAPTURE_GPIO_5:
            capture_mode |=  TIME_CAPTURE_MODE_GPIO_5;
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_5_DOWN_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_5_UP_EVENT_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_5_UP_EVENT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_5_UP_EVENT_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_5_UP_EVENT_CTRLr(unit, regval);
            }
            if (soc_reg_field_valid(unit, CMIC_TS_GPIO_5_CTRLr, ENABLEf)) {
                READ_CMIC_TS_GPIO_5_CTRLr(unit, &regval);
                soc_reg_field_set(unit, CMIC_TS_GPIO_5_CTRLr, &regval, ENABLEf, 1);
                WRITE_CMIC_TS_GPIO_5_CTRLr(unit, regval);
            }
            break;

        case BCM_TIME_CAPTURE_L1_CLOCK_PRIMARY:
            capture_mode |=  TIME_CAPTURE_MODE_L1_CLOCK_PRIMARY;
            READ_CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_L1_CLK_RECOVERED_CLK_COUNT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_L1_CLOCK_SECONDARY:
            capture_mode |=  TIME_CAPTURE_MODE_L1_CLOCK_SECONDARY;
            READ_CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_L1_CLK_RECOVERED_CLK_BKUP_COUNT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_LCPLL:
            capture_mode |= TIME_CAPTURE_MODE_LCPLL;
            READ_CMIC_TS_LCPLL_CLK_COUNT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_LCPLL_CLK_COUNT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_LCPLL_CLK_COUNT_CTRLr(unit, regval);
            break;

        case BCM_TIME_CAPTURE_IP_DM_0:
            capture_mode |= TIME_CAPTURE_MODE_IP_DM_0;
            break;

        case BCM_TIME_CAPTURE_IP_DM_1:
            capture_mode |= TIME_CAPTURE_MODE_IP_DM_1;
            break;

        default:
            break;
        }
    }

    return capture_mode;
}

/*
 * Function:
 *      _bcm_esw_time_trigger_from_timestamp_mode
 * Purpose:
 *      Convert Timesync capture modes to mode flags
 *
 * Parameters:
 *      unit       - (IN) capture_mode
 *
 * Returns:
 *      Capture mode flags
 * Notes:
 */

STATIC uint32
_bcm_esw_time_trigger_from_timestamp_mode (uint32 capture_mode)
{
    uint32 user_flags = 0;
    int    bit_cnt;

    /* processing on user mode flags reduces the dependencies on
     * hardware bit maps, byte shift of mode flags
     * should have been efficient
     */
    for (bit_cnt = 0; bit_cnt < sizeof(uint32); bit_cnt++)
    {
        switch (capture_mode & (1 << bit_cnt))
        {
        case TIME_CAPTURE_MODE_GPIO_0:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_0;
            break;
        case TIME_CAPTURE_MODE_GPIO_1:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_1;
            break;
        case TIME_CAPTURE_MODE_GPIO_2:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_2;
            break;
        case TIME_CAPTURE_MODE_GPIO_3:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_3;
            break;
        case TIME_CAPTURE_MODE_GPIO_4:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_4;
            break;
        case TIME_CAPTURE_MODE_GPIO_5:
            user_flags |=  BCM_TIME_CAPTURE_GPIO_5;
            break;
        case TIME_CAPTURE_MODE_L1_CLOCK_PRIMARY:
            user_flags |=  BCM_TIME_CAPTURE_L1_CLOCK_PRIMARY;
            break;
        case TIME_CAPTURE_MODE_L1_CLOCK_SECONDARY:
            user_flags |=  BCM_TIME_CAPTURE_L1_CLOCK_SECONDARY;
            break;
        case TIME_CAPTURE_MODE_LCPLL:
            user_flags |=  BCM_TIME_CAPTURE_LCPLL;
            break;
        case TIME_CAPTURE_MODE_IP_DM_0:
            user_flags |=  BCM_TIME_CAPTURE_IP_DM_0;
            break;
        case TIME_CAPTURE_MODE_IP_DM_1:
            user_flags |= BCM_TIME_CAPTURE_IP_DM_1;
            break;
        default:
            break;
        }
    }

    return user_flags;
}

#endif /* (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)) */

/*
 * Function:
 *      bcm_esw_time_trigger_enable_set
 * Purpose:
 *      Enables/Disables interrupt handling for external triggers
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      id          - (IN) Time Sync Interface Id
 *      mode_flags  - (IN) Enable/Disable parameter
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_trigger_enable_set(int unit, bcm_time_if_t id, uint32 mode_flags)
{
#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
    uint32          regval, capture_mode = 0;
    soc_control_t   *soc = SOC_CONTROL(unit);
#endif

    /* Check if time feature is supported on a device */
    if (!(soc_feature(unit, soc_feature_time_support) &&
          (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
           SOC_IS_TD2_TT2(unit)))) {
        return (BCM_E_UNAVAIL);
    }
#if defined(BCM_TIME_V3_SUPPORT)
    /* Not supported in 3rd generation time arch */
    if (soc_feature(unit, soc_feature_time_v3)) {
        return BCM_E_UNAVAIL;
        }
#endif /* defined(BCM_TIME_V3_SUPPORT) */


    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

    TIME_LOCK(unit);

#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
    /*
     *   Read time capture control capture modes and
     *   Convert to hardware cature mode bitmap.
     *   Enable interrupt for all capture mode bitmap and
     *   Write time capture mode.
     *
     *   If for ALL capture mode, ignore individual capture modes and
     *   Enable Interrupts.
     */
    READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);

    if (mode_flags & BCM_TIME_CAPTURE_ALL) {
        /* Enable all Time capture mechanisms */
        soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval,
                            TIME_CAPTURE_ENABLEf, 1 );

        /* Return and  ignore setting of individual capture modes */
        return BCM_E_NONE;
    }

    /* Set time capture modes */
    capture_mode = _bcm_esw_time_trigger_to_timestamp_mode (unit, mode_flags);

    /* Configure the HW with capture modes */
    soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval,
                    TIME_CAPTURE_MODEf, (capture_mode) ? capture_mode :
                    TIME_CAPTURE_MODE_DISABLE);

    /* Configure the HW to trigger interrupt for capture events */
    soc_reg_field_set(unit, CMIC_TS_TIME_CAPTURE_CTRLr, &regval, INT_ENABLEf,
                      (capture_mode) ? capture_mode : 0);

    WRITE_CMIC_TS_TIME_CAPTURE_CTRLr(unit, regval);

    TIME_UNLOCK(unit);

    /* Enable/disable broadsync interrupt */
    if (capture_mode) {
        /* Enable Interrupt */
        soc_intr_enable(unit, IRQ_BROADSYNC_INTR);
        if (!soc->time_call_ref_count) {
            soc->soc_time_callout = _bcm_esw_time_hw_interrupt_dflt;
        }

    } else {
        /* Check on the references and disable interrupt and handler */
        soc_intr_disable(unit, IRQ_BROADSYNC_INTR);
        if (!soc->time_call_ref_count) {
            soc->soc_time_callout = NULL;
        }
    }
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_trigger_enable_get
 * Purpose:
 *      Gets interrupt handling status for each heartbeat provided by a
 *      HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time Sync Interface Id
 *      enable  - (OUT) Enable status of interrupt handling
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_trigger_enable_get(int unit, bcm_time_if_t id, uint32 *mode_flags)
{
#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
    uint32      regval, capture_mode = 0;
#endif

    /* Chek if time feature is supported on a device */
    if (!(soc_feature(unit, soc_feature_time_support) &&
          (SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||
           SOC_IS_TD2_TT2(unit)))) {
        return (BCM_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TIME_V3_SUPPORT)
    /* Not supported in 3rd generation time arch */
    if (soc_feature(unit, soc_feature_time_v3)) {
        return BCM_E_UNAVAIL;
    }
#endif /* defined(BCM_TIME_V3_SUPPORT) */


#if (defined(BCM_KATANA2_SUPPORT) || defined(BCM_HELIX4_SUPPORT))
    /* Install heartbeat and external trigger clock interval */
    if (SOC_IS_KATANA2(unit) || SOC_IS_HELIX4(unit)) {
        /* Read HW Configuration */
        READ_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, &regval);

        /* Get interrupt enabled capture modes */
        capture_mode = soc_reg_field_get(unit, CMIC_TIMESYNC_INTERRUPT_ENABLEr, regval, INT_ENABLEf);
        *mode_flags = _bcm_esw_time_trigger_from_timestamp_mode (capture_mode);
    } else
#endif
    {
#if (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT))
        /* Read HW Configuration */
        READ_CMIC_TS_TIME_CAPTURE_CTRLr(unit, &regval);

        /* Get interrupt enabled capture modes */
        capture_mode = soc_reg_field_get(unit, CMIC_TS_TIME_CAPTURE_CTRLr, regval, INT_ENABLEf);

        *mode_flags = _bcm_esw_time_trigger_from_timestamp_mode (capture_mode);
#endif /* (defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)) */
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *      bcm_esw_time_heartbeat_register
 * Purpose:
 *      Registers a call back function to be called on each heartbeat
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      id          - (IN) Time Sync Interface Id
 *      f           - (IN) Function to register
 *      user_data   - (IN) void pointer to store any user information
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_heartbeat_register (int unit, bcm_time_if_t id, bcm_time_heartbeat_cb f,
                                 void *user_data)
{
    soc_control_t   *soc = SOC_CONTROL(unit);

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));
    if (NULL == f) {
        return (BCM_E_PARAM);
    }

    TIME_LOCK(unit);
    /* If HW interrupt handler been registered protect from race condition */
    if (soc->time_call_ref_count) {
        soc->soc_time_callout = _bcm_esw_time_hw_interrupt_dflt;
    }

    /* Register user call back */
    TIME_INTERFACE_CONFIG(unit, id).user_cb->heartbeat_cb = f;
    TIME_INTERFACE_CONFIG(unit, id).user_cb->user_data = user_data;

    /* After registering user cb function and user_data turn on
    HW interrupt handler */
    soc->soc_time_callout = _bcm_esw_time_hw_interrupt;
    /* Only single call back at a time is currently supported */
    soc->time_call_ref_count = 1;

    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_heartbeat_unregister
 * Purpose:
 *      Unregisters a call back function to be called on each heartbeat
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      id          - (IN) Time Sync Interface Id
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_time_heartbeat_unregister (int unit, bcm_time_if_t id)
{
    soc_control_t   *soc = SOC_CONTROL(unit);

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

    TIME_LOCK(unit);

    if (--soc->time_call_ref_count <= 0) {
        soc->time_call_ref_count = 0;
        soc->soc_time_callout = _bcm_esw_time_hw_interrupt_dflt;
    }
    TIME_INTERFACE_CONFIG(unit, id).user_cb->heartbeat_cb = NULL;
    TIME_INTERFACE_CONFIG(unit, id).user_cb->user_data = NULL;

    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_time_sw_dump
 * Purpose:
 *     Displays time information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_time_sw_dump(int unit)
{
    soc_control_t   *soc = SOC_CONTROL(unit);

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information TIME - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        " Time call reference counter = %d\n"), soc->time_call_ref_count));
    if (NULL != soc->soc_time_callout) {
        LOG_CLI((BSL_META_U(unit,
                            " Time interrupt handler address is = %p\n"),
                 (void *)(unsigned long)soc->soc_time_callout));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            " Time interrupt handler is NULL \n")));
    }
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */


/********************************** ESW BroadSync Time Support ***********************************/

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HELIX4_SUPPORT) || \
    defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || \
    defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) || \
    defined(BCM_SABER2_SUPPORT)
STATIC
int
_bcm_esw_time_bs_init(int unit, bcm_time_interface_t *intf)
{
    const int bspll_freq = 20000000; /* 20 MHz */

    uint32 regval = 0;

    int master = ((intf->flags & BCM_TIME_INPUT) == 0);
    int output_enable = master && ((intf->flags & BCM_TIME_ENABLE) != 0);

    int bitclk_divisor = bspll_freq / intf->bitclock_hz;
    int bitclk_high = bitclk_divisor / 2;
    int bitclk_low = bitclk_divisor - bitclk_high;
    int hb_divisor = intf->bitclock_hz / intf->heartbeat_hz;
    int hb_up = (hb_divisor > 200) ? 100 : (hb_divisor / 2);
    int hb_down = hb_divisor - hb_up;
    int bs_ndiv_int = 0;

    uint32 freq_ctrl_lower = 0x40000000;  /* 4ns increment, for 250MHz TSPLL output */

    if ( SOC_IS_MONTEREY(unit) || SOC_IS_FIRELIGHT(unit)) {
        hb_down = (hb_divisor / 2);
        hb_up = hb_divisor - hb_down;
    }

    LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit,
                    "\n id[%d] master[%d] out_en[%d] bc[%dHz] bc_div[%d] high[%d] low[%d] hb[%dHz] hb_div[%d] up[%d] down[%d] bs_ndiv_int[%d]\n"),
                    intf->id, master, output_enable, intf->bitclock_hz, bitclk_divisor, bitclk_high, bitclk_low, intf->heartbeat_hz, hb_divisor, hb_up, hb_down, bs_ndiv_int));

#if defined(BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC)
/*****************************************************************************/
/*   Perform KATANA2, HELIX4, HURRICANE2, GREYHOUND, SABER2, APACHE IPROC HW configuration   */
/*****************************************************************************/
    if (SOC_HAS_TIME_V3_OR_IPROC_MANAGED_BROADSYNC(unit)) {
        freq_ctrl_lower = 4;  /* 4ns increment, for 250MHz TSPLL output */

        if (SOC_IS_GREYHOUND(unit) || SOC_IS_SABER2(unit) ||
            SOC_IS_HURRICANE3(unit)) {
            /* Feature unavailable on devices without BroadSync support. */
            if (soc_feature(unit, soc_feature_time_v3_no_bs)) {
                return BCM_E_UNAVAIL;
            }
        }
        if ( SOC_IS_MONTEREY(unit) ) {
            uint32 ts_common_ctrl_en = 1;

            /* default to enable common control */
            ts_common_ctrl_en = soc_property_get(unit, spn_TS_COUNTER_COMBINED_MODE, 1);
            LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "common ctrl mode enabled[%d]\n"), ts_common_ctrl_en));

            if(IS_BROADSYNC0(intf->id)) {
                /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
                READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
                soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
                WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

                READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 1);
                WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);
            }

            if(IS_BROADSYNC1(intf->id)) {
                READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
                soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
                WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

                READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 1);
                WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);
            }

            /* configure BS0/BS1 Clock configuration registers */
            if(IS_BROADSYNC0(intf->id)) {
                READ_NS_BS0_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
                WRITE_NS_BS0_BS_CONFIGr(unit, regval);
            }

            if(IS_BROADSYNC1(intf->id)) {
                READ_NS_BS1_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
                WRITE_NS_BS1_BS_CONFIGr(unit, regval);
            }

            if(IS_BROADSYNC0(intf->id)) {
                /* 500 Mhz NCO - 2.000 ns */
                READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &regval);
                soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &regval, UINCf, 0x2000);
                WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, regval);

                READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
                soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &regval, LINCf, freq_ctrl_lower);
                WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

                READ_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr(unit, &regval);
                soc_reg_field_set(unit, NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr, &regval, FCLf, 0);
                WRITE_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr(unit, regval);

                READ_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr(unit, &regval);
                soc_reg_field_set(unit, NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr, &regval, FCUf, 0);
                WRITE_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr(unit, regval);

                READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
                soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
                WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

                if (ts_common_ctrl_en != 1) {
                    READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, &regval);
                    soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr, &regval, UINCf, 0x2000);
                    WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, regval);

                    READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
                    soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr, &regval, LINCf, freq_ctrl_lower);
                    WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, regval);

                    READ_NS_TIMESYNC_TS1_CURRENT_FREQ_CTRL_LOWERr(unit, &regval);
                    soc_reg_field_set(unit, NS_TIMESYNC_TS1_CURRENT_FREQ_CTRL_LOWERr, &regval, FCLf, 0);
                    WRITE_NS_TIMESYNC_TS1_CURRENT_FREQ_CTRL_LOWERr(unit, regval);

                    READ_NS_TIMESYNC_TS1_CURRENT_FREQ_CTRL_UPPERr(unit, &regval);
                    soc_reg_field_set(unit, NS_TIMESYNC_TS1_CURRENT_FREQ_CTRL_UPPERr, &regval, FCUf, 0);
                    WRITE_NS_TIMESYNC_TS1_CURRENT_FREQ_CTRL_UPPERr(unit, regval);

                    READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &regval);
                    soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &regval, ENABLEf, 1);
                    WRITE_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, regval);
                }
            }

            /* Setup BS0/BS1 HeartBeat Control registers */
            if(IS_BROADSYNC0(intf->id)) {
                READ_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
                WRITE_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval);
            }

            if(IS_BROADSYNC1(intf->id)) {
                READ_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
                WRITE_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval);
            }

            if(IS_BROADSYNC0(intf->id)) {
                READ_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
                WRITE_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);
            }

            if(IS_BROADSYNC1(intf->id)) {
                READ_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
                WRITE_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);
            }

            if(IS_BROADSYNC0(intf->id)) {
                READ_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
                WRITE_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, regval);
            }

            if(IS_BROADSYNC1(intf->id)) {
                READ_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
                WRITE_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, regval);
            }

            if(IS_BROADSYNC0(intf->id)) {
                READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 0);
                WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);
            }

            if(IS_BROADSYNC1(intf->id)) {
                READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 0);
                WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);
            }

            READ_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr, &regval, ENABLEf, 0);
            WRITE_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
            if(IS_BROADSYNC0(intf->id)) {
                READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
                soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
                WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

                READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 1);
                WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);
            }

            if(IS_BROADSYNC1(intf->id)) {
                READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
                soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
                WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

                READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 1);
                WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);
            }

            /* configure BS0/BS1 Clock configuration registers */
            if(IS_BROADSYNC0(intf->id)) {
                READ_NS_BS0_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
                WRITE_NS_BS0_BS_CONFIGr(unit, regval);
            }

            if(IS_BROADSYNC1(intf->id)) {
                READ_NS_BS1_BS_CONFIGr(unit, &regval);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
                WRITE_NS_BS1_BS_CONFIGr(unit, regval);
            }

        } else if ( SOC_IS_TOMAHAWK3(unit)) {
             /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
            READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

            READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock configuration registers */
            READ_NS_BS0_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_NS_BS0_BS_CONFIGr(unit, regval);

            READ_NS_BS1_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_NS_BS1_BS_CONFIGr(unit, regval);

            /* 500 Mhz NCO - 2.000 ns */
            READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &regval, UINCf, 0x2000);
            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, regval);

            READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &regval, LINCf, freq_ctrl_lower);
            WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

            READ_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr, &regval, FCLf, 0);
            WRITE_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr(unit, regval);

            READ_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr, &regval, FCUf, 0);
            WRITE_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr(unit, regval);

            READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
            WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

            /* Setup BS0/BS1 HeartBeat Control registers */
            READ_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval);

            READ_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval);

            READ_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
            WRITE_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            READ_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
            WRITE_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            READ_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
            WRITE_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
            WRITE_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

            READ_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BROADSYNC_REF_CLK_GEN_CTRLr, &regval, ENABLEf, 0);
            WRITE_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
            READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

            READ_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_NS_BS1_BS_CLK_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock configuration registers */
            READ_NS_BS0_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_NS_BS0_BS_CONFIGr(unit, regval);

            READ_NS_BS1_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_NS_BS1_BS_CONFIGr(unit, regval);
      } else if (SOC_IS_FIRELIGHT(unit)) {
             /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
            READ_IPROC_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CLK_CTRLr, &regval,
                              LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CLK_CTRLr, &regval,
                              HIGH_DURATIONf, bitclk_high);
            WRITE_IPROC_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_IPROC_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CLK_CTRLr, &regval,
                              ENABLEf, 1);
            WRITE_IPROC_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_IPROC_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CLK_CTRLr, &regval,
                              LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CLK_CTRLr, &regval,
                              HIGH_DURATIONf, bitclk_high);
            WRITE_IPROC_NS_BS1_BS_CLK_CTRLr(unit, regval);

            READ_IPROC_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CLK_CTRLr, &regval,
                              ENABLEf, 1);
            WRITE_IPROC_NS_BS1_BS_CLK_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock configuration registers */
            READ_IPROC_NS_BS0_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval, MODEf,
                              master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                              BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                              BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                              BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_IPROC_NS_BS0_BS_CONFIGr(unit, regval);

            READ_IPROC_NS_BS1_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval, MODEf,
                              master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval,
                              BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval,
                              BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval,
                              BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_IPROC_NS_BS1_BS_CONFIGr(unit, regval);

            READ_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr,
                              &regval, LINCf, freq_ctrl_lower);
            WRITE_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, regval);

            READ_IPROC_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr(unit, &regval);
            soc_reg_field_set(unit,
                              IPROC_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr,
                              &regval, FCLf, 0);
            WRITE_IPROC_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_LOWERr(unit, regval);

            READ_IPROC_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr(unit, &regval);
            soc_reg_field_set(unit,
                              IPROC_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr,
                              &regval, FCUf, 0);
            WRITE_IPROC_NS_TIMESYNC_TS0_CURRENT_FREQ_CTRL_UPPERr(unit, regval);

            READ_IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr,
                              &regval, ENABLEf, 1);
            WRITE_IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

            /* Setup BS0/BS1 HeartBeat Control registers */
            READ_IPROC_NS_BS0_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_HEARTBEAT_CTRLr, &regval,
                              ENABLEf, 1);
            WRITE_IPROC_NS_BS0_BS_HEARTBEAT_CTRLr(unit, regval);

            READ_IPROC_NS_BS1_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_HEARTBEAT_CTRLr, &regval,
                              ENABLEf, 1);
            WRITE_IPROC_NS_BS1_BS_HEARTBEAT_CTRLr(unit, regval);

            READ_IPROC_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr,
                              &regval, DOWN_DURATIONf, hb_down);
            WRITE_IPROC_NS_BS0_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            READ_IPROC_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr,
                              &regval, DOWN_DURATIONf, hb_down);
            WRITE_IPROC_NS_BS1_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            READ_IPROC_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_HEARTBEAT_UP_DURATIONr,
                              &regval, UP_DURATIONf, hb_up);
            WRITE_IPROC_NS_BS0_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_IPROC_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_HEARTBEAT_UP_DURATIONr,
                              &regval, UP_DURATIONf, hb_up);
            WRITE_IPROC_NS_BS1_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_IPROC_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf,
                              0);
            WRITE_IPROC_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_IPROC_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf,
                              0);
            WRITE_IPROC_NS_BS1_BS_CLK_CTRLr(unit, regval);

            READ_IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr,
                              &regval, ENABLEf, 0);
            WRITE_IPROC_NS_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
            READ_IPROC_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CLK_CTRLr, &regval,
                              LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CLK_CTRLr, &regval,
                              HIGH_DURATIONf, bitclk_high);
            WRITE_IPROC_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_IPROC_NS_BS0_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CLK_CTRLr, &regval, ENABLEf,
                              1);
            WRITE_IPROC_NS_BS0_BS_CLK_CTRLr(unit, regval);

            READ_IPROC_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CLK_CTRLr, &regval,
                              LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CLK_CTRLr, &regval,
                              HIGH_DURATIONf, bitclk_high);
            WRITE_IPROC_NS_BS1_BS_CLK_CTRLr(unit, regval);

            READ_IPROC_NS_BS1_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CLK_CTRLr, &regval, ENABLEf,
                              1);
            WRITE_IPROC_NS_BS1_BS_CLK_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock configuration registers */
            READ_IPROC_NS_BS0_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval, MODEf,
                              master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                              BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                              BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, IPROC_NS_BS0_BS_CONFIGr, &regval,
                              BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_IPROC_NS_BS0_BS_CONFIGr(unit, regval);

            READ_IPROC_NS_BS1_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval, MODEf,
                              master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval,
                              BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval,
                              BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, IPROC_NS_BS1_BS_CONFIGr, &regval,
                              BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_IPROC_NS_BS1_BS_CONFIGr(unit, regval);
      } else {

            /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
            READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

            READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

            READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

            READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock configuration registers */
            READ_CMIC_BS0_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_CMIC_BS0_CONFIGr(unit, regval);

            READ_CMIC_BS1_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_CMIC_BS1_CONFIGr(unit, regval);

            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr, &regval, FRACf, freq_ctrl_lower);
            WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, regval);

            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr, &regval, NSf, 0);
            WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, regval);

            READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr, &regval, NSf, 0);
            WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, regval);

            READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
            WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

            /* Setup BS0/BS1 HeartBeat Control registers */
            READ_CMIC_BS0_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS0_HEARTBEAT_CTRLr(unit, regval);

            READ_CMIC_BS1_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS1_HEARTBEAT_CTRLr(unit, regval);

            READ_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
            WRITE_CMIC_BS0_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            READ_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
            WRITE_CMIC_BS1_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            READ_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
            WRITE_CMIC_BS0_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
            WRITE_CMIC_BS1_HEARTBEAT_UP_DURATIONr(unit, regval);

            READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

            READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

            READ_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BROADSYNC_REF_CLK_GEN_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BROADSYNC_REF_CLK_GEN_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock control registers (HIGH_DURATION / LOW_DURATION) */
            READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

            READ_CMIC_BS0_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS0_CLK_CTRLr(unit, regval);

            READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

            READ_CMIC_BS1_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS1_CLK_CTRLr(unit, regval);

            /* configure BS0/BS1 Clock configuration registers */
            READ_CMIC_BS0_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS0_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_CMIC_BS0_CONFIGr(unit, regval);

            READ_CMIC_BS1_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS1_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_CMIC_BS1_CONFIGr(unit, regval);
        }
    } else
#endif /* BCM_TIME_V3_OR_IPROC_MANAGED_BROADSYNC */
/*****************************************************************************/
/*        Non-KATANA2, HELIX4, HURRICANE2, GREYHOUND HW configuration        */
/*****************************************************************************/
    {
#if defined(BCM_KATANA_SUPPORT)
        if (SOC_IS_KATANA(unit)) {
            READ_TOP_BROAD_SYNC_PLL_CTRL_REGISTER_3r(unit, &regval);
            bs_ndiv_int = soc_reg_field_get(unit, TOP_BROAD_SYNC_PLL_CTRL_REGISTER_3r,
                                            regval, NDIV_INTf);
        }
#endif /* defined(BCM_KATANA_SUPPORT) */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit) && !SOC_IS_TD2P_TT2P(unit)) {
            READ_TOP_BS_PLL_CTRL_3r(unit, &regval);
            bs_ndiv_int = soc_reg_field_get(unit, TOP_BS_PLL_CTRL_3r,
                                            regval, NDIV_INTf);
            bs_ndiv_int = (regval & 0x3ff);
        } else if (SOC_IS_TD2P_TT2P(unit)) {
            READ_TOP_BS_PLL_CTRL_4r(unit, &regval);
            bs_ndiv_int = soc_reg_field_get(unit, TOP_BS_PLL_CTRL_4r,
                                            regval, NDIV_INTf);
            bs_ndiv_int = (regval & 0x3ff);
       }
#endif /* defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) */

        if (bs_ndiv_int == 300) {
            /* Take a BroadSync NDIV_INT value of 0 as a signal that the input
             * reference frequency is 12.8 MHz, so TSPLL will produce 248MHz,
             * not 250MHz
             */
            freq_ctrl_lower = 0x40842108;
        }

#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            /* m top_soft_reset_reg_2 top_bs_pll_rst_l=0 top_bs_pll_post_rst_l=0 */
            READ_TOP_SOFT_RESET_REG_2r(unit, &regval);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &regval, TOP_BS_PLL_RST_Lf, 0);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &regval, TOP_BS_PLL_POST_RST_Lf, 0);
            WRITE_TOP_SOFT_RESET_REG_2r(unit, regval);

            if (!SOC_IS_TD2P_TT2P(unit)) {
                /* m TOP_BS_PLL_CTRL_0 VCO_DIV2=1 */
                READ_TOP_BS_PLL_CTRL_0r(unit, &regval);
                soc_reg_field_set(unit, TOP_BS_PLL_CTRL_0r, &regval, VCO_DIV2f, 1);
                WRITE_TOP_BS_PLL_CTRL_0r(unit, regval);

                /* m TOP_BS_PLL_CTRL_2 PDIV=1 CH0_MDIV=175 */
                READ_TOP_BS_PLL_CTRL_2r(unit, &regval);
                soc_reg_field_set(unit, TOP_BS_PLL_CTRL_2r, &regval, PDIVf, 1);
                soc_reg_field_set(unit, TOP_BS_PLL_CTRL_2r, &regval, CH0_MDIVf, 175);
                WRITE_TOP_BS_PLL_CTRL_2r(unit, regval);
            } else {

                READ_TOP_BS_PLL_CTRL_0r(unit, &regval);
                soc_reg_field_set(unit, TOP_BS_PLL_CTRL_0r, &regval, VCO_FB_DIV2f, 1);
                WRITE_TOP_BS_PLL_CTRL_0r(unit, regval);

                /* TD2+: Keeping default for VCO_DIV2(=1), NDIV(=140.0), PDIV(=1) */
                READ_TOP_BS_PLL_CTRL_3r(unit, &regval);
                soc_reg_field_set(unit, TOP_BS_PLL_CTRL_3r, &regval, PDIVf, 1);
                soc_reg_field_set(unit, TOP_BS_PLL_CTRL_3r, &regval, CH0_MDIVf, 175);
                WRITE_TOP_BS_PLL_CTRL_3r(unit, regval);
            }

            /* m top_soft_reset_reg_2 top_bs_pll_rst_l=1 */
            READ_TOP_SOFT_RESET_REG_2r(unit, &regval);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &regval, TOP_BS_PLL_RST_Lf, 1);
            WRITE_TOP_SOFT_RESET_REG_2r(unit, regval);

            /* m top_soft_reset_reg_2 top_bs_pll_post_rst_l=1 */
            READ_TOP_SOFT_RESET_REG_2r(unit, &regval);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &regval, TOP_BS_PLL_POST_RST_Lf, 1);
            WRITE_TOP_SOFT_RESET_REG_2r(unit, regval);
        }
#endif /* defined(BCM_TRIDENT2_SUPPORT) */

        if (SOC_IS_KATANA(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit)) {

            /* Validate calculated output based on bitclock_hz */
            if(SOC_FAILURE(soc_reg_field_validate(unit, CMIC_BS_CLK_CTRLr,
                                                       LOW_DURATIONf, bitclk_low))) {
               return BCM_E_PARAM;
            }
            if(SOC_FAILURE(soc_reg_field_validate(unit, CMIC_BS_CLK_CTRLr,
                                                    HIGH_DURATIONf, bitclk_high))) {
               return BCM_E_PARAM;
            }

            /* m CMIC_BS_CLK_CTRL LOW_DURATION=1 HIGH_DURATION=1 */
            READ_CMIC_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

            /* m CMIC_BS_CLK_CTRL ENABLE=1 */
            READ_CMIC_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

            /* m CMIC_BS_CONFIG BS_CLK_OUTPUT_ENABLE=1 BS_HB_OUTPUT_ENABLE=1 BS_TC_OUTPUT_ENABLE=1 */
            READ_CMIC_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_TC_OUTPUT_ENABLEf, output_enable);
            WRITE_CMIC_BS_CONFIGr(unit, regval);

            READ_CMIC_TS_FREQ_CTRL_LOWERr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_LOWERr, &regval, FRACf, freq_ctrl_lower);
            WRITE_CMIC_TS_FREQ_CTRL_LOWERr(unit, regval);

            /* m CMIC_TS_FREQ_CTRL_UPPER ENABLE=1 */
            READ_CMIC_TS_FREQ_CTRL_UPPERr(unit, &regval);
            soc_reg_field_set(unit, CMIC_TS_FREQ_CTRL_UPPERr, &regval, ENABLEf, 1);
            WRITE_CMIC_TS_FREQ_CTRL_UPPERr(unit, regval);

            /* m CMIC_BS_HEARTBEAT_CTRL ENABLE=1 */
            READ_CMIC_BS_HEARTBEAT_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1);
            WRITE_CMIC_BS_HEARTBEAT_CTRLr(unit, regval);

            /* m CMIC_BS_HEARTBEAT_DOWN_DURATION DOWN_DURATION=2400 */
            /* Validate calculated hb_down based on heartbeat_hz */
            if(SOC_FAILURE(soc_reg_field_validate(unit,
                                                  CMIC_BS_HEARTBEAT_DOWN_DURATIONr,
                                                          DOWN_DURATIONf, hb_down))) {
               return BCM_E_PARAM;
            }
            READ_CMIC_BS_HEARTBEAT_DOWN_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_DOWN_DURATIONr, &regval, DOWN_DURATIONf, hb_down);
            WRITE_CMIC_BS_HEARTBEAT_DOWN_DURATIONr(unit, regval);

            /* m CMIC_BS_HEARTBEAT_UP_DURATION UP_DURATION=100 */
            READ_CMIC_BS_HEARTBEAT_UP_DURATIONr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_UP_DURATIONr, &regval, UP_DURATIONf, hb_up);
            WRITE_CMIC_BS_HEARTBEAT_UP_DURATIONr(unit, regval);

            /* m CMIC_BS_CLK_CTRL ENABLE=0 */
            READ_CMIC_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

            /* m CMIC_BS_REF_CLK_GEN_CTRL ENABLE=0 */
            /* BS_REF_CLK from BS_PLL_CLK rather than BS_REF_CLK_generated */

            READ_CMIC_BS_REF_CLK_GEN_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_REF_CLK_GEN_CTRLr, &regval, ENABLEf, 0);
            WRITE_CMIC_BS_REF_CLK_GEN_CTRLr(unit, regval);

            /* m CMIC_BS_CLK_CTRL ENABLE=1 LOW_DURATION=1 HIGH_DURATION=1 */
            READ_CMIC_BS_CLK_CTRLr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, ENABLEf, 1);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, LOW_DURATIONf, bitclk_low);
            soc_reg_field_set(unit, CMIC_BS_CLK_CTRLr, &regval, HIGH_DURATIONf, bitclk_high);
            WRITE_CMIC_BS_CLK_CTRLr(unit, regval);

            /* m CMIC_BS_CONFIG MODE=1 BS_CLK_OUTPUT_ENABLE=1 BS_HB_OUTPUT_ENABLE=1 */
            READ_CMIC_BS_CONFIGr(unit, &regval);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, master ? TIME_MODE_OUTPUT : TIME_MODE_INPUT);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_CLK_OUTPUT_ENABLEf, output_enable);
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, BS_HB_OUTPUT_ENABLEf, output_enable);
            WRITE_CMIC_BS_CONFIGr(unit, regval);
        }
    }

    return BCM_E_NONE;
}
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) ||
          defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HELIX4_SUPPORT) ||
          defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) ||
          defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT) ||
          defiend(BCM_SABER2_SUPPORT) */
#if defined(BCM_HELIX4_SUPPORT)
#define BCM_TIME_HX4_L1_FREQ_SEL_MODE(x) (((x) >= 0) && ((x) <= 3))
#define BCM_TIME_HX4_PORT_VALID(x) ((x) >= 1 && (x) <= 66 )
#define BCM_TIME_HX4_CLK_SRC_VALID(x) \
    (((x) == bcmTimeSynceClockSourcePrimary) || \
     ((x) == bcmTimeSynceClockSourceSecondary) )
/* physical port number based classification */
#define BCM_TIME_HX4_PHYPORT_UNICORE(x) ((x) >= 1 && (x) <= 52 )
#define BCM_TIME_HX4_PHYPORT_WARPCORE(x) ((x) >= 53 && (x) <= 64 )

/*
 * Function:
 *      _bcm_esw_time_hx4_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT)
 *      index     - (IN) if input_src is PORT, specify  port number 1..48(physical port) or 65..66(LCPLL ports)
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_hx4_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int port_sel;

    if (!BCM_TIME_HX4_PORT_VALID(phy_port) ||
        !BCM_TIME_HX4_CLK_SRC_VALID(clk_src) ||
        (phy_port == -1)) {
        return BCM_E_PARAM;
    }

    port_sel = phy_port - 1;
    if ( clk_src == bcmTimeSynceClockSourcePrimary ) {
        /* TOP_MISC_CONTROL_1 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 0));
        /* TOP_MISC_CONTROL_1 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_FREQ_SELf, divisor->stage0_mode));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, PRI_PORT_SELf, port_sel));

        /* Enable L1 prim recov clk as valid,default it is invalid  */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL_1 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 1));
    } else { /* Secondary/Backup SyncE source */
        /* TOP_MISC_CONTROL_1 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 0));
        /* TOP_MISC_CONTROL_1 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_BKUP_FREQ_SELf, divisor->stage0_mode));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, BKUP_PORT_SELf, port_sel));

        /* Enable L1 sec/bkp recov clk as valid,default it is invalid  */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_BKUP_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL_1 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 1));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_hx4_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_hx4_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, phy_port, rval;

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_EGR_L1_CLK_RECOVERY_CTRLr(unit, &rval));
        val = soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr, rval,
                                     PRI_PORT_SELf);
        phy_port = val+ 1;
        divisor->input_src = bcmTimeSynceInputSourceTypePort;
        divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
        /* Freq div mode */
        divisor->stage0_mode = soc_reg_field_get(unit, TOP_MISC_CONTROL_1r,
                                                 rval, L1_RCVD_FREQ_SELf);
        break;
    case bcmTimeSynceClockSourceSecondary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_EGR_L1_CLK_RECOVERY_CTRLr(unit, &rval));
        val= soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr, rval,
                                     BKUP_PORT_SELf);
        phy_port = val+ 1;
        divisor->input_src = bcmTimeSynceInputSourceTypePort;
        divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
        /* Freq div mode */
        divisor->stage0_mode = soc_reg_field_get(unit, TOP_MISC_CONTROL_1r,
                                                 rval, L1_RCVD_BKUP_FREQ_SELf);
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#endif /* defined(BCM_HELIX4_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
#define BCM_TIME_GH_L1_FREQ_SEL_MODE(x) (((x) >= 0) && ((x) <= 3))
#define BCM_TIME_GH_PORT_VALID(x) (((x) >= 1 && (x) <= 48 ) || ((x)>=65 && (x) <= 66))
#define BCM_TIME_GH_CLK_SRC_VALID(x) \
    (((x) == bcmTimeSynceClockSourcePrimary) || \
     ((x) == bcmTimeSynceClockSourceSecondary) )

/*
 * Function:
 *      _bcm_esw_time_gh_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT)
 *      index     - (IN) if input_src is PORT, specify  port number 1..48(physical port) or 65..66(LCPLL ports)
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_gh_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int phy_port = divisor->index;
    int port_sel;

    if (!BCM_TIME_GH_PORT_VALID(phy_port) ||
        !BCM_TIME_GH_CLK_SRC_VALID(clk_src) ||
        (phy_port == -1)) {
        return BCM_E_PARAM;
    }

    if (!SOC_PORT_VALID(unit,phy_port))
    {
        return BCM_E_PARAM;
    }

    port_sel = phy_port - 1;
    if ( clk_src == bcmTimeSynceClockSourcePrimary ) {
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_FREQ_SELf, divisor->stage0_mode));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r,
                                    REG_PORT_ANY, L1_RCVD_PRI_CLK_PORT_SELf, port_sel));

        /* Enable L1 prim recov clk as valid,default it is invalid  */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 1));
    } else { /* Secondary/Backup SyncE source */
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_BKUP_FREQ_SELf, divisor->stage0_mode));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r,
                                    REG_PORT_ANY, L1_RCVD_BKUP_CLK_PORT_SELf, port_sel));

        /* Enable L1 sec/bkp recov clk as valid,default it is invalid  */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_BKUP_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 1));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_gh_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_gh_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, phy_port, rval;

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_2r(unit, &rval));
        val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                    L1_RCVD_PRI_CLK_PORT_SELf);
        phy_port = val+ 1;
        divisor->input_src = bcmTimeSynceInputSourceTypePort;
        divisor->index = phy_port;

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));
        /* Freq div mode */
        divisor->stage0_mode = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r,
                                                 rval, L1_RCVD_FREQ_SELf);
        break;
    case bcmTimeSynceClockSourceSecondary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_2r(unit, &rval));
        val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                    L1_RCVD_BKUP_CLK_PORT_SELf);
        phy_port = val+ 1;
        divisor->input_src = bcmTimeSynceInputSourceTypePort;
        divisor->index = phy_port;

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));
        /* Freq div mode */
        divisor->stage0_mode = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r,
                                                 rval, L1_RCVD_BKUP_FREQ_SELf);
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#endif /* defined(BCM_GREYHOUND_SUPPORT) */

#if defined(BCM_KATANA_SUPPORT)
/*
 * Function:
 *      _bcm_esw_time_katana_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT)
 *      index     - (IN) port no
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_katana_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int phy_port = divisor->index;
    int port_sel;
    uint32 rval;

    port_sel = phy_port - 1;
    if ( clk_src == bcmTimeSynceClockSourcePrimary ) {
      BCM_IF_ERROR_RETURN(
           READ_TOP_MISC_CONTROLr(unit, &rval));
      soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                        L1_CLK0_RECOVERY_MUX_SELf, port_sel);
      BCM_IF_ERROR_RETURN(
            WRITE_TOP_MISC_CONTROLr(unit, rval));
    } else { /* Secondary/Backup SyncE source */
      BCM_IF_ERROR_RETURN(
           READ_TOP_MISC_CONTROLr(unit, &rval));
       soc_reg_field_set(unit, TOP_MISC_CONTROLr, &rval,
                         L1_CLK1_RECOVERY_MUX_SELf, port_sel);
       BCM_IF_ERROR_RETURN(
            WRITE_TOP_MISC_CONTROLr(unit, rval));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_katana_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_katana_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, phy_port, rval;

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        BCM_IF_ERROR_RETURN( READ_TOP_MISC_CONTROLr(unit, &rval));
        val = soc_reg_field_get(unit, TOP_MISC_CONTROLr, rval,
                                  L1_CLK0_RECOVERY_MUX_SELf);
        break;
    case bcmTimeSynceClockSourceSecondary:
        BCM_IF_ERROR_RETURN( READ_TOP_MISC_CONTROLr(unit, &rval));
        val = soc_reg_field_get(unit, TOP_MISC_CONTROLr, rval,
                                     L1_CLK1_RECOVERY_MUX_SELf);
        break;
    default:
        return BCM_E_PARAM;
    }

    phy_port = val+ 1;
    divisor->input_src = bcmTimeSynceInputSourceTypePort;
    divisor->index = phy_port;

    return BCM_E_NONE;
}
#endif /* BCM_KATANA_SUPPORT */

#define BCM_TIME_SYNCE_CLK_SRC_VALID(x) \
    (((x) == bcmTimeSynceClockSourcePrimary) || \
     ((x) == bcmTimeSynceClockSourceSecondary) )

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)

/* Synchronous Ethernet(SyncE) */
/* static definition and functions */

#define BCM_TIME_TD2P_PLL_VALID(x) (((x) >= 0) && ((x) <= 3))
#define BCM_TIME_TOMAHAWK_PLL_VALID(x) (((x) >= 0) && ((x) <= 3))


#define BCM_TIME_TD2P_PORT_VALID(x) ((x) >= 1 && (x) <= 105 )
#define BCM_TIME_TOMAHAWK_PORT_VALID(x) ((x) >= 1 && (x) <= 132 )

STATIC int
_bcm_time_td2p_divctrl_reg_modify_primary(int unit,
                            bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 rval;
    SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CTRLr, &rval,
                      STAGE0_MODEf, divisor->stage0_mode);
    /* STAGE1_MODEf "DON'T CARE" in SDM mode. */
    if (divisor->stage0_mode != bcmTimeSynceModeSDMFracDiv) {
        soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CTRLr, &rval,
                          STAGE1_MODEf, divisor->stage1_div);
    }
    if ((divisor->stage0_mode == bcmTimeSynceModeBypass) ||
        (divisor->stage0_mode == bcmTimeSynceModeSDMFracDiv)) {
        uint16 sdm_val;
        sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                   (divisor->stage0_sdm_frac & 0xff));
        soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CTRLr, &rval,
                          SDM_DIVISORf, sdm_val);
    }
    WRITE_TOP_L1_RCVD_CLK_CTRLr(unit, rval);
    return BCM_E_NONE;
}

STATIC int
_bcm_time_td2p_divctrl_reg_modify_secondary(int unit,
                            bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 rval;
    SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_BKUP_CTRLr(unit, &rval));
    soc_reg_field_set(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr, &rval,
                      STAGE0_MODEf, divisor->stage0_mode);
    /* STAGE1_MODEf "DON'T CARE" in SDM mode. */
    if (divisor->stage0_mode != bcmTimeSynceModeSDMFracDiv) {
        soc_reg_field_set(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr, &rval,
                          STAGE1_MODEf, divisor->stage1_div);
    }
    if ((divisor->stage0_mode == bcmTimeSynceModeBypass) ||
        (divisor->stage0_mode == bcmTimeSynceModeSDMFracDiv)) {
        uint16 sdm_val;
        sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                   (divisor->stage0_sdm_frac & 0xff));
        soc_reg_field_set(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr, &rval,
                          SDM_DIVISORf, sdm_val);
    }
    WRITE_TOP_L1_RCVD_CLK_BKUP_CTRLr(unit, rval);
    return BCM_E_NONE;
}

#ifdef PORTMOD_SUPPORT
STATIC phymod_dispatch_type_t
_bcm_time_synce_tsc_phymod_dispatch_type_get(int unit, int port)
{
    phymod_dispatch_type_t rv = phymodDispatchTypeInvalid;
    int    nof_cores = 0;

    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        phymod_core_access_t core_acc;
        phymod_core_access_t_init(&core_acc);
        portmod_port_main_core_access_get(unit, port, 0, &core_acc, &nof_cores);
        if (nof_cores <= 0) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                      (BSL_META_U(unit,
                                  "Cannot get phy address\n")));
            return BCM_E_PARAM;
        }

        rv= core_acc.type;

    } else
    {
        return phymodDispatchTypeInvalid;
    }

    return rv;
}
#endif


STATIC int
_bcm_esw_time_synce_phy_port_lane_adjust(int unit,
                                         int phy_port,
                                         int *adjusted_lane,
                                         int *adjusted_phy_port)
{
    int first_phy_port=1, first_log_port=0, port, lane = 0;
    uint32 flags = 0;
    int num_lanes = 4;

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
#ifdef PHYMOD_TSCBH_SUPPORT
        phymod_dispatch_type_t dispatch_type = phymodDispatchTypeInvalid;
        dispatch_type =  _bcm_time_synce_tsc_phymod_dispatch_type_get(unit, port);
        if (dispatch_type == phymodDispatchTypeTscbh)
        {
            num_lanes = 8;
        }
#endif /* PHYMOD_TSCBH_SUPPORT */
    }
#endif /* PORTMOD_SUPPORT */

#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5(unit)) {
        first_phy_port = SOC_HX5_PORT_BLOCK_BASE_PORT(port);
    } else
#endif /* defined(BCM_HELIX5_SUPPORT) */
#if defined(BCM_HURRICANE4_SUPPORT)
    if (SOC_IS_HURRICANE4(unit)) {
        first_phy_port = SOC_HR4_PORT_BLOCK_BASE_PORT(port);
    } else
#endif /* defined(BCM_HURRICANE4_SUPPORT) */
#if defined(BCM_FIREBOLT6_SUPPORT)
    if (SOC_IS_FIREBOLT6(unit)) {
        first_phy_port = SOC_FB6_PORT_BLOCK_BASE_PORT(port);
    } else
#endif /* defined(BCM_FIREBOLT6_SUPPORT) */
#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        first_phy_port = SOC_FL_PORT_BLOCK_BASE_PORT(port);
    } else
#endif /* defined(BCM_FIRELIGHT_SUPPORT) */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)){
        first_phy_port = gh2_tsc_phy_port[((phy_port - 58) / 4)]; /* Eagle and Falcon*/
    } else
#endif /* defined(BCM_GREYHOUND2_SUPPORT) */
#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)){
        if (phy_port >= 34 && phy_port < 38) {
            first_phy_port = 34; /* Eagle */
        } else if (phy_port >= 38 && phy_port <= 41) {
            first_phy_port = 38; /* Eagle */
        } else if (phy_port >=2 && phy_port <= 5) {
            first_phy_port = 2; /* QTCE */
        } else if (phy_port >=18 && phy_port <= 21) {
            first_phy_port = 18; /* QTCE */
        } else {
            return BCM_E_PARAM;
        }
    } else
#endif /* defined(BCM_HURRICANE3_SUPPORT) */
    {
        first_phy_port = (SOC_INFO(unit).port_serdes[port] * num_lanes) + 1;
    }

#if defined (BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            /* If the first phy port is disabled, the phy port to
             * logical port map will return -1. In MO, we need NOT
             * pass first logical port to get the lane map. Any logical
             * port should work */
            first_log_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
        } else {
            first_log_port = SOC_INFO(unit).port_p2l_mapping[first_phy_port];
        }
#else
    first_log_port = SOC_INFO(unit).port_p2l_mapping[first_phy_port];
#endif
#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, first_log_port)) {
        phymod_lane_map_t lane_map;
        if (!SOC_PORT_VALID(unit,first_log_port))
        {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(portmod_port_lane_map_get(unit, first_log_port, flags, &lane_map));
        lane = lane_map.lane_map_rx[(phy_port - 1) % num_lanes];
    } else
#endif
    {
        uint32_t lane_swap = 0;
        BCM_IF_ERROR_RETURN(bcm_esw_port_phy_control_get(unit, first_log_port,
                                BCM_PORT_PHY_CONTROL_LANE_SWAP, &lane_swap));
        if (SOC_IS_GREYHOUND2(unit) || SOC_IS_HURRICANE3(unit)){
            lane = (lane_swap >> (((phy_port - 2) % num_lanes) * num_lanes)) & 0xf;
        } else {
            lane = (lane_swap >> (((phy_port - 1) % num_lanes) * num_lanes)) & 0xf;
        }
    }

    phy_port = first_phy_port + lane;

    if (adjusted_lane) {
        *adjusted_lane = lane;
    }
    if (adjusted_phy_port) {
        *adjusted_phy_port = phy_port;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_esw_time_synce_phy_port_get(int unit,
                                 int adjusted_phy_port,
                                 uint32 *phy_port)
{
    int first_phy_port=1, first_log_port=0, lane_indx = 0;
    int adjusted_lane = 0;
    int tmp_phy_port = adjusted_phy_port;
    int num_lanes = 4;
    uint32 flags = 0;

#ifdef PORTMOD_SUPPORT
    {
        
        if (SOC_IS_TOMAHAWK3(unit))
            num_lanes = 8;
    }
#endif

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) { /* Eagle and Falcon */
        first_phy_port = gh2_tsc_phy_port[((adjusted_phy_port - 58) / 4)];
    } else
#endif /*defined(BCM_GREYHOUND2_SUPPORT)*/
#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)) {
        if (adjusted_phy_port >= 34 && adjusted_phy_port < 38) {
            first_phy_port = 34; /* Eagle */
        } else if (adjusted_phy_port >= 38 && adjusted_phy_port <= 41) {
            first_phy_port = 38; /* Eagle */
        } else if (adjusted_phy_port >=2 && adjusted_phy_port <= 5) {
            first_phy_port = 2; /* QTCE */
        } else if (adjusted_phy_port >=18 && adjusted_phy_port <= 21) {
            first_phy_port = 18; /* QTCE */
        } else {
            return BCM_E_PARAM;
        }
    } else
#endif /* defined(BCM_HURRICANE3_SUPPORT) */
    {
        first_phy_port = (((adjusted_phy_port - 1)/num_lanes) * num_lanes) + 1;
    }
    adjusted_lane = adjusted_phy_port - first_phy_port;
    first_log_port = SOC_INFO(unit).port_p2l_mapping[first_phy_port];
#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, first_log_port)) {
        phymod_lane_map_t lane_map;
        if (!SOC_PORT_VALID(unit,first_log_port))
        {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(portmod_port_lane_map_get(unit, first_log_port, flags, &lane_map));
        for (lane_indx = 0; lane_indx < num_lanes; lane_indx++) {
            if (lane_map.lane_map_rx[lane_indx] == adjusted_lane) {
                break;
            }
        }
    } else
#endif
    {
        uint32_t lane_swap = 0; num_lanes = 4;
        BCM_IF_ERROR_RETURN(bcm_esw_port_phy_control_get(unit, first_log_port,
                                BCM_PORT_PHY_CONTROL_LANE_SWAP, &lane_swap));
        for (lane_indx = 0; lane_indx < num_lanes; lane_indx++) {
            if ( ((lane_swap >> (lane_indx * num_lanes)) & 0xf) == adjusted_lane) {
                break;
            }
        }
    }
    tmp_phy_port = first_phy_port + lane_indx;
    if (phy_port) {
        *phy_port = (uint32)tmp_phy_port;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _tsc_synce_phy_register_set
 * Purpose:
 *      SyncE mode register(0x9002:PCS register) set.
 *      This function is equivalent with
 *
 *        phy raw sbus <phy_address> 0.<phy_lane> 0x9002 value
 *
 *      phy_address, phy_lane will be calculated based on input logical port.
 * Parameters:
 *      port  - (IN) logical port number
 *      value - (IN) the value to be written to register.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
/* TSC eagle phy registers address */
#define BCM_TIME_TSCE_MAIN0_SYNCE_CTL_STAGE0_REG        0x9002 /* TSC eagle synce mode register stage 0 */

/* QTC phy registers address */
#define BCM_TIME_QTCE_MAIN0_SYNCE_CTL_STAGE0_REG        0x9008 /* QTC synce mode register stage 0 */

/* TSC falcon phy registers address */
#define BCM_TIME_TSCF_MAIN0_SYNCE_CTL_REG               0x9001 /* TSC falcon synce mode register stage 1 */
#define BCM_TIME_TSCF_MAIN0_SYNCE_CTL_STAGE0_REG        0x9002 /* TSC falcon synce mode register stage 0 */
#define BCM_TIME_TSCF_RX_X4_SYNCE_FRACTIONAL_DIV_REG    0xc13d /* TSC falcon synce fractional divisor register */

STATIC int
_bcm_time_synce_phy_register_set(int unit, int port, int value, uint32 phy_reg)
{
    uint32 phy_devad = 0;    /* devad = 0 means PCS access */
    uint32 phy_aer;
    uint16 phy_addr;
    int    phy_lane;
    int    phy_port;
    int    nof_cores = 0;
    uint32 val;
    /* get internal phy address */
#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        phymod_core_access_t core_acc;
        phymod_core_access_t_init(&core_acc);
        portmod_port_main_core_access_get(unit, port, 0, &core_acc, &nof_cores);
        if (nof_cores <= 0) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                      (BSL_META_U(unit,
                                  "Cannot get phy address\n")));
            return BCM_E_PARAM;
        }
        phy_addr = core_acc.access.addr;
    } else
#endif
    {
        phy_addr = PORT_TO_PHY_ADDR(unit, port);
        if (phy_addr ==  0xff) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                      (BSL_META_U(unit,
                                  "Cannot get phy address\n")));
            return BCM_E_PARAM;
        }
    }

    /* calculate lane index based on logical port */
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
    &phy_lane, NULL));
    phy_aer = (phy_devad << 11) | phy_lane;

    if (phy_reg != BCM_TIME_TSCF_RX_X4_SYNCE_FRACTIONAL_DIV_REG) {
        soc_sbus_mdio_read(unit, phy_addr, phy_reg, &val);
        value = (val & ~(0x3 << (phy_lane *2))) | ((value & 0x3) << phy_lane * 2);
    } else {
        phy_reg = (phy_aer << 16) | phy_reg;
    }

    SOC_IF_ERROR_RETURN(
        soc_sbus_mdio_write(unit, phy_addr, phy_reg, value));

    LOG_VERBOSE(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                "logical  port (%d)\n"
                "physical port (%d)\n"
                "phy addr (%x) lane (%d)\n")
                , port, phy_port
                , phy_addr, phy_lane));
    return BCM_E_NONE;
}

STATIC int
_bcm_time_synce_phy_register_get(int unit, int port, uint32 phy_reg, uint32 *value)
{
    uint32 phy_devad = 0;    /* devad = 0 means PCS access */
    uint32 phy_aer;
    uint16 phy_addr;
    int    phy_lane;
    int    phy_port;
    int    nof_cores = 0;

    /* get internal phy address */
#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        phymod_core_access_t core_acc;
        phymod_core_access_t_init(&core_acc);
        portmod_port_main_core_access_get(unit, port, 0, &core_acc, &nof_cores);
        if (nof_cores <= 0) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                      (BSL_META_U(unit,
                                  "Cannot get phy address\n")));
            return BCM_E_PARAM;
        }
        phy_addr = core_acc.access.addr;
    } else
#endif
    {
        phy_addr = PORT_TO_PHY_ADDR(unit, port);
        if (phy_addr ==  0xff) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                      (BSL_META_U(unit,
                                  "Cannot get phy address\n")));
            return BCM_E_PARAM;
        }
    }

    /* calculate lane index based on logical port */
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
    &phy_lane, NULL));

    phy_aer = (phy_devad << 11) | phy_lane;
    if (phy_reg == BCM_TIME_TSCF_RX_X4_SYNCE_FRACTIONAL_DIV_REG) {
        phy_reg = (phy_aer << 16) | phy_reg;
    }


    SOC_IF_ERROR_RETURN(
        soc_sbus_mdio_read(unit, phy_addr, phy_reg, value));

    if (phy_reg == BCM_TIME_TSCF_MAIN0_SYNCE_CTL_REG || phy_reg == BCM_TIME_TSCF_MAIN0_SYNCE_CTL_STAGE0_REG) {
        soc_sbus_mdio_read(unit, phy_addr, phy_reg, value);
        *value = (*value >> phy_lane *2) & 0x3;
    }

    LOG_VERBOSE(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                "logical  port (%d)\n"
                "physical port (%d)\n"
                "phy addr (%x) lane (%d)\n")
                , port, phy_port
                , phy_addr, phy_lane));
    return BCM_E_NONE;
}



STATIC int
_bcm_time_synce_tsc_divisor_turn_off(int unit, int port)
{
    int rv = BCM_E_NONE;
    int    nof_cores = 0;

#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        phymod_core_access_t core_acc;
        phymod_core_access_t_init(&core_acc);
        portmod_port_main_core_access_get(unit, port, 0, &core_acc, &nof_cores);
        if (nof_cores <= 0) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                      (BSL_META_U(unit,
                                  "Cannot get phy address\n")));
            return BCM_E_PARAM;
        }
#ifdef PHYMOD_TSCE_SUPPORT
        if (core_acc.type == phymodDispatchTypeTsce) {
            rv = _bcm_time_synce_phy_register_set(unit, port, 0x0, BCM_TIME_TSCE_MAIN0_SYNCE_CTL_STAGE0_REG);
        }
#endif /* PHYMOD_TSCE_SUPPORT */

    } else
#endif /* PORTMOD_SUPPORT */
    {
        return BCM_E_PARAM;
    }

    return rv;
}


/* NOTES : this function takes care of TD2Plus only */
STATIC int
_bcm_esw_time_td2p_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int port_sel;

    if (!BCM_TIME_TD2P_PORT_VALID(lport) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        (phy_port == -1)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
    NULL, &phy_port));


    port_sel = phy_port - 1;
    if ( clk_src == bcmTimeSynceClockSourcePrimary ) {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_FREQ_DIV_SELf, 0));
        /* _TOP_L1_RCVD_CLK_CTRL */
        SOC_IF_ERROR_RETURN(_bcm_time_td2p_divctrl_reg_modify_primary(unit, divisor));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    XG_RCVD_PRI_SELf, 0));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, PRI_PORT_SELf, port_sel));
        /* Turn off TSC div11 */
        BCM_IF_ERROR_RETURN(
            _bcm_time_synce_tsc_divisor_turn_off(unit, lport));

        /* Enable SW L1 prim recov clk as valid,default it is invalid*/
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 1));
    } else {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_BKUP_FREQ_DIV_SELf, 0));
        /* _TOP_L1_RCVD_CLK_BKUP_CTRL */
        SOC_IF_ERROR_RETURN
            (_bcm_time_td2p_divctrl_reg_modify_secondary(unit, divisor));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    XG_RCVD_BKUP_SELf, 0));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, BKUP_PORT_SELf, port_sel));
        /* Turn off TSC div11 */
        BCM_IF_ERROR_RETURN(
            _bcm_time_synce_tsc_divisor_turn_off(unit, lport));

        /* Enable SW L1 sec recov clk as valid,default it is invalid*/
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_BKUP_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 1));
    }
    return BCM_E_NONE;
}

/* NOTES : this function takes care of TD2Plus only */
STATIC int
_bcm_esw_time_td2p_synce_clock_set_by_pll(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 pll_index = divisor->index;

    /* coverity[unsigned_compare] */
    if (!BCM_TIME_TD2P_PLL_VALID(pll_index) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src)) {
        return BCM_E_PARAM;
    }
    if (clk_src == bcmTimeSynceClockSourcePrimary) {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_FREQ_DIV_SELf, 0));
        /* _TOP_L1_RCVD_CLK_CTRL */
        SOC_IF_ERROR_RETURN(_bcm_time_td2p_divctrl_reg_modify_primary(unit, divisor));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    XG_RCVD_PRI_SELf, pll_index+1));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, PRI_PORT_SELf, 0));

        /* Enable SW L1 prim recov clk as valid,default it is invalid*/
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 1));
    } else {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_BKUP_FREQ_DIV_SELf, 0));
        /* _TOP_L1_RCVD_CLK_BKUP_CTRL */
        SOC_IF_ERROR_RETURN
            (_bcm_time_td2p_divctrl_reg_modify_secondary(unit, divisor));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    XG_RCVD_BKUP_SELf, pll_index+1));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, BKUP_PORT_SELf, 0));

        /* Enable SW L1 sec recov clk as valid,default it is invalid*/
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_BKUP_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 1));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_td2p_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT, PLL)
 *      index     - (IN) if input_src is PORT, specify logical port number 0..105
 *                       if input_src is PLL, specify PLL index 0..3
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes: This function only configures for td2plus
 */
STATIC int
_bcm_esw_time_td2p_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;

    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_td2p_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    case bcmTimeSynceInputSourceTypePLL:
        rv = _bcm_esw_time_td2p_synce_clock_set_by_pll(unit, clk_src, divisor);
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_time_td2p_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes: This function takes care of TD2Plus only
 */
STATIC int
_bcm_esw_time_td2p_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, val2, rval;
    uint16 sdm_val;

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_2r(unit, &rval));
        val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                XG_RCVD_PRI_SELf);
        SOC_IF_ERROR_RETURN(
             READ_EGR_L1_CLK_RECOVERY_CTRLr(unit, &rval));
        val2 = soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr, rval,
                                     PRI_PORT_SELf);
        if (val == 0) { /* source is Port */
            uint32 phy_port;
            phy_port = val2 + 1;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
        } else { /* source is PLL */
            /* EGR_L1_CLK_RECOVERY_CTRLr.PRI_PORT_SELf needs to be 0
             * Otherwise, there's no clock output */
            if (val2 != 0) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = val - 1;
        }

        SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_CTRLr(unit, &rval));
        /* stage0_mode */
        divisor->stage0_mode = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CTRLr,
                                                 rval, STAGE0_MODEf);
        sdm_val = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CTRLr, rval,
                                    SDM_DIVISORf);
        /* stage0_whole */
        divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
        /* stage0_frac */
        divisor->stage0_sdm_frac = sdm_val & 0xff ;
        /* stage1_div  */
        divisor->stage1_div = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CTRLr,
                                                rval, STAGE1_MODEf);
        break;
    case bcmTimeSynceClockSourceSecondary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_2r(unit, &rval));
        val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                XG_RCVD_BKUP_SELf);
        SOC_IF_ERROR_RETURN(
             READ_EGR_L1_CLK_RECOVERY_CTRLr(unit, &rval));
        val2 = soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr, rval,
                                     BKUP_PORT_SELf);
        if (val == 0) { /* source is Port */
            uint32 phy_port;
            phy_port = val2 + 1;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
        } else { /* source is PLL */
            /* EGR_L1_CLK_RECOVERY_CTRLr.BKUP_PORT_SELf needs to be 0
             * Otherwise, there's no clock output */
            if (val2 != 0) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = val - 1;
        }

        SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_BKUP_CTRLr(unit, &rval));
        /* stage0_mode */
        divisor->stage0_mode = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr,
                                                 rval, STAGE0_MODEf);
        sdm_val = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr, rval,
                                    SDM_DIVISORf);
        /* stage0_whole */
        divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
        /* stage0_frac */
        divisor->stage0_sdm_frac = sdm_val & 0xff ;
        /* stage1_div  */
        divisor->stage1_div = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr,
                                                rval, STAGE1_MODEf);
        break;
    default:
        return BCM_E_PARAM;
    }



    return BCM_E_NONE;
}

STATIC int
_bcm_esw_time_tomahawkx_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    soc_reg_t synce_ctrl = TOP_MISC_CONTROLr;
    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int port_sel;
    uint16 sdm_val;
    portmod_port_synce_clk_ctrl_t config;
    soc_field_t xg_rcvd_sel = XG_RCVD_SELf;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PORT_SELf;
    soc_field_t l1_rcvd_clk_rstn = L1_RCVD_CLK_RSTNf;
    soc_field_t l1_rcvd_freq_sel = L1_RCVD_FREQ_SELf;

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_port_sel = L1_RCVD_BKUP_PORT_SELf;
        xg_rcvd_sel = XG_RCVD_BKUP_SELf;
        l1_rcvd_clk_rstn = L1_RCVD_CLK_BKUP_RSTNf;
        l1_rcvd_freq_sel = L1_RCVD_BKUP_FREQ_SELf;
    }
    /* Set Registers for TOMAHAWK3 */
    if (SOC_IS_TOMAHAWK3(unit)) {
        synce_ctrl = TOP_SYNCE_CTRLr;
        synce_ctrl_2 = TOP_SYNCE_CTRLr;
    }

    if (!BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        !BCM_TIME_TOMAHAWK_PORT_VALID(lport) ||
        (phy_port == -1)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
        NULL, &phy_port));
    port_sel = phy_port - 1;

    /* TOP_MISC_CONTROL */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 0));

    /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_freq_sel, 0));
    sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                    (divisor->stage0_sdm_frac & 0xff));
#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, lport)) {
        portmod_port_synce_clk_ctrl_t_init(unit, &config);
        config.stg0_mode = divisor->stage0_mode;
        config.stg1_mode = 0x0;
        config.sdm_val = sdm_val;
        if (!SOC_PORT_VALID(unit,lport))
        {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_set(unit, lport, &config));
    } else
#endif
    {
        /* Set TSCF SDM divisors */
        /* Set SDM stage 1 mode to bypass */
        BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, divisor->stage1_div,
                            BCM_TIME_TSCF_MAIN0_SYNCE_CTL_REG));

        /* Set stage 0 mode to SDM mode */
        BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport,
                                divisor->stage0_mode, BCM_TIME_TSCF_MAIN0_SYNCE_CTL_STAGE0_REG));

        BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, sdm_val,
                            BCM_TIME_TSCF_RX_X4_SYNCE_FRACTIONAL_DIV_REG));
    }
    /* TOP_MISC_CONTROL_2  - set sg pll port sel 0 */
    if (!SOC_IS_TOMAHAWK3(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                xg_rcvd_sel, 0));
    }
    /* serdes port configuration */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                               REG_PORT_ANY, l1_rcvd_port_sel, port_sel));
    /* TOP_MISC_CONTROL */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 1));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_tomahawkx_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes: This function takes care of tomahawk only
 */
STATIC int
_bcm_esw_time_tomahawkx_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, val2, rval;
    int lport;
    uint32 sdm_val;
    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t xg_rcvd_sel = XG_RCVD_SELf;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PORT_SELf;
    portmod_port_synce_clk_ctrl_t config;

    if (SOC_IS_TOMAHAWK3(unit)) {
        synce_ctrl_2 = TOP_SYNCE_CTRLr;
    }

    portmod_port_synce_clk_ctrl_t_init(unit, &config);

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_port_sel = L1_RCVD_BKUP_PORT_SELf;
        xg_rcvd_sel = XG_RCVD_BKUP_SELf;
    }
    /* input source port */
    val = 0;
    soc_reg32_get(unit, synce_ctrl_2, REG_PORT_ANY, 0, &rval);

    if (!SOC_IS_TOMAHAWK3(unit)) {
        val = soc_reg_field_get(unit, synce_ctrl_2, rval, xg_rcvd_sel);
    }
    val2 = soc_reg_field_get(unit, synce_ctrl_2, rval, l1_rcvd_port_sel);
   if (val == 0) { /* source is Port */
        uint32 phy_port;
        phy_port = val2 + 1;
        _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
        divisor->input_src = bcmTimeSynceInputSourceTypePort;
        divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
    } else { /* source is PLL */
        /* TOP_MISC_CONTROL_2.L1_RCVD_PORT_SELf needs to be 0
         * Otherwise, there's no clock output */
        if (val2 != 0) {
            LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                        "No clock output\n")));
            return BCM_E_FAIL;
        }
        divisor->input_src = bcmTimeSynceInputSourceTypePLL;
        divisor->index = val - 1;
    }

    /* READ the SDM divisors from TSC */
    lport = divisor->index;

    if (!SOC_PORT_VALID(unit,lport))
    {
        return BCM_E_PARAM;
    }

#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, lport)) {
        SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_get(unit, lport, &config));
        /* stage0_mode */
        divisor->stage0_mode = config.stg0_mode;
        sdm_val = config.sdm_val;
        /* stage0_whole */
        divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
        /* stage0_frac */
        divisor->stage0_sdm_frac = sdm_val & 0xff ;
        /* stage1_div  */
        divisor->stage1_div = config.stg1_mode;
    } else
#endif
    {
        /* stage0_mode */
        BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_get(unit, lport,
        BCM_TIME_TSCF_MAIN0_SYNCE_CTL_STAGE0_REG,  &divisor->stage0_mode));
        BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_get(unit, lport,
        BCM_TIME_TSCF_RX_X4_SYNCE_FRACTIONAL_DIV_REG, &sdm_val));
        /* stage0_whole */
        divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
        /* stage0_frac */
        divisor->stage0_sdm_frac = sdm_val & 0xff ;
        /* stage1_div  */
        BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_get(unit, lport,
        BCM_TIME_TSCF_MAIN0_SYNCE_CTL_REG, &divisor->stage1_div));
    }

    return BCM_E_NONE;
}

#define BCM_TIME_APACHEX_PLL_VALID(x) (((x) >= 0) && ((x) <= 3))
#define BCM_TIME_APACHEX_PORT_VALID(x) ((x) >= 1 && (x) <= 72 )
#define BCM_TIME_APACHEX_LCPLL_PORT_BASE (73)

STATIC int
_bcm_esw_time_apachex_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int port_sel;
#ifdef PHYMOD_TSCF_SUPPORT
    uint32 rval;
    uint16 sdm_val;
#endif
    phymod_dispatch_type_t dispatch_type = phymodDispatchTypeInvalid;

    if (!BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        !BCM_TIME_APACHEX_PORT_VALID(lport) ||
        (phy_port == -1)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
        NULL, &phy_port));

    port_sel = phy_port - 1;
    switch (clk_src) {
    case bcmTimeSynceClockSourcePrimary:
            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                        L1_RCVD_CLK_RSTNf, 0));

            /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_FREQ_SELf, 0));

            dispatch_type = _bcm_time_synce_tsc_phymod_dispatch_type_get(unit, lport);
            switch (dispatch_type) {
#ifdef PHYMOD_TSCE_SUPPORT
                case phymodDispatchTypeTsce:
                    /* register is similar to TD2+ */
                    _bcm_time_td2p_divctrl_reg_modify_primary(unit, divisor);

                    /* Turn off TSC div11 */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, 0x0,
                        BCM_TIME_TSCE_MAIN0_SYNCE_CTL_STAGE0_REG));
                    break;
#endif /* PHYMOD_TSCE_SUPPORT */
#ifdef PHYMOD_TSCF_SUPPORT
                case  phymodDispatchTypeTscf:
                    /* reset chip level SDM config before phy SDM config*/
                    SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_CTRLr(unit, &rval));
                    soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CTRLr, &rval,
                                      STAGE0_MODEf, 0x0);
                    soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CTRLr, &rval,
                                      STAGE1_MODEf, 0x0);
                    soc_reg_field_set(unit, TOP_L1_RCVD_CLK_CTRLr, &rval,
                                      SDM_DIVISORf, 0x0);
                    WRITE_TOP_L1_RCVD_CLK_CTRLr(unit, rval);


                    /* Set TSCF SDM divisors */
                    /* Set SDM stage 1 mode to bypass */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, 0x0,
                        BCM_TIME_TSCF_MAIN0_SYNCE_CTL_REG));

                    /* Set stage 0 mode to SDM mode */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport,
                        divisor->stage0_mode, BCM_TIME_TSCF_MAIN0_SYNCE_CTL_STAGE0_REG));

                    sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                               (divisor->stage0_sdm_frac & 0xff));

                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, sdm_val,
                        BCM_TIME_TSCF_RX_X4_SYNCE_FRACTIONAL_DIV_REG));
                    break;
#endif /* PHYMOD_TSCF_SUPPORT */
                    default: break;
            }

            /* TOP_MISC_CONTROL_2 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        XG_RCVD_PRI_SELf, 0));
            /* EGR_L1_CLK_RECOVERY_CTRL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                        REG_PORT_ANY, PRI_PORT_SELf, port_sel));

            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                        L1_RCVD_CLK_RSTNf, 1));
            break;
    case bcmTimeSynceClockSourceSecondary:
            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                        L1_RCVD_CLK_BKUP_RSTNf, 0));

            /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_BKUP_FREQ_SELf, 0));

            dispatch_type = _bcm_time_synce_tsc_phymod_dispatch_type_get(unit, lport);

            switch (dispatch_type) {
#ifdef PHYMOD_TSCE_SUPPORT
                case phymodDispatchTypeTsce:
                    /* register is similar to TD2+ */
                    _bcm_time_td2p_divctrl_reg_modify_secondary(unit, divisor);

                    /* Turn off TSC div11 */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, 0x0,
                        BCM_TIME_TSCE_MAIN0_SYNCE_CTL_STAGE0_REG));
                    break;
#endif /* PHYMOD_TSCE_SUPPORT */
#ifdef PHYMOD_TSCF_SUPPORT
                case phymodDispatchTypeTscf:
                    /* reset chip level SDM config before phy SDM config*/
                    SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_BKUP_CTRLr(unit, &rval));
                    soc_reg_field_set(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr, &rval,
                                      STAGE0_MODEf, 0x0);
                    soc_reg_field_set(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr, &rval,
                                      STAGE1_MODEf, 0x0);
                    soc_reg_field_set(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr, &rval,
                                      SDM_DIVISORf, 0x0);
                    WRITE_TOP_L1_RCVD_CLK_BKUP_CTRLr(unit, rval);


                    /* Set TSCF SDM divisors */
                    /* Set SDM stage 1 mode to bypass */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, 0x0,
                        BCM_TIME_TSCF_MAIN0_SYNCE_CTL_REG));

                    /* Set stage 0 mode to SDM mode */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport,
                        divisor->stage0_mode, BCM_TIME_TSCF_MAIN0_SYNCE_CTL_STAGE0_REG));

                    sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                               (divisor->stage0_sdm_frac & 0xff));

                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, sdm_val,
                        BCM_TIME_TSCF_RX_X4_SYNCE_FRACTIONAL_DIV_REG));
                    break;
#endif /* PHYMOD_TSCF_SUPPORT */
                    default: break;
            }

            /* TOP_MISC_CONTROL_2 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        XG_RCVD_BKUP_SELf, 0));
            /* EGR_L1_CLK_RECOVERY_CTRL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                        REG_PORT_ANY, BKUP_PORT_SELf, port_sel));

            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                        L1_RCVD_CLK_BKUP_RSTNf, 1));
        break;
    /* coverity[dead_error_begin:FALSE] */
    default:
        return BCM_E_PARAM;

    }
    return BCM_E_NONE;
}

/* NOTES : this function takes care of Apache/ Maverick only */
STATIC int
_bcm_esw_time_apachex_synce_clock_set_by_pll(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 pll_index = divisor->index;

    /* coverity[unsigned_compare] */
    if (!BCM_TIME_APACHEX_PLL_VALID(pll_index) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src)) {
        return BCM_E_PARAM;
    }
    pll_index = pll_index + BCM_TIME_APACHEX_LCPLL_PORT_BASE; /* serdes LCPLL base */
    if (clk_src == bcmTimeSynceClockSourcePrimary) {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_FREQ_DIV_SELf, 0));
        /* _TOP_L1_RCVD_CLK_CTRL */
        SOC_IF_ERROR_RETURN(_bcm_time_td2p_divctrl_reg_modify_primary(unit, divisor));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    XG_RCVD_PRI_SELf, pll_index));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, PRI_PORT_SELf, 0));
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 1));
    } else {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_BKUP_FREQ_DIV_SELf, 0));
        /* _TOP_L1_RCVD_CLK_BKUP_CTRL */
        SOC_IF_ERROR_RETURN
            (_bcm_time_td2p_divctrl_reg_modify_secondary(unit, divisor));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    XG_RCVD_BKUP_SELf, pll_index));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, BKUP_PORT_SELf, 0));
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 1));
    }
    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_esw_time_apachex_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT, PLL)
 *      index     - (IN) if input_src is PORT, specify logical port number 0..30
 *                       if input_src is PLL, specify PLL index 0..2
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes: This function only configures for SB2
 */
STATIC int
_bcm_esw_time_apachex_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;
    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_apachex_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    case bcmTimeSynceInputSourceTypePLL:
        rv = _bcm_esw_time_apachex_synce_clock_set_by_pll(unit, clk_src, divisor);
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
}


/*
 * Function:
 *      _bcm_esw_time_apachex_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes: This function takes care of tomahawk only
 */
STATIC int
_bcm_esw_time_apachex_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, val2, rval, lport;
    uint32 sdm_val;
    phymod_dispatch_type_t dispatch_type = phymodDispatchTypeInvalid;

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        /* input source port */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_2r(unit, &rval));
        val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                XG_RCVD_PRI_SELf);
        SOC_IF_ERROR_RETURN(
             READ_EGR_L1_CLK_RECOVERY_CTRLr(unit, &rval));
        val2 = soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr, rval,
                                     PRI_PORT_SELf);

        if (val == 0) { /* source is Port */
            uint32 phy_port;
            phy_port = val2 + 1;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
        } else { /* source is PLL */
            /* TOP_MISC_CONTROL_2.L1_RCVD_PORT_SELf needs to be 0
             * Otherwise, there's no clock output */
            if (val2 != 0) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = BCM_TIME_APACHEX_LCPLL_PORT_BASE - val;
        }

        /* READ the SDM divisors from TSC */
        if (divisor->input_src == bcmTimeSynceInputSourceTypePort) {

            /* input source type port */
            lport = divisor->index;

            dispatch_type = _bcm_time_synce_tsc_phymod_dispatch_type_get(unit, lport);
            switch(dispatch_type) {
#ifdef PHYMOD_TSCE_SUPPORT
                case phymodDispatchTypeTsce:
                    SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_CTRLr(unit, &rval));
                    /* stage0_mode */
                    divisor->stage0_mode = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CTRLr,
                                                             rval, STAGE0_MODEf);
                    sdm_val = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CTRLr, rval,
                                                SDM_DIVISORf);
                    /* stage0_whole */
                    divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
                    /* stage0_frac */
                    divisor->stage0_sdm_frac = sdm_val & 0xff ;
                    /* stage1_div  */
                    divisor->stage1_div = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CTRLr,
                                                            rval, STAGE1_MODEf);
                    break;
#endif /* PHYMOD_TSCE_SUPPORT */
#ifdef PHYMOD_TSCF_SUPPORT
                case phymodDispatchTypeTscf:
                    /* stage0_mode */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_get(unit, lport,
                         BCM_TIME_TSCF_MAIN0_SYNCE_CTL_STAGE0_REG,  &divisor->stage0_mode));

                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_get(unit, lport,
                        BCM_TIME_TSCF_RX_X4_SYNCE_FRACTIONAL_DIV_REG, &sdm_val));

                    /* stage0_whole */
                    divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
                    /* stage0_frac */
                    divisor->stage0_sdm_frac = sdm_val & 0xff ;

                    /* stage1_div  */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_get(unit, lport,
                        BCM_TIME_TSCF_MAIN0_SYNCE_CTL_REG, &divisor->stage1_div));
                    break;
#endif /* PHYMOD_TSCF_SUPPORT */
                default: break;
            }
        } else {
            /* input source type pll */
             SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_CTRLr(unit, &rval));
            /* stage0_mode */
            divisor->stage0_mode = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CTRLr,
                                                     rval, STAGE0_MODEf);
            sdm_val = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CTRLr, rval,
                                        SDM_DIVISORf);
            /* stage0_whole */
            divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
            /* stage0_frac */
            divisor->stage0_sdm_frac = sdm_val & 0xff ;
            /* stage1_div  */
            divisor->stage1_div = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CTRLr,
                                                    rval, STAGE1_MODEf);
        }

        break;
    case bcmTimeSynceClockSourceSecondary:
        /* input source port */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_2r(unit, &rval));
        val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                XG_RCVD_BKUP_SELf);
        SOC_IF_ERROR_RETURN(
             READ_EGR_L1_CLK_RECOVERY_CTRLr(unit, &rval));
        val2 = soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr, rval,
                                     BKUP_PORT_SELf);

        if (val == 0) { /* source is Port */
            uint32 phy_port;
            phy_port = val2 + 1;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
        } else { /* source is PLL */
            /* TOP_MISC_CONTROL_2.L1_RCVD_BKUP_PORT_SELf needs to be 0
             * Otherwise, there's no clock output */
            if (val2 != 0) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = BCM_TIME_APACHEX_LCPLL_PORT_BASE- val;
        }

        /* READ the SDM divisors from TSC */
        if (divisor->input_src == bcmTimeSynceInputSourceTypePort) {
            lport = divisor->index;

            dispatch_type = _bcm_time_synce_tsc_phymod_dispatch_type_get(unit, lport);
                switch(dispatch_type) {
#ifdef PHYMOD_TSCE_SUPPORT
                    case phymodDispatchTypeTsce:
                    SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_BKUP_CTRLr(unit, &rval));
                    /* stage0_mode */
                    divisor->stage0_mode = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr,
                                                             rval, STAGE0_MODEf);
                    sdm_val = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr, rval,
                                                SDM_DIVISORf);
                    /* stage0_whole */
                    divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
                    /* stage0_frac */
                    divisor->stage0_sdm_frac = sdm_val & 0xff ;
                    /* stage1_div  */
                    divisor->stage1_div = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr,
                                                            rval, STAGE1_MODEf);
                    break;
#endif /* PHYMOD_TSCE_SUPPORT */
#ifdef PHYMOD_TSCF_SUPPORT
                    case phymodDispatchTypeTscf:
                    /* stage0_mode */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_get(unit, lport,
                         BCM_TIME_TSCF_MAIN0_SYNCE_CTL_STAGE0_REG,  &divisor->stage0_mode));

                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_get(unit, lport,
                        BCM_TIME_TSCF_RX_X4_SYNCE_FRACTIONAL_DIV_REG, &sdm_val));

                    /* stage0_whole */
                    divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
                    /* stage0_frac */
                    divisor->stage0_sdm_frac = sdm_val & 0xff ;

                    /* stage1_div  */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_get(unit, lport,
                        BCM_TIME_TSCF_MAIN0_SYNCE_CTL_REG, &divisor->stage1_div));
                    break;
#endif /* PHYMOD_TSCF_SUPPORT */
                    default: break;
            }
        } else {
            SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_BKUP_CTRLr(unit, &rval));
            /* stage0_mode */
            divisor->stage0_mode = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr,
                                                     rval, STAGE0_MODEf);
            sdm_val = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr, rval,
                                        SDM_DIVISORf);
            /* stage0_whole */
            divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
            /* stage0_frac */
            divisor->stage0_sdm_frac = sdm_val & 0xff ;
            /* stage1_div  */
            divisor->stage1_div = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_BKUP_CTRLr,
                                                    rval, STAGE1_MODEf);
        }
        break;
    /* coverity[dead_error_begin:FALSE] */
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT)  || defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)*/

#if defined(BCM_SABER2_SUPPORT)

#define BCM_TIME_SB2_PORT_VALID(x) ((x) >= 1 && (x) <= 28)
#define BCM_TIME_SB2_PLL_VALID(x) (((x) >= 0) && ((x) <= 2))

/* NOTES : this function takes care of Saber2 only */
STATIC int
_bcm_esw_time_saber2_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int mux_index;
    uint32 phy_reg = 0x9002;
    if (!BCM_TIME_SB2_PORT_VALID(lport) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        (phy_port == -1)) {
        return BCM_E_PARAM;
    }

    mux_index = lport - 1;

    if ((mux_index >= 24  &&  mux_index <= 27) &&
            divisor->stage0_mode  == bcmTimeSynceModeBypass) {
        return BCM_E_PARAM;
    }


    if ( clk_src == bcmTimeSynceClockSourcePrimary ) {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK0_RECOVERY_DIV_CTRLf, 0x0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK0_RECOVERY_MUX_SELf, mux_index));
        if ((mux_index >= 24  &&  mux_index <= 27) ) {
            /* Egal Ports */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSCE_CONFIGr, REG_PORT_ANY,
                                        SYNCE_STAGE0_MODEf, 0x2));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSCE_CONFIGr, REG_PORT_ANY,
                                        SDM_DIVISORf, 0x14a0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSCE_CONFIGr, REG_PORT_ANY,
                                        SYNCE_MODEf, 0x0));
            phy_port  = _bcm_switch_port_to_phyaddr(unit, lport);
            if (phy_port == 0xff) {
                /* no phy address assigned */
                return BCM_E_PARAM;
            }
            /* all lanes divisor configured 0
             * by default, eagle phy does /11, setting this configuration is
             * mandatory to get correct frequency...
             * Q: do we need to implement per lane reset .. not seen any
             * scenerio though currently
             */
            BCM_IF_ERROR_RETURN(
                soc_sbus_mdio_write(unit, phy_port, phy_reg, 0));
        } else {
            if (divisor->stage0_mode  == bcmTimeSynceModeSDMFracDiv) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                            L1_CLK0_RECOVERY_DIV_CTRLf, 0x3));
            }
        }

        /* enable prim recov clk as VALID indication from invalid status */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK0_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                     L1_RCVD_SW_OVWR_ENf, 1 ));

    } else {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK1_RECOVERY_DIV_CTRLf, 0x0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK1_RECOVERY_MUX_SELf, mux_index));
        if ((mux_index >= 24  &&  mux_index <= 27) ) {
            /* Egal Ports */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSCE_CONFIGr, REG_PORT_ANY,
                                        SYNCE_STAGE0_MODEf, 0x2));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSCE_CONFIGr, REG_PORT_ANY,
                                        SDM_DIVISORf, 0x14a0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSCE_CONFIGr, REG_PORT_ANY,
                                        SYNCE_MODEf, 0x0));
            phy_port  = _bcm_switch_port_to_phyaddr(unit, lport);
            if (phy_port == 0xff) {
                /* no phy address assigned */
                return BCM_E_PARAM;
            }
            /* all lanes divisor configured 0
             * by default, eagle phy does /11, setting this configuration is
             * mandatory to get correct frequency...
             * Q: do we need to implement per lane reset .. not seen any
             * scenerio though currently
             */
            BCM_IF_ERROR_RETURN(
                soc_sbus_mdio_write(unit, phy_port, phy_reg, 0));
        } else {
            if (divisor->stage0_mode  == bcmTimeSynceModeSDMFracDiv) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                            L1_CLK1_RECOVERY_DIV_CTRLf, 0x3));
            }
        }

        /* enable secondary recov clk as VALID indication from invalid status */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK1_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                     L1_RCVD_SW_OVWR_ENf, 1 ));


    }
    return BCM_E_NONE;
}

/* NOTES : this function takes care of Saber2 only */
STATIC int
_bcm_esw_time_saber2_synce_clock_set_by_pll(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 pll_index = divisor->index;
    uint32 mux_index;
    /* coverity[unsigned_compare] */
    if (!BCM_TIME_SB2_PLL_VALID(pll_index) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src)) {
        return BCM_E_PARAM;
    }

    /* PLL we support only 125MHz */
    if (pll_index < 2 && divisor->stage0_mode == bcmTimeSynceModeSDMFracDiv) {
        return BCM_E_PARAM;
    }

    mux_index = pll_index + 28; /* SERDES LCPLL Base for Mux */

    if (clk_src == bcmTimeSynceClockSourcePrimary) {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK0_RECOVERY_DIV_CTRLf, 0x0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK0_RECOVERY_MUX_SELf, mux_index));

        /* enable prim recov clk as VALID indication from invalid status */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK0_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                     L1_RCVD_SW_OVWR_ENf, 1 ));

    } else {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK1_RECOVERY_DIV_CTRLf, 0x0));
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK1_RECOVERY_MUX_SELf, mux_index));

        /* enable secondary recov clk as VALID indication from invalid status */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK1_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                     L1_RCVD_SW_OVWR_ENf, 1 ));

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_saber2_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT, PLL)
 *      index     - (IN) if input_src is PORT, specify logical port number 0..30
 *                       if input_src is PLL, specify PLL index 0..2
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes: This function only configures for SB2
 */
STATIC int
_bcm_esw_time_saber2_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;
    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_saber2_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    case bcmTimeSynceInputSourceTypePLL:
        rv = _bcm_esw_time_saber2_synce_clock_set_by_pll(unit, clk_src, divisor);
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_time_saber2_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes: This function takes care of Saber2 only
 */
STATIC int
_bcm_esw_time_saber2_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 mux_val, div_val, rval;
    uint16 phy_port;

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_0r(unit, &rval));
        mux_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_0r, rval,
                                L1_CLK0_RECOVERY_MUX_SELf);
        div_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_0r, rval,
                                L1_CLK0_RECOVERY_DIV_CTRLf);
        if (mux_val <= 27) { /* source is Port */
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            phy_port = mux_val + 1;
            divisor->index = phy_port;
            /* stage0_mode */
            if ((div_val & 0x3) && (mux_val <=23))
                divisor->stage0_mode = bcmTimeSynceModeSDMFracDiv;
            else if (mux_val >= 24 && mux_val <=27)
                divisor->stage0_mode = bcmTimeSynceModeSDMFracDiv;
            else
                divisor->stage0_mode = bcmTimeSynceModeBypass;

        } else { /* source is PLL */
            if (mux_val > 30) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->stage0_mode = bcmTimeSynceModeBypass;
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = mux_val - 28;
        }

        break;
    case bcmTimeSynceClockSourceSecondary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_0r(unit, &rval));
        mux_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_0r, rval,
                                L1_CLK1_RECOVERY_MUX_SELf);
        div_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_0r, rval,
                                L1_CLK1_RECOVERY_DIV_CTRLf);
        if (mux_val <= 27) {
            /* source is Port */
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            phy_port = mux_val + 1;
            divisor->index = phy_port;
            /* stage0_mode */
            if ((div_val & 0x3) && (mux_val <=23))
                divisor->stage0_mode = bcmTimeSynceModeSDMFracDiv;
            else if (mux_val >= 24 && mux_val <=27)
                divisor->stage0_mode = bcmTimeSynceModeSDMFracDiv;
            else
                divisor->stage0_mode = bcmTimeSynceModeBypass;
        } else {
            /* source is PLL */
            if (mux_val > 30) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->stage0_mode = bcmTimeSynceModeBypass;
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = mux_val - 28;
        }
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}
#endif

#if defined(BCM_METROLITE_SUPPORT)

#define BCM_TIME_ML_PORT_VALID(x) ((x) >= 1 && (x) <= 12) /* Table23, ML top spec(ver0p1) */
#define BCM_TIME_ML_PLL_VALID(x) ((x) <= 1) /* Table23, ML top spec(verop1) */

STATIC int
_bcm_esw_time_metrolite_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int mux_index;
    uint32 phy_reg = 0x9002;
    if (!BCM_TIME_ML_PORT_VALID(lport) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        (phy_port == -1)) {
        return BCM_E_PARAM;
    }

    mux_index = lport - 1;

    /* For eagle TSCE synce div logic handled by portmacro */
    if ((mux_index >= 4  &&  mux_index <= 11) &&
            divisor->stage0_mode  == bcmTimeSynceModeBypass) {
        return BCM_E_PARAM;
    }


    if ( clk_src == bcmTimeSynceClockSourcePrimary ) {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK0_RECOVERY_DIV_CTRLf, 0x5)); /* Table25 ML top spec ::enable active_reset
                                                                            * and TSCE synce div logic
                                                                            */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK0_RECOVERY_MUX_SELf, mux_index));
        if ((mux_index >= 4  &&  mux_index <= 11) ) {
            /* Eagle Ports */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSC_SYNCE_DIV_CONFIG_0r, REG_PORT_ANY,
                                        SYNCE_STAGE0_MODEf, 0x2)); /* TSCE stage0 divisor(SDM mode choosen) */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSC_SYNCE_DIV_CONFIG_0r, REG_PORT_ANY,
                                        SDM_DIVISORf, 0x14a0)); /* TSCE SDM divisor */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSC_SYNCE_DIV_CONFIG_0r, REG_PORT_ANY,
                                        SYNCE_MODEf, 0x0)); /* TSCE stage1 divisor(div1 choosen) */
            phy_port  = _bcm_switch_port_to_phyaddr(unit, lport);
            if (phy_port == 0xff) {
                /* no phy address assigned */
                return BCM_E_PARAM;
            }
            /* all lanes divisor configured 0
             * by default, eagle phy does /11, setting this configuration is
             * mandatory to get correct frequency...
             * Q: do we need to implement per lane reset .. not seen any
             * scenerio though currently
             */
            BCM_IF_ERROR_RETURN(
                soc_sbus_mdio_write(unit, phy_port, phy_reg, 0));
        } else { /* for viper ports : 25MHz/125MHz L1 recovery */
            if (divisor->stage0_mode  == bcmTimeSynceModeSDMFracDiv) { /* 25MHz L1 recovery from viper ports */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                            L1_CLK0_RECOVERY_DIV_CTRLf, 0x3)); /* enable active_reset and default div5 logic */
            } else { /* 125MHz L1 recovery from viper ports*/
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                            L1_CLK0_RECOVERY_DIV_CTRLf, 0x1));
            }
        }

        /* enable prim recov clk as VALID indication from invalid status */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK0_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                     L1_RCVD_SW_OVWR_ENf, 1 ));

    } else {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK1_RECOVERY_DIV_CTRLf, 0x5));/* Table25 ML top spec ::enable active_reset
                                                                            * and TSCE synce div logic
                                                                            */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK1_RECOVERY_MUX_SELf, mux_index));
        if ((mux_index >= 4  &&  mux_index <= 11) ) {
            /* Eagle Ports */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSC_SYNCE_DIV_CONFIG_1r, REG_PORT_ANY,
                                        SYNCE_STAGE0_MODEf, 0x2)); /* TSCE stage0 divisor(SDM mode choosen) */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSC_SYNCE_DIV_CONFIG_1r, REG_PORT_ANY,
                                        SDM_DIVISORf, 0x14a0)); /* TSCE SDM divisor */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_TSC_SYNCE_DIV_CONFIG_1r, REG_PORT_ANY,
                                        SYNCE_MODEf, 0x0)); /* TSCE stage1 divisor(div1 choosen) */

            phy_port  = _bcm_switch_port_to_phyaddr(unit, lport);
            if (phy_port == 0xff) {
                /* no phy address assigned */
                return BCM_E_PARAM;
            }
            /* all lanes divisor configured 0
             * by default, eagle phy does /11, setting this configuration is
             * mandatory to get correct frequency...
             * Q: do we need to implement per lane reset .. not seen any
             * scenerio though currently
             */
            BCM_IF_ERROR_RETURN(
                soc_sbus_mdio_write(unit, phy_port, phy_reg, 0));
        } else { /* for viper ports : 25MHz/125MHz L1 recovery */
            if (divisor->stage0_mode  == bcmTimeSynceModeSDMFracDiv) { /* 25MHz L1 recovery from viper ports */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                            L1_CLK1_RECOVERY_DIV_CTRLf, 0x3)); /* enable active_reset and default div5 logic */
            } else { /* 125MHz L1 recovery from viper ports */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                            L1_CLK1_RECOVERY_DIV_CTRLf, 0x1)); /* enable only active_reset */
            }
        }
        /* enable secondary recov clk as VALID indication from invalid status */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK1_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                     L1_RCVD_SW_OVWR_ENf, 1 ));

    }
    return BCM_E_NONE;
}

STATIC int
_bcm_esw_time_metrolite_synce_clock_set_by_pll(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 pll_index = divisor->index;
    uint32 mux_index;

    if (!BCM_TIME_ML_PLL_VALID(pll_index) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src)) {
        return BCM_E_PARAM;
    }

    /* MasterLCPLL we support only 125MHz */
    if (pll_index < 1 && divisor->stage0_mode == bcmTimeSynceModeSDMFracDiv) {
        return BCM_E_PARAM;
    }

    mux_index = pll_index + 12; /* Master LCPLL Base for Mux */

    if (clk_src == bcmTimeSynceClockSourcePrimary) {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK0_RECOVERY_DIV_CTRLf, 0x1)); /*Table22, ML top spec::enable active_reset only*/
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK0_RECOVERY_MUX_SELf, mux_index));

        /* enable prim recov clk as VALID indication from invalid status */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK0_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                     L1_RCVD_SW_OVWR_ENf, 1 ));

    } else {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK1_RECOVERY_DIV_CTRLf, 0x1));/*Table22, ML top spec::enable active_reset only*/
        SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_0r, REG_PORT_ANY,
                                        L1_CLK1_RECOVERY_MUX_SELf, mux_index));

        /* enable secondary recov clk as VALID indication from invalid status */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                    L1_RCVD_CLK1_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
             (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                     L1_RCVD_SW_OVWR_ENf, 1 ));

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_metrolite_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT, PLL)
 *      index     - (IN) if input_src is PORT, specify logical port number 0..30
 *                       if input_src is PLL, specify PLL index 0..2
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes: ML specific function
 */
STATIC int
_bcm_esw_time_metrolite_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;
    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_metrolite_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    case bcmTimeSynceInputSourceTypePLL:
        rv = _bcm_esw_time_metrolite_synce_clock_set_by_pll(unit, clk_src, divisor);
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_time_metrolite_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes: This function takes care of Saber2 only
 */
STATIC int
_bcm_esw_time_metrolite_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 mux_val, div_val, rval;
    uint16 phy_port;

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_0r(unit, &rval));
        mux_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_0r, rval,
                                L1_CLK0_RECOVERY_MUX_SELf);
        div_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_0r, rval,
                                L1_CLK0_RECOVERY_DIV_CTRLf);
        if (mux_val <= 11) { /* source is Port */
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            phy_port = mux_val + 1;
            divisor->index = phy_port;
            /* stage0_mode */
            if ((div_val & 0x3) && (mux_val <=3))
                divisor->stage0_mode = bcmTimeSynceModeSDMFracDiv;
            else if ((div_val & 0x5) && (mux_val >= 4 && mux_val <=11))
                divisor->stage0_mode = bcmTimeSynceModeSDMFracDiv;
            else
                divisor->stage0_mode = bcmTimeSynceModeBypass;

        } else { /* source is PLL */
            if (mux_val > 13) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->stage0_mode = bcmTimeSynceModeBypass;
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = mux_val - 12;
        }

        break;
    case bcmTimeSynceClockSourceSecondary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_0r(unit, &rval));
        mux_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_0r, rval,
                                L1_CLK1_RECOVERY_MUX_SELf);
        div_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_0r, rval,
                                L1_CLK1_RECOVERY_DIV_CTRLf);
        if (mux_val <= 11) {
            /* source is Port */
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            phy_port = mux_val + 1;
            divisor->index = phy_port;
            /* stage0_mode */
            if ((div_val & 0x3) && (mux_val <=3))
                divisor->stage0_mode = bcmTimeSynceModeSDMFracDiv;
            else if ((div_val & 0x5) && (mux_val >= 4 && mux_val <=11))
                divisor->stage0_mode = bcmTimeSynceModeSDMFracDiv;
            else
                divisor->stage0_mode = bcmTimeSynceModeBypass;
        } else {
            /* source is PLL */
            if (mux_val > 13) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->stage0_mode = bcmTimeSynceModeBypass;
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = mux_val - 12;
        }
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}
#endif /* defined(BCM_METROLITE_SUPPORT) */

#if defined(BCM_TRIDENT3_SUPPORT)

#define BCM_TIME_TD3_PHY_PORT_VALID(x) ((x) >= 1 && (x) <= 128) /* Section 10.2.4, TD3 top spec */
#define BCM_TIME_TD3_PLL_VALID(x) ((x) <= 3) /* Section 10.2.4 Top spec  */

STATIC int
_bcm_esw_time_trident3_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int port_sel;
    uint16 sdm_val;
    portmod_port_synce_clk_ctrl_t config;
    if(!IS_CL_PORT(unit, lport))
        return BCM_E_PARAM;

    if (!BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        !BCM_TIME_TD3_PHY_PORT_VALID(phy_port)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
        NULL, &phy_port));

    port_sel = phy_port - 1;

    portmod_port_synce_clk_ctrl_t_init(unit, &config);

    switch (clk_src) {
    case bcmTimeSynceClockSourcePrimary:
            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_CLK_RSTNf, 0));

            /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_FREQ_SELf, 0));

            /* Set TSCF SDM divisors */
            /* Set SDM stage 1 mode to bypass */

            sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                       (divisor->stage0_sdm_frac & 0xff));

            config.stg0_mode = divisor->stage0_mode;
            config.stg1_mode = 0x0;
            config.sdm_val = sdm_val;
            if (!SOC_PORT_VALID(unit,lport))
            {
                return BCM_E_PARAM;
            }
            SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_set(unit, lport, &config));

            /* TOP_MISC_CONTROL_2  - set sg pll port sel 0 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        XG_RCVD_SELf, 0));

            /* serdes port configuration */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r,
                                        REG_PORT_ANY, L1_RCVD_PORT_SELf, port_sel));

            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_CLK_RSTNf, 1));
            break;
    case bcmTimeSynceClockSourceSecondary:
            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_CLK_BKUP_RSTNf, 0));

            /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_BKUP_FREQ_SELf, 0));

            /* Set TSCF SDM divisors */
            /* Set SDM stage 1 mode to bypass */
            sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                       (divisor->stage0_sdm_frac & 0xff));

            config.stg0_mode = divisor->stage0_mode;
            config.stg1_mode = 0x0;
            config.sdm_val = sdm_val;
            if (!SOC_PORT_VALID(unit,lport))
            {
                return BCM_E_PARAM;
            }
            SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_set(unit, lport, &config));

            /* TOP_MISC_CONTROL_2 - set sg pll port sel 0*/
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        XG_RCVD_BKUP_SELf, 0));

            /* serdes port configuration */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r,
                                        REG_PORT_ANY, L1_RCVD_BKUP_PORT_SELf, port_sel));

            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_CLK_BKUP_RSTNf, 1));
        break;
    /* coverity[dead_error_begin:FALSE] */
    default:
        return BCM_E_PARAM;

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_trident3_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes: This function takes care of trident3 only
 */
STATIC int
_bcm_esw_time_trident3_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, val2, rval;
    int lport;
    uint32 sdm_val;
    portmod_port_synce_clk_ctrl_t config;
    portmod_port_synce_clk_ctrl_t_init(unit, &config);

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        /* input source port */
        SOC_IF_ERROR_RETURN( READ_TOP_MISC_CONTROL_2r(unit, &rval));

        val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval, XG_RCVD_SELf);
        val2 = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval, L1_RCVD_PORT_SELf);

        if (val == 0) { /* source is Port */
            uint32 phy_port;
            phy_port = val2 + 1;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
        } else { /* source is PLL */
            /* TOP_MISC_CONTROL_2.L1_RCVD_PORT_SELf needs to be 0
             * Otherwise, there's no clock output */
            if (val2 != 0) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = val - 1;
        }

        /* READ the SDM divisors from TSC */
        lport = divisor->index;
        if (!SOC_PORT_VALID(unit,lport))
        {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_get(unit, lport, &config));

        /* stage0_mode */
        divisor->stage0_mode = config.stg0_mode;

        sdm_val = config.sdm_val;

        /* stage0_whole */
        divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
        /* stage0_frac */
        divisor->stage0_sdm_frac = sdm_val & 0xff ;

        /* stage1_div  */
        divisor->stage1_div = config.stg1_mode;

        break;
    case bcmTimeSynceClockSourceSecondary:
        /* input source port */
        SOC_IF_ERROR_RETURN( READ_TOP_MISC_CONTROL_2r(unit, &rval));

        val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval, XG_RCVD_BKUP_SELf);
        val2 = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval, L1_RCVD_BKUP_PORT_SELf);

        if (val == 0) { /* source is Port */
            uint32 phy_port;
            phy_port = val2 + 1;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
        } else { /* source is PLL */
            /* TOP_MISC_CONTROL_2.L1_RCVD_BKUP_PORT_SELf needs to be 0
             * Otherwise, there's no clock output */
            if (val2 != 0) {
                LOG_ERROR(BSL_LS_BCM_TIME, (BSL_META_U(unit,
                            "No clock output\n")));
                return BCM_E_FAIL;
            }
            divisor->input_src = bcmTimeSynceInputSourceTypePLL;
            divisor->index = val - 1;
        }

        /* READ the SDM divisors from TSC */
        lport = divisor->index;
        if (!SOC_PORT_VALID(unit,lport))
        {
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_get(unit, lport, &config));

        /* stage0_mode */
        divisor->stage0_mode = config.stg0_mode;

        sdm_val = config.sdm_val;

        /* stage0_whole */
        divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
        /* stage0_frac */
        divisor->stage0_sdm_frac = sdm_val & 0xff ;

        /* stage1_div  */
        divisor->stage1_div = config.stg1_mode;


        break;
    /* coverity[dead_error_begin:FALSE] */
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_esw_time_trident3_synce_clock_set_by_pll(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 pll_index = divisor->index;

    if (!BCM_TIME_TD3_PLL_VALID(pll_index) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src)) {
        return BCM_E_PARAM;
    }
    switch (clk_src) {
        case bcmTimeSynceClockSourcePrimary:
            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_CLK_RSTNf, 0));

            /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_FREQ_SELf, 0));

            /* TOP_MISC_CONTROL_2  - set sg pll port sel 0 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        XG_RCVD_SELf, pll_index));

            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_CLK_RSTNf, 1));
            break;

    case bcmTimeSynceClockSourceSecondary:
            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_CLK_BKUP_RSTNf, 0));

            /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_BKUP_FREQ_SELf, 0));

            /* TOP_MISC_CONTROL_2 - set sg pll port sel 0*/
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        XG_RCVD_BKUP_SELf, pll_index));

            /* TOP_MISC_CONTROL */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_CLK_BKUP_RSTNf, 1));
            break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_trident3_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT, PLL)
 *      index     - (IN) if input_src is PORT, specify logical port number 0..105
 *                       if input_src is PLL, specify PLL index 0..3
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes: This function only configures for td2plus
 */
STATIC int
_bcm_esw_time_trident3_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;

    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_trident3_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    case bcmTimeSynceInputSourceTypePLL:
        rv = _bcm_esw_time_trident3_synce_clock_set_by_pll(unit, clk_src, divisor);
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }

    return rv;
}


#endif /* defined(BCM_TRIDENT3_SUPPORT) */

#if defined(BCM_MAVERICK2_SUPPORT)

#define BCM_TIME_MV2_PHY_PORT_VALID(x) ((x) >= 0 && (x) <= 80) /* Section 10.2.2 and 5.2, MV2 top spec */
STATIC int
_bcm_esw_time_maverick2_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int port_sel;
    uint16 sdm_val;
    portmod_port_synce_clk_ctrl_t config;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_clk_rstn = L1_RCVD_CLK_RSTNf;
    soc_field_t l1_rcvd_freq_sel = L1_RCVD_FREQ_SELf;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PORT_SELf;

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_clk_rstn = L1_RCVD_CLK_BKUP_RSTNf;
        l1_rcvd_freq_sel = L1_RCVD_BKUP_FREQ_SELf;
        l1_rcvd_port_sel = L1_RCVD_BKUP_PORT_SELf;
    }

    if(!IS_CL_PORT(unit, lport))
        return BCM_E_PARAM;

    if (!BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        !BCM_TIME_MV2_PHY_PORT_VALID(phy_port)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
        NULL, &phy_port));

    port_sel = phy_port - 1;

    portmod_port_synce_clk_ctrl_t_init(unit, &config);

    /* TOP_MISC_CONTROL_2 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 0));

    /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_freq_sel, 0));

    /* Set TSCF SDM divisors */
    /* Set SDM stage 1 mode to bypass */

    sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
               (divisor->stage0_sdm_frac & 0xff));

    config.stg0_mode = divisor->stage0_mode;
    config.stg1_mode = 0x0;
    config.sdm_val = sdm_val;
    if (!SOC_PORT_VALID(unit,lport))
    {
        return BCM_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_set(unit, lport, &config));

    /* serdes port configuration */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                                REG_PORT_ANY, l1_rcvd_port_sel, port_sel));

    /* TOP_MISC_CONTROL_2 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 1));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_maverick2_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes: This function takes care of maverick2 only
 */
STATIC int
_bcm_esw_time_maverick2_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, rval;
    int lport;
    uint32 sdm_val;
    uint32 phy_port;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PORT_SELf;

    portmod_port_synce_clk_ctrl_t config;
    portmod_port_synce_clk_ctrl_t_init(unit, &config);

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_port_sel = L1_RCVD_BKUP_PORT_SELf;
    }

    /* input source port */
    SOC_IF_ERROR_RETURN( READ_TOP_MISC_CONTROL_2r(unit, &rval));

    val = soc_reg_field_get(unit, synce_ctrl_2, rval, l1_rcvd_port_sel);

    phy_port = val + 1;
    _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
    divisor->input_src = bcmTimeSynceInputSourceTypePort;
    divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];

    /* READ the SDM divisors from TSC */
    lport = divisor->index;
    if (!SOC_PORT_VALID(unit,lport))
    {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_get(unit, lport, &config));
    /* stage0_mode */
    divisor->stage0_mode = config.stg0_mode;
    sdm_val = config.sdm_val;
    /* stage0_whole */
    divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
    /* stage0_frac */
    divisor->stage0_sdm_frac = sdm_val & 0xff ;
    /* stage1_div  */
    divisor->stage1_div = config.stg1_mode;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_maverick2_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT, PLL)
 *      index     - (IN) if input_src is PORT, specify logical port number 0..105
 *                       if input_src is PLL, specify PLL index 0..3
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes: This function only configures for maverick2
 */
STATIC int
_bcm_esw_time_maverick2_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;

    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_maverick2_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    default:
        rv = BCM_E_PARAM;
        break;
    }
    return rv;
}

#endif /* defined(BCM_MAVERICK2_SUPPORT) */

#define BCMI_TIME_L1_RCVD_CLK_PORT_SEL_MAX (40)
#define BCMI_TIME_PORT_SEL_INVALID         (-1)
#define BCMI_TIME_PHY_PORT_INVALID         (-1)

#if defined(BCM_FIREBOLT6_SUPPORT)

#define BCMI_TIME_FB6_L1_RCVD_CLK_PORT_SEL_MAX (68)
#define BCMI_TIME_FB6_PHY_PORT_VALID(x) ((x) >= 1 && (x) <= FB6_NUM_PHY_PORT)

/* Physical port to PORT_SEL */
STATIC int
bcmi_esw_time_fb6_synce_L1_rcvd_clk_port_get(int phy_port)
{
    if (phy_port >= 1 && phy_port <= FB6_NUM_PHY_PORT) {
        /* 17 x PM_4X25 : 68 SyncE Clocks */
        return (phy_port - 1);
    }
    else {
        return BCMI_TIME_PORT_SEL_INVALID;
    }
}

/* PORT_SEL to Physical port */
STATIC int
bcmi_esw_time_fb6_phy_port_get(int port_sel)
{
    if (port_sel >= 0 && port_sel < FB6_NUM_PHY_PORT) {
        return (port_sel + 1);
    }
    else {
        return BCMI_TIME_PHY_PORT_INVALID;
    }
}
#endif /* BCM_FIREBOLT6_SUPPORT */

#if defined(BCM_HURRICANE4_SUPPORT)
#define BCMI_TIME_HR4_PHY_PORT_VALID(x) ((x) >= 1 && (x) <= 64)

/* Physical port to PORT_SEL */
STATIC int
bcmi_esw_time_hr4_synce_L1_rcvd_clk_port_get(int phy_port)
{
    if (phy_port >= 1 && phy_port <= 24) {
        /* GPHY_0 - GPHY_5     : 24 SyncE Clocks */
        return (phy_port - 1);
    } else if (phy_port >= 25 && phy_port <= 56) {
        /* PM4x10Q_0 + PM4x10Q_1 :  4 + 4 SyncE Clocks */
        return ((phy_port - 25)/4 + 24);
    } else if (phy_port >= 57 && phy_port <= 64) {
        /* PM4x25_0 + PM4x10_0 :  4 + 4 SyncE Clocks */
        return (phy_port - 24 - 1);
    }
    return BCMI_TIME_PORT_SEL_INVALID;
}

/* PORT_SEL to Physical port */
STATIC int
bcmi_esw_time_hr4_phy_port_get(int port_sel)
{
    if (port_sel < 28) {
        return (port_sel + 1);
    }
    else if (port_sel < 32) {
        return (port_sel + 1 + 12);
    }
    else if (port_sel < BCMI_TIME_L1_RCVD_CLK_PORT_SEL_MAX) {
        return (port_sel + 1 + 24);
    }
    else {
        return BCMI_TIME_PHY_PORT_INVALID;
    }
}
#endif /* BCM_HURRICANE4_SUPPORT */

#if defined(BCM_HELIX5_SUPPORT)
/* Section 9.2, HX5 top spec */
#define BCMI_TIME_HX5_PHY_PORT_VALID(x) ((x) >= 1 && (x) <= 76)

STATIC int
bcmi_esw_time_hx5_synce_L1_rcvd_clk_port_get(int phy_port)
{
    if (phy_port >= 1 && phy_port <= 4) {
        return (phy_port - 1);
    }
    else if (phy_port >= 17 && phy_port <=20) {
        return (phy_port - 12 - 1);
    }
    else if (phy_port >= 33 && phy_port <=36) {
        return (phy_port - 24 - 1);
    }
    else if (phy_port >= 49 && phy_port <=76) {
        return (phy_port - 36 - 1);
    }
    else {
        return BCMI_TIME_PORT_SEL_INVALID;
    }
}

STATIC int
bcmi_esw_time_hx5_phy_port_get(int port_sel)
{
    if (port_sel < 4) {
        return (port_sel + 1);
    }
    else if (port_sel < 8) {
        return (port_sel + 1 + 12);
    }
    else if (port_sel < 12) {
        return (port_sel + 1 + 24);
    }
    else if (port_sel < BCMI_TIME_L1_RCVD_CLK_PORT_SEL_MAX) {
        return (port_sel + 1 + 36);
    }
    else {
        return BCMI_TIME_PHY_PORT_INVALID;
    }
}

/* Check if phy_port is a GPHY port or not. */
STATIC int
bcmi_esw_time_is_gphy_port(
    int unit,
    int phy_port)
{
    int val, index;
    for (index = 0;
         index < SOC_DRIVER(unit)->port_num_blktype;
         index++) {
        /* Get the block where the port belongs to */
        val = SOC_PORT_IDX_BLOCK(unit, phy_port, index);
        if (val < 0) {
            continue;
        }
        /* Get the block type */
        val = SOC_BLOCK_TYPE(unit, val);
        if (val == SOC_BLK_GPORT ||
            val == SOC_BLK_GXPORT ||
            val == SOC_BLK_GXFPPORT) {
            return TRUE;
        }
    }
    return FALSE;
}

STATIC int
_bcm_esw_time_hx5_synce_clock_set_by_port(
    int unit,
    bcm_time_synce_clock_src_type_t clk_src,
    bcm_time_synce_divisor_setting_t *divisor)
{
    int lport;
    int phy_port;
    int port_sel = 0;
    uint16 sdm_val;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_clk_rstn = L1_RCVD_CLK_RSTNf;
    soc_field_t l1_rcvd_freq_sel = L1_RCVD_FREQ_SELf;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PORT_SELf;

    if (!BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src)||
        NULL == divisor) {
        return BCM_E_PARAM;
    }

    /* Assumes input index is logical port number */
    lport = divisor->index;
    phy_port = SOC_INFO(unit).port_l2p_mapping[lport];

    if (!SOC_PORT_VALID(unit, lport) ||
#if defined(BCM_HURRICANE4_SUPPORT)
        (!BCMI_TIME_HR4_PHY_PORT_VALID(phy_port) && SOC_IS_HURRICANE4(unit)) ||
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
        (!BCMI_TIME_FB6_PHY_PORT_VALID(phy_port) && SOC_IS_FIREBOLT6(unit)) ||
#endif /* BCM_FIREBOLT6_SUPPORT */
        (!BCMI_TIME_HX5_PHY_PORT_VALID(phy_port) && SOC_IS_HELIX5(unit))) {
        return BCM_E_PARAM;
    }

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_clk_rstn = L1_RCVD_CLK_BKUP_RSTNf;
        l1_rcvd_freq_sel = L1_RCVD_BKUP_FREQ_SELf;
        l1_rcvd_port_sel = L1_RCVD_BKUP_PORT_SELf;
    }

    if(bcmi_esw_time_is_gphy_port(unit, phy_port) == FALSE) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_synce_phy_port_lane_adjust(
                unit, phy_port, NULL, &phy_port));
    }

    
    if (SOC_IS_HELIX5(unit)) {
        port_sel = bcmi_esw_time_hx5_synce_L1_rcvd_clk_port_get(phy_port);
    } else
#if defined(BCM_HURRICANE4_SUPPORT)
    if (SOC_IS_HURRICANE4(unit)) {
        port_sel = bcmi_esw_time_hr4_synce_L1_rcvd_clk_port_get(phy_port);
        phy_port = SOC_HR4_PORT_BLOCK_BASE_PORT(lport);
        if (25 <= phy_port && phy_port <= 56) {
            /* Get first logical port of PM4x10Q. */
            lport = SOC_INFO(unit).port_p2l_mapping[phy_port];
        }
    } else
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
    if (SOC_IS_FIREBOLT6(unit)) {
        port_sel = bcmi_esw_time_fb6_synce_L1_rcvd_clk_port_get(phy_port);
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
    {
        return BCM_E_UNAVAIL;
    }

    if (BCMI_TIME_PORT_SEL_INVALID == port_sel) {
        return BCM_E_INTERNAL;
    }

    /* TOP_MISC_CONTROL_2 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 0));

    /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_freq_sel, 0));

#ifdef PORTMOD_SUPPORT
    if ((SOC_PORT_USE_PORTCTRL(unit, lport))) {
        portmod_port_synce_clk_ctrl_t config;
        portmod_port_synce_clk_ctrl_t_init(unit, &config);
        /* Set TSCF SDM divisors */
        /* Set SDM stage 1 mode to bypass */
        sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                   (divisor->stage0_sdm_frac & 0xff));

        config.stg0_mode = divisor->stage0_mode;
        config.stg1_mode = divisor->stage1_div;
        config.sdm_val = sdm_val;
        SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_set(unit, lport, &config));
    }
#endif /* PORTMOD_SUPPORT */

    /* serdes port configuration */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                                REG_PORT_ANY, l1_rcvd_port_sel, port_sel));

    /* TOP_MISC_CONTROL_2 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 1));
    return BCM_E_NONE;
}

#define BCM_TIME_HX5_PLL_VALID(x) ((x) < 2)
STATIC int
_bcm_esw_time_hx5_synce_clock_set_by_pll(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 pll_index;
    int port_sel;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_clk_rstn = L1_RCVD_CLK_RSTNf;
    soc_field_t l1_rcvd_freq_sel = L1_RCVD_FREQ_SELf;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PORT_SELf;

    if (NULL == divisor) {
        return BCM_E_PARAM;
    }

#if defined(BCM_FIREBOLT6_SUPPORT)
    if (SOC_IS_FIREBOLT6(unit)) {
        /* FB6 doesn't support PLL clock */
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_FIREBOLT6_SUPPORT */

    pll_index = divisor->index;

    if (!BCM_TIME_HX5_PLL_VALID(pll_index) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src)) {
        return BCM_E_PARAM;
    }

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_clk_rstn = L1_RCVD_CLK_BKUP_RSTNf;
        l1_rcvd_freq_sel = L1_RCVD_BKUP_FREQ_SELf;
        l1_rcvd_port_sel = L1_RCVD_BKUP_PORT_SELf;
    }

    /* TOP_MISC_CONTROL */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 0));

    /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                        l1_rcvd_freq_sel, 0));
    /*
     * 0 : xg_master_pll_clk125
     * 1 : xg_serdes_pll_clk125
     */
    port_sel = pll_index + BCMI_TIME_L1_RCVD_CLK_PORT_SEL_MAX;
    /* TOP_MISC_CONTROL_2  - set pll port sel */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                        REG_PORT_ANY, l1_rcvd_port_sel, port_sel));

    /* TOP_MISC_CONTROL */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 1));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_hx5_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes: This function takes care of helix5 only
 */
STATIC int
_bcm_esw_time_hx5_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, val_max, rval;
    int lport;
    uint32 sdm_val;
    uint32 phy_port;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PORT_SELf;

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_port_sel = L1_RCVD_BKUP_PORT_SELf;
    }

    /* input source port */
    SOC_IF_ERROR_RETURN( READ_TOP_MISC_CONTROL_2r(unit, &rval));

#if defined(BCM_FIREBOLT6_SUPPORT)
    if (SOC_IS_FIREBOLT6(unit)) {
        val_max = BCMI_TIME_FB6_L1_RCVD_CLK_PORT_SEL_MAX;
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
    {
        val_max = BCMI_TIME_L1_RCVD_CLK_PORT_SEL_MAX;
    }

    val = soc_reg_field_get(unit, synce_ctrl_2, rval, l1_rcvd_port_sel);

    if (val < val_max) {
        
        if (SOC_IS_HELIX5(unit)) {
            phy_port = bcmi_esw_time_hx5_phy_port_get(val);
        } else
#if defined(BCM_HURRICANE4_SUPPORT)
        if (SOC_IS_HURRICANE4(unit)) {
            phy_port = bcmi_esw_time_hr4_phy_port_get(val);
        } else
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
        if (SOC_IS_FIREBOLT6(unit)) {
            phy_port = bcmi_esw_time_fb6_phy_port_get(val);
        } else
#endif /* BCM_FIREBOLT6_SUPPORT */
        {
            return BCM_E_UNAVAIL;
        }

        if (BCMI_TIME_PHY_PORT_INVALID == phy_port) {
            return BCM_E_INTERNAL;
        }

        if(bcmi_esw_time_is_gphy_port(unit, phy_port) == FALSE) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port));
        }
        divisor->input_src = bcmTimeSynceInputSourceTypePort;
        divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
    } else {
#if defined(BCM_FIREBOLT6_SUPPORT)
        if (SOC_IS_FIREBOLT6(unit)) {
            /* FB6 doesn't support PLL clock */
            return BCM_E_UNAVAIL;
        }
#endif /* BCM_FIREBOLT6_SUPPORT */
        divisor->input_src = bcmTimeSynceInputSourceTypePLL;
        divisor->index = val - val_max;
    }

    if (divisor->input_src == bcmTimeSynceInputSourceTypePort) {
        /* READ the SDM divisors from TSC */
        lport = divisor->index;
        if (!SOC_PORT_VALID(unit, lport)) {
            return BCM_E_PARAM;
        }
#ifdef PORTMOD_SUPPORT
        if (SOC_PORT_USE_PORTCTRL(unit, lport)) {
            portmod_port_synce_clk_ctrl_t config;
            portmod_port_synce_clk_ctrl_t_init(unit, &config);

            SOC_IF_ERROR_RETURN(
                portmod_port_synce_clk_ctrl_get(unit, lport, &config));
            /* stage0_mode */
            divisor->stage0_mode = config.stg0_mode;
            sdm_val = config.sdm_val;
            /* stage0_whole */
            divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
            /* stage0_frac */
            divisor->stage0_sdm_frac = sdm_val & 0xff ;
            /* stage1_div  */
            divisor->stage1_div = config.stg1_mode;
        }
#endif /* PORTMOD_SUPPORT */
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_hx5_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT, PLL)
 *      index     - (IN) if input_src is PORT, specify logical port number 0..105
 *                       if input_src is PLL, specify PLL index 0..3
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes: This function only configures for helix5
 */
STATIC int
_bcm_esw_time_hx5_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;

    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_hx5_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    default:
        rv = _bcm_esw_time_hx5_synce_clock_set_by_pll(unit, clk_src, divisor);
        break;
    }
    return rv;
}

#endif /* defined(BCM_HELIX5_SUPPORT) */

#if defined(BCM_GREYHOUND2_SUPPORT)
#define BCM_TIME_GH2_PHY_PORT_VALID(x) (((x) >= 2 && (x) <= 29) || ((x) >= 42 && (x) <= 45) || \
                                       ((x) >=58 && (x) < 90)) /* Section 7.1.3, GH2 top design spec */

STATIC int
_bcm_esw_time_gh2_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port;
    int port_sel;
    uint16 sdm_val;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PRI_CLK_PORT_SELf;
    soc_field_t l1_rcvd_lnksts_sel = L1_RCVD_PRI_LNKSTS_PORT_SELf;

    soc_reg_t l1_rcvd_clk_ctrl = TOP_L1_RCVD_CLK_CONTROLr;
    soc_field_t l1_rcvd_clk_rstn = L1_RCVD_CLK_RSTNf;
    soc_field_t l1_rcvd_freq_sel = L1_RCVD_FREQ_SELf;

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_port_sel = L1_RCVD_BKUP_CLK_PORT_SELf;
        l1_rcvd_clk_rstn = L1_RCVD_CLK_BKUP_RSTNf;
        l1_rcvd_lnksts_sel = L1_RCVD_BKUP_LNKSTS_PORT_SELf;
        l1_rcvd_freq_sel = L1_RCVD_BKUP_FREQ_SELf;
    }

    if (!SOC_PORT_VALID(unit,lport))
    {
        return BCM_E_PARAM;
    }
    phy_port = SOC_INFO(unit).port_l2p_mapping[lport];

    if (!BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        !BCM_TIME_GH2_PHY_PORT_VALID(phy_port)) {
        return BCM_E_PARAM;
    }

    /* lane adjust for eagle and falcon */
    if (phy_port >= 58) {
        BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
            NULL, &phy_port));
    }

    /* Section 7.1.3, table 19, GH2 top design spec */
    if (phy_port > 1 && phy_port <= 29) {  /* Viper 2-25 + QTCE 26-29 */
        port_sel = phy_port + 6;
    } else if (phy_port >= 42 && phy_port <= 45) {  /* QTCE 42-45 */
        port_sel = phy_port - 6;
    } else if (phy_port >= 58) { /*Eagle and Falcon */
        port_sel = phy_port - 18;
    } else {
        return BCM_E_PARAM;
    }

    if (phy_port >= 26) { /*QTCE, Eagle, Falcon */
        /* Set TSCF SDM divisors */
        /* Set SDM stage 1 mode to bypass */

        sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                   (divisor->stage0_sdm_frac & 0xff));

        SOC_IF_ERROR_RETURN(soc_phyctrl_synce_clock_set(unit, divisor->index, divisor->stage0_mode, 0, sdm_val));
    }

    if (phy_port > 1 && phy_port <= 25) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, l1_rcvd_clk_ctrl, REG_PORT_ANY,
                                    l1_rcvd_freq_sel, divisor->stage0_sdm_whole));
    }
    /* serdes port configuration */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                                REG_PORT_ANY, l1_rcvd_port_sel, port_sel));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                                REG_PORT_ANY, l1_rcvd_lnksts_sel, port_sel));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, l1_rcvd_clk_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 1));
    return BCM_E_NONE;
}

#define BCM_TIME_GH2_PLL_VALID(x) ((x) < 2)
STATIC int
_bcm_esw_time_gh2_synce_clock_set_by_pll(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    return BCM_E_PARAM;
#if 0
    

    uint32 pll_index = divisor->index;
    int port_sel;

    if (!BCM_TIME_GH2_PLL_VALID(pll_index) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src)) {
        return BCM_E_PARAM;
    }

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PRI_CLK_PORT_SELf;

    soc_reg_t l1_rcvd_clk_ctrl = TOP_L1_RCVD_CLK_CONTROLr;
    soc_field_t l1_rcvd_clk_rstn = L1_RCVD_CLK_RSTNf;
    soc_field_t l1_rcvd_freq_sel = L1_RCVD_FREQ_SELf;

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_port_sel = L1_RCVD_BKUP_CLK_PORT_SELf;
        l1_rcvd_clk_rstn = L1_RCVD_CLK_BKUP_RSTNf;
        l1_rcvd_freq_sel = L1_RCVD_BKUP_FREQ_SELf;
    }

    /* TOP_MISC_CONTROL */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, l1_rcvd_clk_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 0));

    /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, l1_rcvd_clk_ctrl, REG_PORT_ANY,
                                        l1_rcvd_freq_sel, 0));
    port_sel = pll_index;
    /* TOP_MISC_CONTROL_2  - set pll port sel */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                        REG_PORT_ANY, l1_rcvd_port_sel, port_sel));

    /* TOP_MISC_CONTROL */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, l1_rcvd_clk_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 1));

    return BCM_E_NONE;
#endif
}

STATIC int
_bcm_esw_time_gh2_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, rval;
    int lport;
    uint32 sdm_val;
    uint32 phy_port = 0;
    uint32 mode0, mode1;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PRI_CLK_PORT_SELf;

    soc_reg_t l1_rcvd_clk_ctrl = TOP_L1_RCVD_CLK_CONTROLr;
    soc_field_t l1_rcvd_freq_sel = L1_RCVD_FREQ_SELf;

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_port_sel = L1_RCVD_BKUP_CLK_PORT_SELf;
    }

    /* input source port */
    SOC_IF_ERROR_RETURN( READ_TOP_MISC_CONTROL_2r(unit, &rval));

    val = soc_reg_field_get(unit, synce_ctrl_2, rval, l1_rcvd_port_sel);

    if (val >= 8 && val <= 35) { /* viper + qtce 26-29*/
        phy_port = val - 6;
    } else if (val >=36 && val <=39) {
        phy_port = val + 6;
    } else if (val >= BCMI_TIME_L1_RCVD_CLK_PORT_SEL_MAX) {
        /* Eagle or Falcon */
        phy_port = val + 18;
        _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
    }

    if (0 == phy_port) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_bcm_esw_time_gh2_synce_clock_get : Error !! Invalid port\n")));
        return BCM_E_PARAM;
    }

    if(phy_port > 1 && phy_port <= 25) {
        val = 0; rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_L1_RCVD_CLK_CONTROLr(unit, &rval));
        val = soc_reg_field_get(unit, l1_rcvd_clk_ctrl, rval, l1_rcvd_freq_sel);
        divisor->stage0_sdm_whole = val;
    }

    divisor->input_src = bcmTimeSynceInputSourceTypePort;
    divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];

    if (divisor->input_src == bcmTimeSynceInputSourceTypePort) {
        /* READ the SDM divisors from TSC */
        lport = divisor->index;
        if (!SOC_PORT_VALID(unit,lport)) {
            return BCM_E_PARAM;
        }
        if (phy_port >= 26) {
            SOC_IF_ERROR_RETURN(soc_phyctrl_synce_clock_get(unit, divisor->index, &mode0, &mode1, &sdm_val));
            /* stage0_whole */
            divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
            /* stage0_frac */
            divisor->stage0_sdm_frac = sdm_val & 0xff ;
            /* stage0_mode */
            divisor->stage0_mode = mode0;
            /* stage1_div  */
            divisor->stage1_div = mode1;
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_esw_time_gh2_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;

    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_gh2_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    default:
        rv = _bcm_esw_time_gh2_synce_clock_set_by_pll(unit, clk_src, divisor);
        break;
    }
    return rv;
}
#endif /* defined(BCM_GREYHOUND2_SUPPORT) */

#if defined(BCM_FIRELIGHT_SUPPORT)
#define BCM_TIME_FL_PHY_PORT_VALID(x) ((x) >= FL_MIN_PHY_PORT_NUM && \
                                        (x) <= FL_NUM_PHY_PORT)


/*
 * Port sel |         Clock source            | Phy port
 *   0      | LCPLL0 channel 0                |    x
 *   1      | N/A                             |    x
 *   2      | External phy recovered clock 0  |    x
 *   3      | External phy recovered clock 1  |    x
 *  7:4     | 4'h0                            |    x
 * 23:20    | 4'h0                            |    x
 *
 * PM4x10Q x3:
 *
 * 1) 48x2.5G:
 * 11:8     | pgw_ge0 lane [3, 2, 1, 0]       |   2-17
 * 15:12    | pgw_ge1 lane [3, 2, 1, 0]       |  18-33
 * 19:16    | pgw_ge2 lane [3, 2, 1, 0]       |  34-49
 *
 * 2) 12x10G:
 * 11:8     | pgw_xl0 lane [3, 2, 1, 0]       |  50-53
 * 15:12    | pgw_xl1 lane [3, 2, 1, 0]       |  54-57
 * 19:16    | pgw_xl2 lane [3, 2, 1, 0]       |  58-61
 *
 * PM4x25 x4:
 * 27:24    | pgw_cl0 lane [3, 2, 1, 0]       |  62-65
 * 31:28    | pgw_cl1 lane [3, 2, 1, 0]       |  66-69
 * 35:32    | pgw_cl2 lane [3, 2, 1, 0]       |  70-73
 * 39:36    | pgw_cl3 lane [3, 2, 1, 0]       |  74-77
 */

STATIC int
bcmi_esw_time_fl_synce_L1_rcvd_clk_port_get(int phy_port)
{
    if (phy_port >= 2 && phy_port <= 49) {
        /* PM4x10Q with Q mode, TSCE0_0, TSCE0_1, ..., TSCE2_3. */
        return ((phy_port - 2)/4 + 8);
    }
    else if (phy_port >= 50 && phy_port <= 61) {
        /* PM4x10Q with eth mode, TSCE0, TSCE1, TSCE2 */
        return (phy_port - 50 + 8);
    }
    else if (phy_port >= 62 && phy_port <= 77) {
        /* Falcon : TSCF0, TSCF1, TSCF2, TSCF3 */
        return (phy_port - 62 + 24);
    }
    else {
        return BCMI_TIME_PORT_SEL_INVALID;
    }
}

STATIC int
bcmi_esw_time_fl_phy_port_get(
    int unit,
    int port_sel)
{
    if (port_sel >= 8 && port_sel <= 19) {
        /* PM4x10Q with Q mode */
        int pport_q = (2 + (port_sel - 8) * 4);
        /* PM4x10Q with eth mode */
        int pport = (50 + (port_sel - 8));
        int lport = SOC_INFO(unit).port_p2l_mapping[pport_q];

        if (bcmi_esw_time_port_is_gmii_mode(unit, lport, NULL)) {
            return pport_q;
        } else {
            return pport;
        }
    }
    else if (port_sel >= 24 && port_sel <= 39) {
        /* Falcon : TSCF0, TSCF1, TSCF2, TSCF3 */
        return (port_sel - 24 + 62);
    }
    else {
        return BCMI_TIME_PHY_PORT_INVALID;
    }
}

STATIC int
bcmi_esw_time_fl_synce_clock_set_by_port(
    int unit,
    bcm_time_synce_clock_src_type_t clk_src,
    bcm_time_synce_divisor_setting_t *divisor)
{
    int lport;
    int phy_port;
    int port_sel = 0;
    uint16 sdm_val;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PRI_CLK_PORT_SELf;
    soc_field_t l1_rcvd_lnksts_sel = L1_RCVD_PRI_LNKSTS_PORT_SELf;

    soc_reg_t l1_rcvd_clk_ctrl = TOP_L1_RCVD_CLK_CONTROLr;
    soc_field_t l1_rcvd_clk_rstn = L1_RCVD_CLK_RSTNf;

    if (!BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src)||
        NULL == divisor) {
        return BCM_E_PARAM;
    }

    /* Assumes input index is logical port number */
    lport = divisor->index;
    phy_port = SOC_INFO(unit).port_l2p_mapping[lport];

    if (!SOC_PORT_VALID(unit, lport) ||
        (!BCM_TIME_FL_PHY_PORT_VALID(phy_port) && SOC_IS_FIRELIGHT(unit))) {
        return BCM_E_PARAM;
    }

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_clk_rstn = L1_RCVD_CLK_BKUP_RSTNf;
        l1_rcvd_lnksts_sel = L1_RCVD_BKUP_LNKSTS_PORT_SELf;
        l1_rcvd_port_sel = L1_RCVD_BKUP_CLK_PORT_SELf;
    }

    if (SOC_IS_FIRELIGHT(unit)) {
        port_sel = bcmi_esw_time_fl_synce_L1_rcvd_clk_port_get(phy_port);
    } else {
        return BCM_E_UNAVAIL;
    }

    if (BCMI_TIME_PORT_SEL_INVALID == port_sel &&
        BCM_TIME_FL_PHY_PORT_VALID(phy_port)) {
        /* Valid phy_port but SyncE is not supported. */
        return BCM_E_UNAVAIL;
    } else if (BCMI_TIME_PORT_SEL_INVALID == port_sel) {
        return BCM_E_INTERNAL;
    }

#ifdef PORTMOD_SUPPORT
    if ((SOC_PORT_USE_PORTCTRL(unit, lport))) {
        portmod_port_synce_clk_ctrl_t config;
        portmod_port_synce_clk_ctrl_t_init(unit, &config);
        /* Set TSCF SDM divisors */
        /* Set SDM stage 1 mode to bypass */
        sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
                   (divisor->stage0_sdm_frac & 0xff));

        config.stg0_mode = divisor->stage0_mode;
        config.stg1_mode = 0x0;
        config.sdm_val = sdm_val;
        SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_set(unit, lport, &config));
    }
#endif /* PORTMOD_SUPPORT */

    /* serdes port configuration */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                                REG_PORT_ANY, l1_rcvd_port_sel, port_sel));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                                REG_PORT_ANY, l1_rcvd_lnksts_sel, port_sel));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, l1_rcvd_clk_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 1));
    return BCM_E_NONE;
}

STATIC int
bcmi_esw_time_fl_synce_clock_set_by_pll(
    int unit,
    bcm_time_synce_clock_src_type_t clk_src,
    bcm_time_synce_divisor_setting_t *divisor)
{
    if (NULL == divisor) {
        return BCM_E_PARAM;
    }

    if (SOC_IS_FIRELIGHT(unit)) {
        /* Firelight doesn't support PLL clock */
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_esw_time_fl_synce_clock_get(
    int unit,
    bcm_time_synce_clock_src_type_t clk_src,
    bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, rval;
    int lport;
    uint32 sdm_val;
    uint32 phy_port = 0;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_port_sel = L1_RCVD_PRI_CLK_PORT_SELf;

    if (NULL == divisor) {
        return BCM_E_PARAM;
    }

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_port_sel = L1_RCVD_BKUP_CLK_PORT_SELf;
    }

    /* input source port */
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));

    val = soc_reg_field_get(unit, synce_ctrl_2, rval, l1_rcvd_port_sel);

    if (SOC_IS_FIRELIGHT(unit)) {
        phy_port = bcmi_esw_time_fl_phy_port_get(unit, val);
    } else {
        return BCM_E_UNAVAIL;
    }

    if (BCMI_TIME_PHY_PORT_INVALID == phy_port) {
        return BCM_E_INTERNAL;
    }

    divisor->input_src = bcmTimeSynceInputSourceTypePort;
    divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];

    if (divisor->input_src == bcmTimeSynceInputSourceTypePort) {
        /* READ the SDM divisors from TSC */
        lport = divisor->index;
        if (!SOC_PORT_VALID(unit, lport)) {
            return BCM_E_PARAM;
        }

#ifdef PORTMOD_SUPPORT
        if (SOC_PORT_USE_PORTCTRL(unit, lport)) {
            portmod_port_synce_clk_ctrl_t config;
            portmod_port_synce_clk_ctrl_t_init(unit, &config);

            SOC_IF_ERROR_RETURN(
                portmod_port_synce_clk_ctrl_get(unit, lport, &config));

            /* stage0_mode */
            divisor->stage0_mode = config.stg0_mode;
            sdm_val = config.sdm_val;
            /* stage0_whole */
            divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
            /* stage0_frac */
            divisor->stage0_sdm_frac = sdm_val & 0xff ;
            /* stage1_div  */
            divisor->stage1_div = config.stg1_mode;
        }
#endif /* PORTMOD_SUPPORT */
    }
    return BCM_E_NONE;
}

STATIC int
bcmi_esw_time_fl_synce_clock_set(
    int unit,
    bcm_time_synce_clock_src_type_t clk_src,
    bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;

    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = bcmi_esw_time_fl_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    default:
        rv = bcmi_esw_time_fl_synce_clock_set_by_pll(unit, clk_src, divisor);
        break;
    }
    return rv;
}
#endif /* defined(BCM_FIRELIGHT_SUPPORT) */

#if defined(BCM_MONTEREY_SUPPORT)
#define BCM_TIME_MO_PHY_PORT_VALID(x) ((x) >= 1 && (x) <= 64) /* Section 7, MO top design spec */

STATIC int
_bcm_esw_time_monterey_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port;
    int port_sel;
    uint16 sdm_val;
    portmod_port_synce_clk_ctrl_t config;

    soc_reg_t synce_ctrl = TOP_MISC_CONTROLr;
    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_clk_rstn = L1_RCVD_CLK_RSTNf;
    soc_field_t l1_rcvd_freq_sel_2 = L1_RCVD_FREQ_SELf;
    soc_field_t l1_rcvd_port_sel_2 = L1_CLK_RECOVERY_PRI_PORT_SELf;

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_clk_rstn = L1_RCVD_CLK_BKUP_RSTNf;
        l1_rcvd_freq_sel_2 = L1_RCVD_BKUP_FREQ_SELf;
        l1_rcvd_port_sel_2 = L1_CLK_RECOVERY_BKUP_PORT_SELf;
    }

    if (!SOC_PORT_VALID(unit,lport)) {
        return BCM_E_PARAM;
    }
    phy_port = SOC_INFO(unit).port_l2p_mapping[lport];

    if (!BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        !BCM_TIME_MO_PHY_PORT_VALID(phy_port)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
        NULL, &phy_port));

    port_sel = phy_port - 1;

    portmod_port_synce_clk_ctrl_t_init(unit, &config);

    /* TOP_MISC_CONTROL_2 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 0));

    /* TOP_MISC_CONTROL_2  - post divisors set to 0 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_freq_sel_2, 1));

    /* Set SDM stage 1 mode to bypass */
    sdm_val = ((divisor->stage0_sdm_whole & 0xff) << 8 |
               (divisor->stage0_sdm_frac & 0xff));

    config.stg0_mode = divisor->stage0_mode;
    config.stg1_mode = 0x0;
    config.sdm_val = sdm_val;
    SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_set(unit, lport, &config));

    /* serdes port configuration */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                                REG_PORT_ANY, l1_rcvd_port_sel_2, port_sel));

    /* TOP_MISC_CONTROL_2 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 1));
    return BCM_E_NONE;
}

#define BCM_TIME_MO_PLL_VALID(x) ((x) < 4)
STATIC int
_bcm_esw_time_monterey_synce_clock_set_by_pll(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 pll_index = divisor->index;
    int port_sel;

    soc_reg_t synce_ctrl = TOP_MISC_CONTROLr;
    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;

    soc_field_t l1_rcvd_clk_rstn = L1_RCVD_CLK_RSTNf;
    soc_field_t l1_rcvd_freq_sel_2 = L1_RCVD_FREQ_SELf;
    soc_field_t l1_rcvd_port_sel_2 = L1_CLK_RECOVERY_PRI_PORT_SELf;
    soc_field_t xg_rcvd_pri_sel_2 = XG_RCVD_PRI_SELf;

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_clk_rstn = L1_RCVD_CLK_BKUP_RSTNf;
        l1_rcvd_freq_sel_2 = L1_RCVD_BKUP_FREQ_SELf;
        l1_rcvd_port_sel_2 = L1_CLK_RECOVERY_BKUP_PORT_SELf;
        xg_rcvd_pri_sel_2 = XG_RCVD_BKUP_SELf;
    }

    if (!BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        !BCM_TIME_MO_PLL_VALID(pll_index)) {
        return BCM_E_PARAM;
    }

    port_sel = pll_index + 64;

    /* TOP_MISC_CONTROL_2 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 0));

    /* TOP_MISC_CONTROL_2  - post divisors set to 1 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2, REG_PORT_ANY,
                                l1_rcvd_freq_sel_2, 1));

    /* pll index-mux configuration */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                                REG_PORT_ANY, l1_rcvd_port_sel_2, port_sel));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl_2,
                                REG_PORT_ANY, xg_rcvd_pri_sel_2, pll_index+1));
    /* EGR_L1_CLK_RECOVERY_CTRL */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                REG_PORT_ANY, PRI_PORT_SELf, 0));

    /* TOP_MISC_CONTROL_2 */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, synce_ctrl, REG_PORT_ANY,
                                l1_rcvd_clk_rstn, 1));

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_esw_time_monterey_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes: This function takes care of monterey only
 */
STATIC int
_bcm_esw_time_monterey_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, val2, rval;
    int lport;
    uint32 sdm_val;
    uint32 phy_port;
    portmod_port_synce_clk_ctrl_t config;

    soc_reg_t synce_ctrl_2 = TOP_MISC_CONTROL_2r;
    soc_field_t l1_rcvd_port_sel_2 = L1_CLK_RECOVERY_PRI_PORT_SELf;
    soc_field_t xg_rcvd_pri_sel_2 = XG_RCVD_PRI_SELf;

    portmod_port_synce_clk_ctrl_t_init(unit, &config);

    if (clk_src == bcmTimeSynceClockSourceSecondary) {
        l1_rcvd_port_sel_2 = L1_CLK_RECOVERY_BKUP_PORT_SELf;
    }

    /* input source port */
    SOC_IF_ERROR_RETURN( READ_TOP_MISC_CONTROL_2r(unit, &rval));

    val = soc_reg_field_get(unit, synce_ctrl_2, rval, l1_rcvd_port_sel_2);
    val2 = soc_reg_field_get(unit, synce_ctrl_2, rval, xg_rcvd_pri_sel_2);

    if (val2 == 0) {
        if (val < 64) {
            phy_port = val + 1;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];

            lport = divisor->index;
            if (!SOC_PORT_VALID(unit,lport)) {
                return BCM_E_PARAM;
            }

            /* READ the SDM divisors from TSC */
            SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_get(unit, lport, &config));
            /* stage0_mode */
            divisor->stage0_mode = config.stg0_mode;
            sdm_val = config.sdm_val;
            /* stage0_whole */
            divisor->stage0_sdm_whole  = ((sdm_val & 0xff00) >> 8);
            /* stage0_frac */
            divisor->stage0_sdm_frac = sdm_val & 0xff;
            /* stage1_div  */
            divisor->stage1_div = config.stg1_mode;
        } else {
            return BCM_E_FAIL;
        }
    } else {
        divisor->input_src = bcmTimeSynceInputSourceTypePLL;
        divisor->index = val2 - 1;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_esw_time_monterey_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;

    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_monterey_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    case bcmTimeSynceInputSourceTypePLL:
        rv = _bcm_esw_time_monterey_synce_clock_set_by_pll(unit, clk_src, divisor);
        break;
    default:
        return BCM_E_PARAM;
    }
    return rv;
}

#endif /*defined(BCM_MONTEREY_SUPPORT)*/

#if defined(BCM_HURRICANE3_SUPPORT)
#define BCM_TIME_HR3_PORT_VALID(x) (((x)>=2 && (x)<=5) || ((x)>=18 && (x)<=21) || \
                                   ((x)>=34 && (x)<=41))

STATIC int
_bcm_esw_time_hurricane3_synce_clock_set_by_port(int unit,
                               bcm_time_synce_clock_src_type_t clk_src,
                               bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int port_sel=0;
    phymod_dispatch_type_t dispatch_type = phymodDispatchTypeInvalid;
    phy_ctrl_t *pc;
    int i, isSGMII=0;
    char *propval;

    if (!BCM_TIME_HR3_PORT_VALID(phy_port) ||
        !BCM_TIME_SYNCE_CLK_SRC_VALID(clk_src) ||
        (phy_port == -1)) {
        return BCM_E_PARAM;
    }

    if (!SOC_REG_IS_VALID(unit, TOP_MISC_CONTROL_2r) ||
        !SOC_REG_IS_VALID(unit, TOP_L1_RCVD_CLK_CONTROLr)) {
        return BCM_E_UNAVAIL;
    }

    if (!SOC_PORT_VALID(unit, lport)) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < 2; i++) {
        propval = soc_property_suffix_num_str_get(unit, i, spn_BCM5616X_INIT_PORT_CONFIG, "qtc");
        if (sal_strcmp(propval, "SGMII") == 0) {
            isSGMII=1;
        }
    }

    if ((phy_port>=2 && phy_port<=5) || (phy_port>=18 && phy_port<=21)) {
        if (isSGMII) {
            BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
                NULL, &phy_port));
            port_sel = phy_port + 22;
        } else {
            if (phy_port<=5) {
                port_sel = 2;
            } else {
                port_sel = 3;
            }
        }
    } else if (phy_port >= 34 && phy_port <=41) {
            BCM_IF_ERROR_RETURN(_bcm_esw_time_synce_phy_port_lane_adjust(unit, phy_port,
                NULL, &phy_port));
            port_sel = phy_port-30;
    } else {
        return BCM_E_PARAM;
    }

    pc = INT_PHY_SW_STATE(unit, lport);
    if (pc == NULL) {
        return BCM_E_FAIL;
    }
    dispatch_type = pc->phymod_ctrl.phy[0]->pm_phy.type;

    if ( clk_src == bcmTimeSynceClockSourcePrimary ) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_L1_RCVD_CLK_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r,
                                    REG_PORT_ANY, L1_RCVD_PRI_CLK_PORT_SELf, port_sel));

        switch (dispatch_type) {
#ifdef PHYMOD_TSCE_SUPPORT
            case phymodDispatchTypeTsce:
                /* TOP_MISC_CONTROL_2 */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                            L1_RCVD_FREQ_SELf, 0));
                    /* Turn off TSC div11 */
                BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, 0x0,
                    BCM_TIME_TSCE_MAIN0_SYNCE_CTL_STAGE0_REG));
                break;
#endif /* PHYMOD_TSCE_SUPPORT */
#ifdef PHYMOD_QTCE_SUPPORT
            case phymodDispatchTypeQtce:
                /* TOP_MISC_CONTROL_2 */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                            L1_RCVD_FREQ_SELf, 3));
                    /* Turn off TSC div11 */
                if (isSGMII) {
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, 0x0,
                        BCM_TIME_QTCE_MAIN0_SYNCE_CTL_STAGE0_REG));
                }
                break;
#endif
            default: break;
        }

        /* Enable L1 prim recov clk as valid,default it is invalid  */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_L1_RCVD_CLK_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 1));

    } else { /* Secondary/Backup SyncE source */
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_L1_RCVD_CLK_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 0));
        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r,
                                    REG_PORT_ANY, L1_RCVD_BKUP_CLK_PORT_SELf, port_sel));

        switch (dispatch_type) {
#ifdef PHYMOD_TSCE_SUPPORT
            case phymodDispatchTypeTsce:

            /* TOP_MISC_CONTROL_2 */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                            L1_RCVD_BKUP_FREQ_SELf, 0));
                    /* Turn off TSC div11 */
                BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, 0x0,
                    BCM_TIME_TSCE_MAIN0_SYNCE_CTL_STAGE0_REG));
                break;
#endif /* PHYMOD_TSCE_SUPPORT */
#ifdef PHYMOD_QTCE_SUPPORT
            case phymodDispatchTypeQtce:
                /* TOP_MISC_CONTROL_2 */
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                            L1_RCVD_BKUP_FREQ_SELf, 3));
                if (isSGMII) {
                    /* Turn off TSC div11 */
                    BCM_IF_ERROR_RETURN(_bcm_time_synce_phy_register_set(unit, lport, 0x0,
                        BCM_TIME_QTCE_MAIN0_SYNCE_CTL_STAGE0_REG));
                }
                break;
#endif
            default: break;
        }

        /* Enable L1 sec/bkp recov clk as valid,default it is invalid  */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_BKUP_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL_2 */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_L1_RCVD_CLK_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 1));
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_esw_time_hurricane3_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    int rv;

    switch(divisor->input_src) {
    case bcmTimeSynceInputSourceTypePort:
        rv = _bcm_esw_time_hurricane3_synce_clock_set_by_port(unit, clk_src, divisor);
        break;
    case bcmTimeSynceInputSourceTypePLL:
        /* rv = _bcm_esw_time_hurricane3_synce_clock_set_by_pll(unit, clk_src, divisor);*/
    default:
        return BCM_E_PARAM;
    }
    return rv;
}

STATIC int
_bcm_esw_time_hurricane3_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 mux_val, rval;
    uint32 phy_port;

    if (!SOC_REG_IS_VALID(unit, TOP_MISC_CONTROL_2r)){
        return BCM_E_UNAVAIL;
    }

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_2r(unit, &rval));
        mux_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                L1_RCVD_PRI_CLK_PORT_SELf);
        if ((mux_val >= 24 && mux_val <= 27) || (mux_val >=40 && mux_val <=43)) {
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            phy_port = mux_val - 22;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
            /* stage0_mode */
            divisor->stage0_mode = bcmTimeSynceModeBypass;
        } else if (mux_val >= 4 && mux_val <= 11) { /* source is Port */
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            phy_port = mux_val + 30;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
            /* stage0_mode */
            divisor->stage0_mode = bcmTimeSynceModeBypass;
        } else { /* source is PLL */
        }

        break;
    case bcmTimeSynceClockSourceSecondary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_TOP_MISC_CONTROL_2r(unit, &rval));
        mux_val = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, rval,
                                L1_RCVD_BKUP_CLK_PORT_SELf);
        if ((mux_val >= 24 && mux_val <= 27) || (mux_val >=40 && mux_val <=43)) {
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            phy_port = mux_val - 22;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
            /* stage0_mode */
            divisor->stage0_mode = bcmTimeSynceModeBypass;
        } else if (mux_val >= 4 && mux_val <= 11) { /* source is Port */
            /* source is Port */
            divisor->input_src = bcmTimeSynceInputSourceTypePort;
            phy_port = mux_val + 30;
            _bcm_esw_time_synce_phy_port_get(unit, phy_port, &phy_port);
            divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];
            /* stage0_mode */
            divisor->stage0_mode = bcmTimeSynceModeBypass;
        } else {
            /* source is PLL */
        }
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}
#endif /*defined(BCM_HURRICANE3_SUPPORT)*/

#if defined(BCM_TRIUMPH3_SUPPORT)
#define BCM_TIME_TR3_PORT_VALID(x) ((x) >= 1 && (x) <= 80 )
#define BCM_TIME_TR3_CLK_SRC_VALID(x) \
    (((x) == bcmTimeSynceClockSourcePrimary) || \
     ((x) == bcmTimeSynceClockSourceSecondary) )
/* physical port number based classification */
#define BCM_TIME_TR3_PHYPORT_UNICORE(x) ((x) >= 1 && (x) <= 52 )
#define BCM_TIME_TR3_PHYPORT_WARPCORE(x) ((x) >= 53 && (x) <= 80 )

/*
 * Function:
 *      _bcm_esw_time_tr3_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      input_src - (IN) input source type (PORT)
 *      index     - (IN) if input_src is PORT, specify  port number 1..80(physical port)
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_tr3_synce_clock_set(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    /* Assumes input index is logical port number */
    int lport = divisor->index;
    int phy_port = SOC_INFO(unit).port_l2p_mapping[lport];
    int port_sel;

    if (!BCM_TIME_TR3_PORT_VALID(phy_port) ||
        !BCM_TIME_TR3_CLK_SRC_VALID(clk_src) ||
        (phy_port == -1)) {
        return BCM_E_PARAM;
    }

    port_sel = phy_port - 1;
    if ( clk_src == bcmTimeSynceClockSourcePrimary ) {
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 0));
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_FREQ_SELf, divisor->stage0_mode));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, PRI_PORT_SELf, port_sel));

        /* Enable L1 prim recov clk as valid,default it is invalid  */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_RSTNf, 1));
    } else { /* Secondary/Backup SyncE source */
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
           (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 0));
        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_BKUP_FREQ_SELf, divisor->stage0_mode));
        /* EGR_L1_CLK_RECOVERY_CTRL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_L1_CLK_RECOVERY_CTRLr,
                                    REG_PORT_ANY, BKUP_PORT_SELf, port_sel));

        /* Enable L1 sec/bkp recov clk as valid,default it is invalid  */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_BKUP_VALIDf, 1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, 1));

        /* TOP_MISC_CONTROL */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_CLK_BKUP_RSTNf, 1));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_tr3_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
        divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_tr3_synce_clock_get(int unit,
                              bcm_time_synce_clock_src_type_t clk_src,
                              bcm_time_synce_divisor_setting_t *divisor)
{
    uint32 val, phy_port, rval;

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_EGR_L1_CLK_RECOVERY_CTRLr(unit, &rval));
        val = soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr, rval,
                                     PRI_PORT_SELf);
        phy_port = val+ 1;
        divisor->input_src = bcmTimeSynceInputSourceTypePort;
        divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROLr(unit, &rval));
        /* Freq div mode */
        divisor->stage0_mode = soc_reg_field_get(unit, TOP_MISC_CONTROLr,
                                                 rval, L1_RCVD_FREQ_SELf);
        break;
    case bcmTimeSynceClockSourceSecondary:
        /* input source */
        SOC_IF_ERROR_RETURN(
             READ_EGR_L1_CLK_RECOVERY_CTRLr(unit, &rval));
        val= soc_reg_field_get(unit, EGR_L1_CLK_RECOVERY_CTRLr, rval,
                                     BKUP_PORT_SELf);
        phy_port = val+ 1;
        divisor->input_src = bcmTimeSynceInputSourceTypePort;
        divisor->index = SOC_INFO(unit).port_p2l_mapping[phy_port];

        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROLr(unit, &rval));
        /* Freq div mode */
        divisor->stage0_mode = soc_reg_field_get(unit, TOP_MISC_CONTROLr,
                                                 rval, L1_RCVD_BKUP_FREQ_SELf);
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

#endif /* defined(BCM_TRIUMPH3_SUPPORT) */


/*
 * Function:
 *      bcm_esw_time_synce_clock_get
 * Purpose:
 *      Get current setting of syncE clock divisor
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      clk_src - (IN) clock source type (Primary, Secondary)
 *      divisor - (OUT) Divisor setting
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_time_synce_clock_get(int unit,
                             bcm_time_synce_clock_src_type_t clk_src,
                             bcm_time_synce_divisor_setting_t *divisor)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_time_synce_divisor_setting)) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (SOC_IS_TD2P_TT2P(unit) && !SOC_IS_TOMAHAWKX(unit) && !SOC_IS_APACHE(unit)) {
            rv = _bcm_esw_time_td2p_synce_clock_get(unit, clk_src, divisor);
        }
#endif

#ifdef BCM_HELIX4_SUPPORT
        if (SOC_IS_HELIX4(unit)) {
            rv = _bcm_esw_time_hx4_synce_clock_get(unit, clk_src, divisor);
        }
#endif

#ifdef BCM_GREYHOUND_SUPPORT
        if (SOC_IS_GREYHOUND(unit)) {
            rv = _bcm_esw_time_gh_synce_clock_get(unit, clk_src, divisor);
        }
#endif

    }

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
      rv = _bcm_esw_time_katana_synce_clock_get(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        rv = _bcm_esw_time_tomahawkx_synce_clock_get(unit, clk_src, divisor);
    }
#endif

#ifdef  BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
      rv = _bcm_esw_time_metrolite_synce_clock_get(unit, clk_src, divisor);
    }
#endif

#ifdef  BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
      rv = _bcm_esw_time_trident3_synce_clock_get(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {
        rv = _bcm_esw_time_apachex_synce_clock_get(unit, clk_src, divisor);
    }
#endif

#ifdef  BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
        rv = _bcm_esw_time_maverick2_synce_clock_get(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
        SOC_IS_FIREBOLT6(unit)) {
        rv = _bcm_esw_time_hx5_synce_clock_get(unit, clk_src, divisor);
    }
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_GREYHOUND2_SUPPORT
#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        rv = bcmi_esw_time_fl_synce_clock_get(unit, clk_src, divisor);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    if (SOC_IS_GREYHOUND2(unit)) {
        rv = _bcm_esw_time_gh2_synce_clock_get(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
        rv = _bcm_esw_time_monterey_synce_clock_get(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit) && !SOC_IS_HELIX4(unit)) {
            rv = _bcm_esw_time_tr3_synce_clock_get(unit, clk_src, divisor);
        }
#endif

    return rv;
}

/*
 * Function:
 *      bcm_esw_time_synce_clock_set
 * Purpose:
 *      Set syncE divisor setting
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      divisor   - (IN) divisor setting.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_esw_time_synce_clock_set(int unit,
                             bcm_time_synce_clock_src_type_t clk_src,
                             bcm_time_synce_divisor_setting_t *divisor)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_time_synce_divisor_setting)) {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (SOC_IS_TD2P_TT2P(unit) && !SOC_IS_TOMAHAWKX(unit) && !SOC_IS_APACHE(unit)) {
            rv = _bcm_esw_time_td2p_synce_clock_set(unit, clk_src, divisor);
        }
#endif

#ifdef BCM_GREYHOUND_SUPPORT
        if (SOC_IS_GREYHOUND(unit)) {
            rv = _bcm_esw_time_gh_synce_clock_set(unit, clk_src, divisor);
        }
#endif

#ifdef BCM_HELIX4_SUPPORT
        if (SOC_IS_HELIX4(unit)) {
            rv = _bcm_esw_time_hx4_synce_clock_set(unit, clk_src, divisor);
        }
#endif

    }
#ifdef  BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
      rv = _bcm_esw_time_katana_synce_clock_set(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
      rv = _bcm_esw_time_tomahawkx_synce_clock_set_by_port(unit, clk_src, divisor);
    }
#endif

#ifdef  BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
      rv = _bcm_esw_time_saber2_synce_clock_set(unit, clk_src, divisor);
    }
#endif

#ifdef  BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
      rv = _bcm_esw_time_metrolite_synce_clock_set(unit, clk_src, divisor);
    }
#endif

#ifdef  BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit)) {
      rv = _bcm_esw_time_trident3_synce_clock_set(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit)) {
        rv = _bcm_esw_time_apachex_synce_clock_set(unit, clk_src, divisor);
    }
#endif /* BCM_APACHE_SUPPORT */

#ifdef  BCM_MAVERICK2_SUPPORT
    if (SOC_IS_MAVERICK2(unit)) {
      rv = _bcm_esw_time_maverick2_synce_clock_set(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
        SOC_IS_FIREBOLT6(unit)) {
        rv = _bcm_esw_time_hx5_synce_clock_set(unit, clk_src, divisor);
    }
#endif /* BCM_HELIX5_SUPPORT */

#ifdef BCM_GREYHOUND2_SUPPORT
#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        rv = bcmi_esw_time_fl_synce_clock_set(unit, clk_src, divisor);
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    if (SOC_IS_GREYHOUND2(unit)) {
        rv = _bcm_esw_time_gh2_synce_clock_set(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
      rv = _bcm_esw_time_monterey_synce_clock_set(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
      rv = _bcm_esw_time_hurricane3_synce_clock_set(unit, clk_src, divisor);
    }
#endif

#ifdef BCM_ESMC_EXTDPLL_SUPPORT
    if (rv == BCM_E_NONE) {
        rv = bcm_extdpll_esmc_select_port_set(unit, divisor->index, clk_src);
        /* Unsquelch the CDR MUX output, if it was squelched earlier */
		rv = _bcm_esw_time_synce_clock_source_squelch_set(unit, clk_src, 0);
    }
#endif /* BCM_ESMC_EXTDPLL_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit) && !SOC_IS_HELIX4(unit)) {
            rv = _bcm_esw_time_tr3_synce_clock_set(unit, clk_src, divisor);
        }
#endif
    return rv;
}

/*
 * Function:
 *      _bcm_esw_time_synce_clock_source_squelch_get
 * Purpose:
 *      Get syncE clock source control option
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      clk_src - (IN) clock source
 *      squelch - (OUT) squelch value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_synce_clock_source_squelch_get(int unit,
                            bcm_time_synce_clock_src_type_t clk_src,
                            int *squelch)
{
    int rv = BCM_E_NONE;
    uint32 regval;

    COMPILER_REFERENCE(regval);
    COMPILER_REFERENCE(rv);

    if (!squelch) return rv;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_TOMAHAWK(unit)) {
        soc_reg_t synce_ctrl = TOP_SYNCE_CTRLr;
        soc_field_t l1_rcvd_sw_ovwr_valid = L1_RCVD_SW_OVWR_VALIDf;
        soc_field_t l1_rcvd_sw_ovwr_enable = L1_RCVD_SW_OVWR_ENf;

        if (SOC_IS_TOMAHAWK(unit)) {
            synce_ctrl = TOP_MISC_CONTROLr;
        }

        if (clk_src == bcmTimeSynceClockSourceSecondary) {
            l1_rcvd_sw_ovwr_valid = L1_RCVD_SW_OVWR_BKUP_VALIDf;
            l1_rcvd_sw_ovwr_enable = L1_RCVD_SW_OVWR_ENf;
        }
        soc_reg32_get(unit, synce_ctrl, REG_PORT_ANY, 0, &regval);

        /* check if software overwrite enabled or not */
        if(soc_reg_field_get(unit, synce_ctrl, regval, l1_rcvd_sw_ovwr_enable)) {
            /* check if we have squelch or supressed */
            *squelch = soc_reg_field_get(unit, synce_ctrl, regval, l1_rcvd_sw_ovwr_valid) ? 0 : 1;
        } else {
            *squelch = 0;
        }
        return rv;
    }
#endif

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            READ_TOP_MISC_CONTROL_1r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_1r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_SABER2_SUPPORT) */

#if defined(BCM_METROLITE_SUPPORT)
        if (SOC_IS_METROLITE(unit)) {
            READ_TOP_MISC_CONTROL_1r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_1r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_METROLITE_SUPPORT) */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TD2P_TT2P(unit)) {
            READ_TOP_MISC_CONTROLr(unit, &regval);
            *squelch = !soc_reg_field_get(unit, TOP_MISC_CONTROLr, regval,
                                            L1_RCVD_CLK_RSTNf);
        }
#endif

#if defined(BCM_GREYHOUND_SUPPORT)
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
            if (SOC_REG_IS_VALID(unit, TOP_MISC_CONTROL_2r)) {
                READ_TOP_MISC_CONTROL_2r(unit, &regval);
                *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
            } else {
                rv = BCM_E_UNAVAIL;
            }
        }
#endif /* defined(BCM_GREYHOUND_SUPPORT) */


#if defined(BCM_HELIX4_SUPPORT)
        if (SOC_IS_HELIX4(unit)) {
            READ_TOP_MISC_CONTROL_1r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_1r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_HELIX4_SUPPORT) */

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3(unit)) {
            READ_TOP_MISC_CONTROL_2r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) { /* Apache and Monterey */
            READ_TOP_MISC_CONTROLr(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROLr, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_APACHE_SUPPORT) */


#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            READ_TOP_MISC_CONTROLr(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROLr, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_TOMAHAWK2_SUPPORT) */

#if defined(BCM_MAVERICK2_SUPPORT)
        if (SOC_IS_MAVERICK2(unit)) {
            READ_TOP_MISC_CONTROL_2r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_MAVERICK2_SUPPORT) */

#if defined(BCM_HELIX5_SUPPORT)
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
            SOC_IS_FIREBOLT6(unit)) {
            READ_TOP_MISC_CONTROL_2r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_HELIX5_SUPPORT) */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit) || SOC_IS_FIRELIGHT(unit)) {
        READ_TOP_L1_RCVD_CLK_CONTROLr(unit, &regval);
        *squelch = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CONTROLr, regval,
                                           L1_RCVD_SW_OVWR_ENf);
    }
#endif /* defined(BCM_GREYHOUND2_SUPPORT) */
        break;

    case bcmTimeSynceClockSourceSecondary:
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            READ_TOP_MISC_CONTROL_1r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_1r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_SABER2_SUPPORT) */

#if defined(BCM_METROLITE_SUPPORT)
        if (SOC_IS_METROLITE(unit)) {
            READ_TOP_MISC_CONTROL_1r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_1r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_METROLITE_SUPPORT) */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TD2P_TT2P(unit) && !SOC_IS_APACHE(unit)) {
            READ_TOP_MISC_CONTROLr(unit, &regval);
            *squelch = !soc_reg_field_get(unit, TOP_MISC_CONTROLr, regval,
                                            L1_RCVD_CLK_BKUP_RSTNf);
        }
#endif

#if defined(BCM_GREYHOUND_SUPPORT)
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
            if (SOC_REG_IS_VALID(unit, TOP_MISC_CONTROL_2r)) {
                READ_TOP_MISC_CONTROL_2r(unit, &regval);
                *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, regval,
                                                L1_RCVD_SW_OVWR_ENf);
            } else {
                rv = BCM_E_UNAVAIL;
            }
        }
#endif /* defined(BCM_HELIX4_SUPPORT) */

#if defined(BCM_HELIX4_SUPPORT)
        if (SOC_IS_HELIX4(unit)) {
            READ_TOP_MISC_CONTROL_1r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_1r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_HELIX4_SUPPORT) */

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3(unit)) {
            READ_TOP_MISC_CONTROL_2r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            READ_TOP_MISC_CONTROLr(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROLr, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_APACHE_SUPPORT) */

#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            READ_TOP_MISC_CONTROLr(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROLr, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_TOMAHAWK2_SUPPORT) */

#if defined(BCM_MAVERICK2_SUPPORT)
        if (SOC_IS_MAVERICK2(unit)) {
            READ_TOP_MISC_CONTROL_2r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_MAVERICK2_SUPPORT) */

#if defined(BCM_HELIX5_SUPPORT)
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
            SOC_IS_FIREBOLT6(unit)) {
            READ_TOP_MISC_CONTROL_2r(unit, &regval);
            *squelch = soc_reg_field_get(unit, TOP_MISC_CONTROL_2r, regval,
                                            L1_RCVD_SW_OVWR_ENf);
        }
#endif /* defined(BCM_HELIX5_SUPPORT) */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit) || SOC_IS_FIRELIGHT(unit)) {
        READ_TOP_L1_RCVD_CLK_CONTROLr(unit, &regval);
        *squelch = soc_reg_field_get(unit, TOP_L1_RCVD_CLK_CONTROLr, regval,
                                           L1_RCVD_SW_OVWR_ENf);
    }
#endif /* defined(BCM_GREYHOUND2_SUPPORT) */
        break;
    default:
        return BCM_E_PARAM;
    }

    return rv;
}


/*
 * Function:
 *      _bcm_esw_time_synce_clock_source_frequency_get
 * Purpose:
 *      Get syncE clock source squelch option
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      frequency - (OUT) synce clock source frequency.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_synce_clock_source_frequency_get(int unit,
                             bcm_time_synce_clock_source_config_t *clk_src_config,
                             int *frequency)

{
    int rv = BCM_E_NONE;
    bcm_time_synce_divisor_setting_t div_out;
    bcm_time_synce_divisor_setting_t_init(&div_out);

    COMPILER_REFERENCE(rv);
    rv = bcm_esw_time_synce_clock_get(unit, clk_src_config->clk_src, &div_out);
    if (rv != BCM_E_UNAVAIL) {
        if (div_out.input_src == bcmTimeSynceInputSourceTypePort) {
            clk_src_config->port = div_out.index;
        } else {
            clk_src_config->pll_index = div_out.index;
        }
    }
    if (IS_MANAGEMENT_PORT(unit,  clk_src_config->port) && clk_src_config->input_src == bcmTimeSynceInputSourceTypePort) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(SOC_IS_TD2P_TT2P(unit) && !SOC_IS_TOMAHAWK(unit) && !SOC_IS_APACHE(unit)) {
        if(div_out.stage0_mode == bcmTimeSynceModeSDMFracDiv && div_out.stage0_sdm_whole == 20 &&
            div_out.stage0_sdm_frac  == 160) {
               *frequency = bcmTimeSyncE25MHz;
        } else if  (div_out.stage0_mode == bcmTimeSynceModeBypass && div_out.stage1_div == bcmTimeSynceStage1Div11) {
            if (div_out.stage0_sdm_whole == 23) {
                *frequency = bcmTimeSyncE23MHz;
            } else if (div_out.stage0_sdm_whole == 28) {
                *frequency = bcmTimeSyncE28MHz;
            } else if (div_out.stage0_sdm_whole == 46) {
                *frequency = bcmTimeSyncE46MHz;
            } else {
                return BCM_E_PARAM;
            }
       } else {
           return BCM_E_PARAM;
       }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_GREYHOUND(unit)) {
         /* Helix4 :
                    stage0_mode=0 -> pass-through mode
                    stage0_mode=1 -> Div_by_5(25MHz) mode
                    stage0_mode=2 -> Div_by_10(31.25MHz) mode
                    stage0_mode=3 -> Div_by_2(156.25MHz) mode
                */
         *frequency = div_out.stage0_mode;
    }
#endif /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)) {
        rv = _bcm_esw_time_hurricane3_synce_clock_get(unit, clk_src_config->clk_src, &div_out);
        if (div_out.input_src == bcmTimeSynceInputSourceTypePort) {
            phymod_dispatch_type_t dispatch_type = phymodDispatchTypeInvalid;
            phy_ctrl_t *pc;
            pc = INT_PHY_SW_STATE(unit, clk_src_config->port);
            if (pc == NULL) {
                dispatch_type = phymodDispatchTypeQtce;
            } else {
                dispatch_type = pc->phymod_ctrl.phy[0]->pm_phy.type;
            }
#ifdef PHYMOD_QTCE_SUPPORT
            if (dispatch_type == phymodDispatchTypeQtce) {
                *frequency = bcmTimeSyncE156MHz;
            }
#endif
#ifdef PHYMOD_TSCE_SUPPORT
            if (dispatch_type == phymodDispatchTypeTsce) {
                *frequency = bcmTimeSyncE515MHz;
            }
#endif
        }
    }
#endif /* defined(BCM_HURRICANE3_SUPPORT) */

#if defined(BCM_HELIX4_SUPPORT)
    if (SOC_IS_HELIX4(unit)) {
        int lport = clk_src_config->port;
        int phy_port = SOC_INFO(unit).port_l2p_mapping[clk_src_config->port];
        int port_speed = 0;

        if (!SOC_PORT_VALID(unit,lport)) {
            return BCM_E_FAIL;
        }

        /* Helix4 :
                   stage0_mode=0 -> pass-through mode
                   stage0_mode=1 -> Div_by_5 mode
                   stage0_mode=2 -> Div_by_10 mode
                   stage0_mode=3 -> Div_by_2 mode
        */

        phy_port = SOC_INFO(unit).port_l2p_mapping[clk_src_config->port];
        BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, clk_src_config->port, &port_speed));
        *frequency = -1;
        switch (port_speed) {
            case 10:
            case 100:
            case 1000:
                /* 1 G ports */
                if (BCM_TIME_HX4_PHYPORT_UNICORE(phy_port)) {
                    if (div_out.stage0_mode == 0) {
                        *frequency = bcmTimeSyncE125MHz;
                    } else if (div_out.stage0_mode == 1) {
                        *frequency = bcmTimeSyncE25MHz;
                    }
                } else if (BCM_TIME_HX4_PHYPORT_WARPCORE(phy_port)) {
                    if (div_out.stage0_mode == 1) {
                        *frequency = bcmTimeSyncE31MHz;
                    }
                }
                break;
            case 2500:
                /* 2.5 G ports */
                if (BCM_TIME_HX4_PHYPORT_UNICORE(phy_port)) {
                    if (div_out.stage0_mode == 2) {
                        *frequency = bcmTimeSyncE31MHz;
                    }
                }
               break;
            case 10000:
                /* 10 G ports */
                if (BCM_TIME_HX4_PHYPORT_UNICORE(phy_port)) {
                    if (div_out.stage0_mode == 2) {
                        *frequency = bcmTimeSyncE31MHz;
                    }
                } else if (BCM_TIME_HX4_PHYPORT_WARPCORE(phy_port)) {
                    if (div_out.stage0_mode == 1) {
                        *frequency = bcmTimeSyncE31MHz;
                    }
                }
               break;
            default:
                return BCM_E_FAIL;
        }
    }
#endif /* BCM_HELIX4_SUPPORT */

#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(unit)) {
      /* Katana */
      *frequency = bcmTimeSyncE125MHz;
    }
#endif /* BCM_KATANA_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        if (div_out.input_src == bcmTimeSynceInputSourceTypePort &&
            SOC_PORT_USE_PORTCTRL(unit, clk_src_config->port)) {
            if ((div_out.stage0_mode == bcmTimeSynceModeSDMFracDiv) &&
                (div_out.stage1_div == bcmTimeSynceStage1Div1)) {
                int port_speed = 0; /* speed specified in mbps*/
                uint32 sdm_divisor = 0;
                phymod_dispatch_type_t dispatch_type =
                    phymodDispatchTypeInvalid;
                bcmi_time_port_mode_t port_mode =
                    bcmi_time_port_mode_invalid;

                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_speed_get
                        (unit, clk_src_config->port, &port_speed));

                sdm_divisor = (div_out.stage0_sdm_whole << 8) |
                               div_out.stage0_sdm_frac;

                dispatch_type =
                    _bcm_time_synce_tsc_phymod_dispatch_type_get
                        (unit, clk_src_config->port);

                bcmi_esw_time_port_mode_get
                    (unit, dispatch_type, clk_src_config->port, &port_mode);

                switch(dispatch_type) {
#ifdef PHYMOD_TSCE16_SUPPORT
                    case phymodDispatchTypeTsce16:
                        /* Eagle port with Ethernet mode. */
                        if (!IS_HG_PORT(unit, clk_src_config->port)) {
                            /* Regular port */
                            if (sdm_divisor == 0x14a0) {
                                *frequency = bcmTimeSyncE25MHz;
                            }
                        } else {
                            /* Higig port */
                            if (sdm_divisor == 0x15e0) {
                                *frequency = bcmTimeSyncE25MHz;
                            }
                        }
                        break;
#endif /* PHYMOD_TSCE16_SUPPORT */
#ifdef PHYMOD_QTCE16_SUPPORT
                        case phymodDispatchTypeQtce16:
                            /* Eagle port with QTC mode. */
                            if (sdm_divisor == 0x14a0 &&
                                (port_speed == 1000 ||
                                 port_speed == 2500)) {
                                /* USXGMII */
                                /* 1G/2.5G - VCO 10.3125, 0x14A0 */
                                *frequency = bcmTimeSyncE25MHz;
                            } else if (sdm_divisor == 0x1400 &&
                                       (port_speed == 10 ||
                                        port_speed == 100 ||
                                        port_speed == 1000)) {
                                /* QSGMII */
                                /* 10/100/1000M - VCO 10, 0x1400 */
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
#endif /* PHYMOD_QTCE16_SUPPORT */
#ifdef PHYMOD_TSCF16_GEN3_SUPPORT
                    case phymodDispatchTypeTscf16_gen3:
                        if (port_mode == bcmi_time_port_mode_single) {
                            switch (port_speed) {
                                case 106000:
                                    /* 106G HG - VCO 27.34375, 0x1B58 */
                                    if (sdm_divisor == 0x1B58) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 100000:
                                    /* 100G - VCO 25.78125, 0x19C8 */
                                    if (sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 42000:
                                    /* 42G HG - VCO 21.875, 0xAF0 */
                                    if (sdm_divisor == 0xAF0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 40000:
                                    /* 40G - VCO 20.625, 0xA50 */
                                    if (sdm_divisor == 0xA50) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                default:
                                    return BCM_E_PARAM;
                                break;
                            }
                        } else if (port_mode == bcmi_time_port_mode_dual) {
                            switch (port_speed) {
                                case 53000:
                                    /* 53G HG - VCO 27.34375, 0x1B58 */
                                    if (sdm_divisor == 0x1B58) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 50000:
                                    /* 50G - VCO 25.78125, 0x19C8 */
                                    if (sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 42000:
                                    /* 42G HG - VCO 21.875, 15E0 */
                                    if (sdm_divisor == 0x15E0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 40000:
                                    /* 40G - VCO 20.625, 0x14A0 */
                                    if (sdm_divisor == 0x14A0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 21000:
                                    /* 21G - VCO 21.875, 0xAF0 */
                                    if (sdm_divisor == 0xAF0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 20000:
                                    /* 20G - VCO 20.625, 0xA50 */
                                    if (sdm_divisor == 0xA50) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                default:
                                    return BCM_E_PARAM;
                                break;
                            }
                        } else if (port_mode == bcmi_time_port_mode_quad) {
                            switch (port_speed) {
                                case 27000:
                                    /* 27G HG - VCO 27.34375, 0x1B58 */
                                    if (sdm_divisor == 0x1B58) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 25000:
                                    /* 25G - VCO 25.78125, 0x19C8 */
                                    if (sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 11000:
                                    /* 11G HG - VCO 27.34375, 0xAF0 */
                                    if (sdm_divisor == 0xAF0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 10000:
                                    /* 10G - VCO 25.78125, 0xA50 */
                                    if (sdm_divisor == 0xA50) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 1000:
                                    /* 1G - VCO 20.625, 0x14A0 */
                                    /* 1G - VCO 25.78125, 0x19C8 */
                                    if (sdm_divisor == 0x14A0 ||
                                        sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                default:
                                    return BCM_E_PARAM;
                                break;
                            }
                        } else if (port_mode == bcmi_time_port_mode_tri) {
                            switch (port_speed) {
                                case 50000:
                                case 25000:
                                    /* 25G/50G - VCO 27.34375, 0x19C8 */
                                    if (sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 40000:
                                    /* 40G - VCO 20.625, 0x14A0 */
                                    if (sdm_divisor == 0x14A0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 20000:
                                case 10000:
                                    /* 10G/20G - VCO 20.625, 0xA50 */
                                    if (sdm_divisor == 0xA50) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                default:
                                    return BCM_E_PARAM;
                                break;
                            }
                        }
                    break;
#endif /*PHYMOD_TSCF16_GEN3_SUPPORT  */
                        default: break;
                    } /* switch(dispatch_type) */
                }
        } else {
            return BCM_E_PARAM;
        }
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    if (SOC_IS_GREYHOUND2(unit)) {
      /* GH2 */
        if (div_out.input_src == bcmTimeSynceInputSourceTypePort) {
            phy_ctrl_t *pc;
            phymod_dispatch_type_t dispatch_type = phymodDispatchTypeInvalid;

            pc = INT_PHY_SW_STATE(unit, clk_src_config->port);
            if (pc == NULL) {
                return BCM_E_FAIL;
            }
            dispatch_type = pc->phymod_ctrl.phy[0]->pm_phy.type;
#ifdef PHYMOD_QTCE_SUPPORT
            if (dispatch_type == phymodDispatchTypeQtce) {
                *frequency = bcmTimeSyncE25MHz;
            }
#endif
            if ((div_out.stage0_mode == bcmTimeSynceModeSDMFracDiv) &&
                (div_out.stage1_div == bcmTimeSynceStage1Div1)) {
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
                if (dispatch_type == phymodDispatchTypeTsce_dpll ) {
                    *frequency = bcmTimeSyncE25MHz;
                }
#endif
#ifdef PHYMOD_TSCF_SUPPORT
                if (dispatch_type == phymodDispatchTypeTscf ) {
                    *frequency = bcmTimeSyncE25MHz;
                }
#endif
            } else {
#ifdef PHYMOD_VIPER_SUPPORT
                if (dispatch_type == phymodDispatchTypeViper) {
                    if (div_out.stage0_sdm_whole == 1) {
                        *frequency = bcmTimeSyncE25MHz;
                    } else if (div_out.stage0_sdm_whole == 3) {
                        *frequency = bcmTimeSyncE156MHz;
                    } else {
                        *frequency = bcmTimeSyncE125MHz;
                    }
                }
#endif
            }
        } else {
            return BCM_E_PARAM;
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
      rv = _bcm_esw_time_saber2_synce_clock_get(unit, clk_src_config->clk_src, &div_out);
      if (rv == BCM_E_NONE) {
          if (div_out.input_src == bcmTimeSynceInputSourceTypePort) {
              clk_src_config->port = div_out.index;
              if (div_out.stage0_mode == bcmTimeSynceModeBypass)
                *frequency = bcmTimeSyncE125MHz;
              else
                *frequency = bcmTimeSyncE25MHz;
          } else {
              clk_src_config->pll_index = div_out.index;
              *frequency = bcmTimeSyncE125MHz;
          }
      }
    }
#endif /* BCM_SABER2_SUPPORT */

#if defined(BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
      rv = _bcm_esw_time_metrolite_synce_clock_get(unit, clk_src_config->clk_src, &div_out);
      if (rv == BCM_E_NONE) {
          if (div_out.input_src == bcmTimeSynceInputSourceTypePort) {
              clk_src_config->port = div_out.index;
              if (div_out.stage0_mode == bcmTimeSynceModeBypass)
                *frequency = bcmTimeSyncE125MHz;
              else
                *frequency = bcmTimeSyncE25MHz;
          } else {
              clk_src_config->pll_index = div_out.index;
              *frequency = bcmTimeSyncE125MHz;
          }
      }
    }
#endif /* BCM_METROLITE_SUPPORT */

#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5(unit) || SOC_IS_FIREBOLT6(unit)) {
        if((clk_src_config->input_src == bcmTimeSynceInputSourceTypePort) &&
           (bcmi_esw_time_port_is_gmii_mode(unit,
                                            clk_src_config->port, NULL))) {
            return BCM_E_CONFIG;
        }
        /* HELIX5 or HR4 or FL6 */
        if (div_out.input_src == bcmTimeSynceInputSourceTypePLL) {
            *frequency = bcmTimeSyncE125MHz;
            clk_src_config->input_src = div_out.input_src;
        }
    }
#endif /* BCM_HELIX5_SUPPORT*/

#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        if (div_out.input_src == bcmTimeSynceInputSourceTypePLL) {
            *frequency = bcmTimeSyncE125MHz;
            clk_src_config->pll_index = div_out.index;
            clk_src_config->input_src = div_out.input_src;
        }
    }
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || \
    defined(BCM_HELIX5_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)

    if (div_out.input_src == bcmTimeSynceInputSourceTypePort) {

    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3(unit) ||
        SOC_IS_APACHE(unit) || SOC_IS_MAVERICK2(unit) ||
        SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
        SOC_IS_FIREBOLT6(unit) || SOC_IS_MONTEREY(unit)) {
        /* tomahawk  & TD3 */
        if ((div_out.stage0_mode == bcmTimeSynceModeSDMFracDiv) &&
                (div_out.stage1_div == bcmTimeSynceStage1Div1)) {

            int port_speed = 0; /* speed specified in mbps*/
            uint32 sdm_divisor = 0;
            phymod_dispatch_type_t dispatch_type = phymodDispatchTypeInvalid;
            bcmi_time_port_mode_t port_mode = bcmi_time_port_mode_invalid;

            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, clk_src_config->port, &port_speed));
            sdm_divisor = (div_out.stage0_sdm_whole <<8) | div_out.stage0_sdm_frac;

            dispatch_type = _bcm_time_synce_tsc_phymod_dispatch_type_get(unit, clk_src_config->port);

            bcmi_esw_time_port_mode_get
                (unit, dispatch_type, clk_src_config->port, &port_mode);
            switch(dispatch_type) {
#ifdef PHYMOD_TSCE_SUPPORT
                case phymodDispatchTypeTsce:
#endif
#ifdef PHYMOD_TSCE16_SUPPORT
                case phymodDispatchTypeTsce16:
#endif
#if defined (PHYMOD_TSCE_SUPPORT) || defined (PHYMOD_TSCE16_SUPPORT)
                    if (!IS_HG_PORT(unit, clk_src_config->port)) {
                        /* regular port */
                        if (sdm_divisor == 0x14a0) {
                            *frequency = bcmTimeSyncE25MHz;
                        }
                    } else {
                        /* hi gig port */
                        if (sdm_divisor == 0x15e0) {
                            *frequency = bcmTimeSyncE25MHz;
                        }
                    }
                    break;
#endif
#ifdef PHYMOD_TSCF_SUPPORT
                case phymodDispatchTypeTscf:
                    switch (port_speed) {
                        case 100:
                            /* 1G ports  - VCO 25.78125 */
                            if (sdm_divisor == 0x19c8) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                        case 10000:
                        case 40000:
                            /* 10G or 40G ports -  VCO 20.625 */
                            if (sdm_divisor == 0xa50) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                         case 25000:
                         case 100000:
                            /* 100G ports - VCO 25.78125*/
                            if (sdm_divisor == 0x19c8) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                         case 11000:
                            /* 11G HG ports -VCO 21.875 */
                            if (sdm_divisor == 0xaf0) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                          case 106000:
                            /* 106G HG ports -VCO 21.875 */
                            if (sdm_divisor == 0x1b58) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                          default:
                            return BCM_E_PARAM;
                            break;
                    }
                    break;
#endif /* PHYMOD_TSCF_SUPPORT */
#ifdef PHYMOD_TSCF16_GEN3_SUPPORT
                    case phymodDispatchTypeTscf16_gen3:
                        if (port_mode == bcmi_time_port_mode_single) {
                            switch (port_speed) {
                                case 106000:
                                    /* 106G HG - VCO 27.34375, 0x1B58 */
                                    if (sdm_divisor == 0x1B58) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 100000:
                                    /* 100G - VCO 25.78125, 0x19C8 */
                                    if (sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 42000:
                                    /* 42G HG - VCO 21.875, 0xAF0 */
                                    if (sdm_divisor == 0xAF0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 40000:
                                    /* 40G - VCO 20.625, 0xA50 */
                                    if (sdm_divisor == 0xA50) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                default:
                                    return BCM_E_PARAM;
                                break;
                            }
                        } else if (port_mode == bcmi_time_port_mode_dual) {
                            switch (port_speed) {
                                case 53000:
                                    /* 53G HG - VCO 27.34375, 0x1B58 */
                                    if (sdm_divisor == 0x1B58) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 50000:
                                    /* 50G - VCO 25.78125, 0x19C8 */
                                    if (sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 42000:
                                    /* 42G HG - VCO 21.875, 15E0 */
                                    if (sdm_divisor == 0x15E0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 40000:
                                    /* 40G - VCO 20.625, 0x14A0 */
                                    if (sdm_divisor == 0x14A0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 21000:
                                    /* 21G - VCO 21.875, 0xAF0 */
                                    if (sdm_divisor == 0xAF0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 20000:
                                    /* 20G - VCO 20.625, 0xA50 */
                                    if (sdm_divisor == 0xA50) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                default:
                                    return BCM_E_PARAM;
                                break;
                            }
                        } else if (port_mode == bcmi_time_port_mode_quad) {
                            switch (port_speed) {
                                case 27000:
                                    /* 27G HG - VCO 27.34375, 0x1B58 */
                                    if (sdm_divisor == 0x1B58) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 25000:
                                    /* 25G - VCO 25.78125, 0x19C8 */
                                    if (sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 11000:
                                    /* 11G HG - VCO 27.34375, 0xAF0 */
                                    if (sdm_divisor == 0xAF0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 10000:
                                    /* 10G - VCO 25.78125, 0xA50 */
                                    if (sdm_divisor == 0xA50) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 1000:
                                    /* 1G - VCO 20.625, 0x14A0 */
                                    /* 1G - VCO 25.78125, 0x19C8 */
                                    if (sdm_divisor == 0x14A0 ||
                                        sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                default:
                                    return BCM_E_PARAM;
                                break;
                            }
                        } else if (port_mode == bcmi_time_port_mode_tri) {
                            switch (port_speed) {
                                case 50000:
                                case 25000:
                                    /* 25G/50G - VCO 27.34375, 0x19C8 */
                                    if (sdm_divisor == 0x19C8) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 40000:
                                    /* 40G - VCO 20.625, 0x14A0 */
                                    if (sdm_divisor == 0x14A0) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                case 20000:
                                case 10000:
                                    /* 10G/20G - VCO 20.625, 0xA50 */
                                    if (sdm_divisor == 0xA50) {
                                        *frequency = bcmTimeSyncE25MHz;
                                    } else {
                                        return BCM_E_PARAM;
                                    }
                                    break;
                                default:
                                    return BCM_E_PARAM;
                                break;
                            }
                        }
                    break;
#endif /*PHYMOD_TSCF16_GEN3_SUPPORT  */

#ifdef PHYMOD_TSCF16_SUPPORT
                case phymodDispatchTypeTscf16:
                    switch (port_speed) {
                        case 100:
                            /* 1G ports  - VCO 25.78125 */
                            if (sdm_divisor == 0x19c8) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                        case 10000:
                        case 40000:
                            /* 10G or 40G ports -  VCO 20.625 */
                            if (sdm_divisor == 0xa50) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                         case 25000:
                         case 100000:
                            /* 100G ports - VCO 25.78125*/
                            if (sdm_divisor == 0x19c8) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                         case 11000:
                            /* 11G HG ports -VCO 21.875 */
                            if (sdm_divisor == 0xaf0) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                          case 106000:
                            /* 106G HG ports -VCO 21.875 */
                            if (sdm_divisor == 0x1b58) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
                          default:
                            return BCM_E_PARAM;
                            break;
                    }
                break;
#endif /* PHYMOD_TSCF16_SUPPORT */
#ifdef PHYMOD_TSCBH_SUPPORT
                case phymodDispatchTypeTscbh:
#ifdef PORTMOD_SUPPORT
                    if (SOC_PORT_USE_PORTCTRL(unit, clk_src_config->port)) {
                        portmod_port_synce_clk_ctrl_t config;
                        portmod_port_synce_clk_ctrl_t_init(unit, &config);

                        SOC_IF_ERROR_RETURN(portmod_port_synce_clk_ctrl_get(unit, clk_src_config->port, &config));
                        if ((config.stg0_mode == 0x2) && (config.stg1_mode == 0x0)) {
                            *frequency = bcmTimeSyncE25MHz;
                        }
                    }
#endif
                break;
#endif /* PHYMOD_TSCBH_SUPPORT */
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
                case phymodDispatchTypeTsce_dpll:
                    switch (port_speed) {
                        case 1000:
                        case 10000:
                        case 20000:
                        case 40000:
                            if (sdm_divisor == 0x14a0) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else if (sdm_divisor == 0x4200) {
                                *frequency = bcmTimeSyncE7MHz;
                            } else if (sdm_divisor == 0x2940) {
                                *frequency = bcmTimeSyncE12MHz;
                            } else if (sdm_divisor == 0x1080) {
                                *frequency = bcmTimeSyncE31MHz;
                            } else if (sdm_divisor == 0xDC0) {
                                *frequency = bcmTimeSyncE37MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                        break;
                        default:
                            return BCM_E_PARAM;
                    }
                break;
#endif /* PHYMOD_TSCE_DPLL_SUPPORT */
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
                case phymodDispatchTypeTscf_gen3:
                    switch (port_speed) {
                        case 1000:
                            if (sdm_divisor == 0x14A0) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else if (sdm_divisor == 0x4200) {
                                *frequency = bcmTimeSyncE7MHz;
                            } else if (sdm_divisor == 0x2940) {
                                *frequency = bcmTimeSyncE12MHz;
                            } else if (sdm_divisor == 0x1080) {
                                *frequency = bcmTimeSyncE31MHz;
                            } else if (sdm_divisor == 0xDC0) {
                                *frequency = bcmTimeSyncE37MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                        break;
                        case 10000:
                        case 20000:
                        case 40000:
                            if (sdm_divisor == 0xA50) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else if (sdm_divisor == 0x2100) {
                                *frequency = bcmTimeSyncE7MHz;
                            } else if (sdm_divisor == 0x14A0) {
                                *frequency = bcmTimeSyncE12MHz;
                            } else if (sdm_divisor == 0x840) {
                                *frequency = bcmTimeSyncE31MHz;
                            } else if (sdm_divisor == 0x6E0) {
                                *frequency = bcmTimeSyncE37MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                        break;
                        case 25000:
                        case 50000:
                        case 100000:
                            if (sdm_divisor == 0x19C8) {
                                *frequency = bcmTimeSyncE25MHz;
                            } else if (sdm_divisor == 0x5280) {
                                *frequency = bcmTimeSyncE7MHz;
                            } else if (sdm_divisor == 0x3390) {
                                *frequency = bcmTimeSyncE12MHz;
                            } else if (sdm_divisor == 0x14A0) {
                                *frequency = bcmTimeSyncE31MHz;
                            } else if (sdm_divisor == 0x1130) {
                                *frequency = bcmTimeSyncE37MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                        break;
                        default:
                            cli_out("not supported port_speed:%d\n", (int)port_speed);
                            return BCM_E_PARAM;
                    }
                    break;
#endif /* PHYMOD_TSCF_GEN3_SUPPORT */
#ifdef PHYMOD_FALCON_DPLL_SUPPORT
                case phymodDispatchTypeFalcon_dpll:
                    if (SOC_IS_MONTEREY(unit)) {
                        /* This section is for Cpri port on Monterey. */
                        switch (port_speed) {
                            case 2457:
                                if (sdm_divisor == 0x7800) {
                                    *frequency = bcmTimeSyncE25MHz;
                                } else {
                                    return BCM_E_PARAM;
                                }
                                break;
                            case 3072:
                                if (sdm_divisor == 0xA00) {
                                    *frequency = bcmTimeSyncE25MHz;
                                } else {
                                    return BCM_E_PARAM;
                                }
                                break;
                            case 4915:
                                if (sdm_divisor == 0x400) {
                                    *frequency = bcmTimeSyncE25MHz;
                                } else {
                                    return BCM_E_PARAM;
                                }
                                break;
                            case 6144:
                                if (sdm_divisor == 0x4B00) {
                                    *frequency = bcmTimeSyncE25MHz;
                                } else {
                                    return BCM_E_PARAM;
                                }
                                break;
                            case 9830:
                            case 10137:
                                if (sdm_divisor == 0x800) {
                                    *frequency = bcmTimeSyncE25MHz;
                                } else {
                                    return BCM_E_PARAM;
                                }
                                break;
                            case 12165:
                            case 24330:
                                if (sdm_divisor == 0x9480) {
                                    *frequency = bcmTimeSyncE25MHz;
                                } else {
                                    return BCM_E_PARAM;
                                }
                                break;
                            default:
                                cli_out("not supported port_speed:%d\n", (int)port_speed);
                                return BCM_E_PARAM;
                        }
                    }
                    break;
#endif /* PHYMOD_FALCON_DPLL_SUPPORT */
#ifdef PHYMOD_QTCE16_SUPPORT
                        case phymodDispatchTypeQtce16:
                            /* Eagle port with QTC mode. */
                            if (sdm_divisor == 0x14a0 &&
                                (port_speed == 1000 ||
                                 port_speed == 2500)) {
                                /* USXGMII */
                                /* 1G/2.5G - VCO 10.3125, 0x14A0 */
                                *frequency = bcmTimeSyncE25MHz;
                            } else if (sdm_divisor == 0x1400 &&
                                       (port_speed == 10 ||
                                        port_speed == 100 ||
                                        port_speed == 1000)) {
                                /* QSGMII */
                                /* 10/100/1000M - VCO 10, 0x1400 */
                                *frequency = bcmTimeSyncE25MHz;
                            } else {
                                return BCM_E_PARAM;
                            }
                            break;
#endif /* PHYMOD_QTCE16_SUPPORT */
                    default: break;
                } /* switch(dispatch_type) */
            }
            else if (!SOC_PORT_USE_PORTCTRL(unit, clk_src_config->port)) {
                /* Not PortMacro, like GPHYs */
#if defined(BCM_HURRICANE4_SUPPORT)
                if (SOC_IS_HURRICANE4(unit)) {
                    /* GPHY0-GPHY5 only supports 25MHz mode */
                    *frequency = bcmTimeSyncE25MHz;
                }
#endif /* BCM_HURRICANE4_SUPPORT */
            }
            if (SOC_IS_TOMAHAWKX(unit) &&  (!SOC_PORT_USE_PORTCTRL(unit, clk_src_config->port)) ) {
                *frequency = bcmTimeSyncE25MHz;
            }
        }
    }
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_TRIDENT3_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) && !SOC_IS_HELIX4(unit)) {
        int lport = clk_src_config->port;
        int phy_port = SOC_INFO(unit).port_l2p_mapping[clk_src_config->port];
        int port_speed = 0;

        if (!SOC_PORT_VALID(unit,lport)) {
            return BCM_E_FAIL;
        }

        /* TR3 :
                   stage0_mode=0 -> pass-through mode
                   stage0_mode=1 -> Div_by_5 mode
                   stage0_mode=2 -> Div_by_10 mode
                   stage0_mode=3 -> Div_by_2 mode
        */

        phy_port = SOC_INFO(unit).port_l2p_mapping[clk_src_config->port];
        BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, clk_src_config->port, &port_speed));
        *frequency = -1;
        switch (port_speed) {
            case 10:
            case 100:
            case 1000:
                /* 1 G ports */
                if (BCM_TIME_TR3_PHYPORT_UNICORE(phy_port) ||
                        BCM_TIME_TR3_PHYPORT_WARPCORE(phy_port)) {
                    if (div_out.stage0_mode == 0) {
                        *frequency = bcmTimeSyncE125MHz;
                    } else if (div_out.stage0_mode == 1) {
                        *frequency = bcmTimeSyncE25MHz;
                    }
                }
                break;
            case 2500:
            case 10000:
                /* 2.5G, 10 G ports */
                if (BCM_TIME_TR3_PHYPORT_UNICORE(phy_port) ||
                        BCM_TIME_TR3_PHYPORT_WARPCORE(phy_port)) {
                    if (div_out.stage0_mode == 1) {
                        *frequency = bcmTimeSyncE31MHz;
                    }
                }
               break;
            default:
                return BCM_E_FAIL;
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */


    return rv;
}

/*
 * Function:
 *      bcm_esw_time_synce_clock_source_control_get
 * Purpose:
 *      Get syncE clock source control option
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      clk_src - (IN) clock source
 *      vlaue - (OUT) control value
 * Returns:
 *      BCM_E_xxx
 * Notes: New wrapper API to support synce operation
 */
int
bcm_esw_time_synce_clock_source_control_get(int unit,
                             bcm_time_synce_clock_source_config_t *clk_src_config,
                             bcm_time_synce_clock_source_control_t control,
                             int *value)
{
    int rv = BCM_E_UNAVAIL;

    switch (control) {
    case bcmTimeSynceClockSourceControlSquelch:
        rv = _bcm_esw_time_synce_clock_source_squelch_get(unit, clk_src_config->clk_src, value);
        break;
    case bcmTimeSynceClockSourceControlFrequency:
        rv = _bcm_esw_time_synce_clock_source_frequency_get(unit, clk_src_config, value);
        break;
    default:
        return BCM_E_PARAM;
    }
    return rv;

}

/*
 * Function:
 *      _bcm_esw_time_synce_clock_source_squelch_set
 * Purpose:
 *      Set syncE clock source squelch option
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      squelch   - (IN) synce clock source squelch setting 1 to enable , 0 to disable.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_synce_clock_source_squelch_set(int unit,
                             bcm_time_synce_clock_src_type_t clk_src,
                             int squelch)

{
    int rv = BCM_E_NONE;

    COMPILER_REFERENCE(rv);
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_TOMAHAWK(unit)) {
        soc_reg_t synce_ctrl = TOP_SYNCE_CTRLr;
        soc_field_t l1_rcvd_sw_ovwr_valid = L1_RCVD_SW_OVWR_VALIDf;
        soc_field_t l1_rcvd_sw_ovwr_enable = L1_RCVD_SW_OVWR_ENf;

        if (SOC_IS_TOMAHAWK(unit)) {
            synce_ctrl = TOP_MISC_CONTROLr;
        }

        if (clk_src == bcmTimeSynceClockSourceSecondary) {
            l1_rcvd_sw_ovwr_valid = L1_RCVD_SW_OVWR_BKUP_VALIDf;
            l1_rcvd_sw_ovwr_enable = L1_RCVD_SW_OVWR_ENf;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, synce_ctrl, REG_PORT_ANY,
                                    l1_rcvd_sw_ovwr_valid, ((squelch) ? 0 : 1)));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, synce_ctrl, REG_PORT_ANY,
                                    l1_rcvd_sw_ovwr_enable, ((squelch) ? 1 : 0)));
        return rv;
    }
#endif

    switch(clk_src) {
    case bcmTimeSynceClockSourcePrimary:
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_TOMAHAWK2_SUPPORT) */

#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_CLK0_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_SABER2_SUPPORT) */

#if defined(BCM_METROLITE_SUPPORT)
        if (SOC_IS_METROLITE(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_CLK0_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_METROLITE_SUPPORT) */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TD2P_TT2P(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                        L1_RCVD_CLK_RSTNf, ((squelch) ? 0 : 1)));
        }
#endif

#if defined(BCM_GREYHOUND_SUPPORT)
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
            if (SOC_REG_IS_VALID(unit, TOP_MISC_CONTROL_2r)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                           L1_RCVD_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                           L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
            } else {
                rv = BCM_E_UNAVAIL;
            }
        }
#endif /* defined(BCM_GREYHOUND_SUPPORT) */

#if defined(BCM_HELIX4_SUPPORT)
        if (SOC_IS_HELIX4(unit) ) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_HELIX4_SUPPORT) */

#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) { /* Both Apache and Monterey */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_APACHE_SUPPORT) */

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

#if defined(BCM_MAVERICK2_SUPPORT)
        if (SOC_IS_MAVERICK2(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_MAVERICK2_SUPPORT) */

#if defined(BCM_HELIX5_SUPPORT)
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
            SOC_IS_FIREBOLT6(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_HELIX5_SUPPORT) */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit) || SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_L1_RCVD_CLK_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_L1_RCVD_CLK_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
    }
#endif /* defined(BCM_GREYHOUND2_SUPPORT) */
        break;

    case bcmTimeSynceClockSourceSecondary:
#if defined(BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK2(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_BKUP_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_TOMAHAWK2_SUPPORT) */

#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_CLK1_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1: 0)));
        }
#endif /* defined(BCM_SABER2_SUPPORT) */

#if defined(BCM_METROLITE_SUPPORT)
        if (SOC_IS_METROLITE(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_CLK1_SW_OVWR_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1: 0)));
        }
#endif /* defined(BCM_METROLITE_SUPPORT) */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TD2P_TT2P(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                        L1_RCVD_CLK_BKUP_RSTNf, ((squelch) ? 0: 1)));
        }
#endif

#if defined(BCM_GREYHOUND_SUPPORT)
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
            if (SOC_REG_IS_VALID(unit, TOP_MISC_CONTROL_2r)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                           L1_RCVD_SW_OVWR_BKUP_VALIDf, ((squelch) ? 0 : 1)));
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                           L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
            } else {
                rv = BCM_E_UNAVAIL;
            }
        }
#endif /* defined(BCM_GREYHOUND_SUPPORT) */


#if defined(BCM_HELIX4_SUPPORT)
        if (SOC_IS_HELIX4(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_BKUP_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_1r, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_HELIX4_SUPPORT) */

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_BKUP_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_TRIDENT3_SUPPORT) */

#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) { /* Both Apache and Monterey */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_BKUP_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROLr, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_APACHE_SUPPORT) */

#if defined(BCM_MAVERICK2_SUPPORT)
        if (SOC_IS_MAVERICK2(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_BKUP_VALIDf, ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                       L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_MAVERICK2_SUPPORT) */

#if defined(BCM_HELIX5_SUPPORT)
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
            SOC_IS_FIREBOLT6(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_BKUP_VALIDf,
                                        ((squelch) ? 0 : 1)));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, TOP_MISC_CONTROL_2r, REG_PORT_ANY,
                                        L1_RCVD_SW_OVWR_ENf,
                                        ((squelch) ? 1 : 0)));
        }
#endif /* defined(BCM_HELIX5_SUPPORT) */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit) || SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_L1_RCVD_CLK_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_BKUP_VALIDf, ((squelch) ? 0 : 1)));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, TOP_L1_RCVD_CLK_CONTROLr, REG_PORT_ANY,
                                    L1_RCVD_SW_OVWR_ENf, ((squelch) ? 1 : 0)));
    }
#endif /* defined(BCM_GREYHOUND2_SUPPORT) */
        break;
    default:
        return BCM_E_PARAM;
    }
    return rv;
}

/*
 * Function:
 *      _bcm_esw_time_synce_clock_source_frequency_set
 * Purpose:
 *      Set syncE clock source squelch option
 * Parameters:
 *      unit      - (IN) Unit number.
 *      clk_src   - (IN) clock source type (Primary, Secondary)
 *      frequency - (IN) synce clock source frequency.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
STATIC int
_bcm_esw_time_synce_clock_source_frequency_set(int unit,
                             bcm_time_synce_clock_source_config_t *clk_src_config,
                             int frequency)

{
    int rv = BCM_E_NONE;
    bcm_time_synce_divisor_setting_t div_in;
    bcm_time_synce_divisor_setting_t_init(&div_in);
    COMPILER_REFERENCE(rv);

    if (clk_src_config->input_src == bcmTimeSynceInputSourceTypePort) {
        div_in.index = clk_src_config->port;
    } else if(clk_src_config->input_src == bcmTimeSynceInputSourceTypePLL) {
        div_in.index = clk_src_config->pll_index;
    } else {
        return BCM_E_PARAM;
    }
    if (IS_MANAGEMENT_PORT(unit,  clk_src_config->port) && clk_src_config->input_src == bcmTimeSynceInputSourceTypePort) {
        return BCM_E_PARAM;
    }

#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5(unit)) {
        if((clk_src_config->input_src == bcmTimeSynceInputSourceTypePort) &&
           (bcmi_esw_time_port_is_gmii_mode(unit,
                                            clk_src_config->port, NULL))) {
            return BCM_E_CONFIG;
        }
    }
#endif /* defined(BCM_HELIX5_SUPPORT) */

    div_in.input_src = clk_src_config->input_src;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(SOC_IS_TD2_TT2(unit) && !SOC_IS_TOMAHAWKX(unit) && !SOC_IS_APACHE(unit)) {
        /*
              *       output_freq = (rx_clk) * stage0_sdm * stage1_div
              *       rx_clk      = (VCO/20)
              *       stage0_sdm  = (stage0_sdm_whole) + (stage0_sdm_frac)/256
              *       stage1_div  = 1
              *       (0 <= stage0_sdm_whole <= 255,
              *        0 <= stage0_sdm_frac  <= 255,
              *        stage1_div is always locked to bypass in SDM mode. Div7,Div11 is not allowed.)
              *
              *       For example,
              *           stage0_sdm_whole = 20
              *           stage0_sdm_frac  = 160
              *           stage0_sdm = 20 + 160/256 = 20.625
              *       Port speed         |VCO        |VCO/20     |stage0  |stage1 |output freq(MHz)
              *       10/40/100G         |10.3125    |515.625    |20.625  |1      |25
              */

        switch(frequency) {
            case bcmTimeSyncE23MHz:
                div_in.stage0_mode      = bcmTimeSynceModeBypass;
                div_in.stage0_sdm_whole = 23;
                div_in.stage1_div       = bcmTimeSynceStage1Div11;
                break;
            case bcmTimeSyncE25MHz:
                div_in.stage0_mode      = bcmTimeSynceModeSDMFracDiv;
                div_in.stage0_sdm_whole = 20;
                div_in.stage0_sdm_frac  = 160;
                break;
            case bcmTimeSyncE28MHz:
                div_in.stage0_mode      = bcmTimeSynceModeBypass;
                div_in.stage0_sdm_whole = 28;
                div_in.stage1_div       = bcmTimeSynceStage1Div11;
                break;
            case bcmTimeSyncE46MHz:
                div_in.stage0_mode      = bcmTimeSynceModeBypass;
                div_in.stage0_sdm_whole = 46;
                div_in.stage1_div       = bcmTimeSynceStage1Div11;
                break;
            default:
                return BCM_E_PARAM;
       }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)) {  /* GREYHOUND SyncE Freq Divisor*/
        if(!BCM_TIME_GH_L1_FREQ_SEL_MODE(frequency)) {
             return BCM_E_PARAM;
         }
        /* In Greyhound raw recovered clock from port will be based on below
            pass-through mode             :: freq = 0(00b)
            Div by 5mode(25MHz)         :: freq = 1(01b)
            Div by 10mode(31.25MHz)   :: freq = 2(10b)
            Div by 2mode(156.25MHz)   :: freq = 3(11b)
           */
        /* raw recovered clock div val  */
        switch(frequency) {
            case 0:/* pass-through mode */
                div_in.stage0_mode      = 0;
                break;
            case 1: /* Div by 5mode(25MHz) */
                div_in.stage0_mode      = 1;
                break;
            case 2: /* Div by 10mode(31.25MHz) */
                div_in.stage0_mode      = 2;
                break;
            default: /*  Div by 2mode(156.25MHz)*/
                div_in.stage0_mode      = 3;
       }
    }
#endif /* BCM_GREYHOUND_SUPPORT */

#ifdef BCM_HELIX4_SUPPORT
    if (SOC_IS_HELIX4(unit)) {  /* HELIX4 SyncE Freq Divisor*/
        int phy_port = SOC_INFO(unit).port_l2p_mapping[clk_src_config->port];
        int port_speed = 0;

        if (clk_src_config->input_src != bcmTimeSynceInputSourceTypePort) {
            return BCM_E_CONFIG;
        }

         BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, clk_src_config->port, &port_speed));

         /* Helix4:
            Unicore ports:  1G Fiber: 125MHz, 2.5G and 10G 312.5MHz
            Wrapcore ports: OS8 VCO 10.3125G, 1G, 10G : 156.25MHz,
                            OS5 VCO 6.35G, 1G: 125MHz 2.5G, 10G: 312.5MHz (No hardware yet)
            */

        /* In Helix4 raw recovered clock from port will be based on below
            pass-through mode  :: freq = 0(00b)
            Div by 5mode       :: freq = 1(01b)
            Div by 10mode      :: freq = 2(10b)
            Div by 2mode       :: freq = 3(11b)
           */
        /* raw recovered clock div val  */
        switch(frequency) {
            case bcmTimeSyncE125MHz:
                if (BCM_TIME_HX4_PHYPORT_UNICORE(phy_port)) {
                    if (port_speed == 10 || port_speed == 100 || port_speed ==  1000) {
                        /* unicore 1G ports  - 125MHz/1 = 125MHz */
                        div_in.stage0_mode = 0;
                    } else {
                        return BCM_E_UNAVAIL;
                    }
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;
            case bcmTimeSyncE25MHz:
                if (BCM_TIME_HX4_PHYPORT_UNICORE(phy_port)) {
                    if (port_speed == 10 || port_speed == 100 || port_speed ==  1000) {
                        /* unicore 1G ports  - 125MHz/5 = 25MHz*/
                        div_in.stage0_mode = 1;
                    } else {
                        return BCM_E_UNAVAIL;
                    }
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;
            case bcmTimeSyncE31MHz:
                if (BCM_TIME_HX4_PHYPORT_UNICORE(phy_port)) {
                    if ((port_speed ==  2500) || (port_speed ==  10000)) {
                        /* unicore 2.5G or 10G ports - 312.5MHz/10 =  31.25MHz */
                        div_in.stage0_mode = 2;
                    } else {
                        return BCM_E_UNAVAIL;
                    }
                } else if (BCM_TIME_HX4_PHYPORT_WARPCORE(phy_port)) {
                    /* warpcore OS8 1G or 10G ports - 156.25/5 = 31.25MHz */
                    if ((port_speed ==  1000) || (port_speed ==  10000)) {
                        div_in.stage0_mode = 1;
                    } else {
                        return BCM_E_UNAVAIL;
                    }
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;
            default:
                return BCM_E_CONFIG;;
       }
    }
#endif /* BCM_HELIX4_SUPPORT */

#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANA(unit)) {
        switch (frequency) {
            case bcmTimeSyncE125MHz:
                /* KT do not have SDM */
                break;
            default:
                return BCM_E_FAIL;
                break;
        }
    }
#endif /* defined(BCM_KATANA_SUPPORT) */

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        switch (frequency) {
            case bcmTimeSyncE25MHz:
                div_in.stage0_mode      = bcmTimeSynceModeSDMFracDiv;
                break;
            case bcmTimeSyncE125MHz:
                div_in.stage0_mode      = bcmTimeSynceModeBypass;
                break;
            default:
                return BCM_E_FAIL;
                break;
        }
    }
#endif /* defined(BCM_SABER2_SUPPORT) */

#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)) {
        switch (frequency) {
            case bcmTimeSyncE515MHz:
                div_in.stage0_mode      = bcmTimeSynceModeBypass;
                break;
            case bcmTimeSyncE156MHz:
                div_in.stage0_mode      = bcmTimeSynceModeBypass;
                break;
            default:
                return BCM_E_FAIL;
                break;
        }
    }
#endif /* defined(BCM_HURRICANE3_SUPPORT) */

#if defined(BCM_GREYHOUND2_SUPPORT)
#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
#ifdef PORTMOD_SUPPORT
        if (clk_src_config->input_src == bcmTimeSynceInputSourceTypePort &&
            SOC_PORT_USE_PORTCTRL(unit, clk_src_config->port)) {
            int port_speed = 0; /* speed specified in mbps */
            phymod_dispatch_type_t dispatch_type = phymodDispatchTypeInvalid;
            int gmii_mode;
            bcmi_time_port_mode_t port_mode = bcmi_time_port_mode_invalid;

            (void)bcmi_esw_time_port_is_gmii_mode(unit, clk_src_config->port,
                                                  &gmii_mode);

            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, clk_src_config->port, &port_speed));

            dispatch_type = _bcm_time_synce_tsc_phymod_dispatch_type_get(unit, clk_src_config->port);
            bcmi_esw_time_port_mode_get
                (unit, dispatch_type, clk_src_config->port, &port_mode);
            switch (frequency) {
                case bcmTimeSyncE25MHz: /* 25MHz using SDM mode */
                    div_in.stage0_mode = bcmTimeSynceModeSDMFracDiv;
                    div_in.stage1_div = bcmTimeSynceStage1Div1;
                    switch(dispatch_type) {
#ifdef PHYMOD_TSCE16_SUPPORT
                        case phymodDispatchTypeTsce16:
                            /* Eagle port with Ethernet mode. */
                            if (!IS_HG_PORT(unit, clk_src_config->port)) {
                                /* Regular port, 0x14A0 */
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xa0;
                            } else {
                                /* Higig port, 0x15E0 */
                                div_in.stage0_sdm_whole = 0x15;
                                div_in.stage0_sdm_frac  = 0xe0;
                            }
                            break;
#endif /* PHYMOD_TSCE16_SUPPORT */
#ifdef PHYMOD_QTCE16_SUPPORT
                        case phymodDispatchTypeQtce16:
                            /* Eagle port with QTC mode. */
                            if (gmii_mode == 2 &&
                                (port_speed == 1000 ||
                                 port_speed == 2500)) {
                                /* USXGMII */
                                /* 1G/2.5G - VCO 10.3125, 0x14A0 */
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xa0;
                            } else if (gmii_mode == 1 &&
                                       (port_speed == 10 ||
                                        port_speed == 100 ||
                                        port_speed == 1000)) {
                                /* QSGMII */
                                /* 10/100/1000M - VCO 10, 0x1400 */
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0x00;
                            }
                            break;
#endif /* PHYMOD_QTCE16_SUPPORT */
#ifdef PHYMOD_TSCF16_GEN3_SUPPORT
                        case phymodDispatchTypeTscf16_gen3:
                            if (port_mode == bcmi_time_port_mode_single) {
                                switch (port_speed) {
                                    case 106000:
                                        /* 106G HG - VCO 27.34375, 0x1B58 */
                                        div_in.stage0_sdm_whole = 0x1B;
                                        div_in.stage0_sdm_frac  = 0x58;
                                        break;
                                    case 100000:
                                        /* 100G - VCO 25.78125, 0x19C8 */
                                        div_in.stage0_sdm_whole = 0x19;
                                        div_in.stage0_sdm_frac  = 0xC8;
                                        break;
                                    case 42000:
                                        /* 42G HG - VCO 21.875, 0xAF0 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0xF0;
                                        break;
                                    case 40000:
                                        /* 40G - VCO 20.625, 0xA50 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0x50;
                                        break;
                                    default:
                                        return BCM_E_PARAM;
                                    break;
                                }
                            } else if (port_mode == bcmi_time_port_mode_dual) {
                                switch (port_speed) {
                                    case 53000:
                                        /* 53G HG - VCO 27.34375, 0x1B58 */
                                        div_in.stage0_sdm_whole = 0x1B;
                                        div_in.stage0_sdm_frac  = 0x58;
                                        break;
                                    case 50000:
                                        /* 50G - VCO 25.78125, 0x19C8 */
                                        div_in.stage0_sdm_whole = 0x19;
                                        div_in.stage0_sdm_frac  = 0xC8;
                                        break;
                                    case 42000:
                                        /* 42G HG - VCO 21.875, 15E0 */
                                        div_in.stage0_sdm_whole = 0x15;
                                        div_in.stage0_sdm_frac  = 0xE0;
                                        break;
                                    case 40000:
                                        /* 40G - VCO 20.625, 0x14A0 */
                                        div_in.stage0_sdm_whole = 0x14;
                                        div_in.stage0_sdm_frac  = 0xA0;
                                        break;
                                    case 21000:
                                        /* 21G - VCO 21.875, 0xAF0 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0xF0;
                                        break;
                                    case 20000:
                                        /* 20G - VCO 20.625, 0xA50 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0x50;
                                        break;
                                    default:
                                        return BCM_E_PARAM;
                                    break;
                                }
                            } else if (port_mode == bcmi_time_port_mode_quad) {
                                int vco_25g =
                                    soc_property_port_get
                                        (unit, clk_src_config->port,
                                         spn_SERDES_1000X_AT_25G_VCO,
                                         1);
                                switch (port_speed) {
                                    case 27000:
                                        /* 27G HG - VCO 27.34375, 0x1B58 */
                                        div_in.stage0_sdm_whole = 0x1B;
                                        div_in.stage0_sdm_frac  = 0x58;
                                        break;
                                    case 25000:
                                        /* 25G - VCO 25.78125, 0x19C8 */
                                        div_in.stage0_sdm_whole = 0x19;
                                        div_in.stage0_sdm_frac  = 0xC8;
                                        break;
                                    case 11000:
                                        /* 11G HG - VCO 27.34375, 0xAF0 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0xF0;
                                        break;
                                    case 10000:
                                        /* 10G - VCO 25.78125, 0xA50 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0x50;
                                        break;
                                    case 1000:
                                        if (vco_25g) {
                                            /* 1G - VCO 25.78125, 0x19C8 */
                                            div_in.stage0_sdm_whole = 0x19;
                                            div_in.stage0_sdm_frac  = 0xC8;
                                        } else {
                                            /* 1G - VCO 20.625, 0x14A0 */
                                            div_in.stage0_sdm_whole = 0x14;
                                            div_in.stage0_sdm_frac  = 0xA0;
                                        }
                                        break;
                                    default:
                                        return BCM_E_PARAM;
                                    break;
                                }
                            } else if (port_mode == bcmi_time_port_mode_tri) {
                                switch (port_speed) {
                                    case 50000:
                                    case 25000:
                                        /* 25G/50G - VCO 27.34375, 0x19C8 */
                                        div_in.stage0_sdm_whole = 0x19;
                                        div_in.stage0_sdm_frac  = 0xC8;
                                        break;
                                    case 40000:
                                        /* 40G - VCO 20.625, 0x14A0 */
                                        div_in.stage0_sdm_whole = 0x14;
                                        div_in.stage0_sdm_frac  = 0xA0;
                                        break;
                                    case 20000:
                                    case 10000:
                                        /* 10G/20G - VCO 20.625, 0xA50 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0x50;
                                        break;
                                    default:
                                        return BCM_E_PARAM;
                                    break;
                                }
                            }
                        break;
#endif /* PHYMOD_TSCF16_GEN3_SUPPORT */
                        default:
                            cli_out("unknown port dispatch type  ... %d\n",
                                    (int)dispatch_type);
                            return BCM_E_FAIL;
                    }
                    break;
                default:
                    /* not supported */
                    return BCM_E_FAIL;
                    break;
            } /* switch(frequency */
        }
#endif /* PORTMOD_SUPPORT */
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    if (SOC_IS_GREYHOUND2(unit)) {
        if (clk_src_config->input_src == bcmTimeSynceInputSourceTypePort) {
            phy_ctrl_t *pc;
            phymod_dispatch_type_t type = phymodDispatchTypeInvalid;
            int speed = 0;

            pc = INT_PHY_SW_STATE(unit, clk_src_config->port);
            if (pc == NULL) {
                return BCM_E_FAIL;
            }
            type = pc->phymod_ctrl.phy[0]->pm_phy.type;
            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, clk_src_config->port, &speed));

            switch (frequency) {
                case bcmTimeSyncE25MHz: /* 25MHz using SDM mode */
                    switch(type) {
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
                        case phymodDispatchTypeTsce_dpll:
                            div_in.stage0_mode = bcmTimeSynceModeSDMFracDiv;
                            div_in.stage1_div = bcmTimeSynceStage1Div1;
                            if (speed == 10606 || speed == 21000 || speed == 42000) {
                                div_in.stage0_sdm_whole = 0x15;
                                div_in.stage0_sdm_frac  = 0xe0;
                            } else {
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xa0;
                            }
                            break;
#endif /* PHYMOD_TSCE_DPLL_SUPPORT */
#ifdef PHYMOD_TSCF_SUPPORT
                        case phymodDispatchTypeTscf:
                            div_in.stage0_mode = bcmTimeSynceModeSDMFracDiv;
                            div_in.stage1_div = bcmTimeSynceStage1Div1;
                            /* falcon ports */
                            if (speed ==  1000 || speed == 10000 || speed== 40000) {
                                /* 1G or 10G or 40G ports -  */
                                div_in.stage0_sdm_whole = 0xa;
                                div_in.stage0_sdm_frac  = 0x50;
                            } else if (speed == 25000 || speed == 50000) {
                                /* 25G or 50G ports - VCO 25.78125*/
                                div_in.stage0_sdm_whole = 0x19;
                                div_in.stage0_sdm_frac  = 0xc8;
                            }
                            if (IS_HG_PORT(unit, clk_src_config->port) && (speed == 10000 || speed == 21000 || speed == 42000)) {
                                /* 10G/21G/42G HG ports -VCO 21.875 */
                                div_in.stage0_sdm_whole = 0xa;
                                div_in.stage0_sdm_frac  = 0xf0;
                            } else if (IS_HG_PORT(unit, clk_src_config->port) && speed == 25000) {
                                /* 25G HG ports  */
                                div_in.stage0_sdm_whole = 0x1b;
                                div_in.stage0_sdm_frac  = 0x58;
                            }
                            break;
#endif /* PHYMOD_TSCF_SUPPORT */
#ifdef PHYMOD_QTCE_SUPPORT
                        case phymodDispatchTypeQtce:
                            div_in.stage0_mode = bcmTimeSynceModeSDMFracDiv;
                            div_in.stage1_div = bcmTimeSynceStage1Div1;
                            if (speed == 10000) {
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xa0;
                            } else {
                                div_in.stage0_sdm_whole = 0x0C;
                                div_in.stage0_sdm_frac  = 0x80;
                            }
                            break;
#endif /* PHYMOD_QTCE_SUPPORT */
#ifdef PHYMOD_VIPER_SUPPORT
                        case phymodDispatchTypeViper:
                            if (speed == 1000) {
                                /* 125 Mhz; post div 5 to get 25Mhz */
                                div_in.stage0_mode = bcmTimeSynceModeBypass;
                                div_in.stage0_sdm_whole = 1;
                                break;
                            }
                            if (speed == 100) {
                                /* TSC rec clock 25Mhz */
                                break;
                            }
#endif
                        default: return BCM_E_PARAM;
                        }
                    break;
                case bcmTimeSyncE125MHz: /* 125MHz viper */
                    switch(type) {
#ifdef PHYMOD_VIPER_SUPPORT
                        case phymodDispatchTypeViper:
                            if (speed == 1000) {
                                /* TSC recvd clock 125 Mhz*/
                                break;
                            }
#endif
                        default: return BCM_E_PARAM;
                    }
                    break;
               case bcmTimeSyncE156MHz: /* 156MHz viper */
                    switch(type) {
#ifdef PHYMOD_VIPER_SUPPORT
                        case phymodDispatchTypeViper:
                            if (speed == 2500) {
                                /* TSC recvd clock 312.5 Mhz post div 2 to get 156.25Mhz*/
                                div_in.stage0_mode = bcmTimeSynceModeBypass;
                                div_in.stage0_sdm_whole = 3;
                                break;
                            }
#endif
                        default: return BCM_E_PARAM;
                    }
                    break;
                default:
                    return BCM_E_PARAM;
            }
        }
    }
#endif /* defined (BCM_GREYHOUND2_SUPPORT) */

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || \
    defined(BCM_HELIX5_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
    if (clk_src_config->input_src == bcmTimeSynceInputSourceTypePort) {
#ifdef PORTMOD_SUPPORT
        if ((SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3(unit) ||
             SOC_IS_APACHE(unit) || SOC_IS_MAVERICK2(unit) ||
             SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit) ||
             SOC_IS_FIREBOLT6(unit) || SOC_IS_MONTEREY(unit))
            && SOC_PORT_USE_PORTCTRL(unit, clk_src_config->port)) {

            int port_speed = 0; /* speed specified in mbps*/
            phymod_dispatch_type_t dispatch_type = phymodDispatchTypeInvalid;
            int gmii_mode;
            bcmi_time_port_mode_t port_mode = bcmi_time_port_mode_invalid;

            (void)bcmi_esw_time_port_is_gmii_mode(unit, clk_src_config->port,
                                                  &gmii_mode);

            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, clk_src_config->port, &port_speed));

            dispatch_type = _bcm_time_synce_tsc_phymod_dispatch_type_get(unit, clk_src_config->port);

            bcmi_esw_time_port_mode_get
                (unit, dispatch_type, clk_src_config->port, &port_mode);

            /* as per top spec 25MHz recovered clock is suggested from all falcon ports
             * using SDM mode configuration
             * Tomahawk has 4 LCPLL ports , 1 - LCPLL1, 2 - LCPLL2, 3 - LCPLL3, 4 - LCPLL4
             * TD3 has 4 XGPLL XGPLL0-XGPLL3
             */
            switch (frequency) {
                case bcmTimeSyncE25MHz: /* 25MHz using SDM mode */
                    div_in.stage0_mode      = bcmTimeSynceModeSDMFracDiv;
                    div_in.stage1_div = bcmTimeSynceStage1Div1;
                    switch(dispatch_type) {
#ifdef PHYMOD_TSCE_SUPPORT
                        case phymodDispatchTypeTsce:
#endif
#ifdef PHYMOD_TSCE16_SUPPORT
                        case phymodDispatchTypeTsce16:
#endif
#if defined (PHYMOD_TSCE_SUPPORT) || defined (PHYMOD_TSCE16_SUPPORT)
                            /* eagle port */
                            if (!IS_HG_PORT(unit, clk_src_config->port)) {
                                /* regular port */
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xa0;
                            } else {
                                /* hi gig port */
                                div_in.stage0_sdm_whole = 0x15;
                                div_in.stage0_sdm_frac  = 0xe0;
                            }
                            break;
#endif
#ifdef PHYMOD_TSCF_SUPPORT
                        case phymodDispatchTypeTscf:
                            /* falcon ports */
                            if (port_speed ==  100) {
                                /* 1G ports  - VCO 25.78125 */
                                div_in.stage0_sdm_whole = 0x19;
                                div_in.stage0_sdm_frac  = 0xc8;
                            } else if (port_speed == 10000 || port_speed == 40000) {
                                /* 10G or 40G ports -  VCO 20.625 */
                                div_in.stage0_sdm_whole = 0xa;
                                div_in.stage0_sdm_frac  = 0x50;
                            } else if (port_speed == 100000 || port_speed == 25000) {
                                /* 100G ports - VCO 25.78125*/
                                div_in.stage0_sdm_whole = 0x19;
                                div_in.stage0_sdm_frac  = 0xc8;
                            } else if (IS_HG_PORT(unit, clk_src_config->port) && port_speed == 11000) {
                                /* 11G HG ports -VCO 21.875 */
                                div_in.stage0_sdm_whole = 0xa;
                                div_in.stage0_sdm_frac  = 0xf0;
                            } else if (IS_HG_PORT(unit, clk_src_config->port) && port_speed == 106000) {
                                /* 106G HG ports -VCO 21.875 */
                                div_in.stage0_sdm_whole = 0x1b;
                                div_in.stage0_sdm_frac  = 0x58;
                            }
                            break;
#endif /* PHYMOD_TSCF_SUPPORT */
#ifdef PHYMOD_TSCF16_GEN3_SUPPORT
                        case phymodDispatchTypeTscf16_gen3:
                            if (port_mode == bcmi_time_port_mode_single) {
                                switch (port_speed) {
                                    case 106000:
                                        /* 106G HG - VCO 27.34375, 0x1B58 */
                                        div_in.stage0_sdm_whole = 0x1B;
                                        div_in.stage0_sdm_frac  = 0x58;
                                        break;
                                    case 100000:
                                        /* 100G - VCO 25.78125, 0x19C8 */
                                        div_in.stage0_sdm_whole = 0x19;
                                        div_in.stage0_sdm_frac  = 0xC8;
                                        break;
                                    case 42000:
                                        /* 42G HG - VCO 21.875, 0xAF0 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0xF0;
                                        break;
                                    case 40000:
                                        /* 40G - VCO 20.625, 0xA50 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0x50;
                                        break;
                                    default:
                                        return BCM_E_PARAM;
                                    break;
                                }
                            } else if (port_mode == bcmi_time_port_mode_dual) {
                                switch (port_speed) {
                                    case 53000:
                                        /* 53G HG - VCO 27.34375, 0x1B58 */
                                        div_in.stage0_sdm_whole = 0x1B;
                                        div_in.stage0_sdm_frac  = 0x58;
                                        break;
                                    case 50000:
                                        /* 50G - VCO 25.78125, 0x19C8 */
                                        div_in.stage0_sdm_whole = 0x19;
                                        div_in.stage0_sdm_frac  = 0xC8;
                                        break;
                                    case 42000:
                                        /* 42G HG - VCO 21.875, 15E0 */
                                        div_in.stage0_sdm_whole = 0x15;
                                        div_in.stage0_sdm_frac  = 0xE0;
                                        break;
                                    case 40000:
                                        /* 40G - VCO 20.625, 0x14A0 */
                                        div_in.stage0_sdm_whole = 0x14;
                                        div_in.stage0_sdm_frac  = 0xA0;
                                        break;
                                    case 21000:
                                        /* 21G - VCO 21.875, 0xAF0 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0xF0;
                                        break;
                                    case 20000:
                                        /* 20G - VCO 20.625, 0xA50 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0x50;
                                        break;
                                    default:
                                        return BCM_E_PARAM;
                                    break;
                                }
                            } else if (port_mode == bcmi_time_port_mode_quad) {
                                int vco_25g =
                                    soc_property_port_get
                                        (unit, clk_src_config->port,
                                         spn_SERDES_1000X_AT_25G_VCO,
                                         1);
                                switch (port_speed) {
                                    case 27000:
                                        /* 27G HG - VCO 27.34375, 0x1B58 */
                                        div_in.stage0_sdm_whole = 0x1B;
                                        div_in.stage0_sdm_frac  = 0x58;
                                        break;
                                    case 25000:
                                        /* 25G - VCO 25.78125, 0x19C8 */
                                        div_in.stage0_sdm_whole = 0x19;
                                        div_in.stage0_sdm_frac  = 0xC8;
                                        break;
                                    case 11000:
                                        /* 11G HG - VCO 27.34375, 0xAF0 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0xF0;
                                        break;
                                    case 10000:
                                        /* 10G - VCO 25.78125, 0xA50 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0x50;
                                        break;
                                    case 1000:
                                        if (vco_25g) {
                                            /* 1G - VCO 25.78125, 0x19C8 */
                                            div_in.stage0_sdm_whole = 0x19;
                                            div_in.stage0_sdm_frac  = 0xC8;
                                        } else {
                                            /* 1G - VCO 20.625, 0x14A0 */
                                            div_in.stage0_sdm_whole = 0x14;
                                            div_in.stage0_sdm_frac  = 0xA0;
                                        }
                                        break;
                                    default:
                                        return BCM_E_PARAM;
                                    break;
                                }
                            } else if (port_mode == bcmi_time_port_mode_tri) {
                                switch (port_speed) {
                                    case 50000:
                                    case 25000:
                                        /* 25G/50G - VCO 27.34375, 0x19C8 */
                                        div_in.stage0_sdm_whole = 0x19;
                                        div_in.stage0_sdm_frac  = 0xC8;
                                        break;
                                    case 40000:
                                        /* 40G - VCO 20.625, 0x14A0 */
                                        div_in.stage0_sdm_whole = 0x14;
                                        div_in.stage0_sdm_frac  = 0xA0;
                                        break;
                                    case 20000:
                                    case 10000:
                                        /* 10G/20G - VCO 20.625, 0xA50 */
                                        div_in.stage0_sdm_whole = 0xA;
                                        div_in.stage0_sdm_frac  = 0x50;
                                        break;
                                    default:
                                        return BCM_E_PARAM;
                                    break;
                                }
                            }
                        break;
#endif /* PHYMOD_TSCF16_GEN3_SUPPORT */
#ifdef PHYMOD_TSCF16_SUPPORT
                        case phymodDispatchTypeTscf16:
                            /* falcon ports */
                            if (port_speed ==  100) {
                                /* 1G ports  - VCO 25.78125 */
                                div_in.stage0_sdm_whole = 0x19;
                                div_in.stage0_sdm_frac  = 0xc8;
                            } else if (port_speed == 10000 || port_speed == 40000) {
                                /* 10G or 40G ports -  VCO 20.625 */
                                div_in.stage0_sdm_whole = 0xa;
                                div_in.stage0_sdm_frac  = 0x50;
                            } else if (port_speed == 100000 || port_speed == 25000) {
                                /* 100G ports - VCO 25.78125*/
                                div_in.stage0_sdm_whole = 0x19;
                                div_in.stage0_sdm_frac  = 0xc8;
                            } else if (IS_HG_PORT(unit, clk_src_config->port) && port_speed == 11000) {
                                /* 11G HG ports -VCO 21.875 */
                                div_in.stage0_sdm_whole = 0xa;
                                div_in.stage0_sdm_frac  = 0xf0;
                            } else if (IS_HG_PORT(unit, clk_src_config->port) && port_speed == 106000) {
                                /* 106G HG ports -VCO 21.875 */
                                div_in.stage0_sdm_whole = 0x1b;
                                div_in.stage0_sdm_frac  = 0x58;
                            }
                            break;
#endif /* PHYMOD_TSCF16_SUPPORT */
#ifdef PHYMOD_TSCBH_SUPPORT
                        case phymodDispatchTypeTscbh:
                            /* TSCBH ports */
                            if (port_speed == 10000 || port_speed == 40000) {
                                /* 10G or 40G ports -  VCO 20.625 */
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xa0;
                            } else if (port_speed == 100000 || port_speed == 20000) {
                                /* 100G or 20G or 400G ports - VCO 25.78125*/
                                div_in.stage0_sdm_whole = 0x33;
                                div_in.stage0_sdm_frac  = 0x90;
                            } else if (port_speed == 50000 || port_speed == 400000){
                                /* 50G port VCO 26.5625 */
                                div_in.stage0_sdm_whole = 0x35;
                                div_in.stage0_sdm_frac  = 0x20;
                            }
                            break;
#endif /* PHYMOD_TSCBH_SUPPORT */
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
                        case phymodDispatchTypeTsce_dpll:
                            if (port_speed == 1000 || port_speed == 10000 || port_speed == 40000 || port_speed == 20000) {
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xa0;
                            } else {
                                div_in.stage0_sdm_whole = 0x15;
                                div_in.stage0_sdm_frac  = 0xE0;
                            }
                            break;
#endif /* PHYMOD_TSCE_DPLL_SUPPORT */
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
                        case phymodDispatchTypeTscf_gen3:
                            if (port_speed == 25000 || port_speed == 100000 || port_speed == 50000) {
                                div_in.stage0_sdm_whole = 0x19;
                                div_in.stage0_sdm_frac  = 0xC8;
                            } else if (port_speed == 1000) {
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xA0;
                            } else {
                                div_in.stage0_sdm_whole = 0x0A;
                                div_in.stage0_sdm_frac  = 0x50;
                            }
                            break;
#endif /* PHYMOD_TSCF_GEN3_SUPPORT */
#ifdef PHYMOD_QTCE16_SUPPORT
                        case phymodDispatchTypeQtce16:
                            /* Eagle port with QTC mode. */
                            if (gmii_mode == 2 &&
                                (port_speed == 1000 ||
                                 port_speed == 2500)) {
                                /* USXGMII */
                                /* 1G/2.5G - VCO 10.3125, 0x14A0 */
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xa0;
                            } else if (gmii_mode == 1 &&
                                       (port_speed == 10 ||
                                        port_speed == 100 ||
                                        port_speed == 1000)) {
                                /* QSGMII */
                                /* 10/100/1000M - VCO 10, 0x1400 */
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0x00;
                            }
                            break;
#endif /* PHYMOD_QTCE16_SUPPORT */
#ifdef PHYMOD_FALCON_DPLL_SUPPORT
                        case phymodDispatchTypeFalcon_dpll:
                            if (SOC_IS_MONTEREY(unit)) {
                                /* This section is for Cpri port on Monterey. */
                                if (port_speed == 2457) {
                                    div_in.stage0_sdm_whole = 0x78;
                                    div_in.stage0_sdm_frac = 0x00;
                                } else if (port_speed == 3072) {
                                    div_in.stage0_sdm_whole = 0x0A;
                                    div_in.stage0_sdm_frac = 0x00;
                                } else if (port_speed == 4915) {
                                    div_in.stage0_sdm_whole = 0x04;
                                    div_in.stage0_sdm_frac = 0x00;
                                } else if (port_speed == 6144) {
                                    div_in.stage0_sdm_whole = 0x4B;
                                    div_in.stage0_sdm_frac = 0x00;
                                } else if (port_speed == 9830 || port_speed == 10137) {
                                    div_in.stage0_sdm_whole = 0x08;
                                    div_in.stage0_sdm_frac = 0x00;
                                } else if (port_speed == 12165 || port_speed == 24330) {
                                    div_in.stage0_sdm_whole = 0x94;
                                    div_in.stage0_sdm_frac = 0x80;
                                }
                                break;
                            }
#endif /* PHYMOD_FALCON_DPLL_SUPPORT */
                        default:
                            cli_out("unknown port dispatch type  ... %d\n", (int)dispatch_type);
                            return BCM_E_FAIL;
                    }
                    break;
                case bcmTimeSyncE7MHz:
                    div_in.stage0_mode      = bcmTimeSynceModeSDMFracDiv;
                    div_in.stage1_div = bcmTimeSynceStage1Div1;
                    switch(dispatch_type) {
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
                        case phymodDispatchTypeTsce_dpll:
                            if (port_speed == 1000 || port_speed == 10000 || port_speed == 40000 || port_speed == 20000) {
                                div_in.stage0_sdm_whole = 0x42;
                                div_in.stage0_sdm_frac  = 0x00;
                            }
                            break;
#endif /* PHYMOD_TSCE_DPLL_SUPPORT */
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
                        case phymodDispatchTypeTscf_gen3:
                            if (port_speed == 25000 || port_speed == 100000 || port_speed == 50000) {
                                div_in.stage0_sdm_whole = 0x52;
                                div_in.stage0_sdm_frac  = 0x80;
                            } else if (port_speed == 1000) {
                                div_in.stage0_sdm_whole = 0x42;
                                div_in.stage0_sdm_frac  = 0x00;
                            } else {
                                div_in.stage0_sdm_whole = 0x21;
                                div_in.stage0_sdm_frac  = 0x00;
                            }
                            break;
#endif /* PHYMOD_TSCF_GEN3_SUPPORT */
                            default: break;
                        }
                    break;
                case bcmTimeSyncE12MHz:
                    div_in.stage0_mode      = bcmTimeSynceModeSDMFracDiv;
                    div_in.stage1_div = bcmTimeSynceStage1Div1;
                        switch(dispatch_type) {
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
                            case phymodDispatchTypeTsce_dpll:
                                if (port_speed == 1000 || port_speed == 10000 || port_speed == 40000 || port_speed == 20000) {
                                    div_in.stage0_sdm_whole = 0x29;
                                    div_in.stage0_sdm_frac  = 0x40;
                                }
                                break;
#endif /* PHYMOD_TSCE_DPLL_SUPPORT */
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
                            case phymodDispatchTypeTscf_gen3:
                                if (port_speed == 25000 || port_speed == 100000 || port_speed == 50000) {
                                    div_in.stage0_sdm_whole = 0x33;
                                    div_in.stage0_sdm_frac  = 0x90;
                                } else if (port_speed == 1000) {
                                    div_in.stage0_sdm_whole = 0x29;
                                    div_in.stage0_sdm_frac  = 0x40;
                                } else {
                                    div_in.stage0_sdm_whole = 0x14;
                                    div_in.stage0_sdm_frac  = 0xa0;
                                }
                                break;
#endif /* PHYMOD_TSCF_GEN3_SUPPORT */
                            default: break;
                            }
                    break;
                case bcmTimeSyncE31MHz:
                    div_in.stage0_mode = bcmTimeSynceModeSDMFracDiv;
                    div_in.stage1_div = bcmTimeSynceStage1Div1;
                        switch(dispatch_type) {
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
                            case phymodDispatchTypeTsce_dpll:
                            if (port_speed == 1000 || port_speed == 10000 || port_speed == 40000 || port_speed == 20000) {
                                div_in.stage0_sdm_whole = 0x10;
                                div_in.stage0_sdm_frac  = 0x80;
                            }
                            break;
#endif
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
                            case phymodDispatchTypeTscf_gen3:
                            if (port_speed == 25000 || port_speed == 100000 || port_speed == 50000) {
                                div_in.stage0_sdm_whole = 0x14;
                                div_in.stage0_sdm_frac  = 0xa0;
                            } else if (port_speed == 1000) {
                                div_in.stage0_sdm_whole = 0x10;
                                div_in.stage0_sdm_frac  = 0x80;
                            } else {
                                div_in.stage0_sdm_whole = 0x08;
                                div_in.stage0_sdm_frac  = 0x40;
                            }
                            break;
#endif /* PHYMOD_TSCF_GEN3_SUPPORT */
                            default: break;
                        }
                    break;
                case bcmTimeSyncE37MHz:
                    div_in.stage0_mode = bcmTimeSynceModeSDMFracDiv;
                    div_in.stage1_div = bcmTimeSynceStage1Div1;
                    switch(dispatch_type) {
#ifdef PHYMOD_TSCE_DPLL_SUPPORT
                        case phymodDispatchTypeTsce_dpll:
                            if (port_speed == 1000 || port_speed == 10000 || port_speed == 40000 || port_speed == 20000) {
                                div_in.stage0_sdm_whole = 0x0D;
                                div_in.stage0_sdm_frac  = 0xC0;
                            }
                            break;
#endif
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
                        case phymodDispatchTypeTscf_gen3:
                            if (port_speed == 25000 || port_speed == 100000 || port_speed == 50000) {
                                div_in.stage0_sdm_whole = 0x11;
                                div_in.stage0_sdm_frac  = 0x30;
                            } else if (port_speed == 1000) {
                                div_in.stage0_sdm_whole = 0x0D;
                                div_in.stage0_sdm_frac  = 0xC0;
                            } else {
                                div_in.stage0_sdm_whole = 0x06;
                                div_in.stage0_sdm_frac  = 0xE0;
                            }
                            break;
#endif /* PHYMOD_TSCF_GEN3_SUPPORT */
                        default: break;
                    }
                    break;
                default:
                    /* not supported */
                    return BCM_E_FAIL;
                    break;
            } /* switch(frequency */
        }
        else
#endif /* PORTMOD_SUPPORT */
        {
            if (!SOC_PORT_USE_PORTCTRL(unit, clk_src_config->port)) {
                /* Not PortMacro, like GPHYs */
#if defined(BCM_HURRICANE4_SUPPORT)
                if (SOC_IS_HURRICANE4(unit)) {
                    switch (frequency) {
                        case bcmTimeSyncE25MHz:
                            /* GPHY0-GPHY5 only supports 25MHz mode */
                            break;
                        default:
                            return BCM_E_UNAVAIL;
                            break;
                    }
                }
#endif /* BCM_HURRICANE4_SUPPORT */
            }
            if (SOC_IS_TOMAHAWK(unit)) {
                switch (frequency) {
                    case bcmTimeSyncE25MHz:
                        div_in.stage0_mode      = bcmTimeSynceModeSDMFracDiv;
                        div_in.stage0_sdm_whole = 20;
                        div_in.stage0_sdm_frac  = 160;
                        break;
                    default:
                        return BCM_E_FAIL;
                break;
                }

            }
        }
    }
#endif /* TH TD3 AP MV2 HX5 HR4 FB6 support */

#if defined(BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        switch (frequency) {
            /* For Viper: div5 logic at Mux1 gives 25MHz
             * For Eagle: default SDM logic choosen TSCE synce div gives 25MHz
             *            make sure portmacro side stage1 is not in default
             *            div11(for SDM stage1 should be in div1)
             */
            case bcmTimeSyncE25MHz:
                div_in.stage0_mode      = bcmTimeSynceModeSDMFracDiv;
                break;
            /* For Viper only : 125MHz bypassed L1 recov_clock */
            case bcmTimeSyncE125MHz:
                div_in.stage0_mode      = bcmTimeSynceModeBypass;
                break;
            default:
                return BCM_E_FAIL;
                break;
        }
    }
#endif /* defined(BCM_METROLITE_SUPPORT) */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) && !SOC_IS_HELIX4(unit)) {  /* TR3 SyncE Freq Divisor*/
        int phy_port = SOC_INFO(unit).port_l2p_mapping[clk_src_config->port];
        int port_speed = 0;

        if (clk_src_config->input_src != bcmTimeSynceInputSourceTypePort) {
            return BCM_E_CONFIG;
        }

         BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, clk_src_config->port, &port_speed));

         /* Triumph 3:
            Unicore ports:  1G Fiber: 125MHz, 2.5G and 10G 156.25MHz
            Wrapcore ports: OS8 VCO 10.3125G, 1G 125MHz 2.5G, 10G  156.25MHz,
                            OS5 VCO 6.25G, 1G: 125MHz 2.5G, 10G: 156.26MHz
            */

        /* In Triumph 3, raw recovered clock from port will be based on below
            pass-through mode  :: freq = 0(00b)
            Div by 5mode       :: freq = 1(01b)
            Div by 10mode      :: freq = 2(10b)
            Div by 2mode       :: freq = 3(11b)
           */
        /* raw recovered clock div val  */
        switch(frequency) {
            case bcmTimeSyncE125MHz:
                if (BCM_TIME_TR3_PHYPORT_UNICORE(phy_port) ||
                        BCM_TIME_TR3_PHYPORT_WARPCORE(phy_port)) {
                    if (port_speed == 10 || port_speed == 100 || port_speed ==  1000) {
                        /* unicore or warpcore 1G ports  - 125MHz/1 = 125MHz */
                        div_in.stage0_mode = 0;
                    } else {
                        return BCM_E_UNAVAIL;
                    }
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;
            case bcmTimeSyncE25MHz:
                if (BCM_TIME_TR3_PHYPORT_UNICORE(phy_port) ||
                        BCM_TIME_TR3_PHYPORT_WARPCORE(phy_port)) {
                    if (port_speed == 10 || port_speed == 100 || port_speed ==  1000) {
                        /* unicore or warpcore 1G ports  - 125MHz/5 = 25MHz*/
                        div_in.stage0_mode = 1;
                    } else {
                        return BCM_E_UNAVAIL;
                    }
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;
            case bcmTimeSyncE31MHz:
                if (BCM_TIME_TR3_PHYPORT_UNICORE(phy_port) ||
                        BCM_TIME_TR3_PHYPORT_WARPCORE(phy_port)) {
                    if ((port_speed ==  2500) || (port_speed ==  10000)) {
                        /* unicore or warpcore 2.5G or 10G ports - 156,25MHz/5 =  31.25MHz */
                        div_in.stage0_mode = 1;
                    } else {
                        return BCM_E_UNAVAIL;
                    }
                } else {
                    return BCM_E_UNAVAIL;
                }
                break;
            default:
                return BCM_E_CONFIG;;
       }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */


    rv = bcm_esw_time_synce_clock_set(unit, clk_src_config->clk_src, &div_in);
    return rv;
}


/*
 * Function:
 *      bcm_esw_time_synce_clock_source_control_set
 * Purpose:
 *      Set syncE clock source control option
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      clk_src_config - (IN) clock source config
 *      value - (IN) control value
 * Returns:
 *      BCM_E_xxx
 * Notes: NEW API to support SYNCE configuration
 */
int
bcm_esw_time_synce_clock_source_control_set(int unit,
                             bcm_time_synce_clock_source_config_t *clk_src_config,
                             bcm_time_synce_clock_source_control_t control,
                             int value)
{
    int rv = BCM_E_UNAVAIL;

    switch (control) {
    case bcmTimeSynceClockSourceControlSquelch:
        rv = _bcm_esw_time_synce_clock_source_squelch_set(unit, clk_src_config->clk_src, value);
        break;
    case bcmTimeSynceClockSourceControlFrequency:
        rv = _bcm_esw_time_synce_clock_source_frequency_set(unit, clk_src_config, value);
        break;
    default:
        return BCM_E_PARAM;
   }

   return rv;
}

/*
 * Function:
 *      bcm_esw_time_bs_time_get
 * Purpose:
 *      Get the broadsync time on the specified unit
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      bs_time - (OUT) broadsync time in seconds and nanoseconds
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */

int
bcm_esw_time_bs_time_get (int unit, bcm_time_spec_t *bs_time)
{
    int rv;   /* Operation return status. */
    uint8 bs_initialized=0;

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    bcm_esw_time_interface_traverse(unit, _bcm_time_interface_traverse_cb,
                                        (void*)&bs_initialized);
    if (!bs_initialized) {
        return BCM_E_INIT;
    }

    if (NULL == bs_time) {
        return (BCM_E_PARAM);
    }

    TIME_LOCK(unit);
    rv = _bcm_time_bs_time_get(unit, bs_time);
    if (BCM_FAILURE(rv)) {
        TIME_UNLOCK(unit);
        return (rv);
    }
    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
int
_bcm_esw_time_counter_get(int unit, int ts_counter, uint64 *pTS_val)
{
    int rv  = BCM_E_NONE;
    soc_reg_t ts0_reg0, ts0_reg1;
    soc_reg_t ts1_reg0, ts1_reg1;
    uint64 ts;
    uint32 upper;
    uint32 lower;

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return (BCM_E_UNAVAIL);
    }

    if (SOC_REG_IS_VALID(unit, NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr)) {
        ts0_reg1 = NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr;
        ts1_reg1 = NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr;
        ts0_reg0 = NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr;
        ts1_reg0 = NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr;
    } else {
        ts0_reg0 = IPROC_NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr;
        ts1_reg0 = IPROC_NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr;
        ts0_reg1 = IPROC_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr;
        ts1_reg1 = IPROC_NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr;
    }

    if (ts_counter == 1) {
        BCM_TIME_READ_NS_REGr(unit, ts1_reg1, 0, &upper);
        BCM_TIME_READ_NS_REGr(unit, ts1_reg0, 0, &lower);
    } else if(ts_counter == 0) {
        BCM_TIME_READ_NS_REGr(unit, ts0_reg1, 0, &upper);
        BCM_TIME_READ_NS_REGr(unit, ts0_reg0, 0, &lower);
    } else {
        return BCM_E_FAIL;
    }

    u64_H(ts) = upper;
    u64_L(ts) = lower;

    if (SOC_IS_TOMAHAWK3(unit)|| SOC_IS_HURRICANE4(unit) || SOC_IS_FIRELIGHT(unit) || SOC_IS_FIREBOLT6(unit)) {
        COMPILER_64_SHR(ts, 4);
    }

    *pTS_val = ts;

    return rv;
}

int
_bcm_esw_time_counter_set(int unit, int ts_counter, uint64 ts_val)
{
    uint32 rval;
    int rv  = BCM_E_NONE;

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return (BCM_E_UNAVAIL);
    }

    /* 52b format frpom 48b ns */
    if(SOC_IS_MONTEREY(unit) || SOC_IS_TOMAHAWK3(unit) || SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit) || SOC_IS_FIRELIGHT(unit)) {
        COMPILER_64_SHL(ts_val, 4);
    }

    if (SOC_IS_FIRELIGHT(unit))
    {
        if (ts_counter == 1) {
            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_val) << 8 | u64_L(ts_val) >> 24);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_L(ts_val) << 8);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, rval));

        } else if (ts_counter == 0) {
            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_val) << 8 | u64_L(ts_val) >> 24);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_L(ts_val) << 8);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, rval));
        } else {
            rv = BCM_E_FAIL;
        }
    } else {
        if (ts_counter == 1) {
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_val) << 8 | u64_L(ts_val) >> 24);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_L(ts_val) << 8);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, rval));

        } else if (ts_counter == 0) {
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_val) << 8 | u64_L(ts_val) >> 24);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_L(ts_val) << 8);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, rval));
        } else {
            rv = BCM_E_FAIL;
        }
    }
    return rv;
}

int
_bcm_esw_time_counter_enable(int unit, int ts_counter, int enable)
{
    uint32 rval;
    int rv  = BCM_E_NONE;

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return (BCM_E_UNAVAIL);
    }

    if (SOC_IS_FIRELIGHT(unit))
    {
        if (ts_counter == 1) {
            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS1_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, rval));
        } else if (ts_counter == 0) {
            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, rval));
        } else {
            rv = BCM_E_FAIL;
        }
    } else {
        if (ts_counter == 1) {
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, rval));
        } else if (ts_counter == 0) {
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, rval));
        } else {
            rv = BCM_E_FAIL;
        }
    }

    return rv;
}

int
_bcm_esw_time_counter_freq_frac_set(int unit, int ts_counter)
{
    uint32 rval;
    int rv  = BCM_E_NONE;

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return (BCM_E_UNAVAIL);
    }

    if (SOC_IS_FIRELIGHT(unit))
    {
        if (ts_counter == 1) {
            /* Disable the common control of TS0 and TS1 */
            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr, &rval, ENABLE_COMMON_CONTROLf, 0);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, rval));

            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr, &rval, UINCf, 0x4000);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, rval));

            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr, &rval, LINCf, 0x0);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, rval));

        } else if (ts_counter == 0) {
            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &rval, UINCf, 0x4000);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, rval));

            SOC_IF_ERROR_RETURN(READ_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &rval));
            soc_reg_field_set(unit, IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &rval, LINCf, 0x0);
            SOC_IF_ERROR_RETURN(WRITE_IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, rval));
        } else {
            rv = BCM_E_FAIL;
        }
    } else {
        if (ts_counter == 1) {
            /* Disable the common control of TS0 and TS1 */
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_COUNTER_CONFIG_SELECTr, &rval, ENABLE_COMMON_CONTROLf, 0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr, &rval, UINCf, 0x2000);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr, &rval, LINCf, 0x0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, rval));

        } else if (ts_counter == 0) {
            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &rval, UINCf, 0x2000);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, rval));

            SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &rval));
            soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &rval, LINCf, 0x0);
            SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, rval));
        } else {
            rv = BCM_E_FAIL;
        }
    }

    return rv;
}

#endif /* BCM_TIME_NANOSYNC_SUPPORT */

/*
 * Function:
 *      bcm_esw_time_ts_counter_set
 * Purpose:
 *      Set timestamper counter
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      counter - (IN)  counter values
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_time_ts_counter_set(
        int unit,
        bcm_time_ts_counter_t  *counter)
{
    int rv = BCM_E_UNAVAIL;
#if (defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
     defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT) || \
     defined(BCM_HURRICANE4_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT) || \
     defined(BCM_FIRELIGHT_SUPPORT))

    uint32 freq_ctrl = 0;
    uint64 check;
    uint32 u64_hi = 0xffff, u64_low = 0xffffffff;

    if (counter == NULL) {
        return BCM_E_PARAM;
    }

    COMPILER_64_SET(check, u64_hi, u64_low);

    if (COMPILER_64_GT(counter->ts_counter_ns, check)) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_nanosync)) {
        int ts_counter;


        
        ts_counter = counter->ts_counter;


        /*Reset the offset since the counter is getting set*/
        SOC_IF_ERROR_RETURN(_bcm_esw_time_ts_offset_reset(unit, ts_counter));

        SOC_IF_ERROR_RETURN(_bcm_esw_time_counter_enable(unit, ts_counter, 0));

        if(SOC_IS_MONTEREY(unit) || SOC_IS_FIREBOLT6(unit) || SOC_IS_FIRELIGHT(unit) || SOC_IS_HURRICANE4(unit)) {
            SOC_IF_ERROR_RETURN(_bcm_esw_time_counter_freq_frac_set(unit, ts_counter));
        }
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_MONTEREY(unit) || SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit) || SOC_IS_FIRELIGHT(unit)) {
            SOC_IF_ERROR_RETURN(_bcm_esw_time_counter_set(unit, ts_counter, counter->ts_counter_ns));
        }
        SOC_IF_ERROR_RETURN(_bcm_esw_time_counter_enable(unit, ts_counter, counter->enable));

        rv = BCM_E_NONE;
    } else
#endif /* BCM_TIME_NANOSYNC_SUPPORT */
    {
        freq_ctrl = (counter->time_format == bcmTimeFormatPTP) ?
            0x40000000 : 0x44B82FA1;

        rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS1_FREQ_CTRL_FRACr,
                REG_PORT_ANY,
                FRACf, freq_ctrl);

        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /*
         * Adding 64bit upper 32 bits into 32 bit container.
         * then extracting only 16 bits.
         */
        COMPILER_64_TO_32_HI(u64_hi, counter->ts_counter_ns);
        rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS1_FREQ_CTRL_UPPERr,
                REG_PORT_ANY,
                NSf, (u64_hi & 0xffff));

        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Adding 64bit lower 32 bits into 32 bit container. */
        COMPILER_64_TO_32_LO(u64_low, counter->ts_counter_ns);
        rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS1_FREQ_CTRL_LOWERr,
                REG_PORT_ANY,
                NSf, u64_low);

        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Set the enable and disable control. */
        rv = soc_reg_field32_modify(unit, CMIC_TIMESYNC_TS1_COUNTER_ENABLEr,
                REG_PORT_ANY,
                ENABLEf, counter->enable);
    }
#endif

    return rv;
}

/*
 * Function:
 *      bcm_esw_time_ts_counter_get
 * Purpose:
 *      Get timestamper counter
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      counter - (OUT)  counter values
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_time_ts_counter_get(
        int unit,
        bcm_time_ts_counter_t  *counter)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TIME_NANOSYNC_SUPPORT)
    soc_reg_t ts0_reg;
    soc_reg_t ts1_reg;
#endif
#if (defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TIME_NANOSYNC_SUPPORT) || \
     defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT) || \
     defined(BCM_FIREBOLT6_SUPPORT) || defined(BCM_FIRELIGHT_SUPPORT))
    uint32 val_hi = 0, val_lo = 0;

    if (counter == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_nanosync)) {
        int ts_counter;
        uint32 regval;

        
        ts_counter = counter->ts_counter;

        SOC_IF_ERROR_RETURN(_bcm_esw_time_counter_get(unit, ts_counter, &counter->ts_counter_ns));

        if (SOC_REG_IS_VALID(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr)) {
            ts0_reg = NS_TIMESYNC_TS0_COUNTER_ENABLEr;
            ts1_reg = NS_TIMESYNC_TS1_COUNTER_ENABLEr;
        } else {
            ts0_reg = IPROC_NS_TIMESYNC_TS0_COUNTER_ENABLEr;
            ts1_reg = IPROC_NS_TIMESYNC_TS1_COUNTER_ENABLEr;
        }

        /* Read the enable status */
        if (ts_counter == 1) {
            BCM_TIME_READ_NS_REGr(unit, ts1_reg, 0, &regval);
            counter->enable = soc_reg_field_get(unit, ts1_reg, regval, ENABLEf);
        } else if(ts_counter == 0) {
            BCM_TIME_READ_NS_REGr(unit, ts0_reg, 0, &regval);
            counter->enable = soc_reg_field_get(unit, ts0_reg, regval, ENABLEf);
        } else {
            return BCM_E_FAIL;
        }

        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_HURRICANE4(unit)) {
            uint32 freq_ctrl = 0, freq_ctrl_2 = 0;
            READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, &freq_ctrl);
            READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, &freq_ctrl_2);

            if ((freq_ctrl == 0x2000) && (freq_ctrl_2 == 0)) {
                counter->time_format = bcmTimeFormatPTP;
            } else {
                counter->time_format = bcmTimeFormatNTP;
            }
        } else {
            counter->time_format = bcmTimeFormatPTP;
        }
        rv = BCM_E_NONE;
    } else
#endif /* BCM_TIME_NANOSYNC_SUPPORT */
    {
        uint32 freq_ctrl = 0;
        rv = READ_CMIC_TIMESYNC_TS1_FREQ_CTRL_FRACr(unit, &freq_ctrl);
        if(BCM_FAILURE(rv)) {
            return rv;
        }

        if (freq_ctrl == 0x40000000) {
            counter->time_format = bcmTimeFormatPTP;
        } else {
            counter->time_format = bcmTimeFormatNTP;
        }

        rv = READ_CMIC_TIMESYNC_TS1_FREQ_CTRL_LOWERr(unit, &val_lo);
        if(BCM_FAILURE(rv)) {
            return rv;
        }

        rv = READ_CMIC_TIMESYNC_TS1_FREQ_CTRL_UPPERr(unit, &val_hi);
        if(BCM_FAILURE(rv)) {
            return rv;
        }

        COMPILER_64_SET(counter->ts_counter_ns, val_hi, val_lo);

        val_lo = 0;
        rv = READ_CMIC_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &val_lo);

        counter->enable = ((val_lo) ? 1 : 0);
    }
#endif

    return rv;
}

#ifdef BCM_TIME_LOCAL_TOD_SUPPORT
/*
 * PTP 32-bit nanosecond field:
 * When the nanosecond[31:0] = 10^9 (decimal), second field will be incremented
 * by one.
 *
 * NTP 32-bit fraction field:
 * Bit[31] means 1/2 second, bit[30] means 1/4 second, ... etc
 *
 * Design Note:
 * Both NTP_TIME and PTP_TIME bit ranges are 90-bit wide
 * - Lower 58-bit (bit[57:0]) is fraction second
 * - Upper 32-bit (bit[89:58]) is second
 * Frequency control has 26 more bits than actual 32-bit required for either
 * fraction (NTP) or nanosecond (PTP). This is for finer control/better precision
 * to adjust frequency offset.
 *
 * So. the actual nanosecond/fraction field in both PTP or NTP is
 * 32-bit wide: {FRAC_SEC_UPPER, FRAC_SEC_LOWER[31:26]} (26+6).
 */
STATIC int
bcmi_time_tod_local_ntp_get(
    int unit,
    bcm_time_tod_t *tod)
{
    uint32 val;
    soc_mem_t mem = NTP_TIMEm;
    ntp_time_entry_t entry;
    uint64 ns, tmp;

    if (tod == NULL) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_use_local_tod)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&entry, 0,
               sizeof(ieee1588_time_entry_t));

    BCM_IF_ERROR_RETURN(
        READ_NTP_TIMEm(unit, MEM_BLOCK_ANY, 0, &entry));

    /* second */
    val = soc_mem_field32_get(unit, mem, &entry, NTP_TIME_ONE_SECf);
    COMPILER_64_SET(tod->ts.seconds, 0, val & 0x1);
    val = soc_mem_field32_get(unit, mem, &entry, NTP_TIME_UPPER_SECf);
    COMPILER_64_SET(tmp, 0, val);
    COMPILER_64_SHL(tmp, 1);
    COMPILER_64_OR(tod->ts.seconds, tmp);

    /* nano second */
    val = soc_mem_field32_get(unit, mem, &entry, NTP_TIME_FRAC_SEC_UPPERf);
    tod->ts.nanoseconds = val << 6;
    val = soc_mem_field32_get(unit, mem, &entry, NTP_TIME_FRAC_SEC_LOWERf);
    tod->ts.nanoseconds |= (val >> 26) & 0x3F;

    COMPILER_64_SET(ns, 0, tod->ts.nanoseconds);
    /* x 1000000000 */
    COMPILER_64_UMUL_32(ns, 1000000000);

    /* / (1 << 32) */
    COMPILER_64_SET(tmp, 0, 1);
    COMPILER_64_SHL(tmp, 32);

    /* ns = frac x 1000000000 / (1 << 32) */
    COMPILER_64_UDIV_64(ns, tmp);
    tod->ts.nanoseconds = COMPILER_64_LO(ns);

    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                            "Current NTP: (%d) sec (%d) ns\n"),
                COMPILER_64_LO(tod->ts.seconds),
                tod->ts.nanoseconds));
    return BCM_E_NONE;
}

STATIC int
bcmi_time_tod_local_ptp_get(
    int unit,
    bcm_time_tod_t *tod)
{
    uint32 val;
    soc_mem_t mem = IEEE1588_TIMEm;
    ieee1588_time_entry_t entry;
    uint64 tmp;

    if (tod == NULL) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_use_local_tod)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&entry, 0,
               sizeof(ieee1588_time_entry_t));

    BCM_IF_ERROR_RETURN(
        READ_IEEE1588_TIMEm(unit, MEM_BLOCK_ANY, 0, &entry));

    /* second */
    val = soc_mem_field32_get(unit, mem, &entry, IEEE1588_TIME_ONE_SECf);
    COMPILER_64_SET(tod->ts.seconds, 0, val & 0x1);
    val = soc_mem_field32_get(unit, mem, &entry, IEEE1588_TIME_UPPER_SECf);
    COMPILER_64_SET(tmp, 0, val);
    COMPILER_64_SHL(tmp, 1);
    COMPILER_64_OR(tod->ts.seconds, tmp);

    /* nano second */
    val = soc_mem_field32_get(unit, mem, &entry, IEEE1588_TIME_FRAC_SEC_UPPERf);
    tod->ts.nanoseconds = val << 6;
    val = soc_mem_field32_get(unit, mem, &entry, IEEE1588_TIME_FRAC_SEC_LOWERf);
    tod->ts.nanoseconds |= (val >> 26) & 0x3F;

    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                            "Current PTP: (%d) sec (%d) ns\n"),
                COMPILER_64_LO(tod->ts.seconds),
                tod->ts.nanoseconds));
    return BCM_E_NONE;
}

STATIC int
bcmi_time_tod_local_ntp_set(
    int unit,
    bcm_time_tod_t *tod)
{
    uint32 sec, regval = 0;
    uint64 frac, tmp;

    if (tod == NULL) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_use_local_tod)) {
        return BCM_E_UNAVAIL;
    }

    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                            "Configure NTP: (%d) sec (%d) ns\n"),
                COMPILER_64_LO(tod->ts.seconds),
                tod->ts.nanoseconds));

    WRITE_TS_TO_CORE_SYNC_ENABLEr(unit, 1);
    sec = COMPILER_64_LO(tod->ts.seconds);

    soc_reg_field_set(unit, NTP_TIME_SECr, &regval,
                      ONE_SECf, sec % 2);
    sec = (sec >> 1) & 0x7FFFFFFF;
    soc_reg_field_set(unit, NTP_TIME_SECr, &regval,
                      UPPER_SECf, sec);
    BCM_IF_ERROR_RETURN(
        WRITE_NTP_TIME_SECr(unit, sec));

    COMPILER_64_SET(frac, 0, tod->ts.nanoseconds);

    /* (1 << 32) / 1000000000 */
    COMPILER_64_SET(tmp, 0, 1);
    COMPILER_64_SHL(tmp, 32);
    COMPILER_64_UDIV_32(tmp, 1000000000);

    /* frac x (1 << 32) / 1000000000 */
    COMPILER_64_UMUL_32(frac, COMPILER_64_LO(tmp));

    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                            "frac : High(0x%x) Low(0x%x)\n"),
                COMPILER_64_HI(frac),
                COMPILER_64_LO(frac)));

    BCM_IF_ERROR_RETURN(
        WRITE_NTP_TIME_FRAC_SEC_UPPERr(unit, COMPILER_64_LO(frac) >> 6));
    BCM_IF_ERROR_RETURN(
        WRITE_NTP_TIME_FRAC_SEC_LOWERr(unit, COMPILER_64_LO(frac) << 26));
    BCM_IF_ERROR_RETURN(
        READ_NTP_TIME_CONTROLr(unit, &regval));

    /* Clear counter enable. */
    BCM_IF_ERROR_RETURN(
        READ_NTP_TIME_CONTROLr(unit, &regval));
    soc_reg_field_set(unit, NTP_TIME_CONTROLr, &regval,
                      COUNT_ENABLEf, 0);
    BCM_IF_ERROR_RETURN(
        WRITE_NTP_TIME_CONTROLr(unit, regval));

    /*
     * Set load-enable bits, |B04...B00|.
     * LOAD_ENABLE = 2'b11111 = 0x1F
     */
    soc_reg_field_set(unit, NTP_TIME_CONTROLr, &regval,
                      LOAD_ENABLEf, 0x1F);
    BCM_IF_ERROR_RETURN(
        WRITE_NTP_TIME_CONTROLr(unit, regval));

    /* Clear load-enable. */
    soc_reg_field_set(unit, NTP_TIME_CONTROLr, &regval,
                      LOAD_ENABLEf, 0);
    BCM_IF_ERROR_RETURN(
        WRITE_NTP_TIME_CONTROLr(unit, regval));

    BCM_IF_ERROR_RETURN(
        WRITE_NTP_TIME_FREQ_CONTROLr(unit, 0x44B82FA0));

    /* Set counter enable. */
    soc_reg_field_set(unit, NTP_TIME_CONTROLr, &regval,
                      COUNT_ENABLEf, 1);
    BCM_IF_ERROR_RETURN(
        WRITE_NTP_TIME_CONTROLr(unit, regval));
    return BCM_E_NONE;
}

STATIC int
bcmi_time_tod_local_ptp_set(
    int unit,
    bcm_time_tod_t *tod)
{
    uint32 sec, regval = 0;

    if (tod == NULL) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_use_local_tod)) {
        return BCM_E_UNAVAIL;
    }

    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                            "Configure PTP: (%d) sec (%d) ns\n"),
                COMPILER_64_LO(tod->ts.seconds),
                tod->ts.nanoseconds));

    WRITE_TS_TO_CORE_SYNC_ENABLEr(unit, 1);
    sec = COMPILER_64_LO(tod->ts.seconds);
    soc_reg_field_set(unit, IEEE1588_TIME_SECr, &regval,
                      ONE_SECf, sec % 2);
    sec = (sec >> 1) & 0x7FFFFFFF;
    soc_reg_field_set(unit, IEEE1588_TIME_SECr, &regval,
                      UPPER_SECf, sec);
    BCM_IF_ERROR_RETURN(
        WRITE_IEEE1588_TIME_SECr(unit, sec));
    BCM_IF_ERROR_RETURN(
        WRITE_IEEE1588_TIME_FRAC_SEC_UPPERr(unit,
                                            tod->ts.nanoseconds >> 6));
    BCM_IF_ERROR_RETURN(
        WRITE_IEEE1588_TIME_FRAC_SEC_LOWERr(unit,
                                            tod->ts.nanoseconds << 26));
    BCM_IF_ERROR_RETURN(
        READ_IEEE1588_TIME_CONTROLr(unit, &regval));

    /* Clear counter enable. */
    BCM_IF_ERROR_RETURN(
        READ_IEEE1588_TIME_CONTROLr(unit, &regval));
    soc_reg_field_set(unit, IEEE1588_TIME_CONTROLr, &regval,
                      COUNT_ENABLEf, 0);
    BCM_IF_ERROR_RETURN(
        WRITE_IEEE1588_TIME_CONTROLr(unit, regval));

    /*
     * Set load-enable bits, |B04...B00|.
     * LOAD_ENABLE = 2'b11111 = 0x1F
     */
    soc_reg_field_set(unit, IEEE1588_TIME_CONTROLr, &regval,
                      LOAD_ENABLEf, 0x1F);
    BCM_IF_ERROR_RETURN(
        WRITE_IEEE1588_TIME_CONTROLr(unit, regval));

    /* Clear load-enable. */
    soc_reg_field_set(unit, IEEE1588_TIME_CONTROLr, &regval,
                      LOAD_ENABLEf, 0);
    BCM_IF_ERROR_RETURN(
        WRITE_IEEE1588_TIME_CONTROLr(unit, regval));

    BCM_IF_ERROR_RETURN(
        WRITE_IEEE1588_TIME_FREQ_CONTROLr(unit, 0x010000000));

    /* Set counter enable. */
    soc_reg_field_set(unit, IEEE1588_TIME_CONTROLr, &regval,
                      COUNT_ENABLEf, 1);
    BCM_IF_ERROR_RETURN(
        WRITE_IEEE1588_TIME_CONTROLr(unit, regval));
    return BCM_E_NONE;
}

STATIC int
bcmi_time_tod_local_tod_get(
    int unit,
    bcm_time_tod_t *tod)
{
    if (tod == NULL) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_use_local_tod)) {
        return BCM_E_UNAVAIL;
    }
#if LOCAL_SW_TOD
     if (tod->time_format == bcmTimeFormatNTP ||
         tod->time_format == bcmTimeFormatPTP) {
        return bcmi_time_tod_local_ptp_ntp_get(unit, tod);
    }
#else
    if (tod->time_format == bcmTimeFormatNTP) {
        return bcmi_time_tod_local_ntp_get(unit, tod);
    } else if (tod->time_format == bcmTimeFormatPTP) {
        return bcmi_time_tod_local_ptp_get(unit, tod);
    }
#endif
    return BCM_E_NONE;
}

STATIC int
bcmi_time_tod_local_tod_set(
    int unit,
    bcm_time_tod_t *tod)
{
    if (tod == NULL) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_use_local_tod)) {
        return BCM_E_UNAVAIL;
    }
    if (tod->time_format == bcmTimeFormatNTP) {
        return bcmi_time_tod_local_ntp_set(unit, tod);
    } else if (tod->time_format == bcmTimeFormatPTP) {
        return bcmi_time_tod_local_ptp_set(unit, tod);
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}
#endif /* BCM_TIME_LOCAL_TOD_SUPPORT */

typedef enum bcmi_time_tod_stage_e {
    bcmi_time_tod_stage_ingress_ft = 0,
    bcmi_time_tod_stage_egress = 1,
    bcmi_time_tod_stage_live_wire_ptp = 2,
    bcmi_time_tod_stage_live_wire_ntp = 3,
    bcmi_time_tod_stage_count = 4
} bcmi_time_tod_stage_t;

#if defined(BCM_TIMESYNC_LIVE_WIRE_TOD_SUPPORT)
/*
 * Function:
 *      bcmi_time_tod_iproc_ptp_get
 * Purpose:
 *      Internal function to get PTP tod values from iProc.
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      tod     - (OUT) Time of the day values.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_time_tod_iproc_ptp_get(
    int unit,
    bcm_time_tod_t *tod)
{
    uint32 regVal = 0, val, data_sec[2];

    if (tod == NULL) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_timesync_live_wire_tod)) {
        return BCM_E_UNAVAIL;
    }

    /* Set timestamp format. */
    tod->time_format = bcmTimeFormatPTP;

    /*
     * A read of PTP_TOD_VALUE_2 causes the full 80b of the PTP.TOD to be
     * latched, and the upper 16b returned on this read. The subsequent reads
     * of PTP_TOD_VALUE_1 and PTP_TOD_VALUE_0 will return the value for the
     * relevant field that were latch on PTP_TOD_VALUE_2 read.
     */

    sal_memset(data_sec, 0, 2 * sizeof(uint32));

    /* PTP.TOD[79:64] Value for CPU read. */
    BCM_TIME_READ_NS_REGr(unit, NS_PTP_TOD_A_VALUE_2r, 0, &regVal);
    data_sec[1] = soc_reg_field_get(unit, NS_PTP_TOD_A_VALUE_2r, regVal, VALf);

    /* PTP.TOD[63:32] Value for CPU read. */
    BCM_TIME_READ_NS_REGr(unit, NS_PTP_TOD_A_VALUE_1r, 0, &regVal);
    data_sec[0] = soc_reg_field_get(unit, NS_PTP_TOD_A_VALUE_1r, regVal, VALf);

    COMPILER_64_SET(tod->ts.seconds, data_sec[1], data_sec[0]);

    /* PTP.TOD[31:0] Value for CPU read */
    BCM_TIME_READ_NS_REGr(unit, NS_PTP_TOD_A_VALUE_0r, 0, &regVal);
    val = soc_reg_field_get(unit, NS_PTP_TOD_A_VALUE_0r, regVal, VALf);

    tod->ts.nanoseconds = val;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_time_tod_iproc_ntp_get
 * Purpose:
 *      Internal function to get NTP tod values from iProc.
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      tod     - (OUT) Time of the day values.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_time_tod_iproc_ntp_get(
    int unit,
    bcm_time_tod_t *tod)
{
    uint32 regVal = 0, val, data_sec[2];

    if (tod == NULL) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, soc_feature_timesync_live_wire_tod)) {
        return BCM_E_UNAVAIL;
    }

    /* Set timestamp format. */
    tod->time_format = bcmTimeFormatNTP;

    /*
     * A read of NTP_TOD_VALUE_1 causes the full 64b of the NTP.TOD to be
     * latched, and the upper 32b returned on this read. The subsequent read of
     * NTP_TOD_VALUE_0 will return the value for the lower 32b field that were
     * latch on NTP_TOD_VALUE_1 read.
     */

    sal_memset(data_sec, 0, 2 * sizeof(uint32));

    /* NTP.TOD[63:32] Value for CPU read. */
    BCM_TIME_READ_NS_REGr(unit, NS_NTP_TOD_VALUE_1r, 0, &regVal);
    data_sec[0] = soc_reg_field_get(unit, NS_NTP_TOD_VALUE_1r, regVal, VALf);

    COMPILER_64_SET(tod->ts.seconds, data_sec[1], data_sec[0]);

    /* NTP.TOD[31:0] Value for CPU read */
    BCM_TIME_READ_NS_REGr(unit, NS_NTP_TOD_VALUE_0r, 0, &regVal);
    val = soc_reg_field_get(unit, NS_NTP_TOD_VALUE_0r, regVal, VALf);

    tod->ts.nanoseconds = val;

    return BCM_E_NONE;
}
#endif /* BCM_TIMESYNC_LIVE_WIRE_TOD_SUPPORT */

/*
 * Function:
 *      bcmi_time_tod_set
 * Purpose:
 *      Internal function to update tod values.
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      tod     - (IN)  Time of the day values.
 *      stage   - (IN)  Stage where time needs to be updated.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_time_tod_set(
        int unit,
        bcm_time_tod_t *tod,
        bcmi_time_tod_stage_t stage)
{
    soc_mem_t mem = INVALIDm;
    uint32 ts_entry[SOC_MAX_MEM_WORDS];
    uint64 check;
    uint32 u64_hi = 0xffff, u64_low = 0xffffffff;
    uint32 data_sec[2], data_nsec[2];
    int rv = 0;

    if (stage == bcmi_time_tod_stage_egress) {
        mem = EGR_TS_UTC_CONVERSIONm;
    } else
#if defined(BCM_TIME_LOCAL_TOD_SUPPORT)
    if (stage == bcmi_time_tod_stage_ingress_ft &&
        soc_feature(unit, soc_feature_use_local_tod)) {
        return bcmi_time_tod_local_tod_set(unit, tod);
    } else
#endif /* BCM_TIME_LOCAL_TOD_SUPPORT */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if ((stage == bcmi_time_tod_stage_ingress_ft) &&
        soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        mem = BSC_TS_UTC_CONVERSIONm;
    } else
#endif /* BCM_FLOWTRACKER_SUPPORT */
#if defined(BCM_TIMESYNC_LIVE_WIRE_TOD_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_live_wire_tod) &&
        (stage == bcmi_time_tod_stage_live_wire_ptp ||
         stage == bcmi_time_tod_stage_live_wire_ntp)) {
        /* NTP_TOD and PTP_TOD are read-only */
        return BCM_E_UNAVAIL;
    } else
#endif /* BCM_TIMESYNC_LIVE_WIRE_TOD_SUPPORT */
    {
       return BCM_E_UNAVAIL;
    }

    sal_memset(&ts_entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
    COMPILER_64_SET(check, u64_hi, u64_low);

    if (COMPILER_64_GT(tod->ts.seconds, check)) {
        return BCM_E_PARAM;
    }
    if (COMPILER_64_GT(tod->ts_adjustment_counter_ns, check)) {
        return BCM_E_PARAM;
    }
#if defined(BCM_FLOWTRACKER_SUPPORT)
    /* Set timestamp format. */
    if (soc_mem_field_valid(unit, mem, TIMESTAMP_FORMATf)) {
        soc_mem_field32_set(unit, mem, (uint32 *)&ts_entry, TIMESTAMP_FORMATf,
                tod->time_format);
    }
#endif /* BCM_FLOWTRACKER_SUPPORT */
    /* Set Nanoseconds. */
    if (soc_mem_field_valid(unit, mem, TIMESTAMP_TOD_NSECf)) {
        soc_mem_field32_set(unit, mem, (uint32 *)&ts_entry, TIMESTAMP_TOD_NSECf,
                tod->ts.nanoseconds);
    }

    sal_memset(data_sec, 0, 2*sizeof(uint32));
    COMPILER_64_TO_32_LO(data_sec[0], tod->ts.seconds);
    COMPILER_64_TO_32_HI(data_sec[1], tod->ts.seconds);

    if (soc_mem_field_valid(unit, mem, TIMESTAMP_TOD_SECf)) {
        soc_mem_field_set(unit, mem, (uint32 *)&ts_entry, TIMESTAMP_TOD_SECf,
                data_sec);
    }

    sal_memset(data_nsec, 0, 2*sizeof(uint32));
    COMPILER_64_TO_32_LO(data_nsec[0], tod->ts_adjustment_counter_ns);
    COMPILER_64_TO_32_HI(data_nsec[1], tod->ts_adjustment_counter_ns);

    if (soc_mem_field_valid(unit, mem, TIMESTAMP_TA_NSECf)) {
        soc_mem_field_set(unit, mem, (uint32 *)&ts_entry, TIMESTAMP_TA_NSECf,
                data_nsec);
    }

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0,  &ts_entry);

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if ((SOC_IS_TOMAHAWK3(unit)) && (!BCM_FAILURE(rv))) {
        soc_mem_t mem_2 = EGR_TS_UTC_CONVERSION_2m;

        if (soc_mem_field_valid(unit, mem_2, TIMESTAMP_TOD_NSECf)) {
            soc_mem_field32_set(unit, mem_2, (uint32 *)&ts_entry, TIMESTAMP_TOD_NSECf,
                    tod->ts.nanoseconds);
        }
        if (soc_mem_field_valid(unit, mem_2, TIMESTAMP_TOD_SECf)) {
            soc_mem_field_set(unit, mem_2, (uint32 *)&ts_entry, TIMESTAMP_TOD_SECf,
                    data_sec);
        }
        if (soc_mem_field_valid(unit, mem_2, TIMESTAMP_TA_NSECf)) {
            soc_mem_field_set(unit, mem_2, (uint32 *)&ts_entry, TIMESTAMP_TA_NSECf,
                    data_nsec);
        }

        rv = soc_mem_write(unit, mem_2, MEM_BLOCK_ALL, 0,  &ts_entry);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    return rv;
}

/*
 * Function:
 *      bcmi_time_tod_get
 * Purpose:
 *      Internal function to get tod values for stage.
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      tod     - (OUT) Time of the day values.
 *      stage   - (IN)  Stage where time is from.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_time_tod_get(
        int unit,
        bcm_time_tod_t *tod,
        bcmi_time_tod_stage_t stage)

{
    soc_mem_t mem = INVALIDm;
    uint32 ts_entry[SOC_MAX_MEM_WORDS];
    uint32 data[2];

    if (stage == bcmi_time_tod_stage_egress) {
        mem = EGR_TS_UTC_CONVERSIONm;
    } else
#if defined(BCM_TIME_LOCAL_TOD_SUPPORT)
    if (stage == bcmi_time_tod_stage_ingress_ft &&
        soc_feature(unit, soc_feature_use_local_tod)) {
        return bcmi_time_tod_local_tod_get(unit, tod);
    } else
#endif /* BCM_TIME_LOCAL_TOD_SUPPORT */
#if defined(BCM_FLOWTRACKER_SUPPORT)
    if ((stage == bcmi_time_tod_stage_ingress_ft) &&
        soc_feature(unit, soc_feature_flex_flowtracker_ver_1)) {
        mem = BSC_TS_UTC_CONVERSIONm;
    } else
#endif /* BCM_FLOWTRACKER_SUPPORT */
#if defined(BCM_TIMESYNC_LIVE_WIRE_TOD_SUPPORT)
    if (soc_feature(unit, soc_feature_timesync_live_wire_tod) &&
        stage == bcmi_time_tod_stage_live_wire_ptp) {
        return bcmi_time_tod_iproc_ptp_get(unit, tod);
    } else if (soc_feature(unit, soc_feature_timesync_live_wire_tod) &&
               stage == bcmi_time_tod_stage_live_wire_ntp) {
        return bcmi_time_tod_iproc_ntp_get(unit, tod);
    } else
#endif /* BCM_TIMESYNC_LIVE_WIRE_TOD_SUPPORT */
    {
       return BCM_E_UNAVAIL;
    }

    sal_memset(&ts_entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0, &ts_entry));

#if defined(BCM_FLOWTRACKER_SUPPORT)
    /* Set timestamp format. */
    if (soc_mem_field_valid(unit, mem, TIMESTAMP_FORMATf)) {
        tod->time_format = soc_mem_field32_get(unit, mem, (uint32 *)&ts_entry,
                TIMESTAMP_FORMATf);
    }
#endif /* BCM_FLOWTRACKER_SUPPORT */
    /* Set Nanoseconds. */
    if (soc_mem_field_valid(unit, mem, TIMESTAMP_TOD_NSECf)) {
        tod->ts.nanoseconds = soc_mem_field32_get(unit, mem, (uint32 *)&ts_entry,
                TIMESTAMP_TOD_NSECf);
    }

    sal_memset(data, 0, 2*sizeof(uint32));

    if (soc_mem_field_valid(unit, mem, TIMESTAMP_TOD_SECf)) {
        soc_mem_field_get(unit, mem, (uint32 *)&ts_entry, TIMESTAMP_TOD_SECf,
                data);
    }

    COMPILER_64_SET(tod->ts.seconds, data[1], data[0]);

    sal_memset(data, 0, 2*sizeof(uint32));

    if (soc_mem_field_valid(unit, mem, TIMESTAMP_TA_NSECf)) {
        soc_mem_field_get(unit, mem, (uint32 *)&ts_entry, TIMESTAMP_TA_NSECf,
                (uint32 *)&data);
    }

    COMPILER_64_SET(tod->ts_adjustment_counter_ns, data[1], data[0]);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_time_tod_set
 * Purpose:
 *      API to update tod values for stage.
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      stages  - (IN)  Stage where time needs to be updated.
 *      tod     - (IN)  Time of the day values.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_time_tod_set(
        int unit,
        uint32 stages,
        bcm_time_tod_t *tod)
{

    int rv = BCM_E_UNAVAIL;

    if (tod == NULL) {
        return BCM_E_PARAM;
    }

#if (defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
     defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT) || \
     defined(BCM_TIME_LOCAL_TOD_SUPPORT))
    if (stages & BCM_TIME_STAGE_ALL) {
        bcmi_time_tod_stage_t stage;
        uint8 set_cnt = 0;
        for (stage = bcmi_time_tod_stage_ingress_ft;
             stage < bcmi_time_tod_stage_count;
             stage++) {
            if (stage == bcmi_time_tod_stage_live_wire_ptp ||
                stage == bcmi_time_tod_stage_live_wire_ntp) {
                /* NTP_TOD and PTP_TOD are read-only */
                continue;
            }
#if defined(BCM_FLOWTRACKER_SUPPORT)
            if ((stage == bcmi_time_tod_stage_ingress_ft) &&
                (!soc_feature(unit, soc_feature_flex_flowtracker_ver_1))) {
                /* Flowtracker module is not enabled */
                continue;
            }
#endif
            rv = bcmi_time_tod_set(unit, tod, stage);
            if (BCM_SUCCESS(rv)) {
                set_cnt++;
            }
        }
        if (set_cnt == 0) {
            /* Loop all and cannot set a successful one. */
            return BCM_E_UNAVAIL;
        } else {
            return BCM_E_NONE;
        }
    } else if (stages & BCM_TIME_STAGE_EGRESS) {
        rv = bcmi_time_tod_set(unit, tod, bcmi_time_tod_stage_egress);
    }  else if (stages & BCM_TIME_STAGE_INGRESS_FLOWTRACKER) {
        rv = bcmi_time_tod_set(unit, tod, bcmi_time_tod_stage_ingress_ft);
    } else if (stages & BCM_TIME_STAGE_LIVE_WIRE_PTP) {
        rv = bcmi_time_tod_set(unit, tod, bcmi_time_tod_stage_live_wire_ptp);
    } else if (stages & BCM_TIME_STAGE_LIVE_WIRE_NTP) {
        rv = bcmi_time_tod_set(unit, tod, bcmi_time_tod_stage_live_wire_ntp);
    } else {
        return BCM_E_PARAM;
    }
#endif

    return rv;
}

/*
 * Function:
 *      bcm_esw_time_tod_get
 * Purpose:
 *      API to get tod values for stage.
 * Parameters:
 *      unit    - (IN)  Unit number.
 *      stages  - (IN)  Stage where time is from.
 *      tod     - (OUT) Time of the day values.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_esw_time_tod_get(
        int unit,
        uint32 stages,
        bcm_time_tod_t *tod)
{
    int rv = BCM_E_UNAVAIL;

    if (tod == NULL) {
        return BCM_E_PARAM;
    }

#if (defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) || \
     defined(BCM_TOMAHAWK2_SUPPORT)|| defined(BCM_MAVERICK2_SUPPORT) || \
     defined(BCM_TIME_LOCAL_TOD_SUPPORT))
    if (stages & BCM_TIME_STAGE_ALL) {
        bcmi_time_tod_stage_t stage;
        for (stage = bcmi_time_tod_stage_ingress_ft;
             stage < bcmi_time_tod_stage_count;
             stage++) {
#if defined(BCM_FLOWTRACKER_SUPPORT)
            if ((stage == bcmi_time_tod_stage_ingress_ft) &&
                (!soc_feature(unit, soc_feature_flex_flowtracker_ver_1))) {
                /* Flowtracker module is not enabled */
                continue;
            }
#endif
            rv = bcmi_time_tod_get(unit, tod, stage);
            if (BCM_SUCCESS(rv)) {
                return BCM_E_NONE;
            }
        }
        if (stage == bcmi_time_tod_stage_count) {
            /* Loop all and cannot get a successful one. */
            return BCM_E_UNAVAIL;
        }
    } else if (stages & BCM_TIME_STAGE_EGRESS) {
        rv = bcmi_time_tod_get(unit, tod, bcmi_time_tod_stage_egress);
    }  else if (stages & BCM_TIME_STAGE_INGRESS_FLOWTRACKER) {
        rv = bcmi_time_tod_get(unit, tod, bcmi_time_tod_stage_ingress_ft);
    } else if (stages & BCM_TIME_STAGE_LIVE_WIRE_PTP) {
        rv = bcmi_time_tod_get(unit, tod, bcmi_time_tod_stage_live_wire_ptp);
    } else if (stages & BCM_TIME_STAGE_LIVE_WIRE_NTP) {
        rv = bcmi_time_tod_get(unit, tod, bcmi_time_tod_stage_live_wire_ntp);
    } else {
        return BCM_E_PARAM;
    }
#endif

    return rv;
}

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
/* Check if _bcm_time_capture_event_t is supported or not. */
STATIC int
bcmi_time_capture_event_supported(
    int unit,
    _bcm_time_capture_event_t event)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return _bcm_time_capture_event_supported_th3[event];
    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
    if (SOC_IS_HURRICANE4(unit)) {
        return _bcm_time_capture_event_supported_hr4[event];
    } else
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
    if (SOC_IS_FIREBOLT6(unit)) {
        return _bcm_time_capture_event_supported_fb6[event];
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        return _bcm_time_capture_event_supported_fl[event];
    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
    {
        return _bcm_time_capture_event_supported_mo[event];
    }
}

int _bcm_esw_get_event_config_from_time_capture(
        int unit,
        bcm_time_capture_t *time_capture,
        _bcm_time_capture_input_t *time_capture_input)
{
    if (time_capture == NULL ||
        time_capture_input == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "get_event_config_from_time_capture: Error !! "
                              "Wrong param\n")));
        return BCM_E_PARAM;
    }

    time_capture_input->event_dest = _bcmTimeCaptureDestCPU;
    time_capture_input->event_divisor = time_capture->event_divisor;
    time_capture_input->port = time_capture->port;
    time_capture_input->port_event_type = time_capture->port_event_type;
    time_capture_input->ppm_check_enable = 0; /* Not valid for FIFO timestamp capture */
    time_capture_input->ts_counter = time_capture->ts_counter;

    switch (time_capture->flags){
    case BCM_TIME_CAPTURE_LOCKED:
    case BCM_TIME_CAPTURE_L1_CLOCK_PRIMARY:
    case BCM_TIME_CAPTURE_L1_CLOCK_SECONDARY:
    case BCM_TIME_CAPTURE_IMMEDIATE:
        bsl_printf("Unable to find mapping for flag set in time_capture\n");
        return BCM_E_NOT_FOUND;
    case BCM_TIME_CAPTURE_GPIO_0:
        time_capture_input->input_event = _bcmTimeCaptureEventGPIO0;
        break;
    case BCM_TIME_CAPTURE_GPIO_1:
        time_capture_input->input_event = _bcmTimeCaptureEventGPIO1;
        break;
    case BCM_TIME_CAPTURE_GPIO_2:
        time_capture_input->input_event = _bcmTimeCaptureEventGPIO2;
        break;
    case BCM_TIME_CAPTURE_GPIO_3:
        time_capture_input->input_event = _bcmTimeCaptureEventGPIO3;
        break;
    case BCM_TIME_CAPTURE_GPIO_4:
        time_capture_input->input_event = _bcmTimeCaptureEventGPIO4;
        break;
    case BCM_TIME_CAPTURE_GPIO_5:
        time_capture_input->input_event = _bcmTimeCaptureEventGPIO5;
        break;
    case BCM_TIME_CAPTURE_LCPLL:
        time_capture_input->input_event = _bcmTimeCaptureEventLCPLL0;
        break;
    case BCM_TIME_CAPTURE_IP_DM_0:
        time_capture_input->input_event = _bcmTimeCaptureEventIPDM0;
        break;
    case BCM_TIME_CAPTURE_IP_DM_1:
        time_capture_input->input_event = _bcmTimeCaptureEventIPDM1;
        break;
    case BCM_TIME_CAPTURE_BS0HB:
        time_capture_input->input_event = _bcmTimeCaptureEventBS0HB;
        break;
    case BCM_TIME_CAPTURE_BS1HB:
        time_capture_input->input_event = _bcmTimeCaptureEventBS1HB;
        break;
    case BCM_TIME_CAPTURE_TS1TS:
        time_capture_input->input_event = _bcmTimeCaptureEventTS1TS;
        break;
    case BCM_TIME_CAPTURE_BS0PLL:
        time_capture_input->input_event = _bcmTimeCaptureEventBS0PLL;
        break;
    case BCM_TIME_CAPTURE_BS1PLL:
        time_capture_input->input_event = _bcmTimeCaptureEventBS1PLL;
        break;
    case BCM_TIME_CAPTURE_RSVD1IF:
        time_capture_input->input_event = _bcmTimeCaptureEventRSVD1IF;
        break;
    case BCM_TIME_CAPTURE_RSVD1RF:
        time_capture_input->input_event = _bcmTimeCaptureEventRSVD1RF;
        break;
    case BCM_TIME_CAPTURE_BS0CONVTC:
        time_capture_input->input_event = _bcmTimeCaptureEventBS0ConvTC;
        break;
    case BCM_TIME_CAPTURE_BS1CONVTC:
        time_capture_input->input_event = _bcmTimeCaptureEventBS1ConvTC;
        break;
    case BCM_TIME_CAPTURE_LCPLL1:
        time_capture_input->input_event = _bcmTimeCaptureEventLCPLL1;
        break;
    case BCM_TIME_CAPTURE_LCPLL2:
        time_capture_input->input_event = _bcmTimeCaptureEventLCPLL2;
        break;
    case BCM_TIME_CAPTURE_LCPLL3:
        time_capture_input->input_event = _bcmTimeCaptureEventLCPLL3;
        break;
    case BCM_TIME_CAPTURE_PORT:
        time_capture_input->input_event = _bcmTimeCaptureEventPORT;
        break;
    }

    if (!bcmi_time_capture_event_supported(unit,
                                           time_capture_input->input_event)) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                  (BSL_META_U(unit,
                              "Unable to find mapping for flag set"
                              "in time_capture. "
                              "Not supported event(%d) \n"),
                              time_capture_input->input_event));
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}


STATIC int _bcm_esw_time_portmap_get(int unit, bcm_port_t port, int *pTimePort)
{
    int rv  = BCM_E_NONE;

    if (pTimePort == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "time_portmap_get: Error !! Wrong param\n")));
        return BCM_E_PARAM;
    }

    /* Following is the mapping in Monterey */
    if ((port >= 1) && (port <= BCM_TIME_CAPTURE_NUM_PORTS)) {
        *pTimePort = port - 1;
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_bcm_esw_time_portmap_get: Error !! Invalid port\n")));
        *pTimePort = BCM_TIME_CAPTURE_NUM_PORTS;
        rv = BCM_E_PARAM;
    }

    return rv;
}

STATIC int
_bcm_esw_time_input_eventId_get(int unit,
                    _bcm_time_capture_input_t *pInputEvent,
                    uint32 *pEventId)
{
    int rv  = BCM_E_NONE;
    int idx = 0;

    if ((pEventId == NULL) ||
        (pInputEvent == NULL) ||
        (pInputEvent->input_event < _bcmTimeCaptureEventCpu) ||
        (pInputEvent->input_event >= _bcmTimeCaptureEventMax)) {
         LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_eventId_get: Error !! Wrong event\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (!bcmi_time_capture_event_supported(unit, pInputEvent->input_event)) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                  (BSL_META_U(unit,
                              "input_eventId_get: Error !! "
                              "Not supported event(%d) \n"),
                              pInputEvent->input_event));
        return BCM_E_UNAVAIL;
    }

    /* EventId 0 to 191 for port events,
     * EventId 192 to 213 for EventCpu to EventLCPLL3
     * EventId 214 for R5
     */
    switch ((int)pInputEvent->input_event) {
        case _bcmTimeCaptureEventPORT:
            if (pInputEvent->port < 0 || pInputEvent->port > BCM_TIME_CAPTURE_NUM_PORTS){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_eventId_get: Error !! Wrong port\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }

            if ((pInputEvent->port_event_type == bcmTimePortEventRXCDR) ||
                (pInputEvent->port_event_type == bcmTimePortEventRXSOF)){
                /* Event could be TXSOF or TXPI based on port type */
                idx = pInputEvent->port*2;
            } else if (pInputEvent->port_event_type == bcmTimePortEventROE) {
                idx = pInputEvent->port*2 + 1;
            } else if ((pInputEvent->port_event_type == bcmTimePortEventTXSOF) ||
                       (pInputEvent->port_event_type == bcmTimePortEventTXPI)){
                /* Event could be TXSOF or TXPI based on port type */
                idx = BCM_TIME_CAPTURE_IA_START_TXPI_TXSOF + pInputEvent->port;
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_eventId_get: Error !! Wrong param\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }
            break;

        case _bcmTimeCaptureEventR5:
            idx = BCM_TIME_CAPTURE_IA_START_R5;
            break;

        default:
            idx = BCM_TIME_CAPTURE_IA_START_MISC + pInputEvent->input_event;
    }

    *pEventId = idx;

exit:
    return rv;
}

STATIC int
_bcm_esw_time_event_divisor_misc_set(int unit, _bcm_time_capture_event_t event_misc,
                            uint32 divisor)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;
    soc_reg_t ns_reg;

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return (BCM_E_UNAVAIL);
    }

    if (!bcmi_time_capture_event_supported(unit, event_misc)) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                  (BSL_META_U(unit,
                              "event_divisor_misc_set: Error !! "
                              "Not supported event(%d) \n"),
                              event_misc));
        return BCM_E_UNAVAIL;
    }

    switch (event_misc) {
        case _bcmTimeCaptureEventCpu:
        case _bcmTimeCaptureEventBS0HB:
        case _bcmTimeCaptureEventBS1HB:
        case _bcmTimeCaptureEventIPDM0:
        case _bcmTimeCaptureEventIPDM1:
        case _bcmTimeCaptureEventTS1TS:
        case _bcmTimeCaptureEventRSVD1IF:
        case _bcmTimeCaptureEventRSVD1RF:
        case _bcmTimeCaptureEventBS0ConvTC:
        case _bcmTimeCaptureEventBS1ConvTC:
            /* No divider */
            bsl_printf("BCM_TIME: event_divisor_misc_set: No divider for event:%d \n", event_misc);
            break;

        case _bcmTimeCaptureEventGPIO0:
        case _bcmTimeCaptureEventGPIO1:
        case _bcmTimeCaptureEventGPIO2:
        case _bcmTimeCaptureEventGPIO3:
        case _bcmTimeCaptureEventGPIO4:
        case _bcmTimeCaptureEventGPIO5:
            idx = event_misc - _bcmTimeCaptureEventGPIO0;
            {
                if (SOC_REG_IS_VALID(unit, _bcm_time_gpio_iregs[idx])) {
                    ns_reg = _bcm_time_gpio_iregs[idx];
                } else if (SOC_REG_IS_VALID(unit,
                                            _bcm_time_gpio_regs[idx])) {
                    ns_reg = _bcm_time_gpio_regs[idx];
                } else {
                   rv = BCM_E_UNAVAIL;
                   break;
                }
                BCM_TIME_READ_NS_REGr(unit, ns_reg, 0, &regVal);
                soc_reg_field_set(unit,  ns_reg, &regVal, DIVISORf, divisor);
                BCM_TIME_WRITE_NS_REGr(unit, ns_reg, 0, regVal);
            }
            break;

        case _bcmTimeCaptureEventBS0PLL:
        case _bcmTimeCaptureEventBS1PLL:
            idx = event_misc - _bcmTimeCaptureEventBS0PLL;
            {
                if (SOC_REG_IS_VALID(unit, _bcm_time_bs_pll_iregs[idx])) {
                    ns_reg = _bcm_time_bs_pll_iregs[idx];
                } else if (SOC_REG_IS_VALID(unit,
                                            _bcm_time_bs_pll_regs[idx])) {
                    ns_reg = _bcm_time_bs_pll_regs[idx];
                } else {
                    rv = BCM_E_UNAVAIL;
                    break;
                }
                BCM_TIME_READ_NS_REGr(unit, ns_reg, 0, &regVal);
                soc_reg_field_set(unit, ns_reg, &regVal, DIVISORf, divisor);
                BCM_TIME_WRITE_NS_REGr(unit, ns_reg, 0, regVal);
            }
            break;

        case _bcmTimeCaptureEventLCPLL0:
        case _bcmTimeCaptureEventLCPLL1:
        case _bcmTimeCaptureEventLCPLL2:
        case _bcmTimeCaptureEventLCPLL3:
            if (SOC_REG_IS_VALID(unit,
                                 IPROC_NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr)) {
                ns_reg = IPROC_NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr;
            } else if (SOC_REG_IS_VALID(unit,
                                        NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr)) {
                ns_reg = NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr;
            } else {
               rv = BCM_E_UNAVAIL;
               break;
            }

            idx = event_misc - _bcmTimeCaptureEventLCPLL0;
            {
                BCM_TIME_READ_NS_REGr(unit, ns_reg, idx, &regVal);
                soc_reg_field_set(unit, ns_reg, &regVal, DIVISORf, divisor);
                BCM_TIME_WRITE_NS_REGr(unit, ns_reg, idx, regVal);
            }
            break;

        default:
           LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_misc_set: Error !! wrong param\n")));
           rv = BCM_E_PARAM;
           goto exit;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_time_event_dest_misc_set(int unit, _bcm_time_capture_event_t event_misc,
                            _bcm_time_capture_dest_cfg_t *pEventDest,
                            uint32 eventId)
{
    int rv  = BCM_E_NONE;
    int idx = 0, misc_start;
    uint32 regVal = 0;
    uint32 dest;
    uint32 chkProfile;

    if (pEventDest == NULL ||
        event_misc < _bcmTimeCaptureEventCpu ||
        event_misc > _bcmTimeCaptureEventLCPLL3) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_misc_set: Error !! Wrong param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (!bcmi_time_capture_event_supported(unit, event_misc)) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                  (BSL_META_U(unit,
                              "capture_enable_misc_set: Error !! "
                              "Not supported event(%d) \n"),
                              event_misc));
        rv = BCM_E_UNAVAIL;
        goto exit;
    }

    BCM_IF_ERROR_RETURN(
        bcmi_time_capture_event_misc_events_range(
            unit, &misc_start, NULL));

    if (SOC_IS_HURRICANE4(unit)) {
        if (pEventDest->event_dest == _bcmTimeCaptureDestCPU) {
            dest = 0;
        } else if (pEventDest->event_dest == _bcmTimeCaptureDestALL) {
            dest = 3;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_dest_misc_set: Error !! wrong event dest\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
    } else {
        if (pEventDest->event_dest == _bcmTimeCaptureDestCPU) {
            dest = 0;
        } else if (pEventDest->event_dest == _bcmTimeCaptureDestM7) {
            dest = 1;
        } else if (pEventDest->event_dest == _bcmTimeCaptureDestALL) {
            dest = 2;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_dest_misc_set: Error !! wrong event dest\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
    }
    idx = misc_start + event_misc;

    
    chkProfile = 0;

    if (SOC_REG_IS_VALID(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr)) {
        BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, &regVal);
        if (SOC_REG_FIELD_VALID(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                EVENT_IDf)) {
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
        }

        if (SOC_REG_FIELD_VALID(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                DESTf)) {
            dest |= soc_reg_field_get(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                      regVal, DESTf);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DESTf, dest);
        }

        if (SOC_REG_FIELD_VALID(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                DPLL_ADDRf)) {
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DPLL_ADDRf, eventId);
        }

        if (SOC_REG_FIELD_VALID(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                GDPLL_TSf)) {
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              GDPLL_TSf,
                              (pEventDest->ts_counter) ? 1 : 0);
        }

        if (SOC_REG_FIELD_VALID(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                PPM_CHECK_ENABLEf)) {
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf,
                              (pEventDest->ppm_check_enable) ? 1 : 0);
        }

        if (SOC_REG_FIELD_VALID(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                CHK_PROFILEf)) {
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
        }
        BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, regVal);
    } else {
        rv = BCM_E_UNAVAIL;
        goto exit;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_time_event_divisor_port_set(int unit, bcm_port_t port,
    bcm_time_port_event_t port_event_type, uint32 divisor)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;

    if (port > BCM_TIME_CAPTURE_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_port_set: Error !! wrong port \n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    idx = port;
    switch (port_event_type) {
        case bcmTimePortEventRXSOF:
        case bcmTimePortEventTXSOF:
        case bcmTimePortEventROE:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_port_set: Error !! No divisor\n")));
            rv = BCM_E_UNAVAIL;
            break;

        case bcmTimePortEventRXCDR:
            {
                BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, &regVal);
                soc_reg_field_set(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, &regVal,
                                DIVISORf, divisor);
                BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, regVal);
            }
            break;

        case bcmTimePortEventTXPI:
            {
                BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, idx, &regVal);
                soc_reg_field_set(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, &regVal,
                                DIVISORf, divisor);
                BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, idx, regVal);
            }
            break;

        default:
           LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_port_set: Error !! Wrong param\n")));
           rv = BCM_E_PARAM;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_time_event_dest_port_set (int unit, bcm_port_t port,
                bcm_time_port_event_t port_event_type,
                _bcm_time_capture_dest_cfg_t *pEventDest,
                uint32 eventId)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;
    uint32 dest;
    uint32 chkProfile;

    if (pEventDest == NULL ||
        port >= BCM_TIME_CAPTURE_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_port_set: Error !! wrong param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }


    if (pEventDest->event_dest == _bcmTimeCaptureDestCPU) {
        dest = 0;
    }else if (pEventDest->event_dest == _bcmTimeCaptureDestM7) {
        dest = 1;
    }else if (pEventDest->event_dest == _bcmTimeCaptureDestALL) {
        dest = 2;
    }else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_port_set: Error !! wrong event dest\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    
    chkProfile = 0;

    switch(port_event_type) {
        case bcmTimePortEventRXCDR:
        case bcmTimePortEventTXPI:
            if (port_event_type == bcmTimePortEventRXCDR){
                idx = port;
            }else if (port_event_type == bcmTimePortEventTXPI) {
                idx = port + BCM_TIME_CAPTURE_NUM_PORTS;
            }

            {
                bsl_printf("BCM_TIME: Setting NS_TIMESYNC_TS_EVENT_FWD_CFG idx:%d\n",
                            idx);

                BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, &regVal);
                soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                                EVENT_IDf, eventId);
                dest |= soc_reg_field_get(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                        regVal, DESTf);
                soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                                DESTf, dest);
                /*soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                                DPLL_ADDRf, *pIaAddr);*/
                soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                                GDPLL_TSf, pEventDest->ts_counter ? 1:0);
                soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                                PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
                soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                                SRC_TS_COUNTERf, pEventDest->ts_counter ? 1:0);
                soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                                CHK_PROFILEf, chkProfile);
                BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, regVal);
            }

            break;

        case bcmTimePortEventRXSOF:    /* Type-0 */
        case bcmTimePortEventTXSOF:    /* Type-1 */
        case bcmTimePortEventROE:      /* Type-2 */
            /*
             * Having 128 Numels, register map is
             * Port-0: type-0, type-1, type-2, unused
             * Port-1: type-0, type-1, type-2, unused
             */
            if ((port >= BCM_TIME_CAPTURE_NUM_CPRI_PORTS) || (port < 0)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_dest_port_set: Error !! Invalid CPRI port\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }

            idx = port*4;
            if (port_event_type == bcmTimePortEventTXSOF) {
                idx = idx + 1;
            }else if (port_event_type == bcmTimePortEventROE) {
                idx = idx + 2;
            }

            {
                bsl_printf("BCM_TIME: Setting NS_TIMESYNC_MAPPER_FWD_CFG idx:%d\n",
                            idx);
                BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, idx, &regVal);
                soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                                EVENT_IDf, eventId);
                dest |= soc_reg_field_get(unit, NS_TIMESYNC_MAPPER_FWD_CFGr,
                                        regVal, DESTf);
                soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                                DESTf, dest);
                /*soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                                DPLL_ADDRf, *pIaAddr);*/
                soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                                PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
                soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                                SRC_TS_COUNTERf, 1);  /* It is TS1 counter */
                soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                                CHK_PROFILEf, chkProfile);
                BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, idx, regVal);
            }
            break;

        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_dest_port_set: Error !! wrong event\n")));
            rv = BCM_E_PARAM;
            goto exit;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_time_event_roe_52b_set(int unit, bcm_port_t port, int enable)
{
    int rv = BCM_E_NONE;
    uint32 enable_field = 0;
    uint32 regVal = 0;
    uint32 reg_enable = 0;

    /* Program NS_TIMESYNC_MAPPER_TYPE2_FORMAT */

    enable_field = 1 << port;

    {
        BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, port, &regVal);
        reg_enable = soc_reg_field_get(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, regVal, CONV_ENf);

        reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

        soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, &regVal, CONV_ENf, reg_enable);
        BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, port, regVal);
    }

    return (rv);
}

STATIC int
_bcm_esw_time_capture_enable_misc_set(int unit, _bcm_time_capture_event_t  event_misc,
                               int enable)
{
    int rv = BCM_E_NONE;
    uint32 idx = 0;
    uint32 enable_field = 0;
    uint32 regVal = 0;
    uint32 reg_enable;

    if (!bcmi_time_capture_event_supported(unit, event_misc)) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                  (BSL_META_U(unit,
                              "capture_enable_misc_set: Error !! "
                              "Not supported event(%d) \n"),
                              event_misc));
        return BCM_E_UNAVAIL;
    }

    /*
    NS_MISC_CLK_EVENT_CTRL
    NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRL
    NS_BROADSYNC0_CLK_EVENT_CTRL
    NS_BROADSYNC1_CLK_EVENT_CTRL
    NS_TIMESYNC_GPIO_x_CTRL
     */
    switch (event_misc){
        case _bcmTimeCaptureEventCpu:
            if (event_misc == _bcmTimeCaptureEventCpu) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_CPU;
            }
        case _bcmTimeCaptureEventBS0HB:
            if (event_misc == _bcmTimeCaptureEventBS0HB) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_BS0HB;
            }
        case _bcmTimeCaptureEventBS1HB:
            if (event_misc == _bcmTimeCaptureEventBS1HB) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_BS1HB;
            }
        case _bcmTimeCaptureEventIPDM0:
            if (event_misc == _bcmTimeCaptureEventIPDM0) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_IPDM0;
            }
        case _bcmTimeCaptureEventIPDM1:
            if (event_misc == _bcmTimeCaptureEventIPDM1) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_IPDM1;
            }
        case _bcmTimeCaptureEventTS1TS:
            if (event_misc == _bcmTimeCaptureEventTS1TS) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_TS1;
            }
        case _bcmTimeCaptureEventRSVD1IF:
            if (event_misc == _bcmTimeCaptureEventRSVD1IF) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_RSVD1IF;
            }
        case _bcmTimeCaptureEventRSVD1RF:
            if (event_misc == _bcmTimeCaptureEventRSVD1RF) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_RSVD1RF;
            }
        case _bcmTimeCaptureEventBS0ConvTC:
            if (event_misc == _bcmTimeCaptureEventBS0ConvTC) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_BS0CONV;
            }
        case _bcmTimeCaptureEventBS1ConvTC:
            if (event_misc == _bcmTimeCaptureEventBS1ConvTC) {
                enable_field = BCM_TIME_CAPTURE_MISC_EVENT_EN_BS1CONV;
            }

            {
                /* Write the enable flag */
                BCM_TIME_READ_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, &regVal);
                reg_enable = soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr, regVal, ENABLEf);

                reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

                soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal,
                                ENABLEf, reg_enable);
                BCM_TIME_WRITE_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, regVal);
            }
            break;

        case _bcmTimeCaptureEventGPIO0:
        case _bcmTimeCaptureEventGPIO1:
        case _bcmTimeCaptureEventGPIO2:
        case _bcmTimeCaptureEventGPIO3:
        case _bcmTimeCaptureEventGPIO4:
        case _bcmTimeCaptureEventGPIO5:
            idx = event_misc - _bcmTimeCaptureEventGPIO0;
            {
                BCM_TIME_READ_NS_REGr(unit, _bcm_time_gpio_regs[idx], 0, &regVal);
#ifndef INCLUDE_GDPLL
                soc_reg_field_set(unit,  _bcm_time_gpio_regs[idx], &regVal,
                               CAPTURE_ENABLEf, enable ? 1:0);
#endif
                BCM_TIME_WRITE_NS_REGr(unit, _bcm_time_gpio_regs[idx], 0, regVal);
}
            break;

        case _bcmTimeCaptureEventBS0PLL:
        case _bcmTimeCaptureEventBS1PLL:
            idx = event_misc - _bcmTimeCaptureEventBS0PLL;
            {
                BCM_TIME_READ_NS_REGr(unit, _bcm_time_bs_pll_regs[idx], 0, &regVal);
                soc_reg_field_set(unit, _bcm_time_bs_pll_regs[idx], &regVal,
                                ENABLEf, enable ? 1:0);
                BCM_TIME_WRITE_NS_REGr(unit, _bcm_time_bs_pll_regs[idx], 0, regVal);
            }
            break;

        case _bcmTimeCaptureEventLCPLL0:
        case _bcmTimeCaptureEventLCPLL1:
        case _bcmTimeCaptureEventLCPLL2:
        case _bcmTimeCaptureEventLCPLL3:
            idx = event_misc - _bcmTimeCaptureEventLCPLL0;
            {
                BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, &regVal);
                soc_reg_field_set(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, &regVal,
                                ENABLEf, enable ? 1:0);
                BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, regVal);
            }
            break;

        case _bcmTimeCaptureEventPORT:
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_enable_misc_set: Error !! wrong event\n")));
            rv = BCM_E_PARAM;
            goto exit;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_time_capture_enable_port_set(int unit, bcm_port_t port, bcm_time_port_event_t port_event_type,
                                 int enable)
{
    int rv = BCM_E_NONE;
    uint32 mapper_reg;
    uint32 regVal = 0;
    uint32 enable_field = 0;
    uint32 reg_enable = 0;

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return (BCM_E_UNAVAIL);
    }

    if (port >= BCM_TIME_CAPTURE_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "event_enable_port_set: Error !! wrong port\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    switch (port_event_type) {
        case bcmTimePortEventRXSOF:
        case bcmTimePortEventTXSOF:
        case bcmTimePortEventROE:
            /* NS has support to 32 CPRI ports */
            if ((port >= BCM_TIME_CAPTURE_NUM_CPRI_PORTS) || (port < 0)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_enable_port_set: Error !! wrong port or non-cpri\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }

            /* Program NS_TIMESYNC_MAPPER_PORT_ENABLE_x */
            enable_field = 1 << (port_event_type - bcmTimePortEventRXSOF);
            mapper_reg = port/8;

            {
                BCM_TIME_READ_NS_REGr(unit, __bcm_time_mapper_port_enable_regs[mapper_reg], 0, &regVal);
                reg_enable = soc_reg_field_get(unit, __bcm_time_mapper_port_enable_regs[mapper_reg], regVal, _bcm_time_mapper_port_enable[port]);

                reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

                soc_reg_field_set(unit, __bcm_time_mapper_port_enable_regs[mapper_reg], &regVal, _bcm_time_mapper_port_enable[port], reg_enable);
                BCM_TIME_WRITE_NS_REGr(unit, __bcm_time_mapper_port_enable_regs[mapper_reg], 0, regVal);
            }

            break;

        case bcmTimePortEventRXCDR:
            {
                /* Program NS_TIMESYNC_RX_CDR_EVENT_CTRL */
                BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr,
                                      port, &regVal);
                soc_reg_field_set(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, &regVal,
                                  ENABLEf, enable ? 1 : 0);
                BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr,
                                       port, regVal);
            }
            break;

        case bcmTimePortEventTXPI:
            /* Program NS_TIMESYNC_TX_PI_CLK_EVENT_CTRL */
            {
                BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, port, &regVal);
                soc_reg_field_set(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, &regVal, ENABLEf, enable ? 1:0);
                BCM_TIME_WRITE_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, port, regVal);
            }
            break;

        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "event_enable_port_set: Error !! wrong port event\n")));
            rv = BCM_E_PARAM;
          goto exit;
    }

exit:
    return (rv);
}

STATIC int
_bcm_esw_time_capture_enable_cpu_set(int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    if (SOC_REG_IS_VALID(unit, NS_MISC_CLK_EVENT_CTRLr)) {
        BCM_TIME_READ_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, &regVal);
        if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr,
                                TIME_CAPTURE_ENABLEf)) {
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal,
                              TIME_CAPTURE_ENABLEf, enable ? 1 : 0);
        }
        if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr,
                                CPU_FIFO1_CAPTURE_ENABLEf)) {
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal,
                              CPU_FIFO1_CAPTURE_ENABLEf, enable ? 1 : 0);
        }
        if (SOC_REG_FIELD_VALID(unit, NS_MISC_CLK_EVENT_CTRLr,
                                CPU_FIFO2_CAPTURE_ENABLEf)) {
            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal,
                              CPU_FIFO2_CAPTURE_ENABLEf, enable ? 1 : 0);
        }
        BCM_TIME_WRITE_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, regVal);
    }

    return (rv);
}

STATIC int
bcm_esw_time_capture_enable(int unit, _bcm_time_capture_input_t event_misc, int enable)
{
    int rv = BCM_E_NONE;

    bsl_printf("BCM TIME: Set enable for time capture event\n");
    if (event_misc.input_event == _bcmTimeCaptureEventPORT) {
        _bcm_esw_time_capture_enable_port_set(unit, event_misc.port, event_misc.port_event_type, enable);
    } else {
        _bcm_esw_time_capture_enable_misc_set(unit, event_misc.input_event, enable);
    }

    _bcm_esw_time_capture_enable_cpu_set(unit,enable);
    return (rv);
}

STATIC int
_bcm_esw_time_event_config_set(int unit, _bcm_time_capture_input_t *pTimeCapture, uint32 eventId)
{
    int rv = BCM_E_NONE;
    _bcm_time_capture_dest_cfg_t eventDest;

    if (pTimeCapture == NULL) {
        return BCM_E_PARAM;
    }

    if (!bcmi_time_capture_event_supported(unit, pTimeCapture->input_event)) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                  (BSL_META_U(unit,
                              "event_config_set: Error !! "
                              "Not supported event(%d) \n"),
                              pTimeCapture->input_event));
        return BCM_E_UNAVAIL;
    }

    /* Configure the event */
    eventDest.event_dest = pTimeCapture->event_dest;
    eventDest.ts_counter = pTimeCapture->ts_counter;
    eventDest.ppm_check_enable = pTimeCapture->ppm_check_enable;

    if ((pTimeCapture->input_event >= _bcmTimeCaptureEventCpu) &&
        (pTimeCapture->input_event <= _bcmTimeCaptureEventLCPLL3)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_time_event_divisor_misc_set(unit,
                            pTimeCapture->input_event,
                            pTimeCapture->event_divisor));
        BCM_IF_ERROR_RETURN(_bcm_esw_time_event_dest_misc_set(unit,
                            pTimeCapture->input_event,
                            &eventDest, eventId));

    } else if(pTimeCapture->input_event == _bcmTimeCaptureEventPORT) {
        if ((pTimeCapture->port_event_type == bcmTimePortEventRXCDR) ||
            (pTimeCapture->port_event_type == bcmTimePortEventTXPI)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_time_event_divisor_port_set(unit,
                            pTimeCapture->port, pTimeCapture->port_event_type,
                            pTimeCapture->event_divisor));
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_time_event_dest_port_set(unit, pTimeCapture->port,
                            pTimeCapture->port_event_type,
                            &eventDest, eventId));

        /* ROE event, disable the 52b conversion SWIPROC-591 */
        if (pTimeCapture->port_event_type == bcmTimePortEventROE)
            BCM_IF_ERROR_RETURN(_bcm_esw_time_event_roe_52b_set(unit,
                            pTimeCapture->port, 0));

    } /*else if(pTimeCapture->input_event == _bcmTimeCaptureEventR5) {*/
        
         /*pIaAddr = 214;
    } */else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "event_config_set: Error !! invalid input event\n")));
        rv = BCM_E_PARAM;
    }

    return (rv);
}

#endif /* BCM_TIME_NANOSYNC_SUPPORT */

int bcm_esw_time_capture_enable_set(
        int unit,
        bcm_time_if_t id,
        bcm_time_capture_t *time_capture,
        int enable,
        int *capture_handle)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
    _bcm_time_capture_input_t time_capture_input;
    int ts_port;
    uint32 eventId = BCM_TIME_CAPTURE_HANDLE_INVALID;

    if (time_capture == NULL ||
        capture_handle == NULL) {
        return (BCM_E_UNAVAIL);
    }

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return (BCM_E_UNAVAIL);
    }

    rv = _bcm_esw_get_event_config_from_time_capture(
            unit, time_capture, &time_capture_input);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    if (time_capture_input.input_event == _bcmTimeCaptureEventPORT) {
        _bcm_esw_time_portmap_get(unit, time_capture_input.port, &ts_port);
        time_capture_input.port = ts_port;
    }

    if (_bcm_esw_time_input_eventId_get(unit,
            &time_capture_input,
            &eventId) != BCM_E_NONE) {
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    if (eventId >= BCM_TIME_CAPTURE_MAX_EVENT_ID){
        bsl_printf("BCM TIME: Internal error. Invalid capture_handle: %d", eventId);
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    if (_bcm_time_capture_event_state[eventId] == BCM_TIME_CAPTURE_EVENT_STATE_NOT_IN_USE &&
            enable == 0) {
        bsl_printf("BCM_TIME: Error! Trying to disable a capture handle that is not enabled");
        rv = BCM_E_PARAM;
        goto exit;
    } else if (_bcm_time_capture_event_state[eventId] == BCM_TIME_CAPTURE_EVENT_STATE_IN_USE &&
            enable == 1) {
        bsl_printf("BCM_TIME: Error! Trying to enable a capture handle that is in use");
        rv = BCM_E_PARAM;
        goto exit;
    }

    _bcm_time_capture_event_state[eventId] = (enable)?BCM_TIME_CAPTURE_EVENT_STATE_IN_USE:BCM_TIME_CAPTURE_EVENT_STATE_NOT_IN_USE;

    bsl_printf("Capture handle generated:%d\n", eventId);
    /*bsl_printf("event_dest: %d, event_divisor: %d, input_event: %d, port: %d, port_event_type: %d, ppm_check_enable: %d, ts_counter: %d\n",time_capture.event_dest,
            time_capture.event_divisor,
            time_capture.input_event,
            time_capture.port,
            time_capture.port_event_type,
            time_capture.ppm_check_enable,
            time_capture.ts_counter);*/

    /* Configure the input event and get iaAddr_ref */
    if(_bcm_esw_time_event_config_set(unit,
            &time_capture_input,
            eventId) != BCM_E_NONE) {
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    rv = bcm_esw_time_capture_enable(unit, time_capture_input, enable);

exit:
    *capture_handle = eventId;
#endif /* BCM_TIME_NANOSYNC_SUPPORT */
    return (rv);
}

#if defined(BCM_TIME_NANOSYNC_SUPPORT) || defined (BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
typedef struct bcm_time_cb_context_s {
    bcm_time_capture_cb cb;
    bcm_time_capture_cb_type_t cb_type;
    void    *user_data_ptr;
}bcm_time_cb_context_t;

bcm_time_cb_context_t cb_overflow,cb_fill,cb_all, cb_ts0_ctr[BCM_UNITS_MAX], cb_ts1_ctr[BCM_UNITS_MAX];

#endif /* BCM_TIME_NANOSYNC_SUPPORT || BCM_TIMESYNC_TIME_CAPTURE_SUPPORT */

#if defined(BCM_TIME_NANOSYNC_SUPPORT)
uint32 g_int_status = 0;  /* Interrupt status */
uint8 g_fifo_data_fill[BCM_TIME_CAPTURE_MAX_FIFO_DATA_SIZE];
uint8 g_fifo_data_overflow[2];
sal_sem_t    g_fifo_sem;
#endif /* BCM_TIME_NANOSYNC_SUPPORT */

#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT) || defined(BCM_TIME_NANOSYNC_SUPPORT)
typedef struct bcm_esw_time_ts_capture_info_s {
    int time_capture_ts_enable;
    int time_capture_ts_active;
    sal_thread_t thread_id_time_capture_ts;
    sal_sem_t ts_fifo_sem;
    uint32 ts_fifo_wr_index;
    uint32 ts_fifo_rd_index;
    uint8 ts_fifo_data_fill[BCM_TIME_TS_CAPTURE_BUF_SIZE];

    uint32 intr_stats_tsfifo_not_empty;
    uint32 intr_stats_tsfifo_overflow;
    uint32 intr_stats_tsfifo_invalid;
} bcm_esw_time_ts_capture_info_t;

STATIC bcm_esw_time_ts_capture_info_t *ts_capture_info[BCM_UNITS_MAX] = {0};
#endif /* BCM_TIMESYNC_TIME_CAPTURE_SUPPORT || BCM_TIME_NANOSYNC_SUPPORT */

int bcm_esw_time_capture_cb_register(
        int unit,
        bcm_time_if_t id,
        bcm_time_capture_cb_type_t cb_type,
        bcm_time_capture_cb cb,
        void *user_data)
{
    int rv = BCM_E_NONE;
#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT) || defined(BCM_TIME_NANOSYNC_SUPPORT)
    if (cb_type != bcmTimeCaptureTS0CntOffsetUpdated && cb_type != bcmTimeCaptureTS1CntOffsetUpdated){
        if (cb_overflow.cb == NULL && cb_fill.cb == NULL && cb_all.cb == NULL){
        /* At least one callback is registered. Enable the interrupt */
#if defined(BCM_TIME_NANOSYNC_SUPPORT)
        if (SOC_IS_MONTEREY(unit) || SOC_IS_TOMAHAWK3(unit)|| SOC_IS_HURRICANE4(unit)) {
            uint32 regval;
            BCM_TIME_READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &regval);
            regval |= INTR_NS_TIME_CAPTURE;
            BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, regval);
        }
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            soc_cmic_intr_enable(unit, TS_INTR);
        }
#endif
#endif /*  BCM_TIME_NANOSYNC_SUPPORT */


#if defined (BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
        if (soc_feature(unit, soc_feature_timesync_time_capture)) {
            (void)soc_cmicm_intr1_enable(unit, IRQ_CMCx_TIMESYNC_INTR);
        }

        if (soc_feature(unit, soc_feature_timesync_time_capture) ||
            soc_feature(unit, soc_feature_nanosync_time_capture)) {
            ts_capture_info[unit]->ts_fifo_rd_index = 0;
            ts_capture_info[unit]->ts_fifo_wr_index = 0;
            sal_memset(ts_capture_info[unit]->ts_fifo_data_fill, 0, BCM_TIME_TS_CAPTURE_BUF_SIZE);
        }
#endif
        }
    }
    switch (cb_type){
    case bcmTimeCaptureOverflow:
        if (cb_overflow.cb != NULL){
            bsl_printf("BCM TIME: Error! Callback function already registered for bcmTimeCaptureOverflow");
            rv = BCM_E_PARAM;
            goto exit;
        }
        cb_overflow.cb_type = bcmTimeCaptureOverflow;
        cb_overflow.cb = cb;
        cb_overflow.user_data_ptr = user_data;
        break;
    case bcmTimeCapturetFill:
        if (cb_fill.cb != NULL){
            bsl_printf("BCM TIME: Error! Callback function already registered for bcmTimeCapturetFill");
            rv = BCM_E_PARAM;
            goto exit;
        }
        cb_fill.cb_type = bcmTimeCapturetFill;
        cb_fill.cb = cb;
        cb_fill.user_data_ptr = user_data;
        break;
    case bcmTimeCaptureAll:
        if (cb_all.cb != NULL){
            bsl_printf("BCM TIME: Error! Callback function already registered for bcmTimeCaptureAll");
            rv = BCM_E_PARAM;
            goto exit;
        }
        cb_all.cb_type = bcmTimeCaptureAll;
        cb_all.cb = cb;
        cb_all.user_data_ptr = user_data;
        break;
    case bcmTimeCaptureTS0CntOffsetUpdated:
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            if (cb_ts0_ctr[unit].cb == NULL){
                uint32 regval;
                BCM_TIME_READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &regval);
                regval |= INTR_TS0_CNT_OFFSET_UPDATED;
                BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, regval);
            }
        }
#endif
        if (cb_ts0_ctr[unit].cb != NULL){
            bsl_printf("BCM TIME: Error! Callback function already registered for bcmTimeCaptureTS0CntOffsetUpdated");
            rv = BCM_E_PARAM;
            goto exit;
        }
        cb_ts0_ctr[unit].cb_type = bcmTimeCaptureTS0CntOffsetUpdated;
        cb_ts0_ctr[unit].cb = cb;
        cb_ts0_ctr[unit].user_data_ptr = user_data;
        break;
    case bcmTimeCaptureTS1CntOffsetUpdated:
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            if (cb_ts1_ctr[unit].cb == NULL){
                uint32 regval;
                BCM_TIME_READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &regval);
                regval |= INTR_TS1_CNT_OFFSET_UPDATED;
                BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, regval);
            }
        }
#endif
        if (cb_ts1_ctr[unit].cb != NULL){
            bsl_printf("BCM TIME: Error! Callback function already registered for bcmTimeCaptureTS1CntOffsetUpdated");
            rv = BCM_E_PARAM;
            goto exit;
        }
        cb_ts1_ctr[unit].cb_type = bcmTimeCaptureTS1CntOffsetUpdated;
        cb_ts1_ctr[unit].cb = cb;
        cb_ts1_ctr[unit].user_data_ptr = user_data;
        break;
    }
exit:
#endif /* #if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) */
    return rv;
}


int bcm_esw_time_capture_cb_unregister(
        int unit,
        bcm_time_if_t id,
        bcm_time_capture_cb_type_t cb_type)
{
#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT) || defined(BCM_TIME_NANOSYNC_SUPPORT)
    switch (cb_type){
    case bcmTimeCaptureOverflow:
        cb_overflow.cb_type = bcmTimeCaptureOverflow;
        cb_overflow.cb = NULL;
        cb_overflow.user_data_ptr = NULL;
        break;
    case bcmTimeCapturetFill:
        cb_fill.cb_type = bcmTimeCapturetFill;
        cb_fill.cb = NULL;
        cb_fill.user_data_ptr = NULL;
        break;
    case bcmTimeCaptureAll:
        cb_all.cb_type = bcmTimeCaptureAll;
        cb_all.cb = NULL;
        cb_all.user_data_ptr = NULL;
        break;
    case bcmTimeCaptureTS0CntOffsetUpdated:
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            uint32 regval;
            BCM_TIME_READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &regval);
            regval &= ~((uint32)INTR_TS0_CNT_OFFSET_UPDATED);
            BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, regval);
        }
#endif
        cb_ts0_ctr[unit].cb_type = bcmTimeCaptureTS0CntOffsetUpdated;
        cb_ts0_ctr[unit].cb = NULL;
        cb_ts0_ctr[unit].user_data_ptr = NULL;
        break;
    case bcmTimeCaptureTS1CntOffsetUpdated:
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            uint32 regval;
            BCM_TIME_READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &regval);
            regval &= ~((uint32)INTR_TS1_CNT_OFFSET_UPDATED);
            BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, regval);
        }
#endif
        cb_ts1_ctr[unit].cb_type = bcmTimeCaptureTS1CntOffsetUpdated;
        cb_ts1_ctr[unit].cb = NULL;
        cb_ts1_ctr[unit].user_data_ptr = NULL;
        break;
    }

    if (cb_overflow.cb == NULL && cb_fill.cb == NULL && cb_all.cb == NULL){
        /* No callbacks are registered anymore. Disable interrupts */
#if defined(BCM_TIME_NANOSYNC_SUPPORT)
        if (soc_feature(unit, soc_feature_timesync_nanosync)) {
            if (SOC_REG_IS_VALID(unit, NS_TS_INT_ENABLEr)) {
            uint32 regval;
            BCM_TIME_READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &regval);
            regval &= ~((uint32)INTR_NS_TIME_CAPTURE);
            BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, regval);
            } else {
            BCM_TIME_WRITE_NS_REGr(unit, IPROC_NS_TS_INT_ENABLEr, 0, 0);
            }
        }
#endif
    }
    if (cb_overflow.cb == NULL && cb_fill.cb == NULL && cb_all.cb == NULL){
        /* At least one callback is registered. Enable the interrupt */
#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
        if (soc_feature(unit, soc_feature_timesync_time_capture)) {
            (void)soc_cmicm_intr1_disable(unit, IRQ_CMCx_TIMESYNC_INTR);
        }
#endif
    }
#endif /* #if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT) */

    return BCM_E_NONE;
}

void
soc_nanosync_intr(void *unit_vp, void *d1, void *d2,
                  void *d3, void *d4)
{
#if defined(BCM_TIME_NANOSYNC_SUPPORT)
    int unit = PTR_TO_INT(unit_vp);
    int valid = 0;

    uint32 int_status_clear = 0;  /* Interrupt status clear */
    uint32 int_enable = 0;  /* Interrupt enable */
    uint32 regval,regval1,regval2,regval3,lower, middle, upper,event_id, fifo_depth, capture_status;

    uint16 g_fifo_data_index = 0;

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return;
    }

    {
        /* Read the GDPLL interrupt */
        BCM_TIME_READ_NS_REGr(unit, NS_TS_INT_STATUSr, 0, &g_int_status);
        BCM_TIME_READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &int_enable);

        /* Process only enabled interrupts */
        g_int_status &= int_enable;

       /* TS0_CNT_OFFSET_UPDATED interrupt */
        if (tsfifo_flag && soc_reg_field_get(unit, NS_TS_INT_STATUSr, g_int_status, TS0_CNT_OFFSET_UPDATEDf)) {

            /* Set W1TC to clear the flag */
            soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS0_CNT_OFFSET_UPDATEDf, 1);
            /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
            BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_STATUSr, 0, int_status_clear);
        }

        /* TS1_CNT_OFFSET_UPDATED interrupt */
        if (tsfifo_flag && soc_reg_field_get(unit, NS_TS_INT_STATUSr, g_int_status, TS1_CNT_OFFSET_UPDATEDf)) {

            /* Set W1TC to clear the flag */
            soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS1_CNT_OFFSET_UPDATEDf, 1);
            /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
            BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_STATUSr, 0, int_status_clear);
        }

        /* TS_FIFO_NOT_EMPTY interrupt */
        if (tsfifo_flag && soc_reg_field_get(unit, NS_TS_INT_STATUSr, g_int_status, TS_FIFO_NOT_EMPTYf)) {

            /* Set W1TC to clear the flag */
            soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS_FIFO_NOT_EMPTYf, 1);
            /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
            BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_STATUSr, 0, int_status_clear);

            BCM_TIME_READ_NS_REGr(unit, NS_TS_CAPTURE_STATUSr, 0, &capture_status);
            fifo_depth = soc_reg_field_get(unit, NS_TS_CAPTURE_STATUSr, capture_status, FIFO_DEPTHf);
            g_fifo_data_fill[g_fifo_data_index++] = 0xFF & fifo_depth;
            g_fifo_data_fill[g_fifo_data_index++] = 0x3 & (fifo_depth >> 8);
            /*bsl_printf("FIFO_Depth: %d\n", fifo_depth);*/

            while (fifo_depth != 0) {
                /* Must read LOWER regiser first */
                /* Read to this register POPs the FIFO */
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r(unit, &regval3);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_2r(unit, &regval2);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r(unit, &regval1);
                READ_NS_TIMESYNC_INPUT_TIME_FIFO_TS_0r(unit, &regval);



                /* Check capture event id */
                event_id = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r,
                                regval3, TS_EVENT_IDf);

                valid = (regval3 >> 20) & 1;

                if (!valid){
                    continue;
                }

#if 0
                /* Calculate seconds/nanoseconds from the timestamp value */
                lower = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_0r,
                                regval, TS0_Lf);

                upper = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r,
                                regval1, TS0_Uf);

                bsl_printf("FIFO timestamp popped. EventId: %d, TS0: %x%x\n", event_id, upper, lower);

                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & event_id;
                g_fifo_data_fill[g_fifo_data_index++] = 0x0;
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & lower;
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & (lower >> 8);
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & (lower >> 16);
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & (lower >> 24);
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & upper;
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & (upper >> 8);
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & (upper >> 16);
#endif

                /* Calculate seconds/nanoseconds from the timestamp value */
                lower = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r,
                                regval1, TS1_Lf);

                middle = soc_reg_field_get(
                                            unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_2r,
                                            regval2, TS1_Mf);

                upper = soc_reg_field_get(
                                unit, NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r,
                                regval3, TS1_Uf);

                lower = (lower & 0xFFF) | ((middle & 0xFFFFF) << 12);
                upper = ((upper & 0xFF) << 12) | ((middle >> 20) & 0xFFF);


                /*bsl_printf("FIFO timestamp popped. EventId: %d, TS1: %x%x\n", event_id, upper, lower);*/

                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & event_id;
                g_fifo_data_fill[g_fifo_data_index++] = 0x0;
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & lower;
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & (lower >> 8);
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & (lower >> 16);
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & (lower >> 24);
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & upper;
                g_fifo_data_fill[g_fifo_data_index++] = 0xFF & (upper >> 8);
                g_fifo_data_fill[g_fifo_data_index++] = 0xF & (upper >> 16);
                fifo_depth--;
            }
        }

        /* TS_FIFO_OVERFLOW interrupt */
        if (tsfifo_flag && soc_reg_field_get(unit, NS_TS_INT_STATUSr, g_int_status, TS_FIFO_OVERFLOWf)) {
            g_fifo_data_index = 0;

            /* Set W1TC to clear the flag */
            soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS_FIFO_OVERFLOWf, 1);
            /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
            BCM_TIME_WRITE_NS_REGr(unit, NS_TS_INT_STATUSr, 0, int_status_clear);

            BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_INPUT_TIME_FIFO_STATUSr, 0, &capture_status);
            event_id = soc_reg_field_get(unit, NS_TIMESYNC_INPUT_TIME_FIFO_STATUSr,
                                        capture_status, SOURCE_OVERFLOW_TYPEf);
            g_fifo_data_overflow[g_fifo_data_index++] = 0xFF & event_id;

        }
    }

#if defined(INCLUDE_GDPLL)
    gdpll_intr(unit, g_int_status, int_enable);
#endif

    /* Signal the thread for buffer read */
    if (tsfifo_flag && g_fifo_sem) {
        sal_sem_give(g_fifo_sem);
    }
    /* Re-enable TS NS IRQ */
    /*  soc_cmicm_intr2_enable(unit, SOC_TIME_NS_INTR_POS); */
    soc_cmicm_intr2_enable(unit, (1<<24) | (1<<25));/* SOC_GDPLL_NS_INTR_POS | SOC_GDPLL_NS_INTR_DEBUG_POS */

#endif /* BCM_TIME_NANOSYNC_SUPPORT */
}

#if defined (BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
void
soc_esw_timesync_ts_intr(int unit)
{

    uint32 int_enable = 0;  /* Interrupt enable */
    uint32 regval1=0, lower=0, upper=0, event_id=0;
    uint32 fifo_depth=0, capture_status=0, fifo_status=0;
    uint32 intr=0, over_flow=0;
    uint16 notify=0;
    uint32 int_status = 0;  /* Interrupt status */
    int buf_full=0, buf_idx=0;

    if (!soc_feature(unit, soc_feature_timesync_time_capture)) {
        return;
    }

    READ_CMIC_TIMESYNC_INTERRUPT_STATUSr(unit, &int_status);

    READ_CMIC_TIMESYNC_CAPTURE_STATUS_1r(unit, &capture_status);

    /* TS_FIFO_NOT_EMPTY interrupt */
    intr = soc_reg_field_get(unit, CMIC_TIMESYNC_INTERRUPT_STATUSr, int_status, INTERRUPTf);
    if (intr)
	{

        READ_CMIC_TIMESYNC_FIFO_STATUSr(unit, &fifo_status);
        over_flow = soc_reg_field_get(unit, CMIC_TIMESYNC_FIFO_STATUSr,
                                    fifo_status, SOURCE_OVERFLOWf);
        if(over_flow) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "TS FIFO status source overflow \n")));
            soc_reg_field_set(unit, CMIC_TIMESYNC_FIFO_STATUSr, &fifo_status, SOURCE_OVERFLOWf, 0);
            WRITE_CMIC_TIMESYNC_FIFO_STATUSr(unit, fifo_status);
        }

        fifo_depth = soc_reg_field_get(unit, CMIC_TIMESYNC_CAPTURE_STATUS_1r, capture_status, FIFO_DEPTHf);
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "FIFO depth[%d]\n"),fifo_depth));

        while (fifo_depth != 0) {
            notify = 1;
            /* Must read LOWER regiser first */
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(unit, &lower);
            READ_CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(unit, &regval1);

            /* Check capture event id */
            event_id = soc_reg_field_get(
                            unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                            regval1, EVENT_IDf);

            /* Calculate seconds/nanoseconds from the timestamp value */
            upper = soc_reg_field_get(
                            unit, CMIC_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr,
                            regval1, TIMESTAMPf);

            /* LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "[%d]:%d:%x%x\n"),fifo_depth, event_id, upper, lower));*/

            buf_idx = (ts_capture_info[unit]->ts_fifo_wr_index);
            buf_full = ((buf_idx + 12) >= BCM_TIME_TS_CAPTURE_BUF_SIZE);
            if (!buf_full) {

            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x1;

            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & event_id;
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0F & (upper >> 16);
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (upper >> 8);
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & upper;
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 24);
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 16);
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 8);
            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & lower;

            ts_capture_info[unit]->ts_fifo_wr_index = buf_idx;
            }
            fifo_depth--;
        }

        if(over_flow) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "[%d]:%d:%x%x \n"),fifo_depth, event_id, upper, lower));
        }

        /* Clear status */
        WRITE_CMIC_TIMESYNC_CAPTURE_STATUS_CLR_1r(unit, capture_status);

        READ_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, &int_enable);

        /* Clear interrupts */
        WRITE_CMIC_TIMESYNC_INTERRUPT_CLRr(unit, int_status);
        WRITE_CMIC_TIMESYNC_INTERRUPT_CLRr(unit, 0);

        /* Enable interrupts */
        WRITE_CMIC_TIMESYNC_INTERRUPT_ENABLEr(unit, int_enable);

    }

    if (notify && !over_flow && (ts_capture_info[unit]->ts_fifo_sem)) {
        sal_sem_give(ts_capture_info[unit]->ts_fifo_sem);
    }

}

#ifdef BCM_CMICX_SUPPORT
void
soc_esw_timesync_ts_cmicx_intr(int unit, void *data)
{
    soc_esw_timesync_ts_intr(unit);
}
#endif /* BCM_CMICX_SUPPORT */

#endif


#if defined(BCM_TIME_NANOSYNC_SUPPORT)
void
time_capture_cb_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    uint8 cb_data_fill[BCM_TIME_CAPTURE_MAX_FIFO_DATA_SIZE];
    uint8 cb_data_overflow[2];
    uint32 int_status = 0;
    uint32 ts_ctr = ~0;
    soc_reg_t ns_reg;

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return;
    }
    if (SOC_REG_IS_VALID(unit, NS_TS_INT_STATUSr)) {
        ns_reg = NS_TS_INT_STATUSr;
    } else {
        ns_reg = IPROC_NS_TS_INT_STATUSr;
    }

    while(1) {
        /* Wait on the signal from the ISR */
        (void)sal_sem_take(g_fifo_sem, sal_sem_FOREVER);

        int_status = g_int_status;

        if (soc_reg_field_get(unit, ns_reg, int_status, TS0_CNT_OFFSET_UPDATEDf)) {
            if (cb_ts0_ctr[unit].cb != NULL){
                ts_ctr = 0;
                cb_ts0_ctr[unit].cb(unit, cb_ts0_ctr[unit].user_data_ptr, &ts_ctr, bcmTimeCaptureTS0CntOffsetUpdated);
            }
        }

        if (soc_reg_field_get(unit, ns_reg, int_status, TS1_CNT_OFFSET_UPDATEDf)) {
            if (cb_ts1_ctr[unit].cb != NULL){
                ts_ctr = 1;
                cb_ts1_ctr[unit].cb(unit, cb_ts1_ctr[unit].user_data_ptr, &ts_ctr, bcmTimeCaptureTS1CntOffsetUpdated);
            }
        }

        if (soc_reg_field_get(unit, ns_reg, int_status, TS_FIFO_NOT_EMPTYf)) {
            sal_memcpy(cb_data_fill, g_fifo_data_fill, BCM_TIME_CAPTURE_MAX_FIFO_DATA_SIZE);
        }

        /* TS_FIFO_OVERFLOW interrupt */
        if (soc_reg_field_get(unit, ns_reg, int_status, TS_FIFO_OVERFLOWf)) {
            sal_memcpy(cb_data_overflow, g_fifo_data_overflow, 2);
        }

        if (soc_reg_field_get(unit, ns_reg, int_status, TS_FIFO_NOT_EMPTYf)) {
            if (cb_fill.cb != NULL)
                cb_fill.cb(unit, cb_fill.user_data_ptr, (uint32 *)cb_data_fill, bcmTimeCapturetFill);
            if (cb_all.cb != NULL)
                cb_all.cb(unit, cb_all.user_data_ptr, (uint32 *)cb_data_fill, bcmTimeCapturetFill);
        }

        /* TS_FIFO_OVERFLOW interrupt */
        if (soc_reg_field_get(unit, ns_reg, int_status, TS_FIFO_OVERFLOWf)) {
            if (cb_overflow.cb != NULL)
                cb_overflow.cb(unit, cb_overflow.user_data_ptr, (uint32 *)cb_data_overflow, bcmTimeCaptureOverflow);
            if (cb_all.cb != NULL)
                cb_all.cb(unit, cb_all.user_data_ptr, (uint32 *)cb_data_overflow, bcmTimeCaptureOverflow);
        }
    }
}


#if defined(BCM_TIME_NANOSYNC_SUPPORT)
void
soc_esw_nanosync_ts_cmicx_intr(int unit, void *data)
{
    int valid = 0;
    uint32 ts_err = 0;
    uint32 int_status = 0;  /* Interrupt status */
    uint32 int_status_clear = 0;  /* Interrupt status clear */
    uint32 int_enable = 0;  /* Interrupt enable */
    uint32 regval0,regval1,regval2,regval3,lower, middle, upper,event_id, fifo_depth, capture_status;
    soc_reg_t tsfifo_ts[4], tsfifo_ovf_status, tsfifo_int_status, tsfifo_int_enable, tsfifo_capture_status;
    soc_field_t tsfifo_not_empty, tsfifo_overflow, tsfifo_depth;

    int buf_full=0, buf_idx=0;

    if (!soc_feature(unit, soc_feature_timesync_nanosync)) {
        return;
    }

    if (SOC_IS_HURRICANE4(unit)) {
        /*
         * There are two FIFOs on HR4.
         * By default, the configuration is map all events to FIFO1.
         * No R5 on HR4, so no FW needs the FIFO.
         * SDK will reads from FIFIO1.
         */
        tsfifo_depth = FIFO1_DEPTHf;
        tsfifo_not_empty = TS_FIFO1_NOT_EMPTYf;
        tsfifo_overflow = TS_FIFO1_OVERFLOWf;
        tsfifo_int_status = NS_TS_INT_STATUSr;
        tsfifo_int_enable = NS_TS_INT_ENABLEr;
        tsfifo_ovf_status = NS_TIMESYNC_INPUT_TIME_FIFO1_STATUSr;
        tsfifo_capture_status = NS_TS_CAPTURE_STATUSr;
        tsfifo_ts[0] = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_0r;
        tsfifo_ts[1] = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_1r;
        tsfifo_ts[2] = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_2r;
        tsfifo_ts[3] = NS_TIMESYNC_INPUT_TIME_FIFO1_TS_3r;
    } else if (SOC_REG_IS_VALID(unit, NS_TIMESYNC_INPUT_TIME_FIFO_STATUSr)) {
        tsfifo_depth = FIFO_DEPTHf;
        tsfifo_not_empty = TS_FIFO_NOT_EMPTYf;
        tsfifo_overflow = TS_FIFO_OVERFLOWf;
        tsfifo_ovf_status = NS_TIMESYNC_INPUT_TIME_FIFO_STATUSr;
        tsfifo_capture_status = NS_TS_CAPTURE_STATUSr;
        tsfifo_int_status = NS_TS_INT_STATUSr;
        tsfifo_int_enable = NS_TS_INT_ENABLEr;
        tsfifo_ts[0] = NS_TIMESYNC_INPUT_TIME_FIFO_TS_0r;
        tsfifo_ts[1] = NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r;
        tsfifo_ts[2] = NS_TIMESYNC_INPUT_TIME_FIFO_TS_2r;
        tsfifo_ts[3] = NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r;
    } else {
        tsfifo_depth = FIFO_DEPTHf;
        tsfifo_not_empty = TS_FIFO_NOT_EMPTYf;
        tsfifo_overflow = TS_FIFO_OVERFLOWf;
        tsfifo_ovf_status = IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_STATUSr;
        tsfifo_capture_status = IPROC_NS_TS_CAPTURE_STATUSr;
        tsfifo_int_status = IPROC_NS_TS_INT_STATUSr;
        tsfifo_int_enable = IPROC_NS_TS_INT_ENABLEr;
        tsfifo_ts[0] = IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_0r;
        tsfifo_ts[1] = IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_1r;
        tsfifo_ts[2] = IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_2r;
        tsfifo_ts[3] = IPROC_NS_TIMESYNC_INPUT_TIME_FIFO_TS_3r;
    }


    /* Read the GDPLL interrupt */
    BCM_TIME_READ_NS_REGr(unit, tsfifo_int_status, 0, &int_status);
    BCM_TIME_READ_NS_REGr(unit, tsfifo_int_enable, 0, &int_enable);

    /* Process only enabled interrupts */
    int_status &= int_enable;

    /* TS_FIFO_NOT_EMPTY interrupt */
    {
        if (soc_reg_field_get(unit, tsfifo_int_status, int_status, tsfifo_not_empty)) {
            ts_capture_info[unit]->intr_stats_tsfifo_not_empty++;

            BCM_TIME_READ_NS_REGr(unit, tsfifo_capture_status, 0, &capture_status);

            fifo_depth = soc_reg_field_get(unit, tsfifo_capture_status, capture_status, tsfifo_depth);

            /*bsl_printf("FIFO_Depth: %d\n", fifo_depth);*/

            for (; fifo_depth != 0; fifo_depth--) {

                /* For nanosync, TS_FIFO_NOT_EMPTY flag needs to be cleared
                 * before the last record is popped.
                 */
                if (fifo_depth == 1) {

                    /* Set W1TC to clear the flag */
                    soc_reg_field_set(unit, tsfifo_int_status, &int_status_clear, tsfifo_not_empty, 1);

                    /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
                    BCM_TIME_WRITE_NS_REGr(unit, tsfifo_int_status, 0, int_status_clear);

                    /*  Read it back to confirm. */
                    BCM_TIME_READ_NS_REGr(unit, tsfifo_int_status, 0, &int_status_clear);
                    if (soc_reg_field_get(unit, tsfifo_int_status, int_status_clear, tsfifo_not_empty)) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "FIFO_NOT_EMPTY status is not cleared\n")));
                    }
                }

                /* Must read LOWER regiser first */
                /* Read to this register POPs the FIFO */
                BCM_TIME_READ_NS_REGr(unit, tsfifo_ts[3], 0, &regval3);
                BCM_TIME_READ_NS_REGr(unit, tsfifo_ts[2], 0, &regval2);
                BCM_TIME_READ_NS_REGr(unit, tsfifo_ts[1], 0, &regval1);
                BCM_TIME_READ_NS_REGr(unit, tsfifo_ts[0], 0, &regval0);


                /* Check capture event id */
                event_id = soc_reg_field_get(
                                unit, tsfifo_ts[3],
                                regval3, TS_EVENT_IDf);
                valid = soc_reg_field_get(
                                unit, tsfifo_ts[3],
                                regval3, TS_VALIDf);

                ts_err = soc_reg_field_get(unit, tsfifo_ts[3], regval3, TS_ERRf);

                if (!valid){
                    ts_capture_info[unit]->intr_stats_tsfifo_invalid++;
                    continue;
                }

                /* Calculate seconds/nanoseconds from the timestamp value */
                lower = soc_reg_field_get(
                                unit, tsfifo_ts[0],
                                regval0, TS0_Lf);

	            upper = soc_reg_field_get(
	                            unit, tsfifo_ts[1],
	                            regval1, TS0_Uf);

	            lower = (lower & 0xFFFFFFFF);
	            upper = ((upper & 0xFFFFF));

	            /*bsl_printf("FIFO timestamp popped. EventId: %d, TS1: %x%x\n", event_id, upper, lower);*/

	            buf_idx = (ts_capture_info[unit]->ts_fifo_wr_index);
	            buf_full = ((buf_idx + 12) >= BCM_TIME_TS_CAPTURE_BUF_SIZE);
	            if (!buf_full) {

	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x1;

	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & event_id;
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = ((ts_err << 4) | (0xF & (upper >> 16)));
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (upper >> 8);
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & upper;
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 24);
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 16);
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 8);
	            ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & lower;

	            ts_capture_info[unit]->ts_fifo_wr_index = buf_idx;
	            }

	            /* TS1: Calculate seconds/nanoseconds from the timestamp value */
	            lower = soc_reg_field_get(
	                            unit, tsfifo_ts[1],
	                            regval1, TS1_Lf);

                middle = soc_reg_field_get(
                                            unit, tsfifo_ts[2],
                                            regval2, TS1_Mf);

                upper = soc_reg_field_get(
                                unit, tsfifo_ts[3],
                                regval3, TS1_Uf);

                lower = (lower & 0xFFF) | ((middle & 0xFFFFF) << 12);
                upper = ((upper & 0xFF) << 12) | ((middle >> 20) & 0xFFF);


                /*bsl_printf("FIFO timestamp popped. EventId: %d, TS1: %x%x\n", event_id, upper, lower);*/

                buf_idx = (ts_capture_info[unit]->ts_fifo_wr_index);
                buf_full = ((buf_idx + 12) >= BCM_TIME_TS_CAPTURE_BUF_SIZE);
                if (!buf_full) {

                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x0;
                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0x1;

                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & event_id;
	        ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = ((ts_err << 4) | (0xF & (upper >> 16)));
                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (upper >> 8);
                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & upper;
                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 24);
                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 16);
                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & (lower >> 8);
                ts_capture_info[unit]->ts_fifo_data_fill[buf_idx++] = 0xFF & lower;

                ts_capture_info[unit]->ts_fifo_wr_index = buf_idx;
                }
            }
        }
    }

    /* TS_FIFO_OVERFLOW interrupt */
    if (soc_reg_field_get(unit, tsfifo_int_status, int_status, tsfifo_overflow)) {
        ts_capture_info[unit]->intr_stats_tsfifo_overflow++;

        /* Set W1TC to clear the flag */
        soc_reg_field_set(unit, tsfifo_int_status, &int_status_clear, tsfifo_overflow, 1);
        /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
        BCM_TIME_WRITE_NS_REGr(unit, tsfifo_int_status, 0, int_status_clear);

        BCM_TIME_READ_NS_REGr(unit, tsfifo_capture_status, 0, &capture_status);
        fifo_depth = soc_reg_field_get(unit, tsfifo_capture_status, capture_status, tsfifo_depth);

        while (fifo_depth) {
            /* Must read LOWER regiser first */
            /* Read to this register POPs the FIFO */
            BCM_TIME_READ_NS_REGr(unit, tsfifo_ts[3], 0, &regval3);
            BCM_TIME_READ_NS_REGr(unit, tsfifo_ts[2], 0, &regval2);
            BCM_TIME_READ_NS_REGr(unit, tsfifo_ts[1], 0, &regval1);
            BCM_TIME_READ_NS_REGr(unit, tsfifo_ts[0], 0, &regval0);

            fifo_depth--;
        }

        /* Read it back to confirm. */
        BCM_TIME_READ_NS_REGr(unit, tsfifo_int_status, 0, &int_status_clear);
        if (soc_reg_field_get(unit, tsfifo_int_status, int_status_clear, tsfifo_overflow)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "FIFO_OVERFLOW status is not cleared\n")));
        }

        BCM_TIME_READ_NS_REGr(unit, tsfifo_ovf_status, 0, &capture_status);

    } else if ((ts_capture_info[unit]->ts_fifo_sem)) {
        /* Signal the thread for buffer read */
        sal_sem_give(ts_capture_info[unit]->ts_fifo_sem);
    }

    /* Re-enable TS NS IRQ */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_cmic_intr_enable(unit, TS_INTR);
    }
#endif


}
#endif /*   BCM_TIME_NANOSYNC_SUPPORT */

int
_bcm_esw_time_capture_init(int unit)
{
    sal_thread_t thread_id_time_capture_cb;

    cb_overflow.cb = NULL;
    cb_overflow.user_data_ptr = NULL;

    cb_fill.cb = NULL;
    cb_fill.user_data_ptr = NULL;

    cb_all.cb = NULL;
    cb_all.user_data_ptr = NULL;

    cb_ts0_ctr[unit].cb = NULL;
    cb_ts0_ctr[unit].user_data_ptr = NULL;

    cb_ts1_ctr[unit].cb = NULL;
    cb_ts1_ctr[unit].user_data_ptr = NULL;


    /* Create the time capture semaphore */
    g_fifo_sem = sal_sem_create("fifo_time_capture", sal_sem_COUNTING, 0);
    if (g_fifo_sem == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "BCM TIME: Error !! failed to create the sem\n")));
          return (SOC_E_MEMORY);
    }

    /* Create the time capture callback thread */
    thread_id_time_capture_cb = sal_thread_create("Time capture cb thread", SAL_THREAD_STKSZ, 100,
                                        time_capture_cb_thread, INT_TO_PTR(unit));
    if (thread_id_time_capture_cb == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "BCM TIME: Error !! callback thread create failed\n")));
        return SOC_E_INTERNAL;
    }

    return BCM_E_NONE;
}
#endif /* BCM_TIME_NANOSYNC_SUPPORT */

#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT) || defined(BCM_TIME_NANOSYNC_SUPPORT)
void
time_capture_ts_cb_thread(void *unit_vp)
{
    int num_entries;
    int unit = PTR_TO_INT(unit_vp);
    uint8 cb_data_fill[12] = {0};

    ts_capture_info[unit]->time_capture_ts_active = 1;

    while (ts_capture_info[unit]->time_capture_ts_enable) {

        /* Wait on the signal from the ISR */
        (void)sal_sem_take(ts_capture_info[unit]->ts_fifo_sem, sal_sem_FOREVER);
        if ((ts_capture_info[unit] != NULL) &&
            (ts_capture_info[unit]->time_capture_ts_enable == 0)) {
            break;
        }

        num_entries = ((ts_capture_info[unit]->ts_fifo_wr_index - ts_capture_info[unit]->ts_fifo_rd_index) / 12);
        while (num_entries) {
            sal_memcpy(cb_data_fill, &(ts_capture_info[unit]->ts_fifo_data_fill[ts_capture_info[unit]->ts_fifo_rd_index]), 12);
            ts_capture_info[unit]->ts_fifo_rd_index += 12;

            if (cb_fill.cb != NULL) {
                 cb_fill.cb(unit, cb_fill.user_data_ptr, (uint32 *)cb_data_fill, bcmTimeCapturetFill);
            }
            num_entries--;
        }
    }

    ts_capture_info[unit]->time_capture_ts_active = 0;

    return;
}

int
bcm_esw_time_capture_ts_init(int unit)
{
#ifdef BCM_CMICX_SUPPORT
    soc_cmic_intr_handler_t handle;
#endif /* BCM_CMICX_SUPPORT */

    /* Enable openloop tsfifo availability only conditionally. */
    if (!soc_property_get(unit, "tsfifo_open_loop", 0)) {
        return BCM_E_NONE;
    }

    if (ts_capture_info[unit] != NULL) {
        return BCM_E_NONE;
    }

    ts_capture_info[unit] = sal_alloc(sizeof(bcm_esw_time_ts_capture_info_t), "ts capture info");
    if (ts_capture_info[unit] == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "BCM TIME: Error !! failed to allocate memory for time capture info\n")));
          return (SOC_E_MEMORY);
    }
    sal_memset(ts_capture_info[unit], 0, sizeof(bcm_esw_time_ts_capture_info_t));

    /* Create the time capture semaphore */
    ts_capture_info[unit]->ts_fifo_sem = sal_sem_create("fifo_time_capture", sal_sem_COUNTING, 0);
    if (ts_capture_info[unit]->ts_fifo_sem == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "BCM TIME: Error !! failed to create semaphore\n")));
          return (SOC_E_MEMORY);
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {

        handle.num = TS_INTR;
#if defined(BCM_TIME_NANOSYNC_SUPPORT)
        if (soc_feature(unit, soc_feature_timesync_nanosync)) {
            handle.intr_fn = soc_esw_nanosync_ts_cmicx_intr;
        } else
#endif /* BCM_TIME_NANOSYNC_SUPPORT */
        {
#if defined (BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
            handle.intr_fn = soc_esw_timesync_ts_cmicx_intr;
#endif
        }
        handle.intr_data = NULL;
        soc_cmic_intr_register(unit, &handle, 1);

        soc_cmic_intr_enable(unit, TS_INTR);
    }
#endif /* BCM_CMICX_SUPPORT */

    /* Create the time capture callback thread */
    ts_capture_info[unit]->time_capture_ts_enable = 1;
    ts_capture_info[unit]->thread_id_time_capture_ts =
        sal_thread_create("Time capture cb thread", SAL_THREAD_STKSZ, 100,
                          time_capture_ts_cb_thread, INT_TO_PTR(unit));

    if (ts_capture_info[unit]->thread_id_time_capture_ts == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "BCM TIME: Error !! callback thread create failed\n")));
        return SOC_E_INTERNAL;
    }

    return BCM_E_NONE;
}

int
bcm_esw_time_capture_ts_deinit(int unit)
{
    soc_timeout_t       to;

    /* Enable openloop tsfifo availability only conditionally. */
    if (!soc_property_get(unit, "tsfifo_open_loop", 0)) {
        return BCM_E_NONE;
    }

    if (ts_capture_info[unit] == NULL) {
        return BCM_E_NONE;
    }

    ts_capture_info[unit]->time_capture_ts_enable = 0;
    sal_sem_give(ts_capture_info[unit]->ts_fifo_sem);

    soc_timeout_init(&to, 50000, 0);

    do {
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit, "TS Thread exit timed out\n")));
        }
    } while (ts_capture_info[unit]->time_capture_ts_active != 0);

    /* Destroy the time capture semaphore */
    sal_sem_destroy(ts_capture_info[unit]->ts_fifo_sem);

    /* Free ts_capture info */
    sal_free(ts_capture_info[unit]);
    ts_capture_info[unit] = NULL;

    return BCM_E_NONE;
}

#endif /* BCM_TIMESYNC_TIME_CAPTURE_SUPPORT */
int bcm_esw_time_ts_offset_set(
    int unit,
    int64 ts_offset,
    uint64 ts_update_intv,
    int ts_counter)
{
    int rv = BCM_E_UNAVAIL;
#if (defined (BCM_MONTEREY_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT) || \
        defined(BCM_FIRELIGHT_SUPPORT)) && \
        defined(COMPILER_HAS_LONGLONG)
    uint32 regval_lower, regval_upper;
    uint64 curr_offset, new_offset, ts_ctr_val;
    soc_reg_t offset_upper, offset_lower, update_upper, update_lower, ts_ctr_upper, ts_ctr_lower;

    if ((ts_update_intv & 0xFFFF000000000000ULL) != 0){
        LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                            "BCM TIME: Error !! Invalid TS interval value\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (SOC_IS_FIRELIGHT(unit))
    {
        BCM_TIME_READ_NS_REGr(unit, IPROC_NS_TIMESYNC_COUNTER_CONFIG_SELECTr, 0, &regval_upper);
    } else {
        BCM_TIME_READ_NS_REGr(unit, NS_TIMESYNC_COUNTER_CONFIG_SELECTr, 0, &regval_upper);
    }

    if((regval_upper != 0) && (ts_counter == 1)){
        LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                            "BCM TIME: Error !! TS1 phase offset cannot be modified when common control is enabled\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (SOC_IS_FIRELIGHT(unit))
    {
        if (ts_counter == 0){
            offset_upper = IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_UPPERr;
            offset_lower = IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_LOWERr;
            update_upper = IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_UPPERr;
            update_lower = IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_LOWERr;
            ts_ctr_upper = IPROC_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr;
            ts_ctr_lower = IPROC_NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr;

        } else if (ts_counter == 1){
            offset_upper = IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_UPPERr;
            offset_lower = IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_LOWERr;
            update_upper = IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_UPPERr;
            update_lower = IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_LOWERr;
            ts_ctr_upper = IPROC_NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr;
            ts_ctr_lower = IPROC_NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "BCM TIME: Error !! Invalid TS counter value\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
    } else {
        if (ts_counter == 0){
            offset_upper = NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_UPPERr;
            offset_lower = NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_LOWERr;
            update_upper = NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_UPPERr;
            update_lower = NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_LOWERr;
            ts_ctr_upper = NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr;
            ts_ctr_lower = NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr;
    
        } else if (ts_counter == 1){
            offset_upper = NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_UPPERr;
            offset_lower = NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_LOWERr;
            update_upper = NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_UPPERr;
            update_lower = NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_LOWERr;
            ts_ctr_upper = NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr;
            ts_ctr_lower = NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "BCM TIME: Error !! Invalid TS counter value\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
    }

    BCM_TIME_READ_NS_REGr(unit, offset_upper, 0, &regval_upper);
    BCM_TIME_READ_NS_REGr(unit, offset_lower, 0, &regval_lower);
    curr_offset = (((uint64)regval_upper & 0xFFFFF) << 32) | (regval_lower);

    BCM_TIME_READ_NS_REGr(unit, ts_ctr_upper, 0, &regval_upper);
    BCM_TIME_READ_NS_REGr(unit, ts_ctr_lower, 0, &regval_lower);
    ts_ctr_val = (((uint64)regval_upper & 0xFFFFF) << 32) | (regval_lower);

    if (ts_offset >= 0){
        new_offset = (curr_offset + ts_offset) % 0xFFFFFFFFFFFFFULL;
    } else {
        ts_offset *= (-1);
        if ((int64)(curr_offset - ts_offset) > 0) {
            new_offset = curr_offset - ts_offset;
        } else {
            new_offset = (curr_offset + 0xFFFFFFFFFFFFFULL) - ts_offset;
        }
    }

    BCM_TIME_WRITE_NS_REGr(unit, offset_upper, 0, (new_offset >> 32) & 0xFFFFF);
    BCM_TIME_WRITE_NS_REGr(unit, offset_lower, 0, new_offset & 0xFFFFFFFF);

    ts_ctr_val = ((ts_ctr_val >> 4) + ts_update_intv) % 0xFFFFFFFFFFFFULL;
    BCM_TIME_WRITE_NS_REGr(unit, update_lower, 0, ts_ctr_val & 0xFFFFFFFF);
    BCM_TIME_WRITE_NS_REGr(unit, update_upper, 0, ((ts_ctr_val >> 32) & 0xFFFF) | 0x10000);
    rv = BCM_E_NONE;
exit:
#endif

    return rv;
}


STATIC int _bcm_esw_time_ts_offset_reset(
    int unit,
    int ts_counter)
{
    int rv = BCM_E_UNAVAIL;

#if (defined (BCM_MONTEREY_SUPPORT) || defined(BCM_FIREBOLT6_SUPPORT) || \
        defined(BCM_FIRELIGHT_SUPPORT) || defined(BCM_HURRICANE4_SUPPORT)) && \
        defined(COMPILER_HAS_LONGLONG)
    uint32 regval_lower, regval_upper;
    uint64 ts_ctr_val;
    soc_reg_t offset_upper, offset_lower, update_upper, update_lower, ts_ctr_upper, ts_ctr_lower;


    if (SOC_IS_FIRELIGHT(unit))
    {
        if (ts_counter == 0){
            offset_upper = IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_UPPERr;
            offset_lower = IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_LOWERr;
            update_upper = IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_UPPERr;
            update_lower = IPROC_NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_LOWERr;
            ts_ctr_upper = IPROC_NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr;
            ts_ctr_lower = IPROC_NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr;

        } else if (ts_counter == 1){
            offset_upper = IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_UPPERr;
            offset_lower = IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_LOWERr;
            update_upper = IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_UPPERr;
            update_lower = IPROC_NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_LOWERr;
            ts_ctr_upper = IPROC_NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr;
            ts_ctr_lower = IPROC_NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "BCM TIME: Error !! Invalid TS counter value\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
    } else {
        if (ts_counter == 0){
            offset_upper = NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_UPPERr;
            offset_lower = NS_TIMESYNC_TS0_FREQ_CTRL_OFFSET_LOWERr;
            update_upper = NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_UPPERr;
            update_lower = NS_TIMESYNC_TS0_FREQ_CTRL_UPDATE_LOWERr;
            ts_ctr_upper = NS_TIMESYNC_TS0_TIMESTAMP_UPPER_STATUSr;
            ts_ctr_lower = NS_TIMESYNC_TS0_TIMESTAMP_LOWER_STATUSr;

        } else if (ts_counter == 1){
            offset_upper = NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_UPPERr;
            offset_lower = NS_TIMESYNC_TS1_FREQ_CTRL_OFFSET_LOWERr;
            update_upper = NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_UPPERr;
            update_lower = NS_TIMESYNC_TS1_FREQ_CTRL_UPDATE_LOWERr;
            ts_ctr_upper = NS_TIMESYNC_TS1_TIMESTAMP_UPPER_STATUSr;
            ts_ctr_lower = NS_TIMESYNC_TS1_TIMESTAMP_LOWER_STATUSr;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "BCM TIME: Error !! Invalid TS counter value\n")));
            rv = BCM_E_PARAM;
            goto exit;
        }
    }


    _bcm_esw_time_counter_enable(unit, ts_counter, 0);
    BCM_TIME_READ_NS_REGr(unit, ts_ctr_upper, 0, &regval_upper);
    BCM_TIME_READ_NS_REGr(unit, ts_ctr_lower, 0, &regval_lower);
    ts_ctr_val = (((uint64)regval_upper & 0xFFFFF) << 32) | (regval_lower);

    BCM_TIME_WRITE_NS_REGr(unit, offset_upper, 0, 0);
    BCM_TIME_WRITE_NS_REGr(unit, offset_lower, 0, 0);

    ts_ctr_val = ((ts_ctr_val >> 4) + 1000) % 0xFFFFFFFFFFFFULL;
    BCM_TIME_WRITE_NS_REGr(unit, update_lower, 0, ts_ctr_val & 0xFFFFFFFF);
    BCM_TIME_WRITE_NS_REGr(unit, update_upper, 0, ((ts_ctr_val >> 32) & 0xFFFF) | 0x10000);
    _bcm_esw_time_counter_enable(unit, ts_counter, 1);
    sal_usleep(1000);
    rv = BCM_E_NONE;
exit:
#endif

    return rv;
}

#ifdef INCLUDE_GDPLL
void _bcm_bs_slave_bhsb4KHz_dpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan)
{
    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                        "Update gdpll chan config for HB 4KHz\n")));
    pGdpllChan->chan_dpll_config.k1[0] = 2649176;
    pGdpllChan->chan_dpll_config.k1[1] = 0;
    pGdpllChan->chan_dpll_config.k1[2] = 0;

    pGdpllChan->chan_dpll_config.k1Shift[0] = 11;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2[0] = 3191478;
    pGdpllChan->chan_dpll_config.k1k2[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2Shift[0] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[2] = 0;

    pGdpllChan->chan_dpll_config.lockDetThres[0] = 380;
    pGdpllChan->chan_dpll_config.lockDetThres[1] = 0;
    pGdpllChan->chan_dpll_config.lockDetThres[2] = 0;

    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo = 250;
    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi = 380;

    pGdpllChan->chan_dpll_config.dwell_count[0] = 1200;
    pGdpllChan->chan_dpll_config.dwell_count[1] = 0;
    pGdpllChan->chan_dpll_config.dwell_count[2] = 0;

    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 100000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    pGdpllChan->chan_dpll_config.nco_bits = 48;
    pGdpllChan->chan_dpll_config.phase_counter_ref = 0;
    pGdpllChan->chan_dpll_config.dpll_num_states = 1;

    pGdpllChan->chan_dpll_config.idump_mod[0] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[1] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[2] = 0;

    pGdpllChan->chan_dpll_config.phase_error_shift = 12;
    u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0x20000000;
    u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0;

    pGdpllChan->chan_dpll_config.invert_phase_error = 0;
    pGdpllChan->chan_dpll_config.norm_phase_error = 0;
    pGdpllChan->chan_dpll_config.phase_error_thres0 = 1600000000;
    pGdpllChan->chan_dpll_config.phase_error_thres1 = 1600000000;
    pGdpllChan->chan_dpll_config.holdover_filter_coeff = 512;

    pGdpllChan->chan_dpll_state.dpll_state = 0;
    u64_H(pGdpllChan->chan_dpll_state.loop_filter) = 0x20000000;
    u64_L(pGdpllChan->chan_dpll_state.loop_filter) = 0;
    pGdpllChan->chan_dpll_state.dwell_counter = 0;
    pGdpllChan->chan_dpll_state.lockDetFilter = 64000;
    u64_H(pGdpllChan->chan_dpll_state.offset) = 0;
    u64_L(pGdpllChan->chan_dpll_state.offset) = 0;
    pGdpllChan->chan_dpll_state.init_flag = 0;
    pGdpllChan->chan_dpll_state.init_offset_flag = 0;
    u64_H(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0;
    pGdpllChan->chan_dpll_state.phaseCounterN = 1;
    pGdpllChan->chan_dpll_state.phaseCounterM = 0;
    pGdpllChan->chan_dpll_state.phaseCounterFrac = 0;
    pGdpllChan->chan_dpll_state.idumpCounter = 0;
    u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    pGdpllChan->chan_dpll_state.lockIndicator = 0;
    pGdpllChan->chan_dpll_state.lossOfLockIndicator = 0;

    return;
}
void _bcm_bs_slave_bhsb1Hz_dpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan)
{
    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                        "Update gdpll chan config for HB 1Hz\n")));
    pGdpllChan->chan_dpll_config.k1[0] = 44050303;
    pGdpllChan->chan_dpll_config.k1[1] = 56877051;
    pGdpllChan->chan_dpll_config.k1[2] = 0;

    pGdpllChan->chan_dpll_config.k1Shift[0] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2[0] = 4155689;
    pGdpllChan->chan_dpll_config.k1k2[1] = 303506;
    pGdpllChan->chan_dpll_config.k1k2[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2Shift[0] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[2] = 0;

    pGdpllChan->chan_dpll_config.lockDetThres[0] = 1024;
    pGdpllChan->chan_dpll_config.lockDetThres[1] = 1024;
    pGdpllChan->chan_dpll_config.lockDetThres[2] = 0;

    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo = 512;
    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi = 1024;

    pGdpllChan->chan_dpll_config.dwell_count[0] = 200;
    pGdpllChan->chan_dpll_config.dwell_count[1] = 200;
    pGdpllChan->chan_dpll_config.dwell_count[2] = 0;

    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 100000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 100000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    pGdpllChan->chan_dpll_config.nco_bits = 48;
    pGdpllChan->chan_dpll_config.phase_counter_ref = 0;
    pGdpllChan->chan_dpll_config.dpll_num_states = 2;

    pGdpllChan->chan_dpll_config.idump_mod[0] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[1] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[2] = 0;

    pGdpllChan->chan_dpll_config.phase_error_shift = 12;
    u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0x20000000;
    u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0;

    pGdpllChan->chan_dpll_config.invert_phase_error = 0;
    pGdpllChan->chan_dpll_config.norm_phase_error = 0;
    pGdpllChan->chan_dpll_config.phase_error_thres0 = 500000000;
    pGdpllChan->chan_dpll_config.phase_error_thres1 = 500000000;
    pGdpllChan->chan_dpll_config.holdover_filter_coeff = 512;

    pGdpllChan->chan_dpll_state.dpll_state = 0;
    u64_H(pGdpllChan->chan_dpll_state.loop_filter) = 0x20000000;
    u64_L(pGdpllChan->chan_dpll_state.loop_filter) = 0;
    pGdpllChan->chan_dpll_state.dwell_counter = 200;
    pGdpllChan->chan_dpll_state.lockDetFilter = 64000;
    u64_H(pGdpllChan->chan_dpll_state.offset) = 0;
    u64_L(pGdpllChan->chan_dpll_state.offset) = 0;
    pGdpllChan->chan_dpll_state.init_flag = 0;
    pGdpllChan->chan_dpll_state.init_offset_flag = 0;
    u64_H(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0;
    pGdpllChan->chan_dpll_state.phaseCounterN = 1;
    pGdpllChan->chan_dpll_state.phaseCounterM = 0;
    pGdpllChan->chan_dpll_state.phaseCounterFrac = 0;
    pGdpllChan->chan_dpll_state.idumpCounter = 0;
    u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    pGdpllChan->chan_dpll_state.lockIndicator = 0;
    pGdpllChan->chan_dpll_state.lossOfLockIndicator = 0;

    return;
}

int
_bcm_bs_slave_dpll_chan_create(int unit, bcm_time_interface_t *intf)
{
    int rv = BCM_E_NONE;
    uint32 flags = BCM_GDPLL_CONF_DPLL |
                           BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                           BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                           BCM_GDPLL_CHAN_OUTPUT_CONF;
    int bs_id = intf->id;
    uint32 rval;
    int gdpllChan = 0;

    bcm_gdpll_chan_t    gdpll_chan;
    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));

    gdpll_chan.event_ref.input_event = ( (bs_id == 0) ? bcmGdpllInputEventBS0ConvTC : bcmGdpllInputEventBS1ConvTC);
    gdpll_chan.event_ref.event_divisor = 1;
    gdpll_chan.event_ref.event_dest = bcmGdpllEventDestM7;
    gdpll_chan.event_ref.ts_counter = 1;
    gdpll_chan.event_ref.ppm_check_enable = 0;

    gdpll_chan.event_fb.input_event = ( (bs_id == 0) ? bcmGdpllInputEventBS0HB : bcmGdpllInputEventBS1HB);
    gdpll_chan.event_fb.event_divisor = 1;
    gdpll_chan.event_fb.event_dest = bcmGdpllEventDestALL;
    gdpll_chan.event_fb.ts_counter = 1;
    gdpll_chan.event_fb.ppm_check_enable = 0;

    gdpll_chan.chan_prio = 0;
    gdpll_chan.out_event = bcmGdpllOutputEventTS1;
    gdpll_chan.ts_pair_dest = bcmGdpllDebugDestM7;

    LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                        "Creating gdpll chan for BS%d slave : HB: %dHz\n"), bs_id, intf->heartbeat_hz));

    if (intf->heartbeat_hz == 4000) {
        _bcm_bs_slave_bhsb4KHz_dpll_chan_config(unit, &gdpll_chan);
    } else if (intf->heartbeat_hz == 1) {
        _bcm_bs_slave_bhsb1Hz_dpll_chan_config(unit, &gdpll_chan);
    } else {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan create failed for BS%d slave \n"), bs_id));
        return BCM_E_PARAM;
    }

    while(1){
        SOC_IF_ERROR_RETURN(READ_NS_BS0_BS_TC_CTRLr(unit, &rval));
        if (soc_reg_field_get(unit, NS_BS0_BS_TC_CTRLr, rval, ENABLEf)) {
            break;
        }
        sal_usleep(1000);
    }
    sal_usleep(2000000);

    rv = bcm_esw_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan create failed for BS%d slave \n"), bs_id));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan%d created for BS%d slave \n"), gdpllChan, bs_id));

        rv = bcm_esw_gdpll_chan_enable(unit, gdpllChan, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan%d enable failed for BS%d slave \n"), gdpllChan, bs_id));
            return rv;
        }
        LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit,
                "\n gdpll chan %d enabled for BS slave ... \n"), gdpllChan));
        TIME_DPLL_CHAN(unit, bs_id) = gdpllChan;
    }
    return rv;
}

int
_bcm_bs_master_dpll_chan_create(int unit, bcm_time_interface_t *intf)
{
    bcm_gdpll_chan_t    gdpll_chan;
    bcm_time_ts_counter_t counter;
    int64 T, V;
    uint64 val_64b;
    uint64 mask_bit;
    uint32 regVal=0;
    uint32 sync_mode_en = 0;
    uint32 start_delay = 0;
    int rv = BCM_E_NONE;
    int gdpllChan = 0;
    uint32 poll_interval_us = 1000000; /* one sec */
    int32 poll_period_us = (60*poll_interval_us); /* one min */

    int bs_id = intf->id;
    uint32 flags = BCM_GDPLL_CONF_DPLL |
                           BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                           BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                           BCM_GDPLL_CHAN_OUTPUT_CONF;

    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));

    gdpll_chan.event_fb.input_event = (bs_id == 0) ? bcmGdpllInputEventBS0PLL : bcmGdpllInputEventBS1PLL;
    gdpll_chan.event_fb.event_divisor = 2500; /* runs at 4KHz */
    gdpll_chan.event_fb.event_dest = bcmGdpllEventDestM7;
    gdpll_chan.event_fb.ts_counter = 1; /* TS1 */
    gdpll_chan.event_fb.ppm_check_enable = 0;

    gdpll_chan.chan_prio = 0;
    gdpll_chan.out_event = (bs_id == 0) ? bcmGdpllOutputEventBSPLL0 : bcmGdpllOutputEventBSPLL1;
    gdpll_chan.ts_pair_dest = bcmGdpllDebugDestM7; /* TBD: Need to recheck if this is required for R5 as well */

    gdpll_chan.chan_dpll_config.k1[0] = 666527938;
    gdpll_chan.chan_dpll_config.k1[1] = 0;
    gdpll_chan.chan_dpll_config.k1[2] = 0;

    gdpll_chan.chan_dpll_config.k1Shift[0] = 2;
    gdpll_chan.chan_dpll_config.k1Shift[1] = 0;
    gdpll_chan.chan_dpll_config.k1Shift[2] = 0;

    gdpll_chan.chan_dpll_config.k1k2[0] = 11115;
    gdpll_chan.chan_dpll_config.k1k2[1] = 0;
    gdpll_chan.chan_dpll_config.k1k2[2] = 0;

    gdpll_chan.chan_dpll_config.k1k2Shift[0] = 5;
    gdpll_chan.chan_dpll_config.k1k2Shift[1] = 0;
    gdpll_chan.chan_dpll_config.k1k2Shift[2] = 0;

    gdpll_chan.chan_dpll_config.lockDetThres[0] = 38000;
    gdpll_chan.chan_dpll_config.lockDetThres[1] = 0;
    gdpll_chan.chan_dpll_config.lockDetThres[2] = 0;

    gdpll_chan.chan_dpll_config.lockIndicatorThresholdLo = 25000;
    gdpll_chan.chan_dpll_config.lockIndicatorThresholdLHi = 38000;

    gdpll_chan.chan_dpll_config.dwell_count[0] = 3000;
    gdpll_chan.chan_dpll_config.dwell_count[1] = 0;
    gdpll_chan.chan_dpll_config.dwell_count[2] = 0;

    u64_H(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[0]) = 200000000;
    u64_H(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_H(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(gdpll_chan.chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    gdpll_chan.chan_dpll_config.nco_bits = 48;
    gdpll_chan.chan_dpll_config.phase_counter_ref = 1;
    gdpll_chan.chan_dpll_config.dpll_num_states = 1;

    gdpll_chan.chan_dpll_config.idump_mod[0] = 1;
    gdpll_chan.chan_dpll_config.idump_mod[1] = 0;
    gdpll_chan.chan_dpll_config.idump_mod[2] = 0;

    gdpll_chan.chan_dpll_config.phase_error_shift = 12;
    u64_H(gdpll_chan.chan_dpll_config.nominal_loop_filter) = 0x0F000000;
    u64_L(gdpll_chan.chan_dpll_config.nominal_loop_filter) = 0;

    gdpll_chan.chan_dpll_config.invert_phase_error = 1;
    gdpll_chan.chan_dpll_config.norm_phase_error = 0;
    gdpll_chan.chan_dpll_config.phase_error_thres0 = 500000000;
    gdpll_chan.chan_dpll_config.phase_error_thres1 = 500000000;
    gdpll_chan.chan_dpll_config.holdover_filter_coeff = 512;

    gdpll_chan.chan_dpll_state.dpll_state = 0;
    u64_H(gdpll_chan.chan_dpll_state.loop_filter) = 0x0F000000;
    u64_L(gdpll_chan.chan_dpll_state.loop_filter) = 0;
    gdpll_chan.chan_dpll_state.dwell_counter = 0;
    gdpll_chan.chan_dpll_state.lockDetFilter = 64000;
    u64_H(gdpll_chan.chan_dpll_state.offset) = 0;
    u64_L(gdpll_chan.chan_dpll_state.offset) = 0;
    gdpll_chan.chan_dpll_state.init_flag = 0;
    gdpll_chan.chan_dpll_state.init_offset_flag = 0;
    u64_H(gdpll_chan.chan_dpll_state.phase_counter) = 0;
    u64_L(gdpll_chan.chan_dpll_state.phase_counter) = 0;
    u64_H(gdpll_chan.chan_dpll_state.phaseCounterDelta) = 0;
    u64_L(gdpll_chan.chan_dpll_state.phaseCounterDelta) = 4000000;
    gdpll_chan.chan_dpll_state.phaseCounterN = 1;
    gdpll_chan.chan_dpll_state.phaseCounterM = 0;
    gdpll_chan.chan_dpll_state.phaseCounterFrac = 0;
    gdpll_chan.chan_dpll_state.idumpCounter = 0;
    u64_H(gdpll_chan.chan_dpll_state.accumPhaseError)=0;
    u64_L(gdpll_chan.chan_dpll_state.accumPhaseError)=0;
    gdpll_chan.chan_dpll_state.lockIndicator = 0;
    gdpll_chan.chan_dpll_state.lossOfLockIndicator = 0;

#define READ_NS_REGr(unit, reg, idx, rvp) \
    soc_iproc_getreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rvp);
#define WRITE_NS_REGr(unit, reg, idx, rv) \
    soc_iproc_setreg(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, idx), rv)

    while(poll_period_us > 0) {
        if (IS_BROADSYNC0(bs_id)) {
            READ_NS_REGr(unit, NS_BROADSYNC_SYNC_MODEr, 0, &regVal );
            sync_mode_en = soc_reg_field_get(unit, NS_BROADSYNC_SYNC_MODEr, regVal, SYNC_MODEf);
            start_delay = soc_reg_field_get(unit, NS_BROADSYNC_SYNC_MODEr, regVal, START_DELAYf);
        } else if (IS_BROADSYNC1(bs_id)) {
            READ_NS_REGr(unit, NS_COMMON_CTRLr, 0, &regVal );
            regVal = soc_reg_field_get(unit, NS_COMMON_CTRLr, regVal, RESERVEDf);
            sync_mode_en = ((regVal >> 26) & 0x1);
            start_delay = (regVal & 0x3FFFFFF);
        }

        if(!sync_mode_en) {
            LOG_VERBOSE(BSL_LS_BCM_TIME,
                        (BSL_META_U(unit,
                            "\n BS%d sync mode is not enabled yet \n"), bs_id));
            sal_usleep(poll_interval_us);
            poll_period_us -= poll_interval_us;
        } else {
            break;
        }
    }

    LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit,
                            "\n BS[%d] syncmode_en[%d] start_delay[%d/0x%08x] \n"),
                            bs_id, sync_mode_en, start_delay, start_delay));
    if(!sync_mode_en) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### BS%d SYNC_MODE IS NOT ENABLED ### \n"), bs_id));
        return BCM_E_INTERNAL;
    }

    /* Get the current 52b timestamper value */
    counter.ts_counter = gdpll_chan.event_fb.ts_counter;
    rv = bcm_esw_time_ts_counter_get(unit, &counter);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(0, "### Error in bcm_time_ts_counter_get ### \n")));
        return rv;
    }

    bcm_time_interface_t *p_intf = TIME_INTERFACE(unit, 0);
    if ((p_intf->flags & BCM_TIME_INPUT) == 0) {
        COMPILER_64_COPY(T, counter.ts_counter_ns);

        /* V = floor(T / 16,000,000,000) */
        COMPILER_64_COPY(V, T);
        COMPILER_64_SET(val_64b, 0x3, 0xB9ACA000);  /* 16,000,000,000 = 0x3_B9AC_A000 */
        COMPILER_64_UDIV_64(V, val_64b);

        /* ensure headroom of (START_DELAY + 1ms) before 1sec TS1 boundary */
        COMPILER_64_ADD_32(V, 2); /* align with (X+2)th 1sec-Boundary */
        COMPILER_64_UMUL_32(V, 1000000000);
        COMPILER_64_SHL(V, 4);
        COMPILER_64_SUB_32(V, 4000000);/* For 4Khz DPLL rate for BS PLL */
    } else {
        uint32 time_0=0;
        uint32 time_1=0;
        uint32 time_2=0;
        uint32 epoch=0, time_sec=0, time_ns=0;
        /* Assuming BS input time register read is atomic in nature */
        READ_NS_BS0_BS_INPUT_TIME_0r(unit, &regVal);
        time_0 = soc_reg_field_get(unit, NS_BS0_BS_INPUT_TIME_0r, regVal, DATAf);
        READ_NS_BS0_BS_INPUT_TIME_1r(unit, &regVal);
        time_1 = soc_reg_field_get(unit, NS_BS0_BS_INPUT_TIME_1r, regVal, DATAf);
        READ_NS_BS0_BS_INPUT_TIME_2r(unit, &regVal);
        time_2 = soc_reg_field_get(unit, NS_BS0_BS_INPUT_TIME_2r, regVal, DATAf);

        epoch = ((time_2>>8)&0x0000FFFF);
        time_sec = ((time_2<<24) | ((time_1>>8)&0x00FFFFFF));
        time_ns = (((time_1&0x3F)<<24) | ((time_0>>8)&0x00FFFFFF));

        COMPILER_64_COPY(val_64b, epoch);
        COMPILER_64_SHL(val_64b, 32);
        COMPILER_64_ADD_32(val_64b, time_sec);

        COMPILER_64_COPY(T, counter.ts_counter_ns);

        COMPILER_64_COPY(V, val_64b);
        /* target to align to sec+2 */
        COMPILER_64_ADD_32(V, 2);
        COMPILER_64_UMUL_32(V, 1000000000);
        V = V % (1ULL<<48);
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                            " CurrTC[%u.%us.%uns] sec[%llu] V[0x%016llx ns]\n"),
                epoch, time_sec, time_ns, (unsigned long long)val_64b, (long long unsigned int)u64_L(V)));


        COMPILER_64_SHL(V, 4);
        COMPILER_64_SUB_32(V, 4000000);
    }

    /* 52b mask for Mod 2^52 */
    u64_H(mask_bit) = 0xFFFFF;
    u64_L(mask_bit) = 0xFFFFFFFF;

    COMPILER_64_AND(V, mask_bit);
    COMPILER_64_COPY(gdpll_chan.chan_dpll_state.phase_counter, V);

    LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit,
                        " BS synth chan create : CurrentTS[%llu/0x%llx] phase_counter[%llu/0x%llx] start_delay[%u]\n"),
             (long long unsigned int)counter.ts_counter_ns, (long long unsigned int)counter.ts_counter_ns,
             (long long unsigned int)gdpll_chan.chan_dpll_state.phase_counter,
             (long long unsigned int)gdpll_chan.chan_dpll_state.phase_counter, start_delay));
    /* Check if the headroom is more than (Start_Delay+1ms)*/
    COMPILER_64_SET(val_64b, 0, 1000000);   /* 1mSec = 1000000nSec */
    COMPILER_64_ADD_32(val_64b, (start_delay*50));
    COMPILER_64_SHR(V, 4);
    COMPILER_64_SHR(T, 4);
    COMPILER_64_SUB_64(V, T);

    if (COMPILER_64_GT(V, val_64b)) {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                            "Available headroom[%llu nsec] \n"), (long long unsigned int)u64_L(V)));
        COMPILER_64_SUB_64(V, val_64b);
        COMPILER_64_UDIV_32(V, 1000);
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                            "wait for %u usec \n"), (unsigned)u64_L(V)));
        sal_usleep(u64_L(V));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                (BSL_META_U(unit,
                            "Insufficient headroom[%llu / 0x%llx] \n"),
                            (long long unsigned int)u64_L(V), (long long unsigned int)u64_L(V)));
    }
    rv = bcm_esw_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan create failed for BS%d master \n"), bs_id));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan%d created for BS%d master \n"), gdpllChan, bs_id));
        rv = bcm_esw_gdpll_chan_enable(unit, gdpllChan, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan enable failed for BS%d master \n"), bs_id));
            return rv;
        }
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan%d enabled for BS%d master \n"), gdpllChan, bs_id));
        TIME_DPLL_CHAN(unit, bs_id) = gdpllChan;
    }
    return rv;
}

static int
_bcm_program_ptpntptod(int unit, bcm_time_spec_t ts, bcm_time_format_t tod_type)
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

    if (tod_type == bcmTimeFormatNTP) {

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

    if (tod_type == bcmTimeFormatPTP) {

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

void _bcm_time_ptp_dpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan)
{

    pGdpllChan->event_fb.input_event = bcmGdpllInputEventIPDM1;
    pGdpllChan->event_fb.event_divisor = 1;
    pGdpllChan->event_fb.event_dest = bcmGdpllEventDestALL;
    pGdpllChan->event_fb.ts_counter = 1; /* TS1 */
    pGdpllChan->event_fb.ppm_check_enable = 0;

    pGdpllChan->chan_prio = 0;
    pGdpllChan->out_event = 11;
    pGdpllChan->ts_pair_dest = bcmGdpllDebugDestM7;

    pGdpllChan->chan_dpll_config.k1[0] = 2882304;
    pGdpllChan->chan_dpll_config.k1[1] = 0;
    pGdpllChan->chan_dpll_config.k1[2] = 0;

    pGdpllChan->chan_dpll_config.k1Shift[0] = 3;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2[0] = 1844675;
    pGdpllChan->chan_dpll_config.k1k2[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2Shift[0] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[2] = 0;

    pGdpllChan->chan_dpll_config.lockDetThres[0] = 3800;
    pGdpllChan->chan_dpll_config.lockDetThres[1] = 0;
    pGdpllChan->chan_dpll_config.lockDetThres[2] = 0;

    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo = 2500;
    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi = 3800;

    pGdpllChan->chan_dpll_config.dwell_count[0] = 50;
    pGdpllChan->chan_dpll_config.dwell_count[1] = 0;
    pGdpllChan->chan_dpll_config.dwell_count[2] = 0;

    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 1000000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    pGdpllChan->chan_dpll_config.nco_bits = 32;
    pGdpllChan->chan_dpll_config.phase_counter_ref = 1;
    pGdpllChan->chan_dpll_config.dpll_num_states = 1;

    pGdpllChan->chan_dpll_config.idump_mod[0] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[1] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[2] = 0;

    pGdpllChan->chan_dpll_config.phase_error_shift = 12;
    u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0x10000000;
    u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0;

    pGdpllChan->chan_dpll_config.invert_phase_error = 1;
    pGdpllChan->chan_dpll_config.norm_phase_error = 0;
    pGdpllChan->chan_dpll_config.phase_error_thres0 = 100000000;
    pGdpllChan->chan_dpll_config.phase_error_thres1 = 100000000;
    pGdpllChan->chan_dpll_config.holdover_filter_coeff = 512;

    pGdpllChan->chan_dpll_state.dpll_state = 0;
    u64_H(pGdpllChan->chan_dpll_state.loop_filter) = 0x10000000;
    u64_L(pGdpllChan->chan_dpll_state.loop_filter) = 0;
    pGdpllChan->chan_dpll_state.dwell_counter = 0;
    pGdpllChan->chan_dpll_state.lockDetFilter = 64000;
    u64_H(pGdpllChan->chan_dpll_state.offset) = 0;
    u64_L(pGdpllChan->chan_dpll_state.offset) = 0;
    pGdpllChan->chan_dpll_state.init_flag = 0;
    pGdpllChan->chan_dpll_state.init_offset_flag = 0;
    /* will be updated accordingly later */
    u64_H(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0x3;
    u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0xB9ACA000;
    pGdpllChan->chan_dpll_state.phaseCounterN = 1;
    pGdpllChan->chan_dpll_state.phaseCounterM = 0;
    pGdpllChan->chan_dpll_state.phaseCounterFrac = 0;
    pGdpllChan->chan_dpll_state.idumpCounter = 0;
    u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    pGdpllChan->chan_dpll_state.lockIndicator = 0;
    pGdpllChan->chan_dpll_state.lossOfLockIndicator = 0;

    return;
}

void _bcm_time_ntp_dpll_chan_config(int unit, bcm_gdpll_chan_t *pGdpllChan)
{

    pGdpllChan->event_fb.input_event = bcmGdpllInputEventIPDM0;
    pGdpllChan->event_fb.event_divisor = 1;
    pGdpllChan->event_fb.event_dest = bcmGdpllEventDestALL;
    pGdpllChan->event_fb.ts_counter = 1; /* TS1 */
    pGdpllChan->event_fb.ppm_check_enable = 0;

    pGdpllChan->chan_prio = 0;
    pGdpllChan->out_event = 10;
    pGdpllChan->ts_pair_dest = bcmGdpllDebugDestM7;

    pGdpllChan->chan_dpll_config.k1[0] = 3094850;
    pGdpllChan->chan_dpll_config.k1[1] = 0;
    pGdpllChan->chan_dpll_config.k1[2] = 0;

    pGdpllChan->chan_dpll_config.k1Shift[0] = 5;
    pGdpllChan->chan_dpll_config.k1Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1Shift[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2[0] = 3961408;
    pGdpllChan->chan_dpll_config.k1k2[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2[2] = 0;

    pGdpllChan->chan_dpll_config.k1k2Shift[0] = 1;
    pGdpllChan->chan_dpll_config.k1k2Shift[1] = 0;
    pGdpllChan->chan_dpll_config.k1k2Shift[2] = 0;

    pGdpllChan->chan_dpll_config.lockDetThres[0] = 3800;
    pGdpllChan->chan_dpll_config.lockDetThres[1] = 0;
    pGdpllChan->chan_dpll_config.lockDetThres[2] = 0;

    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLo = 2500;
    pGdpllChan->chan_dpll_config.lockIndicatorThresholdLHi = 3800;

    pGdpllChan->chan_dpll_config.dwell_count[0] = 50;
    pGdpllChan->chan_dpll_config.dwell_count[1] = 0;
    pGdpllChan->chan_dpll_config.dwell_count[2] = 0;

    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[0]) = 1000000000;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[1]) = 0;
    u64_H(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;
    u64_L(pGdpllChan->chan_dpll_config.phase_error_limiter_thres[2]) = 0;

    pGdpllChan->chan_dpll_config.nco_bits = 32;
    pGdpllChan->chan_dpll_config.phase_counter_ref = 1;
    pGdpllChan->chan_dpll_config.dpll_num_states = 1;

    pGdpllChan->chan_dpll_config.idump_mod[0] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[1] = 0;
    pGdpllChan->chan_dpll_config.idump_mod[2] = 0;

    pGdpllChan->chan_dpll_config.phase_error_shift = 12;
    u64_H(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0x44B82AF1;
    u64_L(pGdpllChan->chan_dpll_config.nominal_loop_filter) = 0;

    pGdpllChan->chan_dpll_config.invert_phase_error = 1;
    pGdpllChan->chan_dpll_config.norm_phase_error = 0;
    pGdpllChan->chan_dpll_config.phase_error_thres0 = 100000000;
    pGdpllChan->chan_dpll_config.phase_error_thres1 = 100000000;
    pGdpllChan->chan_dpll_config.holdover_filter_coeff = 512;

    pGdpllChan->chan_dpll_state.dpll_state = 0;
    u64_H(pGdpllChan->chan_dpll_state.loop_filter) = 0x44B82AF1;
    u64_L(pGdpllChan->chan_dpll_state.loop_filter) = 0;
    pGdpllChan->chan_dpll_state.dwell_counter = 0;
    pGdpllChan->chan_dpll_state.lockDetFilter = 64000;
    u64_H(pGdpllChan->chan_dpll_state.offset) = 0;
    u64_L(pGdpllChan->chan_dpll_state.offset) = 0;
    pGdpllChan->chan_dpll_state.init_flag = 0;
    pGdpllChan->chan_dpll_state.init_offset_flag = 0;
    /* will be updated accordingly later */
    u64_H(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_L(pGdpllChan->chan_dpll_state.phase_counter) = 0;
    u64_H(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0x3;
    u64_L(pGdpllChan->chan_dpll_state.phaseCounterDelta) = 0xB9ACA000;
    pGdpllChan->chan_dpll_state.phaseCounterN = 1;
    pGdpllChan->chan_dpll_state.phaseCounterM = 0;
    pGdpllChan->chan_dpll_state.phaseCounterFrac = 0;
    pGdpllChan->chan_dpll_state.idumpCounter = 0;
    u64_H(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    u64_L(pGdpllChan->chan_dpll_state.accumPhaseError)=0;
    pGdpllChan->chan_dpll_state.lockIndicator = 0;
    pGdpllChan->chan_dpll_state.lossOfLockIndicator = 0;

    return;
}

int
_bcm_bs_tod_dpll_chan_create(int unit, bcm_time_format_t tod_type)
{
    bcm_gdpll_chan_t    gdpll_chan;
    bcm_time_ts_counter_t counter;
    uint64 secs, div, nano_secs;
    bcm_time_spec_t ts;
    int prog_phase_counter = 0;
    int rv = BCM_E_NONE;
    int gdpllChan = 0;

    int32 poll_period_uSec=0;
    uint32 poll_interval_uSec = 0;

    uint32 flags = BCM_GDPLL_CONF_DPLL |
                           BCM_GDPLL_EVENT_CONFIG_REF | BCM_GDPLL_EVENT_CONFIG_FB |
                           BCM_GDPLL_CHAN_ALLOC | BCM_GDPLL_CHAN_SET_PRIORITY |
                           BCM_GDPLL_CHAN_OUTPUT_CONF;

    if ((tod_type != bcmTimeFormatPTP) && (tod_type != bcmTimeFormatNTP)) {
        return BCM_E_PARAM;
    }

    sal_memset(&gdpll_chan, 0, sizeof(bcm_gdpll_chan_t));

    if (tod_type == bcmTimeFormatNTP) {
        _bcm_time_ntp_dpll_chan_config(unit, &gdpll_chan);
    } else {
        _bcm_time_ptp_dpll_chan_config(unit, &gdpll_chan);
    }

    poll_period_uSec = 30*60*1000000;   /* Poll for 30mins */
    poll_interval_uSec = 100000; /* Poll for every 100000uSec=100mSec */

    counter.ts_counter = gdpll_chan.event_fb.ts_counter;

    while(poll_period_uSec > 0) {

        rv = bcm_esw_time_ts_counter_get(unit, &counter);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in bcm_time_ts_counter_get ### \n")));
            return rv;
        }

        COMPILER_64_SHR(counter.ts_counter_ns, 4);

        /* TODO - FIXME !!
         * NTP/PTP ToD counter will get the first ONE_SEC set after 500mSec/537mSec respectively, after enabling.
         * This is verified by reading the present TS counter value(say T1) before programming 0 nanosec to
         * NTP/PTP ToD and read the fisrt 1PPS event(i.e, ONE_SEC) of NTP/PTP ToD. With this its observed that the
         * timestamp is (T1+500mSec/537mSec).
         * To compensate this, start the TS counter prior by 500mSec/(1000-537=463)mSec
         */
        if (tod_type == bcmTimeFormatNTP) {
            COMPILER_64_SUB_32(counter.ts_counter_ns, 500000000);
        } else {
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
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "Error: Could not program phase counter !!! \n")));
        return CMD_FAIL;
    }

    /* Program the NTP counter */
    _bcm_program_ptpntptod(unit, ts, tod_type);

    /* Program the phase counter per spec */
    COMPILER_64_ADD_32(ts.seconds, 1);
    COMPILER_64_UMUL_32(ts.seconds, 1000000000);
    COMPILER_64_SHL(ts.seconds, 4);
    COMPILER_64_COPY(gdpll_chan.chan_dpll_state.phase_counter, ts.seconds);

    rv = bcm_esw_gdpll_chan_create (unit, flags, &gdpll_chan, &gdpllChan);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "\ngdpll chan create failed for tod format [%s] \n"), (tod_type==bcmTimeFormatNTP)? "NTP": "PTP"));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "\ngdpll chan%d created for tod format [%s] \n"), gdpllChan, (tod_type==bcmTimeFormatNTP)? "NTP": "PTP"));

        rv = bcm_esw_gdpll_chan_enable(unit, gdpllChan, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "gdpll chan enable failed for tod format [%s] \n"), (tod_type==bcmTimeFormatNTP)? "NTP": "PTP"));
            return rv;
        }
        LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "\ngdpll chan%d enabled for tod format [%s] \n"), gdpllChan, (tod_type==bcmTimeFormatNTP)? "NTP": "PTP"));

        UPDATE_DPLL_CHAN_FOR_TOD(unit, tod_type, gdpllChan);
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                            "TOD channels: NTP[%d] PTP[%d] \n"), TIME_DPLL_CHAN_NTP(unit), TIME_DPLL_CHAN_PTP(unit)));
    }
    return rv;
}

static int g_dpll_chan_create_unit_num = 0;
void _bs_slave_thread(void *intf_vp) {
    int unit=g_dpll_chan_create_unit_num;
    int rv = BCM_E_NONE;
    bcm_time_interface_t *intf = intf_vp;

    LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit,
                        "Slave thread for BS%d\n"), intf->id));

    rv = _bcm_bs_slave_dpll_chan_create(unit, intf);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(0, "### Error in creating dpll chan for BS slave ### \n")));
    }

    LOG_VERBOSE(BSL_LS_BCM_TIME,
            (BSL_META_U(unit,
                        "Exiting the slave thread for BS%d\n"), intf->id));

    TIME_INTERFACE_CONFIG(unit, intf->id).bs_thread = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

int
_bcm_bs_dpll_chan_destroy(int unit, int bs_id)
{
    int gdpllChan = BCM_TIME_DPLL_CHAN_INVALID;
    int rv = BCM_E_NONE;

    gdpllChan = TIME_DPLL_CHAN(unit, bs_id);

    if( gdpllChan == BCM_TIME_DPLL_CHAN_INVALID) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### gdpll chan invalid  ### \n")));
        return rv;
    } else {
        rv = bcm_esw_gdpll_chan_destroy(unit, gdpllChan);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(0, "### Error in dpll chan destroy ### \n")));
            return rv;
        }
        TIME_DPLL_CHAN(unit, bs_id) = BCM_TIME_DPLL_CHAN_INVALID;
    }
    return rv;
}

int
_bcm_bs_dpll_chan_recreate(int unit, int bs_id)
{
    int gdpllChan = BCM_TIME_DPLL_CHAN_INVALID;
    int rv = BCM_E_NONE;

    gdpllChan = TIME_DPLL_CHAN(unit, bs_id);

    if( gdpllChan == BCM_TIME_DPLL_CHAN_INVALID) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### gdpll chan not created yet  ### \n")));
        return rv;
    } 

    rv = _bcm_bs_dpll_chan_destroy(unit, bs_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(0, "### Error in destroy dpll chan ### \n")));
        return rv;
    }

    if (((TIME_INTERFACE(unit, bs_id))->flags & BCM_TIME_INPUT) == 0) {
        cli_out("\n i/f[%d] in master mode ... \n", bs_id);
        rv = _bcm_bs_master_dpll_chan_create(unit, TIME_INTERFACE(unit, bs_id));
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in creating dpll chan ### \n")));
        }
    }
    return rv;
}

int _bcm_bs_dpll_chan_create(int unit, bcm_time_interface_t *intf)
{
    int rv = BCM_E_NONE;
    int master = ((intf->flags & BCM_TIME_INPUT) == 0);
    sal_thread_t bs_slave_thread = SAL_THREAD_ERROR;

    if(master) {
        rv = _bcm_bs_master_dpll_chan_create(unit, intf);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in creating dpll chan ### \n")));
        }
    } else {
        char thread_name[20] = "BS Slave ";
        g_dpll_chan_create_unit_num = unit;
        thread_name[9] = (unit + 48) % 0xFF;
        thread_name[10] = 0;
        bs_slave_thread = sal_thread_create(thread_name, SAL_THREAD_STKSZ, 100, _bs_slave_thread, intf);
        if (bs_slave_thread == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### Error in creating thread for BS slave ### \n")));
            rv = BCM_E_INTERNAL;
        } else {
            TIME_INTERFACE_CONFIG(unit, intf->id).bs_thread = bs_slave_thread;
        }
    }
    return rv;
}

int _bcm_time_tod_dpll_chan_create(int unit, bcm_time_format_t tod_type) {

    int rv = BCM_E_NONE;
    int dpll_chan = BCM_TIME_DPLL_CHAN_INVALID;


    dpll_chan = (tod_type==bcmTimeFormatNTP)? TIME_DPLL_CHAN_NTP(unit) : TIME_DPLL_CHAN_PTP(unit);

    if( dpll_chan != BCM_TIME_DPLL_CHAN_INVALID) {
        LOG_VERBOSE(BSL_LS_BCM_TIME,
                    (BSL_META_U(unit,
                                "\n dpll chan exists for tod %d\n"), tod_type));
        return BCM_E_EXISTS;
    }

    rv = _bcm_bs_tod_dpll_chan_create(unit, tod_type);
    {
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                        (BSL_META_U(0, "### Error in creating dpll chan for TOD type[%d] ### \n"), tod_type));
        }
    }
    return rv;
}

int _bcm_time_tod_dpll_chan_recreate(int unit, bcm_time_format_t tod_type) {

    int rv = BCM_E_NONE;
    int dpll_chan = BCM_TIME_DPLL_CHAN_INVALID;

    dpll_chan = (tod_type==bcmTimeFormatNTP)? TIME_DPLL_CHAN_NTP(unit) : TIME_DPLL_CHAN_PTP(unit);

    if( dpll_chan == BCM_TIME_DPLL_CHAN_INVALID) {
        LOG_DEBUG(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### gdpll chan not created yet  ### \n")));
        return rv;
    } else {
        rv = bcm_esw_gdpll_chan_destroy(unit, dpll_chan);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                        (BSL_META_U(0, "### Error in destroying dpll chan[%d] for TOD type[%d] ### \n"), dpll_chan, tod_type));
        }
    }

    rv = _bcm_bs_tod_dpll_chan_create(unit, tod_type);
    {
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                        (BSL_META_U(0, "### Error in creating dpll chan for TOD type[%d] ### \n"), tod_type));
        }
    }
    return rv;
}

int _bcm_time_host_notification(int unit, int event_id) {

    int rv = BCM_E_NONE;

    LOG_INFO(BSL_LS_BCM_TIME,
                (BSL_META_U(0, " host notification: %d \n"), event_id));

    if (_BCM_TIME_BS_EVENT_TS_COUNTER_REPROG == event_id) {
        rv = _bcm_bs_dpll_chan_recreate(unit, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### _bcm_bs_dpll_chan_recreate ### \n")));
        }

        rv = _bcm_time_tod_dpll_chan_recreate(unit, bcmTimeFormatNTP);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### _bcm_time_tod_dpll_chan_recreate(NTP) ### \n")));
        }

        rv = _bcm_time_tod_dpll_chan_recreate(unit, bcmTimeFormatPTP);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TIME,
                    (BSL_META_U(0, "### _bcm_time_tod_dpll_chan_recreate(PTP) ### \n")));
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_TIME,
                (BSL_META_U(0, "### event_id(%d) unknown ### \n"), event_id));
        rv = BCM_E_BADID;
    }

    return rv;
}

#endif /* INCLUDE_GDPLL */


/*1. initialize the 80bit/48bit timestamp pair to zero,
 * and initialize the 48 bit HW timestamp counter to zero*/
/*(Ts_80b,ts_48b)*/
int bcm_esw_time_ts_counter_time_init(int unit)
{
    int rv = BCM_E_UNAVAIL;
#if defined(COMPILER_HAS_LONGLONG)
    uint32 regval;

    TIME_LOCK(unit);
    sal_memset(&tod, 0, sizeof(bcm_time_spec_t));
    hwts=0;

    READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 0);
    WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);

    READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr, &regval, NSf, 0);
    WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_UPPERr(unit, regval);

    READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr, &regval, NSf, 0);
    WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_LOWERr(unit, regval);

    READ_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr, &regval, FRACf,(0x40000000));
    WRITE_CMIC_TIMESYNC_TS0_FREQ_CTRL_FRACr(unit, regval);

    READ_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &regval);
    soc_reg_field_set(unit, CMIC_TIMESYNC_TS0_COUNTER_ENABLEr, &regval, ENABLEf, 1);
    WRITE_CMIC_TIMESYNC_TS0_COUNTER_ENABLEr(unit, regval);
    TIME_UNLOCK(unit);
    rv = BCM_E_NONE;
#endif /* defined(COMPILER_HAS_LONGLONG) */

    return rv;
}

/* 2. function will be called by FastPath every 200msec (10 FastPath ticks)
 *    to update the 80bit/48bit timestamp pair.*/
int bcm_esw_time_ts_time_update(int unit)
{
    int rv = BCM_E_UNAVAIL;
#if defined(COMPILER_HAS_LONGLONG)
    bcm_time_ts_counter_t  ctr;
    int64 deltaNs;
    int64 curr_hwts=0;
    int64 tod_ns;

    TIME_LOCK(unit);
    ctr.ts_counter=0;
    ctr.ts_counter_ns=0;

    bcm_esw_time_ts_counter_get(0, &ctr);
    curr_hwts = (int64)ctr.ts_counter_ns;
    deltaNs = ((curr_hwts << 16) - (hwts << 16)) >> 16;
    tod_ns = tod.seconds * 1000000000 + tod.nanoseconds;
    tod_ns += deltaNs;

    tod.seconds = tod_ns / 1000000000;
    tod.nanoseconds = tod_ns % 1000000000;

    hwts = curr_hwts;
    TIME_UNLOCK(unit);
    rv = BCM_E_NONE;
#endif /* defined(COMPILER_HAS_LONGLONG) */

    return rv;
}

/*
3. function that will be used to convert a 32 bit timestamp to an 80 bit timestamp (PTP ToD format, seconds/nanoseconds)
   (using the 80bit/48bit timestamp pair).
*/
int bcm_esw_time_ts_time_convert(int unit, uint32 ts_val, bcm_time_spec_t* time)
{
    int rv = BCM_E_UNAVAIL;
#if defined(COMPILER_HAS_LONGLONG)
    int64 deltaNs;
    int64 tod_ns;

    TIME_LOCK(unit);
    /* 32b to 80b */
    deltaNs = ((((int64)ts_val) << 32) - (hwts << 32)) >> 32;

    tod_ns = tod.seconds * 1000000000 + tod.nanoseconds;
    tod_ns += deltaNs;

    time->seconds = tod_ns / 1000000000;
    time->nanoseconds = tod_ns % 1000000000;
    TIME_UNLOCK(unit);
    rv = BCM_E_NONE;
#endif /* defined(COMPILER_HAS_LONGLONG) */

    return rv;
}
