/*! \file port_internal.h
 *
 * BCM port module Structures and Definitions used internally only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_PORT_INTERNAL_H
#define BCMI_LTSW_PORT_INTERNAL_H
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/generated/port_ha.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/event_mgr.h>


#include <sal/sal_mutex.h>
#include <shr/shr_error.h>

#undef NUM_MODID
#undef NUM_PORT
#undef NUM_ALL_PORT
#undef IS_CPU_PORT
#undef IS_LB_PORT
#undef IS_RDB_PORT
#undef IS_XL_PORT
#undef IS_CL_PORT

/* Max instance number of LPORT_TAB */
#define MAX_INST_NUM_LPORT_TAB   8

#define LOGIC_INDEX_TO_PIPE(_logic_index)   ((_logic_index >> 16) & 0xF)
#define LOGIC_INDEX_TO_INDEX(_logic_index)  (_logic_index & 0xFF)

#define LOGIC_INDEX_GET(_index, _pipe)      ((_pipe << 16) | _index)

typedef struct ltsw_pc_port_init_data_s {
    uint32_t enable;
    uint32_t max_frame_size;
    uint32_t num_lanes;
    uint32_t autoneg;
    uint32_t link_training;
    uint32_t speed;
    uint32_t lpbk_mode;
    uint32_t fec_mode;
} ltsw_pc_port_init_data_t;

#define PORT(_u, _p)        ltsw_port_info[(_u)]->port[(_p)]
#define PORT_INIT(_u) \
        if (ltsw_port_info[(_u)] == NULL) {return SHR_E_INIT;}
#define PORT_LOCK(_u)       \
        sal_mutex_take(ltsw_port_info[(_u)]->lock, SAL_MUTEX_FOREVER)
#define PORT_UNLOCK(_u)     \
        sal_mutex_give(ltsw_port_info[(_u)]->lock)
#define PORT_NUM_MAX(_u)        (ltsw_port_info[(_u)]->port_num_max)
#define PHY_PORT_NUM_MAX(_u)    (ltsw_port_info[(_u)]->phy_port_num_max)
#define PORT_ADDR_MAX(_u)       (ltsw_port_info[(_u)]->port_addr_max)
#define NUM_MODID(_u)           (ltsw_port_info[(_u)]->modid_count)
#define MODID_MAX(_u)           (ltsw_port_info[(_u)]->modid_max)
#define PROFILE_VLAN_ASSIGN_PROT(_u)    (&(ltsw_port_info[(_u)]->profile_vlan_assign_prot))
#define PROFILE_PC_AUTONEG(_u)          (&(ltsw_port_info[(_u)]->pc_an_profile_info))
#define PC_PORT_INIT_DATA(_u, _p)       (&(ltsw_port_info[(_u)]->pc_port_init_data[(_p)]))
#define PHY_PORT(_u, _p)        ltsw_port_info[(_u)]->phy_port[(_p)]
#define PORT_FDR_CBS(_u)        ltsw_port_info[(_u)]->port_fdr_cbs

#define PORT_ITER(_u, _pt, _p)  \
    for ((_p) = 0;  \
         (_p) < PORT_NUM_MAX(_u);  \
         (_p) ++)   \
            if (PORT((_u), (_p)).port_type & (_pt))

#define PHY_PORT_ITER(_u, _p)   \
    for ((_p) = 0;  \
         (_p) < PHY_PORT_NUM_MAX(_u);  \
         (_p) ++)   \
            if (PHY_PORT((_u), (_p)).port != -1)

#define PORT_ETH_ITER(_u, _p)   PORT_ITER((_u), BCMI_LTSW_PORT_TYPE_ETH, (_p))
#define PORT_LB_ITER(_u, _p)    PORT_ITER((_u), BCMI_LTSW_PORT_TYPE_LB, (_p))
#define PORT_FP_ITER(_u, _p)    PORT_ITER((_u), BCMI_LTSW_PORT_TYPE_PORT, (_p))
#define PORT_ALL_ITER(_u, _p)   PORT_ITER((_u), BCMI_LTSW_PORT_TYPE_ALL, (_p))

