/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:
 * Manage common functionality for TSN Statistics counter implementation.
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/tsn.h>
#include <bcm_int/esw/tsn.h>
#include <bcm_int/esw/tsn_stat.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#include <bcm_int/esw/switch.h>

#if defined(BCM_TSN_SUPPORT)
/*
 * TSN stat type feature mapping
 */
STATIC const soc_feature_t tsn_stat_type_feature[bcmTsnStatCount] =
{
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrTaggedPackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressNonLinkLocalPackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrWrongLanPackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrRxErrorPackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrTagErrorPackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrDuplicatePackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrOutOfOrderSrPackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrOwnRxPackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrUnexpectedPackets */
    soc_feature_tsn_mtu_stu, /* bcmTsnStatIngressMtuErrorPackets */
    soc_feature_tsn_mtu_stu, /* bcmTsnStatIngressStuErrorPackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrAcceptedSrPackets */
    soc_feature_tsn_sr, /* bcmTsnStatIngressSrSaSrcFilteredPackets */
    soc_feature_tsn_taf, /* bcmTsnStatIngressTafMeterDrop */
    soc_feature_tsn_taf, /* bcmTsnStatIngressTafGatePass */
    soc_feature_tsn_taf, /* bcmTsnStatIngressTafGateCloseDrop */
    soc_feature_tsn_taf, /* bcmTsnStatIngressTafGateNoByteDrop */
    soc_feature_tsn_taf, /* bcmTsnStatIngressTafMtuPass */
    soc_feature_tsn_sr, /* bcmTsnStatEgressSrTaggedPackets */
    soc_feature_tsn_sr, /* bcmTsnStatEgressNonLinkLocalPackets */
    soc_feature_tsn_mtu_stu, /* bcmTsnStatEgressMtuErrorPackets */
    soc_feature_tsn_mtu_stu /* bcmTsnStatEgressStuErrorPackets */
};

/*
 * TSN stat portcnt member list data structure
 */
typedef struct bcmi_tsn_stat_portcnt_member_s {
    soc_mem_t mem;
    soc_field_t field;
    uint64 *port_counter_hw;
    uint64 *port_counter_sw;
} bcmi_tsn_stat_portcnt_member_t;

/*
 * TSN stat portcnt table entry data structure for temporary access
 */
typedef struct bcmi_tsn_stat_portcnt_table_entry_s {
    uint32 entry_data[SOC_MAX_MEM_WORDS];
} bcmi_tsn_stat_portcnt_table_entry_t;

/*
 * Software book keeping for TSN stat portcnt related information:
 * - enabled matrix for extra counter callback(thread)
 */
typedef struct bcmi_tsn_stat_portcnt_bk_info_s {
    SHR_BITDCL *stat_bitmap;      /* TSN Portcnt Stat bitmap [stats] */
    SHR_BITDCL *stat_port_bitmap; /* TSN Portcnt Port bitmap [stats * ports] */
    int num_port;
} bcmi_tsn_stat_portcnt_bk_info_t;

/*
 * TSN stat portcnt control data structure
 */
typedef struct bcmi_tsn_stat_portcnt_control_s {
    sal_mutex_t portcnt_mutex;
    bcmi_tsn_stat_portcnt_member_t *stats;
    bcmi_tsn_stat_portcnt_table_entry_t *portcnt_entry_temp;
    bcmi_tsn_stat_portcnt_bk_info_t *portcnt_bk_info;
} bcmi_tsn_stat_portcnt_control_t;

/*
 * TSN stat flowcnt control data structure
 */
typedef struct bcmi_tsn_stat_flowcnt_control_s {
    sal_mutex_t flowcnt_mutex;
} bcmi_tsn_stat_flowcnt_control_t;

/*
 * TSN stat control data structure
 */
typedef struct bcmi_tsn_stat_control_s {
    /* TSN stat related control structures defined below */
    bcmi_tsn_stat_portcnt_control_t *portcnt_control;
    bcmi_tsn_stat_flowcnt_control_t *flowcnt_control;
    /* TSN stat device info for chip specific */
    const bcmi_esw_tsn_stat_dev_info_t *tsn_stat_dev_info;
} bcmi_tsn_stat_control_t;

/* TSN stat control */
STATIC bcmi_tsn_stat_control_t *tsn_stat_control[BCM_MAX_NUM_UNITS];

/* Global initialized parameter for TSN stat */
STATIC uint8 tsn_stat_initialized = 0;

static soc_ser_tsn_stat_functions_t ser_tsn_stat_functions;

/*
 * Macro:
 *    TSN_STAT_PORTCNT_IS_INIT (internal)
 * Purpose:
 *    Check that the unit is valid and confirm that
 *    the TSN Port Stat functions are initialized.
 * Parameters:
 *    unit - BCM device number
 * Notes:
 *    Results in return (BCM_E_UNIT), return (BCM_E_UNAVAIL), or
 *    return (BCM_E_INIT) if fails.
 */
#define TSN_STAT_PORTCNT_IS_INIT(_unit_)                         \
    do {                                                         \
        if (!soc_feature(_unit_, soc_feature_tsn)) {             \
            return BCM_E_UNAVAIL;                                \
        }                                                        \
        if (NULL == tsn_stat_control[_unit_]) {                  \
            LOG_ERROR(BSL_LS_BCM_TSN,                            \
                      (BSL_META_U(_unit_,                        \
                                  "TSN stat control Error: "     \
                                  "not initialized\n")));        \
            return BCM_E_INIT;                                   \
        }                                                        \
        if (NULL == tsn_stat_control[_unit_]->portcnt_control) { \
            LOG_ERROR(BSL_LS_BCM_TSN,                            \
                      (BSL_META_U(_unit_,                        \
                                  "TSN portcnt control Error: "  \
                                  "not initialized\n")));        \
            return BCM_E_INIT;                                   \
        }                                                        \
    } while (0)

/*
 * Macro:
 *    TSN_STAT_PORTCNT_IS_INIT (internal)
 * Purpose:
 *    Check that the unit is valid and confirm that
 *    the TSN Port Stat functions are initialized.
 * Parameters:
 *    unit - BCM device number
 * Notes:
 *    Results in return (BCM_E_UNIT), return (BCM_E_UNAVAIL), or
 *    return (BCM_E_INIT) if fails.
 */
#define TSN_STAT_FLOWCNT_IS_INIT(_unit_)                         \
    do {                                                         \
        if (!soc_feature(_unit_, soc_feature_tsn)) {             \
            return BCM_E_UNAVAIL;                                \
        }                                                        \
        if (NULL == tsn_stat_control[_unit_]) {                  \
            LOG_ERROR(BSL_LS_BCM_TSN,                            \
                      (BSL_META_U(_unit_,                        \
                                  "TSN stat control Error: "     \
                                  "not initialized\n")));        \
            return BCM_E_INIT;                                   \
        }                                                        \
        if (NULL == tsn_stat_control[_unit_]->flowcnt_control) { \
            LOG_ERROR(BSL_LS_BCM_TSN,                            \
                      (BSL_META_U(_unit_,                        \
                                  "TSN flowcnt control Error: "  \
                                  "not initialized\n")));        \
            return BCM_E_INIT;                                   \
        }                                                        \
    } while (0)


/* TSN stat mutex lock and unlock */
#define TSN_STAT_PORTCNT_LOCK(pc) \
        sal_mutex_take((pc)->portcnt_mutex, sal_mutex_FOREVER)
#define TSN_STAT_PORTCNT_UNLOCK(pc) \
        sal_mutex_give((pc)->portcnt_mutex)
#define TSN_STAT_FLOWCNT_LOCK(fc) \
            sal_mutex_take((fc)->flowcnt_mutex, sal_mutex_FOREVER)
#define TSN_STAT_FLOWCNT_UNLOCK(fc) \
            sal_mutex_give((fc)->flowcnt_mutex)

/*
 * Enabled matrix for TSN Portcnt Stat usage bitmap operations
 * - if any port is enabled
 */
#define TSN_STAT_PORTCNT_USED_GET(portcnt_bk_info, stat) \
    (((portcnt_bk_info)->stat_bitmap) ? \
     SHR_BITGET((portcnt_bk_info)->stat_bitmap, (stat)) : 0)
#define TSN_STAT_PORTCNT_USED_SET(portcnt_bk_info, stat) \
    SHR_BITSET((portcnt_bk_info)->stat_bitmap, (stat))
#define TSN_STAT_PORTCNT_USED_CLR(portcnt_bk_info, stat) \
    SHR_BITCLR((portcnt_bk_info)->stat_bitmap, (stat))

/*
 * Enabled matrix for TSN Portcnt Port usage bitmap operations
 * with specified TSN stat
 */
#define TSN_STAT_PORTCNT_PORT_USED_GET(portcnt_bk_info, stat, port) \
    (((portcnt_bk_info)->stat_port_bitmap) ? \
     SHR_BITGET((portcnt_bk_info)->stat_port_bitmap, \
     ((stat * (portcnt_bk_info)->num_port) + port)) : 0)
#define TSN_STAT_PORTCNT_PORT_USED_SET(portcnt_bk_info, stat, port) \
    SHR_BITSET((portcnt_bk_info)->stat_port_bitmap, \
               ((stat * (portcnt_bk_info)->num_port) + port))
#define TSN_STAT_PORTCNT_PORT_USED_CLR(portcnt_bk_info, stat, port) \
    SHR_BITCLR((portcnt_bk_info)->stat_port_bitmap, \
               ((stat * (portcnt_bk_info)->num_port) + port))

/* TSN stat type validation */
#define TSN_STAT_IS_VALID(unit, type)                  \
        ((type >= bcmTsnStatIngressSrTaggedPackets) && \
         (type < bcmTsnStatCount))

/* helper macro to execute dispatched specific device function */
#define TSN_STAT_FLOWCNT_FUNC(_name_, _args_)                                \
        ((NULL == tsn_stat_control[unit]->tsn_stat_dev_info->                \
         tsn_stat_flowcnt_dev_info->tsn_stat_flowcnt_##_name_) ? BCM_E_INIT :\
         (tsn_stat_control[unit]->tsn_stat_dev_info->                        \
          tsn_stat_flowcnt_dev_info->tsn_stat_flowcnt_##_name_)_args_)


/*
 * Function:
 *    tsn_stat_control_instance_get
 * Description:
 *    Helper function to retrieve TSN stat control pointer
 * Parameters:
 *    unit  - (IN) BCM device number
 *    tc   - (OUT) TSN stat control data structure
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
tsn_stat_control_instance_get(
    int unit,
    bcmi_tsn_stat_control_t **tc)
{
    /* Input parameter check. */
    if (NULL == tc) {
        return BCM_E_PARAM;
    }

    /* Check if TSN feature supported and initialized for TSN stat */
    if (!soc_feature(unit, soc_feature_tsn)) {
        return BCM_E_UNAVAIL;
    }
    if (NULL == tsn_stat_control[unit]) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN stat control Error: "
                              "not initialized\n")));
        return BCM_E_INIT;
    }
    if (NULL == tsn_stat_control[unit]->tsn_stat_dev_info) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "TSN stat dev info Error: "
                              "not initialized\n")));
        return BCM_E_INIT;
    }

    /* Fill info structure. */
    *tc = tsn_stat_control[unit];

    return BCM_E_NONE;
}

