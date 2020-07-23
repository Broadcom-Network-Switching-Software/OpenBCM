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
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <bcm_int/esw/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#include <bcm_int/esw_dispatch.h>

#define REPL_PORT_CHECK(unit, port) \
    if (!IS_PORT(unit, port)) { return BCM_E_PARAM; }

#if defined(BCM_FIREBOLT_SUPPORT)
typedef struct _fb_repl_port_info_s {
    uint32 *vlan_count;                       /* # VLANs the port repl to */
    uint32 emc_val;                           /* EGRMETERINGCONFIG value */
} _fb_repl_port_info_t;

typedef struct _fb_repl_info_s {
    int ipmc_size;
    uint32 intf_num;              /* Number of interfaces on this device */
    uint16 ipmc_vlan_total;       /* Keep track of total and */
    uint32 *bitmap_entries_used;  /* free entries of IPMC_VLAN table */
    _bcm_repl_list_info_t *repl_list_info;
    _fb_repl_port_info_t *port_info[SOC_MAX_NUM_PORTS];

    /* Per port replication info */
    int *l3_intf_to_l3_nh_ipmc; /* L3_NH array : for L3_INTF based IPMC repl */
    int *l3_nh_to_l3_intf_ipmc;
} _fb_repl_info_t;

static _fb_repl_info_t *_fb_repl_info[BCM_MAX_NUM_UNITS];

#define IPMC_REPL_LOCK(_u_)                    \
    {                                          \
        soc_mem_lock(_u_, MMU_IPMC_VLAN_TBLm); \
    }
#define IPMC_REPL_UNLOCK(_u_)                    \
    {                                            \
        soc_mem_unlock(_u_, MMU_IPMC_VLAN_TBLm); \
    }
#define IPMC_REPL_ID(_u_, _id_) \
        if ((_id_ < 0) || (_id_ >= _fb_repl_info[_u_]->ipmc_size)) \
            { return BCM_E_PARAM; }
#define IPMC_REPL_INIT(unit) \
        if (_fb_repl_info[unit] == NULL) { return BCM_E_INIT; }
#define IPMC_REPL_TOTAL(_u_) \
        _fb_repl_info[_u_]->ipmc_vlan_total
#define IPMC_REPL_GROUP_NUM(_u_) \
        _fb_repl_info[_u_]->ipmc_size
#define IPMC_REPL_INTF_TOTAL(_u_) \
        _fb_repl_info[_u_]->intf_num