#define NUM_ETH_PORT(_u)        bcmi_ltsw_port_number_count((_u), BCMI_LTSW_PORT_TYPE_ETH)
#define NUM_LB_PORT(_u)         bcmi_ltsw_port_number_count((_u), BCMI_LTSW_PORT_TYPE_LB)
#define NUM_PORT(_u)            bcmi_ltsw_port_number_count((_u), BCMI_LTSW_PORT_TYPE_PORT)
#define NUM_ALL_PORT(_u)        bcmi_ltsw_port_number_count((_u), BCMI_LTSW_PORT_TYPE_ALL)

#define IS_ETH_PORT(_u, _p)     (PORT((_u),(_p)).port_type & BCMI_LTSW_PORT_TYPE_ETH)
#define IS_CPU_PORT(_u, _p)     (PORT((_u),(_p)).port_type & BCMI_LTSW_PORT_TYPE_CPU)
#define IS_MGMT_PORT(_u, _p)    (PORT((_u),(_p)).port_type & BCMI_LTSW_PORT_TYPE_MGMT)
#define IS_LB_PORT(_u, _p)      (PORT((_u),(_p)).port_type & BCMI_LTSW_PORT_TYPE_LB)
#define IS_RDB_PORT(_u, _p)     (PORT((_u),(_p)).port_type & BCMI_LTSW_PORT_TYPE_RDB)
#define IS_XL_PORT(_u, _p)      (PORT((_u),(_p)).port_type & BCMI_LTSW_PORT_TYPE_XL)
#define IS_CL_PORT(_u, _p)      (PORT((_u),(_p)).port_type & BCMI_LTSW_PORT_TYPE_CL)

#define MODID_VALID_RANGE(_u, _m)   \
    ((_m) >= 0 && (_m) <= MODID_MAX((_u)))
#define PORT_VALID_RANGE(_u, _p)    \
    ((_p) >= 0 && (_p) < PORT_NUM_MAX(_u))
#define PORT_IS_VALID(_u, _p)          \
    (PORT_VALID_RANGE((_u), (_p)) &&    \
     PORT((_u), (_p)).port_type & BCMI_LTSW_PORT_TYPE_ALL)
#define PHY_PORT_VALID_RANGE(_u, _p)    \
        ((_p) >= 0 && (_p) < PHY_PORT_NUM_MAX(_u))
#define PHY_PORT_IS_VALID(_u, _p)          \
        (PHY_PORT_VALID_RANGE((_u), (_p)) &&    \
         ((ltsw_port_info[(_u)]->phy_port[(_p)].flags & INACTIVE) == 0))

#define LTSW_PORT_ABS(_val) ((_val > 0) ? _val : -_val)

typedef int (*pc_map_val_set_f) (int unit, int in_val, int *out_val);
typedef int (*pc_map_val_get_f) (int unit, int in_val, int *out_val);

/* Port control type map to logical table and field directly. */
typedef struct port_control_map_s
{
    int type;
    bcmi_ltsw_port_tab_t fld;
    pc_map_val_set_f func_set;
    pc_map_val_get_f func_get;
    uint32_t flags;
#define PORT_CTRL_PM    (1 << 0)
} port_control_map_t;

typedef struct ltsw_port_info_s {
    uint32_t            port_type;
    uint32_t            flags;
#define FLEXIBLE        (1 << 0)
#define NO_REMAP        (1 << 1)
#define CT_VALID        (1 << 2)
#define HG_VALID        (1 << 3)
#define INACTIVE        (1 << 4)
    int                 pport;
    char                port_name[BCMI_LTSW_PORT_NAME_MAX];
} ltsw_port_info_t;

typedef struct ltsw_phy_port_info_s {
    uint32_t            flags;
    int                 port;
} ltsw_phy_port_info_t;

typedef int (*ltsw_port_profile_lt_get_f)(int unit, int index, void *profile);
typedef int (*ltsw_port_profile_lt_insert_f)(int unit, int index, void *profile);
typedef int (*ltsw_port_profile_lt_delete_f)(int unit, int index);
typedef int (*ltsw_port_profile_data_int_f)(int unit, void *profile_data);

typedef struct ltsw_pc_profile_info_s {
    bcmi_ltsw_profile_hdl_t phd;
    int entry_idx_min;
    int entry_idx_max;
    int entries_per_set;
    int entry_size;
    bcmi_lt_entry_handle_cb pc_profile_field_set;
    bcmi_lt_entry_handle_cb pc_profile_field_get;
} ltsw_pc_profile_info_t;

