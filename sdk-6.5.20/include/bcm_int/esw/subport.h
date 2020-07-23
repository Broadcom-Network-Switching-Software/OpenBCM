/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains subport definitions internal to the BCM library.
 */

#include <sal/core/sync.h>
#include <bcm/subport.h>
#include <bcm/error.h>
#include <soc/scache.h>
#include <bcm_int/esw/flex_ctr.h>

#ifndef _BCM_INT_ESW_SUBPORT_H_
#define _BCM_INT_ESW_SUBPORT_H_

/** Raghu Start change */

/*****************************************
* Subport CoE common section - start
 */

/* The common data structures between KT2 and TD2+ Subport-CoE 
   implementation */

/* SUBPORT GPORT CoE subtype shift & mask definitions */

/* Subport CoE Common defines */

#define _BCM_COE_PORT_TYPE_ETHERNET         0
#define _BCM_COE_PORT_TYPE_CASCADED         4
#define _BCM_COE_PORT_TYPE_CASCADED_LINKPHY 0
#define _BCM_COE_PORT_TYPE_CASCADED_SUBTAG  1

/* Subport group type defs */
#define _BCM_SUBPORT_COE_TYPE_ETHERNET              0
#define _BCM_SUBPORT_COE_TYPE_CASCADED              1

/* Subport type defs */
#define _BCM_SUBPORT_COE_TYPE_LINKPHY               2
#define _BCM_SUBPORT_COE_TYPE_SUBTAG                3
#define _BCM_SUBPORT_COE_TYPE_GENERAL               1

/*
* LinkPHY/SubTag Subport group gport format 
* bit 31-26 -- gport type (subport group)
* bit 25-24 -- subport group type (cascaded)
* bit 19-18 -- subport subtype (linkphy / subtag / general)
* bit 17    -- subport group is trunk indicator
* bit 16-9  -- subport group port number/trunk group id
* bit 8-0   -- subport group index
*/
#define _BCM_SUBPORT_COE_GROUP_TYPE_MASK      0x3
#define _BCM_SUBPORT_COE_GROUP_TYPE_SHIFT     24
#define _BCM_SUBPORT_COE_GROUP_SUBTYPE_MASK   0x3
#define _BCM_SUBPORT_COE_GROUP_SUBTYPE_SHIFT  18
#define _BCM_SUBPORT_COE_GROUP_PORT_MASK      0xFF
#define _BCM_SUBPORT_COE_GROUP_PORT_SHIFT     9
#define _BCM_SUBPORT_COE_GROUP_SPGID_MASK     0x1FF
#define _BCM_SUBPORT_COE_GROUP_SPGID_SHIFT    0

/*
* LinkPHY/SubTag Subport port gport format 
* bit 31-26  -- gport type (subport port)
* bit 25-24  -- subport port type (LinkPHY / SubTag/ General)
* bit 23-15  -- zero
* bit 14-07  -- subport module index(range 0 to 255) 
* bit 06-00  -- subport port index  (range 0 to 127) 
*/

#define _BCM_SUBPORT_COE_PORT_TYPE_MASK         0x3
#define _BCM_SUBPORT_COE_PORT_TYPE_SHIFT        24
#define _BCM_SUBPORT_COE_PORT_ZERO_BITS_MASK    0x1FF
#define _BCM_SUBPORT_COE_PORT_ZERO_BITS_SHIFT   15
#define _BCM_SUBPORT_COE_PORT_MODULE_MASK       0xFF
#define _BCM_SUBPORT_COE_PORT_MODULE_SHIFT      7
#define _BCM_SUBPORT_COE_PORT_PORT_MASK         0x7F
#define _BCM_SUBPORT_COE_PORT_PORT_SHIFT        0

#define _BCM_SUBPORT_COE_PORT_VALUE_MASK        0x7FFF

#define BCM_SUBPORT_COE_DEFAULT_ETHERTYPE       0x8874

/* Check if gport is LinkPHY subport_group */
#define _BCM_COE_GPORT_IS_LINKPHY_SUBPORT_GROUP(_subport_group)    \
        ((((_subport_group >> _BCM_SUBPORT_COE_GROUP_SUBTYPE_SHIFT) & \
            _BCM_SUBPORT_COE_GROUP_SUBTYPE_MASK) ==    \
            _BCM_SUBPORT_COE_TYPE_LINKPHY) && \
         (((_subport_group >> _BCM_SUBPORT_COE_GROUP_TYPE_SHIFT) & \
            _BCM_SUBPORT_COE_GROUP_TYPE_MASK) ==    \
            _BCM_SUBPORT_COE_TYPE_CASCADED))


/* Check if gport is SubTag subport_group */
#define _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(_subport_group)    \
        ((((_subport_group >> _BCM_SUBPORT_COE_GROUP_SUBTYPE_SHIFT) & \
            _BCM_SUBPORT_COE_GROUP_SUBTYPE_MASK) ==    \
            _BCM_SUBPORT_COE_TYPE_SUBTAG) && \
         (((_subport_group >> _BCM_SUBPORT_COE_GROUP_TYPE_SHIFT) & \
            _BCM_SUBPORT_COE_GROUP_TYPE_MASK) ==    \
            _BCM_SUBPORT_COE_TYPE_CASCADED))


/* Check if gport is general subport_group */
#define _BCM_COE_GPORT_IS_GENERAL_SUBPORT_GROUP(_subport_group)    \
    ((((_subport_group >> _BCM_SUBPORT_COE_GROUP_SUBTYPE_SHIFT) & \
       _BCM_SUBPORT_COE_GROUP_SUBTYPE_MASK) ==    \
      _BCM_SUBPORT_COE_TYPE_GENERAL) && \
     (((_subport_group >> _BCM_SUBPORT_COE_GROUP_TYPE_SHIFT) & \
       _BCM_SUBPORT_COE_GROUP_TYPE_MASK) ==    \
      _BCM_SUBPORT_COE_TYPE_CASCADED))

