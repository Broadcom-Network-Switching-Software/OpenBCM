/*! \file multicast.c
 *
 * Multicast Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/multicast.h>

#include <bcm_int/control.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/multicast_int.h>
#include <bcm_int/ltsw/mbcm/multicast.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/trunk.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/generated/multicast_ha.h>
#include <bcm_int/ltsw/index_table_mgmt.h>

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>


/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_MULTICAST

/*
 * Multicast information.
 */
typedef struct ltsw_mc_info_s {

    /* Multicast module initialized. */
    int                             inited;

    /* L2 multicast mutex lock. */
    sal_mutex_t                     l2mc_mutex;

    /* L2 multicast table minimum index. */
    int                             l2mc_min;

    /* L2 multicast table maximum index. */
    int                             l2mc_max;

    /* L2 multicast group info. */
    bcmint_multicast_l2_grp_info_t *l2mc_grp;

    /* L3 multicast mutex lock. */
    sal_mutex_t                     l3mc_mutex;

    /* L3 multicast table minimum index. */
    int                             l3mc_min;

    /* L3 multicast table maximum index. */
    int                             l3mc_max;

    /* L3 multicast group info. */
    bcmint_multicast_l3_grp_info_t *l3mc_grp;

    /* L3 encap id info. */
    bcmint_multicast_encap_info_t  *encap_id;

    /* L3 encap id size. */
    int                             encap_sz;

    /* L2 multicast table minimum index for VLAN flood. */
    int                             l2mc_vlan_flood_min;

    /* L2 multicast table maximum index for VLAN flood. */
    int                             l2mc_vlan_flood_max;

} ltsw_mc_info_t;

/* Static global variable of multicast info. */
static ltsw_mc_info_t ltsw_mc_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* Multicast information. */
#define MC_INFO(u)        (&ltsw_mc_info[u])

/* Check if MULTICAST module is initialized */
#define MC_INIT_CHECK(u)                                \
    do {                                                \
        ltsw_mc_info_t *mc_i = MC_INFO(u);              \
        if (mc_i->inited == false) {                    \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);        \
        }                                               \
    } while(0)

/* L2MC Lock. */
#define L2MC_LOCK(u) \
        sal_mutex_take(MC_INFO(u)->l2mc_mutex, SAL_MUTEX_FOREVER)

#define L2MC_UNLOCK(u) \
        sal_mutex_give(MC_INFO(u)->l2mc_mutex);

/* L3MC Lock. */
#define L3MC_LOCK(u) \
        sal_mutex_take(MC_INFO(u)->l3mc_mutex, SAL_MUTEX_FOREVER)

#define L3MC_UNLOCK(u) \
        sal_mutex_give(MC_INFO(u)->l3mc_mutex);

/* L2 multicast group table minimum index. */
#define L2MC_MIN(u)  (MC_INFO(u)->l2mc_min)
/* L2 multicast group table maximum index. */
#define L2MC_MAX(u)  (MC_INFO(u)->l2mc_max)

/* L2 multicast group table minimum index for VLAN flooding. */
#define L2MC_VF_MIN(u)  (MC_INFO(u)->l2mc_vlan_flood_min)
/* L2 multicast group table maximum index for VLAN flooding. */
#define L2MC_VF_MAX(u)  (MC_INFO(u)->l2mc_vlan_flood_max)

/* L3 multicast group table minimum index. */
#define L3MC_MIN(u)  (MC_INFO(u)->l3mc_min)
/* L3 multicast group table maximum index. */
#define L3MC_MAX(u)  (MC_INFO(u)->l3mc_max)


/* L2MC Macros for used check. */
#define L2MC_USED_REFCNT(u, idx)   (MC_INFO(u)->l2mc_grp[idx].ref_cnt)
#define L2MC_USED_SET(u, idx)      (L2MC_USED_REFCNT(u, idx) += 1)
#define L2MC_USED_CLR(u, idx)      (L2MC_USED_REFCNT(u, idx) -= 1)
#define L2MC_USED_ISSET(u, idx)    (L2MC_USED_REFCNT(u, idx) > 0)

/* L3MC Macros for used check. */
#define L3MC_USED_REFCNT(u, idx)   (MC_INFO(u)->l3mc_grp[idx].ref_cnt)
#define L3MC_USED_SET(u, idx)      (L3MC_USED_REFCNT(u, idx) += 1)
#define L3MC_USED_CLR(u, idx)      (L3MC_USED_REFCNT(u, idx) -= 1)
#define L3MC_USED_ISSET(u, idx)    (L3MC_USED_REFCNT(u, idx) > 0)
#define L3MC_TYPE(u, idx)          (MC_INFO(u)->l3mc_grp[idx].type)

/* L3 Encap Macros for used check. */
#define ENCAP_USED_SET(u, idx)     (MC_INFO(u)->encap_id[idx].set = 1)
#define ENCAP_USED_CLR(u, idx)     (MC_INFO(u)->encap_id[idx].set = 0)
#define ENCAP_USED_ISSET(u, idx)   (MC_INFO(u)->encap_id[idx].set == 1)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Clean up Multicast module software database.
 *
 * \param [in]  unit          Unit number.
 */
static void
mc_info_cleanup(int unit)
{
    ltsw_mc_info_t *mc_i = MC_INFO(unit);

    if (mc_i->l2mc_mutex) {
        sal_mutex_destroy(mc_i->l2mc_mutex);
        mc_i->l2mc_mutex = NULL;
    }

    if (mc_i->l3mc_mutex) {
        sal_mutex_destroy(mc_i->l3mc_mutex);
        mc_i->l3mc_mutex = NULL;
    }

    return;
}

