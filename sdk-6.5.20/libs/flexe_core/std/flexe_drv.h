#ifndef _FLEXE_DRV_H
#define _FLEXE_DRV_H

#include <shared/bitop.h>
#include <shared/shrextend/shrextend_error.h>

#define FLEXE_CORE_NOF_PORTS 8
#define FLEXE_CORE_NOF_INSTANCES 8
#define FLEXE_CORE_NOF_INSTANCES_DIV2 4
#define FLEXE_CORE_NOF_LPHYS 8

#define FLEXE_CORE_NOF_TIMESLOTS 80
#define FLEXE_NOF_TIMESLOTS_PER_INSTANCE 20
#define FLEXE_CORE_NOF_SUBCALS 10
#define FLEXE_SAR_NOF_TIMESLOTS 80
#define FLEXE_TINYMAC_NOF_TIMESLOTS 81

#define FLEXE_CORE_NOF_CHANNELS 80
#define FLEXE_CORE_MAX_NOF_CHANNELS (FLEXE_CORE_NOF_CHANNELS+2)

#define FLEXE_INVALID_GROUP_ID -1
#define FLEXE_INVALID_PORT_ID 0xF
#define FLEXE_INVALID_CHANNEL_ID 0x7F
#define FLEXE_INVALID_66SW_CHANNEL_ID 0xFF
#define FLEXE_INVALID_SAR_CHANNEL_ID 0x7F
#define FLEXE_INVALID_TINYMAC_CHANNEL_ID 0x7F
#define FLEXE_INVALID_ILKN_CHANNEL_ID 0x200

#define FLEXE_TINYMAC_OH_CHANNEL_ID 80
#define FLEXE_TINYMAC_1588_CHANNEL_ID 81

#define FLEXE_CORE_NOF_CALENDAR 2
#define FLEXE_66BSWITCH_NOF_SUBSYS 3

#define FLEXE_REG_ACCESS_MAX_RETRIES 100

#define FLEXE_MTU 9600
#define FLEXE_RATEADP_MAX_NOF_TIMESLOTS 16

#define FLEXE_OHIF_NOF_OAM_TYPES 64

#define FLEXE_PHY_SPEED_100G 100000

#define FLEXE_GROUP_IS_VALID(_grp) ((_grp >=0) && (_grp<FLEXE_CORE_NOF_PORTS))

#define FLEXE_PORT_SPEED_TO_RATE(_speed) \
    (_speed/FLEXE_PHY_SPEED_100G == 4)?FLEXE_DRV_PORT_SPEED_400G:(flexe_drv_port_speed_mode_e)(_speed/FLEXE_PHY_SPEED_100G)

#define FLEXE_MAX_LOGICAL_PHY_ID 0xFF

#define FLEXE_CORE_PCS_SEL_DIV 2


typedef enum
{
    FLEXE_ENV_50G,
    FLEXE_ENV_100G,
    FLEXE_ENV_200G,
    FLEXE_ENV_400G,
    FLEXE_ENV_50G_BYPASS,
    FLEXE_ENV_100G_BYPASS,
    FLEXE_ENV_200G_BYPASS,
    FLEXE_ENV_400G_BYPASS,
    FLEXE_ENV_50G_LOCAL,
    FLEXE_ENV_100G_LOCAL,
    FLEXE_ENV_200G_LOCAL,
    FLEXE_ENV_400G_LOCAL,
    FLEXE_ENV_50G_INSTANCE_LOCAL,
} flexe_env_mode_e;

#define SOC_PORT_FLEXE_RX                (0x1)
#define SOC_PORT_FLEXE_TX                (0x2)

typedef enum
{
    FLEXE_DRV_SERDES_RATE_25G = 0,
    FLEXE_DRV_SERDES_RATE_26G,
    FLEXE_DRV_SERDES_RATE_53G,
    FLEXE_DRV_SERDES_RATE_COUNT,
} flexe_drv_serdes_rate_mode_e;


typedef enum
{
    FLEXE_DRV_PORT_SPEED_50G = 0,
    FLEXE_DRV_PORT_SPEED_100G,
    FLEXE_DRV_PORT_SPEED_200G,
    FLEXE_DRV_PORT_SPEED_400G,
    FLEXE_DRV_PORT_SPEED_COUNT
} flexe_drv_port_speed_mode_e;