/* Set the subport port gport  type */
#define _BCM_SUBPORT_COE_PORT_TYPE_SET(_subport_port, _type)  \
    (_subport_port |= ((_type & _BCM_SUBPORT_COE_PORT_TYPE_MASK) \
          << _BCM_SUBPORT_COE_PORT_TYPE_SHIFT))


/* Get port number/trunk id associated with subport_group gport */
#define _BCM_SUBPORT_COE_GROUP_PORT_GET(_subport_group) \
        ((_subport_group >> _BCM_SUBPORT_COE_GROUP_PORT_SHIFT) & \
            _BCM_SUBPORT_COE_GROUP_PORT_MASK)

/* Get group id associated with subport_group gport */
#define _BCM_SUBPORT_COE_GROUP_ID_GET(_subport_group) \
        (((_subport_group) >> _BCM_SUBPORT_COE_GROUP_SPGID_SHIFT) & \
            _BCM_SUBPORT_COE_GROUP_SPGID_MASK)

#define _BCM_SUBPORT_COE_GROUP_TYPE_CASCADED_SET(_subport_group) \
    ((_subport_group) |= ((_BCM_SUBPORT_COE_TYPE_CASCADED & \
                          _BCM_SUBPORT_COE_GROUP_TYPE_MASK) \
                              << _BCM_SUBPORT_COE_GROUP_TYPE_SHIFT))
 
/*
 * Subtended port Function Vector Driver
 */
typedef struct bcm_esw_subport_drv_s {
    /*
     * Subport APIs
     */

    /* Initial subport module handlers */

    /* Initalize enhanced subport module */
    int (*subport_init)(int unit);
    /* Create Subport group */
    int (*subport_group_create)(int unit, bcm_subport_group_config_t *config,
             bcm_gport_t *group);
    /* Delete Subport group */
    int (*subport_group_get)(int unit, bcm_gport_t group,
             bcm_subport_group_config_t *config);
    /* Traverse Subport group */
    int (*subport_group_traverse)(int unit, bcm_gport_t subport_group,
                          bcm_subport_port_traverse_cb cb, void *user_data);
    /* Destroy Subport group */
    int (*subport_group_destroy)(int unit, bcm_gport_t group);
    /* Set LinkPhy config */
    int (*subport_linkphy_config_set)(int unit, bcm_gport_t port,
           bcm_subport_group_linkphy_config_t *linkphy_config);
    /* Get LinkPhy config */
    int (*subport_linkphy_config_get)(int unit, bcm_gport_t port,
           bcm_subport_group_linkphy_config_t *linkphy_config);   

    /* Subport port add */
    int (*subport_port_add)(int unit, bcm_subport_config_t *config, 
                            bcm_gport_t *port); 
    /* Subport port get */
    int (*subport_port_get)(int unit, bcm_gport_t port, 
                            bcm_subport_config_t *config);
    /* Subport port traverse */
    int (*subport_port_traverse)(int unit, 
                                 bcm_subport_port_traverse_cb cb, 
                                 void *user_data);
    /* Subport port stat set */
    int (*subport_port_stat_set)(int unit, bcm_gport_t port, 
                                 int stream_id,
                                 bcm_subport_stat_t stat_type, uint64 val);
    /* Subport port stat get */
    int (*subport_port_stat_get)(int unit, bcm_gport_t port, int stream_id,
                                 bcm_subport_stat_t stat_type, uint64 *val);
    /* Subport port delete */
    int (*subport_port_delete)(int unit, bcm_gport_t port);
    /* Subport module cleanup */
    int (*subport_cleanup)(int unit);

    /* CoE module handlers */

    /* Init CoE subport module handlers */
    int (*coe_init)(int unit);
    /* Create Subport group */
    int (*coe_group_create)(int unit, bcm_subport_group_config_t *config,
			 bcm_gport_t *group);
    /* Delete Subport group */
    int (*coe_group_get)(int unit, bcm_gport_t group,
             bcm_subport_group_config_t *config);
    /* Traverse Subport group */
    int (*coe_group_traverse)(int unit, bcm_gport_t subport_group,
                              bcm_subport_port_traverse_cb cb, 
                              void *user_data);
    /* Destroy Subport group */
    int (*coe_group_destroy)(int unit, bcm_gport_t group);
    /* Set LinkPhy config */
    int (*coe_group_linkphy_config_set)(int unit, bcm_gport_t port,
          bcm_subport_group_linkphy_config_t *linkphy_config);
    /* Get LinkPhy config */
    int (*coe_group_linkphy_config_get)(int unit, bcm_gport_t port,
         bcm_subport_group_linkphy_config_t *linkphy_config); 

    /* Subport port add */
    int (*coe_port_add)(int unit, bcm_subport_config_t *config, 
                        bcm_gport_t *port);
    /* Subport port get */
    int (*coe_port_get)(int unit, bcm_gport_t port, 
                        bcm_subport_config_t *config);
    /* Subport port traverse */
    int (*coe_port_traverse)(int unit, bcm_subport_port_traverse_cb cb, 
                             void *user_data);
    /* Subport port stat set */
    int (*coe_port_stat_set)(int unit, bcm_gport_t port, int stream_id,
                         bcm_subport_stat_t stat_type, uint64 val);
    /* Subport port stat get */
    int (*coe_port_stat_get)(int unit, bcm_gport_t port, int stream_id,
                         bcm_subport_stat_t stat_type, uint64 *val);
    /* Subport port delete */
    int (*coe_port_delete)(int unit, bcm_gport_t port);
    /* Subport module cleanup */
    int (*coe_cleanup)(int unit);
    /* subport_linkphy_rx_error_register */
    int (*subport_linkphy_rx_error_register) (int unit,
            bcm_subport_linkphy_rx_errors_t errors,
            bcm_subport_linkphy_rx_error_reg_info_t *reg_info,
            void *user_data);
    /* subport_linkphy_rx_error_unregister */
    int (*subport_linkphy_rx_error_unregister) (int unit,
            bcm_subport_linkphy_rx_errors_t errors);
    /* subport_gport mod-port get */
    int (*subport_gport_modport_get) (int unit,
            bcm_gport_t subport_gport, bcm_module_t *module,
           bcm_port_t *port);
 
} bcm_esw_subport_drv_t;