/*
 * Function:
 *    tsn_stat_portcnt_control_instance_get
 * Description:
 *    Helper function to retrieve TSN stat portcnt control pointer
 * Parameters:
 *    unit  - (IN) BCM device number
 *    pc   - (OUT) TSN port stat control data structure
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
tsn_stat_portcnt_control_instance_get(
    int unit,
    bcmi_tsn_stat_portcnt_control_t **pc)
{
    /* Input parameter check. */
    if (NULL == pc) {
        return BCM_E_PARAM;
    }

    /* Check if TSN feature supported and initialized for TSN portcnt */
    TSN_STAT_PORTCNT_IS_INIT(unit);

    /* Fill info structure. */
    *pc = tsn_stat_control[unit]->portcnt_control;

    return BCM_E_NONE;
}

/*
 * Function:
 *    tsn_stat_flowcnt_control_instance_get
 * Description:
 *    Helper function to retrieve TSN stat flowcnt control pointer
 * Parameters:
 *    unit  - (IN) BCM device number
 *    fc   - (OUT) TSN flow stat control data structure
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
tsn_stat_flowcnt_control_instance_get(
    int unit,
    bcmi_tsn_stat_flowcnt_control_t **fc)
{
    /* Input parameter check. */
    if (NULL == fc) {
        return BCM_E_PARAM;
    }

    /* Check if TSN feature supported and initialized for TSN portcnt */
    TSN_STAT_FLOWCNT_IS_INIT(unit);

    /* Fill info structure. */
    *fc = tsn_stat_control[unit]->flowcnt_control;

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_gport_validate
 * Description:
 *    Helper function to validate port/gport parameter
 * Parameters:
 *    unit  - (IN) BCM device number
 *    port_in  - (IN) Port / Gport to validate
 *    port_out - (OUT) Port number if valid.
 * Returns:
 *    BCM_E_NONE - Port OK
 *    BCM_E_INIT - Not initialized
 *    BCM_E_PORT - Port Invalid
 */
STATIC int
bcmi_esw_tsn_stat_gport_validate(
    int unit,
    bcm_port_t port_in,
    bcm_port_t *port_out)
{
    /* Input parameter check. */
    if (NULL == port_out) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port_in)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, port_in, port_out));
    } else if (SOC_PORT_VALID(unit, port_in) &&
               SOC_PBMP_MEMBER(PBMP_ALL(unit), port_in)) {
        *port_out = port_in;
    } else {
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_sync
 * Purpose:
 *    Synchronize the TSN Portcnt Stat/Port sw bitmap into scache
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_esw_tsn_stat_portcnt_sync(int unit)
{
    bcmi_tsn_stat_control_t *tc;
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    bcmi_tsn_stat_portcnt_bk_info_t *portcnt_bk_info;
    const bcmi_esw_tsn_stat_dev_info_t *tsn_stat_dev_info;
    const tsn_stat_portcnt_dev_info_t *portcnt_dev_info;
    int num_stat;
    int num_stat_port;
    soc_scache_handle_t scache_handle;
    uint8 *portcnt_scache_ptr;
    uint32 portcnt_scache_size;
    int i, p, num_port;

    /* Get TSN stat control */
    BCM_IF_ERROR_RETURN(
        tsn_stat_control_instance_get(unit, &tc));

    /* Get portcnt control */
    BCM_IF_ERROR_RETURN(
        tsn_stat_portcnt_control_instance_get(unit, &pc));

    /* Get chip specific info for TSN stat portcnt */
    tsn_stat_dev_info = tc->tsn_stat_dev_info;
    portcnt_dev_info = tsn_stat_dev_info->tsn_stat_portcnt_dev_info;
    portcnt_bk_info = pc->portcnt_bk_info;

    /* Get num_stat  */
    num_stat = portcnt_dev_info->num_stats;
    /* Get num_stat_port */
    num_stat_port = (num_stat * portcnt_bk_info->num_port);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_TSN_STAT_PORTCNT);
    /* allocate the scache pointer size */
    /* TSN portcnt stat_bitmap and stat_port_bitmap */
    portcnt_scache_size = SHR_BITALLOCSIZE(num_stat) +
                          SHR_BITALLOCSIZE(num_stat_port);

    /* TSN Port Stat counter port_counter_hw and port_counter_sw */
    portcnt_member = &pc->stats[0];
    num_port = soc_mem_index_count(unit, portcnt_member->mem);
    portcnt_scache_size += (num_stat * (num_port * sizeof(uint64)) * 2);

    BCM_IF_ERROR_RETURN(
        _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                portcnt_scache_size, &portcnt_scache_ptr,
                                BCM_TSN_WB_DEFAULT_VERSION, NULL));
    if (NULL == portcnt_scache_ptr) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit,
                                "SCACHE Memory not available \n")));
        return BCM_E_MEMORY;
    }

    TSN_STAT_PORTCNT_LOCK(pc);

    /* TSN portcnt stat_bitmap */
    SHR_BITCOPY_RANGE((SHR_BITDCL *)(portcnt_scache_ptr), 0,
                      portcnt_bk_info->stat_bitmap, 0, num_stat);
    portcnt_scache_ptr += SHR_BITALLOCSIZE(num_stat);

    /* TSN portcnt stat_port_bitmap */
    SHR_BITCOPY_RANGE((SHR_BITDCL *)(portcnt_scache_ptr), 0,
                      portcnt_bk_info->stat_port_bitmap, 0, num_stat_port);
    portcnt_scache_ptr += SHR_BITALLOCSIZE(num_stat_port);

    /* TSN Port Stat counter for port_counter_hw and port_counter_sw */
    for (i = 0; i < num_stat; i++) {
        portcnt_member = &pc->stats[i];
        num_port = soc_mem_index_count(unit, portcnt_member->mem);

        for (p = 0; p < num_port; p++) {
            /* TSN Port Stat counter port_counter_hw */
            sal_memcpy(portcnt_scache_ptr,
                       &(portcnt_member->port_counter_hw[p]), sizeof(uint64));
            portcnt_scache_ptr += sizeof(uint64);
            /* TSN Port Stat counter port_counter_sw */
            sal_memcpy(portcnt_scache_ptr,
                       &(portcnt_member->port_counter_sw[p]), sizeof(uint64));
            portcnt_scache_ptr += sizeof(uint64);
        }
    }

    TSN_STAT_PORTCNT_UNLOCK(pc);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_reload
 * Purpose:
 *    Reload the scache data into TSN Portcnt Stat/Port sw bitmap
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_esw_tsn_stat_portcnt_reload(int unit)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_control_t *tc;
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    bcmi_tsn_stat_portcnt_bk_info_t *portcnt_bk_info;
    const bcmi_esw_tsn_stat_dev_info_t *tsn_stat_dev_info;
    const tsn_stat_portcnt_dev_info_t *portcnt_dev_info;
    int num_stat;
    int num_stat_port;
    soc_scache_handle_t scache_handle;
    uint8 *portcnt_scache_ptr;
    int i, p, num_port;

    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                         "bcmi_esw_tsn_stat_portcnt_reload\n")));

    /* Get TSN stat control */
    BCM_IF_ERROR_RETURN(
        tsn_stat_control_instance_get(unit, &tc));

    /* Get portcnt control */
    BCM_IF_ERROR_RETURN(
        tsn_stat_portcnt_control_instance_get(unit, &pc));

    /* Get chip specific info for TSN stat portcnt */
    tsn_stat_dev_info = tc->tsn_stat_dev_info;
    portcnt_dev_info = tsn_stat_dev_info->tsn_stat_portcnt_dev_info;
    portcnt_bk_info = pc->portcnt_bk_info;

    /* Get num_stat  */
    num_stat = portcnt_dev_info->num_stats;
    /* Get num_stat_port */
    num_stat_port = (num_stat * portcnt_bk_info->num_port);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_TSN_STAT_PORTCNT);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &portcnt_scache_ptr,
                                 BCM_TSN_WB_DEFAULT_VERSION, NULL);

    if (rv == BCM_E_NOT_FOUND) {
        portcnt_scache_ptr = NULL;
        rv = BCM_E_NONE;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (portcnt_scache_ptr != NULL) {
        /* TSN portcnt stat_bitmap */
        SHR_BITCOPY_RANGE(portcnt_bk_info->stat_bitmap, 0,
                          (SHR_BITDCL *)(portcnt_scache_ptr), 0,
                          num_stat);
        portcnt_scache_ptr += SHR_BITALLOCSIZE(num_stat);

        /* TSN portcnt stat_port_bitmap */
        SHR_BITCOPY_RANGE(portcnt_bk_info->stat_port_bitmap, 0,
                          (SHR_BITDCL *)(portcnt_scache_ptr), 0,
                          num_stat_port);
        portcnt_scache_ptr += SHR_BITALLOCSIZE(num_stat_port);

        /* TSN Port Stat counter for port_counter_hw and port_counter_sw */
        for (i = 0; i < num_stat; i++) {
            portcnt_member = &pc->stats[i];
            num_port = soc_mem_index_count(unit, portcnt_member->mem);

            for (p = 0; p < num_port; p++) {
                /* TSN Port Stat counter port_counter_hw */
                sal_memcpy(&(portcnt_member->port_counter_hw[p]),
                           portcnt_scache_ptr, sizeof(uint64));
                portcnt_scache_ptr += sizeof(uint64);
                /* TSN Port Stat counter port_counter_sw */
                sal_memcpy(&(portcnt_member->port_counter_sw[p]),
                           portcnt_scache_ptr, sizeof(uint64));
                portcnt_scache_ptr += sizeof(uint64);
            }
        }
    }

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_flowcnt_sync
 * Purpose:
 *    Synchronize the TSN Flowcnt Stat into scache
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_esw_tsn_stat_flowcnt_sync(int unit)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int scache_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_TSN_STAT_FLOWCNT);

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    TSN_STAT_FLOWCNT_LOCK(fc);
    rv = TSN_STAT_FLOWCNT_FUNC(get_scache_size, (unit, &scache_size));

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_scache_ptr_get(
                 unit, scache_handle, TRUE,
                 scache_size,
                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    }

    if (BCM_SUCCESS(rv)) {
        rv = TSN_STAT_FLOWCNT_FUNC(sync, (unit, scache_ptr));
    }
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_flowcnt_reload
 * Purpose:
 *    Reload the scache data into TSN Flowcnt Stat
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_xxx
 */
int
bcmi_esw_tsn_stat_flowcnt_reload(int unit)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;
    soc_scache_handle_t scache_handle;
    uint8 *scache_ptr;
    int scache_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TSN,
                          BCM_TSN_WB_SCACHE_PART_TSN_STAT_FLOWCNT);

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    TSN_STAT_FLOWCNT_LOCK(fc);
    rv = TSN_STAT_FLOWCNT_FUNC(get_scache_size, (unit, &scache_size));

    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_scache_ptr_get(
                 unit, scache_handle, FALSE,
                 scache_size,
                 &scache_ptr, BCM_TSN_WB_DEFAULT_VERSION, NULL);
    }

    if (BCM_E_NOT_FOUND == rv) {
        /* proceed with Level 1 Warm Boot */
        rv = TSN_STAT_FLOWCNT_FUNC(reload_l1, (unit));
    } else if (BCM_E_NONE == rv) {
        if (NULL == scache_ptr) {
            rv = BCM_E_INTERNAL;
        } else {
            rv = TSN_STAT_FLOWCNT_FUNC(reload_l2, (unit, scache_ptr));
        }
    }
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_collect
 * Description:
 *    TSN Port Counter accumulation routine called periodically
 *    to sync s/w copy with h/w copy.
 * Parameters:
 *    unit - (IN) Unit number
 *    mem -  (IN) TSN port stat counter memory (if -1 for all memories)
 *    counter_idx - (IN) counter index (port index).
 *                  if -1 accumulate for all counters (for ports)
 *                  else retrieve a specific counter (one port).
 * Returns:
 *    None
 * Notes:
 */
