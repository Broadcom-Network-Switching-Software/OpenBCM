/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	stack_int.h
 */

#ifndef _BCM_INT_STACK_H_
#define _BCM_INT_STACK_H_

#include <bcm/stack.h>

/*
 * Stack tag structure
 */
typedef struct stk_tag_s {
#if defined(LE_HOST)
    uint32 rsvp:2;
    uint32 stk_modid:5;
    uint32 ed:1;
    uint32 em:1;
    uint32 md:1;
    uint32 mirr:1;
    uint32 pfm:2;
    uint32 dst_rtag:3;
    uint32 dst_tgid:3;
    uint32 dst_t:1;
    uint32 src_rtag:3;
    uint32 src_tgid:3;
    uint32 src_t:1;
    uint32 stk_cnt:5;
#else
    uint32 stk_cnt:5;
    uint32 src_t:1;
    uint32 src_tgid:3;
    uint32 src_rtag:3;
    uint32 dst_t:1;
    uint32 dst_tgid:3;
    uint32 dst_rtag:3;
    uint32 pfm:2;
    uint32 mirr:1;
    uint32 md:1;
    uint32 em:1;
    uint32 ed:1;
    uint32 stk_modid:5;
    uint32 rsvp:2;
#endif
} stk_tag_t;

#define STK_TAG_LEN 4

#define STACK_TAG(_scnt, _st, _stgid, _srtag, _dt, _dtgid, \
                 _drtag, _pfm, _m, _md, _em, _ed, _smod)  \
        ((_scnt & 0x1f) << 27 |\
        (_st & 0x1) << 26     |\
        (_stgid & 0x7) << 23  |\
        (_srtag & 0x7) << 20  |\
        (_dt & 0x1) << 19     |\
        (_dtgid & 0x7) << 16  |\
        (_drtag & 0x7) << 13  |\
        (_pfm & 0x3) << 11    |\
        (_m & 0x1) << 10      |\
        (_md & 0x1) << 9      |\
        (_em & 0x1) << 8      |\
        (_ed & 0x1) << 7      |\
        (_smod & 0x1f) << 2)

#define STK_MODID_GET(_tag) \
        ((_tag >> 2) & 0x1f)

#define STK_MODID_SET(_ptr_tag, _smod) \
        (*_ptr_tag = ((*_ptr_tag & ~(0x1f << 2)) | ((_smod & 0x1f) << 2)))

#define STK_CNT_GET(_tag) \
        ((_tag >> 27) & 0x1f)

#define STK_CNT_SET(_ptr_tag, _cnt) \
        (*_ptr_tag = ((*_ptr_tag & ~(0x1f << 27)) | ((_cnt & 0x1f) << 27)))

#define STK_PFM_GET(_tag) \
        ((_tag >> 11) & 0x3)

#define STK_PFM_SET(_ptr_tag, _pfm) \
        (*_ptr_tag = ((*_ptr_tag & ~(0x3 << 11)) | ((_pfm & 0x3) << 11)))

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
/* Saves the trunk id that the port belonged to
 * during a link flap.
 */
typedef struct {
    int trunk_id[SOC_MAX_NUM_PORTS];
} _bcm_stk_trident_linkflap_trunk_map_t;

extern
_bcm_stk_trident_linkflap_trunk_map_t _bcm_stk_trident_linkflap_trunk_map[];

#define _BCM_PORT_TRUNK_LINKFLAP_SET(_u, _p, _t) \
        _bcm_stk_trident_linkflap_trunk_map[_u].trunk_id[_p] = _t
#define _BCM_PORT_TRUNK_LINKFLAP_GET(_u, _p) \
        _bcm_stk_trident_linkflap_trunk_map[_u].trunk_id[_p]
#define _BCM_PORT_TRUNK_LINKFLAP_CLEAR(_u, _p) \
        _bcm_stk_trident_linkflap_trunk_map[_u].trunk_id[_p] = BCM_TRUNK_INVALID
#endif

enum {
    _BCM_STK_PORT_MODPORT_OP_SET = 1,
    _BCM_STK_PORT_MODPORT_OP_ADD,
    _BCM_STK_PORT_MODPORT_OP_DELETE,
    _BCM_STK_PORT_MODPORT_OP_COUNT
};

enum {
    _BCM_STK_PORT_MODPORT_DMVOQ_GRP_OP_SET = 1,
    _BCM_STK_PORT_MODPORT_DMVOQ_GRP_OP_GET,
    _BCM_STK_PORT_MODPORT_DMVOQ_GRP_OP_COUNT
};

extern int _bcm_esw_stk_detach(int unit);

typedef void (*_bcm_stk_modid_chg_cb_t)(
    int unit, 
    int  modid,
    void *userdata);

extern int _bcm_esw_stk_modid_chg_cb_register(int unit, 
                                              _bcm_stk_modid_chg_cb_t cb, 
                                              void *userdata);
extern int _bcm_esw_stk_modid_chg_cb_unregister(int unit, 
                                              _bcm_stk_modid_chg_cb_t cb); 