extern bcm_esw_subport_drv_t      *bcm_esw_subport_drv[BCM_MAX_NUM_UNITS];

#define BCM_ESW_SUBPORT_DRV(_u)   (bcm_esw_subport_drv[(_u)])

extern int bcmi_subport_common_init(int unit, bcm_esw_subport_drv_t *drv);

/*
 * Bitmap of subport group id
 * Each Subport group created has unique group id
 * across all ports in the device irrespective of
 * group type LinkPHY/SubTag
 */
extern SHR_BITDCL *_bcm_subport_group_bitmap[BCM_MAX_NUM_UNITS];

/* Count of all subport groups created */
extern int _bcm_subport_group_count[BCM_MAX_NUM_UNITS];

/* Count subport port per subport group*/
extern int *(_bcm_subport_group_subport_port_count[BCM_MAX_NUM_UNITS]);


/*********************************
* Subport common  section - end
 */

/*****************************************
* General purpose subport section - start
 */

/* Bitmap of general subport group id */
extern SHR_BITDCL *_bcm_general_subport_group_bitmap[BCM_MAX_NUM_UNITS];

/* Count of all general subport groups created */
extern int _bcm_general_subport_group_count[BCM_MAX_NUM_UNITS];

/* general subport port info structure */
typedef struct {
    /* Subport group gort attached to */
    bcm_gport_t  group;
    int          valid;
} _bcm_general_subport_port_info_t;

extern _bcm_general_subport_port_info_t
        *(_bcm_general_subport_port_info[BCM_MAX_NUM_UNITS]);

/*****************************************
* General purpose subport section - end
 */

/*********************************
* SubTag subport section - start
 */

/* Bitmap of SubTag subport group id */
extern SHR_BITDCL *_bcm_subtag_group_bitmap[BCM_MAX_NUM_UNITS];

/* Count of all SubTag subport groups created */
extern int _bcm_subtag_subport_group_count[BCM_MAX_NUM_UNITS];

extern SHR_BITDCL *_bcm_subtag_vlan_id_bitmap
                   [BCM_MAX_NUM_UNITS][SOC_MAX_NUM_PORTS];

extern SHR_BITDCL *_bcm_subport_port_group_bitmap[BCM_MAX_NUM_UNITS];

/* SubTag subport port info structure */
typedef struct {
    /* Subport group gort attached to */
    bcm_gport_t  group;
    /* VLAN ID */
    bcm_vlan_t   vlan;
    int          valid;
    int          subtag_tcam_hw_idx;
    int          modport_subport_idx;
    /* Subport-Port, represented as GPORT_SUBPORT */
    bcm_gport_t  subport_port;
    /* Associated subport, represented as GPORT_MODPORT */
    bcm_gport_t  subport_modport;
    int          phb_valid;
    int          priority;
    int          color;
    int          nh_index;
} _bcm_subtag_subport_port_info_t;

extern _bcm_subtag_subport_port_info_t
           *(_bcm_subtag_subport_port_info[BCM_MAX_NUM_UNITS]);

/*********************************
* SubTag subport section - end
 */


/****  Misc section start */

extern sal_mutex_t _bcm_subport_mutex[BCM_MAX_NUM_UNITS];

#define _BCM_SUBPORT_COE_LOCK(unit) \
    sal_mutex_take(_bcm_subport_mutex[unit], sal_mutex_FOREVER);

#define _BCM_SUBPORT_COE_UNLOCK(unit) \
    sal_mutex_give(_bcm_subport_mutex[unit]);


#define _BCM_SUBPORT_COE_CHECK_INIT(_unit_) \
        if (!_bcm_subport_group_bitmap[_unit_]) \
            return BCM_E_INIT

/****  Misc section end */


/** Common defines for CoE implementation - start */

/*
 * Subport group usage bitmap operations
 */
#define _BCM_SUBPORT_COE_GROUP_USED_GET(_u_, _group_) \
                SHR_BITGET(_bcm_subport_group_bitmap[_u_], (_group_))

#define _BCM_SUBPORT_COE_GROUP_USED_SET(_u_, _group_) \
                SHR_BITSET(_bcm_subport_group_bitmap[_u_], (_group_))

#define _BCM_SUBPORT_COE_GROUP_USED_CLR(_u_, _group_) \
                SHR_BITCLR(_bcm_subport_group_bitmap[_u_], (_group_))

/*
 * General subport group usage bitmap operations
 */
#define _BCM_GENERAL_SUBPORT_GROUP_USED_GET(_u_, _group_) \
    SHR_BITGET(_bcm_general_subport_group_bitmap[_u_], (_group_))

#define _BCM_GENERAL_SUBPORT_GROUP_USED_SET(_u_, _group_) \
    SHR_BITSET(_bcm_general_subport_group_bitmap[_u_], (_group_))

#define _BCM_GENERAL_SUBPORT_GROUP_USED_CLR(_u_, _group_) \
    SHR_BITCLR(_bcm_general_subport_group_bitmap[_u_], (_group_))

