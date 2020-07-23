/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ipmc.c
 * Purpose:     Tracks and manages IPMC tables.
 */

#ifdef INCLUDE_L3
#include <shared/bsl.h>
#include <soc/l3x.h>
#include <soc/bradley.h>

#include <bcm/error.h>
#include <bcm/ipmc.h>
#include <bcm/field.h>
#include <bcm/cosq.h>
#include <include/bcm/field.h>

#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/bradley.h>
#ifdef BCM_KATANA_SUPPORT
#include <bcm_int/esw/katana.h>
#endif
#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/subport.h>
#endif /* BCM_KATANA2_SUPPORT */
#include <bcm_int/esw_dispatch.h>

#if defined(BCM_KATANA2_SUPPORT)
#define REPL_PORT_CHECK(unit, port) \
{ \
    if (SOC_IS_KATANA2(unit)) { \
        if (!IS_PORT(unit, port) && \
            !(port >= SOC_INFO(unit).pp_port_index_min) && \
            !(port <= SOC_INFO(unit).pp_port_index_max)) { \
            return BCM_E_PARAM; \
        } \
    } else { \
        if (!IS_PORT(unit, port)) { \
            return BCM_E_PARAM; \
        } \
    } \
}
#else
#define REPL_PORT_CHECK(unit, port) \
    if (!IS_PORT(unit, port)) { \
        return BCM_E_PARAM; \
    }
#endif

#define _BCM_QOS_MAP_TYPE_MASK           0x3ff
#define _BCM_QOS_MAP_SHIFT                  10

typedef struct _tr2_repl_port_info_s {
    int32 *vlan_count;                        /* # VLANs the port repl to */
} _tr2_repl_port_info_t;

typedef struct _tr2_repl_info_s {
    int ipmc_size;
    uint32 intf_num;              /* Number of interfaces on this device */
    uint16 ipmc_vlan_total;       /* Keep track of total and */
    uint32 *bitmap_entries_used;  /* free entries of IPMC_VLAN table */
    _bcm_repl_list_info_t *repl_list_info;
    _tr2_repl_port_info_t *port_info[SOC_MAX_NUM_PORTS];
} _tr2_repl_info_t;

static _tr2_repl_info_t *_tr2_repl_info[BCM_MAX_NUM_UNITS];

#define TD_EGR_L3_NEXT_HOP_OFFSET 8192

#ifdef BCM_KATANA_SUPPORT

typedef struct _kt_repl_queue_info_s {
    int32 *queue_count;           /* # VLANs the queue repl to */
    int32 *queue_count_int;       /* # Count of internally buffered queues */
    int32 *queue_count_ext;       /* # Count of externally buffered queues */
} _kt_repl_queue_info_t;

typedef struct _kt_extq_repl_info_s {
    int ipmc_size;
    uint32 queue_num;
    uint32 extq_list_total;
    uint32 extq_repl_max;
    uint32 *bitmap_entries_used; /* free entries of
                                     MMU_EXT_MC_QUEUE_LIST  table */
    _bcm_repl_list_info_t *repl_extq_list_info;
    _kt_repl_queue_info_t *mcgroup_info;
#if (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    _kt_repl_queue_info_t *port_info[SOC_MAX_NUM_PP_PORTS];
#else
    _kt_repl_queue_info_t *port_info[SOC_MAX_NUM_PORTS];
#endif
} _kt_extq_repl_info_t;

static _kt_extq_repl_info_t *_kt_extq_repl_info[BCM_MAX_NUM_UNITS];

#endif

#define IPMC_REPL_LOCK(_u_)                    \
    if (SOC_IS_KATANA2(unit)) {                \
        soc_mem_lock(_u_, MMU_REPL_LIST_TBLm); \
    } else {                                   \
        soc_mem_lock(_u_, MMU_IPMC_VLAN_TBLm); \
    } 
#define IPMC_REPL_UNLOCK(_u_)                    \
    if (SOC_IS_KATANA2(unit)) {                  \
        soc_mem_unlock(_u_, MMU_REPL_LIST_TBLm); \
    } else {                                     \
        soc_mem_unlock(_u_, MMU_IPMC_VLAN_TBLm); \
    }
#define IPMC_REPL_ID(_u_, _id_) \
        if ((_id_ < 0) || (_id_ >= _tr2_repl_info[_u_]->ipmc_size)) \
            { return BCM_E_PARAM; }
#define IPMC_REPL_INIT(unit) \
        if (_tr2_repl_info[unit] == NULL) { return BCM_E_INIT; }
#define IPMC_REPL_TOTAL(_u_) \
        _tr2_repl_info[_u_]->ipmc_vlan_total
#define IPMC_REPL_INTF_TOTAL(_u_) \
        _tr2_repl_info[_u_]->intf_num