/*!
 * \brief Initialize Multicast module software database.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
mc_info_init(int unit)
{
    ltsw_mc_info_t *mc_i = MC_INFO(unit);
    bcmint_multicast_pub_info_t mc_pub;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    int warm;
    int rv;

    SHR_FUNC_ENTER(unit);

    warm = bcmi_warmboot_get(unit);
    if (mc_i->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_multicast_detach(unit));
    }

    if (mc_i->l2mc_mutex == NULL) {
        mc_i->l2mc_mutex = sal_mutex_create("mcL2InfoMutex");
        SHR_NULL_CHECK(mc_i->l2mc_mutex, SHR_E_MEMORY);
    }

    if (mc_i->l3mc_mutex == NULL) {
        mc_i->l3mc_mutex = sal_mutex_create("mcL3InfoMutex");
        SHR_NULL_CHECK(mc_i->l3mc_mutex, SHR_E_MEMORY);
    }

    /* Initialize public table size. */
    sal_memset(&mc_pub, 0, sizeof(mc_pub));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_pub_info_get(unit, &mc_pub));
    mc_i->l2mc_min = mc_pub.l2mc_min;
    mc_i->l2mc_max = mc_pub.l2mc_max;
    mc_i->l3mc_min = mc_pub.l3mc_min;
    mc_i->l3mc_max = mc_pub.l3mc_max;
    mc_i->encap_sz = mc_pub.encap_sz;
    mc_i->l2mc_vlan_flood_min = mc_pub.l2mc_vlan_flood_min;
    mc_i->l2mc_vlan_flood_max = mc_pub.l2mc_vlan_flood_max;

    /*
     * Initialize L2 mc group info.
     * l2mc_vlan_flood_max represents the real maximum when it is valid.
     */
    if (mc_i->l2mc_vlan_flood_max == 0) {
        ha_array_size = mc_i->l2mc_max + 1;
    } else {
        ha_array_size = mc_pub.l2mc_vlan_flood_max + 1;
    }
    ha_instance_size = sizeof(bcmint_multicast_l2_grp_info_t);
    ha_req_size = ha_array_size * ha_instance_size;
    ha_alloc_size = ha_req_size;
    mc_i->l2mc_grp = bcmi_ltsw_ha_mem_alloc(unit,
                                            BCMI_HA_COMP_ID_MULTICAST,
                                            BCMINT_MULTICAST_L2_GRP_SUB_COMP_ID,
                                            "bcmMultiCastL2Grp",
                                            &ha_alloc_size);
    SHR_NULL_CHECK(mc_i->l2mc_grp, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(mc_i->l2mc_grp, 0, ha_alloc_size);
    }
    rv = bcmi_ltsw_issu_struct_info_report(unit,
              BCMI_HA_COMP_ID_MULTICAST,
              BCMINT_MULTICAST_L2_GRP_SUB_COMP_ID,
              0, ha_instance_size, ha_array_size,
              BCMINT_MULTICAST_L2_GRP_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Initialize L3 mc group info. */
    ha_array_size = mc_i->l3mc_max + 1;
    ha_instance_size = sizeof(bcmint_multicast_l3_grp_info_t);
    ha_req_size = ha_array_size * ha_instance_size;
    ha_alloc_size = ha_req_size;
    mc_i->l3mc_grp = bcmi_ltsw_ha_mem_alloc(unit,
                                            BCMI_HA_COMP_ID_MULTICAST,
                                            BCMINT_MULTICAST_L3_GRP_SUB_COMP_ID,
                                            "bcmMultiCastL3Grp",
                                            &ha_alloc_size);
    SHR_NULL_CHECK(mc_i->l3mc_grp, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                            SHR_E_MEMORY : SHR_E_NONE);
    if (!warm) {
        sal_memset(mc_i->l3mc_grp, 0, ha_alloc_size);
    }
    rv = bcmi_ltsw_issu_struct_info_report(unit,
              BCMI_HA_COMP_ID_MULTICAST,
              BCMINT_MULTICAST_L3_GRP_SUB_COMP_ID,
              0, ha_instance_size, ha_array_size,
              BCMINT_MULTICAST_L3_GRP_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Initialize L3 encap id info. */
    ha_array_size = mc_i->encap_sz;
    ha_instance_size = sizeof(bcmint_multicast_encap_info_t);
    ha_req_size = ha_array_size * ha_instance_size;
    if (ha_req_size) {
        ha_alloc_size = ha_req_size;
        mc_i->encap_id = bcmi_ltsw_ha_mem_alloc(unit,
                                                BCMI_HA_COMP_ID_MULTICAST,
                                                BCMINT_MULTICAST_ENCAP_SUB_COMP_ID,
                                                "bcmMultiCastEncap",
                                                &ha_alloc_size);
        SHR_NULL_CHECK(mc_i->encap_id, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(mc_i->encap_id, 0, ha_alloc_size);
        }
    }
    rv = bcmi_ltsw_issu_struct_info_report(unit,
              BCMI_HA_COMP_ID_MULTICAST,
              BCMINT_MULTICAST_ENCAP_SUB_COMP_ID,
              0, ha_instance_size, ha_array_size,
              BCMINT_MULTICAST_ENCAP_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
       SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

exit:
    if (SHR_FUNC_ERR()) {
        mc_info_cleanup(unit);
        if (!warm && mc_i->l2mc_grp) {
            (void)bcmi_ltsw_ha_mem_free(unit, mc_i->l2mc_grp);
        }
        if (!warm && mc_i->l3mc_grp) {
            (void)bcmi_ltsw_ha_mem_free(unit, mc_i->l3mc_grp);
        }
        if (!warm && mc_i->encap_id) {
            (void)bcmi_ltsw_ha_mem_free(unit, mc_i->encap_id);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover Multicast module software database.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
mc_info_recover(int unit)
{
    int warm = 0;
    int i;

    SHR_FUNC_ENTER(unit);

    /* Do warmboot recover. */
    warm = bcmi_warmboot_get(unit);
    if (warm) {
        for (i = 0; i < MC_INFO(unit)->encap_sz; i++) {
            if (ENCAP_USED_ISSET(unit, i)) {
                SHR_IF_ERR_EXIT
                    (mbcm_ltsw_multicast_encap_recover(unit, i));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate supported multicast type.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  type_flag     BCM_MULTICAST_TYPE_*.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Invalid parameter.
 */
static int
mc_type_validate(int unit, uint32_t type_flag)
{
    SHR_FUNC_ENTER(unit);

    /* Only single multicast type is allowed per API call. */
    /* Therefore number of bits on indicating mc type must equal to 1. */
    if (1 != _shr_popcount(type_flag)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (type_flag) {
        case BCM_MULTICAST_TYPE_L2:
            SHR_EXIT();
            break;
        case BCM_MULTICAST_TYPE_L3:
            if (ltsw_feature(unit, LTSW_FT_L3) &&
                bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
                SHR_EXIT();
            } else {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
            break;
        case BCM_MULTICAST_TYPE_VPLS:
            if (ltsw_feature(unit, LTSW_FT_MPLS)) {
                SHR_EXIT();
            } else {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
            break;
        case BCM_MULTICAST_TYPE_FLOW:
            if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
                SHR_EXIT();
            } else {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the max logical ports count or trunk members count.
 *
 * \param [in]   unit            Unit number
 * \param [out]  max             Returned max number of ports.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_get_max_ports(int unit, int *max)
{
    int max_num_ports;
    bcm_trunk_chip_info_t trunk_info;

    SHR_FUNC_ENTER(unit);

    max_num_ports = bcmi_ltsw_dev_logic_port_num(unit);

    
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_trunk_chip_info_get(unit, &trunk_info));

    if (max_num_ports < trunk_info.trunk_ports_max) {
        *max = trunk_info.trunk_ports_max;
    } else {
        *max = max_num_ports;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an unused l2mc group index and mark it used.
 *
 * \param [in]   unit            Unit number
 * \param [out]  l2mc_id         Returned unused l2mc index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l2_get_free_index(int unit, uint32_t *l2mc_id)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    for (i = L2MC_MIN(unit); i <= L2MC_MAX(unit); i++) {
        if (!L2MC_USED_ISSET(unit, i)) {
            L2MC_USED_SET(unit, i);
            *l2mc_id = i;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_FULL);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a L2 multicast group index.
 *
 * \param [in]   unit            Unit number
 * \param [in]   flags           BCM_MULTICAST_*
 * \param [out]  group           Multicast group ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l2_grp_create(int unit, uint32_t flags, bcm_multicast_t *group)
{
    uint32_t mc_index = 0;
    bool l2mc_locked = false;
    bcm_pbmp_t stk_cur;
    bcm_port_t port;

    SHR_FUNC_ENTER(unit);

    L2MC_LOCK(unit);
    l2mc_locked = true;

    if (flags & BCM_MULTICAST_WITH_ID) {
        mc_index = _BCM_MULTICAST_ID_GET(*group);

        if ((mc_index > L2MC_MAX(unit)) || (mc_index < L2MC_MIN(unit))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (L2MC_USED_ISSET(unit, mc_index)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        L2MC_USED_SET(unit, mc_index);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_get_free_index(unit, &mc_index));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l2_grp_create(unit, mc_index));

    /* Add stack ports to the mc group. */
    if (ltsw_feature(unit, LTSW_FT_STK)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stk_pbmp_get(unit, &stk_cur, NULL, NULL));
        BCM_PBMP_ITER(stk_cur, port) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_multicast_l2_grp_update(unit, mc_index,
                                                   bcmintMcL2GrpMemAdd, &port, 1));
        }
    }

    _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2, mc_index);

exit:
    if (l2mc_locked) {
        L2MC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a L2 multicast group index.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l2_grp_destroy(int unit, bcm_multicast_t group)
{
    uint32_t mc_index;
    bool l2mc_locked = false;

    SHR_FUNC_ENTER(unit);

    mc_index = _BCM_MULTICAST_ID_GET(group);

    if ((mc_index > L2MC_MAX(unit)) || (mc_index < L2MC_MIN(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L2MC_LOCK(unit);
    l2mc_locked = true;

    if (L2MC_USED_REFCNT(unit, mc_index) != 1) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l2_grp_destroy(unit, mc_index));

    /* Free the L2MC index. */
    L2MC_USED_REFCNT(unit, mc_index) = 0;

exit:
    if (l2mc_locked) {
        L2MC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a port to L2 multicast group.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port            GPORT Identifier.
 * \param [in]   encap_id        Mulicast replication ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l2_grp_egress_add(int unit, bcm_multicast_t group,
                     bcm_gport_t port, bcm_if_t encap_id)
{
    uint32_t mc_index;
    int gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int max_num_ports, actual_ports = 0;
    bcm_port_t *local_mem_ports = NULL;
    int modid_local = 0;
    bool l2mc_locked = false;

    SHR_FUNC_ENTER(unit);

    if (encap_id != BCM_IF_INVALID) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_resolve(unit, port, &mod_out, &port_out,
                                      &trunk_id, &gport_id));

    if (BCM_TRUNK_INVALID != trunk_id) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_get_max_ports(unit, &max_num_ports));
        local_mem_ports = sal_alloc(max_num_ports * sizeof(bcm_port_t),
                                    "ltswMcTrunkLocalMem");
        SHR_NULL_CHECK(local_mem_ports, SHR_E_MEMORY);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_local_members_get(unit,
                                               trunk_id,
                                               max_num_ports,
                                               local_mem_ports,
                                               &actual_ports));

        L2MC_LOCK(unit);
        l2mc_locked = true;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_l2_grp_update(unit,
                                               mc_index,
                                               bcmintMcL2GrpMemAdd,
                                               local_mem_ports,
                                               actual_ports));

    } else {
        (void) bcmi_ltsw_modid_is_local(unit, mod_out, &modid_local);

        if (!modid_local) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }

        L2MC_LOCK(unit);
        l2mc_locked = true;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_l2_grp_update(unit,
                                               mc_index,
                                               bcmintMcL2GrpMemAdd,
                                               &port_out,
                                               1));
    }

exit:
    if (l2mc_locked) {
        L2MC_UNLOCK(unit);
    }
    SHR_FREE(local_mem_ports);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a port from L2 multicast group.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port            GPORT Identifier.
 * \param [in]   encap_id        Mulicast replication ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l2_grp_egress_delete(int unit, bcm_multicast_t group,
                        bcm_port_t port, bcm_if_t encap_id)
{
    uint32_t mc_index;
    int gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int max_num_ports, actual_ports = 0;
    bcm_port_t *local_mem_ports = NULL;
    int modid_local = 0;
    bool l2mc_locked = false;

    SHR_FUNC_ENTER(unit);

    if (encap_id != -1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_resolve(unit, port, &mod_out, &port_out,
                                      &trunk_id, &gport_id));

    if (BCM_TRUNK_INVALID != trunk_id) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_get_max_ports(unit, &max_num_ports));
        local_mem_ports = sal_alloc(max_num_ports * sizeof(bcm_port_t),
                                    "ltswMcTrunkLocalMem");
        SHR_NULL_CHECK(local_mem_ports, SHR_E_MEMORY);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_local_members_get(unit,
                                               trunk_id,
                                               max_num_ports,
                                               local_mem_ports,
                                               &actual_ports));
        L2MC_LOCK(unit);
        l2mc_locked = true;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_l2_grp_update(unit,
                                               mc_index,
                                               bcmintMcL2GrpMemDel,
                                               local_mem_ports,
                                               actual_ports));
    } else {
        (void) bcmi_ltsw_modid_is_local(unit, mod_out, &modid_local);

        if (!modid_local) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }

        L2MC_LOCK(unit);
        l2mc_locked = true;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_l2_grp_update(unit,
                                               mc_index,
                                               bcmintMcL2GrpMemDel,
                                               &port_out,
                                               1));
    }

exit:
    if (l2mc_locked) {
        L2MC_UNLOCK(unit);
    }
    SHR_FREE(local_mem_ports);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Assign the complete set of egress GPORTs in the
 *        replication list for the specified multicast group ID.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port_array      List of GPORT IDs.
 * \param [in]   encap_id_array  List of encaption IDs.
 * \param [in]   port_count      Number of GPORTS in replication list
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l2_grp_egress_set(int unit, bcm_multicast_t group, bcm_gport_t *port_array,
                     bcm_if_t *encap_id_array, int port_count)
{
    uint32_t mc_index;
    int i, j;
    int gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int modid_local = 0;
    int max_num_ports, max_log_ports, mem_idx;
    bcm_port_t *trunk_member_list = NULL;
    int trunk_member_cnt = 0;
    int f_port_cnt = 0;  /* Final port member passed to HW. */
    bcm_port_t *f_port_arr = NULL;
    bool l2mc_locked = false;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    if ((port_count > 0) && ((!port_array) || (!encap_id_array))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else if (port_count < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < port_count; i++) {
        if (encap_id_array[i] != -1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);
    SHR_IF_ERR_VERBOSE_EXIT
        (mc_get_max_ports(unit, &max_num_ports));
    max_log_ports = bcmi_ltsw_dev_logic_port_num(unit);

    /* Convert GPORT array into local port numbers. */
    if (port_count > 0) {
        trunk_member_list = sal_alloc(sizeof(bcm_port_t) * max_num_ports,
                                      "ltswMcTrunkMemberList");
        SHR_NULL_CHECK(trunk_member_list, SHR_E_MEMORY);
        sal_memset(trunk_member_list, 0, sizeof(int) * port_count);

        /* Final port list to HW. */
        f_port_arr = sal_alloc(sizeof(bcm_port_t) * max_log_ports,
                               "ltswMcPortList");
        SHR_NULL_CHECK(f_port_arr, SHR_E_MEMORY);
        sal_memset(f_port_arr, 0, sizeof(bcm_port_t) * max_log_ports);
    }

    for (i = 0; i < port_count ; i++) {
        trunk_member_cnt = 0;
        sal_memset(trunk_member_list, 0, sizeof(bcm_port_t) * max_num_ports);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_resolve(unit, port_array[i],
                                          &mod_out, &port_out,
                                          &trunk_id, &gport_id));

        if (BCM_TRUNK_INVALID != trunk_id) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_trunk_local_members_get(unit,
                                                   trunk_id,
                                                   max_num_ports,
                                                   trunk_member_list,
                                                   &trunk_member_cnt));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_modid_is_local(unit, mod_out, &modid_local));

            if (!modid_local) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            trunk_member_list[0] = port_out;
            trunk_member_cnt = 1;
        }

        for (mem_idx = 0; mem_idx < trunk_member_cnt; mem_idx++) {
            /* There are some identical members in a trunk. */
            for (j = 0; j < f_port_cnt; j++) {
                if (f_port_arr[j] == trunk_member_list[mem_idx]) {
                    continue;
                }
            }
            if (f_port_cnt >= max_log_ports) {
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
            f_port_arr[f_port_cnt] = trunk_member_list[mem_idx];
            f_port_cnt++;
        }
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);

    L2MC_LOCK(unit);
    l2mc_locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l2_grp_update(unit,
                                           mc_index,
                                           bcmintMcL2GrpMemSet,
                                           f_port_arr,
                                           f_port_cnt));
exit:
    if (l2mc_locked) {
        L2MC_UNLOCK(unit);
    }
    SHR_FREE(trunk_member_list);
    SHR_FREE(f_port_arr);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve a set of egress multicast GPORTs in the
 *        replication list for the specified multicast group ID.
 *        Notes: If the input parameter port_size = 0, returned port_count
 *        is the total number of ports/encapsulation IDs in replication list.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port_size       Maxium mumber of GPORTS in replication list.
 * \param [out]  port_array      List of GPORT IDs.
 * \param [out]  ecap_id_array   List of encaption IDs.
 * \param [out]  port_count      Actual number of returned ports.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */

static int
mc_l2_grp_egress_get(int unit,
                     bcm_multicast_t group,
                     int port_size,
                     bcm_gport_t *port_array,
                     bcm_if_t *encap_id_array,
                     int *port_count)
{
    uint32_t mc_index;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    if ((NULL == port_count) || (port_size < 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* If port_size = 0, port_array and encap_id_array must be NULL. */
    if (port_size == 0) {
        if ((NULL != port_array) || (NULL != encap_id_array)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l2_egress_get(unit,
                                           mc_index,
                                           port_size,
                                           port_array,
                                           encap_id_array,
                                           port_count));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an unused l3mc group index and mark it used.
 *
 * \param [in]   unit            Unit number
 * \param [out]  l3mc_id         Returned unused l3mc index.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l3_get_free_index(int unit, uint32_t *l3mc_id)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    for (i = L3MC_MIN(unit); i <= L3MC_MAX(unit); i++) {
        if (!L3MC_USED_ISSET(unit, i)) {
            L3MC_USED_SET(unit, i);
            *l3mc_id = i;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_FULL);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a L3 multicast group index.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   flags           BCM_MULTICAST_*.
 * \param [out]  group           Multicast group ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l3_grp_create(int unit, uint32_t flags, bcm_multicast_t *group)
{
    uint32_t mc_index = 0;
    bool l3mc_locked = false;
    uint32_t type = 0;
    bcm_pbmp_t stk_cur;
    bcm_port_t port;
    bcm_if_t encap_id_array = BCM_IF_INVALID;

    SHR_FUNC_ENTER(unit);

    type = flags & BCM_MULTICAST_TYPE_MASK;

    L3MC_LOCK(unit);
    l3mc_locked = true;

    if (flags & BCM_MULTICAST_WITH_ID) {
        mc_index = _BCM_MULTICAST_ID_GET(*group);

        if ((mc_index > L3MC_MAX(unit)) || (mc_index < L3MC_MIN(unit))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (L3MC_USED_ISSET(unit, mc_index)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        L3MC_USED_SET(unit, mc_index);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_get_free_index(unit, &mc_index));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l3_grp_create(unit, mc_index));

    /* Add stack ports to the mc group. */
    if (ltsw_feature(unit, LTSW_FT_STK)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_stk_pbmp_get(unit, &stk_cur, NULL, NULL));
        BCM_PBMP_ITER(stk_cur, port) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_multicast_l3_grp_update(unit, mc_index,
                                                   bcmintMcL3GrpPortEncapAdd,
                                                   port, &encap_id_array, 0));
        }
    }

    /* Remap multicast groups. */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (mbcm_ltsw_multicast_remap_group_set(unit, mc_index, mc_index),
         SHR_E_UNAVAIL);

    if (type == BCM_MULTICAST_TYPE_L3) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L3,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_VPLS) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VPLS,
                                 mc_index);
    } else if (type == BCM_MULTICAST_TYPE_FLOW) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_FLOW,
                                 mc_index);
    }

    L3MC_TYPE(unit, mc_index) = _BCM_MULTICAST_TYPE_GET(*group);

exit:
    if (l3mc_locked) {
        L3MC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free a L3 multicast group index.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   group           Multicast group ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l3_grp_destroy(int unit, bcm_multicast_t group)
{
    uint32_t mc_index;
    bool l3mc_locked = false;
    uint32_t type;

    SHR_FUNC_ENTER(unit);

    mc_index = _BCM_MULTICAST_ID_GET(group);
    type = _BCM_MULTICAST_TYPE_GET(group);

    if ((mc_index > L3MC_MAX(unit)) || (mc_index < L3MC_MIN(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3MC_LOCK(unit);
    l3mc_locked = true;

    if (L3MC_USED_REFCNT(unit, mc_index) != 1) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (L3MC_TYPE(unit, mc_index) != type) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l3_grp_destroy(unit, mc_index));

    /* Free the L3MC index. */
    L3MC_USED_REFCNT(unit, mc_index) = 0;
    L3MC_TYPE(unit, mc_index) = 0;

exit:
    if (l3mc_locked) {
        L3MC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Add a port to L3 multicast group.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           L3 Multicast group ID.
 * \param [in]   port            GPORT Identifier.
 * \param [in]   encap_id        Mulicast replication ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l3_grp_egress_add(int unit, bcm_multicast_t group,
                     bcm_port_t port, bcm_if_t encap_id)
{
    int mc_index, gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int modid_local = FALSE;
    bool l3mc_locked = false;
    bcm_port_t *local_mem_ports = NULL;
    int max_num_ports = 0, actual_ports = 0;
    int i;


    SHR_FUNC_ENTER(unit);

    mc_index = _BCM_MULTICAST_ID_GET(group);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_resolve(unit, port, &mod_out, &port_out,
                                      &trunk_id, &gport_id));

    if (BCM_TRUNK_INVALID != trunk_id) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_get_max_ports(unit, &max_num_ports));
        local_mem_ports = sal_alloc(max_num_ports * sizeof(bcm_port_t),
                                    "ltswMcTrunkLocalMem");
        SHR_NULL_CHECK(local_mem_ports, SHR_E_MEMORY);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_local_members_get(unit,
                                               trunk_id,
                                               max_num_ports,
                                               local_mem_ports,
                                               &actual_ports));
        L3MC_LOCK(unit);
        l3mc_locked = true;

        for (i = 0; i < actual_ports; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_multicast_l3_grp_update(unit,
                                                   mc_index,
                                                   bcmintMcL3GrpPortEncapAdd,
                                                   local_mem_ports[i],
                                                   &encap_id,
                                                   1));
        }
    } else {
        (void) bcmi_ltsw_modid_is_local(unit, mod_out, &modid_local);

        if (!modid_local) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }

        L3MC_LOCK(unit);
        l3mc_locked = true;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_l3_grp_update(unit,
                                               mc_index,
                                               bcmintMcL3GrpPortEncapAdd,
                                               port_out,
                                               &encap_id,
                                               1));
    }

exit:
    if (l3mc_locked) {
        L3MC_UNLOCK(unit);
    }
    SHR_FREE(local_mem_ports);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete a port from L3 multicast group.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           L3 Multicast group ID.
 * \param [in]   port            GPORT Identifier.
 * \param [in]   encap_id        Mulicast replication ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l3_grp_egress_delete(int unit, bcm_multicast_t group,
                        bcm_port_t port, bcm_if_t encap_id)
{
    int mc_index, gport_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int modid_local = 0;
    bool l3mc_locked = false;
    bcm_port_t  *local_mem_ports = NULL;
    int max_num_ports = 0, actual_ports = 0;
    int i;


    SHR_FUNC_ENTER(unit);

    mc_index = _BCM_MULTICAST_ID_GET(group);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_resolve(unit, port, &mod_out, &port_out,
                                      &trunk_id, &gport_id));

    if (BCM_TRUNK_INVALID != trunk_id) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_get_max_ports(unit, &max_num_ports));
        local_mem_ports = sal_alloc(max_num_ports * sizeof(bcm_port_t),
                                    "ltswMcTrunkLocalMem");
        SHR_NULL_CHECK(local_mem_ports, SHR_E_MEMORY);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_trunk_local_members_get(unit,
                                               trunk_id,
                                               max_num_ports,
                                               local_mem_ports,
                                               &actual_ports));

        L3MC_LOCK(unit);
        l3mc_locked = true;

        for (i = 0; i < actual_ports; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_multicast_l3_grp_update(unit,
                                                   mc_index,
                                                   bcmintMcL3GrpPortEncapDel,
                                                   local_mem_ports[i],
                                                   &encap_id,
                                                   1));
        }
    } else {
        (void) bcmi_ltsw_modid_is_local(unit, mod_out, &modid_local);

        if (!modid_local) {
            SHR_ERR_EXIT(SHR_E_PORT);
        }

        L3MC_LOCK(unit);
        l3mc_locked = true;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_l3_grp_update(unit,
                                               mc_index,
                                               bcmintMcL3GrpPortEncapDel,
                                               port_out,
                                               &encap_id,
                                               1));
    }