#ifndef MIN
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a)>(b)) ? (a) : (b))
#endif

#define FLEXE_PHY_NOF_INSTANCES(_speed) \
    MAX(_speed/FLEXE_PHY_SPEED_100G, 1)

#define FLEXE_PHY_NOF_LPHYS(_speed) \
    ((_speed)/50000)


typedef enum
{
    
    FLEXE_DRV_SAR_LOOPBACK_REMOTE,
    
    FLEXE_DRV_SAR_LOOPBACK_RX,
    
    FLEXE_DRV_SAR_LOOPBACK_TX,
} flexe_drv_sar_loopback_mode_e;

#ifdef BE_HOST

#define SHR_BITCOPY_RANGE_U32_TO_U64(_dest, _dest_offset,_src, _src_offset, _num_bits) \
    soc_bitop_range_copy_u32_to_u64_big_endian(_dest, _dest_offset,_src, _src_offset, _num_bits)

#define SHR_BITCOPY_RANGE_U64_TO_U32(_dest, _dest_offset,_src, _src_offset, _num_bits) \
        soc_bitop_range_copy_u64_to_u32_big_endian(_dest, _dest_offset,_src, _src_offset, _num_bits)
#else

#define SHR_BITCOPY_RANGE_U32_TO_U64(_dest, _dest_offset,_src, _src_offset, _num_bits) \
    SHR_BITCOPY_RANGE((SHR_BITDCL *)_dest, _dest_offset,_src, _src_offset, _num_bits)

#define SHR_BITCOPY_RANGE_U64_TO_U32(_dest, _dest_offset,_src, _src_offset, _num_bits) \
        SHR_BITCOPY_RANGE(_dest, _dest_offset,(SHR_BITDCL *)_src, _src_offset, _num_bits)
#endif

#define FLEXE_PHY_SEL_CFG_SET(_cfg, _idx, _val) \
    SHR_BITCOPY_RANGE(_cfg, ((_idx)*4), _val, 0, 4)

#define FLEXE_PHY_SEL_CFG_GET(_cfg, _idx, _val) \
    SHR_BITCOPY_RANGE(_val, 0, _cfg, ((_idx)*4), 4)

#define FLEXE_RX_MAP_TABLE_SET(_map, _bcm_port, _flexe_port) \
    SHR_BITCOPY_RANGE(_map, ((_bcm_port)*4), _flexe_port, 0, 4)

#define FLEXE_RX_MAP_TABLE_GET(_map, _bcm_port, _flexe_port) \
    SHR_BITCOPY_RANGE(_flexe_port, 0, _map, ((_bcm_port)*4), 4)

#define FLEXE_RX_LF_SEL_SET(_lf_sel, _flexe_port, _bcm_port) \
    SHR_BITCOPY_RANGE(_lf_sel, ((_flexe_port)*3), _bcm_port, 0, 3)

#define FLEXE_RX_LF_SEL_GET(_lf_sel, _flexe_port, _bcm_port) \
        SHR_BITCOPY_RANGE(_bcm_port, 0, _lf_sel, ((_flexe_port)*3), 3)

#define FLEXE_RX_MODE_IS_200G_SET(_is_200g, _flexe_port, _val) \
    SHR_BITCOPY_RANGE(_is_200g, _flexe_port/2, _val, 0, 1)

#define FLEXE_TX_MODE_IS_BYPASS_SET(_is_bypass, _flexe_port, _val) \
    SHR_BITCOPY_RANGE(_is_bypass, _flexe_port, _val, 0, 1)

#define FLEXE_TX_MAP_TABLE_SET(_map, _bcm_port, _flexe_port) \
    FLEXE_RX_MAP_TABLE_SET(_map, _bcm_port, _flexe_port)

#define FLEXE_TX_MAP_TABLE_GET(_map, _bcm_port, _flexe_port) \
    FLEXE_RX_MAP_TABLE_GET(_map, _bcm_port, _flexe_port)

#define FLEXE_TX_MODE_IS_200G_SET(_is_200g, _flexe_port, _val) \
    FLEXE_RX_MODE_IS_200G_SET(_is_200g, _flexe_port, _val)

#define FLEXE_CPB_EGR_BP_LEVEL_OFFSET 80
#define FLEXE_CPB_EGR_RD_LEVEL_BASE   56
#define FLEXE_CPB_EGR_HIGH_DEPTH_BASE 64
#define FLEXE_CPB_EGR_LOW_DEPTH_BASE  48

