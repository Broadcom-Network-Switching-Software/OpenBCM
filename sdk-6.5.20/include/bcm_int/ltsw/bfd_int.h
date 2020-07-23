/*! \file bfd_int.h
 *
 * BFD module internal header file.
 * This file contains BFD definitions internal
 * to the BFD module itself.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_BFD_INT_H
#define BCMINT_LTSW_BFD_INT_H

#include <bcm/types.h>
#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <shr/shr_util_pack.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <sal/sal_thread.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmltd/chip/bcmltd_oam_constants.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcm_int/ltsw/generated/bfd_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCM_BFD

#define BFD_LTSW_INFO(_u) (&bfd_glb_info[_u])

#define BFD_LTSW_INIT_CHECK(unit) \
    do { \
        if (BFD_LTSW_INFO(unit)->init == 0) { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define BFD_LTSW_LOCK(unit) \
    do { \
        if (BFD_LTSW_INFO(unit)->lock) { \
            sal_mutex_take(BFD_LTSW_INFO(unit)->lock, SAL_MUTEX_FOREVER); \
        } else { \
            SHR_ERR_EXIT(SHR_E_INIT); \
        } \
    } while(0)

#define BFD_LTSW_UNLOCK(unit) \
    do { \
        if (BFD_LTSW_INFO(unit)->lock) { \
            sal_mutex_give(BFD_LTSW_INFO(unit)->lock); \
        } else { \
            _func_rv = SHR_E_INIT;   \
        } \
    } while(0)

typedef enum _bfd_ha_sub_comp_id_e {
    BFD_SUB_COMP_ID_GLOBAL_LIST                = 0,
    BFD_SUB_COMP_ID_PER_EP_INFO                = 1
}bfd_ha_sub_comp_id_t;


#define BFD_ID_INFO(_u) BFD_LTSW_INFO(_u)->ha_info

#define BFD_GLB_ID_BMP(_u) BFD_ID_INFO(_u).glb_id_bmp->w

#define BFD_GLB_ID_BMP_SET(unit, id)       \
    SHR_BITSET(BFD_GLB_ID_BMP(unit),id)
#define BFD_GLB_ID_BMP_GET(unit, id)       \
    SHR_BITGET(BFD_GLB_ID_BMP(unit),id)
#define BFD_GLB_ID_BMP_CLEAR(unit, id)     \
    SHR_BITCLR(BFD_GLB_ID_BMP(unit),id)


#define BFD_HA_EP_INFO(_u, id) BFD_ID_INFO(_u).ep_info[id]

#define BFD_EP_ENCAP_TYPE(unit, id)             \
        (BFD_HA_EP_INFO(unit,id).ep_type)

#define BFD_EP_EGRESS_IF(unit, id)              \
        (BFD_HA_EP_INFO(unit,id).egress_if)

#define BFD_EP_GPORT(unit, id)                  \
        (BFD_HA_EP_INFO(unit,id).gport)

#define BFD_EP_TX_GPORT(unit, id)               \
        (BFD_HA_EP_INFO(unit,id).tx_gport)

/* Macro to differentiate BFD encap type */
#define BFD_ENCAP_TYPE_TNL_IPV4(x) ((x == bcmBFDTunnelTypeIp4in4) || (x == bcmBFDTunnelTypeIp4in6) || \
                                    (x == bcmBFDTunnelTypeGRE)|| (x == bcmBFDTunnelTypeGre4In6))


#define BFD_ENCAP_TYPE_TNL_IPV6(x) ((x == bcmBFDTunnelTypeIp6in4) || (x == bcmBFDTunnelTypeIp6in6) || \
                                    (x == bcmBFDTunnelTypeGre6In4)|| (x == bcmBFDTunnelTypeGre6In6))


#define BFD_ENCAP_TYPE_TNL_MPLS(x) ((x == bcmBFDTunnelTypeMpls) || (x == bcmBFDTunnelTypeMplsTpCc) || \
                                    (x == bcmBFDTunnelTypeMplsTpCcCv))

#define BFD_LTSW_MAX_MPLS_LABELS 7

#define BFD_LTSW_MEP_IDENTIFIER_MAX_LENGTH 32

#define BCM_LTSW_BFD_ENDPOINT_IPV6 0x80000000

/*
 *   Default BFD RX Event Notification thread priority
 */
