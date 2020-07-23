/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    sat.h
 * Purpose: SAT definitions common to SDK
 */

#ifndef _SOC_SHARED_SAT_H
#define _SOC_SHARED_SAT_H

#include <sal/types.h>
#include <bcm/pkt.h>
#include <bcm_int/common/sat.h>
#include <soc/error.h>

#ifndef SOC_SAND_IN
#define SOC_SAND_IN   const
#define SOC_SAND_OUT
#define SOC_SAND_INOUT
#endif

#define BCM_SAT_GTF_STAT_FIX                      0x1
#define BCM_SAT_DBG                               0x1

/* During CRC calculation the packet data bits are reversed */
#define SOC_SAT_CONFIG_CRC_REVERSED_DATA    0x01
/* During CRC calculation the result is inverted and reversed before stamping */
#define SOC_SAT_CONFIG_CRC_INVERT           0x02
/* During CRC calculations, reset CRC value is taken to be 32'hffffffff */
#define SOC_SAT_CONFIG_CRC_HIGH_RESET       0x04
/* During PRBS calculations the next bit is created using NXOR (and not XOR) */
#define SOC_SAT_CONFIG_PRBS_USE_NXOR        0x08
#define SOC_SAT_MEP_100K_SCAN_IN_SEC        166      /* 1.66ms interval. Avoid floating pt.*/
#define SOC_SAT_100K_NUM                    100000 

/* GTF macro */
#define SOC_SAT_GTF_OBJ_COMMON          _BCM_SAT_GTF_OBJ_COMMON
#define SOC_SAT_GTF_OBJ_CIR             _BCM_SAT_GTF_OBJ_CIR
#define SOC_SAT_GTF_OBJ_EIR             _BCM_SAT_GTF_OBJ_EIR

#define SOC_SAT_GTF_WITH_ID                       0x0001   /* Use the specified GTF ID */        
#define SOC_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS 8
#define SOC_SAT_GTF_PACKET_MAX_PATTERN_LENGTH     32
#define SOC_SAT_GTF_MAX_STAMPS                    3
#define SOC_SAT_GTF_PACKET_EDIT_ADD_END_TLV       0x01
#define SOC_SAT_GTF_PACKET_EDIT_ADD_CRC           0x02
#define SOC_SAT_GTF_NUM_OF_PRIORITIES             2 /* CIR and EIR */
#define SOC_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST 0x01  /* If set, INTERVAL state limit is set to the configuration,
                                                                 otherwise to the number of packets sent in BURST state */
#define SOC_SAT_GTF_SEQ_NUM_DISABLE               -1

#define SOC_SAT_GTF_RATE_IN_BYTES                 0
#define SOC_SAT_GTF_RATE_IN_PACKETS               1
#define SOC_SAT_GTF_RATE_WITH_GRANULARITY         2




#define SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U8        8
#define SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U32       2

#define SOC_SAT_PKT_HERDER_LSB_FIELD_SIZE         18
#define SOC_SAT_PKT_HERDER_MSB_FIELD_SIZE         15
#define SOC_SAT_PKT_LENGTH_PATTERN_FIELD_SIZE     3

/* Table, register index */
#define SAT_TX_EVC_PARAMS_ENTRY_1_TBL_INDX(gtf_id) \
  ((gtf_id<<1))                      /* Per evc parameters, two memory entries are needed. Occupy entries 0-15 (even entries) */
#define SAT_TX_EVC_PARAMS_ENTRY_2_TBL_INDX(gtf_id) \
  ((gtf_id<<1) + 1)                  /* Per evc parameters, two memory entries are needed. Occupy entries 0-15 (odd entries) */
#define SAT_TX_GEN_PARAMS_TBL_INDX(gtf_id, priority) \
  ((gtf_id<<1) + priority + 16)      /* Per generator parameters, occupy entries 16-31 */
#define SAT_TX_PER_GENERATOR_INDX(gtf_id, priority) \
  ((gtf_id<<1) + priority)           /* per generator index */
#define SAT_TX_PER_EVC_INDX(gtf_id) \
  (gtf_id)                           /* per evc index */