#define FSCL_NEXT_BLK_TABLE_DATA(_last, _blk, _next) (((_last) << 14) | ((_blk) << 7) | (_next))

#define FLEXE_DEMUX_REORDER_SRC_INVALID 0xF0
#define FLEXE_DEMUX_NOF_TIMESLOTS_PER_TABLE 8

#define FLEXE_DEMUX_RAM_INVALID_ROW 0xF
#define FLEXE_DEMUX_RDRULE_OFFSET(_channel, _timeslot) ((_channel) * FLEXE_CORE_NOF_TIMESLOTS + _timeslot)

#define FLEXE_DEMUX_GROUP_INFO_SET(_val, _phy, _min_phy) \
    SHR_BITCOPY_RANGE(_val, _phy*4, _min_phy, 0, 4)

#define FLEXE_DEMUX_GROUP_INFO_GET(_val, _phy, _min_phy) \
    SHR_BITCOPY_RANGE(_min_phy, 0, _val, _phy*4, 4)


#define FLEXE_DEMUX_REORDER_INFO_SET(_val, _lphy, _src) \
    SHR_BITCOPY_RANGE_U32_TO_U64(_val, ((_lphy)*8), _src, 0, 8)


#define FLEXE_DEMUX_REORDER_INFO_GET(_val, _lphy, _dest) \
    SHR_BITCOPY_RANGE_U64_TO_U32(_dest, 0, _val, ((_lphy)*8), 8)


#define FLEXE_DEMUX_GROUP_CFG_SET(_val, _lphy, _min_lphy) \
    SHR_BITCOPY_RANGE_U32_TO_U64(_val, ((_lphy)*4), _min_lphy, 0, 4)


#define FLEXE_DEMUX_GROUP_CFG_GET(_val, _lphy, _min_lphy) \
    SHR_BITCOPY_RANGE_U64_TO_U32(_min_lphy, 0, _val, ((_lphy)*4), 4)

#define FLEXE_DEMUX_CALENDAR_CFG_SET(_val, _timeslot, _chanel) \
    SHR_BITCOPY_RANGE(_val, ((_timeslot)*8), _chanel, 0, 8)

#define FLEXE_DEMUX_CALENDAR_CFG_GET(_val, _timeslot, _chanel) \
    SHR_BITCOPY_RANGE(_chanel, 0, _val, ((_timeslot)*8), 8)

#define FLEXE_DEMUX_CHANNEL_OWN_TSGROUP_SET(_val, _chanel, _lphymap) \
    SHR_BITCOPY_RANGE(_val, ((_chanel)*8), _lphymap, 0, 8)

#define FLEXE_DEMUX_CHANNEL_OWN_TSGROUP_GET(_val, _chanel, _lphymap) \
    SHR_BITCOPY_RANGE(_lphymap, 0, _val, ((_chanel)*8), 8)

#define FLEXE_DEMUX_RDRULE_SET(_val, _subcal, _timeslot) \
    SHR_BITCOPY_RANGE(_val, ((_subcal)*8), _timeslot, 0, 8)

#define FLEXE_DEMUX_RDRULE_GET(_val, _subcal, _timeslot) \
    SHR_BITCOPY_RANGE(_timeslot, 0, _val, ((_subcal)*8), 8)

#define FLEXE_DEMUX_TS_TO_WR_TBL_ID(_timeslot) \
    (((_timeslot)/40)*5 + (_timeslot) % 5)

#define FLEXE_DEMUX_TS_TO_WR_TBL_INDEX_BASE(_timeslot) \
    ((((_timeslot) % 40)/5)*10)

#define FLEXE_MAX_COMMON_DIVISOR_10(_x) \
    ((_x % 10 == 0)?10:((_x % 5 == 0)?5:((_x % 2 == 0)?2:1)))

#define FLEXE_MULTI_DESKEW_GRP_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, ((_idx%4)*8), _sel, 0, 8)

#define FLEXE_MULTI_DESKEW_GRP_CFG_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, ((_idx%4)*8), 8)


typedef enum
{
    FLEXE_MUX_MODE_50G = 0,
    FLEXE_MUX_MODE_100G = 1,
    FLEXE_MUX_MODE_100G_2ND_PHY = 2,
} flexe_mux_mode_e;