#define IPMC_REPL_VE_USED_GET(_u_, _i_) \
        SHR_BITGET(_tr2_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_REPL_VE_USED_SET(_u_, _i_) \
        SHR_BITSET(_tr2_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_REPL_VE_USED_CLR(_u_, _i_) \
        SHR_BITCLR(_tr2_repl_info[_u_]->bitmap_entries_used, _i_)

#define IPMC_REPL_LIST_INFO(_u_) \
	_tr2_repl_info[_u_]->repl_list_info
#define IPMC_REPL_PORT_INFO(_u_, _p_) \
        _tr2_repl_info[_u_]->port_info[_p_]
#define IPMC_REPL_PORT_VLAN_COUNT(_u_, _p_, _ipmc_id_) \
        _tr2_repl_info[_u_]->port_info[_p_]->vlan_count[_ipmc_id_]

#ifdef BCM_KATANA_SUPPORT
#define IPMC_EXTQ_ID(_u_, _id_) \
        if ((_id_ < 0) || (_id_ >= _kt_extq_repl_info[_u_]->queue_num)) \
            { return BCM_E_PARAM; }
#define IPMC_EXTQ_REPL_INIT(unit) \
        if (_kt_extq_repl_info[unit] == NULL) { return BCM_E_INIT; }
#define IPMC_EXTQ_LIST_TOTAL(_u_) \
        _kt_extq_repl_info[_u_]->extq_list_total
#define IPMC_EXTQ_REPL_MAX(_u_) \
        _kt_extq_repl_info[_u_]->extq_repl_max
#define IPMC_EXTQ_TOTAL(_u_) \
        _kt_extq_repl_info[_u_]->queue_num
#define IPMC_EXTQ_REPL_VE_USED_GET(_u_, _i_) \
        SHR_BITGET(_kt_extq_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_EXTQ_REPL_VE_USED_SET(_u_, _i_) \
        SHR_BITSET(_kt_extq_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_EXTQ_REPL_VE_USED_CLR(_u_, _i_) \
        SHR_BITCLR(_kt_extq_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_EXTQ_REPL_LIST_INFO(_u_) \
    _kt_extq_repl_info[_u_]->repl_extq_list_info
#define IPMC_EXTQ_GROUP_INFO(_u_) 		\
	_kt_extq_repl_info[_u_]->mcgroup_info
#define IPMC_EXTQ_REPL_QUEUE_COUNT(_u_, _ipmc_id_) \
        _kt_extq_repl_info[_u_]->mcgroup_info->queue_count[_ipmc_id_]
#define IPMC_EXTQ_REPL_QUEUE_COUNT_INT(_u_, _ipmc_id_) \
        _kt_extq_repl_info[_u_]->mcgroup_info->queue_count_int[_ipmc_id_]
#define IPMC_EXTQ_REPL_QUEUE_COUNT_EXT(_u_, _ipmc_id_) \
        _kt_extq_repl_info[_u_]->mcgroup_info->queue_count_ext[_ipmc_id_]
#define IPMC_EXTQ_REPL_PORT_INFO(_u_, _p_) \
        _kt_extq_repl_info[_u_]->port_info[_p_]
#define IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(_u_, _p_, _ipmc_id_) \
        _kt_extq_repl_info[_u_]->port_info[_p_]->queue_count[_ipmc_id_]

#endif

uint8 war_pkt[] = {
/*
 00    01    02    03    04    05    06    07    08    09    10    11    12    13    14    15*/
 0x00, 0x00, 0x5E, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xAA, 0xBB, 0xCC, 0x81, 0x00, 0x00, 0x01,
 0x08, 0x00, 0x45, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x00, 0x40, 0xFF, 0x79, 0xD6, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x00, 0x01, 0x02,
 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0f, 0x10, 0x11, 0x12,
 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x00, 0x01, 0x02

};

typedef struct _rep_regs_s {
    soc_field_t ptr_type;
    soc_field_t port_ptr;
    soc_field_t last_ptr;
    soc_mem_t   mem;
}_rep_regs_t;

_rep_regs_t ch_rep_regs[55] = { {INVALIDf,  INVALIDf,       INVALIDf,    INVALIDm},
                                {INVALIDf,  PORT1_1STPTRf,  PORT1_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {INVALIDf,  PORT2_1STPTRf,  PORT2_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {INVALIDf,  PORT3_1STPTRf,  PORT3_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {INVALIDf,  PORT4_1STPTRf,  PORT4_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {INVALIDf,  PORT5_1STPTRf,  PORT5_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {INVALIDf,  PORT6_1STPTRf,  PORT6_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {INVALIDf,  PORT7_1STPTRf,  PORT7_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {INVALIDf,  PORT8_1STPTRf,  PORT8_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {INVALIDf,  PORT9_1STPTRf,  PORT9_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {INVALIDf,  PORT10_1STPTRf,  PORT10_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {INVALIDf,  PORT11_1STPTRf,  PORT11_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {INVALIDf,  PORT12_1STPTRf,  PORT12_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {INVALIDf,  PORT13_1STPTRf,  PORT13_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {INVALIDf,  PORT14_1STPTRf,  PORT14_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {INVALIDf,  PORT15_1STPTRf,  PORT15_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {INVALIDf,  PORT16_1STPTRf,  PORT16_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {INVALIDf,  PORT17_1STPTRf,  PORT17_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {INVALIDf,  PORT18_1STPTRf,  PORT18_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {INVALIDf,  PORT19_1STPTRf,  PORT19_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {INVALIDf,  PORT20_1STPTRf,  PORT20_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {INVALIDf,  PORT21_1STPTRf,  PORT21_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {INVALIDf,  PORT22_1STPTRf,  PORT22_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {INVALIDf,  PORT23_1STPTRf,  PORT23_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {INVALIDf,  PORT24_1STPTRf,  PORT24_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {INVALIDf,  PORT25_1STPTRf,  PORT25_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {INVALIDf,  PORT26_1STPTRf,  PORT26_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {INVALIDf,  PORT27_1STPTRf,  PORT27_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {INVALIDf,  PORT28_1STPTRf,  PORT28_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {INVALIDf,  PORT29_1STPTRf,  PORT29_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {INVALIDf,  PORT30_1STPTRf,  PORT30_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {INVALIDf,  PORT31_1STPTRf,  PORT31_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {INVALIDf,  PORT32_1STPTRf,  PORT32_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {INVALIDf,  PORT33_1STPTRf,  PORT33_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {INVALIDf,  PORT34_1STPTRf,  PORT34_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {INVALIDf,  PORT35_1STPTRf,  PORT35_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {INVALIDf,  PORT36_1STPTRf,  PORT36_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {INVALIDf,  PORT37_1STPTRf,  PORT37_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {INVALIDf,  PORT38_1STPTRf,  PORT38_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {INVALIDf,  PORT39_1STPTRf,  PORT39_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {INVALIDf,  PORT40_1STPTRf,  PORT40_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {INVALIDf,  PORT41_1STPTRf,  PORT41_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {INVALIDf,  PORT42_1STPTRf,  PORT42_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {INVALIDf,  PORT43_1STPTRf,  PORT43_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {INVALIDf,  PORT44_1STPTRf,  PORT44_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {INVALIDf,  PORT45_1STPTRf,  PORT45_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {INVALIDf,  PORT46_1STPTRf,  PORT46_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {INVALIDf,  PORT47_1STPTRf,  PORT47_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {INVALIDf,  PORT48_1STPTRf,  PORT48_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {INVALIDf,  PORT49_1STPTRf,  PORT49_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {INVALIDf,  PORT50_1STPTRf,  PORT50_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {INVALIDf,  PORT51_1STPTRf,  PORT51_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {INVALIDf,  PORT52_1STPTRf,  PORT52_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {INVALIDf,  PORT53_1STPTRf,  PORT53_LASTf, MMU_IPMC_GROUP_TBL8m},
};

#ifdef BCM_TRIDENT_SUPPORT
static soc_mem_t _bcm_ipmc_trident_rep_regs[] = {
    INVALIDm,              MMU_IPMC_GROUP_TBL1m,
    MMU_IPMC_GROUP_TBL2m,  MMU_IPMC_GROUP_TBL3m,
    MMU_IPMC_GROUP_TBL4m,  MMU_IPMC_GROUP_TBL5m,
    MMU_IPMC_GROUP_TBL6m,  MMU_IPMC_GROUP_TBL7m,
    MMU_IPMC_GROUP_TBL8m,  MMU_IPMC_GROUP_TBL9m,
    MMU_IPMC_GROUP_TBL10m, MMU_IPMC_GROUP_TBL11m,
    MMU_IPMC_GROUP_TBL12m, MMU_IPMC_GROUP_TBL13m,
    MMU_IPMC_GROUP_TBL14m, MMU_IPMC_GROUP_TBL15m,
    MMU_IPMC_GROUP_TBL16m, MMU_IPMC_GROUP_TBL17m,
    MMU_IPMC_GROUP_TBL18m, MMU_IPMC_GROUP_TBL19m,
    MMU_IPMC_GROUP_TBL20m, MMU_IPMC_GROUP_TBL21m,
    MMU_IPMC_GROUP_TBL22m, MMU_IPMC_GROUP_TBL23m,
    MMU_IPMC_GROUP_TBL24m, MMU_IPMC_GROUP_TBL25m,
    MMU_IPMC_GROUP_TBL26m, MMU_IPMC_GROUP_TBL27m,
    MMU_IPMC_GROUP_TBL28m, MMU_IPMC_GROUP_TBL29m,
    MMU_IPMC_GROUP_TBL30m, MMU_IPMC_GROUP_TBL31m,
    MMU_IPMC_GROUP_TBL32m, MMU_IPMC_GROUP_TBL33m,
    MMU_IPMC_GROUP_TBL34m, MMU_IPMC_GROUP_TBL35m,
    MMU_IPMC_GROUP_TBL36m, MMU_IPMC_GROUP_TBL37m,
    MMU_IPMC_GROUP_TBL38m, MMU_IPMC_GROUP_TBL39m,
    MMU_IPMC_GROUP_TBL40m, MMU_IPMC_GROUP_TBL41m,
    MMU_IPMC_GROUP_TBL42m, MMU_IPMC_GROUP_TBL43m,
    MMU_IPMC_GROUP_TBL44m, MMU_IPMC_GROUP_TBL45m,
    MMU_IPMC_GROUP_TBL46m, MMU_IPMC_GROUP_TBL47m,
    MMU_IPMC_GROUP_TBL48m, MMU_IPMC_GROUP_TBL49m,
    MMU_IPMC_GROUP_TBL50m, MMU_IPMC_GROUP_TBL51m,
    MMU_IPMC_GROUP_TBL52m, MMU_IPMC_GROUP_TBL53m,
    MMU_IPMC_GROUP_TBL54m, MMU_IPMC_GROUP_TBL55m,
    MMU_IPMC_GROUP_TBL56m, MMU_IPMC_GROUP_TBL57m,
    MMU_IPMC_GROUP_TBL58m, MMU_IPMC_GROUP_TBL59m,
    MMU_IPMC_GROUP_TBL60m, MMU_IPMC_GROUP_TBL61m,
    MMU_IPMC_GROUP_TBL62m, MMU_IPMC_GROUP_TBL63m,
    MMU_IPMC_GROUP_TBL64m, MMU_IPMC_GROUP_TBL65m
};
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
_rep_regs_t kt_rqe_rep_regs[] = { 
    {INVALIDf,         INVALIDf,       INVALIDf,    INVALIDm},
    {PORT1_PTR_TYPEf,  PORT1_1STPTRf,  PORT1_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT2_PTR_TYPEf,  PORT2_1STPTRf,  PORT2_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT3_PTR_TYPEf,  PORT3_1STPTRf,  PORT3_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT4_PTR_TYPEf,  PORT4_1STPTRf,  PORT4_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT5_PTR_TYPEf,  PORT5_1STPTRf,  PORT5_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT6_PTR_TYPEf,  PORT6_1STPTRf,  PORT6_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT7_PTR_TYPEf,  PORT7_1STPTRf,  PORT7_LASTf, MMU_IPMC_GROUP_TBL0m},
    {PORT8_PTR_TYPEf,  PORT8_1STPTRf,  PORT8_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT9_PTR_TYPEf,  PORT9_1STPTRf,  PORT9_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT10_PTR_TYPEf,  PORT10_1STPTRf,  PORT10_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT11_PTR_TYPEf,  PORT11_1STPTRf,  PORT11_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT12_PTR_TYPEf,  PORT12_1STPTRf,  PORT12_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT13_PTR_TYPEf,  PORT13_1STPTRf,  PORT13_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT14_PTR_TYPEf,  PORT14_1STPTRf,  PORT14_LASTf, MMU_IPMC_GROUP_TBL1m},
    {PORT15_PTR_TYPEf,  PORT15_1STPTRf,  PORT15_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT16_PTR_TYPEf,  PORT16_1STPTRf,  PORT16_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT17_PTR_TYPEf,  PORT17_1STPTRf,  PORT17_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT18_PTR_TYPEf,  PORT18_1STPTRf,  PORT18_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT19_PTR_TYPEf,  PORT19_1STPTRf,  PORT19_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT20_PTR_TYPEf,  PORT20_1STPTRf,  PORT20_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT21_PTR_TYPEf,  PORT21_1STPTRf,  PORT21_LASTf, MMU_IPMC_GROUP_TBL2m},
    {PORT22_PTR_TYPEf,  PORT22_1STPTRf,  PORT22_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT23_PTR_TYPEf,  PORT23_1STPTRf,  PORT23_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT24_PTR_TYPEf,  PORT24_1STPTRf,  PORT24_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT25_PTR_TYPEf,  PORT25_1STPTRf,  PORT25_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT26_PTR_TYPEf,  PORT26_1STPTRf,  PORT26_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT27_PTR_TYPEf,  PORT27_1STPTRf,  PORT27_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT28_PTR_TYPEf,  PORT28_1STPTRf,  PORT28_LASTf, MMU_IPMC_GROUP_TBL3m},
    {PORT29_PTR_TYPEf,  PORT29_1STPTRf,  PORT29_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT30_PTR_TYPEf,  PORT30_1STPTRf,  PORT30_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT31_PTR_TYPEf,  PORT31_1STPTRf,  PORT31_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT32_PTR_TYPEf,  PORT32_1STPTRf,  PORT32_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT33_PTR_TYPEf,  PORT33_1STPTRf,  PORT33_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT34_PTR_TYPEf,  PORT34_1STPTRf,  PORT34_LASTf, MMU_IPMC_GROUP_TBL4m},
    {PORT35_PTR_TYPEf,  PORT35_1STPTRf,  PORT35_LASTf, MMU_IPMC_GROUP_TBL4m},
};

_rep_regs_t kt_toq_rep_regs[] = { 
    {INVALIDf,         INVALIDf,       INVALIDf,    INVALIDm},
    {PORT1_PTR_TYPEf,  PORT1_1STPTRf,  PORT1_LASTf, MMU_TOQ_IPMC_GROUP_TBL0m},
    {PORT2_PTR_TYPEf,  PORT2_1STPTRf,  PORT2_LASTf, MMU_TOQ_IPMC_GROUP_TBL0m},
    {PORT3_PTR_TYPEf,  PORT3_1STPTRf,  PORT3_LASTf, MMU_TOQ_IPMC_GROUP_TBL0m},
    {PORT4_PTR_TYPEf,  PORT4_1STPTRf,  PORT4_LASTf, MMU_TOQ_IPMC_GROUP_TBL0m},
    {PORT5_PTR_TYPEf,  PORT5_1STPTRf,  PORT5_LASTf, MMU_TOQ_IPMC_GROUP_TBL0m},
    {PORT6_PTR_TYPEf,  PORT6_1STPTRf,  PORT6_LASTf, MMU_TOQ_IPMC_GROUP_TBL1m},
    {PORT7_PTR_TYPEf,  PORT7_1STPTRf,  PORT7_LASTf, MMU_TOQ_IPMC_GROUP_TBL1m},
    {PORT8_PTR_TYPEf,  PORT8_1STPTRf,  PORT8_LASTf, MMU_TOQ_IPMC_GROUP_TBL1m},
    {PORT9_PTR_TYPEf,  PORT9_1STPTRf,  PORT9_LASTf, MMU_TOQ_IPMC_GROUP_TBL1m},
    {PORT10_PTR_TYPEf,  PORT10_1STPTRf,  PORT10_LASTf, MMU_TOQ_IPMC_GROUP_TBL1m},
    {PORT11_PTR_TYPEf,  PORT11_1STPTRf,  PORT11_LASTf, MMU_TOQ_IPMC_GROUP_TBL2m},
    {PORT12_PTR_TYPEf,  PORT12_1STPTRf,  PORT12_LASTf, MMU_TOQ_IPMC_GROUP_TBL2m},
    {PORT13_PTR_TYPEf,  PORT13_1STPTRf,  PORT13_LASTf, MMU_TOQ_IPMC_GROUP_TBL2m},
    {PORT14_PTR_TYPEf,  PORT14_1STPTRf,  PORT14_LASTf, MMU_TOQ_IPMC_GROUP_TBL2m},
    {PORT15_PTR_TYPEf,  PORT15_1STPTRf,  PORT15_LASTf, MMU_TOQ_IPMC_GROUP_TBL2m},
    {PORT16_PTR_TYPEf,  PORT16_1STPTRf,  PORT16_LASTf, MMU_TOQ_IPMC_GROUP_TBL3m},
    {PORT17_PTR_TYPEf,  PORT17_1STPTRf,  PORT17_LASTf, MMU_TOQ_IPMC_GROUP_TBL3m},
    {PORT18_PTR_TYPEf,  PORT18_1STPTRf,  PORT18_LASTf, MMU_TOQ_IPMC_GROUP_TBL3m},
    {PORT19_PTR_TYPEf,  PORT19_1STPTRf,  PORT19_LASTf, MMU_TOQ_IPMC_GROUP_TBL3m},
    {PORT20_PTR_TYPEf,  PORT20_1STPTRf,  PORT20_LASTf, MMU_TOQ_IPMC_GROUP_TBL3m},
    {PORT21_PTR_TYPEf,  PORT21_1STPTRf,  PORT21_LASTf, MMU_TOQ_IPMC_GROUP_TBL4m},
    {PORT22_PTR_TYPEf,  PORT22_1STPTRf,  PORT22_LASTf, MMU_TOQ_IPMC_GROUP_TBL4m},
    {PORT23_PTR_TYPEf,  PORT23_1STPTRf,  PORT23_LASTf, MMU_TOQ_IPMC_GROUP_TBL4m},
    {PORT24_PTR_TYPEf,  PORT24_1STPTRf,  PORT24_LASTf, MMU_TOQ_IPMC_GROUP_TBL4m},
    {PORT25_PTR_TYPEf,  PORT25_1STPTRf,  PORT25_LASTf, MMU_TOQ_IPMC_GROUP_TBL4m},
    {PORT26_PTR_TYPEf,  PORT26_1STPTRf,  PORT26_LASTf, MMU_TOQ_IPMC_GROUP_TBL5m},
    {PORT27_PTR_TYPEf,  PORT27_1STPTRf,  PORT27_LASTf, MMU_TOQ_IPMC_GROUP_TBL5m},
    {PORT28_PTR_TYPEf,  PORT28_1STPTRf,  PORT28_LASTf, MMU_TOQ_IPMC_GROUP_TBL5m},
    {PORT29_PTR_TYPEf,  PORT29_1STPTRf,  PORT29_LASTf, MMU_TOQ_IPMC_GROUP_TBL5m},
    {PORT30_PTR_TYPEf,  PORT30_1STPTRf,  PORT30_LASTf, MMU_TOQ_IPMC_GROUP_TBL5m},
    {PORT31_PTR_TYPEf,  PORT31_1STPTRf,  PORT31_LASTf, MMU_TOQ_IPMC_GROUP_TBL6m},
    {PORT32_PTR_TYPEf,  PORT32_1STPTRf,  PORT32_LASTf, MMU_TOQ_IPMC_GROUP_TBL6m},
    {PORT33_PTR_TYPEf,  PORT33_1STPTRf,  PORT33_LASTf, MMU_TOQ_IPMC_GROUP_TBL6m},
    {PORT34_PTR_TYPEf,  PORT34_1STPTRf,  PORT34_LASTf, MMU_TOQ_IPMC_GROUP_TBL6m},
    {PORT35_PTR_TYPEf,  PORT35_1STPTRf,  PORT35_LASTf, MMU_TOQ_IPMC_GROUP_TBL6m},
};

#ifdef BCM_KATANA_SUPPORT
uint32 *sw_to_hw_queue[BCM_MAX_NUM_UNITS];
#endif

STATIC int
_kt_ipmc_vlan_ptr(int unit,int mc_index,
                  bcm_port_t port, int *vptr, int last, int set)
{
    soc_field_t  port_ptr = INVALIDf;
    soc_field_t  last_ptr = INVALIDf;
    soc_field_t  ptr_type = INVALIDf;
    soc_mem_t    rqe_mem = INVALIDm;
    soc_mem_t    toq_mem = INVALIDm;
    mmu_ipmc_group_tbl0_entry_t rqe_entry;
    mmu_toq_ipmc_group_tbl0_entry_t toq_entry;
    int rv;
    _rep_regs_t     *rqe_arr_ptr;
    _rep_regs_t     *toq_arr_ptr;
    int ipmc_ptr;
    int ipmc_id = 0;

    rqe_arr_ptr = kt_rqe_rep_regs;
    rqe_mem = rqe_arr_ptr[port].mem;
    toq_arr_ptr = kt_toq_rep_regs;
    toq_mem = toq_arr_ptr[port].mem;    
    
    port_ptr = rqe_arr_ptr[port].port_ptr;
    last_ptr = rqe_arr_ptr[port].last_ptr;
    ptr_type = rqe_arr_ptr[port].ptr_type;

    if ((INVALIDm == rqe_mem) || (INVALIDf == port_ptr)){
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
    (_bcm_kt_ipmc_mmu_mc_remap_ptr(unit, mc_index, &ipmc_id, FALSE));

    soc_mem_lock(unit, rqe_mem);

    if ((rv = soc_mem_read(unit, rqe_mem, MEM_BLOCK_ALL, 
                           ipmc_id, &rqe_entry)) < 0) {
        soc_mem_unlock(unit, rqe_mem);
        return rv;
    }

    if (!SOC_MEM_IS_VALID(unit, toq_mem)) {
        if (set) {
            soc_mem_field32_set(unit, rqe_mem, &rqe_entry, port_ptr, *vptr);
            soc_mem_field32_set(unit, rqe_mem, &rqe_entry, last_ptr,last ? 1 : 0);
            if (INVALIDf != ptr_type) {
                soc_mem_field32_set(unit, rqe_mem, &rqe_entry, ptr_type, 0);
            }
            if ((rv = soc_mem_write(unit, rqe_mem, MEM_BLOCK_ALL, ipmc_id, 
                                    &rqe_entry)) < 0) {
                soc_mem_unlock(unit, rqe_mem);
                return rv;
            }
        } else {
            *vptr =  soc_mem_field32_get(unit, rqe_mem, &rqe_entry, port_ptr);
        }
    } else {
        if ((rv = soc_mem_read(unit, toq_mem, MEM_BLOCK_ALL, 
                               ipmc_id, &toq_entry)) < 0) {
            soc_mem_unlock(unit, rqe_mem);
            return rv;
        }
        
        if (set) {
            /* encode port_ptr as { 2'b01, MC_INDEX[11:0] } */
            ipmc_ptr = (ipmc_id & 0xFFF) | (0x1 << 12);
            soc_mem_field32_set(unit, rqe_mem, &rqe_entry, port_ptr, ipmc_ptr);
            soc_mem_field32_set(unit, rqe_mem, &rqe_entry, last_ptr, last ? 1 : 0);
            if (INVALIDf != ptr_type) {
                soc_mem_field32_set(unit, rqe_mem, &rqe_entry, ptr_type, 0);
            }
            port_ptr = toq_arr_ptr[port].port_ptr;
            last_ptr = toq_arr_ptr[port].last_ptr;
            ptr_type = toq_arr_ptr[port].ptr_type;
            soc_mem_field32_set(unit, toq_mem, &toq_entry, port_ptr, *vptr);
            if (!soc_feature(unit, soc_feature_ipmc_reduced_table_size)) {
                soc_mem_field32_set(unit, toq_mem, &toq_entry, last_ptr, 
                                    last ? 1 : 0);
            } else {
                soc_mem_field32_set(unit, toq_mem, &toq_entry, last_ptr, 0);
            }
            if (INVALIDf != ptr_type) {
                soc_mem_field32_set(unit, toq_mem, &toq_entry, ptr_type, 0);
            }    
            if ((rv = soc_mem_write(unit, toq_mem, MEM_BLOCK_ALL, ipmc_id, 
                                    &toq_entry)) < 0) {
                soc_mem_unlock(unit, rqe_mem);
                return rv;
            }
            if (soc_feature(unit, soc_feature_ipmc_reduced_table_size)) {
                /* set port ptr = start vlan_ptr at index of start vlan ptr */
                if ((rv = soc_mem_read(unit, toq_mem, MEM_BLOCK_ALL, 
                                       *vptr, &toq_entry)) < 0) {
                    soc_mem_unlock(unit, rqe_mem);
                    return rv;
                }	
                port_ptr = toq_arr_ptr[port].port_ptr;
                soc_mem_field32_set(unit, toq_mem, &toq_entry, port_ptr, *vptr);
                if ((rv = soc_mem_write(unit, toq_mem, MEM_BLOCK_ALL, *vptr, 
                                        &toq_entry)) < 0) {
                    soc_mem_unlock(unit, rqe_mem);
                    return rv;
                }
            }
            if ((rv = soc_mem_write(unit, rqe_mem, MEM_BLOCK_ALL, ipmc_id, 
                                    &rqe_entry)) < 0) {
                soc_mem_unlock(unit, rqe_mem);
                return rv;
            }
        } else {
            port_ptr = toq_arr_ptr[port].port_ptr;
            *vptr =  soc_mem_field32_get(unit, toq_mem, &toq_entry, port_ptr);
        }    
    }
    soc_mem_unlock(unit, rqe_mem);

    return BCM_E_NONE;
}

STATIC int
_kt_ipmc_group_entry_clear(int unit, int start_ptr)
{
    soc_mem_t    toq_mem = INVALIDm;
    mmu_toq_ipmc_group_tbl0_entry_t toq_entry;
    int rv;
    _rep_regs_t     *toq_arr_ptr;
    bcm_port_t port;

    toq_arr_ptr = kt_toq_rep_regs;
    sal_memset(&toq_entry, 0, sizeof(toq_entry));
	
    for (port = 0; port < 36; port++) {
        if (toq_mem != toq_arr_ptr[port].mem) {
            toq_mem = toq_arr_ptr[port].mem;
            soc_mem_lock(unit, toq_mem);
            if ((rv = soc_mem_write(unit, toq_mem, MEM_BLOCK_ALL, start_ptr, 
                                    &toq_entry)) < 0) {
                soc_mem_unlock(unit, toq_mem);
                return rv;
            }
            soc_mem_unlock(unit, toq_mem);
        }
    }

    return BCM_E_NONE;
}

#endif

STATIC int
_tr2_ipmc_vlan_ptr(int unit,int ipmc_id,
                  bcm_port_t port, int *vptr, int last, int set)
{
    soc_field_t  port_ptr = INVALIDf;
    soc_field_t  last_ptr = INVALIDf;
    soc_mem_t    mem = INVALIDm;
    mmu_ipmc_group_tbl0_entry_t gentry;
    int rv;
    _rep_regs_t     *rep_arr_ptr;

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit)) {
        soc_info_t *si;
        bcm_port_t phy_port, mmu_port;

        /* Convert logical port to physical port to mmu port */
        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        mem = _bcm_ipmc_trident_rep_regs[mmu_port];
        port_ptr = PORT_1STPTRf;
        last_ptr = PORT_LASTf;
    } else 
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        return _kt_ipmc_vlan_ptr(unit, ipmc_id, port,
                                 vptr, last, set);
    } else 
#endif /* BCM_KATANA_SUPPORT */
    {
        rep_arr_ptr = ch_rep_regs;
        mem = rep_arr_ptr[port].mem;
        port_ptr = rep_arr_ptr[port].port_ptr;
        last_ptr = rep_arr_ptr[port].last_ptr;
    }

    if ((INVALIDm == mem) || (INVALIDf == port_ptr)){
        return BCM_E_PARAM;
    }

    soc_mem_lock(unit, mem);
    if ((rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, 
                           ipmc_id, &gentry)) < 0) {
        soc_mem_unlock(unit, mem);
        return rv;
    }
    if (set) {
        soc_mem_field32_set(unit, mem, &gentry, port_ptr, *vptr);
        soc_mem_field32_set(unit, mem, &gentry, last_ptr,last ? 1 : 0);
        if ((rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, ipmc_id, 
                                &gentry)) < 0) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
    } else {
        *vptr =  soc_mem_field32_get(unit, mem, &gentry, port_ptr);
    }
    soc_mem_unlock(unit, mem);

    return BCM_E_NONE;
}


#ifdef BCM_KATANA_SUPPORT

STATIC int
_kt_extq_mc_group_ptr(int unit,int ipmc_id,
                  int *extq_ptr, int last, int set)
{
    int         rv = BCM_E_NONE;
    mmu_ext_mc_group_map_entry_t mc_group_entry;
    soc_mem_t mem = MMU_EXT_MC_GROUP_MAPm;
    int remap_id;

    soc_mem_lock(unit, mem);
    if ((rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                          ipmc_id, &mc_group_entry)) < 0) {
       soc_mem_unlock(unit, mem);
       return rv;
    }
    
    if (set) {
       soc_mem_field32_set(unit, mem, &mc_group_entry, 
                           EXT_Q_FIRST_PTRf, *extq_ptr);
       soc_mem_field32_set(unit, mem, &mc_group_entry, 
                           EXT_Q_LAST_LASTf, last ? 1 : 0);

       if ((rv = _bcm_kt_ipmc_mmu_mc_remap_ptr(unit, 0, 
                       &remap_id, FALSE)) < 0) {
            soc_mem_unlock(unit, mem);
            return rv;                
       }
       /* update the new chain in the remapped id */
       if ((rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, remap_id,
                                &mc_group_entry)) < 0) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
       
    } else {
        *extq_ptr = soc_mem_field32_get(unit, mem, &mc_group_entry, 
                                           EXT_Q_FIRST_PTRf);
    }
    soc_mem_unlock(unit, mem);
    
    return rv;
}

/*
 * Function:
 *    _bcm_kt_extq_repl_list_compare
 * Description:
 *    Compare HW list starting at extq_index to the queue list contained
 *      in vlan_vec.
 */

STATIC int
_bcm_kt_extq_repl_list_compare(int unit, int extq_ptr,
                               SHR_BITDCL *q_vec, 
                               uint32 *q_repl_ptr)
{
    int             rv = BCM_E_NONE;
    int             q_num, q_max, ptr_alloc_size;
    int             vec_alloc_size;
    uint32          *curr_repl_ptr = NULL;
    SHR_BITDCL      *curr_vec = NULL;

    q_max = IPMC_EXTQ_TOTAL(unit);
    ptr_alloc_size = q_max * sizeof(int);
    curr_repl_ptr = sal_alloc(ptr_alloc_size, "IPMC repl interface array");
    if (curr_repl_ptr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(curr_repl_ptr, 0, ptr_alloc_size);

    vec_alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_TOTAL(unit));
    curr_vec = sal_alloc(vec_alloc_size, "IPMC extq replication vector");
    if (curr_vec == NULL) {
        sal_free(curr_repl_ptr);
        return BCM_E_MEMORY;
    }
    sal_memset(curr_vec, 0, vec_alloc_size);    

    if (extq_ptr != 0) {
        rv = _bcm_kt_get_subscriber_queue_entries(unit, extq_ptr, curr_vec, 
                                                  curr_repl_ptr, &q_num);
    }

    if (sal_memcmp(q_vec, curr_vec, vec_alloc_size) != 0) {
        rv = BCM_E_NOT_FOUND;
        goto list_compare_done;
    }

    if (sal_memcmp(q_repl_ptr, curr_repl_ptr, ptr_alloc_size) != 0) {
        rv = BCM_E_NOT_FOUND;
        goto list_compare_done;
    }

list_compare_done:
    sal_free(curr_repl_ptr);
    sal_free(curr_vec);
    return rv;
}

#endif

#ifdef BCM_WARM_BOOT_SUPPORT

#ifdef BCM_KATANA_SUPPORT

STATIC int
_bcm_kt_extq_repl_list_get(int unit, int extq_ptr,
                           int *list_ptr, 
                           int *list_num)
{
    int     rv = BCM_E_NONE;
    int     next_ptr1, next_ptr2;
    mmu_ext_mc_queue_list4_entry_t     qlist4_entry;
    
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                    extq_ptr, &qlist4_entry)) < 0) {
        goto repl_list_get_done;
    }

    next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR1f);
    next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR2f);
    IPMC_EXTQ_REPL_VE_USED_SET(unit, extq_ptr);
    list_ptr[*list_num] = extq_ptr;
    (*list_num)++;
    
    if (next_ptr1 != extq_ptr) {
        if ((rv = _bcm_kt_extq_repl_list_get(unit, next_ptr1, list_ptr, 
             list_num)) < 0) {
            goto repl_list_get_done;   
        }                                                    
    } 
           
    if (next_ptr2 != extq_ptr) {
        if ((rv = _bcm_kt_extq_repl_list_get(unit, next_ptr2, list_ptr,
             list_num)) < 0) {  
            goto repl_list_get_done; 
        }                                                
    }

repl_list_get_done:
    return rv;

}

STATIC int
_bcm_kt_extq_repl_reload(int unit, int ipmc_id) 
{
    SHR_BITDCL *q_vec = NULL;
    uint32 *q_repl_ptr = NULL;
    int *extq_list_ptr;
    int q_max, q_num = 0;
    int list_max, list_num = 0;
    int alloc_size;
    int rv = BCM_E_NONE;
    int extq_ptr, extq_hash; 
    _bcm_repl_list_info_t *rli_start, *rli_current;
    int list_start_ptr, remap_id;
    int queue_count = 0;
    int list_size = 0;
    int queue_id, count = 0;
    bcm_port_t port;

    if ((rv = _bcm_kt_ipmc_mmu_mc_remap_ptr(unit, ipmc_id, 
                    &remap_id, FALSE)) < 0) {
         return rv;                
    }

    if ((rv = _kt_extq_mc_group_ptr(unit, remap_id,
                                &extq_ptr, 0, 0)) < 0) {
        return rv;
    }

    if (extq_ptr == 0) {
        return rv;
    }

    rli_start = IPMC_EXTQ_REPL_LIST_INFO(unit);
    if (IPMC_EXTQ_REPL_VE_USED_GET(unit, extq_ptr)) {
        /* We've already traversed this list, just note it */
        for (rli_current = rli_start; rli_current != NULL;
             rli_current = rli_current->next) {
            if (rli_current->index == extq_ptr) {
                (rli_current->refcount)++;
                IPMC_EXTQ_REPL_QUEUE_COUNT(unit, ipmc_id) =
                    rli_current->list_size;
                break;
            }
        }
        if (rli_current == NULL) {
            /* Table out of sync.  Not good. */
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_INTERNAL;
        } else {
            return rv;
        }
    }

    list_max = IPMC_EXTQ_LIST_TOTAL(unit);
    alloc_size = list_max * sizeof(int);
    extq_list_ptr = sal_alloc(alloc_size, "IPMC extq list array");
    if (extq_list_ptr == NULL) {
        return BCM_E_MEMORY;
    }    
    sal_memset(extq_list_ptr, 0, alloc_size);
    if ((rv = _bcm_kt_extq_repl_list_get(unit, extq_ptr, extq_list_ptr, 
                                         &list_num)) < 0 ) {
        goto reload_done;
    }    
        
    q_max = IPMC_EXTQ_TOTAL(unit);
    alloc_size = q_max * sizeof(int);
    q_repl_ptr = sal_alloc(alloc_size, "IPMC repl interface array");
    if (q_repl_ptr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(q_repl_ptr, 0, alloc_size);

    alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_TOTAL(unit));
    q_vec = sal_alloc(alloc_size, "IPMC extq replication vector");
    if (q_vec == NULL) {
        sal_free(q_repl_ptr);
        return BCM_E_MEMORY;
    }
    sal_memset(q_vec, 0, alloc_size);

    if ((rv = _bcm_kt_get_subscriber_queue_entries(unit, extq_ptr, q_vec, 
                                                  q_repl_ptr, &q_num)) < 0 ) {
        goto reload_done;
    }
    
    if (q_num > q_max) {
        rv = BCM_E_PARAM;
        goto reload_done;
    }

    /* Search for list already in table */
    extq_hash =
        _shr_crc32b(0, (uint8 *)q_vec, IPMC_EXTQ_TOTAL(unit));

    for (rli_current = rli_start; rli_current != NULL;
         rli_current = rli_current->next) {
         if (extq_hash == rli_current->hash) {
             rv = _bcm_kt_extq_repl_list_compare(unit, rli_current->index,
                                             q_vec, q_repl_ptr);
             if (rv == BCM_E_NOT_FOUND) {
                 continue; /* Not a match */
             } else if (rv < 0) {
                 goto reload_done; /* Access error */
             } else {
                 break; /* Match */
             }
        }
    }

    if (rli_current != NULL) {
        /* Found a match, point to here and increase reference count */
        if (extq_ptr == rli_current->index) {
            /* We're already pointing to this list, so done */
            rv = BCM_E_NONE;
            goto reload_done;
        } else {
            list_start_ptr = rli_current->index;
            queue_count = rli_current->list_size;
        }
    } else {
        /* Not a match, make a new chain */
        list_start_ptr = extq_ptr;
        list_size = (q_num % 4) ? ((q_num / 4) + 1) : (q_num / 4);
        queue_count = q_num;

        if (queue_count > 0) {
            /* Update data structures */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC extq repl list info");
            if (rli_current == NULL) {
                rv = BCM_E_MEMORY;
                goto reload_done;
            }
            sal_memset(rli_current, 0, alloc_size);
            rli_current->index = list_start_ptr;
            rli_current->hash = extq_hash;
            rli_current->next = rli_start;
            rli_current->list_size = list_size;
            IPMC_EXTQ_REPL_LIST_INFO(unit) = rli_current;
            rli_start = rli_current;
            (rli_current->refcount)++;

        }
    }

    IPMC_EXTQ_REPL_QUEUE_COUNT(unit, ipmc_id) = list_size;

    count = 0;
    for (queue_id = 0; ((queue_id < q_max) && (count < queue_count));
        queue_id++) {
        if (SHR_BITGET(q_vec, queue_id)) {
            count++;
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                if ((rv = bcm_kt2_cosq_port_get(unit, queue_id, &port)) < 0) {
                    goto reload_done;
                }
            } else 

#endif
            {  
                if ((rv = bcm_kt_cosq_port_get(unit, queue_id, &port)) < 0) {
                    goto reload_done;
                }
            }

            if (SOC_PORT_VALID(unit, port)) {
                _bcm_kt_ipmc_port_ext_queue_count_increment(unit, ipmc_id,
                                                            port);
            }
        }
    }

reload_done:
    if (extq_list_ptr != NULL) {
        sal_free(extq_list_ptr);
    }    
    if (q_repl_ptr != NULL) {
        sal_free(q_repl_ptr);
    }
    if (q_vec != NULL) {
        sal_free(q_vec);
    }

    return rv; 
}
#endif

#ifdef BCM_KATANA2_SUPPORT
/*
 * Function:
 *      _bcm_kt2_extq_repl_reload
 * Purpose:
 *      Re-Initialize IPMC extq replication software to state consistent with
 *      hardware
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt2_extq_repl_reload(int unit)
{
    bcm_port_t port;
    int ipmc_id, alloc_size;
    int rv;
    bcm_pbmp_t            pbmp_all;

    BCM_PBMP_CLEAR(pbmp_all);
    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &pbmp_all);
    }
    BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update (unit, &pbmp_all));

    /* Allocate struct for IPMC Extq replication bookkeeping */
    alloc_size = sizeof(_kt_extq_repl_info_t);
    _kt_extq_repl_info[unit] = sal_alloc(alloc_size, "IPMC Extq repl info");
    if (_kt_extq_repl_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_kt_extq_repl_info[unit], 0, alloc_size);
    _kt_extq_repl_info[unit]->ipmc_size = soc_mem_index_count(unit, L3_IPMCm);

    IPMC_EXTQ_REPL_MAX(unit) = (1 << soc_mem_field_length(unit, L3_IPMCm,
                                EXT_Q_NUM_COPIESf)) - 1;
    IPMC_EXTQ_TOTAL(unit) = 4096;
    IPMC_EXTQ_LIST_TOTAL(unit) = soc_mem_index_count(unit, MMU_EXT_MC_QUEUE_LIST0m);
    alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_LIST_TOTAL(unit));
    _kt_extq_repl_info[unit]->bitmap_entries_used =
    sal_alloc(alloc_size, "IPMC Extq repl entries used");
    if (_kt_extq_repl_info[unit]->bitmap_entries_used == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_kt_extq_repl_info[unit]->bitmap_entries_used, 0, alloc_size);

    /* Always reserve slot 0 */
    IPMC_EXTQ_REPL_VE_USED_SET(unit, 0);

    alloc_size = sizeof(_kt_repl_queue_info_t);
    _kt_extq_repl_info[unit]->mcgroup_info =
        sal_alloc(alloc_size, "IPMC MC group info");
    if (_kt_extq_repl_info[unit]->mcgroup_info == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_kt_extq_repl_info[unit]->mcgroup_info, 0, alloc_size);

    alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
    IPMC_EXTQ_GROUP_INFO(unit)->queue_count =
        sal_alloc(alloc_size, "IPMC Extq queue count");
    if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count,
               0, alloc_size);

    IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int =
        sal_alloc(alloc_size, "IPMC Extq queue count internal");
    if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int,
            0, alloc_size);

    IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext =
        sal_alloc(alloc_size, "IPMC Extq queue count external");
    if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext,
            0, alloc_size);

    PBMP_ITER(pbmp_all, port) {
        alloc_size = sizeof(_kt_repl_queue_info_t);
        IPMC_EXTQ_REPL_PORT_INFO(unit, port) =
            sal_alloc(alloc_size, "IPMC Extq repl port info");
        if (IPMC_EXTQ_REPL_PORT_INFO(unit, port) == NULL) {
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port), 0, alloc_size);

        alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
        IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count =
            sal_alloc(alloc_size, "IPMC Extq repl port queue count");
        if (IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count == NULL) {
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count,
                   0, alloc_size);
    }

    /* reserve ipmc index 0 & 1 for extended queue work around */
    BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 0));
    BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 1));
    /*
     * Read IPMC group and IPMC VLAN tables to build up software state
     */
    for (ipmc_id = soc_mem_index_min(unit, L3_IPMCm);
         ipmc_id <= soc_mem_index_max(unit, L3_IPMCm); ipmc_id++) {
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANAX(unit)) {
            if (ipmc_id > 1) {
                rv = _bcm_kt_extq_repl_reload(unit, ipmc_id);
                if (BCM_FAILURE(rv)) {
                    bcm_kt_ipmc_repl_detach(unit);
                    return rv;
                }
            }
        }
#endif
    }
    return BCM_E_NONE;
}

#endif

/*
 * Function:
 *      _bcm_tr2_ipmc_repl_reload
 * Purpose:
 *      Re-Initialize IPMC replication software to state consistent with
 *      hardware
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_ipmc_repl_reload(int unit)
{
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    uint32 ms_bit, ls_bits[2];
    bcm_port_t port;
    int ipmc_id, alloc_size, intf_alloc_size;
    int vlan_ptr, next_vlan_ptr, first_vlan_ptr;
    SHR_BITDCL          *intf_vec = NULL;
    _bcm_repl_list_info_t *rli_start, *rli_current;
    uint8 flags;
    int rv;

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
    uint32 if_cnt = 0;
    uint32 nh_offset = 0;
    uint32 offset = 0;
    uint32 ms_max = 0;
#endif

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        bcm_kt_ipmc_repl_detach(unit);
        if_cnt = soc_mem_index_count(unit, EGR_L3_INTFm);
        /* coverity[large_shift : FALSE] */
        nh_offset = (1 << (soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm,
                                    MSB_VLANf) - 1));
    } else
#endif
    {
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            if_cnt = soc_mem_index_count(unit, EGR_L3_INTFm);
            nh_offset = TD_EGR_L3_NEXT_HOP_OFFSET/64;
        }
#endif
    }
    bcm_tr2_ipmc_repl_detach(unit);

    /* Allocate struct for IPMC replication booking keeping */
    alloc_size = sizeof(_tr2_repl_info_t);
    _tr2_repl_info[unit] = sal_alloc(alloc_size, "IPMC repl info");
    if (_tr2_repl_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_tr2_repl_info[unit], 0, alloc_size);

    _tr2_repl_info[unit]->ipmc_size = soc_mem_index_count(unit, L3_IPMCm);
    IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm) +
                                 soc_mem_index_count(unit, EGR_L3_INTFm);

    IPMC_REPL_TOTAL(unit) = soc_mem_index_count(unit, MMU_IPMC_VLAN_TBLm);
    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_TOTAL(unit));
    _tr2_repl_info[unit]->bitmap_entries_used =
        sal_alloc(alloc_size, "IPMC repl entries used");
    if (_tr2_repl_info[unit]->bitmap_entries_used == NULL) {
        bcm_tr2_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_tr2_repl_info[unit]->bitmap_entries_used, 0, alloc_size);

    /* Always reserve slot 0 (because 0 means NULL pointer) */
    IPMC_REPL_VE_USED_SET(unit, 0);

    if (soc_feature(unit, soc_feature_ipmc_reduced_table_size)) {
        IPMC_REPL_TOTAL(unit) = soc_mem_index_count(unit, MMU_IPMC_VLAN_TBLm) / 2;
        /* reserve slot 1 for null entry for dynamic update */
        IPMC_REPL_VE_USED_SET(unit, 1);
    }

#ifdef BCM_KATANA_SUPPORT    
    if (SOC_IS_KATANAX(unit)) {            
        /* Allocate struct for IPMC Extq replication bookkeeping */
        alloc_size = sizeof(_kt_extq_repl_info_t);
        _kt_extq_repl_info[unit] = sal_alloc(alloc_size, "IPMC Extq repl info");
        if (_kt_extq_repl_info[unit] == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(_kt_extq_repl_info[unit], 0, alloc_size);
        _kt_extq_repl_info[unit]->ipmc_size = soc_mem_index_count(unit, L3_IPMCm);
    
        IPMC_EXTQ_REPL_MAX(unit) = (1 << soc_mem_field_length(unit, L3_IPMCm, 
                                    EXT_Q_NUM_COPIESf)) - 1; 
        IPMC_EXTQ_TOTAL(unit) = 4096;     
        IPMC_EXTQ_LIST_TOTAL(unit) = soc_mem_index_count(unit, MMU_EXT_MC_QUEUE_LIST0m);
        alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_LIST_TOTAL(unit));
        _kt_extq_repl_info[unit]->bitmap_entries_used =
        sal_alloc(alloc_size, "IPMC Extq repl entries used");
        if (_kt_extq_repl_info[unit]->bitmap_entries_used == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(_kt_extq_repl_info[unit]->bitmap_entries_used, 0, alloc_size);
            
        /* Always reserve slot 0 */
        IPMC_EXTQ_REPL_VE_USED_SET(unit, 0);
    
        alloc_size = sizeof(_kt_repl_queue_info_t);
        _kt_extq_repl_info[unit]->mcgroup_info = 
            sal_alloc(alloc_size, "IPMC MC group info");
        if (_kt_extq_repl_info[unit]->mcgroup_info == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(_kt_extq_repl_info[unit]->mcgroup_info, 0, alloc_size);
            
        alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
        IPMC_EXTQ_GROUP_INFO(unit)->queue_count =
            sal_alloc(alloc_size, "IPMC Extq queue count");
        if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count,
                   0, alloc_size);

        IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int =
            sal_alloc(alloc_size, "IPMC Extq queue count internal");
        if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int,
                   0, alloc_size);

        IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext =
            sal_alloc(alloc_size, "IPMC Extq queue count external");
        if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext,
                   0, alloc_size);

        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            alloc_size = sizeof(_kt_repl_queue_info_t);
            IPMC_EXTQ_REPL_PORT_INFO(unit, port) =
                sal_alloc(alloc_size, "IPMC Extq repl port info");
            if (IPMC_EXTQ_REPL_PORT_INFO(unit, port) == NULL) {
                bcm_tr2_ipmc_repl_detach(unit);
                bcm_kt_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
            sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port), 0, alloc_size);

            alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
            IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count =
                sal_alloc(alloc_size, "IPMC Extq repl port queue count");
            if (IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count == NULL) {
                bcm_tr2_ipmc_repl_detach(unit);
                bcm_kt_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
            sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count,
                       0, alloc_size);
        }

        /* reserve ipmc index 0 & 1 for extended queue work around */
        BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 0));
        BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 1));    
        sw_to_hw_queue[unit] =
           sal_alloc(IPMC_EXTQ_TOTAL(unit) * sizeof(uint32),
                    "SW to HW queue");
        if (sw_to_hw_queue[unit] == NULL) {
            bcm_kt_ipmc_repl_detach(unit);
            return (BCM_E_MEMORY);
        }
        sal_memset(sw_to_hw_queue[unit], 0,
           IPMC_EXTQ_TOTAL(unit) * sizeof(uint32));
    }
#endif

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        alloc_size = sizeof(_tr2_repl_port_info_t);
        IPMC_REPL_PORT_INFO(unit, port) =
            sal_alloc(alloc_size, "IPMC repl port info");
        if (IPMC_REPL_PORT_INFO(unit, port) == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port), 0, alloc_size);

        alloc_size = sizeof(int32) * _tr2_repl_info[unit]->ipmc_size;
        IPMC_REPL_PORT_INFO(unit, port)->vlan_count =
            sal_alloc(alloc_size, "IPMC repl port vlan count");
        if (IPMC_REPL_PORT_INFO(unit, port)->vlan_count == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port)->vlan_count,
                   0, alloc_size);
    }

    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(alloc_size, "IPMC repl interface vector");
    if (intf_vec == NULL) {
        bcm_tr2_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    intf_alloc_size = alloc_size;

    rli_start = IPMC_REPL_LIST_INFO(unit);

    /*
     * Read IPMC group and IPMC VLAN tables to build up software state
     */
    for (ipmc_id = soc_mem_index_min(unit, L3_IPMCm);
         ipmc_id <= soc_mem_index_max(unit, L3_IPMCm); ipmc_id++) {

        PBMP_ITER (PBMP_PORT_ALL(unit), port) {
            rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port, &first_vlan_ptr, 0, 0);
            if (BCM_FAILURE(rv)) {
                sal_free(intf_vec);
                bcm_tr2_ipmc_repl_detach(unit);
                return rv;
            }
            if (first_vlan_ptr == 0) {
                continue;
            }

            if (IPMC_REPL_VE_USED_GET(unit, first_vlan_ptr)) {
                /* We've already traversed this list, just note it */
                for (rli_current = rli_start; rli_current != NULL;
                     rli_current = rli_current->next) {
                    if (rli_current->index == first_vlan_ptr) {
                        (rli_current->refcount)++;
                        IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) = 
                            rli_current->list_size;
                        break;
                    }
                }
                if (rli_current == NULL) {
                    /* Table out of sync.  Not good. */
                    sal_free(intf_vec);
                    bcm_tr2_ipmc_repl_detach(unit);
                    return BCM_E_INTERNAL;
                } else {
                    continue;
                }
            }

            sal_memset(intf_vec, 0, intf_alloc_size);
            next_vlan_ptr = first_vlan_ptr;
            vlan_ptr = 0;
            while (vlan_ptr != next_vlan_ptr) {
                vlan_ptr = next_vlan_ptr;
                rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                             vlan_ptr, &vlan_entry);
                if (BCM_FAILURE(rv)) {
                    sal_free(intf_vec);
                    bcm_tr2_ipmc_repl_detach(unit);
                    return rv;
                }
                soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                       LSB_VLAN_BMf, ls_bits);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
                if (SOC_IS_KATANAX(unit) || SOC_IS_TD_TT(unit)) {
                    ms_max = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                       &vlan_entry, MSB_VLANf);
                    if (ms_max >= nh_offset) {
                        /* Replication is on Nexthops */
                        offset = _SHR_BITDCLSIZE(if_cnt);
                        ms_bit = ms_max - nh_offset;
                    } else {
                        /* Replication is on L3 Interface */
                        offset = 0;
                        ms_bit = ms_max;
                    }
                    intf_vec[offset + 2 * ms_bit + 0] = ls_bits[0];
                    intf_vec[offset + 2 * ms_bit + 1] = ls_bits[1];
                } else
#endif
                {
                    ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                       &vlan_entry, MSB_VLANf);
                    intf_vec[2 * ms_bit + 0] = ls_bits[0];
                    intf_vec[2 * ms_bit + 1] = ls_bits[1];
                }

                IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) +=
                      _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
                IPMC_REPL_VE_USED_SET(unit, vlan_ptr);
                next_vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                  &vlan_entry, NEXTPTRf);
                if (soc_feature(unit, soc_feature_ipmc_reduced_table_size)) {
                    if (vlan_ptr == first_vlan_ptr && next_vlan_ptr == 1) {
                        next_vlan_ptr = vlan_ptr;
                    }	
                }
            }   

            /* Create new list element */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC repl list info");
            if (rli_current == NULL) {
                sal_free(intf_vec);
                bcm_tr2_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
            sal_memset(rli_current, 0, alloc_size);
            rli_current->index = first_vlan_ptr;
            rli_current->hash =
                _shr_crc32b(0, (uint8 *)intf_vec,
                            IPMC_REPL_INTF_TOTAL(unit));
            rli_current->next = rli_start;
            rli_current->list_size =
                IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id);
            IPMC_REPL_LIST_INFO(unit) = rli_current;
            rli_start = rli_current;
            (rli_current->refcount)++;
        }

#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANAX(unit)) {
            if (ipmc_id > 1) {
                rv = _bcm_kt_extq_repl_reload(unit, ipmc_id);
                if (BCM_FAILURE(rv)) {
                    sal_free(intf_vec);
                    bcm_tr2_ipmc_repl_detach(unit);
                    bcm_kt_ipmc_repl_detach(unit);
                    return rv;
                }    
            }
        }    
#endif
    }

    /* Recover REPL list mode from HW cache */
    rv = _bcm_esw_ipmc_repl_wb_flags_get(unit,
                                         _BCM_IPMC_WB_REPL_LIST, &flags);
    if (flags) {
        SOC_IPMCREPLSHR_SET(unit, 1);
    }

    sal_free(intf_vec);
    return BCM_E_NONE;
}

#ifdef BCM_KATANA_SUPPORT
/*
 * Function:
 *      _bcm_kt_ipmc_hw_queue_info_scache_size_get
 * Purpose:
 *      Get the required scache size for storing hw queue info.
 * Parameters:
 *      unit - (IN)  StrataSwitch unit #
 *      size - (OUT) Number of bytes
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_kt_ipmc_hw_queue_info_scache_size_get(int unit, uint32 *size)
{
    int hw_queue_size;

    *size = 0;
    hw_queue_size = IPMC_EXTQ_TOTAL(unit) * sizeof(uint32);
    *size += hw_queue_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_ipmc_hw_queue_info_sync
 * Purpose:
 *      Sync hw queue info to scache.
 * Parameters:
 *      unit       - (IN)     StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_kt_ipmc_hw_queue_info_sync(int unit, uint8 **scache_ptr)
{
    int hw_queue_size;

    hw_queue_size = IPMC_EXTQ_TOTAL(unit) * sizeof(uint32);
    sal_memcpy((*scache_ptr), sw_to_hw_queue[unit], hw_queue_size);
    (*scache_ptr) += hw_queue_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_kt_ipmc_hw_queue_info_recover
 * Purpose:
 *      Recover hw queue info from scache.
 * Parameters:
 *      unit       - (IN)     StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_kt_ipmc_hw_queue_info_recover(int unit, uint8 **scache_ptr)
{
    int hw_queue_size;

    hw_queue_size = IPMC_EXTQ_TOTAL(unit) * sizeof(uint32);
    sal_memcpy(sw_to_hw_queue[unit], (*scache_ptr), hw_queue_size);
    (*scache_ptr) += hw_queue_size;

    return BCM_E_NONE;
}
#endif /* BCM_KATANA_SUPPORT */

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_tr2_ipmc_repl_init
 * Purpose:
 *      Initialize IPMC replication.
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_init(int unit)
{
    bcm_port_t          port;
    int                 alloc_size;
    uint32              rval;
#ifdef BCM_KATANA_SUPPORT
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
#endif

    if (SOC_IS_TRIDENT2X(unit)) { 
        return BCM_E_NONE;
    }

    bcm_tr2_ipmc_repl_detach(unit);
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        bcm_kt_ipmc_repl_detach(unit);
    }
#endif

    /* Allocate struct for IPMC replication bookkeeping */
    alloc_size = sizeof(_tr2_repl_info_t);
    _tr2_repl_info[unit] = sal_alloc(alloc_size, "IPMC repl info");
    if (_tr2_repl_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_tr2_repl_info[unit], 0, alloc_size);
    _tr2_repl_info[unit]->ipmc_size = soc_mem_index_count(unit, L3_IPMCm);

#ifdef BCM_BRADLEY_SUPPORT
    if (SOC_REG_FIELD_VALID(unit, MC_CONTROL_5r, SHARED_TABLE_IPMC_SIZEf)) {
        int ipmc_base, ipmc_size;
        
        SOC_IF_ERROR_RETURN
            (soc_hbx_ipmc_size_get(unit, &ipmc_base, &ipmc_size));

        if (_tr2_repl_info[unit]->ipmc_size > ipmc_size) {
            /* Reduce to fix allocated table space */
            _tr2_repl_info[unit]->ipmc_size = ipmc_size;
        }
    }
#endif

    IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm) + 
                                 soc_mem_index_count(unit, EGR_L3_INTFm);

#if defined(BCM_KATANA2_SUPPORT)
    
    if (SOC_IS_KATANA2(unit)) {
        IPMC_REPL_TOTAL(unit) = soc_mem_index_count(unit, MMU_REPL_LIST_TBLm);
    } else
#endif
    {
        IPMC_REPL_TOTAL(unit) = soc_mem_index_count(unit, MMU_IPMC_VLAN_TBLm);
    }
    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_TOTAL(unit));
    _tr2_repl_info[unit]->bitmap_entries_used =
        sal_alloc(alloc_size, "IPMC repl entries used");
    if (_tr2_repl_info[unit]->bitmap_entries_used == NULL) {
        bcm_tr2_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_tr2_repl_info[unit]->bitmap_entries_used, 0, alloc_size);

    /* Always reserve slot 0 */
    IPMC_REPL_VE_USED_SET(unit, 0);

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        alloc_size = sizeof(_tr2_repl_port_info_t);
        IPMC_REPL_PORT_INFO(unit, port) =
            sal_alloc(alloc_size, "IPMC repl port info");
        if (IPMC_REPL_PORT_INFO(unit, port) == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port), 0, alloc_size);

        alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);

        IPMC_REPL_PORT_INFO(unit, port)->vlan_count =
            sal_alloc(alloc_size, "IPMC repl port vlan count");
        if (IPMC_REPL_PORT_INFO(unit, port)->vlan_count == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port)->vlan_count,
                   0, alloc_size);
    }

    if (soc_feature(unit, soc_feature_ipmc_reduced_table_size)) {
        IPMC_REPL_TOTAL(unit) = soc_mem_index_count(unit, MMU_IPMC_VLAN_TBLm) / 2;
        /* reserve slot 1 for null entry for dynamic update */
        IPMC_REPL_VE_USED_SET(unit, 1);
    }


#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        /* Allocate struct for IPMC Extq replication bookkeeping */
        alloc_size = sizeof(_kt_extq_repl_info_t);
        _kt_extq_repl_info[unit] = sal_alloc(alloc_size, "IPMC Extq repl info");
        if (_kt_extq_repl_info[unit] == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(_kt_extq_repl_info[unit], 0, alloc_size);
        _kt_extq_repl_info[unit]->ipmc_size = soc_mem_index_count(unit, L3_IPMCm);

        IPMC_EXTQ_REPL_MAX(unit) = (1 << soc_mem_field_length(unit, L3_IPMCm, 
                                    EXT_Q_NUM_COPIESf)) - 1; 
        IPMC_EXTQ_TOTAL(unit) = 4096;    
        IPMC_EXTQ_LIST_TOTAL(unit) = soc_mem_index_count(unit, MMU_EXT_MC_QUEUE_LIST0m);
        alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_LIST_TOTAL(unit));
        _kt_extq_repl_info[unit]->bitmap_entries_used =
        sal_alloc(alloc_size, "IPMC Extq repl entries used");
        if (_kt_extq_repl_info[unit]->bitmap_entries_used == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(_kt_extq_repl_info[unit]->bitmap_entries_used, 0, alloc_size);

        /* Always reserve slot 0 */
        IPMC_EXTQ_REPL_VE_USED_SET(unit, 0);

        alloc_size = sizeof(_kt_repl_queue_info_t);
        _kt_extq_repl_info[unit]->mcgroup_info = 
            sal_alloc(alloc_size, "IPMC MC group info");
        if (_kt_extq_repl_info[unit]->mcgroup_info == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(_kt_extq_repl_info[unit]->mcgroup_info, 0, alloc_size);

        alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
        IPMC_EXTQ_GROUP_INFO(unit)->queue_count =
            sal_alloc(alloc_size, "IPMC Extq queue count");
        if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count,
                   0, alloc_size);

        IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int =
            sal_alloc(alloc_size, "IPMC Extq queue count internal");
        if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int,
                   0, alloc_size);

        IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext =
            sal_alloc(alloc_size, "IPMC Extq queue count external");
        if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext == NULL) {
            bcm_tr2_ipmc_repl_detach(unit);
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext,
                   0, alloc_size);

        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            alloc_size = sizeof(_kt_repl_queue_info_t);
            IPMC_EXTQ_REPL_PORT_INFO(unit, port) =
                sal_alloc(alloc_size, "IPMC Extq repl port info");
            if (IPMC_EXTQ_REPL_PORT_INFO(unit, port) == NULL) {
                bcm_tr2_ipmc_repl_detach(unit);
                bcm_kt_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
            sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port), 0, alloc_size);

            alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
            IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count =
                sal_alloc(alloc_size, "IPMC Extq repl port queue count");
            if (IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count == NULL) {
                bcm_tr2_ipmc_repl_detach(unit);
                bcm_kt_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
            sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count,
                       0, alloc_size);
        }


        /* reserve ipmc index 0 & 1 for extended queue work around */
        BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 0));
        BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 1));

            /* Read L3 ipmc table. */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY,
                                     0, entry));
            /* use 1 as initial redirection pointer */
        soc_mem_field32_set(unit, L3_IPMCm, entry, 
                                    MMU_MC_REDIRECTION_PTRf, 1);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, 
                                    0, &entry));
        sw_to_hw_queue[unit] =
           sal_alloc(IPMC_EXTQ_TOTAL(unit) * sizeof(uint32),
                    "SW to HW queue");
        if (sw_to_hw_queue[unit] == NULL) {
            bcm_kt_ipmc_repl_detach(unit);
            return (BCM_E_MEMORY);
        }
        sal_memset(sw_to_hw_queue[unit], 0,
           IPMC_EXTQ_TOTAL(unit) * sizeof(uint32));
    }
