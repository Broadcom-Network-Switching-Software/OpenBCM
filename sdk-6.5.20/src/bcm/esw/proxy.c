/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <soc/scache.h>

#ifdef INCLUDE_L3
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/l3x.h>
#include <bcm/error.h>
#include <bcm/proxy.h>
#include <bcm/types.h>
#include <bcm/l3.h>

#include <bcm_int/esw/port.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/proxy.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT_SUPPORT*/

#define _BCM_PROXY_INFO_ENTRY_CREATED   0x00000001
#define _BCM_PROXY_INFO_GROUP_CREATED   0x00000002
#define _BCM_PROXY_INFO_FILTER_CREATED  0x00000004
#define _BCM_PROXY_INFO_INSTALLED       0x00000008
#define _BCM_PROXY_INFO_SERVER          0x00000010

#define _BCM_PROXY_SERVER_INSTANCES     3
#define _BCM_PROXY_BUD_NODE_0           0
#define _BCM_PROXY_BUD_NODE_1           1
#define _BCM_PROXY_HYBRID               2

#ifdef BCM_WARM_BOOT_SUPPORT
/* Warm boot scache version to store
 * server_gport and server_gport_mask information.
 */
#define BCM_PROXY_WB_VERSION_1_0        SOC_SCACHE_VERSION(1,0)
#define BCM_PROXY_WB_DEFAULT_VERSION    BCM_PROXY_WB_VERSION_1_0
#endif

#define _LOCK(unit) _bcm_proxy_control[unit].proxy_lock

#define _BCM_PROXY_LOCK(unit) \
   _LOCK(unit) ? sal_mutex_take(_LOCK(unit), sal_mutex_FOREVER) : -1

#define _BCM_PROXY_UNLOCK(unit) sal_mutex_give(_LOCK(unit))

static bcmi_proxy_bookkeep_t proxy_bookkeep[BCM_MAX_NUM_UNITS];
static int                   proxy_internal_initialized[BCM_MAX_NUM_UNITS] = {0};

#define CHECK_PROXY_INIT(unit)                       \
        if (!proxy_internal_initialized[unit])             \
            return BCM_E_INIT
typedef struct _bcm_proxy_info_s {
    int                      flags; /* _BCM_PROXY_INFO_* */
    bcm_port_t               client_port;
    bcm_proxy_proto_type_t   proto_type;
    bcm_module_t             server_modid;
    bcm_port_t               server_port;
    bcm_proxy_mode_t         mode;
    bcm_field_entry_t        eid;
    bcm_field_group_t        gid;
    struct _bcm_proxy_info_s *next;
} _bcm_proxy_info_t;

typedef struct _bcm_proxy_control_s {
    sal_mutex_t       proxy_lock;
    _bcm_proxy_info_t *proxy_list;
    int               num_clients;
} _bcm_proxy_control_t;

typedef int (*proxy_install_method_t)(int unit,
                                      _bcm_proxy_info_t *dst,
                                      _bcm_proxy_info_t *src);

typedef int (*proxy_uninstall_method_t)(int unit,
                                        _bcm_proxy_info_t *src);

typedef int (*proxy_match_method_t)(_bcm_proxy_info_t *dst,
                                    _bcm_proxy_info_t *src);

typedef struct {
    proxy_install_method_t      install;
    proxy_uninstall_method_t    uninstall;
    proxy_match_method_t        match;
} _bcm_proxy_ifc_t;

STATIC int
_bcm_esw_proxy_client_install_xgs3(int unit, _bcm_proxy_info_t *info);

STATIC int
_bcm_esw_proxy_client_install(int unit,
                              _bcm_proxy_info_t *dst,
                              _bcm_proxy_info_t *src);

STATIC int
_bcm_esw_proxy_client_uninstall_xgs3(int unit, _bcm_proxy_info_t *info);

STATIC int
_bcm_esw_proxy_client_uninstall(int unit, _bcm_proxy_info_t *info);

STATIC int
_bcm_esw_proxy_client_match(_bcm_proxy_info_t *dst, _bcm_proxy_info_t *src);

STATIC int
_bcm_esw_proxy_server_install(int unit,
                              _bcm_proxy_info_t *dst,
                              _bcm_proxy_info_t *src);

STATIC int
_bcm_esw_proxy_server_uninstall(int unit,
                                _bcm_proxy_info_t *src);

STATIC int
_bcm_esw_proxy_server_match(_bcm_proxy_info_t *dst, _bcm_proxy_info_t *src);

static _bcm_proxy_control_t _bcm_proxy_control[BCM_MAX_NUM_UNITS];

STATIC 
_bcm_proxy_ifc_t client_methods = {
    _bcm_esw_proxy_client_install,
    _bcm_esw_proxy_client_uninstall,
    _bcm_esw_proxy_client_match,
};

STATIC 
_bcm_proxy_ifc_t server_methods = {
    _bcm_esw_proxy_server_install,
    _bcm_esw_proxy_server_uninstall,
    _bcm_esw_proxy_server_match,
};

/***************************************************************** Utilities */

/*
 * Function:
 *      _bcm_esw_proxy_gport_resolve
 * Purpose:
 *      Decodes gport into port and module id
 * Parameters:
 *      unit         -  BCM Unit number
 *      gport        - GPORT 
 *      port_out     - (OUT) port encoded into gport 
 *      modid_out    - (OUT) modid encoded into gport
 *      isLocal      - Indicator that port encoded in gport must be local
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_esw_proxy_gport_resolve(int unit, bcm_gport_t gport,
                             bcm_port_t *port_out,
                             bcm_module_t *modid_out, int isLocal)
{

    bcm_port_t      port; 
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             id;

    if (NULL == port_out || NULL == modid_out) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, gport, &modid, &port, &tgid, &id));

    if ((-1 != id) || (BCM_TRUNK_INVALID != tgid)){
        return BCM_E_PORT;
    }

    if (isLocal) {
        int    ismymodid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, modid, &ismymodid));
        if (ismymodid != TRUE) {
            return BCM_E_PARAM;
        }
    } 

    *port_out = port;
    *modid_out = modid;
    
    return BCM_E_NONE;
}

/****************************************************************** Generics */

