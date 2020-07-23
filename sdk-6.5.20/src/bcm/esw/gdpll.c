/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       gdpll.c
 *
 * Remarks:    Broadcom StrataSwitch Time GDPLL API
 *
 * Functions:
 *   Private Interface functions
 *      _bcm_esw_gdpll_event_divisor_misc_set
 *      _bcm_esw_gdpll_event_divisor_misc_get
 *      _bcm_esw_gdpll_event_divisor_port_set
 *      _bcm_esw_gdpll_event_divisor_port_get
 *      _bcm_esw_gdpll_event_dest_misc_set
 *      _bcm_esw_gdpll_event_dest_misc_get
 *      _bcm_esw_gdpll_event_dest_port_set
 *      _bcm_esw_gdpll_event_dest_port_get
 *      _bcm_esw_gdpll_event_enable_misc_set
 *      _bcm_esw_gdpll_event_enable_misc_get
 *      _bcm_esw_gdpll_event_enable_port_set
 *      _bcm_esw_gdpll_event_enable_port_get
 *      _bcm_esw_gdpll_event_roe_52b_set
 *      _bcm_esw_gdpll_event_roe_52b_get
 *      _bcm_esw_gdpll_event_config_set
 *
 *      _bcm_esw_gdpll_capture_enable_m7_set
 *      _bcm_esw_gdpll_capture_enable_m7_get
 *      _bcm_esw_gdpll_capture_enable_cpu_set
 *      _bcm_esw_gdpll_capture_enable_cpu_get
 *
 *      _bcm_esw_gdpll_chan_update_set
 *      _bcm_esw_gdpll_chan_update_get
 *      _bcm_esw_gdpll_chan_enable_set
 *      _bcm_esw_gdpll_chan_enable_get
 *      _bcm_esw_gdpll_chan_out_txpi_set
 *      _bcm_esw_gdpll_chan_out_txpi_get
 *      _bcm_esw_gdpll_chan_out_misc_set
 *      _bcm_esw_gdpll_chan_out_misc_get
 *      _bcm_esw_gdpll_chan_out_enable_set
 *      _bcm_esw_gdpll_chan_out_enable_get
 *      _bcm_esw_gdpll_chan_priority_set
 *
 *      _bcm_esw_gdpll_chan_config_dpll_set
 *      _bcm_esw_gdpll_chan_config_dpll_get
 *      _bcm_esw_gdpll_chan_debug_mode_set
 *      _bcm_esw_gdpll_chan_debug_mode_get
 *      _bcm_esw_gdpll_debug_enable_set
 *
 *      _bcm_esw_gdpll_flush
 *      _bcm_esw_gdpll_init
 *      _bcm_esw_gdpll_deinit
 *      _bcm_esw_gdpll_input_eventId_get
 *      _bcm_esw_gdpll_input_array_alloc
 *      _bcm_esw_gdpll_input_array_free
 *      _bcm_esw_gdpll_chan_alloc
 *      _bcm_esw_gdpll_chan_free
 *
 *      _bcm_esw_tdpll_chan_create
 *      _bcm_esw_tdpll_gdpll_chan_destroy
 *
 *   Public Interface functions
 *      bcm_esw_gdpll_chan_create
 *      bcm_esw_gdpll_chan_destroy
 *      bcm_esw_gdpll_chan_enable
 *      bcm_esw_gdpll_chan_state_get
 *      bcm_esw_gdpll_chan_debug_enable
 *      bcm_esw_gdpll_debug_cb_register
 *      bcm_esw_gdpll_flush
 *      bcm_esw_gdpll_cb_register
 *      bcm_esw_gdpll_cb_unregister
 *      bcm_esw_gdpll_debug
 *      bcm_esw_gdpll_offset_get
 *      bcm_esw_gdpll_offset_set
 */

#include <shared/util.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/iproc.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_internal.h>
#endif

#include <bcm/time.h>
extern int bcm_time_ts_counter_get(
    int unit,
    bcm_time_ts_counter_t *counter);

#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm_int/esw/switch.h>
#include <include/sal/core/dpc.h>

#if defined(INCLUDE_GDPLL)
#include <bcm/gdpll.h>
#include <bcm_int/esw/gdpll.h>

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/uc.h>
#endif
#ifdef BCM_MONTEREY_SUPPORT
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#include <bcm/module.h>
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_WB_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION  BCM_WB_VERSION_1_0
#endif  /* BCM_WARM_BOOT_SUPPORT */

#define BCN_WAR
/****************************************************************************/
/*                      MACRO definitions                                   */
/****************************************************************************/
#define BCM_GDPLL_MISC_EVENT_EN_CPU     (1<<0)
#define BCM_GDPLL_MISC_EVENT_EN_BS0HB   (1<<1)
#define BCM_GDPLL_MISC_EVENT_EN_BS1HB   (1<<2)
#define BCM_GDPLL_MISC_EVENT_EN_IPDM0   (1<<3)
#define BCM_GDPLL_MISC_EVENT_EN_IPDM1   (1<<4)
#define BCM_GDPLL_MISC_EVENT_EN_TS1     (1<<5)
#define BCM_GDPLL_MISC_EVENT_EN_RSVD1IF   (1<<6)
#define BCM_GDPLL_MISC_EVENT_EN_RSVD1RF   (1<<7)
#define BCM_GDPLL_MISC_EVENT_EN_BS0CONV (1<<8)
#define BCM_GDPLL_MISC_EVENT_EN_BS1CONV (1<<9)

/* GDPLL debug buffer thresholds */
#define BCM_GDPLL_DEBUG_BUFFER_START    (0x3274000)
#define BCM_GDPLL_DEBUG_BUFFER_SIZE     (1024*10)   /* 40KB/4 */
#define BCM_GDPLL_DEBUG_THRESHOLD_1K    (1024/4)
#define BCM_GDPLL_DEBUG_THRESHOLD_2K    (2048/4)
#define BCM_GDPLL_DEBUG_THRESHOLD_8K    (8192/4)
#define BCM_GDPLL_DEBUG_THRESHOLD       (BCM_GDPLL_DEBUG_THRESHOLD_1K)

/* Debug buffer interrupts */
#define INTR_GDPLL_DEBUG_THR         (1<<0)
#define INTR_GDPLL_DEBUG_OF          (1<<1)
#define INTR_GDPLL_DEBUG_ECC0        (1<<2)
#define INTR_GDPLL_DEBUG_ECC1        (1<<3)
#define INTR_NS_DEBUG                    (INTR_GDPLL_DEBUG_THR  | \
                                         INTR_GDPLL_DEBUG_OF)
/* NS Interrupts */
#define INTR_GDPLL_IA_0_STATUS      (1<<5)  /* Channel(0:31) update */
#define INTR_GDPLL_IA_1_STATUS      (1<<6)  /* Channel(32:63) update */
#define INTR_GDPLL_IA_2_STATUS      (1<<7)  /* Channel(64:95) update */
#define INTR_GDPLL_IA_3_STATUS      (1<<8)  /* Channel(96:127) update */
#define INTR_GDPLL_OA_0_STATUS      (1<<9)  /* Channel(0:31) OA */
#define INTR_GDPLL_OA_1_STATUS      (1<<10) /* Channel(32:63) OA */
#define INTR_GDPLL_OA_2_STATUS      (1<<11) /* Channel(64:95) OA */
#define INTR_GDPLL_OA_3_STATUS      (1<<12) /* Channel(96:127) OA */
#define INTR_M7_CORE_INT_STATUS             (1<<18)
#define INTR_RSVD1_MESSAGE_RX_STATUS        (1<<22)


#define SOC_GDPLL_NS_INTR_POS       (1<<24)
#define SOC_GDPLL_NS_INTR_DEBUG_POS (1<<25)
#define TS_GPIO_COUNT               6

#define EVENT_TS_TO_CPU         0
#define EVENT_TS_TO_M7          1
#define EVENT_TS_TO_ALL         2

/****************************************************************************/
/*                      LOCAL VARIABLES DECLARATION                         */
/****************************************************************************/
/* Like context structure, etc */
STATIC
soc_reg_t  gpio_regs[TS_GPIO_COUNT] = { NS_TIMESYNC_GPIO_0_CTRLr,
                            NS_TIMESYNC_GPIO_1_CTRLr,
                            NS_TIMESYNC_GPIO_2_CTRLr,
                            NS_TIMESYNC_GPIO_3_CTRLr,
                            NS_TIMESYNC_GPIO_4_CTRLr,
                            NS_TIMESYNC_GPIO_5_CTRLr };

STATIC
soc_reg_t  bs_pll_regs[2] = { NS_BROADSYNC0_CLK_EVENT_CTRLr,
                            NS_BROADSYNC1_CLK_EVENT_CTRLr };

STATIC
soc_reg_t mapper_port_enable_regs[4] = { NS_TIMESYNC_MAPPER_PORT_ENABLE_0r,
                                         NS_TIMESYNC_MAPPER_PORT_ENABLE_1r,
                                         NS_TIMESYNC_MAPPER_PORT_ENABLE_2r,
                                         NS_TIMESYNC_MAPPER_PORT_ENABLE_3r
                                       };
STATIC
soc_reg_t pair_available_regs[4] = { NS_GDPLL_IA_TS_PAIR_AVAILABLE0r,
                                     NS_GDPLL_IA_TS_PAIR_AVAILABLE1r,
                                     NS_GDPLL_IA_TS_PAIR_AVAILABLE2r,
                                     NS_GDPLL_IA_TS_PAIR_AVAILABLE3r,
                                   };
STATIC
soc_reg_t ia_in_status_regs[4] = { NS_GDPLL_IA_IN_STATUS0r,
                                   NS_GDPLL_IA_IN_STATUS1r,
                                   NS_GDPLL_IA_IN_STATUS2r,
                                   NS_GDPLL_IA_IN_STATUS3r,
                                 };
STATIC
soc_reg_t nco_out_enable_regs[4] = { NS_GDPLL_NCO_OUT_ENABLE0r,
                                     NS_GDPLL_NCO_OUT_ENABLE1r,
                                     NS_GDPLL_NCO_OUT_ENABLE2r,
                                     NS_GDPLL_NCO_OUT_ENABLE3r,
                                   };
STATIC
soc_reg_t mapper_port_enable[32] = { PORT0_TS_ENABLEf, PORT1_TS_ENABLEf,
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
soc_reg_t gdpll_ia_status[4] = {
                                GDPLL_IA_0_STATUSf, GDPLL_IA_1_STATUSf,
                                GDPLL_IA_2_STATUSf, GDPLL_IA_3_STATUSf,
                               };


#define NUM_TDPLL_INSTANCE  64  
typedef struct gdpll_tdpll_state_s {
    uint32 tdpll_instace[NUM_TDPLL_INSTANCE];   /* Hold the I/A indicator of each instance */
} gdpll_tdpll_state_t;

STATIC gdpll_tdpll_state_t gdpll_tdpll_state;

STATIC gdpll_context_t *pGdpllCtx[BCM_MAX_NUM_UNITS] = {NULL};
STATIC m7_dpll_param_t m7DpllParam;

STATIC int soc_iproc_m7_write(int unit, uint32 addr, uint32 data);
uint32 cbData[3];   /* For RSVD1 messages */

STATIC int _bcm_esw_gdpll_reinit(int unit, gdpll_context_t *pGdpllContext);

/****************************************************************************/
/*                     Internal functions implementation                    */
/****************************************************************************/
int
 _bcm_esw_get_timestamp(int unit, int timestamper, uint64 *pTs_52)
 {
     int rv  = BCM_E_NONE;
     uint32 acc1 = 0;
     uint32 acc2 = 0;
     uint64 ts;

     if (timestamper == 1) {
         SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_ACCUMULATOR_2r(unit, &acc2));
         SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_ACCUMULATOR_1r(unit, &acc1));
     } else if (timestamper == 0) {
         SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_ACCUMULATOR_2r(unit, &acc2));
         SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_ACCUMULATOR_1r(unit, &acc1));
     } else {
         bsl_printf("GDPLL: Error !! _bcm_esw_get_timestamp: Wrong timestamper:%d\n", timestamper);
         rv = BCM_E_FAIL;
     }

     u64_L(ts) = acc1 >> 8 | acc2 << 24;
     u64_H(ts) = acc2 >> 8;
     *pTs_52 = ts;

     return rv;
}

int
_bcm_esw_set_timestamp(int unit, int timestamper, uint64 ts_52)
{
    uint32 rval;
    int rv  = BCM_E_NONE;

    if (timestamper == 1) {
        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_52) << 8 | u64_L(ts_52) >> 24);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_L(ts_52) << 8);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, rval));

    } else if (timestamper == 0) {
        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, &rval, ACC2f, u64_H(ts_52) << 8 | u64_L(ts_52) >> 24);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, &rval, ACC1f, u64_L(ts_52) << 8);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, &rval, ACC0f, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, rval));
    } else {
        bsl_printf("Error !! _bcm_esw_set_timestamp: Wrong timestamper:%d\n", timestamper);
        rv = BCM_E_FAIL;
    }

    return rv;
}

int
_bcm_esw_timestamp_enable(int unit, int timestamper, int enable)
{
    uint32 rval;
    int rv  = BCM_E_NONE;

    if (timestamper == 1) {
        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, rval));
    } else if (timestamper == 0) {
        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &rval, ENABLEf, enable?1:0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, rval));
    } else {
        bsl_printf("Error !! _bcm_esw_timestamp_enable: Wrong timestamper:%d\n", timestamper);
        rv = BCM_E_FAIL;
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *    _bcm_gdpll_scache_alloc
 * Purpose:
 *    Internal routine used to allocate scache memory for gdpll module
 */
STATIC int
_bcm_gdpll_scache_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *gdpll_scache;
    int alloc_size = 0;

    alloc_size = sizeof(gdpll_context_t);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_GDPLL, 0);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 1,
              alloc_size, &gdpll_scache, BCM_WB_DEFAULT_VERSION, NULL));
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_gdpll_scache_sync
 * Purpose:
 *    Routine to sync the gdpll module to scache
 */
int
_bcm_gdpll_scache_sync(int unit)
{
    int  rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8 *gdpll_scache;
    int alloc_size = 0;

    alloc_size = sizeof(gdpll_context_t);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_GDPLL, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, alloc_size,
                    &gdpll_scache, BCM_WB_DEFAULT_VERSION,
                    NULL);
    if (!BCM_FAILURE(rv)) {
        sal_memcpy(gdpll_scache, pGdpllCtx[unit]/*(TIME_INTERFACE(unit, idx))*/,
                   sizeof(gdpll_context_t));
    }

    return rv;
}

/*
 * Function:
 *    _bcm_esw_gdpll_reinit
 * Purpose:
 *    Internal routine used to reinitialize gdpll module based on HW settings
 *    during Warm boot
 */
STATIC int
_bcm_esw_gdpll_reinit(int unit, gdpll_context_t *pGdpllContext)
{
    soc_scache_handle_t scache_handle;
    uint16      recovered_ver = 0;
    uint8       *gdpll_scache;
    int         rv = BCM_E_NONE;

    if(SOC_WARM_BOOT(unit)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_GDPLL, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                &gdpll_scache, BCM_WB_DEFAULT_VERSION,
                &recovered_ver);
        if (!BCM_FAILURE(rv)) {
            sal_memcpy(pGdpllContext, gdpll_scache,
                        sizeof(gdpll_context_t));
        }
    }

    return (BCM_E_NONE);
}
#endif  /* BCM_WARM_BOOT_SUPPORT */

int
_bcm_esw_synce_phy_lane_get_port(int unit,
                       int lanePort,
                       int isRx,
                       uint32 *lport)
{
    int first_phy_port, first_log_port, lane_indx = 0;
    int adjusted_lane = 0;
    int port = lanePort;
    int tmp_phy_port = port;
    int num_lanes = 4;
    uint32 flags = 0;

    first_phy_port = (((port - 1)/num_lanes) * num_lanes) + 1;
    adjusted_lane = port - first_phy_port;

    port = first_phy_port;
    while (port < (first_phy_port+num_lanes)) {
        first_log_port = SOC_INFO(unit).port_p2l_mapping[port++];
        if (first_log_port!=-1) {
            break;
        }
    }

#ifdef PORTMOD_SUPPORT
    if (soc_feature(unit, soc_feature_portmod)) {
        phymod_lane_map_t lane_map;
        if (!SOC_PORT_VALID(unit,first_log_port)) {
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(portmod_port_lane_map_get(unit, first_log_port, flags, &lane_map));
        for (lane_indx = 0; lane_indx < num_lanes; lane_indx++) {
            if (isRx) {
                if (lane_map.lane_map_rx[lane_indx] == adjusted_lane) {
                    break;
                }
            } else {
                if (lane_map.lane_map_tx[lane_indx] == adjusted_lane) {
                    break;
                }
            }
        }
    }
#endif
    tmp_phy_port = first_phy_port + lane_indx;
    bsl_printf(" ### _bcm_synce_phy_lane_get_port: first_phy_port:%d lane_indx:%d\n", first_phy_port, lane_indx);
    if (lport) {
        *lport = SOC_INFO(unit).port_p2l_mapping[tmp_phy_port];;
    }

    return BCM_E_NONE;
}

int
_bcm_esw_synce_phy_port_get_lane(int unit,
    int port,
    int rx_lane,
    int *adjusted_lane,
    int *adjusted_phy_port)
{
    /* Currently supported only for Monterey;
    * In order to extend it to various other devices and portmacros
    * take into account num_lanes portmod_support */

    int first_phy_port, first_log_port, lane = 0;
    uint32 flags = 0;
    int num_lanes = 4;

    int phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    first_phy_port = (SOC_INFO(unit).port_serdes[port] * num_lanes) + 1;
    first_log_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
#ifdef PORTMOD_SUPPORT
    if (soc_feature(unit, soc_feature_portmod)) {
        phymod_lane_map_t lane_map;
        if (!SOC_PORT_VALID(unit,first_log_port)) {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(portmod_port_lane_map_get(unit, first_log_port, flags, &lane_map));
        if (rx_lane) {
            lane = lane_map.lane_map_rx[(phy_port - 1) % num_lanes];
        } else {
            lane = lane_map.lane_map_tx[(phy_port - 1) % num_lanes];
        }
    }
#endif

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
_bcm_esw_gdpll_portmap_get(int unit, bcm_port_t port, int *pGdpllPort, int isRx)
{
    int rv  = BCM_E_NONE;

    /* Check if the port is valid */
    if (port <= 0) {
        return BCM_E_PORT;
    }

    if (NULL == pGdpllPort) {
        return BCM_E_PARAM;
    }

    /* Following is the mapping in Monterey */
    if ((port >= 1) && (port <= BCM_GDPLL_NUM_PORTS)) {
        rv = _bcm_esw_synce_phy_port_get_lane(unit, (int)port, isRx, NULL, pGdpllPort);
        *pGdpllPort -= 1;
        bsl_printf("### _bcm_esw_gdpll_portmap_get: lPort:%d gdpllPort:%d isRx:%d\n", port, *pGdpllPort, isRx);
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_bcm_esw_gdpll_portmap_get: Error !! Invalid port\n")));
        *pGdpllPort = BCM_GDPLL_NUM_PORTS;
    }

    return rv;
}

STATIC int
_bcm_esw_port_get(int unit, int gdpllPort, bcm_port_t *pPort, int isRx)
{
    int rv  = BCM_E_NONE;

    /* Following is the mapping in Monterey */
    if ((gdpllPort >= 0) && (gdpllPort <= (BCM_GDPLL_NUM_PORTS-1))) {
        _bcm_esw_synce_phy_lane_get_port(unit, gdpllPort+1, isRx, (uint32 *)pPort);
        bsl_printf("### _bcm_esw_port_get: gdpllPort:%d lPort:%d isRx:%d\n", gdpllPort, *pPort, isRx);
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_bcm_esw_port_get: Error !! Invalid gdpll port\n")));
        rv = BCM_E_PARAM;
    }

    return rv;
}

STATIC int
_bcm_esw_gdpll_event_divisor_misc_set(int unit, bcm_gdpll_input_event_t event_misc,
                            uint32 divisor)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;

    switch (event_misc) {
        case bcmGdpllInputEventCpu:
        case bcmGdpllInputEventBS0HB:
        case bcmGdpllInputEventBS1HB:
        case bcmGdpllInputEventIPDM0:
        case bcmGdpllInputEventIPDM1:
        case bcmGdpllInputEventTS1TS:
        case bcmGdpllInputEventRSVD1IF:
        case bcmGdpllInputEventRSVD1RF:
        case bcmGdpllInputEventBS0ConvTC:
        case bcmGdpllInputEventBS1ConvTC:
            /* No divider */
            bsl_printf("GDPLL: event_divisor_misc_set: No divider for event:%d \n", event_misc);
            break;

        case bcmGdpllInputEventGPIO0:
        case bcmGdpllInputEventGPIO1:
        case bcmGdpllInputEventGPIO2:
        case bcmGdpllInputEventGPIO3:
        case bcmGdpllInputEventGPIO4:
        case bcmGdpllInputEventGPIO5:
            idx = event_misc - bcmGdpllInputEventGPIO0;
            READ_NS_REGr(unit, gpio_regs[idx], 0, &regVal);
            soc_reg_field_set(unit,  gpio_regs[idx], &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, gpio_regs[idx], 0, regVal);
            break;

        case bcmGdpllInputEventBS0PLL:
        case bcmGdpllInputEventBS1PLL:
            idx = event_misc - bcmGdpllInputEventBS0PLL;
            READ_NS_REGr(unit, bs_pll_regs[idx], 0, &regVal);
            soc_reg_field_set(unit, bs_pll_regs[idx], &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, bs_pll_regs[idx], 0, regVal);
            break;

        case bcmGdpllInputEventLCPLL0:
        case bcmGdpllInputEventLCPLL1:
        case bcmGdpllInputEventLCPLL2:
        case bcmGdpllInputEventLCPLL3:
            idx = event_misc - bcmGdpllInputEventLCPLL0;
            READ_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, regVal);
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
_bcm_esw_gdpll_event_divisor_misc_get(int unit, bcm_gdpll_input_event_t event_misc,
                            uint32 *pDivisor)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_divisor_port_set(int unit, bcm_port_t port,
    bcm_gdpll_port_event_t port_event_type, uint32 divisor)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;

    if (port > BCM_GDPLL_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_port_set: Error !! wrong port \n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    idx = port;
    switch (port_event_type) {
        case bcmGdpllPortEventRXSOF:
        case bcmGdpllPortEventTXSOF:
        case bcmGdpllPortEventROE:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "event_divisor_port_set: Error !! No divisor\n")));
            rv = BCM_E_UNAVAIL;
            break;

        case bcmGdpllPortEventRXCDR:
            READ_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, regVal);
            /* When the RXCDR(RX clock) is off, the toggle flop in HW requires 3-writes to
             * NS_TIMESYNC_RX_CDR_EVENT_CTRL register to ensure the write has gone through
             * and the logic works as expected when RX clock comes active
             * Ref: SDK-178135
             */
            WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, idx, regVal);
            break;

        case bcmGdpllPortEventTXPI:
            READ_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, &regVal,
                              DIVISORf, divisor);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, idx, regVal);
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
_bcm_esw_gdpll_event_divisor_port_get(int unit, bcm_port_t port,
                     bcm_gdpll_port_event_t port_event_type,
                     uint32 *pDivisor)
{
    int rv  = BCM_E_NONE;
    return (rv);
}