#define BCM_LTSW_BFD_THREAD_PRI_DFLT     200


#define BCMINT_BFD_MEM_ALLOC(_ptr, _sz, _str)     \
    do {                                          \
        SHR_ALLOC(_ptr, _sz, _str);               \
        SHR_NULL_CHECK(_ptr, SHR_E_MEMORY);       \
        sal_memset(_ptr, 0, _sz);                 \
    } while(0)

#define BCMINT_BFD_MEM_FREE(_ptr)               \
    do {                                        \
        SHR_FREE(_ptr);                         \
    } while(0)

/* Checks for 'null' argument */
#define BFD_LTSW_PARAM_NULL_CHECK(arg_)  \
    if ((arg_) == NULL) {       \
        SHR_IF_ERR_EXIT(SHR_E_PARAM);     \
    }

/* BFD Event Handler */
typedef struct bfd_ltsw_event_handler_s {
    struct bfd_ltsw_event_handler_s *next;
    bcm_bfd_event_types_t event_types;
    bcm_bfd_event_cb cb;
    void *user_data;
} bfd_ltsw_event_handler_t;

/* Book keeping information to track allocation of BFD ids. */
typedef struct bcmint_bfd_ha_info_s{
    /*! Bitmap of endpoint IDs.*/
    bcmi_bfd_epid_bmp_t *glb_id_bmp;

    /*! Endpoint params: [EP_TYPE][EGRESS_IF][TX_GPORT][GPORT] */
    bcmi_bfd_ep_info_t *ep_info;

}bcmint_bfd_ha_info_t;

/* Internal data structure to store global BFD information. */
typedef struct bcm_int_bfd_info_s {
    /* Is BFD initialized*/
    bool init;

    /* Maximum number of BFD endpoints that can be created */
    uint32_t max_endpoints;

    /* Maximum number of BFD SHA1 authentication keys. */
    uint32_t max_auth_sha1_keys;

    /* Maximum number of BFD simple password authentication keys. */
    uint32_t max_auth_simple_password_keys;

    /* Maximum packet size. */
    uint32_t max_pkt_size;

    /* Enable to use an internally generated value for the My Discriminator */
    bool use_ep_id_as_disc;

    /* Enable to prevent the run-time update of the remote discriminator */
    bool static_remote_disc;

    /* Enable to raise the LOCAL_STATE_DOWN event if the endpoint does not
       transition to Up state after creation */
    bool state_down_event_on_ep_creation;

    /* Enable to set the C bit when transmitting BFD control packets.*/
    bool cpi;

    /*! Enable to allow transmission of BFD packets from the endpoints. */
    bool tx;

    /*! Enable for raw ingress inject of BFD Tx packets. */
    bool tx_raw_ingress;

    /* BFD module lock. */
    sal_mutex_t lock;

    /* List of event handlers */
    bfd_ltsw_event_handler_t *event_handler_list;

    /* BFD ID info */
    bcmint_bfd_ha_info_t ha_info;
} bcm_int_bfd_info_t;

extern bcm_int_bfd_info_t bfd_glb_info[BCM_MAX_NUM_UNITS];

/* Chip common driver routines */
extern int
bcm_ltsw_bfd_init_common(int unit);

extern int
bcm_ltsw_bfd_detach_common(int unit);


/* Logical tables access routines */

extern int
bcm_ltsw_bfd_control_entry_get(int unit,
        bcm_int_bfd_info_t *entry);

extern int
bcm_ltsw_bfd_control_entry_set(int unit,
        bool update,
        bcm_int_bfd_info_t *entry);

