/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       ipfix.c
 * Purpose:    IPFIX API
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <bcm/error.h>
#include <bcm/ipfix.h>
#include <bcm/mirror.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/ipfix.h>
#include <bcm_int/esw/mirror.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm/module.h>
#include <bcm_int/esw/switch.h>
#endif

#ifdef BCM_IPFIX_SUPPORT
static _bcm_ipfix_ctrl_t *_bcm_ipfix_ctrl[BCM_MAX_NUM_UNITS];

#ifdef BCM_TRIUMPH_SUPPORT
static const _bcm_ipfix_id_def_t _ipfix_tr_id_def = {
    { ING_IPFIX_PORT_CONFIGr, EGR_IPFIX_PORT_CONFIGr },
    { ING_IPFIX_PROFILEm, EGR_IPFIX_PROFILEm },
    { ING_IPFIX_EXPORT_FIFOm, EGR_IPFIX_EXPORT_FIFOm },
    { IPFIX_AGE_CONTROLr, EGR_IPFIX_AGE_CONTROLr },
    { ING_IPFIX_MIRROR_CONTROL_64r, EGR_IPFIX_MIRROR_CONTROL_64r },
    { MTP_INDEX0f, MTP_INDEX1f, MTP_INDEX2f, MTP_INDEX3f },
};

/* Mapping between profile and field in bcm_ipfix_config_t */
static const _bcm_ipfix_profile_def_t _ipfix_tr_profile_def[] = {
    {
        _BCM_IPFIX_PROFILE_TYPE_MEM,
        IP_DSCP_PROFILEf,
        { /* stage */
            { /* ingress */
                _BCM_IPFIX_PROFILE_ID_ING_DSCP_XLATE_TABLE,
                { ING_IPFIX_DSCP_XLATE_TABLEm, INVALIDm },
                { sizeof(ing_ipfix_dscp_xlate_table_entry_t) / sizeof(uint32),
                  0 },
            },
            { /* egress */
                _BCM_IPFIX_PROFILE_ID_EGR_DSCP_XLATE_TABLE,
                { EGR_IPFIX_DSCP_XLATE_TABLEm, INVALIDm },
                { sizeof(egr_ipfix_dscp_xlate_table_entry_t) / sizeof(uint32),
                  0 },
            },
        },
    },
    {
        _BCM_IPFIX_PROFILE_TYPE_MEM,
        IP_IPV4_MASK_PROFILEf,
        { /* stage */
            { /* ingress */
                _BCM_IPFIX_PROFILE_ID_ING_IPV4_MASK_SET_A,
                { ING_IPFIX_IPV4_MASK_SET_Am, ING_IPFIX_IPV4_MASK_SET_Bm },
                { sizeof(ing_ipfix_ipv4_mask_set_a_entry_t) / sizeof(uint32),
                  sizeof(ing_ipfix_ipv4_mask_set_b_entry_t) / sizeof(uint32) },
            },
            { /* egress */
                _BCM_IPFIX_PROFILE_ID_EGR_IPV4_MASK_SET_A,
                { EGR_IPFIX_IPV4_MASK_SET_Am, INVALIDm },
                { sizeof(egr_ipfix_ipv4_mask_set_a_entry_t) / sizeof(uint32),
                  0 },
            },
        },
    },
    {
        _BCM_IPFIX_PROFILE_TYPE_MEM,
        IP_IPV6_MASK_PROFILEf,
        { /* stage */
            { /* ingress */
                _BCM_IPFIX_PROFILE_ID_ING_IPV6_MASK_SET_A,
                { ING_IPFIX_IPV6_MASK_SET_Am, ING_IPFIX_IPV6_MASK_SET_Bm },
                { sizeof(ing_ipfix_ipv6_mask_set_a_entry_t) / sizeof(uint32),
                  sizeof(ing_ipfix_ipv6_mask_set_b_entry_t) / sizeof(uint32) },
            },
            { /* egress */
                _BCM_IPFIX_PROFILE_ID_EGR_IPV6_MASK_SET_A,
                { EGR_IPFIX_IPV6_MASK_SET_Am, INVALIDm },
                { sizeof(egr_ipfix_ipv6_mask_set_a_entry_t) / sizeof(uint32),
                  0 },
            },
        },
    },
    {
        _BCM_IPFIX_PROFILE_TYPE_REG,
        PORT_LIMIT_PROFILEf,
        { /* stage */
            { /* ingress */
                _BCM_IPFIX_PROFILE_ID_ING_PORT_RECORD_LIMIT_SET,
                { ING_IPFIX_PORT_RECORD_LIMIT_SETr, INVALIDr },
                { 0, 0 },
            },
            { /* egress */
                _BCM_IPFIX_PROFILE_ID_EGR_PORT_RECORD_LIMIT_SET,
                { EGR_IPFIX_PORT_RECORD_LIMIT_SETr, INVALIDr },
                { 0, 0 },
            },
        },
    },
    {
        _BCM_IPFIX_PROFILE_TYPE_REG,
        MIN_LIVE_TIME_PROFILEf,
        { /* stage */
            { /* ingress */
                _BCM_IPFIX_PROFILE_ID_ING_MINIMUM_LIVE_TIME_SET,
                { ING_IPFIX_MINIMUM_LIVE_TIME_SETr, INVALIDr },
                { 0, 0 },
            },
            { /* egress */
                _BCM_IPFIX_PROFILE_ID_EGR_MINIMUM_LIVE_TIME_SET,
                { EGR_IPFIX_MINIMUM_LIVE_TIME_SETr, INVALIDr },
                { 0, 0 },
            },
        },
    },
#ifdef BCM_TRIUMPH2_SUPPORT
    {
        _BCM_IPFIX_PROFILE_TYPE_REG,
        MAX_LIVE_TIME_PROFILEf,
        { /* stage */
            { /* ingress */
                _BCM_IPFIX_PROFILE_ID_ING_MAXIMUM_LIVE_TIME_SET,
                { ING_IPFIX_MAXIMUM_LIVE_TIME_SETr, INVALIDr },
                { 0, 0 },
            },
            { /* egress */
                _BCM_IPFIX_PROFILE_ID_EGR_MAXIMUM_LIVE_TIME_SET,
                { EGR_IPFIX_MAXIMUM_LIVE_TIME_SETr, INVALIDr },
                { 0, 0 },
            },
        },
    },
#endif /* BCM_TRIUMPH2_SUPPORT */
    {
        _BCM_IPFIX_PROFILE_TYPE_REG,
        MAX_IDLE_AGE_PROFILEf,
        { /* stage */
            { /* ingress */
                _BCM_IPFIX_PROFILE_ID_ING_MAXIMUM_IDLE_AGE_SET,
                { ING_IPFIX_MAXIMUM_IDLE_AGE_SETr, INVALIDr },
                { 0, 0 },
            },
            { /* egress */
                _BCM_IPFIX_PROFILE_ID_EGR_MAXIMUM_IDLE_AGE_SET,
                { EGR_IPFIX_MAXIMUM_IDLE_AGE_SETr, INVALIDr },
                { 0, 0 },
            },
        },
    },
    {
        _BCM_IPFIX_PROFILE_TYPE_REG,
        SAMPLING_LIMIT_PROFILEf,
        { /* stage */
            { /* ingress */
                _BCM_IPFIX_PROFILE_ID_ING_SAMPLING_LIMIT_SET,
                { ING_IPFIX_SAMPLING_LIMIT_SETr, INVALIDr },
                { 0, 0 },
            },
            { /* egress */
                _BCM_IPFIX_PROFILE_ID_EGR_SAMPLING_LIMIT_SET,
                { EGR_IPFIX_SAMPLING_LIMIT_SETr, INVALIDr },
                { 0, 0 },
            },
        },
    },
    {
        _BCM_IPFIX_PROFILE_TYPE_NONE, /* end of table mark */
    },
};

/*
 * Mapping between bcm_ipfix_config_t.flags bit mask and field
 * of ING_IPFIX_PORT_CONFIG (or EGR_IPFIX_PORT_CONFIG)
 */
static const _bcm_ipfix_flags_def_t _ipfix_tr_port_config_flags_def[] = {
    { BCM_IPFIX_CONFIG_TCP_END_DETECT, TCP_END_DETECT_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_SRC_IP, IP_SIP_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_DST_IP, IP_DIP_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_IP_PROT, IP_PROTOCOL_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_IP_DSCP, IP_DSCP_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_IP_ECN, IP_ECN_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_L4_SRC_PORT, IP_TCP_SRC_PORT_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_L4_DST_PORT, IP_TCP_DEST_PORT_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_IP6_FLOW, IP_IPV6_LABEL_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_ICMP_TYPE, IP_ICMP_TYPE_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_ICMP_CODE, IP_ICMP_CODE_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_MACDA, L2_MAC_DA_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_MACSA, L2_MAC_SA_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_VLAN_ID, L2_VLAN_ID_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_VLAN_PRI, L2_VLAN_PRI_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_ETHER_TYPE, L2_ETH_TYPE_ENABLEf },
    { BCM_IPFIX_CONFIG_KEY_VLAN_TAGGED, L2_TAGGED_ENABLEf },
    { 0 },
};
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

/*
 * Function:
 *      _bcm_ipfix_reinit
 * Description:
 *      Recovers ipfix rate config data info.   
 * Parameters:
 *      unit - (IN) BCM unit number
 * Notes:
 *      None.
 */
STATIC int 
_bcm_ipfix_reinit(int unit)
{
    _bcm_ipfix_ctrl_t *ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    soc_scache_handle_t scache_handle;
    uint8   *ipfix_scache = NULL;
    int size;
    int rv = BCM_E_NONE;
    int arry_size;
    
    sal_memset(&scache_handle , 0, sizeof(soc_scache_handle_t));
    size = soc_mem_index_count(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm);
    arry_size = _SHR_BITDCLSIZE(size);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IPFIX, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE, 0, &ipfix_scache, 
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_E_NOT_FOUND == rv) {
        ipfix_scache = NULL;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    } else {
        sal_memcpy(&ipfix_ctrl->rate_used_count, ipfix_scache,
                            sizeof(ipfix_ctrl->rate_used_count));       
        ipfix_scache += sizeof(ipfix_ctrl->rate_used_count);
        /*
         * Copy rate_id pbmp value from scache to ipfix control structure.
         */
        sal_memcpy(ipfix_ctrl->rate_used_bmp, ipfix_ctrl, arry_size);
    }

    return rv;
}
#endif

STATIC int
_bcm_ipfix_gport_resolve(int unit,
                         bcm_gport_t gport,
                         bcm_port_t *local_port)
{
    bcm_module_t    modid;
    bcm_port_t      port;
    bcm_trunk_t     tgid;
    int             id;

    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, gport, &modid, &port, &tgid, &id));
        if (id != -1 || tgid != -1) {
            return BCM_E_PARAM;
        }
        *local_port = port;
    } else {
        if (!SOC_PORT_VALID(unit, gport)) {
            return BCM_E_PORT;
        }
        *local_port = gport;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_ipfix_init(int unit,
                const _bcm_ipfix_profile_def_t *profile_def,
                const _bcm_ipfix_id_def_t *id_def)
{
    _bcm_ipfix_ctrl_t *ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    const _bcm_ipfix_profile_def_t *def;
    void *profile;
    soc_reg_t reg;
    int stage, ids[2], entry_words[2];
    uint32 rval;
    int size;
#ifdef BCM_WARM_BOOT_SUPPORT
    soc_scache_handle_t scache_handle;
    uint32 ipfix_scache_size;
    uint32 flow_rate_tbl_size;
    uint8 *ipfix_scache = NULL;
    int rv = BCM_E_NONE;
#endif /* !BCM_WARM_BOOT_SUPPORT */

    if (ipfix_ctrl == NULL) {
        ipfix_ctrl = sal_alloc(sizeof(_bcm_ipfix_ctrl_t), "IPFIX Control Data");
        if (ipfix_ctrl == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(ipfix_ctrl, 0, sizeof(_bcm_ipfix_ctrl_t));

        if (soc_feature(unit, soc_feature_ipfix_rate)) {
            size = soc_mem_index_count(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm);
            ipfix_ctrl->rate_used_bmp = 
                    sal_alloc(SHR_BITALLOCSIZE(size), "IPFIX Rate Used BMP");
            if (ipfix_ctrl->rate_used_bmp == NULL) {
                sal_free(ipfix_ctrl);
                return BCM_E_MEMORY;
            }
            sal_memset(ipfix_ctrl->rate_used_bmp, 0, SHR_BITALLOCSIZE(size));
            SHR_BITSET(ipfix_ctrl->rate_used_bmp, 0); /* index 0 is reserved */
        }

        _bcm_ipfix_ctrl[unit] = ipfix_ctrl;

        for (def = profile_def; def->type != _BCM_IPFIX_PROFILE_TYPE_NONE;
             def++) {
            for (stage = 0; stage < 2; stage++) {
                if (!soc_mem_field_valid(unit, id_def->profile[stage],
                                         def->index_field)) {
                    continue;
                }
                profile = &ipfix_ctrl->profiles[def->stage[stage].profile_id];
                if (def->type == _BCM_IPFIX_PROFILE_TYPE_REG) {
                    soc_profile_reg_t_init(profile);
                } else if (def->type == _BCM_IPFIX_PROFILE_TYPE_MEM) {
                    soc_profile_mem_t_init(profile);
                }
            }
        }

        ipfix_ctrl->pid = SAL_THREAD_ERROR;
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            ipfix_ctrl->dma_chan[0] = SOC_MEM_FIFO_DMA_CHANNEL_2;
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            ipfix_ctrl->dma_chan[0] = SOC_MEM_FIFO_DMA_CHANNEL_0;
        }
        ipfix_ctrl->dma_chan[1] = SOC_MEM_FIFO_DMA_CHANNEL_3;

#ifdef BCM_WARM_BOOT_SUPPORT
        if (soc_feature(unit, soc_feature_ipfix_rate)) {
            /*
             * Get the number of flow rate meter entries supported in the 
             * ING_IPFIX_FLOW_RATE_METER_TABLEm table.
             */
            flow_rate_tbl_size = 
                    soc_mem_index_count(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm);
            ipfix_scache_size = SHR_BITALLOCSIZE(flow_rate_tbl_size) + 
                                    sizeof(ipfix_ctrl->rate_used_count);
            SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IPFIX, 0);
            rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
                                            (0 == SOC_WARM_BOOT(unit)),
                                            ipfix_scache_size,
                                            &ipfix_scache, 
                                            BCM_WB_DEFAULT_VERSION, NULL);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                _bcm_esw_ipfix_deinit(unit);
                return rv;
            }
            sal_memset(&ipfix_scache, 0, sizeof(ipfix_scache_size));
        }