/* Mapping of input events to eventId will be device specific */
STATIC int
_bcm_esw_gdpll_input_eventId_get(int unit,
                    bcm_gdpll_chan_input_t *pInputEvent,
                    uint32 *pEventId)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if ((pEventId==NULL) || (pGdpllContext==NULL) ||
        (pInputEvent->input_event < bcmGdpllInputEventCpu) ||
        (pInputEvent->input_event >= bcmGdpllInputEventMax)) {
         LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_eventId_get: Error !! Wrong event\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* EventId 0 to 191 for port events,
     * EventId 192 to 213 for EventCpu to EventLCPLL3
     * EventId 214 for R5
     */
    switch ((int)pInputEvent->input_event) {
        case bcmGdpllInputEventPORT:
            if (pInputEvent->port < 0 || pInputEvent->port > BCM_GDPLL_NUM_PORTS){
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_eventId_get: Error !! Wrong port\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }

            if ((pInputEvent->port_event_type == bcmGdpllPortEventRXCDR) ||
                (pInputEvent->port_event_type == bcmGdpllPortEventRXSOF)){
                /* Event could be TXSOF or TXPI based on port type */
                idx = pInputEvent->port*2;
            } else if (pInputEvent->port_event_type == bcmGdpllPortEventROE) {
                idx = pInputEvent->port*2 + 1;
            } else if ((pInputEvent->port_event_type == bcmGdpllPortEventTXSOF) ||
                       (pInputEvent->port_event_type == bcmGdpllPortEventTXPI)){
                /* Event could be TXSOF or TXPI based on port type */
                idx = BCM_GDPLL_IA_START_TXPI_TXSOF + pInputEvent->port;
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_eventId_get: Error !! Wrong param\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }
            break;

        case bcmGdpllInputEventR5:
            idx = BCM_GDPLL_IA_START_R5;
            break;

        default:
            idx = BCM_GDPLL_IA_START_MISC + pInputEvent->input_event;
    }

    *pEventId = idx;

exit:
    return rv;
}

/* Get the event based on the eventId */
STATIC int
_bcm_esw_gdpll_input_event_get(int unit,
                    int chan, int isFb,
                    bcm_gdpll_input_event_t *pEvent_misc,
                    bcm_gdpll_port_event_t *pPort_event_type,
                    bcm_port_t *pPort)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    bcm_port_t port;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint32 eventId = isFb ? pGdpllContext->dpll_chan[chan].eventId_fb:
                pGdpllContext->dpll_chan[chan].eventId_ref;

    if (eventId >= BCM_GDPLL_NUM_INPUT_EVENTS){
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "input_event_get: Error !! Wrong param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    port = isFb ? pGdpllContext->dpll_chan[chan].lport_fb:pGdpllContext->dpll_chan[chan].lport_ref;
    *pPort = isFb ? pGdpllContext->dpll_chan[chan].gdpllport_fb:pGdpllContext->dpll_chan[chan].gdpllport_ref;

    if (eventId >= BCM_GDPLL_IA_START_R5) {
        /* Event from R5 */
        *pEvent_misc = bcmGdpllInputEventR5;

    } else if (eventId >= BCM_GDPLL_IA_START_MISC) {
        /* Miscellaneous events */
        *pEvent_misc = eventId - BCM_GDPLL_IA_START_MISC;

    } else if (eventId >= BCM_GDPLL_IA_START_TXPI_TXSOF) {
        /* Its a TXPI or TXSOF event based on the port */
        *pEvent_misc = bcmGdpllInputEventPORT;
        if (IS_ROE_PORT(unit, port)) {
            *pPort_event_type = bcmGdpllPortEventTXSOF;
        } else {
            *pPort_event_type = bcmGdpllPortEventTXPI;
        }
    } else {
        /* Its a TXPI or TXSOF event based on the port */
        *pEvent_misc = bcmGdpllInputEventPORT;
        idx = eventId%2;

        if (IS_ROE_PORT(unit, port)) {
            *pPort_event_type = idx ? bcmGdpllPortEventROE : bcmGdpllPortEventRXSOF;
         } else {
            if (idx) {
                /* Unused event ID */
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "input_event_get: Error !! Unused event ID\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }
            *pPort_event_type = bcmGdpllPortEventRXCDR;
         }
    }

exit:
    return rv;
}

STATIC int
_bcm_esw_gdpll_input_eventId_inUse(int unit,
                    uint32 eventId, int *pIsUse)
{
    int rv  = BCM_E_NONE;
    int chan;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    /* Since the event from R5 can be for multiple outpts, skip the check */
    if (eventId == BCM_GDPLL_IA_START_R5) {
        *pIsUse = 0;
        goto exit;
    }

    /* Check if this event is ref or fb for any of the active channels */
    for (chan=0; chan<BCM_GDPLL_NUM_CHANNELS; chan++) {
        if (pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_CHAN_ALLOC) {
            if ((pGdpllContext->dpll_chan[chan].eventId_ref == eventId) ||
                (pGdpllContext->dpll_chan[chan].eventId_fb == eventId)) {
                /* This event is already used by chan */
                *pIsUse = 1;
                bsl_printf("GDPLL: eventId:%d is under use by chan:%d\n", eventId, chan);
                goto exit;
            }
        }
    }

    *pIsUse = 0;

exit:
    return rv;
}

STATIC int
_bcm_esw_gdpll_input_array_alloc(int unit, uint32 eventId, uint32 *pIaAddr)
{
    int rv  = BCM_E_NONE;
    int ia_loc;

    /*
     * Findout if this event is already under use by active channel
     * If not, findout the free ia address from the pool and allocate,
     *
     * Monterey have event id's less than the input array size.
     * Hence let the event id be the input array address for now
     * HW team recommends the eventId to be same as iaAddr
     */

    if (eventId == 214/*BCM_GDPLL_IA_START_R5, R5-event */) {
        /* For now, the IA locations assigned is from 214 */
        int ia_found = 0;
        int instance_count;
        for (ia_loc = eventId; ia_loc < BCM_GDPLL_NUM_IA_ENTRIES; ia_loc++) {
            for (instance_count = 0; instance_count < NUM_TDPLL_INSTANCE; instance_count++) {
                if (gdpll_tdpll_state.tdpll_instace[instance_count] != ia_loc)
                    continue;
            }
            if (instance_count == NUM_TDPLL_INSTANCE) {
                /* Found the ia location */
                ia_found = 1;
                break;
            }

            continue;
        }

        if (!ia_found) {
            bsl_printf("_bcm_esw_gdpll_input_array_alloc: Error!! IA location for R5 is not found\n");
            rv = BCM_E_FULL;
        } else {
            *pIaAddr = ia_loc;
            bsl_printf("_bcm_esw_gdpll_input_array_alloc: ia location for R5 is:%d\n", ia_loc);
        }

    } else {
        *pIaAddr = eventId;
    }

    return rv;
}

STATIC int
_bcm_esw_gdpll_input_array_free(int unit, uint32 eventId)
{
    int rv  = BCM_E_NONE;
    /*
     * Figure out if this eventId is under use by any active channel
     * If not, free the input array address assocuated with this event
     */
    return rv;
}


STATIC int
_bcm_esw_gdpll_event_dest_misc_set(int unit, bcm_gdpll_input_event_t event_misc,
                            gdpll_event_dest_cfg_t *pEventDest,
                            uint32 eventId, uint32 *pIaAddr)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;
    uint32 dest;
    uint32 chkProfile;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (event_misc < bcmGdpllInputEventCpu ||
        event_misc > bcmGdpllInputEventLCPLL3) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_misc_set: Error !! Wrong param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check if the event is already enabled, that may indicate
     * that it will be used by some active GDPLL channel.
     * if so, return error
     */
    /* Check if this event is forwared to any of IA location */
    if ((pGdpllContext->eventId[eventId] >= 0) &&
        (pGdpllContext->eventId[eventId] < BCM_GDPLL_NUM_IA_ENTRIES)) {
        /* This event is already forwarded to IA location and could
         * be in use by an active GDPLL channel.
         * Hence it cant be forwarded
         */
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_misc_set: Error !! Event busy eventId:%d IA:%d\n"), eventId, pGdpllContext->eventId[eventId]));
        rv = BCM_E_BUSY;
        goto exit;
    }

    /* Get the free Input array address */
    BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_input_array_alloc(unit,
        eventId, pIaAddr));

    /* Since we have 150 numels of NS_TIMESYNC_TS_EVENT_FWD_CFG,
     * the miscellaneous events starts from index 128 per table 6.4 in uArch
     */
    idx = 128 + event_misc;

    if (pEventDest->event_dest == bcmGdpllEventDestCPU) {
        dest = EVENT_TS_TO_CPU;
    }else if (pEventDest->event_dest == bcmGdpllEventDestM7) {
        dest = EVENT_TS_TO_M7;
    }else if (pEventDest->event_dest == bcmGdpllEventDestALL) {
        dest = EVENT_TS_TO_ALL;
    }else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_misc_set: Error !! wrong event dest\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    
    chkProfile = 0;

    READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, &regVal);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
    dest |= soc_reg_field_get(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                            regVal, DESTf);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DESTf, dest);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DPLL_ADDRf, *pIaAddr);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              GDPLL_TSf, pEventDest->ts_counter ? 1:0);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              SRC_TS_COUNTERf, pEventDest->ts_counter ? 1:0);
    soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
    WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_dest_misc_get (int unit, bcm_gdpll_input_event_t  event_misc,
                             gdpll_event_dest_cfg_t *pEventDest)
{
    int rv  = BCM_E_NONE;
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_dest_port_set (int unit, bcm_port_t port,
                bcm_gdpll_port_event_t port_event_type,
                gdpll_event_dest_cfg_t *pEventDest,
                uint32 eventId, uint32 *pIaAddr)
{
    int rv  = BCM_E_NONE;
    int idx = 0;
    uint32 regVal = 0;
    uint32 dest;
    uint32 chkProfile;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (port >= BCM_GDPLL_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_port_set: Error !! wrong param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check if the event is already enabled, that may indicate
     * it will be used by some active GDPLL channel.
     * if so, return error
     */
    if ((pGdpllContext->eventId[eventId] >= 0) &&
        (pGdpllContext->eventId[eventId] < BCM_GDPLL_NUM_IA_ENTRIES)) {
        /* This event is already forwarded IA location and could be in use
         * by an active GDPLL channel. Hence it cant be forwarded
         */
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_port_set: Error !! event busy\n")));
        rv = BCM_E_BUSY;
        goto exit;
    }

    /* Get the free Input array address */
    BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_input_array_alloc(unit,
        eventId, pIaAddr));

    if (pEventDest->event_dest == bcmGdpllEventDestCPU) {
        dest = EVENT_TS_TO_CPU;
    }else if (pEventDest->event_dest == bcmGdpllEventDestM7) {
        dest = EVENT_TS_TO_M7;
    }else if (pEventDest->event_dest == bcmGdpllEventDestALL) {
        dest = EVENT_TS_TO_ALL;
    }else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "event_dest_port_set: Error !! wrong event dest\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    
    chkProfile = 0;

    switch(port_event_type) {
        case bcmGdpllPortEventRXCDR:
        case bcmGdpllPortEventTXPI:
            if (port_event_type == bcmGdpllPortEventRXCDR){
                idx = port;
            }else if (port_event_type == bcmGdpllPortEventTXPI) {
                idx = port + BCM_GDPLL_NUM_PORTS;
            }

            bsl_printf("GDPLL: Setting NS_TIMESYNC_TS_EVENT_FWD_CFG idx:%d IA-Addr:%d\n",
                        idx, *pIaAddr);

            READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
            dest |= soc_reg_field_get(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr,
                                    regVal, DESTf);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DESTf, dest);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              DPLL_ADDRf, *pIaAddr);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              GDPLL_TSf, pEventDest->ts_counter ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              SRC_TS_COUNTERf, pEventDest->ts_counter ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, idx, regVal);

            break;

        case bcmGdpllPortEventRXSOF:    /* Type-0 */
        case bcmGdpllPortEventTXSOF:    /* Type-1 */
        case bcmGdpllPortEventROE:      /* Type-2 */
            if (SOC_IS_MONTEREY(unit)) {
                /* Nanosync support 32-CPRI ports for timestamping
                 * Monterey implements below 24-CPRI port maps to Nanosync
                 *   Phys-Port(phys)   GDPLL_PORT   CPRI_GDPLL_Port(TXSOF,RXSOF,ROE)
                 *       1-12            0-11          0-11
                 *      53-64            52-63        12-23
                 */
                if (port <= 11) {
                } else if ((port >= 52) && (port <= 63)){
                    port = port - 52 + 12;
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                          "event_dest_port_set: Error !! Invalid CPRI ref-port:%d\n"),
                              port));
                    rv = BCM_E_PARAM;
                    goto exit;
                }
            }

            /*
             * Having 128 Numels, register map is
             * Port-0: type-0, type-1, type-2, unused
             * Port-1: type-0, type-1, type-2, unused
             */
            if ((port >= BCM_GDPLL_NUM_CPRI_PORTS) || (port < 0)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_dest_port_set: Error !! Invalid CPRI port associated to gdpllPort:%d\n"), port));
                rv = BCM_E_PARAM;
                goto exit;
            }

            idx = port*4;
            if (port_event_type == bcmGdpllPortEventTXSOF) {
                idx = idx + 1;
            }else if (port_event_type == bcmGdpllPortEventROE) {
                idx = idx + 2;
            }

            bsl_printf("GDPLL: Setting NS_TIMESYNC_MAPPER_FWD_CFG idx:%d, IA-Addr:%d\n",
                        idx, *pIaAddr);
            READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              EVENT_IDf, eventId);
            dest |= soc_reg_field_get(unit, NS_TIMESYNC_MAPPER_FWD_CFGr,
                                    regVal, DESTf);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              DESTf, dest);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              DPLL_ADDRf, *pIaAddr);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              PPM_CHECK_ENABLEf, pEventDest->ppm_check_enable ? 1:0);
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              SRC_TS_COUNTERf, 1);  /* It is TS1 counter */
            soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, &regVal,
                              CHK_PROFILEf, chkProfile);
            WRITE_NS_REGr(unit, NS_TIMESYNC_MAPPER_FWD_CFGr, idx, regVal);
            break;

        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_dest_port_set: Error !! wrong event\n")));
            rv = BCM_E_PARAM;
            goto exit;
    }

    /* eventId got set the forwarding to iaAddr */
    bsl_printf("GDPLL: Event Id populated :%d\n",*pIaAddr);
    pGdpllContext->eventId[eventId] = *pIaAddr;

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_dest_port_get(int unit, bcm_port_t port, bcm_gdpll_port_event_t port_event_type,
                             gdpll_event_dest_cfg_t  *pEventDest)
{
    int rv = BCM_E_NONE;
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_enable_misc_set(int unit, bcm_gdpll_input_event_t  event_misc,
                               int enable)
{
    int rv = BCM_E_NONE;
    uint32 idx = 0;
    uint32 enable_field = 0;
    uint32 regVal = 0;
    uint32 reg_enable;

    /*
    NS_MISC_CLK_EVENT_CTRL
    NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRL
    NS_BROADSYNC0_CLK_EVENT_CTRL
    NS_BROADSYNC1_CLK_EVENT_CTRL
    NS_TIMESYNC_GPIO_x_CTRL
     */
    switch (event_misc){
        case bcmGdpllInputEventCpu:
            if (event_misc == bcmGdpllInputEventCpu) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_CPU;
            }
        case bcmGdpllInputEventBS0HB:
            if (event_misc == bcmGdpllInputEventBS0HB) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS0HB;
            }
        case bcmGdpllInputEventBS1HB:
            if (event_misc == bcmGdpllInputEventBS1HB) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS1HB;
            }
        case bcmGdpllInputEventIPDM0:
            if (event_misc == bcmGdpllInputEventIPDM0) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_IPDM0;
            }
        case bcmGdpllInputEventIPDM1:
            if (event_misc == bcmGdpllInputEventIPDM1) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_IPDM1;
            }
        case bcmGdpllInputEventTS1TS:
            if (event_misc == bcmGdpllInputEventTS1TS) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_TS1;
            }
        case bcmGdpllInputEventRSVD1IF:
            if (event_misc == bcmGdpllInputEventRSVD1IF) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_RSVD1IF;
            }
        case bcmGdpllInputEventRSVD1RF:
            if (event_misc == bcmGdpllInputEventRSVD1RF) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_RSVD1RF;
            }
        case bcmGdpllInputEventBS0ConvTC:
            if (event_misc == bcmGdpllInputEventBS0ConvTC) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS0CONV;
            }
        case bcmGdpllInputEventBS1ConvTC:
            if (event_misc == bcmGdpllInputEventBS1ConvTC) {
                enable_field = BCM_GDPLL_MISC_EVENT_EN_BS1CONV;
            }

            /* Write the enable flag */
            READ_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, &regVal);
            reg_enable = soc_reg_field_get(unit, NS_MISC_CLK_EVENT_CTRLr, regVal, ENABLEf);

            reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

            soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal,
                              ENABLEf, reg_enable);
            WRITE_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, regVal);
            break;

        case bcmGdpllInputEventGPIO0:
        case bcmGdpllInputEventGPIO1:
        case bcmGdpllInputEventGPIO2:
        case bcmGdpllInputEventGPIO3:
        case bcmGdpllInputEventGPIO4:
        case bcmGdpllInputEventGPIO5:
            idx = event_misc - bcmGdpllInputEventGPIO0;
            READ_NS_REGr(unit, gpio_regs[idx], 0, &regVal);
            /*  soc_reg_field_set(unit,  gpio_regs[idx], &regVal,
                               CAPTURE_ENABLEf, enable ? 1:0); */
            WRITE_NS_REGr(unit, gpio_regs[idx], 0, regVal);
            break;

        case bcmGdpllInputEventBS0PLL:
        case bcmGdpllInputEventBS1PLL:
            idx = event_misc - bcmGdpllInputEventBS0PLL;
            READ_NS_REGr(unit, bs_pll_regs[idx], 0, &regVal);
            soc_reg_field_set(unit, bs_pll_regs[idx], &regVal,
                              ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, bs_pll_regs[idx], 0, regVal);

            if (event_misc == bcmGdpllInputEventBS0PLL) {
                READ_NS_REGr(unit, NS_BROADSYNC_SYNC_MODEr, 0, &regVal);
                reg_enable = soc_reg_field_get(unit, NS_BROADSYNC_SYNC_MODEr, regVal, SYNC_MODEf);
                if (reg_enable) {
                    bcm_time_ts_counter_t counter;

                    counter.ts_counter= 1; /* TS1 */
                    rv = bcm_time_ts_counter_get(unit, &counter);
                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_PTP,
                                    (BSL_META_U(unit,
                                    "event_enable_misc_set: Error !! bcm_time_ts_counter_get\n")));
                    }

                    READ_NS_REGr(unit, NS_BS0_BS_CLK_CTRLr, 0, &regVal);
                    soc_reg_field_set(unit, NS_BS0_BS_CLK_CTRLr, &regVal, ENABLEf, enable ? 1:0);
                    WRITE_NS_REGr(unit, NS_BS0_BS_CLK_CTRLr, 0, regVal);

                    LOG_VERBOSE(BSL_LS_BCM_PTP,
                                    (BSL_META_U(unit,
                                    " event_enable_misc_set:[%d] CurTime[%llu/0x%llx] \n"), event_misc,
                                    (long long unsigned int)counter.ts_counter_ns,
                                    (long long unsigned int)counter.ts_counter_ns));
                }
            } else if (event_misc == bcmGdpllInputEventBS1PLL) {
                READ_NS_REGr(unit, NS_COMMON_CTRLr, 0, &regVal);
                reg_enable = soc_reg_field_get(unit, NS_COMMON_CTRLr, regVal, RESERVEDf);
                reg_enable = ((reg_enable>>26) & 0x1);
                if (reg_enable) {
                    bcm_time_ts_counter_t counter;

                    counter.ts_counter= 1; /* TS1 */
                    rv = bcm_time_ts_counter_get(unit, &counter);
                    if (rv != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_PTP,
                                    (BSL_META_U(unit,
                                    "event_enable_misc_set: Error !! bcm_time_ts_counter_get\n")));
                    }

                    READ_NS_REGr(unit, NS_BS1_BS_CLK_CTRLr, 0, &regVal);
                    soc_reg_field_set(unit, NS_BS1_BS_CLK_CTRLr, &regVal, ENABLEf, enable ? 1:0);
                    WRITE_NS_REGr(unit, NS_BS1_BS_CLK_CTRLr, 0, regVal);

                    LOG_VERBOSE(BSL_LS_BCM_PTP,
                                    (BSL_META_U(unit,
                                    " event_enable_misc_set:[%d] CurTime[%llu/0x%llx] \n"), event_misc,
                                    (long long unsigned int)counter.ts_counter_ns,
                                    (long long unsigned int)counter.ts_counter_ns));
                }
            }

            break;

        case bcmGdpllInputEventLCPLL0:
        case bcmGdpllInputEventLCPLL1:
        case bcmGdpllInputEventLCPLL2:
        case bcmGdpllInputEventLCPLL3:
            idx = event_misc - bcmGdpllInputEventLCPLL0;
            READ_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, &regVal,
                              ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, NS_TIMESYNC_SERDES_LCPLL_EVENT_CTRLr, idx, regVal);
            break;

        case bcmGdpllInputEventPORT:
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
_bcm_esw_gdpll_event_enable_misc_get(int unit, bcm_gdpll_input_event_t  event_misc,
                              int *pEnable)
{
    int rv = BCM_E_NONE;
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_enable_port_set(int unit, bcm_port_t port, bcm_gdpll_port_event_t port_event_type,
                                 int enable)
{
    int rv = BCM_E_NONE;
    uint32 mapper_reg;
    uint32 regVal = 0;
    uint32 enable_field = 0;
    uint32 reg_enable = 0;

    if (port >= BCM_GDPLL_NUM_PORTS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "event_enable_port_set: Error !! wrong port\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    switch (port_event_type) {
        case bcmGdpllPortEventRXSOF:
        case bcmGdpllPortEventTXSOF:
        case bcmGdpllPortEventROE:
            if (SOC_IS_MONTEREY(unit)) {
                /* Nanosync support 32-CPRI ports for timestamping
                 * Monterey implements below 24-CPRI port maps to Nanosync
                 *   Phys-Port(phys)   GDPLL_PORT   CPRI_GDPLL_Port(TXSOF,RXSOF,ROE)
                 *       1-12            0-11          0-11
                 *      53-64            52-63        12-23
                 */
                if (port <= 11) {
                } else if ((port >= 52) && (port <= 63)){
                    port = port - 52 + 12;
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                          "event_enable_port_set: Error !! Invalid CPRI ref-port:%d\n"),
                              port));
                    rv = BCM_E_PARAM;
                    goto exit;
                }
            }

            /* GDPLL has support to 32 CPRI ports */
            if ((port >= BCM_GDPLL_NUM_CPRI_PORTS) || (port < 0)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "event_enable_port_set: Error !! wrong port or non-cpri\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }

            /* Program NS_TIMESYNC_MAPPER_PORT_ENABLE_x */
            enable_field = 1 << (port_event_type - bcmGdpllPortEventRXSOF);
            mapper_reg = port/8;

            READ_NS_REGr(unit, mapper_port_enable_regs[mapper_reg], 0, &regVal);
            reg_enable = soc_reg_field_get(unit, mapper_port_enable_regs[mapper_reg], regVal, mapper_port_enable[port]);

            reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

            soc_reg_field_set(unit, mapper_port_enable_regs[mapper_reg], &regVal, mapper_port_enable[port], reg_enable);
            WRITE_NS_REGr(unit, mapper_port_enable_regs[mapper_reg], 0, regVal);

            break;

        case bcmGdpllPortEventRXCDR:
            /* Program NS_TIMESYNC_RX_CDR_EVENT_CTRL */
            READ_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, port, &regVal);
            if (soc_reg_field_get(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, regVal, ENABLEf) != enable) {
                soc_reg_field_set(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, &regVal, ENABLEf, enable ? 1:0);
                WRITE_NS_REGr(unit, NS_TIMESYNC_RX_CDR_EVENT_CTRLr, port, regVal);
            }
            break;

        case bcmGdpllPortEventTXPI:
            /* Program NS_TIMESYNC_TX_PI_CLK_EVENT_CTRL */
            READ_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, port, &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, &regVal, ENABLEf, enable ? 1:0);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TX_PI_CLK_EVENT_CTRLr, port, regVal);
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
_bcm_esw_gdpll_event_enable_port_get(int unit, bcm_port_t port, bcm_gdpll_port_event_t port_event_type,
                              int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_roe_52b_set(int unit, bcm_port_t port, int enable)
{
    int rv = BCM_E_NONE;
    uint32 enable_field = 0;
    uint32 regVal = 0;
    uint32 reg_enable = 0;

    /* Program NS_TIMESYNC_MAPPER_TYPE2_FORMAT */

    enable_field = 1 << port;

    READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, port, &regVal);
    reg_enable = soc_reg_field_get(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, regVal, CONV_ENf);

    reg_enable = enable ? (reg_enable | enable_field) : (reg_enable & (~enable_field));

    soc_reg_field_set(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, &regVal, CONV_ENf, reg_enable);
    WRITE_NS_REGr(unit, NS_TIMESYNC_MAPPER_TYPE2_FORMATr, port, regVal);

    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_roe_52b_get(int unit, bcm_port_t port, int *pEnable)
{
    int rv = BCM_E_NONE;
    /* NS_TIMESYNC_MAPPER_TYPE2_FORMAT */

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_update_set(int unit, int chan)
{
    /* Update NS_GDPLL_IA_UPDATE_CONFIG */
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint32 eventId_fb;
    uint32 eventId_ref;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_update_set: Error !! wrong channel\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    eventId_fb = pGdpllContext->dpll_chan[chan].eventId_fb;
    eventId_ref = pGdpllContext->dpll_chan[chan].eventId_ref;

    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);

    /* Set the feedback for the channel */
    if ((eventId_fb < BCM_GDPLL_NUM_INPUT_EVENTS) &&
        (pGdpllContext->eventId[eventId_fb] < BCM_GDPLL_NUM_IA_ENTRIES)){
        bsl_printf("GDPLL: chan_update: feedback Id:0x%x\n", pGdpllContext->eventId[eventId_fb]);
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, FEEDBACK_IDf,
                    pGdpllContext->eventId[eventId_fb]);
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_update_set: Error !! fb event not found\n")));
        rv = BCM_E_NOT_FOUND;
        goto exit;
    }

    /* Set the reference for the channel */
    if (pGdpllContext->dpll_chan[chan].phaseConterRef == 0){
        if ((eventId_ref < BCM_GDPLL_NUM_INPUT_EVENTS) &&
            (pGdpllContext->eventId[eventId_ref] < BCM_GDPLL_NUM_IA_ENTRIES)){
            bsl_printf("GDPLL: chan_update: ref Id:0x%x\n", pGdpllContext->eventId[eventId_ref]);
            soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, REFERENCE_IDf,
                    pGdpllContext->eventId[eventId_ref]);
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_update_set: Error !! ref event not found\n")));
            rv = BCM_E_NOT_FOUND;
            goto exit;
        }
    }

    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal);

