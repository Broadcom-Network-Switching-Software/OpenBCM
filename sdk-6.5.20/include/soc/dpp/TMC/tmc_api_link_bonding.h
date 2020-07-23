/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_LINK_BONDING_INCLUDED__

#define __SOC_TMC_API_LINK_BONDING_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/pkt.h>

#include <bcm/lb.h>










#define SOC_TMC_LB_GLOBAL                            0xFFFFFFF
#define SOC_TMC_LB_LBG_INVALID                       (SOC_TMC_LB_GLOBAL-1)
#define SOC_TMC_LB_LBG_MAX                            5
#define SOC_TMC_LB_NOF_LBG                            6

#define SOC_TMC_LB_MODEM_MAX                         15
#define SOC_TMC_LB_NOF_MODEM                         16
#define SOC_TMC_LB_MODEM_INVALID                     0xFFFFFFF

#define SOC_TMC_LB_BASE_Q_PAIR_MIN                    0
#define SOC_TMC_LB_BASE_Q_PAIR_MAX                    47 

#define SOC_TMC_LB_ING_NOF_TOTAL_SHARED_BUFFER_MAX    16384
#define SOC_TMC_LB_ING_NOF_GUARANTEED_BUFFER_MAX      16384

#define SOC_TMC_LB_ING_SEQUENCE_NUM_WIDTH_MIN         7
#define SOC_TMC_LB_ING_SEQUENCE_NUM_WIDTH_MAX        14

#define SOC_TMC_LB_EGR_SEQUENCE_NUM_WIDTH_MIN         1
#define SOC_TMC_LB_EGR_SEQUENCE_NUM_WIDTH_MAX        20

#define SOC_TMC_LB_ING_PKT_TO_MODEM_VLAN_MAX         0xf
#define SOC_TMC_LB_ING_NOF_VLAN_IN_PKT_TO_MODEM      16

#define SOC_TMC_LB_EGR_DP_MAX                         1

#define SOC_TMC_LB_EGR_NOF_SCHEDULER_ENTRY           16

#define SOC_TMC_LB_EGR_MODEM_SHAPER_RATE_GRANULARITY(unit)    (SOC_DPP_DEFS_GET(unit, granularity_modem_bit_shaper_rate))
#define SOC_TMC_LB_EGR_MODEM_SHAPER_RATE_MAX                  10000000
#define SOC_TMC_LB_EGR_MODEM_SHAPER_BURST_MAX                 0x3fff

#define SOC_TMC_LB_EGR_MODEM_SEGMENT_SHAPER_RATE_GRANULARITY  3815
#define SOC_TMC_LB_EGR_MODEM_SEGMENT_SHAPER_RATE_MAX          (0xffff*3815)
#define SOC_TMC_LB_EGR_MODEM_SEGMENT_SHAPER_BURST_MAX         0xff

#define SOC_TMC_LB_EGR_MODEM_SHAPER_HDR_COMPENSATION_MIN     -63
#define SOC_TMC_LB_EGR_MODEM_SHAPER_HDR_COMPENSATION_MAX      63


#define SOC_TMC_LB_LBG_SPEED_MAX                              30000

#define LB_EGR_NOF_FIFO_RANGE_MAX                             (SOC_TMC_LB_NOF_LBG*2+1)

#define SOC_TMC_LB_VALUE_MAX_CHECK(val, max, para_name) {\
    if ((val) > (max)) { \
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, \
            (_BSL_SOCDNX_MSG("Fail(%s) parameter(%s=%d) should be less than %d\n"), \
                             soc_errmsg(BCM_E_PARAM), para_name, val, (max+1)));\
    } \
}

#define SOC_TMC_LB_VALUE_CHECK(val, min, max, para_name) {\
    if ((val) < (min) || (val) > (max)) { \
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, \
            (_BSL_SOCDNX_MSG("Fail(%s) parameter(%s=%d) should be in range of [%d, %d]\n"), \
                             soc_errmsg(BCM_E_PARAM), para_name, val, min, max));\
    }\
}





typedef uint32 soc_lbg_t;
typedef uint32 soc_modem_t;

typedef enum soc_lb_format_type_e {
    socLbgFormatTypeBypass = 0,
    socLbgFormatTypeNonChannelize = 1,
    socLbgFormatTypeChannelize = 2,
    socLbgFormatTrafficCount = 3
} soc_lb_format_type_t;

typedef struct soc_lb_ing_reorder_config_s {
    uint32 timeout_thresh;
    uint32 max_out_of_order;
    uint32 max_buffer;
} soc_lb_ing_reorder_config_t;


