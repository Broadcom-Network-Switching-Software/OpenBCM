/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM layer SAT APIs
 */

#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/pkt.h>
#include <bcm_int/control.h>
#include <bcm_int/common/sat.h>
#include <bcm_int/common/debug.h>
#if defined(BCM_DPP_SUPPORT)
#include <bcm_int/dpp/oam.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#endif
#include <bcm/sat.h>
#if defined(BCM_SABER2_SUPPORT)
#include <bcm_int/esw/saber2.h>
#endif /* BCM_SABER2_SUPPORT */
#include <sal/core/alloc.h>

#include <soc/drv.h>
#include <soc/shared/mbcm_sat.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#include <bcm/module.h>
#endif

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif

#define _BCM_SAT_NUM_CLKS_PER_SEC_JERICHO    720000000
#define _BCM_SAT_NUM_CLKS_PER_SEC_SABER2     130000000
#define _BCM_SAT_BPS_MODE                    0x1
#define _BCM_SAT_PPS_MODE                    0x2
#define _BCM_SAT_CMIC_TOD_MODE               0x2
#define _BCM_SAT_RATE_NUM_CLKS_CYCLE         720000
#define _BCM_SAT_PAYLOAD_OFFSET_MIN          0
#define _BCM_SAT_PAYLOAD_OFFSET_MAX          0xFFFF
#define _BCM_SAT_SEQ_NUM_OFFSET_MIN          -1
#define _BCM_SAT_SEQ_NUM_OFFSET_MAX          0xFFFF
#define _BCM_SAT_TIME_STAMP_OFFSET_MIN       0
#define _BCM_SAT_TIME_STAMP_OFFSET_MAX       0xFFFF
#define _BCM_SAT_CRC_BYTE_OFFSET_MIN         0
#define _BCM_SAT_CRC_BYTE_OFFSET_MAX         0xFFFF
#define _BCM_SAT_MEF_PAYLOAD_OFFSET          41
#define _BCM_SAT_MEF_SEQ_NUM_OFFSET          30
#define _BCM_SAT_MEF_TIME_STAMP_OFFSET       0
#define _BCM_SAT_MEF_CRC_BYTE_OFFSET_OFFSET      20
#define _BCM_SAT_Y1731_PAYLOAD_OFFSET        0
#define _BCM_SAT_Y1731_SEQ_NUM_OFFSET        0
#define _BCM_SAT_Y1731_TIME_STAMP_OFFSET     22
#define _BCM_SAT_Y1731_CRC_BYTE_OFFSET_OFFSET    20

#define _BCM_SAT_GTF_ID_MIN                  0
#define _BCM_SAT_GTF_ID_MAX                  7

#define _BCM_SAT_GTF_OBJ_COMMON              -1
#define _BCM_SAT_GTF_OBJ_CIR                 0
#define _BCM_SAT_GTF_OBJ_EIR                 1

#define _BCM_SAT_GTF_PKT_HDR_LEN_MIN         1
#define _BCM_SAT_GTF_PKT_HDR_LEN_MAX         127
#define _BCM_SAT_GTF_PKT_LEN_PATN_INX_MIN      0
#define _BCM_SAT_GTF_PKT_LEN_PATN_INX_MAX      7
#define _BCM_SAT_GTF_SEQ_PERIOD_MIN          1
#define _BCM_SAT_GTF_SEQ_PERIOD_MAX          4
#define _BCM_SAT_GTF_STAMP_INC_STEP_MIN      1
#define _BCM_SAT_GTF_STAMP_INC_STEP_MAX      0x7f
#define _BCM_SAT_GTF_STAMP_INC_PERIOD_MIN    0
#define _BCM_SAT_GTF_STAMP_INC_PERIOD_MAX    0x3
#define _BCM_SAT_GTF_STAMP_2_bit_VALUE_MIN   0
#define _BCM_SAT_GTF_STAMP_2_bit_VALUE_MAX   0x3

#define _BCM_SAT_GTF_BW_RATE_MAX                             23000000                /* 23Gbits */
#define _BCM_SAT_GTF_BW_RATE_MAX_QAX                         19200000                /* for QAX the max rate is 19.2Gbit/sec */
#define _BCM_SAT_GTF_BW_RATE_MAX_QUX                         8000000                 /* for QUX the max rate is 8Gbit/sec */
#define _BCM_SAT_GTF_BW_RATE_MIN                             0 
#define _BCM_SAT_GTF_BW_BURST_MAX                            (0x7fff8000/125)        /* 0xffff*(1<<15)*8/1000 */
#define _BCM_SAT_GTF_BW_BURST_MIN                            0
#define _BCM_SAT_GTF_RATE_PATN_HIGH_TH_MAX                   23000000                /* 23Gbits */
#define _BCM_SAT_GTF_RATE_PATN_HIGH_TH_MIN                   1
#define _BCM_SAT_GTF_RATE_PATN_LOW_TH_MAX                    23000000                /* 23Gbits */
#define _BCM_SAT_GTF_RATE_PATN_LOW_TH_MIN                    1
#define _BCM_SAT_GTF_RATE_PATN_STOP_ITER_MAX                 0x3ff
#define _BCM_SAT_GTF_RATE_PATN_STOP_ITER_MIN                 0
#define _BCM_SAT_GTF_RATE_PATN_STOP_BURST_MAX                0xffffffff
#define _BCM_SAT_GTF_RATE_PATN_STOP_BURST_MIN                0
#define _BCM_SAT_GTF_RATE_PATN_STOP_INTERVAL_MAX             0xffffffff
#define _BCM_SAT_GTF_RATE_PATN_STOP_INTERVAL_MIN             0
#define _BCM_SAT_GTF_RATE_PATN_BURST_PACKET_WEIGHT_MAX       0xffffffff
#define _BCM_SAT_GTF_RATE_PATN_BURST_PACKET_WEIGHT_MIN       0
#define _BCM_SAT_GTF_RATE_PATN_INTERVAL_PACKET_WEIGHT_MAX    0xffffffff
#define _BCM_SAT_GTF_RATE_PATN_INTERVAL_PACKET_WEIGHT_MIN    0

#define _BCM_SAT_CTF_TRAP_ID_MAX_NUM       3 
#define _BCM_SAT_CTF_ID_MIN                0
#define _BCM_SAT_CTF_ID_MAX                31
#define _BCM_SAT_CTF_OAM_ID_MIN            0
#define _BCM_SAT_CTF_OAM_ID_MAX            15
#define _BCM_SAT_CTF_TRAP_ID_MIN           0
#define _BCM_SAT_CTF_TRAP_ID_MAX           2
#define _BCM_SAT_CTF_COLOR_MIN             0
#define _BCM_SAT_CTF_COLOR_MAX             3
#define _BCM_SAT_CTF_COS_MIN               0
#define _BCM_SAT_CTF_COS_MAX               7
#define _BCM_SAT_CTF_TRAP_ID_VAL_MIN       0
#define _BCM_SAT_CTF_TRAP_ID_VAL_MAX       255
#define _BCM_SAT_CTF_TRAP_ID_UNSET_VAL     0x0
#define _BCM_SAT_CTF_BINS_LIMIT_CNT_MIN    1
#define _BCM_SAT_CTF_BINS_LIMIT_CNT_MAX    9
#define _BCM_SAT_CTF_BINS_LIMIT_SELECT_MAX 8
#define _BCM_SAT_CTF_SWITCH_STATE_NUM_OF_SLOTS_MIN   0
#define _BCM_SAT_CTF_SWITCH_STATE_NUM_OF_SLOTS_MAX   31
#define _BCM_SAT_CTF_SWITCH_STATE_THRES_PER_SLOT_MIN 0
#define _BCM_SAT_CTF_SWITCH_STATE_THRES_PER_SLOT_MAX 0x1FFFFFF
#define _BCM_SAT_CTF_REPORT_SAMPLING_PER_MAX         100
#define _BCM_SAT_CTF_TRAP_DATA_MIN                   0x0
#define _BCM_SAT_CTF_TRAP_DATA_MAX                   0x3FFF
#define _BCM_SAT_CTF_TRAP_DATA_MASK_MIN              0x0
#define _BCM_SAT_CTF_TRAP_DATA_MASK_MAX              0x3FFF
#define _BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM             16
#define _BCM_SAT_CTF_TRAP_DATA_SSID_MAX              0xF


typedef struct _bcm_sat_ctf_trap_data_s {
    uint16 trap_data;
    uint16 trap_mask;
    uint8  ssid;
}_bcm_sat_ctf_trap_data_t;

typedef struct _bcm_sat_data_s {
    uint32        ctf_id_bitmap;
    uint32        gtf_id_bitmap;
    uint32        gtf_packet_bitmap;
    uint32        ctf_trap_id[_BCM_SAT_CTF_TRAP_ID_MAX_NUM];
    uint32        ctf_trap_bitmap;
    uint32        ctf_ssid_bitmap;
    _bcm_sat_ctf_trap_data_t trap_data[_BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM];
    sal_mutex_t   sat_mutex; /* SAT mutex */
    uint8         rate_mode; /*1: bps 2:pps*/
    uint8         granularity_flag; /*flag for granularity */
    uint8         is_dirty;
    uint8         is_initialized;
} _bcm_sat_data_t;


_bcm_sat_data_t sat_data[BCM_UNITS_MAX] = {{0}};

/* Callbacks are not supported by warmboot. Need re-register after warmboot */
#ifdef BCM_SAT_SUPPORT
static bcm_sat_event_cb _bcm_sat_event_cb[BCM_UNITS_MAX][bcmSATEventCount];
static void *_bcm_sat_event_ud[BCM_UNITS_MAX][bcmSATEventCount];
#endif /* BCM_SAT_SUPPORT */

#define SAT_NULL_CHECK(p, para_name) do {\
    if (p == NULL) { \
        LOG_ERROR(BSL_LS_BCM_SAT, \
                  (BSL_META_U(unit, \
                              "Fail(%s) parameter(%s) is NULL\n"), \
                               soc_errmsg(BCM_E_PARAM), para_name)); \
        return BCM_E_PARAM; \
    }} while(0)

#define SAT_VALUE_CHECK(val, min, max, para_name) do {\
    if ((val) < (min) || (val) >= (max)) { \
        LOG_ERROR(BSL_LS_BCM_SAT, \
                  (BSL_META_U(unit, \
                              "Fail(%s) parameter(%s=%d) should be in range of [%d, %d]\n"), \
                               soc_errmsg(BCM_E_PARAM), para_name, val, min, (max-1))); \
        return BCM_E_PARAM; \
    }} while(0)

#define SAT_VALUE_MAX_CHECK(val, max, para_name) do {\
    if ((val) >= (max)) { \
        LOG_ERROR(BSL_LS_BCM_SAT, \
                  (BSL_META_U(unit, \
                              "Fail(%s) parameter(%s=%d) should be less than %d\n"), \
                               soc_errmsg(BCM_E_PARAM), para_name, val, max)); \
        return BCM_E_PARAM; \
    }} while(0)


#define SAT_BITMAP_EXIST(bitmap, index) \
        ((bitmap) & (1U << (index)))
#define SAT_BITMAP_SET(bitmap, index)   \
        ((bitmap) |= (1U << (index)))
#define SAT_BITMAP_CLR(bitmap, index)     \
        ((bitmap) &= (~(1U << (index))))

/* GTF ID */
#define SAT_GTF_ID_EXIST(psat_data, gtf_id) \
        SAT_BITMAP_EXIST((psat_data)->gtf_id_bitmap, gtf_id)
#define SAT_GTF_ID_SET(psat_data, gtf_id)   \
        SAT_BITMAP_SET((psat_data)->gtf_id_bitmap, gtf_id)
#define SAT_GTF_ID_CLR(psat_data, gtf_id)     \
        SAT_BITMAP_CLR((psat_data)->gtf_id_bitmap, gtf_id)
#define SAT_GTF_ID_CLR_ALL(psat_data)    \
    ((psat_data)->gtf_id_bitmap = 0)