STATIC void
bcmi_esw_tsn_stat_portcnt_collect(
    int unit,
    soc_mem_t mem,
    int counter_idx)
{
    uint32 mem_index_min;
    int mem_index_max;
    uint32 packet_count_hw[2];
    uint64 new_packet_count;
    uint64 prev_packet_count;
    uint64 max_packet_size;
    soc_memacc_t memacc_pkt;
    soc_mem_t l_mem;
    soc_field_t l_field;
    int i, index, width;
    bcmi_tsn_stat_control_t *tc;
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    bcmi_tsn_stat_portcnt_table_entry_t *portcnt_entry_temp;
    const bcmi_esw_tsn_stat_dev_info_t *tsn_stat_dev_info;
    const tsn_stat_portcnt_dev_info_t *portcnt_dev_info;
    uint32 *portcnt_entry_temp_ptr_start, *portcnt_entry_temp_ptr;
    int entry_words;

    /* Get TSN stat control */
    (void)tsn_stat_control_instance_get(unit, &tc);
    if (NULL == tc) {
        return;
    }

    /* Get chip specific info for TSN stat */
    tsn_stat_dev_info = tc->tsn_stat_dev_info;
    if (NULL == tsn_stat_dev_info) {
        return;
    }
    portcnt_dev_info = tsn_stat_dev_info->tsn_stat_portcnt_dev_info;
    if (NULL == portcnt_dev_info) {
        return;
    }

    /* Get portcnt control */
    (void)tsn_stat_portcnt_control_instance_get(unit, &pc);
    if (NULL == pc) {
        return;
    }

    COMPILER_64_ZERO(new_packet_count);
    COMPILER_64_ZERO(prev_packet_count);
    COMPILER_64_ZERO(max_packet_size);

    l_mem = pc->stats[0].mem;
    l_field = pc->stats[0].field;
    /* Get the packet count field width for this device */
    width = soc_mem_field_length(unit, l_mem, l_field);

    /* Hardware support maximum packet size */
    COMPILER_64_SET(max_packet_size, 0, 1);
    COMPILER_64_SHL(max_packet_size, width);

    portcnt_entry_temp = pc->portcnt_entry_temp;
    portcnt_entry_temp_ptr_start = &(portcnt_entry_temp[0].entry_data[0]);
    for (i = 0; i < portcnt_dev_info->num_stats; i++) {
        /* Check if the matrix of (stats) is enabled for portcnt callback */
        if (!TSN_STAT_PORTCNT_USED_GET(pc->portcnt_bk_info, i)) {
            continue;
        }

        l_mem = pc->stats[i].mem;
        l_field = pc->stats[i].field;

        /* Get entry_words for each memory */
        entry_words = soc_mem_entry_words(unit, l_mem);
        if ((mem == -1) || (l_mem == mem)) {
            portcnt_member = &pc->stats[i];

            mem_index_min = soc_mem_index_min(unit, l_mem);
            mem_index_max = soc_mem_index_max(unit, l_mem);

            /*
             * If counter_idx != -1, retrieve specific
             * counter index from the flex counter memory pool.
             */
            if (counter_idx != -1) {
                if ((counter_idx < mem_index_min) ||
                    (counter_idx > mem_index_max)) {
                    return;
                }
                mem_index_min = mem_index_max = counter_idx;
            }

            /* Read all the counters or a specific counter from memory */
            if (counter_idx == -1) {
                if (soc_mem_read_range(unit, l_mem, MEM_BLOCK_ANY,
                                       mem_index_min, mem_index_max,
                                       portcnt_entry_temp_ptr_start) !=
                                       BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_TSN,
                              (BSL_META_U(unit,
                                          "Unable to read a range of mem: "
                                          "%s\n"),
                              SOC_MEM_UFNAME(unit, l_mem)));
                    return;
                }
            } else {
                portcnt_entry_temp_ptr =
                    (portcnt_entry_temp_ptr_start + (entry_words * counter_idx));
                if (soc_mem_read(unit, l_mem, MEM_BLOCK_ANY,
                                 counter_idx,
                                 portcnt_entry_temp_ptr) != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_TSN,
                              (BSL_META_U(unit,
                                          "Unable to read mem: %s\n"),
                              SOC_MEM_UFNAME(unit, l_mem)));
                    return;
                }
            }

            /* Get a (memory, field) access information structure */
            if (soc_memacc_init(unit, l_mem, l_field, &memacc_pkt)) {
                LOG_ERROR(BSL_LS_BCM_TSN,
                          (BSL_META_U(unit,
                                      "Invalid mem: %s or field: %s\n"),
                          SOC_MEM_UFNAME(unit, l_mem),
                          SOC_FIELD_NAME(unit, l_field)));
                return;
            }

            for (index = mem_index_min; index <= mem_index_max; index++) {
                /*
                 * Check if the matrix of (stat, port) is enabled
                 * for portcnt callback
                 */
                if (!TSN_STAT_PORTCNT_PORT_USED_GET(pc->portcnt_bk_info,
                                                    i, index)) {
                    continue;
                }

                portcnt_entry_temp_ptr =
                    (portcnt_entry_temp_ptr_start + (entry_words * index));
                soc_memacc_field_get(&memacc_pkt,
                                     portcnt_entry_temp_ptr,
                                     packet_count_hw);

                /* Set current hw counter into 64-bit new_packet_count */
                if (width > 32) {
                    /* 64-bit field */
                    COMPILER_64_SET(new_packet_count,
                                    packet_count_hw[1],
                                    packet_count_hw[0]);
                } else {
                     /* 32-bit field */
                    COMPILER_64_SET(new_packet_count,
                                    0,
                                    packet_count_hw[0]);
                }

                /* Set previous hw counter into prev_packet_count */
                COMPILER_64_COPY(prev_packet_count,
                                 portcnt_member->port_counter_hw[index]);

                /* Compare old and new hardware counter */
                if (!COMPILER_64_EQ(prev_packet_count, new_packet_count)) {
                    LOG_VERBOSE(BSL_LS_BCM_TSN,
                                (BSL_META_U(unit,
                                            "Port %d: TSN stat(%d)==>"
                                            "Old Packet Count Value %x:%x\t"
                                            "New Packet Count Value %x:%x\n"),
                                index, i,
                                COMPILER_64_HI(prev_packet_count),
                                COMPILER_64_LO(prev_packet_count),
                                COMPILER_64_HI(new_packet_count),
                                COMPILER_64_LO(new_packet_count)));
                    /* Update Soft Copy for current HW counter */
                    COMPILER_64_COPY(portcnt_member->port_counter_hw[index],
                                     new_packet_count);
                }

                /*
                 * Calculate the differences between new and previous
                 * packet counters
                 */
                if (COMPILER_64_GT(prev_packet_count, new_packet_count)) {
                    /* Roll over case */
                    LOG_VERBOSE(BSL_LS_BCM_TSN,
                                (BSL_META_U(unit,
                                            "Roll over happend!\n")));
                    LOG_VERBOSE(BSL_LS_BCM_TSN,
                                (BSL_META_U(unit,
                                            "... Read Packet HW Count    :"
                                            " %x:%x\n"),
                                COMPILER_64_HI(new_packet_count),
                                COMPILER_64_LO(new_packet_count)));

                    COMPILER_64_ADD_64(new_packet_count, max_packet_size);
                    COMPILER_64_SUB_64(new_packet_count, prev_packet_count);
                    LOG_VERBOSE(BSL_LS_BCM_TSN,
                                (BSL_META_U(unit,
                                            "... Diffed Packet HW Count    :"
                                            " %x:%x\n"),
                                COMPILER_64_HI(new_packet_count),
                                COMPILER_64_LO(new_packet_count)));
                } else {
                    /* Normal case */
                    COMPILER_64_SUB_64(new_packet_count, prev_packet_count);
                }

                /* Add difference (if it is) */
                if (!COMPILER_64_IS_ZERO(new_packet_count)) {
                    LOG_VERBOSE(BSL_LS_BCM_TSN,
                                (BSL_META_U(unit,
                                            "Port %d: TSN stat(%d)==>"
                                            "Old Packet Count SW Value: "
                                            "%x:%x\t"),
                                index, i,
                                COMPILER_64_HI(
                                    portcnt_member->port_counter_sw[index]),
                                COMPILER_64_LO(
                                    portcnt_member->port_counter_sw[index])));
                    COMPILER_64_ADD_64(
                        portcnt_member->port_counter_sw[index],
                        new_packet_count);
                    LOG_VERBOSE(BSL_LS_BCM_TSN,
                                (BSL_META_U(unit,
                                            "New Packet Count SW Value: "
                                            "%x:%x\n"),
                                COMPILER_64_HI(
                                    portcnt_member->port_counter_sw[index]),
                                COMPILER_64_LO(
                                    portcnt_member->port_counter_sw[index])));
                }
            }
        }
    }

    return;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_map
 * Description:
 *    Get the counter table index information based on
 *    the giving TSN statistic type
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *    stat_index - (OUT) TSN stat table index
 *    offset_index - (OUT) offset index (port) in the counter table
 *                         (specified TSN stat)
 * Returns:
 *    BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_esw_tsn_stat_portcnt_map(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    int *stat_index,
    int *offset_index)
{
    int i, rv = BCM_E_NONE;
    bcmi_tsn_stat_control_t *tc;
    const bcmi_esw_tsn_stat_dev_info_t *tsn_stat_dev_info;
    const tsn_stat_portcnt_dev_info_t *portcnt_dev_info;
    const tsn_stat_portcnt_table_info_t *portcnt_table;
    bcm_port_t port_out;

    /* Input parameter check. */
    if (NULL == stat_index) {
        return BCM_E_PARAM;
    }
    if (!TSN_STAT_IS_VALID(unit, stat)) {
        return BCM_E_PARAM;
    }

    /* The port will be ignored if offset_index == NULL */
    if (offset_index != NULL) {
        /* Get offset_index based on the giving gport */
        rv = bcmi_esw_tsn_stat_gport_validate(unit, port, &port_out);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        *offset_index = port_out;
    }

    /* Get TSN stat control */
    BCM_IF_ERROR_RETURN(
        tsn_stat_control_instance_get(unit, &tc));

    /* Get chip specific info for TSN stat */
    tsn_stat_dev_info = tc->tsn_stat_dev_info;
    if (NULL == tsn_stat_dev_info) {
        return BCM_E_UNAVAIL;
    }
    portcnt_dev_info = tsn_stat_dev_info->tsn_stat_portcnt_dev_info;
    if (NULL == portcnt_dev_info) {
        return BCM_E_UNAVAIL;
    }

    /* Get stat_index based on the giving TSN statistic type = stat */
    portcnt_table = portcnt_dev_info->portcnt_table_info;
    if (NULL == portcnt_table) {
        return BCM_E_UNAVAIL;
    }

    *stat_index = -1;
    for (i = 0; i < portcnt_dev_info->num_stats; i++) {
        bcm_tsn_stat_t stat_itr = portcnt_table[i].portcnt_stat;

        assert((stat_itr >= 0) && (stat_itr < bcmTsnStatCount));
        if (!soc_feature(unit, tsn_stat_type_feature[stat_itr])) {
            continue;
        }
        if (stat == stat_itr) {
            *stat_index = i;
            break;
        }
    }

    if (*stat_index == -1) {
        /* Cannot find specified TSN statistic type for this device */
        return BCM_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_cleanup
 * Description:
 *    Clean and free all allocated TSN port counter resources
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_stat_portcnt_cleanup(int unit)
{
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    bcmi_tsn_stat_portcnt_bk_info_t *portcnt_bk_info;
    const bcmi_esw_tsn_stat_dev_info_t *tsn_stat_dev_info;
    const tsn_stat_portcnt_dev_info_t *portcnt_dev_info;
    int i;

    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                          "bcmi_esw_tsn_stat_portcnt_cleanup\n")));

    pc = tsn_stat_control[unit]->portcnt_control;

    if (pc->stats != NULL) {
        tsn_stat_dev_info = tsn_stat_control[unit]->tsn_stat_dev_info;
        if (tsn_stat_dev_info != NULL) {
            portcnt_dev_info =
                tsn_stat_dev_info->tsn_stat_portcnt_dev_info;
            if (portcnt_dev_info != NULL) {
                for (i = 0; i < portcnt_dev_info->num_stats; i++) {
                    portcnt_member = &pc->stats[i];
                    if (portcnt_member->port_counter_sw != NULL) {
                        sal_free(portcnt_member->port_counter_sw);
                        portcnt_member->port_counter_sw = NULL;
                    }
                    if (portcnt_member->port_counter_hw != NULL) {
                        sal_free(portcnt_member->port_counter_hw);
                        portcnt_member->port_counter_hw = NULL;
                    }
                }
            }
        }
        sal_free(pc->stats);
        pc->stats = NULL;
    }

    if (pc->portcnt_entry_temp != NULL) {
        soc_cm_sfree(unit, pc->portcnt_entry_temp);
        pc->portcnt_entry_temp = NULL;
    }

    if (pc->portcnt_bk_info != NULL) {
        portcnt_bk_info = pc->portcnt_bk_info;
        if (portcnt_bk_info->stat_bitmap != NULL) {
            sal_free(portcnt_bk_info->stat_bitmap);
            portcnt_bk_info->stat_bitmap = NULL;
        }
        if (portcnt_bk_info->stat_port_bitmap != NULL) {
            sal_free(portcnt_bk_info->stat_port_bitmap);
            portcnt_bk_info->stat_port_bitmap = NULL;
        }
        sal_free(pc->portcnt_bk_info);
        pc->portcnt_bk_info = NULL;
    }

    if (pc->portcnt_mutex != NULL) {
        sal_mutex_destroy(pc->portcnt_mutex);
        pc->portcnt_mutex = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_init
 * Description:
 *    Initialize and allocate all required resources for TSN port counters
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_stat_portcnt_init(int unit)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_control_t *tc;
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    bcmi_tsn_stat_portcnt_bk_info_t *portcnt_bk_info;
    const bcmi_esw_tsn_stat_dev_info_t *tsn_stat_dev_info;
    const tsn_stat_portcnt_dev_info_t *portcnt_dev_info;
    const tsn_stat_portcnt_table_info_t *portcnt_table;
    int i, num_port = 0;

    /* Get TSN stat control */
    BCM_IF_ERROR_RETURN(
        tsn_stat_control_instance_get(unit, &tc));

    pc = tc->portcnt_control;
    if (NULL == pc) {
        return BCM_E_INIT;
    }
    sal_memset(pc, 0, sizeof(bcmi_tsn_stat_portcnt_control_t));

    /* Create portcnt protection mutex. */
    pc->portcnt_mutex = sal_mutex_create("tsn portcnt control mutex");
    if (NULL == pc->portcnt_mutex) {
        return BCM_E_RESOURCE;
    }

    /* Get chip specific info for TSN stat portcnt */
    tsn_stat_dev_info = tc->tsn_stat_dev_info;
    if (NULL == tsn_stat_dev_info) {
        return BCM_E_UNAVAIL;
    }
    portcnt_dev_info = tsn_stat_dev_info->tsn_stat_portcnt_dev_info;
    if (NULL == portcnt_dev_info) {
        return BCM_E_UNAVAIL;
    }

    /* Allocate the TSN portcnt stats member resource */
    pc->stats =
        sal_alloc((portcnt_dev_info->num_stats *
                   sizeof(bcmi_tsn_stat_portcnt_member_t)),
                  "TSN portcnt stats");
    if (NULL == pc->stats) {
        return BCM_E_MEMORY;
    }
    sal_memset(pc->stats, 0,
               (portcnt_dev_info->num_stats *
                sizeof(bcmi_tsn_stat_portcnt_member_t)));

    /*
     * For each TSN Port Stat counter:
     * allocate *port_counter_sw and *port_counter_hw for all ports
     */
    portcnt_table = portcnt_dev_info->portcnt_table_info;
    for (i = 0; i < portcnt_dev_info->num_stats; i++) {
        portcnt_member = &pc->stats[i];
        portcnt_member->mem = portcnt_table[i].portcnt_mem;
        portcnt_member->field = portcnt_table[i].portcnt_field;
        num_port = soc_mem_index_count(unit, portcnt_member->mem);

        /* Allocate port_counter_sw */
        portcnt_member->port_counter_sw =
            sal_alloc((num_port * sizeof(uint64)), "TSN port counter sw");
        if (NULL == portcnt_member->port_counter_sw) {
            return BCM_E_MEMORY;
        }
        sal_memset(portcnt_member->port_counter_sw, 0,
                   (num_port * sizeof(uint64)));

        /* Allocate port_counter_hw */
        portcnt_member->port_counter_hw =
            sal_alloc((num_port * sizeof(uint64)), "TSN port counter hw");
        if (NULL == portcnt_member->port_counter_hw) {
            return BCM_E_MEMORY;
        }
        sal_memset(portcnt_member->port_counter_hw, 0,
                   (num_port * sizeof(uint64)));
    }

    /* Allocate the TSN portcnt temporary counter resource for DMA */
    num_port = soc_mem_index_count(unit, portcnt_table[0].portcnt_mem);
    pc->portcnt_entry_temp =
        soc_cm_salloc(unit,
                      (num_port * sizeof(bcmi_tsn_stat_portcnt_table_entry_t)),
                      "TSN portcnt temporary counter");
    if (NULL == pc->portcnt_entry_temp) {
        return BCM_E_MEMORY;
    }
    sal_memset(pc->portcnt_entry_temp, 0,
               (num_port * sizeof(bcmi_tsn_stat_portcnt_table_entry_t)));

    /* Allocate the TSN portcnt book keeping info resource */
    pc->portcnt_bk_info =
        sal_alloc(sizeof(bcmi_tsn_stat_portcnt_bk_info_t),
                  "TSN portcnt book keeping");
    if (NULL == pc->portcnt_bk_info) {
        return BCM_E_MEMORY;
    }
    sal_memset(pc->portcnt_bk_info, 0,
               sizeof(bcmi_tsn_stat_portcnt_bk_info_t));

    portcnt_bk_info = pc->portcnt_bk_info;
    portcnt_bk_info->stat_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(portcnt_dev_info->num_stats),
                  "TSN portcnt stat_bitmap");
    if (NULL == portcnt_bk_info->stat_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(portcnt_bk_info->stat_bitmap, 0,
               SHR_BITALLOCSIZE(portcnt_dev_info->num_stats));

    portcnt_bk_info->stat_port_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(portcnt_dev_info->num_stats * num_port),
                  "TSN portcnt stat_port_bitmap");
    if (NULL == portcnt_bk_info->stat_port_bitmap) {
        return BCM_E_MEMORY;
    }
    sal_memset(portcnt_bk_info->stat_port_bitmap, 0,
               SHR_BITALLOCSIZE(portcnt_dev_info->num_stats * num_port));

    portcnt_bk_info->num_port = num_port;

    
    /* WARMBOOT related */

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_set
 * Description:
 *    Set 64-bit counter value for specified TSN statistic type.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *     val - (IN) 64-bit counter value.
 * Returns:
 *    BCM_E_XXX.
 * Notes:
 */