typedef enum soc_lb_flush_type_e {
    socLbFlushTypeForceFlush = 0,       
    socLbFlushTypeFlushOnGroupOverflow = 1, 
    socLbFlushTypeFlushOnFifoOverflow = 2, 
    socLbFlushTypeFlushOnSharedOverflow = 3, 
    socLbFlushTypeCount = 4             
} soc_lb_flush_type_t;


typedef struct soc_lb_rx_modem_map_index_s {
    soc_port_t port; 
    bcm_vlan_t vlan; 
} soc_lb_rx_modem_map_index_t;


typedef struct soc_lb_rx_modem_map_config_s {
    soc_lb_format_type_t lbg_type; 
    soc_modem_t modem_id; 
} soc_lb_rx_modem_map_config_t;

typedef enum soc_lb_status_type_e {
    socLbStatusIsInSync = 0,           
    socLbStatusIsEmptyFifo = 1,
    socLbStatusCount = 2
} soc_lb_status_type_t;

typedef enum soc_lb_modem_status_type_e {
    socLbModemStatusIsEmptyFifo = 0,    
    socLbModemStatusCount = 1           
} soc_lb_modem_status_type_t;

typedef enum soc_lb_stat_val_e {
      socLbStatsUnicastPkts = 0,
      socLbStatsMulticastPkts = 1,
      socLbStatsBroadcastPkts = 2,
      socLbStatsPkts64Octets = 3,
      socLbStatsPkts65to127Octets = 4,
      socLbStatsPkts128to255Octets = 5,
      socLbStatsPkts256to511Octets = 6,
      socLbStatsPkts512to1023Octets = 7,
      socLbStatsPkts1024to1518Octets = 8,
      socLbStatsPkts1519Octets = 9,
      socLbStatsPkts = 10,
      socLbStatsOctets = 11,
      socLbStatsDiscardFragments = 12,
      socLbStatsErrorPkts = 13,
      socLbStatsCount = 14
} soc_lb_stat_val_t;

typedef enum soc_lb_modem_stat_val_e {
      socLbModemStatsPkts = 0,
      socLbModemStatsOctets = 1,
      socLbModemStatsCount = 2
} soc_lb_modem_stat_val_t;

typedef struct soc_lb_global_packet_config_s {
    
    uint16 outer_vlan_tpid;
    
    uint16 lb_tpid;
    
    uint32 packet_crc_enable;
} soc_lb_global_packet_config_t;

typedef struct soc_lb_lbg_packet_config_s {
    
    uint32 packet_crc_enable;
} soc_lb_lbg_packet_config_t;

typedef struct soc_lb_modem_packet_config_s {
    soc_lb_format_type_t pkt_format;
    soc_mac_t dst_mac;
    soc_mac_t src_mac;
    
    uint16 vlan;
    int use_global_priority_map;
} soc_lb_modem_packet_config_t;

typedef struct soc_lb_tc_dp_s {
    uint32 tc;
    uint32 dp;
} soc_lb_tc_dp_t;

typedef struct soc_lb_pkt_pri_s {
    uint32 pkt_pri;
    uint32 pkt_cfi;
    uint32 pkt_dp;
} soc_lb_pkt_pri_t;

typedef struct soc_lb_lbg_weight_s {
    soc_lbg_t lbg_id;
    uint32    lbg_weight;
} soc_lb_lbg_weight_t;

typedef enum soc_lb_segment_mode_e {
      socLbSegmentMode128 = 0,
      socLbSegmentMode192 = 1,
      socLbSegmentMode192Enhanced = 2,
      socLbSegmentMode128Enhanced = 3,
      socLbSegmentModeCount = 4
} soc_lb_segment_mode_t;

#define _SOC_LB_SEGMENT_MODE_MSG_INIT        {                            \
        "128",                                    \
        "192",                                    \
        "192Enhanced",                    \
        "128Enhanced",                    \
        "Unknown"                               \
        }

typedef struct soc_lb_modem_shaper_config_s {
    
    uint32 enable;
    
    uint32 rate;
        
    uint32 max_burst;
             
    int32 hdr_compensation;
} soc_lb_modem_shaper_config_t;

typedef struct soc_lb_modem_segment_shaper_config_s {
    
    uint32 enable;
    
    uint32 rate;
        
    uint32 max_burst;
} soc_lb_modem_segment_shaper_config_t;

