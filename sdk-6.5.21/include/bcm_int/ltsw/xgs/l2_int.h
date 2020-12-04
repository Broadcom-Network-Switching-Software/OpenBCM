/*! \file l2_int.h
 *
 * Parse or populate L2 LT fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_XGS_L2_INT_H
#define BCMINT_LTSW_XGS_L2_INT_H

#include <bcm/l2.h>
#include <bcm/types.h>

#include <bcm_int/ltsw/l2_int.h>

#include <sal/sal_types.h>

/******************************************************************************
 * Local definitions
 */

/* Max number of assist or slave fields. */
#define NUM_COMB_FLDS    4

/* Invalid offset of member variable. */
#define INVALID_MEMBER_OFFSET    ((uint64_t)-1)

/* Invalid ID of field. */
#define INVALID_LT_FID           ((uint32_t)-1)

/* Invalid value of field. */
#define INVALID_FLD_VAL          ((uint64_t)-1)

/* Invalid value of flag. */
#define INVALID_FLG_VAL          ((uint32_t)-1)


/* Populate value of structure member variables to LT fields. */
#define DIR_POPULATE   0

/* Parse data from LT fields to structure member variables. */
#define DIR_PARSE      1

/* Common flags */
/* Key field */
#define SELECT_FLAG_KEY    (1 << 0)

/* Data field */
#define SELECT_FLAG_DATA   (1 << 1)

/* For L2_HOST_TAB */
/* Age fields */
#define SELECT_FLAG_L2_HOST_TAB_AGE          (1 << 2)

/* Only access data from CLASS_ID field */
#define SELECT_FLAG_L2_HOST_TAB_CLASS_ID     (1 << 3)

/* Only access data from HIT fields */
#define SELECT_FLAG_L2_HOST_TAB_REPL_HIT     (1 << 4)

/* Resolve data from MODPORT field again */
#define SELECT_FLAG_L2_HOST_TAB_REPL_MODPORT (1 << 5)

/* The field is used only during population operation */
#define SELECT_FLAG_ONLY_POPULATE_USED (1 << 6)

/* Assign/get data to/from member variable of a structure. */
#define MEMBER_VALUE_ASSIGN(_dir_to_mem, _op, _addr, _type, _mem_val) \
    if (_dir_to_mem) { \
        if (_op == '=') { \
            *((_type *)(*(_addr))) = (_type)(_mem_val); \
        } else if (_op == '|') { \
            *((_type *)(*(_addr))) |= (_type)(_mem_val); \
        } else if (_op == '&') { \
            *((_type *)(*(_addr))) &= (_type)(_mem_val); \
        }\
    } else { \
        _mem_val = *((_type *)(*(_addr))); \
    }

/* Data structure is bound to L2_LEARN_CONTROLt. */
typedef struct ltsw_l2_data_control_s {
    /*L2_LEARN_CONTROLt_REPORTf. */
    bool report;

    /* L2_LEARN_CONTROLt_SLOW_POLLf. */
    bool slow_poll;

} ltsw_l2_data_control_t;

/* Data structure is bound to L2_DST_BLOCKt. */
typedef struct ltsw_l2_dst_blk_s {
    /* Value written/read to/from L2_DST_BLOCKt_L2_DST_BLOCK_IDf. */
    int index;

    /* Value written/read to/from L2_DST_BLOCKt_DST_BLOCK_MASKf. */
    bcm_pbmp_t profile;

} ltsw_l2_dst_blk_t;

/* L2 Learn notify info. It is bound to L2_LEARN_DATAt */
typedef struct l2_learn_notify_s {
    /* Opcode */
    bcmlt_opcode_t op;

    /* Start time in us, for debugging usage. */
    sal_usecs_t stime;

    /* Vlan ID. */
    bcm_vlan_t vid;

    /* MAC. */
    uint64_t mac;

    /* Source type. */
    int src_type;

    /* Source trunk group ID */
    bcm_trunk_t tgid;

    /* Source gport ID. */
    int modport;

    /* Virtual port. */
    int svp;

    /* Key, Pipe */
    int pipe;

    /* Key, Data ID */
    int data_id;
} xgs_l2_learn_notify_t;