#define FLEXE_MUX_GROUP_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*4), _sel, 0, 4)

#define FLEXE_MUX_GROUP_CFG_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, ((_idx)*4), 4)

#define FLEXE_MUX_CHANNEL_BELONG_FLEXE_SET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_val, (_idx*4), _lphy, 0, 4)

#define FLEXE_MUX_CHANNEL_BELONG_FLEXE_GET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_lphy, 0, _val, (_idx*4), 4)

#define FLEXE_MUX_PCS_SCH_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*4), _sel, 0, 4)

#define FLEXE_MUX_PCS_SCH_CFG_GET(_reg_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _reg_val, ((_idx)*4), 4)

#define FLEXE_MUX_PCS_CHID_CFG_SET(_val, _idx, _ch) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*8), _ch, 0, 7)

#define FLEXE_MUX_MODE_SET(_val, _idx, _mode) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*2), _mode, 0, 2)

#define FLEXE_MUX_MODE_GET(_val, _idx, _mode) \
    SHR_BITCOPY_RANGE(_mode, 0, _val, ((_idx)*2), 2)

#define FLEXE_MUX_CPHY2_CFG_SET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*4), _lphy, 0, 3)

#define FLEXE_MUX_CPHY2_CFG_GET(_val, _idx, _lphy) \
    SHR_BITCOPY_RANGE(_lphy, 0, _val, ((_idx)*4), 3)

#define FLEXE_MUX_SCH_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*4), _sel, 0, 4)

#define FLEXE_MUX_MSI_CFG_SET(_val, _idx, _channel) \
    SHR_BITCOPY_RANGE(_val, (((_idx)/20)*8), _channel, 0, 7)

#define FLEXE_MUX_MSI_CFG_GET(_val, _idx, _channel) \
    SHR_BITCOPY_RANGE(_channel, 0, _val, (((_idx)/20)*8), 7)

#define FLEXE_MUX_TSNUM_CFG_SET(_val, _idx, _tsnum) \
    SHR_BITCOPY_RANGE(_val, (((_idx)/20)*8), _tsnum, 0, 7)

#define FLEXE_MUX_TSNUM_CFG_GET(_val, _idx, _tsnum) \
    SHR_BITCOPY_RANGE(_tsnum, 0, _val, (((_idx)/20)*8), 7)

#define FLEXE_MUX_CYCLE_CFG_SET(_val, _idx, _cycle) \
    SHR_BITCOPY_RANGE(_val, (((_idx)/20)*8), _cycle, 0, 7)

#define FLEXE_MUX_CYCLE_CFG_GET(_val, _idx, _cycle) \
    SHR_BITCOPY_RANGE(_cycle, 0, _val, (((_idx)/20)*8), 7)

#define FLEXE_MUX_BLK_CFG_SET(_val, _idx, _blk) \
    SHR_BITCOPY_RANGE(_val, (_idx*4), _blk, 0, 4)

#define FLEXE_MUX_BLK_CFG_GET(_val, _idx, _blk) \
    SHR_BITCOPY_RANGE(_blk, 0, _val, (_idx*4), 4)

#define FLEXE_MUX_EN_SEL_CFG(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, (_idx*4), _sel, 0, 3)

#define FLEXE_MUX_EN_SEL_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, (_idx*4), 3)

#define FLEXE_MUX_INST_SEL_CFG_SET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_val, (_idx*4), _sel, 0, 3)

#define FLEXE_MUX_INST_SEL_CFG_GET(_val, _idx, _sel) \
    SHR_BITCOPY_RANGE(_sel, 0, _val, (_idx*4), 3)


#define FLEXE_CHANNELIZE_PCS_CHID_CFG_SET(_val, _idx, _ch) \
    SHR_BITCOPY_RANGE_U32_TO_U64(_val, ((_idx)*8), _ch, 0, 7)

#define FLEXE_66SW_CHID_CFG_SET(_val, _idx, _ch) \
    SHR_BITCOPY_RANGE(_val, ((_idx)*8), _ch, 0, 7)


typedef enum
{
    
    FLEXE_DRV_CALENDAR_SEL_A,
    
    FLEXE_DRV_CALENDAR_SEL_B,
    
    FLEXE_DRV_CALENDAR_COUNT,
} flexe_drv_calendar_sel_e;