#endif /* !BCM_WARM_BOOT_SUPPORT */
    }

    for (def = profile_def; def->type != _BCM_IPFIX_PROFILE_TYPE_NONE; def++) {
        for (stage = 0; stage < 2; stage++) {
            if (!soc_mem_field_valid(unit, id_def->profile[stage],
                                     def->index_field)) {
                continue;
            }
            profile = &ipfix_ctrl->profiles[def->stage[stage].profile_id];
            ids[0] = def->stage[stage].reg_mem_id[0];
            ids[1] = def->stage[stage].reg_mem_id[1];
            if (def->type == _BCM_IPFIX_PROFILE_TYPE_REG) {
                BCM_IF_ERROR_RETURN(soc_profile_reg_create(unit, ids, 1,
                                    profile));
            } else if (def->type == _BCM_IPFIX_PROFILE_TYPE_MEM) {
                entry_words[0] = def->stage[stage].entry_words[0];
                entry_words[1] = def->stage[stage].entry_words[1];
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_create(unit, ids, entry_words,
                                            ids[1] != INVALIDm ? 2 : 1,
                                            profile));
            }
        }
    }

    for (stage = 0; stage < 2; stage++) {
        reg = id_def->age_control[stage];
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, AGE_ENABLEf, 1);
        if (SAL_BOOT_QUICKTURN) {
            if (soc_reg_field_valid(unit, reg, CLK_GRANf)) {         
                soc_reg_field_set(unit, reg, &rval, CLK_GRANf, 0);
            }
        }
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }
    
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(READ_AUX_ARB_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, AUX_ARB_CONTROL_2r, &rval,
                IPFIX_REFRESH_ENABLEf, 1);
        if (SAL_BOOT_QUICKTURN) {
            soc_reg_field_set(unit, AUX_ARB_CONTROL_2r,
                              &rval, IPFIX_CLK_GRANf, 0);
        }
        BCM_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROL_2r(unit, rval));
    }
#endif

#if defined(BCM_WARM_BOOT_SUPPORT) && defined(BCM_IPFIX_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        /*
         * Reload IPFIX module data.
         */
        BCM_IF_ERROR_RETURN(_bcm_ipfix_reinit(unit));
    }
#endif /* !BCM_WARM_BOOT_SUPPORT */
    return BCM_E_NONE;
}

STATIC int
_bcm_ipfix_profile_del(int unit,
                       const _bcm_ipfix_profile_def_t *profile_def,
                       const _bcm_ipfix_id_def_t *id_def,
                       bcm_ipfix_stage_t stage,
                       bcm_port_t port,
                       int count)
{
    _bcm_ipfix_ctrl_t *ipfix_ctrl;
    const _bcm_ipfix_profile_def_t *def;
    void *profile;
    union {
        ing_ipfix_profile_entry_t ing_ipfix_profile;
        egr_ipfix_profile_entry_t egr_ipfix_profile;
    } profile_entry;
    soc_mem_t profile_mem;
    uint32 index;
    int rv;

    ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    profile_mem = id_def->profile[stage];

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY,
                                     port, &profile_entry));
    rv = BCM_E_NONE;
    for (def = profile_def; count && def->type != _BCM_IPFIX_PROFILE_TYPE_NONE;
         count--, def++) {
        if (!soc_mem_field_valid(unit, profile_mem, def->index_field)) {
            continue;
        }
        profile = &ipfix_ctrl->profiles[def->stage[stage].profile_id];
        index = soc_mem_field32_get(unit, profile_mem, &profile_entry,
                                    def->index_field);
        if (def->type == _BCM_IPFIX_PROFILE_TYPE_REG) {
            rv |= soc_profile_reg_delete(unit, profile, index);
        } else if (def->type == _BCM_IPFIX_PROFILE_TYPE_MEM) {
            if (def->stage[stage].reg_mem_id[0] ==
                ING_IPFIX_DSCP_XLATE_TABLEm ||
                def->stage[stage].reg_mem_id[0] ==
                EGR_IPFIX_DSCP_XLATE_TABLEm) {
                index *= 64;
            }
            rv |= soc_profile_mem_delete(unit, profile, index);
        }
    }

    return rv;
}

/*
 * Function:
 *     _bcm_ipfix_rate_id_check
 * Description:
 *     To check the status of the specified rate_id
 * Parameters:
 *     unit            device number
 *     rate_id         rate identifier
 * Return:
 *     BCM_E_UNAVAIL   ipfix feature is not supported for this device
 *     BCM_E_INIT      ipfix component is not initialized on this device
 *     BCM_E_BADID     specified rate_id is not in valid range
 *     BCM_E_NOT_FOUND spefified rate_id is in valid range but not in used
 *     BCM_E_NONE      specified rate_id is in used
 */
STATIC int
_bcm_ipfix_rate_id_check(int unit,
                         bcm_ipfix_rate_id_t rate_id)
{
    if (!soc_feature(unit, soc_feature_ipfix_rate)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }

    /* rate_id 0 is reserved by hardware to indicate disabling rate metering */
    if (rate_id < 1 || rate_id >=
        soc_mem_index_count(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm)) {
        return BCM_E_BADID;
    }

    if (!SHR_BITGET(_bcm_ipfix_ctrl[unit]->rate_used_bmp, rate_id)) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ipfix_rate_get
 * Description:
 *     To check the status of the specified rate_id
 * Parameters:
 *     unit            device number
 *     rate_id         rate identifier
 *     rate_info       pointer to rate info structure
 * Return:
 *     BCM_E_XXX
 * Note:
 *     Caller needs to make sure all arguments are valid for this internal
 *     function.
 */
STATIC int
_bcm_ipfix_rate_get(int unit,
                    bcm_ipfix_rate_id_t rate_id,
                    bcm_ipfix_rate_t *rate_info)
{
    ing_ipfix_flow_rate_meter_table_entry_t entry;
    uint32 cng;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                     MEM_BLOCK_ANY, rate_id, &entry));

    sal_memset(rate_info, 0, sizeof(*rate_info));
    rate_info->rate_id = rate_id;
    rate_info->flags |= BCM_IPFIX_RATE_VIOLATION_WITH_ID;
    /* the token size for BUCKETCOUNT is 1/128 of a flow */
    rate_info->count =
        soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            BUCKETCOUNTf) >> 7;
    rate_info->limit =
        soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            BUCKETSIZEf);
    rate_info->rate =
        soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            REFRESHCOUNTf);
    if (soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            DROPf)) {
        rate_info->flags |= BCM_IPFIX_RATE_VIOLATION_DROP;
    }
    if (soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            COPY_TO_CPUf)) {
        rate_info->flags |= BCM_IPFIX_RATE_VIOLATION_COPY_TO_CPU;
    }
    if (soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            CHANGE_DSCPf)) {
        rate_info->flags |= BCM_IPFIX_RATE_VIOLATION_DSCP_SET;
        rate_info->dscp =
            soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                                NEW_DSCPf);
    }
    if (soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            CHANGE_CNGf)) {
        rate_info->flags |= BCM_IPFIX_RATE_VIOLATION_COLOR_SET;
        cng = soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                  &entry, NEW_CNGf);
        rate_info->color = _BCM_COLOR_DECODING(unit, cng);
    }
    if (soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            CHANGE_PKT_PRIf)) {
        rate_info->flags |= BCM_IPFIX_RATE_VIOLATION_PKT_PRI_SET;
        rate_info->pkt_pri =
            soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                                NEW_PRIf);
    }
    if (soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            CHANGE_INT_PRIf)) {
        rate_info->flags |= BCM_IPFIX_RATE_VIOLATION_INT_PRI_SET;
        rate_info->int_pri =
            soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                                NEW_INT_PRIf);
    }

    return BCM_E_NONE;
}
#endif /* BCM_IPFIX_SUPPORT */

/*
 * Function:
 *     bcm_esw_ipfix_register
 * Description:
 *     To register the callback function for flow info export
 * Parameters:
 *     unit          device number
 *     callback      user callback function
 *     userdata      user data used as argument during callback
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_RESOURCE
 *     BCM_E_UNAVAIL
 */