/*
 * Function:
 *      _bcm_esw_proxy_install
 * Purpose:
 *      Install proxy client or server
 * Parameters:
 *      unit           -  BCM Unit number
 *      bcm_proxy_ifc  -  local port for which redirection is applied
 *      data           -  Packet type to classify for redirection
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_proxy_install(int unit,
                       _bcm_proxy_ifc_t *ifc,
                       _bcm_proxy_info_t *src)
{
    int rv = BCM_E_INTERNAL;
    _bcm_proxy_info_t *dst;

    /* Allocate proxy info structure */
    dst = sal_alloc(sizeof(_bcm_proxy_info_t), "bcm_esw_proxy");

    if (dst == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(dst, 0, sizeof(_bcm_proxy_info_t));

    if (_BCM_PROXY_LOCK(unit)) {
        sal_free(dst);
        return BCM_E_INIT;
    }

    rv = ifc->install(unit, dst, src);
        
    if (BCM_SUCCESS(rv)) {
        /* Put onto list */
        dst->next = _bcm_proxy_control[unit].proxy_list;
        _bcm_proxy_control[unit].proxy_list = dst;
    } else {
        sal_free(dst);
    }

    _BCM_PROXY_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_proxy_uninstall
 * Purpose:
 *      Uninstall proxy client or server
 * Parameters:
 *      unit           -  BCM Unit number
 *      ifc            -  client/server methods
 *      info           -  info record to uninstall
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_proxy_uninstall(int unit,
                         _bcm_proxy_ifc_t *ifc,
                         _bcm_proxy_info_t *src)
{
    int rv = BCM_E_INTERNAL;
    _bcm_proxy_info_t *dst;

    if (_BCM_PROXY_LOCK(unit)) {
        return BCM_E_INIT;
    }

    /* Uninstall the enabled proxy client if the hardware is still
       capable. */
    if (SOC_HW_ACCESS_DISABLE(unit) == 0) {
        rv = ifc->uninstall(unit, src);
    } else {
        rv = BCM_E_NONE;
    }
    /* Unlink from list */
    if (_bcm_proxy_control[unit].proxy_list == src) {
        /* Info at beginning of the list */
        _bcm_proxy_control[unit].proxy_list = src->next;
    } else {
        for (dst = _bcm_proxy_control[unit].proxy_list;
             dst != NULL;
             dst = dst->next) {
            if (dst->next == src) {
                dst->next = src->next;
                break;
            }
        }
    }
    _BCM_PROXY_UNLOCK(unit);

    sal_memset(src, 0, sizeof(_bcm_proxy_info_t));
    sal_free(src);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_proxy_find
 * Purpose:
 *      Find a previously created proxy client or server
 * Parameters:
 *      unit         -  BCM Unit number
 *      ifc          -  interface methods
 *      src          -  data to find
 *      dstp         -  data found, or NULL
 * Returns:
 *      BCM_E_NONE     - found
 *      BCM_E_NOTFOUND - not found
 *      BCM_E_INIT     - subsystem not initialized
 */

STATIC int
_bcm_esw_proxy_find(int unit,
                    _bcm_proxy_ifc_t *ifc,
                    _bcm_proxy_info_t *src,
                    _bcm_proxy_info_t **dstp)
{
    _bcm_proxy_info_t *dst;
    int rv = BCM_E_NOT_FOUND;

    if (_BCM_PROXY_LOCK(unit)) {
        return BCM_E_INIT;
    }

    for (dst = _bcm_proxy_control[unit].proxy_list;
         dst != NULL;
         dst = dst->next) {
        if (ifc->match(dst, src)) {
            rv = BCM_E_EXISTS;
            break;
        }
    }
    _BCM_PROXY_UNLOCK(unit);
    *dstp = dst;

    return rv;
}

/*
 * Function:
 *      _bcm_esw_proxy
 * Purpose:
 *      Generic proxy client or server install or uninstall
 * Parameters:
 *      unit         -  BCM Unit number
 *      ifc          -  interface methods
 *      src          -  data to find
 * Returns:
 *      Pointer to _bcm_proxy_info_t if found
 *      NULL if not found
 */

STATIC int
_bcm_esw_proxy(int unit, _bcm_proxy_ifc_t *ifc,
               _bcm_proxy_info_t *src, int enable)
{
    int rv;
    _bcm_proxy_info_t *dst = NULL;

    rv = _bcm_esw_proxy_find(unit, ifc, src, &dst);

    if (enable) {

        if (rv != BCM_E_NOT_FOUND) {
            return rv;
        }

        rv = _bcm_esw_proxy_install(unit, ifc, src);

    } else {
        /* Disable */

        if (rv != BCM_E_EXISTS) {
            return rv;
        }
        
        rv = _bcm_esw_proxy_uninstall(unit, ifc, dst);

    }

    return rv;
}

/******************************************************************** Client */

/*
 * Function:
 *      _bcm_proxy_client_enabled
 * Purpose:
 *      Returns true if the proxy client is enabled on the given unit
 * Parameters:
 *      unit         -  BCM Unit number
 * Returns:
 *      TRUE or FALSE
 */

int
_bcm_proxy_client_enabled(int unit)
{
    return (_bcm_proxy_control[unit].num_clients > 0);
}

/*
 * Function:
 *      bcm_esw_proxy_client_set
 * Purpose:
 *      Enables redirection for a certain traffic type using either 
 *      FFP or FP rule
 * Parameters:
 *      unit         -  BCM Unit number
 *      client_port  -  local port for which redirection is applied
 *      proto_type   -  Packet type to classify for redirection
 *      server_modid -  Module ID of remote device which performs lookups
 *      server_port  -  Port on remote device where redirected packets are 
 *                      destined to
 *      enable       -  toggle to enable or disable redirection
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_proxy_client_set(int unit, bcm_port_t client_port, 
                         bcm_proxy_proto_type_t proto_type,
                         bcm_module_t server_modid, bcm_port_t server_port, 
                         int enable)
{
    _bcm_proxy_info_t src;

    if (BCM_GPORT_IS_SET(client_port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, client_port, &client_port));
    }
    if (BCM_GPORT_IS_SET(server_port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_proxy_gport_resolve(unit, server_port, &server_port,
                                         &server_modid, 0));
    }

    if (!SOC_PORT_VALID(unit, client_port)) {
        return BCM_E_PORT;
    }
    if (server_port < 0 ) {
        return BCM_E_PORT;
    }

    sal_memset(&src, 0, sizeof(_bcm_proxy_info_t));
    src.client_port  = client_port;
    src.proto_type   = proto_type;
    src.server_modid = server_modid;
    src.server_port  = server_port;

    return _bcm_esw_proxy(unit, &client_methods, &src, enable);
}

/*
 * Function:
 *      _bcm_esw_proxy_client_match
 * Purpose:
 *      Return true if src client record matches dst
 * Parameters:
 *      dst - 
 *      src - 
 * Returns:
 *      True if matched
 */

STATIC int
_bcm_esw_proxy_client_match(_bcm_proxy_info_t *dst, _bcm_proxy_info_t *src)
{
        return (dst->client_port == src->client_port &&
                dst->proto_type == src->proto_type &&
                dst->server_modid == src->server_modid &&
                dst->server_port == src->server_port &&
                (dst->flags & _BCM_PROXY_INFO_SERVER) == 0);
}

/*
 * Function:
 *      _bcm_esw_proxy_server_match
 * Purpose:
 *      Return true if src server record matches dst
 * Parameters:
 *      dst - 
 *      src - 
 * Returns:
 *      True if matched
 */

STATIC int
_bcm_esw_proxy_server_match(_bcm_proxy_info_t *dst,
                            _bcm_proxy_info_t *src)
{
    return (dst->mode == src->mode &&
            dst->server_port == src->server_port &&
            (dst->flags & _BCM_PROXY_INFO_SERVER) != 0);
}

/*
 * Function:
 *      _bcm_esw_proxy_client_create_xgs3
 * Purpose:
 *      Creates proxy client using FP rules, doesn't clean up on error
 * Parameters:
 *      unit           -  BCM Unit number
 *      bcm_proxy_info -  proxy data
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_proxy_client_create_xgs3(int unit, _bcm_proxy_info_t *bcm_proxy_info)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    _field_stage_t         *stage_fc=NULL;  /* Stage Field control structure.   */
    bcm_port_config_t      pc;              /* Port Configuration structure.    */
    bcm_field_group_config_t fg;            /* Group configuration structure.   */
    int                    instance=0;      /* Pipe instance.                   */
    bcm_pbmp_t             mask_pbmp;       /* IPBM mask.                       */
    bcm_field_entry_t      eid;
    bcm_field_group_t      gid;
    bcm_pbmp_t             ingress_pbmp;
    bcm_port_t             client_port = bcm_proxy_info->client_port;
    bcm_proxy_proto_type_t proto_type = bcm_proxy_info->proto_type;
    bcm_module_t           server_modid = bcm_proxy_info->server_modid;
    bcm_port_t             server_port = bcm_proxy_info->server_port;

    BCM_PBMP_PORT_SET(ingress_pbmp, client_port);
   
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc));
    
    bcm_port_config_t_init(&pc);
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &pc));

    bcm_field_group_config_t_init(&fg);
    instance = SOC_INFO(unit).port_pipe[client_port];   
    mask_pbmp = PBMP_ALL(unit); 

    /* Replace chip name check with feature check to make it work for TD3 also similar to TH */
    if (soc_feature(unit, soc_feature_multi_pipe_mapped_ports) || SOC_IS_TOMAHAWKX(unit)) {
        switch (stage_fc->oper_mode) {
            case bcmFieldGroupOperModeGlobal:
                break;
            case bcmFieldGroupOperModePipeLocal:
                fg.ports = pc.per_pipe[instance];
                fg.flags |= BCM_FIELD_GROUP_CREATE_WITH_PORT; 
                mask_pbmp = PBMP_PIPE(unit,instance);
                break;
            default: 
                return (BCM_E_INTERNAL);
        }
    }
           
    BCM_FIELD_QSET_INIT(fg.qset);
    if (soc_feature(unit,soc_feature_td3_style_proxy)) {
        /* TD3 does not support per-port IPBM, instead using
           device port will suffice in this scenario */
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyInPort);
    } else {
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyInPorts);
    }

    switch(proto_type) {
    case BCM_PROXY_PROTO_IP4_ALL:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyEtherType);
        break;
    case BCM_PROXY_PROTO_IP6_ALL:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyEtherType);
        break;
    case BCM_PROXY_PROTO_IP4_MCAST:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyDstIp);
        break;
    case BCM_PROXY_PROTO_IP6_MCAST:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyIpType);
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyDstIp6High);
        break;
    case BCM_PROXY_PROTO_MPLS_UCAST:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyEtherType);
        break;
    case BCM_PROXY_PROTO_MPLS_MCAST:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyEtherType);
        break;
    case BCM_PROXY_PROTO_MPLS_ALL:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyEtherType);
        break;
    case BCM_PROXY_PROTO_IP6_IN_IP4:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyIpProtocol);
        break;
    case BCM_PROXY_PROTO_IP_IN_IP:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyIpProtocol);
        break;
    case BCM_PROXY_PROTO_GRE_IN_IP:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyIpProtocol);
        break;
    case BCM_PROXY_PROTO_UNKNOWN_IP4_UCAST:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyEtherType);
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyPacketRes);
        break;
    /*    coverity[equality_cond]    */
    case BCM_PROXY_PROTO_UNKNOWN_IP6_UCAST:
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyEtherType);
        BCM_FIELD_QSET_ADD(fg.qset, bcmFieldQualifyPacketRes);
        break;
    default:
        return BCM_E_PARAM;
    }
    
    if (soc_feature(unit,soc_feature_td3_style_proxy)) {
        /* For TD3,ensure adding ASET at the group creation phase to avoid
           un-necessary usage of intraslice mode by default */
        BCM_FIELD_ASET_INIT(fg.aset);
        BCM_FIELD_ASET_ADD(fg.aset, bcmFieldActionRedirect);
    }
    BCM_IF_ERROR_RETURN(bcm_esw_field_group_config_create(unit, &fg));
    gid = fg.group;
   
    bcm_proxy_info->flags |= _BCM_PROXY_INFO_GROUP_CREATED;
    bcm_proxy_info->gid = gid;

    BCM_IF_ERROR_RETURN(bcm_esw_field_entry_create(unit, gid, &eid));
    bcm_proxy_info->flags |= _BCM_PROXY_INFO_ENTRY_CREATED;
    bcm_proxy_info->eid = eid;

    if (soc_feature(unit,soc_feature_td3_style_proxy)) {
        /* TD3 does not support per-port IPBM, instead using
          device port will suffice in this scenario */
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_InPort(unit, eid, client_port,
                                           BCM_FIELD_EXACT_MATCH_MASK));
    } else {
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_InPorts(unit, eid, ingress_pbmp,
                                           mask_pbmp));
    }

    switch(proto_type) {
    case BCM_PROXY_PROTO_IP4_ALL:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_EtherType(unit, eid, 0x0800, 0xffff));
        break;
    case BCM_PROXY_PROTO_IP6_ALL:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_EtherType(unit, eid, 0x86dd, 0xffff));
        break;
    case BCM_PROXY_PROTO_IP4_MCAST:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_DstIp(unit, eid, 0xe0000000, 0xf0000000));
        break;
    case BCM_PROXY_PROTO_IP6_MCAST:
        {
            bcm_ip6_t addr, mask;
            BCM_IF_ERROR_RETURN
                (bcm_esw_field_qualify_IpType(unit, eid, bcmFieldIpTypeIpv6));
            sal_memset(addr, 0, 16); /* BCM_IP6_ADDRLEN */
            addr[0] = 0xff;
            sal_memset(mask, 0, 16); /* BCM_IP6_ADDRLEN */
            mask[0] = 0xff;
            mask[1] = 0xff;
            BCM_IF_ERROR_RETURN
                (bcm_esw_field_qualify_DstIp6High(unit, eid, addr, mask));
            break;
        }

    case BCM_PROXY_PROTO_MPLS_UCAST:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_EtherType(unit, eid, 0x8847, 0xffff));
        break;
    case BCM_PROXY_PROTO_MPLS_MCAST:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_EtherType(unit, eid, 0x8848, 0xffff));
        break;
    case BCM_PROXY_PROTO_MPLS_ALL:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_EtherType(unit, eid, 0x8840, 0xfff0));
        break;
    case BCM_PROXY_PROTO_IP6_IN_IP4:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_IpProtocol(unit, eid, 0x29, 0xff));
        break;
    case BCM_PROXY_PROTO_IP_IN_IP:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_IpProtocol(unit, eid, 0x4, 0xff));
        break;
    case BCM_PROXY_PROTO_GRE_IN_IP:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_IpProtocol(unit, eid, 0x2f, 0xff));
        break;
    case BCM_PROXY_PROTO_UNKNOWN_IP4_UCAST:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_EtherType(unit, eid, 0x0800, 0xffff));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_PacketRes(unit, eid,
                                             BCM_FIELD_PKT_RES_L3UCUNKNOWN,
                                             0xf));
        break;
    case BCM_PROXY_PROTO_UNKNOWN_IP6_UCAST:
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_EtherType(unit, eid, 0x86dd, 0xffff));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_qualify_PacketRes(unit, eid,
                                             BCM_FIELD_PKT_RES_L3UCUNKNOWN,
                                             0xf));
        break;
        /* Defensive Default */
        /* coverity[dead_error_begin] */
    default:
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_field_action_add(unit, eid, bcmFieldActionRedirect, 
                                  server_modid, server_port));
    
     BCM_IF_ERROR_RETURN
         (bcm_esw_field_entry_install(unit, eid));
    
    bcm_proxy_info->flags |= _BCM_PROXY_INFO_INSTALLED;
    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
} 