#define IPMC_REPL_VE_USED_GET(_u_, _i_) \
        SHR_BITGET(_fb_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_REPL_VE_USED_SET(_u_, _i_) \
        SHR_BITSET(_fb_repl_info[_u_]->bitmap_entries_used, _i_)
#define IPMC_REPL_VE_USED_CLR(_u_, _i_) \
        SHR_BITCLR(_fb_repl_info[_u_]->bitmap_entries_used, _i_)

#define IPMC_REPL_LIST_INFO(_u_) \
	_fb_repl_info[_u_]->repl_list_info
#define IPMC_REPL_PORT_INFO(_u_, _p_) \
        _fb_repl_info[_u_]->port_info[_p_]
#define IPMC_REPL_PORT_VLAN_COUNT(_u_, _p_, _ipmc_id_) \
        _fb_repl_info[_u_]->port_info[_p_]->vlan_count[_ipmc_id_]

#define IPMC_REPL_L3_INTF_TO_L3_NH_IPMC(_u_, _intf_) \
        _fb_repl_info[(_u_)]->l3_intf_to_l3_nh_ipmc[(_intf_)]

#define IPMC_REPL_L3_NH_TO_L3_INTF_IPMC(_u_, nh_id) \
        _fb_repl_info[(_u_)]->l3_nh_to_l3_intf_ipmc[(nh_id)]

#define REPL_NH_INDEX_UNALLOCATED (-1)
#define REPL_L3_INTF_UNALLOCATED (-1)


typedef struct _rep_regs_s {
    soc_field_t port_ptr;
    soc_field_t last_ptr;
    soc_mem_t   mem;
}_rep_regs_t;


_rep_regs_t fb_rep_regs[28] = { {PORT0_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                {PORT1_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                {PORT2_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                {PORT3_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                {PORT4_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                {PORT5_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                {PORT6_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                {PORT7_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                {PORT8_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                {PORT9_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                {PORT10_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                {PORT11_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                {PORT12_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                {PORT13_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                {PORT14_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                {PORT15_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                {PORT16_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                {PORT17_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                {PORT18_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                {PORT19_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                {PORT20_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                {PORT21_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                {PORT22_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                {PORT23_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                {PORT24_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL4m},
                                {PORT25_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL5m},
                                {PORT26_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL6m},
                                {PORT27_1STPTRf, INVALIDf, MMU_IPMC_GROUP_TBL7m}
};
#if defined(BCM_HB_GW_SUPPORT)
_rep_regs_t hbgw_rep_regs[20] = { {PORT0_1STPTRf,  PORT0_LASTf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT1_1STPTRf,  PORT1_LASTf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT2_1STPTRf,  PORT2_LASTf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT3_1STPTRf,  PORT3_LASTf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT4_1STPTRf,  PORT4_LASTf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT5_1STPTRf,  PORT5_LASTf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT6_1STPTRf,  PORT6_LASTf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT7_1STPTRf,  PORT7_LASTf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT8_1STPTRf,  PORT8_LASTf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT9_1STPTRf,  PORT9_LASTf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT10_1STPTRf,  PORT10_LASTf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT11_1STPTRf,  PORT11_LASTf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT12_1STPTRf,  PORT12_LASTf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT13_1STPTRf,  PORT13_LASTf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT14_1STPTRf,  PORT14_LASTf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT15_1STPTRf,  PORT15_LASTf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT16_1STPTRf,  PORT16_LASTf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT17_1STPTRf,  PORT17_LASTf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT18_1STPTRf,  PORT18_LASTf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT19_1STPTRf,  PORT19_LASTf, MMU_IPMC_GROUP_TBL3m}
};
#endif 
#if defined (BCM_SCORPION_SUPPORT)
_rep_regs_t sco_rep_regs[29] = {  {INVALIDf,       INVALIDf,    INVALIDm},
                                  {PORT1_1STPTRf,  PORT1_LASTf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT2_1STPTRf,  PORT2_LASTf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT3_1STPTRf,  PORT3_LASTf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT4_1STPTRf,  PORT4_LASTf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT5_1STPTRf,  PORT5_LASTf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT6_1STPTRf,  PORT6_LASTf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT7_1STPTRf,  PORT7_LASTf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT8_1STPTRf,  PORT8_LASTf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT9_1STPTRf,  PORT9_LASTf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT10_1STPTRf,  PORT10_LASTf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT11_1STPTRf,  PORT11_LASTf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT12_1STPTRf,  PORT12_LASTf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT13_1STPTRf,  PORT13_LASTf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT14_1STPTRf,  PORT14_LASTf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT15_1STPTRf,  PORT15_LASTf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT16_1STPTRf,  PORT16_LASTf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT17_1STPTRf,  PORT17_LASTf, MMU_IPMC_GROUP_TBL4m},
                                  {PORT18_1STPTRf,  PORT18_LASTf, MMU_IPMC_GROUP_TBL4m},
                                  {PORT19_1STPTRf,  PORT19_LASTf, MMU_IPMC_GROUP_TBL4m},
                                  {PORT20_1STPTRf,  PORT20_LASTf, MMU_IPMC_GROUP_TBL4m},
                                  {PORT21_1STPTRf,  PORT21_LASTf, MMU_IPMC_GROUP_TBL5m},
                                  {PORT22_1STPTRf,  PORT22_LASTf, MMU_IPMC_GROUP_TBL5m},
                                  {PORT23_1STPTRf,  PORT23_LASTf, MMU_IPMC_GROUP_TBL5m},
                                  {PORT24_1STPTRf,  PORT24_LASTf, MMU_IPMC_GROUP_TBL5m},
                                  {PORT25_1STPTRf,  PORT25_LASTf, MMU_IPMC_GROUP_TBL6m},
                                  {PORT26_1STPTRf,  PORT26_LASTf, MMU_IPMC_GROUP_TBL6m},
                                  {PORT27_1STPTRf,  PORT27_LASTf, MMU_IPMC_GROUP_TBL6m},
                                  {PORT28_1STPTRf,  PORT28_LASTf, MMU_IPMC_GROUP_TBL6m}
};
#endif 
#if defined(BCM_RAVEN_SUPPORT)
_rep_regs_t rvn_rep_regs[30] = {  {INVALIDf,       INVALIDf, INVALIDm},
                                  {PORT1_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT2_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT3_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT4_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT5_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT6_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT7_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT8_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT9_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT10_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT11_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT12_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT13_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT14_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT15_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT16_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT17_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT18_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT19_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT20_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT21_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT22_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT23_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT24_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT25_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT26_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT27_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT28_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT29_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m}
};
#endif /* BCM_RAVEN_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
_rep_regs_t tr_rep_regs[55] = { {INVALIDf,       INVALIDf,    INVALIDm},
                                {PORT1_1STPTRf,  PORT1_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT2_1STPTRf,  PORT2_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT3_1STPTRf,  PORT3_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT4_1STPTRf,  PORT4_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT5_1STPTRf,  PORT5_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT6_1STPTRf,  PORT6_LASTf, MMU_IPMC_GROUP_TBL0m},
                                {PORT7_1STPTRf,  PORT7_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT8_1STPTRf,  PORT8_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT9_1STPTRf,  PORT9_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT10_1STPTRf,  PORT10_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT11_1STPTRf,  PORT11_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT12_1STPTRf,  PORT12_LASTf, MMU_IPMC_GROUP_TBL1m},
                                {PORT13_1STPTRf,  PORT13_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT14_1STPTRf,  PORT14_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT15_1STPTRf,  PORT15_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT16_1STPTRf,  PORT16_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT17_1STPTRf,  PORT17_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT18_1STPTRf,  PORT18_LASTf, MMU_IPMC_GROUP_TBL2m},
                                {PORT19_1STPTRf,  PORT19_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT20_1STPTRf,  PORT20_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT21_1STPTRf,  PORT21_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT22_1STPTRf,  PORT22_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT23_1STPTRf,  PORT23_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT24_1STPTRf,  PORT24_LASTf, MMU_IPMC_GROUP_TBL3m},
                                {PORT25_1STPTRf,  PORT25_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT26_1STPTRf,  PORT26_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT27_1STPTRf,  PORT27_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT28_1STPTRf,  PORT28_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT29_1STPTRf,  PORT29_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT30_1STPTRf,  PORT30_LASTf, MMU_IPMC_GROUP_TBL4m},
                                {PORT31_1STPTRf,  PORT31_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT32_1STPTRf,  PORT32_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT33_1STPTRf,  PORT33_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT34_1STPTRf,  PORT34_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT35_1STPTRf,  PORT35_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT36_1STPTRf,  PORT36_LASTf, MMU_IPMC_GROUP_TBL5m},
                                {PORT37_1STPTRf,  PORT37_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT38_1STPTRf,  PORT38_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT39_1STPTRf,  PORT39_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT40_1STPTRf,  PORT40_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT41_1STPTRf,  PORT41_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT42_1STPTRf,  PORT42_LASTf, MMU_IPMC_GROUP_TBL6m},
                                {PORT43_1STPTRf,  PORT43_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT44_1STPTRf,  PORT44_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT45_1STPTRf,  PORT45_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT46_1STPTRf,  PORT46_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT47_1STPTRf,  PORT47_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT48_1STPTRf,  PORT48_LASTf, MMU_IPMC_GROUP_TBL7m},
                                {PORT49_1STPTRf,  PORT49_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {PORT50_1STPTRf,  PORT50_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {PORT51_1STPTRf,  PORT51_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {PORT52_1STPTRf,  PORT52_LASTf, MMU_IPMC_GROUP_TBL8m},
                                {PORT53_1STPTRf,  PORT53_LASTf, MMU_IPMC_GROUP_TBL8m},
};
#endif

#if defined(BCM_ENDURO_SUPPORT)
_rep_regs_t en_rep_regs[] = { {INVALIDf,       INVALIDf,    INVALIDm},
                              {INVALIDf,       INVALIDf,    INVALIDm},
                              {PORT2_1STPTRf,  PORT2_LASTf, MMU_IPMC_GROUP_TBL0m},
                              {PORT3_1STPTRf,  PORT3_LASTf, MMU_IPMC_GROUP_TBL0m}, 
                              {PORT4_1STPTRf,  PORT4_LASTf, MMU_IPMC_GROUP_TBL0m}, 
                              {PORT5_1STPTRf,  PORT5_LASTf, MMU_IPMC_GROUP_TBL0m},
                              {PORT6_1STPTRf,  PORT6_LASTf, MMU_IPMC_GROUP_TBL0m},
                              {PORT7_1STPTRf,  PORT7_LASTf, MMU_IPMC_GROUP_TBL0m},
                              {PORT8_1STPTRf,  PORT8_LASTf, MMU_IPMC_GROUP_TBL0m},
                              {PORT9_1STPTRf,  PORT9_LASTf, MMU_IPMC_GROUP_TBL1m},
                              {PORT10_1STPTRf,  PORT10_LASTf, MMU_IPMC_GROUP_TBL1m},
                              {PORT11_1STPTRf,  PORT11_LASTf, MMU_IPMC_GROUP_TBL1m},
                              {PORT12_1STPTRf,  PORT12_LASTf, MMU_IPMC_GROUP_TBL1m},
                              {PORT13_1STPTRf,  PORT13_LASTf, MMU_IPMC_GROUP_TBL1m},
                              {PORT14_1STPTRf,  PORT14_LASTf, MMU_IPMC_GROUP_TBL1m},
                              {PORT15_1STPTRf,  PORT15_LASTf, MMU_IPMC_GROUP_TBL1m},
                              {PORT16_1STPTRf,  PORT16_LASTf, MMU_IPMC_GROUP_TBL2m},
                              {PORT17_1STPTRf,  PORT17_LASTf, MMU_IPMC_GROUP_TBL2m},
                              {PORT18_1STPTRf,  PORT18_LASTf, MMU_IPMC_GROUP_TBL2m},
                              {PORT19_1STPTRf,  PORT19_LASTf, MMU_IPMC_GROUP_TBL2m},
                              {PORT20_1STPTRf,  PORT20_LASTf, MMU_IPMC_GROUP_TBL2m},
                              {PORT21_1STPTRf,  PORT21_LASTf, MMU_IPMC_GROUP_TBL2m},
                              {PORT22_1STPTRf,  PORT22_LASTf, MMU_IPMC_GROUP_TBL2m},
                              {PORT23_1STPTRf,  PORT23_LASTf, MMU_IPMC_GROUP_TBL3m},                         
                              {PORT24_1STPTRf,  PORT24_LASTf, MMU_IPMC_GROUP_TBL3m},
                              {PORT25_1STPTRf,  PORT25_LASTf, MMU_IPMC_GROUP_TBL3m},
                              {PORT26_1STPTRf,  PORT26_LASTf, MMU_IPMC_GROUP_TBL3m},
                              {PORT27_1STPTRf,  PORT27_LASTf, MMU_IPMC_GROUP_TBL3m},
                              {PORT28_1STPTRf,  PORT28_LASTf, MMU_IPMC_GROUP_TBL3m},
                              {PORT29_1STPTRf,  PORT29_LASTf, MMU_IPMC_GROUP_TBL3m},
};
#endif
#if defined(BCM_HURRICANE_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
_rep_regs_t hu_rep_regs[30] = {  {INVALIDf,       INVALIDf, INVALIDm},
                                  {INVALIDf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT2_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT3_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT4_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT5_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT6_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT7_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT8_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT9_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT10_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT11_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT12_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT13_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL0m},
                                  {PORT14_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT15_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT16_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT17_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT18_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT19_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT20_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT21_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT22_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT23_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT24_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT25_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL1m},
                                  {PORT26_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT27_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT28_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL4m},
                                  {PORT29_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL5m}
};
#endif /* BCM_HURRICANE_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
_rep_regs_t gh_rep_regs[30] = {  {INVALIDf,     INVALIDf, INVALIDm},
                                  {INVALIDf,    INVALIDf, INVALIDm},
                                  {PORT2_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT3_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT4_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL4m},
                                  {PORT5_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL5m},
                                  {PORT6_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL6m},
                                  {PORT7_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL7m},
                                  {PORT8_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL8m},
                                  {PORT9_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL9m},
                                  {PORT10_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL10m},
                                  {PORT11_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL11m},
                                  {PORT12_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL12m},
                                  {PORT13_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL13m},
                                  {PORT14_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL14m},
                                  {PORT15_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL15m},
                                  {PORT16_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL16m},
                                  {PORT17_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL17m},
                                  {PORT18_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL18m},
                                  {PORT19_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL19m},
                                  {PORT20_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL20m},
                                  {PORT21_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL21m},
                                  {PORT22_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL22m},
                                  {PORT23_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL23m},
                                  {PORT24_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL24m},
                                  {PORT25_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL25m},
                                  {PORT26_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL26m},
                                  {PORT27_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL27m},
                                  {PORT28_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL28m},
                                  {PORT29_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL29m}
};
#endif /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
_rep_regs_t hr3_rep_regs[32] = {  {INVALIDf,     INVALIDf, INVALIDm},
                                  {INVALIDf,    INVALIDf, INVALIDm},
                                  {PORT2_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT3_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT4_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL4m},
                                  {PORT5_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL5m},
                                  {PORT6_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL6m},
                                  {PORT7_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL7m},
                                  {PORT8_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL8m},
                                  {PORT9_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL9m},
                                  {PORT10_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL10m},
                                  {PORT11_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL11m},
                                  {PORT12_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL12m},
                                  {PORT13_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL13m},
                                  {PORT14_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL14m},
                                  {PORT15_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL15m},
                                  {PORT16_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL16m},
                                  {PORT17_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL17m},
                                  {PORT18_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL18m},
                                  {PORT19_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL19m},
                                  {PORT20_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL20m},
                                  {PORT21_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL21m},
                                  {PORT22_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL22m},
                                  {PORT23_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL23m},
                                  {PORT24_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL24m},
                                  {PORT25_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL25m},
                                  {PORT26_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL26m},
                                  {PORT27_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL27m},
                                  {PORT28_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL28m},
                                  {PORT29_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL29m},
                                  {PORT30_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL30m},
                                  {PORT31_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL31m}
};
#endif /* BCM_HURRICANE3_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
_rep_regs_t gh2_rep_regs[66] = {  {INVALIDf,     INVALIDf, INVALIDm},
                                  {INVALIDf,    INVALIDf, INVALIDm},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL2m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL3m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL4m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL5m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL6m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL7m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL8m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL9m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL10m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL11m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL12m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL13m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL14m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL15m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL16m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL17m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL18m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL19m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL20m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL21m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL22m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL23m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL24m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL25m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL26m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL27m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL28m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL29m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL30m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL31m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL32m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL33m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL34m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL35m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL36m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL37m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL38m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL39m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL40m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL41m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL42m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL43m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL44m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL45m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL46m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL47m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL48m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL49m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL50m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL51m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL52m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL53m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL54m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL55m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL56m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL57m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL58m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL59m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL60m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL61m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL62m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL63m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL64m},
                                  {PORT_1STPTRf,  INVALIDf, MMU_IPMC_GROUP_TBL65m}
};
#endif /* BCM_GREYHOUND2_SUPPORT */
STATIC int
_fb_ipmc_vlan_ptr(int unit,int ipmc_id,
                  bcm_port_t port, int *vptr, int last, int set)
{
    soc_field_t  port_ptr = INVALIDf;
    soc_field_t  last_ptr = INVALIDf;
    soc_mem_t    mem = INVALIDm;
    mmu_ipmc_group_tbl0_entry_t gentry;
    int rv;
    _rep_regs_t     *rep_arr_ptr;
#if defined(BCM_GREYHOUND2_SUPPORT)
    int mmu_port, phy_port = 0;
    const soc_field_t port_ptrf[8] = {
        PORT_1STPTR_0f, PORT_1STPTR_1f, PORT_1STPTR_2f, PORT_1STPTR_3f,
        PORT_1STPTR_4f, PORT_1STPTR_5f, PORT_1STPTR_6f, PORT_1STPTR_7f};
#endif

    rep_arr_ptr = NULL;
    if (SOC_IS_FB_FX_HX(unit)) {
        rep_arr_ptr = fb_rep_regs;
    }
#if defined(BCM_HB_GW_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        rep_arr_ptr = hbgw_rep_regs;
    }
#endif
#if defined(BCM_RAVEN_SUPPORT)
    if (SOC_IS_RAVEN(unit)) {
        rep_arr_ptr = rvn_rep_regs;
    }
#endif /* BCM_RAVEN_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
#if defined(BCM_HURRICANE_SUPPORT)
    if (SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) {
        rep_arr_ptr = hu_rep_regs;
    } else 
#endif /* BCM_HURRICANE_SUPPORT */
    if (SOC_IS_TR_VL(unit)) {
        rep_arr_ptr = tr_rep_regs;
    }
#endif
#if defined (BCM_SCORPION_SUPPORT)
    if (SOC_IS_SC_CQ(unit)) {
        rep_arr_ptr = sco_rep_regs;
    }
#endif
#if defined(BCM_ENDURO_SUPPORT)
    if (SOC_IS_ENDURO(unit)) {
      rep_arr_ptr = en_rep_regs;
    }
#endif
#if defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_GREYHOUND(unit)) {
      rep_arr_ptr = gh_rep_regs;
    }
#endif
#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)) {
      rep_arr_ptr = hr3_rep_regs;
    }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
      rep_arr_ptr = gh2_rep_regs;
    }
#endif
    if (NULL == rep_arr_ptr) {
        return BCM_E_UNIT;
    }

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
    } else {
        mmu_port = port;
    }

#if defined(BCM_FIRELIGHT_SUPPORT)
    /* Simulation has no mmu port as index, using logical port instead */
    if (SOC_IS_FIRELIGHT(unit) && SAL_BOOT_BCMSIM) {
        mem = rep_arr_ptr[port].mem;
        port_ptr = rep_arr_ptr[port].port_ptr;
        last_ptr = rep_arr_ptr[port].last_ptr;
    } else
#endif
    {
        mem = rep_arr_ptr[mmu_port].mem;
        port_ptr = rep_arr_ptr[mmu_port].port_ptr;
        last_ptr = rep_arr_ptr[mmu_port].last_ptr;
    }
#else
    mem = rep_arr_ptr[port].mem;
    port_ptr = rep_arr_ptr[port].port_ptr;
    last_ptr = rep_arr_ptr[port].last_ptr;
#endif  /* BCM_GREYHOUND2_SUPPORT */

    if ((INVALIDm == mem) || (INVALIDf == port_ptr)){
        return BCM_E_PARAM;
    }
    soc_mem_lock(unit, mem);
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, port_ptrf[0])) {
        /* length of ipmc_id exceed 10 bits using different hw structure */
        if ((rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                               (ipmc_id >> 3), &gentry)) < 0) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        if ((rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                               ipmc_id, &gentry)) < 0) {
            soc_mem_unlock(unit, mem);
            return rv;
        }
    }
    if (set) {
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, port_ptrf[0])) {
            soc_mem_field32_set(unit, mem, &gentry, port_ptrf[(ipmc_id & 0x7)],
                                *vptr);
            if (soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
                soc_mem_field32_set(unit, mem, &gentry, last_ptr,last ? 1 : 0);
            }
            if ((rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, (ipmc_id >> 3),
                                    &gentry)) < 0) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        {
            soc_mem_field32_set(unit, mem, &gentry, port_ptr, *vptr);
            if (soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
                soc_mem_field32_set(unit, mem, &gentry, last_ptr,last ? 1 : 0);
            }
            if ((rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, ipmc_id,
                                    &gentry)) < 0) {
                soc_mem_unlock(unit, mem);
                return rv;
            }
        }
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "set IPMC_GROUPm(port=%d,group=%d,ptr=%d)\n"),
                     port, ipmc_id, *vptr));


    } else {
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, port_ptrf[0])) {
            *vptr = soc_mem_field32_get(unit, mem, &gentry,
                                        port_ptrf[(ipmc_id & 0x7)]);
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        {
            *vptr = soc_mem_field32_get(unit, mem, &gentry, port_ptr);
        }
    }
    soc_mem_unlock(unit, mem);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb_ipmc_repl_freeze
 * Purpose:
 *      Pause IPMC replication (actually all traffic) before reconfiging
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fb_ipmc_repl_freeze(int unit)
{
    int                 rv = BCM_E_NONE;
    uint32              rval;
    uint32              op_verify;
    soc_port_t          port;
    uint32              emb_set, emb_get, emc_set;

    /* MISCCONFIG.METERING_CLK_EN is set by chip init */

    /* This is a lock, so it must be released! */
    SOC_EGRESS_METERING_LOCK(unit);

    emc_set = 0;
    soc_reg_field_set(unit, EGRMETERINGCONFIGr, &emc_set, REFRESHf, 0);
    soc_reg_field_set(unit, EGRMETERINGCONFIGr, &emc_set, THD_SELf, 1);

    emb_set = 0;
    soc_reg_field_set(unit, EGRMETERINGBUCKETr, &emb_set, BUCKETf, 0xffffff);

    PBMP_E_ITER(unit, port) {
        if ((rv = READ_EGRMETERINGCONFIGr(unit, port, &rval)) < 0) {
            break;
        }
        IPMC_REPL_PORT_INFO(unit, port)->emc_val = rval;
        if ((rv = WRITE_EGRMETERINGCONFIGr(unit, port, emc_set)) < 0) {
            break;
        }

        /* Write to EGRMETERINGBUCKET register might be stomped by
         * traffic.  Verify that the IN_PROFILE_FLAG is successfully set. */
        do {
            if ((rv = WRITE_EGRMETERINGBUCKETr(unit, port, emb_set)) < 0) {
                break;
            }

            if (!SAL_BOOT_SIMULATION) {
                if ((rv = READ_EGRMETERINGBUCKETr(unit, port, &emb_get)) < 0) {
                    break;
                }

                op_verify = soc_reg_field_get(unit, EGRMETERINGBUCKETr,
                                              emb_get, IN_PROFILE_FLAGf);
            } else {
                op_verify = 1;
            }
        } while (op_verify != 1);

        if (rv < 0) {
            break;
        }
    }

    if (rv < 0) {
        SOC_EGRESS_METERING_UNLOCK(unit);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_fb_ipmc_repl_thaw
 * Purpose:
 *      Start IPMC replication (actually all traffic) after reconfiging
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fb_ipmc_repl_thaw(int unit)
{
    int                 rv = BCM_E_NONE;
    uint32              rval;
    soc_port_t          port;

    PBMP_E_ITER(unit, port) {
        rval = 0;
        /*
         * all fields in "rval" are already zero
         * soc_reg_field_set(unit, EGRMETERINGBUCKETr, &rval,
         *                   IN_PROFILE_FLAGf, 0);
         * soc_reg_field_set(unit, EGRMETERINGBUCKETr, &rval, BUCKETf, 0);
         */
        if ((rv = WRITE_EGRMETERINGBUCKETr(unit, port, rval)) < 0) {
            break;
        }

        rval = IPMC_REPL_PORT_INFO(unit, port)->emc_val;
        if ((rv = WRITE_EGRMETERINGCONFIGr(unit, port, rval)) < 0) {
            break;
        }
    }

    /* Must thaw here to release lock from freeze */
    SOC_EGRESS_METERING_UNLOCK(unit);
    
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_xgs3_ipmc_repl_reload
 * Purpose:
 *      Re-Initialize IPMC replication software to state consistent with
 *      hardware
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_xgs3_ipmc_repl_reload(int unit)
{
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    uint32 ms_bit, ls_bits[2];
    bcm_port_t port;
    int ipmc_id, alloc_size, intf_alloc_size;
    int vlan_ptr, next_vlan_ptr, first_vlan_ptr;
    SHR_BITDCL          *intf_vec = NULL;
    SHR_BITDCL          *intf_vec_egr = NULL;
    _bcm_repl_list_info_t *rli_start, *rli_current;
    uint8 flags;
    int rv;
    int i;
    int egr_id;
    bcm_l3_egress_t egr_nh;

    bcm_fb_ipmc_repl_detach(unit);
    /* TD3TBD */
    if (SOC_IS_TRIDENT3X(unit)) {
        return BCM_E_NONE;
    }

    /* Allocate struct for IPMC replication booking keeping */
    alloc_size = sizeof(_fb_repl_info_t);
    _fb_repl_info[unit] = sal_alloc(alloc_size, "IPMC repl info");
    if (_fb_repl_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_fb_repl_info[unit], 0, alloc_size);

    IPMC_REPL_GROUP_NUM(unit) = soc_mem_index_count(unit, L3_IPMCm);

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && !SOC_IS_HURRICANEX(unit) && 
            !SOC_IS_GREYHOUND(unit) && !SOC_IS_GREYHOUND2(unit)) {
        /* For VPLS, can replicate to 16k next-hop entries */
        IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    } else
#endif
    {
        IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_INTFm);
    }

    /*
     * soc_feature_l3mc_use_egress_next_hop use EGR_L3_NEXT_HOP as
     * replication interface instead of using EGR_L3_INTF.
     */
    if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
        IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    }

    IPMC_REPL_TOTAL(unit) = soc_mem_index_count(unit, MMU_IPMC_VLAN_TBLm);
    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_TOTAL(unit));
    _fb_repl_info[unit]->bitmap_entries_used =
        sal_alloc(alloc_size, "IPMC repl entries used");
    if (_fb_repl_info[unit]->bitmap_entries_used == NULL) {
        bcm_fb_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_fb_repl_info[unit]->bitmap_entries_used, 0, alloc_size);

    if (NULL == _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc) {
        _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc =
            sal_alloc(sizeof(int) * soc_mem_index_count(unit, EGR_L3_INTFm),
                      "l3_intf_to_next_hop_ipmc");
        if (NULL == _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc) {
            bcm_fb_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
        _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc[i] =
            REPL_NH_INDEX_UNALLOCATED;
    }

    /* Always reserve slot 0 (because 0 means NULL pointer) */
    IPMC_REPL_VE_USED_SET(unit, 0);

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        alloc_size = sizeof(_fb_repl_port_info_t);
        IPMC_REPL_PORT_INFO(unit, port) =
            sal_alloc(alloc_size, "IPMC repl port info");
        if (IPMC_REPL_PORT_INFO(unit, port) == NULL) {
            bcm_fb_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port), 0, alloc_size);

        alloc_size = sizeof(int32) * IPMC_REPL_GROUP_NUM(unit);
        IPMC_REPL_PORT_INFO(unit, port)->vlan_count =
            sal_alloc(alloc_size, "IPMC repl port vlan count");
        if (IPMC_REPL_PORT_INFO(unit, port)->vlan_count == NULL) {
            bcm_fb_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, port)->vlan_count,
                   0, alloc_size);
    }
    /*
     * HR3 and GH family use L3_EGR_NH table instead of L3_INTF when indexed by IPMC_VLAN.
     * And it uses soc_feature_l3mc_use_egress_next_hop to distinquish this
     */
    if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
        alloc_size = SHR_BITALLOCSIZE(soc_mem_index_count(unit,
                                                          EGR_L3_NEXT_HOPm));
    } else {
        alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit));
    }
    intf_vec = sal_alloc(alloc_size, "IPMC repl interface vector");
    if (intf_vec == NULL) {
        bcm_fb_ipmc_repl_detach(unit);
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
            rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port, &first_vlan_ptr, 0, 0);
            if (BCM_FAILURE(rv)) {
                sal_free(intf_vec);
                bcm_fb_ipmc_repl_detach(unit);
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
                    bcm_fb_ipmc_repl_detach(unit);
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
                    bcm_fb_ipmc_repl_detach(unit);
                    return rv;
                }
                soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                                 LSB_VLAN_BMf, ls_bits);
                ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(
                    unit, &vlan_entry, MSB_VLANf);
                intf_vec[2 * ms_bit + 0] = ls_bits[0];
                intf_vec[2 * ms_bit + 1] = ls_bits[1];
                IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) +=
                      _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
                IPMC_REPL_VE_USED_SET(unit, vlan_ptr);
                next_vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                  &vlan_entry, NEXTPTRf);
            }

            /* Create new list element */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC repl list info");
            if (rli_current == NULL) {
                sal_free(intf_vec);
                bcm_fb_ipmc_repl_detach(unit);
                return BCM_E_MEMORY;
            }
            sal_memset(rli_current, 0, alloc_size);
            rli_current->index = first_vlan_ptr;
            if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
                /*
                 * Previous intf_vef is derived from egr_l3_next_hop,
                 * now need to transfer l3_intf to l3_intf_egr.
                 */
                intf_vec_egr  =
                    sal_alloc(SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit)),
                              "IPMC repl interface vector, l3mc use egr nh");
                if (intf_vec_egr == NULL) {
                    sal_free(intf_vec);
                    sal_free(rli_current);
                    bcm_fb_ipmc_repl_detach(unit);
                    return BCM_E_MEMORY;
                }
                sal_memset(intf_vec_egr, 0,
                           SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit)));
                SHR_BIT_ITER(intf_vec, soc_mem_index_count(
                                         unit, EGR_L3_NEXT_HOPm), egr_id) {
                    rv = bcm_xgs3_nh_get(unit, egr_id, &egr_nh);
                        /* SW data not recover, will always return BCM_E_NOT_FOUND */
                    if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
                        sal_free(intf_vec_egr);
                        sal_free(intf_vec);
                        sal_free(rli_current);
                        bcm_fb_ipmc_repl_detach(unit);
                        return rv;
                    }
                    SHR_BITSET(intf_vec_egr, egr_nh.intf);
                }
                rli_current->hash =
                    _shr_crc32b(0, (uint8 *)intf_vec_egr,
                                IPMC_REPL_INTF_TOTAL(unit));
                sal_free(intf_vec_egr);
                intf_vec_egr = NULL;
            } else {
                rli_current->hash =
                    _shr_crc32b(0, (uint8 *)intf_vec,
                                IPMC_REPL_INTF_TOTAL(unit));
            }
            rli_current->next = rli_start;
            rli_current->list_size =
            IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id);
            IPMC_REPL_LIST_INFO(unit) = rli_current;
            rli_start = rli_current;
            (rli_current->refcount)++;
        }
    }

    /* Recover REPL list mode from HW cache */
    rv = _bcm_esw_ipmc_repl_wb_flags_get(unit,
                                         _BCM_IPMC_WB_REPL_LIST, &flags);
    if (flags) {
        SOC_IPMCREPLSHR_SET(unit, 1);
    }

    sal_free(intf_vec);

    /* Init the nexthop to l3 intf sw structure */
    if (NULL == _fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc) {
        _fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc =
            sal_alloc(sizeof(int) * soc_mem_index_count(unit, EGR_L3_NEXT_HOPm),
                      "next_hop_to_l3_intf_ipmc");

        if (NULL == _fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc) {
            bcm_fb_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_NEXT_HOPm); i++) {
        _fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc[i] =
            REPL_L3_INTF_UNALLOCATED;
    }

    if (NULL == _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc) {
        _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc =
            sal_alloc(sizeof(int) * soc_mem_index_count(unit, EGR_L3_INTFm),
                      "l3_intf_to_next_hop_ipmc");

        if (NULL == _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc) {
            bcm_fb_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
        _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc[i] =
            REPL_NH_INDEX_UNALLOCATED;
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_FIREBOLT_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT)
/*
 * Function:
 *      _bcm_xgs3_ipmc_l3entry_list_add
 * Purpose:
 *      Add a L3 entry to IPMC group's linked list of L3 entries
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ipmc_index - (IN) IPMC group ID.
 *      _bcm_l3_cfg_t - (IN) l3 config structure
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_xgs3_ipmc_l3entry_list_add(int unit, int ipmc_index,
                                _bcm_l3_cfg_t *l3cfg)
{
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry;
   
    ipmc_l3entry = sal_alloc(sizeof(_bcm_esw_ipmc_l3entry_t), "IPMC L3 entry");
    if (ipmc_l3entry == NULL) {
        return BCM_E_MEMORY;
    }
    ipmc_l3entry->ip6 = (l3cfg->l3c_flags & BCM_L3_IP6) ? 1 : 0;
    ipmc_l3entry->l3info.flags = l3cfg->l3c_flags;
    ipmc_l3entry->l3info.vrf = l3cfg->l3c_vrf;
    ipmc_l3entry->l3info.ip_addr = l3cfg->l3c_ipmc_group;
    ipmc_l3entry->l3info.src_ip_addr = l3cfg->l3c_src_ip_addr;
    sal_memcpy(ipmc_l3entry->l3info.ip6, l3cfg->l3c_ip6, BCM_IP6_ADDRLEN);
    sal_memcpy(ipmc_l3entry->l3info.sip6, l3cfg->l3c_sip6, BCM_IP6_ADDRLEN);
    ipmc_l3entry->l3info.vid = l3cfg->l3c_vid;
    ipmc_l3entry->l3info.prio = l3cfg->l3c_prio;
    ipmc_l3entry->l3info.ipmc_ptr = l3cfg->l3c_ipmc_ptr;
    ipmc_l3entry->l3info.lookup_class = l3cfg->l3c_lookup_class;
    ipmc_l3entry->next = IPMC_GROUP_INFO(unit, ipmc_index)->l3entry_list;
    IPMC_GROUP_INFO(unit, ipmc_index)->l3entry_list = ipmc_l3entry;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_ipmc_l3entry_list_del
 * Purpose:
 *      Delete a L3 entry from IPMC group's linked list of L3 entries
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ipmc_index - (IN) IPMC group ID.
 *      _bcm_l3_cfg_t - (IN) l3 config structure
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_ipmc_l3entry_list_del(int unit, int ipmc_index,
                                _bcm_l3_cfg_t *l3cfg)
{
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry;
    _bcm_esw_ipmc_l3entry_t *prev_ipmc_l3entry = NULL;
   
    ipmc_l3entry = IPMC_GROUP_INFO(unit, ipmc_index)->l3entry_list;
    while (ipmc_l3entry != NULL) {
        if ((ipmc_l3entry->l3info.ip_addr == l3cfg->l3c_ip_addr) &&
            (ipmc_l3entry->l3info.src_ip_addr == l3cfg->l3c_src_ip_addr) && 
            (!sal_memcmp(ipmc_l3entry->l3info.ip6, l3cfg->l3c_ip6, BCM_IP6_ADDRLEN)) &&
            (!sal_memcmp(ipmc_l3entry->l3info.sip6, l3cfg->l3c_sip6, BCM_IP6_ADDRLEN)) &&
            (ipmc_l3entry->l3info.vid == l3cfg->l3c_vid) &&
            (ipmc_l3entry->l3info.vrf == l3cfg->l3c_vrf)) {
           if (ipmc_l3entry == IPMC_GROUP_INFO(unit, ipmc_index)->l3entry_list) {
               IPMC_GROUP_INFO(unit, ipmc_index)->l3entry_list = ipmc_l3entry->next;
           } else {
               /* 
                * In the following line of code, Coverity thinks the
                * prev_ipmc_l3entry pointer may still be NULL when 
                * dereferenced. This situation will never occur because 
                * if ipmc_l3entry is not pointing to the head of the 
                * linked list, prev_ipmc_l3entry would not be NULL.
                */
               /* coverity[var_deref_op : FALSE] */
               prev_ipmc_l3entry->next = ipmc_l3entry->next;
           }
           sal_free(ipmc_l3entry);
           break;
        }
        prev_ipmc_l3entry = ipmc_l3entry;
        ipmc_l3entry = ipmc_l3entry->next;
    }

    if (ipmc_l3entry == NULL) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs3_ipmc_l3entry_list_update(int unit, int ipmc_index,
                                   _bcm_l3_cfg_t *l3cfg)
{
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry;
   
    ipmc_l3entry = IPMC_GROUP_INFO(unit, ipmc_index)->l3entry_list;
    while (ipmc_l3entry != NULL) {
        if ((ipmc_l3entry->l3info.ip_addr == l3cfg->l3c_ip_addr) &&
            (ipmc_l3entry->l3info.src_ip_addr == l3cfg->l3c_src_ip_addr) && 
            (!sal_memcmp(ipmc_l3entry->l3info.ip6, l3cfg->l3c_ip6, BCM_IP6_ADDRLEN)) &&
            (!sal_memcmp(ipmc_l3entry->l3info.sip6, l3cfg->l3c_sip6, BCM_IP6_ADDRLEN)) &&
            (ipmc_l3entry->l3info.vid == l3cfg->l3c_vid)) {
            ipmc_l3entry->l3info.flags = l3cfg->l3c_flags;
            ipmc_l3entry->l3info.vrf = l3cfg->l3c_vrf;
            ipmc_l3entry->l3info.prio = l3cfg->l3c_prio;
            ipmc_l3entry->l3info.ipmc_ptr = l3cfg->l3c_ipmc_ptr;
            ipmc_l3entry->l3info.lookup_class = l3cfg->l3c_lookup_class;
            break;
        }
        ipmc_l3entry = ipmc_l3entry->next;
    }

    if (ipmc_l3entry == NULL) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_ipmc_l3entry_list_size_get
 * Purpose:
 *      Get the number of L3 entries in IPMC group's linked list.
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ipmc_index - (IN) IPMC group ID.
 *      size       - (OUT) Number of L3 entries.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_ipmc_l3entry_list_size_get(int unit, int ipmc_index,
        int *size)
{
    if (IPMC_USED_ISSET(unit, ipmc_index)) {
        *size = IPMC_GROUP_INFO(unit, ipmc_index)->ref_count - 1;
    } else {
        *size = 0;
    }

    return BCM_E_NONE;
}

#endif /* BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *      bcm_xgs3_ipmc_create
 * Purpose:
 *      Create an ipmc entry.
 */

int
bcm_xgs3_ipmc_create(int unit, int *ipmc_id)
{
    int i;

    IPMC_LOCK(unit);
    for (i = 0; i < IPMC_GROUP_NUM(unit); i++) {
        if (!IPMC_USED_ISSET(unit, i)) {
            IPMC_USED_SET(unit, i);
            *ipmc_id = i;
            IPMC_UNLOCK(unit);
            return BCM_E_NONE;
        }
    }
    IPMC_UNLOCK(unit);

    return (BCM_E_FULL);
}

/*
 * Function:
 *      bcm_xgs3_ipmc_id_alloc
 * Purpose:
 *      allocate the given ipmc entry.
 */

int
bcm_xgs3_ipmc_id_alloc(int unit, int ipmc_id)
{
    IPMC_LOCK(unit);
    IPMC_USED_SET(unit, ipmc_id);
    IPMC_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_xgs3_ipmc_id_free
 * Purpose:
 *      Delete an ipmc entry.
 */

int
bcm_xgs3_ipmc_id_free(int unit, int ipmc_id)
{
    IPMC_INIT(unit);
    IPMC_ID(unit, ipmc_id);

    IPMC_LOCK(unit);
    if (IPMC_USED_ISSET(unit, ipmc_id)) {
        IPMC_USED_CLR(unit, ipmc_id);
    }
    IPMC_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_xgs3_ipmc_id_is_set
 * Purpose:
 *      Verify an ipmc entry.
 */

int
bcm_xgs3_ipmc_id_is_set(int unit, int ipmc_id, int *is_set)
{
    IPMC_INIT(unit);
    IPMC_ID(unit, ipmc_id);

    IPMC_LOCK(unit);
    *is_set = (0 != IPMC_USED_ISSET(unit, ipmc_id));
    IPMC_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _xgs3_ipmc_info_get
 * Purpose:
 *      Fill information to bcm_ipmc_addr_t struct.
 */

STATIC int
_xgs3_ipmc_info_get(int unit, int hw_index, int index, int er_index_get,
                    bcm_ipmc_addr_t *ipmc, ipmc_entry_t *entry, 
                    uint8 do_l3_lkup, 
                    _bcm_esw_ipmc_l3entry_t *use_ipmc_l3entry)
{
    uint32              mod, port_tgid, is_trunk;
    uint32              chk_val;
    int                 no_src_check = FALSE;
#if defined(BCM_FIREBOLT_SUPPORT)
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry = NULL;
#endif

    ipmc->v = soc_L3_IPMCm_field32_get(unit, entry, VALIDf);

    chk_val = SOC_IS_RAVEN(unit) ? 0x7f : 0x3f;
    if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
        is_trunk = soc_L3_IPMCm_field32_get(unit, entry, Tf);
        mod = soc_L3_IPMCm_field32_get(unit, entry, MODULE_IDf);
        port_tgid = soc_L3_IPMCm_field32_get(unit, entry, PORT_NUMf);
        if (is_trunk) {
            if ((port_tgid == chk_val) && ((int)mod == SOC_MODID_MAX(unit))) {
                no_src_check = TRUE;
            } else {
                mod = 0;
                port_tgid = soc_L3_IPMCm_field32_get(unit, entry, TGIDf);
            }
        }
    } else {
        mod = soc_L3_IPMCm_field32_get(unit, entry, MODULE_IDf);
        port_tgid = soc_L3_IPMCm_field32_get(unit, entry, PORT_TGIDf);
        is_trunk = port_tgid & BCM_TGID_TRUNK_INDICATOR(unit);
        if ((port_tgid == chk_val) && ((int)mod == SOC_MODID_MAX(unit))) {
            no_src_check = TRUE;
        } else if (is_trunk) {
            port_tgid = BCM_MODIDf_TGIDf_TO_TRUNK(unit, mod, port_tgid);
        }
    }
    if (no_src_check) {
        ipmc->ts = 0;
        ipmc->mod_id = -1;
        ipmc->port_tgid = -1;
        ipmc->flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
    } else if (is_trunk) {
        ipmc->ts = 1;
        ipmc->mod_id = 0;
        ipmc->port_tgid = port_tgid;
    } else {
        bcm_module_t    mod_in, mod_out;
        bcm_port_t      port_in, port_out;

        mod_in = mod;
        port_in = port_tgid;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod_in, port_in,
                                    &mod_out, &port_out));
        ipmc->ts = 0;
        ipmc->mod_id = mod_out;
        ipmc->port_tgid = port_out;
    }

    if (ipmc->v) {
        if (SOC_IS_FBX(unit) && do_l3_lkup) {
#if defined(BCM_FIREBOLT_SUPPORT)
            if (use_ipmc_l3entry) {
                /* use the passed in l3 info */
                ipmc_l3entry = use_ipmc_l3entry;
            } else {
                /* Note: this simply picks up the first l3 info */
                ipmc_l3entry = IPMC_GROUP_INFO(unit, index)->l3entry_list;
                if (NULL == ipmc_l3entry) {
                    /* No entries in Multicast host table */
                    return BCM_E_EMPTY;
                }
            }
            if (ipmc_l3entry->ip6) {
                ipmc->flags |= BCM_IPMC_IP6;
            } else {
                ipmc->flags &= ~BCM_IPMC_IP6;
            }
#endif /* BCM_FIREBOLT_SUPPORT */
        }

        if (SOC_IS_FBX(unit) && do_l3_lkup) {
#if defined(BCM_FIREBOLT_SUPPORT)
            _bcm_l3_cfg_t l3cfg;
            sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));

            l3cfg.l3c_flags = BCM_L3_IPMC;
            l3cfg.l3c_vrf = ipmc_l3entry->l3info.vrf;
            if (soc_feature(unit, soc_feature_l3_sgv)) {
                ipmc->vid = ipmc_l3entry->l3info.vid;
                l3cfg.l3c_vid = ipmc_l3entry->l3info.vid;
            }

            if (ipmc->flags & BCM_IPMC_HIT_CLEAR) {
                l3cfg.l3c_flags |= BCM_L3_HIT_CLEAR;
            }

            if (ipmc->flags & BCM_IPMC_IP6) {
                sal_memcpy(ipmc->s_ip6_addr, &ipmc_l3entry->l3info.sip6, BCM_IP6_ADDRLEN);
                sal_memcpy(ipmc->mc_ip6_addr, &ipmc_l3entry->l3info.ip6, BCM_IP6_ADDRLEN);
                sal_memcpy(l3cfg.l3c_sip6, &ipmc_l3entry->l3info.sip6, BCM_IP6_ADDRLEN);
                sal_memcpy(l3cfg.l3c_ip6, &ipmc_l3entry->l3info.ip6, BCM_IP6_ADDRLEN);
                l3cfg.l3c_flags |= BCM_L3_IP6;
                BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg));
            } else {
                ipmc->s_ip_addr = ipmc_l3entry->l3info.src_ip_addr;
                ipmc->mc_ip_addr = ipmc_l3entry->l3info.ipmc_group;
                l3cfg.l3c_src_ip_addr = ipmc_l3entry->l3info.src_ip_addr;
                l3cfg.l3c_ipmc_group = ipmc_l3entry->l3info.ipmc_group;
                BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg));
            }
            if (l3cfg.l3c_flags & BCM_L3_HIT) {
                ipmc->flags |= BCM_IPMC_HIT;
            }

            if (l3cfg.l3c_flags & BCM_L3_DST_DISCARD) {
                ipmc->flags |= BCM_IPMC_DISCARD;
            }

            if (ipmc_l3entry->l3info.flags & BCM_L3_RPE) {
                ipmc->cos = ipmc_l3entry->l3info.prio;
                ipmc->flags |= BCM_IPMC_SETPRI;
            } else {
                ipmc->cos = -1;
                ipmc->flags &= ~BCM_IPMC_SETPRI;
            }
            ipmc->group = index;
            ipmc->lookup_class = ipmc_l3entry->l3info.lookup_class;
            ipmc->vrf = ipmc_l3entry->l3info.vrf;