int
bcm_esw_ipfix_register(int unit,
                       bcm_ipfix_callback_t callback,
                       void *userdata)
{
#ifdef BCM_IPFIX_SUPPORT
    _bcm_ipfix_ctrl_t *ipfix_ctrl;
    const _bcm_ipfix_profile_def_t *profile_def = NULL;
    const _bcm_ipfix_id_def_t *id_def = NULL;
    _bcm_ipfix_callback_entry_t *entry;
    int free_index, i;

    if (!soc_feature(unit, soc_feature_ipfix)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        profile_def = _ipfix_tr_profile_def;
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (profile_def == NULL || id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_ipfix_init(unit, profile_def, id_def));
    }
    ipfix_ctrl = _bcm_ipfix_ctrl[unit];

    if (callback == NULL) {
        return BCM_E_PARAM;
    }

    free_index = -1;
    for (i = 0; i < _BCM_IPFIX_CB_MAX; i++) {
        entry = &ipfix_ctrl->callback_entry[i];
        if (entry->state == _BCM_IPFIX_CALLBACK_STATE_INVALID ||
            (ipfix_ctrl->pid == SAL_THREAD_ERROR &&
             entry->state == _BCM_IPFIX_CALLBACK_STATE_UNREGISTERED)) {
            if (free_index < 0) {
                free_index = i;
            }
        } else if (entry->state == _BCM_IPFIX_CALLBACK_STATE_ACTIVE ||
                   entry->state == _BCM_IPFIX_CALLBACK_STATE_REGISTERED) {
            if (entry->callback == callback && entry->userdata == userdata) {
                return BCM_E_NONE;
            }
        }
    }

    if (free_index < 0) {
        return BCM_E_RESOURCE;
    }

    entry = &ipfix_ctrl->callback_entry[free_index];
    entry->callback = callback;
    entry->userdata = userdata;
    entry->state = _BCM_IPFIX_CALLBACK_STATE_REGISTERED;

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_unregister
 * Description:
 *     To unregister the callback function for flow info export
 * Parameters:
 *     unit          device number
 *     callback      user callback function
 *     userdata      user data used as argument during callback
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcm_esw_ipfix_unregister(int unit,
                         bcm_ipfix_callback_t callback,
                         void *userdata)
{
#ifdef BCM_IPFIX_SUPPORT
    _bcm_ipfix_ctrl_t *ipfix_ctrl;
    _bcm_ipfix_callback_entry_t *entry;
    int i;

    if (!soc_feature(unit, soc_feature_ipfix)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }
    ipfix_ctrl = _bcm_ipfix_ctrl[unit];

    for (i = 0; i < _BCM_IPFIX_CB_MAX; i++) {
        entry = &ipfix_ctrl->callback_entry[i];

        if (ipfix_ctrl->pid == SAL_THREAD_ERROR ||
            entry->state == _BCM_IPFIX_CALLBACK_STATE_ACTIVE) {
            if (entry->callback == callback && entry->userdata == userdata) {
                entry->state = _BCM_IPFIX_CALLBACK_STATE_UNREGISTERED;
                return BCM_E_NONE;
            }
        }
    }

    return BCM_E_NOT_FOUND;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_config_set
 * Description:
 *     To set per port IPFIX configuration
 * Parameters:
 *     unit            device number
 *     stage           BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port            port number
 *     config          pointer to ipfix configuration
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_RESOURCE
 *     BCM_E_UNAVAIL
 *     BCM_E_PORT
 */
int
bcm_esw_ipfix_config_set(int unit,
                         bcm_ipfix_stage_t stage,
                         bcm_port_t port,
                         bcm_ipfix_config_t *config)
{
#ifdef BCM_IPFIX_SUPPORT
    _bcm_ipfix_ctrl_t *ipfix_ctrl;
    const _bcm_ipfix_profile_def_t *profile_def = NULL, *def;
    const _bcm_ipfix_id_def_t *id_def = NULL;
    const _bcm_ipfix_flags_def_t *flags_def = NULL;
    void *profile;
    union {
        ing_ipfix_profile_entry_t ing_ipfix_profile;
        egr_ipfix_profile_entry_t egr_ipfix_profile;
    } profile_entry;
    union {
        ing_ipfix_ipv4_mask_set_a_entry_t ing_ipfix_ipv4_mask_set_a;
        ing_ipfix_ipv4_mask_set_b_entry_t ing_ipfix_ipv4_mask_set_b;
        ing_ipfix_ipv6_mask_set_a_entry_t ing_ipfix_ipv6_mask_set_a;
        ing_ipfix_ipv6_mask_set_b_entry_t ing_ipfix_ipv6_mask_set_b;
        egr_ipfix_ipv4_mask_set_a_entry_t egr_ipfix_ipv4_mask_set_a;
        egr_ipfix_ipv6_mask_set_a_entry_t egr_ipfix_ipv6_mask_set_a;
        ing_ipfix_dscp_xlate_table_entry_t ing_ipfix_dscp_xlate_table[64];
        egr_ipfix_dscp_xlate_table_entry_t egr_ipfix_dscp_xlate_table[64];
        uint32 w[1];
    } entry0, entry1;
    void *entries[2];
    uint64 rval64, *rval64s[2];
    soc_reg_t reg;
    soc_mem_t mems[2], profile_mem;
    uint32 rval, fval, index, flags;
    int rv, count, size, i;

    if (!soc_feature(unit, soc_feature_ipfix)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        profile_def = _ipfix_tr_profile_def;
        id_def = &_ipfix_tr_id_def;
        flags_def = _ipfix_tr_port_config_flags_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (profile_def == NULL || id_def == NULL || flags_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_ipfix_init(unit, profile_def, id_def));
    }
    ipfix_ctrl = _bcm_ipfix_ctrl[unit];

    if (stage != BCM_IPFIX_STAGE_INGRESS && stage != BCM_IPFIX_STAGE_EGRESS) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ipfix_gport_resolve(unit, port, &port));

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    profile_mem = id_def->profile[stage];

    reg = id_def->port_config[stage];
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));

    /* Remove all previously programmed profile for the port */
    if (soc_reg_field_get(unit, reg, rval, IPFIX_ENABLEf)) {
        soc_reg_field_set(unit, reg, &rval, IPFIX_ENABLEf, 0);
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

        BCM_IF_ERROR_RETURN(_bcm_ipfix_profile_del(unit, profile_def, id_def,
                                                   stage, port, -1));
    }

    /* Profile */
    sal_memset(&profile_entry, 0, sizeof(profile_entry));
    for (count = 0, def = profile_def;
         def->type != _BCM_IPFIX_PROFILE_TYPE_NONE; count++, def++) {
        if (!soc_mem_field_valid(unit, profile_mem, def->index_field)) {
            continue;
        }
        profile = &ipfix_ctrl->profiles[def->stage[stage].profile_id];
        rv = BCM_E_NONE;
        if (def->type == _BCM_IPFIX_PROFILE_TYPE_REG) {
            reg = def->stage[stage].reg_mem_id[0];
            rval = 0;
            switch (reg) {
            case ING_IPFIX_PORT_RECORD_LIMIT_SETr:
            case EGR_IPFIX_PORT_RECORD_LIMIT_SETr:
                soc_reg_field_set(unit, reg, &rval, ENTRY_LIMITf,
                                  config->entry_limit);
                break;
            case ING_IPFIX_MINIMUM_LIVE_TIME_SETr:
            case EGR_IPFIX_MINIMUM_LIVE_TIME_SETr:
                soc_reg_field_set(unit, reg, &rval, MIN_RECORD_LIFEf,
                                  config->min_time);
                break;
            case ING_IPFIX_MAXIMUM_LIVE_TIME_SETr:
            case EGR_IPFIX_MAXIMUM_LIVE_TIME_SETr:
                soc_reg_field_set(unit, reg, &rval, MAX_RECORD_LIFEf,
                                  config->max_time);
                break;
            case ING_IPFIX_MAXIMUM_IDLE_AGE_SETr:
            case EGR_IPFIX_MAXIMUM_IDLE_AGE_SETr:
                soc_reg_field_set(unit, reg, &rval, MAX_IDLE_AGEf,
                                  config->max_idle_time);
                break;
            case ING_IPFIX_SAMPLING_LIMIT_SETr:
            case EGR_IPFIX_SAMPLING_LIMIT_SETr:
                soc_reg_field_set(unit, reg, &rval, SAMPLING_LIMITf,
                                  config->sample_rate);
                break;
            default:
                continue;
            }
            COMPILER_64_SET(rval64, 0, rval);
            rval64s[0] = &rval64;
            rv = soc_profile_reg_add(unit, profile, rval64s, 1, &index);
        } else if (def->type == _BCM_IPFIX_PROFILE_TYPE_MEM) {
            mems[0] = def->stage[stage].reg_mem_id[0];
            mems[1] = def->stage[stage].reg_mem_id[1];
            size = 1;
            switch (mems[0]) {
            case ING_IPFIX_DSCP_XLATE_TABLEm:
                sal_memset(&entry0, 0, sizeof(entry0));
                size = BCM_IPFIX_DSCP_MASK_COUNT;
                for (i = 0; i < size; i++) {
                    soc_mem_field32_set(unit, mems[0],
                                        &entry0.ing_ipfix_dscp_xlate_table[i],
                                        DSCP_KEYf, config->dscp_mask[i]);
                }
                break;
            case EGR_IPFIX_DSCP_XLATE_TABLEm:
                sal_memset(&entry0, 0, sizeof(entry0));
                size = BCM_IPFIX_DSCP_MASK_COUNT;
                for (i = 0; i < size; i++) {
                    soc_mem_field32_set(unit, mems[0],
                                        &entry0.egr_ipfix_dscp_xlate_table[i],
                                        DSCP_KEYf, config->dscp_mask[i]);
                }
                break;
            case ING_IPFIX_IPV4_MASK_SET_Am:
            case EGR_IPFIX_IPV4_MASK_SET_Am:
                sal_memset(&entry0, 0, sizeof(entry0));
                soc_mem_field_set(unit, mems[0], entry0.w, IPV4_SIP_MASKf,
                                  (uint32 *)&config->src_ip4_mask);
                soc_mem_field_set(unit, mems[0], entry0.w, IPV4_DIP_MASKf,
                                  (uint32 *)&config->dst_ip4_mask);
                if (mems[1] != INVALIDm) {
                    sal_memset(&entry1, 0, sizeof(entry1));
                    soc_mem_field_set(unit, mems[1], entry1.w,
                                      TUNNEL_IPV4_SIP_MASKf,
                                      (uint32 *)&config->tunnel_src_ip4_mask);
                    soc_mem_field_set(unit, mems[1], entry1.w,
                                      TUNNEL_IPV4_DIP_MASKf,
                                      (uint32 *)&config->tunnel_dst_ip4_mask);
                }
                break;
            case ING_IPFIX_IPV6_MASK_SET_Am:
            case EGR_IPFIX_IPV6_MASK_SET_Am:
                soc_mem_field_set(unit, mems[0], entry0.w, IPV6_SIP_MASKf,
                                  (uint32 *)&config->src_ip6_mask);
                soc_mem_field_set(unit, mems[0], entry0.w, IPV6_DIP_MASKf,
                                  (uint32 *)&config->dst_ip6_mask);
                if (mems[1] != INVALIDm) {
                    sal_memset(&entry1, 0, sizeof(entry1));
                    soc_mem_field_set(unit, mems[1], entry1.w,
                                      TUNNEL_IPV6_SIP_MASKf,
                                      (uint32 *)&config->tunnel_src_ip6_mask);
                    soc_mem_field_set(unit, mems[1], entry1.w,
                                      TUNNEL_IPV6_DIP_MASKf,
                                      (uint32 *)&config->tunnel_dst_ip6_mask);
                }
                break;
            default:
                continue;
            }
            entries[0] = &entry0;
            entries[1] = &entry1;
            rv = soc_profile_mem_add(unit, profile, entries, size, &index);
            index /= size;
        }
        if (SOC_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(_bcm_ipfix_profile_del(unit, profile_def,
                                                       id_def, stage, port,
                                                       count));
            return rv;
        }
        soc_mem_field32_set(unit, profile_mem, &profile_entry,
                            def->index_field, index);
    }
    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, profile_mem, MEM_BLOCK_ANY, port,
                       &profile_entry));

    /* Port config register */
    reg = id_def->port_config[stage];
    flags = config->flags;
    rval = 0;
    for (i = 0; flags_def[i].bit_mask; i++) {
        if (flags & flags_def[i].bit_mask) {
            soc_reg_field_set(unit, reg, &rval, flags_def[i].field, 1);
        }
    }
    fval = (flags & BCM_IPFIX_CONFIG_ENABLE_IP4 ? 1 : 0) |
           (flags & BCM_IPFIX_CONFIG_ENABLE_IP6 ? 2 : 0) |
           (flags & BCM_IPFIX_CONFIG_ENABLE_NON_IP ? 4 : 0);
    soc_reg_field_set(unit, reg, &rval, IPFIX_ENABLEf, fval);
    fval = (flags & BCM_IPFIX_CONFIG_KEY_IP4_USE_L2 ? 0 : 1) |
           (flags & BCM_IPFIX_CONFIG_KEY_IP6_USE_L2 ? 0 : 2);
    soc_reg_field_set(unit, reg, &rval, IPFIX_KEY_SELECTf, fval);
    if (config->sample_rate > 1) {
        soc_reg_field_set(unit, reg, &rval, IPFIX_SAMPLE_MODEf, 1);
    }

    if (flags & (BCM_IPFIX_CONFIG_RECORD_DISCARD_PKT |
                 BCM_IPFIX_CONFIG_RECORD_NON_DISCARD_PKT)) {
        /* Record all packets */
        fval = 1;
    } else if (flags & BCM_IPFIX_CONFIG_RECORD_DISCARD_PKT) {
        /* Record only discarded packets */
        fval = 2;
    } else if (flags & BCM_IPFIX_CONFIG_RECORD_NON_DISCARD_PKT) {
        /* Record only non-discarded packets */
        fval = 0;
    } else {
        /* Don't Record any packets */
        fval = 3;
    }

    soc_reg_field_set(unit, reg, &rval, DISCARDED_FLOWS_MODEf, fval);
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)|| SOC_IS_TRIUMPH3(unit)) {
        if (flags & BCM_IPFIX_CONFIG_KEY_SOURCE_PORT_OR_INTERFACE) {
            soc_reg_field_set(unit, reg, &rval, IP_IPFIX_INTF_ENABLEf, 1);
        }
        if (flags & BCM_IPFIX_CONFIG_TCP_FLAGS_LAST) {
            soc_reg_field_set(unit, reg, &rval, TCP_FLAG_MODEf, 1);
        }
    }
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_config_get
 * Description:
 *     To get per port IPFIX configuration
 * Parameters:
 *     unit            device number
 *     stage           BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port            port number
 *     config          pointer to ipfix configuration
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 *     BCM_E_PORT
 */
int
bcm_esw_ipfix_config_get(int unit,
                         bcm_ipfix_stage_t stage,
                         bcm_port_t port,
                         bcm_ipfix_config_t *config)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_profile_def_t *profile_def = NULL, *def;
    const _bcm_ipfix_id_def_t *id_def = NULL;
    const _bcm_ipfix_flags_def_t *flags_def = NULL;
    union {
        ing_ipfix_profile_entry_t ing_ipfix_profile;
        egr_ipfix_profile_entry_t egr_ipfix_profile;
    } profile_entry;
    union {
        ing_ipfix_dscp_xlate_table_entry_t ing_ipfix_dscp_xlate_table;
        ing_ipfix_ipv4_mask_set_a_entry_t ing_ipfix_ipv4_mask_set_a;
        ing_ipfix_ipv4_mask_set_b_entry_t ing_ipfix_ipv4_mask_set_b;
        ing_ipfix_ipv6_mask_set_a_entry_t ing_ipfix_ipv6_mask_set_a;
        ing_ipfix_ipv6_mask_set_b_entry_t ing_ipfix_ipv6_mask_set_b;
        egr_ipfix_dscp_xlate_table_entry_t egr_ipfix_dscp_xlate_table;
        egr_ipfix_ipv4_mask_set_a_entry_t egr_ipfix_ipv4_mask_set_a;
        egr_ipfix_ipv6_mask_set_a_entry_t egr_ipfix_ipv6_mask_set_a;
        uint32 w[1];
    } entry;
    soc_reg_t reg;
    soc_mem_t mems[2], profile_mem;
    uint32 rval, fval, index, flags;
    int i;

    if (!soc_feature(unit, soc_feature_ipfix)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        profile_def = _ipfix_tr_profile_def;
        id_def = &_ipfix_tr_id_def;
        flags_def = _ipfix_tr_port_config_flags_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (profile_def == NULL || id_def == NULL || flags_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }

    if ((stage != BCM_IPFIX_STAGE_INGRESS && stage != BCM_IPFIX_STAGE_EGRESS)
        || config == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ipfix_gport_resolve(unit, port, &port));

    profile_mem = id_def->profile[stage];

    /* Profile */
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY, port, &profile_entry));
    for (def = profile_def; def->type != _BCM_IPFIX_PROFILE_TYPE_NONE; def++) {
        if (!soc_mem_field_valid(unit, profile_mem, def->index_field)) {
            continue;
        }
        index = soc_mem_field32_get(unit, profile_mem, &profile_entry,
                                    def->index_field);
        if (def->type == _BCM_IPFIX_PROFILE_TYPE_REG) {
            reg = def->stage[stage].reg_mem_id[0];
            BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, index, &rval));
            switch (reg) {
            case ING_IPFIX_PORT_RECORD_LIMIT_SETr:
            case EGR_IPFIX_PORT_RECORD_LIMIT_SETr:
                config->entry_limit = soc_reg_field_get(unit, reg, rval,
                                                        ENTRY_LIMITf);
                break;
            case ING_IPFIX_MINIMUM_LIVE_TIME_SETr:
            case EGR_IPFIX_MINIMUM_LIVE_TIME_SETr:
                config->min_time = soc_reg_field_get(unit, reg, rval,
                                                     MIN_RECORD_LIFEf);
                break;
            case ING_IPFIX_MAXIMUM_LIVE_TIME_SETr:
            case EGR_IPFIX_MAXIMUM_LIVE_TIME_SETr:
                config->max_time = soc_reg_field_get(unit, reg, rval,
                                                     MAX_RECORD_LIFEf);
                break;
            case ING_IPFIX_MAXIMUM_IDLE_AGE_SETr:
            case EGR_IPFIX_MAXIMUM_IDLE_AGE_SETr:
                config->max_idle_time = soc_reg_field_get(unit, reg, rval,
                                                          MAX_IDLE_AGEf);
                break;
            case ING_IPFIX_SAMPLING_LIMIT_SETr:
            case EGR_IPFIX_SAMPLING_LIMIT_SETr:
                config->sample_rate = soc_reg_field_get(unit, reg, rval,
                                                        SAMPLING_LIMITf);
                break;
            default:
                continue;
            }
        } else if (def->type == _BCM_IPFIX_PROFILE_TYPE_MEM) {
            mems[0] = def->stage[stage].reg_mem_id[0];
            mems[1] = def->stage[stage].reg_mem_id[1];
            switch (mems[0]) {
            case ING_IPFIX_DSCP_XLATE_TABLEm:
            case EGR_IPFIX_DSCP_XLATE_TABLEm:
                for (i = 0; i < BCM_IPFIX_DSCP_MASK_COUNT; i++) {
                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mems[0],
                                        MEM_BLOCK_ANY, index * 64 + i,
                                        &entry));
                    config->dscp_mask[i] =
                        soc_mem_field32_get(unit, mems[0], entry.w, DSCP_KEYf);
                }
                break;
            case ING_IPFIX_IPV4_MASK_SET_Am:
            case EGR_IPFIX_IPV4_MASK_SET_Am:
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, mems[0], MEM_BLOCK_ANY, index,
                                  &entry));
                soc_mem_field_get(unit, mems[0], entry.w, IPV4_SIP_MASKf,
                                  (uint32 *)&config->src_ip4_mask);
                soc_mem_field_get(unit, mems[0], entry.w, IPV4_DIP_MASKf,
                                  (uint32 *)&config->dst_ip4_mask);
                if (mems[1] != INVALIDm) {
                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mems[1],
                                                     MEM_BLOCK_ANY, index,
                                                     &entry));
                    soc_mem_field_get(unit, mems[1], entry.w,
                                      TUNNEL_IPV4_SIP_MASKf,
                                      (uint32 *)&config->tunnel_src_ip4_mask);
                    soc_mem_field_get(unit, mems[1], entry.w,
                                      TUNNEL_IPV4_DIP_MASKf,
                                      (uint32 *)&config->tunnel_dst_ip4_mask);
                }
                break;
            case ING_IPFIX_IPV6_MASK_SET_Am:
            case EGR_IPFIX_IPV6_MASK_SET_Am:
                BCM_IF_ERROR_RETURN
                    (soc_mem_read(unit, mems[0], MEM_BLOCK_ANY, index,
                                  &entry));
                soc_mem_field_get(unit, mems[0], entry.w, IPV6_SIP_MASKf,
                                  (uint32 *)&config->src_ip6_mask);
                soc_mem_field_get(unit, mems[0], entry.w, IPV6_DIP_MASKf,
                                  (uint32 *)&config->dst_ip6_mask);
                if (mems[1] != INVALIDm) {
                    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mems[1],
                                                     MEM_BLOCK_ANY, index,
                                                     &entry));
                    soc_mem_field_get(unit, mems[1], entry.w,
                                      TUNNEL_IPV6_SIP_MASKf,
                                      (uint32 *)&config->tunnel_src_ip6_mask);
                    soc_mem_field_get(unit, mems[1], entry.w,
                                      TUNNEL_IPV6_DIP_MASKf,
                                      (uint32 *)&config->tunnel_dst_ip6_mask);
                }
                break;
            default:
                continue;
            }
        }
    }

    /* Port config register */
    reg = id_def->port_config[stage];
    flags = 0;
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
    for (i = 0; flags_def[i].bit_mask; i++) {
        if (soc_reg_field_get(unit, reg, rval, flags_def[i].field)) {
            flags |= flags_def[i].bit_mask;
        }
    }
    fval = soc_reg_field_get(unit, reg, rval, IPFIX_ENABLEf);
    flags |= ((fval & 1 ? BCM_IPFIX_CONFIG_ENABLE_IP4 : 0) |
              (fval & 2 ? BCM_IPFIX_CONFIG_ENABLE_IP6 : 0) |
              (fval & 4 ? BCM_IPFIX_CONFIG_ENABLE_NON_IP : 0));
    fval = soc_reg_field_get(unit, reg, rval, IPFIX_KEY_SELECTf);
    flags |= ((fval & 1 ? 0 : BCM_IPFIX_CONFIG_KEY_IP4_USE_L2) |
              (fval & 2 ? 0 : BCM_IPFIX_CONFIG_KEY_IP6_USE_L2));
    fval = soc_reg_field_get(unit, reg, rval, IPFIX_SAMPLE_MODEf);
    if (!fval) {
        config->sample_rate = 0;
    }
    fval = soc_reg_field_get(unit, reg, rval, DISCARDED_FLOWS_MODEf);
    if (fval == 0) {
        flags |= BCM_IPFIX_CONFIG_RECORD_NON_DISCARD_PKT;
    } else if (fval == 1) {
        flags |= (BCM_IPFIX_CONFIG_RECORD_NON_DISCARD_PKT |
                  BCM_IPFIX_CONFIG_RECORD_DISCARD_PKT);
    } else if (fval == 2) {
        flags |= BCM_IPFIX_CONFIG_RECORD_DISCARD_PKT;
    }
    config->flags = flags;

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_rate_create
 * Description:
 *     Add an IPFIX flow rate meter entry
 * Parameters:
 *     unit            device number
 *     rate_info       pointer to rate information
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_RESOURCE
 *     BCM_E_UNAVAIL
 */
