/*! \file ipmc.c
 *
 * IPMC Module Emulator above SDKLT.
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
#include <bcm_int/ltsw/mbcm/ipmc.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/ipmc.h>
#include <bcm_int/ltsw/ipmc_int.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/multicast.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_str.h>


/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_IPMC

/* Returns whether the mask is empty (all ones) or not. */
#define IPMC_IS_IPV4_NO_MASK(mask) ((mask) == 0xFFFFFFFF)

/*
 * Checks whether the mask of the IPv6 address is empty (all 1s)
 * - if the last byte is 0xFF,
 * then we can assume that all bytes in the mask are equal to 0xFF.
 */
#define IPMC_IS_IPV6_NO_MASK(mask) ((mask)[15] == 0xFF)

#define VALID_VLAN_ID(_id_)    ((_id_ > BCM_VLAN_MIN) && (_id_ <= BCM_VLAN_MAX))

/* L3 ingress interface info associated to a RP. */
typedef struct ltsw_ipmc_l3_iif_s {
    /* L3 ingress interface ID. */
    bcm_if_t l3_iif;

    /* Next L3 ingress interface. */
    struct ltsw_ipmc_l3_iif_s *next;

} ltsw_ipmc_l3_iif_t;

/* Per Rendezvous Point info. */
typedef struct ltsw_ipmc_rp_s {
   /* Linked list of active L3 IIFs belonging to this RP. */
   ltsw_ipmc_l3_iif_t  *l3_iif_list;

} ltsw_ipmc_rp_t;

/* IPMC information. */
typedef struct ltsw_ipmc_info_s {

    /* IPMC module initialized. */
    int                       inited;

    /* IPMC mutex lock. */
    sal_mutex_t               mutex;

    /* Maximum number of Rendezvous Point. */
    int                       rp_max;

    /* Minimum number of Rendezvous Point. */
    int                       rp_min;

    /* Bitmap of valid rp id. */
    SHR_BITDCL               *rp_bmp;

    /* RP info for PIM-BIDIR. */
    ltsw_ipmc_rp_t           *rp_info;

    /* Stat counter ID for MC hitbit. */
    uint32_t                 *hit_stat_id;

    /* Hit flexctr is enabled. */
    bool                     hit_flexctr_enabled;

} ltsw_ipmc_info_t;

/* Static global variable of IPMC info. */
static ltsw_ipmc_info_t ltsw_ipmc_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/* IPMC information. */
#define IPMC_INFO(u)        (&ltsw_ipmc_info[u])

/* IPMC RP information. */
#define IPMC_RP(u, rp_id) (&(ltsw_ipmc_info[u].rp_info[rp_id]))

/* Check if IPMC module is initialized. */
#define IPMC_INIT_CHECK(u)                              \
    do {                                                \
        ltsw_ipmc_info_t *ipmc_i = IPMC_INFO(u);        \
        if (!ltsw_feature(unit, LTSW_FT_IPMC)) {        \
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);             \
        }                                               \
        if (ipmc_i->inited == false) {                  \
            SHR_IF_ERR_EXIT(SHR_E_INIT);                \
        }                                               \
    } while(0)

/* Check if RP sub-module is initialized. */
#define IPMC_RP_INIT_CHECK(u)                           \
    do {                                                \
        if (!ltsw_feature(unit, LTSW_FT_PIM_BIDIR)) {   \
            SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);             \
        }                                               \
        if ((IPMC_INFO(u)->inited == false) ||          \
            (IPMC_INFO(u)->rp_max <= 0)) {              \
            SHR_IF_ERR_EXIT(SHR_E_INIT);                \
        }                                               \
    } while(0)

/* Check if RP_ID is valid. */
#define IPMC_RP_ID_CHECK(u, rp_id)                      \
    do {                                                \
        if (((rp_id) < IPMC_INFO(u)->rp_min) ||         \
            ((rp_id) > IPMC_INFO(u)->rp_max)) {         \
            SHR_ERR_EXIT(SHR_E_PARAM);                  \
        }                                               \
    } while(0)