#endif /* BCM_FIREBOLT_SUPPORT */
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _xgs3_ipmc_read
 * Purpose:
 *      Read an ipmc entry.
 */

STATIC INLINE int
_xgs3_ipmc_read(int unit, int ipmc_id, ipmc_entry_t *ipmc_entry)
{
    int rv;

    rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, ipmc_id, ipmc_entry);

    return (rv);
}

/*
 * Function:
 *      _xgs3_ipmc_write
 * Purpose:
 *      Write an ipmc entry from bcm_ipmc_addr_t struct.
 */

STATIC int
_xgs3_ipmc_write(int unit, int ipmc_id, bcm_ipmc_addr_t *ipmc)
{
    int                 rv;
    ipmc_entry_t        entry;
    int                 mod, port_tgid, is_trunk, no_src_check = FALSE;

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, ipmc_id, &entry));

    soc_L3_IPMCm_field32_set(unit, &entry, VALIDf, ipmc->v);

    if ((ipmc->flags & BCM_IPMC_SOURCE_PORT_NOCHECK) ||
        (ipmc->port_tgid < 0)) {                        /* no source port */
        no_src_check = TRUE;
        is_trunk = 0;
        mod = SOC_MODID_MAX(unit);
#ifdef BCM_RAVEN_SUPPORT
        if (SOC_IS_RAVEN(unit)) {
            port_tgid = 0x7f;
        } else
#endif
        {
            port_tgid = 0x3f;
        }
    } else if (ipmc->ts) {                              /* trunk source port */
        is_trunk = 1;
        mod = 0;
        port_tgid = ipmc->port_tgid;
    } else {                                            /* source port */
        bcm_module_t    mod_in, mod_out;
        bcm_port_t      port_in, port_out;

        mod_in = ipmc->mod_id;
        port_in = ipmc->port_tgid;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                    mod_in, port_in,
                                    &mod_out, &port_out));
        /* Check parameters, since above is an application callback */
        if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
            return BCM_E_PORT;
        }
        is_trunk = 0;
        mod = mod_out;
        port_tgid = port_out;
    }

    if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
        if (is_trunk) {
            soc_L3_IPMCm_field32_set(unit, &entry, Tf, 1);
            soc_L3_IPMCm_field32_set(unit, &entry, TGIDf, port_tgid);
        } else {
            soc_L3_IPMCm_field32_set(unit, &entry, MODULE_IDf, mod);
            soc_L3_IPMCm_field32_set(unit, &entry, PORT_NUMf, port_tgid);
            if (no_src_check) {
                soc_L3_IPMCm_field32_set(unit, &entry, Tf, 1);
            } else {
                soc_L3_IPMCm_field32_set(unit, &entry, Tf, 0);
            }
        }
    } else {
        if (is_trunk) {
            mod = BCM_TRUNK_TO_MODIDf(unit, port_tgid);
            port_tgid = BCM_TRUNK_TO_TGIDf(unit, port_tgid);
        }
        soc_L3_IPMCm_field32_set(unit, &entry, MODULE_IDf, mod);
        soc_L3_IPMCm_field32_set(unit, &entry, PORT_TGIDf, port_tgid);
    }

    rv = soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, ipmc_id, &entry);

    return (rv);
}

/*
 * Function:
 *      _xgs3_ipmc_enable
 * Purpose:
 *      Enable/disable IPMC support.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      enable - TRUE: enable IPMC support.
 *               FALSE: disable IPMC support.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_xgs3_ipmc_enable(int unit, int enable)
{
    int                 port, do_vlan;
    bcm_pbmp_t          port_pbmp;

    enable = enable ? 1 : 0;
    do_vlan = soc_property_get(unit, spn_IPMC_DO_VLAN, 1);

    BCM_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_ASSIGN(port_pbmp, PBMP_PORT_ALL(unit));
    BCM_PBMP_OR(port_pbmp, PBMP_CMIC(unit));

    PBMP_ITER(port_pbmp, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port,
                                      _bcmPortIpmcV4Enable, enable));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port,
                                      _bcmPortIpmcV6Enable, enable));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port,
                                      _bcmPortIpmcVlanKey,
                                      (enable && do_vlan) ? 1 : 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_init
 * Purpose:
 *      Initialize the IPMC module and enable IPMC support.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This function has to be called before any other IPMC functions.
 */

int
bcm_fb_er_ipmc_init(int unit)
{
    _bcm_esw_ipmc_t  *info = IPMC_INFO(unit);

    BCM_IF_ERROR_RETURN(bcm_fb_er_ipmc_detach(unit));
    BCM_IF_ERROR_RETURN(_xgs3_ipmc_enable(unit, TRUE));

    IPMC_GROUP_NUM(unit) = soc_mem_index_count(unit, L3_IPMCm);

#ifdef BCM_BRADLEY_SUPPORT
    if (SOC_IS_HBX(unit)) {
        int ipmc_base, ipmc_size;
        
        SOC_IF_ERROR_RETURN
            (soc_hbx_ipmc_size_get(unit, &ipmc_base, &ipmc_size));

        if (IPMC_GROUP_NUM(unit) > ipmc_size) {
            /* Reduce to fix allocated table space */
            IPMC_GROUP_NUM(unit) = ipmc_size;
        }
    }
#endif

    info->ipmc_count = 0;

    info->ipmc_group_info =
        sal_alloc(IPMC_GROUP_NUM(unit) * sizeof(_bcm_esw_ipmc_group_info_t),
                  "IPMC group info");
    if (info->ipmc_group_info == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(info->ipmc_group_info, 0, 
               IPMC_GROUP_NUM(unit) * sizeof(_bcm_esw_ipmc_group_info_t));

    info->ipmc_initialized = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_detach
 * Purpose:
 *      Detach the IPMC module.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_detach(int unit)
{
    _bcm_esw_ipmc_t *info = IPMC_INFO(unit);
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry; 
    int i, rv;

    if (info->ipmc_initialized) {
        /* Delete all IPMC entries. BCM_E_NOT_FOUND or BCM_E_FAIL may be
         * returned by bcm_fb_er_ipmc_delete_all since L3 module
         * clears L3 tables during initialization.
         */
        rv = bcm_fb_er_ipmc_delete_all(unit);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_FAIL)) {
            return rv;
        }

        BCM_IF_ERROR_RETURN(_xgs3_ipmc_enable(unit, FALSE));

        if (info->ipmc_group_info != NULL) {
            for (i = 0; i < IPMC_GROUP_NUM(unit); i++) {
                ipmc_l3entry = IPMC_GROUP_INFO(unit, i)->l3entry_list;
                while (ipmc_l3entry != NULL) {
                    IPMC_GROUP_INFO(unit, i)->l3entry_list = ipmc_l3entry->next;
                    sal_free(ipmc_l3entry);
                    ipmc_l3entry = IPMC_GROUP_INFO(unit, i)->l3entry_list;
                }
            }
            sal_free(info->ipmc_group_info);
            info->ipmc_group_info = NULL;
        }

        info->ipmc_initialized = FALSE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_get
 * Purpose:
 *      Get an IPMC entry by index.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      index - The index number.
 *      ipmc - (OUT) IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_get(int unit, int index, bcm_ipmc_addr_t *ipmc)
{
    ipmc_entry_t        ipmc_entry;
    int                 rv;
    int                 l3index = 0;

    IPMC_INIT(unit);
    IPMC_ID(unit, index);

    if (IPMC_USED_ISSET(unit, index)) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, index, &ipmc_entry));

        rv = _xgs3_ipmc_info_get(unit, l3index, index, TRUE, ipmc, &ipmc_entry, 
                                 1, NULL);
        ipmc->group = index;

        return rv;
    } 

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_fb_er_ipmc_find
 * Purpose:
 *      Look up an IPMC entry by sip, mcip and vid
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      ipmc - (IN) IPMC entry information.
 *      l3cfg - (OUT) The index number.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_fb_er_ipmc_find(int unit, bcm_ipmc_addr_t *ipmc, _bcm_l3_cfg_t *l3cfg)
{
    if (SOC_IS_FBX(unit)) {
        sal_memset(l3cfg, 0, sizeof(_bcm_l3_cfg_t));
        l3cfg->l3c_vid = ipmc->vid;
        l3cfg->l3c_flags = BCM_L3_IPMC;
        l3cfg->l3c_vrf = ipmc->vrf;
        
        if (ipmc->flags & BCM_IPMC_HIT_CLEAR) {
            l3cfg->l3c_flags |= BCM_L3_HIT_CLEAR;
        }

        if (ipmc->flags & BCM_IPMC_IP6) {
            sal_memcpy(l3cfg->l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(l3cfg->l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
            l3cfg->l3c_flags |= BCM_L3_IP6;
            BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l3_ip6_get(unit, l3cfg));
        } else {
            l3cfg->l3c_src_ip_addr = ipmc->s_ip_addr;
            l3cfg->l3c_ipmc_group = ipmc->mc_ip_addr;
            BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l3_ip4_get(unit, l3cfg));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_lookup
 * Purpose:
 *      Look up an IPMC entry by sip, mcip and vid
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      index - (OUT) The index number.
 *      ipmc - (IN, OUT) IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_lookup(int unit, int *index, bcm_ipmc_addr_t *ipmc)
{
    ipmc_entry_t  ipmc_entry;
    _bcm_l3_cfg_t l3cfg;
    int           l3_id, ipmc_id;

    IPMC_INIT(unit);

    BCM_IF_ERROR_RETURN(
        _bcm_fb_er_ipmc_find(unit, ipmc, &l3cfg));

    ipmc_id = SOC_IS_FBX(unit) ? l3cfg.l3c_ipmc_ptr : ipmc->group;
    BCM_IF_ERROR_RETURN
        (_xgs3_ipmc_read(unit, ipmc_id, &ipmc_entry));

    l3_id = SOC_IS_FBX(unit) ? l3cfg.l3c_hw_index : ipmc->group;
    BCM_IF_ERROR_RETURN
        (_xgs3_ipmc_info_get(unit, l3_id, ipmc_id, FALSE, ipmc, &ipmc_entry,
                             0, NULL));
    if (SOC_IS_FBX(unit) && ipmc->v) {
        ipmc->group = l3cfg.l3c_ipmc_ptr;
        ipmc->lookup_class = l3cfg.l3c_lookup_class;
        if (l3cfg.l3c_flags & BCM_L3_HIT) {
            ipmc->flags |= BCM_IPMC_HIT;
        }
        if (l3cfg.l3c_flags & BCM_L3_DST_DISCARD) {
            ipmc->flags |= BCM_IPMC_DISCARD;
        }
        if (l3cfg.l3c_flags & BCM_L3_RPE) {
            ipmc->cos =  l3cfg.l3c_prio;
            ipmc->flags |= BCM_IPMC_SETPRI;
        } else {
            ipmc->cos = -1;
            ipmc->flags &= ~BCM_IPMC_SETPRI;
        }
    }
    if (index != NULL) {
        *index = SOC_IS_FBX(unit) ? l3cfg.l3c_ipmc_ptr : ipmc->group;
    }

    return BCM_E_NONE;
}

#if defined(BCM_FIREBOLT_SUPPORT)

/*
 * Function:
 *      _bcm_fb_ipmc_add
 * Purpose:
 *      Add a new entry to the L3 table.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ipmc - (IN)IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_fb_ipmc_add(int unit, bcm_ipmc_addr_t *ipmc)
{
    _bcm_l3_cfg_t       l3cfg; /* L3 ipmc entry.           */
    int                 rv;    /* Operation return status. */

    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));

    l3cfg.l3c_vid = ipmc->vid;
    l3cfg.l3c_flags = BCM_L3_IPMC;
    l3cfg.l3c_vrf = ipmc->vrf;
    l3cfg.l3c_lookup_class = ipmc->lookup_class;
    if (ipmc->flags & BCM_IPMC_SETPRI) {
        l3cfg.l3c_flags |= BCM_L3_RPE;
        l3cfg.l3c_prio = ipmc->cos;
    }
    if (ipmc->flags & BCM_IPMC_IP6) {
        if (!BCM_IP6_MULTICAST(ipmc->mc_ip6_addr)) {
            return BCM_E_PARAM;
        }
        sal_memcpy(l3cfg.l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3cfg.l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
        l3cfg.l3c_flags |= BCM_L3_IP6;
    } else {
        if (!BCM_IP4_MULTICAST(ipmc->mc_ip_addr)) {
            return BCM_E_PARAM;
        }
        l3cfg.l3c_src_ip_addr = ipmc->s_ip_addr;
        l3cfg.l3c_ipmc_group = ipmc->mc_ip_addr;
    }

    l3cfg.l3c_ipmc_ptr = ipmc->group;
    l3cfg.l3c_flags |=  BCM_L3_HIT;

    if (soc_feature(unit, soc_feature_l3_sgv)) {
        l3cfg.l3c_vid = ipmc->vid;
    }

    if (ipmc->flags & BCM_IPMC_REPLACE) {
        l3cfg.l3c_flags |= BCM_L3_REPLACE;
    }

    if (ipmc->flags & BCM_IPMC_DISCARD) {
        l3cfg.l3c_flags |= BCM_L3_DST_DISCARD;;
    }

    /* Write mcast entry to hw. */
    l3cfg.l3c_hw_index = BCM_XGS3_L3_INVALID_INDEX;/* Add /No overrite. */

    if (ipmc->flags & BCM_IPMC_IP6) {
        rv = mbcm_driver[unit]->mbcm_l3_ip6_add(unit, &l3cfg);
    } else {
        rv = mbcm_driver[unit]->mbcm_l3_ip4_add(unit, &l3cfg);
    }

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_xgs3_ipmc_l3entry_list_add(unit, ipmc->group, 
                                             &l3cfg); 
    }
    
    return (rv);
}