extern int
bcm_ltsw_bfd_tnl_mpls_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_ipv6_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_ipv4_endpoint_entry_set(int unit,
        bool update,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_endpoint_create_common(int unit,
                              bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_tnl_mpls_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_tnl_ipv6_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_tnl_ipv4_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_ipv6_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_ipv4_endpoint_entry_get(int unit,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int
bcm_ltsw_bfd_endpoint_entry_delete(int unit,
            bcm_bfd_endpoint_t id);

extern int bcm_ltsw_bfd_endpoint_destroy_common(int unit,
        bcm_bfd_endpoint_t endpoint);

extern int bcm_ltsw_bfd_destroy_all_common(int unit);

extern int bcm_ltsw_bfd_discard_stat_entry_get(int unit,
        bcm_bfd_discard_stat_t *discarded_info);

extern int bcm_ltsw_bfd_discard_stat_entry_set(int unit,
        bcm_bfd_discard_stat_t *discarded_info);

extern int bcm_ltsw_bfd_auth_simple_password_get_common(int unit,
        int index,
        bcm_bfd_auth_simple_password_t *sp);

extern int bcm_ltsw_bfd_auth_sha1_set_common(int unit,
        int index,
        bcm_bfd_auth_sha1_t *sha1);

extern int bcm_ltsw_bfd_auth_simple_password_set_common(int unit,
        int index,
        bcm_bfd_auth_simple_password_t *sp);

extern int  bcm_ltsw_bfd_endpoint_poll_common(int unit, bcm_bfd_endpoint_t endpoint);

extern int bcm_ltsw_bfd_endpoint_get_common(int unit,
        bcm_bfd_endpoint_t endpoint,
        bcm_bfd_endpoint_info_t *endpoint_info);

extern int bcm_ltsw_bfd_endpoint_destroy_common(int unit,
        bcm_bfd_endpoint_t endpoint);

extern int bcm_ltsw_bfd_destroy_all_common(int unit);

extern int bcm_ltsw_bfd_tx_set_common(int unit, bool enable);

extern int bcm_ltsw_bfd_endpoint_poll_common(int unit, bcm_bfd_endpoint_t endpoint);

extern int bcm_ltsw_bfd_auth_sha1_get_common(int unit,
        int index,
        bcm_bfd_auth_sha1_t *sha1);

extern int bcm_ltsw_bfd_endpoint_stat_entry_get(int unit, bcm_bfd_endpoint_t endpoint,
        bcm_bfd_endpoint_stat_t *ctr_info);

extern int bcm_ltsw_bfd_endpoint_stat_entry_set(int unit, bcm_bfd_endpoint_t endpoint);

extern int bcm_ltsw_bfd_endpoint_stat_get_common(int unit,
        bcm_bfd_endpoint_t endpoint,
        bcm_bfd_endpoint_stat_t *ctr_info, uint32_t options);

extern int bcm_ltsw_bfd_endpoint_stat_entry_set(int unit, bcm_bfd_endpoint_t endpoint);

extern int bcm_ltsw_bfd_endpoint_stat_entry_get(int unit, bcm_bfd_endpoint_t endpoint,
        bcm_bfd_endpoint_stat_t *ctr_info);

extern int bcm_ltsw_bfd_tx_start_field_set(int unit, bool enable);

extern int bcm_ltsw_bfd_endpoint_poll_field_set(int unit, bcm_bfd_endpoint_t endpoint);

extern int bcm_ltsw_bfd_endpoint_sp_entry_set(int unit, int index,
    bcm_bfd_auth_simple_password_t *sp);

extern int bcm_ltsw_bfd_endpoint_sp_entry_get(int unit, int index,
    bcm_bfd_auth_simple_password_t *sp);

extern int bcm_ltsw_bfd_endpoint_auth_sha1_entry_set(int unit, int index, bcm_bfd_auth_sha1_t *sha1);

extern int bcm_ltsw_bfd_endpoint_auth_sha1_entry_get(int unit, int index,  bcm_bfd_auth_sha1_t *sha1);

extern int bcm_ltsw_bfd_event_control_entry_set(int unit, bcm_bfd_event_types_t event_types);

extern int
bcm_ltsw_bfd_event_entry_get(int unit, bcmlt_entry_handle_t entry_hdl,
        bcm_bfd_event_type_t *event, int *ep_id);

extern int
bcm_ltsw_bfd_event_register_common(int unit,
                             bcm_bfd_event_types_t event_types,
                             bcm_bfd_event_cb cb, void *user_data);

extern int bcm_ltsw_bfd_endpoint_sp_entry_delete(int unit, int index);

extern int bcm_ltsw_bfd_endpoint_auth_sha1_entry_delete(int unit,
int index);

extern int bcm_ltsw_bfd_endpoint_status_entry_get(int unit,
        bcm_bfd_endpoint_t endpoint,
        bcm_bfd_endpoint_info_t *ep_info);

extern int bcm_ltsw_bfd_event_unregister_common(int unit,
        bcm_bfd_event_types_t event_types,
        bcm_bfd_event_cb cb);
#endif /* BCMINT_LTSW_BFD_INT_H */