#define FLEXE_OP_A_OR_B(_sel, _a, _b, ...) \
    if (_sel == FLEXE_DRV_CALENDAR_SEL_A) \
    { \
        SHR_IF_ERR_EXIT(_a(__VA_ARGS__)); \
    } \
    else \
    { \
        SHR_IF_ERR_EXIT(_b(__VA_ARGS__)); \
    } \

uint32 soc_flexe_std_bitmap_find_first_set(
    SHR_BITDCL *bitmap,
    int nof_elements);

uint32 soc_flexe_std_bitmap_find_last_set(
    SHR_BITDCL *bitmap,
    int nof_elements);


int
soc_flexe_mux_timeslot_convert(
    int timeslot,
    int is_100g);

shr_error_e soc_flexe_std_mux_rateadp_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)]);


shr_error_e soc_flexe_std_mux_rateadp_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)]);


void soc_bitop_range_copy_u32_to_u64_big_endian(
    uint64 *u64_dest,
    int dest_offset,
    uint32 *src,
    int src_offset,
    int num_bits);


void soc_bitop_range_copy_u64_to_u32_big_endian(
    uint32 *dest,
    int dest_offset,
    uint64 *u64_src,
    int src_offset,
    int num_bits);


shr_error_e soc_flexe_std_init(
    int unit);


shr_error_e soc_flexe_std_rx_phy_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed,
    flexe_drv_serdes_rate_mode_e serdes_rate);


shr_error_e soc_flexe_std_rx_phy_del(
    int unit,
    int bcm_port,
    int speed);


shr_error_e soc_flexe_std_tx_phy_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed);


shr_error_e soc_flexe_std_tx_phy_del(
    int unit,
    int bcm_port,
    int speed);


shr_error_e soc_flexe_std_rx_phy_bypass_set(
    int unit,
    int flexe_port,
    int enable,
    int bypass_channel);


shr_error_e soc_flexe_std_tx_phy_bypass_set(
    int unit,
    int flexe_port,
    int speed,
    int enable,
    int bypass_channel);


shr_error_e soc_flexe_std_phy_add(
    int unit,
    int bcm_port,
    int flexe_port,
    int speed,
    flexe_drv_serdes_rate_mode_e serdes_rate);


shr_error_e soc_flexe_std_phy_del(
    int unit,
    int bcm_port,
    int speed);


shr_error_e soc_flexe_std_phy_bypass_set(
    int unit,
    int flexe_port,
    int speed,
    int enable,
    int bypass_channel);


shr_error_e soc_flexe_std_sar_rx_init(
    int unit);


shr_error_e soc_flexe_std_sar_tx_init(
    int unit);


shr_error_e soc_flexe_std_sar_init(
    int unit);


shr_error_e soc_flexe_std_sar_channel_rx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);


shr_error_e soc_flexe_std_sar_channel_rx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);


shr_error_e soc_flexe_std_sar_channel_tx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);


shr_error_e soc_flexe_std_sar_channel_tx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);


shr_error_e soc_flexe_std_sar_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);


shr_error_e soc_flexe_std_sar_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)]);


shr_error_e soc_flexe_std_sar_channel_map_rx_set(
    int unit,
    int channel,
    int ilkn_channel,
    int enable);


shr_error_e soc_flexe_std_sar_channel_map_tx_set(
    int unit,
    int channel,
    int ilkn_channel,
    int enable);


shr_error_e soc_flexe_std_sar_channel_map_set(
    int unit,
    uint32 flags,
    int channel,
    int ilkn_channel,
    int enable);


shr_error_e soc_flexe_std_sar_loopback_set(
    int unit,
    flexe_drv_sar_loopback_mode_e mode,
    int enable);


shr_error_e soc_flexe_std_env_set(
    int unit,
    int flexe_port,
    int phy_rate,
    int bypass);


shr_error_e soc_flexe_std_tmac_calendar_enable_set(
    int unit,
    int enable);


shr_error_e soc_flexe_std_group_rx_add(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed);


shr_error_e soc_flexe_std_group_rx_remove(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed);


shr_error_e soc_flexe_std_group_tx_add(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed);


shr_error_e soc_flexe_std_group_tx_remove(
    int unit,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed);


shr_error_e soc_flexe_std_group_add(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    uint8 lphys[FLEXE_CORE_NOF_LPHYS],
    int speed);