#define SAT_TX_GEN_PRM_ENTRY_1_TBL_INDX(gtf_id, priority) \
  ((gtf_id<<2) + (priority<<1))           /* per generator parameters, two memory entries are needed, only for QAX*/

#define SAT_TX_GEN_PRM_ENTRY_2_TBL_INDX(gtf_id, priority) \
  ((gtf_id<<2) + (priority<<1) + 1)           /* per generator parameters, two memory entries are needed, only for QAX*/



/* CTF macro */
#define SOC_SAT_CTF_WITH_ID                       0x0001   /* Use the specified CTF ID */
/* SAT CTF packet config flags */
/* If set, add end tlv, otherwise not add end tlv */
#define SOC_SAT_CTF_PACKET_INFO_ADD_END_TLV        0x01
/* If set, add crc, otherwise not add crc */
#define SOC_SAT_CTF_PACKET_INFO_ADD_CRC            0x02

#define SOC_SAT_STAT_MAX_NUM_OF_BINS              10
#define SOC_SAT_CTF_BINS_LIMIT_CNT                9
#define SOC_SAT_CTF_REPORT_ADD_SEQ_NUM            0x01
#define SOC_SAT_CTF_REPORT_ADD_DELAY              0x02
#define SOC_SAT_CTF_TRAP_ID_MAX_NUM               3 
#define SOC_SAT_CTF_TRAP_ID_BITS_NOF              8 
#define SOC_SAT_CTF_REPORT_SAMPLING_PER_MAX       100
#define SOC_SAT_CTF_RX_FLOW_PARAMS_ENTRY_SIZE     9
#define SOC_SAT_CTF_STAT_DO_NOT_CLR_ON_READ       0x1 

/*CTF rx flow params offset*/
#define SOC_SAT_CRC_OFFSET                        (SOC_IS_QAX(unit) ? 31 : 32)
#define SOC_SAT_SEQ_NUM_OFFSET                    (SOC_IS_QAX(unit) ? 47 : 48)
#define SOC_SAT_EXP_RPT_PATTERN_OFFSET            (SOC_IS_QAX(unit) ? 63 : 64)
#define SOC_SAT_ADD_END_TLV_OFFSET                (SOC_IS_QAX(unit) ? 127 : 128)
#define SOC_SAT_ADD_CRC_OFFSET                    (SOC_IS_QAX(unit) ? 128 : 129)
#define SOC_SAT_TIME_STAMP_OFFSET                 (SOC_IS_QAX(unit) ? 129 : 130)
#define SOC_SAT_USE_4_BYTE_PATTERN_OFFSET         (SOC_IS_QAX(unit) ? 145 : 146)
#define SOC_SAT_USE_PRBS_PAYLOAD_OFFSET           (SOC_IS_QAX(unit) ? 146 : 147)
#define SOC_SAT_USE_RPT_PTRN_PAYLOAD_OFFSET       (SOC_IS_QAX(unit) ? 147 : 148)


#define SAT_CTF_IDENTIFIER_TO_INDEX(session_id, trap_id, color, tc) \
    (((session_id) << 7) | ((trap_id) << 5) | ((color) << 3) | (tc))

typedef enum soc_sat_rx_flv_bin_limit_s {
    socSATRxFLVBinLimit0   = 0,
    socSATRxFLVBinLimit1   = 1,
    socSATRxFLVBinLimit2   = 2,
    socSATRxFLVBinLimit3   = 3,
    socSATRxFLVBinLimit4   = 4,
    socSATRxFLVBinLimit5   = 5,
    socSATRxFLVBinLimit6   = 6,
    socSATRxFLVBinLimit7   = 7,
    socSATRxFLVBinLimit8   = 8,
    socSATRxFLVBinLimitCnt = 9
}soc_sat_rx_flv_bin_limit_s;

typedef enum soc_sat_stamp_type_s {
	socSatStampInvalid      = 0,
	socSatStampConstant2Bit = 1,
	socSatStampCounter8Bit  = 2,
	socSatStampCounter16Bit = 3,
	socSatStampConstant16Bit = 4,
	socSatStampCount
} soc_sat_stamp_type_t;

typedef enum soc_sat_payload_type_e {
	socSatPayloadConstant8Bytes = 0,
	socSatPayloadConstant4Bytes = 1,
	socSatPayloadPRBS           = 2,
	socSatPayloadsCount
} soc_sat_payload_type_t; 

