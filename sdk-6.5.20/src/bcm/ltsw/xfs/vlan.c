/*! \file vlan.c
 *
 * VLAN Driver for XFS chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm/l3.h>
#include <bcm_int/control.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/ltsw/xfs/vlan.h>
#include <bcm_int/ltsw/xfs/types.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/vlan_int.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/init.h>
#include <bcm_int/ltsw/obm.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/l2.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/generated/vlan_ha.h>

#include <bsl/bsl.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_types.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_VLAN

#define VLAN_CTRL(prio, cfi, id)   \
    BCM_VLAN_CTRL((uint16_t)(prio), (uint16_t)(cfi), (uint16_t)(id))
#define VLAN_CTRL_PRIO(c)   BCM_VLAN_CTRL_PRIO((uint16_t)(c))
#define VLAN_CTRL_CFI(c)    BCM_VLAN_CTRL_CFI((uint16_t)(c))
#define VLAN_CTRL_ID(c)     BCM_VLAN_CTRL_ID((uint16_t)(c))

/*
 * \name Macros for mask action
 * \anchor LTSW_VLAN_MASK_TARGET_xxx
 */
#define LTSW_VLAN_MASK_TARGET_L2_ONLY   0
#define LTSW_VLAN_MASK_TARGET_L3_ONLY   1
#define LTSW_VLAN_MASK_TARGET_L2_AND_L3 2

/*
 * \name Macros for mask action
 * \anchor LTSW_VLAN_MASK_ACTION_xxx
 */
#define LTSW_VLAN_MASK_ACTION_AND 0
#define LTSW_VLAN_MASK_ACTION_OR  1

/*
 * \brief Block mask structure.
 */
typedef struct ltsw_vlan_block_mask_s {
    /* Block mask port bitmap. */
    bcm_pbmp_t mask_ports;

    /* Block mask target. Refer to LTSW_VLAN_MASK_TARGET_xxx. */
    int mask_target;

    /* Block mask action. Refer to LTSW_VLAN_MASK_ACTION_xxx. */
    int mask_action;
} ltsw_vlan_block_mask_t;

/*
 * \brief Ingress membership check profile structure.
 *
 * This data structure is used to identify the profile structure for ingress
 * membership check (Related LTs - VLAN_ING_MEMBER_PROFILE and
 * VLAN_ING_EGR_MEMBER_PORTS_PROFILE).
 */
typedef struct ltsw_vlan_ing_mshp_chk_profile_s {
    /* Ingress member ports applied on membership check. */
    bcm_pbmp_t member_ports;

    /* Egress member ports applied on membership check. */
    bcm_pbmp_t egr_member_ports;

    /*
     * Block mask target to egress member ports.
     * Refer to LTSW_VLAN_MASK_TARGET_xxx.
     */
    int egr_mask_target;

    /*
     * Block mask action to egress member ports.
     * Refer to LTSW_VLAN_MASK_ACTION_xxx.
     */
    int egr_mask_action;
} ltsw_vlan_ing_mshp_chk_profile_t;

/*
 * \brief Egress membership check profile structure.
 *
 * This data structure is used to identify the profile structure for egress
 * membership check (Related LT - VLAN_EGR_MEMBER_PROFILE).
 */
typedef struct ltsw_vlan_egr_mshp_chk_profile_s {
    /* Egress member ports applied on membership check. */
    bcm_pbmp_t member_ports;
} ltsw_vlan_egr_mshp_chk_profile_t;

/*
 * \brief Egress untag profile structure.
 *
 * This data structure is used to identify the profile structure for egress
 * untag (Related LT - VLAN_EGR_UNTAG_PROFILE).
 */
typedef struct ltsw_vlan_egr_untag_profile_s {
    /* Untagged membership ports. */
    bcm_pbmp_t member_ports;
} ltsw_vlan_egr_untag_profile_t;

/* The counter of block mask sections. */
#define LTSW_VLAN_MASK_SECT_CNT  4

/*
 * \brief Block mask profile structure per VFI.
 *
 * This data structure is used to identify the profile structure for the block
 * mask to be applied on VFI(Related LT - PORT_ING_EGR_BLOCK_1).
 */
typedef struct ltsw_vlan_ing_block_mask_vfi_profile_s {
    ltsw_vlan_block_mask_t mask[LTSW_VLAN_MASK_SECT_CNT];
} ltsw_vlan_ing_block_mask_vfi_profile_t;


/*
 * \brief Block mask control structure.
 */
typedef struct ltsw_vlan_block_mask_ctrl_s {
    /* Offset for known unicast flow. */
    int uc_mask_sel;

    /* Offset for known multicast flow. */
    int mc_mask_sel;

    /* Offset for unknown multicast flow. */
    int umc_mask_sel;

    /* Offset for unknown unicast flow. */
    int uuc_mask_sel;

    /* Offset for broadcast flow. */
    int bc_mask_sel;
} ltsw_vlan_block_mask_ctrl_t;

/*
 * \brief VLAN range map profile structure.
 *
 * This data structure is used to identify the profile structure for VLAN
 * range map (Related LTs - VLAN_ASSIGNMENT_INNER_VLAN_RANGE and
 * VLAN_ASSIGNMENT_OUTER_VLAN_RANGE).
 */
typedef struct ltsw_vlan_range_map_profile_s {
    /* Lower limit of outer vlan range. */
    uint64_t ovid_min[8];

    /* Upper limit of outer vlan range. */
    uint64_t ovid_max[8];

    /* Lower limit of inner vlan range. */
    uint64_t ivid_min[8];

    /* Upper limit of inner vlan range. */
    uint64_t ivid_max[8];
} ltsw_vlan_range_map_profile_t;

#define VLAN_HASH_NUM 32

/*
 * \brief VLAN xlate pipe info.
 */
typedef struct ltsw_vlan_pg_info_s {
    /* Pipe bitmap. */
    uint16_t pipe_bmp;

    /* Head list. */
    uint32_t *head;
} ltsw_vlan_pg_info_t;
/*
 * \brief VLAN xlate per pipe support control.
 */
typedef struct ltsw_xlate_pipe_control_s {
    /* VLAN xlate pipe info.*/
    ltsw_vlan_pg_info_t *pg_info;

    /* Free index.*/
    uint32_t free_idx;

    /* Xlate key data base.*/
    bcmint_vlan_xlate_key_info_t *key_array;

    /* Next list of key_array.*/
    uint32_t *next;
} ltsw_xlate_pipe_control_t;

/*
 * \name Macros for membership information.
 * \anchor LTSW_VLAN_MSHIP_xxx
 */
#define LTSW_VLAN_MSHIP_ING       (1 << 0)
#define LTSW_VLAN_MSHIP_ING_EGR   (1 << 1)
#define LTSW_VLAN_MSHIP_EGR       (1 << 2)

/*
 * \name Macros for the list of functionality
 * \anchor VLAN_F_xxx
 */
#define VLAN_F_VLAN_ENTRY_INIT    (1 << 0)
#define VLAN_F_VLAN_SW_MSHIP      (1 << 1)

/*
 * \name Macros to disable VLAN check.
 * \anchor VLAN_PORT_DIS_xxx
 */
#define VLAN_PORT_DIS_ING_CHK     (1 << 0)
#define VLAN_PORT_DIS_ING_EGR_CHK (1 << 1)
#define VLAN_PORT_DIS_EGR_CHK     (1 << 2)

/*
 * \brief VLAN information.
 */
typedef struct ltsw_vlan_info_s {
    /* VLAN initialized flag. */
    int inited;

    /* Enable list of the functionality. */
    uint32_t flags;

    /* VLAN range lock. */
    sal_mutex_t range_mutex;

    /* VLAN profile lock. */
    sal_mutex_t profile_mutex;

    /* ING VLAN xlate lock. */
    sal_mutex_t ing_xlate_mutex;

    /* EGR VLAN xlate lock. */
    sal_mutex_t egr_xlate_mutex;

    /* Support of VLAN DLF flooding. */
    uint32_t vlan_dlf_flood;

    /* VLAN DLF flooding mode. */
    int vlan_dlf_flood_mode;

    /* block information. */
    ltsw_vlan_block_mask_ctrl_t block_mask_ctrl;

    /* HW table bitmap of per pipe support. */
    uint8_t table_bmp;

    /* Table size of xlate tables. */
    uint32_t xlate_table_size[XLATE_TABLE_CNT];

    /* Enable SW control of xlate tables. */
    uint8_t xlate_ctrl_en;

    /* xlate per pipe control.*/
    ltsw_xlate_pipe_control_t xlate_pipe_ctrl[2];

    /* Membership information for VLAN check. */
    bcmint_vlan_membership_info_t *mship;

    /* Information to disable VLAN check per port. */
    uint8_t *port_dis_chk;
} ltsw_vlan_info_t;

static ltsw_vlan_info_t ltsw_vlan_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* Enable SW control of ing xlate tables. */
#define VLAN_ING_XLATE_CTRL_EN(u) \
    (ltsw_vlan_info[u].xlate_ctrl_en & ((1 << XLATE_TABLE_EGR) - 1))

/* Enable SW control of egr xlate tables. */
#define VLAN_EGR_XLATE_CTRL_EN(u) \
    (ltsw_vlan_info[u].xlate_ctrl_en & \
        ((1 << XLATE_TABLE_EGR) | (1 << XLATE_TABLE_EGR_DW)))

/* Enable pipe support of ing xlate tables. */
#define VLAN_ING_XLATE_PIPE_EN(u) \
    (ltsw_vlan_info[u].table_bmp & ((1 << XLATE_TABLE_EGR) - 1))

/* Enable pipe support of egr xlate tables. */
#define VLAN_EGR_XLATE_PIPE_EN(u) \
    (ltsw_vlan_info[u].table_bmp & \
        ((1 << XLATE_TABLE_EGR) | (1 << XLATE_TABLE_EGR_DW)))

/* Check if egr xlate dw table is supported. */
#define VLAN_EGR_XLATE_DW_EN(u) \
    (ltsw_vlan_info[u].xlate_table_size[XLATE_TABLE_EGR_DW])

/* Get pipe bitmap of ingress port group. */
#define VLAN_ING_XLATE_PG_PIPE(u,pg) \
    (ltsw_vlan_info[u].xlate_pipe_ctrl[0].pg_info[pg].pipe_bmp)

/* Get pipe bitmap of egress port group. */
#define VLAN_EGR_XLATE_PG_PIPE(u,pg) \
    (ltsw_vlan_info[u].xlate_pipe_ctrl[1].pg_info[pg].pipe_bmp)

/* Get head of ingress port group. */
#define VLAN_ING_XLATE_PG_HEAD(u,pg) \
    (ltsw_vlan_info[u].xlate_pipe_ctrl[0].pg_info[pg].head)

/* Get head of egress port group. */
#define VLAN_EGR_XLATE_PG_HEAD(u,pg) \
    (ltsw_vlan_info[u].xlate_pipe_ctrl[1].pg_info[pg].head)

/* Enable to init VLAN entry during init stage. */
#define VLAN_ENTRY_INIT_EN(u) \
    (ltsw_vlan_info[u].flags & VLAN_F_VLAN_ENTRY_INIT)

/* Enable to maintain SW membership. */
#define VLAN_SW_MSHIP_EN(u) \
    (ltsw_vlan_info[u].flags & VLAN_F_VLAN_SW_MSHIP)

/* Take VLAN range lock */
#define VLAN_RANGE_LOCK(u) \
    sal_mutex_take(ltsw_vlan_info[u].range_mutex, SAL_MUTEX_FOREVER)

/* Release VLAN range lock*/
#define VLAN_RANGE_UNLOCK(u) \
    sal_mutex_give(ltsw_vlan_info[u].range_mutex)

/* Take VLAN Profile Lock. */
#define VLAN_PROFILE_LOCK(u) \
    sal_mutex_take(ltsw_vlan_info[u].profile_mutex, SAL_MUTEX_FOREVER)

/* Give VLAN Profile Lock. */
#define VLAN_PROFILE_UNLOCK(u) \
    sal_mutex_give(ltsw_vlan_info[u].profile_mutex)

/* Take VLAN ING XLATE Lock. */
#define VLAN_ING_XLATE_LOCK(u) \
    sal_mutex_take(ltsw_vlan_info[u].ing_xlate_mutex, SAL_MUTEX_FOREVER)

/* Give VLAN ING XLATE Lock. */
#define VLAN_ING_XLATE_UNLOCK(u) \
    sal_mutex_give(ltsw_vlan_info[u].ing_xlate_mutex)

/* Take VLAN EGR XLATE Lock. */
#define VLAN_EGR_XLATE_LOCK(u) \
    sal_mutex_take(ltsw_vlan_info[u].egr_xlate_mutex, SAL_MUTEX_FOREVER)

/* Give VLAN EGR XLATE Lock. */
#define VLAN_EGR_XLATE_UNLOCK(u) \
    sal_mutex_give(ltsw_vlan_info[u].egr_xlate_mutex)

/* Support of VLAN DLF flooding.*/
#define VLAN_DLF_FLOOD(u) (ltsw_vlan_info[u].vlan_dlf_flood)

/*
 * \name Macros for vlan flooding mode
 * \anchor LTSW_VLAN_FLOOD_MODE_xxx
 */
#define LTSW_VLAN_FLOOD_MODE_DEF          0
#define LTSW_VLAN_FLOOD_MODE_EXCLUDE_CPU  1

/* VLAN DLF flooding mode. */
#define VLAN_DLF_FLOOD_MODE(u) (ltsw_vlan_info[u].vlan_dlf_flood_mode)

/* Reserved entry index in VLAN profile table. */
#define VLAN_PROFILE_ING_MSHP_CHK_RESV0 0
#define VLAN_PROFILE_ING_MSHP_CHK_RESV1 \
    BCMI_LTSW_VLAN_PROFILE_ING_MSHP_CHK_IDX_DEF

#define VLAN_PROFILE_EGR_MSHP_CHK_RESV0 0
#define VLAN_PROFILE_EGR_MSHP_CHK_RESV1 \
    BCMI_LTSW_VLAN_PROFILE_EGR_MSHP_CHK_IDX_DEF

#define VLAN_PROFILE_EGR_UNTAG_RESV0 \
    BCMI_LTSW_VLAN_PROFILE_EGR_UNTAG_IDX_DEF

#define VLAN_PROFILE_ING_BLOCK_MASK_RESV0 \
    BCMI_LTSW_VLAN_PROFILE_ING_BLOCK_MASK_IDX_DEF

#define VLAN_PROFILE_RANGE_MAP_RESV0 0

#define VLAN_RESV0 0

/* Internal user data to the callback of vlan range traverse. */
typedef struct ltsw_vlan_range_traverse_data_s {
    /* User callback. */
    bcm_vlan_translate_action_range_traverse_cb cb;

    /* User data. */
    void *user_data;
} ltsw_vlan_range_traverse_data_t;

#define LTSW_VLAN_PBMP_LENGTH  ((BCM_PBMP_PORT_MAX + 64 - 1) / 64)

/* Internal user data to the callback of ingress vlan action traverse. */
typedef struct ltsw_vlan_ing_xlate_action_traverse_data_s {
    /* User callback. */
    bcm_vlan_translate_action_traverse_cb cb;

    /* User data. */
    void *user_data;
} ltsw_vlan_ing_xlate_action_traverse_data_t;

/* Internal user data to the callback of egress vlan action traverse. */
typedef struct ltsw_vlan_egr_xlate_action_traverse_data_s {
    /* User callback. */
    bcm_vlan_translate_egress_action_traverse_cb cb;

    /* User data. */
    void *user_data;
} ltsw_vlan_egr_xlate_action_traverse_data_t;

/*! Opaque control enum. */
typedef enum ltsw_vlan_opaque_control_e {
    OpaqueCtrlDrop = 0,
    OpaqueCtrlCopyToCpu = 1,
    OpaqueCtrlHgExtension = 2,
    OpaqueCtrlCount = 2,
}ltsw_vlan_opaque_control_t;

/*
 * \brief VLAN egress translation structure.
 */
typedef struct ltsw_vlan_egr_xlate_cfg_s {
    /*! Field bitmap. */
    uint32_t fld_bmp;
#define EVT_FLD_OTAG             (1 << 0)
#define EVT_FLD_ITAG             (1 << 1)
#define EVT_FLD_OTAG_ACTION      (1 << 2)
#define EVT_FLD_ITAG_ACTION      (1 << 3)
#define EVT_FLD_DOT1P_MAP_ID     (1 << 4)
#define EVT_FLD_OPAQUE_CTRL      (1 << 5)
#define EVT_FLD_PIPE             (1 << 6)
#define EVT_FLD_FLEX_CTR_ACTION  (1 << 7)
#define EVT_FLD_FLEX_CTR_INDEX   (1 << 8)
#define EVT_FLD_STR_PROFILE_IDX  (1 << 9)

    /* VLAN ID in otag. */
    bcm_vlan_t outer_vlan;

    /* VLAN ID in itag. */
    bcm_vlan_t inner_vlan;

    /* CFI in otag. */
    uint8_t outer_cfi;

    /* CFI in itag. */
    uint8_t inner_cfi;

    /* Priority in otag. */
    int outer_pri;

    /* Priority in itag. */
    int inner_pri;

    /* Otag action. */
    bcm_vlan_action_t otag_action;

    /* Itag action. */
    bcm_vlan_action_t itag_action;

    /* Qos map id of dot1p remarking. */
    int dot1p_map_id;

    /* opaque ctrl id. */
    int opaque_ctrl;

    /* Pipe. */
    int pipe;

    /* Flexctr action. */
    uint32_t flex_ctr_action;

    /* Flexctr index. */
    uint32_t flex_ctr_index;
} ltsw_vlan_egr_xlate_cfg_t;

/*
 * \brief Callback function to handle an egress VLAN translation entry.
 */
typedef int (*ltsw_vlan_egr_xlate_traverse_cb)(
    /* Unit Number.*/
    int unit,

    /* Port ID. */
    bcm_gport_t port,

    /* Outer VLAN. */
    bcm_vlan_t outer_vlan,

    /* Inner VLAN. */
    bcm_vlan_t inner_vlan,

    /* Configuration information for VLAN translation. */
    ltsw_vlan_egr_xlate_cfg_t *cfg,

    /* User data of callback. */
    void *user_data
);

/* Data structure to save the info of ING_VFI_TABLE entry */
typedef struct ltsw_vlan_ing_entry_s {
    /* Index. */
    uint64_t vfi;

    uint64_t fwd_vfi;
    uint64_t dest;
    uint64_t dest_type;
    uint64_t ingress_if;
    uint64_t flex_ctr_action;
    uint64_t stren_prof_idx;
    uint64_t stg;
    uint64_t mship_prof_idx;
    uint64_t block_prof_idx;
    uint64_t outer_tpid_bmp;
    uint64_t opaque_ctrl_id;
    uint64_t pvlan_port_type;
    uint64_t ip4mc_l2_enable;
    uint64_t ip6mc_l2_enable;
    uint64_t l2mc_miss_action;
    uint64_t l2uc_miss_action;
    uint64_t do_not_learn;
    uint64_t l2_host_narrow_enable;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
#define IV_FLD_FWD_VFI                (1 << 0)
#define IV_FLD_DESTINATION            (1 << 1)
#define IV_FLD_INGRESS_IF             (1 << 2)
#define IV_FLD_FLEX_CTR_ACTION        (1 << 3)
#define IV_FLD_STREN_PROF_IDX         (1 << 4)
#define IV_FLD_STG                    (1 << 5)
#define IV_FLD_MSHIP_PROF_IDX         (1 << 6)
#define IV_FLD_BLOCK_PROF_IDX         (1 << 7)
#define IV_FLD_OUTER_TPID_BMP         (1 << 8)
#define IV_FLD_OPAQUE_CTRL_ID         (1 << 9)
#define IV_FLD_PVLAN_PORT_TYPE        (1 << 10)
#define IV_FLD_IP4MC_L2_EN            (1 << 11)
#define IV_FLD_IP6MC_L2_EN            (1 << 12)
#define IV_FLD_L2MC_MISS_ACTION       (1 << 13)
#define IV_FLD_L2UC_MISS_ACTION       (1 << 14)
#define IV_FLD_NOT_LEARN              (1 << 15)
#define IV_FLD_L2_HOST_NARROW_ENABLE  (1 << 16)
} ltsw_vlan_ing_entry_t;

/* Data structure to save the info of EGR_VFI entry */
typedef struct ltsw_vlan_egr_entry_s {
    /* Index. */
    uint64_t vfi;

    uint64_t o_vlan;
    uint64_t i_vlan;
    uint64_t tag_action;
    uint64_t untag_prof_idx;
    uint64_t stg;
    uint64_t mship_prof_idx;
    uint64_t flex_ctr_action;
    uint64_t efp_ctrl_id;
    uint64_t class_id;
    uint64_t view;
    uint64_t vsid;
    uint64_t strength_prfl_idx;

    /* Bitmap of fields to be operated. */
    uint64_t fld_bmp;
#define EV_FLD_O_VLAN                (1 << 0)
#define EV_FLD_I_VLAN                (1 << 1)
#define EV_FLD_TAG_ACTION            (1 << 2)
#define EV_FLD_UNTAG_PROF_IDX        (1 << 3)
#define EV_FLD_STG                   (1 << 4)
#define EV_FLD_MSHIP_PROF_IDX        (1 << 5)
#define EV_FLD_FLEX_CTR_ACTION       (1 << 6)
#define EV_FLD_EFP_CTRL_ID           (1 << 7)
#define EV_FLD_CLASS_ID              (1 << 8)
#define EV_FLD_VIEW                  (1 << 9)
#define EV_FLD_VSID                  (1 << 10)
#define EV_FLD_STRENGTH_PRFL_IDX     (1 << 11)
} ltsw_vlan_egr_entry_t;

#define LTSW_VLAN_EGR_VFI_VIEW_DEFAULT VT_VFI_DEFAULTs

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Clear VLAN information in L2 station associated to a specified VLAN.
 *
 * This function is used to clear VLAN information in L2 station associated to
 * a specified VLAN.
 *
 * \param [in] unit              Unit number.
 * \param [in] vfi               VFI index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_l2_station_clear(int unit, int vfi)
{
    bcmi_ltsw_l2_station_vlan_t l2_notify_info;
    int l2_notify_flags = 0;

    SHR_FUNC_ENTER(unit);

    if (vfi == 0) {
        SHR_EXIT();
    }

    sal_memset(&l2_notify_info, 0, sizeof(bcmi_ltsw_l2_station_vlan_t));

    l2_notify_flags = BCMI_LTSW_L2_STATION_VLAN_F_MEMBER_PRF_PTR |
                      BCMI_LTSW_L2_STATION_VLAN_F_STG;

    /* Notify L2 STATION module. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_station_vlan_update(unit, vfi,
                                          l2_notify_flags,
                                          &l2_notify_info));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VLAN information in L2 station associated to a specified VLAN.
 *
 * This function is used to update VLAN information in L2 station associated to
 * a specified VLAN.
 *
 * \param [in] unit              Unit number.
 * \param [out] entry            Ingress table configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_l2_station_update(int unit, ltsw_vlan_ing_entry_t *entry)
{
    bcmi_ltsw_l2_station_vlan_t l2_notify_info;
    int l2_notify_flags = 0;

    SHR_FUNC_ENTER(unit);

    if (entry->vfi == 0) {
        SHR_EXIT();
    }

    sal_memset(&l2_notify_info, 0, sizeof(bcmi_ltsw_l2_station_vlan_t));

    if (entry->fld_bmp & IV_FLD_STG) {
        l2_notify_flags |= BCMI_LTSW_L2_STATION_VLAN_F_STG;
        l2_notify_info.stg = (uint16_t)entry->stg;
    }

    if (entry->fld_bmp & IV_FLD_MSHIP_PROF_IDX) {
        l2_notify_flags |= BCMI_LTSW_L2_STATION_VLAN_F_MEMBER_PRF_PTR;
        l2_notify_info.member_prf_ptr = (uint16_t)entry->mship_prof_idx;
    }

    /* Notify L2 STATION module. */
    if (l2_notify_flags != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l2_station_vlan_update(unit, entry->vfi,
                                              l2_notify_flags,
                                              &l2_notify_info));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear VLAN information in L3 interface associated to a specified VLAN.
 *
 * This function is used to clear VLAN information in L3 interface associated to
 * a specified VLAN.
 *
 * \param [in] unit              Unit number.
 * \param [in] vfi               VFI index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_l3_intf_clear(int unit, int vfi)
{
    bcmi_ltsw_l3_intf_vfi_t l3_notify_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (vfi == 0) {
        SHR_EXIT();
    }

    sal_memset(&l3_notify_info, 0, sizeof(bcmi_ltsw_l3_intf_vfi_t));

    l3_notify_info.flags = BCMI_LTSW_L3_INTF_VFI_F_TAG |
                           BCMI_LTSW_L3_INTF_VFI_F_TAG_ACTION |
                           BCMI_LTSW_L3_INTF_VFI_F_MEMBERSHIP_PRFL_IDX |
                           BCMI_LTSW_L3_INTF_VFI_F_UNTAG_PRFL_IDX |
                           BCMI_LTSW_L3_INTF_VFI_F_STG;

    /*
     * VLAN is initialized before L3 module, L3 itself need the recover
     * the information via l3_intf_update_all().
     */
    if (!bcmi_ltsw_init_state_get(unit)) {
        rv = bcmi_ltsw_l3_intf_vfi_update(unit, vfi, &l3_notify_info);
        if (rv != SHR_E_INIT) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear VLAN information in L3 interface associated to a specified VLAN.
 *
 * This function is used to clear VLAN information in L3 interface associated to
 * a specified VLAN.
 *
 * \param [in] unit              Unit number.
 * \param [out] entry            Egress table configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_l3_intf_update(int unit, ltsw_vlan_egr_entry_t *entry)
{
    bcmi_ltsw_l3_intf_vfi_t l3_notify_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (entry->vfi == 0) {
        SHR_EXIT();
    }

    sal_memset(&l3_notify_info, 0, sizeof(bcmi_ltsw_l3_intf_vfi_t));

    if (entry->fld_bmp & EV_FLD_O_VLAN) {
        l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_TAG;
        l3_notify_info.tag = (uint16_t)entry->o_vlan;
    }

    if (entry->fld_bmp & EV_FLD_TAG_ACTION) {
        l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_TAG_ACTION;
        l3_notify_info.tag_action = (entry->tag_action & 0x1) ? 0x1: 0x0;
    }

    if (entry->fld_bmp & EV_FLD_MSHIP_PROF_IDX) {
        l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_MEMBERSHIP_PRFL_IDX;
        l3_notify_info.membership_prfl_idx = (uint16_t)entry->mship_prof_idx;
    }

    if (entry->fld_bmp & EV_FLD_UNTAG_PROF_IDX) {
        l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_UNTAG_PRFL_IDX;
        l3_notify_info.untag_prfl_idx = (uint16_t)entry->untag_prof_idx;
    }

    if (entry->fld_bmp & EV_FLD_STG) {
        l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_STG;
        l3_notify_info.stg = (uint16_t)entry->stg;
    }

    /*
     * VLAN is initialized before L3 module, L3 itself need the recover
     * the information via l3_intf_update_all().
     */
    if ((l3_notify_info.flags != 0) && !bcmi_ltsw_init_state_get(unit)) {
        rv = bcmi_ltsw_l3_intf_vfi_update(unit, entry->vfi, &l3_notify_info);
        if (rv != SHR_E_INIT) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VFI information in L2 station associated to a specified VLAN.
 *
 * This function is used to update default VFI member information in L2 station
 * associated to a specified VFI.
 *
 * \param [in] unit              Unit number.
 * \param [in] vfi               VFI index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_l2_station_default_update(int unit, int vfi)
{
    bcmi_ltsw_l2_station_vlan_t l2_notify_info;
    int l2_notify_flags = 0;

    SHR_FUNC_ENTER(unit);

    if (vfi == 0) {
        SHR_EXIT();
    }

    sal_memset(&l2_notify_info, 0, sizeof(bcmi_ltsw_l2_station_vlan_t));

    l2_notify_flags |= BCMI_LTSW_L2_STATION_VLAN_F_MEMBER_PRF_PTR;
    l2_notify_info.member_prf_ptr = BCMI_LTSW_VLAN_PROFILE_ING_MSHP_CHK_IDX_DEF;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_station_vlan_update(unit, vfi,
                                          l2_notify_flags,
                                          &l2_notify_info));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VFI information in L3 interface associated to a specified VLAN.
 *
 * This function is used to update default VFI member information in L3
 * interface associated to a specified VFI.
 *
 * \param [in] unit              Unit number.
 * \param [in] vfi               VFI index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_l3_intf_default_update(int unit, int vfi)
{
    bcmi_ltsw_l3_intf_vfi_t l3_notify_info;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (vfi == 0) {
        SHR_EXIT();
    }

    sal_memset(&l3_notify_info, 0, sizeof(bcmi_ltsw_l3_intf_vfi_t));

    l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_MEMBERSHIP_PRFL_IDX;
    l3_notify_info.membership_prfl_idx =
        BCMI_LTSW_VLAN_PROFILE_EGR_MSHP_CHK_IDX_DEF;

    l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_UNTAG_PRFL_IDX;
    l3_notify_info.untag_prfl_idx =
        BCMI_LTSW_VLAN_PROFILE_EGR_UNTAG_IDX_DEF;

    /*
     * VLAN is initialized before L3 module, L3 itself need the recover
     * the information via l3_intf_update_all().
     */
    if (!bcmi_ltsw_init_state_get(unit)) {
        rv = bcmi_ltsw_l3_intf_vfi_update(unit, vfi, &l3_notify_info);
        if (rv != SHR_E_INIT) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the fields of ingress VLAN  entry.
 *
 * This function is used to set the fields of ingress VLAN  entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container.
 * \param [in] entry             Ingress table configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_entry_fields_set(int unit,
                                bcmlt_entry_handle_t ent_hdl,
                                ltsw_vlan_ing_entry_t *entry)
{
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    int fid;
    const char *str_value;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_ING_VFI_TABLE, &lt_info));
    flds = lt_info->flds;

    fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI].name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, fld_name, entry->vfi));

    if (entry->fld_bmp & IV_FLD_FWD_VFI) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_MAPPED_VFI].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->fwd_vfi));
    }

    if (entry->fld_bmp & IV_FLD_DESTINATION) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl,
                                   fld_name, entry->dest));
        fid = BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION_TYPE;
        fld_name = flds[fid].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].scalar_to_symbol(unit, entry->dest_type, &str_value));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(ent_hdl, fld_name, str_value));
    }

    if (entry->fld_bmp & IV_FLD_INGRESS_IF) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_L3_IIF].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->ingress_if));
    }

    if (entry->fld_bmp & IV_FLD_FLEX_CTR_ACTION) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_FLEX_CTR_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->flex_ctr_action));
    }

    if (entry->fld_bmp & IV_FLD_STREN_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_STRENGTH_PROFILE_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->stren_prof_idx));
    }

    if (entry->fld_bmp & IV_FLD_STG) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_SPANNING_TREE_GROUP].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->stg));
    }

    if (entry->fld_bmp & IV_FLD_MSHIP_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_MEMBERSHIP_PROFILE_PTR].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->mship_prof_idx));
    }

    if (entry->fld_bmp & IV_FLD_BLOCK_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_BITMAP_PROFILE_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->block_prof_idx));
    }

    if (entry->fld_bmp & IV_FLD_OUTER_TPID_BMP) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_EXPECTED_OUTER_TPID_BITMAP].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->outer_tpid_bmp));
    }

    if (entry->fld_bmp & IV_FLD_OPAQUE_CTRL_ID) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_OPAQUE_CTRL_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->opaque_ctrl_id));
    }

    if (entry->fld_bmp & IV_FLD_PVLAN_PORT_TYPE) {
        fid = BCMINT_LTSW_VLAN_FLD_ING_VFI_SRC_PVLAN_PORT_TYPE;
        fld_name = flds[fid].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].scalar_to_symbol(unit, entry->pvlan_port_type, &str_value));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(ent_hdl, fld_name, str_value));
    }

    if (entry->fld_bmp & IV_FLD_IP4MC_L2_EN) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV4MC_L2_ENABLE].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->ip4mc_l2_enable));
    }

    if (entry->fld_bmp & IV_FLD_IP6MC_L2_EN) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV6MC_L2_ENABLE].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->ip6mc_l2_enable));
    }

    if (entry->fld_bmp & IV_FLD_L2MC_MISS_ACTION) {
        fid = BCMINT_LTSW_VLAN_FLD_ING_VFI_L2MC_MISS_ACTION;
        fld_name = flds[fid].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].scalar_to_symbol(unit, entry->l2mc_miss_action, &str_value));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(ent_hdl, fld_name, str_value));
    }

    if (entry->fld_bmp & IV_FLD_L2UC_MISS_ACTION) {
        fid = BCMINT_LTSW_VLAN_FLD_ING_VFI_L2UC_MISS_ACTION;
        fld_name = flds[fid].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].scalar_to_symbol(unit, entry->l2uc_miss_action, &str_value));
        SHR_IF_ERR_VERBOSE_EXIT
             (bcmlt_entry_field_symbol_add(ent_hdl, fld_name, str_value));
    }

    if (entry->fld_bmp & IV_FLD_NOT_LEARN) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_DO_NOT_LEARN].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->do_not_learn));
    }

    if (entry->fld_bmp & IV_FLD_L2_HOST_NARROW_ENABLE) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_L2_HOST_NARROW_LOOKUP_ENABLE].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->l2_host_narrow_enable));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the fields of ingress VLAN  entry.
 *
 * This function is used to get the fields of ingress VLAN  entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container.
 * \param [out] entry            Ingress table configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_entry_fields_get(int unit,
                          bcmlt_entry_handle_t ent_hdl,
                          ltsw_vlan_ing_entry_t *entry)
{
    const char *value;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    int fid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_ING_VFI_TABLE, &lt_info));
    flds = lt_info->flds;

    fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->vfi));

    if (entry->fld_bmp & IV_FLD_FWD_VFI) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_MAPPED_VFI].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->fwd_vfi));
    }

    if (entry->fld_bmp & IV_FLD_DESTINATION) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl,
                                   fld_name, &entry->dest));
        fid = BCMINT_LTSW_VLAN_FLD_ING_VFI_DESTINATION_TYPE;
        fld_name = flds[fid].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(ent_hdl, fld_name, &value));
        SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].symbol_to_scalar(unit, value, &entry->dest_type));
    }

    if (entry->fld_bmp & IV_FLD_INGRESS_IF) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_L3_IIF].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->ingress_if));
    }

    if (entry->fld_bmp & IV_FLD_FLEX_CTR_ACTION) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_FLEX_CTR_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->flex_ctr_action));
    }

    if (entry->fld_bmp & IV_FLD_STREN_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_STRENGTH_PROFILE_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->stren_prof_idx));
    }

    if (entry->fld_bmp & IV_FLD_STG) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_SPANNING_TREE_GROUP].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->stg));
    }

    if (entry->fld_bmp & IV_FLD_MSHIP_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_MEMBERSHIP_PROFILE_PTR].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->mship_prof_idx));
    }

    if (entry->fld_bmp & IV_FLD_BLOCK_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_BITMAP_PROFILE_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->block_prof_idx));
    }

    if (entry->fld_bmp & IV_FLD_OUTER_TPID_BMP) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_EXPECTED_OUTER_TPID_BITMAP].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->outer_tpid_bmp));
    }

    if (entry->fld_bmp & IV_FLD_OPAQUE_CTRL_ID) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_OPAQUE_CTRL_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->opaque_ctrl_id));
    }

    if (entry->fld_bmp & IV_FLD_PVLAN_PORT_TYPE) {
        fid = BCMINT_LTSW_VLAN_FLD_ING_VFI_SRC_PVLAN_PORT_TYPE;
        fld_name = flds[fid].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(ent_hdl, fld_name, &value));
        SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].symbol_to_scalar(unit, value, &entry->pvlan_port_type));
    }

    if (entry->fld_bmp & IV_FLD_IP4MC_L2_EN) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV4MC_L2_ENABLE].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->ip4mc_l2_enable));
    }

    if (entry->fld_bmp & IV_FLD_IP6MC_L2_EN) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_IPV6MC_L2_ENABLE].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->ip6mc_l2_enable));
    }

    if (entry->fld_bmp & IV_FLD_L2MC_MISS_ACTION) {
        fid = BCMINT_LTSW_VLAN_FLD_ING_VFI_L2MC_MISS_ACTION;
        fld_name = flds[fid].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(ent_hdl, fld_name, &value));
        SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].symbol_to_scalar(unit, value, &entry->l2mc_miss_action));
    }

    if (entry->fld_bmp & IV_FLD_L2UC_MISS_ACTION) {
        fid = BCMINT_LTSW_VLAN_FLD_ING_VFI_L2UC_MISS_ACTION;
        fld_name = flds[fid].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(ent_hdl, fld_name, &value));
        SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].symbol_to_scalar(unit, value, &entry->l2uc_miss_action));

    }

    if (entry->fld_bmp & IV_FLD_NOT_LEARN) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_DO_NOT_LEARN].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->do_not_learn));
    }

    if (entry->fld_bmp & IV_FLD_L2_HOST_NARROW_ENABLE) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_L2_HOST_NARROW_LOOKUP_ENABLE].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->l2_host_narrow_enable));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Access entry of the LT ING_VFI_TABLE.
 *
 * \param [in] unit       Unit number.
 * \param [in] opcode     Operation code.
 * \param [in] entry      Entry info.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_ing_table_op(int unit, bcmlt_opcode_t opcode,
                  ltsw_vlan_ing_entry_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_ING_VFI_TABLE, &lt_info));
    flds = lt_info->flds;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));

    switch (opcode) {
        case BCMLT_OPCODE_LOOKUP:
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, entry->vfi));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, ent_hdl, opcode,
                                      BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_entry_fields_get(unit, ent_hdl, entry));
            break;
        case BCMLT_OPCODE_INSERT:
        case BCMLT_OPCODE_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_entry_fields_set(unit, ent_hdl, entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, ent_hdl, opcode,
                                      BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_l2_station_update(unit, entry));
            break;
        case BCMLT_OPCODE_DELETE:
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, entry->vfi));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, ent_hdl, opcode,
                                      BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_l2_station_clear(unit, entry->vfi));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the fields of egress VLAN  entry.
 *
 * This function is used to set the fields of egress VLAN  entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container.
 * \param [in] entry             Ingress table configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_entry_fields_set(int unit,
                          bcmlt_entry_handle_t ent_hdl,
                          ltsw_vlan_egr_entry_t *entry)
{
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    int fid;
    const char *str_value;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit,
                                BCMINT_LTSW_VLAN_EGR_VFI, &lt_info));
    flds = lt_info->flds;
    fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI].name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, fld_name, entry->vfi));

    if (entry->fld_bmp & EV_FLD_O_VLAN) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_0].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->o_vlan));
    }

    if (entry->fld_bmp & EV_FLD_I_VLAN) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_1].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->i_vlan));
    }

    if (entry->fld_bmp & EV_FLD_TAG_ACTION) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_TAG_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->tag_action));
    }

    if (entry->fld_bmp & EV_FLD_UNTAG_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_UNTAG_PROFILE_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->untag_prof_idx));
    }

    if (entry->fld_bmp & EV_FLD_STG) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_STG].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->stg));
    }

    if (entry->fld_bmp & EV_FLD_MSHIP_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_MEMBERSHIP_PROFILE_IDX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->mship_prof_idx));
    }

    if (entry->fld_bmp & EV_FLD_FLEX_CTR_ACTION) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_FLEX_CTR_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name,
                                   entry->flex_ctr_action));
    }

    if (entry->fld_bmp & EV_FLD_EFP_CTRL_ID) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_EFP_CTRL_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->efp_ctrl_id));
    }

    if (entry->fld_bmp & EV_FLD_CLASS_ID) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_CLASS_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->class_id));
    }

    if (entry->fld_bmp & EV_FLD_VIEW) {
        fid = BCMINT_LTSW_VLAN_FLD_EGR_VFI_VIEW_T;
        fld_name = flds[fid].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].scalar_to_symbol(unit, entry->view, &str_value));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(ent_hdl, fld_name, str_value));
    }

    if (entry->fld_bmp & EV_FLD_VSID) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VSID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->vsid));
    }

    if (entry->fld_bmp & EV_FLD_STRENGTH_PRFL_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_STRENGTH_PRFL_IDX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, entry->strength_prfl_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the fields of egress VLAN  entry.
 *
 * This function is used to get the fields of egress VLAN  entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container.
 * \param [out] entry            Ingress table configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_entry_fields_get(int unit,
                          bcmlt_entry_handle_t ent_hdl,
                          ltsw_vlan_egr_entry_t *entry)
{
    const char *value;
    uint64_t view = EGR_VFI_VIEW_DEFAULT;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    int fid;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit,
                                BCMINT_LTSW_VLAN_EGR_VFI, &lt_info));
    flds = lt_info->flds;

    fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI].name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->vfi));

    if (entry->fld_bmp & EV_FLD_O_VLAN) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_0].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->o_vlan));
    }

    if (entry->fld_bmp & EV_FLD_TAG_ACTION) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_TAG_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->tag_action));
    }

    if (entry->fld_bmp & EV_FLD_UNTAG_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_UNTAG_PROFILE_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->untag_prof_idx));
    }

    if (entry->fld_bmp & EV_FLD_STG) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_STG].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->stg));
    }

    if (entry->fld_bmp & EV_FLD_MSHIP_PROF_IDX) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_MEMBERSHIP_PROFILE_IDX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->mship_prof_idx));
    }

    if (entry->fld_bmp & EV_FLD_FLEX_CTR_ACTION) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_FLEX_CTR_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name,
                                   &entry->flex_ctr_action));
    }

    if (entry->fld_bmp & EV_FLD_EFP_CTRL_ID) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_EFP_CTRL_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->efp_ctrl_id));
    }

    if (entry->fld_bmp & EV_FLD_CLASS_ID) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_CLASS_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->class_id));
    }

    fid = BCMINT_LTSW_VLAN_FLD_EGR_VFI_VIEW_T;
    fld_name = flds[fid].name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(ent_hdl, fld_name, &value));
    SHR_IF_ERR_VERBOSE_EXIT
            (flds[fid].symbol_to_scalar(unit, value, &view));
    if (view == EGR_VFI_VIEW_DEFAULT) {
        /*VLAN_1 is only available in default view.*/
        if (entry->fld_bmp & EV_FLD_I_VLAN) {
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VLAN_1].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->i_vlan));
        }
    } else if (view == EGR_VFI_VIEW_VSID) {
        /*VSID is only available in vsid view.*/
        if (entry->fld_bmp & EV_FLD_VSID) {
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VSID].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &entry->vsid));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (entry->fld_bmp & EV_FLD_VIEW) {
        entry->view = view;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Access entry of the LT EGR_VFI.
 *
 * \param [in] unit       Unit number.
 * \param [in] opcode     Operation code.
 * \param [in] entry      Entry info.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_egr_table_op(int unit, bcmlt_opcode_t opcode,
                  ltsw_vlan_egr_entry_t *entry)
{
    int dunit;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit,
             BCMINT_LTSW_VLAN_EGR_VFI, &lt_info));
    flds = lt_info->flds;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));

    switch (opcode) {
        case BCMLT_OPCODE_LOOKUP:
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, entry->vfi));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, ent_hdl, opcode,
                                      BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_entry_fields_get(unit, ent_hdl, entry));
            break;
        case BCMLT_OPCODE_INSERT:
        case BCMLT_OPCODE_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_entry_fields_set(unit, ent_hdl, entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, ent_hdl, opcode,
                                      BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_l3_intf_update(unit, entry));
            break;
        case BCMLT_OPCODE_DELETE:
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, entry->vfi));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, ent_hdl, opcode,
                                      BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_l3_intf_clear(unit, entry->vfi));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set member ports field into an LT entry container.
 *
 * This function is used to set member ports field into an LT entry container.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  ent_hdl              LT entry handle.
 * \param [in]  tbl_name             LT table name.
 * \param [in]  fld_name             LT field name.
 * \param [in]  member_ports         Member ports.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_set(int unit,
                      bcmlt_entry_handle_t ent_hdl,
                      const char *tbl_name,
                      const char *fld_name,
                      bcm_pbmp_t member_ports)
{
    uint8_t *port_dis_chk = NULL;
    uint8_t dis_chk_flg = 0;
    int port_num = 0;
    uint64_t value = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    if(VLAN_SW_MSHIP_EN(unit)) {
        port_dis_chk = ltsw_vlan_info[unit].port_dis_chk;
        if (sal_strcmp(VLAN_ING_MEMBER_PROFILEs, tbl_name) == 0) {
            dis_chk_flg = VLAN_PORT_DIS_ING_CHK;
        } else if (sal_strcmp(VLAN_ING_EGR_MEMBER_PORTS_PROFILEs, tbl_name) == 0) {
            dis_chk_flg = VLAN_PORT_DIS_ING_EGR_CHK;
        } else if (sal_strcmp(VLAN_EGR_MEMBER_PROFILEs, tbl_name) == 0) {
            dis_chk_flg = VLAN_PORT_DIS_EGR_CHK;
        }
    }

    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < port_num; i++) {

       if (port_dis_chk && (port_dis_chk[i] & dis_chk_flg)) {
           /* Set to 1 if check is disabled. */
           value = 1;
       } else {
           value = BCM_PBMP_MEMBER(member_ports, i) ? 1 : 0;
       }

       SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_array_add(ent_hdl, fld_name, i, &value, 1));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get member ports field from an LT entry container.
 *
 * This function is used to set member ports fields from an LT entry container.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  ent_hdl              LT entry handle.
 * \param [in]  fld_name             LT field name.
 * \param [out] member_ports         Member ports.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_member_ports_get(int unit,
                      bcmlt_entry_handle_t ent_hdl,
                      const char *fld_name,
                      bcm_pbmp_t *member_ports)
{
    int max_port_num = 0;
    uint64_t value = 0;
    uint32_t r_elem_cnt = 0, i = 0;
    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(*member_ports);
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(ent_hdl, fld_name, i,
                                         &value, 1, &r_elem_cnt));
        if(value) {
            BCM_PBMP_PORT_ADD(*member_ports, i);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into VLAN_ING_MEMBER_PROFILE table.
 *
 * This function is used to add an entry into VLAN_ING_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_member_profile_entry_add(int unit,
                                  int profile_idx,
                                  ltsw_vlan_ing_mshp_chk_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Add a entry into VLAN_ING_MEMBER_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_ING_MEMBER_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_MEMBER_PROFILE_IDs, profile_idx));

    /* Set ingress member ports applied on membership check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, ent_hdl,
                               VLAN_ING_MEMBER_PROFILEs,
                               MEMBER_PORTSs,
                               profile->member_ports));

    /* Insert VLAN_ING_MEMBER_PROFILE entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into VLAN_ING_MEMBER_PROFILE SW table.
 *
 * This function is used to add an entry into VLAN_ING_MEMBER_PROFILE SW table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_member_profile_entry_sw_add(int unit,
                                     int profile_idx,
                                     ltsw_vlan_ing_mshp_chk_profile_t *profile)
{
    bcmint_vlan_membership_info_t *mship = NULL;
    int offset = 0, port_num = 0, port = 0;

    mship = ltsw_vlan_info[unit].mship;
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    offset = port_num * profile_idx;

    for (port = 0; port < port_num; port++) {
       if (BCM_PBMP_MEMBER(profile->member_ports, port)) {
           mship[offset + port].port_info |= LTSW_VLAN_MSHIP_ING;
       } else {
           mship[offset + port].port_info &= ~LTSW_VLAN_MSHIP_ING;
       }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Update the entry of VLAN_ING_MEMBER_PROFILE table with a given port.
 *
 * This function is used to update the entry of VLAN_ING_MEMBER_PROFILE table
 * with a given port
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  port                 Port number.
 * \param [in]  disable              Disable or enable.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_member_profile_entry_port_update(int unit, int profile_idx,
                                          bcm_port_t port, int disable)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmint_vlan_membership_info_t *mship = NULL;
    int port_num = 0, dunit = 0;
    uint64_t value = 0;

    SHR_FUNC_ENTER(unit);

    if (VLAN_SW_MSHIP_EN(unit)) {
        mship = ltsw_vlan_info[unit].mship;
        port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;

        if (disable) {
            /* Set coresponding bit in membersihp. */
            value = 1;
        } else {
            /* Trust SW information set by PORT_ADD APIs. */
            if (mship[port_num * profile_idx + port].port_info &
                LTSW_VLAN_MSHIP_ING) {
                value = 1;
            } else {
                value = 0;
            }
        }
    } else {
       value = disable ? 1 : 0;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_ING_MEMBER_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_MEMBER_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(ent_hdl, MEMBER_PORTSs, port, &value, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from VLAN_ING_MEMBER_PROFILE table.
 *
 * This function is used to get an entry from VLAN_ING_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_member_profile_entry_get(int unit,
                                  int profile_idx,
                                  ltsw_vlan_ing_mshp_chk_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Find a profile entry from VLAN_ING_MEMBER_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_ING_MEMBER_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_MEMBER_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Get ingress member ports. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_get(unit, ent_hdl, MEMBER_PORTSs,
                               &(profile->member_ports)));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from VLAN_ING_MEMBER_PROFILE SW table.
 *
 * This function is used to get an entry from VLAN_ING_MEMBER_PROFILE SW table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_member_profile_entry_sw_get(int unit,
                                     int profile_idx,
                                     ltsw_vlan_ing_mshp_chk_profile_t *profile)
{
    bcmint_vlan_membership_info_t *mship = NULL;
    int offset = 0, port_num = 0, port = 0;

    mship = ltsw_vlan_info[unit].mship;
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    offset = port_num * profile_idx;

    for (port = 0; port < port_num; port++) {
        if (mship[offset + port].port_info & LTSW_VLAN_MSHIP_ING) {
            BCM_PBMP_PORT_ADD(profile->member_ports, port);
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Delete an entry from VLAN_ING_MEMBER_PROFILE table.
 *
 * This function is used to delete an entry from VLAN_ING_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_member_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_ING_MEMBER_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_MEMBER_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from SW VLAN_ING_MEMBER_PROFILE table.
 *
 * This function is used to delete an entry from SW VLAN_ING_MEMBER_PROFILE
 * table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_member_profile_entry_sw_delete(int unit, int profile_idx)
{
    bcmint_vlan_membership_info_t *mship = NULL;
    int offset = 0, port_num = 0, port = 0;

    mship = ltsw_vlan_info[unit].mship;
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    offset = port_num * profile_idx;

    for (port = 0; port < port_num; port++) {
        mship[offset + port].port_info &= ~LTSW_VLAN_MSHIP_ING;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Add an entry into VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to add an entry into VLAN_ING_EGR_MEMBER_PORTS_PROFILE
 * table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_egr_member_profile_entry_add(int unit,
                                      int profile_idx,
                                      ltsw_vlan_ing_mshp_chk_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *data = NULL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Add a entry into VLAN_ING_EGR_MEMBER_PORTS_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_ING_EGR_MEMBER_PORTS_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               profile_idx));

    /* Set egress member ports in ingress pipeline. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, ent_hdl,
                               VLAN_ING_EGR_MEMBER_PORTS_PROFILEs,
                               EGR_MEMBER_PORTSs,
                               profile->egr_member_ports));

    /* Set mask target of egress member ports. */
    if (profile->egr_mask_target == LTSW_VLAN_MASK_TARGET_L2_ONLY) {
        data = L2_MEMBERs;
    } else if (profile->egr_mask_target == LTSW_VLAN_MASK_TARGET_L3_ONLY) {
        data = L3_MEMBERs;
    } else {
        data = L2_L3_MEMBERs;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, MASK_TARGETs, data));

    /* Set mask action of egress member ports. */
    data = profile->egr_mask_action ? ORs : ANDs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, MASK_ACTIONs, data));

    /* Insert VLAN_ING_EGR_MEMBER_PORTS_PROFILE entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into SW VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to add an entry into SW VLAN_ING_EGR_MEMBER_PORTS
 * _PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_egr_member_profile_entry_sw_add(
    int unit,
    int profile_idx,
    ltsw_vlan_ing_mshp_chk_profile_t *profile)
{
    bcmint_vlan_membership_info_t *mship = NULL;
    int offset = 0, port_num = 0, port = 0;

    mship = ltsw_vlan_info[unit].mship;
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    offset = port_num * profile_idx;

    for (port = 0; port < port_num; port++) {
       if (BCM_PBMP_MEMBER(profile->egr_member_ports, port)) {
           mship[offset + port].port_info |= LTSW_VLAN_MSHIP_ING_EGR;
       } else {
           mship[offset + port].port_info &= ~LTSW_VLAN_MSHIP_ING_EGR;
       }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Update VLAN_ING_EGR_MEMBER_PORTS_PROFILE entry with a given port.
 *
 * This function is used to update VLAN_ING_EGR_MEMBER_PORTS_PROFILE entry
 * with a given port
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  port                 Port number.
 * \param [in]  disable              Disable or enable.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_egr_member_profile_entry_port_update(int unit, int profile_idx,
                                              bcm_port_t port, int disable)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmint_vlan_membership_info_t *mship = NULL;
    int port_num = 0, dunit = 0;
    uint64_t value = 0;

    SHR_FUNC_ENTER(unit);

    if (VLAN_SW_MSHIP_EN(unit)) {
        mship = ltsw_vlan_info[unit].mship;
        port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;

        if (disable) {
            /* Set coresponding bit in membersihp. */
            value = 1;
        } else {
            /* Trust SW information set by PORT_ADD APIs. */
            if (mship[port_num * profile_idx + port].port_info &
                LTSW_VLAN_MSHIP_ING_EGR) {
                value = 1;
            } else {
                value = 0;
            }
        }
    } else {
       value = disable ? 1 : 0;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_ING_EGR_MEMBER_PORTS_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(ent_hdl,
                                     EGR_MEMBER_PORTSs, port, &value, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to get an entry from VLAN_ING_EGR_MEMBER_PORTS_PROFILE
 * table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_egr_member_profile_entry_get(int unit,
                                      int profile_idx,
                                      ltsw_vlan_ing_mshp_chk_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *data = NULL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Find a profile entry from VLAN_ING_EGR_MEMBER_PORTS_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_ING_EGR_MEMBER_PORTS_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));


    /* Get egress member ports in ingress pipeline. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_get(unit, ent_hdl, EGR_MEMBER_PORTSs,
                               &(profile->egr_member_ports)));

    /* Get mask target of egress member ports. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(ent_hdl, MASK_TARGETs, &data));
    if (sal_strcmp(L2_MEMBERs, data) == 0) {
        profile->egr_mask_target = LTSW_VLAN_MASK_TARGET_L2_ONLY;
    } else if (sal_strcmp(L3_MEMBERs, data) == 0) {
        profile->egr_mask_target = LTSW_VLAN_MASK_TARGET_L3_ONLY;
    } else {
        profile->egr_mask_target = LTSW_VLAN_MASK_TARGET_L2_AND_L3;
    }

    /* Get mask action of egress member ports. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(ent_hdl, MASK_ACTIONs, &data));
    if (sal_strcmp(ORs, data) == 0) {
        profile->egr_mask_action = LTSW_VLAN_MASK_ACTION_OR;
    } else {
        profile->egr_mask_action = LTSW_VLAN_MASK_ACTION_AND;
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from SW VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to get an entry from SW VLAN_ING_EGR_MEMBER_PORTS
 * _PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_egr_member_profile_entry_sw_get(
    int unit,
    int profile_idx,
    ltsw_vlan_ing_mshp_chk_profile_t *profile)
{
    bcmint_vlan_membership_info_t *mship = NULL;
    int offset = 0, port_num = 0, port = 0;

    mship = ltsw_vlan_info[unit].mship;
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    offset = port_num * profile_idx;

    for (port = 0; port < port_num; port++) {
        if (mship[offset + port].port_info & LTSW_VLAN_MSHIP_ING_EGR) {
            BCM_PBMP_PORT_ADD(profile->egr_member_ports, port);
        }
    }

    /* Fixed data. */
    profile->egr_mask_target = LTSW_VLAN_MASK_TARGET_L2_ONLY;
    profile->egr_mask_action = LTSW_VLAN_MASK_ACTION_AND;

    return SHR_E_NONE;
}

/*!
 * \brief Delete an entry from VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to delete an entry from
 * VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_egr_member_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_ING_EGR_MEMBER_PORTS_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_ING_EGR_MEMBER_PORTS_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from SW VLAN_ING_EGR_MEMBER_PORTS_PROFILE table.
 *
 * This function is used to delete an entry from SW VLAN_ING_EGR_MEMBER_PORTS
 * _PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_egr_member_profile_entry_sw_delete(int unit, int profile_idx)
{
    bcmint_vlan_membership_info_t *mship = NULL;
    int offset = 0, port_num = 0, port = 0;

    mship = ltsw_vlan_info[unit].mship;
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    offset = port_num * profile_idx;

    for (port = 0; port < port_num; port++) {
        mship[offset + port].port_info &= ~LTSW_VLAN_MSHIP_ING_EGR;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Add an entry into VLAN_EGR_MEMBER_PROFILE table.
 *
 * This function is used to add an entry into VLAN_EGR_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_member_profile_entry_add(int unit,
                                  int profile_idx,
                                  ltsw_vlan_egr_mshp_chk_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Add an entry into VLAN_EGR_MEMBER_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_EGR_MEMBER_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_EGR_MEMBER_PROFILE_IDs, profile_idx));

    /* Set egress member ports applied on membership check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, ent_hdl,
                               VLAN_EGR_MEMBER_PROFILEs,
                               MEMBER_PORTSs,
                               profile->member_ports));

    /* Insert VLAN_EGR_MEMBER_PROFILE entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into SW VLAN_EGR_MEMBER_PROFILE table.
 *
 * This function is used to add an entry into SW VLAN_EGR_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_member_profile_entry_sw_add(int unit,
                                     int profile_idx,
                                     ltsw_vlan_egr_mshp_chk_profile_t *profile)
{
    bcmint_vlan_membership_info_t *mship = NULL;
    int offset = 0, port_num = 0, port = 0;

    mship = ltsw_vlan_info[unit].mship;
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    offset = port_num * profile_idx;

    for (port = 0; port < port_num; port++) {
       if (BCM_PBMP_MEMBER(profile->member_ports, port)) {
           mship[offset + port].port_info |= LTSW_VLAN_MSHIP_EGR;
       } else {
           mship[offset + port].port_info &= ~LTSW_VLAN_MSHIP_EGR;
       }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Update the entry of VLAN_EGR_MEMBER_PROFILE table with a given port.
 *
 * This function is used to update the entry of VLAN_EGR_MEMBER_PROFILE table
 * with a given port
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  port                 Port number.
 * \param [in]  disable              Disable or enable.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_member_profile_entry_port_update(int unit, int profile_idx,
                                          bcm_port_t port, int disable)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmint_vlan_membership_info_t *mship = NULL;
    int port_num = 0, dunit = 0;
    uint64_t value = 0;

    SHR_FUNC_ENTER(unit);

    if (VLAN_SW_MSHIP_EN(unit)) {
        mship = ltsw_vlan_info[unit].mship;
        port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;

        if (disable) {
            /* Set coresponding bit in membersihp. */
            value = 1;
        } else {
            /* Trust SW information set by PORT_ADD APIs. */
            if (mship[port_num * profile_idx + port].port_info &
                LTSW_VLAN_MSHIP_EGR) {
                value = 1;
            } else {
                value = 0;
            }
        }
    } else {
       value = disable ? 1 : 0;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, VLAN_EGR_MEMBER_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_EGR_MEMBER_PROFILE_IDs,
                               profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(ent_hdl, MEMBER_PORTSs, port, &value, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from VLAN_EGR_MEMBER_PROFILE table.
 *
 * This function is used to get an entry from VLAN_EGR_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_member_profile_entry_get(int unit,
                                  int profile_idx,
                                  ltsw_vlan_egr_mshp_chk_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Find a profile entry from VLAN_EGR_MEMBER_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_EGR_MEMBER_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_EGR_MEMBER_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Get egress member ports. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_get(unit, ent_hdl, MEMBER_PORTSs,
                               &(profile->member_ports)));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from SW VLAN_EGR_MEMBER_PROFILE table.
 *
 * This function is used to get an entry from SW VLAN_EGR_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_member_profile_entry_sw_get(int unit,
                                     int profile_idx,
                                     ltsw_vlan_egr_mshp_chk_profile_t *profile)
{
    bcmint_vlan_membership_info_t *mship = NULL;
    int offset = 0, port_num = 0, port = 0;

    mship = ltsw_vlan_info[unit].mship;
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    offset = port_num * profile_idx;

    for (port = 0; port < port_num; port++) {
        if (mship[offset + port].port_info & LTSW_VLAN_MSHIP_EGR) {
            BCM_PBMP_PORT_ADD(profile->member_ports, port);
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Delete an entry from VLAN_EGR_MEMBER_PROFILE table.
 *
 * This function is used to delete an entry from VLAN_EGR_MEMBER_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_member_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_EGR_MEMBER_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_EGR_MEMBER_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from SW VLAN_EGR_MEMBER_PROFILE table.
 *
 * This function is used to delete an entry from SW VLAN_EGR_MEMBER_PROFILE
 * table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_member_profile_entry_sw_delete(int unit, int profile_idx)
{
    bcmint_vlan_membership_info_t *mship = NULL;
    int offset = 0, port_num = 0, port = 0;

    mship = ltsw_vlan_info[unit].mship;
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    offset = port_num * profile_idx;

    for (port = 0; port < port_num; port++) {
        mship[offset + port].port_info &= ~LTSW_VLAN_MSHIP_EGR;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Add an entry into PORT_ING_EGR_BLOCK_1 table.
 *
 * This function is used to add an entry into PORT_ING_EGR_BLOCK_1 table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  block_mask_sel       Block section offset.
 * \param [in]  block_mask           Block mask
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_flood_block_vfi_profile_entry_add(int unit,
                                           int profile_idx,
                                           int block_mask_sel,
                                           ltsw_vlan_block_mask_t *block_mask)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *data = NULL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Add four profile entries into PORT_ING_EGR_BLOCK_1. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              PORT_ING_EGR_BLOCK_1s, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               PORT_ING_EGR_BLOCK_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, PROFILE_SECTIONs, block_mask_sel));

    /* Set block mask ports applied for generic mask. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, ent_hdl,
                               PORT_ING_EGR_BLOCK_1s,
                               MASK_EGR_PORTSs,
                               block_mask->mask_ports));

    /* Set block mask target applied for generic mask. */
    if (block_mask->mask_target == LTSW_VLAN_MASK_TARGET_L2_ONLY) {
        data = L2_MEMBERs;
    } else if (block_mask->mask_target == LTSW_VLAN_MASK_TARGET_L3_ONLY) {
        data = L3_MEMBERs;
    } else {
        data = L2_L3_MEMBERs;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, MASK_TARGETs, data));

    /* Block mask action applied for generic mask. */
    data = block_mask->mask_action ? ORs : ANDs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(ent_hdl, MASK_ACTIONs, data));

    /* Insert profile entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from PORT_ING_EGR_BLOCK_1 table.
 *
 * This function is used to get an entry from PORT_ING_EGR_BLOCK_1 table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  block_mask_sel       Block section offset.
 * \param [out] block_mask           Block mask.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_flood_block_vfi_profile_entry_get(int unit,
                                           int profile_idx,
                                           int  block_mask_sel,
                                           ltsw_vlan_block_mask_t *block_mask)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *data = NULL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Find an entry from PORT_ING_EGR_BLOCK_1. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              PORT_ING_EGR_BLOCK_1s, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               PORT_ING_EGR_BLOCK_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, PROFILE_SECTIONs, block_mask_sel));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Get block mask. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_get(unit, ent_hdl, MASK_EGR_PORTSs,
                               &(block_mask->mask_ports)));

    /* Get block mask target. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(ent_hdl, MASK_TARGETs, &data));
    if (sal_strcmp(L2_MEMBERs, data) == 0) {
        block_mask->mask_target = LTSW_VLAN_MASK_TARGET_L2_ONLY;
    } else if (sal_strcmp(L3_MEMBERs, data) == 0) {
        block_mask->mask_target = LTSW_VLAN_MASK_TARGET_L3_ONLY;
    } else {
        block_mask->mask_target = LTSW_VLAN_MASK_TARGET_L2_AND_L3;
    }

    /* Get mask action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(ent_hdl, MASK_ACTIONs, &data));
    if (sal_strcmp(ORs, data) == 0) {
        block_mask->mask_action = LTSW_VLAN_MASK_ACTION_OR;
    } else {
        block_mask->mask_action = LTSW_VLAN_MASK_ACTION_AND;
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from PORT_ING_EGR_BLOCK_1 table.
 *
 * This function is used to delete an entry from PORT_ING_EGR_BLOCK_1 table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  block_mask_sel       Block section offset.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_flood_block_vfi_profile_entry_delete(int unit,
                                              int profile_idx,
                                              int block_mask_sel)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              PORT_ING_EGR_BLOCK_1s, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                              PORT_ING_EGR_BLOCK_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, PROFILE_SECTIONs, block_mask_sel));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into VLAN_EGR_UNTAG_PROFILE table.
 *
 * This function is used to add an entry into VLAN_EGR_UNTAG_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] profile_idx          Profile entry index.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_entry_add(int unit,
                                 int profile_idx,
                                 ltsw_vlan_egr_untag_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Add an entry into VLAN_EGR_UNTAG_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_EGR_UNTAG_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_EGR_UNTAG_PROFILE_IDs, profile_idx));

    /* Set untag member ports. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_set(unit, ent_hdl,
                               VLAN_EGR_UNTAG_PROFILEs,
                               MEMBER_PORTSs,
                               profile->member_ports));

    /* Insert VLAN_EGR_UNTAG_PROFILE entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from VLAN_EGR_UNTAG_PROFILE table.
 *
 * This function is used to get an entry from VLAN_EGR_UNTAG_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_entry_get(int unit,
                                 int profile_idx,
                                 ltsw_vlan_egr_untag_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    /* Find a profile entry from VLAN_EGR_UNTAG_PROFILE. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_EGR_UNTAG_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_EGR_UNTAG_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Get untag member ports. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_member_ports_get(unit, ent_hdl, MEMBER_PORTSs,
                               &(profile->member_ports)));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from VLAN_EGR_UNTAG_PROFILE table.
 *
 * This function is used to delete an entry from VLAN_EGR_UNTAG_PROFILE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] profile_idx           Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_entry_delete(int unit, int profile_idx)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              VLAN_EGR_UNTAG_PROFILEs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl,
                               VLAN_EGR_UNTAG_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into ingress membership check profile.
 *
 * This function is used to add an entrie into ingress membership check profile
 * (Related LTs -VLAN_ING_MEMBER_PROFILE and VLAN_ING_EGR_MEMBER_PORTS_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              Profile entry.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_mshp_chk_profile_add(int unit,
                              ltsw_vlan_ing_mshp_chk_profile_t *profile,
                              int *profile_idx)
{
    int entries_per_set = 1;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    VLAN_PROFILE_LOCK(unit);

    /* Try to get a free profile index. */
    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          BCMI_LTSW_PROFILE_ING_MSHP_CHK,
                                          profile, 0,
                                          entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        if (*profile_idx == VLAN_PROFILE_ING_MSHP_CHK_RESV0 ||
            *profile_idx == VLAN_PROFILE_ING_MSHP_CHK_RESV1) {
            /* Count rollback since ref_count of default entry is always 1. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_decrease(
                    unit, BCMI_LTSW_PROFILE_ING_MSHP_CHK,
                    entries_per_set, *profile_idx, 1));
        }
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Add the information into VLAN_ING_MEMBER_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_member_profile_entry_add(unit, *profile_idx, profile));

    /* Add the information into VLAN_ING_EGR_MEMBER_PORTS_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_egr_member_profile_entry_add(unit, *profile_idx, profile));

    /* Add membership into SW DB. */
    if(VLAN_SW_MSHIP_EN(unit)) {
        /* Add the information into SW VLAN_ING_MEMBER_PROFILE. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_member_profile_entry_sw_add(unit,
                                                  *profile_idx, profile));
        /* Add the information into SW VLAN_ING_EGR_MEMBER_PORTS_PROFILE. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_egr_member_profile_entry_sw_add(unit,
                                                      *profile_idx, profile));
    }

exit:
    VLAN_PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from ingress membership check profile.
 *
 * This function is used to get entries from ingress membership check profile.
 * (Related LTs -VLAN_ING_MEMBER_PROFILE and VLAN_ING_EGR_MEMBER_PORTS_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 * \param [in]  lock                 Need profile lock.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_mshp_chk_profile_get(int unit,
                              int profile_idx,
                              ltsw_vlan_ing_mshp_chk_profile_t *profile,
                              int lock)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(profile, 0, sizeof(ltsw_vlan_ing_mshp_chk_profile_t));
    if (lock) {
        VLAN_PROFILE_LOCK(unit);
    }

    if(VLAN_SW_MSHIP_EN(unit)) {
        /* Get the information from SW VLAN_ING_MEMBER_PROFILE. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_member_profile_entry_sw_get(unit,
                                                  profile_idx, profile));

        /* Get the information from SW VLAN_ING_EGR_MEMBER_PORTS_PROFILE. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_egr_member_profile_entry_sw_get(unit,
                                                      profile_idx, profile));
    } else {
        /* Get the information from VLAN_ING_MEMBER_PROFILE table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_member_profile_entry_get(unit, profile_idx, profile));

        /* Get the information from VLAN_ING_EGR_MEMBER_PORTS_PROFILE table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_egr_member_profile_entry_get(unit, profile_idx, profile));
    }

exit:
    if (lock) {
        VLAN_PROFILE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete entries from ingress membership check profile.
 *
 * This function is used to delete entries from ingress membership check
 * profile (Related LTs - VLAN_ING_MEMBER_PROFILE and
 * VLAN_ING_EGR_MEMBER_PORTS_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_mshp_chk_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    VLAN_PROFILE_LOCK(unit);

    /* No ref_count decrease for default entry. */
    if (profile_idx == VLAN_PROFILE_ING_MSHP_CHK_RESV0 ||
        profile_idx == VLAN_PROFILE_ING_MSHP_CHK_RESV1) {
        SHR_EXIT();
    }

    rv = bcmi_ltsw_profile_index_free(unit,
                                      BCMI_LTSW_PROFILE_ING_MSHP_CHK,
                                      profile_idx);

    /* Return directly if profile index is still used by others. */
    if (rv == SHR_E_BUSY) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Delete the information from VLAN_ING_MEMBER_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_member_profile_entry_delete(unit, profile_idx));

    /* Delete the information from VLAN_ING_EGR_MEMBER_PORTS_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_egr_member_profile_entry_delete(unit, profile_idx));

    if(VLAN_SW_MSHIP_EN(unit)) {
        /* Delete the information from SW VLAN_ING_MEMBER_PROFILE. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_member_profile_entry_sw_delete(unit, profile_idx));

        /* Delete the information from SW VLAN_ING_EGR_MEMBER_PORTS_PROFILE. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_egr_member_profile_entry_sw_delete(unit, profile_idx));
    }

exit:
    VLAN_PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear all entries in ingress membership check profile.
 *
 * This function is used to clear all entries in ingress membership check
 * profile (Related LTs - VLAN_ING_MEMBER_PROFILE and
 * VLAN_ING_EGR_MEMBER_PORTS_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_mshp_chk_profile_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear all entries in VLAN_ING_MEMBER_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_ING_MEMBER_PROFILEs));

    /* Clear all entries in VLAN_ING_EGR_MEMBER_PORTS_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_ING_EGR_MEMBER_PORTS_PROFILEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash signature for ingress membership check profile.
 *
 * This function is used to calculate hash signature for ingress membership
 * check profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              Ingress membership check profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_mshp_chk_profile_hash_cb(int unit,
                                  void *entries,
                                  int entries_per_set,
                                  uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set *
                      sizeof(ltsw_vlan_ing_mshp_chk_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of ingress membership check profile.
 *
 * This function is used to compare profile set of ingress membership check
 * profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              Ingress membership check profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_mshp_chk_profile_cmp_cb(int unit, void *entries,
                                 int entries_per_set,
                                 int index, int *cmp)
{
    ltsw_vlan_ing_mshp_chk_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_get(unit, index, &profile, 0));

    *cmp = sal_memcmp(entries,
                      (void *)&profile,
                      sizeof(ltsw_vlan_ing_mshp_chk_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register ingress membership check profile.
 *
 * This function is used to register ingress membership check profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_mshp_chk_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_ING_MSHP_CHK;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    /* Get range of profile index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_ING_MEMBER_PROFILEs,
                                       VLAN_ING_MEMBER_PROFILE_IDs,
                                       &idx_min, &idx_max));
    /* Register profile. */
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_ing_mshp_chk_profile_hash_cb,
                                    vlan_ing_mshp_chk_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of ingress membership check profile.
 *
 * This function is used to recover hash and reference count of ingress
 * membership check profile.
 *
 * \param [in] unit                  Unit Number.
 * \param [in] index                 Entry index of profile table.
 * \param [in] l2mc_idx              Entry index of l2mc table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_mshp_chk_profile_recover(int unit, int index, int l2mc_idx)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_ING_MSHP_CHK;
    ltsw_vlan_ing_mshp_chk_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;
    bcm_pbmp_t *pbmp = NULL, pbmp_lb;
    bcm_pbmp_t pbmp_cmic;

    SHR_FUNC_ENTER(unit);

    sal_memset(&profile, 0, sizeof(ltsw_vlan_ing_mshp_chk_profile_t));

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, index, &ref_count));

    if (ref_count == 0) {

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_mshp_chk_profile_get(unit, index, &profile, 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, profile_hdl, &profile, entries_per_set, index));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));

    if (VLAN_DLF_FLOOD(unit) && l2mc_idx) {
        if (BCM_PBMP_NOT_NULL(profile.member_ports) &&
            BCM_PBMP_NOT_NULL(profile.egr_member_ports)) {
            SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbmp_lb));
            BCM_PBMP_REMOVE(profile.egr_member_ports, pbmp_lb);

            if (VLAN_DLF_FLOOD_MODE(unit) == LTSW_VLAN_FLOOD_MODE_EXCLUDE_CPU) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_bitmap(unit,
                                           BCMI_LTSW_PORT_TYPE_CPU,
                                           &pbmp_cmic));
                BCM_PBMP_REMOVE(profile.egr_member_ports, pbmp_cmic);
            }
            pbmp = &(profile.egr_member_ports);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_vlan_flood_grp_recover(unit, pbmp, l2mc_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the information of ingress membership check profile.
 *
 * This function is used to initialize the information of ingress membership
 * check profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_mshp_chk_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_ING_MSHP_CHK;
    ltsw_vlan_ing_mshp_chk_profile_t profile;
    int max_port_num = 0;
    int profile_idx = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_init_state_get(unit)) {
        /* Try to clear all profile info during bcm init. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_mshp_chk_profile_clear(unit));
        if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_unregister(unit, profile_hdl));
        }
    } else {
        /*
         * VLAN-specific init:
         * Profile info has been cleared per entry - vlan_table_clear().
         * Don't unregister it since profile may still be used by others.
         */
         SHR_EXIT();
    }

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_register(unit));

    /*
     * Reserve entry 0 - all ports in egress block mask for routed pkts. Since
     * ipost membership/spanning tree checks should not happen for routed
     * packet, these checks should be based on the outgoing vfi on the egress
     * pipe. - all ports in membership for tunnel decap scenario. If MY_STATION
     * table is not hit, entry 0 of ING0_MEMBERSHIP will be used by default.
     */
    sal_memset(&profile, 0, sizeof(ltsw_vlan_ing_mshp_chk_profile_t));
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i++) {
        BCM_PBMP_PORT_ADD(profile.egr_member_ports, i);
        BCM_PBMP_PORT_ADD(profile.member_ports, i);
    }

    profile.egr_mask_target = LTSW_VLAN_MASK_TARGET_L2_ONLY;
    profile.egr_mask_action = LTSW_VLAN_MASK_ACTION_AND;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_add(unit, &profile, &profile_idx));

    /* Reserved profile entry with all ports should start with index 0. */
    if (profile_idx != VLAN_PROFILE_ING_MSHP_CHK_RESV0) {
        (void)vlan_ing_mshp_chk_profile_delete(unit, profile_idx);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    /* Insert a profile entry only with internal LB ports. */
    sal_memset(&profile, 0, sizeof(ltsw_vlan_ing_mshp_chk_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit,
                               BCMI_LTSW_PORT_TYPE_LB, &(profile.member_ports)));

    BCM_PBMP_ASSIGN(profile.egr_member_ports, profile.member_ports);

    profile.egr_mask_target = LTSW_VLAN_MASK_TARGET_L2_ONLY;
    profile.egr_mask_action = LTSW_VLAN_MASK_ACTION_AND;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_add(unit, &profile, &profile_idx));

    /* Reserved profile entry 1 only with internal LB ports. */
    if (profile_idx != VLAN_PROFILE_ING_MSHP_CHK_RESV1) {
        (void)vlan_ing_mshp_chk_profile_delete(unit, profile_idx);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into egress membership check profile.
 *
 * This function is used to add an entry into egress membership check profile
 * (Related LTs - VLAN_EGR_MEMBER_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              Profile entry.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_mshp_chk_profile_add(int unit,
                              ltsw_vlan_egr_mshp_chk_profile_t *profile,
                              int *profile_idx)
{
    int entries_per_set = 1;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    VLAN_PROFILE_LOCK(unit);

    /* Try to get a free profile index. */
    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          BCMI_LTSW_PROFILE_EGR_MSHP_CHK,
                                          profile, 0,
                                          entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        if (*profile_idx == VLAN_PROFILE_EGR_MSHP_CHK_RESV0 ||
            *profile_idx == VLAN_PROFILE_EGR_MSHP_CHK_RESV1) {
            /* Count rollback since ref_count of default entry is always 1. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_decrease(
                    unit, BCMI_LTSW_PROFILE_EGR_MSHP_CHK,
                    entries_per_set, *profile_idx, 1));
        }
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Add the information into VLAN_EGR_MEMBER_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_member_profile_entry_add(unit, *profile_idx, profile));

    /* Add membership into SW DB. */
    if(VLAN_SW_MSHIP_EN(unit)) {
        /* Add the information into SW VLAN_EGR_MEMBER_PROFILE table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_member_profile_entry_sw_add(unit, *profile_idx, profile));
    }

exit:
    VLAN_PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from egress membership check profile.
 *
 * This function is used to get an entry from egress membership check profile
 * (Related LTs - VLAN_EGR_MEMBER_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 * \param [in]  lock                 Need profile lock.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_mshp_chk_profile_get(int unit,
                              int profile_idx,
                              ltsw_vlan_egr_mshp_chk_profile_t *profile,
                              int lock)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(profile, 0, sizeof(ltsw_vlan_egr_mshp_chk_profile_t));

    if (lock) {
        VLAN_PROFILE_LOCK(unit);
    }

    if(VLAN_SW_MSHIP_EN(unit)) {
        /* Get the information from SW VLAN_EGR_MEMBER_PROFILE table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_member_profile_entry_sw_get(unit, profile_idx, profile));
    } else {
        /* Get the information from VLAN_EGR_MEMBER_PROFILE table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_member_profile_entry_get(unit, profile_idx, profile));
    }

exit:
    if (lock) {
        VLAN_PROFILE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from egress membership check profile.
 *
 * This function is used to delete an entry from egress membership check profile
 * (Related LTs - VLAN_EGR_MEMBER_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_mshp_chk_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    VLAN_PROFILE_LOCK(unit);

    /* No ref_count decrease for default entry. */
    if (profile_idx == VLAN_PROFILE_EGR_MSHP_CHK_RESV0 ||
        profile_idx == VLAN_PROFILE_EGR_MSHP_CHK_RESV1) {
        SHR_EXIT();
    }

    rv = bcmi_ltsw_profile_index_free(unit,
                                      BCMI_LTSW_PROFILE_EGR_MSHP_CHK,
                                      profile_idx);

    /* Return directly if profile index is still used by others. */
    if (rv == SHR_E_BUSY) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Delete the information from VLAN_EGR_MEMBER_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_member_profile_entry_delete(unit, profile_idx));

    if(VLAN_SW_MSHIP_EN(unit)) {
        /* Delete the information from SW VLAN_EGR_MEMBER_PROFILE table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_member_profile_entry_sw_delete(unit, profile_idx));
    }

exit:
    VLAN_PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear all entries in egress membership check profile.
 *
 * This function is used to clear all entries in egress membership check profile
 * (Related LTs - VLAN_EGR_MEMBER_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_mshp_chk_profile_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Delete all entries in VLAN_EGR_MEMBER_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_EGR_MEMBER_PROFILEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash signature for egress membership check profile.
 *
 * This function is used to calculate hash signature for egress membership check
 * profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              egress membership check profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_mshp_chk_profile_hash_cb(int unit, void *entries,
                                  int entries_per_set,
                                  uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set *
                      sizeof(ltsw_vlan_egr_mshp_chk_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of egress membership check profile.
 *
 * This function is used to compare profile set of egress membership check
 * profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              Egress membership check profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_mshp_chk_profile_cmp_cb(int unit, void *entries,
                                 int entries_per_set,
                                 int index, int *cmp)
{
    ltsw_vlan_egr_mshp_chk_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_get(unit, index, &profile, 0));

    *cmp = sal_memcmp(entries,
                      (void *)&profile,
                      sizeof(ltsw_vlan_egr_mshp_chk_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register egress membership check profile.
 *
 * This function is used to register egress membership check profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_mshp_chk_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_EGR_MSHP_CHK;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    /* Get range of profile index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_EGR_MEMBER_PROFILEs,
                                       VLAN_EGR_MEMBER_PROFILE_IDs,
                                       &idx_min, &idx_max));
    /* Register profile. */
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_egr_mshp_chk_profile_hash_cb,
                                    vlan_egr_mshp_chk_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of egress membership check profile.
 *
 * This function is used to recover hash and reference count of egress
 * membership check profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_mshp_chk_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_EGR_MSHP_CHK;
    ltsw_vlan_egr_mshp_chk_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Update profile hash on the first usage. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, index, &ref_count));

    if (ref_count == 0) {

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_mshp_chk_profile_get(unit, index, &profile, 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, profile_hdl, &profile, entries_per_set, index));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the information of egress membership check profile.
 *
 * This function is used to initialize the information of egress membership
 * check profile(Related LTs - VLAN_EGR_MEMBER_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_mshp_chk_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_EGR_MSHP_CHK;
    ltsw_vlan_egr_mshp_chk_profile_t profile;
    int max_port_num = 0;
    int profile_idx = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_init_state_get(unit)) {
        /* Try to clear all profile info during bcm init. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_mshp_chk_profile_clear(unit));
        if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_unregister(unit, profile_hdl));
        }
    } else {
        /*
         * VLAN-specific init:
         * Profile info has been cleared per entry - vlan_table_clear().
         * Don't unregister it since profile may still be used by others.
         */
         SHR_EXIT();
    }

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_register(unit));

    /*
     * Reserve entry 0 with all ports bitmap to be consistent with ingress
     * direction.
     */
    sal_memset(&profile, 0, sizeof(ltsw_vlan_egr_mshp_chk_profile_t));
    max_port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    for (i = 0; i < max_port_num; i++) {
        BCM_PBMP_PORT_ADD(profile.member_ports, i);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_add(unit, &profile, &profile_idx));

    if (profile_idx != VLAN_PROFILE_EGR_MSHP_CHK_RESV0) {
        (void)vlan_egr_mshp_chk_profile_delete(unit, profile_idx);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    /* Reserved profile entry 1 only with internal LB ports. */
    sal_memset(&profile, 0, sizeof(ltsw_vlan_egr_mshp_chk_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit,
                               BCMI_LTSW_PORT_TYPE_LB, &(profile.member_ports)));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_add(unit, &profile, &profile_idx));

    if (profile_idx != VLAN_PROFILE_EGR_MSHP_CHK_RESV1) {
        (void)vlan_egr_mshp_chk_profile_delete(unit, profile_idx);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into block mask profile per VFI.
 *
 * This function is used to add an entry into block mask profile per VFI
 * (Related LTs - PORT_ING_EGR_BLOCK_1).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              Profile entry.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_vfi_profile_add(
    int unit,
    ltsw_vlan_ing_block_mask_vfi_profile_t *profile,
    int *profile_idx)
{
    int entries_per_set = 1;
    int rv = SHR_E_NONE;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    VLAN_PROFILE_LOCK(unit);

    /* Try to get a free profile index. */
    rv = bcmi_ltsw_profile_index_allocate(
             unit,
             BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_0,
             profile, 0, entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly. */
    if (rv == SHR_E_EXISTS) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Add the uc/mc/umc/uuc/bc information into PORT_ING_EGR_BLOCK_1 table. */
    for (i = 0; i < LTSW_VLAN_MASK_SECT_CNT; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_flood_block_vfi_profile_entry_add(
                unit, *profile_idx, i, &(profile->mask[i])));
    }

exit:
    VLAN_PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from block mask profile per VFI.
 *
 * This function is used to get an entry from block mask profile per VFI
 * (Related LTs - PORT_ING_EGR_BLOCK_1).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 * \param [in]  lock                 Need profile lock.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_vfi_profile_get(
    int unit,
    int profile_idx,
    ltsw_vlan_ing_block_mask_vfi_profile_t *profile,
    int lock)
{
    int i = 0;
    SHR_FUNC_ENTER(unit);

    sal_memset(profile, 0, sizeof(ltsw_vlan_ing_block_mask_vfi_profile_t));
    if (lock) {
        VLAN_PROFILE_LOCK(unit);
    }

    /* Get the uc/mc/umc/uuc/bc information from PORT_ING_EGR_BLOCK_1 table. */
    for (i = 0; i < LTSW_VLAN_MASK_SECT_CNT; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_flood_block_vfi_profile_entry_get(
                 unit, profile_idx, i, &(profile->mask[i])));
    }

exit:
    if (lock) {
        VLAN_PROFILE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from block mask profile per VFI.
 *
 * This function is used to delete an entry from block mask profile per VFI
 * (Related LTs - PORT_ING_EGR_BLOCK_1).
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_vfi_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    VLAN_PROFILE_LOCK(unit);

    /* No ref_count decrease for default entry. */
    if (profile_idx == VLAN_PROFILE_ING_BLOCK_MASK_RESV0) {
        SHR_EXIT();
    }

    rv = bcmi_ltsw_profile_index_free(unit,
                                      BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_0,
                                      profile_idx);

    /* Return directly if profile index is still used by others. */
    if (rv == SHR_E_BUSY) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Delete uc/mc/umc/uuc/bc information from PORT_ING_EGR_BLOCK_1 table. */
    for (i = 0; i < LTSW_VLAN_MASK_SECT_CNT; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_flood_block_vfi_profile_entry_delete(unit,
                                                           profile_idx, i));
    }

exit:
    VLAN_PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear all entries in block mask profile per VFI.
 *
 * This function is used to clear all entries in block mask profile per VFI
 * (Related LTs - PORT_ING_EGR_BLOCK_1).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_vfi_profile_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Delete all entries in PORT_ING_EGR_BLOCK_1 table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, PORT_ING_EGR_BLOCK_1s));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash signature for block mask profile per VFI.
 *
 * This function is used to calculate hash signature for block mask profile
 * per VFI.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              Ingress flood block profile for VFI.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_vfi_profile_hash_cb(int unit, void *entries,
                                        int entries_per_set,
                                        uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set *
                      sizeof(ltsw_vlan_ing_block_mask_vfi_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of block mask profile per VFI.
 *
 * This function is used to compare profile set of block mask profile per VFI.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              Ingress flood block profile per VFI.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_vfi_profile_cmp_cb(int unit, void *entries,
                                       int entries_per_set,
                                       int index, int *cmp)
{
    ltsw_vlan_ing_block_mask_vfi_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_block_mask_vfi_profile_get(unit, index, &profile, 0));

    *cmp = sal_memcmp(entries,
                      (void *)&profile,
                      sizeof(ltsw_vlan_ing_block_mask_vfi_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register block mask profile per VFI.
 *
 * This function is used to register block mask profile per VFI.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_vfi_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl[] =
        {BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_0,
         BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_1,
         BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_2,
         BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_3};
    int idx_min[] = {0, 0, 0, 0};
    int idx_max[] = {0, 0, 0, 0};
    uint64_t value_min = 0;
    uint64_t value_max = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    /* Get range of profile index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, PORT_ING_EGR_BLOCK_1s,
                                       PORT_ING_EGR_BLOCK_IDs,
                                       &value_min, &value_max));

    /* Reserve entry 0 for none-block usage. */
    for (i = 0; i < COUNTOF(idx_min); i++) {
        idx_min[i] = (int)value_min + 1;
        idx_max[i] = (int)value_max;
    }

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, profile_hdl, idx_min, idx_max,
                                    COUNTOF(idx_min),
                                    vlan_ing_block_mask_vfi_profile_hash_cb,
                                    vlan_ing_block_mask_vfi_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of block mask profile per VFI.
 *
 * This function is used to recover hash and reference count of block mask
 * profile per VFI.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_vfi_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl =
        BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_0;
    ltsw_vlan_ing_block_mask_vfi_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, index, &ref_count));

    if (ref_count == 0) {

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_block_mask_vfi_profile_get(unit, index, &profile, 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, profile_hdl, &profile, entries_per_set, index));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the information of block mask profile per VFI.
 *
 * This function is used to initialize the information of block mask profile
 * per VFI(Related LTs - PORT_ING_EGR_BLOCK_1).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_vfi_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl =
        BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_0;

    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_init_state_get(unit)) {
        /* Try to clear all profile info during bcm init. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_block_mask_vfi_profile_clear(unit));
        if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_unregister(unit, profile_hdl));
        }
    } else {
        /*
         * VLAN-specific init:
         * Profile info has been cleared per entry - vlan_table_clear().
         * Don't unregister it since profile may still be used by others.
         */
        SHR_EXIT();
    }

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_block_mask_vfi_profile_register(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the information of block mask.
 *
 * This function is used to initialize the information of block mask.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_init(int unit)
{
    int rv;
    const char *tbl_desc;
    ltsw_vlan_block_mask_ctrl_t *block_mask_ctrl =
        &(ltsw_vlan_info[unit].block_mask_ctrl);
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t value = 0;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    
    rv = bcmlt_table_desc_get(dunit,
                              R_BLOCK_MASK_SECTION_CONTROLs, &tbl_desc);

    if (SHR_FAILURE(rv)) {
        block_mask_ctrl->uc_mask_sel  = 0;
        block_mask_ctrl->mc_mask_sel  = 0;
        block_mask_ctrl->umc_mask_sel = 1;
        block_mask_ctrl->uuc_mask_sel = 2;
        block_mask_ctrl->bc_mask_sel  = 3;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, R_BLOCK_MASK_SECTION_CONTROLs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit,
                              ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, BCAST_SECTION_SELs, &value));
    block_mask_ctrl->bc_mask_sel = (int)value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, KNOWN_MCAST_SECTION_SELs, &value));
    block_mask_ctrl->mc_mask_sel = (int)value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, KNOWN_UCAST_SECTION_SELs, &value));
    block_mask_ctrl->uc_mask_sel = (int)value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, UNKNOWN_MCAST_SECTION_SELs, &value));
    block_mask_ctrl->umc_mask_sel = (int)value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, UNKNOWN_UCAST_SECTION_SELs, &value));
    block_mask_ctrl->uuc_mask_sel = (int)value;

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the information of block.
 *
 * This function is used to validate the information of block.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  block                VLAN specific traffic blocking structure.

 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_block_mask_validate(int unit, bcm_vlan_block_t *block)
{
    const char *tbl_desc;
    int dunit = 0;
    int rv;
    ltsw_vlan_block_mask_ctrl_t *block_mask_ctrl =
        &(ltsw_vlan_info[unit].block_mask_ctrl);
    bcm_pbmp_t pbm_1, pbm_2;
    int pbm_1_null = TRUE, pbm_2_null = TRUE;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    
    rv = bcmlt_table_desc_get(dunit,
                              R_BLOCK_MASK_SECTION_CONTROLs, &tbl_desc);

    if (SHR_FAILURE(rv)) {
        SHR_EXIT();
    }

    /* Setction 0 is used for shared pbmp. */
    if (block_mask_ctrl->uc_mask_sel == 0) {
        if (pbm_1_null) {
            BCM_PBMP_ASSIGN(pbm_1, block->unicast);
            pbm_1_null = FALSE;
        } else if (pbm_2_null) {
            BCM_PBMP_ASSIGN(pbm_2, block->unicast);
            pbm_2_null = FALSE;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (block_mask_ctrl->mc_mask_sel == 0) {
        if (pbm_1_null) {
            BCM_PBMP_ASSIGN(pbm_1, block->known_multicast);
            pbm_1_null = FALSE;
        } else if (pbm_2_null) {
            BCM_PBMP_ASSIGN(pbm_2, block->known_multicast);
            pbm_2_null = FALSE;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (block_mask_ctrl->umc_mask_sel == 0) {
        if (pbm_1_null) {
            BCM_PBMP_ASSIGN(pbm_1, block->unknown_multicast);
            pbm_1_null = FALSE;
        } else if (pbm_2_null) {
            BCM_PBMP_ASSIGN(pbm_2, block->unknown_multicast);
            pbm_2_null = FALSE;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (block_mask_ctrl->uuc_mask_sel == 0) {
        if (pbm_1_null) {
            BCM_PBMP_ASSIGN(pbm_1, block->unknown_unicast);
            pbm_1_null = FALSE;
        } else if (pbm_2_null) {
            BCM_PBMP_ASSIGN(pbm_2, block->unknown_unicast);
            pbm_2_null = FALSE;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (block_mask_ctrl->bc_mask_sel == 0) {
        if (pbm_1_null) {
            BCM_PBMP_ASSIGN(pbm_1, block->broadcast);
            pbm_1_null = FALSE;
        } else if (pbm_2_null) {
            BCM_PBMP_ASSIGN(pbm_2, block->broadcast);
            pbm_2_null = FALSE;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (BCM_PBMP_NEQ(pbm_1, pbm_2)){
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into egress untag profile.
 *
 * This function is used to add an entry into egress untag profile(Related LT -
 * VLAN_EGR_UNTAG_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile              Profile entry.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_add(int unit,
                           ltsw_vlan_egr_untag_profile_t *profile,
                           int *profile_idx)
{
    int entries_per_set = 1;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    VLAN_PROFILE_LOCK(unit);

    /* Try to get a free profile index. */
    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          BCMI_LTSW_PROFILE_EGR_UNTAG,
                                          profile, 0,
                                          entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly */
    if (rv == SHR_E_EXISTS) {
        if (*profile_idx == VLAN_PROFILE_EGR_UNTAG_RESV0) {
            /* Count rollback since ref_count of default entry is always 1. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_decrease(
                    unit, BCMI_LTSW_PROFILE_EGR_UNTAG,
                    entries_per_set, *profile_idx, 1));
        }
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Add the information into VLAN_EGR_UNTAG_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_entry_add(unit, *profile_idx, profile));

exit:
    VLAN_PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from egress untag profile.
 *
 * This function is used to get an entry from egress untag profile(Related LT -
 * VLAN_EGR_UNTAG_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [out] profile              Profile entry.
 * \param [in]  lock                 Need profile lock.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_get(int unit,
                           int profile_idx,
                           ltsw_vlan_egr_untag_profile_t *profile,
                           int lock)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(profile, 0, sizeof(ltsw_vlan_egr_untag_profile_t));
    if (lock) {
        VLAN_PROFILE_LOCK(unit);
    }
    /* Get the information from VLAN_EGR_MEMBER_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_entry_get(unit, profile_idx, profile));

exit:
    if (lock) {
        VLAN_PROFILE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from egress untag profile.
 *
 * This function is used to delete an entry from egress untag profile(Related
 * LTs - VLAN_EGR_UNTAG_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    VLAN_PROFILE_LOCK(unit);

    /* No ref_count decrease for default entry. */
    if (profile_idx == VLAN_PROFILE_EGR_UNTAG_RESV0) {
        SHR_EXIT();
    }

    rv = bcmi_ltsw_profile_index_free(unit,
                                      BCMI_LTSW_PROFILE_EGR_UNTAG,
                                      profile_idx);

    /* Return directly if profile index is still used by others. */
    if (rv == SHR_E_BUSY) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Delete the information from VLAN_EGR_UNTAG_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_entry_delete(unit, profile_idx));

exit:
    VLAN_PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear all entries in egress untag profile.
 *
 * This function is used to clear all entries in inegress untag profile(Related
 * LTs - VLAN_EGR_UNTAG_PROFILE).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear all entries in VLAN_EGR_UNTAG_PROFILE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_EGR_UNTAG_PROFILEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash signature for egress untag profile.
 *
 * This function is used to calculate hash signature for egress untag profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              Egress untag profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_hash_cb(int unit, void *entries,
                               int entries_per_set,
                               uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set *
                      sizeof(ltsw_vlan_egr_untag_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of egress untag profile.
 *
 * This function is used to compare profile set of egress untag profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              Egress untag profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [in]  index                Entry index of profile table.
 * \param [out] cmp                  Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_cmp_cb(int unit, void *entries,
                              int entries_per_set,
                              int index, int *cmp)
{
    ltsw_vlan_egr_untag_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_get(unit, index, &profile, 0));

    *cmp = sal_memcmp(entries,
                      (void *)&profile,
                      sizeof(ltsw_vlan_egr_untag_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register egress untag profile.
 *
 * This function is used to register egress untag profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_EGR_UNTAG;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    /* Get the range of profile index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_EGR_UNTAG_PROFILEs,
                                       VLAN_EGR_UNTAG_PROFILE_IDs,
                                       &idx_min, &idx_max));
    /* Register profile. */
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_egr_untag_profile_hash_cb,
                                    vlan_egr_untag_profile_cmp_cb));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of egress untag profile.
 *
 * This function is used to Recover hash and reference count of egress untag
 * profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_EGR_UNTAG;
    ltsw_vlan_egr_untag_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, index, &ref_count));

    if (ref_count == 0) {

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_untag_profile_get(unit, index, &profile, 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, profile_hdl, &profile, entries_per_set, index));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the information of egress untag profile.
 *
 * This function is used to initialize the information of egress untag profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_untag_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_EGR_UNTAG;
    ltsw_vlan_egr_untag_profile_t profile;
    int profile_idx = 0;

    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_init_state_get(unit)) {
        /* Try to clear all profile info during bcm init. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_untag_profile_clear(unit));
        if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_unregister(unit, profile_hdl));
        }
    } else {
        /*
         * VLAN-specific init:
         * Profile info has been cleared per entry - vlan_table_clear().
         * Don't unregister it since profile may still be used by others.
         */
        SHR_EXIT();

    }

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_register(unit));

    /* Reserve entry 0 with empty port bitmap. */
    sal_memset(&profile, 0, sizeof(ltsw_vlan_egr_untag_profile_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_add(unit, &profile, &profile_idx));

    if (profile_idx != VLAN_PROFILE_EGR_UNTAG_RESV0) {
        (void)vlan_egr_untag_profile_delete(unit, profile_idx);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into VLAN_ASSIGNMENT_INNER/OUTER_VLAN_RANGE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  inner                Inner map or outer map.
 * \param [in]  profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_entry_add(int unit,
                                 int profile_idx, int inner,
                                 ltsw_vlan_range_map_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t *min_value, *max_value;
    const char *tbl_name[] = {VLAN_ASSIGNMENT_OUTER_VLAN_RANGEs,
                              VLAN_ASSIGNMENT_INNER_VLAN_RANGEs};
    const char *fld_name[] = {VLAN_ASSIGNMENT_OUTER_VLAN_RANGE_IDs,
                              VLAN_ASSIGNMENT_INNER_VLAN_RANGE_IDs};
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbl_name[inner], &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, fld_name[inner], profile_idx));

    if (inner) {
        min_value = profile->ivid_min;
        max_value = profile->ivid_max;
    } else {
        min_value = profile->ovid_min;
        max_value = profile->ovid_max;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(ent_hdl, VLAN_ID_MINs, 0,
                                     min_value, 8));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(ent_hdl, VLAN_ID_MAXs, 0,
                                     max_value, 8));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from VLAN_ASSIGNMENT_INNER/OUTER_VLAN_RANGE table.
 *
 * This function is used to get an entry from
 * VLAN_ASSIGNMENT_INNER/OUTER_VLAN_RANGE table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  profile_idx          Profile entry index.
 * \param [in]  inner                Inner map or outer map.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_entry_get(int unit,
                                 int profile_idx, int inner,
                                 ltsw_vlan_range_map_profile_t *profile)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t *min_value, *max_value;
    uint32_t r_elem_cnt;
    const char *tbl_name[] = {VLAN_ASSIGNMENT_OUTER_VLAN_RANGEs,
                              VLAN_ASSIGNMENT_INNER_VLAN_RANGEs};
    const char *fld_name[] = {VLAN_ASSIGNMENT_OUTER_VLAN_RANGE_IDs,
                              VLAN_ASSIGNMENT_INNER_VLAN_RANGE_IDs};
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbl_name[inner], &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, fld_name[inner], profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    if (inner) {
        min_value = profile->ivid_min;
        max_value = profile->ivid_max;
    } else {
        min_value = profile->ovid_min;
        max_value = profile->ovid_max;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(ent_hdl, VLAN_ID_MINs, 0,
                                     min_value, 8, &r_elem_cnt));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_get(ent_hdl, VLAN_ID_MAXs, 0,
                                     max_value, 8, &r_elem_cnt));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from VLAN_ASSIGNMENT_INNER/OUTER_VLAN_RANGE table.
 *
 * \param [in]  unit               Unit Number.
  *\param [in]  inner              Inner map or outer map.
 * \param [in]  profile_idx        Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_entry_delete(int unit, int inner, int profile_idx)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *tbl_name[] = {VLAN_ASSIGNMENT_OUTER_VLAN_RANGEs,
                              VLAN_ASSIGNMENT_INNER_VLAN_RANGEs};
    const char *fld_name[] = {VLAN_ASSIGNMENT_OUTER_VLAN_RANGE_IDs,
                              VLAN_ASSIGNMENT_INNER_VLAN_RANGE_IDs};
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbl_name[inner], &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, fld_name[inner], profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into VLAN range map profile.
 *
 * This function is used to add an entry into VLAN range map profile(Related LT -
 * VLAN_ASSIGNMENT_INNER/OUTER_VLAN_RANGE).
 *
 * \param [in]   unit             Unit Number.
 * \param [in]   profile          Profile entry.
 * \param [out]  profile_idx      Profile entry index.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_add(int unit,
                           ltsw_vlan_range_map_profile_t *profile,
                           int *profile_idx)
{
    int entries_per_set = 1;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Try to get a free profile index. */
    rv = bcmi_ltsw_profile_index_allocate(unit,
                                          BCMI_LTSW_PROFILE_VLAN_RANGE_MAP,
                                          profile, 0,
                                          entries_per_set, profile_idx);

    /* Get an existing entry and return the profile index directly. */
    if (rv == SHR_E_EXISTS) {
        if (*profile_idx == VLAN_PROFILE_RANGE_MAP_RESV0) {
            /* Count rollback since ref_count of default entry is always 1. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_decrease(
                    unit, BCMI_LTSW_PROFILE_VLAN_RANGE_MAP,
                    entries_per_set, *profile_idx, 1));
        }
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Add the information into VLAN_ASSIGNMENT_INNER/OUTER_VLAN_RANGE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_add(unit, *profile_idx, 1, profile));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_add(unit, *profile_idx, 0, profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from vlan range map profile.
 *
 * This function is used to delete an entry from vlan range map profile(Related
 * LTs - VLAN_ASSIGNMENT_INNER/OUTER_VLAN_RANGE)).
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] profile_idx          Profile entry index.
 *
 * \retval SHR_E_NONE               No errors.
 * \retval !SHR_E_NONE              Failure.
 */
static int
vlan_range_map_profile_delete(int unit, int profile_idx)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* No reference count decrease for default index. */
    if (profile_idx == VLAN_PROFILE_RANGE_MAP_RESV0) {
        SHR_EXIT();
    }
    rv = bcmi_ltsw_profile_index_free(unit,
                                      BCMI_LTSW_PROFILE_VLAN_RANGE_MAP,
                                      profile_idx);

    /* Return directly if profile index is still used by others. */
    if (rv == SHR_E_BUSY) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Delete the information from VLAN_ASSIGNMENT_INNER/OUTER_VLAN_RANGE. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_delete(unit, 1, profile_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_delete(unit, 0, profile_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from vlan range map profile.
 *
 * This function is used to get entries from vlan range map profile.
 * (Related LTs -VLAN_ASSIGNMENT_INNER_VLAN_RANGE and
 * VLAN_ASSIGNMENT_OUTER_VLAN_RANGE).
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] profile_idx           Profile entry index.
 * \param [out] profile              Profile entry.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_get(int unit, int profile_idx,
                           ltsw_vlan_range_map_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    sal_memset(profile, 0, sizeof(ltsw_vlan_range_map_profile_t));

    /* Get the information from VLAN_ASSIGNMENT_INNER_VLAN_RANGE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_get(unit, profile_idx, 1, profile));

    /* Get the information from VLAN_ASSIGNMENT_OUTER_VLAN_RANGE table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_get(unit, profile_idx, 0, profile));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of vlan range map profile.
 *
 * This function is used to compare profile set of vlan range map profile.
 *
 * \param [in]  unit             Unit Number.
 * \param [in]  entries          Ingress membership check profile.
 * \param [in]  entries_per_set  Number of entries in the profile set.
 * \param [in]  index            Entry index of profile table.
 * \param [out] cmp              Result of comparison.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_cmp_cb(int unit, void *entries, int entries_per_set,
                              int index, int *cmp)
{
    ltsw_vlan_range_map_profile_t profile;

    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_get(unit, index, &profile));

    *cmp = sal_memcmp(entries,
                      (void *)&profile,
                      sizeof(ltsw_vlan_range_map_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate hash signature for vlan range map profile.
 *
 * This function is used to calculate hash signature for vlan range
 * map profile
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  entries              Ingress membership check profile.
 * \param [in]  entries_per_set      Number of entries in the profile set.
 * \param [out] hash                 Hash signature.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_hash_cb(int unit, void *entries,
                               int entries_per_set, uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != 1) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = shr_crc32(0, entries,
                      entries_per_set *
                      sizeof(ltsw_vlan_range_map_profile_t));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete vlan range index.
 *
 * \param [in] unit            Unit number.
 * \param [in] sys_port        System port ID.
 * \param [in] user_data       User_data.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_range_map_delete_cb(int unit, int sys_port, void *user_data)
{
    int profile_idx;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_raw_get(unit, sys_port,
                                    BCMI_PT_RANGE_MAP_INDEX, &profile_idx));
    if (profile_idx != VLAN_PROFILE_RANGE_MAP_RESV0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_delete(unit, profile_idx));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_raw_set(unit, sys_port,
                                        BCMI_PT_RANGE_MAP_INDEX,
                                        VLAN_PROFILE_RANGE_MAP_RESV0));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear all entries in vlan range map profile.
 *
 * This function is used to clear all entries in vlan range map
 * profile (Related LTs - VLAN_ASSIGNMENT_INNER_VLAN_RANGE and
 * VLAN_ASSIGNMENT_OUTER_VLAN_RANGE).
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_ASSIGNMENT_OUTER_VLAN_RANGEs));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, VLAN_ASSIGNMENT_INNER_VLAN_RANGEs));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Register vlan range map profile.
 *
 * This function is used to register vlan range map profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_register(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_RANGE_MAP;
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    int ent_idx_min = 0;
    int ent_idx_max = 0;

    SHR_FUNC_ENTER(unit);

    /* Get the range of profile index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_ASSIGNMENT_OUTER_VLAN_RANGEs,
                                       VLAN_ASSIGNMENT_OUTER_VLAN_RANGE_IDs,
                                       &idx_min, &idx_max));

    /* Register profile. */
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    vlan_range_map_profile_hash_cb,
                                    vlan_range_map_profile_cmp_cb));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count of vlan range map profile.
 *
 * This function is used to recover hash and reference count of vlan range map
 * profile.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index                 Entry index of profile table.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_recover(int unit, int index)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_RANGE_MAP;
    ltsw_vlan_range_map_profile_t profile;
    int entries_per_set = 1;
    uint32_t ref_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Update profile hash on the first usage.  */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl, index, &ref_count));

    if (ref_count == 0) {

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_get(unit, index, &profile));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update
                (unit, profile_hdl, &profile, entries_per_set, index));
    }

    /* Increment reference count. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover hash and reference count per system port.
 *
 * This function is used to recover hash and reference count per system port.
 * profile.
 *
 * \param [in] unit            Unit number.
 * \param [in] sys_port        System port ID.
 * \param [in] user_data       User_data.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_range_map_recover_cb(int unit, int sys_port, void *user_data)
{
    int profile_idx;
    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(user_data);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_raw_get(unit, sys_port,
                                    BCMI_PT_RANGE_MAP_INDEX, &profile_idx));
    if (profile_idx != VLAN_PROFILE_RANGE_MAP_RESV0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_recover(unit, profile_idx));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the information of vlan range map profile.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_range_map_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_VLAN_RANGE_MAP;
    ltsw_vlan_range_map_profile_t profile;
    int profile_idx = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_init_state_get(unit)) {
        /* Try to clear all profile info during bcm init. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_clear(unit));

        if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_unregister(unit, profile_hdl));
        }
    } else {
        /*
         * VLAN-specific init:
         * Don't unregister it since profile may still be used by others.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sys_port_traverse(unit, vlan_range_map_delete_cb, NULL));

        SHR_EXIT();
    }

    /* Register profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_register(unit));

    /* Reserve entry 0 with ranges unused (min == 1, max == 0). */
    sal_memset(&profile, 0, sizeof(ltsw_vlan_range_map_profile_t));
    for (i = 0; i < 8; i++) {
        profile.ivid_min[i] = 1;
        profile.ovid_min[i] = 1;
    }

    /* Add default profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_add(unit, &profile, &profile_idx));
    /* Check if default profile is at index 0.  Make sure default profile always exists. */
    if (profile_idx != VLAN_PROFILE_RANGE_MAP_RESV0) {
        (void)vlan_range_map_profile_delete(unit, profile_idx);
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create mutex for VLAN module.
 *
 * This function is used to create mutex for VLAN module.
 *
 * \param [in] unit              Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_mutex_create(int unit)
{
    ltsw_vlan_info_t *vlan_info = &ltsw_vlan_info[unit];

    SHR_FUNC_ENTER(unit);

    if (vlan_info->range_mutex == NULL) {
        vlan_info->range_mutex = sal_mutex_create("ltswVlanRangeMutex");
        SHR_NULL_CHECK(vlan_info->range_mutex, SHR_E_MEMORY);
    }

    if (vlan_info->profile_mutex == NULL) {
        vlan_info->profile_mutex = sal_mutex_create("ltswVlanProfileMutex");
        SHR_NULL_CHECK(vlan_info->profile_mutex, SHR_E_MEMORY);
    }

    if (vlan_info->ing_xlate_mutex == NULL) {
        vlan_info->ing_xlate_mutex = sal_mutex_create("ltswVlanIngXlateMutex");
        SHR_NULL_CHECK(vlan_info->ing_xlate_mutex, SHR_E_MEMORY);
    }

    if (vlan_info->egr_xlate_mutex == NULL) {
        vlan_info->egr_xlate_mutex = sal_mutex_create("ltswVlanEgrXlateMutex");
        SHR_NULL_CHECK(vlan_info->egr_xlate_mutex, SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy mutex for VLAN module.
 *
 * This function is used to destroy mutex for VLAN module.
 *
 * \param [in] unit              Unit number.
 *
 * \retval None
 */
static void
vlan_mutex_destroy(int unit)
{
    ltsw_vlan_info_t *vlan_info = &ltsw_vlan_info[unit];

    if (vlan_info->range_mutex) {
        sal_mutex_destroy(vlan_info->range_mutex);
        vlan_info->range_mutex = NULL;
    }

    if (vlan_info->profile_mutex) {
        sal_mutex_destroy(vlan_info->profile_mutex);
        vlan_info->profile_mutex = NULL;
    }

    if (vlan_info->ing_xlate_mutex) {
        sal_mutex_destroy(vlan_info->ing_xlate_mutex);
        vlan_info->ing_xlate_mutex = NULL;
    }

    if (vlan_info->egr_xlate_mutex) {
        sal_mutex_destroy(vlan_info->egr_xlate_mutex);
        vlan_info->egr_xlate_mutex = NULL;
    }

    return;
}

/*!
 * \brief Initialize the profiles of VLAN module.
 *
 * This function is used to initialize the profiles of VLAN module.
 *
 * \param [in] unit            Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_profile_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize the information of ingress membership check profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_init(unit));

    /* Initialize the information of egress membership check profile.. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_init(unit));

    /* Initialize the information of block mask profile per VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_block_mask_vfi_profile_init(unit));

    /* Initialize the information of egress untag profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_init(unit));

    /* Initialize the information of vlan range check profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_init(unit));

    /* Initialize the information of l2mc profile. */
    if (VLAN_DLF_FLOOD(unit)) {
        (void)bcmi_ltsw_multicast_vlan_flood_unregister(unit);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_vlan_flood_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Re-initialize the profiles of VLAN module.
 *
 * This function is used to re-initialize the profiles of VLAN module.
 *
 * \param [in] unit            Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_profile_reinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Register ingress membership check profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_register(unit));

    /* Recover reserved entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_recover(unit,
                                           VLAN_PROFILE_ING_MSHP_CHK_RESV0, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_recover(unit,
                                           VLAN_PROFILE_ING_MSHP_CHK_RESV1, 0));

    /* Register egress membership check profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_register(unit));

    /* Recover reserved entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_recover(unit,
                                           VLAN_PROFILE_EGR_MSHP_CHK_RESV0));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_recover(unit,
                                           VLAN_PROFILE_EGR_MSHP_CHK_RESV1));

    /* Register block mask profile per VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_block_mask_vfi_profile_register(unit));

    /* Register egress untag profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_register(unit));

    /* Recover reserved entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_recover(unit,
                                        VLAN_PROFILE_EGR_UNTAG_RESV0));

    /* Register vlan range check profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_register(unit));

    /* Recover reserved entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_recover(unit,
                                        VLAN_PROFILE_RANGE_MAP_RESV0));

    /* Recover VLAN range map information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sys_port_traverse(unit, vlan_range_map_recover_cb, NULL));

    /* Initialize the information of l2mc profile. */
    if (VLAN_DLF_FLOOD(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_vlan_flood_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STG into a specified ingress VFI entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vfi       VFI index.
 * \param [in] stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
vlan_ing_stg_set(int unit, int vfi, bcm_stg_t stg)
{
    ltsw_vlan_ing_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    entry.vfi = vfi;

    entry.fld_bmp |= IV_FLD_STG;
    entry.stg = (uint64_t)stg;

    /* Update vlan entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_UPDATE, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get STG from a specified ingress VFI entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vfi       VFI index.
 * \param [out]stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
vlan_ing_stg_get(int unit, int vfi, bcm_stg_t *stg)
{
    ltsw_vlan_ing_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    entry.vfi = vfi;

    entry.fld_bmp |= IV_FLD_STG;

    /* Update VFI entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    *stg = (bcm_stg_t)entry.stg;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STG into a specified egress VFI entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vfi       VFI index.
 * \param [in] stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
vlan_egr_stg_set(int unit, int vfi, bcm_stg_t stg)
{
    ltsw_vlan_egr_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    entry.vfi = vfi;

    entry.fld_bmp |= EV_FLD_STG;
    entry.stg = (uint64_t)stg;

    /* Update VFI entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, BCMLT_OPCODE_UPDATE, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get STG from a specified egress VFI entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vfi       VFI index.
 * \param [out]stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
vlan_egr_stg_get(int unit, int vfi, bcm_stg_t *stg)
{
    ltsw_vlan_egr_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    entry.vfi = vfi;
    entry.fld_bmp |= EV_FLD_STG;

    /* Update vlan entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    *stg = (bcm_stg_t)entry.stg;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete VLAN XLATE miss policy tables..
 *
 * This function is used to delete  R_VLAN_XLATE_MISS_POLICY
 * and R_EGR_VLAN_XLATE_MISS_POLICY.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */

static int
vlan_xlate_miss_policy_clear(int unit)
{
    const bcmint_vlan_lt_t *lt_info;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmint_vlan_lt_get(unit,
                            BCMINT_LTSW_VLAN_R_VLAN_XLATE_MISS_POLICY,
                            &lt_info);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, lt_info->name));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit,
                            BCMINT_LTSW_VLAN_R_EGR_VLAN_XLATE_MISS_POLICY,
                            &lt_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_clear(unit, lt_info->name));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete VLAN XLATE tables.
 *
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */

static int
vlan_xlate_table_clear(int unit)
{
    const bcmint_vlan_lt_t *lt_info;
    bcmint_vlan_lt_id_t lt_id[] = {
        BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE,
        BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE,
        BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE,
        BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION,
        BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW
    };
    int i, rv;
    SHR_FUNC_ENTER(unit);

    /* Init vlan does not clean xlate tables. */
    if (!bcmi_ltsw_init_state_get(unit)) {
        SHR_EXIT();
    }

    for (i = 0; i < COUNTOF(lt_id); i++) {
        rv = bcmint_vlan_lt_get(unit, lt_id[i], &lt_info);
        if (SHR_SUCCESS(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, lt_info->name));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear VLAN entries.
 *
 * This function is used to clear VLAN entries in VFI tables.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  ingress              ingress or egress.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_table_clear(int unit, int ingress)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int bcm_init = bcmi_ltsw_init_state_get(unit);
    const bcmint_vlan_lt_t *lt_info;
    const bcmint_vlan_fld_t *flds;
    bcmint_vlan_lt_id_t lt_id[] = {BCMINT_LTSW_VLAN_EGR_VFI,
                                   BCMINT_LTSW_VLAN_ING_VFI_TABLE};
    int fld_name[] = {BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI,
                      BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI};
    int rv = SHR_E_NONE;
    bcm_vlan_t vlan = 0;
    uint8_t bmp = 0;
    int dunit = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id[ingress], &lt_info));
    flds = lt_info->flds;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, ent_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        /* Try to clear VLAN profile info per entry during VLAN-only init. */
        if (!bcm_init) {

            if (ingress) {
                ltsw_vlan_ing_entry_t entry;
                sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));
                entry.fld_bmp |= IV_FLD_BLOCK_PROF_IDX |
                                 IV_FLD_OUTER_TPID_BMP |
                                 IV_FLD_MSHIP_PROF_IDX;
                if (VLAN_DLF_FLOOD(unit)) {
                    entry.fld_bmp |= IV_FLD_DESTINATION;
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_ing_entry_fields_get(unit, ent_hdl, &entry));
                if (entry.vfi > BCM_VLAN_MAX) {
                    continue;
                }
                vlan = (bcm_vlan_t)entry.vfi;

                /* Do not touch reserved entry 0. */
                if (vlan != VLAN_RESV0) {
                    /* Try to delete block profile. */
                    /* Entry 0 (only in PT) is reserved for unblock usage. */
                    if (entry.block_prof_idx != 0) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (vlan_ing_block_mask_vfi_profile_delete
                                 (unit, entry.block_prof_idx));
                    }

                    /* Try to destory the profile for ING membership check. */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (vlan_ing_mshp_chk_profile_delete
                             (unit, (int)entry.mship_prof_idx));

                    /* Try to decrease TPID count. */
                    bmp = (uint8_t)entry.outer_tpid_bmp;
                    i = 0;
                    while(bmp) {
                        if (bmp & 1) {
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmi_ltsw_vlan_otpid_entry_delete(unit, i));
                        }
                        bmp = bmp >> 1;
                        i++;
                    }
                    if (VLAN_DLF_FLOOD(unit)) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmi_ltsw_multicast_vlan_flood_grp_destroy
                                                            (unit, entry.dest));
                    }
                }
            } else {
                ltsw_vlan_egr_entry_t entry;
                sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));
                entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX |
                                 EV_FLD_UNTAG_PROF_IDX;
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_egr_entry_fields_get(unit, ent_hdl, &entry));
                if (entry.vfi > BCM_VLAN_MAX) {
                    continue;
                }
                vlan = (bcm_vlan_t)entry.vfi;

                /* Do not touch reserved entry 0. */
                if (vlan != VLAN_RESV0) {
                    /* Try to destroy the profile for EGR membership check. */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (vlan_egr_mshp_chk_profile_delete
                             (unit, (int)entry.mship_prof_idx));

                    /* Try to destroy the profile for untag. */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (vlan_egr_untag_profile_delete
                             (unit, (int)entry.untag_prof_idx));
                }
            }
        } else {
            uint64_t value = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, flds[fld_name[ingress]].name, &value));
            if (value > BCM_VLAN_MAX) {
                continue;
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, ent_hdl, BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));

        if ((!bcm_init) && (vlan != VLAN_RESV0)) {
            /* Notify VLAN info to L2/L3 during VLAN-only init. */
            if (ingress) {
                bcmi_ltsw_l2_view_vlan_info_t info;
                /* Notify L2 STATION module. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_l2_station_clear(unit, vlan));
                /* Notify L2  module for l2 table view. */
                info.vlan = vlan;
                info.l2_view = (bool)bcmVlanL2ViewWide;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_l2_view_vlan_update(unit, &info));
            } else {
                /* Notify L3 INTF module. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_l3_intf_clear(unit, vlan));
            }
        }
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover SW states from the VLAN table.
 *
 * This function is used to recover SW states from the VLAN table.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  ingress              ingress or egress.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_table_recover(int unit, int ingress)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const bcmint_vlan_lt_t *lt_info;
    bcmint_vlan_lt_id_t lt_id[] = {BCMINT_LTSW_VLAN_EGR_VFI,
                                   BCMINT_LTSW_VLAN_ING_VFI_TABLE};
    int rv = SHR_E_NONE;
    bcm_vlan_t vlan = 0;
    uint8_t bmp = 0;
    int dunit = 0;
    int i = 0;
    bcm_pbmp_t pbm;
    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(pbm);
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id[ingress], &lt_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, ent_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        /* Try to recover VLAN profile entries. */
        if (ingress) {
            ltsw_vlan_ing_entry_t entry;
            sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));
            entry.fld_bmp |= IV_FLD_BLOCK_PROF_IDX |
                             IV_FLD_OUTER_TPID_BMP |
                             IV_FLD_MSHIP_PROF_IDX |
                             IV_FLD_STREN_PROF_IDX |
                             IV_FLD_STG;
            if (VLAN_DLF_FLOOD(unit)) {
                entry.fld_bmp |= IV_FLD_DESTINATION;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_entry_fields_get(unit, ent_hdl, &entry));
            vlan = (bcm_vlan_t)entry.vfi;

            /* Entry 0 is reserved. */
            if (vlan == VLAN_RESV0) {
                continue;
            }

            /* No default L2MC group for VPN. */
            if (VLAN_DLF_FLOOD(unit) && (vlan > BCM_VLAN_MAX)) {
                entry.dest = 0;
            }

            /* Try to recover block profile. */
            /* Entry 0 (only in PT) is reserved for unblock usage. */
            if (entry.block_prof_idx != VLAN_PROFILE_ING_BLOCK_MASK_RESV0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_ing_block_mask_vfi_profile_recover
                         (unit, entry.block_prof_idx));
            }

            /* Try to recover TPID. */
            bmp = (uint8_t)entry.outer_tpid_bmp;
            i = 0;
            while(bmp) {
                if (bmp & 1) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_ltsw_vlan_otpid_entry_ref_count_add(unit, i, 1));
                }
                bmp = bmp >> 1;
                i++;
            }

            /* Try to recover the profile for ingress membership check. */
            /* Entry 0 and Entry 1 are reserved. */
            if ((entry.mship_prof_idx != VLAN_PROFILE_ING_MSHP_CHK_RESV0) &&
                (entry.mship_prof_idx != VLAN_PROFILE_ING_MSHP_CHK_RESV1)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_ing_mshp_chk_profile_recover
                         (unit, entry.mship_prof_idx, entry.dest));
            }

            if (vlan <= BCM_VLAN_MAX) {

                /* Recover l2mc entries in VLANs which have empty port members.*/
                if (VLAN_DLF_FLOOD(unit) &&
                    (entry.mship_prof_idx == VLAN_PROFILE_ING_MSHP_CHK_RESV1)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_ltsw_multicast_vlan_flood_grp_recover(unit,
                                                                    &pbm,
                                                                    entry.dest));
                }

                /* Set the usage bitmap of VLANs. */
                if (!bcmint_vlan_info_pre_cfg_get(unit, vlan)) {
                    /* non-zero stren_prof_idx has been set for valid VLAN. */
                    if (entry.stren_prof_idx) {
                        bcmint_vlan_info_set(unit, vlan);
                    }
                }
            }

            /*
             * VLAN is initialized before L3 module, L3 itself need the recover
             * the information via l3_intf_update_all().
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_l2_station_update(unit, &entry));
        } else {

            ltsw_vlan_egr_entry_t entry;
            sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));
            entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX |
                             EV_FLD_UNTAG_PROF_IDX;
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_entry_fields_get(unit, ent_hdl, &entry));

            /* Entry 0 is reserved. */
            if (entry.vfi == VLAN_RESV0) {
                continue;
            }

            /* Try to recover the profile for egress membership check. */
            /* Entry 0 and Entry 1 are reserved. */
            if ((entry.mship_prof_idx != VLAN_PROFILE_EGR_MSHP_CHK_RESV0) &&
                (entry.mship_prof_idx != VLAN_PROFILE_EGR_MSHP_CHK_RESV1)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_egr_mshp_chk_profile_recover
                         (unit, entry.mship_prof_idx));
            }

            /* Try to recover the profile for untag. */
            /* Entry 0 is reserved. */
            if (entry.untag_prof_idx !=
                    BCMI_LTSW_VLAN_PROFILE_EGR_UNTAG_IDX_DEF) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_egr_untag_profile_recover
                         (unit, (int)entry.untag_prof_idx));
            }

        }
    }

    if (rv != SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize ingress VLAN entry during init stage.
 *
 * This function is used to initialize ingress VLAN entry during init stage.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_ing_entry_init(int unit)
{
    int vlan = 0;
    ltsw_vlan_ing_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    for (vlan = BCM_VLAN_MIN; vlan <= BCM_VLAN_MAX; vlan++) {

       if (vlan == VLAN_RESV0) {
           continue;
       }

       /* Add VLAN ID. */
       entry.vfi = vlan;

       /* Learning is disabled for invaild VLAN by default. */
       entry.fld_bmp |= IV_FLD_NOT_LEARN;
       entry.do_not_learn = 1;

       /* Set profile_ptr index to the entry with empty port bitmap. */
       entry.fld_bmp |= IV_FLD_MSHIP_PROF_IDX;
       entry.mship_prof_idx = VLAN_PROFILE_ING_MSHP_CHK_RESV1;

       /* Insert default vlan entry. */
       SHR_IF_ERR_VERBOSE_EXIT
           (vlan_ing_table_op(unit, BCMLT_OPCODE_INSERT, &entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize egress VLAN entry during init stage.
 *
 * This function is used to initialize egress VLAN entry during init stage.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_egr_entry_init(int unit)
{
    int vlan = 0;
    ltsw_vlan_egr_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    for (vlan = BCM_VLAN_MIN; vlan <= BCM_VLAN_MAX; vlan++) {

       if (vlan == VLAN_RESV0) {
           continue;
       }

       /* Add VLAN ID. */
       entry.vfi = vlan;

       /* Set profile_ptr index to the entry with empty port bitmap. */
       entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX;
       entry.mship_prof_idx = VLAN_PROFILE_EGR_MSHP_CHK_RESV1;

       /* Insert default vlan entry. */
       SHR_IF_ERR_VERBOSE_EXIT
           (vlan_egr_table_op(unit, BCMLT_OPCODE_INSERT, &entry));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the VLAN module.
 *
 * This function is used to initialize the VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear VLAN entries in ING_VFI_TABLE. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_table_clear(unit, 1));

    /* Clear VLAN entries in EGR_VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_table_clear(unit, 0));

    /* Delete  VLAN XLATE miss policy tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_miss_policy_clear(unit));

    /* Delete  VLAN XLATE tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_table_clear(unit));

    /* Initialize profiles in VLAN module. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_init(unit));

    /* Init all VLAN entries in order to drop invalid vlan per member check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_entry_init(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_entry_init(unit));
    /* Earlier SDK version might not support such function. */
    bcmint_vlan_entry_init_state_set(unit, 1);
    ltsw_vlan_info[unit].flags |= VLAN_F_VLAN_ENTRY_INIT;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover the SW state in VLAN module.
 *
 * This function is used to recover the SW state in VLAN module.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_reinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Check if all VLAN entries has been inited bofore WB. */
    if (bcmint_vlan_entry_init_state_get(unit)) {
        ltsw_vlan_info[unit].flags |= VLAN_F_VLAN_ENTRY_INIT;
    }

    /* Re-initialize the profiles in VLAN module. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_profile_reinit(unit));

    /* Recover SW states related to ING_VFI_TABLE. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_table_recover(unit, 1));

    /* Recover SW states related to EGR_VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_table_recover(unit, 0));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set default VLAN during initialization.
 *
 * This function is used to set default VLAN during initialization.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] vd                   Default VLAN information.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_default_vlan_populate(int unit, bcm_vlan_data_t *vd)
{
    bcm_pbmp_t  pbmp_all;
    bcm_pbmp_t  pbmp_lb;
    bcm_pbmp_t  pbmp_rdb;
    bcm_pbmp_t  pbmp_cmic;
    bcm_pbmp_t  pbmp_e_all;
    bcm_pbmp_t  pbmp_local;
    int port_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp_all));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbmp_lb));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_RDB, &pbmp_rdb));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_CPU, &pbmp_cmic));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp_e_all));

    /* Set default VLAN. */
    BCM_PBMP_CLEAR(pbmp_local);
    BCM_PBMP_ASSIGN(pbmp_local, pbmp_all);

    vd->vlan_tag = BCM_VLAN_DEFAULT;
    BCM_PBMP_ASSIGN(vd->port_bitmap, pbmp_local);
    BCM_PBMP_ASSIGN(vd->ut_port_bitmap, pbmp_local);
    BCM_PBMP_REMOVE(vd->ut_port_bitmap, pbmp_cmic);
    BCM_PBMP_REMOVE(vd->ut_port_bitmap, pbmp_lb);
    BCM_PBMP_REMOVE(vd->ut_port_bitmap, pbmp_rdb);

    port_type = bcmint_vlan_info_default_port_type_get(unit);

    if (port_type == 1 || port_type == 3) {
        /* No CPU port in default VLAN. */
        BCM_PBMP_REMOVE(vd->port_bitmap, pbmp_cmic);
    }

    if (port_type == 2 || port_type == 3) {
        /* No Ethernet port in default VLAN. */
        BCM_PBMP_REMOVE(vd->port_bitmap, pbmp_e_all);
        BCM_PBMP_REMOVE(vd->ut_port_bitmap, pbmp_e_all);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add ingress reserved VLAN entry 0.
 *
 * This function is reserved VLAN entry 0 pointing to NULL profile entry.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_reserved_entry_add(int unit)
{
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_VFI;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NONE;
    ltsw_vlan_ing_entry_t entry;
    int index = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    entry.vfi = VLAN_RESV0;

    entry.fld_bmp |= IV_FLD_BLOCK_PROF_IDX;
    entry.block_prof_idx = VLAN_PROFILE_ING_BLOCK_MASK_RESV0;

    entry.fld_bmp |= IV_FLD_MSHIP_PROF_IDX;
    entry.mship_prof_idx = VLAN_PROFILE_ING_MSHP_CHK_RESV0;

    entry.fld_bmp |= IV_FLD_STG;
    entry.stg = 0;

    entry.fld_bmp |= IV_FLD_OUTER_TPID_BMP;
    entry.outer_tpid_bmp = 0xf;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &index));
    entry.fld_bmp |= IV_FLD_STREN_PROF_IDX;
    entry.stren_prof_idx = (uint64_t)index;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_INSERT, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add ingress default VLAN entry.
 *
 * This function is used to add ingress default VLAN entry.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] vd                   Default VLAN information.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_ing_default_entry_add(int unit, bcm_vlan_data_t *vd)
{
    ltsw_vlan_ing_mshp_chk_profile_t profile;
    int index = 0;
    ltsw_vlan_ing_entry_t entry;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_VFI;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NONE;
    uint32_t grp_id = 0;
    bcmi_ltsw_l2_view_vlan_info_t info;
    bcmlt_opcode_t op_code = BCMLT_OPCODE_INSERT;
    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    /* Add VLAN ID. */
    entry.vfi = vd->vlan_tag;

    /*
     * Set the index of default Bitmap Mask Profile Table used to apply
     * block mask on L2/L3 bitmap. Entry 0 (with valid = 0) is used for
     * unblcok by default.
     */
    entry.fld_bmp |= IV_FLD_BLOCK_PROF_IDX;
    entry.block_prof_idx = VLAN_PROFILE_ING_BLOCK_MASK_RESV0;

    /* Set destination/destination_type with a default 0 L2MC group for flooding. */
    entry.fld_bmp |= IV_FLD_DESTINATION;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_def_vlan_grp_id_get(unit, &grp_id));
    entry.dest = grp_id;
    entry.dest_type = BCMI_XFS_DEST_TYPE_L2MC_GROUP;

    /* Learning is NOT disabled for this layer 2 forwarding by default. */
    entry.fld_bmp |= IV_FLD_NOT_LEARN;
    entry.do_not_learn = 0;

    /* Do not check outer TPID by default. */
    entry.fld_bmp |= IV_FLD_OUTER_TPID_BMP;
    entry.outer_tpid_bmp = 0;

    /* IPv4 multicast packets can be bridged on this L2 forwarding domain. */
    entry.fld_bmp |= IV_FLD_IP4MC_L2_EN;
    entry.ip4mc_l2_enable = 1;

    /* IPv6 multicast packets can be bridged on this L2 forwarding domain. */
    entry.fld_bmp |= IV_FLD_IP6MC_L2_EN;
    entry.ip6mc_l2_enable = 1;

    /* Flood into the VFI domain for L2 Multicast lookup miss. */
    entry.fld_bmp |= IV_FLD_L2MC_MISS_ACTION;
    entry.l2mc_miss_action = VLAN_MISS_ACTION_TYPE_FLOOD;

    /* Flood into the VFI domain for L2 Unicast lookup miss. */
    entry.fld_bmp |= IV_FLD_L2UC_MISS_ACTION;
    entry.l2uc_miss_action = VLAN_MISS_ACTION_TYPE_FLOOD;

    entry.fld_bmp |= IV_FLD_INGRESS_IF;
    entry.ingress_if = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &index));
    entry.fld_bmp |= IV_FLD_STREN_PROF_IDX;
    entry.stren_prof_idx = (uint64_t)index;

    /* Set VLAN itself for a none-shared or none-private VLAN. */
    entry.fld_bmp |= IV_FLD_FWD_VFI;
    entry.fwd_vfi = vd->vlan_tag;

    /* Set opaque control id. */
    entry.fld_bmp |= IV_FLD_OPAQUE_CTRL_ID;
    entry.opaque_ctrl_id = 0;

    /* Set profile_ptr used by ingress membership check. */
    sal_memset(&profile, 0, sizeof(ltsw_vlan_ing_mshp_chk_profile_t));

    BCM_PBMP_ASSIGN(profile.member_ports, vd->port_bitmap);
    BCM_PBMP_ASSIGN(profile.egr_member_ports, vd->port_bitmap);
    profile.egr_mask_target = LTSW_VLAN_MASK_TARGET_L2_ONLY;
    profile.egr_mask_action = LTSW_VLAN_MASK_ACTION_AND;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_add(unit, &profile, &index));

    entry.fld_bmp |= IV_FLD_MSHIP_PROF_IDX;
    entry.mship_prof_idx = index;

    /* Set STG index used by spanning tree check. */
    entry.fld_bmp |= IV_FLD_STG;
    entry.stg = BCM_STG_DEFAULT;

    /* Use L2 table wide view by default. */
    entry.fld_bmp |= IV_FLD_L2_HOST_NARROW_ENABLE;
    entry.l2_host_narrow_enable = bcmVlanL2ViewWide;
    info.vlan = vd->vlan_tag;
    info.l2_view = (bool)entry.l2_host_narrow_enable;

    /* Insert default vlan entry. */
    op_code =
        VLAN_ENTRY_INIT_EN(unit) ? BCMLT_OPCODE_UPDATE : BCMLT_OPCODE_INSERT;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, op_code, &entry));

    /* Notify L2 module of l2 table view change. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l2_view_vlan_update(unit, &info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add egress default VLAN entry.
 *
 * This function is used to add egress default VLAN entry.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] vd                   Default VLAN information.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_default_entry_add(int unit, bcm_vlan_data_t *vd)
{
    ltsw_vlan_egr_mshp_chk_profile_t mshp_profile;
    ltsw_vlan_egr_untag_profile_t untag_profile;
    int index = 0, pf_idx;
    ltsw_vlan_egr_entry_t entry;
    bcmlt_opcode_t op_code = BCMLT_OPCODE_INSERT;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_EGR_VFI;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NONE;
    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    /* Add VLAN ID. */
    entry.vfi = vd->vlan_tag;

    /* Set profile_ptr used by membership check. */
    sal_memset(&mshp_profile, 0, sizeof(ltsw_vlan_egr_mshp_chk_profile_t));

    BCM_PBMP_ASSIGN(mshp_profile.member_ports, vd->port_bitmap);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_add(unit, &mshp_profile, &index));

    entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX;
    entry.mship_prof_idx = index;

    /* Set default Spanning tree group. */
    entry.fld_bmp |= EV_FLD_STG;
    entry.stg = BCM_STG_DEFAULT;

     /* Set opaque control id. */
    entry.fld_bmp |= EV_FLD_EFP_CTRL_ID;
    entry.efp_ctrl_id = 0;

    /* Set class id. */
    entry.fld_bmp |= EV_FLD_CLASS_ID;
    entry.class_id = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &pf_idx));
    entry.fld_bmp |= EV_FLD_STRENGTH_PRFL_IDX;
    entry.strength_prfl_idx = pf_idx;
    /*
     * Set default outer tag for default VLAN. The 0-4K region of the vfi
     * namespace is reserved for identity mapping of Vlans.
     */
    entry.fld_bmp |= EV_FLD_O_VLAN;
    entry.o_vlan = VLAN_CTRL(0, 0, vd->vlan_tag);

    /* Add ovlan by default since otag is not preserved in L2_IIF. */
    entry.fld_bmp |= EV_FLD_TAG_ACTION;
    entry.tag_action = 1;
    /* And preserve pkt's odot1p by default. Egr port takes final control. */
    entry.tag_action |= (1 << 2);

    /* Set entry view. */
    entry.fld_bmp |= EV_FLD_VIEW;
    entry.view = EGR_VFI_VIEW_DEFAULT;

    /* Set index to untag port lookup to check if egress port is untagged. */
    sal_memset(&untag_profile, 0, sizeof(ltsw_vlan_egr_untag_profile_t));

    BCM_PBMP_ASSIGN(untag_profile.member_ports, vd->ut_port_bitmap);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_add(unit, &untag_profile, &index));

    entry.fld_bmp |= EV_FLD_UNTAG_PROF_IDX;
    entry.untag_prof_idx = index;

    /* Insert default vlan entry. */
    op_code =
        VLAN_ENTRY_INIT_EN(unit) ? BCMLT_OPCODE_UPDATE : BCMLT_OPCODE_INSERT;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, op_code, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add  XLATE miss policy default entry.
 *
 * This function is used to add default R_EGR_VLAN_XLATE_MISS_POLICY
 * and R_VLAN_XLATE_MISS_POLICY entry.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_xlate_miss_policy_default_entry_add(int unit)
{
    int dunit;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const bcmint_vlan_lt_t *lt_info;
    int rv;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    rv = bcmint_vlan_lt_get(unit,
                            BCMINT_LTSW_VLAN_R_VLAN_XLATE_MISS_POLICY,
                            &lt_info);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, ent_hdl, BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));

        (void)bcmlt_entry_free(ent_hdl);
        ent_hdl = BCMLT_INVALID_HDL;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit,
                            BCMINT_LTSW_VLAN_R_EGR_VLAN_XLATE_MISS_POLICY,
                            &lt_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl, BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add default VLAN entry.
 *
 * This function is used to add default VLAN entry.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_default_entry_add(int unit)
{
    bcm_vlan_data_t vd;

    SHR_FUNC_ENTER(unit);

    /* Populate default VLAN information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_default_vlan_populate(unit, &vd));

    /* Set ING_VFI_TABLE. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_reserved_entry_add(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_default_entry_add(unit, &vd));

    /* Set EGR_VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_default_entry_add(unit, &vd));

    /* Set XLATE miss policy default entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_miss_policy_default_entry_add(unit));

    /* Set default vlan into VLAN information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_info_default_vlan_set(unit, vd.vlan_tag));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create ingress VLAN entry
 *
 * This function is used to create ingress VLAN entry.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_ing_entry_create(int unit, bcm_vlan_t vid)
{
    int pre_config = 0;
    int index = 0, l2mc_created = 0;
    uint32_t idx = 0;
    ltsw_vlan_ing_entry_t entry;
    bcmlt_opcode_t op_code = BCMLT_OPCODE_INSERT;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_VFI;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NONE;
    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));
    pre_config = bcmint_vlan_info_pre_cfg_get(unit, vid);

    /* Add VLAN ID. */
    entry.vfi = vid;

    /* Ingore the fileds which may have been configured by VLAN Control. */
    if (!pre_config) {
        bcmi_ltsw_l2_view_vlan_info_t info;

        /* Learning is NOT disabled for this layer 2 forwarding by default. */
        entry.fld_bmp |= IV_FLD_NOT_LEARN;
        entry.do_not_learn = 0;

        /* Do not check outer TPID by default. */
        entry.fld_bmp |= IV_FLD_OUTER_TPID_BMP;
        entry.outer_tpid_bmp = 0;

        /* IPv4 multicast packets can be bridged on L2 forwarding domain. */
        entry.fld_bmp |= IV_FLD_IP4MC_L2_EN;
        entry.ip4mc_l2_enable = 1;

        /* IPv6 multicast packets can be bridged on L2 forwarding domain. */
        entry.fld_bmp |= IV_FLD_IP6MC_L2_EN;
        entry.ip6mc_l2_enable = 1;

        /* Set opaque control id. */
        entry.fld_bmp |= IV_FLD_OPAQUE_CTRL_ID;
        entry.opaque_ctrl_id = 0;

        /* Flood into the VFI domain for L2 Multicast lookup miss. */
        entry.fld_bmp |= IV_FLD_L2MC_MISS_ACTION;
        entry.l2mc_miss_action = VLAN_MISS_ACTION_TYPE_FLOOD;

        /* Flood into the VFI domain for L2 Unicast lookup miss. */
        entry.fld_bmp |= IV_FLD_L2UC_MISS_ACTION;
        entry.l2uc_miss_action = VLAN_MISS_ACTION_TYPE_FLOOD;

        entry.fld_bmp |= IV_FLD_INGRESS_IF;
        entry.ingress_if = 0;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &index));
        entry.fld_bmp |= IV_FLD_STREN_PROF_IDX;
        entry.stren_prof_idx = (uint64_t)index;

        /* Set profile_ptr index to the entry with empty port bitmap. */
        entry.fld_bmp |= IV_FLD_MSHIP_PROF_IDX;
        entry.mship_prof_idx = VLAN_PROFILE_ING_MSHP_CHK_RESV1;

        /* Use L2 table wide view by default. */
        entry.fld_bmp |= IV_FLD_L2_HOST_NARROW_ENABLE;
        entry.l2_host_narrow_enable = bcmVlanL2ViewWide;
        info.vlan = vid;
        info.l2_view = (bool)entry.l2_host_narrow_enable;
        /* Notify L2 module of l2 table view change. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l2_view_vlan_update(unit, &info));
    }

    /* Set VLAN itself for a none-shared or none-private VLAN. */
    entry.fld_bmp |= IV_FLD_FWD_VFI;
    entry.fwd_vfi = vid;

    /* Set STG index used by spanning tree check SF. */
    entry.fld_bmp |= IV_FLD_STG;
    entry.stg = 0;

    /* Set index to apply block mask on L2/L3 bitmap. */
    entry.fld_bmp |= IV_FLD_BLOCK_PROF_IDX;
    entry.block_prof_idx = VLAN_PROFILE_ING_BLOCK_MASK_RESV0;

    if (VLAN_DLF_FLOOD(unit)) {
        bcm_pbmp_t pbm;
        BCM_PBMP_CLEAR(pbm);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_vlan_flood_grp_create(unit, &pbm, &idx));
        entry.fld_bmp |= IV_FLD_DESTINATION;
        entry.dest_type = BCMI_XFS_DEST_TYPE_L2MC_GROUP;
        entry.dest = idx;
        l2mc_created = 1;
    }
    /* Insert/update a vlan entry. */
    if (!pre_config) {
        op_code = VLAN_ENTRY_INIT_EN(unit) ? BCMLT_OPCODE_UPDATE :
                                             BCMLT_OPCODE_INSERT;
    } else {
        op_code = BCMLT_OPCODE_UPDATE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, op_code, &entry));

exit:
    if(SHR_FUNC_ERR() && l2mc_created) {
        (void)bcmi_ltsw_multicast_vlan_flood_grp_destroy(unit, idx);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add egress VLAN entry.
 *
 * This function is used to add egress VLAN entry.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  vid                  VLAN ID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_egr_entry_create(int unit, bcm_vlan_t vid)
{
    int pre_config = 0;
    ltsw_vlan_egr_entry_t entry;
    int pf_idx = 0;
    bcmlt_opcode_t op_code = BCMLT_OPCODE_INSERT;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_EGR_VFI;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NONE;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    pre_config = bcmint_vlan_info_pre_cfg_get(unit, vid);

    /* Add VLAN ID. */
    entry.vfi = vid;

    /* Set default Spanning tree group. */
    entry.fld_bmp |= EV_FLD_STG;
    entry.stg = 0;

    /* Set entry view. */
    entry.fld_bmp |= EV_FLD_VIEW;
    entry.view = EGR_VFI_VIEW_DEFAULT;

    /* Set index to untag port lookup to check if egress port is untagged. */
    entry.fld_bmp |= EV_FLD_UNTAG_PROF_IDX;
    entry.untag_prof_idx = VLAN_PROFILE_EGR_UNTAG_RESV0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &pf_idx));
    entry.fld_bmp |= EV_FLD_STRENGTH_PRFL_IDX;
    entry.strength_prfl_idx = pf_idx;

    /* Insert/update a vlan entry. */
    if (!pre_config) {

        /* Set profile_ptr index to the entry with empty port bitmap. */
        entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX;
        entry.mship_prof_idx = VLAN_PROFILE_EGR_MSHP_CHK_RESV1;

        /* Identity mapping between VLAN and VFI in 0-4K region. */
        entry.fld_bmp |= EV_FLD_O_VLAN;
        entry.o_vlan = VLAN_CTRL(0, 0, vid);
        /* Add ovlan by default since otag is not preserved in L2_IIF. */
        entry.fld_bmp |= EV_FLD_TAG_ACTION;
        entry.tag_action = 1;
        /* Preserve pkt's odot1p by default. Egr port takes final control. */
        entry.tag_action |= (1 << 2);

        /* Set opaque control id. */
        entry.fld_bmp |= EV_FLD_EFP_CTRL_ID;
        entry.efp_ctrl_id = 0;

        /* Set class id. */
        entry.fld_bmp |= EV_FLD_CLASS_ID;
        entry.class_id = 0;

        op_code = VLAN_ENTRY_INIT_EN(unit) ? BCMLT_OPCODE_UPDATE :
                                             BCMLT_OPCODE_INSERT;
    } else {
        op_code = BCMLT_OPCODE_UPDATE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, op_code, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a VLAN entry
 *
 * This function is used to create a VLAN entry.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_entry_create(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    /* Create an entry in LT ING_VFI_TABLE. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_entry_create(unit, vid));

    /* Create an entry in LT EGR_VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_entry_create(unit, vid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy ingress VLAN entry.
 *
 * This function is used to destroy ingress VLAN entry.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_ing_entry_destroy(int unit, bcm_vlan_t vid)
{
    uint8_t bmp = 0;
    int i = 0;
    ltsw_vlan_ing_entry_t entry;
    bcmlt_opcode_t op_code = BCMLT_OPCODE_DELETE;

    SHR_FUNC_ENTER(unit);

    /* Add VLAN ID. */
    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));
    entry.vfi = vid;
    entry.fld_bmp |= IV_FLD_BLOCK_PROF_IDX |
                     IV_FLD_OUTER_TPID_BMP |
                     IV_FLD_MSHIP_PROF_IDX |
                     IV_FLD_DESTINATION;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    /* Try to destroy block mask. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_block_mask_vfi_profile_delete(unit, entry.block_prof_idx));

    /* Try to clear the information of expected outer TPID. */
    bmp = (uint8_t)entry.outer_tpid_bmp;
    while(bmp) {
        if (bmp & 1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_vlan_otpid_entry_delete(unit, i));
        }
        bmp = bmp >> 1;
        i++;
    }

    /* Try to destory the profile for membership check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_delete(unit, entry.mship_prof_idx));

    /* Try to destory the l2mc flooding. */
    if (VLAN_DLF_FLOOD(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_vlan_flood_grp_destroy(unit, entry.dest));
    }

    if (VLAN_ENTRY_INIT_EN(unit)) {

        /* Update ingress VLAN entry with default value. */
        sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));
        entry.vfi = vid;
        entry.fld_bmp = IV_FLD_FWD_VFI | IV_FLD_DESTINATION |
                        IV_FLD_INGRESS_IF | IV_FLD_FLEX_CTR_ACTION |
                        IV_FLD_STREN_PROF_IDX | IV_FLD_STG |
                        IV_FLD_MSHIP_PROF_IDX | IV_FLD_BLOCK_PROF_IDX |
                        IV_FLD_OUTER_TPID_BMP | IV_FLD_OPAQUE_CTRL_ID |
                        IV_FLD_PVLAN_PORT_TYPE | IV_FLD_IP4MC_L2_EN |
                        IV_FLD_IP6MC_L2_EN | IV_FLD_L2MC_MISS_ACTION |
                        IV_FLD_L2UC_MISS_ACTION | IV_FLD_NOT_LEARN |
                        IV_FLD_L2_HOST_NARROW_ENABLE;

        entry.do_not_learn = 1;
        entry.mship_prof_idx = VLAN_PROFILE_ING_MSHP_CHK_RESV1;

        op_code = BCMLT_OPCODE_UPDATE;
    } else {
        /* Delete ingress VLAN entry. */
        op_code = BCMLT_OPCODE_DELETE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, op_code, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy egress VLAN entry.
 *
 * This function is used to destroy egress VLAN entry.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_egr_entry_destroy(int unit, bcm_vlan_t vid)
{
    ltsw_vlan_egr_entry_t entry;
    bcmlt_opcode_t op_code = BCMLT_OPCODE_DELETE;

    SHR_FUNC_ENTER(unit);

    /* Lookup VLAN entry. */
    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));
    entry.vfi = vid;
    entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX |
                     EV_FLD_UNTAG_PROF_IDX;
    SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    /* Try to destory the profile for membership check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_delete(unit, (int)entry.mship_prof_idx));

    /* Try to destory the profile for untag. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_delete(unit, (int)entry.untag_prof_idx));

    if (VLAN_ENTRY_INIT_EN(unit)) {

        /* Update egress VLAN entry with default value. */
        sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));
        entry.vfi = vid;
        entry.fld_bmp = EV_FLD_O_VLAN | EV_FLD_I_VLAN |
                        EV_FLD_TAG_ACTION | EV_FLD_UNTAG_PROF_IDX |
                        EV_FLD_STG | EV_FLD_MSHIP_PROF_IDX |
                        EV_FLD_FLEX_CTR_ACTION | EV_FLD_EFP_CTRL_ID |
                        EV_FLD_CLASS_ID | EV_FLD_VIEW |
                        EV_FLD_VSID | EV_FLD_STRENGTH_PRFL_IDX;
        entry.mship_prof_idx = VLAN_PROFILE_EGR_MSHP_CHK_RESV1;
        op_code = BCMLT_OPCODE_UPDATE;
    } else {
        /* Delete egress VLAN entry. */
        op_code = BCMLT_OPCODE_DELETE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, op_code, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy VLAN entry
 *
 * This function is used to destroy VLAN entry.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   vid      VLAN ID.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
static int
vlan_entry_destroy(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    /* Delete an entry in LT ING_VFI_TABLE. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_entry_destroy(unit, vid));

    /* Delete an entry in LT EGR_VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_entry_destroy(unit, vid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set member ports into ingress VLAN table.
 *
 * This function is used to set member ports into ingress VLAN table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  ing_pbmp      Ingress member ports.
 * \param [in]  ing_egr_pbmp  Egress member ports in ingress pipeline.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_ing_entry_port_set(int unit, bcm_vlan_t vid,
                        bcm_pbmp_t ing_pbmp, bcm_pbmp_t ing_egr_pbmp)
{
    ltsw_vlan_ing_mshp_chk_profile_t profile;
    int index = 0;
    bcm_pbmp_t pbmp_lb;
    bcm_pbmp_t pbmp_cmic;
    int l2mc_created = 0;
    uint32_t def_grp = 0, l2mc_idx = 0;
    ltsw_vlan_ing_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    /* Add VLAN ID. */
    entry.vfi = vid;

    /* Update profile for ingress membership check. */
    sal_memset(&profile, 0, sizeof(ltsw_vlan_ing_mshp_chk_profile_t));

    BCM_PBMP_ASSIGN(profile.member_ports, ing_pbmp);
    BCM_PBMP_ASSIGN(profile.egr_member_ports, ing_egr_pbmp);

    profile.egr_mask_target = LTSW_VLAN_MASK_TARGET_L2_ONLY;
    profile.egr_mask_action = LTSW_VLAN_MASK_ACTION_AND;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_add(unit, &profile, &index));

    /* Set profile_ptr used by membership check. */
    entry.fld_bmp |= IV_FLD_MSHIP_PROF_IDX;
    entry.mship_prof_idx = index;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbmp_lb));
    BCM_PBMP_REMOVE(ing_egr_pbmp, pbmp_lb);

    if (VLAN_DLF_FLOOD_MODE(unit) == LTSW_VLAN_FLOOD_MODE_EXCLUDE_CPU) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit,
                                   BCMI_LTSW_PORT_TYPE_CPU, &pbmp_cmic));
        BCM_PBMP_REMOVE(ing_egr_pbmp, pbmp_cmic);
    }

    if (VLAN_DLF_FLOOD(unit) && (vid <= BCM_VLAN_MAX)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_vlan_flood_grp_create(unit, &ing_egr_pbmp, &l2mc_idx));
        entry.fld_bmp |= IV_FLD_DESTINATION;
        entry.dest_type = BCMI_XFS_DEST_TYPE_L2MC_GROUP;
        entry.dest = l2mc_idx;
        l2mc_created = 1;
    }
    /* In TD4 specific flooding mode, keep port members in default l2mc
     * group always the same as original default vlan.
     */
    if ((vid == BCM_VLAN_DEFAULT) && (!VLAN_DLF_FLOOD(unit))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_def_vlan_grp_id_get(unit, &def_grp));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_vlan_flood_grp_update(unit, &ing_egr_pbmp, def_grp));
    }

    /* Update vlan entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_UPDATE, &entry));

exit:
    if(SHR_FUNC_ERR() && l2mc_created) {
        (void)bcmi_ltsw_multicast_vlan_flood_grp_destroy(unit, l2mc_idx);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set member ports into egress VLAN table.
 *
 * This function is used to set member ports into egress VLAN table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  egr_pbmp      Egress member ports.
 * \param [in]  egr_ubmp      Untagged member ports.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_egr_entry_port_set(int unit, bcm_vlan_t vid,
                        bcm_pbmp_t egr_pbmp, bcm_pbmp_t egr_ubmp)
{
    ltsw_vlan_egr_mshp_chk_profile_t mshp_profile;
    ltsw_vlan_egr_untag_profile_t untag_profile;
    int index = 0;
    ltsw_vlan_egr_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    /* Add VLAN ID. */
    entry.vfi = vid;

    /* Update profile_ptr used by membership check. */
    sal_memset(&mshp_profile, 0, sizeof(ltsw_vlan_egr_mshp_chk_profile_t));

    BCM_PBMP_ASSIGN(mshp_profile.member_ports, egr_pbmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_add(unit, &mshp_profile, &index));

    entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX;
    entry.mship_prof_idx = index;

    /* Update index to untag port lookup to check if egress port is untagged. */
    sal_memset(&untag_profile, 0, sizeof(ltsw_vlan_egr_untag_profile_t));

    BCM_PBMP_ASSIGN(untag_profile.member_ports, egr_ubmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_add(unit, &untag_profile, &index));

    entry.fld_bmp |= EV_FLD_UNTAG_PROF_IDX;
    entry.untag_prof_idx = index;

    /* Update vlan entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, BCMLT_OPCODE_UPDATE, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get member ports from ingress VLAN table.
 *
 * This function is used to get member ports from ingress VLAN table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [out] ing_pbmp      Ingress member ports.
 * \param [out] ing_egr_pbmp  Egress member ports in ingress pipeline.
 * \param [out] ing_prof_idx  Profile index for ingress member check.
 * \param [out] l2mc_idx      Profile index for L2_MC_GROUP.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_ing_entry_port_get(int unit, bcm_vlan_t vid, bcm_pbmp_t *ing_pbmp,
                        bcm_pbmp_t *ing_egr_pbmp, int *ing_prof_idx,
                        int *l2mc_idx)
{
    ltsw_vlan_ing_mshp_chk_profile_t profile;
    ltsw_vlan_ing_entry_t entry;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    /* Add VLAN ID. */
    entry.vfi = vid;
    entry.fld_bmp |= IV_FLD_MSHIP_PROF_IDX | IV_FLD_DESTINATION;

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    if (ing_prof_idx) {
        *ing_prof_idx = (int)entry.mship_prof_idx;
    }

    if (ing_pbmp || ing_egr_pbmp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_mshp_chk_profile_get(unit, (int)entry.mship_prof_idx,
                                           &profile, 1));

        if (ing_pbmp) {
            BCM_PBMP_ASSIGN(*ing_pbmp, profile.member_ports);
        }

        if (ing_egr_pbmp) {
            BCM_PBMP_ASSIGN(*ing_egr_pbmp, profile.egr_member_ports);
        }
    }
    if (l2mc_idx) {
        *l2mc_idx = entry.dest;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get member ports from egress VLAN table.
 *
 * This function is used to get member ports from egress VLAN table.
 *
 * \param [in]  unit           Unit number.
 * \param [in]  vid            VLAN ID.
 * \param [out] egr_pbmp       Egress member ports.
 * \param [out] egr_ubmp       Untagged member ports.
 * \param [out] egr_prof_idx   Profile index for egress member check.
 * \param [out] untag_prof_idx Profile index for untagged member.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
vlan_egr_entry_port_get(int unit, bcm_vlan_t vid,
                        bcm_pbmp_t *egr_pbmp, bcm_pbmp_t *egr_ubmp,
                        int *egr_prof_idx, int *untag_prof_idx)
{
    ltsw_vlan_egr_mshp_chk_profile_t mshp_profile;
    ltsw_vlan_egr_untag_profile_t untag_profile;
    ltsw_vlan_egr_entry_t entry;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    /* Allocate entry container. */
    entry.vfi = vid;
    entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX | EV_FLD_UNTAG_PROF_IDX;

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    if (egr_pbmp || egr_prof_idx) {
        /* Get profile used by membership check. */
        if (egr_prof_idx) {
            *egr_prof_idx = (int)entry.mship_prof_idx;
        }

        if (egr_pbmp) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_mshp_chk_profile_get(unit, (int)entry.mship_prof_idx,
                                               &mshp_profile, 1));
            BCM_PBMP_ASSIGN(*egr_pbmp, mshp_profile.member_ports);
        }
    }

    if (egr_ubmp || untag_prof_idx) {
        /* Get untagged profile. */
        if (untag_prof_idx) {
            *untag_prof_idx = (int)entry.untag_prof_idx;
        }

        if (egr_ubmp) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_untag_profile_get(unit, (int)entry.untag_prof_idx,
                                            &untag_profile, 1));
            BCM_PBMP_ASSIGN(*egr_ubmp, untag_profile.member_ports);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the entry view of egress VLAN table.
 *
 * This function is used to get the table view of egress VLAN table.
 *
 * \param [in]  unit           Unit number.
 * \param [in]  vid            VLAN ID.
 * \param [out] entry_view     Entry view.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
vlan_egr_entry_view_get(int unit, bcm_vlan_t vid, egr_vfi_view_t *entry_view)
{
    uint32_t vfi = 0;
    ltsw_vlan_egr_entry_t entry;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    /* Get VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, vid, &vfi));

    /* Add VLAN ID. */
    entry.vfi = vfi;
    entry.fld_bmp |= EV_FLD_VIEW;

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    *entry_view = entry.view;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set member ports into VLAN table.
 *
 * This function is used to set member ports into VLAN table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  ing_pbmp      Ingress member ports.
 * \param [in]  ing_egr_pbmp  Egress member ports in ingress pipeline.
 * \param [in]  egr_pbmp      Egress member ports.
 * \param [in]  egr_ubmp      Untagged member ports.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_entry_port_set(int unit, bcm_vlan_t vid,
                    bcm_pbmp_t ing_pbmp, bcm_pbmp_t ing_egr_pbmp,
                    bcm_pbmp_t egr_pbmp, bcm_pbmp_t egr_ubmp)
{
    bcm_pbmp_t pbmp_lb;

    SHR_FUNC_ENTER(unit);

    /*
     * Unlike Legacy chips, Hardware also performs ingress and egress VLAN
     * membership checks on internal loopback port.
     * To keep consistent with Legacy behavior, loopback ports are always in
     * VLAN membership by default.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbmp_lb));

    BCM_PBMP_OR(ing_pbmp, pbmp_lb);
    BCM_PBMP_OR(ing_egr_pbmp, pbmp_lb);
    BCM_PBMP_OR(egr_pbmp, pbmp_lb);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_entry_port_set(unit, vid, ing_pbmp, ing_egr_pbmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_entry_port_set(unit, vid, egr_pbmp, egr_ubmp));


exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get member ports from VLAN table.
 *
 * This function is used to get member ports from VLAN table.
 *
 * \param [in]  unit           Unit number.
 * \param [in]  vid            VLAN ID.
 * \param [out] ing_pbmp       Ingress member ports.
 * \param [out] ing_egr_pbmp   Egress member ports in ingress pipeline.
 * \param [out] egr_pbmp       Egress member ports.
 * \param [out] egr_ubmp       Untagged member ports.
 * \param [out] ing_prof_idx   Profile index for ingress member check.
 * \param [out] egr_prof_idx   Profile index for egress member check.
 * \param [out] untag_prof_idx Profile index for untagged member.
 * \param [out] l2mc_idx      Profile index for L2_MC_GROUP.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
vlan_entry_port_get(int unit, bcm_vlan_t vid,
                    bcm_pbmp_t *ing_pbmp, bcm_pbmp_t *ing_egr_pbmp,
                    bcm_pbmp_t *egr_pbmp, bcm_pbmp_t *egr_ubmp,
                    int *ing_prof_idx, int *egr_prof_idx, int *untag_prof_idx,
                    int *l2mc_idx)
{
    bcm_pbmp_t pbmp_lb;

    SHR_FUNC_ENTER(unit);

    if (ing_pbmp || ing_egr_pbmp || ing_prof_idx || l2mc_idx) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_entry_port_get(unit, vid, ing_pbmp,
                                     ing_egr_pbmp, ing_prof_idx, l2mc_idx));
    }

    if (egr_pbmp || egr_ubmp || egr_prof_idx || untag_prof_idx) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_entry_port_get(unit, vid, egr_pbmp, egr_ubmp,
                                     egr_prof_idx, untag_prof_idx));
    }

    if (ing_pbmp || ing_egr_pbmp || egr_pbmp) {
        /* Legacy behaviors - Do not expose Internal loopback ports to user. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbmp_lb));

        if (ing_pbmp) {
            BCM_PBMP_REMOVE(*ing_pbmp, pbmp_lb);
        }

        if (ing_egr_pbmp) {
            BCM_PBMP_REMOVE(*ing_egr_pbmp, pbmp_lb);
        }

        if (egr_pbmp) {
            BCM_PBMP_REMOVE(*egr_pbmp, pbmp_lb);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an outer TPID entry from LT VLAN_OUTER_TPID.
 *
 * This function is used to get an outer TPID entry from LT VLAN_OUTER_TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Entry index.
 * \param [out]  tpid                Outer TPID value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_otpid_entry_get(int unit, int index, uint16_t *tpid)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *tbl_name[] = {R_ING_OUTER_TPID_0s, R_ING_OUTER_TPID_1s,
                              R_ING_OUTER_TPID_2s, R_ING_OUTER_TPID_3s};
    int rv = SHR_E_NONE;
    uint64_t value = 0;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    *tpid = 0;

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbl_name[index], &ent_hdl));

    rv = bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);

    /* Entry may be not created before warmboot.*/
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    }

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, TPIDs, &value));

    *tpid = (uint16_t)value;

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an outer TPID entry into LT VLAN_OUTER_TPID.
 *
 * This function is used to get an outer TPID entry into LT VLAN_OUTER_TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  index                Entry index.
 * \param [in]  tpid                 Outer TPID value.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_otpid_entry_add(int unit, int index, uint16_t tpid)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *tbl_name[] = {R_ING_OUTER_TPID_0s, R_ING_OUTER_TPID_1s,
                              R_ING_OUTER_TPID_2s, R_ING_OUTER_TPID_3s};
    bcmi_ltsw_obm_port_control_t ctrl_type[] = {bcmiObmPortControlOuterTpid0,
                                                bcmiObmPortControlOuterTpid1,
                                                bcmiObmPortControlOuterTpid2,
                                                bcmiObmPortControlOuterTpid3};
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbl_name[index], &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, TPIDs, (uint64_t)tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, ent_hdl, BCMLT_PRIORITY_NORMAL));

    /*
     * Per legacy behavior, the configurations between Port TPID and OBM TPID
     * must be in sync.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_obm_pm_port_control_set(unit, -1,
                                           ctrl_type[index], (int)tpid));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the VLAN outer TPID tables.
 *
 * This function is used to initialize the VLAN outer TPID tables.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_otpid_table_init(int unit)
{
    uint16_t otpid_def[] = {0x8100, 0x9100, 0x88a8, 0xaa8a};
    int tpid_num = 0;
    int index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_otpid_max_num(unit, &tpid_num));

    /* Update VLAN TPID information. */
    for (index = 0; index < tpid_num; index++) {

        if (bcmi_warmboot_get(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_otpid_entry_get(unit, index, &otpid_def[index]));

        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_otpid_entry_add(unit, index, otpid_def[index]));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_otpid_info_tpid_set(unit, index, otpid_def[index]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the validation of ingress translate configuration.
 *
 * This function is used to check the validation of ingress translate
 * configuration.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_cfg_validate(int unit,
                            bcm_vlan_translate_key_t key_type,
                            bcm_gport_t port,
                            bcm_vlan_t outer_vlan,
                            bcm_vlan_t inner_vlan,
                            bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    ltsw_vlan_info_t *vlan_info = &ltsw_vlan_info[unit];
    SHR_FUNC_ENTER(unit);

    /* Only port group is available, and GPORT is not supported so far. */
    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Only group keys are supported. */
    switch (key_type) {
    case bcmVlanTranslateKeyPortGroupDouble:
        if (outer_vlan > BCM_VLAN_MAX || inner_vlan > BCM_VLAN_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (!vlan_info->xlate_table_size[XLATE_TABLE_ING_3]) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
        break;
    case bcmVlanTranslateKeyPortGroupOuter:
        if (outer_vlan > BCM_VLAN_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (!vlan_info->xlate_table_size[XLATE_TABLE_ING_1]) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
        break;
    case bcmVlanTranslateKeyPortGroupInner:
        if (inner_vlan > BCM_VLAN_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (!vlan_info->xlate_table_size[XLATE_TABLE_ING_2]) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*
     * Only preserve actions are supported in ingress vxlate.
     * bcmVlanActionNone - Preserve.
     * bcmVlanActionDelete - Not preserve.
     */
    if (cfg) {
        if ((cfg->otag_action != bcmVlanActionNone) &&
            (cfg->otag_action != bcmVlanActionDelete)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if ((cfg->itag_action != bcmVlanActionNone) &&
            (cfg->itag_action != bcmVlanActionDelete)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        /* For stren profile, Null fld_bmp not supported. */
        if (cfg->fld_bmp == 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the validation of egress translation configuration.
 *
 * This function is used to check the validation of egress translation
 * configuration.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Egress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_cfg_validate(int unit,
                            bcm_gport_t port,
                            bcm_vlan_t outer_vlan,
                            bcm_vlan_t inner_vlan,
                            ltsw_vlan_egr_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    /* Only port group is available, and GPORT is not supported so far. */
    if (BCM_GPORT_IS_SET(port)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Check the validation of VLAN or VPN. */
    if (!bcmi_ltsw_virtual_vlan_vpn_valid(unit, outer_vlan)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Inner VLAN is not used in Trident4 for now. */
    if (inner_vlan != BCM_VLAN_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (cfg) {
        /*
         * Only add actions are supported in egress vxlate.
         * bcmVlanActionNone - Don't add new tag.
         * bcmVlanActionAdd - Add new tag.
         */
        if ((cfg->otag_action != bcmVlanActionNone) &&
            (cfg->otag_action != bcmVlanActionAdd)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if ((cfg->itag_action != bcmVlanActionNone) &&
            (cfg->itag_action != bcmVlanActionAdd)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (cfg->outer_vlan > BCM_VLAN_MAX ||
            cfg->inner_vlan > BCM_VLAN_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (cfg->outer_pri < 0 || cfg->outer_pri > BCM_PRIO_MAX ||
            cfg->inner_pri < 0 || cfg->inner_pri > BCM_PRIO_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (cfg->outer_cfi > 1 || cfg->inner_cfi > 1) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the pre config fields of ingress VLAN translation entry.
 *
 * This function is used to get the pre config fields of ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl         LT entry container.
 * \param [out] cfg             Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_pre_config_get(int unit,
                                bcmlt_entry_handle_t ent_hdl,
                                bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    uint64_t value = 0;
    bcmlt_entry_info_t info;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth[] = {BCMI_LTSW_SBR_PTH_VLAN_XLATE_1,
                                             BCMI_LTSW_SBR_PTH_VLAN_XLATE_2,
                                             BCMI_LTSW_SBR_PTH_VLAN_XLATE_3};
    bcmi_ltsw_sbr_profile_ent_type_t ent_type =
        BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP_NO_VLAN_TAG_PRESERVE_CTRL;
    int stren, i, lt_num;
    const bcmint_vlan_lt_t *lt_info;
    bcmint_vlan_lt_id_t lt_id[] = {BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE,
                                   BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE,
                                   BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE};
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    SHR_FUNC_ENTER(unit);
    sal_memset(&info, 0, sizeof(bcmlt_entry_info_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_info_get(ent_hdl, &info));
    lt_num = COUNTOF(lt_id);

    for(i = 0; i < lt_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit, lt_id[i], &lt_info));
        if (sal_strcmp(info.table_name, lt_info->name) == 0) {
            break;
        }
    }
    if (i == lt_num) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    flds = lt_info->flds;

    if (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF_STRENGTH)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF_STRENGTH].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        if (value != 0) {
            cfg->fld_pre_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF;
        }
    }

    if (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI_STRENGTH)) {
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI_STRENGTH].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        if (value != 0) {
            cfg->fld_pre_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_VFI;
        }
    }

    if (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_SVP)) {
        /* 0 is not a valid value for SVP. */
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_SVP].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        if (value != 0) {
            cfg->fld_pre_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP;
        }
    }

    if (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_STRENGTH_PROFILE_INDEX)) {
        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_STRENGTH_PROFILE_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth[i], ent_type, &stren));
        if (stren != value) {
            cfg->fld_pre_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the ing xlate stren profile entry type.
 *
 * This function is used to get the ing xlate table stren profile entry type.
 *
 * \param [in] unit           Unit number.
 * \param [in] fld_bmp        bmp of BCMI_LTSW_VLAN_ING_XLATE_FLD_***.
 * \param [out] ent_type      Entry type.
 *
 * \retval SHR_E_NONE            No errors.
 */
static int
vlan_ing_xlate_sbr_profile_ent_type_get(
    int unit, uint32_t fld_bmp, bcmi_ltsw_sbr_profile_ent_type_t *ent_type)
{

    /* Vfi is always together with tag_action.*/
    if (fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_VFI) {
        fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS;
    }

    if ((fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF) &&
        (fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP) &&
        (fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS)) {
        *ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF;
        return SHR_E_NONE;
    }

    if ((fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS) &&
        (fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP)) {
        *ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF;
        return SHR_E_NONE;
    }

    if ((fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF) &&
        (fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS)) {
        *ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP;
        return SHR_E_NONE;
    }

    if ((fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF) &&
        (fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP)) {
        return SHR_E_PARAM;
    }

    if (fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF) {
        *ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_SVP_NO_VLAN_TAG_PRESERVE_CTRL;
        return SHR_E_NONE;
    }

    if (fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS) {
        *ent_type = BCMI_LTSW_SBR_PET_NO_L2_IIF_NO_L3_IIF_NO_SVP;
        return SHR_E_NONE;
    }

    if (fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP) {
        return SHR_E_PARAM;
    }

    return SHR_E_PARAM;
}

/*!
 * \brief Set the key of ingress VLAN translation entry.
 *
 * This function is used to set the key of ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port group ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] ent_hdl          LT entry container.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_key_set(int unit,
                             bcm_vlan_translate_key_t key_type,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcmlt_entry_handle_t *ent_hdl)
{
    int dunit = 0;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    switch (key_type) {
    case bcmVlanTranslateKeyPortGroupDouble:
        /* Allocate entry container and set key for dtag. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit,
                 BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE, &lt_info));
        flds = lt_info->flds;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_info->name, ent_hdl));

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OVID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*ent_hdl, fld_name, outer_vlan));

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_IVID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*ent_hdl, fld_name, inner_vlan));

        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*ent_hdl, fld_name, port));
        break;
    case bcmVlanTranslateKeyPortGroupOuter:
        /* Allocate entry container and set key for otag. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit,
                 BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE, &lt_info));
        flds = lt_info->flds;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_info->name, ent_hdl));

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OVID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*ent_hdl, fld_name, outer_vlan));

        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*ent_hdl, fld_name, port));
        break;
    case bcmVlanTranslateKeyPortGroupInner:
        /* Allocate entry container and set key for itag. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit,
                 BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE, &lt_info));
        flds = lt_info->flds;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_info->name, ent_hdl));

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_IVID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*ent_hdl, fld_name, inner_vlan));

        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*ent_hdl, fld_name, port));

        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the key of egress VLAN translation entry.
 *
 * This function is used to set the key of egress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port group ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] ent_hdl          LT entry container.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_key_set(int unit,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcmlt_entry_handle_t *ent_hdl)
{
    int dunit = 0;
    uint32_t vfi = 0;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    bcmint_vlan_lt_id_t lt_id = VLAN_EGR_XLATE_DW_EN(unit) ?
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW :
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id, &lt_info));
    flds = lt_info->flds;

    /* Allocate entry container. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->name, ent_hdl));

    /* Set port group ID. */
    fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_GROUP_ID].name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*ent_hdl, fld_name, port));

    /* Get VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, outer_vlan, &vfi));
    fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VFI].name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*ent_hdl, fld_name, vfi));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the key of ingress VLAN translation entry.
 *
 * This function is used to get the key of ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [out] port             Port group ID.
 * \param [out] outer_vlan       Outer VLAN ID.
 * \param [out] inner_vlan       Inner VLAN ID.
 * \param [out] pipe             Pipe.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_key_get(int unit,
                             bcmlt_entry_handle_t ent_hdl,
                             bcm_vlan_translate_key_t key_type,
                             bcm_gport_t *port,
                             bcm_vlan_t *outer_vlan,
                             bcm_vlan_t *inner_vlan,
                             int *pipe)
{
    uint64_t value = 0;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;

    SHR_FUNC_ENTER(unit);

    switch (key_type) {
    case bcmVlanTranslateKeyPortGroupDouble:
        /* Get key fields for dtag. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit,
                 BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE, &lt_info));
        flds = lt_info->flds;
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OVID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        *outer_vlan = (bcm_vlan_t)value;

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_IVID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        *inner_vlan = (bcm_vlan_t)value;

        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        *port = (bcm_gport_t) value;
        if (pipe) {
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_PIPE].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
            *pipe = (bcm_gport_t) value;
        }
        break;
    case bcmVlanTranslateKeyPortGroupOuter:
        /* Get key fields for otag. */
         SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit,
                 BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE, &lt_info));
        flds = lt_info->flds;
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OVID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        *outer_vlan = (bcm_vlan_t)value;

        *inner_vlan = BCM_VLAN_INVALID;

        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        *port = (bcm_gport_t) value;
        if (pipe) {
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_PIPE].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
            *pipe = (bcm_gport_t) value;
        }
        break;
    case bcmVlanTranslateKeyPortGroupInner:
        /* Get key fields for itag. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit,
                 BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE, &lt_info));
        flds = lt_info->flds;
        *outer_vlan = BCM_VLAN_INVALID;

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_IVID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        *inner_vlan = (bcm_vlan_t)value;

        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF_PORT_GROUP_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        *port = (bcm_gport_t) value;
        if (pipe) {
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_PIPE].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
            *pipe = (bcm_gport_t) value;
        }
        break;
    default:
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the key of egress VLAN translation entry.
 *
 * This function is used to get the key of egress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container.
 * \param [out] port             Port group ID.
 * \param [out] outer_vlan       Outer VLAN ID.
 * \param [out] inner_vlan       Inner VLAN ID.
 * \param [out] pipe             Pipe.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_key_get(int unit,
                             bcmlt_entry_handle_t ent_hdl,
                             bcm_gport_t *port,
                             bcm_vlan_t *outer_vlan,
                             bcm_vlan_t *inner_vlan,
                             int *pipe)
{
    uint64_t value = 0;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    bcmint_vlan_lt_id_t lt_id = VLAN_EGR_XLATE_DW_EN(unit) ?
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW :
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id, &lt_info));
    flds = lt_info->flds;

    fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_GROUP_ID].name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
    *port = (bcm_gport_t)value;

    fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VFI].name;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, fld_name, &value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vpnid_get(unit, value, outer_vlan));

    *inner_vlan = BCM_VLAN_INVALID;
    if (pipe) {
        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_PIPE].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        *pipe = (bcm_gport_t) value;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the fields of ingress VLAN translation entry.
 *
 * This function is used to set the fields of ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_fields_set(int unit,
                                bcmlt_entry_handle_t ent_hdl,
                                bcm_vlan_translate_key_t key_type,
                                bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    uint8_t ctrl = 0;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type;
    bcmi_ltsw_sbr_tbl_hdl_t th;
    bcmi_ltsw_sbr_fld_type_t fld_type;
    int stren;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;

    SHR_FUNC_ENTER(unit);

    switch (key_type) {
        case bcmVlanTranslateKeyPortGroupDouble:
            pth = BCMI_LTSW_SBR_PTH_VLAN_XLATE_3;
            th = BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_3_TABLE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_vlan_lt_get(unit,
                     BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE, &lt_info));
            break;
        case bcmVlanTranslateKeyPortGroupOuter:
            pth = BCMI_LTSW_SBR_PTH_VLAN_XLATE_1;
            th = BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_1_TABLE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_vlan_lt_get(unit,
                     BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE, &lt_info));
            break;
        case bcmVlanTranslateKeyPortGroupInner:
            pth = BCMI_LTSW_SBR_PTH_VLAN_XLATE_2;
            th = BCMI_LTSW_SBR_TH_ING_VLAN_XLATE_2_TABLE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_vlan_lt_get(unit,
                     BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE, &lt_info));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    flds = lt_info->flds;

    /* Set L2 forwarding domain. */
    if (cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_VFI) {
        if (cfg->vfi == 0) {
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI_STRENGTH].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, 0));
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, 0));
        } else {
            fld_type = BCMI_LTSW_SBR_FT_VFI;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_sbr_fld_value_get(unit, th, fld_type, &stren));
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI_STRENGTH].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, (uint64_t)stren));
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->vfi));
            cfg->fld_pre_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_VFI;
        }
    }

    /* Set L3 ingress interface. */
    if (cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF) {
        if (cfg->l3_iif == 0) {
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF_STRENGTH].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, 0));
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, 0));
        } else {
            fld_type = BCMI_LTSW_SBR_FT_L3_IIF;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_sbr_fld_value_get(unit, th, fld_type, &stren));
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF_STRENGTH].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl,
                                       fld_name, (uint64_t)stren));
            fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->l3_iif));
            cfg->fld_pre_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF;
        }
    }

    /* Set L2 ingress interface. */
    if (cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_L2_IIF) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->l2_iif));
    }

    /* Set SVP. */
    if (cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_SVP].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->svp));
        cfg->fld_pre_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP;
    }

    /*
     * Set VLAN tag preserve control.
     * vlan_tag_preserve_ctrl[0]: preserve incoming OTAG.
     * vlan_tag_preserve_ctrl[1]: preserve incoming ITAG.
     */
    if (cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS) {
        ctrl = ((cfg->itag_action == bcmVlanActionDelete) ? 0 : 1) << 1 |
               ((cfg->otag_action == bcmVlanActionDelete) ? 0 : 1);

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_VLAN_TAG_PRESERVE_CTRL].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, ctrl));
        cfg->fld_pre_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS;
    }

    if (cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_STR_PROFILE_IDX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_sbr_profile_ent_type_get(unit, cfg->fld_pre_bmp,
                                                     &ent_type));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &stren));
        fld_name
            = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_STRENGTH_PROFILE_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld_name, (uint64_t)stren));
    }

    /* Set opaque control id. */
    if (cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID_1) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID_1].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->opaque_ctrl_id_1));
    }

    if (cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->opaque_ctrl_id));
    }

    if ((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION) &&
        (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_FLEX_CTR_ACTION))) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_FLEX_CTR_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->flex_ctr_action));
    }

    if ((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0) &&
        (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPQ_OBJ_0))) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPQ_OBJ_0].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->opaque_obj0));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the fields of egress VLAN translation entry.
 *
 * This function is used to set the fields of egress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container.
 * \param [in] cfg               Egress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_fields_set(int unit,
                                bcmlt_entry_handle_t ent_hdl,
                                ltsw_vlan_egr_xlate_cfg_t *cfg)
{
    uint16_t vlan = 0;
    uint8_t action = 0;
    int stren;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = VLAN_EGR_XLATE_DW_EN(unit) ?
        BCMI_LTSW_SBR_PTH_EGR_VXLT_DW : BCMI_LTSW_SBR_PTH_EGR_VXLT;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_DEF;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    bcmint_vlan_lt_id_t lt_id = VLAN_EGR_XLATE_DW_EN(unit) ?
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW :
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id, &lt_info));
    flds = lt_info->flds;

    /* Set outer tag. */
    if (cfg->fld_bmp & EVT_FLD_OTAG) {
        vlan = VLAN_CTRL(cfg->outer_pri, cfg->outer_cfi, cfg->outer_vlan);

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_0].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, vlan));
    }

    /* Set inner tag. */
    if (cfg->fld_bmp & EVT_FLD_ITAG) {
        vlan = VLAN_CTRL(cfg->inner_pri, cfg->inner_cfi, cfg->inner_vlan);

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_1].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, vlan));
    }

    /*
     * Set VLAN tag action.
     * tag_action[0]: otag_action = 0: delete, 1: add/replace;
     * tag_action[1]: itag_action = 0: delete, 1: add/replace;
     * tag_action[2]: odot1p -- 0: adhere to tag_action[0],
     *                          1: (copy_mode) preserve packet's odot1p;
     * tag_action[3]: Reserved;
     */
    if ((cfg->fld_bmp & EVT_FLD_OTAG_ACTION) ||
        (cfg->fld_bmp & EVT_FLD_ITAG_ACTION)) {
        action = ((cfg->itag_action == bcmVlanActionAdd) ? 1 : 0) << 1 |
                  ((cfg->otag_action == bcmVlanActionAdd) ? 1 : 0);
        /* Preserve pkt's odot1p by default. Egr port takes final control. */
        action |= (1 << 2);

        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_TAG_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, action));
    }

    /* Set base pointer for DOT1P remarking lookup used by QoS remarking. */
    if (cfg->fld_bmp & EVT_FLD_DOT1P_MAP_ID) {
        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_DOT1P_REMARK_BASE_PTR].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->dot1p_map_id));
    }

    if (cfg->fld_bmp & EVT_FLD_STR_PROFILE_IDX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &stren));
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_STRENGTH_PRFL_IDX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, (uint64_t)stren));
    }

    if (cfg->fld_bmp & EVT_FLD_OPAQUE_CTRL) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_OPAQ_CTRL].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->opaque_ctrl));
    }

    if ((cfg->fld_bmp & EVT_FLD_FLEX_CTR_ACTION) &&
        (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_ACTION))) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->flex_ctr_action));
    }

    if ((cfg->fld_bmp & EVT_FLD_FLEX_CTR_INDEX) &&
        (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_INDEX))) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(ent_hdl, fld_name, cfg->flex_ctr_index));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the fields of ingress VLAN translation entry.
 *
 * This function is used to get the fields of ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [out] cfg              Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_fields_get(int unit,
                                bcmlt_entry_handle_t ent_hdl,
                                bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    uint64_t value = 0;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;

    SHR_FUNC_ENTER(unit);

    /* Fields of ING_VLAN_XLATE_x_TABLE are same, so pick one to get lt_info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit,
             BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE, &lt_info));
    flds = lt_info->flds;

    /* Get L2 forwarding domain. */
    if ((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_VFI) ||
        (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_VFI].name;
        if(fld_name || cfg->fld_bmp) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
            cfg->vfi = (int)value;
        } else {
            cfg->vfi = 0;
        }
    }

    /* Get L3 ingress interface. */
    if ((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF) ||
        (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L3_IIF].name;
        if(fld_name || cfg->fld_bmp) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
            cfg->l3_iif = (int)value;
        } else {
            cfg->l3_iif = 0;
        }
    }

    /* Get L2 ingress interface. */
    if ((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_L2_IIF) ||
        (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_L2_IIF].name;
        if(fld_name || cfg->fld_bmp) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
            cfg->l2_iif = (int)value;
        } else {
            cfg->l2_iif = 0;
        }
    }

    /* Get SVP. */
    if ((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP) ||
        (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_SVP].name;
        if(fld_name || cfg->fld_bmp) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
            cfg->svp = (int)value;
        } else {
            cfg->svp = 0;
        }
    }

    /* Get VLAN actions per tag preserve control. */
    if ((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS) ||
        (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_VLAN_TAG_PRESERVE_CTRL].name;
        if(fld_name || cfg->fld_bmp) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
            cfg->otag_action=
                (value & 0x1) ? bcmVlanActionNone : bcmVlanActionDelete;
            cfg->itag_action =
                ((value >> 1) & 0x1) ? bcmVlanActionNone : bcmVlanActionDelete;
        } else {
            cfg->otag_action = bcmVlanActionNone;
            cfg->itag_action = bcmVlanActionNone;
        }
    }

    /* Get VLAN pre config info from strength. */
    if (cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_PRE_CONFIG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_entry_pre_config_get(unit, ent_hdl, cfg));
    }

    if ((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID) ||
        (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->opaque_ctrl_id = (int)value;
    }

    /* Get opaque control id. */
    if ((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID_1) ||
        (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPAQUE_CTRL_ID_1].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->opaque_ctrl_id_1 = (int)value;
    }

    if (((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION) ||
        (cfg->fld_bmp == 0)) &&
        (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_FLEX_CTR_ACTION))) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_FLEX_CTR_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->flex_ctr_action = (uint32_t)value;
    }

    if (((cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0) ||
        (cfg->fld_bmp == 0)) &&
        (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPQ_OBJ_0))) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPQ_OBJ_0].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->opaque_obj0 = (uint32_t)value;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the fields of egress VLAN translation entry.
 *
 * This function is used to get the fields of egress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] ent_hdl           LT entry container..
 * \param [out] cfg              Egress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_fields_get(int unit,
                                bcmlt_entry_handle_t ent_hdl,
                                ltsw_vlan_egr_xlate_cfg_t *cfg)
{
    uint64_t value = 0;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    bcmint_vlan_lt_id_t lt_id = VLAN_EGR_XLATE_DW_EN(unit) ?
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW :
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id, &lt_info));
    flds = lt_info->flds;

    /* Get outer tag. */
    if ((cfg->fld_bmp & EVT_FLD_OTAG) || (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_0].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->outer_pri = VLAN_CTRL_PRIO(value);
        cfg->outer_cfi = VLAN_CTRL_CFI(value);
        cfg->outer_vlan = VLAN_CTRL_ID(value);
    }

    /* Get inner tag. */
    if ((cfg->fld_bmp & EVT_FLD_ITAG) || (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_VLAN_1].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->inner_pri = VLAN_CTRL_PRIO(value);
        cfg->inner_cfi = VLAN_CTRL_CFI(value);
        cfg->inner_vlan = VLAN_CTRL_ID(value);
    }

    /* Get VLAN tag action. */
    if ((cfg->fld_bmp & EVT_FLD_OTAG_ACTION) ||
        (cfg->fld_bmp & EVT_FLD_ITAG_ACTION) ||
        (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_TAG_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        if ((cfg->fld_bmp & EVT_FLD_OTAG_ACTION) || (cfg->fld_bmp == 0)) {
            cfg->otag_action =
                (value & 0x1) ? bcmVlanActionAdd: bcmVlanActionNone;
        }
        if ((cfg->fld_bmp & EVT_FLD_ITAG_ACTION) || (cfg->fld_bmp == 0)) {
            cfg->itag_action =
                ((value >> 1) & 0x1) ? bcmVlanActionAdd : bcmVlanActionNone;
        }
    }

    /* Get DOT1P map ID. */
    if ((cfg->fld_bmp & EVT_FLD_DOT1P_MAP_ID) || (cfg->fld_bmp == 0)) {
        fld_name =
            flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_DOT1P_REMARK_BASE_PTR].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->dot1p_map_id = (int)value;
    }
    if ((cfg->fld_bmp & EVT_FLD_OPAQUE_CTRL) || (cfg->fld_bmp == 0)) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_OPAQ_CTRL].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->opaque_ctrl = (int)value;
    }

    if (((cfg->fld_bmp & EVT_FLD_FLEX_CTR_ACTION) || (cfg->fld_bmp == 0)) &&
        (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_ACTION))) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_ACTION].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->flex_ctr_action = (uint32_t)value;
    }

    if (((cfg->fld_bmp & EVT_FLD_FLEX_CTR_INDEX) || (cfg->fld_bmp == 0)) &&
        (lt_info->fld_bmp & (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_INDEX))) {
        fld_name = flds[BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_INDEX].name;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(ent_hdl, fld_name, &value));
        cfg->flex_ctr_index = (uint32_t)value;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Generate xlate key hash.
 *
 * This function is used to generate xlate key hash.
 *
 * \param [in] ovid             Outer vlan id.
 * \param [in] ivid             Inner vlan id.
 *
 * \retval hash value.
 */
static uint16_t
vlan_xlate_key_hash(uint16_t ovid, uint16_t ivid)
{
    return (ovid + ivid + 2) % VLAN_HASH_NUM;
}

/*!
 * \brief Compare xlate key.
 *
 * This function is used to compare xlate key.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key type.
 * \param [in] port_1/2          Port.
 * \param [in] ovid_1/2          Outer vlan id.
 * \param [in] ivid_1/2          Inner vlan id.
 *
 * \retval Ture/False
 */
static int
vlan_xlate_key_equal(int unit, bcm_vlan_translate_key_t key_type,
                       bcm_gport_t port_1, bcm_gport_t port_2,
                       bcm_vlan_t ovid_1, bcm_vlan_t ovid_2,
                       bcm_vlan_t ivid_1, bcm_vlan_t ivid_2)
{
   int rv;

   switch (key_type) {
        case bcmVlanTranslateKeyPortGroupDouble:
            rv = (port_1 == port_2) &&
                 (ovid_1 == ovid_2) &&
                 (ivid_1 == ivid_2);
            break;
        case bcmVlanTranslateKeyPortGroupInner:
            rv = (port_1 == port_2) &&
                 (ivid_1 == ivid_2);
            break;
        case bcmVlanTranslateKeyPortGroupOuter:
        case bcmVlanTranslateKeyInvalid:
        default:
            rv = (port_1 == port_2) &&
                 (ovid_1 == ovid_2);
            break;
    }

   return (rv ? TRUE : FALSE);
}

/*!
 * \brief Operate on xlate tables per pipe.
 *
 * This function is used to operate on xlate tables per pipe.
 *
 * \param [in] unit              Unit number.
 * \param [in] tid               Table id.
 * \param [in] pipe_support      If per pipe config is supported.
 * \param [in] op                Operation code.
 * \param [in] pipe_bmp          Pipe bitmap if per pipe config is supported.
 * \param [in] ent_hdl           Entry handle.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_xlate_entry_pipe_op(int unit,
                         vlan_xlate_table_id_t tid,
                         int pipe_support,
                         bcmlt_opcode_t op,
                         uint32_t pipe_bmp,
                         bcmlt_entry_handle_t ent_hdl)
{
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    bcmint_vlan_lt_id_t lt_id;
    int i = 0, j = 0, fld;
    SHR_FUNC_ENTER(unit);

    if (pipe_support) {
        /* Try to get field name of pipe here*/
        lt_id = (tid >= XLATE_TABLE_EGR) ?
                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION :
                BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE;
         fld = (tid >= XLATE_TABLE_EGR) ?
               BCMINT_LTSW_VLAN_FLD_EGR_XLATE_PIPE :
               BCMINT_LTSW_VLAN_FLD_ING_XLATE_PIPE;
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmint_vlan_lt_get(unit, lt_id, &lt_info));
          fld_name = lt_info->flds[fld].name;

          for (i = 0; i <= bcmi_ltsw_dev_pp_pipe_max(unit); i++) {
              if (pipe_bmp & (1 << i)) {
                  SHR_IF_ERR_EXIT_EXCEPT_IF
                      (bcmlt_entry_field_remove(ent_hdl, fld_name),
                       SHR_E_NOT_FOUND);
                  SHR_IF_ERR_VERBOSE_EXIT
                      (bcmlt_entry_field_add(ent_hdl, fld_name, i));
                  SHR_IF_ERR_VERBOSE_EXIT
                      (bcmi_lt_entry_commit(unit, ent_hdl, op,
                                            BCMLT_PRIORITY_NORMAL));
                  if (op == BCMLT_OPCODE_LOOKUP) {
                      SHR_EXIT();
                  }
            }
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, ent_hdl, op, BCMLT_PRIORITY_NORMAL));
    }
exit:
    /* Roll back on INSERT op. */
    if (SHR_FUNC_ERR() && i && pipe_support && (op == BCMLT_OPCODE_INSERT)) {
        for (j = 0; j < i; j++) {
            if (pipe_bmp & (1 << j)) {
                (void)bcmlt_entry_field_remove(ent_hdl, fld_name);
                (void)bcmlt_entry_field_add(ent_hdl, fld_name, j);
                (void)bcmi_lt_entry_commit(unit, ent_hdl, BCMLT_OPCODE_DELETE,
                                           BCMLT_PRIORITY_NORMAL);
            }
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if port group is dvp port group.
 *
 * \param [in]  unit              Unit number.
 * \param [in]  pg                Port group id.
 * \param [out] is_dvp_pg         Is dvp port group.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_dvp_port_group_id_check(int unit, bcm_port_t pg, int *is_dvp_pg)
{
    bcm_port_group_range_t range;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_group_range_get(unit, bcmPortGroupDvp, &range));

    if (pg <= range.max && pg >= range.min) {
        *is_dvp_pg = 1;
    } else {
        *is_dvp_pg = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the pipe info of port group.
 *
 * This function is used to get the pipe info of port group.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key type.
 * \param [in] port_group        Port group.
 * \param [in] Ingress           Ingress/egress.
 * \param [out] pipe_support     If per pipe config is supported.
 * \param [out] tid              Table id.
 * \param [out] pipe_bmp         Pipe bitmap if per pipe config is supported.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_xlate_entry_pipe_get(int unit,
                          bcm_vlan_translate_key_t key_type,
                          bcm_gport_t port_group,
                          int ingress,
                          int *pipe_support,
                          vlan_xlate_table_id_t *tid,
                          uint16_t *pipe_bmp)
{
    uint8_t bmp = ltsw_vlan_info[unit].table_bmp;
    ltsw_vlan_pg_info_t *pg_info;
    int flags, index = 0;
    uint32_t pbmp_get = 0;
    int sw_support = 0, is_dvp_pg = 0;
    vlan_xlate_table_id_t egr_xlate = VLAN_EGR_XLATE_DW_EN(unit) ?
                                      XLATE_TABLE_EGR_DW : XLATE_TABLE_EGR;

    SHR_FUNC_ENTER(unit);
    *pipe_support = 0;

    if (ingress) {
        switch (key_type) {
            case bcmVlanTranslateKeyPortGroupDouble:
                if (bmp & 1 << XLATE_TABLE_ING_3){
                    *pipe_support = 1;
                }
                *tid = XLATE_TABLE_ING_3;
                break;
            case bcmVlanTranslateKeyPortGroupOuter:
                if (bmp & 1 << XLATE_TABLE_ING_1){
                    *pipe_support = 1;
                }
                *tid = XLATE_TABLE_ING_1;
                break;
            case bcmVlanTranslateKeyPortGroupInner:
                if (bmp & 1 << XLATE_TABLE_ING_2){
                    *pipe_support = 1;
                }
                *tid = XLATE_TABLE_ING_2;
                break;
            default:
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                break;
        }
    } else {
        if (bmp & 1 << egr_xlate){
            *pipe_support = 1;
        }
        *tid = egr_xlate;
        index = 1;
    }

    sw_support = ingress ?
        VLAN_ING_XLATE_CTRL_EN(unit): VLAN_EGR_XLATE_CTRL_EN(unit);

    if ((*pipe_support || sw_support) && pipe_bmp) {
        pg_info =
            &(ltsw_vlan_info[unit].xlate_pipe_ctrl[index].pg_info[port_group]);
        /*Get pipe info from vlan module first.*/
        if (pg_info->pipe_bmp) {
            *pipe_bmp = pg_info->pipe_bmp;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_dvp_port_group_id_check(unit, port_group, &is_dvp_pg));
            if (!ingress && is_dvp_pg) {
                  pbmp_get = pg_info->pipe_bmp;
            } else {
                flags = ingress ? BCMI_LTSW_PORT_ING : BCMI_LTSW_PORT_EGR;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_group_to_pipes(unit, port_group,
                                                   flags, &pbmp_get));
            }
            *pipe_bmp = (uint16_t)pbmp_get;
            /*If no pipe in this port group, install the entry into all pipes.*/
            if (*pipe_bmp == 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_dev_pp_pipe_bmp(unit, &pbmp_get));
                *pipe_bmp = (uint16_t)pbmp_get;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete xlate per pipe key info.
 *
 * This function is used to delete xlate per pipe key info.
 *
 * \param [in] unit             Unit number.
 * \param [in] port             Port group ID.
 * \param [in] tbl_id           Table id.
 * \param [in] outer_vlan       Outer VLAN ID.
 * \param [in] inner_vlan       Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_xlate_entry_pipe_info_delete(int unit,
                                  bcm_gport_t port,
                                  vlan_xlate_table_id_t tbl_id,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan)
{
    ltsw_xlate_pipe_control_t *ctrl = NULL;
    ltsw_vlan_pg_info_t *pg_info = NULL;
    uint32_t prev_i, hash_value = 0;
    int i, is_dvp_pg = 0;
    uint32_t head_zero[VLAN_HASH_NUM] = {0};
    SHR_FUNC_ENTER(unit);

    if (tbl_id >= XLATE_TABLE_EGR) {
        ctrl = &(ltsw_vlan_info[unit].xlate_pipe_ctrl[1]);
        inner_vlan = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_dvp_port_group_id_check(unit, port, &is_dvp_pg));
    } else {
        ctrl = &(ltsw_vlan_info[unit].xlate_pipe_ctrl[0]);
        if (tbl_id == XLATE_TABLE_ING_1) {
            inner_vlan = 0;
        } else if (tbl_id == XLATE_TABLE_ING_2) {
            outer_vlan = 0;
        }
    }

    /* Find out key and delete list info. */
    pg_info = &(ctrl->pg_info[port]);
    hash_value = vlan_xlate_key_hash(outer_vlan, inner_vlan);
    SHR_NULL_CHECK(pg_info->head, SHR_E_NOT_FOUND);
    prev_i = pg_info->head[hash_value];
    for (i = pg_info->head[hash_value]; i != 0; i = ctrl->next[i]) {
        if ((ctrl->key_array[i].outer_vlan == outer_vlan) &&
            (ctrl->key_array[i].inner_vlan == inner_vlan)) {
            if (prev_i != i) {
               ctrl->next[prev_i] = ctrl->next[i];
               ctrl->next[i] = ctrl->free_idx;
               ctrl->free_idx = i;
            } else {
               pg_info->head[hash_value] = ctrl->next[i];
               ctrl->next[i] = ctrl->free_idx;
               ctrl->free_idx = i;
            }
            ctrl->key_array[i].inner_vlan = 0;
            ctrl->key_array[i].outer_vlan = 0;
            ctrl->key_array[i].pg_id = 0;
            break;
        }
        prev_i = i;
    }
    if(!i) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if(!pg_info->head[hash_value]) {
        if (!sal_memcmp(pg_info->head, head_zero,
                        sizeof(uint32_t) * VLAN_HASH_NUM)) {
            SHR_FREE(pg_info->head);
            if (!is_dvp_pg) {
                pg_info->pipe_bmp = 0;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add xlate per pipe key info.
 *
 * This function is used to add xlate per pipe key info.
 *
 * \param [in] unit             Unit number.
 * \param [in] port             Port group ID.
 * \param [in] tbl_id           Table id.
 * \param [in] pipe_bmp         Pipe_bmp.
 * \param [in] outer_vlan       Outer VLAN ID.
 * \param [in] inner_vlan       Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_xlate_entry_pipe_info_add(int unit,
                               bcm_gport_t port,
                               vlan_xlate_table_id_t tbl_id,
                               uint32_t pipe_bmp,
                               bcm_vlan_t outer_vlan,
                               bcm_vlan_t inner_vlan)
{
    ltsw_xlate_pipe_control_t *ctrl = NULL;
    ltsw_vlan_pg_info_t *pg_info = NULL;
    uint32_t alloc_size = 0;
    uint32_t new_head, hash_value = 0;
    SHR_FUNC_ENTER(unit);

    if (tbl_id >= XLATE_TABLE_EGR) {
        ctrl = &(ltsw_vlan_info[unit].xlate_pipe_ctrl[1]);
        inner_vlan = 0;
    } else {
        ctrl = &(ltsw_vlan_info[unit].xlate_pipe_ctrl[0]);
        if (tbl_id == XLATE_TABLE_ING_1) {
            inner_vlan = 0;
        } else if (tbl_id == XLATE_TABLE_ING_2) {
            outer_vlan = 0;
        }
    }

    if(!ctrl->free_idx) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }

    pg_info = &(ctrl->pg_info[port]);
    if (pg_info->head == NULL) {
        alloc_size = sizeof(uint32_t) * VLAN_HASH_NUM;
        SHR_ALLOC(pg_info->head, alloc_size, "bcmVlanXlatePgHead");
        SHR_NULL_CHECK(pg_info->head, SHR_E_MEMORY);
        sal_memset(pg_info->head, 0, alloc_size);
        pg_info->pipe_bmp = pipe_bmp;
    }

    /* Add key info. */
    new_head = ctrl->free_idx;
    ctrl->key_array[new_head].outer_vlan = outer_vlan;
    ctrl->key_array[new_head].inner_vlan = inner_vlan;
    ctrl->key_array[new_head].pg_id = port;

    /* Update list info. */
    hash_value = vlan_xlate_key_hash(outer_vlan, inner_vlan);
    ctrl->free_idx = ctrl->next[new_head];
    ctrl->next[new_head] = pg_info->head[hash_value];
    pg_info->head[hash_value] = new_head;

exit:
    if (SHR_FUNC_ERR() && alloc_size) {
        SHR_FREE(pg_info->head);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add xlate per pipe key info.
 *
 * This function is used to add xlate per pipe key info.
 *
 * \param [in] unit              Unit number.
 * \param [in] flags             Ingress/egress flag.
 * \param [in] array_size        Array size.
 * \param [in] ctrl              Pipe control.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_xlate_entry_pipe_info_recover(int unit, int flags, int array_size,
                                   ltsw_xlate_pipe_control_t *ctrl)
{
    int i = 0, j;
    uint32_t pipe_bmp = 0;
    ltsw_vlan_pg_info_t *pg_info = NULL;
    bcmint_vlan_xlate_key_info_t *key = ctrl->key_array;
    uint32_t alloc_size = 0;
    uint32_t hash_value = 0;
    SHR_FUNC_ENTER(unit);

    ctrl->free_idx = 0;
    ctrl->next[0] = 0;

    for (i = 1; i < array_size; i++) {
        if (key[i].outer_vlan || key[i].inner_vlan) {
            pg_info = &(ctrl->pg_info[key[i].pg_id]);
            if(!pg_info->head) {
                /* Recover pipe bitmap.*/
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_port_group_to_pipes(unit, key[i].pg_id,
                                                   flags, &pipe_bmp));
                /*If no pipe in this port group, install into all pipes.*/
                if (pipe_bmp == 0) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_ltsw_dev_pp_pipe_bmp(unit, &pipe_bmp));
                }
                pg_info->pipe_bmp = pipe_bmp;

                /* Alloc head memory.*/
                alloc_size = sizeof(uint32_t) * VLAN_HASH_NUM;
                SHR_ALLOC(pg_info->head, alloc_size, "bcmVlanXlatePgHead");
                SHR_NULL_CHECK(pg_info->head, SHR_E_MEMORY);
                sal_memset(pg_info->head, 0, alloc_size);
            }
            /* Recover head list.*/
            hash_value = vlan_xlate_key_hash(key[i].outer_vlan,
                                             key[i].inner_vlan);
            ctrl->next[i] = pg_info->head[hash_value];
            pg_info->head[hash_value] = i;
        } else {
            /* Recover free list.*/
            ctrl->next[i] = ctrl->free_idx;
            ctrl->free_idx = i;
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        for (j = 1; j < i; j++) {
            pg_info = &(ctrl->pg_info[key[j].pg_id]);
            SHR_FREE(pg_info->head);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add ingress VLAN translation entry.
 *
 * This function is used to add ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port group ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_add(int unit,
                         bcm_vlan_translate_key_t key_type,
                         bcm_gport_t port,
                         bcm_vlan_t outer_vlan,
                         bcm_vlan_t inner_vlan,
                         bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)

{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int pipe_support = 0;
    uint16_t pipe_bmp = 0;
    vlan_xlate_table_id_t tbl_id;

    SHR_FUNC_ENTER(unit);

    VLAN_ING_XLATE_LOCK(unit);

    /* Set the key of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_key_set(unit, key_type,
                                      port, outer_vlan,
                                      inner_vlan, &ent_hdl));

    /* Set the fields of ingress vxlate entry.  */
    cfg->fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_STR_PROFILE_IDX;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_fields_set(unit, ent_hdl, key_type, cfg));

    /* Get the pg pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port,
                                   1, &pipe_support, &tbl_id, &pipe_bmp));

    /* Insert ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,

                                  BCMLT_OPCODE_INSERT, pipe_bmp, ent_hdl));
    /* Update SW info. */
    if (VLAN_ING_XLATE_CTRL_EN(unit) || pipe_support) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_xlate_entry_pipe_info_add(unit, port, tbl_id, pipe_bmp,
                                            outer_vlan, inner_vlan));
    }
exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    VLAN_ING_XLATE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update ingress VLAN translation entry.
 *
 * This function is used to update ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port group ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_update(int unit,
                            bcm_vlan_translate_key_t key_type,
                            bcm_gport_t port,
                            bcm_vlan_t outer_vlan,
                            bcm_vlan_t inner_vlan,
                            bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_vlan_ing_xlate_cfg_t cfg_get;
    int pipe_support = 0;
    uint16_t pipe_bmp = 0;
    vlan_xlate_table_id_t tbl_id;
    SHR_FUNC_ENTER(unit);

    sal_memset(&cfg_get, 0, sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));

    VLAN_ING_XLATE_LOCK(unit);

    /* Set the key of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_key_set(unit, key_type, port,
                                      outer_vlan, inner_vlan, &ent_hdl));
    /* Get the pg pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port,
                                   1, &pipe_support, &tbl_id, &pipe_bmp));
    SHR_IF_ERR_CONT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_LOOKUP, pipe_bmp, ent_hdl));

    /* There is pre configuration of this entry.*/
    if (SHR_FUNC_VAL_IS(SHR_E_NONE)) {
        /* Get pre config fields bitmap.  */
        cfg_get.fld_bmp = BCMI_LTSW_VLAN_ING_XLATE_FLD_PRE_CONFIG;
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_entry_fields_get(unit, ent_hdl, &cfg_get));
        cfg->fld_pre_bmp = cfg_get.fld_pre_bmp;

        /* Clear entry content. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(ent_hdl));
        ent_hdl = BCMLT_INVALID_HDL;

        /* Set the key of ingress vxlate entry again. */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_entry_key_set(unit, key_type, port,
                                          outer_vlan, inner_vlan, &ent_hdl));
    } else if (!SHR_FUNC_VAL_IS(SHR_E_NOT_FOUND)) {
        SHR_EXIT();
    }

    /* Set the fields of ingress vxlate entry.  */
    cfg->fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_STR_PROFILE_IDX;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_fields_set(unit, ent_hdl, key_type, cfg));

    /* Update ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_UPDATE, pipe_bmp, ent_hdl));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    VLAN_ING_XLATE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ingress VLAN translation entry.
 *
 * This function is used to get ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port group ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] cfg              Ingress xlate configuration.
 * \param [in] lock              Need lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_get(int unit,
                         bcm_vlan_translate_key_t key_type,
                         bcm_gport_t port,
                         bcm_vlan_t outer_vlan,
                         bcm_vlan_t inner_vlan,
                         bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg,
                         int lock)

{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint16_t pipe_bmp = 0;
    int pipe_support;
    vlan_xlate_table_id_t tbl_id;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_ING_XLATE_LOCK(unit);
    }

    /* Set the key of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_key_set(unit, key_type, port,
                                      outer_vlan, inner_vlan, &ent_hdl));

    /* Get the pg pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port,
                                   1, &pipe_support, &tbl_id, &pipe_bmp));
    /* Find ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_LOOKUP, pipe_bmp, ent_hdl));

    /* Get the fields of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_fields_get(unit, ent_hdl, cfg));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_ING_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress VLAN translation entry.
 *
 * This function is used to get egress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port group ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] cfg              Egress xlate configuration.
 * \param [in] lock              Need lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_get(int unit,
                         bcm_gport_t port,
                         bcm_vlan_t outer_vlan,
                         bcm_vlan_t inner_vlan,
                         ltsw_vlan_egr_xlate_cfg_t *cfg,
                         int lock)

{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint16_t pipe_bmp = 0;
    int pipe_support;
    vlan_xlate_table_id_t tbl_id;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_EGR_XLATE_LOCK(unit);
    }

    /* Set the key of egress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_key_set(unit, port,
                                      outer_vlan, inner_vlan, &ent_hdl));

    /* Get the pg pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid, port,
                                   0, &pipe_support, &tbl_id, &pipe_bmp));
    /* Find xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_LOOKUP, pipe_bmp, ent_hdl));

    /* Get the fields of egress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_fields_get(unit, ent_hdl, cfg));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_EGR_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ingress VLAN translation entry.
 *
 * This function is used to set ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_set(int unit,
                         bcm_vlan_translate_key_t key_type,
                         bcm_gport_t port,
                         bcm_vlan_t outer_vlan,
                         bcm_vlan_t inner_vlan,
                         bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)

{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int pipe_support = 0, rv;
    uint16_t pipe_bmp = 0;
    vlan_xlate_table_id_t tbl_id;

    SHR_FUNC_ENTER(unit);

    VLAN_ING_XLATE_LOCK(unit);

    /* Set the key of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_key_set(unit, key_type,
                                      port, outer_vlan,
                                      inner_vlan, &ent_hdl));

    /* Set the fields of ingress vxlate entry. */
    cfg->fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_STR_PROFILE_IDX;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_fields_set(unit, ent_hdl, key_type, cfg));

    /* Get the pg pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port,
                                   1, &pipe_support, &tbl_id, &pipe_bmp));

    /* Update/insert ingress xlate entry. */
    rv = vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_UPDATE, pipe_bmp, ent_hdl);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                      BCMLT_OPCODE_INSERT, pipe_bmp, ent_hdl));
        /* Update xlate sw control info. */
        if (VLAN_ING_XLATE_CTRL_EN(unit) || pipe_support) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_entry_pipe_info_add(unit, port, tbl_id, pipe_bmp,
                                                outer_vlan, inner_vlan));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    VLAN_ING_XLATE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set egress VLAN translation entry.
 *
 * This function is used to set egress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Egress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_set(int unit,
                         bcm_gport_t port,
                         bcm_vlan_t outer_vlan,
                         bcm_vlan_t inner_vlan,
                         ltsw_vlan_egr_xlate_cfg_t *cfg)

{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int pipe_support = 0, rv;
    uint16_t pipe_bmp = 0;
    vlan_xlate_table_id_t tbl_id;

    SHR_FUNC_ENTER(unit);
    VLAN_EGR_XLATE_LOCK(unit);

    /* Set the key of egress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_key_set(unit, port, outer_vlan,
                                      inner_vlan, &ent_hdl));

    /* Set the fields of egress vxlate entry. */
    cfg->fld_bmp |= EVT_FLD_STR_PROFILE_IDX;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_fields_set(unit, ent_hdl, cfg));

    /* Get the pg pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid, port,
                                   0, &pipe_support, &tbl_id, &pipe_bmp));
    /* Update/insert ingress xlate entry. */
    rv = vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_UPDATE, pipe_bmp, ent_hdl);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                      BCMLT_OPCODE_INSERT, pipe_bmp, ent_hdl));
        /* Update xlate sw control info. */
        if (VLAN_EGR_XLATE_CTRL_EN(unit) || pipe_support) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_entry_pipe_info_add(unit, port, tbl_id, pipe_bmp,
                                                outer_vlan, inner_vlan));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    VLAN_EGR_XLATE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete ingress VLAN translation entry.
 *
 * This function is used to delete ingress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port group ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_delete(int unit,
                            bcm_vlan_translate_key_t key_type,
                            bcm_gport_t port,
                            bcm_vlan_t outer_vlan,
                            bcm_vlan_t inner_vlan)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint16_t pipe_bmp = 0;
    int pipe_support;
    vlan_xlate_table_id_t tbl_id;

    SHR_FUNC_ENTER(unit);

    VLAN_ING_XLATE_LOCK(unit);

    /* Set the key of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_key_set(unit, key_type, port,
                                      outer_vlan, inner_vlan, &ent_hdl));
    /* Get the pg pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port,
                                   1, &pipe_support, &tbl_id, &pipe_bmp));
    /* Delete ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_DELETE, pipe_bmp, ent_hdl));

    if (VLAN_ING_XLATE_CTRL_EN(unit) || pipe_support) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_xlate_entry_pipe_info_delete(unit, port, tbl_id,
                                               outer_vlan, inner_vlan));
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    VLAN_ING_XLATE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete egress VLAN translation entry.
 *
 * This function is used to delete egress VLAN translation entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port group ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_delete(int unit,
                            bcm_gport_t port,
                            bcm_vlan_t outer_vlan,
                            bcm_vlan_t inner_vlan)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint16_t pipe_bmp = 0;
    int pipe_support;
    vlan_xlate_table_id_t tbl_id;

    SHR_FUNC_ENTER(unit);
    VLAN_EGR_XLATE_LOCK(unit);

    /* Set the key of egress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_key_set(unit, port,
                                      outer_vlan, inner_vlan, &ent_hdl));

    /* Get the pg pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid, port,
                                   0, &pipe_support, &tbl_id, &pipe_bmp));
    /* Delete egress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_DELETE, pipe_bmp, ent_hdl));
    /* Update xlate sw control info. */
    if (VLAN_EGR_XLATE_CTRL_EN(unit) || pipe_support) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_xlate_entry_pipe_info_delete(unit, port, tbl_id,
                                               outer_vlan, inner_vlan));
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    VLAN_EGR_XLATE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse over all entries in ingress translation table.
 *
 * This function is used to traverse over all ingress translation entries and
 * call parse function.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Call back function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_traverse(int unit,
                              bcmi_ltsw_vlan_ing_xlate_traverse_cb cb,
                              void *user_data)
{
    bcm_vlan_translate_key_t key_type[] = {bcmVlanTranslateKeyPortGroupOuter,
                                           bcmVlanTranslateKeyPortGroupInner,
                                           bcmVlanTranslateKeyPortGroupDouble};
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmint_vlan_lt_id_t lt_id[] = {BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE,
                                   BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE,
                                   BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE};
    const bcmint_vlan_lt_t *lt_info;
    bcmi_ltsw_vlan_ing_xlate_cfg_t cfg[2];
    bcm_vlan_t outer_vlan[2];
    bcm_vlan_t inner_vlan[2];
    bcm_gport_t port[2];
    int rv[2], rv_cb = SHR_E_NONE;
    int dunit = 0;
    int i = 0, num_pipe, num, num_tmp, alloc_size;
    int pipe_support = 0, *pipe_ptr[2] = {NULL, NULL}, pipe[2];
    uint32_t pipe_bmp = 0;
    uint32_t *pipe_mask = NULL;
    vlan_xlate_table_id_t tbl_id;

    SHR_FUNC_ENTER(unit);
    VLAN_ING_XLATE_LOCK(unit);

    sal_memset(cfg, 0, 2 * sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));

    num_pipe = bcmi_ltsw_dev_pp_pipe_max(unit) + 1;
    alloc_size = sizeof(uint32_t) * num_pipe;
    SHR_ALLOC(pipe_mask, alloc_size, "xlate pipe mask");
    SHR_NULL_CHECK(pipe_mask, SHR_E_MEMORY);
    sal_memset(pipe_mask, 0, alloc_size);
    for (i = 0; i < num_pipe; i++) {
        pipe_mask[i] = (1 << i) - 1;
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    for (i = 0; i < COUNTOF(key_type); i++) {

        /* Get if pipe is supported in this table.*/
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_xlate_entry_pipe_get(unit, key_type[i], BCM_PORT_INVALID,
                                       1, &pipe_support, &tbl_id, NULL));
        if (pipe_support) {
            pipe_ptr[0] = &pipe[0];
            pipe_ptr[1] = &pipe[1];
        }

        /* Read one entry and save info in advance.
         * The customer callback function could be delete and will delete at most
         * num_pipe entries at one time in pipe mode.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit, lt_id[i], &lt_info));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));
        rv[0] = bcmi_lt_entry_commit(unit, ent_hdl,
                    BCMLT_OPCODE_TRAVERSE, BCMLT_PRIORITY_NORMAL);
        if (rv[0] == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_xlate_entry_key_get(unit, ent_hdl, key_type[i],
                                              &port[0], &outer_vlan[0],
                                              &inner_vlan[0], pipe_ptr[0]));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_xlate_entry_fields_get(unit, ent_hdl, &cfg[0]));
        }
        num = 0;

        while (rv[num] == SHR_E_NONE) {

            if (pipe_support) {
                pipe_bmp = VLAN_ING_XLATE_PG_PIPE(unit, port[num]);
                /* Skip duplicate entries across pipes.
                 * Skip entries already deleted in previous traverse.
                 */
                if (pipe_bmp & pipe_mask[*pipe_ptr[num]]) {

                    /* Get next, save info. */
                    num = num ? 0 : 1;
                    rv[num] = bcmi_lt_entry_commit(unit, ent_hdl,
                         BCMLT_OPCODE_TRAVERSE, BCMLT_PRIORITY_NORMAL);
                    if (rv[num]) {
                        break;
                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (vlan_ing_xlate_entry_key_get(unit, ent_hdl,
                             key_type[i], &port[num], &outer_vlan[num],
                             &inner_vlan[num], pipe_ptr[num]));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (vlan_ing_xlate_entry_fields_get(unit,
                                                         ent_hdl, &cfg[num]));
                    continue;
                }
            }

            /* Get next, save info. */
            num_tmp = num;
            num = num ? 0 : 1;

            /* Skip those with same key but different pipe.*/
            while ((rv[num] = bcmi_lt_entry_commit(unit, ent_hdl,
                                  BCMLT_OPCODE_TRAVERSE,
                                  BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_ing_xlate_entry_key_get(unit, ent_hdl, key_type[i],
                                                  &port[num], &outer_vlan[num],
                                                  &inner_vlan[num], pipe_ptr[num]));
                if (!vlan_xlate_key_equal(unit, key_type[i],
                                          port[0], port[1],
                                          outer_vlan[0], outer_vlan[1],
                                          inner_vlan[0], inner_vlan[1])) {
                    break;
                }
            }

            if (!rv[num]) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_ing_xlate_entry_fields_get(unit, ent_hdl, &cfg[num]));
            }
            rv_cb = cb(unit, key_type[i], port[num_tmp],
                       outer_vlan[num_tmp], inner_vlan[num_tmp],
                       &cfg[num_tmp], user_data);
            if (SHR_FAILURE(rv_cb) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv_cb);
            }
        }

        if ((rv[num] != SHR_E_NOT_FOUND) && SHR_FAILURE(rv[num])) {
            SHR_IF_ERR_VERBOSE_EXIT(rv[num]);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(ent_hdl));
        ent_hdl = BCMLT_INVALID_HDL;
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FREE(pipe_mask);
    VLAN_ING_XLATE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse over all entries in egress translation table.
 *
 * This function is used to traverse over all egress translation entries and
 * call parse function.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Call back function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_traverse(int unit,
                              ltsw_vlan_egr_xlate_traverse_cb cb,
                              void *user_data)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    ltsw_vlan_egr_xlate_cfg_t cfg[2];
    bcm_vlan_t outer_vlan[2];
    bcm_vlan_t inner_vlan[2];
    bcm_gport_t port[2];
    int rv[2], rv_cb = SHR_E_NONE;
    int dunit = 0;
    int i = 0, num_pipe, alloc_size, num, num_tmp;
    int pipe_support = 0, *pipe_ptr[2] = {NULL, NULL}, pipe[2];
    uint32_t pipe_bmp = 0;
    uint32_t *pipe_mask = NULL;
    vlan_xlate_table_id_t tbl_id;
    const bcmint_vlan_lt_t *lt_info;
    bcmint_vlan_lt_id_t lt_id = VLAN_EGR_XLATE_DW_EN(unit) ?
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW :
                                BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION;

    SHR_FUNC_ENTER(unit);
    VLAN_EGR_XLATE_LOCK(unit);

    sal_memset(cfg, 0, 2 * sizeof(ltsw_vlan_egr_xlate_cfg_t));

    num_pipe = bcmi_ltsw_dev_pp_pipe_max(unit) + 1;
    alloc_size = sizeof(uint32_t) * num_pipe;
    SHR_ALLOC(pipe_mask, alloc_size, "xlate pipe mask");
    SHR_NULL_CHECK(pipe_mask, SHR_E_MEMORY);
    sal_memset(pipe_mask, 0, alloc_size);
    for (i = 0; i < num_pipe; i++) {
        pipe_mask[i] = (1 << i) -1;
    }
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid,
                                   BCM_PORT_INVALID,
                                   0, &pipe_support, &tbl_id, NULL));
    if (pipe_support) {
        pipe_ptr[0] = &pipe[0];
        pipe_ptr[1] = &pipe[1];
    }

    /* Read one entry and save info in advance.
     * The customer callback function could be delete and will delete at most
     * num_pipe entries at one time in pipe mode.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id, &lt_info));
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));
    rv[0] = bcmi_lt_entry_commit(unit, ent_hdl,
                BCMLT_OPCODE_TRAVERSE, BCMLT_PRIORITY_NORMAL);
    if (rv[0] == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_xlate_entry_key_get(unit, ent_hdl,
                                          &port[0], &outer_vlan[0],
                                          &inner_vlan[0], pipe_ptr[0]));
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_xlate_entry_fields_get(unit, ent_hdl, &cfg[0]));
    }
    num = 0;

    while (rv[num] == SHR_E_NONE) {

        if (pipe_support) {
            pipe_bmp = VLAN_EGR_XLATE_PG_PIPE(unit, port[num]);
            /* Skip duplicate entries across pipes.
             * Skip entries already deleted in previous traverse.
             */
            if (pipe_bmp & pipe_mask[*pipe_ptr[num]]) {

                /* Get next, save info. */
                num = num ? 0 : 1;
                rv[num] = bcmi_lt_entry_commit(unit, ent_hdl,
                                               BCMLT_OPCODE_TRAVERSE,
                                               BCMLT_PRIORITY_NORMAL);
                if (rv[num]) {
                    break;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_egr_xlate_entry_key_get(unit, ent_hdl,
                                              &port[num], &outer_vlan[num],
                                              &inner_vlan[num], pipe_ptr[num]));
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_egr_xlate_entry_fields_get(unit, ent_hdl, &cfg[num]));
                continue;
            }
        }

        /* Get next, save info. */
        num_tmp = num;
        num = num ? 0 : 1;

        /* Skip those with same key but different pipe.*/
        while ((rv[num] = bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_TRAVERSE,
                              BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {

            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_xlate_entry_key_get(unit, ent_hdl,
                                          &port[num], &outer_vlan[num],
                                          &inner_vlan[num], pipe_ptr[num]));
            if (!vlan_xlate_key_equal(unit, bcmVlanTranslateKeyInvalid,
                                     port[0], port[1],
                                     outer_vlan[0], outer_vlan[1],
                                     inner_vlan[0], inner_vlan[1])) {
                break;
            }
        }

        if (!rv[num]) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_xlate_entry_fields_get(unit, ent_hdl, &cfg[num]));
        }

        rv_cb = cb(unit, port[num_tmp], outer_vlan[num_tmp],
                   inner_vlan[num_tmp], &cfg[num_tmp], user_data);
        if (SHR_FAILURE(rv_cb) && bcmi_ltsw_dev_cb_err_abort(unit)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv_cb);
        }
    }

    if ((rv[num] != SHR_E_NOT_FOUND) && SHR_FAILURE(rv[num])) {
        SHR_IF_ERR_VERBOSE_EXIT(rv[num]);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FREE(pipe_mask);
    VLAN_EGR_XLATE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Call user provided callback for ingress vlan action traverse.
 *
 * This function is used to call user provided callback for ingress vlan action
 * traverse.
 *
 * \param [in] unit              Unit number.
 * \param [in] trvs_info         Traverse information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_action_traverse_cb(int unit,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_gport_t port,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg,
                                  void *user_data)
{
    ltsw_vlan_ing_xlate_action_traverse_data_t *trvs_data;
    bcm_vlan_action_set_t action;

    SHR_FUNC_ENTER(unit);

    bcm_vlan_action_set_t_init(&action);

    trvs_data = (ltsw_vlan_ing_xlate_action_traverse_data_t *)user_data;

    /* Get action fields. */
    action.outer_tag = cfg->otag_action;
    action.inner_tag = cfg->itag_action;
    action.ingress_if = cfg->l3_iif;

    if (cfg->opaque_ctrl_id & 0x1 << OpaqueCtrlDrop) {
        action.flags |= BCM_VLAN_ACTION_SET_DROP;
    }
    if (cfg->opaque_ctrl_id & 0x1 << OpaqueCtrlCopyToCpu) {
        action.flags |= BCM_VLAN_ACTION_SET_COPY_TO_CPU;
    }

    BCMI_LTSW_VLAN_XLATE_OPAQUE_HW2SW(action.opaque_ctrl_id,
                                      cfg->opaque_ctrl_id,
                                      cfg->opaque_ctrl_id_1);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vpnid_get(unit, cfg->vfi,
                                     &(action.forwarding_domain)));

    SHR_IF_ERR_VERBOSE_EXIT
        (trvs_data->cb(unit, port, key_type, outer_vlan, inner_vlan,
                        &action, trvs_data->user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Call user provided callback for egress vlan action traverse.
 *
 * This function is used to call user provided callback for ingress vlan action
 * traverse.
 *
 * \param [in] unit              Unit number.
 * \param [in] trvs_info         Traverse information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_action_traverse_cb(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  ltsw_vlan_egr_xlate_cfg_t *cfg,
                                  void *user_data)
{
    ltsw_vlan_egr_xlate_action_traverse_data_t *trvs_data;
    bcm_vlan_action_set_t action;
    int map_id = 0;

    SHR_FUNC_ENTER(unit);

    bcm_vlan_action_set_t_init(&action);

    trvs_data = (ltsw_vlan_egr_xlate_action_traverse_data_t *)user_data;

    /* Get action fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_qos_default_ptr_get(unit, bcmiQosMapTypeL2Egress, &map_id));
    if (map_id == cfg->dot1p_map_id) {
        action.priority = -1;
    } else {
        action.priority = cfg->outer_pri;
        action.new_outer_cfi = cfg->outer_cfi;
        action.new_inner_pkt_prio = cfg->inner_pri;
        action.new_inner_cfi = cfg->inner_cfi;
    }

    action.new_outer_vlan = cfg->outer_vlan;
    action.new_inner_vlan = cfg->inner_vlan;

    action.outer_tag = cfg->otag_action;
    action.inner_tag = cfg->itag_action;

    if (cfg->opaque_ctrl & 0x1 << OpaqueCtrlDrop) {
        action.flags |= BCM_VLAN_ACTION_SET_DROP;
    }

    if (cfg->opaque_ctrl & 0x1 << OpaqueCtrlHgExtension) {
        action.flags |= BCM_VLAN_ACTION_SET_HIGIG3_EXTENSION_ADD;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (trvs_data->cb(unit, port, outer_vlan, inner_vlan,
                       &action, trvs_data->user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the validation of VLAN control configuration.
 *
 * This function is used to check the validation of VLAN control configuration.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  valid_fields  Valid fields for VLAN control structure,
 *                            BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [in]  control       VLAN control structure.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_control_validate(int unit,
                      bcm_vlan_t vid,
                      uint32_t *valid_fields,
                      bcm_vlan_control_vlan_t *control)
{
    uint32_t supported_flags = 0;
    uint32_t supported_fields = 0;
    uint32_t mc_flags = 0;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_virtual_vlan_vpn_valid(unit, vid)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    supported_fields = BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK |
                       BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK |
                       BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK |
                       BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK |
                       BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK |
                       BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK |
                       BCM_VLAN_CONTROL_VLAN_EGRESS_ACTION_MASK |
                       BCM_VLAN_CONTROL_VLAN_L2_VIEW_MASK |
                       BCM_VLAN_CONTROL_VLAN_EGRESS_CLASS_ID_MASK |
                       BCM_VLAN_CONTROL_VLAN_EGRESS_OPAQUE_CTRL_ID_MASK |
                       BCM_VLAN_CONTROL_VLAN_INGRESS_OPAQUE_CTRL_ID_MASK;

    supported_flags = BCM_VLAN_LEARN_DISABLE |
                      BCM_VLAN_IP4_MCAST_L2_DISABLE |
                      BCM_VLAN_IP6_MCAST_L2_DISABLE |
                      BCM_VLAN_NON_UCAST_DROP |
                      BCM_VLAN_UNKNOWN_UCAST_DROP;

    mc_flags = BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK |
               BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK |
               BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK;

    if (*valid_fields != BCM_VLAN_CONTROL_VLAN_ALL_MASK) {
        if ((~supported_fields) & *valid_fields) {
            SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
        }
    }

    if (!BCMI_LTSW_VIRTUAL_VPN_IS_SET(vid) &&
        VLAN_DLF_FLOOD(unit) && (*valid_fields & mc_flags)) {
        if (*valid_fields != BCM_VLAN_CONTROL_VLAN_ALL_MASK) {
            SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
        } else {
            *valid_fields &= ~mc_flags;
        }
    }

    if (control == NULL) {
        SHR_EXIT();
    }

    if ((~supported_flags) & control->flags) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
    }

    if (control->flags2 != 0) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
    }

    if ((*valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) &&
        (*valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) &&
        (control->unknown_multicast_group != control->broadcast_group)) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
    }

    if ((*valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) &&
        (*valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) &&
        (control->unknown_multicast_group != control->unknown_unicast_group)) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
    }

    if ((*valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) &&
        (*valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) &&
        (control->broadcast_group != control->unknown_unicast_group)) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
    }

    if (*valid_fields & BCM_VLAN_CONTROL_VLAN_EGRESS_ACTION_MASK) {
        if ((control->egress_action.outer_tag != bcmVlanActionNone) &&
            (control->egress_action.outer_tag != bcmVlanActionAdd)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if ((control->egress_action.inner_tag != bcmVlanActionNone) &&
            (control->egress_action.inner_tag != bcmVlanActionAdd)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (control->egress_action.new_outer_vlan > BCM_VLAN_MAX ||
            control->egress_action.new_inner_vlan > BCM_VLAN_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (control->egress_action.priority < 0 ||
            control->egress_action.priority > BCM_PRIO_MAX ||
            control->egress_action.new_inner_pkt_prio > BCM_PRIO_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (control->egress_action.new_outer_cfi > 1 ||
            control->egress_action.new_inner_cfi > 1) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get control information from ingress VLAN table.
 *
 * This function is used to get control information from ingress VLAN table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  valid_fields  Valid fields for VLAN control structure,
 *                            BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [out]  control      VLAN control structure.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_ing_entry_control_get(int unit,
                           bcm_vlan_t vid,
                           uint32_t valid_fields,
                           bcm_vlan_control_vlan_t *control)
{
    bcm_multicast_t flooding_group = 0;
    uint32_t vfi = 0;
    uint8_t bmp = 0;
    int i = 0;
    ltsw_vlan_ing_entry_t entry;

    SHR_FUNC_ENTER(unit);

    /* Get VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, vid, &vfi));
    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    /* Add VLAN ID. */
    entry.vfi = vfi;
    entry.fld_bmp |= IV_FLD_DESTINATION | IV_FLD_FWD_VFI |
                     IV_FLD_INGRESS_IF | IV_FLD_OUTER_TPID_BMP |
                     IV_FLD_IP4MC_L2_EN | IV_FLD_IP6MC_L2_EN |
                     IV_FLD_L2MC_MISS_ACTION | IV_FLD_L2UC_MISS_ACTION |
                     IV_FLD_NOT_LEARN | IV_FLD_L2_HOST_NARROW_ENABLE |
                     IV_FLD_OPAQUE_CTRL_ID;

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    /* Get flooding group. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK ||
        valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK ||
        valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {

        /* Get flooding group and type. */
        if (BCMI_XFS_DEST_TYPE_L2MC_GROUP == entry.dest_type) {
            _BCM_MULTICAST_GROUP_SET(flooding_group,
                                     _BCM_MULTICAST_TYPE_L2, entry.dest);
        } else if (BCMI_XFS_DEST_TYPE_L3MC_GROUP == entry.dest_type) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_multicast_l3_grp_id_get(unit, (uint32_t)entry.dest,
                                                   &flooding_group));
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
        control->broadcast_group = flooding_group;
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) {
        control->unknown_multicast_group = flooding_group;
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) {
        control->unknown_unicast_group = flooding_group;
    }

    /* Get mapped VFI. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vpnid_get(unit, entry.fwd_vfi,
                                         &(control->forwarding_vlan)));
    }

    /* Get L3_IIF. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        control->ingress_if = (bcm_if_t)entry.ingress_if;
    }

    /* Get EXPECTED_OUTER_TPID_BITMAP. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK) {
        bmp = (uint8_t)entry.outer_tpid_bmp;
        while(bmp) {
            if (bmp & 1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_otpid_entry_get(unit, i,
                                                    &control->outer_tpid));
                break;
            }
            bmp = bmp >> 1;
            i++;
        }
    }

    /* Get L2 table view. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_L2_VIEW_MASK) {
        control->l2_view = entry.l2_host_narrow_enable;
    }

    /* Get opaque control id. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_OPAQUE_CTRL_ID_MASK) {
        control->ingress_opaque_ctrl_id = (int)entry.opaque_ctrl_id;
    }

    /* IPv4 MC packets can be bridged on this L2 forwarding domain or not. */
    if (!entry.ip4mc_l2_enable) {
        control->flags |= BCM_VLAN_IP4_MCAST_L2_DISABLE;
    }

    /* IPv6 MC packets can be bridged on this L2 forwarding domain or not. */
    if (!entry.ip6mc_l2_enable) {
        control->flags |= BCM_VLAN_IP6_MCAST_L2_DISABLE;
    }

    /* Get L2 Multicast lookup miss action. */
    if (entry.l2mc_miss_action == VLAN_MISS_ACTION_TYPE_DROP) {
        control->flags |= BCM_VLAN_NON_UCAST_DROP;
    }

    /* L2 Unicast lookup miss action. */
    if (entry.l2uc_miss_action == VLAN_MISS_ACTION_TYPE_DROP) {
       control->flags |= BCM_VLAN_UNKNOWN_UCAST_DROP;
    }

    /* Learning is disabled or not. */
    if (entry.do_not_learn) {
       control->flags |= BCM_VLAN_LEARN_DISABLE;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get control information from egress VLAN table.
 *
 * This function is used to get control information from egress VLAN table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  valid_fields  Valid fields for VLAN control structure,
 *                            BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [out]  control      VLAN control structure.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
vlan_egr_entry_control_get(int unit,
                           bcm_vlan_t vid,
                           uint32_t valid_fields,
                           bcm_vlan_control_vlan_t *control)
{
    uint32_t vfi = 0;
    ltsw_vlan_egr_entry_t entry;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    /* Get VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, vid, &vfi));

    /* Add VLAN ID. */
    entry.vfi = vfi;
    entry.fld_bmp |= EV_FLD_CLASS_ID | EV_FLD_O_VLAN |
                     EV_FLD_I_VLAN | EV_FLD_VIEW | EV_FLD_TAG_ACTION |
                     EV_FLD_EFP_CTRL_ID;

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    /* Get opaque control id. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_EGRESS_OPAQUE_CTRL_ID_MASK) {
        control->egress_opaque_ctrl_id = (int)entry.efp_ctrl_id;
    }

    /* Get class id. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_EGRESS_CLASS_ID_MASK) {
        control->egress_class_id = (int)entry.class_id;
    }

    /* Get VLAN action. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_EGRESS_ACTION_MASK) {

        /* Get outer tag. */
        control->egress_action.priority = VLAN_CTRL_PRIO(entry.o_vlan);
        control->egress_action.new_outer_cfi = VLAN_CTRL_CFI(entry.o_vlan);
        control->egress_action.new_outer_vlan = VLAN_CTRL_ID(entry.o_vlan);

        /* Get entry view. */
        if (EGR_VFI_VIEW_DEFAULT == entry.view) {
            /* Get inner tag -  only on VT_VFI_DEFAULT view */
            control->egress_action.new_inner_pkt_prio =
                VLAN_CTRL_PRIO(entry.i_vlan);
            control->egress_action.new_inner_cfi = VLAN_CTRL_CFI(entry.i_vlan);
            control->egress_action.new_inner_vlan = VLAN_CTRL_ID(entry.i_vlan);
        }

        /* Get VLAN tag action. */
        control->egress_action.outer_tag =
            (entry.tag_action & 0x1) ? bcmVlanActionAdd: bcmVlanActionNone;
        control->egress_action.inner_tag =
            ((entry.tag_action >> 1) & 0x1) ? bcmVlanActionAdd : bcmVlanActionNone;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set control information into ingress VLAN table.
 *
 * This function is used to set control information into ingress VLAN table.
 *
 * \param [in]  unit             Unit number.
 * \param [in]  vid              VLAN ID.
 * \param [in]  pre_config_first First pre-config VLAN or not.
 * \param [in]  valid_fields     Valid fields for VLAN control structure,
 *                               BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [in]  control          VLAN control structure.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_entry_control_set(int unit,
                           bcm_vlan_t vid,
                           int pre_config_first,
                           uint32_t valid_fields,
                           bcm_vlan_control_vlan_t *control)
{
    bcm_vlan_control_vlan_t old_control;
    int dest_set = 0;
    uint32_t vfi = 0, fwd_vfi = 0;
    int index = 0;
    ltsw_vlan_ing_entry_t entry;
    bcmlt_opcode_t op_code = BCMLT_OPCODE_UPDATE;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_VFI;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    /* Get VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, vid, &vfi));

    /* Add VLAN ID. */
    entry.vfi = vfi;

    entry.dest_type = BCMI_XFS_DEST_TYPE_NO_OP;
    if ((dest_set == 0) &&
        (valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK)) {
        if (control->broadcast_group) {
            if (!_BCM_MULTICAST_IS_L2(control->broadcast_group)) {
                entry.dest_type = BCMI_XFS_DEST_TYPE_L3MC_GROUP;
            } else {
                entry.dest_type = BCMI_XFS_DEST_TYPE_L2MC_GROUP;
            }
            entry.dest = _BCM_MULTICAST_ID_GET(control->broadcast_group);
        }
        dest_set = 1;
    }

    if ((dest_set == 0) &&
        (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK)) {
        if (control->unknown_multicast_group) {
            if (!_BCM_MULTICAST_IS_L2(control->unknown_multicast_group)) {
                entry.dest_type = BCMI_XFS_DEST_TYPE_L3MC_GROUP;
            } else {
                entry.dest_type = BCMI_XFS_DEST_TYPE_L2MC_GROUP;
            }
            entry.dest =
                _BCM_MULTICAST_ID_GET(control->unknown_multicast_group);
        }
        dest_set = 1;
    }

    if ((dest_set == 0) &&
        (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK)) {
        if (control->unknown_unicast_group) {
            if (!_BCM_MULTICAST_IS_L2(control->unknown_unicast_group)) {
                entry.dest_type = BCMI_XFS_DEST_TYPE_L3MC_GROUP;
            } else {
                entry.dest_type = BCMI_XFS_DEST_TYPE_L2MC_GROUP;
            }
            entry.dest = _BCM_MULTICAST_ID_GET(control->unknown_unicast_group);
        }
        dest_set = 1;
    }

    if (dest_set) {
        /* Set flooding group and type. */
        entry.fld_bmp |= IV_FLD_DESTINATION;
    }

    /* Set mapped VFI. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_FORWARDING_VLAN_MASK) {
        entry.fld_bmp |= IV_FLD_FWD_VFI;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vfi_idx_get(unit, control->forwarding_vlan,
                                           &fwd_vfi));
        entry.fwd_vfi = fwd_vfi;
    }

    /* Set L3_IIF. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
         if (SHR_SUCCESS
             (bcmi_ltsw_l3_ing_intf_id_validate(unit, control->ingress_if))) {
             ent_type = BCMI_LTSW_SBR_PET_DEF;
        } else {
            ent_type = BCMI_LTSW_SBR_PET_NONE;
        }
        entry.fld_bmp |= IV_FLD_INGRESS_IF;
        entry.ingress_if = control->ingress_if;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth, ent_type, &index));
        entry.fld_bmp |= IV_FLD_STREN_PROF_IDX;
        entry.stren_prof_idx = (uint64_t)index;
    } else {
        /*Insert VFI entry for the first time.*/
        if (pre_config_first) {
            ent_type = BCMI_LTSW_SBR_PET_NONE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth,
                                                     ent_type, &index));
            entry.fld_bmp |= IV_FLD_STREN_PROF_IDX;
            entry.stren_prof_idx = (uint64_t)index;
        }
        /*else if update VFI entry without INGRESS_IF_MASK, do not change stren. */
    }

    /* Set EXPECTED_OUTER_TPID_BITMAP. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK) {
        old_control.outer_tpid = 0;
        if (!pre_config_first) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_entry_control_get(unit, vid,
                     BCM_VLAN_CONTROL_VLAN_OUTER_TPID_MASK, &old_control));
        }
        if (old_control.outer_tpid != control->outer_tpid) {
            if (old_control.outer_tpid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_otpid_entry_lookup
                         (unit, old_control.outer_tpid, &index));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_ltsw_vlan_otpid_entry_delete(unit, index));
            }
            entry.fld_bmp |= IV_FLD_OUTER_TPID_BMP;
            entry.outer_tpid_bmp = 0;
            if (control->outer_tpid) {
                SHR_IF_ERR_CONT
                    (bcmi_ltsw_vlan_otpid_entry_add
                        (unit, control->outer_tpid, &index));
                if (SHR_FUNC_ERR()) {
                    if (old_control.outer_tpid != 0) {
                        (void)bcmi_ltsw_vlan_otpid_entry_add
                                  (unit, old_control.outer_tpid, &index);
                    }
                    SHR_EXIT();
                }
                entry.outer_tpid_bmp = 0x1 << index;
            }
        }
    }

    /* Set L2 table view. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_L2_VIEW_MASK) {
        bcmi_ltsw_l2_view_vlan_info_t info;
        info.vlan = vid;
        info.l2_view = control->l2_view;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l2_view_vlan_update(unit, &info));
        entry.fld_bmp |= IV_FLD_L2_HOST_NARROW_ENABLE;
        entry.l2_host_narrow_enable = control->l2_view;
    }

    /* Set opaque control id. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_OPAQUE_CTRL_ID_MASK) {
        entry.fld_bmp |= IV_FLD_OPAQUE_CTRL_ID;
        entry.opaque_ctrl_id = control->ingress_opaque_ctrl_id;
    }

    /* IPv4 MC packets can be bridged on this L2 forwarding domain or not. */
    entry.fld_bmp |= IV_FLD_IP4MC_L2_EN;
    entry.ip4mc_l2_enable =
        (control->flags & BCM_VLAN_IP4_MCAST_L2_DISABLE) ? 0 : 1;

    /* IPv6 MC packets can be bridged on this L2 forwarding domain or not. */
    entry.fld_bmp |= IV_FLD_IP6MC_L2_EN;
    entry.ip6mc_l2_enable =
        (control->flags & BCM_VLAN_IP6_MCAST_L2_DISABLE) ? 0 : 1;

    /* Set L2 Multicast lookup miss action. */
    entry.fld_bmp |= IV_FLD_L2MC_MISS_ACTION;
    entry.l2mc_miss_action =
        (control->flags & BCM_VLAN_NON_UCAST_DROP) ?
            VLAN_MISS_ACTION_TYPE_DROP: VLAN_MISS_ACTION_TYPE_FLOOD;

    /* L2 Unicast lookup miss action. */
    entry.fld_bmp |= IV_FLD_L2UC_MISS_ACTION;
    entry.l2uc_miss_action =
        (control->flags & BCM_VLAN_UNKNOWN_UCAST_DROP) ?
            VLAN_MISS_ACTION_TYPE_DROP : VLAN_MISS_ACTION_TYPE_FLOOD;

    /* Learning is disabled or not. */
    entry.fld_bmp |= IV_FLD_NOT_LEARN;
    entry.do_not_learn = (control->flags & BCM_VLAN_LEARN_DISABLE) ? 1 : 0;

    /* Insert/update an ingress vlan entry. */
    if (pre_config_first) {

        /* Set profile_ptr index to the entry with empty port bitmap. */
        entry.fld_bmp |= IV_FLD_MSHIP_PROF_IDX;
        entry.mship_prof_idx = VLAN_PROFILE_ING_MSHP_CHK_RESV1;

        op_code = VLAN_ENTRY_INIT_EN(unit) ? BCMLT_OPCODE_UPDATE :
                                             BCMLT_OPCODE_INSERT;
    } else {
        op_code = BCMLT_OPCODE_UPDATE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, op_code, &entry));


exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set control information into egress VLAN table.
 *
 * This function is used to set control information into egress VLAN table.
 *
 * \param [in]  unit             Unit number.
 * \param [in]  vid              VLAN ID.
 * \param [in]  pre_config_first First pre-config VLAN or not.
 * \param [in]  valid_fields     Valid fields for VLAN control structure,
 *                               BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [in]  control          VLAN control structure.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_entry_control_set(int unit,
                           bcm_vlan_t vid,
                           int pre_config_first,
                           uint32_t valid_fields,
                           bcm_vlan_control_vlan_t *control)
{
    egr_vfi_view_t entry_view = EGR_VFI_VIEW_DEFAULT;
    uint32_t vfi = 0;
    int pf_idx = 0;
    ltsw_vlan_egr_entry_t entry;
    bcmlt_opcode_t op_code = BCMLT_OPCODE_UPDATE;
    bcmi_ltsw_sbr_profile_tbl_hdl_t pth = BCMI_LTSW_SBR_PTH_EGR_VFI;
    bcmi_ltsw_sbr_profile_ent_type_t ent_type = BCMI_LTSW_SBR_PET_NONE;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    /* Get VFI. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, vid, &vfi));
    entry.vfi = vfi;

    /* Set opaque control id. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_EGRESS_OPAQUE_CTRL_ID_MASK) {
        entry.fld_bmp |= EV_FLD_EFP_CTRL_ID;
        entry.efp_ctrl_id = control->egress_opaque_ctrl_id;
    }

    /* Set class id. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_EGRESS_CLASS_ID_MASK) {
        entry.fld_bmp |= EV_FLD_CLASS_ID;
        entry.class_id = control->egress_class_id;
        if (entry.class_id) {
            ent_type = BCMI_LTSW_SBR_PET_DEF;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_profile_ent_index_get(unit, pth,
                                                 ent_type, &pf_idx));
        entry.fld_bmp |= EV_FLD_STRENGTH_PRFL_IDX;
        entry.strength_prfl_idx = pf_idx;
    }

    /* Set VLAN action. */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_EGRESS_ACTION_MASK) {

        /* Set outer tag. */
        entry.o_vlan = VLAN_CTRL(control->egress_action.priority,
                                 control->egress_action.new_outer_cfi,
                                 control->egress_action.new_outer_vlan);

        entry.fld_bmp |= EV_FLD_O_VLAN;

        /*
         * tag_action[0]: otag_action = 0: delete, 1: add/replace;
         * tag_action[1]: itag_action = 0: delete, 1: add/replace;
         * tag_action[2]: Reserved;
         * tag_action[3]: Reserved;
         */
        entry.tag_action =
            (control->egress_action.outer_tag == bcmVlanActionAdd) ? 1 : 0;

        if (!pre_config_first) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_entry_view_get(unit, vid, &entry_view));
        }

        /* Set inner tag. */
        if ((!pre_config_first &&(EGR_VFI_VIEW_DEFAULT == entry_view))
            || pre_config_first) {
            entry.i_vlan = VLAN_CTRL(control->egress_action.new_inner_pkt_prio,
                                     control->egress_action.new_inner_cfi,
                                     control->egress_action.new_inner_vlan);
            entry.fld_bmp |= EV_FLD_I_VLAN;

            entry.tag_action |= ((control->egress_action.inner_tag ==
                             bcmVlanActionAdd) ? 1 : 0) << 1;
        }

        entry.fld_bmp |= EV_FLD_TAG_ACTION;
        /* Preserve pkt's odot1p by default. Egr port takes final control. */
        entry.tag_action |= (1 << 2);
    }

    /* Insert/update an egress vlan entry. */
    if (pre_config_first) {

        /* Set entry view. */
        entry.fld_bmp |= EV_FLD_VIEW;
        entry.view = EGR_VFI_VIEW_DEFAULT;

        /* Set profile_ptr index to the entry with empty port bitmap. */
        entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX;
        entry.mship_prof_idx = VLAN_PROFILE_EGR_MSHP_CHK_RESV1;

        /* Set default ovlan value and actions. */
        if (!(valid_fields & BCM_VLAN_CONTROL_VLAN_EGRESS_ACTION_MASK) &&
            (vfi <= BCM_VLAN_MAX)) {
            /* Identity mapping between VLAN and VFI in 0-4K region. */
            entry.fld_bmp |= EV_FLD_O_VLAN;
            entry.o_vlan = VLAN_CTRL(0, 0, vfi);
            /* Add ovlan by default since otag is not preserved in L2_IIF. */
            entry.fld_bmp |= EV_FLD_TAG_ACTION;
            entry.tag_action = 1;
            /* Preserve pkt's odot1p by default. Egr port takes final control. */
            entry.tag_action |= (1 << 2);
        }

        op_code = VLAN_ENTRY_INIT_EN(unit) ? BCMLT_OPCODE_UPDATE :
                                             BCMLT_OPCODE_INSERT;
    } else {
        op_code = BCMLT_OPCODE_UPDATE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, op_code, &entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set port-specific VLAN configuration.
 *
 * This function is used to set port-specific VLAN configuration.
 *
 * \param [in] unit               Unit number.
 * \param [inout]port             Port number.
 * \param [in]  type              Control port type.
 *
 * \retval SHR_E_NONE             No errors.
 * \retval !SHR_E_NONE            Failure.
 */
static int
vlan_control_port_set(int unit,
                      int port,
                      bcm_vlan_control_port_t type,
                      int arg)
{
    bcm_port_t port_out;

    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_virtual_vp_is_vfi_type(unit, port)) {
        /*
         * bcmVlanTranslateEgressEnable -
         *     ing_svp_table.use_itag_dot1p_cfi_for_phb.
         * bcmVlanPortUseInnerPri -
         *     ing_svp_table.use_itag_dot1p_cfi_for_phb.
         * bcmVlanPortUseOuterPri -
         *     ing_svp_table.use_otag_dot1p_cfi_for_phb.
         * bcmVlanPortPayloadTagsDelete -
         *     egr_dvp.process_ctrl[0] for bcmVlanPortPayloadTagsDelete.
         * bcmVlanPortTranslateEgressVnidEnable -
         *     egr_dvp.process_ctrl[1] for bcmVlanPortTranslateEgressVnidEnable.
         */
        arg = (arg ? 1 : 0);
        switch (type) {
        case bcmVlanTranslateEgressEnable:
        case bcmVlanPortUseInnerPri:
        case bcmVlanPortUseOuterPri:
        case bcmVlanPortPayloadTagsDelete:
        case bcmVlanPortTranslateEgressVnidEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_virtual_port_vlan_ctrl_set(unit, port, type, arg));

            SHR_EXIT();
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port_out));

        arg = (arg ? 1 : 0);
        switch (type) {
        case bcmVlanTranslateIngressEnable:
            /* Set ing_l2_iif_table.vlan_xlate_enable. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, port_out,
                                        BCMI_PT_ING_VT_EN, arg));

            SHR_EXIT();
        case bcmVlanTranslateEgressEnable:
            /* Set egr_l2_oif.l2_process_ctrl[0] for egr vlan translation. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_egr_vlan_xlate_enable_set(unit, port_out, arg));

            SHR_EXIT();
        case bcmVlanPortUseInnerPri:
            /* Set ing_l2_iif_attributes_table.use_itag_dot1p_cfi_for_phb. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, port_out,
                                        BCMI_PT_IVLAN_MAP_TO_PHB, arg));
            SHR_EXIT();
        case bcmVlanPortUseOuterPri:
            /* Set ing_l2_iif_attributes_table.use_otag_dot1p_cfi_for_phb. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_set(unit, port_out,
                                        BCMI_PT_OVLAN_MAP_TO_PHB, arg));

            SHR_EXIT();

        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port-specific VLAN configuration.
 *
 * This function is used to get port-specific VLAN configuration.
 *
 * \param [in] unit               Unit number.
 * \param [in]port                Port number.
 * \param [in] type               Control port type.
 * \param [out] arg               Arg vlaue which is dependent on control type.
 *
 * \retval SHR_E_NONE             No errors.
 * \retval !SHR_E_NONE            Failure.
 */
static int
vlan_control_port_get(int unit,
                      bcm_port_t port,
                      bcm_vlan_control_port_t type,
                      int *arg)
{
    bcm_port_t port_out;

    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_virtual_vp_is_vfi_type(unit, port)) {
        switch (type) {
        case bcmVlanTranslateEgressEnable:
        case bcmVlanPortUseInnerPri:
        case bcmVlanPortUseOuterPri:
        case bcmVlanPortPayloadTagsDelete:
        case bcmVlanPortTranslateEgressVnidEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_virtual_port_vlan_ctrl_get(unit, port, type, arg));

            SHR_EXIT();
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port, &port_out));

        switch (type) {
        case bcmVlanTranslateIngressEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get(unit, port_out,
                                        BCMI_PT_ING_VT_EN, arg));

            SHR_EXIT();
        case bcmVlanTranslateEgressEnable:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_egr_vlan_xlate_enable_get(unit, port_out, arg));

            SHR_EXIT();
        case bcmVlanPortUseInnerPri:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get(unit, port_out,
                                        BCMI_PT_IVLAN_MAP_TO_PHB, arg));
            SHR_EXIT();
        case bcmVlanPortUseOuterPri:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_tab_get(unit, port_out,
                                        BCMI_PT_OVLAN_MAP_TO_PHB, arg));

            SHR_EXIT();

        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate vlan vfi.
 *
 * \param [in]      unit            Unit number.
 * \param [in]      vlan_vfi        VLAN Id.
 * \param [out]     vfi             Physical VFI index.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
vlan_vfi_resolve(int unit, bcm_vlan_t vlan_vfi, uint32_t *vfi)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, vlan_vfi, vfi));

    /* No usage check during bcm init. */
    if (bcmi_ltsw_init_state_get(unit)) {
        SHR_EXIT();
    }

    if (BCMI_LTSW_VIRTUAL_VPN_IS_SET(vlan_vfi)) {
        if(bcmi_ltsw_virtual_vfi_used_get(unit, *vfi,
                                          BCMI_LTSW_VFI_TYPE_ANY)) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    } else if (ltsw_vlan_info[unit].inited) {
        if (!bcmint_vlan_info_exist_check(unit, *vfi)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Internal function to validate the given port to be a
 *         supported GPORT. Once validation successfully completes, resolve the
 *         given port to its local port representation.
 *
 * \param [in] unit                 Unit number.
 * \param [in] in_port              Port/gport to be validated and resolved.
 * \param [out] out_port            Local port. If NULL then the function would
 *                                  only validate the given port.
 *
 * \retval SHR_E_NONE                 No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_gport_resolve(int unit, bcm_port_t in_port, bcm_port_t *out_port)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out_port, SHR_E_PARAM);

    if (BCM_GPORT_IS_LOCAL(in_port) ||
        BCM_GPORT_IS_LOCAL_CPU(in_port) ||
        BCM_GPORT_IS_DEVPORT(in_port) ||
        BCM_GPORT_IS_MODPORT(in_port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, in_port, out_port));
    } else {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Call user provided callback for VLAN range traverse.
 *
 * \param [in] unit            Unit number.
 * \param [in] gport           gport ID.
 * \param [in] user_data       user_data.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_range_traverse_cb(int unit, bcm_gport_t gport, void *user_data)
{
    ltsw_vlan_range_traverse_data_t *trvs_data;
    int prof_idx, i;
    ltsw_vlan_range_map_profile_t profile;

    SHR_FUNC_ENTER(unit);
    trvs_data = (ltsw_vlan_range_traverse_data_t *)user_data;
    sal_memset(&profile, 0, sizeof(ltsw_vlan_range_map_profile_t));

    /* Get vlan range profile index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, gport,
                                BCMI_PT_RANGE_MAP_INDEX, &prof_idx));
    /* Skip default profile. */
    if (VLAN_PROFILE_RANGE_MAP_RESV0 == prof_idx) {
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_get(unit, prof_idx, 0, &profile));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_get(unit, prof_idx, 1, &profile));

    /* Call back. */
    for (i = 0; i < 8; i++) {
       if ((1 == profile.ovid_min[i]) && (0 == profile.ovid_max[i])) {
           break;
       } else {
           SHR_IF_ERR_VERBOSE_EXIT
               (trvs_data->cb(unit, gport,
                              profile.ovid_min[i], profile.ovid_max[i],
                              BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                              NULL, trvs_data->user_data));
        }
    }

    for (i = 0; i < 8; i++) {
       if ((1 == profile.ivid_min[i]) && (0 == profile.ivid_max[i])) {
           break;
       } else {
           SHR_IF_ERR_VERBOSE_EXIT
               (trvs_data->cb(unit, gport,
                              BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                              profile.ivid_min[i], profile.ivid_max[i],
                              NULL, trvs_data->user_data));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an vlan range item to the VLAN range table.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port_array       Port array Id.
 * \param [in]      port_cnt         Num of ports.
 * \param [in]      outer_vlan_low   Outer VLAN ID Low.
 * \param [in]      outer_vlan_high  Outer VLAN ID High.
 * \param [in]      inner_vlan_low   Inner VLAN ID Low.
 * \param [in]      inner_vlan_high  Inner VLAN ID High.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
vlan_translate_action_range_add(int unit,
                                bcm_port_t *port_array, int port_cnt,
                                bcm_vlan_t outer_vlan_low,
                                bcm_vlan_t outer_vlan_high,
                                bcm_vlan_t inner_vlan_low,
                                bcm_vlan_t inner_vlan_high)
{
    int old_idx, new_idx;
    ltsw_vlan_range_map_profile_t profile;
    int i, j;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port_array[0],
                                BCMI_PT_RANGE_MAP_INDEX, &old_idx));

    /* Construct new profile. */
    sal_memset(&profile, 0, sizeof(ltsw_vlan_range_map_profile_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_get(unit, old_idx, 1, &profile));

    if (inner_vlan_low != BCM_VLAN_INVALID) {
        for (i = 0 ; i < 8 ; i++) {
            /* Find the first unused min/max range. Unused ranges are
             * identified by { min == 1, max == 0 }.
             */
            if ((profile.ivid_min[i] == 1) && (profile.ivid_max[i] == 0)) {
                break;
            } else if (profile.ivid_min[i] == inner_vlan_low) {
                /* Can't have multiple ranges with the same min. */
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
            }
        }
        if (i == 8) {
            /* All ranges are taken. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }

        /* Insert the new range into the table entry sorted by min VID. */
        for ( ; i > 0 ; i--) {
            if (profile.ivid_min[i - 1] > inner_vlan_low) {
                /* Move existing min/max down. */
                profile.ivid_min[i] = profile.ivid_min[i - 1];
                profile.ivid_max[i] = profile.ivid_max[i - 1];
            } else {
                break;
            }
        }
        profile.ivid_min[i] = inner_vlan_low;
        profile.ivid_max[i] = inner_vlan_high;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_get(unit, old_idx, 0, &profile));
    if (outer_vlan_low != BCM_VLAN_INVALID) {
        for (i = 0 ; i < 8 ; i++) {
            /* Find the first unused min/max range. Unused ranges are
             * identified by { min == 1, max == 0 }.
             */
            if ((profile.ovid_min[i] == 1) && (profile.ovid_max[i] == 0)) {
                break;
            } else if (profile.ovid_min[i] == outer_vlan_low) {
                /* Can't have multiple ranges with the same min. */
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
            }
        }
        if (i == 8) {
            /* All ranges are taken. */
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_FULL);
        }

        /* Insert the new range into the table entry sorted by min VID. */
        for ( ; i > 0 ; i--) {
            if (profile.ovid_min[i-1] > outer_vlan_low) {
                /* Move existing min/max down. */
                profile.ovid_min[i] = profile.ovid_min[i - 1];
                profile.ovid_max[i] = profile.ovid_max[i - 1];
            } else {
                break;
            }
        }
        profile.ovid_min[i] = outer_vlan_low;
        profile.ovid_max[i] = outer_vlan_high;
    }

    for (j = 0; j < port_cnt; j++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_add(unit, &profile, &new_idx));
        SHR_IF_ERR_CONT
            (bcmi_ltsw_port_tab_set(unit, port_array[j],
                                    BCMI_PT_RANGE_MAP_INDEX, new_idx));
        if (SHR_FUNC_ERR()) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_range_map_profile_delete(unit, new_idx));
            SHR_EXIT();
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_delete(unit, old_idx));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an vlan range item in the VLAN range table.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port_array       Port array IDs.
 * \param [in]      port_cnt         Num of ports.
 * \param [in]      outer_vlan_low   Outer VLAN ID Low.
 * \param [in]      outer_vlan_high  Outer VLAN ID High.
 * \param [in]      inner_vlan_low   Inner VLAN ID Low.
 * \param [in]      inner_vlan_high  Inner VLAN ID High.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
vlan_translate_action_range_delete(int unit,
                                   bcm_port_t *port_array, int port_cnt,
                                   bcm_vlan_t outer_vlan_low,
                                   bcm_vlan_t outer_vlan_high,
                                   bcm_vlan_t inner_vlan_low,
                                   bcm_vlan_t inner_vlan_high)
{
    int old_idx, new_idx;
    ltsw_vlan_range_map_profile_t profile;
    int i, j;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, port_array[0],
                                BCMI_PT_RANGE_MAP_INDEX, &old_idx));

    sal_memset(&profile, 0, sizeof(ltsw_vlan_range_map_profile_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_get(unit, old_idx, 1, &profile));

    if (inner_vlan_low != BCM_VLAN_INVALID) {
        /* Find the min/max range. */
        for (i = 0 ; i < 8 ; i++) {
            if ((profile.ivid_min[i] == inner_vlan_low) &&
                (profile.ivid_max[i] == inner_vlan_high)) {
                break;
            }
        }
        if (i == 8) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        /* Remove the range from the table entry and fill in the gap. */
        for ( ; i < 7 ; i++) {
            /* Move existing min/max UP */
            profile.ivid_min[i] = profile.ivid_min[i + 1];
            profile.ivid_max[i] = profile.ivid_max[i + 1];
        }
        /* Mark last min/max range as unused. Unused ranges are
                * identified by { min == 1, max == 0 }.
                */
        profile.ivid_min[i] = 1;
        profile.ivid_max[i] = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_range_map_profile_entry_get(unit, old_idx, 0, &profile));
    if (outer_vlan_low != BCM_VLAN_INVALID) {
        /* Find the min/max range. */
        for (i = 0 ; i < 8 ; i++) {
            if ((profile.ovid_min[i] == outer_vlan_low) &&
                (profile.ovid_max[i] == outer_vlan_high)) {
                break;
            }
        }
        if (i == 8) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        /* Remove the range from the table entry and fill in the gap. */
        for ( ; i < 7 ; i++) {
            /* Move existing min/max UP. */
            profile.ovid_min[i] = profile.ovid_min[i + 1];
            profile.ovid_max[i] = profile.ovid_max[i + 1];
        }
        /* Mark last min/max range as unused. Unused ranges are
                * identified by { min == 1, max == 0 }.
                */
        profile.ovid_min[i] = 1;
        profile.ovid_max[i] = 0;
    }

    for (j = 0; j < port_cnt; j++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_add(unit, &profile, &new_idx));
        SHR_IF_ERR_CONT
            (bcmi_ltsw_port_tab_set(unit, port_array[j],
                                    BCMI_PT_RANGE_MAP_INDEX, new_idx));
        if (SHR_FUNC_ERR()) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_range_map_profile_delete(unit, new_idx));
            SHR_EXIT();
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_delete(unit, old_idx));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief operation on VLAN_XLATE_MISS_POLICY tables.
 *
 * \param [in]       unit           Unit number.
 * \param [in]       opcode         Table operation code.
 * \param [in]       ctrl           Opaque control enum.
 * \param [in]       ingress        Ingress flag.
 * \param [in/out]   arg            input when lookup and output when update.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
static int
vlan_xlate_miss_policy_table_op(int unit, bcmlt_opcode_t opcode,
                                ltsw_vlan_opaque_control_t ctrl,
                                int ingress, int *arg)
{
    int dunit;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *fld = NULL;
    uint64_t data;
    const bcmint_vlan_lt_t *lt_info;
    bcmint_vlan_lt_id_t lt_id[] =
        {BCMINT_LTSW_VLAN_R_EGR_VLAN_XLATE_MISS_POLICY,
         BCMINT_LTSW_VLAN_R_VLAN_XLATE_MISS_POLICY};
    int fld_id[] =
        {BCMINT_LTSW_VLAN_FLD_EGR_VT_MISS_POLICY_VT_CTRL,
         BCMINT_LTSW_VLAN_FLD_VT_MISS_POLICY_VT_CTRL};

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id[ingress], &lt_info));
    fld = lt_info->flds[fld_id[ingress]].name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, fld, &data));

    switch (opcode) {
        case BCMLT_OPCODE_LOOKUP:
            *arg = ((int)data & (0x1 << ctrl)) ? 1 : 0;
            break;
        case BCMLT_OPCODE_UPDATE:
            if (*arg != 0 && *arg != 1) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            data = (data & (~(0x1 << ctrl))) | (*arg << ctrl);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_remove(ent_hdl, fld));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(ent_hdl, fld, data));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_UPDATE, BCMLT_PRIORITY_NORMAL));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup the information of VLAN xlate pipe control.
 *
 * This function is used to cleanup the info of VLAN xlate pipe control.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_xlate_pipe_control_cleanup(int unit)
{
    int i, j;
    ltsw_vlan_info_t *vlan_info = &ltsw_vlan_info[unit];
    uint32_t pg_num[2] = {0};
    ltsw_vlan_pg_info_t *pg_ptr = NULL;
    SHR_FUNC_ENTER(unit);

    /* Init vlan does not clean xlate tables, so sw does not clean either.*/
    if (!bcmi_ltsw_init_state_get(unit)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit, ING_L2_IIF_TABLEs, &pg_num[0]));
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit, EGR_DVPs, &pg_num[1]));

    for (i = 0; i < 2; i ++) {
        pg_ptr = vlan_info->xlate_pipe_ctrl[i].pg_info;
        /* Free head mem.*/
        if (pg_ptr) {
            for (j = 0; j < pg_num[i]; j++) {
                SHR_FREE(pg_ptr[j].head);
            }
        }
        SHR_FREE(vlan_info->xlate_pipe_ctrl[i].next);
        SHR_FREE(vlan_info->xlate_pipe_ctrl[i].pg_info);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the information of VLAN xlate pipe control.
 *
 * This function is used to initialize the info of VLAN xlate pipe control.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_xlate_pipe_control_init(int unit)
{
    ltsw_vlan_info_t *vlan_info = &ltsw_vlan_info[unit];
    struct {
        bcmint_vlan_lt_id_t pipe_tbl;
        vlan_xlate_table_id_t tbl_id;
        bcmint_vlan_lt_id_t tbl_name;
    } xlate_tables[] = {
        {BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE_PIPE_CONFIG, XLATE_TABLE_ING_1,
            BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE},
        {BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE_PIPE_CONFIG, XLATE_TABLE_ING_2,
            BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE},
        {BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE_PIPE_CONFIG, XLATE_TABLE_ING_3,
            BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE},
        {BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_PIPE_CONFIG, XLATE_TABLE_EGR,
            BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION},
        {BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW_PIPE_CONFIG,
            XLATE_TABLE_EGR_DW, BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW}
    };
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    const bcmint_vlan_fld_t *flds;
    int i, j, dunit = 0, flags, shared = 0;
    uint32_t inst_size = 0, array_size = 0, alloc_size = 0;
    uint32_t pipe_ctrl[2] = {0}, pg_num[2] = {0};
    uint32_t xlate_num[2] = {0}, xlate_num_tmp = 0, xlate_num_share[] = {0, 0};
    int sw_xlate_ctrl[2];
    ltsw_vlan_pg_info_t *pg_ptr = NULL;
    ltsw_xlate_pipe_control_t *ctrl_ptr = NULL;
    bcmint_vlan_xlate_key_info_t *ha_key_ptr = NULL, *ha_key_ptr_2 = NULL;
    int wb_flag = bcmi_warmboot_get(unit), rv;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t value = 0;
    SHR_FUNC_ENTER(unit);

    /* Init vlan does not clean xlate tables, so sw does not init either.*/
    if (!bcmi_ltsw_init_state_get(unit)) {
        SHR_EXIT();
    }

    /* It is used as sw control of xlate info. */
    vlan_info->xlate_ctrl_en = 0;
    sw_xlate_ctrl[0] = VLAN_ING_XLATE_CTRL_EN(unit);
    sw_xlate_ctrl[1] = VLAN_EGR_XLATE_CTRL_EN(unit);

    for (i = 0; i < COUNTOF(xlate_tables); i++) {
        rv = bcmint_vlan_lt_get(unit, xlate_tables[i].tbl_name, &lt_info);
        if (rv == SHR_E_UNAVAIL) {
            vlan_info->xlate_table_size[i] = 0;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_capacity_get(unit, lt_info->name,
                                      &(vlan_info->xlate_table_size[i])));
        }
    }

    /* Get xlate pipe support.*/
    dunit = bcmi_ltsw_dev_dunit(unit);
    vlan_info->table_bmp = 0;
    for (i = 0; i < COUNTOF(xlate_tables); i++) {
        if (!vlan_info->xlate_table_size[i]) {
            continue;
        }

        rv = bcmint_vlan_lt_get(unit, xlate_tables[i].pipe_tbl, &lt_info);
        if (SHR_SUCCESS(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));
            flds = lt_info->flds;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_commit(ent_hdl, BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL));
            fld_name =
                flds[BCMINT_LTSW_VLAN_FLD_VLAN_XLATE_TABLE_PIPE_CONFIG_PIPEUNIQUE].name;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(ent_hdl, fld_name, &value));

            if (value && vlan_info->xlate_table_size[i]) {
                vlan_info->table_bmp |= (1 << xlate_tables[i].tbl_id);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_free(ent_hdl));
            ent_hdl = BCMLT_INVALID_HDL;
        }

        xlate_num_tmp = vlan_info->xlate_table_size[i];
        j = xlate_tables[i].tbl_id < XLATE_TABLE_EGR ? 0 : 1;

        if (value || sw_xlate_ctrl[j]) {
            /* XLATE tables may share same PT resources. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_vlan_xlate_table_share(unit, xlate_tables[i].tbl_id,
                                               &shared));
            if (shared) {
                xlate_num_share[j] = xlate_num_share[j] > xlate_num_tmp ?
                                     xlate_num_share[j] : xlate_num_tmp;
            } else {
                xlate_num[j] += xlate_num_tmp;
            }
        }
    }
    xlate_num[0] += xlate_num_share[0];
    xlate_num[1] += xlate_num_share[1];

    if (!vlan_info->xlate_ctrl_en && !vlan_info->table_bmp) {
        SHR_EXIT();
    }

    /* Caculate xlate_pipe_ctrl needed. One for ingress, One for egress. */
    if (VLAN_ING_XLATE_CTRL_EN(unit) || VLAN_ING_XLATE_PIPE_EN(unit)) {
        pipe_ctrl[0] = 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit, ING_L2_IIF_TABLEs, &pg_num[0]));
    }
    if (VLAN_EGR_XLATE_CTRL_EN(unit) || VLAN_EGR_XLATE_PIPE_EN(unit)) {
        pipe_ctrl[1] = 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit, EGR_DVPs, &pg_num[1]));
    }

    /* Alloc ltsw_vlan_pg_info_t.*/
    inst_size = sizeof(ltsw_vlan_pg_info_t);
    for (i = 0; i < 2; i ++) {
        if (pipe_ctrl[i]) {
            alloc_size = pg_num[i] * inst_size;
            SHR_ALLOC(pg_ptr, alloc_size, "bcmVlanPgInfo");
            SHR_NULL_CHECK(pg_ptr, SHR_E_MEMORY);
            sal_memset(pg_ptr, 0, alloc_size);
            vlan_info->xlate_pipe_ctrl[i].pg_info = pg_ptr;
            pg_ptr = NULL;
        }
    }

    /* Alloc key array.*/
    inst_size = sizeof(bcmint_vlan_xlate_key_info_t);
    /* Add one more reserved member (pipe_ctrl[i]). */
    array_size = xlate_num[0] + pipe_ctrl[0] +
                 xlate_num[1] + pipe_ctrl[1];
    alloc_size = inst_size * array_size;
    ha_key_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_VLAN,
                                        BCMINT_VLAN_SUB_COMP_ID_XLATE_KEY,
                                        "bcmVlanXlateKey", &alloc_size);
    /* Save this ha ptr for free usage. */
    ha_key_ptr_2 = ha_key_ptr;

    SHR_NULL_CHECK(ha_key_ptr, SHR_E_MEMORY);
    if (!wb_flag) {
        sal_memset(ha_key_ptr, 0, alloc_size);
    }
    rv = bcmi_ltsw_issu_struct_info_report(unit,
              BCMI_HA_COMP_ID_VLAN,
              BCMINT_VLAN_SUB_COMP_ID_XLATE_KEY,
              0, inst_size, array_size,
              BCMINT_VLAN_XLATE_KEY_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Alloc and recover next list.
     * The length of 'next' is same as 'key_array' and they share same index.
     * They are split up only because 'key_array' uses ISSU but 'next' does not.
     */
    for (i = 0; i < 2; i ++) {
        ctrl_ptr = &(vlan_info->xlate_pipe_ctrl[i]);
        if (pipe_ctrl[i]) {
            ctrl_ptr->key_array = ha_key_ptr;
            array_size = xlate_num[i] + pipe_ctrl[i];
            ha_key_ptr += array_size;
            alloc_size = array_size * sizeof(uint32_t);
            SHR_ALLOC(ctrl_ptr->next, alloc_size, "bcmVlanXlateCtrlNext");
            SHR_NULL_CHECK(ctrl_ptr->next, SHR_E_MEMORY);
            if(wb_flag) {
                flags = i ? BCMI_LTSW_PORT_EGR : BCMI_LTSW_PORT_ING;
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_xlate_entry_pipe_info_recover
                         (unit, flags, array_size, ctrl_ptr));
            } else {
                /* All key_array is linked in list starting from free_idx.
                 * 0 is invalid index.
                 */
                ctrl_ptr->free_idx = 1;
                ctrl_ptr->next[0] = 0;
                for (j = 1; j < array_size - 1; j++) {
                    ctrl_ptr->next[j] = j + 1;
                }
                ctrl_ptr->next[array_size - 1] = 0;
            }
        } else {
            ctrl_ptr->key_array = NULL;
            ctrl_ptr->next = NULL;
            ctrl_ptr->free_idx = 0;
        }
    }
exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (SHR_FUNC_ERR() && !wb_flag) {
        if (ha_key_ptr_2) {
            (void) bcmi_ltsw_ha_mem_free(unit, ha_key_ptr_2);
            ha_key_ptr_2 = NULL;
            ha_key_ptr = NULL;
        }
    }
    if (SHR_FUNC_ERR()) {
        for (i = 0; i < 2; i ++) {
            SHR_FREE(vlan_info->xlate_pipe_ctrl[i].pg_info);
            SHR_FREE(vlan_info->xlate_pipe_ctrl[i].next);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the entry information copied from another pipe.
 *
 * This function is used to check the entry information copied from another
 * pipe.
 *
 * \param [in] unit              Unit number.
 * \param [in] tid               Table id.
 * \param [in] ent_hdl           Entry handle.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_xlate_entry_pipe_copy_check(int unit,
                                 vlan_xlate_table_id_t tid,
                                 bcmlt_entry_handle_t ent_hdl)
{
    int stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    const bcmint_vlan_lt_t *lt_info;
    const char *fld_name;
    bcmint_vlan_lt_id_t lt_id;
    uint32_t act_idx = 0;
    uint64_t value = 0;
    int act_rv = 0;
    int act_id = 0;
    int obj_id = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    switch (tid) {
    case XLATE_TABLE_ING_1:
        lt_id = BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE;
        break;
    case XLATE_TABLE_ING_2:
        lt_id = BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE;
        break;
    case XLATE_TABLE_ING_3:
        lt_id = BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE;
        break;
    case XLATE_TABLE_EGR_DW:
        lt_id = BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW;
        break;
    case XLATE_TABLE_EGR:
        /* No flexctr information. */
        SHR_EXIT();
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (tid == XLATE_TABLE_EGR_DW) {
        act_id = BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_ACTION;
        obj_id = BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_INDEX;
        stage = BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR;
    } else {
        act_id = BCMINT_LTSW_VLAN_FLD_ING_XLATE_FLEX_CTR_ACTION;
        obj_id = BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPQ_OBJ_0;
        stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id, &lt_info));

    if (lt_info->fld_bmp & (1 << act_id)) {
        fld_name = lt_info->flds[act_id].name;
        rv = bcmlt_entry_field_get(ent_hdl, fld_name, &value);
        if (SHR_SUCCESS(rv)) {
            act_idx = (uint32_t)value;
            if (act_idx != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
                /* Get flexctr action mode. */
                act_rv = bcmi_ltsw_flexctr_action_is_single_pipe(
                             unit, stage, act_idx);
                if (act_rv > 0) {
                    /* Single-pipe action. */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_remove(ent_hdl, fld_name));

                    if (lt_info->fld_bmp & (1 << obj_id)) {
                        fld_name = lt_info->flds[obj_id].name;
                        SHR_IF_ERR_EXIT_EXCEPT_IF
                            (bcmlt_entry_field_remove(ent_hdl, fld_name),
                             SHR_E_NOT_FOUND);
                    }
                }
            }
        } else {
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update vlan xlate pipe sw info.
 *
 * \param [in] unit              Unit number.
 * \param [in] pg                Port group.
 * \param [in] ingress           Ingress/egress.
 * \param [in] bmp               Pipe bitmap.
 * \param [in] op_code           Op code: Insert/Delete.
 * \param [in] ctrl              Pipe control pointer.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_xlate_pipe_update_bmp(int unit, bcm_port_t pg, int ingress,
                           uint16_t pbmp, bcmi_ltsw_vlan_table_opcode_t op_code,
                           ltsw_xlate_pipe_control_t *ctrl)
{
    ltsw_vlan_pg_info_t *pg_info = NULL;
    bcmint_vlan_xlate_key_info_t *key = NULL;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int i, j, only_sw_update = 0, ing_lock = 0, egr_lock = 0;
    bcm_vlan_translate_key_t key_type;
    vlan_xlate_table_id_t table_id;
    bcmlt_opcode_t op;
    vlan_xlate_table_id_t lt_id = VLAN_EGR_XLATE_DW_EN(unit) ?
                                  XLATE_TABLE_EGR_DW : XLATE_TABLE_EGR;
    SHR_FUNC_ENTER(unit);

    /* Only insert and delete operation are supported in this function. */
    if (op_code == bcmiVlanTableOpInsert) {
        op = BCMLT_OPCODE_INSERT;
    } else if (op_code == bcmiVlanTableOpDelete) {
        op = BCMLT_OPCODE_DELETE;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (ingress) {
        VLAN_ING_XLATE_LOCK(unit);
        ing_lock = 1;
        if (VLAN_ING_XLATE_CTRL_EN(unit) && !VLAN_ING_XLATE_PIPE_EN(unit)) {
            only_sw_update = 1;
        }
    } else {
       VLAN_EGR_XLATE_LOCK(unit);
       egr_lock = 1;
       if (VLAN_EGR_XLATE_CTRL_EN(unit) && !VLAN_EGR_XLATE_PIPE_EN(unit)) {
            only_sw_update = 1;
        }
    }
    pg_info = &(ctrl->pg_info[pg]);
    SHR_NULL_CHECK(pg_info->head, SHR_E_NOT_FOUND);

    if (!only_sw_update) {
        for (i = 0; i < VLAN_HASH_NUM; i++) {
            for (j = pg_info->head[i]; j != 0; j = ctrl->next[j]) {
                key = &(ctrl->key_array[j]);
                if(ingress) {
                    if (key->outer_vlan && key->inner_vlan) {
                        key_type = bcmVlanTranslateKeyPortGroupDouble;
                        table_id = XLATE_TABLE_ING_3;
                    } else if (key->outer_vlan) {
                        key_type = bcmVlanTranslateKeyPortGroupOuter;
                        table_id = XLATE_TABLE_ING_1;
                    } else {
                        key_type = bcmVlanTranslateKeyPortGroupInner;
                        table_id = XLATE_TABLE_ING_2;
                    }

                    if (!(ltsw_vlan_info[unit].table_bmp & (1 << table_id))) {
                        continue;
                    }

                    SHR_IF_ERR_VERBOSE_EXIT
                        (vlan_ing_xlate_entry_key_set(unit, key_type,
                                                      key->pg_id, key->outer_vlan,
                                                      key->inner_vlan, &ent_hdl));
                    if (op == BCMLT_OPCODE_INSERT) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (vlan_xlate_entry_pipe_op(unit, table_id, 1,
                                                      BCMLT_OPCODE_LOOKUP,
                                                      pg_info->pipe_bmp, ent_hdl));

                        /* Do not copy single-pipe flexctr information. */
                        SHR_IF_ERR_VERBOSE_EXIT
                            (vlan_xlate_entry_pipe_copy_check(unit, table_id,
                                                              ent_hdl));
                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (vlan_xlate_entry_pipe_op(unit, table_id, 1, op,
                                                  pbmp, ent_hdl));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (vlan_egr_xlate_entry_key_set(unit, key->pg_id,
                                                      key->outer_vlan,
                                                      BCM_VLAN_INVALID, &ent_hdl));
                    if (op == BCMLT_OPCODE_INSERT) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (vlan_xlate_entry_pipe_op(unit, lt_id, 1,
                                                      BCMLT_OPCODE_LOOKUP,
                                                      pg_info->pipe_bmp, ent_hdl));

                        /* Do not copy single-pipe flexctr information. */
                        SHR_IF_ERR_VERBOSE_EXIT
                            (vlan_xlate_entry_pipe_copy_check(unit, lt_id,
                                                              ent_hdl));

                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (vlan_xlate_entry_pipe_op(unit, lt_id, 1,
                                                  op, pbmp, ent_hdl));
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_free(ent_hdl));
                ent_hdl = BCMLT_INVALID_HDL;
            }
        }
    }

    if (op == BCMLT_OPCODE_INSERT) {
        pg_info->pipe_bmp |= pbmp;
    } else {
        pg_info->pipe_bmp &= ~pbmp;
    }

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }

    if (ingress) {
        if(ing_lock) {
            VLAN_ING_XLATE_UNLOCK(unit);
        }
    } else {
        if (egr_lock) {
            VLAN_EGR_XLATE_UNLOCK(unit);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the VLAN membership information.
 *
 * This function is used to initialize the VLAN membership information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
vlan_membership_init(int unit)
{
    ltsw_vlan_info_t *vlan_info = &ltsw_vlan_info[unit];
    bcmint_vlan_membership_info_t *ha_ptr = NULL;
    uint32_t inst_size = 0;
    uint32_t array_size = 0;
    uint32_t alloc_size = 0;
    uint32_t entry_num = 0;
    uint32_t port_num = 0;
    bcm_port_t port = 0;
    uint16_t dis_flags = 0;
    int wb_flag = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    vlan_info->flags |= VLAN_F_VLAN_SW_MSHIP;

    wb_flag = bcmi_warmboot_get(unit);
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;

    /* Alloc HA memory for the VLAN membership. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, VLAN_ING_MEMBER_PROFILEs, &entry_num));
    inst_size = sizeof(bcmint_vlan_membership_info_t);
    array_size = port_num * entry_num;
    alloc_size = inst_size * array_size;
    ha_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_VLAN,
                                    BCMINT_VLAN_SUB_COMP_ID_MSHIP,
                                    "bcmLtswVlanMship", &alloc_size);
    SHR_NULL_CHECK(ha_ptr, SHR_E_MEMORY);

    vlan_info->mship = ha_ptr;
    if (!wb_flag) {
        /*
         * keep consistent with profile DB since VLAN_ING/EGR_MEMBER_PROFILEs
         * are cleared only during bcm_init.
         */
        if (bcmi_ltsw_init_state_get(unit)) {
            sal_memset(ha_ptr, 0, alloc_size);
        }
    }

    rv = bcmi_ltsw_issu_struct_info_report(unit,
              BCMI_HA_COMP_ID_VLAN,
              BCMINT_VLAN_SUB_COMP_ID_MSHIP,
              0, inst_size, array_size,
              BCMINT_VLAN_MEMBERSHIP_INFO_T_ID);

    if (rv != SHR_E_EXISTS) {
        if (SHR_SUCCESS(rv) && wb_flag) {
            /* ISSU case: Did not support this feature in previous version. */
            vlan_info->flags &= ~VLAN_F_VLAN_SW_MSHIP;
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    if (VLAN_SW_MSHIP_EN(unit)) {
        /* The status to disable membership check per port. */
        alloc_size = port_num * sizeof(uint8_t);
        SHR_ALLOC(vlan_info->port_dis_chk,
                  alloc_size, "bcmLtswVlanDisChk");
        SHR_NULL_CHECK(vlan_info->port_dis_chk, SHR_E_MEMORY);
        sal_memset(vlan_info->port_dis_chk, 0, alloc_size);
        for (port = 0; port < port_num; port++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_vlan_check_disable_get(unit, port, &dis_flags));

            if (!dis_flags) {
                continue;
            }
            if (dis_flags & BCMI_LTSW_VLAN_CHK_ING) {
                vlan_info->port_dis_chk[port] |= VLAN_PORT_DIS_ING_CHK;
            }
            if (dis_flags & BCMI_LTSW_VLAN_CHK_ING_EGR) {
                vlan_info->port_dis_chk[port] |= VLAN_PORT_DIS_ING_EGR_CHK;
            }
            if (dis_flags & BCMI_LTSW_VLAN_CHK_EGR) {
                vlan_info->port_dis_chk[port] |= VLAN_PORT_DIS_EGR_CHK;
            }
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!wb_flag && ha_ptr) {
            (void) bcmi_ltsw_ha_mem_free(unit, ha_ptr);
            ha_ptr = NULL;
            vlan_info->mship = NULL;
        }
        SHR_FREE(vlan_info->port_dis_chk);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Display VLAN xlate software structure information.
 *
 * \param [in] unit      Unit Number.
 * \param [in] ingress   Ingress/Egress.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
vlan_xlate_entry_dump(int unit, int ingress)
{
    ltsw_vlan_info_t *vlan_info = &ltsw_vlan_info[unit];
    ltsw_xlate_pipe_control_t *ctrl = NULL;
    ltsw_vlan_pg_info_t *pg_info = NULL;
    bcmint_vlan_xlate_key_info_t *key = NULL;
    uint32_t ing_lock = 0, egr_lock = 0, is_continue = 0;
    char *mode[2] = {"Global", "Pipe"};
    uint32_t pg_num;
    uint32_t i, j, m, n = 0, mode_idx, cnt, free_cnt = 0;
    SHR_FUNC_ENTER(unit);

    if (ingress) {
        if (!VLAN_ING_XLATE_CTRL_EN(unit) && !VLAN_ING_XLATE_PIPE_EN(unit)) {
            SHR_EXIT();
        }
        VLAN_ING_XLATE_LOCK(unit);
        ing_lock = 1;
        ctrl = &(vlan_info->xlate_pipe_ctrl[0]);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit, ING_L2_IIF_TABLEs, &pg_num));
    } else {
       if (!VLAN_EGR_XLATE_CTRL_EN(unit) && !VLAN_EGR_XLATE_PIPE_EN(unit)) {
            SHR_EXIT();
        }
       VLAN_EGR_XLATE_LOCK(unit);
       egr_lock = 1;
       ctrl = &(vlan_info->xlate_pipe_ctrl[1]);
       SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_capacity_get(unit, EGR_DVPs, &pg_num));
    }

    key = ctrl->key_array;
    pg_info = ctrl->pg_info;

    LOG_CLI((BSL_META_U(unit,
                        "\nVLAN XLATE SW Information - Unit %d\n"), unit));
    if (ingress) {
        for (i = 0; i < 3; i++) {
            mode_idx = vlan_info->table_bmp & (1 << i) ?  1 : 0;
            LOG_CLI((BSL_META_U(unit,
                            "\tING_VLAN_XLATE_%d: %d entries, %s mode\n"),
                            i+1, vlan_info->xlate_table_size[i],
                            mode[mode_idx]));
        }
    } else {
       mode_idx = vlan_info->table_bmp & (1 << 3) ?  1 : 0;
       LOG_CLI((BSL_META_U(unit,
                    "\tEGR_VLAN_XLATE: %d entries, %s mode\n"),
                    vlan_info->xlate_table_size[3],
                    mode[mode_idx]));
    }

    for (i = 0; i < pg_num; i++) {
        if (pg_info[i].pipe_bmp) {
            LOG_CLI((BSL_META_U(unit,
                    "\nPort Group: %d  Pipe Bitmap: 0x%x\n\t"),
                     i, pg_info[i].pipe_bmp));
            if (pg_info[i].head) {
                n = 0;
                cnt = 0;
                for (j = 0; j < VLAN_HASH_NUM; j++) {
                    for (m = pg_info[i].head[j]; m != 0; m = ctrl->next[m]) {
                        LOG_CLI((BSL_META_U(unit, "%d(%d, %d, %d), "), m,
                            key[m].outer_vlan, key[m].inner_vlan, key[m].pg_id));
                        n++;
                        cnt++;
                        if (n%5 == 0) {
                            n = 0;
                            LOG_CLI((BSL_META_U(unit, "\n\t")));
                        }
                    }
                }
                LOG_CLI((BSL_META_U(unit, "Total: %d\n\t"), cnt));
            }
        }
    }

    LOG_CLI((BSL_META_U(unit,"\nFree list:   \n\t")));
    for (m = ctrl->free_idx, n = m, cnt = 0; ; m = ctrl->next[m]) {
        if ((n == m + 1) || (m == n + 1)) {
            is_continue += 1;
        } else {
            if (is_continue) {
                LOG_CLI((BSL_META_U(unit, "-(%d)-%d"), is_continue, n));
                is_continue = 0;
                cnt += 3;
            }
            if (cnt) {
                LOG_CLI((BSL_META_U(unit, ", ")));
                if (cnt > 15) {
                    cnt = 0;
                    LOG_CLI((BSL_META_U(unit, "\n\t")));
                }
            }
            LOG_CLI((BSL_META_U(unit, "%d"), m));
            cnt++;
        }
        n = m;
        if (m == 0) {
            break;
        }
        free_cnt++;
    }

    LOG_CLI((BSL_META_U(unit, ", %d free entries left.\n\n"), free_cnt));

exit:
    if (ingress) {
        if(ing_lock) {
            VLAN_ING_XLATE_UNLOCK(unit);
        }
    } else {
        if (egr_lock) {
            VLAN_EGR_XLATE_UNLOCK(unit);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update ingress VLAN translation entries in all the pipes.
 *
 * This function is used to update ingress VLAN translation entries in all the
 * pipes. Please note strength profile index would not be updated.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 * \param [in] lock              Need profile lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_update_all(int unit,
                                bcm_vlan_translate_key_t key_type,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan,
                                bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg,
                                int lock)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    vlan_xlate_table_id_t tbl_id;
    int pipe_support = 0;
    uint16_t pipe_bmp = 0;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_ING_XLATE_LOCK(unit);
    }

    /* Set the key of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_key_set(unit, key_type, port,
                                      outer_vlan, inner_vlan, &ent_hdl));

    /* Set the fields of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_fields_set(unit, ent_hdl,
                                         key_type, cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port, 1,
                                   &pipe_support, &tbl_id, &pipe_bmp));

    /* Update ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_UPDATE, pipe_bmp,
                                  ent_hdl));
exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_ING_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update ingress VLAN translation entries in a given pipe.
 *
 * This function is used to update ingress VLAN translation entries in a given
 * pipe. Please note strength profile index would not be updated.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 * \param [in] lock              Need profile lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_pipe_update(int unit,
                                 bcm_vlan_translate_key_t key_type,
                                 bcm_gport_t port,
                                 bcm_vlan_t outer_vlan,
                                 bcm_vlan_t inner_vlan,
                                 bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg,
                                 int lock)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    vlan_xlate_table_id_t tbl_id;
    int pipe_support = 0;
    uint16_t pipe_bmp = 0;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_ING_XLATE_LOCK(unit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port, 1,
                                   &pipe_support, &tbl_id, &pipe_bmp));

    if (!pipe_support ||
        !(cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_PIPE)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (!(pipe_bmp & (1 << cfg->pipe))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Set the key of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_key_set(unit, key_type, port,
                                      outer_vlan, inner_vlan, &ent_hdl));

    /* Set the fields of ingress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_fields_set(unit, ent_hdl,
                                         key_type, cfg));

    /* Update ingress xlate entry for a given pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_UPDATE,
                                  1 << (cfg->pipe),
                                  ent_hdl));
exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_ING_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ingress VLAN translation entry in a given pipe.
 *
 * This function is used to get ingress VLAN translation entry in a given pipe.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 * \param [in] lock              Need profile lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_pipe_get(int unit,
                              bcm_vlan_translate_key_t key_type,
                              bcm_gport_t port,
                              bcm_vlan_t outer_vlan,
                              bcm_vlan_t inner_vlan,
                              bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg,
                              int lock)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    vlan_xlate_table_id_t tbl_id;
    uint16_t pipe_bmp = 0;
    int pipe_support = 0;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_ING_XLATE_LOCK(unit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port, 1,
                                   &pipe_support, &tbl_id, &pipe_bmp));

    if (!pipe_support ||
        !(cfg->fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_PIPE)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (!(pipe_bmp & (1 << cfg->pipe))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_key_set(unit, key_type, port,
                                      outer_vlan, inner_vlan,
                                      &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_LOOKUP,
                                  1 << (cfg->pipe),
                                  ent_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_fields_get(unit, ent_hdl, cfg));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_ING_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get ingress VLAN translation entries in all pipes.
 *
 * This function is used to get ingress VLAN translation entries in all pipes.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfgs              Ingress xlate configurations.
 * \param [in] cfg_count         Number of ingress xlate configurations.
 * \param [in] lock              Need profile lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_entry_get_all(int unit,
                             bcm_vlan_translate_key_t key_type,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             bcmi_ltsw_vlan_ing_xlate_cfg_t *cfgs,
                             int *cfg_count,
                             int lock)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    vlan_xlate_table_id_t tbl_id;
    int pipe_support = 0;
    uint16_t pipe_bmp = 0;
    int pipe = 0;
    int count = 0;
    int pipe_num = 0;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_ING_XLATE_LOCK(unit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port,
                                   1, &pipe_support, &tbl_id, &pipe_bmp));

    if (pipe_support) {
        /* Get per-pipe entry information. */
        pipe_num = bcmi_ltsw_dev_pp_pipe_max(unit) + 1;
        for (pipe = 0; pipe < pipe_num; pipe++) {

            if (!(pipe_bmp & (1 << pipe))) {
                continue;
            }

            cfgs[count].fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_PIPE;
            cfgs[count].pipe = pipe;

            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_xlate_entry_pipe_get(unit, key_type, port,
                                               outer_vlan, inner_vlan,
                                               &cfgs[count], 0));
            count++;
        }
    } else {
        /* Get global entry information. */
        SHR_IF_ERR_VERBOSE_EXIT
             (vlan_ing_xlate_entry_get(unit, key_type, port,
                                       outer_vlan, inner_vlan,
                                       &cfgs[count], 0));
        count++;
    }

    *cfg_count = count;

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_ING_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update egress VLAN translation entries in all the pipes.
 *
 * This function is used to update egress VLAN translation entries in all the
 * pipes. Please note strength profile index would not be updated.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Egress xlate configuration.
 * \param [in] lock              Need profile lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_update_all(int unit,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan,
                                ltsw_vlan_egr_xlate_cfg_t *cfg,
                                int lock)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    vlan_xlate_table_id_t tbl_id;
    int pipe_support = 0;
    uint16_t pipe_bmp = 0;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_EGR_XLATE_LOCK(unit);
    }

    /* Set the key of egress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_key_set(unit, port,
                                      outer_vlan, inner_vlan, &ent_hdl));

    /* Set the fields of egress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_fields_set(unit, ent_hdl, cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid, port,
                                   0, &pipe_support, &tbl_id, &pipe_bmp));

    /* Update egress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_UPDATE, pipe_bmp,
                                  ent_hdl));
exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_EGR_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update egress VLAN translation entries in a given pipe.
 *
 * This function is used to update egress VLAN translation entries in a given
 * pipe. Please note strength profile index would not be updated.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Egress xlate configuration.
 * \param [in] lock              Need profile lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_pipe_update(int unit,
                                 bcm_gport_t port,
                                 bcm_vlan_t outer_vlan,
                                 bcm_vlan_t inner_vlan,
                                 ltsw_vlan_egr_xlate_cfg_t *cfg,
                                 int lock)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    vlan_xlate_table_id_t tbl_id;
    int pipe_support = 0;
    uint16_t pipe_bmp = 0;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_EGR_XLATE_LOCK(unit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid, port,
                                   0, &pipe_support, &tbl_id, &pipe_bmp));

    if (!pipe_support ||
        !(cfg->fld_bmp & EVT_FLD_PIPE)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (!(pipe_bmp & (1 << cfg->pipe))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Set the key of egress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_key_set(unit, port,
                                      outer_vlan, inner_vlan, &ent_hdl));

    /* Set the fields of egress vxlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_fields_set(unit, ent_hdl, cfg));

    /* Update egress xlate entry for a given pipe. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_UPDATE,
                                  1 << (cfg->pipe),
                                  ent_hdl));
exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_EGR_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get egress VLAN translation entry in a given pipe.
 *
 * This function is used to get egress VLAN translation entry in a given pipe.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Egress xlate configuration.
 * \param [in] lock              Need profile lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_pipe_get(int unit,
                              bcm_gport_t port,
                              bcm_vlan_t outer_vlan,
                              bcm_vlan_t inner_vlan,
                              ltsw_vlan_egr_xlate_cfg_t *cfg,
                              int lock)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    vlan_xlate_table_id_t tbl_id;
    uint16_t pipe_bmp = 0;
    int pipe_support = 0;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_EGR_XLATE_LOCK(unit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid, port,
                                   0, &pipe_support, &tbl_id, &pipe_bmp));

    if (!pipe_support ||
        !(cfg->fld_bmp & EVT_FLD_PIPE)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (!(pipe_bmp & (1 << cfg->pipe))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_key_set(unit, port,
                                      outer_vlan, inner_vlan,
                                      &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_op(unit, tbl_id, pipe_support,
                                  BCMLT_OPCODE_LOOKUP,
                                  1 << (cfg->pipe),
                                  ent_hdl));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_fields_get(unit, ent_hdl, cfg));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_EGR_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get egress VLAN translation entries in all pipes.
 *
 * This function is used to get egress VLAN translation entries in all pipes.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfgs              Egress xlate configurations.
 * \param [in] cfg_count         Number of egress xlate configurations.
 * \param [in] lock              Need profile lock.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_entry_get_all(int unit,
                             bcm_gport_t port,
                             bcm_vlan_t outer_vlan,
                             bcm_vlan_t inner_vlan,
                             ltsw_vlan_egr_xlate_cfg_t *cfgs,
                             int *cfg_count,
                             int lock)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    vlan_xlate_table_id_t tbl_id;
    int pipe_support = 0;
    uint16_t pipe_bmp = 0;
    int pipe = 0;
    int count = 0;
    int pipe_num = 0;

    SHR_FUNC_ENTER(unit);

    if (lock) {
        VLAN_EGR_XLATE_LOCK(unit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid, port,
                                   0, &pipe_support, &tbl_id, &pipe_bmp));

    if (pipe_support) {
        /* Get per-pipe entry information. */
        pipe_num = bcmi_ltsw_dev_pp_pipe_max(unit) + 1;
        for (pipe = 0; pipe < pipe_num; pipe++) {

            if (!(pipe_bmp & (1 << pipe))) {
                continue;
            }

            cfgs[count].fld_bmp |= EVT_FLD_PIPE;
            cfgs[count].pipe = pipe;

            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_xlate_entry_pipe_get(unit, port,
                                               outer_vlan, inner_vlan,
                                               &cfgs[count], 0));
            count++;
        }
    } else {
        /* Get global entry information. */
        SHR_IF_ERR_VERBOSE_EXIT
             (vlan_egr_xlate_entry_get(unit, port,
                                       outer_vlan, inner_vlan,
                                       &cfgs[count], 0));
        count++;
    }

    *cfg_count = count;

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    if (lock) {
        VLAN_EGR_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the validation of ingress translate flexctr configuration.
 *
 * This function is used to check the validation of ingress translate
 * flexctr configuration.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_flexctr_validate(int unit,
                                bcm_vlan_translate_key_t key_type,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan)
{
    const bcmint_vlan_lt_t *lt_info;
    bcmint_vlan_lt_id_t lt_id;

    SHR_FUNC_ENTER(unit);

    switch (key_type) {
        case bcmVlanTranslateKeyPortGroupDouble:
            lt_id = BCMINT_LTSW_VLAN_ING_VLAN_XLATE_3_TABLE;
            break;
        case bcmVlanTranslateKeyPortGroupOuter:
            lt_id = BCMINT_LTSW_VLAN_ING_VLAN_XLATE_1_TABLE;
            break;
        case bcmVlanTranslateKeyPortGroupInner:
            lt_id = BCMINT_LTSW_VLAN_ING_VLAN_XLATE_2_TABLE;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id, &lt_info));

    if (!(lt_info->fld_bmp &
         (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_FLEX_CTR_ACTION)) ||
        !(lt_info->fld_bmp &
         (1 << BCMINT_LTSW_VLAN_FLD_ING_XLATE_OPQ_OBJ_0))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_cfg_validate(unit, key_type, port,
                                     outer_vlan, inner_vlan, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach flex-counter into a given ingress VLAN translation table.
 *
 * This function is used to attach the count ID and object value into a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_flexctr_attach(int unit,
                              bcm_vlan_translate_key_t key_type,
                              bcm_gport_t port,
                              bcm_vlan_t outer_vlan,
                              bcm_vlan_t inner_vlan,
                              bcm_vlan_flexctr_config_t *config)

{
    bcmi_ltsw_flexctr_counter_info_t counter_info;
    bcmi_ltsw_vlan_ing_xlate_cfg_t vt_cfg;
    vlan_xlate_table_id_t vt_tbl_id;
    int update_object_only = 0;
    int vt_pipe_unique = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port, 1,
                                   &vt_pipe_unique, &vt_tbl_id, NULL));

    sal_memset(&counter_info, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                               config->stat_counter_id,
                                               &counter_info));

    if (counter_info.source != bcmFlexctrSourceIngVlanTranslate) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (counter_info.single_pipe) {
        /*
         * It's not expected to attach single-pipe action into global VT entry.
         * Available case would be: pipe_unique VT + pipe_unique Flexctr.
         */
        if (!vt_pipe_unique) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* pipe_unique VT + single-pipe Flexctr action. */
        sal_memset(&vt_cfg, 0, sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));
        vt_cfg.fld_bmp = BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION |
                         BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0 |
                         BCMI_LTSW_VLAN_ING_XLATE_FLD_PIPE;
        vt_cfg.pipe = counter_info.pipe;

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_entry_pipe_get(unit, key_type, port,
                                           outer_vlan, inner_vlan,
                                           &vt_cfg, 0));
    } else {
        /*
         * Attach global flexctr action to all VT entries.
         *   - global VT + global Flexctr action
         *   - pipe_unique VT + global Flexctr action
         */

        sal_memset(&vt_cfg, 0, sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));
        vt_cfg.fld_bmp = BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION |
                         BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0;

        SHR_IF_ERR_VERBOSE_EXIT
             (vlan_ing_xlate_entry_get(unit, key_type, port,
                                       outer_vlan, inner_vlan,
                                       &vt_cfg, 0));
    }

    if (vt_cfg.flex_ctr_action != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {

        /* Check flexctr action mode. */
        rv = bcmi_ltsw_flexctr_action_is_single_pipe(
                 unit, BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
                 vt_cfg.flex_ctr_action);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            if ((counter_info.single_pipe && (rv == 0)) ||
                (!(counter_info.single_pipe) && (rv > 0))) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
            }
        }

        /* Update flexctr index/object only. */
        if ((vt_cfg.flex_ctr_action == counter_info.action_index) &&
            (vt_cfg.opaque_obj0 != config->object_value)) {
            update_object_only = 1;
        } else {
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }

    sal_memset(&vt_cfg, 0, sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));

    if (!update_object_only) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_attach_counter_id_status_update(
                unit, config->stat_counter_id));

        vt_cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION;
        vt_cfg.flex_ctr_action = counter_info.action_index;
    }

    vt_cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0;
    vt_cfg.opaque_obj0 = config->object_value;

    /* Set flexctr information. */
    if (counter_info.single_pipe) {
        vt_cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_PIPE;
        vt_cfg.pipe = counter_info.pipe;
        rv = vlan_ing_xlate_entry_pipe_update(unit, key_type, port,
                                              outer_vlan, inner_vlan,
                                              &vt_cfg, 0);
    } else {
        rv = vlan_ing_xlate_entry_update_all(unit, key_type, port,
                                             outer_vlan, inner_vlan,
                                             &vt_cfg, 0);
    }

    if (SHR_FAILURE(rv)) {
        if (!update_object_only) {
            (void)bcmi_ltsw_flexctr_detach_counter_id_status_update(
                    unit, config->stat_counter_id);
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex-counter from a given ingress vlan translation table.
 *
 * This function is used to get the count id and object value from a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] array_size        Size of allocated entries in config array.
 * \param [out]config_array      Array of flex counter configurations.
 * \param [out]count             Number of elements in config array. If
 *                               array_size is 0, config array will be ignored
 *                               and count will be filled with an actual number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_flexctr_get(int unit,
                           bcm_vlan_translate_key_t key_type,
                           bcm_gport_t port,
                           bcm_vlan_t outer_vlan,
                           bcm_vlan_t inner_vlan,
                           int array_size,
                           bcm_vlan_flexctr_config_t *config_array,
                           int *count)
{
    bcmi_ltsw_flexctr_counter_info_t counter_info;
    bcmi_ltsw_vlan_ing_xlate_cfg_t *vt_cfgs = NULL;
    int vt_cfg_count = 0;
    int pipe_num_max = 0;
    int local_count = 0;
    int i = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    /*
     * qestions
     * How to check if action is global or single-pipe based on action index.
     * How to get counter_id:
     *     three cases:  - single-pipe mode + single-pipe action
     *                   - single-pipe mode + global action
     *                   - global mode + global action
     */
    pipe_num_max = bcmi_ltsw_dev_pp_pipe_max(unit) + 1;

    SHR_ALLOC(vt_cfgs,
              sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t) * pipe_num_max,
              "bcmLtswVlanIvtCfg");
    SHR_NULL_CHECK(vt_cfgs, SHR_E_MEMORY);
    sal_memset(vt_cfgs, 0,
               sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t) * pipe_num_max);

    for (i = 0; i < pipe_num_max; i++) {
        vt_cfgs[i].fld_bmp = BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION |
                             BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_get_all(unit, key_type, port,
                                      outer_vlan, inner_vlan,
                                      vt_cfgs, &vt_cfg_count, 0));

    for (i = 0; i < vt_cfg_count; i++) {

        if (vt_cfgs[i].flex_ctr_action == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
            continue;
        }

        if ((array_size > 0) && (array_size > local_count)) {
            sal_memset(&counter_info, 0,
                       sizeof(bcmi_ltsw_flexctr_counter_info_t));

            if (vt_cfgs[i].fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_PIPE) {
                counter_info.pipe = vt_cfgs[i].pipe;
            }

            counter_info.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
            counter_info.source = bcmFlexctrSourceIngVlanTranslate;
            counter_info.action_index = vt_cfgs[i].flex_ctr_action;
            counter_info.stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;

            SHR_IF_ERR_VERBOSE_EXIT
               (bcmi_ltsw_flexctr_counter_id_get(
                   unit, &counter_info,
                   &(config_array[local_count].stat_counter_id)));
            config_array[local_count].object_value = vt_cfgs[i].opaque_obj0;
        }

        local_count++;

        if (vt_cfgs[i].fld_bmp & BCMI_LTSW_VLAN_ING_XLATE_FLD_PIPE) {
            /* All pipe entries shall share global flexctr action. */
            rv = bcmi_ltsw_flexctr_action_is_single_pipe(
                     unit, BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
                     vt_cfgs[i].flex_ctr_action);
            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            } else {
                /* Break for Global action. */
                if (!rv) {
                    break;
                }
            }
        }
    }

    *count = local_count;

exit:
    SHR_FREE(vt_cfgs);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach flex-counter from a given ingress vlan translation table.
 *
 * This function is used to detach the count ID and object value from a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_flexctr_detach(int unit,
                              bcm_vlan_translate_key_t key_type,
                              bcm_gport_t port,
                              bcm_vlan_t outer_vlan,
                              bcm_vlan_t inner_vlan,
                              bcm_vlan_flexctr_config_t *config)
{
    bcmi_ltsw_flexctr_counter_info_t counter_info;
    bcmi_ltsw_vlan_ing_xlate_cfg_t vt_cfg;
    vlan_xlate_table_id_t vt_tbl_id;
    int vt_pipe_unique = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, key_type, port, 1,
                                   &vt_pipe_unique, &vt_tbl_id, NULL));

    sal_memset(&counter_info, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&vt_cfg, 0, sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                               config->stat_counter_id,
                                               &counter_info));

    if (counter_info.source != bcmFlexctrSourceIngVlanTranslate) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (counter_info.single_pipe) {
        /*
         * It's not expected to detach single-pipe action into global VT entry.
         * Available case would be: pipe_unique VT + pipe_unique Flexctr.
         */
        if (!vt_pipe_unique) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* pipe_unique VT + single-pipe Flexctr action. */
        vt_cfg.fld_bmp = BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION |
                         BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0 |
                         BCMI_LTSW_VLAN_ING_XLATE_FLD_PIPE;

        vt_cfg.pipe = counter_info.pipe;
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_entry_pipe_get(unit, key_type, port,
                                           outer_vlan, inner_vlan,
                                           &vt_cfg, 0));

    } else {
        /*
         * Detach global flexctr action from all VT entries.
         *   - global VT + global Flexctr action
         *   - pipe_unique VT + global Flexctr action
         */
        vt_cfg.fld_bmp = BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION |
                         BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0;

        SHR_IF_ERR_VERBOSE_EXIT
           (vlan_ing_xlate_entry_get(unit, key_type, port,
                                     outer_vlan, inner_vlan,
                                     &vt_cfg, 0));
    }

    if (vt_cfg.flex_ctr_action != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        /* Check flexctr action mode. */
        rv = bcmi_ltsw_flexctr_action_is_single_pipe(
                 unit, BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
                 vt_cfg.flex_ctr_action);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            if ((counter_info.single_pipe && (rv == 0)) ||
                (!(counter_info.single_pipe) && (rv > 0))) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
        }
    }

    if (vt_cfg.flex_ctr_action != counter_info.action_index) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Clear flexctr informaiton per pipe. */
    vt_cfg.flex_ctr_action = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    vt_cfg.opaque_obj0 = 0;

    if (counter_info.single_pipe) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_entry_pipe_update(unit, key_type, port,
                                              outer_vlan, inner_vlan,
                                              &vt_cfg, 0));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_entry_update_all(unit, key_type, port,
                                             outer_vlan, inner_vlan,
                                             &vt_cfg, 0));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(
            unit, config->stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach all flex-counters from a given ingress vlan translation table.
 *
 * This function is used to detach all the count IDs and object values from a
 * given ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_ing_xlate_flexctr_detach_all(int unit,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_gport_t port,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan)
{
    bcm_vlan_flexctr_config_t *flexctr_cfgs = NULL;
    bcmi_ltsw_vlan_ing_xlate_cfg_t vt_cfg;
    int pipe_num_max = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    /* Detach all flexctr information. */
    pipe_num_max = bcmi_ltsw_dev_pp_pipe_max(unit) + 1;

    SHR_ALLOC(flexctr_cfgs,
              sizeof(bcm_vlan_flexctr_config_t) * pipe_num_max,
              "bcmLtswVlanIvtCtrCfg");
    SHR_NULL_CHECK(flexctr_cfgs, SHR_E_MEMORY);
    sal_memset(flexctr_cfgs, 0,
               sizeof(bcm_vlan_flexctr_config_t) * pipe_num_max);

    SHR_IF_ERR_VERBOSE_EXIT
       (vlan_ing_xlate_flexctr_get(unit, key_type, port,
                                   outer_vlan, inner_vlan,
                                   pipe_num_max, flexctr_cfgs,
                                   &pipe_num_max));

    sal_memset(&vt_cfg, 0, sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));
    vt_cfg.fld_bmp = BCMI_LTSW_VLAN_ING_XLATE_FLD_FLEX_CTR_ACTION |
                     BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_OBJ_0;

    vt_cfg.flex_ctr_action = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    vt_cfg.opaque_obj0 = 0;

    /* Clear flexctr information in all pipes. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_update_all(unit, key_type, port,
                                         outer_vlan, inner_vlan,
                                         &vt_cfg, 0));

    for (i = 0; i < pipe_num_max; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(
                unit, flexctr_cfgs[i].stat_counter_id));
    }

exit:
    SHR_FREE(flexctr_cfgs);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the validation of egress translate flexctr configuration.
 *
 * This function is used to check the validation of egress translate
 * flexctr configuration.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_flexctr_validate(int unit,
                                bcm_gport_t port,
                                bcm_vlan_t outer_vlan,
                                bcm_vlan_t inner_vlan)
{
    const bcmint_vlan_lt_t *lt_info;
    bcmint_vlan_lt_id_t lt_id;

    SHR_FUNC_ENTER(unit);

    lt_id = BCMINT_LTSW_VLAN_EGR_VLAN_TRANSLATION_DW;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, lt_id, &lt_info));

    if (!(lt_info->fld_bmp &
         (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_ACTION)) ||
        !(lt_info->fld_bmp &
         (1 << BCMINT_LTSW_VLAN_FLD_EGR_XLATE_FLEX_CTR_INDEX))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_cfg_validate(unit, port,
                                     outer_vlan, inner_vlan, NULL));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach flex-counter into a given egress VLAN translation table.
 *
 * This function is used to attach the count ID and object value into a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_flexctr_attach(int unit,
                              bcm_gport_t port,
                              bcm_vlan_t outer_vlan,
                              bcm_vlan_t inner_vlan,
                              bcm_vlan_flexctr_config_t *config)

{
    bcmi_ltsw_flexctr_counter_info_t counter_info;
    ltsw_vlan_egr_xlate_cfg_t vt_cfg;
    vlan_xlate_table_id_t vt_tbl_id;
    int update_object_only = 0;
    int vt_pipe_unique = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid, port,
                                   0, &vt_pipe_unique, &vt_tbl_id, NULL));

    sal_memset(&counter_info, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                               config->stat_counter_id,
                                               &counter_info));

    if (counter_info.source != bcmFlexctrSourceEgrVlanTranslate) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if(counter_info.single_pipe) {
        /*
         * It's not expected to attach single-pipe action into global VT entry.
         * Available case would be: pipe_unique VT + pipe_unique Flexctr.
         */
        if (!vt_pipe_unique) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* pipe_unique VT + single-pipe Flexctr action. */
        sal_memset(&vt_cfg, 0, sizeof(ltsw_vlan_egr_xlate_cfg_t));
        vt_cfg.fld_bmp = EVT_FLD_FLEX_CTR_ACTION |
                         EVT_FLD_FLEX_CTR_INDEX |
                         EVT_FLD_PIPE;
        vt_cfg.pipe = counter_info.pipe;

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_xlate_entry_pipe_get(unit, port,
                                           outer_vlan, inner_vlan,
                                           &vt_cfg, 0));
    } else {
        /*
         * Attach global flexctr action to all VT entries.
         *   - global VT + global Flexctr action
         *   - pipe_unique VT + global Flexctr action
         */

        sal_memset(&vt_cfg, 0, sizeof(ltsw_vlan_egr_xlate_cfg_t));
        vt_cfg.fld_bmp = EVT_FLD_FLEX_CTR_ACTION |
                         EVT_FLD_FLEX_CTR_INDEX;

        SHR_IF_ERR_VERBOSE_EXIT
             (vlan_egr_xlate_entry_get(unit, port,
                                       outer_vlan, inner_vlan,
                                       &vt_cfg, 0));
    }

    if (vt_cfg.flex_ctr_action != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        /* Check flexctr action mode. */
        rv = bcmi_ltsw_flexctr_action_is_single_pipe(
                 unit, BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
                 vt_cfg.flex_ctr_action);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            if ((counter_info.single_pipe && (rv == 0)) ||
                (!(counter_info.single_pipe) && (rv > 0))) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
            }
        }

        /* Update flexctr index/object only. */
        if ((vt_cfg.flex_ctr_action == counter_info.action_index) &&
            (vt_cfg.flex_ctr_index != config->object_value)) {
            update_object_only = 1;
        } else {
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }

    sal_memset(&vt_cfg, 0, sizeof(ltsw_vlan_egr_xlate_cfg_t));

    if (!update_object_only) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_attach_counter_id_status_update(
                unit, config->stat_counter_id));

        vt_cfg.fld_bmp |= EVT_FLD_FLEX_CTR_ACTION;
        vt_cfg.flex_ctr_action = counter_info.action_index;
    }

    vt_cfg.fld_bmp |= EVT_FLD_FLEX_CTR_INDEX;
    vt_cfg.flex_ctr_index = config->object_value;

    /* Set flexctr information. */
    if (counter_info.single_pipe) {
        vt_cfg.fld_bmp |= EVT_FLD_PIPE;
        vt_cfg.pipe = counter_info.pipe;
        rv = vlan_egr_xlate_entry_pipe_update(unit, port,
                                              outer_vlan, inner_vlan,
                                              &vt_cfg, 0);
    } else {
        rv = vlan_egr_xlate_entry_update_all(unit, port,
                                             outer_vlan, inner_vlan,
                                             &vt_cfg, 0);
    }

    if (SHR_FAILURE(rv)) {
        if (!update_object_only) {
            (void)bcmi_ltsw_flexctr_detach_counter_id_status_update(
                    unit, config->stat_counter_id);
        }
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex-counter from a given egress vlan translation table.
 *
 * This function is used to get the count id and object value from a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] array_size        Size of allocated entries in config array.
 * \param [out]config_array      Array of flex counter configurations.
 * \param [out]count             Number of elements in config array. If
 *                               array_size is 0, config array will be ignored
 *                               and count will be filled with an actual number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_flexctr_get(int unit,
                           bcm_gport_t port,
                           bcm_vlan_t outer_vlan,
                           bcm_vlan_t inner_vlan,
                           int array_size,
                           bcm_vlan_flexctr_config_t *config_array,
                           int *count)
{
    bcmi_ltsw_flexctr_counter_info_t counter_info;
    ltsw_vlan_egr_xlate_cfg_t *vt_cfgs = NULL;
    int vt_cfg_count = 0;
    int pipe_num_max = 0;
    int local_count = 0;
    int i = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    pipe_num_max = bcmi_ltsw_dev_pp_pipe_max(unit) + 1;

    SHR_ALLOC(vt_cfgs,
              sizeof(ltsw_vlan_egr_xlate_cfg_t) * pipe_num_max,
              "bcmLtswVlanEvtCfg");
    SHR_NULL_CHECK(vt_cfgs, SHR_E_MEMORY);
    sal_memset(vt_cfgs, 0,
               sizeof(ltsw_vlan_egr_xlate_cfg_t) * pipe_num_max);

    for (i = 0; i < pipe_num_max; i++) {
        vt_cfgs[i].fld_bmp = EVT_FLD_FLEX_CTR_ACTION |
                             EVT_FLD_FLEX_CTR_INDEX;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_get_all(unit, port,
                                      outer_vlan, inner_vlan,
                                      vt_cfgs, &vt_cfg_count, 0));

    for (i = 0; i < vt_cfg_count; i++) {

        if (vt_cfgs[i].flex_ctr_action == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
            continue;
        }

        if ((array_size > 0) && (array_size > local_count)) {
            sal_memset(&counter_info, 0,
                       sizeof(bcmi_ltsw_flexctr_counter_info_t));

            if (vt_cfgs[i].fld_bmp & EVT_FLD_PIPE) {
                counter_info.pipe = vt_cfgs[i].pipe;
            }

            counter_info.direction = BCMI_LTSW_FLEXCTR_DIR_EGRESS;
            counter_info.source = bcmFlexctrSourceEgrVlanTranslate;
            counter_info.action_index = vt_cfgs[i].flex_ctr_action;
            counter_info.stage = BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR;

            SHR_IF_ERR_VERBOSE_EXIT
               (bcmi_ltsw_flexctr_counter_id_get(
                   unit, &counter_info,
                   &(config_array[local_count].stat_counter_id)));
            config_array[local_count].object_value = vt_cfgs[i].flex_ctr_index;
        }

        local_count++;

        if (vt_cfgs[i].fld_bmp & EVT_FLD_PIPE) {
            /* All pipe entries shall share global flexctr action. */
            rv = bcmi_ltsw_flexctr_action_is_single_pipe(
                     unit, BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
                     vt_cfgs[i].flex_ctr_action);
            if (SHR_FAILURE(rv)) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            } else {
                /* Break for Global action. */
                if (rv == 0) {
                    break;
                }
            }
        }
    }

    *count = local_count;

exit:
    SHR_FREE(vt_cfgs);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach flex-counter from a given egress vlan translation table.
 *
 * This function is used to detach the count ID and object value from a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_flexctr_detach(int unit,
                              bcm_gport_t port,
                              bcm_vlan_t outer_vlan,
                              bcm_vlan_t inner_vlan,
                              bcm_vlan_flexctr_config_t *config)
{
    bcmi_ltsw_flexctr_counter_info_t counter_info;
    ltsw_vlan_egr_xlate_cfg_t vt_cfg;
    vlan_xlate_table_id_t vt_tbl_id;
    int vt_pipe_unique = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_entry_pipe_get(unit, bcmVlanTranslateKeyInvalid, port, 0,
                                   &vt_pipe_unique, &vt_tbl_id, NULL));

    sal_memset(&counter_info, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&vt_cfg, 0, sizeof(ltsw_vlan_egr_xlate_cfg_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                               config->stat_counter_id,
                                               &counter_info));

    if (counter_info.source != bcmFlexctrSourceEgrVlanTranslate) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (counter_info.single_pipe) {
        /*
         * It's not expected to detach single-pipe action for global VT entry.
         * Available case would be: pipe_unique VT + pipe_unique Flexctr.
         */
        if (!vt_pipe_unique) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* pipe_unique VT + single-pipe Flexctr action. */
        vt_cfg.fld_bmp = EVT_FLD_FLEX_CTR_ACTION |
                         EVT_FLD_FLEX_CTR_INDEX |
                         EVT_FLD_PIPE;

        vt_cfg.pipe = counter_info.pipe;
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_xlate_entry_pipe_get(unit, port,
                                           outer_vlan, inner_vlan,
                                           &vt_cfg, 0));
    } else {
        /*
         * Detach global flexctr action from all VT entries.
         *   - global VT + global Flexctr action
         *   - pipe_unique VT + global Flexctr action
         */
        vt_cfg.fld_bmp = EVT_FLD_FLEX_CTR_ACTION |
                         EVT_FLD_FLEX_CTR_INDEX;

        SHR_IF_ERR_VERBOSE_EXIT
           (vlan_egr_xlate_entry_get(unit, port,
                                     outer_vlan, inner_vlan,
                                     &vt_cfg, 0));

    }

    if (vt_cfg.flex_ctr_action != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        /* Check flexctr action mode. */
        rv = bcmi_ltsw_flexctr_action_is_single_pipe(
                 unit, BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
                 vt_cfg.flex_ctr_action);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        } else {
            if ((counter_info.single_pipe && (rv == 0)) ||
                (!(counter_info.single_pipe) && (rv > 0))) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
        }
    }

    if (vt_cfg.flex_ctr_action != counter_info.action_index) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Clear flexctr informaiton per pipe. */
    vt_cfg.flex_ctr_action = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    vt_cfg.flex_ctr_index = 0;

    if (counter_info.single_pipe) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_xlate_entry_pipe_update(unit, port,
                                              outer_vlan, inner_vlan,
                                              &vt_cfg, 0));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_xlate_entry_update_all(unit, port,
                                             outer_vlan, inner_vlan,
                                             &vt_cfg, 0));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(
            unit, config->stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach all flex-counters from a given egress vlan translation table.
 *
 * This function is used to detach all the count IDs and object values from a
 * given egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_egr_xlate_flexctr_detach_all(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan)
{
    bcm_vlan_flexctr_config_t *flexctr_cfgs = NULL;
    ltsw_vlan_egr_xlate_cfg_t vt_cfg;
    int pipe_num_max = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    /* Detach all flexctr information. */
    pipe_num_max = bcmi_ltsw_dev_pp_pipe_max(unit) + 1;

    SHR_ALLOC(flexctr_cfgs,
              sizeof(bcm_vlan_flexctr_config_t) * pipe_num_max,
              "bcmLtswVlanEvtCtrCfg");
    SHR_NULL_CHECK(flexctr_cfgs, SHR_E_MEMORY);
    sal_memset(flexctr_cfgs, 0,
               sizeof(bcm_vlan_flexctr_config_t) * pipe_num_max);

    SHR_IF_ERR_VERBOSE_EXIT
       (vlan_egr_xlate_flexctr_get(unit, port,
                                   outer_vlan, inner_vlan,
                                   pipe_num_max, flexctr_cfgs,
                                   &pipe_num_max));

    sal_memset(&vt_cfg, 0, sizeof(ltsw_vlan_egr_xlate_cfg_t));
    vt_cfg.fld_bmp = EVT_FLD_FLEX_CTR_ACTION |
                     EVT_FLD_FLEX_CTR_INDEX;

    vt_cfg.flex_ctr_action = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    vt_cfg.flex_ctr_index = 0;

    /* Clear flexctr information in all pipes. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_update_all(unit, port,
                                         outer_vlan, inner_vlan,
                                         &vt_cfg, 0));

    for (i = 0; i < pipe_num_max; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(
                unit, flexctr_cfgs[i].stat_counter_id));
    }

exit:
    SHR_FREE(flexctr_cfgs);
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize VLAN moudle.
 *
 * This function is used to initialize the all VLAN table information with
 * default VLAN.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xfs_ltsw_vlan_init(int unit)
{
    uint32_t num_prof;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_mutex_create(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_membership_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_block_mask_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_pipe_control_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, VLAN_ING_MEMBER_PROFILEs, &num_prof));
    VLAN_DLF_FLOOD(unit) =
        bcmi_ltsw_property_get(unit,
                               BCMI_CPN_VLAN_FLOODING_L2MC_NUM_RESERVED,
                               num_prof);
    VLAN_DLF_FLOOD_MODE(unit) =
        bcmi_ltsw_property_get(unit, BCMI_CPN_VLAN_FLOODING_MODE,
                               LTSW_VLAN_FLOOD_MODE_DEF);

    if ((VLAN_DLF_FLOOD_MODE(unit) != LTSW_VLAN_FLOOD_MODE_DEF) &&
        (VLAN_DLF_FLOOD_MODE(unit) != LTSW_VLAN_FLOOD_MODE_EXCLUDE_CPU)) {
        SHR_ERR_EXIT(BCM_E_CONFIG);
    }

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_reinit(unit));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_init(unit));

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_default_entry_add(unit));
    }

    ltsw_vlan_info[unit].inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        vlan_mutex_destroy(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the VLAN moudle.
 *
 * This function is used to de-initialize the VLAN moudle.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xfs_ltsw_vlan_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    ltsw_vlan_info[unit].inited = 0;
    ltsw_vlan_info[unit].flags = 0;

    SHR_FREE(ltsw_vlan_info[unit].port_dis_chk);

    vlan_mutex_destroy(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_xlate_pipe_control_cleanup(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize outer TPID information.
 *
 * This function is used to initialize outer TPID information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xfs_ltsw_vlan_otpid_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_otpid_table_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach outer TPID information.
 *
 * This function is used to detach outer TPID information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xfs_ltsw_vlan_otpid_detach(int unit)
{
    COMPILER_REFERENCE(unit);
    return SHR_E_NONE;
}

/*!
 * \brief Add VLAN outer TPID.
 *
 * \param [in] unit      Unit Number.
 * \param [in] index     Entry index.
 * \param [in] tpid      Outer TPID value.
 *
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
xfs_ltsw_vlan_otpid_add(int unit, int index, uint16_t tpid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_otpid_entry_add(unit, index, tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_otpid_info_tpid_set(unit, index, tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_otpid_info_ref_count_add(unit, index, 1));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set global inner TPID.
 *
 * This function is used to set global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in]  tpid                 TPID to be set.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xfs_ltsw_vlan_itpid_set(int unit, uint16_t tpid)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, R_ING_INNER_TPIDs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, TPIDs, (uint64_t)tpid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, ent_hdl, BCMLT_PRIORITY_NORMAL));

    /*
     * Per legacy behavior, the configurations between Port TPID and OBM TPID
     * must be in sync.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_obm_pm_port_control_set(unit, -1,
                                           bcmiObmPortControlInnerTpid,
                                           (int)tpid));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get global inner TPID.
 *
 * This function is used to get global inner TPID.
 *
 * \param [in]  unit                 Unit Number.
 * \param [out] tpid                 Inner TPID.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
int
xfs_ltsw_vlan_itpid_get(int unit, uint16_t *tpid)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    uint64_t value = 0;
    int dunit = 0;
    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, R_ING_INNER_TPIDs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, TPIDs, &value));
    *tpid = (uint16_t)value;

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void) bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add member ports into a specified vlan.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  ing_pbmp      Ingress member ports.
 * \param [in]  egr_pbmp      Egress member ports.
 * \param [in]  egr_ubmp      Untagged member ports.
 * \param [in]  flags         BCM_VLAN_PORT_XXX flags.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
xfs_ltsw_vlan_port_add(int unit, bcm_vlan_t vid, bcm_pbmp_t ing_pbmp,
                       bcm_pbmp_t egr_pbmp, bcm_pbmp_t egr_ubmp, int flags)
{
    bcm_pbmp_t ori_ing_egr_pbmp, ori_egr_pbmp;
    bcm_pbmp_t ori_egr_ubmp, ori_ing_pbmp;
    bcm_pbmp_t ubmp, pbmp, ie_pbmp, ing_egr_pbmp;
    int ing_prof_idx, egr_prof_idx, untag_prof_idx, l2mc_idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vid, &ori_ing_pbmp, &ori_ing_egr_pbmp,
                             &ori_egr_pbmp, &ori_egr_ubmp, &ing_prof_idx,
                             &egr_prof_idx, &untag_prof_idx, &l2mc_idx));

    /* Make the port tagged/untagged if its already added to the vlan. */
    BCM_PBMP_ASSIGN(ubmp, ori_egr_ubmp);
    BCM_PBMP_ASSIGN(pbmp, ori_egr_pbmp);
    BCM_PBMP_ASSIGN(ie_pbmp, ori_ing_egr_pbmp);
    BCM_PBMP_ASSIGN(ing_egr_pbmp, egr_pbmp);

    BCM_PBMP_XOR(ubmp, egr_ubmp);
    BCM_PBMP_REMOVE(pbmp, ubmp);
    BCM_PBMP_REMOVE(ie_pbmp, ubmp);

    BCM_PBMP_REMOVE(egr_pbmp, pbmp);
    BCM_PBMP_REMOVE(egr_ubmp, pbmp);
    BCM_PBMP_REMOVE(ing_egr_pbmp, ie_pbmp);
    BCM_PBMP_REMOVE(ing_pbmp, ori_ing_pbmp);

    /* Only allow untagged ports on the vlan. */
    BCM_PBMP_AND(egr_ubmp, egr_pbmp);

    if (BCM_PBMP_NOT_NULL(egr_pbmp) ||
        BCM_PBMP_NOT_NULL(egr_ubmp) ||
        BCM_PBMP_NOT_NULL(ing_pbmp) ||
        BCM_PBMP_NOT_NULL(ing_egr_pbmp)) {

        /* Set egress untagged ports. */
        BCM_PBMP_REMOVE(ori_egr_ubmp, egr_pbmp);
        BCM_PBMP_OR(ori_egr_ubmp, egr_ubmp);

        /* Set ingress egress ports. */
        if (!(flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
            BCM_PBMP_OR(ori_ing_egr_pbmp, ing_egr_pbmp);
        }

        /* Set ingress ports. */
        BCM_PBMP_OR(ori_ing_pbmp, ing_pbmp);

        /* Set egress ports. */
        BCM_PBMP_OR(ori_egr_pbmp, egr_pbmp);

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_entry_port_set(unit, vid,
                                 ori_ing_pbmp, ori_ing_egr_pbmp,
                                 ori_egr_pbmp, ori_egr_ubmp));

        /* Delete old profile. */
        if (VLAN_DLF_FLOOD(unit) && (vid <= BCM_VLAN_MAX)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_multicast_vlan_flood_grp_destroy(unit, l2mc_idx));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_mshp_chk_profile_delete(unit, ing_prof_idx));

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_mshp_chk_profile_delete(unit, egr_prof_idx));

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_untag_profile_delete(unit, untag_prof_idx));
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Get member ports from a specified vlan.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [out] ing_pbmp      Ingress member ports.
 * \param [out] egr_pbmp      Egress member ports.
 * \param [out] egr_ubmp      Untagged member ports.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
xfs_ltsw_vlan_port_get(int unit, bcm_vlan_t vid, bcm_pbmp_t *ing_pbmp,
                       bcm_pbmp_t *egr_pbmp, bcm_pbmp_t *egr_ubmp)
{
    bcm_pbmp_t ing_egr_pbmp;

    SHR_FUNC_ENTER(unit);

    BCM_PBMP_CLEAR(ing_egr_pbmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vid, ing_pbmp, &ing_egr_pbmp,
                             egr_pbmp, egr_ubmp, NULL, NULL, NULL, NULL));

    if (egr_pbmp) {
        BCM_PBMP_OR(*egr_pbmp, ing_egr_pbmp);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove member ports from a specified vlan.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  vid           VLAN ID.
 * \param [in]  ing_pbmp      Ingress member ports.
 * \param [in]  egr_pbmp      Egress member ports.
 * \param [in]  egr_ubmp      Untagged member ports.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
xfs_ltsw_vlan_port_remove(int unit,
                          bcm_vlan_t vid,
                          bcm_pbmp_t ing_pbmp,
                          bcm_pbmp_t egr_pbmp,
                          bcm_pbmp_t egr_ubmp)
{
    bcm_pbmp_t ori_ing_egr_pbmp, ori_egr_pbmp;
    bcm_pbmp_t ori_egr_ubmp, ori_ing_pbmp;
    int ing_prof_idx, egr_prof_idx, untag_prof_idx, l2mc_idx;

    SHR_FUNC_ENTER(unit);

    /* Remove member ports. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vid, &ori_ing_pbmp, &ori_ing_egr_pbmp,
                             &ori_egr_pbmp, &ori_egr_ubmp, &ing_prof_idx,
                             &egr_prof_idx, &untag_prof_idx, &l2mc_idx));

    BCM_PBMP_REMOVE(ori_ing_pbmp, ing_pbmp);
    BCM_PBMP_REMOVE(ori_ing_egr_pbmp, egr_pbmp);
    BCM_PBMP_REMOVE(ori_egr_pbmp, egr_pbmp);
    BCM_PBMP_REMOVE(ori_egr_ubmp, egr_ubmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_set(unit, vid,
                             ori_ing_pbmp, ori_ing_egr_pbmp,
                             ori_egr_pbmp, ori_egr_ubmp));

    /* Delete old profile. */
    if (VLAN_DLF_FLOOD(unit) && (vid <= BCM_VLAN_MAX)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_vlan_flood_grp_destroy(unit, l2mc_idx));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_delete(unit, ing_prof_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_delete(unit, egr_prof_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_delete(unit, untag_prof_idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check the validation of VLAN actions per port.
 *
 * This function is used to check the validation of VLAN actions per port.
 *
 * \param [in] unit              Unit number.
 * \param [inout]port            Port number.
 * \param [in] action            VLAN actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
vlan_port_action_validate(int unit,
                          bcm_port_t *port,
                          bcm_vlan_action_set_t *action)
{
    bcm_port_t port_in;

    SHR_FUNC_ENTER(unit);

    port_in = *port;

    if (!bcmi_ltsw_virtual_vp_is_vfi_type(unit, port_in)) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, port_in, port));

        /* Check forwarding domain for local port. */
        if (action) {
            if (!bcmi_ltsw_virtual_vlan_vpn_valid(unit,
                                                  action->forwarding_domain)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    }

    /* Check VLAN cotrol information. */
    if (action) {
        if ((action->outer_tag != bcmVlanActionNone) &&
            (action->outer_tag != bcmVlanActionDelete)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if ((action->inner_tag != bcmVlanActionNone) &&
            (action->inner_tag != bcmVlanActionDelete)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (action->priority < 0 || action->priority > BCM_PRIO_MAX ||
            action->new_inner_pkt_prio > BCM_PRIO_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (action->new_outer_cfi > 1 || action->new_inner_cfi > 1) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set per VLAN or VPN configuration.
 *
 * This function is used to set per VLAN or VPN configuration.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  vlan            VLAN ID.
 * \param [in]  valid_fields    Valid fields for VLAN control structure,
 *                              BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [in]  control         VLAN control structure.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
xfs_ltsw_vlan_control_vlan_set(int unit,
                               bcm_vlan_t vid,
                               uint32_t valid_fields,
                               bcm_vlan_control_vlan_t *control)
{
    int pre_config_first = 0;

    SHR_FUNC_ENTER(unit);

    /* Check the validation of VLAN control configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_control_validate(unit, vid, &valid_fields, control));

    /* Legacy behavior - Pre-configuration is only available for VLAN usage. */
    if (!BCMI_LTSW_VIRTUAL_VPN_VFI_IS_SET(vid)) {
        if (bcmint_vlan_info_exist_check(unit, vid)) {
            pre_config_first = 0;
        } else if (bcmint_vlan_info_pre_cfg_get(unit, vid)) {
            pre_config_first = 0;
        } else {
            pre_config_first = 1;
        }
    }

    /* Set control information into ingress VLAN table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_entry_control_set(unit, vid,
                                    pre_config_first, valid_fields, control));

    /* Set control information into egress VLAN table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_entry_control_set(unit, vid,
                                    pre_config_first, valid_fields, control));

    if (pre_config_first) {
        bcmint_vlan_info_pre_cfg_set(unit, vid);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get per VLAN or VPN configuration.
 *
 * This function is used to get per VLAN or VPN configuration.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  vlan            VLAN ID.
 * \param [in]  valid_fields    Valid fields for VLAN control structure,
 *                              BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 * \param [out] control         VLAN control structure.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
xfs_ltsw_vlan_control_vlan_get(int unit,
                               bcm_vlan_t vid,
                               uint32_t valid_fields,
                               bcm_vlan_control_vlan_t *control)
{
    SHR_FUNC_ENTER(unit);

    /* Legacy behavior - call sal_memset to reset control information. */
    sal_memset(control, 0, sizeof(bcm_vlan_control_vlan_t));

    /* Check the validation of VLAN control configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_control_validate(unit, vid, &valid_fields, NULL));

    /* Get control information into ingress VLAN table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_entry_control_get(unit, vid, valid_fields, control));

    /* Get control information into egress VLAN table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_entry_control_get(unit, vid, valid_fields, control));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set miscellaneous port-specific VLAN options.
 *
 * This function is used to set miscellaneous port-specific VLAN options.
 *
 * \param [in]  unit             Unit number.
 * \param [in]  port             PORT ID.
 * \param [in]  type             Control port type.
 * \param [in]  arg              Arg vlaue which is dependent on control type.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_control_port_set(int unit,
                               bcm_port_t port,
                               bcm_vlan_control_port_t type,
                               int arg)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port_out;

    SHR_FUNC_ENTER(unit);

    if (port == -1) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));

        BCM_PBMP_ITER(pbmp, port_out) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_control_port_set(unit, port_out, type, arg));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_control_port_set(unit, port, type, arg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get miscellaneous port-specific VLAN options.
 *
 * This function is used to get miscellaneous port-specific VLAN options.
 *
 * \param [in]  unit             Unit number.
 * \param [in]  port             PORT ID.
 * \param [in]  type             Control port type.
 * \param [out] arg              Arg vlaue which is dependent on control type.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_control_port_get(int unit,
                               bcm_port_t port,
                               bcm_vlan_control_port_t type,
                               int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_control_port_get(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate and configure a VLAN.
 *
 * This function is used to allocate and configure a VLAN.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_create(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_create(unit, vid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a VLAN.
 *
 * This function is used to destroy a VLAN.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vid             VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_destroy(int unit, bcm_vlan_t vid)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_destroy(unit, vid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry to ingress VLAN translation table.
 *
 * This function is used to add an entry to ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_add(int unit,
                            bcm_vlan_translate_key_t key_type,
                            bcm_gport_t port,
                            bcm_vlan_t outer_vlan,
                            bcm_vlan_t inner_vlan,
                            bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    /* Check the validation of ingress translate configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_cfg_validate(unit, key_type, port,
                                     outer_vlan, inner_vlan, cfg));

    /* Add ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_add(unit, key_type, port,
                                  outer_vlan, inner_vlan, cfg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update an entry in ingress VLAN translation table.
 *
 * This function is used to update an entry in ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] cfg               Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_update(int unit,
                               bcm_vlan_translate_key_t key_type,
                               bcm_gport_t port,
                               bcm_vlan_t outer_vlan,
                               bcm_vlan_t inner_vlan,
                               bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    /* Check the validation of ingress translate configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_cfg_validate(unit, key_type, port,
                                     outer_vlan, inner_vlan, cfg));
    /* Update ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_update(unit, key_type, port,
                                     outer_vlan, inner_vlan, cfg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from ingress VLAN translation table.
 *
 * This function is used to get an entry from ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] cfg              Ingress xlate configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_get(int unit,
                            bcm_vlan_translate_key_t key_type,
                            bcm_gport_t port,
                            bcm_vlan_t outer_vlan,
                            bcm_vlan_t inner_vlan,
                            bcmi_ltsw_vlan_ing_xlate_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    /* Check the validation of ingress translate configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_cfg_validate(unit, key_type, port,
                                     outer_vlan, inner_vlan, NULL));

    /* Get ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_get(unit, key_type, port,
                                  outer_vlan, inner_vlan, cfg, 1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from ingress VLAN translation table.
 *
 * This function is used to delete an entry from ingress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_delete(int unit,
                               bcm_vlan_translate_key_t key_type,
                               bcm_gport_t port,
                               bcm_vlan_t outer_vlan,
                               bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    /* Check the validation of ingress translate configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_cfg_validate(unit, key_type, port,
                                     outer_vlan, inner_vlan, NULL));

    /* Delete ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_delete(unit, key_type,
                                     port, outer_vlan, inner_vlan));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse over all entries in ingress translation table.
 *
 * This function is used to traverse over all ingress translation entries and
 * call given callback with action structure.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Call back function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_traverse(int unit,
                                 bcmi_ltsw_vlan_ing_xlate_traverse_cb cb,
                                 void *user_data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an action entry to ingress VLAN translation table.
 *
 * This function is used to add an action entry to ingress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] action            VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_action_add(int unit,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_gport_t port,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action)
{
    bcmi_ltsw_vlan_ing_xlate_cfg_t cfg;
    bcmi_ltsw_vlan_ing_xlate_cfg_t old_cfg;
    uint32_t vfi = 0;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&cfg,     0, sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));
    sal_memset(&old_cfg, 0, sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));

    /* Set tag actions. */
    cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS;
    cfg.otag_action = action->outer_tag;
    cfg.itag_action = action->inner_tag;

    /* Check the validation of ingress translate configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_cfg_validate(unit, key_type, port,
                                     outer_vlan, inner_vlan, &cfg));
    /*  Set l3_iif when it's valid. */
    if (SHR_SUCCESS
            (bcmi_ltsw_l3_ing_intf_id_validate(unit, action->ingress_if))) {
        cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF;
        cfg.l3_iif = action->ingress_if;
    } else {
        /* Update l3_iif strength when it's invalid. */
        cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF;
    }

    /* Set vfi when it's valid. */
    if (bcmi_ltsw_virtual_vlan_vpn_valid(unit, action->forwarding_domain)){
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_vfi_idx_get(unit,
                                           action->forwarding_domain, &vfi));
        cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_VFI;
        cfg.vfi = (int)vfi;
    } else {
        /* Update vfi strength when it's invalid. */
        cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_VFI;
    }

    /* Set opaque ctrl id. */
    cfg.fld_bmp |= (BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID |
                    BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID_1);
    if (action->flags & BCM_VLAN_ACTION_SET_DROP) {
        cfg.opaque_ctrl_id |= 0x1 << OpaqueCtrlDrop;
    }
    if (action->flags & BCM_VLAN_ACTION_SET_COPY_TO_CPU) {
        cfg.opaque_ctrl_id |= 0x1 << OpaqueCtrlCopyToCpu;
    }

    /* Set opaque_ctrl_id when it's valid. */
    if (action->opaque_ctrl_id){
        if (action->opaque_ctrl_id < 0 ||
            action->opaque_ctrl_id > BCMI_LTSW_VLAN_XLATE_OPAQUE_MAX) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        BCMI_LTSW_VLAN_XLATE_OPAQUE_SW2HW(action->opaque_ctrl_id,
                                          cfg.opaque_ctrl_id,
                                          cfg.opaque_ctrl_id_1);
    }

    /* Set field flags. */
    old_cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP;

    /* Get ingress xlate entry. */
    rv = vlan_ing_xlate_entry_get(unit, key_type, port,
                                  outer_vlan, inner_vlan, &old_cfg, 1);
    if (SHR_SUCCESS(rv)) {
        if (old_cfg.svp) {
            cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_SVP;
            cfg.svp      = old_cfg.svp;
        }
    }

    /* Set ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_set(unit, key_type, port,
                                  outer_vlan, inner_vlan, &cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an action entry from ingress VLAN translation table.
 *
 * This function is used to get an action entry from ingress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] action           VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_action_get(int unit,
                                   bcm_vlan_translate_key_t key_type,
                                   bcm_gport_t port,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action)
{
    bcmi_ltsw_vlan_ing_xlate_cfg_t cfg;

    SHR_FUNC_ENTER(unit);

    /* Check the validation of ingress translate configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_cfg_validate(unit, key_type, port,
                                     outer_vlan, inner_vlan, NULL));

    sal_memset(&cfg, 0, sizeof(bcmi_ltsw_vlan_ing_xlate_cfg_t));

    /* Set field flags. */
    cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_VFI;
    cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_L3_IIF;
    cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_TAG_ACTIONS;
    cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID;
    cfg.fld_bmp |= BCMI_LTSW_VLAN_ING_XLATE_FLD_OPQ_CTRL_ID_1;

    /* Get ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_get(unit, key_type, port,
                                  outer_vlan, inner_vlan, &cfg, 1));

    /* Get action fields. */
    bcm_vlan_action_set_t_init(action);
    action->outer_tag = cfg.otag_action;
    action->inner_tag = cfg.itag_action;
    action->ingress_if = cfg.l3_iif;
    BCMI_LTSW_VLAN_XLATE_OPAQUE_HW2SW(action->opaque_ctrl_id,
                                      cfg.opaque_ctrl_id,
                                      cfg.opaque_ctrl_id_1);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vpnid_get(unit, cfg.vfi,
                                     &(action->forwarding_domain)));

    if (cfg.opaque_ctrl_id & 0x1 << OpaqueCtrlDrop) {
        action->flags |= BCM_VLAN_ACTION_SET_DROP;
    }
    if (cfg.opaque_ctrl_id & 0x1 << OpaqueCtrlCopyToCpu) {
        action->flags |= BCM_VLAN_ACTION_SET_COPY_TO_CPU;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an action entry from ingress VLAN translation table.
 *
 * This function is used to delete an action entry from ingress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          Key Type - bcmVlanTranslateKey*.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_action_delete(int unit,
                                      bcm_vlan_translate_key_t key_type,
                                      bcm_gport_t port,
                                      bcm_vlan_t outer_vlan,
                                      bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    /* Check the validation of ingress translate configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_cfg_validate(unit, key_type, port,
                                     outer_vlan, inner_vlan, NULL));

    /* Delete ingress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_delete(unit, key_type,
                                     port, outer_vlan, inner_vlan));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse over all entries in ingress translation table.
 *
 * This function is used to traverse over all ingress translation entries and
 * call given callback with action structure.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Call back function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_action_traverse(
    int unit,
    bcm_vlan_translate_action_traverse_cb cb,
    void *user_data)
{
    ltsw_vlan_ing_xlate_action_traverse_data_t trvs_data;

    SHR_FUNC_ENTER(unit);

    sal_memset(&trvs_data, 0,
               sizeof(ltsw_vlan_ing_xlate_action_traverse_data_t));

    trvs_data.cb = cb;
    trvs_data.user_data = user_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_entry_traverse(unit,
                                       vlan_ing_xlate_action_traverse_cb,
                                       (void *)&trvs_data));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an action entry to egress VLAN translation table.
 *
 * This function is used to add an action entry to egress VLAN translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] action            VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_egr_xlate_action_add(int unit,
                                   bcm_gport_t port,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action)
{
    bcmi_ltsw_qos_map_type_t type;
    ltsw_vlan_egr_xlate_cfg_t cfg;

    SHR_FUNC_ENTER(unit);

    sal_memset(&cfg, 0, sizeof(ltsw_vlan_egr_xlate_cfg_t));

    /* Set qos map id directly. */
    cfg.fld_bmp |= EVT_FLD_DOT1P_MAP_ID;
    if (action->flags & BCM_VLAN_ACTION_SET_OUTER_QOS_MAP_ID ||
        action->flags & BCM_VLAN_ACTION_SET_INNER_QOS_MAP_ID) {

        /* inner_qos_map_id and outer_qos_map_id should set the same. */
        if ((action->flags & BCM_VLAN_ACTION_SET_OUTER_QOS_MAP_ID) &&
            (action->flags & BCM_VLAN_ACTION_SET_INNER_QOS_MAP_ID) &&
            (action->outer_qos_map_id != -1) &&
            (action->inner_qos_map_id == action->outer_qos_map_id)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_qos_map_id_resolve(unit, action->outer_qos_map_id,
                                              &type, &(cfg.dot1p_map_id)));
        } else {
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else if (action->priority == -1) {
        /* Legacy behavior - the default dot1p mapping table is used. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_qos_default_ptr_get(unit, bcmiQosMapTypeL2Egress,
                                           &(cfg.dot1p_map_id)));
    } else {
         /* Set priority and cfi. */
         cfg.outer_pri = action->priority;
         cfg.outer_cfi = action->new_outer_cfi;
         cfg.inner_pri = action->new_inner_pkt_prio;
         cfg.inner_cfi = action->new_inner_cfi;
         /* Reset dot1p map ID. */
         SHR_IF_ERR_VERBOSE_EXIT
             (bcmi_ltsw_qos_dummy_ptr_get(unit, bcmiQosMapTypeL2Egress,
                                          &(cfg.dot1p_map_id)));
    }

    /* Set VLAN ID. */
    cfg.fld_bmp |= EVT_FLD_OTAG;
    cfg.outer_vlan = action->new_outer_vlan;
    cfg.fld_bmp |= EVT_FLD_ITAG;
    cfg.inner_vlan = action->new_inner_vlan;

    /* Set tag actions. */
    cfg.fld_bmp |= EVT_FLD_OTAG_ACTION;
    cfg.otag_action = action->outer_tag;
    cfg.fld_bmp |= EVT_FLD_ITAG_ACTION;
    cfg.itag_action = action->inner_tag;

    /* Set opaque ctrl id. */
    cfg.fld_bmp |= EVT_FLD_OPAQUE_CTRL;
    if (action->flags & BCM_VLAN_ACTION_SET_DROP) {
        cfg.opaque_ctrl |= 0x1 << OpaqueCtrlDrop;
    }

    if (action->flags & BCM_VLAN_ACTION_SET_HIGIG3_EXTENSION_ADD) {
        cfg.opaque_ctrl |= 0x1 << OpaqueCtrlHgExtension;
    }

    /* Only drop, qos_map and hgs_extension are supported in egr. */
    if (action->flags & (~(BCM_VLAN_ACTION_SET_DROP |
                           BCM_VLAN_ACTION_SET_OUTER_QOS_MAP_ID |
                           BCM_VLAN_ACTION_SET_INNER_QOS_MAP_ID |
                           BCM_VLAN_ACTION_SET_HIGIG3_EXTENSION_ADD))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check the validation of egress translation configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_cfg_validate(unit, port, outer_vlan, inner_vlan, &cfg));

    /* Set egress vlan action entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_set(unit, port, outer_vlan, inner_vlan, &cfg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an action entry from egress VLAN translation table.
 *
 * This function is used to get an action entry from egress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [out] action           VLAN Actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_egr_xlate_action_get(int unit,
                                   bcm_gport_t port,
                                   bcm_vlan_t outer_vlan,
                                   bcm_vlan_t inner_vlan,
                                   bcm_vlan_action_set_t *action)
{
    ltsw_vlan_egr_xlate_cfg_t cfg;
    int map_id = 0, dummy_id = 0;

    SHR_FUNC_ENTER(unit);

    /* Check the validation of egress translation configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_cfg_validate(unit, port, outer_vlan, inner_vlan, NULL));

    sal_memset(&cfg, 0, sizeof(ltsw_vlan_egr_xlate_cfg_t));

    /* Set field flags. */
    cfg.fld_bmp = EVT_FLD_OTAG |
                  EVT_FLD_ITAG |
                  EVT_FLD_OTAG_ACTION |
                  EVT_FLD_ITAG_ACTION |
                  EVT_FLD_DOT1P_MAP_ID |
                  EVT_FLD_OPAQUE_CTRL;

    /* Get egress vlan action entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_get(unit, port, outer_vlan, inner_vlan, &cfg, 1));

    bcm_vlan_action_set_t_init(action);

    /* Get priority and cfi. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_qos_default_ptr_get(unit, bcmiQosMapTypeL2Egress, &map_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_qos_dummy_ptr_get(unit, bcmiQosMapTypeL2Egress, &dummy_id));
    if (map_id == cfg.dot1p_map_id) {
        action->priority = -1;
    } else if (dummy_id == cfg.dot1p_map_id) {
        action->priority = cfg.outer_pri;
        action->new_outer_cfi = cfg.outer_cfi;
        action->new_inner_pkt_prio = cfg.inner_pri;
        action->new_inner_cfi = cfg.inner_cfi;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_qos_map_id_construct(unit, cfg.dot1p_map_id,
                                            bcmiQosMapTypeL2Egress,
                                            &(action->outer_qos_map_id)));
        action->flags |= BCM_VLAN_ACTION_SET_OUTER_QOS_MAP_ID;
        action->inner_qos_map_id = action->outer_qos_map_id;
        action->flags |= BCM_VLAN_ACTION_SET_INNER_QOS_MAP_ID;
    }

    /* Get VLAN ID. */
    action->new_outer_vlan = cfg.outer_vlan;
    action->new_inner_vlan = cfg.inner_vlan;

    /* Get tag actions. */
    action->outer_tag = cfg.otag_action;
    action->inner_tag = cfg.itag_action;

    /* Get opaque ctrl id. */
    if (cfg.opaque_ctrl & 0x1 << OpaqueCtrlDrop) {
        action->flags |= BCM_VLAN_ACTION_SET_DROP;
    }

    if (cfg.opaque_ctrl & 0x1 << OpaqueCtrlHgExtension) {
        action->flags |= BCM_VLAN_ACTION_SET_HIGIG3_EXTENSION_ADD;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an action entry from egress VLAN translation table.
 *
 * This function is used to delete an action entry from egress VLAN translation
 * table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_egr_xlate_action_delete(int unit,
                                      bcm_gport_t port,
                                      bcm_vlan_t outer_vlan,
                                      bcm_vlan_t inner_vlan)
{
    SHR_FUNC_ENTER(unit);

    /* Check the validation of egress translate configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_cfg_validate(unit, port, outer_vlan, inner_vlan, NULL));

    /* Delete egress xlate entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_delete(unit, port, outer_vlan, inner_vlan));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse over all entries in egress translation table.
 *
 * This function is used to traverse over all egress translation entries and
 * call given callback with action structure.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Call back function.
 * \param [in] user_data         User data to a call back.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_egr_xlate_action_traverse(
    int unit,
    bcm_vlan_translate_egress_action_traverse_cb cb,
    void *user_data)
{
    ltsw_vlan_egr_xlate_action_traverse_data_t trvs_data;

    SHR_FUNC_ENTER(unit);

    sal_memset(&trvs_data, 0,
               sizeof(ltsw_vlan_egr_xlate_action_traverse_data_t));

    trvs_data.cb = cb;
    trvs_data.user_data = user_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_entry_traverse(unit,
                                       vlan_egr_xlate_action_traverse_cb,
                                       (void *)&trvs_data));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set VLAN actions per port.
 *
 * This function is used to set VLAN actions per port.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 * \param [in] action            VLAN actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_port_action_set(int unit,
                              bcm_port_t port,
                              bcm_vlan_action_set_t *action)
{
    bcm_port_t  port_out;
    SHR_FUNC_ENTER(unit);
    port_out = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_port_action_validate(unit, &port_out, action));

    if (bcmi_ltsw_virtual_vp_is_vfi_type(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_port_vlan_action_set(unit, port_out, action));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_ing_vlan_action_set(unit, port_out, action, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get VLAN actions per port.
 *
 * This function is used to get VLAN actions per port.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 * \param [out]action            VLAN actions.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_port_action_get(int unit,
                              bcm_port_t port,
                              bcm_vlan_action_set_t *action)
{
    bcm_port_t  port_out;
    SHR_FUNC_ENTER(unit);
    port_out = port;

    bcm_vlan_action_set_t_init(action);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_port_action_validate(unit, &port_out, NULL));

    if (bcmi_ltsw_virtual_vp_is_vfi_type(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_port_vlan_action_get(unit, port_out, action));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_ing_vlan_action_get(unit, port_out, action, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Reset VLAN actions per port.
 *
 * This function is used to reset VLAN actions per port, and default VLAN
 * actions will be used.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_port_action_reset(int unit, bcm_port_t port)
{
    bcm_port_t  port_out;
    SHR_FUNC_ENTER(unit);
    port_out = port;

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_port_action_validate(unit, &port_out, NULL));

    if (bcmi_ltsw_virtual_vp_is_vfi_type(unit, port)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_virtual_port_vlan_action_reset(unit, port_out));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_ing_vlan_action_reset(unit, port_out));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set force VLAN port type into VLAN table.
 *
 * This function is used to set force VLAN port type into VLAN table.
 *
 * \param [in] unit              Unit number.
 * \param [in] vlan              VLAN ID.
 * \param [in] port_type         Force VLAN port type -
 *                               BCM_PORT_FORCE_VLAN_*_PORT.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_force_port_set(int unit, int vlan, int port_type)
{
    ltsw_vlan_ing_entry_t entry;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    /* Add VLAN ID. */
    entry.vfi = vlan;

    entry.fld_bmp |= IV_FLD_PVLAN_PORT_TYPE;
    if (port_type == BCM_PORT_FORCE_VLAN_PROMISCUOUS_PORT) {
        entry.pvlan_port_type = PVLAN_PORT_TYPE_PROMISCUOUS;
    } else if (port_type ==  BCM_PORT_FORCE_VLAN_COMMUNITY_PORT) {
        entry.pvlan_port_type = PVLAN_PORT_TYPE_COMMUNITY;
    } else if (port_type == BCM_PORT_FORCE_VLAN_ISOLATED_PORT) {
        entry.pvlan_port_type = PVLAN_PORT_TYPE_ISOLATED;
    } else {
        SHR_EXIT();
    }

    /* Update vlan entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_UPDATE, &entry));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the state of VLAN membership check on a given port.
 *
 * This function is used to set the state of VLAN membership check on a given
 * port.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port number.
 * \param [in] disable           Disable or enable.
 * \param [in] flags             Direction flags.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_port_check_set(int unit, bcm_port_t port, int disable, uint16_t flags)
{
    ltsw_vlan_info_t *vlan_info = &ltsw_vlan_info[unit];
    uint64_t idx_min = 0;
    uint64_t idx_max = 0;
    uint32_t ref_count = 0;
    uint32_t port_num = 0;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    VLAN_PROFILE_LOCK(unit);

    if (!VLAN_SW_MSHIP_EN(unit)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
    if (port >= port_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, VLAN_ING_MEMBER_PROFILEs,
                                       VLAN_ING_MEMBER_PROFILE_IDs,
                                       &idx_min, &idx_max));

    for (i = (int)idx_min; i <= (int)idx_max; i++) {

        if ((flags & BCMI_LTSW_VLAN_CHK_ING) ||
            (flags & BCMI_LTSW_VLAN_CHK_ING_EGR)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_get(unit,
                                                 BCMI_LTSW_PROFILE_ING_MSHP_CHK,
                                                 i, &ref_count));
            if (ref_count != 0) {
                if (flags & BCMI_LTSW_VLAN_CHK_ING) {
                    SHR_IF_ERR_VERBOSE_EXIT
                          (vlan_ing_member_profile_entry_port_update(
                              unit, i, port, disable));
                }
                if (flags & BCMI_LTSW_VLAN_CHK_ING_EGR) {
                    SHR_IF_ERR_VERBOSE_EXIT
                          (vlan_ing_egr_member_profile_entry_port_update(
                               unit, i, port, disable));
                }
            }
        }

        if (flags & BCMI_LTSW_VLAN_CHK_EGR) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_get(unit,
                                                 BCMI_LTSW_PROFILE_EGR_MSHP_CHK,
                                                 i, &ref_count));
            if (ref_count != 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_egr_member_profile_entry_port_update(
                        unit, i, port, disable));
            }
        }
    }

    if (flags & BCMI_LTSW_VLAN_CHK_ING) {
        if (disable) {
            vlan_info->port_dis_chk[port] |= VLAN_PORT_DIS_ING_CHK;
        } else {
            vlan_info->port_dis_chk[port] &= ~VLAN_PORT_DIS_ING_CHK;
        }
    }
    if (flags & BCMI_LTSW_VLAN_CHK_ING_EGR) {
        if (disable) {
            vlan_info->port_dis_chk[port] |= VLAN_PORT_DIS_ING_EGR_CHK;
        } else {
            vlan_info->port_dis_chk[port] &= ~VLAN_PORT_DIS_ING_EGR_CHK;
        }
    }
    if (flags & BCMI_LTSW_VLAN_CHK_EGR) {
        if (disable) {
            vlan_info->port_dis_chk[port] |= VLAN_PORT_DIS_EGR_CHK;
        } else {
            vlan_info->port_dis_chk[port] &= ~VLAN_PORT_DIS_EGR_CHK;
        }
    }

exit:
    VLAN_PROFILE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get force VLAN port type from VLAN table.
 *
 * This function is used to get force VLAN port type from VLAN table.
 *
 * \param [in] unit              Unit number.
 * \param [in] vlan              VLAN ID.
 * \param [out]port_type         Force VLAN port type -
 *                               BCM_PORT_FORCE_VLAN_*_PORT.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_force_port_get(int unit, int vlan, int *port_type)
{
    ltsw_vlan_ing_entry_t entry;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    /* Add VLAN ID. */
    entry.vfi = vlan;
    entry.fld_bmp |= IV_FLD_PVLAN_PORT_TYPE;

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    if (PVLAN_PORT_TYPE_PROMISCUOUS == entry.pvlan_port_type) {
        *port_type = BCM_PORT_FORCE_VLAN_PROMISCUOUS_PORT;
    } else if (PVLAN_PORT_TYPE_COMMUNITY == entry.pvlan_port_type) {
        *port_type = BCM_PORT_FORCE_VLAN_COMMUNITY_PORT;
    } else if (PVLAN_PORT_TYPE_ISOLATED == entry.pvlan_port_type) {
        *port_type = BCM_PORT_FORCE_VLAN_ISOLATED_PORT;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given vlan.
 *
 * This function is used to attach flex counter to the given vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_stat_attach(int unit, bcm_vlan_t vid,
                          bcmi_ltsw_flexctr_counter_info_t *info)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *key_field = NULL, *data_field = NULL;
    uint32_t vfi = 0;
    uint64_t flex_ctr_action;
    int dunit = 0;
    const bcmint_vlan_lt_t *lt_info_ing, *lt_info_egr;
    const bcmint_vlan_fld_t *flds;

    SHR_FUNC_ENTER(unit);

    /* Get VFI index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, vid, &vfi));
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_ING_VFI_TABLE, &lt_info_ing));
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_EGR_VFI, &lt_info_egr));

    if (!sal_strcmp(info->table_name, lt_info_ing->name)) {
        flds = lt_info_ing->flds;
        key_field = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI].name;
        data_field = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_FLEX_CTR_ACTION].name;
    } else if (!sal_strcmp(info->table_name, lt_info_egr->name)) {
        flds = lt_info_egr->flds;
        key_field = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI].name;
        data_field = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_FLEX_CTR_ACTION].name;;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Allocate entry container. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, info->table_name, &ent_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, key_field, vfi));

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Get current flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, data_field, &flex_ctr_action));

    /* Already attached a flex counter action. */
    if (flex_ctr_action != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    /* Clear entry content. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(ent_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, key_field, vfi));

    flex_ctr_action = (uint64_t)info->action_index;
    /* Attach flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, data_field, flex_ctr_action));

    /* Update VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries to the given vlan.
 *
 * This function is used to detach flex counter to the given vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_stat_detach(int unit, bcm_vlan_t vid,
                          bcmi_ltsw_flexctr_counter_info_t *info)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    const char *key_field = NULL, *data_field = NULL;
    uint32_t vfi = 0;
    uint64_t flex_ctr_action;
    int dunit = 0;
    const bcmint_vlan_lt_t *lt_info_ing, *lt_info_egr;
    const bcmint_vlan_fld_t *flds;

    SHR_FUNC_ENTER(unit);

    /* Get VFI index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, vid, &vfi));
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_ING_VFI_TABLE, &lt_info_ing));
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_EGR_VFI, &lt_info_egr));

    if (!sal_strcmp(info->table_name, lt_info_ing->name)) {
        flds = lt_info_ing->flds;
        key_field = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_VFI].name;
        data_field = flds[BCMINT_LTSW_VLAN_FLD_ING_VFI_FLEX_CTR_ACTION].name;
    } else if (!sal_strcmp(info->table_name, lt_info_egr->name)) {
        flds = lt_info_egr->flds;
        key_field = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_VFI].name;
        data_field = flds[BCMINT_LTSW_VLAN_FLD_EGR_VFI_FLEX_CTR_ACTION].name;;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Allocate entry container. */
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, info->table_name, &ent_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, key_field, vfi));

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Get current flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(ent_hdl, data_field, &flex_ctr_action));

    /* Current flex counter action is not the detached one. */
    if (flex_ctr_action != (uint64_t)info->action_index) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Clear entry content. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(ent_hdl));

    /* Add VLAN ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, key_field, vfi));

    /* Detach flex counter action. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(ent_hdl, data_field,
                               BCMI_LTSW_FLEXCTR_ACTION_INVALID));

    /* Update VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, ent_hdl,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (ent_hdl != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get counter entries of the given vlan.
 *
 * This function is used to get flex counter to the given vlan.
 *
 * \param [in] unit              Unit number.
 * \param [in] vid               VLAN ID.
 * \param [out] info             Counter information.
 * \param [out] num_info         Number of counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_stat_get(int unit,
                       bcm_vlan_t vid,
                       bcmi_ltsw_flexctr_counter_info_t *info,
                       uint32_t *num_info)
{
    uint32_t vfi = 0;
    ltsw_vlan_ing_entry_t ing_entry;
    ltsw_vlan_egr_entry_t egr_entry;
    const bcmint_vlan_lt_t *lt_info;

    SHR_FUNC_ENTER(unit);
    sal_memset(&ing_entry, 0, sizeof(ltsw_vlan_ing_entry_t));
    sal_memset(&egr_entry, 0, sizeof(ltsw_vlan_egr_entry_t));

    /* Get VFI index. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_virtual_vfi_idx_get(unit, vid, &vfi));
    ing_entry.vfi = vfi;
    ing_entry.fld_bmp |= IV_FLD_FLEX_CTR_ACTION;
    egr_entry.vfi = vfi;
    egr_entry.fld_bmp |= EV_FLD_FLEX_CTR_ACTION;

    *num_info = 0;

    /* Find VLAN entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_LOOKUP, &ing_entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_table_op(unit, BCMLT_OPCODE_LOOKUP, &egr_entry));

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_ING_VFI_TABLE, &lt_info));
    info[*num_info].direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
    info[*num_info].source = bcmFlexctrSourceIngVlan;
    info[*num_info].table_name = lt_info->name;
    info[*num_info].action_index = (uint32_t)ing_entry.flex_ctr_action;
    info[*num_info].stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    (*num_info)++;

    SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_EGR_VFI, &lt_info));
    info[*num_info].direction = BCMI_LTSW_FLEXCTR_DIR_EGRESS;
    info[*num_info].source = bcmFlexctrSourceEgrVlan;
    info[*num_info].table_name = lt_info->name;
    info[*num_info].action_index = (uint32_t)egr_entry.flex_ctr_action;
    info[*num_info].stage = BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR;
    (*num_info)++;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a Gport to the specified vlan. Adds WLAN/VLAN/NIV/Extender ports to
 *        broadcast, multicast and unknown unicast groups.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   port            Gport id.
 * \param [in]   flags           BCM_VLAN_PORT_XXX.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_gport_add(int unit, bcm_vlan_t vlan, bcm_gport_t port, int flags)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp, ubmp, ing_pbmp;
    uint32_t vfi = 0, supported_flags = 0;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vlan, &vfi));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_gport_resolve(unit, port, &local_port));
    /* Skip internal loopback port.*/
    if (bcmi_ltsw_port_is_type(unit, local_port, BCMI_LTSW_PORT_TYPE_LB)) {
        SHR_EXIT();
    }

    supported_flags = BCM_VLAN_GPORT_ADD_UNTAGGED |
                      BCM_VLAN_GPORT_ADD_INGRESS_ONLY |
                      BCM_VLAN_GPORT_ADD_EGRESS_ONLY |
                      BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
    if ((~supported_flags) & flags) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if ((flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY) &&
        (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (flags & BCM_VLAN_GPORT_ADD_INGRESS_ONLY) {
        if (flags & BCM_VLAN_GPORT_ADD_UNTAGGED) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if ((flags & BCM_VLAN_GPORT_ADD_EGRESS_ONLY) ||
            (flags & BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (flags & (BCM_VLAN_PORT_EGRESS_ONLY |
                 BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        BCM_PBMP_CLEAR(ing_pbmp);
    } else {
        BCM_PBMP_PORT_SET(ing_pbmp, local_port);
    }
    if (flags & BCM_VLAN_PORT_INGRESS_ONLY) {
        BCM_PBMP_CLEAR(pbmp);
    } else {
        BCM_PBMP_PORT_SET(pbmp, local_port);
    }
    if (flags & BCM_VLAN_PORT_UNTAGGED) {
        /* No such thing as untagged CPU. */
        if (local_port != bcmi_ltsw_port_cpu(unit)){
            BCM_PBMP_PORT_SET(ubmp, local_port);
        }
    } else {
        BCM_PBMP_CLEAR(ubmp);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_vlan_port_add(unit, vfi, ing_pbmp, pbmp, ubmp, flags));
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Delete all Gports from the specified vlan.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_gport_delete_all(int unit, bcm_vlan_t vlan)
{
    bcm_pbmp_t pbmp;
    uint32_t vfi = 0;
    int ing_prof_idx, egr_prof_idx, untag_prof_idx, l2mc_idx;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vlan, &vfi));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vfi, NULL, NULL,NULL, NULL,
                             &ing_prof_idx, &egr_prof_idx, &untag_prof_idx,
                             &l2mc_idx));
    BCM_PBMP_CLEAR(pbmp);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_set(unit, vfi, pbmp, pbmp, pbmp, pbmp));

    /* Delete old profile. */
    if (VLAN_DLF_FLOOD(unit) && (vfi <= BCM_VLAN_MAX)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_multicast_vlan_flood_grp_destroy(unit, l2mc_idx));
   }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_mshp_chk_profile_delete(unit, ing_prof_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_mshp_chk_profile_delete(unit, egr_prof_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_untag_profile_delete(unit, untag_prof_idx));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a Gport from the specified vlan with flag.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   port            Gport id.
 * \param [out]  flags           BCM_VLAN_PORT_XXX.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_gport_delete(int unit, bcm_vlan_t vlan,
                           bcm_gport_t port, int flags)
{
    bcm_pbmp_t ing_pbmp, pbmp, ubmp;
    bcm_port_t local_port;
    uint32_t vfi, supported_flags = 0;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vlan, &vfi));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_gport_resolve(unit, port, &local_port));
    supported_flags = BCM_VLAN_GPORT_ADD_UNTAGGED |
                      BCM_VLAN_GPORT_ADD_INGRESS_ONLY |
                      BCM_VLAN_GPORT_ADD_EGRESS_ONLY |
                      BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
    if (flags & ~supported_flags) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (flags &
        (BCM_VLAN_PORT_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY)) {
        BCM_PBMP_CLEAR(ing_pbmp);
    } else {
        BCM_PBMP_PORT_SET(ing_pbmp, local_port);
    }
    if (flags & BCM_VLAN_PORT_INGRESS_ONLY) {
        BCM_PBMP_CLEAR(pbmp);
    } else {
        BCM_PBMP_PORT_SET(pbmp, local_port);
    }
    if (flags & BCM_VLAN_PORT_UNTAGGED) {
        BCM_PBMP_PORT_SET(ubmp, local_port);
    } else {
        BCM_PBMP_CLEAR(ubmp);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_vlan_port_remove(unit, vfi, ing_pbmp, pbmp, ubmp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a virtual or physical port from the specified VLAN.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   port            Gport id.
 * \param [out]  flags           BCM_VLAN_PORT_XXX.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_gport_get(int unit, bcm_vlan_t vlan,
                        bcm_gport_t port, int *flags)
{
    bcm_pbmp_t pbmp, ubmp, ing_pbmp, ing_egr_pbmp;
    bcm_port_t local_port;
    uint32_t local_flags = 0, vfi = 0;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vlan, &vfi));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_gport_resolve(unit, port, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vfi, &ing_pbmp, &ing_egr_pbmp,
                             &pbmp, &ubmp, NULL, NULL, NULL, NULL));
    BCM_PBMP_OR(pbmp, ing_egr_pbmp);
    if (!BCM_PBMP_MEMBER(pbmp, local_port)) {
        local_flags |= BCM_VLAN_PORT_INGRESS_ONLY;
    }
    if (!BCM_PBMP_MEMBER(ing_pbmp, local_port)) {
        local_flags |= BCM_VLAN_PORT_EGRESS_ONLY;
    }
    if (local_flags ==
        (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
        SHR_ERR_EXIT(BCM_E_NOT_FOUND);
    }
    if (BCM_PBMP_MEMBER(ubmp, local_port)) {
        local_flags |= BCM_VLAN_PORT_UNTAGGED;
    }
    if (local_flags & BCM_VLAN_PORT_EGRESS_ONLY) {
        if (!BCM_PBMP_MEMBER(ing_egr_pbmp, local_port)) {
            local_flags &= ~BCM_VLAN_PORT_EGRESS_ONLY;
            local_flags |= BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
        }
    }
    *flags = local_flags;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all virtual and physical ports from the specified VLAN.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   vlan            VLAN ID.
 * \param [in]   array_max       Max array size.
 * \param [out]  gport_array     Gport id array.
 * \param [out]  flags_array     BCM_VLAN_PORT_XXX array.
 * \param [out]  array_size      Output array size.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_gport_get_all(int unit, bcm_vlan_t vlan, int array_max,
                            bcm_gport_t *gport_array, int *flags_array,
                            int* array_size)
{
    bcm_pbmp_t          pbmp, ubmp, ing_pbmp, bmp, ing_egr_pbmp;
    bcm_port_t          port, gport;
    int                 port_cnt = 0;
    uint32_t            local_flags = 0, vfi = 0;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vlan, &vfi));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_entry_port_get(unit, vfi, &ing_pbmp, &ing_egr_pbmp,
                             &pbmp, &ubmp, NULL, NULL, NULL, NULL));
    BCM_PBMP_ASSIGN(bmp, pbmp);
    BCM_PBMP_OR(bmp, ubmp);
    BCM_PBMP_OR(bmp, ing_pbmp);
    BCM_PBMP_OR(pbmp, ing_egr_pbmp);
    BCM_PBMP_ITER(bmp, port) {
        if ((port_cnt == array_max) && (gport_array)) {
            break;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_gport_get(unit, port, &gport));
        local_flags = 0;
        if (!BCM_PBMP_MEMBER(pbmp, port)) {
            local_flags |= BCM_VLAN_PORT_INGRESS_ONLY;
        }
         if (!BCM_PBMP_MEMBER(ing_pbmp, port)) {
            local_flags |= BCM_VLAN_PORT_EGRESS_ONLY;
        }
        if (local_flags ==
            (BCM_VLAN_PORT_INGRESS_ONLY | BCM_VLAN_PORT_EGRESS_ONLY)) {
            continue;
        }
        if (local_flags & BCM_VLAN_PORT_EGRESS_ONLY) {
            if (!BCM_PBMP_MEMBER(ing_egr_pbmp, port)) {
                local_flags &= ~BCM_VLAN_PORT_EGRESS_ONLY;
                local_flags |= BCM_VLAN_GPORT_ADD_EGRESS_L3_ONLY;
            }
        }
        if (flags_array) {
            if (BCM_PBMP_MEMBER(ubmp, port)) {
                local_flags |= BCM_VLAN_PORT_UNTAGGED;
            }
            flags_array[port_cnt] = local_flags;
        }
        if (gport_array) {
            gport_array[port_cnt] = gport;
        }
        port_cnt++;
    }

    if (!port_cnt) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *array_size = port_cnt;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set miscellaneous VLAN-specific chip options.
 *
 * \param [in]   unit     Unit number.
 * \param [in]   type     A value from bcm_vlan_control_t enumeration list.
 * \param [in]   arg      State whose meaning is dependent on 'type'.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
xfs_ltsw_vlan_control_set(int unit, bcm_vlan_control_t type, int arg)
{
    SHR_FUNC_ENTER(unit);
    switch (type) {
        case bcmVlanTranslate:
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_vlan_control_port_set
                     (unit, -1, bcmVlanTranslateIngressEnable, arg));
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_vlan_control_port_set
                     (unit, -1, bcmVlanTranslateEgressEnable, arg));
            break;
        case bcmVlanControlTranslateIngressMissDrop:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_miss_policy_table_op(unit, BCMLT_OPCODE_UPDATE,
                                                 OpaqueCtrlDrop, 1, &arg));
             break;
        case bcmVlanControlTranslateIngressMissCopyToCpu:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_miss_policy_table_op(unit, BCMLT_OPCODE_UPDATE,
                                                 OpaqueCtrlCopyToCpu, 1, &arg));
             break;
        case bcmVlanControlTranslateEgressMissDrop:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_miss_policy_table_op(unit, BCMLT_OPCODE_UPDATE,
                                                 OpaqueCtrlDrop, 0, &arg));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get miscellaneous VLAN-specific chip options.
 *
 * \param [in]    unit     Unit number.
 * \param [in]    type     A value from bcm_vlan_control_t enumeration list.
 * \param [out]   arg      State whose meaning is dependent on 'type'.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
xfs_ltsw_vlan_control_get(int unit, bcm_vlan_control_t type, int *arg)
{
    bcm_port_t port;
    bcm_pbmp_t pbmp;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ETH, &pbmp));
    BCM_PBMP_ITER(pbmp, port) {
        break;
    }

    switch (type) {
        case bcmVlanTranslate:
            SHR_IF_ERR_VERBOSE_EXIT
                (xfs_ltsw_vlan_control_port_get
                    (unit, port, bcmVlanTranslateIngressEnable, arg));
            break;
        case bcmVlanControlTranslateIngressMissDrop:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_miss_policy_table_op(unit, BCMLT_OPCODE_LOOKUP,
                                                 OpaqueCtrlDrop, 1, arg));
            break;
        case bcmVlanControlTranslateIngressMissCopyToCpu:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_miss_policy_table_op(unit, BCMLT_OPCODE_LOOKUP,
                                                 OpaqueCtrlCopyToCpu, 1, arg));
            break;
        case bcmVlanControlTranslateEgressMissDrop:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_miss_policy_table_op(unit, BCMLT_OPCODE_LOOKUP,
                                                 OpaqueCtrlDrop, 0, arg));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update all VLAN information to L2 station.
 *
 * This function is used to update all VLAN information to L2 station, and it is
 * only called by VLAN or L2 STATION during init.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_l2_station_update_all(int unit,
                                    bcmi_ltsw_vlan_l2_station_update_cb cb,
                                    void *user_data)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_l2_station_vlan_t l2_notify_info;
    int l2_notify_flags = 0;
    int rv = SHR_E_NONE;
    bcm_vlan_t vlan = 0;
    int dunit = 0;
    ltsw_vlan_ing_entry_t entry;
    const bcmint_vlan_lt_t *lt_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_ING_VFI_TABLE, &lt_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, ent_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        entry.fld_bmp |= IV_FLD_STG |
                         IV_FLD_MSHIP_PROF_IDX;
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_entry_fields_get(unit, ent_hdl, &entry));

        if (entry.vfi == VLAN_RESV0) {
            continue;
        }

        if (cb) {
            vlan = (bcm_vlan_t)entry.vfi;

            l2_notify_flags |= BCMI_LTSW_L2_STATION_VLAN_F_MEMBER_PRF_PTR;
            l2_notify_info.member_prf_ptr = (uint16_t)entry.mship_prof_idx;

            l2_notify_flags |= BCMI_LTSW_L2_STATION_VLAN_F_STG;
            l2_notify_info.stg = (uint16_t)entry.stg;

            SHR_IF_ERR_VERBOSE_EXIT
                (cb(unit, vlan, l2_notify_flags, &l2_notify_info, user_data));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_l2_station_update(unit, &entry));
        }
    }

    if ((rv != SHR_E_NOT_FOUND) && SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Update all VLAN information to L3 interface.
 *
 * This function is used to update all VLAN information to L3 interface, and it
 * is only called by L3 INTF module during init.
 *
 * \param [in] unit              Unit number.
 * \param [in] cb                Callback function.
 * \param [in] user_data         User data of callback.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_l3_intf_update_all(int unit,
                                 bcmi_ltsw_vlan_l3_intf_update_cb cb,
                                 void *user_data)
{
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;
    bcmi_ltsw_l3_intf_vfi_t l3_notify_info;
    int rv = SHR_E_NONE;
    bcm_vlan_t vlan = 0;
    int dunit = 0;
    ltsw_vlan_egr_entry_t entry;
    const bcmint_vlan_lt_t *lt_info;

    SHR_FUNC_ENTER(unit);
    sal_memset(&entry, 0, sizeof(ltsw_vlan_egr_entry_t));
    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_vlan_lt_get(unit, BCMINT_LTSW_VLAN_EGR_VFI, &lt_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_info->name, &ent_hdl));

    while ((rv = bcmi_lt_entry_commit(unit, ent_hdl,
                                      BCMLT_OPCODE_TRAVERSE,
                                      BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        entry.fld_bmp |= EV_FLD_MSHIP_PROF_IDX |
                         EV_FLD_UNTAG_PROF_IDX |
                         EV_FLD_STG |
                         EV_FLD_O_VLAN |
                         EV_FLD_TAG_ACTION;
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_entry_fields_get(unit, ent_hdl, &entry));

        if (entry.vfi == VLAN_RESV0) {
            continue;
        }

        vlan = (bcm_vlan_t)entry.vfi;

        l3_notify_info.flags = BCMI_LTSW_L3_INTF_VFI_F_MEMBERSHIP_PRFL_IDX;
        l3_notify_info.membership_prfl_idx = (uint16_t)entry.mship_prof_idx;

        l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_UNTAG_PRFL_IDX;
        l3_notify_info.untag_prfl_idx = (uint16_t)entry.untag_prof_idx;

        l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_STG;
        l3_notify_info.stg = (uint16_t)entry.stg;

        l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_TAG;
        l3_notify_info.tag = (uint16_t)entry.o_vlan;

        l3_notify_info.flags |= BCMI_LTSW_L3_INTF_VFI_F_TAG_ACTION;
        l3_notify_info.tag_action = (entry.tag_action & 0x1) ? 0x1: 0x0;

        SHR_IF_ERR_VERBOSE_EXIT
            (cb(unit, vlan, &l3_notify_info, user_data));
    }

    if ((rv != SHR_E_NOT_FOUND) && SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry to the VLAN Translation table, which assigns
 *        VLAN actions for packets matching within the VLAN range(s).
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port             Port Id.
 * \param [in]      outer_vlan_low   Outer VLAN ID Low.
 * \param [in]      outer_vlan_high  Outer VLAN ID High.
 * \param [in]      inner_vlan_low   Inner VLAN ID Low.
 * \param [in]      inner_vlan_high  Inner VLAN ID High.
 * \param [in]      action           Action for outer and inner tag.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_translate_action_range_add(int unit, bcm_gport_t port,
                                         bcm_vlan_t outer_vlan_low,
                                         bcm_vlan_t outer_vlan_high,
                                         bcm_vlan_t inner_vlan_low,
                                         bcm_vlan_t inner_vlan_high,
                                         bcm_vlan_action_set_t *action)
{
    int port_cnt = 1, i;
    bcm_port_t *port_array = NULL, local_port;
    bcm_trunk_t tid;
    bcm_trunk_member_t *member = NULL;
    SHR_FUNC_ENTER(unit);

    /*
     * VLAN action and VLAN compression are decoupled.
     * VLAN action is not configured in this function.
     */
    if (action != NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        tid = BCM_GPORT_TRUNK_GET(port);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_trunk_get(unit, tid, NULL, 0, NULL, &port_cnt));
        if (port_cnt > 0) {
            SHR_ALLOC(port_array, sizeof(bcm_port_t) * port_cnt, "bcmPortArray");
            SHR_NULL_CHECK(port_array, SHR_E_MEMORY);
            sal_memset(port_array, 0, sizeof(bcm_port_t) * port_cnt);

            SHR_ALLOC(member, sizeof(bcm_trunk_member_t) * port_cnt,
                      "bcmTrunkMemberArray");
            SHR_NULL_CHECK(member, SHR_E_MEMORY);
            sal_memset(member, 0, sizeof(bcm_trunk_member_t) * port_cnt);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_trunk_get(unit, tid, NULL, port_cnt, member, &port_cnt));
            for (i = 0; i < port_cnt; i++) {
                port_array[i] = member[i].gport;
            }
        } else {
            SHR_EXIT();
        }
    } else {
        SHR_ALLOC(port_array, sizeof(bcm_port_t), "bcmPortArray");
        SHR_NULL_CHECK(port_array, SHR_E_MEMORY);
        sal_memset(port_array, 0, sizeof(bcm_port_t));

        if (!BCM_GPORT_IS_SET(port)) {
            BCM_GPORT_LOCAL_SET(port_array[0], port);
        } else if (BCM_GPORT_IS_LOCAL(port) || BCM_GPORT_IS_MODPORT(port)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, port, &local_port));
            port_array[0] = port;
        } else {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    }

    VLAN_RANGE_LOCK(unit);
    SHR_IF_ERR_CONT
        (vlan_translate_action_range_add(unit, port_array, port_cnt,
                                         outer_vlan_low, outer_vlan_high,
                                         inner_vlan_low, inner_vlan_high));

    VLAN_RANGE_UNLOCK(unit);
exit:
    SHR_FREE(port_array);
    SHR_FREE(member);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from the VLAN Translation table
 *        for the specified VLAN range(s).
 *
 * \param [in]      unit             Unit number.
 * \param [in]      port             Port Id.
 * \param [in]      outer_vlan_low   Outer VLAN ID Low.
 * \param [in]      outer_vlan_high  Outer VLAN ID High.
 * \param [in]      inner_vlan_low   Inner VLAN ID Low.
 * \param [in]      inner_vlan_high  Inner VLAN ID High.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_translate_action_range_delete(int unit, bcm_gport_t port,
                                           bcm_vlan_t outer_vlan_low,
                                           bcm_vlan_t outer_vlan_high,
                                           bcm_vlan_t inner_vlan_low,
                                           bcm_vlan_t inner_vlan_high)
{
    int port_cnt = 1, i;
    bcm_trunk_t tid;
    bcm_trunk_member_t *member = NULL;
    bcm_gport_t   *port_array = NULL, local_port;
    SHR_FUNC_ENTER(unit);

    if (BCM_GPORT_IS_TRUNK(port)) {
        tid = BCM_GPORT_TRUNK_GET(port);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_trunk_get(unit, tid, NULL, 0, NULL, &port_cnt));
        if (port_cnt > 0) {
            SHR_ALLOC(port_array, sizeof(bcm_port_t) * port_cnt, "bcmPortArray");
            SHR_NULL_CHECK(port_array, SHR_E_MEMORY);
            sal_memset(port_array, 0, sizeof(bcm_port_t) * port_cnt);

            SHR_ALLOC(member, sizeof(bcm_trunk_member_t) * port_cnt,
                      "bcmTrunkMemberArray");
            SHR_NULL_CHECK(member, SHR_E_MEMORY);
            sal_memset(member, 0, sizeof(bcm_trunk_member_t) * port_cnt);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_trunk_get(unit, tid, NULL, port_cnt, member, &port_cnt));
            for (i = 0; i < port_cnt; i++) {
                port_array[i] = member[i].gport;
            }
        } else {
            SHR_EXIT();
        }
    } else {
        SHR_ALLOC(port_array, sizeof(bcm_port_t), "bcmPortArray");
        SHR_NULL_CHECK(port_array, SHR_E_MEMORY);
        sal_memset(port_array, 0, sizeof(bcm_port_t));

        if (!BCM_GPORT_IS_SET(port)) {
            BCM_GPORT_LOCAL_SET(port_array[0], port);
        } else if (BCM_GPORT_IS_LOCAL(port) || BCM_GPORT_IS_MODPORT(port)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_validate(unit, port, &local_port));
            port_array[0] = port;
        } else {
            SHR_ERR_EXIT(SHR_E_PORT);
        }
    }

    VLAN_RANGE_LOCK(unit);
    SHR_IF_ERR_CONT
        (vlan_translate_action_range_delete(unit, port_array, port_cnt,
                                            outer_vlan_low, outer_vlan_high,
                                            inner_vlan_low, inner_vlan_high));
    VLAN_RANGE_UNLOCK(unit);
exit:
    SHR_FREE(member);
    SHR_FREE(port_array);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all VLAN range entries from the VLAN Translation table.
 *
 * \param [in]      unit             Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_translate_action_range_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);
    VLAN_RANGE_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_sys_port_traverse(unit, vlan_range_map_delete_cb, NULL));
exit:
    VLAN_RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverses over VLAN Translation table and call provided callback
 *        with valid entries.
 *
 * \param [in]      unit             Unit number.
 * \param [in]      cb               Call back function.
 * \param [in]      user_data        Pointer to user specific data.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_translate_action_range_traverse(int unit,
    bcm_vlan_translate_action_range_traverse_cb cb, void *user_data)
{
    ltsw_vlan_range_traverse_data_t trvs_data;
    SHR_FUNC_ENTER(unit);

    sal_memset(&trvs_data, 0,
               sizeof(ltsw_vlan_range_traverse_data_t));
    trvs_data.cb = cb;
    trvs_data.user_data = user_data;

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_gport_traverse(unit, BCM_GPORT_TYPE_NONE,
                                      vlan_range_traverse_cb,
                                      (void *)&trvs_data));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_ltsw_port_gport_traverse(unit, BCM_GPORT_TYPE_MODPORT,
                                      vlan_range_traverse_cb,
                                      (void *)&trvs_data));
exit:
   SHR_FUNC_EXIT();

}

/*!
 * \brief Get per VLAN block configuration.
 *
 * \param [in]   unit       Unit number.
 * \param [in]   vid        VLAN ID.
 * \param [out]  block    VLAN specific traffic blocking structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
xfs_ltsw_vlan_block_get(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    uint32_t vfi;
    ltsw_vlan_ing_block_mask_vfi_profile_t prof;
    int port_num;
    ltsw_vlan_ing_entry_t entry;
    ltsw_vlan_block_mask_ctrl_t *mask_ctrl =
        &(ltsw_vlan_info[unit].block_mask_ctrl);

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vid, &vfi));

    entry.vfi = vfi;
    entry.fld_bmp |= IV_FLD_BLOCK_PROF_IDX;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));

    if (entry.block_prof_idx == VLAN_PROFILE_ING_BLOCK_MASK_RESV0) {
        sal_memset(block, 0, sizeof(bcm_vlan_block_t));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_block_mask_vfi_profile_get(unit,
                                                 entry.block_prof_idx,
                                                 &prof, 1));

        /* Negate first after get from HW. */
        port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;
        SHR_BITNEGATE_RANGE(
            (uint32_t *)(&(prof.mask[mask_ctrl->mc_mask_sel].mask_ports)),
            0, port_num, (uint32_t *)(&block->known_multicast));
        SHR_BITNEGATE_RANGE(
            (uint32_t *)(&(prof.mask[mask_ctrl->umc_mask_sel].mask_ports)),
            0, port_num, (uint32_t *)(&block->unknown_multicast));
        SHR_BITNEGATE_RANGE(
            (uint32_t *)(&(prof.mask[mask_ctrl->uuc_mask_sel].mask_ports)),
            0, port_num, (uint32_t *)(&block->unknown_unicast));
        SHR_BITNEGATE_RANGE(
            (uint32_t *)(&(prof.mask[mask_ctrl->bc_mask_sel].mask_ports)),
            0, port_num, (uint32_t *)(&block->broadcast));
        SHR_BITNEGATE_RANGE(
            (uint32_t *)(&(prof.mask[mask_ctrl->uc_mask_sel].mask_ports)),
            0, port_num, (uint32_t *)(&block->unicast));
    }

exit:
    SHR_FUNC_EXIT();
}

 /*!
 * \brief Set per VLAN block configuration.
 *
 * \param [in]   unit      Unit number.
 * \param [in]   vid       VLAN ID.
 * \param [in]   block    VLAN specific traffic blocking structure.
 *
 * \retval SHR_E_NONE     No errors.
 * \retval !SHR_E_NONE    Failure.
 */
int
xfs_ltsw_vlan_block_set(int unit, bcm_vlan_t vid, bcm_vlan_block_t *block)
{
    uint32_t vfi;
    ltsw_vlan_ing_block_mask_vfi_profile_t prof;
    int prof_idx, new_prof_added = 0, port_num = 0;
    int mask_target = LTSW_VLAN_MASK_TARGET_L2_AND_L3;
    int mask_action = LTSW_VLAN_MASK_ACTION_AND;
    ltsw_vlan_block_mask_ctrl_t *mask_ctrl =
        &(ltsw_vlan_info[unit].block_mask_ctrl);
    int i = 0;
    uint64_t old_prof_idx;
    ltsw_vlan_ing_entry_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_block_mask_validate(unit, block));

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vid, &vfi));
    sal_memset(&entry, 0, sizeof(ltsw_vlan_ing_entry_t));

    entry.vfi = vfi;
    entry.fld_bmp |= IV_FLD_BLOCK_PROF_IDX;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_LOOKUP, &entry));
    old_prof_idx = entry.block_prof_idx;

    sal_memset(&prof, 0, sizeof(ltsw_vlan_ing_block_mask_vfi_profile_t));

    /* Negate the mask to apply for HW. */
    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;

    SHR_BITNEGATE_RANGE(
        (uint32_t *)(&block->unicast), 0, port_num,
        (uint32_t *)(&(prof.mask[mask_ctrl->uc_mask_sel].mask_ports)));
    SHR_BITNEGATE_RANGE(
        (uint32_t *)(&block->known_multicast), 0, port_num,
        (uint32_t *)(&(prof.mask[mask_ctrl->mc_mask_sel].mask_ports)));
    SHR_BITNEGATE_RANGE(
        (uint32_t *)(&block->unknown_multicast), 0, port_num,
        (uint32_t *)(&(prof.mask[mask_ctrl->umc_mask_sel].mask_ports)));
    SHR_BITNEGATE_RANGE(
        (uint32_t *)(&block->unknown_unicast), 0, port_num,
        (uint32_t *)(&(prof.mask[mask_ctrl->uuc_mask_sel].mask_ports)));
    SHR_BITNEGATE_RANGE(
        (uint32_t *)(&block->broadcast), 0, port_num,
        (uint32_t *)(&(prof.mask[mask_ctrl->bc_mask_sel].mask_ports)));

    for (i = 0; i < LTSW_VLAN_MASK_SECT_CNT; i++) {
        prof.mask[i].mask_target = mask_target;
        prof.mask[i].mask_action = mask_action;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_block_mask_vfi_profile_add(unit, &prof, &prof_idx));
    new_prof_added = 1;
    entry.block_prof_idx = prof_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_table_op(unit, BCMLT_OPCODE_UPDATE, &entry));

    /* Default profile is reserved. */
    if (old_prof_idx != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_block_mask_vfi_profile_delete(unit, old_prof_idx));
    }
    new_prof_added = 0;
exit:
    if (new_prof_added && prof_idx != 0) {
        (void)vlan_ing_block_mask_vfi_profile_delete(unit, prof_idx);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete VLAN profile entry per index.
 *
 * This function is used to delete VLAN profile entry per index.
 *
 * \param [in] unit              Unit number.
 * \param [in] profile_hdl       Profile handle.
 * \param [in] index             Profile entry index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_profile_delete(int unit,
                             bcmi_ltsw_profile_hdl_t profile_hdl,
                             int index)
{
    SHR_FUNC_ENTER(unit);

    switch (profile_hdl) {
        case BCMI_LTSW_PROFILE_ING_MSHP_CHK:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_mshp_chk_profile_delete(unit, index));
            break;
        case BCMI_LTSW_PROFILE_EGR_MSHP_CHK:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_mshp_chk_profile_delete(unit, index));
            break;
        case BCMI_LTSW_PROFILE_EGR_UNTAG:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_untag_profile_delete(unit, index));
            break;
        case BCMI_LTSW_PROFILE_ING_BLOCK_MASK_VFI_SECT_0:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_block_mask_vfi_profile_delete(unit, index));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update VLAN range index when trunk member changes.
 *
 * This function is used to update VLAN range index when trunk member changes.
 *
 * \param [in]  unit              Unit Number.
 * \param [in]  gport             Original trunk member gport.
 * \param [in]  num_joining       Number of joining ports in trunk.
 * \param [in]  joining_arr       Joining ports in trunk.
 * \param [in]  num_leaving       Number of leaving ports in trunk.
 * \param [in]  leaving_arr       Leaving ports in trunk.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
xfs_ltsw_vlan_range_update(int unit, bcm_gport_t gport,
                           int num_joining, bcm_gport_t *joining_arr,
                           int num_leaving, bcm_gport_t *leaving_arr)
{
    int i, trunk_range_idx, port_range_idx, entries_per_set = 1;
    SHR_FUNC_ENTER(unit);

    /* This vlan range lock is nested inside trunk lock. */
    VLAN_RANGE_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_tab_get(unit, gport,
                                BCMI_PT_RANGE_MAP_INDEX, &trunk_range_idx));
    for (i = 0; i < num_joining; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, joining_arr[i],
                                    BCMI_PT_RANGE_MAP_INDEX, &port_range_idx));

        if (trunk_range_idx == port_range_idx) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_delete(unit, port_range_idx));
        if (trunk_range_idx != VLAN_PROFILE_RANGE_MAP_RESV0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_increase(
                         unit, BCMI_LTSW_PROFILE_VLAN_RANGE_MAP,
                         entries_per_set, trunk_range_idx, 1));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, joining_arr[i],
                                    BCMI_PT_RANGE_MAP_INDEX, trunk_range_idx));
    }

    if (trunk_range_idx == VLAN_PROFILE_RANGE_MAP_RESV0) {
        SHR_EXIT();
    }

    for (i = 0; i < num_leaving; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_get(unit, leaving_arr[i],
                                    BCMI_PT_RANGE_MAP_INDEX, &port_range_idx));
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_range_map_profile_delete(unit, port_range_idx));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_tab_set(unit, leaving_arr[i],
                                    BCMI_PT_RANGE_MAP_INDEX,
                                    VLAN_PROFILE_RANGE_MAP_RESV0));
    }
exit:
    VLAN_RANGE_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get port bitmap of reserved default VLAN flooding group.
 *
 * This function is used to get port pbmp of default VLAN flooding group.
 *
 * \param [in]   unit               Unit Number.
 * \param [in]   pbmp               Port bitmap.
 *
 * \retval SHR_E_NONE               No errors.
 * \retval !SHR_E_NONE              Failure.
 */
int
xfs_ltsw_vlan_def_flood_port_get(int unit, bcm_pbmp_t *pbmp)
{
    bcm_pbmp_t pbmp_cmic;
    bcm_pbmp_t pbmp_lb;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_entry_port_get(unit, BCM_VLAN_DEFAULT, NULL,
                                 pbmp, NULL, NULL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbmp_lb));
    BCM_PBMP_REMOVE(*pbmp, pbmp_lb);

    if (VLAN_DLF_FLOOD_MODE(unit) == LTSW_VLAN_FLOOD_MODE_EXCLUDE_CPU) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit,
                                   BCMI_LTSW_PORT_TYPE_CPU, &pbmp_cmic));
        BCM_PBMP_REMOVE(*pbmp, pbmp_cmic);
    }

exit:
      SHR_FUNC_EXIT();
}

/*
 * \brief Update vlan xlate pipe info.
 *
 * \param [in] unit              Unit number.
 * \param [in] pg                Port group.
 * \param [in] ingress           Ingress/egress.
 * \param [in] pipe              Pipe.
 * \param [in] op                Op code: Insert/Delete.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_xlate_pipe_update(int unit, bcm_port_t pg, int ingress,
                                int pipe, bcmi_ltsw_vlan_table_opcode_t op)
{
    ltsw_xlate_pipe_control_t *ctrl = NULL;
    ltsw_vlan_pg_info_t *pg_info = NULL;
    uint32_t all_pipe_pbmp = 0, pbmp, pbmp_del = 0, pbmp_insert = 0;
    int start = 0, is_dvp_pg = 0;
    SHR_FUNC_ENTER(unit);

    if (ingress) {
        ctrl = &(ltsw_vlan_info[unit].xlate_pipe_ctrl[0]);
        if (!VLAN_ING_XLATE_CTRL_EN(unit) && !VLAN_ING_XLATE_PIPE_EN(unit)) {
            SHR_EXIT();
        }
    } else {
        ctrl = &(ltsw_vlan_info[unit].xlate_pipe_ctrl[1]);
        if (!VLAN_EGR_XLATE_CTRL_EN(unit) && !VLAN_EGR_XLATE_PIPE_EN(unit)) {
            SHR_EXIT();
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_dvp_port_group_id_check(unit, pg, &is_dvp_pg));
    }

    pg_info = &(ctrl->pg_info[pg]);

    /* When there is no port in pg, all pipes are inserted.
     * So when first port is added, delete the extra pipe info.
     * When last port is deleted, add all pipes in HW.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_pp_pipe_bmp(unit, &all_pipe_pbmp));
    if (op == bcmiVlanTableOpInsert && pg_info->pipe_bmp == all_pipe_pbmp) {
        op = bcmiVlanTableOpDelete;
        pbmp = all_pipe_pbmp & (~pipe);
    } else if (op == bcmiVlanTableOpDelete && (pg_info->pipe_bmp == pipe)) {
        op = bcmiVlanTableOpInsert;
        pbmp = all_pipe_pbmp & (~pipe);
    } else {
        pbmp = pipe;
    }

    /* Return if no pg related action configured ever.*/
    if (pg_info->head == NULL) {

        /* For egr xlate dvp group case, op is always Set.*/
        if (!ingress && is_dvp_pg) {
            pg_info->pipe_bmp = pbmp;
        }

        SHR_EXIT();
    }

    start = 1;
    if (op == bcmiVlanTableOpSet) {
        /* Currently pipe will be 0 in two cases below. Add to all pipes.
         * 1. this pg does not have vp.
         * 2. this pg only only has vps on trunks which do not have ports.
         */
        if (pipe == 0) {
            pipe = all_pipe_pbmp;
        }

        /* Insert new pipe. */
        pbmp_insert= pipe & ~(pg_info->pipe_bmp);
        if (pbmp_insert) {
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_pipe_update_bmp(unit, pg, ingress, pbmp_insert,
                                            bcmiVlanTableOpInsert, ctrl));
        }

        /* Delete old pipe. */
        pbmp_del = (pg_info->pipe_bmp) & ~pipe;
        if (pbmp_del) {
            SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_xlate_pipe_update_bmp(unit, pg, ingress, pbmp_del,
                                                bcmiVlanTableOpDelete, ctrl));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
                (vlan_xlate_pipe_update_bmp(unit, pg, ingress, pbmp, op, ctrl));
    }
    start = 0;

exit:
    if (SHR_FUNC_ERR() && start) {
        /* Roll back on error.*/
        if (op == bcmiVlanTableOpSet) {
            if (pbmp_insert) {
                (void)vlan_xlate_pipe_update_bmp(unit, pg, ingress, pbmp_insert,
                                                 bcmiVlanTableOpDelete, ctrl);
            }

            if (pbmp_del) {
                (void)vlan_xlate_pipe_update_bmp(unit, pg, ingress, pbmp_del,
                                                 bcmiVlanTableOpInsert, ctrl);
            }
        } else {
            op = (op == bcmiVlanTableOpDelete) ?
                 bcmiVlanTableOpInsert : bcmiVlanTableOpDelete;
            (void)vlan_xlate_pipe_update_bmp(unit, pg, ingress, pbmp, op, ctrl);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set STG into a specified VLAN entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN id.
 * \param [in] ingress   Ingress or egress.
 * \param [in] stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
xfs_ltsw_vlan_stg_set(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t stg)
{
    uint32_t vfi;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vid, &vfi));

    if (ingress) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_stg_set(unit, vfi, stg));

    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_stg_set(unit, vfi, stg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get STG from a specified VLAN entry.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN id.
 * \param [in] ingress   Ingress or egress.
 * \param [out]stg       STG index.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
xfs_ltsw_vlan_stg_get(int unit, bcm_vlan_t vid, int ingress, bcm_stg_t *stg)
{
    uint32_t vfi;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vid, &vfi));

    if (ingress) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_stg_get(unit, vfi, stg));

    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_stg_get(unit, vfi, stg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Notify VLAN membership check information for L2 tunnel case.
 *
 * \param [in] unit      Unit Number.
 * \param [in] vid       VLAN id.
 * \param [in] add       Add or delete.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
xfs_ltsw_vlan_check_info_notify(int unit, bcm_vlan_t vid, int add)
{
    uint32_t vfi;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_vfi_resolve(unit, vid, &vfi));

    if (add) {
        /*
         * Default information is used here. All the information can be
         * retrieved from VFI LT tables if necessary.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_l2_station_default_update(unit, vfi));
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_l3_intf_default_update(unit, vfi));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_l2_station_clear(unit, vfi));
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_l3_intf_clear(unit, vfi));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach flex-counter into a given ingress VLAN translation table.
 *
 * This function is used to attach the count ID and object value into a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_flexctr_attach(int unit,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config)


{
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_flexctr_validate(unit, key_type, port,
                                         outer_vlan, inner_vlan));

    VLAN_ING_XLATE_LOCK(unit);
    locked = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_flexctr_attach(unit, key_type, port,
                                       outer_vlan, inner_vlan, config));
exit:
    if (locked) {
        VLAN_ING_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach flex-counter from a given ingress vlan translation table.
 *
 * This function is used to detach the count ID and object value from a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_flexctr_detach(int unit,
                                       bcm_vlan_translate_key_t key_type,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config)
{
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_flexctr_validate(unit, key_type, port,
                                         outer_vlan, inner_vlan));

    VLAN_ING_XLATE_LOCK(unit);
    locked = 1;
    if (config) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_flexctr_detach(unit, key_type, port,
                                           outer_vlan, inner_vlan, config));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_ing_xlate_flexctr_detach_all(unit, key_type, port,
                                               outer_vlan, inner_vlan));
    }

exit:
    if (locked) {
        VLAN_ING_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex-counter from a given ingress vlan translation table.
 *
 * This function is used to get the count ID and object value from a given
 * ingress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] key_type          VLAN translation key type.
 * \param [in] Port              GPORT (global port) identifier.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] array_size        Size of allocated entries in config array.
 * \param [out]config_array      Array of flex counter configurations.
 * \param [out]count             Number of elements in config array. If
 *                               array_size is 0, config array will be ignored
 *                               and count will be filled with an actual number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_ing_xlate_flexctr_get(int unit,
                                    bcm_vlan_translate_key_t key_type,
                                    bcm_gport_t port,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    int array_size,
                                    bcm_vlan_flexctr_config_t *config_array,
                                    int *count)
{
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_flexctr_validate(unit, key_type, port,
                                         outer_vlan, inner_vlan));
    VLAN_ING_XLATE_LOCK(unit);
    locked = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_xlate_flexctr_get(unit, key_type, port,
                                    outer_vlan, inner_vlan,
                                    array_size, config_array, count));
exit:
    if (locked) {
        VLAN_ING_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach flex-counter into a given egress VLAN translation table.
 *
 * This function is used to attach the count ID and object value into a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_egr_xlate_flexctr_attach(int unit,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config)

{
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_flexctr_validate(unit, port,
                                         outer_vlan, inner_vlan));

    VLAN_EGR_XLATE_LOCK(unit);
    locked = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_flexctr_attach(unit, port,
                                       outer_vlan, inner_vlan, config));
exit:
    if (locked) {
        VLAN_EGR_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Detach flex-counter from a given egress vlan translation table.
 *
 * This function is used to detach the count ID and object value from a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] config            Flex counter configuration.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_egr_xlate_flexctr_detach(int unit,
                                       bcm_gport_t port,
                                       bcm_vlan_t outer_vlan,
                                       bcm_vlan_t inner_vlan,
                                       bcm_vlan_flexctr_config_t *config)
{
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_flexctr_validate(unit, port,
                                         outer_vlan, inner_vlan));

    VLAN_EGR_XLATE_LOCK(unit);
    locked = 1;
    if (config) {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_xlate_flexctr_detach(unit, port,
                                           outer_vlan, inner_vlan, config));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (vlan_egr_xlate_flexctr_detach_all(unit, port,
                                               outer_vlan, inner_vlan));
    }

exit:
    if (locked) {
        VLAN_EGR_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex-counter from a given egress vlan translation table.
 *
 * This function is used to get the count ID and object value from a given
 * egress vlan translation table.
 *
 * \param [in] unit              Unit number.
 * \param [in] Port              Port ID.
 * \param [in] outer_vlan        Outer VLAN ID.
 * \param [in] inner_vlan        Inner VLAN ID.
 * \param [in] array_size        Size of allocated entries in config array.
 * \param [out]config_array      Array of flex counter configurations.
 * \param [out]count             Number of elements in config array. If
 *                               array_size is 0, config array will be ignored
 *                               and count will be filled with an actual number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
xfs_ltsw_vlan_egr_xlate_flexctr_get(int unit,
                                    bcm_gport_t port,
                                    bcm_vlan_t outer_vlan,
                                    bcm_vlan_t inner_vlan,
                                    int array_size,
                                    bcm_vlan_flexctr_config_t *config_array,
                                    int *count)
{
    int locked = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_flexctr_validate(unit, port,
                                         outer_vlan, inner_vlan));
    VLAN_EGR_XLATE_LOCK(unit);
    locked = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_xlate_flexctr_get(unit, port,
                                    outer_vlan, inner_vlan,
                                    array_size, config_array, count));
exit:
    if (locked) {
        VLAN_EGR_XLATE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Display VLAN software structure information.
 *
 * \param [in] unit      Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
int
xfs_ltsw_vlan_info_dump(int unit)
{
    ltsw_vlan_info_t *vi = &ltsw_vlan_info[unit];
    bcmi_ltsw_profile_hdl_t profile_hdl[] = {BCMI_LTSW_PROFILE_ING_MSHP_CHK,
                                             BCMI_LTSW_PROFILE_ING_MSHP_CHK,
                                             BCMI_LTSW_PROFILE_EGR_MSHP_CHK};
    uint8_t port_flg[] = {LTSW_VLAN_MSHIP_ING,
                          LTSW_VLAN_MSHIP_ING_EGR,
                          LTSW_VLAN_MSHIP_EGR};
    ltsw_vlan_ing_mshp_chk_profile_t ing_profile;
    ltsw_vlan_egr_mshp_chk_profile_t egr_profile;
    bcm_pbmp_t pbmp;
    uint32_t ref_count = 0;
    uint32_t port_num = 0;
    uint32_t offset = 0;
    uint32_t entry_num = 0;
    int i = 0, idx = 0, port = 0, lock = 0;
    int pre_state = 0, state = 0;

    SHR_FUNC_ENTER(unit);

    LOG_CLI((BSL_META_U(unit,
                        "\nVLAN XFS SW Information - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  inited            : %4d\n"), vi->inited));
    LOG_CLI((BSL_META_U(unit,
                        "  Support Flags     : %4d\n"), vi->flags));
    LOG_CLI((BSL_META_U(unit,
                        "  Support DLF flood : %4d\n"), vi->vlan_dlf_flood));
    LOG_CLI((BSL_META_U(unit,
                        "  DLF flood mode    : %4d\n"), vi->vlan_dlf_flood_mode));
    LOG_CLI((BSL_META_U(unit,
                        "  Block Mask Ctrl   : uc offset  = %4d\n"),
                        vi->block_mask_ctrl.uc_mask_sel));
    LOG_CLI((BSL_META_U(unit,
                        "                      mc offset  = %4d\n"),
                        vi->block_mask_ctrl.mc_mask_sel));
    LOG_CLI((BSL_META_U(unit,
                        "                      umc offset = %4d\n"),
                        vi->block_mask_ctrl.umc_mask_sel));
    LOG_CLI((BSL_META_U(unit,
                        "                      uuc offset = %4d\n"),
                        vi->block_mask_ctrl.uuc_mask_sel));
    LOG_CLI((BSL_META_U(unit,
                        "                      bc offset  = %4d\n"),
                        vi->block_mask_ctrl.bc_mask_sel));
    LOG_CLI((BSL_META_U(unit,
                        "  SW Mship Enabled: %4d\n\n"),
                        VLAN_SW_MSHIP_EN(unit) ? 1: 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, VLAN_ING_MEMBER_PROFILEs, &entry_num));

    port_num = bcmi_ltsw_dev_logic_port_max(unit) + 1;

    VLAN_PROFILE_LOCK(unit);
    lock = 1;

    for (i = 0; i < COUNTOF(port_flg); i++) {
        if (!VLAN_SW_MSHIP_EN(unit)) {
            break;
        }
        if (port_flg[i] == LTSW_VLAN_MSHIP_ING) {
            LOG_CLI((BSL_META_U(unit,
                                "    VLAN_ING_MEMBER_PROFILE:  \n")));
        } else if (port_flg[i] == LTSW_VLAN_MSHIP_ING_EGR) {
            LOG_CLI((BSL_META_U(unit,
                                "    VLAN_ING_EGR_MEMBER_PORTS_PROFILE:  \n")));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "    VLAN_EGR_MEMBER_PROFILE:  \n")));
        }

        for (idx = 0; idx < (int)entry_num; idx++) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_profile_ref_count_get(unit,
                                                 profile_hdl[i],
                                                 idx, &ref_count));
            if (ref_count == 0) {
                continue;
            }

            /* Display SW entry. */
            LOG_CLI((BSL_META_U(unit,
                                "      Entry = %4d: "), idx));
            offset = port_num * idx;
            LOG_CLI((BSL_META_U(unit, "[0")));
            for (port = 0; port < port_num; port++) {

                if (vi->mship[offset + port].port_info & port_flg[i]) {
                    state = 1;
                } else {
                    state = 0;
                }
                if ((pre_state != state) && (port > 0)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "-%d]=%d [%d"), port - 1,
                                        pre_state, port));
                }

                if (port + 1 == port_num) {
                    if (pre_state == state) {
                        LOG_CLI((BSL_META_U(unit, "-%d"), port));
                    }
                    LOG_CLI((BSL_META_U(unit, "]=%d"), state));
                }
                pre_state = state;
            }
            LOG_CLI((BSL_META_U(unit, "\n")));

            /* Compare the data between HW and SW. */
            if (port_flg[i] == LTSW_VLAN_MSHIP_ING) {
                sal_memset(&ing_profile, 0,
                           sizeof(ltsw_vlan_ing_mshp_chk_profile_t));
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_ing_member_profile_entry_get(
                        unit, idx, &ing_profile));
                BCM_PBMP_ASSIGN(pbmp, ing_profile.member_ports);
                sal_memset(&ing_profile, 0,
                           sizeof(ltsw_vlan_ing_mshp_chk_profile_t));
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_ing_member_profile_entry_sw_get(
                        unit, idx, &ing_profile));
                BCM_PBMP_XOR(pbmp, ing_profile.member_ports);
                BCM_PBMP_ITER(pbmp, port) {
                    if (!(vi->port_dis_chk[port] & VLAN_PORT_DIS_ING_CHK)) {
                        LOG_CLI((BSL_META_U(unit,
                                "      Mismatched Entry = %4d: port = %d \n"),
                                idx, port));
                        break;
                    }
                }
            } else if (port_flg[i] == LTSW_VLAN_MSHIP_ING_EGR) {
                sal_memset(&ing_profile, 0,
                           sizeof(ltsw_vlan_ing_mshp_chk_profile_t));
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_ing_egr_member_profile_entry_get(
                        unit, idx, &ing_profile));
                BCM_PBMP_ASSIGN(pbmp, ing_profile.egr_member_ports);
                sal_memset(&ing_profile, 0,
                           sizeof(ltsw_vlan_ing_mshp_chk_profile_t));
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_ing_egr_member_profile_entry_sw_get(
                        unit, idx, &ing_profile));
                BCM_PBMP_XOR(pbmp, ing_profile.egr_member_ports);
                BCM_PBMP_ITER(pbmp, port) {
                    if (!(vi->port_dis_chk[port] & VLAN_PORT_DIS_ING_EGR_CHK)) {
                        LOG_CLI((BSL_META_U(unit,
                                "      Mismatched Entry = %4d: port = %d \n"),
                                idx, port));
                        break;
                    }
                }
            } else {
                sal_memset(&egr_profile, 0,
                           sizeof(ltsw_vlan_egr_mshp_chk_profile_t));
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_egr_member_profile_entry_get(
                        unit, idx, &egr_profile));
                BCM_PBMP_ASSIGN(pbmp, egr_profile.member_ports);
                sal_memset(&egr_profile, 0,
                           sizeof(ltsw_vlan_egr_mshp_chk_profile_t));
                SHR_IF_ERR_VERBOSE_EXIT
                    (vlan_egr_member_profile_entry_sw_get(
                        unit, idx, &egr_profile));
                BCM_PBMP_XOR(pbmp, egr_profile.member_ports);
                BCM_PBMP_ITER(pbmp, port) {
                    if (!(vi->port_dis_chk[port] & VLAN_PORT_DIS_EGR_CHK)) {
                        LOG_CLI((BSL_META_U(unit,
                                "      Mismatched Entry = %4d: port = %d \n"),
                                idx, port));
                        break;
                    }
                }
            }
        }
    }

    if (VLAN_SW_MSHIP_EN(unit)) {
        LOG_CLI((BSL_META_U(unit,
                            "\n  Disable state of VLAN check:  \n")));
        for (i = 0; i < port_num; i++) {
            if (!VLAN_SW_MSHIP_EN(unit)) {
                break;
            }
            if (vi->port_dis_chk[i]) {
                LOG_CLI((BSL_META_U(unit,
                                    "    port = %4d: flags = %x\n"),
                                    i, vi->port_dis_chk[i]));
            }
        }
    }
    VLAN_PROFILE_UNLOCK(unit);
    lock = 0;

    (void)vlan_xlate_entry_dump(unit, 1);
    (void)vlan_xlate_entry_dump(unit, 0);

exit:
    if (lock) {
        VLAN_PROFILE_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}