shr_error_e soc_flexe_std_group_remove(
    int unit,
    int group,
    SHR_BITDCL phy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_PORTS)],
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int speed);


shr_error_e soc_flexe_std_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel);


shr_error_e soc_flexe_std_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel);


shr_error_e soc_flexe_std_channel_rx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)],
    int calendar_sel);


shr_error_e soc_flexe_std_channel_rx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel);


shr_error_e soc_flexe_std_channel_tx_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_SAR_NOF_TIMESLOTS)],
    int calendar_sel);


shr_error_e soc_flexe_std_channel_tx_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int calendar_sel);


shr_error_e soc_flexe_std_channel_bypass_set(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_TIMESLOTS)],
    int enable);


shr_error_e soc_flexe_std_client_channel_add(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_TINYMAC_NOF_TIMESLOTS)]);


shr_error_e soc_flexe_std_client_channel_del(
    int unit,
    int channel,
    SHR_BITDCL tsmap[_SHR_BITDCLSIZE(FLEXE_TINYMAC_NOF_TIMESLOTS)]);


shr_error_e soc_flexe_std_66bsw_set(
    int unit,
    int src_channel,
    int dest_channel,
    int index,
    int enable);


shr_error_e soc_flexe_std_oh_loopback_set(
    int unit,
    int instance,
    int enable);


shr_error_e soc_flexe_std_oh_config(
    int unit,
    int instance);


shr_error_e soc_flexe_port_default_config(
    int unit,
    int bcm_port,
    int speed);


shr_error_e soc_flexe_std_oh_logical_phy_id_set(
    int unit,
    int instance,
    int logical_phy_id);


shr_error_e soc_flexe_std_oh_logical_phy_id_get(
    int unit,
    int instance,
    uint32 flags,
    int *logical_phy_id);


shr_error_e soc_flexe_std_oh_group_id_set(
    int unit,
    int instance,
    int group_id);


shr_error_e soc_flexe_std_oh_group_id_get(
    int unit,
    int instance,
    uint32 flags,
    int *group_id);


shr_error_e soc_flexe_std_oh_client_id_set(
    int unit,
    int instance,
    int cal_id,
    int timeslot,
    int client_id);


shr_error_e soc_flexe_std_oh_client_id_get(
    int unit,
    int flags,
    int instance,
    int cal_id,
    int timeslot,
    int *client_id);


shr_error_e soc_flexe_std_oh_phymap_update(
    int unit,
    int instance,
    int logical_phy_id,
    int count,
    int clr);


shr_error_e soc_flexe_std_oh_phymap_clear(
    int unit,
    int instance);


shr_error_e soc_flexe_std_oh_cr_bit_set(
    int unit,
    int instance,
    int cal_id);


shr_error_e soc_flexe_std_oh_cr_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id);


shr_error_e soc_flexe_std_oh_ca_bit_set(
    int unit,
    int instance,
    int cal_id);


shr_error_e soc_flexe_std_oh_ca_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id);


shr_error_e soc_flexe_std_oh_c_bit_set(
    int unit,
    int instance,
    int cal_id);


shr_error_e soc_flexe_std_oh_c_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *cal_id);


shr_error_e soc_flexe_std_oh_sc_bit_set(
    int unit,
    int instance,
    int sync_config);


shr_error_e soc_flexe_std_oh_sc_bit_get(
    int unit,
    int instance,
    uint32 flags,
    int *sync_config);


shr_error_e soc_flexe_std_active_calendar_set(
    int unit,
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    int cal_id);


shr_error_e soc_flexe_std_active_calendar_get(
    int unit,
    int instance,
    SHR_BITDCL lphy_bitmap[_SHR_BITDCLSIZE(FLEXE_CORE_NOF_LPHYS)],
    uint32 flags,
    int *cal_id);


shr_error_e soc_flexe_std_oh_alarm_status_get(
    int unit,
    int instance,
    uint16 *alarm_status);


shr_error_e soc_flexe_std_oh_cal_mismatch_ts_get(
    int unit,
    int instance,
    SHR_BITDCL * time_slots);


shr_error_e soc_flexe_std_oam_alarm_status_get(
    int unit,
    int client_channel,
    uint16 *alarms_status);