/*
 * SubTag subport group usage bitmap operations
 */
#define _BCM_SUBPORT_COE_SUBTAG_GROUP_USED_GET(_u_, _group_) \
            SHR_BITGET(_bcm_subtag_group_bitmap[_u_], (_group_))

#define _BCM_SUBPORT_COE_SUBTAG_GROUP_USED_SET(_u_, _group_) \
            SHR_BITSET(_bcm_subtag_group_bitmap[_u_], (_group_))

#define _BCM_SUBPORT_COE_SUBTAG_GROUP_USED_CLR(_u_, _group_) \
            SHR_BITCLR(_bcm_subtag_group_bitmap[_u_], (_group_))

/* Katana2 SubTag VLAN id usage bitmap for given port*/
#define _BCM_COE_SUBTAG_VLAN_ID_USED_GET(_u_, _p, _vlan_id_) \
            SHR_BITGET(_bcm_subtag_vlan_id_bitmap[_u_][_p], (_vlan_id_))

#define _BCM_COE_SUBTAG_VLAN_ID_USED_SET(_u_, _p, _vlan_id_) \
            SHR_BITSET(_bcm_subtag_vlan_id_bitmap[_u_][_p], (_vlan_id_))

#define _BCM_COE_SUBTAG_VLAN_ID_USED_CLR(_u_, _p, _vlan_id_) \
            SHR_BITCLR(_bcm_subtag_vlan_id_bitmap[_u_][_p], (_vlan_id_))


/* Set the SubTag subport group gport */

/* The encoding is like this, based on the flags used:
    GPORT-TYPE-SUBPORT, SUBTYPE-SUBPORT, PHYSICAL-PORT, GroupId
 */

#define _BCM_SUBPORT_COE_SUBTAG_GROUP_SET(_subport_group, \
                                          _port, _gid) \
            (BCM_GPORT_SUBPORT_GROUP_SET((_subport_group), \
                        (((_BCM_SUBPORT_COE_TYPE_SUBTAG & \
                          _BCM_SUBPORT_COE_GROUP_SUBTYPE_MASK) \
                              << _BCM_SUBPORT_COE_GROUP_SUBTYPE_SHIFT) | \
                         ((_port & _BCM_SUBPORT_COE_GROUP_PORT_MASK) \
                              << _BCM_SUBPORT_COE_GROUP_PORT_SHIFT) | \
                         ((_gid & _BCM_SUBPORT_COE_GROUP_SPGID_MASK) \
                              << _BCM_SUBPORT_COE_GROUP_SPGID_SHIFT))))

/* Set the LinkPHY subport group gport */

/* The encoding is like this, based on the flags used:
    GPORT-TYPE-SUBPORT, SUBTYPE-LINKPHY, PHYSICAL-PORT, GroupId
 */
#define _BCM_COE_LINKPHY_SUBPORT_GROUP_SET(_subport_group, \
                                           _port, _gid) \
    (BCM_GPORT_SUBPORT_GROUP_SET((_subport_group), \
                        (((_BCM_SUBPORT_COE_TYPE_LINKPHY & \
                          _BCM_SUBPORT_COE_GROUP_SUBTYPE_MASK) \
                              << _BCM_SUBPORT_COE_GROUP_SUBTYPE_SHIFT) | \
                         ((_port & _BCM_SUBPORT_COE_GROUP_PORT_MASK) \
                              << _BCM_SUBPORT_COE_GROUP_PORT_SHIFT) | \
                         ((_gid & _BCM_SUBPORT_COE_GROUP_SPGID_MASK) \
                              << _BCM_SUBPORT_COE_GROUP_SPGID_SHIFT))))

/* Set the general group gport */

/* The encoding is like this, based on the flags used:
   GPORT-TYPE-SUBPORT, SUBTYPE-GENERAL, PHYSICAL-PORT, GroupId
 */
#define _BCM_GENERAL_SUBPORT_GROUP_SET(_subport_group, \
                                       _port, _gid) \
    (BCM_GPORT_SUBPORT_GROUP_SET((_subport_group), \
                             (((_BCM_SUBPORT_COE_TYPE_GENERAL & \
                                _BCM_SUBPORT_COE_GROUP_SUBTYPE_MASK) \
                               << _BCM_SUBPORT_COE_GROUP_SUBTYPE_SHIFT) | \
                              ((_port & _BCM_SUBPORT_COE_GROUP_PORT_MASK) \
                               << _BCM_SUBPORT_COE_GROUP_PORT_SHIFT) | \
                              ((_gid & _BCM_SUBPORT_COE_GROUP_SPGID_MASK) \
                               << _BCM_SUBPORT_COE_GROUP_SPGID_SHIFT))))

/** Common defines for CoE implementation - end */


#ifdef BCM_HGPROXY_COE_SUPPORT

#define _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_VALID_GET(unit, tcam_hw_id) \
    (_bcm_subtag_subport_port_info[unit][tcam_hw_id].valid)

#define _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_GROUP_GET(unit, tcam_hw_id) \
    (_bcm_subtag_subport_port_info[unit][tcam_hw_id].group)

#define _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_VLAN_GET(unit, tcam_hw_id) \
    (_bcm_subtag_subport_port_info[unit][tcam_hw_id].vlan)

#define _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_TCAM_HW_ID_GET(unit, tcam_hw_id) \
    (_bcm_subtag_subport_port_info[unit][tcam_hw_id].subtag_tcam_hw_idx)

#define _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_SUBPORT_MODPORT_GET(unit, tcam_hw_id) \
    (_bcm_subtag_subport_port_info[unit][tcam_hw_id].subport_modport)