int
bcm_esw_ipfix_rate_create(int unit,
                          bcm_ipfix_rate_t *rate_info)
{
#ifdef BCM_IPFIX_SUPPORT
    _bcm_ipfix_ctrl_t *ipfix_ctrl;
    const _bcm_ipfix_profile_def_t *profile_def = NULL;
    const _bcm_ipfix_id_def_t *id_def = NULL;
    ing_ipfix_flow_rate_meter_table_entry_t entry;
    bcm_ipfix_rate_id_t free_rate_id;
    int rv, size, bmp_size, i;
    uint32 bits, cng, rval;

    if (!soc_feature(unit, soc_feature_ipfix_rate)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        profile_def = _ipfix_tr_profile_def;
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (profile_def == NULL || id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_ipfix_init(unit, profile_def, id_def));
    }
    ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    size = soc_mem_index_count(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm);

    if (rate_info == NULL) {
        return BCM_E_PARAM;
    }

    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_WITH_ID) {
        rv = _bcm_ipfix_rate_id_check(unit, rate_info->rate_id);
        if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_REPLACE) {
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        } else {
            if (BCM_SUCCESS(rv)) {
                return BCM_E_EXISTS;
            } else if (rv != BCM_E_NOT_FOUND) {
                return rv;
            }
        }
    } else {
        if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_REPLACE) {
            return BCM_E_PARAM;
        }
    }

    if (rate_info->limit >=
        (1 << soc_mem_field_length(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                   BUCKETSIZEf))) {
        return BCM_E_PARAM;
    }

    if (rate_info->rate >=
        (1 << soc_mem_field_length(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                   REFRESHCOUNTf))) {
        return BCM_E_PARAM;
    }

    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_DSCP_SET) {
        if (rate_info->dscp > 63) {
            return BCM_E_PARAM;
        }
    }

    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_COLOR_SET) {
        if (rate_info->color < 0 || rate_info->color >= bcmColorCount) {
            return BCM_E_PARAM;
        }
    }

    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_PKT_PRI_SET) {
        if (rate_info->pkt_pri < 0 || rate_info->pkt_pri > 7) {
            return BCM_E_PARAM;
        }
    }

    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_INT_PRI_SET) {
        if (rate_info->int_pri < 0 ||
            rate_info->int_pri >=
            (1 << soc_mem_field_length(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                       NEW_INT_PRIf))) {

            return BCM_E_PARAM;
        }
    }

    if (!(rate_info->flags & BCM_IPFIX_RATE_VIOLATION_WITH_ID)) {
        free_rate_id = size;
        bmp_size = _SHR_BITDCLSIZE(size);
        /* find the index of first zero from rate_used_bmp */
        for (i = 0; i < bmp_size; i++) {
            if (ipfix_ctrl->rate_used_bmp[i] == 0xffffffff) {
                continue;
            }
            bits = ipfix_ctrl->rate_used_bmp[i];
            bits &= (bits << 1) | 0x00000001;
            bits &= (bits << 2) | 0x00000003;
            bits &= (bits << 4) | 0x0000000f;
            bits &= (bits << 8) | 0x000000ff;
            bits &= (bits << 16) | 0x0000ffff;
            free_rate_id = (i << 5) + _shr_popcount(bits);
            break;
        }
        if (free_rate_id >= size) {
            return BCM_E_RESOURCE;
        }
        rate_info->rate_id = free_rate_id;
    }

    sal_memset(&entry, 0, sizeof(entry));
    soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                        BUCKETSIZEf, rate_info->limit);
    soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                        REFRESHCOUNTf, rate_info->rate);
    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_DROP) {
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            DROPf, 1);
    }
    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_COPY_TO_CPU) {
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            COPY_TO_CPUf, 1);
    }
    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_DSCP_SET) {
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            CHANGE_DSCPf, 1);
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            NEW_DSCPf, rate_info->dscp);
    }
    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_COLOR_SET) {
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            CHANGE_CNGf, 1);
        cng = _BCM_COLOR_ENCODING(unit, rate_info->color);
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            NEW_CNGf, cng);
    }
    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_PKT_PRI_SET) {
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            CHANGE_PKT_PRIf, 1);
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            NEW_PRIf, rate_info->pkt_pri);
    }
    if (rate_info->flags & BCM_IPFIX_RATE_VIOLATION_INT_PRI_SET) {
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            CHANGE_INT_PRIf, 1);
        soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                            NEW_INT_PRIf, rate_info->int_pri);
    }
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                      MEM_BLOCK_ANY, rate_info->rate_id,
                                      &entry));

    SHR_BITSET(ipfix_ctrl->rate_used_bmp, rate_info->rate_id);

    if (!(rate_info->flags & BCM_IPFIX_RATE_VIOLATION_REPLACE)) {
        if (!ipfix_ctrl->rate_used_count) {
            BCM_IF_ERROR_RETURN(READ_ING_IPFIX_CONFIGr(unit, &rval));
            soc_reg_field_set(unit, ING_IPFIX_CONFIGr, &rval,
                              FLOW_RATE_ENABLEf, 1);
            BCM_IF_ERROR_RETURN(WRITE_ING_IPFIX_CONFIGr(unit, rval));
        }
        ipfix_ctrl->rate_used_count++;
    }

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_rate_destroy
 * Description:
 *     Delete an IPFIX flow rate meter entry
 * Parameters:
 *     unit            device number
 *     rate_id         rate identifier
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 *     BCM_E_BADID
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcm_esw_ipfix_rate_destroy(int unit,
                           bcm_ipfix_rate_id_t rate_id)
{
#ifdef BCM_IPFIX_SUPPORT
    _bcm_ipfix_ctrl_t *ipfix_ctrl;
    uint32 rval;

    BCM_IF_ERROR_RETURN(_bcm_ipfix_rate_id_check(unit, rate_id));

    ipfix_ctrl = _bcm_ipfix_ctrl[unit];

    SHR_BITCLR(ipfix_ctrl->rate_used_bmp, rate_id);

    ipfix_ctrl->rate_used_count--;
    if (!ipfix_ctrl->rate_used_count) {
        BCM_IF_ERROR_RETURN(READ_ING_IPFIX_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, ING_IPFIX_CONFIGr, &rval, FLOW_RATE_ENABLEf,
                          0);
        BCM_IF_ERROR_RETURN(WRITE_ING_IPFIX_CONFIGr(unit, rval));
    }

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_rate_get
 * Description:
 *     Get IPFIX flow rate meter entry for the specified id
 * Parameters:
 *     unit            device number
 *     rate_info       pointer to rate information carrying rate identifier
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_NOT_FOUND
 *     BCM_E_BADID
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcm_esw_ipfix_rate_get(int unit,
                       bcm_ipfix_rate_t *rate_info)
{
#ifdef BCM_IPFIX_SUPPORT
    if (rate_info == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ipfix_rate_id_check(unit, rate_info->rate_id));

    return _bcm_ipfix_rate_get(unit, rate_info->rate_id, rate_info);
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_rate_traverse
 * Description:
 *     Traverse through IPFIX flow rate meter entries
 * Parameters:
 *     unit            device number
 *     cb              user callback function
 *     userdata        user data used as argument during callback
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcm_esw_ipfix_rate_traverse(int unit,
                            bcm_ipfix_rate_traverse_cb cb,
                            void *userdata)
{
#ifdef BCM_IPFIX_SUPPORT
    bcm_ipfix_rate_t rate_info;
    int size, index;

    if (!soc_feature(unit, soc_feature_ipfix_rate)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (cb == NULL) {
        return BCM_E_PARAM;
    }

    size = soc_mem_index_count(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm);
    for (index = 1; index < size; index++) {
        if (!SHR_BITGET(_bcm_ipfix_ctrl[unit]->rate_used_bmp, index)) {
            continue;
        }
        BCM_IF_ERROR_RETURN(_bcm_ipfix_rate_get(unit, index, &rate_info));
        (*cb)(unit, &rate_info, userdata);
    }

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_rate_destroy_all
 * Description:
 *     Delete all IPFIX flow rate meter entries
 * Parameters:
 *     unit            device number
 * Return:
 *     BCM_E_NONE
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcm_esw_ipfix_rate_destroy_all(int unit)
{
#ifdef BCM_IPFIX_SUPPORT
    _bcm_ipfix_ctrl_t *ipfix_ctrl;
    int size;

    if (!soc_feature(unit, soc_feature_ipfix_rate)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }
    ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    size = soc_mem_index_count(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm);

    sal_memset(ipfix_ctrl->rate_used_bmp, 0, SHR_BITALLOCSIZE(size));
    SHR_BITSET(ipfix_ctrl->rate_used_bmp, 0); /* index 0 is reserved */

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_rate_mirror_add
 * Description:
 *     Add a mirror destination to the IPFIX flow rate meter entry
 * Parameters:
 *     unit            device number
 *     rate_id         rate identifier
 *     mirror_dest_id  pre-allocated mirror destination identifier
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 *     BCM_E_EXISTS
 *     BCM_E_BADID
 *     BCM_E_RESOURCE
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcm_esw_ipfix_rate_mirror_add(int unit,
                              bcm_ipfix_rate_id_t rate_id,
                              bcm_gport_t mirror_dest_id)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_id_def_t *id_def = NULL;
    ing_ipfix_flow_rate_meter_table_entry_t entry;
    uint32 mirror_bmp;
    int mtp_index, free_index, index;

    BCM_IF_ERROR_RETURN(_bcm_ipfix_rate_id_check(unit, rate_id));

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (!BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        return BCM_E_BADID;
    }
    mtp_index = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                     MEM_BLOCK_ANY, rate_id, &entry));
    mirror_bmp = soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                     &entry, MIRRORf);

    free_index = -1;
    for (index = _BCM_IPFIX_RATE_MIRROR_COUNT - 1; index >= 0; index--) {
        if (!(mirror_bmp & (1 << index))) {
            free_index = index;
            continue;
        }
        if (soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                                id_def->mtp_index[index]) == mtp_index) {
            return BCM_E_EXISTS;
        }
    }
    if (free_index < 0) {
        return BCM_E_RESOURCE;
    }

    soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                        id_def->mtp_index[free_index], mtp_index);
    soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                        MIRRORf, mirror_bmp | (1 << free_index));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                      MEM_BLOCK_ANY, rate_id, &entry));

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_rate_mirror_delete
 * Description:
 *     Delete a mirror destination from the IPFIX flow rate meter entry
 * Parameters:
 *     unit            device number
 *     rate_id         rate identifier
 *     mirror_dest_id  pre-allocated mirror destination identifier
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 *     BCM_E_BADID
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcm_esw_ipfix_rate_mirror_delete(int unit,
                                 bcm_ipfix_rate_id_t rate_id,
                                 bcm_gport_t mirror_dest_id)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_id_def_t *id_def = NULL;
    ing_ipfix_flow_rate_meter_table_entry_t entry;
    uint32 mirror_bmp;
    int mtp_index, index;

    BCM_IF_ERROR_RETURN(_bcm_ipfix_rate_id_check(unit, rate_id));

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (!BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        return BCM_E_BADID;
    }
    mtp_index = BCM_GPORT_MIRROR_GET(mirror_dest_id);

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                     MEM_BLOCK_ANY, rate_id, &entry));
    mirror_bmp = soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                     &entry, MIRRORf);

    for (index = 0; index < _BCM_IPFIX_RATE_MIRROR_COUNT; index++) {
        if (!(mirror_bmp & (1 << index))) {
            continue;
        }
        if (soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                                id_def->mtp_index[index]) == mtp_index) {
            break;
        }
    }
    if (index == _BCM_IPFIX_RATE_MIRROR_COUNT) {
        return BCM_E_NOT_FOUND;
    }

    soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                        MIRRORf, mirror_bmp ^ (1 << index));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                      MEM_BLOCK_ANY, rate_id, &entry));

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_rate_mirror_delete_all
 * Description:
 *     Delete all mirror destination associated to the IPFIX flow rate meter
 *     entry
 * Parameters:
 *     unit            device number
 *     rate_id         rate identifier
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 *     BCM_E_BADID
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcm_esw_ipfix_rate_mirror_delete_all(int unit,
                                     bcm_ipfix_rate_id_t rate_id)
{
#ifdef BCM_IPFIX_SUPPORT
    ing_ipfix_flow_rate_meter_table_entry_t entry;

    BCM_IF_ERROR_RETURN(_bcm_ipfix_rate_id_check(unit, rate_id));

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                     MEM_BLOCK_ANY, rate_id, &entry));
    soc_mem_field32_set(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm, &entry,
                        MIRRORf, 0);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                      MEM_BLOCK_ANY, rate_id, &entry));

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_rate_mirror_get
 * Description:
 *     Get all mirror destination from the IPFIX flow rate meter entry
 * Parameters:
 *     unit              device number
 *     rate_id           rate identifier
 *     mirror_dest_size  size of mirror_dest_id array
 *     mirror_dest_id    array to store mirror destination identifier
 *     mirror_dest_count number of entries stored in the mirror_dest_id array
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 *     BCM_E_BADID
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 */
int
bcm_esw_ipfix_rate_mirror_get(int unit,
                              bcm_ipfix_rate_id_t rate_id,
                              int mirror_dest_size,
                              bcm_gport_t *mirror_dest_id,
                              int *mirror_dest_count)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_id_def_t *id_def = NULL;
    ing_ipfix_flow_rate_meter_table_entry_t entry;
    uint32 mirror_bmp;
    int mtp_index, index;

    BCM_IF_ERROR_RETURN(_bcm_ipfix_rate_id_check(unit, rate_id));

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (mirror_dest_id == NULL || mirror_dest_count == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                     MEM_BLOCK_ANY, rate_id, &entry));

    mirror_bmp = soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                     &entry, MIRRORf);

    *mirror_dest_count = 0;
    for (index = 0; index < _BCM_IPFIX_RATE_MIRROR_COUNT; index++) {
        if (!(mirror_bmp & (1 << index))) {
            continue;
        }
        if (*mirror_dest_count >= mirror_dest_size) {
            break;
        }
        mtp_index = soc_mem_field32_get(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm,
                                        &entry, id_def->mtp_index[index]);
        BCM_GPORT_MIRROR_SET(mirror_dest_id[*mirror_dest_count], mtp_index);
        (*mirror_dest_count)++;
    }

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_mirror_config_set
 * Description:
 *     Set IPFIX mirror control configuration of the specified port
 * Parameters:
 *     unit            device number
 *     stage           BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port            generic port identifier
 *     config          pointer to ipfix mirror configuration
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 *     BCM_E_PORT
 */