typedef struct ltsw_port_profile_info_s {
    bcmi_ltsw_profile_hdl_t phd;
    int entry_idx_min;
    int entry_idx_max;
    int entries_per_set;
    int entry_size;
    ltsw_port_profile_lt_get_f lt_get;
    ltsw_port_profile_lt_insert_f lt_ins;
    ltsw_port_profile_lt_delete_f lt_del;
    /* For non unique mode, inst is 0 always. */
    int inst;
} ltsw_port_profile_info_t;

#define HA_PAUSE_RX_EN     (1 << 0)

typedef struct ltsw_dport_info_s {
    int enable;
    int indexed;
    int direct;
    int dport_num;
    int *l2d;
    int *d2l;
} ltsw_dport_info_t;

#define PC_FDR_CW_S0_ERR    (1 << 0)
#define PC_FDR_CW_S1_ERR    (1 << 1)
#define PC_FDR_CW_S2_ERR    (1 << 2)
#define PC_FDR_CW_S3_ERR    (1 << 3)
#define PC_FDR_CW_S4_ERR    (1 << 4)
#define PC_FDR_CW_S5_ERR    (1 << 5)
#define PC_FDR_CW_S6_ERR    (1 << 6)
#define PC_FDR_CW_S7_ERR    (1 << 7)
#define PC_FDR_CW_S8_ERR    (1 << 8)

/* Port callback list of fdr interrupt notification. */
typedef struct port_fdr_cb_s {
    struct port_fdr_cb_s *next;
    bcm_port_fdr_handler_t cb;
    void* user_data;
} port_fdr_cb_t;

/* Port FDR Statistics notification info. */
typedef struct port_fdr_stats_notif_s {
    bcmlt_opcode_t op;
    int port;
    uint64 start_time;
    uint64 end_time;
    uint32 num_uncorrect_cws;
    uint32 num_cws;
    uint32 num_symb_errs;
    uint32 cw_s0_errs;
    uint32 cw_s1_errs;
    uint32 cw_s2_errs;
    uint32 cw_s3_errs;
    uint32 cw_s4_errs;
    uint32 cw_s5_errs;
    uint32 cw_s6_errs;
    uint32 cw_s7_errs;
    uint32 cw_s8_errs;
} port_fdr_stats_notif_t;

typedef struct ltsw_port_ctrl_info_s {
    sal_mutex_t                 lock;
    int                         modid_max;
    int                         modid_count;
    int                         port_num_max;
    int                         port_addr_max;
    int                         phy_port_num_max;
    int                         pm_num_max;
    int                         min_lanes_per_pport;
    int                         max_frame_size;
    ltsw_pc_profile_info_t      pc_an_profile_info;
    ltsw_port_info_t            *port;
    ltsw_phy_port_info_t        *phy_port;
    ltsw_pc_port_init_data_t    *pc_port_init_data;
    bcmint_port_ha_info_t       *port_ha;
    ltsw_dport_info_t           dport;
    port_fdr_cb_t               *port_fdr_cbs;
} ltsw_port_ctrl_info_t;

typedef struct ltsw_port_map_s {
    int lport;
    int pipe;
    int pport;
    int ptype;
    int flags;
} ltsw_port_map_t;

extern ltsw_port_ctrl_info_t *ltsw_port_info[BCM_MAX_NUM_UNITS];

/*! HA subcomponent id for port ha information. */
#define BCMINT_PORT_SUB_COMP_ID_HA      (0)
#define BCMINT_PORT_SUB_COMP_ID_XFS_HA  (1)

#define LTSW_PM_MAX_PLL_NUM 2

typedef enum ltsw_pm_type_e {
    ltswPmTypeNone        = 0,
    ltswPmTypeCpu         = 1,
    ltswPmTypeLoopback    = 2,
    ltswPmTypePm4x10      = 3,
    ltswPmTypePm4x25      = 4,
    ltswPmTypePm8x50      = 5,
    ltswPmTypePm8x50Gen2  = 6,
    ltswPmTypePm8x50Gen3  = 7,
    ltswPmTypePmqtc       = 8,
    ltswPmTypePmsgmii4px2 = 9,
    ltswPmTypePm8x100     = 10
} ltsw_pm_type_t;