#define _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_MODPORT_IDX_GET(unit, tcam_hw_id) \
    (_bcm_subtag_subport_port_info[unit][tcam_hw_id].modport_subport_idx)

#define _BCM_SUBPORT_COE_SUBTAG_PORT_INFO_SET(unit, tcam_hw_id, tcam_hw_mem_idx, \
        sp_group, vlan_id, subport_port_val, subport_modport_val, phbValid, \
        pri, col, is_valid, modport_subport_idx_val) \
{ \
    _bcm_subtag_subport_port_info[unit][tcam_hw_id].group = sp_group; \
    _bcm_subtag_subport_port_info[unit][tcam_hw_id].vlan = vlan_id; \
    _bcm_subtag_subport_port_info[unit] \
                                 [tcam_hw_id].subtag_tcam_hw_idx = \
                                 tcam_hw_mem_idx; \
    _bcm_subtag_subport_port_info[unit][tcam_hw_id].subport_port = \
                                subport_port_val; \
    _bcm_subtag_subport_port_info[unit][tcam_hw_id].subport_modport = \
                                subport_modport_val; \
    _bcm_subtag_subport_port_info[unit][tcam_hw_id].phb_valid = phbValid; \
    _bcm_subtag_subport_port_info[unit][tcam_hw_id].priority = pri; \
    _bcm_subtag_subport_port_info[unit][tcam_hw_id].color = col; \
    _bcm_subtag_subport_port_info[unit][tcam_hw_id].valid = is_valid; \
    _bcm_subtag_subport_port_info[unit][tcam_hw_id].modport_subport_idx = \
                                modport_subport_idx_val; \
}

/* Set the subport port gport mod,port id */
#define _BCM_SUBPORT_COE_PORT_ID_SET(_subport_port, mod, port)  \
    (BCM_GPORT_SUBPORT_PORT_SET((_subport_port), \
        ((mod & _BCM_SUBPORT_COE_PORT_MODULE_MASK) \
             << _BCM_SUBPORT_COE_PORT_MODULE_SHIFT) | \
        ((port & _BCM_SUBPORT_COE_PORT_PORT_MASK) \
             << _BCM_SUBPORT_COE_PORT_PORT_SHIFT)))

/* Get the subport port gport mod id */
#define _BCM_SUBPORT_COE_PORT_ID_MOD_GET(_subport_port)  \
        ((_subport_port >> _BCM_SUBPORT_COE_PORT_MODULE_SHIFT) & \
                 _BCM_SUBPORT_COE_PORT_MODULE_MASK)

/* Get the subport port gport port id */
#define _BCM_SUBPORT_COE_PORT_ID_PORT_GET(_subport_port)  \
        ((_subport_port >> _BCM_SUBPORT_COE_PORT_PORT_SHIFT) & \
                 _BCM_SUBPORT_COE_PORT_PORT_MASK)   

/* Get the subport port gport mod id */
#define _BCM_SUBPORT_COE_PORT_ID_GET(_subport_port)  \
        ((_subport_port & (_BCM_SUBPORT_COE_PORT_VALUE_MASK)))

/* Check if gport is SubTag subport_port */
#define _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(_unit, _subport_port)    \
        (((((_subport_port) >> _BCM_SUBPORT_COE_PORT_TYPE_SHIFT) & \
                        _BCM_SUBPORT_COE_PORT_TYPE_MASK) == \
                        _BCM_SUBPORT_COE_TYPE_SUBTAG) && \
         ((((_subport_port) >> _BCM_SUBPORT_COE_PORT_ZERO_BITS_SHIFT) & \
                        _BCM_SUBPORT_COE_PORT_ZERO_BITS_MASK) == 0))

/* Traverse all valid hgproxy SubTag subport ports */
#define BCM_HGPROXY_SUBTAG_COE_PORT_ITER(i, gport)                          \
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||               \
        (soc_feature(unit, soc_feature_channelized_switching))) &&          \
        (_bcm_subtag_subport_port_info[unit] != NULL))                      \
        for (i = 0; i < SOC_INFO(unit).max_subport_coe_ports; i++)          \
            if((_bcm_subtag_subport_port_info[unit][i].valid) &&            \
                (gport =                                                    \
                    _bcm_subtag_subport_port_info[unit][i].subport_port))

/* Get the subport port gport mod id */
#define _BCM_SUBPORT_COE_PORT_GLP_GET(_subport_port)  \
   (_BCM_SUBPORT_COE_PORT_ID_MOD_GET(_subport_port) << 8 ) |  \
   (_BCM_SUBPORT_COE_PORT_ID_PORT_GET(_subport_port))

#endif /* BCM_HGPROXY_COE_SUPPORT */


#ifdef BCM_APACHE_SUPPORT

#define _BCM_APACHE_SUBPORT_COE_GROUP_MAX     (256)
#define _BCM_APACHE_SUBPORT_COE_PORT_MAX      (256)
#define _BCM_APACHE_MAX_SUBPORT_COE_PER_PORT  (80)

#endif /* BCM_APACHE_SUPPORT */

/** End change */

#ifdef BCM_KATANA2_SUPPORT

/* Subport Common defines */
#define _BCM_KT2_PORT_TYPE_ETHERNET         _BCM_COE_PORT_TYPE_ETHERNET
#define _BCM_KT2_PORT_TYPE_CASCADED         _BCM_COE_PORT_TYPE_CASCADED
#define _BCM_KT2_PORT_TYPE_CASCADED_LINKPHY _BCM_COE_PORT_TYPE_CASCADED_LINKPHY
#define _BCM_KT2_PORT_TYPE_CASCADED_SUBTAG  _BCM_COE_PORT_TYPE_CASCADED_SUBTAG