int
bcm_esw_ipfix_mirror_config_set(int unit,
                                bcm_ipfix_stage_t stage,
                                bcm_gport_t port,
                                bcm_ipfix_mirror_config_t *config)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_id_def_t *id_def = NULL;
    soc_reg_t reg;
    uint64 rval64;

    if (!soc_feature(unit, soc_feature_ipfix_flow_mirror)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (stage != BCM_IPFIX_STAGE_INGRESS && stage != BCM_IPFIX_STAGE_EGRESS) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ipfix_gport_resolve(unit, port, &port));

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    reg = id_def->mirror_control[stage];

    if (config->pkt_count >=
        (1 << soc_reg_field_length(unit, reg, MIRROR_COUNTf))) {
        return BCM_E_PARAM;
    }

    if (config->tcp_flags_mask >=
        (1 << soc_reg_field_length(unit, reg, TCP_FLAG_MASKf))) {
        return BCM_E_PARAM;
    }

    if (config->flags & BCM_IPFIX_MIRROR_CONFIG_TTL_OFFSET_MAX) {
        if (config->ttl_offset_max >=
            (1 << soc_reg_field_length(unit, reg, TTL_RANGE_VALf))) {
            return BCM_E_PARAM;
        }
    }

    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, MIRROR_COUNTf,
                          config->pkt_count);
    soc_reg64_field32_set(unit, reg, &rval64, TCP_FLAG_MASKf,
                          config->tcp_flags_mask);
    if (config->flags & BCM_IPFIX_MIRROR_CONFIG_TTL_OFFSET_MAX) {
        soc_reg64_field32_set(unit, reg, &rval64, TTL_RANGE_CHECK_ENABLEf, 1);
        soc_reg64_field32_set(unit, reg, &rval64, TTL_RANGE_VALf,
                              config->ttl_offset_max);
    }
    if (config->flags & BCM_IPFIX_MIRROR_CONFIG_FRAGMENT) {
        soc_reg64_field32_set(unit, reg, &rval64, FRAGMENT_MASKf, 1);
    }
    if (config->flags & BCM_IPFIX_MIRROR_CONFIG_NON_FRAGMENT) {
        soc_reg64_field32_set(unit, reg, &rval64, NON_FRAGMENT_MASKf, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_mirror_config_get
 * Description:
 *     Get IPFIX mirror control configuration of the specified port
 * Parameters:
 *     unit            device number
 *     stage           BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port            generic port identifier
 *     config          pointer to ipfix mirror configuration
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 *     BCM_E_PORT
 */
int
bcm_esw_ipfix_mirror_config_get(int unit,
                                bcm_ipfix_stage_t stage,
                                bcm_gport_t port,
                                bcm_ipfix_mirror_config_t *config)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_id_def_t *id_def = NULL;
    soc_reg_t reg;
    uint64 rval64;

    if (!soc_feature(unit, soc_feature_ipfix_flow_mirror)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (stage != BCM_IPFIX_STAGE_INGRESS && stage != BCM_IPFIX_STAGE_EGRESS) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ipfix_gport_resolve(unit, port, &port));

    if (config == NULL) {
        return BCM_E_PARAM;
    }

    reg = id_def->mirror_control[stage];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    config->flags = 0;
    config->pkt_count = soc_reg64_field32_get(unit, reg, rval64,
                                              MIRROR_COUNTf);
    config->tcp_flags_mask = soc_reg64_field32_get(unit, reg, rval64,
                                                   TCP_FLAG_MASKf);
    if (soc_reg64_field32_get(unit, reg, rval64, TTL_RANGE_CHECK_ENABLEf)) {
        config->flags |= BCM_IPFIX_MIRROR_CONFIG_TTL_OFFSET_MAX;
        config->ttl_offset_max = soc_reg64_field32_get(unit, reg, rval64,
                                                       TTL_RANGE_VALf);
    }
    if (soc_reg64_field32_get(unit, reg, rval64, FRAGMENT_MASKf)) {
        config->flags |= BCM_IPFIX_MIRROR_CONFIG_FRAGMENT;
    }
    if (soc_reg64_field32_get(unit, reg, rval64, NON_FRAGMENT_MASKf)) {
        config->flags |= BCM_IPFIX_MIRROR_CONFIG_NON_FRAGMENT;
    }

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_mirror_port_dest_add
 * Description:
 *     Add an IPFIX mirror destination to the specified port
 * Parameters:
 *     unit            device number
 *     stage           BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port            generic port identifier
 *     mirror_dest_id  pre-allocated mirror destination identifier
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_EXISTS
 *     BCM_E_BADID
 *     BCM_E_RESOURCE
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 *     BCM_E_PORT
 */
int
bcm_esw_ipfix_mirror_port_dest_add(int unit,
                                   bcm_ipfix_stage_t stage,
                                   bcm_gport_t port,
                                   bcm_gport_t mirror_dest_id)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_id_def_t *id_def = NULL;
    soc_reg_t reg;
    uint64 rval64;
    uint32 mirror_bmp;
    int mtp_index, free_index, index;
#ifdef BCM_TRIUMPH_SUPPORT
    uint32 flags = BCM_MIRROR_PORT_INGRESS;
    int rv;
#endif /* BCM_TRIUMPH_SUPPORT */

    if (!soc_feature(unit, soc_feature_ipfix_flow_mirror)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (stage != BCM_IPFIX_STAGE_INGRESS && stage != BCM_IPFIX_STAGE_EGRESS) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ipfix_gport_resolve(unit, port, &port));

    if (!BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        return BCM_E_BADID;
    }

    reg = id_def->mirror_control[stage];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_APOLLO(unit)) {
        if (stage == BCM_IPFIX_STAGE_EGRESS) {
            flags = BCM_MIRROR_PORT_EGRESS_TRUE;
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_mirror_mtp_reserve(unit, mirror_dest_id, flags, &mtp_index));
    } else
#endif /* BCM_TRIUMPH_SUPPORT */
    {
        mtp_index = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    }

    mirror_bmp = soc_reg64_field32_get(unit, reg, rval64, MIRRORf);

    free_index = -1;
    for (index = _BCM_IPFIX_RATE_MIRROR_COUNT - 1; index >= 0; index--) {
        if (!(mirror_bmp & (1 << index))) {
            free_index = index;
            continue;
        }
        if (soc_reg64_field32_get(unit, reg, rval64,
                                  id_def->mtp_index[index]) == mtp_index) {
            return BCM_E_EXISTS;
        }
    }
    if (free_index < 0) {
#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_APOLLO(unit)) {
        _bcm_esw_mirror_mtp_unreserve(unit, mtp_index, TRUE, flags);
    }
#endif /* BCM_TRIUMPH_SUPPORT */

        return BCM_E_RESOURCE;
    }

    soc_reg64_field32_set(unit, reg, &rval64, id_def->mtp_index[free_index],
                          mtp_index);
    soc_reg64_field32_set(unit, reg, &rval64, MIRRORf,
                          mirror_bmp | (1 << free_index));
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_APOLLO(unit)) {
        rv = soc_reg_set(unit, reg, port, 0, rval64);
        if (BCM_FAILURE(rv)) {
            _bcm_esw_mirror_mtp_unreserve(unit, mtp_index, TRUE, flags);
            return rv;
        }

        if (stage == BCM_IPFIX_STAGE_INGRESS) {
            /*INGRESS rely on MIRROR_CONTROL, EGRSS_TRUE donot rely on this*/
            BCM_IF_ERROR_RETURN(_bcm_esw_mirror_enable_set(unit, port, TRUE));
        }
    } else
#endif /* BCM_TRIUMPH_SUPPORT */
    {
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    }

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_mirror_port_dest_delete
 * Description:
 *     Delete an IPFIX mirror destination from the specified port
 * Parameters:
 *     unit            device number
 *     stage           BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port            generic port identifier
 *     mirror_dest_id  pre-allocated mirror destination identifier
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_NOT_FOUND
 *     BCM_E_BADID
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 *     BCM_E_PORT
 */
int
bcm_esw_ipfix_mirror_port_dest_delete(int unit,
                                      bcm_ipfix_stage_t stage,
                                      bcm_gport_t port,
                                      bcm_gport_t mirror_dest_id)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_id_def_t *id_def = NULL;
    soc_reg_t reg;
    uint64 rval64;
    uint32 mirror_bmp;
    int mtp_index, index;
#ifdef BCM_TRIUMPH_SUPPORT
    uint32 flags = BCM_MIRROR_PORT_INGRESS;
#endif /* BCM_TRIUMPH_SUPPORT */

    if (!soc_feature(unit, soc_feature_ipfix_flow_mirror)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (stage != BCM_IPFIX_STAGE_INGRESS && stage != BCM_IPFIX_STAGE_EGRESS) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ipfix_gport_resolve(unit, port, &port));

    if (!BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        return BCM_E_BADID;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_APOLLO(unit)) {
        if (stage == BCM_IPFIX_STAGE_EGRESS) {
            flags = BCM_MIRROR_PORT_EGRESS_TRUE;
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_mirror_mtp_match(unit, mirror_dest_id, flags, &mtp_index));
    } else
#endif /* BCM_TRIUMPH_SUPPORT */
    {
        mtp_index = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    }

    reg = id_def->mirror_control[stage];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    mirror_bmp = soc_reg64_field32_get(unit, reg, rval64, MIRRORf);

    for (index = 0; index < _BCM_IPFIX_RATE_MIRROR_COUNT; index++) {
        if (!(mirror_bmp & (1 << index))) {
            continue;
        }
        if (soc_reg64_field32_get(unit, reg, rval64,
                                  id_def->mtp_index[index]) == mtp_index) {
            soc_reg64_field32_set(unit, reg, &rval64, id_def->mtp_index[index], 0);
            break;
        }
    }
    if (index == _BCM_IPFIX_RATE_MIRROR_COUNT) {
        return BCM_E_NOT_FOUND;
    }

    soc_reg64_field32_set(unit, reg, &rval64, MIRRORf,
                          mirror_bmp ^ (1 << index));
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_APOLLO(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_mirror_mtp_unreserve(unit, mtp_index, TRUE, flags));
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_mirror_port_dest_delete_all
 * Description:
 *     Delete all IPFIX mirror destination from the specified port
 * Parameters:
 *     unit            device number
 *     stage           BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port            generic port identifier
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 *     BCM_E_PORT
 */
int
bcm_esw_ipfix_mirror_port_dest_delete_all(int unit,
                                          bcm_ipfix_stage_t stage,
                                          bcm_gport_t port)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_id_def_t *id_def = NULL;
    soc_reg_t reg;
    uint64 rval64;
#ifdef BCM_TRIUMPH_SUPPORT
    uint32 mirror_bmp;
    int mtp_index, index;