/*
 * Function:
 *      _bcm_fb_ipmc_del
 * Purpose:
 *      Remove an  entry from the L3 table.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ipmc - (IN)IPMC entry information.
 *      modify_l3entry_list - (IN) Controls whether to modify IPMC group's
 *                                 L3 entry list.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_fb_ipmc_del(int unit, bcm_ipmc_addr_t *ipmc, int modify_l3entry_list)
{
    _bcm_l3_cfg_t       l3cfg; /* L3 ipmc entry.           */
    int                 rv;    /* Operation return status. */
    int                 ipmc_index = 0;

    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));

    l3cfg.l3c_vid = ipmc->vid;
    l3cfg.l3c_flags = BCM_L3_IPMC;
    l3cfg.l3c_vrf = ipmc->vrf;

    if (ipmc->flags & BCM_IPMC_IP6) {
        if (!BCM_IP6_MULTICAST(ipmc->mc_ip6_addr)) {
            return BCM_E_PARAM;
        }
        sal_memcpy(l3cfg.l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3cfg.l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
        l3cfg.l3c_flags |= BCM_L3_IP6;
    } else {
        if (!BCM_IP4_MULTICAST(ipmc->mc_ip_addr)) {
            return BCM_E_PARAM;
        }
        l3cfg.l3c_src_ip_addr = ipmc->s_ip_addr;
        l3cfg.l3c_ipmc_group = ipmc->mc_ip_addr;
    }

    if (soc_feature(unit, soc_feature_l3_sgv)) {
        l3cfg.l3c_vid = ipmc->vid;
    }

    if (ipmc->flags & BCM_IPMC_IP6) {
        rv = mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg);
        if (BCM_SUCCESS(rv)) {
            ipmc_index = l3cfg.l3c_ipmc_ptr;
            rv = mbcm_driver[unit]->mbcm_l3_ip6_delete(unit, &l3cfg);
        }
    } else {
        rv = mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg);
        if (BCM_SUCCESS(rv)) {
            ipmc_index = l3cfg.l3c_ipmc_ptr;
            rv = mbcm_driver[unit]->mbcm_l3_ip4_delete(unit, &l3cfg);
        }
    }

    if (BCM_SUCCESS(rv) && modify_l3entry_list) {
        rv = _bcm_xgs3_ipmc_l3entry_list_del(unit, ipmc_index, 
                                             &l3cfg); 
    }

    return (rv);
}
/*
 * Function:
 *      _bcm_fb_ipmc_replace
 * Purpose:
 *      Overwrite an entry in the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      ipmc - New IPMC entry information.
 *      old_l3cfg - Old IPMC configuration information.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fb_ipmc_replace(int unit, bcm_ipmc_addr_t *ipmc, _bcm_l3_cfg_t *old_l3cfg)
{
    _bcm_l3_cfg_t l3cfg;
    int old_ipmc_index;

    l3cfg = *old_l3cfg;
    old_ipmc_index = l3cfg.l3c_ipmc_ptr;
    
    if (!(ipmc->flags & BCM_IPMC_SETPRI)) {
        l3cfg.l3c_flags &= ~BCM_L3_RPE;
        l3cfg.l3c_prio = 0;
    } else {
        l3cfg.l3c_flags |= BCM_L3_RPE;
        l3cfg.l3c_prio = ipmc->cos;
    }

    if (!(ipmc->flags & BCM_IPMC_DISCARD)) {
        l3cfg.l3c_flags &= ~BCM_L3_DST_DISCARD;
    } else {
        l3cfg.l3c_flags |= BCM_L3_DST_DISCARD;
    }

    l3cfg.l3c_ipmc_ptr = ipmc->group;

    BCM_IF_ERROR_RETURN
        (bcm_xgs3_l3_replace(unit, &l3cfg));
    
    if (old_ipmc_index != ipmc->group) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_ipmc_l3entry_list_del(unit, old_ipmc_index,
                                             &l3cfg)); 
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_ipmc_l3entry_list_add(unit, ipmc->group,
                                             &l3cfg)); 
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_ipmc_l3entry_list_update(unit, old_ipmc_index,
                                             &l3cfg));
    }

    return BCM_E_NONE;
}
#endif /* BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *      _bcm_xgs3_ipmc_src_port_compare
 * Purpose:
 *      Compare the IPMC source port parameters.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      ipmc_index - IPMC index to be shared.
 *      ipmc - IPMC address entry info.
 *      match - (OUT) Match indication.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_ipmc_src_port_compare(int unit, int ipmc_index,
        bcm_ipmc_addr_t *ipmc, int *match)
{
    ipmc_entry_t entry;
    int t_f, tgid_f, mod_f, port_f, port_tgid_f;
    int no_src_check, is_trunk, tgid, mod, port;

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, ipmc_index, &entry));

    no_src_check = FALSE;
    is_trunk = 0;
    tgid = -1;
    mod = -1;
    port = -1;
    if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
        t_f = soc_L3_IPMCm_field32_get(unit, &entry, Tf);
        tgid_f = soc_L3_IPMCm_field32_get(unit, &entry, TGIDf);
        mod_f = soc_L3_IPMCm_field32_get(unit, &entry, MODULE_IDf);
        port_f = soc_L3_IPMCm_field32_get(unit, &entry, PORT_NUMf);

        if ((t_f == 1) && (mod_f == SOC_MODID_MAX(unit)) &&
                (port_f == (SOC_IS_RAVEN(unit) ? 0x7f : 0x3f))) {
            no_src_check = TRUE;
        } else if (t_f == 1) {
            is_trunk = 1;
            tgid = tgid_f;
        } else {
            mod = mod_f;
            port = port_f;
        }
    } else {
        mod_f = soc_L3_IPMCm_field32_get(unit, &entry, MODULE_IDf);
        port_tgid_f = soc_L3_IPMCm_field32_get(unit, &entry, PORT_TGIDf);

        if ((mod_f == SOC_MODID_MAX(unit)) &&
                (port_tgid_f == (SOC_IS_RAVEN(unit) ? 0x7f : 0x3f))) {
            no_src_check = TRUE;
        } else if (BCM_TGID_TRUNK_INDICATOR(unit) & port_tgid_f) {
            is_trunk = 1;
            tgid = BCM_MODIDf_TGIDf_TO_TRUNK(unit, mod_f, port_tgid_f);
        } else {
            mod = mod_f;
            port = port_tgid_f;
        }
    }

    *match = FALSE;
    if ((ipmc->flags & BCM_IPMC_SOURCE_PORT_NOCHECK) ||
        (ipmc->port_tgid < 0)) {                        /* no source port */
        if (no_src_check) {
            *match = TRUE;
        }
    } else if (ipmc->ts) {                              /* trunk source port */
        if (is_trunk && (tgid == ipmc->port_tgid)) {
            *match = TRUE;
        }
    } else {                                            /* source port */
        bcm_module_t    mod_in, mod_out;
        bcm_port_t      port_in, port_out;

        mod_in = ipmc->mod_id;
        port_in = ipmc->port_tgid;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                    mod_in, port_in,
                                    &mod_out, &port_out));
        /* Check parameters, since above is an application callback */
        if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
            return BCM_E_PORT;
        }

        if ((mod == mod_out) && (port == port_out)) {
            *match = TRUE;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_ipmc_src_port_consistency_check
 * Purpose:
 *      When multiple IPMC address entries share the same IPMC
 *      index, they must also have the same port parameter
 *      for the purpose of IPMC source port checking. This
 *      procedure makes sure this is the case.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      ipmc_index - IPMC index to be shared.
 *      ipmc - IPMC address entry info.
 *      already_used - Indicates if the given IPMC address entry is
 *                     already using the ipmc index to be shared.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_ipmc_src_port_consistency_check(int unit, int ipmc_index,
        bcm_ipmc_addr_t *ipmc, int already_used, int *need_write)
{
    int l3entry_list_size = 0;
    int match;

    *need_write = TRUE;

    if (SOC_IS_FBX(unit)) {
#if defined(BCM_FIREBOLT_SUPPORT)
        BCM_IF_ERROR_RETURN(_bcm_xgs3_ipmc_l3entry_list_size_get(unit,
                    ipmc_index, &l3entry_list_size));
#endif /* BCM_FIREBOLT_SUPPORT */
    } 
        
    if ((already_used && (l3entry_list_size > 1)) ||
            (!already_used && (l3entry_list_size > 0))) {
        /* If there are IPMC address entries other than the given IPMC address
         * entry that are pointing the given ipmc index, verify that the IPMC
         * source port check parameters are the same. If not, the ipmc index
         * cannot be shared.
         */ 
        BCM_IF_ERROR_RETURN(_bcm_xgs3_ipmc_src_port_compare(unit,
                    ipmc_index, ipmc, &match));
        if (!match) {
            return BCM_E_PARAM;
        } 

        *need_write = FALSE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_add
 * Purpose:
 *      Add a new entry to the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      ipmc - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_add(int unit, bcm_ipmc_addr_t *ipmc)
{
    _bcm_l3_cfg_t   l3cfg;
    int             old_ipmc_index;
    int new_entry, rv, need_write;

    IPMC_INIT(unit);
    IPMC_ID(unit, ipmc->group);

    if (!IPMC_USED_ISSET(unit, ipmc->group)) {
        return BCM_E_PARAM;
    }

    /* Check if IPMC entry already exists.
     * In case of BCM_IPMC_REPLACE flag being set,
     * if entry is already present, the entry will be replaced,
     * else new entry will be created. This is the expected behaviour
     */
    rv = _bcm_fb_er_ipmc_find(unit, ipmc, &l3cfg);
    old_ipmc_index = SOC_IS_FBX(unit) ? l3cfg.l3c_ipmc_ptr : ipmc->group;
    if (BCM_SUCCESS(rv)) { 
        if (!(ipmc->flags & BCM_IPMC_REPLACE)) {
           return (BCM_E_EXISTS);
        } else {
            new_entry = FALSE;
        }
    } else {
        /* Return if error occured. */
        if (rv != BCM_E_NOT_FOUND) {
            return (rv);
        }
        new_entry = TRUE;
    }

    if (new_entry) { 
        BCM_IF_ERROR_RETURN(_bcm_xgs3_ipmc_src_port_consistency_check(unit,
                    ipmc->group, ipmc, 0, &need_write));

        /* Increment the reference count of the given ipmc_index. */
        BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, ipmc->group));
    } else {
        if (ipmc->group != old_ipmc_index) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_ipmc_src_port_consistency_check(unit,
                                                           ipmc->group,
                                                           ipmc, 0, &need_write));
            /* Increment the reference count of the given ipmc_index. */
            BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_alloc(unit, ipmc->group));

            /* Decrement the reference count of the old ipmc_index. */
            BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_id_free(unit, old_ipmc_index));
            if (!IPMC_USED_ISSET(unit, old_ipmc_index)) {
                /* Reference count should not be zero yet. */
                return BCM_E_INTERNAL;
            }
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_ipmc_src_port_consistency_check(unit,
                                                           ipmc->group,
                                                           ipmc, 1, &need_write));
        }
    } 

    ipmc->v = (ipmc->flags & BCM_IPMC_ADD_DISABLED) ? 0 : 1;

    if (new_entry) {
        if (need_write) {
            /* Write L3_IPMC table entry. */
            rv = _xgs3_ipmc_write(unit, ipmc->group, ipmc);
            if (BCM_FAILURE(rv)) {
                bcm_xgs3_ipmc_id_free(unit, ipmc->group);
                return (rv);
            }
        }

        /* Add new L3 table entry */
        if (SOC_IS_FBX(unit)) {
#if defined(BCM_FIREBOLT_SUPPORT)
            rv = _bcm_fb_ipmc_add(unit, ipmc);
#endif /* BCM_FIREBOLT_SUPPORT */
        }
        if (BCM_FAILURE(rv)) {
            bcm_xgs3_ipmc_id_free(unit, ipmc->group);
            if (!IPMC_USED_ISSET(unit, ipmc->group)) {
                /* Reference count should not be zero yet. */
                return BCM_E_INTERNAL;
            }
            return (rv);
        }
    } else {
        if (need_write) {
            /* Write L3_IPMC table entry. */
            rv = _xgs3_ipmc_write(unit, ipmc->group, ipmc);
            if (BCM_FAILURE(rv)) {
                if (SOC_IS_FBX(unit)) {
#if defined(BCM_FIREBOLT_SUPPORT)
                    _bcm_fb_ipmc_del(unit, ipmc, TRUE);
#endif /* BCM_FIREBOLT_SUPPORT */
                }
                bcm_xgs3_ipmc_id_free(unit, ipmc->group);
                return (rv);
            }
        }

        /* Update existing IPMC entry */
        rv = _bcm_fb_ipmc_replace(unit, ipmc, &l3cfg);
        if (BCM_FAILURE(rv)) {
            if (SOC_IS_FBX(unit)) {
#if defined(BCM_FIREBOLT_SUPPORT)
                _bcm_fb_ipmc_del(unit, ipmc, TRUE);
#endif /* BCM_FIREBOLT_SUPPORT */
            }
            bcm_xgs3_ipmc_id_free(unit, ipmc->group);
            return rv;
        }
    } 

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_put
 * Purpose:
 *      Overwrite an entry in the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      index - Table index to overwrite.
 *      data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_put(int unit, int index, bcm_ipmc_addr_t *ipmc)
{
    IPMC_INIT(unit);
    IPMC_ID(unit, index); 

    BCM_IF_ERROR_RETURN(_xgs3_ipmc_write(unit, index, ipmc));

    if (SOC_IS_FBX(unit)) {
#if defined(BCM_FIREBOLT_SUPPORT)
        _bcm_l3_cfg_t   l3cfg;

        sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
        l3cfg.l3c_vid = ipmc->vid;
        l3cfg.l3c_flags = BCM_L3_IPMC;
        l3cfg.l3c_vrf = ipmc->vrf;
        l3cfg.l3c_lookup_class = ipmc->lookup_class;
        if (ipmc->flags & BCM_IPMC_IP6) {
            sal_memcpy(l3cfg.l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(l3cfg.l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
            l3cfg.l3c_flags |= BCM_L3_IP6;
            BCM_IF_ERROR_RETURN
                (mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg));
        } else {
            l3cfg.l3c_src_ip_addr = ipmc->s_ip_addr;
            l3cfg.l3c_ipmc_group = ipmc->mc_ip_addr;
            BCM_IF_ERROR_RETURN
                (mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg));
        }

        return _bcm_fb_ipmc_replace(unit, ipmc, &l3cfg);
#endif /* BCM_FIREBOLT_SUPPORT */
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb_er_ipmc_delete
 * Purpose:
 *      Delete an entry from the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      data - IPMC entry information.
 *      modify_l3entry_list - Control whether to modify IPMC group's linked
 *                            list of L3 entries.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If BCM_IPMC_KEEP_ENTRY is true, the entry valid bit is cleared
 *      but the entry is not deleted from the table.
 */

STATIC int
_bcm_fb_er_ipmc_delete(int unit, bcm_ipmc_addr_t *ipmc, int modify_l3entry_list)
{
    int                 ipmc_id;
    _bcm_l3_cfg_t       l3cfg;

    IPMC_INIT(unit);

    if (SOC_IS_FBX(unit)) {
#if defined(BCM_FIREBOLT_SUPPORT)
        sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
        l3cfg.l3c_vid = ipmc->vid;
        l3cfg.l3c_flags = BCM_L3_IPMC;
        l3cfg.l3c_vrf = ipmc->vrf;
        if (ipmc->flags & BCM_IPMC_IP6) {
            sal_memcpy(l3cfg.l3c_sip6, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(l3cfg.l3c_ip6, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
            l3cfg.l3c_flags |= BCM_L3_IP6;
            BCM_IF_ERROR_RETURN(
                mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg));
        } else {
            l3cfg.l3c_src_ip_addr = ipmc->s_ip_addr;
            l3cfg.l3c_ipmc_group = ipmc->mc_ip_addr;
            BCM_IF_ERROR_RETURN(
                mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg));
        }
#endif /* BCM_FIREBOLT_SUPPORT */
    }
    ipmc_id = SOC_IS_FBX(unit) ? l3cfg.l3c_ipmc_ptr : ipmc->group;

    if (!(ipmc->flags & BCM_IPMC_KEEP_ENTRY)) {

        if (SOC_IS_FBX(unit)) {
#if defined(BCM_FIREBOLT_SUPPORT)
            BCM_IF_ERROR_RETURN(_bcm_fb_ipmc_del(unit, ipmc, modify_l3entry_list));
#endif /* BCM_FIREBOLT_SUPPORT */
        }

        bcm_xgs3_ipmc_id_free(unit, ipmc_id);
        if (!IPMC_USED_ISSET(unit, ipmc_id)) {
            /* Reference count should not be zero yet. */
            return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_delete
 * Purpose:
 *      Delete an entry from the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If BCM_IPMC_KEEP_ENTRY is true, the entry valid bit is cleared
 *      but the entry is not deleted from the table.
 */

int
bcm_fb_er_ipmc_delete(int unit, bcm_ipmc_addr_t *ipmc)
{
    return _bcm_fb_er_ipmc_delete(unit, ipmc, TRUE);
}

/*
 * Function:
 *      bcm_fb_er_ipmc_delete_all
 * Purpose:
 *      Delete all entries from the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_delete_all(int unit)
{
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry; 
    int rv = BCM_E_NONE, i;
#if defined(BCM_FIREBOLT_SUPPORT)
    _bcm_l3_cfg_t l3cfg;
#endif /* BCM_FIREBOLT_SUPPORT */

    IPMC_INIT(unit);

    IPMC_LOCK(unit);
    for (i = 0; i < IPMC_GROUP_NUM(unit); i++) {
        if (IPMC_USED_ISSET(unit, i)) {
            ipmc_l3entry = IPMC_GROUP_INFO(unit, i)->l3entry_list;
            while (ipmc_l3entry != NULL) {
                if (SOC_IS_FBX(unit)) {
#if defined(BCM_FIREBOLT_SUPPORT)
                    sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
                    l3cfg.l3c_vid = ipmc_l3entry->l3info.vid;
                    l3cfg.l3c_flags = BCM_L3_IPMC;
                    l3cfg.l3c_vrf = ipmc_l3entry->l3info.vrf;
                    if (ipmc_l3entry->l3info.flags & BCM_L3_IP6) {
                        sal_memcpy(l3cfg.l3c_sip6, ipmc_l3entry->l3info.sip6, BCM_IP6_ADDRLEN);
                        sal_memcpy(l3cfg.l3c_ip6, ipmc_l3entry->l3info.ip6, BCM_IP6_ADDRLEN);
                        l3cfg.l3c_flags |= BCM_L3_IP6;
                    } else {
                        l3cfg.l3c_src_ip_addr = ipmc_l3entry->l3info.src_ip_addr;
                        l3cfg.l3c_ipmc_group = ipmc_l3entry->l3info.ip_addr;
                    }
                    rv = bcm_xgs3_l3_del(unit, &l3cfg);
                    if (BCM_FAILURE(rv)) {
                        IPMC_UNLOCK(unit);
                        return rv;
                    }
#endif /* BCM_FIREBOLT_SUPPORT */
                }
                IPMC_GROUP_INFO(unit, i)->l3entry_list = ipmc_l3entry->next;
                sal_free(ipmc_l3entry);
                ipmc_l3entry = IPMC_GROUP_INFO(unit, i)->l3entry_list;
            }

            /* For IPMC groups that have non-zero reference counts,
             * decrease reference count to 1, meaning the IPMC group
             * is only referenced by bcm_multicast_egress APIs.
             */
            IPMC_USED_ONE(unit, i);
        }
    }

    IPMC_UNLOCK(unit);

    return rv;
}


/*
 * Function:
 *      bcm_xgs3_ipmc_age
 * Purpose:
 *      Age out the ipmc entry by clearing the HIT bit when appropriate,
 *      the ipmc entry itself is removed if HIT bit is not set.
 * Parameters:
 *      unit       -  (IN) BCM device number.
 *      flags      -  (IN) The criteria used to age out ipmc table.
 *                         IPv6/IPv4
 *      age_cb     -  (IN) Call back routine.
 *      user_data  -  (IN) User provided cookie for callback.
 * Returns:
 *      BCM_E_XXX 
 */
int
bcm_xgs3_ipmc_age(int unit, uint32 flags, bcm_ipmc_traverse_cb age_cb, 
                 void *user_data)
{
    int idx;                   /* Ipmc table iteration index. */
    bcm_ipmc_addr_t entry;     /* Ipmc entry iterator.        */
    int rv = BCM_E_NONE;       /* Operation return status.    */
    ipmc_entry_t ipmc_entry;
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry;
    _bcm_esw_ipmc_l3entry_t *prev_ipmc_l3entry = NULL;
    int l3index = 0;

    IPMC_INIT(unit);
    IPMC_LOCK(unit);

    for (idx = 0; idx < IPMC_GROUP_NUM(unit); idx++) {
        if (IPMC_USED_ISSET(unit, idx)) {

            rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, idx, &ipmc_entry);
            if (BCM_FAILURE(rv)) {
                goto age_done;
            }

            ipmc_l3entry = IPMC_GROUP_INFO(unit, idx)->l3entry_list;
            while (ipmc_l3entry != NULL) {
                sal_memset(&entry, 0, sizeof(bcm_ipmc_addr_t));
                rv = _xgs3_ipmc_info_get(unit, l3index, idx, TRUE, &entry, 
                                         &ipmc_entry, 1, ipmc_l3entry);
                if (BCM_FAILURE(rv)) {
                    goto age_done;
                }

                /* Make sure update only ipv4 or ipv6 entries. */
                if ((flags & BCM_IPMC_IP6) != (entry.flags & BCM_IPMC_IP6)) {
                    prev_ipmc_l3entry = ipmc_l3entry;
                    ipmc_l3entry = ipmc_l3entry->next;
                    continue;
                }

                if (entry.flags & BCM_IPMC_HIT) {
                    if (SOC_IS_FBX(unit)) {
                        _bcm_l3_cfg_t l3cfg;
                        /* Clear hit bit on used entry (by doing a lookup !!) */
                        sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
                        l3cfg.l3c_flags |= BCM_L3_HIT_CLEAR;
                        l3cfg.l3c_vid = ipmc_l3entry->l3info.vid;
                        l3cfg.l3c_flags |= BCM_L3_IPMC;
                        l3cfg.l3c_vrf = ipmc_l3entry->l3info.vrf;
                        l3cfg.l3c_vid = ipmc_l3entry->l3info.vid;

                        if (ipmc_l3entry->ip6) {
                            sal_memcpy(l3cfg.l3c_sip6, &ipmc_l3entry->l3info.sip6, 
                                       BCM_IP6_ADDRLEN);
                            sal_memcpy(l3cfg.l3c_ip6, &ipmc_l3entry->l3info.ip6, 
                                       BCM_IP6_ADDRLEN);
                            l3cfg.l3c_flags |= BCM_L3_IP6;
                            rv = (mbcm_driver[unit]->mbcm_l3_ip6_get(unit, &l3cfg));
                        } else {
                            l3cfg.l3c_src_ip_addr = ipmc_l3entry->l3info.src_ip_addr;
                            l3cfg.l3c_ipmc_group = ipmc_l3entry->l3info.ipmc_group;
                            rv = (mbcm_driver[unit]->mbcm_l3_ip4_get(unit, &l3cfg));
                        }
                    } else {  
                        /* Clear hit bit on used entry. */
                        entry.flags |= BCM_IPMC_HIT_CLEAR;
                        rv = _xgs3_ipmc_info_get(unit, l3index, idx, TRUE, &entry, 
                                                 &ipmc_entry, 0, NULL);
                    }
                    if (BCM_FAILURE(rv)) {
                        goto age_done;
                    }
                    prev_ipmc_l3entry = ipmc_l3entry;
                    ipmc_l3entry = ipmc_l3entry->next;
                } else {
                    /* Delete IPMC L3 entry. Inhibit modification of IPMC group's
                     * l3entry_list by _bcm_fb_er_ipmc_delete.
                     */
                    rv = _bcm_fb_er_ipmc_delete(unit, &entry, FALSE);
                    if (BCM_FAILURE(rv)) {
                        goto age_done;
                    }

                    /* Delete from IPMC group's l3entry_list */
                    if (ipmc_l3entry == IPMC_GROUP_INFO(unit, idx)->l3entry_list) {
                        IPMC_GROUP_INFO(unit, idx)->l3entry_list = ipmc_l3entry->next;
                        sal_free(ipmc_l3entry);
                        ipmc_l3entry = IPMC_GROUP_INFO(unit, idx)->l3entry_list;
                    } else {
                        /* 
                         * In the following line of code, Coverity thinks the
                         * prev_ipmc_l3entry pointer may still be NULL when 
                         * dereferenced. This situation will never occur because 
                         * if ipmc_l3entry is not pointing to the head of the 
                         * linked list, prev_ipmc_l3entry would not be NULL.
                         */
                        /* coverity[var_deref_op : FALSE] */
                        prev_ipmc_l3entry->next = ipmc_l3entry->next;
                        sal_free(ipmc_l3entry);
                        ipmc_l3entry = prev_ipmc_l3entry->next;
                    }

                    /* Invoke user callback. */
                    if (NULL != age_cb) {
                        _BCM_MULTICAST_GROUP_SET(entry.group,
                                _BCM_MULTICAST_TYPE_L3, entry.group);
                        rv = (*age_cb)(unit, &entry, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                            goto age_done;
                        }
#endif
                    }
                }
            }
        }
    }

age_done:
    IPMC_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_xgs3_ipmc_traverse
 * Purpose:
 *      Go through all valid ipmc entries, and call the callback function
 *      at each entry
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      flags     - (IN) The criteria used to age out ipmc table.
 *      cb        - (IN) User supplied callback function.
 *      user_data - (IN) User supplied cookie used in parameter 
 *                       in callback function.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_xgs3_ipmc_traverse(int unit, uint32 flags, bcm_ipmc_traverse_cb cb,
                       void *user_data)
{
    int idx;                   /* Ipmc table iteration index. */
    bcm_ipmc_addr_t entry;     /* Ipmc entry iterator.        */
    int rv = BCM_E_NONE;       /* Operation return status.    */
    ipmc_entry_t ipmc_entry;
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry;
    int l3index = 0;

    IPMC_INIT(unit);
    IPMC_LOCK(unit);

    for (idx = 0; idx < IPMC_GROUP_NUM(unit); idx++) {
        if (IPMC_USED_ISSET(unit, idx)) {

            rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, idx, &ipmc_entry);
            if (BCM_FAILURE(rv)) {
                goto traverse_done;
            }

            ipmc_l3entry = IPMC_GROUP_INFO(unit, idx)->l3entry_list;
            while (ipmc_l3entry != NULL) {
                sal_memset(&entry, 0, sizeof(bcm_ipmc_addr_t));
                rv = _xgs3_ipmc_info_get(unit, l3index, idx, TRUE, &entry, 
                                         &ipmc_entry, 1, ipmc_l3entry);
                if (BCM_FAILURE(rv)) {
                    goto traverse_done;
                }

                if ((flags & BCM_IPMC_IP6) != (entry.flags & BCM_IPMC_IP6)) {
                    ipmc_l3entry = ipmc_l3entry->next;
                    continue;
                }

                /* Get the pointer to next ipmc_l3entry before callback,
                 * since callback may delete the current ipmc_l3entry.
                 */
                ipmc_l3entry = ipmc_l3entry->next;

                /* Invoke user callback. */
                _BCM_MULTICAST_GROUP_SET(entry.group,
                                _BCM_MULTICAST_TYPE_L3, entry.group);
                rv = (*cb)(unit, &entry, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
                if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                    goto traverse_done;
                }
#endif
            }
        }
    }

traverse_done:
    IPMC_UNLOCK(unit);
    return (rv);
}


/*
 * Function:
 *      bcm_fb_er_ipmc_enable
 * Purpose:
 *      Enable or disable IPMC chip functions.
 * Parameters:
 *      unit - Unit number
 *      enable - TRUE to enable; FALSE to disable
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_enable(int unit, int enable)
{
    IPMC_INIT(unit);

    return _xgs3_ipmc_enable(unit, enable);
}

/*
 * Function:
 *      _bcm_xgs3_ipmc_valid_as_hit_get
 * Purpose:
 *      Check if IPMC valid bit as hit bit mode is enabled.
 * Parameters:
 *      unit - Unit number
 *      valid_as_hit - (OUT) TRUE for hit bit; FALSE for valid bit
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_xgs3_ipmc_valid_as_hit_get(int unit, int *valid_as_hit)
{
    IPMC_INIT(unit);
    *valid_as_hit = IPMC_VALID_AS_HIT(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_src_port_check
 * Purpose:
 *      Enable or disable Source Port checking in IPMC lookups.
 * Parameters:
 *      unit - Unit number
 *      enable - TRUE to enable; FALSE to disable
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_src_port_check(int unit, int enable)
{
    IPMC_INIT(unit);

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_src_ip_search
 * Purpose:
 *      Enable or disable Source IP significance in IPMC lookups.
 * Parameters:
 *      unit - Unit number
 *      enable - TRUE to enable; FALSE to disable
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_src_ip_search(int unit, int enable)
{
    IPMC_INIT(unit);

    if (enable) {
        return BCM_E_NONE;  /* always on */
    } else {
        return BCM_E_FAIL;  /* cannot be disabled */
    }
}


/*
 * Function:
 *      bcm_fb_er_ipmc_egress_port_set
 * Purpose:
 *      Configure the IP Multicast egress properties
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port to config.
 *      mac  - MAC address.
 *      untag - 1: The IP multicast packet is transmitted as untagged packet.
 *              0: The IP multicast packet is transmitted as tagged packet
 *              with VLAN tag vid.
 *      vid  - VLAN ID.
 *      ttl  - 1 to disable the TTL decrement, 0 otherwise.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_egress_port_set(int unit, bcm_port_t port,
                               const bcm_mac_t mac, int untag,
                               bcm_vlan_t vid, int ttl)
{
    uint32      cfg2;

#if defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_RAPTOR(unit) || SOC_IS_HAWKEYE(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (!SOC_PBMP_PORT_VALID(port) || !IS_PORT(unit, port)) {
        return BCM_E_BADID;
    }

    cfg2 = 0;

    soc_reg_field_set(unit, EGR_IPMC_CFG2r, &cfg2,
                      UNTAGf, untag ? 1 : 0);
    soc_reg_field_set(unit, EGR_IPMC_CFG2r, &cfg2,
                      VIDf, vid);
    soc_reg_field_set(unit, EGR_IPMC_CFG2r, &cfg2,
                      DISABLE_SA_REPLACEf, 0);

    SOC_IF_ERROR_RETURN(WRITE_EGR_IPMC_CFG2r(unit, port, cfg2));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_er_ipmc_egress_port_get
 * Purpose:
 *      Return the IP Multicast egress properties
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - port to config.
 *      mac - (OUT) MAC address.
 *      untag - (OUT) 1: The IP multicast packet is transmitted as
 *                       untagged packet.
 *                    0: The IP multicast packet is transmitted as tagged
 *                       packet with VLAN tag vid.
 *      vid - (OUT) VLAN ID.
 *      ttl_thresh - (OUT) Drop IPMC packets if TTL <= ttl_thresh.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_er_ipmc_egress_port_get(int unit, bcm_port_t port, sal_mac_addr_t mac,
                            int *untag, bcm_vlan_t *vid, int *ttl_thresh)
{
    uint32              cfg2;

#if defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_RAPTOR(unit) || SOC_IS_HAWKEYE(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif

    if (!SOC_PBMP_PORT_VALID(port) || !IS_PORT(unit, port)) {
        return BCM_E_BADID;
    }


    SOC_IF_ERROR_RETURN(READ_EGR_IPMC_CFG2r(unit, port, &cfg2));

    *untag = soc_reg_field_get(unit, EGR_IPMC_CFG2r, cfg2, UNTAGf);
    *vid = soc_reg_field_get(unit, EGR_IPMC_CFG2r, cfg2, VIDf);
    *ttl_thresh = -1;

    return BCM_E_NONE;
}

#if defined(BCM_FIREBOLT_SUPPORT)
/*
 * Function:
 *      bcm_fb_ipmc_repl_init
 * Purpose:
 *      Initialize IPMC replication.
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_ipmc_repl_init(int unit)
{
    bcm_port_t          port;
    int                 alloc_size;
    int                 act_port;
#if defined(BCM_GREYHOUND2_SUPPORT)
    int                 mmu_port, phy_port = 0;
#endif
    int                 i;

    bcm_fb_ipmc_repl_detach(unit);

    /* Allocate struct for IPMC replication bookkeeping */
    alloc_size = sizeof(_fb_repl_info_t);
    _fb_repl_info[unit] = sal_alloc(alloc_size, "IPMC repl info");
    if (_fb_repl_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_fb_repl_info[unit], 0, alloc_size);
    _fb_repl_info[unit]->ipmc_size = soc_mem_index_count(unit, L3_IPMCm);

#ifdef BCM_BRADLEY_SUPPORT
    if (SOC_IS_HBX(unit)) {
        int ipmc_base, ipmc_size;
        
        SOC_IF_ERROR_RETURN
            (soc_hbx_ipmc_size_get(unit, &ipmc_base, &ipmc_size));

        if (_fb_repl_info[unit]->ipmc_size > ipmc_size) {
            /* Reduce to fix allocated table space */
            _fb_repl_info[unit]->ipmc_size = ipmc_size;
        }
    }
#endif

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit) && !SOC_IS_HURRICANEX(unit) && 
            !SOC_IS_GREYHOUND(unit) && !SOC_IS_GREYHOUND2(unit)) {
        /* For VPLS, can replicate to 16k next-hop entries */
        IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    } else