#define _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(_unit, _gport) \
    (BCM_GPORT_IS_SUBPORT_PORT(_gport) && \
        ((soc_feature(_unit, soc_feature_linkphy_coe) && \
        _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT (_unit, _gport)) || \
        (soc_feature(_unit, soc_feature_subtag_coe) && \
         _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT (_unit, _gport)) || \
        (soc_feature(_unit, soc_feature_general_cascade) && \
         _BCM_KT2_GPORT_IS_GENERAL_SUBPORT_PORT (_unit, _gport))))

/* LinkPHY subport specific defines */
#define _BCM_KT2_LINKPHY_STREAMS_MAX                1024
#define _BCM_KT2_LINKPHY_PER_PORT_STREAMS_MAX       128
#define _BCM_KT2_LINKPHY_PER_BLK_STREAMS_MAX        128
/* 
 * Maximum number of streams supported are 2 per
 * subport where each subport port can support 4 
 * Stream Ids in the range Id, Id + 3.
 */ 
#define _BCM_KT2_LINKPHY_MAX_STREAM_IDS_PER_PORT    4

#define _BCM_SB2_MAX_LINKPHY_PORTS                  4
#define _BCM_SB2_MAX_INT_STREAMS_PER_LINKPHY_PORT   8
#define _BCM_SB2_MAX_EXT_STREAMS_PER_LINKPHY_PORT   32

#define _BCM_ML_MAX_LINKPHY_PORTS                  4
#define _BCM_ML_MAX_INT_STREAMS_PER_LINKPHY_PORT   6
#define _BCM_ML_MAX_EXT_STREAMS_PER_LINKPHY_PORT   24
#define _BCM_ML_LINKPHY_PER_BLK_STREAMS_MAX       24

/*
* Katana2:
* There are two tables in TXLP Block in MXQ8 and MXQ9 blocks 
* 1. TXLP_PORT_ADDR_MAP_TABLE - This is programmed when the 
* port is in Non-linkPhy Mode. The start_addr and the end_addr
* are set based on the speed of the port.
* 2. TXLP_STREAM_ADDR_MAP_TABLE - This is programmed when the 
* port is in LinkPhy mode. The startaddr and end_addr are 
* programmed per stream.
* The buffer allocation done as part of the above should be unique
* There are 3423 lines of buffer, reserving the first 0x5f lines
* of buffer for the ports which are in Non-LinkPhy mode.
* When LinkPhy is enabled on a port, the start_addr and end_addr
* for the port in TXLP_PORT_ADDR_MAP_TABLE is set to 0. And the 
* buffer allocated to the port previously is unused. Since the port
* is in LinkPhy mode the start_addr and end_addr in 
* TXLP_STREAM_ADDR_MAP_TABLE is programmed with buffer space 
* leaving buffer space from 0x0 to 0x5f as reserve for Non-linkPhy
* Ports.
*/
#define _BCM_KT2_LINKPHY_STREAM_ID_FLUSH_TIMEOUT    20000

/*
* Katana2 LinkPHY/SubTag subport feature supports
* 1.creation of multiple logical subport groups per port.
*        Port/trunk_id and group id are stored in subport group gport.
* 2.attachment of multiple subport ports to a subport group
*       A subport port is represented by
*       a. (port, external stream id array)] for LinkPHY subport
*       b. (port, VLAN) for SubTag subport
*       Group id and subport_port id are stored in subport port gport.
*/

#define _BCM_KT2_SUBPORT_TYPE_ETHERNET               _BCM_SUBPORT_COE_TYPE_ETHERNET
#define _BCM_KT2_SUBPORT_TYPE_CASCADED               _BCM_SUBPORT_COE_TYPE_CASCADED
#define _BCM_KT2_SUBPORT_TYPE_LINKPHY                _BCM_SUBPORT_COE_TYPE_LINKPHY
#define _BCM_KT2_SUBPORT_TYPE_SUBTAG                 _BCM_SUBPORT_COE_TYPE_SUBTAG
#define _BCM_KT2_SUBPORT_TYPE_GENERAL                _BCM_SUBPORT_COE_TYPE_GENERAL

/*
* LinkPHY/SubTag Subport group gport format 
* bit 31-26 -- gport type (subport group)
* bit 25-24  -- subport group type (cascaded)
* bit 18-17 -- subport subtype (linkphy / subtag / general)
* bit 17    -- subport group is trunk indicator
* bit 16-9  -- subport group port number/trunk group id
* bit 8 -0  -- subport group index
*/
#define _BCM_KT2_SUBPORT_GROUP_TYPE_MASK \
            _BCM_SUBPORT_COE_GROUP_TYPE_MASK
#define _BCM_KT2_SUBPORT_GROUP_TYPE_SHIFT \
            _BCM_SUBPORT_COE_GROUP_TYPE_SHIFT
#define _BCM_KT2_SUBPORT_GROUP_SUBTYPE_MASK \
            _BCM_SUBPORT_COE_GROUP_SUBTYPE_MASK
#define _BCM_KT2_SUBPORT_GROUP_SUBTYPE_SHIFT \
            _BCM_SUBPORT_COE_GROUP_SUBTYPE_SHIFT
#define _BCM_KT2_SUBPORT_GROUP_PORT_MASK \
            _BCM_SUBPORT_COE_GROUP_PORT_MASK
#define _BCM_KT2_SUBPORT_GROUP_PORT_SHIFT \
            _BCM_SUBPORT_COE_GROUP_PORT_SHIFT
#define _BCM_KT2_SUBPORT_GROUP_SPGID_MASK \
            _BCM_SUBPORT_COE_GROUP_SPGID_MASK
#define _BCM_KT2_SUBPORT_GROUP_SPGID_SHIFT \
            _BCM_SUBPORT_COE_GROUP_SPGID_SHIFT