#endif

    if (soc_property_get(unit, spn_IPMC_INDEPENDENT_MODE, 0)) {
        BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, IPMC_IND_MODEf, 1);
        BCM_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    }
    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_BCMSIM && !SAL_BOOT_XGSSIM) {
        /* Hardware has already cleared the table during reset for Tridnet */
        if (!SOC_IS_TD_TT(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL0m, COPYNO_ALL, FALSE));
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL1m, COPYNO_ALL, FALSE));
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL2m, COPYNO_ALL, FALSE));
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL3m, COPYNO_ALL, FALSE));
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL4m, COPYNO_ALL, FALSE));
            if (SOC_MEM_IS_VALID(unit, MMU_IPMC_GROUP_TBL5m)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL5m, COPYNO_ALL, FALSE));
            }
            if (SOC_MEM_IS_VALID(unit, MMU_IPMC_GROUP_TBL6m)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL6m, COPYNO_ALL, FALSE));
            }
            if (SOC_MEM_IS_VALID(unit, MMU_IPMC_GROUP_TBL7m)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL7m, COPYNO_ALL, FALSE));
            }
            if (SOC_MEM_IS_VALID(unit, MMU_IPMC_GROUP_TBL8m)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL8m, COPYNO_ALL, FALSE));
            }
            if (SOC_MEM_IS_VALID(unit, MMU_IPMC_VLAN_TBLm)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_IPMC_VLAN_TBLm, COPYNO_ALL, FALSE));
            }
        }
    }
    
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_detach
 * Purpose:
 *      Initialize IPMC replication.
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_detach(int unit)
{
    bcm_port_t          port;
    _bcm_repl_list_info_t *rli_current, *rli_free;

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        BCM_IF_ERROR_RETURN(bcm_kt_ipmc_repl_detach(unit));
    }
#endif

    if (_tr2_repl_info[unit] != NULL) {
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            if (_tr2_repl_info[unit]->port_info[port] != NULL) {
                if (_tr2_repl_info[unit]->
                    port_info[port]->vlan_count != NULL) {
                    sal_free(_tr2_repl_info[unit]->
                             port_info[port]->vlan_count);
                }
                sal_free(_tr2_repl_info[unit]->port_info[port]);
            }
        }

        if (_tr2_repl_info[unit]->bitmap_entries_used != NULL) {
            sal_free(_tr2_repl_info[unit]->bitmap_entries_used);
        }

        if (_tr2_repl_info[unit]->repl_list_info != NULL) {
            rli_current = IPMC_REPL_LIST_INFO(unit);
            while (rli_current != NULL) {
                rli_free = rli_current;
                rli_current = rli_current->next;
                sal_free(rli_free);
            }
        }

        sal_free(_tr2_repl_info[unit]);
        _tr2_repl_info[unit] = NULL;
    }

    return BCM_E_NONE;
}

int
_bcm_tr2_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                             int id)
{
    int *if_array = NULL;
    int intf_num, intf_max, alloc_size, rv = BCM_E_NONE;
    int                 partition;
    bcm_l3_intf_t       l3_intf;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(int);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &intf_num);
    if (BCM_SUCCESS(rv)) {
        if (intf_num < intf_max) {
            if_array[intf_num++] = id;

            /* For IPMC, check port is a member of the L3 interface's VLAN.
             * Performing this check here is more efficient than performing
             * it in bcm_tr2_ipmc_egress_intf_set.
             */ 
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, id)) {
                /* L3 interface is used */
                partition = soc_mem_index_count(unit, EGR_L3_INTFm);
                if (id > partition) {
                    rv = BCM_E_PARAM;
                    goto intf_add_done;
                }

                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = id;
                if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                    goto intf_add_done;
                }
            } 

            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             intf_num, if_array, FALSE);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

intf_add_done:

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array); 
    return rv;
}

int
_bcm_tr2_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                                int if_max, int id)
{
    int  *if_array = NULL;
    int  alloc_size, if_count, if_cur, match, rv = BCM_E_NONE;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    } else if ((if_max <= 0) ||
               ((uint32)if_max > IPMC_REPL_INTF_TOTAL(unit))) {
        return BCM_E_PARAM;
    }
    alloc_size = if_max * sizeof(int);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, if_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array, FALSE);
        } else { 
            rv = BCM_E_NOT_FOUND;
        }
    }
    
    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *	_bcm_tr2_repl_list_compare
 * Description:
 *	Compare HW list starting at vlan_index to the VLAN list contained
 *      in vlan_vec.
 */

STATIC int
_bcm_tr2_repl_list_compare(int unit, int vlan_index,
                          SHR_BITDCL *intf_vec)
{
    uint32		ms_bit, hw_ms_bit, ms_max, msb;
    uint32		ls_bits[2], hw_ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int	                vlan_ptr, last_vlan_ptr;
    int                 repl_type;
    int                 ls_len, if_cnt, nh_cnt, offset;
    int                 nh_offset;

    if_cnt = soc_mem_index_count(unit, EGR_L3_INTFm);
    nh_cnt = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    last_vlan_ptr = -1;
    vlan_ptr = vlan_index;
    ls_len = soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm, LSB_VLAN_BMf);
    if (SOC_IS_TD_TT(unit)) {
        nh_offset = TD_EGR_L3_NEXT_HOP_OFFSET;
    } else {
        /* coverity[large_shift : FALSE] */
        /* coverity[negative_shift : FALSE] */
        nh_offset = (1 << (soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm,
                                                   MSB_VLANf) - 1)) * ls_len;
    }
    /* Iterate over the two replication types - interfaces and next hops */
    for (repl_type = 0; repl_type < 2; repl_type++) {
        if (repl_type == 0) {
            ms_max = _SHR_BITDCLSIZE(if_cnt) / 2; /* 32 -> 64 */
            offset = 0;
        } else {
            ms_max = _SHR_BITDCLSIZE(nh_cnt) / 2; /* 32 -> 64 */
            offset = _SHR_BITDCLSIZE(if_cnt);
        }
        for (ms_bit = 0; ms_bit < ms_max; ms_bit++) {
            ls_bits[0] = intf_vec[offset + 2 * ms_bit + 0];
            ls_bits[1] = intf_vec[offset + 2 * ms_bit + 1];
            if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
                if (vlan_ptr == last_vlan_ptr) { /* HW list end, not app list */
                    return BCM_E_NOT_FOUND;
                }
                SOC_IF_ERROR_RETURN
                    (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                                                       vlan_ptr, &vlan_entry));
                hw_ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                           &vlan_entry, MSB_VLANf);
                soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                                       LSB_VLAN_BMf, hw_ls_bits);

                if (repl_type == 1) {
                    /* Replication is over next hops */
                    msb = ms_bit + nh_offset / 64;
                } else {
                    /* Replication is over interfaces */
                    msb = ms_bit;
                }

                if ((hw_ms_bit != msb) || (ls_bits[0] != hw_ls_bits[0]) ||
                    (ls_bits[1] != hw_ls_bits[1])) {
                    return BCM_E_NOT_FOUND;
                }
                last_vlan_ptr = vlan_ptr;
                vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                              &vlan_entry, NEXTPTRf);
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_tr2_repl_list_free
 * Description:
 *	Release the IPMC_VLAN entries in the HW list starting at start_ptr.
 */

STATIC int
_bcm_tr2_repl_list_free(int unit, int start_ptr)
{
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int	                vlan_ptr, last_vlan_ptr;

    last_vlan_ptr = -1;
    vlan_ptr = start_ptr;

    while (vlan_ptr != last_vlan_ptr) {
        SOC_IF_ERROR_RETURN
            (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                                               vlan_ptr, &vlan_entry));
        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                          &vlan_entry, NEXTPTRf);

        if (soc_feature(unit, soc_feature_ipmc_reduced_table_size)) {
            if (last_vlan_ptr == start_ptr && vlan_ptr == 1) {
                vlan_ptr = last_vlan_ptr;
            }	
        }
		
        IPMC_REPL_VE_USED_CLR(unit, last_vlan_ptr);
    }

#ifdef BCM_KATANA_SUPPORT
    if (soc_feature(unit, soc_feature_ipmc_reduced_table_size)) {
        return _kt_ipmc_group_entry_clear(unit, start_ptr);
    }	
#endif
	
    return BCM_E_NONE;
}

STATIC int
_tr2_ipmc_repl_next_free_ptr(int unit)
{
    int                 ix, bit;
    SHR_BITDCL          not_ptrs;

    for (ix = 0; ix < _SHR_BITDCLSIZE(IPMC_REPL_TOTAL(unit)); ix++) {
        not_ptrs = ~_tr2_repl_info[unit]->bitmap_entries_used[ix];
        if (not_ptrs) {
            for (bit = 0; bit < SHR_BITWID; bit++) {
                if (not_ptrs & (1 << bit)) {
                    return (ix * SHR_BITWID) + bit;
                }
            }
        }
    }

    return -1;
}

STATIC int
_kt_ipmc_repl_next_free_ptr(int unit, int head)
{
    int                 ix, bit;
    int                 start_ix, end_ix;
    SHR_BITDCL          not_ptrs;

    start_ix = (head == 1) ? IPMC_REPL_TOTAL(unit) : 0;
    end_ix = start_ix + IPMC_REPL_TOTAL(unit);

    for (ix = _SHR_BITDCLSIZE(start_ix); ix < _SHR_BITDCLSIZE(end_ix); ix++) {
        not_ptrs = ~_tr2_repl_info[unit]->bitmap_entries_used[ix];
        if (not_ptrs) {
            for (bit = 0; bit < SHR_BITWID; bit++) {
                if (not_ptrs & (1 << bit)) {
                    return (ix * SHR_BITWID) + bit;
                }
            }
        }
    }

    return -1;
}

/*
 * Function:
 *	_bcm_tr2_repl_list_write
 * Description:
 *	Write the VLAN list contained in vlan_vec into the HW table.
 *      Return the start_index and total VLAN count.
 */

STATIC int
_bcm_tr2_repl_list_write(int unit, int *start_index, int *count,
                        SHR_BITDCL *intf_vec)
{
    uint32		ms_bit, ms_max, msb;
    uint32		ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int	                vlan_ptr, last_vlan_ptr, last_write_ptr;
    int                 vlan_count = 0;
    int                 repl_type;
    int                 ls_len, if_cnt, nh_cnt, offset;
    int                 nh_offset;
    
    if_cnt = soc_mem_index_count(unit, EGR_L3_INTFm);
    nh_cnt = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    last_vlan_ptr = -1;
    last_write_ptr = -1;
    ls_len = soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm, LSB_VLAN_BMf);
    if (SOC_IS_TD_TT(unit)) {
        nh_offset = TD_EGR_L3_NEXT_HOP_OFFSET;
    } else {
        /* coverity[large_shift : FALSE] */
        nh_offset = (1 << (soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm,
                                                   MSB_VLANf) - 1)) * ls_len;
    }
    /* Iterate over the two replication types - interfaces and next hops */
    for (repl_type = 0; repl_type < 2; repl_type++) {
        if (repl_type == 0) {
            ms_max = _SHR_BITDCLSIZE(if_cnt) / 2; /* 32 -> 64 */
        } else {
            ms_max = _SHR_BITDCLSIZE(nh_cnt) / 2; /* 32 -> 64 */
        }
        for (ms_bit = 0; ms_bit < ms_max; ms_bit++) {
            offset = (repl_type == 0) ? 0 : _SHR_BITDCLSIZE(if_cnt);
            ls_bits[0] = intf_vec[offset + 2 * ms_bit + 0];
            ls_bits[1] = intf_vec[offset + 2 * ms_bit + 1];
            if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
                vlan_ptr = _tr2_ipmc_repl_next_free_ptr(unit);
                if (last_vlan_ptr > 0) {
                    /* Write previous non-zero entry */
                    soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                            &vlan_entry, NEXTPTRf,
                            (vlan_ptr > 0) ? vlan_ptr : last_vlan_ptr);
                    /* If vlan_ptr <= 0, terminate so
                     * later cleanup can remove the chain. */
                    SOC_IF_ERROR_RETURN
                        (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                            MEM_BLOCK_ALL, last_vlan_ptr, &vlan_entry));
                    last_write_ptr = last_vlan_ptr;
                    if (vlan_ptr < 0) {
                        _bcm_tr2_repl_list_free(unit, *start_index);
                        return BCM_E_RESOURCE;
                    }
                } else {
                    if (vlan_ptr < 0) {
                        return BCM_E_RESOURCE;
                    }
                    *start_index = vlan_ptr;
                }
                sal_memset(&vlan_entry, 0, sizeof(vlan_entry));
                if (repl_type == 1) {
                    /* Replication is over next hops */
                    msb = ms_bit + nh_offset / 64;
                } else {
                    /* Replication is over interfaces */
                    msb = ms_bit;
                }
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                                   MSB_VLANf, msb);
                soc_MMU_IPMC_VLAN_TBLm_field_set(unit, &vlan_entry,
                                                 LSB_VLAN_BMf, ls_bits);
                IPMC_REPL_VE_USED_SET(unit, vlan_ptr);
                vlan_count +=
                    _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
                last_vlan_ptr = vlan_ptr;
            }
        }
    }

    if (last_vlan_ptr > 0) {
        /* Write final entry */
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                          &vlan_entry, NEXTPTRf, last_vlan_ptr);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                   MEM_BLOCK_ALL, last_vlan_ptr, &vlan_entry));
        
        if (last_write_ptr > 0) {
            soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);
            if ((_shr_popcount(ls_bits[0]) +
                 _shr_popcount(ls_bits[1])) == 1) {
                /* Mark previous link with "last" flag */
                SOC_IF_ERROR_RETURN
                    (READ_MMU_IPMC_VLAN_TBLm(unit,
                          MEM_BLOCK_ANY, last_write_ptr, &vlan_entry));
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                                   LASTf, 1);
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                          MEM_BLOCK_ALL, last_write_ptr, &vlan_entry));
            }
        }
    }

    *count = vlan_count;
    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_kt_repl_list_write
 * Description:
 *    Write the VLAN list contained in vlan_vec into the HW table.
 *    Return the start_index and total VLAN count.
 */