int
bcmi_esw_tsn_stat_portcnt_set(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    uint64 val)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    int stat_index, offset_index, total_entries;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t counter_mem;
    soc_field_t counter_field;
    uint32 max_packet_value[2];
    int width;

    BCM_IF_ERROR_RETURN(
        tsn_stat_portcnt_control_instance_get(unit, &pc));

    rv = bcmi_esw_tsn_stat_portcnt_map(unit, port, stat,
                                       &stat_index, &offset_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    TSN_STAT_PORTCNT_LOCK(pc);
    portcnt_member = &pc->stats[stat_index];
    counter_mem = portcnt_member->mem;
    counter_field = portcnt_member->field;
    total_entries = soc_mem_index_count(unit, counter_mem);

    if (offset_index >= total_entries) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX(Port). "
                              "Must be < Total Entries %d \n"),
                  total_entries));
        TSN_STAT_PORTCNT_UNLOCK(pc);
        assert(0);
    }

    /* Memory read from hardware */
    sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    if (soc_mem_read(unit, counter_mem, MEM_BLOCK_ANY,
                     offset_index, &entry) != BCM_E_NONE) {
        TSN_STAT_PORTCNT_UNLOCK(pc);
        return BCM_E_INTERNAL;
    }

    width = soc_mem_field_length(unit, counter_mem, counter_field);
    if (width > 32) {
        /* 64-bit field */
        max_packet_value[0] = COMPILER_64_LO(val);
        max_packet_value[1] = COMPILER_64_HI(val);
    } else {
         /* 32-bit field */
        max_packet_value[0] = COMPILER_64_LO(val);
        max_packet_value[1] = 0;
    }
    soc_mem_field_set(unit, counter_mem, (uint32 *)entry,
                      counter_field, max_packet_value);

    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                          "Packet Count HW Value\t:TABLE:"
                          "%s INDEX(port):%d : %x:%x \n"),
              SOC_MEM_UFNAME(unit, counter_mem),
              offset_index,
              max_packet_value[1],
              max_packet_value[0]));

    /* Memory write into hardware */
    if (soc_mem_write(unit, counter_mem, MEM_BLOCK_ALL,
                      offset_index, &entry) != BCM_E_NONE) {
        TSN_STAT_PORTCNT_UNLOCK(pc);
        return BCM_E_INTERNAL;
    }

    /* Update Soft Copy for HW counter */
    COMPILER_64_SET(portcnt_member->port_counter_hw[offset_index],
                    max_packet_value[1],
                    max_packet_value[0]);

    /* Update Soft Copy for SW counter */
    COMPILER_64_SET(portcnt_member->port_counter_sw[offset_index],
                    COMPILER_64_HI(val),
                    COMPILER_64_LO(val));

    /* Set to enable the portcnt callback for matrix of (stat, port) */
    if (!TSN_STAT_PORTCNT_USED_GET(pc->portcnt_bk_info, stat_index)) {
        TSN_STAT_PORTCNT_USED_SET(pc->portcnt_bk_info, stat_index);
    }
    if (!TSN_STAT_PORTCNT_PORT_USED_GET(pc->portcnt_bk_info,
                                        stat_index, offset_index)) {
        TSN_STAT_PORTCNT_PORT_USED_SET(pc->portcnt_bk_info,
                                       stat_index, offset_index);
    }

    TSN_STAT_PORTCNT_UNLOCK(pc);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_get
 * Description:
 *    Get 64-bit counter value for specified TSN statistic type.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    sync_mode - (IN) if 1 read hw counter else sw accumulated counter
 *    stat - (IN) TSN statistics type (see tsn.h)
 *     val - (OUT) 64-bit counter value.
 * Returns:
 *    BCM_E_XXX.
 * Notes:
 */