/*
* LinkPHY/SubTag Subport port gport format 
* bit 31-26  -- gport type (subport port)
* bit 25-24  -- subport port type (LinkPHY / SubTag/ General)
* bit 23-08  -- zero
* bit 07-00  -- subport port index  (range 42 to 169)
*/

#define _BCM_KT2_SUBPORT_PORT_TYPE_MASK           0x3
#define _BCM_KT2_SUBPORT_PORT_TYPE_SHIFT          24
#define _BCM_KT2_SUBPORT_PORT_ZERO_BITS_MASK      0xFFFF
#define _BCM_KT2_SUBPORT_PORT_ZERO_BITS_SHIFT     8
#define _BCM_KT2_SUBPORT_PORT_PP_PORT_MASK        0xFF
#define _BCM_KT2_SUBPORT_PORT_PP_PORT_SHIFT       0

/* Check if gport is LinkPHY subport_group */
#define _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_GROUP(_subport_group)    \
            _BCM_COE_GPORT_IS_LINKPHY_SUBPORT_GROUP(_subport_group)

/* Check if gport is SubTag subport_group */
#define _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_GROUP(_subport_group)    \
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_GROUP(_subport_group)

/* Check if gport is general subport_group */
#define _BCM_KT2_GPORT_IS_GENERAL_SUBPORT_GROUP(_subport_group)    \
            _BCM_COE_GPORT_IS_GENERAL_SUBPORT_GROUP(_subport_group)

/* Set the subport port gport  type*/
#define _BCM_KT2_SUBPORT_PORT_TYPE_SET(_subport_port, _type)  \
             _BCM_SUBPORT_COE_PORT_TYPE_SET(_subport_port, _type)

/* Set the subport port gport  pp_port id*/
#define _BCM_KT2_SUBPORT_PORT_ID_SET(_subport_port, _pp_port)  \
    (BCM_GPORT_SUBPORT_PORT_SET((_subport_port), \
        ((_pp_port & _BCM_KT2_SUBPORT_PORT_PP_PORT_MASK) \
             << _BCM_KT2_SUBPORT_PORT_PP_PORT_SHIFT)))

/* Get the subport port gport pp_port id */
#define _BCM_KT2_SUBPORT_PORT_ID_GET(_subport_port)   \
    (((_subport_port) >> _BCM_KT2_SUBPORT_PORT_PP_PORT_SHIFT) & \
                         _BCM_KT2_SUBPORT_PORT_PP_PORT_MASK)


/* Check if gport is LinkPHY subport_port */
#define _BCM_KT2_GPORT_IS_LINKPHY_SUBPORT_PORT(_unit, _subport_port)    \
        (((((_subport_port) >> _BCM_KT2_SUBPORT_PORT_TYPE_SHIFT) & \
                        _BCM_KT2_SUBPORT_PORT_TYPE_MASK) == \
                        _BCM_KT2_SUBPORT_TYPE_LINKPHY) && \
         ((((_subport_port) >> _BCM_KT2_SUBPORT_PORT_ZERO_BITS_SHIFT) & \
                        _BCM_KT2_SUBPORT_PORT_ZERO_BITS_MASK) == 0) && \
        (BCM_PBMP_MEMBER(SOC_INFO(_unit).linkphy_pp_port_pbm, \
            (((_subport_port) >> _BCM_KT2_SUBPORT_PORT_PP_PORT_SHIFT) & \
                        _BCM_KT2_SUBPORT_PORT_PP_PORT_MASK))))

/* Check if gport is SubTag subport_port */
#define _BCM_KT2_GPORT_IS_SUBTAG_SUBPORT_PORT(_unit, _subport_port)    \
        (((((_subport_port) >> _BCM_KT2_SUBPORT_PORT_TYPE_SHIFT) & \
                        _BCM_KT2_SUBPORT_PORT_TYPE_MASK) == \
                        _BCM_KT2_SUBPORT_TYPE_SUBTAG) && \
         ((((_subport_port) >> _BCM_KT2_SUBPORT_PORT_ZERO_BITS_SHIFT) & \
                        _BCM_KT2_SUBPORT_PORT_ZERO_BITS_MASK) == 0) && \
        (BCM_PBMP_MEMBER(SOC_INFO(_unit).subtag_pp_port_pbm, \
            (((_subport_port) >> _BCM_KT2_SUBPORT_PORT_PP_PORT_SHIFT) & \
                        _BCM_KT2_SUBPORT_PORT_PP_PORT_MASK))))

/* Check if gport is General subport_port */
#define _BCM_KT2_GPORT_IS_GENERAL_SUBPORT_PORT(_unit, _subport_port)    \
    (((((_subport_port) >> _BCM_KT2_SUBPORT_PORT_TYPE_SHIFT) & \
                    _BCM_KT2_SUBPORT_PORT_TYPE_MASK) == \
                    _BCM_KT2_SUBPORT_TYPE_GENERAL) && \
     ((((_subport_port) >> _BCM_KT2_SUBPORT_PORT_ZERO_BITS_SHIFT) & \
                    _BCM_KT2_SUBPORT_PORT_ZERO_BITS_MASK) == 0) && \
     (BCM_PBMP_MEMBER(SOC_INFO(_unit).general_pp_port_pbm, \
                      (((_subport_port) >> _BCM_KT2_SUBPORT_PORT_PP_PORT_SHIFT) & \
                       _BCM_KT2_SUBPORT_PORT_PP_PORT_MASK))))


typedef struct {
    bcm_port_t port;
    int        port_type;
    bcm_vlan_t subtag;
    int        num_streams;
    uint16     dev_int_stream_id[BCM_SUBPORT_CONFIG_MAX_STREAMS];
} _bcm_kt2_subport_info_t;