/* GTF packet config */
#define SAT_GTF_PACKET_EXIST(psat_data, gtf_id) \
            SAT_BITMAP_EXIST((psat_data)->gtf_packet_bitmap, gtf_id)
#define SAT_GTF_PACKET_SET(psat_data, gtf_id)   \
            SAT_BITMAP_SET((psat_data)->gtf_packet_bitmap, gtf_id)
#define SAT_GTF_PACKET_CLR(psat_data, gtf_id)     \
            SAT_BITMAP_CLR((psat_data)->gtf_packet_bitmap, gtf_id)
#define SAT_GTF_PACKET_CLR_ALL(psat_data)    \
        ((psat_data)->gtf_packet_bitmap = 0)


#define SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id) do {\
    if (!SAT_GTF_ID_EXIST(psat_data, gtf_id)) {\
        LOG_ERROR(BSL_LS_BCM_SAT, \
                  (BSL_META_U(unit, \
                              "Fail(%s) gtf_id(%d) does not exist\n"), \
                               soc_errmsg(BCM_E_NOT_FOUND), gtf_id)); \
        return BCM_E_NOT_FOUND; \
    }} while(0)
#define SAT_GTF_PACKET_NOT_EXIST_EXIT(psat_data, gtf_id) do {\
    if (!SAT_GTF_PACKET_EXIST(psat_data, gtf_id)) {\
        LOG_ERROR(BSL_LS_BCM_SAT, \
                  (BSL_META_U(unit, \
                              "Fail(%s) gtf_id(%d) packet config does not exist\n"), \
                               soc_errmsg(BCM_E_NOT_FOUND), gtf_id)); \
        return BCM_E_NOT_FOUND; \
    }} while(0)


/* CTF ID */
#define SAT_CTF_ID_EXIST(psat_data, ctf_id) \
        SAT_BITMAP_EXIST((psat_data)->ctf_id_bitmap, ctf_id)
#define SAT_CTF_ID_SET(psat_data, ctf_id)   \
        SAT_BITMAP_SET((psat_data)->ctf_id_bitmap, ctf_id)
#define SAT_CTF_ID_CLR(psat_data, ctf_id)     \
        SAT_BITMAP_CLR((psat_data)->ctf_id_bitmap, ctf_id)
#define SAT_CTF_ID_CLR_ALL(psat_data)    \
    ((psat_data)->ctf_id_bitmap = 0)
#define SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id) do {\
    if (!SAT_CTF_ID_EXIST(psat_data, ctf_id)) {\
        LOG_ERROR(BSL_LS_BCM_SAT, \
                  (BSL_META_U(unit, \
                              "Fail(%s) ctf_id(%d) does not exist\n"), \
                               soc_errmsg(BCM_E_NOT_FOUND), ctf_id)); \
        return BCM_E_NOT_FOUND; \
    }} while(0)

/* CTF TRAP ID */
#define SAT_CTF_TRAP_ID_EXIST(psat_data, trap_idx) \
        SAT_BITMAP_EXIST((psat_data)->ctf_trap_bitmap, trap_idx)
#define SAT_CTF_TRAP_ID_SET(psat_data, trap_idx, trap_id)   \
        (psat_data)->ctf_trap_id[trap_idx] = trap_id; \
        SAT_BITMAP_SET((psat_data)->ctf_trap_bitmap, trap_idx)
#define SAT_CTF_TRAP_ID_CLR(psat_data, trap_idx)     \
        (psat_data)->ctf_trap_id[trap_idx] = _BCM_SAT_CTF_TRAP_ID_UNSET_VAL; \
        SAT_BITMAP_CLR((psat_data)->ctf_trap_bitmap, trap_idx)
#define SAT_CTF_TRAP_ID_CLR_ALL(psat_data)  do {   \
    ((psat_data)->ctf_trap_bitmap = 0); \
    for (trap_idx = 0; trap_idx < _BCM_SAT_CTF_TRAP_ID_MAX_NUM; trap_idx++) { \
        (psat_data)->ctf_trap_id[trap_idx] = _BCM_SAT_CTF_TRAP_ID_UNSET_VAL; \
    }} while(0)

#define _BCM_SAT_SYSTEM_LOCK(unit)                                                \
          sal_mutex_take(sat_data[unit].sat_mutex, sal_mutex_FOREVER)
#define _BCM_SAT_SYSTEM_UNLOCK(unit) sal_mutex_give(sat_data[unit].sat_mutex)

#ifdef BCM_WARM_BOOT_SUPPORT
#define SAT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SAT_WB_CURRENT_VERSION            SAT_WB_VERSION_1_0
#ifdef BCM_SAT_SUPPORT
STATIC int bcm_common_sat_wb_init(int unit);
STATIC int _bcm_common_sat_trap_id_find (_bcm_sat_data_t* psat_data,uint32 trap_id, int32* trap_idx);
#endif /* BCM_SAT_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_SAT_SUPPORT
#define SAT_INIT_CHECK(unit) \
    do { \
        if (sat_data[unit].is_initialized == FALSE) { \
            LOG_ERROR(BSL_LS_BCM_SAT, \
                      (BSL_META_U(unit, \
                                  "Fail(%s), Please enable SAT feature in advance\n"), \
                                   soc_errmsg(BCM_E_INIT))); \
            return BCM_E_INIT; \
        } \
    } while(0)

static uint8 _bcm_common_sat_rate_mode_get(int unit) {
    return  sat_data[unit].rate_mode;
}
static uint8 _bcm_common_sat_rate_mode_set(int unit, uint8 rate_mode) {
    sat_data[unit].rate_mode = rate_mode;
    return BCM_E_NONE;
}
static uint8 _bcm_common_sat_granularity_flag_get(int unit) {
    return  sat_data[unit].granularity_flag;
}
static uint8 _bcm_common_sat_granularity_flag_set(int unit, int flag) {
    sat_data[unit].granularity_flag = flag;
    return BCM_E_NONE;
}

#endif /* BCM_SAT_SUPPORT */
/* Functions */

#if defined(BCM_SAT_SUPPORT) || defined(BCM_WARM_BOOT_SUPPORT)
static _bcm_sat_data_t* sat_data_get(int unit) {
#ifdef CRASH_RECOVERY_SUPPORT
    soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_sat);
#endif
    sat_data[unit].is_dirty = TRUE;
    return &(sat_data[unit]);
}

#endif

#ifdef BCM_WARM_BOOT_SUPPORT
static uint8 _bcm_common_sat_is_dirty(int unit) {
   return sat_data[unit].is_dirty;
}

static void _bcm_commot_sat_mark_not_dirty(int unit) {
   sat_data[unit].is_dirty = FALSE;

}
#endif

#ifdef BCM_SAT_SUPPORT

int bcm_common_sat_init(int unit)
{
    int rv = BCM_E_UNAVAIL;

    _bcm_sat_data_t *psat_data;
    soc_sat_init_t soc_sat_init;
    uint32 freq_hz = 0;
    int granularity = 1000;    /* 1kbits per sec */

    psat_data = sat_data_get(unit);
    sal_memset(psat_data, 0, sizeof(_bcm_sat_data_t));
#ifdef BCM_WARM_BOOT_SUPPORT
    rv = bcm_common_sat_wb_init(unit);
#endif
    psat_data->sat_mutex = sal_mutex_create("SAT LOCK");
    if(psat_data->sat_mutex == NULL) {
        BCM_FREE(psat_data->sat_mutex);
        LOG_ERROR(BSL_LS_BCM_SAT,
                  (BSL_META_U(unit,
                              "Fail to allocate memory for sat_mutex\n")));        
        return BCM_E_MEMORY;
    }
    psat_data->is_initialized = TRUE;

    rv = mbcm_sat_init(unit);
    if (rv != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_BCM_SAT,
                  (BSL_META_U(unit,
                              "soc_dpp_init error in mbcm_sat_init\n")));
        return rv;
    }
    if (!SOC_WARM_BOOT(unit)) {
        /* Time tickets */
        freq_hz = SOC_INFO(unit).frequency * 1000000;
#if defined(BCM_DPP_SUPPORT)
        if (SOC_IS_JERICHO(unit)) {
            freq_hz = arad_chip_kilo_ticks_per_sec_get(unit)*1000;
            granularity = 64000;    /* 64kbits per sec */
        }
#endif
        soc_sat_init.cmic_tod_mode = _BCM_SAT_CMIC_TOD_MODE;
        soc_sat_init.num_clks_sec = freq_hz;
        soc_sat_init.rate_num_clks_cycle = (freq_hz/granularity)*8;    /* 1kbits per sec(125 cycles per sec) */

        _BCM_SAT_SYSTEM_LOCK(unit);
    	rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_general_cfg_init, (unit, &soc_sat_init));
    	_BCM_SAT_SYSTEM_UNLOCK(unit);
    }
    return rv;
}

int bcm_common_sat_detach(int unit)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_sat_data_t *psat_data;

    psat_data = sat_data_get(unit);
    rv = BCM_E_NONE;
    sal_memset(psat_data, 0, sizeof(_bcm_sat_data_t));

    if (psat_data->sat_mutex != NULL) {
        sal_mutex_destroy(psat_data->sat_mutex);
		psat_data->sat_mutex = NULL;
    }

    return rv;
}
#endif /* BCM_SAT_SUPPORT */

/*
 * Function:
 *      bcm_sat_endpoint_info_t_init
 * Purpose:
 *      Initialize an SAT endpoint info structure
 * Parameters:
 *      endpoint_info - (OUT) Pointer to SAT endpoint info structure
 *                            to be initialized
 * Returns:
 *      NONE 
 * Notes:
 */

void bcm_sat_endpoint_info_t_init (bcm_sat_endpoint_info_t *endpoint_info)
{
    if (NULL != endpoint_info) {
        sal_memset(endpoint_info, 0, sizeof(bcm_sat_endpoint_info_t));
        endpoint_info->src_gport = BCM_GPORT_INVALID;
        endpoint_info->dest_gport = BCM_GPORT_INVALID;
    }
    return;
}

void bcm_sat_config_t_init(bcm_sat_config_t *conf)
{
    if (NULL != conf) {
        sal_memset(conf, 0, sizeof (*conf));
    }
    return;
}

int
bcm_common_sat_config_get(
    int unit,
    bcm_sat_config_t* conf)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_config_t soc_conf;
    SAT_INIT_CHECK(unit);

    SAT_NULL_CHECK(conf, "conf");
    sal_memset(&soc_conf, 0, sizeof(soc_sat_config_t));

    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_config_get, (unit, &soc_conf));
    _BCM_SAT_SYSTEM_UNLOCK(unit); 
    conf->config_flags = soc_conf.config_flags;
    conf->timestamp_format = soc_conf.timestamp_format;
#endif /* BCM_SAT_SUPPORT */

    return rv;
}

int
bcm_common_sat_config_set(
    int unit,
    bcm_sat_config_t* conf)

{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_config_t soc_conf;
    SAT_INIT_CHECK(unit);

    SAT_NULL_CHECK(conf, "conf");
    SAT_VALUE_CHECK(conf->timestamp_format, bcmSATTimestampFormatIEEE1588v1, (bcmSATTimestampFormatNTP+1), "timestamp_format");
    sal_memset(&soc_conf, 0, sizeof(soc_sat_config_t));
    soc_conf.config_flags = conf->config_flags;
    soc_conf.timestamp_format = conf->timestamp_format;
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_config_set, (unit, &soc_conf));
#ifdef BCM_SABER2_SUPPORT
     if (BCM_SUCCESS(rv) && SOC_IS_SABER2(unit)) {
         rv = bcm_sb2_sat_ts_format_set(unit, soc_conf.timestamp_format);
     }
#endif
    _BCM_SAT_SYSTEM_UNLOCK(unit);    
#endif /* BCM_SAT_SUPPORT */

    return rv;
}