#endif /* BCM_TRIUMPH_SUPPORT */

    if (!soc_feature(unit, soc_feature_ipfix_flow_mirror)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (stage != BCM_IPFIX_STAGE_INGRESS && stage != BCM_IPFIX_STAGE_EGRESS) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ipfix_gport_resolve(unit, port, &port));

    reg = id_def->mirror_control[stage];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_APOLLO(unit)) {
        mirror_bmp = soc_reg64_field32_get(unit, reg, rval64, MIRRORf);
        for (index = 0; index < _BCM_IPFIX_RATE_MIRROR_COUNT; index++) {
            if (!(mirror_bmp & (1 << index))) {
                continue;
            }

            mtp_index = soc_reg64_field32_get(unit, reg, rval64,
                                        id_def->mtp_index[index]);

            if (stage == BCM_IPFIX_STAGE_INGRESS) {
                BCM_IF_ERROR_RETURN(_bcm_esw_mirror_mtp_unreserve(unit, mtp_index,
                                           TRUE, BCM_MIRROR_PORT_INGRESS));
            } else {
                BCM_IF_ERROR_RETURN(_bcm_esw_mirror_mtp_unreserve(unit, mtp_index,
                                           TRUE, BCM_MIRROR_PORT_EGRESS_TRUE));
            }

            soc_reg64_field32_set(unit, reg, &rval64, id_def->mtp_index[index], 0);
        }
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    soc_reg64_field32_set(unit, reg, &rval64, MIRRORf, 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_ipfix_mirror_port_dest_get
 * Description:
 *     Get all IPFIX mirror destination of the specified port
 * Parameters:
 *     unit              device number
 *     stage             BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port              generic port identifier
 *     mirror_dest_size  size of mirror_dest_id array
 *     mirror_dest_id    array to store mirror destination identifier
 *     mirror_dest_count number of entries stored in the mirror_dest_id array
 * Return:
 *     BCM_E_NONE
 *     BCM_E_PARAM
 *     BCM_E_UNAVAIL
 *     BCM_E_INIT
 *     BCM_E_PORT
 */
int
bcm_esw_ipfix_mirror_port_dest_get(int unit,
                                   bcm_ipfix_stage_t stage,
                                   bcm_gport_t port,
                                   int mirror_dest_size,
                                   bcm_gport_t *mirror_dest_id,
                                   int *mirror_dest_count)
{
#ifdef BCM_IPFIX_SUPPORT
    const _bcm_ipfix_id_def_t *id_def = NULL;
    soc_reg_t reg;
    uint64 rval64;
    uint32 mirror_bmp;
    int mtp_index, index;

    if (!soc_feature(unit, soc_feature_ipfix_flow_mirror)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (_bcm_ipfix_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }

    if (stage != BCM_IPFIX_STAGE_INGRESS && stage != BCM_IPFIX_STAGE_EGRESS) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_ipfix_gport_resolve(unit, port, &port));

    if (mirror_dest_id == NULL || mirror_dest_count == NULL) {
        return BCM_E_PARAM;
    }

    reg = id_def->mirror_control[stage];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    mirror_bmp = soc_reg64_field32_get(unit, reg, rval64, MIRRORf);

    for (index = 0; index < _BCM_IPFIX_RATE_MIRROR_COUNT; index++) {
        if (!(mirror_bmp & (1 << index))) {
            continue;
        }
        if (*mirror_dest_count >= mirror_dest_size) {
            break;
        }
        mtp_index = soc_reg64_field32_get(unit, reg, rval64,
                                          id_def->mtp_index[index]);
        BCM_GPORT_MIRROR_SET(mirror_dest_id[*mirror_dest_count], mtp_index);
        (*mirror_dest_count)++;
    }

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

/*
 * Function:
 *     bcm_ipfix_dump_export_entry
 * Description:
 *     To print export entry
 * Parameters:
 *     unit            Device number
 *     data            Pointer to converted data in API format
 * Return:
 *     None
 */
void
bcm_ipfix_dump_export_entry(int unit, bcm_ipfix_data_t *data)
{
#ifdef BCM_IPFIX_SUPPORT
    char *stage_str, *type_str, *reason_str;

    if (data->stage == BCM_IPFIX_STAGE_INGRESS) {
        stage_str = "ingress";
    } else if (data->stage == BCM_IPFIX_STAGE_EGRESS) {
        stage_str = "egress";
    } else {
        stage_str = "unknown stage";
    }

    if (data->flags & BCM_IPFIX_DATA_TYPE_L2) {
        type_str = "(L2)";
    } else if (data->flags & BCM_IPFIX_DATA_TYPE_IP4) {
        type_str = "(IPv4)";
    } else if (data->flags & BCM_IPFIX_DATA_TYPE_IP6) {
        type_str = "(IPv6)";
    } else {
        type_str = "(Unknown)";
    }

    if (data->flags & BCM_IPFIX_DATA_REASON_FLOW_TIMEOUT) {
        reason_str = "flow expired";
    } else if (data->flags & BCM_IPFIX_DATA_REASON_FLOW_END) {
        reason_str = "flow end";
    } else if (data->flags & BCM_IPFIX_DATA_REASON_FLOW) {
        reason_str = "early export";
    } else if (data->flags & BCM_IPFIX_DATA_REASON_COUNT) {
        reason_str = "counter overflow";
    } else if (data->flags & BCM_IPFIX_DATA_REASON_TIMESTAMP) {
        reason_str = "timestamp overflow";
    } else {
        reason_str = "unknown";
    }

    LOG_CLI((BSL_META_U(unit,
                        "\t%s %s port %d reason=%s\n"),
             type_str, stage_str, data->port, reason_str));
    LOG_CLI((BSL_META_U(unit,
                        "\tstart_timestamp=%d last_timestamp=%d"
             " byte_count=%d pkt_count=%d\n"),
             data->start_timestamp, data->last_timestamp,
             data->byte_count, data->pkt_count));
    LOG_CLI((BSL_META_U(unit,
                        "\tnon_fragment_pkt_detected=%d fragmented_pkt_detected=%d\n"),
             data->flags & BCM_IPFIX_DATA_NON_FRAGMENT_PKT_DETECTED ? 1: 0,
             data->flags & BCM_IPFIX_DATA_FRAGMENT_PKT_DETECTED ? 1: 0));
    LOG_CLI((BSL_META_U(unit,
                        "\tttl_changed_detected=%d ttl=%d tcp_flags=0x%x\n"),
             data->flags & BCM_IPFIX_DATA_TTL_CHANGED_DETECTED ? 1: 0,
             data->ttl, data->tcp_flags));
    LOG_CLI((BSL_META_U(unit,
                        "\tdrop_detected=%d dest_port=0x%x egress_vlan=%d "
                        "dest_multicast=0x%x\n"),
             data->flags & BCM_IPFIX_DATA_DROP_DETECTED ? 1: 0,
             data->dest_port, data->egress_vlan, data->dest_multicast));

    if (data->flags & BCM_IPFIX_DATA_TYPE_L2) {
        LOG_CLI((BSL_META_U(unit,
                            "\tvlan_tagged=%d vlan=%d vlan_prio=0x%x "
                            "ether_type=0x%x\n"),
                 data->flags & BCM_IPFIX_DATA_VLAN_TAGGED ? 1 : 0,
                 data->vlan, data->vlan_prio, data->ether_type));
        LOG_CLI((BSL_META_U(unit,
                            "\tsrc_mac_addr=%02x-%02x-%02x-%02x-%02x-%02x"
                            " dst_mac_addr=%02x-%02x-%02x-%02x-%02x-%02x\n"),
                 data->src_mac_addr[0], data->src_mac_addr[1],
                 data->src_mac_addr[2], data->src_mac_addr[3],
                 data->src_mac_addr[4], data->src_mac_addr[5],
                 data->dst_mac_addr[0], data->dst_mac_addr[1],
                 data->dst_mac_addr[2], data->dst_mac_addr[3],
                 data->dst_mac_addr[4], data->dst_mac_addr[5]));
    } else if (data->flags & BCM_IPFIX_DATA_TYPE_IP4) {
        LOG_CLI((BSL_META_U(unit,
                            "\tsrc_ip4_addr=%d.%d.%d.%d dst_ip4_addr=%d.%d.%d.%d\n"),
                 (data->src_ip4_addr >> 24) & 0xff,
                 (data->src_ip4_addr >> 16) & 0xff,
                 (data->src_ip4_addr >> 8) & 0xff,
                 data->src_ip4_addr & 0xff,
                 (data->dst_ip4_addr >> 24) & 0xff,
                 (data->dst_ip4_addr >> 16) & 0xff,
                 (data->dst_ip4_addr >> 8) & 0xff,
                 data->dst_ip4_addr & 0xff));
        LOG_CLI((BSL_META_U(unit,
                            "\tip_protocol=0x%x tos=0x%x\n"),
                 data->ip_protocol, data->tos));
        LOG_CLI((BSL_META_U(unit,
                            "\tl4_src_port=%d l4_dst_port=%d source_port=0x%x "
                            "vlan=%d\n"),
                 data->l4_src_port, data->l4_dst_port, data->source_port,
                 data->vlan));
    } else if (data->flags & BCM_IPFIX_DATA_TYPE_IP6) {
        LOG_CLI((BSL_META_U(unit,
                            "\tsrc_ip6_addr=%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                            "%02x%02x:%02x%02x:%02x%02x:%02x%02x\n"),
                 data->src_ip6_addr[0], data->src_ip6_addr[1],
                 data->src_ip6_addr[2], data->src_ip6_addr[3],
                 data->src_ip6_addr[4], data->src_ip6_addr[5],
                 data->src_ip6_addr[6], data->src_ip6_addr[7],
                 data->src_ip6_addr[8], data->src_ip6_addr[9],
                 data->src_ip6_addr[10], data->src_ip6_addr[11],
                 data->src_ip6_addr[12], data->src_ip6_addr[13],
                 data->src_ip6_addr[14], data->src_ip6_addr[15]));
        LOG_CLI((BSL_META_U(unit,
                            "\tdst_ip6_addr=%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                            "%02x%02x:%02x%02x:%02x%02x:%02x%02x\n"),
                 data->dst_ip6_addr[0], data->dst_ip6_addr[1],
                 data->dst_ip6_addr[2], data->dst_ip6_addr[3],
                 data->dst_ip6_addr[4], data->dst_ip6_addr[5],
                 data->dst_ip6_addr[6], data->dst_ip6_addr[7],
                 data->dst_ip6_addr[8], data->dst_ip6_addr[9],
                 data->dst_ip6_addr[10], data->dst_ip6_addr[11],
                 data->dst_ip6_addr[12], data->dst_ip6_addr[13],
                 data->dst_ip6_addr[14], data->dst_ip6_addr[15]));
        LOG_CLI((BSL_META_U(unit,
                            "\tip_protocol=0x%x tos=0x%x ip6_flow_label=0x%x\n"),
                 data->ip_protocol, data->tos, data->ip6_flow_label));
        LOG_CLI((BSL_META_U(unit,
                            "\tl4_src_port=%d l4_dst_port=%d source_port=0x%x "
                            "vlan=%d\n"),
                 data->l4_src_port, data->l4_dst_port, data->source_port,
                 data->vlan));
    }
#endif /* BCM_IPFIX_SUPPORT */
}

#ifdef BCM_IPFIX_SUPPORT
/*
 * Function:
 *     _bcm_ipfix_get_field
 * Description:
 *     A generic routine to extract a fragment of bits from source bit stream
 * Parameters:
 *     str             Source bit stream, bit 0 is the LSB at LS word offset
 *     minbit          Bit number of the first bit of the extracted data
 *     maxbit          Bit number of the last bit of the extracted data
 *     data_vp         Pointer to array for the extracted data
 * Return:
 *     None
 */
STATIC void
_bcm_ipfix_get_field(uint32 *str,
                     uint32 minbit,
                     uint32 maxbit,
                     void *data_vp)
{
    int len, str_index, data_index, right_shift_count, left_shift_count;
    uint32 *data = data_vp;

    len = maxbit - minbit + 1;
    str_index = minbit >> 5;
    data_index = 0;
    right_shift_count = minbit & 0x1f;
    left_shift_count = 32 - right_shift_count;

    if (right_shift_count) {
        for (; len > 0; len -= 32) {
            data[data_index] = str[str_index++] >> right_shift_count;
            data[data_index++] |= str[str_index] << left_shift_count;
        }
    } else {
        for (; len > 0; len -= 32) {
            data[data_index++] = str[str_index++];
        }
    }
    if (len & 0x1f) {
        data[data_index - 1] &= (1 << (len & 0x1f)) - 1;
    }
}

/*
 * Function:
 *     _bcm_ipfix_set_field
 * Description:
 *     A generic routine to implant a fragment of bits into target bit stream
 * Parameters:
 *     str             Target bit stream, bit 0 is the LSB at LS word offset
 *     minbit          Bit number of the first bit of the extracted data
 *     maxbit          Bit number of the last bit of the extracted data
 *     data_vp         Pointer to array for the source data
 * Return:
 *     None
 */
STATIC void
_bcm_ipfix_set_field(uint32 *str,
                     uint32 minbit,
                     uint32 maxbit,
                     void *data_vp)
{
    int len, str_index, data_index, right_shift_count, left_shift_count;
    uint32 *data = data_vp;
    uint32 mask;

    len = maxbit - minbit + 1;
    str_index = minbit >> 5;
    data_index = 0;
    left_shift_count = minbit & 0x1f;
    right_shift_count = 32 - left_shift_count;

    if (left_shift_count) {
        mask = 0xffffffff;
        for (; len >= 32; len -= 32) {
            str[str_index] &= ~(mask << left_shift_count);
            str[str_index++] |= data[data_index] << left_shift_count;
            str[str_index] &= ~(mask >> right_shift_count);
            str[str_index] |= data[data_index++] >> right_shift_count;
        }
        if (len) {
            mask = (1 << len) - 1;
            str[str_index] &= ~(mask << left_shift_count);
            str[str_index++] |= data[data_index] << left_shift_count;
            str[str_index] &= ~(mask >> right_shift_count);
            str[str_index] |= data[data_index] >> right_shift_count;
        }
    } else {
        for (; len >= 32; len -= 32) {
            str[str_index++] = data[data_index++];
        }
        if (len) {
            mask = (1 << len) - 1;
            str[str_index] &= ~mask;
            str[str_index] |= data[data_index];
        }
    }
}

/*
 * Function:
 *     _bcm_ipfix_process_export_entry
 * Description:
 *     To unpack export entry and convert into API format
 * Parameters:
 *     unit            Device number
 *     stage           BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     entry           Export entry from hardware
 *     data            Pointer to converted data in API format
 * Return:
 *     BCM_E_NONE
 *     BCM_E_INTERNAL
 *     BCM_E_BUSY      Type is IPV6 upper, convert when IPV6 lower is seen
 *     BCM_E_NOT_FOUND Export entry does not have valid bit set
 */
STATIC int
_bcm_ipfix_process_export_entry(int unit,
                                bcm_ipfix_stage_t stage,
                                void *entry,
                                bcm_ipfix_data_t *data)
{
    const _bcm_ipfix_id_def_t *id_def = NULL;
    soc_mem_t mem;
    uint32 type, flags;
    bcm_module_t modid;
    bcm_port_t port;
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
    uint32 intf_type, intf, tunnel;
    tunnel_entry_t tunnel_entry;
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
    static struct {
        int type;
        uint32 reason;
        uint32 session[3]; /* 95 bits */
        uint32 data[2];    /* 34 bits */
        uint32 key[11];    /* 340 bits */
    } fields[2];
    uint32 fval[12];

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    mem = id_def->export_fifo[stage];

    if (!soc_mem_field32_get(unit, mem, entry, VALIDf)) {
        fields[stage].type = -1;
        return BCM_E_NOT_FOUND;
    }

    type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);
    if (type != 3) {
        fields[stage].reason = soc_mem_field32_get(unit, mem, entry,
                                                   FIFO_REASON_CODEf);
        soc_mem_field_get(unit, mem, entry, DATA_INFOf,
                          fields[stage].data);
        soc_mem_field_get(unit, mem, entry, SESSION_INFOf,
                          fields[stage].session);
    }
    switch (type) {
    case 0: /* L2 */
        soc_mem_field_get(unit, mem, entry, L2_KEYf, fields[stage].key);
        sal_memset(data, 0, sizeof(bcm_ipfix_data_t));
        flags = BCM_IPFIX_DATA_TYPE_L2;
        _bcm_ipfix_get_field(fields[stage].key, 6, 21, fval);
        data->ether_type = fval[0];
        _bcm_ipfix_get_field(fields[stage].key, 22, 22, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_VLAN_TAGGED;
        }
        _bcm_ipfix_get_field(fields[stage].key, 23, 25, fval);
        data->vlan_prio = fval[0];
        _bcm_ipfix_get_field(fields[stage].key, 26, 37, fval);
        data->vlan = fval[0];
        _bcm_ipfix_get_field(fields[stage].key, 38, 85, fval);
        SAL_MAC_ADDR_FROM_UINT32(data->dst_mac_addr, fval);
        _bcm_ipfix_get_field(fields[stage].key, 86, 133, fval);
        SAL_MAC_ADDR_FROM_UINT32(data->src_mac_addr, fval);
        break;
    case 1: /* IPv4 */
        soc_mem_field_get(unit, mem, entry, IPV4_KEYf, fields[stage].key);
        sal_memset(data, 0, sizeof(bcm_ipfix_data_t));
        flags = BCM_IPFIX_DATA_TYPE_IP4;
        if (SOC_IS_TRIUMPH(unit)) {
            _bcm_ipfix_get_field(fields[stage].key, 64, 95,
                                 &data->dst_ip4_addr);
            _bcm_ipfix_get_field(fields[stage].key, 96, 127,
                                 &data->src_ip4_addr);
        } else if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
                   SOC_IS_VALKYRIE2(unit)) {
            _bcm_ipfix_get_field(fields[stage].key, 69, 100,
                                 &data->dst_ip4_addr);
            _bcm_ipfix_get_field(fields[stage].key, 101, 132,
                                 &data->src_ip4_addr);
        } else if (SOC_IS_TRIUMPH3(unit)) {
            _bcm_ipfix_get_field(fields[stage].key, 72, 103,
                                 &data->dst_ip4_addr);
            _bcm_ipfix_get_field(fields[stage].key, 104, 135,
                                 &data->src_ip4_addr);
        }
        break;
    case 2: /* IPv6 Upper half */
        if (SOC_IS_TRIUMPH(unit)) {
            soc_mem_field_get(unit, mem, entry, IPV6_UPPER_KEYf,
                              &fields[stage].key[7]);
        } else if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
                   SOC_IS_VALKYRIE2(unit)) {
            soc_mem_field_get(unit, mem, entry, IPV6_UPPER_KEYf, fval);
            _bcm_ipfix_set_field(fields[stage].key, 229, 344, fval);
        } else if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field_get(unit, mem, entry, IPV6_UPPER_KEYf, fval);
            _bcm_ipfix_set_field(fields[stage].key, 232, 347, fval);
        }
        fields[stage].type = type;
        return BCM_E_BUSY;
    case 3: /* IPv6 Lower half, key only */
        if (fields[stage].type != 2) {
            return BCM_E_NOT_FOUND;
        }
        sal_memset(data, 0, sizeof(bcm_ipfix_data_t));
        flags = BCM_IPFIX_DATA_TYPE_IP6;
        if (SOC_IS_TRIUMPH(unit)) {
            soc_mem_field_get(unit, mem, entry, IPV6_LOWER_KEYf,
                              fields[stage].key);
        } else if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
                   SOC_IS_VALKYRIE2(unit)) {
            soc_mem_field_get(unit, mem, entry, IPV6_LOWER_KEYf, fval);
            _bcm_ipfix_set_field(fields[stage].key, 0, 228, fval);
        } else if (SOC_IS_TRIUMPH3(unit)) {
            soc_mem_field_get(unit, mem, entry, IPV6_LOWER_KEYf, fval);
            _bcm_ipfix_set_field(fields[stage].key, 0, 231, fval);
        }
        if (SOC_IS_TRIUMPH(unit)) {
            _bcm_ipfix_get_field(fields[stage].key, 64, 83,
                                 &data->ip6_flow_label);
            _bcm_ipfix_get_field(fields[stage].key, 84, 211, fval);
            SAL_IP6_ADDR_FROM_UINT32(data->dst_ip6_addr, fval);
            _bcm_ipfix_get_field(fields[stage].key, 212, 339, fval);
            SAL_IP6_ADDR_FROM_UINT32(data->src_ip6_addr, fval);
        } else if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
                   SOC_IS_VALKYRIE2(unit)) {
            _bcm_ipfix_get_field(fields[stage].key, 69, 88,
                                 &data->ip6_flow_label);
            _bcm_ipfix_get_field(fields[stage].key, 89, 216, fval);
            SAL_IP6_ADDR_FROM_UINT32(data->dst_ip6_addr, fval);
            _bcm_ipfix_get_field(fields[stage].key, 217, 344, fval);
            SAL_IP6_ADDR_FROM_UINT32(data->src_ip6_addr, fval);
         } else if (SOC_IS_TRIUMPH3(unit)) {
            _bcm_ipfix_get_field(fields[stage].key, 72, 91,
                                 &data->ip6_flow_label);
            _bcm_ipfix_get_field(fields[stage].key, 92, 219, fval);
            SAL_IP6_ADDR_FROM_UINT32(data->dst_ip6_addr, fval);
            _bcm_ipfix_get_field(fields[stage].key, 220, 347, fval);
            SAL_IP6_ADDR_FROM_UINT32(data->src_ip6_addr, fval);
         }
        break;
    default:
        return BCM_E_INTERNAL;
    }
    fields[stage].type = type;
    _bcm_ipfix_get_field(fields[stage].key, 0, 5, fval);
    data->port = fval[0];
    if (fields[stage].type) { /* Common IP key fields */
        _bcm_ipfix_get_field(fields[stage].key, 6, 13, fval);
        data->ip_protocol = fval[0];
        _bcm_ipfix_get_field(fields[stage].key, 14, 21, fval);
        data->tos = fval[0];
        _bcm_ipfix_get_field(fields[stage].key, 22, 37, fval);
        data->l4_src_port = fval[0];
        _bcm_ipfix_get_field(fields[stage].key, 38, 53, fval);
        data->l4_dst_port = fval[0];
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
            SOC_IS_VALKYRIE2(unit)) {
            _bcm_ipfix_get_field(fields[stage].key, 54, 55, &intf_type);
            _bcm_ipfix_get_field(fields[stage].key, 56, 68, &intf);
            switch (intf_type) {
            case 0: /* VLAN */
                data->vlan = intf;
                break;
            case 1: /* L3_IIF */
                if (intf < 4096) { /* vlan id */
                    data->vlan = intf;
                } else if (intf < 4096 + 512) { /* L3_TUNNEL index */
                    if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_IDf)) {
                        BCM_IF_ERROR_RETURN
                            (soc_mem_read(unit, L3_TUNNELm, MEM_BLOCK_ANY,
                                          intf - 4096, &tunnel_entry));
                        tunnel = soc_mem_field32_get
                            (unit, L3_TUNNELm, &tunnel_entry, TUNNEL_IDf);
                        if (tunnel) {
                            BCM_GPORT_TUNNEL_ID_SET(data->source_port, tunnel);
                        }
                    }
                } else { /* MPLS */
                    /* vrf = intf - 4096 - 512 */
                }
                break;
            case 2: /* SVP */
                if (_bcm_vp_used_get(unit, intf, _bcmVpTypeMpls)) {
                    BCM_GPORT_MPLS_PORT_ID_SET(data->source_port, intf);
                } else if (_bcm_vp_used_get(unit, intf, _bcmVpTypeMim)) {
                    BCM_GPORT_MIM_PORT_ID_SET(data->source_port, intf);
                } else if (_bcm_vp_used_get(unit, intf, _bcmVpTypeSubport)) {
                    BCM_GPORT_SUBPORT_GROUP_SET(data->source_port, intf);
                } else if (_bcm_vp_used_get(unit, intf, _bcmVpTypeWlan)) {
                    BCM_GPORT_WLAN_PORT_ID_SET(data->source_port, intf);
                }
            }
        }
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
    }

    data->stage = stage;
