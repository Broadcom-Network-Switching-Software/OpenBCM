/** \file bcm_int/dnx/sat/sat.h
 *
 * Internal DNX SAT APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/dnx_dispatch.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_sat.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/counter.h>
#include <bcm/types.h>
#include <bcm_int/common/debug.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sat.h>
#include <bcm_int/dnx/algo/sat/algo_sat.h>

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>

#ifndef SAT_H_INCLUDED__
/* { */
#define SAT_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINEs
 * {
 */

#define DNX_ALGO_TEMPLATE_SAT_PKT_HEADER "TEMPLATE_SAT_PKT_HEADER"
#define DNX_SAT_BITS_PER_BYTE               (8)
#define DNX_SAT_GTF_ID_MIN                  (0)
#define DNX_SAT_GTF_ID_MAX                  (DNX_DATA_MAX_SAT_GENERATOR_NOF_GTF_IDS - 1)
#define DNX_SAT_GTF_PKT_HDR_LEN_MIN         (1)
#define DNX_SAT_GTF_PKT_HDR_LEN_MAX         (DNX_DATA_MAX_SAT_GENERATOR_GTF_PKT_HEADER_MAX_LENGTH - 1)
#define DNX_SAT_GTF_PKT_LEN_PATN_INX_MIN    (0)
#define DNX_SAT_GTF_PKT_LEN_PATN_INX_MAX    (DNX_DATA_MAX_SAT_GENERATOR_GTF_PKT_LENGTH_MAX_PATN)
#define DNX_SAT_GTF_SEQ_PERIOD_MIN          (1)
#define DNX_SAT_GTF_SEQ_PERIOD_MAX          (DNX_DATA_MAX_SAT_GENERATOR_GTF_MAX_SEQ_PERIOD)
#define DNX_SAT_GTF_STAMP_INC_STEP_MIN      (1)
#define DNX_SAT_GTF_STAMP_INC_STEP_MAX      (DNX_DATA_MAX_SAT_GENERATOR_GTF_STAMP_MAX_INC_STEP )
#define DNX_SAT_GTF_STAMP_INC_PERIOD_MIN    (0)
#define DNX_SAT_GTF_STAMP_INC_PERIOD_MAX    (DNX_DATA_MAX_SAT_GENERATOR_GTF_STAMP_MAX_INC_PERIOD)
#define DNX_SAT_GTF_STAMP_2_bit_VALUE_MIN   (0)
#define DNX_SAT_GTF_STAMP_2_bit_VALUE_MAX   (0x3)
#define DNX_SAT_GTF_STAMP_16_bit_VALUE_MIN  (0)
#define DNX_SAT_GTF_STAMP_16_bit_VALUE_MAX  (0xffff)
#define DNX_SAT_GTF_BW_BURST_MIN            (0)
#define DNX_SAT_GTF_BW_BURST_MAX            (DNX_DATA_MAX_SAT_GENERATOR_GTF_BW_MAX_VALUE)
#define DNX_SAT_GTF_BW_RATE_MIN             (0)
#define DNX_SAT_GTF_BW_RATE_MAX             (DNX_DATA_MAX_SAT_GENERATOR_GTF_RATE_MAX)
#define DNX_SAT_GTF_BW_PPS_RATE_MAX         (DNX_DATA_MAX_SAT_GENERATOR_GTF_PPS_RATE_MAX)
#define DNX_SAT_GTF_RATE_PATN_HIGH_TH_MIN   (1)
#define DNX_SAT_GTF_RATE_PATN_HIGH_TH_MAX   (DNX_DATA_MAX_SAT_GENERATOR_GTF_RATE_MAX)
#define DNX_SAT_GTF_RATE_PATN_LOW_TH_MIN    (1)
#define DNX_SAT_GTF_RATE_PATN_LOW_TH_MAX    (DNX_DATA_MAX_SAT_GENERATOR_GTF_RATE_MAX)
#define DNX_SAT_PKT_HEADER_DEFAULT_PROFILE  (0)
#define DNX_SAT_DEFAULT_CREDIT_BACKPRESSURE (8)
#define DNX_SAT_GTF_SEQ_NUM_DISABLE         (-1)
#define DNX_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS (8)
#define DNX_SAT_GTF_PACKET_LENGTH_MIN       (65)
#define DNX_SAT_BPS_MODE                    0x1
#define DNX_SAT_PPS_MODE                    0x2

#define DNX_SAT_CTF_ID_MIN                  (0)
#define DNX_SAT_CTF_ID_MAX                  (DNX_DATA_MAX_SAT_COLLECTOR_NOF_CTF_IDS - 1)
#define DNX_SAT_CTF_ID_HALF                 (DNX_DATA_MAX_SAT_COLLECTOR_NOF_CTF_IDS/2)
#define DNX_SAT_CTF_OAM_ID_MIN              (0)
#define DNX_SAT_CTF_OAM_ID_MAX              (DNX_DATA_MAX_SAT_COLLECTOR_NOF_OAM_IDS - 1)
#define DNX_SAT_PKT_HERDER_SIZE             (DNX_DATA_MAX_SAT_GENERATOR_GTF_PKT_HEADER_MAX_LENGTH/4)
#define DNX_SAT_PKT_HERDER_LSB_FIELD_SIZE   (DNX_SAT_PKT_HERDER_SIZE/2)
#define DNX_SAT_PKT_HERDER_MSB_FIELD_SIZE   (DNX_SAT_PKT_HERDER_SIZE/2)
#define DNX_SAT_CTF_TRAP_ID_MIN             (0)
#define DNX_SAT_CTF_TRAP_ID_MAX             (2)
#define DNX_SAT_CTF_COLOR_MIN               (0)
#define DNX_SAT_CTF_COLOR_MAX               (3)
#define DNX_SAT_CTF_COS_MIN                 (0)
#define DNX_SAT_CTF_COS_MAX                 (7)
#define DNX_SAT_CTF_TRAP_ID_UNSET_VAL       (0x0)
#define DNX_SAT_CTF_BINS_LIMIT_CNT_MIN      (1)
#define DNX_SAT_CTF_BINS_LIMIT_CNT_MAX      (DNX_DATA_MAX_SAT_COLLECTOR_CTF_BINS_LIMIT_ENTRIES)
#define DNX_SAT_CTF_REPORT_SAMPLING_PER_MAX (100)
#define DNX_SAT_CTF_MAX_REPORT_SHRESHOLD (255)
#define DNX_SAT_BITS_PER_BYTE               (8)
#define DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8  (8)
#define DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U32 (2)
#define DNX_SAT_PKT_LENGTH_PATTERN_FIELD_SIZE      ((DNX_DATA_MAX_SAT_GENERATOR_GTF_PKT_LEN_PATTEN_SIZE/8)/4)
#define DNX_SAT_PKT_HERDER_BITS             (DNX_DATA_MAX_SAT_GENERATOR_GTF_PKT_HEADER_MAX_LENGTH*8)

#define DNX_AM_TEMPLATE_SAT_PKT_HEADER_LOW_ID      (0)
#define DNX_AM_TEMPLATE_SAT_PKT_HEADER_COUNT       (DNX_DATA_MAX_SAT_GENERATOR_GTF_PKT_HEADER_ENTRIES + 1)
#define DNX_AM_TEMPLATE_SAT_PKT_HEADER_MAX_ENTRIES(unit) DNX_DATA_MAX_SAT_GENERATOR_NOF_GTF_IDS
#define DNX_AM_TEMPLATE_SAT_PKT_HEADER_SIZE        (sizeof(dnx_sat_gtf_pkt_hdr_tbl_info_t))
/*
 * Max number of JR2 SAT events is 20 since 640 bits/32 bits = 20.
 */
#define DNX_SAT_MAX_JR2_SAT_EVENTS 20

/* Maximum number of events */
#define DNX_SAT_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW (10240)
/* convert rate compensation from float to int, inorder to save in DBAL */
#define DNX_SAT_GTF_RATE_CMP_CONVERSION (1000000.0)

/* Callbacks are not supported by warmboot. Need to re-register after warmboot */
bcm_sat_event_cb _g_dnx_sat_event_cb[BCM_MAX_NUM_UNITS][bcmSATEventCount];

/* User data */
void *_g_dnx_sat_event_ud[BCM_MAX_NUM_UNITS][bcmSATEventCount];

/** JR2 SAT EVENT FORMAT */
typedef struct dnx_sat_event_data_s
{
    uint8 is_valid;
    uint8 flow_id;
    uint8 d_set;
    uint8 s_set;
    uint8 size;
    uint8 event_type;
    uint32 frame_delay;
    uint32 seq_num;
} dnx_sat_event_data_t;

/** The interrupt event data. Format and the data array */
typedef struct dnx_sat_interrupt_event_data_s
{
    dnx_sat_event_data_t jr2_sat_event_data[DNX_SAT_MAX_JR2_SAT_EVENTS];
} dnx_sat_interrupt_event_data_t;

/* SAT event types generated by OAMP */
typedef enum
{
    DNX_SAT_OAMP_DMA_EVENT_TYPE_EVENT,
    DNX_SAT_OAMP_DMA_EVENT_TYPE_STAT_EVENT,
    DNX_SAT_OAMP_DMA_EVENT_TYPE_NOF
} dnx_sat_dma_event_type_t;

typedef enum dnx_sat_max_value_type_s
{
    dnxSATPayloadOffset = 0,
    dnxSATSeqNumOffset = 1,
    dnxSATTimeStampOffset = 2,
    dnxSATCrcOffset = 3,
    dnxSATGtfRateStopIter = 4,
    dnxSATNumSlotForStateToSwitch = 5,
    dnxSATThersholdPerSlot = 6,
    dnxSATCtfTrapData = 7,
    dnxSATCtfTrapDataMask = 8,
    dnxSATCtfTrapSessionId = 9,
    dnxSATRportShreshold = 10,
    dnxSATCtfTrapId = 11,
    dnxSATGtfSeqNumOffset = 12,
    dnxSATGtfCrcOffset = 13,
    dnxSATMaxValueTypeCnt = 14
} dnx_sat_max_value_type_t;

typedef struct dnx_sat_init_s
{
    uint32 rate_num_clks_cycle;
    uint32 num_clks_sec;
} dnx_sat_init_t;

typedef struct dnx_sat_ctf_trap_data_s
{
    uint16 trap_data;
    uint16 trap_mask;
    uint8 ssid;
} dnx_sat_ctf_trap_data_t;

typedef struct dnx_sat_data_s
{
    uint32 ctf_trap_id[DNX_DATA_MAX_SAT_COLLECTOR_TRAP_ID_ENTRIES];
    dnx_sat_ctf_trap_data_t trap_data[DNX_DATA_MAX_SAT_COLLECTOR_TRAP_DATA_ENTRIES];
    uint32 gtf_header_id[DNX_DATA_MAX_SAT_GENERATOR_NOF_GTF_IDS];
    uint8 is_dirty;
    uint8 rate_mode;            /* 1: bps 2:pps */
    uint8 granularity_flag;     /* flag for granularity */
} dnx_sat_data_t;

typedef struct dnx_sat_gtf_pkt_hdr_tbl_info_s
{
    uint32 pkt_header[DNX_SAT_PKT_HERDER_SIZE];
    uint32 pkt_header_len;
} dnx_sat_gtf_pkt_hdr_tbl_info_t;

typedef enum dnx_sat_rx_flv_bin_limit_s
{
    dnxSATRxFLVBinLimit0 = 0,
    dnxSATRxFLVBinLimit1 = 1,
    dnxSATRxFLVBinLimit2 = 2,
    dnxSATRxFLVBinLimit3 = 3,
    dnxSATRxFLVBinLimit4 = 4,
    dnxSATRxFLVBinLimit5 = 5,
    dnxSATRxFLVBinLimit6 = 6,
    dnxSATRxFLVBinLimit7 = 7,
    dnxSATRxFLVBinLimit8 = 8,
    dnxSATRxFLVBinLimitCnt = 9
} dnx_sat_rx_flv_bin_limit_s;

typedef struct dnx_sat_gtf_stamp_field_tbl_info_s
{
    uint32 stamp_2_ena;
    uint32 stamp_2_offset;
    uint32 stamp_2_shift;
    uint32 stamp_2_value;
    uint32 counter_8_ena;
    uint32 counter_8_offset;
    uint32 counter_8_shift;
    uint32 counter_8_mask;
    uint32 counter_8_inc_step;
    uint32 counter_8_period;
    uint32 counter_16_ena;
    uint32 counter_16_offset;
    uint32 counter_16_shift;
    uint32 counter_16_mask;
    uint32 counter_16_inc_step;
    uint32 counter_16_period;
    uint32 stamp_16_ena;
    uint32 stamp_16_offset;
    uint32 stamp_16_shift;
    uint32 stamp_16_value;
} dnx_sat_gtf_stamp_field_tbl_info_t;

/*
 * }
 */
/*
 * MACROs
 * {
 */

/* } */

shr_error_e dnx_sat_init(
    int unit);

shr_error_e dnx_sat_deinit(
    int unit);

shr_error_e dnx_sat_gtf_stat_multi_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    uint64 *packet_cnt,
    uint64 *byte_cnt);