/* IPMC Lock. */
#define IPMC_LOCK(u) \
    sal_mutex_take(IPMC_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define IPMC_UNLOCK(u) \
    sal_mutex_give(IPMC_INFO(u)->mutex);

/* IPMC hitbit by flexctr. */
#define IPMC_HIT_FLEXCTR_ENABLED(_u) (IPMC_INFO(_u)->hit_flexctr_enabled)

#define IPMC_HIT_FLEXCTR_STAT_ID(_u) (*ltsw_ipmc_info[_u].hit_stat_id)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Validate ipmc lookup key.
 *
 * \param [in] unit Unit number.
 * \param [in] data L3 IPMC forwarding entry information.
 * \param [out] subnet Return whether the key is subnet.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ipmc_key_validate(int unit, bcm_ipmc_addr_t *data, int *subnet)
{
    uint32_t max_vrf = 0;

    SHR_FUNC_ENTER(unit);

    /* Make sure ipmc was initialized on the device. */
    IPMC_INIT_CHECK(unit);

    if (data == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*
     * Destination address must be multicast.
     * Source address must be unicast.
     */
    if (data->flags & BCM_IPMC_IP6) {
        if (!BCM_IP6_MULTICAST(data->mc_ip6_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (BCM_IP6_MULTICAST(data->s_ip6_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (IPMC_IS_IPV6_NO_MASK(data->mc_ip6_mask)) {
            *subnet = 0;
        } else {
            *subnet = 1;
        }
    } else {
        if (!BCM_IP4_MULTICAST(data->mc_ip_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (BCM_IP4_MULTICAST(data->s_ip_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (IPMC_IS_IPV4_NO_MASK(data->mc_ip_mask)) {
            *subnet = 0;
        } else {
            *subnet = 1;
        }
    }

    /* Validate vrf range. */
    SHR_IF_ERR_EXIT(
        bcmi_ltsw_l3_vrf_max_get(unit, &max_vrf));
    if ((data->vrf > (int)max_vrf) ||
        ((*subnet == 0) && (data->vrf < BCM_L3_VRF_DEFAULT)) ||
        ((*subnet == 1) && (data->vrf < BCM_L3_VRF_GLOBAL))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validate key l3_iif range. */
    if ((*subnet == 0) && (data->ing_intf != BCM_IF_INVALID)) {
        SHR_IF_ERR_EXIT(
            bcmi_ltsw_l3_ing_intf_id_validate(unit, data->ing_intf));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup IPMC RP information.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ipmc_rp_cleanup(int unit)
{
    ltsw_ipmc_info_t *ipmc_i = IPMC_INFO(unit);
    int i;
    ltsw_ipmc_l3_iif_t *cur_ptr, *next_ptr;

    SHR_FUNC_ENTER(unit);

    /* Cleanup rp l3_iif list info. */
    for (i = 0; i <= ipmc_i->rp_max; i++) {
        /* Free linked list of active L3_IIFs. */
        cur_ptr = ipmc_i->rp_info[i].l3_iif_list;
        while (NULL != cur_ptr) {
            next_ptr = cur_ptr->next;
            SHR_FREE(cur_ptr);
            cur_ptr = next_ptr;
        }
        ipmc_i->rp_info[i].l3_iif_list = NULL;
    }
    SHR_FREE(ipmc_i->rp_info);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup IPMC RP information.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ipmc_rp_init(int unit)
{
    ltsw_ipmc_info_t *ipmc_i = IPMC_INFO(unit);
    bcmint_ipmc_pub_info_t ipmc_pub;
    int wb_flag = bcmi_warmboot_get(unit);
    int size;
    uint32_t bmp_size = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ipmc_pub, 0, sizeof(ipmc_pub));
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ipmc_pub_info_get(unit, &ipmc_pub));
    ipmc_i->rp_max = ipmc_pub.rp_max;
    ipmc_i->rp_min = ipmc_pub.rp_min;
    if (ipmc_i->rp_max > 0) {
        size = (ipmc_i->rp_max + 1) * sizeof(ltsw_ipmc_rp_t);
        SHR_ALLOC(ipmc_i->rp_info, size, "ltswIpmcRpInfo");
        SHR_NULL_CHECK(ipmc_i->rp_info, SHR_E_MEMORY);
        sal_memset(ipmc_i->rp_info, 0, size);
    }

    bmp_size = SHR_BITALLOCSIZE(ipmc_pub.rp_max - ipmc_pub.rp_min + 1);
    ipmc_i->rp_bmp = bcmi_ltsw_ha_mem_alloc(unit,
                                           BCMI_HA_COMP_ID_IPMC,
                                           BCMINT_IPMC_SUB_COMP_ID_RP_BMP,
                                           "bcmIPMCRpBmp",
                                           &bmp_size);
    SHR_NULL_CHECK(ipmc_i->rp_bmp, SHR_E_MEMORY);
    if (!wb_flag) {
        sal_memset(ipmc_i->rp_bmp, 0, bmp_size);
    }

exit:
    SHR_FUNC_EXIT()
}

/*!
 * \brief Cleanup IPMC module software database.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ipmc_info_cleanup(int unit)
{
    ltsw_ipmc_info_t *ipmc_i = IPMC_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_PIM_BIDIR)) {
        (void)ipmc_rp_cleanup(unit);
    }

    if (ipmc_i->mutex) {
        sal_mutex_destroy(ipmc_i->mutex);
        ipmc_i->mutex = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize IPMC module software database.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
static int
ipmc_info_init(int unit)
{
    ltsw_ipmc_info_t *ipmc_i = IPMC_INFO(unit);

    SHR_FUNC_ENTER(unit);

    if (ipmc_i->inited) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_ipmc_detach(unit));
    }

    if (ipmc_i->mutex == NULL) {
        ipmc_i->mutex = sal_mutex_create("ipmcMutex");
        SHR_NULL_CHECK(ipmc_i->mutex, SHR_E_MEMORY);
    }

    /* Initialize RP info. */
    if (ltsw_feature(unit, LTSW_FT_PIM_BIDIR)) {
        SHR_IF_ERR_EXIT
            (ipmc_rp_init(unit));
    }

exit:
    if (SHR_FUNC_ERR()) {
        ipmc_info_cleanup(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover IPMC module software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Out of memory.
 */
static int
ipmc_info_recover(int unit)
{
    int warm = 0;
    uint32_t rp_bmp;
    int l3_iif_min, l3_iif_max, rp_id;
    int i, rv;
    ltsw_ipmc_l3_iif_t *cur_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_PIM_BIDIR)) {
        SHR_FUNC_EXIT();
    }

    warm = bcmi_warmboot_get(unit);
    if (warm) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_ing_intf_id_range_get(unit,
                                                &l3_iif_min,
                                                &l3_iif_max));
        for (i = l3_iif_min; i <= l3_iif_max; i++) {
            rv = bcmi_ltsw_l3_ing_intf_rp_get(unit, i, &rp_bmp);
            if (rv != SHR_E_NONE) {
                continue;
            }
            for (rp_id = IPMC_INFO(unit)->rp_min;
                 rp_id <= IPMC_INFO(unit)->rp_max;
                 rp_id++) {
                /* coverity[callee_ptr_arith : FALSE] */
                if (SHR_BITGET(&rp_bmp, rp_id)) {
                    if (0 == SHR_BITGET(IPMC_INFO(unit)->rp_bmp, rp_id)) {
                        SHR_ERR_EXIT(SHR_E_INTERNAL);
                    }
                    cur_ptr = NULL;
                    SHR_ALLOC(cur_ptr,
                              sizeof(ltsw_ipmc_l3_iif_t),
                              "ltswIpmcL3Iif");
                    SHR_NULL_CHECK(cur_ptr, SHR_E_MEMORY);
                    cur_ptr->l3_iif = i;
                    cur_ptr->next = IPMC_RP(unit, rp_id)->l3_iif_list;
                    IPMC_RP(unit, rp_id)->l3_iif_list = cur_ptr;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the flexctr mode.
 *
 * Currently, IPMC hitbit flexctr only works in flexctr global mode.
 * This function is used to get the flexctr mode. It will be removed
 * after flexctr support IPMC hitbit in per-pipe mode.
 *
 * \param [in]  unit Unit number.
 * \param [out] mode 0: Flexctr is in global mode. 1: per-pipe mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
flexctr_mode_get(int unit, int *mode)
{
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(dunit, CTR_EFLEX_CONFIGs, &eh));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, CTR_ING_EFLEX_OPERMODE_PIPEUNIQUEs, &data));

    *mode = data;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Init flexctr for IPMC hitbit.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ipmc_hitbit_flexctr_init(int unit)
{
    ltsw_ipmc_info_t *info = &ltsw_ipmc_info[unit];
    bcm_flexctr_action_t action = {0};
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_a_op = NULL;
    int min, max, width, mode = 0;
    uint32_t sz;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (flexctr_mode_get(unit, &mode));

    sz = sizeof(uint32_t);
    info->hit_stat_id = bcmi_ltsw_ha_mem_alloc(unit,
                                         BCMI_HA_COMP_ID_IPMC,
                                         BCMINT_IPMC_SUB_COMP_ID_MC_HIT_STAT_ID,
                                         "bcmIpmcHitbitStatId",
                                         &sz);
    SHR_NULL_CHECK(info->hit_stat_id, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((sz < sizeof(uint32_t)) ? SHR_E_MEMORY : SHR_E_NONE);

    if (bcmi_warmboot_get(unit)) {
        info->hit_flexctr_enabled = true;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_ing_class_id_range_get(unit, &min, &max, &width));

    action.source = bcmFlexctrSourceIpmc;
    action.hint = 0;
    action.mode = bcmFlexctrCounterModeWide;
    /* Maximun count of logical counter index. Each counter has 64 bits. */
    action.index_num = (max + 1 + 64)/64;
    if (mode == 1) {
        /* Need to create a global action in pipe unique mode. */
        action.flags |= BCM_FLEXCTR_ACTION_FLAGS_GLOBAL;
    }

    /* Physical counter index is (L3DstClassId / 64). */
    index_op = &action.index_operation;
    index_op->object[0] = bcmFlexctrObjectIngL3DstClassId;
    index_op->mask_size[0] = width; /* Width of Class_ID. */
    index_op->shift[0] = 6; /* Divide by 64. */
    index_op->object[1] = bcmFlexctrObjectConstZero;
    index_op->mask_size[1] = 1;
    index_op->shift[1] = 0;

    /* Hit bit position is bit (L3DstClassId & 0x3f) */
    value_a_op = &action.operation_a;
    value_a_op->select = bcmFlexctrValueSelectCounterValue;
    value_a_op->object[0] = bcmFlexctrObjectIngL3DstClassId;
    value_a_op->mask_size[0] = 6; /* Mod 64 to get the bit position. */
    value_a_op->shift[0] = 0;
    value_a_op->object[1] = bcmFlexctrObjectConstZero;
    value_a_op->mask_size[1] = 1;
    value_a_op->shift[1] = 0;
    value_a_op->type = bcmFlexctrValueOperationTypeSetBit;

    SHR_IF_ERR_EXIT
        (bcm_ltsw_flexctr_action_create(unit, 0, &action,
                                        info->hit_stat_id));

    info->hit_flexctr_enabled = true;

exit:
    if (SHR_FUNC_ERR() && !bcmi_warmboot_get(unit)) {
        if (!info->hit_stat_id) {
            (void)bcmi_ltsw_ha_mem_free(unit, info->hit_stat_id);
            info->hit_stat_id = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup flexctr reserved for IPMC hitbit.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ipmc_hitbit_flexctr_cleanup(int unit)
{
    ltsw_ipmc_info_t *info = &ltsw_ipmc_info[unit];

    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcm_ltsw_flexctr_action_destroy(unit, *info->hit_stat_id),
         SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Constuct key-fields in bcmi_ltsw_l3_fib_t.
 *
 * \param [in] unit Unit Number.
 * \param [in] ipmc Ipmc data structure.
 * \param [out] l3_fib L3 fib structure.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Fail
 */
static int
ipmc_addr_to_l3_fib_key(int unit,
                        bcm_ipmc_addr_t *ipmc,
                        bcmi_ltsw_l3_fib_t *l3_fib)
{
    int subnet = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ipmc_key_validate(unit, ipmc, &subnet));

    if (ipmc->flags & BCM_IPMC_IP6) {
        l3_fib->flags |= BCM_L3_IP6;
        sal_memcpy(l3_fib->sip6_addr, ipmc->s_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3_fib->sip6_addr_mask, ipmc->s_ip6_mask, BCM_IP6_ADDRLEN);
        sal_memcpy(l3_fib->ip6_addr, ipmc->mc_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3_fib->ip6_addr_mask, ipmc->mc_ip6_mask, BCM_IP6_ADDRLEN);
    } else {
        l3_fib->sip_addr = ipmc->s_ip_addr;
        l3_fib->sip_addr_mask = ipmc->s_ip_mask;
        l3_fib->ip_addr = ipmc->mc_ip_addr;
        l3_fib->ip_addr_mask = ipmc->mc_ip_mask;
    }

    l3_fib->vrf = ipmc->vrf;
    l3_fib->l3_iif = ipmc->ing_intf;
    l3_fib->network = subnet;
    l3_fib->flags |= BCM_L3_IPMC;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Constructs gport from port, modid or trunk given in the data structure
 *        if bcmSwitchUseGport is set.
 *
 * \param [in] unit Unit Number.
 * \param [in/out] data Data structure to use in order to construct a gport.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Fail
 */
STATIC int
ipmc_gport_construct(int unit, bcm_ipmc_addr_t *data)
{
    int isGport;
    bcm_gport_t gport;
    bcmi_ltsw_gport_info_t gport_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_switch_control_get(unit, bcmSwitchUseGport, &isGport));

    /* if output should not be gport then do nothing*/
    if (!isGport) {
        SHR_EXIT();
    }

    if (data->ts) {
        gport_info.tgid = data->port_tgid;
        gport_info.gport_type = _SHR_GPORT_TYPE_TRUNK;
    } else {
        gport_info.modid = data->mod_id;
        gport_info.gport_type = _SHR_GPORT_TYPE_MODPORT;
        gport_info.port = data->port_tgid;
    }
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_port_gport_construct(unit, &gport_info, &gport));

    data->port_tgid = gport;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Resolve port, modid or trunk from given gport.
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Given gport.
 * \param [out] port Physical port encoded in gport.
 * \param [out] modid Module id encoded in gport.
 * \param [out] tgid Trunk group ID encoded in gport.
 * \param [out] tbit Trunk indicator 1 if trunk encoded in gport 0 otherwise.
 * \param [in] isPortLocal Indicator if port encoded in gport must be only local physical port.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Fail
 */
STATIC int
ipmc_gport_resolve(int unit, bcm_gport_t gport, bcm_port_t *port,
                   bcm_module_t *modid, bcm_trunk_t *tgid, bool *tbit,
                   int isPortLocal)
{
    int id;
    bcm_trunk_t local_tgid;
    bcm_module_t local_modid;
    bcm_port_t local_port;
    int ismymodid;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(port, SHR_E_PARAM);
    SHR_NULL_CHECK(modid, SHR_E_PARAM);
    SHR_NULL_CHECK(modid, SHR_E_PARAM);
    SHR_NULL_CHECK(tbit, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmi_ltsw_port_gport_resolve(unit, gport, &local_modid, &local_port,
        &local_tgid, &id));

    if (-1 != id) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    if (BCM_TRUNK_INVALID != local_tgid) {
        *tgid = local_tgid;
        *tbit = 1;
    } else {
        *port = local_port;
        *modid = local_modid;
        *tbit = 0;
        if (isPortLocal) {
            SHR_IF_ERR_EXIT(
                bcmi_ltsw_modid_is_local(unit, local_modid, &ismymodid));
            if (ismymodid != TRUE) {
                SHR_ERR_EXIT(BCM_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

int
bcmi_ltsw_ipmc_hit_flexctr_action_id_get(int unit, uint32_t *action_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_id, SHR_E_PARAM);

    IPMC_INIT_CHECK(unit);

    if (!IPMC_HIT_FLEXCTR_ENABLED(unit)) {
        *action_id = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                               IPMC_HIT_FLEXCTR_STAT_ID(unit),
                                               &ci));

    *action_id = ci.action_index;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ipmc_flexctr_hit_get(int unit, uint32_t object, uint32_t action_id,
                               bool hit_clr, uint32_t *hit_flags)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};
    uint32_t stat_id, ctr_idx, flags = 0;
    bcm_flexctr_counter_value_t ctr_val;

    SHR_FUNC_ENTER(unit);

    if ((action_id == BCMI_LTSW_FLEXCTR_ACTION_INVALID) ||
        !IPMC_HIT_FLEXCTR_ENABLED(unit)) {
        SHR_EXIT();
    }

    ci.action_index = action_id;
    ci.stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &stat_id));

    /* Incase user attaches other flexctr action than the one reserved for
     * MC-hit to MC entry, return directly. User should use flexctr API to
     * fetch the counter value. */
    if (IPMC_HIT_FLEXCTR_STAT_ID(unit) != stat_id) {
        SHR_EXIT();
    }

    flags |= BCM_IPMC_HIT_ENABLE;

    /* Divide by 64. (Each physical counter has 64 bits) */
    ctr_idx = object >> 6;
    sal_memset(&ctr_val, 0, sizeof(bcm_flexctr_counter_value_t));
    SHR_IF_ERR_EXIT
        (bcm_ltsw_flexctr_stat_sync_get(unit, stat_id, 1, &ctr_idx, &ctr_val));

    if (ctr_val.value[1] & (1ULL << (object % 64))) {
        flags |= BCM_IPMC_HIT;

        if (hit_clr) {
            ctr_val.value[1] &= ~(1ULL << (object % 64));
            SHR_IF_ERR_EXIT
                (bcm_ltsw_flexctr_stat_set(unit, stat_id, 1, &ctr_idx,
                                           &ctr_val));
        }
    }

exit:
    if (hit_flags) {
        *hit_flags = flags;
    }

    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ipmc_l3_fib_to_ipmc(int unit,
                              bcmi_ltsw_l3_fib_t *l3_fib,
                              bcm_ipmc_addr_t *ipmc)
{
    bool hit_clr;
    uint32_t hit_flags = 0;

    SHR_FUNC_ENTER(unit);

    if ((l3_fib == NULL) || (ipmc == NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ipmc->s_ip_addr   = l3_fib->sip_addr;
    ipmc->mc_ip_addr  = l3_fib->ip_addr;
    ipmc->mc_ip_mask  = l3_fib->ip_addr_mask;
    ipmc->vrf         = l3_fib->vrf;
    sal_memcpy(ipmc->s_ip6_addr, l3_fib->sip6_addr, BCM_IP6_ADDRLEN);
    sal_memcpy(ipmc->mc_ip6_addr, l3_fib->ip6_addr, BCM_IP6_ADDRLEN);
    sal_memcpy(ipmc->mc_ip6_mask, l3_fib->ip6_addr_mask, BCM_IP6_ADDRLEN);

    if (!l3_fib->network) {
        if (!(l3_fib->flags & BCM_L3_IP6)) {
            ipmc->mc_ip_mask = 0xFFFFFFFF;
        } else {
            bcm_ip6_mask_create(ipmc->mc_ip6_mask, 128);
        }

        ipmc->ing_intf = l3_fib->l3_iif ? l3_fib->l3_iif : BCM_IF_INVALID;
    } else if (ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        ipmc->ing_intf = l3_fib->l3_iif ? l3_fib->l3_iif : BCM_IF_INVALID;
    }

    ipmc->group = l3_fib->mc_group;
    ipmc->l3a_intf = l3_fib->expected_l3_iif;
    ipmc->mtu = l3_fib->mtu;
    ipmc->lookup_class = l3_fib->class_id;
    ipmc->opaque_ctrl_id = l3_fib->opaque_ctrl_id;
    if (l3_fib->rpa_id != 0) {
        ipmc->rp_id = l3_fib->rpa_id;
    } else {
        ipmc->rp_id = BCM_IPMC_RP_ID_INVALID;
    }

    if (l3_fib->flags & BCM_L3_IP6) {
        ipmc->flags |= BCM_IPMC_IP6;
    }
    if (l3_fib->flags & BCM_L3_REPLACE) {
        ipmc->flags |= BCM_IPMC_REPLACE;
    }
    if (l3_fib->flags & BCM_L3_COPY_TO_CPU) {
        ipmc->flags |= BCM_IPMC_COPYTOCPU;
    }
    if (l3_fib->flags & BCM_L3_DST_DISCARD) {
        ipmc->flags |= BCM_IPMC_DISCARD;
    }
    if (l3_fib->int_flags & BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_TOCPU) {
        ipmc->flags |= BCM_IPMC_RPF_FAIL_TOCPU;
    }

    if (l3_fib->int_flags & BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_DROP) {
        ipmc->flags |= BCM_IPMC_RPF_FAIL_DROP;
    }

    if (l3_fib->int_flags & BCMI_LTSW_L3_FIB_F_IPMC_SOURCE_PORT_NOCHECK) {
        ipmc->flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
    }

    if (l3_fib->int_flags & BCMI_LTSW_L3_FIB_F_IPMC_POST_LOOKUP_RPF_CHECK) {
        ipmc->flags |= BCM_IPMC_POST_LOOKUP_RPF_CHECK;
    }

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_IPMC_HITBIT)) {
        /* Get the MC hit info. */
        hit_clr = (l3_fib->flags & BCM_L3_HIT_CLEAR) ? true : false;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_ipmc_flexctr_hit_get(unit, l3_fib->class_id,
                                            l3_fib->flex_ctr_action_index,
                                            hit_clr, &hit_flags));
        ipmc->flags |= hit_flags;
    } else {
        if (l3_fib->flags & BCM_L3_HIT) {
            ipmc->flags |= BCM_IPMC_HIT;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize IPMC module and enable IPMC support.
 *
 * This function has to be called before any other IPMC functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_IPMC)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Initialize IPMC module software database. */
    SHR_IF_ERR_EXIT
        (ipmc_info_init(unit));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ipmc_init(unit));

    if (bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)){
        SHR_IF_ERR_EXIT
            (ipmc_info_recover(unit));
    }

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_IPMC_HITBIT) &&
        bcmi_ltsw_property_get(unit, BCMI_CPN_FLEXCTR_ACTION_RSVD_IPMC_HITBIT,
                               0)) {
        SHR_IF_ERR_EXIT
            (ipmc_hitbit_flexctr_init(unit));
    }

    IPMC_INFO(unit)->inited = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize IPMC module and disable IPMC support.
 *
 * This function has to be called before any other IPMC functions.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Detach successfully.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_detach(int unit)
{
    uint32_t attrib = BCMI_LTSW_L3_FIB_ATTR_MC;

    SHR_FUNC_ENTER(unit);

    if (IPMC_INFO(unit)->inited == FALSE) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ipmc_detach(unit));

    IPMC_LOCK(unit);
    IPMC_INFO(unit)->inited = FALSE;
    IPMC_UNLOCK(unit);

    if (!bcmi_warmboot_get(unit)) {
        if (bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)){
            attrib |= (BCMI_LTSW_L3_FIB_ATTR_IPV4 |
                       BCMI_LTSW_L3_FIB_ATTR_IPV6 |
                       BCMI_LTSW_L3_FIB_ATTR_HOST |
                       BCMI_LTSW_L3_FIB_ATTR_NETWORK |
                       BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP |
                       BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP);

            SHR_IF_ERR_EXIT
                (bcmi_ltsw_l3_fib_mc_delete_all(unit, attrib));
        }

        if (IPMC_HIT_FLEXCTR_ENABLED(unit)) {
            SHR_IF_ERR_EXIT
                (ipmc_hitbit_flexctr_cleanup(unit));
        }
    }

    SHR_IF_ERR_EXIT
        (ipmc_info_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/disable IPMC support.
 *
 * \param [in] unit Unit number.
 * \param [in] enable 1: enable IPMC support. 0: disable IPMC support.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_enable(int unit, int enable)
{
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_INIT_CHECK(unit);

    IPMC_LOCK(unit);
    ipmc_locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ipmc_enable(unit, enable));

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Adds a new entry to the L3 IPMC forwarding table.
 *
 * \param [in] unit Unit number.
 * \param [in] data L3 IPMC forwarding entry information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_add(int unit, bcm_ipmc_addr_t *data)
{
    int subnet = 0;
    bcmi_ltsw_l3_fib_t l3_fib = {0};
    bool ipmc_locked = false;
    bcmi_ltsw_flexctr_counter_info_t ci;
    int rv;

    SHR_FUNC_ENTER(unit);

    IPMC_INIT_CHECK(unit);

    if (data->flags & BCM_IPMC_L2) {
        IPMC_LOCK(unit);
        ipmc_locked = true;

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_ipmc_add_l2(unit, data));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (ipmc_key_validate(unit, data, &subnet));

    /* Validate expected l3_iif range. */
    if (data->l3a_intf != 0) {
        SHR_IF_ERR_EXIT(
            bcmi_ltsw_l3_ing_intf_id_validate(unit, data->l3a_intf));
    }

    

    /* Validate cos range. */
    if (!BCM_PRIORITY_VALID(data->cos)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (_BCM_MULTICAST_IS_L3(data->group) ||
        _BCM_MULTICAST_IS_VPLS(data->group) ||
        _BCM_MULTICAST_IS_FLOW(data->group)) {
        l3_fib.mc_group = data->group;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (data->flags & BCM_IPMC_IP6) {
        sal_memcpy(l3_fib.sip6_addr, data->s_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3_fib.sip6_addr_mask, data->s_ip6_mask, BCM_IP6_ADDRLEN);
        sal_memcpy(l3_fib.ip6_addr, data->mc_ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(l3_fib.ip6_addr_mask, data->mc_ip6_mask, BCM_IP6_ADDRLEN);
    } else {
        l3_fib.sip_addr = data->s_ip_addr;
        l3_fib.sip_addr_mask = data->s_ip_mask;
        l3_fib.ip_addr  = data->mc_ip_addr;
        l3_fib.ip_addr_mask = data->mc_ip_mask;
    }
    if (!subnet || ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        l3_fib.l3_iif = data->ing_intf;
    }

    l3_fib.vrf             = data->vrf;
    l3_fib.expected_l3_iif = data->l3a_intf;
    l3_fib.port_tgid       = data->port_tgid;
    l3_fib.mod_id          = data->mod_id;
    l3_fib.is_trunk        = data->ts;
    l3_fib.mtu             = data->mtu;
    l3_fib.class_id        = data->lookup_class;
    l3_fib.network         = subnet;
    if (data->rp_id != BCM_IPMC_RP_ID_INVALID) {
        l3_fib.rpa_id  = data->rp_id;
    } else {
        l3_fib.rpa_id = 0;
    }

    l3_fib.opaque_ctrl_id = data->opaque_ctrl_id;

    /* Always set BCM_L3_IPMC flag to notify l3 fib. */
    l3_fib.flags |= BCM_L3_IPMC;
    if (data->flags & BCM_IPMC_IP6) {
        l3_fib.flags |= BCM_L3_IP6;
    }
    if (data->flags & BCM_IPMC_REPLACE) {
        l3_fib.flags |= BCM_L3_REPLACE;
    }
    if (data->flags & BCM_IPMC_COPYTOCPU) {
        l3_fib.flags |= BCM_L3_COPY_TO_CPU;
    }
    if (data->flags & BCM_IPMC_DISCARD) {
        l3_fib.flags |= BCM_L3_DST_DISCARD;
    }
    /* Dont set hit bits if user says so */
    if (data->flags & BCM_IPMC_HIT_CLEAR) {
        l3_fib.flags |= BCM_L3_HIT_CLEAR;
    } else {
        l3_fib.flags |= BCM_L3_HIT;
    }
    if (data->flags & BCM_IPMC_RPF_FAIL_TOCPU) {
        l3_fib.int_flags |= BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_TOCPU;
    }

    if (data->flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK) {
        l3_fib.int_flags |= BCMI_LTSW_L3_FIB_F_IPMC_POST_LOOKUP_RPF_CHECK;
        if (data->flags & BCM_IPMC_RPF_FAIL_DROP) {
            l3_fib.int_flags |= BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_DROP;
        }
        if (data->flags & BCM_IPMC_RPF_FAIL_TOCPU) {
            l3_fib.int_flags |= BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_TOCPU;
        }
    }

    if (data->flags & BCM_IPMC_SOURCE_PORT_NOCHECK) {
        l3_fib.int_flags |= BCMI_LTSW_L3_FIB_F_IPMC_SOURCE_PORT_NOCHECK;
    }

    if (!(l3_fib.int_flags & BCMI_LTSW_L3_FIB_F_IPMC_SOURCE_PORT_NOCHECK)) {
        if (BCM_GPORT_IS_SET(l3_fib.port_tgid)) {
            SHR_IF_ERR_EXIT
                (ipmc_gport_resolve(unit, l3_fib.port_tgid,
                                    &(l3_fib.port_tgid),
                                    &(l3_fib.mod_id),
                                    &(l3_fib.port_tgid),
                                    &(l3_fib.is_trunk), 0));
        }
    }

    if (data->flags & BCM_IPMC_HIT_ENABLE) {
        if (!IPMC_HIT_FLEXCTR_ENABLED(unit)) {
            SHR_ERR_MSG_EXIT(SHR_E_UNAVAIL,
                (BSL_META_U(unit,
                            "Flexctr action for IPMC hit is not "
                            "initialized.\n")));
        }
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_flexctr_counter_id_info_get(unit,
                                               IPMC_HIT_FLEXCTR_STAT_ID(unit),
                                               &ci));
        l3_fib.flex_ctr_action_index = ci.action_index;
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                               IPMC_HIT_FLEXCTR_STAT_ID(unit)));
    } else {
        l3_fib.flex_ctr_action_index = BCMI_LTSW_FLEXCTR_ACTION_INVALID;
    }

    rv = bcmi_ltsw_l3_fib_mc_add(unit, &l3_fib);
    if (SHR_FAILURE(rv)) {
        if (l3_fib.flex_ctr_action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                               IPMC_HIT_FLEXCTR_STAT_ID(unit)));
        }
        SHR_ERR_EXIT(rv);
    }

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from the L3 IPMC forwarding table.
 *
 * \param [in] unit Unit number.
 * \param [in] data L3 IPMC forwarding entry information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_remove(int unit, bcm_ipmc_addr_t *data)
{
    bcmi_ltsw_l3_fib_t l3_fib = {0};
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_INIT_CHECK(unit);

    if (data->flags & BCM_IPMC_L2) {
        IPMC_LOCK(unit);
        ipmc_locked = true;

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_ipmc_remove_l2(unit, data));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (ipmc_addr_to_l3_fib_key(unit, data, &l3_fib));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_fib_mc_delete(unit, &l3_fib));

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove all entries from the L3 IPMC forwarding table.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_remove_all(int unit)
{
    int rv = SHR_E_NONE;
    uint32_t attrib = BCMI_LTSW_L3_FIB_ATTR_MC;
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_INIT_CHECK(unit);

    IPMC_LOCK(unit);
    ipmc_locked = true;

    
    rv = mbcm_ltsw_ipmc_delete_all_l2(unit);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    IPMC_UNLOCK(unit);
    ipmc_locked = false;

    if (!bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)){
        SHR_EXIT();
    }

    attrib |= (BCMI_LTSW_L3_FIB_ATTR_IPV4 |
               BCMI_LTSW_L3_FIB_ATTR_IPV6 |
               BCMI_LTSW_L3_FIB_ATTR_HOST |
               BCMI_LTSW_L3_FIB_ATTR_NETWORK |
               BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP |
               BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_fib_mc_delete_all(unit, attrib));

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an IPMC entry from the L3 IPMC forwarding table.
 *
 * \param [in] unit Unit number.
 * \param [in] data L3 IPMC forwarding entry information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_find(int unit, bcm_ipmc_addr_t *data)
{
    bcmi_ltsw_l3_fib_t l3_fib = {0};
    bool ipmc_locked = false, hit_clr;
    uint32 hit_flags = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);

    IPMC_INIT_CHECK(unit);

    if (data->flags & BCM_IPMC_L2) {
        IPMC_LOCK(unit);
        ipmc_locked = true;

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_ipmc_find_l2(unit, data));
        SHR_EXIT();
    }

    hit_clr = (data->flags & BCM_IPMC_HIT_CLEAR) ? true : false;

    SHR_IF_ERR_EXIT
        (ipmc_addr_to_l3_fib_key(unit, data, &l3_fib));

    if (data->flags & BCM_IPMC_HIT_CLEAR) {
        l3_fib.flags |= BCM_L3_HIT_CLEAR;
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_fib_mc_find(unit, &l3_fib));

    data->group = l3_fib.mc_group;
    data->l3a_intf = l3_fib.expected_l3_iif;
    data->mtu = l3_fib.mtu;
    data->lookup_class = l3_fib.class_id;
    if (l3_fib.rpa_id != 0) {
        data->rp_id = l3_fib.rpa_id;
    } else {
        data->rp_id = BCM_IPMC_RP_ID_INVALID;
    }

    data->opaque_ctrl_id = l3_fib.opaque_ctrl_id;

    if (l3_fib.flags & BCM_L3_IP6) {
        data->flags |= BCM_IPMC_IP6;
    }
    if (l3_fib.flags & BCM_L3_COPY_TO_CPU) {
        data->flags |= BCM_IPMC_COPYTOCPU;
    }
    if (l3_fib.flags & BCM_L3_DST_DISCARD) {
        data->flags |= BCM_IPMC_DISCARD;
    }
    if (l3_fib.flags & BCM_L3_HIT) {
        data->flags |= BCM_IPMC_HIT;
    }
    if (l3_fib.int_flags & BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_TOCPU) {
        data->flags |= BCM_IPMC_RPF_FAIL_TOCPU;
    }

    if (l3_fib.int_flags & BCMI_LTSW_L3_FIB_F_IPMC_RPF_FAIL_DROP) {
        data->flags |= BCM_IPMC_RPF_FAIL_DROP;
    }

    if (l3_fib.int_flags & BCMI_LTSW_L3_FIB_F_IPMC_SOURCE_PORT_NOCHECK) {
        data->flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
    }

    if (l3_fib.int_flags & BCMI_LTSW_L3_FIB_F_IPMC_POST_LOOKUP_RPF_CHECK) {
        data->flags |= BCM_IPMC_POST_LOOKUP_RPF_CHECK;
    }

    data->port_tgid = l3_fib.port_tgid;
    SHR_IF_ERR_EXIT
        (ipmc_gport_construct(unit, data));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_ipmc_flexctr_hit_get(unit, l3_fib.class_id,
                                        l3_fib.flex_ctr_action_index,
                                        hit_clr, &hit_flags));
    data->flags |= hit_flags;

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Go through all valid IPMC entries, and call the callback function
 *        at each entry.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Used to decide IPMC entry type.
 * \param [in] cb User supplied callback function.
 * \param [in] user_data User supplied cookie used as parameter
 *                       in callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_traverse(int unit, uint32 flags,
                       bcm_ipmc_traverse_cb cb, void *user_data)
{
    uint32_t attrib = BCMI_LTSW_L3_FIB_ATTR_MC;
    bool ipmc_locked = false;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    IPMC_INIT_CHECK(unit);

    /* Input parameters check. */
    if (cb == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }


    IPMC_LOCK(unit);
    ipmc_locked = true;

    rv = mbcm_ltsw_ipmc_traverse_l2(unit, flags, cb, user_data);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    IPMC_UNLOCK(unit);
    ipmc_locked = false;

    if (flags & BCM_IPMC_L2) {
        SHR_EXIT();
    }

    if (flags & BCM_IPMC_IP6) {
        attrib |= BCMI_LTSW_L3_FIB_ATTR_IPV6;
    } else {
        attrib |= BCMI_LTSW_L3_FIB_ATTR_IPV4;
    }
    attrib |=  (BCMI_LTSW_L3_FIB_ATTR_HOST |
                BCMI_LTSW_L3_FIB_ATTR_NETWORK |
                BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP |
                BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_fib_mc_traverse(unit, attrib, cb,
                                      bcmi_ltsw_ipmc_l3_fib_to_ipmc,
                                      user_data));
exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Age on IPMC routing table, clear HIT bit of entry if set.
 *
 * \param [in] unit Unit number.
 * \param [in] flags IPMC entry flags.
 * \param [in] age_out User supplied callback function.
 * \param [in] user_data  User provided cookie for callback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_age(int unit, uint32 flags, bcm_ipmc_traverse_cb age_out,
                  void *user_data)
{
    uint32_t attrib = BCMI_LTSW_L3_FIB_ATTR_MC;
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_INIT_CHECK(unit);

    IPMC_LOCK(unit);
    ipmc_locked = true;

    if (flags & BCM_IPMC_IP6) {
        attrib |= BCMI_LTSW_L3_FIB_ATTR_IPV6;
    } else {
        attrib |= BCMI_LTSW_L3_FIB_ATTR_IPV4;
    }

    attrib |= BCMI_LTSW_L3_FIB_ATTR_HOST |
              BCMI_LTSW_L3_FIB_ATTR_NETWORK |
              BCMI_LTSW_L3_FIB_ATTR_SRC_GROUP |
              BCMI_LTSW_L3_FIB_ATTR_STAR_GROUP;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_fib_mc_age(unit, attrib, age_out,
                                 bcmi_ltsw_ipmc_l3_fib_to_ipmc, user_data));

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Reinitialize/clear the egress IP multicast configuration.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_egress_port_init(int unit)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    bcm_mac_t mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    BCM_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_ipmc_egress_port_set(unit, port, mac, 0, 0, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Config the egress IP multicast Configuration Register.
 *
 * \param [in] unit Unit number.
 * \param [in] port port to config.
 * \param [in] mac MAC address.
 * \param [in] untag 1: The IP multicast packet is transmitted as untagged packet.
 *                   0: The IP multicast packet is transmitted as tagged packet
 *                      with VLAN tag vid.
 *
 * \param [in] vid VLAN ID.
 * \param [in] ttl_thresh Drop IPMC packets if TTL of IP packet <= ttl_thresh.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_egress_port_set(int unit, bcm_port_t port, const bcm_mac_t mac,
                              int untag, bcm_vlan_t vid, int ttl_threshold)
{
    SHR_FUNC_ENTER(unit);

    if (!VALID_VLAN_ID(vid) && vid != BCM_VLAN_NONE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (!BCM_TTL_VALID(ttl_threshold)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ipmc_egress_port_set(unit, port, mac, untag, vid,
                                        ttl_threshold));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the egress IP multicast Configuration Register.
 *
 * \param [in] unit Unit number.
 * \param [in] port port to config.
 * \param [out] mac MAC address.
 * \param [out] untag 1: The IP multicast packet is transmitted as untagged packet.
 *                    0: The IP multicast packet is transmitted as tagged packet
 *                       with VLAN tag vid.
 *
 * \param [out] vid VLAN ID.
 * \param [out] ttl_thresh Drop IPMC packets if TTL of IP packet <= ttl_thresh.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_egress_port_get(int unit, bcm_port_t port, bcm_mac_t mac,
                              int *untag, bcm_vlan_t *vid, int *ttl_threshold)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_ipmc_egress_port_get(unit, port, mac, untag, vid,
                                        ttl_threshold));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a rendezvous point.
 *
 * \param [in] unit Unit number.
 * \param [in] flags BCM_IPMC_RP_xxx flags.
 * \param [in/out] rp_id Retunred rendezvous point ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_rp_create(int unit, uint32 flags, int *rp_id)
{
    int i;
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_RP_INIT_CHECK(unit);

    if (rp_id == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    IPMC_LOCK(unit);
    ipmc_locked = true;

    if (flags & BCM_IPMC_RP_WITH_ID) {
        IPMC_RP_ID_CHECK(unit, *rp_id);
        if (SHR_BITGET(IPMC_INFO(unit)->rp_bmp, *rp_id)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
        SHR_BITSET(IPMC_INFO(unit)->rp_bmp, *rp_id);
    } else {
        for (i = IPMC_INFO(unit)->rp_min; i <= IPMC_INFO(unit)->rp_max; i++) {
            if (0 == SHR_BITGET(IPMC_INFO(unit)->rp_bmp, i)) {
                SHR_BITSET(IPMC_INFO(unit)->rp_bmp, i);
                *rp_id = i;
                break;
            }
        }
        if (i > IPMC_INFO(unit)->rp_max) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a rendezvous point.
 *
 * \param [in] unit Unit number.
 * \param [in] rp_id Rendezvous point ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_rp_destroy(int unit, int rp_id)
{
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_RP_INIT_CHECK(unit);

    IPMC_RP_ID_CHECK(unit, rp_id);

    IPMC_LOCK(unit);
    ipmc_locked = true;

    if (0 == SHR_BITGET(IPMC_INFO(unit)->rp_bmp, rp_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_EXIT
        (bcm_ltsw_ipmc_rp_delete_all(unit, rp_id));

    SHR_BITCLR(IPMC_INFO(unit)->rp_bmp, rp_id);

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set active L3 interfaces for a rendezvous point.
 *
 * \param [in] unit Unit number.
 * \param [in] rp_id Rendezvous point ID.
 * \param [in] intf_count Number of elements in intf_array.
 * \param [in] intf_array Array of active L3 interfaces.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_rp_set(int unit, int rp_id,
                     int intf_count, bcm_if_t *intf_array)
{
    ltsw_ipmc_l3_iif_t *cur_ptr, *pre_ptr;
    SHR_BITDCL *intf_bmp = NULL;
    SHR_BITDCL *active_bmp = NULL;
    int l3_iif_min, l3_iif_max, num_l3_iif;
    int i;
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_RP_INIT_CHECK(unit);

    IPMC_RP_ID_CHECK(unit, rp_id);

    IPMC_LOCK(unit);
    ipmc_locked = true;

    if (0 == SHR_BITGET(IPMC_INFO(unit)->rp_bmp, rp_id)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (intf_count > 0 && intf_array == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_ing_intf_id_range_get(unit, &l3_iif_min, &l3_iif_max));
    num_l3_iif = l3_iif_max - l3_iif_min + 1;
    SHR_ALLOC(intf_bmp, SHR_BITALLOCSIZE(num_l3_iif), "ltswIpmcL3IifBmp");
    SHR_NULL_CHECK(intf_bmp, SHR_E_MEMORY);
    sal_memset(intf_bmp, 0, SHR_BITALLOCSIZE(num_l3_iif));
    for (i = 0; i < intf_count; i++) {
        SHR_IF_ERR_EXIT(
            bcmi_ltsw_l3_ing_intf_id_validate(unit, intf_array[i]));
        SHR_BITSET(intf_bmp, intf_array[i]);
    }

    /*
     * Traverse RP's existing linked list of active L3 IIFs.
     * Delete the L3 IIFs that are no longer active, i.e. not in intf_array.
     * Keep track of L3 IIFs that remain active in active_bmp.
     */
    SHR_ALLOC(active_bmp, SHR_BITALLOCSIZE(num_l3_iif), "ltswIpmcL3IifBmp");
    SHR_NULL_CHECK(active_bmp, SHR_E_MEMORY);
    sal_memset(active_bmp, 0, SHR_BITALLOCSIZE(num_l3_iif));
    pre_ptr = NULL;
    cur_ptr = IPMC_RP(unit, rp_id)->l3_iif_list;
    while (NULL != cur_ptr) {
        if (SHR_BITGET(intf_bmp, cur_ptr->l3_iif)) {
            /* This L3 IIF remains active. */
            SHR_BITSET(active_bmp, cur_ptr->l3_iif);
            pre_ptr = cur_ptr;
            cur_ptr = cur_ptr->next;
        } else {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_l3_ing_intf_rp_delete(unit, cur_ptr->l3_iif, rp_id));

            if (cur_ptr == IPMC_RP(unit, rp_id)->l3_iif_list) {
                IPMC_RP(unit, rp_id)->l3_iif_list = cur_ptr->next;
                SHR_FREE(cur_ptr);
                cur_ptr = IPMC_RP(unit, rp_id)->l3_iif_list;
            } else {
                pre_ptr->next = cur_ptr->next;
                SHR_FREE(cur_ptr);
                cur_ptr = pre_ptr->next;
            }
        }
    }

    /* Add the L3 IIFs that are not active, i.e. not in active_bitmap. */
    for (i = 0; i < intf_count; i++) {
        if (SHR_BITGET(active_bmp, intf_array[i])) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_ing_intf_rp_add(unit, intf_array[i], rp_id));

        cur_ptr = NULL;
        SHR_ALLOC(cur_ptr, sizeof(ltsw_ipmc_l3_iif_t), "ltswIpmcL3Iif");
        SHR_NULL_CHECK(cur_ptr, SHR_E_MEMORY);
        cur_ptr->l3_iif = intf_array[i];
        cur_ptr->next = IPMC_RP(unit, rp_id)->l3_iif_list;
        IPMC_RP(unit, rp_id)->l3_iif_list = cur_ptr;
    }

exit:
    SHR_FREE(intf_bmp);
    SHR_FREE(active_bmp);
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get active L3 interfaces for a rendezvous point.
 *
 * \param [in] unit Unit number.
 * \param [in] rp_id Rendezvous point ID.
 * \param [in] intf_max Maximum number of elements in intf_array.
 * \param [out] intf_array Array of active L3 interfaces.
 * \param [out] intf_count Number of elements returned in intf_array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_rp_get(int unit, int rp_id,
                     int intf_max, bcm_if_t *intf_array, int *intf_count)
{
    ltsw_ipmc_l3_iif_t *cur_ptr;
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_RP_INIT_CHECK(unit);

    IPMC_RP_ID_CHECK(unit, rp_id);

    IPMC_LOCK(unit);
    ipmc_locked = true;

    if (0 == SHR_BITGET(IPMC_INFO(unit)->rp_bmp, rp_id)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if ((intf_max > 0 && intf_array == NULL) ||
        (intf_max == 0 && intf_array != NULL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (intf_count == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *intf_count = 0;
    cur_ptr = IPMC_RP(unit, rp_id)->l3_iif_list;
    while (NULL != cur_ptr) {
        if (NULL != intf_array) {
            intf_array[*intf_count] = cur_ptr->l3_iif;
        }
        (*intf_count)++;
        if (*intf_count == intf_max) {
            break;
        }
        cur_ptr = cur_ptr->next;
    }

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an active L3 interface to a rendezvous point.
 *
 * \param [in] unit Unit number.
 * \param [in] rp_id Rendezvous point ID.
 * \param [in] intf_id L3 interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_rp_add(int unit, int rp_id, bcm_if_t intf_id)
{
    ltsw_ipmc_l3_iif_t *cur_ptr = NULL, *iif_ptr = NULL;
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_RP_INIT_CHECK(unit);

    IPMC_RP_ID_CHECK(unit, rp_id);

    IPMC_LOCK(unit);
    ipmc_locked = true;

    if (0 == SHR_BITGET(IPMC_INFO(unit)->rp_bmp, rp_id)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT(
        bcmi_ltsw_l3_ing_intf_id_validate(unit, intf_id));

    

    iif_ptr = IPMC_RP(unit, rp_id)->l3_iif_list;
    while (NULL != iif_ptr) {
        if (iif_ptr->l3_iif == intf_id) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        } else {
            iif_ptr = iif_ptr->next;
        }
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_ing_intf_rp_add(unit, intf_id, rp_id));

    /* Insert intf_id to head of linked list of L3 IIFs. */
    SHR_ALLOC(cur_ptr, sizeof(ltsw_ipmc_l3_iif_t), "ltswIpmcL3Iif");
    SHR_NULL_CHECK(cur_ptr, SHR_E_MEMORY);
    cur_ptr->l3_iif = intf_id;
    cur_ptr->next = IPMC_RP(unit, rp_id)->l3_iif_list;
    IPMC_RP(unit, rp_id)->l3_iif_list = cur_ptr;

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an active L3 interface from a rendezvous point.
 *
 * \param [in] unit Unit number.
 * \param [in] rp_id Rendezvous point ID.
 * \param [in] intf_id L3 interface ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_rp_delete(int unit, int rp_id, bcm_if_t intf_id)
{
    ltsw_ipmc_l3_iif_t *cur_ptr, *pre_ptr;
    bool ipmc_locked = false;

    SHR_FUNC_ENTER(unit);

    IPMC_RP_INIT_CHECK(unit);

    IPMC_RP_ID_CHECK(unit, rp_id);

    IPMC_LOCK(unit);
    ipmc_locked = true;

    if (0 == SHR_BITGET(IPMC_INFO(unit)->rp_bmp, rp_id)) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT(
        bcmi_ltsw_l3_ing_intf_id_validate(unit, intf_id));

    pre_ptr = NULL;
    cur_ptr = IPMC_RP(unit, rp_id)->l3_iif_list;
    while (cur_ptr != NULL) {
        if (cur_ptr->l3_iif == intf_id) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_l3_ing_intf_rp_delete(unit, intf_id, rp_id));

            if (cur_ptr == IPMC_RP(unit, rp_id)->l3_iif_list) {
                IPMC_RP(unit, rp_id)->l3_iif_list = cur_ptr->next;
            } else {
                /*
                 * Coverity thinks the prev_ptr pointer may still be NULL
                 * when dereferenced. This situation will never occur
                 * because if current_ptr is not pointing to the head of
                 * the linked list, prev_ptr would not be NULL.
                 */
                /* coverity[var_deref_op : FALSE] */
                pre_ptr->next = cur_ptr->next;
            }
            SHR_FREE(cur_ptr);
            SHR_EXIT();
        } else {
            pre_ptr = cur_ptr;
            cur_ptr = cur_ptr->next;
        }
    }
    if (cur_ptr == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    if (ipmc_locked) {
        IPMC_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all active L3 interfaces from a rendezvous point.
 *
 * \param [in] unit Unit number.
 * \param [in] rp_id Rendezvous point ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_rp_delete_all(int unit, int rp_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_ipmc_rp_set(unit, rp_id, 0, NULL));

exit:
    SHR_FUNC_EXIT();
}

void
bcmi_ltsw_ipmc_sw_dump(int unit)
{
    ltsw_ipmc_info_t *ipmc_i = IPMC_INFO(unit);
    int i;
    ltsw_ipmc_l3_iif_t *l3_iif_ptr;

    if (!ipmc_i->inited) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information IPMC - Unit %d\n"),
             unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Init : %d\n"),
             ipmc_i->inited));
    LOG_CLI((BSL_META_U(unit,
                        "  Minimum number of Rendezvous Point : %d\n"),
             ipmc_i->rp_min));
    LOG_CLI((BSL_META_U(unit,
                        "  Maximum number of Rendezvous Point : %d\n"),
             ipmc_i->rp_max));
    /* Just show the in-use RP. */
    if (!ltsw_feature(unit, LTSW_FT_PIM_BIDIR)) {
        return;
    }
    for (i = ipmc_i->rp_min; i <= ipmc_i->rp_max; i++) {
        if (SHR_BITGET(IPMC_INFO(unit)->rp_bmp, i)) {
            LOG_CLI((BSL_META_U(unit,
                        "  RP ID : %d\n"), i));
            LOG_CLI((BSL_META_U(unit,
                        "      L3_IIF list : \n")));
            l3_iif_ptr = IPMC_RP(unit, i)->l3_iif_list;
            while (l3_iif_ptr != NULL) {
                LOG_CLI((BSL_META_U(unit,
                        "          L3_IIF ID : %d\n"), l3_iif_ptr->l3_iif));
                l3_iif_ptr = l3_iif_ptr->next;
            }
        }
    }
}

/*!
 * \brief Attach counter entries to the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] info IPMC entry info.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_ipmc_stat_attach(int unit, bcm_ipmc_addr_t *info,
                          uint32 stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bcmi_ltsw_l3_fib_t l3_fib = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ipmc_addr_to_l3_fib_key(unit, info, &l3_fib));

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    if (ci.source != bcmFlexctrSourceIpmc) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    pre_ci.direction = ci.direction;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_fib_mc_flexctr_info_get(unit, &l3_fib, &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_l3_fib_mc_flexctr_attach(unit, &l3_fib, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries from the given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] info IPMC entry info.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_ipmc_stat_detach(int unit, bcm_ipmc_addr_t *info)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    bcmi_ltsw_l3_fib_t l3_fib = {0};
    uint32_t ctr_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ipmc_addr_to_l3_fib_key(unit, info, &l3_fib));

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_fib_mc_flexctr_info_get(unit, &l3_fib, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_l3_fib_mc_flexctr_detach(unit, &l3_fib));

    if (!SHR_FUNC_ERR()) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit, ctr_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter ID associated with given IPMC group.
 *
 * \param [in] unit Unit number.
 * \param [in] info IPMC entry info.
 * \param [in] type Type of the counter to retrieve.
 * \param [out] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_ipmc_stat_id_get(int unit, bcm_ipmc_addr_t *info,
                         bcm_ipmc_stat_t stat,
                         uint32 *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    bcmi_ltsw_l3_fib_t l3_fib = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (ipmc_addr_to_l3_fib_key(unit, info, &l3_fib));

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_fib_mc_flexctr_info_get(unit, &l3_fib, &ci));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set flex counter object value for the given IPMC route.
 *
 * \param [in] unit Unit number.
 * \param [in] info IPMC route info.
 * \param [in] value The flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_flexctr_object_set(int unit, bcm_ipmc_addr_t *info,
                                 uint32_t value)
{
    bcmi_ltsw_l3_fib_t l3_fib = {0};

    SHR_FUNC_ENTER(unit);

    IPMC_INIT_CHECK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (ipmc_addr_to_l3_fib_key(unit, info, &l3_fib));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_ipmc_flexctr_object_set(unit, &l3_fib, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex counter object value for the given IPMC route.
 *
 * \param [in] unit Unit Number.
 * \param [in] info IPMC route info.
 * \param [out] value Flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_ipmc_flexctr_object_get(int unit, bcm_ipmc_addr_t *info,
                                  uint32_t *value)
{
    bcmi_ltsw_l3_fib_t l3_fib = {0};

    SHR_FUNC_ENTER(unit);

    IPMC_INIT_CHECK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (ipmc_addr_to_l3_fib_key(unit, info, &l3_fib));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_ipmc_flexctr_object_get(unit, &l3_fib, value));

exit:
    SHR_FUNC_EXIT();
}

