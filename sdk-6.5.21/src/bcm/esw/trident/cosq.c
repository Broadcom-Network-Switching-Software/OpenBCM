/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS Queue Management
 * Purpose: API to set different cosq, priorities, and scheduler registers.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/defs.h>
#if defined(BCM_TRIDENT_SUPPORT)
#include <bcm/types.h>
#include <bcm/pkt.h>
#include <bcm/tx.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/trident.h>

#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/cosq.h>

#include <bcm_int/esw_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define TD_CELL_FIELD_MAX       0xffff

/* MMU cell size in bytes */
#define _BCM_TD_PACKET_HEADER_BYTES       64    /* bytes */
#define _BCM_TD_DEFAULT_MTU_BYTES         1536  /* bytes */
#define _BCM_TD_TOTAL_CELLS               46080 /* 45k cells */
#define _BCM_TD_BYTES_PER_CELL            208   /* bytes */
#define _BCM_TD_BYTES_TO_CELLS(_byte_)  \
    (((_byte_) + _BCM_TD_BYTES_PER_CELL - 1) / _BCM_TD_BYTES_PER_CELL)

#define _BCM_TD_NUM_UCAST_QUEUE_GROUP            10
#define _BCM_TD_NUM_MCAST_QUEUE_GROUP            5
#define _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP        16
#define _BCM_TD_NUM_SCHEDULER                    9
#define _BCM_TD_NUM_S2_SCHEDULER                 3
#define _BCM_TD_NUM_S3_SCHEDULER                 4

#define _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE        8
#define _BCM_TD_NUM_NON_SC_QM_MCAST_QUEUE        4

#define _BCM_TD_S1_SCHEDULER_NUM_INPUT           7
#define _BCM_TD_S2_SCHEDULER_NUM_INPUT           9
#define _BCM_TD_S3_SCHEDULER_NUM_INPUT           2
#define _BCM_TD_EXT_S3_SCHEDULER_NUM_INPUT       18

#define _BCM_TD_NUM_CPU_QUEUES                48
#define _TD_NUM_COS    NUM_COS
#define _BCM_TD_MAX_NUM_LLFC_EXTQ     4  /* At max, 4 EXTqs can be affected
                                            by a given LLFC priority. */
#define _BCM_TD_NUM_PFC_CLASS         8  /* TD supports eight PFC priorities. */

#define WRED_MIN_PKT_SIZE            148 /* Should be greater than one cell.*/
#define SOC_WRED_WR_MAX_NUM_PORTS      4 /* 1 extended and 1 non-extended port
                                                         from each pipeline.*/

#define PORT_IS_VALID(_u, _p)       \
        IS_XE_PORT(_u, _p) ? 1 :    \
        IS_HG_PORT(_u, _p) ? 1 : 0


#define STG_BITS_PER_PORT       2
#define STG_PORTS_PER_WORD      (32 / STG_BITS_PER_PORT)
#define STG_WORD(port)          ((port) / STG_PORTS_PER_WORD)
#define STG_BITS_SHIFT(port)    \
        (STG_BITS_PER_PORT * ((port) % STG_PORTS_PER_WORD))

/* Shifts first LSB byte of the mac n times. */
#define MAC_LSB_SHIFT(d, s, n)   \
            d[5] = s[5] << n;

typedef enum {
    _BCM_TD_COSQ_TYPE_UCAST,
    _BCM_TD_COSQ_TYPE_MCAST,
    _BCM_TD_COSQ_TYPE_EXT_UCAST
} _bcm_td_cosq_type_t;

typedef struct _bcm_td_voq_profile_s {
    int         id;
    int         hw_index;
    int         int_prio2q[16];
} _bcm_td_voq_profile_t;

typedef struct _bcm_td_cosq_node_s {
    struct _bcm_td_cosq_node_s *parent;
    struct _bcm_td_cosq_node_s *sibling;
    struct _bcm_td_cosq_node_s *child;
    bcm_gport_t gport;
    int numq;
    int base;
    int cosq_attached_to;
    int hw_cosq_attached_to;
    int level;
    _bcm_td_voq_profile_t voq_profile;
} _bcm_td_cosq_node_t;

typedef struct _bcm_td_cosq_port_info_s {
    _bcm_td_cosq_node_t ucast[_BCM_TD_NUM_UCAST_QUEUE_GROUP];
    _bcm_td_cosq_node_t mcast[_BCM_TD_NUM_MCAST_QUEUE_GROUP];
    _bcm_td_cosq_node_t ext_ucast[_BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP];
    _bcm_td_cosq_node_t sched[_BCM_TD_NUM_SCHEDULER];
} _bcm_td_cosq_port_info_t;

typedef struct _bcm_td_cosq_cpu_cosq_config_s {
    /* All MC queues have DB and MCQE space */
    int enable;
    int q_min_limit;
    int q_shared_limit;
    uint8 q_limit_dynamic;
    uint8 q_limit_enable;
} _bcm_td_cosq_cpu_cosq_config_t;

/* Structure to store bitmaps of PFC queue. */
typedef struct _bcm_td_cosq_pfc_cos_bmp_s
{
   uint32 uc_bmp;         /* Bitmap of unicast queue. */
   uint32 mc_bmp;         /* Bitmap of multicast queue. */
   int    ext_uc_count;   /* Number of extended queues. */
   int    ext_uc_qid[_BCM_TD_MAX_NUM_LLFC_EXTQ]; /* IDs of extended queues. */
} _bcm_td_cosq_pfc_cos_bmp_t;

/* Global structure to store default state of port and switch. */
typedef struct _wred_war_ctrl_s
{
   soc_port_t  port[SOC_WRED_WR_MAX_NUM_PORTS];
   int         port_mode[SOC_WRED_WR_MAX_NUM_PORTS];
   int         port_crc_mode[SOC_WRED_WR_MAX_NUM_PORTS];
   int         is_ge_port[SOC_WRED_WR_MAX_NUM_PORTS];
   port_tab_entry_t port_entry[SOC_WRED_WR_MAX_NUM_PORTS];
   epc_link_bmap_entry_t epc_entry;           /* EPC state. */
   uint32      stg_entry[SOC_MAX_MEM_WORDS];  /* STP group ports state map.   */
   int         parity_gen_en;
   int         parity_chk_en;
   int         enable[SOC_WRED_WR_MAX_NUM_PORTS];
   int         speed[SOC_WRED_WR_MAX_NUM_PORTS];
   int         duplex[SOC_WRED_WR_MAX_NUM_PORTS];
   bcm_port_abil_t advert[SOC_WRED_WR_MAX_NUM_PORTS];
   int         autoneg[SOC_WRED_WR_MAX_NUM_PORTS];
}_wred_war_ctrl_t;

_wred_war_ctrl_t war; /* Global control structure to store 
                       * WRED workaround configuration.
                       */

STATIC _bcm_td_cosq_port_info_t *_bcm_td_cosq_port_info[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td_voq_cos_map_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td_wred_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_mem_t *_bcm_td_sample_int_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_td_feedback_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_td_llfc_profile[BCM_MAX_NUM_UNITS];
STATIC soc_profile_reg_t *_bcm_td_ext_llfc_profile[BCM_MAX_NUM_UNITS];

STATIC _bcm_td_cosq_cpu_cosq_config_t *_bcm_td_cosq_cpu_cosq_config[BCM_MAX_NUM_UNITS][_BCM_TD_NUM_CPU_QUEUES];

STATIC const soc_field_t _bcm_td_cpq_en_field[] = {
    CPQ_EN0f, CPQ_EN1f,
    CPQ_EN2f, CPQ_EN3f,
    CPQ_EN4f, CPQ_EN5f,
    CPQ_EN6f, CPQ_EN7f,
    CPQ_EN8f, CPQ_EN9f
};
STATIC const soc_field_t _bcm_td_cpq_index_field[] = {
    CPQ_PROFILE_INDEX0f, CPQ_PROFILE_INDEX1f,
    CPQ_PROFILE_INDEX2f, CPQ_PROFILE_INDEX3f,
    CPQ_PROFILE_INDEX4f, CPQ_PROFILE_INDEX5f,
    CPQ_PROFILE_INDEX6f, CPQ_PROFILE_INDEX7f,
    CPQ_PROFILE_INDEX8f, CPQ_PROFILE_INDEX9f
};
STATIC const soc_field_t _bcm_td_eqtb_index_field[] = {
    EQTB_INDEX0f, EQTB_INDEX1f
};
STATIC const soc_field_t _bcm_td_act_offset_field[] = {
    QNTZ_ACT_OFFSET0f, QNTZ_ACT_OFFSET1f
};
STATIC const soc_field_t _bcm_td_sitb_sel_field[] = {
    SITB_SEL0f, SITB_SEL1f
};
static const soc_reg_t _bcm_td_ext_uc_spid_regs[] =
{
    OP_EX_PORT_CONFIG_SPID_0r, OP_EX_PORT_CONFIG_SPID_1r,
    OP_EX_PORT_CONFIG_SPID_2r, OP_EX_PORT_CONFIG_SPID_3r,
    OP_EX_PORT_CONFIG_SPID_4r
};
static const soc_field_t _bcm_td_uc_spid_fields[] = {
    COS0_SPIDf, COS1_SPIDf, COS2_SPIDf, COS3_SPIDf,
    COS4_SPIDf, COS5_SPIDf, COS6_SPIDf, COS7_SPIDf,
    COS8_SPIDf, COS9_SPIDf
};
static const soc_field_t _bcm_td_ext_uc_spid_fields[] = {
    Q0_SPIDf, Q1_SPIDf, Q2_SPIDf, Q3_SPIDf,
    Q4_SPIDf, Q5_SPIDf, Q6_SPIDf, Q7_SPIDf,
    Q8_SPIDf, Q9_SPIDf, Q10_SPIDf, Q11_SPIDf,
    Q12_SPIDf, Q13_SPIDf, Q14_SPIDf, Q15_SPIDf,
    Q16_SPIDf, Q17_SPIDf, Q18_SPIDf, Q19_SPIDf,
    Q20_SPIDf, Q21_SPIDf, Q22_SPIDf, Q23_SPIDf,
    Q24_SPIDf, Q25_SPIDf, Q26_SPIDf, Q27_SPIDf,
    Q28_SPIDf, Q29_SPIDf, Q30_SPIDf, Q31_SPIDf,
    Q32_SPIDf, Q33_SPIDf, Q34_SPIDf, Q35_SPIDf,
    Q36_SPIDf, Q37_SPIDf, Q38_SPIDf, Q39_SPIDf,
    Q40_SPIDf, Q41_SPIDf, Q42_SPIDf, Q43_SPIDf,
    Q44_SPIDf, Q45_SPIDf, Q46_SPIDf, Q47_SPIDf,
    Q48_SPIDf, Q49_SPIDf, Q50_SPIDf, Q51_SPIDf,
    Q52_SPIDf, Q53_SPIDf, Q54_SPIDf, Q55_SPIDf,
    Q56_SPIDf, Q57_SPIDf, Q58_SPIDf, Q59_SPIDf,
    Q60_SPIDf, Q61_SPIDf, Q62_SPIDf, Q63_SPIDf,
    Q64_SPIDf, Q65_SPIDf, Q66_SPIDf, Q67_SPIDf,
    Q68_SPIDf, Q69_SPIDf, Q70_SPIDf, Q71_SPIDf,
    Q72_SPIDf, Q73_SPIDf
};

STATIC int _bcm_td_create_default_profile(int unit, bcm_port_t gport, int numq,
                                          _bcm_td_voq_profile_t *profile);

STATIC int _bcm_td_voq_gport_mapping_set(int unit, bcm_port_t gport, 
                                         int int_prio, bcm_cos_t cos);

/* Number of COSQs configured for this device */
STATIC int _bcm_td_num_cosq[SOC_MAX_NUM_DEVICES];

extern sal_mutex_t cosq_sync_lock[SOC_MAX_NUM_DEVICES];
#define TD_COSQ_LOCK(unit)      do { \
                                    if (cosq_sync_lock[unit]) { \
                                        sal_mutex_take(cosq_sync_lock[unit], \
                                                       sal_mutex_FOREVER); \
                                    } \
                                } while (0)
#define TD_COSQ_UNLOCK(unit)    do { \
                                    if (cosq_sync_lock[unit]) { \
                                        sal_mutex_give(cosq_sync_lock[unit]); \
                                    } \
                                } while (0)

#define _BCM_TD_VOQ_QID_MIN_ID      (64)
#define _BCM_TD_VOQ_QID_MAX_ID      (127)

#define _BCM_TD_IS_VOQ_GPORT(unit, gport)     \
 ((BCM_GPORT_IS_UCAST_QUEUE_GROUP((gport))) &&  \
  ((BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET((gport)) >= _BCM_TD_NUM_UCAST_QUEUE_GROUP) && \
   (BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET((gport)) <  \
            (_BCM_TD_NUM_UCAST_QUEUE_GROUP + _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP))))


#define _BCM_TD_NUM_COS_IN_QUEUE_GROUP(unit, gport) (4) 

#define _BCM_TD_VOQ_GPORT_QUEUE_ID(unit, gport, offset)  \
    (_BCM_TD_VOQ_QID_MIN_ID +   (offset) +               \
     ((BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET((gport)) - _BCM_TD_NUM_UCAST_QUEUE_GROUP) * 16)) 
        
#define _BCM_TD_IS_PORT_DMVOQ_CAPABLE(u, p) \
            (((&SOC_INFO((u)))->port_num_ext_cosq[(p)]) ? 1 : 0)

STATIC int _bcm_td_cosq_voq_init(int unit, bcm_port_t port);

STATIC int
_bcm_td_cosq_dynamic_thresh_enable_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_t type, int *arg);

/*
 * queues on non-CPU ports:
 *     5 multicast queue group (1 queue per group)
 *         queue 0 - 3 can connected to input 0 of S3.0 - S3.3 scheduler
 *                                   or input 0 - 3 of MC group scheduler
 *         queue 4 (SC/QM queue) is fixed connected to input 4 of S1 scheduler
 *     10 unicast queue group (1 queue per group)
 *         queue 0 - 3 can connected to input 1 of S3.0 - S3.3 scheduler
 *                                   or input 4 - 7 of S2 scheduler
 *         queue 4 - 7 are fixed connected to input 8 - 11 of S2 scheduler
 *         queue 8 (SC queue) is fixed connected to input 5 of S1 scheduler
 *         queue 9 (QM queue) is fixed connected to input 6 of S1 scheduler
 *     16 extended unicast queue group (up to 4 queues per group)
 *         only exist in extended queue ports
 *         queue 0 - 15 are fixed connected to input 2 - 17 of S3.0 scheduler
 *         queue 16 - 31 are fixed connected to input 2 - 17 of S3.1 scheduler
 *         queue 32 - 47 are fixed connected to input 2 - 17 of S3.2 scheduler
 *         queue 48 - 63 are fixed connected to input 2 - 17 of S3.3 scheduler
 * schedulers on non-CPU ports:
 *     1 S1 scheduler (7 input)
 *     1 MC group scheduler (4 input)
 *         fixed connected to input 0 of S1 scheduler
 *     3 S2 schedulers (12 input)
 *         scheduler 0 - 2 are fixed connected to input 1 - 3 of S1 scheduler
 *     4 S3 schedulers (8 input for regular port, 72 input for extended port)
 *         scheduler 0 - 3 are fixed connected to input 0 - 3 of S2 scheduler
 *
 * Unicast queue group gport format:
 *   31       26 25                   14 13                        0
 *  +-------.---+---.-------.-------.---+---.-------.-------.-------+
 *  |0 0 1 0 0 1|   sysportid (port)    |            qid            |
 *  +-------.---+---.-------.-------.---+---.-------.-------.-------+
 * qid: 0 - 9 for unicast queue, 10 - 25 for extended unicast queue
 *
 * Multicast queue group gport format:
 *   31       26 25                   14 13                        0
 *  +-------.---+---.-------.-------.---+---.-------.-------.-------+
 *  |0 0 1 1 0 0|   sysportid (port)    |            qid            |
 *  +-------.---+---.-------.-------.---+---.-------.-------.-------+
 * qid: 0 - 4
 *
 * Scheduler gport format:
 *   31       26 25               16 15            8 7             0
 *  +-------.---+---.-------.-------+-------.-------+-------.-------+
 *  |0 0 1 1 0 1|    scheduler id   |    module     |     port      |
 *  +-------.---+---.-------.-------+-------.-------+-------.-------+
 * qid: 0 - 8
 */

#ifndef BCM_SW_STATE_DUMP_DISABLE
STATIC int
_bcm_td_cosq_port_info_dump(int unit, bcm_port_t port)
{
    _bcm_td_cosq_port_info_t *port_info;
    _bcm_td_cosq_node_t *node;
    int index, empty;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    port_info = &_bcm_td_cosq_port_info[unit][port];

    empty = TRUE;
    for (index = 0; index < _BCM_TD_NUM_SCHEDULER; index++) {
        if (port_info->sched[index].numq > 0) {
            empty = FALSE;
            break;
        }
    }
    if (empty) {
        for (index = 0; index < _BCM_TD_NUM_UCAST_QUEUE_GROUP; index++) {
            if (port_info->ucast[index].numq > 0) {
                empty = FALSE;
                break;
            }
        }
    }
    if (empty) {
        for (index = 0; index < _BCM_TD_NUM_MCAST_QUEUE_GROUP; index++) {
            if (port_info->mcast[index].numq > 0) {
                empty = FALSE;
                break;
            }
        }
    }
    if (empty) {
        for (index = 0; index < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP; index++) {
            if (port_info->ext_ucast[index].numq > 0) {
                empty = FALSE;
                break;
            }
        }
    }

    if (empty == TRUE) {
        return BCM_E_NOT_FOUND;
    }

    LOG_CLI((BSL_META_U(unit,
                        "=== port %d\n"), port));
    for (index = 0; index < _BCM_TD_NUM_UCAST_QUEUE_GROUP; index++) {
        node = &port_info->ucast[index];
        LOG_CLI((BSL_META_U(unit,
                            "ucast[%d]: gport=%x numq=%d base=%d cosq_att=%d "
                 "hw_cosq_att=%d level=%d\n"),
                 index, node->gport, node->numq, node->base,
                 node->cosq_attached_to, node->hw_cosq_attached_to,
                 node->level));
        if (node->numq == 0) {
            continue;
        }
        if (node->parent != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  parent gport=%x\n"), node->parent->gport));
        }
        if (node->sibling != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  sibling gport=%x\n"), node->sibling->gport));
        }
        if (node->child != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  child  gport=%x\n"), node->child->gport));
        }
    }

    for (index = 0; index < _BCM_TD_NUM_MCAST_QUEUE_GROUP; index++) {
        node = &port_info->mcast[index];
        LOG_CLI((BSL_META_U(unit,
                            "mcast[%d]: gport=%x numq=%d base=%d cosq_att=%d "
                 "hw_cosq_att=%d level=%d\n"),
                 index, node->gport, node->numq, node->base,
                 node->cosq_attached_to, node->hw_cosq_attached_to,
                 node->level));
        if (node->numq == 0) {
            continue;
        }
        if (node->parent != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  parent gport=%x\n"), node->parent->gport));
        }
        if (node->sibling != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  sibling gport=%x\n"), node->sibling->gport));
        }
        if (node->child != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  child  gport=%x\n"), node->child->gport));
        }
    }

    for (index = 0; index < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP; index++) {
        node = &port_info->ext_ucast[index];
        LOG_CLI((BSL_META_U(unit,
                            "ext_ucast[%d]: gport=%x numq=%d base=%d cosq_att=%d "
                 "hw_cosq_att=%d level=%d\n"),
                 index, node->gport, node->numq, node->base,
                 node->cosq_attached_to, node->hw_cosq_attached_to,
                 node->level));
        if (node->numq == 0) {
            continue;
        }
        if (node->parent != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  parent gport=%x\n"), node->parent->gport));
        }
        if (node->sibling != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  sibling gport=%x\n"), node->sibling->gport));
        }
        if (node->child != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  child  gport=%x\n"), node->child->gport));
        }
    }

    for (index = 0; index < _BCM_TD_NUM_SCHEDULER; index++) {
        node = &port_info->sched[index];
        LOG_CLI((BSL_META_U(unit,
                            "sched[%d]: gport=%x numq=%d base=%d cosq_att=%d "
                 "hw_cosq_att=%d level=%d\n"),
                 index, node->gport, node->numq, node->base,
                 node->cosq_attached_to, node->hw_cosq_attached_to,
                 node->level));
        if (node->numq == 0) {
            continue;
        }
        if (node->parent != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  parent gport=%x\n"), node->parent->gport));
        }
        if (node->sibling != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  sibling gport=%x\n"), node->sibling->gport));
        }
        if (node->child != NULL) {
            LOG_CLI((BSL_META_U(unit,
                                "  child  gport=%x\n"), node->child->gport));
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *     _bcm_td_cosq_node_get
 * Purpose:
 *     Get internal information of queue group or scheduler type GPORT
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) queue group/scheduler GPORT identifier
 *     modid      - (OUT) module identifier
 *     port       - (OUT) port number
 *     id         - (OUT) queue group or scheduler identifier
 *     node       - (OUT) node structure for the specified GPORT
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_cosq_node_get(int unit, bcm_gport_t gport, bcm_module_t *modid,
                      bcm_port_t *port, int *id, _bcm_td_cosq_node_t **node)
{
    soc_info_t *si;
    _bcm_td_cosq_port_info_t *port_info;
    _bcm_td_cosq_node_t *node_base;
    bcm_module_t encap_modid, modid_out;
    bcm_port_t encap_port, port_out;
    uint32 encap_id;
    int result, index;

    si = &SOC_INFO(unit);

    if (_bcm_td_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid_out));
        port_out = BCM_GPORT_MCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        encap_modid = (BCM_GPORT_SCHEDULER_GET(gport) >> 8) & 0xff;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, encap_modid, &result));
        if (result != TRUE) {
            return BCM_E_PORT;
        }
        encap_port = BCM_GPORT_SCHEDULER_GET(gport) & 0xff;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, encap_modid,
                                     encap_port, &modid_out, &port_out));
    } else {
        return BCM_E_PORT;
    }

    if (!SOC_PORT_VALID(unit, port_out)) {
        return BCM_E_PORT;
    }
    port_info = &_bcm_td_cosq_port_info[unit][port_out];

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        if (index < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
            node_base = port_info->ucast;
        } else {
            index -= _BCM_TD_NUM_UCAST_QUEUE_GROUP;
            if (si->port_num_ext_cosq[port_out] == 0 ||
                index >= _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP) {
                return BCM_E_PORT;
            }
            node_base = port_info->ext_ucast;
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        encap_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        index = encap_id;
        if (index >= _BCM_TD_NUM_MCAST_QUEUE_GROUP) {
            return BCM_E_PORT;
        }
        node_base = port_info->mcast;
    } else { /* scheduler */
        encap_id = (BCM_GPORT_SCHEDULER_GET(gport) >> 16) & 0x3ff;
        index = encap_id;
        if (index >= _BCM_TD_NUM_SCHEDULER) {
            return BCM_E_PORT;
        }
        node_base = port_info->sched;
    }

    if (node_base[index].numq == 0) {
        return BCM_E_NOT_FOUND;
    }

    if (modid != NULL) {
        *modid = modid_out;
    }
    if (port != NULL) {
        *port = port_out;
    }
    if (id != NULL) {
        *id = encap_id;
    }
    if (node != NULL) {
        *node = &node_base[index];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_port_resolve
 * Purpose:
 *     Resolve following COS queue related GRPOT
 *         BCM_GPORT_TYPE_UCAST_QUEUE_GROUP
 *         BCM_GPORT_TYPE_MCAST_QUEUE_GROUP
 *         BCM_GPORT_TYPE SCHEDULER
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) queue group/scheduler GPORT identifier
 *     modid      - (OUT) module identifier
 *     port       - (OUT) port number
 *     trunk_id   - (OUT) trunk identifier
 *     id         - (OUT) queue group or scheduler identifier
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td_cosq_port_resolve(int unit, bcm_gport_t gport, bcm_module_t *modid,
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id)
{
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_node_get(unit, gport, modid, port, id, NULL));

	if (trunk_id != NULL) {
        *trunk_id = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_localport_resolve
 * Purpose:
 *     Resolve GRPOT if it is a local port
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) GPORT identifier
 *     local_port - (OUT) local port number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_cosq_localport_resolve(int unit, bcm_gport_t gport,
                               bcm_port_t *local_port)
{
    bcm_module_t module;
    bcm_port_t port;
    bcm_trunk_t trunk;
    int id, result;

    if (!BCM_GPORT_IS_SET(gport)) {
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
        *local_port = gport;
        return BCM_E_NONE;
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &module, &port, &trunk, &id));

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, module, &result));
    if (result == FALSE) {
        return BCM_E_PORT;
    }

    *local_port = port;

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_child_node_at_input(_bcm_td_cosq_node_t *node, int cosq,
                                  _bcm_td_cosq_node_t **child_node)
{
    _bcm_td_cosq_node_t *cur_node;

    if (NULL == node) {
        return BCM_E_PARAM;
    }

    for (cur_node = node->child; cur_node; cur_node = cur_node->sibling) {
        if (cur_node->cosq_attached_to == cosq) {
            break;
        }
    }
    if (!cur_node) {
        return BCM_E_NOT_FOUND;
    }

    *child_node = cur_node;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_index_resolve
 * Purpose:
 *     Find hardware index for the given port/cosq
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number or GPORT identifier
 *     cosq       - (IN) COS queue number
 *     style      - (IN) index style (_BCM_TD_COSQ_INDEX_STYLE_XXX)
 *     local_port - (OUT) local port number
 *     index      - (OUT) result index
 *     count      - (OUT) number of index
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td_cosq_index_resolve(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           _bcm_td_cosq_index_style_t style,
                           bcm_port_t *local_port, int *index, int *count)
{
    soc_info_t *si;
    _bcm_td_cosq_node_t *node, *cur_node, *s3_node;
    bcm_port_t resolved_port, mmu_port;
    int resolved_index;
    int id, startq, numq, is_ext_cosq;

    si = &SOC_INFO(unit);

    if (cosq < -1) {
        return BCM_E_PARAM;
    } else if (cosq == -1) {
        startq = 0;
    } else {
        startq = cosq;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
        BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, port, NULL, &resolved_port, &id,
                                   &node));
        if (node->hw_cosq_attached_to < 0) { /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }
        numq = node->numq;
        if (startq >= numq) {
            return BCM_E_PARAM;
        }
    } else {
        /* optionally validate port */
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &resolved_port));
        if (resolved_port < 0) {
            return BCM_E_PORT;
        }
        node = NULL;
        numq = 0;
    }

    switch (style) {
    case _BCM_TD_COSQ_INDEX_STYLE_BUCKET:
        /*
         * hardware index for single queue bucket control:
         *     (MAXBUCKETCONFIG_64, MAXBUCKET,
         *      MINBUCKETCONFIG_64, MINBUCKET)
         *     cpu: index 0 - 47
         *     loopback port: index 0 - 4
         *     regular port:
         *         index 0 - 9 for unicast queue 0 - queue 9
         *         index 10 - 14 for multicast queue 0 - queue 4
         *     extended queue port:
         *         index 0 - 63 for extended unicast queue 0 - queue 63
         *         index 64 - 73 for unicast queue 0 - queue 9
         *         index 74 - 78 for multicast queue 0 - queue 4
         * hardware index for S3 schuduler bucket control:
         *     (S3_MAXBUCKETCONFIG_64, S3_MAXBUCKET,
         *      S3_MINBUCKETCONFIG_64, S3_MINBUCKET)
         *     (not for cpu port)
         *     index 0 - 3 for S3.0, S3.1, S3.2, S3.3 respectively
         * hardware index for S2 schuduler input bucket control:
         *     (S2_MAXBUCKETCONFIG_64, S2_MAXBUCKET,
         *      S2_MINBUCKETCONFIG_64, S2_MINBUCKET)
         *     (not for cpu port)
         *     index 0 for MC group
         *     input 1 - 3 for S2.0, S2.1, S2.2 respevtively
         * for non-gport:
         *     cpu port
         *         use cosq as hardware queue index
         *     port other than cpu:
         *         cosq 0 for the summary of unicast and multicast queue 0
         *         cosq 1 for the summary of unicast and multicast queue 1
         *         cosq 2 for the summary of unicast and multicast queue 2
         *         cosq 3 for the summary of unicast and multicast queue 3
         *         cosq 4 - 7 for unicast queue 4 - queue 7
         *         cosq 8 for unicast SC queue
         *         cosq 9 for unicast QM queue
         *         what do we want multicast SC/QM queue to be ??
         */
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (si->port_num_ext_cosq[resolved_port] == 0) {
                    resolved_index = node->base;
                } else {
                    if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                        /* regular unicast queue */
                        resolved_index = 64 + node->base;
                    } else { /* extended unicast queue */
                        resolved_index = node->base + startq * 16;
                    }
                }
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                if (si->port_num_ext_cosq[resolved_port] == 0) {
                    resolved_index = 10 + node->base;
                } else {
                    resolved_index = 74 + node->base;
                }
            } else { /* scheduler */
                if (node->level == 2 || node->level == 3) {
                    numq = 1;
                    if (startq >= numq) {
                        return BCM_E_PARAM;
                    }
                    resolved_index = node->hw_cosq_attached_to;
                } else {
                    return BCM_E_PARAM;
                }
            }
        } else { /* node == NULL */
            if (IS_CPU_PORT(unit, resolved_port)) {
                numq = si->port_num_cosq[resolved_port];
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                resolved_index = startq;
            } else {
                numq = _BCM_TD_NUM_UCAST_QUEUE_GROUP;
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                if (startq < _BCM_TD_NUM_S3_SCHEDULER) {
                    /* use S3.0 - S3.3 for unicast and multicast queue 0 - 3 */
                    resolved_index = startq;
                } else { /* unicast queue 4 - 7, SC queue, QM queue */
                    resolved_index = si->port_num_ext_cosq[resolved_port] +
                        startq;
                }
            }
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_BUCKET_MODE:
        /*
         * hardware bit position for SHAPING_CONTROL
         *     cpu: bit 0 - 47
         *     regular port, extended queue port, and loopback port:
         *         bit 0 - 9 for unicast queue 0 - queue 9
         *         bit 10 - 14 for multicast queue 0 - queue 4
         *         bit 15 - 18 for S3.0, S3.1, S3.2, S3.3 respectively
         *         bit 19 for MC group scheduler bucket mode
         *         bit 20 - 22 for S2.0, S2.1, S2.2, respectively
         *     extended queue port:
         *         bit 23 - 86 for extended unicast queue 0 - queue 63
         *         bit 48 and up are actually in EXT1_SHAPING_CONTROL_PORTx
         *         for example bit 48 means bit 0 in EXT1_SHAPING_CONTROL_PORTx
         *         and bit 55 means bit 7 in EXT1_SHAPING_CONTROL_PORTx.
         * for non-gport:
         *     use the same interpretation as _BCM_TD_COSQ_INDEX_STYLE_BUCKET.
         */
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    /* regular unicast queue */
                    resolved_index = node->base;
                } else { /* extended unicast queue */
                    resolved_index = 23 + node->base + startq * 16;
                }
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                resolved_index = 10 + node->base;
            } else { /* scheduler */
                numq = 1;
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                if (node->level == 2) {
                    resolved_index = 19 + node->hw_cosq_attached_to;
                } else if (node->level == 3) {
                    resolved_index = 15 + node->hw_cosq_attached_to;
                } else {
                    return BCM_E_PARAM;
                }
            }
        } else { /* node == NULL */
            if (IS_CPU_PORT(unit, resolved_port)) {
                numq = si->port_num_cosq[resolved_port];
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                resolved_index = startq;
            } else {
                numq = _BCM_TD_NUM_UCAST_QUEUE_GROUP;
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                if (startq < _BCM_TD_NUM_S3_SCHEDULER) {
                    /* S3.0 - S3.3 for unicast queue 0 - 3 */
                    resolved_index = 15 + startq;
                } else { /* unicast queue 4 - 7, SC queue, QM queue */
                    resolved_index = startq;
                }
            }
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_WRED:
        /*
         * hardware index for regular port:
         *     (WRED_CONFIG, WRED_AVG_QSIZE)
         *     (not for cpu or loopback port)
         *     index 0 - 7 for unicast queue 0 - 7
         * hardware index for extended queue port:
         *     (DMVOQ_WRED_CONFIG, VOQ_WRED_AVG_QSIZE)
         *     index 0 - 63 for extended unicast queue 0 - 63
         *     index 64 - 71 for unicast queue 0 - 7
         *     (WRED_CONFIG, WRED_AVG_QSIZE)
         *     index 0: summary of unicast queue 0 and extended queue 0 - 15
         *     index 1: summary of unicast queue 1 and extended queue 16 - 31
         *     index 2: summary of unicast queue 2 and extended queue 32 - 47
         *     index 3: summary of unicast queue 3 and extended queue 48 - 63
         *     these summary does not need to equal to the sum of corresponding
         *     entries in DMVOQ_WRED_CONFIG
         * for non-gport:
         *     regular port:
         *         use cosq as hardware queue index
         *         cosq 0 - 7 for unicast queue 0 - queue 7
         *     extended queue port:
         *         cosq 0 - 7 for unicast queue 0 - queue 7
         *         cosq 8 - 71 for extended unicast queue 0 - 63
         *         instead of using hardwre queue index, adjust it to
         *         make the unicast queue has the same cosq number as regular
         *         port
         */
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    /* regular unicast queue */
                    if (node->base >= _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE) {
                        /* unicast SC or QM queue */
                        return BCM_E_PARAM;
                    }
                    if (si->port_num_ext_cosq[resolved_port] == 0) {
                        resolved_index = node->base;
                    } else {
                        resolved_index = 64 + node->base;
                    }
                } else { /* extended unicast queue */
                    resolved_index = node->base + startq * 16;
                }
            } else { /* multicast queue group or scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            if (IS_CPU_PORT(unit, resolved_port) ||
                IS_LB_PORT(unit, resolved_port)) {
                return BCM_E_PORT;
            }
            /* regular ucast and extended ucast queues */
            numq = _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE +
                si->port_num_ext_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            if (startq < _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE) {
                resolved_index = si->port_num_ext_cosq[resolved_port] + startq;
            } else {
                resolved_index = startq - _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE;
            }
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_WRED_DROP_THRESH:
        /*
         * hardware index:
         *     (WRED_DROP_THD_UC_ENQ0, WRED_DROP_THD_UC_ENQ1,
         *      WRED_DROP_THD_UC_DEQ0, WRED_DROP_THD_UC_DEQ1)
         *     (not for cpu or loopback port)
         *     regular port:
         *         index 0 - 63 for port 1 extended unicast queue 63 - 0
         *         index 64 - 127 for port 2 extended unicast queue 63 - 0
         *         index 128 - 191 for port 3 extended unicast queue 63 - 0
         *         index 192 - 255 for port 4 extended unicast queue 63 - 0
         *         index 256 - 319 for port 34 extended unicast queue 63 - 0
         *         index 320 - 383 for port 35 extended unicast queue 63 - 0
         *         index 384 - 447 for port 36 extended unicast queue 63 - 0
         *         index 448 - 511 for port 37 extended unicast queue 63 - 0
         *         index 512 - 519 for port 64 unicast queue 0 - 7
         *         index 520 - 775 for port 1 - 32 unicast queue 0 - 7
         *         index 776 - 783 for port 65 unicast queue 0 - 7
         *         index 784 - 1023 for port 34 - 63 unicast queue 0 - 7
         * for non-gport:
         *     regular port:
         *         use cosq as hardware queue index
         *         cosq 0 - 7 for unicast queue 0 - queue 7
         *     extended queue port:
         *         cosq 0 - 7 for unicast queue 0 - queue 7
         *         cosq 8 - 71 for extended unicast queue 0 - 63
         */
        is_ext_cosq = FALSE;
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    /* regular unicast queue */
                    if (node->base >= _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE) {
                        /* unicast SC or QM queue */
                        return BCM_E_PARAM;
                    }
                    resolved_index = node->base;
                } else { /* extended unicast queue */
                    resolved_index = 63 - node->base + startq * 16;
                    is_ext_cosq = TRUE;
                }
            } else { /* multicast queue group or scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            if (IS_CPU_PORT(unit, resolved_port) ||
                IS_LB_PORT(unit, resolved_port)) {
                return BCM_E_PORT;
            }
            /* regular ucast and extended ucast queues */
            numq = _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE +
                si->port_num_ext_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            if (startq < _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE) {
                resolved_index = startq;
            } else {
                resolved_index =
                    63 - (startq - _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE);
                is_ext_cosq = TRUE;
            }
        }
        /* Add port base offset */
        mmu_port = si->port_p2m_mapping[si->port_l2p_mapping[resolved_port]];
        if (is_ext_cosq) {
            if (mmu_port < 33) {
                resolved_index += (mmu_port - 1) *
                    si->port_num_ext_cosq[resolved_port];
            } else {
                resolved_index += 256 + (mmu_port - 34) *
                    si->port_num_ext_cosq[resolved_port];
            }
        } else {
            if (mmu_port < 64) {
                resolved_index +=
                    512 + mmu_port * _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE;
            } else if (mmu_port == 64) {
                resolved_index += 512;
            } else {
                resolved_index += 776;
            }
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_SCHEDULER:
        /*
         * hardware index for (S1) COSWEIGHTS:
         *     cpu port
         *         index 0 - 47 are for queue 0 - 47
         *     regular port, extended queue port, and loopback port:
         *         index 0 is for multicast group scheduler
         *         index 1 - 3 are for S2.1, S2.2, S2.3 respectively
         *         index 4 is for multicast SC and QM queue
         *         index 5 is for unicast SC queue
         *         index 6 is for unicast QM queue
         * hardware index for S2_COSWEIGHTS:
         *     (not for cpu port)
         *     index 0 - 3 are for S3.0, S3.1, S3.2, S3.3 respectively
         *     index 4 - 11 are for unicast queue 0 - 7
         * hardware bit position for S2_COSMASK, S2_MINSPCONFIG:
         *     (not for cpu port)
         *     bit 0 - 3 are for S3.0, S3.1, S3.2, S3.3 respectively
         *     bit 4 - 11 are for unicast queue 0 - 7
         * hardware index for S3_COSWEIGHTS:
         *     (not for cpu port)
         *     regular port:
         *         index 0, 2, 4, 6 are for multicast queue 0, 1, 2, 3
         *             respectively
         *         index 1, 3, 5, 7 are for unicast queue 0, 1, 2, 3
         *             respectively
         *     extended queue port:
         *         index 0, 18, 36, 54 are for multicast queue 0, 1, 2, 3
         *             respectively
         *         index 1, 19, 37, 55 are for unicast queue 0, 1, 2, 3
         *             respectively
         *         index 2 - 17 are for extended unicast queue 0 - 15
         *         index 20 - 35 are for extended unicast queue 16 - 31
         *         index 38 - 53 are for extended unicast queue 32 - 47
         *         index 56 - 71 are for extended unicast queue 48 - 63
         * hardware bit position for S3_COSMASK, S3_MINSPCONFIG:
         *     (not for cpu port)
         *     there are 4 entries in each register
         *         entry 0 is for input to S3.0
         *         entry 1 is for input to S3.1
         *         entry 2 is for input to S3.2
         *         entry 3 is for input to S3.3
         *     regular port and extended queue port:
         *         bit 0 of entry 0 is for multicast queue 0
         *         bit 0 of entry 1 is for multicast queue 1
         *         bit 0 of entry 2 is for multicast queue 2
         *         bit 0 of entry 3 is for multicast queue 3
         *         bit 1 of entry 0 is for unicast queue 0
         *         bit 1 of entry 1 is for unicast queue 1
         *         bit 1 of entry 2 is for unicast queue 2
         *         bit 1 of entry 3 is for unicast queue 3
         *     extended queue port:
         *         bit 2 - 17 of entry 0 are for extended unicast queue 0 - 15
         *         bit 2 - 17 of entry 1 are for extended unicast queue 16 - 31
         *         bit 2 - 17 of entry 2 are for extended unicast queue 32 - 47
         *         bit 2 - 17 of entry 3 are for extended unicast queue 48 - 63
         * hardware bit position for S3_COSMASK_MC, S3_MINSPCONFIG_MC:
         *    bit 0 - 3 for multicast queue 0 - 3
         */
        if (node != NULL) {
            if (BCM_GPORT_IS_SCHEDULER(port)) {
                for (cur_node = node->child; cur_node != NULL;
                     cur_node = cur_node->sibling) {
                    if (cur_node->cosq_attached_to == startq) {
                        break;
                    }
                }
                if (cur_node == NULL) {
                    if (node->level == 3 &&
                        si->port_num_ext_cosq[resolved_port] != 0) {
                        /* search to see if it matches the extended unicast
                         * queue index from the corresponding S3.0 node */
                        for (s3_node = node->parent->child; s3_node != NULL;
                             s3_node = s3_node->sibling) {
                            if (s3_node->hw_cosq_attached_to == 0) { /* S3.0 */
                                break;
                            }
                        }
                        if (s3_node != NULL) {
                            for (cur_node = s3_node->child; cur_node != NULL;
                                 cur_node = cur_node->sibling) {
                                if (cur_node->cosq_attached_to == startq) {
                                    break;
                                }
                            }
                        }
                    }
                }
                if (cur_node == NULL) {
                    /* Nothing is attached to the specified cosq */
                    resolved_index = -1;
                } else if (node->level == 3) { /* S3 scheduler */
                    resolved_index = node->hw_cosq_attached_to *
                        (si->port_num_ext_cosq[resolved_port] ? 18 : 2) +
                        cur_node->hw_cosq_attached_to;
                } else if (node->level == 2 &&
                           node->hw_cosq_attached_to == 0) {
                    /* MC group scheduler */
                    resolved_index = cur_node->hw_cosq_attached_to *
                        (si->port_num_ext_cosq[resolved_port] ? 18 : 2);
                } else { /* S1 or S2 scheduler */
                    resolved_index = cur_node->hw_cosq_attached_to;
                }
            } else { /* unicast queue group or multicast queue group */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            if (IS_CPU_PORT(unit, resolved_port)) {
                numq = si->num_cpu_cosq;
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                resolved_index = startq;
            } else { /* S2.0 scheduler */
                numq = _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE;
                if (startq >= numq) {
                    return BCM_E_PARAM;
                }
                /*
                 * cosq 0 - 3 (via S3.0 - S3.3) map to index 0 - 3
                 * cosq 4 - 7 map to index 8 - 11
                 */
                resolved_index = startq >= 4 ? startq + 4 : startq;
            }
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_PERQ_XMT:
        /*
         * software assigned index for cpu port:
         *     index 0 - 47
         * software assigned index for loopback port:
         *     index 0 - 8
         * software assigned index for regular port:
         *     index 0 - 9 for unicast queue 0 - 9
         *     index 10 - 14 for multicast queue 0 - 4
         * software assigned index for extended queue port:
         *     index 0 - 9 for unicast queue 0 - 9
         *     index 10 - 14 for multicast queue 0 - 4
         *     index 15 - 78 for extended unicast queue 0 - 63
         */
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    /* regular unicast queue */
                    resolved_index = node->base;
                } else { /* extended unicast queue */
                    resolved_index = si->port_num_uc_cosq[resolved_port] +
                        si->port_num_cosq[resolved_port] +
                        node->base + startq * 16;
                }
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                /* multicast queue */
                resolved_index = si->port_num_uc_cosq[resolved_port] +
                    node->base;
            } else { /* scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            numq = si->port_num_uc_cosq[resolved_port] +
                si->port_num_cosq[resolved_port] +
                si->port_num_ext_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = startq;
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (id >= _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    /* extended unicast queue */
                    return BCM_E_PARAM;
                }
                if (si->port_num_ext_cosq[resolved_port] == 0) {
                    resolved_index = node->base;
                } else {
                    resolved_index = 64 + node->base;
                }
            } else { /* multicast queue group or scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            numq = si->port_num_uc_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            if (si->port_num_ext_cosq[resolved_port] == 0) {
                resolved_index = startq;
            } else {
                resolved_index = 64 + startq;
            }
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (id >= _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    /* extended unicast queue */
                    return BCM_E_PARAM;
                }
                resolved_index = node->base;
            } else { /* multicast queue group or scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            numq = si->port_num_uc_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = startq;
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE:
        if (node != NULL) {
            if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                resolved_index = node->base;
            } else { /* unicast queue group or scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            numq = si->port_num_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = startq;
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    /* regular unicast queue */
                    return BCM_E_PARAM;
                }
                resolved_index = node->base + startq * 16;
            } else { /* multicast queue group or scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            numq = si->port_num_ext_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = startq;
        }
        break;
    case _BCM_TD_COSQ_INDEX_STYLE_EGR_POOL:
        if (node != NULL) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
                if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    /* regular unicast queue */
                    resolved_index = node->base;
                } else { /* extended unicast queue */
                    resolved_index = node->base + startq * 16;
                }
            } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
                resolved_index = node->base;
            } else { /* scheduler */
                return BCM_E_PARAM;
            }
        } else { /* node == NULL */
            numq = si->port_num_cosq[resolved_port];
            if (startq >= numq) {
                return BCM_E_PARAM;
            }
            resolved_index = startq;
        }
        break;

    case _BCM_TD_COSQ_INDEX_STYLE_UC_EGR_POOL:
        numq = si->port_num_uc_cosq[resolved_port];
        if (startq >= numq) {
                return BCM_E_PARAM;
        }
        resolved_index = startq;
        break;

    default:
        return BCM_E_INTERNAL;
    }

    if (index != NULL) {
        if (resolved_index == -1) {
            return BCM_E_NOT_FOUND;
        }
        *index = resolved_index;
    }
    if (local_port != NULL) {
        *local_port = resolved_port;
    }
    if (count != NULL) {
        *count = cosq == -1 ? numq : 1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_node_resolve
 * Purpose:
 *     Resolve queue number in the specified scheduler tree and its subtree
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number
 *     node       - (IN) node structure for the specified scheduler node
 *     cosq       - (IN) COS queue to attach to
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_cosq_node_resolve(int unit, bcm_port_t port, _bcm_td_cosq_node_t *node,
                          bcm_cos_queue_t cosq)
{
    soc_info_t *si;
    _bcm_td_cosq_node_t *s1_node, *s2_node, *q_node, *cur_node;
    uint32 map, s1_map, s2_map, mcg_map, s3_mc_map, s3_uc_map, s3_ext_uc_map;
    int base, i;

    if (node->parent == NULL) {
        /* Not attached, should never happen */
        return BCM_E_NOT_FOUND;
    }

    si = &SOC_INFO(unit);

    /* find the current cosq_attached_to usage */
    map = 0;
    for (cur_node = node->parent->child; cur_node != NULL;
         cur_node = cur_node->sibling) {
        if (cur_node->cosq_attached_to >= 0) {
            map |= 1 << cur_node->cosq_attached_to;
        }
    }

    if (cosq < 0) {
        for (i = 0; i < node->parent->numq; i++) {
            if ((map & (1 << i)) == 0) {
                node->cosq_attached_to = i;
                break;
            }
        }
        if (i == node->parent->numq) {
            return BCM_E_PARAM;
        }
    } else {
        if (map & (1 << cosq)) {
            return BCM_E_EXISTS;
        }
        node->cosq_attached_to = cosq;
    }
    node->hw_cosq_attached_to = -1;

    /* find the current S3 to S2 mapping */
    s3_mc_map = 0;
    s3_uc_map = 0;
    s3_ext_uc_map = 0;
    s2_map = 0;
    mcg_map = 0;
    s1_map = 0;
    for (s1_node = node->parent; s1_node->parent != NULL;
         s1_node = s1_node->parent);
    for (s2_node = s1_node->child; s2_node != NULL;
         s2_node = s2_node->sibling) {
        if (s2_node->hw_cosq_attached_to < 0) {
            /* S2/MC group node may not be resolved yet */
            continue;
        }
        s1_map |= 1 << s2_node->hw_cosq_attached_to;
        if (s2_node->hw_cosq_attached_to == 0) { /* MC group node */
            for (q_node = s2_node->child; q_node != NULL;
                 q_node = q_node->sibling) {
                mcg_map |= 1 << q_node->hw_cosq_attached_to;
            }
        } else if (s2_node->hw_cosq_attached_to < 4) {
            /* S2 node can be mapped to input 1, 2, or 3 of S1 node */
            for (cur_node = s2_node->child; cur_node != NULL;
                 cur_node = cur_node->sibling) {
                s2_map |= 1 << cur_node->hw_cosq_attached_to;
                if (cur_node->hw_cosq_attached_to < 4) {
                    /* S3 node can be mapped to input 0, 1, 2, or 3 of S2
                     * node */
                    for (q_node = cur_node->child; q_node != NULL;
                         q_node = q_node->sibling) {
                        if (q_node->hw_cosq_attached_to < 0) {
                            continue;
                        } else if (q_node->hw_cosq_attached_to == 0) {
                            /* multicast queue is always mapped to input 0 of
                             * S3 node */
                            s3_mc_map |= 1 << cur_node->hw_cosq_attached_to;
                        } else if (q_node->hw_cosq_attached_to == 1) {
                            /* unicast queue is always mapped to input 1 of
                             * S3 node */
                            s3_uc_map |= 1 << cur_node->hw_cosq_attached_to;
                        } else {
                            /* extended unicast queue can be mapped to input 2
                             * to 17 of S3 node */
                            s3_ext_uc_map |=
                                1 << (q_node->hw_cosq_attached_to - 2);
                        }
                    }
                }
            }
        }
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
        if (BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(node->gport) <
            _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
            /* regular ucast queue */
            if (node->parent->level == 1) {
                if ((s1_map & 0x60) == 0x60) {
                    /* both SC queue and QM queue have been assigned */
                    return BCM_E_FAIL;
                }
            } else if (node->parent->level == 2) {
                if (node->parent->hw_cosq_attached_to == 0) {
                    /* ucast queue can not attach to MC group */
                    return BCM_E_FAIL;
                }
                if (((s2_map & 0xf00) == 0xf00) && s3_uc_map != 0) {
                    /* ucast queue 4-7 have already been assigned,
                     * but one or more of ucast queue 0-3 has attached to S3 */
                    return BCM_E_FAIL;
                }
                if ((cosq < 4) && s3_uc_map != 0) {
                    /* Try to attach ucast queue 0-3 to S2,
                     * but one or more of ucast queue 0-3 has attached to S3 */
                    return BCM_E_FAIL;
                }
            } else if (node->parent->level == 3) {
                if ((s2_map & 0x0f0) != 0) {
                /* One or more of ucast queue 0-3 has attached to S2 */
                    return BCM_E_FAIL;
                }
            } else {
                return BCM_E_FAIL;
            }
        } else {
            /* extended ucast queue */
            if (node->parent->level != 3) {
                return BCM_E_FAIL;
            }
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
        if (node->parent->level == 1) {
            if ((s1_map & 0x10) == 0x10) {
                /* multicast SC/QM queue has been assigned */
                return BCM_E_FAIL;
            }
        } else if (node->parent->level == 2) {
            if (node->parent->hw_cosq_attached_to >= 1 &&
                node->parent->hw_cosq_attached_to <= 3) {
                /* mcast queue can not attach to S2 */
                return BCM_E_FAIL;
            }
            if (s3_mc_map != 0) {
                /* Some mcast queue has attached to S3 */
                return BCM_E_FAIL;
            }
        } else if (node->parent->level == 3) {
            if (mcg_map != 0) {
                /* Some mcast queue has attached to MC group */
                return BCM_E_FAIL;
            }
        } else {
            return BCM_E_FAIL;
        }
    } else if (BCM_GPORT_IS_SCHEDULER(node->gport)) {
        if (node->parent->level == 1) {
            if ((s1_map & 0x00f) == 0x00f) {
                /* There are only 3 S2 schedulers and 1 MC group scheduler */
                return BCM_E_FAIL;
            }
            if (node->numq > _BCM_TD_S2_SCHEDULER_NUM_INPUT) {
                /* hardware allows up to 9 input */
                return BCM_E_FAIL;
            }
        } else if (node->parent->level == 2) {
            if ((s2_map & 0x00f) == 0x00f) {
                /* There are only 4 S3 schedulers */
                return BCM_E_FAIL;
            }
            if (si->port_num_ext_cosq[port] == 0) { /* regular port */
                if (node->numq > _BCM_TD_S3_SCHEDULER_NUM_INPUT) {
                    /* possible input: 1 UC, 1 MC */
                    return BCM_E_FAIL;
                }
            } else { /* extended queue port */
                if (node->numq > _BCM_TD_EXT_S3_SCHEDULER_NUM_INPUT) {
                    /* possible input: 1 UC, 1 MC, 16 EXT UC */
                    return BCM_E_FAIL;
                }
            }
        } else {
            return BCM_E_FAIL;
        }
    } else {
        return BCM_E_INTERNAL;
    }

    /* Resolve S2/MC group node if needed */
    if (node->parent->level == 2 && node->parent->hw_cosq_attached_to < 0) {
        s2_node = node->parent;
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
            /* MC group node is always mapped to input 0 of S1 node */
            s2_node->hw_cosq_attached_to = 0;
            s2_node->base = 0;
        } else {
            /* S2 node can be mapped to input 1, 2, or 3 of S1 node */
            for (i = 1; i < 4; i++) {
                if ((s1_map & (1 << i)) == 0) {
                    s2_node->hw_cosq_attached_to = i;
                    s2_node->base = 0;
                    break;
                }
            }
            if (s2_node->hw_cosq_attached_to < 0) {
                return BCM_E_FAIL;
            }
        }
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(node->gport)) {
        if (BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(node->gport) <
            _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
            /* regular unicast queue */
            if (node->parent->level == 1) {
                if (!(s1_map & 0x20)) { /* unicast SC queue */
                    node->hw_cosq_attached_to = 5;
                    node->base = 8;
                } else { /* unicast QM queue */
                    node->hw_cosq_attached_to = 6;
                    node->base = 9;
                }
            } else if (node->parent->level == 2) {
                /* If regular unicast queue is attached to S2 scheduler,
                 * queue 0 - 3 can be mapped to input 4 - 7 of S2 node,
                 * queue 4 - 7 are fixed mapped to input 8 - 11 of S2 node */
                base = (cosq < 4) ? 4 : 8;
                for (i = 0; i < 4; i++) {
                    if ((s2_map & (1 << (base + i))) == 0) {
                        node->hw_cosq_attached_to = base + i;
                        node->base = node->hw_cosq_attached_to - 4;
                        break;
                    }
                }
            } else if (node->parent->level == 3) {
                /* If regular unicast queue is attached to S3 scheduler,
                 * queue 0 - 3 are fixed mapped to input 1 of S3 node 0 - 3,
                 * S3 node 0 - 3 are mapped to input 0 - 3 of each S2 node */
                node->hw_cosq_attached_to = 1;
                node->base = node->parent->hw_cosq_attached_to;
            } else {
                return BCM_E_FAIL;
            }
        } else { /* extended unicast queue */
            if (node->parent->level == 3) {
                /* queue 0 - 15 are fixed mapped to input 2 - 17 of S3 node 0,
                 * queue 16 - 31 are fixed mapped to input 2 - 17 of S3 node 1,
                 * queue 32 - 47 are fixed mapped to input 2 - 17 of S3 node 2,
                 * queue 48 - 63 are fixed mapped to input 2 - 17 of S3 node 3,
                 * S3 node 0 - 3 are mapped to input 0 - 3 of each S2 node */
                for (i = 0; i < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP; i++) {
                    if ((s3_ext_uc_map & (1 << i)) == 0) {
                        node->hw_cosq_attached_to = 2 + i;
                        node->base = i;
                        s3_ext_uc_map |= 1 << i;
                        break;
                    }
                }
            }
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(node->gport)) {
        if (node->parent->level == 1) { /* multicast SC/QM queue */
            node->hw_cosq_attached_to = 4;
            node->base = 4;
        } else if (node->parent->level == 2) {
            /* If multicast queue is attached to MC group scheduler,
             * queue 0 - 3 are mapped to input 0 - 3 of MC group node */
            for (i = 0; i < _BCM_TD_NUM_MCAST_QUEUE_GROUP; i++) {
                if ((mcg_map & (1 << i)) == 0) {
                    node->hw_cosq_attached_to = i;
                    node->base = i;
                    break;
                }
            }
        } else if (node->parent->level == 3) {
            /* If multicast queue is attached to S3 scheduler,
             * queue 0 - 3 are fixed mapped to input 0 of S3 node 0 - 3,
             * S3 node 0 - 3 are mapped to input 0 - 3 of each S2 node */
            node->hw_cosq_attached_to = 0;
            node->base = node->parent->hw_cosq_attached_to;
        }
    } else if (BCM_GPORT_IS_SCHEDULER(node->gport)) {
        if (node->parent->level == 1) {
            /* We can not foresee if this node is S2.x or MC group yet */
        } else if (node->parent->level == 2 ) {
            for (i = 0; i < _BCM_TD_NUM_S3_SCHEDULER; i++) {
                /* S3 node can be mapped to input 0, 1, 2, or 3 of S2 node */
                if ((s2_map & (1 << i)) == 0) {
                    node->hw_cosq_attached_to = i;
                    node->base = 0;
                    break;
                }
            }
        }
        node->level = node->parent->level + 1;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_sched_node_set(int unit, bcm_port_t port)
{
    uint64 rval64[3];
    uint32 rval;
    int uc_to_s2, use_mc_group, index;
    _bcm_td_cosq_port_info_t *port_info;
    _bcm_td_cosq_node_t *s1_node, *s2_node, *cur_node;
    int i;
    static const soc_field_t fields[] = {
        S3_GROUP_NO_I0f, S3_GROUP_NO_I1f, S3_GROUP_NO_I2f, S3_GROUP_NO_I3f,
        S3_GROUP_NO_I4f, S3_GROUP_NO_I5f, S3_GROUP_NO_I6f, S3_GROUP_NO_I7f,
        S3_GROUP_NO_I8f
    };

    COMPILER_64_ZERO(rval64[0]);
    for (index = 0; index < 9; index++) {
        soc_reg64_field32_set(unit, S2_S3_ROUTINGr, &rval64[0], fields[index],
                              0x1f);
    }
    rval64[1] = rval64[0];
    rval64[2] = rval64[0];

    port_info = &_bcm_td_cosq_port_info[unit][port];
    s1_node = NULL;
    for (i = 0; i < _BCM_TD_NUM_SCHEDULER; i++) {
        if (port_info->sched[i].level == 1) {
            s1_node = &port_info->sched[i];
            break;
        }
    }
    if (s1_node != NULL) {
        use_mc_group = 0;
        uc_to_s2 = 0;
        for (s2_node = s1_node->child; s2_node != NULL;
             s2_node = s2_node->sibling) {
            if (s2_node->hw_cosq_attached_to < 0) {
                continue;
            } else if (s2_node->hw_cosq_attached_to == 0) { /* MC group */
                if (s2_node->child != NULL) {
                    use_mc_group = 1;
                }
            } else if (s2_node->hw_cosq_attached_to < 4) { /* S2 */
                index = s2_node->hw_cosq_attached_to - 1;
                for (cur_node = s2_node->child; cur_node != NULL;
                     cur_node = cur_node->sibling) {
                    soc_reg64_field32_set(unit, S2_S3_ROUTINGr, &rval64[index],
                                          fields[cur_node->cosq_attached_to],
                                          cur_node->hw_cosq_attached_to);
                    if (cur_node->hw_cosq_attached_to >= 4 &&
                        cur_node->hw_cosq_attached_to < 8) {
                        /* at least one of unicast queue 0 - 3 is directly
                         * attached to S2 */
                        uc_to_s2 = 1;
                    }
                }
            }
        }

        BCM_IF_ERROR_RETURN(READ_S3_CONFIGr(unit, port, &rval));
        soc_reg_field_set(unit, S3_CONFIGr, &rval, ROUTE_UC_TO_S2f, uc_to_s2);
        BCM_IF_ERROR_RETURN(WRITE_S3_CONFIGr(unit, port, rval));

        BCM_IF_ERROR_RETURN(READ_S3_CONFIG_MCr(unit, port, &rval));
        soc_reg_field_set(unit, S3_CONFIG_MCr, &rval, USE_MC_GROUPf,
                          use_mc_group);
        BCM_IF_ERROR_RETURN(WRITE_S3_CONFIG_MCr(unit, port, rval));
    } else {
        /* make it act like a non-ETS style scheduler
         * unicast queue 0 and multicast queue 0 connect to S3.0
         * unicast queue 1 and multicast queue 1 connect to S3.1
         * unicast queue 2 and multicast queue 2 connect to S3.2
         * unicast queue 3 and multicast queue 3 connect to S3.3
         * S3.0 - S3.3 and unicast queue 4 - 7 connect to S2.0
         * S2.1, S2.2, MC group are not used
         */
        /* S3.0 - S3.3 to S2.0 input 0 - 3 (S3 group number 0 - 3)*/
        for (i = 0; i < _BCM_TD_NUM_S3_SCHEDULER; i++) {
            soc_reg64_field32_set(unit, S2_S3_ROUTINGr, &rval64[0], fields[i],
                                  i);
        }
        /* unicast queues 4 - 7 to S2.0 input 4 - 7 (S3 group number 8 - 11) */
        for (i = _BCM_TD_NUM_S3_SCHEDULER;
             i < _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE; i++) {
            soc_reg64_field32_set(unit, S2_S3_ROUTINGr, &rval64[0], fields[i],
                                  i + 4);
        }

        rval = 0;
        soc_reg_field_set(unit, S3_CONFIGr, &rval, SCHEDULING_SELECTf, 0x55);
        BCM_IF_ERROR_RETURN(WRITE_S3_CONFIGr(unit, port, rval));

        BCM_IF_ERROR_RETURN(WRITE_S3_CONFIG_MCr(unit, port, 0));

        rval = 0;
        soc_reg_field_set(unit, S2_CONFIGr, &rval, SCHEDULING_SELECTf, 0x15);
        BCM_IF_ERROR_RETURN(WRITE_S2_CONFIGr(unit, port, rval));
    }

    for (index = 0; index < _BCM_TD_NUM_S2_SCHEDULER; index++) {
        BCM_IF_ERROR_RETURN
            (WRITE_S2_S3_ROUTINGr(unit, port, index, rval64[index]));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_node_unresolve
 * Purpose:
 *     Unresolve queue number in the specified scheduler tree and its subtree
 * Parameters:
 *     unit       - (IN) unit number
 *     node       - (IN) node structure for the specified scheduler node
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_cosq_node_unresolve(int unit, _bcm_td_cosq_node_t *node)
{
    if (node->cosq_attached_to < 0) {
        /* Has been unresolved already */
        return BCM_E_NONE;
    }

    if (node->child != NULL) {
        _bcm_td_cosq_node_unresolve(unit, node->child);
    }

    if (node->sibling != NULL && node->sibling->cosq_attached_to >= 0) {
        _bcm_td_cosq_node_unresolve(unit, node->sibling);
    }

    node->cosq_attached_to = -1;
    node->hw_cosq_attached_to = -1;
    node->level = 0;
    node->parent = NULL;
    node->sibling = NULL;
    node->child = NULL;

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_config_set(int unit, int numq)
{
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1], *hg_entries[1];
    uint32 index, hg_index;
    int count, hg_count;
    bcm_port_t port;
    int cos, prio;
    uint32 i;
#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    port_cos_map_entry_t hg_cos_map_entries[16];
#endif

    if (numq < 1 || numq > 8) {
        return BCM_E_PARAM;
    }

    /* Distribute first 8 internal priority levels into the specified number
     * of cosq, map remaining internal priority levels to highest priority
     * cosq */
    sal_memset(cos_map_entries, 0, sizeof(cos_map_entries));
    entries[0] = &cos_map_entries;
    hg_entries[0] = &cos_map_entries;
    prio = 0;
    for (cos = 0; cos < numq; cos++) {
        for (i = 8 / numq + (cos < 8 % numq ? 1 : 0); i > 0; i--) {
            soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio],
                                COSf, cos);
            prio++;
        }
    }
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[prio], COSf,
                            numq - 1);
    }

    /* Map internal priority levels to CPU CoS queues */
    BCM_IF_ERROR_RETURN(_bcm_esw_cpu_cosq_mapping_default_set(unit, numq));

#ifdef BCM_COSQ_HIGIG_MAP_DISABLE
    /* Use identical mapping for Higig port */
    sal_memset(hg_cos_map_entries, 0, sizeof(hg_cos_map_entries));
    hg_entries[0] = &hg_cos_map_entries;
    prio = 0;
    for (prio = 0; prio < 8; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            COSf, prio);
    }
    for (prio = 8; prio < 16; prio++) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &hg_cos_map_entries[prio],
                            COSf, 7);
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_td_cos_map_profile[unit], entries, 16,
                             &index));
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_td_cos_map_profile[unit], hg_entries,
                             16, &hg_index));
    count = 0;
    hg_count = 0;
    PBMP_ALL_ITER(unit, port) {
        if (IS_HG_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                        hg_index / 16));
            hg_count++;
        } else {
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, COS_MAP_SELm, port, SELECTf,
                                        index / 16));
            count++;
        }
    }
    if (SOC_INFO(unit).cpu_hg_index != -1) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                    SOC_INFO(unit).cpu_hg_index, SELECTf,
                                    hg_index / 16));
        hg_count++;
    }
    for (i = 1; i < count; i++) {
        soc_profile_mem_reference(unit, _bcm_td_cos_map_profile[unit], index,
                                  0);
    }
    for (i = 1; i < hg_count; i++) {
        soc_profile_mem_reference(unit, _bcm_td_cos_map_profile[unit],
                                  hg_index, 0);
    }

    _bcm_td_num_cosq[unit] = numq;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_mapping_set
 * Purpose:
 *     Set COS queue for the specified priority of an ingress port
 * Parameters:
 *     unit     - (IN) unit number
 *     ing_port - (IN) ingress port
 *     gport    - (IN) queue group GPORT identifier
 *     priority - (IN) priority value to map
 *     flags    - (IN) BCM_COSQ_GPORT_XXX_QUEUE_GROUP
 *     gport    - (IN) queue group GPORT identifier
 *     cosq     - (IN) COS queue number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td_cosq_mapping_set(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    bcm_port_t local_port;
    bcm_cos_queue_t hw_cosq, mc_hw_cosq = 0;
    soc_field_t field, field2 = INVALIDf;
    cos_map_sel_entry_t cos_map_sel_entry;
    port_cos_map_entry_t cos_map_entries[16];
    void *entries[1];
    uint32 old_index, new_index;
    int rv;

    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_localport_resolve(unit, ing_port, &local_port));

    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        if (gport == -1) {
	    hw_cosq = cosq;
	} else {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, gport, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &hw_cosq, NULL));
        }
        field = UC_COS1f;
        break;
    case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
        if (gport == -1) {
	    hw_cosq = cosq;
	} else {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve
             (unit, gport, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
              NULL, &hw_cosq, NULL));
        }
        field = MC_COS1f;
        break;
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
        if (gport == -1) {
            hw_cosq = (cosq >> 16) & 0xffff;
            mc_hw_cosq = cosq & 0xffff;
        } else {
            return BCM_E_PARAM;
        }
        field = UC_COS1f;
        field2 = MC_COS1f;
        break;    
    case BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP:
        if (gport == -1) {
	    return BCM_E_UNAVAIL; 
	} else {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve
             (unit, gport, cosq, _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
              NULL, &hw_cosq, NULL));
        }
        field = HG_COSf;
        break;
    default:
        return BCM_E_PARAM;
    }

    /* There are only 5 MC QUEUES [0,4] attached to a port. */
    if (flags == BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) {
        if (hw_cosq > (_BCM_TD_NUM_MCAST_QUEUE_GROUP - 1)) {
            return BCM_E_PARAM;
        }
    }

    entries[0] = &cos_map_entries;

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                           &cos_map_sel_entry));
    old_index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                    SELECTf);
    old_index *= 16;

    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_td_cos_map_profile[unit],
                             old_index, 16, entries));
    soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority], field,
                        hw_cosq);
    if (field2 != INVALIDf) {
        soc_mem_field32_set(unit, PORT_COS_MAPm, &cos_map_entries[priority],
                            field2,mc_hw_cosq);
    }
    
    soc_mem_lock(unit, PORT_COS_MAPm);

    rv = soc_profile_mem_delete(unit, _bcm_td_cos_map_profile[unit],
                                old_index);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    if (BCM_SUCCESS(rv) && IS_CPU_PORT(unit, local_port)) {
        rv = soc_profile_mem_delete(unit, _bcm_td_cos_map_profile[unit],
                                    old_index);
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, PORT_COS_MAPm);
        return rv;
    }

    rv = soc_profile_mem_add(unit, _bcm_td_cos_map_profile[unit], entries,
                             16, &new_index);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    if (BCM_SUCCESS(rv) && IS_CPU_PORT(unit, local_port)) {
        rv = soc_profile_mem_reference(unit, _bcm_td_cos_map_profile[unit],
                                       new_index, 16);
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

    if (rv == SOC_E_RESOURCE) {
        (void)soc_profile_mem_reference(unit, _bcm_td_cos_map_profile[unit],
                                        old_index, 16);
#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
        if (IS_CPU_PORT(unit, local_port)) {
            (void)soc_profile_mem_reference(unit,
                                            _bcm_td_cos_map_profile[unit],
                                            old_index, 16);
        }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */
    }

    soc_mem_unlock(unit, PORT_COS_MAPm);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    soc_mem_field32_set(unit, COS_MAP_SELm, &cos_map_sel_entry, SELECTf,
                        new_index / 16);
    BCM_IF_ERROR_RETURN
        (WRITE_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                            &cos_map_sel_entry));

#ifndef BCM_COSQ_HIGIG_MAP_DISABLE
    if (IS_CPU_PORT(unit, local_port)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, COS_MAP_SELm,
                                    SOC_INFO(unit).cpu_hg_index, SELECTf,
                                    new_index / 16));
    }
#endif /* BCM_COSQ_HIGIG_MAP_DISABLE */

    if (flags == BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_voq_gport_mapping_set(unit, gport, priority, cosq));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_mapping_get
 * Purpose:
 *     Get COS queue for the specified priority of an ingress port
 * Parameters:
 *     unit     - (IN) unit number
 *     ing_port - (IN) ingress port
 *     gport    - (IN) queue group GPORT identifier
 *     priority - (IN) priority value to map
 *     flags    - (IN) BCM_COSQ_GPORT_XXX_QUEUE_GROUP
 *     gport    - (OUT) queue group GPORT identifier
 *     cosq     - (OUT) COS queue number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td_cosq_mapping_get(int unit, bcm_port_t ing_port, bcm_cos_t priority,
                         uint32 flags, bcm_gport_t *gport,
                         bcm_cos_queue_t *cosq)
{
    _bcm_td_cosq_port_info_t *port_info;
    _bcm_td_cosq_node_t *node;
    bcm_port_t local_port;
    bcm_cos_queue_t hw_cosq;
    int index, i;
    cos_map_sel_entry_t cos_map_sel_entry;
    void *entry_p;

    if (priority < 0 || priority >= 16) {
        return BCM_E_PARAM;
    }

    if (flags != BCM_COSQ_GPORT_UCAST_QUEUE_GROUP &&
        flags != BCM_COSQ_GPORT_MCAST_QUEUE_GROUP &&
        flags != BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_localport_resolve(unit, ing_port, &local_port));

    BCM_IF_ERROR_RETURN
        (READ_COS_MAP_SELm(unit, MEM_BLOCK_ANY, local_port,
                           &cos_map_sel_entry));
    index = soc_mem_field32_get(unit, COS_MAP_SELm, &cos_map_sel_entry,
                                SELECTf);
    index *= 16;

    entry_p = SOC_PROFILE_MEM_ENTRY(unit, _bcm_td_cos_map_profile[unit],
                                    port_cos_map_entry_t *,
                                    index + priority);

    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, UC_COS1f);
        if (IS_CPU_PORT(unit, local_port) || IS_LB_PORT(unit, local_port) ||
            !gport) {
            if (gport) {
                *gport = BCM_GPORT_INVALID;
            }
            *cosq = hw_cosq;
        } else {
            port_info = &_bcm_td_cosq_port_info[unit][local_port];
            for (i = 0; i < _BCM_TD_NUM_UCAST_QUEUE_GROUP; i++) {
                node = &port_info->ucast[i];
                if (node->numq > 0 && node->base == hw_cosq) {
                    *gport = node->gport;
                    *cosq = 0;
                    break;
                }
            }
            if (i == _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                return BCM_E_NOT_FOUND;
            }
        }
        break;
    case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
        hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, MC_COS1f);
        if (IS_CPU_PORT(unit, local_port) || IS_LB_PORT(unit, local_port) ||
            !gport) {
            if (gport) {
                *gport = BCM_GPORT_INVALID;
            }
            *cosq = hw_cosq;
        } else {
            port_info = &_bcm_td_cosq_port_info[unit][local_port];
            for (i = 0; i < _BCM_TD_NUM_MCAST_QUEUE_GROUP; i++) {
                node = &port_info->mcast[i];
                if (node->numq > 0 && node->base == hw_cosq) {
                    *gport = node->gport;
                    *cosq = 0;
                    break;
                }
            }
            if (i == _BCM_TD_NUM_MCAST_QUEUE_GROUP) {
                return BCM_E_NOT_FOUND;
            }
        }
        break;
    case BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP:
        hw_cosq = soc_mem_field32_get(unit, PORT_COS_MAPm, entry_p, HG_COSf);
        if (IS_CPU_PORT(unit, local_port) || IS_LB_PORT(unit, local_port) ||
            !gport) {
            if (gport) {
                *gport = BCM_GPORT_INVALID;
            }
            *cosq = hw_cosq;
        } else {
            port_info = &_bcm_td_cosq_port_info[unit][local_port];
            for (i = 0; i < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP; i++) {
                node = &port_info->ext_ucast[i];
                if (node->numq > 0 && node->base == hw_cosq % 16) {
                    *gport = node->gport;
                    *cosq = hw_cosq / 16;
                    break;
                }
            }
            if (i == _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP) {
                return BCM_E_NOT_FOUND;
            }
        }
        break;
    }

    return BCM_E_NONE;
}

/*
 *  Convert HW drop probability to percent value
 */
STATIC int
_bcm_td_hw_drop_prob_to_percent[] = {
    0,     /* 0  */
    1,     /* 1  */
    2,     /* 2  */
    3,     /* 3  */
    4,     /* 4  */
    5,     /* 5  */
    6,     /* 6  */
    7,     /* 7  */
    8,     /* 8  */
    9,     /* 9  */
    10,    /* 10 */
    25,    /* 11 */
    50,    /* 12 */
    75,    /* 13 */
    100,   /* 14 */
    -1     /* 15 */
};

STATIC int
_bcm_td_percent_to_drop_prob(int percent)
{
   int i;

   for (i = 14; i > 0 ; i--) {
      if (percent >= _bcm_td_hw_drop_prob_to_percent[i]) {
          break;
      }
   }
   return i;
}

STATIC int
_bcm_td_drop_prob_to_percent(int drop_prob) {
   return (_bcm_td_hw_drop_prob_to_percent[drop_prob]);
}

/*
 * index: degree, value: contangent(degree) * 100
 * max value is 0xffff (16-bit) at 0 degree
 */
STATIC int
_bcm_td_cotangent_lookup_table[] =
{
    /*  0.. 5 */  65535, 5728, 2863, 1908, 1430, 1143,
    /*  6..11 */    951,  814,  711,  631,  567,  514,
    /* 12..17 */    470,  433,  401,  373,  348,  327,
    /* 18..23 */    307,  290,  274,  260,  247,  235,
    /* 24..29 */    224,  214,  205,  196,  188,  180,
    /* 30..35 */    173,  166,  160,  153,  148,  142,
    /* 36..41 */    137,  132,  127,  123,  119,  115,
    /* 42..47 */    111,  107,  103,  100,   96,   93,
    /* 48..53 */     90,   86,   83,   80,   78,   75,
    /* 54..59 */     72,   70,   67,   64,   62,   60,
    /* 60..65 */     57,   55,   53,   50,   48,   46,
    /* 66..71 */     44,   42,   40,   38,   36,   34,
    /* 72..77 */     32,   30,   28,   26,   24,   23,
    /* 78..83 */     21,   19,   17,   15,   14,   12,
    /* 84..89 */     10,    8,    6,    5,    3,    1,
    /* 90     */      0
};

/*
 * Given a slope (angle in degrees) from 0 to 90, return the number of cells
 * in the range from 0% drop probability to 100% drop probability.
 */
STATIC int
_bcm_td_angle_to_cells(int angle)
{
    return (_bcm_td_cotangent_lookup_table[angle]);
}

/*
 * Given a number of packets in the range from 0% drop probability
 * to 100% drop probability, return the slope (angle in degrees).
 */
STATIC int
_bcm_td_cells_to_angle(int packets)
{
    int angle;

    for (angle = 90; angle >= 0 ; angle--) {
        if (packets <= _bcm_td_cotangent_lookup_table[angle]) {
            break;
        }
    }
    return angle;
}

STATIC int
_bcm_td_cosq_refresh_freeze(int unit)
{
    uint32 reg;
    int rv = BCM_E_NONE;

    if (soc_feature(unit, soc_feature_bucket_update_freeze)) {
        TD_COSQ_LOCK(unit);

        rv = READ_MISCCONFIGr(unit, &reg);
        if (BCM_FAILURE(rv)) {
            TD_COSQ_UNLOCK(unit);
            return rv;
        }
        soc_reg_field_set(unit, MISCCONFIGr, &reg, REFRESH_ENf, 0);
        rv = WRITE_MISCCONFIGr(unit, reg);
        if (BCM_FAILURE(rv)) {
            TD_COSQ_UNLOCK(unit);
            return rv;
        }
    }

    return rv;
}

STATIC int
_bcm_td_cosq_refresh_thaw(int unit)
{
    uint32 reg;
    int rv = BCM_E_NONE;

    if (soc_feature(unit, soc_feature_bucket_update_freeze)) {
        rv = READ_MISCCONFIGr(unit, &reg);
        if (BCM_FAILURE(rv)) {
            TD_COSQ_UNLOCK(unit);
            return rv;
        }
        soc_reg_field_set(unit, MISCCONFIGr, &reg, REFRESH_ENf, 1);
        rv = WRITE_MISCCONFIGr(unit, reg);
        if (BCM_FAILURE(rv)) {
            TD_COSQ_UNLOCK(unit);
            return rv;
        }

        TD_COSQ_UNLOCK(unit);
    }

    return rv;
}

/*
 * Function:
 *     _bcm_td_cosq_bucket_set
 * Purpose:
 *     Configure COS queue bandwidth control bucket
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_bandwidth - (IN) min bucket bandwidth (kbps or packet/second)
 *     max_bandwidth - (IN) max bucket bandwidth (kbps or packet/second)
 *     min_burst     - (IN) min bucket burst size (kbits or packets)
 *     max_burst     - (IN) max bucket burst size (kbits or packets)
 *     flags         - (IN) BCM_COSQ_BW_PACKET_MODE_XXX
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_td_cosq_bucket_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                        uint32 min_bandwidth, uint32 max_bandwidth,
                        uint32 min_burst, uint32 max_burst,
                        uint32 flags)
{
    soc_info_t *si;
    _bcm_td_cosq_node_t *node;
    bcm_port_t local_port, mmu_port, mmu_cmic_port, mmu_lb_port;
    int rv, index;
    soc_reg_t max_config_reg, max_reg, min_config_reg, min_reg, shaping_reg;
    uint32 rval, fval;
    uint32 orig_bandwidth, orig_burst;
    uint64 rval64, orig_rval64, bucket_rval64, mode_rval64;
    uint32 refresh_rate, bucketsize, granularity, meter_flags;
    int refresh_bitsize, bucket_bitsize;
    static const soc_reg_t ext_shaping_reg_x[] = {
        EXT1_SHAPING_CONTROL_PORT1r, EXT1_SHAPING_CONTROL_PORT2r,
        EXT1_SHAPING_CONTROL_PORT3r, EXT1_SHAPING_CONTROL_PORT4r
    };
    static const soc_reg_t ext_shaping_reg_y[] = {
        EXT1_SHAPING_CONTROL_PORT34r, EXT1_SHAPING_CONTROL_PORT35r,
        EXT1_SHAPING_CONTROL_PORT36r, EXT1_SHAPING_CONTROL_PORT37r
    };

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_BUCKET,
                                    &local_port, &index, NULL));

    if (BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, port, NULL, NULL, NULL, &node));
        if (node->level == 3) { /* S3 scheduler */
            max_config_reg = S3_MAXBUCKETCONFIG_64r;
            max_reg = S3_MAXBUCKETr;
            min_config_reg = S3_MINBUCKETCONFIG_64r;
            min_reg = S3_MINBUCKETr;
        } else if (node->level == 2) { /* S2 scheduler */
            max_config_reg = S2_MAXBUCKETCONFIG_64r;
            max_reg = S2_MAXBUCKETr;
            min_config_reg = S2_MINBUCKETCONFIG_64r;
            min_reg = S2_MINBUCKETr;
        } else {
            return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
               BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        max_config_reg = MAXBUCKETCONFIG_64r;
        max_reg = MAXBUCKETr;
        min_config_reg = MINBUCKETCONFIG_64r;
        min_reg = MINBUCKETr;
    } else { /* local port */
        if (cosq < _BCM_TD_NUM_S3_SCHEDULER &&
            !IS_CPU_PORT(unit, local_port)) {
            /* use S3.0 - S3.3 for unicast and multicast queue 0 - 3 */
            max_config_reg = S3_MAXBUCKETCONFIG_64r;
            max_reg = S3_MAXBUCKETr;
            min_config_reg = S3_MINBUCKETCONFIG_64r;
            min_reg = S3_MINBUCKETr;
        } else { /* unicast queue 4 - 7 */
            max_config_reg = MAXBUCKETCONFIG_64r;
            max_reg = MAXBUCKETr;
            min_config_reg = MINBUCKETCONFIG_64r;
            min_reg = MINBUCKETr;
        }
    }

    meter_flags = flags & BCM_COSQ_BW_PACKET_MODE ?
        _BCM_TD_METER_FLAG_PACKET_MODE : 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    /* Retrieve MAXBUCKETCONFIG_64 and max bucket size */
    BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_freeze(unit));
    BCM_IF_ERROR_RETURN
        (soc_reg_get(unit, max_config_reg, local_port, index, &orig_rval64));
    BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_thaw(unit));
    granularity =
        soc_reg64_field32_get(unit, max_config_reg, orig_rval64, METER_GRANf);
    refresh_rate =
        soc_reg64_field32_get(unit, max_config_reg, orig_rval64, MAX_REFRESHf);
    bucketsize =
        soc_reg64_field32_get(unit, max_config_reg, orig_rval64, MAX_THDf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                         granularity, meter_flags,
                                         &orig_bandwidth, &orig_burst));

    /* Construct MAXBUCKETCONFIG_64 */
    refresh_bitsize = soc_reg_field_length(unit, max_config_reg, MAX_REFRESHf);
    bucket_bitsize = soc_reg_field_length(unit, max_config_reg, MAX_THDf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, max_bandwidth, max_burst,
                                         meter_flags, refresh_bitsize,
                                         bucket_bitsize, &refresh_rate,
                                         &bucketsize, &granularity));
    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set(unit, max_config_reg, &rval64, METER_GRANf,
                          granularity);
    soc_reg64_field32_set(unit, max_config_reg, &rval64, MAX_REFRESHf,
                          refresh_rate);
    soc_reg64_field32_set(unit, max_config_reg, &rval64, MAX_THDf, bucketsize);

    if (COMPILER_64_NE(rval64, orig_rval64)) { /* value changed */
        if (orig_burst != max_burst) {
            /* Construct MAXBUCKET, pre-fill it to the current bucket size */
            COMPILER_64_ZERO(bucket_rval64);
            if (meter_flags & _BCM_TD_METER_FLAG_NON_LINEAR) {
                fval = ((bucketsize & 0xff) + 256) << ((bucketsize >> 8) + 8);
            } else {
                fval = bucketsize << 16;
            }
            soc_reg64_field32_set(unit, max_reg, &bucket_rval64, MAX_BUCKETf,
                                  fval);

            BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_freeze(unit));
            /* Write MAXBUCKETCONFIG */
            rv = soc_reg_set(unit, max_config_reg, local_port, index, rval64);
            if (BCM_SUCCESS(rv)) {
                /* Write MAXBUCKET */
                rv =soc_reg_set(unit, max_reg, local_port, index,
                                bucket_rval64);
            }
            BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_thaw(unit));
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        } else {
            /* Write the constructed value to hardware */
            BCM_IF_ERROR_RETURN(soc_reg_set(unit, max_config_reg, local_port,
                                            index, rval64));
        }
    }

    /* Retrieve MINBUCKETCONFIG_64 and min bucket size */
    BCM_IF_ERROR_RETURN
        (soc_reg_get(unit, min_config_reg, local_port, index, &orig_rval64));
    granularity =
        soc_reg64_field32_get(unit, min_config_reg, orig_rval64, METER_GRANf);
    refresh_rate =
        soc_reg64_field32_get(unit, min_config_reg, orig_rval64, MIN_REFRESHf);
    bucketsize =
        soc_reg64_field32_get(unit, min_config_reg, orig_rval64, MIN_THDf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                         granularity, meter_flags,
                                         &orig_bandwidth, &orig_burst));

    /* Construct MINBUCKETCONFIG_64 */
    refresh_bitsize = soc_reg_field_length(unit, min_config_reg, MIN_REFRESHf);
    bucket_bitsize = soc_reg_field_length(unit, min_config_reg, MIN_THDf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_rate_to_bucket_encoding(unit, min_bandwidth, min_burst,
                                         meter_flags, refresh_bitsize,
                                         bucket_bitsize, &refresh_rate,
                                         &bucketsize, &granularity));
    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set(unit, min_config_reg, &rval64, METER_GRANf,
                          granularity);
    soc_reg64_field32_set(unit, min_config_reg, &rval64, MIN_REFRESHf,
                          refresh_rate);
    soc_reg64_field32_set(unit, min_config_reg, &rval64, MIN_THDf, bucketsize);

    if (COMPILER_64_NE(rval64, orig_rval64)) { /* value changed */
        if (orig_burst != min_burst) {
            /* Construct MINBUCKET, pre-fill it to the current bucket size */
            COMPILER_64_ZERO(bucket_rval64);
            if (meter_flags & _BCM_TD_METER_FLAG_NON_LINEAR) {
                fval = ((bucketsize & 0xff) + 256) << ((bucketsize >> 8) + 8);
            } else {
                fval = bucketsize << 16;
            }
            soc_reg64_field32_set(unit, min_reg, &bucket_rval64, MIN_BUCKETf,
                                  fval);

            BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_freeze(unit));
            /* Write MINBUCKETCONFIG */
            rv = soc_reg_set(unit, min_config_reg, local_port, index, rval64);
            if (BCM_SUCCESS(rv)) {
                /* Write MINBUCKET */
                rv =soc_reg_set(unit, min_reg, local_port, index,
                                bucket_rval64);
            }
            BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_thaw(unit));
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        } else {
            /* Write the constructed value to hardware */
            BCM_IF_ERROR_RETURN(soc_reg_set(unit, min_config_reg, local_port,
                                            index, rval64));
        }
    }

    /* resolve index into SHAPING_CONTROL register */
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_BUCKET_MODE,
                                    NULL, &index, NULL));

    /* Configure SHAPING_CONTROL */
    if (index < 48) { /* in SHAPING_CONTROL */
        shaping_reg = SHAPING_CONTROLr;
    } else { /* in EXT1_SHAPING_CONTROL_PORTx */
        index -= 48;
        si = &SOC_INFO(unit);
        mmu_cmic_port =
            si->port_p2m_mapping[si->port_l2p_mapping[si->cmic_port]];
        mmu_lb_port = si->port_p2m_mapping[si->port_l2p_mapping[si->lb_port]];
        mmu_port = si->port_p2m_mapping[si->port_l2p_mapping[local_port]];
        if (mmu_port < mmu_lb_port) { /* X pipe */
            shaping_reg = ext_shaping_reg_x[mmu_port - mmu_cmic_port - 1];
        } else { /* Y pipe */
            shaping_reg = ext_shaping_reg_y[mmu_port - mmu_lb_port - 1];
        }
    }

    /* Retrieve SHAPING_CONTROL */
    BCM_IF_ERROR_RETURN
        (soc_reg_get(unit, shaping_reg, local_port, 0, &orig_rval64));

    /* Construct SHAPING_CONTROL */
    if (index < 32) {
        COMPILER_64_SET(mode_rval64, 0, 1 << index);
    } else {
        COMPILER_64_SET(mode_rval64, 1 << (index - 32), 0);
    }
    rval64 = orig_rval64;
    if (flags & BCM_COSQ_BW_PACKET_MODE) {
        COMPILER_64_OR(rval64, mode_rval64);
    } else {
        COMPILER_64_NOT(mode_rval64);
        COMPILER_64_AND(rval64, mode_rval64);
    }

    /* Write the constructed value to SHAPING_CONTROL hardware */
    if (COMPILER_64_NE(rval64, orig_rval64)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_set(unit, shaping_reg, local_port, 0, rval64));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_bucket_get
 * Purpose:
 *     Get COS queue bandwidth control bucket setting
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_bandwidth - (OUT) min bucket bandwidth (kbps or packet/second)
 *     max_bandwidth - (OUT) max bucket bandwidth (kbps or packet/second)
 *     min_burst     - (OUT) min bucket burst size (kbits or packets)
 *     max_burst     - (OUT) max bucket burst size (kbits or packets)
 *     flags         - (OUT) BCM_COSQ_BW_PACKET_MODE_XXX
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_td_cosq_bucket_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                        uint32 *min_bandwidth, uint32 *max_bandwidth,
                        uint32 *min_burst, uint32 *max_burst,
                        uint32 *flags)
{
    soc_info_t *si;
    _bcm_td_cosq_node_t *node;
    bcm_port_t local_port, mmu_port, mmu_cmic_port, mmu_lb_port;
    int index, rv;
    soc_reg_t max_config_reg, min_config_reg, shaping_reg;
    uint32 rval;
    uint64 rval64;
    uint32 refresh_rate, bucketsize, granularity, meter_flags;
    static const soc_reg_t ext_shaping_reg_x[] = {
        EXT1_SHAPING_CONTROL_PORT1r, EXT1_SHAPING_CONTROL_PORT2r,
        EXT1_SHAPING_CONTROL_PORT3r, EXT1_SHAPING_CONTROL_PORT4r
    };
    static const soc_reg_t ext_shaping_reg_y[] = {
        EXT1_SHAPING_CONTROL_PORT34r, EXT1_SHAPING_CONTROL_PORT35r,
        EXT1_SHAPING_CONTROL_PORT36r, EXT1_SHAPING_CONTROL_PORT37r
    };

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    if (min_bandwidth == NULL || max_bandwidth == NULL || 
        min_burst == NULL || max_burst == NULL || flags == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_BUCKET_MODE,
                                    &local_port, &index, NULL));

    if (index < 48) { /* in SHAPING_CONTROL */
        shaping_reg = SHAPING_CONTROLr;
    } else { /* in EXT1_SHAPING_CONTROL_PORTx */
        index -= 48;
        si = &SOC_INFO(unit);
        mmu_cmic_port =
            si->port_p2m_mapping[si->port_l2p_mapping[si->cmic_port]];
        mmu_lb_port = si->port_p2m_mapping[si->port_l2p_mapping[si->lb_port]];
        mmu_port = si->port_p2m_mapping[si->port_l2p_mapping[local_port]];
        if (mmu_port < mmu_lb_port) { /* X pipe */
            shaping_reg = ext_shaping_reg_x[mmu_port - mmu_cmic_port - 1];
        } else { /* Y pipe */
            shaping_reg = ext_shaping_reg_y[mmu_port - mmu_lb_port - 1];
        }
    }
    BCM_IF_ERROR_RETURN
        (soc_reg_get(unit, shaping_reg, local_port, 0, &rval64));
    if (index < 32) {
        *flags = COMPILER_64_LO(rval64) & (1 << index) ?
            BCM_COSQ_BW_PACKET_MODE : 0;
    } else {
        *flags = COMPILER_64_HI(rval64) & (1 << (index - 32)) ?
            BCM_COSQ_BW_PACKET_MODE : 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_BUCKET,
                                    NULL, &index, NULL));

    if (BCM_GPORT_IS_SCHEDULER(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, port, NULL, NULL, NULL, &node));
        if (node->level == 3) { /* S3 scheduler */
            max_config_reg = S3_MAXBUCKETCONFIG_64r;
            min_config_reg = S3_MINBUCKETCONFIG_64r;
        } else if (node->level == 2) { /* S2 scheduler */
            max_config_reg = S2_MAXBUCKETCONFIG_64r;
            min_config_reg = S2_MINBUCKETCONFIG_64r;
        } else {
            return BCM_E_PARAM;
        }
    } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
               BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
        max_config_reg = MAXBUCKETCONFIG_64r;
        min_config_reg = MINBUCKETCONFIG_64r;
    } else { /* local port */
        if (cosq < _BCM_TD_NUM_S3_SCHEDULER &&
            !IS_CPU_PORT(unit, local_port)) {
            /* use S3.0 - S3.3 for unicast and multicast queue 0 - 3 */
            max_config_reg = S3_MAXBUCKETCONFIG_64r;
            min_config_reg = S3_MINBUCKETCONFIG_64r;
        } else { /* unicast queue 4 - 7 */
            max_config_reg = MAXBUCKETCONFIG_64r;
            min_config_reg = MINBUCKETCONFIG_64r;
        }
    }

    meter_flags = *flags & BCM_COSQ_BW_PACKET_MODE ?
        _BCM_TD_METER_FLAG_PACKET_MODE : 0;
    BCM_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    if (soc_reg_field_get(unit, MISCCONFIGr, rval, ITU_MODE_SELf)) {
        meter_flags |= _BCM_TD_METER_FLAG_NON_LINEAR;
    }

    BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_freeze(unit));
    rv = soc_reg_get(unit, max_config_reg, local_port, index, &rval64);
    BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_thaw(unit));
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    granularity =
        soc_reg64_field32_get(unit, max_config_reg, rval64, METER_GRANf);
    refresh_rate =
        soc_reg64_field32_get(unit, max_config_reg, rval64, MAX_REFRESHf);
    bucketsize = soc_reg64_field32_get(unit, max_config_reg, rval64, MAX_THDf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                         granularity, meter_flags,
                                         max_bandwidth, max_burst));

    BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_freeze(unit));
    rv = soc_reg_get(unit, min_config_reg, local_port, index, &rval64);
    BCM_IF_ERROR_RETURN(_bcm_td_cosq_refresh_thaw(unit));
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    granularity =
        soc_reg64_field32_get(unit, min_config_reg, rval64, METER_GRANf);
    refresh_rate =
        soc_reg64_field32_get(unit, min_config_reg, rval64, MIN_REFRESHf);
    bucketsize = soc_reg64_field32_get(unit, min_config_reg, rval64, MIN_THDf);
    BCM_IF_ERROR_RETURN
        (_bcm_td_bucket_encoding_to_rate(unit, refresh_rate, bucketsize,
                                         granularity, meter_flags,
                                         min_bandwidth, min_burst));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_wred_set
 * Purpose:
 *     Configure unicast queue WRED setting
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     flags               - (IN) BCM_COSQ_DISCARD_XXX
 *     min_thresh          - (IN)
 *     max_thresh          - (IN)
 *     drop_probablity     - (IN)
 *     gain                - (IN)
 *     ignore_enable_flags - (IN)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_td_cosq_wred_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      uint32 flags, uint32 min_thresh, uint32 max_thresh,
                      int drop_probability, int gain, int ignore_enable_flags)
{
    soc_info_t *si;
    soc_reg_t reg;
    soc_field_t field;
    uint32 rval;
    bcm_port_t local_port;
    int index;
    uint32 profile_index, old_profile_index;
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    void *entries[6];
    soc_mem_t mems[6];
    int rate, i, exists = 0;
    static soc_mem_t wred_mems[6] = {
        MMU_WRED_DROP_CURVE_PROFILE_0m,
        MMU_WRED_DROP_CURVE_PROFILE_1m,
        MMU_WRED_DROP_CURVE_PROFILE_2m,
        MMU_WRED_DROP_CURVE_PROFILE_3m,
        MMU_WRED_DROP_CURVE_PROFILE_4m,
        MMU_WRED_DROP_CURVE_PROFILE_5m
    };

    if ((port == -1) && (!(flags & BCM_COSQ_DISCARD_DEVICE))) {
        return BCM_E_PORT;
    }
    entries[0] = &entry_tcp_green;
    entries[1] = &entry_tcp_yellow;
    entries[2] = &entry_tcp_red;
    entries[3] = &entry_nontcp_green;
    entries[4] = &entry_nontcp_yellow;
    entries[5] = &entry_nontcp_red;
    si = &SOC_INFO(unit);
    /*
     * if port == -1 and BCM_COSQ_DISCARD_DEVICE
     *  set the all the service pools
     */
    if ((port == -1) && (flags & BCM_COSQ_DISCARD_DEVICE)) {
        reg = GLOBAL_SP_WRED_CONFIGr;
        field = WRED_ENABLEf;
        local_port = 0;
        index = 0;
    } else {
        /*
         * For extended queue port:
         * WRED_CONFIG:
         *     index 0: summary of regular uc cosq 0 and extended uc queue 0-15
         *     index 1: summary of regular uc cosq 1 and extended uc queue 16-31
         *     index 2: summary of regular uc cosq 2 and extended uc queue 32-47
         *     index 3: summary of regular uc cosq 3 and extended uc queue 48-63
         *     these summary does not need to equal to the sum of corresponding
         *     entries in DMVOQ_WRED_CONFIG
         * DMVOQ_WRED_CONFIG:
         *     index 0 - 63 for extended unicast queue 0 - queue 63
         *     index 64 - 71 for unicast queue 0 - queue 7
         */

        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_WRED,
                                        &local_port, &index, NULL));
        if (flags & (BCM_COSQ_DISCARD_DEVICE | BCM_COSQ_DISCARD_PORT)) {
            /* Get the pool number for the queue */
            if (si->port_num_ext_cosq[local_port] == 0) { /* regular port */
                reg = OP_UC_PORT_CONFIG1_CELLr;
                field = _bcm_td_uc_spid_fields[index];
            } else { /* extended queue port */
                reg = _bcm_td_ext_uc_spid_regs[index / 16];
                field = _bcm_td_ext_uc_spid_fields[index];
            }
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, 0, &rval));
            index = soc_reg_field_get(unit, reg, rval, field);

            if (flags & BCM_COSQ_DISCARD_DEVICE) {
                reg = GLOBAL_SP_WRED_CONFIGr;
                field = WRED_ENABLEf;
                local_port = 0;
            } else {
                reg = PORT_SP_WRED_CONFIGr;
                field = WRED_ENABLEf;
            }
        } else {
            if (si->port_num_ext_cosq[local_port] == 0) { /* regular port */
                reg = WRED_CONFIGr;
            } else { /* extended queue port */
                reg = DMVOQ_WRED_CONFIGr;
            }
            field = WRED_ENf;
        }
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, index, &rval));

    old_profile_index = 0xffffffff;
    if (flags & (BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_COLOR_ALL |
                 BCM_COSQ_DISCARD_TCP)) {
        old_profile_index = soc_reg_field_get(unit, reg, rval, PROFILE_INDEXf);
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_get(unit, _bcm_td_wred_profile[unit],
                                 old_profile_index, 1, entries));
        for (i = 0; i < 6; i++) {
            mems[i] = INVALIDm;
        }
        if (!(flags & (BCM_COSQ_DISCARD_COLOR_GREEN |
                       BCM_COSQ_DISCARD_COLOR_YELLOW |
                       BCM_COSQ_DISCARD_COLOR_RED))) {
            flags |= BCM_COSQ_DISCARD_COLOR_ALL;
        }
        if (!(flags & BCM_COSQ_DISCARD_NONTCP) || (flags & BCM_COSQ_DISCARD_TCP)) {
            if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                mems[0] = MMU_WRED_DROP_CURVE_PROFILE_0m;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                mems[1] = MMU_WRED_DROP_CURVE_PROFILE_1m;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                mems[2] = MMU_WRED_DROP_CURVE_PROFILE_2m;
            }
        }
        if (flags & BCM_COSQ_DISCARD_NONTCP) {                                                                  
            if (flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
                mems[3] = MMU_WRED_DROP_CURVE_PROFILE_3m;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
                mems[4] = MMU_WRED_DROP_CURVE_PROFILE_4m;
            }
            if (flags & BCM_COSQ_DISCARD_COLOR_RED) {
                mems[5] = MMU_WRED_DROP_CURVE_PROFILE_5m;
            }
        }
        rate = _bcm_td_percent_to_drop_prob(drop_probability);
        for (i = 0; i < 6; i++) {
            exists = 0;
            if ((soc_mem_field32_get(unit, wred_mems[i], entries[i],
                                    MIN_THDf) != 0xffff) &&
                                    (mems[i] == INVALIDm)) {
                mems[i] = wred_mems[i];
                exists = 1;
            } else {
                soc_mem_field32_set(unit, wred_mems[i], entries[i], MIN_THDf,
                                    (mems[i] == INVALIDm) ? 0xffff :
                                    min_thresh);
            }

            if ((soc_mem_field32_get(unit, wred_mems[i], entries[i],
                                    MAX_THDf) != 0xffff) &&
                                    ((mems[i] == INVALIDm) || exists)) {
                mems[i] = wred_mems[i];
                exists = 1;
            } else {
                soc_mem_field32_set(unit, wred_mems[i], entries[i], MAX_THDf,
                                    (mems[i] == INVALIDm) ? 0xffff :
                                    max_thresh);
            }

            if (!exists) {
                soc_mem_field32_set(unit, wred_mems[i], entries[i],
                                    MAX_DROP_RATEf,
                                    (mems[i] == INVALIDm) ? 0 : rate);
            }
        }
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, _bcm_td_wred_profile[unit], entries, 1,
                                 &profile_index));

        soc_reg_field_set(unit, reg, &rval, PROFILE_INDEXf, profile_index);
        soc_reg_field_set(unit, reg, &rval, WEIGHTf, gain);
    }

    /* Some APIs only modify profiles */
    if (!ignore_enable_flags) {
        soc_reg_field_set(unit, reg, &rval, CAP_AVERAGEf,
                          flags & BCM_COSQ_DISCARD_CAP_AVERAGE ? 1 : 0);
        soc_reg_field_set(unit, reg, &rval, field,
                          flags & BCM_COSQ_DISCARD_ENABLE ? 1 : 0);
        if (reg == WRED_CONFIGr || reg == DMVOQ_WRED_CONFIGr) {
            soc_reg_field_set(unit, reg, &rval, ECN_MARKING_ENf,
                              flags & BCM_COSQ_DISCARD_MARK_CONGESTION ?
                              1 : 0);
        }
    }
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, local_port, index, rval));
    if ((port == -1) && (flags & BCM_COSQ_DISCARD_DEVICE)) {
        for (i = 1; i< 4; i++) {
            BCM_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, _bcm_td_wred_profile[unit], entries, 1,
                                     &profile_index));
            BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, local_port, i, rval));
        }
    }

    if (old_profile_index != 0xffffffff) {
        BCM_IF_ERROR_RETURN
            (soc_profile_mem_delete(unit, _bcm_td_wred_profile[unit],
                                    old_profile_index));
        if ((port == -1) &&(flags & BCM_COSQ_DISCARD_DEVICE)) {
            for (i = 1; i< 4; i++) {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_delete(unit, _bcm_td_wred_profile[unit],
                                            old_profile_index));
            }
        }
    }


       if (!((port == -1) && (flags & BCM_COSQ_DISCARD_DEVICE))) {
           BCM_IF_ERROR_RETURN
               (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                           _BCM_TD_COSQ_INDEX_STYLE_WRED_DROP_THRESH,
                                           NULL, &index, NULL));
           BCM_IF_ERROR_RETURN
               (soc_mem_write(unit, MMU_WRED_DROP_THD_UC_ENQ0m, MEM_BLOCK_ALL, index,
                              soc_mem_entry_null(unit, MMU_WRED_DROP_THD_UC_ENQ0m)));
           BCM_IF_ERROR_RETURN
               (soc_mem_write(unit, MMU_WRED_DROP_THD_UC_ENQ1m, MEM_BLOCK_ALL, index,
                              soc_mem_entry_null(unit, MMU_WRED_DROP_THD_UC_ENQ1m)));
           BCM_IF_ERROR_RETURN
               (soc_mem_write(unit, MMU_WRED_DROP_THD_UC_DEQ0m, MEM_BLOCK_ALL, index,
                              soc_mem_entry_null(unit, MMU_WRED_DROP_THD_UC_DEQ0m)));
           BCM_IF_ERROR_RETURN
               (soc_mem_write(unit, MMU_WRED_DROP_THD_UC_DEQ1m, MEM_BLOCK_ALL, index,
                              soc_mem_entry_null(unit, MMU_WRED_DROP_THD_UC_DEQ1m)));
       }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_wred_get
 * Purpose:
 *     Get unicast queue WRED setting
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     flags               - (IN/OUT) BCM_COSQ_DISCARD_XXX
 *     min_thresh          - (OUT)
 *     max_thresh          - (OUT)
 *     drop_probablity     - (OUT)
 *     gain                - (OUT)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
STATIC int
_bcm_td_cosq_wred_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                      uint32 *flags, uint32 *min_thresh, uint32 *max_thresh,
                      int *drop_probability, int *gain)
{
    soc_info_t *si;
    soc_reg_t reg;
    soc_field_t field;
    uint32 rval;
    bcm_port_t local_port;
    int index, profile_index;
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    void *entries[6];
    void *entry_p;
    soc_mem_t mem;
    int rate;
    if ((port == -1) && (!(*flags & BCM_COSQ_DISCARD_DEVICE))) {
        return BCM_E_PORT;
    }

    si = &SOC_INFO(unit);
    if ((port == -1 ) && (*flags & BCM_COSQ_DISCARD_DEVICE)) {
        reg = GLOBAL_SP_WRED_CONFIGr;
        field = WRED_ENABLEf;
        local_port = 0;
        index = 0;
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_WRED,
                                        &local_port, &index, NULL));
        if (*flags & (BCM_COSQ_DISCARD_DEVICE | BCM_COSQ_DISCARD_PORT)) {
            /* Get the pool number for the queue */
            if (si->port_num_ext_cosq[local_port] == 0) { /* regular port */
                reg = OP_UC_PORT_CONFIG1_CELLr;
                field = _bcm_td_uc_spid_fields[index];
            } else { /* extended queue port */
                reg = _bcm_td_ext_uc_spid_regs[index / 16];
                field = _bcm_td_ext_uc_spid_fields[index];
            }
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, 0, &rval));
            index = soc_reg_field_get(unit, reg, rval, field);
            if (*flags & BCM_COSQ_DISCARD_DEVICE) {
                reg = GLOBAL_SP_WRED_CONFIGr;
                field = WRED_ENABLEf;
                local_port = 0;
            } else {
                reg = PORT_SP_WRED_CONFIGr;
                field = WRED_ENABLEf;
            }
        } else {
            if (si->port_num_ext_cosq[local_port] == 0) { /* regular port */
                reg = WRED_CONFIGr;
            } else { /* extended queue port */
                reg = DMVOQ_WRED_CONFIGr;
            }
            field = WRED_ENf;
        }
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, index, &rval));

    if (!(*flags & BCM_COSQ_DISCARD_NONTCP)) {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_0m;
            entry_p = &entry_tcp_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_1m;
            entry_p = &entry_tcp_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_2m;
            entry_p = &entry_tcp_red;
        } else {
           mem = MMU_WRED_DROP_CURVE_PROFILE_0m;
           entry_p = &entry_tcp_green;
        }
    } else {
        if (*flags & BCM_COSQ_DISCARD_COLOR_GREEN) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_3m;
            entry_p = &entry_nontcp_green;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_YELLOW) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_4m;
            entry_p = &entry_nontcp_yellow;
        } else if (*flags & BCM_COSQ_DISCARD_COLOR_RED) {
            mem = MMU_WRED_DROP_CURVE_PROFILE_5m;
            entry_p = &entry_nontcp_red;
        } else {
           mem = MMU_WRED_DROP_CURVE_PROFILE_3m;
           entry_p = &entry_nontcp_green;
        }
    }
    profile_index = soc_reg_field_get(unit, reg, rval, PROFILE_INDEXf);
    entries[0] = &entry_tcp_green;
    entries[1] = &entry_tcp_yellow;
    entries[2] = &entry_tcp_red;
    entries[3] = &entry_nontcp_green;
    entries[4] = &entry_nontcp_yellow;
    entries[5] = &entry_nontcp_red;
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_get(unit, _bcm_td_wred_profile[unit],
                             profile_index, 1, entries));
    if (min_thresh != NULL) {
        *min_thresh = soc_mem_field32_get(unit, mem, entry_p, MIN_THDf);
    }
    if (max_thresh != NULL) {
        *max_thresh = soc_mem_field32_get(unit, mem, entry_p, MAX_THDf);
    }
    if (drop_probability != NULL) {
        rate = soc_mem_field32_get(unit, mem, entry_p, MAX_DROP_RATEf);
        *drop_probability = _bcm_td_drop_prob_to_percent(rate);
    }

    if (gain != NULL) {
        *gain = soc_reg_field_get(unit, reg, rval, WEIGHTf);
    }

    *flags &= ~(BCM_COSQ_DISCARD_CAP_AVERAGE | BCM_COSQ_DISCARD_ENABLE);
    if (soc_reg_field_get(unit, reg, rval, CAP_AVERAGEf)) {
        *flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
    }
    if (soc_reg_field_get(unit, reg, rval, field)) {
        *flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    if (reg == WRED_CONFIGr || reg == DMVOQ_WRED_CONFIGr) {
        if (soc_reg_field_get(unit, reg, rval, ECN_MARKING_ENf)) {
            *flags |= BCM_COSQ_DISCARD_MARK_CONGESTION;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_sched_set
 * Purpose:
 *     Set scheduling mode
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     mode                - (IN) scheduling mode (BCM_COSQ_XXX)
 *     num_weights         - (IN) number of entries in weights array
 *     weights             - (IN) weights array
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_cosq_sched_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       int mode, int num_weights, int *weights)
{
    soc_info_t *si;
    _bcm_td_cosq_port_info_t *port_info;
    _bcm_td_cosq_node_t *node;
    bcm_port_t local_port;
    int index, minsp_index;
    soc_reg_t config_reg, weight_reg, minsp_reg;
    uint32 rval, fval, rval_hi, rval_lo;
    uint64 rval64;
    int mode_sel_shift, max_weight, minsp_width, mode_sel, i;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    si = &SOC_INFO(unit);

    mode_sel_shift = 0;
    minsp_index = 0;
    minsp_width = -1;

    if (BCM_GPORT_IS_SCHEDULER(port)) { /* ETS style scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                   &node));

        if (cosq + num_weights > node->numq) {
            return BCM_E_PARAM;
        }

        if (node->hw_cosq_attached_to < 0) { /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }

        switch (node->level) {
        case 1: /* S1 scheduler */
            config_reg = ESCONFIGr;
            weight_reg = COSWEIGHTSr;
            minsp_reg = MINSPCONFIGr;
            break;
        case 2:
            if (node->hw_cosq_attached_to == 0) { /* MC group scheduler */
                config_reg = S3_CONFIG_MCr;
                weight_reg = S3_COSWEIGHTSr;
                minsp_reg = S3_MINSPCONFIG_MCr;
            } else { /* S2 scheduler */
                config_reg = S2_CONFIGr;
                weight_reg = S2_COSWEIGHTSr;
                minsp_reg = S2_MINSPCONFIGr;
                mode_sel_shift = (node->hw_cosq_attached_to - 1) * 2;
            }
            break;
        case 3: /* S3 scheduler */
            config_reg = S3_CONFIGr;
            weight_reg = S3_COSWEIGHTSr;
            minsp_reg = S3_MINSPCONFIGr;
            mode_sel_shift = node->hw_cosq_attached_to * 2;
            minsp_index = node->hw_cosq_attached_to;
            minsp_width = si->port_num_ext_cosq[local_port] ? 18 : 2;
            break;
        default:
            return BCM_E_INTERNAL;
        }
    } else { /* non-ETS style scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        if (IS_CPU_PORT(unit, local_port)) {
            if (cosq + num_weights > si->num_cpu_cosq) {
                return BCM_E_PARAM;
            }
            config_reg = ESCONFIGr;
            weight_reg = COSWEIGHTSr;
            minsp_reg = MINSPCONFIG_CPUr;
        } else {
            port_info = &_bcm_td_cosq_port_info[unit][local_port];
            for (i = 0; i < _BCM_TD_NUM_SCHEDULER; i++) {
                if (port_info->sched[i].numq > 0 &&
                    port_info->sched[i].cosq_attached_to != -1) {
                    /* ETS style scheduling has been setup */
                    return BCM_E_PORT;
                }
            }
            if (cosq + num_weights > _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE) {
                return BCM_E_PARAM;
            }
            config_reg = S2_CONFIGr;
            weight_reg = S2_COSWEIGHTSr;
            minsp_reg = S2_MINSPCONFIGr;
            mode_sel_shift = 0; /* S2.0 */
        }
    }

    switch (mode) {
    case BCM_COSQ_STRICT:
        mode_sel = 0;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        mode_sel = 1;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
        mode_sel = 2;
        break;
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        mode_sel = 3;
        break;
    default:
        return BCM_E_PARAM;
    }

    if (mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN ||
        mode == BCM_COSQ_DEFICIT_ROUND_ROBIN) {
        max_weight =
            (1 << soc_reg_field_length(unit, weight_reg, COSWEIGHTSf)) - 1;
        for (i = 0; i < num_weights; i++) {
            if (weights[i] < 0 || weights[i] > max_weight) {
                return BCM_E_PARAM;
            }
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_get(unit, minsp_reg, local_port, minsp_index, &rval64));
        rval_hi = COMPILER_64_HI(rval64);
        rval_lo = COMPILER_64_LO(rval64);
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, port, cosq + i,
                                            _BCM_TD_COSQ_INDEX_STYLE_SCHEDULER,
                                            NULL, &index, NULL));
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, weight_reg, local_port, index, &rval));
            soc_reg_field_set(unit, weight_reg, &rval, COSWEIGHTSf,
                              weights[i]);
            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, weight_reg, local_port, index, rval));

            /* Set strict priority bit if weight is zero */
            if (minsp_width != -1) {
                index %= minsp_width;
            }
            if (index < 32) {
                if (weights[i] != 0) {
                    rval_lo &= ~(1 << index);
                } else {
                    rval_lo |= 1 << index;
                }
            } else {
                if (weights[i] != 0) {
                    rval_hi &= ~(1 << (index - 32));
                } else {
                    rval_hi |= 1 << (index - 32);
                }
            }
        }

        COMPILER_64_SET(rval64, rval_hi, rval_lo);
        BCM_IF_ERROR_RETURN
            (soc_reg_set(unit, minsp_reg, local_port, 0, rval64));
    }

    /* Program the scheduling mode */
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, config_reg, local_port, 0, &rval));
    fval = soc_reg_field_get(unit, config_reg, rval, SCHEDULING_SELECTf);
    fval &= ~(0x3 << mode_sel_shift);
    fval |= mode_sel << mode_sel_shift;
        soc_reg_field_set(unit, config_reg, &rval, SCHEDULING_SELECTf, fval);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, config_reg, local_port, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_sched_get
 * Purpose:
 *     Get scheduling mode setting
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) port number or GPORT identifier
 *     cosq                - (IN) COS queue number
 *     mode                - (OUT) scheduling mode (BCM_COSQ_XXX)
 *     num_weights         - (IN) number of entries in weights array
 *     weights             - (OUT) weights array
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_cosq_sched_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                       int *mode, int num_weights, int *weights)
{
    soc_info_t *si;
    _bcm_td_cosq_port_info_t *port_info;
    _bcm_td_cosq_node_t *node;
    bcm_port_t local_port;
    int index;
    soc_reg_t config_reg, weight_reg;
    uint32 rval;
    int mode_sel_shift, mode_sel, i;

    if (cosq < 0) {
        if (cosq == -1) {
            /* caller needs to resolve the wild card value (-1) */
            return BCM_E_INTERNAL;
        } else { /* reject all other negative value */
            return BCM_E_PARAM;
        }
    }

    si = &SOC_INFO(unit);

    mode_sel_shift = 0;

    if (BCM_GPORT_IS_SCHEDULER(port)) { /* ETS style scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, port, NULL, &local_port, NULL,
                                   &node));

        if (cosq + num_weights > node->numq) {
            return BCM_E_PARAM;
        }

        if (node->hw_cosq_attached_to < 0) { /* Not resolved yet */
            return BCM_E_NOT_FOUND;
        }

        switch (node->level) {
        case 1: /* S1 scheduler */
            config_reg = ESCONFIGr;
            weight_reg = COSWEIGHTSr;
            break;
        case 2:
            if (node->hw_cosq_attached_to == 0) { /* MC group scheduler */
                config_reg = S3_CONFIG_MCr;
                weight_reg = S3_COSWEIGHTSr;
            } else { /* S2 scheduler */
                config_reg = S2_CONFIGr;
                weight_reg = S2_COSWEIGHTSr;
                mode_sel_shift = (node->hw_cosq_attached_to - 1) * 2;
            }
            break;
        case 3: /* S3 scheduler */
            config_reg = S3_CONFIGr;
            weight_reg = S3_COSWEIGHTSr;
            mode_sel_shift = node->hw_cosq_attached_to * 2;
            break;
        default:
            return BCM_E_INTERNAL;
        }
    } else { /* non-ETS style scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        if (IS_CPU_PORT(unit, local_port)) {
            if (cosq + num_weights > si->num_cpu_cosq) {
                return BCM_E_PARAM;
            }
            config_reg = ESCONFIGr;
            weight_reg = COSWEIGHTSr;
        } else {
            port_info = &_bcm_td_cosq_port_info[unit][local_port];
            for (i = 0; i < _BCM_TD_NUM_SCHEDULER; i++) {
                if (port_info->sched[i].numq > 0 &&
                    port_info->sched[i].cosq_attached_to != -1) {
                    /* ETS style scheduling has been setup */
                    return BCM_E_PORT;
                }
            }
            if (cosq + num_weights > _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE) {
                return BCM_E_PARAM;
            }
            config_reg = S2_CONFIGr;
            weight_reg = S2_COSWEIGHTSr;
            mode_sel_shift = 0; /* S2.0 */
        }
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, config_reg, local_port, 0, &rval));
    mode_sel = soc_reg_field_get(unit, config_reg, rval, SCHEDULING_SELECTf);

    switch ((mode_sel >> mode_sel_shift) & 0x03) {
    case 0:
        *mode = BCM_COSQ_STRICT;
        break;
    case 1:
        *mode = BCM_COSQ_ROUND_ROBIN;
        break;
    case 2:
        *mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
        break;
    case 3:
        *mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
        break;
    }

    if (*mode == BCM_COSQ_WEIGHTED_ROUND_ROBIN ||
        *mode == BCM_COSQ_DEFICIT_ROUND_ROBIN) {
        for (i = 0; i < num_weights; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, port, cosq + i,
                                            _BCM_TD_COSQ_INDEX_STYLE_SCHEDULER,
                                            NULL, &index, NULL));
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, weight_reg, local_port, 
                                index, &rval));
            weights[i] = soc_reg_field_get(unit, weight_reg, rval,
                                           COSWEIGHTSf);
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_ingress_sp_get(int unit, bcm_gport_t gport,
                         bcm_cos_queue_t pri_grp, int *p_pool_start,
                         int *p_pool_end)
{
    int local_port;
    uint32 rval, pool;
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    if (pri_grp == BCM_COS_INVALID) {
        *p_pool_start = 0;
        *p_pool_end = 3;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    if ((pri_grp < 0) || (pri_grp >= 8)) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_PORT_PG_SPIDr(unit, local_port, &rval));
    pool = soc_reg_field_get(unit, PORT_PG_SPIDr, rval,
                                prigroup_spid_field[pri_grp]);

    *p_pool_start = *p_pool_end = pool;
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_1

#define _BCM_TD_COSQ_WB_UCAST_COSQ_MASK        0x1f /* byte 0 bit 4-0 */
#define _BCM_TD_COSQ_WB_UCAST_COSQ_SHIFT       0
#define _BCM_TD_COSQ_WB_UCAST_BASE_MASK        0x07 /* byte 0 bit 7-5 */
#define _BCM_TD_COSQ_WB_UCAST_BASE_SHIFT       5
#define _BCM_TD_COSQ_WB_MCAST_COSQ_MASK        0x1f /* byte 0 bit 4-0 */
#define _BCM_TD_COSQ_WB_MCAST_COSQ_SHIFT       0
#define _BCM_TD_COSQ_WB_MCAST_BASE_MASK        0x07 /* byte 0 bit 7-5 */
#define _BCM_TD_COSQ_WB_MCAST_BASE_SHIFT       5
#define _BCM_TD_COSQ_WB_EXT_UCAST_COSQ_MASK    0x1f /* byte 0 bit 4-0 */
#define _BCM_TD_COSQ_WB_EXT_UCAST_COSQ_SHIFT   0
#define _BCM_TD_COSQ_WB_EXT_UCAST_NUMQ_MASK    0x07 /* byte 0 bit 7-5 */
#define _BCM_TD_COSQ_WB_EXT_UCAST_NUMQ_SHIFT   5
#define _BCM_TD_COSQ_WB_EXT_UCAST_BASE_MASK    0x0f /* byte 1 bit 3-0 */
#define _BCM_TD_COSQ_WB_EXT_UCAST_BASE_SHIFT   0
#define _BCM_TD_COSQ_WB_SCHED_LEVEL_MASK       0x03 /* byte 0 bit 1-0 */
#define _BCM_TD_COSQ_WB_SCHED_LEVEL_SHIFT      0
#define _BCM_TD_COSQ_WB_SCHED_NUMQ_MASK        0x1f /* byte 0 bit 6-2 */
#define _BCM_TD_COSQ_WB_SCHED_NUMQ_SHIFT       2
#define _BCM_TD_COSQ_WB_SCHED_COSQ_MASK        0x0f /* byte 1 bit 3-0 */
#define _BCM_TD_COSQ_WB_SCHED_COSQ_SHIFT       0
#define _BCM_TD_COSQ_WB_SCHED_HW_COSQ_MASK     0x07 /* byte 1 bit 6-4 */
#define _BCM_TD_COSQ_WB_SCHED_HW_COSQ_SHIFT    4

#define SET_FIELD(_field, _value)                       \
    (((_value) & _BCM_TD_COSQ_WB_## _field## _MASK) <<  \
     _BCM_TD_COSQ_WB_## _field## _SHIFT)
#define GET_FIELD(_field, _byte)                        \
    (((_byte) >> _BCM_TD_COSQ_WB_## _field## _SHIFT) &  \
     _BCM_TD_COSQ_WB_## _field## _MASK)

/*
 * Function:
 *     _bcm_td_cosq_wb_alloc_size_get
 * Purpose:
 *     Calculates the scache space required for a given warmboot version 
 * Parameters:
 *     unit - unit number
 *     wb_ver - WarmBoot version
 * Returns:
 *     Size of scache size required
 * Notes:
 *    Handle scache allocation sizes for new versions
 */
STATIC int
_bcm_td_cosq_wb_alloc_size_get(int unit, int wb_ver)
{
    bcm_port_t port;
    int alloc_size = 0;

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        if (SOC_INFO(unit).port_num_ext_cosq[port] != 0) {
            alloc_size += 68;
        } else {
            alloc_size += 36;
        }
        /* Scache space to save ucast SC/QM queu info */
        if (wb_ver >= BCM_WB_VERSION_1_1) {
            alloc_size += 3;
        }
    }

    return alloc_size;
}

/*
 * Function:
 *     _bcm_td_cosq_wb_alloc
 * Purpose:
 *     Allocates required scache space in Cold Boot
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_cosq_wb_alloc(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int rv, alloc_size;

    alloc_size = _bcm_td_cosq_wb_alloc_size_get(unit, BCM_WB_DEFAULT_VERSION);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    /* Allocate required scache size */
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE, alloc_size,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        rv = BCM_E_NONE;
    }

    return rv;
}

int
bcm_td_cosq_sync(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *map;
    uint8 *scache_ptr_2, *map_2;
    _bcm_td_cosq_port_info_t *port_info;
    _bcm_td_cosq_node_t *node;
    bcm_port_t port;
    int index;
    uint32 num_cosq;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0,
                                 &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL));

    /* WB version 1.1 state starts at the end of scache for 1.0 version */
    scache_ptr_2 = scache_ptr +
                   _bcm_td_cosq_wb_alloc_size_get(unit, BCM_WB_VERSION_1_0);

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        port_info = &_bcm_td_cosq_port_info[unit][port];

        /* Store unicast queue info into scache */
        map = scache_ptr;
        *map = 0;
        scache_ptr++;
        for (index = 0; index < _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE; index++) {
            node = &port_info->ucast[index];
            if (node->numq == 0) {
                continue;
            }
            /*
             * value range:
             *     numq: 1, or 0 for unused entry
             *     base: 0 - 7
             *     cosq_attached_to: -1 for unattached node
             *                       0 - 1 on regular port
             *                       0 - 17 on extended queue port
             */
            *map |= 1 << index;
            *scache_ptr = SET_FIELD(UCAST_COSQ, node->cosq_attached_to) |
                          SET_FIELD(UCAST_BASE, node->base);
            scache_ptr++;
        }

        /* Store unicast SC/QM queue info into scache */
        /* Note that scache_ptr_2 is used to recovered for level 1.2 */
        map_2 = scache_ptr_2;
        *map_2 = 0;
        scache_ptr_2++;
        for (index = _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE;
             index < _BCM_TD_NUM_UCAST_QUEUE_GROUP;
             index++) {
            node = &port_info->ucast[index];
            if (node->numq == 0) {
                continue;
            }
            /*
             * value range for SC or QM queue (special encoding):
             *     numq: 1, or 0 for unused entry
             *     base : 8 - 9
             *     cosq_attached_to: -1 for unattched node
             *                       0 - 6
             *     use magic number (base + 10) in UCAST_COSQ
             *     use cosq_attached_to in UCAST_BASE
             */
            *map_2 |= 1 << (index - _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE);
            *scache_ptr_2 = SET_FIELD(UCAST_COSQ, node->base + 10) |
                            SET_FIELD(UCAST_BASE, node->cosq_attached_to);
            scache_ptr_2++;
        }

        /* Store multicast queue info into scache */
        map = scache_ptr;
        *map = 0;
        scache_ptr++;
        for (index = 0; index < _BCM_TD_NUM_MCAST_QUEUE_GROUP; index++) {
            node = &port_info->mcast[index];
            if (node->numq == 0) {
                continue;
            }
            /*
             * value range:
             *     numq: 1, or 0 for unused entry
             *     base: 0 - 3
             *     cosq_attached_to: -1 for unattached node
             *                       0 - 1 on regular port
             *                       0 - 17 on extended queue port
             * value range for SC/QM queue:
             *     numq: 1, or 0 for unused entry
             *     base: 4
             *     cosq_attached_to: -1 for unattached node
             *                       0 - 6
             */
            *map |= 1 << index;
            *scache_ptr = SET_FIELD(MCAST_COSQ, node->cosq_attached_to) |
                SET_FIELD(MCAST_BASE, node->base);
            scache_ptr++;
        }

        /* Store extended unicast queue info into scache */
        if (SOC_INFO(unit).port_num_ext_cosq[port] != 0) {
            map = scache_ptr;
            *map = 0;
            scache_ptr += 2;
            for (index = 0; index < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP;
                 index++) {
                node = &port_info->ext_ucast[index];
                if (node->numq == 0) {
                    continue;
                }
                /*
                 * value range:
                 *     numq: 1 - 4, or 0 for unused entry
                 *     base: 0 - 15
                 *     cosq_attached_to: 0 - 17, or -1 for unattached node
                 */
                map[index >> 3] |= 1 << (index & 0x07);
                *scache_ptr =
                    SET_FIELD(EXT_UCAST_COSQ, node->cosq_attached_to) |
                    SET_FIELD(EXT_UCAST_NUMQ, node->numq);
                scache_ptr++;
                if (node->cosq_attached_to != -1) {
                    *scache_ptr = SET_FIELD(EXT_UCAST_BASE, node->base);
                    scache_ptr++;
                }
            }
        }

        /* Store scheduler info into scache */
        map = scache_ptr;
        *map = 0;
        scache_ptr += 2;
        for (index = 0; index < _BCM_TD_NUM_SCHEDULER; index++) {
            node = &port_info->sched[index];
            if (node->numq == 0) {
                continue;
            }
            /*
             * value range:
             *     numq: 0 for unused entry
             *           1 - 4 for S1 scheduler
             *           1 - 4 for MC group scheduler
             *           1 - 9 for S2 scheduler
             *           1 - 18 for S3 scheduler
             *     cosq_attached_to: -1 for unattached node
             *                       0 for S1 scheduler
             *                       0 - 3 for MC group and S3 scheduler
             *                       0 - 8 for S3 scheduler
             *     hw_cosq_attached_to: -1 for unattached or unresolved node
             *                          0 for S1 and MC group scheduler
             *                          1 - 3 for S2 scheduler
             *                          0 - 3 for S3 scheduler
             *     level: 1 - 3, or 0 for unattached node
             */
            map[index >> 3] |= 1 << (index & 0x07);
            *scache_ptr = SET_FIELD(SCHED_LEVEL, node->level) |
                SET_FIELD(SCHED_NUMQ, node->numq);
            scache_ptr++;
            if (node->level != 0) {
                *scache_ptr = SET_FIELD(SCHED_COSQ, node->cosq_attached_to) |
                    SET_FIELD(SCHED_HW_COSQ, node->hw_cosq_attached_to);
                scache_ptr++;
            }
        }
    }
    /* Number COSQ */
    num_cosq = _bcm_td_num_cosq[unit];
    sal_memcpy(scache_ptr_2, &num_cosq, sizeof(num_cosq));

    return BCM_E_NONE;
}

int
bcm_td_cosq_reinit(int unit)
{
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr, *map, value;
    uint8 *scache_ptr_2, *map_2;
    uint16 recovered_ver;
    int additional_scache_size = 0;
    soc_info_t *si;
    _bcm_td_cosq_port_info_t *port_info;
    _bcm_td_cosq_node_t *node, *s1_node, *s2_node[3], *s3_node[4], *mcg_node;
    bcm_module_t local_modid, modid_out;
    bcm_port_t port, port_out, phy_port, mmu_port;
    uint64 rval64;
    uint32 addr, rval, s2_input[3], sched_encap;
    int rv, uc_to_s2, use_mc_group, index, s2_index, hw_cosq, stable_size = 0;
    soc_profile_mem_t *profile_mem;
    soc_profile_reg_t *profile_reg0, *profile_reg1;
    soc_reg_t reg;
    int set_index, cosq, numq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    static const soc_field_t fields[] = {
        S3_GROUP_NO_I0f, S3_GROUP_NO_I1f, S3_GROUP_NO_I2f, S3_GROUP_NO_I3f,
        S3_GROUP_NO_I4f, S3_GROUP_NO_I5f, S3_GROUP_NO_I6f, S3_GROUP_NO_I7f,
        S3_GROUP_NO_I8f
    };
    uint32 num_cosq;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) { 
        /* COSQ warmboot requires extended scache support i.e. level2 warmboot*/  
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_COSQ, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    si = &SOC_INFO(unit);

    /* WB version 1.1 state starts at the end of scache for 1.0 version */
    scache_ptr_2 = scache_ptr +
                   _bcm_td_cosq_wb_alloc_size_get(unit, BCM_WB_VERSION_1_0);

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        port_info = &_bcm_td_cosq_port_info[unit][port];

        /* Retrieve unicast queue info from scache */
        map = scache_ptr;
        scache_ptr++;
        for (index = 0; index < _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE; index++) {
            if ((*map & (1 << index)) == 0) {
                continue;
            }
            node = &port_info->ucast[index];
            node->numq = 1;
            value = GET_FIELD(UCAST_COSQ, *scache_ptr);
            node->cosq_attached_to =
                value == _BCM_TD_COSQ_WB_UCAST_COSQ_MASK ? -1 : value;
            node->base = GET_FIELD(UCAST_BASE, *scache_ptr);
            scache_ptr++;
        }

        /* Retrieve unicast SC/QM queue info from scache */
        /* Note that scache_ptr_2 is used here for recovering this info */
        if (recovered_ver >= BCM_WB_VERSION_1_1) {
        map_2 = scache_ptr_2;
        scache_ptr_2++;
        for (index = _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE;
             index < _BCM_TD_NUM_UCAST_QUEUE_GROUP;
             index++) {

            if ((*map_2 &
                (1 << (index - _BCM_TD_NUM_NON_SC_QM_UCAST_QUEUE))) == 0) {
                continue;
            }
            node = &port_info->ucast[index];
            node->numq = 1;
            value = GET_FIELD(UCAST_COSQ, *scache_ptr_2);
            node->cosq_attached_to = GET_FIELD(UCAST_BASE, *scache_ptr_2);
            node->base = value - 10;
            scache_ptr_2++;
        }
        }

        /* Retrieve multicast queue info from scache */
        map = scache_ptr;
        scache_ptr++;
        for (index = 0; index < _BCM_TD_NUM_MCAST_QUEUE_GROUP; index++) {
            if ((*map & (1 << index)) == 0) {
                continue;
            }
            node = &port_info->mcast[index];
            node->numq = 1;
            value = GET_FIELD(MCAST_COSQ, *scache_ptr);
            node->cosq_attached_to =
                value == _BCM_TD_COSQ_WB_MCAST_COSQ_MASK ? -1 : value;
            node->base = GET_FIELD(MCAST_BASE, *scache_ptr);
            scache_ptr++;
        }

        /* Retrieve extended unicast queue info from scache */
        if (si->port_num_ext_cosq[port] != 0) {
            map = scache_ptr;
            scache_ptr += 2;
            for (index = 0; index < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP;
                 index++) {
                if ((map[index >> 3] & (1 << (index & 0x07))) == 0) {
                    continue;
                }
                node = &port_info->ext_ucast[index];
                value = GET_FIELD(EXT_UCAST_COSQ, *scache_ptr);
                node->cosq_attached_to =
                    value == _BCM_TD_COSQ_WB_EXT_UCAST_COSQ_MASK ? -1 : value;
                node->numq = GET_FIELD(EXT_UCAST_NUMQ, *scache_ptr);
                scache_ptr++;
                if (node->cosq_attached_to != -1) {
                    node->base = GET_FIELD(EXT_UCAST_BASE, *scache_ptr);
                    scache_ptr++;
                }
            }
        }

        /* Retrieve scheduler info from scache */
        map = scache_ptr;
        scache_ptr += 2;
        for (index = 0; index < _BCM_TD_NUM_SCHEDULER; index++) {
            if ((map[index >> 3] & (1 << (index & 0x07))) == 0) {
                continue;
            }
            node = &port_info->sched[index];
            node->level = GET_FIELD(SCHED_LEVEL, *scache_ptr);
            node->numq = GET_FIELD(SCHED_NUMQ, *scache_ptr);
            scache_ptr++;
            if (node->level != 0) {
                value = GET_FIELD(SCHED_COSQ, *scache_ptr);
                node->cosq_attached_to =
                    value == _BCM_TD_COSQ_WB_SCHED_COSQ_MASK ? -1 : value;
                value = GET_FIELD(SCHED_HW_COSQ, *scache_ptr);
                node->hw_cosq_attached_to =
                    value == _BCM_TD_COSQ_WB_SCHED_HW_COSQ_MASK ? -1 : value;
                scache_ptr++;
            } else {
                node->cosq_attached_to = -1;
                node->hw_cosq_attached_to = -1;
            }
        }
    }
    /* Number COSQ */
    sal_memcpy(&num_cosq, scache_ptr_2, sizeof(num_cosq));
    if (!num_cosq) {
        /*sync didn't happen in the previous boot */
        num_cosq = _bcm_esw_cosq_config_property_get(unit);
    }
    _bcm_td_num_cosq[unit] = num_cosq;

    /* Ideally this equation holds good for all incremental versions */
    additional_scache_size =
            _bcm_td_cosq_wb_alloc_size_get(unit, BCM_WB_DEFAULT_VERSION) -
            _bcm_td_cosq_wb_alloc_size_get(unit, recovered_ver);

    /* Need to realloc unavailable space for subsequent syncs */
    SOC_IF_ERROR_RETURN
            (soc_scache_realloc(unit, scache_handle, additional_scache_size));

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        port_info = &_bcm_td_cosq_port_info[unit][port];

        /* Retrieve info from hardware */
        for (s2_index = 0; s2_index < _BCM_TD_NUM_S2_SCHEDULER; s2_index++) {
            s2_input[s2_index] = 0;
            BCM_IF_ERROR_RETURN
                (READ_S2_S3_ROUTINGr(unit, port, s2_index, &rval64));
            for (index = 0; index < _BCM_TD_S2_SCHEDULER_NUM_INPUT; index++) {
                hw_cosq = soc_reg64_field32_get(unit, S2_S3_ROUTINGr, rval64,
                                                fields[index]);
                if (hw_cosq != 0x1f) {
                    s2_input[s2_index] |= 1 << hw_cosq;
                }
            }
        }
        BCM_IF_ERROR_RETURN(READ_S3_CONFIGr(unit, port, &rval));
        uc_to_s2 = soc_reg_field_get(unit, S3_CONFIGr, rval, ROUTE_UC_TO_S2f);
        BCM_IF_ERROR_RETURN(READ_S3_CONFIG_MCr(unit, port, &rval));
        use_mc_group = soc_reg_field_get(unit, S3_CONFIG_MCr, rval,
                                         USE_MC_GROUPf);

        /* Get modid */
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_modid));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, local_modid,
                                     port, &modid_out, &port_out));

        s1_node = NULL;
        s2_node[0] = s2_node[1] = s2_node[2] = NULL;
        s3_node[0] = s3_node[1] = s3_node[2] = s3_node[3] = NULL;
        mcg_node = NULL;

        /* Recover software state of scheduler */
        for (index = 0; index < _BCM_TD_NUM_SCHEDULER; index++) {
            node = &port_info->sched[index];
            if (node->numq == 0) {
                continue;
            }
            sched_encap = (index << 16) | (modid_out << 8) | port_out;
            BCM_GPORT_SCHEDULER_SET(node->gport, sched_encap);
            if (node->cosq_attached_to == -1) {
                continue;
            }
            if (node->level == 1) {
                s1_node = node;
            } else if (node->level == 2) {
                if (node->hw_cosq_attached_to == 0) {
                    mcg_node = node;
                } else if (node->hw_cosq_attached_to != -1) {
                    s2_node[node->hw_cosq_attached_to - 1] = node;
                }
            } else if (node->level == 3) {
                if (node->hw_cosq_attached_to != -1) {
                    s3_node[node->hw_cosq_attached_to] = node;
                }
            }
        }

        if (s1_node == NULL)
            continue;

        /* Recover software state of S2 scheduler */
        for (index = 0; index < _BCM_TD_NUM_S2_SCHEDULER; index++) {
            if (s2_node[index] == NULL) {
                continue;
            }
            s2_node[index]->parent = s1_node;
            s2_node[index]->sibling = s1_node->child;
            s1_node->child = s2_node[index];
        }

        /* Recover software state of S3 scheduler */
        for (index = 0; index < _BCM_TD_NUM_S3_SCHEDULER; index++) {
            if (s3_node[index] == NULL) {
                continue;
            }
            hw_cosq = index;
            for (s2_index = 0; s2_index < _BCM_TD_NUM_S2_SCHEDULER;
                 s2_index++) {
                if (s2_input[s2_index] & (1 << hw_cosq)) {
                    s3_node[index]->parent = s2_node[s2_index];
                    s3_node[index]->sibling = s2_node[s2_index]->child;
                    s2_node[s2_index]->child = s3_node[index];
                    break;
                }
            }
        }

        /* Recover software state of unicast queue */
        for (index = 0; index < _BCM_TD_NUM_UCAST_QUEUE_GROUP; index++) {
            node = &port_info->ucast[index];
            if (node->numq == 0) {
                continue;
            }
            BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(node->gport, port, index);
            if (node->cosq_attached_to == -1) {
                node->hw_cosq_attached_to = -1;
                continue;
            }
            if (node->base > 7) { /* unicast SC or QM queue */
                node->parent = s1_node;
                node->sibling = s1_node->child;
                s1_node->child = node;
                node->hw_cosq_attached_to = node->base - 3; /* 5 or 6 */
            } else if (uc_to_s2 || node->base >= 4) {
                hw_cosq = node->base + 4;
                for (s2_index = 0; s2_index < _BCM_TD_NUM_S2_SCHEDULER;
                     s2_index++) {
                    if (s2_input[s2_index] & (1 << hw_cosq)) {
                        node->parent = s2_node[s2_index];
                        node->sibling = s2_node[s2_index]->child;
                        s2_node[s2_index]->child = node;
                        node->hw_cosq_attached_to = hw_cosq;
                        break;
                    }
                }
            } else {
                node->parent = s3_node[node->base];
                node->sibling = s3_node[node->base]->child;
                s3_node[node->base]->child = node;
                node->hw_cosq_attached_to = 1;
            }
        }

        /* Recover software state of multicast queue */
        for (index = 0; index < _BCM_TD_NUM_MCAST_QUEUE_GROUP; index++) {
            node = &port_info->mcast[index];
            if (node->numq == 0) {
                continue;
            }
            BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(node->gport, port, index);
            if (node->cosq_attached_to == -1) {
                node->hw_cosq_attached_to = -1;
                continue;
            }
            if (node->base > 3) { /* multicast SC/QM queue */
                node->parent = s1_node;
                node->sibling = s1_node->child;
                s1_node->child = node;
                node->hw_cosq_attached_to = 4;
            } else if (use_mc_group) {
                node->parent = mcg_node;
                node->sibling = mcg_node->child;
                mcg_node->child = node;
                node->hw_cosq_attached_to = node->base;
            } else {
                node->parent = s3_node[node->base];
                node->sibling = s3_node[node->base]->child;
                s3_node[node->base]->child = node;
                node->hw_cosq_attached_to = 0;
            }
        }

        /* Recover software state of extended unicast queue */
        for (index = 0; index < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP; index++) {
            node = &port_info->ext_ucast[index];
            if (node->numq == 0) {
                continue;
            }
            BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET
                (node->gport, port, _BCM_TD_NUM_UCAST_QUEUE_GROUP + index);
            if (node->cosq_attached_to == -1) {
                node->hw_cosq_attached_to = -1;
                continue;
            }
            node->parent = s3_node[node->base];
            node->sibling = s3_node[node->base]->child;
            s3_node[node->base]->child = node;
            node->hw_cosq_attached_to = node->base + 2;
        }
    }

    /* Update PORT_COS_MAP memory profile reference counter */
    profile_mem = _bcm_td_cos_map_profile[unit];
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL, port, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index * 16, 16));
    }
    if (SOC_INFO(unit).cpu_hg_index != -1) {
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, COS_MAP_SELm, MEM_BLOCK_ALL,
                                         SOC_INFO(unit).cpu_hg_index, &entry));
        set_index = soc_mem_field32_get(unit, COS_MAP_SELm, &entry, SELECTf);
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_reference(unit, profile_mem, set_index, 1));
    }

    

    /* Update MMU_WRED_DROP_CURVE_PROFILE_x memory profile reference counter */
    profile_mem = _bcm_td_wred_profile[unit];
    PBMP_PORT_ITER(unit, port) {
        if (SOC_INFO(unit).port_num_ext_cosq[port] == 0) { /* regular port */
            reg = WRED_CONFIGr;
        } else { /* extended queue port */
            reg = DMVOQ_WRED_CONFIGr;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, port, -1,
                                        _BCM_TD_COSQ_INDEX_STYLE_WRED, NULL,
                                        NULL, &numq));
        for (cosq = 0; cosq < numq; cosq++) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                            _BCM_TD_COSQ_INDEX_STYLE_WRED,
                                            NULL, &index, NULL));
            addr = soc_reg_addr(unit, reg, port, index);
            SOC_IF_ERROR_RETURN(soc_reg32_read(unit, addr, &rval));
            set_index = soc_reg_field_get(unit, reg, rval, PROFILE_INDEXf);
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, profile_mem, set_index, 1));
        }
    }

    /* Update MMU_QCN_SITB memory profile and MMU_QCN_CFP_SEQ register profile
     * reference counter */
    profile_mem = _bcm_td_sample_int_profile[unit];
    profile_reg0 = _bcm_td_feedback_profile[unit];
    numq = sizeof(_bcm_td_cpq_en_field) / sizeof(_bcm_td_cpq_en_field[0]);
    PBMP_PORT_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_ENABLEm, MEM_BLOCK_ANY,
                                         mmu_port, &entry));
        for (cosq = 0; cosq < numq; cosq++) {
            if (!soc_mem_field32_get(unit, MMU_QCN_ENABLEm, &entry,
                                     _bcm_td_cpq_en_field[cosq])) {
                continue;
            }
            index = soc_mem_field32_get(unit, MMU_QCN_ENABLEm, &entry,
                                        _bcm_td_cpq_index_field[cosq]);
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_CPQCFGm,
                                             MEM_BLOCK_ANY, mmu_port, &entry));
            set_index = soc_mem_field32_get(unit, MMU_QCN_CPQCFGm, &entry,
                                            _bcm_td_sitb_sel_field[index]);
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_reference(unit, profile_mem, set_index * 64,
                                           64));
            set_index = soc_mem_field32_get(unit, MMU_QCN_CPQCFGm, &entry,
                                            _bcm_td_eqtb_index_field[index]);
            SOC_IF_ERROR_RETURN
                (soc_profile_reg_reference(unit, profile_reg0, set_index, 1));
        }
    }

    /* Update PRIO2COS_LLFC and PRIO2EXTQ_LLFC register profile reference
     * counter */
    profile_reg0 = _bcm_td_llfc_profile[unit];
    profile_reg1 = _bcm_td_ext_llfc_profile[unit];
    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        SOC_IF_ERROR_RETURN(READ_PORT_LLFC_CFGr(unit, port, &rval));
        set_index = soc_reg_field_get(unit, PORT_LLFC_CFGr, rval,
                                      PROFILE_INDEXf);
        SOC_IF_ERROR_RETURN
            (soc_profile_reg_reference(unit, profile_reg0, set_index * 16,
                                       16));
        set_index = soc_reg_field_get(unit, PORT_LLFC_CFGr, rval,
                                      EXT_PER_Q_INDEXf);
        SOC_IF_ERROR_RETURN
            (soc_profile_reg_reference(unit, profile_reg1, set_index * 16,
                                       16));
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *     bcm_td_cosq_init
 * Purpose:
 *     Initialize (clear) all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_init(int unit)
{
    STATIC int _td_max_cosq = -1;
    int cosq, numq, alloc_size, i;
    bcm_port_t port;
    soc_reg_t reg;
    soc_reg_t mem;
    static soc_mem_t wred_mems[6] = {
        MMU_WRED_DROP_CURVE_PROFILE_0m, MMU_WRED_DROP_CURVE_PROFILE_1m,
        MMU_WRED_DROP_CURVE_PROFILE_2m, MMU_WRED_DROP_CURVE_PROFILE_3m,
        MMU_WRED_DROP_CURVE_PROFILE_4m, MMU_WRED_DROP_CURVE_PROFILE_5m
    };
    int entry_words[6];
    mmu_wred_drop_curve_profile_0_entry_t entry_tcp_green;
    mmu_wred_drop_curve_profile_1_entry_t entry_tcp_yellow;
    mmu_wred_drop_curve_profile_2_entry_t entry_tcp_red;
    mmu_wred_drop_curve_profile_3_entry_t entry_nontcp_green;
    mmu_wred_drop_curve_profile_4_entry_t entry_nontcp_yellow;
    mmu_wred_drop_curve_profile_5_entry_t entry_nontcp_red;
    void *entries[6];
    uint64 rval64[16], *rval64s[1];
    uint32 profile_index, rval;
    soc_info_t *si;

    if (_td_max_cosq < 0) {
        _td_max_cosq = NUM_COS(unit);
        NUM_COS(unit) = 8;
    }

    if (!SOC_WARM_BOOT(unit)) {    /* Cold Boot */
        BCM_IF_ERROR_RETURN (bcm_td_cosq_detach(unit, 0));
    }

    numq = soc_property_get(unit, spn_BCM_NUM_COS, BCM_COS_DEFAULT);

    if (numq < 1) {
        numq = 1;
    } else if (numq > 8) {
        numq = 8;
    }

    if (!cosq_sync_lock[unit]) {
        cosq_sync_lock[unit] = sal_mutex_create("TD cosq sync lock");
        if (cosq_sync_lock[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

    si = &SOC_INFO(unit);
    /* Create ETS port information structures */
    alloc_size = sizeof(_bcm_td_cosq_port_info_t) * 66;
    if (_bcm_td_cosq_port_info[unit] == NULL) {
        _bcm_td_cosq_port_info[unit] =
            sal_alloc(alloc_size, "_bcm_td_cosq_port_info");
    }
    sal_memset(_bcm_td_cosq_port_info[unit], 0, alloc_size);

    /* Create profile for PORT_COS_MAP table */
    if (_bcm_td_cos_map_profile[unit] == NULL) {
        _bcm_td_cos_map_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                  "COS_MAP Profile Mem");
        if (_bcm_td_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td_cos_map_profile[unit]);
    }
    mem = PORT_COS_MAPm;
    entry_words[0] = sizeof(port_cos_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, entry_words, 1,
                                               _bcm_td_cos_map_profile[unit]));

    /* Create profile for VOQ_COS_MAP table */
    if (_bcm_td_voq_cos_map_profile[unit] == NULL) {
        _bcm_td_voq_cos_map_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t), "VOQ_COS_MAP Profile Mem");
        if (_bcm_td_voq_cos_map_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td_voq_cos_map_profile[unit]);
    }
    mem = VOQ_COS_MAPm;
    entry_words[0] = sizeof(voq_cos_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, entry_words, 1,
                                _bcm_td_voq_cos_map_profile[unit]));

    /* Create profile for MMU_WRED_DROP_CURVE_PROFILE_x tables */
    if (_bcm_td_wred_profile[unit] == NULL) {
        _bcm_td_wred_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                               "WRED Profile Mem");
        if (_bcm_td_wred_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td_wred_profile[unit]);
    }
    entry_words[0] =
        sizeof(mmu_wred_drop_curve_profile_0_entry_t) / sizeof(uint32);
    entry_words[1] =
        sizeof(mmu_wred_drop_curve_profile_1_entry_t) / sizeof(uint32);
    entry_words[2] =
        sizeof(mmu_wred_drop_curve_profile_2_entry_t) / sizeof(uint32);
    entry_words[3] =
        sizeof(mmu_wred_drop_curve_profile_3_entry_t) / sizeof(uint32);
    entry_words[4] =
        sizeof(mmu_wred_drop_curve_profile_4_entry_t) / sizeof(uint32);
    entry_words[5] =
        sizeof(mmu_wred_drop_curve_profile_5_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, wred_mems, entry_words, 6,
                                _bcm_td_wred_profile[unit]));

    /* Create profile for MMU_QCN_SITB table */
    if (_bcm_td_sample_int_profile[unit] == NULL) {
        _bcm_td_sample_int_profile[unit] =
            sal_alloc(sizeof(soc_profile_mem_t), "QCN sample Int Profile Mem");
        if (_bcm_td_sample_int_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(_bcm_td_sample_int_profile[unit]);
    }
    mem = MMU_QCN_SITBm;
    entry_words[0] = sizeof(mmu_qcn_sitb_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_create(unit, &mem, entry_words, 1,
                                _bcm_td_sample_int_profile[unit]));

    /* Create profile for MMU_QCN_CPQ_SEQ register */
    if (_bcm_td_feedback_profile[unit] == NULL) {
        _bcm_td_feedback_profile[unit] =
            sal_alloc(sizeof(soc_profile_reg_t), "QCN Feedback Profile Reg");
        if (_bcm_td_feedback_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(_bcm_td_feedback_profile[unit]);
    }
    reg = MMU_QCN_CPQ_SEQr;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_create(unit, &reg, 1,
                                _bcm_td_feedback_profile[unit]));

    /* Create profile for PRIO2COS_LLFC register */
    if (_bcm_td_llfc_profile[unit] == NULL) {
        _bcm_td_llfc_profile[unit] =
            sal_alloc(sizeof(soc_profile_reg_t), "PRIO2COS_LLFC Profile Reg");
        if (_bcm_td_llfc_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(_bcm_td_llfc_profile[unit]);
    }
    reg = PRIO2COS_LLFCr;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_create(unit, &reg, 1, _bcm_td_llfc_profile[unit]));

    /* Create profile for PRIO2EXTQ_LLFC register */
    if (_bcm_td_ext_llfc_profile[unit] == NULL) {
        _bcm_td_ext_llfc_profile[unit] =
            sal_alloc(sizeof(soc_profile_reg_t), "PRIO2EXTQ_LLFC Profile Reg");
        if (_bcm_td_ext_llfc_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_reg_t_init(_bcm_td_ext_llfc_profile[unit]);
    }
    reg = PRIO2EXTQ_LLFCr;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_create(unit, &reg, 1,
                                _bcm_td_ext_llfc_profile[unit]));

    for (i = 0; i < si->num_cpu_cosq; i++) {
        if (_bcm_td_cosq_cpu_cosq_config[unit][i] == NULL) {
            _bcm_td_cosq_cpu_cosq_config[unit][i] =
                sal_alloc(sizeof(_bcm_td_cosq_cpu_cosq_config_t), "CPU Cosq Config");

            if (_bcm_td_cosq_cpu_cosq_config[unit][i] == NULL) {
                return BCM_E_MEMORY;
            }
            sal_memset(_bcm_td_cosq_cpu_cosq_config[unit][i], 0,
                       sizeof(_bcm_td_cosq_cpu_cosq_config_t));
            _bcm_td_cosq_cpu_cosq_config[unit][i]->enable = 1;
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return bcm_td_cosq_reinit(unit);
    } else {
        BCM_IF_ERROR_RETURN(_bcm_td_cosq_wb_alloc(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Add default entries for PORT_COS_MAP memory profile */
    BCM_IF_ERROR_RETURN(_bcm_td_cosq_config_set(unit, numq));

    /* Add default entries for VOQ_COS_MAP memory profile */
    PBMP_PORT_ITER(unit, port) {
        if (!_BCM_TD_IS_PORT_DMVOQ_CAPABLE(unit, port)) {
            continue;
        }
        BCM_IF_ERROR_RETURN(_bcm_td_cosq_voq_init(unit, port));
    }

    /* Add default entries for MMU_WRED_DROP_CURVE_PROFILE_x memory profile */
    sal_memset(&entry_tcp_green, 0, sizeof(entry_tcp_green));
    sal_memset(&entry_tcp_yellow, 0, sizeof(entry_tcp_yellow));
    sal_memset(&entry_tcp_red, 0, sizeof(entry_tcp_red));
    sal_memset(&entry_nontcp_green, 0, sizeof(entry_nontcp_green));
    sal_memset(&entry_nontcp_yellow, 0, sizeof(entry_nontcp_yellow));
    sal_memset(&entry_nontcp_red, 0, sizeof(entry_nontcp_red));
    entries[0] = &entry_tcp_green;
    entries[1] = &entry_tcp_yellow;
    entries[2] = &entry_tcp_red;
    entries[3] = &entry_nontcp_green;
    entries[4] = &entry_nontcp_yellow;
    entries[5] = &entry_nontcp_red;
    for (i = 0; i < 6; i++) {
        soc_mem_field32_set(unit, wred_mems[i], entries[i],
                            MIN_THDf, 0xffff);
        soc_mem_field32_set(unit, wred_mems[i], entries[i],
                            MAX_THDf, 0xffff);
    }
    profile_index = 0xffffffff;
    PBMP_PORT_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, port, -1,
                                        _BCM_TD_COSQ_INDEX_STYLE_WRED, NULL,
                                        NULL, &numq));
        for (cosq = 0; cosq < numq; cosq++) {
            if (profile_index == 0xffffffff) {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit, _bcm_td_wred_profile[unit],
                                         entries, 1, &profile_index));
            } else {
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_reference(unit,
                                               _bcm_td_wred_profile[unit],
                                               profile_index, 0));
            }
        }
    }

    /* Add default entries for PRIO2COS_LLFC register profile */
    sal_memset(rval64, 0, sizeof(rval64));
    rval64s[0] = rval64;
    profile_index = 0xffffffff;
    PBMP_PORT_ITER(unit, port) {
        if (profile_index == 0xffffffff) {
            BCM_IF_ERROR_RETURN
                (soc_profile_reg_add(unit, _bcm_td_llfc_profile[unit],
                                     rval64s, 16, &profile_index));
        } else {
            BCM_IF_ERROR_RETURN
                (soc_profile_reg_reference(unit, _bcm_td_llfc_profile[unit],
                                           profile_index, 0));
        }
    }

    /* Add default entries for PRIO2EXTQ_LLFC register profile */
    profile_index = 0xffffffff;
    PBMP_PORT_ITER(unit, port) {
        if (profile_index == 0xffffffff) {
            BCM_IF_ERROR_RETURN
                (soc_profile_reg_add(unit, _bcm_td_ext_llfc_profile[unit],
                                     rval64s, 16, &profile_index));
        } else {
            BCM_IF_ERROR_RETURN
                (soc_profile_reg_reference(unit,
                                           _bcm_td_ext_llfc_profile[unit],
                                           profile_index, 0));
        }
    }

    /*
     * Always strict priority for SC and QM COS queues
     * S1 COS4 is for multicast SC and QM queue
     * S1 COS5 is for unicast SC queue
     * S1 COS6 is for unicast QM queue
     */
    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        BCM_IF_ERROR_RETURN(READ_MINSPCONFIGr(unit, port, &rval));
        soc_reg_field_set(unit, MINSPCONFIGr, &rval, COS4_IS_SPf, 1);
        soc_reg_field_set(unit, MINSPCONFIGr, &rval, COS5_IS_SPf, 1);
        soc_reg_field_set(unit, MINSPCONFIGr, &rval, COS6_IS_SPf, 1);
        BCM_IF_ERROR_RETURN(WRITE_MINSPCONFIGr(unit, port, rval));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_detach
 * Purpose:
 *     Discard all COS schedule/mapping state.
 * Parameters:
 *     unit - unit number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_detach(int unit, int software_state_only)
{
    int i;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    /* Cleanup ETS port information structures */
    if (_bcm_td_cosq_port_info[unit] != NULL) {
        sal_free(_bcm_td_cosq_port_info[unit]);
        _bcm_td_cosq_port_info[unit] = NULL;
    }

    /* Cleanup profile for PORT_COS_MAP table */
    if (_bcm_td_cos_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td_cos_map_profile[unit]));
        sal_free(_bcm_td_cos_map_profile[unit]);
        _bcm_td_cos_map_profile[unit] = NULL;
    }

    /* Cleanup profile for VOQ_COS_MAP table */
    if (_bcm_td_voq_cos_map_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td_voq_cos_map_profile[unit]));
        sal_free(_bcm_td_voq_cos_map_profile[unit]);
        _bcm_td_voq_cos_map_profile[unit] = NULL;
    }

    /* Cleanup profile for MMU_WRED_DROP_CURVE_PROFILE_x tables */
    if (_bcm_td_wred_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td_wred_profile[unit]));
        sal_free(_bcm_td_wred_profile[unit]);
        _bcm_td_wred_profile[unit] = NULL;
    }

    /* Cleanup profile for MMU_QCN_SITB table */
    if (_bcm_td_sample_int_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_mem_destroy(unit,
                                _bcm_td_sample_int_profile[unit]));
        sal_free(_bcm_td_sample_int_profile[unit]);
        _bcm_td_sample_int_profile[unit] = NULL;
    }

    /* Cleanup profile for MMU_QCN_CPQ_SEQ register */
    if (_bcm_td_feedback_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_reg_destroy(unit,
                                _bcm_td_feedback_profile[unit]));
        sal_free(_bcm_td_feedback_profile[unit]);
        _bcm_td_feedback_profile[unit] = NULL;
    }

    /* Cleanup profile for PRIO2COS_LLFC register */
    if (_bcm_td_llfc_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_reg_destroy(unit,
                                _bcm_td_llfc_profile[unit]));
        sal_free(_bcm_td_llfc_profile[unit]);
        _bcm_td_llfc_profile[unit] = NULL;
    }

    /* Cleanup profile for PRIO2EXTQ_LLFC register */
    if (_bcm_td_ext_llfc_profile[unit] != NULL) {
        SOC_IF_ERROR_RETURN(soc_profile_reg_destroy(unit,
                                _bcm_td_ext_llfc_profile[unit]));
        sal_free(_bcm_td_ext_llfc_profile[unit]);
        _bcm_td_ext_llfc_profile[unit] = NULL;
    }

    for (i = 0; i < si->num_cpu_cosq; i++) {
        if (_bcm_td_cosq_cpu_cosq_config[unit][i] != NULL) {
            sal_free(_bcm_td_cosq_cpu_cosq_config[unit][i]);
            _bcm_td_cosq_cpu_cosq_config[unit][i] = NULL;
        }
    }

    if (cosq_sync_lock[unit]) {
        sal_mutex_destroy(cosq_sync_lock[unit]);
        cosq_sync_lock[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_config_set
 * Purpose:
 *     Set the number of COS queues
 * Parameters:
 *     unit - unit number
 *     numq - number of COS queues (1-8).
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_config_set(int unit, int numq)
{
    soc_mem_t mem;
    int entry_words;

    mem = PORT_COS_MAPm;
    entry_words = sizeof(port_cos_map_entry_t) / sizeof(uint32);
    BCM_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                               _bcm_td_cos_map_profile[unit]));

    return _bcm_td_cosq_config_set(unit, numq);
}

/*
 * Function:
 *     bcm_td_cosq_config_get
 * Purpose:
 *     Get the number of cos queues
 * Parameters:
 *     unit - unit number
 *     numq - (Output) number of cosq
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_config_get(int unit, int *numq)
{
    if (_bcm_td_num_cosq[unit] == 0) {
        return BCM_E_INIT;
    }

    if (numq != NULL) {
        *numq = _bcm_td_num_cosq[unit];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_mapping_set
 * Purpose:
 *     Set which cosq a given priority should fall into
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) queue group GPORT identifier
 *     priority - (IN) priority value to map
 *     cosq     - (IN) COS queue to map to
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_mapping_set(int unit, bcm_port_t port, bcm_cos_t priority,
                        bcm_cos_queue_t cosq)
{
    bcm_pbmp_t pbmp;
    bcm_port_t local_port;
    bcm_cos_queue_t  mc_cosq_max, mc_cosq;

    mc_cosq_max = (_BCM_TD_NUM_MCAST_QUEUE_GROUP - 1);

    BCM_PBMP_CLEAR(pbmp);
    if (port == -1) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port)) {
                return BCM_E_PARAM;
            } else if (BCM_GPORT_IS_LOCAL(port)) {
                local_port = BCM_GPORT_LOCAL_GET(port);
            } else {
                return BCM_E_PARAM;
            }
        } else {
            local_port = port;
        }
        if (!SOC_PORT_VALID(unit, local_port)) {
            return BCM_E_PORT;
        }
        BCM_PBMP_PORT_ADD(pbmp, local_port);
    }
	  	
    if ((cosq < 0) || (cosq >= _TD_NUM_COS(unit))) {
        return BCM_E_PARAM;
    }
    PBMP_ITER(pbmp, local_port) {
    if (IS_LB_PORT(unit, local_port)) {
        continue;
    }
    /* If no gport, map the int prio symmetrically for ucast and
    * mcast 
    * There are only 5 MC QUEUES [0,4] attached to a port
    */
    mc_cosq = (cosq > mc_cosq_max) ? mc_cosq_max : cosq;
    BCM_IF_ERROR_RETURN(_bcm_td_cosq_mapping_set(unit, local_port,
        priority,
        BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
        -1, (cosq << 16 | (mc_cosq & 0xffff))));
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *     bcm_td_cosq_mapping_get
 * Purpose:
 *     Determine which COS queue a given priority currently maps to.
 * Parameters:
 *     unit     - (IN) unit number
 *     gport    - (IN) queue group GPORT identifier
 *     priority - (IN) priority value to map
 *     cosq     - (OUT) COS queue to map to
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_mapping_get(int unit, bcm_port_t port, bcm_cos_t priority,
                        bcm_cos_queue_t *cosq)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);

    if (port == -1) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    } else {
        if (BCM_GPORT_IS_SET(port)) {
            if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_MCAST_QUEUE_GROUP(port) ||
                BCM_GPORT_IS_SCHEDULER(port)) {
                return BCM_E_PARAM;
            } else if (BCM_GPORT_IS_LOCAL(port)) {
                local_port = BCM_GPORT_LOCAL_GET(port);
            } else {
                return BCM_E_PARAM;
            }
        } else {
            local_port = port;
        }
        if (!SOC_PORT_VALID(unit, local_port)) {
            return BCM_E_PORT;
        }
        BCM_PBMP_PORT_ADD(pbmp, local_port);
    }

    PBMP_ITER(pbmp, local_port) {
        if (IS_LB_PORT(unit, local_port)) {
            continue;
        }
        BCM_IF_ERROR_RETURN(_bcm_td_cosq_mapping_get(unit, local_port, 
               priority, BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, NULL, cosq));
        break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_port_sched_set
 * Purpose:
 *     Set up class-of-service policy and corresponding weights and delay
 * Parameters:
 *     unit    - (IN) unit number
 *     pbm     - (IN) port bitmap
 *     mode    - (IN) Scheduling mode (BCM_COSQ_xxx)
 *     weights - (IN) Weights for each COS queue
 *     delay   - This parameter is not used
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_port_sched_set(int unit, bcm_pbmp_t pbm,
                           int mode, const int *weights, int delay)
{
    bcm_port_t port;
    int num_weights;

    BCM_PBMP_ITER(pbm, port) {
        /* coverity[overrun-local : FALSE] */
        num_weights = _TD_NUM_COS(unit);
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_sched_set(unit, port, 0, mode, num_weights,
                                    (int *)weights));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_port_sched_get
 * Purpose:
 *     Retrieve class-of-service policy and corresponding weights and delay
 * Parameters:
 *     unit     - (IN) unit number
 *     pbm      - (IN) port bitmap
 *     mode     - (OUT) Scheduling mode (BCM_COSQ_XXX)
 *     weights  - (OUT) Weights for each COS queue
 *     delay    - This parameter is not used
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_port_sched_get(int unit, bcm_pbmp_t pbm,
                           int *mode, int *weights, int *delay)
{
    bcm_port_t port;

    BCM_PBMP_ITER(pbm, port) {
        /* coverity[overrun-local : FALSE] */
        /* coverity[illegal_address : FALSE] */
        if (IS_CPU_PORT(unit, port) && SOC_PBMP_NEQ(pbm, PBMP_CMIC(unit))) {
            continue;
        }
        return _bcm_td_cosq_sched_get(unit, port, 0, mode, 8, weights);
    }

    return BCM_E_PORT;
}

/*
 * Function:
 *     bcm_td_cosq_sched_weight_max_get
 * Purpose:
 *     Retrieve maximum weights for given COS policy
 * Parameters:
 *     unit    - (IN) unit number
 *     mode    - (IN) Scheduling mode (BCM_COSQ_xxx)
 *     weight_max - (OUT) Maximum weight for COS queue.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    switch (mode) {
    case BCM_COSQ_STRICT:
        *weight_max = BCM_COSQ_WEIGHT_STRICT;
        break;
    case BCM_COSQ_ROUND_ROBIN:
        *weight_max = BCM_COSQ_WEIGHT_MIN;
        break;
    case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
    case BCM_COSQ_DEFICIT_ROUND_ROBIN:
        *weight_max =
            (1 << soc_reg_field_length(unit, COSWEIGHTSr, COSWEIGHTSf)) - 1;
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_bandwidth_set
 * Purpose:
 *     Configure COS queue bandwidth control
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_quantum   - (IN)
 *     max_quantum   - (IN)
 *     burst_quantum - (IN)
 *     flags         - (IN)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
int
bcm_td_cosq_port_bandwidth_set(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 min_quantum, uint32 max_quantum,
                               uint32 burst_quantum, uint32 flags)
{
    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    return _bcm_td_cosq_bucket_set(unit, port, cosq, min_quantum,
                                   max_quantum, min_quantum, max_quantum,
                                   flags);
}

/*
 * Function:
 *     bcm_td_cosq_bandwidth_get
 * Purpose:
 *     Get COS queue bandwidth control configuration
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     min_quantum   - (OUT)
 *     max_quantum   - (OUT)
 *     burst_quantum - (OUT)
 *     flags         - (OUT)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
int
bcm_td_cosq_port_bandwidth_get(int unit, bcm_port_t port,
                               bcm_cos_queue_t cosq,
                               uint32 *min_quantum, uint32 *max_quantum,
                               uint32 *burst_quantum, uint32 *flags)
{
    uint32 tmp;

    if (cosq < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td_cosq_bucket_get(unit, port, cosq, min_quantum,
                                                max_quantum, &tmp, 
                                                burst_quantum, flags));

    return BCM_E_NONE;
}

int
bcm_td_cosq_port_pps_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int pps)
{
    uint32 min, max, burst, flags;
    
    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_bucket_get(unit, port, cosq, &min, &max, &burst, &burst,
                                 &flags));

    

    /* Replace the current PPS setting, keep BURST the same */
    return _bcm_td_cosq_bucket_set(unit, port, cosq, min, pps, burst, burst,
                                   flags | BCM_COSQ_BW_PACKET_MODE);
}

int
bcm_td_cosq_port_pps_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                         int *pps)
{
    uint32 min, max, burst, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_bucket_get(unit, port, cosq, &min, &max, &burst, &burst,
                                 &flags));
    *pps = max;

    return BCM_E_NONE;
}

int
bcm_td_cosq_port_burst_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int burst)
{
    uint32 min, max, cur_burst, flags;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    /* Get the current PPS and BURST settings */
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_bucket_get(unit, port, cosq, &min, &max, &cur_burst,
                                 &cur_burst, &flags));

    

    /* Replace the current BURST setting, keep PPS the same */
    return _bcm_td_cosq_bucket_set(unit, port, cosq, min, max, burst, burst,
                                   flags | BCM_COSQ_BW_PACKET_MODE);
}

int
bcm_td_cosq_port_burst_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                           int *burst)
{
    uint32 min, max, cur_burst, flags, tmp;

    if (!IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    } else if (cosq < 0 || cosq >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_bucket_get(unit, port, cosq, &min, &max, &tmp, 
                                 &cur_burst, &flags));
    *burst = cur_burst;

    return BCM_E_NONE;
}

int
bcm_td_cosq_discard_set(int unit, uint32 flags)
{
    bcm_port_t port;
    bcm_cos_queue_t cosq;
    int numq;

    if (flags & ~(BCM_COSQ_DISCARD_DEVICE |
                  BCM_COSQ_DISCARD_NONTCP |
                  BCM_COSQ_DISCARD_COLOR_ALL |
                  BCM_COSQ_DISCARD_PORT |
                  BCM_COSQ_DISCARD_TCP |
                  BCM_COSQ_DISCARD_COLOR_GREEN |
                  BCM_COSQ_DISCARD_COLOR_YELLOW |
                  BCM_COSQ_DISCARD_COLOR_RED |
                  BCM_COSQ_DISCARD_CAP_AVERAGE |
                  BCM_COSQ_DISCARD_ENABLE |
                  BCM_COSQ_DISCARD_MARK_CONGESTION)) {
        return BCM_E_PARAM;
    }

    flags &= ~(BCM_COSQ_DISCARD_NONTCP | BCM_COSQ_DISCARD_COLOR_ALL);

    PBMP_PORT_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, port, -1,
                                        _BCM_TD_COSQ_INDEX_STYLE_WRED, NULL,
                                        NULL, &numq));
        for (cosq = 0; cosq < numq; cosq++) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_wred_set(unit, port, cosq, flags, 0, 0, 0, 0,
                                       FALSE));
        }
    }

    return BCM_E_NONE;
}

int
bcm_td_cosq_discard_get(int unit, uint32 *flags)
{
    bcm_port_t port;

    PBMP_PORT_ITER(unit, port) {
        *flags = 0;
        /* use setting from hardware cosq index 0 of the first port */
        return _bcm_td_cosq_wred_get(unit, port, 0, flags, NULL, NULL, NULL,
                                     NULL);
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_td_cosq_discard_port_set
 * Purpose:
 *     Configure port WRED setting
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     color         - (IN)
 *     drop_start    - (IN)
 *     drop_slot     - (IN)
 *     average_time  - (IN)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
int
bcm_td_cosq_discard_port_set(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int drop_start, int drop_slope,
                             int average_time)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int gain;
    uint32 min_thresh, max_thresh, shared_limit;
    uint32 rval, bits;
    int numq, i;

    if (drop_start < 0 || drop_start > 100 ||
        drop_slope < 0 || drop_slope > 90) {
        return BCM_E_PARAM;
    }

    /*
     * average queue size is reculated every 4us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     * gain = log2(average_time / 4)
     */
    bits = (average_time / 4) & 0xffff;
    if (bits != 0) {
        bits |= bits >> 1;
        bits |= bits >> 2;
        bits |= bits >> 4;
        bits |= bits >> 8;
        gain = _shr_popcount(bits) - 1;
    } else {
        gain = 0;
    }

    /*
     * per-port cell limit may be disabled.
     * per-port per-cos cell limit may be set to use dynamic method.
     * therefore drop_start percentage is based on per-device total shared
     * cells.
     */
    BCM_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_CELLr(unit, &rval));
    shared_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMIT_CELLr,
                                     rval, OP_BUFFER_SHARED_LIMIT_CELLf);
    min_thresh = drop_start * shared_limit / 100;

    /* Calculate the max threshold. For a given slope (angle in
     * degrees), determine how many packets are in the range from
     * 0% drop probability to 100% drop probability. Add that
     * number to the min_treshold to the the max_threshold.
     */
    max_thresh = min_thresh + _bcm_td_angle_to_cells(drop_slope);
    if (max_thresh > TD_CELL_FIELD_MAX) {
        max_thresh = TD_CELL_FIELD_MAX;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (cosq == -1) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                            _BCM_TD_COSQ_INDEX_STYLE_WRED,
                                            NULL, NULL, &numq));
            cosq = 0;
        } else {
            numq = 1;
        }
        for (i = 0; i < numq; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_wred_set(unit, port, cosq + i, color,
                                       min_thresh, max_thresh, 100, gain,
                                       TRUE));
        }
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }

        BCM_PBMP_ITER(pbmp, local_port) {
            if (cosq == -1) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve(unit, local_port, cosq,
                                                _BCM_TD_COSQ_INDEX_STYLE_WRED,
                                                NULL, NULL, &numq));
                cosq = 0;
            } else {
                numq = 1;
            }
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_wred_set(unit, local_port, cosq + i,
                                           color, min_thresh, max_thresh, 100,
                                           gain, TRUE));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_discard_port_get
 * Purpose:
 *     Get port WRED setting
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     color         - (OUT)
 *     drop_start    - (OUT)
 *     drop_slot     - (OUT)
 *     average_time  - (OUT)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     If port is any form of local port, cosq is the hardware queue index.
 */
int
bcm_td_cosq_discard_port_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                             uint32 color, int *drop_start, int *drop_slope,
                             int *average_time)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;
    int gain, drop_prob;
    uint32 min_thresh, max_thresh, shared_limit;
    uint32 rval;

    if (drop_start == NULL || drop_slope == NULL || average_time == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        local_port = port;
    } else {
        if (port == -1) {
            BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));
        } else if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        } else {
            BCM_PBMP_PORT_SET(pbmp, port);
        }
        BCM_PBMP_ITER(pbmp, local_port) {
            break;
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_wred_get(unit, local_port, cosq == -1 ? 0 : cosq,
                               &color, &min_thresh, &max_thresh, &drop_prob,
                               &gain));

    /*
     * average queue size is reculated every 4us, the formula is
     * avg_qsize(t + 1) =
     *     avg_qsize(t) + (cur_qsize - avg_qsize(t)) / (2 ** gain)
     */
    *average_time = (1 << gain) * 4;

    /*
     * per-port cell limit may be disabled.
     * per-port per-cos cell limit may be set to use dynamic method.
     * therefore drop_start percentage is based on per-device total shared
     * cells.
     */
    BCM_IF_ERROR_RETURN(READ_OP_BUFFER_SHARED_LIMIT_CELLr(unit, &rval));
    shared_limit = soc_reg_field_get(unit, OP_BUFFER_SHARED_LIMIT_CELLr,
                                     rval, OP_BUFFER_SHARED_LIMIT_CELLf);
    if (min_thresh > shared_limit) {
        min_thresh = shared_limit;
    }

    if (max_thresh > shared_limit) {
        max_thresh = shared_limit;
    }

    *drop_start = min_thresh * 100 / shared_limit;

    /* Calculate the slope using the min and max threshold.
     * The angle is calculated knowing drop probability at min
     * threshold is 0% and drop probability at max threshold is 100%.
     */
    *drop_slope = _bcm_td_cells_to_angle(max_thresh - min_thresh);

    return BCM_E_NONE;
}

int
bcm_td_cosq_stat_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, uint64 value)
{
    soc_info_t *si;
    _bcm_td_cosq_node_t *node;
    bcm_port_t local_port;
    int startq, numq, i;

    si = &SOC_INFO(unit);

    switch (stat) {
    case bcmCosqStatDroppedPackets:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP,
                  &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC, startq,
                                 value));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_PKT, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, &numq));
            if (!IS_CPU_PORT(unit, local_port) && cosq != BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            if (cosq == BCM_COS_INVALID) {
                numq = 1;
            }
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                     startq + i, value));
            }
            if (si->port_num_uc_cosq[local_port] > 0) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP,
                      &local_port, &startq, &numq));
                if (cosq == BCM_COS_INVALID) {
                    numq = 1;
                }
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                         startq + i, value));
                }
            }
        }
        break;
    case bcmCosqStatDroppedBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP,
                  &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC, startq,
                                 value));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE, startq,
                                 value));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, &numq));
            if (!IS_CPU_PORT(unit, local_port) && cosq != BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            if (cosq == BCM_COS_INVALID) {
                numq = 1;
            }
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE,
                                     startq + i, value));
            }
            if (si->port_num_uc_cosq[local_port] > 0) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP,
                      &local_port, &startq, &numq));
                if (cosq == BCM_COS_INVALID) {
                    numq = 1;
                }
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC,
                                         startq + i, value));
                }
            }
        }
        break;
    case bcmCosqStatYellowCongestionDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, 0,
                             value));
        break;
    case bcmCosqStatRedCongestionDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED, 0,
                             value));
        break;
    case bcmCosqStatGreenDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN, 0,
                             value));
        break;
    case bcmCosqStatYellowDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW, 0,
                             value));
        break;
    case bcmCosqStatRedDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED, 0, value));
        break;
    case bcmCosqStatOutPackets:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_node_get(unit, port, NULL,
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
            if (BCM_GPORT_IS_SCHEDULER(port)) {
               /* Not support more than one-layer scheduler node analysis*/
               return BCM_E_PARAM;
            }
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            if (BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(port) <
                _BCM_TD_NUM_UCAST_QUEUE_GROUP) { /* regular ucast queue */
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      &local_port, &startq, NULL));
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC,
                                     startq, value));
            } else { /* extended ucast queue */
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq,
                      _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
                      NULL, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td_cosq_index_resolve
                         (unit, port, cosq < 0 ? i : cosq,
                          _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
                          &local_port, &startq, NULL));
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set
                         (unit, local_port,
                          SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT,
                          startq, value));
                }
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, startq,
                                 value));
        } else {
            if (cosq < BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
            if (cosq == BCM_COS_INVALID) {
                /* UC queue */
                for (i = 0; i < si->port_num_uc_cosq[local_port]; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC,
                                         i, value));
                }
                /* MC queue */
                for (i = 0; i < si->port_num_cosq[local_port]; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                         i, value));
                }
            } else {
                /*
                 * cosq=0 for ucast queue 0 + mcast queue 0
                 * cosq=1 for ucast queue 1 + mcast queue 1
                 * cosq=2 for ucast queue 2 + mcast queue 2
                 * cosq=3 for ucast queue 3 + mcast queue 3
                 * cosq=4 for ucast queue 4
                 * cosq=5 for ucast queue 5
                 * cosq=6 for ucast queue 6
                 * cosq=7 for ucast queue 7
                 * cosq=8 for unicast SC queue + multicast SC/QM queue
                 * cosq=9 for unicast QM queue.
                 */
                if (IS_CPU_PORT(unit, local_port) ||
                    IS_LB_PORT(unit, local_port)) {
                    if (cosq >= (si->port_num_cosq[local_port])) {
                        return BCM_E_PARAM;
                    }
                } else if (cosq >= (si->port_num_uc_cosq[local_port])){
                    return BCM_E_PARAM;
                }

                if (IS_CPU_PORT(unit, local_port) ||
                    IS_LB_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                     cosq, value));
                } else {
                    /* UC queue */
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC,
                                     cosq, value));
                    /* MC queue */
                    if ((cosq < (si->port_num_cosq[local_port] - 1)) ||
                        (cosq == 8)) {
                        cosq = (cosq == 8) ? 4 : cosq;
                        BCM_IF_ERROR_RETURN
                            (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                         cosq, value));
                    }
                }
            }
        }
        break;
    case bcmCosqStatOutBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_node_get(unit, port, NULL,
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
            if (BCM_GPORT_IS_SCHEDULER(port)) {
               /* Not support more than one-layer scheduler node analysis*/
               return BCM_E_PARAM;
            }
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            if (BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(port) <
                _BCM_TD_NUM_UCAST_QUEUE_GROUP) { /* regular ucast queue */
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      &local_port, &startq, NULL));
                BCM_IF_ERROR_RETURN
                    (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC,
                                     startq, value));
            } else { /* extended ucast queue */
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq,
                      _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
                      NULL, NULL, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td_cosq_index_resolve
                         (unit, port, cosq < 0 ? i : cosq,
                          _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
                          &local_port, &startq, NULL));
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set
                         (unit, local_port,
                          SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT,
                          startq, value));
                }
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (soc_counter_set(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, startq,
                                 value));
        } else {
            if (cosq < BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
            if (cosq == BCM_COS_INVALID) {
                /* UC queue */
                for (i = 0; i < si->port_num_uc_cosq[local_port]; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC,
                                         i, value));
                }
                /* MC queue */
                for (i = 0; i < si->port_num_cosq[local_port]; i++) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                         i, value));
                }
            } else {
                /*
                 * cosq=0 for ucast queue 0 + mcast queue 0
                 * cosq=1 for ucast queue 1 + mcast queue 1
                 * cosq=2 for ucast queue 2 + mcast queue 2
                 * cosq=3 for ucast queue 3 + mcast queue 3
                 * cosq=4 for ucast queue 4
                 * cosq=5 for ucast queue 5
                 * cosq=6 for ucast queue 6
                 * cosq=7 for ucast queue 7
                 * cosq=8 for unicast SC queue + multicast SC/QM queue
                 * cosq=9 for unicast QM queue.
                 */
                if (IS_CPU_PORT(unit, local_port) ||
                    IS_LB_PORT(unit, local_port)) {
                    if (cosq >= (si->port_num_cosq[local_port])) {
                        return BCM_E_PARAM;
                    }
                } else if (cosq >= (si->port_num_uc_cosq[local_port])){
                    return BCM_E_PARAM;
                }

                if (IS_CPU_PORT(unit, local_port) ||
                    IS_LB_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                     cosq, value));
                } else {
                    /* UC queue */
                    BCM_IF_ERROR_RETURN
                        (soc_counter_set(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC,
                                     cosq, value));
                    /* MC queue */
                    if ((cosq < (si->port_num_cosq[local_port] - 1)) ||
                        (cosq == 8)) {
                        cosq = (cosq == 8) ? 4 : cosq;
                        BCM_IF_ERROR_RETURN
                            (soc_counter_set(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                         cosq, value));
                    }
                }
            }
        }
        break;
    case bcmCosqStatIeee8021CnCpTransmittedCnms:
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  &local_port, NULL, NULL));
        BCM_IF_ERROR_RETURN
            (bcm_td_cosq_congestion_queue_get(unit, port, cosq, &startq));
                  
        BCM_IF_ERROR_RETURN
            (soc_counter_set(unit, local_port,
                             SOC_COUNTER_NON_DMA_MMU_QCN_CNM, startq, value)); 
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

int
bcm_td_cosq_stat_get(int unit, bcm_port_t port, bcm_cos_queue_t cosq,
                     bcm_cosq_stat_t stat, int sync_mode, uint64 *value)
{
    soc_info_t *si;
    _bcm_td_cosq_node_t *node;
    bcm_port_t local_port;
    int startq, numq, i, start_hw_index, end_hw_index;
    uint64 sum, value64;
    int (*counter_get) (int , soc_port_t , soc_reg_t , int , uint64 *);

    if (value == NULL) {
        return BCM_E_PARAM;
    }

    /*
     * if sync-mode is set, update the software cached counter value, 
     * with the hardware count and then retrieve the count.
     * else return the software cache counter value.
     */
    counter_get = (sync_mode == 1)? soc_counter_sync_get: soc_counter_get;

    si = &SOC_INFO(unit);

    switch (stat) {
    case bcmCosqStatDroppedPackets:
        COMPILER_64_ZERO(sum);
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP,
                  &local_port, &startq, &numq));
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                     startq + i, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, &numq));
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                     startq + i, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, &numq));
            if (!IS_CPU_PORT(unit, local_port) && cosq != BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                     startq + i, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }
            if (si->port_num_uc_cosq[local_port] > 0) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                         startq + i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            }
        }
        *value = sum;
        break;
    case bcmCosqStatDroppedBytes:
        COMPILER_64_ZERO(sum);
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP,
                  &local_port, &startq, &numq));
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC,
                                     startq + i, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, &numq));
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE,
                                     startq + i, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, &numq));
            if (!IS_CPU_PORT(unit, local_port) && cosq != BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < numq; i++) {
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE,
                                     startq + i, &value64));
                COMPILER_64_ADD_64(sum, value64);
            }
            if (si->port_num_uc_cosq[local_port] > 0) {
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_DROP,
                      &local_port, &startq, &numq));
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC,
                                         startq + i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            }
        }
        *value = sum;
        break;
    case bcmCosqStatYellowCongestionDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW, 0,
                             value));
        break;
    case bcmCosqStatRedCongestionDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED, 0,
                             value));
        break;
    case bcmCosqStatGreenDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN, 0,
                             value));
        break;
    case bcmCosqStatYellowDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW, 0,
                             value));
        break;
    case bcmCosqStatRedDiscardDroppedPackets:
        if (cosq != BCM_COS_INVALID) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED, 0, value));
        break;
    case bcmCosqStatOutPackets:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_node_get(unit, port, NULL,
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
            if (BCM_GPORT_IS_SCHEDULER(port)) {
               /* Not support more than one-layer scheduler node analysis*/
               return BCM_E_PARAM;
            }
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            if (BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(port) <
                _BCM_TD_NUM_UCAST_QUEUE_GROUP) { /* regular ucast queue */
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      &local_port, &startq, NULL));
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC,
                                     startq, value));
            } else { /* extended ucast queue */
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq,
                      _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
                      NULL, NULL, &numq));
                COMPILER_64_ZERO(sum);
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td_cosq_index_resolve
                         (unit, port, cosq < 0 ? i : cosq,
                          _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
                          &local_port, &startq, NULL));
                    BCM_IF_ERROR_RETURN
                        (counter_get
                         (unit, local_port,
                          SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_EXT,
                          startq, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
                *value = sum;
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT, startq,
                                 value));
        } else {
            if (cosq < BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                /* UC queue */
                for (i = 0; i < si->port_num_uc_cosq[local_port]; i++) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC,
                                     i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
                /* MC queue */
                for (i = 0; i < si->port_num_cosq[local_port]; i++) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                         i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            } else {
                /*
                 * cosq=0 for ucast queue 0 + mcast queue 0
                 * cosq=1 for ucast queue 1 + mcast queue 1
                 * cosq=2 for ucast queue 2 + mcast queue 2
                 * cosq=3 for ucast queue 3 + mcast queue 3
                 * cosq=4 for ucast queue 4
                 * cosq=5 for ucast queue 5
                 * cosq=6 for ucast queue 6
                 * cosq=7 for ucast queue 7
                 * cosq=8 for unicast SC queue + multicast SC/QM queue
                 * cosq=9 for unicast QM queue.
                 */
                if (IS_CPU_PORT(unit, local_port) ||
                    IS_LB_PORT(unit, local_port)) {
                    if (cosq >= (si->port_num_cosq[local_port])) {
                        return BCM_E_PARAM;
                    }
                } else if (cosq >= (si->port_num_uc_cosq[local_port])){
                    return BCM_E_PARAM;
                }

                if (IS_CPU_PORT(unit, local_port) ||
                    IS_LB_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                     cosq, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                } else {
                    /* UC queue */
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC,
                                     cosq, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                    /* MC queue */
                    if ((cosq < (si->port_num_cosq[local_port] - 1)) ||
                        (cosq == 8)){
                        cosq = (cosq == 8) ? 4 : cosq;
                        BCM_IF_ERROR_RETURN
                            (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
                                         cosq, &value64));
                        COMPILER_64_ADD_64(sum, value64);
                    }
                }
            }
            *value = sum;
        }
        break;
    case bcmCosqStatOutBytes:
        if (BCM_GPORT_IS_SCHEDULER(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_node_get(unit, port, NULL,
                                        &local_port, NULL, &node));
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_child_node_at_input(node, cosq, &node));
            port = node->gport;
            if (BCM_GPORT_IS_SCHEDULER(port)) {
               /* Not support more than one-layer scheduler node analysis*/
               return BCM_E_PARAM;
            }
        }

        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) {
            if (BCM_GPORT_UCAST_QUEUE_GROUP_QID_GET(port) <
                _BCM_TD_NUM_UCAST_QUEUE_GROUP) { /* regular ucast queue */
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                      &local_port, &startq, NULL));
                BCM_IF_ERROR_RETURN
                    (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC,
                                     startq, value));
            } else { /* extended ucast queue */
                BCM_IF_ERROR_RETURN
                    (_bcm_td_cosq_index_resolve
                     (unit, port, cosq,
                      _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
                      NULL, NULL, &numq));
                COMPILER_64_ZERO(sum);
                for (i = 0; i < numq; i++) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_td_cosq_index_resolve
                         (unit, port, cosq < 0 ? i : cosq,
                          _BCM_TD_COSQ_INDEX_STYLE_EXT_UCAST_QUEUE,
                          &local_port, &startq, NULL));
                    BCM_IF_ERROR_RETURN
                        (counter_get
                         (unit, local_port,
                          SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_EXT,
                          startq, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
                *value = sum;
            }
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                  &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                                 SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE, startq,
                                 value));
        } else {
            if (cosq < BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, port, &local_port));
            COMPILER_64_ZERO(sum);
            if (cosq == BCM_COS_INVALID) {
                /* UC queue */
                for (i = 0; i < si->port_num_uc_cosq[local_port]; i++) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC,
                                     i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
                /* MC queue */
                for (i = 0; i < si->port_num_cosq[local_port]; i++) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                         i, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                }
            } else {
                /*
                 * cosq=0 for ucast queue 0 + mcast queue 0
                 * cosq=1 for ucast queue 1 + mcast queue 1
                 * cosq=2 for ucast queue 2 + mcast queue 2
                 * cosq=3 for ucast queue 3 + mcast queue 3
                 * cosq=4 for ucast queue 4
                 * cosq=5 for ucast queue 5
                 * cosq=6 for ucast queue 6
                 * cosq=7 for ucast queue 7
                 * cosq=8 for unicast SC queue + multicast SC/QM queue
                 * cosq=9 for unicast QM queue.
                 */
                if (IS_CPU_PORT(unit, local_port) ||
                    IS_LB_PORT(unit, local_port)) {
                    if (cosq >= (si->port_num_cosq[local_port])) {
                        return BCM_E_PARAM;
                    }
                } else if (cosq >= (si->port_num_uc_cosq[local_port])){
                    return BCM_E_PARAM;
                }

                if (IS_CPU_PORT(unit, local_port) ||
                    IS_LB_PORT(unit, local_port)) {
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                     cosq, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                } else {
                    /* UC queue */
                    BCM_IF_ERROR_RETURN
                        (counter_get(unit, local_port,
                                     SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC,
                                     cosq, &value64));
                    COMPILER_64_ADD_64(sum, value64);
                    /* MC queue */
                    if ((cosq < (si->port_num_cosq[local_port] - 1)) ||
                        (cosq == 8)){
                        cosq = (cosq == 8) ? 4 : cosq;
                        BCM_IF_ERROR_RETURN
                            (counter_get(unit, local_port,
                                         SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
                                         cosq, &value64));
                        COMPILER_64_ADD_64(sum, value64);
                    }
                }
            }
            *value = sum;
        }
        break;
    case bcmCosqStatIeee8021CnCpTransmittedCnms:
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve
                 (unit, port, cosq, _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  &local_port, NULL, NULL));
        BCM_IF_ERROR_RETURN
            (bcm_td_cosq_congestion_queue_get(unit, port, cosq, &startq));
        BCM_IF_ERROR_RETURN
            (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_MMU_QCN_CNM,startq, value));
        break;
    case bcmCosqStatIngressPortPoolSharedBytesPeak:
        COMPILER_64_ZERO(sum);
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_ingress_sp_get(unit, port, cosq, &start_hw_index,
                                         &end_hw_index));
        BCM_IF_ERROR_RETURN(_bcm_td_cosq_localport_resolve(unit, port,
                                                           &local_port));
        for (i = start_hw_index; i <= end_hw_index; i++) {
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_POOL_PEAK,
                             i, &value64));
            COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_TD_BYTES_PER_CELL);
        *value = sum;
        break;
    case bcmCosqStatIngressPortPoolSharedBytesCurrent:
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_ingress_sp_get(unit, port, cosq, &start_hw_index,
                                         &end_hw_index));
        BCM_IF_ERROR_RETURN(_bcm_td_cosq_localport_resolve(unit, port,
                                                           &local_port));

        COMPILER_64_ZERO(sum);
        for (i = start_hw_index; i <= end_hw_index; i++) {
            /* coverity[NEGATIVE_RETURNS: FALSE] */
            BCM_IF_ERROR_RETURN
                (counter_get(unit, local_port,
                             SOC_COUNTER_NON_DMA_POOL_CURRENT,
                             i, &value64));
           COMPILER_64_ADD_64(sum, value64);
        }

        COMPILER_64_UMUL_32(sum, _BCM_TD_BYTES_PER_CELL);
        *value = sum;
        break;

    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_cosq_quantize_table_set
 * Purpose:
 *     Generate quantized feedback lookup table
 * Parameters:
 *     unit          - (IN) unit number
 *     profile_index - (IN) profile index
 *     weight_code   - (IN) weight encoding
 *     set_point     - (IN) queue size set point
 *     active_offset - (OUT) most significant non-zero bit position
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     The formular to quantize feedback value is:
 *     quantized_feedbad = celling(feedback / max_feedback * 63)
 *         where max_feedback = (2 * cpW + 1) * cpQsp
 *         cpW is feedback weight
 *         cpQsp is queue size set point
 */
STATIC int
_bcm_td_cosq_quantize_table_set(int unit, int profile_index, int weight_code,
                                int set_point, int *active_offset)
{
    int ref_count;
    int weight_x_4, feedback, max_feedback, quantized_feedback, shift;
    int base_index, index;

    /* weight = 2 ** (weight_code - 2) e.g. 0 for 1/4, 1 for 1/2, ...  */
    weight_x_4 = 1 << weight_code;
    max_feedback = ((2 * weight_x_4 + 4) * set_point + 3) / 4;
    shift = 0;
    while ((max_feedback >> shift) > 127) {
        shift++;
    }

    BCM_IF_ERROR_RETURN
        (soc_profile_reg_ref_count_get(unit, _bcm_td_feedback_profile[unit],
                                       profile_index, &ref_count));

    /*
     * Hardware use lookup table to find quantized feedback in order to avoid
     * division, use most significant 7-bit of feedback value as lookup table
     * index to find quantized_feedback value
     */
    if (ref_count == 1) {
        base_index = profile_index << 7;
        for (index = 0; index < 128; index++) {
            feedback = index << shift;
            if (feedback > max_feedback) {
                quantized_feedback = 63;
            } else {
                quantized_feedback = (feedback * 63 + max_feedback - 1) /
                    max_feedback;
            }
            BCM_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, MMU_QCN_QFBTBm,
                                        base_index  + index, CPQ_QNTZFBf,
                                        quantized_feedback));
        }
    }

    *active_offset = shift + 6;

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_sample_int_table_set(int unit, int min, int max,
                                  uint32 *profile_index)
{
    mmu_qcn_sitb_entry_t sitb_entries[64];
    void *entries[1];
    int i, j, center, inc, value, index;

    sal_memset(sitb_entries, 0, sizeof(sitb_entries));
    entries[0] = &sitb_entries;
    for (i = 0; i < 8; i++) {
        center = max - (max - min) * i / 7;
        inc = (max - min) / 7 / 8;
        for (j = 0; j < 8; j++, inc = -inc) {
            index = i * 8 + j;
            value = center + (j + 1) / 2 * inc;
            if (value > 255) {
                value = 255;
            } else if (value < 1) {
                value = 1;
            }
            soc_mem_field32_set(unit, MMU_QCN_SITBm, &sitb_entries[index],
                                CPQ_SIf, value);
        }
    }

    return soc_profile_mem_add(unit, _bcm_td_sample_int_profile[unit], entries,
                               64, (uint32 *)profile_index);
}

/*
 * Function:
 *     bcm_td_cosq_congestion_queue_set
 * Purpose:
 *     Enable/Disable a cos queue as congestion managed queue
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     index         - (IN) congestion managed queue index
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_congestion_queue_set(int unit, bcm_port_t port,
                                 bcm_cos_queue_t cosq, int index)
{
    soc_info_t *si;
    bcm_port_t local_port;
    int phy_port, mmu_port;
    uint32 rval;
    uint64 rval64, *rval64s[1];
    mmu_qcn_enable_entry_t enable_entry;
    mmu_qcn_cpqcfg_entry_t cpqcfg_entry;
    int cosq_index, active_offset, count;
    uint32 profile_index, sample_profile_index;
    int weight_code, set_point;
	int qindex;
	
    si = &SOC_INFO(unit);

    if (cosq < 0 || cosq >= _bcm_td_num_cosq[unit]) {
        return BCM_E_PARAM;
    }

    if (index < -1 || index > 1) {
        return BCM_E_PARAM;
    }

	BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                    &local_port, &qindex, NULL));
	
    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_ENABLEm,
                                     MEM_BLOCK_ANY, mmu_port, &enable_entry));
    if (index == -1) {
        if (!soc_mem_field32_get(unit, MMU_QCN_ENABLEm, &enable_entry,
                                 _bcm_td_cpq_en_field[qindex])) {
            return BCM_E_NONE;
        }

        index = soc_mem_field32_get(unit, MMU_QCN_ENABLEm, &enable_entry,
                                    _bcm_td_cpq_index_field[qindex]);
        soc_mem_field32_set(unit, MMU_QCN_ENABLEm, &enable_entry,
                            _bcm_td_cpq_en_field[qindex], 0);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, MMU_QCN_ENABLEm, MEM_BLOCK_ANY, mmu_port,
                           &enable_entry));

        /* Delete feedback parameter profile */
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_CPQCFGm, MEM_BLOCK_ANY,
                                         mmu_port, &cpqcfg_entry));
        profile_index =
            soc_mem_field32_get(unit, MMU_QCN_CPQCFGm, &cpqcfg_entry,
                                _bcm_td_eqtb_index_field[index]);
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_delete(unit, _bcm_td_feedback_profile[unit],
                                    profile_index));
    } else {
        count = sizeof(_bcm_td_cpq_en_field) / sizeof(_bcm_td_cpq_en_field[0]);
        for (cosq_index = 0; cosq_index < count; cosq_index++) {
            if (!soc_mem_field32_get(unit, MMU_QCN_ENABLEm, &enable_entry,
                                     _bcm_td_cpq_en_field[cosq_index])) {
                continue;
            }
            if (soc_mem_field32_get(unit, MMU_QCN_ENABLEm, &enable_entry,
                                    _bcm_td_cpq_index_field[cosq_index]) ==
                index) {
                if (cosq_index == qindex) {
                    return BCM_E_NONE;
                } else {
                    return BCM_E_EXISTS;
                }
            }
        }

        /* Use hardware reset value as default quantize setting */
        weight_code = 3;
        set_point = 0x96;
        rval = 0;
        soc_reg_field_set(unit, MMU_QCN_CPQ_SEQr, &rval, CPWf, weight_code);
        soc_reg_field_set(unit, MMU_QCN_CPQ_SEQr, &rval, CPQEQf, set_point);
        COMPILER_64_SET(rval64, 0, rval);
        rval64s[0] = &rval64;
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_add(unit, _bcm_td_feedback_profile[unit],
                                 rval64s, 1, (uint32 *)&profile_index));

        /* Update quantized feedback calculation lookup table */
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_quantize_table_set(unit, profile_index, weight_code,
                                             set_point, &active_offset));

        /* Pick some sample interval */
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_sample_int_table_set(unit, 13, 127,
                                               &sample_profile_index));

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_CPQCFGm, MEM_BLOCK_ANY,
                                         mmu_port, &cpqcfg_entry));
        soc_mem_field32_set(unit, MMU_QCN_CPQCFGm, &cpqcfg_entry,
                            _bcm_td_act_offset_field[index], active_offset);
        soc_mem_field32_set(unit, MMU_QCN_CPQCFGm, &cpqcfg_entry,
                            _bcm_td_eqtb_index_field[index], profile_index);
        soc_mem_field32_set(unit, MMU_QCN_CPQCFGm, &cpqcfg_entry,
                            _bcm_td_sitb_sel_field[index],
                            sample_profile_index);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_QCN_CPQCFGm, MEM_BLOCK_ANY,
                                          mmu_port, &cpqcfg_entry));

        soc_mem_field32_set(unit, MMU_QCN_ENABLEm, &enable_entry,
                            _bcm_td_cpq_en_field[qindex], 1);
        soc_mem_field32_set(unit, MMU_QCN_ENABLEm, &enable_entry,
                            _bcm_td_cpq_index_field[qindex], index);
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, MMU_QCN_ENABLEm, MEM_BLOCK_ANY, mmu_port,
                           &enable_entry));
    }

    /* Need to program ING_OUTER_DOT1P_MAPPING_TABLE to bypass the cosq */

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_congestion_queue_get
 * Purpose:
 *     Get congestion managed queue index of the specified cos queue
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     index         - (OUT) congestion managed queue index
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_congestion_queue_get(int unit, bcm_port_t port,
                                 bcm_cos_queue_t cosq, int *index)
{
    soc_info_t *si;
    bcm_port_t local_port;
    int phy_port, mmu_port;
    mmu_qcn_enable_entry_t entry;
	int qindex;

    si = &SOC_INFO(unit);

    if (cosq < 0 || cosq >= _bcm_td_num_cosq[unit]) {
        return BCM_E_PARAM;
    }

    if (index == NULL) {
        return BCM_E_PARAM;
    }

	BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                    &local_port, &qindex, NULL));

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, MMU_QCN_ENABLEm, MEM_BLOCK_ANY, mmu_port, &entry));
    if (soc_mem_field32_get(unit, MMU_QCN_ENABLEm, &entry,
                            _bcm_td_cpq_en_field[qindex])) {
        *index = soc_mem_field32_get(unit, MMU_QCN_ENABLEm, &entry,
                                     _bcm_td_cpq_index_field[qindex]);
    } else {
        *index = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_congestion_quantize_set
 * Purpose:
 *     Set quantized congestion feedback algorithm parameters
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     weight_code   - (IN) weight encoding (use -1 for unchange)
 *     set_point     - (IN) queue size set point (use -1 for unchange)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_congestion_quantize_set(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int weight_code,
                                    int set_point)
{
    soc_info_t *si;
    bcm_port_t local_port;
    int phy_port, mmu_port;
    uint32 rval;
    uint64 rval64, *rval64s[1];
    mmu_qcn_cpqcfg_entry_t entry;
    int cpq_index, active_offset;
    uint32 profile_index, old_profile_index;

    si = &SOC_INFO(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                    &local_port, NULL, NULL));

    BCM_IF_ERROR_RETURN
        (bcm_td_cosq_congestion_queue_get(unit, port, cosq, &cpq_index));
    if (cpq_index == -1) {
        /* The cosq specified is not enabled as congestion managed queue */
        return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, MMU_QCN_CPQCFGm, MEM_BLOCK_ANY, mmu_port, &entry));
    old_profile_index =
        soc_mem_field32_get(unit, MMU_QCN_CPQCFGm, &entry,
                            _bcm_td_eqtb_index_field[cpq_index]);

    BCM_IF_ERROR_RETURN(READ_MMU_QCN_CPQ_SEQr(unit, old_profile_index, &rval));
    if (weight_code == -1) {
        weight_code = soc_reg_field_get(unit, MMU_QCN_CPQ_SEQr, rval, CPWf);
    } else {
        if (weight_code < 0 || weight_code > 7) {
            return BCM_E_PARAM;
        }
        soc_reg_field_set(unit, MMU_QCN_CPQ_SEQr, &rval, CPWf, weight_code);
    }
    if (set_point == -1) {
        set_point = soc_reg_field_get(unit, MMU_QCN_CPQ_SEQr, rval, CPQEQf);
    } else {
        if (set_point < 0 || set_point > 0xffff) {
            return BCM_E_PARAM;
        }
        soc_reg_field_set(unit, MMU_QCN_CPQ_SEQr, &rval, CPQEQf, set_point);
    }
    COMPILER_64_SET(rval64, 0, rval);
    rval64s[0] = &rval64;
    /* Add new feedback parameter profile */
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_add(unit, _bcm_td_feedback_profile[unit], rval64s,
                             1, (uint32 *)&profile_index));
    /* Delete original feedback parameter profile */
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_delete(unit, _bcm_td_feedback_profile[unit],
                                old_profile_index));

    /* Update quantized feedback calculation lookup table */
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_quantize_table_set(unit, profile_index, weight_code,
                                         set_point, &active_offset));

    soc_mem_field32_set(unit, MMU_QCN_CPQCFGm, &entry,
                        _bcm_td_act_offset_field[cpq_index], active_offset);
    soc_mem_field32_set(unit, MMU_QCN_CPQCFGm, &entry,
                        _bcm_td_eqtb_index_field[cpq_index], profile_index);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_QCN_CPQCFGm, MEM_BLOCK_ANY,
                                      mmu_port, &entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_congestion_quantize_get
 * Purpose:
 *     Get quantized congestion feedback algorithm parameters
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) port number or GPORT identifier
 *     cosq          - (IN) COS queue number
 *     weight_code   - (OUT) weight encoding
 *     set_point     - (OUT) queue size set point
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_congestion_quantize_get(int unit, bcm_port_t port,
                                    bcm_cos_queue_t cosq, int *weight_code,
                                    int *set_point)
{
    soc_info_t *si;
    bcm_port_t local_port;
    int phy_port, mmu_port;
    int cpq_index, profile_index;
    uint32 rval;
    mmu_qcn_cpqcfg_entry_t entry;

    si = &SOC_INFO(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                    &local_port, NULL, NULL));

    BCM_IF_ERROR_RETURN
        (bcm_td_cosq_congestion_queue_get(unit, port, cosq, &cpq_index));
    if (cpq_index == -1) {
        /* The cosq specified is not enabled as congestion managed queue */
        return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, MMU_QCN_CPQCFGm, MEM_BLOCK_ANY, mmu_port, &entry));
    profile_index = soc_mem_field32_get(unit, MMU_QCN_CPQCFGm, &entry,
                                        _bcm_td_eqtb_index_field[cpq_index]);

    BCM_IF_ERROR_RETURN(READ_MMU_QCN_CPQ_SEQr(unit, profile_index, &rval));
    if (weight_code != NULL) {
        *weight_code = soc_reg_field_get(unit, MMU_QCN_CPQ_SEQr, rval, CPWf);
    }
    if (set_point != NULL) {
        *set_point = soc_reg_field_get(unit, MMU_QCN_CPQ_SEQr, rval, CPQEQf);
    }

    return BCM_E_NONE;
}

int
bcm_td_cosq_congestion_sample_int_set(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int min, int max)
{
    soc_info_t *si;
    bcm_port_t local_port;
    int phy_port, mmu_port;
    mmu_qcn_cpqcfg_entry_t cpqcfg_entry;
    mmu_qcn_sitb_entry_t sitb_entry;
    int cpq_index;
    uint32 profile_index, old_profile_index;

    si = &SOC_INFO(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                    &local_port, NULL, NULL));

    BCM_IF_ERROR_RETURN
        (bcm_td_cosq_congestion_queue_get(unit, port, cosq, &cpq_index));
    if (cpq_index == -1) {
        /* The cosq specified is not enabled as congestion managed queue */
        return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_CPQCFGm, MEM_BLOCK_ANY,
                                     mmu_port, &cpqcfg_entry));
    old_profile_index =
        soc_mem_field32_get(unit, MMU_QCN_CPQCFGm, &cpqcfg_entry,
                            _bcm_td_sitb_sel_field[cpq_index]);

    if (max == -1) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_SITBm, MEM_BLOCK_ANY,
                                         old_profile_index * 64, &sitb_entry));
        max = soc_mem_field32_get(unit, MMU_QCN_SITBm, &sitb_entry, CPQ_SIf);
    } else {
        if (max < 1 || max > 255) {
            return BCM_E_PARAM;
        }
    }
    if (min == -1) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_SITBm, MEM_BLOCK_ANY,
                                         old_profile_index * 64 + 56,
                                         &sitb_entry));
        min = soc_mem_field32_get(unit, MMU_QCN_SITBm, &sitb_entry, CPQ_SIf);
    } else {
        if (min < 1 || min > 255) {
            return BCM_E_PARAM;
        }
    }

    /* Add new sample interval profile */
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_sample_int_table_set(unit, min, max, &profile_index));

    /* Delete original sample interval profile */
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit, _bcm_td_sample_int_profile[unit],
                                old_profile_index * 64));

    soc_mem_field32_set(unit, MMU_QCN_CPQCFGm, &cpqcfg_entry,
                        _bcm_td_sitb_sel_field[cpq_index], profile_index / 64);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, MMU_QCN_CPQCFGm, MEM_BLOCK_ANY,
                                      mmu_port, &cpqcfg_entry));

    return BCM_E_NONE;
}

int
bcm_td_cosq_congestion_sample_int_get(int unit, bcm_port_t port,
                                      bcm_cos_queue_t cosq, int *min, int *max)
{
    soc_info_t *si;
    bcm_port_t local_port;
    int phy_port, mmu_port;
    mmu_qcn_cpqcfg_entry_t cpqcfg_entry;
    mmu_qcn_sitb_entry_t sitb_entry;
    int cpq_index, profile_index;

    si = &SOC_INFO(unit);

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_index_resolve(unit, port, cosq,
                                    _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                    &local_port, NULL, NULL));

    BCM_IF_ERROR_RETURN
        (bcm_td_cosq_congestion_queue_get(unit, port, cosq, &cpq_index));
    if (cpq_index == -1) {
        /* The cosq specified is not enabled as congestion managed queue */
        return BCM_E_PARAM;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_CPQCFGm, MEM_BLOCK_ANY,
                                     mmu_port, &cpqcfg_entry));
    profile_index = soc_mem_field32_get(unit, MMU_QCN_CPQCFGm, &cpqcfg_entry,
                                        _bcm_td_sitb_sel_field[cpq_index]);

    if (max != NULL) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_SITBm, MEM_BLOCK_ANY,
                                         profile_index * 64, &sitb_entry));
        *max = soc_mem_field32_get(unit, MMU_QCN_SITBm, &sitb_entry, CPQ_SIf);
    }
    if (min != NULL) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, MMU_QCN_SITBm, MEM_BLOCK_ANY,
                                         profile_index * 64 + 56,
                                         &sitb_entry));
        *min = soc_mem_field32_get(unit, MMU_QCN_SITBm, &sitb_entry, CPQ_SIf);
    }

    return BCM_E_NONE;
}

int
bcm_td_cosq_drop_status_enable_set(int unit, bcm_port_t port, int enable)
{
    soc_info_t *si;
    soc_mem_t mem;
    int phy_port, mmu_port, base, count, idx;
    uint32 rval;

    si = &SOC_INFO(unit);

    /* Multicast queue */
    count = IS_LB_PORT(unit, port) ? 5 : si->port_num_cosq[port];
    for (idx = 0; idx < count; idx++) {
        SOC_IF_ERROR_RETURN
            (READ_OP_QUEUE_CONFIG1_CELLr(unit, port, idx, &rval));
        soc_reg_field_set(unit,OP_QUEUE_CONFIG1_CELLr, &rval,
                          Q_E2E_DS_EN_CELLf, enable ? 1 : 0);
        SOC_IF_ERROR_RETURN
            (WRITE_OP_QUEUE_CONFIG1_CELLr(unit, port, idx, rval));
    }

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    if (si->port_num_ext_cosq[port] == 0) { /* regular port */
        /* Regular unicast queue */
        if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) { /* X pipe */
            mem = MMU_THDO_CONFIG_0m;
            /* Index starts from mmu port 5, 10 entries per port */
            base = (mmu_port - 5) * 10;
        } else { /* Y pipe */
            mem = MMU_THDO_CONFIG_1m;
            /* Index starts from mmu port 38, 10 entries per port */
            base = (mmu_port - 38) * 10;
        }
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, mem, base + idx,
                                        Q_E2E_DS_EN_CELLf, 1));
        }
    } else { /* extended queue port */
        /*
         * Depends on OP_VOQ_PORT_CONFIG mode setting for the extended
         * queue port:
         * - cos mode
         *     use MMU_THDO_CONFIG_SP_x table, 10 entries per port
         * - queue mode
         *     use MMU_THDO_CONFIG_EX_x table, 74 entries per port
         *     extended unicast queue: index 0-63
         *     regular unicast queue: index 64-73
         */
        /* Regular unicast queue in queue mode and extended unicast queue */
        if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) { /* X pipe */
            mem = MMU_THDO_CONFIG_EX_0m;
            /* Index starts from mmu port 1, 74 entries per port */
            base = (mmu_port - 1) * 74;
        } else { /* Y pipe */
            mem = MMU_THDO_CONFIG_EX_1m;
            /* Index starts from mmu port 34, 74 entries per port */
            base = (mmu_port - 34) * 74;
        }
        count = si->port_num_uc_cosq[port] + si->port_num_ext_cosq[port];
        for (idx = 0; idx < count; idx++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, mem, base + idx,
                                        Q_E2E_DS_EN_CELLf, enable ? 1 : 0));
        }

        /* Regular unicast queue in cos mode */
        if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) { /* X pipe */
            mem = MMU_THDO_CONFIG_SP_0m;
            /* Index starts from mmu port 1, 10 entries per port */
            base = (mmu_port - 1) * 10;
        } else { /* Y pipe */
            mem = MMU_THDO_CONFIG_SP_1m;
            /* Index starts from mmu port 34, 10 entries per port */
            base = (mmu_port - 34) * 10;
        }
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_field32_modify(unit, mem, base + idx,
                                        Q_E2E_DS_EN_CELLf, enable ? 1 : 0));
        }
    }

    /* Regular unicast queue */
    SOC_IF_ERROR_RETURN(READ_OP_UC_PORT_CONFIG1_CELLr(unit, port, &rval));
    soc_reg_field_set(unit,OP_UC_PORT_CONFIG1_CELLr, &rval,
                      Q_E2E_DS_ENf, enable ? 0xff : 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_UC_PORT_CONFIG1_CELLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_OP_THR_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, OP_THR_CONFIGr, &rval, EARLY_E2E_SELECTf,
                      enable ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_OP_THR_CONFIGr(unit, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_egr_pool_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int id, startq, pool;
    soc_reg_t reg;
    soc_field_t field;
    uint32 rval;
    int num_cells, default_mtu_cells;
    soc_info_t *si;

    num_cells = 0;
    default_mtu_cells = _BCM_TD_BYTES_TO_CELLS(_BCM_TD_DEFAULT_MTU_BYTES +
                                               _BCM_TD_PACKET_HEADER_BYTES);
    si = &SOC_INFO(unit);

    if (type == bcmCosqControlUCEgressPool) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_UC_EGR_POOL,
                                        &local_port, &startq, NULL));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_EGR_POOL,
                                        &local_port, &startq, NULL));
    }
    if (type == bcmCosqControlEgressPoolLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            reg = OP_UC_PORT_CONFIG1_CELLr;
        } else {
            reg = OP_PORT_CONFIG1_CELLr;
        }
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, PORT_LIMIT_ENABLE_CELLf,
                          arg ? 1 : 0);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, local_port, 0, rval));
        return BCM_E_NONE;
    } else if (type == bcmCosqControlEgressPool) {
        if (arg < 0 || arg > 3) {
            return BCM_E_PARAM;
        }
    } else if (type ==  bcmCosqControlUCEgressPool || type == bcmCosqControlMCEgressPool) {
        if (arg < 0 || arg > 3) {
            return BCM_E_PARAM;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }
    } else if (type == bcmCosqControlEgressPoolLimitBytes ||
               type == bcmCosqControlEgressPoolYellowLimitBytes ||
               type == bcmCosqControlEgressPoolRedLimitBytes) {
        if (arg < 0) {
            return BCM_E_PARAM;
        }
        num_cells = arg / _BCM_TD_BYTES_PER_CELL;
        if (num_cells > _BCM_TD_TOTAL_CELLS) {
            return BCM_E_PARAM;
        }
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
        if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
            reg = OP_UC_PORT_CONFIG1_CELLr;
            field = _bcm_td_uc_spid_fields[startq];
            /* regular unicast queue */
        } else {
            /* extended unicast queue */
            reg = _bcm_td_ext_uc_spid_regs[startq / 16];
            field = _bcm_td_ext_uc_spid_fields[startq];
        }
        startq = 0;
    } else if (type == bcmCosqControlUCEgressPool) {
        if (si->port_num_ext_cosq[local_port] == 0) {
            /* regular unicast queue */
            reg = OP_UC_PORT_CONFIG1_CELLr;
            field = _bcm_td_uc_spid_fields[startq];
        } else { 
            /* extended unicast queue */
            reg = _bcm_td_ext_uc_spid_regs[(startq + 64) / 16]; 
            field = _bcm_td_ext_uc_spid_fields[startq + 64];
        }
    } else { /* multicast queue group or local port */
        reg = OP_QUEUE_CONFIG1_CELLr;
        field = Q_SPIDf;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, startq, &rval));
    if (type == bcmCosqControlEgressPool || type == bcmCosqControlUCEgressPool ||
        type == bcmCosqControlMCEgressPool) {
        soc_reg_field_set(unit, reg, &rval, field, arg);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, local_port, startq, rval));
        return BCM_E_NONE;
    }

    pool = soc_reg_field_get(unit, reg, rval, field);
    switch (type) {
    case bcmCosqControlEgressPoolLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            reg = OP_UC_PORT_CONFIG_CELLr;
        } else {
            reg = OP_PORT_CONFIG_CELLr;
        }
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, pool, &rval));
        soc_reg_field_set(unit, reg, &rval, OP_SHARED_LIMIT_CELLf, num_cells);
        num_cells = num_cells > default_mtu_cells ?
            num_cells - default_mtu_cells : 0;
        soc_reg_field_set(unit, reg, &rval, OP_SHARED_RESET_VALUE_CELLf,
                          num_cells);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, local_port, pool, rval));
        break;
    case bcmCosqControlEgressPoolYellowLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            reg = OP_UC_PORT_LIMIT_COLOR_CELLr;
        } else {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, pool, &rval));
        /* granularity for color limit is 8 cells */
        soc_reg_field_set(unit, reg, &rval, YELf, num_cells / 8);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, local_port, pool, rval));
        break;
    case bcmCosqControlEgressPoolRedLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            reg = OP_UC_PORT_LIMIT_COLOR_CELLr;
        } else {
            reg = OP_PORT_LIMIT_COLOR_CELLr;
        }
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, pool, &rval));
        /* granularity for color limit is 8 cells */
        soc_reg_field_set(unit, reg, &rval, REDf, num_cells / 8);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, local_port, pool, rval));
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_egr_pool_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int id, startq, pool;
    soc_reg_t reg;
    soc_field_t field;
    uint32 rval;
    soc_info_t *si;
    si = &SOC_INFO(unit);

    if (type == bcmCosqControlUCEgressPool) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_UC_EGR_POOL,
                                        &local_port, &startq, NULL));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_EGR_POOL,
                                        &local_port, &startq, NULL));
    }
    if (type == bcmCosqControlEgressPoolLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            reg = OP_UC_PORT_CONFIG1_CELLr;
        } else {
            reg = OP_PORT_CONFIG1_CELLr;
        }
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, 0, &rval));
        *arg = soc_reg_field_get(unit, reg, rval, PORT_LIMIT_ENABLE_CELLf);
        return BCM_E_NONE;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
        if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
            reg = OP_UC_PORT_CONFIG1_CELLr;
            field = _bcm_td_uc_spid_fields[startq];
            /* regular unicast queue */
        } else {
            /* extended unicast queue */
            reg = _bcm_td_ext_uc_spid_regs[startq / 16];
            field = _bcm_td_ext_uc_spid_fields[startq];
        }
        startq = 0;
    } else if (type == bcmCosqControlUCEgressPool) {
        if (si->port_num_ext_cosq[local_port] == 0) {
            /* regular unicast queue */
            reg = OP_UC_PORT_CONFIG1_CELLr;
            field = _bcm_td_uc_spid_fields[startq];
        } else { 
            /* extended unicast queue */
            reg = _bcm_td_ext_uc_spid_regs[(startq + 64)/16];
            field = _bcm_td_ext_uc_spid_fields[startq + 64];
        }
    } else { /* multicast queue group or local port */
        reg = OP_QUEUE_CONFIG1_CELLr;
        field = Q_SPIDf;
    }

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, startq, &rval));
    pool = soc_reg_field_get(unit, reg, rval, field);
    if (type == bcmCosqControlEgressPool || type == bcmCosqControlUCEgressPool ||
        type == bcmCosqControlMCEgressPool) {
        *arg = pool;
        return BCM_E_NONE;
    }

    switch (type) {
    case bcmCosqControlEgressPoolLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            reg = OP_UC_PORT_CONFIG_CELLr;
        } else {
            reg = OP_PORT_CONFIG_CELLr;
        }
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, pool, &rval));
        *arg = soc_reg_field_get(unit, reg, rval, OP_SHARED_LIMIT_CELLf) *
            _BCM_TD_BYTES_PER_CELL;
        break;
    case bcmCosqControlEgressPoolYellowLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            reg = OP_UC_PORT_LIMIT_COLOR_CELLr;
        } else {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, pool, &rval));
        /* granularity for color limit is 8 cells */
        *arg = soc_reg_field_get(unit, reg, rval, YELf) * 8 *
            _BCM_TD_BYTES_PER_CELL;
        break;
    case bcmCosqControlEgressPoolRedLimitBytes:
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            reg = OP_UC_PORT_LIMIT_COLOR_CELLr;
        } else {
            reg = OP_PORT_LIMIT_COLOR_CELLr;
        }
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, local_port, pool, &rval));
        /* granularity for color limit is 8 cells */
        *arg = soc_reg_field_get(unit, reg, rval, REDf) * 8 *
            _BCM_TD_BYTES_PER_CELL;
        break;
    default:
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_alpha_set(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_drop_limit_alpha_value_t arg)
{
    bcm_port_t local_port;
    int id,startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_info_t *si;    
    int alpha;
    uint32 rval;  
    int dynamic_thresh_mode;
    int port_pg;
    soc_reg_t reg = INVALIDr;    
    static const soc_reg_t prigroup_reg[] = {
        PORT_PRI_GRP0r, PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };     

    switch(arg) {
    case bcmCosqControlDropLimitAlpha_1_64:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_64;
        break;
    case bcmCosqControlDropLimitAlpha_1_32:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_32;
        break;
    case bcmCosqControlDropLimitAlpha_1_16:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_16;
        break;
    case bcmCosqControlDropLimitAlpha_1_8:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_8;
        break;
    case bcmCosqControlDropLimitAlpha_1_4:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_4;
        break;
    case bcmCosqControlDropLimitAlpha_1_2:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_2;
        break;
    case bcmCosqControlDropLimitAlpha_1:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_1;
        break;   
    case bcmCosqControlDropLimitAlpha_2:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_2;
        break;
    case bcmCosqControlDropLimitAlpha_4:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_4;
        break;
    case bcmCosqControlDropLimitAlpha_8:
        alpha = SOC_TD_COSQ_DROP_LIMIT_ALPHA_8;
        break;         
    default:
        return BCM_E_PARAM;          
    }

    si = &SOC_INFO(unit);

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressUCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
    
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
        if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
             /* regular unicast queue */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_0m;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_1m;
            }           
        } else {
            /* extended unicast queue */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_EX_0m;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_EX_1m;
            }
        } 
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, 
                            Q_SHARED_ALPHA_CELLf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressMCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        BCM_IF_ERROR_RETURN
            (READ_OP_QUEUE_CONFIG_CELLr(unit, local_port, cosq, &rval));
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                          Q_SHARED_ALPHA_CELLf, alpha);
        BCM_IF_ERROR_RETURN
            (WRITE_OP_QUEUE_CONFIG_CELLr(unit, local_port, cosq, rval));  
        
    } else {
        BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                               bcmCosqControlIngressPortPGSharedDynamicEnable,
                               &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        
        /* get PG for port using Port+Cos */
        if (cosq < 8) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        
        SOC_IF_ERROR_RETURN
            (READ_PG_SHARED_LIMIT_CELLr(unit, local_port, port_pg, &rval));
        soc_reg_field_set(unit, PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_LIMITf, alpha);
        SOC_IF_ERROR_RETURN
            (WRITE_PG_SHARED_LIMIT_CELLr(unit, local_port, port_pg, rval));        
    }   
     
    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_alpha_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_drop_limit_alpha_value_t *arg)
{
    bcm_port_t local_port;
    int id,startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_info_t *si;  
    int alpha;
    uint32 rval;    
    int dynamic_thresh_mode;
    int port_pg;
    soc_reg_t reg = INVALIDr;    
    static const soc_reg_t prigroup_reg[] = {
        PORT_PRI_GRP0r, PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };     

    si = &SOC_INFO(unit);

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressUCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
        if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
             /* regular unicast queue */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_0m;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_1m;
            }           
        } else {
            /* extended unicast queue */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_EX_0m;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_EX_1m;
            }
        }      
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, Q_SHARED_ALPHA_CELLf);
        
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                   bcmCosqControlEgressMCSharedDynamicEnable, 
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        BCM_IF_ERROR_RETURN
            (READ_OP_QUEUE_CONFIG_CELLr(unit, local_port, cosq, &rval));
        alpha = soc_reg_field_get(unit, OP_QUEUE_CONFIG_CELLr, rval,
                                  Q_SHARED_ALPHA_CELLf);   
        
    } else {
        BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                               bcmCosqControlIngressPortPGSharedDynamicEnable,
                               &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }
        
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        
        /* get PG for port using Port+Cos */
        if (cosq < 8) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        
        SOC_IF_ERROR_RETURN
            (READ_PG_SHARED_LIMIT_CELLr(unit, local_port, port_pg, &rval));
        alpha = soc_reg_field_get(unit, PG_SHARED_LIMIT_CELLr, rval,
                                 PG_SHARED_LIMITf);         
    } 
    
    switch(alpha) {
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_64:
        *arg = bcmCosqControlDropLimitAlpha_1_64;
        break;
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_32:
        *arg = bcmCosqControlDropLimitAlpha_1_32;
        break;
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_16:
        *arg = bcmCosqControlDropLimitAlpha_1_16;
        break;
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_8:
        *arg = bcmCosqControlDropLimitAlpha_1_8;
        break;
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_4:
        *arg = bcmCosqControlDropLimitAlpha_1_4;
        break;
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_1_2:
        *arg = bcmCosqControlDropLimitAlpha_1_2;
        break;
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_1:
        *arg = bcmCosqControlDropLimitAlpha_1;
        break;   
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_2:
        *arg = bcmCosqControlDropLimitAlpha_2;
        break;
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_4:
        *arg = bcmCosqControlDropLimitAlpha_4;
        break;
    case SOC_TD_COSQ_DROP_LIMIT_ALPHA_8:
        *arg = bcmCosqControlDropLimitAlpha_8;
        break;         
    default:
        return BCM_E_PARAM;          
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_dynamic_thresh_enable_set(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int id,startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_info_t *si;  
    uint32 rval;
    int port_pg;
    soc_reg_t reg = INVALIDr;    
    static const soc_reg_t prigroup_reg[] = {
        PORT_PRI_GRP0r, PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };    

    si = &SOC_INFO(unit);

    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        if ((cosq < 0) || (cosq >= 16)) {
            return BCM_E_PARAM;
        }
        
        /* get PG for port using Port+Cos */
        if (cosq < 8) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        
        SOC_IF_ERROR_RETURN
            (READ_PG_SHARED_LIMIT_CELLr(unit, local_port, port_pg, &rval));
        soc_reg_field_set(unit, PG_SHARED_LIMIT_CELLr, &rval,
                          PG_SHARED_DYNAMICf, arg ? 1 : 0);
        SOC_IF_ERROR_RETURN
            (WRITE_PG_SHARED_LIMIT_CELLr(unit, local_port, port_pg, rval));        
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));

            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
            if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                 /* regular unicast queue */
                if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                    mem = MMU_THDO_CONFIG_0m;
                } else { /* Y pipe */
                    mem = MMU_THDO_CONFIG_1m;
                }           
            } else {
                /* extended unicast queue */
                if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                    mem = MMU_THDO_CONFIG_EX_0m;
                } else { /* Y pipe */
                    mem = MMU_THDO_CONFIG_EX_1m;
                }
            }
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
            soc_mem_field32_set(unit, mem, entry, 
                                Q_LIMIT_DYNAMIC_CELLf, arg ? 1 : 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
            
        } else {
            return BCM_E_PARAM;
        }       
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (READ_OP_QUEUE_CONFIG1_CELLr(unit, local_port, cosq, &rval));
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_CELLr, &rval,
                              Q_LIMIT_DYNAMIC_CELLf, arg ? 1 : 0);
            BCM_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIG1_CELLr(unit, local_port, cosq, rval));

        } else {
            return BCM_E_PARAM;
        }      
       
    } else {
        return BCM_E_PARAM;
    }

    /* Set default alpha value*/
    if (arg) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_alpha_set(unit, gport, cosq, 
                                    bcmCosqControlDropLimitAlpha_1_64));
    }
    
    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_dynamic_thresh_enable_get(int unit, 
                        bcm_gport_t gport, bcm_cos_queue_t cosq, 
                        bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int id,startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_info_t *si;
    uint32 rval;
    int port_pg;
    soc_reg_t reg = INVALIDr;    
    static const soc_reg_t prigroup_reg[] = {
        PORT_PRI_GRP0r, PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };     
    
    si = &SOC_INFO(unit);

    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        if ((cosq < 0) || (cosq >= 16)) {
            return BCM_E_PARAM;
        }
        
        /* get PG for port using Port+Cos */
        if (cosq < 8) {
            reg = prigroup_reg[0];
        } else {
            reg = prigroup_reg[1];
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, local_port, 0, &rval));
        port_pg = soc_reg_field_get(unit, reg, rval, prigroup_field[cosq]);
        
        SOC_IF_ERROR_RETURN
            (READ_PG_SHARED_LIMIT_CELLr(unit, local_port, port_pg, &rval));
        *arg = soc_reg_field_get(unit, PG_SHARED_LIMIT_CELLr, rval,
                                 PG_SHARED_DYNAMICf);         
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         &local_port, &startq, NULL));

            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
            if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                 /* regular unicast queue */
                if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                    mem = MMU_THDO_CONFIG_0m;
                } else { /* Y pipe */
                    mem = MMU_THDO_CONFIG_1m;
                }           
            } else {
                /* extended unicast queue */
                if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                    mem = MMU_THDO_CONFIG_EX_0m;
                } else { /* Y pipe */
                    mem = MMU_THDO_CONFIG_EX_1m;
                }
            }
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
            *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_DYNAMIC_CELLf);
    
        } else {
            return BCM_E_PARAM;
        }       
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
            BCM_IF_ERROR_RETURN
                (READ_OP_QUEUE_CONFIG1_CELLr(unit, local_port, cosq, &rval));
            *arg = soc_reg_field_get(unit, OP_QUEUE_CONFIG1_CELLr, rval,
                                     Q_LIMIT_DYNAMIC_CELLf); 

        } else {
            return BCM_E_PARAM;
        }       
    } else {
        return BCM_E_PARAM;
    }
    
    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_egr_queue_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int id, startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_info_t *si;
    int granularity = 1;
    uint32 rval, rval2;
    int phy_port, mmu_port;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);

    arg /= _BCM_TD_BYTES_PER_CELL;

    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, NULL, NULL));
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            id = cosq;
        }

        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /* regular unicast queue */
        if (si->port_num_ext_cosq[local_port] == 0) {
            /* Front-panel ports */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_0m;
                startq = (mmu_port - 5) * 10 + id;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_1m;
                startq = (mmu_port - 38) * 10 + id;
            }
        } else {
            /* extended queue ports:
             *   use MMU_THDO_CONFIG_EX_x table, 74 entries per port
             *   extended unicast queues: index 0-63
             *   regular unicast queue: index 64-73
             */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_EX_0m;
                startq = (mmu_port - 1) * 74 + 64 + id;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_EX_1m;
                startq = (mmu_port - 34) * 74 + 64 + id;
            }
        }

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));

        if (type == bcmCosqControlEgressUCQueueSharedLimitBytes) {
            soc_mem_field32_set(unit, mem, entry, Q_SHARED_LIMIT_CELLf, (arg/granularity));
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLE_CELLf, 1);
            soc_mem_field32_set(unit, mem, entry, Q_LIMIT_DYNAMIC_CELLf, 0);
        } else {
            soc_mem_field32_set(unit, mem, entry, Q_MIN_CELLf, (arg/granularity));
        }

        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (cosq != BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            startq = cosq;
        }
        BCM_IF_ERROR_RETURN
            (READ_OP_QUEUE_CONFIG_CELLr(unit, local_port, startq, &rval));
        if (type == bcmCosqControlEgressMCQueueSharedLimitBytes) {
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                              Q_SHARED_LIMIT_CELLf, arg/granularity);

            /* enable the queue discard thresholds */
            BCM_IF_ERROR_RETURN
                (READ_OP_QUEUE_CONFIG1_CELLr(unit, local_port, startq, &rval2));
            soc_reg_field_set(unit,OP_QUEUE_CONFIG1_CELLr, &rval2,
                                    Q_LIMIT_ENABLE_CELLf, 1);
            soc_reg_field_set(unit,OP_QUEUE_CONFIG1_CELLr, &rval2,
                                    Q_LIMIT_DYNAMIC_CELLf, 0);
            BCM_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIG1_CELLr(unit, local_port, startq, rval2));
        } else {
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                              Q_MIN_CELLf, arg/granularity);
        }
        BCM_IF_ERROR_RETURN
            (WRITE_OP_QUEUE_CONFIG_CELLr(unit, local_port, startq, rval));
    }else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_egr_queue_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            bcm_cosq_control_t type, int *arg)
{
    bcm_port_t local_port;
    int id, startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_info_t *si;
    int granularity = 1;
    uint32 rval;
    int phy_port, mmu_port;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);

    if ((type == bcmCosqControlEgressUCQueueMinLimitBytes) ||
        (type == bcmCosqControlEgressUCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, NULL, NULL));
            BCM_IF_ERROR_RETURN(_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            id = cosq;
        }

        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /* regular unicast queue */
        if (si->port_num_ext_cosq[local_port] == 0) {
            /* Front-panel ports */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_0m;
                startq = (mmu_port - 5) * 10 + id;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_1m;
                startq = (mmu_port - 38) * 10 + id;
            }
        } else {
            /* extended queue ports
             *     use MMU_THDO_CONFIG_EX_x table, 74 entries per port
             *     extended unicast queues: index 0-63
             *     regular unicast queue: index 64-73
             */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_EX_0m;
                startq = (mmu_port - 1) * 74 + 64 + id;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_EX_1m;
                startq = (mmu_port - 34) * 74 + 64 + id;
            }
        }

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        if (type == bcmCosqControlEgressUCQueueSharedLimitBytes) {
            *arg = soc_mem_field32_get(unit, mem, entry, Q_SHARED_LIMIT_CELLf);
        } else {
            *arg = soc_mem_field32_get(unit, mem, entry, Q_MIN_CELLf);
        }
    } else if ((type == bcmCosqControlEgressMCQueueMinLimitBytes) ||
               (type == bcmCosqControlEgressMCQueueSharedLimitBytes)) {
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (cosq != BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            startq = cosq;
        }
        BCM_IF_ERROR_RETURN
            (READ_OP_QUEUE_CONFIG_CELLr(unit, local_port, startq, &rval));
        if (type == bcmCosqControlEgressMCQueueMinLimitBytes) {
            *arg = soc_reg_field_get(unit, OP_QUEUE_CONFIG_CELLr, rval,
                                     Q_MIN_CELLf);
        } else {
            *arg = soc_reg_field_get(unit, OP_QUEUE_CONFIG_CELLr, rval,
                                     Q_SHARED_LIMIT_CELLf);
        }
    }else {
        return BCM_E_PARAM;
    }

    *arg = (*arg) * granularity * _BCM_TD_BYTES_PER_CELL;
    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_egr_queue_limit_enable_set(int unit, bcm_gport_t gport,
                                                bcm_cos_queue_t cosq,
                                                bcm_cosq_control_t type,
                                                int arg)
{
    bcm_port_t local_port;
    int id, startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_info_t *si;
    uint32 rval;
    int phy_port, mmu_port;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);
    arg = arg ? 1 : 0;

    if (type == bcmCosqControlEgressUCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, NULL, NULL));
            BCM_IF_ERROR_RETURN(_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            id = cosq;
        }

        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /* regular unicast queue */
        if (si->port_num_ext_cosq[local_port] == 0) {
            /* Front-panel ports */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_0m;
                startq = (mmu_port - 5) * 10 + id;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_1m;
                startq = (mmu_port - 38) * 10 + id;
            }
        } else {
            /* extended queue ports
             *     use MMU_THDO_CONFIG_EX_x table, 74 entries per port
             *     extended unicast queues: index 0-63
             *     regular unicast queue: index 64-73
             */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_EX_0m;
                startq = (mmu_port - 1) * 74 + 64 + id;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_EX_1m;
                startq = (mmu_port - 34) * 74 + 64 + id;
            }
        }

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, Q_LIMIT_ENABLE_CELLf, arg);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else if (type == bcmCosqControlEgressMCQueueLimitEnable) {
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (cosq != BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            startq = cosq;
        }
        /* enable the queue discard thresholds */
        BCM_IF_ERROR_RETURN
            (READ_OP_QUEUE_CONFIG1_CELLr(unit, local_port, startq, &rval));
        soc_reg_field_set(unit,OP_QUEUE_CONFIG1_CELLr, &rval,
                                Q_LIMIT_ENABLE_CELLf, arg);
        BCM_IF_ERROR_RETURN
            (WRITE_OP_QUEUE_CONFIG1_CELLr(unit, local_port, startq, rval));
    }else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_egr_queue_limit_enable_get(int unit, bcm_gport_t gport,
                                                bcm_cos_queue_t cosq,
                                                bcm_cosq_control_t type,
                                                int *arg)
{
    bcm_port_t local_port;
    int id, startq;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_info_t *si;
    uint32 rval;
    int phy_port, mmu_port;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);

    if (type == bcmCosqControlEgressUCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                             _BCM_TD_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                             &local_port, NULL, NULL));
            BCM_IF_ERROR_RETURN(_bcm_td_cosq_node_get(unit, gport, NULL, NULL, &id, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            id = cosq;
        }

        phy_port = si->port_l2p_mapping[local_port];
        mmu_port = si->port_p2m_mapping[phy_port];

        /* regular unicast queue */
        if (si->port_num_ext_cosq[local_port] == 0) {
            /* Front-panel ports */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_0m;
                startq = (mmu_port - 5) * 10 + id;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_1m;
                startq = (mmu_port - 38) * 10 + id;
            }
        } else {
            /* extended queue ports
             *     use MMU_THDO_CONFIG_EX_x table, 74 entries per port
             *     extended unicast queues: index 0-63
             *     regular unicast queue: index 64-73
             */
            if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
                mem = MMU_THDO_CONFIG_EX_0m;
                startq = (mmu_port - 1) * 74 + 64 + id;
            } else { /* Y pipe */
                mem = MMU_THDO_CONFIG_EX_1m;
                startq = (mmu_port - 34) * 74 + 64 + id;
            }
        }

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, Q_LIMIT_ENABLE_CELLf);
    } else if (type == bcmCosqControlEgressMCQueueLimitEnable) {
        if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            if (cosq != BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                         _BCM_TD_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            startq = cosq;
        }

        BCM_IF_ERROR_RETURN
            (READ_OP_QUEUE_CONFIG1_CELLr(unit, local_port, startq, &rval));
        *arg = soc_reg_field_get(unit, OP_QUEUE_CONFIG1_CELLr, rval,
                                     Q_LIMIT_ENABLE_CELLf);
    }else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

int
bcm_td_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    switch (type) {
    case bcmCosqControlEgressPool:
    case bcmCosqControlEgressPoolLimitBytes:
    case bcmCosqControlEgressPoolYellowLimitBytes:
    case bcmCosqControlEgressPoolRedLimitBytes:
    case bcmCosqControlEgressPoolLimitEnable:
    case bcmCosqControlUCEgressPool:
    case bcmCosqControlMCEgressPool:
        return _bcm_td_cosq_egr_pool_set(unit, gport, cosq, type, arg);
    case bcmCosqControlDropLimitAlpha:
        return _bcm_td_cosq_alpha_set(unit, gport, cosq, 
                            (bcm_cosq_control_drop_limit_alpha_value_t)arg);
    case bcmCosqControlIngressPortPGSharedDynamicEnable:        
    case bcmCosqControlEgressUCSharedDynamicEnable:
    case bcmCosqControlEgressMCSharedDynamicEnable:
        return _bcm_td_cosq_dynamic_thresh_enable_set(unit, gport, cosq, 
                                                       type, arg);        
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        return _bcm_td_cosq_egr_queue_set(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
        return _bcm_td_cosq_egr_queue_limit_enable_set(unit, gport, cosq,
                                                        type, arg);

    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

int
bcm_td_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int *arg)
{
    switch (type) {
    case bcmCosqControlEgressPool:
    case bcmCosqControlEgressPoolLimitBytes:
    case bcmCosqControlEgressPoolYellowLimitBytes:
    case bcmCosqControlEgressPoolRedLimitBytes:
    case bcmCosqControlEgressPoolLimitEnable:
    case bcmCosqControlUCEgressPool:
    case bcmCosqControlMCEgressPool:
        return _bcm_td_cosq_egr_pool_get(unit, gport, cosq, type, arg);
    case bcmCosqControlDropLimitAlpha:
        return _bcm_td_cosq_alpha_get(unit, gport, cosq,
                            (bcm_cosq_control_drop_limit_alpha_value_t *)arg);
    case bcmCosqControlIngressPortPGSharedDynamicEnable:        
    case bcmCosqControlEgressUCSharedDynamicEnable:
    case bcmCosqControlEgressMCSharedDynamicEnable:
        return _bcm_td_cosq_dynamic_thresh_enable_get(unit, gport, cosq, 
                                                       type, arg);        
    case bcmCosqControlEgressUCQueueSharedLimitBytes:
    case bcmCosqControlEgressMCQueueSharedLimitBytes:
    case bcmCosqControlEgressUCQueueMinLimitBytes:
    case bcmCosqControlEgressMCQueueMinLimitBytes:
        return _bcm_td_cosq_egr_queue_get(unit, gport, cosq, type, arg);
    case bcmCosqControlEgressUCQueueLimitEnable:
    case bcmCosqControlEgressMCQueueLimitEnable:
        return _bcm_td_cosq_egr_queue_limit_enable_get(unit, gport, cosq,
                                                        type, arg);

    default:
        break;
    }

    return BCM_E_UNAVAIL;
}

#if 0
STATIC int
_bcm_td_cosq_gport_detach(int unit, bcm_port_t port)
{
    soc_info_t *si;
    int phy_port, mmu_port;
    int cosq, weights[16];
    uint32 rval, fval_hi, fval_lo;
    uint64 rval64, fval64;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    /* Detach from physical port */
    BCM_IF_ERROR_RETURN(READ_ING_COS_MODEr(unit, port, &rval));
    soc_reg_field_set(unit, ING_COS_MODEr, &rval, QUEUE_MODEf, 0);
    soc_reg_field_set(unit, ING_COS_MODEr, &rval, COS_MODEf, 0);
    BCM_IF_ERROR_RETURN(WRITE_ING_COS_MODEr(unit, port, rval));

    if (si->port_group[port] < 2) {
        /* X pipe COS_MODE_SEL has 32 ports (mmu port 1-32) */
        BCM_IF_ERROR_RETURN(READ_COS_MODE_Xr(unit, &rval64));
        fval_lo = soc_reg64_field32_get(unit, COS_MODE_Xr, rval64,
                                        COS_MODE_SELf);
        fval_lo &= ~(1 << (mmu_port - 1));
        soc_reg64_field32_set(unit, COS_MODE_Xr, &rval64, COS_MODE_SELf,
                              fval_lo);
        BCM_IF_ERROR_RETURN(WRITE_COS_MODE_Xr(unit, rval64));
    } else {
        /* Y pipe COS_MODE_SEL has 33 ports (mmu port 33-65) */
        BCM_IF_ERROR_RETURN(READ_COS_MODE_Yr(unit, &rval64));
        fval64 = soc_reg64_field_get(unit, COS_MODE_Yr, rval64, COS_MODE_SELf);
        fval_lo = COMPILER_64_LO(fval64);
        fval_hi = COMPILER_64_HI(fval64);
        if (mmu_port - 33 < 32) {
            fval_lo &= ~(1 << (mmu_port - 33));
        } else {
            fval_hi &= ~(1 << (mmu_port - 33 - 32));
        }
        COMPILER_64_SET(fval64, fval_hi, fval_lo);
        soc_reg64_field_set(unit, COS_MODE_Yr, &rval64, COS_MODE_SELf, fval64);
        BCM_IF_ERROR_RETURN(WRITe_COS_MODE_Yr(unit, rval64));
    }

    
    /* Clear bandwidth of all Stage1 COSQs */
    
    for (cosq = 8; cosq < 24; cosq++) {
        BCM_IF_ERROR_RETURN(bcm_td_cosq_bucket_set(unit, port, cosq, 0, 0, 0));
    }

    /* Clear weights of all Stage1 COSQs */
    for (cosq = 0; cosq < 16; cosq++) {
        weights[cosq] = 0;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_sched_set(unit, port, 0, BCM_COSQ_WEIGHTED_ROUND_ROBIN,
                                16, weights));

    /* Clear weight for S1 output queue */
    cosq = 8;
    BCM_IF_ERROR_RETURN(READ_COSWEIGHTSr(unit, port, cosq, &val));
    soc_reg_field_set(unit, COSWEIGHTSr, &val, COSWEIGHTSf, 0);
    BCM_IF_ERROR_RETURN(WRITE_COSWEIGHTSr(unit, port, cosq, val));

    /* Clear discard settings for all Stage1 COSQs */
    for (cosq = 8; cosq < 24; cosq++) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_wred_set(unit, port, cosq,
                                   BCM_COSQ_DISCARD_COLOR_ALL |
                                   BCM_COSQ_DISCARD_NONTCP,
                                   TD_CELL_FIELD_MAX,
                                   TD_CELL_FIELD_MAX, 100, 0,
                                   TRUE));
    }

    _td_sched_num_cosq[unit][port] = 0;

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *     bcm_td_cosq_gport_add
 * Purpose:
 *     Create a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     port  - (IN) port number
 *     numq  - (IN) number of COS queues
 *     flags - (IN) flags (BCM_COSQ_GPORT_XXX)
 *     gport - (OUT) GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32 flags,
                      bcm_gport_t *gport)
{
    soc_info_t *si;
    _bcm_td_cosq_port_info_t *port_info;
    _bcm_td_cosq_node_t *node;
    bcm_module_t local_modid, modid_out;
    bcm_port_t local_port, port_out;
    int id;
    uint32 sched_encap;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td_cosq_gport_add: unit=%d port=0x%x numq=%d flags=0x%x\n"),
              unit, port, numq, flags));

    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }    

    if (gport == NULL) {
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);

    if (_bcm_td_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_localport_resolve(unit, port, &local_port));

    if (local_port < 0 || IS_CPU_PORT(unit, local_port) ||
        IS_LB_PORT(unit, local_port)) {
        return BCM_E_PORT;
    }

    port_info = &_bcm_td_cosq_port_info[unit][local_port];
    switch (flags) {
    case BCM_COSQ_GPORT_UCAST_QUEUE_GROUP:
        if (numq != 1) {
            return BCM_E_PARAM;
        }
        for (id = 0; id < _BCM_TD_NUM_UCAST_QUEUE_GROUP; id++) {
            if (port_info->ucast[id].numq == 0) {
                break;
            }
        }
        if (id == _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
            return BCM_E_RESOURCE;
        }
        BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        node = &port_info->ucast[id];
        break;
    case BCM_COSQ_GPORT_MCAST_QUEUE_GROUP:
        if (numq != 1) {
            return BCM_E_PARAM;
        }
        for (id = 0; id < _BCM_TD_NUM_MCAST_QUEUE_GROUP; id++) {
            if (port_info->mcast[id].numq == 0) {
                break;
            }
        }
        if (id == _BCM_TD_NUM_MCAST_QUEUE_GROUP) {
            return BCM_E_RESOURCE;
        }
        BCM_GPORT_MCAST_QUEUE_GROUP_SYSQID_SET(*gport, local_port, id);
        node = &port_info->mcast[id];
        break;
    case BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP:
        if (si->port_num_ext_cosq[local_port] == 0) { /* regular port */
            return BCM_E_PARAM;
        }
        if (numq <= 0 || numq > 4) {
            return BCM_E_PARAM;
        }
        for (id = 0; id < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP; id++) {
            if (port_info->ext_ucast[id].numq == 0) {
                break;
            }
        }
        if (id == _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP) {
            return BCM_E_RESOURCE;
        }
        BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET
            (*gport, local_port, _BCM_TD_NUM_UCAST_QUEUE_GROUP + id);
        node = &port_info->ext_ucast[id];

        /* create VOQ profile which maps internal priorities evenly to
         * queues */
        SOC_IF_ERROR_RETURN
            (_bcm_td_create_default_profile(unit, *gport, numq, 
                                            &node->voq_profile));

        break;
    case BCM_COSQ_GPORT_SCHEDULER:
    case 0:
        /*
         * Can not validate actual number of cosq until attach time.
         * The max number of configurable input:
         *   S1: 7 (S2.0, S2.1, S2.2, mcast group, multiast SC/QM queue,
         *          unicast SC queue, unicast QM queue)
         *   S2: 9 (hardware allows to select up to 9 of the possible 12 input)
         *   S3: 18 (1 regular ucast queue, 1 mcast queue,
         *           16 extended ucast queues)
         */
        if (si->port_num_ext_cosq[local_port] == 0) { /* regular port */
            if (numq <= 0 || numq > _BCM_TD_S2_SCHEDULER_NUM_INPUT) {
                return BCM_E_PARAM;
            }
        } else { /* extended queue port */
            if (numq <= 0 || numq > _BCM_TD_EXT_S3_SCHEDULER_NUM_INPUT) {
                return BCM_E_PARAM;
            }
        }
        for (id = 0; id < _BCM_TD_NUM_SCHEDULER; id++) {
            if (port_info->sched[id].numq == 0) {
                break;
            }
        }
        if (id == _BCM_TD_NUM_SCHEDULER) {
            return BCM_E_RESOURCE;
        }

        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &local_modid));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, local_modid,
                                     local_port, &modid_out, &port_out));
        sched_encap = (id << 16) | (modid_out << 8) | port_out;
        BCM_GPORT_SCHEDULER_SET(*gport, sched_encap);
        node = &port_info->sched[id];
        break;
    default:
        return BCM_E_PARAM;
    }

    node->gport = *gport;
    node->numq = numq;
    node->cosq_attached_to = -1;
    node->hw_cosq_attached_to = -1;
    node->level = 0;
    node->parent = NULL;
    node->sibling = NULL;
    node->child = NULL;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "                       gport=0x%x\n"),
              *gport));

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_gport_delete_all(int unit, bcm_gport_t gport)
{
    _bcm_td_cosq_node_t *node;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td_cosq_gport_delete: unit=%d gport=0x%x\n"),
              unit, gport));

    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }
    
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));
        if (node->child != NULL) {
            BCM_IF_ERROR_RETURN(
                    _bcm_td_cosq_gport_delete_all(unit, node->child->gport));
        }
        if (node->sibling != NULL) {
            BCM_IF_ERROR_RETURN(
                    _bcm_td_cosq_gport_delete_all(unit, node->sibling->gport));
        }   
        if (node->cosq_attached_to >= 0) {
            BCM_IF_ERROR_RETURN(
                bcm_td_cosq_gport_detach(unit, node->gport, BCM_GPORT_INVALID, -1));
        }       
        node->numq = 0;
    } else {
        return BCM_E_PORT;
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_delete
 * Purpose:
 *     Destroy a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (IN) GPORT identifier
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    _bcm_td_cosq_node_t *node = NULL, *tnode;
    _bcm_td_cosq_port_info_t *port_info;
    int ii, local_port;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td_cosq_gport_delete: unit=%d gport=0x%x\n"),
              unit, gport));

    if (!soc_feature(unit, soc_feature_ets)) {
        return BCM_E_UNAVAIL;
    }
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) || 
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, gport, NULL, NULL, NULL, &node));

        if (node->cosq_attached_to >= 0) {
            BCM_IF_ERROR_RETURN
                (bcm_td_cosq_gport_detach(unit, node->gport, BCM_GPORT_INVALID, -1));
        }

        node->numq = 0;
    } else {
        local_port = (BCM_GPORT_IS_LOCAL(gport)) ?
           BCM_GPORT_LOCAL_GET(gport) : BCM_GPORT_MODPORT_PORT_GET(gport);

        if (!SOC_PORT_VALID(unit, local_port)) {
            return BCM_E_PORT;
        }

        port_info = &_bcm_td_cosq_port_info[unit][local_port];
        for (ii = 0; ii < _BCM_TD_NUM_SCHEDULER; ii++) {
            tnode = &port_info->sched[ii];
            if (port_info->sched[ii].numq == 0) {
                continue;
            }
            /* s1 node */
            if (tnode->level == 1) {   
                node = tnode;
                break;
            }
        }
        if (node == NULL) {
            return BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_td_cosq_gport_delete_all(unit, node->gport));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_get
 * Purpose:
 *     Get a cosq gport structure
 * Parameters:
 *     unit  - (IN) unit number
 *     gport - (IN) GPORT identifier
 *     port  - (OUT) port number
 *     numq  - (OUT) number of COS queues
 *     flags - (OUT) flags (BCM_COSQ_GPORT_XXX)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                      int *numq, uint32 *flags)
{
    _bcm_td_cosq_node_t *node;
    bcm_module_t modid;
    bcm_port_t local_port;
    int id;
    _bcm_gport_dest_t dest;

    if (port == NULL || numq == NULL || flags == NULL) {
        return BCM_E_PARAM;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td_cosq_gport_get: unit=%d gport=0x%x\n"),
              unit, gport));

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_node_get(unit, gport, NULL, &local_port, &id, &node));

    if (SOC_USE_GPORT(unit)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
        dest.modid = modid;
        dest.port = local_port;
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, port));
    } else {
        *port = local_port;
    }

    *numq = node->numq;

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        if (id < _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
            *flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
        } else {
            *flags = BCM_COSQ_GPORT_DESTMOD_UCAST_QUEUE_GROUP;
        }
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        *flags = BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;
    } else if (BCM_GPORT_IS_SCHEDULER(gport)) {
        *flags = BCM_COSQ_GPORT_SCHEDULER;
    } else {
        *flags = 0;
    }

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "                       port=0x%x numq=%d flags=0x%x\n"),
              *port, *numq, *flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_traverse
 * Purpose:
 *     Walks through the valid COSQ GPORTs and calls
 *     the user supplied callback function for each entry.
 * Parameters:
 *     unit       - (IN) unit number
 *     trav_fn    - (IN) Callback function.
 *     user_data  - (IN) User data to be passed to callback function
 * Returns:
 *     BCM_E_NONE - Success.
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                           void *user_data)
{
    _bcm_td_cosq_port_info_t *port_info;
    bcm_module_t my_modid, modid_out;
    bcm_port_t port, port_out;
    bcm_gport_t gport;
    int id, rv = BCM_E_NONE;

    if (_bcm_td_cosq_port_info[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
    PBMP_ALL_ITER(unit, port) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    my_modid, port, &modid_out, &port_out));
        BCM_GPORT_MODPORT_SET(gport, modid_out, port_out);

        port_info = &_bcm_td_cosq_port_info[unit][port];
        for (id = 0; id < _BCM_TD_NUM_UCAST_QUEUE_GROUP; id++) {
            if (port_info->ucast[id].numq == 0) {
                continue;
            }
            /* coverity[value_overwrite] */
            rv = cb(unit, gport, port_info->ucast[id].numq,
                    BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                    port_info->ucast[id].gport, user_data);
            if (BCM_FAILURE(rv)) {
#ifdef BCM_CB_ABORT_ON_ERR
                if (SOC_CB_ABORT_ON_ERR(unit)) {
                    return rv;
                }
#endif
            }
        }
        for (id = 0; id < _BCM_TD_NUM_MCAST_QUEUE_GROUP; id++) {
            if (port_info->mcast[id].numq == 0) {
                continue;
            }
            /* coverity[value_overwrite] */
            rv = cb(unit, gport, port_info->mcast[id].numq,
                    BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                    port_info->mcast[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#endif
        }
        for (id = 0; id < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP; id++) {
            if (port_info->ext_ucast[id].numq == 0) {
                continue;
            }
            /* coverity[value_overwrite] */
            rv = cb(unit, gport, port_info->ext_ucast[id].numq,
                    BCM_COSQ_GPORT_UCAST_QUEUE_GROUP,
                    port_info->ext_ucast[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#endif
        }
        for (id = 0; id < _BCM_TD_NUM_SCHEDULER; id++) {
            if (port_info->sched[id].numq == 0) {
                continue;
            }
            /* coverity[value_overwrite] */
            rv = cb(unit, gport, port_info->sched[id].numq,
                    BCM_COSQ_GPORT_SCHEDULER,
                    port_info->sched[id].gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
            if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
                return rv;
            }
#endif
        }
    }

    return BCM_E_NONE;
}

int
bcm_td_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    return _bcm_td_cosq_mapping_set(unit, ing_port, int_pri, flags, gport,
                                    cosq);
}

int
bcm_td_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                              bcm_cos_t int_pri, uint32 flags,
                              bcm_gport_t *gport, bcm_cos_queue_t *cosq)
{
    if (gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    return _bcm_td_cosq_mapping_get(unit, ing_port, int_pri, flags, gport,
                                    cosq);
}

/*
 * Function:
 *     bcm_td_cosq_gport_bandwidth_set
 * Purpose:
 *     Configure COS queue bandwidth control
 * Parameters:
 *     unit   - (IN) unit number
 *     gport  - (IN) GPORT identifier
 *     cosq   - (IN) COS queue to configure, -1 for all COS queues
 *     kbits_sec_min - (IN) minimum bandwidth, kbits/sec
 *     kbits_sec_max - (IN) maximum bandwidth, kbits/sec
 *     flags  - (IN) BCM_COSQ_BW_*
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 kbits_sec_min,
                                uint32 kbits_sec_max, uint32 flags)
{
    int numq, i;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_BUCKET,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_bucket_set(unit, gport, cosq + i, kbits_sec_min,
                                     kbits_sec_max, kbits_sec_min, 
                                     kbits_sec_max, flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_bandwidth_get
 * Purpose:
 *     Get COS queue bandwidth control configuration
 * Parameters:
 *     unit   - (IN) unit number
 *     gport  - (IN) GPORT identifier
 *     cosq   - (IN) COS queue to get, -1 for any COS queue
 *     kbits_sec_min - (OUT) minimum bandwidth, kbits/sec
 *     kbits_sec_max - (OUT) maximum bandwidth, kbits/sec
 *     flags  - (OUT) BCM_COSQ_BW_*
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq, uint32 *kbits_sec_min,
                                uint32 *kbits_sec_max, uint32 *flags)
{
    uint32 kbits_sec_burst;

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                 kbits_sec_min, kbits_sec_max,
                                 &kbits_sec_burst, &kbits_sec_burst, flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_bandwidth_burst_set
 * Purpose:
 *     Configure COS queue bandwidth burst setting
 * Parameters:
 *      unit        - (IN) unit number
 *      gport       - (IN) GPORT identifier
 *      cosq        - (IN) COS queue to configure, -1 for all COS queues
 *      kbits_burst - (IN) maximum burst, kbits
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_cosq_gport_bandwidth_burst_set(int unit, bcm_gport_t gport,
                                      bcm_cos_queue_t cosq,
                                      uint32 kbits_burst_min,
                                      uint32 kbits_burst_max)
{
    int numq, i;
    uint32 kbits_sec_min, kbits_sec_max, kbits_sec_burst, flags = 0;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_BUCKET,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_bucket_get(unit, gport, cosq + i, &kbits_sec_min,
                                     &kbits_sec_max, &kbits_sec_burst,
                                     &kbits_sec_burst, &flags));
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_bucket_set(unit, gport, cosq + i, kbits_sec_min,
                                     kbits_sec_max, kbits_burst_min,
                                     kbits_burst_max, flags));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_bandwidth_burst_get
 * Purpose:
 *     Get COS queue bandwidth burst setting
 * Parameters:
 *     unit        - (IN) unit number
 *     gport       - (IN) GPORT identifier
 *     cosq        - (IN) COS queue to get, -1 for any queue
 *     kbits_burst - (OUT) maximum burst, kbits
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_bandwidth_burst_get(int unit, bcm_gport_t gport,
                                       bcm_cos_queue_t cosq,
                                       uint32 *kbits_burst_min,
                                       uint32 *kbits_burst_max)
{
    uint32 kbits_sec_min, kbits_sec_max, flags;

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_bucket_get(unit, gport, cosq == -1 ? 0 : cosq,
                                 &kbits_sec_min, &kbits_sec_max, 
                                 kbits_burst_min, kbits_burst_max, &flags));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_sched_set
 * Purpose:
 *     Configure COS queue scheduler setting
 * Parameters:
 *      unit   - (IN) unit number
 *      gport  - (IN) GPORT identifier
 *      cosq   - (IN) COS queue to configure, -1 for all COS queues
 *      mode   - (IN) Scheduling mode, one of BCM_COSQ_xxx
 *      weight - (IN) queue weight
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                            bcm_cos_queue_t cosq, int mode, int weight)
{
    int rv, numq, i, count;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    count = 0;
    for (i = 0; i < numq; i++) {
        rv = _bcm_td_cosq_sched_set(unit, gport, cosq + i, mode, 1, &weight);
        if (rv == BCM_E_NOT_FOUND) {
            continue;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        } else {
            count++;
        }
    }

    return count > 0 ? BCM_E_NONE : BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_td_cosq_gport_sched_get
 * Purpose:
 *     Get COS queue scheduler setting
 * Parameters:
 *     unit   - (IN) unit number
 *     gport  - (IN) GPORT identifier
 *     cosq   - (IN) COS queue to get, -1 for any queue
 *     mode   - (OUT) Scheduling mode, one of BCM_COSQ_xxx
 *     weight - (OUT) queue weight
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_sched_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                            int *mode, int *weight)
{
    int rv, numq, i;

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_SCHEDULER,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        rv = _bcm_td_cosq_sched_get(unit, gport, cosq + i, mode, 1, weight);
        if (rv == BCM_E_NOT_FOUND) {
            continue;
        } else {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *     bcm_td_cosq_gport_discard_set
 * Purpose:
 *     Configure port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) GPORT identifier
 *     cosq    - (IN) COS queue to configure, -1 for all COS queues
 *     discard - (IN) discard settings
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_gport_discard_t *discard)
{
    int numq, i;
    uint32 min_thresh, max_thresh;
    int cell_size, cell_field_max;

    if (discard == NULL ||
        discard->gain < 0 || discard->gain > 15 ||
        discard->drop_probability < 0 || discard->drop_probability > 100 ||
        (discard->min_thresh > discard->max_thresh) ||
        (discard->min_thresh < 0)) {
        return BCM_E_PARAM;
    }

    cell_size = _BCM_TD_BYTES_PER_CELL;
    cell_field_max = TD_CELL_FIELD_MAX;

    min_thresh = discard->min_thresh;
    max_thresh = discard->max_thresh;
    if (discard->flags & BCM_COSQ_DISCARD_BYTES) {
        /* Convert bytes to cells */
        min_thresh += (cell_size - 1);
        min_thresh /= cell_size;

        max_thresh += (cell_size - 1);
        max_thresh /= cell_size;

        if ((min_thresh > cell_field_max) ||
            (max_thresh > cell_field_max)) {
            return BCM_E_PARAM;
        }
    } else {
        /* Packet mode not supported */
        return BCM_E_PARAM;
    }

    if (cosq == -1) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_index_resolve(unit, gport, cosq,
                                        _BCM_TD_COSQ_INDEX_STYLE_WRED,
                                        NULL, NULL, &numq));
        cosq = 0;
    } else {
        numq = 1;
    }

    for (i = 0; i < numq; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_wred_set(unit, gport, cosq + i, discard->flags,
                                   min_thresh, max_thresh,
                                   discard->drop_probability, discard->gain,
                                   FALSE));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_discard_get
 * Purpose:
 *     Get port WRED setting
 * Parameters:
 *     unit    - (IN) unit number
 *     port    - (IN) GPORT identifier
 *     cosq    - (IN) COS queue to get, -1 for any queue
 *     discard - (OUT) discard settings
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq,
                              bcm_cosq_gport_discard_t *discard)
{
    uint32 min_thresh, max_thresh;

    if (discard == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_wred_get(unit, gport, cosq == -1 ? 0 : cosq,
                               &discard->flags, &min_thresh, &max_thresh,
                               &discard->drop_probability, &discard->gain));

    /* Convert number of cells to number of bytes */
    discard->min_thresh = min_thresh * _BCM_TD_BYTES_PER_CELL;
    discard->max_thresh = max_thresh * _BCM_TD_BYTES_PER_CELL;

    return BCM_E_NONE;
}

STATIC int
_bcm_td_cosq_pfc_class_resolve(bcm_switch_control_t sctype, int *type,
                               int *class)
{
    switch (sctype) {
    case bcmSwitchPFCClass7Queue:
    case bcmSwitchPFCClass6Queue:
    case bcmSwitchPFCClass5Queue:
    case bcmSwitchPFCClass4Queue:
    case bcmSwitchPFCClass3Queue:
    case bcmSwitchPFCClass2Queue:
    case bcmSwitchPFCClass1Queue:
    case bcmSwitchPFCClass0Queue:
        *type = _BCM_TD_COSQ_TYPE_UCAST;
        break;
    case bcmSwitchPFCClass7McastQueue:
    case bcmSwitchPFCClass6McastQueue:
    case bcmSwitchPFCClass5McastQueue:
    case bcmSwitchPFCClass4McastQueue:
    case bcmSwitchPFCClass3McastQueue:
    case bcmSwitchPFCClass2McastQueue:
    case bcmSwitchPFCClass1McastQueue:
    case bcmSwitchPFCClass0McastQueue:
        *type = _BCM_TD_COSQ_TYPE_MCAST;
        break;
    case bcmSwitchPFCClass7DestmodQueue:
    case bcmSwitchPFCClass6DestmodQueue:
    case bcmSwitchPFCClass5DestmodQueue:
    case bcmSwitchPFCClass4DestmodQueue:
    case bcmSwitchPFCClass3DestmodQueue:
    case bcmSwitchPFCClass2DestmodQueue:
    case bcmSwitchPFCClass1DestmodQueue:
    case bcmSwitchPFCClass0DestmodQueue:
        *type = _BCM_TD_COSQ_TYPE_EXT_UCAST;
        break;
    default:
        return BCM_E_PARAM;
    }

    switch (sctype) {
    case bcmSwitchPFCClass7Queue:
    case bcmSwitchPFCClass7McastQueue:
    case bcmSwitchPFCClass7DestmodQueue:
        *class = 7;
        break;
    case bcmSwitchPFCClass6Queue:
    case bcmSwitchPFCClass6McastQueue:
    case bcmSwitchPFCClass6DestmodQueue:
        *class = 6;
        break;
    case bcmSwitchPFCClass5Queue:
    case bcmSwitchPFCClass5McastQueue:
    case bcmSwitchPFCClass5DestmodQueue:
        *class = 5;
        break;
    case bcmSwitchPFCClass4Queue:
    case bcmSwitchPFCClass4McastQueue:
    case bcmSwitchPFCClass4DestmodQueue:
        *class = 4;
        break;
    case bcmSwitchPFCClass3Queue:
    case bcmSwitchPFCClass3McastQueue:
    case bcmSwitchPFCClass3DestmodQueue:
        *class = 3;
        break;
    case bcmSwitchPFCClass2Queue:
    case bcmSwitchPFCClass2McastQueue:
    case bcmSwitchPFCClass2DestmodQueue:
        *class = 2;
        break;
    case bcmSwitchPFCClass1Queue:
    case bcmSwitchPFCClass1McastQueue:
    case bcmSwitchPFCClass1DestmodQueue:
        *class = 1;
        break;
    case bcmSwitchPFCClass0Queue:
    case bcmSwitchPFCClass0McastQueue:
    case bcmSwitchPFCClass0DestmodQueue:
        *class = 0;
        break;
    /*
     * COVERITY
     *
     * This default is unreachable but needed for some compiler. 
     */ 
    /* coverity[dead_error_begin] */
    default:
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_fc_status_map_gport
 * Purpose:
 *     Update cos_bmp with gport cosq.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number
 *     type       - (IN) cosq type
 *     gport      - (IN) gport to be mapped
 *     cos_map    - (OUT) hw_cosq of gport
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_fc_status_map_gport(int unit, int port, int type, bcm_gport_t gport,
                            _bcm_td_cosq_pfc_cos_bmp_t *cos_bmp)
{
    bcm_port_t local_port;
    bcm_port_t resolved_port;
    _bcm_td_cosq_node_t *node;
    uint32 uc_bmp, mc_bmp;
    int id, cnt;
    int ext_uc_count;
    int *ext_uc_qid = NULL;

    if (NULL == cos_bmp) {
        return BCM_E_PARAM;
    }
    
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_localport_resolve(unit, port, &local_port));

    uc_bmp = cos_bmp->uc_bmp;
    mc_bmp = cos_bmp->mc_bmp;
    ext_uc_count = cos_bmp->ext_uc_count;
    ext_uc_qid = &(cos_bmp->ext_uc_qid[0]);
    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_SCHEDULER(gport)) {
            return BCM_E_PORT;
        }

        BCM_IF_ERROR_RETURN(
                _bcm_td_cosq_node_get(unit, gport, NULL,
                                      &resolved_port, &id, &node));
        if (resolved_port != local_port) {
            return BCM_E_PORT;
        }
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            if (id > _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                /* extended unicast queue */
                if (type != _BCM_TD_COSQ_TYPE_EXT_UCAST) {
                    return BCM_E_PARAM;
                }
                if (ext_uc_count == _BCM_TD_MAX_NUM_LLFC_EXTQ) {
                    return BCM_E_RESOURCE;
                }
                for (cnt = 0; cnt < ext_uc_count; cnt++) {
                    if (ext_uc_qid[cnt] == node->base + 64) {
                        break;
                    }
                }
                if (cnt == ext_uc_count) {
                    ext_uc_qid[ext_uc_count] = node->base + 64;
                    ext_uc_count++;
                }
            } else { /* unicast queue */
                if (type != _BCM_TD_COSQ_TYPE_UCAST) {
                    return BCM_E_PARAM;
                }
                uc_bmp |= 1 << node->base;
            }
        } else { /* multicast queue */
            if (type != _BCM_TD_COSQ_TYPE_MCAST) {
                return BCM_E_PARAM;
            }
            mc_bmp |= 1 << node->base;
        }
    } else {
        if (type == _BCM_TD_COSQ_TYPE_UCAST) {
            bcm_cos_queue_t mc_cosq_max; 
            mc_cosq_max = (_BCM_TD_NUM_MCAST_QUEUE_GROUP - 1);
            uc_bmp |= 1 << gport;
            /* In the legacy mode, need to set both UC & MC queue */
            if (gport > mc_cosq_max) {
                mc_bmp |= 1 << mc_cosq_max;
            } else {
                mc_bmp |= 1 << gport;
            }
        } else {
            return BCM_E_PARAM;
        }
    }

    cos_bmp->uc_bmp = uc_bmp;
    cos_bmp->mc_bmp = mc_bmp;
    cos_bmp->ext_uc_count = ext_uc_count;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_port_pfc_profile_set
 * Purpose:
 *     Set PFC mapping profile for port. Updates PRIO2COS_LLFC & PORT_LLFC_CFG.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) port number
 *     op         - (IN) operation type
 *     entry_cnt  - (IN) count of pfc classes and according bitmaps
 *     pfc_class  - (IN) class list
 *     cos_bmp    - (IN) bmp list
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td_port_pfc_profile_set(int unit, int port, _bcm_cosq_op_t op,
                             int entry_cnt, int *pfc_class,
                             _bcm_td_cosq_pfc_cos_bmp_t *cos_bmp)
{
    static const soc_field_t valid_field[] = {
        Q_VLD0f, Q_VLD1f, Q_VLD2f, Q_VLD3f
    };
    static const soc_field_t num_field[] = {
        Q_NUM0f, Q_NUM1f, Q_NUM2f, Q_NUM3f
    };
    uint32 profile_index, old_profile_index;
    uint32 ext_profile_index, ext_old_profile_index;
    uint64 rval64[16], *rval64s[1];
    uint64 ext_rval64[16], *ext_rval64s[1];
    uint32 rval, fval;
    uint32 uc_bmp, mc_bmp;
    int rv, index, id, ext_uc_count, free_index, cur_ext_uc_qid[4];
    int *ext_uc_qid = NULL;
    int entry, cur_class, uc_mc_update = 0, euc_update = 0;

    rval64s[0] = rval64;
    ext_rval64s[0] = ext_rval64;
    BCM_IF_ERROR_RETURN(READ_PORT_LLFC_CFGr(unit, port, &rval));
    old_profile_index = soc_reg_field_get(unit, PORT_LLFC_CFGr, rval,
                                          PROFILE_INDEXf) * 16;
    BCM_IF_ERROR_RETURN(soc_profile_reg_get(unit, _bcm_td_llfc_profile[unit],
                                            old_profile_index, 16, rval64s));
    ext_old_profile_index = soc_reg_field_get(unit, PORT_LLFC_CFGr, rval,
                                              EXT_PER_Q_INDEXf) * 16;
    BCM_IF_ERROR_RETURN
        (soc_profile_reg_get(unit, _bcm_td_ext_llfc_profile[unit],
                             ext_old_profile_index, 16, ext_rval64s));
    
    for (entry = 0; entry < entry_cnt; entry++) {
        cur_class = pfc_class[entry];
        uc_bmp = cos_bmp[entry].uc_bmp;
        mc_bmp = cos_bmp[entry].mc_bmp;
        ext_uc_count = cos_bmp[entry].ext_uc_count;
        ext_uc_qid = &(cos_bmp[entry].ext_uc_qid[0]);
        
        if (uc_bmp != 0 || mc_bmp != 0) {
            if (uc_bmp != 0) {
                /* coverity[dead_error_condition : FALSE] */
                if (op == _BCM_COSQ_OP_SET) {
                    fval = uc_bmp;
                } else {
                    fval = soc_reg64_field32_get(unit, PRIO2COS_LLFCr,
                                                 rval64[cur_class],
                                                 UC_COS0_10_BMPf);
                    if (op == _BCM_COSQ_OP_ADD) {
                        fval |= uc_bmp;
                    } else { /* _BCM_COSQ_OP_DELETE */
                        fval &= ~uc_bmp;
                    }
                }
                soc_reg64_field32_set(unit, PRIO2COS_LLFCr, &rval64[cur_class],
                                      UC_COS0_10_BMPf, fval);
            }
            if (mc_bmp != 0) {
                if (op == _BCM_COSQ_OP_SET) {
                    fval = mc_bmp;
                } else {
                    fval = soc_reg64_field32_get(unit, PRIO2COS_LLFCr,
                                                 rval64[cur_class],
                                                 MC_COS0_5_BMPf);
                    if (op == _BCM_COSQ_OP_ADD) {
                        fval |= mc_bmp;
                    } else { /* _BCM_COSQ_OP_DELETE */
                        fval &= ~mc_bmp;
                    }
                }
                soc_reg64_field32_set(unit, PRIO2COS_LLFCr, &rval64[cur_class],
                                      MC_COS0_5_BMPf, fval);
            }
            uc_mc_update = 1;
        }

        if (ext_uc_count != 0) {
            if (op == _BCM_COSQ_OP_SET) {
                COMPILER_64_ZERO(ext_rval64[cur_class]);
                for (index = 0; index < ext_uc_count; index++) {
                    soc_reg64_field32_set(unit, PRIO2EXTQ_LLFCr,
                                          &ext_rval64[cur_class],
                                          valid_field[index], 1);
                    soc_reg64_field32_set(unit, PRIO2EXTQ_LLFCr,
                                          &ext_rval64[cur_class],
                                          num_field[index], ext_uc_qid[index]);
                }
            } else {
                for (id = 0; id < sizeof(num_field) / sizeof(num_field[0]);
                     id++) {
                    cur_ext_uc_qid[id] = -1;
                    if (soc_reg64_field32_get(unit, PRIO2EXTQ_LLFCr,
                                              ext_rval64[cur_class],
                                              valid_field[id])) {
                        cur_ext_uc_qid[id] =
                            soc_reg64_field32_get(unit, PRIO2EXTQ_LLFCr,
                                                  ext_rval64[cur_class],
                                                  num_field[id]);
                    }
                }
                if (op == _BCM_COSQ_OP_ADD) {
                    free_index = -1;
                    for (index = 0; index < ext_uc_count; index++) {
                        for (id = 0;
                             id < sizeof(num_field) / sizeof(num_field[0]);
                             id++) {
                            if (ext_uc_qid[index] == cur_ext_uc_qid[id]) {
                                /* already exist */
                                break;
                            }
                            if (free_index == -1 && cur_ext_uc_qid[id] == -1) {
                                free_index = id;
                            }
                        }
                        if (id < sizeof(num_field) / sizeof(num_field[0])) {
                            continue;
                        }
                        soc_reg64_field32_set(unit, PRIO2EXTQ_LLFCr,
                                              &ext_rval64[cur_class],
                                              valid_field[free_index], 1);
                        soc_reg64_field32_set(unit, PRIO2EXTQ_LLFCr,
                                              &ext_rval64[cur_class],
                                              num_field[free_index],
                                              ext_uc_qid[index]);
                    }
                } else { /* _BCM_COSQ_OP_DELETE */
                    for (index = 0; index < ext_uc_count; index++) {
                        for (id = 0;
                             id < sizeof(num_field) / sizeof(num_field[0]);
                             id++) {
                            if (ext_uc_qid[index] == cur_ext_uc_qid[id]) {
                                soc_reg64_field32_set(unit, PRIO2EXTQ_LLFCr,
                                                      &ext_rval64[cur_class],
                                                      valid_field[id], 0);
                                break;
                            }
                        }
                    }
                }
            }
            euc_update = 1;
        }
    }

    if (uc_mc_update) {
        BCM_IF_ERROR_RETURN
                (soc_profile_reg_delete(unit, _bcm_td_llfc_profile[unit],
                                        old_profile_index));
        rv = soc_profile_reg_add(unit, _bcm_td_llfc_profile[unit], rval64s,
                                 16, &profile_index);
        if (SOC_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN
                (soc_profile_reg_reference(unit, _bcm_td_llfc_profile[unit],
                                           old_profile_index, 16));
            return rv;
        }
        soc_reg_field_set(unit, PORT_LLFC_CFGr, &rval, PROFILE_INDEXf,
                          profile_index / 16);
    }
    if (euc_update) {
        BCM_IF_ERROR_RETURN
                (soc_profile_reg_delete(unit, _bcm_td_ext_llfc_profile[unit],
                                        ext_old_profile_index));
        rv = soc_profile_reg_add(unit, _bcm_td_ext_llfc_profile[unit],
                                 ext_rval64s, 16, &ext_profile_index);
        if (SOC_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN
                (soc_profile_reg_reference(unit, _bcm_td_ext_llfc_profile[unit],
                                           ext_old_profile_index, 16));
            return rv;
        }
        soc_reg_field_set(unit, PORT_LLFC_CFGr, &rval, EXT_PER_Q_INDEXf,
                          ext_profile_index / 16);
    }
    BCM_IF_ERROR_RETURN(WRITE_PORT_LLFC_CFGr(unit, port, rval));

    return BCM_E_NONE;
}

int
bcm_td_cosq_port_pfc_op(int unit, bcm_port_t port,
                        bcm_switch_control_t sctype, _bcm_cosq_op_t op,
                        bcm_gport_t *gport, int gport_count)
{
    _bcm_td_cosq_pfc_cos_bmp_t cos_bmp;
    int type = -1, class = -1, index;    

    if (IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    }
    if (gport_count < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_td_cosq_pfc_class_resolve(sctype, &type, &class));

    sal_memset(&cos_bmp, 0, sizeof(cos_bmp));
    for (index = 0; index < gport_count; index++) {
        if (!BCM_GPORT_IS_SET(gport[index]) && (_BCM_COSQ_OP_CLEAR != op) && 
            (gport[index] < 0 || gport[index] >= _BCM_TD_NUM_UCAST_QUEUE_GROUP)
           ) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_td_fc_status_map_gport(unit, port, type, gport[index],
                                        &cos_bmp));
    }

    if (op == _BCM_COSQ_OP_CLEAR) {
        cos_bmp.uc_bmp = (1 << _BCM_TD_NUM_UCAST_QUEUE_GROUP) - 1;
        cos_bmp.mc_bmp = (1 << _BCM_TD_NUM_MCAST_QUEUE_GROUP) - 1;
    }

    BCM_IF_ERROR_RETURN(_bcm_td_port_pfc_profile_set(unit, port, op,
                                                     1, &class, &cos_bmp));

    return BCM_E_NONE;
}

int
bcm_td_cosq_port_pfc_get(int unit, bcm_port_t port,
                         bcm_switch_control_t sctype,
                         bcm_gport_t *gport, int gport_count,
                         int *actual_gport_count)
{
    _bcm_td_cosq_port_info_t *port_info;
    bcm_port_t local_port;
    int type = -1, class = -1, id, hw_cosq, count, i;
    static const soc_field_t valid_field[] = {
        Q_VLD0f, Q_VLD1f, Q_VLD2f, Q_VLD3f
    };
    static const soc_field_t num_field[] = {
        Q_NUM0f, Q_NUM1f, Q_NUM2f, Q_NUM3f
    };
    uint32 profile_index;
    uint64 rval64[16], *rval64s[1];
    uint32 rval, bmp;

    if (IS_CPU_PORT(unit, port)) {
        return BCM_E_PORT;
    }

    if ((gport == NULL) && (gport_count > 0)) {
        return BCM_E_PARAM;
    }
    
    if (actual_gport_count == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_localport_resolve(unit, port, &local_port));

    BCM_IF_ERROR_RETURN(_bcm_td_cosq_pfc_class_resolve(sctype, &type, &class));

    port_info = &_bcm_td_cosq_port_info[unit][local_port];
    count = 0;

    rval64s[0] = rval64;
    BCM_IF_ERROR_RETURN(READ_PORT_LLFC_CFGr(unit, local_port, &rval));
    if (type == _BCM_TD_COSQ_TYPE_UCAST || type == _BCM_TD_COSQ_TYPE_MCAST) {
        profile_index = soc_reg_field_get(unit, PORT_LLFC_CFGr, rval,
                                          PROFILE_INDEXf) * 16;
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_get(unit, _bcm_td_llfc_profile[unit],
                                 profile_index, 16, rval64s));
        if (type == _BCM_TD_COSQ_TYPE_UCAST) {
            bmp = soc_reg64_field32_get(unit, PRIO2COS_LLFCr, rval64[class],
                                        UC_COS0_10_BMPf);
            for (hw_cosq = 0; hw_cosq < _BCM_TD_NUM_UCAST_QUEUE_GROUP;
                 hw_cosq++) {
                if (!(bmp & (1 << hw_cosq))) {
                    continue;
                }
                for (id = 0; id < _BCM_TD_NUM_UCAST_QUEUE_GROUP; id++) {
                    if (port_info->ucast[id].parent == NULL || 
                        port_info->ucast[id].base != hw_cosq) {
                        continue;
                    }
                    if (gport_count > 0) {
                        gport[count] = port_info->ucast[id].gport;
                    }
                    count++;
                    break;
                }
                if (id == _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    if (gport_count > 0) {
                        gport[count] = hw_cosq;
                    }
                    count++;
                }
                if ((gport_count > 0) && (count == gport_count)) {
                    break;
                }
            }
        } else {
            bmp = soc_reg64_field32_get(unit, PRIO2COS_LLFCr, rval64[class],
                                        MC_COS0_5_BMPf);
            for (hw_cosq = 0; hw_cosq < _BCM_TD_NUM_MCAST_QUEUE_GROUP;
                 hw_cosq++) {
                if (!(bmp & (1 << hw_cosq))) {
                    continue;
                }
                for (id = 0; id < _BCM_TD_NUM_MCAST_QUEUE_GROUP; id++) {
                    if (port_info->mcast[id].parent == NULL ||
                        port_info->mcast[id].base != hw_cosq) {
                        continue;
                    }
                    if (gport_count > 0) {
                        gport[count] = port_info->mcast[id].gport;
                    }
                    count++;
                    break;
                }
                if (id == _BCM_TD_NUM_MCAST_QUEUE_GROUP) {
                    if (gport_count > 0) {
                        gport[count] = hw_cosq;
                    }
                    count++;
                }
                if ((gport_count > 0) && (count == gport_count)) {
                    break;
                }
            }
        }
    } else {
        profile_index = soc_reg_field_get(unit, PORT_LLFC_CFGr, rval,
                                          EXT_PER_Q_INDEXf) * 16;
        BCM_IF_ERROR_RETURN
            (soc_profile_reg_get(unit, _bcm_td_ext_llfc_profile[unit],
                                 profile_index, 16, rval64s));
        for (i = 0; i < sizeof(num_field) / sizeof(num_field[0]); i++) {
            if (!soc_reg64_field32_get(unit, PRIO2EXTQ_LLFCr,
                                      rval64[class], valid_field[i])) {
                continue;
            }
            hw_cosq = soc_reg64_field32_get(unit, PRIO2EXTQ_LLFCr,
                                            rval64[class], num_field[i]) - 64;
            for (id = 0; id < _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP; id++) {
                if (port_info->ext_ucast[id].parent == NULL ||
                    port_info->ext_ucast[id].base != hw_cosq) {
                    continue;
                }
                if (gport_count > 0) {
                    gport[count] = port_info->ext_ucast[id].gport;
                }
                count++;
                break;
            }
            if (id == _BCM_TD_NUM_EXT_UCAST_QUEUE_GROUP) {
                if (gport_count > 0) {
                    gport[count] = hw_cosq;
                }
                count++;
            }
            if ((gport_count > 0) && (count == gport_count)) {
                break;
            }
        }
    }

    if (count == 0) {
        return BCM_E_NOT_FOUND;
    }
    *actual_gport_count = count;
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_pfc_class_mapping_set
 * Purpose:
 *     Set PFC mapping for port. PFC class is mapped to cosq.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_count      - (IN) count of mapping
 *     mapping_array    - (IN) mappings 
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_pfc_class_mapping_set(int unit,
    bcm_gport_t port,
    int array_count,
    bcm_cosq_pfc_class_mapping_t *mapping_array)
{
    int index, count;
    int cosq, type;
    int cur_class = 0, pfc_class[_BCM_TD_NUM_PFC_CLASS];
    bcm_gport_t cur_gport = 0;
    _bcm_td_cosq_pfc_cos_bmp_t cos_bmp[_BCM_TD_NUM_PFC_CLASS];


    if ((array_count < 0) || (array_count > _BCM_TD_NUM_PFC_CLASS)) {
        return BCM_E_PARAM;
    }
    if (mapping_array == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(cos_bmp, 0, sizeof(cos_bmp));
    for (count = 0; count < array_count; count++) {
        cur_class = mapping_array[count].class_id;
        if ((cur_class < 0) || (cur_class >= _BCM_TD_NUM_PFC_CLASS)) {
            return BCM_E_PARAM;
        }
        
        for (index = 0; index < BCM_COSQ_PFC_GPORT_COUNT; index++) {
            cur_gport = mapping_array[count].gport_list[index];
            if (cur_gport == BCM_GPORT_INVALID) {
                break;
            }

            if (BCM_GPORT_IS_SET(cur_gport)) {
                if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(cur_gport)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_td_cosq_node_get(unit, cur_gport, NULL, NULL,
                                              &cosq, NULL));
                    
                    if (cosq > _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                        /* extended unicast queue */
                        type = _BCM_TD_COSQ_TYPE_EXT_UCAST;
                    } else {
                        type = _BCM_TD_COSQ_TYPE_UCAST;
                    }
                } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(cur_gport)) {
                    type = _BCM_TD_COSQ_TYPE_MCAST;
                } else {
                    /* Scheduler and other types of gport are not supported. */
                    return BCM_E_PARAM;                    
                }
            } else {
                if (cur_gport > _BCM_TD_NUM_UCAST_QUEUE_GROUP) {
                    return BCM_E_PARAM;
                } else {
                    /* gport represents the physical Cos value. */
                    type = _BCM_TD_COSQ_TYPE_UCAST;
                }
            }
            BCM_IF_ERROR_RETURN(
                _bcm_td_fc_status_map_gport(unit, port, type, cur_gport,
                                            &cos_bmp[cur_class]));
        }
    }

    for (index = 0; index < _BCM_TD_NUM_PFC_CLASS; index++) {
        pfc_class[index] = index;
    }
    BCM_IF_ERROR_RETURN(_bcm_td_port_pfc_profile_set(unit, port,
        _BCM_COSQ_OP_SET, _BCM_TD_NUM_PFC_CLASS, pfc_class, cos_bmp));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_pfc_class_mapping_get
 * Purpose:
 *     Get PFC mapping for port.
 *     Retrieves cosq gport associated to PFC classes.
 * Parameters:
 *     unit             - (IN) unit number
 *     port             - (IN) port number
 *     array_max        - (IN) max retrieve count
 *     mapping_array    - (OUT) mappings
 *     array_count      - (OUT) retrieved mapping count
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_pfc_class_mapping_get(int unit,
    bcm_gport_t port,
    int array_max,
    bcm_cosq_pfc_class_mapping_t *mapping_array,
    int *array_count)
{
    int rv = 0;
    int cur_class = 0;
    int actual_gport_count, class_count = 0;
    bcm_switch_control_t sc[] = { bcmSwitchPFCClass0Queue,
                                  bcmSwitchPFCClass1Queue,
                                  bcmSwitchPFCClass2Queue,
                                  bcmSwitchPFCClass3Queue,
                                  bcmSwitchPFCClass4Queue,
                                  bcmSwitchPFCClass5Queue,
                                  bcmSwitchPFCClass6Queue,
                                  bcmSwitchPFCClass7Queue
                                };

    if ((mapping_array == NULL) && (array_max > 0)) {
        return BCM_E_PARAM;
    }
    if (array_count == NULL) {
        return BCM_E_PARAM;
    }

    for (cur_class = 0; cur_class < _BCM_TD_NUM_PFC_CLASS; cur_class++) {
        actual_gport_count = 0;
        if (array_max > 0) {
            rv = bcm_td_cosq_port_pfc_get(unit, port, sc[cur_class],
                        mapping_array[class_count].gport_list,
                        BCM_COSQ_PFC_GPORT_COUNT, &actual_gport_count);
        } else {
            rv = bcm_td_cosq_port_pfc_get(unit, port, sc[cur_class],
                         NULL, -1, &actual_gport_count);
        }
        if (rv == BCM_E_NONE) {
            if (array_max > 0) {
                mapping_array[class_count].class_id = cur_class;
                if (actual_gport_count < BCM_COSQ_PFC_GPORT_COUNT) {
                    mapping_array[class_count].gport_list[actual_gport_count] = 
                                                             BCM_GPORT_INVALID;
                }
            }
            class_count++;
        } else if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }

        if ((class_count == array_max) && (array_max > 0)) {
            break;
        }
    }

    *array_count = class_count;
    if (class_count == 0) {
        return BCM_E_NOT_FOUND;
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_attach
 * Purpose:
 *     Attach sched_port to the specified index (cosq) of input_port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (IN) GPORT to attach to
 *     cosq       - (IN) COS queue to attach to (-1 for first available cosq)
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_attach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    _bcm_td_cosq_node_t *sched_node, *input_node;
    bcm_port_t sched_port, input_port;
    int rv;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td_cosq_gport_attach: unit=%d sched_port=0x%x "
                         "input_port=0x%x cosq=%d\n"),
              unit, sched_gport, input_gport, cosq));

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_node_get(unit, sched_gport, NULL, &sched_port, NULL,
                               &sched_node));

    if (sched_node->cosq_attached_to >= 0) {
        /* Has already attached */
        return BCM_E_EXISTS;
    }

    if (BCM_GPORT_IS_SCHEDULER(input_gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_node_get(unit, input_gport, NULL, &input_port, NULL,
                                   &input_node));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_td_cosq_localport_resolve(unit, input_gport, &input_port));
        input_node = NULL;
    }

    if (sched_port != input_port) {
        return BCM_E_PORT;
    }

    if (input_node != NULL) {  /* sched_gport is not an S1 scheduler */
        if (input_node->cosq_attached_to < 0) {
            /* Only allow to attach to a node that has already attached */
            return BCM_E_PARAM;
        }
        if (cosq < -1 || cosq >= input_node->numq) {
            return BCM_E_PARAM;
        }
        sched_node->parent = input_node;
        sched_node->sibling = input_node->child;
        input_node->child = sched_node;
        rv = _bcm_td_cosq_node_resolve(unit, input_port, sched_node, cosq);
        if (BCM_FAILURE(rv)) {
            input_node->child = sched_node->sibling;
            return rv;
        }
        BCM_IF_ERROR_RETURN(_bcm_td_cosq_sched_node_set(unit, sched_port));

        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "                         hw_cosq=%d\n"),
                  sched_node->hw_cosq_attached_to));
    } else { /* sched_gport is an S1 scheduler */
        if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
            return BCM_E_PORT;
        }

        if (sched_node->numq > _BCM_TD_S1_SCHEDULER_NUM_INPUT) {
            return BCM_E_FAIL;
        }
        sched_node->cosq_attached_to = 0;
        sched_node->hw_cosq_attached_to = 0;
        sched_node->level = 1;
    }

#if 0
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ING_COS_MODEr, input_port,
                                SELECTf, 0x3)); /* VLAN_COS */
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, COS_MODEr, input_port,
                                SELECTf, 0x3)); /* VLAN_COS */
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_detach
 * Purpose:
 *     Detach sched_port from the specified index (cosq) of input_port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (IN) GPORT to detach from
 *     cosq       - (IN) COS queue to detach from
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_td_cosq_gport_detach(int unit, bcm_gport_t sched_gport,
                         bcm_gport_t input_gport, bcm_cos_queue_t cosq)
{
    _bcm_td_cosq_node_t *sched_node, *input_node, *prev_node;
    bcm_port_t sched_port, input_port;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "bcm_td_cosq_gport_detach: unit=%d sched_port=0x%x "
                         "input_port=0x%x cosq=%d\n"),
              unit, sched_gport, input_gport, cosq));

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_node_get(unit, sched_gport, NULL, &sched_port, NULL,
                               &sched_node));

    if (sched_node->cosq_attached_to < 0) {
        /* Not attached yet */
        return BCM_E_PORT;
    }

    /* input_gport argument is redundant, check if it is specified */
    if (input_gport != BCM_GPORT_INVALID) {
        if (BCM_GPORT_IS_SCHEDULER(input_gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_node_get(unit, input_gport, NULL, &input_port,
                                       NULL, &input_node));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_td_cosq_localport_resolve(unit, input_gport,
                                                &input_port));
            input_node = NULL;
        }
        if (sched_port != input_port) {
            return BCM_E_PORT;
        }
        if (sched_node->parent != input_node) {
            return BCM_E_PORT;
        }
    }

    /* cosq argument is redundant, check if it is specified */
    if (cosq != -1) {
        if (sched_node->parent == NULL) {
            /*schedule node is S1*/
            if (sched_node->cosq_attached_to != 0) {
                return BCM_E_PARAM;
            }
        } else if (sched_node->cosq_attached_to != cosq) {
            return BCM_E_PARAM;
        }
    }

    if (sched_node->parent != NULL) { /* sched_node is not an S1 scheduler */
        if (sched_node->parent->child == sched_node) {
            sched_node->parent->child = sched_node->sibling;
        } else {
            prev_node = sched_node->parent->child;
            while (prev_node != NULL && prev_node->sibling != sched_node) {
                prev_node = prev_node->sibling;
            }
            if (prev_node == NULL) {
                return BCM_E_INTERNAL;
            }
            prev_node->sibling = sched_node->sibling;
        }
        sched_node->parent = NULL;
        sched_node->sibling = NULL;
    }

    BCM_IF_ERROR_RETURN(_bcm_td_cosq_node_unresolve(unit, sched_node));
    BCM_IF_ERROR_RETURN(_bcm_td_cosq_sched_node_set(unit, sched_port));

#if 0
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, COS_MODEr, input_port, SELECTf, 0x0));
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, ING_COS_MODEr, input_port, SELECTf, 0x0));
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_attach_get
 * Purpose:
 *     Get attached status of a scheduler port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (OUT) GPORT to attach to
 *     cosq       - (OUT) COS queue to attached to
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_td_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                             bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    _bcm_td_cosq_node_t *sched_node;
    bcm_module_t modid, modid_out;
    bcm_port_t port, port_out;

    if (input_gport == NULL || cosq == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_node_get(unit, sched_gport, &modid, &port, NULL,
                               &sched_node));

    if (sched_node->cosq_attached_to < 0) {
        /* Not attached yet */
        return BCM_E_NOT_FOUND;
    }

    if (sched_node->parent != NULL) { /* sched_node is not an S1 scheduler */
        *input_gport = sched_node->parent->gport;
    } else {  /* sched_node is an S1 scheduler */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, modid, port,
                                    &modid_out, &port_out));
        BCM_GPORT_MODPORT_SET(*input_gport, modid_out, port_out);
    }
    *cosq = sched_node->cosq_attached_to;

    return BCM_E_NONE;
}

STATIC const soc_reg_t hg_voqfc_msg_sel_reg[8] = {
        VOQFC_MSG_PORT_SEL0r, 
        VOQFC_MSG_PORT_SEL1r, 
        VOQFC_MSG_PORT_SEL2r, 
        VOQFC_MSG_PORT_SEL3r, 
        VOQFC_MSG_PORT_SEL4r, 
        VOQFC_MSG_PORT_SEL5r, 
        VOQFC_MSG_PORT_SEL6r, 
        VOQFC_MSG_PORT_SEL7r
    };

STATIC const soc_reg_t hg_voqfc_group_merge_reg[8] = {
        VOQFC_STATE_MERGE_GRP0r, 
        VOQFC_STATE_MERGE_GRP1r, 
        VOQFC_STATE_MERGE_GRP2r, 
        VOQFC_STATE_MERGE_GRP3r, 
        VOQFC_STATE_MERGE_GRP4r, 
        VOQFC_STATE_MERGE_GRP5r, 
        VOQFC_STATE_MERGE_GRP6r, 
        VOQFC_STATE_MERGE_GRP7r
    };

/*
 * Returns index of the port for indexing into MSG_SEL.
 */
STATIC int
_bcm_td_voq_port_index(int unit, bcm_port_t port)
{
    soc_info_t *si;
    int     ii, pipe, mmu_base[2], index = -1;
    bcm_port_t phy_port, mmu_port;

    si = &SOC_INFO(unit);
    
    phy_port = si->port_l2p_mapping[port];

    if (phy_port < 0) {
        return -1;
    }

    mmu_base[0] = 1;
    mmu_base[1] = 34;

    for (pipe = 0; pipe < 2; pipe++) {
        for (ii = 0; ii < 4; ii++) {
            mmu_port = mmu_base[pipe] + ii;
            if (si->port_m2p_mapping[mmu_port] == phy_port) {
                index = (pipe * 4) + ii;
            }
        }
    }

    return index;
}

STATIC int
_bcm_cosq_voq_find_empty_msg_sel_entry(int unit, bcm_port_t port, 
                                       bcm_port_t fab_egr_port,
                                       int *msg_sel_index)
{
    soc_reg_t merge_grp_reg, msg_sel_reg;
    uint32    reg_addr, reg_data;
    int       ii, pidx = -1;
    uint16    used_bmp = 0, profile_bmp;
    uint32    fabric_port[16];

    *msg_sel_index = -1;

    if ((pidx = _bcm_td_voq_port_index(unit, port)) < 0) {
        return BCM_E_UNAVAIL;
    }

    msg_sel_reg = hg_voqfc_msg_sel_reg[pidx];
    merge_grp_reg = hg_voqfc_group_merge_reg[pidx];

    for (ii = 0; ii < 16; ii++) {
        reg_addr = soc_reg_addr(unit, msg_sel_reg, REG_PORT_ANY, ii);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &reg_data));
        fabric_port[ii] = soc_reg_field_get(unit, msg_sel_reg, reg_data, MSG_PORTf);
    }

    for (ii = 0; ii < 16; ii++) {
        reg_addr = soc_reg_addr(unit, merge_grp_reg, REG_PORT_ANY, ii);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &reg_data));
        profile_bmp = soc_reg_field_get(unit, merge_grp_reg, 
                                            reg_data, MERGE_ST_BMPf);
        used_bmp |= profile_bmp;
    }

    for (ii = 0; ii < 16; ii++) {
        /* check already in use */
        if ((used_bmp & (1 << ii)) && (fab_egr_port == fabric_port[ii])) {
            *msg_sel_index = ii;
            return BCM_E_NONE;
        }
    }
    /* Not found, ok, get an first empty slot */
    for (ii = 0; ii < 16; ii++) {
        /* check already in use */
        if ((used_bmp & (1 << ii)) == 0) {
            *msg_sel_index = ii;
            return BCM_E_NONE;
        }
    }

    return BCM_E_FULL;
}

STATIC int
_bcm_cosq_voq_find_current_msg_sel_entry(int unit, bcm_port_t port, 
                                       bcm_port_t fab_egr_port,
                                       int q_group, int *msg_sel_index)
{
    soc_reg_t merge_grp_reg, msg_sel_reg;
    uint32    reg_addr, reg_data;
    int       ii, pidx = -1;
    uint16    used_bmp = 0, profile_bmp;
    uint32    fabric_port[16];
    int       from_q_grp, to_q_grp;

    *msg_sel_index = -1;

    if ((pidx = _bcm_td_voq_port_index(unit, port)) < 0) {
        return BCM_E_UNAVAIL;
    }

    msg_sel_reg = hg_voqfc_msg_sel_reg[pidx];
    merge_grp_reg = hg_voqfc_group_merge_reg[pidx];

    for (ii = 0; ii < 16; ii++) {
        reg_addr = soc_reg_addr(unit, msg_sel_reg, REG_PORT_ANY, ii);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &reg_data));
        fabric_port[ii] = soc_reg_field_get(unit, msg_sel_reg, reg_data, MSG_PORTf);
    }

    if (q_group < 0) {
        from_q_grp = 0;
        to_q_grp = 15;
    } else {
        from_q_grp = to_q_grp = q_group;
    }

    for (ii = from_q_grp; ii <= to_q_grp; ii++) {
        reg_addr = soc_reg_addr(unit, merge_grp_reg, REG_PORT_ANY, ii);
        SOC_IF_ERROR_RETURN(soc_reg32_read(unit, reg_addr, &reg_data));
        profile_bmp = soc_reg_field_get(unit, merge_grp_reg, reg_data, MERGE_ST_BMPf);

        used_bmp |= profile_bmp;
    }

    /* Not found, ok, get an first empty slot */
    for (ii = 0; ii < 16; ii++) {
        if ((used_bmp & (1 << ii)) && (fab_egr_port == fabric_port[ii])) {
            *msg_sel_index = ii;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

STATIC int
_bcm_td_voq_fabric_port_set(int unit, bcm_port_t port, 
                              bcm_port_t fab_egr_port,
                              int msg_sel_index, int q_grp_idx)
{
    soc_reg_t merge_grp_reg, msg_sel;
    uint32    reg_data;
    int       voq_pidx = -1;
    uint16    bmp;

    if ((voq_pidx = _bcm_td_voq_port_index(unit, port)) < 0) {
        return BCM_E_PARAM;
    }

    msg_sel = hg_voqfc_msg_sel_reg[voq_pidx];
    merge_grp_reg = hg_voqfc_group_merge_reg[voq_pidx];

    reg_data = 0;
    soc_reg_field_set(unit, msg_sel, &reg_data, MSG_PORTf, fab_egr_port);
    BCM_IF_ERROR_RETURN
        (soc_reg32_set(unit, msg_sel, REG_PORT_ANY, msg_sel_index, reg_data));

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, merge_grp_reg, REG_PORT_ANY, q_grp_idx,
                       &reg_data));
    bmp = soc_reg_field_get(unit, merge_grp_reg, reg_data, MERGE_ST_BMPf);
    if ((bmp & (1 << q_grp_idx)) == 0) {
        bmp |= (1 << q_grp_idx);
        soc_reg_field_set(unit, merge_grp_reg, &reg_data, MERGE_ST_BMPf, bmp);

        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, merge_grp_reg, REG_PORT_ANY, q_grp_idx,
                           reg_data));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td_voq_fabric_port_unset(int unit, bcm_port_t port, 
                              bcm_port_t fab_egr_port, int q_grp,
                              uint32 *new_fabric_pbmp)
{
    uint32    reg_data;
    int       voq_pidx = -1;
    uint16    profile_bmp, ii, jj;
    uint32    fabric_port[16];
    soc_reg_t merge_grp_reg, msg_sel_reg;

    if ((voq_pidx = _bcm_td_voq_port_index(unit, port)) < 0) {
        return BCM_E_PARAM;
    }

    msg_sel_reg = hg_voqfc_msg_sel_reg[voq_pidx];
    merge_grp_reg = hg_voqfc_group_merge_reg[voq_pidx];

    for (ii = 0; ii < 16; ii++) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, msg_sel_reg, REG_PORT_ANY, ii, &reg_data));
        fabric_port[ii] = soc_reg_field_get(unit, msg_sel_reg, reg_data, MSG_PORTf);
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, merge_grp_reg, REG_PORT_ANY, q_grp, &reg_data));
    profile_bmp = soc_reg_field_get(unit, merge_grp_reg, reg_data,
                                    MERGE_ST_BMPf);

    for (jj = 0; jj < 16; jj++) {
        if ((profile_bmp & (1 << jj)) && (fab_egr_port == fabric_port[jj])) {
            profile_bmp &= ~(1 << jj);

            soc_reg_field_set(unit, merge_grp_reg, &reg_data, 
                              MERGE_ST_BMPf, profile_bmp);
            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, merge_grp_reg, REG_PORT_ANY, q_grp,
                               reg_data));
            *new_fabric_pbmp = profile_bmp;
            break;
        }
    }

    return BCM_E_NONE;
}

STATIC int _bcm_td_cosq_voq_init(int unit, bcm_port_t port)
{
    soc_reg_t merge_grp_reg;
    uint32    reg_data = 0;
    int       pidx = -1, ii;

    if ((pidx = _bcm_td_voq_port_index(unit, port)) < 0) {
        return BCM_E_NONE;
    }

    merge_grp_reg = hg_voqfc_group_merge_reg[pidx];

    /*
     * clear out VOQFC_STATE_MERGE_GRPx registers.
     */
    for (ii = 0; ii < 16; ii++) {
        soc_reg_field_set(unit, merge_grp_reg, &reg_data, MERGE_ST_BMPf, 0);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, merge_grp_reg, REG_PORT_ANY, ii, reg_data));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_td_create_default_profile(int unit, bcm_port_t gport, int numq,
                                _bcm_td_voq_profile_t *profile)
{
    int     ii;

    if ((numq <= 0) || (!profile)) {
        return BCM_E_PARAM;
    }

    profile->hw_index = -1;
    profile->id = -1;

    for (ii = 0; ii < 16; ii++) {
        profile->int_prio2q[ii] = 
                        _BCM_TD_VOQ_GPORT_QUEUE_ID(unit, gport, (ii*numq/16));
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_td_add_voq_profile(int unit, _bcm_td_cosq_node_t *node)
{
    uint32 new_index;
    voq_cos_map_entry_t voq_cos_map_entries[16];
    void *entries[1];
    int ii;
    _bcm_td_voq_profile_t *profile = &node->voq_profile;

    sal_memset(voq_cos_map_entries, 0, sizeof(voq_cos_map_entries));

    for (ii = 0; ii < 16; ii++) {
        soc_mem_field32_set(unit, VOQ_COS_MAPm, 
                   &voq_cos_map_entries[ii], VOQ_COSf, profile->int_prio2q[ii]);
    }

    entries[0] = &voq_cos_map_entries;
    BCM_IF_ERROR_RETURN
        (soc_profile_mem_add(unit, _bcm_td_voq_cos_map_profile[unit], 
                             entries, 16, &new_index));

    profile->hw_index = new_index;
    profile->id = new_index/16;

    return BCM_E_NONE;
}


/* Map internal priority to voq cos */
STATIC int
_bcm_td_voq_gport_mapping_set(int unit, bcm_port_t gport, int int_prio, 
                              bcm_cos_t cos)
{
    _bcm_td_voq_profile_t *profile;
    _bcm_td_cosq_node_t *node;
    int qid, id, prev_profile_id = -1, profile_id, modid, rv;
    bcm_port_t  local_port, port, dest_port;

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_node_get(unit, gport, NULL, &local_port, &id, &node));

    profile = &node->voq_profile;

    qid = _BCM_TD_VOQ_GPORT_QUEUE_ID(unit, gport, cos);
    if (qid == profile->int_prio2q[int_prio]) {
        return BCM_E_NONE;
    }

    profile->int_prio2q[int_prio] = qid;

    if (profile->id < 0) {
        return BCM_E_NONE;
    }

    prev_profile_id = profile->id;

    /*
     * Update modport_sw table for all the ingress ports that use this
     * voq group to use new profile.
     */
    PBMP_PORT_ITER(unit, port) {
        for (modid = 0; modid <= SOC_MODID_MAX(unit); modid++) {
            rv = bcm_esw_stk_port_modport_get(unit, port,
                                              modid, &dest_port);

            if ((!BCM_SUCCESS(rv)) || (dest_port != local_port)) {
                continue;
            }

            rv = bcm_stk_modport_voq_cosq_profile_get(unit, port, 
                                                      modid, &profile_id);
            if ((!BCM_SUCCESS(rv)) || (prev_profile_id != profile_id)) {
                continue;
            }

            /*
             * Add VOQ profile to HW. and delete the previos VOQ profile
             */
            BCM_IF_ERROR_RETURN(_bcm_td_add_voq_profile(unit, node));

            BCM_IF_ERROR_RETURN
                (soc_profile_mem_delete(unit, _bcm_td_voq_cos_map_profile[unit], 
                                        prev_profile_id * 16));

            BCM_IF_ERROR_RETURN
                (bcm_stk_modport_voq_cosq_profile_set(unit, port, 
                                                      modid, profile->id));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_modid_attach
 * Purpose:
 *     Get attached status of a scheduler port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (OUT) GPORT to attach to
 *     cosq       - (OUT) COS queue to attached to
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_td_cosq_gport_destmod_attach(int unit, bcm_gport_t gport,
                               bcm_port_t ing_port,
                               bcm_module_t dest_modid, 
                               int fabric_egress_port)
{
    int q_grp;
    bcm_port_t local_port;
    int msg_sel_index, cur_profile_id = -1;
    _bcm_td_cosq_node_t *node;
    _bcm_td_voq_profile_t *profile;

    BCM_IF_ERROR_RETURN
        (bcm_stk_modport_voq_cosq_profile_get(unit, ing_port, 
                                              dest_modid, &cur_profile_id));
    if (cur_profile_id >= 0) {
        return BCM_E_BUSY;
    }

    if (!_BCM_TD_IS_VOQ_GPORT(unit, gport)) {
        return BCM_E_PARAM;
    }
    
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_node_get(unit, gport, NULL, &local_port, &q_grp, &node));
    if (node == NULL) {
        return BCM_E_PARAM;
    }

    /*
     * find an empty index to read VOQFC[port] state.
     */
    if (_bcm_cosq_voq_find_empty_msg_sel_entry(unit, local_port, 
                                    fabric_egress_port, &msg_sel_index) < 0) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit,
                              "Error: unit %d port %d MSG_SEL enteries full \n"),
                              unit, local_port));
        return BCM_E_FULL;
    }

    q_grp = q_grp - _BCM_TD_NUM_UCAST_QUEUE_GROUP;
    SOC_IF_ERROR_RETURN
        (_bcm_td_voq_fabric_port_set(unit, local_port, 
                                     fabric_egress_port, msg_sel_index, q_grp));

    BCM_IF_ERROR_RETURN(_bcm_td_add_voq_profile(unit, node));

    profile = &node->voq_profile;

    /* update modport table to new profile */
    BCM_IF_ERROR_RETURN
        (bcm_stk_modport_voq_cosq_profile_set(unit, ing_port, 
                                              dest_modid, profile->id));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_td_cosq_gport_modid_detach
 * Purpose:
 *     Get attached status of a scheduler port
 * Parameters:
 *     unit       - (IN) unit number
 *     sched_port - (IN) scheduler GPORT identifier
 *     input_port - (OUT) GPORT to attach to
 *     cosq       - (OUT) COS queue to attached to
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcm_td_cosq_gport_destmod_detach(int unit, bcm_gport_t gport,
                                 bcm_port_t ing_port,
                                 bcm_module_t dest_modid, 
                                 int fabric_egress_port)
{
    int qid, q_grp, id, rv;
    _bcm_td_voq_profile_t *profile;
    bcm_port_t local_port, port, m_dest_port;
    int msg_sel_index, cur_profile_id = -1, use_cnt, profile_id;
    uint32      new_fab_pbmp;
    _bcm_td_cosq_node_t *node;

    if (!_BCM_TD_IS_VOQ_GPORT(unit, gport)) {
        return BCM_E_PARAM;
    }
    
    local_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(gport);
    if (!_BCM_TD_IS_PORT_DMVOQ_CAPABLE(unit, local_port)) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit,
                              "Error: unit %d port %d is not a DestMod flow-control capable \n"),
                   unit, local_port));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (bcm_stk_modport_voq_cosq_profile_get(unit, ing_port, 
                                              dest_modid, &cur_profile_id));

    /*
     * check if port really has a VOQ profile attached to it.
     */
    if (cur_profile_id < 0) {
        return BCM_E_NONE;
    }

    /*
     * Get the cosmap profile and add to HW.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_node_get(unit, gport, NULL, &local_port, &id, &node));

    profile = &node->voq_profile;

    /*
     * validate that the profile we are trying to detach is really 
     * the one attached to this port.
     */
    if (cur_profile_id != profile->id) {
        return BCM_E_PARAM;
    }

    qid = _BCM_TD_VOQ_GPORT_QUEUE_ID(unit, gport, 0);
    q_grp = (qid - _BCM_TD_VOQ_QID_MIN_ID) % 16;

    /*
     * Disconnect VOQ profile for this ingress port.
     */
    BCM_IF_ERROR_RETURN
        (bcm_stk_modport_voq_cosq_profile_set(unit, ing_port, dest_modid, -1));


    BCM_IF_ERROR_RETURN
        (soc_profile_mem_delete(unit, _bcm_td_voq_cos_map_profile[unit], 
                                profile->hw_index));

    /* 
     * get the number of ports still using this profile. if none clear
     * out msg_sel register.
     */
    use_cnt = 0;
    PBMP_PORT_ITER(unit, port) {
        rv = bcm_esw_stk_port_modport_get(unit, port,
                                          dest_modid, &m_dest_port);

        if ((BCM_FAILURE(rv)) || (m_dest_port != local_port)) {
            continue;
        }

        rv = bcm_stk_modport_voq_cosq_profile_get(unit, port, 
                                                  dest_modid, &profile_id);

        if ((BCM_FAILURE(rv)) || (profile->id != profile_id)) {
            continue;
        }
        use_cnt++;
    }

    if (use_cnt) {
        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (_bcm_cosq_voq_find_current_msg_sel_entry(unit, local_port, 
                                    fabric_egress_port, q_grp, &msg_sel_index));

    if (msg_sel_index < 0) {
        return BCM_E_NOT_FOUND;
    }

    SOC_IF_ERROR_RETURN
        (_bcm_td_voq_fabric_port_unset(unit, local_port, 
                                       fabric_egress_port, q_grp, &new_fab_pbmp));
    profile->id = -1;
    profile->hw_index = -1;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_cosq_cpu_cosq_enable_set
 * Purpose:
 *      To enable/disable Rx of packets on the specified CPU cosq.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      cosq          - (IN) CPU Cosq ID
 *      enable        - (IN) Enable/Disable
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_cosq_cpu_cosq_enable_set(
    int unit, 
    bcm_cos_queue_t cosq, 
    int enable)
{
    uint32 rval, rval1;
    int enable_status;
    soc_info_t *si;
    _bcm_td_cosq_cpu_cosq_config_t *cpu_cosq;

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= si->num_cpu_cosq)) {
        return BCM_E_PARAM;
    }

    cpu_cosq = _bcm_td_cosq_cpu_cosq_config[unit][cosq];

    if (!cpu_cosq) {
        return BCM_E_INTERNAL;
    }
    if (enable) {
        enable = 1;
    }
    BCM_IF_ERROR_RETURN(bcm_td_cosq_cpu_cosq_enable_get(unit, cosq,
                                                        &enable_status));
    if (enable == enable_status) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (READ_OP_QUEUE_CONFIG_CELLr(unit, 0, cosq, &rval));
    BCM_IF_ERROR_RETURN
        (READ_OP_QUEUE_CONFIG1_CELLr(unit, 0, cosq, &rval1));

    if (enable) {
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                          Q_SHARED_LIMIT_CELLf, cpu_cosq->q_shared_limit);
        soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                          Q_MIN_CELLf, cpu_cosq->q_min_limit);
    } else {
        cpu_cosq->q_shared_limit = soc_reg_field_get(unit,
                                                     OP_QUEUE_CONFIG_CELLr,
                                                     rval,
                                                     Q_SHARED_LIMIT_CELLf);
        cpu_cosq->q_min_limit = soc_reg_field_get(unit,
                                                  OP_QUEUE_CONFIG_CELLr,
                                                  rval,
                                                  Q_MIN_CELLf);
        cpu_cosq->q_limit_dynamic = soc_reg_field_get(unit,
                                                      OP_QUEUE_CONFIG1_CELLr,
                                                      rval1,
                                                      Q_LIMIT_DYNAMIC_CELLf);
        cpu_cosq->q_limit_enable = soc_reg_field_get(unit,
                                                     OP_QUEUE_CONFIG1_CELLr,
                                                     rval1,
                                                     Q_LIMIT_ENABLE_CELLf);
    }

    cpu_cosq->enable = enable;
    soc_reg_field_set(unit, OP_QUEUE_CONFIG1_CELLr, &rval1,
                      Q_LIMIT_DYNAMIC_CELLf, 
                      enable ? cpu_cosq->q_limit_dynamic : 0);
    soc_reg_field_set(unit, OP_QUEUE_CONFIG1_CELLr, &rval1,
                      Q_LIMIT_ENABLE_CELLf,
                      enable ? cpu_cosq->q_limit_enable : 1);
    BCM_IF_ERROR_RETURN
        (WRITE_OP_QUEUE_CONFIG1_CELLr(unit, 0, cosq, rval1));
    BCM_IF_ERROR_RETURN
        (WRITE_OP_QUEUE_CONFIG_CELLr(unit, 0, cosq, enable ? rval : 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_cosq_cpu_cosq_enable_get
 * Purpose:
 *      To get enable/disable status on Rx of packets on the specified CPU cosq.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      cosq          - (IN) CPU Cosq ID
 *      enable        - (OUT) Enable/Disable
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td_cosq_cpu_cosq_enable_get(
    int unit, 
    bcm_cos_queue_t cosq, 
    int *enable)
{
    _bcm_td_cosq_cpu_cosq_config_t *cpu_cosq;
    soc_info_t *si;
    uint32 rval, rval1;
    uint32 min_limit, shared_limit, limit_enable, dynamic_enable;
    int hw_enable;

    si = &SOC_INFO(unit);

    if ((cosq < 0) || (cosq >= si->num_cpu_cosq)) {
        return BCM_E_PARAM;
    }

    cpu_cosq = _bcm_td_cosq_cpu_cosq_config[unit][cosq];
    if (!cpu_cosq) {
        return BCM_E_INTERNAL;
    }

    hw_enable = 1;
    /* Sync up software record with hardware status. */
    BCM_IF_ERROR_RETURN
        (READ_OP_QUEUE_CONFIG_CELLr(unit, 0, cosq, &rval));
    BCM_IF_ERROR_RETURN
        (READ_OP_QUEUE_CONFIG1_CELLr(unit, 0, cosq, &rval1));
    min_limit = soc_reg_field_get(unit, OP_QUEUE_CONFIG_CELLr,
                                  rval, Q_MIN_CELLf);
    shared_limit = soc_reg_field_get(unit, OP_QUEUE_CONFIG_CELLr,
                                     rval, Q_SHARED_LIMIT_CELLf);
    limit_enable = soc_reg_field_get(unit, OP_QUEUE_CONFIG1_CELLr,
                                     rval1, Q_LIMIT_ENABLE_CELLf);
    dynamic_enable = soc_reg_field_get(unit, OP_QUEUE_CONFIG1_CELLr,
                                       rval1, Q_LIMIT_DYNAMIC_CELLf);
    if (limit_enable && (dynamic_enable == 0) &&
        (min_limit == 0) && (shared_limit == 0)) {
        hw_enable = 0;
    }
    cpu_cosq->enable = hw_enable;

    *enable = cpu_cosq->enable;
    return BCM_E_NONE;
}


#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_td_cosq_sw_dump
 * Purpose:
 *     Displays COS Queue information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_td_cosq_sw_dump(int unit)
{
    bcm_port_t port;

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information COSQ - Unit %d\n"), unit));

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }
        (void)_bcm_td_cosq_port_info_dump(unit, port);
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *     _bcm_td_port_enqueue_set
 * Purpose:
 *     Enable or Disable the enqueing the packets to the port
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) gport of the port 
 *     enable     - (IN) TRUE to enable
 *                      FALSE to disable
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_td_port_enqueue_set(int unit, bcm_port_t gport,
                         int enable)
{
    uint64 rval64 ,rval64_tmp ;
    int mmu_port, phy_port;
    int i;
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    soc_info_t *si;
    soc_reg_t reg_tmp;
    soc_reg_t reg[2][2] = {
        {
            OUTPUT_PORT_RX_ENABLE0_64r,
            OUTPUT_PORT_RX_ENABLE1_64r,
        },
        {
            INPUT_PORT_RX_ENABLE0_64r,
            INPUT_PORT_RX_ENABLE1_64r,
        }
    };

    COMPILER_64_ZERO(rval64);
    COMPILER_64_ZERO(rval64_tmp);


    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    for (i = 0; i < 2; i++) {

        if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
            reg_tmp = reg[i][0];
        } else { /* Y pipe */
            reg_tmp = reg[i][1];
        }
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg_tmp, REG_PORT_ANY, 0, &rval64));

        mmu_port &= 0x3f;
 
        /* OUTPUT_PORT_RX and INPUT_PORT_RX is organised as follows:
           port 0 to port 32 in RX_ENABLE0 and port 33 to 63 in RX_ENABLE1
           the following logic is to set 0-32 bits for port 33 to 63 in RX_ENABLE1 */

        if (mmu_port > 32) {
            mmu_port = mmu_port - 33;
        }

        if (mmu_port < 32) {
            COMPILER_64_SET(rval64_tmp, 0, 1 << mmu_port);
        } else {
            COMPILER_64_SET(rval64_tmp, 1 << (mmu_port - 32), 0);
        }

        if (!enable) {
            COMPILER_64_NOT(rval64_tmp);
            COMPILER_64_AND(rval64, rval64_tmp);
        } else {
            COMPILER_64_OR(rval64, rval64_tmp);
        }
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg_tmp, REG_PORT_ANY, 0, rval64));
    }
    return rv;
}

/*
 * Function:
 *     _bcm_td_port_enqueue_get
 * Purpose:
 *     Getting the enqueing state of the port
 * Parameters:
 *     unit       - (IN) unit number
 *     gport      - (IN) gport of the port 
 *     enable     - (OUT) TRUE if enable
 *                      FALSE if disable
 * Returns:
 *     BCM_E_XXX
 */


int
_bcm_td_port_enqueue_get(int unit, bcm_port_t gport,
                         int *enable)
{
    uint64 rval64 ; 
    int mmu_port, phy_port;
    int rv = BCM_E_NONE;
    bcm_port_t local_port;
    soc_info_t *si;
    soc_reg_t reg;

    COMPILER_64_ZERO(rval64);

    BCM_IF_ERROR_RETURN
        (_bcm_td_cosq_localport_resolve(unit, gport, &local_port));

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];


    if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
        reg = OUTPUT_PORT_RX_ENABLE0_64r;
    } else { /* Y pipe */
        reg = OUTPUT_PORT_RX_ENABLE1_64r;
    }
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));

    mmu_port &= 0x3f;

    if (mmu_port > 32) {
        mmu_port = mmu_port - 33;
    }
    if (COMPILER_64_BITTEST(rval64 , mmu_port)) {
        *enable = TRUE;
    } else {
        *enable = FALSE;
    }
    return rv;
}

/* 
 * Function:    _bcm_td_wred_war_ports_select
 *
 * Purpose:     Selects one extended queue port and one 
 *              regular port from each (X/Y)pipeline. 
 */
STATIC int
_bcm_td_wred_war_ports_select(int unit)
{
   
   soc_info_t      *si;
   soc_port_t local_port;
   soc_port_t rcpu_port = 0xff; /* Invalid port number */
   int local_port_mode;
   _wred_war_ctrl_t *w;
   int valid_port = 0;
   mac_driver_t *mac = NULL;

   mac = &soc_mac_x;
   w = &war;

   w->port[0] =  w->port[1] =  w->port[2] =  w->port[3] = -1;
   
   si = &SOC_INFO(unit);

   if (SOC_IS_RCPU_UNIT(unit)) {
       rcpu_port = RCPU_PORT(unit);
   }
  
   PBMP_ALL_ITER(unit, local_port) {
      valid_port = PORT_IS_VALID(unit, local_port);
      if (!valid_port || (rcpu_port == local_port)) {
          continue;
      }
	  
      SOC_IF_ERROR_RETURN
            (MAC_ENCAP_GET(mac, unit, local_port, &local_port_mode));
      
      if (SOC_PBMP_MEMBER(si->xpipe_pbm, local_port)) { /* X pipe */
          if ((si->port_num_ext_cosq[local_port] == 0) && (w->port[0] == -1)) { 
              w->port[0] = local_port;     /* regular port */
              w->port_mode[0] = local_port_mode;
          } else if ((si->port_num_ext_cosq[local_port] != 0) &&
                     (w->port[1] == -1)) { /* extended queue port */
	      w->port[1] = local_port;
	      w->port_mode[1] = local_port_mode;
	  } else {
	      continue; 
	  }
      } else { /* Y pipe */
          if ((si->port_num_ext_cosq[local_port] == 0) && (w->port[2] == -1)) {
              w->port[2] = local_port;     /* regular port */
  	      w->port_mode[2] = local_port_mode;
          } else if ((si->port_num_ext_cosq[local_port] != 0) &&
                     (w->port[3] == -1)) { /* extended queue port */
	      w->port[3] = local_port;
	      w->port_mode[3] = local_port_mode;
	  } else {
	      continue;
	  }
      }	  
      if ((w->port[0] != -1) && (w->port[1] != -1) && 
          (w->port[2] != -1) && (w->port[3] != -1)) {
          break;
      }
   }
  
   return SOC_E_NONE;
}

/*
 * Function:     _bcm_td_wred_war_configure
 *
 * Purpose:      configures wred workaround on ports and switch.
 *
 * Description : This function function does below things:
 *               -Stops live traffic by configuring EPC_LINK_BMAP.
 *               -Converts higig ports into xe ports and removes higig
 *                configuration that was on the ports.
 *               -Configures MAC loopback mode on four ports. 
 *               -Configures MAC CRC mode to 1 to generate CRC packetsi. 
 *               -Enables CRC checks on the switch and configures ING_PRI_CNG
 *                map table to mark ingress packets with red color. 
 */
STATIC int
_bcm_td_wred_war_configure(int unit)
{
   int     i;
   uint32  rval;
   uint64  tx_ctrl;
   pbmp_t  pbm;
   uint32  stg_entry[SOC_MAX_MEM_WORDS];
   mac_driver_t            *mac = NULL;
   _wred_war_ctrl_t        *w;
   port_tab_entry_t        p_entry;
   epc_link_bmap_entry_t   epc_entry;
   uint32 pri_map[SOC_MAX_MEM_FIELD_WORDS];

   sal_memset(pri_map, 0, sizeof(pri_map));


   mac = &soc_mac_x;
   w = &war;

   SOC_PBMP_CLEAR(pbm);
   /* Reads default link status.*/
   SOC_IF_ERROR_RETURN(READ_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL,
                                           0, &w->epc_entry));

   /* Reads default STG mode on switch.*/
   SOC_IF_ERROR_RETURN(soc_mem_read(unit, STG_TABm, MEM_BLOCK_ANY, 1, w->stg_entry));

   sal_memset(&epc_entry, 0, sizeof(epc_entry));
   sal_memset(&stg_entry, 0, sizeof(stg_entry));

   /* Removes all ports from EPC_LINK_BMAP table. */
   if (SOC_IS_RCPU_UNIT(unit)) {
       SOC_PBMP_PORT_ADD(pbm, RCPU_PORT(unit));
   }
   soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, &epc_entry, PORT_BITMAPf, &pbm);
   SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL,
                                            0, &epc_entry));

   for (i = 0; i < SOC_WRED_WR_MAX_NUM_PORTS; i++) {
      if (w->port[i] == -1) {
          continue;
      }

      /*Get current settings of each port*/
      BCM_IF_ERROR_RETURN(bcm_esw_port_enable_get(unit, w->port[i], &w->enable[i]));
      BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, w->port[i], &w->speed[i]));
      BCM_IF_ERROR_RETURN(bcm_esw_port_duplex_get(unit, w->port[i], &w->duplex[i]));
      BCM_IF_ERROR_RETURN(bcm_esw_port_advert_get(unit, w->port[i], &w->advert[i]));
      BCM_IF_ERROR_RETURN(bcm_esw_port_autoneg_get(unit, w->port[i], &w->autoneg[i]));

      /* Configures IEEE mode on all non IEEE ports.*/
      if ((w->port_mode[i] != SOC_ENCAP_IEEE) && SOC_IS_TRIDENT(unit)) {
	  SOC_IF_ERROR_RETURN(MAC_ENCAP_SET(mac, unit, w->port[i],
                                                        SOC_ENCAP_IEEE));
          SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, EGR_PORTm,
                                                 w->port[i], HIGIG2f, 0));
          SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLPORT_CONFIGr,
                                            w->port[i], HIGIG2_MODEf, 0));
          SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, EGR_ING_PORTm,
                                                 w->port[i], HIGIG2f, 0));
      }
      
      BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit, w->port[i], w->enable[i]));
      /* Configures MAC loop back on port.*/ 
      SOC_IF_ERROR_RETURN(bcm_esw_port_loopback_set(unit, w->port[i], BCM_PORT_LOOPBACK_MAC));

      /* Configures XMAC_TX_CTRL register to send FCS packets. */ 
      SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, w->port[i], &tx_ctrl));
      w->port_crc_mode[i] = soc_reg64_field32_get(unit, XMAC_TX_CTRLr, 
                                                  tx_ctrl, CRC_MODEf);
      soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &tx_ctrl, CRC_MODEf, 1);
      SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, w->port[i], tx_ctrl));
  
      /* Set PORT_BRIDGE bit on port.*/ 
      SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY, 
                                       w->port[i], &w->port_entry[i]));
      sal_memcpy(&p_entry, &w->port_entry[i], sizeof(p_entry));
      soc_mem_field32_set(unit, PORT_TABm, &p_entry, PORT_BRIDGEf, 1);
      SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL,
                                        w->port[i], &p_entry));

      SOC_PBMP_PORT_ADD(pbm, w->port[i]);

      stg_entry[STG_WORD(w->port[i])] |= (0x3 << STG_BITS_SHIFT(w->port[i]));
   }

   /* Saves default configuration of MISCCONFIG register in global structure
      and configures parity check and parity gen  fields. */
   SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
   w->parity_gen_en = soc_reg_field_get(unit, MISCCONFIGr, rval, PARITY_GEN_ENf);
   w->parity_chk_en = soc_reg_field_get(unit, MISCCONFIGr, rval, PARITY_CHK_ENf);

   soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_CHK_ENf, 1);
   soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_GEN_ENf, 1);
   SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

   /* Write spanning tree group port state to hw. */
   SOC_IF_ERROR_RETURN(soc_mem_write(unit, STG_TABm, MEM_BLOCK_ALL,
                                     1, stg_entry));

   /* Write port status on switch. */
   soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, &epc_entry, PORT_BITMAPf, &pbm);
   SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 
                                            0, &epc_entry));
 
   /* Mark priority 0 packets with red color. */ 
   if (SOC_IS_TRIDENT3X(unit)) {
       SOC_IF_ERROR_RETURN(READ_PHB_MAPPING_TBL_1m(unit, MEM_BLOCK_ANY,
                                                 0, &pri_map));
       soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, &pri_map, CNGf, 1);
       SOC_IF_ERROR_RETURN(WRITE_PHB_MAPPING_TBL_1m(unit, MEM_BLOCK_ALL,
                                                  0, &pri_map));
   } else {
       SOC_IF_ERROR_RETURN(READ_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY,
                                                 0, &pri_map));
       soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, CNGf, 1);
       SOC_IF_ERROR_RETURN(WRITE_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ALL,
                                                  0, &pri_map));
   }
 
   return SOC_E_NONE;
}

/*
 * Function: _bcm_td_wred_war_hw_state_restore
 *
 * Purpose:  Restores default state of ports and other reg/mem's.
 */

STATIC int
_bcm_td_wred_war_hw_state_restore(int unit)
{
   int     i;
   int     mode;
   uint32  rval;
   uint64  tx_ctrl;
   soc_pbmp_t       pbmp;
   mac_driver_t     *mac = NULL;
   _wred_war_ctrl_t *w;
   uint32 pri_map[SOC_MAX_MEM_FIELD_WORDS];

   int    adv, max_speed = 0;
   mac = &soc_mac_x;
   w   = &war;

   SOC_PBMP_CLEAR(pbmp);

   sal_memset(pri_map, 0, sizeof(pri_map));

   for (i = 0; i < SOC_WRED_WR_MAX_NUM_PORTS; i++) {
      if (w->port[i] == -1) {
          continue;
      }

      if ((w->port_mode[i] != SOC_ENCAP_IEEE) && SOC_IS_TRIDENT(unit)) {
          SOC_IF_ERROR_RETURN
            (MAC_ENCAP_SET(mac, unit, w->port[i], w->port_mode[i]));

          mode = soc_property_port_get(unit, w->port[i], 
                                       spn_HIGIG2_HDR_MODE, 0);
          SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, EGR_PORTm,
                                              w->port[i], HIGIG2f, mode));
          SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLPORT_CONFIGr,
                                         w->port[i], HIGIG2_MODEf, mode));
          SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, EGR_ING_PORTm,
                                              w->port[i], HIGIG2f, mode));
      }
      SOC_IF_ERROR_RETURN
            (bcm_esw_port_loopback_set(unit, w->port[i], BCM_PORT_LOOPBACK_NONE));

      SOC_IF_ERROR_RETURN(bcm_esw_port_speed_max(unit, w->port[i], &max_speed));
      BCM_IF_ERROR_RETURN(bcm_esw_port_autoneg_set(unit, w->port[i], w->autoneg[i]));
      /* if speed is 0, set the port speed to max. if port autoneg is enable,
      * port max speed may be 0. if port autoneg is enable and port max speed is 0,
      * bcm_esw_port_speed_set will be return SOC_E_CONFIG(-15).
      */
      if (w->autoneg[i] == 0) {
          BCM_IF_ERROR_RETURN(bcm_esw_port_duplex_set(unit, w->port[i], w->duplex[i]));
          if (w->speed[i] != 0 || max_speed != 0) {
              BCM_IF_ERROR_RETURN(bcm_esw_port_speed_set(unit, w->port[i], w->speed[i]));
          }
      }
      adv = soc_property_port_get(unit, w->port[i], spn_PORT_INIT_ADV, -1);
      if (adv != -1) {
          BCM_IF_ERROR_RETURN(bcm_esw_port_advert_set(unit, w->port[i], w->advert[i]));      
      }
      BCM_IF_ERROR_RETURN(bcm_esw_port_enable_set(unit, w->port[i], w->enable[i]));

      SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, w->port[i], &tx_ctrl));
      soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &tx_ctrl, 
                            CRC_MODEf, w->port_crc_mode[i]);
      SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, w->port[i], tx_ctrl));

      /* Restore settings on port.*/
      SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL, 
                                        w->port[i], &w->port_entry[i]));

      BCM_IF_ERROR_RETURN(bcm_esw_stat_clear(unit, w->port[i]));
      SOC_PBMP_PORT_ADD(pbmp, w->port[i]);
   }

   SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
   soc_reg_field_set(unit, MISCCONFIGr, &rval, 
                     PARITY_CHK_ENf, w->parity_gen_en);
   soc_reg_field_set(unit, MISCCONFIGr, &rval, 
                     PARITY_GEN_ENf, w->parity_chk_en);
   SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

   /* Write default spanning tree group port states to hw. */
   SOC_IF_ERROR_RETURN(soc_mem_write(unit, STG_TABm, MEM_BLOCK_ALL, 
                                     1, w->stg_entry));

   /* Restore default value in ING_PR_CNG_MAP_TABLE. */
   if (SOC_IS_TRIDENT3X(unit)) {
       SOC_IF_ERROR_RETURN(READ_PHB_MAPPING_TBL_1m(unit, MEM_BLOCK_ANY,
                                                 0, &pri_map));
       soc_mem_field32_set(unit, PHB_MAPPING_TBL_1m, &pri_map, CNGf, 0);
       SOC_IF_ERROR_RETURN(WRITE_PHB_MAPPING_TBL_1m(unit, MEM_BLOCK_ALL,
                                                        0, &pri_map));
   } else {
       SOC_IF_ERROR_RETURN(READ_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ANY,
                                                 0, &pri_map));
       soc_mem_field32_set(unit, ING_PRI_CNG_MAPm, &pri_map, CNGf, 0);
       SOC_IF_ERROR_RETURN(WRITE_ING_PRI_CNG_MAPm(unit, MEM_BLOCK_ALL,
                                                        0, &pri_map));
   }

   /* Clear counters of war ports. */
   SOC_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbmp, 0));

   /* Write default port status to hw. */
   SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL,
                                            0, &w->epc_entry));
   
   return SOC_E_NONE;
}

/*
 * Function:     _bcm_td_wred_mem_war
 *
 * Purpose:      Warkaound to recover WRED memories from 
 *               corrupted state to good state.
 *
 * Description : This logic sends 8 unicast packets that are larger
 *               than 1 cell and contain a bad crc from below ports
 *               to put WRED memories in good state.
 *
 *               - Packet 1 ingresses on any port in the x-pipe and is supposed
 *                 to be switched to a non-extended queuing port in the x-pipe 
 *               - Packet 2 ingresses on any port in the x-pipe and is supposed 
 *                 to be switched to a non-extended queuing port in the y-pipe
 *               - Packet 3 ingresses on any port in the x-pipe and is supposed 
 *                 to be switched to an extended queuing port in the x-pipe 
 *               - Packet 4 ingresses on any port in the x-pipe and is supposed 
 *                 to be switched to an extended queuing port in the y-pipe
 *
 *               - Packet 5 ingresses on any port in the y-pipe and is supposed 
 *                 to be switched to a non-extended queuing port in the x-pipe
 *               - Packet 6 ingresses on any port in the y-pipe and is supposed 
 *                 to be switched to a non-extended queuing port in the y-pip
 *               - Packet 7 ingresses on any port in the y-pipe and is supposed 
 *                 to be switched to an extended queuing port in the x-pipe
 *               - Packet 8 ingresses on any port in the y-pipe and is supposed 
 *                 to be switched to an extended queuing port in the y-pipe
 */
int 
_bcm_td_wred_mem_war(int unit)
{
   int         rv = SOC_E_NONE;
   int         i, j;
   int         l2_flags;
   int         pkt_flags;
   bcm_pkt_t   *tx_pkt;
   bcm_mac_t   mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
   bcm_mac_t   dstmac, srcmac; 
   l2x_entry_t l2x_entry;
   _wred_war_ctrl_t *w;

   w = &war;

   sal_memset(&l2x_entry, 0, sizeof (l2x_entry)); 
   sal_memset(dstmac, 0, sizeof (dstmac));
   sal_memset(srcmac, 0, sizeof (srcmac));

   /* Selects one regular port and one extented queue port from each pipeline
    * and configures wred workaround settings on selected ports and on switch.
    */
   BCM_IF_ERROR_RETURN(_bcm_td_wred_war_ports_select(unit));
   BCM_IF_ERROR_RETURN(_bcm_td_wred_war_configure(unit));


   if (SOC_IS_TRIDENT(unit)) {
       /* Clears stray entries. */
       l2_flags = BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_MATCH_STATIC |
                  BCM_L2_REPLACE_VLAN_AND_VPN_TYPE;
       rv = bcm_esw_l2_replace(unit, l2_flags, NULL, -1, -1, -1);
       if (rv != BCM_E_NONE) {
           BCM_IF_ERROR_RETURN(_bcm_td_wred_war_hw_state_restore(unit));
           return rv;
       }

       /* Adds L2 entries in L2 table. */
       soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf, 0);
       soc_L2Xm_field32_set(unit, &l2x_entry, STATIC_BITf, 1);
       soc_L2Xm_field32_set(unit, &l2x_entry, VALIDf, 1);
       soc_L2Xm_field32_set(unit, &l2x_entry, VLAN_IDf, 1);
       for (i = 0; i < SOC_WRED_WR_MAX_NUM_PORTS; i++) {
          if (w->port[i] == -1) {
              continue;
          }
          MAC_LSB_SHIFT(dstmac, mac, i);
          soc_L2Xm_mac_addr_set(unit,&l2x_entry, MAC_ADDRf, dstmac);
          soc_L2Xm_field32_set(unit, &l2x_entry, PORT_NUMf, w->port[i]);
          soc_mem_insert(unit, L2Xm, MEM_BLOCK_ALL, (void *)&l2x_entry);
       }
  } 

   /* Allocates memory for packet. */
   pkt_flags = BCM_TX_CRC_APPEND | BCM_TX_NO_PAD;
   rv = bcm_pkt_alloc(unit, WRED_MIN_PKT_SIZE, pkt_flags, &tx_pkt);
   if (rv != BCM_E_NONE) {
       BCM_IF_ERROR_RETURN(_bcm_td_wred_war_hw_state_restore(unit));
       return rv;
   }

   /* Configure packet parameters.*/
   tx_pkt->unit = unit;
   tx_pkt->call_back = 0;
   tx_pkt->blk_count = 1;
   tx_pkt->_vtag[0] = 0x81;
   tx_pkt->_vtag[1] = 0x00;
   tx_pkt->_vtag[2] = 0x00;
   tx_pkt->_vtag[3] = 0x01;
   tx_pkt->flags |= BCM_PKT_F_NO_VTAG;
   tx_pkt->opcode = BCM_HG_OPCODE_UC;
	
   /* This logic prepares 8 SOBMH packets with different src 
    * and dst mac addresses and transmits packets from regular ports.
    */			
   for (i = 0; i < SOC_WRED_WR_MAX_NUM_PORTS; i++) {
     if ((w->port[i] == -1) || (i == 1) || (i == 3)) {
          /* Skip if the port is extended port.
           * Send packets only from regular port.
           */
          continue;
      }
      MAC_LSB_SHIFT(srcmac, mac, i);
      BCM_PKT_HDR_SMAC_SET(tx_pkt, srcmac);
      BCM_PKT_PORT_SET(tx_pkt, w->port[i], FALSE, FALSE);
      		
      for (j = 0; j < SOC_WRED_WR_MAX_NUM_PORTS; j++) {
         if ((w->port[j] == -1)) {
             continue;
         }

         MAC_LSB_SHIFT(dstmac, mac, j);
         BCM_PKT_HDR_DMAC_SET(tx_pkt, dstmac);

         if (SOC_IS_TITAN(unit)) {
             tx_pkt->flags |= BCM_PKT_F_HGHDR;
             tx_pkt->flags |= BCM_TX_SRC_MOD;
             tx_pkt->src_mod = 2; /* Some randum value. */
             tx_pkt->src_port = 0;
             tx_pkt->dest_port = w->port[j];
             tx_pkt->dest_mod = (j > 1) ? 1 : 0; /* To handle DUAL_MODID's. */
         }

         rv = bcm_esw_tx(unit, tx_pkt, NULL);
         if (rv <= -1) {
	     goto cleanup;
	 }
      }
   }
   
cleanup:
    /* Frees packet pointer. */
    rv = bcm_pkt_free(unit, tx_pkt);

    if (SOC_IS_TRIDENT(unit)) {
        /* Delete L2 entries. */
        for (i = 0; i < SOC_WRED_WR_MAX_NUM_PORTS; i++) {
           if (w->port[i] == -1) {
               continue;
           }
           MAC_LSB_SHIFT(dstmac, mac, i);
           soc_L2Xm_mac_addr_set(unit,&l2x_entry, MAC_ADDRf, dstmac);
           soc_L2Xm_field32_set(unit, &l2x_entry, PORT_NUMf, w->port[i]);
           SOC_IF_ERROR_RETURN(soc_mem_delete(unit, L2Xm,
                               MEM_BLOCK_ANY, (void *)&l2x_entry));
        }
    } 

    BCM_IF_ERROR_RETURN(_bcm_td_wred_war_hw_state_restore(unit));
             
    return rv;
}
#else /* BCM_TRIDENT_SUPPORT */
typedef int _td_cosq_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRIDENT_SUPPORT */