typedef enum ltsw_port_vco_type_e {
    ltswPortVCOInvalid = -1,
    ltswPortVCONone    = 0,
    ltswPortVCO10P3125 = 1,
    ltswPortVCO12P5    = 2,
    ltswPortVCO20P625G = 3,
    ltswPortVCO25P781G = 4,
    ltswPortVCO26P562G = 5,
    ltswPortVCO41P25G  = 6,
    ltswPortVCO51P562G = 7,
    ltswPortVCO53P125G = 8,
    ltswPortVCOCount
} ltsw_port_vco_type_t;

typedef enum ltsw_port_custom_speed_vco_type_e {
    ltswSpeedVcoNone      = 0,
    ltswSpeed1gAt6p25g    = 1,
    ltswSpeed1gAt12p5g    = 2,
    ltswSpeed1gAt25p781g  = 3,
    ltswSpeed2p5gAt12p5g  = 4,
    ltswSpeed5gAt12p5g    = 5,
    ltswSpeedVcoCount
} ltsw_port_custom_speed_vco_type_t;

typedef struct ltsw_port_fs_ability_table_entry_t_s {
    uint32 speed; /* port speed in Mbps */
    uint32 num_lanes; /* number of lanes */
    bcm_port_phy_fec_t fec_type; /* FEC type */
    ltsw_port_vco_type_t vco; /* associated VCO rate of the ability */
} ltsw_port_fs_ability_table_entry_t_t;

typedef struct ltsw_port_an_ability_table_entry_s {
    uint32 speed; /* port speed in Mbps */
    uint32 num_lanes; /* number of lanes */
    bcm_port_phy_fec_t fec_type; /* FEC type */
    bcm_port_autoneg_mode_t an_mode; /* autoneg mode such as cl73, bam or msa */
    ltsw_port_vco_type_t vco; /* associated VCO rate of the ability */
} ltsw_port_an_ability_table_entry_t;

static inline int
is_fp_port(int u, int p)
{
    return (ltsw_port_info[u]->port[p].port_type & BCMI_LTSW_PORT_TYPE_PORT);
}

static inline int
port_fp_validate(int u, int p)
{
    return (is_fp_port(u, p) ? SHR_E_NONE : SHR_E_PORT);
}

static inline int
is_aux_port(int u, int p)
{
    return (ltsw_port_info[u]->port[p].port_type & (BCMI_LTSW_PORT_TYPE_CPU |
                                                    BCMI_LTSW_PORT_TYPE_MGMT |
                                                    BCMI_LTSW_PORT_TYPE_LB |
                                                    BCMI_LTSW_PORT_TYPE_RDB));
}

typedef int (*ltsw_port_tab_index_get_f)(int unit, int port, int *index);

typedef struct ltsw_port_tab_lt_info_s {
    const char      *name;
    const char      *key;
    ltsw_port_tab_index_get_f   index_get;
} ltsw_port_tab_lt_info_t;

/* symbol <-> enum convertion */
typedef struct ltsw_port_tab_sym_list_s
{
    int         len;
    const char  **syms;
} ltsw_port_tab_sym_list_t;

/* int <-> enum convertion */
typedef struct ltsw_port_tab_int_list_s
{
    int         len;
    const int   *vals;
} ltsw_port_tab_int_list_t;

/* enum combined of multiple fields, eg SAMPLER_INDEX_3_0 + SAMPLER_INDEX_5_4 */
typedef struct ltsw_port_tab_com_fld_s
{
    const char  *field;
    uint8_t     width;
    uint8_t     offset;
} ltsw_port_tab_com_fld_t;

typedef struct ltsw_port_tab_com_fld_list_s
{
    int                             len;
    const ltsw_port_tab_com_fld_t   *flds;
} ltsw_port_tab_com_fld_list_t;

typedef struct ltsw_port_tab_info_s {
    const ltsw_port_tab_lt_info_t   *table;
    const char                      *field;
    uint32_t                        flags;
#define FLD_IS_ENUM                 (1 << 0)
#define FLD_IS_SYMBOL               (1 << 1)
#define FLD_IS_ARRAY                (1 << 2)
#define FLD_IS_COMBINED             (1 << 3)
#define SYNC_TO_TRUNK               (1 << 4)
#define SYNC_TO_LPORT               (1 << 5)
    const void                      *list;
} ltsw_port_tab_info_t;