STATIC int
_bcm_kt_repl_list_write(int unit, int *start_index, int *count,
                        SHR_BITDCL *intf_vec, int intf_count, 
                        int nh_count)
{
    uint32        ms_bit, ms_max, msb;
    uint32        ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int                 vlan_ptr, last_vlan_ptr, last_write_ptr;
    int                 vlan_count = 0;
    int                 repl_type = 0;
    int                 if_cnt, nh_cnt, offset;
    int                 nh_offset;
    int                 head = 1;

    if_cnt = soc_mem_index_count(unit, EGR_L3_INTFm);
    nh_cnt = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    last_vlan_ptr = -1;
    last_write_ptr = -1;

    /* coverity[large_shift : FALSE] */
    nh_offset = (1 << (soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm,
                                            MSB_VLANf) - 1)) * 64;

    if ((intf_count > 0) && (nh_count > 0)) {
        /* there is no need to have a mixed replication types in a given
         * mcast grpup */
        return BCM_E_PARAM;
    }

    if (nh_count > 0) {
        repl_type = 1;
        ms_max = _SHR_BITDCLSIZE(nh_cnt) / 2; /* 32 -> 64 */
    } else {
        ms_max = _SHR_BITDCLSIZE(if_cnt) / 2; /* 32 -> 64 */ 
    }

    offset = (repl_type == 0) ? 0 : _SHR_BITDCLSIZE(if_cnt);
    for (ms_bit = 0; ms_bit < ms_max; ms_bit++) {
        ls_bits[0] = intf_vec[offset + 2 * ms_bit + 0];
        ls_bits[1] = intf_vec[offset + 2 * ms_bit + 1];
        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            if (soc_feature(unit, soc_feature_ipmc_reduced_table_size)) {
                vlan_ptr = _kt_ipmc_repl_next_free_ptr(unit, head);
                head = 0;
            } else {
                vlan_ptr = _tr2_ipmc_repl_next_free_ptr(unit);
            }
            if (last_vlan_ptr > 0) {
                /* Write previous non-zero entry */
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                        &vlan_entry, NEXTPTRf,
                        (vlan_ptr > 0) ? vlan_ptr : last_vlan_ptr);
                /* If vlan_ptr <= 0, terminate so
                             * later cleanup can remove the chain. */
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                        MEM_BLOCK_ALL, last_vlan_ptr, &vlan_entry));
                last_write_ptr = last_vlan_ptr;
                if (vlan_ptr < 0) {
                    _bcm_tr2_repl_list_free(unit, *start_index);
                    return BCM_E_RESOURCE;
                }
            } else {
                if (vlan_ptr < 0) {
                    return BCM_E_RESOURCE;
                }
                *start_index = vlan_ptr;
            }
            sal_memset(&vlan_entry, 0, sizeof(vlan_entry));
            if (repl_type == 1) {
                /* Replication is over next hops */
                msb = ms_bit + nh_offset / 64;
            } else {
                /* Replication is over interfaces */
                msb = ms_bit;
            }
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                               MSB_VLANf, msb);
            soc_MMU_IPMC_VLAN_TBLm_field_set(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);
            IPMC_REPL_VE_USED_SET(unit, vlan_ptr);
            vlan_count +=
                _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
            last_vlan_ptr = vlan_ptr;
        }
    }

    if (last_vlan_ptr > 0) {
        /* Write final entry */
        if (soc_feature(unit, soc_feature_ipmc_reduced_table_size) &&
            vlan_count == 1) {
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry, 
                                               NEXTPTRf, 1);
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry, 
                                               LASTf, 1);
			
        } else {
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry, 
                                               NEXTPTRf, last_vlan_ptr);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                   MEM_BLOCK_ALL, last_vlan_ptr, &vlan_entry));

        if (last_write_ptr > 0) {
            soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);
            if ((_shr_popcount(ls_bits[0]) +
                 _shr_popcount(ls_bits[1])) == 1) {
                /* Mark previous link with "last" flag */
                SOC_IF_ERROR_RETURN
                    (READ_MMU_IPMC_VLAN_TBLm(unit,
                          MEM_BLOCK_ANY, last_write_ptr, &vlan_entry));
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                                   LASTf, 1);
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                          MEM_BLOCK_ALL, last_write_ptr, &vlan_entry));
            }
        }
    }

    *count = vlan_count;
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_tr2_ipmc_egress_intf_set
 * Purpose:
 *	Assign set of egress interfaces to port's replication list for chosen
 *	IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	ipmc_id  - The index number.
 *	port     - port to list.
 *	if_count - number of interfaces in replication list.
 *      if_array - (IN) List of interface numbers.
 *      check_port - (IN) If if_array consists of L3 interfaces, this parameter 
 *                        controls whether to check the given port is a member
 *                        in each L3 interface's VLAN. This check should be  
 *                        disabled when not needed, in order to improve
 *                        performance.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_tr2_ipmc_egress_intf_set(int unit, int ipmc_id, bcm_port_t port,
                            int if_count, bcm_if_t *if_array, int check_port)
{
    int			rv = BCM_E_NONE;
    SHR_BITDCL          *intf_vec = NULL;
    int			list_start_ptr, prev_start_ptr;
    int                 alloc_size, repl_hash, vlan_count, last_flag;
    int                 if_num, partition;
    int                 intf_count = 0;
    int                 nh_count = 0;
    bcm_l3_intf_t       l3_intf;
    _bcm_repl_list_info_t *rli_start, *rli_current, *rli_prev;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if ((uint32)if_count > IPMC_REPL_INTF_TOTAL(unit)) {
        return BCM_E_PARAM;
    }

    /* Partition between L3 interfaces and NHIs in the intf_vec
     * intf_vec = [L3 interfaces, NHIs] */
    partition = soc_mem_index_count(unit, EGR_L3_INTFm);

    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(alloc_size, "IPMC repl interface vector");
    if (intf_vec == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(intf_vec, 0, alloc_size);

    IPMC_REPL_LOCK(unit);
    if ((if_count == 0) && 
        IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) == 0) {
        goto intf_set_done;
    }

    /* Interface validation and vector construction */
    for (if_num = 0; if_num < if_count; if_num++) {
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num]) && 
            (uint32)(if_array[if_num]) > partition) {
            rv = BCM_E_PARAM;
            goto intf_set_done;
        }
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num])) {
            /* L3 interface is used */
            if (check_port) {
                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = if_array[if_num];
                if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                    goto intf_set_done;
                }
            }
            SHR_BITSET(intf_vec, if_array[if_num]);
            intf_count++;
        } else {
            /* Next hop is used */
            SHR_BITSET(intf_vec, partition + if_array[if_num] - 
                       BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
            nh_count++;
        }
    }

    /* Check previous group pointer */
    if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port,
                                &prev_start_ptr, 0, 0)) < 0) {
        goto intf_set_done;
    }

    /* Search for list already in table */
    rli_start = IPMC_REPL_LIST_INFO(unit);

    repl_hash =
        _shr_crc32b(0, (uint8 *)intf_vec, IPMC_REPL_INTF_TOTAL(unit));

    for (rli_current = rli_start; rli_current != NULL;
         rli_current = rli_current->next) {
        if (repl_hash == rli_current->hash) {
            rv = _bcm_tr2_repl_list_compare(unit, rli_current->index,
                                           intf_vec);
            if (rv == BCM_E_NOT_FOUND) {
                continue; /* Not a match */
            } else if (rv < 0) {
                goto intf_set_done; /* Access error */
            } else {
                break; /* Match */
            }
        }
    }

    if (rli_current != NULL) {
        /* Found a match, point to here and increase reference count */
        if (prev_start_ptr == rli_current->index) {
            /* We're already pointing to this list, so done */
            rv = BCM_E_NONE;
            goto intf_set_done;
        } else {
            list_start_ptr = rli_current->index;
            vlan_count = rli_current->list_size;
        }
    } else {
        /* Not a match, make a new chain */
        if (!SOC_IS_KATANAX(unit)) {
            if ((rv = _bcm_tr2_repl_list_write(unit, &list_start_ptr,
                                             &vlan_count, intf_vec)) < 0) {
                goto intf_set_done;
            }
        } else {
            if ((rv = _bcm_kt_repl_list_write(unit, &list_start_ptr,
                                             &vlan_count, intf_vec,
                                             intf_count, nh_count)) < 0) {
                goto intf_set_done;
            }
        }

        if (vlan_count > 0) {
            /* Update data structures */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC repl list info");
            if (rli_current == NULL) {
                /* Release list */
                _bcm_tr2_repl_list_free(unit, list_start_ptr);
                rv = BCM_E_MEMORY;
                goto intf_set_done;
            }
            sal_memset(rli_current, 0, alloc_size);
            rli_current->index = list_start_ptr;
            rli_current->hash = repl_hash;
            rli_current->next = rli_start;
            rli_current->list_size = vlan_count;
            IPMC_REPL_LIST_INFO(unit) = rli_current;
            rli_start = rli_current;
        }
    }

    last_flag = (vlan_count == 1);

    if (vlan_count > 0) {
        if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port,
                                    &list_start_ptr, last_flag, 1)) < 0) {
            if (rli_current->refcount == 0) {
                /* This was new */
                _bcm_tr2_repl_list_free(unit, list_start_ptr);
                IPMC_REPL_LIST_INFO(unit) = rli_current->next;
                sal_free(rli_current);
            }
            goto intf_set_done;
        }

        (rli_current->refcount)++;
        /* we don't need this rli_current anymore */
    } else if (prev_start_ptr != 0) {
        list_start_ptr = 0;
        if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port,
                                    &list_start_ptr, last_flag, 1)) < 0) {
            goto intf_set_done;
        }
    }
    IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) = vlan_count;

    if (prev_start_ptr != 0) {
        rli_prev = NULL;
        for (rli_current = rli_start; rli_current != NULL;
             rli_current = rli_current->next) {
            if (prev_start_ptr == rli_current->index) {
                (rli_current->refcount)--;
                if (rli_current->refcount == 0) {
                    /* Free these linked list entries */
                    rv = _bcm_tr2_repl_list_free(unit, prev_start_ptr);
                    /* If we have an error, we'll fall out anyway */
                    if (rli_prev == NULL) {
                        IPMC_REPL_LIST_INFO(unit) = rli_current->next;
                    } else {
                        rli_prev->next = rli_current->next;
                    }
                    sal_free(rli_current);
                }
                break;
            }
            rli_prev = rli_current;
        }
    }

 intf_set_done:
    IPMC_REPL_UNLOCK(unit);
    if (intf_vec != NULL) {
        sal_free(intf_vec);
    }
    return rv;
}

/*
 * Function:
 *	bcm_tr2_ipmc_egress_intf_get
 * Purpose:
 *	Retreieve set of egress interfaces in port's replication list
 *      for chosen IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	ipmc_id  - The index number.
 *	port     - port to list.
 *	if_max   - maximum number of interfaces in replication list.
 *      if_array - (OUT) List of interface numbers.
 *	if_count - (OUT) number of interfaces returned in replication list.
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *      If the input parameter if_max = 0, return in the output parameter
 *      if_count the total number of interfaces in the specified multicast 
 *      group's replication list.
 */

int
bcm_tr2_ipmc_egress_intf_get(int unit, int ipmc_id, bcm_port_t port,
                            int if_max, bcm_if_t *if_array, int *if_count)
{
    int			rv = BCM_E_NONE;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int                 vlan_ptr, last_vlan_ptr;
    uint32              vlan_count;
    int                 ls_pos, ls_len;
    int                 rvth = BCM_E_NONE;
    int                 intf_base, nh_offset;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (if_max < 0) {
        return BCM_E_PARAM;
    }

    IPMC_REPL_LOCK(unit);
    if (IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) == 0) {
        *if_count = 0;
        IPMC_REPL_UNLOCK(unit);
        return rvth;
    }

    if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto intf_get_done;
    }
    last_vlan_ptr = -1;
    vlan_count = 0;
    ls_len = soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm, LSB_VLAN_BMf);
    if (SOC_IS_TD_TT(unit)) {
        nh_offset = TD_EGR_L3_NEXT_HOP_OFFSET;
    } else {
        /* coverity[large_shift : FALSE] */
        nh_offset = (1 << (soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm,
                                                MSB_VLANf) - 1)) * ls_len;
    }
    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto intf_get_done;
        }
        /* Each MSB represents 64 entries in LSB bitmap */
        intf_base = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit, &vlan_entry,
                                                       MSB_VLANf) * ls_len;
        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                         LSB_VLAN_BMf, ls_bits);
        /* Using MSB, check if L3 interface or next hop */
        
        for (ls_pos = 0; ls_pos < ls_len; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                if (if_max == 0) {
                    vlan_count++;
                } else {
                    /* Check if L3 interface or next hop */
                    if (intf_base >= nh_offset) {
                        /* Entry contains next hops */
                        if_array[vlan_count++] = intf_base - nh_offset +
                            ls_pos + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                    } else {
                        /* Entry contains interfaces */
                        if_array[vlan_count++] = intf_base + ls_pos;
                    }
                    if (vlan_count == (uint32)if_max) {
                        *if_count = vlan_count;
                        goto intf_get_done;
                    }
                }
            }
        }
        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                      &vlan_entry, NEXTPTRf);

        if (vlan_count >= IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
            break;
        }
    }

    *if_count = vlan_count;

 intf_get_done:
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_add
 * Purpose:
 *      Add VLAN to selected ports' replication list for chosen
 *      IPMC group.
 *Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to add.
 *      vlan     - VLAN to replicate.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_ipmc_repl_add(int unit, int ipmc_id, bcm_port_t port,
                     bcm_vlan_t vlan)
{
    int alloc_size, intf_max, if_count, rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    pbmp_t pbmp, ubmp;
    bcm_l3_intf_t l3_intf;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    /* Check if port belongs to this VLAN */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, vlan, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    if (bcm_esw_l3_intf_find_vlan(unit, &l3_intf) < 0) {
        return BCM_E_PARAM;
    }

    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        if (if_count < intf_max) {
            if_array[if_count++] = l3_intf.l3a_intf_id;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array, FALSE);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_delete
 * Purpose:
 *      Remove VLAN from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to remove.
 *      vlan     - VLAN to delete from replication.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_delete(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_t vlan)
{
    int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    bcm_l3_intf_t       l3_intf;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_vid = vlan;
    if (bcm_esw_l3_intf_find_vlan(unit, &l3_intf) < 0) {
        return BCM_E_PARAM;
    }


    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == l3_intf.l3a_intf_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array, FALSE);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }
    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_set
 * Purpose:
 *      Assign set of VLANs provided to port's replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to list.
 *      vlan_vec - (IN) vector of replicated VLANs common to selected ports.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_set(int unit, int ipmc_id, bcm_port_t port,
                     bcm_vlan_vector_t vlan_vec)
{
    int rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    bcm_l3_intf_t l3_intf;
    pbmp_t pbmp, ubmp;
    int  intf_num, intf_max, alloc_size, vid;

    if (!SOC_IPMCREPLSHR_GET(unit) &&
        !soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        return BCM_E_CONFIG;
    }

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(if_array, 0, alloc_size);
    intf_num = 0;
    for (vid = BCM_VLAN_MIN; vid < BCM_VLAN_MAX; vid++) {
        if (BCM_VLAN_VEC_GET(vlan_vec, vid)) {
            rv = bcm_esw_vlan_port_get(unit, vid, &pbmp, &ubmp);
            if (BCM_FAILURE(rv)) {
                sal_free(if_array);
                return rv;
            }
            if (!BCM_PBMP_MEMBER(pbmp, port)) {
                sal_free(if_array);
                return BCM_E_PARAM;
            }
            bcm_l3_intf_t_init(&l3_intf);
            l3_intf.l3a_vid = vid;
            if ((rv = bcm_esw_l3_intf_find_vlan(unit, &l3_intf)) < 0) {
                sal_free(if_array);
                return rv;
            }
            if_array[intf_num++] = l3_intf.l3a_intf_id;
        }
    }

    rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                     intf_num, if_array, FALSE);

    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_get
 * Purpose:
 *      Return set of VLANs selected for port's replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to list.
 *      vlan_vec - (OUT) vector of replicated VLANs common to selected ports.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_get(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_vector_t vlan_vec)
{
    int                 rv = BCM_E_NONE;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    bcm_l3_intf_t       l3_intf;
    bcm_l3_egress_t     egress_object;
    int                 vlan_ptr, last_vlan_ptr;
    uint32              vlan_count;
    int                 ls_pos, ls_len, nh_idx;
    int                 rvth = BCM_E_NONE;
    int                 intf_base, nh_offset;

    IPMC_REPL_INIT(unit);

    IPMC_REPL_ID(unit, ipmc_id);

    REPL_PORT_CHECK(unit, port);

    BCM_VLAN_VEC_ZERO(vlan_vec);

    IPMC_REPL_LOCK(unit);
    if (IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) == 0) {
        IPMC_REPL_UNLOCK(unit);
        return rvth;
    }

    if ((rv = _tr2_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto get_done;
    }
    last_vlan_ptr = -1;
    vlan_count = 0;
    ls_len = soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm, LSB_VLAN_BMf);
    if (SOC_IS_TD_TT(unit)) {
        nh_offset = TD_EGR_L3_NEXT_HOP_OFFSET;
    } else {
        /* coverity[large_shift : FALSE] */
        nh_offset = (1 << (soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm,
                                                MSB_VLANf) - 1)) * ls_len;
    }
    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto get_done;
        }
        /* Each MSB represents 64 entries in LSB bitmap */
        intf_base = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit, &vlan_entry,
                                                       MSB_VLANf) * ls_len;
        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                         LSB_VLAN_BMf, ls_bits);
        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                      &vlan_entry, NEXTPTRf);

        /* Translate the HW L3 interfaces into the corresponding VLANs.
         * Note, this may result in some loss of information.
         */
        for (ls_pos = 0; ls_pos < ls_len; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                bcm_l3_intf_t_init(&l3_intf);
                if (intf_base >= nh_offset) {
                    /* Next hop */
                    nh_idx = intf_base - nh_offset + ls_pos +
                        BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                    if ((rv = bcm_esw_l3_egress_get(unit, nh_idx,
                                                    &egress_object)) < 0) {
                        goto get_done;
                    }
                    /* Coverity: egress_object.intf is initialized above
                     * by bcm_esw_l3_egress_get. */
                    /* coverity[uninit_use : FALSE] */
                    l3_intf.l3a_intf_id = egress_object.intf;
                } else {
                    /* L3 Interface */
                    l3_intf.l3a_intf_id = intf_base + ls_pos;
                }
                if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                    goto get_done;
                }
                BCM_VLAN_VEC_SET(vlan_vec, l3_intf.l3a_vid);
                vlan_count++;
            }
        }
        if (vlan_count >= IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
            break;
        }
    }

 get_done:
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_delete_all
 * Purpose:
 *      Remove all VLANs from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The MC index number.
 *      port     - port from which to remove VLANs.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tr2_ipmc_repl_delete_all(int unit, int ipmc_id, bcm_port_t port)
{
    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        /* Nothing to do */
        return BCM_E_NONE;
    }

    return bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port, 0, NULL, FALSE);
}

/*
 * Function:
 *      bcm_tr2_ipmc_egress_intf_add
 * Purpose:
 *      Add L3 interface to selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to add.
 *      l3_intf  - L3 interface to replicate.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                            bcm_l3_intf_t *l3_intf)
{
    bcm_if_t *if_array = NULL;
    int  intf_num, intf_max, alloc_size, rv = BCM_E_NONE;
    pbmp_t pbmp, ubmp;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    /* Check if port belongs to this VLAN */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, l3_intf->l3a_vid, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &intf_num);
    if (BCM_SUCCESS(rv)) {
        if (intf_num < intf_max) {
            if_array[intf_num++] = l3_intf->l3a_intf_id;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             intf_num, if_array, FALSE);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_egress_intf_delete
 * Purpose:
 *      Remove L3 interface from selected ports' replication list for chosen
 *      IPMC group.
 * Parameters:
 *      unit     - StrataSwitch PCI device unit number.
 *      ipmc_id  - The index number.
 *      port     - port to remove.
 *      l3_intf  - L3 interface to delete from replication.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                               bcm_l3_intf_t *l3_intf)
{
    bcm_if_t *if_array = NULL;
    int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }


    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_tr2_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == l3_intf->l3a_intf_id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = bcm_tr2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array, FALSE);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_availability_get
 * Purpose:
 *      Get the percentage of IPMC repilication table that's free.
 * Parameters:
 *      unit - (IN) Device Number
 *      available_percent - (OUT) Available percentage
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_ipmc_repl_availability_get(int unit, int *available_percent)
{
    int                 ix, bit;
    SHR_BITDCL          not_ptrs;
    int                 num_available_entries;

    num_available_entries = 0;
    for (ix = 0; ix < _SHR_BITDCLSIZE(IPMC_REPL_TOTAL(unit)); ix++) {
        not_ptrs = ~_tr2_repl_info[unit]->bitmap_entries_used[ix];
        if (not_ptrs) {
            for (bit = 0; bit < SHR_BITWID; bit++) {
                if (not_ptrs & (1 << bit)) {
                    num_available_entries++;
                }
            }
        }
    }

    *available_percent = (100 * num_available_entries) / IPMC_REPL_TOTAL(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_ipmc_repl_list_split
 * Purpose:
 *      Split the list of encap ids into sublists that match existing
 *      replication lists. The sublists do not overlap with each other.
 * Parameters:
 *      unit - (IN) Device Number
 *      array_size       - (IN) Number of valid elements in encap_id_array and
 *                              sublist_id_array
 *      encap_id_array   - (IN) Array of encap ids
 *      sublist_id_array - (OUT) Sublist id each encap id is assigned to
 *      sublist_max   - (IN)  Max number of sublists allowed
 *      sublist_count - (OUT) Number of sublists of used
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_ipmc_repl_list_split(int unit, int array_size,
                    bcm_if_t *encap_id_array, int *sublist_id_array, 
                    int sublist_max, int *sublist_count)
{
    int rv = BCM_E_NONE;
    int alloc_size;
    SHR_BITDCL *intf_vec = NULL;
    SHR_BITDCL *subset_intf_vec = NULL;
    int partition;
    int i;
    int sublist_id;
    int intf_vec_size, subset_size;
    _bcm_repl_list_info_t *rli_current = NULL;
    int vlan_ptr, last_vlan_ptr;
    int is_subset;
    uint32 ms_bit, hi_bit;
    uint32 ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int ls_pos;
    bcm_if_t encap_id;

    /* Input parameter check */

    if (array_size <= 0) {
       return BCM_E_PARAM;
    }

    if ((NULL == encap_id_array) || (NULL == sublist_id_array)) {
       return BCM_E_PARAM;
    }

    if (sublist_max <= 0) {
        return BCM_E_PARAM;
    }

    IPMC_REPL_LOCK(unit);

    /* Construct interface vector from encap_id_array */

    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(alloc_size, "IPMC repl interface vector");
    if (intf_vec == NULL) {
        rv = BCM_E_MEMORY;
        goto done;
    }
    sal_memset(intf_vec, 0, alloc_size);

    /* Partition between L3 interfaces and NHIs in the intf_vec
     * intf_vec = [L3 interfaces, NHIs] */
    partition = soc_mem_index_count(unit, EGR_L3_INTFm);

    for (i = 0; i < array_size; i++) {
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, encap_id_array[i]) && 
            (uint32)(encap_id_array[i]) > partition) {
            rv = BCM_E_PARAM;
            goto done;
        }
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, encap_id_array[i])) {
            /* L3 interface is used */
            SHR_BITSET(intf_vec, encap_id_array[i]);
        } else {
            /* Next hop is used */
            SHR_BITSET(intf_vec, partition + encap_id_array[i] - 
                       BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
        }
    }

    /* Traverse existing replication lists to find those that match subsets of
     * the interface vector. As matching subsets are found, they are removed 
     * from the interface vector. 
     */

    subset_intf_vec = sal_alloc(alloc_size, "Subset IPMC repl interface vector");
    if (subset_intf_vec == NULL) {
        rv = BCM_E_MEMORY;
        goto done;
    }
    sal_memset(subset_intf_vec, 0, alloc_size);

    sublist_id = 0;
    SHR_BITCOUNT_RANGE(intf_vec, intf_vec_size, 0, IPMC_REPL_INTF_TOTAL(unit));

    for (rli_current = IPMC_REPL_LIST_INFO(unit);
            rli_current != NULL;
            rli_current = rli_current->next) {

        if (rli_current->list_size == 0) {
            continue;
        }

        if (rli_current->list_size > intf_vec_size) {
            /* If the current list's size is greater than the number of
             * set bits in interface vector, the current list cannot be
             * a subset.
             */
            continue;
        }

        vlan_ptr = rli_current->index;
        last_vlan_ptr = -1;
        /* coverity[large_shift : FALSE] */
        hi_bit = (1 << (soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm,
                        MSB_VLANf) - 1));
        is_subset = TRUE;
        sal_memset(subset_intf_vec, 0, alloc_size);
        while (vlan_ptr != last_vlan_ptr) {
            rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                            vlan_ptr, &vlan_entry);
            if (BCM_FAILURE(rv)) {
                goto done;
            }

            ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                    &vlan_entry, MSB_VLANf);
            soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                    LSB_VLAN_BMf, ls_bits);

            for (ls_pos = 0; ls_pos < 64; ls_pos++) {
                if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                    /* Using MSB, check if L3 interface or next hop */
                    if (ms_bit & hi_bit) {
                        /* Entry contains next hops */
                        encap_id = ((ms_bit & (hi_bit - 1)) * 64) + ls_pos +
                            BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                        if (SHR_BITGET(intf_vec, partition + encap_id - 
                                    BCM_XGS3_DVP_EGRESS_IDX_MIN(unit))) {
                            SHR_BITSET(subset_intf_vec, partition + encap_id - 
                                    BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
                        } else {
                            /* Not found in interface vector, 
                             * the current list cannot be a subset.
                             */ 
                            is_subset = FALSE;
                            break;
                        } 
                    } else {
                        /* Entry contains L3 interfaces */
                        encap_id = (ms_bit * 64) + ls_pos;
                        if (SHR_BITGET(intf_vec, encap_id)) {
                            SHR_BITSET(subset_intf_vec, encap_id);
                        } else {
                            /* Not found in interface vector, 
                             * the current list cannot be a subset.
                             */ 
                            is_subset = FALSE;
                            break;
                        }
                    }
                }
            }
            if (!is_subset) {
                break;
            }

            last_vlan_ptr = vlan_ptr;
            vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                    &vlan_entry, NEXTPTRf);
        }

        if (is_subset) {
            /* Make sure subset_intf_vec is not all zeroes */
            SHR_BITCOUNT_RANGE(subset_intf_vec, subset_size, 0,
                    IPMC_REPL_INTF_TOTAL(unit));
            if (subset_size > 0) {
                /* Set sublist id for those encap ids in encap_id_array that
                 * belong to the subset.
                 */
                for (i = 0; i < array_size; i++) {
                    if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, encap_id_array[i])) {
                        /* L3 interface is used */
                        if (SHR_BITGET(subset_intf_vec, encap_id_array[i])) {
                            sublist_id_array[i] = sublist_id;
                        }
                    } else {
                        /* Next hop is used */
                        if (SHR_BITGET(subset_intf_vec, partition + 
                                    encap_id_array[i] -
                                    BCM_XGS3_DVP_EGRESS_IDX_MIN(unit))) {
                            sublist_id_array[i] = sublist_id;
                        }
                    }
                }

                /* Update sublist id */
                sublist_id++;
                if (sublist_id == sublist_max) {
                    break;
                }

                /* Remove the found subset from interface vector */
                for (i = 0; i < IPMC_REPL_INTF_TOTAL(unit); i++) {
                    if (SHR_BITGET(subset_intf_vec, i)) {
                        SHR_BITCLR(intf_vec, i);
                        intf_vec_size--;
                        subset_size--;
                        if (subset_size == 0) {
                            break;
                        }
                    }
                }
                if (intf_vec_size == 0) {
                    break;
                }
            }
        }
    }

    if (NULL != sublist_count) {
        *sublist_count = sublist_id;
    }

done:
    if (intf_vec) {
        sal_free(intf_vec);
    }
    if (subset_intf_vec) {
        sal_free(subset_intf_vec);
    }
    IPMC_REPL_UNLOCK(unit);
    return rv;
}
#ifdef BCM_KATANA2_SUPPORT
/*
 * Function:
 *      bcm_kt2_ipmc_repl_init
 * Purpose:
 *      Initialize IPMC replication.
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int bcm_kt2_ipmc_repl_init(int unit) 
{
    int    alloc_size;
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
    bcm_port_t port;
    bcm_pbmp_t            pbmp_all;

    BCM_PBMP_CLEAR(pbmp_all);
    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &pbmp_all);
    }
    BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update (unit, &pbmp_all));

    /* Allocate struct for IPMC Extq replication bookkeeping */
    alloc_size = sizeof(_kt_extq_repl_info_t);
    _kt_extq_repl_info[unit] = sal_alloc(alloc_size, "IPMC Extq repl info");
    if (_kt_extq_repl_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_kt_extq_repl_info[unit], 0, alloc_size);
    _kt_extq_repl_info[unit]->ipmc_size = soc_mem_index_count(unit, L3_IPMCm);

    IPMC_EXTQ_REPL_MAX(unit) = (1 << soc_mem_field_length(unit, L3_IPMCm,
                                EXT_Q_NUM_COPIESf)) - 1;
    IPMC_EXTQ_TOTAL(unit) = 4096;
    IPMC_EXTQ_LIST_TOTAL(unit) = soc_mem_index_count(unit, MMU_EXT_MC_QUEUE_LIST0m);
    alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_LIST_TOTAL(unit));
    _kt_extq_repl_info[unit]->bitmap_entries_used =
    sal_alloc(alloc_size, "IPMC Extq repl entries used");
    if (_kt_extq_repl_info[unit]->bitmap_entries_used == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_kt_extq_repl_info[unit]->bitmap_entries_used, 0, alloc_size);

    /* Always reserve slot 0 */
    IPMC_EXTQ_REPL_VE_USED_SET(unit, 0);

    alloc_size = sizeof(_kt_repl_queue_info_t);
    _kt_extq_repl_info[unit]->mcgroup_info =
        sal_alloc(alloc_size, "IPMC MC group info");
    if (_kt_extq_repl_info[unit]->mcgroup_info == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_kt_extq_repl_info[unit]->mcgroup_info, 0, alloc_size);

    alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
    IPMC_EXTQ_GROUP_INFO(unit)->queue_count =
        sal_alloc(alloc_size, "IPMC Extq queue count");
    if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count,
               0, alloc_size);

    IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int =
        sal_alloc(alloc_size, "IPMC Extq queue count internal");
    if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count_int,
               0, alloc_size);

    IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext =
        sal_alloc(alloc_size, "IPMC Extq queue count external");
    if (IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(IPMC_EXTQ_GROUP_INFO(unit)->queue_count_ext,
               0, alloc_size);

    sw_to_hw_queue[unit] =
      sal_alloc(IPMC_EXTQ_TOTAL(unit) * sizeof(uint32),
                "SW to HW queue");
    if (sw_to_hw_queue[unit] == NULL) {
        bcm_kt_ipmc_repl_detach(unit);
        return (BCM_E_MEMORY);
    }
    sal_memset(sw_to_hw_queue[unit], 0,
               IPMC_EXTQ_TOTAL(unit) * sizeof(uint32));

    PBMP_ITER(pbmp_all, port) {
        alloc_size = sizeof(_kt_repl_queue_info_t);
        IPMC_EXTQ_REPL_PORT_INFO(unit, port) =
            sal_alloc(alloc_size, "IPMC Extq repl port info");
        if (IPMC_EXTQ_REPL_PORT_INFO(unit, port) == NULL) {
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port), 0, alloc_size);

        alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
        IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count =
            sal_alloc(alloc_size, "IPMC Extq repl port queue count");
        if (IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count == NULL) {
            bcm_kt_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count,
                   0, alloc_size);
    }

    /* reserve ipmc index 0 & 1 for extended queue work around */
    BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 0));
    BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, 1));

        /* Read L3 ipmc table. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY,
                                 0, entry));
        /* use 1 as initial redirection pointer */
    soc_mem_field32_set(unit, L3_IPMCm, entry,
                                MMU_MC_REDIRECTION_PTRf, 1);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL,
                                0, &entry));
    return BCM_E_NONE;
}