#ifdef BCM_SAT_SUPPORT
STATIC int _bcm_common_sat_gtf_freed_id_find (
     _bcm_sat_data_t* psat_data,
     bcm_sat_gtf_t* gtf_id
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    for (idx = _BCM_SAT_GTF_ID_MIN; idx <= _BCM_SAT_GTF_ID_MAX; idx++) {
        if (!SAT_GTF_ID_EXIST(psat_data, idx)) {
            break;
        }
    }

    if (idx == (_BCM_SAT_GTF_ID_MAX + 1)) {
        rv = BCM_E_RESOURCE;
    } else {
        *gtf_id = idx;
    }

    return rv;
}
#endif

void bcm_sat_header_user_define_offsets_t_init(bcm_sat_header_user_define_offsets_t *offsets)
{
    if (NULL != offsets) {
        sal_memset(offsets, 0, sizeof (*offsets));
    }
    return;
}

void bcm_sat_payload_t_init(bcm_sat_payload_t *payload)
{
    if (NULL != payload) {
        sal_memset(payload, 0, sizeof (*payload));
    }
    return;
}

void bcm_sat_stamp_t_init(bcm_sat_stamp_t *stamp)
{
    if (NULL != stamp) {
        sal_memset(stamp, 0, sizeof (*stamp));
    }
    return;
}

void bcm_sat_gtf_packet_edit_t_init(bcm_sat_gtf_packet_edit_t *packet_edit)
{
    if (NULL != packet_edit) {
        sal_memset(packet_edit, 0, sizeof (*packet_edit));
    }
    return;
}

void bcm_sat_gtf_packet_config_t_init(bcm_sat_gtf_packet_config_t *pkt_cfg)
{
    if (NULL != pkt_cfg) {
        sal_memset(pkt_cfg, 0, sizeof (*pkt_cfg));
    }
    return;
}

void bcm_sat_gtf_bandwidth_t_init(bcm_sat_gtf_bandwidth_t *bw)
{
    if (NULL != bw) {
        sal_memset(bw, 0, sizeof (*bw));
    }
    return;
}

void bcm_sat_gtf_rate_pattern_t_init(bcm_sat_gtf_rate_pattern_t *rate_pattern)
{
    if (NULL != rate_pattern) {
        sal_memset(rate_pattern, 0, sizeof (*rate_pattern));
    }
    return;
}

int bcm_common_sat_gtf_create (
    int            unit,
    uint32         flags,
    bcm_sat_gtf_t *gtf_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    bcm_sat_gtf_t tmp_gtf_id = 0;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(gtf_id, "gtf_id");
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) flags(%d) gtf_id(%d)\n", BSL_FUNC, BSL_LINE, 
        unit, flags, (*gtf_id) ));

	rv = BCM_E_NONE;
    if (flags & BCM_SAT_GTF_WITH_ID) {
        SAT_VALUE_CHECK(*gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");
        if (SAT_GTF_ID_EXIST(psat_data, *gtf_id)) {
            rv = BCM_E_EXISTS;
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) gtf_id(%d) exists\n"), soc_errmsg(rv), (*gtf_id)));
            return rv;
        } else {
            tmp_gtf_id = *gtf_id;
        }
    } else {
        /* Find a freed gtf ID */
        rv = _bcm_common_sat_gtf_freed_id_find(psat_data, &tmp_gtf_id);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) to find a freed gtf ID\n"), soc_errmsg(rv)));
            return rv;
        }
    }

    *gtf_id = tmp_gtf_id;

    SAT_GTF_ID_SET(psat_data, tmp_gtf_id);

#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_destroy (
    int unit,
    bcm_sat_gtf_t gtf_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    int pkt_gen_en = 0;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d)\n", BSL_FUNC, BSL_LINE, unit, gtf_id ));
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");

    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    /* Disable Generate Packet */
    pkt_gen_en = 0;
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_packet_gen_set, (unit, gtf_id, _BCM_SAT_GTF_OBJ_COMMON, pkt_gen_en));
    _BCM_SAT_SYSTEM_UNLOCK(unit);

    SAT_GTF_ID_CLR(psat_data, gtf_id);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    if(psat_data->gtf_id_bitmap == 0){
        /* if no gtf,  init rate mode*/
        rv =_bcm_common_sat_rate_mode_set(unit, 0);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                    (BSL_META_U(unit,"Fail(%s) init rate mode fail\n"), soc_errmsg(rv)));
        }
        /* if no gtf,  init granularity flag*/
        rv =_bcm_common_sat_granularity_flag_set(unit,0);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                    (BSL_META_U(unit,"Fail(%s) init granularity fail\n"), soc_errmsg(rv)));
        }
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_destroy_all (
    int unit
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    bcm_sat_gtf_t gtf_id;
    int pkt_gen_en = 0;

    psat_data = sat_data_get(unit);
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d)\n", BSL_FUNC, BSL_LINE, unit));
    rv = BCM_E_NONE;
    
    _BCM_SAT_SYSTEM_LOCK(unit);
    for(gtf_id = _BCM_SAT_GTF_ID_MIN; gtf_id <= _BCM_SAT_GTF_ID_MAX; gtf_id++)
    {
        if (SAT_GTF_ID_EXIST(psat_data, gtf_id)) {
            /* Disable Generate Packet */
            rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_packet_gen_set, (unit, gtf_id, _BCM_SAT_GTF_OBJ_COMMON, pkt_gen_en));
        }
    }
    _BCM_SAT_SYSTEM_UNLOCK(unit);

    SAT_GTF_ID_CLR_ALL(psat_data);
    SAT_GTF_PACKET_CLR_ALL(psat_data);

    if (rv != BCM_E_NONE) {
        return rv;
    }

    rv =_bcm_common_sat_rate_mode_set(unit, 0);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_SAT,
                (BSL_META_U(unit,"Fail(%s) init rate mode fail\n"), soc_errmsg(rv)));
    }

    /* if no gtf,  init granularity flag*/
    rv =_bcm_common_sat_granularity_flag_set(unit,0);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_SAT,
                (BSL_META_U(unit,"Fail(%s) init granularity fail\n"), soc_errmsg(rv)));
    }

#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_inuse(int unit)
{
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    bcm_sat_gtf_t gtf_id;

    psat_data = sat_data_get(unit);
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d)\n", BSL_FUNC, BSL_LINE, unit));

    _BCM_SAT_SYSTEM_LOCK(unit);
    for(gtf_id = _BCM_SAT_GTF_ID_MIN; gtf_id <= _BCM_SAT_GTF_ID_MAX; gtf_id++)
    {
        if (SAT_GTF_ID_EXIST(psat_data, gtf_id)) {
            _BCM_SAT_SYSTEM_UNLOCK(unit);
            return TRUE;
        }
    }

    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return FALSE;
}


int
bcm_common_sat_gtf_traverse(
    int unit,
    bcm_sat_gtf_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    bcm_sat_gtf_t gtf_id;
    _bcm_sat_data_t *psat_data;

    psat_data = sat_data_get(unit);
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) \n", BSL_FUNC, BSL_LINE, unit ));
    SAT_NULL_CHECK(user_data, "user_data");

    for(gtf_id = _BCM_SAT_GTF_ID_MIN; gtf_id <= _BCM_SAT_GTF_ID_MAX; gtf_id++)
    {
        if (SAT_GTF_ID_EXIST(psat_data, gtf_id)) {
            /* Invoke user callback. */
            (*cb)(unit, gtf_id, user_data);
        }
    }

    rv = BCM_E_NONE;
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

/* 
 *  Dump data in buffer - For debug usage.
 */
void _sat_packet_config_dump(bcm_sat_gtf_packet_config_t *config) {
    int i = 0, j = 0, ele = 0;
    uint8 *buffer; 
    int len; 

    if (!LOG_CHECK(BSL_LS_BCM_SAT | BSL_VERBOSE)) {
        return;
    }
    if (config == NULL) {
        return;
    }

    cli_out("  header_type:%d\n", config->sat_header_type);
    cli_out("  header_info:\n");
    if ((config->header_info.pkt_data != NULL) && (config->header_info.pkt_data[0].data != NULL)) {
        buffer = config->header_info.pkt_data[0].data;
        len = config->header_info.pkt_data[0].len;
        cli_out("    len:%d\n", len);
        cli_out("    data:");
        ele = (len < 0)? 0 : len;
        ele = (ele > (_BCM_SAT_GTF_PKT_HDR_LEN_MAX + 1)) ? (_BCM_SAT_GTF_PKT_HDR_LEN_MAX + 1) : ele;
        for (i = 0; i < ele ; i++) {
            if ((i % 16) == 0) 
                cli_out("\n    ");
            else if ((i % 4) == 0)
                cli_out(" ");
            cli_out("%02x", buffer[i]);
        }
    }
    else {
        cli_out("Invalid");
    }
    cli_out("\n");
    cli_out("  payload_type:%d\n", config->payload.payload_type);
    cli_out("  payload_pattern:");
    ele = (config->payload.payload_type == bcmSatPayloadConstant8Bytes) ? 8 : 4;
    if (config->payload.payload_type != bcmSatPayloadPRBS) {
        for (i = 0; i < ele; i++) {
            cli_out("%02x ", config->payload.payload_pattern[i]);
        }
    }
    cli_out("\n");
    for (i = 0; i < BCM_SAT_GTF_NUM_OF_PRIORITIES; i++) {
        cli_out("  %s packet_edit:\n", ((i == 0)?"cir":"eir"));
        cli_out("    packet_length:");
        for (j = 0; j < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; j++) {
            cli_out("%d ", config->packet_edit[i].packet_length[j]);
        }
        cli_out("\n");
        cli_out("    packet_length_pattern:");
        for (j = 0; j < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; j++) {
            cli_out("%d ", config->packet_edit[i].packet_length_pattern[j]);
        }
        cli_out("\n");
        cli_out("    pattern_length:%d\n", config->packet_edit[i].pattern_length);
        cli_out("    number_of_stamps:%d\n", config->packet_edit[i].number_of_stamps);
        for (j = 0; j < config->packet_edit[i].number_of_stamps; j++) {
            cli_out("    stamp(%d):  stamp_type:%d  field_type:%d  inc_step:%d  inc_period_packets:%d  value:%d  offset:%d\n", 
                j, 
                config->packet_edit[i].stamps[j].stamp_type, 
                config->packet_edit[i].stamps[j].field_type, 
                config->packet_edit[i].stamps[j].inc_step, 
                config->packet_edit[i].stamps[j].inc_period_packets, 
                config->packet_edit[i].stamps[j].value, 
                config->packet_edit[i].stamps[j].offset);
        }
        cli_out("    number_of_ctfs:%d\n", config->packet_edit[i].number_of_ctfs);
        cli_out("    flags:%d\n", config->packet_edit[i].flags);
    }
    cli_out("  packet_context_id:%d\n", config->packet_context_id);
    cli_out("  offsets:\n");
    cli_out("    seq_number_offset:%d\n", config->offsets.seq_number_offset);
    cli_out("    timestamp_offset:%d\n", config->offsets.timestamp_offset);
}