typedef enum soc_sat_header_type_e {
    socSatHeaderUserDefined = 0,
    socSatHeaderMEF         = 1,
    socSatHeaderY1564       = 2,
    socSatHeadersCount 
} soc_sat_header_type_t;

typedef enum soc_sat_stamp_field_s {
	socSatStampFieldUserDefined = 0,
	socSatStampFieldTc          = 1, 
	socSatStampFieldDp          = 2, 
	socSatStampFieldSrcMac      = 3,
	socSatStampFieldDestMac     = 4,
	socSatStampFieldInnerVlan   = 5,
	socSatStampFieldOuterVlan   = 6,
	socSatStampFieldsCount
} soc_sat_stamp_field_t;

typedef enum soc_sat_gtf_rate_pattern_mode_e {
	socSatGtfRatePatternContinues   = 0,
	socSatGtfRatePatternSimpleBurst = 1,
	socSatGtfRatePatternCombined    = 2,
	socSatGtfRatePatternInterval    = 3,
	socSatGtfRatePatternsCount
} soc_sat_gtf_rate_pattern_mode_t; 

typedef enum soc_sat_gtf_stat_counter_e {
	socSatGtfStatPacketCount = 0,
	socSatGtfStatCount
} soc_sat_gtf_stat_counter_t;

/* SAT Timestamp Formats */
typedef enum soc_sat_timestamp_format_e {
    socSATTimestampFormatIEEE1588v1 = 0x00, /* Low-order 64 bits of the IEEE
                                           1588-2008 (1588v2) Precision Time
                                           Protocol timestamp format [IEEE1588] */
    socSATTimestampFormatNTP = 0x01      /* Network Time Protocol version 4
                                           64-bit timestamp format [RFC5905] */
} soc_sat_timestamp_format_t;

typedef struct soc_sat_init_s {
	uint32 cmic_tod_mode;
	uint32 rate_num_clks_cycle;
   	uint32 num_clks_sec;
} soc_sat_init_t;

/* Sat common configuration Object */
typedef struct soc_sat_config_s {
    soc_sat_timestamp_format_t timestamp_format; /* SAT timestamp format */
    uint32 config_flags; 
} soc_sat_config_t;

/* GTF */
typedef int soc_sat_gtf_t;

typedef struct soc_sat_header_user_define_offsets_s {
    int payload_offset; 
    int timestamp_offset; 
    int seq_number_offset;
    int crc_byte_offset;
} soc_sat_header_user_define_offsets_t;

typedef struct soc_sat_stamp_s {
	soc_sat_stamp_type_t stamp_type;
	soc_sat_stamp_field_t field_type; 
	uint32 inc_step; /* for counter stamp */
	uint32 inc_period_packets; /* for counter stamp */
	uint32 value; /* for constant stamp */
	uint32 offset; /* offset in bits, for user defined field type */
} soc_sat_stamp_t;

typedef struct soc_sat_payload_s {
	soc_sat_payload_type_t payload_type;
	uint8	              payload_pattern [SOC_SAT_PAYLOAD_MAX_PATTERN_NOF_U8];
} soc_sat_payload_t;

typedef struct soc_sat_gtf_packet_edit_s {
	uint32 packet_length[SOC_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS];
	uint32 packet_length_pattern[SOC_SAT_GTF_PACKET_MAX_PATTERN_LENGTH]; 
	uint32 pattern_length;
	soc_sat_stamp_t stamps[SOC_SAT_GTF_MAX_STAMPS];
	uint32 number_of_stamps;
	uint32 number_of_ctfs; 
	uint32 flags;
} soc_sat_gtf_packet_edit_t;

typedef struct soc_sat_gtf_packet_config_s {
	soc_sat_header_type_t sat_header_type;
    bcm_pkt_t header_info;
	soc_sat_payload_t payload;
    soc_sat_gtf_packet_edit_t packet_edit[SOC_SAT_GTF_NUM_OF_PRIORITIES];
	uint32 packet_context_id;
    soc_sat_header_user_define_offsets_t offsets;
}soc_sat_gtf_packet_config_t;