/*
 * Function:
 *      _bcm_esw_proxy_client_uninstall_xgs3
 * Purpose:
 *      Removes and destroys proxy client FP resources
 * Parameters:
 *      unit           -  BCM Unit number
 *      bcm_proxy_info -  proxy data
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_proxy_client_uninstall_xgs3(int unit,
                                     _bcm_proxy_info_t *info)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    bcm_field_entry_t eid   = info->eid;
    bcm_field_group_t gid   = info->gid;
    int flags               = info->flags;

    if (flags & _BCM_PROXY_INFO_INSTALLED) {
        BCM_IF_ERROR_RETURN(bcm_esw_field_entry_remove(unit, eid));
    }

    if (flags & _BCM_PROXY_INFO_ENTRY_CREATED) {
        BCM_IF_ERROR_RETURN(bcm_esw_field_entry_destroy(unit, eid));
    }

    if (flags & _BCM_PROXY_INFO_GROUP_CREATED) {
        BCM_IF_ERROR_RETURN(bcm_esw_field_group_destroy(unit, gid));
    }

    return BCM_E_NONE;
#else
    return BCM_E_UNAVAIL;
#endif
}

/*
 * Function:
 *      _bcm_esw_proxy_client_install_xgs3
 * Purpose:
 *      Creates proxy client using FP rules, clean up on error
 * Parameters:
 *      unit           -  BCM Unit number
 *      bcm_proxy_info -  proxy data
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_proxy_client_install_xgs3(int unit,
                                   _bcm_proxy_info_t *info)
{
    int rv = BCM_E_INTERNAL;

    rv = _bcm_esw_proxy_client_create_xgs3(unit, info);

    if (BCM_FAILURE(rv)) {
        LOG_WARN(BSL_LS_BCM_L3,
                 (BSL_META_U(unit,
                             "Proxy: could not install client: %s\n"),
                  bcm_errmsg(rv)));
        /* Ignore errors from uninstall */
        _bcm_esw_proxy_client_uninstall_xgs3(unit, info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_proxy_client_uninstall
 * Purpose:
 *      Uninstall proxy client for all device families
 * Parameters:
 *      unit           -  BCM Unit number
 *      bcm_proxy_info -  proxy data
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_proxy_client_uninstall(int unit, _bcm_proxy_info_t *info)
{
    int rv = BCM_E_INTERNAL;

    if (SOC_IS_XGS3_SWITCH(unit)) {
        rv = _bcm_esw_proxy_client_uninstall_xgs3(unit, info);
    }

    if (BCM_SUCCESS(rv)) {
        _bcm_proxy_control[unit].num_clients--;
    }

    return rv;
}


/*
 * Function:
 *      _bcm_esw_proxy_client_install
 * Purpose:
 *      Install proxy client for all device families
 * Parameters:
 *      unit           -  BCM Unit number
 *      bcm_proxy_info -  proxy data
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_proxy_client_install(int unit,
                              _bcm_proxy_info_t *dst,
                              _bcm_proxy_info_t *src)
{
    int rv = BCM_E_INTERNAL;

    dst->client_port  = src->client_port;
    dst->proto_type   = src->proto_type;
    dst->server_modid = src->server_modid;
    dst->server_port  = src->server_port;
    dst->server_port  = src->server_port;

    if (SOC_IS_XGS3_SWITCH(unit)) {
        rv = _bcm_esw_proxy_client_install_xgs3(unit, dst);
    }

    if (BCM_SUCCESS(rv)) {
        _bcm_proxy_control[unit].num_clients++;
    }
    return rv;
}

/******************************************************************** Server */
/*
 * Function:
 *      bcmi_td3_proxy_server_config (internal function)
 * Purpose:
 *      Enables various kinds of lookups for packets coming from remote
 *      (proxy client) devices on the server side for TD3
 * Parameters:
 *      unit        -  BCM Unit number
 *      server_port -  Local port to which packets from remote devices are
 *                     destined to
 *      mode        -  Indicates lookup type
 *      enable      -  TRUE to enable lookups
 *                     FALSE to disable lookups
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_td3_proxy_server_config(int unit, bcm_port_t server_port,
                          bcm_proxy_mode_t mode, int enable)
{
    int ing_port = 0;
    uint32 mod = 0;
    static soc_field_t fields[5] = {VALIDf, DEST_MODULE_IDf, DEST_PORT_IDf, DEST_MODULE_ID_MASKf, DEST_PORT_ID_MASKf};
    uint32 values[5] , field_num = 5;
    bcm_module_t    modid_mask;
    bcm_gport_t     server_port_mask;
    /* Generalize chip specific memory name*/
    soc_mem_t port_table = ING_DEVICE_PORTm;
    soc_reg_t hg_reg = HG_PKT_SIGNATURE_1r;
    uint32 ptab[SOC_MAX_MEM_WORDS];
    sal_memset(ptab, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, port_table, MEM_BLOCK_ANY, server_port, ptab));
    if (BCM_PROXY_MODE_HIGIG == mode) {
       soc_mem_field32_set(unit, port_table, ptab,
                           HG_LOOKUP_ENABLEf, enable ? 1 : 0);
       soc_mem_field32_set(unit, port_table, ptab,
                           HYBRID_MODE_ENABLEf, 0);
    } else {
        /* BCM_PROXY_MODE_HYBRID */
        soc_mem_field32_set(unit, port_table, ptab,
                            HYBRID_MODE_ENABLEf, enable ? 1 : 0);
        soc_mem_field32_set(unit, port_table, ptab,
                            HG_LOOKUP_ENABLEf, 0);
    }
    soc_mem_field32_set(unit, port_table, ptab,
                        USE_MH_VIDf, enable ? 1 : 0);
    soc_mem_field32_set(unit, port_table, ptab,
                        USE_MH_PKT_PRIf, enable ? 1 : 0);

    BCM_IF_ERROR_RETURN
           (soc_mem_write(unit, port_table, MEM_BLOCK_ALL, server_port, ptab));
    /*
     * Program LPORT table for the HG port;
     * HIGIG_PACKET must not be set; MY_MODID must be
     * set appropriately, and V4/V6 enable bits
    */
    mod = soc_mem_field32_get(unit, port_table, ptab, MY_MODIDf);
    sal_memset(ptab, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    soc_LPORT_TABm_field32_set(unit, ptab, V4L3_ENABLEf,
                               enable ? 1 : 0);
    soc_LPORT_TABm_field32_set(unit, ptab, V6L3_ENABLEf,
                               enable ? 1 : 0);
    soc_LPORT_TABm_field32_set(unit, ptab, V4IPMC_ENABLEf,
                               enable ? 1 : 0);
    soc_LPORT_TABm_field32_set(unit, ptab, V6IPMC_ENABLEf,
                               enable ? 1 : 0);
    soc_LPORT_TABm_field32_set(unit, ptab, IPMC_DO_VLANf,
                               enable ? 1 : 0);
    soc_LPORT_TABm_field32_set(unit, ptab, FILTER_ENABLEf,
                               enable ? 1 : 0);

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, LPORT_TABm, MEM_BLOCK_ALL,
                       server_port, ptab));

    /* Set "magic destination" to this server port to
       match prior devices' behavior */
    modid_mask       = ((1 << soc_reg_field_length(unit, hg_reg, DEST_MODULE_ID_MASKf)) - 1);
    server_port_mask = ((1 << soc_reg_field_length(unit, hg_reg, DEST_PORT_ID_MASKf)) - 1);

    values[0] = enable;
    values[1] = mod;
    values[2] = server_port;
    values[3] = modid_mask;
    values[4] = server_port_mask;
    PBMP_HG_ITER(unit, ing_port) {
        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, hg_reg, ing_port,
                                     field_num, fields, values));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_proxy_server_retrieve
 * Purpose:
 *      Retrieves proxy related settings on the server side for TD3 device
 * Parameters:
 *      unit        - BCM Unit number
 *      server_port - Local port to which packets from remote devices are destined to
 *      mode        - proxy server mode
 *      enable      - (OUT) server status
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_td3_proxy_server_retrieve(int unit, bcm_port_t server_port,
                          bcm_proxy_mode_t mode, int *enable)
{
    uint64 hgld;
    int hg_enable;
    /* Generalize chip specific memory and reg name */
    soc_mem_t port_table = ING_DEVICE_PORTm;
    soc_reg_t hg_reg = HG_PKT_SIGNATURE_1r;
    uint32 ptab[SOC_MAX_MEM_WORDS];
    sal_memset(ptab, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

    BCM_IF_ERROR_RETURN
        (soc_reg64_get(unit, hg_reg, server_port, 0, &hgld));

    if (server_port == soc_reg64_field32_get(unit, hg_reg,hgld, DEST_PORT_IDf)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, port_table, MEM_BLOCK_ANY, server_port, ptab));
        if (BCM_PROXY_MODE_HIGIG == mode) {
            hg_enable = soc_mem_field32_get(unit, port_table, ptab, HG_LOOKUP_ENABLEf);
        } else {
            hg_enable = soc_mem_field32_get(unit, port_table, ptab, HYBRID_MODE_ENABLEf);
        }
        *enable = hg_enable;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_proxy_server_signature_match (internal function)
 * Purpose:
 *      Enables programming of the appropiate signature match register
 *      to enable proxy processing on the server device based on the
 *      type of application.(Hybrid mode or Bud-node proxy)
 * Parameters:
 *      unit - (IN) Unit number.
 *      local_ingress_port - (IN) Incoming Higig stack port
 *      proxy_server - (IN) Proxy server configuration
 *      hg_reg - (IN) Register used for singature match.
 *      enable - (IN) Enable proxy server on selected Higig stack port
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_td3_proxy_server_signature_match(int unit, bcm_gport_t local_ingress_port,
                              bcm_proxy_server_t *proxy_server, soc_reg_t hg_reg, int enable)
{
    bcm_module_t modid = 0, modid_mask = 0, default_modid_mask;
    bcm_port_t   server_port = 0, server_port_mask = 0, default_port_mask;
    uint32       field_num = 5;
    uint32       values[5] = {0};
    static       soc_field_t fields[5] = {VALIDf, DEST_MODULE_IDf, DEST_PORT_IDf,
        DEST_MODULE_ID_MASKf, DEST_PORT_ID_MASKf};

    default_modid_mask = ((1 << soc_reg_field_length(unit, hg_reg, DEST_MODULE_ID_MASKf)) - 1);
    default_port_mask  = ((1 << soc_reg_field_length(unit, hg_reg, DEST_PORT_ID_MASKf)) - 1);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_proxy_gport_resolve(unit, proxy_server->server_gport,
                                      &server_port, &modid, 0));

    /* Only valid flags are allowed */
    if (proxy_server->flags > (BCM_PROXY_SERVER_MASK_PORT | BCM_PROXY_SERVER_KEEP_PRIORITY |
        BCM_PROXY_SERVER_KEEP_VID | BCM_PROXY_SERVER_MASK_MODID)) {
       return BCM_E_PARAM;
    }
    /* Logic to flexibly mask the destination port and/or
     * destination modid while performing signature match */
    if (proxy_server->server_gport_mask == BCM_PROXY_SERVER_GPORT_ALL_MASK) {
        /* Use default full mask for modid and port unless appropriate flags are passed */
        modid_mask       = (proxy_server->flags & BCM_PROXY_SERVER_MASK_MODID) ? 0 : default_modid_mask;
        server_port_mask = (proxy_server->flags & BCM_PROXY_SERVER_MASK_PORT) ? 0 : default_port_mask;
    } else {
        modid_mask = BCM_GPORT_PROXY_MODID_GET(proxy_server->server_gport_mask);
        server_port_mask = BCM_GPORT_PROXY_PORT_GET(proxy_server->server_gport_mask);
        if (proxy_server->flags & BCM_PROXY_SERVER_MASK_MODID) {
            if (!(modid_mask == 0)) {
                /* When this flag is present, if at all user passes server_gport_mask;
                 * then modid_mask has to be zero. Otherwise it is PARAM Error*/
                return BCM_E_PARAM;
            }
        }
        if (proxy_server->flags & BCM_PROXY_SERVER_MASK_PORT) {
            if (!(server_port_mask == 0)) {
                /* When this flag is present, if at all user passes server_gport_mask;
                 * then server_port_mask has to be zero. Otherwise it is PARAM Error */
                return BCM_E_PARAM;
            }
        }
    }
    values[0] = enable;
    values[1] = modid;
    values[2] = server_port;
    values[3] = modid_mask;
    values[4] = server_port_mask;

    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, hg_reg, local_ingress_port,
                                 field_num, fields, values));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_map_hg_port_to_index(internal function)
 * Purpose:
 *      Creates a mapping of all the HiGig ports to a corresponding index
 *      in an arrya of HiGig ports. Useful in allocating optimal
 *      memory while allocating book keeping structures.
 * Parameters:
 *      unit - (IN) Unit number.
 *      local_ingress_port - (IN) Incoming Higig stack port
 *      mapped_index - (OUT) Index which points to the location of the
 *      localingress port in an arry of HG ports
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_map_hg_port_to_index(int unit, bcm_gport_t local_ingress_port, int *mapped_index)
{
    int index = 0;
    int i = 0;
    int ing_port = 0;
    int hg_size;
    int *hg_array = NULL;

    BCM_PBMP_COUNT(PBMP_HG_ALL(unit), hg_size);
    hg_array = sal_alloc(sizeof(int) * hg_size, "Hg Array");
    if (hg_array == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(hg_array, 0, sizeof(int) * hg_size);

    PBMP_HG_ITER(unit, ing_port) {
        hg_array[index] = ing_port;
        index++;
    }
    for (i = 0; i < hg_size; i++) {
        if (hg_array[i] == local_ingress_port) {
            *mapped_index = i;
        }
    }
    sal_free(hg_array);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_proxy_server_port_config
 * Purpose:
 *      Enables various kinds of lookups on TD3 device on
 *      behalf of remote legacy or XGS3 devices on a per ingress port
 *      basis.  The local ingress port is expected to be a Higig stack
 *      port.  The server_gport in the proxy_server structure is the
 *      target BCM_GPORT_PROXY (modid,port) destination which
 *      indicates that hybrid proxy lookup is required.
 * Parameters:
 *      unit - (IN) Unit number.
 *      local_ingress_port - (IN) Incoming Higig stack port
 *      proxy_server - (IN) Proxy server configuration
 *      enable - (IN) Enable proxy server on selected Higig stack port
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_td3_proxy_server_port_config(int unit, bcm_gport_t local_ingress_port,
                              bcm_proxy_server_t *proxy_server,
                              int enable)
{
    bcm_gport_t  server_gport = 0, server_gport_mask = 0;
    int          i = 0;
    int          mapped_index = 0;
    int          cpu_port = SOC_INFO(unit).cpu_hg_index;
    /* Generalize chip specific memory and reg name */
    soc_mem_t port_table = ING_DEVICE_PORTm;
    soc_reg_t hg_reg = HG_PKT_SIGNATURE_1r;
    static uint32 reg_val_legacy[_BCM_PROXY_RESERVED_DGLP_NUMBER] = {0};
    static uint32 flow_type[_BCM_PROXY_RESERVED_DGLP_NUMBER] = {0};
    static soc_reg_t config_regs[] = {
        HG_PKT_SIGNATURE_2r,
        HG_PKT_SIGNATURE_3r
    };
    /* Use legacy registers for Bud node proxy. Will be handled by CCH
       to map into appropriate TD3 programming */
    static soc_reg_t config_regs_legacy[] = {
        ING_PACKET_PROCESSING_CONTROL_0r,
        ING_PACKET_PROCESSING_CONTROL_1r
    };
    static soc_field_t config_reg_fields[] = {
        ING_PACKET_PROCESSING_ENABLE_0f,
        ING_PACKET_PROCESSING_ENABLE_1f
    };
    uint32       ptab[SOC_MAX_MEM_WORDS];
    uint64       const0_64 = COMPILER_64_INIT(0,0);
    sal_memset(ptab, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, port_table, MEM_BLOCK_ANY,
                local_ingress_port, ptab));
    if (enable) {
        server_gport = proxy_server->server_gport;
        if (!BCM_GPORT_IS_PROXY(server_gport)) {
            /* The server gport is required to be in
             * BCM_GPORT_PROXY format */
            return BCM_E_PORT;
        }
        server_gport_mask = proxy_server->server_gport_mask;
        if (!(server_gport_mask == BCM_PROXY_SERVER_GPORT_ALL_MASK) && !BCM_GPORT_IS_PROXY(server_gport_mask)) {
            /* The server gport mask; when specified;
             * is required to be in BCM_GPORT_PROXY format
             */
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (bcmi_map_hg_port_to_index(unit, local_ingress_port, &mapped_index));
        if (BCM_PROXY_MODE_HIGIG == proxy_server->mode) {
            /* BCM_PROXY_MODE_HIGIG */
            soc_mem_field32_set(unit, port_table, ptab,	HG_LOOKUP_ENABLEf, 1);
            soc_mem_field32_set(unit, port_table, ptab,	HYBRID_MODE_ENABLEf, 0);
        } else if (BCM_PROXY_MODE_HYBRID == proxy_server->mode) {
            /* BCM_PROXY_MODE_HYBRID */
            soc_mem_field32_set(unit, port_table, ptab,	HG_LOOKUP_ENABLEf, 0);
            soc_mem_field32_set(unit, port_table, ptab,	HYBRID_MODE_ENABLEf, 1);
            BCM_IF_ERROR_RETURN
                (bcmi_td3_proxy_server_signature_match(unit, local_ingress_port,
                                                       proxy_server, hg_reg, enable));
            /* Maintain software state of proxy server in third instance of proxy_bookkeep */
            sal_memcpy(&proxy_bookkeep[unit].proxy_server_copy[_BCM_PROXY_HYBRID][mapped_index],
                    proxy_server, sizeof(bcm_proxy_server_t));
        } else if (BCM_PROXY_MODE_SECOND_PASS == proxy_server->mode) {
            if (!soc_feature(unit, soc_feature_hg_proxy_second_pass)) {
                return BCM_E_UNAVAIL;
            }
            if ((proxy_server->flow_type > bcmProxySecondPassFlowTypeTrillTermination) ||
                    (proxy_server->flow_type < bcmProxySecondPassFlowTypeNone)) {
                return BCM_E_PARAM;
            }
            /* If exist, update the entry*/
            for(i = 0; i < _BCM_PROXY_RESERVED_DGLP_NUMBER; i++) {
                /* Flow type is defaulted to "NONE" in CANCUN. CCH is used at SOC layer to overwrite it */
                /* Retaining the legacy register and field names at BCM layer */
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, config_regs_legacy[i], REG_PORT_ANY,
                                   0, &reg_val_legacy[i]));
                flow_type[i] = soc_reg_field_get(unit,config_regs_legacy[i],
                                              reg_val_legacy[i], FLOW_TYPEf);
                if (proxy_server->flow_type == flow_type[i]) {
                    soc_reg_field_set(unit,config_regs_legacy[i],
                            &reg_val_legacy[i], FLOW_TYPEf, proxy_server->flow_type);
                    BCM_IF_ERROR_RETURN
                        (bcmi_td3_proxy_server_signature_match(unit, local_ingress_port,
                                                               proxy_server, config_regs[i], enable));
                    /* Maintain software state of proxy server in first two instances of proxy_bookkeep */
                    sal_memcpy(&proxy_bookkeep[unit].proxy_server_copy[i][mapped_index],
                            proxy_server, sizeof(bcm_proxy_server_t));

                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, config_regs_legacy[i], REG_PORT_ANY,
                                       0, reg_val_legacy[i]));

                    soc_mem_field32_set(unit, port_table, ptab, config_reg_fields[i], 1);
                    soc_mem_field32_set(unit, port_table, ptab, HG_LOOKUP_ENABLEf, 0);
                    soc_mem_field32_set(unit, port_table, ptab, REMOVE_MH_SRC_PORTf, 1);

                    BCM_IF_ERROR_RETURN
                        (soc_mem_write(unit, port_table, MEM_BLOCK_ALL, local_ingress_port, ptab));
                    return BCM_E_NONE;
                }
            }

            /* if not exist, create it */
            for(i = 0; i < _BCM_PROXY_RESERVED_DGLP_NUMBER; i++) {
                if (bcmProxySecondPassFlowTypeNone == flow_type[i]) {
                    soc_reg_field_set(unit,config_regs_legacy[i],
                            &reg_val_legacy[i], FLOW_TYPEf, proxy_server->flow_type);
                    BCM_IF_ERROR_RETURN
                        (bcmi_td3_proxy_server_signature_match(unit, local_ingress_port,
                                                               proxy_server, config_regs[i], enable));
                    /* Maintain software state of proxy server in first two instances of proxy_bookkeep */
                    sal_memcpy(&proxy_bookkeep[unit].proxy_server_copy[i][mapped_index],
                            proxy_server, sizeof(bcm_proxy_server_t));

                    SOC_IF_ERROR_RETURN
                        (soc_reg32_set(unit, config_regs_legacy[i], REG_PORT_ANY,
                                       0, reg_val_legacy[i]));

                    soc_mem_field32_set(unit, port_table, ptab, config_reg_fields[i], 1);
                    soc_mem_field32_set(unit, port_table, ptab, HG_LOOKUP_ENABLEf, 0);
                    soc_mem_field32_set(unit, port_table, ptab, REMOVE_MH_SRC_PORTf, 1);

                    BCM_IF_ERROR_RETURN
                        (soc_mem_write(unit, port_table, MEM_BLOCK_ALL, local_ingress_port, ptab));
                    return BCM_E_NONE;
                }
            }
            return BCM_E_FULL;
        } else {
            /* We don't support BCM_PROXY_MODE_LOOPBACK in this per-port
             * version of the API.
             */
            return BCM_E_PARAM;
        }
        /* Keeping TD3 code consistent with other devices in the
         * interpretation of the two flags for using info from module header */
        soc_mem_field32_set(unit, port_table, ptab, USE_MH_VIDf,
                proxy_server->flags & BCM_PROXY_SERVER_KEEP_VID ? 0 : 1);
        soc_mem_field32_set(unit, port_table, ptab, USE_MH_PKT_PRIf,
                proxy_server->flags & BCM_PROXY_SERVER_KEEP_PRIORITY ? 0 : 1);

        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, port_table, MEM_BLOCK_ALL, local_ingress_port, ptab));

        if (IS_CPU_PORT(unit, local_ingress_port) &&
            (BCM_PROXY_MODE_HYBRID == proxy_server->mode)) {
            sal_memset(ptab, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

            BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, port_table, MEM_BLOCK_ANY, cpu_port, ptab));
            soc_mem_field32_set(unit, port_table, ptab, HG_LOOKUP_ENABLEf, 0);
            soc_mem_field32_set(unit, port_table, ptab, HYBRID_MODE_ENABLEf, 1);
            soc_mem_field32_set(unit, port_table, ptab, USE_MH_VIDf,
                proxy_server->flags & BCM_PROXY_SERVER_KEEP_VID ? 0 : 1);
            soc_mem_field32_set(unit, port_table, ptab, USE_MH_PKT_PRIf,
                proxy_server->flags & BCM_PROXY_SERVER_KEEP_PRIORITY ? 0 : 1);
            BCM_IF_ERROR_RETURN(soc_mem_write(
                unit, port_table, MEM_BLOCK_ALL, cpu_port, ptab));
        }
    } else {
        /* when enable is 0 */
        if (NULL == proxy_server) {
            return BCM_E_PARAM;
        }
        if (BCM_PROXY_MODE_SECOND_PASS == proxy_server->mode) {
            if (soc_feature(unit, soc_feature_hg_proxy_second_pass)) {

                if ((proxy_server->flow_type > bcmProxySecondPassFlowTypeTrillTermination) ||
                        (proxy_server->flow_type < bcmProxySecondPassFlowTypeNone)) {
                    return BCM_E_PARAM;
                }
                for(i = 0; i < _BCM_PROXY_RESERVED_DGLP_NUMBER; i++) {
                    /* Use legacy register and field names at BCM layer.
                     * CCH is used at SOC layer to get the corresponding values */
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, config_regs_legacy[i], REG_PORT_ANY,
                                       0, &reg_val_legacy[i]));
                    flow_type[i] = soc_reg_field_get(unit,config_regs_legacy[i],
                            reg_val_legacy[i], FLOW_TYPEf);

                    if (proxy_server->flow_type == flow_type[i]) {
                        int dglp_is_shared = 0;
                        bcm_gport_t local_port = 0;
                        uint32 return_val[SOC_MAX_MEM_WORDS];
                        sal_memset(return_val, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

                        soc_mem_field32_set(unit, port_table, ptab, config_reg_fields[i], 0);
                        soc_mem_field32_set(unit, port_table, ptab, USE_MH_VIDf, 0);
                        soc_mem_field32_set(unit, port_table, ptab, USE_MH_PKT_PRIf, 0);
                        soc_mem_field32_set(unit, port_table, ptab, REMOVE_MH_SRC_PORTf, 0);

                        BCM_IF_ERROR_RETURN
                            (soc_mem_write(unit, port_table, MEM_BLOCK_ALL, local_ingress_port, ptab));
                        BCM_PBMP_ITER(PBMP_ALL(unit),local_port) {
                            BCM_IF_ERROR_RETURN
                                (soc_mem_read(unit, port_table, MEM_BLOCK_ANY, local_port, return_val));
                            if (soc_mem_field32_get(unit, port_table, return_val, config_reg_fields[i])) {
                                dglp_is_shared = 1;
                            }
                        }
                        if (!dglp_is_shared) {
                            soc_reg_field_set(unit,config_regs_legacy[i],
                                    &reg_val_legacy[i], FLOW_TYPEf,0);

                            SOC_IF_ERROR_RETURN
                                (soc_reg32_set(unit, config_regs_legacy[i], REG_PORT_ANY, 0, reg_val_legacy[i]));
                            SOC_IF_ERROR_RETURN
                                (soc_reg64_set(unit, config_regs[i], REG_PORT_ANY, 0, const0_64));
                        }
                        return BCM_E_NONE;
                    }
                }
                return BCM_E_NOT_FOUND;
            } else {
                return BCM_E_UNAVAIL;
            }
        }
        /* Clear server configuration, but retain the independent
         * source port knockout configuration REMOVE_MH_SRC_PORTf
         */
        soc_mem_field32_set(unit, port_table, ptab, HG_LOOKUP_ENABLEf, 0);
        soc_mem_field32_set(unit, port_table, ptab, HYBRID_MODE_ENABLEf, 0);
        soc_mem_field32_set(unit, port_table, ptab, USE_MH_PKT_PRIf, 0);
        soc_mem_field32_set(unit, port_table, ptab, USE_MH_VIDf, 0);
        for(i = 0; i < _BCM_PROXY_RESERVED_DGLP_NUMBER; i++) {
            soc_mem_field32_set(unit, port_table, ptab, config_reg_fields[i], 0);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, port_table, MEM_BLOCK_ALL, local_ingress_port, ptab));

        if (IS_CPU_PORT(unit, local_ingress_port) &&
            (BCM_PROXY_MODE_HYBRID == proxy_server->mode)) {
            sal_memset(ptab, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

            BCM_IF_ERROR_RETURN(soc_mem_read(
                unit, port_table, MEM_BLOCK_ANY, cpu_port, ptab));
            soc_mem_field32_set(unit, port_table, ptab, HG_LOOKUP_ENABLEf, 0);
            soc_mem_field32_set(unit, port_table, ptab, HYBRID_MODE_ENABLEf, 0);
            soc_mem_field32_set(unit, port_table, ptab, USE_MH_PKT_PRIf, 0);
            soc_mem_field32_set(unit, port_table, ptab, USE_MH_VIDf, 0);
            BCM_IF_ERROR_RETURN(soc_mem_write(
                unit, port_table, MEM_BLOCK_ALL, cpu_port, ptab));
        }

        /* Clear the HG_PKT_SINGATURE_1 config */
        SOC_IF_ERROR_RETURN
            (soc_reg64_set(unit, hg_reg, local_ingress_port, 0, const0_64));
    }
    /* End of else loop (enable is 0) */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_td3_proxy_server_port_retrieve
 * Purpose:
 *      Retrieves the current proxy server configuration on a
 *      given Higig stack port in TD3, if any.
 * Parameters:
 *      unit - (IN) Unit number.
 *      local_ingress_port - (IN) Incoming Higig stack port
 *      proxy_server - (OUT) Proxy server configuration
 *      enable - (OUT) Enable proxy server on selected Higig stack port
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
bcmi_td3_proxy_server_port_retrieve(int unit, bcm_gport_t local_ingress_port,
                                    bcm_proxy_server_t *proxy_server,
                                    int *enable)
{
    uint32          hglkup, hybrid;
    int             i = 0;
    int             mapped_index = 0;
    /* Generalize chip specific memory and reg name */
    soc_mem_t port_table = ING_DEVICE_PORTm;
    static uint32 reg_val_legacy[_BCM_PROXY_RESERVED_DGLP_NUMBER] = {0};
    static uint32 flow_type[_BCM_PROXY_RESERVED_DGLP_NUMBER] = {0};
    static soc_reg_t config_regs_legacy[] = {
        ING_PACKET_PROCESSING_CONTROL_0r,
        ING_PACKET_PROCESSING_CONTROL_1r
    };
    static soc_field_t config_reg_fields[] = {
        ING_PACKET_PROCESSING_ENABLE_0f,
        ING_PACKET_PROCESSING_ENABLE_1f
    };
    uint32          ptab[SOC_MAX_MEM_WORDS];
    sal_memset(ptab, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, port_table, MEM_BLOCK_ANY, local_ingress_port, ptab));
    BCM_IF_ERROR_RETURN
        (bcmi_map_hg_port_to_index(unit, local_ingress_port, &mapped_index));
    if (BCM_PROXY_MODE_SECOND_PASS == proxy_server->mode) {
        if (!soc_feature(unit, soc_feature_hg_proxy_second_pass)) {
            return BCM_E_UNAVAIL;
        }

        for(i = 0; i < _BCM_PROXY_RESERVED_DGLP_NUMBER; i++) {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, config_regs_legacy[i], REG_PORT_ANY, 0, &reg_val_legacy[i]));
            flow_type[i] = soc_reg_field_get(unit, config_regs_legacy[i], reg_val_legacy[i], FLOW_TYPEf);

            if (proxy_server->flow_type == flow_type[i]) {
                /* Get proxy server information from software state */
                sal_memcpy(proxy_server, &proxy_bookkeep[unit].proxy_server_copy[i][mapped_index],
                        sizeof(bcm_proxy_server_t));
                *enable = soc_mem_field32_get(unit, port_table, ptab, config_reg_fields[i]);
                return BCM_E_NONE;
            }
        }
        return BCM_E_NOT_FOUND;
    }
    hglkup = soc_mem_field32_get(unit, port_table, ptab, HG_LOOKUP_ENABLEf);
    hybrid = soc_mem_field32_get(unit, port_table, ptab, HYBRID_MODE_ENABLEf);

    if (hglkup) {
        *enable = TRUE;
        proxy_server->mode = BCM_PROXY_MODE_HIGIG;
        /* Keeping TD3 code consistent with other devices in the
         * interpretation of the two flags for using info from module header */
        if (!soc_mem_field32_get(unit, port_table, ptab, USE_MH_VIDf)) {
            proxy_server->flags |= BCM_PROXY_SERVER_KEEP_VID;
        }

        if (!soc_mem_field32_get(unit, port_table, ptab, USE_MH_PKT_PRIf)) {
            proxy_server->flags |= BCM_PROXY_SERVER_KEEP_PRIORITY;
        }
    } else {
        if (hybrid) {
            *enable = TRUE;
            /*mode = BCM_PROXY_MODE_HYBRID;*/
            /* Get proxy server information from software state */
            sal_memcpy(proxy_server, &proxy_bookkeep[unit].proxy_server_copy[_BCM_PROXY_HYBRID][mapped_index],
                    sizeof(bcm_proxy_server_t));
        } else {
            *enable = FALSE;
            /* Nothing else to do */
            return BCM_E_NONE;
        }
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      bcmi_proxy_server_wb_alloc
 * Purpose:
 *      Allocates persisitent memory for Warm Boot scache.
 * Parameters:
 *      unit - Device number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_proxy_server_wb_alloc(int unit)
{
    int rv = BCM_E_NONE;
    int port_size;
    uint8 *proxy_server_scache;
    soc_scache_handle_t scache_handle;

    BCM_PBMP_COUNT(PBMP_HG_ALL(unit), port_size);
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_PROXY, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                port_size * sizeof(bcm_proxy_server_t),
                &proxy_server_scache,
                BCM_PROXY_WB_DEFAULT_VERSION, NULL);
        if (BCM_E_NOT_FOUND == rv) {
            /* Proceed with Level 1 Warm Boot */
            rv = BCM_E_NONE;
        }
    return rv;
}