exit:
    return (rv);
}

int
_bcm_esw_gdpll_chan_get(int unit, int chan,
                           bcm_gdpll_chan_t *pGdpllChan)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    *pGdpllChan = pGdpllContext->dpll_chan[chan].gdpll_chan_config;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_enable_set(int unit, int chan, int enable)
{
   /* NS_GDPLL_IA_UPDATE_CONFIG */
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_enable_set: Error !! invalid channel\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (enable) {
        READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
        if (soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal, ENABLEf)) {
            /* Already in enable state */
            return rv;
        }

        /* Clear the status flags if any */
        WRITE_NS_REGr(unit, pair_available_regs[chan/32], 0, 1<<(chan%32));
        WRITE_NS_REGr(unit, ia_in_status_regs[chan/32], 0, 1<<(chan%32));
    }

    bsl_printf("GDPLL: Set enable:%d on GDPLL channel:%d\n", enable, chan);
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, ENABLEf, enable ? 1:0);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_enable_get(int unit, int chan, int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_capture_enable_m7_set(int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    /* NS_MISC_CLK_EVENT_CTRL */
    bsl_printf("GDPLL: gdpll_capture_enable_m7_set enable:%d\n", enable);

    READ_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, &regVal);
    soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal, GDPLL_CAPTURE_ENABLEf, enable ? 1:0);
    WRITE_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, regVal);

    return (rv);
}

STATIC int
_bcm_esw_gdpll_capture_enable_m7_get(int unit, int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}

int
_bcm_esw_gdpll_capture_enable_cpu_set(int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    READ_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, &regVal);
    soc_reg_field_set(unit, NS_MISC_CLK_EVENT_CTRLr, &regVal, TIME_CAPTURE_ENABLEf, enable ? 1:0);
    WRITE_NS_REGr(unit, NS_MISC_CLK_EVENT_CTRLr, 0, regVal);

    return (rv);
}

STATIC int
_bcm_esw_gdpll_capture_enable_cpu_get(int unit, int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_flush(int unit)
{
    int rv = BCM_E_NONE;
    /* NS_GDPLL_GDPLL_COMMON_CTRL */

    WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_CTRLr, 0, 1);
    sal_usleep(100);
    WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_CTRLr, 0, 0);

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_txpi_set(int unit, int chan, bcm_port_t port)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    if ((chan >= BCM_GDPLL_NUM_CHANNELS) &&
        (port >= BCM_GDPLL_NUM_PORTS)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_out_txpi_set: Error !! invalid param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* NS_GDPLL_NCO_UPDATE_CONTROL */
    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_IDf, port);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPEf, 1);
    soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_TYPEf, 0);
    WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_txpi_get(int unit, int chan, bcm_port_t *pPort)
{
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_misc_set(int unit, int chan,
                        bcm_gdpll_output_event_t event)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 dest_hw_id = 0;
    uint32 dest_hw_type = 0;
#ifdef BCN_WAR
    uint32 dpllAddr, addrOffset;
    uint32 enableReg = 0;
#endif

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_out_misc_set: Error !! invalid param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    switch ((int)event) {
        case bcmGdpllOutputEventBSPLL0:
        case bcmGdpllOutputEventBSPLL1:
        case bcmGdpllOutputEventLCPLL0:
        case bcmGdpllOutputEventLCPLL1:
        case bcmGdpllOutputEventLCPLL2:
        case bcmGdpllOutputEventLCPLL3:
            bsl_printf("GDPLL: Setting NCO out control for BSPLL/LCPLL\n");
            dest_hw_id = BCM_GDPLL_NUM_PORTS +
                    (event - bcmGdpllOutputEventBSPLL0);
            dest_hw_type = 0;
            break;

        case bcmGdpllOutputEventTS0:
        case bcmGdpllOutputEventTS1:
            bsl_printf("GDPLL: Setting NCO out control for TS0/TS1\n");
            dest_hw_id = event - bcmGdpllOutputEventTS0;
            dest_hw_type = 1;
            break;

        /* This is supposed to be BCN counter which is not there,
         * but now R5 reads the NCO and updates the BCN counter
         */
        case bcmGdpllOutputEventBCN:
            bsl_printf("GDPLL: Setting NCO out control for BCN\n");
            dest_hw_id = 1;
            dest_hw_type = 1;
            break;

        case bcmGdpllOutputEventNTPTOD:
            bsl_printf("GDPLL: Setting NCO out control for NTP-TOD\n");
            dest_hw_id = 2;
            dest_hw_type = 1;
            break;

        case bcmGdpllOutputEventPTPTOD:
            bsl_printf("GDPLL: Setting NCO out control for PTP-TOD\n");
            dest_hw_id = 3;
            dest_hw_type = 1;
            break;

        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_out_misc_set: Error !! invalid event\n")));
            rv = BCM_E_PARAM;
            goto exit;
    }

    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, &regVal);
#ifdef BCN_WAR
    if ((event == bcmGdpllOutputEventBCN) || (event == bcmGdpllOutputEventNTPTOD) ||
        (event == bcmGdpllOutputEventPTPTOD)) {
        

        dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
        /* Set the Offset location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);

        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPEf, 0);
        if (event == bcmGdpllOutputEventBCN) {
            soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BCN_UPDATE_LOC, chan);
        } else if (event == bcmGdpllOutputEventNTPTOD) {
            soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC, chan);
            soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
            LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                                " Writing DpllOffset_addr:0x%08x@0x%08x for NTP-TOD \n"),
                     (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_NTPTOD_DPLL_OFFSET_LOC)));
        } else if (event == bcmGdpllOutputEventPTPTOD) {
            soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC, chan);
            soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
            LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                                " Writing DpllOffset_addr:0x%08x@0x%08x for PTP-TOD \n"),
                     (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_PTPTOD_DPLL_OFFSET_LOC)));
        }

        /* Enable the Channel Output array status reporting to R5 */
        READ_NS_REGr(unit, nco_out_enable_regs[chan/32], 0, &enableReg);
        enableReg |= 1<<(chan%32);
        soc_reg_field_set(unit, nco_out_enable_regs[chan/32], &enableReg, INTENf, enableReg);
        WRITE_NS_REGr(unit, nco_out_enable_regs[chan/32], 0, enableReg);

        if (chan < BCM_GDPLL_NUM_CHANNELS) {
            /* Enable in the NS interrupt line */
            sal_usleep(100);
            READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &enableReg);
            enableReg |= INTR_GDPLL_OA_0_STATUS | INTR_GDPLL_OA_1_STATUS |
                         INTR_GDPLL_OA_2_STATUS | INTR_GDPLL_OA_3_STATUS;
            WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, enableReg);
        }

    } else {
#endif
        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_IDf, dest_hw_id);
        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_TYPEf, 1);
        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_TYPEf, dest_hw_type);
#ifdef BCN_WAR
    }
#endif
    WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, chan, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_misc_get(int unit, int chan,
                        bcm_gdpll_output_event_t *pOutEvent)
{
    int rv = BCM_E_NONE;

    return (rv);
}

#if 0  
STATIC int
_bcm_esw_gdpll_chan_out_enable_set(int unit, int chan, int enable)
{
    int rv = BCM_E_NONE;
    /* NS_GDPLL_NCO_UPDATE_COMMON_CONTROL */

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_out_enable_get(int unit, int chan,
                               int *pEnable)
{
    int rv = BCM_E_NONE;

    return (rv);
}
#endif

STATIC int
soc_iproc_m7_write(int unit, uint32 addr, uint32 data)
{
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        soc_cm_iproc_write(unit, addr, data);
        return BCM_E_NONE;
    }
    return BCM_E_NONE;
}

STATIC int
soc_iproc_m7_read(int unit, uint32 addr)
{
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        return soc_cm_iproc_read(unit, addr);
    }
    return BCM_E_NONE;
}

int
_bcm_esw_gdpll_m7_status (int unit, uint8 *pBuff)
{
    int rv = BCM_E_NONE;
    int count;
    int version_size = DPLL_FW_VERSION_SIZE;
    uint32 *pVersion;

    uint32 m7AliveIndi = BCM_M7_DTCM_BASE + DPLL_ALIVE_OFFSET;
    uint32 versionAddr = BCM_M7_DTCM_BASE + DPLL_FW_VERSION_OFFSET;

    pVersion = (uint32 *)pBuff;
    if (pBuff == NULL) {
        return BCM_E_PARAM;
    }

    version_size = version_size / 4;
    soc_iproc_m7_write(unit, m7AliveIndi, 0);
    sal_usleep(100);
    if(soc_iproc_m7_read(unit, m7AliveIndi)) {
        /* M7 is alive */
        for (count=0; count < version_size; count++) {
            *pVersion = swap32(soc_iproc_m7_read(unit, versionAddr));
            versionAddr = versionAddr + 4;
            pVersion++;
        }
    } else {
        return BCM_E_FAIL;
    }

    return rv;
}