void dnx_gtf_stat_update_callback(
    int unit);

shr_error_e dnx_am_template_sat_pkt_header_create(
    int unit,
    int default_profile,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * header_info);

shr_error_e dnx_am_template_sat_pkt_header_allocate(
    int unit,
    int flags,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * header_info,
    int *is_allocated,
    int *header_template);

shr_error_e dnx_am_template_sat_pkt_header_free(
    int unit,
    int header_template,
    int *is_last);

shr_error_e dnx_am_template_sat_pkt_header_data_get(
    int unit,
    int header_template,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * header_info);

shr_error_e dnx_am_template_sat_pkt_header_exchange(
    int unit,
    int flags,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * header_info,
    int old_header_template,
    int *is_last,
    int *new_header_template,
    int *is_allocated);

void dnx_algo_sat_pkt_header_print_cb(
    int unit,
    const void *data);

shr_error_e dnx_sat_gtf_id_allocate(
    int unit,
    uint32 flags,
    bcm_sat_gtf_t * gtf_id);

shr_error_e dnx_sat_gtf_id_free(
    int unit,
    bcm_sat_gtf_t gtf_id);

shr_error_e dnx_sat_nof_free_gtf(
    int unit,
    int *nof_free_gtf);

shr_error_e dnx_sat_ctf_id_allocate(
    int unit,
    uint32 flags,
    bcm_sat_ctf_t * ctf_id);