typedef struct soc_sat_gtf_bandwidth_s {
    uint32 flags;
	uint32 rate;
	uint32 max_burst;
	uint32 granularity;
} soc_sat_gtf_bandwidth_t; 

typedef struct  soc_sat_gtf_rate_pattern_s {
	soc_sat_gtf_rate_pattern_mode_t rate_pattern_mode;
	uint32 high_threshold;
	uint32 low_threshold;
	uint32 stop_iter_count; 
	uint32 stop_burst_count;
	uint32 stop_interval_count;
	uint32 burst_packet_weight;
	uint32 interval_packet_weight;
	uint32 flags;
} soc_sat_gtf_rate_pattern_t;

/* CTF */
typedef int soc_sat_ctf_t;

typedef struct soc_sat_ctf_packet_info_s {
	soc_sat_header_type_t sat_header_type; 
	soc_sat_payload_t payload;
	uint32 flags;
	soc_sat_header_user_define_offsets_t offsets; /* for user defined only */
} soc_sat_ctf_packet_info_t;

typedef struct soc_sat_ctf_identifier_s {
	uint32 tc; 
	uint32 color;  
	uint32 trap_id;
	uint32 session_id;
} soc_sat_ctf_identifier_t;

typedef struct soc_sat_ctf_bin_limit_s {
	uint32 bin_select; /* 0-8 in Jericho*/
	uint32 bin_limit;
} soc_sat_ctf_bin_limit_t;

typedef struct soc_sat_ctf_stat_config_s {
	uint32 use_global_bin_config;
	uint32 bin_min_delay;
	uint32 bin_step;
	uint32 update_counters_in_unvavail_state;
} soc_sat_ctf_stat_config_t; 

typedef struct soc_sat_ctf_stat_s {
	uint64 received_packet_cnt;
	uint64 out_of_order_packet_cnt;
	uint64 err_packet_cnt;
	uint64 received_octet_cnt;
	uint64 out_of_order_octet_cnt;
	uint64 err_octet_cnt;
	uint64 last_packet_delay;
	uint64 bin_delay_counters[SOC_SAT_STAT_MAX_NUM_OF_BINS];
	uint64 accumulated_delay_count;
	uint64 max_packet_delay;
	uint64 min_packet_delay;
	uint64 time_of_day_stamp;
	uint64 sec_in_avail_state_counter;
        uint64 last_packet_seq_number;      /* Last received packet sequence number */
} soc_sat_ctf_stat_t; 

typedef struct  soc_sat_ctf_availability_config_s {
	uint32 switch_state_num_of_slots;
	uint32 switch_state_threshold_per_slot;
} soc_sat_ctf_availability_config_t ;

typedef struct soc_sat_ctf_report_config_s {
	uint32 report_sampling_percent; 
	uint32 flags;
} soc_sat_ctf_report_config_t;

typedef struct soc_sat_report_event_data_s {
	int sequence_number;
	int sequence_number_valid;
	int delay;
	int delay_valid;
} soc_sat_report_event_data_t;

typedef int (*soc_sat_handler_t)(int unit);

/* Functions */
extern soc_error_t soc_sat_config_set  (
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_sat_config_t* conf
    );

extern soc_error_t soc_sat_config_get  (
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT soc_sat_config_t* conf
    );

extern soc_error_t soc_sat_general_cfg_init (
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_sat_init_t *sat_init
    );

extern soc_error_t soc_sat_gtf_packet_gen_set (
    SOC_SAND_IN int unit, 
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
    SOC_SAND_IN int priority,
	SOC_SAND_IN int enable
    );

extern soc_error_t soc_sat_gtf_packet_gen_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_sat_gtf_t gtf_id,
    SOC_SAND_IN int priority,
    SOC_SAND_OUT int *enable
    );

extern soc_error_t soc_sat_gtf_packet_config (
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_sat_gtf_t gtf_id, 
    SOC_SAND_IN soc_sat_gtf_packet_config_t *config
    );

extern soc_error_t soc_sat_gtf_bandwidth_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_sat_gtf_t gtf_id,	 
    SOC_SAND_IN int priority,
    SOC_SAND_IN soc_sat_gtf_bandwidth_t *bw
    );