typedef struct ltsw_port_tab_field_s {
    bcmi_ltsw_port_tab_t        type;
    const ltsw_port_tab_info_t  *info;
    int                         index;
    uint32_t                    op;
#define PORT_CFG                (1 << 0)
#define LPORT_CFG               (1 << 1)
#define TRUNK_CFG               (1 << 3)
#define ID_KNOWN                (1 << 4)
    union {
        uint64_t                n;
        struct {
            uint64_t            *buf;
            uint16_t            size;
            uint16_t            idx;
        } a;
    } data;
} ltsw_port_tab_field_t;

/*
 * FlexPort Operations Changes
 *
 * Flags to be used to determine the type of operations required
 * when the FlexPort API multi_set() is called.
 *
 * OP_NONE  - No changes.
 * OP_ADD   - Add a new logical port. It requires FlexPort sequence.
 * OP_DEL   - Delete a logical port. It requires FlexPort sequence.
 * OP_REMAP - Change in port mapping.  It requires FlexPort sequence.
 * OP_LANES - Change in lanes.  It requires FlexPort sequence.
 * OP_SPEED - Change in speed.  This is covered in a FlexPort sequence.
 *            If 'remap' or 'lanes' are not changing, then
 *            calling the 'speed' set function is enough (no need
 *            to call the FlexPort sequence).
 * OP_ENCAP - Change in encap mode.  It is NOT covered in a FlexPort
 *            sequence, so an explicit call must be made to change
 *            the encap mode.
 * OP_DETACH- Detach a port. Required by port deletion or modification.
 * OP_ATTACH_FIRST  - The port need be attached before other ports.
 * OP_PC_RESET      - The port need be delete then add on pc level.
 */
#define FLEXPORT_OP_NONE    (0)
#define FLEXPORT_OP_ADD     (1 << 0)
#define FLEXPORT_OP_DEL     (1 << 1)
#define FLEXPORT_OP_REMAP   (1 << 2)
#define FLEXPORT_OP_LANES   (1 << 3)
#define FLEXPORT_OP_SPEED   (1 << 4)
#define FLEXPORT_OP_FEC     (1 << 5)
#define FLEXPORT_OP_PMD     (1 << 6)
#define FLEXPORT_OP_TRAIN   (1 << 7)
#define FLEXPORT_OP_ENCAP   (1 << 8)
#define FLEXPORT_OP_DETACH  (1 << 16)
#define FLEXPORT_OP_ATTACH_FIRST    (1 << 17)
#define FLEXPORT_OP_ATTACH_SECOND   (1 << 18)
#define FLEXPORT_OP_PC_RESET        (1 << 19)

/*
 * Flexport Internal Flags
 *
 * FLG_NO_REMAP - Only speed, fec_type, etc can be changed.
 *                Can't add/del/remap port. Can't change lanes.
 */

#define FLEXPORT_FLG_NO_REMAP   (1 << 31)

typedef struct ltsw_flexport_port_validation_resolve_s {
    int required_vco_num;
    ltsw_port_vco_type_t required_vco[2];
    uint32_t new_rs544_bmp;
    uint32_t new_rs272_bmp;
} ltsw_flexport_port_validation_resolve_t;

typedef struct ltsw_flexport_port_cfg_s {
    int valid;
    uint32_t op;
    uint32_t flags;
    int lport;
    int pport;
    int pmid;
    int pmport;
    int lanes;
    int speed;
    int fec;
    int pmd;
    int training;
    int encap;
} ltsw_flexport_port_cfg_t;

typedef struct ltsw_flexport_port_info_s {
    int valid;
    int pport;
    int pmid;
    int pmport;
    int lanes;
    int speed;
    int fec;
    int pmd;
    int training;
    int encap;
} ltsw_flexport_port_info_t;

#define MAX_PORT_PER_PM   16
typedef struct ltsw_flexport_pm_cfg_s {
    int valid;
    bcm_pbmp_t pbm;
    int num_op;
    ltsw_flexport_port_cfg_t *operation[MAX_PORT_PER_PM * 2];
} ltsw_flexport_pm_cfg_t;

typedef struct ltsw_flexport_resource_s {
    int num_port;
    int num_pm;
    int num_op;
    ltsw_flexport_port_info_t *current;
    ltsw_flexport_port_cfg_t *operation;
    ltsw_flexport_port_info_t *new;
    ltsw_flexport_pm_cfg_t *pm;
} ltsw_flexport_resource_t;