int bcm_common_sat_gtf_packet_config (
    int unit,
    bcm_sat_gtf_t gtf_id, 
    bcm_sat_gtf_packet_config_t *config
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_packet_config_t *soc_pkt_cfg;
    int i;
    int priority;
    int stamp_count[bcmSatStampCount] = {0};
    uint8 pkt_data[128];
    int pkt_len_idx = 0;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(config, "config");
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d)\n", BSL_FUNC, BSL_LINE, unit, gtf_id ));
    _sat_packet_config_dump(config);
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(config->sat_header_type, bcmSatHeaderUserDefined, bcmSatHeadersCount, "sat_header_type");
    SAT_NULL_CHECK(config->header_info.pkt_data, "config->header_info.pkt_data");
    SAT_VALUE_CHECK(config->header_info.pkt_data[0].len, _BCM_SAT_GTF_PKT_HDR_LEN_MIN, (_BCM_SAT_GTF_PKT_HDR_LEN_MAX+1), "header_info.pkt_data[0].len");
    SAT_VALUE_CHECK(config->payload.payload_type, bcmSatPayloadConstant8Bytes, bcmSatPayloadsCount, "payload_type");
    SAT_VALUE_CHECK(config->offsets.seq_number_offset, _BCM_SAT_SEQ_NUM_OFFSET_MIN, (_BCM_SAT_SEQ_NUM_OFFSET_MAX+1), "seq_number_offset");
    SAT_VALUE_CHECK(config->offsets.crc_byte_offset, _BCM_SAT_CRC_BYTE_OFFSET_MIN, (_BCM_SAT_CRC_BYTE_OFFSET_MAX+1), "crc_offset");
    for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++) {

        SAT_VALUE_CHECK(config->packet_edit[priority].pattern_length, 1, (BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH+1), "pattern_length");
        for (i = 0; i < config->packet_edit[priority].pattern_length; i++) {
            SAT_VALUE_MAX_CHECK(config->packet_edit[priority].packet_length_pattern[i], (_BCM_SAT_GTF_PKT_LEN_PATN_INX_MAX+1), "packet_length_pattern");
            pkt_len_idx = config->packet_edit[priority].packet_length_pattern[i];
            if ((config->packet_edit[priority].packet_length[pkt_len_idx]) <= (config->header_info.pkt_data[0].len) ||
                (config->packet_edit[priority].packet_length[pkt_len_idx]) > _BCM_SAT_PAYLOAD_OFFSET_MAX) {
                LOG_ERROR(BSL_LS_BCM_SAT,
                          (BSL_META_U(unit,
                                      "Fail(%s) packet_length(%d) should be greater than packet header length(%d) and less then max packet_length(%d)\n"),
                                      soc_errmsg(BCM_E_PARAM), config->packet_edit[priority].packet_length[pkt_len_idx], config->header_info.pkt_data[0].len, _BCM_SAT_PAYLOAD_OFFSET_MAX));
                return BCM_E_PARAM;
            }
        }
        SAT_VALUE_MAX_CHECK(config->packet_edit[priority].number_of_stamps, (BCM_SAT_GTF_MAX_STAMPS+1), "number_of_stamps");
        SAT_VALUE_CHECK(config->packet_edit[priority].number_of_ctfs, _BCM_SAT_GTF_SEQ_PERIOD_MIN, (_BCM_SAT_GTF_SEQ_PERIOD_MAX+1), "number_of_ctfs");

        stamp_count[bcmSatStampConstant2Bit] = stamp_count[bcmSatStampCounter8Bit] = stamp_count[bcmSatStampCounter16Bit] = 0;
        for (i = 0; i < BCM_SAT_GTF_MAX_STAMPS; i++) {
            SAT_VALUE_CHECK(config->packet_edit[priority].stamps[i].stamp_type, bcmSatStampInvalid, bcmSatStampCount, "stamp_type");
            SAT_VALUE_CHECK(config->packet_edit[priority].stamps[i].field_type, bcmSatStampFieldUserDefined, bcmSatStampFieldsCount, "stamp_field_type");

            if ((config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter8Bit) || 
                (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter16Bit)) {
                SAT_VALUE_CHECK(config->packet_edit[priority].stamps[i].inc_step, _BCM_SAT_GTF_STAMP_INC_STEP_MIN, _BCM_SAT_GTF_STAMP_INC_STEP_MAX+1, "stamp_inc_step");
                SAT_VALUE_MAX_CHECK(config->packet_edit[priority].stamps[i].inc_period_packets, (_BCM_SAT_GTF_STAMP_INC_PERIOD_MAX+1), "stamp_inc_period_packets");
            }
            else if (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampConstant2Bit) {
                SAT_VALUE_MAX_CHECK(config->packet_edit[priority].stamps[i].value, (_BCM_SAT_GTF_STAMP_2_bit_VALUE_MAX+1), "stamp_value");
            }

            stamp_count[config->packet_edit[priority].stamps[i].stamp_type]++;
        }

        if ((stamp_count[bcmSatStampConstant2Bit] > 1) || 
            (stamp_count[bcmSatStampCounter8Bit] > 1) ||
            (stamp_count[bcmSatStampCounter16Bit] > 1)) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) a CIR/EIR can have only one 2bit stamp, one 8bit counter and one 16bit counter\n"), soc_errmsg(BCM_E_PARAM)));
            return BCM_E_PARAM;
        }
    }
    
#if defined(BCM_DPP_SUPPORT)
    SAT_VALUE_MAX_CHECK(config->packet_context_id, (MAX_NUM_OF_CORES), "packet_context_id");