#endif
#ifdef BCM_KATANA_SUPPORT
/*
 * Function:
 *      _bcm_kt_init_check
 * Purpose:
 *      Katana initialization
 *      unit         - StrataSwitch PCI device unit number.
 *      ipmc_id      - The replication group number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt_init_check(int unit, int ipmc_id)
{
    if (SOC_IS_KATANA(unit)) {
       IPMC_REPL_INIT(unit);
       IPMC_REPL_ID(unit, ipmc_id);
    } 
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
       BCM_IF_ERROR_RETURN(_bcm_kt2_init_check(unit, ipmc_id));
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_kt_ipmc_repl_detach
 * Purpose:
 *      Initialize IPMC replication.
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_ipmc_repl_detach(int unit)
{
    _bcm_repl_list_info_t *rli_current, *rli_free;
    bcm_port_t port;
    bcm_pbmp_t            pbmp_all;

    BCM_PBMP_CLEAR(pbmp_all);
    BCM_PBMP_ASSIGN(pbmp_all, PBMP_ALL(unit));

#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &pbmp_all);
    }
    if (SOC_IS_KATANA2(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update (unit, &pbmp_all));
    }
#endif

    if (_kt_extq_repl_info[unit] != NULL) {
         PBMP_ITER(pbmp_all, port) {
            if (_kt_extq_repl_info[unit]->port_info[port] != NULL) {
                if (_kt_extq_repl_info[unit]->port_info[port]->queue_count
                    != NULL) {
                    sal_free(
                        _kt_extq_repl_info[unit]->port_info[port]->queue_count);
                }
                sal_free(_kt_extq_repl_info[unit]->port_info[port]);
            }
        }

        if (_kt_extq_repl_info[unit]->mcgroup_info != NULL) {
            if (_kt_extq_repl_info[unit]->mcgroup_info->queue_count 
                                            != NULL) {
                sal_free(_kt_extq_repl_info[unit]->
                         mcgroup_info->queue_count);
            }
            if (_kt_extq_repl_info[unit]->mcgroup_info->queue_count_int
                                            != NULL) {
                sal_free(_kt_extq_repl_info[unit]->
                         mcgroup_info->queue_count_int);
            }
            if (_kt_extq_repl_info[unit]->mcgroup_info->queue_count_ext
                                            != NULL) {
                sal_free(_kt_extq_repl_info[unit]->
                         mcgroup_info->queue_count_ext);
            }
            sal_free(_kt_extq_repl_info[unit]->mcgroup_info);
        }

        if (_kt_extq_repl_info[unit]->bitmap_entries_used != NULL) {
            sal_free(_kt_extq_repl_info[unit]->bitmap_entries_used);
        }

        if (_kt_extq_repl_info[unit]->repl_extq_list_info != NULL) {
            rli_current = IPMC_EXTQ_REPL_LIST_INFO(unit);
            while (rli_current != NULL) {
                rli_free = rli_current;
                rli_current = rli_current->next;
                sal_free(rli_free);
            }
        }

        sal_free(_kt_extq_repl_info[unit]);
        _kt_extq_repl_info[unit] = NULL;
    }

    if (sw_to_hw_queue[unit] != NULL) {
        sal_free(sw_to_hw_queue[unit]);
        sw_to_hw_queue[unit] = NULL;
    }

    return BCM_E_NONE;
}


int
_bcm_kt_ipmc_mmu_mc_remap_ptr(int unit, int ipmc_id, 
                        int *remap_id, int set)
{
    int rv = BCM_E_NONE;
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t mem = L3_IPMCm;

    soc_mem_lock(unit, mem);
    
    if ((rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     ipmc_id, entry)) < 0) {
        soc_mem_unlock(unit, mem);
           return rv;
    }                                 

    if (set) {
        soc_mem_field32_set(unit, mem, entry, 
                           MMU_MC_REDIRECTION_PTRf, *remap_id);
       if ((rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, ipmc_id,
                                entry)) < 0) {
            soc_mem_unlock(unit, mem);
            return rv;
        }        
    } else {     
        *remap_id = soc_mem_field32_get(unit, mem, entry, 
                                        MMU_MC_REDIRECTION_PTRf);
        if (*remap_id == 0) {
            *remap_id = ipmc_id;
        }
    }    

    soc_mem_unlock(unit, mem);
    
    return rv;
}

int 
_bcm_kt_ipmc_update_group(int unit, soc_mem_t mem, int ipmc_id,
                          mmu_ipmc_group_tbl0_entry_t *entry)
{
    int rv = BCM_E_NONE;
    _rep_regs_t *rqe_arr_ptr;
    int port, port_ptr = 0;

    rqe_arr_ptr = kt_rqe_rep_regs;
    for (port = 0; port < 36; port++) {
        if (rqe_arr_ptr[port].mem == mem) {
            port_ptr = soc_mem_field32_get(unit, mem, entry, 
                                           rqe_arr_ptr[port].port_ptr);
            if (port_ptr) {
                /* encode new port_ptr as { 2'b01, MC_INDEX[11:0] } */
                soc_mem_field32_set(unit, mem, entry, rqe_arr_ptr[port].port_ptr, 
                                    ((ipmc_id & 0xFFF) | (0x1 << 12)));
            }
        }
    }

    return rv;
}

int _bcm_kt_ipmc_port_ext_queue_count(int unit,
                                     int ipmc_id, bcm_port_t port)
{
    if (SOC_PORT_VALID(unit, port)) {
        return IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(unit, port, ipmc_id);
    } else {
        return BCM_E_PORT;
    }
}

void _bcm_kt_ipmc_port_ext_queue_count_increment(int unit,
                                     int ipmc_id, bcm_port_t port)
{
    if ((IPMC_EXTQ_REPL_PORT_INFO(unit, port) != NULL) &&
        (IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count != NULL)) {
        IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(unit, port, ipmc_id)++;
    }
    if (IS_EXT_MEM_PORT(unit, port)) {
        IPMC_EXTQ_REPL_QUEUE_COUNT_EXT(unit, ipmc_id)++;
    } else {
        IPMC_EXTQ_REPL_QUEUE_COUNT_INT(unit, ipmc_id)++;
    }
    return;
}

void _bcm_kt_ipmc_port_ext_queue_count_decrement(int unit,
                                     int ipmc_id, bcm_port_t port)
{
    if ((IPMC_EXTQ_REPL_PORT_INFO(unit, port) != NULL) &&
        (IPMC_EXTQ_REPL_PORT_INFO(unit, port)->queue_count != NULL)) {
        IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(unit, port, ipmc_id)--;
    }
    if (IS_EXT_MEM_PORT(unit, port)) {
        IPMC_EXTQ_REPL_QUEUE_COUNT_EXT(unit, ipmc_id)--;
    } else {
        IPMC_EXTQ_REPL_QUEUE_COUNT_INT(unit, ipmc_id)--;
    }
    return;
}

int _bcm_kt2_qflush_warfp_action_add(int unit, int entry_id, bcm_port_t port, uint8 int_pri) {
    int rv = 0, rv1 = 0;
    uint32 param0, param1 = 0;
    bcm_pbmp_t pbmp, get_pbmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    rv = bcm_esw_field_action_ports_get(unit, entry_id, bcmFieldActionEgressPortsAdd, &get_pbmp);
    if ((rv ==0) && (BCM_PBMP_NEQ(pbmp, get_pbmp))) {
        rv1 = bcm_esw_field_action_remove(unit, entry_id, bcmFieldActionEgressPortsAdd);
        if (rv1 < 0) {
           return rv1;
        }
        rv1 = bcm_esw_field_action_ports_add(unit, entry_id, bcmFieldActionEgressPortsAdd, pbmp);
        if (rv1 < 0) {
           return rv1;
        }
    }
    if (rv != 0) {
        rv1 = bcm_esw_field_action_ports_add(unit, entry_id, bcmFieldActionEgressPortsAdd, pbmp);
        if (rv1 < 0) {
           return rv1;
        }
    }

    rv = bcm_esw_field_action_get(unit,entry_id, bcmFieldActionPrioIntNew, &param0, &param1);
    if (rv == 0) {
        rv1 = bcm_esw_field_action_remove(unit, entry_id, bcmFieldActionPrioIntNew);
        if (rv1 < 0) {
           return rv1;
        }

    }
    param0 = int_pri;
    param1 = 0;
    rv = bcm_esw_field_action_add(unit, entry_id, bcmFieldActionPrioIntNew, param0, param1);
    if (rv < 0) {
        return rv;
    }
    rv = bcm_esw_field_entry_install(unit, entry_id);
    if (rv < 0) {
        return rv;
    }

    return BCM_E_NONE;
}

int _bcm_kt2_qflush_war(int unit)
{
    int i;
    int rv = 0;
    int ifp_entry_id = 0;
    bcm_pkt_t  *tx_pkt;
    bcm_port_t  port = 24;
    bcm_l2_addr_t l2addr;
    bcm_mac_t dest = {0x00, 0x00, 0x5E, 0x00, 0x00, 0xFF};

    ifp_entry_id  = soc_property_get(unit, spn_QUEUE_FLUSH_WAR_FP_ENTRYID, 0);
    if (ifp_entry_id == 0) {
        return BCM_E_PARAM;
    }
    bcm_l2_addr_t_init(&l2addr, dest, 1);
    l2addr.port = 24;
    l2addr.flags = BCM_L2_STATIC | BCM_L2_DISCARD_DST;
    BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_add(unit, &l2addr));
    for( i = 0 ; i < 16; i++) {
        rv = _bcm_kt2_qflush_warfp_action_add(unit, ifp_entry_id, port, i);
        if (rv < 0) {
           return rv;
        }
        /* Allocates memory for packet. */
        (void)bcm_pkt_alloc(unit, sizeof(war_pkt), BCM_TX_CRC_REGEN | BCM_TX_ETHER, &tx_pkt);
        /* Configure packet parameters
         BCM_PKT_HDR_DMAC_SET(tx_pkt, dmac); */
        /* Send test packet */
        (void)bcm_pkt_memcpy(tx_pkt, 0, war_pkt, sizeof(war_pkt));
        (void)bcm_esw_tx(unit, tx_pkt, NULL);
        (void)bcm_pkt_free(unit, tx_pkt);
    }

    BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_delete(unit, dest, 1));
    return BCM_E_NONE;
}

int 
_bcm_kt_ipmc_set_remap_group(int unit, int ipmc_id, 
        bcm_port_t port, int count)
{
    int rv = BCM_E_NONE;
    int i, int_mem = 0, ext_mem = 0;
    int new_remap_id, old_remap_id;
    int flush_active = 0;
    uint32 rval = 0;
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS];
    uint32 timeout_val;
    soc_timeout_t timeout;
    mmu_ipmc_group_tbl0_entry_t ipmc_group_entry;
    mmu_toq_ipmc_group_tbl0_entry_t toq_group_entry;
    bcm_pbmp_t destination_pbmp;
    int port_ext_queue_count = 0;
    soc_mem_t dest_pbm_mem;
    int war_enable = 0;
    int war_ifp_id = 0;
    int qstatus = 0;

#ifdef BCM_KATANA2_SUPPORT
    mmu_repl_grp_tbl0_entry_t repl_group_entry;
    soc_mem_t repl_grp_mem[] = {
        MMU_REPL_GRP_TBL0m,
        MMU_REPL_GRP_TBL1m,
        MMU_REPL_GRP_TBL2m
    };
#endif

    soc_mem_t ipmc_mem[] = {
        MMU_IPMC_GROUP_TBL0m,
        MMU_IPMC_GROUP_TBL1m,
        MMU_IPMC_GROUP_TBL2m,
        MMU_IPMC_GROUP_TBL3m,
        MMU_IPMC_GROUP_TBL4m 
    };
    soc_mem_t toq_mem[] = {
        MMU_TOQ_IPMC_GROUP_TBL0m,
        MMU_TOQ_IPMC_GROUP_TBL1m,
        MMU_TOQ_IPMC_GROUP_TBL2m,
        MMU_TOQ_IPMC_GROUP_TBL3m,
        MMU_TOQ_IPMC_GROUP_TBL4m,
        MMU_TOQ_IPMC_GROUP_TBL5m,
        MMU_TOQ_IPMC_GROUP_TBL6m
    };
    /*
     *  Workaround for extended queues 
     *  update the redirection ptr, memory type information in L3_IPMC,
     *  copy the ipmc group table from the current locationt to
     *  redirected location.
     */

    BCM_IF_ERROR_RETURN
        (_bcm_kt_ipmc_mmu_mc_remap_ptr(unit, 0, &new_remap_id, FALSE));
    BCM_IF_ERROR_RETURN
        (_bcm_kt_ipmc_mmu_mc_remap_ptr(unit, ipmc_id, &old_remap_id, FALSE));
    /* Saber2 doesn't have MMU_IPMC_GROUP_TBL 1-4 */
    for (i = 0; i <= 4; i++) {
        if (SOC_MEM_IS_VALID(unit, ipmc_mem[i])) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, ipmc_mem[i], MEM_BLOCK_ANY,
                        old_remap_id, &ipmc_group_entry));

            if (SOC_MEM_IS_VALID(unit, toq_mem[0])) {
                BCM_IF_ERROR_RETURN
                    (_bcm_kt_ipmc_update_group(unit, ipmc_mem[i], new_remap_id, 
                                               &ipmc_group_entry));
            }
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, ipmc_mem[i], MEM_BLOCK_ANY,
                        new_remap_id, &ipmc_group_entry));
        }    
    }    

    if (SOC_MEM_IS_VALID(unit, toq_mem[0])) {
        for (i = 0; i <= 6; i++) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, toq_mem[i], MEM_BLOCK_ANY,
                        old_remap_id, &toq_group_entry));

            BCM_IF_ERROR_RETURN(soc_mem_write(unit, toq_mem[i], MEM_BLOCK_ANY,
                        new_remap_id, &toq_group_entry));
        }    
    }
#ifdef BCM_KATANA2_SUPPORT
    for (i = 2; i >= 0; i--) {
        if (SOC_MEM_IS_VALID(unit, repl_grp_mem[i])) {
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, repl_grp_mem[i], MEM_BLOCK_ANY,
                        old_remap_id, &repl_group_entry));

            BCM_IF_ERROR_RETURN(soc_mem_write(unit, repl_grp_mem[i], MEM_BLOCK_ANY,
                        new_remap_id, &repl_group_entry));
        }
    }
#endif

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY,
                ipmc_id, entry));
    soc_mem_field32_set(unit, L3_IPMCm, entry, MMU_MC_REDIRECTION_PTRf, 
            new_remap_id);

    /*
     * Get extended queue_count_int and extended queue_count_ext
     * If there is at least one extended queue replication that
     * requires external buffer, EXT_Q_EXTERNALf is set to 1.
     * If there is at least one extended queue replication that
     * requires internal buffer, EXT_Q_INTERNALf is set to 1.
     */
    int_mem = IPMC_EXTQ_REPL_QUEUE_COUNT_INT(unit, ipmc_id) ? 1 : 0;
    ext_mem = IPMC_EXTQ_REPL_QUEUE_COUNT_EXT(unit, ipmc_id) ? 1 : 0;

    soc_mem_field32_set(unit, L3_IPMCm, entry, EXT_Q_INTERNALf,
            int_mem);
    soc_mem_field32_set(unit, L3_IPMCm, entry, EXT_Q_EXTERNALf,
            ext_mem);
    soc_mem_field32_set(unit, L3_IPMCm, entry, EXT_Q_NUM_COPIESf,
            count);
    soc_mem_field32_set(unit, L3_IPMCm, entry, VALIDf, 1);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ANY,
                ipmc_id, entry));

    BCM_PBMP_CLEAR(destination_pbmp);

    if (SOC_MEM_IS_VALID(unit ,L3_IPMC_2m)) {
        /* For Katana2 */
        dest_pbm_mem = L3_IPMC_2m ;
    } else {
        /* Saber2, Katana */
        dest_pbm_mem = L3_IPMCm;
    }

    if (SOC_MEM_FIELD_VALID(unit, dest_pbm_mem, DESTINATION_PBMf)) {

        port_ext_queue_count = IPMC_EXTQ_REPL_PORT_QUEUE_COUNT(unit,
                port, ipmc_id);

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, dest_pbm_mem, MEM_BLOCK_ANY,
                    ipmc_id, entry));

        /* Extract Destination_pbm for egress mirror support on extended queues. */
        soc_mem_pbmp_field_get(unit,
                dest_pbm_mem, entry, DESTINATION_PBMf, &destination_pbmp);
        if (port_ext_queue_count) {
            BCM_PBMP_PORT_ADD(destination_pbmp, port);
        } else {
            if (BCM_PBMP_MEMBER(destination_pbmp, port)) {
                BCM_PBMP_PORT_REMOVE(destination_pbmp, port);
            }
        }
        soc_mem_pbmp_field_set(unit,
                dest_pbm_mem, entry, DESTINATION_PBMf, &destination_pbmp);

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, dest_pbm_mem, MEM_BLOCK_ANY,
                    ipmc_id, entry));
    }

    if (!(SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM  || SAL_BOOT_XGSSIM)) {
        /* flush the old entry */
        if (SOC_REG_IS_VALID(unit, TOQ_QUEUE_FLUSH0r)) {
            soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_ID0f, 0xfff);
            soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_NUMf, 1);
            soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_ACTIVEf, 1);
            soc_reg_field_set(unit, TOQ_QUEUE_FLUSH0r, &rval, Q_FLUSH_EXTERNALf, 
                    ext_mem);
            BCM_IF_ERROR_RETURN(WRITE_TOQ_QUEUE_FLUSH0r(unit, rval));
        }

        if (SOC_REG_IS_VALID(unit, TOQ_FLUSH0r)) {
            soc_reg_field_set(unit, TOQ_FLUSH0r, &rval,
                    FLUSH_TYPEf, 0);
            if (SOC_IS_SABER2(unit)) {
                soc_reg_field_set(unit, TOQ_FLUSH0r, &rval,
                        FLUSH_ID0f, 0x3ff);
            } else {
                soc_reg_field_set(unit, TOQ_FLUSH0r, &rval,
                        FLUSH_ID0f, 0xfff);
            }
            soc_reg_field_set(unit, TOQ_FLUSH0r, &rval,
                    FLUSH_NUMf, 1);
            soc_reg_field_set(unit, TOQ_FLUSH0r, &rval,
                    FLUSH_ACTIVEf, 1);
            soc_reg_field_set(unit, TOQ_FLUSH0r, &rval,
                    FLUSH_EXTERNALf, ext_mem);
            BCM_IF_ERROR_RETURN(WRITE_TOQ_FLUSH0r(unit, rval));
        }

        /* wait for flush to complete */
        flush_active = 1;
        timeout_val = soc_property_get(unit, spn_MMU_IPMC_QUEUE_FLUSH_TIMEOUT, 800000);
        soc_timeout_init(&timeout, timeout_val, 0);

        while (flush_active) {
            if (SOC_REG_IS_VALID(unit, TOQ_QUEUE_FLUSH0r)) {
                BCM_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval));
                flush_active = soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval,
                        Q_FLUSH_ACTIVEf);
            }
            if (SOC_REG_IS_VALID(unit, TOQ_FLUSH0r)) {
                BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval));
                flush_active = soc_reg_field_get(unit, TOQ_FLUSH0r, rval,
                        FLUSH_ACTIVEf);
            }

            if (soc_timeout_check(&timeout)) {

                if (soc_feature(unit, soc_feature_queue_flush_war_enable)) {
                    war_enable = soc_property_get(unit, spn_QUEUE_FLUSH_WAR_ENABLE, 0);
                    war_ifp_id = soc_property_get(unit, spn_QUEUE_FLUSH_WAR_FP_ENTRYID, 0);
                    BCM_IF_ERROR_RETURN(READ_MMU_ITE_Q_FLUSH_STATUS_0r(unit, &rval));
                    qstatus = soc_reg_field_get(unit, MMU_ITE_Q_FLUSH_STATUS_0r, rval,
                            Q_FLUSH_STATEf);
                    flush_active = 1;
                    timeout_val = soc_property_get(unit, spn_MMU_IPMC_QUEUE_FLUSH_TIMEOUT, 800000);
                    soc_timeout_init(&timeout, timeout_val, 0);
                    /* After the timeout reset the SW polling time.
                     * If time out is because of queue struck state then call WAR sequence. */
                    if (war_enable && war_ifp_id && IS_EXT_MEM_PORT(unit, port) && (qstatus == 2)) {

                        while (flush_active) {
                            BCM_IF_ERROR_RETURN(_bcm_kt2_qflush_war(unit));
                            if (SOC_REG_IS_VALID(unit, TOQ_QUEUE_FLUSH0r)) {
                                BCM_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval));
                                flush_active = soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval,
                                        Q_FLUSH_ACTIVEf);
                            }

                            if (SOC_REG_IS_VALID(unit, TOQ_FLUSH0r)) {
                                BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval));
                                flush_active = soc_reg_field_get(unit, TOQ_FLUSH0r, rval,
                                        FLUSH_ACTIVEf);
                            }
                            if (soc_timeout_check(&timeout)) {
                                LOG_CLI((BSL_META_U(unit,
                                                "ERROR: Queue  flush operation failed \n")));
                                return (BCM_E_TIMEOUT);
                            }
                        }

                    } else {
                     /* Time out not because of queue struck state continue to poll for reset amount of time */
                        while (flush_active) {
                            if (SOC_REG_IS_VALID(unit, TOQ_QUEUE_FLUSH0r)) {
                                BCM_IF_ERROR_RETURN(READ_TOQ_QUEUE_FLUSH0r(unit, &rval));
                                flush_active = soc_reg_field_get(unit, TOQ_QUEUE_FLUSH0r, rval,
                                        Q_FLUSH_ACTIVEf);
                            }

                            if (SOC_REG_IS_VALID(unit, TOQ_FLUSH0r)) {
                                BCM_IF_ERROR_RETURN(READ_TOQ_FLUSH0r(unit, &rval));
                                flush_active = soc_reg_field_get(unit, TOQ_FLUSH0r, rval,
                                        FLUSH_ACTIVEf);
                            }
                            if (soc_timeout_check(&timeout)) {
                                LOG_CLI((BSL_META_U(unit,
                                                "ERROR: Queue  flush operation failed \n")));
                                return (BCM_E_TIMEOUT);
                            }
                        }
                    }
                } else if (soc_timeout_check(&timeout)) {
                    LOG_CLI((BSL_META_U(unit,
                                    "ERROR: Queue  flush operation failed \n")));
                    return (BCM_E_TIMEOUT);
                }

                }
            }
        }
        /* reclaim the old entry for remapping */
        BCM_IF_ERROR_RETURN
            (_bcm_kt_ipmc_mmu_mc_remap_ptr(unit, 0, &old_remap_id, TRUE));

    return rv;
}

int
_bcm_kt_ipmc_egress_intf_set(int unit, int ipmc_id, bcm_port_t port, 
                             int ipmc_ptr, int if_count, bcm_if_t *if_array, 
                             int *new_ipmc_ptr, int *last_ipmc_flag, 
                             int check_port)
{
    int                 rv = BCM_E_NONE;
    int                 list_start_ptr, prev_start_ptr;
    int                 alloc_size, repl_hash, vlan_count, last_flag;
    int                 if_num, partition;
    int                 intf_count = 0;
    int                 nh_count = 0;
    bcm_l3_intf_t       l3_intf;
    pbmp_t              pbmp, ubmp;
    SHR_BITDCL          *intf_vec = NULL;
    _bcm_repl_list_info_t *rli_start, *rli_current, *rli_prev;
    int remap_id;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if ((uint32)if_count > IPMC_REPL_INTF_TOTAL(unit)) {
        return BCM_E_PARAM;
    }

    /* Partition between L3 interfaces and NHIs in the intf_vec
     * intf_vec = [L3 interfaces, NHIs] */
    partition = soc_mem_index_count(unit, EGR_L3_INTFm);

    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(alloc_size, "IPMC repl interface vector");
    if (intf_vec == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(intf_vec, 0, alloc_size);

    IPMC_REPL_LOCK(unit);

    /* Interface validation and vector construction */
    for (if_num = 0; if_num < if_count; if_num++) {
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num]) &&
            (uint32)(if_array[if_num]) > partition) {
            rv = BCM_E_PARAM;
            goto intf_set_done;
        }
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num])) {
            /* L3 interface is used */
            if (check_port) {
                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = if_array[if_num];
                if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                    goto intf_set_done;
                }
                if ((rv = bcm_esw_vlan_port_get(unit, l3_intf.l3a_vid,
                                &pbmp, &ubmp)) < 0) {
                    goto intf_set_done;
                }
                if (!BCM_PBMP_MEMBER(pbmp, port)) {
                    rv = BCM_E_PARAM;
                    goto intf_set_done;
                }
            }
            SHR_BITSET(intf_vec, if_array[if_num]);
            intf_count++;
        } else {
            /* Next hop is used */
            SHR_BITSET(intf_vec, partition + if_array[if_num] -
                       BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
            nh_count++;
        }
    }

    prev_start_ptr = ipmc_ptr;

    /* Search for list already in table */
    rli_start = IPMC_REPL_LIST_INFO(unit);

    repl_hash =
        _shr_crc32b(0, (uint8 *)intf_vec, IPMC_REPL_INTF_TOTAL(unit));

    for (rli_current = rli_start; rli_current != NULL;
         rli_current = rli_current->next) {
        if (repl_hash == rli_current->hash) {
            rv = _bcm_tr2_repl_list_compare(unit, rli_current->index,
                                           intf_vec);
            if (rv == BCM_E_NOT_FOUND) {
                continue; /* Not a match */
            } else if (rv < 0) {
                goto intf_set_done; /* Access error */
            } else {
                break; /* Match */
            }
        }
    }

    if (rli_current != NULL) {
        /* Found a match, point to here and increase reference count */
        if (prev_start_ptr == rli_current->index) {
            /* We're already pointing to this list, so done */
            rv = BCM_E_NONE;
            goto intf_set_done;
        } else {
            list_start_ptr = rli_current->index;
            vlan_count = rli_current->list_size;
        }
    } else {
        /* Not a match, make a new chain */
        if ((rv = _bcm_kt_repl_list_write(unit, &list_start_ptr,
                                         &vlan_count, intf_vec,
                                         intf_count, nh_count)) < 0) {
            goto intf_set_done;
        }

        if (vlan_count > 0) {
            /* Update data structures */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC repl list info");
            if (rli_current == NULL) {
                /* Release list */
                _bcm_tr2_repl_list_free(unit, list_start_ptr);
                rv = BCM_E_MEMORY;
                goto intf_set_done;
            }
            sal_memset(rli_current, 0, alloc_size);
            rli_current->index = list_start_ptr;
            rli_current->hash = repl_hash;
            rli_current->next = rli_start;
            rli_current->list_size = vlan_count;
            IPMC_REPL_LIST_INFO(unit) = rli_current;
            rli_start = rli_current;
        }
    }

    last_flag = (vlan_count == 1);
    *last_ipmc_flag = last_flag;

    if (vlan_count > 0) {
        (rli_current->refcount)++;
        /* we don't need this rli_current anymore */
    } else if (prev_start_ptr != 0) {
        list_start_ptr = 0;
    }
    *new_ipmc_ptr = list_start_ptr;        
    /* update the vlan count to remapped index */
    /* need to updated the vlan count is proper index in IPMC_REPL_PORT_VLAN_COUNT
       without this change vlan count is updated  in invalid array index
       this will cause _bcm_kt_ipmc_egress_intf_delete to fail in below condition.
       Refer to SDK-108543 for more info.
    */
    if ((rv = _bcm_kt_ipmc_mmu_mc_remap_ptr(unit, 
                          0, &remap_id, FALSE)) < 0) {
        goto intf_set_done;
    }
    IPMC_REPL_PORT_VLAN_COUNT(unit, port, remap_id) = vlan_count;
    if ((rv = _bcm_kt_ipmc_mmu_mc_remap_ptr(unit, 
                          ipmc_id, &remap_id, FALSE)) < 0) {
        goto intf_set_done;
    }
    IPMC_REPL_PORT_VLAN_COUNT(unit, port, remap_id) = vlan_count;

    if (prev_start_ptr != 0) {
        rli_prev = NULL;
        for (rli_current = rli_start; rli_current != NULL;
             rli_current = rli_current->next) {
            if (prev_start_ptr == rli_current->index) {
                (rli_current->refcount)--;
                if (rli_current->refcount == 0) {
                    /* Free these linked list entries */
                    rv = _bcm_tr2_repl_list_free(unit, prev_start_ptr);
                    /* If we have an error, we'll fall out anyway */
                    if (rli_prev == NULL) {
                        IPMC_REPL_LIST_INFO(unit) = rli_current->next;
                    } else {
                        rli_prev->next = rli_current->next;
                    }
                    sal_free(rli_current);
                }
                break;
            }
            rli_prev = rli_current;
        }
    }

 intf_set_done:
    IPMC_REPL_UNLOCK(unit);
    if (intf_vec != NULL) {
        sal_free(intf_vec);
    }
    return rv;
}

int
_bcm_kt_ipmc_egress_intf_get(int unit, int ipmc_id, bcm_port_t port,  
                             int ipmc_ptr, int if_max, bcm_if_t *if_array, 
                             int *if_count)
{
    int                 rv = BCM_E_NONE;
    int                 vlan_ptr, last_vlan_ptr;
    int                 ls_pos;
    int                 rvth = BCM_E_NONE;
    int                 intf_base, nh_offset;
    uint32              ls_bits[2];
    uint32              vlan_count;
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (if_max < 0) {
        return BCM_E_PARAM;
    }

    IPMC_REPL_LOCK(unit);
    if (IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) == 0) {
        *if_count = 0;
        IPMC_REPL_UNLOCK(unit);
        return rvth;
    }

    vlan_ptr = ipmc_ptr;
    last_vlan_ptr = -1;
    vlan_count = 0;
    if (SOC_IS_TD_TT(unit)) {
        nh_offset = TD_EGR_L3_NEXT_HOP_OFFSET;
    } else {
        /* coverity[large_shift : FALSE] */
        nh_offset = (1 << (soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm,
                                                MSB_VLANf) - 1)) * 64;
    }
    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto intf_get_done;
        }
        /* Each MSB represents 64 entries in LSB bitmap */
        intf_base = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit, &vlan_entry,
                                                       MSB_VLANf) * 64;
        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                         LSB_VLAN_BMf, ls_bits);
        /* Using MSB, check if L3 interface or next hop */

        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                if (if_max == 0) {
                    vlan_count++;
                } else {
                    /* Check if L3 interface or next hop */
                    if (intf_base >= nh_offset) {
                        /* Entry contains next hops */
                        if_array[vlan_count++] = intf_base - nh_offset +
                            ls_pos + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                    } else {
                        /* Entry contains interfaces */
                        if_array[vlan_count++] = intf_base + ls_pos;
                    }
                    if (vlan_count == (uint32)if_max) {
                        *if_count = vlan_count;
                        goto intf_get_done;
                    }
                }
            }
        }
        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                      &vlan_entry, NEXTPTRf);

        if (vlan_count >= IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
            break;
        }
    }

    *if_count = vlan_count;

 intf_get_done:
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