STATIC int
_bcm_esw_gdpll_chan_config_dpll_set(int unit, int chan,
                        dpll_param_t *pDpllParam)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 stateNum;
    uint32 count;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 *pData;

    if ((chan >= BCM_GDPLL_NUM_CHANNELS) ||
        (pDpllParam == NULL)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_config_dpll_set: Error !! invalid param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Clear the param memory */
    sal_memset(&m7DpllParam, 0, sizeof(m7DpllParam));

    /*  m7DpllParam.debugMode = pDpllParam->debugMode; */

    m7DpllParam.m7_dpll_config.DPLLNumOfStates = pDpllParam->dpll_config.dpll_num_states;
    for (stateNum = 0; stateNum < m7DpllParam.m7_dpll_config.DPLLNumOfStates; stateNum++) {
        m7DpllParam.m7_dpll_config.K1Shift[stateNum] = pDpllParam->dpll_config.k1Shift[stateNum];
        m7DpllParam.m7_dpll_config.K1K2Shift[stateNum] = pDpllParam->dpll_config.k1k2Shift[stateNum];

        m7DpllParam.m7_dpll_config.K1[stateNum] = swap32(pDpllParam->dpll_config.k1[stateNum]);
        m7DpllParam.m7_dpll_config.K1K2[stateNum] = swap32(pDpllParam->dpll_config.k1k2[stateNum]);
        m7DpllParam.m7_dpll_config.LockDetThres[stateNum] = swap32(pDpllParam->dpll_config.lockDetThres[stateNum]);
        m7DpllParam.m7_dpll_config.DwellCount[stateNum] = swap16(pDpllParam->dpll_config.dwell_count[stateNum]);
        u64_H(m7DpllParam.m7_dpll_config.PhaseErrorLimiterThres[stateNum]) =
                    swap64_h(pDpllParam->dpll_config.phase_error_limiter_thres[stateNum]);
        u64_L(m7DpllParam.m7_dpll_config.PhaseErrorLimiterThres[stateNum]) =
                    swap64_l(pDpllParam->dpll_config.phase_error_limiter_thres[stateNum]);
        m7DpllParam.m7_dpll_config.IdumpMod[stateNum] = swap16(pDpllParam->dpll_config.idump_mod[stateNum]);
    }

    m7DpllParam.m7_dpll_config.LockIndicatorThresholdLo = swap32(pDpllParam->dpll_config.lockIndicatorThresholdLo);
    m7DpllParam.m7_dpll_config.LockIndicatorThresholdHi = swap32(pDpllParam->dpll_config.lockIndicatorThresholdLHi);
    m7DpllParam.m7_dpll_config.NCObits = pDpllParam->dpll_config.nco_bits;
    m7DpllParam.m7_dpll_config.PhaseCounterRef = pDpllParam->dpll_config.phase_counter_ref;
    m7DpllParam.m7_dpll_config.PhaseErrorShift = pDpllParam->dpll_config.phase_error_shift;
    u64_H(m7DpllParam.m7_dpll_config.NominalLoopFilter) = swap64_h(pDpllParam->dpll_config.nominal_loop_filter);
    u64_L(m7DpllParam.m7_dpll_config.NominalLoopFilter) = swap64_l(pDpllParam->dpll_config.nominal_loop_filter);
    m7DpllParam.m7_dpll_config.InvertPhaseError = pDpllParam->dpll_config.invert_phase_error;

    m7DpllParam.m7_dpll_config.NormalizePhaseError = pDpllParam->dpll_config.norm_phase_error;
    m7DpllParam.m7_dpll_config.PhaseErrorThres0 = swap32(pDpllParam->dpll_config.phase_error_thres0);
    m7DpllParam.m7_dpll_config.PhaseErrorThres1 = swap32(pDpllParam->dpll_config.phase_error_thres1);
    m7DpllParam.m7_dpll_config.HoldoverFilterCoeff = swap16(pDpllParam->dpll_config.holdover_filter_coeff);

    /* DPLL State setting */
    m7DpllParam.m7_dpll_state.DPLLState = pDpllParam->dpll_state.dpll_state;
    u64_H(m7DpllParam.m7_dpll_state.LoopFilter) = swap64_h(pDpllParam->dpll_state.loop_filter);
    u64_L(m7DpllParam.m7_dpll_state.LoopFilter) = swap64_l(pDpllParam->dpll_state.loop_filter);

    u64_H(m7DpllParam.m7_dpll_state.Offset) = swap64_h(pDpllParam->dpll_state.offset);
    u64_L(m7DpllParam.m7_dpll_state.Offset) = swap64_l(pDpllParam->dpll_state.offset);

    u64_H(m7DpllParam.m7_dpll_state.PhaseCounter) = swap64_h(pDpllParam->dpll_state.phase_counter);
    u64_L(m7DpllParam.m7_dpll_state.PhaseCounter) = swap64_l(pDpllParam->dpll_state.phase_counter);

    u64_H(m7DpllParam.m7_dpll_state.PhaseCounterDelta) = swap64_h(pDpllParam->dpll_state.phaseCounterDelta);
    u64_L(m7DpllParam.m7_dpll_state.PhaseCounterDelta) = swap64_l(pDpllParam->dpll_state.phaseCounterDelta);

    u64_H(m7DpllParam.m7_dpll_state.AccumPhaseError) = swap64_h(pDpllParam->dpll_state.accumPhaseError);
    u64_L(m7DpllParam.m7_dpll_state.AccumPhaseError) = swap64_l(pDpllParam->dpll_state.accumPhaseError);

    m7DpllParam.m7_dpll_state.DwellCounter = swap32(pDpllParam->dpll_state.dwell_counter);
    m7DpllParam.m7_dpll_state.LockDetFilter = swap32(pDpllParam->dpll_state.lockDetFilter);
    m7DpllParam.m7_dpll_state.InitPhaseCounterFlag = pDpllParam->dpll_state.init_flag;
    m7DpllParam.m7_dpll_state.InitOffsetFlag = pDpllParam->dpll_state.init_offset_flag;
    m7DpllParam.m7_dpll_state.PhaseCounterN = swap16(pDpllParam->dpll_state.phaseCounterN);
    m7DpllParam.m7_dpll_state.PhaseCounterM = swap16(pDpllParam->dpll_state.phaseCounterM);
    m7DpllParam.m7_dpll_state.PhaseCounterFrac = swap16(pDpllParam->dpll_state.phaseCounterFrac);
    m7DpllParam.m7_dpll_state.IdumpCounter = swap16(pDpllParam->dpll_state.idumpCounter);

    m7DpllParam.m7_dpll_state.LockIndicator = 0;
    m7DpllParam.m7_dpll_state.LossOfLockIndicator = 0;
    m7DpllParam.m7_dpll_state.LargeErrorIndicator = 0;

    /* Exclude the debugMode location as it will be programmed in debugMode API */
    pData = (uint32 *)&m7DpllParam; pData++;
    dpllAddr += sizeof(uint32);

    for (count = 0; count < sizeof(m7DpllParam)-4;) {
        soc_iproc_m7_write(unit, dpllAddr, swap32(*pData));
        pData++;
        count += sizeof(uint32);
        dpllAddr += sizeof(uint32);
    }

    /* Update the channel for phase counter reference */
        bsl_printf("GDPLL: Set the phase counter ref\n");
        READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, PHASECOUNTERREFf,
            pDpllParam->dpll_config.phase_counter_ref ? 1:0);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_config_dpll_get(int unit, int chan,
                        dpll_param_t *pDpllParam)
{
    int rv = BCM_E_NONE;
    uint32 count;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 *pData;
    uint32 stateNum;

    pData = (uint32 *)&m7DpllParam;
    if ((chan >= BCM_GDPLL_NUM_CHANNELS) ||
        (pDpllParam == NULL)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_config_dpll_get: Error !! invalid param\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Read the DTCM locations for DPLL config */
    for (count = 0; count < sizeof(m7_dpll_param_t);) {
        *pData = swap32(soc_iproc_m7_read(unit, dpllAddr));
        pData++;
        count += sizeof(uint32);
        dpllAddr += sizeof(uint32);
    }

    pDpllParam->debugMode = m7DpllParam.debugMode;
    pDpllParam->dpll_config.dpll_num_states = m7DpllParam.m7_dpll_config.DPLLNumOfStates;

    for (stateNum = 0; stateNum < m7DpllParam.m7_dpll_config.DPLLNumOfStates; stateNum++) {
        pDpllParam->dpll_config.k1Shift[stateNum] = m7DpllParam.m7_dpll_config.K1Shift[stateNum];
        pDpllParam->dpll_config.k1k2Shift[stateNum] = m7DpllParam.m7_dpll_config.K1K2Shift[stateNum];

        pDpllParam->dpll_config.k1[stateNum] = swap32(m7DpllParam.m7_dpll_config.K1[stateNum]);
        pDpllParam->dpll_config.k1k2[stateNum] = swap32(m7DpllParam.m7_dpll_config.K1K2[stateNum]);
        pDpllParam->dpll_config.lockDetThres[stateNum] = swap32(m7DpllParam.m7_dpll_config.LockDetThres[stateNum]);
        pDpllParam->dpll_config.dwell_count[stateNum] = swap16(m7DpllParam.m7_dpll_config.DwellCount[stateNum]);
        u64_H(pDpllParam->dpll_config.phase_error_limiter_thres[stateNum]) =
            swap64_h(m7DpllParam.m7_dpll_config.PhaseErrorLimiterThres[stateNum]);
        u64_L(pDpllParam->dpll_config.phase_error_limiter_thres[stateNum]) =
            swap64_l(m7DpllParam.m7_dpll_config.PhaseErrorLimiterThres[stateNum]);
        pDpllParam->dpll_config.idump_mod[stateNum] = swap16(m7DpllParam.m7_dpll_config.IdumpMod[stateNum]);
    }

    pDpllParam->dpll_config.lockIndicatorThresholdLo = swap32(m7DpllParam.m7_dpll_config.LockIndicatorThresholdLo);
    pDpllParam->dpll_config.lockIndicatorThresholdLHi = swap32(m7DpllParam.m7_dpll_config.LockIndicatorThresholdHi);

    pDpllParam->dpll_config.nco_bits = m7DpllParam.m7_dpll_config.NCObits;
    pDpllParam->dpll_config.phase_counter_ref = m7DpllParam.m7_dpll_config.PhaseCounterRef;
    pDpllParam->dpll_config.phase_error_shift = m7DpllParam.m7_dpll_config.PhaseErrorShift;
    u64_H(pDpllParam->dpll_config.nominal_loop_filter) = swap64_h(m7DpllParam.m7_dpll_config.NominalLoopFilter);
    u64_L(pDpllParam->dpll_config.nominal_loop_filter) = swap64_l(m7DpllParam.m7_dpll_config.NominalLoopFilter);
    pDpllParam->dpll_config.invert_phase_error = m7DpllParam.m7_dpll_config.InvertPhaseError;

    pDpllParam->dpll_config.norm_phase_error = m7DpllParam.m7_dpll_config.NormalizePhaseError;
    pDpllParam->dpll_config.phase_error_thres0 = swap32(m7DpllParam.m7_dpll_config.PhaseErrorThres0);
    pDpllParam->dpll_config.phase_error_thres1 = swap32(m7DpllParam.m7_dpll_config.PhaseErrorThres1);
    pDpllParam->dpll_config.holdover_filter_coeff = swap16(m7DpllParam.m7_dpll_config.HoldoverFilterCoeff);

    /* DPLL State setting */
    pDpllParam->dpll_state.dpll_state = m7DpllParam.m7_dpll_state.DPLLState;

    pDpllParam->dpll_state.dwell_counter = swap32(m7DpllParam.m7_dpll_state.DwellCounter);
    pDpllParam->dpll_state.lockDetFilter = swap32(m7DpllParam.m7_dpll_state.LockDetFilter);
    pDpllParam->dpll_state.init_flag = m7DpllParam.m7_dpll_state.InitPhaseCounterFlag;
    pDpllParam->dpll_state.init_offset_flag = m7DpllParam.m7_dpll_state.InitOffsetFlag;
    u64_H(pDpllParam->dpll_state.loop_filter) = swap64_h(m7DpllParam.m7_dpll_state.LoopFilter);
    u64_L(pDpllParam->dpll_state.loop_filter) = swap64_l(m7DpllParam.m7_dpll_state.LoopFilter);

    u64_H(pDpllParam->dpll_state.offset) = swap64_h(m7DpllParam.m7_dpll_state.Offset);
    u64_L(pDpllParam->dpll_state.offset) = swap64_l(m7DpllParam.m7_dpll_state.Offset);

    u64_H(pDpllParam->dpll_state.phase_counter) = swap64_h(m7DpllParam.m7_dpll_state.PhaseCounter);
    u64_L(pDpllParam->dpll_state.phase_counter) = swap64_l(m7DpllParam.m7_dpll_state.PhaseCounter);

    u64_H(pDpllParam->dpll_state.phaseCounterDelta) = swap64_h(m7DpllParam.m7_dpll_state.PhaseCounterDelta);
    u64_L(pDpllParam->dpll_state.phaseCounterDelta) = swap64_l(m7DpllParam.m7_dpll_state.PhaseCounterDelta);

    u64_H(pDpllParam->dpll_state.accumPhaseError) = swap64_h(m7DpllParam.m7_dpll_state.AccumPhaseError);
    u64_L(pDpllParam->dpll_state.accumPhaseError) = swap64_l(m7DpllParam.m7_dpll_state.AccumPhaseError);
    pDpllParam->dpll_state.phaseCounterN = swap16(m7DpllParam.m7_dpll_state.PhaseCounterN);
    pDpllParam->dpll_state.phaseCounterM = swap16(m7DpllParam.m7_dpll_state.PhaseCounterM);
    pDpllParam->dpll_state.phaseCounterFrac = swap16(m7DpllParam.m7_dpll_state.PhaseCounterFrac);
    pDpllParam->dpll_state.idumpCounter = swap16(m7DpllParam.m7_dpll_state.IdumpCounter);

    pDpllParam->dpll_state.lockIndicator = m7DpllParam.m7_dpll_state.LockIndicator;
    pDpllParam->dpll_state.lossOfLockIndicator = m7DpllParam.m7_dpll_state.LossOfLockIndicator;
    pDpllParam->dpll_state.largeErrorIndicator = m7DpllParam.m7_dpll_state.LargeErrorIndicator;

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_debug_enable_set(int unit, int enable)
{
    /* This API may not be required as the debugging is
     * always enabled during the init API
     */
    int rv = BCM_E_NONE;

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_debug_mode_set(int unit, int chan,
            bcm_gdpll_debug_mode_t debugMode)
{
    int rv = BCM_E_NONE;
    uint32 dpll_debugMode;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

    /* Info - debugEnable debugMode addresses visible by M7 */
    dpll_debugMode = dpllAddr + 0/*DPLL_DEBUG_MODE*/;

    /* M7 ITCM configuration, for the desired debug mode */
    soc_iproc_m7_write(unit, dpll_debugMode, debugMode);

    return (rv);
}

int
_bcm_esw_gdpll_holdover_set(int unit, int chan, uint8 enable)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOff_holdover;
    uint32 addrOff_InitOffset;
    uint32 regVal;
    int fb_id=0;

    addrOff_holdover = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.HoldoverFlag);
    addrOff_InitOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.InitOffsetFlag);

    /* Set the holdover flag */
    regVal = soc_iproc_m7_read(unit, dpllAddr+addrOff_holdover);
    regVal = enable ? regVal | 0x1 : regVal & (~0x1);
    soc_iproc_m7_write(unit, dpllAddr+addrOff_holdover, regVal);

    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    if (pGdpllContext->dpll_chan[chan].phaseConterRef == 0){
        /* Set the phasecounter flag to enable the HW to trigger M7 on
         * feedback OR the fb/ref pair
         */
        READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, PHASECOUNTERREFf,
             enable ? 1:0);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal);
    }

    /* Set the initOffset flag if required*/
    if (enable == 0)
    {
        regVal = soc_iproc_m7_read(unit, dpllAddr+addrOff_InitOffset);
        regVal |= 0x10000;
        soc_iproc_m7_write(unit, dpllAddr+addrOff_InitOffset, regVal);
    } else {
        if (pGdpllContext->dpll_chan[chan].phaseConterRef == 1){

            READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
            fb_id = soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal,FEEDBACK_IDf);

            uint32_t reg_hi=0, reg_low=0;
#define NS_GDPLL_INPUT_ARRAY_TS_000_LSB (0x03271004)
#define NS_GDPLL_INPUT_ARRAY_TS_000_MSB (0x03271000)

            reg_hi = (NS_GDPLL_INPUT_ARRAY_TS_000_MSB + (fb_id*8));
            reg_low = (NS_GDPLL_INPUT_ARRAY_TS_000_LSB + (fb_id*8));

            soc_iproc_setreg(unit, reg_hi, 0xc0de);
            soc_iproc_setreg(unit, reg_low, 0xdeadbeaf);
        }
    }

    return rv;
}

int
_bcm_esw_gdpll_holdover_get(int unit, int chan, uint8 *enable)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOff_holdover;
    uint32 regVal;

    addrOff_holdover = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.HoldoverFlag);

    /* Set the holdover flag */
    regVal = soc_iproc_m7_read(unit, dpllAddr+addrOff_holdover);
    *enable = regVal & 0x1;

    return rv;
}

int
_bcm_esw_gdpll_pbo_set(int unit, int chan)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOff_InitOffset;
    uint32 regVal;

    addrOff_InitOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.InitOffsetFlag);

    /* Set the initOffset flag */
    regVal = soc_iproc_m7_read(unit, dpllAddr+addrOff_InitOffset);
    regVal |= 0x10000;
    soc_iproc_m7_write(unit, dpllAddr+addrOff_InitOffset, regVal);

    return rv;
}

int
bcm_esw_gdpll_offset_set(int unit, int chan,
            uint64 Offset)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOffset;
    dpll_param_t dpllParam;

    /* Check if the InitOffsetFlag is set */
    if(_bcm_esw_gdpll_chan_config_dpll_get(unit, chan,
                &dpllParam) != BCM_E_NONE) {
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    if (dpllParam.dpll_state.init_offset_flag) {
        bsl_printf("GDPLL: offset_set: Error !! InitOffsetFlag is set\n");
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);

    /* M7 DTCM configuration, for the Offset update */
    soc_iproc_m7_write(unit, dpllAddr+addrOffset, u64_L(Offset));
    soc_iproc_m7_write(unit, dpllAddr+addrOffset+4, u64_H(Offset));

exit:
    return (rv);
}

int
bcm_esw_gdpll_offset_get(int unit, int chan,
            uint64 *pOffset)
{
    int rv = BCM_E_NONE;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOffset;

    addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);

    /* M7 DTCM configuration, for the Offset update */
    u64_L(*pOffset) = soc_iproc_m7_read(unit, dpllAddr+addrOffset);
    u64_H(*pOffset) = soc_iproc_m7_read(unit, dpllAddr+addrOffset+4);

    return (rv);
}

/*STATIC*/ int
_bcm_esw_gdpll_chan_debug_mode_get(int unit, int chan,
            bcm_gdpll_debug_mode_t *pDebugMode)
{
    int rv = BCM_E_NONE;
    uint32 dpll_debugMode;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

    if (!pDebugMode || (chan >= BCM_GDPLL_NUM_CHANNELS)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Info - debugEnable debugMode addresses visible by M7 */
    dpll_debugMode = dpllAddr + 0/*DPLL_DEBUG_MODE*/;

    /* M7 ITCM configuration, for the desired debug mode */
    *pDebugMode = soc_iproc_m7_read(unit, dpll_debugMode);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_deinit(int unit)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    /* Destroy protection mutex. */
    if (NULL != pGdpllContext->mutex) {
        sal_mutex_destroy(pGdpllContext->mutex);
        pGdpllContext->mutex = NULL;
    }

    return (rv);
}

void
gdpll_cb_thread (void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    int chan;
    int flag = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (pGdpllContext == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                   "gdpll_cb_thread: Error !! context not setup\n")));
        return;
    }

    while(1) {
        /* Wait on the signal */
        (void)sal_sem_take(pGdpllContext->cb_sem, sal_sem_FOREVER);

        for (chan = 0; chan < BCM_GDPLL_NUM_CHANNELS; chan++) {
            /* Notify, if there is a "state change" OR "if Error flag is
             * set with different error code" */
            flag = 0;
            if (pGdpllContext->dpll_chan[chan].curr_state.state !=
                pGdpllContext->dpll_chan[chan].prev_state.state) {
                flag = 1;
            } else if ((pGdpllContext->dpll_chan[chan].curr_state.state & BCM_GDPLL_CHAN_STATE_ERROR) &&
                       (pGdpllContext->dpll_chan[chan].prev_state.error_code !=
                        pGdpllContext->dpll_chan[chan].curr_state.error_code)) {
                flag = 1;
            }

            if (flag) {
                /* Callback with chan status info */
                pGdpllContext->dpll_chan[chan].prev_state = pGdpllContext->dpll_chan[chan].curr_state;
                if (pGdpllContext->gdpll_cb[bcmGdpllCbChanState]) {
                    pGdpllContext->dpll_chan[chan].curr_state.chan = chan;
                    pGdpllContext->gdpll_cb[bcmGdpllCbChanState](unit,
                        pGdpllContext->pGdpll_cb_data[bcmGdpllCbChanState],
                        (uint32 *)&pGdpllContext->dpll_chan[chan].curr_state, bcmGdpllCbChanState);
                }
            }
        }
    }
}

void
gdpll_debug_buffer_send_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint32 regVal;
    uint32 dpllDebugFlag = BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET;

    if (pGdpllContext == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                  "gdpll_debug_buffer_send_thread: Error !! context not setup\n")));
        return;
    }

    while(1) {
     /* Wait on the signal from the ISR */
        (void)sal_sem_take(pGdpllContext->debug_sem, sal_sem_FOREVER);
        if (pGdpllContext->debug_cb && soc_iproc_m7_read (unit, dpllDebugFlag)) {
            /* Return of this cb confirms the buffer consumption */
            pGdpllContext->debug_cb(unit, pGdpllContext->pUserData,
                    (BCM_GDPLL_DEBUG_BUFFER_START + (pGdpllContext->debugRdIdx * 4)),
                    BCM_GDPLL_DEBUG_THRESHOLD);
        }

        READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_RSTATEr, 0, &regVal);
        pGdpllContext->debugRdIdx = regVal;
    }
}

STATIC int
_bcm_update_tod_ptpntp(int unit, int chan_num, bcm_gdpll_output_event_t event)
{
    uint32 nco_hi, nco_lo;

    if (chan_num >= BCM_GDPLL_NUM_CHANNELS || chan_num < 0) {
        return BCM_E_INTERNAL;
    }

    /* Read the OA for the correction value */
    READ_NS_REGr(unit, NS_GDPLL_OUTPUT_ARRAY_000_MSBr, chan_num, &nco_hi);
    READ_NS_REGr(unit, NS_GDPLL_OUTPUT_ARRAY_000_LSBr, chan_num, &nco_lo);

    if (event == bcmGdpllOutputEventNTPTOD) {
        WRITE_NTP_TIME_FREQ_CONTROLr(unit, nco_lo);
    } else if (event == bcmGdpllOutputEventPTPTOD) {
        WRITE_IEEE1588_TIME_FREQ_CONTROLr(unit, nco_lo);
    }

    return BCM_E_NONE;
}