#endif

    soc_pkt_cfg = sal_alloc(sizeof(soc_sat_gtf_packet_config_t),
                            "soc_sat_gtf_packet_config");
    if (!soc_pkt_cfg) {
        return BCM_E_MEMORY;
    }

    sal_memset(soc_pkt_cfg, 0, sizeof(soc_sat_gtf_packet_config_t));
    /* Don't allow to configure gtf packet during packet generate enable */
    soc_pkt_cfg->sat_header_type = config->sat_header_type;
    soc_pkt_cfg->header_info.pkt_data = &(soc_pkt_cfg->header_info._pkt_data);
    soc_pkt_cfg->header_info.blk_count = 1;
	soc_pkt_cfg->header_info.pkt_data[0].data = pkt_data;
    soc_pkt_cfg->header_info.pkt_data[0].len = config->header_info.pkt_data[0].len;
    sal_memcpy(soc_pkt_cfg->header_info.pkt_data[0].data, config->header_info.pkt_data[0].data, sizeof(uint8)*config->header_info.pkt_data[0].len);    
    soc_pkt_cfg->payload.payload_type = config->payload.payload_type;
    sal_memcpy(soc_pkt_cfg->payload.payload_pattern, config->payload.payload_pattern, sizeof(uint8)*BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE);    
    
    for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++) {
        sal_memcpy(soc_pkt_cfg->packet_edit[priority].packet_length, config->packet_edit[priority].packet_length, sizeof(uint32)*BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS); 
        sal_memcpy(soc_pkt_cfg->packet_edit[priority].packet_length_pattern, config->packet_edit[priority].packet_length_pattern, sizeof(uint32)*BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH); 
        soc_pkt_cfg->packet_edit[priority].pattern_length = config->packet_edit[priority].pattern_length;

        soc_pkt_cfg->packet_edit[priority].number_of_stamps = config->packet_edit[priority].number_of_stamps;
        for (i = 0; i < config->packet_edit[priority].number_of_stamps; i++) {
            soc_pkt_cfg->packet_edit[priority].stamps[i].stamp_type = config->packet_edit[priority].stamps[i].stamp_type;
            soc_pkt_cfg->packet_edit[priority].stamps[i].field_type = config->packet_edit[priority].stamps[i].field_type;
            if ((config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter8Bit) || 
                (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter16Bit)) {
                soc_pkt_cfg->packet_edit[priority].stamps[i].inc_step = config->packet_edit[priority].stamps[i].inc_step;
                soc_pkt_cfg->packet_edit[priority].stamps[i].inc_period_packets = config->packet_edit[priority].stamps[i].inc_period_packets;
            }
            else if (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampConstant2Bit) {
                soc_pkt_cfg->packet_edit[priority].stamps[i].value = config->packet_edit[priority].stamps[i].value;
            }  
            soc_pkt_cfg->packet_edit[priority].stamps[i].offset= config->packet_edit[priority].stamps[i].offset;
        }
        soc_pkt_cfg->packet_edit[priority].number_of_ctfs = config->packet_edit[priority].number_of_ctfs;
        if(config->packet_edit[priority].flags & BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV) {
            soc_pkt_cfg->packet_edit[priority].flags |= SOC_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
        }
        if(config->packet_edit[priority].flags & BCM_SAT_GTF_PACKET_EDIT_ADD_CRC) {
            soc_pkt_cfg->packet_edit[priority].flags |= SOC_SAT_GTF_PACKET_EDIT_ADD_CRC;
        }
    }

    soc_pkt_cfg->packet_context_id = config->packet_context_id;
    soc_pkt_cfg->offsets.payload_offset = config->offsets.payload_offset;
    soc_pkt_cfg->offsets.timestamp_offset = config->offsets.timestamp_offset;
    soc_pkt_cfg->offsets.seq_number_offset = config->offsets.seq_number_offset;
    soc_pkt_cfg->offsets.crc_byte_offset = config->offsets.crc_byte_offset;
    
    _BCM_SAT_SYSTEM_LOCK(unit);
    rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_packet_config,
                              (unit, gtf_id, soc_pkt_cfg));
    _BCM_SAT_SYSTEM_UNLOCK(unit);

    SAT_GTF_PACKET_SET(psat_data, gtf_id);
    sal_free(soc_pkt_cfg);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_bandwidth_set (
    int unit, 
    bcm_sat_gtf_t gtf_id,	 
    int priority,
    bcm_sat_gtf_bandwidth_t *bw
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_bandwidth_t soc_bandwidth;
    uint8         rate_mode = 0; /*1: bps 2:pps*/
    uint8         is_granularity_set = 1;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(bw, "bw");
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) priority(%d) rate(%d) max_burst(%d)\n", BSL_FUNC, BSL_LINE, 
        unit, gtf_id, priority, bw->rate, bw->max_burst));

    if (SOC_IS_QUX(unit)) {
        SAT_VALUE_MAX_CHECK(bw->rate, (_BCM_SAT_GTF_BW_RATE_MAX_QUX+1), "rate");
    } else if (SOC_IS_QAX(unit)) {
        SAT_VALUE_MAX_CHECK(bw->rate, (_BCM_SAT_GTF_BW_RATE_MAX_QAX+1), "rate");
    } else {
        SAT_VALUE_MAX_CHECK(bw->rate, (_BCM_SAT_GTF_BW_RATE_MAX+1), "rate");
    }
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_COMMON, (_BCM_SAT_GTF_OBJ_EIR+1), "priority");       
    SAT_VALUE_MAX_CHECK(bw->max_burst, (_BCM_SAT_GTF_BW_BURST_MAX+1), "max_burst");

    if (SOC_IS_QAX(unit)) {
        if(bw->flags & BCM_SAT_GTF_RATE_WITH_GRANULARITY){
            if(!(bw->flags & BCM_SAT_GTF_RATE_IN_PACKETS)){
                LOG_ERROR(BSL_LS_BCM_SAT,(BSL_META_U(unit,"Fail(%s) granularity only for PPS \n"), soc_errmsg(BCM_E_PARAM)));
                return BCM_E_PARAM;
            }
            /* checking granularity value*/
            if(bw->granularity == 0){
                LOG_ERROR(BSL_LS_BCM_SAT,(BSL_META_U(unit,"Fail(%s) granularity shouldn't be zero \n"), soc_errmsg(BCM_E_PARAM)));
                return BCM_E_PARAM;
            }
            /* checking if granularity is set. Granularity is shared by all GTF*/
            if(is_granularity_set == _bcm_common_sat_granularity_flag_get(unit)){
                LOG_WARN(BSL_LS_BCM_SAT,(BSL_META_U(unit,
                    "Warning :  Granularity is shared by all GTFs, which will influce all the GTF  granularity\n")));
            }
            /* setting  granularity flag*/
            rv =_bcm_common_sat_granularity_flag_set(unit,1);
            if (rv != BCM_E_NONE) {
                return rv;
            }
        }

        if(bw->flags & BCM_SAT_GTF_RATE_IN_PACKETS){
            rate_mode = _BCM_SAT_PPS_MODE;
        }
        else{
            rate_mode = _BCM_SAT_BPS_MODE;
        }

        /* fisrt GTF will decide the "global mode" for BPS/PPS */
        if(0 == _bcm_common_sat_rate_mode_get(unit)){
            rv =_bcm_common_sat_rate_mode_set(unit, rate_mode);
            if (rv != BCM_E_NONE) {
                return rv;
            }
            if(rate_mode == _BCM_SAT_PPS_MODE){
                /* Default set granularity flag in PPS mode, default granularity is 1 packet/s*/
                rv =_bcm_common_sat_granularity_flag_set(unit,1);
                if (rv != BCM_E_NONE) {
                    return rv;
                }
            }
        }
        else if(rate_mode != _bcm_common_sat_rate_mode_get(unit)){
            LOG_ERROR(BSL_LS_BCM_SAT,(BSL_META_U(unit,
                "Fail(%s)  BPS/PPS mode conflicts with global rate mode (which is decided by first GTF ), you should delete all GTFs before change it\n"), soc_errmsg(BCM_E_PARAM)));
                return BCM_E_PARAM;
        }
    }

    soc_bandwidth.rate = bw->rate;
    soc_bandwidth.max_burst = bw->max_burst;
    soc_bandwidth.flags = bw->flags;
    soc_bandwidth.granularity = bw->granularity;

    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_bandwidth_set, (unit, gtf_id, priority, &soc_bandwidth));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_bandwidth_get (
    int unit, 
    bcm_sat_gtf_t gtf_id,	 
    int priority,
    bcm_sat_gtf_bandwidth_t *bw
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_bandwidth_t soc_bandwidth;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(bw, "bw");
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_COMMON, (_BCM_SAT_GTF_OBJ_EIR+1), "priority");       
    
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_bandwidth_get, (unit, gtf_id, priority, &soc_bandwidth));
    _BCM_SAT_SYSTEM_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        bw->rate = soc_bandwidth.rate;
        bw->max_burst = soc_bandwidth.max_burst;
        bw->flags = soc_bandwidth.flags;
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_rate_pattern_set (
	int unit,
	bcm_sat_gtf_t gtf_id,
	int priority,
	bcm_sat_gtf_rate_pattern_t *config
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_rate_pattern_t soc_rate_pattern;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(config, "config");
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) priority(%d)\n" 
        "  rate_pattern_mode(%d)\n"
        "  high_threshold(%d) low_threshold(%d)\n"
        "  stop_iter_count(%d) stop_burst_count(%d) stop_interval_count(%d)\n"
        "  burst_packet_weight(%d) interval_packet_weight(%d) flags(%d)\n", BSL_FUNC, BSL_LINE, 
        unit, gtf_id, priority, 
        config->rate_pattern_mode, config->high_threshold, config->low_threshold, 
        config->stop_iter_count, config->stop_burst_count, config->stop_interval_count, 
        config->burst_packet_weight, config->interval_packet_weight, config->flags));
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_CIR, (_BCM_SAT_GTF_OBJ_EIR+1), "gtf_id");       

    SAT_VALUE_CHECK(config->high_threshold, _BCM_SAT_GTF_RATE_PATN_HIGH_TH_MIN, (_BCM_SAT_GTF_RATE_PATN_HIGH_TH_MAX+1), "high_threshold");
    SAT_VALUE_CHECK(config->low_threshold, _BCM_SAT_GTF_RATE_PATN_LOW_TH_MIN, (_BCM_SAT_GTF_RATE_PATN_LOW_TH_MAX+1), "low_threshold");
    SAT_VALUE_MAX_CHECK(config->stop_iter_count, (_BCM_SAT_GTF_RATE_PATN_STOP_ITER_MAX+1), "stop_iter_count");

    if (config->rate_pattern_mode == bcmSatGtfRatePatternContinues) {
        if (config->high_threshold != config->low_threshold) {
            rv = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in continues mode, high_threshold(%d) should equals to low_threshold(%d)\n"), 
                                  soc_errmsg(rv), config->high_threshold, config->low_threshold));
            return rv;
        }
    }
    else if (config->rate_pattern_mode == bcmSatGtfRatePatternSimpleBurst) {
        if (config->high_threshold <= config->low_threshold) {
            rv = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in simplebusrt mode, high_threshold(%d) should be greater than low_threshold(%d)\n"), 
                                  soc_errmsg(rv), config->high_threshold, config->low_threshold));
            return rv;
        }
    }
    else if (config->rate_pattern_mode == bcmSatGtfRatePatternCombined) {
        if (config->high_threshold < config->low_threshold) {
            rv = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in combined mode, high_threshold(%d) should be greater than low_threshold(%d)\n"), 
                                  soc_errmsg(rv), config->high_threshold, config->low_threshold));
            return rv;
        }
    }
    else if (config->rate_pattern_mode == bcmSatGtfRatePatternInterval) {
        if (config->high_threshold < config->low_threshold) {
            rv = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "Fail(%s) in interval mode, high_threshold(%d) should be greater than low_threshold(%d)\n"),
                                  soc_errmsg(rv), config->high_threshold, config->low_threshold));
            return rv;
        }
    }
    else {
        rv = BCM_E_PARAM;
        LOG_ERROR(BSL_LS_BCM_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) rate_pattern_mode(%d) is not supported\n"), 
                              soc_errmsg(rv), config->rate_pattern_mode));
        return rv;
    }
    
    sal_memset(&soc_rate_pattern, 0, sizeof(soc_rate_pattern));
    
    if(config->flags & BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST) {
        soc_rate_pattern.flags |= SOC_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST;
    }
    soc_rate_pattern.rate_pattern_mode = config->rate_pattern_mode;
    soc_rate_pattern.high_threshold = config->high_threshold;
    soc_rate_pattern.low_threshold = config->low_threshold;
    soc_rate_pattern.stop_iter_count = config->stop_iter_count;
    soc_rate_pattern.stop_burst_count = config->stop_burst_count;
    soc_rate_pattern.stop_interval_count = config->stop_interval_count;
    soc_rate_pattern.burst_packet_weight = config->burst_packet_weight;
    soc_rate_pattern.interval_packet_weight = config->interval_packet_weight;

    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_rate_pattern_set, (unit, gtf_id, priority, &soc_rate_pattern));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_rate_pattern_get (
	int unit,
	bcm_sat_gtf_t gtf_id,
	int priority,
	bcm_sat_gtf_rate_pattern_t *config
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_rate_pattern_t soc_rate_pattern;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(config, "config");
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_CIR, (_BCM_SAT_GTF_OBJ_EIR+1), "priority");       
    sal_memset(&soc_rate_pattern, 0, sizeof(soc_rate_pattern));

    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_rate_pattern_get, (unit, gtf_id, priority, &soc_rate_pattern));
    _BCM_SAT_SYSTEM_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        config->rate_pattern_mode = soc_rate_pattern.rate_pattern_mode;
        config->high_threshold = soc_rate_pattern.high_threshold;
        config->low_threshold = soc_rate_pattern.low_threshold;
        config->stop_iter_count = soc_rate_pattern.stop_iter_count;
        config->stop_burst_count = soc_rate_pattern.stop_burst_count;
        config->stop_interval_count = soc_rate_pattern.stop_interval_count;
        config->burst_packet_weight = soc_rate_pattern.burst_packet_weight;
        config->interval_packet_weight = soc_rate_pattern.interval_packet_weight;
        if(soc_rate_pattern.flags & BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST) {
            config->flags |= SOC_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST;
        }        
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_packet_start (
    int unit, 
    bcm_sat_gtf_t gtf_id,	 
    bcm_sat_gtf_pri_t priority
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    int pkt_gen_en = 0;
    SAT_INIT_CHECK(unit);
    
    psat_data = sat_data_get(unit);
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) priority(%d)\n", BSL_FUNC, BSL_LINE, 
        unit, gtf_id, priority));
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");
    SAT_VALUE_CHECK(priority, bcmSatGtfPriAll, (bcmSatGtfPriEir+1), "priority");
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);
    SAT_GTF_PACKET_NOT_EXIST_EXIT(psat_data, gtf_id);

    /* Enable Genrate Packet */
    pkt_gen_en = 1;
    _BCM_SAT_SYSTEM_LOCK(unit);
	rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_packet_gen_set, (unit, gtf_id, priority, pkt_gen_en));
    _BCM_SAT_SYSTEM_UNLOCK(unit);

#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_packet_stop (
    int unit, 
    bcm_sat_gtf_t gtf_id,	 
    bcm_sat_gtf_pri_t priority
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    int pkt_gen_en = 0;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d) gtf_id(%d) priority(%d)\n", BSL_FUNC, BSL_LINE, 
        unit, gtf_id, priority));
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");
    SAT_VALUE_CHECK(priority, bcmSatGtfPriAll, (bcmSatGtfPriEir+1), "priority");
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    /* Disable Genrate Packet */
    pkt_gen_en = 0;
    _BCM_SAT_SYSTEM_LOCK(unit);
	rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_packet_gen_set, (unit, gtf_id, priority, pkt_gen_en));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_stat_get (
	int unit,
	bcm_sat_gtf_t gtf_id,
	int priority,
	uint32 flags,
	bcm_sat_gtf_stat_counter_t type,
	uint64* value
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_gtf_stat_counter_t stat_type;
    uint64 stat_cur;
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

#ifdef CRASH_RECOVERY_SUPPORT
    /* the problem is that the sat_data[unit] is being reset in the wb/cr reset.
       After doing bcm_common_sat_ctf_create/bcm_common_sat_gtf_create the sat_data
       is updated (ctf_id_bitmap/gtf_id_bitmap), but after the reset it is being zero. */
    soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_sat);
#endif

    SAT_NULL_CHECK(value, "value");
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1), "gtf_id");

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_CIR, (_BCM_SAT_GTF_OBJ_EIR+1), "priority");  
    SAT_VALUE_CHECK(type, bcmSatGtfStatPacketCount, (bcmSatGtfStatCount), "type");
    /* parameter check */

    psat_data = sat_data_get(unit);
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);
    if (flags != 0) {
        rv = BCM_E_PARAM;
        LOG_ERROR(BSL_LS_BCM_SAT,
                  (BSL_META_U(unit,
                              "Fail(%s) flags(%d) should be 0\n"), 
                              soc_errmsg(rv), flags));
        return rv;
    }

    stat_type = type;
    COMPILER_64_ZERO(stat_cur);

    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_stat_get, (unit, gtf_id, priority, flags, stat_type, &stat_cur));
    _BCM_SAT_SYSTEM_UNLOCK(unit);

    COMPILER_64_ZERO(*value);
    COMPILER_64_ADD_64(*value, stat_cur);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

/* CTF functions */
void bcm_sat_ctf_packet_info_t_init(bcm_sat_ctf_packet_info_t *packet_info)
{
    if (NULL != packet_info) {
        sal_memset(packet_info, 0, sizeof (*packet_info));
    }
    return;
}

void bcm_sat_ctf_identifier_t_init(bcm_sat_ctf_identifier_t *ctf_identifier)
{
    if (NULL != ctf_identifier) {
        sal_memset(ctf_identifier, 0, sizeof (*ctf_identifier));
    }
    return;
}

void bcm_sat_ctf_bin_limit_t_init(bcm_sat_ctf_bin_limit_t *bins)
{
    if (NULL != bins) {
        sal_memset(bins, 0, sizeof (*bins));
    }
    return;
}

void bcm_sat_ctf_stat_config_t_init(bcm_sat_ctf_stat_config_t *stat_cfg)
{
    if (NULL != stat_cfg) {
        sal_memset(stat_cfg, 0, sizeof (*stat_cfg));
    }
    return;
}

void bcm_sat_ctf_stat_t_init(bcm_sat_ctf_stat_t *stat)
{
    if (NULL != stat) {
        sal_memset(stat, 0, sizeof (*stat));
    }
    return;
}

void bcm_sat_ctf_availability_config_t_init(bcm_sat_ctf_availability_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof (*config));
    }
    return;
}

void bcm_sat_ctf_report_config_t_init(bcm_sat_ctf_report_config_t *reports)
{
    if (NULL != reports) {
        sal_memset(reports, 0, sizeof (*reports));
    }
    return;
}