typedef struct {
    
    SHR_BITDCL   *lb_egr_if_bmp_used; 
    uint64        lb_ing_modem_fragment_cnt[SOC_TMC_LB_NOF_MODEM];
    uint64        lb_ing_modem_byte_cnt[SOC_TMC_LB_NOF_MODEM];
    int           lb_egr_lbg_fifo_start[LB_EGR_NOF_FIFO_RANGE_MAX];
    int           lb_egr_lbg_fifo_nof_entries[LB_EGR_NOF_FIFO_RANGE_MAX];
} soc_lb_info_t;


typedef enum soc_lb_obj_e {
    socLbObjAll    = 0,
    socLbObjGlobal = 1,
    socLbObjLbg    = 2,
    socLbObjModem  = 3,
    socLbObjCount  = 4
} soc_lb_obj_t;

typedef enum soc_lb_direction_type_e {
    socLbDirectionRx    = 0,
    socLbDirectionTx    = 1,
    socLbDirectionBoth  = 2,
    socLbDirectionCount = 3
} soc_lb_direction_type_t;

typedef struct soc_lb_info_key_s {

    soc_lb_direction_type_t direction;

    soc_lb_obj_t obj_type;

    soc_lbg_t    lbg_id;

    soc_modem_t  modem_id;

} soc_lb_info_key_t;

typedef struct soc_lb_cfg_rx_glb_info_s {

    uint32 buffer_size;

    int seq_num_width;

    int nof_total_shared_buffer;           

    int single_buffer_size;                
} soc_lb_cfg_rx_glb_info_t;

typedef struct soc_lb_cfg_tx_glb_info_s {

    soc_lb_segment_mode_t segment_mode;    

    uint16                vlan_tpid;

    uint16                lbg_tpid;

    uint32                is_pkt_crc_ena;  

    soc_lb_pkt_pri_t      pkt_pri[SOC_TMC_NOF_TRAFFIC_CLASSES][SOC_TMC_NOF_DROP_PRECEDENCE];
} soc_lb_cfg_tx_glb_info_t;

typedef struct soc_lb_cfg_rx_lbg_info_s {

    int        modem_bmp;

    soc_port_t logic_port;

    uint32     ptc;

    uint32     timeout;

    uint32     max_out_of_order;

    uint32     max_buffer;

    int        nof_guaranteed_buffer;       
} soc_lb_cfg_rx_lbg_info_t;

typedef struct soc_lb_cfg_tx_lbg_info_s {

    uint32     is_enable;

    int        modem_bmp;

    soc_port_t logic_port;

    uint32     fifo_start;

    uint32     fifo_end;

    int        seq_num_width;

    uint32     lbg_weight;

    soc_lb_segment_mode_t segment_mode;  

    uint32                is_pkt_crc_ena;  

} soc_lb_cfg_tx_lbg_info_t;

typedef struct soc_lb_cfg_rx_modem_info_s {

    soc_port_t logic_port;

    uint32     vlan_bmp;

    soc_lbg_t  lbg_id;

    uint32     fifo_start;

    uint32     fifo_size;
} soc_lb_cfg_rx_modem_info_t;

typedef struct soc_lb_cfg_tx_modem_info_s {
    soc_lbg_t  lbg_id;

    soc_lb_modem_packet_config_t modem_packet;

    soc_lb_modem_shaper_config_t modem_shaper;

    uint32 egr_intf;

    soc_port_t logic_port;
} soc_lb_cfg_tx_modem_info_t;

typedef struct soc_lb_cfg_rx_info_s {

    soc_lb_cfg_rx_glb_info_t   rx_glb_info;

    soc_lb_cfg_rx_lbg_info_t   rx_lbg_info[SOC_TMC_LB_NOF_LBG];

    soc_lb_cfg_rx_modem_info_t rx_modem_info[SOC_TMC_LB_NOF_MODEM];
} soc_lb_cfg_rx_info_t;

typedef struct soc_lb_cfg_tx_info_s {

    soc_lb_cfg_tx_glb_info_t   tx_glb_info;

    soc_lb_cfg_tx_lbg_info_t   tx_lbg_info[SOC_TMC_LB_NOF_LBG];

    soc_lb_cfg_tx_modem_info_t tx_modem_info[SOC_TMC_LB_NOF_MODEM];
} soc_lb_cfg_tx_info_t;

typedef struct soc_lb_cfg_info_s {

    soc_lb_cfg_rx_info_t rx_cfg_info;

    soc_lb_cfg_tx_info_t tx_cfg_info;
} soc_lb_cfg_info_t;















#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