int
_bcm_kt_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port, 
                             int ipmc_ptr, int id, int  *new_ipmc_ptr, 
                             int *last_flag)
{
    int                 rv = BCM_E_NONE;
    int                 *if_array = NULL;
    int                 intf_num, intf_max, alloc_size;
    int                 partition;
    bcm_l3_intf_t       l3_intf;
    pbmp_t              pbmp, ubmp;
    int                 i = 0;    
    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(int);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = _bcm_kt_ipmc_egress_intf_get(unit, ipmc_id, port, ipmc_ptr, intf_max,
                                     if_array, &intf_num);
    if (BCM_SUCCESS(rv)) {

        for (i = 0 ;i < intf_num ; i++ ) {
             if (if_array[i] == id) {
                 rv = BCM_E_EXISTS;
                 goto intf_add_done;
             }
        }

        if (intf_num < intf_max) {
            if_array[intf_num++] = id;

            /* For IPMC, check port is a member of the L3 interface's VLAN.
             * Performing this check here is more efficient than performing
             * it in bcm_tr2_ipmc_egress_intf_set.
             */
            if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, id)) {
                /* L3 interface is used */
                partition = soc_mem_index_count(unit, EGR_L3_INTFm);
                if (id > partition) {
                    rv = BCM_E_PARAM;
                    goto intf_add_done;
                }

                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id = id;
                if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                    goto intf_add_done;
                }
                if ((rv = bcm_esw_vlan_port_get(unit, l3_intf.l3a_vid,
                                &pbmp, &ubmp)) < 0) {
                    goto intf_add_done;
                }
                if (!BCM_PBMP_MEMBER(pbmp, port)) {
                    rv = BCM_E_PARAM;
                    goto intf_add_done;
                }
            }

            rv = _bcm_kt_ipmc_egress_intf_set(unit, ipmc_id, port, ipmc_ptr,
                                             intf_num, if_array, 
                                             new_ipmc_ptr, last_flag, FALSE);
        } else {
            rv = BCM_E_FULL;
        }
    }

intf_add_done:

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

int
_bcm_kt_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                                int ipmc_ptr, int id, int  *new_ipmc_ptr, 
                                int *last_flag)
{
    int  rv = BCM_E_NONE;
    int  *if_array = NULL;
    int  alloc_size, if_count, if_cur, match;
    int  intf_max;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    } 

    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max * sizeof(int);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = _bcm_kt_ipmc_egress_intf_get(unit, ipmc_id, port, ipmc_ptr, 
                                      intf_max, if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        match = FALSE;
        for (if_cur = 0; if_cur < if_count; if_cur++) {
            if (match) {
                if_array[if_cur - 1] = if_array[if_cur];
            } else {
                if (if_array[if_cur] == id) {
                    match = TRUE;
                }
            }
        }
        if (match) {
            if_count--;
            rv = _bcm_kt_ipmc_egress_intf_set(unit, ipmc_id, port, ipmc_ptr,
                                             if_count, if_array, new_ipmc_ptr, 
                                             last_flag, FALSE);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

int 
_bcm_kt_ipmc_subscriber_egress_intf_traverse(int unit, int extq_ptr, 
                                             int *baseq_id, 
                                             int *ptr_array, int *q_num) 
{
    int rv = BCM_E_NOT_FOUND;
    int next_ptr1, next_ptr2;
    int queue_id;
    int ptr, type, i;
    mmu_ext_mc_queue_list0_entry_t qlist0_entry;
    mmu_ext_mc_queue_list1_entry_t qlist1_entry; 
    mmu_ext_mc_queue_list4_entry_t qlist4_entry;
    soc_field_t        base_qid[] = { BASE_QID0f, BASE_QID1f };    
    soc_field_t        repl_ptr[] = { L3_REPL_PTR0f, L3_REPL_PTR1f };    
    soc_field_t        ptr_type[] = { L3_REPL_TYPE0f, L3_REPL_TYPE1f };

    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                      extq_ptr, &qlist0_entry)) < 0) {
        goto extq_list_done;
    }
    if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
        if ((rv = READ_MMU_EXT_MC_QUEUE_LIST1m(unit, MEM_BLOCK_ANY,
                        extq_ptr, &qlist1_entry)) < 0) {
            goto extq_list_done;
        }
    }
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                      extq_ptr, &qlist4_entry)) < 0) {
        goto extq_list_done;
    }
    
    for (i = 0; i < 2; i++) {     
        if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, 
                                    &qlist0_entry, base_qid[i])) != 0) {
            ptr = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, 
                                    &qlist0_entry, repl_ptr[i]);
            if (SOC_MEM_FIELD_VALID(unit, MMU_EXT_MC_QUEUE_LIST0m, 
                                    L3_REPL_TYPE0f) || 
                SOC_MEM_FIELD_VALID(unit, MMU_EXT_MC_QUEUE_LIST0m,
                                    L3_REPL_TYPE1f)) {
                /* KATANA */
                type = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit,
                                    &qlist0_entry, ptr_type[i]);
                ptr = ptr | (type << 14);
            }

            baseq_id[*q_num] = queue_id;
            ptr_array[*q_num] = ptr ;
            (*q_num)++;
         }
       }    
    if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
        for (i = 0; i < 2; i++) {    
            if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, 
                            &qlist1_entry, base_qid[i])) != 0) {
                ptr = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, 
                        &qlist1_entry, repl_ptr[i]);
                if (SOC_MEM_FIELD_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m,
                            L3_REPL_TYPE0f) ||
                        SOC_MEM_FIELD_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m,
                            L3_REPL_TYPE1f)) {
                    /* KATANA */
                    type = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit,
                            &qlist1_entry, ptr_type[i]);
                    ptr = ptr | (type << 14);
                }
                baseq_id[*q_num] = queue_id;
                ptr_array[*q_num] = ptr ;
                (*q_num)++;                 
            }
        }
    }
         
    next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                            &qlist4_entry, EXT_Q_NXT_PTR1f);
    next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                            &qlist4_entry, EXT_Q_NXT_PTR2f);
           
    if (next_ptr1 != extq_ptr) {
        rv = _bcm_kt_ipmc_subscriber_egress_intf_traverse(unit, next_ptr1, 
                                                 baseq_id, ptr_array, q_num);
    } 
  
    if (next_ptr2 != extq_ptr) {
        rv = _bcm_kt_ipmc_subscriber_egress_intf_traverse(unit, next_ptr2, 
                                                 baseq_id, ptr_array, q_num);
    }    
    
extq_list_done:
    return rv;
}

#ifdef BCM_KATANA2_SUPPORT
int
_bcm_kt2_ipmc_sub_egress_intf_trav_ext_set(int unit, int extq_ptr,
                                             int port,
                                             int ext_mem_enable)
{
    int rv = BCM_E_NOT_FOUND;
    int next_ptr1, next_ptr2;
    int queue_id;
    int i, port_get;
    mmu_ext_mc_queue_list0_entry_t qlist0_entry;
    mmu_ext_mc_queue_list1_entry_t qlist1_entry;
    mmu_ext_mc_queue_list4_entry_t qlist4_entry;
    soc_field_t        base_qid[] = { BASE_QID0f, BASE_QID1f };
    soc_field_t        buff_type[]  = { BUFF_TYPE0f, BUFF_TYPE1f };

    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                      extq_ptr, &qlist0_entry)) < 0) {
        goto extq_list_done;
    }
    if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
        if ((rv = READ_MMU_EXT_MC_QUEUE_LIST1m(unit, MEM_BLOCK_ANY,
                        extq_ptr, &qlist1_entry)) < 0) {
            goto extq_list_done;
        }
    }
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                      extq_ptr, &qlist4_entry)) < 0) {
        goto extq_list_done;
    }

    for (i = 0; i < 2; i++) {
        if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit,
                                    &qlist0_entry, base_qid[i])) != 0) {
            BCM_IF_ERROR_RETURN
                (bcm_kt2_cosq_port_get(unit, queue_id, &port_get));
            if (port_get == port) {
                if (!ext_mem_enable) {
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit,
                        &qlist0_entry,
                        buff_type[i], 0);
                } else {
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit,
                        &qlist0_entry,
                        buff_type[i], 1);
                }
            }
        }
    }
    if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
        for (i = 0; i < 2; i++) {
            if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit,
                            &qlist1_entry, base_qid[i])) != 0) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_cosq_port_get(unit, queue_id, &port_get));
                if (port_get == port) {
                    if (!ext_mem_enable) {
                         soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit,
                             &qlist1_entry,
                             buff_type[i], 0);
                     } else {
                         soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit,
                             &qlist1_entry,
                             buff_type[i], 1);
                     }
                }
            }
        }
    }
    BCM_IF_ERROR_RETURN(WRITE_MMU_EXT_MC_QUEUE_LIST0m(unit,
        MEM_BLOCK_ANY,
        extq_ptr, &qlist0_entry));
    BCM_IF_ERROR_RETURN(WRITE_MMU_EXT_MC_QUEUE_LIST1m(unit,
        MEM_BLOCK_ANY,
        extq_ptr, &qlist1_entry));
    next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit,
                            &qlist4_entry, EXT_Q_NXT_PTR1f);
    next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit,
                            &qlist4_entry, EXT_Q_NXT_PTR2f);

    if (next_ptr1 != extq_ptr) {
        rv = _bcm_kt2_ipmc_sub_egress_intf_trav_ext_set(unit, next_ptr1,
                                                 port, ext_mem_enable);
    }

    if (next_ptr2 != extq_ptr) {
        rv = _bcm_kt2_ipmc_sub_egress_intf_trav_ext_set(unit, next_ptr2,
                                                 port, ext_mem_enable);
    }

extq_list_done:
    return rv;
}

int
_bcm_kt2_ipmc_subscriber_egress_queue_ext_set(int unit, int port, int ipmc_id, int ext_mem_enable)
{
    int extq_ptr;

    BCM_IF_ERROR_RETURN(_kt_extq_mc_group_ptr(unit, ipmc_id,
                        &extq_ptr, 0, 0));
    if (extq_ptr != 0) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_ipmc_sub_egress_intf_trav_ext_set(unit, extq_ptr,
                                                                      port, ext_mem_enable));
    }
     return BCM_E_NONE;
}
#endif

int
_bcm_kt_get_subscriber_queue_entries(int unit, int extq_ptr, 
                                     SHR_BITDCL *q_vec, 
                                     uint32  *q_repl_ptr, 
                                     int *q_num)
{
    int     rv = BCM_E_NONE;
    int     i;
    int     next_ptr1, next_ptr2;
    int     queue_id;
    int     ptr, type, copy, buf_type, vlan_last;
    mmu_ext_mc_queue_list0_entry_t     qlist0_entry;
    mmu_ext_mc_queue_list1_entry_t     qlist1_entry; 
    mmu_ext_mc_queue_list4_entry_t     qlist4_entry;
    soc_field_t        base_qid[]  = { BASE_QID0f, BASE_QID1f }; 
    soc_field_t        repl_ptr[]  = { L3_REPL_PTR0f, L3_REPL_PTR1f };    
    soc_field_t        l3_last[]   = { L3_LAST0f, L3_LAST1f };
    soc_field_t        ipmc_last[] = { IPMC_VLAN_LAST0f, IPMC_VLAN_LAST1f };
    soc_field_t        ptr_type[]  = { L3_REPL_TYPE0f, L3_REPL_TYPE1f };
    soc_field_t        l2_copy[]   = { L2_COPY0f, L2_COPY1f };
    soc_field_t        buff_type[]  = { BUFF_TYPE0f, BUFF_TYPE1f };
    soc_field_t        profile_idx[] = { Q_OFFSET_PROF0f, Q_OFFSET_PROF1f };
    int profile = 0;
    
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                          extq_ptr, &qlist0_entry)) < 0) {
        goto q_entries_done;
    }
    if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
        if ((rv = READ_MMU_EXT_MC_QUEUE_LIST1m(unit, MEM_BLOCK_ANY,
                        extq_ptr, &qlist1_entry)) < 0) {
            goto q_entries_done;
        }
    } 
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                          extq_ptr, &qlist4_entry)) < 0) {
        goto q_entries_done;
    }

    /* store the content of queue list entries */
    for (i = 0; i < 2; i++) {     
        if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, 
                                    &qlist0_entry, base_qid[i])) != 0) {    
            ptr = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry, 
                                                           repl_ptr[i]);
            copy = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry, 
                                                           l2_copy[i]);
            buf_type = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry, 
                                                           buff_type[i]);
            if (SOC_MEM_FIELD_VALID(unit, MMU_EXT_MC_QUEUE_LIST0m,             
                L3_REPL_TYPE0f)) {
                /* KATANA */
                type = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit,
                                                               &qlist0_entry,
                                                               ptr_type[i]);
                vlan_last = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry,
                                                           ipmc_last[i]);
                ptr |= ((type << 14) | (copy << 15) | (buf_type << 16) \
                    | (vlan_last << 17));
            } else {
                /* KATANA2 */
                profile = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry,
                                                              profile_idx[i]);
                vlan_last = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit, &qlist0_entry,
                                                           l3_last[i]);
                ptr |= ((copy << 15) | (buf_type << 16) |
                        (vlan_last << 17) | (profile << 18));
            }
            q_repl_ptr[queue_id] = ptr;
            SHR_BITSET(q_vec, queue_id);
            (*q_num)++;
        }
    }
    if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
        for (i = 0; i < 2; i++) {     
            if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, 
                            &qlist1_entry, base_qid[i])) != 0) {    
                ptr = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, &qlist1_entry, 
                        repl_ptr[i]);
                copy = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, &qlist1_entry, 
                        l2_copy[i]);
                buf_type = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, &qlist1_entry, 
                        buff_type[i]);
                if (SOC_MEM_FIELD_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m,
                            L3_REPL_TYPE0f)) {
                    /* KATANA */
                    vlan_last = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, &qlist1_entry,
                            ipmc_last[i]);
                    type = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, &qlist1_entry,
                            ptr_type[i]);
                    ptr |= ((type << 14) | (copy << 15) | (buf_type << 16) \
                        | (vlan_last << 17));

                } else {
                    /* KATANA2 */            
                    vlan_last = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, &qlist1_entry, 
                            l3_last[i]);
                    profile = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit, &qlist1_entry,
                            profile_idx[i]);
                    ptr |= ((copy << 15) | (buf_type << 16) | 
                            (vlan_last << 17)| (profile << 18));
                }

                q_repl_ptr[queue_id] = ptr;    
                SHR_BITSET(q_vec, queue_id);
                (*q_num)++;
            }
        }
    }
    next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR1f);
    next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR2f);
    
    if (next_ptr1 != extq_ptr) {
        if ((rv = _bcm_kt_get_subscriber_queue_entries(unit, next_ptr1, q_vec, 
                                                       q_repl_ptr, q_num)) < 0) {
            goto q_entries_done;                                                                                                
        }                                                    
    } 
           
    if (next_ptr2 != extq_ptr) {
        if ((rv = _bcm_kt_get_subscriber_queue_entries(unit, next_ptr2, q_vec, 
                                                       q_repl_ptr, q_num)) < 0) {
            goto q_entries_done;                                                                                                        
        }                                                
    }    

q_entries_done:
    return rv;
}

int _bcm_kt_ipmc_subscriber_queues_get(int unit, int ipmc_id, 
                                       SHR_BITDCL *q_vec, 
                                       uint32 *q_repl_ptrs,
                                       int *q_num)
{
    int         rv = BCM_E_NONE;
    int         extq_ptr;    

    if ((rv = _kt_extq_mc_group_ptr(unit, ipmc_id,
                                &extq_ptr, 0, 0)) < 0) {
        return rv;
    }
    
    if (extq_ptr != 0) {
        rv = _bcm_kt_get_subscriber_queue_entries(unit, extq_ptr, q_vec,
                                                  q_repl_ptrs, q_num);
    }
    
    return rv;
}

/*
 * Function:
 *    _bcm_kt_extq__repl_list_free
 * Description:
 *    Release the MMU_EXT_MC_QUEUE_LIST entries in the HW list starting 
 *     at extq_ptr.
 */
STATIC int
_bcm_kt_extq_repl_list_free(int unit, int extq_ptr)
{
    int     rv = BCM_E_NONE;
    int     next_ptr1, next_ptr2;
    mmu_ext_mc_queue_list4_entry_t     qlist4_entry;
    
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                    extq_ptr, &qlist4_entry)) < 0) {
        goto repl_list_free_done;
    }

    next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR1f);
    next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit, 
                                     &qlist4_entry, EXT_Q_NXT_PTR2f);
    IPMC_EXTQ_REPL_VE_USED_CLR(unit, extq_ptr);
    
    if (next_ptr1 != extq_ptr) {
        if ((rv = _bcm_kt_extq_repl_list_free(unit, next_ptr1)) < 0) {
            goto repl_list_free_done;   
        }                                                    
    } 
           
    if (next_ptr2 != extq_ptr) {
        if ((rv = _bcm_kt_extq_repl_list_free(unit, next_ptr2)) < 0) {  
            goto repl_list_free_done; 
        }                                                
    }

repl_list_free_done:
    return rv;

}

STATIC int
_bcm_kt_extq_repl_next_free_ptr(int unit)
{
    int                 ix, bit;
    SHR_BITDCL          not_ptrs;

    for (ix = 0; ix < _SHR_BITDCLSIZE(IPMC_EXTQ_LIST_TOTAL(unit)); ix++) {
        not_ptrs = ~_kt_extq_repl_info[unit]->bitmap_entries_used[ix];
        if (not_ptrs) {
            for (bit = 0; bit < SHR_BITWID; bit++) {
                if (not_ptrs & (1 << bit)) {
                    return (ix * SHR_BITWID) + bit;
                }
            }
        }
    }

    return -1;
}

/*
 * Function:
 *    _bcm_kt_extq_repl_list_write
 * Description:
 *    Write the extq list contained in q_vec into the HW table.
 *      Return the start_index and total VLAN count.
 */

STATIC int
_bcm_kt_extq_repl_list_write(int unit, int q_num, 
                        SHR_BITDCL *q_vec, uint32 *q_repl_ptr,
                        int *extq_index, int *count)
{
    int     node_max, alloc_size;
    int     next1, next2;
    int     i, q_count = 0;
    int     rv = BCM_E_NONE;
    int     type, copy, last, buf_type, vlan_last;
    int     reset_val  = 1;
    _bcm_repl_list_info_t *list_array = NULL;
    _bcm_repl_list_info_t *cur_node, *next, *fast_node;
    mmu_ext_mc_queue_list0_entry_t     qlist0_entry;
    mmu_ext_mc_queue_list1_entry_t     qlist1_entry; 
    mmu_ext_mc_queue_list4_entry_t     qlist4_entry;
    soc_field_t        base_qid[] = { BASE_QID0f, BASE_QID1f }; 
    soc_field_t        repl_ptr[] = { L3_REPL_PTR0f, L3_REPL_PTR1f };    
    soc_field_t        l2_copy[]  = { L2_COPY0f, L2_COPY1f };
    soc_field_t        l3_last[] = { L3_LAST0f, L3_LAST1f };     
    soc_field_t        ipmc_last[] = { IPMC_VLAN_LAST0f, IPMC_VLAN_LAST1f };     
    soc_field_t        ptr_type[] = { L3_REPL_TYPE0f, L3_REPL_TYPE1f };
    soc_field_t        buff_type[] = { BUFF_TYPE0f, BUFF_TYPE1f };
    soc_field_t        reset_field[] = { L3_REPL_TYPE0f, L3_REPL_TYPE1f };
    soc_field_t        profile_idx[] = { Q_OFFSET_PROF0f, Q_OFFSET_PROF1f };
    int profile;

    if (q_num == 0) {
        /* nothing to be written into extended queue list */
        *count = q_count;
        return BCM_E_NONE;
    }    
    
    if (SOC_IS_KATANA2(unit)) {
        reset_val = 0x4000;
        for (i = 0; i < 2; i++) {
            reset_field[i] = repl_ptr[i];
        }
    } 

    node_max = (q_num % 4) ? (q_num / 4 + 1) : q_num / 4;
    alloc_size = node_max * sizeof(_bcm_repl_list_info_t);
    list_array = sal_alloc(alloc_size, "IPMC repl node array");
    if (list_array == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(list_array, 0, alloc_size);

    /* create a link list of entries with free qlist pointers */    
    for (i = 0; i < node_max; i++) {
        cur_node = &list_array[i];
        cur_node->index = _bcm_kt_extq_repl_next_free_ptr(unit);
        if (cur_node->index < 0) {
            rv = BCM_E_RESOURCE;
            goto clean_up;
        }
        IPMC_EXTQ_REPL_VE_USED_SET(unit, cur_node->index);
        if (i < (node_max - 1)) { /* create a chain till penultimate node */
            cur_node->next = &list_array[i+1];
        }
    }    

    /* copy back the stored contents into qlist entries */
    fast_node = cur_node = &list_array[0];
    *extq_index = cur_node->index;

    sal_memset(&qlist0_entry, 0, sizeof(mmu_ext_mc_queue_list0_entry_t));
    sal_memset(&qlist1_entry, 0, sizeof(mmu_ext_mc_queue_list1_entry_t));    
    sal_memset(&qlist4_entry, 0, sizeof(mmu_ext_mc_queue_list4_entry_t));    

    *count = 0;
    for (i = 0; i < IPMC_EXTQ_TOTAL(unit); i++) {
        if (q_repl_ptr[i] != 0) {
            if (q_count < 2) {
                soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry, 
                                                        base_qid[q_count], i);
                copy = (q_repl_ptr[i] >> 15) & 1;
                buf_type = (q_repl_ptr[i] >> 16) & 1;
                vlan_last = (q_repl_ptr[i] >> 17) & 1;
                soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry, 
                                                        l2_copy[q_count], copy);
                soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry, 
                                                        buff_type[q_count], buf_type);
                if (SOC_MEM_FIELD_VALID(unit, MMU_EXT_MC_QUEUE_LIST0m,
                                        L3_REPL_TYPE0f)) {
                    /* KATANA */
                    type = (q_repl_ptr[i] >> 14) & 1;
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry,
                                                        ipmc_last[q_count], 
                                                        vlan_last);
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry,
                                                        ptr_type[q_count], type);
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry,
                                                        repl_ptr[q_count],
                                                        q_repl_ptr[i] & 0x3fff);
                } else {
                    /* KATANA2 */
                    profile = (q_repl_ptr[i] >> 18) & 3;
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry,
                                                        l3_last[q_count],
                                                        vlan_last);
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry,
                                                        profile_idx[q_count],
                                                        profile);
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry,
                                                        repl_ptr[q_count],
                                                        q_repl_ptr[i] & 0x7fff);
                }

             } else {

                 if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
                     soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                             base_qid[(q_count % 2)], i);
                     copy = (q_repl_ptr[i] >> 15) & 1;
                     buf_type = (q_repl_ptr[i] >> 16) & 1;
                     vlan_last = (q_repl_ptr[i] >> 17) & 1;
                     soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                             l2_copy[(q_count % 2)], copy);
                     soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                             buff_type[(q_count % 2)], buf_type);
                     if (SOC_MEM_FIELD_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m,
                                 L3_REPL_TYPE0f)) {
                         /* KATANA */
                         type = (q_repl_ptr[i] >> 14) & 1;
                         soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                                 ipmc_last[(q_count % 2)], 
                                 vlan_last);
                         soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                                 ptr_type[(q_count % 2)], type);
                         soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                                 repl_ptr[(q_count % 2)],
                                 q_repl_ptr[i] & 0x3fff);
                     } else {
                         /* KATANA2 */
                         profile = (q_repl_ptr[i] >> 18) & 3;
                         soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                                 l3_last[(q_count % 2)],
                                 vlan_last);
                         soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                                                        profile_idx[q_count % 2],
                                                         profile);
                         soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                                 repl_ptr[(q_count % 2)],
                                 q_repl_ptr[i] & 0x7fff);
                     }
                 }
            }
            q_count++;
            last = 0;
            if (q_count % 4 == 0) { 
                if ((*count < 2) || (*count % 2) != 0) {
                    next = fast_node->next;
                    if (next != NULL) {
                        next1 = next->index;
                        fast_node = next;
                        if (fast_node->next != NULL) {
                            next2 = fast_node->next->index;
                            fast_node = fast_node->next;
                            /* need to set the last flag now, if this is the last enrty */
                            if (fast_node->next == NULL) {
                                last = 1;
                            }
                        } else {
                            next2 = cur_node->index;
                            last = 1;
                        }
                    } else {
                        next1 = cur_node->index;
                        next2 = next1;
                    }    
                } else {
                    next1 = cur_node->index;
                    next2 = next1;
                }

                soc_MMU_EXT_MC_QUEUE_LIST4m_field32_set(unit, &qlist4_entry, 
                                                    EXT_Q_NXT_PTR1f, next1);
                soc_MMU_EXT_MC_QUEUE_LIST4m_field32_set(unit, &qlist4_entry, 
                                                    EXT_Q_NXT_PTR2f, next2);
                soc_MMU_EXT_MC_QUEUE_LIST4m_field32_set(unit, &qlist4_entry,
                                                    EXT_Q_LAST_LASTf, last);
                
                if ((rv = WRITE_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                          cur_node->index, &qlist0_entry)) < 0) {
                    goto clean_up;
                }
                if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
                    if ((rv = WRITE_MMU_EXT_MC_QUEUE_LIST1m(unit, MEM_BLOCK_ANY,
                                    cur_node->index, &qlist1_entry)) < 0) {
                        goto clean_up;
                    }
                }
                if ((rv = WRITE_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                          cur_node->index, &qlist4_entry)) < 0) {
                    goto clean_up;
                }

                sal_memset(&qlist0_entry, 0, sizeof(mmu_ext_mc_queue_list0_entry_t));
                sal_memset(&qlist1_entry, 0, sizeof(mmu_ext_mc_queue_list1_entry_t));    
                sal_memset(&qlist4_entry, 0, sizeof(mmu_ext_mc_queue_list4_entry_t));
                cur_node = cur_node->next;
                *count += 1;
                q_count = 0;
            }
            
        }    

    }    

    if (q_count != 0) {
        soc_MMU_EXT_MC_QUEUE_LIST4m_field32_set(unit, &qlist4_entry, 
                                                EXT_Q_NXT_PTR1f, cur_node->index);
        soc_MMU_EXT_MC_QUEUE_LIST4m_field32_set(unit, &qlist4_entry, 
                                                EXT_Q_NXT_PTR2f, cur_node->index);
        
        /* disable other unused queue entries in the list */
        /* KATANA2 set L2_COPY==0 AND L3_REPL_PTR 0x4000 */
        /* KATANA set  L2==0 AND L3_REPL_TYPE==1 AND L3_REPL_PTR==0 to disable */         
        switch (q_count) {
        case 1:
            soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit, &qlist0_entry,
                                                    reset_field[1], 
                                                    reset_val);
            if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
                soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                        reset_field[0], 
                        reset_val);
                soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                        reset_field[1],
                        reset_val);
            }
            break;
        case 2:
            if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
                soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                        reset_field[0],
                        reset_val);
                soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                        reset_field[1], 
                        reset_val);
            }
            break;
        case 3:
            if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
                soc_MMU_EXT_MC_QUEUE_LIST1m_field32_set(unit, &qlist1_entry,
                        reset_field[1], 
                        reset_val);
            }
            break;
        default:
            rv = BCM_E_INTERNAL;
            goto clean_up;
        }
  
        if ((rv = WRITE_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                         cur_node->index, &qlist0_entry)) < 0) {
            goto clean_up;
        }
        if (SOC_MEM_IS_VALID(unit, MMU_EXT_MC_QUEUE_LIST1m)) {
            if ((rv = WRITE_MMU_EXT_MC_QUEUE_LIST1m(unit, MEM_BLOCK_ANY,
                            cur_node->index, &qlist1_entry)) < 0) {
                goto clean_up;
            }
        }
        if ((rv = WRITE_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                         cur_node->index, &qlist4_entry)) < 0) {
            goto clean_up;
        }
        *count += 1;

    }    

clean_up:
    if (list_array != NULL) {
        sal_free(list_array);
    }
    return rv;

}    