STATIC void
gdpll_state_change_chan_capture(int unit, int chan)
{
    gdpll_context_t *pGdpllContext;
    dpll_param_t dpllParam;

    pGdpllContext = pGdpllCtx[unit];
    if (NULL == pGdpllContext) {
        return;
    }

    if (pGdpllContext->dpll_chan[chan].enable) {
        /* If chan is enabled, poll on channel status */

        /* Construct the current chan SW state (Lock, Large-Error) */
        if (_bcm_esw_gdpll_chan_config_dpll_get(unit, chan, &dpllParam) != BCM_E_NONE) {
            return;
        }

        pGdpllContext->dpll_chan[chan].curr_state.state  = BCM_GDPLL_CHAN_STATE_ENABLE;
        pGdpllContext->dpll_chan[chan].curr_state.state |=
            dpllParam.dpll_state.largeErrorIndicator ? BCM_GDPLL_CHAN_STATE_ERROR:0;
        pGdpllContext->dpll_chan[chan].curr_state.error_code =
            dpllParam.dpll_state.largeErrorIndicator ? bcmGdpllChanErrorDpllLError:bcmGdpllChanErrorNone;

        /* If the DPLL is not tracking large error, get the lock indication */
        if (!dpllParam.dpll_state.largeErrorIndicator) {
            pGdpllContext->dpll_chan[chan].curr_state.state |=
                dpllParam.dpll_state.lockIndicator ? BCM_GDPLL_CHAN_STATE_LOCK:0;
        }
    } else {
        pGdpllContext->dpll_chan[chan].curr_state.state  = 0;
        pGdpllContext->dpll_chan[chan].curr_state.error_code = 0;
    }

}

STATIC void
gdpll_state_change_capture(int unit)
{
    int chan;
    gdpll_context_t *pGdpllContext;

    pGdpllContext = pGdpllCtx[unit];
    if (NULL == pGdpllContext) {
        return;
    }

    /* Retrieve state of all the enabled channels */
    for (chan = 0; chan < BCM_GDPLL_NUM_CHANNELS; chan++) {
        gdpll_state_change_chan_capture(unit, chan);
    }
}

/* Deferred call to get the RSVD1 message to user over callback */
void gdpll_intr_dpc(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    uint32 *pBuff = (uint32 *)d1;

    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (pGdpllContext && (pGdpllContext->gdpll_cb[bcmGdpllCbRsvd1])) {
        /* Return of this cb confirms the data consumption */
        pGdpllContext->gdpll_cb[bcmGdpllCbRsvd1](unit, pGdpllContext->pGdpll_cb_data[bcmGdpllCbRsvd1],
                    pBuff, bcmGdpllCbRsvd1);
    }
}

/* Nanosync generic interrupt */
void
gdpll_intr(int unit, uint32 int_status, uint32 int_enable)
{
    uint32 int_status_clear = 0;  /* Interrupt status clear */
    uint32 stat, stat0, stat1, stat2, stat3;  /* Interrupt status */
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    int chan_num, chan_idx;
    uint32 regVal = 0;
    int post_cb_sem = 0;
    uint32 dpllAddr, addrOffset;
    uint32 intEnable = 0;
    uint32 count = 0;

#if 1   
    /* TS0_CNT_OFFSET_UPDATED interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, TS0_CNT_OFFSET_UPDATEDf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "soc_nanosync_intr: TS0_CNT_OFFSET_UPDATEDf \n")));
        /* Set W1TC to clear the flag */
        soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS0_CNT_OFFSET_UPDATEDf, 1);
    }

    /* TS1_CNT_OFFSET_UPDATED interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, TS1_CNT_OFFSET_UPDATEDf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "soc_nanosync_intr: TS1_CNT_OFFSET_UPDATEDf \n")));
        /* Set W1TC to clear the flag */
        soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, TS1_CNT_OFFSET_UPDATEDf, 1);
    }
#endif

    /* M7_CORE_INT_STATUS interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, M7_CORE_INT_STATUSf)) {

        /* Check the status in NS_GDPLL_M7_CORE_INT_STATUS for LOCKUP,
         * SYSRESET_REQ, TXEV, ERRORf interrupts
         */
        READ_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_STATUSr, 0, &regVal);
        READ_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_ENABLEr, 0, &intEnable);
        intEnable &= regVal;

        LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
            "soc_nanosync_intr: M7_CORE_INT_STATUS-0x%x\n"), regVal));

        /* Process ERROR interrupt */
        if (soc_reg_field_get(unit, NS_GDPLL_M7_CORE_INT_STATUSr, intEnable, ERRORf)) {
            /* Read ERROR_INFO and ack the interrupt */
            READ_NS_REGr(unit, NS_GDPLL_M7_ERROR_INFOr, 0, &regVal);
            WRITE_NS_REGr(unit, NS_GDPLL_M7_ERROR_INFO_INTr, 0, 0);

            LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                "soc_nanosync_intr: ERROR INFO:0x%x\n"), regVal));

            /* Process M7 interrupt for state change */
            if (regVal & (INTR_STATE_CHANGE << 8)) {
                /* This is a state change SW interrupt on one of the GDPLL channels */
                gdpll_state_change_capture(unit);
                post_cb_sem = 1;    /* Notify the State change */
            }
        }

        /* Process LOCKUP interrupt */
        if (soc_reg_field_get(unit, NS_GDPLL_M7_CORE_INT_STATUSr, intEnable, LOCKUPf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: LOCKUP\n")));
            WRITE_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_STATUSr, 0, 0x1);
        }

        /* Process SYSRESET_REQ interrupt */
        if (soc_reg_field_get(unit, NS_GDPLL_M7_CORE_INT_STATUSr, intEnable, SYSRESET_REQf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: SYSRESET_REQ\n")));
            WRITE_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_STATUSr, 0, 0x2);
        }

        /* Process TXEV interrupt */
        if (soc_reg_field_get(unit, NS_GDPLL_M7_CORE_INT_STATUSr, intEnable, TXEVf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: TXEV\n")));
            WRITE_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_STATUSr, 0, 0x4);
        }

        /* Set W1TC to clear the flag */
        soc_reg_field_set(unit, NS_TS_INT_STATUSr, &int_status_clear, M7_CORE_INT_STATUSf, 1);
    }

    /* Write W1TC to clear the flags in NS_TS_INT_STATUS */
    WRITE_NS_REGr(unit, NS_TS_INT_STATUSr, 0, int_status_clear);

    /*
     * In HW error intr, we clear the M7-SW state change indications if any,
     * captured in M7_CORE_INT_STATUS. Since HW error gets priority, clear the
     * SW indications in GDPLL_IA_0/1/2/3_STATUS processing
     */
    /* GDPLL_IA_0/1/2/3_STATUS interrupt */
    count = 0;
    while (count < 4) {
        if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, gdpll_ia_status[count])) {
            READ_NS_REGr(unit, ia_in_status_regs[count], 0, &stat);

            chan_num = count*32; chan_idx = 0;
            while (chan_idx < 32) {
                if ( pGdpllContext->dpll_chan[chan_num+chan_idx].enable &&
                     ((stat >> chan_idx) & 0x1)) {
                    /* Capture error code, clear SW indicators, post sem for notification */
                    READ_NS_REGr(unit, NS_GDPLL_IA_ERROR_CODEr, chan_num+chan_idx, &regVal);
                        pGdpllContext->dpll_chan[chan_num+chan_idx].curr_state.state =
                            BCM_GDPLL_CHAN_STATE_ENABLE | BCM_GDPLL_CHAN_STATE_ERROR;
                    pGdpllContext->dpll_chan[chan_num+chan_idx].curr_state.error_code = regVal & 0x3;

                    dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan_num+chan_idx);
                    addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.LockIndicator);
                    soc_iproc_m7_write(unit, dpllAddr+addrOffset, 0);
                    post_cb_sem = 1;

                    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                           " Error: chan-%d error_code-0x%x\n"), chan_num+chan_idx, regVal));
                }
                chan_idx++;
            }

            WRITE_NS_REGr(unit, ia_in_status_regs[count], 0, stat);
        }
        count++;
    }

    /* Post callback sema for state change notification */
    if (post_cb_sem) {
        sal_sem_give(pGdpllContext->cb_sem);
    }

    /* RSVD1_MESSAGE_RX_STATUS interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, BRCM_RESERVED_IPROC_3_1f/*RSVD1_MESSAGE_RX_STATUSf*/)) {
        if (pGdpllContext) {
            if (pGdpllContext->gdpll_cb[bcmGdpllCbRsvd1]) {
                /* Read the RSVD1 status registers
                 * MESSAGE_CAPTURE_0_STATUS - BRCM_RESERVED_IPROC_6
                 * MESSAGE_CAPTURE_1_STATUS - BRCM_RESERVED_IPROC_7
                 * MESSAGE_CAPTURE_2_STATUS - BRCM_RESERVED_IPROC_8
                 */
                 READ_NS_REGr(unit, BRCM_RESERVED_IPROC_6r, 0, &cbData[0]);
                 READ_NS_REGr(unit, BRCM_RESERVED_IPROC_7r, 0, &cbData[1]);
                 READ_NS_REGr(unit, BRCM_RESERVED_IPROC_8r, 0, &cbData[2]);
#if 0
                /* Return of this cb confirms the data consumption */
                pGdpllContext->gdpll_cb[bcmGdpllCbRsvd1](unit, pGdpllContext->pGdpll_cb_data[bcmGdpllCbRsvd1],
                    cbData, bcmGdpllCbRsvd1);
#else
                sal_dpc(gdpll_intr_dpc,
                        INT_TO_PTR(unit), cbData, NULL, NULL, NULL);
#endif
            }
        }

        /* Clear the leaf level status at BRCM_RESERVED_IPROC_9 */
        WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_9r, 0, 0x3);
    }

#if 0   /* This was enabled to test the NTP/PTP TOD corrections from host */
    /* GDPLL_OA_x_STATUS interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_OA_0_STATUSf) ||
        soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_OA_1_STATUSf) ||
        soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_OA_2_STATUSf) ||
        soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_OA_3_STATUSf)) {

        READ_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS0r, 0, &stat0);
        READ_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS1r, 0, &stat1);
        READ_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS2r, 0, &stat2);
        READ_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS3r, 0, &stat3);

        
        if (1 == soc_property_get(unit,"tod_ptpntp_host_update", -1)) {

            /* Check if this NCO update is for NTPTOD */
            chan_num = soc_iproc_m7_read(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC);
            if (chan_num < 32 && (stat0 & (1<<chan_num))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventNTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS0r, 0, (1<<chan_num));
            } else if (chan_num < 64 && (stat1 & (1<<(chan_num-32)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventNTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS1r, 0, (1<<(chan_num-32)));
            } else if (chan_num < 96 && (stat2 & (1<<(chan_num-64)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventNTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS2r, 0, (1<<(chan_num-64)));
            } else if (chan_num < 128 && (stat3 & (1<<(chan_num-96)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventNTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS3r, 0, (1<<(chan_num-96)));
            }
        }

        if (1 == soc_property_get(unit,"tod_ptpntp_host_update", -1)) {

            /* Check if this NCO update is for PTPTOD */
            chan_num = soc_iproc_m7_read(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC);
            if (chan_num < 32 && (stat0 & (1<<chan_num))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventPTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS0r, 0, (1<<chan_num));
            } else if (chan_num < 64 && (stat1 & (1<<(chan_num-32)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventPTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS1r, 0, (1<<(chan_num-32)));
            } else if (chan_num < 96 && (stat2 & (1<<(chan_num-64)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventPTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS2r, 0, (1<<(chan_num-64)));
            } else if (chan_num < 128 && (stat3 & (1<<(chan_num-96)))) {
                _bcm_update_tod_ptpntp(unit, chan_num, bcmGdpllOutputEventPTPTOD);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS3r, 0, (1<<(chan_num-96)));
            }
        }
    }
#endif

    /* GDPLL_NCO_OVERLOAD_x_STATUS interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_NCO_OVERLOAD_0_STATUSf) ||
        soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_NCO_OVERLOAD_1_STATUSf) ||
        soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_NCO_OVERLOAD_2_STATUSf) ||
        soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_NCO_OVERLOAD_3_STATUSf)) {

        READ_NS_REGr(unit, NS_GDPLL_NCO_OVERLOAD_STATUS0r, 0, &stat0);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OVERLOAD_STATUS0r, 0, stat0);

        READ_NS_REGr(unit, NS_GDPLL_NCO_OVERLOAD_STATUS1r, 0, &stat1);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OVERLOAD_STATUS1r, 0, stat1);

        READ_NS_REGr(unit, NS_GDPLL_NCO_OVERLOAD_STATUS2r, 0, &stat2);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OVERLOAD_STATUS2r, 0, stat2);

        READ_NS_REGr(unit, NS_GDPLL_NCO_OVERLOAD_STATUS3r, 0, &stat3);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OVERLOAD_STATUS3r, 0, stat3);

        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NCO_OVERLOAD(0:31): 0x%x\n"), stat0));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NCO_OVERLOAD(32:63): 0x%x\n"), stat1));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NCO_OVERLOAD(64:95): 0x%x\n"), stat2));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NCO_OVERLOAD(96:127): 0x%x\n"), stat3));
    }

    /* GDPLL_COMMON_STATUS interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_COMMON_STATUSf)) {
        READ_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_INT_STATUSr, 0, &stat0);
        WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_INT_STATUSr, 0, stat0);

        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: COMMON_INT_STATUS(0:3): 0x%x\n"), stat0));
    }

    /* TS_CPU_FIFO_ECC_ERR_STATUS interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, TS_CPU_FIFO_ECC_ERR_STATUSf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: TS_CPU_FIFO_ECC_ERR_STATUSf\n")));
    }

    /* GDPLL_MEM_ECC_ERR_STATUS interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, GDPLL_MEM_ECC_ERR_STATUSf)) {
        READ_NS_REGr(unit, NS_GDPLL_NCO_MEM_ECC_STATUSr, 0, &stat0);
        READ_NS_REGr(unit, NS_GDPLL_WTCHDG_MEM_ECC_STATUSr, 0, &stat1);
        READ_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_STATUSr, 0, &stat2);

        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NCO_MEM_ECC_STATUS: 0x%x\n"), stat0));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_GDPLL_WTCHDG_MEM_ECC_STATUS: 0x%x\n"), stat1));
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_GDPLL_IA_MEM_ECC_STATUS: 0x%x\n"), stat2));
    }

    /* M7_TCM_ECC_ERR_STATUS interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, M7_TCM_ECC_ERR_STATUSf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: M7_TCM_ECC_ERR_STATUS\n")));
    }

    /* RoE_TS_ERR interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, ROE_TS_ERRf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                  " Error: RoE_TS_ERRf \n")));
    }

    /* TS_CAPTURE_OVRD interrupt */
    if (soc_reg_field_get(unit, NS_TS_INT_STATUSr, int_status, TS_CAPTURE_OVRDf)) {
        READ_NS_REGr(unit, NS_RX_CDR_EVENT_OVRD_STATUS0r, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_RX_CDR_EVENT_OVRD_STATUS0:0x%x\n"), stat0));

        READ_NS_REGr(unit, NS_RX_CDR_EVENT_OVRD_STATUS1r, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_RX_CDR_EVENT_OVRD_STATUS10:0x%x\n"), stat0));

        READ_NS_REGr(unit, NS_TX_PI_EVENT_OVRD_STATUS0r, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_TX_PI_EVENT_OVRD_STATUS0:0x%x\n"), stat0));

        READ_NS_REGr(unit, NS_TX_PI_EVENT_OVRD_STATUS1r, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_TX_PI_EVENT_OVRD_STATUS1:0x%x\n"), stat0));

        READ_NS_REGr(unit, NS_MISC_EVENT_OVRD_STATUSr, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_MISC_EVENT_OVRD_STATUS:0x%x\n"), stat0));

        READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_RX_TYPE0_OVRD_STATUSr, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_TIMESYNC_MAPPER_RX_TYPE0_OVRD_STATUS:0x%x\n"), stat0));

        READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_RX_TYPE1_OVRD_STATUSr, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_TIMESYNC_MAPPER_RX_TYPE1_OVRD_STATUS:0x%x\n"), stat0));

        READ_NS_REGr(unit, NS_TIMESYNC_MAPPER_RX_TYPE2_OVRD_STATUSr, 0, &stat0);
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "soc_nanosync_intr: NS_TIMESYNC_MAPPER_RX_TYPE2_OVRD_STATUS:0x%x\n"), stat0));
    }

#if 0 /* Interrupt is re-enabled in time.c */
    /* Re-enable GDPLL NS IRQ */
    soc_cmicm_intr2_enable(unit, SOC_GDPLL_NS_INTR_POS);
#endif
}


/* Nanosync debug buffer interrupt */
void
soc_nanosync_debug_intr(void *unit_vp, void *d1, void *d2,
                           void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    uint32 regVal = 0;
    uint32 int_status = 0;  /* Interrupt status */
    uint32 int_enable = 0;  /* Interrupt enable */
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    /* Read the GDPLL interrupt status */
    READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, 0, &int_status);

    /* Process threshold interrupt */
    if (soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, int_status, THRESHOLD_REACHEDf)) {
        READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_RSTATEr, 0, &regVal);
        regVal = (pGdpllContext->debugRdIdx + BCM_GDPLL_DEBUG_THRESHOLD) % BCM_GDPLL_DEBUG_BUFFER_SIZE;
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_RSTATEr, 0, regVal);

        int_enable |= INTR_GDPLL_DEBUG_THR;
        /* Signal the thread for buffer read */
        if (pGdpllContext->debug_sem) {
            sal_sem_give(pGdpllContext->debug_sem);
        } else {
            uint32 dpllDebugFlag = BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET;

            if (pGdpllContext->debug_cb && soc_iproc_m7_read (unit, dpllDebugFlag)) {
                /* Return of this cb confirms the buffer consumption */
                pGdpllContext->debug_cb(unit, pGdpllContext->pUserData,
                    (BCM_GDPLL_DEBUG_BUFFER_START + (pGdpllContext->debugRdIdx * 4)),
                    BCM_GDPLL_DEBUG_THRESHOLD);
            }

            READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_RSTATEr, 0, &regVal);
            pGdpllContext->debugRdIdx = regVal;
        }
    }

    if (soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, int_status, OVERFLOWf)) {
        int_enable |= INTR_GDPLL_DEBUG_OF;
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "OVERFLOW !!\n")));
        soc_iproc_m7_write(unit, (BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET), 0);
    }

    if (soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, int_status, ECC_ERR_MEM0f)) {
        int_enable |= INTR_GDPLL_DEBUG_ECC0;
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ECC_ERR_MEM0 !!\n")));
    }

    if (soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, int_status, ECC_ERR_MEM1f)) {
        int_enable |= INTR_GDPLL_DEBUG_ECC1;
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "ECC_ERR_MEM1 !!\n")));
    }

    /* Clear the interrupt status, which is W1TC */
    WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_STATUSr, 0, int_status);

    /* Re-enable the interrupts */
    READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, 0, &regVal);
    int_enable |= soc_reg_field_get(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, regVal, INTENf);
    soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, &regVal, INTENf, int_enable);
    WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, 0, int_enable);

    /* Re-enable GDPLL debug buffer interrupt */
    soc_cmicm_intr2_enable(unit, SOC_GDPLL_NS_INTR_POS | SOC_GDPLL_NS_INTR_DEBUG_POS);
}

void clear_m7_memory(int unit)
{
    uint32 m7_itcmaddr = BCM_M7_ITCM_BASE;
    uint32 m7_dtcmaddr = BCM_M7_DTCM_BASE;
    uint32 i;
    uint32 itcm_size = 1024*8;
    uint32 dtcm_size = 1024*32;
    uint32 rval;

    /* Pull M7 out of reset for TCM initialization */
    rval = 0;
    READ_M7SS_CTRLr(unit, &rval);
    soc_reg_field_set(unit, M7SS_CTRLr, &rval, SYS_RESETf, 0x0);
    soc_reg_field_set(unit, M7SS_CTRLr, &rval, POWERUP_RESETf, 0x0);
    WRITE_M7SS_CTRLr(unit, rval);

    /* Clear ITCM */
    for (i=0; i<itcm_size;) {
        soc_uc_mem_write(unit, m7_itcmaddr, 0);
        i = i+4;
        m7_itcmaddr = m7_itcmaddr+4;
    }

    /* Clear DTCM */
    for (i=0; i<dtcm_size;) {
        soc_uc_mem_write(unit, m7_dtcmaddr, 0);
        i = i+4;
        m7_dtcmaddr = m7_dtcmaddr+4;
    }
}