/*
 * Function:
 *      bcmi_proxy_server_sync
 * Purpose:
 *      Stores proxy_server_gport and proxy_server_gport_mask information
 *      in scache so that it can be recovered on warm boot
 * Parameters:
 *      unit - Device number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_proxy_server_sync(int unit)
{
    int alloc_size, port_size, idx = 0;
    soc_scache_handle_t scache_handle;
    uint8               *proxy_server_scache;

    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
        return BCM_E_NONE;
    }
    BCM_PBMP_COUNT(PBMP_HG_ALL(unit), port_size);
    alloc_size = port_size * sizeof(bcm_proxy_server_t);

    CHECK_PROXY_INIT(unit);
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_PROXY, 0);
    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 alloc_size * _BCM_PROXY_SERVER_INSTANCES, &proxy_server_scache,
                                 BCM_PROXY_WB_DEFAULT_VERSION, NULL));
    for (idx = 0; idx < _BCM_PROXY_SERVER_INSTANCES; idx++) {
        /* copy proxy_server instance into scache and point to the next location */
        sal_memcpy(proxy_server_scache,
                proxy_bookkeep[unit].proxy_server_copy[idx], alloc_size);
        proxy_server_scache += alloc_size;
    }
    return BCM_E_NONE;
}

STATIC int
bcmi_proxy_server_recover(int unit)
{
    int alloc_size, port_size, idx = 0, rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8               *proxy_server_scache;

    BCM_PBMP_COUNT(PBMP_HG_ALL(unit), port_size);
    alloc_size = port_size * sizeof(bcm_proxy_server_t);

    /* We can't distinguish boot path for level 1 and level 2 warmboot
     * HR3 is the only chip we need to run level 1 warmboot which
     * means we don't do "exit clean" and not configuring any scache info
     *
     * Work around here is for HR3 only - We return from here
     * if this is level 1 warmboot by checking whether user configures
     * the scache_file
     *
     * Test case to verify this fix is: AT_IFP_wb_lvl1_005
     */
    if (SOC_IS_HURRICANEX(unit)) {
        if ((NULL == soc_property_get_str(unit, spn_STABLE_FILENAME)) &&
            (NULL == soc_property_get_str(unit, "scache_filename"))) {
            return BCM_E_NONE;
        }
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_PROXY, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
              alloc_size * _BCM_PROXY_SERVER_INSTANCES, &proxy_server_scache,
              BCM_PROXY_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } else if (rv == BCM_E_NOT_FOUND) {
        return bcmi_proxy_server_wb_alloc(unit);
    }

    for (idx = 0; idx < _BCM_PROXY_SERVER_INSTANCES; idx++) {
        /* copy scache data into proxy_server instance and point to the next location */
        sal_memcpy(proxy_bookkeep[unit].proxy_server_copy[idx],
                proxy_server_scache, alloc_size);
        proxy_server_scache += alloc_size;
    }
    return BCM_E_NONE;
}
#endif /*BCM_WARM_BOOT_SUPPORT*/
/*
 * Function:
 *      _bcm_esw_proxy_server_set
 * Purpose:
 *      Enables various kinds of lookups for packets coming from remote
 *      (proxy client) devices (internal function)
 * Parameters:
 *      unit        -  BCM Unit number
 *      server_port -  Local port to which packets from remote devices are 
 *                     destined to
 *      mode        -  Indicates lookup type
 *      enable      -  TRUE to enable lookups
 *                     FALSE to disable lookups
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_proxy_server_set(int unit, bcm_port_t server_port, 
                          bcm_proxy_mode_t mode, int enable)
{
    
    if (BCM_GPORT_IS_SET(server_port)) {
        bcm_module_t    modid;
        BCM_IF_ERROR_RETURN(
            _bcm_esw_proxy_gport_resolve(unit, server_port,
                                         &server_port, &modid, 1));
    }
    
    if (IS_HG_PORT(unit, server_port) || IS_CPU_PORT(unit, server_port)) {
        if ((mode != BCM_PROXY_MODE_HIGIG) && (mode != BCM_PROXY_MODE_HYBRID)) {
            return BCM_E_PARAM;
        }
           
#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_FBX(unit)) {
            uint32 dst_bmp;
            bcm_pbmp_t pbmp;
            uint32 val, mod = 0;
            int lport_idx;
            port_tab_entry_t ptab;
            
            if (!soc_feature(unit, soc_feature_higig_lookup)) {
                return BCM_E_UNAVAIL;
            }

            /*
             * Program PORT table V4L3_ENABLE, V6L3_ENABLE;
             * required for IP-IP tunneling on XGS3 devices.
             * This is not necessary for XGS4 and later.
             */

            
            if (!SOC_IS_TRX(unit)) {
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
                                  server_port, &ptab));
                soc_PORT_TABm_field32_set(unit, &ptab, V4L3_ENABLEf,
                                          enable ? 1 : 0);
                soc_PORT_TABm_field32_set(unit, &ptab, V6L3_ENABLEf,
                                          enable ? 1 : 0);
                BCM_IF_ERROR_RETURN
                    (soc_mem_write(unit, PORT_TABm, MEM_BLOCK_ALL,
                                   server_port, &ptab));
            }

            if (soc_feature(unit,soc_feature_td3_style_proxy)) {
               /* Program relevant TD3 tables and registers */
               BCM_IF_ERROR_RETURN(bcmi_td3_proxy_server_config(unit, server_port, mode, enable));
            } else {
                /*
                 * Program LPORT table for the HG port;
                 * HIGIG_PACKET must not be set; MY_MODID must be
                 * set appropriately, and V4/V6 enable bits
                 */
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, PORT_TABm, MEM_BLOCK_ANY,
                                  server_port, &ptab));
                if (soc_mem_field_valid(unit, PORT_TABm, MY_MODIDf)) {
                    mod = soc_PORT_TABm_field32_get(unit, &ptab, MY_MODIDf);
                }
                
                /* For Triumph, we use entry 0 so all remote ports
                 * will use this entry.  This is the behavior for previous
                 * devices 
                 */
                lport_idx = SOC_IS_TR_VL(unit) ? 0 : server_port;
                
                sal_memset(&ptab, 0, sizeof(port_tab_entry_t));
                BCM_IF_ERROR_RETURN
                        (soc_mem_read(unit, LPORT_TABm, MEM_BLOCK_ANY,
                                      lport_idx, &ptab));                
                if (soc_mem_field_valid(unit, LPORT_TABm, MY_MODIDf)) {
                    soc_LPORT_TABm_field32_set(unit, &ptab, MY_MODIDf, mod);
                }
                soc_LPORT_TABm_field32_set(unit, &ptab, V4L3_ENABLEf,
                                           enable ? 1 : 0);
                soc_LPORT_TABm_field32_set(unit, &ptab, V6L3_ENABLEf,
                                           enable ? 1 : 0);
                soc_LPORT_TABm_field32_set(unit, &ptab, V4IPMC_ENABLEf,
                                           enable ? 1 : 0);
                soc_LPORT_TABm_field32_set(unit, &ptab, V6IPMC_ENABLEf,
                                           enable ? 1 : 0);
                soc_LPORT_TABm_field32_set(unit, &ptab, IPMC_DO_VLANf,
                                           enable ? 1 : 0);
                soc_LPORT_TABm_field32_set(unit, &ptab, FILTER_ENABLEf,
                                           enable ? 1 : 0);

                BCM_IF_ERROR_RETURN
                    (soc_mem_write(unit, LPORT_TABm, MEM_BLOCK_ALL,
                                   lport_idx, &ptab));

                SOC_IF_ERROR_RETURN(READ_IHG_LOOKUPr
                                    (unit, server_port, &val));
                if (BCM_PROXY_MODE_HIGIG == mode) {
                    /* BCM_PROXY_MODE_HIGIG */
                    soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                            HG_LOOKUP_ENABLEf, enable ? 1: 0);
                    soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                            HYBRID_MODE_ENABLEf, 0);
                } else {
                    /* BCM_PROXY_MODE_HYBRID */
                    soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                            HG_LOOKUP_ENABLEf, 0);
                    soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                            HYBRID_MODE_ENABLEf, enable ? 1: 0);
                }
                if (SOC_IS_TRX(unit)) {
                    int ing_port;
                    soc_reg_t reg;
                    static soc_field_t fields[2] = { DST_MODIDf, DST_PORTf };
                    uint32 values[2];
                    /* Set "magic destination" to this server port to
                       match prior devices' behavior */
                    reg = HG_LOOKUP_DESTINATIONr;
                    values[0] = mod;
                    values[1] = server_port;
                    if (SOC_REG_INFO(unit, reg).regtype == soc_portreg) {
                        PBMP_HG_ITER(unit, ing_port) {
                            SOC_IF_ERROR_RETURN
                                (soc_reg_fields32_modify(unit, reg, ing_port,
                                                         2, fields, values));
                        }
                    } else {
                        SOC_IF_ERROR_RETURN
                            (soc_reg_fields32_modify(unit, reg, REG_PORT_ANY,
                                                     2, fields, values));
                    }
                } else {
                    BCM_PBMP_PORT_SET(pbmp, server_port);
                    dst_bmp = SOC_PBMP_WORD_GET(pbmp, 0);
                    dst_bmp >>= SOC_HG_OFFSET(unit);
                    soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                      DST_HG_LOOKUP_BITMAPf, dst_bmp);
                }
                if (SOC_REG_FIELD_VALID(unit, IHG_LOOKUPr,
                                        LOOKUP_WITH_MH_SRC_PORTf)) {
                    soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                      LOOKUP_WITH_MH_SRC_PORTf, enable ? 1 :0);
                }
                if (SOC_REG_FIELD_VALID(unit, IHG_LOOKUPr,
                                        USE_MH_INTERNAL_PRIf)) {
                    soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                      USE_MH_INTERNAL_PRIf, enable ? 1: 0);
                }
                soc_reg_field_set(unit, IHG_LOOKUPr, &val, USE_MH_VIDf,
                                  enable ? 1: 0);
                soc_reg_field_set(unit, IHG_LOOKUPr, &val, USE_MH_PKT_PRIf,
                                  enable ? 1: 0);
                SOC_IF_ERROR_RETURN(WRITE_IHG_LOOKUPr(unit, server_port, val));
            }
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
     } else {

         if (mode != BCM_PROXY_MODE_LOOPBACK) {
             return BCM_E_PARAM;
         }

         if (enable) {
             BCM_IF_ERROR_RETURN
                 (bcm_esw_port_loopback_set(unit, server_port,
                                            BCM_PORT_LOOPBACK_MAC));
	 } else {
             BCM_IF_ERROR_RETURN
                 (bcm_esw_port_loopback_set(unit, server_port,
                                            BCM_PORT_LOOPBACK_NONE));
         }
     }
     return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_proxy_server_get
 * Purpose:
 *      Get the lookup mode of XGS3 device.
 * Parameters:
 *      unit        -  BCM Unit number
 *      server_port -  Local port to which packets from remote devices are 
 *                     destined to
 *      mode        -  proxy server mode
 *      enable      -  (OUT) server status
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_proxy_server_get(int unit, bcm_port_t server_port, 
                         bcm_proxy_mode_t mode, int *enable)
{
    int rv = BCM_E_NONE;
    *enable = FALSE;

    if (BCM_GPORT_IS_SET(server_port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, server_port, &server_port));
    }

    if (!SOC_PORT_VALID(unit, server_port)) {
        return BCM_E_PORT;
    }

    if (!(IS_HG_PORT(unit, server_port) || IS_CPU_PORT(unit, server_port))) {
        int status;

	if (mode != BCM_PROXY_MODE_LOOPBACK) {
	    return BCM_E_PARAM;
	}
    /*    coverity[uninit_use_in_call : FALSE]    */
	    
        rv = bcm_esw_port_loopback_get(unit, server_port, &status);
        if (BCM_SUCCESS(rv)) {
            if (status == BCM_PORT_LOOPBACK_MAC ||
                status == BCM_PORT_LOOPBACK_PHY) {
                *enable = TRUE;
            } 
        }
    } else {

        if ((mode != BCM_PROXY_MODE_HIGIG) && (mode != BCM_PROXY_MODE_HYBRID)) {
	        return BCM_E_PARAM;
        }

#ifdef BCM_XGS3_SWITCH_SUPPORT
	if (soc_feature(unit, soc_feature_higig_lookup)) {
            int port, hg_enable;
            uint32 val, dst_bmp;
            bcm_pbmp_t pbmp, r_pbmp;
            soc_reg_t reg;

            if (SOC_IS_TRX(unit)) {
                uint32 hgld;
                if (soc_feature(unit,soc_feature_td3_style_proxy)) {
                   BCM_IF_ERROR_RETURN(bcmi_td3_proxy_server_retrieve(unit, server_port, mode, enable));
                } else {
                    /* Set "magic destination" to this server port to
                       match prior devices' behavior */
                    reg = HG_LOOKUP_DESTINATIONr;
                    if (SOC_REG_INFO(unit, reg).regtype == soc_portreg) {
                        SOC_IF_ERROR_RETURN
                                (soc_reg32_get(unit, reg, server_port, 0, &hgld));
                    } else {
                        SOC_IF_ERROR_RETURN
                            (READ_HG_LOOKUP_DESTINATIONr(unit, &hgld));
                    }
                    if (server_port == soc_reg_field_get(unit,
                                                        HG_LOOKUP_DESTINATIONr,
                                                        hgld,
                                                        DST_PORTf)) {
                        SOC_IF_ERROR_RETURN
                            (READ_IHG_LOOKUPr(unit, server_port, &val));
                        if (BCM_PROXY_MODE_HIGIG == mode) {
                            hg_enable = soc_reg_field_get(unit, IHG_LOOKUPr, val,
                                                          HG_LOOKUP_ENABLEf);
                        } else {
                            hg_enable = soc_reg_field_get(unit, IHG_LOOKUPr, val,
                                                          HYBRID_MODE_ENABLEf);
                        }
                        *enable = hg_enable;
                    }
                }
            } else {
                PBMP_HG_ITER(unit, port) {
                    SOC_PBMP_CLEAR(r_pbmp);
                    BCM_PBMP_PORT_SET(pbmp, server_port);

                    SOC_IF_ERROR_RETURN(READ_IHG_LOOKUPr(unit, port, &val));
                    hg_enable = soc_reg_field_get(unit, IHG_LOOKUPr, val,
                                                  HG_LOOKUP_ENABLEf);
                    dst_bmp = soc_reg_field_get(unit, IHG_LOOKUPr, val,
                                                DST_HG_LOOKUP_BITMAPf);
                    dst_bmp <<= SOC_HG_OFFSET(unit);
                    SOC_PBMP_WORD_SET(r_pbmp, 0, dst_bmp);
                    BCM_PBMP_AND(pbmp, r_pbmp);

                    if (hg_enable && BCM_PBMP_EQ(pbmp, r_pbmp)) {
                        *enable = TRUE;
                        break;
                    }
                }
            }
	}
#endif
    }

    return rv;
}