int
bcmi_esw_tsn_stat_portcnt_get(
    int unit,
    bcm_gport_t port,
    int sync_mode,
    bcm_tsn_stat_t stat,
    uint64 *val)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    int stat_index, offset_index, total_entries;
    soc_mem_t counter_mem;

    /* Input parameter check. */
    if (NULL == val) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        tsn_stat_portcnt_control_instance_get(unit, &pc));

    rv = bcmi_esw_tsn_stat_portcnt_map(unit, port, stat,
                                       &stat_index, &offset_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    portcnt_member = &pc->stats[stat_index];
    counter_mem = portcnt_member->mem;
    total_entries = soc_mem_index_count(unit, counter_mem);

    if (offset_index >= total_entries) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX(Port). "
                              "Must be < Total Entries %d \n"),
                  total_entries));
        return BCM_E_PARAM;
    }

    TSN_STAT_PORTCNT_LOCK(pc);

    /* Set to enable the portcnt callback for matrix of (stat, port) */
    if (!TSN_STAT_PORTCNT_USED_GET(pc->portcnt_bk_info, stat_index)) {
        TSN_STAT_PORTCNT_USED_SET(pc->portcnt_bk_info, stat_index);
    }
    if (!TSN_STAT_PORTCNT_PORT_USED_GET(pc->portcnt_bk_info,
                                        stat_index, offset_index)) {
        TSN_STAT_PORTCNT_PORT_USED_SET(pc->portcnt_bk_info,
                                       stat_index, offset_index);
        /* Force sync_mode = 1 for the first time touch */
        sync_mode = 1;
    }

    /* sync the software counter with hardware counter */
    if (sync_mode == 1) {
        bcmi_esw_tsn_stat_portcnt_collect(unit, counter_mem, offset_index);
    }

    /* Packet Count HW Value */
    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                          "Packet Count HW Value\t:TABLE:"
                          "%s INDEX(port):%d : %x:%x \n"),
              SOC_MEM_UFNAME(unit, counter_mem),
              offset_index,
              COMPILER_64_HI(portcnt_member->port_counter_hw[offset_index]),
              COMPILER_64_LO(portcnt_member->port_counter_hw[offset_index])));

    /* Return Packet Count SW Value */
    COMPILER_64_COPY(*val,
                     portcnt_member->port_counter_sw[offset_index]);
    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                          "Packet Count SW Value\t:TABLE:"
                          "%s INDEX(port):%d : %x:%x \n"),
              SOC_MEM_UFNAME(unit, counter_mem),
              offset_index,
              COMPILER_64_HI(*val),
              COMPILER_64_LO(*val)));

    TSN_STAT_PORTCNT_UNLOCK(pc);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_used_update
 * Description:
 *    Helper function to update TSN stat portcnt usage bitmap.
 * Parameters:
 *    unit - (IN) Unit number
 *    port - (IN) port number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *      op - (IN) TSN stat portcnt usage bitmap operation modes.
 * Returns:
 *    BCM_E_XXX.
 * Notes:
 */
int
bcmi_esw_tsn_stat_portcnt_used_update(
    int unit,
    bcm_gport_t port,
    bcm_tsn_stat_t stat,
    bcmi_tsn_stat_portcnt_used_operation_t op)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_portcnt_control_t *pc;
    int stat_index, offset_index;
    bcm_port_t port_out = 0, p;
    bcm_pbmp_t pbmp;
    int empty;

    BCM_PBMP_CLEAR(pbmp);
    if (port == -1) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
        BCM_PBMP_ITER(pbmp, p) {
            port_out = p;
            break;
        }
    } else {
        rv = bcmi_esw_tsn_stat_gport_validate(unit, port, &port_out);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        BCM_PBMP_PORT_SET(pbmp, port_out);
    }

    BCM_IF_ERROR_RETURN(
        tsn_stat_portcnt_control_instance_get(unit, &pc));

    rv = bcmi_esw_tsn_stat_portcnt_map(unit, port_out, stat,
                                       &stat_index, &offset_index);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    switch (op) {
        case BCM_TSN_STAT_PORTCNT_USED_SET:
            if (!TSN_STAT_PORTCNT_USED_GET(pc->portcnt_bk_info, stat_index)) {
                TSN_STAT_PORTCNT_USED_SET(pc->portcnt_bk_info, stat_index);
            }

            BCM_PBMP_ITER(pbmp, p) {
                if (!TSN_STAT_PORTCNT_PORT_USED_GET(pc->portcnt_bk_info,
                                                    stat_index, p)) {
                    TSN_STAT_PORTCNT_PORT_USED_SET(pc->portcnt_bk_info,
                                                   stat_index, p);
                }
            }

            break;
        case BCM_TSN_STAT_PORTCNT_USED_CLEAR:
            if (TSN_STAT_PORTCNT_USED_GET(pc->portcnt_bk_info, stat_index)) {
                BCM_PBMP_ITER(pbmp, p) {
                    if (TSN_STAT_PORTCNT_PORT_USED_GET(pc->portcnt_bk_info,
                                                       stat_index, p)) {
                        TSN_STAT_PORTCNT_PORT_USED_CLR(pc->portcnt_bk_info,
                                                       stat_index, p);
                    }
                }

                empty = 1;
                PBMP_ALL_ITER(unit, p) {
                    if (TSN_STAT_PORTCNT_PORT_USED_GET(pc->portcnt_bk_info,
                                                       stat_index, p)) {
                        empty = 0; /* Not empty */
                        break;
                    }
                }

                if (empty) {
                    TSN_STAT_PORTCNT_USED_CLR(pc->portcnt_bk_info, stat_index);
                }
            }

            break;
        default:
            return BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_stat_sync
 * Description:
 *    Helper function to sync HW/SW TSN stat portcnt per specified stat type.
 * Parameters:
 *    unit - (IN) Unit number
 *    stat - (IN) TSN statistics type (see tsn.h)
 * Returns:
 *    BCM_E_XXX.
 * Notes:
 */
int
bcmi_esw_tsn_stat_portcnt_stat_sync(int unit, bcm_tsn_stat_t stat)
{
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    int stat_index;
    soc_mem_t counter_mem;

    BCM_IF_ERROR_RETURN(
        tsn_stat_portcnt_control_instance_get(unit, &pc));

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_stat_portcnt_map(unit, -1, stat,
                                      &stat_index, NULL));

    portcnt_member = &(pc->stats[stat_index]);
    counter_mem = portcnt_member->mem;

    /* Sync the software counter with hardware counter (per stat type) */
    TSN_STAT_PORTCNT_LOCK(pc);
    bcmi_esw_tsn_stat_portcnt_collect(unit, counter_mem, -1);
    TSN_STAT_PORTCNT_UNLOCK(pc);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_hw_sw_get
 * Description:
 *    Helper function to get both HW and SW TSN stat portcnt
 *    per specified stat type and port.
 * Parameters:
 *    unit - (IN) Unit number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *    port - (IN) port number
 *    hw_value - (OUT) 64-bit HW counter value.
 *    sw_value - (OUT) 64-bit SW counter value.
 * Returns:
 *    BCM_E_XXX.
 * Notes:
 */
int
bcmi_esw_tsn_stat_portcnt_hw_sw_get(
    int unit,
    bcm_tsn_stat_t stat,
    bcm_gport_t port,
    uint64 *hw_value,
    uint64 *sw_value)
{
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    int stat_index, offset_index, total_entries;
    soc_mem_t counter_mem;

    /* Input parameter check. */
    if ((NULL == hw_value) || (NULL == sw_value)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        tsn_stat_portcnt_control_instance_get(unit, &pc));

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_stat_portcnt_map(unit, port, stat,
                                      &stat_index, &offset_index));

    portcnt_member = &(pc->stats[stat_index]);
    counter_mem = portcnt_member->mem;
    total_entries = soc_mem_index_count(unit, counter_mem);

    if (offset_index >= total_entries) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX(Port). "
                              "Must be < Total Entries %d \n"),
                  total_entries));
        return BCM_E_PARAM;
    }

    TSN_STAT_PORTCNT_LOCK(pc);

    /* Return Packet Count HW Value */
    COMPILER_64_COPY(*hw_value,
                     portcnt_member->port_counter_hw[offset_index]);
    /* Return Packet Count SW Value */
    COMPILER_64_COPY(*sw_value,
                     portcnt_member->port_counter_sw[offset_index]);

    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                          "Packet Count HW Value\t:TABLE:"
                          "%s INDEX(port):%d : %x:%x \n"),
              SOC_MEM_UFNAME(unit, counter_mem),
              offset_index,
              COMPILER_64_HI(*hw_value),
              COMPILER_64_LO(*hw_value)));
    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                          "Packet Count SW Value\t:TABLE:"
                          "%s INDEX(port):%d : %x:%x \n"),
              SOC_MEM_UFNAME(unit, counter_mem),
              offset_index,
              COMPILER_64_HI(*sw_value),
              COMPILER_64_LO(*sw_value)));

    TSN_STAT_PORTCNT_UNLOCK(pc);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_hw_set
 * Description:
 *    Helper function to set HW only TSN stat portcnt
 *    per specified stat type and port.
 * Parameters:
 *    unit - (IN) Unit number
 *    stat - (IN) TSN statistics type (see tsn.h)
 *    port - (IN) port number
 *    hw_value - (OUT) 64-bit HW counter value.
 * Returns:
 *    BCM_E_XXX.
 * Notes:
 */