#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
#define BCM_SUBPORT_WB_VERSION_1_0          SOC_SCACHE_VERSION(1,0)
#define BCM_SUBPORT_WB_VERSION_1_1          SOC_SCACHE_VERSION(1,1)
#define BCM_SUBPORT_WB_DEFAULT_VERSION      BCM_SUBPORT_WB_VERSION_1_1

extern int _bcm_esw_subport_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_tr_subport_sw_dump(int unit);
extern void _bcm_tr2_subport_sw_dump(int unit);
extern void _bcm_kt2_subport_sw_dump(int unit);

#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern int
_bcm_subport_physical_port_get(int unit, int pp_port,
                              int *local_port);

extern int
_bcmi_linkphy_subport_stream_dfc_update(int unit,
                                        bcm_subport_config_t *config,
                                        int pp_port_index,
                                        int ext_stream_id,
                                        int enable);

extern int
_bcmi_linkphy_subport_stream_flush(int unit,
                                   bcm_subport_config_t *config,
                                   int pp_port_index,
                                   int ext_stream_id);

extern int
_bcm_coe_subtag_subport_port_subport_num_get(int unit,
                                             bcm_gport_t subport,
                                             bcm_port_t *port,
                                             int *index);

extern int
_bcm_coe_subtag_subport_num_subport_gport_get(int unit,
                                             int subport_num,
                                             bcm_gport_t *subport_gport);

extern int
_bcm_coe_subport_mod_port_subport_num_get(int unit,
                                           bcm_module_t modid,
                                           bcm_port_t portid,
                                           int *subport_num);


#ifdef BCM_CHANNELIZED_SWITCHING_SUPPORT

#define BCMI_CES_MAX_CHANNEL_ID       255

#define BCMI_CES_SUBPORT_PKT_VLAN_CHK(_u_, v)  \
            (v > BCMI_CES_MAX_CHANNEL_ID)


typedef struct _bcm_subport_port_group_state_s {
   int num_groups;
   SHR_BITDCL *_bcm_subport_port_group_bitmap;
   int *_bcm_subport_port_group_refcnt;
}_bcm_subport_port_group_state_t;

extern _bcm_subport_port_group_state_t subport_port_group_state[BCM_MAX_NUM_UNITS];


#define _BCM_SUBPORT_PORT_GROUP_CNT(unit) \
            subport_port_group_state[unit].num_groups

#define _BCM_SUBPORT_PORT_GROUP_BMP(unit) \
            (subport_port_group_state[unit])._bcm_subport_port_group_bitmap

#define _BCM_SUBPORT_PORT_GROUP_REFCNT(unit) \
            (subport_port_group_state[unit])._bcm_subport_port_group_refcnt

#define _BCM_SUBPORT_PORT_GROUP_ID_REFCNT(unit, id) \
            (subport_port_group_state[unit])._bcm_subport_port_group_refcnt[id]

/* Function definitions: CES */
extern bcm_error_t
bcmi_subport_port_group_init(int unit);

extern bcm_error_t
bcmi_subport_port_group_id_create(int unit,
                                  uint32 options,
                                  bcm_subport_port_group_t *group_id);
extern bcm_error_t
bcmi_subport_port_group_id_destroy(int unit, bcm_subport_port_group_t group_id);

extern bcm_error_t
bcmi_subport_port_group_id_destroy_all(int unit);

extern bcm_error_t
bcmi_subport_port_group_id_get(int unit, bcm_subport_port_group_t group_id);

extern bcm_error_t
bcmi_subport_port_group_id_get_all(int unit,
                                   int max_groups,
                                   bcm_subport_port_group_t *group_idz,
                                   int *group_size);
extern bcm_error_t
bcmi_subport_port_group_subport_multi_set(int unit,
                                          bcm_subport_port_group_t group_id,
                                          int num_ports,
                                          bcm_gport_t *subport_gports);

extern bcm_error_t
bcmi_subport_port_group_subport_multi_get(int unit,
                                          bcm_subport_port_group_t group_id,
                                          int max_subports,
                                          bcm_gport_t *subport_gports,
                                          int *num_subports);
#define BCMI_PORT_GROUP_CLEANUP_IF_ERROR(rv)      \
            do {                       \
                if (BCM_FAILURE(rv)) { \
                    goto cleanup;      \
                }                      \
            } while(0);



extern int
bcmi_subport_tunnel_pbmp_profile_init(int unit);

extern int
bcmi_subport_tunnel_pbmp_profile_create(int unit,
                                       int num_subports,
                                       bcm_gport_t *subport_gports,
                                       int *profile_index);

extern int
bcmi_subport_tunnel_pbmp_profile_get(int unit,
                                       int num_subports,
                                       bcm_gport_t *subport_gports,
                                       int *profile_index);

extern int
bcmi_subport_tunnel_pbmp_profile_destroy(int unit,
                                         int profile_index);
extern int
bcmi_subport_tunnel_pbmp_profile_destroy_all(int unit);

extern int
bcmi_subport_multicast_lc_pbm_set(int unit,
                                  int repl_group,
                                  bcm_gport_t subport_gport,
                                  int set_reset);

extern int
bcmi_subport_multicast_lc_pbm_clear_all(int unit,
                                        int repl_group);

extern int
_bcm_coe_subtag_subport_port_get_by_modport(int unit,
                         bcm_gport_t modport_gport,
                         bcm_gport_t *subport_gport);



#define BCMI_FB6_SUBPORT_MODID_PORT_MAX      (128)

#endif /*BCM_CHANNELIZED_SWITCHING_SUPPORT*/

#endif /* !_BCM_INT_ESW_SUBPORT_H_ */