/*
 * Function:
 *      _bcm_esw_proxy_server
 * Purpose:
 *      Enable/disable proxy server
 * Parameters:
 *      unit        -  BCM Unit number
 *      dst         -  Proxy server data
 *      enable      -  server enable/diable flag
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_proxy_server(int unit, _bcm_proxy_info_t *src, int enable)
{
    int rv = BCM_E_INTERNAL;


    rv = _bcm_esw_proxy_server_set(unit, src->server_port, src->mode, enable);

    return rv;
}

/*
 * Function:
 *      _bcm_esw_proxy_server_install
 * Purpose:
 *      Install and enable proxy server
 * Parameters:
 *      unit        -  BCM Unit number
 *      dst         -  Proxy server saved data
 *      src         -  Proxy server initial data
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_proxy_server_install(int unit,
                              _bcm_proxy_info_t *dst,
                              _bcm_proxy_info_t *src)
{
    dst->server_port  = src->server_port;
    dst->mode  = src->mode;
    dst->flags |= (_BCM_PROXY_INFO_INSTALLED|_BCM_PROXY_INFO_SERVER);

    return _bcm_esw_proxy_server(unit, dst, 1);
}

/*
 * Function:
 *      _bcm_esw_proxy_server_uninstall
 * Purpose:
 *      Uninstall and disable proxy server
 * Parameters:
 *      unit        -  BCM Unit number
 *      dst         -  Proxy server saved data
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_proxy_server_uninstall(int unit, _bcm_proxy_info_t *src)
{
    return _bcm_esw_proxy_server(unit, src, 0);
}

/*
 * Function:
 *      bcm_esw_proxy_server_set
 * Purpose:
 *      Enables various kinds of lookups for packets coming from remote
 *      (proxy client)
 * Parameters:
 *      unit        -  BCM Unit number
 *      server_port -  Local port to which packets from remote devices are 
 *                     destined to
 *      mode        -  Indicates lookup type
 *      enable      -  TRUE to enable lookups
 *                     FALSE to disable lookups
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_proxy_server_set(int unit, bcm_port_t server_port, 
                          bcm_proxy_mode_t mode, int enable)
{
    _bcm_proxy_info_t src;

    if (BCM_GPORT_IS_SET(server_port)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, server_port, &server_port));
    }

    sal_memset(&src, 0, sizeof(_bcm_proxy_info_t));
    src.server_port  = server_port;
    src.mode  = mode;

    return _bcm_esw_proxy(unit, &server_methods, &src, enable);
}

/*
 * Function:
 *      bcm_esw_proxy_server_port_set
 * Purpose:
 *      This API enables various kinds of lookups on XGS3 device on
 *      behalf of remote legacy or XGS3 devices on a per ingress port
 *      basis.  The local ingress port is expected to be a Higig stack
 *      port.  The server_gport in the proxy_server structure is the
 *      target BCM_GPORT_PROXY (modid,port) destination which
 *      indicates that hybrid proxy lookup is required.
 * Parameters:
 *      unit - (IN) Unit number.
 *      local_ingress_port - (IN) Incoming Higig stack port
 *      proxy_server - (IN) Proxy server configuration
 *      enable - (IN) Enable proxy server on selected Higig stack port
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_proxy_server_port_set(int unit, bcm_gport_t local_ingress_port, 
                              bcm_proxy_server_t *proxy_server, 
                              int enable)
{
    bcm_module_t    modid;
    bcm_gport_t     server_gport, server_port;
    uint32          val, field_num, rmsp;
    static soc_field_t fields[3] =
        { DST_MODIDf, DST_PORTf, DST_PORT_MASKf };
    uint32          values[3];
    int i = 0;
    uint32 flow_type[_BCM_PROXY_RESERVED_DGLP_NUMBER] = {0};
    uint32 reg_val[_BCM_PROXY_RESERVED_DGLP_NUMBER] = {0};
    static soc_reg_t config_regs[] = {
        ING_PACKET_PROCESSING_CONTROL_0r,
        ING_PACKET_PROCESSING_CONTROL_1r
    };    
    static soc_field_t config_reg_fields[] = {
        ING_PACKET_PROCESSING_ENABLE_0f,
        ING_PACKET_PROCESSING_ENABLE_1f
    };

    if (!soc_feature(unit, soc_feature_proxy_port_property)) {
        return BCM_E_UNAVAIL;
    }

    if ((NULL == proxy_server) && enable) {
        /* No configuration to enable */
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(local_ingress_port)) {
        /* Note, modid is a throwaway arg here */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_proxy_gport_resolve(unit, local_ingress_port,
                                          &local_ingress_port, &modid, 1));
    } else if (!SOC_PORT_VALID(unit, local_ingress_port)) { 
        return BCM_E_PORT; 
    }

    if (!(IS_HG_PORT(unit, local_ingress_port) ||
          IS_CPU_PORT(unit, local_ingress_port))) {
        
        /* Only Higig ports (including the CPU port as
         * Higig) are valid.
         */
        return BCM_E_PORT;
    }

    if (soc_feature(unit,soc_feature_td3_style_proxy)) {
       /* Program relevant TD3 tables and registers */
       BCM_IF_ERROR_RETURN(bcmi_td3_proxy_server_port_config(unit, local_ingress_port, proxy_server, enable));
    } else {
        SOC_IF_ERROR_RETURN(READ_IHG_LOOKUPr(unit, local_ingress_port, &val));

        if (enable) {
            server_gport = proxy_server->server_gport;
            if (!BCM_GPORT_IS_PROXY(server_gport)) {
                /* The server gport is required to be in
                 * BCM_GPORT_PROXY format */
                return BCM_E_PORT;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_esw_proxy_gport_resolve(unit, server_gport,
                                              &server_port, &modid, 0));

            if (BCM_PROXY_MODE_HIGIG == proxy_server->mode) {
                /* BCM_PROXY_MODE_HIGIG */
                soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                  HG_LOOKUP_ENABLEf, 1);
                soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                  HYBRID_MODE_ENABLEf, 0);
            } else if (BCM_PROXY_MODE_HYBRID == proxy_server->mode) {
                /* BCM_PROXY_MODE_HYBRID */
                soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                  HG_LOOKUP_ENABLEf, 0);
                soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                  HYBRID_MODE_ENABLEf, 1);
            } else if (BCM_PROXY_MODE_SECOND_PASS == proxy_server->mode) {
                if (!soc_feature(unit, soc_feature_hg_proxy_second_pass)) {
                    return BCM_E_UNAVAIL;
                }

                if ((proxy_server->flow_type > bcmProxySecondPassFlowTypeTrillTermination) ||
                    (proxy_server->flow_type < bcmProxySecondPassFlowTypeNone)) {
                    return BCM_E_PARAM;
                }

                /* if exist, update the entry */
                for(i = 0; i < _BCM_PROXY_RESERVED_DGLP_NUMBER; i++) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, config_regs[i], REG_PORT_ANY, 0, &reg_val[i]));
                    flow_type[i] = soc_reg_field_get(unit,config_regs[i], reg_val[i], FLOW_TYPEf);

                    if (proxy_server->flow_type == flow_type[i]) {
                        soc_reg_field_set(unit,config_regs[i],
                            &reg_val[i], FLOW_TYPEf,proxy_server->flow_type);

                        soc_reg_field_set(unit,config_regs[i],
                            &reg_val[i], DST_MODIDf, modid);
                        soc_reg_field_set(unit,config_regs[i],
                            &reg_val[i], DST_PORTf, server_port);

                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, config_regs[i], REG_PORT_ANY, 0, reg_val[i]));

                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            config_reg_fields[i], 1);
                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            HG_LOOKUP_ENABLEf, 0);
                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            USE_MH_VIDf, 1);
                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            USE_MH_PKT_PRIf, 1);
                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            REMOVE_MH_SRC_PORTf, 1);

                        SOC_IF_ERROR_RETURN
                            (WRITE_IHG_LOOKUPr(unit, local_ingress_port, val));
                        return BCM_E_NONE;
                    }
                }

                /* if not exist, create it */
                for(i = 0; i < _BCM_PROXY_RESERVED_DGLP_NUMBER; i++) {
                    if (bcmProxySecondPassFlowTypeNone == flow_type[i]) {
                        soc_reg_field_set(unit,config_regs[i],
                            &reg_val[i], FLOW_TYPEf,proxy_server->flow_type);

                        soc_reg_field_set(unit,config_regs[i],
                            &reg_val[i], DST_MODIDf, modid);
                        soc_reg_field_set(unit,config_regs[i],
                            &reg_val[i], DST_PORTf, server_port);

                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, config_regs[i], REG_PORT_ANY, 0, reg_val[i]));

                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            config_reg_fields[i], 1);
                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            HG_LOOKUP_ENABLEf, 0);
                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            USE_MH_VIDf, 1);
                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            USE_MH_PKT_PRIf, 1);
                        soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                            REMOVE_MH_SRC_PORTf, 1);

                        SOC_IF_ERROR_RETURN
                            (WRITE_IHG_LOOKUPr(unit, local_ingress_port, val));

                        return BCM_E_NONE;
                    }
                }

                return BCM_E_FULL;
            } else {
                /* We don't support BCM_PROXY_MODE_LOOPBACK in this per-port
                 * version of the API.
                 */
                return BCM_E_PARAM;
            }

            /* Invert sense for HW settings */
            soc_reg_field_set(unit, IHG_LOOKUPr, &val, USE_MH_VIDf,
                    proxy_server->flags & BCM_PROXY_SERVER_KEEP_VID ? 0 : 1);

            soc_reg_field_set(unit, IHG_LOOKUPr, &val, USE_MH_PKT_PRIf,
                    proxy_server->flags & BCM_PROXY_SERVER_KEEP_PRIORITY ? 0 :1);

            /* Set "magic destination" server port to trigger proxy lookups */
            values[0] = modid;
            values[1] = server_port;

            if (SOC_REG_FIELD_VALID(unit, HG_LOOKUP_DESTINATIONr,
                                     DST_PORT_MASKf) &&
                (proxy_server->flags & BCM_PROXY_SERVER_MASK_PORT)) {
                values[2] = 1;
                field_num = 3;
            } else {
                field_num = 2;
            }

            SOC_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, HG_LOOKUP_DESTINATIONr,
                                         local_ingress_port, field_num,
                                         fields, values));

            SOC_IF_ERROR_RETURN
                (WRITE_IHG_LOOKUPr(unit, local_ingress_port, val));
        } else {
            if (NULL == proxy_server) {
                return BCM_E_PARAM;
            }
            if (BCM_PROXY_MODE_SECOND_PASS == proxy_server->mode) {
                if (soc_feature(unit, soc_feature_hg_proxy_second_pass)) {

                    if ((proxy_server->flow_type > bcmProxySecondPassFlowTypeTrillTermination) ||
                        (proxy_server->flow_type < bcmProxySecondPassFlowTypeNone)) {
                        return BCM_E_PARAM;
                    }

                    for(i = 0; i < _BCM_PROXY_RESERVED_DGLP_NUMBER; i++) {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_get(unit, config_regs[i], REG_PORT_ANY, 0, &reg_val[i]));

                        flow_type[i] = soc_reg_field_get(unit,config_regs[i], reg_val[i], FLOW_TYPEf);

                        if (proxy_server->flow_type == flow_type[i]) {
                            int dglp_is_shared = 0;
                            bcm_gport_t local_port = 0;
                            uint32 return_val = 0;

                            soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                config_reg_fields[i], 0);
                            soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                USE_MH_VIDf, 0);
                            soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                USE_MH_PKT_PRIf, 0);
                            soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                                REMOVE_MH_SRC_PORTf, 0);

                            SOC_IF_ERROR_RETURN
                                (WRITE_IHG_LOOKUPr(unit, local_ingress_port, val));

                            BCM_PBMP_ITER(PBMP_ALL(unit),local_port) {
                                SOC_IF_ERROR_RETURN
                                    (READ_IHG_LOOKUPr(unit, local_port, &return_val));
                                if (soc_reg_field_get(unit, IHG_LOOKUPr, return_val, config_reg_fields[i])) {
                                    dglp_is_shared = 1;
                                }
                            }

                            if(!dglp_is_shared) {
                                soc_reg_field_set(unit,config_regs[i],
                                    &reg_val[i], FLOW_TYPEf,0);

                                soc_reg_field_set(unit,config_regs[i],
                                    &reg_val[i], DST_MODIDf, 0);
                                soc_reg_field_set(unit,config_regs[i],
                                    &reg_val[i], DST_PORTf, 0);

                                SOC_IF_ERROR_RETURN
                                    (soc_reg32_set(unit, config_regs[i], REG_PORT_ANY, 0, reg_val[i]));
                            }

                            return BCM_E_NONE;
                        }
                    }
                    return BCM_E_NOT_FOUND;
                } else {
                    return BCM_E_UNAVAIL;
                }
            }
            /* Clear server configuration, but retain the independent
             * source port knockout configuration .
             */
            rmsp = soc_reg_field_get(unit, IHG_LOOKUPr, val,
                                     REMOVE_MH_SRC_PORTf);
            val = 0;
            soc_reg_field_set(unit, IHG_LOOKUPr, &val,
                              REMOVE_MH_SRC_PORTf, rmsp);
            SOC_IF_ERROR_RETURN
                (WRITE_IHG_LOOKUPr(unit, local_ingress_port, val));

            /* Clear the HG_LOOKUP_DESTINATION config */
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, HG_LOOKUP_DESTINATIONr,
                               local_ingress_port, 0, 0));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_proxy_server_port_get
 * Purpose:
 *      This API retrieves the current proxy server configuration on a
 *      given Higig stack port, if any.
 * Parameters:
 *      unit - (IN) Unit number.
 *      local_ingress_port - (IN) Incoming Higig stack port
 *      proxy_server - (OUT) Proxy server configuration
 *      enable - (OUT) Enable proxy server on selected Higig stack port
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_esw_proxy_server_port_get(int unit, bcm_gport_t local_ingress_port, 
                              bcm_proxy_server_t *proxy_server, 
                              int *enable)
{
    bcm_module_t    modid;
    bcm_gport_t     server_port;
    uint32          cfg_val, dst_val;
    int             hglkup, hybrid;
    bcm_proxy_mode_t mode;
    bcm_module_t reserved_modid = 0;
    bcm_gport_t reserved_port = 0;
    bcm_gport_t server_gport = 0;
    int i = 0;
    uint32 flow_type[_BCM_PROXY_RESERVED_DGLP_NUMBER] = {0};
    uint32 reg_val[_BCM_PROXY_RESERVED_DGLP_NUMBER] = {0};
    static soc_reg_t config_regs[] = {
        ING_PACKET_PROCESSING_CONTROL_0r,
        ING_PACKET_PROCESSING_CONTROL_1r,
    };    
    static soc_field_t config_reg_fields[] = {
        ING_PACKET_PROCESSING_ENABLE_0f,
        ING_PACKET_PROCESSING_ENABLE_1f
    };


    if (!soc_feature(unit, soc_feature_proxy_port_property)) {
        return BCM_E_UNAVAIL;
    }

    if ((NULL == proxy_server) || (NULL == enable)) {
        /* No way to return configuration */
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(local_ingress_port)) {
        /* Note, modid is a throwaway arg here */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_proxy_gport_resolve(unit, local_ingress_port,
                                          &local_ingress_port, &modid, 1));
    } else if (!SOC_PORT_VALID(unit, local_ingress_port)) { 
        return BCM_E_PORT; 
    }

    if (!(IS_HG_PORT(unit, local_ingress_port) ||
          IS_CPU_PORT(unit, local_ingress_port))) {
        
        /* Only Higig ports (including the CPU port as
         * Higig) are valid.
         */
        return BCM_E_PORT;
    }
    if (soc_feature(unit,soc_feature_td3_style_proxy)) {
       /* Program relevant TD3 tables and registers */
       BCM_IF_ERROR_RETURN(bcmi_td3_proxy_server_port_retrieve(unit, local_ingress_port, proxy_server, enable));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_IHG_LOOKUPr(unit, local_ingress_port, &cfg_val));

        if (BCM_PROXY_MODE_SECOND_PASS == proxy_server->mode) {
            if (!soc_feature(unit, soc_feature_hg_proxy_second_pass)) {
                return BCM_E_UNAVAIL;
            }

            for(i = 0; i < _BCM_PROXY_RESERVED_DGLP_NUMBER; i++) {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, config_regs[i], REG_PORT_ANY, 0, &reg_val[i]));
                flow_type[i] = soc_reg_field_get(unit,config_regs[i], reg_val[i], FLOW_TYPEf);

                if (proxy_server->flow_type == flow_type[i]) {
                    reserved_modid = soc_reg_field_get(unit,
                                        config_regs[i], reg_val[i], DST_MODIDf);

                    reserved_port = soc_reg_field_get(unit,
                                        config_regs[i], reg_val[i], DST_PORTf);

                    BCM_GPORT_PROXY_SET(server_gport, reserved_modid, reserved_port);

                    proxy_server->server_gport = server_gport;

                    *enable = soc_reg_field_get(unit, IHG_LOOKUPr, cfg_val,
                                                config_reg_fields[i]);

                    return BCM_E_NONE;
                }
            }
            return BCM_E_NOT_FOUND;
        }

        hglkup = soc_reg_field_get(unit, IHG_LOOKUPr, cfg_val,
                                   HG_LOOKUP_ENABLEf);
        hybrid = soc_reg_field_get(unit, IHG_LOOKUPr, cfg_val,
                                   HYBRID_MODE_ENABLEf);

        if (hglkup) {
            if (hybrid) {
                /* Illegal configuration */
                return BCM_E_INTERNAL;
            } else {
                *enable = TRUE;
                mode = BCM_PROXY_MODE_HIGIG;
            }
        } else {
            if (hybrid) {
                *enable = TRUE;
                mode = BCM_PROXY_MODE_HYBRID;
            } else {
                *enable = FALSE;
                /* Nothing else to do */
                return BCM_E_NONE;
            }
        }

        bcm_proxy_server_t_init(proxy_server);

        proxy_server->mode = mode;

        /* Invert sense from HW settings */
        if (0 == soc_reg_field_get(unit, IHG_LOOKUPr, cfg_val, USE_MH_VIDf)) {
            proxy_server->flags |= BCM_PROXY_SERVER_KEEP_VID;
        }

        if (0 == soc_reg_field_get(unit, IHG_LOOKUPr, cfg_val, USE_MH_PKT_PRIf)) {
            proxy_server->flags |= BCM_PROXY_SERVER_KEEP_PRIORITY;
        }

        
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, HG_LOOKUP_DESTINATIONr,
                           local_ingress_port, 0, &dst_val));

        if (SOC_REG_FIELD_VALID(unit, HG_LOOKUP_DESTINATIONr,
                                DST_PORT_MASKf) &&
            soc_reg_field_get(unit, HG_LOOKUP_DESTINATIONr,
                              dst_val, DST_PORT_MASKf)) {
            proxy_server->flags |= BCM_PROXY_SERVER_MASK_PORT;
        }

        modid = soc_reg_field_get(unit, HG_LOOKUP_DESTINATIONr,
                                  dst_val, DST_MODIDf);
        server_port = soc_reg_field_get(unit, HG_LOOKUP_DESTINATIONr,
                                        dst_val, DST_PORTf);
        BCM_GPORT_PROXY_SET(proxy_server->server_gport, modid, server_port);
    }
        return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_esw_proxy_cleanup_data
 * Purpose:
 *      Uninstall and release proxy data
 * Parameters:
 *      unit        -  BCM Unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_proxy_cleanup_data(int unit)
{
   _bcm_proxy_ifc_t *ifc;

    if (_BCM_PROXY_LOCK(unit)) {
        return BCM_E_INIT;
    }

    /* Cleanup clients/servers */

    while (_bcm_proxy_control[unit].proxy_list) {
        if (_bcm_proxy_control[unit].proxy_list->flags &
            _BCM_PROXY_INFO_SERVER) {
            ifc = &server_methods;
        } else {
            ifc = &client_methods;
        }
        (void)_bcm_esw_proxy_uninstall
            (unit, ifc, _bcm_proxy_control[unit].proxy_list);
    }

    _BCM_PROXY_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_proxy_init
 * Purpose:
 *      Initialize the Proxy subsystem
 * Parameters:
 *      unit        -  BCM Unit number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_proxy_init(int unit)
{
    int rv = BCM_E_NONE;
    if (soc_feature(unit,soc_feature_td3_style_proxy)) {
        int port_size = soc_mem_index_count(unit, ING_DEVICE_PORTm) - 1;
        int idx = 0;

        if (proxy_internal_initialized[unit]) {
            bcm_esw_proxy_cleanup(unit);
        }

        proxy_bookkeep[unit].proxy_server_copy =
            (bcm_proxy_server_t **)
            sal_alloc(_BCM_PROXY_SERVER_INSTANCES *
                    sizeof(bcm_proxy_server_t *), "proxy server sw state entry");

        sal_memset(proxy_bookkeep[unit].proxy_server_copy, 0,
                (_BCM_PROXY_SERVER_INSTANCES * sizeof(bcm_proxy_server_t*)));

        if (proxy_bookkeep[unit].proxy_server_copy == NULL) {
            return BCM_E_MEMORY;
        }

        for (idx = 0; idx < _BCM_PROXY_SERVER_INSTANCES; idx++) {
            proxy_bookkeep[unit].proxy_server_copy[idx] =
                (bcm_proxy_server_t *)
                sal_alloc(port_size * sizeof(bcm_proxy_server_t),
                        "per instance proxy server entry");

            sal_memset(proxy_bookkeep[unit].proxy_server_copy[idx], 0,
                    (port_size * sizeof(bcm_proxy_server_t)));

            if (proxy_bookkeep[unit].proxy_server_copy[idx] == NULL) {
                bcm_esw_proxy_cleanup(unit);
                return BCM_E_MEMORY;
            }
        }
    }
    if (_LOCK(unit) == NULL) {
        _LOCK(unit) = sal_mutex_create("bcm_proxy_lock");
        rv = (_LOCK(unit) == NULL) ? BCM_E_MEMORY : BCM_E_NONE;
    }

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_proxy_cleanup_data(unit);
    }
    if (soc_feature(unit,soc_feature_td3_style_proxy)) {
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(bcm_esw_proxy_cleanup(unit));
            return rv;
        }
#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            rv = bcmi_proxy_server_recover(unit);
        } else { /* Cold Boot */
            rv = bcmi_proxy_server_wb_alloc(unit);
        }
