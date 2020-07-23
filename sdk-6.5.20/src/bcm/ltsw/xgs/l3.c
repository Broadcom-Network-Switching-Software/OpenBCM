/*! \file l3.c
 *
 * XGS L3 Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/control.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/xgs/l3.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/types.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/l3_int.h>
#include <bcm_int/ltsw/l3_egress_int.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/util.h>
#include <bcm/switch.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

typedef struct l3_info_s {
    /* L3 module initialized. */
    uint8_t inited;

    /* L3 Mutex. */
    sal_mutex_t mutex;

    /* Minimum v4 option profile index. */
    uint32_t v4_option_profile_min;

    /* Maximum v4 option profile index. */
    uint32_t v4_option_profile_max;

    /* Entries per v4 option profile. */
    uint32_t v4_option_profile_size;

    /* HW idx for v4 option profile. */
    uint32_t *v4_option_profile_hw_idx;

    /* Bitmap of free v4 option profile index. */
    SHR_BITDCL *v4_option_profile_fbmp;

    /* Minimum v6 prefix map index. */
    uint32_t v6_prefix_map_min;

    /* Maximum v6 prefix map index. */
    uint32_t v6_prefix_map_max;

    /* Bitmap of free v6 prefix map index. */
    SHR_BITDCL *v6_prefix_map_fbmp;
} l3_info_t;

static l3_info_t l3_info[BCM_MAX_NUM_UNITS] = {{0}};

#define L3_INITED(_u) (l3_info[_u].inited)

#define L3_LOCK(_u)     \
    sal_mutex_take(l3_info[_u].mutex, SAL_MUTEX_FOREVER)

#define L3_UNLOCK(_u)   \
    sal_mutex_give(l3_info[_u].mutex)

#define L3_IPV4_OPTION_PROFILE_MIN(_u) l3_info[_u].v4_option_profile_min
#define L3_IPV4_OPTION_PROFILE_MAX(_u) l3_info[_u].v4_option_profile_max
#define L3_IPV4_OPTION_PROFILE_TBL_SZ(_u) (L3_IPV4_OPTION_PROFILE_MAX(_u) - L3_IPV4_OPTION_PROFILE_MIN(_u) + 1)
#define L3_IPV4_OPTION_PROFILE_VALID(_u, _id)       \
    (((_id) >= L3_IPV4_OPTION_PROFILE_MIN(_u)) && ((_id) <= L3_IPV4_OPTION_PROFILE_MAX(_u)))

#define L3_IPV4_OPTION_PROFILE_SIZE(_u) l3_info[_u].v4_option_profile_size
#define L3_IPV4_OPTION_PROFILE_HW_IDX(_u, _id) l3_info[_u].v4_option_profile_hw_idx[_id]

#define L3_IPV4_OPTION_PROFILE_FBMP_GET(_u, _id)    \
        SHR_BITGET(l3_info[_u].v4_option_profile_fbmp, (_id))

#define L3_IPV4_OPTION_PROFILE_FBMP_SET(_u, _id)    \
        SHR_BITSET(l3_info[_u].v4_option_profile_fbmp, (_id))

#define L3_IPV4_OPTION_PROFILE_FBMP_CLR(_u, _id)    \
        SHR_BITCLR(l3_info[_u].v4_option_profile_fbmp, (_id))


#define L3_IPV6_PREFIX_MAP_MIN(_u) l3_info[_u].v6_prefix_map_min
#define L3_IPV6_PREFIX_MAP_MAX(_u) l3_info[_u].v6_prefix_map_max
#define L3_IPV6_PREFIX_MAP_TBL_SZ(_u) (L3_IPV6_PREFIX_MAP_MAX(_u) - L3_IPV6_PREFIX_MAP_MIN(_u) + 1)
#define L3_IPV6_PREFIX_MAP_VALID(_u, _id)       \
    (((_id) >= L3_IPV6_PREFIX_MAP_MIN(_u)) && ((_id) <= L3_IPV6_PREFIX_MAP_MAX(_u)))
#define L3_IPV6_PREFIX_MAP_FBMP_GET(_u, _id)    \
            SHR_BITGET(l3_info[_u].v6_prefix_map_fbmp, (_id))

#define L3_IPV6_PREFIX_MAP_FBMP_SET(_u, _id)    \
            SHR_BITSET(l3_info[_u].v6_prefix_map_fbmp, (_id))

#define L3_IPV6_PREFIX_MAP_FBMP_CLR(_u, _id)    \
            SHR_BITCLR(l3_info[_u].v6_prefix_map_fbmp, (_id))

const char *ecmp_hash_mask_str[] = {LSB_10BITSs, LSB_11BITSs,
                                    LSB_12BITSs, LSB_13BITSs,
                                    LSB_14BITSs, LSB_15BITSs,
                                    LSB_16BITSs};