#endif
    {
        /* for GH case, eventhough the L3MC egress logic changed to goto 
         * EGR_L3_NEXT_HOPm before EGR_L3_INTFm, the maximum replication still 
         * be limited of EGR_L3_INTFm size.
         */
        IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_INTFm);
    }

    /*
     * soc_feature_l3mc_use_egress_next_hop use EGR_L3_NEXT_HOP as
     * replication interface instead of using EGR_L3_INTF.
     */
    if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
        IPMC_REPL_INTF_TOTAL(unit) = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    }

    IPMC_REPL_TOTAL(unit) = soc_mem_index_count(unit, MMU_IPMC_VLAN_TBLm);
    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_TOTAL(unit));
    _fb_repl_info[unit]->bitmap_entries_used =
        sal_alloc(alloc_size, "IPMC repl entries used");
    if (_fb_repl_info[unit]->bitmap_entries_used == NULL) {
        bcm_fb_ipmc_repl_detach(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(_fb_repl_info[unit]->bitmap_entries_used, 0, alloc_size);

    if (NULL == _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc) {
        _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc =
            sal_alloc(sizeof(int) * soc_mem_index_count(unit, EGR_L3_INTFm),
                      "l3_intf_to_next_hop_ipmc");
        if (NULL == _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc) {
            bcm_fb_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
        _fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc[i] =
            REPL_NH_INDEX_UNALLOCATED;
    }

    if (NULL == _fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc) {
        _fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc =
            sal_alloc(sizeof(int) * soc_mem_index_count(unit, EGR_L3_NEXT_HOPm),
                      "next_hop_to_l3_intf_ipmc");

        if (NULL == _fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc) {
            bcm_fb_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
    }
    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_NEXT_HOPm); i++) {
        _fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc[i] =
            REPL_L3_INTF_UNALLOCATED;
    }

    /* Always reserve slot 0 */
    IPMC_REPL_VE_USED_SET(unit, 0);

    PBMP_ITER(PBMP_PORT_ALL(unit), port) {
        act_port = port;
        alloc_size = sizeof(_fb_repl_port_info_t);
        IPMC_REPL_PORT_INFO(unit, act_port) =
            sal_alloc(alloc_size, "IPMC repl port info");
        if (IPMC_REPL_PORT_INFO(unit, act_port) == NULL) {
            bcm_fb_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, act_port), 0, alloc_size);
       
        alloc_size = sizeof(int32) * soc_mem_index_count(unit, L3_IPMCm);
        IPMC_REPL_PORT_INFO(unit, act_port)->vlan_count =
            sal_alloc(alloc_size, "IPMC repl port vlan count");
        if (IPMC_REPL_PORT_INFO(unit, act_port)->vlan_count == NULL) {
            bcm_fb_ipmc_repl_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(IPMC_REPL_PORT_INFO(unit, act_port)->vlan_count,
                   0, alloc_size);
    }

    if (!SAL_BOOT_QUICKTURN) {
#ifdef BCM_GREYHOUND_SUPPORT
        /* to clear all MMU_IPMC_GROUP_TBLm
         * 
         *  Note : Greyhound device changed these tables from system to 
         *          port based tables. 
         */
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)){
            _rep_regs_t *ipmc_group_tbl = gh_rep_regs;

#if defined(BCM_HURRICANE3_SUPPORT)
            if (SOC_IS_HURRICANE3(unit)) {
                ipmc_group_tbl = hr3_rep_regs;
            }
#endif  /* HURRICANE3 */
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                ipmc_group_tbl = gh2_rep_regs;
            }
#endif  /* HURRICANE3 */

            PBMP_ITER(PBMP_PORT_ALL(unit), port) {
#if defined(BCM_GREYHOUND2_SUPPORT)
                if (SOC_IS_GREYHOUND2(unit)) {
                    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
                    act_port = mmu_port;
                } else 
#endif
                {
                    act_port = port;
                }
                if (ipmc_group_tbl[act_port].mem == INVALIDm) {
                    continue;
                }

                SOC_IF_ERROR_RETURN(soc_mem_clear(unit, 
                        ipmc_group_tbl[act_port].mem, COPYNO_ALL, FALSE));
            }
        } else 
#endif  /* BCM_GREYHOUND_SUPPORT */
        {

        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL0m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL1m, COPYNO_ALL, FALSE));
        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL2m, COPYNO_ALL, FALSE));
        if (!SOC_IS_RAVEN(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL3m, COPYNO_ALL, FALSE));
        }
        if (!SOC_IS_HB_GW(unit) && !SOC_IS_RAVEN(unit) && !SOC_IS_ENDURO(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL4m, COPYNO_ALL, FALSE));
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL5m, COPYNO_ALL, FALSE));
			if(!SOC_IS_HURRICANEX(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL6m, COPYNO_ALL, FALSE));
			}
            if (!SOC_IS_SC_CQ(unit) && !SOC_IS_HURRICANEX(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL7m, COPYNO_ALL, 
                                   FALSE));
            }
        }
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_MEM_IS_VALID(unit, MMU_IPMC_GROUP_TBL8m)) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, MMU_IPMC_GROUP_TBL8m, COPYNO_ALL, FALSE));
        }
#endif

        SOC_IF_ERROR_RETURN
            (soc_mem_clear(unit, MMU_IPMC_VLAN_TBLm, COPYNO_ALL, FALSE));

        } /* if (SOC_IS_GREYHOUND) */
    }
    
    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_fb_ipmc_repl_detach
 * Purpose:
 *      Initialize IPMC replication.
 * Parameters:
 *      unit     - SOC unit #
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_fb_ipmc_repl_detach(int unit)
{
    bcm_port_t          port;
    _bcm_repl_list_info_t *rli_current, *rli_free;

    if (_fb_repl_info[unit] != NULL) {
        PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            if (_fb_repl_info[unit]->port_info[port] != NULL) {
                if (_fb_repl_info[unit]->
                    port_info[port]->vlan_count != NULL) {
                    sal_free(_fb_repl_info[unit]->
                             port_info[port]->vlan_count);
                }
                sal_free(_fb_repl_info[unit]->port_info[port]);
            }
        }

        if (_fb_repl_info[unit]->bitmap_entries_used != NULL) {
            sal_free(_fb_repl_info[unit]->bitmap_entries_used);
        }

        if (_fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc != NULL) {
            sal_free(_fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc);
        }

        if (_fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc != NULL) {
            sal_free(_fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc);
        }

        if (_fb_repl_info[unit]->repl_list_info != NULL) {
            rli_current = IPMC_REPL_LIST_INFO(unit);
            while (rli_current != NULL) {
                rli_free = rli_current;
                rli_current = rli_current->next;
                sal_free(rli_free);
            }
        }

        sal_free(_fb_repl_info[unit]);
        _fb_repl_info[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_fb_ipmc_repl_get
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
bcm_fb_ipmc_repl_get(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_vector_t vlan_vec)
{
    int                 rv = BCM_E_NONE;
    uint32              ms_bit;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    bcm_l3_intf_t       l3_intf;
    int                 vlan_ptr, last_vlan_ptr;
    uint32              vlan_count;
    int                 ls_pos;
    int                 rvth = BCM_E_NONE;

    IPMC_REPL_INIT(unit);

    IPMC_REPL_ID(unit, ipmc_id);

    REPL_PORT_CHECK(unit, port);

    BCM_VLAN_VEC_ZERO(vlan_vec);

    IPMC_REPL_LOCK(unit);
    if (IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) == 0) {
        IPMC_REPL_UNLOCK(unit);
        return rvth;
    }

    if (soc_feature(unit, soc_feature_ipmc_repl_freeze) && 
        ((rv = _bcm_fb_ipmc_repl_freeze(unit)) < 0)) {
        IPMC_REPL_UNLOCK(unit);
        return rv;
    }

    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto get_done;
    }
    last_vlan_ptr = -1;
    vlan_count = 0;
    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto get_done;
        }
        ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                    &vlan_entry, MSB_VLANf);
        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                         LSB_VLAN_BMf, ls_bits);
        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                      &vlan_entry, NEXTPTRf);

        /* Translate the HW L3 interfaces into the corresponding VLANs.
         * Note, this may result in some loss of information.
         */
        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                bcm_l3_intf_t_init(&l3_intf);
                l3_intf.l3a_intf_id =
                    (ms_bit << 6) + ls_pos;
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
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze)) {
        rvth = _bcm_fb_ipmc_repl_thaw(unit);
        rv = (rv < 0) ? rv : rvth;
    }
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