#endif /* BCM_WARM_BOOT_SUPPORT */

        if (BCM_SUCCESS(rv)) {
            proxy_internal_initialized[unit] = 1;
        }
    }
    return rv;
}

/*
 * Function:
 *      bcm_esw_proxy_cleanup
 * Purpose:
 *      Deinitialize the Proxy subsystem
 * Parameters:
 *      unit        -  BCM Unit number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_proxy_cleanup(int unit)
{
    int idx = 0;
    if (_LOCK(unit) != NULL) {
        (void) _bcm_esw_proxy_cleanup_data(unit);
        sal_mutex_destroy(_LOCK(unit));
        _LOCK(unit) = NULL;
    }
    if (soc_feature(unit,soc_feature_td3_style_proxy)) {
        for (idx = 0; idx < _BCM_PROXY_SERVER_INSTANCES; idx++) {
            if (NULL != proxy_bookkeep[unit].proxy_server_copy[idx]) {
                sal_free(proxy_bookkeep[unit].proxy_server_copy[idx]);
                proxy_bookkeep[unit].proxy_server_copy[idx] = NULL;
            }
        }
        if (NULL != proxy_bookkeep[unit].proxy_server_copy) {
            sal_free(proxy_bookkeep[unit].proxy_server_copy);
            proxy_bookkeep[unit].proxy_server_copy = NULL;
        }

        proxy_internal_initialized[unit] = 0;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_proxy_egress_create
 * Purpose:
 *      Create Proxy Egress forwarding object.
 * Parameters:
 *      unit    - (IN)  bcm device.
 *      flags   - (IN)  BCM_PROXY_REPLACE: replace existing.
 *                      BCM_PROXY_WITH_ID: intf argument is given.
 *      proxy_egress     - (IN) Egress forwarding destination.
 *      proxy_if_id    - (OUT) proxy interface id pointing to Egress object.
 *                      This is an IN argument if either BCM_PROXY_REPLACE
 *                      or BCM_PROXY_WITH_ID are given in flags.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_proxy_egress_create(int unit, uint32 flags, bcm_proxy_egress_t *proxy_egress, 
                         bcm_if_t *proxy_if_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit) && soc_feature(unit, soc_feature_l3)) {
        bcm_proxy_egress_t proxy_egress_local;

        /* Input parameters check. */
        if ((NULL == proxy_egress) || (NULL == proxy_if_id)) {
            return (BCM_E_PARAM);
        }

        /* Copy provided structure to local so it can be modified. */
        sal_memcpy(&proxy_egress_local, proxy_egress, sizeof(bcm_proxy_egress_t));

        L3_LOCK(unit);
        rv = bcm_td2_proxy_egress_create(unit, flags, &proxy_egress_local, proxy_if_id);
        L3_UNLOCK(unit);
    }