/* Head information will be used in l2_lt_entry_set_cb or l2_lt_entry_get_cb */
typedef struct ltsw_lt_head_info_s {
    /* Transfer data of fields to value of members of structure. Vice versa. */
    const bcmint_l2_map_info_t *lt_info;

    /* Filter LT fields during l2_lt_entry_set_cb */
    uint32_t filter_set;

    /* Filter LT fields during l2_lt_entry_get_cb */
    uint32_t filter_get;

} ltsw_l2_lt_head_info_t;

/* It is bound to L2_OPAQUE_TAGt */
typedef struct l2_opaque_tag_s {
    /* Head */
    ltsw_l2_lt_head_info_t *lt_head_info;

    /* Value of L2_OPAQUE_TAGt_L2_OPAQUE_TAG_IDf. */
    int opaque_tag_id;

    /* Value of L2_OPAQUE_TAGt_TAG_SIZEf. */
    int tag_size;

    /* Value of L2_OPAQUE_TAGt_ETHERTYPEf. */
    int ether_type;

    /* Value of L2_OPAQUE_TAGt_TAG_TYPEf. */
    int tag_type;

} l2_opaque_tag_t;

/* Encap head info */
typedef struct ltsw_l2_lt_head_info_encap_s {
    /* Head */
    ltsw_l2_lt_head_info_t *lt_head_info;

} ltsw_l2_lt_head_info_encap_t;

/* It is bound to L2_LEARN_OVERRIDEt */
typedef struct l2_learn_override_s {
    /* Head */
    ltsw_l2_lt_head_info_t *lt_head_info;

    /* Value of L2_LEARN_OVERRIDEt_MAC_MOVE_OVERRIDEf */
    int mac_move_override;

    /* Value of L2_LEARN_OVERRIDEt_MAC_LEARN_OVERRIDEf */
    int mac_learn_override;

} l2_learn_override_t;

/* Get internal offset of a member variable */
#define OFFSET_OF(type, member) ((unsigned long)(&((type *)0)->member))

/* Get internal offset of a member variable of bcm_l2_addr_t */
#define L2_ADDR_MEMB(member) OFFSET_OF(bcm_l2_addr_t, member)

/* Get internal offset of a member variable of bcm_l2_station_t */
#define L2_STATION_MEMB(member) OFFSET_OF(bcm_l2_station_t, member)

/* Get internal offset of a member variable of bcm_l2_cache_addr_t */
#define L2_CACHE_MEMB(member) OFFSET_OF(bcm_l2_cache_addr_t, member)

/* Get internal offset of a member variable of ltsw_l2_dst_blk_t */
#define L2_DST_BLK(member) OFFSET_OF(ltsw_l2_dst_blk_t, member)

/* Get internal offset of a member variable of xgs_l2_learn_notify_t */
#define L2_LEARN_NOTIFY(member) OFFSET_OF(xgs_l2_learn_notify_t, member)

/* Get internal offset of a member variable of ltsw_l2_data_control_t */
#define L2_LEARN_CTRL(member) OFFSET_OF(ltsw_l2_data_control_t, member)

/* Get internal offset of a member variable of l2_opaque_tag_t */
#define L2_OPAQUE_TAG(member) OFFSET_OF(l2_opaque_tag_t, member)

/* Get internal offset of a member variable of l2_learn_override_t */
#define L2_LEARN_OVERRIDE(member) OFFSET_OF(l2_learn_override_t, member)