const char *ecmp_hash_alg_str[] = {ZEROs, CRC32Us, CRC32Ls, LSBs};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Add an entry into L3_MTU table.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Profile entry index.
 * \param [in] mtu Mtu size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_set(int unit, int index, int mtu, l3_mtu_type type)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    if (type == L3_MTU_TYPE_UC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_UC_MTUs, &eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, VLANs, index));
    } else if (type == L3_MTU_TYPE_MC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_MC_MTUs, &eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TM_MC_GROUP_IDs, index));
    } else if (type == L3_MTU_TYPE_TUNNEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_UC_TNL_MTUs, &eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, L3_EIF_IDs, index));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, L3_MTUs, mtu));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from L3_MTU table.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Profile entry index.
 * \param [out] mtu Mtu size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_get(int unit, int index, l3_mtu_type type, int *mtu)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    if (type == L3_MTU_TYPE_UC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_UC_MTUs, &eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, VLANs, index));
    } else if (type == L3_MTU_TYPE_MC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_MC_MTUs, &eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TM_MC_GROUP_IDs, index));
    } else if (type == L3_MTU_TYPE_TUNNEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_UC_TNL_MTUs, &eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, L3_EIF_IDs, index));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, L3_MTUs, &value));

    *mtu = (int)value;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from L3_MTU table.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Profile entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_delete(int unit, int index, l3_mtu_type type)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t mtu_min, mtu_max;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    if (type == L3_MTU_TYPE_UC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_UC_MTUs, &eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, VLANs, index));
    } else if (type == L3_MTU_TYPE_MC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_MC_MTUs, &eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TM_MC_GROUP_IDs, index));
    } else if (type == L3_MTU_TYPE_TUNNEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, L3_UC_TNL_MTUs, &eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, L3_EIF_IDs, index));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
    (bcmi_lt_field_value_range_get(unit, L3_UC_TNL_MTUs, L3_MTUs,
                                   &mtu_min, &mtu_max));

    SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, L3_MTUs, mtu_max));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the information of L3 MTU.
 *
 * This function is used to clear the information of L3 MTU.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_UC_MTUs));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_UC_TNL_MTUs));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_MC_MTUs));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Clear the information of L3 MTU.
 *
 * This function is used to clear the information of L3 MTU.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_init(int unit)
{
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    if (!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_mtu_clear(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the information of L3 MTU.
 *
 * This function is used to initialize the information of L3 MTU.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_default_set(int unit)
{
    uint64_t idx_min, idx_max;
    uint64_t mtu_min, mtu_max;
    uint64_t index;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;

    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, L3_UC_TNL_MTUs, L3_EIF_IDs,
                                           &idx_min, &idx_max));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, L3_UC_TNL_MTUs, L3_MTUs,
                                           &mtu_min, &mtu_max));

        for (index = idx_min; index <= idx_max; index++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, L3_UC_TNL_MTUs, &eh));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, L3_EIF_IDs, index));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, L3_MTUs, mtu_max));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT,
                                      BCMLT_PRIORITY_NORMAL));
            (void)bcmlt_entry_free(eh);
            eh = BCMLT_INVALID_HDL;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, L3_MC_MTUs, TM_MC_GROUP_IDs,
                                           &idx_min, &idx_max));

        for (index = idx_min; index <= idx_max; index++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, L3_MC_MTUs, &eh));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, TM_MC_GROUP_IDs, index));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, L3_MTUs, mtu_max));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT,
                                      BCMLT_PRIORITY_NORMAL));
            (void)bcmlt_entry_free(eh);
            eh = BCMLT_INVALID_HDL;
        }

    }
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve a L3_IP_OPTION_CONTROL_PROFILE profile.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_idx Profile Index.
 * \param [in] offset Offset in a profile.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_ipv4_options_profile_entry_get(
    int unit,
    int profile_idx,
    int offset,
    bcmi_ltsw_l3_ipv4_options_profile_t *prof_entry)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IP_OPTION_CONTROL_PROFILEs, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, L3_IP_OPTION_CONTROL_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PKT_IP_OPTION_NUMBERs, offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, DROPs, &data));
    prof_entry->drop = (bool) data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, COPY_TO_CPUs, &data));
    prof_entry->copy_to_cpu = (bool) data;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a L3_IP_OPTION_CONTROL_PROFILE profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_idx Profile Index.
  * \param [in] offset Offset in the profile.
 * \param [out] prof_entry Profile entry returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_ipv4_options_profile_entry_set(
    int unit,
    int profile_idx,
    int offset,
    bcmi_ltsw_l3_ipv4_options_profile_t *prof_entry)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(prof_entry, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IP_OPTION_CONTROL_PROFILEs, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, L3_IP_OPTION_CONTROL_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PKT_IP_OPTION_NUMBERs, offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, DROPs,
                               prof_entry->drop));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, COPY_TO_CPUs,
                               prof_entry->copy_to_cpu));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a L3_IP_OPTION_CONTROL_PROFILE profile entry.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_idx Profile Index.
 * \param [in] offset Offset in the profile.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_ipv4_options_profile_entry_del(
    int unit,
    int profile_idx,
    int offset)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IP_OPTION_CONTROL_PROFILEs, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, L3_IP_OPTION_CONTROL_PROFILE_IDs, profile_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PKT_IP_OPTION_NUMBERs, offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Add L3_IP_OPTION_CONTROL_PROFILE profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entry L3_IP_OPTION_CONTROL_PROFILE entry.
 * \param [out] index Base index of L3_IP_OPTION_CONTROL_PROFILE.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ipv4_options_profile_add(
    int unit,
    bcmi_ltsw_l3_ipv4_options_profile_t *entry,
    int *index)
{
    int rv, i;
    int entries_per_profile;
    int profile_idx;

    SHR_FUNC_ENTER(unit);

    entries_per_profile = L3_IPV4_OPTION_PROFILE_SIZE(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(index, SHR_E_PARAM);

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_L3_IPV4_OPTION_CONTROL,
                                          entry, 0, entries_per_profile, index);
    if (rv == SHR_E_EXISTS) {
        /* the same profile already exists */
        SHR_EXIT();
    }

    if (rv == SHR_E_FULL) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (SHR_FAILURE(rv)) {
       SHR_ERR_EXIT(rv);
    }

    profile_idx = *index / entries_per_profile;

    for (i  = 0; i < entries_per_profile; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ipv4_options_profile_entry_set(unit, profile_idx, i, &entry[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get entries from LT L3_IP_OPTION_CONTROL_PROFILE.
 *
 * This function is used to get entries from LT L3_IP_OPTION_CONTROL_PROFILE.
 *
 * \param [in]  unit                 Unit Number.
 * \param [in] index               Profile base index.
 * \param [out] prof_entry      Profile entries.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
l3_ipv4_options_profile_get(int unit,
                            int index,
                            bcmi_ltsw_l3_ipv4_options_profile_t *prof_entry)
{
    int i;
    int entries_per_profile;
    int profile_idx;

    SHR_FUNC_ENTER(unit);
    entries_per_profile = L3_IPV4_OPTION_PROFILE_SIZE(unit);
    profile_idx = index / entries_per_profile;

    for (i  = 0; i < entries_per_profile; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ipv4_options_profile_entry_get(unit, profile_idx,
                                               i, &prof_entry[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete entries from LT L3_IP_OPTION_CONTROL_PROFILE.
 *
 * \param [in] unit Unit number.
 * \param [in] index Base Index of LT L3_IP_OPTION_CONTROL_PROFILE.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ipv4_options_profile_delete(
    int unit,
    int index)
{
    int rv;
    int i;
    int entries_per_profile;
    int profile_idx;

    SHR_FUNC_ENTER(unit);

    entries_per_profile = L3_IPV4_OPTION_PROFILE_SIZE(unit);
    profile_idx = index / entries_per_profile;

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_L3_IPV4_OPTION_CONTROL,
                                      index);

    if (rv == SHR_E_BUSY) {
        /* profile entry is still inused */
       SHR_EXIT();
    }

    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    /* empty the profile entry */
    for (i  = 0; i < entries_per_profile; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
        (l3_ipv4_options_profile_entry_del(unit, profile_idx, i));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash callback function for L3_IP_OPTION_CONTROL_PROFILE profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries L3  OPTION_CONTROL profile entries.
 * \param [in] entries_per_set Number of entries per set.
 * \param [out] hash Hash value returned.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_ipv4_options_profile_entry_hash_cb(
    int unit,
    void *entries,
    int entries_per_set,
    uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (entries_per_set != L3_IPV4_OPTION_PROFILE_SIZE(unit)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(hash, SHR_E_PARAM);

    *hash = shr_crc32(0, entries,
                      entries_per_set * sizeof(bcmi_ltsw_l3_ipv4_options_profile_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Comparison callback function for L3_IP_OPTION_CONTROL_PROFILE profile.
 *
 * \param [in] unit Unit number.
 * \param [in] entries L3_IP_OPTION_CONTROL_PROFILE entry.
 * \param [in] entries_per_set Number of entries per set.
 * \param [in] profile_idx Profile Index of LT entry.
 * \param [out] cmp_result Comparison result.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
l3_ipv4_options_profile_entry_cmp_cb(
    int unit,
    void *entries,
    int entries_per_set,
    int profile_idx,
    int *cmp_result)
{
    int profile_size;
    bcmi_ltsw_l3_ipv4_options_profile_t prof_entry[BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK];

    SHR_FUNC_ENTER(unit);

    profile_size = L3_IPV4_OPTION_PROFILE_SIZE(unit);

    if (entries_per_set != profile_size) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_NULL_CHECK(entries, SHR_E_PARAM);
    SHR_NULL_CHECK(cmp_result, SHR_E_PARAM);

    sal_memset(prof_entry, 0, sizeof(prof_entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ipv4_options_profile_get(unit, profile_idx, prof_entry));

    *cmp_result = sal_memcmp(entries, prof_entry, sizeof(prof_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover LT L3_IP_OPTION_CONTROL_PROFILE..
 *
 * \param [in] unit Unit number.
 * \param [in] profile_idx Profile Index of L3_IP_OPTION_CONTROL_PROFILE.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ipv4_options_profile_recover(
    int unit,
    int profile_idx)
{
    uint32_t ref_cnt;
    uint32_t index;
    uint64_t idx_min, idx_max;
    bcmi_ltsw_l3_ipv4_options_profile_t profile_entries[BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK] = {{0}};
    int entries_per_set = L3_IPV4_OPTION_PROFILE_SIZE(unit);
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_L3_IPV4_OPTION_CONTROL;

    SHR_FUNC_ENTER(unit);

    index = profile_idx * entries_per_set;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IP_OPTION_CONTROL_PROFILEs,
                                       L3_IP_OPTION_CONTROL_PROFILE_IDs,
                                       &idx_min, &idx_max));

    if ((profile_idx < idx_min) || (profile_idx > idx_max)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, profile_hdl,
                                         index, &ref_cnt));
    if (ref_cnt == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ipv4_options_profile_get(unit, index, &profile_entries[0]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, profile_hdl, profile_entries,
                                           entries_per_set, index));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, profile_hdl,
                                              entries_per_set, index, 1));

exit:
    SHR_FUNC_EXIT();
}

static int
l3_ip4_options_default_profile_create(int unit)
{
    int i, id = BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX;
    bcmi_ltsw_l3_ipv4_options_profile_t ip_option_profile[BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK];
    bcmi_ltsw_l3_ipv4_options_profile_t *entry;
    bool drop, copy_cpu;
    int index;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);
    if (warm) {
        SHR_EXIT();
    }

    /* Reserve a chunk in the IP4_OPTION Profile table */
    sal_memset(ip_option_profile, 0, sizeof(ip_option_profile));
    copy_cpu = false;
    drop = true;

    for (i = 0; i < BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK; i++) {
        entry = &ip_option_profile[i];
        entry->copy_to_cpu = copy_cpu;
        entry->drop = drop;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ipv4_options_profile_add(unit, ip_option_profile, &index));

    if (L3_IPV4_OPTION_PROFILE_FBMP_GET(unit, id)) {
        L3_IPV4_OPTION_PROFILE_FBMP_CLR(unit, id);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    L3_IPV4_OPTION_PROFILE_HW_IDX(unit, id) = index;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Init LT L3_IP_OPTION_CONTROL_PROFILE information.
 *
 * This function is used to init LT L3_IP_OPTION_CONTROL_PROFILE information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
l3_ipv4_options_profile_init(int unit)
{
    uint32_t sz, alloc_sz;
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_L3_IPV4_OPTION_CONTROL;
    int ent_idx_min = 0;
    int ent_idx_max = 0;
    int entries_per_set;
    uint64_t min, max;
    l3_info_t *info = &l3_info[unit];
    int i, profile_cnt;

    SHR_FUNC_ENTER(unit);

    /* V4 option profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IP_OPTION_CONTROL_PROFILEs,
                                       PKT_IP_OPTION_NUMBERs, &min, &max));

    info->v4_option_profile_size = max + 1;

    entries_per_set = L3_IPV4_OPTION_PROFILE_SIZE(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IP_OPTION_CONTROL_PROFILEs,
                                       L3_IP_OPTION_CONTROL_PROFILE_IDs, &min, &max));
    info->v4_option_profile_min = (uint32_t)min;
    info->v4_option_profile_max = (uint32_t)max;
    sz = SHR_BITALLOCSIZE(max + 1);
    alloc_sz = sz;
    info->v4_option_profile_fbmp = bcmi_ltsw_ha_mem_alloc(unit,
                                                          BCMI_HA_COMP_ID_L3,
                                                          BCMINT_LTSW_L3_HA_IP_OPTION_FBMP,
                                                          "bcmL3IpOptionFbmp",
                                                          &alloc_sz);
    SHR_NULL_CHECK(info->v4_option_profile_fbmp, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);
    SHR_BITSET_RANGE(info->v4_option_profile_fbmp,
                     L3_IPV4_OPTION_PROFILE_MIN(unit), L3_IPV4_OPTION_PROFILE_TBL_SZ(unit));

    sz = ((uint32_t)max + 1) * sizeof(uint32_t);
    alloc_sz = sz;
    info->v4_option_profile_hw_idx = bcmi_ltsw_ha_mem_alloc(unit,
                                                            BCMI_HA_COMP_ID_L3,
                                                            BCMINT_LTSW_L3_HA_IP_OPTION_HW,
                                                            "bcmL3IpOptionHw",
                                                            &alloc_sz);

    SHR_NULL_CHECK(info->v4_option_profile_hw_idx, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    profile_cnt = L3_IPV4_OPTION_PROFILE_TBL_SZ(unit) / entries_per_set;


    /* Register profile. */
    if (bcmi_ltsw_profile_register_check(unit, profile_hdl)) {
        SHR_EXIT();
    }

    ent_idx_min = (int)min;
    ent_idx_max = ((int)max + 1) * entries_per_set - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &profile_hdl,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    l3_ipv4_options_profile_entry_hash_cb,
                                    l3_ipv4_options_profile_entry_cmp_cb));

    if (bcmi_warmboot_get(unit)) {
        for (i = 0; i < profile_cnt; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_ipv4_options_profile_recover(unit, i));
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!bcmi_warmboot_get(unit)) {
            if (info->v4_option_profile_hw_idx) {
                (void)bcmi_ltsw_ha_mem_free(unit, info->v4_option_profile_hw_idx);
            }
            if (info->v4_option_profile_fbmp) {
                (void)bcmi_ltsw_ha_mem_free(unit, info->v4_option_profile_fbmp);
            }
            (void)bcmi_ltsw_profile_unregister(unit, profile_hdl);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the information of L3 MTU.
 *
 * This function is used to clear the information of L3 MTU.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ipv4_options_profile_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_IP_OPTION_CONTROL_PROFILEs));
    }
exit:
    SHR_FUNC_EXIT();
}


/*
 * \brief Deinit LT L3_IP_OPTION_CONTROL_PROFILE information.
 *
 * This function is used to deinit LT L3_IP_OPTION_CONTROL_PROFILE information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
l3_ipv4_options_profile_deinit(int unit)
{
    bcmi_ltsw_profile_hdl_t profile_hdl = BCMI_LTSW_PROFILE_L3_IPV4_OPTION_CONTROL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ipv4_options_profile_clear(unit));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_unregister(unit, profile_hdl));

exit:
    SHR_FUNC_EXIT();
}

static int
l3_ipv4_option_profile_id_alloc(int unit)
{
    int i, option_profile_size = -1;

    option_profile_size = L3_IPV4_OPTION_PROFILE_SIZE(unit);
    for (i = 0; i < option_profile_size; i++) {
        if (L3_IPV4_OPTION_PROFILE_FBMP_GET(unit, i)) {
            return i;
        }
    }
    return -1;
}

/*!
 * \brief Recover LT L3_IPV6_PREFIX_TO_IPV4_MAP.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ip6_prefix_map_recover(
    int unit)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit;
    int rv = SHR_E_NONE;
    uint64_t id;
    uint64_t match;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IPV6_PREFIX_TO_IPV4_MAPs, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, L3_IPV6_PREFIX_TO_IPV4_MAP_IDs, &id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, MATCHs, &match));

        if (!match) {
            continue;
        }

        /* Don't recover from entry that not eligible for LT operation. */
        if ((id < L3_IPV6_PREFIX_MAP_MIN(unit)) ||
            (id > L3_IPV6_PREFIX_MAP_MAX(unit))) {
            continue;
        }

        L3_IPV6_PREFIX_MAP_FBMP_CLR(unit, id);
    }
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Init LT L3_IPV6_PREFIX_TO_IPV4_MAP information.
 *
 * This function is used to init LT L3_IPV6_PREFIX_TO_IPV4_MAP information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
l3_ip6_prefix_map_init(int unit)
{
    uint32_t sz;
    uint64_t min, max;
    l3_info_t *info = &l3_info[unit];

    SHR_FUNC_ENTER(unit);

    /* V6 Prefix map. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IPV6_PREFIX_TO_IPV4_MAPs,
                                       L3_IPV6_PREFIX_TO_IPV4_MAP_IDs, &min, &max));
    info->v6_prefix_map_min = min;
    info->v6_prefix_map_max = max;
    sz = SHR_BITALLOCSIZE(max + 1);
    SHR_ALLOC(info->v6_prefix_map_fbmp, sz, "bcmXgsL3IP6PrefixMapFbmp");
    SHR_NULL_CHECK(info->v6_prefix_map_fbmp, SHR_E_MEMORY);
    SHR_BITSET_RANGE(info->v6_prefix_map_fbmp,
                     L3_IPV6_PREFIX_MAP_MIN(unit), L3_IPV6_PREFIX_MAP_TBL_SZ(unit));

    if (bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ip6_prefix_map_recover(unit));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (info->v6_prefix_map_fbmp) {
            SHR_FREE(info->v6_prefix_map_fbmp);
        }
    }
    SHR_FUNC_EXIT();
}

/*
 * \brief Deinit LT L3_IPV6_PREFIX_TO_IPV4_MAP information.
 *
 * This function is used to deinit LT L3_IPV6_PREFIX_TO_IPV4_MAP information.
 *
 * \param [in]  unit                 Unit Number.
 *
 * \retval SHR_E_NONE                No errors.
 * \retval !SHR_E_NONE               Failure.
 */
static int
l3_ip6_prefix_map_deinit(int unit)
{
    l3_info_t *info = &l3_info[unit];

    SHR_FUNC_ENTER(unit);

    if (info->v6_prefix_map_fbmp) {
        SHR_FREE(info->v6_prefix_map_fbmp);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the information of L3 MTU.
 *
 * This function is used to clear the information of L3 MTU.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ip6_prefix_map_clear(int unit)
{
    l3_info_t *info = &l3_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_IPV6_PREFIX_TO_IPV4_MAPs));

        SHR_BITSET_RANGE(info->v6_prefix_map_fbmp,
                         L3_IPV6_PREFIX_MAP_MIN(unit),
                         L3_IPV6_PREFIX_MAP_TBL_SZ(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a list of IPv6 96 bit prefixes which are mapped to ipv4 lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] map_size Size of allocated entries in ip6_array.
 * \param [out] ip6_array Array of mapped prefixes.
 * \param [out] ip6_count Number of entries of ip6_array actually filled in.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ip6_prefix_map_get(int unit, int map_size,
                      bcm_ip6_t *ip6_array, int *ip6_count)
{
    int dunit;
    int rv;
    uint64_t v6[2];
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /*  Make sure module was initialized. */
    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    /* Input parameters sanity check. */
    if ((NULL == ip6_count) || ((NULL == ip6_array) && (0 != map_size))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *ip6_count = 0;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IPV6_PREFIX_TO_IPV4_MAPs, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, IPV6_PREFIX_UPPERs, &v6[1]));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, IPV6_PREFIX_LOWERs, &v6[0]));

        bcmi_ltsw_util_uint64_to_ip6((bcm_ip6_t *)ip6_array[*ip6_count], v6);

        (*ip6_count)++;

        if (*ip6_count > map_size) {
            break;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an IPv6 prefix map into IPv4 entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ip6_addr New IPv6 translation address.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ip6_prefix_map_add(int unit, bcm_ip6_t ip6_addr)
{
    int dunit;
    int rv;
    int i;
    uint64_t v6[2];
    uint64_t v6_in[2];
    uint64_t match;
    uint64_t id;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /*  Make sure module was initialized. */
    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    bcmi_ltsw_util_ip6_to_uint64(v6_in, (bcm_ip6_t *)ip6_addr);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IPV6_PREFIX_TO_IPV4_MAPs, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, L3_IPV6_PREFIX_TO_IPV4_MAP_IDs, &id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, MATCHs, &match));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, IPV6_PREFIX_UPPERs, &v6[1]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, IPV6_PREFIX_LOWERs, &v6[0]));

        if (!match) {
            continue;
        }

        if ((v6[0] == v6_in[0]) && (v6[1] == v6_in[1])) {
           SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }
    }

    for (i = 0; i < L3_IPV6_PREFIX_MAP_TBL_SZ(unit); i++) {
        if (L3_IPV6_PREFIX_MAP_FBMP_GET(unit, i)) {
            break;
        }
    }
    if (i >= L3_IPV6_PREFIX_MAP_TBL_SZ(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, L3_IPV6_PREFIX_TO_IPV4_MAP_IDs, i));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, IPV6_PREFIX_LOWERs, v6_in[0]));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, IPV6_PREFIX_UPPERs, v6_in[1]));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, MATCHs, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh,
                              BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
    L3_IPV6_PREFIX_MAP_FBMP_CLR(unit, i);
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy an IPv6 prefix map entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ip6_addr IPv6 translation address.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ip6_prefix_map_delete(int unit, bcm_ip6_t ip6_addr)
{
    int dunit;
    int rv;
    uint64_t v6[2];
    uint64_t v6_in[2];
    uint64_t match;
    uint64_t id;
    bool entry_hit = false;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /*  Make sure module was initialized. */
    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    bcmi_ltsw_util_ip6_to_uint64(v6_in, (bcm_ip6_t *)ip6_addr);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_IPV6_PREFIX_TO_IPV4_MAPs, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, L3_IPV6_PREFIX_TO_IPV4_MAP_IDs, &id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, MATCHs, &match));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, IPV6_PREFIX_UPPERs, &v6[1]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, IPV6_PREFIX_LOWERs, &v6[0]));

        if (!match) {
            continue;
        }

        if (0 == sal_memcmp(v6, v6_in, 12)) {
            entry_hit = true;
            break;
        }
    }

    if (entry_hit) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_clear(eh));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, L3_IPV6_PREFIX_TO_IPV4_MAP_IDs, id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh,
                                  BCMLT_OPCODE_DELETE, BCMLT_PRIORITY_NORMAL));
        L3_IPV6_PREFIX_MAP_FBMP_SET(unit, id);

    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Flush all IPv6 prefix maps.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ip6_prefix_map_delete_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ip6_prefix_map_clear(unit));