int
bcmi_esw_tsn_stat_portcnt_hw_set(
    int unit,
    bcm_tsn_stat_t stat,
    bcm_gport_t port,
    uint64 hw_value)
{
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_portcnt_member_t *portcnt_member;
    int stat_index, offset_index, total_entries;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t counter_mem;
    soc_field_t counter_field;
    uint32 packet_count_hw[2];
    uint64 old_packet_count;
    int width;

    BCM_IF_ERROR_RETURN(
        tsn_stat_portcnt_control_instance_get(unit, &pc));

    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_stat_portcnt_map(unit, port, stat,
                                      &stat_index, &offset_index));

    TSN_STAT_PORTCNT_LOCK(pc);
    portcnt_member = &(pc->stats[stat_index]);
    counter_mem = portcnt_member->mem;
    counter_field = portcnt_member->field;
    total_entries = soc_mem_index_count(unit, counter_mem);

    if (offset_index >= total_entries) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX(Port). "
                              "Must be < Total Entries %d \n"),
                  total_entries));
        TSN_STAT_PORTCNT_UNLOCK(pc);
        assert(0);
    }

    /* Memory read from hardware */
    sal_memset(entry, 0, sizeof(uint32) * SOC_MAX_MEM_WORDS);
    if (soc_mem_read(unit, counter_mem, MEM_BLOCK_ANY,
                     offset_index, &entry) != BCM_E_NONE) {
        TSN_STAT_PORTCNT_UNLOCK(pc);
        return BCM_E_INTERNAL;
    }

    width = soc_mem_field_length(unit, counter_mem, counter_field);

    /* Get current HW value */
    sal_memset(packet_count_hw, 0, sizeof(uint32) * 2);
    soc_mem_field_get(unit, counter_mem, (uint32 *)entry,
                      counter_field, packet_count_hw);
    if (width > 32) {
        /* 64-bit field */
        COMPILER_64_SET(old_packet_count,
                        packet_count_hw[1],
                        packet_count_hw[0]);
    } else {
         /* 32-bit field */
        COMPILER_64_SET(old_packet_count,
                        0,
                        packet_count_hw[0]);
    }

    /* Compare old and new hardware counter */
    if (!COMPILER_64_EQ(old_packet_count, hw_value)) {
        LOG_VERBOSE(BSL_LS_BCM_TSN,
                    (BSL_META_U(unit,
                                "Port %d: TSN stat(%d)==>"
                                "Old HW Packet Count Value %x:%x\t"
                                "New HW Packet Count Value %x:%x\n"),
                    offset_index, stat_index,
                    COMPILER_64_HI(old_packet_count),
                    COMPILER_64_LO(old_packet_count),
                    COMPILER_64_HI(hw_value),
                    COMPILER_64_LO(hw_value)));

        if (width > 32) {
            /* 64-bit field */
            packet_count_hw[0] = COMPILER_64_LO(hw_value);
            packet_count_hw[1] = COMPILER_64_HI(hw_value);
        } else {
             /* 32-bit field */
            packet_count_hw[0] = COMPILER_64_LO(hw_value);
            packet_count_hw[1] = 0;
        }

        soc_mem_field_set(unit, counter_mem, (uint32 *)entry,
                          counter_field, packet_count_hw);

        /* Memory write into hardware */
        if (soc_mem_write(unit, counter_mem, MEM_BLOCK_ALL,
                          offset_index, &entry) != BCM_E_NONE) {
            TSN_STAT_PORTCNT_UNLOCK(pc);
            return BCM_E_INTERNAL;
        }
    }

    /* Update Soft Copy for current HW counter */
    COMPILER_64_COPY(portcnt_member->port_counter_hw[offset_index],
                     hw_value);

    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                          "Packet Count HW Value\t:TABLE:"
                          "%s INDEX(port):%d : %x:%x \n"),
              SOC_MEM_UFNAME(unit, counter_mem),
              offset_index,
              COMPILER_64_HI(portcnt_member->port_counter_hw[offset_index]),
              COMPILER_64_LO(portcnt_member->port_counter_hw[offset_index])));

    TSN_STAT_PORTCNT_UNLOCK(pc);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_portcnt_update
 * Description:
 *    For SER module to update the SW & HW counter
 *    with given memory and memory index
 * Parameters:
 *    unit - (IN) Unit number
 *    mem -  (IN) TSN stat counter memory
 *    mem_idx - (IN) memory index
 *    new_count - (IN) new counter value
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_stat_portcnt_update(
    int unit,
    soc_mem_t mem,
    int mem_idx,
    uint64 new_count)
{
    bcmi_tsn_stat_control_t *tc;
    const bcmi_esw_tsn_stat_dev_info_t *tsn_stat_dev_info;
    const tsn_stat_portcnt_dev_info_t *portcnt_dev_info;
    const tsn_stat_portcnt_table_info_t *portcnt_table;
    int total_entries, i;
    bcm_gport_t port;
    bcm_tsn_stat_t stat_itr = 0;

    /* Input parameter check. */
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return BCM_E_PARAM;
    }

    total_entries = soc_mem_index_count(unit, mem);
    if (mem_idx >= total_entries) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX(Port). "
                              "Must be < Total Entries %d \n"),
                  total_entries));
        assert(0);
    }

    /* Get TSN stat control */
    BCM_IF_ERROR_RETURN(
        tsn_stat_control_instance_get(unit, &tc));

    /* Get chip specific info for TSN stat */
    tsn_stat_dev_info = tc->tsn_stat_dev_info;
    if (NULL == tsn_stat_dev_info) {
        return BCM_E_UNAVAIL;
    }
    portcnt_dev_info = tsn_stat_dev_info->tsn_stat_portcnt_dev_info;
    if (NULL == portcnt_dev_info) {
        return BCM_E_UNAVAIL;
    }

    /* Get stat_index based on the giving TSN statistic type = stat */
    portcnt_table = portcnt_dev_info->portcnt_table_info;
    if (NULL == portcnt_table) {
        return BCM_E_UNAVAIL;
    }

    for (i = 0; i < portcnt_dev_info->num_stats; i++) {
        stat_itr = portcnt_table[i].portcnt_stat;

        assert((stat_itr >= 0) && (stat_itr < bcmTsnStatCount));
        if (!soc_feature(unit, tsn_stat_type_feature[stat_itr])) {
            continue;
        }
        if (mem == portcnt_table[i].portcnt_mem) {
            break;
        }
    }

    if (i == portcnt_dev_info->num_stats) {
        return BCM_E_NOT_FOUND;
    }

    port = mem_idx;
    BCM_IF_ERROR_RETURN(
        bcmi_esw_tsn_stat_portcnt_set(unit, port, stat_itr, new_count));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_group_create
 * Description:
 *      Create a TSN stat group by picking stat types (in an array) to be
 *      included for counting
 * Parameters:
 *      unit             - (IN) unit number
 *      group_type       - (IN) TSN statistic group type
 *      count            - (IN) the number of elements in stat_arr
 *      stat_arr         - (IN) array for tsn stat types
 *      stat_id          - (OUT) tsn statistic group id
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_group_create(
    int unit,
    bcm_tsn_stat_group_type_t group_type,
    int count,
    bcm_tsn_stat_t *stat_arr,
    bcm_tsn_stat_group_t *stat_id)
{
    int i, rv = BCM_E_NONE;
    uint32 enable_values;
    uint32 group_id;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    /* check feature support or not */
    for (i = 0; i < count; i++) {
        bcm_tsn_stat_t stat_itr = stat_arr[i];

        if (stat_itr < 0 || stat_itr >= bcmTsnStatCount) {
            return BCM_E_PARAM;
        }
        if (!soc_feature(unit, tsn_stat_type_feature[stat_itr])) {
            return BCM_E_UNAVAIL;
        }
    }

    BCM_IF_ERROR_RETURN(
        TSN_STAT_FLOWCNT_FUNC(group_enable_value_encode,
            (unit, group_type, stat_arr, count, &enable_values)));

    TSN_STAT_FLOWCNT_LOCK(fc);
    rv = TSN_STAT_FLOWCNT_FUNC(group_enable_value_insert,
             (unit, group_type, enable_values, &group_id));
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    BCM_IF_ERROR_RETURN(rv);
    BCM_IF_ERROR_RETURN(
        TSN_STAT_FLOWCNT_FUNC(group_id_encode,
            (unit, group_type, group_id, stat_id)));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_group_set
 * Description:
 *      Set configuration of the TSN stat group
 * Parameters:
 *      unit             - (IN) unit number
 *      stat_id          - (IN) TSN statistic group id
 *      count            - (IN) the number of elements in stat_arr
 *      stat_arr         - (IN) array for TSN stat types
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_group_set(
    int unit,
    bcm_tsn_stat_group_t stat_id,
    int count,
    bcm_tsn_stat_t *stat_arr)
{
    int i, rv = BCM_E_NONE;
    bcm_tsn_stat_group_type_t group_type;
    uint32 group_id;
    int group_refcnt;
    uint32 new_enable_values;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    /* check feature support or not */
    for (i = 0; i < count; i++) {
        bcm_tsn_stat_t stat_itr = stat_arr[i];

        if (stat_itr < 0 || stat_itr >= bcmTsnStatCount) {
            return BCM_E_PARAM;
        }
        if (!soc_feature(unit, tsn_stat_type_feature[stat_itr])) {
            return BCM_E_UNAVAIL;
        }
    }

    /* decode */
    BCM_IF_ERROR_RETURN(
        TSN_STAT_FLOWCNT_FUNC(group_id_decode,
            (unit, stat_id, &group_type, &group_id)));
    BCM_IF_ERROR_RETURN(
        TSN_STAT_FLOWCNT_FUNC(group_enable_value_encode,
            (unit, group_type, stat_arr, count, &new_enable_values)));

    TSN_STAT_FLOWCNT_LOCK(fc);
    /* check group id has been just created and
     * not attached on any flow yet
     */
    rv = TSN_STAT_FLOWCNT_FUNC(group_refcnt_get,
             (unit, group_type, group_id, &group_refcnt));
    if (BCM_SUCCESS(rv)) {
        if (0 == group_refcnt) {
            rv = BCM_E_PARAM;
        } else if (1 != group_refcnt) {
            rv = BCM_E_BUSY;
        }
    }

    /* check the new setting has already existed or not */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_STAT_FLOWCNT_FUNC(group_enable_value_exist_check,
                 (unit, group_type, new_enable_values));
    }

    /* write new value */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_STAT_FLOWCNT_FUNC(group_enable_value_set,
                 (unit, group_type, group_id, new_enable_values));
    }
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_group_get
 * Description:
 *      Get configuration of the TSN stat group
 * Parameters:
 *      unit             - (IN) unit number
 *      stat_id          - (IN) TSN statistic group id
 *      group_type       - (IN) TSN statistic group type
 *      max              - (OUT) the size of stat_arr
 *      stat_arr         - (OUT) array for returned TSN stat types
 *      count            - (OUT) actual number of TSN stat types
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_group_get(
    int unit,
    bcm_tsn_stat_group_t stat_id,
    bcm_tsn_stat_group_type_t *ret_group_type,
    int max,
    bcm_tsn_stat_t *stat_arr,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_tsn_stat_group_type_t group_type;
    uint32 group_id;
    int group_refcnt;
    uint32 enable_values;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    if (NULL == ret_group_type) {
        return BCM_E_PARAM;
    }

    /* decode stat_id to  group_type, group_id */
    BCM_IF_ERROR_RETURN(
        TSN_STAT_FLOWCNT_FUNC(group_id_decode,
            (unit, stat_id, &group_type, &group_id)));

    TSN_STAT_FLOWCNT_LOCK(fc);
    /* and check group id has been created or not */
    rv = TSN_STAT_FLOWCNT_FUNC(group_refcnt_get,
             (unit, group_type, group_id, &group_refcnt));
    if (BCM_SUCCESS(rv)) {
        if (0 == group_refcnt) {
            rv = BCM_E_PARAM;
        }
    }
    /* get value */
    if (BCM_SUCCESS(rv)) {
        rv = TSN_STAT_FLOWCNT_FUNC(group_enable_value_get,
                 (unit, group_type, group_id, &enable_values));
    }
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    BCM_IF_ERROR_RETURN(rv);
    BCM_IF_ERROR_RETURN(
        TSN_STAT_FLOWCNT_FUNC(group_enable_value_decode,
            (unit, group_type, enable_values, stat_arr, max, count)));
    *ret_group_type = group_type;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_group_destroy
 * Description:
 *      Destroy a TSN stat group
 * Parameters:
 *      unit             - (IN) unit number
 *      stat_id          - (IN) TSN statistic group id
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_group_destroy(
    int unit,
    bcm_tsn_stat_group_t stat_id)
{
    int rv = BCM_E_NONE;
    bcm_tsn_stat_group_type_t group_type;
    uint32 group_id;
    int group_refcnt;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));


    /* decode */
    BCM_IF_ERROR_RETURN(
        TSN_STAT_FLOWCNT_FUNC(group_id_decode,
            (unit, stat_id, &group_type, &group_id)));

    TSN_STAT_FLOWCNT_LOCK(fc);
    /* check group id has been created and not attached on any flow */
    rv = TSN_STAT_FLOWCNT_FUNC(group_refcnt_get,
             (unit, group_type, group_id, &group_refcnt));
    if (BCM_SUCCESS(rv)) {
        if (0 == group_refcnt) {
            rv = BCM_E_PARAM;
        } else if (1 != group_refcnt) {
            rv = BCM_E_BUSY;
        }
    }
    if (BCM_SUCCESS(rv)) {
        rv = TSN_STAT_FLOWCNT_FUNC(group_enable_value_delete,
                 (unit, group_type, group_id));
    }
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_group_traverse
 * Description:
 *      Traverse TSN stat groups
 * Parameters:
 *      unit             - (IN) unit number
 *      cb               - (IN) User provided call back function
 *      user_data        - (IN) User provided data
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_group_traverse(
    int unit,
    bcm_tsn_stat_group_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    TSN_STAT_FLOWCNT_LOCK(fc);
    rv = TSN_STAT_FLOWCNT_FUNC(group_traverse, (unit, cb, user_data));
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_flow_get
 * Description:
 *      Get the stat group for this flow with specified group type
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      group_type       - (IN) TSN statistic group type
 *      stat_group       - (OUT)TSN statistic group
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_flow_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_stat_group_type_t group_type,
    bcm_tsn_stat_group_t *stat_id)
{
    uint32 group_id;
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    if (NULL == stat_id) {
        return BCM_E_PARAM;
    }

    TSN_STAT_FLOWCNT_LOCK(fc);
    rv = TSN_STAT_FLOWCNT_FUNC(flow_get,
             (unit, flow_id, group_type, &group_id));
    TSN_STAT_FLOWCNT_UNLOCK(fc);
    BCM_IF_ERROR_RETURN(rv);

    if (BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED == group_id) {
        *stat_id = BCM_TSN_STAT_GROUP_INVALID;
    } else {
        BCM_IF_ERROR_RETURN(
            TSN_STAT_FLOWCNT_FUNC(group_id_encode,
                (unit, group_type, group_id, stat_id)));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_flow_set
 * Description:
 *      Attach ingress/egress stat group to this flow and enable counting for
 *      the stat group. To disable counting for a specific group type, set
 *      BCM_TSN_STAT_GROUP_INVALID as stat_group
 * Parameters:
 *      unit             - (IN) unit number
 *      flow             - (IN) SR flow
 *      group_type       - (IN) TSN statistic group type
 *      stat_group       - (IN) TSN statistic group
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_flow_set(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    bcm_tsn_stat_group_type_t group_type,
    bcm_tsn_stat_group_t new_stat_id)
{
    int rv = BCM_E_NONE;
    uint32 new_group_id;
    int new_group_refcnt;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    /* parameter check if the user want to switch to the new group id */
    if (BCM_TSN_STAT_GROUP_INVALID != new_stat_id) {
        bcm_tsn_stat_group_type_t new_group_type;

        /* new group id should have the right group type */
        BCM_IF_ERROR_RETURN(
            TSN_STAT_FLOWCNT_FUNC(group_id_decode,
                (unit, new_stat_id, &new_group_type, &new_group_id)));

        if (new_group_type != group_type) {
            BCM_IF_ERROR_RETURN(BCM_E_PARAM);
        }
    } else {
        new_group_id = BCMI_TSN_FLOWCNT_GROUP_ID_RESERVED;
    }

    TSN_STAT_FLOWCNT_LOCK(fc);
    /* check new group id has been created or not */
    if (BCM_TSN_STAT_GROUP_INVALID != new_stat_id) {
        rv = TSN_STAT_FLOWCNT_FUNC(group_refcnt_get,
                 (unit, group_type, new_group_id, &new_group_refcnt));
        if (BCM_SUCCESS(rv)) {
            if (0 == new_group_refcnt) {
                rv = BCM_E_PARAM;
            }
        }
    }
    if (BCM_SUCCESS(rv)) {
        rv = TSN_STAT_FLOWCNT_FUNC(flow_set,
                 (unit, flow_id, group_type, new_group_id));
    }
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_set
 * Description:
 *      Helper function for flow counter set
 * Parameters:
 *      unit             - (IN) unit number
 *      flow_id          - (IN) SR flow
 *      arr_cnt          - (IN) size of stat_arr and value_arr
 *      stat_type_arr    - (IN) TSN stat types
 *      stat_value_arr   - (IN) TSN stat values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_set(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    uint32 arr_cnt,
    bcm_tsn_stat_t *stat_type_arr,
    uint64 *stat_value_arr)
{
    int i, rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    /* check feature support or not */
    for (i = 0; i < arr_cnt; i++) {
        bcm_tsn_stat_t stat_itr = stat_type_arr[i];

        if (stat_itr < 0 || stat_itr >= bcmTsnStatCount) {
            return BCM_E_PARAM;
        }
        if (!soc_feature(unit, tsn_stat_type_feature[stat_itr])) {
            return BCM_E_UNAVAIL;
        }
    }

    TSN_STAT_FLOWCNT_LOCK(fc);
    /* for write, write both sw and hw value */
    rv = TSN_STAT_FLOWCNT_FUNC(counter_access,
                               (unit, flow_id, bcmiTsnStatCounterActionWrite,
                                arr_cnt, stat_type_arr,
                                stat_value_arr, stat_value_arr));
    TSN_STAT_FLOWCNT_UNLOCK(fc);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_get
 * Description:
 *      Helper function for flow counter get
 * Parameters:
 *      unit             - (IN) unit number
 *      flow_id          - (IN) SR flow
 *      sync_mode        - (IN) if 1 read hw counter else sw accumulated counter
 *      arr_cnt          - (IN) size of stat_arr and value_arr
 *      stat_type_arr    - (IN) TSN stat types
 *      stat_value_arr   - (IN) TSN stat values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_get(
    int unit,
    bcm_tsn_sr_flow_t flow_id,
    int sync_mode,
    uint32 arr_cnt,
    bcm_tsn_stat_t *stat_type_arr,
    uint64 *stat_value_arr)
{
    int i, rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    /* check feature support or not */
    for (i = 0; i < arr_cnt; i++) {
        bcm_tsn_stat_t stat_itr = stat_type_arr[i];

        if (stat_itr < 0 || stat_itr >= bcmTsnStatCount) {
            return BCM_E_PARAM;
        }
        if (!soc_feature(unit, tsn_stat_type_feature[stat_itr])) {
            return BCM_E_UNAVAIL;
        }
    }

    TSN_STAT_FLOWCNT_LOCK(fc);
    /* for read, just need to read value from sw database
     * (maybe need to sync at first)
     */
    rv = TSN_STAT_FLOWCNT_FUNC(counter_access,
                               (unit, flow_id,
                                (sync_mode ?
                                 bcmiTsnStatCounterActionReadSync :
                                 bcmiTsnStatCounterActionRead),
                                arr_cnt, stat_type_arr, stat_value_arr, NULL));
    TSN_STAT_FLOWCNT_UNLOCK(fc);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_fbmp_get
 * Description:
 *      given a stat type, return all hw flow_id(s) been attached on it
 * Parameters:
 *      unit             - (IN) unit number
 *      stat             - (IN) TSN stat type
 *      fbmp             - (OUT)bitmap for return hw flow_id(s)
 *      fbmp_size        - (IN) size of fbmp
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_fbmp_get(
    int unit,
    bcm_tsn_stat_t stat,
    SHR_BITDCL  *fbmp,
    uint32 fbmp_size)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    if (stat < 0 || stat >= bcmTsnStatCount) {
        return BCM_E_PARAM;
    }

    if (!soc_feature(unit, tsn_stat_type_feature[stat])) {
        return BCM_E_UNAVAIL;
    }

    TSN_STAT_FLOWCNT_LOCK(fc);
    rv = TSN_STAT_FLOWCNT_FUNC(flow_get_fbmp, (unit, stat, fbmp, fbmp_size));
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    return rv;
}


/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_hw_sw_get
 * Description:
 *      Helper function for flow counter get both sw/hw value
 * Parameters:
 *      unit             - (IN) unit number
 *      stat             - (IN) TSN stat type
 *      flow_id          - (IN) SR flow
 *      hw_value         - (OUT) hw value
 *      sw_value         - (OUT) sw value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_hw_sw_get(
    int unit,
    bcm_tsn_stat_t stat,
    bcm_tsn_sr_flow_t flow_id,
    uint64 *hw_value,
    uint64 *sw_value)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    if (stat < 0 || stat >= bcmTsnStatCount) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, tsn_stat_type_feature[stat])) {
        return BCM_E_UNAVAIL;
    }

    TSN_STAT_FLOWCNT_LOCK(fc);
    rv = TSN_STAT_FLOWCNT_FUNC(counter_access,
                               (unit, flow_id, bcmiTsnStatCounterActionReadSync,
                                1, &stat, sw_value, hw_value));
    TSN_STAT_FLOWCNT_UNLOCK(fc);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_tsn_stat_flowcnt_hw_set
 * Description:
 *      Helper function for flow counter set hw value
 * Parameters:
 *      unit             - (IN) unit number
 *      stat             - (IN) TSN stat type
 *      flow_id          - (IN) SR flow
 *      hw_value         - (IN) hw value
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_esw_tsn_stat_flowcnt_hw_set(
    int unit,
    bcm_tsn_stat_t stat,
    bcm_tsn_sr_flow_t flow_id,
    uint64 hw_value)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    if (stat < 0 || stat >= bcmTsnStatCount) {
        return BCM_E_PARAM;
    }
    if (!soc_feature(unit, tsn_stat_type_feature[stat])) {
        return BCM_E_UNAVAIL;
    }

    TSN_STAT_FLOWCNT_LOCK(fc);
    rv = TSN_STAT_FLOWCNT_FUNC(counter_access,
                               (unit, flow_id, bcmiTsnStatCounterActionWrite,
                                1, &stat, NULL, &hw_value));
    TSN_STAT_FLOWCNT_UNLOCK(fc);
    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_flowcnt_stat_sync
 * Description:
 *    Helper function to sync HW/SW TSN stat flowcnt per specified stat type.
 * Parameters:
 *    unit - (IN) Unit number
 *    stat - (IN) TSN statistics type (see tsn.h)
 * Returns:
 *    BCM_E_XXX.
 * Notes:
 */
int bcmi_esw_tsn_stat_flowcnt_stat_sync(int unit, bcm_tsn_stat_t stat)
{
    int rv = BCM_E_NONE;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    if (stat < 0 || stat >= bcmTsnStatCount) {
        return BCM_E_PARAM;
    }

    if (!soc_feature(unit, tsn_stat_type_feature[stat])) {
        return BCM_E_UNAVAIL;
    }

    TSN_STAT_FLOWCNT_LOCK(fc);
    rv = TSN_STAT_FLOWCNT_FUNC(counter_sync_stat, (unit, stat));
    TSN_STAT_FLOWCNT_UNLOCK(fc);

    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_flowcnt_cleanup
 * Description:
 *    Clean and free all allocated TSN flow counter resources
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_stat_flowcnt_cleanup(int unit)
{
    bcmi_tsn_stat_flowcnt_control_t *fc;

    LOG_DEBUG(BSL_LS_BCM_TSN,
              (BSL_META_U(unit,
                          "bcmi_esw_tsn_stat_flowcnt_cleanup\n")));

    fc = tsn_stat_control[unit]->flowcnt_control;
    if (NULL != fc->flowcnt_mutex) {
        sal_mutex_destroy(fc->flowcnt_mutex);
        fc->flowcnt_mutex = NULL;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *    bcmi_esw_tsn_stat_flowcnt_init
 * Description:
 *    Initialize and allocate all required resources for TSN flow counters
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_stat_flowcnt_init(int unit)
{
    bcmi_tsn_stat_control_t *tc;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    /* Get TSN stat control */
    BCM_IF_ERROR_RETURN(
        tsn_stat_control_instance_get(unit, &tc));

    /* Set up the unit flowcnt control structure. */
    fc = tc->flowcnt_control;
    if (NULL == fc) {
        return BCM_E_INIT;
    }
    sal_memset(fc, 0, sizeof(bcmi_tsn_stat_flowcnt_control_t));

    /* Create flowcnt protection mutex. */
    fc->flowcnt_mutex = sal_mutex_create("tsn flowcnt control mutex");
    if (NULL == fc->flowcnt_mutex) {
        return BCM_E_RESOURCE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_flowcnt_update
 * Description:
 *    For SER module to update the SW & HW counter
 *    with given memory and memory index
 * Parameters:
 *    unit - (IN) Unit number
 *    mem -  (IN) TSN stat counter memory
 *    mem_idx - (IN) memory index
 *    new_count - (IN) new counter value
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_stat_flowcnt_update(
    int unit,
    soc_mem_t mem,
    int mem_idx,
    uint64 new_count)
{
    bcm_error_t rv = BCM_E_NOT_FOUND;
    bcmi_tsn_stat_flowcnt_control_t *fc;
    int total_entries;

    /* Input parameter check. */
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return BCM_E_PARAM;
    }

    total_entries = soc_mem_index_count(unit, mem);
    if (mem_idx >= total_entries) {
        LOG_ERROR(BSL_LS_BCM_TSN,
                  (BSL_META_U(unit,
                              "Wrong OFFSET_INDEX(Flow). "
                              "Must be < Total Entries %d \n"),
                  total_entries));
        assert(0);
    }
    BCM_IF_ERROR_RETURN(
        tsn_stat_flowcnt_control_instance_get(unit, &fc));

    TSN_STAT_FLOWCNT_LOCK(fc);
    /* for write, write both sw and hw value */
    rv = TSN_STAT_FLOWCNT_FUNC(counter_update,
                               (unit, mem, mem_idx, new_count));
    TSN_STAT_FLOWCNT_UNLOCK(fc);
    return rv;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_counter_update
 * Description:
 *    For SER module to update the SW & HW counter
 *    with given memory and memory index
 * Parameters:
 *    unit - (IN) Unit number
 *    mem -  (IN) TSN stat counter memory
 *    mem_idx - (IN) memory index
 *    new_count - (IN) new counter value
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_stat_counter_update(
    int unit,
    soc_mem_t mem,
    int mem_idx,
    uint64 new_count)
{
    bcm_error_t rv1 = BCM_E_NOT_FOUND, rv2 = BCM_E_NOT_FOUND;

    /* Check if it's sr flow memory, update flow cnt if it is */
    rv1 = bcmi_esw_tsn_stat_flowcnt_update(
            unit, mem, mem_idx, new_count);
    if (BCM_SUCCESS(rv1)) {
        return rv1;
    }

    /* Check if it's sr port memory, update port cnt if it is */
    rv2 = bcmi_esw_tsn_stat_portcnt_update(
            unit, mem, mem_idx, new_count);
    if (BCM_SUCCESS(rv2)) {
        return rv2;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_counter_cb
 * Description:
 *    Callback function for counter collection
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
STATIC void
bcmi_esw_tsn_stat_counter_cb(int unit)
{
    bcmi_tsn_stat_portcnt_control_t *pc;
    bcmi_tsn_stat_flowcnt_control_t *fc;

    (void)tsn_stat_portcnt_control_instance_get(unit, &pc);
    (void)tsn_stat_flowcnt_control_instance_get(unit, &fc);
    if (NULL == pc || NULL == fc) {
        return;
    }

    /* TSN stat portcnt callback */
    TSN_STAT_PORTCNT_LOCK(pc);
    bcmi_esw_tsn_stat_portcnt_collect(unit, -1, -1);
    TSN_STAT_PORTCNT_UNLOCK(pc);
    /* TSN stat flowcnt callback */
    TSN_STAT_FLOWCNT_LOCK(fc);
    (void)TSN_STAT_FLOWCNT_FUNC(counter_sync_all, (unit));
    TSN_STAT_FLOWCNT_UNLOCK(fc);
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_threshold_init
 * Description:
 *    Initialize for TSN stat counter threshold
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
bcmi_esw_tsn_stat_threshold_init(int unit)
{
    bcmi_tsn_stat_control_t *tc;
    const bcmi_esw_tsn_stat_dev_info_t *stat_dev_info;
    const tsn_stat_threshold_ctrl_info_t *threshold_ctrl_info;

    BCM_IF_ERROR_RETURN(
        tsn_stat_control_instance_get(unit, &tc));
    stat_dev_info = tc->tsn_stat_dev_info;

    threshold_ctrl_info = stat_dev_info->tsn_stat_threshold_ctrl_info;
    if ((threshold_ctrl_info != NULL) &&
        (threshold_ctrl_info->tsn_stat_threshold_ctrl_init != NULL)) {
        return threshold_ctrl_info->tsn_stat_threshold_ctrl_init(unit);
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_cleanup
 * Description:
 *    Clean and free all allocated TSN stat counter resources
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_stat_cleanup(int unit)
{
    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_stat_cleanup\n")));


    soc_counter_extra_unregister(unit, bcmi_esw_tsn_stat_counter_cb);

    if (tsn_stat_control[unit] != NULL) {
        if (tsn_stat_control[unit]->portcnt_control != NULL) {
            bcmi_esw_tsn_stat_portcnt_cleanup(unit);
            sal_free(tsn_stat_control[unit]->portcnt_control);
            tsn_stat_control[unit]->portcnt_control = NULL;
        }
        if (tsn_stat_control[unit]->flowcnt_control != NULL) {
            bcmi_esw_tsn_stat_flowcnt_cleanup(unit);
            sal_free(tsn_stat_control[unit]->flowcnt_control);
            tsn_stat_control[unit]->flowcnt_control = NULL;
        }
        if (tsn_stat_control[unit]->tsn_stat_dev_info != NULL) {
            TSN_STAT_FLOWCNT_FUNC(cleanup, (unit));
        }
        sal_free(tsn_stat_control[unit]);
        tsn_stat_control[unit] = NULL;
    }

    ser_tsn_stat_functions._soc_tsn_stat_counter_update_f = NULL;
    soc_ser_tsn_stat_function_register(unit, &ser_tsn_stat_functions);

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_init
 * Description:
 *    Initialize and allocate all required resources for TSN stat counters
 * Parameters:
 *    unit - (IN) Unit number
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_stat_init(int unit)
{
    int rv = BCM_E_NONE;
    int i;
    bcmi_tsn_stat_control_t *tc;

    LOG_VERBOSE(BSL_LS_BCM_TSN,
                (BSL_META_U(unit,
                            "bcmi_esw_tsn_stat_init\n")));

    /* Global initialization flag setup */
    if (tsn_stat_initialized == 0) {
        for (i = 0; i < BCM_MAX_NUM_UNITS; i++) {
            tsn_stat_control[i] = NULL;
        }
        tsn_stat_initialized = 1;
    }

    /* Clean up the TSN stat control related resources */
    if (tsn_stat_control[unit] != NULL) {
        bcmi_esw_tsn_stat_cleanup(unit);
    }

    /* Allocate TSN stat control resource for this unit. */
    tsn_stat_control[unit] = sal_alloc(sizeof(bcmi_tsn_stat_control_t),
                                       "tsn stat control");
    tc = tsn_stat_control[unit];
    if (NULL == tc) {
        return BCM_E_MEMORY;
    }
    sal_memset(tc, 0, sizeof(bcmi_tsn_stat_control_t));


    /* Get the chip tsn_stat_dev_info structure. */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        rv = bcmi_gh2_tsn_stat_info_init(unit, &tc->tsn_stat_dev_info);
        if (BCM_SUCCESS(rv) && NULL == tc->tsn_stat_dev_info) {
            rv = BCM_E_INIT;
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    /* Release resource if any error occurred. */
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return rv;
    }
    /* Check if tsn_stat_dev_info is NULL */
    if (NULL == tc->tsn_stat_dev_info) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return BCM_E_UNAVAIL;
    }

    /* Initialize TSN Stat portcnt */
    tsn_stat_control[unit]->portcnt_control
        = sal_alloc(sizeof(bcmi_tsn_stat_portcnt_control_t),
                    "tsn portcnt control");
    if (NULL == tsn_stat_control[unit]->portcnt_control) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return BCM_E_MEMORY;
    }
    rv = bcmi_esw_tsn_stat_portcnt_init(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return rv;
    }

    /* Initialize TSN Stat flowtcnt */
    tsn_stat_control[unit]->flowcnt_control
        = sal_alloc(sizeof(bcmi_tsn_stat_flowcnt_control_t),
                    "tsn flowcnt control");
    if (NULL == tsn_stat_control[unit]->flowcnt_control) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return BCM_E_MEMORY;
    }
    rv = bcmi_esw_tsn_stat_flowcnt_init(unit);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return rv;
    }

    /* Initialize chip-specific data */
    rv = TSN_STAT_FLOWCNT_FUNC(init, (unit));
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return rv;
    }

    /* Register callback function for TSN stat counter */
    rv = soc_counter_extra_register(unit, bcmi_esw_tsn_stat_counter_cb);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return rv;
    }

    /* Register callback function for SER TSN stat counter */
    ser_tsn_stat_functions._soc_tsn_stat_counter_update_f =
        &bcmi_esw_tsn_stat_counter_update;
    rv = soc_ser_tsn_stat_function_register(unit, &ser_tsn_stat_functions);
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return rv;
    }

    /* Initialize TSN Stat counter threshold */
    rv = bcmi_esw_tsn_stat_threshold_init(unit);
    /* Release resource if any error occurred. */
    if (BCM_FAILURE(rv)) {
        bcmi_esw_tsn_stat_cleanup(unit);
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_threshold_set
 * Purpose:
 *    Configure threshold for a specific stat type on
 *    a specific source
 * Parameters:
 *    unit   - (IN) Unit number
 *    source - (IN) Stat source for TSN stat threshold check
 *    stat   - (IN) TSN statistics type (see tsn.h)
 *    config - (IN) TSN stat threshold configuration structure
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_stat_threshold_set(
    int unit,
    bcm_tsn_stat_threshold_source_t source,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config)
{
    bcmi_tsn_stat_control_t *tc;
    const bcmi_esw_tsn_stat_dev_info_t *stat_dev_info;
    const tsn_stat_threshold_ctrl_info_t *threshold_ctrl_info;

    BCM_IF_ERROR_RETURN(
        tsn_stat_control_instance_get(unit, &tc));
    stat_dev_info = tc->tsn_stat_dev_info;

    threshold_ctrl_info = stat_dev_info->tsn_stat_threshold_ctrl_info;
    if ((threshold_ctrl_info != NULL) &&
        (threshold_ctrl_info->tsn_stat_threshold_ctrl_set != NULL)) {
        BCM_IF_ERROR_RETURN(
            threshold_ctrl_info->tsn_stat_threshold_ctrl_set(unit,
                                                             source,
                                                             stat,
                                                             config));

        /* Enable stat matrix (all ports) for syncing the counter */
        return bcmi_esw_tsn_stat_portcnt_used_update
                      (unit, -1, stat, BCM_TSN_STAT_PORTCNT_USED_SET);
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    bcmi_esw_tsn_stat_threshold_get
 * Purpose:
 *    Get the threshold configuration for a specific stat type on
 *    a specific source
 * Parameters:
 *    unit   - (IN) Unit number
 *    source - (IN) Stat source for TSN stat threshold check
 *    stat   - (IN) TSN statistics type (see tsn.h)
 *    config - (OUT) TSN stat threshold configuration structure
 * Returns:
 *    BCM_E_XXX
 */
int
bcmi_esw_tsn_stat_threshold_get(
    int unit,
    bcm_tsn_stat_threshold_source_t source,
    bcm_tsn_stat_t stat,
    bcm_tsn_stat_threshold_config_t *config)
{
    bcmi_tsn_stat_control_t *tc;
    const bcmi_esw_tsn_stat_dev_info_t *stat_dev_info;
    const tsn_stat_threshold_ctrl_info_t *threshold_ctrl_info;

    BCM_IF_ERROR_RETURN(
        tsn_stat_control_instance_get(unit, &tc));
    stat_dev_info = tc->tsn_stat_dev_info;

    threshold_ctrl_info = stat_dev_info->tsn_stat_threshold_ctrl_info;
    if ((threshold_ctrl_info != NULL) &&
        (threshold_ctrl_info->tsn_stat_threshold_ctrl_get != NULL)) {
        return threshold_ctrl_info->tsn_stat_threshold_ctrl_get(unit,
                                                                source,
                                                                stat,
                                                                config);
    }

    return BCM_E_UNAVAIL;
}

#endif /* BCM_TSN_SUPPORT */