shr_error_e dnx_sat_ctf_id_free(
    int unit,
    bcm_sat_ctf_t ctf_id);

shr_error_e dnx_sat_ctf_trap_entry_allocate(
    int unit,
    int *trap_entry_idx);

shr_error_e dnx_sat_ctf_trap_entry_free(
    int unit,
    int trap_entry_idx);

shr_error_e dnx_sat_ctf_trap_id_allocate(
    int unit,
    int *trap_idx);

shr_error_e dnx_sat_ctf_trap_id_free(
    int unit,
    int trap_idx);

shr_error_e dnx_sat_packet_config_exp_payload_uint8_to_long(
    uint8 exp_payload_u8[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8],
    uint8 num_of_bytes,
    uint32 is_reverse,
    uint32 exp_payload_long[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U32]);

shr_error_e dnx_sat_packet_config_exp_payload_long_to_uint8(
    uint8 exp_payload_u8[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U8],
    uint8 num_of_bytes,
    uint32 is_reverse,
    uint32 exp_payload_long[DNX_SAT_PAYLOAD_MAX_PATTERN_NOF_U32]);

shr_error_e dnx_sat_max_value_get(
    int unit,
    dnx_sat_max_value_type_t type,
    int *max_value);

shr_error_e dnx_sat_data_set(
    int unit,
    dnx_sat_data_t * psat_data);

shr_error_e dnx_sat_data_get(
    int unit,
    dnx_sat_data_t * psat_data);

shr_error_e dnx_sat_dynamic_memory_access_set(
    int unit,
    uint32 enable);

shr_error_e dnx_sat_gtf_credit_config(
    int unit,
    int pipe_id,
    int credit);

void dnx_sat_dma_event_handler(
    int unit,
    void *entry,
    int entry_size);

void dnx_sat_event_handler(
    int unit);

shr_error_e dnx_sat_fifo_interrupt_handler(
    int unit,
    void *host_entry);

shr_error_e dnx_sat_ctf_reports_config_set(
    int unit,
    bcm_sat_ctf_t ctf_id,
    bcm_sat_ctf_report_config_t * reports);

shr_error_e dnx_sat_session_inuse(
    int unit,
    int session_id,
    int *found);

shr_error_e dnx_sat_trap_idx_get(
    int unit,
    uint32 trap_id,
    int *trap_idx);

shr_error_e dnx_sat_gtf_config_rate_set(
    int unit,
    bcm_sat_gtf_t gtf_id,
    uint32 gtf_config_rate,
    uint32 gtf_rate_compensation,
    int packet_length_compensation);

/* } */
#endif /* SAT_H_INCLUDED__ */