extern int _bcm_esw_stk_modmap_map(int unit, int setget,
                   bcm_module_t mod_in, bcm_port_t port_in,
                   bcm_module_t *mod_out, bcm_port_t *port_out);

#ifdef BCM_TOMAHAWK_SUPPORT
extern int _bcm_esw_stk_asymmetric_dual_modmap_map(int unit, int setget,
                   bcm_module_t *mod_in, bcm_port_t *port_in);
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
extern int _bcm_esw_my_modid_additional_check(int unit, bcm_module_t modid, int *result);
extern int _bcm_esw_stk_modid_range_info_get(int unit, int *mod_base, uint32 *mod_mask);
extern int _bcm_esw_stk_modid_range_set(int unit, int mod_base, uint32 mod_mask, uint32 valid);
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

extern int _bcm_esw_tr_trunk_override_ucast_set(int unit, bcm_port_t port,
                                      bcm_trunk_t hgtid, int modid, int enable);

extern int _bcm_esw_tr_trunk_override_ucast_get(int unit, bcm_port_t port,
                                     bcm_trunk_t hgtid, int modid, int *enable);
extern int _bcm_esw_src_modid_base_index_get(int unit, bcm_module_t modid,
                                     int is_local, int *base_index);
extern int _bcm_esw_src_modid_port_get(int unit, int stm_inx, bcm_module_t *modid,
                                     int *port);
extern int _bcm_esw_src_mod_port_table_index_get(int unit, bcm_module_t modid,
                                     bcm_port_t port, int *index);
extern int _bcm_esw_src_mod_port_table_index_get_from_port(int unit,
                                                           bcm_port_t port,
                                                           int *index);

extern int
bcm_stk_modport_voq_cosq_profile_get(int unit, bcm_port_t ing_port, 
                               bcm_module_t dest_modid, int *profile_id);

extern int
bcm_stk_modport_voq_cosq_profile_set(int unit, bcm_port_t ing_port, 
                               bcm_module_t dest_modid, int profile_id);

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX4_SUPPORT)
extern int 
_bcm_td_stk_modport_map_linkscan_handler(int unit, 
                bcm_port_t port, bcm_port_info_t *info);
extern int
_bcm_td_stk_modport_map_member_link_handler(int unit,
                                            bcm_gport_t port,
                                            bcm_trunk_t tid,
                                            int test_only,
                                            int *status);
#endif

extern int bcmi_esw_src_modid_base_index_bk_init(int unit);

#define BCM_MODID_TYPE_COE 0x1
#define BCM_MODID_TYPE_MULTI_NEXT_HOPS 0x2

extern int _bcm_stk_aux_modport_valid(int unit, uint32 type, 
                                      bcm_module_t modid, bcm_port_t port);

extern int _bcm_stk_aux_mod_valid(int unit, uint32 type, int modid);

extern int _bcm_stk_aux_modport_info_get(int unit, uint32 type, 
                                         bcm_stk_modid_config_t *modIdInfo);

#if defined(BCM_WARM_BOOT_SUPPORT) 
extern int bcm_esw_reload_stk_my_modid_get(int unit);
extern int _bcm_esw_stk_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_stk_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

typedef struct module_id_mask_s {
    uint32 mod_base; /* module base for extra modules */
    uint32 mod_mask; /* module mask for extra modules */
} module_id_mask_t;
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
/* In XGS3 devices prior to Trident, tables that contain per (modid, port)
 * information, such as the SOURCE_TRUNK_MAP table, contain
 * (SOC_MODID_MAX + 1) * (SOC_PORT_ADDR_MAX + 1) entries. The index to such
 * tables is formed by modid * (SOC_PORT_ADDR_MAX + 1) + port. In Trident,
 * the index is computed differently. First, the source modid is used to index
 * a table that gives a base index, such as the SOURCE_TRUNK_MAP_MODBASE table.
 * The base index is then added to port ID to derive the final index.
 */
typedef struct src_modid_base_index_bookkeeping_s {
    uint16 *num_ports; /* An array keeping track of number of ports per module */
    SHR_BITDCL *src_mod_port_table_bitmap;
                      /* Keeps track of which entries are used in tables that
                         are indexed by source modid base index and port */
    int *coe_module_id_list; /* Keeps track of modules allocated for CoE */
    module_id_mask_t module_id_range; /* Keeps track of modules of multiple next hops on port */
    uint32 default_base_idx; /* Default base index for non-configured MOD IDs */
    int *multi_nexthop_module_id_list; /* Keeps track of modules allocated for Multi Next Hops */
    int *module_id_type_list; /* Keeps track of modules allocated*/
} src_modid_base_index_bookkeeping_t;
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
extern int _bcm_esw_stk_get_mod_config_by_type(int unit, int flags, int mod_index,
                                               int *mod_id, int *valid, int *base_idx);
#endif

#endif	/* !_BCM_INT_STACK_H_ */