int
_bcm_esw_gdpll_init(int unit)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 alloc_sz = 0;
    uint32 count = 0;
    gdpll_context_t *pGdpllContext = NULL;
    sal_thread_t thread_id;
    uint32 reg;
    uint32 regCount;
    int debug_thread_pri = 0;

    /* Clear the tdpll state structure */
    for (count = 0; count < NUM_TDPLL_INSTANCE; count++) {
        gdpll_tdpll_state.tdpll_instace[count] = BCM_GDPLL_NUM_IA_ENTRIES;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    _bcm_gdpll_scache_alloc(unit);
#endif

    /* Allocate time config structure. */
    alloc_sz = sizeof(gdpll_context_t);
    pGdpllContext = sal_alloc(alloc_sz, "GDPLL module");
    if (NULL == pGdpllContext) {
        rv = BCM_E_MEMORY;
        goto exit;
    }

    if (SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_bcm_esw_gdpll_reinit(unit, pGdpllContext));
    } else {
        sal_memset(pGdpllContext, 0, alloc_sz);

        /* Set all events available */
        for (count = 0; count < BCM_GDPLL_NUM_INPUT_EVENTS; count++) {
            pGdpllContext->eventId[count] = BCM_GDPLL_NUM_IA_ENTRIES;
        }
    }   /* SOC_WARM_BOOT */

    /* Create protection mutex. */
    pGdpllContext->mutex = NULL;
    pGdpllContext->mutex = sal_mutex_create("GDPLL mutex");
    if (NULL == pGdpllContext->mutex) {
        _bcm_esw_gdpll_deinit(unit);
        return (BCM_E_MEMORY);
    }

    pGdpllContext->debug_cb = NULL;
    if (!SOC_WARM_BOOT(unit)) {
        clear_m7_memory(unit);

        

        /* Configure debug threshold level */
        READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_THRESHOLDr, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_THRESHOLDr, &regVal, THRESHOLDf, BCM_GDPLL_DEBUG_THRESHOLD);
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_THRESHOLDr, 0, regVal);

        /* Enable the GDPLL debug feature */
        /* 1->0 transition to clear the buffer pointers */
        READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, &regVal, ENABLEf, 1);
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, regVal);
        soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, &regVal, ENABLEf, 0);
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, regVal);
        soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, &regVal, ENABLEf, 1);
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, regVal);
    }   /* SOC_WARM_BOOT */

    pGdpllCtx[unit] = pGdpllContext;

    pGdpllContext->debugRdIdx = 0;

    debug_thread_pri = soc_property_get(unit,"gdpll_debug_thread_pri", -1);
    pGdpllContext->debug_sem = NULL;
    if ((debug_thread_pri >= 0) &&  (debug_thread_pri < 255)) {
        /* Create the debug buffer semaphore */
        pGdpllContext->debug_sem = sal_sem_create("gdpll_debug_buff", sal_sem_COUNTING, 0);
        if (pGdpllContext->debug_sem == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                      "_bcm_esw_gdpll_init: Error !! failed to create the sem\n")));
              return (BCM_E_MEMORY);
        }

        /* Create the debug thread */
        thread_id = sal_thread_create("GDPLL Debug thread", SAL_THREAD_STKSZ, debug_thread_pri,
                        gdpll_debug_buffer_send_thread, INT_TO_PTR(unit));
        if (thread_id == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "GDPLL Error !! debug thread create failed\n")));
            return BCM_E_INTERNAL;
        }
    }

    /* Create the GDPLL CB thread */
    pGdpllContext->cb_sem = sal_sem_create("gdpll_cb", sal_sem_COUNTING, 0);
    if (pGdpllContext->cb_sem == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_bcm_esw_gdpll_init: Error !! failed to create the cb_sem\n")));
          return (BCM_E_MEMORY);
    }

    /* Create the debug thread */
    thread_id = sal_thread_create("GDPLL CB thread", SAL_THREAD_STKSZ, 100/*thread_pri*/,
                    gdpll_cb_thread, INT_TO_PTR(unit));
    if (thread_id == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "GDPLL Error !! CB thread create failed\n")));
        return BCM_E_INTERNAL;
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Disable the ECC protection before Input-Array clearing */
        READ_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, &regVal,
                          DISABLE_ECCf, 1);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, 0, regVal);

        /* Clear the Input Array */
        reg = BCM_GDPLL_IA_START;
        for (regCount = 0; regCount < (BCM_GDPLL_NUM_IA_ENTRIES*2); regCount++) {
            soc_iproc_setreg(unit, reg+regCount*4, 0);
        }

        /* Clear the Output Array */
        reg = BCM_GDPLL_OA_START;
        for (regCount = 0; regCount < (BCM_GDPLL_NUM_OA_ENTRIES*2); regCount++) {
            soc_iproc_setreg(unit, reg+regCount*4, 0);
        }

        /* Enable the ECC protection on Input-Array */
        READ_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, &regVal,
                          DISABLE_ECCf, 0);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_MEM_ECC_CONTROLr, 0, regVal);

        /* Clear the out status flags */
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS0r, 0, 0xFFFFFFFF); /* 0:31  */
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS1r, 0, 0xFFFFFFFF);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS2r, 0, 0xFFFFFFFF);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_STATUS3r, 0, 0xFFFFFFFF);

        /* Enable the required leaf level interrupts that reflects
         * in TS generic interrupts
         */
        /* Enable the channel update status reporting */
        WRITE_NS_REGr(unit, NS_GDPLL_IA_IN_ENABLE0r, 0, 0xFFFFFFFF); /* 0:31  */
        WRITE_NS_REGr(unit, NS_GDPLL_IA_IN_ENABLE1r, 0, 0xFFFFFFFF);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_IN_ENABLE2r, 0, 0xFFFFFFFF);
        WRITE_NS_REGr(unit, NS_GDPLL_IA_IN_ENABLE3r, 0, 0xFFFFFFFF);

#if 0
        /* Enable the Channel Output array status reporting */
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_ENABLE0r, 0, 0xFFFFFFFF); /* 0:31  */
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_ENABLE1r, 0, 0xFFFFFFFF);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_ENABLE2r, 0, 0xFFFFFFFF);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_OUT_ENABLE3r, 0, 0xFFFFFFFF);
#endif

        /* Enable NCO overload detection is global to all the channeld ?? */
        READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_COMMON_CONTROLr, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_COMMON_CONTROLr, &regVal, NCO_WERROR_DISABLEf, 1);
        WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_COMMON_CONTROLr, 0, regVal);

        /* ECC error detection is set by default, ECC_CONTROL register */

        /* Enable GDPLL common interrupt status reporting */
        WRITE_NS_REGr(unit, NS_GDPLL_GDPLL_COMMON_INT_ENABLEr, 0, 0xF);

        /* Enable the M7 core-interrupt SW status reporting */
        WRITE_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_ENABLEr, 0, 0x8/*0xF*/);

        /* There are no enabling control for TS_CAPTURE_OVRD */

        /* Set the watchdog on every channel
         * The watchdog status will get reflected in GDPLL_IA_IN_STATUS* and
         * corresponding error code in NS_GDPLL_IA_ERROR_CODE
         */
        for (count = 0; count < BCM_GDPLL_NUM_CHANNELS; count++) {
            /* Set enable and Interval field NS_GDPLL_IA_WDOG_CONFIG */
            READ_NS_REGr(unit, NS_GDPLL_IA_WDOG_CONFIGr, count, &regVal);
            soc_reg_field_set(unit, NS_GDPLL_IA_WDOG_CONFIGr, &regVal,
                              INTERVALf, 0xFFFFF);  
            soc_reg_field_set(unit, NS_GDPLL_IA_WDOG_CONFIGr, &regVal,
                              ENABLEf, 1);
            WRITE_NS_REGr(unit, NS_GDPLL_IA_WDOG_CONFIGr, count, regVal);
        }

        if (soc_feature(unit, soc_feature_rsvd1_intf)) {
            /* Pull RSVD1 PLL out of reset */
            SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &regVal));
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &regVal, BRCM_RESERVED_TOP_1_1f, 1);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &regVal, BRCM_RESERVED_TOP_1_2f, 1);
            SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, regVal));

            /* Init the BCN counter */
            READ_NS_REGr(unit, BRCM_RESERVED_IPROC_12r, 0, &regVal);
            soc_reg_field_set(unit, BRCM_RESERVED_IPROC_12r, &regVal, BRCM_RESERVED_IPROC_12_1f, 0x400);
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_12r, 0, regVal);

            READ_NS_REGr(unit, BRCM_RESERVED_IPROC_13r, 0, &regVal);
            soc_reg_field_set(unit, BRCM_RESERVED_IPROC_13r, &regVal, BRCM_RESERVED_IPROC_13_1f, 0);
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_13r, 0, regVal);

            READ_NS_REGr(unit, BRCM_RESERVED_IPROC_11r, 0, &regVal);
            soc_reg_field_set(unit, BRCM_RESERVED_IPROC_11r, &regVal, BRCM_RESERVED_IPROC_11_1f, 0x1);
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_11r, 0, regVal);
        }

        /* Set capture_enable for all the TS GPIO's */
        for (count = 0; count < TS_GPIO_COUNT; count++) {
            READ_NS_REGr(unit, gpio_regs[count], 0, &regVal);
            soc_reg_field_set(unit,  gpio_regs[count], &regVal,
                              CAPTURE_ENABLEf, 1);
            WRITE_NS_REGr(unit, gpio_regs[count], 0, regVal);
            READ_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, (131+count), &regVal);
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal, EVENT_IDf, (195+count));
            soc_reg_field_set(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, &regVal, DESTf, 0x1);
            WRITE_NS_REGr(unit, NS_TIMESYNC_TS_EVENT_FWD_CFGr, (131+count), regVal);
        }

        if (!SOC_WARM_BOOT(unit)) {
            /* Set broadsync setting to reset state */
            SOC_IF_ERROR_RETURN(WRITE_NS_BS0_BS_TC_CTRLr(unit, 0));
            SOC_IF_ERROR_RETURN(WRITE_NS_BS1_BS_TC_CTRLr(unit, 0));
        }

        /* Check if the Offset is non-word aligned */
        reg = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);
        if (reg%4) {
            bsl_printf("GDPLL: WARN !! Offset field at offset:%d\n", reg);
        }

        /* Check if HoldoverFlag and NominalLoopFiletr are non-word aligned */
        reg = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.HoldoverFlag);
        if (reg%4) {
            bsl_printf("GDPLL: WARN !! HoldoverFlag field at offset:%d\n", reg);
        }

        reg = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_config.NominalLoopFilter);
        if (reg%4) {
            bsl_printf("GDPLL: WARN !! NominalLoopFilter field at offset:%d\n", reg);
        }

        /* Check the size of DPLL param */
        count = sizeof(m7DpllParam);
        if (count > DPLL_PARAM_SIZE) {
            bsl_printf("GDPLL: ERROR !! DPLL param size:%d is more than allocated:%d\n",
                       count, DPLL_PARAM_SIZE);
            return BCM_E_NONE;
        }

        /* Invalidate the NCO out configuration registers */
        if (SOC_IS_MONTEREY(unit)) {
            /* Monterey uArch spec defines hw_type:0 and hw_id:70 as invalid */
            for (count = 0; count < BCM_GDPLL_NUM_CHANNELS; count++) {
                READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, count, &regVal);
                soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_IDf, 70);
                soc_reg_field_set(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, &regVal, DEST_HW_TYPEf, 0);
                WRITE_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, count, regVal);
            }
        }

        /* Mark the locations as free untill 1588 channel is not configured */
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_1588_FREQ_LOCK_LOC, 0);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_1588_DPLL_OFFSET_LOC, 0);

        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_FREQ_LOCK_LOC, 0);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_DPLL_OFFSET_LOC, 0);

        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_FREQ_LOCK_LOC, 0);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_DPLL_OFFSET_LOC, 0);

        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_DPLL_OFFSET_LOC, 0);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_DPLL_OFFSET_LOC, 0);

        /* Comand M7 not to generate dump */
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_CHAN_DUMP_LOC +
                           SAL_OFFSETOF(dpll_dump_t, chan), 0xDEAD);

        /* Mask the GDPLL Debug IRQ interrupts and NS_TS IRQ interrupts */
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_INT_ENABLEr, 0, INTR_NS_DEBUG);

#ifdef BCN_WAR
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BCN_UPDATE_LOC, 0xFF);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC, 0xFF);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC, 0xFF);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC, 0xFF);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1_UPDATE_LOC, 0xFF);
#endif
    }   /* !SOC_WARM_BOOT */

    /* Enable both(GDPLL debug buffer and Error) interrupts */
    soc_cmicm_intr2_enable(unit, SOC_GDPLL_NS_INTR_POS | SOC_GDPLL_NS_INTR_DEBUG_POS);

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_alloc(int unit, int *pChan)
{
    int rv = BCM_E_NONE;
    uint32 idx = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if (pGdpllContext == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                  "_bcm_esw_gdpll_chan_alloc: Error !! No context\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    for (idx = 0; idx < BCM_GDPLL_NUM_CHANNELS; idx++) {
        if (!(pGdpllContext->dpll_chan[idx].flag & BCM_GDPLL_CHAN_ALLOC)) {
            *pChan = idx;
            goto exit;
        }
    }

    rv = BCM_E_FULL;

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_free(int unit, int chan)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if ((pGdpllContext == NULL) ||
        (chan >= BCM_GDPLL_NUM_CHANNELS)){
        rv = BCM_E_PARAM;
        goto exit;
    }

    pGdpllContext->dpll_chan[chan].flag &= ~BCM_GDPLL_CHAN_ALLOC;

exit:
    return (rv);
}

STATIC int
_bcm_esw_gdpll_event_config_set(int unit, bcm_gdpll_chan_input_t *pChanInput, uint32 eventId, uint32 *pIaAddr)
{
    int rv = BCM_E_NONE;
    gdpll_event_dest_cfg_t eventDest;

    /* Configure the event */
    eventDest.event_dest = pChanInput->event_dest;
    eventDest.ts_counter = pChanInput->ts_counter;
    eventDest.ppm_check_enable = pChanInput->ppm_check_enable;

    if ((pChanInput->input_event >= bcmGdpllInputEventCpu) &&
        (pChanInput->input_event <= bcmGdpllInputEventLCPLL3)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_divisor_misc_set(unit,
                            pChanInput->input_event,
                            pChanInput->event_divisor));
        BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_dest_misc_set(unit,
                            pChanInput->input_event,
                            &eventDest, eventId, pIaAddr));

    } else if(pChanInput->input_event == bcmGdpllInputEventPORT) {
        if ((pChanInput->port_event_type == bcmGdpllPortEventRXCDR) ||
            (pChanInput->port_event_type == bcmGdpllPortEventTXPI)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_divisor_port_set(unit,
                            pChanInput->port, pChanInput->port_event_type,
                            pChanInput->event_divisor));
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_dest_port_set(unit, pChanInput->port,
                            pChanInput->port_event_type,
                            &eventDest, eventId, pIaAddr));

        /* ROE event, disable the 52b conversion SWIPROC-591 */
        if (pChanInput->port_event_type == bcmGdpllPortEventROE)
            BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_event_roe_52b_set(unit,
                            pChanInput->port, 0));

    } else if(pChanInput->input_event == bcmGdpllInputEventR5) {
        
         *pIaAddr = BCM_GDPLL_IA_START_R5;

        /* Get the free Input array address */
        /*BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_input_array_alloc(unit,
            eventId, pIaAddr));*/
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "event_config_set: Error !! invalid input event\n")));
        rv = BCM_E_PARAM;
    }

    return (rv);
}

STATIC int
_bcm_esw_gdpll_chan_priority_set(int unit, int chan, bcm_gdpll_chan_priority_t chan_prio)
{
    /* NS_GDPLL_IA_UPDATE_CONFIG */
    int rv = BCM_E_NONE;
    uint32 regVal = 0;

    if (chan >= BCM_GDPLL_NUM_CHANNELS) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "chan_priority_set: Error !! invalid chan\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, SPEED_BINf, chan_prio);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, regVal);

exit:
    return (rv);
}

/****************************************************************************/
/*                      API functions implementation                        */
/****************************************************************************/

int
bcm_esw_gdpll_input_event_poll(int unit, bcm_gdpll_input_event_t input_event,
    uint32 timeout)
{
    uint32 rval = 0;
    uint32 poll_interval_uSec = 5;  /* 1uSec polling interval */
    uint32 poll_period_uSec = timeout;
    int rv = BCM_E_NONE;
    soc_field_t field;

    switch (input_event) {
        case bcmGdpllInputEventBS0HB:
        case bcmGdpllInputEventBS1HB:
            /* Set BS Input-Mode and Enable-Capture */
            if (input_event == bcmGdpllInputEventBS0HB) {
                READ_NS_BS0_BS_CONFIGr(unit, &rval);
                soc_reg_field_set(unit, NS_BS0_BS_CONFIGr, &rval, MODEf, 0);
                WRITE_NS_BS0_BS_CONFIGr(unit, rval);

                READ_NS_MISC_CLK_EVENT_CTRLr(unit, &rval);
                rval = rval | 0x2;
                WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, rval);

            } else {
                READ_NS_BS1_BS_CONFIGr(unit, &rval);
                soc_reg_field_set(unit, NS_BS1_BS_CONFIGr, &rval, MODEf, 0);
                WRITE_NS_BS1_BS_CONFIGr(unit, rval);

                READ_NS_MISC_CLK_EVENT_CTRLr(unit, &rval);
                rval = rval | 0x4;
                WRITE_NS_MISC_CLK_EVENT_CTRLr(unit, rval);
            }

            field = (input_event == bcmGdpllInputEventBS0HB) ? BS0_HEARTBEATf : BS1_HEARTBEATf;
            break;

        case bcmGdpllInputEventGPIO0:
        case bcmGdpllInputEventGPIO1:
        case bcmGdpllInputEventGPIO2:
        case bcmGdpllInputEventGPIO3:
        case bcmGdpllInputEventGPIO4:
        case bcmGdpllInputEventGPIO5:
            field = (input_event == bcmGdpllInputEventGPIO0) ? TS_GPIO_0f:
                (input_event == bcmGdpllInputEventGPIO1) ? TS_GPIO_1f:
                (input_event == bcmGdpllInputEventGPIO2) ? TS_GPIO_2f:
                (input_event == bcmGdpllInputEventGPIO3) ? TS_GPIO_3f:
                (input_event == bcmGdpllInputEventGPIO4) ? TS_GPIO_4f: TS_GPIO_5f;
            break;

        default:
            return BCM_E_UNAVAIL;
    }

    /* Clear if any flag is already set */
    soc_reg_field_set(unit, NS_MISC_EVENT_STATUSr, &rval, field, 1);
    WRITE_NS_MISC_EVENT_STATUSr(unit, rval);

    /* Poll on the next event */
    rv = BCM_E_TIMEOUT;
    while (poll_period_uSec > 0) {
        READ_NS_MISC_EVENT_STATUSr(unit, &rval);
        if (soc_reg_field_get(unit, NS_MISC_EVENT_STATUSr, rval, field)) {
            rv = BCM_E_NONE;
            break;
        }
        sal_usleep(poll_interval_uSec);
        poll_period_uSec -= poll_interval_uSec;
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_gdpll_chan_enable
 * Purpose:
 *      Channel enable
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      chan - (IN) Channel Number
 *      enable   - (IN) Enable flag
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_gdpll_chan_enable(int unit, int chan, int enable)
{
    int rv = BCM_E_NONE;
    bcm_gdpll_input_event_t event_misc;
    bcm_gdpll_port_event_t port_event_type;
    bcm_port_t port;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint8 buffer[DPLL_FW_VERSION_SIZE];

    if((chan >= BCM_GDPLL_NUM_CHANNELS) ||
       (pGdpllContext == NULL)) {
        rv = BCM_E_PARAM;
        return rv;
    }

    /* Channel is not allocated and feedbak is configured */
    if (!(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_CHAN_ALLOC) ||
        !(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_EVENT_CONFIG_FB)) {
        rv = BCM_E_NOT_FOUND;
        return rv;
    }

    /* Check if the M7 is alive to service the channel events */
    rv = _bcm_esw_gdpll_m7_status(unit, buffer);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "gdpll_chan_enable: M7-FW not running\n")));
        return rv;
    }

    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* Check if phaseCounterRef and event-ref is not configured */
    if (!pGdpllContext->dpll_chan[chan].phaseConterRef &&
        !(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_EVENT_CONFIG_REF)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                      "gdpll_chan_enable: Error !! phase ref error\n")));
        rv = BCM_E_PARAM;
        goto exit;
    }

    if (enable == 0) {
        /* For disable case, first disable the channel */
        _bcm_esw_gdpll_chan_enable_set(unit, chan, enable);
    }

    /* enable=1, Enable the events in sequence */
        bsl_printf("GDPLL: Set enable for feedback event\n");
        /* set enable for feedback event */
        _bcm_esw_gdpll_input_event_get(unit, chan, 1,
                    &event_misc, &port_event_type, &port);
        bsl_printf("GDPLL: chan_enable: FB event:%d port:%d event_type:%d\n", event_misc, port, port_event_type);
        if (event_misc == bcmGdpllInputEventPORT) {
            _bcm_esw_gdpll_event_enable_port_set(unit, port, port_event_type, enable);
        } else if (event_misc == bcmGdpllInputEventR5) {
            
        } else {
            _bcm_esw_gdpll_event_enable_misc_set(unit, event_misc, enable);
        }

        /* set enable for reference event */
        if (!pGdpllContext->dpll_chan[chan].phaseConterRef) {
            if (!(pGdpllContext->dpll_chan[chan].flag & BCM_GDPLL_EVENT_CONFIG_REF)) {
                /* Phase reference is not set, return error */
                LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                      "gdpll_chan_enable: Error !! phase ref is not set\n")));
                rv = BCM_E_PARAM;
                goto exit;
            }

            bsl_printf("GDPLL: Set enable for reference event\n");
            _bcm_esw_gdpll_input_event_get(unit, chan, 0,
                        &event_misc, &port_event_type, &port);
            bsl_printf("GDPLL: chan_enable: REF event:%d port:%d event_type:%d\n", event_misc, port, port_event_type);
            if (event_misc == bcmGdpllInputEventPORT) {
                _bcm_esw_gdpll_event_enable_port_set(unit, port, port_event_type, enable);
            } else if (event_misc == bcmGdpllInputEventR5) {
                
            } else {
                _bcm_esw_gdpll_event_enable_misc_set(unit, event_misc, enable);
            }
        }

    if (enable) {
        /*
         * For enable:1, first enable the IA capture and then enable the
         * GDPLL channel
         */
        /* Enable GDPLL input array capture */
        _bcm_esw_gdpll_capture_enable_m7_set(unit, enable);

        /* Enable the GDPLL channel */
        _bcm_esw_gdpll_chan_enable_set(unit, chan, enable);
    }

    /* For channel disable, clear the PAIR_AVAILABLE flag which is just
     * a debug hook
     */
    if (enable == 0) {
        /* Clear the status flags if any */
        WRITE_NS_REGr(unit, pair_available_regs[chan/32], 0, 1<<(chan%32));
        WRITE_NS_REGr(unit, ia_in_status_regs[chan/32], 0, 1<<(chan%32));
    }

    /* Set enable in channel context */
    if (rv == BCM_E_NONE) {
        pGdpllContext->dpll_chan[chan].enable = (uint8)enable;
    }

    /* Update the current state and trigger cb for notification */
    gdpll_state_change_chan_capture(unit, chan);
    sal_sem_give(pGdpllContext->cb_sem);