exit:
    SHR_FUNC_EXIT();
}
/******************************************************************************
 * Public Internal HSDK functions
 */
int
xgs_ltsw_l3_init(int unit)
{
    l3_info_t *info = &l3_info[unit];

    SHR_FUNC_ENTER(unit);

    if (info->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_l3_deinit(unit));
    }

    sal_memset(info, 0, sizeof(l3_info_t));

    info->mutex = sal_mutex_create("bcmXGSL3info");
    SHR_NULL_CHECK(info->mutex, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT(l3_mtu_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT(l3_mtu_default_set(unit));

    SHR_IF_ERR_VERBOSE_EXIT(l3_ipv4_options_profile_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT(l3_ip4_options_default_profile_create(unit));

    SHR_IF_ERR_VERBOSE_EXIT(l3_ip6_prefix_map_init(unit));

    info->inited = 1;

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (xgs_ltsw_l3_control_set(unit,
                                      bcmiL3ControlEcmpHashUseFlowSelEcmp,
                                      1), SHR_E_NOT_FOUND);
        SHR_IF_ERR_VERBOSE_EXIT
            (xgs_ltsw_l3_enable_set(0, 1));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (info->mutex) {
            sal_mutex_destroy(info->mutex);
        }
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_deinit(int unit)
{
    l3_info_t * info = &l3_info[unit];

    SHR_FUNC_ENTER(unit);

    if (info->inited) {
        SHR_NULL_CHECK(info->mutex, SHR_E_INIT);

        L3_LOCK(unit);
        info->inited = 0;
        L3_UNLOCK(unit);
        sal_mutex_destroy(info->mutex);

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_mtu_clear(unit));

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ipv4_options_profile_deinit(unit));

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ip6_prefix_map_deinit(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_mtu_set(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_mtu_set(unit, cfg->index, cfg->mtu, cfg->type));
exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_mtu_get(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);
    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_mtu_get(unit, cfg->index, cfg->type, &cfg->mtu));

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_mtu_delete(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_LOCK(unit);
    locked = true;
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_mtu_delete(unit, cfg->index, cfg->type));

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_info_get(int unit, bcm_l3_info_t *l3info)
{
    uint64_t idx_min, idx_max;
    uint32_t value, value1, value2;
    bool locked = false;
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id;
    const bcmint_ltsw_l3_egress_tbl_t *tbl_info = NULL;
    const char *ltname;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_LOCK(unit);
    locked = true;

    sal_memset(l3info, 0, sizeof(bcm_l3_info_t));

    /* VRF. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IIFs, VRF_IDs,
                                       &idx_min, &idx_max));
    l3info->l3info_max_vrf = idx_max;

    /* Egress interface. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, L3_EIFs, &value));
    l3info->l3info_max_intf = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, L3_EIFs, &value));
    l3info->l3info_used_intf = value;

    /* Route. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, L3_IPV4_UC_ROUTEs, &value));
    l3info->l3info_max_route = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, L3_IPV4_UC_ROUTEs, &value));
    l3info->l3info_used_route = value;

    /* Nexthop. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, L3_UC_NHOPs, &value));
    l3info->l3info_max_nexthop = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, L3_UC_NHOPs, &value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, TNL_MPLS_ENCAP_NHOPs, &value1));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, TNL_MPLS_TRANSIT_NHOPs, &value2));
    l3info->l3info_used_nexthop = value + value1 + value2;

    /* Tunnel init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TNL_IPV4_ENCAPs, &value));
    l3info->l3info_max_tunnel_init = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, TNL_IPV4_ENCAPs, &value));
    l3info->l3info_used_tunnel_init = value;

    /* Tunnel terminate. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, TNL_IPV4_DECAPs, &value));
    l3info->l3info_max_tunnel_term = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, TNL_IPV4_DECAPs, &value));
    l3info->l3info_used_tunnel_term = value;

    /* Interface group. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IIFs, CLASS_IDs,
                                       &idx_min, &idx_max));
    l3info->l3info_max_intf_group = (int)idx_max;

    /* ECMP info. */
    tbl_id = BCMINT_LTSW_L3_ECMP_TBL_UL;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

    ltname = tbl_info->name;

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, ltname, ECMP_IDs,
                                           &idx_min, &idx_max));
    l3info->l3info_max_ecmp_groups = (int)(idx_max - idx_min);

    if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        tbl_id = BCMINT_LTSW_L3_ECMP_TBL_OL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_l3_ecmp_tbl_get(unit, tbl_id, &tbl_info));

        ltname = tbl_info->name;

        SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_value_range_get(unit, ltname, ECMP_IDs,
                                               &idx_min, &idx_max));
        l3info->l3info_max_ecmp_groups += (int)(idx_max - idx_min + 1);
    }

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_enable_set(int unit, int enable)
{
    int port;
    bcm_pbmp_t port_pbmp;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &port_pbmp));

    /* Iterate over all the ports. */
    BCM_PBMP_ITER(port_pbmp, port) {
        (void)bcm_ltsw_port_control_set(unit, port, bcmPortControlIP4, enable);
        (void)bcm_ltsw_port_control_set(unit, port, bcmPortControlIP6, enable);
    }
exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_control_set(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t value)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    int rv;
    bool locked = false;
    int i;
    uint64_t field_val[MAX_FIELD_CNT];
    const char *field_symbol[MAX_FIELD_CNT];
    const bcmint_ltsw_l3_switch_control_tbl_t *ti = NULL;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_switch_control_tbl_get(unit, &ti));

    if (!ti[control].field_count) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    switch (control) {
        case bcmiL3ControlEcmpHashControl:
            field_val[0] = BCM_HASH_CONTROL_MULTIPATH_USERDEF_VAL(value);
            field_val[1] = (value & BCM_HASH_CONTROL_MULTIPATH_DIP) ? 1 : 0;
            field_val[2] = (value & BCM_HASH_CONTROL_ECMP_ENHANCE) ? 1 : 0;
            break;
        case bcmiL3ControlEcmpHashMultipath:
            if (value == BCM_HASH_ZERO) {
                field_symbol[0] = ecmp_hash_alg_str[0];
            } else if (value == BCM_HASH_LSB) {
                field_symbol[0] = ecmp_hash_alg_str[3];
            } else if (value == BCM_HASH_CRC32L) {
                field_symbol[0] = ecmp_hash_alg_str[2];
            } else if (value == BCM_HASH_CRC32U) {
                field_symbol[0] = ecmp_hash_alg_str[1];
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            break;
        case bcmiL3ControlEcmpHashBitCountSelect:
            if (value < 0 || value > 6) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            field_symbol[0] = ecmp_hash_mask_str[value];
            break;
        case bcmiL3ControlWESPProtocolEnable:
            if (value < 0 || value > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            field_val[0] = value;
            field_val[1] = value;
            break;
        case bcmiL3ControlWESPProtocol:
            if (value < 0 || value > 1) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            field_val[0] = value;
            field_val[1] = value;
            break;
        default:
            field_val[0] = value;
            break;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ti[control].ltname, &eh));

    for (i = 0; i < ti[control].field_count; i++) {
        if (!ti[control].field_symbol) {
            rv = bcmlt_entry_field_add(eh, ti[control].field_str[i],
                                       field_val[i]);
        } else {
            rv = bcmlt_entry_field_symbol_add(eh, ti[control].field_str[i],
                                              field_symbol[i]);
        }
        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        } else if (SHR_FAILURE(rv)){
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_control_get(
    int unit,
    bcmi_ltsw_l3_control_t control,
    uint32_t *value)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    int rv;
    bool locked = false;
    uint64_t field_value[MAX_FIELD_CNT];
    const char *field_symbol[MAX_FIELD_CNT];
    bcmlt_field_def_t fld_def[MAX_FIELD_CNT];
    const bcmint_ltsw_l3_switch_control_tbl_t *ti = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }
    if (!value) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_switch_control_tbl_get(unit, &ti));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, ti[control].ltname, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        for (i = 0; i < ti[control].field_count; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit, ti[control].ltname,
                                           ti[control].field_str[i], &fld_def[i]));
            if (!ti[control].field_symbol) {
                field_value[i] = fld_def[i].def;
            } else {
                field_symbol[i] = fld_def[i].sym_def;
            }
        }

        switch (control) {
            case bcmiL3ControlEcmpHashControl:
                if (field_value[0]) {
                    *value |= BCM_HASH_CONTROL_MULTIPATH_DIP;
                }
                if (field_value[2]) {
                    *value |= BCM_HASH_CONTROL_ECMP_ENHANCE;
                }
                break;
            case bcmiL3ControlEcmpHashMultipath:
                if (!sal_strcmp(field_symbol[0], ecmp_hash_alg_str[0])) {
                    *value = BCM_HASH_ZERO;
                } else if (!sal_strcmp(field_symbol[0], ecmp_hash_alg_str[1])) {
                    *value = BCM_HASH_CRC32U;
                } else if (!sal_strcmp(field_symbol[0], ecmp_hash_alg_str[2])) {
                    *value = BCM_HASH_CRC32L;
                } else if (!sal_strcmp(field_symbol[0], ecmp_hash_alg_str[3])) {
                    *value = BCM_HASH_LSB;
                }
                break;
            case bcmiL3ControlEcmpHashBitCountSelect:
                for (i = 0; i < 7; i++) {
                    if (!sal_strcmp(field_symbol[0], ecmp_hash_mask_str[i])) {
                        *value = i;
                        break;
                    }
                }
                break;
            default:
                *value = field_value[0];
                break;
        }

        SHR_EXIT();
    }

    for (i = 0; i < ti[control].field_count; i++) {
        if (!ti[control].field_symbol) {
            rv = bcmlt_entry_field_get(eh, ti[control].field_str[i],
                                       &field_value[i]);
        } else {
            rv = bcmlt_entry_field_symbol_get(eh, ti[control].field_str[i],
                                              &field_symbol[i]);
        }
        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        } else if (SHR_FAILURE(rv)){
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    switch (control) {
        case bcmiL3ControlEcmpHashControl:
            if (field_value[0]) {
                *value |= BCM_HASH_CONTROL_MULTIPATH_DIP;
            }
            if (field_value[2]) {
                *value |= BCM_HASH_CONTROL_ECMP_ENHANCE;
            }
            break;
        case bcmiL3ControlEcmpHashMultipath:
            if (!sal_strcmp(field_symbol[0], ecmp_hash_alg_str[0])) {
                *value = BCM_HASH_ZERO;
            } else if (!sal_strcmp(field_symbol[0], ecmp_hash_alg_str[1])) {
                *value = BCM_HASH_CRC32U;
            } else if (!sal_strcmp(field_symbol[0], ecmp_hash_alg_str[2])) {
                *value = BCM_HASH_CRC32L;
            } else if (!sal_strcmp(field_symbol[0], ecmp_hash_alg_str[3])) {
                *value = BCM_HASH_LSB;
            }
            break;
        case bcmiL3ControlEcmpHashBitCountSelect:
            for (i = 0; i < 7; i++) {
                if (!sal_strcmp(field_symbol[0], ecmp_hash_mask_str[i])) {
                    *value = i;
                    break;
                }
            }
            break;
        default:
            *value = field_value[0];
            break;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_port_control_set(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    bool locked = false;
    const char *lt_name = L3_MC_PORT_CONTROLs;
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_LOCK(unit);
    locked = true;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    value = port;
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PORT_IDs, value));

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_UNTAG) {
        value = info->ipmc_untag;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, UNTAGs, value));
    }

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_TTL_DEC) {
        value = info->ipmc_ttl_dec ? 0 : 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, SKIP_TTL_DECs, value));
    }

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_TTL_CHECK) {
        value = info->ipmc_ttl_check ? 0 : 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, SKIP_TTL_CHECKs, value));
    }

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_SA_REPLACE) {
        value = info->ipmc_sa_replace ? 0 : 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, SKIP_SA_REPLACEs, value));
    }

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_VLAN) {
        value = info->ipmc_vid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, OVIDs, value));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_port_control_get(
    int unit,
    int port,
    int control,
    bcmi_ltsw_l3_port_control_t *info)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    bool locked = false;
    const char *lt_name = L3_MC_PORT_CONTROLs;
    uint64_t value;
    int rv;
    bcmlt_field_def_t fld_def;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_LOCK(unit);
    locked = true;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    value = port;
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PORT_IDs, value));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_UNTAG) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, lt_name,
                                       UNTAGs, &fld_def));
            info->ipmc_untag = (bool)fld_def.def;
        }

        if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_TTL_DEC) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, lt_name,
                                       SKIP_TTL_DECs, &fld_def));
            info->ipmc_ttl_dec = (bool)fld_def.def;
        }

        if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_TTL_CHECK) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, lt_name,
                                       SKIP_TTL_CHECKs, &fld_def));
            info->ipmc_ttl_check = (bool)fld_def.def;
        }

        if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_SA_REPLACE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, lt_name,
                                       SKIP_SA_REPLACEs, &fld_def));
            info->ipmc_sa_replace = (bool)fld_def.def;
        }

        if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_VLAN) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, lt_name,
                                       OVIDs, &fld_def));
            info->ipmc_vid = (uint16_t)fld_def.def;
        }

        SHR_EXIT();
    }

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_UNTAG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh,UNTAGs, &value));
        info->ipmc_untag = (bool)value;
    }

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_TTL_DEC) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, SKIP_TTL_DECs, &value));
        info->ipmc_ttl_dec = (bool)value;
    }

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_TTL_CHECK) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, SKIP_TTL_CHECKs, &value));
        info->ipmc_ttl_check = (bool)value;
    }

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_SA_REPLACE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, SKIP_SA_REPLACEs, &value));
        info->ipmc_sa_replace = (bool)value;
    }

    if (control & BCMI_LTSW_L3_PC_ATTR_IPMC_VLAN) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, OVIDs, &value));
        info->ipmc_vid = (uint16_t)value;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ipv6_reserved_multicast_addr_multi_set(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    bool locked = false;
    const char *lt_name = L3_IPV6_RESERVED_MCs;
    uint64_t v6[2];
    uint64_t v6_mask[2];
    int i;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (num <= 0 || num > 2) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!ip6_addr || !ip6_mask) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_LOCK(unit);
    locked = true;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    for (i = 0; i < num; i++) {
        bcmi_ltsw_util_ip6_to_uint64(v6, (bcm_ip6_t *)ip6_addr[i]);
        bcmi_ltsw_util_ip6_to_uint64(v6_mask, (bcm_ip6_t *)ip6_mask[i]);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, L3_IPV6_RESERVED_MC_IDs, i));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, DST_IPV6_LOWERs, v6[0]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, DST_IPV6_UPPERs, v6[1]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, DST_IPV6_MASK_LOWERs, v6_mask[0]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, DST_IPV6_MASK_UPPERs, v6_mask[1]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ipv6_reserved_multicast_addr_multi_get(
    int unit,
    int num,
    bcm_ip6_t *ip6_addr,
    bcm_ip6_t *ip6_mask)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    bool locked = false;
    const char *lt_name = L3_IPV6_RESERVED_MCs;
    uint64_t v6[2];
    uint64_t v6_mask[2];
    int rv;
    int count = 0;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (num <= 0 || num > 2) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!ip6_addr || !ip6_mask) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_LOCK(unit);
    locked = true;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    while ((rv = bcmlt_entry_commit(eh, BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, DST_IPV6_UPPERs, &v6[1]));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, DST_IPV6_LOWERs, &v6[0]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, DST_IPV6_MASK_UPPERs, &v6_mask[1]));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, DST_IPV6_MASK_LOWERs, &v6_mask[0]));

        bcmi_ltsw_util_uint64_to_ip6((bcm_ip6_t *)&ip6_addr[count], v6);
        bcmi_ltsw_util_uint64_to_ip6((bcm_ip6_t *)&ip6_mask[count], v6_mask);

        count++;

        if (count >= num) {
            break;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_l3_ip4_options_profile_create(int unit, uint32_t flags,
                                       bcm_l3_ip4_options_action_t default_action,
                                       int *ip4_options_profile_id)
{
    bool locked = false;
    int i, id;
    bcmi_ltsw_l3_ipv4_options_profile_t ip_option_profile[BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK];
    bcmi_ltsw_l3_ipv4_options_profile_t *entry;
    bool drop, copy_cpu;
    int index;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    /* Check for pre-specified ID */
    if (flags & BCM_L3_IP4_OPTIONS_WITH_ID) {
        id = *ip4_options_profile_id;
        if (id > L3_IPV4_OPTION_PROFILE_MAX(unit) ) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (!L3_IPV4_OPTION_PROFILE_FBMP_GET(unit, id)) {
            if (!(flags & BCM_L3_IP4_OPTIONS_REPLACE)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
            } else {
                if (id == BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BADID);
                }
            }
        }
    }

    L3_LOCK(unit);
    locked = true;

    /* Reserve a chunk in the IP4_OPTION Profile table */
    sal_memset(ip_option_profile, 0, sizeof(ip_option_profile));

    /* Set the default action for the profile */
    switch (default_action) {
    case bcmIntfIPOptionActionCopyToCPU:
        copy_cpu = true;
        drop = false;
        break;
    case bcmIntfIPOptionActionDrop:
        copy_cpu = false;
        drop = true;
        break;
    case bcmIntfIPOptionActionCopyCPUAndDrop:
        copy_cpu = true;
        drop = true;
        break;
    case bcmIntfIPOptionActionNone:
    default :
        copy_cpu = false;
        drop = false;
        break;
    }
    for (i = 0; i < BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK; i++) {
        entry = &ip_option_profile[i];
        entry->copy_to_cpu = copy_cpu;
        entry->drop = drop;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ipv4_options_profile_add(unit, ip_option_profile, &index));

    if (flags & BCM_L3_IP4_OPTIONS_WITH_ID) {
        id = *ip4_options_profile_id;
        if (L3_IPV4_OPTION_PROFILE_FBMP_GET(unit, id)) {
            L3_IPV4_OPTION_PROFILE_FBMP_CLR(unit, id);
        }
    } else {
        id = l3_ipv4_option_profile_id_alloc(unit);
        if (id == -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (l3_ipv4_options_profile_delete(unit, index));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }
        L3_IPV4_OPTION_PROFILE_FBMP_CLR(unit, id);
        *ip4_options_profile_id = id;
    }

    L3_IPV4_OPTION_PROFILE_HW_IDX(unit, id) = index;

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ip4_options_profile_destroy(int unit,
                                        int ip4_options_profile_id)
{
    int id;
    int index;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (ip4_options_profile_id == BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BADID);
    }

    L3_LOCK(unit);
    locked = true;

    id = ip4_options_profile_id;
    if (L3_IPV4_OPTION_PROFILE_FBMP_GET(unit, id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    } else {
        index = L3_IPV4_OPTION_PROFILE_HW_IDX(unit, id);
        L3_IPV4_OPTION_PROFILE_HW_IDX(unit, id) = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ipv4_options_profile_delete(unit, index));
        L3_IPV4_OPTION_PROFILE_FBMP_SET(unit, id);
    }
exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ip4_options_action_set(int unit,
                                   int ip4_options_profile_id,
                                   int ip4_option,
                                   bcm_l3_ip4_options_action_t action)
{
    bcmi_ltsw_l3_ipv4_options_profile_t ip_option_profile[BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK];
    int id, index = -1;
    bcmi_ltsw_l3_ipv4_options_profile_t *entry = NULL;
    int offset;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    /*  Make sure module was initialized. */
    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_LOCK(unit);
    locked = true;

    id = ip4_options_profile_id;
    if (id == BCM_L3_IP4_OPTIONS_DEF_PROFILE_INDEX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BADID);
    }

    if (L3_IPV4_OPTION_PROFILE_FBMP_GET(unit, id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else {
        /* Base index of table in hardware */
        index = L3_IPV4_OPTION_PROFILE_HW_IDX(unit, id);

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ipv4_options_profile_get(unit, index, ip_option_profile));

        /* Offset within table */
        offset = ip4_option;

        /* Modify what's needed */
        entry = &ip_option_profile[offset];
        switch (action) {
        case bcmIntfIPOptionActionCopyToCPU:
            entry->copy_to_cpu = true;
            entry->drop = false;
            break;
        case bcmIntfIPOptionActionDrop:
            entry->copy_to_cpu = false;
            entry->drop = true;
            break;
        case bcmIntfIPOptionActionCopyCPUAndDrop:
            entry->copy_to_cpu = true;
            entry->drop = true;
            break;
        case bcmIntfIPOptionActionNone:
        default :
            entry->copy_to_cpu = false;
            entry->drop = false;
        }
        /* Delete the old profile chunk */
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ipv4_options_profile_delete(unit, index));

        /* Add new chunk and store new HW index */
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ipv4_options_profile_add(unit, ip_option_profile, &index));

        L3_IPV4_OPTION_PROFILE_HW_IDX(unit, id) = index;
    }
exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ip4_options_action_get(int unit,
                                   int ip4_options_profile_id,
                                   int ip4_option,
                                   bcm_l3_ip4_options_action_t *action)
{
    bcmi_ltsw_l3_ipv4_options_profile_t ip_option_profile[BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK];
    int id;
    int offset;
    bool copy_to_cpu, drop;
    int index = -1;
    bool locked = false;
    bcmi_ltsw_l3_ipv4_options_profile_t *entry = NULL;

    SHR_FUNC_ENTER(unit);

    /*  Make sure module was initialized. */
    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    id = ip4_options_profile_id;
    L3_LOCK(unit);
    locked = true;

    if (L3_IPV4_OPTION_PROFILE_FBMP_GET(unit, id)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else {
        /* Base index of table in hardware */
        index = L3_IPV4_OPTION_PROFILE_HW_IDX(unit, id);

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_ipv4_options_profile_get(unit, index, ip_option_profile));

        /* Offset within table */
        offset = ip4_option;

        /* Get what's needed */
        entry = &ip_option_profile[offset];

        copy_to_cpu = entry->copy_to_cpu;
        drop = entry->drop;

        *action = bcmIntfIPOptionActionNone;
        if (!copy_to_cpu && !drop) {
            *action = bcmIntfIPOptionActionNone;
        }
        if (copy_to_cpu && !drop) {
            *action = bcmIntfIPOptionActionCopyToCPU;
        }
        if (!copy_to_cpu && drop) {
            *action = bcmIntfIPOptionActionDrop;
        }
        if (copy_to_cpu && drop) {
            *action = bcmIntfIPOptionActionCopyCPUAndDrop;
        }
    }
exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ip6_prefix_map_get(int unit, int map_size,
                               bcm_ip6_t *ip6_array, int *ip6_count)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ip6_prefix_map_get(unit, map_size, ip6_array, ip6_count));

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ip6_prefix_map_add(int unit, bcm_ip6_t ip6_addr)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ip6_prefix_map_add(unit, ip6_addr));

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ip6_prefix_map_delete(int unit, bcm_ip6_t ip6_addr)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ip6_prefix_map_delete(unit, ip6_addr));

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ip6_prefix_map_delete_all(int unit)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ip6_prefix_map_delete_all(unit));

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ip4_options_profile_id2idx(
    int unit,
    int profile_id,
    int *hw_idx)
{
    SHR_FUNC_ENTER(unit);

    L3_LOCK(unit);

    *hw_idx = L3_IPV4_OPTION_PROFILE_HW_IDX(unit, profile_id) /
              BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK;

    L3_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_l3_ip4_options_profile_idx2id(
    int unit,
    int hw_idx,
    int *profile_id)
{
    int i;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    L3_LOCK(unit);
    locked = true;

    hw_idx = hw_idx * BCMI_LTSW_L3_IP_OPTION_PROFILE_CHUNK;

    for (i = 0; i <= L3_IPV4_OPTION_PROFILE_MAX(unit); i++) {
        if (hw_idx == L3_IPV4_OPTION_PROFILE_HW_IDX(unit, i)) {
            break;
        }
    }
    if (i > L3_IPV4_OPTION_PROFILE_MAX(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *profile_id = i;

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