shr_error_e soc_flexe_std_oam_alarm_enable_set(
    int unit,
    int client_channel,
    int alarm_type,
    int enable);


shr_error_e soc_flexe_std_oam_base_period_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 base_period);


shr_error_e soc_flexe_std_oam_base_period_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *base_period);


shr_error_e soc_flexe_std_oam_base_insert_enable_set(
    int unit,
    int client_channel,
    uint32 enable);


shr_error_e soc_flexe_std_oam_base_insert_enable_get(
    int unit,
    int client_channel,
    uint32 *enable);


shr_error_e soc_flexe_std_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable);


shr_error_e soc_flexe_std_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable);


shr_error_e soc_flexe_std_sar_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable);


shr_error_e soc_flexe_std_sar_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable);


shr_error_e soc_flexe_std_oam_local_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable);


shr_error_e soc_flexe_std_oam_local_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable);


shr_error_e soc_flexe_std_oam_remote_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable);


shr_error_e soc_flexe_std_oam_remote_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable);


shr_error_e soc_flexe_std_oam_sd_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num);


shr_error_e soc_flexe_std_oam_sd_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num);


shr_error_e soc_flexe_std_oam_sd_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);


shr_error_e soc_flexe_std_oam_sd_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);


shr_error_e soc_flexe_std_oam_sd_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);


shr_error_e soc_flexe_std_oam_sd_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);


shr_error_e soc_flexe_std_oam_sd_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num);


shr_error_e soc_flexe_std_oam_sd_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num);


shr_error_e soc_flexe_std_oam_sd_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);


shr_error_e soc_flexe_std_oam_sd_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);


shr_error_e soc_flexe_std_oam_sd_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);


shr_error_e soc_flexe_std_oam_sd_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);


shr_error_e soc_flexe_std_oam_sf_bip8_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num);


shr_error_e soc_flexe_std_oam_sf_bip8_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num);


shr_error_e soc_flexe_std_oam_sf_bip8_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);


shr_error_e soc_flexe_std_oam_sf_bip8_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);


shr_error_e soc_flexe_std_oam_sf_bip8_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);


shr_error_e soc_flexe_std_oam_sf_bip8_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);


shr_error_e soc_flexe_std_oam_sf_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num);


shr_error_e soc_flexe_std_oam_sf_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num);


shr_error_e soc_flexe_std_oam_sf_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);


shr_error_e soc_flexe_std_oam_sf_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);


shr_error_e soc_flexe_std_oam_sf_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold);


shr_error_e soc_flexe_std_oam_sf_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold);


shr_error_e soc_flexe_std_oam_bip8_counter_get(
    int unit,
    int client_channel,
    uint64 *val);


shr_error_e soc_flexe_std_oam_bei_counter_get(
    int unit,
    int client_channel,
    uint64 *val);


shr_error_e soc_flexe_std_oam_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val);


shr_error_e soc_flexe_std_oam_base_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val);


shr_error_e
soc_flexe_std_oam_alarm_collection_timer_step_set(
    int unit,
    int step);


shr_error_e
soc_flexe_std_oam_alarm_collection_timer_step_get(
    int unit,
    int *step);


shr_error_e
soc_flexe_std_oam_alarm_collection_step_count_set(
    int unit,
    int step_count);


shr_error_e
soc_flexe_std_oam_alarm_collection_step_count_get(
    int unit,
    int *step_count);

shr_error_e soc_flexe_std_sar_cell_mode_set(
    int unit,
    int cell_mode);

shr_error_e soc_flexe_std_sar_cell_mode_get(
    int unit,
    int *cell_mode);

shr_error_e soc_flexe_std_rx_channel_map_table_set(
    int unit,
    int bcm_port,
    uint32 local_port);

shr_error_e soc_flexe_std_tx_channel_map_table_set(
    int unit,
    int bcm_port,
    uint32 local_port);

shr_error_e soc_flexe_std_client_local_fault_get(
    int unit,
    int client_channel,
    int *local_fault_status);

shr_error_e soc_flexe_std_client_remote_fault_get(
    int unit,
    int client_channel,
    int *remote_fault_status);

shr_error_e soc_flexe_std_sar_client_local_fault_get(
    int unit,
    int client_channel,
    int *local_fault_status);

shr_error_e soc_flexe_std_sar_client_remote_fault_get(
    int unit,
    int client_channel,
    int *remote_fault_status);

#endif
