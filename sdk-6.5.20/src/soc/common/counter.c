/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Packet Statistics Counter Management
 *
 * Firebolt     1024 bytes per port (128 * uint64)
 * Helix        1024 bytes per port (128 * uint64)
 * Felix        1024 bytes per port (128 * uint64)
 * Raptor       1024 bytes per port (128 * uint64)
 * Hercules:    1024 bytes per port (128 * uint64)
 * HUMV:        2048 bytes per port (256 * uint64)
 * Bradley:     2048 bytes per port (256 * uint64)
 * Goldwing:    2048 bytes per port (256 * uint64)
 * Triumph:     4096 bytes per port (512 * uint64)
 *
 * BCM5670 Endian: on Hercules, the well-intentioned ES_BIG_ENDIAN_OTHER
 * causes counter DMA to write the most significant word first in the
 * DMA buffer.  This is wrong because not all big-endian hosts have the
 * same setting for ES_BIG_ENDIAN_OTHER (e.g. mousse and idtrp334).
 * This problem makes it necessary to check the endian select to
 * determine whether to swap words.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/time.h>

#include <soc/drv.h>
#include <soc/counter.h>
#include <soc/ll.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/register.h>
#ifdef BCM_ESW_SUPPORT
#include <soc/format.h>
#endif
#ifdef BCM_TRIDENT_SUPPORT
#include <soc/trident.h>
#endif
#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/utils.h>
#endif
#if defined(BCM_DFE_SUPPORT)
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/ARAD/arad_stat.h>
#endif
#if defined(BCM_JERICHO_SUPPORT)
#include <soc/dpp/JER/jer_stat.h>
#endif
#if defined(BCM_88750_SUPPORT)
#include <soc/dfe/fe1600/fe1600_stat.h>
#endif
#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/mbcm.h>
#endif
#if defined(BCM_DNX_SUPPORT)
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_er_threading.h>
#endif
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#include <bcm_int/esw_dispatch.h>
#endif
#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif
#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <soc/td2_td2p.h>
#endif
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>
#endif
#if defined(BCM_KATANA2_SUPPORT)
#include <soc/saber2.h>
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/tomahawk.h>
#include <soc/tomahawk2.h>
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif
#if defined(BCM_HELIX5_SUPPORT)
#include <soc/helix5.h>
#endif
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#endif
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif

#if defined(BCM_GREYHOUND2_SUPPORT)
#include <soc/greyhound2.h>
#endif

#if defined(BCM_FIRELIGHT_SUPPORT)
#include <soc/firelight.h>
#endif

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif /* CRASH_RECOVERY_SUPPORT */

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_ha.h>
#endif

#include <soc/dnxc/multithread_analyzer.h>

#if defined(INCLUDE_PHY_8806X)
extern int phy_mt2_update_led_speed_activity(int unit, soc_port_t port);
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)

#define COUNTER_IDX_PORTBASE(unit, port) \
        ((port) * SOC_CONTROL(unit)->counter_perport)

#define COUNTER_IDX_GET(unit, ctr_ref, port) \
        (COUNTER_IDX_PORTBASE(unit, port) + \
         SOC_REG_CTR_IDX(unit, (ctr_ref)->reg) + (ctr_ref)->index)

#define COUNTER_IDX_OFFSET(unit)        0

#define COUNTER_MIN_IDX_GET(unit, port) \
        (COUNTER_IDX_PORTBASE(unit, port) + COUNTER_IDX_OFFSET(unit))

soc_counter_control_t *soc_counter_control[SOC_MAX_NUM_DEVICES];

static soc_counter_extra_f soc_counter_extra[SOC_MAX_NUM_DEVICES]
                                            [SOC_COUNTER_EXTRA_CB_MAX];
/* Number of cosq per unit per port */
static int num_cosq[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS] = {{ 0 }};

#ifdef BCM_SBUSDMA_SUPPORT
volatile uint32 _soc_counter_pending[SOC_MAX_NUM_DEVICES] = { 0 };
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
static uint32 *buff_max[SOC_MAX_NUM_DEVICES] = { NULL };
static void *host_entry[SOC_MAX_NUM_DEVICES] = { NULL };
static void *host_buff[SOC_MAX_NUM_DEVICES] = { NULL };
#endif

#if defined(BCM_XGS_SUPPORT) || defined(BCM_PETRA_SUPPORT)
STATIC int
soc_counter_collect64(int unit, int discard, soc_port_t tmp_port, soc_reg_t hw_ctr_reg);
#endif /* defined(BCM_XGS_SUPPORT) || defined(BCM_PETRA_SUPPORT) */


STATIC void
soc_counter_collect_non_dma_entries(int unit,
                                    int tmp_port,
                                    int port_idx,
                                    int ar_idx,
                                    soc_reg_t ctr_reg);
STATIC int
_soc_counter_non_dma_is_invalid(int unit, soc_reg_t reg, soc_port_t port);

#ifdef BCM_TOMAHAWK_SUPPORT
STATIC void
soc_counter_collect_th_non_dma_entries(int unit,
                                        int tmp_port,
                                        int port_idx,
                                        int ar_idx,
                                        soc_reg_t ctr_reg);
#endif /* BCM_TOMAHAWK_SUPPORT */
STATIC void soc_counter_generic_collect_non_dma_entries(int unit,
                                                        soc_ctr_control_info_t
                                                        ctrl_info,
                                                        int base_idx,
                                                        int ar_idx,
                                                        soc_reg_t ctr_reg);
/*
 * Turn on COUNTER_BENCH and "debug +verbose" to benchmark the CPU time
 * spent on counter activity.  NOTE: most useful on platforms where
 * sal_time_usecs() has usec precision instead of clock tick precision.
 */

#undef COUNTER_BENCH

/* Per port mapping to counter map structures */
#define PORT_CTR_REG(unit, port, idx) \
        (&SOC_CONTROL(unit)->counter_map[port]->cmap_base[idx])
#define PORT_CTR_NUM(unit, port) \
        (SOC_CONTROL(unit)->counter_map[port]->cmap_size)

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)

static uint64 *soc_counter_tbuf[SOC_MAX_NUM_DEVICES];
#define SOC_COUNTER_TBUF_SIZE(unit) \
    (SOC_CONTROL(unit)->counter_perport * sizeof(uint64))

#ifdef BCM_TRIUMPH2_SUPPORT
#define COUNTER_TIMESTAMP_FIFO_SIZE     4
#define DIRECT_TIMESTAMP_DMA_COUNTER_NUM    100
#define XMAC_DMA_COUNTER_BASE               42
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
typedef struct _kt_cosq_counter_mem_map_s {
    soc_mem_t mem;
    soc_counter_non_dma_id_t non_dma_id;
}_kt_cosq_counter_mem_map_t;

/*
 * CTR_FLEX_COUNT_0/1/2/3 = Total ENQ discarded
 * CTR_FLEX_COUNT_4/5/6/7  = Red packet ENQ discarded
 * CTR_FLEX_COUNT_8/9/10/11  = Total DEQ
 */
_kt_cosq_counter_mem_map_t _kt_cosq_mem_map[] =
{
    { CTR_FLEX_COUNT_0m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT },
    { CTR_FLEX_COUNT_1m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT },
    { CTR_FLEX_COUNT_2m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT },
    { CTR_FLEX_COUNT_3m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT },
    { CTR_FLEX_COUNT_4m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED },
    { CTR_FLEX_COUNT_5m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED },
    { CTR_FLEX_COUNT_6m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED },
    { CTR_FLEX_COUNT_7m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED },
    { CTR_FLEX_COUNT_8m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT },
    { CTR_FLEX_COUNT_9m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT },
    { CTR_FLEX_COUNT_10m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT },
    { CTR_FLEX_COUNT_11m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT }
};

/*
 * CTR_FLEX_COUNT_0 = Total ENQ discarded
 * CTR_FLEX_COUNT_1 = Total ENQ Accepted
 * CTR_FLEX_COUNT_2 = Green ENQ discarded
 * CTR_FLEX_COUNT_3 = Yellow ENQ discarded
 * CTR_FLEX_COUNT_4    = Red packet ENQ discarded
 * CTR_FLEX_COUNT_5 = Green ENQ Accepted
 * CTR_FLEX_COUNT_6 = Yellow ENQ Accepted
 * CTR_FLEX_COUNT_7 = Red ENQ Accepted
 * CTR_FLEX_COUNT_8    = Total DEQ
*/
_kt_cosq_counter_mem_map_t _kt_cosq_gport_mem_map[] =
{
    { CTR_FLEX_COUNT_0m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT },
    { CTR_FLEX_COUNT_1m, SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT },
    { CTR_FLEX_COUNT_2m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN },
    { CTR_FLEX_COUNT_3m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW },
    { CTR_FLEX_COUNT_4m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED },
    { CTR_FLEX_COUNT_5m, SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_GREEN },
    { CTR_FLEX_COUNT_6m, SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_YELLOW },
    { CTR_FLEX_COUNT_7m, SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_RED },
    { CTR_FLEX_COUNT_8m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT }
};
#endif
#ifdef BCM_KATANA2_SUPPORT
/*
 * CTR_FLEX_COUNT_0/1/2/3 = Total ENQ discarded
 * CTR_FLEX_COUNT_4/5/6/7 = Red packet ENQ discarded
 * CTR_FLEX_COUNT_8/9/10/11 = Total DEQ
 */
_sb2_cosq_counter_mem_map_t _kt2_cosq_mem_map[] =
{
    { CTR_FLEX_COUNT_0m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,"DROP_PKT","DROP_BYTE"},
    { CTR_FLEX_COUNT_1m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,"DROP_PKT","DROP_BYTE"},
    { CTR_FLEX_COUNT_2m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,"DROP_PKT","DROP_BYTE"},
    { CTR_FLEX_COUNT_3m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,"DROP_PKT","DROP_BYTE"},
    { CTR_FLEX_COUNT_4m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED,"DROP_PKT_R","DROP_BYTE_R" },
    { CTR_FLEX_COUNT_5m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED,"DROP_PKT_R","DROP_BYTE_R" },
    { CTR_FLEX_COUNT_6m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED,"DROP_PKT_R","DROP_BYTE_R" },
    { CTR_FLEX_COUNT_7m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED,"DROP_PKT_R","DROP_BYTE_R" },
    { CTR_FLEX_COUNT_8m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,"PERQ_PKT","PERQ_BYTE"},
    { CTR_FLEX_COUNT_9m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,"PERQ_PKT","PERQ_BYTE" },
    { CTR_FLEX_COUNT_10m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,"PERQ_PKT","PERQ_BYTE"},
    { CTR_FLEX_COUNT_11m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,"PERQ_PKT","PERQ_BYTE" },
};

/*
 * CTR_FLEX_COUNT_0 = Total ENQ discarded
 * CTR_FLEX_COUNT_1 = Total ENQ Accepted
 * CTR_FLEX_COUNT_2 = Green ENQ discarded
 * CTR_FLEX_COUNT_3 = Yellow ENQ discarded
 * CTR_FLEX_COUNT_4 = Red packet ENQ discarded
 * CTR_FLEX_COUNT_5 = Green ENQ Accepted
 * CTR_FLEX_COUNT_6 = Yellow ENQ Accepted
 * CTR_FLEX_COUNT_7 = Red ENQ Accepted
 * CTR_FLEX_COUNT_8 = Total DEQ
*/
_sb2_cosq_counter_mem_map_t _kt2_cosq_gport_mem_map[] =
{
    { CTR_FLEX_COUNT_0m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT },
    { CTR_FLEX_COUNT_1m, SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT },
    { CTR_FLEX_COUNT_2m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN },
    { CTR_FLEX_COUNT_3m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW },
    { CTR_FLEX_COUNT_4m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED },
    { CTR_FLEX_COUNT_5m, SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_GREEN },
    { CTR_FLEX_COUNT_6m, SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_YELLOW },
    { CTR_FLEX_COUNT_7m, SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT_RED },
    { CTR_FLEX_COUNT_8m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT }
};

/*
 * CTR_FLEX_COUNT_0/1 = Total ENQ discarded
 * CTR_FLEX_COUNT_2/3  = Red packet ENQ discarded
 * CTR_FLEX_COUNT_4/5  = Total DEQ
 */
_sb2_cosq_counter_mem_map_t _sb2_cosq_mem_map[] =
{
    { CTR_FLEX_COUNT_0m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,"DROP_PKT","DROP_BYTE"},
    { CTR_FLEX_COUNT_1m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,"DROP_PKT","DROP_BYTE"},
    { CTR_FLEX_COUNT_2m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED,"DROP_PKT_R","DROP_BYTE_R" },
    { CTR_FLEX_COUNT_3m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED,"DROP_PKT_R","DROP_BYTE_R" },
    { CTR_FLEX_COUNT_4m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,"PERQ_PKT","PERQ_BYTE"},
    { CTR_FLEX_COUNT_5m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,"PERQ_PKT","PERQ_BYTE" },
};

/*
 * CTR_FLEX_COUNT_0 = Total ENQ discarded
 * CTR_FLEX_COUNT_1  = Red packet ENQ discarded
 * CTR_FLEX_COUNT_2  = Total DEQ
 */
_sb2_cosq_counter_mem_map_t _dg2_cosq_mem_map[] =
{
    { CTR_FLEX_COUNT_0m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,"DROP_PKT","DROP_BYTE"},
    { CTR_FLEX_COUNT_1m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED,"DROP_PKT_R","DROP_BYTE_R" },
    { CTR_FLEX_COUNT_2m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,"PERQ_PKT","PERQ_BYTE"},
};

/*
 * CTR_FLEX_COUNT_0 = Total ENQ discarded
 * CTR_FLEX_COUNT_1 = Total ENQ Accepted
 * CTR_FLEX_COUNT_2 = Green ENQ discarded
 * CTR_FLEX_COUNT_3 = Yellow ENQ discarded
 * CTR_FLEX_COUNT_4 = Red packet ENQ discarded
 * CTR_FLEX_COUNT_5 = Total DEQ
*/
_sb2_cosq_counter_mem_map_t _sb2_cosq_gport_mem_map[] =
{
    { CTR_FLEX_COUNT_0m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT },
    { CTR_FLEX_COUNT_1m, SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT },
    { CTR_FLEX_COUNT_2m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_GREEN },
    { CTR_FLEX_COUNT_3m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_YELLOW },
    { CTR_FLEX_COUNT_4m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED },
    { CTR_FLEX_COUNT_5m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT },
};

#if defined (BCM_METROLITE_SUPPORT)
/*
 * CTR_FLEX_COUNT_0 = Total ENQ discarded
 * CTR_FLEX_COUNT_1  = Red packet ENQ discarded
 * CTR_FLEX_COUNT_2  = Total DEQ
 */
_sb2_cosq_counter_mem_map_t _ml_cosq_mem_map[] =
{   { CTR_FLEX_COUNT_0m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,"DROP_PKT","DROP_BYTE"},
    { CTR_FLEX_COUNT_1m, SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED,"DROP_PKT_R","DROP_BYTE_R" },
    { CTR_FLEX_COUNT_2m, SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,"PERQ_PKT","PERQ_BYTE" },
};
#endif
#endif
#ifdef BCM_KATANA2_SUPPORT
void
soc_sb2_cosq_counter_mem_map_get(int unit, int *num_elem,
                                 _sb2_cosq_counter_mem_map_t **mem_map)
{
    uint16 dev_id;
    uint8 rev_id;
    soc_cm_get_id(unit, &dev_id, &rev_id);
#if defined (BCM_METROLITE_SUPPORT)
     if (SOC_IS_METROLITE(0)) {
         *mem_map = _ml_cosq_mem_map;
         *num_elem = sizeof(_ml_cosq_mem_map)/sizeof(_sb2_cosq_counter_mem_map_t);
     } else
#endif
    if (dev_id == BCM56233_DEVICE_ID) {
        *mem_map = _dg2_cosq_mem_map;
        *num_elem = sizeof(_dg2_cosq_mem_map)/sizeof(_sb2_cosq_counter_mem_map_t);
     } else if (SOC_IS_SABER2(unit)) {
        *mem_map = _sb2_cosq_mem_map;
        *num_elem = sizeof(_sb2_cosq_mem_map)/sizeof(_sb2_cosq_counter_mem_map_t);
     } else {
        *mem_map = _kt2_cosq_mem_map;
        *num_elem = sizeof(_kt2_cosq_mem_map)/sizeof(_sb2_cosq_counter_mem_map_t);
     }

}
#endif

int
is_xaui_rx_counter(soc_reg_t ctr_reg)
{
    switch (ctr_reg) {
    case IR64r:
    case IR127r:
    case IR255r:
    case IR511r:
    case IR1023r:
    case IR1518r:
    case IR2047r:
    case IR4095r:
    case IR9216r:
    case IR16383r:
    case IRBCAr:
    case IRBYTr:
    case IRERBYTr:
    case IRERPKTr:
    case IRFCSr:
    case IRFLRr:
    case IRFRGr:
    case IRJBRr:
    case IRJUNKr:
    case IRMAXr:
    case IRMCAr:
    case IRMEBr:
    case IRMEGr:
    case IROVRr:
    case IRPKTr:
    case IRUNDr:
    case IRXCFr:
    case IRXPFr:
    case IRXUOr:
#ifdef BCM_TRX_SUPPORT
    case IRUCr:
#ifdef BCM_ENDURO_SUPPORT
    case IRUCAr:
#endif /* BCM_ENDURO_SUPPORT */
    case IRPOKr:
    case MAC_RXLLFCMSGCNTr:
#endif
#if defined(BCM_SCORPION_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT)
    case IRXPPr:
#endif
        return 1;
    default:
        break;
    }
    return 0;
}
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
STATIC int
_soc_counter_trident_get_info(int unit, soc_port_t port, soc_reg_t id,
                              int *base_index, int *num_entries)
{
    soc_control_t *soc;
    soc_info_t *si;
    soc_counter_non_dma_t *non_dma;
    soc_port_t phy_port, mmu_port;
    soc_port_t mmu_cmic_port, mmu_lb_port;

    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    if (port < 0) {
        return SOC_E_PARAM;
    }

    si = &SOC_INFO(unit);
    if (si->port_l2p_mapping[port] == -1) {
        *base_index = 0;
        *num_entries = 0;
        return SOC_E_NONE;
    }

    mmu_cmic_port = si->port_p2m_mapping[si->port_l2p_mapping[si->cmic_port]];
    mmu_lb_port = si->port_p2m_mapping[si->port_l2p_mapping[si->lb_port]];

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    switch (id) {
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
        if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) { /* in X pipe */
            *base_index = si->port_cosq_base[port];
        } else { /* in Y pipe */
            *base_index = si->port_cosq_base[port] +
                non_dma->dma_index_max[0] + 1;
        }
        *num_entries = si->port_num_cosq[port];
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
        if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) { /* in X pipe */
            *base_index = si->port_uc_cosq_base[port];
        } else { /* in Y pipe */
            *base_index = si->port_uc_cosq_base[port] +
                non_dma->dma_index_max[0] + 1;
        }
        *num_entries = si->port_num_uc_cosq[port];
        break;
    case SOC_COUNTER_NON_DMA_MMU_QCN_CNM:
        if((mmu_port == mmu_lb_port) || (mmu_port == mmu_cmic_port)){
            *num_entries = 0;
            *base_index = 0;
        }else{
            *num_entries = 2;/* 2 QCN queue per port */
            *base_index = port * (*num_entries);
        }
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT:
        if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) { /* in X pipe */
            *base_index = si->port_ext_cosq_base[port];
        } else { /* in Y pipe */
            *base_index = si->port_ext_cosq_base[port] +
                non_dma->dma_index_max[0] + 1;
        }
        *num_entries = si->port_num_ext_cosq[port];
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
        if (mmu_port == mmu_cmic_port) {
            *base_index = 0;
            *num_entries = 48;
        } else {
            *base_index = 48 + (mmu_port - mmu_cmic_port - 1) * 5;
            *num_entries = 5;
        }
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
        if (mmu_port < mmu_lb_port) { /* in X pipe */
            if (mmu_port == mmu_cmic_port) {
                *base_index = 0;
                *num_entries = 0;
            } else if (mmu_port <= mmu_cmic_port + 4) {
                *base_index = (mmu_port - mmu_cmic_port - 1) * 74;
                *num_entries = 74; /* 10 ucast + 64 ext ucast */
            } else {
                *base_index = 74 * 4 + (mmu_port - mmu_cmic_port - 5) * 10;
                *num_entries = 10;
            }
        } else { /* in Y pipe */
            if (mmu_port == mmu_lb_port) {
                *base_index = 0;
                *num_entries = 0;
            } else if (mmu_port <= mmu_lb_port + 4) {
                *base_index = 576 + (mmu_port - mmu_lb_port - 1) * 74;
                *num_entries = 74; /* 10 ucast + 64 ext ucast */
            } else {
                *base_index = 576 + 74 * 4 + (mmu_port - mmu_lb_port - 5) * 10;
                *num_entries = 10;
            }
        }
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
        *num_entries = 1;
        *base_index = port;
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
        *num_entries = 1;
        *base_index = mmu_port;
        break;
    case SOC_COUNTER_NON_DMA_PG_MIN_PEAK:
    case SOC_COUNTER_NON_DMA_PG_MIN_CURRENT:
    case SOC_COUNTER_NON_DMA_PG_SHARED_PEAK:
    case SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT:
    case SOC_COUNTER_NON_DMA_PG_HDRM_PEAK:
    case SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT:
        *num_entries = 8;
        *base_index = port * (*num_entries);
        break;
    case SOC_COUNTER_NON_DMA_QUEUE_PEAK:
    case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
        *num_entries = 5;
        *base_index = port * (*num_entries);
        break;
    case SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK:
    case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
        *num_entries = 10;
        *base_index = port * (*num_entries);
        break;
    case SOC_COUNTER_NON_DMA_EXT_QUEUE_PEAK:
    case SOC_COUNTER_NON_DMA_EXT_QUEUE_CURRENT:
        *num_entries = 64;
        *base_index = port * (*num_entries);
        break;
    case SOC_COUNTER_NON_DMA_POOL_PEAK:
    case SOC_COUNTER_NON_DMA_POOL_CURRENT:
        *base_index = 0;
        *num_entries = non_dma->num_entries;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    *base_index += non_dma->base_index;

    return SOC_E_NONE;
}
#endif /* BCM_TRIDENT_SUPPORT */

#if defined ( BCM_APACHE_SUPPORT )
#define _AP_PBLKS_PER_PIPE            9
#define _APACHE_XLPS_PER_PGW          9
#define _APACHE_PORTS_PER_XLP         4

#define _MONTEREY_XLPS_PER_PGW        8
#define _MN_PBLKS_PER_PIPE            8

STATIC int
_soc_counter_apache_get_child_dma_by_idx(int unit, soc_counter_non_dma_t *parent,
                                     int idx, soc_counter_non_dma_t **child)
{
    int rv = SOC_E_NONE;

    if ((parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        if (idx >= parent->extra_ctr_ct) {
            return SOC_E_PARAM;
        }
        *child = &parent->extra_ctrs[idx << 1];
    } else {
        *child = parent;
    }
    return rv;
}

/* For a given logical port, return the OBM id and the lane number */
int
soc_apache_port_obm_info_get(int unit, soc_port_t port,
                               int *obm_id, int *lane)
{
    soc_info_t *si;
    soc_port_t phy_port;
    int obm, pgw;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) || IS_MACSEC_PORT(unit, port) ||
        IS_RDB_PORT(unit, port)) {
        /*  nothing to do for non-idb ports */
        return SOC_E_NONE;
    }

    /* OBM group (num groups = num tsc's) belongs to PGW0/PGW1  */
    obm = si->port_serdes[port];
    if (SOC_IS_MONTEREY(unit)) {
        obm %= _MONTEREY_XLPS_PER_PGW;
    } else {
    obm %= _APACHE_XLPS_PER_PGW;
    }

    pgw = si->port_group[port];
    if (lane) {
        *lane = (phy_port - 1) % _APACHE_PORTS_PER_XLP;
        *lane += (pgw) ? 4 : 0;
    }
    if (obm_id) {
        *obm_id = obm;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_counter_apache_get_pgw_inst(int unit, soc_port_t port,
                                 int *pgw_inst, int *lane)
{
    soc_info_t *si;
    soc_port_t phy_port;
    int pgw;

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) || IS_MACSEC_PORT(unit, port) ||
        IS_RDB_PORT(unit, port)) {
        /*  nothing to do for non-idb ports */
        return SOC_E_NONE;
    }

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    pgw = si->port_group[port];
    if (lane) {
        *lane = (phy_port - 1) % _APACHE_PORTS_PER_XLP;
    }
    if (pgw_inst) {
        *pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
    }
    return SOC_E_NONE;
}

STATIC int
_soc_counter_apache_get_info(int unit, soc_port_t port, soc_reg_t id,
                              int *base_index, int *num_entries)
{
    soc_info_t *si;
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    soc_port_t phy_port = 0, mmu_port = 0;
    int obm_id, lane;
    int maxports = 75;
    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    si = &SOC_INFO(unit);
    if (port >= 0  && port < maxports) {
        if (si->port_l2p_mapping[port] == -1) {
            *base_index = 0;
            *num_entries = 0;
            return SOC_E_NONE;
        }

        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
    } else {

      mmu_port =-1 ;

    }

    switch (id) {
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
        if (mmu_port >= 0) {
#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                *base_index =
                  soc_monterey_logical_qnum_hw_qnum(unit, port,
                                             si->port_cosq_base[port], 0);
            } else
#endif
           {
            *base_index =
                soc_apache_logical_qnum_hw_qnum(unit, port,
                                             si->port_cosq_base[port], 0);
            }
            *num_entries = si->port_num_cosq[port];
        } else {
            *base_index = port;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
        if (mmu_port >= 0) {
#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                *base_index =
                  soc_monterey_logical_qnum_hw_qnum(unit, port,
                                             si->port_uc_cosq_base[port], 1);
            } else
#endif
            {
            *base_index =
                soc_apache_logical_qnum_hw_qnum(unit, port,
                                             si->port_uc_cosq_base[port], 1);
            }
            *num_entries = si->port_num_uc_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
    case SOC_COUNTER_NON_DMA_QUEUE_PEAK:
    case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
        if (mmu_port >= 0) {
            *base_index = si->port_cosq_base[port];
            *num_entries = si->port_num_cosq[port];
        } else {
            *base_index = port;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
    case SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK:
    case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
        if (mmu_port >= 0) {
            *base_index = si->port_uc_cosq_base[port];
            *num_entries = si->port_num_uc_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_MMU_QCN_CNM:
        if (mmu_port >= 0) {
            *base_index = si->port_uc_cosq_base[port];
            *num_entries = si->port_num_uc_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_LOW_PRI:
        *base_index = (phy_port - 1) / 16 * 4;
        *num_entries = 4;
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT:
        if (mmu_port >= 0) {
            *base_index = non_dma->dma_index_min[0];
            *num_entries = non_dma->dma_index_max[0] + 1;
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING_METER:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING_METER:
        if (mmu_port >= 0) {
            *num_entries = non_dma->entries_per_port;
            *base_index = mmu_port;
        } else {
            return SOC_E_INTERNAL;
        }
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
        if ((mmu_port >= 0) && (mmu_port < 74)) {
            *base_index = (mmu_port & 0x7f) * non_dma->entries_per_port;
            *num_entries = non_dma->entries_per_port;
        } else {
            return SOC_E_INTERNAL;
        }
        break;
    case SOC_COUNTER_NON_DMA_PG_MIN_PEAK:
    case SOC_COUNTER_NON_DMA_PG_MIN_CURRENT:
    case SOC_COUNTER_NON_DMA_PG_SHARED_PEAK:
    case SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT:
    case SOC_COUNTER_NON_DMA_PG_HDRM_PEAK:
    case SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT:
        if (mmu_port >= 0) {
            *base_index = (mmu_port & 0x7f) * non_dma->entries_per_port;
            *num_entries = non_dma->entries_per_port;
        } else {
            return SOC_E_INTERNAL;
        }
        break;
    case SOC_COUNTER_NON_DMA_DROP_RQ_PKT:
    case SOC_COUNTER_NON_DMA_DROP_RQ_BYTE:
    case SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED:
    case SOC_COUNTER_NON_DMA_POOL_PEAK:
    case SOC_COUNTER_NON_DMA_POOL_CURRENT:
    case SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_PEAK:
    case SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_CURRENT:
    case SOC_COUNTER_NON_DMA_E2E_DROP_COUNT:
        *base_index = 0;
        *num_entries = non_dma->num_entries;
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_EXPRESS:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_EXPRESS:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_LO:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_LO:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_HI:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_HI:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS0:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS0:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS1:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS1:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_EXPRESS:
    case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_EXPRESS:
        *base_index = 0;
        *num_entries = 1;
        lane = 0;
        /* find obm from the port number */
        SOC_IF_ERROR_RETURN
            (soc_apache_port_obm_info_get(unit, port, &obm_id, &lane));
        if (obm_id >= _AP_PBLKS_PER_PIPE) {
            return SOC_E_PARAM;
        }
        _soc_counter_apache_get_child_dma_by_idx(unit, non_dma, obm_id, &non_dma);
        *base_index += lane;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    *base_index += non_dma->base_index;

    return SOC_E_NONE;
}
#endif /* BCM_APACHE_SUPPORT */

#if defined(BCM_MONTEREY_SUPPORT) && defined (CPRIMOD_SUPPORT)
STATIC int
_soc_counter_monterey_cpri_get_info(int unit, soc_port_t port, soc_reg_t id,
                              int *base_index, int *num_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;

    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    if ((id >= SOC_COUNTER_NON_DMA_CPRI_START) &&
        (id <= SOC_COUNTER_NON_DMA_CPRI_END)) {
        *base_index = port;
        *num_entries = non_dma->entries_per_port;
        *base_index += non_dma->base_index;
    }

    return SOC_E_NONE;
}
#endif /* BCM_MONTEREY_SUPPORT && CPRIMOD_SUPPORT */

#if defined ( BCM_TRIDENT2_SUPPORT ) || defined ( BCM_TRIDENT2PLUS_SUPPORT )
STATIC int
_soc_counter_trident2_get_info(int unit, soc_port_t port, soc_reg_t id,
                              int *base_index, int *num_entries)
{
    soc_info_t *si;
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    soc_port_t phy_port, mmu_port;
    int pipe;

    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    if (_soc_counter_non_dma_is_invalid(unit, non_dma->reg, port)) {
        return SOC_E_UNAVAIL;
    }

    si = &SOC_INFO(unit);
    if (port >= 0) {
        if (si->port_l2p_mapping[port] == -1) {
            *base_index = 0;
            *num_entries = 0;
            return SOC_E_NONE;
        }

        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        pipe = (mmu_port >= 64) ? 1 : 0;
    } else {
        phy_port = mmu_port = -1;
        pipe = -1;
    }
    COMPILER_REFERENCE(pipe);

    switch (id) {
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
        if (mmu_port >= 0) {
            *base_index =
                soc_td2_logical_qnum_hw_qnum(unit, port,
                                             si->port_cosq_base[port], 0);
            *num_entries = si->port_num_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
        if (mmu_port >= 0) {
            *base_index =
                soc_td2_logical_qnum_hw_qnum(unit, port,
                                             si->port_uc_cosq_base[port], 1);
            *num_entries = si->port_num_uc_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
    case SOC_COUNTER_NON_DMA_QUEUE_PEAK:
    case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
        if (mmu_port >= 0) {
            *base_index = si->port_cosq_base[port];
            *num_entries = si->port_num_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
    case SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK:
    case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
        if (mmu_port >= 0) {
            *base_index = si->port_uc_cosq_base[port];
            *num_entries = si->port_num_uc_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_EXT:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_EXT:
        if (mmu_port >= 0) {
            if (pipe > 0) {
                /*
                 * Y-PIPE entries in DMA buffer is begging with 1480.
                 */
                *base_index = 1480;
                *num_entries = non_dma->dma_index_max[1]
                               - non_dma->dma_index_min[1] + 1;
            } else {
                *base_index = 0;
                *num_entries = non_dma->dma_index_max[0] + 1;
            }
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_MMU_QCN_CNM:
        if (mmu_port >= 0) {
            *base_index = si->port_uc_cosq_base[port];
            *num_entries = si->port_num_uc_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_LOW_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_HIGH_PRI:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_LOW_PRI:
        *base_index = SOC_INFO(unit).port_group[port] * 4;
        *num_entries = 4;
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT:
        if (mmu_port >= 0) {
            *base_index = non_dma->dma_index_min[0];
            *num_entries = non_dma->dma_index_max[0] + 1;
            if (pipe > 0) {
                *base_index = non_dma->dma_index_max[0] + 1;
            }
        } else {
            *base_index = 0;
            *num_entries = non_dma->num_entries;
        }
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING_METER:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING_METER:
        if (mmu_port >= 0) {
            *num_entries = non_dma->entries_per_port;
            *base_index = mmu_port;
        } else {
            return SOC_E_INTERNAL;
        }
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
        if (mmu_port >= 0) {
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) { /* in X pipe */
                *base_index = (mmu_port & 0x3f) * non_dma->entries_per_port;
            } else { /* in Y pipe */
                *base_index = (mmu_port & 0x3f) * non_dma->entries_per_port +
                    (non_dma->num_entries / 2);
            }
            *num_entries = non_dma->entries_per_port;
        } else {
            return SOC_E_INTERNAL;
        }
        break;
    case SOC_COUNTER_NON_DMA_PG_MIN_PEAK:
    case SOC_COUNTER_NON_DMA_PG_MIN_CURRENT:
    case SOC_COUNTER_NON_DMA_PG_SHARED_PEAK:
    case SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT:
    case SOC_COUNTER_NON_DMA_PG_HDRM_PEAK:
    case SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT:
        if (mmu_port >= 0) {
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) { /* in X pipe */
                *base_index = (mmu_port & 0x3f) * non_dma->entries_per_port;
            } else { /* in Y pipe */
                *base_index = (mmu_port & 0x3f) * non_dma->entries_per_port +
                    non_dma->dma_index_max[0] + 1;
            }
            *num_entries = non_dma->entries_per_port;
        } else {
            return SOC_E_INTERNAL;
        }
        break;
    case SOC_COUNTER_NON_DMA_DROP_RQ_PKT:
    case SOC_COUNTER_NON_DMA_DROP_RQ_BYTE:
    case SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED:
    case SOC_COUNTER_NON_DMA_POOL_PEAK:
    case SOC_COUNTER_NON_DMA_POOL_CURRENT:
    case SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_PEAK:
    case SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_CURRENT:
    case SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_Y_PEAK:
    case SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_Y_CURRENT:
        *base_index = 0;
        *num_entries = non_dma->num_entries;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    *base_index += non_dma->base_index;

    return SOC_E_NONE;
}
#endif /* BCM_TRIDENT2_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
STATIC int
_soc_counter_triumph3_get_info(int unit, soc_port_t port, soc_reg_t id,
                              int *base_index, int *num_entries)
{
    soc_info_t *si;
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    soc_port_t phy_port, mmu_port;

    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    si = &SOC_INFO(unit);
    if (port >= 0) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
    } else {
        phy_port = mmu_port = -1;
    }

    switch (id) {
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
        if (mmu_port >= 0) {
            if (mmu_port <= 39) {
                *base_index = mmu_port * 8;
                *num_entries = 8;
            } else if (mmu_port <= 55) {
                *base_index = 320 + (mmu_port - 40) * 10;
                *num_entries = 10;
            } else if (mmu_port == 56) {
                *base_index = 0x1e0;
                *num_entries = 8;
            } else if (mmu_port == 57) {
                *num_entries = 0;
            } else if (mmu_port == 58) {
                *base_index = 0x1e8;
                *num_entries = 8;
            } else if (mmu_port == 59) {
                *base_index = 0x200;
                *num_entries = 48;
            } else if (mmu_port == 60) {
                *base_index = 0x1f0;
                *num_entries = 8;
            } else if (mmu_port == 61) {
                *base_index = 0x1f8;
                *num_entries = 1;
            } else {
                return SOC_E_PARAM;
            }
        } else {
            *base_index = 0;
            *num_entries = 560;
        }
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
        if (mmu_port >= 0) {
            *base_index = si->port_cosq_base[port] + 1024;
            *num_entries = si->port_num_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = 1592;
        }
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
        if (mmu_port >= 0) {
            *base_index = si->port_uc_cosq_base[port];
            *num_entries = si->port_num_uc_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = 1592;
        }
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
    case SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK:
    case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
        if (mmu_port >= 0) {
            *base_index = si->port_uc_cosq_base[port];
            *num_entries = si->port_num_uc_cosq[port];
        } else {
            *base_index = 0;
            *num_entries = 1024;
        }
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
        *num_entries = 1;
        *base_index = port;
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
        if (mmu_port >= 0) {
            *num_entries = 1;
            *base_index = mmu_port;
        } else {
            return SOC_E_INTERNAL;
        }
        break;
    case SOC_COUNTER_NON_DMA_PG_MIN_PEAK:
    case SOC_COUNTER_NON_DMA_PG_MIN_CURRENT:
    case SOC_COUNTER_NON_DMA_PG_SHARED_PEAK:
    case SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT:
    case SOC_COUNTER_NON_DMA_PG_HDRM_PEAK:
    case SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT:
        if (mmu_port >= 0) {
            *num_entries = 8;
            *base_index = mmu_port * (*num_entries);
        } else {
            *base_index = 0;
            *num_entries = 504;
        }
        break;
    case SOC_COUNTER_NON_DMA_QUEUE_PEAK:
    case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
        *num_entries = 48;
        *base_index = port * (*num_entries);
        break;
    case SOC_COUNTER_NON_DMA_POOL_PEAK:
    case SOC_COUNTER_NON_DMA_POOL_CURRENT:
        *base_index = 0;
        *num_entries = non_dma->num_entries;
        break;
    default:
        return SOC_E_INTERNAL;
    }
    *base_index += non_dma->base_index;

    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
STATIC int
_soc_counter_katana_get_info(int unit, soc_port_t port, soc_reg_t id,
                              int *base_index, int *num_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    int max_ports = 0;
#if defined (BCM_SABER2_SUPPORT)
    soc_info_t *si = &SOC_INFO(unit);
#endif
    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    max_ports = (SOC_IS_KATANA(unit)) ? SOC_MAX_NUM_PORTS :
                                        SOC_MAX_NUM_PP_PORTS;
#if defined (BCM_SABER2_SUPPORT)
             if (SOC_IS_SABER2(unit)) {
                 max_ports = si->cpu_hg_pp_port_index;
             }
#endif
    if (port >= 0 ) {
        if (port < max_ports) {
            if (SOC_IS_KATANA2(unit)) {
               *base_index = SOC_INFO(unit).port_uc_cosq_base[port];
            }
            else if (SOC_IS_KATANAX(unit)) {
               *base_index = SOC_INFO(unit).port_cosq_base[port];
            }
            *num_entries = SOC_INFO(unit).port_num_uc_cosq[port];
            *base_index += non_dma->base_index;
        } else {
            /* extended queues */
            *base_index = port;
            *num_entries = non_dma->num_entries;
            *base_index += non_dma->base_index;
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
int
_soc_counter_katana2_get_mcindex(int unit, soc_port_t port, soc_reg_t id,
                              int *base_index, int *num_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    int max_ports = 0;
    soc = SOC_CONTROL(unit);
    if (id < NUM_SOC_REG) {
        return SOC_E_PARAM;
    }
    if (id >= SOC_COUNTER_NON_DMA_END) {
        return SOC_E_PARAM;
    }
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];
    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }
    max_ports = SOC_MAX_NUM_PP_PORTS;

    if (port >= 0 ) {
        if (port < max_ports) {
               *base_index = SOC_INFO(unit).port_cosq_base[port];
            *num_entries = SOC_INFO(unit).port_num_uc_cosq[port];
            *base_index += non_dma->base_index;
        } else {
            *base_index = -1;
        }
    }

    return SOC_E_NONE;
}
#endif

#ifdef BCM_GREYHOUND2_SUPPORT
STATIC int
_soc_counter_gh2_get_info(int unit, soc_port_t port, soc_reg_t id,
                          int *base_index, int *num_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    soc_port_t phy_port, mmu_port;
    int i;

    if ((NULL == base_index) || (NULL == num_entries)) {
        return SOC_E_PARAM;
    }

    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    if (port >= 0) {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    } else {
        phy_port = mmu_port = -1;
    }

    if (non_dma->entries_per_port == 1) {
        /* This non-dma counter is per port */
        *base_index = non_dma->base_index + port;
        *num_entries = 1;
    } else {
        switch (id) {
        case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
        case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
            if (mmu_port >= 0) {
                if (mmu_port >= SOC_GH2_64Q_MMU_PORT_IDX_MIN) {
                    /* Port with 64 COSQ */
                    *num_entries = SOC_GH2_2LEVEL_QUEUE_NUM;
                    *base_index =
                        (SOC_GH2_64Q_MMU_PORT_IDX_MIN *
                        SOC_GH2_LEGACY_QUEUE_NUM) +
                        ((mmu_port - SOC_GH2_64Q_MMU_PORT_IDX_MIN) *
                        (*num_entries));
                } else {
                     /* Port with 8 COSQ */
                    *num_entries = SOC_GH2_LEGACY_QUEUE_NUM;
                    *base_index = mmu_port * (*num_entries);
                }
            } else {
                *base_index = 0;
                *num_entries =
                    (SOC_GH2_64Q_MMU_PORT_IDX_MIN *
                    SOC_GH2_LEGACY_QUEUE_NUM) +
                    ((SOC_GH2_64Q_MMU_PORT_IDX_MAX -
                    SOC_GH2_64Q_MMU_PORT_IDX_MIN + 1) *
                    SOC_GH2_2LEVEL_QUEUE_NUM);
            }
            *base_index += non_dma->base_index;
            break;
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
            *base_index = non_dma->base_index;
            for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
                if (i == port) {
                    break;
                }
                *base_index += SOC_GH2_2LEVEL_QUEUE_NUM;
            }
            *num_entries = SOC_GH2_2LEVEL_QUEUE_NUM;
            break;
        default:
            *base_index = non_dma->base_index;
            for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
                if (i == port) {
                    break;
                }
                *base_index += num_cosq[unit][i];
            }
            *num_entries = num_cosq[unit][port];
            return SOC_E_NONE;
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_FIRELIGHT_SUPPORT
STATIC int
_soc_counter_fl_get_info(int unit, soc_port_t port, soc_reg_t id,
                          int *base_index, int *num_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    soc_port_t phy_port, mmu_port;
    int i;

    if ((NULL == base_index) || (NULL == num_entries)) {
        return SOC_E_PARAM;
    }

    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    if (port >= 0) {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    } else {
        phy_port = mmu_port = -1;
    }

    if (non_dma->entries_per_port == 1) {
        /* This non-dma counter is per port */
        *base_index = non_dma->base_index + port;
        *num_entries = 1;
    } else {
        switch (id) {
        case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
        case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
            if (mmu_port >= 0) {
                if (mmu_port >= SOC_FL_64Q_MMU_PORT_IDX_MIN) {
                    /* Port with 64 COSQ */
                    *num_entries = SOC_FL_2LEVEL_QUEUE_NUM;
                    *base_index =
                        (SOC_FL_64Q_MMU_PORT_IDX_MIN *
                        SOC_FL_LEGACY_QUEUE_NUM) +
                        ((mmu_port - SOC_FL_64Q_MMU_PORT_IDX_MIN) *
                        (*num_entries));
                } else {
                     /* Port with 8 COSQ */
                    *num_entries = SOC_FL_LEGACY_QUEUE_NUM;
                    *base_index = mmu_port * (*num_entries);
                }
            } else {
                *base_index = 0;
                *num_entries =
                    (SOC_FL_64Q_MMU_PORT_IDX_MIN *
                    SOC_FL_LEGACY_QUEUE_NUM) +
                    ((SOC_FL_64Q_MMU_PORT_IDX_MAX -
                    SOC_FL_64Q_MMU_PORT_IDX_MIN + 1) *
                    SOC_FL_2LEVEL_QUEUE_NUM);
            }
            *base_index += non_dma->base_index;
            break;
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
            *base_index = non_dma->base_index;
            for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
                if (i == port) {
                    break;
                }
                *base_index += SOC_FL_2LEVEL_QUEUE_NUM;
            }
            *num_entries = SOC_FL_2LEVEL_QUEUE_NUM;
            break;
        default:
            *base_index = non_dma->base_index;
            for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
                if (i == port) {
                    break;
                }
                *base_index += num_cosq[unit][i];
            }
            *num_entries = num_cosq[unit][port];
            return SOC_E_NONE;
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_FIRELIGHT_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
STATIC int
_soc_counter_fb_get_info(int unit, soc_port_t port, soc_reg_t id,
                         int *base_index, int *num_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    int i;

    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    if (non_dma->entries_per_port == 1) {
        /* This non-dma counter is per port */
        *base_index = non_dma->base_index + port;
        *num_entries = 1;
    } else {
        if (SOC_IS_SC_CQ(unit)) {
            /* This non-dma counter is per cosq */
            if (non_dma->flags & _SOC_COUNTER_NON_DMA_PERQ_REG) {
                *base_index = non_dma->base_index;
                for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
                    if (i == port) {
                        break;
                    }
                    *base_index += num_cosq[unit][i];
                }
                *num_entries = num_cosq[unit][port];
            } else {
                /* This non-dma counter is per uc/mc q */
                *base_index = non_dma->base_index +
                              (non_dma->entries_per_port * port);
                *num_entries = non_dma->entries_per_port;
            }
        } else {
            *base_index = non_dma->base_index;
            for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
                if (i == port) {
                    break;
                }
                *base_index += num_cosq[unit][i];
            }
            *num_entries = num_cosq[unit][port];
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
STATIC int
_soc_counter_shadow_get_info(int unit, soc_port_t port, soc_reg_t id,
                         int *base_index, int *num_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;

    soc = SOC_CONTROL(unit);
    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
        return SOC_E_UNAVAIL;
    }

    *base_index = non_dma->base_index;
    *num_entries = 1;

    return SOC_E_NONE;
}
#endif /* BCM_SHADOW_SUPPORT */

int
_soc_controlled_counter_get_info(int unit, soc_port_t port, soc_reg_t ctr_reg,
                            int *base_index, int *num_entries, char **cname)
{
    soc_control_t       *soc = SOC_CONTROL(unit);

    if (ctr_reg < 0) {
        return SOC_E_PARAM;
    }

    if (soc->controlled_counters == NULL) {
        return SOC_E_UNAVAIL;
    }

    if(!COUNTER_IS_COLLECTED(soc->controlled_counters[ctr_reg])) {
        return SOC_E_PARAM;
    }

    {
        *base_index = COUNTER_IDX_PORTBASE(unit, port) + soc->controlled_counters[ctr_reg].counter_idx;
        *num_entries = 1; /* MAC counters are non-array */
    }

    if (NULL != cname) {
        *cname = soc->controlled_counters[ctr_reg].cname;
    }

    return SOC_E_NONE;
}

#ifdef BCM_PETRA_SUPPORT
STATIC int
_soc_counter_arad_get_info(int unit, soc_port_t port, soc_reg_t ctr_reg,
                            int *base_index, int *num_entries, char **cname)
{
    int rv = SOC_E_NONE;

    if (_SOC_CONTROLLED_COUNTER_USE(unit, port)) {
        rv = _soc_controlled_counter_get_info(unit, port, ctr_reg, base_index,
                                             num_entries, cname);

    }  else if (ctr_reg >= NUM_SOC_REG) {
        if (ctr_reg >= SOC_COUNTER_NON_DMA_END) {
            return SOC_E_PARAM;
        } else {
            /*non dma counters*/
            return SOC_E_UNAVAIL;
        }
    } else  {
        *base_index = COUNTER_IDX_PORTBASE(unit, port) +
            SOC_REG_CTR_IDX(unit, ctr_reg) + _SOC_CONTROLLED_COUNTER_NOF(unit)/*added shift for tbe controlled registers*/;

        *num_entries = SOC_REG_NUMELS(unit, ctr_reg);
        if (cname) {
            *cname = SOC_REG_NAME(unit, ctr_reg);
        }
    }
    return rv;

}
#endif /* BCM_PETRA_SUPPORT */

#ifdef SOC_COUNTER_TABLE_SUPPORT
STATIC int
_soc_counter_tbl_get_info(int unit, soc_port_t port, int ctr_reg,
                          soc_ctr_tbl_entry_t **entry) {
    int i;
    int ctr_fld;
    soc_ctr_tbl_info_t   *ctr_tbl_info = SOC_CTR_TBL_INFO(unit);
    soc_ctr_tbl_entry_t  *ctr_tbl_entry = NULL;
    int entry_num = 0;

    if ((entry == NULL) || (ctr_reg < SOC_COUNTER_TABLE_FIELD_START)) {
         return SOC_E_PARAM;
     }

    ctr_tbl_entry = ctr_tbl_info->tables;
    entry_num = ctr_tbl_info->num;

    ctr_fld = ctr_reg - SOC_COUNTER_TABLE_FIELD_START;

    for(i=0; i < entry_num; i++) {
        if(ctr_fld == ctr_tbl_entry->ctr_field) {
            *entry = ctr_tbl_entry;
            return SOC_E_NONE;
        }
        ctr_tbl_entry++;
    }

    return SOC_E_NOT_FOUND;
}
#endif /* SOC_COUNTER_TABLE_SUPPORT */

STATIC int
_soc_counter_get_info(int unit, soc_port_t port, soc_reg_t ctr_reg,
                      int *base_index, int *num_entries, char **cname)
{
    int rv;
#ifdef BCM_TOMAHAWK_SUPPORT
    soc_reg_t ctr_reg_offset = ctr_reg;
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef SOC_COUNTER_TABLE_SUPPORT
    soc_ctr_tbl_entry_t *ctr_tbl_entry = NULL;
#endif /* SOC_COUNTER_TABLE_SUPPORT */

    if ((!(SOC_IS_TD2_TT2(unit) || SOC_IS_TRIUMPH3(unit) ||
           SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit))) && (port < 0)) {
        return SOC_E_PARAM;
    }

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        rv = _soc_counter_arad_get_info(unit, port, ctr_reg, base_index,
                                         num_entries, cname);
        return rv;
    }
#endif /* BCM_PETRA_SUPPORT */

    if (soc_feature(unit, soc_feature_controlled_counters)) {
            rv = _soc_controlled_counter_get_info(unit, port, ctr_reg, base_index,
                                             num_entries, cname);
            return rv;
    }

    if ((ctr_reg >= SOC_COUNTER_NON_DMA_START) &&
        (ctr_reg < SOC_COUNTER_NON_DMA_END)) {
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            /* TH has encodings for ctr_reg */
            ctr_reg &= 0x80ffffff;
        }
#endif /* BCM_TOMAHAWK_SUPPORT */

        if (ctr_reg >= SOC_COUNTER_NON_DMA_END) {
            return SOC_E_PARAM;
        }

#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            rv = soc_counter_tomahawk_get_info(unit, port, ctr_reg_offset,
                                                base_index, num_entries);
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
            rv = soc_counter_trident3_get_info(unit, port, ctr_reg_offset,
                                                base_index, num_entries);
        } else
#endif /* BCM_TRIDENT3_SUPPORT */

#if defined (BCM_MONTEREY_SUPPORT) && defined (CPRIMOD_SUPPORT)
        /*
         * This will be used only for CPRI counters.
         * For all the other counters info, Monterey uses Apache
         * counter get info.
         */
        if ((SOC_IS_MONTEREY(unit)) &&
            ((ctr_reg >= SOC_COUNTER_NON_DMA_CPRI_START) &&
            (ctr_reg <= SOC_COUNTER_NON_DMA_CPRI_END))) {
            rv = _soc_counter_monterey_cpri_get_info(unit, port, ctr_reg,
                                                     base_index, num_entries);
        } else
#endif /* BCM_MONTEREY_SUPPORT && CPRIMOD_SUPPORT */

#ifdef BCM_HURRICANE4_SUPPORT
        if (SOC_IS_HURRICANE4(unit)) {
            rv = soc_counter_hurricane4_get_info(unit, port, ctr_reg_offset,
                                                base_index, num_entries);
        } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
        if (SOC_IS_HELIX5(unit)) {
            rv = soc_counter_helix5_get_info(unit, port, ctr_reg_offset,
                                                base_index, num_entries);
        } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
        if (SOC_IS_FIREBOLT6(unit)) {
            rv = soc_counter_firebolt6_get_info(unit, port, ctr_reg_offset,
                                                base_index, num_entries);
        } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
        if (SOC_IS_APACHE(unit)) {
            rv = _soc_counter_apache_get_info(unit, port, ctr_reg,
                                                base_index, num_entries);
        } else
#endif /* BCM_APACHE_SUPPORT */
#if defined ( BCM_TRIDENT2_SUPPORT ) || defined ( BCM_TRIDENT2PLUS_SUPPORT )
        if (SOC_IS_TD2_TT2(unit)) {
            rv = _soc_counter_trident2_get_info(unit, port, ctr_reg, base_index,
                                               num_entries);
        } else
#endif /* BCM_TRIDENT2_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            rv = _soc_counter_trident_get_info(unit, port, ctr_reg, base_index,
                                               num_entries);
        } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            rv = _soc_counter_triumph3_get_info(unit, port, ctr_reg, base_index,
                                               num_entries);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANAX(unit)) {
            rv = _soc_counter_katana_get_info(unit, port, ctr_reg, base_index,
                                               num_entries);
        } else
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
        if (SOC_IS_GREYHOUND2(unit)) {
#if defined(BCM_FIRELIGHT_SUPPORT)
            if (soc_feature(unit, soc_feature_fl)) {
                    rv = _soc_counter_fl_get_info(unit, port, ctr_reg, base_index,
                                                   num_entries);
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                rv = _soc_counter_gh2_get_info(unit, port, ctr_reg, base_index,
                                               num_entries);
            }
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
        if (SOC_IS_FBX(unit) && !(SOC_IS_SHADOW(unit))) {
            rv = _soc_counter_fb_get_info(unit, port, ctr_reg, base_index,
                                          num_entries);
        } else
#endif /* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_SHADOW_SUPPORT
        if (SOC_IS_SHADOW(unit)) {
            rv = _soc_counter_shadow_get_info(unit, port, ctr_reg, base_index,
                                          num_entries);
        } else
#endif /* BCM_SHADOW_SUPPORT */
        {
            rv = SOC_E_UNAVAIL;
        }

        if (rv < 0) {
            return rv;
        }
        if (cname) {
            *cname = SOC_CONTROL(unit)->
                counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].cname;
        }

    } else {
        if ((ctr_reg < SOC_COUNTER_NON_DMA_START) &&
            !SOC_REG_IS_ENABLED(unit, ctr_reg)) {
            return SOC_E_PARAM;
        }
#ifdef BCM_SHADOW_SUPPORT
        if (SOC_IS_SHADOW(unit) && IS_IL_PORT(unit, port)) {
            return SOC_E_PORT;
        }
#endif /* BCM_SHADOW_SUPPORT */

#ifdef SOC_COUNTER_TABLE_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            if(ctr_reg >= SOC_COUNTER_TABLE_FIELD_START) {
                SOC_IF_ERROR_RETURN(
                    _soc_counter_tbl_get_info(unit, port, ctr_reg, &ctr_tbl_entry));
                *num_entries = 1;
                *base_index = COUNTER_IDX_PORTBASE(unit, port) + ctr_tbl_entry->ctr_idx;
                if (cname) {
                    *cname = SOC_FIELD_NAME(unit, ctr_tbl_entry->ctr_field);
                }
                return SOC_E_NONE;
            }
        }
#endif /* SOC_COUNTER_TABLE_SUPPORT */

        if(!SOC_REG_IS_VALID(unit, ctr_reg)) {
            return SOC_E_PARAM;
        }

        *base_index = COUNTER_IDX_PORTBASE(unit, port) +
            SOC_REG_CTR_IDX(unit, ctr_reg);
        if (SOC_IS_TRIUMPH3(unit)) {
            *base_index -= 0x20;
        }
#ifdef BCM_HURRICANE2_SUPPORT
        if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit) || \
            SOC_IS_GREYHOUND(unit)) {
            *base_index -= 0x37;
        }
#endif /* BCM_HURRICANE2_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
        if (SOC_IS_GREYHOUND2(unit)) {
            *base_index -= 0x6f;
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
        *num_entries = SOC_REG_NUMELS(unit, ctr_reg);
        if (cname) {
            *cname = SOC_REG_NAME(unit, ctr_reg);
        }
    }

    return SOC_E_NONE;
}

#ifdef SOC_COUNTER_TABLE_SUPPORT
STATIC int
_soc_ctr_tbl_field_get(int unit, soc_reg_t ctr_reg, int port,
                       soc_ctr_tbl_entry_t *ctr_tbl_info_entry,
                       uint64 *ctr_val) {
    soc_mem_t       mem;
    soc_field_t     ctr_fld;
    soc_format_t    format;
    int             copyno, index;
    uint32          row_ent[16];            
    uint32          fval[2];
    int             phy_port;

    if(ctr_reg < SOC_COUNTER_TABLE_FIELD_START) {
        return SOC_E_PARAM;
    }

    if(ctr_tbl_info_entry == NULL || ctr_val == NULL) {
        return SOC_E_PARAM;
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    phy_port -= SOC_COUNTER_TABLE_BASE_PORT;
    ctr_fld = ctr_tbl_info_entry->ctr_field;
    mem = ctr_tbl_info_entry->mem;
    format = ctr_tbl_info_entry->format;
    index = (phy_port % NUM_PORTS_PER_CDMIB_MEM) * NUM_CDMIB_MEM_ROWS_PER_PORT
            + ctr_tbl_info_entry->row_offset;
    copyno = SOC_MEM_BLOCK_MIN(unit, mem) +
            (phy_port / NUM_PORTS_PER_CDMIB_MEM);

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, copyno, index, row_ent));
    soc_format_field_get(unit, format, row_ent, ctr_fld, fval);
    COMPILER_64_SET(*ctr_val, fval[1], fval[0]);

    return SOC_E_NONE;
}

STATIC int
_soc_ctr_tbl_field_set(int unit, soc_reg_t ctr_reg, int port,
                       soc_ctr_tbl_entry_t *ctr_tbl_info_entry,
                       uint64 *ctr_val) {
    soc_mem_t       mem;
    soc_field_t     ctr_fld;
    soc_format_t    format;
    int             copyno, index;
    uint32          row_ent[16];        
    uint32          fval[2];
    int             phy_port;

    if(ctr_reg < SOC_COUNTER_TABLE_FIELD_START) {
        return SOC_E_PARAM;
    }

    if(ctr_tbl_info_entry == NULL || ctr_val == NULL) {
        return SOC_E_PARAM;
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    phy_port -= SOC_COUNTER_TABLE_BASE_PORT;
    ctr_fld = ctr_tbl_info_entry->ctr_field;
    mem = ctr_tbl_info_entry->mem;
    format = ctr_tbl_info_entry->format;
    index = (phy_port % NUM_PORTS_PER_CDMIB_MEM) * NUM_CDMIB_MEM_ROWS_PER_PORT
            + ctr_tbl_info_entry->row_offset;
    copyno = SOC_MEM_BLOCK_MIN(unit, mem) +
            (phy_port / NUM_PORTS_PER_CDMIB_MEM);

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, copyno, index, row_ent));
    COMPILER_64_TO_32_HI(fval[1], *ctr_val);
    COMPILER_64_TO_32_LO(fval[0], *ctr_val);
    soc_format_field_set(unit, format, row_ent, ctr_fld, fval);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, copyno, index, row_ent));

    return SOC_E_NONE;
}
#endif /* SOC_COUNTER_TABLE_SUPPORT */

/*
 * Function:
 *      soc_counter_idx_get
 * Purpose:
 *      Get the index of a counter given the counter and port
 * Parameters:
 *      unit - The SOC unit number
 *      reg - The register number
 *      port - The port for which index is being calculated
 * Returns:
 *      SOC_E_XXX, no it is not
 * Notes:
 */

int
soc_counter_idx_get(int unit, soc_reg_t reg, int ar_idx, int port)
{
    int base_index, num_entries;
    if (_soc_counter_get_info(unit, port, reg, &base_index, &num_entries,
                              NULL) < 0) {
        return -1;
    }

    if (ar_idx < 0) {
        return base_index;
    } else if (ar_idx < num_entries) {
        return base_index + ar_idx;
    } else {
        return -1;
    }
}

/*
 * Function:
 *      _soc_counter_num_cosq_init
 * Purpose:
 *      Initialize the number of COSQ per port.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports the unit has.
 * Returns:
 *      None.
 */
STATIC void
_soc_counter_num_cosq_init(int unit, int nports)
{
    /* Some compiler may generate false warning in partial build dead code */
    if (nports > SOC_MAX_NUM_PORTS) {
        return;
    }

#ifdef BCM_TRIUMPH_SUPPORT
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        return;
    }
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        return;
    }
#endif /* BCM_SHADOW_SUPPORT */
#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) || \
        SOC_IS_GREYHOUND2(unit)) {
        int port;

        /* all ports 8Q, including G, HG, CMIC and the reserved port 1 */
        for (port = 0; port < nports; port++) {
            num_cosq[unit][port] = 8;
        }

    } else
#endif /* BCM_HURRICANE_SUPPORT || BCM_GREYHOUND_SUPPORT*/
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)){
        const int port_24q[] = {26, 27, 28, 29, 30, 31, 34, 38, 39, 42, 43, 46, 50, 51};
        const int port_16q = 54;
        int port, i;

        for (port = 0; port < nports; port++) {
            if (IS_CPU_PORT(unit, port)) {
                num_cosq[unit][port] = NUM_CPU_COSQ(unit);
            } else {
                num_cosq[unit][port] = 8;
                for (i = 0; i < sizeof(port_24q) / sizeof(port_24q[1]); i++) {
                    if (port == port_24q[i]) {
                        num_cosq[unit][port] = 24;
                        break;
                    }
                }
                if ((port == port_16q) && (port < SOC_MAX_NUM_PORTS)) {
                    num_cosq[unit][port] = 16;
                }
            }
        }

    } else
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit)){
        const int port_24q[] = {26, 27, 28, 29};
        int port, i;

        for (port = 0; port < nports; port++) {
            if (IS_CPU_PORT(unit, port)) {
                num_cosq[unit][port] = NUM_CPU_COSQ(unit);
            } else {
                num_cosq[unit][port] = 8;
                for (i = 0; i < sizeof(port_24q) / sizeof(port_24q[1]); i++) {
                    if (port == port_24q[i]) {
                        num_cosq[unit][port] = 24;
                        break;
                    }
                }
            }
        }

    } else
#endif /* BCM_ENDURO_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        int port;

        for (port = 0; port < nports; port++) {
            if (IS_CPU_PORT(unit, port)) {
                num_cosq[unit][port] = NUM_CPU_COSQ(unit);
            /* coverity[overrun-local] */
            } else if (IS_HG_PORT(unit, port)) {
                num_cosq[unit][port] = 24;
            } else {
                num_cosq[unit][port] = 8;
            }
        }
    } else
#endif /* BCM_KATANA_SUPPORT */
    if (SOC_IS_TR_VL(unit)) {
        const int port_24q[] = {2, 3, 14, 15, 26, 27, 28, 29, 30, 31, 32, 43};
        int port, i;

        for (port = 0; port < nports; port++) {
            if (IS_CPU_PORT(unit, port)) {
                num_cosq[unit][port] = NUM_CPU_COSQ(unit);
            } else {
                num_cosq[unit][port] = 8;
                for (i = 0; i < sizeof(port_24q) / sizeof(port_24q[1]); i++) {
                    if (port == port_24q[i]) {
                        num_cosq[unit][port] = 24;
                        break;
                    }
                }
            }
        }
    } else
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_SC_CQ(unit) || SOC_IS_HB_GW(unit) || SOC_IS_FB_FX_HX(unit)) {
        int port;

        for (port = 0; port < nports; port++) {
            if (IS_CPU_PORT(unit, port)) {
                num_cosq[unit][port] = NUM_CPU_COSQ(unit);
            } else {
                num_cosq[unit][port] = 8;
            }
        }
    } else
#endif /* BCM_FIREBOLT_SUPPORT */
    {
        return;
    }

    return;
}

#ifdef BCM_TRIUMPH3_SUPPORT
STATIC int
_soc_counter_triumph3_non_dma_init(int unit, int nports,
                                   int non_dma_start_index,
                                   int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1, *non_dma2;
    int num_entries, alloc_size;
    uint32 *buf;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    /* EGR_PERQ_XMT_COUNTERS size depends on user's portmap config */
    num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    alloc_size = num_entries * sizeof(uint32) *
                 soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "MC_PERQ_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = EGR_PERQ_XMT_COUNTERSm;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                    SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "MC_PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 =  &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->pbmp = PBMP_ALL(unit);
    non_dma2->entries_per_port = 10;
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_PKT";

    non_dma2 =  &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->pbmp = PBMP_ALL(unit);
    non_dma2->entries_per_port = 10;
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_BYTE";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_MC_DROP_MEMm);
    non_dma0->mem = MMU_CTR_MC_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKT_CNTf;
    non_dma0->cname = "MCQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = non_dma0->num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTf;
    non_dma1->cname = "MCQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_UC_DROP_MEMm);
    non_dma0->mem = MMU_CTR_UC_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKT_CNTf;
    non_dma0->cname = "UCQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = non_dma0->num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTf;
    non_dma1->cname = "UCQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = DROP_PKT_CNT_INGr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "DROP_PKT_ING";
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_BYTE_CNT_ING_64r;
    non_dma1->cname = "DROP_BYTE_ING";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = 63; /* one per port */
    non_dma0->mem = MMU_CTR_COLOR_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKT_CNTf;
    non_dma0->cname = "DROP_PKT_YEL";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = 63 * 4;
    non_dma0->dma_index_max[0] = 63 * 4 + 62;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "DROP_PKT_RED";
    non_dma1->dma_index_min[0] = 63 * 3;
    non_dma1->dma_index_max[0] = 63 * 3 + 62;
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_GRE";
    non_dma1->dma_index_min[0] = 63 * 2;
    non_dma1->dma_index_max[0] = 63 * 2 + 62;
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_YEL";
    non_dma1->dma_index_min[0] = 63 * 1;
    non_dma1->dma_index_max[0] = 66 * 1 + 62;
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_RED";
    non_dma1->dma_index_min[0] = 0;
    non_dma1->dma_index_max[0] = 62;
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 4;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = TOTAL_BUFFER_COUNT_CELL_SPr;
    non_dma0->field = TOTAL_BUFFER_COUNTf;
    non_dma0->cname = "POOL_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "POOL_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->num_entries = nports * non_dma0->entries_per_port;
    non_dma0->dma_index_max[0] = non_dma0->num_entries - 1;
    non_dma0->mem = THDI_PORT_PG_CNTRSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG_MIN_COUNTf;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_mem[0] = non_dma0->mem;
    non_dma0->dma_index_max[0] = non_dma0->num_entries - 1;
    non_dma0->cname = "PG_MIN_PEAK";
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
            _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_MIN_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PG_SHARED_COUNTf;
    non_dma1->cname = "PG_SHARED_PEAK";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_SHARED_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PG_HDRM_COUNTf;
    non_dma1->cname = "PG_HDRM_PEAK";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_HDRM_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma1->pbmp = PBMP_PORT_ALL(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->entries_per_port = 48;
    non_dma1->num_entries = nports * non_dma1->entries_per_port;
    non_dma1->reg = OP_QUEUE_TOTAL_COUNT_CELLr;
    non_dma1->mem = INVALIDm;
    non_dma1->field = Q_TOTAL_COUNT_CELLf;
    non_dma1->cname = "MC_QUEUE_PEAK";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "MC_QUEUE_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma1->pbmp = PBMP_PORT_ALL(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->entries_per_port = 10;
    non_dma1->num_entries = soc_mem_index_count(unit, MMU_THDO_COUNTER_QUEUEm);
    non_dma1->reg = INVALIDr;
    non_dma1->mem = MMU_THDO_COUNTER_QUEUEm;
    non_dma1->field =  SHARED_COUNTf;
    non_dma1->dma_buf[0] = buf;
    non_dma1->cname = "UC_QUEUE_PEAK";
    non_dma1->dma_mem[0] = non_dma1->mem;
    non_dma1->dma_index_max[0] = non_dma1->num_entries - 1;
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "UC_QUEUE_CUR";
    return SOC_E_NONE;
}
#endif


#ifdef BCM_TRIDENT_SUPPORT
/*
 * Function:
 *      _soc_counter_trident_non_dma_init
 * Purpose:
 *      Initialize Trident's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports.
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
STATIC int
_soc_counter_trident_non_dma_init(int unit, int nports, int non_dma_start_index,
        int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1, *non_dma2;
    int entry_words, num_entries[2], alloc_size, table_size;
    uint32 *buf;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    soc_trident_get_egr_perq_xmt_counters_size(unit, &num_entries[0],
                                               &num_entries[1]);
    entry_words = soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm);

    /* EGR_PERQ_XMT_COUNTERS size depends on user's portmap config */
    alloc_size = (num_entries[0] + num_entries[1]) * entry_words * sizeof(uint32);

    /* MMU_CTR_UC_DROP_MEM is the largest among MMU_CTR_*_DROP_MEM tables */
    table_size = soc_mem_index_count(unit, MMU_CTR_UC_DROP_MEMm) *
        soc_mem_entry_words(unit, MMU_CTR_UC_DROP_MEMm) * sizeof(uint32);
    if (alloc_size < table_size) {
        alloc_size = table_size;
    }

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* cpu port has max number of queues */
    non_dma0->num_entries = num_entries[0] + num_entries[1];
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "MC_PERQ_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = num_entries[0] - 1;
    non_dma0->dma_mem[0] = EGR_PERQ_XMT_COUNTERS_Xm;
#if (SOC_MAX_NUM_PIPES > 1)
    non_dma0->dma_buf[1] = &buf[num_entries[0] * entry_words];
    non_dma0->dma_index_max[1] = num_entries[1] - 1;
    non_dma0->dma_mem[1] = EGR_PERQ_XMT_COUNTERS_Ym;
#endif
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "MC_PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 10;
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_PKT";

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 10;
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_BYTE";

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 64;
    non_dma2->num_entries = 0;
    non_dma2->cname = "EXT_PERQ_PKT";

    non_dma2 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 64;
    non_dma2->num_entries = 0;
    non_dma2->cname = "EXT_PERQ_BYTE";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_MC_DROP_MEMm);
    non_dma0->mem = MMU_CTR_MC_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKT_CNTf;
    non_dma0->cname = "MCQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = non_dma0->num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTf;
    non_dma1->cname = "MCQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 74; /*  10 ucast + 64 ext ucast */
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_UC_DROP_MEMm);
    non_dma0->mem = MMU_CTR_UC_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKT_CNTf;
    non_dma0->cname = "UCQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = non_dma0->num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTf;
    non_dma1->cname = "UCQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = DROP_PKT_CNT_INGr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "DROP_PKT_ING";
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_BYTE_CNT_ING_64r;
    non_dma1->cname = "DROP_BYTE_ING";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = 66; /* one per port */
    non_dma0->mem = MMU_CTR_COLOR_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKT_CNTf;
    non_dma0->cname = "DROP_PKT_YEL";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = 66 * 4;
    non_dma0->dma_index_max[0] = 66 * 4 + 65;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "DROP_PKT_RED";
    non_dma1->dma_index_min[0] = 66 * 3;
    non_dma1->dma_index_max[0] = 66 * 3 + 65;
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_GRE";
    non_dma1->dma_index_min[0] = 66 * 2;
    non_dma1->dma_index_max[0] = 66 * 2 + 65;
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_YEL";
    non_dma1->dma_index_min[0] = 66 * 1;
    non_dma1->dma_index_max[0] = 66 * 1 + 65;
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_RED";
    non_dma1->dma_index_min[0] = 0;
    non_dma1->dma_index_max[0] = 65;
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 4;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = TOTAL_BUFFER_COUNT_CELL_SPr;
    non_dma0->field = TOTAL_BUFFER_COUNTf;
    non_dma0->cname = "POOL_PEAK";
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "POOL_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->num_entries = nports * non_dma0->entries_per_port;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = PG_MIN_COUNT_CELLr;
    non_dma0->field = PG_MIN_COUNTf;
    non_dma0->cname = "PG_MIN_PEAK";
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_MIN_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PG_SHARED_COUNT_CELLr;
    non_dma1->field = PG_SHARED_COUNTf;
    non_dma1->cname = "PG_SHARED_PEAK";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_SHARED_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PG_HDRM_COUNT_CELLr;
    non_dma1->field = PG_HDRM_COUNTf;
    non_dma1->cname = "PG_HDRM_PEAK";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_HDRM_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->pbmp = PBMP_ALL(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->entries_per_port = 5;
    non_dma1->num_entries = nports * non_dma0->entries_per_port;
    non_dma1->reg = OP_QUEUE_TOTAL_COUNT_CELLr;
    non_dma1->field = Q_TOTAL_COUNT_CELLf;
    non_dma1->cname = "MC_QUEUE_PEAK";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "MC_QUEUE_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->pbmp = PBMP_PORT_ALL(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->entries_per_port = 10;
    non_dma1->num_entries = nports * non_dma1->entries_per_port;
    non_dma1->reg = OP_UC_QUEUE_TOTAL_COUNT_CELLr;
    non_dma1->field =  Q_TOTAL_COUNT_CELLf;
    non_dma1->cname = "UC_QUEUE_PEAK";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "UC_QUEUE_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EXT_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->pbmp = PBMP_EQ(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->entries_per_port = 64;
    non_dma1->num_entries = nports * non_dma1->entries_per_port;
    non_dma1->reg = OP_EX_QUEUE_TOTAL_COUNT_CELLr;
    non_dma1->cname = "EXT_QUEUE_PEAK";
    non_dma1->field =  Q_TOTAL_COUNT_CELLf;
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EXT_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma2, non_dma1, sizeof(soc_counter_non_dma_t));
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "EXT_QUEUE_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_MMU_QCN_CNM -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 2;
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_QCN_CNM_COUNTERm);
    non_dma0->mem = MMU_QCN_CNM_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = CNM_CNTf;
    non_dma0->cname = "QCN_CNM_COUNTER";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_mem[0] = non_dma0->mem;
    non_dma0->dma_index_min[0] = 0;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    *non_dma_entries += non_dma0->num_entries;

    return SOC_E_NONE;
}
#endif /* BCM_TRIDENT_SUPPORT */

#if defined (BCM_MONTEREY_SUPPORT)
/* Function to initialize extra counters  for monterey*/
STATIC int
soc_counter_monterey_extra_obm_ctrs_init(int unit,
                                   soc_counter_non_dma_t *non_dma_parent,
                                   soc_counter_non_dma_t *non_dma_extra,
                                   uint32 extra_ctr_ct,
                                   int *total_entries)
{
#define LOSSY_LO  0
#define LOSSY_HI  1
#define LOSSLESS0 2
#define LOSSLESS1 3
#define EXPRESS   4

#define COUNTER_TYPE_DROP    0
#define COUNTER_TYPE_ENQUEUE 1
    char obm_type_str[][12] = {"LOSSY_LO", "LOSSY_HI",
                               "LOSSLESS0", "LOSSLESS1", "EXPRESS"};
    char obm_ctr_type_str[][10] = {"DROP", "ENQUEUE"};
    int  i, byte_flag = 0, obm_type = LOSSY_LO, parent_base_index = 0;
    int  counter_type = COUNTER_TYPE_DROP;
    uint32 count = 0;

    soc_reg_t obm_drop_pkt_regs[][2][5] = {
        {
         {IDB_OBM0_LOSSY_LO_PKT_DROP_COUNTr,
          IDB_OBM0_LOSSY_HI_PKT_DROP_COUNTr,
          IDB_OBM0_LOSSLESS0_PKT_DROP_COUNTr,
          IDB_OBM0_LOSSLESS1_PKT_DROP_COUNTr,
          IDB_OBM0_EXPRESS_PKT_DROP_COUNTr},

         {IDB_OBM0_LOSSY_LO_ARRIVAL_PKT_COUNTr,
          IDB_OBM0_LOSSY_HI_ARRIVAL_PKT_COUNTr,
          IDB_OBM0_LOSSLESS0_ARRIVAL_PKT_COUNTr,
          IDB_OBM0_LOSSLESS1_ARRIVAL_PKT_COUNTr,
          IDB_OBM0_EXPRESS_ARRIVAL_PKT_COUNTr}
        },
        {
         {IDB_OBM1_LOSSY_LO_PKT_DROP_COUNTr,
          IDB_OBM1_LOSSY_HI_PKT_DROP_COUNTr,
          IDB_OBM1_LOSSLESS0_PKT_DROP_COUNTr,
          IDB_OBM1_LOSSLESS1_PKT_DROP_COUNTr,
          IDB_OBM1_EXPRESS_PKT_DROP_COUNTr},

         {IDB_OBM1_LOSSY_LO_ARRIVAL_PKT_COUNTr,
          IDB_OBM1_LOSSY_HI_ARRIVAL_PKT_COUNTr,
          IDB_OBM1_LOSSLESS0_ARRIVAL_PKT_COUNTr,
          IDB_OBM1_LOSSLESS1_ARRIVAL_PKT_COUNTr,
          IDB_OBM1_EXPRESS_ARRIVAL_PKT_COUNTr}
        },
        {
         {IDB_OBM2_LOSSY_LO_PKT_DROP_COUNTr,
          IDB_OBM2_LOSSY_HI_PKT_DROP_COUNTr,
          IDB_OBM2_LOSSLESS0_PKT_DROP_COUNTr,
          IDB_OBM2_LOSSLESS1_PKT_DROP_COUNTr,
          IDB_OBM2_EXPRESS_PKT_DROP_COUNTr},

         {IDB_OBM2_LOSSY_LO_ARRIVAL_PKT_COUNTr,
          IDB_OBM2_LOSSY_HI_ARRIVAL_PKT_COUNTr,
          IDB_OBM2_LOSSLESS0_ARRIVAL_PKT_COUNTr,
          IDB_OBM2_LOSSLESS1_ARRIVAL_PKT_COUNTr,
          IDB_OBM2_EXPRESS_ARRIVAL_PKT_COUNTr}
        },
        {
         {IDB_OBM3_LOSSY_LO_PKT_DROP_COUNTr,
          IDB_OBM3_LOSSY_HI_PKT_DROP_COUNTr,
          IDB_OBM3_LOSSLESS0_PKT_DROP_COUNTr,
          IDB_OBM3_LOSSLESS1_PKT_DROP_COUNTr,
          IDB_OBM3_EXPRESS_PKT_DROP_COUNTr},

         {IDB_OBM3_LOSSY_LO_ARRIVAL_PKT_COUNTr,
          IDB_OBM3_LOSSY_HI_ARRIVAL_PKT_COUNTr,
          IDB_OBM3_LOSSLESS0_ARRIVAL_PKT_COUNTr,
          IDB_OBM3_LOSSLESS1_ARRIVAL_PKT_COUNTr,
          IDB_OBM3_EXPRESS_ARRIVAL_PKT_COUNTr}
        },
        {
         {IDB_OBM4_LOSSY_LO_PKT_DROP_COUNTr,
          IDB_OBM4_LOSSY_HI_PKT_DROP_COUNTr,
          IDB_OBM4_LOSSLESS0_PKT_DROP_COUNTr,
          IDB_OBM4_LOSSLESS1_PKT_DROP_COUNTr,
          IDB_OBM4_EXPRESS_PKT_DROP_COUNTr},

         {IDB_OBM4_LOSSY_LO_ARRIVAL_PKT_COUNTr,
          IDB_OBM4_LOSSY_HI_ARRIVAL_PKT_COUNTr,
          IDB_OBM4_LOSSLESS0_ARRIVAL_PKT_COUNTr,
          IDB_OBM4_LOSSLESS1_ARRIVAL_PKT_COUNTr,
          IDB_OBM4_EXPRESS_ARRIVAL_PKT_COUNTr}
        },
        {
         {IDB_OBM5_LOSSY_LO_PKT_DROP_COUNTr,
          IDB_OBM5_LOSSY_HI_PKT_DROP_COUNTr,
          IDB_OBM5_LOSSLESS0_PKT_DROP_COUNTr,
          IDB_OBM5_LOSSLESS1_PKT_DROP_COUNTr,
          IDB_OBM5_EXPRESS_PKT_DROP_COUNTr},

         {IDB_OBM5_LOSSY_LO_ARRIVAL_PKT_COUNTr,
          IDB_OBM5_LOSSY_HI_ARRIVAL_PKT_COUNTr,
          IDB_OBM5_LOSSLESS0_ARRIVAL_PKT_COUNTr,
          IDB_OBM5_LOSSLESS1_ARRIVAL_PKT_COUNTr,
          IDB_OBM5_EXPRESS_ARRIVAL_PKT_COUNTr}
        },
        {
         {IDB_OBM6_LOSSY_LO_PKT_DROP_COUNTr,
          IDB_OBM6_LOSSY_HI_PKT_DROP_COUNTr,
          IDB_OBM6_LOSSLESS0_PKT_DROP_COUNTr,
          IDB_OBM6_LOSSLESS1_PKT_DROP_COUNTr,
          IDB_OBM6_EXPRESS_PKT_DROP_COUNTr},

         {IDB_OBM6_LOSSY_LO_ARRIVAL_PKT_COUNTr,
          IDB_OBM6_LOSSY_HI_ARRIVAL_PKT_COUNTr,
          IDB_OBM6_LOSSLESS0_ARRIVAL_PKT_COUNTr,
          IDB_OBM6_LOSSLESS1_ARRIVAL_PKT_COUNTr,
          IDB_OBM6_EXPRESS_ARRIVAL_PKT_COUNTr}
        },
        {
         {IDB_OBM7_LOSSY_LO_PKT_DROP_COUNTr,
          IDB_OBM7_LOSSY_HI_PKT_DROP_COUNTr,
          IDB_OBM7_LOSSLESS0_PKT_DROP_COUNTr,
          IDB_OBM7_LOSSLESS1_PKT_DROP_COUNTr,
          IDB_OBM7_EXPRESS_PKT_DROP_COUNTr},

         {IDB_OBM7_LOSSY_LO_ARRIVAL_PKT_COUNTr,
          IDB_OBM7_LOSSY_HI_ARRIVAL_PKT_COUNTr,
          IDB_OBM7_LOSSLESS0_ARRIVAL_PKT_COUNTr,
          IDB_OBM7_LOSSLESS1_ARRIVAL_PKT_COUNTr,
          IDB_OBM7_EXPRESS_ARRIVAL_PKT_COUNTr}
        }
    };
    soc_reg_t obm_drop_byte_regs[][2][5] = {
        {
         {IDB_OBM0_LOSSY_LO_BYTE_DROP_COUNTr,
          IDB_OBM0_LOSSY_HI_BYTE_DROP_COUNTr,
          IDB_OBM0_LOSSLESS0_BYTE_DROP_COUNTr,
          IDB_OBM0_LOSSLESS1_BYTE_DROP_COUNTr,
          IDB_OBM0_EXPRESS_BYTE_DROP_COUNTr},

         {IDB_OBM0_LOSSY_LO_ARRIVAL_BYTE_COUNTr,
          IDB_OBM0_LOSSY_HI_ARRIVAL_BYTE_COUNTr,
          IDB_OBM0_LOSSLESS0_ARRIVAL_BYTE_COUNTr,
          IDB_OBM0_LOSSLESS1_ARRIVAL_BYTE_COUNTr,
          IDB_OBM0_EXPRESS_ARRIVAL_BYTE_COUNTr}
        },
        {
         {IDB_OBM1_LOSSY_LO_BYTE_DROP_COUNTr,
          IDB_OBM1_LOSSY_HI_BYTE_DROP_COUNTr,
          IDB_OBM1_LOSSLESS0_BYTE_DROP_COUNTr,
          IDB_OBM1_LOSSLESS1_BYTE_DROP_COUNTr,
          IDB_OBM1_EXPRESS_BYTE_DROP_COUNTr},

         {IDB_OBM1_LOSSY_LO_ARRIVAL_BYTE_COUNTr,
          IDB_OBM1_LOSSY_HI_ARRIVAL_BYTE_COUNTr,
          IDB_OBM1_LOSSLESS0_ARRIVAL_BYTE_COUNTr,
          IDB_OBM1_LOSSLESS1_ARRIVAL_BYTE_COUNTr,
          IDB_OBM1_EXPRESS_ARRIVAL_BYTE_COUNTr}
        },
        {
         {IDB_OBM2_LOSSY_LO_BYTE_DROP_COUNTr,
          IDB_OBM2_LOSSY_HI_BYTE_DROP_COUNTr,
          IDB_OBM2_LOSSLESS0_BYTE_DROP_COUNTr,
          IDB_OBM2_LOSSLESS1_BYTE_DROP_COUNTr,
          IDB_OBM2_EXPRESS_BYTE_DROP_COUNTr},

         {IDB_OBM2_LOSSY_LO_ARRIVAL_BYTE_COUNTr,
          IDB_OBM2_LOSSY_HI_ARRIVAL_BYTE_COUNTr,
          IDB_OBM2_LOSSLESS0_ARRIVAL_BYTE_COUNTr,
          IDB_OBM2_LOSSLESS1_ARRIVAL_BYTE_COUNTr,
          IDB_OBM2_EXPRESS_ARRIVAL_BYTE_COUNTr}
        },
        {
         {IDB_OBM3_LOSSY_LO_BYTE_DROP_COUNTr,
          IDB_OBM3_LOSSY_HI_BYTE_DROP_COUNTr,
          IDB_OBM3_LOSSLESS0_BYTE_DROP_COUNTr,
          IDB_OBM3_LOSSLESS1_BYTE_DROP_COUNTr,
          IDB_OBM3_EXPRESS_BYTE_DROP_COUNTr},

         {IDB_OBM3_LOSSY_LO_ARRIVAL_BYTE_COUNTr,
          IDB_OBM3_LOSSY_HI_ARRIVAL_BYTE_COUNTr,
          IDB_OBM3_LOSSLESS0_ARRIVAL_BYTE_COUNTr,
          IDB_OBM3_LOSSLESS1_ARRIVAL_BYTE_COUNTr,
          IDB_OBM3_EXPRESS_ARRIVAL_BYTE_COUNTr}
        },
        {
         {IDB_OBM4_LOSSY_LO_BYTE_DROP_COUNTr,
          IDB_OBM4_LOSSY_HI_BYTE_DROP_COUNTr,
          IDB_OBM4_LOSSLESS0_BYTE_DROP_COUNTr,
          IDB_OBM4_LOSSLESS1_BYTE_DROP_COUNTr,
          IDB_OBM4_EXPRESS_BYTE_DROP_COUNTr},

         {IDB_OBM4_LOSSY_LO_ARRIVAL_BYTE_COUNTr,
          IDB_OBM4_LOSSY_HI_ARRIVAL_BYTE_COUNTr,
          IDB_OBM4_LOSSLESS0_ARRIVAL_BYTE_COUNTr,
          IDB_OBM4_LOSSLESS1_ARRIVAL_BYTE_COUNTr,
          IDB_OBM4_EXPRESS_ARRIVAL_BYTE_COUNTr}
        },
        {
         {IDB_OBM5_LOSSY_LO_BYTE_DROP_COUNTr,
          IDB_OBM5_LOSSY_HI_BYTE_DROP_COUNTr,
          IDB_OBM5_LOSSLESS0_BYTE_DROP_COUNTr,
          IDB_OBM5_LOSSLESS1_BYTE_DROP_COUNTr,
          IDB_OBM5_EXPRESS_BYTE_DROP_COUNTr},

         {IDB_OBM5_LOSSY_LO_ARRIVAL_BYTE_COUNTr,
          IDB_OBM5_LOSSY_HI_ARRIVAL_BYTE_COUNTr,
          IDB_OBM5_LOSSLESS0_ARRIVAL_BYTE_COUNTr,
          IDB_OBM5_LOSSLESS1_ARRIVAL_BYTE_COUNTr,
          IDB_OBM5_EXPRESS_ARRIVAL_BYTE_COUNTr}
        },
        {
         {IDB_OBM6_LOSSY_LO_BYTE_DROP_COUNTr,
          IDB_OBM6_LOSSY_HI_BYTE_DROP_COUNTr,
          IDB_OBM6_LOSSLESS0_BYTE_DROP_COUNTr,
          IDB_OBM6_LOSSLESS1_BYTE_DROP_COUNTr,
          IDB_OBM6_EXPRESS_BYTE_DROP_COUNTr},

         {IDB_OBM6_LOSSY_LO_ARRIVAL_BYTE_COUNTr,
          IDB_OBM6_LOSSY_HI_ARRIVAL_BYTE_COUNTr,
          IDB_OBM6_LOSSLESS0_ARRIVAL_BYTE_COUNTr,
          IDB_OBM6_LOSSLESS1_ARRIVAL_BYTE_COUNTr,
          IDB_OBM6_EXPRESS_ARRIVAL_BYTE_COUNTr}
        },
        {
         {IDB_OBM7_LOSSY_LO_BYTE_DROP_COUNTr,
          IDB_OBM7_LOSSY_HI_BYTE_DROP_COUNTr,
          IDB_OBM7_LOSSLESS0_BYTE_DROP_COUNTr,
          IDB_OBM7_LOSSLESS1_BYTE_DROP_COUNTr,
          IDB_OBM7_EXPRESS_BYTE_DROP_COUNTr},

         {IDB_OBM7_LOSSY_LO_ARRIVAL_BYTE_COUNTr,
          IDB_OBM7_LOSSY_HI_ARRIVAL_BYTE_COUNTr,
          IDB_OBM7_LOSSLESS0_ARRIVAL_BYTE_COUNTr,
          IDB_OBM7_LOSSLESS1_ARRIVAL_BYTE_COUNTr,
          IDB_OBM7_EXPRESS_ARRIVAL_BYTE_COUNTr}
        }
    };
    count = COUNTOF(obm_drop_byte_regs);
    if (count > extra_ctr_ct) {
        return SOC_E_PARAM;
    }

    /* Do sanity check for input params */
    if (!(non_dma_parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        return SOC_E_PARAM;
    }
    switch (non_dma_parent->id) {
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_EXPRESS:
            byte_flag = 0;
            counter_type = COUNTER_TYPE_DROP;
            break;
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS1:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_EXPRESS:
            byte_flag = 0;
            counter_type = COUNTER_TYPE_ENQUEUE;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_EXPRESS:
            byte_flag = 1;
            counter_type = COUNTER_TYPE_DROP;
            break;
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS1:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_EXPRESS:
            byte_flag = 1;
            counter_type = COUNTER_TYPE_ENQUEUE;
            break;
        default:
            return SOC_E_PARAM;
    }

    switch (non_dma_parent->id) {
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_LO:
            obm_type = LOSSY_LO;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_HI:
            obm_type = LOSSY_HI;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS0:
            obm_type = LOSSLESS0;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS1:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS1:
            obm_type = LOSSLESS1;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_EXPRESS:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_EXPRESS:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_EXPRESS:
        case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_EXPRESS:
            obm_type = EXPRESS;
            break;
    }

    parent_base_index = non_dma_parent->base_index;
    *total_entries = 0;

    for (i = 0; i < extra_ctr_ct; i++) {
        *non_dma_extra = *non_dma_parent;
        if (byte_flag == 0) {
            non_dma_extra->reg = obm_drop_pkt_regs[i][counter_type][obm_type];
        } else {
            non_dma_extra->reg = obm_drop_byte_regs[i][counter_type][obm_type];
        }
        non_dma_extra->extra_ctrs = NULL;
        non_dma_extra->extra_ctr_ct = 0;
        non_dma_extra->base_index = parent_base_index + *total_entries;
        non_dma_extra->flags = (non_dma_parent->flags &
                                ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                _SOC_COUNTER_NON_DMA_SUBSET_CHILD;

        non_dma_extra->cname = sal_alloc(sal_strlen(non_dma_parent->cname) + 9,
                                         "Extra ctrs cname");

        if (non_dma_extra->cname == NULL) {
            return SOC_E_MEMORY;
        }

        sal_sprintf(non_dma_extra->cname, "*OBM%d_%s_%s_%s",
                    i, obm_type_str[obm_type],
                    obm_ctr_type_str[counter_type],
                    byte_flag ? "BYTE":"PKT");

        non_dma_extra->num_entries = non_dma_extra->entries_per_port;
        *total_entries += non_dma_extra->num_entries;

        non_dma_extra += 2;
    }
    return SOC_E_NONE;
}
#endif /* BCM_MONTEREY_SUPPORT */

#if defined (BCM_APACHE_SUPPORT)
/* Function to initialize extra counters */
STATIC int
soc_counter_apache_extra_obm_ctrs_init(int unit,
                                   soc_counter_non_dma_t *non_dma_parent,
                                   soc_counter_non_dma_t *non_dma_extra,
                                   uint32 extra_ctr_ct,
                                   int *total_entries)
{
#define LOSSY_LO 0
#define LOSSY_HI 1
#define LOSSLESS0 2
#define LOSSLESS1 3
    char obm_type_str[][12] = {"LOSSY_LO", "LOSSY_HI",
                               "LOSSLESS0", "LOSSLESS1"}; /* For CNAME */
    int i, byte_flag = 0, obm_type = 0, parent_base_index = 0;
    uint32 count = 0;

    soc_reg_t obm_drop_pkt_regs[][4] = {
        {IDB_OBM0_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM0_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM0_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM0_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM1_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM1_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM1_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM1_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM2_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM2_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM2_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM2_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM3_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM3_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM3_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM3_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM4_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM4_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM4_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM4_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM5_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM5_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM5_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM5_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM6_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM6_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM6_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM6_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM7_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM7_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM7_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM7_LOSSLESS1_PKT_DROP_COUNTr
        },
        {IDB_OBM8_LOSSY_LO_PKT_DROP_COUNTr, IDB_OBM8_LOSSY_HI_PKT_DROP_COUNTr,
         IDB_OBM8_LOSSLESS0_PKT_DROP_COUNTr, IDB_OBM8_LOSSLESS1_PKT_DROP_COUNTr
        },
    };
    soc_reg_t obm_drop_byte_regs[][4] = {
        {IDB_OBM0_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM0_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM0_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM0_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM1_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM1_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM1_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM1_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM2_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM2_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM2_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM2_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM3_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM3_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM3_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM3_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM4_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM4_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM4_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM4_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM5_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM5_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM5_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM5_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM6_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM6_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM6_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM6_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM7_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM7_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM7_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM7_LOSSLESS1_BYTE_DROP_COUNTr
        },
        {IDB_OBM8_LOSSY_LO_BYTE_DROP_COUNTr, IDB_OBM8_LOSSY_HI_BYTE_DROP_COUNTr,
         IDB_OBM8_LOSSLESS0_BYTE_DROP_COUNTr, IDB_OBM8_LOSSLESS1_BYTE_DROP_COUNTr
        },
    };

    count = COUNTOF(obm_drop_byte_regs);
    if (count > extra_ctr_ct) {
        return SOC_E_PARAM;
    }

    /* Do sanity check for input params */
    if (!(non_dma_parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* Only for Extra counters */
        return SOC_E_PARAM;
    }
    switch (non_dma_parent->id) {
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
            byte_flag = 0;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
            byte_flag = 1;
            break;
        default:
            return SOC_E_PARAM;
    }

    switch (non_dma_parent->id) {
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
            obm_type = LOSSY_LO;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
            obm_type = LOSSY_HI;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
            obm_type = LOSSLESS0;
            break;
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
            obm_type = LOSSLESS1;
            break;
    }

    parent_base_index = non_dma_parent->base_index;
    *total_entries = 0;

    for (i = 0; i < extra_ctr_ct; i++) {
        *non_dma_extra = *non_dma_parent;
        if (byte_flag == 0) {
            non_dma_extra->reg = obm_drop_pkt_regs[i][obm_type];
        } else {
            non_dma_extra->reg = obm_drop_byte_regs[i][obm_type];
        }
        non_dma_extra->extra_ctrs = NULL;
        non_dma_extra->extra_ctr_ct = 0;
        non_dma_extra->base_index = parent_base_index + *total_entries;
        non_dma_extra->flags = (non_dma_parent->flags &
                                ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                _SOC_COUNTER_NON_DMA_SUBSET_CHILD;

        non_dma_extra->cname = sal_alloc(sal_strlen(non_dma_parent->cname) + 9,
                                         "Extra ctrs cname");

        if (non_dma_extra->cname == NULL) {
            return SOC_E_MEMORY;
        }

        sal_sprintf(non_dma_extra->cname, "*OBM%d_%s_DRP_%s",
                    i, obm_type_str[obm_type], byte_flag ? "BYTE":"PKT");

        non_dma_extra->num_entries = non_dma_extra->entries_per_port;
        *total_entries += non_dma_extra->num_entries;

        /* Allocation of Child Control blocks.
         * [0],..<even indices> - PKT_CTR non_dma ctrl block
         * [1],..<odd indices> - BYTE_CTR non_dma ctrl block
         * Hence in below code, jump by 2, so all similar control blocks are
         * initilized at the same time.
         */
        non_dma_extra += 2;
        /* coverity[check_after_deref] */
        if (!non_dma_extra) {
            /* ERR: Cannot be NULL, until 'i reaches 'extra_ctr_ct' */
            /* This condition may not hit but intentional for defensive check */
            /* coverity[dead_error_line] */
            return SOC_E_INTERNAL;
        }
    }
    return SOC_E_NONE;
}

STATIC int
soc_counter_common_extra_obm_ctrs_init(int unit,
                                       soc_counter_non_dma_t *non_dma_parent,
                                       soc_counter_non_dma_t *non_dma_extra,
                                       uint32 extra_ctr_ct,
                                       int *total_entries)
{
#if defined(BCM_MONTEREY_SUPPORT)
     if (SOC_IS_MONTEREY(unit)) {
         return soc_counter_monterey_extra_obm_ctrs_init(unit,
                                                         non_dma_parent,
                                                         non_dma_extra,
                                                         extra_ctr_ct,
                                                         total_entries);
     }
#endif
     return soc_counter_apache_extra_obm_ctrs_init(unit,
                                                     non_dma_parent,
                                                     non_dma_extra,
                                                     extra_ctr_ct,
                                                     total_entries);
}

#if defined(BCM_MONTEREY_SUPPORT) && defined (CPRIMOD_SUPPORT)
STATIC int
_soc_counter_monterey_non_dma_cpri_init(int unit, int nports,
                                        int non_dma_start_index,
                                        int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1, *non_dma2;
    *non_dma_entries = 0;

    soc = SOC_CONTROL(unit);

    /*
     * Adding CPRI counter information to the non dma info
     * Although CPRI counters are DMA'able, it is inefficient
     * because for DMA'ing the counters should be seperated by
     * the same offset, else descriptors need to be created
     * for each set of counters. There are only a few CPRI
     * so not using dma.
     * All the CPRI counters are
     * 1. Read Only
     * 2. Clear on Read
     * 3. Saturate on Overflow
     */


    /*
     *  symbol error counter that increments upon detecting symbol
     *  error (due to invalid symbol or disparity error)
     */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_CPRI_RX_SYMBOL_ERR -
                                     SOC_COUNTER_NON_DMA_START];

    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_READ_ONLY |
                      _SOC_COUNTER_NON_DMA_CLEAR_ON_READ |
                      _SOC_COUNTER_NON_DMA_SATURATE_ON_OVERFLOW|
                      _SOC_COUNTER_NON_DMA_CPRI_ONLY;

    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->pbmp = PBMP_ROE_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = CPRI_RXPCS_0_CNTR_STATUSr;
    non_dma0->field = RXPCS_BER_COUNTf;
    non_dma0->cname = "CPRI_RX_SYMBOL_ERR";
    *non_dma_entries += non_dma0->num_entries;

    /*
     * Counts the number of seed vector mismatches. It increments
     * if the seed vector of the current hyperframe does not match
     * the sts_rxpcs_seed_vector value.
     */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_CPRI_RX_SEED_VECTOR_MISMATCH -
                              SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RXPCS_1_CNTR_STATUSr;
    non_dma1->field = RXPCS_SEED_MISMATCH_CNTf;
    non_dma1->cname = "CPRI_RX_SEED_MISMATCH";
    *non_dma_entries += non_dma1->num_entries;


    /* Count number of state transitions into the error state */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_CPRI_RX_STATE_TRANSITION_ERR -
                              SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RXPCS_2_CNTR_STATUSr;
    non_dma1->field = RXPCS_64B66B_ERR_HF_CNTf;
    non_dma1->cname = "CPRI_RX_STATE_TRANSITION_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /*
     *  VSD Control Packet
     *  Packet count with Flow-Id out of range(0-127)
     */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_TX_VSD_CTRL_PKT_FLOW_ID_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_TXFRM_CWA_VSD_CTRL_PACKET_SIZE_ERR_STATUSr;
    non_dma1->field = TX_CWA_VSD_CTRL_PACKET_FLOW_ID_ERR_CNTf;
    non_dma1->cname = "CPRI_TX_VSD_CTRL_FLOW_ID_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /*
     *  VSD Control Packet
     *  Counts the number of packets with packet size error.
     *  Packet is longer than 16 bytes (1 decap data word).
     */
    non_dma2 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_TX_VSD_CTRL_PKT_SIZE_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma2 = *non_dma1;
    non_dma2->base_index = non_dma_start_index + *non_dma_entries;
    non_dma2->field = TX_CWA_VSD_CTRL_PACKET_SIZE_ERR_CNTf;
    non_dma2->cname = "CPRI_TX_VSD_CTRL_PKT_SIZE_ERR";
    *non_dma_entries += non_dma2->num_entries;

    /*
     * VSD Raw Packet
     * Counts the number of packets with packet size error.
     * Either packet is too short and ended before HF end, or
     * packet is too long and extends past HF end.
     */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_TX_VSD_RAW_PKT_SIZE_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_TXFRM_CWA_VSD_RAW_PACKET_SIZE_ERR_STATUSr;
    non_dma1->field = TX_CWA_VSD_RAW_PACKET_SIZE_ERR_CNTf;
    non_dma1->cname = "CPRI_TX_VSD_RAW_PKT_SIZE_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /*
     * Counts the number of packets dropped by RSVD5 due to CRC error.
     */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_RSVD5_TX_DROP_CRC_ERR -
                                     SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_1r;
    non_dma1->field = BRCM_RESERVED_CPM_1_1f;
    non_dma1->cname = "CPRI_TX_RSVD5_DROP_CRC_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /*
     * Counts the number of single message control streams
     * dropped due to bad CRC.
     */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_RSVD4_RX_SINGLE_MSG_DROP_CRC_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_323r;
    non_dma1->field = BRCM_RESERVED_CPM_323_0f;
    non_dma1->cname = "RSVD4_RX_SINGLE_MSG_DROP";
    *non_dma_entries += non_dma1->num_entries;

    /*
     * RSVD4 control message processing status counter.
     * Counts the number of cases when the tag was not found during lookup.
     */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_RSVD4_RX_TAG_LKUP_FAIL -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_324r;
    non_dma1->field = BRCM_RESERVED_CPM_324_0f;
    non_dma1->cname = "RSVD4_RX_TAG_LKUP_FAIL";
    *non_dma_entries += non_dma1->num_entries;

    /* single message control streams including the error messages */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_RSVD4_RX_SINGLE_MSG_CTRL_STREAMS -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_325r;
    non_dma1->field = BRCM_RESERVED_CPM_325_0f;
    non_dma1->cname = "RSVD4_RX_SINGLE_MSG_CTRL_STREAMS";
    *non_dma_entries += non_dma1->num_entries;

    /*
     * Counts the number of multiple message control
     * streams including the error messages.
     */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_RSVD4_RX_MULPILE_MSG_CTRL_STREAMS -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_327r;
    non_dma1->field = BRCM_RESERVED_CPM_327_0f;
    non_dma1->cname = "RSVD4_RX_MULTIPLE_MSG_CTRL_STREAMS";
    *non_dma_entries += non_dma1->num_entries;

    /*
     * Counts the number of incorrect timestamp sequences.
     * Increments once per incorrect timestamp sequence.
     */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_RSVD4_RX_INCORRECT_TIME_STAMP_SEQ -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_326r;
    non_dma1->field = BRCM_RESERVED_CPM_326_0f;
    non_dma1->cname = "RSVD4_RX_INCORRECT_TS_SEQ";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the number of data bytes in error. */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_DATA_BYTES_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RX_STATS_CNTR_0_STATUSr;
    non_dma1->field = RX_ERR_BYTE_CNTRf;
    non_dma1->cname = "CPRI_RX_DATA_BYTE_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the number of messages dropped due to header errors. */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_RSVD4_HDR_MSG_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RX_STATS_CNTR_1_STATUSr;
    non_dma1->field = BRCM_RESERVED_CPM_328_0f;
    non_dma1->cname = "RSVD4_RX_HDR_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /*
     * Counts the number of RSVD4 messages whose headers did not
     * match any of the options in the look up table.
     */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_RSVD4_MSG_HDR_LKUP_FAIL -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RX_STATS_CNTR_2_STATUSr;
    non_dma1->field = BRCM_RESERVED_CPM_329_0f;
    non_dma1->cname = "RSVD4_RX_HDR_LKUP_FAIL";
    *non_dma_entries += non_dma1->num_entries;

    /*
     * Counts the total number of RSVD4 messages including
     * the error and dropped messages.
     */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_RSVD4_TOTAL_MSG_COUNT -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RX_STATS_CNTR_3_STATUSr;
    non_dma1->field = BRCM_RESERVED_CPM_330_0f;
    non_dma1->cname = "RSVD4_RX_TOTAL_MSG_COUNT";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the number of incorect HFN extracted */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_INCORRECT_HFN_EXTRACT -
                    SOC_COUNTER_NON_DMA_START];

    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RX_STATS_CNTR_4_STATUSr;
    non_dma1->field = RXFRM_HFN_ERR_CNTRf;
    non_dma1->cname = "CPRI_RX_INCORRECT_HFN";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the number of incorect BFN extracted */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_INCORRECT_BFN_EXTRACT -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RX_STATS_CNTR_5_STATUSr;
    non_dma1->field = RXFRM_BFN_ERR_CNTRf;
    non_dma1->cname = "CPRI_RX_INCORRECT_BFN";
    *non_dma_entries += non_dma1->num_entries;

    /* Count of bytes in control words with error indication */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_CTRL_WORD_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RX_STATS_CNTR_6_STATUSr;
    non_dma1->field = RX_CW_ERR_CNTf;
    non_dma1->cname = "CPRI_RX_CW_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /* count of Generic Control Word words dropped due to byte errors */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_GENERIC_CTRL_WORD_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_RX_GCW_ERR_CNTR_STATUSr;
    non_dma1->field = RX_GCW_ERR_CNTf;
    non_dma1->cname = "CPRI_RX_GCW_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /* VSD raw packets dropped due to byte errors */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_VSD_RAW_PKT_DROP -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_VSDRAW_ERR_CNTR_STATUSr;
    non_dma1->field = RX_VSD_RAW_ERR_CNTf;
    non_dma1->cname = "CPRI_RX_VSD_RAW_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /* Count of VSD control packets dropped due to byte error */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_VSD_CTRL_PKT_DROP -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_VSD_CTRL_ERR_CNTR_STATUSr;
    non_dma1->field = RX_VSD_CTRL_ERR_CNTf;
    non_dma1->cname = "CPRI_RX_VSD_CTRL_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the number of RSVD4 data slots used as data messages. */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_TX_RSVD4_DATA_SLOT_DATA_MSG -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_6r;
    non_dma1->field = BRCM_RESERVED_CPM_6_0f;
    non_dma1->cname = "RSVD4_DATA_SLOT_DATA_MSG_CNT";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the number of RSVD4 data slots used as empty messages */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_TX_RSVD4_DATA_SLOT_EMPTY_MSG -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_7r;
    non_dma1->field = BRCM_RESERVED_CPM_7_0f;
    non_dma1->cname = "RSVD4_DATA_SLOT_EMPTY_MSG_CNT";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the number of RSVD4 control slots used as control messages */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_TX_RSVD4_CTRL_SLOT_CTRL_MSG -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_8r;
    non_dma1->field = BRCM_RESERVED_CPM_8_0f;
    non_dma1->cname = "RSVD4_CTRL_SLOT_CTRL_MSG_CNT";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the number of RSVD4 control slots used as empty messages */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_TX_RSVD4_CTRL_SLOT_EMPTY_MSG -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_9r;
    non_dma1->field = BRCM_RESERVED_CPM_9_0f;
    non_dma1->cname = "RSVD4_CTRL_SLOT_EMPTY_MSG_CNT";
    *non_dma_entries += non_dma1->num_entries;

    /* Count of RSVD5 messages with incorrect parity. */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_RSVD5_RX_MSG_PARITY_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = BRCM_RESERVED_CPM_316r;
    non_dma1->field = BRCM_RESERVED_CPM_316_0f;
    non_dma1->cname = "RSVD5_MSG_PARITY_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /*
     * HDLC error frame counter. Increments if the HDLC frame
     * contains errors, abort sequence or FCS errors. Counts runt
     * frames too that are dropped.
     */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_HDLC_FRAME_ERR -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_HDLC_ERR_CNTR_STATUSr;
    non_dma1->field = RX_HDLC_ERROR_FRAME_CNTf;
    non_dma1->cname = "CPRI_RX_HDLC_FRM_ERR";
    *non_dma_entries += non_dma1->num_entries;

    /* HDLC good frame counter. */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_HDLC_FRAME_OK -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_HDLC_CNTR_STATUSr;
    non_dma1->field = RX_HDLC_GOOD_FRAME_CNTf;
    non_dma1->cname = "CPRI_RX_HDLC_FRM_CNT";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the error packets dropped in the MAC */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_FAST_ETH_PKT_MAC_DROP -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_FAST_ETH_ERR_CNTR_STATUSr;
    non_dma1->field = RX_FAST_ETH_MAC_ERR_PKT_CNTf;
    non_dma1->cname = "CPRI_RX_FAST_ETH_PKT_DROP_CNT";
    *non_dma_entries += non_dma1->num_entries;

    /* Counts the number of valid packets forwarded to the encap block */
    non_dma1 = &soc->counter_non_dma[
                    SOC_COUNTER_NON_DMA_CPRI_RX_FAST_ETH_PKT_MAC_OK -
                    SOC_COUNTER_NON_DMA_START];

    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CPRI_FAST_ETH_CNTR_STATUSr;
    non_dma1->field = RX_FAST_ETH_MAC_PKT_CNTf;
    non_dma1->cname = "CPRI_RX_FAST_ETH_PKT_CNT";
    *non_dma_entries += non_dma1->num_entries;

    return SOC_E_NONE;
}
#endif /* BCM_MONTEREY_SUPPORT  && CPRIMOD_SUPPORT */

STATIC int
_soc_counter_apache_non_dma_init(int unit, int nports,
                                   int non_dma_start_index,
                                   int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1, *non_dma2, *temp;
    int  alloc_size;
    uint32 *buf;
    uint32 dma_num_entries=0,dma_num_index=0, dma_index_max = 0;
    int full_chip = 1, rv, num_extra_ctr_entries;
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    /* MMU_CTR_UC_DROP_MEM is the largest table to be DMA'ed */
    if (SOC_IS_MONTEREY(unit)) {
        alloc_size = 2 * soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm) *
                      sizeof(uint32) *  soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm);
        dma_num_entries=67;
        dma_num_index=66;
        dma_index_max = 639;
    } else {
    alloc_size = 2 * soc_mem_index_count(unit, MMU_CTR_UC_DROP_MEMm) *
                  sizeof(uint32) *  soc_mem_entry_words(unit, MMU_CTR_UC_DROP_MEMm);
        dma_num_entries=74;
        dma_num_index=74;
        dma_index_max = 16383;
    }

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* cpu port has max number of queues */
    non_dma0->num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm) / ((full_chip)? 1 : 2);
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "MC_PERQ_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm) - 1;
    non_dma0->dma_mem[0] = EGR_PERQ_XMT_COUNTERSm;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "MC_PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 10;
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_PKT";

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 10;
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_BYTE";

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;

    non_dma2->entries_per_port = 0;
    non_dma2->num_entries = 0;
    SOC_PBMP_CLEAR(non_dma2->pbmp);

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->entries_per_port = 0;
    non_dma2->num_entries = 0;
    SOC_PBMP_CLEAR(non_dma2->pbmp);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* APACHE 48 for cpu port */
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_MC_DROP_MEM0m);
    non_dma0->mem = MMU_CTR_MC_DROP_MEM0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "MCQ_DROP_PKT";
    non_dma0->dma_mem[0] = MMU_CTR_MC_DROP_MEM0m;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "MCQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 10;
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_UC_DROP_MEMm);
    non_dma0->mem = MMU_CTR_UC_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "UCQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = dma_index_max;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "UCQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_ING_DROP_MEMm);
    non_dma0->mem = MMU_CTR_ING_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "DROP_PKT_ING";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->mem);
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "DROP_BYTE_ING";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING_METER -
                              SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_ING_DROP_MEMm);
    non_dma0->mem = MMU_CTR_MTRI_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "DROP_PKT_IMTR";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->mem);
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING_METER -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "DROP_BYTE_IMTR";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    /* TO DO: APACHE For this ver of regs file num of entries changed */
    non_dma0->num_entries = dma_num_entries; /* one per port */
    non_dma0->mem = MMU_CTR_COLOR_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "DROP_PKT_YEL";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = dma_num_index * 4;
    non_dma0->dma_index_max[0] = (dma_num_index * 4) + (dma_num_index -1);
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "DROP_PKT_RED";
    /* TO DO: APACHE For this ver of regs file num of entries changed */
    non_dma1->dma_index_min[0] = dma_num_index * 3;
    non_dma1->dma_index_max[0] = (dma_num_index * 3) + (dma_num_index - 1);
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_GRE";
    /* TO DO: APACHE For this ver of regs file num of entries changed */
    non_dma1->dma_index_min[0] = dma_num_index * 2;
    non_dma1->dma_index_max[0] = (dma_num_index * 2) + (dma_num_index - 1);
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_YEL";
    /* TO DO: APACHE For this ver of regs file num of entries changed */
    non_dma1->dma_index_min[0] = dma_num_index * 1;
    non_dma1->dma_index_max[0] = (dma_num_index * 1) + (dma_num_index - 1);
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_RED";
    non_dma1->dma_index_min[0] = 0;
    non_dma1->dma_index_max[0] = 65;
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 11;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = DROP_PKT_CNT_RQE_PKTr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "RQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->pbmp = PBMP_ALL(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_RQE_BYTE_64r;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "RQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_RQE_YELr;
    non_dma1->field = PKTCNTf;
    non_dma1->cname = "RQ_DROP_PKT_YEL";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_RQE_REDr;
    non_dma1->field = PKTCNTf;
    non_dma1->cname = "RQ_DROP_PKT_RED";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 4;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = THDI_POOL_SHARED_COUNT_SPr;
    non_dma0->field = TOTAL_BUFFER_COUNTf;
    non_dma0->cname = "POOL_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "POOL_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->num_entries = nports * non_dma0->entries_per_port;
    non_dma0->mem = THDI_PORT_PG_CNTRS_RT1_Xm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG_MIN_COUNTf;
    non_dma0->cname = "PG_MIN_PEAK";
    non_dma0->dma_mem[0] = THDI_PORT_PG_CNTRS_RT1_Xm;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
            _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_MIN_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PG_SHARED_COUNTf;
    non_dma1->cname = "PG_SHARED_PEAK";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_SHARED_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->num_entries = nports * non_dma0->entries_per_port;
    non_dma0->mem = THDI_PORT_PG_CNTRS_RT2_Xm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG_HDRM_COUNTf;
    non_dma0->cname = "PG_HDRM_PEAK";
    non_dma0->dma_mem[0] = THDI_PORT_PG_CNTRS_RT2_Xm;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_HDRM_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_THDM_DB_QUEUE_COUNT_0m);
    non_dma0->mem = MMU_THDM_DB_QUEUE_COUNT_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = SHARED_COUNTf;
    non_dma0->cname = "QUEUE_PEAK";
    non_dma0->dma_mem[0] = MMU_THDM_DB_QUEUE_COUNT_0m;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "QUEUE_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 10;
    non_dma0->num_entries =
        soc_mem_index_count(unit, MMU_THDU_XPIPE_COUNTER_QUEUEm);
    non_dma0->mem = MMU_THDU_XPIPE_COUNTER_QUEUEm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = SHARED_COUNTf;
    non_dma0->cname = "UC_QUEUE_PEAK";
    non_dma0->dma_mem[0] = MMU_THDU_XPIPE_COUNTER_QUEUEm;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]) - 1;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "UC_QUEUE_CUR";
    if (!SOC_IS_MONTEREY(unit)) {
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_MMU_QCN_CNM -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_FLEX_MAPPING;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 10;
    non_dma0->num_entries = 1 * soc_mem_index_count(unit, MMU_QCN_CNM_COUNTERm);

    non_dma0->mem = MMU_QCN_CNM_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = CNM_CNTf;
    non_dma0->cname = "QCN_CNM_COUNTER";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_mem[0] = non_dma0->mem;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    non_dma0->soc_counter_hw_idx_get = soc_apache_qcn_counter_hw_index_get;
    *non_dma_entries += non_dma0->num_entries;
    }
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 1;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = THDI_GLOBAL_HDRM_COUNT_PIPEXr;
    non_dma0->field = GLOBAL_HDRM_COUNTf;

    non_dma0->cname = "GLOBAL_HDRM_COUNT_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_CURRENT -
        SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "GLOBAL_HDRM_COUNT_CURR";

    non_dma0 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_E2E_DROP_COUNT -
                              SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->reg = E2E_DROP_COUNTr;
    non_dma0->mem = INVALIDm;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 1;
    non_dma0->field = COUNTf;
    non_dma0->cname = "E2E_DROP_COUNT";
    *non_dma_entries += non_dma1->num_entries;



    soc_ctr_ctrl->port_obm_info_get = soc_apache_port_obm_info_get;
    /* Control block for IDB OBM counters */
    /* Lossy LO drop counters - Pkt Counters */
    non_dma0 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO -
                              SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO;
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->reg = IDB_OBM0_LOSSY_LO_PKT_DROP_COUNTr;
    non_dma0->cname = "OBM_LOSSY_LO_DRP_PKT";

    /* Common across all OBM structures */
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_OBM;
    /* Use Parent-Child model to cover all 8 OBMs */
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->entries_per_port = 8;
    non_dma0->num_entries = 1;
    non_dma0->mem = INVALIDm;
    non_dma0->field = COUNTERf;
    non_dma0->dma_buf[0] = buf;
    non_dma0->pbmp = PBMP_ALL(unit);
    /* Remove CPU port as OBM counters not applicable to CPU port*/
    SOC_PBMP_PORT_REMOVE(non_dma0->pbmp, 0);

    /*
     * Below represents how Extra cntrs are arranged
     * for OBM 0-8 Drop Counters.
     * n = NUM_OBMs (9)
     *
     *   [0]     [2]        [2n-2]
     *  ------  ------     ------
     * |      ||      |...|      |
     * | PKT  || PKT  |...| PKT  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------ ... ------
     * |      ||      |...|      |
     * | BYTE || BYTE |...| BYTE |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------     ------
     *   [1]     [3]        [2n-1]
     */
    non_dma0->extra_ctr_ct = SOC_IS_MONTEREY(unit) ? _MN_PBLKS_PER_PIPE :_AP_PBLKS_PER_PIPE; /* 9 OBMs per Pipe */
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    temp = non_dma0->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossy_lo");
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma0,
                                                non_dma0->extra_ctrs,
                                                non_dma0->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                        non_dma0->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* Lossy LO drop counters - Byte Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSY_LO_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSY_LO_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossy HI drop counters - Packet Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSY_HI_PKT_DROP_COUNTr;
    non_dma1->cname = "OBM_LOSSY_HI_DRP_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossy_hi");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossy HI drop counters - Byte Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSY_HI_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSY_HI_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossless0 drop counters - Packet Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS0_PKT_DROP_COUNTr;
    non_dma1->cname = "OBM_LOSSLESS0_DRP_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossless0");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossless0 drop counters - Byte Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS0_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSLESS0_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossless1 drop counters - Packet counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS1_PKT_DROP_COUNTr;
    non_dma1->cname = "OBM_LOSSLESS1_DRP_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                            "non_dma_ctrs_obm_lossless1");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Lossless1 drop counters - Byte Counters */
    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = IDB_OBM0_LOSSLESS1_BYTE_DROP_COUNTr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM_LOSSLESS1_DRP_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
        rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Following counters are Monterey specific */
    if (SOC_IS_MONTEREY(unit)) {
        /* Express drop counters - Packet counters */
        non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_EXPRESS -
                                SOC_COUNTER_NON_DMA_START];
        *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_EXPRESS;
        non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_EXPRESS_PKT_DROP_COUNTr;
        non_dma1->cname = "OBM_EXPRESS_DRP_PKT";

        temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                                "non_dma_ctrs_obm_express_drop");

        if (non_dma1->extra_ctrs == NULL) {
            non_dma1->extra_ctr_ct = 0;
            non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
            return SOC_E_MEMORY;
        } else {
            sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                  non_dma1,
                                                  non_dma1->extra_ctrs,
                                                  non_dma1->extra_ctr_ct,
                                                  &num_extra_ctr_entries);
            if (rv != SOC_E_NONE) {
                LOG_CLI((BSL_META_U(unit,
                                    "ERR: Extra ctrs Init FAILED for id %d\n"),
                         non_dma1->id - SOC_COUNTER_NON_DMA_START));
            }
            non_dma1->num_entries = num_extra_ctr_entries;
        }
        *non_dma_entries += non_dma1->num_entries;

        /* Express drop counters - Byte Counters */
        non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_EXPRESS -
                                SOC_COUNTER_NON_DMA_START];
        *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_EXPRESS;
        non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_EXPRESS_BYTE_DROP_COUNTr;
        non_dma1->field = COUNTERf;
        non_dma1->cname = "OBM_EXPRESS_DRP_BYTE";

        /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
        non_dma1->extra_ctrs = temp + 1;
        if (non_dma1->extra_ctrs) {
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                        non_dma1,
                                                        non_dma1->extra_ctrs,
                                                        non_dma1->extra_ctr_ct,
                                                        &num_extra_ctr_entries);
            if (rv != SOC_E_NONE) {
                LOG_CLI((BSL_META_U(unit,
                                    "ERR: Extra ctrs Init FAILED for id %d\n"),
                                    non_dma1->id - SOC_COUNTER_NON_DMA_START));
            }
        }
        *non_dma_entries += non_dma1->num_entries;

        /* Lossy_lo enqueue counters - Packet counters */
        non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_LO -
                              SOC_COUNTER_NON_DMA_START];
        *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_LO;
        non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_LOSSY_LO_ARRIVAL_PKT_COUNTr;
        non_dma1->cname = "OBM_LOSSY_LO_ENQ_PKT";

        temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                                "non_dma_ctrs_obm_lossy_lo_drop");

        if (non_dma1->extra_ctrs == NULL) {
            non_dma1->extra_ctr_ct = 0;
            non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
            return SOC_E_MEMORY;
        } else {
            sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                  non_dma1,
                                                  non_dma1->extra_ctrs,
                                                  non_dma1->extra_ctr_ct,
                                                  &num_extra_ctr_entries);
            if (rv != SOC_E_NONE) {
                LOG_CLI((BSL_META_U(unit,
                                    "ERR: Extra ctrs Init FAILED for id %d\n"),
                         non_dma1->id - SOC_COUNTER_NON_DMA_START));
            }
            non_dma1->num_entries = num_extra_ctr_entries;
        }
        *non_dma_entries += non_dma1->num_entries;

        /* Lossy_lo enqueue counters - Byte Counters */
        non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_LO -
                              SOC_COUNTER_NON_DMA_START];
        *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_LO;
        non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_LOSSY_LO_ARRIVAL_BYTE_COUNTr;
        non_dma1->field = COUNTERf;
        non_dma1->cname = "OBM_LOSSY_LO_ENQ_BYTE";

        /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
        non_dma1->extra_ctrs = temp + 1;
        if (non_dma1->extra_ctrs) {
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                        non_dma1,
                                                        non_dma1->extra_ctrs,
                                                        non_dma1->extra_ctr_ct,
                                                        &num_extra_ctr_entries);
            if (rv != SOC_E_NONE) {
                LOG_CLI((BSL_META_U(unit,
                                    "ERR: Extra ctrs Init FAILED for id %d\n"),
                                    non_dma1->id - SOC_COUNTER_NON_DMA_START));
            }
        }
        *non_dma_entries += non_dma1->num_entries;

        /* Lossy_hi enqueue counters - Packet counters */
        non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_HI -
                                 SOC_COUNTER_NON_DMA_START];
        *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_HI;
        non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_LOSSY_HI_ARRIVAL_PKT_COUNTr;
        non_dma1->cname = "OBM_LOSSY_HI_ENQ_PKT";

        temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                                "non_dma_ctrs_obm_lossy_hi_drop");

        if (non_dma1->extra_ctrs == NULL) {
            non_dma1->extra_ctr_ct = 0;
            non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
            sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                  non_dma1,
                                                  non_dma1->extra_ctrs,
                                                  non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                         non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
            non_dma1->num_entries = num_extra_ctr_entries;
    }
        *non_dma_entries += non_dma1->num_entries;

        /* Lossy_hi enqueue counters - Byte Counters */
    non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_HI -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_HI;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_LOSSY_HI_ARRIVAL_BYTE_COUNTr;
    non_dma1->field = COUNTERf;
        non_dma1->cname = "OBM_LOSSY_HI_ENQ_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

        /* Lossless0 enqueue counters - Packet counters */
    non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS0 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_LOSSLESS0_ARRIVAL_PKT_COUNTr;
        non_dma1->cname = "OBM_LOSSLESS0_ENQ_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                                "non_dma_ctrs_obm_lossless0_enqueue");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

        /* Lossless0 enqueue counters - Byte Counters */
    non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS0 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_LOSSLESS0_ARRIVAL_BYTE_COUNTr;
    non_dma1->field = COUNTERf;
        non_dma1->cname = "OBM_LOSSLESS0_ENQ_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

        /* Lossless1 enqueue counters - Packet counters */
    non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS1 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS1;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_LOSSLESS1_ARRIVAL_PKT_COUNTr;
        non_dma1->cname = "OBM_LOSSLESS1_ENQ_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                                "non_dma_ctrs_obm_lossless1_enqueue");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

        /* Lossless1 enqueue counters - Byte Counters */
    non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS1 -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS1;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_LOSSLESS1_ARRIVAL_BYTE_COUNTr;
    non_dma1->field = COUNTERf;
        non_dma1->cname = "OBM_LOSSLESS1_ENQ_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;

        /* Express enqueue counters - Packet counters */
    non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_EXPRESS -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_EXPRESS;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_EXPRESS_ARRIVAL_PKT_COUNTr;
        non_dma1->cname = "OBM_EXPRESS_ENQ_PKT";

    temp = non_dma1->extra_ctrs = sal_alloc(alloc_size,
                                                "non_dma_ctrs_obm_express_enqueue");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

        /* Express enqueue counters - Byte Counters */
    non_dma1 =
           &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_EXPRESS -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
        non_dma1->id = SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_EXPRESS;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->reg = IDB_OBM0_EXPRESS_ARRIVAL_BYTE_COUNTr;
    non_dma1->field = COUNTERf;
        non_dma1->cname = "OBM_EXPRESS_ENQ_BYTE";

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = temp + 1;
    if (non_dma1->extra_ctrs) {
            rv = soc_counter_common_extra_obm_ctrs_init(unit,
                                                non_dma1,
                                                non_dma1->extra_ctrs,
                                                non_dma1->extra_ctr_ct,
                                                &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     non_dma1->id - SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;
    }
    /* End of Control block for IDB OBM counters */

    return SOC_E_NONE;
}
#endif /* BCM_APACHE_SUPPORT */



#if defined ( BCM_TRIDENT2_SUPPORT ) || defined ( BCM_TRIDENT2PLUS_SUPPORT )
STATIC int
_soc_counter_trident2_non_dma_init(int unit, int nports,
                                   int non_dma_start_index,
                                   int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1, *non_dma2;
    int  alloc_size;
    uint32 *buf;
    int full_chip = 1;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SAL_BOOT_QUICKTURN) {
        if ( !soc_td2p_if_full_chip(unit)) {
            /* testing with half chip */
            full_chip = 0;
        }
    }
#endif

    /* MMU_CTR_UC_DROP_MEM is the largest table to be DMA'ed */

    alloc_size = 2 * soc_mem_index_count(unit, MMU_CTR_UC_DROP_MEMm) *
                  sizeof(uint32) *  soc_mem_entry_words(unit, MMU_CTR_UC_DROP_MEMm);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* cpu port has max number of queues */
    non_dma0->num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERS_Xm) * ((full_chip)? 2 : 1);
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "MC_PERQ_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_buf[1] = (full_chip)?
              &buf[soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERS_Xm) *
                   soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERS_Xm)] : NULL;
    non_dma0->dma_index_max[0] = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERS_Xm) - 1;
    non_dma0->dma_index_max[1] = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERS_Xm) - 1;
    non_dma0->dma_mem[0] = EGR_PERQ_XMT_COUNTERS_Xm;
    non_dma0->dma_mem[1] = EGR_PERQ_XMT_COUNTERS_Ym;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "MC_PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 16; /* front pannel port has max 16 L2 UC queues */
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_PKT";

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 16; /* front pannel port has max 16 L2 UC queues */
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_BYTE";

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->entries_per_port = 0;
    non_dma2->num_entries = 0;
    SOC_PBMP_CLEAR(non_dma2->pbmp);

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->entries_per_port = 0;
    non_dma2->num_entries = 0;
    SOC_PBMP_CLEAR(non_dma2->pbmp);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_MC_DROP_MEM0m) +
        soc_mem_index_count(unit, MMU_CTR_MC_DROP_MEM1m);
    non_dma0->mem = MMU_CTR_MC_DROP_MEM0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "MCQ_DROP_PKT";
    non_dma0->dma_mem[0] = MMU_CTR_MC_DROP_MEM0m;
    non_dma0->dma_mem[1] = MMU_CTR_MC_DROP_MEM1m;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_buf[1] =
        &buf[soc_mem_index_count(unit, non_dma0->dma_mem[0]) *
             soc_mem_entry_words(unit, non_dma0->dma_mem[0])];
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    non_dma0->dma_index_max[1] = soc_mem_index_max(unit, non_dma0->dma_mem[1]);
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "MCQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 16; /* front pannel port has max 16 L2 UC queues */
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_UC_DROP_MEMm);
    non_dma0->mem = MMU_CTR_UC_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "UCQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_buf[1] =
        &buf[1480 * soc_mem_entry_words(unit, non_dma0->mem)];
    non_dma0->dma_index_max[0] = 1479;
    non_dma0->dma_index_min[1] = 1536;
    non_dma0->dma_index_max[1] = 3015;
    non_dma0->dma_mem[0] = non_dma0->mem;
    non_dma0->dma_mem[1] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "UCQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_EXT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma2->entries_per_port = 0;
    non_dma2->num_entries = 0;
    SOC_PBMP_CLEAR(non_dma2->pbmp);

    non_dma2 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_EXT -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->entries_per_port = 0;
    non_dma2->num_entries = 0;
    SOC_PBMP_CLEAR(non_dma2->pbmp);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_ING_DROP_MEMm);
    non_dma0->mem = MMU_CTR_ING_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "DROP_PKT_ING";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->mem);
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "DROP_BYTE_ING";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING_METER -
                              SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_ING_DROP_MEMm);
    non_dma0->mem = MMU_CTR_MTRI_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "DROP_PKT_IMTR";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->mem);
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING_METER -
                              SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "DROP_BYTE_IMTR";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = 106; /* one per port */
    non_dma0->mem = MMU_CTR_COLOR_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "DROP_PKT_YEL";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = 106 * 4;
    non_dma0->dma_index_max[0] = (106 * 4) + 105;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "DROP_PKT_RED";
    non_dma1->dma_index_min[0] = 106 * 3;
    non_dma1->dma_index_max[0] = (106 * 3) + 105;
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_GRE";
    non_dma1->dma_index_min[0] = 106 * 2;
    non_dma1->dma_index_max[0] = (106 * 2) + 105;
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_YEL";
    non_dma1->dma_index_min[0] = 106 * 1;
    non_dma1->dma_index_max[0] = (106 * 1) + 105;
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_RED";
    non_dma1->dma_index_min[0] = 0;
    non_dma1->dma_index_max[0] = 105;
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA;
    /* SDK-67053 : Setting the port bitmap so that this counter can
       be cleared by the "clear c" command */
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 11;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = DROP_PKT_CNT_RQE_PKTr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "RQ_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    /* SDK-67053 : Setting the port bitmap so that this counter can
       be cleared by the "clear c" command */
    non_dma1->pbmp = PBMP_ALL(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_RQE_BYTE_64r;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "RQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_RQE_YELr;
    non_dma1->field = PKTCNTf;
    non_dma1->cname = "RQ_DROP_PKT_YEL";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_RQE_REDr;
    non_dma1->field = PKTCNTf;
    non_dma1->cname = "RQ_DROP_PKT_RED";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 4;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = THDI_POOL_SHARED_COUNT_SPr;
    non_dma0->field = TOTAL_BUFFER_COUNTf;
    non_dma0->cname = "POOL_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_POOL_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "POOL_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->num_entries = nports * non_dma0->entries_per_port;
    non_dma0->mem = THDI_PORT_PG_CNTRS_RT1_Xm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG_MIN_COUNTf;
    non_dma0->cname = "PG_MIN_PEAK";
    non_dma0->dma_mem[0] = THDI_PORT_PG_CNTRS_RT1_Xm;
    non_dma0->dma_mem[1] = THDI_PORT_PG_CNTRS_RT1_Ym;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_buf[1] = (full_chip)?
        &buf[soc_mem_index_count(unit, non_dma0->dma_mem[0]) *
             soc_mem_entry_words(unit, non_dma0->dma_mem[0])] : NULL;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    non_dma0->dma_index_max[1] = soc_mem_index_max(unit, non_dma0->dma_mem[1]);
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
            _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_MIN_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = PG_SHARED_COUNTf;
    non_dma1->cname = "PG_SHARED_PEAK";
    *non_dma_entries += non_dma1->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_SHARED_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->num_entries = nports * non_dma0->entries_per_port;
    non_dma0->mem = THDI_PORT_PG_CNTRS_RT2_Xm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PG_HDRM_COUNTf;
    non_dma0->cname = "PG_HDRM_PEAK";
    non_dma0->dma_mem[0] = THDI_PORT_PG_CNTRS_RT2_Xm;
    non_dma0->dma_mem[1] = THDI_PORT_PG_CNTRS_RT2_Ym;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_buf[1] = (full_chip)?
        &buf[soc_mem_index_count(unit, non_dma0->dma_mem[0]) *
             soc_mem_entry_words(unit, non_dma0->dma_mem[0])] : NULL;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    non_dma0->dma_index_max[1] = soc_mem_index_max(unit, non_dma0->dma_mem[1]);
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "PG_HDRM_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->num_entries =
        soc_mem_index_count(unit, MMU_THDM_DB_QUEUE_COUNT_0m) +
        soc_mem_index_count(unit, MMU_THDM_DB_QUEUE_COUNT_1m);
    non_dma0->mem = MMU_THDM_DB_QUEUE_COUNT_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = SHARED_COUNTf;
    non_dma0->cname = "QUEUE_PEAK";
    non_dma0->dma_mem[0] = MMU_THDM_DB_QUEUE_COUNT_0m;
    non_dma0->dma_mem[1] = MMU_THDM_DB_QUEUE_COUNT_1m;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_buf[1] =
        &buf[soc_mem_index_count(unit, non_dma0->dma_mem[0]) *
             soc_mem_entry_words(unit, non_dma0->dma_mem[0])];
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    non_dma0->dma_index_max[1] = soc_mem_index_max(unit, non_dma0->dma_mem[1]);
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "QUEUE_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_PEAK;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 10;
    non_dma0->num_entries =
        soc_mem_index_count(unit, MMU_THDU_XPIPE_COUNTER_QUEUEm) +
        soc_mem_index_count(unit, MMU_THDU_YPIPE_COUNTER_QUEUEm);
    non_dma0->mem = MMU_THDU_XPIPE_COUNTER_QUEUEm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = SHARED_COUNTf;
    non_dma0->cname = "UC_QUEUE_PEAK";
    non_dma0->dma_mem[0] = MMU_THDU_XPIPE_COUNTER_QUEUEm;
    non_dma0->dma_mem[1] = MMU_THDU_YPIPE_COUNTER_QUEUEm;
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_buf[1] =
        &buf[((soc_mem_index_count(unit, non_dma0->dma_mem[0])) - 1) *
             soc_mem_entry_words(unit, non_dma0->dma_mem[0])];
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]) - 1;
    non_dma0->dma_index_max[1] = soc_mem_index_max(unit, non_dma0->dma_mem[1]);
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "UC_QUEUE_CUR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_MMU_QCN_CNM -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_FLEX_MAPPING;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 10;
    non_dma0->num_entries = 2 * soc_mem_index_count(unit, MMU_QCN_CNM_COUNTERm);
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SAL_BOOT_QUICKTURN && SOC_IS_TD2P_TT2P(unit)) {
        non_dma0->num_entries = 1;
    }
#endif

    non_dma0->mem = MMU_QCN_CNM_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = CNM_CNTf;
    non_dma0->cname = "QCN_CNM_COUNTER";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_mem[0] = non_dma0->mem;
    non_dma0->dma_index_max[0] = soc_mem_index_max(unit, non_dma0->dma_mem[0]);
    non_dma0->soc_counter_hw_idx_get = soc_td2_qcn_counter_hw_index_get;
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_HIGH_PRI -
                                 SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_OBM;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 4;
    non_dma0->num_entries = 32;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = PGW_OBM0_HIGH_PRI_PKT_DROPr;
    non_dma0->field = COUNTERf;
    non_dma0->cname = "OBM0_HIGH_PRI_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_LOW_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM0_LOW_PRI_PKT_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM0_LOW_PRI_DROP_PKT";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_HIGH_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM0_HIGH_PRI_BYTE_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM0_HIGH_PRI_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_LOW_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM0_LOW_PRI_BYTE_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM0_LOW_PRI_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_HIGH_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_OBM;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 4;
    non_dma0->num_entries = 32;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = PGW_OBM1_HIGH_PRI_PKT_DROPr;
    non_dma0->field = COUNTERf;
    non_dma0->cname = "OBM1_HIGH_PRI_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_LOW_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM1_LOW_PRI_PKT_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM1_LOW_PRI_DROP_PKT";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_HIGH_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM1_HIGH_PRI_BYTE_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM1_HIGH_PRI_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_LOW_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM1_LOW_PRI_BYTE_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM1_LOW_PRI_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_HIGH_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_OBM;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 4;
    non_dma0->num_entries = 32;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = PGW_OBM2_HIGH_PRI_PKT_DROPr;
    non_dma0->field = COUNTERf;
    non_dma0->cname = "OBM2_HIGH_PRI_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_LOW_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM2_LOW_PRI_PKT_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM2_LOW_PRI_DROP_PKT";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_HIGH_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM2_HIGH_PRI_BYTE_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM2_HIGH_PRI_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_LOW_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM2_LOW_PRI_BYTE_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM2_LOW_PRI_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_HIGH_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_OBM;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 4;
    non_dma0->num_entries = 32;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = PGW_OBM3_HIGH_PRI_PKT_DROPr;
    non_dma0->field = COUNTERf;
    non_dma0->cname = "OBM3_HIGH_PRI_DROP_PKT";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_LOW_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM3_LOW_PRI_PKT_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM3_LOW_PRI_DROP_PKT";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_HIGH_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM3_HIGH_PRI_BYTE_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM3_HIGH_PRI_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_LOW_PRI -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = PGW_OBM3_LOW_PRI_BYTE_DROPr;
    non_dma1->field = COUNTERf;
    non_dma1->cname = "OBM3_LOW_PRI_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 1;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = THDI_GLOBAL_HDRM_COUNT_PIPEXr;
    non_dma0->field = GLOBAL_HDRM_COUNTf;

    non_dma0->cname = "GLOBAL_HDRM_COUNT_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_X_CURRENT -
        SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "GLOBAL_HDRM_COUNT_CURR";

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_Y_PEAK -
        SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PEAK;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries = 1;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = THDI_GLOBAL_HDRM_COUNT_PIPEYr;
    non_dma0->field = GLOBAL_HDRM_COUNTf;
    non_dma0->cname = "GLOBAL_HDRM_COUNT_PEAK";
    non_dma0->dma_buf[0] = buf;
    *non_dma_entries += non_dma0->num_entries;

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_GLOBAL_HDR_COUNT_Y_CURRENT -
        SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_CURRENT;
    non_dma2->cname = "GLOBAL_HDRM_COUNT_CURR";




    return SOC_E_NONE;

}
#endif /* BCM_TRIDENT2_SUPPORT || BCM_TRIDENT2PLUS_SUPPORT */

#ifdef BCM_HURRICANE_SUPPORT
/*
 * Function:
 *      _soc_counter_hu_non_dma_init
 * Purpose:
 *      Initialize Hurricane's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports.
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
STATIC int
_soc_counter_hu_non_dma_init(int unit, int nports, int non_dma_start_index,
        int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1;
    int max_cosq_per_port, total_num_cosq, port;
    int num_entries, alloc_size;
    uint32 *buf;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    max_cosq_per_port = 0;
    total_num_cosq = 0;
    for (port = 0; port < nports; port++) {
        if (num_cosq[unit][port] > max_cosq_per_port) {
            max_cosq_per_port = num_cosq[unit][port];
        }
        total_num_cosq += num_cosq[unit][port];
    }

    num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = max_cosq_per_port;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "PERQ_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = CNGDROPCOUNT1r;
    non_dma0->field = DROPPKTCOUNTf;
    non_dma0->cname = "DROP_PKT_YEL";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = CNGDROPCOUNT0r;
    non_dma0->field = DROPPKTCOUNTf;
    non_dma0->cname = "DROP_PKT_RED";
    *non_dma_entries += non_dma0->num_entries;

    if (SOC_IS_HURRICANE2(unit)) {
        non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                         SOC_COUNTER_NON_DMA_START];
        non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
        non_dma0->pbmp = PBMP_ALL(unit);
        non_dma0->base_index = non_dma_start_index + *non_dma_entries;
        non_dma0->entries_per_port = max_cosq_per_port;
        non_dma0->num_entries = total_num_cosq;
        non_dma0->mem = INVALIDm;
        non_dma0->reg = EGRDROPPKTCOUNT_COSr;
        non_dma0->field = DROPPEDPKTCOUNTf;
        non_dma0->cname = "PERQ_DROP_PKT_EGR";
        *non_dma_entries += non_dma0->num_entries;
    }

    return SOC_E_NONE;
}
#endif /* BCM_HURRICANE_SUPPORT */
#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
/*
 * Function:
 *      _soc_counter_gh_non_dma_init
 * Purpose:
 *      Initialize Greyhound's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports.
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
STATIC int
_soc_counter_gh_non_dma_init(int unit, int nports,
        int non_dma_start_index, int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1;
    int max_cosq_per_port, total_num_cosq, port;
    int num_entries, alloc_size;
    uint32 *buf;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    max_cosq_per_port = 0;
    total_num_cosq = 0;
    for (port = 0; port < nports; port++) {
        if (num_cosq[unit][port] > max_cosq_per_port) {
            max_cosq_per_port = num_cosq[unit][port];
        }
        total_num_cosq += num_cosq[unit][port];
    }

    num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = max_cosq_per_port;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "PERQ_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = CNGDROPCOUNT1r;
    non_dma0->field = DROPPKTCOUNTf;
    non_dma0->cname = "DROP_PKT_YEL";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = CNGDROPCOUNT0r;
    non_dma0->field = DROPPKTCOUNTf;
    non_dma0->cname = "DROP_PKT_RED";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = ING_DROPPKT_CNTr;
    non_dma0->field = FC_DROP_COUNTERf;
    non_dma0->cname = "DROP_PKT_ING";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = max_cosq_per_port;
    non_dma0->num_entries = total_num_cosq;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = EGRDROPPKTCOUNT_COSr;
    non_dma0->field = DROPPEDPKTCOUNTf;
    non_dma0->cname = "PERQ_DROP_PKT_EGR";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = WRED_DROP_COUNTERr;
    non_dma0->field = COUNTERf;
    non_dma0->cname = "WRED_DROP_CNT";
    *non_dma_entries += non_dma0->num_entries;

    num_entries = soc_mem_index_count(unit, EGR_PER_Q_ECN_MARKEDm);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, EGR_PER_Q_ECN_MARKEDm) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_ECN_MARKED -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = max_cosq_per_port;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = EGR_PER_Q_ECN_MARKEDm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "PERQ_ECN_MARKED";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    return SOC_E_NONE;
}
#endif /* BCM_GREYHOUND_SUPPORT || BCM_HURRICANE3_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
/*
 * Function:
 *      _soc_counter_gh2_non_dma_init
 * Purpose:
 *      Initialize Greyhound2's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports.
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
STATIC int
_soc_counter_gh2_non_dma_init(int unit, int nports,
        int non_dma_start_index, int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1;
    int max_cosq_per_port, total_num_cosq, port;
    int num_entries, alloc_size;
    uint32 *buf;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    max_cosq_per_port = 0;
    total_num_cosq = 0;
    for (port = 0; port < nports; port++) {
        if (num_cosq[unit][port] > max_cosq_per_port) {
            max_cosq_per_port = num_cosq[unit][port];
        }
        total_num_cosq += num_cosq[unit][port];
    }

    num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 64;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "PERQ_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = CNGDROPCOUNT1r;
    non_dma0->field = DROPPKTCOUNTf;
    non_dma0->cname = "DROP_PKT_YEL";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = CNGDROPCOUNT0r;
    non_dma0->field = DROPPKTCOUNTf;
    non_dma0->cname = "DROP_PKT_RED";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = ING_DROPPKT_CNTr;
    non_dma0->field = FC_DROP_COUNTERf;
    non_dma0->cname = "DROP_PKT_ING";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 64;
    non_dma0->num_entries = nports * non_dma0->entries_per_port;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = EGRDROPPKTCOUNT_COSr;
    non_dma0->field = DROPPEDPKTCOUNTf;
    non_dma0->cname = "PERQ_DROP_PKT_EGR";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = WRED_DROP_COUNTERr;
    non_dma0->field = COUNTERf;
    non_dma0->cname = "WRED_DROP_CNT";
    *non_dma_entries += non_dma0->num_entries;

    num_entries = soc_mem_index_count(unit, EGR_PER_Q_ECN_MARKEDm);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, EGR_PER_Q_ECN_MARKEDm) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_ECN_MARKED -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = max_cosq_per_port;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = EGR_PER_Q_ECN_MARKEDm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "PERQ_ECN_MARKED";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_EXPRESS-
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = TAS_OVERRUN_PKT_CNT_EXPRESSr;
    non_dma0->field = CNTf;
    non_dma0->cname = "TAS_TXOVERRUN_EXPRESS";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_PREEMPT-
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = TAS_OVERRUN_PKT_CNT_PREEMPTr;
    non_dma0->field = CNTf;
    non_dma0->cname = "TAS_TXOVERRUN_PREEMPT";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 =
        &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PREEMPT_MAC_MERGE_HOLD_CNT-
                              SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = MACMERGERHOLDCOUNTr;
    non_dma0->field = CNTf;
    non_dma0->cname = "PREEMPT_MACMERGE_HOLD_CNT";
    *non_dma_entries += non_dma0->num_entries;

    num_entries = soc_mem_index_count(unit, ING_RX_COUNTERm) >> 1;
    alloc_size = num_entries *
        soc_mem_entry_words(unit, ING_RX_COUNTERm) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_ING_RX_COUNTER_NON_PREEMPTABLE_CNT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = ING_RX_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "ING_RX_NON_PREEMPTABLE_CNT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = 0;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    num_entries = soc_mem_index_count(unit, ING_RX_COUNTERm) >> 1;
    alloc_size = num_entries *
        soc_mem_entry_words(unit, ING_RX_COUNTERm) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_ING_RX_COUNTER_PREEMPTABLE_CNT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = ING_RX_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "ING_RX_PREEMPTABLE_CNT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = soc_mem_index_count(unit, ING_RX_COUNTERm) >> 1;
    non_dma0->dma_index_max[0] = soc_mem_index_count(unit, ING_RX_COUNTERm) - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    num_entries = soc_mem_index_count(unit, EGR_MFRAME_COUNTERm);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, EGR_MFRAME_COUNTERm) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_TX_COUNTER_PREEMPTABLE_CNT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = EGR_MFRAME_COUNTERm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "EGR_TX_PREEMPTABLE_CNT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = 0;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    return SOC_E_NONE;
}
#endif /* BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_KATANA_SUPPORT

STATIC int
_soc_counter_kt_cosq_gport_non_dma_init(int unit, int nports,
                                        int non_dma_start_index,
                                        int *non_dma_entries)
{
    soc_control_t *soc;
#ifdef BCM_KATANA2_SUPPORT
    soc_port_t my_port;
#endif
    soc_counter_non_dma_t *non_dma0, *non_dma1;
    _kt_cosq_counter_mem_map_t *mem_map;
    int table_size, id;
    uint32 *buf;
    char *cname_pkt[] = { "DROP_PKT", "ACCEPT_PKT",
                          "DROP_PKT_G", "DROP_PKT_Y",
                          "DROP_PKT_R", "ACCEPT_PKT_G",
                          "ACCEPT_PKT_Y", "ACCEPT_PKT_R",
                          "PERQ_PKT" };
    char *cname_byte[] = { "DROP_BYTE", "ACCEPT_BYTE",
                          "DROP_BYTE_G", "DROP_BYTE_Y",
                          "DROP_BYTE_R", "ACCEPT_BYTE_G",
                          "ACCEPT_BYTE_Y", "ACCEPT_BYTE_R",
                          "PERQ_BYTE" };

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    table_size = soc_mem_index_count(unit, CTR_FLEX_COUNT_0m) *
        soc_mem_entry_words(unit, CTR_FLEX_COUNT_0m) * sizeof(uint32);

    buf = soc_cm_salloc(unit, table_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, table_size);

    mem_map = _kt_cosq_gport_mem_map;

    /*
     * CTR_FLEX_COUNT_0 = Total ENQ discarded
     * CTR_FLEX_COUNT_1 = Total ENQ Accepted
     * CTR_FLEX_COUNT_2 = Green ENQ discarded
     * CTR_FLEX_COUNT_3 = Yellow ENQ discarded
     * CTR_FLEX_COUNT_4 = Red packet ENQ discarded
     * CTR_FLEX_COUNT_5 = Green ENQ Accepted
     * CTR_FLEX_COUNT_6 = Yellow ENQ Accepted
     * CTR_FLEX_COUNT_7 = Red ENQ Accepted
     * CTR_FLEX_COUNT_8 = Total DEQ
     */
    for (id = 0; id < 9; id++) {
        if (soc_feature(unit, soc_feature_counter_toggled_read) &&
            (mem_map[id].non_dma_id == SOC_COUNTER_NON_DMA_COSQ_DROP_PKT ||
             mem_map[id].non_dma_id == SOC_COUNTER_NON_DMA_COSQ_ACCEPT_PKT)) {
            continue;
        }
        non_dma0 = &soc->counter_non_dma[mem_map[id].non_dma_id -
                                         SOC_COUNTER_NON_DMA_START];
        non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                          _SOC_COUNTER_NON_DMA_DO_DMA |
                          _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
        if (id == 0) {
            non_dma0->flags |= _SOC_COUNTER_NON_DMA_ALLOC;
        }
        if (!soc_feature(unit, soc_feature_counter_toggled_read)) {
            /* adjust the extra count 1 in s/w counters */
            if (!SOC_IS_KATANA2(unit)) {
                non_dma0->flags |= _SOC_COUNTER_NON_DMA_EXTRA_COUNT;
            }
        }
        non_dma0->pbmp = PBMP_ALL(unit);
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            SOC_PBMP_CLEAR(non_dma0->pbmp);
            for (my_port =  SOC_INFO(unit).cmic_port;
                 my_port <= SOC_INFO(unit).lb_port;
                 my_port++) {
                 if (SOC_INFO(unit).block_valid[SOC_PORT_BLOCK(unit,my_port)]) {
                     SOC_PBMP_PORT_ADD(non_dma0->pbmp, my_port);
                 }
            }
        }
#endif
        non_dma0->base_index = non_dma_start_index + *non_dma_entries;
        non_dma0->entries_per_port = 8;
        non_dma0->num_entries = soc_mem_index_count(unit, CTR_FLEX_COUNT_0m);
        non_dma0->mem = mem_map[id].mem;
        non_dma0->reg = INVALIDr;
        non_dma0->field = COUNTf;
        non_dma0->cname = cname_pkt[id];
        non_dma0->dma_buf[0] = buf;
        non_dma0->dma_index_max[0] = non_dma0->num_entries - 1;
        non_dma0->dma_mem[0] = non_dma0->mem;
        *non_dma_entries += non_dma0->num_entries;

        non_dma1 = &soc->counter_non_dma[mem_map[id].non_dma_id -
                                        SOC_COUNTER_NON_DMA_START + 1];
        sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
        non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                          _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
        non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->field = BYTE_COUNTf;
        non_dma1->cname = cname_byte[id];
        *non_dma_entries += non_dma1->num_entries;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_counter_katana_non_dma_init
 * Purpose:
 *      Initialize Katana's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports.
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
STATIC int
_soc_counter_katana_non_dma_init(int unit, int nports, int non_dma_start_index,
                                 int *non_dma_entries)
{
    soc_control_t *soc;
#ifdef BCM_KATANA2_SUPPORT
    soc_port_t my_port;
#endif
    soc_counter_non_dma_t *non_dma0, *non_dma1;
    _kt_cosq_counter_mem_map_t *mem_map;
    int table_size;
    uint32 *buf;
    int i, id;
    int entry_words, buf_base;
    char *cname_pkt[] = { "DROP_PKT", "DROP_PKT_R",
                          "PERQ_PKT" };
    char *cname_byte[] = { "DROP_BYTE", "DROP_BYTE_R",
                           "PERQ_BYTE" };

    if (soc_feature(unit, soc_feature_cosq_gport_stat_ability)) {
        SOC_IF_ERROR_RETURN
            (_soc_counter_kt_cosq_gport_non_dma_init(unit, nports,
                non_dma_start_index, non_dma_entries));
        return SOC_E_NONE;
    }
    mem_map = _kt_cosq_mem_map;
    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    entry_words = soc_mem_entry_words(unit, CTR_FLEX_COUNT_0m);
    table_size = soc_mem_index_count(unit, CTR_FLEX_COUNT_0m) * 4 *
                 entry_words * sizeof(uint32);
    buf = soc_cm_salloc(unit, table_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, table_size);

    /*
     * CTR_FLEX_COUNT_0/1/2/3 = Total ENQ discarded
     * CTR_FLEX_COUNT_4/5/6/7  = Red packet ENQ discarded
     * CTR_FLEX_COUNT_8/9/10/11  = Total DEQ
     */
    for (id = 0; id < 12; id = id + 4) {
        if (soc_feature(unit, soc_feature_counter_toggled_read) &&
            mem_map[id].non_dma_id == SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED) {
            continue;
        }
        non_dma0 = &soc->counter_non_dma[mem_map[id].non_dma_id -
                                     SOC_COUNTER_NON_DMA_START];
        non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                          _SOC_COUNTER_NON_DMA_DO_DMA |
                          _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
        if (id == 0) {
            non_dma0->flags |= _SOC_COUNTER_NON_DMA_ALLOC;
        }
        if (!soc_feature(unit, soc_feature_counter_toggled_read)) {
            /* adjust the extra count 1 in s/w counters */
            if (!SOC_IS_KATANA2(unit)) {
                non_dma0->flags |= _SOC_COUNTER_NON_DMA_EXTRA_COUNT;
            }
        }
        non_dma0->pbmp = PBMP_ALL(unit);
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            SOC_PBMP_CLEAR(non_dma0->pbmp);
            for (my_port =  SOC_INFO(unit).cmic_port;
                 my_port <= SOC_INFO(unit).lb_port;
                 my_port++) {
                 if (SOC_INFO(unit).block_valid[SOC_PORT_BLOCK(unit,my_port)]) {
                     SOC_PBMP_PORT_ADD(non_dma0->pbmp,my_port);
                 }
            }
        }
#endif
        non_dma0->base_index = non_dma_start_index + *non_dma_entries;
        non_dma0->entries_per_port = 8;
        non_dma0->mem = mem_map[id].mem;
        if (soc_feature(unit, soc_feature_counter_toggled_read) &&
            (id == 8)) {
            non_dma0->mem = mem_map[id].mem - 4;
        }
        non_dma0->num_entries = 4 * soc_mem_index_count(unit, non_dma0->mem);
        non_dma0->reg = INVALIDr;
        non_dma0->field = COUNTf;
        non_dma0->cname = cname_pkt[id / 4];

        entry_words = soc_mem_entry_words(unit, non_dma0->mem);
        for (i = 0; i < 4; i++) {
            buf_base = i * entry_words * soc_mem_index_count(unit, non_dma0->mem);
            non_dma0->dma_buf[i] = &buf[buf_base];
            non_dma0->dma_index_min[i] = 0;
            non_dma0->dma_index_max[i] = soc_mem_index_count(unit, non_dma0->mem) - 1;
            non_dma0->dma_mem[i] = mem_map[id + i].mem;
            if (soc_feature(unit, soc_feature_counter_toggled_read) &&
                (id == 8)) {
                non_dma0->dma_mem[i] = mem_map[id + i].mem - 4;
            }
        }
        *non_dma_entries += non_dma0->num_entries;

        non_dma1 = &soc->counter_non_dma[mem_map[id].non_dma_id -
                                    SOC_COUNTER_NON_DMA_START + 1];
        sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
        non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                          _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
        non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->field = BYTE_COUNTf;
        non_dma1->cname = cname_byte[id / 4];
        *non_dma_entries += non_dma1->num_entries;
    }

    return SOC_E_NONE;
}
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
STATIC int
_soc_counter_sb2_cosq_gport_non_dma_init(int unit, int nports,
                                        int non_dma_start_index,
                                        int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1;
    _sb2_cosq_counter_mem_map_t *mem_map;
    int table_size, id, max_id = 9;
    uint32 *buf;
    char *cname_pkt[] = { "DROP_PKT", "ACCEPT_PKT",
                          "DROP_PKT_G", "DROP_PKT_Y",
                          "DROP_PKT_R", "ACCEPT_PKT_G",
                          "ACCEPT_PKT_Y", "ACCEPT_PKT_R",
                          "MC_PERQ_PKT" };
    char *cname_byte[] = { "DROP_BYTE", "ACCEPT_BYTE",
                          "DROP_BYTE_G", "DROP_BYTE_Y",
                          "DROP_BYTE_R", "ACCEPT_BYTE_G",
                          "ACCEPT_BYTE_Y", "ACCEPT_BYTE_R",
                          "MC_PERQ_BYTE" };

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    table_size = soc_mem_index_count(unit, CTR_FLEX_COUNT_0m) *
        soc_mem_entry_words(unit, CTR_FLEX_COUNT_0m) * sizeof(uint32);

    buf = soc_cm_salloc(unit, table_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, table_size);

    if (SOC_IS_SABER2(unit)) {
        mem_map = _sb2_cosq_gport_mem_map;
        max_id = 6;
    } else {
        mem_map = _kt2_cosq_gport_mem_map;
    }

    /*
     * CTR_FLEX_COUNT_0 = Total ENQ discarded
     * CTR_FLEX_COUNT_1 = Total ENQ Accepted
     * CTR_FLEX_COUNT_2 = Green ENQ discarded
     * CTR_FLEX_COUNT_3 = Yellow ENQ discarded
     * CTR_FLEX_COUNT_4 = Red packet ENQ discarded
     * CTR_FLEX_COUNT_5 = Total DEQ
     */
    for (id = 0; id < max_id; id++) {
        non_dma0 = &soc->counter_non_dma[mem_map[id].non_dma_id -
                                         SOC_COUNTER_NON_DMA_START];
        non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                          _SOC_COUNTER_NON_DMA_DO_DMA |
                          _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
        if (id == 0) {
            non_dma0->flags |= _SOC_COUNTER_NON_DMA_ALLOC;
        }
        if (!soc_feature(unit, soc_feature_counter_toggled_read)) {
            /* adjust the extra count 1 in s/w counters */
            non_dma0->flags |= _SOC_COUNTER_NON_DMA_EXTRA_COUNT;
        }
        non_dma0->pbmp = PBMP_ALL(unit);
        non_dma0->base_index = non_dma_start_index + *non_dma_entries;
        non_dma0->entries_per_port = 8;
        non_dma0->num_entries = soc_mem_index_count(unit, CTR_FLEX_COUNT_0m);
        non_dma0->mem = mem_map[id].mem;
        non_dma0->reg = INVALIDr;
        non_dma0->field = COUNTf;
        non_dma0->cname = cname_pkt[id];
        non_dma0->dma_buf[0] = buf;
        non_dma0->dma_index_max[0] = non_dma0->num_entries - 1;
        non_dma0->dma_mem[0] = non_dma0->mem;
        *non_dma_entries += non_dma0->num_entries;

        non_dma1 = &soc->counter_non_dma[mem_map[id].non_dma_id -
                                        SOC_COUNTER_NON_DMA_START + 1];
        sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
        non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                          _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
        non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->field = BYTE_COUNTf;
        non_dma1->cname = cname_byte[id];
        *non_dma_entries += non_dma1->num_entries;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_counter_saber2_non_dma_init
 * Purpose:
 *      Initialize Saber2's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports.
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
STATIC int
_soc_counter_saber2_non_dma_init(int unit, int nports, int non_dma_start_index,
                                 int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1;
    _sb2_cosq_counter_mem_map_t *mem_map;
    soc_port_t my_port;
    int table_size;
    uint32 *buf;
    int i, id, max_id = 6, increment = 2;
    int entry_words, buf_base;
    uint16 dev_id;
    uint8 rev_id;

    if (soc_feature(unit, soc_feature_cosq_gport_stat_ability)) {
        SOC_IF_ERROR_RETURN
            (_soc_counter_sb2_cosq_gport_non_dma_init(unit, nports,
                                                      non_dma_start_index, non_dma_entries));
        return SOC_E_NONE;
    }
#if defined (BCM_KATANA2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        mem_map = _sb2_cosq_mem_map;
    } else {
        mem_map = _kt2_cosq_mem_map;
        increment = 4;
        max_id = 12;
    }
#endif
#if defined (BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        mem_map = _ml_cosq_mem_map;
        increment = 1;
        max_id = 3;
    }
#endif
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (dev_id == BCM56233_DEVICE_ID) {
        /*
         * CTR_FLEX_COUNT_0 = Total ENQ discarded
         * CTR_FLEX_COUNT_1  = Red packet ENQ discarded
         * CTR_FLEX_COUNT_2  = Total DEQ
         */
        mem_map = _dg2_cosq_mem_map;
        max_id = 3;
        increment = 1;
    }
    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    entry_words = soc_mem_entry_words(unit, CTR_FLEX_COUNT_0m);
    table_size = (soc_mem_index_count(unit, CTR_FLEX_COUNT_0m) * increment *
            entry_words * sizeof(uint32));

    buf = soc_cm_salloc(unit, table_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, table_size);

    /*
     * CTR_FLEX_COUNT_0/1 = Total ENQ discarded
     * CTR_FLEX_COUNT_2/3  = Red packet ENQ discarded
     * CTR_FLEX_COUNT_4/5  = Total DEQ
     */
    for (id = 0; id < max_id; id = id + increment) {
        if (soc_feature(unit, soc_feature_counter_toggled_read) &&
                mem_map[id].non_dma_id == SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_RED) {
            continue;
        }
        non_dma0 = &soc->counter_non_dma[mem_map[id].non_dma_id -
            SOC_COUNTER_NON_DMA_START];
        non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
            _SOC_COUNTER_NON_DMA_DO_DMA |
            _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
        if (id == 0) {
            non_dma0->flags |= _SOC_COUNTER_NON_DMA_ALLOC;
        }
        SOC_PBMP_CLEAR(non_dma0->pbmp);
        for (my_port =  SOC_INFO(unit).cmic_port;
                my_port <= SOC_INFO(unit).lb_port;
                my_port++) {
            if (SOC_INFO(unit).block_valid[SOC_PORT_BLOCK(unit,my_port)]) {
                SOC_PBMP_PORT_ADD(non_dma0->pbmp, my_port);
            }
        }

        non_dma0->base_index = non_dma_start_index + *non_dma_entries;
        non_dma0->entries_per_port = 48; /* Need 48 entries to display CPU queues */
        non_dma0->mem = mem_map[id].mem;
        non_dma0->num_entries = increment * soc_mem_index_count(unit, non_dma0->mem);
        non_dma0->reg = INVALIDr;
        non_dma0->field = COUNTf;
        non_dma0->cname = mem_map[id].cname_pkt;

        entry_words = soc_mem_entry_words(unit, non_dma0->mem);
        for (i = 0; i < increment; i++) {
            buf_base = i * entry_words * soc_mem_index_count(unit, non_dma0->mem);
            non_dma0->dma_buf[i] = &buf[buf_base];
            non_dma0->dma_index_min[i] = 0;
            non_dma0->dma_index_max[i] = soc_mem_index_count(unit, non_dma0->mem) - 1;
            non_dma0->dma_mem[i] = mem_map[id + i].mem;
        }
        *non_dma_entries += non_dma0->num_entries;

        non_dma1 = &soc->counter_non_dma[mem_map[id].non_dma_id -
            SOC_COUNTER_NON_DMA_START + 1];
        sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
        non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
            _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
        non_dma1->base_index = non_dma_start_index + *non_dma_entries;
        non_dma1->field = BYTE_COUNTf;
        non_dma1->cname = mem_map[id].cname_byte;
        *non_dma_entries += non_dma1->num_entries;
    }
    return SOC_E_NONE;
}
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
/*
 * Function:
 *      _soc_counter_tr_non_dma_init
 * Purpose:
 *      Initialize Triumph's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports.
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
STATIC int
_soc_counter_tr_non_dma_init(int unit, int nports, int non_dma_start_index,
        int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1;
    int max_cosq_per_port, total_num_cosq, port;
    int num_entries, alloc_size;
    uint32 *buf;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    max_cosq_per_port = 0;
    total_num_cosq = 0;
    for (port = 0; port < nports; port++) {
        if (num_cosq[unit][port] > max_cosq_per_port) {
            max_cosq_per_port = num_cosq[unit][port];
        }
        total_num_cosq += num_cosq[unit][port];
    }

    num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = max_cosq_per_port;
    non_dma0->num_entries = num_entries;
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "PERQ_PKT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PERQ_REG;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = max_cosq_per_port;
    non_dma0->num_entries = total_num_cosq;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = DROP_PKT_CNTr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "PERQ_DROP_PKT";
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_BYTE_CNTr;
    non_dma1->cname = "PERQ_DROP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = DROP_PKT_CNT_INGr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "DROP_PKT_ING";
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_BYTE_CNT_INGr;
    non_dma1->cname = "DROP_BYTE_ING";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_YELr;
    non_dma1->cname = "DROP_PKT_YEL";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = DROP_PKT_CNT_REDr;
    non_dma1->cname = "DROP_PKT_RED";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_TX_LLFC_MSG_CNT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->pbmp = PBMP_ALL(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->mem = INVALIDm;
    non_dma1->reg = TXLLFCMSGCNTr;
    non_dma1->field = TXLLFCMSGCNT_STATSf;
    non_dma1->cname = "MAC_TXLLFCMSG";
    non_dma1->entries_per_port = 1;
    non_dma1->num_entries = nports;
    *non_dma_entries += non_dma1->num_entries;
    return SOC_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_SCORPION_SUPPORT
/*
 * Function:
 *      _soc_counter_sc_non_dma_init
 * Purpose:
 *      Initialize Scorpion's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
_soc_counter_sc_non_dma_init(int unit, int nports, int non_dma_start_index,
        int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1;
    int max_cosq_per_port, total_num_cosq, port;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    max_cosq_per_port = 0;
    total_num_cosq = 0;
    for (port = 0; port < nports; port++) {
        if (num_cosq[unit][port] > max_cosq_per_port) {
            max_cosq_per_port = num_cosq[unit][port];
        }
        total_num_cosq += num_cosq[unit][port];
    }

    /* EGR_PERQ_XMT_COUNTERS register support in scorpion */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = SOC_REG_NUMELS(unit, EGR_PERQ_XMT_COUNTERSr);
    non_dma0->num_entries = nports * non_dma0->entries_per_port;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = EGR_PERQ_XMT_COUNTERSr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "PERQ_PKT";
    *non_dma_entries += non_dma0->num_entries;


    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_PERQ_REG;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = max_cosq_per_port;
    non_dma0->num_entries = total_num_cosq;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = HOLDROP_PKT_CNTr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "PERQ_DROP_PKT";
    *non_dma_entries += non_dma0->num_entries;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_IBP -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = IBP_DROP_PKT_CNTr;
    non_dma0->field = COUNTf;
    non_dma0->cname = "DROP_PKT_IBP";
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_CFAP -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CFAP_DROP_PKT_CNTr;
    non_dma1->cname = "DROP_PKT_CFAP";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = YELLOW_CNG_DROP_CNTr;
    non_dma1->cname = "DROP_PKT_YEL";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = RED_CNG_DROP_CNTr;
    non_dma1->cname = "DROP_PKT_RED";
    *non_dma_entries += non_dma1->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_TX_LLFC_MSG_CNT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->pbmp = PBMP_ALL(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->mem = INVALIDm;
    non_dma1->reg = TXLLFCMSGCNTr;
    non_dma1->field = TXLLFCMSGCNT_STATSf;
    non_dma1->cname = "MAC_TXLLFCMSG";
    non_dma1->entries_per_port = 1;
    non_dma1->num_entries = nports;
    *non_dma_entries += non_dma1->num_entries;

    return SOC_E_NONE;
}
#endif /* BCM_SCORPION_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
/*
 * Function:
 *      _soc_counter_shadow_non_dma_init
 * Purpose:
 *      Initialize Shadow's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports.
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_MEMORY
 */
STATIC int
_soc_counter_shadow_non_dma_init(int unit, int nports, int non_dma_start_index,
        int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0,*non_dma1;
    int num_entries,alloc_size;
    uint32 *buf;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    /* Reusing non_dma id for interlaken counters */

    /* IL_STAT_MEM_0 Entries */
    num_entries = soc_mem_index_count(unit, IL_STAT_MEM_0m);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, IL_STAT_MEM_0m) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);


    /* IL_STAT_MEM_0: RX_STAT_PKT_COUNTf */
    /* Overload SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_IL_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = 1;
    non_dma0->mem = IL_STAT_MEM_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = RX_STAT_PKT_COUNTf;
    non_dma0->cname = "IL_RX_PKTCNT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = 0;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    /* IL_STAT_MEM_0: RX_STAT_BYTE_COUNTf */
    /* Overload SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = RX_STAT_BYTE_COUNTf;
    non_dma1->cname = "IL_RX_BYTECNT";
    *non_dma_entries += non_dma1->num_entries;

    /* IL_STAT_MEM_0: RX_STAT_BAD_PKT_ILERR_COUNTf */
    /* Overload SOC_COUNTER_NON_DMA_COSQ_DROP_PKT */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = RX_STAT_BAD_PKT_ILERR_COUNTf;
    non_dma1->cname = "IL_RX_ERRCNT";
    *non_dma_entries += non_dma1->num_entries;


    /* IL_STAT_MEM_1 Entries */
    num_entries = soc_mem_index_count(unit, IL_STAT_MEM_1m);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, IL_STAT_MEM_1m) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);


    /* IL_STAT_MEM_1:  RX_STAT_GTMTU_PKT_COUNTf Greater that MTU size*/
    /* Overload SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_IL_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = 1;
    non_dma0->mem = IL_STAT_MEM_1m;
    non_dma0->reg = INVALIDr;
    non_dma0->field =  RX_STAT_GTMTU_PKT_COUNTf;
    non_dma0->cname = "IL_RX_GTMTUCNT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = 0;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    /* IL_STAT_MEM_1: RX_STAT_EQMTU_PKT_COUNTf - equal to MTU Size*/
    /* Overload SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = RX_STAT_EQMTU_PKT_COUNTf;
    non_dma1->cname = "IL_RX_EQMTUCNT";
    *non_dma_entries += non_dma1->num_entries;


    /* IL_STAT_MEM_1: RX_STAT_IEEE_CRCERR_PKT_COUNTf - IEEE CRC err count*/
    /* Overload SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = RX_STAT_IEEE_CRCERR_PKT_COUNTf;
    non_dma1->cname = "IL_RX_CRCERRCNT";
    *non_dma_entries += non_dma1->num_entries;

    /* IL_STAT_MEM_2 Entries */
    num_entries = soc_mem_index_count(unit, IL_STAT_MEM_2m);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, IL_STAT_MEM_2m) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    /* IL_STAT_MEM_2:  RX_STAT_PKT_COUNTf Rx stat packet count*/
    /* Overload SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_IL_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = 1;
    non_dma0->mem = IL_STAT_MEM_2m;
    non_dma0->reg = INVALIDr;
    non_dma0->field =  RX_STAT_PKT_COUNTf;
    non_dma0->cname = "IL_RX_STATCNT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = 0;
    non_dma0->dma_index_max[0] = num_entries - 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    /* IL_STAT_MEM_3 Entries */
    num_entries = soc_mem_index_count(unit, IL_STAT_MEM_3m);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, IL_STAT_MEM_3m) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    /* IL_STAT_MEM_3:  TX_STAT_PKT_COUNT */
    /* Overload SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma0->pbmp = PBMP_IL_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = 1;
    non_dma0->mem = IL_STAT_MEM_3m;
    non_dma0->reg = INVALIDr;
    non_dma0->field =  TX_STAT_PKT_COUNTf;
    non_dma0->cname = "IL_TX_PKTCNT";
    non_dma0->dma_buf[0] = buf;
    non_dma0->dma_index_min[0] = 0;
    non_dma0->dma_index_max[0] = 1;
    non_dma0->dma_mem[0] = non_dma0->mem;
    *non_dma_entries += non_dma0->num_entries;

    /* IL_STAT_MEM_3: TX_STAT_BYTE_COUNTf */
    /* Overload SOC_COUNTER_NON_DMA_PORT_DROP_PKT_IBP */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_IBP -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = TX_STAT_BYTE_COUNTf;
    non_dma1->cname = "IL_TX_BYTECNT";
    *non_dma_entries += non_dma1->num_entries;

    /* IL_STAT_MEM_3: TX_STAT_BAD_PKT_PERR_COUNTf */
    /* Overload SOC_COUNTER_NON_DMA_PORT_DROP_PKT_CFAP */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_CFAP -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = TX_STAT_BAD_PKT_PERR_COUNTf;
    non_dma1->cname = "IL_TX_ERRCNT";
    *non_dma_entries += non_dma1->num_entries;

    /* IL_STAT_MEM_3: TX_STAT_GTMTU_PKT_COUNTf */
    /* Overload SOC_COUNTER_NON_DMA_PORT_DROP_PKT */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = TX_STAT_GTMTU_PKT_COUNTf;
    non_dma1->cname = "IL_TX_GTMTUCNT";
    *non_dma_entries += non_dma1->num_entries;

    /* IL_STAT_MEM_3: TX_STAT_EQMTU_PKT_COUNT - Greater than mtu size */
    /* Overload SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = TX_STAT_EQMTU_PKT_COUNTf;
    non_dma1->cname = "IL_TX_EQMTUCNT";
    *non_dma_entries += non_dma1->num_entries;

    /* IL_STAT_MEM_4 Entries */
    /* IL_STAT_MEM_4 overloaded on SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT */
    num_entries = soc_mem_index_count(unit, IL_STAT_MEM_4m);
    alloc_size = num_entries *
        soc_mem_entry_words(unit, IL_STAT_MEM_4m) * sizeof(uint32);

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);

    /* TX_STAT_PKT_COUNT */
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
        _SOC_COUNTER_NON_DMA_DO_DMA | _SOC_COUNTER_NON_DMA_ALLOC;
    non_dma1->pbmp = PBMP_IL_ALL(unit);
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->entries_per_port = 1;
    non_dma1->num_entries = 1;
    non_dma1->mem = IL_STAT_MEM_4m;
    non_dma1->reg = INVALIDr;
    non_dma1->field = TX_STAT_PKT_COUNTf;
    non_dma1->cname = "IL_TX_STATCNT";
    non_dma1->dma_buf[0] = buf;
    non_dma1->dma_index_min[0] = 0;
    non_dma1->dma_index_max[0] = num_entries - 1;
    non_dma1->dma_mem[0] = non_dma1->mem;
    *non_dma_entries += non_dma1->num_entries;

    return SOC_E_NONE;
}
#endif /* BCM_SHADOW_SUPPORT */

#ifdef BCM_BRADLEY_SUPPORT
/*
 * Function:
 *      _soc_counter_hb_non_dma_init
 * Purpose:
 *      Initialize Bradley's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
_soc_counter_hb_non_dma_init(int unit, int nports, int non_dma_start_index,
        int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    non_dma = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT -
                                    SOC_COUNTER_NON_DMA_START];
    non_dma->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma->pbmp = PBMP_ALL(unit);
    non_dma->base_index = non_dma_start_index + *non_dma_entries;
    non_dma->entries_per_port = 1;
    non_dma->num_entries = nports;
    non_dma->mem = INVALIDm;
    non_dma->reg = DROP_PKT_CNTr;
    non_dma->field = COUNTf;
    non_dma->cname = "DROP_PKT_MMU";
    *non_dma_entries += non_dma->num_entries;

    return SOC_E_NONE;
}
#endif /* BCM_BRADLEY_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
/*
 * Function:
 *      _soc_counter_fb_non_dma_init
 * Purpose:
 *      Initialize Firebolt's non-DMA counters.
 * Parameters:
 *      unit - The SOC unit number
 *      nports - Number of ports
 *      non_dma_start_index - The starting index of non-DMA counter entries.
 *      non_dma_entries - (OUT) The number of non-DMA counter entries.
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
_soc_counter_fb_non_dma_init(int unit, int nports, int non_dma_start_index,
        int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1;

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->num_entries = nports;
    non_dma0->mem = INVALIDm;
    non_dma0->reg = CNGDROPCOUNT1r;
    non_dma0->field = DROPPKTCOUNTf;
    non_dma0->cname = "DROP_PKT_YEL";
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    sal_memcpy(non_dma1, non_dma0, sizeof(soc_counter_non_dma_t));
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->reg = CNGDROPCOUNT0r;
    non_dma1->cname = "DROP_PKT_RED";
    *non_dma_entries += non_dma1->num_entries;

    return SOC_E_NONE;
}
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_SBUSDMA_SUPPORT
static sbusdma_desc_handle_t *_soc_blk_counter_handles[SOC_MAX_NUM_DEVICES] = {NULL};
static soc_blk_ctr_process_t **_blk_ctr_process[SOC_MAX_NUM_DEVICES] = {NULL};
void _soc_sbusdma_blk_ctr_cb(int unit, int status, sbusdma_desc_handle_t handle,
                             void *data)
{
    uint16 i, j, arr, bufidx, hwidx;
    uint64 ctr_new, ctr_prev, ctr_diff;
    int wide, width, f;
    volatile uint64 *vptr;
    uint32 *hwptr;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_reg_t ctr_reg;
    soc_blk_ctr_process_t *ctr_process = _blk_ctr_process[unit][PTR_TO_INT(data)];
    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit,
                            "In blk counter cb [%d]\n"), handle));
    if (status == SOC_E_NONE) {
        LOG_DEBUG(BSL_LS_SOC_COUNTER,
                    (BSL_META_U(unit,
                                "Complete: blk:%d, index: %d, entries: %d.\n"),
                     ctr_process->blk, ctr_process->bindex, ctr_process->entries));
        /* Process data (need bindex, count) */
        hwptr = (uint32*)ctr_process->buff;
        for (i=0, bufidx = 0, hwidx = 0; i<ctr_process->entries; i++) {
            arr = soc->blk_ctr_desc[ctr_process->bindex].desc[i].entries;
            ctr_reg = soc->blk_ctr_desc[ctr_process->bindex].desc[i].reg;
            wide = (soc->blk_ctr_desc[ctr_process->bindex].desc[i].width > 1) ? 1 : 0;
            for (j=0; j<arr; j++, bufidx++, hwidx++) {
                uint32 *ptr = &hwptr[hwidx];
                if (wide) {
                    if (soc->counter_flags & SOC_COUNTER_F_SWAP64) {
                        COMPILER_64_SET(ctr_new, ptr[0], ptr[1]);
                    } else {
                        COMPILER_64_SET(ctr_new, ptr[1], ptr[0]);
                    }
                    hwidx++;
                } else {
                    COMPILER_64_SET(ctr_new, 0, ptr[0]);
                }
                if (!COMPILER_64_IS_ZERO(ctr_new)) {
                    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                                (BSL_META_U(unit,
                                            "idx: %d, bufidx: %d, :val: %x_%x\n"),
                                 i, bufidx, COMPILER_64_HI(ctr_new),
                                 COMPILER_64_LO(ctr_new)));
                }
                ctr_prev = ctr_process->hwval[bufidx];
                vptr = &ctr_process->swval[bufidx];
                if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                    /* clear delta. (not maintaining delta for now) */
                } else {
                    ctr_diff = ctr_new;
                    if (COMPILER_64_LT(ctr_diff, ctr_prev)) { /* handle rollover */
                        uint64 wrap_amt;
                        f = 0;
                        width = SOC_REG_INFO(unit, ctr_reg).fields[f].len;
                        while((SOC_REG_INFO(unit, ctr_reg).fields + f) != NULL) {
                            if (SOC_REG_INFO(unit, ctr_reg).fields[f].field ==
                                COUNTf) {
                                width = SOC_REG_INFO(unit, ctr_reg).fields[f].len;
                                break;
                            }
                            f++;
                        }
                        if (width < 32) {
                            COMPILER_64_SET(wrap_amt, 0, 1UL << width);
                            COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                        } else if (width < 64) {
                            COMPILER_64_SET(wrap_amt, 1UL << (width - 32), 0);
                            COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                        }
                    }
                    COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                    ctr_process->hwval[bufidx] = ctr_new;
                    COMPILER_64_ADD_64(*vptr, ctr_diff);
                }
            }
        }
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Counter SBUSDMA failed: blk:%d, index: %d, entries: %d.\n"),
                   ctr_process->blk, ctr_process->bindex, ctr_process->entries));
        if (status == SOC_E_TIMEOUT) {
            /* delete this handles desc */
            (void)soc_sbusdma_desc_delete(unit, handle);
            _soc_blk_counter_handles[unit][ctr_process->bindex] = 0;
        }
    }
}
#endif

/* Function to Set all Sw maintained counter values to 0 */
void
soc_counter_ctr_reset_to_zero(int unit)
{
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);
    soc_control_t         *soc = SOC_CONTROL(unit);

    if (soc->counter_sw_val != NULL) {
        sal_memset(soc->counter_sw_val, 0,
                   soc_ctr_ctrl->total_dma_entries * sizeof(uint64));
    }
    if (soc->counter_hw_val != NULL) {
        sal_memset(soc->counter_hw_val, 0,
                   soc_ctr_ctrl->total_dma_entries * sizeof(uint64));
    }
    if (soc->counter_delta != NULL) {
        sal_memset(soc->counter_delta, 0,
                   soc_ctr_ctrl->total_dma_entries * sizeof(uint64));
    }
    return;
}

/*
 * Function:
 *      soc_counter_attach
 * Purpose:
 *      Initialize counter module.
 * Notes:
 *      Allocates counter collection buffers.
 *      We need to work with the true data for the chip, not the current
 *      pbmp_valid settings.  They may be changed after attach, but the
 *      memory won't be reallocated at that time.
 */

int
soc_counter_attach(int unit)
{
    soc_control_t         *soc;
    int                   n_entries, n_bytes;
    int                   non_dma_entries = 0;
    int                   portsize, nports, ports64;
    int                   blk, bindex, port, phy_port;
    int                   blktype, ctype, idx;
    uint32                array_idx;
    char                  *counter_level;
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    int                   ct;
#endif /* BCM_TOMAHAWK_SUPPORT || BCM_APACHE_SUPPORT*/
#if defined(BCM_TOMAHAWK2_SUPPORT)
    soc_info_t *si = &SOC_INFO(unit);
#endif
#if defined(BCM_PETRA_SUPPORT)
    int                   nof_entries_to_alloacte;
#endif /* BCM_PETRA_SUPPORT */

    assert(SOC_UNIT_VALID(unit));

    soc = SOC_CONTROL(unit);
        soc->ctr_evict_pid = SAL_THREAD_ERROR;
        soc->ctr_evict_interval = 0;
        soc->counter_pid = SAL_THREAD_ERROR;
        soc->counter_interval = 0;
        SOC_PBMP_CLEAR(soc->counter_pbmp);
        soc->counter_trigger = NULL;
        soc->counter_intr = NULL;
        /* Note that flags will be reinitialized in soc_counter_start */
        if (soc_feature(unit, soc_feature_stat_dma) && !SOC_IS_RCPU_ONLY(unit)) {
            soc->counter_flags = SOC_COUNTER_F_DMA;
        } else {
            soc->counter_flags = 0;
        }
        soc->counter_coll_prev = soc->counter_coll_cur = sal_time_usecs();

    /*For soc_feature_controlled_counters feture (use only the relevant)*/
    if (soc_feature(unit, soc_feature_controlled_counters)) {
            /*For not soc_feature_ignore_controlled_counter_priority feture (use only the relevant)*/
            if (!soc_feature(unit, soc_feature_ignore_controlled_counter_priority)) {
                counter_level = soc_property_get_str(unit, spn_SOC_COUNTER_CONTROL_LEVEL);
                if(NULL == counter_level) {
                    soc->soc_controlled_counter_flags |= _SOC_CONTROLLED_COUNTER_FLAG_HIGH;
                } else if (sal_strcmp(counter_level, "HIGH_LEVEL")) {
                    soc->soc_controlled_counter_flags |= _SOC_CONTROLLED_COUNTER_FLAG_HIGH;
                } else if (sal_strcmp(counter_level, "MEDIUM_LEVEL")) {
                    soc->soc_controlled_counter_flags |= (_SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MEDIUM);
                } else if (sal_strcmp(counter_level, "LOW_LEVEL")) {
                    soc->soc_controlled_counter_flags |= (_SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MEDIUM | _SOC_CONTROLLED_COUNTER_FLAG_LOW);
                } else { /*default*/
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "soc_counter_attach: unit %d:illegal counter level \n"), unit));
                    return SOC_E_FAIL;
                }
            }
            /*count the relevant counters (according to level)*/
            soc->soc_controlled_counter_num = 0;
            soc->soc_controlled_counter_all_num = 0;
            if (soc->controlled_counters) {
                for (array_idx = 0; soc->controlled_counters[array_idx].controlled_counter_f != NULL; array_idx++) {

#ifdef BCM_PETRA_SUPPORT
                if (SOC_IS_ARAD(unit)) {
                    /*do not collect NIF counters - the register are not cleared on read*/
                    if ( (soc->controlled_counters[array_idx].flags & _SOC_CONTROLLED_COUNTER_FLAG_NIF) || (soc->controlled_counters[array_idx].flags & _SOC_CONTROLLED_COUNTER_FLAG_ILKN) ) {
                        continue;
                    }
                }
#endif
                (soc->soc_controlled_counter_all_num)++;
                if (soc_feature(unit, soc_feature_ignore_controlled_counter_priority)) {
                    (soc->soc_controlled_counter_num)++;
                    soc->controlled_counters[array_idx].counter_idx = soc->soc_controlled_counter_all_num - 1;
                    LOG_DEBUG(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "counter_idx, %d. counter_id,%d.\n"),
                                            soc->controlled_counters[array_idx].counter_idx,
                                            soc->controlled_counters[array_idx].counter_id));
                } else {
                    if (soc->controlled_counters[array_idx].flags & soc->soc_controlled_counter_flags) {
                        (soc->soc_controlled_counter_num)++;
                        soc->controlled_counters[array_idx].counter_idx = soc->soc_controlled_counter_all_num - 1;
                    }
                }
            }
        }
    }
    portsize = 0;
    port = 0;
    /* We can't use pbmp_valid calculations, so we must do this manually. */
    for (phy_port = 0; ; phy_port++) {
        blk = SOC_PORT_BLOCK(unit, phy_port);
#if defined(BCM_MONTEREY_SUPPORT)
       if (SOC_IS_MONTEREY(unit) ) {
           if ((((phy_port > 0) && (phy_port < 13)) ||
                 ((phy_port > 52) && (phy_port < 65)))
               && !(IS_ROE_PORT(unit, port))) {
                blk = SOC_PORT_IDX_BLOCK(unit, phy_port ,1);
           }
      }
#endif
        bindex = SOC_PORT_BINDEX(unit, phy_port);
        if (blk < 0 && bindex < 0) {                    /* end of list */
            break;
        }
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            if (port < SOC_INFO(unit).port_p2l_mapping[phy_port]) {
                port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            }
        } else {
            port = phy_port;
        }
    }

    port++;

#if defined(BCM_TOMAHAWK2_SUPPORT)
    /* for flex port, should reserve enough counter resources */
    if (SOC_IS_TOMAHAWK2(unit)) {
        if (!SHR_BITNULL_RANGE(si->flexible_pm_bitmap, 1, _TH2_PBLKS_PER_DEV)) {
            int num_pipe = soc_property_get(unit, "num_pipe", 4);
            port = num_pipe * _TH2_DEV_PORTS_PER_PIPE;
        }
    }
#endif

    /* Don't count last port if CMIC */
    if (CMIC_PORT(unit) == (port - 1)) {
        /*
         * On XGS3 switch CMIC port stats are DMAable. Allocate space
         * in case these stats need to be DMAed as well.
         */
        if (!soc_feature(unit, soc_feature_cpuport_stat_dma)) {
            port--;
        }
    }

    nports = port;      /* 0..max_port_num */
    ports64 = 0;

    if (SOC_IS_HB_GW(unit)) {
        portsize = 2048;
        ports64 = nports;
    } else if (SOC_IS_APACHE(unit)) {
        portsize = 17 * 1024;
        ports64 = nports;
    } else if (SOC_IS_TRX(unit)) {
        portsize = 4096;
        ports64 = nports;
    } else if (SOC_IS_HERCULES(unit) || SOC_IS_XGS3_SWITCH(unit)) {
        portsize = 1024;
        ports64 = nports;
#if defined(BCM_PETRA_SUPPORT)
    } else if (SOC_IS_ARAD(unit)) {
        nof_entries_to_alloacte = FABRIC_LOGICAL_PORT_BASE(unit) + SOC_DPP_DEFS_GET(unit, first_fabric_link_id) + SOC_DPP_DEFS_GET(unit, nof_fabric_links);
        if (nof_entries_to_alloacte < nports){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Number of counters (%d) is larger than counters allowed in database (%d)!\n"),
                              nports, nof_entries_to_alloacte));
        }
        nports = nof_entries_to_alloacte;
        ports64 = nports;
        portsize = sizeof(uint64) * (soc->soc_controlled_counter_all_num + 160 /*nof non controlled counters*/);
#endif
#if defined(BCM_DNX_SUPPORT)
    } else if (SOC_IS_DNX(unit)) {
        nports = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
        ports64 = nports;
        portsize = sizeof(uint64)*(soc->soc_controlled_counter_all_num);
#endif
#if defined(BCM_DFE_SUPPORT)
    } else if (SOC_IS_DFE(unit)) {
        ports64 = nports;
        portsize = sizeof(uint64)*(soc->soc_controlled_counter_all_num);
#endif
#if defined(BCM_DNXF_SUPPORT)
    } else if (SOC_IS_DNXF(unit)) {
        ports64 = nports;
        portsize = sizeof(uint64)*(soc->soc_controlled_counter_all_num);
#endif
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_counter_attach: unit %d: unexpected chip type\n"),
                   unit));
        return SOC_E_FAIL;
    }

    soc->counter_perport = portsize / sizeof(uint64);
    soc->counter_n32 = 0;
    soc->counter_n64 = (portsize / sizeof(uint64)) * ports64;
    soc->counter_ports32 = 0;
    if(soc->counter_flags & SOC_COUNTER_F_DMA) {
        soc->counter_ports64 = ports64;
    }
    n_bytes = portsize * nports;

    n_entries = soc->counter_n32 + soc->counter_n64;
    soc->counter_portsize = portsize;
    soc->counter_bufsize = n_bytes;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_counter_attach: %d bytes/port, %d ports, %d ctrs/port, "
                            "%d ports with %d ctrs\n"),
                 portsize, nports, soc->counter_perport,
                 ports64, soc->counter_n64));

    /* Initialize SOC Counter Control Structure */
    if (SOC_CTR_CTRL(unit) != NULL) {
        sal_free(soc_ctr_ctrl);
        SOC_CTR_CTRL(unit) = NULL;
    }

    if (SOC_CTR_CTRL(unit) == NULL) {
        soc_ctr_ctrl = sal_alloc(sizeof(soc_counter_control_t),
                                 "cntr_ctrl_s");
        if (!soc_ctr_ctrl) {
            return SOC_E_MEMORY;
        }
    }
    sal_memset(soc_ctr_ctrl, 0, sizeof(soc_counter_control_t));
    SOC_CTR_CTRL(unit) = soc_ctr_ctrl;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        soc_ctr_ctrl->collect_non_stat_dma = soc_counter_collect_th_non_dma_entries;
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
    {
        soc_ctr_ctrl->collect_non_stat_dma = soc_counter_collect_non_dma_entries;
    }

    /* Initialize Non-DMA counters */

    if (soc->counter_non_dma == NULL) {
        soc->counter_non_dma = sal_alloc((SOC_COUNTER_NON_DMA_END -
                                         SOC_COUNTER_NON_DMA_START) *
                                         sizeof(soc_counter_non_dma_t),
                                         "cntr_non_dma");
        if (soc->counter_non_dma == NULL) {
            goto error;
        }
    } else {
        for (idx = 0; idx < SOC_COUNTER_NON_DMA_END - SOC_COUNTER_NON_DMA_START;
             idx++) {
            if (soc->counter_non_dma[idx].flags & _SOC_COUNTER_NON_DMA_ALLOC) {
                soc_cm_sfree(unit, soc->counter_non_dma[idx].dma_buf[0]);
            }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
            if((soc->counter_non_dma[idx].extra_ctrs != NULL) &&
               ((soc->counter_non_dma[idx].flags & _SOC_COUNTER_NON_DMA_DO_DMA) ||
                (soc->counter_non_dma[idx].flags & _SOC_COUNTER_NON_DMA_OBM))){
                uint32 ct2, ctr_offset;
                uint32 fld_ct = 2; /* TH/TH2 has default fld_ct = 2 */
#ifdef BCM_TOMAHAWK3_SUPPORT
                if(SOC_IS_TOMAHAWK3(unit)) {
                    fld_ct = soc->counter_non_dma[idx].extra_ctr_fld_ct;
                }
#endif /* BCM_TOMAHAWK3_SUPPORT */
                for(ct = 0; ct < soc->counter_non_dma[idx].extra_ctr_ct; ct++) {
                    for(ct2 = 0; ct2 < fld_ct; ct2++) {
                        ctr_offset = ct * fld_ct + ct2;
                        if((soc->counter_non_dma[idx].extra_ctrs[ctr_offset].cname != NULL) &&
                           (soc->counter_non_dma[idx].extra_ctrs[ctr_offset].cname[0] == '*')){
                            sal_free(soc->counter_non_dma[idx].extra_ctrs[ctr_offset].cname);
                            soc->counter_non_dma[idx].extra_ctrs[ctr_offset].cname = NULL;
                    }
                    }
                }

                sal_free(soc->counter_non_dma[idx].extra_ctrs);
                soc->counter_non_dma[idx].extra_ctrs = NULL;
                if((soc->counter_non_dma[idx].flags & _SOC_COUNTER_NON_DMA_OBM) &&
                   (soc->counter_non_dma[idx].id != SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS)) {
                    soc->counter_non_dma[idx+1].extra_ctrs = NULL;
                }
            }
#endif /* BCM_TOMAHAWK_SUPPORT */
        }
    }
    sal_memset(soc->counter_non_dma, 0,
               (SOC_COUNTER_NON_DMA_END - SOC_COUNTER_NON_DMA_START) *
               sizeof(soc_counter_non_dma_t));

    /* Controlled counters */
    if (!soc_feature(unit, soc_feature_controlled_counters)) {
        soc->controlled_counters = NULL;
    }

    /* Initialize the number of cosq counters per port */
    _soc_counter_num_cosq_init(unit, nports);

#ifdef BCM_TRIUMPH_SUPPORT
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        if (SOC_FAILURE(_soc_counter_triumph3_non_dma_init(unit,
                            nports, n_entries, &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            if (SOC_FAILURE(soc_counter_tomahawk3_non_dma_init
                            (unit, nports, n_entries, &non_dma_entries))) {
                goto error;
            }
        } else
#endif
        if (SOC_FAILURE(soc_counter_tomahawk_non_dma_init
                        (unit, nports, n_entries, &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
        if (SOC_FAILURE(soc_counter_trident3_non_dma_init
                        (unit, nports, n_entries, &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        if (SOC_FAILURE(soc_counter_hurricane4_non_dma_init
                        (unit, nports, n_entries, &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        if (SOC_FAILURE(soc_counter_helix5_non_dma_init
                        (unit, nports, n_entries, &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_HELIX5_SUPPORT */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        if (SOC_FAILURE(soc_counter_firebolt6_non_dma_init
                        (unit, nports, n_entries, &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_FIREBOLT6_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        if (SOC_FAILURE(_soc_counter_apache_non_dma_init(unit,
                        nports, n_entries, &non_dma_entries))) {
            goto error;
        }
#if defined(BCM_MONTEREY_SUPPORT) && defined (CPRIMOD_SUPPORT)
        /* Add CPRI counters if supported */
        if (SOC_IS_MONTEREY(unit)) {
            int cpri_non_dma_entries = 0;
            if (SOC_FAILURE(_soc_counter_monterey_non_dma_cpri_init(unit,
                                        nports, n_entries + non_dma_entries,
                                        &cpri_non_dma_entries))) {
                goto error;
            }
            non_dma_entries += cpri_non_dma_entries;
        }
#endif /* BCM_MONTEREY_SUPPORT && CPRIMOD_SUPPORT */
    } else
#endif /* BCM_APACHE_SUPPORT */
#if defined ( BCM_TRIDENT2_SUPPORT ) || defined ( BCM_TRIDENT2PLUS_SUPPORT )
    if (SOC_IS_TD2_TT2(unit)) {
        if (SOC_FAILURE(_soc_counter_trident2_non_dma_init(unit,
                            nports, n_entries, &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        if (SOC_FAILURE(_soc_counter_trident_non_dma_init(unit, nports, n_entries,
                        &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_HURRICANE_SUPPORT
    if (SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {
        if (SOC_FAILURE(_soc_counter_hu_non_dma_init(unit, nports, n_entries,
                        &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_HURRICANE_SUPPORT */
#if defined(BCM_GREYHOUND_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
        if (SOC_FAILURE(_soc_counter_gh_non_dma_init(unit, nports, n_entries,
                        &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_GREYHOUND_SUPPORT || BCM_HURRICANE3_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        if (SOC_FAILURE(_soc_counter_gh2_non_dma_init(unit, nports, n_entries,
                                                      &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
 /*we may not need separate saber2_non_dma() function */
    if (SOC_IS_KATANA2(unit)) {
        if (SOC_FAILURE(_soc_counter_saber2_non_dma_init(unit, nports, n_entries,
                        &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        if (SOC_FAILURE(_soc_counter_katana_non_dma_init(unit, nports, n_entries,
                        &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_KATANA_SUPPORT */
    if (SOC_IS_TR_VL(unit)) {
        if (SOC_FAILURE(_soc_counter_tr_non_dma_init(unit, nports, n_entries,
                        &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit) && (!(SOC_IS_SHADOW(unit)))) {
        if (SOC_FAILURE(_soc_counter_sc_non_dma_init(unit, nports, n_entries,
                        &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_SCORPION_SUPPORT */
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        if (!SOC_PBMP_IS_NULL(PBMP_IL_ALL(unit))) {
            if (SOC_FAILURE(_soc_counter_shadow_non_dma_init(unit, nports,
                            n_entries, &non_dma_entries))) {
                goto error;
            }
        }
    } else
#endif /* BCM_SHADOW_SUPPORT */
#ifdef BCM_BRADLEY_SUPPORT
    if (SOC_IS_HB_GW(unit)) {
        if (SOC_FAILURE(_soc_counter_hb_non_dma_init(unit, nports, n_entries,
                        &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_BRADLEY_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FB_FX_HX(unit)) {
        if (SOC_FAILURE(_soc_counter_fb_non_dma_init(unit, nports, n_entries,
                        &non_dma_entries))) {
            goto error;
        }
    } else
#endif /* BCM_FIREBOLT_SUPPORT */
    {
        non_dma_entries = 0;
    }

    soc->counter_n64_non_dma = non_dma_entries;

    /*
     * Counter DMA buffer allocation (32 and 64 bit allocated together)
     */

    assert(n_bytes > 0);

    /* Hardware DMA buf */
    if(soc->counter_flags & SOC_COUNTER_F_DMA) {
        if (soc->counter_buf32 == NULL) {
            soc->counter_buf32 = soc_cm_salloc(unit, n_bytes, "cntr_dma_buf");
            if (soc->counter_buf32 == NULL) {
                goto error;
            }
            soc->counter_buf64 = (uint64 *)&soc->counter_buf32[soc->counter_n32];
        }
        sal_memset(soc->counter_buf32, 0, n_bytes);
    }

    n_entries += non_dma_entries;

    /* Hardware value buf */
    if (soc->counter_hw_val != NULL) {
        sal_free(soc->counter_hw_val);
        soc->counter_hw_val = NULL;
    }
    if (soc->counter_hw_val == NULL) {
        soc->counter_hw_val = sal_alloc(n_entries * sizeof(uint64),
                                        "cntr_hw_val");
        if (soc->counter_hw_val == NULL) {
            goto error;
        }
    }
    sal_memset(soc->counter_hw_val, 0, n_entries * sizeof(uint64));

    /* Software value buf */
    if (soc->counter_sw_val != NULL) {
        sal_free(soc->counter_sw_val);
        soc->counter_sw_val = NULL;
    }
    if (soc->counter_sw_val == NULL) {
        soc->counter_sw_val = sal_alloc(n_entries * sizeof(uint64),
                                        "cntr_sw_val");
        if (soc->counter_sw_val == NULL) {
            goto error;
        }
    }
    sal_memset(soc->counter_sw_val, 0, n_entries * sizeof(uint64));

    /* Delta buf */
    if (soc->counter_delta != NULL) {
        sal_free(soc->counter_delta);
        soc->counter_delta = NULL;
    }
    if (soc->counter_delta == NULL) {
        soc->counter_delta = sal_alloc(n_entries * sizeof(uint64),
                                       "cntr_delta");
        if (soc->counter_delta == NULL) {
            goto error;
        }
    }
    sal_memset(soc->counter_delta, 0, n_entries * sizeof(uint64));
    soc_ctr_ctrl->total_dma_entries = n_entries;

#ifdef BCM_SBUSDMA_SUPPORT
    _soc_counter_pending[unit] = 0;
#endif

    /* Set up and install counter maps */

    /* We can't use pbmp_valid calculations, so we must do this manually. */
    for (phy_port = 0; ; phy_port++) {
#if defined(BCM_DNX_SUPPORT)
        if (SOC_IS_DNX(unit)) {
            break;            /*skip due to dnx using controlled counter */
        }
#endif
        blk = SOC_PORT_BLOCK(unit, phy_port);
        bindex = SOC_PORT_BINDEX(unit, phy_port);
        if (blk < 0 && bindex < 0) {                    /* end of list */
            break;
        }
        if (blk < 0) {                                  /* empty slot */
            continue;
        }
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port < 0) {
                continue;
            }
        } else {
            port = phy_port;
            if (port >= nports) {
                continue;
            }
        }
#if defined(BCM_MONTEREY_SUPPORT)
        if (!(IS_ROE_PORT(unit, port)) && SOC_IS_MONTEREY(unit)) {
            if (((phy_port > 0) && (phy_port < 13)) ||
                 ((phy_port > 52) && (phy_port < 65))) {
                blk = SOC_PORT_IDX_BLOCK(unit, phy_port,1);
                bindex = SOC_PORT_IDX_BINDEX(unit, phy_port,1);
                if (blk < 0 && bindex < 0) {                    /* end of list */
                    break;
                }
                if (blk < 0) {                                  /* empty slot */
                    continue;
                }
            }
        }
#endif
        blktype = SOC_BLOCK_INFO(unit, blk).type;
        switch (blktype) {
        case SOC_BLK_EPIC:
            ctype = SOC_CTR_TYPE_FE;
            break;
        case SOC_BLK_GPIC:
        case SOC_BLK_GPORT:
        case SOC_BLK_QGPORT:
        case SOC_BLK_SPORT:
            ctype = SOC_CTR_TYPE_GE;
            break;
        case SOC_BLK_XQPORT:
        case SOC_BLK_XGPORT:
            if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
                ctype = SOC_CTR_TYPE_XE;
            } else {
                ctype = SOC_CTR_TYPE_GE;
            }
            break;
        case SOC_BLK_IPIC:
        case SOC_BLK_HPIC:
            ctype = SOC_CTR_TYPE_HG;
            break;
        case SOC_BLK_XPIC:
        case SOC_BLK_XPORT:
        case SOC_BLK_GXPORT:
        case SOC_BLK_GXFPPORT:
        case SOC_BLK_XLPORT:
        case SOC_BLK_XLPORTB0:
        case SOC_BLK_XTPORT:
        case SOC_BLK_CLPORT:
        case SOC_BLK_MXQPORT:
        case SOC_BLK_XWPORT:
        case SOC_BLK_CDPORT:
            if (SOC_IS_GREYHOUND(unit) && blktype == SOC_BLK_GXPORT){
                if (SOC_BLOCK_PORT(unit, blk) < 0){
                    ctype = SOC_CTR_TYPE_XE;
                } else {
                    /* Greyhound GE port with SOC_BLK_GXPORT case*/
                    ctype = SOC_CTR_TYPE_GE;
                }
            } else {
                ctype = SOC_CTR_TYPE_XE;
            }

#ifdef BCM_SABER2_SUPPORT
            if (SOC_IS_SABER2(unit)) {
                if (blktype == SOC_BLK_MXQPORT) {
                    ctype = SOC_CTR_TYPE_GE;
                    break;
                }
                if (blktype == SOC_BLK_XLPORT) {
                    ctype = SOC_CTR_TYPE_XE;
                    break;
                }
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit, " UNKNOWN TYPE \n")));
            }
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
            if (SOC_IS_GREYHOUND2(unit) && blktype == SOC_BLK_CLPORT) {
                ctype = SOC_CTR_TYPE_CE;
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
                if (IS_EGPHY_PORT(unit, port) || IS_QSGMII_PORT(unit, port)) {
                    ctype = SOC_CTR_TYPE_GE;
                } else {
                    ctype = SOC_CTR_TYPE_XE;
                }
                if (blktype == SOC_BLK_CLPORT) {
                    ctype = SOC_CTR_TYPE_CE;
                }
                break;
            }
#endif
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWKPLUS_SUPPORT) || defined (BCM_TOMAHAWK2_SUPPORT) || defined (BCM_TOMAHAWK3_SUPPORT)
            if ((SOC_IS_TRIDENT3X(unit)) || (SOC_IS_TOMAHAWKPLUS(unit)) ||
                (SOC_IS_TOMAHAWK2(unit)) || (SOC_IS_TOMAHAWK3(unit))) {
                if (blktype == SOC_BLK_CLPORT) {
                    ctype = SOC_CTR_TYPE_CE;
                    break;
                }
                if (blktype == SOC_BLK_CDPORT) {
                    ctype = SOC_CTR_TYPE_CD;
                    break;
                }
                if (blktype == SOC_BLK_XLPORT) {
                    ctype = SOC_CTR_TYPE_XE;
                    break;
                }
                LOG_WARN(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "UNKNOWN TYPE \n")));
            }
#endif
            break;
		case SOC_BLK_CLG2PORT:
			ctype =	SOC_CTR_TYPE_CE;
			break;
        case SOC_BLK_CLP:
        case SOC_BLK_XLP:
            if (IS_IL_PORT(unit, port)) {
                continue;
            } else if (IS_CE_PORT(unit, port)) {
                ctype = SOC_CTR_TYPE_CE;
            } else if (IS_QSGMII_PORT(unit,port)) {
                ctype = SOC_CTR_TYPE_GE;
            } else {
                ctype = SOC_CTR_TYPE_XE;
            }
            break;
        case SOC_BLK_MXQ:
            ctype = SOC_CTR_TYPE_GE;
            break;
        case SOC_BLK_CMIC:
#if defined(BCM_MONTEREY_SUPPORT)
        case SOC_BLK_CPRI:
#endif
            if (soc_feature(unit, soc_feature_cpuport_stat_dma)) {
                ctype = SOC_CTR_TYPE_CPU;
                break;
            }
        case SOC_BLK_CPIC:
        default:
            continue;
        }

        if (!SOC_CONTROL(unit)->counter_map[port]) {
            SOC_CONTROL(unit)->counter_map[port] =
                &SOC_CTR_DMA_MAP(unit, ctype);
            assert(SOC_CONTROL(unit)->counter_map[port]);
        }

#ifdef BCM_WARM_BOOT_SUPPORT
        /* Flexport operation can change port mappings.
         * counter_map should be reassigned based on recovered port info
         * from scache, especially for devices using multiple counter maps of
         * different sizes
         */
        if (SOC_WARM_BOOT(unit)) {
            SOC_CONTROL(unit)->counter_map[port] =
                &SOC_CTR_DMA_MAP(unit, ctype);
            assert(SOC_CONTROL(unit)->counter_map[port]);
        }
#endif
        assert(SOC_CONTROL(unit)->counter_map[port]->cmap_base);

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        /* We need this done in a port loop like this one, so we're
         * piggybacking on it. */

        if (soc_feature(unit, soc_feature_timestamp_counter) &&
            SOC_CONTROL(unit)->sw_timestamp_fifo_enable) {
            /* A HW register implemented as a FIFO-pop on read is in the
             * counter collection space.  We must copy the DMA'd values
             * into a SW FIFO and make them available to the application
             * on request.
             */
            if (soc->counter_timestamp_fifo[port] == NULL) {
                soc->counter_timestamp_fifo[port] =
                    sal_alloc(sizeof(shr_fifo_t), "Timestamp counter FIFO");
                if (soc->counter_timestamp_fifo[port] == NULL) {
                    goto error;
                }
            } else {
                SHR_FIFO_FREE(soc->counter_timestamp_fifo[port]);
            }
            sal_memset(soc->counter_timestamp_fifo[port], 0,
                       sizeof(shr_fifo_t));
            SHR_FIFO_ALLOC(soc->counter_timestamp_fifo[port],
                           COUNTER_TIMESTAMP_FIFO_SIZE, sizeof(uint32),
                           SHR_FIFO_FLAG_DO_NOT_PUSH_DUPLICATE);
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
    }
#ifdef BCM_SBUSDMA_SUPPORT
    /* Init blk counters */
    if (soc->blk_ctr_desc_count) {
        uint16 ctr;
        /* Calc size */
        n_entries = 0;
        for (idx = 0; idx < soc->blk_ctr_desc_count; idx++) {
            ctr = 0;
            while (soc->blk_ctr_desc[idx].desc[ctr].reg != INVALIDr) {
                n_entries += soc->blk_ctr_desc[idx].desc[ctr].entries;
                ctr++;
            }
        }
        soc->blk_ctr_count = n_entries;
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "Total ctr blks: %d, Total entries: %d\n"),
                  soc->blk_ctr_desc_count, n_entries));
        if (_soc_blk_counter_handles[unit] == NULL) {
            _soc_blk_counter_handles[unit] = sal_alloc(n_entries * sizeof(sbusdma_desc_handle_t),
                                                 "blk_ctr_desc_handles");
            if (_soc_blk_counter_handles[unit] == NULL) {
                goto error;
            }
        }
        sal_memset(_soc_blk_counter_handles[unit], 0, n_entries * sizeof(sbusdma_desc_handle_t));

        if (soc->blk_ctr_buf == NULL) {
            soc->blk_ctr_buf = soc_cm_salloc(unit, n_entries * sizeof(uint64),
                                             "blk_ctr_hw_buff");
            if (soc->blk_ctr_buf == NULL) {
                goto error;
            }
        }
        sal_memset(soc->blk_ctr_buf, 0, n_entries * sizeof(uint64));

        if (soc->blk_counter_hw_val == NULL) {
            soc->blk_counter_hw_val = sal_alloc(n_entries * sizeof(uint64),
                                                "blk_ctr_hw_vals");
            if (soc->blk_counter_hw_val == NULL) {
                goto error;
            }
        }
        sal_memset(soc->blk_counter_hw_val, 0, n_entries * sizeof(uint64));

        if (soc->blk_counter_sw_val == NULL) {
            soc->blk_counter_sw_val = sal_alloc(n_entries * sizeof(uint64),
                                                "blk_ctr_sw_vals");
            if (soc->blk_counter_sw_val == NULL) {
                goto error;
            }
        }
        sal_memset(soc->blk_counter_sw_val, 0, n_entries * sizeof(uint64));
    }
#endif
    return SOC_E_NONE;

 error:

#ifdef BCM_SBUSDMA_SUPPORT
    if (soc->blk_counter_sw_val != NULL) {
        sal_free(soc->blk_counter_sw_val);
        soc->blk_counter_sw_val = NULL;
    }

    if (soc->blk_counter_hw_val != NULL) {
        sal_free(soc->blk_counter_hw_val);
        soc->blk_counter_hw_val = NULL;
    }

    if (soc->blk_ctr_buf != NULL) {
        soc_cm_sfree(unit, soc->blk_ctr_buf);
        soc->blk_ctr_buf = NULL;
    }

    if (_soc_blk_counter_handles[unit] != NULL) {
        sal_free(_soc_blk_counter_handles[unit]);
        _soc_blk_counter_handles[unit] = NULL;
    }
#endif

#ifdef BCM_TRIUMPH2_SUPPORT
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (soc->counter_timestamp_fifo[port] != NULL) {
            SHR_FIFO_FREE(soc->counter_timestamp_fifo[port]);
            sal_free(soc->counter_timestamp_fifo[port]);
            soc->counter_timestamp_fifo[port] = NULL;
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    if (soc->counter_evict != NULL) {
        sal_free(soc->counter_evict);
        soc->counter_evict = NULL;
    }

    if (soc->counter_non_dma != NULL) {
        for (idx = 0;
             idx < SOC_COUNTER_NON_DMA_END - SOC_COUNTER_NON_DMA_START;
             idx++) {
            if (soc->counter_non_dma[idx].flags & _SOC_COUNTER_NON_DMA_ALLOC) {
                soc_cm_sfree(unit, soc->counter_non_dma[idx].dma_buf[0]);
            }
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
            if((soc->counter_non_dma[idx].extra_ctrs != NULL) &&
               ((soc->counter_non_dma[idx].flags & _SOC_COUNTER_NON_DMA_DO_DMA) ||
                (soc->counter_non_dma[idx].flags & _SOC_COUNTER_NON_DMA_OBM))){
                for(ct = 0; ct < soc->counter_non_dma[idx].extra_ctr_ct; ct++) {
                    if((soc->counter_non_dma[idx].extra_ctrs[ct*2].cname != NULL) &&
                       (soc->counter_non_dma[idx].extra_ctrs[ct*2].cname[0] == '*')){
                        sal_free(soc->counter_non_dma[idx].extra_ctrs[ct*2].cname);
                        soc->counter_non_dma[idx].extra_ctrs[ct*2].cname = NULL;
                    }

                    if((soc->counter_non_dma[idx].extra_ctrs[ct*2+1].cname != NULL) &&
                       (soc->counter_non_dma[idx].extra_ctrs[ct*2+1].cname[0] == '*')){
                        sal_free(soc->counter_non_dma[idx].extra_ctrs[ct*2+1].cname);
                        soc->counter_non_dma[idx].extra_ctrs[ct*2+1].cname = NULL;
                    }
                }

                sal_free(soc->counter_non_dma[idx].extra_ctrs);
                soc->counter_non_dma[idx].extra_ctrs = NULL;
                if((soc->counter_non_dma[idx].flags & _SOC_COUNTER_NON_DMA_OBM) &&
                   (soc->counter_non_dma[idx].id != SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS)) {
                    soc->counter_non_dma[idx+1].extra_ctrs = NULL;
                }
            }
#endif /* BCM_TOMAHAWK_SUPPORT */
        }
        sal_free(soc->counter_non_dma);
        soc->counter_non_dma = NULL;
    }

    if (soc->counter_buf32 != NULL) {
        soc_cm_sfree(unit, soc->counter_buf32);
        soc->counter_buf32 = NULL;
        soc->counter_buf64 = NULL;
    }

    if (soc->counter_hw_val != NULL) {
        sal_free(soc->counter_hw_val);
        soc->counter_hw_val = NULL;
    }

    if (soc->counter_sw_val != NULL) {
        sal_free(soc->counter_sw_val);
        soc->counter_sw_val = NULL;
    }

    if (soc->counter_delta != NULL) {
        sal_free(soc->counter_delta);
        soc->counter_delta = NULL;
    }

    return SOC_E_MEMORY;
}

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
int
_soc_counter_trident2p_non_dma_pbmp_update(int unit)
{
    int id, i;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *non_dma;
    int soc_counter_non_dma1[] = {
        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, /* MC_PERQ_PKT   */
        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, /* MC_PERQ_BYTE  */
        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, /* MCQ_DROP_PKT  */
        SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, /* MCQ_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING, /* DROP_PKT_ING  */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING, /* DROP_BYTE_ING */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING_METER, /* DROP_PKT_IMTR */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING_METER, /* DROP_BYTE_IMTR */
        SOC_COUNTER_NON_DMA_DROP_RQ_PKT, /* RQ_DROP_PKT */
        SOC_COUNTER_NON_DMA_DROP_RQ_BYTE, /* RQ_DROP_BYTE */
        SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW, /* RQ_DROP_PKT_YEL */
        SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED, /* RQ_DROP_PKT_RED */
        SOC_COUNTER_NON_DMA_PG_MIN_PEAK, /* PG_MIN_PEAK */
        SOC_COUNTER_NON_DMA_PG_MIN_CURRENT, /* PG_MIN_CUR */
        SOC_COUNTER_NON_DMA_PG_SHARED_PEAK, /* PG_SHARED_PEAK */
        SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT, /* PG_SHARED_CUR */
        SOC_COUNTER_NON_DMA_PG_HDRM_PEAK, /* PG_HDRM_PEAK */
        SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT, /* PG_HDRM_CUR */
        SOC_COUNTER_NON_DMA_QUEUE_PEAK, /* QUEUE_PEAK */
        SOC_COUNTER_NON_DMA_QUEUE_CURRENT /* QUEUE_CUR */
    };

    int soc_counter_non_dma2[] = {
        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC, /* UC_PERQ_PKT */
        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC, /* UC_PERQ_BYTE */
        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, /* UCQ_DROP_PKT */
        SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC, /* UCQ_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, /* DROP_PKT_YEL */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED, /* DROP_PKT_RED */
        SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN, /* WRED_PKT_GRE */
        SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW, /* WRED_PKT_YEL */
        SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED, /* WRED_PKT_RED */
        SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK, /* UC_QUEUE_PEAK */
        SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT, /* UC_QUEUE_CUR */
        SOC_COUNTER_NON_DMA_MMU_QCN_CNM, /* QCN_CNM_COUNTER */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_HIGH_PRI, /* OBM0_HIGH_PRI_DROP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM0_LOW_PRI, /* OBM0_LOW_PRI_DROP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_HIGH_PRI, /* OBM0_HIGH_PRI_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM0_LOW_PRI, /* OBM0_LOW_PRI_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_HIGH_PRI, /* OBM1_HIGH_PRI_DROP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM1_LOW_PRI, /* OBM1_LOW_PRI_DROP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_HIGH_PRI, /* OBM1_HIGH_PRI_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM1_LOW_PRI, /* OBM1_LOW_PRI_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_HIGH_PRI, /* OBM2_HIGH_PRI_DROP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM2_LOW_PRI, /* OBM2_LOW_PRI_DROP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_HIGH_PRI, /* OBM2_HIGH_PRI_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM2_LOW_PRI, /* OBM2_LOW_PRI_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_HIGH_PRI, /* OBM3_HIGH_PRI_DROP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM3_LOW_PRI, /* OBM3_LOW_PRI_DROP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_HIGH_PRI, /* OBM3_HIGH_PRI_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM3_LOW_PRI /* OBM3_LOW_PRI_DROP_BYTE */
    };

    for (i = 0; i < sizeof(soc_counter_non_dma1)/sizeof(int); ++i) {
        id = soc_counter_non_dma1[i];
        non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];
        non_dma->pbmp = PBMP_ALL(unit);
    }

    for (i = 0; i < sizeof(soc_counter_non_dma2)/sizeof(int); ++i) {
        id = soc_counter_non_dma2[i];
        non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];
        non_dma->pbmp = PBMP_PORT_ALL(unit);
    }

    return SOC_E_NONE;
}
#endif /* defined(BCM_TRIDENT2PLUS_SUPPORT) */

#if defined (BCM_APACHE_SUPPORT)
int
_soc_counter_apache_non_dma_pbmp_update(int unit)
{
    int id, i;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *non_dma;
    int soc_counter_non_dma1[] = {
        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, /* MC_PERQ_PKT   */
        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, /* MC_PERQ_BYTE  */
        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, /* MCQ_DROP_PKT  */
        SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, /* MCQ_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING, /* DROP_PKT_ING  */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING, /* DROP_BYTE_ING */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING_METER, /* DROP_PKT_IMTR */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING_METER, /* DROP_BYTE_IMTR */
        SOC_COUNTER_NON_DMA_PG_MIN_PEAK, /* PG_MIN_PEAK */
        SOC_COUNTER_NON_DMA_PG_MIN_CURRENT, /* PG_MIN_CUR */
        SOC_COUNTER_NON_DMA_PG_SHARED_PEAK, /* PG_SHARED_PEAK */
        SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT, /* PG_SHARED_CUR */
        SOC_COUNTER_NON_DMA_PG_HDRM_PEAK, /* PG_HDRM_PEAK */
        SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT, /* PG_HDRM_CUR */
        SOC_COUNTER_NON_DMA_QUEUE_PEAK, /* QUEUE_PEAK */
        SOC_COUNTER_NON_DMA_QUEUE_CURRENT /* QUEUE_CUR */
    };

    int soc_counter_non_dma2[] = {
        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC, /* UC_PERQ_PKT */
        SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC, /* UC_PERQ_BYTE */
        SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, /* UCQ_DROP_PKT */
        SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC, /* UCQ_DROP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, /* DROP_PKT_YEL */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED, /* DROP_PKT_RED */
        SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN, /* WRED_PKT_GRE */
        SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW, /* WRED_PKT_YEL */
        SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED, /* WRED_PKT_RED */
        SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK, /* UC_QUEUE_PEAK */
        SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT, /* UC_QUEUE_CUR */
        SOC_COUNTER_NON_DMA_MMU_QCN_CNM, /* QCN_CNM_COUNTER */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO, /* OBM_LOSSY_LO_DRP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO, /* OBM_LOSSY_LO_DRP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI, /* OBM_LOSSY_HI_DRP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI, /* OBM_LOSSY_HI_DRP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0, /* OBM_LOSSLESS0_DRP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0, /* OBM_LOSSLESS0_DRP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1, /* OBM_LOSSLESS1_DRP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1, /* OBM_LOSSLESS1_DRP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_EXPRESS, /* OBM_EXPRESS_DRP_PKT */
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_EXPRESS, /* OBM_EXPRESS_DRP_BYTE */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_LO, /* OBM_LOSSY_LO_ENQ_PKT */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_LO, /* OBM_LOSSY_LO_ENQ_BYTE */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_HI, /* OBM_LOSSY_HI_ENQ_PKT */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_HI, /* OBM_LOSSY_HI_ENQ_BYTE */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS0, /* OBM_LOSSLESS0_ENQ_PKT */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS0, /* OBM_LOSSLESS0_ENQ_BYTE */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS1, /* OBM_LOSSLESS1_ENQ_PKT */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS1, /* OBM_LOSSLESS0_ENQ_BYTE */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_EXPRESS, /* OBM_EXPRESS_ENQ_PKT */
        SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_EXPRESS /* OBM_EXPRESS_ENQ_BYTE */
    };

    for (i = 0; i < sizeof(soc_counter_non_dma1)/sizeof(int); ++i) {
        id = soc_counter_non_dma1[i];
        non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];
        non_dma->pbmp = PBMP_ALL(unit);
    }

    for (i = 0; i < sizeof(soc_counter_non_dma2)/sizeof(int); ++i) {
        id = soc_counter_non_dma2[i];
        non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];
        non_dma->pbmp = PBMP_PORT_ALL(unit);
    }

    return SOC_E_NONE;
}
#endif /* BCM_APACHE_SUPPORT */

/*
 * Function:
 *      soc_counter_non_dma_pbmp_update
 * Purpose:
 *      Update non_dma->pbmp
 * Notes:
 */
int
soc_counter_non_dma_pbmp_update(int unit)
{
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        SOC_IF_ERROR_RETURN(_soc_counter_apache_non_dma_pbmp_update(unit));
    }
#endif /* BCM_APACHE_SUPPORT */
#if defined ( BCM_TRIDENT2PLUS_SUPPORT )
    if (SOC_IS_TD2P_TT2P(unit)) {
        SOC_IF_ERROR_RETURN(_soc_counter_trident2p_non_dma_pbmp_update(unit));
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_port_cmap_get/set
 * Purpose:
 *      Access the counter map structure per port
 */

soc_cmap_t *
soc_port_cmap_get(int unit, soc_port_t port)
{
    if (!SOC_UNIT_VALID(unit)) {
        return NULL;
    }
    if (!SOC_IS_KATANA2(unit)) {
        if (!SOC_PORT_VALID(unit, port) ||
            (!IS_PORT(unit, port) &&
             (!soc_feature(unit, soc_feature_cpuport_stat_dma)))) {
            return NULL;
        }
    }
    return SOC_CONTROL(unit)->counter_map[port];
}

int
soc_port_cmap_set(int unit, soc_port_t port, soc_ctr_type_t ctype)
{
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }
    if (!SOC_IS_KATANA2(unit)) {
        if (!SOC_PORT_VALID(unit, port) ||
            (!IS_PORT(unit, port) &&
             (!soc_feature(unit, soc_feature_cpuport_stat_dma)))) {
            return SOC_E_PARAM;
        }
    }
    SOC_CONTROL(unit)->counter_map[port] = &SOC_CTR_DMA_MAP(unit, ctype);
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_counter_detach
 * Purpose:
 *      Finalize counter module.
 * Notes:
 *      Stops counter task if running.
 *      Deallocates counter collection buffers.
 */

int
soc_counter_detach(int unit)
{
    soc_control_t       *soc;
    int                 i;
#ifdef BCM_TRIUMPH2_SUPPORT
    soc_port_t          port;
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    int                   ct;
#endif /* BCM_TOMAHAWK_SUPPORT */
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);

    assert(SOC_UNIT_VALID(unit));

    /*
     * COVERITY
     *
     * assert validates the input
     */
    /* coverity[overrun-local : FALSE] */
    soc = SOC_CONTROL(unit);

    /*
     * COVERITY
     *
     * assert validates the input
     */
    /* coverity[overrun-local : FALSE] */
    SOC_IF_ERROR_RETURN(soc_counter_stop(unit));

#ifdef BCM_TRIUMPH2_SUPPORT
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (soc->counter_timestamp_fifo[port] != NULL) {
            SHR_FIFO_FREE(soc->counter_timestamp_fifo[port]);
            sal_free(soc->counter_timestamp_fifo[port]);
            soc->counter_timestamp_fifo[port] = NULL;
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    if (soc->counter_evict != NULL) {
        sal_free(soc->counter_evict);
        soc->counter_evict = NULL;
    }

    if (soc_ctr_ctrl != NULL) {
        sal_free(soc_ctr_ctrl);
        SOC_CTR_CTRL(unit) = NULL;
    }

    if (soc->counter_non_dma != NULL) {
        for (i = 0; i < SOC_COUNTER_NON_DMA_END - SOC_COUNTER_NON_DMA_START;
             i++) {
            if (soc->counter_non_dma[i].flags & _SOC_COUNTER_NON_DMA_ALLOC) {
                soc_cm_sfree(unit, soc->counter_non_dma[i].dma_buf[0]);
            }

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_APACHE_SUPPORT)
            if((soc->counter_non_dma[i].extra_ctrs != NULL) &&
               ((soc->counter_non_dma[i].flags & _SOC_COUNTER_NON_DMA_DO_DMA) ||
                (soc->counter_non_dma[i].flags & _SOC_COUNTER_NON_DMA_OBM))){
                for(ct = 0; ct < soc->counter_non_dma[i].extra_ctr_ct; ct++) {
                    if((soc->counter_non_dma[i].extra_ctrs[ct*2].cname != NULL) &&
                       (soc->counter_non_dma[i].extra_ctrs[ct*2].cname[0] == '*')){
                        sal_free(soc->counter_non_dma[i].extra_ctrs[ct*2].cname);
                        soc->counter_non_dma[i].extra_ctrs[ct*2].cname = NULL;
                    }

                    if((soc->counter_non_dma[i].extra_ctrs[ct*2+1].cname != NULL) &&
                       (soc->counter_non_dma[i].extra_ctrs[ct*2+1].cname[0] == '*')){
                        sal_free(soc->counter_non_dma[i].extra_ctrs[ct*2+1].cname);
                        soc->counter_non_dma[i].extra_ctrs[ct*2+1].cname = NULL;
                    }
                }

                sal_free(soc->counter_non_dma[i].extra_ctrs);
                soc->counter_non_dma[i].extra_ctrs = NULL;
                if((soc->counter_non_dma[i].flags & _SOC_COUNTER_NON_DMA_OBM) &&
                   (soc->counter_non_dma[i].id != SOC_COUNTER_NON_DMA_PORT_OBM_FC_EVENTS)) {
                    soc->counter_non_dma[i+1].extra_ctrs = NULL;
                }
            }
#endif /* BCM_TOMAHAWK_SUPPORT */
        }
        sal_free(soc->counter_non_dma);
        soc->counter_non_dma = NULL;
    }

    if (soc->counter_buf32 != NULL) {
        soc_cm_sfree(unit, soc->counter_buf32);
        soc->counter_buf32 = NULL;
        soc->counter_buf64 = NULL;
    }

    if (soc->counter_hw_val != NULL) {
        sal_free(soc->counter_hw_val);
        soc->counter_hw_val = NULL;
    }

    if (soc->counter_sw_val != NULL) {
        sal_free(soc->counter_sw_val);
        soc->counter_sw_val = NULL;
    }

    if (soc->counter_delta != NULL) {
        sal_free(soc->counter_delta);
        soc->counter_delta = NULL;
    }

#ifdef BCM_SBUSDMA_SUPPORT
    if (_soc_blk_counter_handles[unit] != NULL) {
        sal_free(_soc_blk_counter_handles[unit]);
        _soc_blk_counter_handles[unit] = NULL;
    }

    if (soc->blk_ctr_buf != NULL) {
        soc_cm_sfree(unit, soc->blk_ctr_buf);
        soc->blk_ctr_buf = NULL;
    }

    if (soc->blk_counter_hw_val != NULL) {
        sal_free(soc->blk_counter_hw_val);
        soc->blk_counter_hw_val = NULL;
    }

    if (soc->blk_counter_sw_val != NULL) {
        sal_free(soc->blk_counter_sw_val);
        soc->blk_counter_sw_val = NULL;
    }

    _soc_counter_pending[unit] = 0;
#endif
    return SOC_E_NONE;
}

/*
 * StrataSwitch counter register map
 *
 * NOTE: soc_attach verifies this map is correct and prints warnings if
 * not.  The map should contain only registers with a single field named
 * COUNT, and all such registers should be in the map.
 *
 * The soc_counter_map[] array is a list of counter registers in the
 * order found in the internal address map and counter DMA buffer.
 * soc_counter_map[0] corresponds to S-Channel address
 * COUNTER_OFF_MIN, and contains SOC_CTR_MAP_SIZE(unit) entries.
 *
 * This map structure, contents, and size are exposed only to provide a
 * convenient way to loop through all available counters.
 *
 * Accumulated counters are stored as 64-bit values and may be written
 * and read by multiple tasks on a 32-bit processor.  This requires
 * atomic operation to get correct values.  Also, to fetch-and-clear a
 * counter requires atomic operation.
 *
 * These atomic operations are very brief, so rather than using an
 * inefficient mutex we use splhi to lock out interrupts and task
 * switches.  In theory, splhi is only a few instructions on most
 * processors.
 */

#define COUNTER_ATOMIC_DEF soc_control_t *
#define COUNTER_ATOMIC_BEGIN(_soc) \
    do { \
        if ((_soc) && (_soc)->counter_lock) { \
            sal_spinlock_lock((_soc)->counter_lock); \
        } \
    } while (0)
#define COUNTER_ATOMIC_END(_soc) \
    do { \
        if ((_soc) && (_soc)->counter_lock) { \
            sal_spinlock_unlock((_soc)->counter_lock); \
        } \
    } while (0)

#ifdef BROADCOM_DEBUG

char *soc_ctr_type_names[] = SOC_CTR_TYPE_NAMES_INITIALIZER;

/*
 * Function:
 *      _soc_counter_verify (internal)
 * Purpose:
 *      Verify contents of soc_counter_map[] array
 */
void
_soc_counter_verify(int unit)
{
    int                 i, errors, num_ctrs, found;
    soc_ctr_type_t      ctype;
    soc_reg_t           reg;
    int                 ar_idx;
    uint32              skip_offset = 0;

    LOG_DEBUG(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_counter_verify: unit %d begins\n"), unit));
    errors = 0;
    if (SOC_IS_TRIUMPH3(unit)) {
        skip_offset = 32;
    }
#if defined(BCM_HURRICANE2_SUPPORT)
    if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit) || \
        SOC_IS_GREYHOUND(unit)) {
        skip_offset = 55;
    }
#endif

#ifdef BCM_GREYHOUND2_SUPPORT
    if (SOC_IS_GREYHOUND2(unit)) {
        skip_offset = 111;
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

    for (ctype = SOC_CTR_TYPE_FE; ctype < SOC_CTR_NUM_TYPES; ctype++) {
        if (SOC_HAS_CTR_TYPE(unit, ctype)) {
            num_ctrs = SOC_CTR_MAP_SIZE(unit, ctype);
            for (i = 0; i < num_ctrs; i++) {
                reg = SOC_CTR_TO_REG(unit, ctype, i);
                ar_idx = SOC_CTR_TO_REG_IDX(unit, ctype, i);
                if (SOC_COUNTER_INVALID(unit, reg)) {
                    continue;
                }
                if (reg >= SOC_COUNTER_TABLE_FIELD_START) {
                    continue; 
                };
                if (!SOC_REG_IS_COUNTER(unit, reg)) {
                    LOG_DEBUG(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "soc_counter_verify: "
                                            "%s cntr %s (%d) index %d "
                                            "is not a counter\n"),
                                 soc_ctr_type_names[ctype],
                                 SOC_REG_NAME(unit, reg), reg, i));
                    errors = 1;
                }
                if (((SOC_REG_CTR_IDX(unit, reg) + ar_idx) -
                     COUNTER_IDX_OFFSET(unit) - skip_offset)
                    != i) {
                    LOG_DEBUG(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "soc_counter_verify: "
                                            "%s cntr %s (%d) index mismatch.\n"
                                            "    (ctr_idx %d + ar_idx %d) - offset %d != "
                                            "index in ctr array %d\n"),
                                 soc_ctr_type_names[ctype],
                                 SOC_REG_NAME(unit, reg), reg,
                                 SOC_REG_CTR_IDX(unit, reg),
                                 ar_idx,
                                 COUNTER_IDX_OFFSET(unit),
                                 i));
                    errors = 1;
                }
            }
        }
    }

    for (reg = 0; reg < NUM_SOC_REG; reg++) {
        if (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg)) {
            found = FALSE;
            i = -1;
            for (ctype = SOC_CTR_TYPE_FE; ctype < SOC_CTR_NUM_TYPES; ctype++) {
                if (SOC_HAS_CTR_TYPE(unit, ctype)) {
                    num_ctrs = SOC_CTR_MAP_SIZE(unit, ctype);
                    for (i = 0; i < num_ctrs; i++) {
                        if (SOC_CTR_TO_REG(unit, ctype, i) == reg) {
                            if ((SOC_REG_CTR_IDX(unit, reg) -
                                 COUNTER_IDX_OFFSET(unit) - skip_offset) != i) {
                                LOG_DEBUG(BSL_LS_SOC_COMMON,
                                            (BSL_META_U(unit,
                                                        "soc_counter_verify: "
                                                        "%s cntr %s (%d) index mismatch.\n"
                                                        "    (ctr_idx %d - offset %d) != "
                                                        "index in ctr array %d\n"),
                                             soc_ctr_type_names[ctype],
                                             SOC_REG_NAME(unit, reg), reg,
                                             SOC_REG_CTR_IDX(unit, reg),
                                             COUNTER_IDX_OFFSET(unit),
                                             i));
                                errors = 1;
                            }
                            found = TRUE;
                            break;
                        }
                    }
                }
                if (found) {
                    break;
                }
            }

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_KATANA_SUPPORT)
            if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit) ||
                SOC_IS_HURRICANE2(unit) || SOC_IS_KATANAX(unit)) {
                /* OAM_SEC_NS_COUNTER_64 is a general purpose counter
                   which counts time for entire chip */
                if ((!found) && (OAM_SEC_NS_COUNTER_64r == reg)) {
                    found = TRUE;
                }
            }
#endif
#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)
            if(SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) || \
               SOC_IS_GREYHOUND2(unit)) {
                if( (!found) && (
                    (HOLD_COS0r == reg) ||
                    (HOLD_COS1r == reg) ||
                    (HOLD_COS2r == reg) ||
                    (HOLD_COS3r == reg) ||
                    (HOLD_COS4r == reg) ||
                    (HOLD_COS5r == reg) ||
                    (HOLD_COS6r == reg) ||
                    (HOLD_COS7r == reg) ) ) {
                    found = TRUE;
                }
            }
#endif
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                if ((!found) && (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_RXLP))) {
                    found = TRUE;
                }
            }
#endif
            /*  Few RTS registers are marked as counters.. (ISCOUNTER => 1) */
            if (SOC_IS_APACHE(unit)) {
                if ((!found) &&
                    (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_IPROC))) {
                    found = TRUE;
                }
            }

#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                if ((!found) &&
                    (SOC_BLOCK_IS(SOC_REG_INFO(unit, reg).block, SOC_BLK_CPRI))) {
                    found = TRUE;
                }
            }
#endif

/* Workaround for SDK-130624. Remove this block after ARCHTH3-423
   is resolved and integrated */
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                if (!found) {
                    switch (reg) {
                        case CPU_MASQUERADE_COUNTER_DEST_TYPEr:
                        case CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE0r:
                        case CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE1r:
                        case CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE2r:
                        case CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE3r:
                        case CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE4r:
                        case CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE5r:
                        case CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE6r:
                        case CPU_MASQUERADE_COUNTER_DEST_TYPE_PIPE7r:
                        case IS_PKSCH_PKT_CREDITS_PER_PIPEr:
                        case IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE0r:
                        case IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE1r:
                        case IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE2r:
                        case IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE3r:
                        case IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE4r:
                        case IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE5r:
                        case IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE6r:
                        case IS_PKSCH_PKT_CREDITS_PER_PIPE_PIPE7r:
                        case MMU_CRB_CREDIT_DROP_PKT_COUNTr:
                        case MMU_CRB_INVALID_DESTINATION_PKT_COUNTr:
                        case MMU_CRB_PKT_DROP_CNTR_STATr:
                        case MMU_EBCFG_ECC_SINGLE_BIT_ERRORSr:
                        case MMU_EBCFG_MEM_FAIL_INT_CTRr:
                        case MMU_EBCTM_BURST_CTRL_STSr:
                        case MMU_EBCTM_CT_FSM_STSr:
                        case MMU_EBPTS_EDB_CREDIT_COUNTERr:
                        case MMU_EBPTS_PKSCH_PKT_CREDITS_PER_PIPEr:
                        case MMU_GLBCFG_ECC_SINGLE_BIT_ERRORSr:
                        case MMU_GLBCFG_MEM_FAIL_INT_CTRr:
                        case MMU_INTFO_OOBFC_MSG_TX_CNTr:
                        case MMU_ITMCFG_ECC_SINGLE_BIT_ERRORSr:
                        case MMU_ITMCFG_MEM_FAIL_INT_CTRr:
                        case MMU_PTSCH_EB_CREDIT_CONFIGr:
                        case MMU_RL_DEBUG_PKT_CNTr:
                            found = TRUE;
                            break;
                        default:
                            break;
                    }

                }
            }
#endif /* BCM_TOMAHAWK3_SUPPORT */

            if (!found) {
                LOG_DEBUG(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "soc_counter_verify: "
                                        "counter %d %s is missing (i=%d, 0x%x)\n"),
                             (int)reg, SOC_REG_NAME(unit, reg), i, i));
                errors = 1;
            }
        }
    }

    if (errors) {
        LOG_CLI((BSL_META_U(unit,
                            "\nERRORS found during counter initialization.  "
                            "Set debug verbose for more info.\n\n")));
    }

    LOG_DEBUG(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_counter_verify: unit %d ends\n"), unit));
}

/*
 * Function:
 *      _soc_counter_illegal
 * Purpose:
 *      Routine to display info about bad counter and halt.
 */

void
_soc_counter_illegal(int unit, soc_reg_t ctr_reg)
{
    LOG_CLI((BSL_META_U(unit,
                        "soc_counter_get: unit %d: "
                        "ctr_reg %d (%s) is not a counter\n"),
             unit, ctr_reg, SOC_REG_NAME(unit, ctr_reg)));
    /* assert(0); */
}

#endif /* BROADCOM_DEBUG */

#ifdef BCM_TOMAHAWK_SUPPORT
extern uint32 soc_mem_fifo_delay_value;


STATIC void
_soc_counter_fifo_process(int unit, central_ctr_eviction_fifo_entry_t *entry)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *pkt_ctr_dma, *byte_ctr_dma;
    soc_counter_evict_t *evict;
    soc_mem_t mem;
    int index, mem_id, pipe;
    uint32 fval[2];
    int pkt_ctr_id, byte_ctr_id, offset, base_index;
    uint64 ctr_diff;

    COUNTER_ATOMIC_DEF s = SOC_CONTROL(unit);

    soc = SOC_CONTROL(unit);

    mem = CENTRAL_CTR_EVICTION_FIFOm;
    if (SOC_IS_TRIDENT3X(unit)) {
        index = soc_format_field32_get(unit, CTR_EVICTION_MESSAGEfmt, entry, INDEXf);
        mem_id = soc_format_field32_get(unit, CTR_EVICTION_MESSAGEfmt, entry, POOL_NUMf);
        pipe = soc_format_field32_get(unit, CTR_EVICTION_MESSAGEfmt, entry, PIPE_NUMf);
    } else {
        if(SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWK3(unit)) {
        index = soc_mem_field32_get(unit, mem, entry, INDEXf);
    } else {
    index = soc_mem_field32_get(unit, mem, entry, CEV_INDEXf);
    }
    mem_id = soc_mem_field32_get(unit, mem, entry, POOL_NUMf);
    pipe = soc_mem_field32_get(unit, mem, entry, PIPE_NUMf);
    }
    /* Check if pool number is within the range */
    if ((mem_id < 1) || (mem_id > soc->counter_evict_max_mem_id)) {
        soc->counter_evict_inval_poolid_count++;
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                    (BSL_META_U(unit,
                     "Invalid eviction pool# = %d\n"), mem_id));
        return;
    }

    evict = &soc->counter_evict[mem_id];
    pkt_ctr_id = evict->pkt_counter_id;
    byte_ctr_id = evict->byte_counter_id;
    offset = evict->offset[pipe];

    pkt_ctr_dma = &soc->counter_non_dma[pkt_ctr_id - SOC_COUNTER_NON_DMA_START];

    if (SOC_IS_TRIDENT3X(unit)) {
        fval[0] = soc_format_field32_get(unit, CTR_EVICTION_MESSAGEfmt,
                                         entry, PKT_CNTf);
    } else {
        fval[0] = soc_mem_field32_get(unit, mem, entry, PKT_CNTf);
    }

    COMPILER_64_SET(ctr_diff, 0, fval[0]);
    base_index = pkt_ctr_dma->base_index + offset;

    COUNTER_ATOMIC_BEGIN(s);
    COMPILER_64_ADD_64(soc->counter_sw_val[base_index + index], ctr_diff);
    COUNTER_ATOMIC_END(s);

    byte_ctr_dma = &soc->counter_non_dma[byte_ctr_id - SOC_COUNTER_NON_DMA_START];
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_format_field_get(unit, CTR_EVICTION_MESSAGEfmt, entry->entry_data,
                             BYTE_CNTf, fval);
    } else {
        soc_mem_field_get(unit, mem, entry->entry_data, BYTE_CNTf, fval);
    }
    COMPILER_64_SET(ctr_diff, fval[1], fval[0]);
    base_index = byte_ctr_dma->base_index + offset;

    COUNTER_ATOMIC_BEGIN(s);
    COMPILER_64_ADD_64(soc->counter_sw_val[base_index + index], ctr_diff);
    COUNTER_ATOMIC_END(s);

}

void
_soc_ctr_evict_fifo_dma_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc = SOC_PCI_CMC(unit);
    soc_mem_t mem;
    int i, count, adv_threshold;
    uint32 rval, stat_fail_mask;
    uint32 hostmem_timeout, done, error;
    int rv, interval, non_empty, fail;
    uint8 chan;
    int entries_per_buf, entry_words, yield_entries, yield_num;
    uint32 stat_fail_ovrflw_mask;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        chan = SOC_MEM_FIFO_DMA_CHANNEL_0;
    }else
#endif
    {
    chan = SOC_MEM_FIFO_DMA_CHANNEL_1;
    }

    /* Maximum possible counter evictions per second is 2700. Hostbuf size is
     * set to double the value to account for load conditions */
    entries_per_buf = soc_property_get(unit, "spn_COUNTER_EVICT_HOSTBUF_SIZE",
                                       _SOC_CTR_EVICT_HOSTBUF_SIZE);
    adv_threshold = entries_per_buf / 2;
    yield_entries = soc_property_get(unit, "spn_COUNTER_EVICT_ENTRIES_MAX",
                                     _SOC_CTR_EVICT_ENTRIES_MAX);
    yield_num = yield_entries;

    mem = CENTRAL_CTR_EVICTION_FIFOm;
    entry_words = soc_mem_entry_words(unit, mem);
    host_buff[unit] = soc_cm_salloc(unit, entries_per_buf * entry_words *
                              sizeof(uint32), "Counter Eviction DMA Buffer");
    if (host_buff[unit] == NULL) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_CTR_EVICT_DMA, __LINE__,
                           SOC_E_MEMORY);
        goto cleanup_exit;
    }

    rv = soc_fifodma_masks_get(unit, &hostmem_timeout, &stat_fail_ovrflw_mask, &error, &done);

    rv = soc_fifodma_start(unit, chan, TRUE, mem, 0, MEM_BLOCK_ANY, 0,
                                      entries_per_buf, host_buff[unit]);
    if (SOC_FAILURE(rv)) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_CTR_EVICT_DMA, __LINE__,
                           rv);
        goto cleanup_exit;
    }

    host_entry[unit] = host_buff[unit];
    buff_max[unit] = (uint32 *)(host_entry[unit]) + (entries_per_buf * entry_words);

    stat_fail_mask = hostmem_timeout | stat_fail_ovrflw_mask;
    soc->counter_evict_inval_poolid_count = 0;
    while ((interval = soc->ctr_evict_interval)) {
        fail = FALSE;
        /* Interrupt */
        if (soc->ctrEvictDmaIntrEnb) {
            soc_fifodma_intr_enable(unit, chan);
            if (sal_sem_take(soc->ctrEvictIntr, interval) < 0) {
                LOG_DEBUG(BSL_LS_SOC_EVICT,
                            (BSL_META_U(unit,
                                        "%s polling timeout "
                                        "soc_mem_fifo_delay_value=%d\n"),
                             soc->ctr_evict_name, soc_mem_fifo_delay_value));
            } else {
                LOG_DEBUG(BSL_LS_SOC_EVICT,
                            (BSL_META_U(unit,
                                        "%s woken up soc_mem_fifo_delay_value=%d\n"),
                             soc->ctr_evict_name, soc_mem_fifo_delay_value));
                /* check for timeout or overflow and either process or continue */
                soc_fifodma_status_get(unit, chan, &rval);
                if (rval & stat_fail_mask) {
                    fail = TRUE;
                }
            }
        /* Polling */
        } else {
            /* Sleep for counter eviction interval */
            sal_usleep(interval);
        }

        EVICT_LOCK(unit);
        do {
            non_empty = FALSE;

            /* get entry count, process if nonzero else continue */
            rv = soc_fifodma_num_entries_get(unit, chan, &count);
            if (SOC_SUCCESS(rv)) {
                non_empty = TRUE;
                if (count > adv_threshold) {
                    count = adv_threshold;
                }

                for (i = 0; i < count; i++, yield_num--) {
                    if(!soc->counter_interval) {
                        EVICT_UNLOCK(unit);
                        goto cleanup_exit;
                    }
                    /* Yield if value set in counter_evict_entries_max config variable is
                     * not default value and if number specified in config variable has been
                     * reached
                     */
                    if (yield_entries) {
                        if (yield_num == 0) {
                            yield_num = yield_entries;
                            sal_thread_yield();
                        }
                    }

                    _soc_counter_fifo_process(unit, host_entry[unit]);
                    host_entry[unit] = (uint32 *)(host_entry[unit]) + entry_words;
                    /* handle roll over */
                    if ((uint32 *)(host_entry[unit]) >= buff_max[unit]) {
                        host_entry[unit] = host_buff[unit];
                    }
                }
                soc_fifodma_set_entries_read(unit, chan, i);
            }

            /* check and clear error */
            soc_fifodma_status_get(unit, chan, &rval);
            if (rval & done) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "FIFO DMA engine terminated for "
                                      "cmc[%d]:chan[%d]\n"),
                           cmc, chan));
                if (rval & error) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "FIFO DMA engine encountered error: "
                                          "[0x%x]\n"),
                               rval));
                }
                EVICT_UNLOCK(unit);
                goto cleanup_exit;
            }
        } while (non_empty);
        EVICT_UNLOCK(unit);

        /* Clearing of the FIFO_CH_DMA_INT interrupt by resetting
           overflow & timeout status in FIFO_CHy_RD_DMA_STAT_CLR reg */
        if (fail) {
            soc_fifodma_status_clear(unit, chan);
        }
    }

cleanup_exit:
    soc_fifodma_stop(unit, chan);

    if (host_buff[unit] != NULL) {
        soc_cm_sfree(unit, host_buff[unit]);
        host_buff[unit] = NULL;
        host_entry[unit] = NULL;
    }
    soc->ctr_evict_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

int
soc_ctr_evict_stop(int unit)
{
    soc_control_t *soc;
    int                 rv = SOC_E_NONE;
    soc_timeout_t       to;
    int chan;

    if (!soc_feature(unit, soc_feature_counter_eviction)) {
        return SOC_E_UNAVAIL;
    }

    soc = SOC_CONTROL(unit);

    SOC_IF_ERROR_RETURN(soc_counter_tomahawk_eviction_enable(unit, FALSE));

    if (!soc_feature(unit, soc_feature_fifo_dma) ||
        SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        chan = SOC_MEM_FIFO_DMA_CHANNEL_1;
        soc_fifodma_intr_disable(unit, chan);
    }

    soc->ctr_evict_interval = 0; /* Request exit */
    if (soc->ctr_evict_pid && (soc->ctr_evict_pid != SAL_THREAD_ERROR)) {
        /* Wake up thread so it will check the exit flag */
        if (soc->ctrEvictIntr != NULL) {
            sal_sem_give(soc->ctrEvictIntr);
        }
        /* Give thread a few seconds to wake up and exit */
        if (SAL_BOOT_SIMULATION) {
            soc_timeout_init(&to, 300 * 1000000, 0);
        } else {
            soc_timeout_init(&to, 60 * 1000000, 0);
        }
        while (soc->ctr_evict_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                  "thread will not exit\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }
    return rv;
}

int
soc_ctr_evict_start(int unit, uint32 flags, sal_usecs_t interval)
{
    soc_control_t *soc;
    uint32 flag = _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
    int pri, chan;

    if (!soc_feature(unit, soc_feature_counter_eviction)) {
        return SOC_E_UNAVAIL;
    }

    if (!soc_property_get(unit, "ctr_evict_enable", 1)) {
        LOG_CLI((BSL_META_U(unit,
                            "*** skip counter eviction start for now\n")));
        soc_counter_tomahawk_eviction_flags_update(unit, flag, 0);
        return SOC_E_NONE;
    }

    soc = SOC_CONTROL(unit);

    if (soc->ctr_evict_interval != 0) {
        SOC_IF_ERROR_RETURN(soc_ctr_evict_stop(unit));
    }

    sal_snprintf(soc->ctr_evict_name, sizeof(soc->ctr_evict_name),
                 "CtrEvict.%d", unit);
    soc->ctr_evict_flags = flags;
    soc->ctr_evict_interval = interval;

    if (interval == 0) {
        return SOC_E_NONE;
    }


    if (soc->ctr_evict_pid == SAL_THREAD_ERROR) {
        pri = soc_property_get(unit, "spn_COUNTER_EVICT_THREAD_PRI",
                               _SOC_CTR_EVICT_THREAD_PRI);

        soc->ctr_evict_pid =
            sal_thread_create(soc->ctr_evict_name, SAL_THREAD_STKSZ, pri,
                              _soc_ctr_evict_fifo_dma_thread,
                              INT_TO_PTR(unit));
        if (soc->ctr_evict_pid == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_ctr_evict_start: Could not start thread\n")));
            return SOC_E_MEMORY;
        }
    }

    if (!soc_feature(unit, soc_feature_fifo_dma)) {
        chan = SOC_MEM_FIFO_DMA_CHANNEL_1;
        soc_fifodma_intr_enable(unit, chan);
    }

    /* coverity[dead_error_line] */
    SOC_IF_ERROR_RETURN(soc_counter_tomahawk_eviction_enable(unit, TRUE));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_counter_eviction_sync
 * Purpose:
 *      Sync values stored in Eviction Fifo to the SW val
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      This is to ensure counter values in Transit(in Eviction FIFO) to be
 *      synced when stat sync is issue.
 *      Ensures that ALL counter activity that occurred before the sync
 *      is reflected in the results of any soc_counter_get()-type
 *      routine that is called after the sync.
 */
int
soc_counter_eviction_sync(int unit)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    uint8 chan = SOC_MEM_FIFO_DMA_CHANNEL_1;

    /* disable interrupt before releasing semaphore, which causes eviction
     * thread to sync stat from Eviction fifo to Sw_val
     */
    soc_fifodma_intr_disable(unit, chan);

    if (soc->ctrEvictIntr != NULL) {
        sal_sem_give(soc->ctrEvictIntr);
    }

    return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK_SUPPORT */

/*
 * Function:
 *      soc_counter_autoz
 * Purpose:
 *      Set or clear AUTOZ mode for all MAC counter registers (all ports)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      enable - if TRUE, enable, otherwise disable AUTOZ mode
 * Returns:
 *      SOC_E_XXX.
 * Notes:
 *      This module requires AUTOZ is off to accumulate counters properly.
 *      On 5665, this also turns on/off the MMU counters AUTOZ mode.
 */

int
soc_counter_autoz(int unit, int enable)
{
    soc_port_t          port;

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_counter_autoz: unit=%d enable=%d\n"), unit, enable));

    PBMP_PORT_ITER(unit, port) {
#if defined(BCM_ESW_SUPPORT)
        SOC_IF_ERROR_RETURN(soc_autoz_set(unit, port, enable));
#endif
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      _soc_counter_get
 * Purpose:
 *      Given a counter register number and port number, fetch the
 *      64-bit software-accumulated counter value or read from device counter
 *      register. The software-accumulated counter value is zeroed if requested.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ctr_reg - counter register
 *      zero - if TRUE, current counter is zeroed after reading.
 *      sync_mode - if TRUE, read from device else software accumulated value
 *      val - (OUT) 64-bit counter value.
 * Returns:
 *      SOC_E_XXX.
 * Notes:
 *      Returns 0 if ctr_reg is INVALIDr or not enabled.
 */

STATIC INLINE int
_soc_counter_get(int unit, soc_port_t port, soc_reg_t ctr_reg, int ar_idx,
                 int zero, int sync_mode, uint64 *val)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 port_index, num_entries;
    char                *cname;
    uint64              *vptr;
    uint64              value;
    COUNTER_ATOMIC_DEF  s = SOC_CONTROL(unit);
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);

#ifdef BCM_TOMAHAWK_SUPPORT
    if ((SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) &&
        (ctr_reg >= SOC_COUNTER_NON_DMA_START) &&
        (ctr_reg < SOC_COUNTER_NON_DMA_END)) {
        soc_counter_non_dma_t *non_dma =
                    &soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START];
        soc_ctr_control_info_t ctrl_info;
        if (soc_tomahawk_mem_is_xpe(unit, non_dma->mem)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_XPE;
        }
#ifdef BCM_FLOWTRACKER_SUPPORT
        else if ((ctr_reg >=
            SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_NUM_EXCEEDED_CNT) &&
            (ctr_reg <= SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_METER_EXCEEDED_CNT)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_FT_GROUP;
            sync_mode = 1;
        }
        else if ((ctr_reg >=
            SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_START_CNT) &&
            (ctr_reg <= SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_END_CNT)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_FT_GROUP;
            sync_mode = 1;
        }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if (soc_tomahawk3_mem_is_itm(unit, non_dma->mem) ||
                 soc_tomahawk3_reg_is_itm(unit, non_dma->reg)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;

            /* some exceptions here */
            if(SOC_MEM_IS_VALID(unit, non_dma->mem)) {
                switch(non_dma->mem) {
                    case MMU_THDO_SRC_PORT_DROP_COUNTm:
                        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
                    break;
                    default:
                    break;
                }
            }
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
        else {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
        }
        ctrl_info.instance = port;
        return soc_counter_generic_get(unit, ctr_reg, ctrl_info,
                                       sync_mode ? SOC_COUNTER_SYNC_ENABLE : 0,
                                       ar_idx, val);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    /*when port is invalid, rely on ar_idx to get ind*/
    if ((port == SOC_PORT_INVALID) && SOC_IS_TRIDENT2PLUS(unit) &&
        (ctr_reg >= SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC &&
         ctr_reg <= SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC )) {
       SOC_IF_ERROR_RETURN(_soc_counter_get_info(unit, SOC_PORT_MIN(unit,all),
                                    ctr_reg, &port_index, &num_entries, &cname));
       port_index += ar_idx;
     } else
#endif
    {
        /* coverity[negative_returns] */
        SOC_IF_ERROR_RETURN(_soc_counter_get_info(unit, port, ctr_reg,
                                                  &port_index, &num_entries,
                                                  &cname));
        if (ar_idx >= num_entries) {
            return SOC_E_PARAM;
        }

        if (sync_mode == TRUE) {
            COUNTER_LOCK(unit);

            /* sync the software counter with hardware counter */
            if (!SOC_COUNTER_INVALID(unit, ctr_reg)) {
#if defined(BCM_XGS_SUPPORT) || defined(BCM_PETRA_SUPPORT)
                soc_counter_collect64(unit, FALSE, port, ctr_reg);
#endif
            } else if (ctr_reg >= SOC_COUNTER_NON_DMA_START &&
                       ctr_reg < SOC_COUNTER_NON_DMA_END) {
            if(soc_ctr_ctrl->collect_non_stat_dma) {
                soc_ctr_ctrl->collect_non_stat_dma(unit, port, port_index,
                                                   ar_idx, ctr_reg);
          }
#ifdef BCM_TOMAHAWK_SUPPORT
          if (soc_feature(unit, soc_feature_counter_eviction)) {
 	      SOC_IF_ERROR_RETURN(soc_counter_eviction_sync(unit));
	  }
#endif

          LOG_DEBUG(BSL_LS_SOC_COUNTER,
                      (BSL_META_U(unit,
                                  "port[%d], ctr_reg[%d],"
                                  "port_index[%d], ar_idx[%d]"
                                  "num_entries[%d], cname[%s] "),
                       port, ctr_reg, port_index, ar_idx,
                       num_entries, cname));
        }
        COUNTER_UNLOCK(unit);
    }

        if (ar_idx > 0 && ar_idx < num_entries) {
            port_index += ar_idx;
        }
    }

    /* Try to minimize the atomic section as much as possible */
    if (ctr_reg >= SOC_COUNTER_NON_DMA_START &&
        ctr_reg < SOC_COUNTER_NON_DMA_END &&
        (soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].flags &
         _SOC_COUNTER_NON_DMA_CURRENT)) {
        vptr = &soc->counter_hw_val[port_index];
    } else {
        vptr = &soc->counter_sw_val[port_index];
    }
    if (zero) {
        COUNTER_ATOMIC_BEGIN(s);
        value = *vptr;
        COMPILER_64_ZERO(*vptr);
        COUNTER_ATOMIC_END(s);
    } else {
        COUNTER_ATOMIC_BEGIN(s);
        value = *vptr;
        COUNTER_ATOMIC_END(s);
    }

    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "cntr get %s port=%d "
                             "port_index=%d vptr=%p val=0x%08x_%08x\n"),
                  cname,
                  port,
                  port_index,
                  (void *)vptr, COMPILER_64_HI(value), COMPILER_64_LO(value)));

    *val = value;

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_counter_instance_encode
 * Purpose:
 *      Given an input array of heterogeneous soc_ctr_control_info_t
 *      data structures, encode them into one single output
 *      soc_ctr_control_info_t data structure.
 * Parameters:
 *      input_ctrl_info  - (IN) Input array of ctrl_info variables to be
 *                          encoded.
 *      count            - (IN) Number of ctrl_info variables to be encoded.
 *      output_ctrl_info - (OUT) Encoded output ctrl_info data structure.
 * Returns:
 *      SOC_E_XXX.
 * Notes:
 *      Encoding support currently exists for SOC_CTR_INSTANCE_TYPE_POOL_PIPE
 *      and SOC_CTR_INSTANCE_TYPE_XPE_PORT
 *      Decoding is done inside soc_tomahawk_counter_generic_get_info function.
 *      Encoding scheme
 *      Bits 0-3 PIPE
 *           4-9 POOL
 *         10-13 XPE
 *         14-23 PORT
 *         24-25 ITM
 */
int
soc_counter_instance_encode(soc_ctr_control_info_t *input_ctrl_info, int count,
                             soc_ctr_control_info_t *output_ctrl_info)
{
    switch (count) {
        case 1:
            *output_ctrl_info = input_ctrl_info[0];
            return SOC_E_NONE;
            break;

        case 2:
            if ((input_ctrl_info[0].instance_type == SOC_CTR_INSTANCE_TYPE_POOL)
                    && (input_ctrl_info[1].instance_type ==
                        SOC_CTR_INSTANCE_TYPE_PIPE)) {
                if (input_ctrl_info[1].instance == -1) {
                    *output_ctrl_info = input_ctrl_info[1];
                } else {
                    output_ctrl_info->instance_type = SOC_CTR_INSTANCE_TYPE_POOL_PIPE;
                    /* First 4 bits allocated to Pipe and next 6 allocated to
                     * Pool */
                    output_ctrl_info->instance = ((input_ctrl_info[0].instance << 4)
                                                    | (input_ctrl_info[1].instance));
                }
                return SOC_E_NONE;
            } else if ((input_ctrl_info[0].instance_type ==
                        SOC_CTR_INSTANCE_TYPE_PIPE)
                        && (input_ctrl_info[1].instance_type ==
                        SOC_CTR_INSTANCE_TYPE_POOL)) {
                if (input_ctrl_info[0].instance == -1) {
                    *output_ctrl_info = input_ctrl_info[0];
                } else {
                    output_ctrl_info->instance_type = SOC_CTR_INSTANCE_TYPE_POOL_PIPE;
                    output_ctrl_info->instance = ((input_ctrl_info[1].instance << 4)
                                                | (input_ctrl_info[0].instance));
                }
                return SOC_E_NONE;
            } else if ((input_ctrl_info[0].instance_type ==
                        SOC_CTR_INSTANCE_TYPE_XPE)
                       && (input_ctrl_info[1].instance_type ==
                       SOC_CTR_INSTANCE_TYPE_PORT)) {
                    output_ctrl_info->instance_type = SOC_CTR_INSTANCE_TYPE_XPE_PORT;
                    /*Bits 10-13 allocated to XPE and next 10 allocated to
                     * Port */
                    output_ctrl_info->instance = ((input_ctrl_info[0].instance << 10)
                                                    |(input_ctrl_info[1].instance << 14));
                return SOC_E_NONE;
            } else if ((input_ctrl_info[0].instance_type ==
                        SOC_CTR_INSTANCE_TYPE_PORT)
                       && (input_ctrl_info[1].instance_type ==
                       SOC_CTR_INSTANCE_TYPE_XPE)) {
                    output_ctrl_info->instance_type = SOC_CTR_INSTANCE_TYPE_XPE_PORT;
                    /*Bits 10-13 allocated to XPE and next 10 allocated to
                     * Port */
                    output_ctrl_info->instance = ((input_ctrl_info[0].instance << 14)
                                                    |(input_ctrl_info[1].instance << 10));
                return SOC_E_NONE;
            } else if ((input_ctrl_info[0].instance_type ==
                        SOC_CTR_INSTANCE_TYPE_ITM)
                       && (input_ctrl_info[1].instance_type ==
                       SOC_CTR_INSTANCE_TYPE_PORT)) {
                    output_ctrl_info->instance_type = SOC_CTR_INSTANCE_TYPE_ITM_PORT;
                    output_ctrl_info->instance =
                        ((input_ctrl_info[0].instance << CTR_INSTANCE_BIT_SHIFT_ITM) |
                         (input_ctrl_info[1].instance << CTR_INSTANCE_BIT_SHIFT_PORT));
                return SOC_E_NONE;
            } else {
                return SOC_E_PARAM;
            }
            break;

        default:
            return SOC_E_PARAM;

    }
}

static INLINE int
_soc_counter_generic_get(int unit, soc_ctr_control_info_t ctrl_info, soc_reg_t ctr_reg,
                         int ar_idx, int zero, int sync_mode, uint64 *val)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 base_index, num_entries,i, pipe_max_idx = 0;
    uint64              *vptr;
    uint64              value;
#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_info_t * si = &SOC_INFO(unit);
#endif

    COUNTER_ATOMIC_DEF  s = SOC_CONTROL(unit);
    COMPILER_64_SET(*val, 0, 0);
    SOC_IF_ERROR_RETURN(soc_counter_generic_get_info(unit, ctrl_info,ctr_reg,
                                                     &base_index,&num_entries));
    if (ar_idx >= num_entries) {
        return SOC_E_PARAM;
    }
    /* Retrieve ar_idx across all pipes */
    if (((ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_PIPE) &&
        (ctrl_info.instance == -1)) || (ctrl_info.instance_type ==
        SOC_CTR_INSTANCE_TYPE_POOL)) {
        pipe_max_idx = base_index + (NUM_PIPE(unit) * num_entries);
    }

    if (sync_mode == TRUE) {
        COUNTER_LOCK(unit);

        /* sync the software counter with hardware counter */
        if (!SOC_COUNTER_INVALID(unit, ctr_reg)) {
#if defined(BCM_XGS_SUPPORT) || defined(BCM_PETRA_SUPPORT)
            if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_PORT) {
                soc_counter_collect64(unit, FALSE, ctrl_info.instance, ctr_reg);
            }
#endif
        } else if (ctr_reg >= SOC_COUNTER_NON_DMA_START &&
                   ctr_reg < SOC_COUNTER_NON_DMA_END) {
            if (pipe_max_idx == 0) {
               soc_counter_generic_collect_non_dma_entries(unit, ctrl_info,
                                                           base_index,
                                                           ar_idx, ctr_reg);
            } else if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_PIPE) {
                for (i = 0; i < NUM_PIPE(unit); i++) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                    if (SOC_PBMP_IS_NULL(si->pipe_pbm[i])) {
                        continue;
                    }
#endif
                    ctrl_info.instance = i;
                    soc_counter_generic_collect_non_dma_entries(unit,
                                                                ctrl_info,
                                                                base_index + i * num_entries,
                                                                ar_idx,
                                                                ctr_reg);
                }
            }
            else if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_POOL ||
                    ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_POOL_PIPE) {
                int pipe_start = 0, pipe_end = 0, i = 0;
                soc_ctr_control_info_t     ctrl_info_in[2], ctrl_info_out;

                ctrl_info_in[0].instance_type = SOC_CTR_INSTANCE_TYPE_POOL;
                ctrl_info_in[0].instance = 0;
                ctrl_info_in[1].instance_type = SOC_CTR_INSTANCE_TYPE_PIPE;
                ctrl_info_in[1].instance = 0;

                if (ctrl_info.instance_type
                    == SOC_CTR_INSTANCE_TYPE_POOL) {
                    pipe_start = 0;
                    pipe_end = NUM_PIPE(unit) - 1;
                    ctrl_info_in[0].instance = ctrl_info.instance;
                } else {
                    if (ctrl_info.instance != -1) {
                        pipe_start = pipe_end = (ctrl_info.instance & 0xF);
                        ctrl_info_in[0].instance = (ctrl_info.instance >> 4) & 0x3F;
                    } else {
                        pipe_start = 0;
                        pipe_end = NUM_PIPE(unit) - 1;
                    }
                }

                for(i = pipe_start; i <= pipe_end; i++) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                    if (SOC_PBMP_IS_NULL(si->pipe_pbm[i])) {
                        continue;
                    }
#endif
                    ctrl_info_in[1].instance = i;
                    soc_counter_instance_encode(ctrl_info_in, 2, &ctrl_info_out);
                    soc_counter_generic_collect_non_dma_entries(unit,
                                                                ctrl_info_out,
                                                                base_index,
                                                                ar_idx,
                                                                ctr_reg);
                }
            }
        }

        COUNTER_UNLOCK(unit);

#ifdef BCM_TOMAHAWK_SUPPORT
        if (soc_feature(unit, soc_feature_counter_eviction)) {
            int i = 0;
            int rv = 0;
            int count = 0;
            uint8 chan = SOC_MEM_FIFO_DMA_CHANNEL_1;
            soc_mem_t mem = CENTRAL_CTR_EVICTION_FIFOm;
            int entry_words = soc_mem_entry_words(unit, mem);

#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TOMAHAWK3(unit)) {
                chan = SOC_MEM_FIFO_DMA_CHANNEL_0;
            }
#endif
            EVICT_LOCK(unit);
            if (host_entry[unit]) {
                rv = soc_fifodma_num_entries_get(unit, chan, &count);
                if (SOC_SUCCESS(rv)) {
                    for (i = 0; i < count; i++) {
                        if (!soc->counter_interval) {
                            break;
                        }
                        _soc_counter_fifo_process(unit, host_entry[unit]);
                        host_entry[unit] = (uint32*)(host_entry[unit]) + entry_words;
                        if ((uint32*)(host_entry[unit]) >= buff_max[unit]) {
                            host_entry[unit] = host_buff[unit];
                        }
                    }
                    soc_fifodma_set_entries_read(unit, chan, i);
                }
            }
            EVICT_UNLOCK(unit);
        }
#endif
    }

    if (ar_idx > 0 && ar_idx < num_entries) {
        base_index += ar_idx;
    }

    do {
        /* Try to minimize the atomic section as much as possible */
        if ((ctr_reg >= NUM_SOC_REG) &&
            (soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].flags &
            _SOC_COUNTER_NON_DMA_CURRENT)) {
            vptr = &soc->counter_hw_val[base_index];
        } else {
            vptr = &soc->counter_sw_val[base_index];
        }
        if (zero) {
            COUNTER_ATOMIC_BEGIN(s);
            value = *vptr;
            COMPILER_64_ZERO(*vptr);
            COUNTER_ATOMIC_END(s);
        } else {
            COUNTER_ATOMIC_BEGIN(s);
            value = *vptr;
            COUNTER_ATOMIC_END(s);
        }
        base_index += num_entries;
        COMPILER_64_ADD_64(*val, value);
    } while (base_index < pipe_max_idx);

    return SOC_E_NONE;
}

int soc_counter_sync_mode_get(int unit)
{
#ifdef BCM_PETRA_SUPPORT
#ifdef CRASH_RECOVERY_SUPPORT
    if (SOC_IS_ARAD(unit))
    {
        /* In case crash recovery is enabled - read the values from HW instead of SW */
        if (SOC_CR_ENABALED(unit))
            {
                return TRUE; /* read hw accumulated */
            }
        return FALSE; /* read sw accumulated */
    }
    else
#endif /* CRASH_RECOVERY_SUPPORT */
#endif
    {
        return FALSE; /* read sw accumulated */
    }
}

/*
 * Function:
 *      soc_counter_generic_get
 * Purpose:
 *      Given a counter register number, instance type, instance and a counter
 *      value, get both the software-accumulated value from the counter.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      ctr_reg - counter register to retrieve.
 *      ctrl_info - Counter Control Info Structure
 *                  Contains Instance type and instance.
 *      val(OUT) - 64-bit counter value.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *  *   Currently supported instance types are
 *      1. SOC_CTR_INSTANCE_TYPE_PORT
 *      2. SOC_CTR_INSTANCE_TYPE_POOL
 *      3. SOC_CTR_INSTANCE_TYPE_PIPE
 *      4. SOC_CTR_INSTANCE_TYPE_POOL_PIPE
 *      5. SOC_CTR_INSTANCE_TYPE_XPE
 *      6. SOC_CTR_INSTANCE_TYPE_XPE_PORT
 */
int
soc_counter_generic_get(int unit, soc_reg_t ctr_reg,
        soc_ctr_control_info_t ctrl_info, uint32 flags,
        int ar_idx, uint64 *val)
{
    int sync_mode = soc_counter_sync_mode_get(unit);
    int rv = SOC_E_NONE;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    int pipe, xpe, port;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8 rev_id;
#endif
    uint64 value;
    COMPILER_64_ZERO(*val);
    if (ctr_reg < SOC_COUNTER_NON_DMA_START ||
        ctr_reg >= SOC_COUNTER_NON_DMA_END) {
        return SOC_E_PARAM;
    }

    if (soc->counter_non_dma == NULL) {
        return SOC_E_RESOURCE;
    }

    if (!(soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].flags)) {
        return SOC_E_UNAVAIL;
    }

    if ((ctr_reg >= SOC_COUNTER_NON_DMA_START) &&
        (ctr_reg < SOC_COUNTER_NON_DMA_END)) {
        soc_counter_non_dma_t *non_dma =
            &soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START];
        soc_mem_t mem = non_dma->mem;
        soc_reg_t reg = non_dma->reg;
        soc_ctr_control_info_t input_ctrl_info [2], output_ctrl_info;
        if (flags & SOC_COUNTER_SYNC_ENABLE) {
            sync_mode = TRUE;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        soc_cm_get_id(unit, &dev_id, &rev_id);
        if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_ITM) {
            int itm;
            port = ctrl_info.instance;
            pipe = si->port_pipe[port];
            input_ctrl_info[0].instance_type = ctrl_info.instance_type;
            input_ctrl_info[1].instance = ctrl_info.instance;
            input_ctrl_info[1].instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
            if (mem != INVALIDm) {
                for (itm = 0; itm < NUM_ITM(unit) ; itm++) {
                    if (itm == 1 && (dev_id == BCM56983_DEVICE_ID)) {
                        continue;
                    }
                    input_ctrl_info[0].instance = itm;

                    if (SOC_MEM_UNIQUE_ACC_ITM(unit,mem,itm) == INVALIDm) {
                        continue;
                    }

                    if(itm > 0 &&
                       !(non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
                        /* skip if ITM memories that has non-ITM base type */
                        continue;
                    }

                    soc_counter_instance_encode(input_ctrl_info,2,&output_ctrl_info);
                    rv = _soc_counter_generic_get(unit, output_ctrl_info,
                                                  ctr_reg, ar_idx, FALSE,
                                                  sync_mode, &value);
                    if (rv != SOC_E_NONE) {
                        return rv;
                    }
                    /* Sum values from all valid itm-port combinations */
                    COMPILER_64_ADD_64(*val, value);
                }
            } else if (reg != INVALIDr) {

                for (itm = 0; itm < NUM_ITM(unit) ; itm++) {
                    if (itm == 1 && (dev_id == BCM56983_DEVICE_ID)) {
                        continue;
                    }
                    input_ctrl_info[0].instance = itm;

                    soc_counter_instance_encode(input_ctrl_info,2,&output_ctrl_info);
                    rv = _soc_counter_generic_get(unit, output_ctrl_info,
                                                  ctr_reg, ar_idx, FALSE,
                                                  sync_mode, &value);

                    if (rv != SOC_E_NONE) {
                        return rv;
                    }
                    /* Sum values from all valid itm-port combinations */
                    COMPILER_64_ADD_64(*val, value);
                }
            }
        } else
#endif /* BCM_TOMAHAWK3_SUPPORT */

        /* Each XPE maps to a pair of IPIPE or EPIPE based on memory type.
         * Hence loop through all applicable XPEs for a given pipe and return
         * accumulated value*/
        if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_XPE) {
            port = ctrl_info.instance;
            pipe = si->port_pipe[port];
            if (mem != INVALIDm) {
                input_ctrl_info[0].instance_type = ctrl_info.instance_type;
                input_ctrl_info[1].instance = ctrl_info.instance;
                input_ctrl_info[1].instance_type = SOC_CTR_INSTANCE_TYPE_PORT;

                for (xpe = 0; xpe < NUM_XPE(unit) ; xpe++) {
                    input_ctrl_info[0].instance = xpe;
                    if ((mem == MMU_CTR_COLOR_DROP_MEMm) && !SOC_IS_TOMAHAWK2(unit)) {
                        /* Get valid XPE/Pipe Combination */
                        if(!((si->epipe_xpe_map[pipe] >> xpe) & 1)) {
                            continue;
                        }
                    } else {
                        if (SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit,mem,xpe,pipe) == INVALIDm) {
                            /* Invalid XPE/Pipe Combination */
                            continue;
                        }
                    }
                    soc_counter_instance_encode(input_ctrl_info,2,&output_ctrl_info);
                    rv = _soc_counter_generic_get(unit, output_ctrl_info,
                                                  ctr_reg, ar_idx, FALSE,
                                                  sync_mode, &value);
                    if (rv != SOC_E_NONE) {
                        return rv;
                    }
                    /* Sum values from all valid xpe-port combinations */
                    COMPILER_64_ADD_64(*val, value);

                }
            } else if (reg != INVALIDr) {
                input_ctrl_info[0].instance_type = ctrl_info.instance_type;

                for (xpe = 0; xpe < NUM_XPE(unit) ; xpe++) {
                    input_ctrl_info[0].instance = xpe;
                    if (reg == THDI_POOL_SHARED_COUNT_SPr) {
                        /* Get valid XPE/Pipe Combination */
                        if(!((si->ipipe_xpe_map[pipe] >> xpe) & 1)) {
                            continue;
                        }
                    }
                    rv = _soc_counter_generic_get(unit, input_ctrl_info[0],
                                                  ctr_reg, ar_idx, FALSE,
                                                  sync_mode, &value);
                    if (rv != SOC_E_NONE) {
                        return rv;
                    }
                    /* Sum values from all valid xpe-port combinations */
                    COMPILER_64_ADD_64(*val, value);
                }
            }
        } else {
            rv = _soc_counter_generic_get(unit, ctrl_info, ctr_reg, ar_idx,
                                          FALSE, sync_mode, val);
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_counter_get_per_buffer
 * Purpose:
 *      Given a counter register number, instance type, instance and a counter
 *      value, get the counter value of each buffer.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      ctr_reg - counter register to retrieve.
 *      ctrl_info - Counter Control Info Structure
 *                  Contains Instance type and instance.
 *      val(OUT) - 64-bit counter value array.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *  *   Currently supported instance types are
 *      1. SOC_CTR_INSTANCE_TYPE_PORT
 *      2. SOC_CTR_INSTANCE_TYPE_POOL
 *      3. SOC_CTR_INSTANCE_TYPE_PIPE
 *      4. SOC_CTR_INSTANCE_TYPE_POOL_PIPE
 *      5. SOC_CTR_INSTANCE_TYPE_XPE
 *      6. SOC_CTR_INSTANCE_TYPE_XPE_PORT
 */
int soc_counter_get_per_buffer(
    int unit,
    soc_reg_t ctr_reg,
    soc_ctr_control_info_t ctrl_info,
    uint32 flags,
    int ar_idx,
    uint64 *val)
{
    int sync_mode = soc_counter_sync_mode_get(unit);
    int rv = SOC_E_NONE;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    int pipe = 0, xpe, port;

    COMPILER_64_ZERO(*val);
    if (ctr_reg < SOC_COUNTER_NON_DMA_START ||
        ctr_reg >= SOC_COUNTER_NON_DMA_END) {
        return SOC_E_PARAM;
    }

    if (soc->counter_non_dma == NULL) {
        return SOC_E_RESOURCE;
    }

    if (!(soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].flags)) {
        return SOC_E_UNAVAIL;
    }

    if ((ctr_reg >= SOC_COUNTER_NON_DMA_START) &&
        (ctr_reg < SOC_COUNTER_NON_DMA_END)) {
        soc_counter_non_dma_t *non_dma =
            &soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START];
        soc_mem_t mem = non_dma->mem;
        soc_reg_t reg = non_dma->reg;
        soc_ctr_control_info_t input_ctrl_info [2], output_ctrl_info;
        if (flags & SOC_COUNTER_SYNC_ENABLE) {
            sync_mode = TRUE;
        }

        /* Each XPE maps to a pair of IPIPE or EPIPE based on memory type.
         * Hence loop through all applicable XPEs for a given pipe and return
         * accumulated value*/
        if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_XPE) {
            port = ctrl_info.instance;
            if (port != SOC_PORT_INVALID) {
                pipe = si->port_pipe[port];
            }
            if (mem != INVALIDm) {
                input_ctrl_info[0].instance_type = ctrl_info.instance_type;
                input_ctrl_info[1].instance = ctrl_info.instance;
                input_ctrl_info[1].instance_type = SOC_CTR_INSTANCE_TYPE_PORT;

                for (xpe = 0; xpe < NUM_XPE(unit) ; xpe++) {
                    input_ctrl_info[0].instance = xpe;
                    if (port != SOC_PORT_INVALID) {
                        if ((mem == MMU_CTR_COLOR_DROP_MEMm) && !SOC_IS_TOMAHAWK2(unit)) {
                            /* Get valid XPE/Pipe Combination */
                            if(!((si->epipe_xpe_map[pipe] >> xpe) & 1)) {
                                continue;
                            }
                        } else {
                            if (SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit,mem,xpe,pipe) == INVALIDm) {
                                /* Invalid XPE/Pipe Combination */
                                continue;
                            }
                        }
                    }
                    soc_counter_instance_encode(input_ctrl_info,2,&output_ctrl_info);
                    rv = _soc_counter_generic_get(unit, output_ctrl_info,
                                                  ctr_reg, ar_idx, FALSE,
                                                  sync_mode, &val[xpe]);
                    if (rv != SOC_E_NONE) {
                        return rv;
                    }
                }
            } else if (reg != INVALIDr) {
                input_ctrl_info[0].instance_type = ctrl_info.instance_type;

                for (xpe = 0; xpe < NUM_XPE(unit) ; xpe++) {
                    input_ctrl_info[0].instance = xpe;
                    /* Get counter of all XPEs when port is SOC_PORT_INVALID */
                    if ((reg == THDI_POOL_SHARED_COUNT_SPr) &&
                        (port != SOC_PORT_INVALID)) {
                        /* Get valid XPE/Pipe Combination */
                        if(!((si->ipipe_xpe_map[pipe] >> xpe) & 1)) {
                            continue;
                        }
                    }
                    rv = _soc_counter_generic_get(unit, input_ctrl_info[0],
                                                  ctr_reg, ar_idx, FALSE,
                                                  sync_mode, &val[xpe]);
                    if (rv != SOC_E_NONE) {
                        return rv;
                    }
                }
            }
        } else {
            rv = _soc_counter_generic_get(unit, ctrl_info, ctr_reg, ar_idx,
                                          FALSE, sync_mode, val);
        }
    }

    return rv;
}

#ifdef BCM_APACHE_SUPPORT
int
soc_counter_apache_generic_get_info(int unit,soc_ctr_control_info_t ctrl_info,
                                      soc_reg_t id, int *base_index,
                                      int *num_entries)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *non_dma;
    soc_port_t port;

    if ((id < NUM_SOC_REG) || (id >= SOC_COUNTER_NON_DMA_END)) {
        return SOC_E_PARAM;
    }

    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID) &&
            !(non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* For Parent-Child model, return E_UNAVAIL if child non_dma is INVALID.
         */
        return SOC_E_UNAVAIL;
    }

    *base_index = 0;
    switch (ctrl_info.instance_type) {
        case SOC_CTR_INSTANCE_TYPE_PORT:
            port = ctrl_info.instance;
            if (si->port_l2p_mapping[port] == -1) {
                *base_index = 0;
                *num_entries = 0;
                return SOC_E_PARAM;
            }

            switch (id) {
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO:
                case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_LO:
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_HI:
                case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSY_HI:
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS0:
                case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS0:
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSLESS1:
                case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1:
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_EXPRESS:
                case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_EXPRESS:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_LO:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_LO:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSY_HI:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSY_HI:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS0:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS0:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_LOSSLESS1:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_LOSSLESS1:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_PKT_OBM_EXPRESS:
                case SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_EXPRESS:
                    if (IS_RDB_PORT(unit, port) ||
                        IS_LB_PORT(unit, port)) {
                        return SOC_E_PARAM;
                    }
                    return _soc_counter_apache_get_info(unit, port, id,
                                                         base_index,
                                                         num_entries);
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;

        default:
            return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}
#endif

int
soc_counter_generic_get_info(int unit, soc_ctr_control_info_t ctrl_info,
                             soc_reg_t id, int *base_index, int *num_entries)
{
    int rv = SOC_E_UNAVAIL;
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        rv = soc_counter_tomahawk_generic_get_info(unit, ctrl_info, id,
                base_index, num_entries);
    }
#endif
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
        rv = soc_counter_trident3_generic_get_info(unit, ctrl_info, id,
                base_index, num_entries);
    }
#endif
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        rv = soc_counter_hurricane4_generic_get_info(unit, ctrl_info, id,
                base_index, num_entries);
    }
#endif
#ifdef BCM_HELIX5_SUPPORT
    if (SOC_IS_HELIX5(unit)) {
        rv = soc_counter_helix5_generic_get_info(unit, ctrl_info, id,
                base_index, num_entries);
    }
#endif
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
        rv = soc_counter_firebolt6_generic_get_info(unit, ctrl_info, id,
                base_index, num_entries);
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        rv = soc_counter_apache_generic_get_info(unit, ctrl_info, id,
                base_index, num_entries);
    }
#endif
    return rv;
}

/*
 * Function:
 *      soc_counter_get, soc_counter_get32
 *      soc_counter_get_zero, soc_counter_get32_zero
 * Purpose:
 *      Given a counter register number and port number, fetch the
 *      64-bit software-accumulated counter value (normal versions)
 *      or the 64-bit software-accumulated counter value truncated to
 *      32 bits (32-bit versions).  The software-accumulated counter
 *      value is zeroed if requested.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ctr_reg - counter register to retrieve.
 *      val - (OUT) 64/32-bit counter value.
 * Returns:
 *      SOC_E_XXX.
 * Notes:
 *      The 32-bit version returns only the lower 32 bits of the 64-bit
 *      counter value.
 */


int
soc_counter_get(int unit, soc_port_t port, soc_reg_t ctr_reg,
                int ar_idx, uint64 *val)
{
    int sync_mode = soc_counter_sync_mode_get(unit);
    return _soc_counter_get(unit, port, ctr_reg, ar_idx,
                            FALSE, sync_mode, val);
}

int
soc_counter_get32(int unit, soc_port_t port, soc_reg_t ctr_reg,
                  int ar_idx, uint32 *val)
{
    uint64              val64;
    int                 rv;
    int sync_mode = soc_counter_sync_mode_get(unit);

    rv = _soc_counter_get(unit, port, ctr_reg, ar_idx,
                          FALSE, sync_mode, &val64);

    if (rv >= 0) {
        COMPILER_64_TO_32_LO(*val, val64);
    }

    return rv;
}

int
soc_counter_get_zero(int unit, soc_port_t port,
                     soc_reg_t ctr_reg, int ar_idx, uint64 *val)
{
    int sync_mode = soc_counter_sync_mode_get(unit);
    return _soc_counter_get(unit, port, ctr_reg, ar_idx,
                            TRUE, sync_mode, val);
}

int
soc_counter_get32_zero(int unit, soc_port_t port,
                       soc_reg_t ctr_reg, int ar_idx, uint32 *val)
{
    uint64              val64;
    int                 rv;
    int sync_mode = soc_counter_sync_mode_get(unit);

    rv = _soc_counter_get(unit, port, ctr_reg, ar_idx,
                          TRUE, sync_mode, &val64);

    if (rv >= 0) {
        COMPILER_64_TO_32_LO(*val, val64);
    }

    return rv;
}

/*
 * Function:
 *      soc_counter_sync_get, soc_counter_sync_get32
 * Purpose:
 *      Given a counter register and port number, sync the
 *      counter value from the device register to the sw accumulated
 *      value.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ctr_reg - counter register to retrieve.
 *      val - (OUT) 64/32-bit counter value.
 * Returns:
 *      SOC_E_XXX.
 */

int
soc_counter_sync_get(int unit, soc_port_t port, soc_reg_t ctr_reg,
                     int ar_idx, uint64 *val)
{
    int sync_mode = TRUE; /* read register */
    return _soc_counter_get(unit, port, ctr_reg, ar_idx,
                            FALSE, sync_mode, val);
}

int
soc_counter_sync_get32(int unit, soc_port_t port, soc_reg_t ctr_reg,
                       int ar_idx, uint32 *val)
{
    uint64              val64;
    int                 rv;
    int                 sync_mode = TRUE; /* read register */

    rv = _soc_counter_get(unit, port, ctr_reg, ar_idx,
                          FALSE, sync_mode, &val64);

    if (rv >= 0) {
        COMPILER_64_TO_32_LO(*val, val64);
    }

    return rv;
}

int
soc_counter_direct_get(int unit, soc_port_t port, soc_reg_t ctr_reg,
                       int ar_idx, uint64 *val)
{
    int                 port_index, num_entries;
    int                 port_base, index;
    char                *cname;
    uint64              value = COMPILER_64_INIT(0, 0);

    port_base = COUNTER_MIN_IDX_GET(unit, port);
    SOC_IF_ERROR_RETURN(_soc_counter_get_info(unit, port, ctr_reg,
                                              &port_index, &num_entries,
                                              &cname));
    if (ar_idx >= num_entries) {
        return SOC_E_PARAM;
    }
    index = port_index - port_base;
    ctr_reg = PORT_CTR_REG(unit, port, index)->reg;
    ar_idx = PORT_CTR_REG(unit, port, index)->index;
    if (!SOC_COUNTER_INVALID(unit, ctr_reg)) {
#ifdef SOC_COUNTER_TABLE_SUPPORT
#if defined (BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit) &&
            SOC_REG_IS_COUNTER_TABLE(unit, ctr_reg)) {
            soc_ctr_tbl_entry_t *ctr_tbl_info_entry = NULL;
            SOC_IF_ERROR_RETURN
                (_soc_counter_tbl_get_info(unit, port, ctr_reg, &ctr_tbl_info_entry));
            SOC_IF_ERROR_RETURN
                (_soc_ctr_tbl_field_get(unit, ctr_reg, port, ctr_tbl_info_entry, &value));
        } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#endif /* SOC_COUNTER_TABLE_SUPPORT */
        {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, ctr_reg, port, ar_idx,
                                            &value));
        }
    }

    *val = value;

    return SOC_E_NONE;
}

#ifdef BCM_TOMAHAWK_SUPPORT
STATIC int
_soc_counter_generic_get_delta(int unit, soc_ctr_control_info_t ctrl_info,
                              soc_reg_t ctr_reg, int ar_idx, uint64 *value)
{
    int base_index, num_entries;
    soc_control_t   *soc = SOC_CONTROL(unit);
    COUNTER_ATOMIC_DEF  s = SOC_CONTROL(unit);
    int pipe_max_idx = 0;
    uint64 *vptr;
    uint64 val;

    COMPILER_64_ZERO(*value);

    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get_info(unit, ctrl_info, ctr_reg,
                                      &base_index, &num_entries));

    if (ar_idx >= num_entries) {
        return SOC_E_PARAM;
    }

    if (((ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_PIPE) &&
        (ctrl_info.instance == -1)) || (ctrl_info.instance_type ==
        SOC_CTR_INSTANCE_TYPE_POOL)) {
        pipe_max_idx = base_index + (NUM_PIPE(unit) * num_entries);
    }

    if (ar_idx > 0 && ar_idx < num_entries) {
        base_index += ar_idx;
    }

    if (soc->counter_interval == 0) {
        COMPILER_64_ZERO(*value);
    } else {
        do {
            if ((ctr_reg >= NUM_SOC_REG) &&
                (soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].flags &
                _SOC_COUNTER_NON_DMA_CURRENT)) {
                return SOC_E_PARAM;
            } else {
                vptr = &soc->counter_delta[base_index];
            }
            COUNTER_ATOMIC_BEGIN(s);
            val = *vptr;
            COUNTER_ATOMIC_END(s);
            base_index += num_entries;
            COMPILER_64_ADD_64(*value, val);
        } while (base_index < pipe_max_idx);
    }
    return SOC_E_NONE;

}

STATIC int
soc_counter_generic_get_delta(int unit, soc_ctr_control_info_t ctrl_info,
                              soc_reg_t ctr_reg, int ar_idx, uint64 *value)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;
    soc_info_t *si = &SOC_INFO(unit);
    uint64 val;
    int pipe, xpe, port;

    COMPILER_64_ZERO(*value);
    COMPILER_64_ZERO(val);

    if (ctr_reg >= SOC_COUNTER_NON_DMA_END) {
        return SOC_E_PARAM;
    }

    if (!(soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].flags)) {
        return SOC_E_UNAVAIL;
    }

    if ((ctr_reg >= SOC_COUNTER_NON_DMA_START) &&
        (ctr_reg < SOC_COUNTER_NON_DMA_END)) {
        soc_mem_t mem = soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].mem;
        soc_reg_t reg = soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].reg;
        soc_ctr_control_info_t input_ctrl_info[2], output_ctrl_info;
        /* Each XPE maps to a pair of IPIPE or EPIPE based on memory type.
         * Hence loop through all applicable XPEs for a given pipe and return
         * accumulated value*/
        if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_XPE) {
            port = ctrl_info.instance;
            pipe = si->port_pipe[port];
            if (mem != INVALIDm) {
                input_ctrl_info[0].instance_type = ctrl_info.instance_type;
                input_ctrl_info[1].instance = ctrl_info.instance;
                input_ctrl_info[1].instance_type = SOC_CTR_INSTANCE_TYPE_PORT;

                for (xpe = 0; xpe < NUM_XPE(unit) ; xpe++) {
                    input_ctrl_info[0].instance = xpe;
                    if (mem == MMU_CTR_COLOR_DROP_MEMm) {
                        /* Get valid XPE/Pipe Combination */
                        if(!((si->epipe_xpe_map[pipe] >> xpe) & 1)) {
                            continue;
                        }
                    } else {
                        if (SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit,mem,xpe,pipe) == INVALIDm) {
                            /* Invalid XPE/Pipe Combination */
                            continue;
                        }
                    }
                    soc_counter_instance_encode(input_ctrl_info,2,&output_ctrl_info);
                    rv = _soc_counter_generic_get_delta(unit, output_ctrl_info,
                                                  ctr_reg, ar_idx,
                                                  &val);
                    if (rv != SOC_E_NONE) {
                        return rv;
                    }
                    /* Sum values from all valid xpe-port combinations */
                    COMPILER_64_ADD_64(*value, val);
                }
            } else if (reg != INVALIDr) {
                input_ctrl_info[0].instance_type = ctrl_info.instance_type;

                for (xpe = 0; xpe < NUM_XPE(unit) ; xpe++) {
                    input_ctrl_info[0].instance = xpe;
                    if (reg == THDI_POOL_SHARED_COUNT_SPr) {
                        /* Get valid XPE/Pipe Combination */
                        if(!((si->ipipe_xpe_map[pipe] >> xpe) & 1)) {
                            continue;
                        }
                    }
                    rv = _soc_counter_generic_get_delta(unit, input_ctrl_info[0],
                                                  ctr_reg, ar_idx,
                                                  &val);
                    if (rv != SOC_E_NONE) {
                        return rv;
                    }
                    /* Sum values from all valid xpe-port combinations */
                    COMPILER_64_ADD_64(*value, val);
                }
            }
#ifdef BCM_TOMAHAWK3_SUPPORT
        } else if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_ITM) {
            uint16 itm;
            port = ctrl_info.instance;
            pipe = si->port_pipe[port];
            input_ctrl_info[0].instance_type = ctrl_info.instance_type;
            input_ctrl_info[1].instance = ctrl_info.instance;
            input_ctrl_info[1].instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
            for (itm = 0; itm < NUM_ITM(unit) ; itm++) {
                input_ctrl_info[0].instance = itm;

                soc_counter_instance_encode(input_ctrl_info,2,&output_ctrl_info);
                rv = _soc_counter_generic_get_delta(unit, output_ctrl_info,
                                              ctr_reg, ar_idx,
                                              &val);
                if (rv != SOC_E_NONE) {
                    return rv;
                }
                /* Sum values for a counter from all ITMs */
                COMPILER_64_ADD_64(*value, val);
            }
#endif
        } else {
            rv = _soc_counter_generic_get_delta(unit, ctrl_info, ctr_reg, ar_idx,
                                                value);
        }
    } else {
            rv = _soc_counter_generic_get_delta(unit, ctrl_info, ctr_reg, ar_idx,
                                                value);
    }

    return rv;
}

STATIC int
soc_counter_generic_get_rate(int unit, soc_ctr_control_info_t ctrl_info,
                             soc_reg_t ctr_reg, int ar_idx, uint64 *rate)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    uint64 value;

    COMPILER_64_ZERO(value);
    COMPILER_64_ZERO(*rate);

    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get_delta(unit, ctrl_info, ctr_reg, ar_idx,
                                        &value));

    if (soc->counter_interval == 0) {
        COMPILER_64_ZERO(*rate);
    } else {
#ifdef  COMPILER_HAS_DOUBLE
        /*
         * This uses floating point right now because uint64 multiply/divide
         * support is missing from many compiler libraries.
         */

        #define UINT32_MAX_VALUE (4294967296.0)
        uint32          ratedblHI, ratedblLO;
        double          interval, delta, ratedbl;

        delta = COMPILER_64_HI(value) * UINT32_MAX_VALUE +
                COMPILER_64_LO(value);

        interval = SAL_USECS_SUB(soc->counter_coll_cur,
                                 soc->counter_coll_prev) / 1000000.0;

        if (interval < 0.0001) {
            COMPILER_64_SET(*rate, 0, 0);
        } else {
            ratedbl = (delta / interval + 0.5);
            ratedblHI = (uint32)(ratedbl / UINT32_MAX_VALUE);
            ratedblLO = (uint32)(ratedbl - ratedblHI * UINT32_MAX_VALUE);

            COMPILER_64_SET(*rate, ratedblHI, ratedblLO);
        }

#else   /* !COMPILER_HAS_DOUBLE */
        uint32          delta32, rate32;
        int             interval;

        COMPILER_64_TO_32_LO(delta32, value);

        interval = SAL_USECS_SUB(soc->counter_coll_cur, soc->counter_coll_prev);

        if (interval < 100) {
            rate32 = 0;
        } else {
            rate32 = _shr_div_exp10(delta32, interval, 6);
        }

        COMPILER_64_SET(*rate, 0, rate32);
#endif  /* !COMPILER_HAS_DOUBLE */
    }
    return SOC_E_NONE;
}
#endif  /* BCM_TOMAHAWK_SUPPORT */

/*
 * Function:
 *      soc_counter_get_rate
 * Purpose:
 *      Returns the counter incrementing rate in units of 1/sec.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ctr_reg - counter register to retrieve.
 *      rate - (OUT) 64-bit rate value.
 * Returns:
 *      SOC_E_XXX.
 */

int
soc_counter_get_rate(int unit, soc_port_t port,
                     soc_reg_t ctr_reg, int ar_idx, uint64 *rate)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 port_index, num_entries;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) &&
        (ctr_reg >= SOC_COUNTER_NON_DMA_START) &&
        (ctr_reg < SOC_COUNTER_NON_DMA_END)) {
        soc_ctr_control_info_t ctrl_info;
        int mem = soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START].mem;

        if (soc_tomahawk_mem_is_xpe(unit, mem)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_XPE;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if (soc_tomahawk3_mem_is_itm(unit, mem)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;

            /* some exceptions here */
            if(SOC_MEM_IS_VALID(unit, mem)) {
                switch(mem) {
                    case MMU_THDO_SRC_PORT_DROP_COUNTm:
                        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
                    break;
                    default:
                    break;
                }
            }
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
        else {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
        }
        ctrl_info.instance = port;
        return soc_counter_generic_get_rate(unit, ctrl_info, ctr_reg,
                                            ar_idx, rate);
    }
#endif
    SOC_IF_ERROR_RETURN(_soc_counter_get_info(unit, port, ctr_reg,
                                              &port_index, &num_entries,
                                              NULL));
    if (ar_idx >= num_entries) {
        return SOC_E_PARAM;
    }

    if (ar_idx > 0 && ar_idx < num_entries) {
        port_index += ar_idx;
    }

    if (soc->counter_interval == 0) {
        COMPILER_64_ZERO(*rate);
    } else {
#ifdef  COMPILER_HAS_DOUBLE
        /*
         * This uses floating point right now because uint64 multiply/divide
         * support is missing from many compiler libraries.
         */

        #define UINT32_MAX_VALUE (4294967296.0)
        uint32          ratedblHI, ratedblLO;
        double          interval, delta, ratedbl;

        delta = COMPILER_64_HI(soc->counter_delta[port_index]) * UINT32_MAX_VALUE +
                COMPILER_64_LO(soc->counter_delta[port_index]);

        interval = SAL_USECS_SUB(soc->counter_coll_cur,
                                 soc->counter_coll_prev) / 1000000.0;

        if (interval < 0.0001) {
            COMPILER_64_SET(*rate, 0, 0);
        } else {
            ratedbl = (delta / interval + 0.5);
            ratedblHI = (uint32)(ratedbl / UINT32_MAX_VALUE);
            ratedblLO = (uint32)(ratedbl - ratedblHI * UINT32_MAX_VALUE);

            COMPILER_64_SET(*rate, ratedblHI, ratedblLO);
        }

#else   /* !COMPILER_HAS_DOUBLE */
        uint32          delta32, rate32;
        int             interval;

        COMPILER_64_TO_32_LO(delta32, soc->counter_delta[port_index]);

        interval = SAL_USECS_SUB(soc->counter_coll_cur, soc->counter_coll_prev);

        if (interval < 100) {
            rate32 = 0;
        } else {
            rate32 = _shr_div_exp10(delta32, interval, 6);
        }

        COMPILER_64_SET(*rate, 0, rate32);
#endif  /* !COMPILER_HAS_DOUBLE */
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_counter_get32_rate
 * Purpose:
 *      Returns the counter incrementing rate in units of 1/sec.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ctr_reg - counter register to retrieve.
 *      rate - (OUT) 32-bit delta value.
 * Returns:
 *      SOC_E_XXX.
 */

int
soc_counter_get32_rate(int unit, soc_port_t port,
                       soc_reg_t ctr_reg, int ar_idx, uint32 *rate)
{
    uint64              rate64;

    SOC_IF_ERROR_RETURN(soc_counter_get_rate(unit, port, ctr_reg,
                                             ar_idx, &rate64));

    COMPILER_64_TO_32_LO(*rate, rate64);

    return SOC_E_NONE;
}

/* Function:
 *      soc_counter_generic_multi_get
 * Purpose:
 *      Given a counter register number, count, instance type and instance,
 *      fetch multiple 64-bit software-accumulated counter values from
 *      ar_idx to ar_idx + count.
 * Parameter:
 *      unit  - StrataSwitch Unit #.
 *      ctr_reg - Counter Register to retrieve.
 *      ctrl_info - Counter Control Info Structure
 *                  Contains the instance type and instance.
 *      flags - not yet implemented.
 *      ar_idx - Base index of counter to be retrieved.
 *      count - Number of entries to retrieve.
 *      val - (OUT) Base address of array holding the 64-bit counter values.
 *Returns:
 *      SOC_E_XXX
 *Notes:
 *      If count is set to 0, nothing is retrieved
*/
int
soc_counter_generic_multi_get(int unit, soc_reg_t ctr_reg,
                              soc_ctr_control_info_t ctrl_info,
                              uint32 flags, int ar_idx, int count,
                              uint64 *val)
{
    int sync_mode = soc_counter_sync_mode_get(unit);
    int rv = SOC_E_NONE;
    int i = 0; /* return value and iterator */
    soc_control_t *soc = SOC_CONTROL(unit);
    uint64 counter_val; /* value holder for each _soc_counter_generic_get call */
    COMPILER_64_ZERO(counter_val);

    if (flags & SOC_COUNTER_SYNC_ENABLE) {
        sync_mode = TRUE;
    }
    if (count < 0) {
        return SOC_E_PARAM;
    }

    if (soc->counter_non_dma == NULL) {
        return SOC_E_RESOURCE;
    }

    for (i = 0; i < count; i++) {
        rv = _soc_counter_generic_get(unit, ctrl_info, ctr_reg, (ar_idx+i),
                                        FALSE, sync_mode, &counter_val);
        val[i] = counter_val;
        if (rv!= SOC_E_NONE) {
            return rv;
        }
    }
    return rv;

}

STATIC int
_soc_distribute_u64(uint64 value, uint64 *l, uint64 *r)
{
    uint64 tmp, tmp1;
    tmp = value;
    COMPILER_64_SHR(tmp, 1);
    *l = tmp;
    COMPILER_64_SET(tmp1, 0, 1);
    COMPILER_64_AND(tmp1, value);
    COMPILER_64_ADD_64(tmp1, tmp);
    *r = tmp1;
    return 0;
}

/*
 * Function:
 *      soc_counter_set, soc_counter_set32
 * Purpose:
 *      Given a counter register number, port number, and a counter
 *      value, set both the hardware and software-accumulated counters
 *      to the value.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      ctr_reg - counter register to retrieve.
 *      val - 64/32-bit counter value.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The 32-bit version sets the upper 32-bits of the 64-bit
 *      software counter to zero.  The specified value is truncated
 *      to the width of the hardware register when storing to hardware.
 *      Use the value 0 to clear counters.
 */

STATIC int
_soc_counter_set(int unit, soc_port_t port, soc_reg_t ctr_reg,
                 int ar_idx, uint64 val64)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 port_index, num_entries;
    int                 index_min, index_max, index;
    soc_counter_non_dma_t *non_dma;
    uint32              entry[SOC_MAX_MEM_FIELD_WORDS], fval[2];
    int                 dma_base_index;
    soc_mem_t           dma_mem;
    COUNTER_ATOMIC_DEF  s = SOC_CONTROL(unit);
    int         rv = SOC_E_NONE;
    int                 prime_ctr_write, i;
    uint32              val32;
    int numpipe = 0;
    int ctr_is_readonly = 0;
#ifdef BCM_KATANA_SUPPORT
    uint32              pkt_count = 0;
#endif /* BCM_KATANA_SUPPORT */
    int qindex = -1;
#ifdef BCM_KATANA2_SUPPORT
      int j=0;
#endif
#if defined(BCM_SABER2_SUPPORT) || defined(BCM_KATANA_SUPPORT)
    uint16 dev_id;
    uint8 rev_id;
#endif
#ifdef BCM_TRIUMPH3_SUPPORT
    int skip_hw_set = 0;
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
    if ((SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit))&&
        (ctr_reg >= SOC_COUNTER_NON_DMA_START) &&
        (ctr_reg < SOC_COUNTER_NON_DMA_END)) {
        soc_ctr_control_info_t ctrl_info;
        if (IS_LB_PORT(unit, port)) {
            /* Do nothing for LB port */
            return SOC_E_NONE;
        }
        non_dma = &soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START];
        if (soc_tomahawk_mem_is_xpe(unit, non_dma->mem)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_XPE;
        }
#ifdef BCM_FLOWTRACKER_SUPPORT
        else if ((ctr_reg >=
            SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_NUM_EXCEEDED_CNT) &&
            (ctr_reg <= SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_METER_EXCEEDED_CNT)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_FT_GROUP;
        }
        else if ((ctr_reg >=
            SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_START_CNT) &&
            (ctr_reg <= SOC_COUNTER_NON_DMA_FT_GROUP_FLOW_END_CNT)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_FT_GROUP;
        }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if (soc_tomahawk3_mem_is_itm(unit, non_dma->mem) ||
                 soc_tomahawk3_reg_is_itm(unit, non_dma->reg)) {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_ITM;

            /* some exceptions here */
            if(SOC_MEM_IS_VALID(unit, non_dma->mem)) {
                switch(non_dma->mem) {
                    case MMU_THDO_SRC_PORT_DROP_COUNTm:
                        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
                    break;
                    default:
                    break;
                }
            }
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */
        else {
            ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
        }
        ctrl_info.instance = port;

        if (ctr_reg == SOC_COUNTER_NON_DMA_SFLOW_ING_PKT ||
            ctr_reg == SOC_COUNTER_NON_DMA_SFLOW_FLEX_PKT)
        {
            if (ar_idx == -1) {
                ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_SFLOW;
                ctrl_info.instance = -1;
                ar_idx = port;
            }
        }
        return soc_counter_generic_set(unit, ctr_reg, ctrl_info, 0,
                                       ar_idx, val64);
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit) &&
        (ctr_reg >= SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO) &&
        (ctr_reg <= SOC_COUNTER_NON_DMA_PORT_ENQUEUE_BYTE_OBM_EXPRESS)) {
        soc_ctr_control_info_t ctrl_info;
        if (IS_LB_PORT(unit, port) ||
            IS_RDB_PORT(unit, port) ||
            IS_MACSEC_PORT(unit, port) ||
            IS_CPU_PORT(unit, port)) {
            /* Do nothing for LB port */
            return SOC_E_NONE;
        }
        ctrl_info.instance = port;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
        return soc_counter_generic_set(unit, ctr_reg, ctrl_info, 0,
                                       ar_idx, val64);
    }
#endif
#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit) &&
        (ctr_reg >= SOC_COUNTER_NON_DMA_PORT_DROP_PKT_OBM_LOSSY_LO) &&
        (ctr_reg <= SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_OBM_LOSSLESS1)) {
        soc_ctr_control_info_t ctrl_info;
        if (IS_LB_PORT(unit, port) ||
            IS_CPU_PORT(unit, port)) {
            /* Do nothing for LB port */
            return SOC_E_NONE;
        }
        ctrl_info.instance = port;
        ctrl_info.instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
        return soc_counter_generic_set(unit, ctr_reg, ctrl_info, 0,
                                       ar_idx, val64);
    }
#endif
    SOC_IF_ERROR_RETURN(_soc_counter_get_info(unit, port, ctr_reg,
                                              &port_index, &num_entries,
                                              NULL));
    if (ar_idx >= num_entries) {
        return SOC_E_PARAM;
    }

    if (ar_idx < 0) {
        index_min = 0;
        index_max = num_entries - 1;
    } else {
        index_min = index_max = ar_idx;
    }

/*
 * Wait for dma counter processing completion
 */
#ifdef BCM_SBUSDMA_SUPPORT
        while (_soc_counter_pending[unit] && soc->counter_interval) {
            sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 100);
        }
#endif

    COUNTER_LOCK(unit);
#ifdef BCM_KATANA2_SUPPORT
/*
 * We are looping twice to set the mc stats in case of packing mode
*/
    for (j=0; j<2; j++) {
        if( j == 1) {
            if( SOC_IS_KATANA2(unit) && soc_feature(unit,
                                  soc_feature_mmu_packing) ) {
               rv = _soc_counter_katana2_get_mcindex(unit, port, ctr_reg,
                                            &port_index, &num_entries);
               if (SOC_FAILURE(rv)) {
                   goto counter_set_done;
               }
               if(port_index == -1) {
                  continue;
               }
            }
            else {
               continue;
            }
         }
#else
      {
#endif

    for (index = index_min; index <= index_max; index++) {
        ctr_is_readonly = 0;
#ifdef BCM_TRIUMPH3_SUPPORT
        skip_hw_set = 0;
#endif
#if defined(BCM_PETRA_SUPPORT)
    if (!_SOC_CONTROLLED_COUNTER_USE(unit, port)) {
#endif
        if ((ctr_reg >= SOC_COUNTER_NON_DMA_START) &&
            (ctr_reg < SOC_COUNTER_NON_DMA_END)) {
            non_dma = &soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START];
            if (non_dma->flags & (_SOC_COUNTER_NON_DMA_PEAK |
                                  _SOC_COUNTER_NON_DMA_CURRENT)) {
                continue;
            }

            if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
                goto counter_set_done;
            } else if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                (non_dma->extra_ctrs != NULL)) {
                goto counter_set_done;
            }

            if (non_dma->mem != INVALIDm) {
                dma_mem = non_dma->dma_mem[0];
                dma_base_index = port_index - non_dma->base_index +
                    non_dma->dma_index_min[0];
#if defined (BCM_METROLITE_SUPPORT)
               if (SOC_IS_METROLITE(unit)) {
                   numpipe = 1;
               } else
#endif
#ifdef BCM_SABER2_SUPPORT
               if ( SOC_IS_SABER2(unit) ) {
                   soc_cm_get_id(unit, &dev_id, &rev_id);
                   if (dev_id == BCM56233_DEVICE_ID) {
                       numpipe = 1;
                   } else {
                       numpipe = 2;
                   }
               } else
#endif

#if defined (BCM_KATANA2_SUPPORT)
               if ( SOC_IS_KATANA2(unit) ) {
                    numpipe = 4;
              } else
#endif
#if defined (BCM_KATANA_SUPPORT)
              if ( SOC_IS_KATANA(unit)) {
                    numpipe = 4;
                    soc_cm_get_id(unit, &dev_id, &rev_id);
                    if ((dev_id == BCM56445_DEVICE_ID)|| (dev_id == BCM56446_DEVICE_ID)||
                        (dev_id == BCM56447_DEVICE_ID)|| (dev_id == BCM56448_DEVICE_ID)||
                        (dev_id == BCM55441_DEVICE_ID)) {
                        numpipe = 1;
                    }
               } else
#endif
               {
                    numpipe = SOC_INFO(unit).num_pipe;
               }
                for (i = 1; i < numpipe; i++) {
                    if (non_dma->dma_mem[i] != INVALIDm &&
                            dma_base_index > non_dma->dma_index_max[i - 1]) {
                        dma_mem = non_dma->dma_mem[i];
                        dma_base_index -= non_dma->dma_index_max[i - 1] + 1;
                        dma_base_index += non_dma->dma_index_min[i];
                    }
                }

                /* Need to retrieve the table index
                 * for flexible mapping Non-DMA counter*/
                if (non_dma->flags & _SOC_COUNTER_NON_DMA_FLEX_MAPPING) {
                    if (non_dma->soc_counter_hw_idx_get == NULL) {
                        continue;
                    }
                    rv = non_dma->soc_counter_hw_idx_get(unit, port,
                            dma_base_index + index, &qindex);
                    if (SOC_FAILURE(rv)) {
                        goto counter_set_done;
                    }
                    if (-1 == qindex) {
                        continue;
                    }
                } else {
                    qindex = dma_base_index + index;
                }

                rv = soc_mem_read(unit, dma_mem, MEM_BLOCK_ANY, qindex, entry);
                if (SOC_FAILURE(rv)) {
                    goto counter_set_done;
                }
#ifdef BCM_KATANA_SUPPORT
                if ((SOC_IS_KATANA(unit)) &&
                        (!soc_feature(unit, soc_feature_counter_toggled_read))) {
                    soc_mem_field_get(unit, dma_mem, entry, COUNTf, &pkt_count);
                    if (!SAL_BOOT_BCMSIM) {
                        if (pkt_count) {
                            pkt_count -= 1;
                        }
                    }
                    soc_mem_field_set(unit, dma_mem, entry,
                            COUNTf, &pkt_count);

                }
#endif
                fval[0] = COMPILER_64_LO(val64);
                fval[1] = COMPILER_64_HI(val64);
                soc_mem_field_set(unit, dma_mem, entry,
                        non_dma->field, fval);

                /*
                 * If parity is not enabled on the device
                 * set the parity bit in the MMU_CTR_UC_DROP_MEMm
                 * during counter set as suggested by DE.
                 */
                if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIUMPH3(unit)) {
                    uint32 rval = 0;
                    if (dma_mem == MMU_CTR_UC_DROP_MEMm) {
                        rv = READ_MISCCONFIGr(unit, &rval);
                        if (SOC_SUCCESS(rv)) {
                            if (!soc_reg_field_get(unit, MISCCONFIGr,
                                                   rval, PARITY_GEN_ENf)) {
                                soc_mem_field32_set(unit, dma_mem, entry,
                                                    PARITYf, 0);
                                if (COMPILER_64_IS_ZERO(val64)) {
	  	  	 	                        sal_memset(entry, 0, sizeof(entry));
                                }
                            }
                        }
                    }
                }

                rv = soc_mem_write(unit, dma_mem, MEM_BLOCK_ALL, qindex, entry);
                if (SOC_FAILURE(rv)) {
                    goto counter_set_done;
                }

                /* Check non-DMA counters (memory) is read-only or not */
                if (soc_mem_flags(unit, dma_mem) & SOC_MEM_FLAG_READONLY) {
                    ctr_is_readonly = 1;
                }
            } else if (non_dma->reg != INVALIDr) {
                if (port < 0) {
                    rv = SOC_E_PARAM;
                    goto counter_set_done;
                }

                /* Check non-DMA counters (register) is read-only or not */
                if (SOC_REG_INFO(unit, non_dma->reg).flags & SOC_REG_FLAG_RO) {
                    ctr_is_readonly = 1;
                }

                /* For Per port -> PBMP_MEMBER(non_dma->pbmp, port) is valid
                 * and for system level counters it is invalid
                 * For system level counters -> SOC_PBMP_IS_NULL is valid
                 * and invalid for per port cases.
                 */
                if (PBMP_MEMBER(non_dma->pbmp, port) ||
                    (SOC_PBMP_IS_NULL(non_dma->pbmp) &&
                    (non_dma->num_entries == 1))) {
                    uint64 regval64;
                    uint32 regval32;
                    int base_index = 0;

                    if (SOC_IS_TR_VL(unit) || SOC_IS_SC_CQ(unit)) {
                        if (non_dma->reg == TXLLFCMSGCNTr) {
                            if (!IS_HG_PORT( unit, port)) {
                                continue;
                            }
                        }
                    }

                    if (!ctr_is_readonly) { /* non-DMA counters (register) is not readonly */
                        rv = soc_reg_get(unit, non_dma->reg,
                                         port, index, &regval64);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }

                        if (SOC_REG_IS_64(unit, non_dma->reg)) {
                            soc_reg64_field_set(unit, non_dma->reg, &regval64,
                                                non_dma->field, val64);
                        } else {
                            regval32 = COMPILER_64_LO(regval64);
                            soc_reg_field_set(unit, non_dma->reg, &regval32,
                                              non_dma->field, COMPILER_64_LO(val64));
                            COMPILER_64_SET(regval64, 0, regval32);
                        }

                        rv = soc_reg_set(unit, non_dma->reg,
                                         port, index, regval64);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }
                    } else { /* non-DMA counters (register) is readonly */
                        if (soc_feature(unit, soc_feature_tas)) {
                            if (ctr_reg == SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_EXPRESS ||
                                ctr_reg == SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_PREEMPT) {
                                /* read-only register, only take value=0 to clear it */
                                if (!COMPILER_64_IS_ZERO(val64)) {
                                    rv = SOC_E_UNAVAIL;
                                    goto counter_set_done;
                                }

                                /*
                                 * Both TAS_OVERRUN_PKT_CNT_EXPRESS/PREEMPT will be clear to 0
                                 * by set 1 to TAS_OVERRUN_CNT_CLRf.
                                 */
                                rv = soc_reg32_get(unit, TAS_GATE_PURGE_ENABLEr,
                                                   port, index, &regval32);
                                if (SOC_FAILURE(rv)) {
                                    goto counter_set_done;
                                }

                                soc_reg_field_set(unit, TAS_GATE_PURGE_ENABLEr, &regval32,
                                                  TAS_OVERRUN_CNT_CLRf, TRUE);

                                rv = soc_reg32_set(unit, TAS_GATE_PURGE_ENABLEr,
                                                   port, index, regval32);
                                if (SOC_FAILURE(rv)) {
                                    goto counter_set_done;
                                }

                                if (ctr_reg == SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_EXPRESS) {
                                    /*
                                      * Also need to clear software based counter value bufs
                                      * sw_val/hw_val/delta for PREEMPT
                                      */
                                    rv = _soc_counter_get_info(
                                             unit, port,
                                             SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_PREEMPT,
                                             &base_index, &num_entries, NULL);
                                } else {
                                    /*
                                      * Also need to clear software based counter value bufs
                                      * sw_val/hw_val/delta for EXPRESS
                                      */
                                    rv = _soc_counter_get_info(
                                             unit, port,
                                             SOC_COUNTER_NON_DMA_TAS_TXOVERRUN_EXPRESS,
                                             &base_index, &num_entries, NULL);
                                }

                                if (SOC_FAILURE(rv)) {
                                    goto counter_set_done;
                                }

                                /* The following section updates 64-bit values and must be atomic */
                                COUNTER_ATOMIC_BEGIN(s);
                                soc->counter_sw_val[base_index + index] = val64;
                                soc->counter_hw_val[base_index + index] = val64;
                                COMPILER_64_SET(soc->counter_delta[base_index + index], 0, 0);
                                COUNTER_ATOMIC_END(s);

                                /* need to clear soc->counter_hw_val to match the hardware value */
                                ctr_is_readonly = 0;
                            }
                        }
                    }
                }
            }
        } else if (ctr_reg != INVALIDr) {
            if (port < 0) {
                rv = SOC_E_PARAM;
                goto counter_set_done;
            }

#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit) && (IS_HG_PORT(unit, port))
                          && ((ctr_reg == RRPKTr) || (ctr_reg == RRBYTr)) ) {
                    skip_hw_set = 1;
                    ctr_is_readonly = 1;
            }
#endif

#ifdef SOC_COUNTER_TABLE_SUPPORT
            if (SOC_REG_IS_COUNTER_TABLE (unit, ctr_reg)) {
                if (SOC_IS_TOMAHAWK3(unit)) {
                    soc_ctr_tbl_entry_t *ctr_tbl_info_entry = NULL;
                    SOC_IF_ERROR_RETURN
                        (_soc_counter_tbl_get_info(unit, port, ctr_reg, &ctr_tbl_info_entry));
                    SOC_IF_ERROR_RETURN
                        (_soc_ctr_tbl_field_set(unit, ctr_reg, port, ctr_tbl_info_entry, &val64));
                } else {
                    return SOC_E_PARAM;
                }
            }
            else
#endif /* SOC_COUNTER_TABLE_SUPPORT */
            if (ctr_reg >= NUM_SOC_REG) {
                return SOC_E_PARAM;
            } else if (!(SOC_REG_INFO(unit, ctr_reg).flags & SOC_REG_FLAG_64_BITS) &&
                !SOC_IS_XGS3_SWITCH(unit)) {
                val32 = COMPILER_64_LO(val64);
                rv = soc_reg32_set(unit, ctr_reg, port, index, val32);
                if (SOC_FAILURE(rv)) {
                    goto counter_set_done;
                }
            } else {
                if (soc_feature(unit, soc_feature_prime_ctr_writes) &&
                    /*SOC_REG_INFO(unit, ctr_reg).block == SOC_BLK_EGR) {*/
                    SOC_BLOCK_IS(SOC_REG_INFO(unit, ctr_reg).block, SOC_BLK_EGR)) {
                    prime_ctr_write = TRUE;
                } else {
                    prime_ctr_write = FALSE;
                }

                if (prime_ctr_write) {
                    rv = soc_reg32_get(unit, ctr_reg, port, index, &val32);
                    if (SOC_FAILURE(rv)) {
                        goto counter_set_done;
                    }
                }
#ifdef BCM_TRIUMPH3_SUPPORT
                if (!skip_hw_set) {
#else
                {
#endif
                rv = soc_reg_set(unit, ctr_reg, port, index, val64);
                if (SOC_FAILURE(rv)) {
                    goto counter_set_done;
                }
                }
                if (prime_ctr_write) {
                    rv = soc_reg32_get(unit, ctr_reg, port, index, &val32);
                    if (SOC_FAILURE(rv)) {
                        goto counter_set_done;
                    }
                }
            }

            /* Check DMA counters (register) is read-only or not */
            if (SOC_REG_IS_VALID(unit, ctr_reg) &&
                SOC_REG_INFO(unit, ctr_reg).flags & SOC_REG_FLAG_RO) {
                ctr_is_readonly = 1;
            }
        }
#if defined(BCM_PETRA_SUPPORT)
    }
#endif
        /* The following section updates 64-bit values and must be atomic */
        COUNTER_ATOMIC_BEGIN(s);
        soc->counter_sw_val[port_index + index] = val64;
        /*
         * It checks if the counter is read-only. If so, only the soc->counter_sw_val is set and
         * soc->counter_hw_val is left untouched to match the hardware value.
         */
        if (!ctr_is_readonly) {
            soc->counter_hw_val[port_index + index] = val64;
        }
        COMPILER_64_SET(soc->counter_delta[port_index + index], 0, 0);
        COUNTER_ATOMIC_END(s);
        /*
          * For counter sync operation update the dma buffer
          * for the port index with the value read from hardware
          * counter register.
          */
         if ((ctr_reg != INVALIDr) &&
             (!SOC_COUNTER_INVALID(unit, ctr_reg)) &&
             (soc->counter_flags & SOC_COUNTER_F_DMA)) {
             int ctr_idx;
             uint32 *ptr;
             int port_base_dma, port_base;
             int  port_index, num_entries;
             char *cname;

             /* get index of counter register */
             SOC_IF_ERROR_RETURN(_soc_counter_get_info(unit, port, ctr_reg,
                                                       &port_index,
                                                       &num_entries,
                                                       &cname));
             port_base = COUNTER_MIN_IDX_GET(unit, port);
             port_base_dma = COUNTER_MIN_IDX_GET(unit,
                                                 port - soc->counter_ports32);
             ctr_idx = port_index - port_base;
#ifdef BCM_SBUSDMA_SUPPORT
             if (soc_feature(unit, soc_feature_sbusdma) &&
                 (ctr_reg < NUM_SOC_REG) &&
                 !SOC_REG_IS_64(unit, ctr_reg)) {
                 uint32 *buf32 ;
                     if (SOC_IS_TOMAHAWKX(unit)) {
                         if (ctr_idx < soc->counter_egr_first) {
                             buf32 = (uint32 *)&soc->counter_buf64[
                                                     port_base_dma +
                                                     soc->counter_ing_first];
                             ptr = &buf32[ctr_idx - soc->counter_ing_first];
                         } else {
                             buf32 = (uint32 *)&soc->counter_buf64[
                                                     port_base_dma +
                                                     soc->counter_egr_first];
                             ptr = &buf32[ctr_idx - soc->counter_egr_first];
                         }
                     } else {
                         if (ctr_idx < soc->counter_egr_first) {
                             buf32 = (uint32 *)&soc->counter_buf64[
                                                     port_base_dma +
                                                     soc->counter_ing_first];
                             ptr = &buf32[ctr_idx - soc->counter_ing_first];
                         } else if (ctr_idx < soc->counter_mac_first) {
                             buf32 = (uint32 *)&soc->counter_buf64[
                                                     port_base_dma +
                                                     soc->counter_egr_first];
                             ptr = &buf32[ctr_idx - soc->counter_egr_first];
                         } else {
                             buf32 = (uint32 *)&soc->counter_buf64[
                                                     port_base_dma +
                                                     soc->counter_mac_first];
                             ptr = &buf32[ctr_idx - soc->counter_mac_first];
                         }
                     }
                     COUNTER_ATOMIC_BEGIN(s);
                     if (soc_reg_field_length(
                                unit, ctr_reg, COUNTf) == 32) {
                         COMPILER_64_TO_32_LO(ptr[0], val64);
                     } else {
                         ptr[0] = COMPILER_64_LO(val64) &
                                     ((1 << soc_reg_field_length
                                             (unit, ctr_reg, COUNTf)) - 1);
                     }
                     COUNTER_ATOMIC_END(s);
             } else
#endif /* BCM_SBUSDMA_SUPPORT */
             {
                 ptr = (uint32 *)&soc->counter_buf64[port_base_dma + ctr_idx];
                 if (SOC_IS_TOMAHAWK3(unit) &&
                     SOC_REG_IS_COUNTER_TABLE(unit, ctr_reg) &&
                     soc->counter_mib_tbl_first > 0) {
                     /* For counter tables, dma_offset is obtained from SOC_CTR_TBL_INFO */
                     int dma_offset = SOC_CTR_TBL_INFO(unit)->tables[ctr_idx - soc->counter_mib_tbl_first].dma_offset;
                     ptr = (uint32 *)&soc->counter_buf64[port_base_dma + soc->counter_mib_tbl_first + dma_offset];
                 }
                 COUNTER_ATOMIC_BEGIN(s);
                 /* Update the DMA location */
                 if (soc->counter_flags & SOC_COUNTER_F_SWAP64) {
                     COMPILER_64_TO_32_HI(ptr[0], val64);
                     COMPILER_64_TO_32_LO(ptr[1], val64);
                 } else {
                     COMPILER_64_TO_32_LO(ptr[0], val64);
                     COMPILER_64_TO_32_HI(ptr[1], val64);
                 }
                 COUNTER_ATOMIC_END(s);
             }
         }/* ctr_reg != INVALIDr */
    }
 }
 counter_set_done:
    COUNTER_UNLOCK(unit);
    return rv;
}

int
soc_counter_set(int unit, soc_port_t port, soc_reg_t ctr_reg,
                int ar_idx, uint64 val)
{
    return _soc_counter_set(unit, port, ctr_reg, ar_idx, val);
}

int
soc_counter_set32(int unit, soc_port_t port, soc_reg_t ctr_reg,
                  int ar_idx, uint32 val)
{
    uint64              val64;

    COMPILER_64_SET(val64, 0, val);

    return _soc_counter_set(unit, port, ctr_reg, ar_idx, val64);
}

STATIC int
_soc_counter_generic_set(int unit, soc_ctr_control_info_t ctrl_info, soc_reg_t id,
        int ar_idx, uint64 val)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);
    int                 base_index, base_index_temp, num_entries,
                        entries_per_pipe = 0;
    int                 index_min, index_max, index;
    soc_counter_non_dma_t *non_dma;
    uint32              entry[SOC_MAX_MEM_FIELD_WORDS], fval[2];
    int                 dma_base_index = 0;
    soc_mem_t           dma_mem;
    COUNTER_ATOMIC_DEF  s = SOC_CONTROL(unit);
    int         rv = SOC_E_NONE;
    int                 prime_ctr_write, i;
    uint32              val32;
    soc_port_t port = -1 ;
    int subset_ct = 0, pipe = 0, pipe_start = 0, pipe_end = 0, all_pipe = 0;
    uint8 clear_on_read = 0;
    int field_len = 0;
    uint64 masked_val;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
#endif
    COMPILER_64_ZERO(masked_val);
    if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_PORT) {
        port = ctrl_info.instance;
        if (port < 0) {
            return SOC_E_PARAM;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit) &&
            SOC_CONTROL(unit)->halfchip == 1) {
            pipe = SOC_INFO(unit).port_pipe[port];
            if (pipe == 2 || pipe == 3 || pipe == 4 || pipe == 5) {
                return SOC_E_PARAM;
            }
        }
#endif
    }

    SOC_IF_ERROR_RETURN
        (soc_counter_generic_get_info(unit, ctrl_info, id,
                                      &base_index, &num_entries));
    base_index_temp = base_index;

    if ((ar_idx >= num_entries) || (ar_idx < -1)) {
        return SOC_E_PARAM;
    }

    if (ar_idx == -1) {
        index_min = 0;
        index_max = num_entries - 1;
    } else {
        index_min = index_max = ar_idx;
    }

    COUNTER_LOCK(unit);
    for (index = index_min; index <= index_max; index++) {
        if (id >= NUM_SOC_REG) {
            non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];
            if (non_dma->flags & (_SOC_COUNTER_NON_DMA_PEAK |
                        _SOC_COUNTER_NON_DMA_CURRENT)) {
                continue;
            }

            /* To iterate across pipes */
            if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_POOL_PIPE) {
                pipe = ctrl_info.instance & 0xF;
                if (pipe == -1) {
                    all_pipe = 1;
                } else if (pipe >= NUM_PIPE(unit)) {
                    return SOC_E_PARAM;
                }
            } else if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_POOL) {
                   all_pipe = 1;
            } else if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_PIPE) {
                pipe = ctrl_info.instance;
                if (pipe == -1) {
                    all_pipe = 1;
                } else if (pipe >= NUM_PIPE(unit)) {
                    return SOC_E_PARAM;
                }

                entries_per_pipe = num_entries;
            }

            if (all_pipe == 1) {
                pipe_start = 0;
                pipe_end = NUM_PIPE(unit) - 1;
            } else {
                pipe_start = pipe_end = pipe;
            }

            if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID) &&
                !(non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
                goto counter_set_done;
            } else if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                       (non_dma->extra_ctrs != NULL)) {
                /* If there is subset of counters, use subset info for DMA */
                non_dma = non_dma->extra_ctrs;

                if ((ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_POOL) ||
                    (ctrl_info.instance_type ==
                        SOC_CTR_INSTANCE_TYPE_POOL_PIPE)) {
                    /* Decoding for POOL_PIPE and multiply by 2 for
                     * parent-child model */
                    entries_per_pipe = num_entries;
                    if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_POOL) {
                        subset_ct = ctrl_info.instance << 1;
                    } else {
                        subset_ct = (ctrl_info.instance & 0x3F0) >> 3;
                    }
                } else if (ctrl_info.instance_type ==
                           SOC_CTR_INSTANCE_TYPE_ITM_PORT) {
                    subset_ct =
                        (ctrl_info.instance & CTR_INSTANCE_BIT_MASK_ITM) >>
                        CTR_INSTANCE_BIT_SHIFT_ITM;
                    entries_per_pipe = non_dma->num_entries;
                } else if (ctrl_info.instance_type ==
                           SOC_CTR_INSTANCE_TYPE_XPE_PORT) {
                    /* Decoding for XPE_PORT and multiply by 2 for parent-child
                     * model */
                    subset_ct = (ctrl_info.instance & 0x3C00) >> 9;
                    if ((non_dma->mem == MMU_CTR_COLOR_DROP_MEMm) && !SOC_IS_TOMAHAWK2(unit)) {
                        if (SOC_IS_TRIDENT3(unit)) {
                            entries_per_pipe = non_dma->num_entries / 2;
                        } else {
                            entries_per_pipe = non_dma->num_entries;
                        }
                    } else {
                        if (SOC_IS_HELIX5(unit) || SOC_IS_MAVERICK2(unit) ||
                            SOC_IS_HURRICANE4(unit) || SOC_IS_FIREBOLT6(unit)) {
                            entries_per_pipe = non_dma->num_entries;
                        } else {
                            entries_per_pipe = non_dma->num_entries / 2;
                        }
                    }
                } else if (ctrl_info.instance_type ==
                           SOC_CTR_INSTANCE_TYPE_SFLOW) {
                    subset_ct = ctrl_info.instance << 1;
                    entries_per_pipe = non_dma->num_entries;
                } else if (ctrl_info.instance_type ==
                           SOC_CTR_INSTANCE_TYPE_FT_GROUP) {
                    subset_ct = ctrl_info.instance << 1;
                    entries_per_pipe = non_dma->num_entries;

                } else
#ifdef BCM_APACHE_SUPPORT
                if (SOC_IS_APACHE(unit) &&
                            (ctrl_info.instance_type ==
                                SOC_CTR_INSTANCE_TYPE_PORT)) {
                    int obm_id, lane;
                    SOC_IF_ERROR_RETURN
                        (soc_apache_port_obm_info_get(unit, port,
                                                      &obm_id, &lane));
                    entries_per_pipe = non_dma->num_entries;
                    subset_ct = (obm_id << 1);
                } else
#endif
                {
                    entries_per_pipe = non_dma->num_entries;
                    if (non_dma->mem != INVALIDm) {
                        entries_per_pipe =
                            soc_mem_index_count(unit, non_dma->dma_mem[0]);
                    }
                }
                non_dma = non_dma + subset_ct;

                if (!non_dma ||
                    !(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
                    goto counter_set_done;
                }
            } else {
                if (non_dma->mem != INVALIDm) {
                    if ((non_dma->mem == MMU_CTR_COLOR_DROP_MEMm) &&
                        SOC_IS_TRIDENT3X(unit)) {
                        entries_per_pipe = (non_dma->num_entries)/2;
                    } else {
                        entries_per_pipe =
                            soc_mem_index_count(unit, non_dma->dma_mem[0]);
                    }
                } else {
                    entries_per_pipe = non_dma->num_entries;
                }
            }

            if (non_dma->flags & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ) {
                clear_on_read = 1;
            }
            if (non_dma->mem != INVALIDm) {
                /* If CLR_ON_READ is enabled for the Counter, perform only
                 * Software updation.
                 */
                /* Iterate across pipes for POOL and PIPE = -1 case */

                field_len = soc_mem_field_length(unit, non_dma->mem,
                                                 non_dma->field);
                if (field_len < 32) {
                    COMPILER_64_SET(masked_val, 0, ((1 << field_len) - 1));
                    COMPILER_64_AND(masked_val, val);
                } else {
                    COMPILER_64_SET(masked_val, ((1 << (field_len - 32)) - 1),
                                    0xffffffff);
                    COMPILER_64_AND(masked_val, val);
                }
                if (all_pipe) {
                    dma_base_index = base_index;
                    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                        /* Skip if no enabled ports in pipe */
                        if (SOC_PBMP_IS_NULL(PBMP_PIPE(unit, pipe))) {
                            continue;
                        }
                        dma_mem = non_dma->dma_mem[pipe];
                        rv = soc_mem_read(unit, dma_mem, MEM_BLOCK_ANY,
                                          index, entry);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }

                        if (clear_on_read) {
                            /* Intent is to set All pipe value to 0.
                             * Since Clear on read is true, reading clears the
                             * values in Hardware.
                             * Optimization - to not set the value to zero again.
                             */
                            continue;
                        }
                        if (pipe == 0) {
                            fval[0] = COMPILER_64_LO(masked_val);
                            fval[1] = COMPILER_64_HI(masked_val);
                        } else {
                            fval[0] = 0;
                            fval[1] = 0;
                        }
                        soc_mem_field_set(unit, dma_mem, entry,
                                non_dma->field, fval);

                        rv = soc_mem_write(unit, dma_mem, MEM_BLOCK_ALL,
                                           index, entry);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }

                        if(non_dma->dma_num_entries[pipe] != 0) {
                            base_index += non_dma->dma_num_entries[pipe];
                        } else {
                            base_index += entries_per_pipe;
                        }
                    }
                    base_index = base_index_temp;
                } else {
                    int pipe_offset = 0;
                    dma_mem = non_dma->dma_mem[0];
                    base_index = base_index_temp;

                    for (i = 0; i < NUM_PIPE(unit); i++) {
                        if (non_dma->dma_mem[i] != INVALIDm) {
                            dma_mem = non_dma->dma_mem[i];
                            dma_base_index = base_index - non_dma->base_index;
                            pipe_offset = dma_base_index / entries_per_pipe;

                            if (non_dma->dma_index_min[i]) {
                                dma_base_index += non_dma->dma_index_min[i];
                            }
                            break;
                        }
                    }
                    if (pipe_offset) {
                        for (; i < NUM_PIPE(unit); i++) {
                            if (non_dma->dma_mem[i] != INVALIDm) {
                                /* Choose the appropriate index where pipes are
                                 * applicable. In XPE case, each XPE is valid
                                 * for only pair of pipes(and can have holes in
                                 * between.
                                 */
                                if (pipe_offset) {
                                    pipe_offset--;
                                    continue;
                                } else {
                                    break;
                                }
                            }
                        }
                        i %= NUM_PIPE(unit);
                        dma_mem = non_dma->dma_mem[i];
                        dma_base_index = (dma_base_index % entries_per_pipe) +
                                         non_dma->dma_index_min[i];
                    }

                    rv = soc_mem_read(unit, dma_mem, MEM_BLOCK_ANY,
                            dma_base_index + index, entry);
                    if (SOC_FAILURE(rv)) {
                        goto counter_set_done;
                    }

                    if (!clear_on_read) {
                        /* Set Hardware only when Clear On Read is False.
                         * Else Hardware will be cleared by Read operation
                         * above.
                         */
                        fval[0] = COMPILER_64_LO(masked_val);
                        fval[1] = COMPILER_64_HI(masked_val);
                        soc_mem_field_set(unit, dma_mem, entry,
                                non_dma->field, fval);

                        /*
                         * If parity is not enabled on the device
                         * set the parity bit in the MMU_CTR_UC_DROP_MEMm
                         * during counter set as suggested by DE.
                         */

                        rv = soc_mem_write(unit, dma_mem, MEM_BLOCK_ALL,
                                dma_base_index + index, entry);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }
                    }
                }
            } else if (non_dma->reg != INVALIDr) {
                if (ctrl_info.instance_type ==
                           SOC_CTR_INSTANCE_TYPE_ITM_PORT) {
                    port =
                        (ctrl_info.instance & CTR_INSTANCE_BIT_MASK_ITM) >>
                        CTR_INSTANCE_BIT_SHIFT_ITM;
                    /* port means ITM instance# but not used here, where
                       base_index already reflect the ITM# offset */
                    entries_per_pipe = 0;
                    pipe_start = 0;
                    pipe_end = 0;
                    base_index = base_index_temp;
                }
                if (port < 0) {
                    rv = SOC_E_PARAM;
                    goto counter_set_done;
                }
                if ((non_dma->flags & _SOC_COUNTER_NON_DMA_OBM) &&
                    (soc_ctr_ctrl->port_obm_info_get)) {
                    /* Set for OBM based counters.
                     * Lossy_Lo, Lossy_Hi, Lossless0, Lossless1
                     */
                    uint64 regval64;
                    uint32 regval32;
                    int obm_id = 0, lane = 0;
                    soc_reg_t reg = INVALIDr;
                    soc_info_t *si = &SOC_INFO(unit);;

                    rv = soc_ctr_ctrl->port_obm_info_get(unit, port,
                                                         &obm_id, &lane);
                    if (SOC_FAILURE(rv)) {
                        goto counter_set_done;
                    }
#ifdef BCM_APACHE_SUPPORT
                    if (SOC_IS_APACHE(unit)) {
                        int pgw_inst = 0;

                        _soc_counter_apache_get_pgw_inst(unit, port, &pgw_inst, &lane);
                        rv = soc_reg_get(unit, non_dma->reg,
                                pgw_inst, lane, &regval64);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }
                        if (SOC_REG_IS_64(unit, non_dma->reg)) {
                            soc_reg64_field_set(unit, non_dma->reg, &regval64,
                                    non_dma->field, val);
                        } else {
                            regval32 = COMPILER_64_LO(regval64);
                            soc_reg_field_set(unit, non_dma->reg, &regval32,
                                    non_dma->field, COMPILER_64_LO(val));
                            COMPILER_64_SET(regval64, 0, regval32);
                        }
                        rv = soc_reg_set(unit, non_dma->reg,
                                pgw_inst, lane, regval64);

                    } else
#endif
                    {
                        pipe = si->port_pipe[port];
                        if (obm_id) {
                            subset_ct = obm_id << 1;
                            non_dma = non_dma + subset_ct;
                        }

                        reg = SOC_REG_UNIQUE_ACC(unit, non_dma->reg)[pipe];
                        rv = soc_reg_get(unit, reg, pipe, lane, &regval64);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }

                        if (SOC_REG_IS_64(unit, non_dma->reg)) {
                            soc_reg64_field_set(unit, non_dma->reg, &regval64,
                                    non_dma->field, val);
                        } else {
                            regval32 = COMPILER_64_LO(regval64);
                            soc_reg_field_set(unit, non_dma->reg, &regval32,
                                    non_dma->field, COMPILER_64_LO(val));
                            COMPILER_64_SET(regval64, 0, regval32);
                        }

                        rv = soc_reg_set(unit, reg, pipe, lane, regval64);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }
                    }
                } else {
                    if (PBMP_MEMBER(non_dma->pbmp, port)) {
                        uint64 regval64;
                        uint32 regval32;

                        rv = soc_reg_get(unit, non_dma->reg,
                                port, index, &regval64);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }
                        if (SOC_REG_IS_64(unit, non_dma->reg)) {
                            soc_reg64_field_set(unit, non_dma->reg, &regval64,
                                    non_dma->field, val);
                        } else {
                            regval32 = COMPILER_64_LO(regval64);
                            soc_reg_field_set(unit, non_dma->reg, &regval32,
                                    non_dma->field, COMPILER_64_LO(val));
                            COMPILER_64_SET(regval64, 0, regval32);
                        }
                        rv = soc_reg_set(unit, non_dma->reg,
                                port, index, regval64);
                        if (SOC_FAILURE(rv)) {
                            goto counter_set_done;
                        }
                    }
                }
            }
        } else {
            if (port < 0) {
                rv = SOC_E_PARAM;
                goto counter_set_done;
            }

            if (!(SOC_REG_INFO(unit, id).flags & SOC_REG_FLAG_64_BITS) &&
                    !SOC_IS_XGS3_SWITCH(unit)) {
                val32 = COMPILER_64_LO(val);
                rv = soc_reg32_set(unit, id, port, index, val32);
                if (SOC_FAILURE(rv)) {
                    goto counter_set_done;
                }
            } else {
                if (soc_feature(unit, soc_feature_prime_ctr_writes) &&
                        /*SOC_REG_INFO(unit, id).block == SOC_BLK_EGR) */
                    SOC_BLOCK_IS(SOC_REG_INFO(unit, id).block, SOC_BLK_EGR)) {
                    prime_ctr_write = TRUE;
                } else {
                    prime_ctr_write = FALSE;
                }

                if (prime_ctr_write) {
                    rv = soc_reg32_get(unit, id, port, index, &val32);
                    if (SOC_FAILURE(rv)) {
                        goto counter_set_done;
                    }
                }
                rv = soc_reg_set(unit, id, port, index, val);
                if (SOC_FAILURE(rv)) {
                    goto counter_set_done;
                }
                if (prime_ctr_write) {
                    rv = soc_reg32_get(unit, id, port, index, &val32);
                    if (SOC_FAILURE(rv)) {
                        goto counter_set_done;
                    }
                }
            }
        }

        for (pipe = pipe_start; pipe <= pipe_end; pipe++) {
            /* The following section updates 64-bit values and must be atomic */
            COUNTER_ATOMIC_BEGIN(s);
            soc->counter_sw_val[base_index + index] = val;
            soc->counter_hw_val[base_index + index] = val;
            COMPILER_64_SET(soc->counter_delta[base_index + index], 0, 0);
            COUNTER_ATOMIC_END(s);
            base_index += entries_per_pipe;

            /* When Set is called with value with Pipe = -1
             * Set given value for Pipe 0 and set other pipe values to 0
             */
            if (all_pipe) {
                COMPILER_64_SET(val, 0, 0);
            }
        }
    }

counter_set_done:
    COUNTER_UNLOCK(unit);
    return rv;

}

/*
 * Function:
 *      soc_counter_generic_set
 * Purpose:
 *      Given a counter register number, instance type, instance and a counter
 *      value, set both the hardware and software-accumulated counters to the
 *      value.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      ctrl_info - Counter Control Info Structure.
 *                  Contains the instance type and instance.
 *      id - counter register id to set.
 *      val - 64 bit counter value.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Currently supported instance types are
 *      1. SOC_CTR_INSTANCE_TYPE_PORT
 *      2. SOC_CTR_INSTANCE_TYPE_POOL
 *      3. SOC_CTR_INSTANCE_TYPE_PIPE
 */
int
soc_counter_generic_set(int unit, soc_reg_t id,
                        soc_ctr_control_info_t ctrl_info, uint32 flags,
                        int ar_idx, uint64 val)
{
    int xpe,i,port,pipe,rv=0;
    soc_ctr_control_info_t input_ctrl_info[2], output_ctrl_info;
    soc_info_t *si = &SOC_INFO(unit);
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_mem_t mem = INVALIDm;
#ifdef BCM_TOMAHAWK3_SUPPORT
    uint16 dev_id;
    uint8 rev_id;
#endif
    if (id < SOC_COUNTER_NON_DMA_START ||
        id >= SOC_COUNTER_NON_DMA_END) {
        return SOC_E_PARAM;
    }

    if (soc->counter_non_dma == NULL) {
        return SOC_E_RESOURCE;
    }

    if (!(soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START].flags)) {
        return SOC_E_UNAVAIL;
    }
    mem = soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START].mem;
    if ((ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_PIPE) &&
        (ctrl_info.instance == -1)) {
        for (i = 0; i < NUM_PIPE(unit) ; i++) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_PBMP_IS_NULL(si->pipe_pbm[i])) {
                continue;
            }
#endif
            ctrl_info.instance = i;
            rv =  _soc_counter_generic_set(unit, ctrl_info, id, ar_idx, val);
            if (rv < 0) {
                return rv;
            }
        }
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_ITM) {
        uint64 setval[2];
        int setval_iterate = 0, itm;
        port = ctrl_info.instance;
        pipe = si->port_pipe[port];

        input_ctrl_info[0].instance_type = ctrl_info.instance_type;
        input_ctrl_info[1].instance = ctrl_info.instance;
        input_ctrl_info[1].instance_type = SOC_CTR_INSTANCE_TYPE_PORT;

        if ((NUM_ITM(unit) == 1) || (dev_id == BCM56983_DEVICE_ID)) {
            COMPILER_64_COPY(setval[0], val);
        } else {
            _soc_distribute_u64(val, &setval[0], &setval[1]);
        }

        for (itm = 0; itm < NUM_ITM(unit) ; itm++) {
            if (itm == 1 && (dev_id == BCM56983_DEVICE_ID)) {
                continue;
            }
            input_ctrl_info[0].instance = itm;

            soc_counter_instance_encode(input_ctrl_info,2,&output_ctrl_info);
            rv = _soc_counter_generic_set(unit, output_ctrl_info, id,
                                          ar_idx, setval[setval_iterate]);
            setval_iterate++;
            if (rv != SOC_E_NONE) {
                break;
            }
        }

        return rv;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_XPE) {
        uint64 setval[2];
        int setval_iterate = 0;
        port = ctrl_info.instance;
        pipe = si->port_pipe[port];
        input_ctrl_info[0].instance_type = ctrl_info.instance_type;
        input_ctrl_info[1].instance = ctrl_info.instance;
        input_ctrl_info[1].instance_type = SOC_CTR_INSTANCE_TYPE_PORT;
        if (NUM_XPE(unit) == 1) {
            COMPILER_64_COPY(setval[0], val);
        } else {
            _soc_distribute_u64(val, &setval[0], &setval[1]);
        }

        for (xpe = 0; xpe < NUM_XPE(unit) ; xpe++) {
            input_ctrl_info[0].instance = xpe;
            if ((mem == MMU_CTR_COLOR_DROP_MEMm) && !SOC_IS_TOMAHAWK2(unit)) {
                if(!((si->epipe_xpe_map[pipe] >> xpe) & 1)) {
                    continue;
                }

            } else {
                if (SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit,mem,xpe,pipe) == INVALIDm) {
                    /* Invalid XPE/Pipe Combination */
                    continue;
                }
            }
            soc_counter_instance_encode(input_ctrl_info,2,&output_ctrl_info);
            rv = _soc_counter_generic_set(unit, output_ctrl_info, id,
                                          ar_idx, setval[setval_iterate]  );
            setval_iterate++;
            if (rv < 0) {
                return rv;
            }
        }
        return rv;
    } else if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_SFLOW &&
               ctrl_info.instance == -1) {
       for (i = 0; i < SOC_SFLOW_CTR_TYPE_COUNT ; i++) {
           ctrl_info.instance = i;
           rv = _soc_counter_generic_set(unit, ctrl_info, id, ar_idx, val);
           if (rv < 0) {
               return rv;
            }
       }
       return rv;
    } else {
        return _soc_counter_generic_set(unit, ctrl_info, id, ar_idx, val);
    }
}

/* Function:
 *      soc_counter_generic_multi_set
 * Purpose:
 *      Given a counter register number, count, instance type, instance and
 *      array of values, set multiple hardware and software-accumulated counter
 *      values to values specified in input val array.
 * Parameter:
 *      unit  - StrataSwitch Unit #.
 *      ctr_reg - Counter Register to retrieve.
 *      ctrl_info - Counter Control Info Structure
 *                  Contains the instance type and instance.
 *      flags - not yet implemented.
 *      ar_idx - Base index of counter to be set.
 *      count - Number of consecutive entries to be set.
 *      val - Base address of array holding the values to be set in
 *      counters.
 *Returns:
 *      SOC_E_XXX
 *Notes:
 *      If count is set to 0, nothing is set.
*/
int
soc_counter_generic_multi_set(int unit,  soc_reg_t ctr_reg,
                              soc_ctr_control_info_t ctrl_info,
                              uint32 flags, int ar_idx, int count,
                              uint64 *val)
{
    int i;
    int rv = SOC_E_NONE;
    soc_control_t *soc = SOC_CONTROL(unit);

    if (count < 0) {
        return SOC_E_PARAM;
    }

    if (soc->counter_non_dma == NULL) {
        return SOC_E_RESOURCE;
    }

    for (i = 0; i<count; i++) {
        rv = _soc_counter_generic_set(unit, ctrl_info, ctr_reg, (ar_idx+i),
                                        val[i]);
        if (rv!=SOC_E_NONE) {
            return rv;
        }
    }
    return rv;
}

STATIC int
_soc_counter_non_dma_is_invalid(int unit, soc_reg_t reg, soc_port_t port)
{
    int pgw_index = -1, xlp = 0, xlp_index, blk;

    switch (reg) {
    case PGW_OBM0_HIGH_PRI_PKT_DROPr:
    case PGW_OBM0_LOW_PRI_PKT_DROPr:
    case PGW_OBM0_HIGH_PRI_BYTE_DROPr:
    case PGW_OBM0_LOW_PRI_BYTE_DROPr:
        xlp = 0;
        break;
    case PGW_OBM1_HIGH_PRI_PKT_DROPr:
    case PGW_OBM1_LOW_PRI_PKT_DROPr:
    case PGW_OBM1_HIGH_PRI_BYTE_DROPr:
    case PGW_OBM1_LOW_PRI_BYTE_DROPr:
        xlp = 1;
        break;
    case PGW_OBM2_HIGH_PRI_PKT_DROPr:
    case PGW_OBM2_LOW_PRI_PKT_DROPr:
    case PGW_OBM2_HIGH_PRI_BYTE_DROPr:
    case PGW_OBM2_LOW_PRI_BYTE_DROPr:
        xlp = 2;
        break;
    case PGW_OBM3_HIGH_PRI_PKT_DROPr:
    case PGW_OBM3_LOW_PRI_PKT_DROPr:
    case PGW_OBM3_HIGH_PRI_BYTE_DROPr:
    case PGW_OBM3_LOW_PRI_BYTE_DROPr:
        xlp = 3;
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
        if (IS_RDB_PORT(unit, port) || IS_MACSEC_PORT(unit,port)) {
            return TRUE;
        }
        break;
    default:
        return FALSE;
    }

    pgw_index = SOC_INFO(unit).port_group[port];
    if (pgw_index == -1) {
        return FALSE;
    }
    xlp_index = pgw_index * 4 + xlp;
    for (blk = 0; SOC_BLOCK_TYPE(unit, blk) >= 0; blk++) {
        if (SOC_BLOCK_TYPE(unit, blk) != SOC_BLK_XLPORT) {
            continue;
        }
        if (SOC_BLOCK_NUMBER(unit, blk) == xlp_index) {
            if (!SOC_INFO(unit).block_valid[blk]) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

/*
 * Function:
 *      _soc_counter_set_by_port
 * Purpose:
 *      Given a port bit map, set all counters for the specified ports to
 *      the requested value.  Useful mainly to clear all the counters.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pbmp  - Bit map of all ports to set counters on.
 *      ctr_reg - if !INVALIDr set only a specified counter
 *                else all the counters for a port
 *      val  - 64/32-bit counter value.
 * Returns:
 *      SOC_E_XXX
 */

int
_soc_counter_set_by_port(int unit, pbmp_t pbmp, soc_reg_t ctr_reg,
                         int ar_idx, uint64 val)
{
    int                 i, rv = 0;
    soc_port_t          port;
    soc_ctr_ref_t       *ctr_ref;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *non_dma = NULL;
    int                 min_index = 0;
    int                 max_ctr = 0;
    int                 num_idx = 0;
    int                 dma_id = 0;
    int                 non_dma_counter_end = 0;
    int                 port_base;
    int                 port_index, num_entries;
    char                *cname;
    int                 dma_counters = 1;
    int                 non_dma_counters = 1;

    PBMP_ITER(pbmp, port) {
        /*
         * Flexing operations on certain devices can create new ports,
         * or delete existing ports. This can happen when the counter
         * thread is not running. So we make a check here before proceeding
         * with further processing. We ignore processing for ports which no
         * longer exist.
         */
        if (!SOC_CONTROL(unit)->counter_map[port]) {
            continue;
        }

if (IS_MACSEC_PORT(unit, port)) {
    continue;
}
        if (!SOC_PORT_VALID(unit, port)) {
            continue;
        }

        /*
         * If ctr_reg != INVALIDr the request is for
         * setting a specific counter register for
         * a port.
         */
        if (ctr_reg != INVALIDr) {
            if ((ctr_reg < NUM_SOC_REG) ||
                SOC_REG_IS_COUNTER_TABLE(unit, ctr_reg)) {
                /* No DMA counters for the loopback port */
                if (IS_LB_PORT(unit, port)
#if defined(BCM_SHADOW_SUPPORT)
                    || IS_IL_PORT(unit, port)
#endif
                ) {
                    continue;
                }
                SOC_IF_ERROR_RETURN(
                        _soc_counter_get_info(unit, port, ctr_reg,
                                              &port_index, &num_entries,
                                              &cname));

                port_base = COUNTER_IDX_PORTBASE(unit, port);
                min_index = port_index - port_base;
                max_ctr = min_index + 1;
                non_dma_counters = 0;

                dma_id = 0;
                non_dma_counter_end = 0;
            } else {
                /*
                 * if ctr_reg >= NUM_SOC_REG, set stats for a
                 * specific port and queue in the hardware
                 * and the sw copy of the same.
                 */
                min_index = 0;
                max_ctr = 0;
                dma_counters = 0;

                dma_id = ctr_reg - SOC_COUNTER_NON_DMA_START;
                non_dma_counter_end = dma_id + 1;

            }
        } else {
            /*
             * The request is for setting all the
             * counter registers for a port.
             */
            dma_id = 0;
            non_dma_counter_end = SOC_COUNTER_NON_DMA_END -
                                  SOC_COUNTER_NON_DMA_START;

            /* No DMA counters for the loopback port */
            if (IS_LB_PORT(unit, port) ||
                IS_RDB_PORT(unit, port) ||
                IS_MACSEC_PORT(unit, port)
#if defined(BCM_SHADOW_SUPPORT)
                || IS_IL_PORT(unit, port)
#endif
               ) {
                min_index = 0;
                max_ctr =  0;
            } else {
                min_index = 0;
                max_ctr =  PORT_CTR_NUM(unit, port);
            }
        }


        if (!IS_LB_PORT(unit, port)
            && !IS_RDB_PORT(unit, port)
            && !IS_MACSEC_PORT(unit,port)
#if defined(BCM_SHADOW_SUPPORT)
            && !IS_IL_PORT(unit, port)
#endif
        )
        {

            if (dma_counters ) {
                /* No DMA counters for the loopback port */
                for (i = min_index; i < max_ctr; i++) {
                    ctr_ref = PORT_CTR_REG(unit, port, i);
                    num_idx = (ctr_reg == INVALIDr)? ctr_ref->index: ar_idx;
                    if (!SOC_COUNTER_INVALID(unit, ctr_ref->reg)) {
                        if (!SOC_REG_IS_COUNTER_TABLE(unit,ctr_ref->reg) &&
                            !SOC_REG_PORT_VALID(unit,ctr_ref->reg, port)) {
                            continue;
                        }

                        rv = _soc_counter_set(unit, port, ctr_ref->reg,
                                              num_idx, val);

                        if (rv == SOC_E_FAIL) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                              "Error %d calling "
                                              "soc_counter_set for"
                                              "port: %d, reg: %d, "
                                              "index: %d\n"), rv,
                                   port, ctr_ref->reg, ctr_ref->index));
                        }
                    }
                }
            }/* dma_counters */
       }

       if (non_dma_counters) {
            for (i = dma_id; i < non_dma_counter_end ; i++) {
                 non_dma = &soc->counter_non_dma[i];
                /* Clear counters should not clear Non Port based counters (like
                 * Flex and Field counters) - they are to be cleared by the
                 * corresponding module */
                if (SOC_PBMP_IS_NULL(non_dma->pbmp)) {
                    continue;
                }

	            if (_soc_counter_non_dma_is_invalid(unit, non_dma->reg, port)) {
	                continue;
	            }

if (!IS_MACSEC_PORT(unit, port)) {
#if defined (BCM_APACHE_SUPPORT)
                  if (SOC_IS_APACHE(unit)) {
                      if (((SOC_COUNTER_NON_DMA_START + i) == SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT)
                            || ((SOC_COUNTER_NON_DMA_START + i) == SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT) ) {
                         continue;
                      }
                  }
#endif
                rv = _soc_counter_set(unit, port,
                                      SOC_COUNTER_NON_DMA_START + i, -1,
                                      val);
}
                if (rv == SOC_E_FAIL) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Error %d calling soc_counter_set "
                                          "for port: %d, "
                                           "non-DMA index: %d\n"),
                                           rv, port, i));
                }
            }
        }
#ifdef BCM_BRADLEY_SUPPORT
        /*
         * The HOLD register is not supported by DMA on HUMV/Bradley,
         * so we cannot use the standard counter_set function.
         */
        if (SOC_IS_HBX(unit)) {
            if (SOC_REG_IS_VALID(unit, HOLD_Xr) &&
                SOC_REG_IS_VALID(unit, HOLD_Yr)) {
            SOC_IF_ERROR_RETURN
                (WRITE_HOLD_Xr(unit, port, COMPILER_64_LO(val) / 2));
            SOC_IF_ERROR_RETURN
                (WRITE_HOLD_Yr(unit, port, COMPILER_64_LO(val) / 2));
            }
        }
#endif
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_counter_clear_by_port_reg
 * Purpose:
 *      Given a port,counter register.  clear counters for the specified ports
 *      Useful mainly to clear individual snmp counters.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Bit map of all ports to set counters on.
 *      ctr_reg - counter register to clear.
 *      val  - 64bit counter value(set to 0).
 * Returns:
 *      SOC_E_XXX
 */

int
soc_counter_clear_by_port_reg(int unit, soc_port_t port, soc_reg_t ctr_reg,
                            int ar_idx, uint64 val)
{
    pbmp_t      pbmp;

    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_PORT_SET(pbmp, port);
    COMPILER_64_ZERO(val);
    return _soc_counter_set_by_port(unit, pbmp, ctr_reg, ar_idx, val);
}


/*
 * Function:
 *      soc_counter_set_by_port, soc_counter_set32_by_port
 * Purpose:
 *      Given a port bit map, set all counters for the specified ports to
 *      the requested value.  Useful mainly to clear all the counters.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      pbm  - Bit map of all ports to set counters on.
 *      val  - 64/32-bit counter value.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The 32-bit version sets the upper 32-bits of the 64-bit
 *      software counters to zero.  The specified value is truncated
 *      to the size of the hardware register when storing to hardware.
 */

int
soc_counter_set_by_port(int unit, pbmp_t pbmp, uint64 val)
{
    return _soc_counter_set_by_port(unit, pbmp, INVALIDr, 0, val);
}

int
soc_counter_set32_by_port(int unit, pbmp_t pbmp, uint32 val)
{
    uint64              val64;

    COMPILER_64_SET(val64, 0, val);

    return soc_counter_set_by_port(unit, pbmp, val64);
}

int
soc_controlled_counter_clear(int unit, soc_port_t port)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    uint64              ctr_new;
    int                 index, port_base;
    soc_controlled_counter_t* ctr;

    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit,
                            "soc_controlled_counter_clear: unit=%d "
                            "port=%d\n"), unit, port));

    if ((!soc_feature(unit, soc_feature_controlled_counters)) ||
         (!soc->controlled_counters)) {
        return SOC_E_NONE;
    }

    port_base = COUNTER_IDX_PORTBASE(unit, port);

    COUNTER_LOCK(unit);

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit) && !SOC_IS_JERICHO(unit)) {
        SOC_IF_ERROR_RETURN(soc_arad_stat_clear_on_read_set(unit , 1));
    }
#endif
#ifdef BCM_JERICHO_SUPPORT
    /*Jericho collected counters are not cleared on read*/
    if (SOC_IS_JERICHO(unit)) {
        SOC_IF_ERROR_RETURN(soc_jer_nif_controlled_counter_clear(unit, port));
    }
#endif
    for (index = 0; soc->controlled_counters[index].controlled_counter_f != NULL; index++) {
        COUNTER_ATOMIC_DEF s = SOC_CONTROL(unit);
        uint32 clear_on_read;

        ctr = &soc->controlled_counters[index];
        if (ctr->controlled_counter_clear_f != NULL) {
            ctr->controlled_counter_clear_f(unit, ctr->counter_id, port);
        } else {
            /* assume clear on read */
            ctr->controlled_counter_f(unit, ctr->counter_id, port, &ctr_new, &clear_on_read);
            if(!clear_on_read) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "Counter Id(%d) is not cleared!\n"), ctr->counter_id));
            }
        }
        /*check if counter is relevant*/
        if(!COUNTER_IS_COLLECTED(soc->controlled_counters[index])) {
            continue;
        }
        /* The following section updates 64-bit values and must be atomic */
        COUNTER_ATOMIC_BEGIN(s);
        COMPILER_64_SET(soc->counter_sw_val[port_base + ctr->counter_idx], 0, 0);
        COMPILER_64_SET(soc->counter_hw_val[port_base + ctr->counter_idx], 0, 0);
        COMPILER_64_SET(soc->counter_delta[port_base + ctr->counter_idx], 0, 0);
        COUNTER_ATOMIC_END(s);

    }

#ifdef BCM_PETRA_SUPPORT
        if (SOC_IS_ARAD(unit) && !SOC_IS_JERICHO(unit)) {
            SOC_IF_ERROR_RETURN(soc_arad_stat_clear_on_read_set(unit , 0));
        }
#endif

    COUNTER_UNLOCK(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_counter_set_by_reg, soc_counter_set32_by_reg
 * Purpose:
 *      Given a counter register number and a counter value, set both
 *      the hardware and software-accumulated counters to the value
 *      for all ports.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      ctr_reg - counter register to set.
 *      val - 64/32-bit counter value.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The 32-bit version sets the upper 32-bits of the 64-bit
 *      software counters to zero.  The specified value is truncated
 *      to the size of the hardware register when storing to hardware.
 */

int
soc_counter_set_by_reg(int unit, soc_reg_t ctr_reg, int ar_idx, uint64 val)
{
    soc_port_t          port;

    if (SOC_COUNTER_INVALID(unit, ctr_reg)) {
        return SOC_E_NONE;
    }

#ifdef BROADCOM_DEBUG
    if (!SOC_REG_IS_COUNTER(unit, ctr_reg)) {
        _soc_counter_illegal(unit, ctr_reg);
        return SOC_E_NONE;
    }
#endif /* BROADCOM_DEBUG */


    PBMP_PORT_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(_soc_counter_set(unit, port, ctr_reg,
                                             ar_idx, val));
    }

    return SOC_E_NONE;
}

int
soc_counter_set32_by_reg(int unit, soc_reg_t ctr_reg, int ar_idx, uint32 val)
{
    uint64              val64;

    COMPILER_64_SET(val64, 0, val);

    return soc_counter_set_by_reg(unit, ctr_reg, ar_idx, val64);
}

/* Function:
 *      soc_counter_config_multi_get
 * Purpose:
 *      Given a counter register number, count, instance type, instance...
 * Parameter:
 *      unit        - StrataSwitch Unit #.
 *      ctr_reg     - Counter Register to retrieve.
 *      count       - Number of consecutive entries to be set.
 *      ctrl_info   - Counter Control Info Structure
 *                    Contains the instance type and instance.
 *      ctrl_config - Counter Control config structure to Get Dma attributes.
 *                    Refer soc_ctr_ctrl_config_s.
 *Returns:
 *      SOC_E_XXX
 *Notes:
 *      If count is set to 0, nothing is retrieved.
*/
int
soc_counter_config_multi_get(int unit, soc_reg_t ctr_reg, int count,
                             soc_ctr_control_info_t *ctrl_info,
                             soc_ctr_ctrl_config_t *ctrl_config)
{
    int i;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *non_dma = NULL;
    soc_counter_non_dma_t *parent_dma = NULL;
    int value = -1, config_type = 0;
    int instance_type = -1, in_pipe = -1;
    int c_offset, rv = SOC_E_NONE;
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);

    if (soc_ctr_ctrl == NULL) {
        return SOC_E_INIT;
    }

    if ((count < 0) || (!ctrl_config) || (!ctrl_info)) {
        return SOC_E_PARAM;
    }

    if (soc->counter_non_dma == NULL) {
        return SOC_E_RESOURCE;
    }

    if ((ctr_reg < SOC_COUNTER_NON_DMA_START) ||
        (ctr_reg >= SOC_COUNTER_NON_DMA_END)) {
        return SOC_E_PARAM;
    }

    for (i = 0; i < count; i++) {
        /* Check sanity of Ctrl_info */
        instance_type = ctrl_info[i].instance_type;

        if (!((instance_type == SOC_CTR_INSTANCE_TYPE_PIPE) ||
             (instance_type == SOC_CTR_INSTANCE_TYPE_POOL) ||
             (instance_type == SOC_CTR_INSTANCE_TYPE_POOL_PIPE))) {
            return SOC_E_PARAM;
        }

        parent_dma = &soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START];
        non_dma = parent_dma;

        if (soc_ctr_ctrl->get_child_dma) {
            rv = soc_ctr_ctrl->get_child_dma(unit, ctr_reg,
                                             ctrl_info[i], &non_dma);
            if (rv != SOC_E_NONE) {
                return rv;
            }
        }

        switch (instance_type) {
            case SOC_CTR_INSTANCE_TYPE_PIPE:
                in_pipe = ctrl_info[i].instance;
                if ((in_pipe >=  NUM_PIPE(unit)) || (in_pipe < -1)) {
                   return SOC_E_PARAM;
                }
                break;
            case SOC_CTR_INSTANCE_TYPE_POOL:
                c_offset = ctrl_info[i].instance;
                if ((c_offset < 0) || (c_offset >= parent_dma->extra_ctr_ct)) {
                    return SOC_E_PARAM;
                }
                in_pipe = -1;
                break;
            case SOC_CTR_INSTANCE_TYPE_POOL_PIPE:
                return SOC_E_UNAVAIL;
            /* coverity[dead_error_begin] */
            default:
                return SOC_E_PARAM;
        }

        config_type = ctrl_config[i].config_type;
        value = -1;
        switch (config_type) {
            case SOC_CTR_CTRL_CONFIG_DMA_ENABLE:
                value = (non_dma->flags & _SOC_COUNTER_NON_DMA_VALID);
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_MINIDX:
                if (in_pipe == -1) {
                    in_pipe = 0;
                }
                value = non_dma->dma_index_min[in_pipe];
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_MAXIDX:
                if (in_pipe == -1) {
                    in_pipe = 0;
                }
                value = non_dma->dma_index_max[in_pipe];
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_ALL:
                if (non_dma->dma_rate_profile == _SOC_COUNTER_DMA_RATE_PROFILE_MASK) {
                    value = 1;
                } else {
                    value = 0;
                }
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_AUTO:
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_WFS:
                return SOC_E_UNAVAIL;
                break;
            default:
                return SOC_E_PARAM;
        }
        ctrl_config[i].config_val = value;
    }
    return rv;
}

/* Function:
 *      soc_counter_config_multi_set
 * Purpose:
 *      Given a counter register number, count, instance type, instance...
 * Parameter:
 *      unit        - StrataSwitch Unit #.
 *      ctr_reg     - Counter Register to retrieve.
 *      count       - Number of consecutive entries to be set.
 *      ctrl_info   - Counter Control Info Structure
 *                    Contains the instance type and instance.
 *      ctrl_config - Counter Control config structure to set Dma attributes.
 *                    Refer soc_ctr_ctrl_config_s.
 *Returns:
 *      SOC_E_XXX
 *Notes:
 *      If count is set to 0, nothing is set.
*/
int
soc_counter_config_multi_set(int unit, soc_reg_t ctr_reg, int count,
                             soc_ctr_control_info_t *ctrl_info,
                             soc_ctr_ctrl_config_t *ctrl_config)
{
    int i, max_idx = 0;
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *non_dma = NULL;
    soc_counter_non_dma_t *parent_dma = NULL;
    int value = -1, config_type = 0;
    int instance_type = -1, in_pipe = -1, pipe = -1;
    int c_offset, rv = SOC_E_NONE;
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);

    if (soc_ctr_ctrl == NULL) {
        return SOC_E_INIT;
    }

    if ((count < 0) || (!ctrl_config) || (!ctrl_info)) {
        return SOC_E_PARAM;
    }

    if ((ctr_reg < SOC_COUNTER_NON_DMA_START) ||
        (ctr_reg >= SOC_COUNTER_NON_DMA_END)) {
        return SOC_E_PARAM;
    }

    if (soc->counter_non_dma == NULL) {
        return SOC_E_RESOURCE;
    }

    for (i = 0; i < count; i++) {
        /* Check sanity of Ctrl_info */
        instance_type = ctrl_info[i].instance_type;

        if (!((instance_type == SOC_CTR_INSTANCE_TYPE_PIPE) ||
             (instance_type == SOC_CTR_INSTANCE_TYPE_POOL) ||
             (instance_type == SOC_CTR_INSTANCE_TYPE_POOL_PIPE))) {
            return SOC_E_PARAM;
        }

        parent_dma = &soc->counter_non_dma[ctr_reg - SOC_COUNTER_NON_DMA_START];
        non_dma = parent_dma;
        /* Get child dma if this falls under Parent-child model. */
        if (soc_ctr_ctrl->get_child_dma) {
            rv = soc_ctr_ctrl->get_child_dma(unit, ctr_reg,
                                             ctrl_info[i], &non_dma);
        }
        max_idx = non_dma->num_entries / NUM_PIPE(unit);

        switch (instance_type) {
            case SOC_CTR_INSTANCE_TYPE_PIPE:
                in_pipe = ctrl_info[i].instance;
                if ((in_pipe >=  NUM_PIPE(unit)) || (in_pipe < -1)) {
                   return SOC_E_PARAM;
                }
                break;
            case SOC_CTR_INSTANCE_TYPE_POOL:
                c_offset = ctrl_info[i].instance;
                if ((c_offset < 0) || (c_offset >= parent_dma->extra_ctr_ct)) {
                    return SOC_E_PARAM;
                }
                in_pipe = -1;
                break;
            case SOC_CTR_INSTANCE_TYPE_POOL_PIPE:
                return SOC_E_UNAVAIL;
            /* coverity[dead_error_begin] */
            default:
                return SOC_E_PARAM;
        }

        value = ctrl_config[i].config_val;
        config_type = ctrl_config[i].config_type;

        switch (config_type) {
            case SOC_CTR_CTRL_CONFIG_DMA_ENABLE:
                if (value) {
                    non_dma->flags |= _SOC_COUNTER_NON_DMA_VALID;
                    if ((parent_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                        (!(parent_dma->flags & _SOC_COUNTER_NON_DMA_VALID))  &&
                        (ctr_reg != SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE) &&
                        (ctr_reg != SOC_COUNTER_NON_DMA_ING_FLEX_BYTE)) {
                        parent_dma->flags |= _SOC_COUNTER_NON_DMA_VALID;
                    }
                } else {
                    non_dma->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
                }
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_MINIDX:
                if ((value < 0) || (value > max_idx)) {
                    return SOC_E_PARAM;
                }
                if (in_pipe == -1) {
                    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                        non_dma->dma_index_min[pipe] = value;
                    }
                } else {
                    non_dma->dma_index_min[in_pipe] = value;
                }
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_MAXIDX:
                if ((value < 0) || (value > max_idx)) {
                    return SOC_E_PARAM;
                }
                if (in_pipe == -1) {
                    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                        non_dma->dma_index_max[pipe] = value;
                    }
                } else {
                    non_dma->dma_index_max[in_pipe] = value;
                }
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_ALL:
                if (soc_ctr_ctrl->dma_config_update) {
                    rv = soc_ctr_ctrl->dma_config_update(unit, non_dma,
                                                         config_type, value);
                }
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_AUTO:
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_WFS:
                return SOC_E_UNAVAIL;
                break;
            default:
                return SOC_E_PARAM;
        }
    }
    return rv;
}

/*
 * Function:
 *      soc_counter_collect32
 * Purpose:
 *      This routine gets called each time the counter transfer has
 *      completed a cycle.  It collects the 32-bit counters.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      discard - If TRUE, the software counters are not updated; this
 *              results in only synchronizing the previous hardware
 *              count buffer.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Computes the deltas in the hardware counters from the last
 *      time it was called and adds them to the high resolution (64-bit)
 *      software counters in counter_sw_val[].  It takes wrapping into
 *      account for counters that are less than 32 bits wide.
 *      It also computes counter_delta[].
 */

STATIC int
soc_counter_collect32(int unit, int discard)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_port_t          port;
    soc_reg_t           ctr_reg;
    soc_ctr_ref_t       *ctr_ref;
    uint32              ctr_new, ctr_prev, ctr_diff;
    int                 index;
    int                 port_index;
    int                 dma;
    int                 recheck_cntrs;
    int                 ar_idx;

    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit,
                            "soc_counter_collect32: unit=%d discard=%d\n"),
                 unit, discard));

    dma = ((soc->counter_flags & SOC_COUNTER_F_DMA) &&
           !discard);

    recheck_cntrs = soc_feature(unit, soc_feature_recheck_cntrs);

    PBMP_ITER(soc->counter_pbmp, port) {
        /*
         * Flexing operations on certain devices can create new ports,
         * or delete existing ports. This can happen when the counter
         * thread is not running. So we make a check here before proceeding
         * with further processing. We ignore processing for ports which no
         * longer exist.
         */
        if (!SOC_CONTROL(unit)->counter_map[port]) {
            continue;
        }

        if ((IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) &&
            (!SOC_IS_XGS3_SWITCH(unit))) {
                continue;
        }
#if defined(BCM_SHADOW_SUPPORT)
        if (SOC_IS_SHADOW(unit) && IS_IL_PORT(unit, port)) {
                continue;
        }
#endif /* BCM_SHADOW_SUPPORT */


        port_index = COUNTER_MIN_IDX_GET(unit, port);

        for (index = 0; index < PORT_CTR_NUM(unit, port);
             index++, port_index++) {
            volatile uint64 *vptr;
            COUNTER_ATOMIC_DEF s = SOC_CONTROL(unit);

            ctr_ref = PORT_CTR_REG(unit, port, index);
            ctr_reg = ctr_ref->reg;
            ar_idx = ctr_ref->index;

            if (SOC_COUNTER_INVALID(unit, ctr_reg)) {
                continue;
            }
            if (!SOC_REG_PORT_VALID(unit,ctr_reg, port)) {
                continue;
            }

            ctr_prev = COMPILER_64_LO(soc->counter_hw_val[port_index]);

            if (dma) {
                ctr_new = soc->counter_buf32[port_index];
            } else {                   /* Not DMA.  Just read register */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, ctr_reg, port, ar_idx,
                                   &ctr_new));
            }

            if ( (recheck_cntrs == TRUE) && (ctr_new != ctr_prev) ) {
                /* Seeds of doubt, double-check */
                uint32 ctr_new2;
                int suspicious = 0;
                SOC_IF_ERROR_RETURN
                  (soc_reg32_get(unit, ctr_reg, port, 0, &ctr_new2));

                /* Check for partial ordering, with wrap */
                if (ctr_new < ctr_prev) {
                    if ((ctr_new2 < ctr_new) || (ctr_new2 > ctr_prev)) {
                        /* prev < new2 < new, bad data */
                        /* Try again next time */
                        ctr_new = ctr_prev;
                        suspicious = 1;
                    }
                } else {
                    if ((ctr_new2 < ctr_new) && (ctr_new2 > ctr_prev)) {
                        /* prev < new2 < new, bad data */
                        /* Try again next time */
                        ctr_new = ctr_prev;
                        suspicious = 1;
                    }
                }
                /* Otherwise believe ctr_new */

                if (suspicious) {
#if !defined(SOC_NO_NAMES)
                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit,
                                         "soc_counter_collect32: "
                                         "unit %d, port%d: suspicious %s "
                                         "counter read (%s)\n"),
                              unit, port,
                              dma?"DMA":"manual",
                              SOC_REG_NAME(unit, ctr_reg)));
#else
                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit,
                                         "soc_counter_collect32: "
                                         "unit %d, port%d: suspicious %s "
                                         "counter read (Counter%d)\n"),
                              unit, port,
                              dma?"DMA":"manual",
                              ctr_reg));
#endif /* SOC_NO_NAMES */
                } /* Suspicious counter change */
            } /* recheck_cntrs == TRUE */

            if (ctr_new == ctr_prev) {
                COUNTER_ATOMIC_BEGIN(s);
                COMPILER_64_SET(soc->counter_delta[port_index], 0, 0);
                COUNTER_ATOMIC_END(s);
                continue;
            }

            if (discard) {
                COUNTER_ATOMIC_BEGIN(s);
                /* Update the DMA location */
                soc->counter_buf32[port_index] = ctr_new;
                /* Update the previous value buffer */
                COMPILER_64_SET(soc->counter_hw_val[port_index], 0, ctr_new);
                COMPILER_64_SET(soc->counter_delta[port_index], 0, 0);
                COUNTER_ATOMIC_END(s);
                continue;
            }

            LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                     (BSL_META_U(unit,
                                 "soc_counter_collect32: ctr %d => %u\n"),
                      port_index, ctr_new));

            vptr = &soc->counter_sw_val[port_index];

            ctr_diff = ctr_new;

            if (ctr_diff < ctr_prev) {
                int             width, i = 0;

                /*
                 * Counter must have wrapped around.  Add the proper
                 * wrap-around amount.  Full 32-bit wide counters do not
                 * need anything added.
                 */
                width = SOC_REG_INFO(unit, ctr_reg).fields[0].len;
                if (soc_feature(unit, soc_feature_counter_parity)
#ifdef BCM_HURRICANE2_SUPPORT
                    || soc_reg_field_valid(unit, ctr_reg, PARITYf)
#endif
                   )
                {
                    while((SOC_REG_INFO(unit, ctr_reg).fields + i) != NULL) {
                        if (SOC_REG_INFO(unit, ctr_reg).fields[i].field ==
                            COUNTf) {
                            width = SOC_REG_INFO(unit, ctr_reg).fields[i].len;
                            break;
                        }
                        i++;
                    }
                }
                if (width < 32) {
                    ctr_diff += (1UL << width);
                }
            }

            ctr_diff -= ctr_prev;

            COUNTER_ATOMIC_BEGIN(s);
            COMPILER_64_ADD_32(*vptr, ctr_diff);
            COMPILER_64_SET(soc->counter_delta[port_index], 0, ctr_diff);
            COMPILER_64_SET(soc->counter_hw_val[port_index], 0, ctr_new);
            COUNTER_ATOMIC_END(s);

        }

        /* If signaled to exit then return  */
        if (!soc->counter_interval) {
            return SOC_E_NONE;
        }

        /*
         * Allow other tasks to run between processing each port.
         */

        sal_thread_yield();
    }

    return SOC_E_NONE;
}

#ifdef BCM_SBUSDMA_SUPPORT

#ifdef SOC_COUNTER_TABLE_SUPPORT
#define COUNTER_SBUSDMA_DESC_SIZE     5
#elif defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
#define COUNTER_SBUSDMA_DESC_SIZE     4
#else
#define COUNTER_SBUSDMA_DESC_SIZE     3
#endif

#if defined(BCM_ESW_SUPPORT) && (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
STATIC sbusdma_desc_handle_t
       _soc_port_counter_handles[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PP_PORTS][COUNTER_SBUSDMA_DESC_SIZE];
#else
STATIC sbusdma_desc_handle_t
          _soc_port_counter_handles[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS][COUNTER_SBUSDMA_DESC_SIZE];
#endif

#endif /* BCM_SBUSDMA_SUPPORT */

int soc_controlled_counter_update_by_port(int unit, soc_port_t dst_port, soc_port_t src_port)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                  dst_port_base, src_port_base;
    int                  number_of_port_counters;

    COUNTER_LOCK(unit);

    dst_port_base = COUNTER_IDX_PORTBASE(unit, dst_port);
    src_port_base = COUNTER_IDX_PORTBASE(unit, src_port);
    /*
     * get number of counters per port
     */
    number_of_port_counters = SOC_CONTROL(unit)->counter_perport;
    /*Copy SW counter data from src port to dst port*/
    sal_memcpy(&soc->counter_delta[dst_port_base],  &soc->counter_delta[src_port_base],
               sizeof(uint64) * number_of_port_counters);
    sal_memcpy(&soc->counter_hw_val[dst_port_base],  &soc->counter_hw_val[src_port_base],
               sizeof(uint64) * number_of_port_counters);
    sal_memcpy(&soc->counter_sw_val[dst_port_base],  &soc->counter_sw_val[src_port_base],
               sizeof(uint64) * number_of_port_counters);
    /*Clear SW counter data on src port*/
    sal_memset(&soc->counter_delta[src_port_base],  0x0, sizeof(uint64) * number_of_port_counters);
    sal_memset(&soc->counter_hw_val[src_port_base], 0x0, sizeof(uint64) * number_of_port_counters);
    sal_memset(&soc->counter_sw_val[src_port_base], 0x0, sizeof(uint64) * number_of_port_counters);

    COUNTER_UNLOCK(unit);

    return SOC_E_NONE;
}

int soc_controlled_counter_length_get(int unit, soc_controlled_counter_t* ctr, int *length)
{
    if (ctr && ctr->controlled_counter_length_f != NULL) {
        ctr->controlled_counter_length_f(unit, ctr->counter_id, length);
    } else {

        LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_controlled_counter_length_get: unit %d:"
                          "please define controlled_counter_length_get() function.\n"),
               unit));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}
/*
 * Function:
 *      soc_controlled_counters_collect64
 * Purpose:
 *      This routine gets called each time the counter transfer has
 *      completed a cycle.  It collects the 64-bit controlled counters.
 */
int
soc_controlled_counters_collect64(int unit, int discard)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_port_t          port;
    uint64              ctr_new;
    uint64              ctr_diff,ctr_prev;
    uint64              ctr_delta;
    int                 index, port_base;
    soc_controlled_counter_t* ctr;
    uint32 clear_on_read;

    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit,
                            "soc_controlled_counters_collect64: unit=%d "
                            "discard=%d\n"), unit, discard));

     if (!soc_feature(unit, soc_feature_controlled_counters)) {
        return SOC_E_NONE;
    }

    PBMP_ITER(soc->counter_pbmp, port) {
        if (!soc->controlled_counters) {
            break;
        }
        for (index = 0; soc->controlled_counters[index].controlled_counter_f != NULL; index++) {
            volatile uint64 *vptr;
            COUNTER_ATOMIC_DEF s = SOC_CONTROL(unit);

            ctr = &soc->controlled_counters[index];

            if(!COUNTER_IS_COLLECTED(soc->controlled_counters[index])) {
                continue;
            }

            ctr->controlled_counter_f(unit, ctr->counter_id, port, &ctr_new, &clear_on_read);

            if(clear_on_read) {
                /* Counters are Clear-on-Read. Add ctr_new to
                 * previously stored sw value if discard is not set
                 */

                port_base = COUNTER_IDX_PORTBASE(unit, port);
                /* In order to improve access efficiency. Here we didn't take atomic lock first,
                 * and get the delta value directly. In case the delta value was messed up, it
                 * doesn't matter. Because the delta vaule was not used to update SOC DB.
                 */
                ctr_delta = soc->counter_delta[port_base + ctr->counter_idx];

                if (COMPILER_64_IS_ZERO(ctr_new) && COMPILER_64_IS_ZERO(ctr_delta)) {
                    continue;
                }

                if (discard) {
                    /* Update the previous value buffer */
                    COUNTER_ATOMIC_BEGIN(s);
                    soc->counter_hw_val[port_base + ctr->counter_idx] = ctr_new;
                    COMPILER_64_ZERO(soc->counter_delta[port_base + ctr->counter_idx]);
                    COUNTER_ATOMIC_END(s);
                    continue;
                }
                LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                         (BSL_META_U(unit,
                                     "soc_controlled_counters_collect64: ctr %d => "
                                     "0x%08x_%08x\n"), port_base + ctr->counter_idx,
                          COMPILER_64_HI(ctr_new), COMPILER_64_LO(ctr_new)));

                vptr = &soc->counter_sw_val[port_base + ctr->counter_idx];
                COUNTER_ATOMIC_BEGIN(s);
                COMPILER_64_ADD_64(*vptr, ctr_new);
                soc->counter_delta[port_base + ctr->counter_idx] = ctr_new;
                soc->counter_hw_val[port_base + ctr->counter_idx] = ctr_new;
                COUNTER_ATOMIC_END(s);
            } else {
                /* Counters are NOT Clear-on-Read. Add ctr_new to
                 * previously stored sw value if discard is not set.
                 * Should consider the HW reg roll back.
                 */
                port_base = COUNTER_IDX_PORTBASE(unit, port);
                /* Atomic because soc_counter_set may update 64-bit value */
                COUNTER_ATOMIC_BEGIN(s);
                ctr_prev = soc->counter_hw_val[port_base + ctr->counter_idx];
                COUNTER_ATOMIC_END(s);

                if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                    COUNTER_ATOMIC_BEGIN(s);
                    COMPILER_64_ZERO(soc->counter_delta[port_base + ctr->counter_idx]);
                    COUNTER_ATOMIC_END(s);
                    continue;
                }

                vptr = &soc->counter_sw_val[port_base + ctr->counter_idx];
                ctr_diff = ctr_new;
                if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                    int             width;
                    uint64          wrap_amt;
                    /*
                     * Counter must have wrapped around.
                     * Add the proper wrap-around amount.
                     */
                    SOC_IF_ERROR_RETURN(soc_controlled_counter_length_get(unit, ctr, &width));

                    if (width < 32) {
                        COMPILER_64_SET(wrap_amt, 0, 1UL << width);
                        COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                    } else if (width < 64) {
                        COMPILER_64_SET(wrap_amt, 1UL << (width - 32), 0);
                        COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                    }
                }

                COMPILER_64_SUB_64(ctr_diff, ctr_prev);

                COUNTER_ATOMIC_BEGIN(s);
                COMPILER_64_ADD_64(*vptr, ctr_diff);
                soc->counter_delta[port_base + ctr->counter_idx ] = ctr_diff;
                soc->counter_hw_val[port_base + ctr->counter_idx ] = ctr_new;
                COUNTER_ATOMIC_END(s);
            }

        }

        /* If signaled to exit then return  */
        if (!soc->counter_interval) {
            return SOC_E_NONE;
        }

        /* Allow other tasks to run between processing each port */
        COUNTER_UNLOCK(unit);
        sal_thread_yield();
        COUNTER_LOCK(unit);
    }

    return SOC_E_NONE;
}

#ifdef BCM_XGS_SUPPORT

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
/*
 * Function:
 *      _soc_xgs3_update_link_activity
 * Purpose:
 *      Report the link activities to LED processor.
 * Parameters:
 *      unit     - switch unit
 *      port     - port number
 *      tx_byte  - transmitted byte during counter DMA interval
 *      rx_byte  - received byte during counter DMA interval
 *
 * Returns:
 *      SOC_E_*
 */
static int
_soc_xgs3_update_link_activity(int unit, soc_port_t port,
                               uint32 tx_byte, uint32 rx_byte)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                  port_speed, interval, byte;
    uint32               portdata, act_byte;
    soc_info_t *si = &SOC_INFO(unit);

    if (si->port_speed_max[port] >= 100000) {
        port_speed = si->port_speed_max[port];
    } else {
        SOC_IF_ERROR_RETURN(soc_mac_speed_get(unit, port, &port_speed));
    }

    portdata = 0;
    interval = soc->counter_interval;

    if (tx_byte < rx_byte) {
        act_byte = rx_byte;
    } else {
        act_byte = tx_byte;
    }

    if (act_byte > (uint32)(((port_speed / 8) * interval) / 2)) {
        /* transmitting or receiving more than 50% */
        portdata |= 0x2;
    } else if (act_byte > (uint32)(((port_speed / 8) * interval) / 4)) {
        /* transmitting or receiving more than 25% */
        portdata |= 0x4;
    } else if (act_byte > 0) {
        /* some transmitting or receiving activity on the link */
        portdata |= 0x8;
    }

    if (tx_byte) {
        /* Indicate TX activity */
        portdata |= 0x10;
    }
    if (rx_byte) {
        /* Indicate RX activity */
        portdata |= 0x20;
    }

#ifdef BCM_TRIDENT_SUPPORT
#define MONTEREY_LEDUP_PORT_MAX        64
#define APACHE_LEDUP_PORT_MAX          36
#define TRIDENT_LEDUP0_PORT_MAX        36
#define TRIDENT2_LEDUP0_PORT_MAX       64

    if (SOC_IS_TD_TT(unit)) {
        soc_info_t *si;
        int phy_port;  /* physical port number */
        uint32 dram_base;
        uint32 prev_data;

        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[port];

        /* trident first 36 ports in ledproc0, the other 36 ports in ledproc1*/
        if (phy_port > TRIDENT_LEDUP0_PORT_MAX) {
            phy_port -= TRIDENT_LEDUP0_PORT_MAX;
            dram_base = CMICE_LEDUP1_DATA_RAM_BASE;
        } else {
            dram_base = CMICE_LEDUP0_DATA_RAM_BASE;
        }
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            if (SOC_IS_TRIDENT2X(unit) && !SOC_IS_APACHE(unit)) {
                int i, skip_count = 0;
                int led_ix = (phy_port > TRIDENT2_LEDUP0_PORT_MAX)? 1:0;
                for (i=1; i<phy_port; i++) {
                    skip_count += ((si->port_p2l_mapping[i+(led_ix*TRIDENT2_LEDUP0_PORT_MAX)] == -1) ? 1: 0);
                }
                if (phy_port > TRIDENT2_LEDUP0_PORT_MAX) {
                    phy_port -= TRIDENT2_LEDUP0_PORT_MAX;
                    dram_base = CMIC_LEDUP1_DATA_RAM_OFFSET;
                } else {
                    dram_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
                }
                phy_port -= skip_count;
            } else if (SOC_IS_MONTEREY(unit) ) {
                phy_port = si->port_l2p_mapping[port];
                    dram_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
            } else if (SOC_IS_APACHE(unit) ) {
                phy_port = si->port_l2p_mapping[port];
                /* Apache first 36 ports in ledproc0, the other 36 ports in ledproc1*/
                if (phy_port > APACHE_LEDUP_PORT_MAX) {
                    phy_port -= APACHE_LEDUP_PORT_MAX;
                    dram_base = CMIC_LEDUP1_DATA_RAM_OFFSET;
                } else {
                    dram_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
                }
            }
        }
#endif /* BCM_CMICM_SUPPORT */
        if (soc_feature(unit, soc_feature_led_data_offset_a0)) {
            byte = LS_LED_DATA_OFFSET_A0 + phy_port;
            if (SOC_IS_APACHE(unit)) {
                byte -= 1; /* Start from A0 offset */
            }
        } else {
            byte = LS_LED_DATA_OFFSET + phy_port;
        }

        prev_data = soc_pci_read(unit, dram_base + CMIC_LED_REG_SIZE * byte);
        prev_data &= ~0x3e;
        portdata |= prev_data;
        soc_pci_write(unit, dram_base + CMIC_LED_REG_SIZE * byte, portdata);

    } else
#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
        uint32 lastdata = 0;
        byte = LS_LED_DATA_OFFSET_A0 + port;
        lastdata = soc_pci_read(unit, CMIC_LEDUP0_DATA_RAM_OFFSET + CMIC_LED_REG_SIZE * byte);
        lastdata &= ~0x30;
        portdata |= lastdata;
        soc_pci_write(unit, CMIC_LEDUP0_DATA_RAM_OFFSET + CMIC_LED_REG_SIZE * byte, portdata);
    } else
#endif
#endif
    {
        /* 20 bytes from LS_LED_DATA_OFFSET is used by linkscan callback */
        byte = LS_LED_DATA_OFFSET + 20 + port;
        soc_pci_write(unit, CMIC_LED_DATA_RAM(byte), portdata);
    }
    return SOC_E_NONE;
}
#endif
#endif /* BCM_XGS_SUPPORT */

#if defined(BCM_XGS_SUPPORT) || defined(BCM_PETRA_SUPPORT)
/*
 * Function:
 *      soc_counter_collect64
 * Purpose:
 *      This routine gets called each time the counter transfer has
 *      completed a cycle.  It collects the 64-bit counters.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      discard - If true, the software counters are not updated; this
 *              results in only synchronizing the previous hardware
 *              count buffer.
 *      tmp_port - if -1 sw accumulation of all the counters done.
 *             port valid when hw_ctr_reg is valid
 *      hw_ctr_reg - if -1 sw accumulation of all the counters done.
 *                   else hw register value of the tmp_port synced
 *                   to sw counter.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      It computes the deltas in the hardware counters from the last
 *      time it was called and adds them to the high resolution (64-bit)
 *      software counters in counter_sw_val[].  It takes wrapping into
 *      account for counters that are less than 64 bits wide.
 *      It also computes counter_delta[].
 */

STATIC int
soc_counter_collect64(int unit, int discard, soc_port_t tmp_port, soc_reg_t hw_ctr_reg)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_port_t          port;
    soc_reg_t           ctr_reg;
    uint64              ctr_new, ctr_prev, ctr_diff;
    int                 index, max_index;
    int                 port_base, port_base_dma;
    int                 dma = 0;
    int                 recheck_cntrs;
    int                 ar_idx;
    pbmp_t              counter_pbmp;
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    int                 pio_hold, pio_hold_enable;
    uint64              *counter_tmp = NULL;
    uint32              tx_byt_to_led = 0;
    uint32              rx_byt_to_led = 0;
    int                 rxcnt_xaui = 0;
    int                 skip_xaui_rx_counters;
    soc_reg_t           reg_1a = INVALIDr;
    soc_reg_t           reg_1b = INVALIDr;
    soc_reg_t           reg_2a = INVALIDr;
    soc_reg_t           reg_2b = INVALIDr;
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit,
                            "soc_counter_collect64: unit=%d discard=%d\n"),
                 unit, discard));

    COMPILER_64_ZERO(ctr_new);
    COMPILER_64_ZERO(ctr_prev);
    COMPILER_64_ZERO(ctr_diff);

    /* hw_ctr_reg is INVALIDr for normal sw counter accumulation */
    if (hw_ctr_reg == INVALIDr) {
        dma = ((soc->counter_flags & SOC_COUNTER_F_DMA) && (discard != TRUE));
    }
    recheck_cntrs = soc_feature(unit, soc_feature_recheck_cntrs);

#ifdef BCM_BRADLEY_SUPPORT
    pio_hold = SOC_IS_HBX(unit);
    pio_hold_enable = (soc->counter_flags & SOC_COUNTER_F_HOLD);

    if (SOC_IS_HBX(unit) && soc_feature(unit, soc_feature_bigmac_rxcnt_bug)) {
        
        rxcnt_xaui = 1;
        reg_1a = MAC_TXPSETHRr;
        reg_1b = IRFCSr;
        reg_2a = MAC_TXMAXSZr;
        reg_2b = IROVRr;
        if (dma) {
            counter_tmp = soc_counter_tbuf[unit];
        }
    }
#endif /* BCM_BRADLEY_SUPPORT */

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_bigmac_rxcnt_bug)) {
        
        rxcnt_xaui = 1;
        reg_1a = MAC_TXPSETHRr;
#ifdef BCM_ENDURO_SUPPORT
        if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
            reg_1b = IRUCAr;
        } else
#endif /* BCM_ENDURO_SUPPORT */
        {
            reg_1b = IRUCr;
        }
        reg_2a = MAC_TXMAXSZr;
        reg_2b = IRFCSr;
        if (dma) {
            counter_tmp = soc_counter_tbuf[unit];
        }
    }
#endif /* BCM_TRX_SUPPORT */

    if (hw_ctr_reg == INVALIDr) {
        /* accumulate sw counters for all ports */
        SOC_PBMP_CLEAR(counter_pbmp);
        SOC_PBMP_ASSIGN(counter_pbmp, soc->counter_pbmp);
    } else {
        /* sync and accumulate sw counter for specified register of port */
        SOC_PBMP_CLEAR(counter_pbmp);
        port = tmp_port;
        SOC_PBMP_PORT_ADD(counter_pbmp, port);
    }

    PBMP_ITER(counter_pbmp, port) {
        /*
         * Flexing operations on certain devices can create new ports,
         * or delete existing ports. This can happen when the counter
         * thread is not running. So we make a check here before proceeding
         * with further processing. We ignore processing for ports which no
         * longer exist.
         */
        if (!SOC_CONTROL(unit)->counter_map[port]) {
            continue;
        }

        if (!SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_ARAD(unit) &&
            !IS_HG_PORT(unit, port) && !IS_XE_PORT(unit, port)) {
                continue;
        }


        port_base = COUNTER_MIN_IDX_GET(unit, port);
        port_base_dma = COUNTER_MIN_IDX_GET(unit, port - soc->counter_ports32);

#ifdef BCM_PETRA_SUPPORT
        if (SOC_IS_ARAD(unit)) {
            if (_SOC_CONTROLLED_COUNTER_USE(unit, port)) {
                continue;
            }
            port_base += _SOC_CONTROLLED_COUNTER_NOF(unit);
        }
#endif /*BCM_PETRA_SUPPORT*/
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
        /*
         * Handle XAUI Rx counter bug.
         *
         * If the IRFCS counter returns the contents of the
         * MAC_TXPSETHR register, and the IROVR counter returns the
         * contents of the MAC_TXMAXSZ register, we assume that the
         * counters are bogus.
         *
         * By using two counters we significantly reduce the chance of
         * getting stuck if the real counter value matches the bogus
         * reference.
         *
         * To protect ourselves from the error condition happening
         * while the counter DMA is in progress, we manually check the
         * counters before and after we copy the DMA buffer contents.
         */
        skip_xaui_rx_counters = 1;
        if (rxcnt_xaui &&
            (IS_GX_PORT(unit, port) ||
             (IS_XG_PORT(unit, port) &&
              (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port))))) {
            uint64 val_1a, val_1b, val_2a, val_2b;
            uint32 *p1, *p2;
            int rxcnt_appear_valid = 0;

            /* Read reference values and counter values */
            if (soc_reg_get(unit, reg_1a, port, 0, &val_1a) == 0 &&
                soc_reg_get(unit, reg_1b, port, 0, &val_1b) == 0 &&
                soc_reg_get(unit, reg_2a, port, 0, &val_2a) == 0 &&
                soc_reg_get(unit, reg_2b, port, 0, &val_2b) == 0) {

                /* Check for bogus values */
                if (COMPILER_64_NE(val_1a, val_1b) ||
                    COMPILER_64_NE(val_2a, val_2b)) {
                    rxcnt_appear_valid = 1;
                }
                if (rxcnt_appear_valid && counter_tmp) {
                    /* Copy DMA buffer */
                    sal_memcpy(counter_tmp, &soc->counter_buf64[port_base_dma],
                               SOC_COUNTER_TBUF_SIZE(unit));
                    p1 = (uint32 *)&counter_tmp[SOC_REG_CTR_IDX(unit, reg_1b)];
                    p2 = (uint32 *)&counter_tmp[SOC_REG_CTR_IDX(unit, reg_2b)];
                    /* Read counter values again */
                    if (soc_reg_get(unit, reg_1b, port, 0, &val_1b) == 0 &&
                        soc_reg_get(unit, reg_2b, port, 0, &val_2b) == 0) {
                        /* Check for bogus value again, including DMA buffer */
                        if ((COMPILER_64_NE(val_1a, val_1b) ||
                             COMPILER_64_NE(val_2a, val_2b)) &&
                            (p1[0] != COMPILER_64_LO(val_1a) ||
                             p2[0] != COMPILER_64_LO(val_2a))) {
                            /* Counters appears to be valid */
                            skip_xaui_rx_counters = 0;
                        }
                    }
                } else if (rxcnt_appear_valid && !dma) {
                    /* If not DMA we take the chance and read the counters */
                    skip_xaui_rx_counters = 0;
                }
            }
        }
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

        if (hw_ctr_reg == INVALIDr) {
            index = 0;
            max_index = PORT_CTR_NUM(unit, port);
        } else {
            int  port_index1, num_entries1;
            char *cname;

            /* get index of counter register */
            SOC_IF_ERROR_RETURN(_soc_counter_get_info(unit, port, hw_ctr_reg,
                                                      &port_index1, &num_entries1,
                                                      &cname));

            index = port_index1 - port_base;
            max_index = index + 1;
        }

        for ( ; index < max_index; index++) {
            volatile uint64 *vptr;
            COUNTER_ATOMIC_DEF s = SOC_CONTROL(unit);

            ctr_reg = PORT_CTR_REG(unit, port, index)->reg;
            ar_idx = PORT_CTR_REG(unit, port, index)->index;

            if (SOC_COUNTER_INVALID(unit, ctr_reg)) {
                continue;
            }
            if ((ctr_reg < NUM_SOC_REG) && !SOC_REG_PORT_IDX_VALID(unit,ctr_reg, port, ar_idx)) {
                continue;
            }

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            /* Trap for BigMAC TX Timestamp FIFO reads
             *
             * MAC_TXTIMESTAMPFIFOREADr is immediately after
             * MAC_RXLLFCMSGCNTr.  It is a single register exposing
             * a HW FIFO which pops on read.  And it is in the counter
             * DMA range.  So we detect MAC_RXLLFCMSGCNTr and record
             * the value of the next piece of data in a SW FIFO for
             * use by the API.
             */
            if (soc_feature(unit, soc_feature_timestamp_counter) &&
                dma && (ctr_reg == MAC_RXLLFCMSGCNTr) &&
                (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) &&
                (soc->counter_timestamp_fifo[port] != NULL) &&
                !SHR_FIFO_IS_FULL(soc->counter_timestamp_fifo[port])) {
                uint32 *ptr =
                    (uint32 *)&soc->counter_buf64[port_base_dma + index + 1];

                if (ptr[0] != 0) { /* Else, HW FIFO empty */
                    SHR_FIFO_PUSH(soc->counter_timestamp_fifo[port], &(ptr[0]));
                }
            }
            /*If spn_SW_TIMESTAMP_FIFO_ENABLE is 0, the counter DMA will not
             *read register MAC_TXTIMESTAMPFIFOREAD.
             */
            if (!SOC_CONTROL(unit)->sw_timestamp_fifo_enable &&
                IS_XE_PORT(unit, port) &&
                (index >= (DIRECT_TIMESTAMP_DMA_COUNTER_NUM +
                           XMAC_DMA_COUNTER_BASE))) {
                dma = FALSE;
            }

#endif /* BCM_TRIUMPH2_SUPPORT */

            /* Atomic because soc_counter_set may update 64-bit value */
            COUNTER_ATOMIC_BEGIN(s);
            ctr_prev = soc->counter_hw_val[port_base + index];
            COUNTER_ATOMIC_END(s);

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
            if (pio_hold && ctr_reg == HOLDr) {
                if (pio_hold_enable) {
                    uint64 ctr_prev_x, ctr_prev_y;
                    uint32 ctr_x, ctr_y;
                    int idx_x, idx_y;
                    /*
                     * We need to account for the two values of HOLD
                     * separately to get correct rollover behavior.
                     * Use holes in counter space to store HOLD_X/Y values
                     * HOLD_X at RDBGC4r, hole, IUNHGIr
                     * HOLD_Y at IUNKOPCr, hole, RDBGC5r
                     */

                    idx_x = SOC_REG_CTR_IDX(unit, RDBGC4r) + 1;
                    idx_y = SOC_REG_CTR_IDX(unit, IUNKOPCr) + 1;

                    COUNTER_ATOMIC_BEGIN(s);
                    ctr_prev_x = soc->counter_hw_val[port_base + idx_x];
                    ctr_prev_y = soc->counter_hw_val[port_base + idx_y];
                    COUNTER_ATOMIC_END(s);

                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, HOLD_Xr, port, ar_idx,
                                       &ctr_x));
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, HOLD_Yr, port, ar_idx,
                                       &ctr_y));

                    if (discard) {
                        /* Just save the new sum to be used below */
                        COMPILER_64_SET(ctr_new, 0, ctr_x + ctr_y);
                    } else {
                        int             width = 0;
                        uint64          wrap_amt;

                        COMPILER_64_SET(ctr_diff, 0, ctr_x);
                        /* We know the width of HOLDr is < 32 */
                        if (COMPILER_64_LT(ctr_diff, ctr_prev_x)) {
                            width =
                                SOC_REG_INFO(unit, ctr_reg).fields[0].len;
                            COMPILER_64_SET(wrap_amt, 0, 1UL << width);
                            COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                        }
                        /* Calculate HOLD_X diff from previous */
                        COMPILER_64_SUB_64(ctr_diff, ctr_prev_x);
                        ctr_new = ctr_diff;

                        COMPILER_64_SET(ctr_diff, 0, ctr_y);
                        if (COMPILER_64_LT(ctr_diff, ctr_prev_y)) {
                            width =
                                SOC_REG_INFO(unit, ctr_reg).fields[0].len;
                            COMPILER_64_SET(wrap_amt, 0, 1UL << width);
                            COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                        }
                        /* Calculate HOLD_Y diff from previous */
                        COMPILER_64_SUB_64(ctr_diff, ctr_prev_y);

                        /* Combine diffs */
                        COMPILER_64_ADD_64(ctr_new, ctr_diff);
                        /* Add previous value so logic below handles
                         * all of the other updates */
                        COMPILER_64_ADD_64(ctr_new, ctr_prev);
                    }

                    /* Update previous values with new values.
                     * Since these are INVALID registers, the other
                     * counter logic will not touch them. */
                    COMPILER_64_SET(soc->counter_hw_val[port_base + idx_x], 0, ctr_x);
                    COMPILER_64_SET(soc->counter_hw_val[port_base + idx_y], 0, ctr_y);
                } else {
                    /*
                     * The counter collected by DMA is not reliable
                     * due to incorrect access type, so force zero.
                     */
                    COMPILER_64_ZERO(ctr_new);
                }
            } else if (IS_HYPLITE_PORT(unit, port) && is_xaui_rx_counter(ctr_reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_read(unit, ctr_reg,
                                    soc_reg_addr(unit, ctr_reg, port, ar_idx),
                                    &ctr_new));
            } else if (rxcnt_xaui && is_xaui_rx_counter(ctr_reg)) {
                if (skip_xaui_rx_counters) {
                    ctr_new = ctr_prev;
                } else if (counter_tmp) {
                    uint32 *ptr = (uint32 *)&counter_tmp[index];
                    /* No need to check for SWAP64 flag */
                    COMPILER_64_SET(ctr_new, ptr[1], ptr[0]);
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, ctr_reg, port, ar_idx,
                                     &ctr_new));
                }
            } else
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */
            if (dma) {
#ifdef BCM_SBUSDMA_SUPPORT
#ifdef SOC_COUNTER_TABLE_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit) &&
                    soc_feature(unit, soc_feature_sbusdma) &&
                         SOC_REG_IS_COUNTER_TABLE(unit, ctr_reg)) {
                    uint32 *ptr;
                    int dma_offset = SOC_CTR_TBL_INFO(unit)->tables[index - soc->counter_mib_tbl_first].dma_offset;
                    ptr = (uint32 *)&soc->counter_buf64[port_base_dma + soc->counter_mib_tbl_first + dma_offset];
                    if (soc->counter_flags & SOC_COUNTER_F_SWAP64) {
                        COMPILER_64_SET(ctr_new, ptr[0], ptr[1]);
                    } else {
                        COMPILER_64_SET(ctr_new, ptr[1], ptr[0]);
                    }
                } else
#endif /* SOC_COUNTER_TABLE_SUPPORT */
                if (soc_feature(unit, soc_feature_sbusdma) &&
                    (ctr_reg < NUM_SOC_REG) &&
                    !SOC_REG_IS_64(unit, ctr_reg)) {
                    uint32 *buf32, *ptr, temp = 0;
                    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3(unit)) {
                        if (index < soc->counter_egr_first) {
                            buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                  soc->counter_ing_first];
                            ptr = &buf32[index - soc->counter_ing_first];
                        } else {
                            buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                  soc->counter_egr_first];
                            ptr = &buf32[index - soc->counter_egr_first];
                        }
                        temp = *ptr & ((1 << soc_reg_field_length
                               (unit, ctr_reg, COUNTf)) - 1);
                    } else {
                        if (index < soc->counter_egr_first) {
                            buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                  soc->counter_ing_first];
                            ptr = &buf32[index - soc->counter_ing_first];
                        } else if (index < soc->counter_mac_first) {
                            buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                  soc->counter_egr_first];
                            ptr = &buf32[index - soc->counter_egr_first];
                        } else {
                            buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                  soc->counter_mac_first];
                            ptr = &buf32[index - soc->counter_mac_first];
                        }

                        if (soc_reg_field_length(unit, ctr_reg, COUNTf) == 32) {
                            temp = *ptr & 0xffffffff;
                        } else {
                            temp = *ptr & ((1 << soc_reg_field_length
                                   (unit, ctr_reg, COUNTf)) - 1);
                        }
                    }
                    COMPILER_64_SET(ctr_new, 0, temp);
                } else
#endif /* BCM_SBUSDMA_SUPPORT */
                {
                    uint32 *ptr =
                        (uint32 *)&soc->counter_buf64[port_base_dma + index];
                    if (soc->counter_flags & SOC_COUNTER_F_SWAP64) {
                        COMPILER_64_SET(ctr_new, ptr[0], ptr[1]);
                    } else {
                        COMPILER_64_SET(ctr_new, ptr[1], ptr[0]);
                    }
                }
            } else {
                if(ctr_reg < NUM_SOC_REG) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, ctr_reg, port, ar_idx,
                                 &ctr_new));
            }
#ifdef SOC_COUNTER_TABLE_SUPPORT
                else if (SOC_IS_TOMAHAWK3(unit) &&
                         SOC_REG_IS_COUNTER_TABLE(unit, ctr_reg)) {
                    soc_ctr_tbl_entry_t *ctr_tbl_info_entry = NULL;
                    SOC_IF_ERROR_RETURN
                        (_soc_counter_tbl_get_info(unit, port, ctr_reg, &ctr_tbl_info_entry));
                    SOC_IF_ERROR_RETURN
                        (_soc_ctr_tbl_field_get(unit, ctr_reg, port, ctr_tbl_info_entry, &ctr_new));
                }
#endif /* SOC_COUNTER_TABLE_SUPPORT */
            }

            /*
             * For counter sync operation update the dma buffer
             * for the port index with the value read from hardware
             * counter register.
             */
            if ((hw_ctr_reg != INVALIDr) &&
                (soc->counter_flags & SOC_COUNTER_F_DMA)) {
                uint32 *ptr;
#ifdef BCM_SBUSDMA_SUPPORT
                if (soc_feature(unit, soc_feature_sbusdma) &&
                    (ctr_reg < NUM_SOC_REG) &&
                    !SOC_REG_IS_64(unit, ctr_reg)) {
                    uint32 *buf32;
                        if (SOC_IS_TOMAHAWKX(unit)) {
                            if (index < soc->counter_egr_first) {
                                buf32 = (uint32 *)&soc->counter_buf64[
                                                        port_base_dma +
                                                        soc->counter_ing_first];
                                ptr = &buf32[index - soc->counter_ing_first];
                            } else {
                                buf32 = (uint32 *)&soc->counter_buf64[
                                                        port_base_dma +
                                                        soc->counter_egr_first];
                                ptr = &buf32[index - soc->counter_egr_first];
                            }
                        } else {
                            if (index < soc->counter_egr_first) {
                                buf32 = (uint32 *)&soc->counter_buf64[
                                                        port_base_dma +
                                                        soc->counter_ing_first];
                                ptr = &buf32[index - soc->counter_ing_first];
                            } else if (index < soc->counter_mac_first) {
                                buf32 = (uint32 *)&soc->counter_buf64[
                                                        port_base_dma +
                                                        soc->counter_egr_first];
                                ptr = &buf32[index - soc->counter_egr_first];
                            } else {
                                buf32 = (uint32 *)&soc->counter_buf64[
                                                        port_base_dma +
                                                        soc->counter_mac_first];
                                ptr = &buf32[index - soc->counter_mac_first];
                            }
                        }
                        COUNTER_ATOMIC_BEGIN(s);
                        if (soc_reg_field_length(
                                   unit, ctr_reg, COUNTf) == 32) {
                            COMPILER_64_TO_32_LO(ptr[0], ctr_new);
                        } else {
                            ptr[0] = COMPILER_64_LO(ctr_new) &
                                        ((1 << soc_reg_field_length
                                                (unit, ctr_reg, COUNTf)) - 1);
                        }
                        COUNTER_ATOMIC_END(s);

                } else
#endif /* BCM_SBUSDMA_SUPPORT */
                {
#ifdef BCM_TOMAHAWK3_SUPPORT
                    if (SOC_IS_TOMAHAWK3(unit) &&
                        soc_feature(unit, soc_feature_sbusdma) &&
                        SOC_REG_IS_COUNTER_TABLE(unit, ctr_reg)) {
                        int dma_offset = SOC_CTR_TBL_INFO(unit)->tables[index - soc->counter_mib_tbl_first].dma_offset;
                        ptr = (uint32 *)&soc->counter_buf64[port_base_dma + soc->counter_mib_tbl_first + dma_offset];
                    } else
#endif
                    {
                        ptr = (uint32 *)&soc->counter_buf64[port_base_dma + index];
                    }
                    COUNTER_ATOMIC_BEGIN(s);
                    /* Update the DMA location */
                    if (soc->counter_flags & SOC_COUNTER_F_SWAP64) {
                        COMPILER_64_TO_32_HI(ptr[0], ctr_new);
                        COMPILER_64_TO_32_LO(ptr[1], ctr_new);
                    } else {
                        COMPILER_64_TO_32_LO(ptr[0], ctr_new);
                        COMPILER_64_TO_32_HI(ptr[1], ctr_new);
                    }
                    COUNTER_ATOMIC_END(s);
                }
            }/* hw_ctr_reg != INVALIDr */

            if (soc_feature(unit, soc_feature_counter_parity)
#ifdef BCM_HURRICANE2_SUPPORT
                || soc_reg_field_valid(unit, ctr_reg, PARITYf)
#endif
               )
            {
                uint32 temp;
                uint32 temp_len;
                if (soc_reg_field_valid(unit, ctr_reg, EVEN_PARITYf) ||
                    soc_reg_field_valid(unit, ctr_reg, PARITYf)) {
                    temp_len = soc_reg_field_length(unit, ctr_reg, COUNTf);
                    if (temp_len < 32) {
                        temp = COMPILER_64_LO(ctr_new) & ((1 << temp_len) - 1);
                        COMPILER_64_SET(ctr_new, 0, temp);
                    } else {
                        temp = COMPILER_64_HI(ctr_new) &
                               ((1 << (temp_len - 32)) - 1);
                        COMPILER_64_SET(ctr_new, temp, COMPILER_64_LO(ctr_new));
                    }
                }
            }

            if ( (recheck_cntrs == TRUE) &&
                 COMPILER_64_NE(ctr_new, ctr_prev) ) {
                /* Seeds of doubt, double-check */
                uint64 ctr_new2;
                int suspicious = 0;
                SOC_IF_ERROR_RETURN
                  (soc_reg_get(unit, ctr_reg, port, 0,
                                &ctr_new2));

                /* Check for partial ordering, with wrap */
                if (COMPILER_64_LT(ctr_new, ctr_prev)) {
                    if (COMPILER_64_LT(ctr_new2, ctr_new) ||
                        COMPILER_64_GE(ctr_new2, ctr_prev)) {
                        /* prev < new2 < new, bad data */
                        /* Try again next time */
                        ctr_new = ctr_prev;
                        suspicious = 1;
                    }
                } else {
                    if (COMPILER_64_LT(ctr_new2, ctr_new) &&
                        COMPILER_64_GE(ctr_new2, ctr_prev)) {
                        /* prev < new2 < new, bad data */
                        /* Try again next time */
                        ctr_new = ctr_prev;
                        suspicious = 1;
                    }
                }
                /* Otherwise believe ctr_new */

                if (suspicious) {
#if !defined(SOC_NO_NAMES)
                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit,
                                         "soc_counter_collect64: "
                                         "unit %d, port%d: suspicious %s "
                                         "counter read (%s)\n"),
                              unit, port,
                              dma?"DMA":"manual",
                              SOC_REG_NAME(unit, ctr_reg)));
#else
                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit,
                                         "soc_counter_collect64: "
                                         "unit %d, port%d: suspicious %s "
                                         "counter read (Counter%d)\n"),
                              unit, port,
                              dma?"DMA":"manual",
                              ctr_reg));
#endif /* SOC_NO_NAMES */
                } /* Suspicious counter change */
            } /* recheck_cntrs == TRUE */

            if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                COUNTER_ATOMIC_BEGIN(s);
                COMPILER_64_ZERO(soc->counter_delta[port_base + index]);
                COUNTER_ATOMIC_END(s);
                continue;
            }

            if (discard) {
                if (dma) {
                    uint32 *ptr;
#ifdef BCM_SBUSDMA_SUPPORT
                    if (soc_feature(unit, soc_feature_sbusdma) &&
                        (ctr_reg < NUM_SOC_REG) &&
                        !SOC_REG_IS_64(unit, ctr_reg)) {
                        uint32 *buf32;
                        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3(unit)) {
                            if (index < soc->counter_egr_first) {
                                buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                      soc->counter_ing_first];
                                ptr = &buf32[index - soc->counter_ing_first];
                            } else {
                                buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                      soc->counter_egr_first];
                                ptr = &buf32[index - soc->counter_egr_first];
                            }
                        } else {
                            if (index < soc->counter_egr_first) {
                                buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                      soc->counter_ing_first];
                                ptr = &buf32[index - soc->counter_ing_first];
                            } else if (index < soc->counter_mac_first) {
                                buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                      soc->counter_egr_first];
                                ptr = &buf32[index - soc->counter_egr_first];
                            } else {
                                buf32 = (uint32 *)&soc->counter_buf64[port_base_dma +
                                                                      soc->counter_mac_first];
                                ptr = &buf32[index - soc->counter_mac_first];
                            }
                        }
                    } else
#endif
                    {
                        ptr = (uint32 *)&soc->counter_buf64[port_base_dma + index];
                    }
                    COUNTER_ATOMIC_BEGIN(s);
                    /* Update the DMA location */
                    if (soc->counter_flags & SOC_COUNTER_F_SWAP64) {
                        COMPILER_64_TO_32_HI(ptr[0], ctr_new);
                        COMPILER_64_TO_32_LO(ptr[1], ctr_new);
                    } else {
                        COMPILER_64_TO_32_LO(ptr[0], ctr_new);
                        COMPILER_64_TO_32_HI(ptr[1], ctr_new);
                    }
                } else {
                    COUNTER_ATOMIC_BEGIN(s);
                }
                /* Update the previous value buffer */
                soc->counter_hw_val[port_base + index] = ctr_new;
                COMPILER_64_ZERO(soc->counter_delta[port_base + index]);
                COUNTER_ATOMIC_END(s);
                continue;
            }

            LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                     (BSL_META_U(unit,
                                 "soc_counter_collect64: ctr %d => 0x%08x_%08x\n"),
                      port_base + index,
                      COMPILER_64_HI(ctr_new), COMPILER_64_LO(ctr_new)));

            vptr = &soc->counter_sw_val[port_base + index];

            ctr_diff = ctr_new;

            if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                int             width, i = 0;
                uint64          wrap_amt;

                /*
                 * Counter must have wrapped around.
                 * Add the proper wrap-around amount.
                 */
#ifdef SOC_COUNTER_TABLE_SUPPORT
                if(SOC_REG_IS_COUNTER_TABLE(unit, ctr_reg)) {
                    soc_ctr_tbl_entry_t *ctr_tbl_info_entry = NULL;

                    SOC_IF_ERROR_RETURN
                        (_soc_counter_tbl_get_info(unit, port, ctr_reg, &ctr_tbl_info_entry));
                    width = soc_format_field_length(unit,
                                ctr_tbl_info_entry->format,
                                ctr_tbl_info_entry->ctr_field);
                } else
#endif
                {
                    width = SOC_REG_INFO(unit, ctr_reg).fields[0].len;
                    if (soc_feature(unit, soc_feature_counter_parity)
#ifdef BCM_HURRICANE2_SUPPORT
                    || soc_reg_field_valid(unit, ctr_reg, PARITYf)
#endif
                   )
                {
                    while((SOC_REG_INFO(unit, ctr_reg).fields + i) != NULL) {
                        if (SOC_REG_INFO(unit, ctr_reg).fields[i].field ==
                            COUNTf) {
                            width = SOC_REG_INFO(unit, ctr_reg).fields[i].len;
                            break;
                        }
                        i++;
                    }
                }
                }
                if (width < 32) {
                    COMPILER_64_SET(wrap_amt, 0, 1UL << width);
                    COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                } else if (width < 64) {
                    COMPILER_64_SET(wrap_amt, 1UL << (width - 32), 0);
                    COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                }
            }

            COMPILER_64_SUB_64(ctr_diff, ctr_prev);

            COUNTER_ATOMIC_BEGIN(s);
            COMPILER_64_ADD_64(*vptr, ctr_diff);
            soc->counter_delta[port_base + index] = ctr_diff;
            soc->counter_hw_val[port_base + index] = ctr_new;
            COUNTER_ATOMIC_END(s);

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
            if (soc_feature(unit, soc_feature_ctr_xaui_activity)) {
#ifdef BCM_TRIDENT_SUPPORT
                if (SOC_IS_TD_TT(unit)) {
                    if (ctr_reg == RPKTr) {
                        COMPILER_64_TO_32_LO(rx_byt_to_led, ctr_diff);
                    } else if (ctr_reg == TPKTr) {
                        COMPILER_64_TO_32_LO(tx_byt_to_led, ctr_diff);
                    }
                } else
#ifdef BCM_SABER2_SUPPORT
 if (SOC_IS_SABER2(unit)) {
                    if (IS_MXQ_PORT(unit, port)) {
                    if (ctr_reg == MXQ_RPKTr) {
                        COMPILER_64_TO_32_LO(rx_byt_to_led, ctr_diff);
                    } else if (ctr_reg == MXQ_TPKTr) {
                        COMPILER_64_TO_32_LO(tx_byt_to_led, ctr_diff);
                    }
                   } else {
                         if (ctr_reg == RPKTr) {
                        COMPILER_64_TO_32_LO(rx_byt_to_led, ctr_diff);
                    } else if (ctr_reg == TPKTr) {
                        COMPILER_64_TO_32_LO(tx_byt_to_led, ctr_diff);
                    }
                  }

                } else
#endif
#endif
                {
#ifdef BCM_BRADLEY_SUPPORT
                    if (ctr_reg == IRBYTr) {
                        COMPILER_64_TO_32_LO(rx_byt_to_led, ctr_diff);
                    } else if (ctr_reg == ITBYTr) {
                        COMPILER_64_TO_32_LO(tx_byt_to_led, ctr_diff);
                    }
#endif
                }
            }
#endif
        }

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
        if (soc_feature(unit, soc_feature_ctr_xaui_activity)) {
            _soc_xgs3_update_link_activity(unit, port, tx_byt_to_led,
                                           rx_byt_to_led);
            tx_byt_to_led = 0;
            rx_byt_to_led = 0;
        }
#endif

#if defined(INCLUDE_PHY_8806X)
        phy_mt2_update_led_speed_activity(unit, port);
#endif

        /* If signaled to exit then return  */
        if (!soc->counter_interval) {
            return SOC_E_NONE;
        }

        /*
         * Allow other tasks to run between processing each port.
         */

        if (hw_ctr_reg == INVALIDr) {
            COUNTER_UNLOCK(unit);
            sal_thread_yield();
            COUNTER_LOCK(unit);
        }

        if (!soc->counter_interval) {
            return SOC_E_NONE;
        }
    }

    return SOC_E_NONE;
}
#endif /*defined(BCM_XGS_SUPPORT) || defined(BCM_PETRA_SUPPORT)*/

#ifdef BCM_KATANA_SUPPORT
STATIC int
_soc_counter_katana_a0_non_dma_entries(int unit, int dma_id, int port_idx,
                                       int idx, soc_reg_t ctr_reg)

{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma, *non_dma1;
    int base_index, id, width, i, index;
    int rv, entry_words;
    uint32 fval[2];
    uint64 ctr_new;
    int buffer_seg, seg_addr;
    soc_mem_t buffer_mem, dma_mem;
    uint32 rval = 0;
    COUNTER_ATOMIC_DEF s = SOC_CONTROL(unit);
    int field_count;
    soc_field_t field;
    int dma_base_index;
    int num_non_dma_entries = 0;
    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit,
                            "_soc_counter_katana_a0_non_dma_entries: unit=%d id=%d\n"),
                 unit, dma_id));

    soc = SOC_CONTROL(unit);

    if (soc->counter_non_dma == NULL) {
        return SOC_E_RESOURCE;
    }

    non_dma = &soc->counter_non_dma[dma_id];

    if (non_dma->flags & _SOC_COUNTER_NON_DMA_DO_DMA) {
        non_dma1 = &soc->counter_non_dma[dma_id + 1];
    } else {
        return SOC_E_NONE;
    }

    entry_words = soc_mem_entry_words(unit, non_dma->mem);
    if (non_dma->field != INVALIDf) {
        width = soc_mem_field_length(unit, non_dma->mem,
                                     non_dma->field);
    } else {
        width = 32;
    }

    buffer_seg = (soc_feature(unit, soc_feature_cosq_gport_stat_ability)) ?
                 (11 * 1024) : (8 * 1024);
    buffer_mem = (soc_feature(unit, soc_feature_cosq_gport_stat_ability)) ?
                 CTR_FLEX_COUNT_11m : CTR_FLEX_COUNT_8m;

    for (id = 0; id < 2; id++) {
        index = non_dma->mem - CTR_FLEX_COUNT_0m;
        seg_addr = (id == 0) ? buffer_seg : (index * 1024);
        soc_reg_field_set(unit, CTR_SEGMENT_STARTr, &rval, SEG_STARTf,
                          seg_addr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit,CTR_SEGMENT_STARTr, REG_PORT_ANY,index, rval));
        LOG_DEBUG(BSL_LS_SOC_COUNTER,
                    (BSL_META_U(unit,
                                "id=%d index=%d seg_addr=%d \n"),
                     id, index, seg_addr));
        if (ctr_reg == INVALIDr) {
           for (i = 0; i < 4; i++) {
               if (non_dma->flags & _SOC_COUNTER_NON_DMA_DO_DMA) {
                  if (non_dma->dma_buf[i] == NULL) {
                      continue;
                  }

                  dma_mem = (id == 0) ? non_dma->dma_mem[i] : (buffer_mem + i);
                  rv = soc_mem_read_range(unit, dma_mem,
                                         MEM_BLOCK_ANY,
                                         non_dma->dma_index_min[i],
                                         non_dma->dma_index_max[i],
                                         non_dma->dma_buf[i]);
                  if (SOC_FAILURE(rv)) {
                     return rv;
                  }
               }
           }
           dma_mem = non_dma->mem;
           base_index = non_dma->base_index;
           num_non_dma_entries = non_dma->num_entries;
        }
        else {
           /* Collect counter values for a specific port */
           dma_mem = non_dma->dma_mem[0];
           dma_base_index = port_idx - non_dma->base_index +
                            non_dma->dma_index_min[0];

           for (i = 1; i < 4; i++) {
                if (non_dma->dma_buf[i] == NULL) {
                    continue;
                }
                if (dma_base_index > non_dma->dma_index_max[i - 1]) {
                    dma_mem = non_dma->dma_mem[i];
                    dma_base_index -= non_dma->dma_index_max[i - 1] + 1;
                    dma_base_index += non_dma->dma_index_min[i];

                }
           }

           rv = soc_mem_read(unit, dma_mem, MEM_BLOCK_ANY,
                             dma_base_index + idx,
                             non_dma->dma_buf[0]);


           if (SOC_FAILURE(rv)) {
               return rv;
           }

           base_index = port_idx + idx;
           num_non_dma_entries = 1;

        }
        for (field_count = 0; field_count < 2; field_count++) {
            base_index = (field_count == 0) ? base_index :
                                              non_dma1->base_index;
            field  = (field_count == 0) ? non_dma->field :
                                          non_dma1->field;
            for (i = 0; i < num_non_dma_entries; i++) {
                soc_mem_field_get(unit, dma_mem,
                                  &non_dma->dma_buf[0][entry_words * i],
                                  field, fval);
                if (width <= 32) {
                    COMPILER_64_SET(ctr_new, 0, fval[0]);
                } else {
                    COMPILER_64_SET(ctr_new, fval[1], fval[0]);
                }

                COUNTER_ATOMIC_BEGIN(s);
                COMPILER_64_ADD_64(soc->counter_sw_val[base_index + i],
                                   ctr_new);
                soc->counter_hw_val[base_index + i] = ctr_new;
                soc->counter_delta[base_index + i] = ctr_new;
                COUNTER_ATOMIC_END(s);
            }
        }
    }
    return SOC_E_NONE;
}
#endif /* BCM_KATANA_SUPPORT */

STATIC int
soc_counter_non_dma_ready(int unit, soc_counter_non_dma_t *non_dma, int *ctr_dma_ct)
{
    soc_control_t *soc;
    int max_profile, curr_profile_pick, given_profiles;

    soc = SOC_CONTROL(unit);

    if (soc->counter_sync_req) {
        /* Return True for all Counters when sync is in Progress */
        return 1;
    }

    if ((!(non_dma->flags & _SOC_COUNTER_NON_DMA_CTR_EVICT)) ||
        (soc->ctr_evict_interval == 0)) {
        /* Traditional DMA. Hence return true.
         */
        return 1;
    }

    if (non_dma->dma_rate_profile & _SOC_COUNTER_DMA_RATE_PROFILE_ALL) {
        /* Enabled for All profiles */
        return 1;
    }


    /* get max profile value */
    max_profile = _SOC_COUNTER_DMA_RATE_PROFILE_MAX;

    *ctr_dma_ct = *ctr_dma_ct % max_profile;
    curr_profile_pick = 1 << *ctr_dma_ct;

    given_profiles =
        (non_dma->dma_rate_profile & _SOC_COUNTER_DMA_RATE_PROFILE_MASK) >>
                    _SOC_COUNTER_DMA_RATE_PROFILE_OFFSET;

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "dma_prof 0x%x, pick_prof 0x%x, ctr_dma_ct %d\n"),
              given_profiles, curr_profile_pick, *ctr_dma_ct));
    if (given_profiles & curr_profile_pick) {
        return 1;
    }

    return 0;
}

/*
 * Function:
 *      soc_counter_collect_non_dma_entries
 * Purpose:
 *      This routine collects and accumulates all the non-dmaable counters.
 *      Gets invoked from the counter thread and _soc_counter_get.
 *      Based on arg the ctr_reg
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      tmp_port - StrataSwitch port #.
 *      port_idx - port index
 *      ar_idx -  index
 *      ctr_reg - if INVALIDr, collects and accumulates counter values for all
 *                else collects and accumulates for a specified port.
 * Returns:
 *      NONE
 * Notes:
 */

STATIC void
soc_counter_collect_non_dma_entries(int unit,
                                    int tmp_port,
                                    int port_idx,
                                    int ar_idx,
                                    soc_reg_t ctr_reg)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    int base_index, width, i;
    int id = 0;
    int idx = 0;
    int port;
    uint32 fval[2];
    uint64 ctr_new, ctr_prev, ctr_diff, wrap_amt;
    int rv=0, count, entry_words;
    COUNTER_ATOMIC_DEF s = SOC_CONTROL(unit);
    int dma_base_index;
    int num_non_dma_entries = 0;
    soc_mem_t dma_mem;
    int non_dma_counter_end = SOC_COUNTER_NON_DMA_END -
                               SOC_COUNTER_NON_DMA_START;

    int index_max = 0;
    int j, subset_ct; /* For Parent-Child Model */

    int qindex;
    int port_index, num_entries;
    char *cname;
    soc_reg_t ctr_tmp_reg;
    uint8 tab_entry;

    soc = SOC_CONTROL(unit);

    if (soc->counter_non_dma == NULL) {
        return;
    }

    COMPILER_REFERENCE(tab_entry);
    /*
     * if ctr_reg != -1, retrieve stats for a
     * specific port and queue from the hardware
     * update the sw copy of the same.
     */
    if (ctr_reg >= NUM_SOC_REG) {
        id = ctr_reg - SOC_COUNTER_NON_DMA_START;
        non_dma_counter_end = id + 1;
        idx = (ar_idx < 0)? 0: ar_idx;
    }

    for (; id < non_dma_counter_end; id++) {
        non_dma = &soc->counter_non_dma[id];

        if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
            continue;
        }

        /* If signaled to exit then return  */
        if (!soc->counter_interval) {
            return;
        }

        width = entry_words = 0;

        if (non_dma->mem != INVALIDm) {

#ifdef BCM_KATANA_SUPPORT
            if ((SOC_IS_KATANA(unit) || SOC_IS_KATANA2(unit)) &&
                (soc_feature(unit, soc_feature_counter_toggled_read))) {

                rv = _soc_counter_katana_a0_non_dma_entries(unit, id, port_idx,
                                                            idx, ctr_reg);

                if (SOC_FAILURE(rv)) {
                    return;
                } else {
                    continue;
                }
            }
#endif


            entry_words = soc_mem_entry_words(unit, non_dma->mem);
            if (non_dma->field != INVALIDf) {
                width = soc_mem_field_length(unit, non_dma->mem,
                                             non_dma->field);
            } else {
                width = 32;
            }

            if (ctr_reg == INVALIDr) {
                /* For KT/KT/SB2, the loop below really refers
                   to the number of instances of the
                   memories, which is 4 for KT/KT2 and 2 for
                   Saber2 */
#if (defined BCM_KATANA2_SUPPORT || defined BCM_KATANA_SUPPORT)
                for (i = 0; i < (SOC_IS_SABER2(unit) ? 2 : 4); i++)
#else
                for (i = 0; i < NUM_PIPE(unit); i++)
#endif
                {
                    /* Do only if the Pipe mem is VALID. */
                    if (non_dma->dma_mem[i] == INVALIDm) {
                        continue;
                    }
                    if (non_dma->flags & _SOC_COUNTER_NON_DMA_DO_DMA) {

                        if (non_dma->dma_buf[i] == NULL) {
                            continue;
                        }

                        rv = soc_mem_read_range(unit, non_dma->dma_mem[i],
                                                MEM_BLOCK_ANY,
                                                non_dma->dma_index_min[i],
                                                non_dma->dma_index_max[i],
                                                non_dma->dma_buf[i]);

                        if (SOC_FAILURE(rv)) {
                            return;
                        }
                    }
                }
                dma_mem = non_dma->mem;
                base_index = non_dma->base_index;
                index_max = non_dma->num_entries;

                num_non_dma_entries = index_max;

            } else {

                /* Collect counter values for a specific port */
                dma_mem = non_dma->dma_mem[0];
                dma_base_index = port_idx - non_dma->base_index +
                                 non_dma->dma_index_min[0];

                for (i = 1; i < NUM_PIPE(unit); i++) {
                     if (non_dma->dma_buf[i] == NULL) {
                         continue;
                     }

                     if (dma_base_index > non_dma->dma_index_max[i - 1]) {
                         dma_mem = non_dma->dma_mem[i];
                         dma_base_index -= non_dma->dma_index_max[i - 1] + 1;
                         dma_base_index += non_dma->dma_index_min[i];
                     }
                }

                /* Need to retrieve the table index
                 * for flexible mapping Non-DMA counter*/
                if (non_dma->flags & _SOC_COUNTER_NON_DMA_FLEX_MAPPING) {
                    if (non_dma->soc_counter_hw_idx_get == NULL) {
                        continue;
                    }
                    rv = non_dma->soc_counter_hw_idx_get(unit, tmp_port,
                                                         dma_base_index + idx,
                                                         &qindex);
                    if (SOC_FAILURE(rv)) {
                        return;
                    }
                    if (-1 == qindex) {
                        continue;
                    }
                } else {
                    qindex = dma_base_index + idx;
#ifdef BCM_KATANA_SUPPORT
                    if (SOC_IS_KATANAX(unit) && !SOC_IS_SABER2(unit)) {
                        /* Adjust the stats table to be referenced as per
                           the computed index */
                        if ((ctr_reg == SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT) ||
                            (ctr_reg == SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE)) {
                            /* Get the table_entry based on the index range
                               4K Queues and 1K each of 'CTR_FLEX_COUNT'
                               memories */
                            tab_entry = qindex / 1024;

                            /* Point to the memory that is tied to to queue
                               index in question
                               (1->1K --> CTR_FLEX_COUNT_8m)
                               (1K->2K --> CTR_FLEX_COUNT_9m)
                               (2K->3K --> CTR_FLEX_COUNT_10m)
                               (3K->4K --> CTR_FLEX_COUNT_11m)

                               8 below refers to the start of
                               'SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT' entry
                               in _kt_cosq_mem_map/_kt2_cosq_mem_map */
                            dma_mem = _kt_cosq_mem_map[tab_entry + 8].mem;

                            /* If tab_entry is non-zero it means we are
                               referring to any of the last 3 tables
                               tied to 'SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT/BYTE',
                               index into the same correctly.
                            */
                            if (tab_entry) {
                                qindex -= (1024 * tab_entry);
                            }
                        }

                    }
#endif
                }


                rv = soc_mem_read(unit, dma_mem, MEM_BLOCK_ANY,
                                  qindex, non_dma->dma_buf[0]);


                if (SOC_FAILURE(rv)) {
                    return;
                }

                base_index = port_idx + idx;
                num_non_dma_entries = 1;

            }

            /* For flexible mapping Non-DMA counter, need to assign the counter
             * value to SW structure based the table index retrieved
             * when the routine is to collect all the values
             * for all Non-DMA counters*/
            if ((non_dma->flags & _SOC_COUNTER_NON_DMA_FLEX_MAPPING)
                    && (ctr_reg == INVALIDr)) {
                ctr_tmp_reg = SOC_COUNTER_NON_DMA_START + id;
                PBMP_ITER(soc->counter_pbmp, port) {
                    rv = _soc_counter_get_info(unit, port, ctr_tmp_reg,
                                               &port_index, &num_entries,
                                               &cname);
                    if (SOC_FAILURE(rv)) {
                        return;
                    }
                    dma_base_index = port_index - non_dma->base_index +
                                     non_dma->dma_index_min[0];
                    for (i = 0; i < num_entries; i++) {
                        if (non_dma->soc_counter_hw_idx_get == NULL) {
                            continue;
                        }
                        rv = non_dma->soc_counter_hw_idx_get(unit, port,
                             dma_base_index + i, &qindex);
                        if (SOC_FAILURE(rv)) {
                            return;
                        }
                        if (-1 == qindex) {
                            continue;
                        }

                        soc_mem_field_get(unit, dma_mem,
                            &non_dma->dma_buf[0][entry_words * qindex],
                            non_dma->field, fval);
                        if (width <= 32) {
                            COMPILER_64_SET(ctr_new, 0, fval[0]);
                        } else {
                            COMPILER_64_SET(ctr_new, fval[1], fval[0]);
                        }
                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_EXTRA_COUNT) {
                            if (!COMPILER_64_IS_ZERO(ctr_new)) {
                                COMPILER_64_SUB_32(ctr_new, 1);
                            }
                        }

                        COUNTER_ATOMIC_BEGIN(s);
                        ctr_prev = soc->counter_hw_val[port_index + i];
                        COUNTER_ATOMIC_END(s);

                        if (!(non_dma->flags
                            & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                            if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                                COUNTER_ATOMIC_BEGIN(s);
                                COMPILER_64_ZERO
                                    (soc->counter_delta[port_index + i]);
                                COUNTER_ATOMIC_END(s);
                                continue;
                            }
                        }

                        ctr_diff = ctr_new;

                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_PEAK) {
                            COUNTER_ATOMIC_BEGIN(s);
                            if (COMPILER_64_GT(ctr_new,
                                    soc->counter_sw_val[port_index + i])) {
                                soc->counter_sw_val[port_index + i] = ctr_new;
                            }
                            soc->counter_hw_val[port_index + i] = ctr_new;
                            COMPILER_64_ZERO
                                (soc->counter_delta[port_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }

                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_CURRENT) {
                            COUNTER_ATOMIC_BEGIN(s);
                            soc->counter_sw_val[port_index + i] = ctr_new;
                            soc->counter_hw_val[port_index + i] = ctr_new;
                            COMPILER_64_ZERO(soc->counter_delta[port_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }

                        if (!(non_dma->flags
                            & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                            if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                                if (width < 32) {
                                    COMPILER_64_ADD_32(ctr_diff, 1U << width);
                                } else if (width < 64) {
                                    COMPILER_64_SET(
                                        wrap_amt, 1U << (width - 32), 0);
                                    COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                                }
                            }

                            COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                        }

                        COUNTER_ATOMIC_BEGIN(s);
                        COMPILER_64_ADD_64(soc->counter_sw_val[port_index + i],
                                           ctr_diff);
                        soc->counter_hw_val[port_index + i] = ctr_new;
                        soc->counter_delta[port_index + i] = ctr_diff;
                        COUNTER_ATOMIC_END(s);
                    }
                }
            } else {
                for (i = 0; i < num_non_dma_entries; i++) {
                    soc_mem_field_get(unit, non_dma->mem,
                                      &non_dma->dma_buf[0][entry_words *
                                      i],
                                      non_dma->field, fval);
                    if (width <= 32) {
                        COMPILER_64_SET(ctr_new, 0, fval[0]);
                    } else {
                        COMPILER_64_SET(ctr_new, fval[1], fval[0]);
                    }
                    if (non_dma->flags & _SOC_COUNTER_NON_DMA_EXTRA_COUNT) {
                        if (!COMPILER_64_IS_ZERO(ctr_new)) {
                            COMPILER_64_SUB_32(ctr_new, 1);
                        }
                    }

                    COUNTER_ATOMIC_BEGIN(s);
                    ctr_prev = soc->counter_hw_val[base_index + i];
                    COUNTER_ATOMIC_END(s);

                    if (!(non_dma->flags
                        & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                        if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                            COUNTER_ATOMIC_BEGIN(s);
                            COMPILER_64_ZERO
                                (soc->counter_delta[base_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }
                    }

                    ctr_diff = ctr_new;

                    if (non_dma->flags & _SOC_COUNTER_NON_DMA_PEAK) {
                        COUNTER_ATOMIC_BEGIN(s);
                        if (COMPILER_64_GT
                                (ctr_new,
                                 soc->counter_sw_val[base_index + i])) {
                            soc->counter_sw_val[base_index + i] = ctr_new;
                        }
                        soc->counter_hw_val[base_index + i] = ctr_new;
                        COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                        COUNTER_ATOMIC_END(s);
                        continue;
                    }

                    if (non_dma->flags & _SOC_COUNTER_NON_DMA_CURRENT) {
                        COUNTER_ATOMIC_BEGIN(s);
                        soc->counter_sw_val[base_index + i] = ctr_new;
                        soc->counter_hw_val[base_index + i] = ctr_new;
                        COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                        COUNTER_ATOMIC_END(s);
                        continue;
                    }

                    if (!(non_dma->flags
                        & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                        if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                            if (width < 32) {
                                COMPILER_64_ADD_32(ctr_diff, 1U << width);
                            } else if (width < 64) {
                                COMPILER_64_SET
                                    (wrap_amt, 1U << (width - 32), 0);
                                COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                            }
                        }

                        COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                    }

                    COUNTER_ATOMIC_BEGIN(s);
                    COMPILER_64_ADD_64(soc->counter_sw_val[base_index + i],
                                       ctr_diff);
                    soc->counter_hw_val[base_index + i] = ctr_new;
                    soc->counter_delta[base_index + i] = ctr_diff;
                    COUNTER_ATOMIC_END(s);
                }
            }
        } else if (non_dma->reg != INVALIDr) {
            pbmp_t counter_pbmp;

            count = 0;
            if (non_dma->field != INVALIDf) {
                width = soc_reg_field_length(unit, non_dma->reg,
                                             non_dma->field);
            } else {
                width = 32;
            }

            /*
             * If stats retrieval is for a specific
             * port, set the pbmp for counter collection.
             */
            if (ctr_reg >= NUM_SOC_REG) {
                SOC_PBMP_CLEAR(counter_pbmp);
                port = tmp_port;
                SOC_PBMP_PORT_ADD(counter_pbmp, port);
            } else {
                SOC_PBMP_CLEAR(counter_pbmp);
                SOC_PBMP_ASSIGN(counter_pbmp, soc->counter_pbmp);
            }

            PBMP_ITER(counter_pbmp, port) {
                /*
                 * stats retrieval for a specific port.
                 */
                if (_soc_counter_non_dma_is_invalid(unit, non_dma->reg, port)) {
                    continue;
                }
                j = 0;
                subset_ct = 0;

                if (ctr_reg >= NUM_SOC_REG) {
                    if (!SOC_PBMP_MEMBER(counter_pbmp, port)) {
                        continue;
                    }

                    if (SOC_IS_TR_VL(unit) || SOC_IS_SC_CQ(unit)) {
                        if (non_dma->reg == TXLLFCMSGCNTr) {
                            if (!IS_HG_PORT( unit, port)) {
                                continue;
                            }
                        }
                    }

                    i = idx;
                    count = i + 1;
                    base_index = port_idx;
                } else {
                    if (non_dma->entries_per_port == 0) {
                        /*
                         * OK to over-write port variable as loop
                         * breaks after 1 iter.  Check below for
                         * the same codition.
                         */
                        port = REG_PORT_ANY;
                        count = non_dma->num_entries;
                        base_index = non_dma->base_index;
                    } else {
                        if (!SOC_PBMP_MEMBER(non_dma->pbmp, port)) {
                            continue;
                        }

                        if (SOC_IS_TR_VL(unit) || SOC_IS_SC_CQ(unit)) {
                            if (non_dma->reg == TXLLFCMSGCNTr) {
                                if (!IS_HG_PORT( unit, port)) {
                                    continue;
                                }
                            }
                        }

                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_PERQ_REG) {
                            count = num_cosq[unit][port];
                            base_index = non_dma->base_index;
                            for (i = 0; i < port; i++) {
                                base_index += num_cosq[unit][i];
                            }
#ifdef BCM_APACHE_SUPPORT
                        } else if (SOC_IS_APACHE(unit) &&
                                (non_dma->flags &
                                   _SOC_COUNTER_NON_DMA_OBM)) {
                            count = non_dma->entries_per_port;
                            base_index = non_dma->base_index;
                            subset_ct = non_dma->extra_ctr_ct;
                            if (non_dma->extra_ctrs) {
                                non_dma = non_dma->extra_ctrs;
                            }
#endif
                        } else if (non_dma->flags &
                                   _SOC_COUNTER_NON_DMA_OBM) {
                            count = non_dma->entries_per_port;
                            base_index = non_dma->base_index +
                                         SOC_INFO(unit).port_group[port] * count;
                        } else {
                            count = non_dma->entries_per_port;
                            base_index = non_dma->base_index + port * count;
                        }
                    }
                }

                do {
                    for (i = 0; i < count; i++) {
#ifdef BCM_APACHE_SUPPORT
                        if (SOC_IS_APACHE(unit) &&
                            (non_dma->flags & _SOC_COUNTER_NON_DMA_OBM)) {
                                soc_reg_t reg = non_dma->reg;
                                int pgw_inst = (i / _APACHE_PORTS_PER_XLP);
                                pgw_inst = (pgw_inst | SOC_REG_ADDR_INSTANCE_MASK);
                                base_index = non_dma->base_index;
                                rv = soc_reg_get(unit, reg, pgw_inst,
                                                (i % _APACHE_PORTS_PER_XLP), &ctr_new);
                        } else
#endif
#if defined(BCM_MONTEREY_SUPPORT) && defined (CPRIMOD_SUPPORT)
                        if (SOC_IS_MONTEREY(unit) &&
                            (non_dma->flags & _SOC_COUNTER_NON_DMA_CPRI_ONLY)) {
                            if ((port >=0) && (IS_ROE_PORT(unit, port))) {
                                rv = soc_reg_get(unit, non_dma->reg, port, i, &ctr_new);
                            }
                        } else
#endif
                        {
                            /* coverity[negative_returns : FALSE] */
                            rv = soc_reg_get(unit, non_dma->reg, port, i, &ctr_new);
                        }
                        if (SOC_FAILURE(rv)) {
                            return;
                        }
                        COUNTER_ATOMIC_BEGIN(s);
                        ctr_prev = soc->counter_hw_val[base_index + i];
                        COUNTER_ATOMIC_END(s);

                        if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                            COUNTER_ATOMIC_BEGIN(s);
                            COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }

                        ctr_diff = ctr_new;

                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_PEAK) {
                            COUNTER_ATOMIC_BEGIN(s);
                            if (COMPILER_64_GT(ctr_new, ctr_prev)) {
                                soc->counter_sw_val[base_index + i] = ctr_new;
                            }
                            soc->counter_hw_val[base_index + i] = ctr_new;
                            COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                        COUNTER_ATOMIC_END(s);
                            continue;
                        }

                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_CURRENT) {
                            COUNTER_ATOMIC_BEGIN(s);
                            soc->counter_sw_val[base_index + i] = ctr_new;
                            soc->counter_hw_val[base_index + i] = ctr_new;
                            COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }
#ifdef BCM_MONTEREY_SUPPORT
                        /*
                         * The counter with clear on read, there is no need to check
                         * against the previous counter.  It can be used as is.
                         * Only non clear on read counter need to process with previous
                         * counter to check for increment as well as wrap around.
                         */
                        if (SOC_IS_MONTEREY(unit)) {
                            if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                                if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                                    if (width < 32) {
                                        COMPILER_64_ADD_32(ctr_diff, 1U << width);
                                    } else if (width < 64) {
                                        COMPILER_64_SET(wrap_amt, 1U << (width - 32), 0);
                                        COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                                    }
                                }
                                COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                            }
                        } else
#endif
                        {
                           if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                                if (width < 32) {
                                    COMPILER_64_ADD_32(ctr_diff, 1U << width);
                                } else if (width < 64) {
                                    COMPILER_64_SET(wrap_amt, 1U << (width - 32), 0);
                                    COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                                }
                            }

                            COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                        }

                        COUNTER_ATOMIC_BEGIN(s);
                        COMPILER_64_ADD_64(soc->counter_sw_val[base_index + i],
                                        ctr_diff);
                        soc->counter_hw_val[base_index + i] = ctr_new;
                        soc->counter_delta[base_index + i] = ctr_diff;
                        COUNTER_ATOMIC_END(s);
                    }
                    j++;
                    if ((subset_ct > 1) && (j < subset_ct)) {
                        non_dma += 2;
                    }
                } while ((j < subset_ct) && non_dma);
                if ((non_dma->entries_per_port == 0) ||
                    (subset_ct != 0)) {
                    break;
                }
            }
        } else {
            continue;
        }
    }
}

#ifdef BCM_TOMAHAWK_SUPPORT
STATIC void
soc_counter_collect_th_non_dma_entries(int unit,
                                    int tmp_port,
                                    int port_idx,
                                    int ar_idx,
                                    soc_reg_t ctr_reg)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    COUNTER_ATOMIC_DEF s = SOC_CONTROL(unit);
    soc_mem_t dma_mem;
    int base_index, width, i, j, id = 0, idx = 0, obm_flag = 0, port;
    uint32 fval[2], efp_ctr_rval;
    uint64 ctr_new, ctr_prev, ctr_diff, wrap_amt;
    int rv, count, entry_words;
    int dma_base_index, num_non_dma_entries = 0;
    int non_dma_counter_end = SOC_COUNTER_NON_DMA_END -
                              SOC_COUNTER_NON_DMA_START;
    static int ctr_dma_ct = 0;
    int index_max = 0, max_pipe, pipe, default_entries_per_pipe = 0;
    uint32 subset_ct = 0; /* For Parent-Child Model */
    int qindex, port_index, num_entries;
    char *cname;
    soc_reg_t ctr_tmp_reg;
    uint8 error_return = 0;

    soc = SOC_CONTROL(unit);

    if (soc->counter_non_dma == NULL) {
        return;
    }

    max_pipe = SOC_CONTROL(unit)->max_num_pipe; /* store locally */

    if (soc_feature(unit, soc_feature_th_a0_sw_war) &&
        soc->ctr_evict_interval) {
        soc_reg_t reg = EGR_EFP_CNTR_UPDATE_CONTROLr;
        efp_ctr_rval = 0;
        rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &efp_ctr_rval);
        if (SOC_FAILURE(rv)) {
            return;
        }
        soc_reg_field_set(unit, reg, &efp_ctr_rval, CLR_ON_READf, 1);
        rv = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, efp_ctr_rval);
        if (SOC_FAILURE(rv)) {
            return;
        }
    }

    /*
     * if ctr_reg != -1, retrieve stats for a
     * specific port and queue from the hardware
     * update the sw copy of the same.
     */
    if (ctr_reg >= NUM_SOC_REG) {
        id = ctr_reg - SOC_COUNTER_NON_DMA_START;
        non_dma_counter_end = id + 1;
        idx = (ar_idx < 0)? 0: ar_idx;
    }

    for (; id < non_dma_counter_end; id++) {
        default_entries_per_pipe = 0;
        non_dma = &soc->counter_non_dma[id];

        if(non_dma == NULL) {
            continue;
        }

        if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
            continue;
        }

        /* If signaled to exit then return  */
        if (!soc->counter_interval) {
            error_return = 1;
            goto done;
        }

        width = entry_words = 0;

        if (non_dma->mem != INVALIDm) {
            count = 0;
            /* If there is subset of counters, use subset info for DMA */
            if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                (non_dma->extra_ctrs != NULL)) {
#ifdef BCM_TOMAHAWK3_SUPPORT
                if(SOC_IS_TOMAHAWK3(unit) && (non_dma->extra_ctr_fld_ct != 0)) {
                    subset_ct = non_dma->extra_ctr_ct;
                } else
#endif
                {
                /* Do PACKET_CTRf and BYTE_CTRf sequentially. Hence 2x loop */
                subset_ct = non_dma->extra_ctr_ct << 1;
                }
                non_dma = non_dma->extra_ctrs;
            } else {
                subset_ct = 1;
            }

            do {
                /* If signaled to exit then return  */
                if (!soc->counter_interval) {
                    error_return = 1;
                    goto done;
                }
                if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
                    count++;
                    non_dma++;
                    continue;
                }

                /* Check if this Counter is up for DMA.
                 * During Slow-DMA, not all counters are up for DMA at the same
                 * time/frequency.
                 */
                if ((non_dma->flags & _SOC_COUNTER_NON_DMA_CTR_EVICT) &&
                    (!soc_counter_non_dma_ready(unit, non_dma, &ctr_dma_ct))) {
                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit,
                                         "id %d(%d): NOT pickd for DMA(pipes %d). Ct %d,mem %d/%d/%d\n"),
                              id, SOC_COUNTER_NON_DMA_START, count,
                              max_pipe, non_dma->mem,
                              non_dma->dma_mem[0], non_dma->dma_mem[1]));
                    count++;
                    non_dma++;
                    continue;
                }
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit)) {
                    if ((non_dma->flags & _SOC_COUNTER_NON_DMA_DO_DMA) &&
                        count == 0) {
                        COUNTER_UNLOCK(unit);
                        sal_thread_yield();
                        COUNTER_LOCK(unit);
                    }
                } else
#endif
                {
                    if (non_dma->flags & _SOC_COUNTER_NON_DMA_DO_DMA) {
                        COUNTER_UNLOCK(unit);
                        sal_thread_yield();
                        COUNTER_LOCK(unit);
                    }
                }
                LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                         (BSL_META_U(unit,
                                     "id %d(%d): picked for DMA(Pipes %d). mem %d/%d/%d\n"),
                          id, SOC_COUNTER_NON_DMA_START, max_pipe,
                          non_dma->mem, non_dma->dma_mem[0],
                          non_dma->dma_mem[1]));

                entry_words = soc_mem_entry_words(unit, non_dma->mem);
                if (non_dma->field != INVALIDf) {
                    width = soc_mem_field_length(unit, non_dma->mem,
                                                 non_dma->field);
                } else {
                    width = 32;
                }

                    if (non_dma->dma_mem[0] != INVALIDm) {
                    default_entries_per_pipe =
                        soc_mem_index_count(unit, non_dma->dma_mem[0]);
                    }


                if (ctr_reg == INVALIDr) {
                    for (i = 0; i < max_pipe; i++) {
                        /* Do only if the Pipe mem is VALID. */
                        if (non_dma->dma_mem[i] == INVALIDm) {
                            continue;
                        }
                        /* Skip if no enabled ports in pipe */
                        if (SOC_PBMP_IS_NULL(PBMP_PIPE(unit, i))) {
                            continue;
                        }
                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_DO_DMA) {

                            if (non_dma->dma_buf[i] == NULL) {
                                continue;
                            }

                            rv = soc_mem_read_range(unit, non_dma->dma_mem[i],
                                                    MEM_BLOCK_ANY,
                                                    non_dma->dma_index_min[i],
                                                    non_dma->dma_index_max[i],
                                                    non_dma->dma_buf[i]);

                            if (SOC_FAILURE(rv)) {
                                error_return = 1;
                                goto done;
                            }
                        }
                    }
                    dma_mem = non_dma->mem;
                    base_index = non_dma->base_index;
                    index_max = non_dma->num_entries;
                    num_non_dma_entries = index_max;

                    /* Set range over which field values from h/w are to be
                     * extracted for color drop counters*/
                    if (non_dma->mem == MMU_CTR_COLOR_DROP_MEMm) {
                        if(SOC_IS_TRIDENT3(unit) || SOC_IS_MAVERICK2(unit)) {
                            default_entries_per_pipe = 66;
                        } else if (SOC_IS_TOMAHAWK2(unit)) {
                            default_entries_per_pipe = 34;
                        } else if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
                            default_entries_per_pipe = 68;
                        } else {
                            default_entries_per_pipe = 68;
                        }
                        num_non_dma_entries = default_entries_per_pipe;
                    }

                } else {

                    /* Collect counter values for a specific port */
                    dma_mem = non_dma->dma_mem[0];
                    dma_base_index = port_idx - non_dma->base_index +
                                     non_dma->dma_index_min[0];

                    for (i = 1; i < NUM_PIPE(unit); i++) {
                         if (non_dma->dma_buf[i] == NULL) {
                             continue;
                         }

                         if (dma_base_index > non_dma->dma_index_max[i - 1]) {
                             dma_mem = non_dma->dma_mem[i];
                             dma_base_index -= non_dma->dma_index_max[i - 1] + 1;
                             dma_base_index += non_dma->dma_index_min[i];
                         }
                    }

                    /* Need to retrieve the table index
                     * for flexible mapping Non-DMA counter*/
                    if (non_dma->flags & _SOC_COUNTER_NON_DMA_FLEX_MAPPING) {
                        if (non_dma->soc_counter_hw_idx_get == NULL) {
                            continue;
                        }
                        rv = non_dma->soc_counter_hw_idx_get(unit, tmp_port,
                                                             dma_base_index + idx,
                                                             &qindex);
                        if (SOC_FAILURE(rv)) {
                            error_return = 1;
                            goto done;
                        }
                        if (-1 == qindex) {
                            continue;
                        }
                    } else {
                        qindex = dma_base_index + idx;
                    }

                    rv = soc_mem_read(unit, dma_mem, MEM_BLOCK_ANY,
                                      qindex, non_dma->dma_buf[0]);


                    if (SOC_FAILURE(rv)) {
                        error_return = 1;
                        goto done;
                    }

                    /* coverity[UNUSED_VALUE: FALSE] */
                    base_index = port_idx + idx;
                    num_non_dma_entries = 1;
                }

                /* For flexible mapping Non-DMA counter, need to assign the counter
                 * value to SW structure based the table index retrieved
                 * when the routine is to collect all the values
                 * for all Non-DMA counters*/
                if ((non_dma->flags & _SOC_COUNTER_NON_DMA_FLEX_MAPPING)
                        && (ctr_reg == INVALIDr)) {
                    ctr_tmp_reg = SOC_COUNTER_NON_DMA_START + id;
                    PBMP_ITER(soc->counter_pbmp, port) {
                        rv = _soc_counter_get_info(unit, port, ctr_tmp_reg,
                                                   &port_index, &num_entries,
                                                   &cname);
                        if (SOC_FAILURE(rv)) {
                            error_return = 1;
                            goto done;
                        }
                        dma_base_index = port_index - non_dma->base_index +
                                         non_dma->dma_index_min[0];
                        for (i = 0; i < num_entries; i++) {
                            if (non_dma->soc_counter_hw_idx_get == NULL) {
                                continue;
                            }
                            rv = non_dma->soc_counter_hw_idx_get(unit, port,
                                 dma_base_index + i, &qindex);
                            if (SOC_FAILURE(rv)) {
                                error_return = 1;
                                goto done;
                            }
                            if (-1 == qindex) {
                                continue;
                            }

                            soc_mem_field_get(unit, dma_mem,
                                &non_dma->dma_buf[0][entry_words * qindex],
                                non_dma->field, fval);
                            if (width <= 32) {
                                COMPILER_64_SET(ctr_new, 0, fval[0]);
                            } else {
                                COMPILER_64_SET(ctr_new, fval[1], fval[0]);
                            }
                            if (non_dma->flags & _SOC_COUNTER_NON_DMA_EXTRA_COUNT) {
                                if (!COMPILER_64_IS_ZERO(ctr_new)) {
                                    COMPILER_64_SUB_32(ctr_new, 1);
                                }
                            }

                            COUNTER_ATOMIC_BEGIN(s);
                            ctr_prev = soc->counter_hw_val[port_index + i];
                            COUNTER_ATOMIC_END(s);

                            if (!(non_dma->flags
                                & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                                if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                                    COUNTER_ATOMIC_BEGIN(s);
                                    COMPILER_64_ZERO
                                        (soc->counter_delta[port_index + i]);
                                    COUNTER_ATOMIC_END(s);
                                    continue;
                                }
                            }

                            ctr_diff = ctr_new;

                            if (non_dma->flags & _SOC_COUNTER_NON_DMA_PEAK) {
                                COUNTER_ATOMIC_BEGIN(s);
                                if (COMPILER_64_GT(ctr_new,
                                        soc->counter_sw_val[port_index + i])) {
                                    soc->counter_sw_val[port_index + i] = ctr_new;
                                }
                                soc->counter_hw_val[port_index + i] = ctr_new;
                                COMPILER_64_ZERO
                                    (soc->counter_delta[port_index + i]);
                                COUNTER_ATOMIC_END(s);
                                continue;
                            }

                            if (non_dma->flags & _SOC_COUNTER_NON_DMA_CURRENT) {
                                COUNTER_ATOMIC_BEGIN(s);
                                soc->counter_sw_val[port_index + i] = ctr_new;
                                soc->counter_hw_val[port_index + i] = ctr_new;
                                COMPILER_64_ZERO(soc->counter_delta[port_index + i]);
                                COUNTER_ATOMIC_END(s);
                                continue;
                            }

                            if (!(non_dma->flags
                                & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                                if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                                    if (width < 32) {
                                        COMPILER_64_ADD_32(ctr_diff, 1U << width);
                                    } else if (width < 64) {
                                        COMPILER_64_SET(
                                            wrap_amt, 1U << (width - 32), 0);
                                        COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                                    }
                                }

                                COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                            }

                            COUNTER_ATOMIC_BEGIN(s);
                            COMPILER_64_ADD_64(soc->counter_sw_val[port_index + i],
                                               ctr_diff);
                            soc->counter_hw_val[port_index + i] = ctr_new;
                            soc->counter_delta[port_index + i] = ctr_diff;
                            COUNTER_ATOMIC_END(s);
                        }
                    }
                } else {
                    int invalid_pipe_ct = 0;
                    int entries_per_pipe[SOC_MAX_NUM_PIPES];
                    for (pipe = 0; pipe < max_pipe; pipe++) {
                        if (!num_non_dma_entries) {
                            break;
                        }
                        if (!non_dma->dma_buf[pipe]) {
                            /* Skip the pipe indices if dma_buf for given pipe
                             * is NULL.
                             */
                            invalid_pipe_ct++;
                            continue;
                        }

                        entries_per_pipe[pipe] = default_entries_per_pipe;
                        base_index = non_dma->base_index +
                                    (pipe - invalid_pipe_ct) * default_entries_per_pipe;

#ifdef BCM_TOMAHAWK3_SUPPORT
                        if (SOC_IS_TOMAHAWK3(unit)) {
                            int pipe2 = 0;
                            if (SOC_CONTROL(unit)->halfchip == 1 &&
                                non_dma->dma_mem[pipe] == INVALIDm) {
                                continue;
                            }
                            entries_per_pipe[pipe] = non_dma->dma_num_entries[pipe];
                            base_index = non_dma->base_index;
                            for(pipe2 = 1; pipe2 <= pipe; pipe2++) {
                                base_index += non_dma->dma_num_entries[pipe2 - 1];
                            }
                        }
#endif
                        /* Skip if no enabled ports in pipe */
                        if (SOC_PBMP_IS_NULL(PBMP_PIPE(unit, pipe))) {
                            continue;
                        }

                        for (i = 0 ; i < entries_per_pipe[pipe]; i++) {
                            soc_mem_field_get(unit, non_dma->mem,
                                      &non_dma->dma_buf[pipe][entry_words * i],
                                      non_dma->field, fval);
                            if (width <= 32) {
                                COMPILER_64_SET(ctr_new, 0, fval[0]);
                            } else {
                                COMPILER_64_SET(ctr_new, fval[1], fval[0]);
                            }
                            if (non_dma->flags & _SOC_COUNTER_NON_DMA_EXTRA_COUNT) {
                                if (!COMPILER_64_IS_ZERO(ctr_new)) {
                                    COMPILER_64_SUB_32(ctr_new, 1);
                                }
                            }

                            COUNTER_ATOMIC_BEGIN(s);
                            ctr_prev = soc->counter_hw_val[base_index + i];
                            COUNTER_ATOMIC_END(s);

                            if (!(non_dma->flags
                                & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                                if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                                    COUNTER_ATOMIC_BEGIN(s);
                                    COMPILER_64_ZERO
                                        (soc->counter_delta[base_index + i]);
                                    COUNTER_ATOMIC_END(s);
                                    continue;
                                }
                            }

                            ctr_diff = ctr_new;

                            if (non_dma->flags & _SOC_COUNTER_NON_DMA_PEAK) {
                                COUNTER_ATOMIC_BEGIN(s);
                                if (COMPILER_64_GT
                                        (ctr_new,
                                         soc->counter_sw_val[base_index + i])) {
                                    soc->counter_sw_val[base_index + i] = ctr_new;
                                }
                                soc->counter_hw_val[base_index + i] = ctr_new;
                                COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                                COUNTER_ATOMIC_END(s);
                                continue;
                            }

                            if (non_dma->flags & _SOC_COUNTER_NON_DMA_CURRENT) {
                                COUNTER_ATOMIC_BEGIN(s);
                                soc->counter_sw_val[base_index + i] = ctr_new;
                                soc->counter_hw_val[base_index + i] = ctr_new;
                                COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                                COUNTER_ATOMIC_END(s);
                                continue;
                            }

                            if (!(non_dma->flags
                                & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                                if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                                    if (width < 32) {
                                        COMPILER_64_ADD_32(ctr_diff, 1U << width);
                                    } else if (width < 64) {
                                        COMPILER_64_SET
                                            (wrap_amt, 1U << (width - 32), 0);
                                        COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                                    }
                                }

                                COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                            }

                            COUNTER_ATOMIC_BEGIN(s);
                            COMPILER_64_ADD_64(soc->counter_sw_val[base_index + i],
                                               ctr_diff);
                            if (non_dma->flags & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ) {
                                ctr_new = soc->counter_sw_val[base_index + i];
                                ctr_diff = ctr_new;
                                COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                            }
                            soc->counter_hw_val[base_index + i] = ctr_new;
                            soc->counter_delta[base_index + i] = ctr_diff;
                            COUNTER_ATOMIC_END(s);
                        }
                    }
                }
                count++;
                non_dma++;
            } while ((count < subset_ct) && non_dma);
        } else if (non_dma->reg != INVALIDr) {
            int mmu_xpe_ctr = 0;
            int mmu_itm_ctr = 0;
            pbmp_t counter_pbmp;
            soc_counter_non_dma_t *non_dma_tmp = non_dma;

            count = 0;
            if (non_dma->field != INVALIDf) {
                width = soc_reg_field_length(unit, non_dma->reg,
                                             non_dma->field);
            } else {
                width = 32;
            }

            /*
             * If stats retrieval is for a specific
             * port, set the pbmp for counter collection.
             */
            if (ctr_reg >= NUM_SOC_REG) {
                SOC_PBMP_CLEAR(counter_pbmp);
                port = tmp_port;
                SOC_PBMP_PORT_ADD(counter_pbmp, port);
            } else {
                SOC_PBMP_CLEAR(counter_pbmp);
                SOC_PBMP_ASSIGN(counter_pbmp, soc->counter_pbmp);
            }

            obm_flag = 0;
            PBMP_ITER(counter_pbmp, port) {
                subset_ct = 1;
                /*
                 * stats retrieval for a specific port.
                 */

                j = 0;
                if (ctr_reg >= NUM_SOC_REG) {
                    if (!SOC_PBMP_MEMBER(counter_pbmp, port)) {
                        continue;
                    }

                    i = idx;
                    count = i + 1;
                    base_index = port_idx;
                } else {
                    if (non_dma->entries_per_port == 0) {
                        /*
                         * OK to over-write port variable as loop
                         * breaks after 1 iter.  Check below for
                         * the same codition.
                         */
                        port = REG_PORT_ANY;
                        count = non_dma->num_entries;
                        base_index = non_dma->base_index;
                        if ((non_dma->id == SOC_COUNTER_NON_DMA_DROP_RQ_PKT) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_DROP_RQ_BYTE) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_DROP_RQ_PKT_YELLOW) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_DROP_RQ_PKT_RED) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_POOL_PEAK) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_POOL_CURRENT) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_HDRM_POOL_PEAK) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_HDRM_POOL_CURRENT) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_EGRESS_POOL_PEAK) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_EGRESS_POOL_CURRENT) ||
                            (non_dma->id == SOC_COUNTER_NON_DMA_QGROUP_MIN_CURRENT)) {
                            mmu_xpe_ctr = 1;
                        } else if ((non_dma->id == SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT) ||
                                   (non_dma->id == SOC_COUNTER_NON_DMA_PRIQ_DROP_BYTE) ||
                                   (non_dma->id == SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_YELLOW) ||
                                   (non_dma->id == SOC_COUNTER_NON_DMA_PRIQ_DROP_PKT_RED) ||
                                   (non_dma->id == SOC_COUNTER_NON_DMA_HDRM_POOL_DROP_PKT)) {
                            mmu_itm_ctr = 1;
                        }
                    } else {
                        if (!SOC_PBMP_MEMBER(non_dma->pbmp, port)) {
                            continue;
                        }

                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_PERQ_REG) {
                            count = num_cosq[unit][port];
                            base_index = non_dma->base_index;
                            for (i = 0; i < port; i++) {
                                base_index += num_cosq[unit][i];
                            }
                        } else if (non_dma->flags &
                                   _SOC_COUNTER_NON_DMA_OBM) {
                            if (obm_flag == 0) {
                                count = non_dma->entries_per_port * max_pipe;
                                base_index = non_dma->base_index;
                                obm_flag = 1; /* covers all ports for the
                                               * current obm */
                                subset_ct = non_dma->extra_ctr_ct;
                                if (non_dma->extra_ctrs) {
                                    non_dma_tmp = non_dma->extra_ctrs;
                                }
                            } else {
                                break;
                            }
                        } else {
                            count = non_dma->entries_per_port;
                            base_index = non_dma->base_index + port * count;
                        }
                    }
                }

                do {
                    for (i = 0; i < count; i++) {
                        if (non_dma_tmp->flags & _SOC_COUNTER_NON_DMA_OBM) {
                            soc_reg_t reg = non_dma_tmp->reg;
#ifdef BCM_TOMAHAWK3_SUPPORT
                            int phy_port, obm_port, is_active;
#endif
                            /* coverity[divide_by_zero : FALSE] */
                            pipe = i / non_dma->entries_per_port;
                            /* Skip if no enabled ports in pipe */
                            if (SOC_PBMP_IS_NULL(PBMP_PIPE(unit, pipe))) {
                                continue;
                            }
                            reg = SOC_REG_UNIQUE_ACC(unit, non_dma_tmp->reg)[pipe];
                            base_index = non_dma_tmp->base_index;
#ifdef BCM_TOMAHAWK3_SUPPORT
                            if (SOC_IS_TOMAHAWK3(unit)) {
                                obm_port = i % non_dma->entries_per_port;
                                phy_port =
                                    soc_counter_tomahawk3_obm_drop_to_phy_port(unit, pipe, reg, obm_port);
                                if (phy_port < 0 ||
                                        SOC_INFO(unit).port_p2l_mapping[phy_port] == -1) {
                                    continue;
                                }

                                rv = soc_th3_portctrl_pm_is_active(unit, phy_port, &is_active);
                                if (SOC_FAILURE(rv)) {
                                    error_return = 1;
                                    goto done;
                                }

                                /* Skip counters for this PM if it's not initialized */
                                if (!is_active) {
                                    continue;
                                }

                                rv = soc_reg_get(unit, reg, pipe, obm_port, &ctr_new);
                            } else
#endif
                            {
                                rv = soc_reg_get(unit, reg, pipe,
                                             i % non_dma->entries_per_port, &ctr_new);
                            }
                        } else {
                            /* coverity[negative_returns : FALSE] */
                            if (mmu_xpe_ctr == 1) {
                                rv = soc_reg_get(unit, non_dma_tmp->reg,
                                                 (i / (count / NUM_XPE(unit))),
                                                 (i % (count / NUM_XPE(unit))), &ctr_new);
                            } else if (mmu_itm_ctr == 1) {
                                /* Skip if inactive ITM */
                                if(((1 << (i / (count / NUM_ITM(unit)))) &
                                   soc->info.itm_map) == 0) {
                                    continue;
                                }

                                rv = soc_reg_get(unit, non_dma_tmp->reg,
                                                 (i / (count / NUM_ITM(unit))),
                                                 (i % (count / NUM_ITM(unit))), &ctr_new);
                            } else {
                                rv = soc_reg_get(unit, non_dma_tmp->reg, port, i, &ctr_new);
                            }
                        }
                        if (SOC_FAILURE(rv)) {
                            error_return = 1;
                            goto done;
                        }
                        COUNTER_ATOMIC_BEGIN(s);
                        ctr_prev = soc->counter_hw_val[base_index + i];
                        COUNTER_ATOMIC_END(s);

                        if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                            COUNTER_ATOMIC_BEGIN(s);
                            COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }

                        ctr_diff = ctr_new;

                        if (non_dma_tmp->flags & _SOC_COUNTER_NON_DMA_PEAK) {
                            COUNTER_ATOMIC_BEGIN(s);
                            if (COMPILER_64_GT(ctr_new,
                                               soc->counter_sw_val[base_index + i])) {
                                soc->counter_sw_val[base_index + i] = ctr_new;
                            }
                            soc->counter_hw_val[base_index + i] = ctr_new;
                            COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }

                        if (non_dma_tmp->flags & _SOC_COUNTER_NON_DMA_CURRENT) {
                            COUNTER_ATOMIC_BEGIN(s);
                            soc->counter_sw_val[base_index + i] = ctr_new;
                            soc->counter_hw_val[base_index + i] = ctr_new;
                            COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }

                        if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                            if (width < 32) {
                                COMPILER_64_ADD_32(ctr_diff, 1U << width);
                            } else if (width < 64) {
                                COMPILER_64_SET(wrap_amt, 1U << (width - 32), 0);
                                COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                            }
                        }

                        COMPILER_64_SUB_64(ctr_diff, ctr_prev);

                        COUNTER_ATOMIC_BEGIN(s);
                        COMPILER_64_ADD_64(soc->counter_sw_val[base_index + i],
                                           ctr_diff);
                        soc->counter_hw_val[base_index + i] = ctr_new;
                        soc->counter_delta[base_index + i] = ctr_diff;
                        COUNTER_ATOMIC_END(s);
                    }
                    j++;
                    if (subset_ct > 1) {
                        non_dma_tmp += 2;
                    }
                } while ((j < subset_ct) && non_dma_tmp);

                if (non_dma->entries_per_port == 0) {
                    break;
                }
            }
            COUNTER_UNLOCK(unit);
            sal_thread_yield();
            COUNTER_LOCK(unit);
        }
    }

done:
    if (soc_feature(unit, soc_feature_th_a0_sw_war) &&
        soc->ctr_evict_interval) {
        soc_reg_t reg = EGR_EFP_CNTR_UPDATE_CONTROLr;
        efp_ctr_rval = 0;
        rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &efp_ctr_rval);
        if (SOC_FAILURE(rv)) {
            return;
        }

        soc_reg_field_set(unit, reg, &efp_ctr_rval, CLR_ON_READf, 0);
        rv = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, efp_ctr_rval);
        if (SOC_FAILURE(rv)) {
            return;
        }
    }

    if (error_return) {
        return;
    }
    ctr_dma_ct++;
}
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
/*
 * Function:
 *      _soc_xgs3_counter_dma_setup
 * Purpose:
 *      Configure hardware registers for counter collection.  Used during
 *      soc_counter_thread initialization.
 * Parameters:
 *      unit    - switch unit
 * Returns:
 *      SOC_E_*
 */
STATIC int
_soc_xgs3_counter_dma_setup(int unit)
{
    soc_control_t       *soc;
    int                 csize;
    pbmp_t              pbmp;
    soc_reg_t           reg;
    soc_ctr_type_t      ctype;
    uint32              val;
    uint32              ing_blk, ing_blkoff, ing_pstage, ing_c_cnt;
    uint32              egr_blk, egr_blkoff, egr_pstage, egr_c_cnt;
#ifdef BCM_SHADOW_SUPPORT
    uint32              inv_c_cnt = 0;
#endif
    uint32              gmac_c_cnt, xmac_c_cnt;
    int                 blk, bindex, blk_num, port, phy_port;
    soc_reg_t           blknum_reg, portnum_reg;
    int                 num_blknum, num_portnum, blknum_offset, portnum_offset;
    uint32              blknum_mask, portnum_mask;
    uint32              blknum_map[20];
    uint32              portnum_map[24];
#ifdef BCM_CMICM_SUPPORT
    int cmc = SOC_PCI_CMC(unit);
#endif
    ing_blk = ing_blkoff = ing_pstage = 0;
    egr_blk = egr_blkoff = egr_pstage = 0;

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        ing_blk = SOC_BLK_IPIPE;
        egr_blk = SOC_BLK_EPIPE;
        ing_blkoff = SOC_BLOCK2OFFSET(unit, IPIPE_BLOCK(unit));
        egr_blkoff = SOC_BLOCK2OFFSET(unit, EPIPE_BLOCK(unit));
    }
#endif

    soc = SOC_CONTROL(unit);
    SOC_PBMP_ASSIGN(pbmp, soc->counter_pbmp);
    /*
     * Calculate the number of counters from each block
     */
    ing_c_cnt = egr_c_cnt = gmac_c_cnt = xmac_c_cnt = 0;
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        inv_c_cnt = ing_c_cnt;
    }
#endif
    for (ctype = 0; ctype < SOC_CTR_NUM_TYPES; ctype++) {
        if (!SOC_HAS_CTR_TYPE(unit, ctype)) {
            continue;
        }
        for (csize = SOC_CTR_MAP_SIZE(unit, ctype) - 1; csize > 0; csize--) {
            reg = SOC_CTR_TO_REG(unit, ctype, csize);
            if (SOC_COUNTER_INVALID(unit, reg)) {
                continue;                       /* skip trailing invalids */
            }
            if (SOC_REG_BLOCK_IS(unit, reg, ing_blk) &&
                                 (ing_c_cnt == 0)) {
                ing_c_cnt = csize + 1;
                if (soc_feature(unit, soc_feature_new_sbus_format)) {
                    ing_pstage = (SOC_REG_INFO(unit, reg).offset >> 26) & 0x3F;
                } else {
                    ing_pstage = (SOC_REG_INFO(unit, reg).offset >> 24) & 0xFF;
                }
            }
            if (SOC_REG_BLOCK_IS(unit, reg, egr_blk) &&
                                 (egr_c_cnt == 0)) {
                egr_c_cnt = csize + 1;
                if (soc_feature(unit, soc_feature_new_sbus_format)) {
                    egr_pstage = (SOC_REG_INFO(unit, reg).offset >> 26) & 0x3F;
                } else {
                    egr_pstage = (SOC_REG_INFO(unit, reg).offset >> 24) & 0xFF;
                }
            }
            if ((SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_QGPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_SPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XGPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XQPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXFPPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORTB0) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XTPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XWPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLG2PORT)) &&
                (gmac_c_cnt == 0)) {
                gmac_c_cnt = csize + 1;
            }
            if ((SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XGPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XQPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORTB0) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XTPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XWPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MXQPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT) ||
                 SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLG2PORT)) &&
                (xmac_c_cnt == 0)) {
                xmac_c_cnt = csize + 1;
            }
        }
    }
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        /* Remove leading invalids (Shadow) */
        for (csize = 0; csize < SOC_CTR_MAP_SIZE(unit, SOC_CTR_TYPE_XE); csize++) {
            reg = SOC_CTR_TO_REG(unit, SOC_CTR_TYPE_XE, csize);
            if (SOC_COUNTER_INVALID(unit, reg)) {
                inv_c_cnt++;
            } else {
                break;
            }
        }
    }
#endif /* BCM_SHADOW_SUPPORT */

    egr_c_cnt = (egr_c_cnt) ? (egr_c_cnt - ing_c_cnt) : 0;
    gmac_c_cnt = (gmac_c_cnt) ? (gmac_c_cnt - egr_c_cnt - ing_c_cnt) : 0;
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (!SOC_CONTROL(unit)->sw_timestamp_fifo_enable) {
        xmac_c_cnt = DIRECT_TIMESTAMP_DMA_COUNTER_NUM;
    } else
#endif
    {
        xmac_c_cnt = (xmac_c_cnt) ? (xmac_c_cnt - egr_c_cnt - ing_c_cnt) : 0;
    }
#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        ing_c_cnt -= inv_c_cnt;
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "ing_c_cnt = %d egr_c_cnt = %d "
                         "gmac_c_cnt = %d xmac_c_cnt = %d\n"
                         "ing_pstage = %d egr_pstage = %d "
                         "ing_blkoff = %d egr_blkoff = %d\n"),
              ing_c_cnt, egr_c_cnt, gmac_c_cnt, xmac_c_cnt,
              ing_pstage, egr_pstage, ing_blkoff, egr_blkoff));

    /* Prepare for CMIC_STAT_DMA_PORTNUM_MAP and BLKNUM_MAP */
    if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORTNUM_MAP_3_0r)) {
        portnum_reg = CMIC_STAT_DMA_PORTNUM_MAP_3_0r;
        num_portnum = 4;
        portnum_offset = 8;
#if defined(BCM_HB_GW_SUPPORT)
    } else if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORTNUM_MAP_5_0r)) {
        portnum_reg = CMIC_STAT_DMA_PORTNUM_MAP_5_0r;
        num_portnum = 6;
        portnum_offset = 5;
#endif
    } else if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORTNUM_MAP_7_0r)) {
        portnum_reg = CMIC_STAT_DMA_PORTNUM_MAP_7_0r;
        num_portnum = 8;
        portnum_offset = 4;
    } else {
        return SOC_E_INTERNAL;
    }
    portnum_mask =
        (1 << soc_reg_field_length(unit, portnum_reg, SBUS_PORTNUM_0f)) - 1;

    if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_BLKNUM_MAP_4_0r)) {
        blknum_reg = CMIC_STAT_DMA_BLKNUM_MAP_4_0r;
        num_blknum = 5;
        blknum_offset = 6;
    } else if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_BLKNUM_MAP_7_0r)) {
        blknum_reg = CMIC_STAT_DMA_BLKNUM_MAP_7_0r;
        num_blknum = 8;
        blknum_offset = 4;
    } else {
        return SOC_E_INTERNAL;
    }
    blknum_mask =
        (1 << soc_reg_field_length(unit, blknum_reg, SBUS_BLKNUM_0f)) - 1;

    sal_memset(portnum_map, 0, sizeof(portnum_map));
    sal_memset(blknum_map, 0, sizeof(blknum_map));
    PBMP_ITER(pbmp, port) {
        /* Check port value to avoid out of bound array access */
        if ((port / num_blknum) >= 20) {
            return SOC_E_INTERNAL;
        }
        if ((port / num_portnum) >= 24) {
            return SOC_E_INTERNAL;
        }
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        blk = SOC_PORT_BLOCK(unit, phy_port);
        bindex = SOC_PORT_BINDEX(unit, phy_port);
        blk_num = SOC_BLOCK2OFFSET(unit, blk);
        /* coverity[overrun-local] */
        blknum_map[port / num_blknum] |=
            (blk_num & blknum_mask) << (blknum_offset * (port % num_blknum));
        /* coverity[overrun-local] */
        portnum_map[port / num_portnum] |=
            (bindex & portnum_mask) << (portnum_offset * (port % num_portnum));
    }

    switch (portnum_reg) {
    case CMIC_STAT_DMA_PORTNUM_MAP_3_0r:
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_3_0r(unit, portnum_map[0]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_7_4r(unit, portnum_map[1]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_11_8r(unit, portnum_map[2]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_15_12r(unit, portnum_map[3]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_19_16r(unit, portnum_map[4]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_23_20r(unit, portnum_map[5]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_27_24r(unit, portnum_map[6]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_31_28r(unit, portnum_map[7]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_35_32r(unit, portnum_map[8]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_39_36r(unit, portnum_map[9]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_43_40r(unit, portnum_map[10]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_47_44r(unit, portnum_map[11]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_51_48r(unit, portnum_map[12]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_55_52r(unit, portnum_map[13]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_59_56r(unit, portnum_map[14]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_63_60r(unit, portnum_map[15]);
        if (!SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORTNUM_MAP_67_64r)) {
            break;
        }
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_67_64r(unit, portnum_map[16]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_71_68r(unit, portnum_map[17]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_75_72r(unit, portnum_map[18]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_79_76r(unit, portnum_map[19]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_83_80r(unit, portnum_map[20]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_87_84r(unit, portnum_map[21]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_91_88r(unit, portnum_map[22]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_95_92r(unit, portnum_map[23]);
        break;
#if defined(BCM_HB_GW_SUPPORT)
    case CMIC_STAT_DMA_PORTNUM_MAP_5_0r:
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_5_0r(unit, portnum_map[0]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_11_6r(unit, portnum_map[1]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_17_12r(unit, portnum_map[2]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_23_18r(unit, portnum_map[3]);
        break;
#endif
    case CMIC_STAT_DMA_PORTNUM_MAP_7_0r:
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_7_0r(unit, portnum_map[0]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_15_8r(unit, portnum_map[1]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_23_16r(unit, portnum_map[2]);
        WRITE_CMIC_STAT_DMA_PORTNUM_MAP_31_24r(unit, portnum_map[3]);
        break;
    }

    switch (blknum_reg) {
    case CMIC_STAT_DMA_BLKNUM_MAP_4_0r:
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_4_0r(unit, blknum_map[0]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_9_5r(unit, blknum_map[1]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_14_10r(unit, blknum_map[2]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_19_15r(unit, blknum_map[3]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_24_20r(unit, blknum_map[4]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_29_25r(unit, blknum_map[5]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_34_30r(unit, blknum_map[6]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_39_35r(unit, blknum_map[7]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_44_40r(unit, blknum_map[8]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_49_45r(unit, blknum_map[9]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_54_50r(unit, blknum_map[10]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_59_55r(unit, blknum_map[11]);
        if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_BLKNUM_MAP_63_60r)) {
            WRITE_CMIC_STAT_DMA_BLKNUM_MAP_63_60r(unit, blknum_map[12]);
            break;
        }
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_64_60r(unit, blknum_map[12]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_69_65r(unit, blknum_map[13]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_74_70r(unit, blknum_map[14]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_79_75r(unit, blknum_map[15]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_84_80r(unit, blknum_map[16]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_89_85r(unit, blknum_map[17]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_94_90r(unit, blknum_map[18]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_95r(unit, blknum_map[19]);
        break;
    case CMIC_STAT_DMA_BLKNUM_MAP_7_0r:
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_7_0r(unit, blknum_map[0]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_15_8r(unit, blknum_map[1]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_23_16r(unit, blknum_map[2]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_31_24r(unit, blknum_map[3]);
        if (!SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_BLKNUM_MAP_39_32r)) {
            break;
        }
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_39_32r(unit, blknum_map[4]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_47_40r(unit, blknum_map[5]);
        WRITE_CMIC_STAT_DMA_BLKNUM_MAP_55_48r(unit, blknum_map[6]);

        break;
    }

    /*
     * Reset value in CMIC_STAT_DMA_SETUP register is good. No
     * additional setup necessary for FB/ER
     */
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        soc_pci_write(unit,CMIC_CMCx_STAT_DMA_ADDR_OFFSET(cmc),soc_cm_l2p(unit, soc->counter_buf64));
        soc_pci_write(unit,CMIC_CMCx_STAT_DMA_PORTS_0_OFFSET(cmc),SOC_PBMP_WORD_GET(pbmp, 0));
        if (SOC_REG_IS_VALID(unit, CMIC_CMC0_STAT_DMA_PORTS_1r)) { 
            soc_pci_write(unit,CMIC_CMCx_STAT_DMA_PORTS_1_OFFSET(cmc),SOC_PBMP_WORD_GET(pbmp, 1));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_CMC0_STAT_DMA_PORTS_2r)) {  
            soc_pci_write(unit,CMIC_CMCx_STAT_DMA_PORTS_2_OFFSET(cmc),SOC_PBMP_WORD_GET(pbmp, 2));
        }
        SOC_PBMP_ASSIGN(pbmp, PBMP_HG_ALL(unit));
        SOC_PBMP_OR(pbmp, PBMP_XE_ALL(unit));
        SOC_PBMP_AND(pbmp, soc->counter_pbmp);

        WRITE_CMIC_STAT_DMA_PORT_TYPE_MAP_0r(unit, SOC_PBMP_WORD_GET(pbmp, 0));
        if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORT_TYPE_MAP_1r)) {
            WRITE_CMIC_STAT_DMA_PORT_TYPE_MAP_1r
                (unit, SOC_PBMP_WORD_GET(pbmp, 1));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORT_TYPE_MAP_2r)) {
            WRITE_CMIC_STAT_DMA_PORT_TYPE_MAP_2r
                (unit, SOC_PBMP_WORD_GET(pbmp, 2));
        }
    } else
#endif /* CMICM Support */
    {
        WRITE_CMIC_STAT_DMA_ADDRr(unit, soc_cm_l2p(unit, soc->counter_buf64));

        WRITE_CMIC_STAT_DMA_PORTSr(unit, SOC_PBMP_WORD_GET(pbmp, 0));
        if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORTS_HIr)) {
            WRITE_CMIC_STAT_DMA_PORTS_HIr(unit, SOC_PBMP_WORD_GET(pbmp, 1));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORTS_HI_2r)) {
            WRITE_CMIC_STAT_DMA_PORTS_HI_2r(unit, SOC_PBMP_WORD_GET(pbmp, 2));
        }

        if (SOC_IS_HB_GW(unit)) {
            SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else {
            SOC_PBMP_ASSIGN(pbmp, PBMP_HG_ALL(unit));
            SOC_PBMP_OR(pbmp, PBMP_XE_ALL(unit));
        }
        SOC_PBMP_AND(pbmp, soc->counter_pbmp);

        WRITE_CMIC_STAT_DMA_PORT_TYPE_MAPr(unit, SOC_PBMP_WORD_GET(pbmp, 0));
        if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORT_TYPE_MAP_HIr)) {
            WRITE_CMIC_STAT_DMA_PORT_TYPE_MAP_HIr
                (unit, SOC_PBMP_WORD_GET(pbmp, 1));
        }
        if (SOC_REG_IS_VALID(unit, CMIC_STAT_DMA_PORT_TYPE_MAP_HI_2r)) {
            WRITE_CMIC_STAT_DMA_PORT_TYPE_MAP_HI_2r
                (unit, SOC_PBMP_WORD_GET(pbmp, 2));
        }
    }
    val = 0;
    soc_reg_field_set(unit, CMIC_STAT_DMA_ING_STATS_CFGr,
                      &val, ING_ETH_BLK_NUMf, ing_blkoff);
    soc_reg_field_set(unit, CMIC_STAT_DMA_ING_STATS_CFGr,
                      &val, ING_STAT_COUNTERS_NUMf, ing_c_cnt);
    soc_reg_field_set(unit, CMIC_STAT_DMA_ING_STATS_CFGr,
                      &val, ING_STATS_PIPELINE_STAGE_NUMf, ing_pstage);
    WRITE_CMIC_STAT_DMA_ING_STATS_CFGr(unit, val);

    val = 0;
    soc_reg_field_set(unit, CMIC_STAT_DMA_EGR_STATS_CFGr,
                      &val, EGR_ETH_BLK_NUMf, egr_blkoff);
    soc_reg_field_set(unit, CMIC_STAT_DMA_EGR_STATS_CFGr,
                      &val, EGR_STAT_COUNTERS_NUMf, egr_c_cnt);
    soc_reg_field_set(unit, CMIC_STAT_DMA_EGR_STATS_CFGr,
                      &val, EGR_STATS_PIPELINE_STAGE_NUMf, egr_pstage);
    WRITE_CMIC_STAT_DMA_EGR_STATS_CFGr(unit, val);

    val = 0;
    soc_reg_field_set(unit, CMIC_STAT_DMA_MAC_STATS_CFGr,
                      &val, MAC_G_STAT_COUNTERS_NUMf, gmac_c_cnt);
    soc_reg_field_set(unit, CMIC_STAT_DMA_MAC_STATS_CFGr,
                      &val, MAC_X_STAT_COUNTERS_NUMf, xmac_c_cnt);
    soc_reg_field_set(unit, CMIC_STAT_DMA_MAC_STATS_CFGr,
                      &val, CPU_STATS_PORT_NUMf, CMIC_PORT(unit));
    WRITE_CMIC_STAT_DMA_MAC_STATS_CFGr(unit, val);

#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        val = soc_pci_read(unit,CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc));
        soc_reg_field_set(unit, CMIC_CMC0_STAT_DMA_CFGr, &val,
                          ST_DMA_ITER_DONE_CLRf, 1);
        if (soc_feature(unit, soc_feature_new_sbus_format)) {
            soc_reg_field_set(unit, CMIC_CMC0_STAT_DMA_CFGr, &val,
                              EN_TR3_SBUS_STYLEf, 1);
        }
        soc_pci_write(unit,CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc),val);
        soc_cmicm_intr0_enable(unit, IRQ_CMCx_STAT_ITER_DONE);
    } else
#endif
    {
        WRITE_CMIC_DMA_STATr(unit, DS_STAT_DMA_ITER_DONE_CLR);
        soc_intr_enable(unit, IRQ_STAT_ITER_DONE);
    }
    return SOC_E_NONE;
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_SBUSDMA_SUPPORT
void _soc_sbusdma_port_ctr_cb(int unit, int status, sbusdma_desc_handle_t handle,
                              void *data)
{
    int  phy_port;
    COUNTER_ATOMIC_DEF  s = SOC_CONTROL(unit);

    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = SOC_INFO(unit).port_p2l_mapping[PTR_TO_INT(data)];
    } else {
        phy_port = PTR_TO_INT(data);
    }

    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                (BSL_META_U(unit,
                            "In port counter cb [%d]\n"), handle));
    if (status == SOC_E_NONE) {
        LOG_DEBUG(BSL_LS_SOC_COUNTER,
                    (BSL_META_U(unit,
                                "Complete: port:%d.\n"),phy_port));
    } else {
        uint8 i;
        int blk;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Counter SBUSDMA failed: Handle:%d port:%d.\n"), handle,phy_port));
        if (status == SOC_E_TIMEOUT) {
            (void)soc_sbusdma_desc_delete(unit, handle);
            for (i=0; i<=(COUNTER_SBUSDMA_DESC_SIZE-1); i++) {
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
                if ((i == 3) && (!(SOC_IS_GREYHOUND2(unit) || SOC_IS_MONTEREY(unit)))) {
                    break;
                }
#endif
                if (_soc_port_counter_handles[unit][PTR_TO_INT(data)][i] == handle) {
                    _soc_port_counter_handles[unit][PTR_TO_INT(data)][i] = 0;
                    break;
                }
                blk = SOC_PORT_BLOCK(unit, PTR_TO_INT(data));
                if ((i == 2) &&
                    (!SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORT) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORTB0) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XTPORT) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XWPORT) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_MXQPORT) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GPORT) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GXPORT) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GXFPPORT) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CLPORT) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CLG2PORT) &&
                     !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CDPORT))) {
                    break;
                }
            }
        }
    }
    COUNTER_ATOMIC_BEGIN(s);
    _soc_counter_pending[unit]--;
    COUNTER_ATOMIC_END(s);
}

int
_soc_counter_sbusdma_setup(int unit)
{
    soc_control_t       *soc;
    int                 csize;
    soc_ctr_type_t      ctype;
    soc_reg_t           reg, ing_reg, egr_reg, mac_reg, gmac_reg;
    int                 ing_c_cnt, egr_c_cnt, mac_c_cnt, gmac_c_cnt;
    int                 ing_first, egr_first, mac_first, gmac_first;
    int                 ing_last, egr_last, mac_last, gmac_last;
    uint32              ing_addr, egr_addr, mac_addr;
    int                 ing_blkoff, egr_blkoff, mac_blkoff;
    uint8               ing_acc_type, egr_acc_type, mac_acc_type;
    int                 blk, port, phy_port, i;
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
    soc_reg_t           mac_xlpmib_reg;
    int                 mac_xlpmib_c_cnt, mac_xlpmib_first, mac_xlpmib_last;
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_HELIX5_SUPPORT)
    soc_reg_t           mac_clmib_reg;
    int                 mac_clmib_c_cnt, mac_clmib_first, mac_clmib_last;
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_KATANA2_SUPPORT
    soc_port_t          my_port;
#endif
    int                 port_num_blktype;
    soc_pbmp_t          all_pbmp;
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
    int                 use_gmac, gmac_c_width;
#endif
#ifdef SOC_COUNTER_TABLE_SUPPORT
    int mib_tbl_first = 10000; /* a huge number */
    int mib_tbl_last = -1;
    int mib_tbl_c_cnt = 0;
    uint32 mib_tbl_addr = 0;
    uint8 mib_tbl_acc_type = 0;
#endif

    soc = SOC_CONTROL(unit);
    port_num_blktype = SOC_DRIVER(unit)->port_num_blktype > 1 ?
        SOC_DRIVER(unit)->port_num_blktype : 1;

    sal_memset(_soc_port_counter_handles[unit], 0, sizeof(_soc_port_counter_handles[unit]));
    ing_reg = INVALIDr;
    egr_reg = INVALIDr;
    mac_reg = INVALIDr;
    gmac_reg = INVALIDr;
    ing_first = 10000; /* a huge number */
    egr_first = 10000;
    mac_first = 10000;
    gmac_first = 10000;
    ing_last = -1;
    egr_last = -1;
    mac_last = -1;
    gmac_last = -1;
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
    mac_xlpmib_reg = INVALIDr;
    mac_xlpmib_first = 10000;
    mac_xlpmib_last = -1;
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_HELIX5_SUPPORT)
    mac_clmib_reg = INVALIDr;
    mac_clmib_first = 10000;
    mac_clmib_last = -1;
#endif /* BCM_GREYHOUND2_SUPPORT */
    ing_reg = INVALIDr;
    egr_reg = INVALIDr;
    mac_reg = INVALIDr;
    gmac_reg = INVALIDr;
    for (ctype = 0; ctype < SOC_CTR_NUM_TYPES; ctype++) {
        if (!SOC_HAS_CTR_TYPE(unit, ctype)) {
            continue;
        }
        for (csize = SOC_CTR_MAP_SIZE(unit, ctype) - 1; csize >= 0; csize--) {
            reg = SOC_CTR_TO_REG(unit, ctype, csize);
            if (SOC_COUNTER_INVALID(unit, reg)) {
                continue;                       /* skip trailing invalids */
            }
#ifdef SOC_COUNTER_TABLE_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit) &&
                SOC_REG_IS_COUNTER_TABLE(unit, reg)) {
                if (mib_tbl_first > csize) {
                    mib_tbl_first = csize;
                }
                if (mib_tbl_last < csize) {
                    mib_tbl_last = csize;
                }
                continue;
            }
#endif
            if (SOC_REG_BLOCK_IS(unit, reg, SOC_BLK_IPIPE)) {
                if (ing_first > csize) {
                    ing_first = csize;
                    ing_reg = reg;
                }
                if (ing_last < csize) {
                    ing_last = csize;
                }
            }
            if (SOC_REG_BLOCK_IS(unit, reg, SOC_BLK_EPIPE)) {
                if (egr_first > csize) {
                    egr_first = csize;
                    egr_reg = reg;
                }
                if (egr_last < csize) {
                    egr_last = csize;
                }
            }

#ifdef BCM_SABER2_SUPPORT
            if (SOC_IS_SABER2(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MXQPORT)) {
                    if (mac_first > csize) {
                        mac_first = csize;
                        mac_reg = reg;
                    }
                    if (mac_last < csize) {
                        mac_last = csize;
                    }
                    mac_reg = reg;
                    continue;
                }
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT)) {
                    if (gmac_first > csize) {
                        gmac_first = csize;
                        gmac_reg = reg;
                    }
                    if (gmac_last < csize) {
                        gmac_last = csize;
                    }
                    continue;
                }
            }
#endif /* BCM_SABER2_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT)) {
                    if (mac_first > csize) {
                        mac_first = csize;
                    }
                    if (mac_last < csize) {
                        mac_last = csize;
                    }
                    mac_reg = reg;
                    continue;
                }
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXPORT) ||
                    SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXFPPORT)) {
                    if (gmac_first > csize) {
                        gmac_first = csize;
                    }
                    if (gmac_last < csize) {
                        gmac_last = csize;
                    }
                    gmac_reg = reg;
                    continue;
                }

                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT)) {
                    if (mac_clmib_first > csize) {
                        mac_clmib_first = csize;
                    }
                    if (mac_clmib_last < csize) {
                        mac_clmib_last = csize;
                    }
                    mac_clmib_reg = reg;
                    continue;
                }
            }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT)) {
                    if (mac_first > csize) {
                        mac_first = csize;
                    }
                    if (mac_last < csize) {
                        mac_last = csize;
                    }
                    mac_reg = reg;
                    continue;
                }
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXPORT)) {
                    if (gmac_first > csize) {
                        gmac_first = csize;
                    }
                    if (gmac_last < csize) {
                        gmac_last = csize;
                    }
                    gmac_reg = reg;
                    continue;
                }

                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT)) {
                    if (mac_clmib_first > csize) {
                        mac_clmib_first = csize;
                    }
                    if (mac_clmib_last < csize) {
                        mac_clmib_last = csize;
                    }
                    mac_clmib_reg = reg;
                    continue;
                }
            }
#endif /* BCM_HELIX5_SUPPORT */
#if defined(BCM_TOMAHAWKPLUS_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT)
            if (SOC_IS_TOMAHAWKPLUS(unit) || SOC_IS_TRIDENT3X(unit) ||
                SOC_IS_TOMAHAWK2(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT)) {
                    if (mac_first > csize) {
                        mac_first = csize;
                        mac_reg = reg;
                    }
                    if (mac_last < csize) {
                        mac_last = csize;
                    }
                    mac_reg = reg;
                    continue;
                }
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT)) {
                    if (gmac_first > csize) {
                        gmac_first = csize;
                        gmac_reg = reg;
                    }
                    if (gmac_last < csize) {
                        gmac_last = csize;
                    }
                    continue;
                }
            }
#endif /* BCM_TRIDENT3_SUPPORT || BCM_TOMAHAWKPLUS_SUPPORT || BCM_TOMAHAWK2_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit) || SOC_IS_MONTEREY(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT)) {
                    /* The first XLPMIB_XXXr offset = 0x10000 (XLPMIB_R64r) */
                    if (SOC_REG_INFO(unit, reg).offset & 0x10000) {
                        if (mac_xlpmib_first > csize) {
                            mac_xlpmib_first = csize;
                            mac_xlpmib_reg = reg;
                        }
                        if (mac_xlpmib_last < csize) {
                            mac_xlpmib_last = csize;
                        }
                        mac_xlpmib_reg = reg;
                    } else {
                        if (mac_first > csize) {
                            mac_first = csize;
                            mac_reg = reg;
                        }
                        if (mac_last < csize) {
                            mac_last = csize;
                        }
                        mac_reg = reg;
                    }
                    continue;
                }
            }

#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT)) {
#if defined(BCM_FIRELIGHT_SUPPORT)
                    if (soc_feature(unit, soc_feature_fl)) {
                        /* The first CLPMIB_XXXr offset = 0x10000 (CLPMIB_R64r) */
                        if (!(SOC_REG_INFO(unit, reg).offset & 0x10000)) {
                            if (mac_clmib_first > csize) {
                                mac_clmib_first = csize;
                                mac_clmib_reg = reg;
                            }
                            if (mac_clmib_last < csize) {
                                mac_clmib_last = csize;
                            }
                            mac_clmib_reg = reg;
                        }
                    } else
#endif /* BCM_FIRELIGHT_SUPPORT */
                    {
                        if (mac_clmib_first > csize) {
                            mac_clmib_first = csize;
                            mac_clmib_reg = reg;
                        }
                        if (mac_clmib_last < csize) {
                            mac_clmib_last = csize;
                        }
                        mac_clmib_reg = reg;
                    }
                    continue;
                }
            }
#endif /* BCM_GREYHOUND2_SUPPORT */

            if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORTB0) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XTPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XWPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MXQPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_PGW_CL)) {
                if (mac_first > csize) {
                    mac_first = csize;
                    mac_reg = reg;
                }
                if (mac_last < csize) {
                    mac_last = csize;
                }
                mac_reg = reg;
            } else if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GPORT)||
                       SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXFPPORT) ||
                       SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXPORT)) {
                if (gmac_first > csize) {
                    gmac_first = csize;
                    gmac_reg = reg;
                }
                if (gmac_last < csize) {
                    gmac_last = csize;
                }
            }
            /* CLG2 MIB have some new unique registers which are added
             * in seperate counter map. So we use gmac for CLG2.
             */
            if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLG2PORT)) {
                if (gmac_first > csize) {
                    gmac_first = csize;
                    gmac_reg = reg;
                }
                if (gmac_last < csize) {
                    gmac_last = csize;
                }
            }
        }
    }

    ing_c_cnt = ing_last != -1 ? ing_last - ing_first + 1 : 0;
    egr_c_cnt = egr_last != -1 ? egr_last - egr_first + 1 : 0;
    mac_c_cnt = mac_last != -1 ? mac_last - mac_first + 1 : 0;
    gmac_c_cnt = gmac_last != -1 ? gmac_last - gmac_first + 1 : 0;
    soc->counter_ing_first = ing_first;
    soc->counter_egr_first = egr_first;
    soc->counter_mac_first = mac_last != -1 ? mac_first : gmac_first;
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
    mac_xlpmib_c_cnt = mac_xlpmib_first != -1 ? mac_xlpmib_last - mac_xlpmib_first + 1 : 0;
#endif
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_HELIX5_SUPPORT)
    mac_clmib_c_cnt = mac_clmib_first != -1 ? mac_clmib_last - mac_clmib_first + 1 : 0;
#endif /* BCM_GREYHOUND2_SUPPORT */

    assert(SOC_REG_IS_VALID(unit, ing_reg));
    assert(SOC_REG_IS_VALID(unit, egr_reg));
    ing_addr = soc_reg_addr_get(unit, ing_reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &ing_blkoff,
                                &ing_acc_type);
    egr_addr = soc_reg_addr_get(unit, egr_reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &egr_blkoff,
                                &egr_acc_type);

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "ing_c_cnt = %d egr_c_cnt = %d mac_c_cnt = %d gmac_c_cnt = %d\n"
                         "ing_blkoff = %d ing_acc_type = %d ing_addr = %08x \n"
                         "egr_blkoff = %d egr_acc_type = %d egr_addr = %08x \n"),
              ing_c_cnt, egr_c_cnt, mac_c_cnt, gmac_c_cnt,
              ing_blkoff, ing_acc_type, ing_addr,
              egr_blkoff, egr_acc_type, egr_addr));
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "mac_xlpmib_c_cnt = %d "),
                  mac_xlpmib_c_cnt));
    }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "mac_xlpmib_c_cnt = %d mac_clmib_c_cnt = %d\n"),
                  mac_xlpmib_c_cnt, mac_clmib_c_cnt));
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#ifdef SOC_COUNTER_TABLE_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        mib_tbl_c_cnt = mib_tbl_first != -1 ? mib_tbl_last - mib_tbl_first + 1 : 0;
        mib_tbl_addr = soc_mem_addr_get(unit, CDMIB_MEMm, 0, 0, 0, &mib_tbl_acc_type);
        soc->counter_mib_tbl_first = mib_tbl_first;
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit, "mib_tbl_c_cnt = %d\n"), mib_tbl_c_cnt));
    }
#endif

        SOC_PBMP_CLEAR(all_pbmp);
        SOC_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            SOC_PBMP_CLEAR(all_pbmp);
            for (my_port =  SOC_INFO(unit).cmic_port;
                 my_port <= SOC_INFO(unit).lb_port;
                 my_port++) {
                 if (SOC_INFO(unit).block_valid[SOC_PORT_BLOCK(unit,my_port)]) {
                     SOC_PBMP_PORT_ADD(all_pbmp,my_port);
                 }
            }
        }
#endif

#if defined(BCM_KATANA2_SUPPORT)
        if (!(soc_feature(unit, soc_feature_discontinuous_pp_port))) {
            if (soc_feature(unit, soc_feature_linkphy_coe) &&
                SOC_INFO(unit).linkphy_enabled) {
                SOC_PBMP_REMOVE(all_pbmp, SOC_INFO(unit).linkphy_pp_port_pbm);
            }
            if (soc_feature(unit, soc_feature_subtag_coe) &&
                SOC_INFO(unit).subtag_enabled) {
                SOC_PBMP_REMOVE(all_pbmp, SOC_INFO(unit).subtag_pp_port_pbm);
            }
        }
#endif
        PBMP_ITER(all_pbmp, port) {

        soc_sbusdma_desc_ctrl_t ctrl = {0};
        soc_sbusdma_desc_cfg_t cfg;
        uint64 *buff;

        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        } else {
            phy_port = port;
        }
        buff = &soc->counter_buf64[port * soc->counter_perport];

        /* IPIPE */
        ctrl.flags = 0;
        ctrl.cfg_count = 1;
        ctrl.buff = &buff[ing_first];
        ctrl.cb = _soc_sbusdma_port_ctr_cb;
        ctrl.data = INT_TO_PTR(phy_port);
#define _IP_COUNTERS "IP COUNTERS"
        sal_strncpy_s(ctrl.name, _IP_COUNTERS, sizeof(ctrl.name));
        cfg.acc_type = ing_acc_type;
        cfg.blk = ing_blkoff;
        cfg.addr = ing_addr + port;
        cfg.width = SOC_REG_IS_64(unit, ing_reg) ? 2 : 1;
        cfg.count = ing_c_cnt;
        cfg.addr_shift = 8;
        SOC_IF_ERROR_RETURN
            (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                     &_soc_port_counter_handles[unit][phy_port][0]));
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "port %d phy_port %d handle ip: %d\n"),
                  port, phy_port, _soc_port_counter_handles[unit][phy_port][0]));

        /* EPIPE */
        ctrl.buff = &buff[egr_first];
#define _EP_COUNTERS "EP COUNTERS"
        sal_strncpy_s(ctrl.name, _EP_COUNTERS, sizeof(ctrl.name));
        cfg.acc_type = egr_acc_type;
        cfg.blk = egr_blkoff;
        cfg.addr = egr_addr + port;
        cfg.width = SOC_REG_IS_64(unit, egr_reg) ? 2 : 1;
        cfg.count = egr_c_cnt;
        cfg.addr_shift = 8;
        SOC_IF_ERROR_RETURN
            (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                     &_soc_port_counter_handles[unit][phy_port][1]));
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "port %d phy_port %d handle ep: %d\n"),
                  port, phy_port, _soc_port_counter_handles[unit][phy_port][1]));

#ifdef SOC_COUNTER_TABLE_SUPPORT
        /* MIB_TABLE */
        if (SOC_IS_TOMAHAWK3(unit) &&
            SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_CDPORT) &&
            PBMP_MEMBER(soc->counter_pbmp, port)) {
            ctrl.flags = SOC_SBUSDMA_MEMORY_CMD_MSG;
            ctrl.cfg_count = 1;
            ctrl.buff = &buff[mib_tbl_first];
            ctrl.cb = _soc_sbusdma_port_ctr_cb;
            ctrl.data = INT_TO_PTR(phy_port);
#define _MIB_TABLE_COUNTERS "MIB TBL CTRS"
            sal_strncpy_s(ctrl.name, _MIB_TABLE_COUNTERS, sizeof(ctrl.name));
            cfg.acc_type = mib_tbl_acc_type;
            cfg.blk = SOC_PORT_BLOCK(unit, phy_port);
            cfg.blk = SOC_BLOCK2SCH(unit, cfg.blk);
            cfg.addr = mib_tbl_addr + SOC_PORT_BINDEX(unit, phy_port) * NUM_CDMIB_MEM_ROWS_PER_PORT;
            cfg.width = 16; /* treat one row (8 counters) as one here */
            cfg.count = mib_tbl_c_cnt / 8; /* 8 counters per row in table */
            cfg.addr_shift = 0;
            SOC_IF_ERROR_RETURN
                (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                         &_soc_port_counter_handles[unit][phy_port][4]));
            LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                     (BSL_META_U(unit,
                                 "port %d phy_port %d handle mib_table: %d\n"),
                      port, phy_port, _soc_port_counter_handles[unit][phy_port][4]));
        }
#endif

        /* MAC */
        for (i = 0; i < port_num_blktype; i++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            if (blk < 0) {
                continue;
            }
            assert(SOC_REG_IS_VALID(unit, mac_reg) ||
                SOC_REG_IS_VALID(unit, gmac_reg));

            if (SOC_REG_IS_VALID(unit, mac_reg)) {
                if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, mac_reg).block,
                                      SOC_BLOCK_TYPE(unit, blk))) {
                    if (SOC_BLOCK_PORT(unit, blk) < 0) {
                        /* Greyhound phy_port 2-5 have both GXPORT and
                           XLPORT block attribute. Need to check block_port
                           to identify the valid attribute */
                        continue;
                    }
                    break;
                }
            }
            if (SOC_REG_IS_VALID(unit, gmac_reg)) {
                if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, gmac_reg).block,
                                      SOC_BLOCK_TYPE(unit, blk))) {
                    if (SOC_BLOCK_PORT(unit, blk) < 0) {
                        /* Greyhound phy_port 2-5 have both GXPORT and
                           XLPORT block attribute. Need to check block_port
                           to identify the valid attribute */
                        continue;
                    }
                    break;
                }
            }
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_HELIX5_SUPPORT)
            if (SOC_REG_IS_VALID(unit, mac_clmib_reg)) {
                if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, mac_clmib_reg).block,
                                      SOC_BLOCK_TYPE(unit, blk))) {
                    if (SOC_BLOCK_PORT(unit, blk) < 0) {
                        continue;
                    }
                    break;
                }
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
        }
        if (i == port_num_blktype) {
            continue;
        }

        mac_blkoff = 0;
        mac_addr = 0;
        mac_acc_type = 0;
#if defined (BCM_METROLITE_SUPPORT)
        if (SOC_IS_METROLITE(unit)) {
            if(SOC_PORT_VALID(unit, port)) {
                if (phy_port == 0) {
                    continue;
                }
                if (phy_port >= 1 && phy_port <= 4) {
                    mac_addr = soc_reg_addr_get(unit, mac_reg, port, 0, FALSE,
                                                &mac_blkoff, &mac_acc_type);
                    cfg.count = mac_c_cnt;
                } else {
                    mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0, FALSE,
                                                &mac_blkoff, &mac_acc_type);
                    cfg.count = gmac_c_cnt;
                }
                cfg.width = 2;
            }
        } else
#endif

#if defined (BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            if(SOC_PORT_VALID(unit, port)) {
                if (phy_port == 0) {
                    continue;
                }
                if (phy_port >= 1 && phy_port <= 24) {
                    mac_addr = soc_reg_addr_get(unit, mac_reg, port, 0,
                                                SOC_REG_ADDR_OPTION_NONE,
                                                &mac_blkoff, &mac_acc_type);
                    cfg.count = mac_c_cnt;
                } else {
                    mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0,
                                                SOC_REG_ADDR_OPTION_NONE,
                                                &mac_blkoff, &mac_acc_type);
                    cfg.count = gmac_c_cnt;
                }
                cfg.width = 2;
            }
        } else
#endif
#if defined(BCM_HELIX5_SUPPORT)
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
            use_gmac = (IS_EGPHY_PORT(unit, port) || IS_QSGMII_PORT(unit, port)) ? 1: 0;
            cfg.width = 2;
            if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_CLPORT)) {
                mac_addr = soc_reg_addr_get(unit, mac_clmib_reg, port, 0, FALSE,
                                            &mac_blkoff, &mac_acc_type);
                cfg.count = mac_clmib_c_cnt;
            } else if (use_gmac) {
                mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0, FALSE,
                                            &mac_blkoff, &mac_acc_type);
                cfg.count = gmac_c_cnt;
                cfg.width = 1;
            } else {
                mac_addr = soc_reg_addr_get(unit, mac_reg, port, 0, FALSE,
                                            &mac_blkoff, &mac_acc_type);
                cfg.count = mac_c_cnt;
            }
        } else
#endif /* HELIX5 */
#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3X(unit)) {
            use_gmac =
                SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port),
                                    SOC_BLK_XLPORT) ? TRUE : FALSE;
            gmac_c_width = 2;
            if (use_gmac) {
                mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0, FALSE,
                                            &mac_blkoff, &mac_acc_type);
                cfg.count = gmac_c_cnt;
                cfg.width = gmac_c_width;
            } else {
                mac_addr = soc_reg_addr_get(unit, mac_reg, port, 0, FALSE,
                                            &mac_blkoff, &mac_acc_type);
                cfg.count = mac_c_cnt;
                cfg.width = 2;
            }
        } else
#endif /* TRIDENT3 */
#if defined BCM_TOMAHAWKPLUS_SUPPORT
        if (SOC_IS_TOMAHAWKPLUS(unit) &&
            (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_XLPORT))) {
                mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0,
                                            SOC_REG_ADDR_OPTION_NONE,
                                            &mac_blkoff, &mac_acc_type);
                cfg.count = gmac_c_cnt;
                cfg.width = 2;
        } else
#endif /* BCM_TOMAHAWKPLUS_SUPPORT */
#if defined BCM_TOMAHAWK2_SUPPORT
        if (SOC_IS_TOMAHAWK2(unit)) {
            use_gmac = SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_XLPORT) ? TRUE : FALSE;
            gmac_c_width = 2;

            if (use_gmac) {
                mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0,
                                            SOC_REG_ADDR_OPTION_NONE,
                                            &mac_blkoff, &mac_acc_type);
                cfg.count = gmac_c_cnt;
                cfg.width = gmac_c_width;
            } else {
                mac_addr = soc_reg_addr_get(unit, mac_reg, port, 0,
                                            SOC_REG_ADDR_OPTION_NONE,
                                            &mac_blkoff, &mac_acc_type);
                cfg.count = mac_c_cnt;
                cfg.width = 2;
            }
        } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
        if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_CLG2PORT)) {
            mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0,
                                        SOC_REG_ADDR_OPTION_NONE,
                                        &mac_blkoff, &mac_acc_type);
            cfg.count = gmac_c_cnt;
            cfg.width = 2;
        } else if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_GPORT)||
                   SOC_BLOCK_IS_CMP(unit, SOC_PORT_IDX_BLOCK(unit, phy_port,i),
                                    SOC_BLK_GXPORT)) {

            mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0,
                                        SOC_REG_ADDR_OPTION_NONE, &mac_blkoff,
                                        &mac_acc_type);
            cfg.count = gmac_c_cnt;
            cfg.width = 1;
        } else {
#ifdef BCM_GREYHOUND2_SUPPORT
            if ((SOC_IS_GREYHOUND2(unit)) &&
                SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_CLPORT)) {
                mac_addr = soc_reg_addr_get(unit, mac_clmib_reg, port, 0,
                                            SOC_REG_ADDR_OPTION_NONE, &mac_blkoff,
                                            &mac_acc_type);
                cfg.count = mac_clmib_c_cnt;
                cfg.width = 2;
            } else
#endif /* BCM_GREYHOUND2_SUPPORT */
            {
                mac_addr = soc_reg_addr_get(unit, mac_reg, port, 0,
                                            SOC_REG_ADDR_OPTION_NONE, &mac_blkoff,
                                            &mac_acc_type);
                cfg.count = mac_c_cnt;
                cfg.width = 2;
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "port %d mac_blkoff = %d, mac_addr = %08x\n"),
                  port, mac_blkoff, mac_addr));

        /* MAC or port group */
        ctrl.buff = &buff[soc->counter_mac_first];
#define _MIBS "MIBS"
        sal_strncpy_s(ctrl.name, _MIBS, sizeof(ctrl.name));
        cfg.acc_type = mac_acc_type;
        cfg.blk = mac_blkoff;
        cfg.addr = mac_addr;
        cfg.addr_shift = 8;
        SOC_IF_ERROR_RETURN
            (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                     &_soc_port_counter_handles[unit][phy_port][2]));
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "port %d phy_port %d handle mib: %d\n"),
                  port, phy_port, _soc_port_counter_handles[unit][phy_port][2]));
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
        /* MAC for XLPMIB */
        if ((SOC_IS_GREYHOUND2(unit) && (!soc_feature(unit, soc_feature_fl)) &&
            (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_XLPORT))) ||
            (SOC_IS_MONTEREY(unit))){
            mac_addr = soc_reg_addr_get(unit, mac_xlpmib_reg, port, 0,
                                        SOC_REG_ADDR_OPTION_NONE, &mac_blkoff,
                                        &mac_acc_type);
            cfg.count = mac_xlpmib_c_cnt;
            cfg.width = 2;

            LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                     (BSL_META_U(unit,
                                 "port %d mac_xlpmib_c_cnt = %d\n"
                                 "mac_blkoff = %d, mac_addr = %08x\n"),
                      port, mac_xlpmib_c_cnt, mac_blkoff, mac_addr));

            ctrl.buff = &buff[mac_xlpmib_first];
#define _XLPMIBS "XLPMIBS"
            sal_strncpy_s(ctrl.name, _XLPMIBS, sizeof(ctrl.name));
            cfg.acc_type = mac_acc_type;
            cfg.blk = mac_blkoff;
            cfg.addr = mac_addr;
            cfg.addr_shift = 8;
            SOC_IF_ERROR_RETURN
                (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                         &_soc_port_counter_handles[unit][phy_port][3]));
            LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                     (BSL_META_U(unit,
                                 "port %d phy_port %d handle xlpmib: %d\n"),
                      port, phy_port, _soc_port_counter_handles[unit][phy_port][3]));
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
    }



    if (soc->blk_ctr_desc_count) {
        int rc;
        uint16 cnt, bindex;
        soc_sbusdma_desc_ctrl_t ctrl = {0};
        soc_sbusdma_desc_cfg_t *cfg;
        uint64 *buff, *hwval, *swval;
        sal_memset(_soc_blk_counter_handles[unit], 0,
                   soc->blk_ctr_desc_count * sizeof(sbusdma_desc_handle_t));
        _blk_ctr_process[unit] = sal_alloc(soc->blk_ctr_desc_count *
                                     sizeof(soc_blk_ctr_process_t*),
                                     "blk_ctr_process_ptr");
        buff = soc->blk_ctr_buf;
        hwval = soc->blk_counter_hw_val;
        swval = soc->blk_counter_sw_val;
        for (bindex = 0; bindex < soc->blk_ctr_desc_count; bindex++) {
            cnt = 0;
            while (soc->blk_ctr_desc[bindex].desc[cnt].reg != INVALIDr) {
                cnt++;
            }
            if (cnt) {
                uint8 at;
                uint16 i;
                int ctr_blk;
                soc_blk_ctr_process_t *ctr_process;
                cfg = sal_alloc(cnt * sizeof(soc_sbusdma_desc_cfg_t),
                      "sbusdma_desc_cfg");
                if (cfg == NULL) {
                    return SOC_E_MEMORY;
                }
                ctrl.flags = 0;
                ctrl.cfg_count = cnt;
                ctrl.buff = buff;
                ctrl.cb = _soc_sbusdma_blk_ctr_cb;
#define _BLK_CTRS "BLK CTRS"
                sal_strncpy_s(ctrl.name, _BLK_CTRS, sizeof(ctrl.name));
                ctr_process = sal_alloc(sizeof(soc_blk_ctr_process_t), "blk_ctr_process");
                if (cfg == NULL) {
                    sal_free(cfg);
                    return SOC_E_MEMORY;
                }
                ctr_process->blk = soc->blk_ctr_desc[bindex].blk;
                ctr_process->bindex = bindex;
                ctr_process->entries = cnt;
                ctr_process->buff = buff;
                ctr_process->hwval = hwval;
                ctr_process->swval = swval;
                ctrl.data = INT_TO_PTR((uint32)bindex);
                _blk_ctr_process[unit][bindex] = ctr_process;
                for (i=0; i<cnt; i++) {
                    cfg[i].addr = soc_reg_addr_get(unit,
                                                   soc->blk_ctr_desc[bindex].desc[i].reg,
                                                   REG_PORT_ANY, 0,
                                                   SOC_REG_ADDR_OPTION_NONE,
                                                   &ctr_blk, &at);
                    cfg[i].blk = ctr_blk;
                    cfg[i].width = soc->blk_ctr_desc[bindex].desc[i].width;
                    cfg[i].count = soc->blk_ctr_desc[bindex].desc[i].entries;
                    cfg[i].addr_shift = soc->blk_ctr_desc[bindex].desc[i].shift;
                }
                rc = soc_sbusdma_desc_create(unit, &ctrl, cfg, TRUE,
                                             &_soc_blk_counter_handles[unit][bindex]);
                sal_free(cfg);
                if (rc != SOC_E_NONE) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "Desc creation error for handle blk: %d\n"),
                               _soc_blk_counter_handles[unit][bindex]));
                    return rc;
                }
                LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                         (BSL_META_U(unit,
                                     "handle blk: %d\n"),
                          _soc_blk_counter_handles[unit][bindex]));
            }
            buff += cnt; hwval += cnt; swval += cnt;
        }
    }
    return SOC_E_NONE;
}

#if defined(BCM_XGS5_SWITCH_PORT_SUPPORT)
/*
 * Function:
 *      soc_counter_port_sbusdma_desc_alloc
 * Purpose:
 *      Allocate the Sbus DMA descriptors for the port.
 *      .
 * Parameters:
 *      unit    -  (IN) Unit number.
 *      port    -  (IN) Logical port.
 * Returns:
 *      SOC_E_NONE              Success
 *      SOC_E_*
 */
int
soc_counter_port_sbusdma_desc_alloc(int unit, soc_port_t port)
{
    soc_control_t       *soc;
    int                 csize;
    soc_ctr_type_t      ctype;
    soc_reg_t           reg, ing_reg, egr_reg, mac_reg, gmac_reg;
    int                 ing_c_cnt, egr_c_cnt, mac_c_cnt, gmac_c_cnt;
    int                 ing_first, egr_first, mac_first, gmac_first;
    int                 ing_last, egr_last, mac_last, gmac_last;
    uint32              ing_addr, egr_addr, mac_addr;
    int                 ing_blkoff, egr_blkoff, mac_blkoff;
    uint8               ing_acc_type, egr_acc_type, mac_acc_type;
    int                 blk, phy_port, i;
    int                 port_num_blktype;
    soc_sbusdma_desc_ctrl_t   ctrl = {0};
    soc_sbusdma_desc_cfg_t    cfg;
    uint64              *buff;
#if defined(BCM_TOMAHAWK2_SUPPORT)
    int                 use_gmac;
#endif
#if defined(BCM_TOMAHAWK2_SUPPORT)
    int                 gmac_c_width;
#endif
#ifdef SOC_COUNTER_TABLE_SUPPORT
    int mib_tbl_first = 10000; /* a huge number */
    int mib_tbl_last = -1;
    int mib_tbl_c_cnt = 0;
    uint32 mib_tbl_addr = 0;
    uint8 mib_tbl_acc_type = 0;
#endif
#if  defined(BCM_MONTEREY_SUPPORT)
    soc_reg_t           mac_xlpmib_reg;
    int                 mac_xlpmib_c_cnt, mac_xlpmib_first, mac_xlpmib_last;
#endif
#if defined(BCM_HELIX5_SUPPORT)
    soc_reg_t           mac_clmib_reg;
    int                 mac_clmib_c_cnt, mac_clmib_first, mac_clmib_last;
#endif
    soc = SOC_CONTROL(unit);

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    port_num_blktype = SOC_DRIVER(unit)->port_num_blktype > 1 ?
        SOC_DRIVER(unit)->port_num_blktype : 1;

    ing_reg = INVALIDr;
    egr_reg = INVALIDr;
    mac_reg = INVALIDr;
    gmac_reg = INVALIDr;
    ing_first = 10000; /* a huge number */
    egr_first = 10000;
    mac_first = 10000;
    gmac_first = 10000;
    ing_last = -1;
    egr_last = -1;
    mac_last = -1;
    gmac_last = -1;
    ing_reg = INVALIDr;
    egr_reg = INVALIDr;
    mac_reg = INVALIDr;
    gmac_reg = INVALIDr;
#if defined(BCM_MONTEREY_SUPPORT)
    mac_xlpmib_reg = INVALIDr;
    mac_xlpmib_first = 10000;
    mac_xlpmib_last = -1;
#endif
#if defined(BCM_HELIX5_SUPPORT)
    mac_clmib_reg = INVALIDr;
    mac_clmib_first = 10000;
    mac_clmib_last = -1;
#endif
    for (ctype = 0; ctype < SOC_CTR_NUM_TYPES; ctype++) {
        if (!SOC_HAS_CTR_TYPE(unit, ctype)) {
            continue;
        }
        for (csize = SOC_CTR_MAP_SIZE(unit, ctype) - 1; csize >= 0; csize--) {
            reg = SOC_CTR_TO_REG(unit, ctype, csize);
            if (SOC_COUNTER_INVALID(unit, reg)) {
                continue;                       /* skip trailing invalids */
            }
#ifdef SOC_COUNTER_TABLE_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit) &&
                SOC_REG_IS_COUNTER_TABLE(unit, reg)) {
                if (mib_tbl_first > csize) {
                    mib_tbl_first = csize;
                }
                if (mib_tbl_last < csize) {
                    mib_tbl_last = csize;
                }
                continue;
            }
#endif
            if (SOC_REG_BLOCK_IS(unit, reg, SOC_BLK_IPIPE)) {
                if (ing_first > csize) {
                    ing_first = csize;
                    ing_reg = reg;
                }
                if (ing_last < csize) {
                    ing_last = csize;
                }
            }
            if (SOC_REG_BLOCK_IS(unit, reg, SOC_BLK_EPIPE)) {
                if (egr_first > csize) {
                    egr_first = csize;
                    egr_reg = reg;
                }
                if (egr_last < csize) {
                    egr_last = csize;
                }
            }
#if defined(BCM_TOMAHAWK2_SUPPORT)
            if (SOC_IS_TOMAHAWK2(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT)) {
                    if (mac_first > csize) {
                        mac_first = csize;
                        mac_reg = reg;
                    }
                    if (mac_last < csize) {
                        mac_last = csize;
                    }
                    mac_reg = reg;
                    continue;
                }
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT)) {
                    if (gmac_first > csize) {
                        gmac_first = csize;
                        gmac_reg = reg;
                    }
                    if (gmac_last < csize) {
                        gmac_last = csize;
                    }
                    continue;
                }
            }
#endif /* BCM_TOMAHAWK2_SUPPORT */
#if  defined(BCM_MONTEREY_SUPPORT)
            if (SOC_IS_MONTEREY(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT)) {
                    /* The first XLPMIB_XXXr offset = 0x10000 (XLPMIB_R64r) */
                    if (SOC_REG_INFO(unit, reg).offset & 0x10000) {
                        if (mac_xlpmib_first > csize) {
                            mac_xlpmib_first = csize;
                            mac_xlpmib_reg = reg;
                        }
                        if (mac_xlpmib_last < csize) {
                            mac_xlpmib_last = csize;
                        }
                        mac_xlpmib_reg = reg;

                  } else  {
                        if (mac_first > csize) {
                            mac_first = csize;
                            mac_reg = reg;
                        }
                        if (mac_last < csize) {
                            mac_last = csize;
                        }
                        mac_reg = reg;
                    }
                        continue;
                }
            }
#endif
#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT)) {
                    if (mac_first > csize) {
                        mac_first = csize;
                    }
                    if (mac_last < csize) {
                        mac_last = csize;
                    }
                    mac_reg = reg;
                    continue;
                }

                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXPORT) ||
                    SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXFPPORT)) {
                    if (gmac_first > csize) {
                        gmac_first = csize;
                    }
                    if (gmac_last < csize) {
                        gmac_last = csize;
                    }
                    gmac_reg = reg;
                    continue;
                }

                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT)) {
                    if (mac_clmib_first > csize) {
                        mac_clmib_first = csize;
                    }
                    if (mac_clmib_last < csize) {
                        mac_clmib_last = csize;
                    }
                    mac_clmib_reg = reg;
                    continue;
                }
            }
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT)) {
                    if (mac_first > csize) {
                        mac_first = csize;
                    }
                    if (mac_last < csize) {
                        mac_last = csize;
                    }
                    mac_reg = reg;
                    continue;
                }

                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXPORT)) {
                    if (gmac_first > csize) {
                        gmac_first = csize;
                    }
                    if (gmac_last < csize) {
                        gmac_last = csize;
                    }
                    gmac_reg = reg;
                    continue;
                }

                if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT)) {
                    if (mac_clmib_first > csize) {
                        mac_clmib_first = csize;
                    }
                    if (mac_clmib_last < csize) {
                        mac_clmib_last = csize;
                    }
                    mac_clmib_reg = reg;
                    continue;
                }
            }
#endif /* BCM_HELIX5_SUPPORT */

            if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XLPORTB0) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XTPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_XWPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_MXQPORT) ||
                SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_PGW_CL)) {
                if (mac_first > csize) {
                    mac_first = csize;
                    mac_reg = reg;
                }
                if (mac_last < csize) {
                    mac_last = csize;
                }
                mac_reg = reg;
            } else if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GPORT)||
                       SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXFPPORT) ||
                       SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_GXPORT)) {
                if (gmac_first > csize) {
                    gmac_first = csize;
                    gmac_reg = reg;
                }
                if (gmac_last < csize) {
                    gmac_last = csize;
                }
            }
            /* CLG2 MIB have some new unique registers which are added
             * in seperate counter map. So we use gmac for CLG2.
             */
            if (SOC_REG_BLOCK_IN_LIST(unit, reg, SOC_BLK_CLG2PORT)) {
                if (gmac_first > csize) {
                    gmac_first = csize;
                    gmac_reg = reg;
                }
                if (gmac_last < csize) {
                    gmac_last = csize;
                }
            }
        }
    }

    ing_c_cnt = ing_first != -1 ? ing_last - ing_first + 1 : 0;
    egr_c_cnt = egr_first != -1 ? egr_last - egr_first + 1 : 0;
    mac_c_cnt = mac_first != -1 ? mac_last - mac_first + 1 : 0;
    gmac_c_cnt = gmac_first != -1 ? gmac_last - gmac_first + 1 : 0;
#if  defined(BCM_HELIX5_SUPPORT)
    mac_clmib_c_cnt = mac_clmib_first != -1 ? mac_clmib_last - mac_clmib_first + 1 : 0;
#endif /* BCM_MONTEREY_SUPPORT */
    soc->counter_ing_first = ing_first;
    soc->counter_egr_first = egr_first;
    soc->counter_mac_first = mac_first;
#if  defined(BCM_MONTEREY_SUPPORT)
    mac_xlpmib_c_cnt = mac_xlpmib_first != -1 ? mac_xlpmib_last - mac_xlpmib_first + 1 : 0;
#endif /* BCM_MONTEREY_SUPPORT */
    assert(SOC_REG_IS_VALID(unit, ing_reg));
    assert(SOC_REG_IS_VALID(unit, egr_reg));
    ing_addr = soc_reg_addr_get(unit, ing_reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &ing_blkoff,
                                &ing_acc_type);
    egr_addr = soc_reg_addr_get(unit, egr_reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_NONE, &egr_blkoff,
                                &egr_acc_type);

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "ing_c_cnt = %d egr_c_cnt = %d mac_c_cnt = %d gmac_c_cnt = %d\n"
                         "ing_blkoff = %d ing_acc_type = %d ing_addr = %08x \n"
                         "egr_blkoff = %d egr_acc_type = %d egr_addr = %08x \n"),
              ing_c_cnt, egr_c_cnt, mac_c_cnt, gmac_c_cnt,
              ing_blkoff, ing_acc_type, ing_addr,
              egr_blkoff, egr_acc_type, egr_addr));
#if defined(BCM_MONTEREY_SUPPORT)
    if ( SOC_IS_MONTEREY(unit)) {
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "mac_xlpmib_c_cnt = %d \n"),
                  mac_xlpmib_c_cnt ));
    }
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef SOC_COUNTER_TABLE_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        mib_tbl_c_cnt = mib_tbl_first != -1 ? mib_tbl_last - mib_tbl_first + 1 : 0;
        mib_tbl_addr = soc_mem_addr_get(unit, CDMIB_MEMm, 0, 0, 0, &mib_tbl_acc_type);
        soc->counter_mib_tbl_first = mib_tbl_first;
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit, "mib_tbl_c_cnt = %d\n"), mib_tbl_c_cnt));
    }
#endif
    buff = &soc->counter_buf64[port * soc->counter_perport];

    /* IPIPE */
    ctrl.flags = 0;
    ctrl.cfg_count = 1;
    ctrl.buff = &buff[ing_first];
    ctrl.cb = _soc_sbusdma_port_ctr_cb;
    ctrl.data = INT_TO_PTR(phy_port);
    ctrl.pc = 0;
    ctrl.pc_data = 0;
#define _IP_COUNTERS "IP COUNTERS"
    sal_strncpy_s(ctrl.name, _IP_COUNTERS, sizeof(ctrl.name));
    cfg.acc_type = ing_acc_type;
    cfg.blk = ing_blkoff;
    cfg.addr = ing_addr + port;
    cfg.width = SOC_REG_IS_64(unit, ing_reg) ? 2 : 1;
    cfg.count = ing_c_cnt;
    cfg.addr_shift = 8;
    SOC_IF_ERROR_RETURN
        (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                 &_soc_port_counter_handles[unit][phy_port][0]));
    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "port %d phy_port %d handle ip: %d\n"),
              port, phy_port, _soc_port_counter_handles[unit][phy_port][0]));

    /* EPIPE */
    ctrl.buff = &buff[egr_first];
#define _EP_COUNTERS "EP COUNTERS"
    sal_strncpy(ctrl.name, _EP_COUNTERS, sal_strlen(_EP_COUNTERS)+1);
    cfg.acc_type = egr_acc_type;
    cfg.blk = egr_blkoff;
    cfg.addr = egr_addr + port;
    cfg.width = SOC_REG_IS_64(unit, egr_reg) ? 2 : 1;
    cfg.count = egr_c_cnt;
    cfg.addr_shift = 8;
    SOC_IF_ERROR_RETURN
        (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                 &_soc_port_counter_handles[unit][phy_port][1]));
    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "port %d phy_port %d handle ep: %d\n"),
              port, phy_port, _soc_port_counter_handles[unit][phy_port][1]));

#ifdef SOC_COUNTER_TABLE_SUPPORT
    /* MIB_TABLE */
    if (SOC_IS_TOMAHAWK3(unit) &&
        SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_CDPORT)) {
        ctrl.flags = SOC_SBUSDMA_MEMORY_CMD_MSG;
        ctrl.cfg_count = 1;
        ctrl.buff = &buff[mib_tbl_first];
        ctrl.cb = _soc_sbusdma_port_ctr_cb;
        ctrl.data = INT_TO_PTR(phy_port);
#define _MIB_TABLE_COUNTERS "MIB TBL CTRS"
        sal_strncpy_s(ctrl.name, _MIB_TABLE_COUNTERS, sizeof(ctrl.name));
        cfg.acc_type = mib_tbl_acc_type;
        cfg.blk = SOC_PORT_BLOCK(unit, phy_port);
        cfg.blk = SOC_BLOCK2SCH(unit, cfg.blk);
        cfg.addr = mib_tbl_addr + SOC_PORT_BINDEX(unit, phy_port) * NUM_CDMIB_MEM_ROWS_PER_PORT;
        cfg.width = 16; /* treat one row (8 counters) as one here */
        cfg.count = mib_tbl_c_cnt / 8; /* 8 counters per row in table */
        cfg.addr_shift = 0;
        SOC_IF_ERROR_RETURN
            (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                     &_soc_port_counter_handles[unit][phy_port][4]));
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "port %d phy_port %d handle mib_table: %d\n"),
                  port, phy_port, _soc_port_counter_handles[unit][phy_port][4]));
    }
#endif

    /* MAC */
    for (i = 0; i < port_num_blktype; i++) {
        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
        if (blk < 0) {
            continue;
        }
        assert(SOC_REG_IS_VALID(unit, mac_reg) ||
            SOC_REG_IS_VALID(unit, gmac_reg));

        if (SOC_REG_IS_VALID(unit, mac_reg)) {
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, mac_reg).block,
                                  SOC_BLOCK_TYPE(unit, blk))) {
                if (SOC_BLOCK_PORT(unit, blk) < 0) {
                    /* Greyhound phy_port 2-5 have both GXPORT and
                       XLPORT block attribute. Need to check block_port
                       to identify the valid attribute */
                    continue;
                }
                break;
            }
        }
        if (SOC_REG_IS_VALID(unit, gmac_reg)) {
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, gmac_reg).block,
                                  SOC_BLOCK_TYPE(unit, blk))) {
                if (SOC_BLOCK_PORT(unit, blk) < 0) {
                    /* Greyhound phy_port 2-5 have both GXPORT and
                       XLPORT block attribute. Need to check block_port
                       to identify the valid attribute */
                    continue;
                }
                break;
            }
        }

#if defined(BCM_HELIX5_SUPPORT)
        if (SOC_REG_IS_VALID(unit, mac_clmib_reg)) {
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, mac_clmib_reg).block,
                                  SOC_BLOCK_TYPE(unit, blk))) {
                if (SOC_BLOCK_PORT(unit, blk) < 0) {
                    continue;
                }
                break;
            }
        }
#endif /* BCM_HELIX5_SUPPORT */

    }
    if (i == port_num_blktype) {
        return SOC_E_NONE;
    }

    mac_blkoff = 0;
    mac_addr = 0;
    mac_acc_type = 0;

#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK2(unit)) {
        use_gmac = SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_XLPORT) ? TRUE : FALSE;
        gmac_c_width = 2;

        if (use_gmac) {
            mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0,
                                        SOC_REG_ADDR_OPTION_NONE, &mac_blkoff,
                                        &mac_acc_type);
            cfg.count = gmac_c_cnt;
            cfg.width = gmac_c_width;
        } else {
            mac_addr = soc_reg_addr_get(unit, mac_reg, port, 0,
                                        SOC_REG_ADDR_OPTION_NONE, &mac_blkoff,
                                        &mac_acc_type);
            cfg.count = mac_c_cnt;
            cfg.width = 2;
        }
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
#if defined(BCM_HELIX5_SUPPORT)
    if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
        cfg.width = 2;
        if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_CLPORT)) {
            mac_addr = soc_reg_addr_get(unit, mac_clmib_reg, port, 0, FALSE,
                                        &mac_blkoff, &mac_acc_type);
            cfg.count = mac_clmib_c_cnt;
        } else if (IS_QSGMII_PORT(unit, port) || IS_EGPHY_PORT(unit, port)) {
            mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0, FALSE,
                                        &mac_blkoff, &mac_acc_type);
            cfg.count = gmac_c_cnt;
            cfg.width = 1;
        } else {
            mac_addr = soc_reg_addr_get(unit, mac_reg, port, 0, FALSE,
                                        &mac_blkoff, &mac_acc_type);
            cfg.count = mac_c_cnt;
        }
    } else
#endif /* BCM_HELIX5_SUPPORT */
    if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_CLG2PORT)) {
        mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0,
                                    SOC_REG_ADDR_OPTION_NONE,
                                    &mac_blkoff, &mac_acc_type);
        cfg.count = gmac_c_cnt;
        cfg.width = 2;
    } else if (SOC_BLOCK_IS_CMP(unit, SOC_PORT_BLOCK(unit, phy_port), SOC_BLK_GPORT)||
               SOC_BLOCK_IS_CMP(unit, SOC_PORT_IDX_BLOCK(unit, phy_port,i),
                                SOC_BLK_GXFPPORT) ||
               SOC_BLOCK_IS_CMP(unit, SOC_PORT_IDX_BLOCK(unit, phy_port,i),
                                SOC_BLK_GXPORT)) {
        mac_addr = soc_reg_addr_get(unit, gmac_reg, port, 0,
                                    SOC_REG_ADDR_OPTION_NONE, &mac_blkoff,
                                    &mac_acc_type);
        cfg.count = gmac_c_cnt;
        cfg.width = 1;
    } else {

        mac_addr = soc_reg_addr_get(unit, mac_reg, port, 0,
                                    SOC_REG_ADDR_OPTION_NONE, &mac_blkoff,
                                    &mac_acc_type);
        cfg.count = mac_c_cnt;
        cfg.width = 2;

    }

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "port %d mac_blkoff = %d, mac_addr = %08x\n"),
              port, mac_blkoff, mac_addr));
    ctrl.buff = &buff[mac_first];
#define _MIBS "MIBS"
    sal_strncpy_s(ctrl.name, _MIBS, sizeof(ctrl.name));
    cfg.acc_type = mac_acc_type;
    cfg.blk = mac_blkoff;
    cfg.addr = mac_addr;
    cfg.addr_shift = 8;
    SOC_IF_ERROR_RETURN
        (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                 &_soc_port_counter_handles[unit][phy_port][2]));
    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "port %d phy_port %d handle mib: %d\n"),
              port, phy_port, _soc_port_counter_handles[unit][phy_port][2]));

#if  defined(BCM_MONTEREY_SUPPORT)
        /* MAC for XLPMIB */
        if (SOC_IS_MONTEREY(unit))
        {
            mac_addr = soc_reg_addr_get(unit, mac_xlpmib_reg, port, 0,
                                        SOC_REG_ADDR_OPTION_NONE, &mac_blkoff,
                                        &mac_acc_type);
            cfg.count = mac_xlpmib_c_cnt;
            cfg.width = 2;

            LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                     (BSL_META_U(unit,
                                 "port %d mac_xlpmib_c_cnt = %d\n"
                                 "mac_blkoff = %d, mac_addr = %08x\n"),
                      port, mac_xlpmib_c_cnt, mac_blkoff, mac_addr));

            ctrl.buff = &buff[mac_xlpmib_first];
#define _XLPMIBS "XLPMIBS"
            sal_strncpy_s(ctrl.name, _XLPMIBS, sizeof(ctrl.name));
            cfg.acc_type = mac_acc_type;
            cfg.blk = mac_blkoff;
            cfg.addr = mac_addr;
            cfg.addr_shift = 8;
            SOC_IF_ERROR_RETURN
                (soc_sbusdma_desc_create(unit, &ctrl, &cfg, TRUE,
                                         &_soc_port_counter_handles[unit][phy_port][3]));
            LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                     (BSL_META_U(unit,
                                 "port %d phy_port %d handle xlpmib: %d\n"),
                      port, phy_port, _soc_port_counter_handles[unit][phy_port][3]));
        }
#endif /* BCM_MONTEREY_SUPPORT*/
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_counter_port_sbusdma_desc_free
 * Purpose:
 *      Deallocte or free the Sbus DMA descriptors
 *      associated with the port.
 *
 * Parameters:
 *      unit    -  (IN) Unit number.
 *      port    -  (IN) Logical port.
 * Returns:
 *      SOC_E_NONE              Success
 *      SOC_E_*
 */
int
soc_counter_port_sbusdma_desc_free(int unit, soc_port_t port)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint8 i, state = 0;
    int ret, err = 0;
    int blk, bindex, phy_port;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    if (phy_port == -1 ) {
       return SOC_E_INTERNAL;
    }

    blk = SOC_PORT_BLOCK(unit, phy_port);
    bindex = SOC_PORT_BINDEX(unit, phy_port);
    if (_soc_port_counter_handles[unit][phy_port][0] && soc->counter_interval) {
        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "port: %d blk: %d, bindex: %d\n"),
                  port, blk, bindex));
    }
    for (i = 0; i <= (COUNTER_SBUSDMA_DESC_SIZE-1); i++) {
        if ((i == 2) &&
            (!SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORT) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORTB0) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XTPORT) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XWPORT) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_MXQPORT) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GPORT) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GXPORT) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GXFPPORT) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CLPORT) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CLG2PORT) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CPRI) &&
             !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CDPORT))) {
            continue;
        }
#if  defined(BCM_MONTEREY_SUPPORT)
            if (i == 3) {
                if (!SOC_IS_MONTEREY(unit)) {
                    continue;
                }
            }
#endif
        if (_soc_port_counter_handles[unit][phy_port][i]) {
            do {
                (void)soc_sbusdma_desc_get_state(unit, _soc_port_counter_handles[unit][phy_port][i],
                                                 &state);
                if (state) {
                    sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 10);
                }
            } while (state);

            ret = soc_sbusdma_desc_delete(unit, _soc_port_counter_handles[unit][phy_port][i]);
            if (ret) {
                err++;
            }
            _soc_port_counter_handles[unit][phy_port][i] = 0;
        }
    }

    return err;
}
#endif /* BCM_XGS5_SWITCH_PORT_SUPPORT*/

int
_soc_counter_sbudma_desc_free_all(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint8 i, state = 0;
    int ret, err = 0;
    soc_port_t port;
    int blk, bindex, phy_port;
    soc_pbmp_t all_pbmp;
#ifdef BCM_KATANA2_SUPPORT
    soc_port_t my_port;
#endif

    SOC_PBMP_CLEAR(all_pbmp);
    SOC_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        SOC_PBMP_CLEAR(all_pbmp);
        for (my_port =  SOC_INFO(unit).cmic_port;
             my_port <= SOC_INFO(unit).lb_port;
             my_port++) {
             if (SOC_INFO(unit).block_valid[SOC_PORT_BLOCK(unit,my_port)]) {
                 SOC_PBMP_PORT_ADD(all_pbmp,my_port);
             }
        }
    }
#endif
#if defined(BCM_KATANA2_SUPPORT)
    if (!(soc_feature(unit, soc_feature_discontinuous_pp_port))) {
        if (soc_feature(unit, soc_feature_linkphy_coe) &&
            SOC_INFO(unit).linkphy_enabled) {
            SOC_PBMP_REMOVE(all_pbmp, SOC_INFO(unit).linkphy_pp_port_pbm);
        }
        if (soc_feature(unit, soc_feature_subtag_coe) &&
            SOC_INFO(unit).subtag_enabled) {
            SOC_PBMP_REMOVE(all_pbmp, SOC_INFO(unit).subtag_pp_port_pbm);
        }
    }
#endif
    PBMP_ITER(all_pbmp, port) {
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        } else {
            phy_port = port;
        }

        blk = SOC_PORT_BLOCK(unit, phy_port);
#if defined(BCM_MONTEREY_SUPPORT)
       if (SOC_IS_MONTEREY(unit) ) {
           if ((((phy_port > 0) && (phy_port < 13)) ||
                 ((phy_port > 52) && (phy_port < 65)))
               && !(IS_ROE_PORT(unit, port))) {
                blk = SOC_PORT_IDX_BLOCK(unit, phy_port ,1);
           } else {
                blk = SOC_PORT_IDX_BLOCK(unit, phy_port ,0);
           }
      }
#endif
        bindex = SOC_PORT_BINDEX(unit, phy_port);
        if (_soc_port_counter_handles[unit][phy_port][0] && soc->counter_interval) {
            LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                     (BSL_META_U(unit,
                                 "port: %d blk: %d, bindex: %d\n"),
                      port, blk, bindex));
        }
        for (i=0; i<=(COUNTER_SBUSDMA_DESC_SIZE-1); i++) {
            if ((i == 2) &&
                (!SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORT) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORTB0) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XTPORT) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XWPORT) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_MXQPORT) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GPORT) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GXPORT) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GXFPPORT) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CLPORT) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CPRI) &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CLG2PORT &&
                 !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CDPORT)))) {
                continue;
            }
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
            if (i == 3) {
                if (!(SOC_IS_GREYHOUND2(unit) || SOC_IS_MONTEREY(unit))) {
                    continue;
                }
                if (SOC_IS_GREYHOUND2(unit) &&
                    (!SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORT))) {
                    continue;
                }
            }
#endif
            if (_soc_port_counter_handles[unit][phy_port][i]) {
                do {
                    (void)soc_sbusdma_desc_get_state(unit, _soc_port_counter_handles[unit][phy_port][i],
                                                     &state);
                    if (state) {
                        sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 10);
                    }
                } while (state);

                ret = soc_sbusdma_desc_delete(unit, _soc_port_counter_handles[unit][phy_port][i]);
                if (ret) {
                    err++;
                }
                _soc_port_counter_handles[unit][phy_port][i] = 0;
            }
        }
    }
    if (soc->blk_ctr_desc_count && _soc_blk_counter_handles[unit]) {
        uint16 bindex;
        for (bindex = 0; bindex < soc->blk_ctr_desc_count; bindex++) {
            if (_soc_blk_counter_handles[unit][bindex]) {
                do {
                    (void)soc_sbusdma_desc_get_state(unit, _soc_blk_counter_handles[unit][bindex],
                                                     &state);
                    if (state) {
                        sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 10);
                    }
                } while (state);
                ret = soc_sbusdma_desc_delete(unit, _soc_blk_counter_handles[unit][bindex]);
                if (ret) {
                    err++;
                }
                _soc_blk_counter_handles[unit][bindex] = 0;
                if (_blk_ctr_process[unit][bindex]) {
                    sal_free(_blk_ctr_process[unit][bindex]);
                    _blk_ctr_process[unit][bindex] = NULL;
                }
            }
        }
        if (_blk_ctr_process[unit]) {
            sal_free(_blk_ctr_process[unit]);
            _blk_ctr_process[unit] = NULL;
        }
    }
    return err;
}

int
soc_blk_counter_get(int unit, soc_block_t blk, soc_reg_t ctr_reg, int ar_idx,
                    uint64 *val)
{
    uint16 bindex, rindex;
    soc_blk_ctr_process_t *ctr_process;
    soc_ctr_reg_desc_t *reg_desc;
    soc_control_t *soc = SOC_CONTROL(unit);

    if (soc->blk_ctr_desc_count && _blk_ctr_process[unit]) {
        for (bindex = 0; bindex < soc->blk_ctr_desc_count; bindex++) {
            if (_blk_ctr_process[unit][bindex]) {
                ctr_process = _blk_ctr_process[unit][bindex];
                if (ctr_process->blk == blk) {
                    reg_desc = soc->blk_ctr_desc[ctr_process->bindex].desc;
                    for (rindex = 0; reg_desc[rindex].reg != INVALIDr;
                         rindex++) {
                        if (reg_desc[rindex].reg == ctr_reg) {
                            if (ar_idx < reg_desc[rindex].entries) {
                                *val = ctr_process->swval[ar_idx];
                                return SOC_E_NONE;
                            } else {
                                return SOC_E_PARAM;
                            }
                        }
                    }
                    return SOC_E_PARAM;
                }
            }
        }
        return SOC_E_PARAM;
    }
    return SOC_E_INIT;
}

#endif /* BCM_SBUSDMA_SUPPORT */

#ifdef BCM_XGS_SUPPORT
/*
 * Function:
 *      _soc_counter_dma_setup
 * Purpose:
 *      Configure hardware registers for counter collection.  Used during
 *      soc_counter_thread initialization.
 * Parameters:
 *      unit    - switch unit
 * Returns:
 *      SOC_E_*
 */

static int
_soc_counter_dma_setup(int unit)
{
    soc_control_t       *soc;
    uint32              creg_first, creg_last, cireg_first, cireg_last;
    int                 csize;
    pbmp_t              pbmp;
    soc_reg_t           reg;
    soc_ctr_type_t      ctype;
    uint32              offset;
#ifdef BCM_HERCULES_SUPPORT
    uint32              val;
#endif
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
#ifdef BCM_SBUSDMA_SUPPORT
        if (soc_feature(unit, soc_feature_sbusdma)) {
            int rv = _soc_counter_sbusdma_setup(unit);
            if (rv) {
                (void)_soc_counter_sbudma_desc_free_all(unit);
            }
            return rv;
        } else
#endif
        {
            return(_soc_xgs3_counter_dma_setup(unit));
        }
    }
#endif

    soc = SOC_CONTROL(unit);
    SOC_PBMP_ASSIGN(pbmp, soc->counter_pbmp);

    WRITE_CMIC_STAT_DMA_ADDRr(unit, soc_cm_l2p(unit, soc->counter_buf32));
    WRITE_CMIC_STAT_DMA_PORTSr(unit, SOC_PBMP_WORD_GET(pbmp, 0));

    /*
     * Calculate the first and last register offsets for collection
     * Higig offsets are kept separately
     */
    creg_first = creg_last = cireg_first = cireg_last = 0;
    for (ctype = 0; ctype < SOC_CTR_NUM_TYPES; ctype++) {
        if (!SOC_HAS_CTR_TYPE(unit, ctype)) {
            continue;
        }
        if (ctype == SOC_CTR_TYPE_GFE) {        /* offsets screwed up */
            continue;
        }
        reg = SOC_CTR_TO_REG(unit, ctype, 0);
        if (!SOC_COUNTER_INVALID(unit, reg)) {
            if (ctype == SOC_CTR_TYPE_HG) {
                cireg_first = SOC_REG_INFO(unit, reg).offset;
            } else {
                creg_first = SOC_REG_INFO(unit, reg).offset;
                if (cireg_first == 0) {
                    cireg_first = creg_first;
                }
            }
        }
        csize = SOC_CTR_MAP_SIZE(unit, ctype) - 1;
        for (; csize > 0; csize--) {
            reg = SOC_CTR_TO_REG(unit, ctype, csize);
            if (SOC_COUNTER_INVALID(unit, reg)) {
                continue;                       /* skip trailing invalids */
            }
            offset = SOC_REG_INFO(unit, reg).offset;
            csize = SOC_REG_IS_64(unit, reg) ?
                sizeof(uint64) : sizeof(uint32);
            csize = (soc->counter_portsize / csize) - 1;
            if (ctype == SOC_CTR_TYPE_HG) {
                if (offset > cireg_last) {
                    cireg_last = offset;
                    if (cireg_last > cireg_first + csize) {
                        cireg_last = cireg_first + csize;
                    }
                }
            } else {
                if (offset > creg_last) {
                    creg_last = offset;
                    if (creg_last > creg_first + csize) {
                        creg_last = creg_first + csize;
                    }
                }
            }
            break;
        }
    }

    /*
     * Set the various configuration registers
     */
#ifdef BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        val = 0;
        creg_first = cireg_first;
        creg_last = cireg_last;
        soc_reg_field_set(unit, CMIC_64BIT_STATS_CFGr, &val,
                          L_STAT_REGf, creg_last);
        soc_reg_field_set(unit, CMIC_64BIT_STATS_CFGr, &val,
                          F_STAT_REGf, creg_first);
        WRITE_CMIC_64BIT_STATS_CFGr(unit, val);
    } else {
        val = 0;
        soc_reg_field_set(unit, CMIC_STAT_DMA_SETUPr, &val,
                          L_STAT_REGf, creg_last);
        soc_reg_field_set(unit, CMIC_STAT_DMA_SETUPr, &val,
                          F_STAT_REGf, creg_first);
        WRITE_CMIC_STAT_DMA_SETUPr(unit, val);
    }
#endif
    WRITE_CMIC_DMA_STATr(unit, DS_STAT_DMA_ITER_DONE_CLR);


#ifdef BCM_CMICM_SUPPORT
        if(soc_feature(unit, soc_feature_cmicm)) {
            soc_cmicm_intr0_enable(unit, IRQ_CMCx_STAT_ITER_DONE);
        } else
#endif
        {
            soc_intr_enable(unit, IRQ_STAT_ITER_DONE);
        }

    return SOC_E_NONE;
}
#endif /*BCM_XGS_SUPPORT*/
/*
 * Function:
 *      soc_counter_thread
 * Purpose:
 *      Master counter collection and accumulation thread.
 * Parameters:
 *      unit_vp - StrataSwitch unit # (as a void *).
 * Returns:
 *      Nothing, does not return.
 */

void
soc_counter_thread(void *unit_vp)
{
    int                 unit = PTR_TO_INT(unit_vp);
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 rv = SOC_E_NONE;
    int                 interval;
#if defined(WRITE_CMIC_DMA_STATr) || (defined(BCM_CMICM_SUPPORT) && defined(BCM_ESW_SUPPORT))
    uint32              val;
#endif
    int                 dma;
    sal_usecs_t         cdma_timeout, now;
    COUNTER_ATOMIC_DEF  s = SOC_CONTROL(unit);
    int                 sync_gnt = FALSE;
    int                 i;
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);
#if defined(BCM_ESW_SUPPORT)
#ifdef BCM_CMICM_SUPPORT
    int cmc = SOC_PCI_CMC(unit);
#endif
#endif
#if defined(BCM_XGS3_SWITCH_SUPPORT) || defined(BCM_ESW_SUPPORT)
    static int          _stat_err[SOC_MAX_NUM_DEVICES] = {0};
#endif
#ifdef BCM_SBUSDMA_SUPPORT
    soc_timeout_t       to;
    int                 timer_started = 0;
#endif
#ifdef BCM_TRIDENT2_SUPPORT
    int                 age_sync = FALSE;
#endif

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_counter_thread: unit=%d\n"), unit));

    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, TRUE));

    /*
     * Create a semaphore used to time the trigger scans, and if DMA is
     * used, monitor for the Stats DMA Iteration Done interrupt.
     */
    cdma_timeout = soc_property_get(unit, spn_CDMA_TIMEOUT_USEC, 1000000);

    dma = ((soc->counter_flags & SOC_COUNTER_F_DMA) != 0);

#ifdef BCM_DNX_SUPPORT
    while(SOC_WARM_BOOT(unit) && SOC_IS_DNX(unit))
    {
        /** Busy wait */
        sal_usleep(10000);
    }

    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_COUNTER);
    }
#endif /*BCM_DNX_SUPPORT*/

    if (dma) {
#ifdef BCM_XGS_SUPPORT
        rv = _soc_counter_dma_setup(unit);
#endif /* BCM_XGS_SUPPORT */
        if (rv < 0) {
            goto done;
        }
    }

    /*
     * The hardware timer can only be used for intervals up to about
     * 1.048 seconds.  This implementation uses a software timer (via
     * semaphore timeout) instead of the hardware timer.
     */

    while ((interval = soc->counter_interval) != 0) {
#ifdef COUNTER_BENCH
        sal_usecs_t     start_time;
#endif
        int             err = 0;

        /*
         * Use a semaphore timeout instead of a sleep to achieve the
         * desired delay between scans.  This allows this thread to exit
         * immediately when soc_counter_stop wants it to.
         */

        LOG_DEBUG(BSL_LS_SOC_COUNTER,
                    (BSL_META_U(unit,
                                "soc_counter_thread: sleep %d\n"), interval));

        (void)sal_sem_take(soc->counter_trigger, interval);

        if (soc->counter_interval == 0) {       /* Exit signaled */
            break;
        }

#ifdef BCM_TRIDENT2_SUPPORT
        if (soc->l2x_sw_aging && soc->l2x_age_sync != NULL) {
            sal_sem_take(soc->l2x_age_sync, sal_sem_FOREVER);
            age_sync = TRUE;
        }
#endif

        if (soc->counter_sync_req) {
            sync_gnt = TRUE;
        }

#ifdef COUNTER_BENCH
        start_time = sal_time_usecs();
#endif


        /*
         * If in DMA mode, use DMA to transfer the counters into
         * memory.  Start a DMA pass by enabling DMA and clearing
         * STAT_DMA_DONE bit.  Wait for the pass to finish.
         */
        COUNTER_LOCK(unit);

#ifdef BCM_SBUSDMA_SUPPORT
        if (dma && soc_feature(unit, soc_feature_sbusdma)) {
            uint8 i;
            int ret;
            soc_port_t port;
            int blk, bindex, phy_port;

            PBMP_ITER(soc->counter_pbmp, port) {
                if (soc_feature(unit, soc_feature_logical_port_num)) {
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                } else {
                    phy_port = port;
                }
#ifdef BCM_KATANA2_SUPPORT
                if (SOC_IS_KATANA2(unit)) {
                    if (!SOC_PORT_VALID(unit, phy_port)) {
                        continue;
                    }
                }
#endif
                blk = SOC_PORT_BLOCK(unit, phy_port);
                bindex = SOC_PORT_BINDEX(unit, phy_port);
                if (SOC_IS_MONTEREY(unit)) {
                    if ( ((phy_port > 0) && (phy_port < 13)) ||
                       ((phy_port > 52) && (phy_port < 65))) {
                        blk = SOC_PORT_IDX_BLOCK(unit, phy_port,1);
                        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port,1);
                    }
                }
                if (_soc_port_counter_handles[unit][phy_port][0] && soc->counter_interval) {
                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit,
                                         "port: %d blk: %d, bindex: %d\n"),
                              port, blk, bindex));
                }
                for (i=0; i<=(COUNTER_SBUSDMA_DESC_SIZE-1); i++) {
                    if ((i == 2) &&
                        (!SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORT) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORTB0) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XTPORT) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XWPORT) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_MXQPORT) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GPORT) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GXPORT) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_GXFPPORT) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CLPORT) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CLG2PORT) &&
                         !SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_CDPORT))) {
                        break;
                    }
#if defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_MONTEREY_SUPPORT)
                    if (i == 3) {
                        if (!(SOC_IS_GREYHOUND2(unit) || SOC_IS_MONTEREY(unit))) {
                            continue;
                        }
                        if (SOC_IS_GREYHOUND2(unit) &&
                            (!SOC_BLOCK_IS_CMP(unit, blk, SOC_BLK_XLPORT))) {
                            continue;
                        }
                    }
#endif
                    if (_soc_port_counter_handles[unit][phy_port][i] && soc->counter_interval) {
                        do {
                            ret = soc_sbusdma_desc_run(unit, _soc_port_counter_handles[unit][phy_port][i]);
                            if ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) {
                                if (ret == SOC_E_INIT) {
                                    /* sbus dma thread not running!! */
                                    break;
                                }

                                /*
                                 * timeout if sbus dma has been busy
                                 * for too long.
                                 */
                                if (!timer_started) {
                                 soc_timeout_init(&to, 2 * cdma_timeout, 0);
                                    timer_started = 1;
                                }

                                if (soc_timeout_check(&to)) {
                                    LOG_WARN(BSL_LS_SOC_COMMON,
                                             (BSL_META_U(unit,
                                                         "Counters read operation timedout\n")));
                                    break;
                                }
                                COUNTER_UNLOCK(unit);
                                sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 100);
                                COUNTER_LOCK(unit);
                            } else {
                                COUNTER_ATOMIC_BEGIN(s);
                                _soc_counter_pending[unit]++;
                                COUNTER_ATOMIC_END(s);
                            }
                        } while(((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) &&
                                _soc_port_counter_handles[unit][phy_port][i] &&
                                soc->counter_interval);

                        if (timer_started) {
                            timer_started = 0;
                        }
                    } else {
                        if (soc->counter_interval == 0) {   /* Exit signaled */
                            break;
                        }
                    }
                }
            }

            if (soc->blk_ctr_desc_count && soc->counter_interval) {
                for (i=0; i<soc->blk_ctr_desc_count; i++) {
                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit,
                                         "blk ctr %d\n"), i));
                    if (_soc_blk_counter_handles[unit][i] && soc->counter_interval) {
                        do {
                            ret = soc_sbusdma_desc_run(unit, _soc_blk_counter_handles[unit][i]);
                            if ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) {
                                COUNTER_UNLOCK(unit);
                                sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 10);
                                COUNTER_LOCK(unit);
                            }
                        } while(((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) && soc->counter_interval);
                    } else {
                        if (soc->counter_interval == 0) {   /* Exit signaled */
                            break;
                        }
                    }
                }
            }
            if (soc->counter_interval == 0) {   /* Exit signaled */
                COUNTER_UNLOCK(unit);
                break;
            }
        }
#endif
        if (dma && !soc_feature(unit, soc_feature_sbusdma)) {
            LOG_DEBUG(BSL_LS_SOC_COUNTER,
                        (BSL_META_U(unit,
                                    "soc_counter_thread: trigger DMA\n")));

#ifdef BCM_CMICM_SUPPORT
#if defined(BCM_ESW_SUPPORT)

            if(soc_feature(unit, soc_feature_cmicm)) {
                /* Clear Status */
                val = soc_pci_read(unit, CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc));
                soc_reg_field_set(unit, CMIC_CMC0_STAT_DMA_CFGr, &val,
                                  ENf, 0);
                soc_reg_field_set(unit, CMIC_CMC0_STAT_DMA_CFGr, &val,
                                  ST_DMA_ITER_DONE_CLRf, 0);
                soc_pci_write(unit, CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc), val);
                /* start DMA */
                soc_reg_field_set(unit, CMIC_CMC0_STAT_DMA_CFGr, &val,
                                  ENf, 1);
                soc_pci_write(unit, CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc), val);
            } else
#endif
#endif
            {
                /* Clear Status and start DMA */
#ifdef WRITE_CMIC_DMA_STATr
                WRITE_CMIC_DMA_STATr(unit, DS_STAT_DMA_DONE_CLR);
                READ_CMIC_STAT_DMA_SETUPr(unit, &val);
                soc_reg_field_set(unit, CMIC_STAT_DMA_SETUPr, &val,
                                  ENf, 1);
                WRITE_CMIC_STAT_DMA_SETUPr(unit, val);
#endif
            }
            /* Wait for ISR to wake semaphore */
#ifdef BCM_CMICM_SUPPORT
            if(soc_feature(unit, soc_feature_cmicm)) {
                if (sal_sem_take(soc->counter_intr, cdma_timeout) >= 0) {
                    soc_cm_sinval(unit,
                                  (void *)soc->counter_buf32,
                                  soc->counter_bufsize);

                    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                                (BSL_META_U(unit,
                                            "soc_counter_thread: "
                                            "DMA iter done\n")));

#ifdef COUNTER_BENCH
                    LOG_DEBUG(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "Time taken for dma: %d usec\n"),
                                 SAL_USECS_SUB(sal_time_usecs(), start_time)));
#endif
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "soc_counter_thread: "
                                          "DMA did not finish buf32=%p\n"),
                               (void *)soc->counter_buf32));
                    err = 1;
                }
#if defined(BCM_ESW_SUPPORT)

                if (soc_pci_read(unit, CMIC_CMCx_STAT_DMA_STAT_OFFSET(cmc)) & ST_CMCx_DMA_ERR) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "soc_counter_thread: unit = %d DMA Error\n"),
                               unit));
                    if (soc_ser_stat_error(unit, -1) != SOC_E_NONE) {
                        _stat_err[unit]++;
                        /* Error is reported atleast one more time after correction.
                           So try few times and abort if correction really fails */
                        if (_stat_err[unit] > 10) {
                            err = 1;
                        }
                    } else {
                        _stat_err[unit] = 0;
                    }
                }
#endif
            } else
#endif /* CMICm Support */
            {
                if (sal_sem_take(soc->counter_intr, cdma_timeout) >= 0) {
                    soc_cm_sinval(unit,
                                  (void *)soc->counter_buf32,
                                  soc->counter_bufsize);

                    LOG_DEBUG(BSL_LS_SOC_COUNTER,
                                (BSL_META_U(unit,
                                            "soc_counter_thread: "
                                            "DMA iter done\n")));

#ifdef COUNTER_BENCH
                    LOG_DEBUG(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "Time taken for dma: %d usec\n"),
                                 SAL_USECS_SUB(sal_time_usecs(), start_time)));
#endif
                } else {
#ifdef BCM_XGS3_SWITCH_SUPPORT
                    if (SOC_IS_XGS3_SWITCH(unit) &&
                        soc_pci_read(unit, CMIC_DMA_STAT) & DS_STAT_DMA_ITER_DONE_TST) {
                        LOG_WARN(BSL_LS_SOC_COMMON,
                                 (BSL_META_U(unit,
                                             "Intr disabled, mask_reg=0x%x, reenable it\n"),
                                  soc_pci_read(unit, CMIC_IRQ_MASK)));
                        WRITE_CMIC_DMA_STATr(unit, DS_STAT_DMA_ITER_DONE_CLR);
                        soc_intr_enable(unit, IRQ_STAT_ITER_DONE);
                    } else
#endif
                    {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "soc_counter_thread: "
                                              "DMA did not finish buf32=%p\n"),
                                   (void *)soc->counter_buf32));
                        err = 1;
                    }
                }
#ifdef BCM_XGS3_SWITCH_SUPPORT
                if (SOC_IS_XGS3_SWITCH(unit)) {
                    if (soc_pci_read(unit, CMIC_DMA_STAT) & DS_STAT_DMA_ERROR) {
                        WRITE_CMIC_DMA_STATr(unit, DS_STAT_DMA_DONE_CLR);
                        if (soc_feature(unit, soc_feature_stat_dma_error_ack)) {
                            WRITE_CMIC_DMA_STATr(unit, DS_STAT_DMA_ERROR_CLR - 1);
                        } else {
                            WRITE_CMIC_DMA_STATr(unit, DS_STAT_DMA_ERROR_CLR);
                        }
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "soc_counter_thread: unit = %d DMA Error\n"),
                                   unit));
                        if (soc_ser_stat_error(unit, -1) != SOC_E_NONE) {
                            _stat_err[unit]++;
                            /* Error is reported atleast one more time after correction.
                               So try few times and abort if correction really fails */
                            if (_stat_err[unit] > 10) {
                                err = 1;
                                _stat_err[unit] = 0;
                            }
                        } else {
                            _stat_err[unit] = 0;
                        }
                    }
                }
#endif
            }
            if (soc->counter_interval == 0) {   /* Exit signaled */
                COUNTER_UNLOCK(unit);
                break;
            }
        }

        /*
         * Add up changes to counter values.
         */

#ifdef BCM_SBUSDMA_SUPPORT
        while (_soc_counter_pending[unit] && soc->counter_interval) {
            COUNTER_UNLOCK(unit);
            sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 100);
            COUNTER_LOCK(unit);
        }
#endif

        now = sal_time_usecs();
        COUNTER_ATOMIC_BEGIN(s);
        soc->counter_coll_prev = soc->counter_coll_cur;
        soc->counter_coll_cur = now;
        COUNTER_ATOMIC_END(s);

        if ( (!err) && (soc->counter_n32 > 0) && (soc->counter_interval) ) {
            rv = soc_counter_collect32(unit, FALSE);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_counter_thread: collect32 failed: %s\n"),
                           soc_errmsg(rv)));
                err = 1;
            }
        }

        if ((!err) && (soc->counter_n64 > 0) && (soc->counter_interval) ) {
            if(SOC_IS_SAND(unit)) {
                if (SOC_IS_ARAD(unit)) {
#ifdef BCM_PETRA_SUPPORT
                    rv = soc_counter_collect64(unit, FALSE, -1, INVALIDr);
#endif
                } else {
                    rv = SOC_E_NONE;
                }
            } else {
#ifdef BCM_XGS_SUPPORT
                rv = soc_counter_collect64(unit, FALSE, -1, INVALIDr);
#endif
            }

            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_counter_thread: collect64 failed: "
                                      "%s\n"), soc_errmsg(rv)));
                err = 1;
            }
        }

        /* soc_counter_collect64 could trigger thread schedule */
        if (!(soc->counter_interval)) {
            COUNTER_UNLOCK(unit);
            break;
        }

        if ((soc->counter_interval)) {
            soc_controlled_counters_collect64(unit, FALSE);
            if (rv < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_counter_thread: soc_controlled_counters_collect64 failed: "
                                      "%s\n"), soc_errmsg(rv)));
                err = 1;
            }
        }

        /* soc_controlled_counters_collect64 could trigger thread schedule */
        if (!(soc->counter_interval)) {
            COUNTER_UNLOCK(unit);
            break;
        }

        /*
         * Check optional non CMIC counter DMA support entries
         * These counters are included in "show counter" output
         */
        if ((soc->counter_interval)) {
            if(soc_ctr_ctrl->collect_non_stat_dma) {
                soc_ctr_ctrl->collect_non_stat_dma(unit, -1, -1, -1, -1);
            }
        }

        COUNTER_UNLOCK(unit);

        /* soc_counter_collect_non_dma_entries could trigger thread schedule */
        if (!(soc->counter_interval)) {
            break;
        }

        /*
         * Callback for additional work
         * These counters are not included in "show counter" output
         */
        for (i = 0; i < SOC_COUNTER_EXTRA_CB_MAX; i++) {
            if (soc_counter_extra[unit][i] != NULL) {
                soc_counter_extra[unit][i](unit);
            }

            /* soc_counter_extra could trigger thread schedule */
            if (!(soc->counter_interval)) {
                if (err) {
                    rv = SOC_E_INTERNAL;
                }
                goto done;
            }
        }

#ifdef BCM_TRIDENT2_SUPPORT
        if (age_sync) {
            sal_sem_give(soc->l2x_age_sync);
            age_sync = FALSE;
        }
#endif

        /*
         * Forgive spurious errors
         */

        if (err) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "AbnormalThreadExit:"
                                      "soc_counter_thread: Too many errors\n")));
                rv = SOC_E_INTERNAL;
                goto done;
            }

#ifdef COUNTER_BENCH
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Iteration time: %d usec\n"),
                     SAL_USECS_SUB(sal_time_usecs(), start_time)));
#endif

        if (sync_gnt) {
            soc->counter_sync_req = 0;
            sync_gnt = 0;
        }
    }

    rv = SOC_E_NONE;

 done:
#ifdef BCM_TRIDENT2_SUPPORT
        if (age_sync) {
            sal_sem_give(soc->l2x_age_sync);
        }
#endif

    if (rv < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_counter_thread: Operation failed - exiting\n")));
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_COUNTER, __LINE__, rv);
    }

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_counter_thread: exiting\n")));

    soc->counter_pid = SAL_THREAD_ERROR;
    soc->counter_interval = 0;

    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, FALSE));

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(unit, DNX_ERR_RECOVERY_INTERNAL_THREAD_COUNTER);
    }
#endif

    sal_thread_exit(0);
}

/*
 * Function:
 *      soc_counter_start
 * Purpose:
 *      Start the counter collection, S/W accumulation process.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      flags - SOC_COUNTER_F_xxx flags.
 *      interval - collection period in micro-seconds.
 *              Using 0 is the same as calling soc_counter_stop().
 *      pbmp - bit map of ports to collact counters on.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_counter_start(int unit, uint32 flags, int interval, pbmp_t pbmp)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    char                pfmt[SOC_PBMP_FMT_LEN];
    int         rv = SOC_E_NONE;
    soc_port_t          p;
    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_counter_start: unit=%d flags=0x%x "
                         "interval=%d pbmp=%s\n"),
              unit, flags, interval, SOC_PBMP_FMT(pbmp, pfmt)));

    /* Stop if already running */
#ifdef ADAPTER_SERVER_MODE
#ifndef BCM_DNX_SUPPORT
    return SOC_E_NONE;
#endif
#endif /* ADAPTER_SERVER_MODE */
    if (soc->counter_interval != 0) {
        SOC_IF_ERROR_RETURN(soc_counter_stop(unit));
    }

    if (interval == 0) {
        return SOC_E_NONE;
    }

    /*
     * Create fresh semaphores
     * Semaphore already existing means other thread still holds it.
     * Unexpected results could happen if we mandatorily destroy it here.
     * So we need to retry soc_counter_stop(). If fail, then warn and return.
     */

    if ((soc->counter_trigger != NULL || soc->counter_intr != NULL) &&
        (rv = soc_counter_stop(unit)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_counter_start: sem unexpectedly survives\n")));
        return SOC_E_INTERNAL;
    }

    soc->counter_trigger =
        sal_sem_create("counter_trigger", sal_sem_BINARY, 0);

    soc->counter_intr =
        sal_sem_create("counter_intr", sal_sem_BINARY, 0);

    if (soc->counter_trigger == NULL || soc->counter_intr == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_counter_start: sem create failed\n")));
        return SOC_E_INTERNAL;
    }

    /* Create fresh lock */
    if (soc->counter_lock != NULL) {
        sal_spinlock_destroy(soc->counter_lock);
        soc->counter_lock = NULL;
    }

    soc->counter_lock = sal_spinlock_create("counter spinlock");

    if (soc->counter_lock == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_counter_start: lock create failed\n")));
        return SOC_E_INTERNAL;
    }

    sal_snprintf(soc->counter_name,
                 sizeof(soc->counter_name),
                 "bcmCNTR.%d", unit);

    SOC_PBMP_ASSIGN(soc->counter_pbmp, pbmp);
    PBMP_ITER(soc->counter_pbmp, p) {
        if ((SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), p))) {
            SOC_PBMP_PORT_REMOVE(soc->counter_pbmp, p);
        }
        if (IS_LB_PORT(unit, p)) {
            SOC_PBMP_PORT_REMOVE(soc->counter_pbmp, p);
        }
    }
    soc->counter_flags = flags;

    soc->counter_flags &= ~SOC_COUNTER_F_SWAP64; /* HW takes care of this */

    if (!soc_feature(unit, soc_feature_stat_dma) || SOC_IS_RCPU_ONLY(unit)) {
        soc->counter_flags &= ~SOC_COUNTER_F_DMA;
    }

    /*
     * The HOLD register is not supported by DMA on HUMV/Bradley,
     * but in order to allow certain test scripts to pass, we
     * optionally collect this register manually.
     */

#ifdef BCM_BRADLEY_SUPPORT
    soc->counter_flags &= ~SOC_COUNTER_F_HOLD;

    if (SOC_IS_HBX(unit)) {
        if (soc_property_get(unit, spn_CDMA_PIO_HOLD_ENABLE, 1)) {
            soc->counter_flags |= SOC_COUNTER_F_HOLD;
        }
    }
#endif /* BCM_BRADLEY_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (soc_feature(unit, soc_feature_bigmac_rxcnt_bug)) {
        /* Allocate buffer for DMA counter validation */
        soc_counter_tbuf[unit] = sal_alloc(SOC_COUNTER_TBUF_SIZE(unit),
                                           "counter_tbuf");
        if (soc_counter_tbuf[unit] == NULL) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_counter_thread: unit %d: "
                                  "failed to allocate temp counter buffer\n"),
                       unit));
        }
    }
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

    SOC_IF_ERROR_RETURN(soc_counter_autoz(unit, 0));

    /* Synchronize counter 'prev' values with current hardware counters */

    soc->counter_coll_prev = soc->counter_coll_cur = sal_time_usecs();

    if (soc->counter_n32 > 0) {
        COUNTER_LOCK(unit);
        rv = soc_counter_collect32(unit, TRUE);
        COUNTER_UNLOCK(unit);
        SOC_IF_ERROR_RETURN(rv);
    }

    if (soc->counter_n64 > 0) {
        COUNTER_LOCK(unit);
        if(SOC_IS_SAND(unit)) {
            rv = SOC_E_NONE;
        } else {
#if defined(BCM_XGS_SUPPORT)
            rv = soc_counter_collect64(unit,FALSE, -1, INVALIDr);
#endif
        }
        COUNTER_UNLOCK(unit);
        SOC_IF_ERROR_RETURN(rv);
    }

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    /*
     * Temporarily allow DBAL writes for current thread even if it's generally disabled.
     */
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
    {
        rv = dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_DBAL);
        SOC_IF_ERROR_RETURN(rv);
    }
#endif /*BCM_DNX_SUPPORT*/

    soc_controlled_counters_collect64(unit, TRUE);

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
    /*
     * revert dnxc_allow_hw_write_enable.
     */
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
    {
        rv = dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_DBAL);
    }
#endif /*BCM_DNX_SUPPORT*/

    /* Start the thread */

    if (interval != 0) {
        soc->counter_interval = interval;

        soc->counter_pid =
            sal_thread_create(soc->counter_name,
                              SAL_THREAD_STKSZ,
                              soc_property_get(unit,
                                               spn_COUNTER_THREAD_PRI,
                                               _SOC_CTR_THREAD_PRI),
                              soc_counter_thread, INT_TO_PTR(unit));

        if (soc->counter_pid == SAL_THREAD_ERROR) {
            soc->counter_interval = 0;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_counter_start: thread create failed\n")));
            return (SOC_E_INTERNAL);
        }

        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "soc_counter_start: complete\n")));
    }
#ifdef BCM_TOMAHAWK_SUPPORT
    if ((soc_feature(unit, soc_feature_counter_eviction)) &&
        (!SOC_WARM_BOOT(unit))) {
        /* During WB, eviction will be enabled post WB init */
        SOC_IF_ERROR_RETURN(soc_ctr_evict_start(unit, 0, interval));
    }
#endif /* BCM_TOMAHAWK_SUPPORT */


    return (SOC_E_NONE);
}



/*
 * Function:
 *      soc_counter_port_collect_enable_set
 * Purpose:
 *      Enable/disable the gathering of MIB statistics on this port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number #.
 *      enable: enable/disable the port's mib gathering.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_counter_port_collect_enable_set(int unit, int port, int enable)
{
    soc_control_t       *soc = SOC_CONTROL(unit);

    if (IS_LB_PORT(unit, port)) {
        return SOC_E_NONE;
    }

    if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port)) {
        return SOC_E_NONE;
    }

    COUNTER_LOCK(unit);

    if (enable) {
        SOC_PBMP_PORT_ADD(soc->counter_pbmp,port);
    } else {
        SOC_PBMP_PORT_REMOVE(soc->counter_pbmp,port);
    }

    COUNTER_UNLOCK(unit);

    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_counter_status
 * Purpose:
 *      Get the status of counter collection, S/W accumulation process.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      flags - SOC_COUNTER_F_xxx flags.
 *      interval - collection period in micro-seconds.
 *      pbmp - bit map of ports to collact counters on.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_counter_status(int unit, uint32 *flags, int *interval, pbmp_t *pbmp)
{
    soc_control_t       *soc = SOC_CONTROL(unit);

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_counter_status: unit=%d\n"), unit));

    *interval = soc->counter_interval;
    *flags = soc->counter_flags;
    SOC_PBMP_ASSIGN(*pbmp, soc->counter_pbmp);

    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_counter_sync
 * Purpose:
 *      Force an immediate counter update
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Ensures that ALL counter activity that occurred before the sync
 *      is reflected in the results of any soc_counter_get()-type
 *      routine that is called after the sync.
 */

int
soc_counter_sync(int unit)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_timeout_t       to;
    uint32              stat_sync_timeout;

    if (soc->counter_interval == 0) {
        return SOC_E_DISABLED;
    }

    /* Trigger a collection */

    soc->counter_sync_req = TRUE;

    sal_sem_give(soc->counter_trigger);

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_counter_eviction)) {
        SOC_IF_ERROR_RETURN(soc_counter_eviction_sync(unit));
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    if (SAL_BOOT_QUICKTURN) {
        stat_sync_timeout = STAT_SYNC_TIMEOUT_QT;
    } else if (SAL_BOOT_BCMSIM) {
        stat_sync_timeout = STAT_SYNC_TIMEOUT_BCMSIM;
    } else if (SOC_IS_RCPU_ONLY(unit)) {
        stat_sync_timeout = STAT_SYNC_TIMEOUT*4;
    } else {
        stat_sync_timeout = STAT_SYNC_TIMEOUT;
    }
    stat_sync_timeout = soc_property_get(unit,
                                         spn_BCM_STAT_SYNC_TIMEOUT,
                                         stat_sync_timeout);
    soc_timeout_init(&to, stat_sync_timeout, 0);
    while (soc->counter_sync_req) {
        if (soc_timeout_check(&to)) {
            if (soc->counter_sync_req) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_counter_sync: counter thread not responding\n")));
                soc->counter_sync_req = FALSE;
                return SOC_E_TIMEOUT;
            }
        }

        sal_usleep(10000);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_counter_stop
 * Purpose:
 *      Terminate the counter collection, S/W accumulation process.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_counter_stop(int unit)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 rv = SOC_E_NONE;
    soc_timeout_t       to;
    sal_usecs_t         cdma_timeout;
#ifdef BCM_XGS_SUPPORT
#ifdef BCM_CMICM_SUPPORT
    int cmc = SOC_PCI_CMC(unit);
#endif
#endif

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_counter_stop: unit=%d\n"), unit));

    if (SAL_BOOT_QUICKTURN) {
        cdma_timeout = CDMA_TIMEOUT_QT;
    } else if (SAL_BOOT_BCMSIM || SAL_BOOT_PLISIM) {
        cdma_timeout = CDMA_TIMEOUT_BCMSIM;
    } else if (SOC_IS_RCPU_ONLY(unit)) {
        cdma_timeout = CDMA_TIMEOUT*10;
    } else {
        cdma_timeout = CDMA_TIMEOUT*5;
    }
    cdma_timeout = soc_property_get(unit, spn_CDMA_TIMEOUT_USEC, cdma_timeout);

    /* Stop thread if present. */

    if (soc->counter_interval != 0) {
        sal_thread_t    sample_pid;

        /*
         * Signal by setting interval to 0, and wake up thread to speed
         * its exit.  It may also be waiting for the hardware interrupt
         * semaphore.  Wait a limited amount of time for it to exit.
         */

        soc->counter_interval = 0;

        sal_sem_give(soc->counter_intr);
        sal_sem_give(soc->counter_trigger);

        soc_timeout_init(&to, cdma_timeout, 0);

        while ((sample_pid = soc->counter_pid) != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_counter_stop: thread did not exit\n")));
                soc->counter_pid = SAL_THREAD_ERROR;
                rv = SOC_E_TIMEOUT;
                break;
            }

            sal_usleep(10000);
        }
    }

#ifdef BCM_XGS_SUPPORT
    if ((soc->counter_flags & SOC_COUNTER_F_DMA) &&
        !soc_feature(unit, soc_feature_sbusdma)) {
        uint32          val;

        /*
         * Disable hardware counter scanning.
         * Turn off all ports to speed up final scan cycle.
         *
         * This cleanup is done here instead of at the end of the
         * counter thread so counter DMA will turn off even if the
         * thread is non responsive.
         */

#ifdef BCM_CMICM_SUPPORT
        if(soc_feature(unit, soc_feature_cmicm)) {

            soc_cmicm_intr0_disable(unit, IRQ_CMCx_STAT_ITER_DONE);

            val = soc_pci_read(unit, CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc));
            soc_reg_field_set(unit, CMIC_CMC0_STAT_DMA_CFGr, &val, ENf, 0);
            soc_reg_field_set(unit, CMIC_CMC0_STAT_DMA_CFGr, &val, E_Tf, 0);
            soc_pci_write(unit, CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc), val);
            soc_pci_write(unit, CMIC_CMCx_STAT_DMA_PORTS_0_OFFSET(cmc), 0);
            if (SOC_REG_IS_VALID(unit, CMIC_CMC0_STAT_DMA_PORTS_1r)) { 
                soc_pci_write(unit, CMIC_CMCx_STAT_DMA_PORTS_1_OFFSET(cmc), 0);
            }
            if (SOC_REG_IS_VALID(unit, CMIC_CMC0_STAT_DMA_PORTS_2r)) { 
                soc_pci_write(unit, CMIC_CMCx_STAT_DMA_PORTS_2_OFFSET(cmc), 0);
            }
        } else
#endif
        {

            soc_intr_disable(unit, IRQ_STAT_ITER_DONE);

            READ_CMIC_STAT_DMA_SETUPr(unit, &val);
            soc_reg_field_set(unit, CMIC_STAT_DMA_SETUPr, &val, ENf, 0);
            soc_reg_field_set(unit, CMIC_STAT_DMA_SETUPr, &val, E_Tf, 0);
            WRITE_CMIC_STAT_DMA_SETUPr(unit, val);
            WRITE_CMIC_STAT_DMA_PORTSr(unit, 0);
#if defined(BCM_RAPTOR_SUPPORT)
            if (soc_feature(unit, soc_feature_register_hi)) {
                WRITE_CMIC_STAT_DMA_PORTS_HIr(unit, 0);
            }
#endif /* BCM_RAPTOR_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
            if (SOC_IS_TR_VL(unit)) {
                WRITE_CMIC_STAT_DMA_PORTS_HIr(unit, 0);
            }
#endif /* BCM_TRIUMPH_SUPPORT */
        }

        /*
         * Wait for STAT_DMA_ACTIVE to go away, with a timeout in case
         * it never does.
         */

        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "soc_counter_stop: waiting for idle\n")));

        soc_timeout_init(&to, cdma_timeout, 0);
#ifdef BCM_CMICM_SUPPORT
        if(soc_feature(unit, soc_feature_cmicm)) {
            while (soc_pci_read(unit, CMIC_CMCx_STAT_DMA_STAT_OFFSET(cmc)) & ST_CMCx_DMA_ACTIVE) {
                if (soc_timeout_check(&to)) {
                    rv = SOC_E_INTERNAL;
                    break;
                }
            }
        } else
#endif
        {
            while (soc_pci_read(unit, CMIC_DMA_STAT) & DS_STAT_DMA_ACTIVE) {
                if (soc_timeout_check(&to)) {
                    rv = SOC_E_TIMEOUT;
                    break;
                }
            }
        }
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
#ifdef BCM_SBUSDMA_SUPPORT
    if ((soc->counter_flags & SOC_COUNTER_F_DMA) &&
        soc_feature(unit, soc_feature_sbusdma)) {
        int err;
        if ((err = _soc_counter_sbudma_desc_free_all(unit)) != 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_counter_stop: [%d] Desc free error(s)\n"),
                       err));
        }
    }
#endif
#endif

#ifdef BCM_BRADLEY_SUPPORT
    if (soc_counter_tbuf[unit]) {
        sal_free(soc_counter_tbuf[unit]);
        soc_counter_tbuf[unit] = NULL;
    }
#endif /* BCM_BRADLEY_SUPPORT */
#endif /* BCM_XGS_SUPPORT */

    if (rv != SOC_E_TIMEOUT) {
        if (NULL != soc->counter_intr) {
            sal_sem_destroy(soc->counter_intr);
            soc->counter_intr = NULL;
        }
        if (NULL != soc->counter_trigger) {
            sal_sem_destroy(soc->counter_trigger);
            soc->counter_trigger = NULL;
        }
    }

    if (soc->counter_lock != NULL) {
        sal_spinlock_destroy(soc->counter_lock);
        soc->counter_lock = NULL;
    }

    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_counter_stop: stopped\n")));

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_counter_eviction)) {
        SOC_IF_ERROR_RETURN(soc_ctr_evict_stop(unit));
    }
#endif /* BCM_TOMAHAWK_SUPPORT */

    return (rv);
}

/*
 * Function:
 *      soc_counter_extra_register
 * Purpose:
 *      Register callback for additional counter collection.
 * Parameters:
 *      unit - The SOC unit number
 *      fn   - Callback function.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
int
soc_counter_extra_register(int unit, soc_counter_extra_f fn)
{
    int i;

    if (fn == NULL) {
        return SOC_E_PARAM;
    }

    for (i = 0; i < SOC_COUNTER_EXTRA_CB_MAX; i++) {
        if (soc_counter_extra[unit][i] == fn) {
            return SOC_E_NONE;
        }
    }

    for (i = 0; i < SOC_COUNTER_EXTRA_CB_MAX; i++) {
        if (soc_counter_extra[unit][i] == NULL) {
            soc_counter_extra[unit][i] = fn;
            return SOC_E_NONE;
        }
    }

    return SOC_E_FULL;
}

/*
 * Function:
 *      soc_counter_extra_unregister
 * Purpose:
 *      Unregister callback for additional counter collection.
 * Parameters:
 *      unit - The SOC unit number
 *      fn   - Callback function.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
int
soc_counter_extra_unregister(int unit, soc_counter_extra_f fn)
{
    int i;

    if (fn == NULL) {
        return SOC_E_PARAM;
    }

    for (i = 0; i < SOC_COUNTER_EXTRA_CB_MAX; i++) {
        if (soc_counter_extra[unit][i] == fn) {
            soc_counter_extra[unit][i] = NULL;
            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

int
soc_counter_timestamp_get(int unit, soc_port_t port,
                          uint32 *timestamp)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NOT_FOUND;

    if (!soc_feature(unit, soc_feature_timestamp_counter)) {
        return rv;
    }

    if (soc->counter_timestamp_fifo[port] == NULL) {
        return rv;
    }

    COUNTER_LOCK(unit);
    if (!SHR_FIFO_IS_EMPTY(soc->counter_timestamp_fifo[port])) {
        SHR_FIFO_POP(soc->counter_timestamp_fifo[port], timestamp);
        rv = SOC_E_NONE;
    }
    COUNTER_UNLOCK(unit);
    return rv;
}

/* Function to return associated dma for a given dma structure.
 * Example, Associated DMA for ING_FLEX_PKT is ING_FLEX_BYTE and vice versa.
 */
STATIC
void soc_counter_get_associated_dma(int unit, soc_reg_t id,
                                    soc_counter_non_dma_t *non_dma,
                                    soc_counter_non_dma_t **non_dma_temp)
{
    if (non_dma_temp == NULL) {
        return;
    }

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
        /* If CLR_ON_READ is NOT set, Return NULL */
        *non_dma_temp = NULL;
        return;
    }
    switch (id) {
        case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
        case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:
        case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
        case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
        case SOC_COUNTER_NON_DMA_EFP_PKT:
            *non_dma_temp = non_dma + 1;
            break;
        case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE:
        case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
        case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
        case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
        case SOC_COUNTER_NON_DMA_EFP_BYTE:
            *non_dma_temp = non_dma - 1;
            break;
        default:
            *non_dma_temp = NULL;
            break; /* all other cases not handled yet */
    }

}

/*
 * Function:
 *      soc_counter_generic_collect_non_dma_entries
 * Purpose:
 *      This routine is similar to the soc_counter_collect_non_dma_entries
 *      routine. The difference is that soc_counter_collect_non_dma_entries can
 *      handle only SOC_CTR_INSTANCE_TYPE_PORT while this routine can handle the
 *      types defined in soc_ctr_instance_type_e.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      ctrl_info - StrataSwitch port #.
 *      base_idx - base index returned by generic_get_info.
 *      ar_idx -  index.
 *      ctr_reg - if INVALIDr, collects and accumulates counter values for all
 *                else collects and accumulates for a specified port.
 *                Currently, this functionality is not being used.
 * Returns:
 *      NONE
 * Notes:
 */

STATIC void
soc_counter_generic_collect_non_dma_entries(int unit,
                                            soc_ctr_control_info_t ctrl_info,
                                            int base_idx,
                                            int ar_idx,
                                            soc_reg_t ctr_reg)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma, *non_dma_temp;
    int base_index = 0, width, i, j;
    int id = 0;
    int idx = 0;
    int port=0;
    uint32 fval[2];
    uint64 ctr_new, ctr_prev, ctr_diff, wrap_amt;
    int rv = 0, count, entry_words;
    COUNTER_ATOMIC_DEF s = SOC_CONTROL(unit);
    int dma_base_index = 0;
    int num_non_dma_entries = 0;
    soc_mem_t dma_mem;
    int non_dma_counter_end = SOC_COUNTER_NON_DMA_END -
                              SOC_COUNTER_NON_DMA_START;
    static int ctr_dma_ct = 0;
    int index_max = 0, pipe = 0, entries_per_pipe = 0, inst_pipe = -1;
    int pipe_start = 0, pool_id;
    uint32 subset_ct = 0; /* For Parent-Child Model */
    uint32 fld_ct = 2;
    soc_info_t *si = NULL;

    si = &SOC_INFO(unit);

    soc = SOC_CONTROL(unit);

    if (soc->counter_non_dma == NULL) {
        return;
    }

    /*
     * if ctr_reg != -1, retrieve stats for a
     * specific port and queue from the hardware
     * update the sw copy of the same.
     */
    if (ctr_reg >= NUM_SOC_REG) {
        id = ctr_reg - SOC_COUNTER_NON_DMA_START;
        non_dma_counter_end = id + 1;
        idx = (ar_idx < 0)? 0: ar_idx;
    }

    for (; id < non_dma_counter_end; id++) {
        non_dma = &soc->counter_non_dma[id];

        if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID) && ctr_reg == INVALIDr) {
            continue;
        }

        /* If signaled to exit then return  */
        if (!soc->counter_interval) {
            return;
        }

        width = entry_words = 0;

        if (non_dma->mem != INVALIDm) {
            count = 0;

            /* If there is subset of counters, use subset info for DMA */
            if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                    (non_dma->extra_ctrs != NULL)) {
                /* Do PACKET_CTRf and BYTE_CTRf sequentially. Hence 2x loop */
                subset_ct = non_dma->extra_ctr_ct << 1;
#ifdef BCM_TOMAHAWK3_SUPPORT
                if(SOC_IS_TOMAHAWK3(unit) && (non_dma->extra_ctr_fld_ct != 0)) {
                    subset_ct = non_dma->extra_ctr_ct;
                    fld_ct = non_dma->extra_ctr_fld_ct;
                }
#endif
                non_dma = non_dma->extra_ctrs;
            } else {
                subset_ct = 1;
            }

            do {
                /* If signaled to exit then return  */
                if (!soc->counter_interval) {
                    return;
                }
                if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID) && ctr_reg == INVALIDr) {
                    break;
                }

                /* Check if this Counter is up for DMA.
                 * During Slow-DMA, not all counters are up for DMA at the same
                 * time/frequency.
                 */
                if ((non_dma->flags & _SOC_COUNTER_NON_DMA_CTR_EVICT) &&
                    (!soc_counter_non_dma_ready(unit, non_dma, &ctr_dma_ct))) {
                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit,
                                         "id %d(%d): NOT pickd for DMA(pipes %d). Ct %d, mem:\n\t%d\n"),
                              id, SOC_COUNTER_NON_DMA_START, count,
                              NUM_PIPE(unit), non_dma->mem));
                    for (i = 0; i < NUM_PIPE(unit); i++) {
                        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit, "\t%d\n"), non_dma->dma_mem[i]));
                    }
                    count++;
                    non_dma++;
                    continue;
                }

                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                             (BSL_META_U(unit,
                                     "id %d(%d): picked for DMA(Pipes %d). mem:\n\t%d\n"),
                              id, SOC_COUNTER_NON_DMA_START, NUM_PIPE(unit),
                          non_dma->mem));
                for (i = 0; i < NUM_PIPE(unit); i++) {
                    LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                         (BSL_META_U(unit, "\t%d\n"), non_dma->dma_mem[i]));
                }
                entry_words = soc_mem_entry_words(unit, non_dma->mem);

                /* Non-DMA can have either register based or memory based
                 * counters. INVALIDr signifies memory based */
                if (ctr_reg == INVALIDr) {
                    for (i = 0; i < NUM_PIPE(unit); i++) {
                        /* Do only if the Pipe mem is VALID. */
                        if (non_dma->dma_mem[i] == INVALIDm) {
                            continue;
                        }
                        /* Skip if no enabled ports in pipe */
                        if (SOC_PBMP_IS_NULL(PBMP_PIPE(unit, i))) {
                            continue;
                        }
                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_DO_DMA) {

                            if (non_dma->dma_buf[i] == NULL) {
                                continue;
                            }

                            rv = soc_mem_read_range(unit,
                                                    non_dma->dma_mem[i],
                                                    MEM_BLOCK_ANY,
                                                    non_dma->dma_index_min[i],
                                                    non_dma->dma_index_max[i],
                                                    &non_dma->dma_buf[i]);

                            if (SOC_FAILURE(rv)) {
                                return;
                            }
                        }
                    }

                    base_index = non_dma->base_index;
                    index_max = non_dma->num_entries;
                    entries_per_pipe = non_dma->num_entries / NUM_PIPE(unit);
                    num_non_dma_entries = index_max;

                } else {
                    subset_ct = 1; /* Only for given instance type */
                    /* Collect counter values for a specific instance */

                    base_index = base_idx + ar_idx;
                    num_non_dma_entries = 1;
                    if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_XPE_PORT) {
                        int xpe_inst = (ctrl_info.instance >> 10) & 0xf;
                        int port = ctrl_info.instance >> 14;
                        soc_counter_non_dma_t *parent_dma;

                        parent_dma = &soc->counter_non_dma[id];
                        inst_pipe = si->port_pipe[port];

                        if ((xpe_inst >= 0) && (xpe_inst < NUM_XPE(unit))) {
                            if (parent_dma->extra_ctrs) {
                                non_dma = &parent_dma->extra_ctrs[xpe_inst << 1];
                            }
                            /* Each XPE has 2 pipe entries */
                            if ( 1 == NUM_PIPE(unit)) {
                                entries_per_pipe = non_dma->num_entries;
                            } else
                            {
                                entries_per_pipe = non_dma->num_entries / 2;
                            }
                            dma_mem = non_dma->dma_mem[inst_pipe];
                            if (dma_mem == INVALIDm) {
                                break;
                            }

                            dma_base_index = (base_idx - non_dma->base_index) %
                                             entries_per_pipe +
                                             non_dma->dma_index_min[inst_pipe];
                            rv = soc_mem_read(unit, dma_mem, MEM_BLOCK_ANY,
                                              dma_base_index + idx,
                                              non_dma->dma_buf[inst_pipe]);
                            if (SOC_FAILURE(rv)) {
                                return;
                            }
                        }
                    } else if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_ITM_PORT) {
                        /* Decode ITM instance and port for TYPE_ITM_PORT */
                        int itm_inst =
                            (ctrl_info.instance & CTR_INSTANCE_BIT_MASK_ITM) >>
                            CTR_INSTANCE_BIT_SHIFT_ITM;
                        int port =
                            (ctrl_info.instance & CTR_INSTANCE_BIT_MASK_PORT) >>
                            CTR_INSTANCE_BIT_SHIFT_PORT;
                        soc_counter_non_dma_t *parent_dma;

                        parent_dma = &soc->counter_non_dma[id];
                        inst_pipe = si->port_pipe[port];

                        if ((itm_inst >= 0) && (itm_inst < NUM_ITM(unit)) &&
                            (parent_dma->extra_ctrs)) {
                            non_dma = &parent_dma->extra_ctrs[itm_inst];
                            entries_per_pipe = non_dma->num_entries;
                            dma_mem = non_dma->dma_mem[inst_pipe];
#ifdef BCM_TOMAHAWK3_SUPPORT
                            /* In normal case, only dma_mem[0] is filled */
                            if (SOC_IS_TOMAHAWK3(unit) &&
                                (SOC_CONTROL(unit)->halfchip == 0)) {
                                dma_mem = non_dma->dma_mem[0];
                            }
#endif
                            if (dma_mem == INVALIDm) {
                                break;
                            }

                            /* Retrieve table index by substract the base_index */
                            dma_base_index = (base_idx - non_dma->base_index);
                            rv = soc_mem_read(unit, dma_mem, MEM_BLOCK_ANY,
                                              dma_base_index + idx,
                                              non_dma->dma_buf[inst_pipe]);
                            if (SOC_FAILURE(rv)) {
                                return;
                            }
                        }
                    } else if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_PORT) {
                        dma_mem = non_dma->dma_mem[0];
                        dma_base_index = base_idx - non_dma->base_index +
                                         non_dma->dma_index_min[0];
                        inst_pipe = 0;

                        for (i = 1; i < NUM_PIPE(unit); i++) {
                            if (non_dma->dma_buf[i] == NULL) {
                                continue;
                            }

                            if (dma_base_index > non_dma->dma_index_max[i - 1]) {
                                inst_pipe = i;
                                dma_mem = non_dma->dma_mem[i];
                                dma_base_index -= non_dma->dma_index_max[i - 1] + 1;
                                dma_base_index += non_dma->dma_index_min[i];
                            }
                        }
                        rv = soc_mem_read(unit, dma_mem, MEM_BLOCK_ANY,
                                          dma_base_index + idx,
                                          non_dma->dma_buf[inst_pipe]);
                        if (SOC_FAILURE(rv)) {
                            return;
                        }
                    } else if (ctrl_info.instance_type ==
                               SOC_CTR_INSTANCE_TYPE_PIPE) {
                        inst_pipe = ctrl_info.instance;
                        entries_per_pipe = non_dma->num_entries / NUM_PIPE(unit);
                        rv = soc_mem_read(unit,
                                          non_dma->dma_mem[inst_pipe],
                                          MEM_BLOCK_ANY, ar_idx,
                                          non_dma->dma_buf[inst_pipe]);
                        if (SOC_FAILURE(rv)) {
                            return;
                        }
                    }
                    else if ((ctrl_info.instance_type
                                == SOC_CTR_INSTANCE_TYPE_POOL) ||
                             (ctrl_info.instance_type
                                == SOC_CTR_INSTANCE_TYPE_POOL_PIPE)) {
                        /* Only POOL_PIPE is possible. Type POOL was translated to
                           type POOL_PIPE in _soc_counter_generic_get */
                        pipe_start = (ctrl_info.instance & 0xF);
                        pool_id = (ctrl_info.instance >> 4) & 0x3F;

                        non_dma = &(non_dma[pool_id << 1]);

                        rv = soc_mem_read(unit, non_dma->dma_mem[pipe_start],
                                              MEM_BLOCK_ANY, idx,
                                              non_dma->dma_buf[pipe_start]);
                        if (SOC_FAILURE(rv)) {
                            return;
                        }
                        entries_per_pipe = non_dma->num_entries / NUM_PIPE(unit);
                    }
                    else if ((ctrl_info.instance_type
                                == SOC_CTR_INSTANCE_TYPE_FT_GROUP)) {

/*                        non_dma = &(non_dma[pool_id << 1]);*/

                        rv = soc_mem_read(unit, non_dma->dma_mem[0],
                                              MEM_BLOCK_ANY, idx,
                                              non_dma->dma_buf[0]);
                        if (SOC_FAILURE(rv)) {
                            return;
                        }
                        entries_per_pipe = non_dma->num_entries / NUM_PIPE(unit);
                    }

                }

                pipe = 0;
                non_dma_temp = non_dma;
                for (j = 0; j < fld_ct; j++) { /* for packet and byte */
                    for (i = 0; i < num_non_dma_entries; i++) {
                        if (ctr_reg == INVALIDr) {
                            pipe = i / entries_per_pipe;
                            /* buffer is NULL. Can happen if memory has holes */
                            if (!non_dma->dma_buf[pipe]) {
                                /* Skip the pipe indices if dma_buf for given pipe
                                 * is NULL.
                                 * Also -1 because for-loop increases index by 1
                                 * after continue (-1).
                                 */
                                i += entries_per_pipe - 1;
                                continue;
                            }
                            soc_mem_field_get(unit, non_dma->mem,
                                              &non_dma->dma_buf[pipe][entry_words *
                                              (i % entries_per_pipe)],
                                              non_dma_temp->field, fval);
                        } else {
                            /* Only for given instance type */
                            if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_PIPE) {
                                /* Recalculate the base_index when converting from
                                   pkt to byte or byte to pkt */
                                if (j > 0) {
                                    base_index += non_dma_temp->base_index -
                                                  non_dma->base_index;
                                }
                                soc_mem_field_get(unit, non_dma->mem,
                                                  &non_dma->dma_buf[inst_pipe]
                                                  [entry_words *(i % entries_per_pipe)]
                                                  , non_dma_temp->field, fval);
                            } else if ((ctrl_info.instance_type ==
                                       SOC_CTR_INSTANCE_TYPE_XPE_PORT) ||
                                       (ctrl_info.instance_type ==
                                        SOC_CTR_INSTANCE_TYPE_PORT)) {
                                /* Recalculate the base_index when converting from
                                   pkt to byte or byte to pkt */
                                if (j > 0) {
                                    base_index += non_dma_temp->base_index -
                                                  non_dma->base_index;
                                }
                                if (inst_pipe != -1) {
                                    soc_mem_field_get(unit,
                                                      non_dma->dma_mem[inst_pipe],
                                                      &non_dma->dma_buf[inst_pipe][0],
                                                      non_dma_temp->field, fval);
                                } else {
                                    break;
                                }
                            }
                            else if ((ctrl_info.instance_type ==
                                      SOC_CTR_INSTANCE_TYPE_POOL) ||
                                     (ctrl_info.instance_type ==
                                      SOC_CTR_INSTANCE_TYPE_POOL_PIPE)) {
                                 /* num_non_dma_entries is always 1 because only
                                    POOL_PIPE is possible */
                                 pipe = pipe_start;
                                 i = pipe_start * entries_per_pipe;

                                 if (!non_dma->dma_buf[pipe]) {
                                       continue;
                                  }
                                 soc_mem_field_get(unit, non_dma->mem,
                                              non_dma->dma_buf[pipe],
                                              non_dma_temp->field, fval);
                                /* recalculate base_index for pkt or byte */
                                base_index = (non_dma_temp->base_index) + idx;
                            } else if ((ctrl_info.instance_type ==
                                       SOC_CTR_INSTANCE_TYPE_ITM_PORT)) {
                                if (inst_pipe != -1) {
                                    soc_mem_field_get(unit,
                                                      non_dma->dma_mem[0],
                                                      &non_dma->dma_buf[inst_pipe][0],
                                                      non_dma_temp->field, fval);
                                } else {
                                    break;
                                }
                            }
                            else if ((ctrl_info.instance_type ==
                                      SOC_CTR_INSTANCE_TYPE_FT_GROUP)) {
                                 /* num_non_dma_entries is always 1 because only
                                    POOL_PIPE is possible */
                                 if (!non_dma->dma_buf[0]) {
                                       continue;
                                  }
                                 soc_mem_field_get(unit, non_dma->mem,
                                              non_dma->dma_buf[0],
                                              non_dma_temp->field, fval);
                                /* recalculate base_index for pkt or byte */
                                base_index = (non_dma_temp->base_index) + idx;
                            }
                        }
                        if (non_dma_temp->field != INVALIDf) {
                            width = soc_mem_field_length(unit, non_dma_temp->mem,
                                non_dma_temp->field);
                        } else {
                            width = 32;
                        }
                        if (width <= 32) {
                            /*
                             * COVERITY
                             * faval is passed as output parameter to function
                             * soc_mem_field_get to get initialized
                             */
                            /* coverity[uninit_use : FALSE] */
                            COMPILER_64_SET(ctr_new, 0, fval[0]);
                        } else {
                            /* coverity[uninit_use : FALSE] */
                            COMPILER_64_SET(ctr_new, fval[1], fval[0]);
                        }
                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_EXTRA_COUNT) {
                            if (!COMPILER_64_IS_ZERO(ctr_new)) {
                                COMPILER_64_SUB_32(ctr_new, 1);
                            }
                        }
                        COUNTER_ATOMIC_BEGIN(s);
                        ctr_prev = soc->counter_hw_val[base_index + i];
                        COUNTER_ATOMIC_END(s);

                        if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                            if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                                COUNTER_ATOMIC_BEGIN(s);
                                COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                                COUNTER_ATOMIC_END(s);
                                continue;
                            }
                        }

                        ctr_diff = ctr_new;

                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_PEAK) {
                            COUNTER_ATOMIC_BEGIN(s);
                            if (COMPILER_64_GT(ctr_new, soc->counter_sw_val[base_index + i])) {
                                soc->counter_sw_val[base_index + i] = ctr_new;
                            }
                            soc->counter_hw_val[base_index + i] = ctr_new;
                            COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }

                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_CURRENT) {
                            COUNTER_ATOMIC_BEGIN(s);
                            soc->counter_sw_val[base_index + i] = ctr_new;
                            soc->counter_hw_val[base_index + i] = ctr_new;
                            COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                            COUNTER_ATOMIC_END(s);
                            continue;
                        }

                        if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ)) {
                            if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                                if (width < 32) {
                                    COMPILER_64_ADD_32(ctr_diff, 1U << width);
                                } else if (width < 64) {
                                    COMPILER_64_SET(wrap_amt, 1U << (width - 32), 0);
                                    COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                                }
                            }

                            COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                        }

                        COUNTER_ATOMIC_BEGIN(s);
                        COMPILER_64_ADD_64(soc->counter_sw_val[base_index + i],
                                           ctr_diff);
                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_CLEAR_ON_READ) {
                            ctr_new = soc->counter_sw_val[base_index + i];
                            ctr_diff = ctr_new;
                            COMPILER_64_SUB_64(ctr_diff, ctr_prev);
                        }
                        soc->counter_hw_val[base_index + i] = ctr_new;
                        soc->counter_delta[base_index + i] = ctr_diff;
                        COUNTER_ATOMIC_END(s);
                    }
                    /* Get the associated dma. Get the byte from the pkt, or pkt from the byte */
                    non_dma_temp = NULL;
                    /* do this check for CLEAR_ON_READ CASE ONLY *
                     */
                    if ((ctr_reg != INVALIDr)) {
                        soc_counter_get_associated_dma(unit, ctr_reg, non_dma,
                                                       &non_dma_temp);
                    }
                    if (non_dma_temp == NULL) {
                        /* Don't loop for the second time if the associated dma is NULL
                           or when doing a global sync */
                        break;
                    }
                }
                count++;
                non_dma++;
            } while ((count < subset_ct) && non_dma);
        } else if (non_dma->reg != INVALIDr) {
            pbmp_t counter_pbmp;

            count = 0;
            if (non_dma->field != INVALIDf) {
                width = soc_reg_field_length(unit, non_dma->reg,
                                             non_dma->field);
            } else {
                width = 32;
            }

            /*
             * If stats retrieval is for a specific
             * port, set the pbmp for counter collection.
             */
            if (ctr_reg >= NUM_SOC_REG) {
                SOC_PBMP_CLEAR(counter_pbmp);
                if (ctrl_info.instance_type == SOC_CTR_INSTANCE_TYPE_ITM_PORT) {
                    /* For ITM counter register, port is used as ITM instance */
                    port =
                        (ctrl_info.instance & CTR_INSTANCE_BIT_MASK_ITM) >>
                        CTR_INSTANCE_BIT_SHIFT_ITM;
                } else {
                    port = ctrl_info.instance;
                }
                SOC_PBMP_PORT_ADD(counter_pbmp, port);
            } else {
                SOC_PBMP_CLEAR(counter_pbmp);
                SOC_PBMP_ASSIGN(counter_pbmp, soc->counter_pbmp);
            }

            PBMP_ITER(counter_pbmp, port) {
                /*
                 * stats retrieval for a specific port.
                 */

                if (ctr_reg >= NUM_SOC_REG) {
                    if (!SOC_PBMP_MEMBER(counter_pbmp, port)) {
                        continue;
                    }

                    i = idx;
                    count = i + 1;
                    base_index = base_idx;
                } else {
                    if (non_dma->entries_per_port == 0) {
                        /*
                         * OK to over-write port variable as loop
                         * breaks after 1 iter.  Check below for
                         * the same codition.
                         */
                        port = REG_PORT_ANY;
                        count = non_dma->num_entries;
                        base_index = non_dma->base_index;
                    } else {
                        if (!SOC_PBMP_MEMBER(non_dma->pbmp, port)) {
                            continue;
                        }

                        if (non_dma->flags & _SOC_COUNTER_NON_DMA_PERQ_REG) {
                            count = num_cosq[unit][port];
                            base_index = non_dma->base_index;
                            for (i = 0; i < port; i++) {
                                base_index += num_cosq[unit][i];
                            }
                        } else {
                            count = non_dma->entries_per_port;
                            base_index = non_dma->base_index + port * count;
                        }
                    }
                }

                for (i = 0; i < count; i++) {
                    /* coverity[negative_returns : FALSE] */
                    rv = soc_reg_get(unit, non_dma->reg, port, i, &ctr_new);
                    if (SOC_FAILURE(rv)) {
                        return;
                    }
                    COUNTER_ATOMIC_BEGIN(s);
                    ctr_prev = soc->counter_hw_val[base_index + i];
                    COUNTER_ATOMIC_END(s);

                    if (COMPILER_64_EQ(ctr_new, ctr_prev)) {
                        COUNTER_ATOMIC_BEGIN(s);
                        COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                        COUNTER_ATOMIC_END(s);
                        continue;
                    }

                    ctr_diff = ctr_new;

                    if (non_dma->flags & _SOC_COUNTER_NON_DMA_PEAK) {
                        COUNTER_ATOMIC_BEGIN(s);
                        if (COMPILER_64_GT(ctr_new,
                                           soc->counter_sw_val[base_index + i])) {
                            soc->counter_sw_val[base_index + i] = ctr_new;
                        }
                        soc->counter_hw_val[base_index + i] = ctr_new;
                        COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                        COUNTER_ATOMIC_END(s);
                        continue;
                    }

                    if (non_dma->flags & _SOC_COUNTER_NON_DMA_CURRENT) {
                        COUNTER_ATOMIC_BEGIN(s);
                        soc->counter_sw_val[base_index + i] = ctr_new;
                        soc->counter_hw_val[base_index + i] = ctr_new;
                        COMPILER_64_ZERO(soc->counter_delta[base_index + i]);
                        COUNTER_ATOMIC_END(s);
                        continue;
                    }

                    if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                        if (width < 32) {
                            COMPILER_64_ADD_32(ctr_diff, 1U << width);
                        } else if (width < 64) {
                            COMPILER_64_SET(wrap_amt, 1U << (width - 32), 0);
                            COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                        }
                    }

                    COMPILER_64_SUB_64(ctr_diff, ctr_prev);

                    COUNTER_ATOMIC_BEGIN(s);
                    COMPILER_64_ADD_64(soc->counter_sw_val[base_index + i],
                                       ctr_diff);
                    soc->counter_hw_val[base_index + i] = ctr_new;
                    soc->counter_delta[base_index + i] = ctr_diff;
                    COUNTER_ATOMIC_END(s);
                }
                if (non_dma->entries_per_port == 0) {
                    break;
                }
            }
        } else {
            continue;
        }
    }
    ctr_dma_ct++;
}

/*
 * Function:
 *      _soc_counter_port_pbmp_add(int unit, int port)
 * Purpose:
 *      add port to counter_pbmp
 *
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number #.
 *
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_counter_port_pbmp_add(int unit, int port)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int phy_port;
    int blk;
    int bindex;
    int blktype;
    int port_idx;
    soc_ctr_type_t ctype;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    if (phy_port < 0) {
        return SOC_E_INTERNAL;
    }

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);
#if defined(BCM_MONTEREY_SUPPORT)
     if (!(IS_ROE_PORT(unit, port)) && SOC_IS_MONTEREY(unit)) {
         if ( ((phy_port > 0) && (phy_port < 13)) ||
             ((phy_port > 48) && (phy_port < 65))) {
              blk = SOC_PORT_IDX_BLOCK(unit, phy_port,1);
              bindex = SOC_PORT_IDX_BINDEX(unit, phy_port,1);
         }
    }
#endif
    if (blk < 0 && bindex < 0) { /* End of regsfile port list */
        return SOC_E_NONE;
    }

    for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
         port_idx++) {
        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
        if (blk < 0) { /* End of block list of each port */
            break;
        }

        blktype = SOC_BLOCK_INFO(unit, blk).type;
        switch (blktype) {
        case SOC_BLK_CPORT:
        case SOC_BLK_XLPORT:
        case SOC_BLK_XLPORTB0:
        case SOC_BLK_CLPORT:
        case SOC_BLK_CLG2PORT:
        case SOC_BLK_CDPORT:
        case SOC_BLK_GXPORT:
        case SOC_BLK_GXFPPORT:
            ctype = SOC_CTR_TYPE_XE;

#ifdef BCM_GREYHOUND_SUPPORT
            if (SOC_IS_GREYHOUND(unit) && blktype == SOC_BLK_GXPORT){
                if (SOC_BLOCK_PORT(unit, blk) < 0){
                    ctype = SOC_CTR_TYPE_XE;
                } else {
                    /* Greyhound GE port with SOC_BLK_GXPORT case*/
                    ctype = SOC_CTR_TYPE_GE;
                }
            }
#endif /* BCM_GREYHOUND_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
            if (SOC_IS_GREYHOUND2(unit) && blktype == SOC_BLK_CLPORT) {
                ctype = SOC_CTR_TYPE_CE;
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
                if (IS_QSGMII_PORT(unit, port) || IS_EGPHY_PORT(unit, port)) {
                    ctype = SOC_CTR_TYPE_GE;
                } else {
                    ctype = SOC_CTR_TYPE_XE;
                }
                if (blktype == SOC_BLK_CLPORT) {
                    ctype = SOC_CTR_TYPE_CE;
                }
                break;
            }
#endif
#if defined(BCM_TRIDENT3_SUPPORT) || defined BCM_TOMAHAWKPLUS_SUPPORT || \
    defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWKPLUS(unit) ||
                SOC_IS_TOMAHAWK2(unit) || SOC_IS_TOMAHAWK3(unit)) {
                if (blktype == SOC_BLK_CLPORT) {
                    ctype = SOC_CTR_TYPE_CE;
                    break;
                }
                if (blktype == SOC_BLK_CDPORT) {
                    ctype = SOC_CTR_TYPE_CD;
                    break;
                }
                if (blktype == SOC_BLK_XLPORT) {
                    ctype = SOC_CTR_TYPE_XE;
                    break;
                }
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit, " UNKNOWN TYPE \n")));
            }
#endif /* BCM_TOMAHAWKPLUS_SUPPORT */
            if (blktype == SOC_BLK_CLG2PORT) {
                ctype = SOC_CTR_TYPE_CE;
            }
            if (SOC_BLOCK_PORT(unit, blk) < 0) {
                ctype = -1;
            }
            break;
#if defined(BCM_MONTEREY_SUPPORT)
        case SOC_BLK_CPRI:
                if(IS_ROE_PORT(unit, port) &&
                            SOC_IS_MONTEREY(unit) ) {
                   ctype = SOC_CTR_TYPE_CPU;
                } else
                {
                ctype = -1;
                }
            break;
#endif
        default:
            ctype = -1;
            break;
        }

        if (ctype != -1) {
            SOC_IF_ERROR_RETURN(soc_port_cmap_set(unit, port, ctype));
            SOC_PBMP_PORT_ADD(soc->counter_pbmp,port);
            break;
        }
    } /* For each block in the given port */

    return SOC_E_NONE;
}

int
soc_counter_port_pbmp_add(int unit, int port)
{
    int rv;

    COUNTER_LOCK(unit);
    rv = _soc_counter_port_pbmp_add(unit, port);
    COUNTER_UNLOCK(unit);
    return rv;
}

int
soc_counter_port_pbmp_remove(int unit, int port)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    COUNTER_LOCK(unit);
    soc->counter_map[port] = 0;
    SOC_PBMP_PORT_REMOVE(soc->counter_pbmp,port);
    COUNTER_UNLOCK(unit);
    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_ser_update_counter
 *
 * Purpose:
 *      Update both sw and corresponding hw counter
 *      (to keep SW and HW counters coherent)
 *
 * Parameters:
 *      unit - SOC unit number
 *
 *      is_reg - nonZero => counter is declared as REG in regsfile
 *             - Zero    => counter is declared as MEM in regsfile
 *
 *      restore_reg - register view that needs to be written
 *                    (for REG based counters)
 *
 *      restore_mem - mem view (hw) that needs to be written
 *                    (for MEM based counters)
 *
 *      index - index for MEM based counter tables
 *            - element_number for REG based counter
 *
 *      copyno - copyno for use with soc_mem_write
 *             - port for use with soc_reg_set
 *
 *      base_reg - base reg view (for REG based counters)
 *
 *      base_mem - base mem view (for MEM based counters)
 *
 *      inst_num - Instance number
 *               - Pool number in case of flex counters
 *               - XPE number in case of mmu counters
 *               - obm queue number in case of obm counters
 *               - 0 otherwise
 *
 *      pipe   - Will be invalid (-1) for MMU_CTR_COLOR_DROP_MEM_XPE0-3
 *             - Will be >= 0 and < NUM_PIPE(unit) otherwise
 *
 *      restore_last - nonZero => write last sw recorded value to hw counter
 *                   - Zero    => clear both sw and hw counter
 *
 * Returns:
 *      SOC_E_???
 *
 * Notes:
 */
int
soc_ser_update_counter(int unit, int is_reg, soc_reg_t restore_reg,
                          soc_mem_t restore_mem, int index, int copyno,
                          soc_reg_t base_reg, soc_mem_t base_mem,
                          int inst_num, int pipe, int restore_last)
{
    void *null_entry = NULL;

    LOG_VERBOSE(BSL_LS_SOC_SER, (BSL_META_U(unit,
        "restore_%s %s, index=%0d, copyno=%0d, "
        "base_%s=%s, inst_num=%0d, pipe=%0d, restore_last=%0d \n"),
        is_reg? "reg" : "mem",
        is_reg? SOC_REG_NAME(unit, restore_reg)
              : SOC_MEM_NAME(unit, restore_mem),
        index, copyno,
        is_reg? "reg" : "mem",
        is_reg? SOC_REG_NAME(unit, base_reg)
              : SOC_MEM_NAME(unit, base_mem),
        inst_num, pipe, restore_last));

    if (is_reg &&
        SOC_REG_IS_VALID(unit, restore_reg) &&
        SOC_REG_IS_VALID(unit, base_reg)) {
        int reg_set_port = copyno;

        /* clear hw reg */
        if (SOC_REG_IS_64(unit, restore_reg)) {
            uint64 tmp64;
            COMPILER_64_ZERO(tmp64);
            LOG_WARN(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "Clearing restore_reg %s, 64b, pipe=%0d, numels=%0d\n"),
                 SOC_REG_NAME(unit, restore_reg), pipe, index));
            SOC_IF_ERROR_RETURN(
                soc_reg_set(unit, restore_reg, reg_set_port, index, tmp64));
        } else {
            LOG_WARN(BSL_LS_SOC_SER,
                (BSL_META_U(unit,
                            "Clearing restore_reg %s, 32b, pipe=%0d, numels=%0d\n"),
                 SOC_REG_NAME(unit, restore_reg), pipe, index));
            SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, restore_reg, reg_set_port, index, 0));
        }
        return SOC_E_NONE;
    } else if (!is_reg &&
        SOC_MEM_IS_VALID(unit, restore_mem) &&
        SOC_MEM_IS_VALID(unit, base_mem)) {

        /* clear hw mem */
        null_entry = soc_mem_entry_null(unit, restore_mem);
        return(soc_mem_write(unit, restore_mem, copyno, index, null_entry));
    }

    return SOC_E_FAIL;
}

#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) */