STATIC int
_fb_ipmc_repl_next_free_ptr(int unit)
{
    int                 ix, bit;
    SHR_BITDCL          not_ptrs;

    for (ix = 0; ix < _SHR_BITDCLSIZE(IPMC_REPL_TOTAL(unit)); ix++) {
        not_ptrs = ~_fb_repl_info[unit]->bitmap_entries_used[ix];
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
 *      bcm_fb_ipmc_repl_add
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
bcm_fb_ipmc_repl_add(int unit, int ipmc_id, bcm_port_t port,
                     bcm_vlan_t vlan)
{
    int                 rv = BCM_E_NONE;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int                 ms_bit, target_ms_bit, ls_bit;
    int                 vlan_ptr, last_vlan_ptr;
    pbmp_t pbmp, ubmp;
    bcm_l3_intf_t l3_intf;
    uint32              vlan_count;
    int                 rvth = BCM_E_NONE;

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

    if (SOC_IPMCREPLSHR_GET(unit) ||
            soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        int alloc_size, intf_max, if_count, rv = BCM_E_NONE;
        bcm_if_t *if_array = NULL;
        
        intf_max = IPMC_REPL_INTF_TOTAL(unit);
        alloc_size = intf_max * sizeof(bcm_if_t);
        if_array = sal_alloc(alloc_size, "IPMC repl interface array");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }

        IPMC_REPL_LOCK(unit);
        rv = bcm_fb_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                         if_array, &if_count);
        if (BCM_SUCCESS(rv)) {
            if (if_count < intf_max) {
                if_array[if_count++] = l3_intf.l3a_intf_id;
                rv = bcm_fb_ipmc_egress_intf_set(unit, ipmc_id, port,
                                                 if_count, if_array, 1, FALSE);
            } else {
                rv = BCM_E_EXISTS;
            }
        }

        IPMC_REPL_UNLOCK(unit);
        sal_free(if_array);
        return rv;
    }

    IPMC_REPL_LOCK(unit);
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze) && 
        ((rv = _bcm_fb_ipmc_repl_freeze(unit)) < 0)) {
        IPMC_REPL_UNLOCK(unit);
        return rv;
    }

    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto add_done;
    }

    target_ms_bit = (l3_intf.l3a_intf_id >> 6) & 0x3f;
    ls_bit = l3_intf.l3a_intf_id & 0x3f;

    last_vlan_ptr = -1;
    vlan_count = 0;
    ms_bit = -1;

    if (IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        /*
         * Add to existing chain
         */
        while (vlan_ptr != last_vlan_ptr) {
            if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                              vlan_ptr, &vlan_entry)) < 0) {
                goto add_done;
            }

            ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                       &vlan_entry, MSB_VLANf);

            if (ms_bit == target_ms_bit) {
                /* Add to existing entry */
                break;
            }

            last_vlan_ptr = vlan_ptr;
            vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                              &vlan_entry, NEXTPTRf);

            soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);
            vlan_count +=
                _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
            if (vlan_count >= IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
                break;
            }
        }
    }

    if (ms_bit != target_ms_bit) {
        /* New entry, find slot */
        vlan_ptr = _fb_ipmc_repl_next_free_ptr(unit);
        if (vlan_ptr < 0) {
            /* Out of LS entries */
            rv = BCM_E_RESOURCE;
            goto add_done;
        }

        if (last_vlan_ptr < 0) {
            /* Update group table */
            if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id,
                                          port, &vlan_ptr, 0, 1)) < 0) {
                goto add_done;
            }
        } else {
            /* Update last entry */
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                               NEXTPTRf, vlan_ptr);
            if ((rv = WRITE_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                          last_vlan_ptr, &vlan_entry)) < 0) {
                goto add_done;
            }
        }

        sal_memset(&vlan_entry, 0, sizeof(vlan_entry));
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                           MSB_VLANf, target_ms_bit);
        /* Point to self */
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                           NEXTPTRf, vlan_ptr);

        IPMC_REPL_VE_USED_SET(unit, vlan_ptr);
    }

    /* Insert replacement */
    soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                     LSB_VLAN_BMf, ls_bits);
    if (ls_bits[ls_bit / 32] & (1 << (ls_bit % 32))) {
        /* Already exists! */
        rv = BCM_E_EXISTS;
        goto add_done;
    }
    ls_bits[ls_bit / 32] |= 1 << (ls_bit % 32);
    soc_MMU_IPMC_VLAN_TBLm_field_set(unit, &vlan_entry,
                                     LSB_VLAN_BMf, ls_bits);


    if ((rv = WRITE_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                       vlan_ptr, &vlan_entry)) < 0) {
        goto add_done;
    }

    IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)++;

 add_done:
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze)) {
        rvth = _bcm_fb_ipmc_repl_thaw(unit);
        rv = (rv < 0) ? rv : rvth;
    }
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_fb_ipmc_repl_delete
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
bcm_fb_ipmc_repl_delete(int unit, int ipmc_id, bcm_port_t port,
                        bcm_vlan_t vlan)
{
    int                 rv = BCM_E_NONE;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry, prev_vlan_entry;
    int                 ms_bit, target_ms_bit, ls_bit;
    int                 vlan_ptr, last_vlan_ptr, next_vlan_ptr;
    bcm_l3_intf_t       l3_intf;
    uint32              vlan_count;
    int                 rvth = BCM_E_NONE;

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

    if (SOC_IPMCREPLSHR_GET(unit) ||
            soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;
        bcm_if_t *if_array = NULL;
        
        intf_max = IPMC_REPL_INTF_TOTAL(unit);
        alloc_size = intf_max * sizeof(bcm_if_t);
        if_array = sal_alloc(alloc_size, "IPMC repl interface array");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }

        IPMC_REPL_LOCK(unit);
        rv = bcm_fb_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
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
                rv = bcm_fb_ipmc_egress_intf_set(unit, ipmc_id, port,
                                                 if_count, if_array, 1, FALSE);
            } else {
                rv = BCM_E_NOT_FOUND;
            }
        }
        IPMC_REPL_UNLOCK(unit);
        sal_free(if_array);
        return rv;
    }

    IPMC_REPL_LOCK(unit);
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze) && 
        ((rv = _bcm_fb_ipmc_repl_freeze(unit)) < 0)) {
        IPMC_REPL_UNLOCK(unit);
        return rv;
    }

    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto del_done;
    }

    target_ms_bit = (l3_intf.l3a_intf_id >> 6) & 0x3f;
    ls_bit = l3_intf.l3a_intf_id & 0x3f;

    last_vlan_ptr = 0; /* Since ptr 0 should never be used! */
    vlan_count = 0;
    ms_bit = -1;

    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto del_done;
        }

        ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                        &vlan_entry, MSB_VLANf);
        if (ms_bit == target_ms_bit) {
            /* Delete existing entry */
            soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);
            if (!(ls_bits[ls_bit / 32] & (1 << (ls_bit % 32)))) {
                /* Doesn't exist! */
                rv = BCM_E_NOT_FOUND;
                goto del_done;
            }
            ls_bits[ls_bit / 32] &= ~(1 << (ls_bit % 32));
            soc_MMU_IPMC_VLAN_TBLm_field_set(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);

            if ((ls_bits[0] == 0) && (ls_bits[1] == 0)) {
                /* Wiped out the entry */
                /* Record before overwrite */
                next_vlan_ptr =
                    soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                           &vlan_entry, NEXTPTRf);
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                                            &vlan_entry, MSB_VLANf, 0);
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                                            &vlan_entry, NEXTPTRf, 0);
                if (last_vlan_ptr == 0) {
                    /* First entry for this (group, port) */
                    if (vlan_ptr == next_vlan_ptr) {
                        /* Only link in chain */
                        next_vlan_ptr = 0;
                    }
                    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id,
                                       port, &next_vlan_ptr, 0, 1)) < 0) {
                       goto del_done;
                    }
                } else {
                    /* Link over with last entry */
                    if (vlan_ptr == next_vlan_ptr) {
                        /* If this is end of chain, end on previous link */
                        next_vlan_ptr = last_vlan_ptr;
                    }
                    soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                            &prev_vlan_entry, NEXTPTRf, next_vlan_ptr);
                    if ((rv = WRITE_MMU_IPMC_VLAN_TBLm(unit,
                                    MEM_BLOCK_ALL, last_vlan_ptr,
                                    &prev_vlan_entry)) < 0) {
                        goto del_done;
                    }
                }

                IPMC_REPL_VE_USED_CLR(unit, vlan_ptr);
            }

            if ((rv = WRITE_MMU_IPMC_VLAN_TBLm(unit,
                                MEM_BLOCK_ALL, vlan_ptr, &vlan_entry)) < 0) {
                goto del_done;
            }

            IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)--;

            goto del_done;
        }

        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                          &vlan_entry, NEXTPTRf);
        sal_memcpy(&prev_vlan_entry, &vlan_entry,
                   sizeof(mmu_ipmc_vlan_tbl_entry_t));

        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                               LSB_VLAN_BMf, ls_bits);
        vlan_count +=
            _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
        if (vlan_count >= IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
            rv = BCM_E_NOT_FOUND;
            goto del_done;
        }
    }

    rv = BCM_E_NOT_FOUND;

 del_done:
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze)) {
        rvth = _bcm_fb_ipmc_repl_thaw(unit);
        rv = (rv < 0) ? rv : rvth;
    }
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_fb_ipmc_repl_delete_all
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
bcm_fb_ipmc_repl_delete_all(int unit, int ipmc_id, bcm_port_t port)
{
    int                 rv = BCM_E_NONE;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int                 vlan_ptr, last_vlan_ptr;
    int                 rvth = BCM_E_NONE;
    int                 num_vlans;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        /* Nothing to do */
        return BCM_E_NONE;
    }

    if (SOC_IPMCREPLSHR_GET(unit) ||
            soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        return bcm_fb_ipmc_egress_intf_set(unit, ipmc_id, port, 0, NULL, 1, FALSE);
    }

    IPMC_REPL_LOCK(unit);
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze) && 
        ((rv = _bcm_fb_ipmc_repl_freeze(unit)) < 0)) {
        IPMC_REPL_UNLOCK(unit);
        return rv;
    }

    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id,
                                  port, &vlan_ptr, 0, 0)) < 0) {
        goto del_all_done;
    }
    last_vlan_ptr = 0;
    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id,
                                  port, &last_vlan_ptr, 0, 1)) < 0) {
        goto del_all_done;
    }

    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto del_all_done;
        }

        if ((rv = WRITE_MMU_IPMC_VLAN_TBLm(unit,
                        MEM_BLOCK_ALL, vlan_ptr, soc_mem_entry_null(unit,
                            MMU_IPMC_VLAN_TBLm))) < 0) {
            goto del_all_done;
        }

        IPMC_REPL_VE_USED_CLR(unit, vlan_ptr);

        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                               &vlan_entry, NEXTPTRf);

        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                               LSB_VLAN_BMf, ls_bits);
        num_vlans = _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
        if (IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) < num_vlans) {
            IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) = 0;
            goto del_all_done;
        }
        IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id) -= num_vlans;
    }

 del_all_done:
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze)) {
        rvth = _bcm_fb_ipmc_repl_thaw(unit);
        rv = (rv < 0) ? rv : rvth;
    }
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_fb_ipmc_egress_intf_add
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
bcm_fb_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                            bcm_l3_intf_t *l3_intf)
{
    int                 rv = BCM_E_NONE;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int                 ms_bit, target_ms_bit, ls_bit;
    int                 vlan_ptr, last_vlan_ptr;
    pbmp_t pbmp, ubmp;
    uint32              vlan_count;
    int                 rvth = BCM_E_NONE;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    /* Check if port belongs to this VLAN */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_get(unit, l3_intf->l3a_vid, &pbmp, &ubmp));
    if (!SOC_PBMP_MEMBER(pbmp, port)) {
        return BCM_E_PARAM;
    }

    if (SOC_IPMCREPLSHR_GET(unit) ||
            soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        bcm_if_t *if_array = NULL;
        int  intf_num, intf_max, alloc_size, rv = BCM_E_NONE;

        intf_max = IPMC_REPL_INTF_TOTAL(unit);
        alloc_size = intf_max * sizeof(bcm_if_t);
        if_array = sal_alloc(alloc_size, "IPMC repl interface array");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }

        IPMC_REPL_LOCK(unit);
        rv = bcm_fb_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                         if_array, &intf_num);
        if (BCM_SUCCESS(rv)) {
            if (intf_num < intf_max) {
                if_array[intf_num++] = l3_intf->l3a_intf_id;
                rv = bcm_fb_ipmc_egress_intf_set(unit, ipmc_id, port,
                                                 intf_num, if_array, 1, FALSE);
            } else {
                rv = BCM_E_EXISTS;
            }
        }

        IPMC_REPL_UNLOCK(unit);
        sal_free(if_array);
        return rv;
    }

    IPMC_REPL_LOCK(unit);
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze) && 
        ((rv = _bcm_fb_ipmc_repl_freeze(unit)) < 0)) {
        IPMC_REPL_UNLOCK(unit);
        return rv;
    }

    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto add_done;
    }

    target_ms_bit = (l3_intf->l3a_intf_id >> 6) & 0x3f;
    ls_bit = l3_intf->l3a_intf_id & 0x3f;

    last_vlan_ptr = -1;
    vlan_count = 0;
    ms_bit = -1;

    if (IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        /*
         * Add to existing chain
         */
        while (vlan_ptr != last_vlan_ptr) {
            if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                              vlan_ptr, &vlan_entry)) < 0) {
                goto add_done;
            }

            ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                       &vlan_entry, MSB_VLANf);

            if (ms_bit == target_ms_bit) {
                /* Add to existing entry */
                break;
            }

            last_vlan_ptr = vlan_ptr;
            vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                              &vlan_entry, NEXTPTRf);

            soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);
            vlan_count +=
                _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
            if (vlan_count >= IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
                break;
            }
        }
    }

    if (ms_bit != target_ms_bit) {
        /* New entry, find slot */
        vlan_ptr = _fb_ipmc_repl_next_free_ptr(unit);
        if (vlan_ptr < 0) {
            /* Out of LS entries */
            rv = BCM_E_RESOURCE;
            goto add_done;
        }

        if (last_vlan_ptr < 0) {
            /* Update group table */
            if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id,
                                          port, &vlan_ptr, 0, 1)) < 0) {
                goto add_done;
            }
        } else {
            /* Update last entry */
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                               NEXTPTRf, vlan_ptr);
            if ((rv = WRITE_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                          last_vlan_ptr, &vlan_entry)) < 0) {
                goto add_done;
            }
        }

        sal_memset(&vlan_entry, 0, sizeof(vlan_entry));
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                           MSB_VLANf, target_ms_bit);
        /* Point to self */
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                           NEXTPTRf, vlan_ptr);

        IPMC_REPL_VE_USED_SET(unit, vlan_ptr);
    }

    /* Insert replacement */
    soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                     LSB_VLAN_BMf, ls_bits);
    if (ls_bits[ls_bit / 32] & (1 << (ls_bit % 32))) {
        /* Already exists! */
        rv = BCM_E_EXISTS;
        goto add_done;
    }
    ls_bits[ls_bit / 32] |= 1 << (ls_bit % 32);
    soc_MMU_IPMC_VLAN_TBLm_field_set(unit, &vlan_entry,
                                     LSB_VLAN_BMf, ls_bits);


    if ((rv = WRITE_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                       vlan_ptr, &vlan_entry)) < 0) {
        goto add_done;
    }

    IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)++;

 add_done:
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze)) {
        rvth = _bcm_fb_ipmc_repl_thaw(unit);
        rv = (rv < 0) ? rv : rvth;
    }
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_fb_ipmc_egress_intf_delete
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
bcm_fb_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                               bcm_l3_intf_t *l3_intf)
{
    int                 rv = BCM_E_NONE;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry, prev_vlan_entry;
    int                 ms_bit, target_ms_bit, ls_bit;
    int                 vlan_ptr, last_vlan_ptr, next_vlan_ptr;
    uint32              vlan_count;
    int                 rvth = BCM_E_NONE;

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    }

    if (SOC_IPMCREPLSHR_GET(unit) ||
            soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        bcm_if_t *if_array = NULL;
        int alloc_size, intf_max, if_count, if_cur, match, rv = BCM_E_NONE;

        intf_max = IPMC_REPL_INTF_TOTAL(unit);
        alloc_size = intf_max * sizeof(bcm_if_t);
        if_array = sal_alloc(alloc_size, "IPMC repl interface array");
        if (if_array == NULL) {
            return BCM_E_MEMORY;
        }

        IPMC_REPL_LOCK(unit);
        rv = bcm_fb_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
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
                rv = bcm_fb_ipmc_egress_intf_set(unit, ipmc_id, port,
                                                 if_count, if_array, 1, FALSE);
            } else {
                rv = BCM_E_NOT_FOUND;
            }
        }

        IPMC_REPL_UNLOCK(unit);
        sal_free(if_array);
        return rv;
    }

    IPMC_REPL_LOCK(unit);
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze) && 
        ((rv = _bcm_fb_ipmc_repl_freeze(unit)) < 0)) {
        IPMC_REPL_UNLOCK(unit);
        return rv;
    }

    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto del_done;
    }

    target_ms_bit = (l3_intf->l3a_intf_id >> 6) & 0x3f;
    ls_bit = l3_intf->l3a_intf_id & 0x3f;

    last_vlan_ptr = 0; /* Since ptr 0 should never be used! */
    vlan_count = 0;
    ms_bit = -1;

    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto del_done;
        }

        ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                        &vlan_entry, MSB_VLANf);
        if (ms_bit == target_ms_bit) {
            /* Delete existing entry */
            soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);
            if (!(ls_bits[ls_bit / 32] & (1 << (ls_bit % 32)))) {
                /* Doesn't exist! */
                rv = BCM_E_NOT_FOUND;
                goto del_done;
            }
            ls_bits[ls_bit / 32] &= ~(1 << (ls_bit % 32));
            soc_MMU_IPMC_VLAN_TBLm_field_set(unit, &vlan_entry,
                                             LSB_VLAN_BMf, ls_bits);

            if ((ls_bits[0] == 0) && (ls_bits[1] == 0)) {
                /* Wiped out the entry */
                /* Record before overwrite */
                next_vlan_ptr =
                    soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                           &vlan_entry, NEXTPTRf);
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                                            &vlan_entry, MSB_VLANf, 0);
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                                            &vlan_entry, NEXTPTRf, 0);
                if (last_vlan_ptr == 0) {
                    /* First entry for this (group, port) */
                    if (vlan_ptr == next_vlan_ptr) {
                        /* Only link in chain */
                        next_vlan_ptr = 0;
                    }
                    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port,
                                                &next_vlan_ptr, 0, 1)) < 0) {
                       goto del_done;
                    }
                } else {
                    /* Link over with last entry */
                    if (vlan_ptr == next_vlan_ptr) {
                        /* If this is end of chain, end on previous link */
                        next_vlan_ptr = last_vlan_ptr;
                    }
                    soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                            &prev_vlan_entry, NEXTPTRf, next_vlan_ptr);
                    if ((rv = WRITE_MMU_IPMC_VLAN_TBLm(unit,
                                    MEM_BLOCK_ALL, last_vlan_ptr,
                                    &prev_vlan_entry)) < 0) {
                        goto del_done;
                    }
                }

                IPMC_REPL_VE_USED_CLR(unit, vlan_ptr);
            }

            if ((rv = WRITE_MMU_IPMC_VLAN_TBLm(unit,
                                MEM_BLOCK_ALL, vlan_ptr, &vlan_entry)) < 0) {
                goto del_done;
            }

            IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)--;

            goto del_done;
        }

        last_vlan_ptr = vlan_ptr;
        vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                          &vlan_entry, NEXTPTRf);
        sal_memcpy(&prev_vlan_entry, &vlan_entry,
                   sizeof(mmu_ipmc_vlan_tbl_entry_t));

        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                               LSB_VLAN_BMf, ls_bits);
        vlan_count +=
            _shr_popcount(ls_bits[0]) + _shr_popcount(ls_bits[1]);
        if (vlan_count >= IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
            rv = BCM_E_NOT_FOUND;
            goto del_done;
        }
    }

    rv = BCM_E_NOT_FOUND;

del_done:
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze)) {
        rvth = _bcm_fb_ipmc_repl_thaw(unit);
        rv = (rv < 0) ? rv : rvth;
    }
    IPMC_REPL_UNLOCK(unit);
    return rv;
}