exit:
    sal_mutex_give(pGdpllContext->mutex);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_chan_create
 * Purpose:
 *      Initialize time module
 * Parameters:
 *      unit  - (IN) StrataSwitch Unit #.
 *      flags - (IN) Flags for channel create
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_gdpll_chan_create(int unit, uint32 flags,
                          bcm_gdpll_chan_t *pGdpllChan, int *pChan)
{
    int rv = BCM_E_NONE;
    uint32 iaAddr_fb    = BCM_GDPLL_NUM_IA_ENTRIES;   /* Indicating free */
    uint32 iaAddr_ref   = BCM_GDPLL_NUM_IA_ENTRIES;   /* Indicating free */
    uint32 eventId_fb   = BCM_GDPLL_NUM_INPUT_EVENTS; /* Indicating invalid event */
    uint32 eventId_ref  = BCM_GDPLL_NUM_INPUT_EVENTS; /* Indicating invalid event */
    int phaseCounterRef = 0;
    int  gdpll_port;
    uint32 dpllAddr, addrOffset;
    int use_rx_lanemap=0;

    int chan;
    int inUse = 0;
    dpll_param_t   dpllParam;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if ((pChan == NULL) || (pGdpllChan == NULL) ||
        (pGdpllContext == NULL)) {
        return BCM_E_PARAM;
    }

    /* Lock the channel access */
    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* Get the free channel
     * If the channel is allocated earlier, then pChan carries the
     * allocated channel number, otherwise flag should need to be
     * with BCM_GDPLL_CHAN_ALLOC
     */
    if(flags & BCM_GDPLL_CHAN_ALLOC) {
        /* Alloc new channel */
        if(_bcm_esw_gdpll_chan_alloc(unit, &chan) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        *pChan = chan;
    } else if((*pChan < BCM_GDPLL_NUM_CHANNELS) &&
              (pGdpllContext->dpll_chan[*pChan].flag & BCM_GDPLL_CHAN_ALLOC)){
        /* Channel is already allocated earlier and channel number
         * passed by the user
         */
        chan = *pChan;
    } else {
        /* Channel is not allocated earlier either */
        rv = BCM_E_PARAM;
        goto exit;
    }

    pGdpllContext->dpll_chan[chan].gdpll_chan_config = *pGdpllChan;
    /* Feedback event forward configuration */
    if(flags & BCM_GDPLL_EVENT_CONFIG_FB) {
        if (pGdpllChan->event_fb.input_event == bcmGdpllInputEventPORT) {
            if (pGdpllChan->event_fb.port_event_type == bcmGdpllPortEventTXPI) {
                if (IS_CL_PORT(unit, pGdpllChan->event_fb.port)) {
                    use_rx_lanemap = 0;
                } else if (IS_XL_PORT(unit, pGdpllChan->event_fb.port)) {
                    use_rx_lanemap = 1;
                }
                rv = _bcm_esw_gdpll_portmap_get(unit, pGdpllChan->event_fb.port, &gdpll_port, use_rx_lanemap);
                if (rv != BCM_E_NONE) {
                    goto exit;
                }

            } else if ((pGdpllChan->event_fb.port_event_type == bcmGdpllPortEventTXSOF) ||
                       (pGdpllChan->event_fb.port_event_type == bcmGdpllPortEventRXSOF) ||
                       (pGdpllChan->event_fb.port_event_type == bcmGdpllPortEventROE)) {
                gdpll_port = SOC_INFO(unit).port_l2p_mapping[pGdpllChan->event_fb.port];
                gdpll_port -= 1;
            } else {
                rv = _bcm_esw_gdpll_portmap_get(unit, pGdpllChan->event_fb.port, &gdpll_port, 1);
                if (rv != BCM_E_NONE) {
                    goto exit;
                }
            }
            bsl_printf("GDPLL: FB lPort:%d phys_port:%d gdpll_port:%d\n",
                pGdpllChan->event_fb.port, SOC_INFO(unit).port_l2p_mapping[pGdpllChan->event_fb.port], gdpll_port);

            pGdpllContext->dpll_chan[chan].lport_fb = pGdpllChan->event_fb.port;
            pGdpllContext->dpll_chan[chan].gdpllport_fb = gdpll_port;
            pGdpllChan->event_fb.port = gdpll_port;
        }

        if(_bcm_esw_gdpll_input_eventId_get(unit,
                    &pGdpllChan->event_fb,
                    &eventId_fb) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        bsl_printf("GDPLL: Feedback eventId:%d\n", eventId_fb);
        /* Check if the event is already under use */
        if(_bcm_esw_gdpll_input_eventId_inUse(unit,
                    eventId_fb, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (inUse) {
            /* Event is under use by other channel */
            rv = BCM_E_PARAM;
            goto exit;
        }

        /* Configure the input event and get iaAddr_fb */
        if(_bcm_esw_gdpll_event_config_set(unit, &(pGdpllChan->event_fb),
                    eventId_fb, &iaAddr_fb) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        pGdpllContext->eventId[eventId_fb] = iaAddr_fb;
    }

    /* Reference event forward configuration */
    if(flags & BCM_GDPLL_EVENT_CONFIG_REF) {
        if (pGdpllChan->event_ref.input_event == bcmGdpllInputEventPORT) {
            if (pGdpllChan->event_ref.port_event_type == bcmGdpllPortEventTXPI) {
                rv = _bcm_esw_gdpll_portmap_get(unit, pGdpllChan->event_ref.port, &gdpll_port, 1);
                if (rv != BCM_E_NONE) {
                    goto exit;
                }
            } else if ((pGdpllChan->event_ref.port_event_type == bcmGdpllPortEventTXSOF) ||
                       (pGdpllChan->event_ref.port_event_type == bcmGdpllPortEventRXSOF) ||
                       (pGdpllChan->event_ref.port_event_type == bcmGdpllPortEventROE)) {
                gdpll_port = SOC_INFO(unit).port_l2p_mapping[pGdpllChan->event_ref.port];
                gdpll_port -= 1;
            } else {
                rv = _bcm_esw_gdpll_portmap_get(unit, pGdpllChan->event_ref.port, &gdpll_port, 1);
                if (rv != BCM_E_NONE) {
                    goto exit;
                }
            }
            bsl_printf("GDPLL: REF lPort:%d phys_port:%d gdpll_port:%d\n",
                pGdpllChan->event_ref.port, SOC_INFO(unit).port_l2p_mapping[pGdpllChan->event_ref.port], gdpll_port);

            pGdpllContext->dpll_chan[chan].lport_ref = pGdpllChan->event_ref.port;
            pGdpllContext->dpll_chan[chan].gdpllport_ref = gdpll_port;
            pGdpllChan->event_ref.port = gdpll_port;
        }

        if (_bcm_esw_gdpll_input_eventId_get(unit,
                    &pGdpllChan->event_ref,
                    &eventId_ref) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        bsl_printf("GDPLL: Reference eventId:%d\n", eventId_ref);
        /* Check if the event is already under use */
        if(_bcm_esw_gdpll_input_eventId_inUse(unit,
                    eventId_ref, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        if (!inUse) {
            /* Configure the Reference event and get iaAddr_ref */
            if(_bcm_esw_gdpll_event_config_set(unit, &(pGdpllChan->event_ref), eventId_ref, &iaAddr_ref) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
            pGdpllContext->eventId[eventId_ref] = iaAddr_ref;
        }
    }

    /* Channel priority set */
    if(flags & BCM_GDPLL_CHAN_SET_PRIORITY) {
        if(_bcm_esw_gdpll_chan_priority_set(unit, chan,
                                pGdpllChan->chan_prio) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
    }

    /* Channel DPLL configuration */
    if(flags & BCM_GDPLL_CONF_DPLL) {
        dpllParam.dpll_config = pGdpllChan->chan_dpll_config;
        dpllParam.dpll_state  = pGdpllChan->chan_dpll_state;
        if(_bcm_esw_gdpll_chan_config_dpll_set(unit, chan,
                    &dpllParam) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (pGdpllChan->chan_dpll_config.phase_counter_ref) {
            phaseCounterRef = 1;
        }
    }

    /* Channel output configurations */
    if(flags & BCM_GDPLL_CHAN_OUTPUT_CONF) {
        if (pGdpllChan->out_event == bcmGdpllOutputEventTXPI) {
            /* Do not consider lane-map settings for NCO UPDATE hw_id */
            gdpll_port = SOC_INFO(unit).port_l2p_mapping[pGdpllChan->port] - 1;
            pGdpllChan->port = gdpll_port;
            bsl_printf("_bcm_esw_gdpll_chan_out_txpi_set : nco_hwid[%d]\n", pGdpllChan->port);
            if(_bcm_esw_gdpll_chan_out_txpi_set(unit, chan, pGdpllChan->port) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
        } else {
            if(_bcm_esw_gdpll_chan_out_misc_set(unit, chan, pGdpllChan->out_event) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
        }
    }

    /* Set the channel soft state data structures */
    if((flags & BCM_GDPLL_EVENT_CONFIG_REF) &&
       (pGdpllContext->dpll_chan[chan].phaseConterRef == 0)) {
        pGdpllContext->dpll_chan[chan].eventId_ref = eventId_ref;
    }

    if(flags & BCM_GDPLL_EVENT_CONFIG_FB) {
        pGdpllContext->dpll_chan[chan].eventId_fb = eventId_fb;
    }

    /* Set the channel soft state data structures */
    if (phaseCounterRef) {

        /* If the ref event is not in use by any channels, then free */
        if (pGdpllContext->dpll_chan[chan].eventId_ref < BCM_GDPLL_NUM_INPUT_EVENTS) {
            if(_bcm_esw_gdpll_input_eventId_inUse(unit,
               pGdpllContext->dpll_chan[chan].eventId_ref, &inUse) != BCM_E_NONE) {
                rv = BCM_E_INTERNAL;
                goto exit;
            }
            if (!inUse) {
                pGdpllContext->eventId[pGdpllContext->dpll_chan[chan].eventId_ref] = BCM_GDPLL_NUM_IA_ENTRIES;
            } else {
                bsl_printf("GDPLL: This channel has phaseCounterRef but the ref settings are used by other channel\n");
            }
        }

        pGdpllContext->dpll_chan[chan].phaseConterRef = 1;
        pGdpllContext->dpll_chan[chan].eventId_ref = BCM_GDPLL_NUM_INPUT_EVENTS; /* Indicating invalid event */;
    }

    /* Now update the flags for the channel */
    pGdpllContext->dpll_chan[chan].flag |= flags;

    /* Upadet the channel with ref and fb event */
    _bcm_esw_gdpll_chan_update_set(unit, chan);

    /* Program the lockIndicator and offset of 1588 counter to the location under for R5 to consume for PTP */
    if ((pGdpllChan->out_event == bcmGdpllOutputEventTS0) ||
        (pGdpllChan->out_event == bcmGdpllOutputEventTS1)) {

        dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

        /* Set the LockIndicator location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.LockIndicator);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_1588_FREQ_LOCK_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing FreqLockInd_addr:0x%08x@0x%08x 1588 TS0/TS1 out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_1588_FREQ_LOCK_LOC)));

        /* Set the Offset location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_1588_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing DpllOffset_addr:0x%08x@0x%08x for 1588 TS0/TS1 out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_1588_DPLL_OFFSET_LOC)));
        if (pGdpllChan->event_fb.input_event == bcmGdpllInputEventBS0HB) {
            soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC, chan);
        }
    } else if ( (pGdpllChan->event_fb.input_event == bcmGdpllInputEventBS0PLL) &&
                (pGdpllChan->out_event == bcmGdpllOutputEventBSPLL0)) {

        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC, chan);
        dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

        /* Set the LockIndicator location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.LockIndicator);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_FREQ_LOCK_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing FreqLockInd_addr:0x%08x@0x%08x BS0PLL out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_BS0PLL_FREQ_LOCK_LOC)));

        /* Set the Offset location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing DpllOffset_addr:0x%08x@0x%08x for BS0PLL out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_BS0PLL_DPLL_OFFSET_LOC)));
    } else if ( (pGdpllChan->event_fb.input_event == bcmGdpllInputEventBS1PLL) &&
                (pGdpllChan->out_event == bcmGdpllOutputEventBSPLL1)) {

        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1_UPDATE_LOC, chan);

        dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);

        /* Set the LockIndicator location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.LockIndicator);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_FREQ_LOCK_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing FreqLockInd_addr:0x%08x@0x%08x BS1PLL out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_BS1PLL_FREQ_LOCK_LOC)));

        /* Set the Offset location on DTCM */
        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.Offset);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_DPLL_OFFSET_LOC, dpllAddr+addrOffset);
        LOG_VERBOSE(BSL_LS_BCM_PTP,
                    (BSL_META_U(unit,
                        " Writing DpllOffset_addr:0x%08x@0x%08x for BS0PLL out event \n"),
                        (dpllAddr+addrOffset), (BCM_M7_DTCM_BASE + DPLL_BS1PLL_DPLL_OFFSET_LOC)));
    }

exit:
    sal_mutex_give(pGdpllContext->mutex);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_chan_destroy
 * Purpose:
 *      Destroys the GDPLL channel
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      chan - (IN) Channel number
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_gdpll_chan_destroy(int unit, int chan)
{
    int rv = BCM_E_NONE;
    int inUse = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
#ifdef BCN_WAR
    uint32 enableReg = 0;
    uint32 disable_chan_nco = 0;
#endif

    if((chan >= BCM_GDPLL_NUM_CHANNELS) ||
       (pGdpllContext == NULL)) {
        return BCM_E_PARAM;
    }

    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* If channel is not created, then return BCM_E_NOT_FOUND */
    if (pGdpllContext->dpll_chan[chan].flag == 0) {
        return BCM_E_NOT_FOUND;
    }

    /* Disable the channel */
    bcm_esw_gdpll_chan_enable(unit, chan, 0);
    pGdpllContext->dpll_chan[chan].flag = 0;

    /* If the ref event is not in use by any channels, then free */
    if (pGdpllContext->dpll_chan[chan].eventId_ref < BCM_GDPLL_NUM_INPUT_EVENTS) {
        if(_bcm_esw_gdpll_input_eventId_inUse(unit,
           pGdpllContext->dpll_chan[chan].eventId_ref, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (!inUse) {
            pGdpllContext->eventId[pGdpllContext->dpll_chan[chan].eventId_ref] = BCM_GDPLL_NUM_IA_ENTRIES;
        } else {
            bsl_printf("GDPLL: Error !! Ref event is in use by other channel \n");
        }
    }

    /* If the fb event is not in use by any channels, then free */
    if (pGdpllContext->dpll_chan[chan].eventId_fb < BCM_GDPLL_NUM_INPUT_EVENTS) {
        if(_bcm_esw_gdpll_input_eventId_inUse(unit,
            pGdpllContext->dpll_chan[chan].eventId_fb, &inUse) != BCM_E_NONE) {
            rv = BCM_E_INTERNAL;
            goto exit;
        }
        if (!inUse) {
            pGdpllContext->eventId[pGdpllContext->dpll_chan[chan].eventId_fb] = BCM_GDPLL_NUM_IA_ENTRIES;
        } else {
            bsl_printf("GDPLL: Error !! FB event is IN use by other channel\n");
        }
    }

    _bcm_esw_gdpll_input_array_free(unit,
        pGdpllContext->dpll_chan[chan].eventId_ref);
    pGdpllContext->dpll_chan[chan].eventId_ref = BCM_GDPLL_NUM_INPUT_EVENTS;

    _bcm_esw_gdpll_input_array_free(unit,
        pGdpllContext->dpll_chan[chan].eventId_fb);
    pGdpllContext->dpll_chan[chan].eventId_fb = BCM_GDPLL_NUM_INPUT_EVENTS;
    pGdpllContext->dpll_chan[chan].phaseConterRef = 0; /*BCM_GDPLL_NUM_INPUT_EVENTS; */

#ifdef BCN_WAR
    /*
     * If the channel is for BCN output, then clear location to indicate R5,
     * not to process the NCO and update BCN counter
     */
    if(chan == soc_iproc_m7_read(unit, BCM_M7_DTCM_BASE + DPLL_BCN_UPDATE_LOC)) {
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BCN_UPDATE_LOC, 0xFF);
        disable_chan_nco = 1;
    } else if (chan == soc_iproc_m7_read(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC)) {
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_UPDATE_LOC, 0xFF);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_NTPTOD_DPLL_OFFSET_LOC, 0x0);
        disable_chan_nco = 1;
    } else if (chan == soc_iproc_m7_read(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC)) {
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_UPDATE_LOC, 0xFF);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_PTPTOD_DPLL_OFFSET_LOC, 0x0);
        disable_chan_nco = 1;
    }
    if (chan == soc_iproc_m7_read(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC)) {
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0_UPDATE_LOC, 0xFF);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS0PLL_DPLL_OFFSET_LOC, 0x0);
    } else if (chan == soc_iproc_m7_read(unit, BCM_M7_DTCM_BASE + DPLL_BS1_UPDATE_LOC)){
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1_UPDATE_LOC, 0xFF);
        soc_iproc_m7_write(unit, BCM_M7_DTCM_BASE + DPLL_BS1PLL_DPLL_OFFSET_LOC, 0x0);
    }

    if (disable_chan_nco) {
        /* Disable the Channel Output array status reporting over interrupt */
        READ_NS_REGr(unit, nco_out_enable_regs[chan/32], 0, &enableReg);
        enableReg &= ~(1<<(chan%32));
        soc_reg_field_set(unit, nco_out_enable_regs[chan/32], &enableReg, INTENf, enableReg);
        WRITE_NS_REGr(unit, nco_out_enable_regs[chan/32], 0, enableReg);
    }
#endif

exit:
    sal_mutex_give(pGdpllContext->mutex);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_chan_state_get
 * Purpose:
 *      Get Channel status
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      chan  - (IN) Channel Number
 *      dpllState - (OUT) DPLL State
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_gdpll_chan_state_get(int unit, int chan, uint32 *dpllState)
{
    int rv = BCM_E_NONE;
    dpll_param_t dpllParam;
    uint32 regVal = 0;
    uint8 chan_enable = 0;
    uint8 chan_err = 0;
    uint32 dpllAddr = DPLL_CHAN_PARAM_ADDR(BCM_M7_DTCM_BASE, chan);
    uint32 addrOffset;
    uint8 lError = 0;
    uint8 lock;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if ((chan >= BCM_GDPLL_NUM_CHANNELS) || (chan < 0) || (dpllState == NULL) ||
        (!pGdpllContext)) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check the channel enable status */
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, chan, &regVal);
    chan_enable = soc_reg_field_get(unit, NS_GDPLL_IA_UPDATE_CONFIGr, regVal, ENABLEf);
    if (!chan_enable) {
        /* Channel is not enabled */
        *dpllState = 0;
        goto exit;
    }

    /* If state change callback is already registered, then get the state from SW state
     * maintained per async notification
     */
    if (pGdpllContext->gdpll_cb[bcmGdpllCbChanState]) {
        if (pGdpllContext->dpll_chan[chan].prev_state.state & BCM_GDPLL_CHAN_STATE_ERROR) {
            /* Error can be either a DPLL error or the HW error */
            if (pGdpllContext->dpll_chan[chan].prev_state.error_code != bcmGdpllChanErrorDpllLError) {
                /* Its a HW error */
                chan_err = 1;
            } else {
                /* Its a DPLL large error */
                lError = 1;
            }
        }

        /* Get the DPLL lock */
        lock = (pGdpllContext->dpll_chan[chan].prev_state.state & BCM_GDPLL_CHAN_STATE_LOCK) ? 1:0;
        *dpllState = lock | chan_enable << 8 | lError << 30 | chan_err << 31;
        goto exit;
    }

    /* Following state retrieval is when the callback is not registered */
    READ_NS_REGr(unit, ia_in_status_regs[chan/32], 0, &regVal);
    chan_err = (regVal & (1<<(chan%32))) ? 1:0;

    if(_bcm_esw_gdpll_chan_config_dpll_get(unit, chan,
                &dpllParam) != BCM_E_NONE) {
        rv = BCM_E_INTERNAL;
        goto exit;
    }

    /* Clear the M7-DPLL indications */
    if (chan_err) {
        dpllParam.dpll_state.lockIndicator = 0;
        dpllParam.dpll_state.lossOfLockIndicator = 0;
        dpllParam.dpll_state.largeErrorIndicator = 0;

        addrOffset = SAL_OFFSETOF(m7_dpll_param_t, m7_dpll_state.LockIndicator);
        soc_iproc_m7_write(unit, dpllAddr+addrOffset, 0);

        /* Clear the channel error, if any */
        WRITE_NS_REGr(unit, ia_in_status_regs[chan/32], 0, 1<<(chan%32));
    }
    *dpllState = dpllParam.dpll_state.lockIndicator |
                 (dpllParam.dpll_state.lossOfLockIndicator & 0x7F) << 1 |
                 chan_enable << 8 | chan_err << 31 |
                 (dpllParam.dpll_state.largeErrorIndicator & 0x1) << 30;
exit:
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_chan_debug_enable
 * Purpose:
 *      Enable Channel debugging
 * Parameters:
 *      unit     - (IN) StrataSwitch Unit #.
 *      chan - (IN) Channel Number
 *      debug_mode  - (IN) Debug mode
 *      enable      - (IN) Enable flag
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_gdpll_chan_debug_enable(int unit, int chan,
                    bcm_gdpll_debug_mode_t debug_mode, int enable)
{
    int rv = BCM_E_NONE;

    if(chan >= BCM_GDPLL_NUM_CHANNELS) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_gdpll_chan_debug_mode_set(unit, chan,
                        debug_mode));
exit:
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_debug_cb_register
 * Purpose:
 *      Register Channel debug callback
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      user_data - (IN) Users private data
 *      cb      - (IN) Callback handler
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_gdpll_debug_cb_register(int unit, void *user_data,
                            bcm_gdpll_debug_cb cb)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if(pGdpllContext == NULL) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    pGdpllContext->debug_cb = cb;
    pGdpllContext->pUserData = user_data;

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_cb_register
 * Purpose:
 *      Register Generic GDPLL callback
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      cb_type - (IN) Callback type
 *      cb      - (IN) Callback handler
 *      user_data - (IN) Users private data
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int bcm_esw_gdpll_cb_register(int unit, bcm_gdpll_cb_reg_t cb_type,
                          bcm_gdpll_cb cb, void *user_data)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    uint32 regVal = 0;
    uint32 intEnable = 0;

    /* Param Validation */
    if (cb_type >= bcmGdpllCbMax) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Check if cb is already registered */
    if ((pGdpllContext == NULL) ||
       (cb && (pGdpllContext->gdpll_cb[cb_type] != NULL))) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    /* Lock as we update the interrupt enable register */
    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    pGdpllContext->gdpll_cb[cb_type] = cb;
    pGdpllContext->pGdpll_cb_data[cb_type] = user_data;

    /* Read the main int enable */
    READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &intEnable);

    /* Callback specific controls */
    switch (cb_type) {
    case bcmGdpllCbRsvd1:
        /* Enable the RSVD1 message arrival status leaf-level interrupt(both RX and ERR interrupts)
         * in MESSAGE_INT_ENABLE(BRCM_RESERVED_IPROC_10)
         * Enable RSVD1 interrupt on Nanosync interrupt line in NS_TS_INT_ENABLE(bit-22)
         */
        if (soc_feature(unit, soc_feature_rsvd1_intf)) {

            /* Clear the earlier status if any */
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_9r, 0, 0x3);

            /* Enable the RSVD1 leaf-level interupt, both RX and ERR */
            READ_NS_REGr(unit, BRCM_RESERVED_IPROC_10r, 0, &regVal);
            regVal |= 0x3;
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_10r, 0, regVal);

            /* Enable RSVD1 status interrupt on Nanosync intr line */
            intEnable |= INTR_RSVD1_MESSAGE_RX_STATUS;
        }
        break;

    case bcmGdpllCbChanState:
        /* Enable the M7 core-interrupt ERROR status reporting */
        READ_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_ENABLEr, 0, &regVal);
        regVal |= 0x8;
        WRITE_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_ENABLEr, 0, regVal);

        /* Capture the DPLL state and post sema for cb before enabling interrupts */
        gdpll_state_change_capture(unit);
        sal_sem_give(pGdpllContext->cb_sem);

        /* Enable M7 Core and IA error interrupts */
        intEnable |= INTR_M7_CORE_INT_STATUS;
        intEnable |= INTR_GDPLL_IA_0_STATUS | INTR_GDPLL_IA_1_STATUS |
                  INTR_GDPLL_IA_2_STATUS | INTR_GDPLL_IA_3_STATUS;
        break;

    case bcmGdpllCbMax:
        break;
    }

    /* Set the int enable bits */
    WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, intEnable);
    sal_mutex_give(pGdpllContext->mutex);

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_cb_unregister
 * Purpose:
 *      Un-register Generic GDPLL callback
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      cb_type - (IN) Callback type
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int bcm_esw_gdpll_cb_unregister(int unit, bcm_gdpll_cb_reg_t cb_type)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    /* Param Validation */
    if (cb_type >= bcmGdpllCbMax) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    if(pGdpllContext == NULL) {
        rv = BCM_E_PARAM;
        goto exit;
    }
    sal_mutex_take(pGdpllContext->mutex, sal_mutex_FOREVER);

    /* Callback specific controls */
    switch (cb_type) {
    case bcmGdpllCbRsvd1:
        if (soc_feature(unit, soc_feature_rsvd1_intf)) {
            /* Disable RSVD1 status interrupt on Nanosync intr line */
            READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &regVal);
            regVal &= ~INTR_RSVD1_MESSAGE_RX_STATUS;
            WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, regVal);

            /* Disable the RSVD1 leaf-level interupt, both RX and ERR */
            regVal = 0;
            WRITE_NS_REGr(unit, BRCM_RESERVED_IPROC_10r, 0, regVal);
        }
        break;

    case bcmGdpllCbChanState:
        /* Disable the M7 core ERROR interrupt */

        /* Enable the M7 core-interrupt ERROR status reporting */
        READ_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_ENABLEr, 0, &regVal);
        regVal &= ~0x8;
        WRITE_NS_REGr(unit, NS_GDPLL_M7_CORE_INT_ENABLEr, 0, regVal);

        /* Disable M7 Core and IA error interrupts */
        READ_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, &regVal);
        regVal &= ~INTR_M7_CORE_INT_STATUS;
        regVal &= ~INTR_GDPLL_IA_0_STATUS;
        regVal &= ~INTR_GDPLL_IA_1_STATUS;
        regVal &= ~INTR_GDPLL_IA_2_STATUS;
        regVal &= ~INTR_GDPLL_IA_3_STATUS;
        WRITE_NS_REGr(unit, NS_TS_INT_ENABLEr, 0, regVal);

        break;

    case bcmGdpllCbMax:
        break;
    }

    pGdpllContext->gdpll_cb[cb_type] = NULL;
    pGdpllContext->pGdpll_cb_data[cb_type] = NULL;
    sal_mutex_give(pGdpllContext->mutex);

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_flush
 * Purpose:
 *      Flush the GDPLL pipeline
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_gdpll_flush (int unit)
{
    int rv = BCM_E_NONE;
    int chan = 0;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if(pGdpllContext == NULL) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    for(chan = 0; chan < BCM_GDPLL_NUM_CHANNELS; chan++) {
        bcm_esw_gdpll_chan_destroy(unit, chan);
    }

    /* Disable the GDPLL capture */
    _bcm_esw_gdpll_capture_enable_m7_set(unit, 0);

    pGdpllContext->debug_cb = NULL;
    pGdpllContext->pUserData = NULL;

    _bcm_esw_gdpll_flush(unit);

exit:
    return (rv);
}