#ifdef BCM_SAT_SUPPORT
STATIC int _bcm_common_sat_ctf_freed_id_find (
     _bcm_sat_data_t* psat_data,
     bcm_sat_ctf_t* ctf_id
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    for (idx = _BCM_SAT_CTF_ID_MIN; idx <= _BCM_SAT_CTF_ID_MAX; idx++) {
        if (!SAT_CTF_ID_EXIST(psat_data, idx)) {
            break;
        }
    }

    if (idx == (_BCM_SAT_CTF_ID_MAX + 1)) {
        rv = BCM_E_RESOURCE;
    } else {
        *ctf_id = idx;
    }

    return rv;
}

STATIC int _bcm_common_sat_ctf_freed_trap_id_find (
     _bcm_sat_data_t* psat_data,
     int32* trap_idx
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    for (idx = 0; idx < _BCM_SAT_CTF_TRAP_ID_MAX_NUM; idx++) {
        if (!SAT_CTF_TRAP_ID_EXIST(psat_data, idx)) {
            break;
        }
    }

    if (idx == _BCM_SAT_CTF_TRAP_ID_MAX_NUM) {
        rv = BCM_E_RESOURCE;
    } else {
        *trap_idx = idx;
    }

    return rv;
}

STATIC int _bcm_common_sat_trap_id_find (
     _bcm_sat_data_t* psat_data,
     uint32 trap_id,
     int32* trap_idx
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    for (idx = 0; idx < _BCM_SAT_CTF_TRAP_ID_MAX_NUM; idx++) {
        if (trap_id == psat_data->ctf_trap_id[idx] && SAT_CTF_TRAP_ID_EXIST(psat_data, idx)) {
            break;
        }
    }

    if (idx == _BCM_SAT_CTF_TRAP_ID_MAX_NUM) {
        rv = BCM_E_NOT_FOUND;
    } else {
        *trap_idx = idx;
    }

    return rv;
}

STATIC int _bcm_common_sat_ctf_trap_entry_find (
     _bcm_sat_data_t* psat_data,
     uint32  trap_data,
     uint32  trap_mask,
     uint32  *entry_idx
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;
    uint8 found = FALSE;

    /* Check the entry whether is existed */
    for (idx = 0; idx < _BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM; idx++) {
        if (psat_data->trap_data[idx].trap_data == trap_data &&
            psat_data->trap_data[idx].trap_mask == trap_mask &&
            SAT_BITMAP_EXIST(psat_data->ctf_ssid_bitmap, idx)) {
            found = TRUE;
            break;
        }
    }

    if (found == TRUE) {
        *entry_idx = idx;
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}


STATIC int _bcm_common_sat_ctf_freed_trap_entry_find (
     _bcm_sat_data_t* psat_data,
     uint32  trap_data,
     uint32  trap_mask,
     uint32  *entry_idx
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    /* Check the entry whether is existed */
    rv = _bcm_common_sat_ctf_trap_entry_find(psat_data, trap_data, trap_mask, entry_idx);
    if (rv == BCM_E_NONE) {
        *entry_idx = idx;
        rv = BCM_E_EXISTS;
    } else {
        /* Find a freed entry */
        rv = BCM_E_NONE;
        for (idx = 0; idx < _BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM; idx++) {
            if (!SAT_BITMAP_EXIST(psat_data->ctf_ssid_bitmap, idx)) {
                break;
            }
        }

        if (idx == _BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM) {
            rv = BCM_E_RESOURCE;
        } else {
            *entry_idx = idx;
        }
    }

    return rv;
}
#endif

int bcm_common_sat_ctf_create (
     int unit,
     uint32 flags,
     bcm_sat_ctf_t *ctf_id
    )
{
    int rv = BCM_E_UNAVAIL;
 #ifdef BCM_SAT_SUPPORT   
    soc_sat_ctf_report_config_t soc_reports;
    _bcm_sat_data_t *psat_data;
    bcm_sat_ctf_t tmp_ctf_id = 0;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(ctf_id, "ctf_id");
    rv = BCM_E_NONE;
    if (flags & BCM_SAT_CTF_WITH_ID) {
        /* In case return BCM_E_PARAM if ctf_id is invalid */
        SAT_VALUE_CHECK(*ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1), "ctf_id");
        if (SAT_CTF_ID_EXIST(psat_data, *ctf_id)) {
            return BCM_E_EXISTS;
        } else {
            tmp_ctf_id = *ctf_id;
        }
    } else {
        /* Find a freed CTF ID */
        /* In case return BCM_E_RESOURCE if haven't resource.
         */
        rv = _bcm_common_sat_ctf_freed_id_find(psat_data, &tmp_ctf_id);
        if (rv != BCM_E_NONE) {
            return rv;
        }
        *ctf_id = tmp_ctf_id;
    }
    SAT_CTF_ID_SET(psat_data, tmp_ctf_id);

    /*Do not generate cpu report by default.
     */
    sal_memset(&soc_reports, 0, sizeof(bcm_sat_ctf_report_config_t));
    soc_reports.report_sampling_percent = 0;
    _BCM_SAT_SYSTEM_LOCK(unit);
    rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_reports_config_set, (unit, *ctf_id, &soc_reports));
    _BCM_SAT_SYSTEM_UNLOCK(unit);

#endif /* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_destroy (
    int unit,
    bcm_sat_ctf_t ctf_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    rv = BCM_E_NONE;
    psat_data = sat_data_get(unit);
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1), "ctf_id");
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    SAT_CTF_ID_CLR(psat_data, ctf_id);
#endif /* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_destroy_all (
    int unit
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    rv = BCM_E_NONE;
    psat_data = sat_data_get(unit);
    SAT_CTF_ID_CLR_ALL(psat_data);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_inuse(int unit)
{
#ifdef BCM_SAT_SUPPORT
    bcm_sat_ctf_t ctf_id;
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    LOG_VERBOSE(BSL_LS_BCM_SAT, ("%s %d: u(%d)\n", BSL_FUNC, BSL_LINE, unit));

    _BCM_SAT_SYSTEM_LOCK(unit);
    for (ctf_id = _BCM_SAT_CTF_ID_MIN;
            ctf_id <= _BCM_SAT_CTF_ID_MAX; ctf_id++) {
        if (SAT_CTF_ID_EXIST(psat_data, ctf_id)) {
            _BCM_SAT_SYSTEM_UNLOCK(unit);
            return TRUE;
        }
    }
    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return FALSE;
}

int
bcm_common_sat_ctf_traverse(
    int unit,
    bcm_sat_ctf_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    bcm_sat_ctf_t ctf_id;
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    rv = BCM_E_NONE;
    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(user_data, "user_data");
    for(ctf_id = _BCM_SAT_CTF_ID_MIN; ctf_id <= _BCM_SAT_CTF_ID_MAX; ctf_id++)
    {
        if (SAT_CTF_ID_EXIST(psat_data, ctf_id)) {
            /* Invoke user callback. */
            (*cb)(unit, ctf_id, user_data);
        }
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_packet_config (
	int unit,
	bcm_sat_ctf_t ctf_id,
	bcm_sat_ctf_packet_info_t *packet_info
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_packet_info_t soc_packet_info;
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(packet_info, "packet_info");
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1), "ctf_id");
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    SAT_VALUE_CHECK(packet_info->sat_header_type, bcmSatHeaderUserDefined, bcmSatHeadersCount, "sat_header_type");
    SAT_VALUE_CHECK(packet_info->payload.payload_type, bcmSatPayloadConstant8Bytes, bcmSatPayloadsCount, "payload_type");
    SAT_VALUE_CHECK(packet_info->offsets.payload_offset, _BCM_SAT_PAYLOAD_OFFSET_MIN, (_BCM_SAT_PAYLOAD_OFFSET_MAX+1), "payload_offset");
    SAT_VALUE_CHECK(packet_info->offsets.seq_number_offset, _BCM_SAT_SEQ_NUM_OFFSET_MIN, (_BCM_SAT_SEQ_NUM_OFFSET_MAX+1), "seq_number_offset");
    SAT_VALUE_CHECK(packet_info->offsets.timestamp_offset, _BCM_SAT_TIME_STAMP_OFFSET_MIN, (_BCM_SAT_TIME_STAMP_OFFSET_MAX+1), "timestamp_offset");
    if(packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC) {
        SAT_VALUE_CHECK(packet_info->offsets.crc_byte_offset, _BCM_SAT_CRC_BYTE_OFFSET_MIN, (_BCM_SAT_CRC_BYTE_OFFSET_MAX+1), "crc_byte_offset");
    }
    sal_memset(&soc_packet_info, 0, sizeof(soc_packet_info));

    if(packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV) {
        soc_packet_info.flags |= SOC_SAT_CTF_PACKET_INFO_ADD_END_TLV;
    }
    if(packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC) {
        soc_packet_info.flags |= SOC_SAT_CTF_PACKET_INFO_ADD_CRC;
    }
    soc_packet_info.sat_header_type = packet_info->sat_header_type;
    soc_packet_info.payload.payload_type = packet_info->payload.payload_type;
    sal_memcpy(soc_packet_info.payload.payload_pattern, packet_info->payload.payload_pattern, sizeof(uint8)*BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE);
    if (packet_info->sat_header_type == bcmSatHeaderUserDefined) {
        soc_packet_info.offsets.payload_offset    = packet_info->offsets.payload_offset;
        soc_packet_info.offsets.seq_number_offset = packet_info->offsets.seq_number_offset;    
        soc_packet_info.offsets.timestamp_offset  = packet_info->offsets.timestamp_offset;
        if (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC) {
            soc_packet_info.offsets.crc_byte_offset  = packet_info->offsets.crc_byte_offset;
        }
    }
    else if (packet_info->sat_header_type == bcmSatHeaderY1731) {
        soc_packet_info.offsets.payload_offset    = 
            (packet_info->offsets.payload_offset ? packet_info->offsets.payload_offset : _BCM_SAT_Y1731_PAYLOAD_OFFSET);
        soc_packet_info.offsets.seq_number_offset = 
            (packet_info->offsets.seq_number_offset ? packet_info->offsets.seq_number_offset : _BCM_SAT_Y1731_SEQ_NUM_OFFSET);
        soc_packet_info.offsets.timestamp_offset  = 
            (packet_info->offsets.timestamp_offset ? packet_info->offsets.timestamp_offset : _BCM_SAT_Y1731_TIME_STAMP_OFFSET);
        if (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC) {
            soc_packet_info.offsets.crc_byte_offset  = (packet_info->offsets.crc_byte_offset ?
                packet_info->offsets.crc_byte_offset : _BCM_SAT_Y1731_CRC_BYTE_OFFSET_OFFSET);
        }
    }
    else if (packet_info->sat_header_type == bcmSatHeaderMEF) {
        soc_packet_info.offsets.payload_offset    = 
            (packet_info->offsets.payload_offset ? packet_info->offsets.payload_offset : _BCM_SAT_MEF_PAYLOAD_OFFSET);
        soc_packet_info.offsets.seq_number_offset = 
            (packet_info->offsets.seq_number_offset ? packet_info->offsets.seq_number_offset : _BCM_SAT_MEF_SEQ_NUM_OFFSET);
        soc_packet_info.offsets.timestamp_offset  = 
            (packet_info->offsets.timestamp_offset ? packet_info->offsets.timestamp_offset : _BCM_SAT_MEF_TIME_STAMP_OFFSET);
        if (packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC) {
            soc_packet_info.offsets.crc_byte_offset  = (packet_info->offsets.crc_byte_offset ?
                packet_info->offsets.crc_byte_offset : _BCM_SAT_MEF_CRC_BYTE_OFFSET_OFFSET);
        }
    }
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_packet_config, (unit, ctf_id, &soc_packet_info));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_identifier_map (
	int unit,
	bcm_sat_ctf_identifier_t *identifier,
	bcm_sat_ctf_t ctf_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_identifier_t soc_identifier;
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(identifier, "identifier");
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1), "ctf_id");
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    SAT_VALUE_MAX_CHECK(identifier->session_id, (_BCM_SAT_CTF_OAM_ID_MAX+1), "session_id");
    SAT_VALUE_MAX_CHECK(identifier->trap_id, (_BCM_SAT_CTF_TRAP_ID_MAX+1), "trap_id");
    SAT_VALUE_MAX_CHECK(identifier->color, (_BCM_SAT_CTF_COLOR_MAX+1), "color");
    SAT_VALUE_MAX_CHECK(identifier->tc, (_BCM_SAT_CTF_COS_MAX+1), "tc");
    sal_memset(&soc_identifier, 0, sizeof(soc_identifier));
    soc_identifier.session_id = identifier->session_id;
    soc_identifier.trap_id = identifier->trap_id;
    soc_identifier.color = identifier->color;
    soc_identifier.tc = identifier->tc;
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_identifier_map, (unit, &soc_identifier, ctf_id));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_identifier_unmap (
	int unit,
	bcm_sat_ctf_identifier_t *identifier
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_identifier_t soc_identifier;
    SAT_INIT_CHECK(unit);

    SAT_NULL_CHECK(identifier, "identifier");
    SAT_VALUE_MAX_CHECK(identifier->session_id, (_BCM_SAT_CTF_OAM_ID_MAX+1), "session_id");
    SAT_VALUE_MAX_CHECK(identifier->trap_id, (_BCM_SAT_CTF_TRAP_ID_MAX+1), "trap_id");
    SAT_VALUE_MAX_CHECK(identifier->color, (_BCM_SAT_CTF_COLOR_MAX+1), "color");
    SAT_VALUE_MAX_CHECK(identifier->tc, (_BCM_SAT_CTF_COS_MAX+1), "tc");
    sal_memset(&soc_identifier, 0, sizeof(soc_identifier));
    soc_identifier.session_id = identifier->session_id;
    soc_identifier.trap_id = identifier->trap_id;
    soc_identifier.color = identifier->color;
    soc_identifier.tc = identifier->tc;
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_identifier_unmap, (unit, &soc_identifier));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_trap_add (
	int unit, 
	uint32 trap_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    int trap_idx = 0;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_VALUE_MAX_CHECK(trap_id, (_BCM_SAT_CTF_TRAP_ID_VAL_MAX+1), "trap_id");
    rv = _bcm_common_sat_trap_id_find(psat_data, trap_id, &trap_idx);
    if (rv == BCM_E_NONE) {
        rv = BCM_E_EXISTS;
        return rv;
    }
    rv = BCM_E_NONE;
    rv = _bcm_common_sat_ctf_freed_trap_id_find(psat_data, &trap_idx);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    SAT_CTF_TRAP_ID_SET(psat_data, trap_idx, trap_id);
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_set, (unit, _BCM_SAT_CTF_TRAP_ID_MAX_NUM, psat_data->ctf_trap_id));
    _BCM_SAT_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_trap_idx_get(int unit, uint32 trap_id,int *trap_idx)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    int trap_index = 0;
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_VALUE_MAX_CHECK(trap_id, (_BCM_SAT_CTF_TRAP_ID_VAL_MAX+1), "trap_id");
    rv = _bcm_common_sat_trap_id_find(psat_data, trap_id, &trap_index);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    *trap_idx = trap_index;