#if defined(BCM_KATANA_SUPPORT)
int 
_bcm_kt_ipmc_extq_list_update_all(int unit, int ipmc_id, int q_num,
                                       SHR_BITDCL *q_vec, uint32 *q_repl_ptr)
{
    int rv = BCM_E_NONE;
    int prev_start_ptr, list_start_ptr;
    _bcm_repl_list_info_t *rli_start, *rli_current, *rli_prev;
    int queue_count = 0;
    

    /*Check previous group pointer*/
    if ((rv = _kt_extq_mc_group_ptr(unit, ipmc_id, &prev_start_ptr, 0, 0)) < 0 ){
        return rv;
    }  

    rli_start =  IPMC_EXTQ_REPL_LIST_INFO(unit);
    if ((rv = _bcm_kt_extq_repl_list_write(unit, q_num, q_vec, q_repl_ptr,
                    &list_start_ptr,&queue_count)) < 0) {
        return rv;
    }

    if (prev_start_ptr != 0) {
        list_start_ptr = 0;
        if ((rv = _kt_extq_mc_group_ptr(unit, ipmc_id,
                        &list_start_ptr, 0, 1)) < 0) {
            return rv;
        }
    }

    IPMC_EXTQ_REPL_QUEUE_COUNT(unit, ipmc_id) = queue_count;

    if (prev_start_ptr != 0) {
        rli_prev = NULL;
        for (rli_current = rli_start; rli_current != NULL;
                rli_current = rli_current->next) {
            if (prev_start_ptr == rli_current->index) {
                (rli_current->refcount)--;
                if (rli_current->refcount == 0) {
                    /* Free these linked list entries */
                    rv = _bcm_kt_extq_repl_list_free(unit, prev_start_ptr);
                    /* If we have an error, we'll fall out anyway */
                    if (rli_prev == NULL) {
                        IPMC_EXTQ_REPL_LIST_INFO(unit) = rli_current->next;
                    } else {
                        rli_prev->next = rli_current->next;
                    }
                    sal_free(rli_current);
                }
                break;
            }
            rli_prev = rli_current;
        }
    }
    return rv;
}
#endif

int
_bcm_kt_ipmc_extq_list_update(int unit, int ipmc_id, bcm_port_t port, 
                              int q_num, SHR_BITDCL *q_vec, 
                              uint32 *q_repl_ptr)
{
    int     rv = BCM_E_NONE;
    int     list_start_ptr, prev_start_ptr;
    int     alloc_size, extq_hash, last_flag;
    int     queue_count = 0;
    _bcm_repl_list_info_t *rli_start, *rli_current, *rli_prev;

    uint32 hash_key_size, q_repl_ptr_size, q_max;
    uint8 *hash_key = NULL;

    /* Check previous group pointer */
    if ((rv = _kt_extq_mc_group_ptr(unit, ipmc_id,
                                &prev_start_ptr, 0, 0)) < 0) {
        goto list_update_done;
    }

    /* Search for list already in table */
    rli_start = IPMC_EXTQ_REPL_LIST_INFO(unit);

    /* Format the hash key to be a consolidated value of
       ( q_vector, port-number, q-reply-ptr ) */

    /* Computed the total bytes needed for key */
    /* Compute the 'q_repl_ptr_size' first */
    q_max = IPMC_EXTQ_TOTAL(unit);
    q_repl_ptr_size = q_max * sizeof(int);

    hash_key_size = q_repl_ptr_size +
                    sizeof(bcm_port_t) +
                    IPMC_EXTQ_TOTAL(unit)/8;
    hash_key = sal_alloc(hash_key_size, "hash_key_alloc_ipmc_extq");

    if (hash_key == NULL) {
        return BCM_E_MEMORY;
    }

    /* Copy over the hash key values */
    sal_memcpy(hash_key, (void *)q_repl_ptr, q_repl_ptr_size);
    sal_memcpy((hash_key + q_repl_ptr_size),
               &port,
               sizeof(bcm_port_t));
    sal_memcpy(hash_key + q_repl_ptr_size + sizeof(bcm_port_t),
               (uint8 *)q_vec,
               IPMC_EXTQ_TOTAL(unit)/8);

    /* Compute hash based on the hash key put together */
    extq_hash =
      _shr_crc32b(0, (uint8 *)hash_key, (q_repl_ptr_size * 8) +
                                        (sizeof(bcm_port_t) * 8) +
                                        IPMC_EXTQ_TOTAL(unit));

      for (rli_current = rli_start; rli_current != NULL;
           rli_current = rli_current->next) {
          if (extq_hash == rli_current->hash) {
              rv = _bcm_kt_extq_repl_list_compare(unit, rli_current->index,
                                             q_vec, q_repl_ptr);
              if (rv == BCM_E_NOT_FOUND) {
                  continue; /* Not a match */
              } else if (rv < 0) {
                  goto list_update_done; /* Access error */
              } else {
                  break; /* Match */
              }
          }
      }

      if (rli_current != NULL) {
          /* Found a match, point to here and increase reference count */
          if (prev_start_ptr == rli_current->index) {
              /* We're already pointing to this list, so done */
              rv = BCM_E_NONE;
              goto list_update_done;
          } else {
              list_start_ptr = rli_current->index;
              queue_count = rli_current->list_size;
          }
      } else {
          /* Not a match, make a new chain */
          if ((rv = _bcm_kt_extq_repl_list_write(unit, q_num, 
                                                q_vec, q_repl_ptr,
                                                &list_start_ptr,
                                                &queue_count)) < 0) {
          goto list_update_done;
          }

          if (queue_count > 0) {
              /* Update data structures */
              alloc_size = sizeof(_bcm_repl_list_info_t);
              rli_current = sal_alloc(alloc_size, "IPMC extq repl list info");
              if (rli_current == NULL) {
                  /* Release list */
                  _bcm_kt_extq_repl_list_free(unit, list_start_ptr);
                  rv = BCM_E_MEMORY;
                  goto list_update_done;
              }
              sal_memset(rli_current, 0, alloc_size);
              rli_current->index = list_start_ptr;
              rli_current->hash = extq_hash;
              rli_current->next = rli_start;
              rli_current->list_size = queue_count;
              IPMC_EXTQ_REPL_LIST_INFO(unit) = rli_current;
              rli_start = rli_current;
          }
      }

      last_flag = (queue_count == 1);

      if (queue_count > 0) {
          if ((rv = _kt_extq_mc_group_ptr(unit, ipmc_id, 
                                  &list_start_ptr, last_flag, 1)) < 0) {
          if (rli_current->refcount == 0) {
              /* This was new */
              _bcm_kt_extq_repl_list_free(unit, list_start_ptr);
              IPMC_EXTQ_REPL_LIST_INFO(unit) = rli_current->next;
              sal_free(rli_current);
          }
          goto list_update_done;
          }

          (rli_current->refcount)++;
          /* we don't need this rli_current anymore */
      } else if (prev_start_ptr != 0) {
          list_start_ptr = 0;
          if ((rv = _kt_extq_mc_group_ptr(unit, ipmc_id,
                                  &list_start_ptr, last_flag, 1)) < 0) {
          goto list_update_done;
          }
      }
      
      IPMC_EXTQ_REPL_QUEUE_COUNT(unit, ipmc_id) = queue_count;

      if (prev_start_ptr != 0) {
          rli_prev = NULL;
          for (rli_current = rli_start; rli_current != NULL;
               rli_current = rli_current->next) {
              if (prev_start_ptr == rli_current->index) {
                  (rli_current->refcount)--;
                  if (rli_current->refcount == 0) {
                      /* Free these linked list entries */
                      rv = _bcm_kt_extq_repl_list_free(unit, prev_start_ptr);
                      /* If we have an error, we'll fall out anyway */
                      if (rli_prev == NULL) {
                          IPMC_EXTQ_REPL_LIST_INFO(unit) = rli_current->next;
                      } else {
                          rli_prev->next = rli_current->next;
                      }
                      sal_free(rli_current);
                  }
                  break;
              }
              rli_prev = rli_current;
          }
      }

    list_update_done:

    if (hash_key) {
        sal_free(hash_key);
    }
    return rv;
}

#ifdef BCM_KATANA2_SUPPORT
int
_bcm_kt2_ipmc_subscriber_egress_queue_qos_map(int unit, int extq_ptr,
                                             int base_queue_id,
                                             int *map_id,
                                             int set)
{
    int rv = BCM_E_NOT_FOUND;
    int next_ptr1, next_ptr2;
    int queue_id;
    int i;
    int profile = 0;
    mmu_ext_mc_queue_list0_entry_t qlist0_entry;
    mmu_ext_mc_queue_list1_entry_t qlist1_entry;
    mmu_ext_mc_queue_list4_entry_t qlist4_entry;
    soc_field_t   base_qid[] = { BASE_QID0f, BASE_QID1f };
    soc_field_t   profile_idx[] = { Q_OFFSET_PROF0f, Q_OFFSET_PROF1f };


    if (map_id == NULL) {
        return BCM_E_INTERNAL;
    }

    if (set) {
        if ((*map_id >> _BCM_QOS_MAP_SHIFT) !=
            _BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP) {
           return BCM_E_PARAM;
        }
        /* Extract the profile_offset from QOS MAP*/
        profile = *map_id & _BCM_QOS_MAP_TYPE_MASK;

        if ((profile < KT2_MIN_RQE_PROFILE_INDEX ) ||
            (profile > KT2_MAX_RQE_PROFILE_INDEX)) {
            return BCM_E_PARAM;
        }
    }

    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST0m(unit, MEM_BLOCK_ANY,
                                      extq_ptr, &qlist0_entry)) < 0) {
        goto extq_list_done;
    }
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST1m(unit, MEM_BLOCK_ANY,
                                      extq_ptr, &qlist1_entry)) < 0) {
        goto extq_list_done;
    }
    if ((rv = READ_MMU_EXT_MC_QUEUE_LIST4m(unit, MEM_BLOCK_ANY,
                                      extq_ptr, &qlist4_entry)) < 0) {
        goto extq_list_done;
    }

    for (i = 0; i < 2; i++) {
        if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST0m_field32_get(unit,
                                    &qlist0_entry, base_qid[i])) != 0) {

            if (base_queue_id == queue_id) {
                if (set) {

                    /* set the profile index of the queue list entry */
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit,
                                        &qlist0_entry,
                                        profile_idx[i], profile);

                    rv = WRITE_MMU_EXT_MC_QUEUE_LIST0m(unit,
                                          MEM_BLOCK_ANY,
                                          extq_ptr, &qlist0_entry);
                } else {
                    profile = soc_mem_field32_get(unit,
                                    MMU_EXT_MC_QUEUE_LIST0m,
                                    &qlist0_entry, profile_idx[i]);
                    *map_id = profile | (_BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP
                                                     << _BCM_QOS_MAP_SHIFT);

                    rv = BCM_E_NONE;
                }

                goto extq_list_done;
            }
        }
    }
    for (i = 0; i < 2; i++) {
        if ((queue_id = soc_MMU_EXT_MC_QUEUE_LIST1m_field32_get(unit,
                                    &qlist1_entry, base_qid[i])) != 0) {
            if (base_queue_id == queue_id) {
                if (set) {

                    /* set the profile index of the queue list entry */
                    soc_MMU_EXT_MC_QUEUE_LIST0m_field32_set(unit,
                                        &qlist0_entry,
                                        profile_idx[i], profile);

                    rv = WRITE_MMU_EXT_MC_QUEUE_LIST0m(unit,
                                          MEM_BLOCK_ANY,
                                          extq_ptr, &qlist0_entry);
                } else {
                    profile = soc_mem_field32_get(unit,
                                    MMU_EXT_MC_QUEUE_LIST0m,
                                    &qlist0_entry, profile_idx[i]);
                    *map_id = profile | (_BCM_QOS_MAP_TYPE_RQE_QUEUE_OFFSET_MAP
                                                     << _BCM_QOS_MAP_SHIFT);
                    rv = BCM_E_NONE;
                }

                goto extq_list_done;
            }
        }
    }

    next_ptr1 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit,
                            &qlist4_entry, EXT_Q_NXT_PTR1f);
    next_ptr2 = soc_MMU_EXT_MC_QUEUE_LIST4m_field32_get(unit,
                            &qlist4_entry, EXT_Q_NXT_PTR2f);

    if (next_ptr1 != extq_ptr) {
        rv = _bcm_kt2_ipmc_subscriber_egress_queue_qos_map(unit, next_ptr1,
                                                 base_queue_id, map_id, set);
    }

    if (next_ptr2 != extq_ptr) {
        rv = _bcm_kt2_ipmc_subscriber_egress_queue_qos_map(unit, next_ptr2,
                                                 base_queue_id, map_id, set);
    }

extq_list_done:
    return rv;
}

/*
 * Function:
 *    _bcm_kt2_ipmc_subscriber_egress_intf_qos_map_set
 * Purpose:
 *    Assign the complete set of egress GPORTs in the
 *   replication list for the specified multicast index.
 * Parameters:
 *   unit              - (IN) Device Number
 *   group             - (IN) Multicast group ID
 *   port              - (IN) GPORT Identifier
 *   subscriber queue  - (IN) Subscriber queue group GPORT Identifiers
 *   qos_map_id        - (IN) Qos Map ID
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_kt2_ipmc_subscriber_egress_intf_qos_map_set(int unit,
                                        int ipmc_id,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int qos_map_id)
{
    int         rv = BCM_E_NONE;
    int         extq_ptr;
    int         queue_id;


    /*
     1. For ipmc_id traverse the queue list
     2. if queue matches the list entry set the profile
     3. else error
     */
    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_kt_init_check(unit, ipmc_id));

    if ((rv = _bcm_kt2_cosq_index_resolve(unit,  subscriber_queue, 0,
                           _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                           NULL,  &queue_id, NULL)) < 0) {
        goto clean_up;
    }

    IPMC_REPL_LOCK(unit);
    if ((rv = _kt_extq_mc_group_ptr(unit, ipmc_id,
                                &extq_ptr, 0, 0)) < 0) {
        goto clean_up;
    }

    if (extq_ptr == 0) {
        rv = BCM_E_INTERNAL;
        goto clean_up;
    }

    rv = _bcm_kt2_ipmc_subscriber_egress_queue_qos_map(unit, extq_ptr,
                                                  queue_id,
                                                  &qos_map_id,
                                                  1);

    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,"\n unit %d: Failure - setting subscriber queue %d Qos Map profile %d"),
                           unit, subscriber_queue, qos_map_id));
    } 
clean_up:
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *    _bcm_kt2_ipmc_subscriber_egress_intf_qos_map_get
 * Purpose:
 *    Assign the complete set of egress GPORTs in the
 *   replication list for the specified multicast index.
 * Parameters:
 *   unit              - (IN) Device Number
 *   group             - (IN) Multicast group ID
 *   port              - (IN) GPORT Identifier
 *   subscriber queue  - (IN) Subscriber queue group GPORT Identifiers
 *   qos_map_id        - (OUT) Qos Map ID
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_kt2_ipmc_subscriber_egress_intf_qos_map_get(int unit,
                                        int ipmc_id,
                                        bcm_gport_t port,
                                        bcm_gport_t subscriber_queue,
                                        int *qos_map_id)
{
    int         rv = BCM_E_NONE;
    int         extq_ptr;
    int         queue_id;


    /*
     1. For ipmc_id traverse the queue list
     2. if queue matches the list entry set the profile
     3. else error
     */
    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_kt_init_check(unit, ipmc_id));

    if (qos_map_id == NULL) {
        return BCM_E_INTERNAL;
    }
    IPMC_REPL_LOCK(unit);

    if ((rv = _bcm_kt2_cosq_index_resolve(unit,  subscriber_queue, 0,
                           _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                           NULL,  &queue_id, NULL)) < 0) {
        goto clean_up;
    }
    if ((rv = _kt_extq_mc_group_ptr(unit, ipmc_id,
                                &extq_ptr, 0, 0)) < 0) {
        goto clean_up;
    }

    if (extq_ptr == 0) {
        rv = BCM_E_INTERNAL;
        goto clean_up;
    }

    rv = _bcm_kt2_ipmc_subscriber_egress_queue_qos_map(unit, extq_ptr,
                                                  queue_id,
                                                  qos_map_id,
                                                  0);

    if (BCM_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_COMMON,
                      (BSL_META_U(unit,
        "unit %d: Failure - getting subscriber queue %d Qos Map profile %d"),
                           unit, subscriber_queue, *qos_map_id));
    }
clean_up:
    IPMC_REPL_UNLOCK(unit);
    return rv;
}
#endif /* BCM_KATANA2_SUPPORT */
/*
 * Function:
 *    _bcm_kt_ipmc_subscriber_egress_intf_set
 * Purpose:
 *    Assign set of egress interfaces to port's replication list for chosen
 *    IPMC group.
 * Parameters:
 *    unit     - StrataSwitch PCI device unit number.
 *    ipmc_id  - The index number.
 *    port     - port to list.
 *    subscriber_queue - subscriber's gport
 *    if_count - number of interfaces in replication list.
 *    if_array - (IN) List of interface numbers.
 *      check_port - (IN) If if_array consists of L3 interfaces, this parameter
 *                        controls whether to check the given port is a member
 *                        in each L3 interface's VLAN. This check should be
 *                        disabled when not needed, in order to improve
 *                        performance.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_kt_ipmc_subscriber_egress_intf_set(int unit, int ipmc_id,
                                        bcm_port_t port, int if_count,
                                        int *id,
                                        bcm_gport_t subscriber_queue,
                                        int is_l3,
                                        int *queue_count,
                                        int *q_count_increased)
{
    int     *if_array = NULL;
    int     q_num = 0;
    int     alloc_size;
    int     rv = BCM_E_NONE;
    int     queue_id;
    int     q_found = 0;
    int     q_max, ptr_type = 1;
    int     ipmc_ptr, i;
    int     new_ipmc_ptr = 0;
    int     last_ipmc_flag = 0;
    uint32     *q_repl_ptr = NULL;
    SHR_BITDCL *q_vec = NULL;
    int     partition = TD_EGR_L3_NEXT_HOP_OFFSET;
    int     nh_index = 0;
    int     port_qcount_increased = 0;
    uint16  sw_index = 0;
    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_kt_init_check(unit, ipmc_id));

    /* Hardware decides the replication on L3 interfaces or
     * Next hop based on L3_REPL_PTRX 13th bit
     * < 8k replication on Nexthop
     * > 8k replication on L3 interfaces
     */
    partition = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);

    q_max = IPMC_EXTQ_TOTAL(unit);
    alloc_size = q_max * sizeof(int);
    q_repl_ptr = sal_alloc(alloc_size, "IPMC repl interface array");
    if (q_repl_ptr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(q_repl_ptr, 0, alloc_size);

    alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_TOTAL(unit));
    q_vec = sal_alloc(alloc_size, "IPMC extq replication vector");
    if (q_vec == NULL) {
        sal_free(q_repl_ptr);
        return BCM_E_MEMORY;
    }
    sal_memset(q_vec, 0, alloc_size);

    IPMC_REPL_LOCK(unit);    

    if ((rv = _bcm_kt_ipmc_subscriber_queues_get(unit, ipmc_id, q_vec, 
                            q_repl_ptr, &q_num)) < 0 ) {
        goto intf_add_done;                            
    }
    
    if (q_num > q_max) {
        rv = BCM_E_PARAM;
        goto intf_add_done;                            
    }
    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(subscriber_queue)) {
        sw_index = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(
                      subscriber_queue);
    } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(subscriber_queue)) {
        sw_index = BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(
                      subscriber_queue);
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(subscriber_queue)) {
        sw_index = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(subscriber_queue);
    } else {
        rv = BCM_E_PARAM;
        goto intf_add_done;
    }
    if (sw_index >= IPMC_EXTQ_TOTAL(unit)) {
        rv = BCM_E_PARAM;
        goto intf_add_done;
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        if ((rv = _bcm_kt2_cosq_index_resolve(unit,  subscriber_queue, 0,
                                 _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                                 NULL,  &queue_id, NULL)) < 0) {
            goto intf_add_done;
        }
    } else /* BCM_KATANA2_SUPPORT */
#endif
    {
        if ((rv = _bcm_kt_cosq_index_resolve(unit,  subscriber_queue, 0,
                                _BCM_KT_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                                NULL,  &queue_id, NULL)) < 0) {
            goto intf_add_done;
        }
    }
    sw_to_hw_queue[unit][sw_index] = (queue_id << 8) | port;

    if (SHR_BITGET(q_vec, queue_id)) {
        q_found = 1;
        if (SOC_IS_KATANA2(unit)) {         
            ptr_type = (q_repl_ptr[queue_id] >> 13) & 0x3;  
        } else {
            ptr_type = (q_repl_ptr[queue_id] >> 14) & 1; 
        }
    }

    if (q_found) {
        if (SOC_IS_KATANA2(unit)) {
            ipmc_ptr = q_repl_ptr[queue_id] & 0x1fff;
            
        } else {
            ipmc_ptr = q_repl_ptr[queue_id] & 0x3fff;
        }
        if (ptr_type == 0) {
            /* already ipmc ptr */
            /* add this interface into ipmc chain */
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                if (if_count == 1) {
                rv = _bcm_kt2_ipmc_egress_intf_add(unit, ipmc_id, port,
                                                 ipmc_ptr, *id,
                                                 &new_ipmc_ptr,
                                                 &last_ipmc_flag);
                } else {
                  rv =  _bcm_kt2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                                 ipmc_ptr, if_count,
                                                 id, &new_ipmc_ptr,
                                                 &last_ipmc_flag, FALSE);

                }
                if (rv) { 
                    goto intf_add_done;
                }

            } 
#endif
            if (SOC_IS_KATANA(unit)) {
                if (if_count == 1) {
                     rv = _bcm_kt_ipmc_egress_intf_add(unit, ipmc_id, port,
                                                 ipmc_ptr, *id,
                                                 &new_ipmc_ptr,
                                                 &last_ipmc_flag);

                } else {
                   rv = _bcm_kt_ipmc_egress_intf_set(unit, ipmc_id, port,
                                                 ipmc_ptr, if_count,
                                                 id, &new_ipmc_ptr,
                                                 &last_ipmc_flag, FALSE);

                }            

                if (rv) { 
                    goto intf_add_done;
                } 
            }
            q_repl_ptr[queue_id] = (q_repl_ptr[queue_id] & (~0 << 14)) +
                                   new_ipmc_ptr;
            if (last_ipmc_flag) {
                q_repl_ptr[queue_id] |= (last_ipmc_flag << 17);
            }
            else { 
                q_repl_ptr[queue_id] &= ~(1 << 17);
            }
        } else {
            /* this piece of code should only execute for KATANA  
               as ptr_type will always set to 0 for KATANA2
             */
            /* its a l3 or nh entry */
            alloc_size =  (if_count + 1) * sizeof(int);
            if_array = sal_alloc(alloc_size, "IPMC repl interface array");
            if (if_array == NULL) {
                rv = BCM_E_MEMORY;
                goto intf_add_done;
            }
            if (ipmc_ptr < partition) {
                if_array[0] = ipmc_ptr + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
            } else {
                if_array[0] = ipmc_ptr - partition;
            }
            for (i = 0; i < if_count; i++) {
                if_array[1+i] = id[i];
            }
            _bcm_kt_ipmc_egress_intf_set(unit, ipmc_id, port, 0, (if_count + 1),
                                        if_array, &new_ipmc_ptr, &last_ipmc_flag, 
                                        FALSE);
            /* replace the ptr_type */
            q_repl_ptr[queue_id] = (q_repl_ptr[queue_id] & (~0 << 15)) + 
                                   new_ipmc_ptr; 
            
            if (last_ipmc_flag) {
                q_repl_ptr[queue_id] |= (last_ipmc_flag << 17);
            }
            else { 
                q_repl_ptr[queue_id] &= ~(1 << 17);
            }
        }
    }

    if (q_found == 0) {
        SHR_BITSET(q_vec, queue_id);
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            if (if_count == 1) {
                _bcm_kt2_ipmc_egress_intf_add(unit, ipmc_id, port,
                                              0, *id,
                                              &new_ipmc_ptr,
                                              &last_ipmc_flag);
            } else {
                _bcm_kt2_ipmc_egress_intf_set(unit, ipmc_id, port,
                                              0, if_count,
                                              id, &new_ipmc_ptr,
                                              &last_ipmc_flag, FALSE);

            }                 
            q_repl_ptr[queue_id] = new_ipmc_ptr;
            q_repl_ptr[queue_id] |= (last_ipmc_flag << 17);
        } 
#endif
        if (SOC_IS_KATANA(unit)) {
            /* set interface accordingly, the ptr_type = 1 */        
            if (if_count == 1) {
                /* Replication is next hop or L3 interface */                                 
                if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, *id)) {
                    /* L3 interface */
                    nh_index = partition + *id;
                } else {
                    /* Next hop entry */
                    nh_index = *id - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) ;
                }
                q_repl_ptr[queue_id] = (1 << 14) + nh_index;
            } else {
                _bcm_kt_ipmc_egress_intf_set(unit, ipmc_id, port, 0, if_count , id, 
                                          &new_ipmc_ptr, &last_ipmc_flag, FALSE);
                q_repl_ptr[queue_id] = new_ipmc_ptr; /* replace the ptr_type */
                q_repl_ptr[queue_id] |= (last_ipmc_flag << 17);

            }
        }
        q_num++;
        port_qcount_increased = 1;
    }

    if (q_num > IPMC_EXTQ_REPL_MAX(unit)) {
        rv = BCM_E_PARAM;
        goto intf_add_done;                                            
    }

    if (IS_EXT_MEM_PORT(unit, port)) {
        q_repl_ptr[queue_id] |= (1 << 16);
    }

    if ((rv = _bcm_kt_ipmc_extq_list_update(unit, ipmc_id, port, q_num, 
                                            q_vec, q_repl_ptr)) < 0 ) {
        goto intf_add_done;                                            
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
        rv = _bcm_kt2_intf_ref_cnt_increment(unit, sw_index, if_count);
    } else
#endif
    {
        rv = _bcm_kt_intf_ref_cnt_increment(unit, sw_index, if_count);
    }

    if (queue_count != NULL) {
        *queue_count = q_num;
    }

    if (q_count_increased != NULL) {
        *q_count_increased = port_qcount_increased;
    }

intf_add_done:
    IPMC_REPL_UNLOCK(unit);
    if (q_repl_ptr != NULL) {
        sal_free(q_repl_ptr);
    }
    if (q_vec != NULL) {
        sal_free(q_vec);
    }
    if (if_array != NULL) {
        sal_free(if_array);
    }
    return rv;

}

/*
 * Function:
 *    bcm_kt_ipmc_subscriber_egress_intf_get
 * Purpose:
 *    Retreieve set of egress interfaces in extended  queue's replication list
 *      for chosen IPMC group.
 * Parameters:
 *    unit     - StrataSwitch PCI device unit number.
 *    ipmc_id  - The index number.
 *    port     - port to list.
 *    subscriber_queue - subscriber gport 
 *    if_max   - maximum number of interfaces in replication list.
 *      if_array - (OUT) List of interface numbers.
 *    if_count - (OUT) number of interfaces returned in replication list.
 * Returns:
 *    BCM_E_XXX
 * Notes:
 *      If the input parameter if_max = 0, return in the output parameter
 *      if_count the total number of interfaces in the specified multicast
 *      group's replication list.
 */
int
_bcm_kt_ipmc_subscriber_egress_intf_get(int unit, int ipmc_id, int if_max, 
                                        bcm_gport_t *port_array, 
                                        bcm_if_t *if_array, 
                                        bcm_gport_t *subs_array, 
                                        int *count)
{
       int             rv = BCM_E_NONE;
       int             alloc_size;
       int             vlan_ptr, last_vlan_ptr;
       int             extq_ptr;    
       int             vlan_count;
       int             ls_pos;
       int             intf_base, nh_offset;
       int             i;    
       int             *baseq_array = NULL;
       int             *repl_ptr_array = NULL;
       int             q_count = 0;
       int             id = 0;
       int             partition = TD_EGR_L3_NEXT_HOP_OFFSET;
#ifdef BCM_KATANA2_SUPPORT
       int             temp_count = 0;    
       int             j = 0 ;  
#endif
       uint32          ls_bits[2];    
       mmu_ipmc_vlan_tbl_entry_t vlan_entry;
          
       
       BCM_IF_ERROR_RETURN(_bcm_kt_init_check(unit, ipmc_id));

       /* Hardware decides the replication on L3 interfaces or
        * Next hop based on L3_REPL_PTRX 13th bit
        * < 8k replication on Nexthop
        * > 8k replication on L3 interfaces
        */
       partition = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
 
       if (if_max < 0 || if_max > IPMC_EXTQ_TOTAL(unit)) {
           return BCM_E_PARAM;
       }
    
       alloc_size =  IPMC_EXTQ_REPL_MAX(unit) * sizeof(int);
       baseq_array = sal_alloc(alloc_size, "IPMC repl base queue array");
       if (baseq_array == NULL) {
            return BCM_E_MEMORY;
       }    
       repl_ptr_array = sal_alloc(alloc_size, "IPMC repl interface array");
       if (repl_ptr_array == NULL) {
            sal_free(baseq_array);
            return BCM_E_MEMORY;
       }    
           
       IPMC_REPL_LOCK(unit);

       if ((rv = _kt_extq_mc_group_ptr(unit, ipmc_id,
                                &extq_ptr, 0, 0)) < 0) {
            goto intf_get_done;
        }

        if (extq_ptr != 0) {
           rv = _bcm_kt_ipmc_subscriber_egress_intf_traverse(unit, extq_ptr, 
                                      baseq_array, repl_ptr_array, &q_count);
        }

       vlan_count = 0;    
       for (i = 0; i < q_count; i++) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                if ((repl_ptr_array[i] >> 13) & 0x3) {
                    rv = BCM_E_INTERNAL;
                    goto intf_get_done;
                } else {
                    vlan_ptr = repl_ptr_array[i]  & 0x1fff;
                   
                    rv = _bcm_kt2_ipmc_egress_intf_get (unit, ipmc_id, 
                                                  vlan_ptr,
                                                  &if_array[vlan_count], &temp_count);
                    if(rv != BCM_E_NONE)
                       goto intf_get_done;
                    j = vlan_count;  
                    vlan_count += temp_count;
                     
                    if(vlan_count > if_max) 
                       goto intf_get_done;
                    for( ; j < vlan_count; j++) 
                    {
                        subs_array[j] = baseq_array[i] ; 
                    }
                }
            
            } 