int
_bcm_fb_ipmc_egress_intf_add(int unit, int ipmc_id, bcm_port_t port,
                             int id, int is_l3)
{
    int *if_array = NULL;
    int intf_num, intf_max, alloc_size, rv = BCM_E_NONE;
    bcm_l3_intf_t       l3_intf;
    int         is_l3_intf = 0, int_l3_nh_created = -1;
    int         intf_index = 0, nh_index = 0;
    uint32      nh_flags;
    bcm_l3_egress_t         nh_info;
    egr_l3_next_hop_entry_t egr_nh;

    if (!SOC_IPMCREPLSHR_GET(unit) && !SOC_IS_TRX(unit) &&
        !soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        return BCM_E_UNAVAIL;
    }

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
    rv = bcm_fb_ipmc_egress_intf_get(unit, ipmc_id, port, intf_max,
                                     if_array, &intf_num);
    if (BCM_SUCCESS(rv)) {
        if (intf_num < intf_max) {
            if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
                if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, id)) {
                    id = id - BCM_XGS3_EGRESS_IDX_MIN(unit);
                } else {
                    /*
                     *  SDK-111244: to request L3 INTF index format for
                     * adding L3MC replication as well.
                     *
                     * Due to the L3_INTF can be shared by L3_NH, once user
                     *  request IPMC replication through L3_INTF index, an
                     *  internal L3_NH entry will always created internally
                     *  for such request.
                     */
                    is_l3_intf = TRUE;
                    intf_index = id;

                    /* index validation */
                    if (intf_index >= BCM_XGS3_L3_IF_TBL_SIZE(unit)) {
                        rv = BCM_E_PARAM;
                        goto intf_add_done;
                    } else if (!BCM_L3_INTF_USED_GET(unit, intf_index)) {
                        rv = BCM_E_NOT_FOUND;
                        goto intf_add_done;
                    }

                    if (IPMC_REPL_L3_INTF_TO_L3_NH_IPMC(unit, intf_index) !=
                            REPL_NH_INDEX_UNALLOCATED) {
                        id = IPMC_REPL_L3_INTF_TO_L3_NH_IPMC(unit, intf_index);
                        _bcm_xgs3_nh_ref_cnt_incr(unit, id);
                    } else {
                        bcm_l3_egress_t_init(&nh_info);
                        nh_flags = _BCM_L3_SHR_MATCH_DISABLE |
                                    _BCM_L3_SHR_WRITE_DISABLE;
                        rv = bcm_xgs3_nh_add(unit, nh_flags, &nh_info,
                                                &nh_index);
                        if (BCM_FAILURE(rv)) {
                            goto intf_add_done;
                        }

                        int_l3_nh_created = nh_index;

                        sal_memset(&egr_nh, 0,
                                    sizeof(egr_l3_next_hop_entry_t));
                        /* ENTRY_TYPEf=7 is for L3MC view */
                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            ENTRY_TYPEf, 7);

                        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                            L3MC__INTF_NUMf, id);

                        rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                                    MEM_BLOCK_ALL, nh_index, &egr_nh);
                        if (BCM_FAILURE(rv)) {
                            goto intf_add_done;
                        }

                        LOG_VERBOSE(BSL_LS_BCM_IPMC, (BSL_META_U(unit,
                                    "unit %d, IPMC replication for L3_INTF(id=%d):"
                                    "L3_NH(id=%d) is created!\n"),
                                    unit, id, nh_index));

                        /* change id from L3_INTF to L3_NH index */
                        id = nh_index;
                        IPMC_REPL_L3_INTF_TO_L3_NH_IPMC(unit, intf_index) = id;
                        IPMC_REPL_L3_NH_TO_L3_INTF_IPMC(unit, id) = intf_index;
                    }
                }
            } else {
                if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, id)) {
                    id = id - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
                }
            }
            if_array[intf_num++] = id;

            /* For IPMC, check port is a member of the L3 interface's VLAN.
             * Performing this check here is more efficient than performing
             * it in bcm_fb_ipmc_egress_intf_set.
             */ 
            if ((uint32)id > IPMC_REPL_INTF_TOTAL(unit)) {
                rv = BCM_E_PARAM;
                goto intf_add_done;
            }

            if (is_l3) {
                bcm_l3_intf_t_init(&l3_intf);
                
                if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
                    if (is_l3_intf){
                        l3_intf.l3a_intf_id = intf_index;
                    } else {
                        bcm_l3_egress_t egr_nh;

                        rv = bcm_xgs3_nh_get(unit, id, &egr_nh);
                        if (BCM_FAILURE(rv)) {
                            goto intf_add_done;
                        }
                        l3_intf.l3a_intf_id = egr_nh.intf;
                    }
                } else {
                    l3_intf.l3a_intf_id = id;
                }
                
                if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                    goto intf_add_done;
                }
            }

            rv = bcm_fb_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             intf_num, if_array, is_l3, FALSE);
        } else {
            rv = BCM_E_EXISTS;
        }
    }

intf_add_done:

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array); 

    if (BCM_FAILURE(rv) && (int_l3_nh_created != -1)) {
        nh_index = int_l3_nh_created;
        if (bcm_xgs3_nh_del(unit, 0, nh_index) != BCM_E_NONE) {
            LOG_VERBOSE(BSL_LS_BCM_IPMC, (BSL_META_U(unit,
                        "unit %d, can't delete internal L3_NH(id=%d)!\n"),
                        unit, nh_index));
        }
    }

    return rv;
}

int
_bcm_fb_ipmc_egress_intf_delete(int unit, int ipmc_id, bcm_port_t port,
                                int if_max, int id, int is_l3)
{
    int  intf_index = 0;
    int  *if_array = NULL;
    int  alloc_size, if_count, if_cur, match, rv = BCM_E_NONE;

    if (!SOC_IPMCREPLSHR_GET(unit) && !SOC_IS_TRX(unit) &&
        !soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        return BCM_E_CONFIG;
    }

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if (!IPMC_REPL_PORT_VLAN_COUNT(unit, port, ipmc_id)) {
        return BCM_E_NOT_FOUND;
    } else if ((if_max <= 0) || ((uint32)if_max > IPMC_REPL_INTF_TOTAL(unit))) {
        return BCM_E_PARAM;
    }
    alloc_size = if_max * sizeof(int);
    if_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (if_array == NULL) {
        return BCM_E_MEMORY;
    }

    IPMC_REPL_LOCK(unit);
    rv = bcm_fb_ipmc_egress_intf_get(unit, ipmc_id, port, if_max,
                                     if_array, &if_count);
    if (BCM_SUCCESS(rv)) {
        /* id now is still at encap_id format. Need to modify for GH case. */
        if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
            if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, id)) {
                id = id - BCM_XGS3_EGRESS_IDX_MIN(unit);
            } else {
                /*
                 *  SDK-111244 : to request L3 INTF index format for
                 * deleting L3MC replication as well.
                 */
                intf_index = id;

                /* index validation */
                if (intf_index >= BCM_XGS3_L3_IF_TBL_SIZE(unit)) {
                    rv = BCM_E_PARAM;
                    goto intf_del_done;
                } else if (!BCM_L3_INTF_USED_GET(unit, intf_index)) {
                    rv = BCM_E_NOT_FOUND;
                    goto intf_del_done;
                }

                if (IPMC_REPL_L3_INTF_TO_L3_NH_IPMC(unit, intf_index) ==
                            REPL_NH_INDEX_UNALLOCATED) {
                    rv = BCM_E_NOT_FOUND;
                    goto intf_del_done;
                }

                /* reassign "id" for further delete process */
                id = IPMC_REPL_L3_INTF_TO_L3_NH_IPMC(unit, intf_index);
            }
        }

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
            rv = bcm_fb_ipmc_egress_intf_set(unit, ipmc_id, port,
                                             if_count, if_array, is_l3, FALSE);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
    }

intf_del_done:

    IPMC_REPL_UNLOCK(unit);
    sal_free(if_array);
    return rv;
}

/*
 * Function:
 *	_bcm_fb_repl_list_compare
 * Description:
 *	Compare HW list starting at vlan_index to the VLAN list contained
 *      in vlan_vec.
 */

STATIC int
_bcm_fb_repl_list_compare(int unit, int vlan_index,
                          SHR_BITDCL *intf_vec)
{
    uint32		ms_bit, hw_ms_bit, ms_max;
    uint32		ls_bits[2], hw_ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int	                vlan_ptr, last_vlan_ptr;
    int  intf_less32 = 0; /* L3_INTF support <=32 */

    last_vlan_ptr = -1;
    vlan_ptr = vlan_index;
    ms_max = _SHR_BITDCLSIZE(IPMC_REPL_INTF_TOTAL(unit))/2; /* 32 -> 64 */

    if (ms_max == 0) {
        intf_less32 = 1;
        ms_max = 1;
    }

    for (ms_bit = 0; ms_bit < ms_max; ms_bit++) {
        ls_bits[0] = intf_vec[2 * ms_bit + 0];
        if (intf_less32) {
            ls_bits[1] = 0;
        } else {
            ls_bits[1] = intf_vec[2 * ms_bit + 1];
        }

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
            if ((hw_ms_bit != ms_bit) || (ls_bits[0] != hw_ls_bits[0]) ||
                (ls_bits[1] != hw_ls_bits[1])) {
                return BCM_E_NOT_FOUND;
            }
            last_vlan_ptr = vlan_ptr;
            vlan_ptr = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                          &vlan_entry, NEXTPTRf);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_fb_repl_list_free
 * Description:
 *	Release the IPMC_VLAN entries in the HW list starting at start_ptr.
 */

STATIC int
_bcm_fb_repl_list_free(int unit, int start_ptr)
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
        IPMC_REPL_VE_USED_CLR(unit, last_vlan_ptr);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_fb_repl_list_write
 * Description:
 *	Write the VLAN list contained in vlan_vec into the HW table.
 *      Return the start_index and total VLAN count.
 */

STATIC int
_bcm_fb_repl_list_write(int unit, int *start_index, int *count,
                        SHR_BITDCL *intf_vec)
{
    uint32		ms_bit, ms_max;
    uint32		ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int	                vlan_ptr, last_vlan_ptr;
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
    int  last_write_ptr = -1;
#endif
    int                 vlan_count = 0;

    int  intf_less32 = 0; /* L3_INTF support <=32 */
    last_vlan_ptr = -1;

    ms_max = _SHR_BITDCLSIZE(IPMC_REPL_INTF_TOTAL(unit)) / 2; /* 32 -> 64 */

    if (ms_max == 0) {
        intf_less32 = 1;
        ms_max = 1;
    }

    for (ms_bit = 0; ms_bit < ms_max; ms_bit++) {
        ls_bits[0] = intf_vec[2 * ms_bit + 0];
        if (intf_less32) {
            ls_bits[1] = 0;
        } else {
            ls_bits[1] = intf_vec[2 * ms_bit + 1];
        }

        if ((ls_bits[0] != 0) || (ls_bits[1] != 0)) {
            vlan_ptr = _fb_ipmc_repl_next_free_ptr(unit);
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
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
                last_write_ptr = last_vlan_ptr;
#endif
                if (vlan_ptr < 0) {
                    _bcm_fb_repl_list_free(unit, *start_index);
                    return BCM_E_RESOURCE;
                }
            } else {
                if (vlan_ptr < 0) {
                    return BCM_E_RESOURCE;
                }
                *start_index = vlan_ptr;
            }
            sal_memset(&vlan_entry, 0, sizeof(vlan_entry));
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                                               MSB_VLANf, ms_bit);
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
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit,
                          &vlan_entry, NEXTPTRf, last_vlan_ptr);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_IPMC_VLAN_TBLm(unit,
                   MEM_BLOCK_ALL, last_vlan_ptr, &vlan_entry));
        
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
        if (soc_feature(unit, soc_feature_ipmc_repl_penultimate) &&
            (last_write_ptr > 0)) {
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
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRIUMPH_SUPPORT */
    }

    *count = vlan_count;
    return BCM_E_NONE;
}


/*
 * Function:
 *	_bcm_fb_ipmc_egress_intf_nh_clear
 * Description:
 *	clear the internal created EGR_L3_NEXT_HOP entry
 *    especially for the IPMC needs.
 */

STATIC int
_bcm_fb_ipmc_egress_intf_nh_clear(
    int unit,
    int ipmc_id,
    bcm_port_t port,
    int if_count,
    bcm_if_t *if_array)
{
    int  *nh_array = NULL;
    int rv = BCM_E_NONE, rv1 = BCM_E_NONE, match;
    int if_num, nh_num, nh_count, if_max, nh_id, nh2intf;
    int alloc_size;
    bcm_l3_egress_t nh_info;


    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if ((uint32)if_count > IPMC_REPL_INTF_TOTAL(unit)) {
        return BCM_E_PARAM;
    }
    if_max = IPMC_REPL_INTF_TOTAL(unit);

    alloc_size = IPMC_REPL_INTF_TOTAL(unit) * sizeof(int);
    nh_array = sal_alloc(alloc_size, "IPMC repl interface array");
    if (nh_array == NULL) {
        return BCM_E_MEMORY;
    }


    IPMC_REPL_LOCK(unit);
    /* the intf get from bellow api is NH index */
    if ((rv = bcm_fb_ipmc_egress_intf_get(unit, ipmc_id, port, if_max,
                                           nh_array, &nh_count)) < 0) {
        goto internal_nh_del_done;
    }

    /* Check whether the old if_array is included in the new if_array */
    for (nh_num = 0; nh_num < nh_count; nh_num++) {
        match = 0;
        /* the NH index is not internally created */
        if (IPMC_REPL_L3_NH_TO_L3_INTF_IPMC(unit, nh_array[nh_num]) ==
                REPL_L3_INTF_UNALLOCATED) {
            continue;
        }
        for (if_num = 0; if_num < if_count; if_num++) {
            if ((uint32)if_array[if_num] > IPMC_REPL_INTF_TOTAL(unit)) {
                rv = BCM_E_PARAM;
                goto internal_nh_del_done;
            }
            if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
                nh_id = if_array[if_num];
            }
            else {
                nh_id = IPMC_REPL_L3_INTF_TO_L3_NH_IPMC(unit, if_array[if_num]);
            }
            /* if match */
            if ((nh_id != REPL_NH_INDEX_UNALLOCATED) &&
                    (nh_id == nh_array[nh_num])) {
                match = 1;
                break;
            }
        }
        /* Delete the internal created NH index*/
        if (match == 0) {

            /* remove this internal created L3_NH, when ref_count=1 */
            if ((rv = bcm_xgs3_nh_del(unit, 0, nh_array[nh_num])) < 0 ) {
                goto internal_nh_del_done;
            }
            /* make sure the NH is being deleted from HW */
            bcm_l3_egress_t_init(&nh_info);
            rv1 = bcm_xgs3_nh_get(unit, nh_array[nh_num], &nh_info);
            if (rv1 == BCM_E_NOT_FOUND) {
                nh2intf = IPMC_REPL_L3_NH_TO_L3_INTF_IPMC(unit,
                                                          nh_array[nh_num]);
                IPMC_REPL_L3_INTF_TO_L3_NH_IPMC(
                    unit, nh2intf) = REPL_NH_INDEX_UNALLOCATED;
                IPMC_REPL_L3_NH_TO_L3_INTF_IPMC(
                    unit, nh_array[nh_num]) = REPL_L3_INTF_UNALLOCATED;
                LOG_VERBOSE(BSL_LS_BCM_IPMC, (BSL_META_U(unit,
                            "unit %d, internal L3_NH(id=%d) "
                            "is deleted\n"),
                            unit, nh_array[nh_num]));
            }
        }
    }

    internal_nh_del_done:
    IPMC_REPL_UNLOCK(unit);
    sal_free(nh_array);
    return rv;
}

/*
 * Function:
 *	bcm_fb_ipmc_egress_intf_set
 * Purpose:
 *	Assign set of egress interfaces to port's replication list for chosen
 *	IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	ipmc_id  - The index number.
 *	port     - port to list.
 *	if_count - number of interfaces in replication list.
 *      if_array - (IN) List of interface numbers.
 *      is_l3    - (IN) FALSE if list contains non-L3 interface (ex. MPLS/SUBPORT)
 *      check_port - (IN) If is_l3 = TRUE, this parameter controls whether to
 *                        check the given port is a member in each L3 interface's 
 *                        VLAN. This check should be disabled when not needed,
 *                        in order to improve performance.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_fb_ipmc_egress_intf_set(int unit, int ipmc_id, bcm_port_t port,
                            int if_count, bcm_if_t *if_array, int is_l3,
                            int check_port)
{
    int			rvth = BCM_E_NONE, rv = BCM_E_NONE;
    SHR_BITDCL          *intf_vec = NULL;
    int			list_start_ptr, prev_start_ptr;
    int                 alloc_size, repl_hash, vlan_count, last_flag;
    int                 if_num;
    bcm_l3_intf_t       l3_intf;
    _bcm_repl_list_info_t *rli_start, *rli_current, *rli_prev;

    if (!SOC_IS_FBX(unit)) {
	return BCM_E_UNAVAIL;
    }

    if (!SOC_IPMCREPLSHR_GET(unit) && !SOC_IS_HURRICANEX(unit) && 
            !SOC_IS_GREYHOUND(unit) && !SOC_IS_GREYHOUND2(unit) &&
            !soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        return BCM_E_CONFIG;
    }

    IPMC_REPL_INIT(unit);
    IPMC_REPL_ID(unit, ipmc_id);
    REPL_PORT_CHECK(unit, port);

    if ((uint32)if_count > IPMC_REPL_INTF_TOTAL(unit)) {
        return BCM_E_PARAM;
    }

    alloc_size = SHR_BITALLOCSIZE(IPMC_REPL_INTF_TOTAL(unit));
    intf_vec = sal_alloc(alloc_size, "IPMC repl interface vector");
    if (intf_vec == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(intf_vec, 0, alloc_size);

    IPMC_REPL_LOCK(unit);
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze) && 
        ((rv = _bcm_fb_ipmc_repl_freeze(unit)) < 0)) {
        IPMC_REPL_UNLOCK(unit);
        sal_free(intf_vec);
        return rv;
    }

    /* Interface validation and vector construction */
    for (if_num = 0; if_num < if_count; if_num++) {
        if ((uint32)if_array[if_num] > IPMC_REPL_INTF_TOTAL(unit)) {
            rv = BCM_E_PARAM;
            goto intf_set_done;
        }
        if (is_l3 && check_port) {
            bcm_l3_intf_t_init(&l3_intf);
                
            if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
                bcm_l3_egress_t egr_nh;
                    
                rv = bcm_xgs3_nh_get(unit, if_array[if_num], &egr_nh);
                if (BCM_FAILURE(rv)) {
                    goto intf_set_done;
                }
                l3_intf.l3a_intf_id = egr_nh.intf;
            } else {
                l3_intf.l3a_intf_id = if_array[if_num];
            }
            
            if ((rv = bcm_esw_l3_intf_get(unit, &l3_intf)) < 0) {
                goto intf_set_done;
            }
        }
        SHR_BITSET(intf_vec, if_array[if_num]);
    }

    /* Check previous group pointer */
    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port,
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
            rv = _bcm_fb_repl_list_compare(unit, rli_current->index,
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
        /* delete the internal created NH idx when it is removed out of IPMC*/
        if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
            if ((rv = _bcm_fb_ipmc_egress_intf_nh_clear(
                        unit, ipmc_id, port, if_count, if_array)) < 0){
                goto intf_set_done;
            }
        }
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
        if ((rv = _bcm_fb_repl_list_write(unit, &list_start_ptr,
                                         &vlan_count, intf_vec)) < 0) {
            goto intf_set_done;
        }

        if (vlan_count > 0) {
            /* Update data structures */
            alloc_size = sizeof(_bcm_repl_list_info_t);
            rli_current = sal_alloc(alloc_size, "IPMC repl list info");
            if (rli_current == NULL) {
                /* Release list */
                _bcm_fb_repl_list_free(unit, list_start_ptr);
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
        /* delete the internal created NH idx when it is removed out of IPMC*/
        if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
            if ((rv = _bcm_fb_ipmc_egress_intf_nh_clear(
                        unit, ipmc_id, port, if_count, if_array)) < 0){
                goto intf_set_done;
            }
        }
    }

    last_flag = (vlan_count == 1);
    if (vlan_count > 0) {
        if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port,
                                    &list_start_ptr, last_flag, 1)) < 0) {
            if (rli_current->refcount == 0) {
                /* This was new */
                _bcm_fb_repl_list_free(unit, list_start_ptr);
                IPMC_REPL_LIST_INFO(unit) = rli_current->next;
                sal_free(rli_current);
            }
            goto intf_set_done;
        }

        (rli_current->refcount)++;
        /* we don't need this rli_current anymore */
    } else if (prev_start_ptr != 0) {
        list_start_ptr = 0;
        if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port,
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
                    rv = _bcm_fb_repl_list_free(unit, prev_start_ptr);
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
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze)) {
        rvth = _bcm_fb_ipmc_repl_thaw(unit);
        rv = (rv < 0) ? rv : rvth;
    }
    IPMC_REPL_UNLOCK(unit);
    if (intf_vec != NULL) {
        sal_free(intf_vec);
    }
    return rv;
}