extern soc_error_t soc_sat_gtf_bandwidth_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_sat_gtf_t gtf_id,	 
    SOC_SAND_IN int priority,
    SOC_SAND_OUT soc_sat_gtf_bandwidth_t *bw
    );

extern soc_error_t soc_sat_gtf_rate_pattern_set (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
	SOC_SAND_IN int priority,
	SOC_SAND_IN soc_sat_gtf_rate_pattern_t *config
    );

extern soc_error_t soc_sat_gtf_rate_pattern_get (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
	SOC_SAND_IN int priority,
	SOC_SAND_OUT soc_sat_gtf_rate_pattern_t *config
    );

extern soc_error_t soc_sat_gtf_stat_get (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_gtf_t gtf_id,
	SOC_SAND_IN int priority,
	SOC_SAND_IN uint32 flags,
	SOC_SAND_IN soc_sat_gtf_stat_counter_t type,
	SOC_SAND_OUT uint64* value
    );

extern soc_error_t soc_sat_ctf_packet_config (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_packet_info_t *packet_info
    );

extern soc_error_t soc_sat_ctf_identifier_map (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_identifier_t *identifier,
	SOC_SAND_IN soc_sat_ctf_t ctf_id
    );

extern soc_error_t soc_sat_ctf_identifier_unmap (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_identifier_t *identifier
    );

extern soc_error_t soc_sat_ctf_trap_set (
	SOC_SAND_IN int unit,
	SOC_SAND_IN uint32 trap_id_cnt,
	SOC_SAND_IN uint32* trap_id
);

extern soc_error_t soc_sat_ctf_bin_limit_set (
	SOC_SAND_IN int unit, 
	SOC_SAND_IN int bins_count, 
 	SOC_SAND_IN soc_sat_ctf_bin_limit_t* bins
    );

extern soc_error_t soc_sat_ctf_bin_limit_get (
	SOC_SAND_IN  int unit, 
	SOC_SAND_IN  int max_bins_count,
	SOC_SAND_OUT int * bins_count, 
 	SOC_SAND_OUT soc_sat_ctf_bin_limit_t* bins
    );

extern soc_error_t soc_sat_ctf_stat_config_set (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_stat_config_t *stat
    );

extern soc_error_t soc_sat_ctf_stat_get (
	SOC_SAND_IN int unit, 
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN uint32 flags,
	SOC_SAND_OUT soc_sat_ctf_stat_t* stat
    );

extern soc_error_t soc_sat_ctf_availability_config_set (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_availability_config_t *config
	);

extern soc_error_t soc_sat_ctf_trap_data_to_session_map (
	SOC_SAND_IN int unit,
	SOC_SAND_IN uint32 trap_data, 
	SOC_SAND_IN uint32 trap_data_mask,
    SOC_SAND_IN uint32 entry_idx,
	SOC_SAND_IN uint32 session_id
    );

extern soc_error_t soc_sat_ctf_trap_data_to_session_unmap (
	SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 entry_idx
    ); 

extern soc_error_t soc_sat_ctf_reports_config_set (
	SOC_SAND_IN int unit,
	SOC_SAND_IN soc_sat_ctf_t ctf_id,
	SOC_SAND_IN soc_sat_ctf_report_config_t *reports
    );

extern soc_error_t soc_sat_oamp_stat_event_control(
	SOC_SAND_IN int unit,
	SOC_SAND_IN int enable);

extern soc_error_t soc_sat_oamp_stat_event_clear(
	SOC_SAND_IN int unit);

extern soc_error_t soc_common_sat_handler_register(
    SOC_SAND_IN int unit,
    SOC_SAND_IN soc_sat_handler_t handler);

extern soc_error_t soc_common_sat_handler_get(
    SOC_SAND_IN int unit,
    SOC_SAND_OUT soc_sat_handler_t *handler);

extern int soc_sat_session_inuse(
    SOC_SAND_IN int unit,
	SOC_SAND_IN int session_id);

#ifdef BCM_WARM_BOOT_SUPPORT
int soc_sat_wb_init(int unit);
int soc_sat_wb_sync(int unit, int sync);
int soc_sat_wb_deinit(int unit);
#endif

#endif /* _SOC_SHARED_SAT_H */