#endif/* BCM_SAT_SUPPORT */
   return rv;
}

int bcm_common_sat_ctf_trap_remove (
	int unit, 
	uint32 trap_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    int trap_idx = 0;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_VALUE_MAX_CHECK(trap_id, (_BCM_SAT_CTF_TRAP_ID_VAL_MAX+1), "trap_id");
    rv = _bcm_common_sat_trap_id_find(psat_data, trap_id, &trap_idx);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    SAT_CTF_TRAP_ID_CLR(psat_data, trap_idx);
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_set, (unit, _BCM_SAT_CTF_TRAP_ID_MAX_NUM, psat_data->ctf_trap_id));
    _BCM_SAT_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_trap_remove_all
    (
	int unit
    )
{
    int rv = BCM_E_UNAVAIL;
 #ifdef BCM_SAT_SUPPORT   
    _bcm_sat_data_t *psat_data;
    int trap_idx = 0;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_CTF_TRAP_ID_CLR_ALL(psat_data);
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_set, (unit, _BCM_SAT_CTF_TRAP_ID_MAX_NUM, psat_data->ctf_trap_id));
    _BCM_SAT_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_bin_limit_set (
	int unit, 
	int bins_count, 
 	bcm_sat_ctf_bin_limit_t* bins
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_bin_limit_t soc_bins_limit[_BCM_SAT_CTF_BINS_LIMIT_CNT_MAX];
    int idx = 0;
    SAT_INIT_CHECK(unit);

    SAT_NULL_CHECK(bins, "bins");
    SAT_VALUE_CHECK(bins_count, _BCM_SAT_CTF_BINS_LIMIT_CNT_MIN, (_BCM_SAT_CTF_BINS_LIMIT_CNT_MAX+1), "bins_count");
    sal_memset(&soc_bins_limit, 0, sizeof(soc_bins_limit));
    for (idx = 0; idx < bins_count; idx++) {
        SAT_VALUE_MAX_CHECK(bins[idx].bin_select, _BCM_SAT_CTF_BINS_LIMIT_CNT_MAX, "bin_select");
        soc_bins_limit[idx].bin_select = bins[idx].bin_select;
        soc_bins_limit[idx].bin_limit = bins[idx].bin_limit;
    }
    _BCM_SAT_SYSTEM_LOCK(unit);
    rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_bin_limit_set, (unit, bins_count, soc_bins_limit));
    _BCM_SAT_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_bin_limit_get (
	int unit, 
	int max_bins_count,
        bcm_sat_ctf_bin_limit_t* bins,
	int * bins_count 
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_bin_limit_t soc_bins_limit[_BCM_SAT_CTF_BINS_LIMIT_CNT_MAX];
    int idx = 0;
    SAT_INIT_CHECK(unit);

    SAT_NULL_CHECK(bins_count, "bins_count");
    SAT_NULL_CHECK(bins, "bins");
    SAT_VALUE_CHECK(max_bins_count, _BCM_SAT_CTF_BINS_LIMIT_CNT_MIN, _BCM_SAT_CTF_BINS_LIMIT_CNT_MAX+1, "max_bins_count");
    sal_memset(&soc_bins_limit, 0, sizeof(soc_bins_limit));
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_bin_limit_get, (unit, max_bins_count, bins_count, soc_bins_limit));
    _BCM_SAT_SYSTEM_UNLOCK(unit); 
    for (idx = 0; idx < (*bins_count); idx++) {
        bins[idx].bin_select = soc_bins_limit[idx].bin_select;
        bins[idx].bin_limit = soc_bins_limit[idx].bin_limit;
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_stat_config_set (
	int unit,
	bcm_sat_ctf_t ctf_id,
	bcm_sat_ctf_stat_config_t *stat
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_stat_config_t soc_sat_ctf_stat_cfg;
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(stat, "stat");
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1), "ctf_id");
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    sal_memset(&soc_sat_ctf_stat_cfg, 0, sizeof(soc_sat_ctf_stat_cfg));
    soc_sat_ctf_stat_cfg.bin_min_delay = stat->bin_min_delay;
    soc_sat_ctf_stat_cfg.bin_step = stat->bin_step;
    soc_sat_ctf_stat_cfg.use_global_bin_config = (stat->use_global_bin_config ? 1 : 0);
    soc_sat_ctf_stat_cfg.update_counters_in_unvavail_state = (stat->update_counters_in_unvavail_state ? 1 : 0);
    _BCM_SAT_SYSTEM_LOCK(unit);
    rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_stat_config_set, (unit, ctf_id, &soc_sat_ctf_stat_cfg));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_stat_get (
	int unit, 
	bcm_sat_ctf_t ctf_id,
	uint32 flags,
	bcm_sat_ctf_stat_t* stat
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
	soc_sat_ctf_stat_t soc_stat;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(stat, "stat");
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1), "ctf_id");
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    sal_memset(&soc_stat, 0, sizeof(soc_sat_ctf_stat_t));
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_stat_get, (unit, ctf_id, flags, &soc_stat));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
    sal_memcpy(stat, &soc_stat, sizeof(bcm_sat_ctf_stat_t));
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_availability_config_set (
	int unit,
	bcm_sat_ctf_t ctf_id,
	bcm_sat_ctf_availability_config_t *config
	)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_availability_config_t avail_cfg;
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(config, "config");
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1), "ctf_id");
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    SAT_VALUE_MAX_CHECK(config->switch_state_num_of_slots,
                       (_BCM_SAT_CTF_SWITCH_STATE_NUM_OF_SLOTS_MAX+1), "switch_state_num_of_slots");
    SAT_VALUE_MAX_CHECK(config->switch_state_threshold_per_slot,
                       (_BCM_SAT_CTF_SWITCH_STATE_THRES_PER_SLOT_MAX+1), "switch_state_threshold_per_slot");
    sal_memset(&avail_cfg, 0, sizeof(avail_cfg));
    avail_cfg.switch_state_num_of_slots = config->switch_state_num_of_slots;
    avail_cfg.switch_state_threshold_per_slot = config->switch_state_threshold_per_slot;
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_availability_config_set, (unit, ctf_id, &avail_cfg));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_trap_data_to_session_map (
	int unit,
	uint32 trap_data, 
	uint32 trap_data_mask,
	uint32 session_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    uint32 entry_idx;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_VALUE_MAX_CHECK(trap_data, (_BCM_SAT_CTF_TRAP_DATA_MAX+1), "trap_data");
    SAT_VALUE_MAX_CHECK(trap_data_mask, (_BCM_SAT_CTF_TRAP_DATA_MASK_MAX+1), "trap_data_mask");
    SAT_VALUE_MAX_CHECK(session_id, (_BCM_SAT_CTF_TRAP_DATA_SSID_MAX+1), "session_id");

    rv = _bcm_common_sat_ctf_freed_trap_entry_find(psat_data, trap_data, trap_data_mask, &entry_idx);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_data_to_session_map, (unit, trap_data, trap_data_mask, entry_idx, session_id));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
    SAT_BITMAP_SET(psat_data->ctf_ssid_bitmap, entry_idx);
    psat_data->trap_data[entry_idx].trap_data = trap_data;
    psat_data->trap_data[entry_idx].trap_mask = trap_data_mask;
    psat_data->trap_data[entry_idx].ssid = session_id;
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_trap_data_to_session_unmap (
	int unit,
	uint32 trap_data,
	uint32 trap_data_mask
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    uint32 entry_idx;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_VALUE_MAX_CHECK(trap_data, (_BCM_SAT_CTF_TRAP_DATA_MAX+1), "trap_data");
    SAT_VALUE_MAX_CHECK(trap_data_mask, (_BCM_SAT_CTF_TRAP_DATA_MASK_MAX+1), "trap_data_mask");
    rv = _bcm_common_sat_ctf_trap_entry_find(psat_data, trap_data, trap_data_mask, &entry_idx);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_data_to_session_unmap, (unit, entry_idx));
    _BCM_SAT_SYSTEM_UNLOCK(unit);
    SAT_BITMAP_CLR(psat_data->ctf_ssid_bitmap, entry_idx);
    sal_memset(&(psat_data->trap_data[entry_idx]), 0, sizeof(_bcm_sat_ctf_trap_data_t));
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_reports_config_set (
	int unit,
	bcm_sat_ctf_t ctf_id,
	bcm_sat_ctf_report_config_t *reports
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
	soc_sat_ctf_report_config_t soc_reports;
    _bcm_sat_data_t *psat_data;
    SAT_INIT_CHECK(unit);

    psat_data = sat_data_get(unit);
    SAT_NULL_CHECK(reports, "reports");
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1), "ctf_id");
    SAT_VALUE_MAX_CHECK(reports->report_sampling_percent, (_BCM_SAT_CTF_REPORT_SAMPLING_PER_MAX+1), "report_sampling_percent");
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    sal_memset(&soc_reports, 0, sizeof(bcm_sat_ctf_report_config_t));
    soc_reports.report_sampling_percent = reports->report_sampling_percent;
    if (reports->flags & BCM_SAT_CTF_REPORT_ADD_SEQ_NUM) {
        soc_reports.flags |= SOC_SAT_CTF_REPORT_ADD_SEQ_NUM;
    }
    if (reports->flags & BCM_SAT_CTF_REPORT_ADD_DELAY) {
        soc_reports.flags |= SOC_SAT_CTF_REPORT_ADD_DELAY;
    }
    _BCM_SAT_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_reports_config_set, (unit, ctf_id, &soc_reports));
    _BCM_SAT_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_interrupt_process(int unit)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        bcm_sat_event_type_t type = bcmSATEventReport;
        BCM_IF_ERROR_RETURN(bcm_sb2_sat_ctf_report_process(unit,
                                        _bcm_sat_event_cb[unit][type],
                                        _bcm_sat_event_ud[unit][type]));
    }

    /* Clear OAMP interrupt */
    rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_oamp_stat_event_clear, (unit));