#ifdef BCM_TRIUMPH3_SUPPORT    
    if (SOC_IS_TRIUMPH3(unit)) {
        _bcm_ipfix_get_field(fields[stage].data, 35, 35, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_NON_FRAGMENT_PKT_DETECTED;
        }
        _bcm_ipfix_get_field(fields[stage].data, 34, 34, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_FRAGMENT_PKT_DETECTED;
        }
        _bcm_ipfix_get_field(fields[stage].data, 33, 33, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_TTL_CHANGED_DETECTED;
        }
        _bcm_ipfix_get_field(fields[stage].data, 18, 18, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_DROP_DETECTED;
        }
        _bcm_ipfix_get_field(fields[stage].data, 25, 32, &data->ttl);
        _bcm_ipfix_get_field(fields[stage].data, 19, 24, &data->tcp_flags);
    } else 
#endif /* BCM_TRIUMPH3_SUPPORT */    
    {
        _bcm_ipfix_get_field(fields[stage].data, 33, 33, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_NON_FRAGMENT_PKT_DETECTED;
        }
        _bcm_ipfix_get_field(fields[stage].data, 32, 32, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_FRAGMENT_PKT_DETECTED;
        }
        _bcm_ipfix_get_field(fields[stage].data, 31, 31, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_TTL_CHANGED_DETECTED;
        }
        _bcm_ipfix_get_field(fields[stage].data, 16, 16, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_DROP_DETECTED;
        }
        _bcm_ipfix_get_field(fields[stage].data, 23, 30, &data->ttl);
        _bcm_ipfix_get_field(fields[stage].data, 17, 22, &data->tcp_flags);
    }
    _bcm_ipfix_get_field(fields[stage].data, 0, 2, fval);
    switch (fval[0]) {
    case 0: /* unicast - modid / port */
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            /* DESTINATIONf: bits 3-9: PORTID, bits 10-17: MODULE */
            _bcm_ipfix_get_field(fields[stage].data, 3, 9, &port);
            _bcm_ipfix_get_field(fields[stage].data, 10, 17, &modid);
        } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            /* DESTINATIONf: bits 3-8: PORTID, bits 9-15: MODULE */
            _bcm_ipfix_get_field(fields[stage].data, 3, 8, &port);
            _bcm_ipfix_get_field(fields[stage].data, 9, 15, &modid);
        }
        BCM_GPORT_MODPORT_SET(data->dest_port, modid, port);
        break;
    case 1: /* unicast - trunk */
        SOC_GPORT_TRUNK_SET(data->dest_port, fval[0]);
        break;
    case 2: /* L2 multicast ID */
        _bcm_ipfix_get_field(fields[stage].data, 3, 15, fval);
        _BCM_MULTICAST_GROUP_SET
            (data->dest_multicast, _BCM_MULTICAST_TYPE_L2, fval[0]);
        break;
    case 3: /* L3 multicast ID */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            _bcm_ipfix_get_field(fields[stage].data, 3, 17, fval);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            _bcm_ipfix_get_field(fields[stage].data, 3, 15, fval);
        }
        if (_bcm_tr_multicast_ipmc_group_type_get
            (unit, fval[0], &data->dest_multicast) == BCM_E_NOT_FOUND) {
            _BCM_MULTICAST_GROUP_SET
                (data->dest_multicast, _BCM_MULTICAST_TYPE_L3, fval[0]);
        }
#endif /* BCM_TRX_SUPPORT && INCLUDE_L3 */
        break;
    case 4: /* L2 broadcast (VLAN ID) */
    case 5: /* L2 DLF at time of first packet (VLAN ID) */
        _bcm_ipfix_get_field(fields[stage].data, 3, 15, fval);
        data->egress_vlan = fval[0];
        break;
    default:
        break;
    }

    _bcm_ipfix_get_field(fields[stage].session, 0, 15, &data->start_timestamp);
    _bcm_ipfix_get_field(fields[stage].session, 16, 31, &data->last_timestamp);
    _bcm_ipfix_get_field(fields[stage].session, 32, 63, &data->byte_count);
    _bcm_ipfix_get_field(fields[stage].session, 64, 89, &data->pkt_count);
    if (soc_feature(unit, soc_feature_ipfix_rate)) {
        _bcm_ipfix_get_field(fields[stage].session, 90, 90, fval);
        if (fval[0]) {
            flags |= BCM_IPFIX_DATA_RATE_VIOLATION;
        }
    }

    switch(fields[stage].reason) {
    case 1:
        flags |= BCM_IPFIX_DATA_REASON_FLOW_END;
        break;
    case 2:
        flags |= BCM_IPFIX_DATA_REASON_FLOW_TIMEOUT;
        break;
    case 3:
        flags |= BCM_IPFIX_DATA_REASON_FLOW;
        break;
    case 4:
        flags |= BCM_IPFIX_DATA_REASON_COUNT;
        break;
    case 5:
        flags |= BCM_IPFIX_DATA_REASON_TIMESTAMP;
        break;
    default:
        break;
    }

    data->flags = flags;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_ipfix_fifo_dma_thread
 * Description:
 *     A task thread to process IPFIX export FIFOs via FIFO DMA
 * Parameters:
 *     unit_vp          Device number
 * Return:
 *     None
 */
STATIC void
_bcm_ipfix_fifo_dma_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    _bcm_ipfix_ctrl_t *ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    _bcm_ipfix_callback_entry_t *cb_entry;
    const _bcm_ipfix_id_def_t *id_def = NULL;
    bcm_ipfix_data_t data;
    int rv, entries_per_buf, adv_threshold, interval, count, i, j, non_empty;
    int stage, chan, entry_words[2];
    void *host_buf[2], *export_entry[2];
#ifdef BCM_CMICM_SUPPORT
    uint32 rval = 0;
    uint8 overflow_ch2 = 0, overflow_ch3 = 0;
    uint8 timeout_ch2 = 0, timeout_ch3 = 0;
    int cmc = SOC_PCI_CMC(unit);
#endif /* BCM_CMICM_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (id_def == NULL) {
        ipfix_ctrl->pid = SAL_THREAD_ERROR;
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_IPFIX, __LINE__, 
                           BCM_E_INTERNAL);
        sal_thread_exit(0);
        return;
    }

    entries_per_buf = soc_property_get(unit, spn_IPFIX_HOSTBUF_SIZE, 1024);
    adv_threshold = entries_per_buf / 2;

    entry_words[0] = soc_mem_entry_words(unit, id_def->export_fifo[0]);
    host_buf[0] =
        soc_cm_salloc(unit, entries_per_buf * entry_words[0] * sizeof(uint32),
                      "Ingress IPFIX DMA Buffer");
    if (host_buf[0] == NULL) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_IPFIX, __LINE__, 
                           BCM_E_MEMORY);
        goto cleanup_exit;
    }

    entry_words[1] = soc_mem_entry_words(unit, id_def->export_fifo[1]);
    host_buf[1] =
        soc_cm_salloc(unit, entries_per_buf * entry_words[1] * sizeof(uint32),
                      "Egress IPFIX DMA Buffer");
    if (host_buf[1] == NULL) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_IPFIX, __LINE__, 
                           BCM_E_MEMORY);
        goto cleanup_exit;
    }

    for (stage = 0; stage < 2; stage++) {
        rv = soc_mem_fifo_dma_start(unit, ipfix_ctrl->dma_chan[stage],
                                   id_def->export_fifo[stage],
                                   MEM_BLOCK_ANY, entries_per_buf,
                                   host_buf[stage]); 
        if (BCM_FAILURE(rv)) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                               SOC_SWITCH_EVENT_THREAD_IPFIX, __LINE__, rv);
            goto cleanup_exit;
        }