exit:
    if (l3mc_locked) {
        L3MC_UNLOCK(unit);
    }
    SHR_FREE(local_mem_ports);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Assign the complete set of egress GPORTs in the
 *        replication list for the specified multicast group ID.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port_array      List of GPORT IDs.
 * \param [in]   encap_id_array  List of encaption IDs.
 * \param [in]   port_count      Number of GPORTS in replication list
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l3_grp_egress_set(int unit, bcm_multicast_t group, bcm_gport_t *port_array,
                     bcm_if_t *encap_id_array, int port_count)
{
    int         *tmp_encap_arr = NULL;
    int          encap_count;
    bcm_port_t  *lp_arr = NULL;
    bcm_port_t **tlmp_arr = NULL;
    int         *tlmp_cnt_arr = NULL;
    bcm_module_t mod_out;
    bcm_port_t   port_out;
    int          gport_id;
    bcm_trunk_t  trunk_id;
    int mc_index, i, idx, idx_max;
    int modid_local = 0;
    bool l3mc_locked = false;

    bcm_pbmp_t mc_pbmp;
    bcm_port_t port_iter;
    int trunk_local_found;

    int max_num_ports = 0;

    SHR_FUNC_ENTER(unit);

    mc_index = _BCM_MULTICAST_ID_GET(group);
    SHR_IF_ERR_VERBOSE_EXIT
        (mc_get_max_ports(unit, &max_num_ports));

    /* Convert GPORT array into local port numbers */
    if (port_count > 0) {
        /*
         * Local port array.
         * It records whether a port_array[i] is a trunk.
         * If it is a trunk, its value equals to trunk_id.
         * Otherwise, its value equals to local ports of port_array[i].
         */
        lp_arr = sal_alloc(sizeof(bcm_port_t) * port_count, "ltswMcLPArr");
        SHR_NULL_CHECK(lp_arr, SHR_E_MEMORY);

        tmp_encap_arr =
            sal_alloc(sizeof(int) * port_count, "ltswMcTmpEncapArr");
        SHR_NULL_CHECK(tmp_encap_arr, SHR_E_MEMORY);

        /*
         * Trunk local member ports count.
         * It's an port_count array to store members count of each trunk.
         * If port_array[i] is trunk, then trunk_local_ports should be non-zero.
         */
        tlmp_cnt_arr = sal_alloc(sizeof(int) * port_count, "ltswMcTLMPCnt");
        SHR_NULL_CHECK(tlmp_cnt_arr, SHR_E_MEMORY);
        sal_memset(tlmp_cnt_arr, 0, sizeof(int) * port_count);

        /*
         * Trunk local members ports array.
         * It's a port_count array to store possible pointers.
         * Every member is a pointer to a possible trunk local members list.
         * If port_array[i] is trunk, then local_trunk_port_array[i] is non-zero.
         */
        tlmp_arr =
            sal_alloc(sizeof(bcm_port_t *) * port_count, "ltswMcTLMPArrPt");
        SHR_NULL_CHECK(tlmp_arr, SHR_E_MEMORY);

        for (i = 0; i < port_count; i++) {
            tlmp_arr[i] =
                sal_alloc(sizeof(bcm_port_t) * max_num_ports, "ltswMcTLMPArr");
            SHR_NULL_CHECK(tlmp_arr[i], SHR_E_MEMORY);
            sal_memset(tlmp_arr[i], 0, sizeof(bcm_port_t) * max_num_ports);
        }
    }


    for (i = 0; i < port_count; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_resolve(unit, port_array[i],
                                          &mod_out, &port_out,
                                          &trunk_id, &gport_id));

        if (BCM_TRUNK_INVALID != trunk_id) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_trunk_local_members_get(unit,
                                                   trunk_id,
                                                   max_num_ports,
                                                   tlmp_arr[i],
                                                   &tlmp_cnt_arr[i]));
            /* Record the trunk ID to use later. */
            BCM_GPORT_TRUNK_SET(lp_arr[i], trunk_id);
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_modid_is_local(unit, mod_out, &modid_local));

            if (!modid_local) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            lp_arr[i] = port_out;
        }
    }

    L3MC_LOCK(unit);
    l3mc_locked = true;

    /* Clear all encap id list for all port. */
    if (0 == port_count) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_l3_grp_update(unit,
                                               mc_index,
                                               bcmintMcL3GrpPortEncapDelAll,
                                               BCM_PORT_INVALID,
                                               NULL,
                                               0));
    } else {
        /*
         * For each port, walk through the list of GPORTs
         * and collect all encap ids that match the port.
         */

        BCM_PBMP_CLEAR(mc_pbmp);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &mc_pbmp));

        BCM_PBMP_ITER(mc_pbmp, port_iter) {
            /* This records all encap_ids for a specific port. */
            encap_count = 0;
            for (i = 0; i < port_count ; i++) {
                if (BCM_GPORT_IS_TRUNK(lp_arr[i])) {
                    trunk_local_found = 0;
                    idx_max = tlmp_cnt_arr[i];
                    for (idx = 0; idx < idx_max; idx++) {
                        if (tlmp_arr[i][idx] == port_iter) {
                            trunk_local_found = 1;
                            break;
                        }
                    }
                    if (!trunk_local_found) {
                        continue;
                    }
                } else {
                    if (lp_arr[i] != port_iter) {
                        continue;
                    }
                }

                tmp_encap_arr[encap_count] = encap_id_array[i];
                encap_count++;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_multicast_l3_grp_update(unit,
                                                   mc_index,
                                                   bcmintMcL3GrpPortEncapSet,
                                                   port_iter,
                                                   tmp_encap_arr,
                                                   encap_count));
        }
    }