#endif /* BCM_SABER2_SUPPORT */

#endif /* BCM_SAT_SUPPORT */

    return rv;
}

int
bcm_common_sat_event_register(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_SAT_SUPPORT
    bcm_sat_event_type_t type;

    if (soc_feature(unit, soc_feature_sat) &&
        soc_property_get(unit, spn_SAT_ENABLE, 0)) {
        rv = BCM_E_NONE;
        for (type = 0; type < bcmSATEventCount; type++) {
            if (type == event_type) {
                break;
            }
        }
        if (type == bcmSATEventCount) {
            return BCM_E_PARAM;
        }

        if (_bcm_sat_event_cb[unit][type] &&
            (_bcm_sat_event_cb[unit][type] != cb)) {
            LOG_ERROR(BSL_LS_BCM_SAT, (BSL_META_U(unit,
              "EVENT %d already has a registered callback\n"), type));
            return BCM_E_EXISTS;
        }
        _bcm_sat_event_cb[unit][type] = cb;
        _bcm_sat_event_ud[unit][type] = user_data;

#if defined(BCM_SABER2_SUPPORT)
        BCM_IF_ERROR_RETURN(soc_common_sat_handler_register(
                                unit, bcm_common_sat_interrupt_process));
        /* Enable OAMP interrupt */
        rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_oamp_stat_event_control, (unit, 1));
#elif defined(BCM_DPP_SUPPORT)
        rv = _bcm_jer_sat_event_register(unit, event_type, cb, user_data);
#endif
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int
bcm_common_sat_event_unregister(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_SAT_SUPPORT
    bcm_sat_event_type_t type;

    if (soc_feature(unit, soc_feature_sat) &&
        soc_property_get(unit, spn_SAT_ENABLE, 0)) {
        rv = BCM_E_NONE;
        for (type = 0; type < bcmSATEventCount; type++) {
            if (type == event_type) {
                break;
            }
        }
        if (type == bcmSATEventCount) {
            return BCM_E_PARAM;
        }

        if (_bcm_sat_event_cb[unit][type] &&
            (_bcm_sat_event_cb[unit][type] != cb)) {
            LOG_ERROR(BSL_LS_BCM_SAT, (BSL_META_U(unit,
              "A different callback is registered for %d\n"), type));
            return BCM_E_EXISTS;
        }

        _bcm_sat_event_cb[unit][type] = NULL;
        _bcm_sat_event_ud[unit][type] = NULL;
#if defined(BCM_SABER2_SUPPORT)
        /* Disable OAMP interrupt */
        rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_oamp_stat_event_control, (unit, 0));
        BCM_IF_ERROR_RETURN(soc_common_sat_handler_register(unit, NULL));
#elif defined(BCM_DPP_SUPPORT)
        rv = _bcm_jer_sat_event_unregister(unit, event_type, cb);
#endif

    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}


/*
 * Function:
 *      bcm_common_sat_endpoint_create
 * Purpose:
 *      Create or replace a SAT endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an SAT endpoint structure
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_common_sat_endpoint_create(
    int unit,
    bcm_sat_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_sat))
    {
#ifdef BCM_SABER2_SUPPORT
        rv = bcm_sb2_sat_endpoint_create(unit, endpoint_info);
#endif /* BCM_SABER2_SUPPORT */
    }

    return rv;
}

/*
 * Function:
 *      bcm_common_sat_endpoint_destroy
 * Purpose:
 *      Destroy an SAT endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the SAT endpoint object to destroy
 *      flags - (IN) The SAT UPMEP/DOWNMEP endpoint type to be destroyed
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_common_sat_endpoint_destroy(
    int unit,
    bcm_sat_endpoint_t endpoint,
    uint32 flags)

{
    int rv = BCM_E_UNAVAIL;
    if (soc_feature(unit, soc_feature_sat))
    {
#ifdef BCM_SABER2_SUPPORT
        rv = bcm_sb2_sat_endpoint_destroy(unit, endpoint, flags);
#endif /* BCM_SABER2_SUPPORT */
    }

    return rv;

}

/*
 * Function:
 *      bcm_common_sat_endpoint_destroy_all
 * Purpose:
 *      Destroy all SAT endpoint objects associated for a given SAT
 *      endpoint type (UPMEP/DOWNMEP)
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) The SAT UPMEP/DOWNMEP endpoint type whose endpoints should be destroyed
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_common_sat_endpoint_destroy_all(
    int unit,
    uint32 flags)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_sat))
    {
#ifdef BCM_SABER2_SUPPORT
        rv = bcm_sb2_sat_endpoint_destroy_all(unit, flags);
#endif /* BCM_SABER2_SUPPORT */
    }

    return rv;
}


/*
 * Function:
 *      bcm_common_sat_endpoint_get
 * Purpose:
 *      Get an SAT endpoint object
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an SAT endpoint structure to
 *                      receive the data
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int bcm_common_sat_endpoint_get(
    int unit,
    bcm_sat_endpoint_t endpoint,
    uint32 flags,
    bcm_sat_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_sat))
    {
#ifdef BCM_SABER2_SUPPORT
        rv = bcm_sb2_sat_endpoint_get(unit, endpoint, flags, endpoint_info);
#endif /* BCM_SABER2_SUPPORT */
    }

    return rv;
}


/*
 * Function:
 *      bcm_common_sat_endpoint_traverse
 * Purpose:
 *      Traverse the set of SAT endpoints associated with the
 *      specified group, calling a specified callback for each one
 * Parameters:
 *      unit - (IN) Unit number.
 *      cb - (IN) A pointer to the callback function to call for each OAM
endpoint in the specified group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * result =s:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_common_sat_endpoint_traverse(
    int unit,
    uint32 flags,
    bcm_sat_endpoint_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_sat))
    {
#ifdef BCM_SABER2_SUPPORT
        rv = bcm_sb2_sat_endpoint_traverse(unit,flags,cb,user_data);
#endif /* BCM_SABER2_SUPPORT */
    }

    return rv;
}

int bcm_common_oamp_port_enable_get(int unit, bcm_port_t port) {

#if defined(BCM_SABER2_SUPPORT)
    int oamp_enable;
    if(SOC_IS_SABER2(unit) && (port == _BCM_SB2_SAT_OAMP_PHY_PORT_NUMBER)) {
        (void)bcm_sb2_sat_oamp_enable_get(unit, &oamp_enable);
        /* Skip OAMP port for saber2 device */
        if(oamp_enable) {
            return TRUE;
        }
    }
#endif

    return FALSE;
}
/*
 * Function:
 *      bcm_common_sat_session_inuse
 * Purpose:
 *      To get the whether the session ID was used or not
 * Parameters:
 *      unit - (IN) Unit number.
 *      session_id - (IN) SAT session_id

 * result: True, session ID is in used. False, session ID is free.
 *      
 * Notes:
 */
int bcm_common_sat_session_inuse( int unit,int session_id)
{
    int rv = BCM_E_UNAVAIL;
    
    #ifdef BCM_SAT_SUPPORT
    
    rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_session_inuse, (unit, session_id));
    #endif
    
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
#ifdef BCM_SAT_SUPPORT
int
bcm_common_sat_wb_init(int unit)
{
    _bcm_sat_data_t   *psat_data;
    int             rv = BCM_E_NONE;
    uint32          size;
    uint32          scache_handle;
    uint8           *scache_ptr = NULL;
    uint16          default_ver = SAT_WB_CURRENT_VERSION;
    uint16          recovered_ver = SAT_WB_CURRENT_VERSION;
    int             stable_size;
    int             create;

    psat_data = sat_data_get(unit);

    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_SAT, 0);
    create = (SOC_WARM_BOOT(unit) ? FALSE : TRUE);
    size = (SOC_WARM_BOOT(unit) ? 0 : sizeof(_bcm_sat_data_t));

    /* on cold boot, setup scache */
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      create,
                                      &size, &scache_ptr,
                                      default_ver, &recovered_ver);
    if (BCM_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        LOG_ERROR(BSL_LS_BCM_SAT,
                  (BSL_META_U(unit,
                              "Error(%s) reading scache. scache_ptr:%p and len:%d\n"),
                   soc_errmsg(rv), scache_ptr, size));
        /* If warmboot initialization fails, skip using warmboot for sat */
        rv = SOC_E_NOT_FOUND;
        return rv;
    }

    LOG_VERBOSE(BSL_LS_BCM_SAT,
                (BSL_META_U(unit,
                            "SAT: allocating 0x%x (%d) bytes of scache:"),
                 size, size));
    
    if (SOC_WARM_BOOT(unit)) {
        soc_scache_handle_used_set(unit, scache_handle, size);
        /* if supporting warmboot, use scache */
        if (scache_ptr) {
            sal_memcpy(psat_data, scache_ptr, sizeof(_bcm_sat_data_t));
            /* clear out non-recoverable resources */
            psat_data->sat_mutex = NULL;
        } 
    }

    return BCM_E_NONE;
}
#endif /* BCM_SAT_SUPPORT */

int
bcm_common_sat_wb_sync(int unit, int sync)
{
    int                     rv = BCM_E_NONE;
    uint8                   *scache_ptr = NULL;
    uint32                  scache_len = 0;
    soc_scache_handle_t     scache_handle;
    uint16                  default_ver = SAT_WB_CURRENT_VERSION;
    uint16                  recovered_ver = SAT_WB_CURRENT_VERSION;
    _bcm_sat_data_t         *psat_data;
    int                     stable_size;

    if(!_bcm_common_sat_is_dirty(unit))
        return BCM_E_NONE;

    psat_data = sat_data_get(unit);

    if (SOC_WARM_BOOT(unit)) {
        LOG_ERROR(BSL_LS_BCM_SAT,
                  (BSL_META_U(unit,
                              "Cannot write to SCACHE during WarmBoot\n")));
        return SOC_E_INTERNAL;
    }
    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_SAT, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &scache_len, &scache_ptr,
                                      default_ver, &recovered_ver);
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        LOG_ERROR(BSL_LS_BCM_SAT,
                  (BSL_META_U(unit,
                              "Error(%s) reading scache. scache_ptr:%p and len:%d\n"),
                   soc_errmsg(rv), scache_ptr, scache_len));
        return rv;
    }

    sal_memcpy(scache_ptr, psat_data, sizeof(_bcm_sat_data_t));

    rv = soc_scache_handle_used_set(unit, scache_handle, sizeof(_bcm_sat_data_t));

    if (sync) {
        rv = soc_scache_commit(unit);
        if (rv != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_SAT,
                      (BSL_META_U(unit,
                                  "Error(%s) sync'ing scache to Persistent memory. \n"),
                       soc_errmsg(rv)));
            return rv;
        }
    }

    _bcm_commot_sat_mark_not_dirty(unit);

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