#endif
            if (SOC_IS_KATANA(unit)) {
                if ((repl_ptr_array[i] >> 14) & 1) { /* if its a nh or l3 intf */

                    id = repl_ptr_array[i] & ~(1 << 14);
                    if (id < partition)
                    {
                        /* Next hop */
                        if_array[vlan_count] =  id + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                    } else {
                        /* L3 interface */
                        if_array[vlan_count] =  id - partition;
                    }
                    subs_array[vlan_count++] = baseq_array[i]; 

                    if (vlan_count == (uint32)if_max) {
                        *count = vlan_count;
                        goto intf_get_done;
                    }
                } else { /* its a ipmc ptr */
                    vlan_ptr = repl_ptr_array[i]  & 0x3fff; 
                    last_vlan_ptr = -1;
                    if (SOC_IS_TD_TT(unit)) {
                        nh_offset = TD_EGR_L3_NEXT_HOP_OFFSET;
                    } else {
                        /* coverity[large_shift : FALSE] */
                        nh_offset = (1 << (soc_mem_field_length(unit, 
                                                      MMU_IPMC_VLAN_TBLm,
                                                      MSB_VLANf) - 1)) * 64;
                    }
                    while (vlan_ptr != last_vlan_ptr) {
                        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                                                     vlan_ptr, &vlan_entry)) < 0) {
                            goto intf_get_done;
                        }
                        /* Each MSB represents 64 entries in LSB bitmap */
                        intf_base = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit, &vlan_entry,
                                                              MSB_VLANf) * 64;
                        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                                        LSB_VLAN_BMf, ls_bits);
                        /* Using MSB, check if L3 interface or next hop */
    
                        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
                            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                                if (if_max == 0) {
                                    vlan_count++;
                                } else {
                                    subs_array[vlan_count] = baseq_array[i];
                                    /* Check if L3 interface or next hop */
                                    if (intf_base >= nh_offset) {
                                    /* Entry contains next hops */
                                    if_array[vlan_count++] = intf_base - nh_offset +
                                                                ls_pos + 
                                                                BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                                    } else {
                                        /* Entry contains interfaces */
                                        if_array[vlan_count++] = intf_base + ls_pos;
                                    }
                                    if (vlan_count == (uint32)if_max) {
                                        *count = vlan_count;
                                        goto intf_get_done;
                                    }
                                }
                            }
                        }
                        last_vlan_ptr = vlan_ptr;
                        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                         &vlan_entry, NEXTPTRf);
                    }        
                }
            } 
       }

       *count = vlan_count;
    
    intf_get_done:
       IPMC_REPL_UNLOCK(unit);
       if (baseq_array != NULL) {
           sal_free(baseq_array);
       }
       if (repl_ptr_array != NULL) {
           sal_free(repl_ptr_array);
       }    
       return rv;
}

#if defined(BCM_KATANA_SUPPORT)
int
_bcm_kt_ipmc_egress_intf_delete_all( int unit, int ipmc_id, int ipmc_ptr)
{
    bcm_if_t *if_array = NULL;
    int alloc_size, intf_max, if_count = 0, rv = BCM_E_NONE;
    SHR_BITDCL *intf_vec = NULL;
    int if_num, partition;
    int intf_count= 0, repl_hash;
    int nh_offset, intf_base;
    uint32 ls_bits[2];
    int ls_pos; 
    int vlan_ptr, last_vlan_ptr;
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    _bcm_repl_list_info_t *rli_start,*rli_current, *rli_prev = NULL;
    uint32 nh_count = 0;

    partition = soc_mem_index_count(unit, EGR_L3_INTFm);
    intf_max = IPMC_REPL_INTF_TOTAL(unit);
    alloc_size = intf_max *sizeof(bcm_if_t);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }
    alloc_size = SHR_BITALLOCSIZE(intf_max);
    intf_vec = sal_alloc(alloc_size, "Repl interface vector");
    if (intf_vec == NULL) {
        sal_free(if_array);
        return BCM_E_MEMORY;
    }
    sal_memset(intf_vec, 0, alloc_size);
    IPMC_REPL_LOCK(unit);
    vlan_ptr = ipmc_ptr;
    last_vlan_ptr = -1;

    if (SOC_IS_TD_TT(unit)) {
        nh_offset = TD_EGR_L3_NEXT_HOP_OFFSET;
    } else {
        /* coverity[large_shift : FALSE] */
        /* coverity[negative_shift : FALSE] */
        nh_offset = (1 << (soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm,
                        MSB_VLANf) - 1)) * 64;
    }
    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                        vlan_ptr, &vlan_entry)) < 0) {
            goto error;
        }
        /* Each MSB represents 64 entries in LSB bitmap */
        intf_base = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit, &vlan_entry,
                MSB_VLANf) * 64;
        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                LSB_VLAN_BMf, ls_bits);
        /* Using MSB, check if L3 interface or next hop */

        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                if (intf_max == 0) {
                    if_count++;
                } else {
                    /* Check if L3 interface or next hop */
                    if (intf_base >= nh_offset) {
                        /* Entry contains next hops */
                        if_array[if_count++] = intf_base - nh_offset +
                            ls_pos + BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                    } else {
                        /* Entry contains interfaces */
                        if_array[if_count++] = intf_base + ls_pos;
                    }
                    if (if_count == (uint32)intf_max) {
                        break;
                    }
                }
            }
        }
        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                &vlan_entry, NEXTPTRf);

    } 
    /*prepare the interface vector and find out the refrence count */
    for (if_num = 0; if_num < if_count; if_num++) {
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num]) &&
                (uint32)(if_array[if_num]) > partition) {
            rv = BCM_E_PARAM;
            break;
        }
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, if_array[if_num])) {
            /* L3 interface is used */
            SHR_BITSET(intf_vec, if_array[if_num]);
            intf_count++;
        } else {
            /* Next hop is used */
            SHR_BITSET(intf_vec, partition + if_array[if_num] -
                    BCM_XGS3_DVP_EGRESS_IDX_MIN(unit));
            nh_count++;
        }
    }
    rli_start = IPMC_REPL_LIST_INFO(unit);
    repl_hash =
        _shr_crc32b(0, (uint8 *)intf_vec, IPMC_REPL_INTF_TOTAL(unit));
    for (rli_current = rli_start; rli_current != NULL;
            rli_current = rli_current->next) {
        if (repl_hash == rli_current->hash) {
            rv = _bcm_tr2_repl_list_compare(unit, rli_current->index,
                    intf_vec);
            if (rv == BCM_E_NOT_FOUND) {
                continue; /* Not a match */
            } else if (rv < 0) {
                goto error; /* Access error */
            } else {
                break; /* Match */
            }
        }
        rli_prev = rli_current;
    }

    if (rli_current){
        rli_current->refcount-- ;
        if (rli_current->refcount == 0){
            rv = _bcm_tr2_repl_list_free(unit, rli_current->index);
            /* If we have an error, we'll fall out anyway */
            if (rli_prev == NULL) {
                IPMC_REPL_LIST_INFO(unit) = rli_current->next;
            } else {
                rli_prev->next = rli_current->next;
            }
            sal_free(rli_current);
        }
    }
error:
    sal_free(if_array);
    sal_free(intf_vec);
    IPMC_REPL_UNLOCK(unit);
    return rv;
}


int
_bcm_kt_ipmc_subscriber_egress_intf_delete_all(int unit, int ipmc_id,
                                           int port_count,
                                           bcm_port_t *port_array,
                                           int *encap_array,
                                           bcm_if_t *q_array,
                                           int *q_count_decreased)
{
    bcm_error_t rv = BCM_E_NONE;
    int alloc_size, q_max;
    uint32 *q_repl_ptr = NULL;
    SHR_BITDCL *q_vec = NULL;
    int q_num = 0;
    int queue_id;
    int ipmc_ptr, ptr_type = 0;
    int i, sw_index = 0;
    bcm_port_t port = 0;

    q_max = _kt_extq_repl_info[unit]->queue_num;
    /*queue replication pointers array*/
    alloc_size = q_max * sizeof(int);
    q_repl_ptr = sal_alloc(alloc_size, "IPMC repl interface array");
    if (q_repl_ptr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(q_repl_ptr, 0, alloc_size);

    alloc_size = SHR_BITALLOCSIZE(q_max);
    q_vec = sal_alloc(alloc_size, "IPMC extq replication vector");
    if (q_vec == NULL) {
        sal_free(q_repl_ptr);
        return BCM_E_MEMORY;
    }
    sal_memset(q_vec, 0, alloc_size);

    IPMC_REPL_LOCK(unit);
    /*get the queue vector for the ipmc id */
    if ((rv = _bcm_kt_ipmc_subscriber_queues_get(unit, ipmc_id, q_vec,
                    q_repl_ptr, &q_num)) < 0 ) {
        goto intf_add_done;
    }

    if (q_num > q_max) {
        rv = BCM_E_PARAM;
        goto intf_add_done;
    }
    for (i = 0; i < port_count; i++) {
        /* get ipmc ptr and delete all interfaces associated with it */
        queue_id = q_array[i];
        if (queue_id == q_array[i+1]) {
            continue;
        }
        if (SOC_IS_KATANA2(unit)) {
            ptr_type = (q_repl_ptr[queue_id] >> 13) & 0x3;
            ipmc_ptr = q_repl_ptr[queue_id] & 0x1fff;
        } else {
            ptr_type = (q_repl_ptr[queue_id] >> 14) & 1;
            ipmc_ptr = q_repl_ptr[queue_id] & 0x3fff;
        }

        if (ptr_type == 0) {
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2 (unit)) {
                rv = _bcm_kt2_ipmc_egress_intf_delete_all(unit, ipmc_id,
                                                          ipmc_ptr);
            } else
#endif /* BCM_KATANA2_SUPPORT */
            {
                rv = _bcm_kt_ipmc_egress_intf_delete_all(unit, ipmc_id,
                                                         ipmc_ptr);
            }
            if (BCM_FAILURE(rv)) {
                goto intf_add_done;
            }
        }
        q_repl_ptr[queue_id] = 0; 
    }
    alloc_size = SHR_BITALLOCSIZE(q_max);
    sal_memset(q_vec, 0, alloc_size);
    *q_count_decreased = q_num;
    q_num = 0;
    rv = _bcm_kt_ipmc_extq_list_update_all(unit, ipmc_id, q_num,
            q_vec, q_repl_ptr);
    if (BCM_FAILURE(rv)) {
        goto intf_add_done;
    }
    for (i = 0; i < port_count; i++) {
        sw_index = q_array[i];
        rv = _bcm_kt_hw_to_sw_queue(unit, &sw_index, &port);
        if (BCM_FAILURE(rv)) {
            goto intf_add_done;
        }
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
            rv = _bcm_kt2_intf_ref_cnt_decrement(unit, sw_index, 1);
            if (BCM_FAILURE(rv)) {
                goto intf_add_done;
            }
        } else
#endif
        {
            rv = _bcm_kt_intf_ref_cnt_decrement(unit, sw_index, 1);
            if (BCM_FAILURE(rv)) {
                goto intf_add_done;
            }
        }
    }
intf_add_done:
    IPMC_REPL_UNLOCK(unit);
    if (q_repl_ptr != NULL) {
        sal_free(q_repl_ptr);
    }
    if (q_vec != NULL) {
        sal_free(q_vec);
    }
    return rv;
}
#endif

/*
 * Function:
 *    bcm_kt_ipmc_subscriber_egress_intf_delete
 * Purpose:
 *    Assign set of egress interfaces to port's replication list for chosen
 *    IPMC group.
 * Parameters:
 *    unit     - StrataSwitch PCI device unit number.
 *    ipmc_id  - The index number.
 *    port     - port to list.
 *    subscriber_queue - subscriber's gport
 *    encap_id - (IN) List of interface numbers.
 *      check_port - (IN) If if_array consists of L3 interfaces, this parameter
 *                        controls whether to check the given port is a member
 *                        in each L3 interface's VLAN. This check should be
 *                        disabled when not needed, in order to improve
 *                        performance.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_kt_ipmc_subscriber_egress_intf_delete(int unit, int ipmc_id,
                                           bcm_port_t port, 
                                           int id,
                                           bcm_gport_t subscriber_queue,
                                           int *queue_count,
                                           int *q_count_decreased)
{
    int     q_num = 0;
    int     alloc_size;
    int     rv = BCM_E_NONE;
    int     queue_id;
    int     q_found = 0;
    int     q_max, ptr_type = 0;
    int     ipmc_ptr;
    int     new_ipmc_ptr = 0;
    int     last_ipmc_flag = 0;
    uint32     *q_repl_ptr = NULL;
    SHR_BITDCL *q_vec = NULL;
    int     nh_index = 0;
    int     partition = TD_EGR_L3_NEXT_HOP_OFFSET;
    int     port_qcount_decreased = 0;
    uint16  sw_index = 0;

    REPL_PORT_CHECK(unit, port);
    BCM_IF_ERROR_RETURN(_bcm_kt_init_check(unit, ipmc_id));

    /* Hardware decides the replication on L3 interfaces or
     * Next hop based on L3_REPL_PTRX 13th bit
     * < 8k replication on Nexthop
     * > 8k replication on L3 interfaces
     */
    partition = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
 
    q_max = IPMC_EXTQ_TOTAL(unit);
    alloc_size = q_max * sizeof(int);
    q_repl_ptr = sal_alloc(alloc_size, "IPMC repl interface array");
    if (q_repl_ptr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(q_repl_ptr, 0, alloc_size);

    alloc_size = SHR_BITALLOCSIZE(IPMC_EXTQ_TOTAL(unit));
    q_vec = sal_alloc(alloc_size, "IPMC extq replication vector");
    if (q_vec == NULL) {
        sal_free(q_repl_ptr);
        return BCM_E_MEMORY;
    }
    sal_memset(q_vec, 0, alloc_size);

    IPMC_REPL_LOCK(unit);    

    if ((rv = _bcm_kt_ipmc_subscriber_queues_get(unit, ipmc_id, q_vec, 
                            q_repl_ptr, &q_num)) < 0 ) {
        goto intf_add_done;
    }
    
    if (q_num > q_max) {
        rv = BCM_E_PARAM;
        goto intf_add_done;
    }
    if (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(subscriber_queue)) {
        sw_index = BCM_GPORT_UCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(
                      subscriber_queue);
    } else if (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(subscriber_queue)) {
        sw_index = BCM_GPORT_MCAST_SUBSCRIBER_QUEUE_GROUP_QID_GET(
                      subscriber_queue);
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(subscriber_queue)) {
        sw_index = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(subscriber_queue);
    } else {
        rv = BCM_E_PARAM;
        goto intf_add_done;
    }
    if (sw_index >= IPMC_EXTQ_TOTAL(unit)) {
        rv = BCM_E_PARAM;
        goto intf_add_done;
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        if ((rv = _bcm_kt2_cosq_index_resolve(unit,  subscriber_queue, 0,
                                 _BCM_KT2_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                                 NULL,  &queue_id, NULL)) < 0) {
            if (rv == BCM_E_NOT_FOUND) {
                queue_id = sw_to_hw_queue[unit][sw_index] >> 8;
            } else {
                goto intf_add_done;
            }
        }

    } else
#endif /* BCM_KATANA2_SUPPORT */
    {
        if ((rv = _bcm_kt_cosq_index_resolve(unit,  subscriber_queue, 0,
                                _BCM_KT_COSQ_INDEX_STYLE_QUEUE_REPLICATION,
                                NULL,  &queue_id, NULL)) < 0) {
            if (rv == BCM_E_NOT_FOUND) {
                queue_id = sw_to_hw_queue[unit][sw_index] >> 8;
            } else {
                goto intf_add_done;
            }
        }
    }

    if (SHR_BITGET(q_vec, queue_id)) {
        q_found = 1;
        if (SOC_IS_KATANA2(unit)) {
            ptr_type = (q_repl_ptr[queue_id] >> 13) & 0x3; 
            ipmc_ptr = q_repl_ptr[queue_id] & 0x1fff;
        } else {
            ptr_type = (q_repl_ptr[queue_id] >> 14) & 1;
            ipmc_ptr = q_repl_ptr[queue_id] & 0x3fff;
        } 
    }

    if (q_found == 0) {
        rv = BCM_E_NOT_FOUND;
        goto intf_add_done;
    }
    
    if (ptr_type == 0) {
        /* already ipmc ptr */        
#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            _bcm_kt2_ipmc_egress_intf_delete(unit, ipmc_id, port, ipmc_ptr, id, 
                                             &new_ipmc_ptr, &last_ipmc_flag);
        } 
#endif
        if (SOC_IS_KATANA(unit)) {
            _bcm_kt_ipmc_egress_intf_delete(unit, ipmc_id, port, ipmc_ptr, id, 
                                            &new_ipmc_ptr, &last_ipmc_flag);         
        }
        if (new_ipmc_ptr != 0) {
            q_repl_ptr[queue_id] = (q_repl_ptr[queue_id] & (~0 << 14)) +
                                    new_ipmc_ptr;
            q_repl_ptr[queue_id] |= (last_ipmc_flag << 17);
        } else { 
            /* this is the last entry */
            q_repl_ptr[queue_id] = 0;
            SHR_BITCLR(q_vec, queue_id);
            q_num--;
            port_qcount_decreased = 1;
        }

    } else {
        /* its a l3 or nh entry */
        if (!BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, id)) {
            /* L3 interface */
            nh_index = id + partition;
        } else {
            /* Next hop entry */
            nh_index = id - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }
        if (nh_index != ipmc_ptr) {
            /* encap id does not exist */
            rv = BCM_E_PARAM;
            goto intf_add_done;
        }
        q_repl_ptr[queue_id] = 0;
        SHR_BITCLR(q_vec, queue_id); 
        q_num--;
        port_qcount_decreased = 1;
    }

    if ((rv = _bcm_kt_ipmc_extq_list_update(unit, ipmc_id, port, q_num, 
                                            q_vec, q_repl_ptr)) < 0 ) {
        goto intf_add_done;
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
        rv = _bcm_kt2_intf_ref_cnt_decrement(unit, sw_index, 1);
    } else
#endif
    {
        rv = _bcm_kt_intf_ref_cnt_decrement(unit, sw_index, 1);
    }

    if (queue_count != NULL) {
        *queue_count = q_num;
    }

    if (q_count_decreased != NULL) {
        *q_count_decreased = port_qcount_decreased;
    }

intf_add_done:
    IPMC_REPL_UNLOCK(unit);
    if (q_repl_ptr != NULL) {
        sal_free(q_repl_ptr);
    }
    if (q_vec != NULL) {
        sal_free(q_vec);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_kt_hw_to_sw_queue
 * Purpose:
 *      Finds sw queue index for the corresponding hw queue
 * Parameters:
 *      unit        - (IN) StrataSwitch unit #
 *      queue_array - (IN/OUT) Queue array pointer
 *      port        - (OUT) Pointer to Port
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_kt_hw_to_sw_queue(int unit, int *queue_array, bcm_port_t *port)
{
    int index;
    for (index = 0; index < IPMC_EXTQ_TOTAL(unit); index++) {
         if ((sw_to_hw_queue[unit][index] >> 8) == *queue_array) {
             *queue_array = index;
             *port = sw_to_hw_queue[unit][index] & 0xff;
             return BCM_E_NONE;
         }
    }
    *queue_array = -1;
    *port = -1;
    return BCM_E_NOT_FOUND;
}

#endif

#ifndef BCM_SW_STATE_DUMP_DISABLE
#ifdef BCM_KATANA2_SUPPORT
/*
 * Function:
 *     bcm_kt2_ipmc_subscriber_sw_dump
 * Purpose:
 *     Displays IPMC Subscriber replication information 
 *     maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void 
bcm_kt2_ipmc_subscriber_sw_dump(int unit)
{
    int                   i, j;
    uint32                *bitmap;
    _bcm_repl_list_info_t *rli_start, *rli_current;
    _kt_repl_queue_info_t  *group_info;
    _kt_extq_repl_info_t   *extq_info;


    /*
     * _kt_extq_repl_info
     */

    LOG_CLI((BSL_META_U(unit,
                        "  IPMC EXTQ REPL Info -\n")));
    extq_info = _kt_extq_repl_info[unit];
    LOG_CLI((BSL_META_U(unit,
                        "    IPMC Size    : %d\n"), extq_info->ipmc_size));
    LOG_CLI((BSL_META_U(unit,
                        "    Queue Num    : %d\n"), extq_info->queue_num));
    LOG_CLI((BSL_META_U(unit,
                        "    Extq total   : %d\n"), extq_info->extq_list_total));

    LOG_CLI((BSL_META_U(unit,
                        "    Bitmap (index:value-hex) :")));
    if (extq_info->bitmap_entries_used != NULL) {
        bitmap =  extq_info->bitmap_entries_used;
        for (i = 0, j = 0;
             i < _SHR_BITDCLSIZE(extq_info->extq_list_total); i++) {
            /* If zero, skip print */
            if (bitmap[i] == 0) {
                continue;
            }
            if (!(j % 4)) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "  %5d:%8.8x"), i, bitmap[i]));
            j++;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    group_info = extq_info->mcgroup_info;
    for (i = 0, j = 0; i < extq_info->ipmc_size; i++) {
        /* If zero, skip print */
        if (group_info->queue_count[i] == 0) {
            continue;
        }
        if ((j > 0) && !(j % 4)) {
            LOG_CLI((BSL_META_U(unit,
                                "\n         ")));
        }
        LOG_CLI((BSL_META_U(unit,
                            " %5d:%-4d"), i, group_info->queue_count[i]));
        j++;
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
   
    rli_start = IPMC_EXTQ_REPL_LIST_INFO(unit);
    LOG_CLI((BSL_META_U(unit,
                        "    List Info    -\n")));
    for (rli_current = rli_start; rli_current != NULL;
         rli_current = rli_current->next) {
        LOG_CLI((BSL_META_U(unit,
                            "    Hash:  0x%08x\n"), rli_current->hash));
        LOG_CLI((BSL_META_U(unit,
                            "    Index: %4d\n"), rli_current->index));
        LOG_CLI((BSL_META_U(unit,
                            "    Size:  %4d\n"), rli_current->list_size));
        LOG_CLI((BSL_META_U(unit,
                            "    Refs:  %4d\n"), rli_current->refcount));
    }
}

#endif
/*
 * Function:
 *     _bcm_tr2_ipmc_repl_sw_dump
 * Purpose:
 *     Displays IPMC replication information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_tr2_ipmc_repl_sw_dump(int unit)
{
    int                   i, j, port;
    _tr2_repl_info_t       *repl_info;
    uint32                *bitmap;
    _tr2_repl_port_info_t  *port_info;
    _bcm_repl_list_info_t *rli_start, *rli_current;
#ifdef BCM_KATANA_SUPPORT
    _kt_repl_queue_info_t  *group_info;
    _kt_extq_repl_info_t   *extq_info;
#endif
    /*
     * _tr2_repl_info
     */

    LOG_CLI((BSL_META_U(unit,
                        "  IPMC REPL Info -\n")));
    repl_info = _tr2_repl_info[unit];
    if (repl_info != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "    IPMC Size    : %d\n"), repl_info->ipmc_size));
        LOG_CLI((BSL_META_U(unit,
                            "    Intf Size    : %d\n"), repl_info->intf_num));
        LOG_CLI((BSL_META_U(unit,
                            "    Vlan total   : %d\n"), repl_info->ipmc_vlan_total));

        LOG_CLI((BSL_META_U(unit,
                            "    Bitmap (index:value-hex) :")));
        if (repl_info->bitmap_entries_used != NULL) {
            bitmap =  repl_info->bitmap_entries_used;
            for (i = 0, j = 0;
                 i < _SHR_BITDCLSIZE(repl_info->ipmc_vlan_total); i++) {
                /* If zero, skip print */
                if (bitmap[i] == 0) {
                    continue;
                }
                if (!(j % 4)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "  %5d:%8.8x"), i, bitmap[i]));
                j++;
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "\n")));

        LOG_CLI((BSL_META_U(unit,
                            "    Port Info    -\n")));
        LOG_CLI((BSL_META_U(unit,
                            "    port (index:vlan-count) :\n")));
        for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
            port_info = repl_info->port_info[port];
            LOG_CLI((BSL_META_U(unit,
                                "    %3d -"), port));
            if (port_info == NULL) {
                LOG_CLI((BSL_META_U(unit,
                                    " null\n")));
                continue;
            }
            for (i = 0, j = 0; i < repl_info->ipmc_size; i++) {
                /* If zero, skip print */
                if (port_info->vlan_count[i] == 0) {
                    continue;
                }
                if ((j > 0) && !(j % 4)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n         ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %5d:%-4d"), i, port_info->vlan_count[i]));
                j++;
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        }

        
        rli_start = IPMC_REPL_LIST_INFO(unit);
        LOG_CLI((BSL_META_U(unit,
                            "    List Info    -\n")));
        for (rli_current = rli_start; rli_current != NULL;
             rli_current = rli_current->next) {
            LOG_CLI((BSL_META_U(unit,
                                "    Hash:  0x%08x\n"), rli_current->hash));
            LOG_CLI((BSL_META_U(unit,
                                "    Index: %4d\n"), rli_current->index));
            LOG_CLI((BSL_META_U(unit,
                                "    Size:  %4d\n"), rli_current->list_size));
            LOG_CLI((BSL_META_U(unit,
                                "    Refs:  %4d\n"), rli_current->refcount));
        }
    }

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        /*
         * _kt_extq_repl_info
         */

        LOG_CLI((BSL_META_U(unit,
                            "  IPMC EXTQ REPL Info -\n")));
        extq_info = _kt_extq_repl_info[unit];
        if (repl_info != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "    IPMC Size    : %d\n"), extq_info->ipmc_size));
            LOG_CLI((BSL_META_U(unit,
                                "    Queue Num    : %d\n"), extq_info->queue_num));
            LOG_CLI((BSL_META_U(unit,
                                "    Extq total   : %d\n"), extq_info->extq_list_total));

            LOG_CLI((BSL_META_U(unit,
                                "    Bitmap (index:value-hex) :")));
            if (extq_info->bitmap_entries_used != NULL) {
                bitmap =  extq_info->bitmap_entries_used;
                for (i = 0, j = 0;
                     i < _SHR_BITDCLSIZE(extq_info->extq_list_total); i++) {
                    /* If zero, skip print */
                    if (bitmap[i] == 0) {
                        continue;
                    }
                    if (!(j % 4)) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n    ")));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "  %5d:%8.8x"), i, bitmap[i]));
                    j++;
                }
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));

            group_info = extq_info->mcgroup_info;
            for (i = 0, j = 0; i < extq_info->ipmc_size; i++) {
                /* If zero, skip print */
                if (group_info->queue_count[i] == 0) {
                    continue;
                }
                if ((j > 0) && !(j % 4)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n         ")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    " %5d:%-4d"), i, group_info->queue_count[i]));
                j++;
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
            
            rli_start = IPMC_EXTQ_REPL_LIST_INFO(unit);
            LOG_CLI((BSL_META_U(unit,
                                "    List Info    -\n")));
            for (rli_current = rli_start; rli_current != NULL;
                 rli_current = rli_current->next) {
                LOG_CLI((BSL_META_U(unit,
                                    "    Hash:  0x%08x\n"), rli_current->hash));
                LOG_CLI((BSL_META_U(unit,
                                    "    Index: %4d\n"), rli_current->index));
                LOG_CLI((BSL_META_U(unit,
                                    "    Size:  %4d\n"), rli_current->list_size));
                LOG_CLI((BSL_META_U(unit,
                                    "    Refs:  %4d\n"), rli_current->refcount));
            }
        }
        
    }    
#endif

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* INCLUDE_L3 */