#endif /* BCM_TRIDENT2_SUPPORT*/

    return rv;
}

/*
 * Function:
 *      bcm_esw_proxy_egress_destroy
 * Purpose:
 *      Destroy Proxy Egress forwarding destination.
 * Parameters:
 *      unit    - (IN) bcm device.
 *      proxy_if_id    - (IN) proxy interface id pointing to Egress object
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_proxy_egress_destroy(int unit, bcm_if_t proxy_if_id)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit) && soc_feature(unit, soc_feature_l3)) {

        L3_LOCK(unit);
        rv = bcm_td2_proxy_egress_destroy(unit, proxy_if_id);
        L3_UNLOCK(unit);
    }
#endif /* BCM_TRIDENT2_SUPPORT*/

    return rv;
}

/*
 * Function:
 *      bcm_esw_proxy_egress_get
 * Purpose:
 *      Get Proxy Egress forwarding destination.
 * Parameters:
 *      unit    - (IN) bcm device.
 *      proxy_if_id    - (IN) Proxy Egress destination
 *      proxy_egress    - (OUT) Egress forwarding destination.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_proxy_egress_get (int unit, bcm_if_t proxy_if_id, bcm_proxy_egress_t *proxy_egress) 
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit) && soc_feature(unit, soc_feature_l3)) {
        /* Input parameters check. */
        if (NULL == proxy_egress) {
            return (BCM_E_PARAM);
        }

        L3_LOCK(unit);
        rv = bcm_td2_proxy_egress_get (unit, proxy_if_id, proxy_egress);
        L3_UNLOCK(unit);
    }
#endif /* BCM_TRIDENT2_SUPPORT*/

    return rv;
}


/*
 * Function:
 *      bcm_esw_proxy_egress_traverse
 * Purpose:
 *      Goes through proxy egress objects table and runs the user callback
 *      function at each valid egress objects entry passing back the
 *      information for that object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      trav_fn    - (IN) Callback function. 
 *      user_data  - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_esw_proxy_egress_traverse(int unit, 
                           bcm_proxy_egress_traverse_cb trav_fn, void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit) && soc_feature(unit, soc_feature_l3)) {
        L3_LOCK(unit);
        rv = bcm_td2_proxy_egress_traverse(unit, trav_fn, user_data);
        L3_UNLOCK(unit);

    }
#endif /* BCM_TRIDENT2_SUPPORT*/

    return rv;
}

#else /* INCLUDE_L3 */
typedef int _bcm_esw_proxy_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_L3 */