/* TPID types */
#define OUTER_TPID              0x1
#define INNER_TPID              0x2
#define PAYLOAD_OUTER_TPID      0x4
#define PAYLOAD_INNER_TPID      0x8
#define OBM_OUTER_TPID          0x10
#define OBM_INNER_TPID          0x20

/* port class */
#define CLASS_ID_INVALID        ((uint32_t)(-1))

/*!
 * \brief Get profile entries from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] index First profile entry index.
 * \param [in] pinfo Profile information.
 * \param [out] profile Profile entries.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_profile_get(int unit, int index,
                             ltsw_port_profile_info_t *pinfo, void *profile);

/*!
 * \brief Add profile.
 *
 * If same entries existed, just increament reference count.
 *
 * \param [in] unit Unit number.
 * \param [in] profile Profile to be added.
 * \param [in] ref_count Add how many reference counts for this profile.
 * \param [in] pinfo Profile information.
 * \param [out] index Index of the added or existed profile enrty.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_profile_add(int unit, void *profile, int ref_count,
                             ltsw_port_profile_info_t *pinfo, int *index);

/*!
 * \brief Add profile reference.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the added profile entry.
 * \param [in] ref_count Add how many reference counts for this profile.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_profile_add_ref(int unit, int index, int ref_count,
                                 ltsw_port_profile_info_t *pinfo);

/*!
 * \brief Delete profile.
 *
 * If the entry is referred by more than one soure, just decreament ref count.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index of the first entry to be deleted.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_profile_delete(int unit, int index,
                                ltsw_port_profile_info_t *pinfo);

/*!
 * \brief Recover hash and reference count for profile.
 *
 * Hash is recovered from HW. Reference count is always increamented.
 *
 * \param [in] unit Unit number.
 * \param [in] index Profile index to be recovered.
 * \param [in] pinfo Profile information.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_profile_recover(int unit, int index,
                                 ltsw_port_profile_info_t *pinfo);

/*!
 * \brief Compare profile set.
 *
 * Compare if given profile entries equals to the entries in LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entries First profile entry in the set.
 * \param [in] index Profile table index to be compared.
 * \param [in] pinfo Profile information.
 * \param [out] cmp 0 if equal, otherwise not equal.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_profile_cmp(int unit, void *entries, int index,
                             ltsw_port_profile_info_t *pinfo, int *cmp);

/*!
 * \brief Set multiple PORT/LPORT fields.
 *
 * Helper funtion for port init and port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] fields Array of LT fields to modify.
 * \param [in] fields_cnt Number of fields to modify.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_tab_multi_set(int unit, bcm_port_t port,
                               ltsw_port_tab_field_t *fields,
                               int fields_cnt);

/*!
 * \brief Get multiple PORT/LPORT fields.
 *
 * Helper funtion for port init and port control APIs.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] fields Array of LT fields to get.
 * \param [in] fields_cnt Number of fields to get.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_tab_multi_get(int unit, bcm_port_t port,
                               ltsw_port_tab_field_t *fields,
                               int fields_cnt);

/*!
 * \brief Helper funtion to validate port/gport parameter for port controls.
 *
 * \param [in] unit Unit number.
 * \param [in] port_in Port / Gport to validate.
 * \param [out] port_out Port number if valid.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_control_gport_validate(int unit, bcm_port_t port_in,
                                        bcm_port_t *port_out);

/*!
 * \brief Get the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [out] value Current value of the port feature.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_control_pm_get(int unit, bcm_port_t port,
                                bcm_port_control_t type,
                                int *value);

/*!
 * \brief Set the status of specified port feature.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] type Enum value of the port feature.
 * \param [in] value Value of the port feature to be set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_control_pm_set(int unit, bcm_port_t port,
                                bcm_port_control_t type,
                                int value);

/*!
 * \brief Initialize PC profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_pc_profile_init(int unit);

/*!
 * \brief De-initialize PC profile information.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_pc_profile_deinit(int unit);

/*!
 * \brief Initialize PC profile information on specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_pc_profile_id_init_per_port(int unit, bcm_port_t port);

/*!
 * \brief Clear PC profile information on specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_pc_profile_clear_per_port(int unit, bcm_port_t port);

extern int bcmint_ltsw_pc_profile_add(int unit, void * profile, int ref_count,
                                ltsw_pc_profile_info_t * pinfo, int * index);

extern int bcmint_ltsw_pc_profile_delete(int unit, int index,
                                ltsw_pc_profile_info_t * pinfo);

extern int bcmint_ltsw_pc_profile_get(int unit, int index,
                                ltsw_pc_profile_info_t * pinfo, void * profile);

extern int bcmint_ltsw_pc_profile_cmp(int unit, void * entries, int index,
                                ltsw_pc_profile_info_t * pinfo, int * cmp);
/*!
 * \brief Clear PC profile information on specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_pc_profile_clear_per_port(int unit, bcm_port_t port);

/*!
 * \brief Get number of Port Macros in a device.
 *
 * \param [in] unit Unit number.
 * \param [out] max_pm_num Max PM numbers.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_pc_max_pm_size_get(int unit, int *max_pm_num);

/*!
 * \brief Intialize port-based SW data.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_software_init_per_port(int unit, int port);

/*!
 * \brief De-intialize port-based SW data.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_software_deinit_per_port(int unit, int port);

/*!
 * \brief Initialize PHY and MAC for port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_pm_init_per_port(int unit, int port);

/*!
 * \brief Check array order. Delete operation must be first in array.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_flexport_operation_order_validate(
    int unit,
    ltsw_flexport_resource_t *resource);

/*!
 * \brief Convert gport, validate logical and physical ports are in range and flexible.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_flexport_port_eligible_validate(
    int unit,
    ltsw_flexport_resource_t *resource);

/*!
 * \brief Check that existed ports are disabled.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_flexport_port_status_validate(int unit,
                                          ltsw_flexport_resource_t *resource);

/*!
 * \brief Check ports to be 'detached' do not have linkscan mode set.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_flexport_linkscan_validate(int unit,
                                       ltsw_flexport_resource_t *resource);

/*!
 * \brief Check logical port and physical port are in same pipe.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_flexport_pipe_validate(int unit,
                                   ltsw_flexport_resource_t *resource);

/*!
 * \brief Check port numbers are available and mappings are not replicated.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_flexport_port_mapping_validate(int unit,
                                           ltsw_flexport_resource_t *resource);

/*!
 * \brief Check pmd lane configurations are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_lane_config_validate(int unit,
                                          ltsw_flexport_resource_t *resource);

/*!
 * \brief Check link training configurations are valid.
 *
 * \param [in] unit Unit number.
 * \param [in] resource Flexport internal resource.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_flexport_link_training_validate(int unit,
                                            ltsw_flexport_resource_t *resource);

/*!
 * \brief get the lane mask for the logic port.
 *
 * \param [in]  unit Unit number.
 * \param [in]  port Port number.
 * \param [out] lane_mask lane mask of the port.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */

extern int
bcmint_ltsw_port_lane_mask_get(int unit,
                                  bcm_port_t port, uint32_t *lane_mask);

/*!
 * \brief VCO get.
 *
 * \param [in] unit    Unit number.
 * \param [in] pm_id   pm id.
 * \param [OUT] pll_num PLL number in the PM core.
 * \param [OUT] tvco.
 * \param [OUT] ovco ovco is invalid if it`s single PLL.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
int
bcmint_ltsw_port_vco_get(int unit, int pm_id, int *pll_num, int *tvco, int *ovco);

/*!
 * \brief pm type get.
 *
 * \param [in] unit Unit number.
 * \param [in] pm id.
 * \param [OUT] value .
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_pm_type_get(int unit, int pm_id, int *pm_type);

/*!
 * \brief lane mask get by physical port.
 *
 * \param [in] unit     Unit number.
 * \param [in] pport    physical Port number.
 * \param [in] num_lane lane Port number.
 * \param [OUT] lane_mask .
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_pport_lane_mask_get(int unit, int pport, int num_lane, uint32_t *lane_mask);

/*!
 * \brief port pbmp get by the PM_ID.
 *
 * \param [in] unit     Unit number.
 * \param [in] pm_id    pm id.
 * \param [OUT] port_pbmp .
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_pm_ports_get(int unit, int pm_id, bcm_pbmp_t *port_pbmp);

extern int
bcmint_ltsw_port_num_lane_get(int unit, bcm_port_t port, uint32_t *num_lane);

/*!
 * \brief Get the customer vco for the speed.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] custom_speed_vco.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_custom_speed_vco_get(int unit, bcm_port_t port, int *custom_speed_vco);

/*!
 * \brief pm id get.
 *
 * \param [in] unit     Unit number.
 * \param [in] pport    physical port.
 * \param [OUT] pm_id   pm_id.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_pport_pm_id_get(int unit, int pport, int *pm_id);

/*!
 * \brief Set the pause state for the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] pause_tx Boolean value.
 * \param [in] pause_rx Boolean value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_pause_set(int unit, bcm_port_t port, int pause_tx, int pause_rx);

/*!
 * \brief Set encapsulation mode on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] mode Encapsulation mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_encap_set(int unit, bcm_port_t port, int mode);

/*!
 * \brief Get encapsulation mode on a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] mode Encapsulation mode.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_encap_get(int unit, bcm_port_t port, int *mode);

/*!
 * \brief Initialize encapsulation mode for all ports.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_encap_init(int unit);

/*!
 * \brief Get the autonegotiation state of the port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] autoneg Boolean value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_autoneg_get(int unit, bcm_port_t port, int *autoneg);

/*!
 * \brief Retrieve the local port abilities.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] the max num of autoneg ability this port can advertise.
 * \param [out] values indicating the ability this port advertise.
 * \param [out] the actual num of ability that this port advertise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
*/
extern int
bcmint_ltsw_port_autoneg_ability_advert_get(int unit,
                                     bcm_port_t port,
                                     int max_num_ability,
                                     bcm_port_speed_ability_t *abilities,
                                     int *actual_num_ability);