exit:
    if (tlmp_arr) {
        for (i = 0; i < port_count; i++) {
            SHR_FREE(tlmp_arr[i]);
        }
        SHR_FREE(tlmp_arr);
    }
    SHR_FREE(tlmp_cnt_arr);
    SHR_FREE(tmp_encap_arr);
    SHR_FREE(lp_arr);
    if (l3mc_locked) {
        L3MC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve a set of egress multicast GPORTs in the
 *        replication list for the specified L3 multicast group ID.
 *        Notes: If the input parameter port_size = 0, returned port_count
 *        is the total number of ports/encapsulation IDs in replication list.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port_size       Maxium mumber of GPORTS in replication list.
 * \param [out]  port_array      List of GPORT IDs.
 * \param [out]  ecap_id_array   List of encaption IDs.
 * \param [out]  port_count      Actual number of returned ports.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */

static int
mc_l3_grp_egress_get(int unit,
                     bcm_multicast_t group,
                     int port_size,
                     bcm_gport_t *port_array,
                     bcm_if_t *encap_id_array,
                     int *port_count)
{
    uint32_t mc_index;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check. */
    if ((port_count == NULL) || (port_size < 0)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* If port_size = 0, port_array and encap_id_array must be NULL. */
    if (port_size == 0) {
        if ((NULL != port_array) || (encap_id_array != NULL)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    mc_index = _BCM_MULTICAST_ID_GET(group);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l3_egress_get(unit,
                                           mc_index,
                                           port_size,
                                           port_array,
                                           encap_id_array,
                                           port_count));
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Get group id from HW L3_IPMC id.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   ipmc_id         l3 ipmc hw index.
 * \param [in]   group           Multicast group ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_l3_grp_id_get(int unit, uint32_t ipmc_id, bcm_multicast_t *group)
{
    SHR_FUNC_ENTER(unit);

    if ((ipmc_id > L3MC_MAX(unit)) || (ipmc_id < L3MC_MIN(unit))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((MC_INFO(unit)->l3mc_grp[ipmc_id].ref_cnt)) {
        _BCM_MULTICAST_GROUP_SET(*group,
                                 MC_INFO(unit)->l3mc_grp[ipmc_id].type,
                                 ipmc_id);
        SHR_EXIT();
    } else {
        *group = 0;
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Translate group type to BCM_MULTICAST_* flag.
 *
 * \param [in]   grp_type        Multicast group type.
 *
 * \retval BCM_MULTICAST_*.
 */
static uint32_t
mc_grp_type_to_flags(int grp_type)
{
    uint32_t flags = 0;

    switch (grp_type) {
    case _BCM_MULTICAST_TYPE_L2:
        flags = BCM_MULTICAST_TYPE_L2;
        break;
    case _BCM_MULTICAST_TYPE_L3:
        flags = BCM_MULTICAST_TYPE_L3;
        break;
    case _BCM_MULTICAST_TYPE_VPLS:
        flags = BCM_MULTICAST_TYPE_VPLS;
        break;
    case _BCM_MULTICAST_TYPE_SUBPORT:
        flags = BCM_MULTICAST_TYPE_SUBPORT;
        break;
    case _BCM_MULTICAST_TYPE_MIM:
        flags = BCM_MULTICAST_TYPE_MIM;
        break;
    case _BCM_MULTICAST_TYPE_WLAN:
        flags = BCM_MULTICAST_TYPE_WLAN;
        break;
    case _BCM_MULTICAST_TYPE_VLAN:
        flags = BCM_MULTICAST_TYPE_VLAN;
        break;
    case _BCM_MULTICAST_TYPE_TRILL:
        flags = BCM_MULTICAST_TYPE_TRILL;
        break;
    case _BCM_MULTICAST_TYPE_NIV:
        flags = BCM_MULTICAST_TYPE_NIV;
        break;
    case _BCM_MULTICAST_TYPE_EGRESS_OBJECT:
        flags = BCM_MULTICAST_TYPE_EGRESS_OBJECT;
        break;
    case _BCM_MULTICAST_TYPE_L2GRE:
        flags = BCM_MULTICAST_TYPE_L2GRE;
        break;
    case _BCM_MULTICAST_TYPE_VXLAN:
        flags = BCM_MULTICAST_TYPE_VXLAN;
        break;
    case _BCM_MULTICAST_TYPE_FLOW:
        flags = BCM_MULTICAST_TYPE_FLOW;
        break;
    case _BCM_MULTICAST_TYPE_EXTENDER:
        flags = BCM_MULTICAST_TYPE_EXTENDER;
        break;
    default:
        /* flags defaults to 0 above */
        break;
    }

    return flags;
}

/*!
 * \brief Translate BCM_MULTICAST_* flag to group type.
 *
 * \param [in]  flags   BCM_MULTICAST_*.
 *
 * \retval Group type.
 */
static int
mc_flags_to_grp_type(uint32 flags)
{
    int group_type = 0;

    if (flags & BCM_MULTICAST_TYPE_L2) {
        group_type = _BCM_MULTICAST_TYPE_L2;
    } else if (flags & BCM_MULTICAST_TYPE_L3){
        group_type = _BCM_MULTICAST_TYPE_L3;
    } else if (flags & BCM_MULTICAST_TYPE_VPLS){
        group_type = _BCM_MULTICAST_TYPE_VPLS;
    } else if (flags & BCM_MULTICAST_TYPE_SUBPORT){
        group_type = _BCM_MULTICAST_TYPE_SUBPORT;
    } else if (flags & BCM_MULTICAST_TYPE_MIM){
        group_type = _BCM_MULTICAST_TYPE_MIM;
    } else if (flags & BCM_MULTICAST_TYPE_WLAN){
        group_type = _BCM_MULTICAST_TYPE_WLAN;
    } else if (flags & BCM_MULTICAST_TYPE_VLAN) {
        group_type = _BCM_MULTICAST_TYPE_VLAN;
    } else if (flags & BCM_MULTICAST_TYPE_TRILL) {
        group_type = _BCM_MULTICAST_TYPE_TRILL;
    } else if (flags & BCM_MULTICAST_TYPE_NIV) {
        group_type = _BCM_MULTICAST_TYPE_NIV;
    } else if (flags & BCM_MULTICAST_TYPE_EGRESS_OBJECT) {
        group_type = _BCM_MULTICAST_TYPE_EGRESS_OBJECT;
    } else if (flags & BCM_MULTICAST_TYPE_L2GRE) {
        group_type = _BCM_MULTICAST_TYPE_L2GRE;
    } else if (flags & BCM_MULTICAST_TYPE_VXLAN) {
        group_type = _BCM_MULTICAST_TYPE_VXLAN;
    } else if (flags & BCM_MULTICAST_TYPE_FLOW) {
        group_type = _BCM_MULTICAST_TYPE_FLOW;
    } else if (flags & BCM_MULTICAST_TYPE_EXTENDER) {
        group_type = _BCM_MULTICAST_TYPE_EXTENDER;
    }

    return group_type;
}

/*!
 * \brief Check if the given multicast group is available on the device.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   group           Multicast group ID.
 *
 * \retval SHR_E_NONE            Multicast group is valid and available.
 * \retval SHR_E_EXISTS          Multicast group is valid but used.
 * \retval SHR_E_UNAVAIL         Multicast group is invalid.
 * \retval !SHR_E_NONE           Failure.
 */
static int
mc_grp_is_free(int unit, bcm_multicast_t group)
{
    int      mc_index;
    uint8_t  grp_type = 0;
    uint32_t type_flags = 0;

    SHR_FUNC_ENTER(unit);

    mc_index = _BCM_MULTICAST_ID_GET(group);

    if (_BCM_MULTICAST_IS_L2(group)) {
        if ((mc_index > L2MC_MAX(unit)) || (mc_index < L2MC_MIN(unit))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (L2MC_USED_ISSET(unit, mc_index)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else if (_BCM_MULTICAST_IS_SET(group)) {
        grp_type = _BCM_MULTICAST_TYPE_GET(group);
        type_flags = mc_grp_type_to_flags(grp_type);
        SHR_IF_ERR_VERBOSE_EXIT(mc_type_validate(unit, type_flags));

        if ((mc_index > L3MC_MAX(unit)) || (mc_index < L3MC_MIN(unit))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (L3MC_USED_ISSET(unit, mc_index)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if the given multicast group is a valid L3 or virtual multicast group.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   group           Multicast group ID.
 *
 * \retval SHR_E_NONE            Multicast group is valid.
 * \retval !SHR_E_NONE          Multicast group is invalid.
 */
static int
mc_multicast_l3_group_check(int unit, bcm_multicast_t group)
{
    int rv;
    SHR_FUNC_ENTER(unit);

    if (!_BCM_MULTICAST_IS_SET(group)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (_BCM_MULTICAST_IS_L2(group)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = mc_grp_is_free(unit, group);
    if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a multicast encap id based on multicast encapsulation info.
 *
 * \param [in]   unit          Unit number
 * \param [in]   mc_encap      Multicast encapsualtion information.
 * \param [out]  encap_id      Returned encap_id.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mc_encap_create(int unit,
                bcm_multicast_encap_t *mc_encap,
                bcm_if_t *encap_id)
{
    int l3mc_lock = FALSE;

    SHR_FUNC_ENTER(unit);

    if ((mc_encap == NULL) || (encap_id == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3MC_LOCK(unit);
    l3mc_lock = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_encap_create(unit, mc_encap, encap_id));

    if (ENCAP_USED_ISSET(unit, *encap_id)) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }
    ENCAP_USED_SET(unit, *encap_id);

exit:
    if (l3mc_lock) {
        L3MC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a multicast encap id.
 *
 * \param [in] unit            Unit number
 * \param [in] encap_id        Multicast encap_id.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
int
mc_encap_destroy(int unit, bcm_if_t encap_id)
{
    int l3mc_lock = FALSE;

    SHR_FUNC_ENTER(unit);

    if ((encap_id < 0) || (encap_id >= MC_INFO(unit)->encap_sz)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (!ENCAP_USED_ISSET(unit, encap_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    L3MC_LOCK(unit);
    l3mc_lock = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_encap_destroy(unit, encap_id));

    ENCAP_USED_CLR(unit, encap_id);

exit:
    if (l3mc_lock) {
        L3MC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get multicast encapsulation info from a multicast encap id.
 *
 * \param [in]  unit           Unit number
 * \param [in]  encap_id       Multicast encap_id.
 * \param [out] mc_encap       Returned encapsualtion info.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mc_encap_get(
    int unit,
    bcm_if_t encap_id,
    bcm_multicast_encap_t *mc_encap)
{
    SHR_FUNC_ENTER(unit);

    if (mc_encap == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((encap_id < 0) || (encap_id >= MC_INFO(unit)->encap_sz)) {
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    if (!ENCAP_USED_ISSET(unit, encap_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_encap_get(unit, encap_id, mc_encap));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the Encap ID for Egress_object.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   intf            Egress Object ID.
 * \param [out]  encap_id        Returned encap_id.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mc_egr_obj_encap_get(int unit,
                     bcm_multicast_t group,
                     bcm_if_t intf,
                     bcm_if_t *encap_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(encap_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_multicast_egress_object_encap_get(unit, group, intf,
                                                     encap_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update l2 group info based on port bitmap.
 *
 * \param [in]   unit            Unit number
 * \param [in]   pbmp            Port bitmap.
 * \param [in]   op              bcmintMcL2GrpMem***.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mc_l2_grp_pbmp_op(int unit, bcm_pbmp_t pbmp,
                  bcmint_multicast_l2_grp_op_code_t op)
{
    int mc_index;
    bcm_port_t port;
    SHR_FUNC_ENTER(unit);

    L2MC_LOCK(unit);
    BCM_PBMP_ITER(pbmp, port) {
        for (mc_index = L2MC_MIN(unit);
             mc_index <= L2MC_MAX(unit); mc_index++) {
            if (L2MC_USED_ISSET(unit, mc_index)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_multicast_l2_grp_update(unit, mc_index,
                                                       op, &port, 1));
            }
        }
    }
exit:
    L2MC_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update l3 group info based on port bitmap.
 *
 * \param [in]   unit            Unit number
 * \param [in]   pbmp            Port bitmap.
 * \param [in]   op              bcmintMcL3GrpPortEncap***.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
mc_l3_grp_pbmp_op(int unit, bcm_pbmp_t pbmp,
                  bcmint_multicast_l3_grp_op_code_t op)
{
    int mc_index;
    bcm_port_t port;
    bcm_if_t encap_id_array = BCM_IF_INVALID;
    SHR_FUNC_ENTER(unit);

    L3MC_LOCK(unit);
    BCM_PBMP_ITER(pbmp, port) {
        for (mc_index = L3MC_MIN(unit);
             mc_index <= L3MC_MAX(unit); mc_index++) {
            if (L3MC_USED_ISSET(unit, mc_index)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_multicast_l3_grp_update(unit, mc_index,
                                                       op, port,
                                                       &encap_id_array, 1));
            }
        }
    }
exit:
    L3MC_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Init Multicast module.
 *
 * \param [in] unit              Unit number.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval SHR_E_MEMORY          Out of memory.
 */
int
bcm_ltsw_multicast_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize Multicast module software database. */
    SHR_IF_ERR_EXIT
        (mc_info_init(unit));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_multicast_init(unit));

    SHR_IF_ERR_EXIT
        (mc_info_recover(unit));

    MC_INFO(unit)->inited = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach Multicast module.
 *
 * \param [in] unit              Unit number
 *
 * \retval SHR_E_NONE            No errors.
 */
int
bcm_ltsw_multicast_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (MC_INFO(unit)->inited == FALSE) {
        SHR_EXIT();
    }

    L2MC_LOCK(unit);
    L3MC_LOCK(unit);

    MC_INFO(unit)->inited = FALSE;

    L3MC_UNLOCK(unit);
    L2MC_UNLOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_multicast_detach(unit));

    mc_info_cleanup(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate a multicast group index.
 *
 * \param [in]   unit            Unit number
 * \param [in]   flags           BCM_MULTICAST_*
 * \param [out]  group           Multicast group ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_create(int unit, uint32_t flags, bcm_multicast_t *group)
{
    uint32_t type;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    SHR_NULL_CHECK(group, SHR_E_PARAM);

    type = flags & BCM_MULTICAST_TYPE_MASK;
    SHR_IF_ERR_VERBOSE_EXIT
        (mc_type_validate(unit, type));

    if (type == BCM_MULTICAST_TYPE_L2) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_grp_create(unit, flags, group));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_create(unit, flags, group));
    }

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Free a multicast group index.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_destroy(int unit, bcm_multicast_t group)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    if (!_BCM_MULTICAST_IS_SET(group)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = mc_grp_is_free(unit, group);
    if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (_BCM_MULTICAST_IS_L2(group)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_grp_destroy(unit, group));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_destroy(unit, group));
    }

exit:
   SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the flags associated with a multicast group.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [out]  flags           BCM_MULTICAST_*
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_group_get(int unit, bcm_multicast_t group, uint32_t *flags)
{
    uint32_t        mc_index;
    uint32_t        type_flags;
    bcm_multicast_t get_group;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    SHR_NULL_CHECK(flags, SHR_E_PARAM);

    mc_index = _BCM_MULTICAST_ID_GET(group);

    /* Check for the type of the multicast group. */
    if (_BCM_MULTICAST_IS_L2(group)) {
        if ((mc_index > L2MC_MAX(unit)) || (mc_index < L2MC_MIN(unit))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (!L2MC_USED_ISSET(unit, mc_index)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        type_flags = BCM_MULTICAST_TYPE_L2;
    } else {
        if (!_BCM_MULTICAST_IS_L3(group) &&
            !_BCM_MULTICAST_IS_VPLS(group) &&
            !_BCM_MULTICAST_IS_FLOW(group)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_id_get(unit, mc_index, &get_group));
        if (get_group != group) {
            /* IPMC index is allocated, but not to the same group type. */
            type_flags = 0;
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            type_flags = mc_grp_type_to_flags(_BCM_MULTICAST_TYPE_GET(group));
        }
    }

    *flags = (type_flags | BCM_MULTICAST_WITH_ID);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if the given multicast group is available on the device.
 *
 * \param [in]   unit      Unit number
 * \param [in]   group     Multicast group ID.
 *
 * \retval SHR_E_NONE      Group is valid and availalble on the device.
 * \retval SHR_E_EXISTS    Group is valid but already in use on this device.
 * \retval !SHR_E_NONE     Group is invalid no this device.
 */
int
bcm_ltsw_multicast_group_is_free(int unit, bcm_multicast_t group)
{
    uint32_t        mc_index;
    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);
    mc_index = _BCM_MULTICAST_ID_GET(group);

    /* Check for the type of the multicast group. */
    if (_BCM_MULTICAST_IS_L2(group)) {
        if ((mc_index > L2MC_MAX(unit)) || (mc_index < L2MC_MIN(unit))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (L2MC_USED_ISSET(unit, mc_index)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        if (!_BCM_MULTICAST_IS_L3(group) &&
            !_BCM_MULTICAST_IS_VPLS(group) &&
            !_BCM_MULTICAST_IS_FLOW(group)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if ((mc_index > L3MC_MAX(unit)) || (mc_index < L3MC_MIN(unit))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if ((MC_INFO(unit)->l3mc_grp[mc_index].ref_cnt)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve the minimum and maximum unallocated multicast groups
 * for a given multicast type.
 *
 * \param [in]   unit      Unit number
 * \param [in]   group     Multicast group ID.
 * \param [out]  group_min Minimum available multicast group of specified type.
 * \param [out]  group_max Maximum available multicast group of specified type.
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
int
bcm_ltsw_multicast_group_free_range_get(int unit, uint32 type_flag,
                                       bcm_multicast_t *group_min,
                                       bcm_multicast_t *group_max)
{
    bcm_multicast_t mg_min, mg_max, group, free_min, free_max;
    uint32_t index_min, index_max, grp_type;
    int rv = BCM_E_PARAM;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_type_validate(unit, type_flag));

    if (type_flag == BCM_MULTICAST_TYPE_L2) {
        index_min = L2MC_MIN(unit);
        index_max = L2MC_MAX(unit);
    } else {
        index_min = L3MC_MIN(unit);
        index_max = L3MC_MAX(unit);
    }

    grp_type = mc_flags_to_grp_type(type_flag);
    _BCM_MULTICAST_GROUP_SET(mg_min, grp_type, index_min);
    _BCM_MULTICAST_GROUP_SET(mg_max, grp_type, index_max);

    free_min = free_max = 0;
    for (group = mg_min; group <= mg_max; group++) {
        rv = mc_grp_is_free(unit, group);
        if (rv == SHR_E_NONE) {
            if (0 == free_min) {
                free_min = group;
            }
            free_max = group;
        } else if (BCM_E_EXISTS == rv) {
            rv = BCM_E_NONE;
        } else {
            /* Real error, return. */
            break;
        }
    }

    if (rv == SHR_E_NONE) {
        if (0 == free_min) {
            /* No available groups of this type. */
            return BCM_E_NOT_FOUND;
        } else {
            if (group_min) {
                *group_min = free_min;
            }
            if (group_max) {
                *group_max = free_max;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a GPORT to the replication list for the specified
 *        mulitcast group ID.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port            GPORT ID.
 * \param [in]   encap_id        Encap ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_egress_add(int unit, bcm_multicast_t group,
                              bcm_gport_t port, bcm_if_t encap_id)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    rv = mc_grp_is_free(unit, group);
    if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (_BCM_MULTICAST_IS_L2(group)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_grp_egress_add(unit, group, port, encap_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_egress_add(unit, group, port, encap_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete GPORT from the replication list for the specified
 *        mulitcast group ID.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port            GPORT ID.
 * \param [in]   encap_id        Encap ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_egress_delete(int unit, bcm_multicast_t group,
                                 bcm_gport_t port, bcm_if_t encap_id)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    rv = mc_grp_is_free(unit, group);
    if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (_BCM_MULTICAST_IS_L2(group)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_grp_egress_delete(unit, group, port, encap_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_egress_delete(unit, group, port, encap_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all replications for the specified mulitcast group ID.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_egress_delete_all(int unit, bcm_multicast_t group)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    rv = mc_grp_is_free(unit, group);
    if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (_BCM_MULTICAST_IS_L2(group)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_grp_egress_set(unit, group, NULL, NULL, 0));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_egress_set(unit, group, NULL, NULL, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Assign the complete set of egress GPORTs in the
 *        replication list for the specified multicast group ID.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port_count      Number of GPORTS in replication list
 * \param [in]   port_array      List of GPORT IDs.
 * \param [in]   encap_id_array  List of encap IDs.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_egress_set(int unit, bcm_multicast_t group,
                              int port_count, bcm_gport_t *port_array,
                              bcm_if_t *encap_id_array)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    if (port_count > 0) {
        if (NULL == port_array) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (NULL == encap_id_array) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (port_count < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = mc_grp_is_free(unit, group);
    if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (_BCM_MULTICAST_IS_L2(group)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_grp_egress_set(unit, group, port_array,
                                  encap_id_array, port_count));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_egress_set(unit, group, port_array,
                                  encap_id_array, port_count));
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Retrieve a set of egress multicast GPORTs in the
 *        replication list for the specified multicast group ID.
 *        Notes: If the input parameter port_max = 0, returned port_count
 *        is the total number of ports/encapsulation IDs in replication list.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   port_max        Maxium mumber of GPORTS in replication list.
 * \param [out]  port_array      List of GPORT IDs.
 * \param [out]  encap_id_array  List of encap IDs.
 * \param [out]  port_count      Actual number of returned ports.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_egress_get(int unit, bcm_multicast_t group,
                              int port_max, bcm_gport_t *port_array,
                              bcm_if_t *encap_id_array, int *port_count)
{
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    rv = mc_grp_is_free(unit, group);
    if (rv != SHR_E_EXISTS) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (_BCM_MULTICAST_IS_L2(group)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_grp_egress_get(unit, group, port_max, port_array,
                                  encap_id_array, port_count));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_egress_get(unit, group, port_max, port_array,
                                  encap_id_array, port_count));
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Get the Encap ID for Egress_object.
 *
 * \param [in]   unit            Unit number
 * \param [in]   group           Multicast group ID.
 * \param [in]   intf            Egress Object ID.
 * \param [out]  encap_id        Returned encap_id.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_egress_object_encap_get(int unit,
                                           bcm_multicast_t group,
                                           bcm_if_t intf,
                                           bcm_if_t *encap_id)
{
    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    SHR_NULL_CHECK(encap_id, SHR_E_PARAM);


    if (_BCM_MULTICAST_IS_L3(group) ||
        _BCM_MULTICAST_IS_FLOW(group)) {
        SHR_IF_ERR_EXIT
            (mc_egr_obj_encap_get(unit, group, intf, encap_id));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Iterate over the defined multicast groups of the type
 * specified in 'flags'. If all types are desired, use
 * MULTICAST_TYPE_MASK.
 *
 * \param [in] unit Unit number
 * \param [in] trav_fn Callback function.
 * \param [in] flags BCM_MULTICAST_*
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
int
bcm_ltsw_multicast_group_traverse(int unit,
                                  bcm_multicast_group_traverse_cb_t trav_fn,
                                  uint32 flags, void *user_data)
{
    uint32 group_flags, flags_mask;
    bcm_multicast_t group;
    int mc_index;
    int rv;

    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    flags_mask = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_TYPE_L3;

    if (ltsw_feature(unit, LTSW_FT_L3) &&
        bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
        flags_mask |= BCM_MULTICAST_TYPE_L3;
    }

    if (ltsw_feature(unit, LTSW_FT_MPLS)) {
        flags_mask |= BCM_MULTICAST_TYPE_VPLS;
    }

    if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        flags_mask |= BCM_MULTICAST_TYPE_FLOW;
    }

    if ((flags & flags_mask) == 0) {
        /* No recognized multicast types to traverse */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    flags &= flags_mask;

    if (flags & BCM_MULTICAST_TYPE_L2) {
        group_flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID;
        for (mc_index = L2MC_MIN(unit); mc_index <= L2MC_MAX(unit);
             mc_index++) {
            if (L2MC_USED_ISSET(unit, mc_index)) {
                _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L2,
                                         mc_index);
                rv = (*trav_fn)(unit, group, group_flags, user_data);
                if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                    SHR_IF_ERR_VERBOSE_EXIT(rv);
                }
            }
        }
    }

    if (flags & ~(BCM_MULTICAST_TYPE_L2)) {
        for (mc_index = L3MC_MIN(unit); mc_index <= L3MC_MAX(unit);
             mc_index++) {
            if (L3MC_USED_ISSET(unit, mc_index)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mc_l3_grp_id_get(unit, mc_index, &group));
                group_flags =
                    mc_grp_type_to_flags(_BCM_MULTICAST_TYPE_GET(group))
                    | BCM_MULTICAST_WITH_ID;;
                if (group_flags & flags) {
                    rv = (*trav_fn)(unit, group, group_flags, user_data);
                    if (SHR_FAILURE(rv) && bcmi_ltsw_dev_cb_err_abort(unit)) {
                        SHR_IF_ERR_VERBOSE_EXIT(rv);
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a multicast encap id based on multicast encapsulation info.
 *
 * \param [in]   unit          Unit number
 * \param [in]   mc_encap      Multicast encapsualtion information.
 * \param [out]  encap_id      Returned encap_id.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
int
bcm_ltsw_multicast_encap_create(int unit,
                                bcm_multicast_encap_t *mc_encap,
                                bcm_if_t *encap_id)
{
    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_encap_create(unit, mc_encap, encap_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a multicast encap id.
 *
 * \param [in] unit            Unit number
 * \param [in] encap_id        Multicast encap_id.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
int
bcm_ltsw_multicast_encap_destroy(int unit, bcm_if_t encap_id)
{
    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_encap_destroy(unit, encap_id));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Get multicast encapsulation info from a multicast encap id.
 *
 * \param [in]  unit           Unit number
 * \param [in]  encap_id       Multicast encap_id.
 * \param [out] mc_encap       Returned encapsualtion info.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
int
bcm_ltsw_multicast_encap_get(
    int unit,
    bcm_if_t encap_id,
    bcm_multicast_encap_t *mc_encap)
{
    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    SHR_NULL_CHECK(mc_encap, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_encap_get(unit, encap_id, mc_encap));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Set a multicast group mapping.
 *
 * \param [in]  unit           Unit number.
 * \param [in]  from Multicast group ID to map from.
 * \param [in] to Multicast group ID to map to.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
int
bcm_ltsw_multicast_control_set(
    int unit,
    bcm_multicast_t group,
    bcm_multicast_control_t type,
    int arg)
{
    int mc_index = 0, mc_index_to = 0;
    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_multicast_l3_group_check(unit, group));
    mc_index = _BCM_MULTICAST_ID_GET(group);

    switch (type) {
    case bcmMulticastControlMtu:
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_mtu_set(unit, mc_index, arg));
        break;
    case bcmMulticastRemapGroup:
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_multicast_l3_group_check(unit, arg));
        mc_index_to   = _BCM_MULTICAST_ID_GET(arg);

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_remap_group_set(unit, mc_index, mc_index_to));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Get a multicast group mapping.
 *
 * \param [in]  unit           Unit number.
 * \param [in]  from Multicast group ID to map from.
 * \param [in] to Multicast group ID to map to.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
int
bcm_ltsw_multicast_control_get(
    int unit,
    bcm_multicast_t group,
    bcm_multicast_control_t type,
    int *arg)
{
    int mc_index = 0, mc_index_to;
    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_multicast_l3_group_check(unit, group));

    switch (type) {
    case bcmMulticastControlMtu:
        mc_index = _BCM_MULTICAST_ID_GET(group);
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_mtu_get(unit, mc_index, arg));
        break;
    case bcmMulticastRemapGroup:
        mc_index = _BCM_MULTICAST_ID_GET(group);

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_multicast_remap_group_get(unit, mc_index, &mc_index_to));

        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_id_get(unit, mc_index_to, (bcm_multicast_t *)arg));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();

}

int
bcmi_ltsw_multicast_l3_grp_id_get(
    int unit,
    uint32_t ipmc_id,
    bcm_multicast_t *group)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mc_l3_grp_id_get(unit, ipmc_id, group));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_l2_grp_validate(
    int unit,
    int mc_index,
    int *in_use)
{
    SHR_FUNC_ENTER(unit);

    MC_INIT_CHECK(unit);
    SHR_NULL_CHECK(in_use,SHR_E_PARAM);

    *in_use = L2MC_USED_ISSET(unit, mc_index);
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_def_vlan_grp_id_get(int unit, uint32_t *grp_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_def_vlan_grp_id_get(unit, grp_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_def_vlan_grp_update(int unit)
{
    bool l2mc_locked = FALSE;

    SHR_FUNC_ENTER(unit);

    L2MC_LOCK(unit);
    l2mc_locked = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_def_vlan_grp_update(unit));

exit:
    if (l2mc_locked) {
        L2MC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_l2_l3_combined_set(int unit, bool enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l2_l3_combined_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_l2_l3_combined_get(int unit, bool *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l2_l3_combined_get(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_stk_update(int unit)
{

    bcm_pbmp_t add_ports, remove_ports;
    bcm_pbmp_t stk_cur, stk_pre;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_stk_pbmp_get(unit, &stk_cur, NULL, &stk_pre));
    BCM_PBMP_ASSIGN(add_ports, stk_cur);
    BCM_PBMP_REMOVE(add_ports, stk_pre);
    BCM_PBMP_ASSIGN(remove_ports, stk_pre);
    BCM_PBMP_REMOVE(remove_ports, stk_cur);

    if (BCM_PBMP_NOT_NULL(add_ports)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_grp_pbmp_op(unit, add_ports, bcmintMcL2GrpMemAdd));
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_pbmp_op(unit, add_ports, bcmintMcL3GrpPortEncapAdd));
    }

    if (BCM_PBMP_NOT_NULL(remove_ports)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l2_grp_pbmp_op(unit, remove_ports, bcmintMcL2GrpMemDel));
        SHR_IF_ERR_VERBOSE_EXIT
            (mc_l3_grp_pbmp_op(unit, remove_ports, bcmintMcL3GrpPortEncapDel));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_repl_port_attach(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_repl_port_attach(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_repl_port_detach(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_repl_port_detach(unit, port));

exit:
    SHR_FUNC_EXIT();
}

void
bcmi_ltsw_multicast_sw_dump(int unit)
{
    ltsw_mc_info_t *mc_i = MC_INFO(unit);
    int i;

    if (!mc_i->inited) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information MULTICAST - Unit %d\n"),
             unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Init : %d\n"),
             mc_i->inited));
    LOG_CLI((BSL_META_U(unit,
                        "  L2 multicast table minimum index : %d\n"),
             mc_i->l2mc_min));
    LOG_CLI((BSL_META_U(unit,
                        "  L2 multicast table maximum index : %d\n"),
             mc_i->l2mc_max));
    /* Just show the in-use L2 multicast groups. */
    for (i = L2MC_MIN(unit); i <= L2MC_MAX(unit); i++) {
        if (L2MC_USED_ISSET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                        "  L2 multicast group : %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                        "      in use : %d\n"), L2MC_USED_REFCNT(unit, i)));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "  L3 multicast table minimum index : %d\n"),
             mc_i->l3mc_min));
    LOG_CLI((BSL_META_U(unit,
                        "  L3 multicast table maximum index : %d\n"),
             mc_i->l3mc_max));
    /* Just show the in-use L3 multicast groups. */
    for (i = L3MC_MIN(unit); i <= L3MC_MAX(unit); i++) {
        if (L3MC_USED_ISSET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                        "  L3 multicast group : %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                        "      in use : %d\n"), L3MC_USED_REFCNT(unit, i)));
            LOG_CLI((BSL_META_U(unit,
                        "      group type : %d\n"), L3MC_TYPE(unit, i)));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "  L3 encapsulation ID size : %d\n"),
             mc_i->encap_sz));
    /* Just show the in-use encap ID. */
    for (i = 0; i < mc_i->encap_sz; i++) {
        if (ENCAP_USED_ISSET(unit, i)) {
            LOG_CLI((BSL_META_U(unit,
                        "  L3 encap ID : %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                        "      in use : %d\n"), 1));
        }
    }
}

int
bcmi_ltsw_multicast_vlan_flood_grp_create(
    int unit, bcm_pbmp_t *pbmp, uint32_t *mc_index)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_idx_tbl_entry_alloc(unit, 0,
                                       BCMI_LTSW_IDX_TBL_ID_L2_MC_GROUP,
                                       BCMI_L2_MC_GROUP_USER_ID_VLAN,
                                       pbmp, (int *)mc_index));
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_vlan_flood_grp_update(
    int unit, bcm_pbmp_t *pbmp, uint32_t mc_index)
{
    uint32_t max_ports;
    bcm_port_t *port_arr = NULL;
    uint32_t i, cnt = 0, def_grp = 0;

    SHR_FUNC_ENTER(unit);

    /*L2 MC resources for VLAN are managed by index table.
     *This function is only for default grp update.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_def_vlan_grp_id_get(unit, &def_grp));
    if (mc_index != def_grp) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    max_ports = bcmi_ltsw_dev_logic_port_num(unit);
    port_arr = sal_alloc(sizeof(bcm_port_t) * max_ports, "xfsMcPortIdArr");
    SHR_NULL_CHECK(port_arr, SHR_E_MEMORY);
    sal_memset(port_arr, 0, sizeof(bcm_port_t) * max_ports);
    for (i = 0; i < max_ports; i++) {
        if (BCM_PBMP_MEMBER(*pbmp, i)) {
            port_arr[cnt] = i;
            cnt++;
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_l2_grp_update(unit, mc_index,
                                           bcmintMcL2GrpMemSet,
                                           port_arr, cnt));
exit:
    SHR_FREE(port_arr);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_vlan_flood_grp_destroy(
    int unit, uint32_t mc_index)
{
    uint32_t def_grp = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_def_vlan_grp_id_get(unit, &def_grp));
    if (mc_index == def_grp) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_idx_tbl_entry_free(unit, 0,
                                      BCMI_LTSW_IDX_TBL_ID_L2_MC_GROUP,
                                      BCMI_L2_MC_GROUP_USER_ID_VLAN,
                                      mc_index));
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_vlan_flood_grp_recover
    (int unit, bcm_pbmp_t *pbmp, uint32_t index)
{
    uint32 ref_cnt, grp_id;

    SHR_FUNC_ENTER(unit);

    /* Default vlan 1 always occupy l2mc index 0 which is managed singly. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_multicast_def_vlan_grp_id_get(unit, &grp_id));
    if (grp_id == index) {
         SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_idx_tbl_entry_ref_get(unit,
                                         BCMI_LTSW_IDX_TBL_ID_L2_MC_GROUP,
                                         BCMI_L2_MC_GROUP_USER_ID_VLAN,
                                         index, &ref_cnt));
    if ((ref_cnt == 0) && (pbmp != NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_idx_tbl_entry_hash_update(unit,
                                                 BCMI_LTSW_IDX_TBL_ID_L2_MC_GROUP,
                                                 BCMI_L2_MC_GROUP_USER_ID_VLAN,
                                                 pbmp, index));
    } else if ((ref_cnt == 0) && (pbmp == NULL)) {
        /* L2MC and vlan member ports should have same ref_cnt info. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_idx_tbl_entry_ref_increase(unit,
                                              BCMI_LTSW_IDX_TBL_ID_L2_MC_GROUP,
                                              BCMI_L2_MC_GROUP_USER_ID_VLAN,
                                              index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_vlan_flood_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_multicast_vlan_flood_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_multicast_vlan_flood_unregister(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_idx_tbl_unregister(unit, BCMI_LTSW_IDX_TBL_ID_L2_MC_GROUP,
                                      BCMI_L2_MC_GROUP_USER_ID_VLAN));
exit:
    SHR_FUNC_EXIT();
}