#ifdef BCM_CMICM_SUPPORT
        export_entry[stage] = host_buf[stage]; 
#endif
    }

    while ((interval = ipfix_ctrl->interval)) {
        if (SOC_CONTROL(unit)->ipfixIntrEnb) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit)) {
                (void)soc_cmicm_intr0_enable
                            (unit, 
                             IRQ_CMCx_FIFO_CH_DMA(SOC_MEM_FIFO_DMA_CHANNEL_2) | 
                             IRQ_CMCx_FIFO_CH_DMA(SOC_MEM_FIFO_DMA_CHANNEL_3));
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                (void)soc_intr_enable(unit, (IRQ_FIFO_CH0_DMA | IRQ_FIFO_CH3_DMA));
            }

#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm)) {
                 /* check for timeout & overflow */
                rval = soc_pci_read(unit, 
                          CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, 
                                          SOC_MEM_FIFO_DMA_CHANNEL_2));          
                overflow_ch2 = soc_reg_field_get(unit, 
                                 CMIC_CMC0_FIFO_CH2_RD_DMA_STATr, rval,
                                 HOSTMEM_OVERFLOWf);
                timeout_ch2 = soc_reg_field_get(unit, 
                                CMIC_CMC0_FIFO_CH2_RD_DMA_STATr, rval,
                                HOSTMEM_TIMEOUTf); 
                overflow_ch2 |= timeout_ch2;

                rval = soc_pci_read(unit, 
                          CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, 
                                          SOC_MEM_FIFO_DMA_CHANNEL_3));          
                overflow_ch3 = soc_reg_field_get(unit, 
                                 CMIC_CMC0_FIFO_CH3_RD_DMA_STATr, rval,
                                 HOSTMEM_OVERFLOWf);
                timeout_ch3 = soc_reg_field_get(unit, 
                                CMIC_CMC0_FIFO_CH3_RD_DMA_STATr, rval,
                                HOSTMEM_TIMEOUTf); 
                overflow_ch3 |= timeout_ch3;
            }
#endif /* BCM_CMICM_SUPPORT */
        }

        if (sal_sem_take(SOC_CONTROL(unit)->ipfixIntr, interval) < 0) {
            LOG_VERBOSE(BSL_LS_BCM_DMA,
                        (BSL_META_U(unit,
                                    "IPFIX polling timeout\n")));
        } else {
            LOG_VERBOSE(BSL_LS_BCM_DMA,
                        (BSL_META_U(unit,
                                    "IPFIX woken up\n")));
        }

        for (i = 0; i < _BCM_IPFIX_CB_MAX; i++) {
            cb_entry = &ipfix_ctrl->callback_entry[i];
            switch (cb_entry->state) {
            case _BCM_IPFIX_CALLBACK_STATE_REGISTERED:
                cb_entry->state = _BCM_IPFIX_CALLBACK_STATE_ACTIVE;
                break;
            case _BCM_IPFIX_CALLBACK_STATE_UNREGISTERED:
                cb_entry->state = _BCM_IPFIX_CALLBACK_STATE_INVALID;
                break;
            default:
                break;
            }
        }

        do {
            non_empty = FALSE;
            for (stage = 0; stage < 2; stage++) {
                chan = ipfix_ctrl->dma_chan[stage];
#ifdef BCM_CMICM_SUPPORT
                if (soc_feature(unit, soc_feature_cmicm)) {
                    rv = soc_mem_fifo_dma_get_num_entries(unit, chan, &count);
                } else
#endif
                { 
                    rv = soc_mem_fifo_dma_get_read_ptr(unit, chan, &export_entry[0],
                                                       &count);
                }
                if (SOC_SUCCESS(rv)) {
                    non_empty = TRUE;
                    if (count > adv_threshold) {
                        count = adv_threshold;
                    }
                    for (i = 0; i < count; i++) {
#ifdef BCM_CMICM_SUPPORT
                        rv = _bcm_ipfix_process_export_entry(unit, stage,
                                                             export_entry[stage],
                                                             &data);
                        export_entry[stage] =
                            (uint32 *)export_entry[stage] + entry_words[stage];
                        /*From the begining of host buffer*/	 
                        if (((uint32 *)export_entry[stage] - (uint32 *)host_buf[stage])/entry_words[stage] 
	                        == entries_per_buf) {
	                        export_entry[stage] = host_buf[stage];
	                    }
#else
                        rv = _bcm_ipfix_process_export_entry(unit, stage,
                                                             export_entry[0],
                                                             &data);
                        export_entry[0] =
                            (uint32 *)export_entry[0] + entry_words[stage];
#endif        
                        if (rv < 0) {
                            continue;
                        }
						
						
                        if (bsl_check(bslLayerBcm, bslSourceDma, bslSeverityVerbose, unit)) {
                            bcm_ipfix_dump_export_entry(unit, &data);
                        }
                        for (j = 0; j < _BCM_IPFIX_CB_MAX; j++) {
                            cb_entry = &ipfix_ctrl->callback_entry[j];
                            if (cb_entry->state ==
                                _BCM_IPFIX_CALLBACK_STATE_ACTIVE) {
                                cb_entry->callback(unit, &data,
                                                   cb_entry->userdata);
                            }
                        }
                    }
#ifdef BCM_CMICM_SUPPORT
                    if (soc_feature(unit, soc_feature_cmicm)) {
                        (void)_soc_mem_sbus_fifo_dma_set_entries_read(unit, chan, count);
                    } else
#endif
                    { 
                        (void)soc_mem_fifo_dma_advance_read_ptr(unit, chan, count);
                    }
                }
            }
        } while (non_empty);
        /* clearing of the FIFO_CH_DMA_INT interrupt by resetring 
           overflow & timeout status in FIFO_CHx_RD_DMA_STAT_CLR reg */
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm)) {
                if (overflow_ch2) {
                    rval = 0;
                    soc_reg_field_set(unit, 
                                 CMIC_CMC0_FIFO_CH2_RD_DMA_STAT_CLRr, &rval,
                                 HOSTMEM_OVERFLOWf, 1);
                    soc_reg_field_set(unit, 
                                 CMIC_CMC0_FIFO_CH2_RD_DMA_STAT_CLRr, &rval,
                                 HOSTMEM_TIMEOUTf, 1);
                    soc_pci_write(unit, 
                              CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, 
                              SOC_MEM_FIFO_DMA_CHANNEL_2), rval);
                }
                if (overflow_ch3) {
                    rval = 0;
                    soc_reg_field_set(unit, 
                                 CMIC_CMC0_FIFO_CH3_RD_DMA_STAT_CLRr, &rval,
                                 HOSTMEM_OVERFLOWf, 1);
                    soc_reg_field_set(unit, 
                                 CMIC_CMC0_FIFO_CH3_RD_DMA_STAT_CLRr, &rval,
                                 HOSTMEM_TIMEOUTf, 1);
                    soc_pci_write(unit, 
                              CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, 
                              SOC_MEM_FIFO_DMA_CHANNEL_3), rval);
                }
            }
#endif /* BCM_CMICM_SUPPORT */
    }

cleanup_exit:
    (void)soc_mem_fifo_dma_stop(unit, ipfix_ctrl->dma_chan[0]);
    (void)soc_mem_fifo_dma_stop(unit, ipfix_ctrl->dma_chan[1]);

    if (host_buf[0] != NULL) {
        soc_cm_sfree(unit, host_buf[0]);
    }
    if (host_buf[1] != NULL) {
        soc_cm_sfree(unit, host_buf[1]);
    }
    ipfix_ctrl->pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}
#endif /* BCM_IPFIX_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_esw_ipfix_sync
 * Purpose:
 *      Record ipfix module persisitent info for Level 2 Warm Boot
 * Parameters: 
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */ 
int     
_bcm_esw_ipfix_sync(int unit) 
{
#ifdef BCM_IPFIX_SUPPORT
    _bcm_ipfix_ctrl_t *ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    soc_scache_handle_t scache_handle;
    uint8 *ipfix_scache = NULL;  
    int size;
    int arry_size;

    if (soc_feature(unit, soc_feature_ipfix_rate)) {
        /*
         * Return error if IPFIX is not initialized.
         */
        if (_bcm_ipfix_ctrl[unit] == NULL) {
            return BCM_E_INIT;
        }

        sal_memset(&scache_handle, 0, sizeof(soc_scache_handle_t));
        /*
         * Get ipfix base address handle and start writing 
         * Level-2 recovery data to memory.
         */
        size = soc_mem_index_count(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm);
        arry_size = _SHR_BITDCLSIZE(size);
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IPFIX, 0);
        BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 
                                                    FALSE, 0,
                                                    &ipfix_scache, 
                                                    BCM_WB_DEFAULT_VERSION, 
                                                    NULL));
        sal_memcpy(ipfix_scache, &ipfix_ctrl->rate_used_count, 
                    sizeof(ipfix_ctrl->rate_used_count)); 
        ipfix_scache += sizeof(ipfix_ctrl->rate_used_count);
        sal_memcpy(ipfix_scache, ipfix_ctrl->rate_used_bmp, arry_size);
    }
#endif /* !BCM_IPFIX_SUPPORT */
    return BCM_E_NONE;
}
#endif /* !BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *     bcm_ipfix_export_fifo_control
 * Description:
 *     To start/stop ipfix export fifo processing thread
 * Parameters:
 *     unit            device number
 *     sal_usecs_t     polling interval, set to 0 to stop the thread
 * Return:
 *     BCM_E_NONE
 *     BCM_E_MEMORY
 *     BCM_E_UNAVAIL
 */
int
bcm_ipfix_export_fifo_control(int unit, sal_usecs_t interval)
{
#ifdef BCM_IPFIX_SUPPORT
    _bcm_ipfix_ctrl_t *ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    const _bcm_ipfix_profile_def_t *profile_def = NULL;
    const _bcm_ipfix_id_def_t *id_def = NULL;
    int pri;
    char name[16];

    if (!soc_feature(unit, soc_feature_ipfix) ||
        !soc_feature(unit, soc_feature_fifo_dma)) {
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TR_VL(unit)) {
        profile_def = _ipfix_tr_profile_def;
        id_def = &_ipfix_tr_id_def;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    if (profile_def == NULL || id_def == NULL) {
        return BCM_E_INTERNAL;
    }

    if (ipfix_ctrl == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_ipfix_init(unit, profile_def, id_def));
        ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    }

    sal_snprintf(name, sizeof(name), "bcmIpfixDma.%d", unit);

    ipfix_ctrl->interval = interval;
    if (interval) {
        if (ipfix_ctrl->pid == SAL_THREAD_ERROR) {
            pri = soc_property_get(unit, spn_IPFIX_THREAD_PRI, 100);
            ipfix_ctrl->pid = sal_thread_create(name, SAL_THREAD_STKSZ, pri,
                                                _bcm_ipfix_fifo_dma_thread,
                                                INT_TO_PTR(unit));
            if (ipfix_ctrl->pid == SAL_THREAD_ERROR) {
                LOG_ERROR(BSL_LS_BCM_IPFIX,
                          (BSL_META_U(unit,
                                      "Could not start thread\n")));
                return BCM_E_MEMORY;
            }
        }
    } else {
        /* Wake up thread so it will check the changed interval value */
        sal_sem_give(SOC_CONTROL(unit)->ipfixIntr);
    }

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

int _bcm_esw_ipfix_deinit(int unit)
{
#ifdef BCM_IPFIX_SUPPORT
    soc_timeout_t timeout;
    _bcm_ipfix_ctrl_t *ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    int i;
#ifdef BCM_TRIUMPH3_SUPPORT
    uint32 rval;
#endif

    if (NULL == ipfix_ctrl) {
        return (BCM_E_NONE);
    }

    bcm_ipfix_export_fifo_control(unit, 0);

    soc_timeout_init(&timeout, SAL_BOOT_BCMSIM ? 30000000 : 10000000, 0);

    while (ipfix_ctrl->pid != SAL_THREAD_ERROR)
    {
        if (soc_timeout_check(&timeout) != 0)
        {
            LOG_ERROR(BSL_LS_BCM_IPFIX,
                      (BSL_META_U(unit,
                                  "IPFIX thread did not exit.\n")));

            return BCM_E_INTERNAL;
        }
    }

    for (i = 0; i < _BCM_IPFIX_CB_MAX; i++) {
        ipfix_ctrl->callback_entry[i].state =
            _BCM_IPFIX_CALLBACK_STATE_INVALID;
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN(READ_AUX_ARB_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, AUX_ARB_CONTROL_2r, &rval,
                IPFIX_REFRESH_ENABLEf, 0);
        BCM_IF_ERROR_RETURN(WRITE_AUX_ARB_CONTROL_2r(unit, rval));
    }
#endif

    return BCM_E_NONE;
#else /* BCM_IPFIX_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_IPFIX_SUPPORT */
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_ipfix_sw_dump
 * Description:
 *     Displays IPFIX information maintained in software.
 * Parameters:
 *     unit - Device unit number.
 * Return:
 *     None
 */
void _bcm_ipfix_sw_dump(int unit)
{
#ifdef BCM_IPFIX_SUPPORT
    _bcm_ipfix_ctrl_t *ipfix_ctrl = _bcm_ipfix_ctrl[unit];
    int size;
    int rate_id;
    int rate_id_exists = 0;

    if (NULL == ipfix_ctrl) {
        LOG_ERROR(BSL_LS_BCM_IPFIX,
                  (BSL_META_U(unit,
                              "IPFIX module not initialized.\n")));
        return;
    }
    size = soc_mem_index_count(unit, ING_IPFIX_FLOW_RATE_METER_TABLEm);
    LOG_CLI((BSL_META_U(unit,
                        "\n SW Information IPFIX - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "\n\tIPFIX Flow Rate Meter ID Info\n")));
    LOG_CLI((BSL_META_U(unit,
                        "\t-----------------------------\n")));
    for (rate_id = 1; rate_id < size; rate_id++)  {
        if (SHR_BITGET(_bcm_ipfix_ctrl[unit]->rate_used_bmp, rate_id)) {
            LOG_CLI((BSL_META_U(unit,
                                "\t\tMeterId:%d - exists\n"), rate_id));
            rate_id_exists = 1;
        }
    }
    if (rate_id_exists == 0) {
            LOG_CLI((BSL_META_U(unit,
                                "\t\tNone.\n")));
    }
    if (ipfix_ctrl->rate_used_count == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "\n\tFlow Rate Meter config - Disabled\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "\n\tFlow Rate Meter config - Enabled\n")));
    }
#endif /* !BCM_IPFIX_SUPPORT */
    return;
}
#endif /* !BCM_WARM_BOOT_SUPPORT_SW_DUMP */