/*!
 * \brief Get the port resource configuration for the specified port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] resource Port resource configuration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_resource_get(int unit,
                           bcm_gport_t port, bcm_port_resource_t *resource);

/*!
 * \brief Get the PortMacro ID of the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] pm_id PortMacro ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_pm_id_get(int unit, bcm_port_t port, int *pm_id);

/*!
 * \brief Return current Link up/down status.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] status FALSE for link down and TRUE for link up.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_link_status_get(int unit, bcm_port_t port, int * status);

/*!
 * \brief Physically enable/disable the MAC/PHY on this port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] enable TRUE for port is enabled, FALSE for port is disabled.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmint_ltsw_port_enable_set(int unit, bcm_port_t port, int enable);

/*!
* \brief Set the configuration for Flight Data Recorder (FDR) on a port
*
* \param [in]   unit    Unit number.
* \param [in]   port    Port number.
* \param [in]  fdr_config   Flight Data Recorder (FDR) configuration.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
extern int
bcmint_ltsw_port_fdr_config_set(int unit, bcm_port_t port, bcm_port_fdr_config_t *fdr_config);

/*!
* \brief Get the configuration for Flight Data Recorder (FDR) on a port
*
* \param [in]   unit    Unit number.
* \param [in]   port    Port number.
* \param [out]  fdr_config   Flight Data Recorder (FDR) configuration.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
extern int
bcmint_ltsw_port_fdr_config_get(int unit, bcm_port_t port, bcm_port_fdr_config_t *fdr_config);

/*!
* \brief Get the statistics of Flight Data Recorder (FDR) on a port
*
* \param [in]   unit    Unit number.
* \param [in]   port    Port number.
* \param [in]   field bmp   Field bitmap.
* \param [out]  fdr_stats   Flight Data Recorder (FDR) statistics.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
extern int
bcmint_ltsw_port_fdr_stats_get(int unit, int port, uint32_t field_bmp, bcm_port_fdr_stats_t *fdr_stats);

/*!
* \brief Register port Flight Data Recorder (FDR) interrupt notification callbacks
*
* \param [in]   unit       Unit number.
* \param [in]   f          Flight Data Recorder (FDR) callback.
* \param [in]   user_data  user data pointer.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
extern int
bcmint_ltsw_port_fdr_callback_register(int unit, bcm_port_fdr_handler_t f, void * user_data);

/*!
* \brief Unregister port Flight Data Recorder (FDR) interrupt notification callbacks
*
* \param [in]   unit       Unit number.
* \param [in]   f          Flight Data Recorder (FDR) callback.
* \param [in]   user_data  user data pointer.
*
* \return SHR_E_NONE on success and error code otherwise.
*/
extern int
bcmint_ltsw_port_fdr_callback_unregister(int unit, bcm_port_fdr_handler_t f, void * user_data);

#endif /* BCMI_LTSW_PORT_INTERNAL_H */