/*
 * Function:
 *	bcm_fb_ipmc_egress_intf_get
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
bcm_fb_ipmc_egress_intf_get(int unit, int ipmc_id, bcm_port_t port,
                            int if_max, bcm_if_t *if_array, int *if_count)
{
    int			rv = BCM_E_NONE;
    uint32              ms_bit;
    uint32              ls_bits[2];
    mmu_ipmc_vlan_tbl_entry_t vlan_entry;
    int                 vlan_ptr, last_vlan_ptr;
    uint32              vlan_count;
    int                 ls_pos;
    int                 rvth = BCM_E_NONE;

    if (!SOC_IS_FBX(unit)) {
	return BCM_E_UNAVAIL;
    }

    if (!SOC_IPMCREPLSHR_GET(unit) && !SOC_IS_HURRICANEX(unit) && 
            !SOC_IS_GREYHOUND(unit) && !SOC_IS_GREYHOUND2(unit) &&
            !soc_feature(unit, soc_feature_ipmc_repl_penultimate)) {
        return BCM_E_CONFIG;
    }

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

    if (soc_feature(unit, soc_feature_ipmc_repl_freeze) && 
        ((rv = _bcm_fb_ipmc_repl_freeze(unit)) < 0)) {
        IPMC_REPL_UNLOCK(unit);
        return rv;
    }

    if ((rv = _fb_ipmc_vlan_ptr(unit, ipmc_id, port, &vlan_ptr, 0, 0)) < 0) {
        goto intf_get_done;
    }
    last_vlan_ptr = -1;
    vlan_count = 0;
    while (vlan_ptr != last_vlan_ptr) {
        if ((rv = READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ANY,
                                          vlan_ptr, &vlan_entry)) < 0) {
            goto intf_get_done;
        }
        ms_bit = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                                                    &vlan_entry, MSB_VLANf);
        soc_MMU_IPMC_VLAN_TBLm_field_get(unit, &vlan_entry,
                                         LSB_VLAN_BMf, ls_bits);
        for (ls_pos = 0; ls_pos < 64; ls_pos++) {
            if (ls_bits[ls_pos / 32] & (1 << (ls_pos % 32))) {
                if (if_max == 0) {
                    vlan_count++;
                } else {
                    if_array[vlan_count++] = (ms_bit * 64) + ls_pos;
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
    if (soc_feature(unit, soc_feature_ipmc_repl_freeze)) {
        rvth = _bcm_fb_ipmc_repl_thaw(unit);
        rv = (rv < 0) ? rv : rvth;
    }
    IPMC_REPL_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *	bcm_fb_ipmc_repl_set
 * Purpose:
 *	Assign set of VLANs provided to port's replication list for chosen
 *	IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	ipmc_id  - The index number.
 *	port     - port to list.
 *	vlan_vec - (IN) vector of replicated VLANs common to selected ports.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_fb_ipmc_repl_set(int unit, int ipmc_id, bcm_port_t port,
                     bcm_vlan_vector_t vlan_vec)
{
    int	rv = BCM_E_NONE;
    bcm_if_t *if_array = NULL;
    bcm_l3_intf_t l3_intf;
    pbmp_t pbmp, ubmp;
    int  intf_num, intf_max, alloc_size, vid;

    if (!SOC_IS_FBX(unit)) {
	return BCM_E_UNAVAIL;
    }

    if (!SOC_IPMCREPLSHR_GET(unit) &&  !SOC_IS_HURRICANEX(unit) && 
            !SOC_IS_GREYHOUND(unit) && !!SOC_IS_GREYHOUND2(unit) &&
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
            if ((rv = bcm_esw_vlan_port_get(unit, vid, &pbmp, &ubmp)) < 0) {
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

    rv = bcm_fb_ipmc_egress_intf_set(unit, ipmc_id, port,
                                     intf_num, if_array, 1, FALSE);

    sal_free(if_array);
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Reload IPMC state on XGS3 class of devices
 */
int
_bcm_xgs3_ipmc_reinit(int unit)
{
    int i, rv = BCM_E_NONE;
    int ipmc_ptr;
    int l3_min, l3_max;
    _bcm_esw_ipmc_t  *info = IPMC_INFO(unit);
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry;

    info->ipmc_group_info = NULL;

    IPMC_GROUP_NUM(unit) = soc_mem_index_count(unit, L3_IPMCm);

    info->ipmc_count = 0;

    info->ipmc_group_info =
        sal_alloc(IPMC_GROUP_NUM(unit) * sizeof(_bcm_esw_ipmc_group_info_t),
                  "IPMC group info");
    if (info->ipmc_group_info == NULL) {
        rv = BCM_E_MEMORY;
        goto ret_err;
    }
    sal_memset(info->ipmc_group_info, 0, 
               IPMC_GROUP_NUM(unit) * sizeof(_bcm_esw_ipmc_group_info_t));

    if (SOC_IS_FBX(unit)) {
        l3_entry_ipv4_multicast_entry_t *l3x_entry, *ipmc_table;
        l3_entry_ipv6_multicast_entry_t *l3x_v6entry, *ipmc6_table;
        ipmc_entry_t *l3_ipmc_entry, *l3_ipmc_table;
        int ipmc_tbl_sz, ipmc6_tbl_sz, l3_ipmc_tbl_sz;
        uint8 flags;

        l3_min = soc_mem_index_min(unit, L3_ENTRY_IPV4_MULTICASTm);
        l3_max = soc_mem_index_max(unit, L3_ENTRY_IPV4_MULTICASTm);
        ipmc_tbl_sz = sizeof(l3_entry_ipv4_multicast_entry_t) * \
                                (l3_max - l3_min + 1);
        ipmc_table = soc_cm_salloc(unit, ipmc_tbl_sz, "ipmc tbl dma");
        if (ipmc_table == NULL) {
            rv = BCM_E_MEMORY;
            goto ret_err;
        }

        memset((void *)ipmc_table, 0, ipmc_tbl_sz);
        if ((rv = soc_mem_read_range(unit, L3_ENTRY_IPV4_MULTICASTm,
                                     MEM_BLOCK_ANY,
                                     l3_min, l3_max, ipmc_table)) < 0) {
            soc_cm_sfree(unit, ipmc_table);
            goto ret_err;
        }

        for (i = l3_min; i <= l3_max; i++) {
            l3x_entry = soc_mem_table_idx_to_pointer(unit,
                                          L3_ENTRY_IPV4_MULTICASTm,
                                          l3_entry_ipv4_multicast_entry_t *,
                                          ipmc_table, i);

            if (soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, l3x_entry,
                                                         VALIDf) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, l3x_entry,
                                                         IPMCf) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, l3x_entry,
                                                         V6f)) {
                continue;
            }

            ipmc_ptr = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                                   l3x_entry, L3MC_INDEXf);
            IPMC_USED_SET(unit, ipmc_ptr);
            {
#if defined(BCM_FIREBOLT_SUPPORT)
                _bcm_l3_cfg_t l3cfg;
                sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
                l3cfg.l3c_flags |= BCM_L3_IPMC;
                l3cfg.l3c_flags |=  BCM_L3_HIT;
                if (soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                                        l3x_entry, RPEf)) {
                    l3cfg.l3c_flags |=  BCM_L3_RPE;
                }
                if (soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, l3x_entry,
                                                             DST_DISCARDf)) {
                    l3cfg.l3c_flags |=  BCM_L3_DST_DISCARD;
                }
                l3cfg.l3c_ipmc_group = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                                        l3x_entry, GROUP_IP_ADDRf);
                l3cfg.l3c_src_ip_addr = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                                        l3x_entry, SOURCE_IP_ADDRf);
                l3cfg.l3c_vid = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                                        l3x_entry, VLAN_IDf);
                l3cfg.l3c_prio = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                                        l3x_entry, PRIf);
                if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV4_MULTICASTm, CLASS_IDf)) {
                    l3cfg.l3c_lookup_class =
                        soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit,
                                                    l3x_entry, CLASS_IDf);
                }
                rv = _bcm_xgs3_ipmc_l3entry_list_add(unit, ipmc_ptr, &l3cfg);                
#endif /* BCM_FIREBOLT_SUPPORT */
            }
            
            if (rv < 0) {
                soc_cm_sfree(unit, ipmc_table);
                goto ret_err;
            }
        }
        soc_cm_sfree(unit, ipmc_table);

        /*
         * Deal with IPv6 multicast table now
         */
        l3_min = soc_mem_index_min(unit, L3_ENTRY_IPV6_MULTICASTm);
        l3_max = soc_mem_index_max(unit, L3_ENTRY_IPV6_MULTICASTm);
        ipmc6_tbl_sz = sizeof(l3_entry_ipv6_multicast_entry_t) * \
                                       (l3_max - l3_min + 1);

        ipmc6_table = soc_cm_salloc(unit, ipmc6_tbl_sz, "ipmc v6 tbl dma");
        if (ipmc6_table == NULL) {
            rv = BCM_E_MEMORY;
            goto ret_err;
        }

        memset((void *)ipmc6_table, 0, ipmc6_tbl_sz);

        if ((rv = soc_mem_read_range(unit, L3_ENTRY_IPV6_MULTICASTm,
                                     MEM_BLOCK_ANY,
                                     l3_min, l3_max, ipmc6_table)) < 0) {
            soc_cm_sfree(unit, ipmc6_table);
            goto ret_err;
        }

        for (i = l3_min; i <= l3_max; i++) {

            l3x_v6entry = soc_mem_table_idx_to_pointer(unit,
                                          L3_ENTRY_IPV6_MULTICASTm,
                                          l3_entry_ipv6_multicast_entry_t *,
                                          ipmc6_table, i);
            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         VALID_0f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         VALID_1f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         VALID_2f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         VALID_3f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         V6_0f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         V6_1f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         V6_2f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         V6_3f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         IPMC_0f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         IPMC_1f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         IPMC_2f) == 0) {
                continue;
            }

            if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                         IPMC_3f) == 0) {
                continue;
            }

            ipmc_ptr = soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit,
                                    l3x_v6entry, L3MC_INDEX_0f);
            IPMC_USED_SET(unit, ipmc_ptr);
            {
#if defined(BCM_FIREBOLT_SUPPORT)
                _bcm_l3_cfg_t l3cfg;
                sal_memset(&l3cfg, 0, sizeof(_bcm_l3_cfg_t));
                l3cfg.l3c_flags |= BCM_IPMC_IP6;
                l3cfg.l3c_flags |= BCM_L3_IPMC;
                l3cfg.l3c_flags |=  BCM_L3_HIT;
                if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit,
                                                        l3x_v6entry, RPEf)) {
                    l3cfg.l3c_flags |=  BCM_L3_RPE;
                }
                if (soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, l3x_v6entry,
                                                             DST_DISCARDf)) {
                    l3cfg.l3c_flags |=  BCM_L3_DST_DISCARD;
                }
                /* Get group address. */
                soc_mem_ip6_addr_get(unit, L3_ENTRY_IPV6_MULTICASTm, l3x_v6entry, 
                                     GROUP_IP_ADDR_LWR_64f, l3cfg.l3c_ip6, 
                                     SOC_MEM_IP6_LOWER_ONLY);
                
                soc_mem_ip6_addr_get(unit, L3_ENTRY_IPV6_MULTICASTm, l3x_v6entry, 
                                     GROUP_IP_ADDR_UPR_56f, l3cfg.l3c_ip6, 
                                     SOC_MEM_IP6_UPPER_ONLY);
                
                /* Get source  address. */
                soc_mem_ip6_addr_get(unit, L3_ENTRY_IPV6_MULTICASTm, l3x_v6entry, 
                                     SOURCE_IP_ADDR_LWR_64f, l3cfg.l3c_sip6, 
                                     SOC_MEM_IP6_LOWER_ONLY);
                soc_mem_ip6_addr_get(unit, L3_ENTRY_IPV6_MULTICASTm, l3x_v6entry, 
                                     SOURCE_IP_ADDR_UPR_64f, l3cfg.l3c_sip6, 
                                     SOC_MEM_IP6_UPPER_ONLY);
                
                l3cfg.l3c_ip6[0] = 0xff;    /* Set entry to multicast*/ 
                l3cfg.l3c_vid = soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit,
                                                        l3x_v6entry, VLAN_IDf);
                l3cfg.l3c_prio = soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit,
                                                        l3x_v6entry, PRIf);
                l3cfg.l3c_lookup_class = soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit,
                                                l3x_v6entry, CLASS_IDf);
                rv = _bcm_xgs3_ipmc_l3entry_list_add(unit, ipmc_ptr, &l3cfg);
#endif /* BCM_FIREBOLT_SUPPORT */
            }   
            if (rv < 0) {
                soc_cm_sfree(unit, ipmc6_table);
                goto ret_err;
            }
        }
        soc_cm_sfree(unit, ipmc6_table);

        /* Recover multicast mode from HW cache */
        rv = _bcm_esw_ipmc_repl_wb_flags_get(unit,
                                             _BCM_IPMC_WB_MULTICAST_MODE,
                                             &flags);

        if (flags) {
            /*
             * Increase reference count for all mulitcast groups now
             */
            l3_min = soc_mem_index_min(unit, L3_IPMCm);
            l3_max = soc_mem_index_max(unit, L3_IPMCm);
            l3_ipmc_tbl_sz = sizeof(ipmc_entry_t) * \
                (l3_max - l3_min + 1);

            l3_ipmc_table = soc_cm_salloc(unit, l3_ipmc_tbl_sz,
                                          "L3 ipmc tbl dma");
            if (l3_ipmc_table == NULL) {
                rv = BCM_E_MEMORY;
                goto ret_err;
            }

            memset((void *)l3_ipmc_table, 0, l3_ipmc_tbl_sz);

            if ((rv = soc_mem_read_range(unit, L3_IPMCm,
                                         MEM_BLOCK_ANY,
                                         l3_min, l3_max,
                                         l3_ipmc_table)) < 0) {
                soc_cm_sfree(unit, l3_ipmc_table);
                goto ret_err;
            }

            for (i = l3_min; i <= l3_max; i++) {
                l3_ipmc_entry = soc_mem_table_idx_to_pointer(unit,
                                                             L3_IPMCm,
                                                         ipmc_entry_t *,
                                                         l3_ipmc_table, i);
                if (0 == soc_L3_IPMCm_field32_get(unit, l3_ipmc_entry,
                                                  VALIDf)) {
                    continue;
                }
        
                /* It's a multicast group we need to note. */
                IPMC_USED_SET(unit, i);
            }
            soc_cm_sfree(unit, l3_ipmc_table);
        }

        /*
         * Recover replication state
         */
        rv = _bcm_xgs3_ipmc_repl_reload(unit);
    }

ret_err:
    if (BCM_FAILURE(rv)) {

        if (info->ipmc_group_info != NULL) {
            for (i = 0; i < IPMC_GROUP_NUM(unit); i++) {
                ipmc_l3entry = IPMC_GROUP_INFO(unit, i)->l3entry_list;
                while (ipmc_l3entry != NULL) {
                    IPMC_GROUP_INFO(unit, i)->l3entry_list = ipmc_l3entry->next;
                    sal_free(ipmc_l3entry);
                    ipmc_l3entry = IPMC_GROUP_INFO(unit, i)->l3entry_list;
                }
            }
            sal_free(info->ipmc_group_info);
            info->ipmc_group_info = NULL;
        }
    } else {
        info->ipmc_initialized = TRUE;
    }

    return rv;
}

/*
 * Sync IPMC state on XGS3 class of devices
 */
int
_bcm_fb_ipmc_sync(
    int unit,
    uint8 **scache_ptr)
{
    int i;

    /* Input parameters sanity check. */
    if (NULL == scache_ptr) {
        return (BCM_E_PARAM);
    }

    /* Synce IPMC replication info*/
    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_NEXT_HOPm); i++) {
        sal_memcpy((*scache_ptr),
                   &(_fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc[i]),
                   sizeof(int));
        (*scache_ptr) += sizeof(int);
    }

    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
        sal_memcpy((*scache_ptr),
                   &(_fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc[i]),
                   sizeof(int));
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}

int
_bcm_fb_ipmc_scahce_size_get(
    int unit,
    uint32 *size)
{
    int num_l3_nh, num_l3_intf;

    /* Input parameters sanity check. */
    if (NULL == size) {
        return (BCM_E_PARAM);
    }

    *size = 0;
    /* Get IPMC replication info scache size*/
    num_l3_nh = soc_mem_index_count(unit, EGR_L3_NEXT_HOPm);
    num_l3_intf = soc_mem_index_count(unit, EGR_L3_INTFm);
    *size = (num_l3_nh * sizeof(int)) + (num_l3_intf * sizeof(int));

    return BCM_E_NONE;
}

int
_bcm_fb_ipmc_repl_scache_recover(
    int unit,
    uint8 **scache_ptr)
{
    int i;

    /* Input parameters sanity check. */
    if (NULL == scache_ptr) {
        return (BCM_E_PARAM);
    }

    /* Recover IPMC replication info state */
    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_NEXT_HOPm); i++) {
        sal_memcpy(&(_fb_repl_info[unit]->l3_nh_to_l3_intf_ipmc[i]),
                   (*scache_ptr),
                   sizeof(int));
        (*scache_ptr) += sizeof(int);
    }

    for (i = 0; i < soc_mem_index_count(unit, EGR_L3_INTFm); i++) {
        sal_memcpy(&(_fb_repl_info[unit]->l3_intf_to_l3_nh_ipmc[i]),
                   (*scache_ptr),
                   sizeof(int));
        (*scache_ptr) += sizeof(int);
    }

    return BCM_E_NONE;
}





#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_xgs3_ipmc_repl_sw_dump
 * Purpose:
 *     Displays IPMC replication information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_xgs3_ipmc_repl_sw_dump(int unit)
{
    int                   i, j, port;
    _fb_repl_info_t       *repl_info;
    uint32                *bitmap;
    _fb_repl_port_info_t  *port_info;
    _bcm_repl_list_info_t *rli_start, *rli_current;

    /*
     * _fb_repl_info
     */

    LOG_CLI((BSL_META_U(unit,
                        "  IPMC REPL Info -\n")));
    repl_info = _fb_repl_info[unit];
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
            for (i = 0, j = 0; i < IPMC_GROUP_NUM(unit); i++) {
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

    return;
}

/*
 * Function:
 *     _bcm_xgs3_ipmc_sw_dump
 * Purpose:
 *     Displays IPMC information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_xgs3_ipmc_sw_dump(int unit)
{
    int                   i, j;
    _bcm_esw_ipmc_t      *ipmc_info;
    _bcm_esw_ipmc_l3entry_t *ipmc_l3entry;


    /*
     * xgs3_ipmc_info
     */

    ipmc_info = IPMC_INFO(unit);

    LOG_CLI((BSL_META_U(unit,
                        "  XGS3 IPMC Info -\n")));
    LOG_CLI((BSL_META_U(unit,
                        "    Init        : %d\n"), ipmc_info->ipmc_initialized));
    LOG_CLI((BSL_META_U(unit,
                        "    Size        : %d\n"), IPMC_GROUP_NUM(unit)));
    LOG_CLI((BSL_META_U(unit,
                        "    Count       : %d\n"), ipmc_info->ipmc_count));

    LOG_CLI((BSL_META_U(unit,
                        "    Alloc index :")));
    if (ipmc_info->ipmc_group_info != NULL) {
        for (i = 0, j = 0; i < IPMC_GROUP_NUM(unit); i++) {
            /* If not set, skip print */
            if (!IPMC_USED_ISSET(unit, i)) {
                continue;
            }
            if (!(j % 10)) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "  %5d"), i));
            j++;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    LOG_CLI((BSL_META_U(unit,
                        "    Reference count (index:value) :")));
    if (ipmc_info->ipmc_group_info != NULL) {
        for (i = 0, j = 0; i < IPMC_GROUP_NUM(unit); i++) {
            if (!IPMC_USED_ISSET(unit, i)) {
                continue;
            }
            if (!(j % 4)) {
                LOG_CLI((BSL_META_U(unit,
                                    "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "  %5d:%-5d"), i, IPMC_GROUP_INFO(unit, i)->ref_count));
            j++;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    LOG_CLI((BSL_META_U(unit,
                        "    IP6 (index:value) :")));
    if (ipmc_info->ipmc_group_info != NULL) {
        for (i = 0, j = 0; i < IPMC_GROUP_NUM(unit); i++) {
            ipmc_l3entry = IPMC_GROUP_INFO(unit, i)->l3entry_list;
            while (ipmc_l3entry != NULL) {
                if (ipmc_l3entry->ip6 == 1) {
                    if (!(j % 4)) {
                        LOG_CLI((BSL_META_U(unit,
                                            "\n    ")));
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        "  %5d:%-5d"), i, ipmc_l3entry->ip6));
                    j++;
                }
                ipmc_l3entry = ipmc_l3entry->next;
            }
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    _bcm_xgs3_ipmc_repl_sw_dump(unit);

    return;
}
#endif /*  BCM_SW_STATE_DUMP_DISABLE */

#endif /* BCM_FIREBOLT_SUPPORT */

#endif  /* INCLUDE_L3 */

typedef int _bcm_esw_firebolt_ipmc_not_empty; /* Make ISO compilers happy. */