/*!
 * \brief Transfer callback routine for L2_DST_BLOCKt_DST_BLOCK_MASKf
 * and ltsw_l2_dst_blk_t.profile
 *
 * \param [in] unit Unit number
 * \param [in/out] param Parameters which not only can be used to decode data,
 * but also can be used to save data.
 * \param [in] dir DIR_POPULATE: get data from ltsw_l2_dst_blk_t.profile,
 * then decode data to L2_DST_BLOCKt_DST_BLOCK_MASKf.
 * DIR_PARSE: get data from L2_DST_BLOCKt_DST_BLOCK_MASKf, then decode data to
 * ltsw_l2_dst_blk_t.profile.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l2_dst_blk_profile_trans(int unit, bcmint_l2_trans_data_t *param, int dir);

/*!
 * \brief Get destination type according to user data.
 *
 * \param [in] unit Unit number
 * \param [in/out] param Parameters which not only can be used to decode data,
 * but also can be used to save data.
 * \param [in] dir DIR_POPULATE: get destination type. DIR_PARSE: Unavailable.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l2_dest_type_trans(int unit, bcmint_l2_trans_data_t *param, int dir);

/*!
 * \brief Transfer tgid from data structure to field value. Or transfer field value to tgid or port.
 *
 * \param [in] unit Unit number
 * \param [in/out] param Parameters which not only can be used to decode data,
 * but also can be used to save data.
 * \param [in] dir DIR_POPULATE: set tgid to LT field. DIR_PARSE: Parse tgid from LT field.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l2_tgid_trans(int unit,  bcmint_l2_trans_data_t *param, int dir);

/*!
 * \brief Transfer l2mc_group from data structure to field value. Or transfer field value to l2mc_group.
 *
 * \param [in] unit Unit number
 * \param [in/out] param Parameters which not only can be used to decode data,
 * but also can be used to save data.
 * \param [in] dir DIR_POPULATE: set l2mc_group to LT field. DIR_PARSE: Parse l2mc_group from LT field.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l2_mc_grp_trans(int unit, bcmint_l2_trans_data_t *param, int dir);

/*!
 * \brief Transfer l3mc_group from data structure to field value. Or transfer field value to l3mc_group.
 *
 * \param [in] unit Unit number
 * \param [in/out] param Parameters which not only can be used to decode data,
 * but also can be used to save data.
 * \param [in] dir DIR_POPULATE: set l3mc_group to LT field. DIR_PARSE: Parse l3mc_group from LT field.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l3_mc_grp_trans(int unit, bcmint_l2_trans_data_t *param, int dir);

/*!
 * \brief Transfer port from data structure to field value. Or transfer field value to port.
 *
 * \param [in] unit Unit number
 * \param [in/out] param Parameters which not only can be used to decode data,
 * but also can be used to save data.
 * \param [in] dir DIR_POPULATE: set port to LT field. DIR_PARSE: Parse port from LT field.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l2_dest_port_trans(int unit, bcmint_l2_trans_data_t *param, int dir);

/*!
 * \brief Transfer Class Id from data structure to field value. Or transfer field value to Class ID.
 *
 * \param [in] unit Unit number
 * \param [in/out] param Parameters which not only can be used to decode data,
 * but also can be used to save data.
 * \param [in] dir DIR_POPULATE: set Class ID to LT field. DIR_PARSE: Parse Class ID from LT field.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l2_class_id_trans(int unit, bcmint_l2_trans_data_t *param, int dir);

/*!
 * \brief Transfer field value from vpn to vfi. Or transfer field value from vfi to vpn.
 *
 * \param [in] unit Unit number
 * \param [in/out] param Parameters which not only can be used to decode data,
 * but also can be used to save data.
 * \param [in] dir DIR_POPULATE: vpn->vfi. DIR_PARSE: vfi->vpn.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l2_vfi_trans(int unit, bcmint_l2_trans_data_t *param, int dir);

/*!
 * \brief Transfer VP from data structure to field value. Or transfer field value to VP.
 *
 * \param [in] unit Unit number
 * \param [in/out] param Parameters which not only can be used to decode data,
 * but also can be used to save data.
 * \param [in] dir DIR_POPULATE: set VP to LT field. DIR_PARSE: Parse VP from LT field.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l2_dvp_trans(int unit, bcmint_l2_trans_data_t *param, int dir);

/*!
 * \brief During L2 initialization, need to call this routine to register L2 ID map info.
 *
 * \param [in] unit Unit number
 * \param [in] lt_map_info LT ID map information.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
xgs_ltsw_l2_lt_map_register(int unit, const bcmint_l2_map_info_t *lt_map_info);

#endif /* BCMINT_LTSW_XGS_L2_INT_H */