/*
 * Function:
 *      bcm_esw_gdpll_debug
 * Purpose:
 *      Enable the GDPLL global debug feature
 *      This will set the flag for M7 to start pushing the debug
 *      content to debug buffers
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcm_esw_gdpll_debug (int unit, int enable)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
    uint32 dpllDebugFlag = BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET;

    if (enable) {
        /* Check if its already enabled */
        if(soc_iproc_m7_read (unit, dpllDebugFlag)) {
            /* Already enabled. Return error */
            rv = BCM_E_PARAM;
            goto exit;
        }

        /* Reset the debug pointers and re-enable */
        READ_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, &regVal);
        soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, &regVal, ENABLEf, 0);
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, regVal);

        soc_reg_field_set(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, &regVal, ENABLEf, 1);
        WRITE_NS_REGr(unit, NS_GDPLL_DEBUG_M7DG_ENABLEr, 0, regVal);
    }

    soc_iproc_m7_write(unit, dpllDebugFlag, enable);

exit:
    return (rv);
}

int
_bcm_esw_gdpll_debug_get (int unit, int *pEnable)
{
    int rv = BCM_E_NONE;
    uint32 dpllDebugFlag = BCM_M7_DTCM_BASE + DPLL_DEBUG_ENABLE_OFFSET;

    if (!pEnable) {
        rv = BCM_E_PARAM;
        goto exit;
    }
    *pEnable = soc_iproc_m7_read (unit, dpllDebugFlag);

exit:
    return rv;
}

/* Retrieve debug dump on a given channel
 * unit: (IN)
 * pChanDump: (IN/OUT)
 */
int
_bcm_esw_gdpll_debug_dump (int unit, dpll_dump_t *pChanDump)
{
    int rv = BCM_E_NONE;
    uint32 *pData;
    uint32 chan;
    uint32 poll_interval_uSec = 100;  /* 1uSec polling interval */
    int count;
    uint32 dumpAddr = BCM_M7_DTCM_BASE + DPLL_CHAN_DUMP_LOC;
    uint32 addr;

    /* polling interval is set to 4-secs, assuming that 1G channel
     * rate at 1Hz */
    uint32 poll_period_interval = 4*1000000; /* 4*1000000uSec=4sec */

    if (!pChanDump) {
        rv = BCM_E_PARAM;
        goto exit;
    }

    chan = swap32(soc_iproc_m7_read (unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan)));
    if (chan != 0xDEAD) {
        bsl_printf("ERROR: M7FW Busy 0x%x\n", chan);

        /* Comand M7 not to generate dump */
        soc_iproc_m7_write(unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan),
                       swap32(0xDEAD));
        rv = BCM_E_BUSY;
        goto exit;
    }

    /* Write the channel number and poll for M7 to respond */
    rv = BCM_E_TIMEOUT;
    chan = pChanDump->chan;
    soc_iproc_m7_write(unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan), swap32(chan));
    while (poll_period_interval > 0) {
        /* Check response from M7 */
        if (0xDEAD == swap32(soc_iproc_m7_read (unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan)))) {
            /* Debug dump is captured, read the DTCM locations for DPLL config */
            addr = dumpAddr;
            pData = (uint32 *)pChanDump;
            for (count = 0; count < sizeof(dpll_dump_t);) {
                *pData = swap32(soc_iproc_m7_read(unit, addr));
                pData++;
                count += sizeof(uint32);
                addr += sizeof(uint32);
            }
            pChanDump->chan = chan;

            /* Compensate the endianness chage for 64b */
            count = 0;
            while (count < 2) {
                u64_H(pChanDump->debug_dump[count].feedback) = swap64_h(pChanDump->debug_dump[count].feedback);
                u64_L(pChanDump->debug_dump[count].feedback) = swap64_l(pChanDump->debug_dump[count].feedback);

                u64_H(pChanDump->debug_dump[count].reference) = swap64_h(pChanDump->debug_dump[count].reference);
                u64_L(pChanDump->debug_dump[count].reference) = swap64_l(pChanDump->debug_dump[count].reference);

                u64_H(pChanDump->debug_dump[count].perror) = swap64_h(pChanDump->debug_dump[count].perror);
                u64_L(pChanDump->debug_dump[count].perror) = swap64_l(pChanDump->debug_dump[count].perror);

                count++;
            }

            rv = BCM_E_NONE;
            break;
        }
        sal_usleep(poll_interval_uSec);
        poll_period_interval -= poll_interval_uSec;
    }

    /* Comand M7 not to generate dump */
    soc_iproc_m7_write(unit, dumpAddr + SAL_OFFSETOF(dpll_dump_t, chan),
                       swap32(0xDEAD));
exit:
    return rv;
}

int
_bcm_esw_gdpll_tdpllinstance_bind(int unit, int gdpllChan, uint32 out_clock_offset)
{
    int rv = BCM_E_NONE;
    uint32 regVal = 0;
/*    uint32  eventId_ref;
    bcm_gdpll_chan_input_t inputEvent;*/
    uint32 iaAddr;
    uint32 dest_hw_id = 0;
    uint32 dest_hw_type = 0;

    if ((gdpllChan < 0 || gdpllChan >= BCM_GDPLL_NUM_CHANNELS)) {
        bsl_printf("_bcm_esw_gdpll_tdpllinstance_bind: Error!! parameters\n");
        rv = BCM_E_PARAM;
        goto exit;
    }
#if 0

    if (BCM_GDPLL_NUM_IA_ENTRIES == gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]) {
        /* Alloc the IA entry */
        inputEvent.input_event = bcmGdpllInputEventR5;
        if(_bcm_esw_gdpll_input_eventId_get(unit,
                    &inputEvent,
                    &eventId_ref) != BCM_E_NONE) {
            bsl_printf("_bcm_esw_gdpll_tdpllinstance_bind: Error!! in input_eventId_get \n");
            rv = BCM_E_INTERNAL;
            goto exit;
        }

        rv = _bcm_esw_gdpll_input_array_alloc(unit, eventId_ref, &iaAddr);
        if (rv != BCM_E_NONE) {
            bsl_printf("_bcm_esw_gdpll_tdpllinstance_bind: Error!! input_array_alloc failed. %s\n", bcm_errmsg(rv));
            goto exit;
        }

        gdpll_tdpll_state.tdpll_instace[tdpll_instance_num] = iaAddr;
    }

    iaAddr = gdpll_tdpll_state.tdpll_instace[tdpll_instance_num];
    bsl_printf("_bcm_esw_gdpll_tdpllinstance_bind Channel: %d ref:%d/%d\n", gdpllChan, gdpll_tdpll_state.tdpll_instace[tdpll_instance_num], gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]+1);
#else
    iaAddr = 214 + out_clock_offset;
#endif

    
    /* Channel update with reference indix */
    READ_NS_REGr(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, gdpllChan, &regVal);
    dest_hw_id =  soc_reg_field_get(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, regVal, DEST_HW_IDf);
    dest_hw_type = soc_reg_field_get(unit, NS_GDPLL_NCO_UPDATE_CONTROLr, regVal, DEST_HW_TYPEf);

    /* If the channel is 1588, set feedback or set reference */
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, gdpllChan, &regVal);
    if (((dest_hw_id == 1) || (dest_hw_id == 0)) && (dest_hw_type == 1)) {
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, FEEDBACK_IDf, iaAddr);
    } else {
        soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, REFERENCE_IDf, iaAddr);
    }

    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, gdpllChan, regVal);

exit:
    return rv;
}

uint32 _bcm_esw_gdpll_event_fb_divisor_get(int unit, int chan) {
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];
    return (pGdpllContext->dpll_chan[chan].gdpll_chan_config.event_fb.event_divisor);
}

#if 0
int
_bcm_esw_tdpll_chan_create(int unit, uint32 flags,
                          bcm_gdpll_chan_t *pGdpllChan, int *pChan,
                          uint32 tdpll_instance_num)
{
    int rv = BCM_E_NONE;
    uint32 iaAddr;
    uint32 eventId_ref;
    uint32 regVal = 0;

    /* Checks if reference is set to be from R5 and check instance number */
    if ((bcmGdpllInputEventR5 != pGdpllChan->event_ref.input_event) ||
        (tdpll_instance_num < 0 || tdpll_instance_num >= NUM_TDPLL_INSTANCE)) {
        bsl_printf("_bcm_esw_tdpll_chan_create: Error!! parameters\n");
        rv = BCM_E_PARAM;
        goto exit;
    }

    rv = bcm_esw_gdpll_chan_create (unit, flags, pGdpllChan, pChan);
    if (BCM_FAILURE(rv)) {
        bsl_printf("_bcm_esw_tdpll_chan_create: Error!! gdpll_chan_create failed. %s\n", bcm_errmsg(rv));
        goto exit;
    }

    if (BCM_GDPLL_NUM_IA_ENTRIES == gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]) {
        /* Alloc the IA entry */
        if(_bcm_esw_gdpll_input_eventId_get(unit,
                    &pGdpllChan->event_ref,
                    &eventId_ref) != BCM_E_NONE) {
            bsl_printf("_bcm_esw_tdpll_chan_create: Error!! in input_eventId_get \n");
            rv = BCM_E_INTERNAL;
            goto deleteChan;
        }

        rv = _bcm_esw_gdpll_input_array_alloc(unit, eventId_ref, &iaAddr);
        if (rv != BCM_E_NONE) {
            bsl_printf("_bcm_esw_tdpll_chan_create: Error!! input_array_alloc failed. %s\n", bcm_errmsg(rv));
            goto deleteChan;
        }

        gdpll_tdpll_state.tdpll_instace[tdpll_instance_num] = iaAddr;
    }

    bsl_printf("_bcm_esw_tdpll_chan_create Channel: %d ref:%d\n", *pChan, gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]);

    /* Channel update with reference indix */
    READ_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, *pChan, &regVal);
    soc_reg_field_set(unit, NS_GDPLL_IA_UPDATE_CONFIGr, &regVal, REFERENCE_IDf,
                    gdpll_tdpll_state.tdpll_instace[tdpll_instance_num]);
    WRITE_NS_REGr(unit, NS_GDPLL_IA_UPDATE_CONFIGr, *pChan, regVal);
    return rv;

deleteChan:
    rv = bcm_esw_gdpll_chan_destroy(unit, *pChan);
    if (BCM_FAILURE(rv)) {
        bsl_printf("_bcm_esw_tdpll_chan_create: chan destroy failed\n");
    }
exit:
    return rv;
}

/*
 * Call this API if user want to perform one of the bellow or both
 * 1. disassociate instance with all outputs with valid instance number
 * 2. Free the channel allocated
 */
int
_bcm_esw_tdpll_gdpll_chan_destroy(int unit, int chan, uint32 tdpll_instance_num)
{
    int rv = BCM_E_NONE;

    rv = bcm_esw_gdpll_chan_destroy(unit, chan);
    if (BCM_FAILURE(rv)) {
        bsl_printf("_bcm_esw_tdpll_gdpll_chan_destroy: chan destroy failed\n");
    }

    /* Free the instance with associated outputs */
    if ((tdpll_instance_num >= 0) && (tdpll_instance_num < NUM_TDPLL_INSTANCE)) {
        gdpll_tdpll_state.tdpll_instace[tdpll_instance_num] = BCM_GDPLL_NUM_IA_ENTRIES;
    }

    return rv;
}
#endif

int bcm_esw_gdpll_chan_get(
    int unit,
    int chan,
    bcm_gdpll_chan_t *gdpll_chan)
{
    int rv = BCM_E_NONE;
    gdpll_context_t *pGdpllContext = pGdpllCtx[unit];

    if ((chan < 0) || (chan > BCM_GDPLL_NUM_CHANNELS) || (gdpll_chan == NULL)){
        rv = BCM_E_PARAM;
        goto exit;
    }

    sal_memcpy(gdpll_chan, &(pGdpllContext->dpll_chan[chan].gdpll_chan_config),
                       sizeof(bcm_gdpll_chan_t));
exit:
    return rv;
}
#endif
