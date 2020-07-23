/*! \file flexdigest.c
 *
 * Flex Digest management.
 * This file contains the management for Flex Digests.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm/flexdigest.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/flexdigest_int.h>
#include <bcm_int/ltsw/generated/flexdigest_ha.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/mbcm/flexdigest.h>
#include <bcm_int/ltsw/chip/bcmgene_feature.h>

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FLEXDIGEST

/*! Signature for FLEX DIGEST hashing. */
#define FD_SIGNATURE 0xfddb

/*! Flex Digest sub-feature test. */
#define FD_SUB_FEATURE_TEST(_db, _f) \
    ((_db)->features & (BCMGENE_FLEXDIGEST_SUB_F_##_f))

/*! Flex Digest profile is not initialized. */
#define FD_PROFILE_NOT_INIT(_prof) \
    ((_prof)->id_count > 0 && (_prof)->id_bitmap == NULL && (_prof)->desc)

/*! The valid Flex Digest profile. */
#define FD_PROFILE_VALID(_prof) \
    ((_prof)->id_count > 0 && (_prof)->id_bitmap && (_prof)->desc)

/*! The invalid Flex Digest profile. */
#define FD_PROFILE_INVALID(_prof) \
    (!FD_PROFILE_VALID(_prof))

/*! Flex Digest profile identifier validate. */
#define FD_PROFILE_ID_VALID(_prof, _id) \
    ((_id) >= 0 && (_id) < (_prof)->id_count)

/*! Invalid Flex Digest profile identifier. */
#define FD_PROFILE_ID_INVALID(_prof, _id) \
    (!FD_PROFILE_ID_VALID(_prof, _id))

/*! Test Flex Digest profile identifer. */
#define FD_PROFILE_ID_TEST(_prof, _id) \
    (SHR_BITGET((_prof)->id_bitmap,(_id)))

/*! Add Flex Digest profile identifer. */
#define FD_PROFILE_ID_ADD(_prof, _id) \
    (SHR_BITSET((_prof)->id_bitmap,(_id)))

/*! Remove Flex Digest profile identifer. */
#define FD_PROFILE_ID_REMOVE(_prof, _id) \
    (SHR_BITCLR((_prof)->id_bitmap,(_id)))

/*! Flex Digest profile identifer iteration. */
#define FD_PROFILE_ID_ITER(_prof, _id) \
    for ((_id) = 0; (_id) < (_prof)->id_count; (_id)++)

/*! Flex Digest profile identifer iteration done. */
#define FD_PROFILE_ID_ITER_LAST(_prof, _id) \
        ((_id) == (_prof)->id_count)

#define FD_QSET_OR(qset_a, qset_b) \
    SHR_BITOR_RANGE(qset_a.w, qset_b.w, 0, BCM_FLEXDIGEST_QUALIFY_MAX, qset_a.w)

#define FD_HASH_BIN_MAX 16

/*!
 * \brief The profile of flexdigest functionality.
 */
typedef struct fd_profile_s {
    /*! Profile identifier maximum count. */
    int id_count;

    /*! Profile identifier bitmap. */
    SHR_BITDCL *id_bitmap;

    /*! Profile description. */
    char *desc;

    /*! Profile dump function. */
    int (*recover)(int unit, struct fd_profile_s *profile);

    /*! Profile dump function. */
    int (*dump)(int unit, int profile_id);

} fd_profile_t;


/*!
 * \brief The normalization database of flexdigest.
 */
typedef struct fd_norm_db_s {

    /*! FD normalization features. */
    uint32_t features;

    /*! FD normalization seed profile. */
    fd_profile_t seed_profile;

    /*! FD normalization profile. */
    fd_profile_t profile;

} fd_norm_db_t;

/*!
 * \brief Entry qualifier status flags.
 */
/*! The entry qualifier is not used. */
#define FD_ENTRY_QUAL_UNUSED                     (0)
/*! The entry qualifier is used. */
#define FD_ENTRY_QUAL_USED                  (1 << 0)

/*!
 * \brief Entry action status flags.
 */
/*! The entry action is not used. */
#define FD_ENTRY_ACTION_UNUSED                   (0)
/*! The entry action is used. */
#define FD_ENTRY_ACTION_USED                (1 << 0)

/*!
 * \brief Entry status flags.
 */
/*! The entry is not used. */
#define FD_ENTRY_UNUSED                          (0)
/*! The entry is used. */
#define FD_ENTRY_USED                       (1 << 0)
/*! Software entry differs from one in hardware. */
#define FD_ENTRY_DIRTY                      (1 << 1)
/*! Software entry is installed in hw. */
#define FD_ENTRY_INSTALLED                  (1 << 2)

/*
 * Group status flags.
 */
/*! The flex digest group is not used. */
#define FD_GROUP_UNUSED                          (0)
/*! The flex digest group is created. */
#define FD_GROUP_USED                       (1 << 0)

/*
 * Match flags.
 */
/*! The flex digest match is not used. */
#define FD_MATCH_UNUSED                          (0)
/*! The flex digest match is created. */
#define FD_MATCH_USED                       (1 << 0)

/*
 * Match flags.
 */
/*! The flex digest match is not used. */
#define FD_MATCH_ID_UNUSED                          (0)
/*! The flex digest match is created. */
#define FD_MATCH_ID_USED                       (1 << 0)

/*!
 * \brief The group resource database of flexdigest.
 */
typedef struct fd_lkup_group_resource_s {

    /*! FD lookup group qualifier set in group. */
    bcmint_flexdigest_lkup_group_qualifier_presel_t *qualifier_presel_pool;

    /*! FD lookup entries in group. */
    bcmint_flexdigest_lkup_entry_t *entry_pool;

    /*! FD lookup entry qualifier pool. */
    bcmint_flexdigest_lkup_entry_qualifier_t *qualifier_pool;

    /*! FD lookup entry action pool. */
    bcmint_flexdigest_lkup_entry_action_t *action_pool;

} fd_lkup_group_resource_t;

/*!
 * \brief The group subcomponent id of flexdigest.
 */
typedef enum fd_lkup_group_sub_id_e {

    /*! FD lookup group sub component id for qualifier presel pool. */
    FD_LKUP_GROUP_SUB_ID_QUALIFIER_PRESEL_POOL = 0,

    /*! FD lookup group sub component id for entry pool. */
    FD_LKUP_GROUP_SUB_ID_ENTRY_POOL = 1,

    /*! FD lookup group sub component id for qualifier pool. */
    FD_LKUP_GROUP_SUB_ID_QUALIFIER_POOL = 2,

    /*! FD lookup group sub component id for action pool. */
    FD_LKUP_GROUP_SUB_ID_ACTION_POOL = 3,

    /*! FD lookup group sub component id count. */
    FD_LKUP_GROUP_SUB_ID_COUNT = 4

} fd_lkup_group_sub_id_t;

/*!
 * \brief The lookup database of flexdigest.
 */
typedef struct fd_lkup_db_s {

    /*! FD lookup features. */
    uint32_t features;

    /*! The number of FD lookup groups. */
    int num_groups;

    /*! The number of FD lookup entries. */
    int num_entries;

    /*! The number of FD lookup entries per group. */
    int num_entries_per_group;

    /*! The maximum priority of FD lookup group. */
    int group_prio_max;

    /*! The maximum priority of FD lookup entry. */
    int entry_prio_max;

    /*! The count of FD lookup pre-selected qualifiers. */
    int qualifier_presel_count;

    /*! The FD lookup groups. */
    bcmint_flexdigest_lkup_group_t *groups;

    /*! The FD lookup group resources. */
    fd_lkup_group_resource_t *group_resources;

    /*! The number of FD lookup match identifiers. */
    int num_match_ids;

    /*! The bit width of FD lookup match bitmap. */
    int match_width;

    /*! The FD lookup match information pool based on match identifier. */
    bcmint_flexdigest_lkup_match_info_t *match_info_pool;

    /*! The FD lookup match criteria pool. */
    bcmint_flexdigest_lkup_match_t *match_pool;

} fd_lkup_db_t;


/*!
 * \brief The hashing database of flexdigest.
 */
typedef struct fd_hash_db_s {

    /*! FD hashing features. */
    uint32_t features;

    /*! FD hashing profile. */
    fd_profile_t profile;

} fd_hash_db_t;


/*!
 * \brief Generic database of flexdigest module.
 */
typedef struct fd_db_s {
    /*! FD module lock. */
    sal_mutex_t lock;

    /*! FD signature. */
    uint32_t signature;

    /*! FD normalization database. */
    fd_norm_db_t norm;

    /*! FD lookup database. */
    fd_lkup_db_t lkup;

    /*! FD hashing database. */
    fd_hash_db_t hash;

} fd_db_t;

/*!
 * \brief The FD database.
 */
static fd_db_t flexdigest_db[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */


/*!
 * \brief Create protection mutex for flexdigest.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lock_create(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];

    SHR_FUNC_ENTER(unit);

    if (fd_db->lock == NULL) {
        fd_db->lock = sal_mutex_create("bcmFlexDigestMutex");
        SHR_NULL_CHECK(fd_db->lock, SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy protection mutex for flexdigest.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
fd_lock_destroy(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];

    SHR_FUNC_ENTER(unit);

    if (fd_db->lock != NULL) {
        sal_mutex_destroy(fd_db->lock);
        fd_db->lock = NULL;
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flexdigest mutax lock.
 *
 * \param [in] unit Unit Number.
 */
static void
fd_lock(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];

    if (fd_db->lock) {
        sal_mutex_take(fd_db->lock, SAL_MUTEX_FOREVER);
    }
}

/*!
 * \brief Flexdigest mutax unlock.
 *
 * \param [in] unit Unit Number.
 */
static void
fd_unlock(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];

    if (fd_db->lock) {
        sal_mutex_give(fd_db->lock);
    }
}

/*!
 * \brief Initialize a flexdigest profile.
 *
 * \param [in] profile The Flex Digest profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_profile_init(fd_profile_t *profile)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    if (FD_PROFILE_NOT_INIT(profile)) {
        SHR_BITDCL *id_bitmap = NULL;
        SHR_ALLOC(id_bitmap,
                  SHR_BITALLOCSIZE(profile->id_count),
                  profile->desc);
        SHR_NULL_CHECK(id_bitmap, SHR_E_MEMORY);
        sal_memset(id_bitmap, 0, SHR_BITALLOCSIZE(profile->id_count));
        profile->id_bitmap = id_bitmap;
        id_bitmap = NULL;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-init a flexdigest profile.
 *
 * \param [in] profile The Flex Digest profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_profile_deinit(fd_profile_t *profile)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    if (FD_PROFILE_VALID(profile)) {
        SHR_FREE(profile->id_bitmap);
        profile->id_count = 0;
        profile->desc = NULL;
        profile->recover = NULL;
        profile->dump = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a profile identifier.
 *
 * \param [in] profile The Flex Digest profile.
 * \param [in] option Create option. See BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_xxx
 * \param [in/out] id Profile Identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_profile_create(
    fd_profile_t *profile,
    uint32_t option,
    int *id)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);
    SHR_NULL_CHECK(id, SHR_E_PARAM);

    if (FD_PROFILE_INVALID(profile)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (option & BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID) {
        if (id == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (FD_PROFILE_ID_INVALID(profile, *id)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (FD_PROFILE_ID_TEST(profile, *id)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
        FD_PROFILE_ID_ADD(profile, *id);
    } else {
        int i;
        FD_PROFILE_ID_ITER(profile, i) {
            if (FD_PROFILE_ID_TEST(profile, i)) {
                continue;
            }
            FD_PROFILE_ID_ADD(profile, i);
            *id = i;
            break;
        }
        if (FD_PROFILE_ID_ITER_LAST(profile, i)) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a profile identifier.
 *
 * \param [in] profile The Flex Digest profile.
 * \param [in] id Profile Identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_profile_destory(
    fd_profile_t *profile,
    int id)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    if (FD_PROFILE_INVALID(profile)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (FD_PROFILE_ID_INVALID(profile, id)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (FD_PROFILE_ID_TEST(profile, id)) {
        FD_PROFILE_ID_REMOVE(profile, id);
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
fd_profile_recover(int unit, fd_profile_t *profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    if (FD_PROFILE_VALID(profile) && profile->recover) {
        SHR_IF_ERR_VERBOSE_EXIT
            (profile->recover(unit, profile));
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

#if defined(BROADCOM_DEBUG)
/*!
 * \brief dump the configuration saved in profile.
 *
 * \param [in] unit Unit number.
 * \param [in] profile The Flex Digest profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_profile_dump(int unit, fd_profile_t *profile)
{
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    if (FD_PROFILE_VALID(profile) && profile->dump) {
        LOG_CLI((BSL_META_U(unit, "\t%s(%d):\n"),
                            profile->desc, profile->id_count));
        for (i = 0; i < profile->id_count; i++) {
            if (FD_PROFILE_ID_TEST(profile, i)) {
                if (profile->dump) {
                    profile->dump(unit, i);
                }
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}
#endif

/*!
 * \brief Judge the flexdigest db is initialized or not.
 *
 * \param [in] unit Unit Number.
 *
 * \retval true initialized.
 * \retval false non-initialized.
 */
static bool
fd_db_is_initialized(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];

    if (fd_db->signature == FD_SIGNATURE) {
        return true;
    }

    return false;
}

/*!
 * \brief Validate the chip feature for flexdigest functionality.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_functionality(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_FLEXDIGEST)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Validate the flexdigest component initialization status.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_initialization(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!fd_db_is_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest normalization profile seed identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] seed_profile_id Normalization seed profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_norm_profile_seed_id(
    int unit,
    int seed_profile_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = &fd_norm->seed_profile;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_norm, NORM_SEED_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!FD_PROFILE_ID_VALID(profile, seed_profile_id)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!FD_PROFILE_ID_TEST(profile, seed_profile_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest normalization profile identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] norm_profile_id Normalization profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_norm_profile_id(
    int unit,
    int norm_profile_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = &fd_norm->profile;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_norm, NORM_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!FD_PROFILE_ID_VALID(profile, norm_profile_id)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!FD_PROFILE_ID_TEST(profile, norm_profile_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest normalization profile controlling parameters.
 *
 * \param [in] unit Unit Number.
 * \param [in] norm_profile_id Normalization profile identifier.
 * \param [in] type Normalization profile controlling type.
 * \param [in] value Normalization profile controlling value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_norm_profile(
    int unit,
    int norm_profile_id,
    bcm_flexdigest_norm_profile_control_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    switch (type) {
    case bcmFlexDigestNormProfileControlNorm:
        if (value < 0 || value > 1) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        break;
    case bcmFlexDigestNormProfileControlSeedId:
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_validate_norm_profile_seed_id(unit, value));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest sub-feature of lookup group.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_group_sf(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_lkup, LKUP_GROUP)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (fd_lkup->groups == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest group id.
 *
 * \param [in] unit Unit Number.
 * \param [in] group Flex digest group number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_group_id(
    int unit,
    int group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_sf(unit));

    if (group < 0 || group >= fd_lkup->num_groups) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest lookup group configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] qset Qualifier set.
 * \param [in] pri Group priority.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_group_info(
    int unit,
    bcm_flexdigest_qset_t qset,
    int pri)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_group_qualifier_presel_t *q = NULL;
    bcm_flexdigest_qset_t presel_qset;
    int i, j, count;

    SHR_FUNC_ENTER(unit);

    SHR_BITCOUNT_RANGE(qset.w, count, 0, bcmFlexDigestQualifyCount);
    if (count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    BCM_FLEXDIGEST_QSET_INIT(presel_qset);

    for (i = 0; i < fd_lkup->num_groups; i++) {
        grp = &fd_lkup->groups[i];
        grp_res = &fd_lkup->group_resources[i];
        if (grp->flags == FD_GROUP_UNUSED) {
            continue;
        }
        for (j = 0; j < fd_lkup->qualifier_presel_count; j++) {
            q = &grp_res->qualifier_presel_pool[j];
            if (q->valid) {
                BCM_FLEXDIGEST_QSET_ADD(presel_qset, q->qualifier);
            }
        }
    }
    FD_QSET_OR(presel_qset, qset);

    if (BCM_FLEXDIGEST_QSET_TEST(presel_qset, bcmFlexDigestQualifyMatchId)) {
        BCM_FLEXDIGEST_QSET_REMOVE(presel_qset, bcmFlexDigestQualifyMatchId);
    }

    SHR_BITCOUNT_RANGE(presel_qset.w, count, 0, bcmFlexDigestQualifyCount);
    if (count > fd_lkup->qualifier_presel_count) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_validate_lkup_group_qset(unit, presel_qset));

    if ((pri < 0 && pri != BCM_FLEXDIGEST_GROUP_PRIO_ANY) ||
        (pri > fd_lkup->group_prio_max)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest group for used status.
 *
 * \param [in] unit Unit Number.
 * \param [in] group Flex digest group.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_group_used(
    int unit,
    bcm_flexdigest_group_t group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_id(unit, group));

    grp = &fd_lkup->groups[group];

    if (grp->flags == FD_GROUP_UNUSED) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest entry identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] group Flex digest group.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_group_entry_id(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_entry_t entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    int entry_min, entry_max;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_id(unit, group));

    grp = &fd_lkup->groups[group];

    entry_min = grp->entry_id_base;
    entry_max = grp->entry_id_base + grp->entry_id_count - 1;

    if (entry < entry_min || entry > entry_max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest entry for used status.
 *
 * \param [in] unit Unit Number.
 * \param [in] group Flex digest group.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_group_entry_used(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_entry_t entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    int entry_id_offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_used(unit, group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_id(unit, group, entry));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];

    if (ent->flags == FD_ENTRY_UNUSED) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest match id sub-feature.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_match_id_sf(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_lkup, LKUP_MATCH_ID)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (fd_lkup->match_info_pool == NULL ||
        fd_lkup->match_pool == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest match id.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id Match criteria identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_match_id(
    int unit,
    bcm_flexdigest_match_id_t match_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id_sf(unit));

    if (match_id > fd_lkup->num_match_ids) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    match_info = &fd_lkup->match_info_pool[match_id];
    if (match_info->flags == FD_MATCH_ID_UNUSED) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest action.
 *
 * \param [in] unit Unit Number.
 * \param [in] action Flex digest lookup action.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_action(
    int unit,
    bcm_flexdigest_action_t action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_validate_lkup_action(unit, action));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest action configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] action Flex digest lookup action.
 * \param [in] param0 Parameter 0.
 * \param [in] param1 Parameter 0.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_action_params(
    int unit,
    bcm_flexdigest_action_t action,
    uint16_t param0,
    uint16_t param1)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_validate_lkup_action_params(unit, action, param0, param1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest lookup qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] group Unit Number.
 * \param [in] qualifier The flex digest qualifier.
 * \param [in] data Qualifier data.
 * \param [in] mask Qualifier mask.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_qualifier(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_qualify_t qualifier)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_group_qualifier_presel_t *q = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    if (qualifier < 0 || qualifier >= bcmFlexDigestQualifyCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (qualifier) {
    case bcmFlexDigestQualifyMatchId:
        break;
    default:
        grp_res = &fd_lkup->group_resources[group];
        for (i = 0; i < fd_lkup->qualifier_presel_count; i++) {
            q = &grp_res->qualifier_presel_pool[i];
            if (q->valid && q->qualifier == qualifier) {
                break;
            }
        }
        if (i == fd_lkup->qualifier_presel_count) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest lookup qualifier data.
 *
 * \param [in] unit Unit Number.
 * \param [in] group Unit Number.
 * \param [in] qualifier The flex digest qualifier.
 * \param [in] data Qualifier data.
 * \param [in] mask Qualifier mask.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_lkup_qualifier_data(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_qualify_t qualifier,
    uint16_t data,
    uint16_t mask)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_qualifier(unit, group, qualifier));

    switch (qualifier) {
    case bcmFlexDigestQualifyMatchId:
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_validate_lkup_match_id(unit, data));
        break;
    default:
        if (data & ~mask) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_validate_lkup_qualifier(unit, qualifier,
                                                          data, mask));
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest hashing profile identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] hash_profile_id Hashing profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_hash_profile_id(
    int unit,
    int hash_profile_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_hash_db_t *fd_hash = &fd_db->hash;
    fd_profile_t *profile = &fd_hash->profile;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_hash, HASH_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!FD_PROFILE_ID_VALID(profile, hash_profile_id)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    if (!FD_PROFILE_ID_TEST(profile, hash_profile_id)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the flexdigest hashing profile controlling parameters.
 *
 * \param [in] unit Unit Number.
 * \param [in] hash_profile_id Hashing profile identifier.
 * \param [in] type Hashing profile controlling type.
 * \param [in] value Hashing profile controlling value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_validate_hash_profile(
    int unit,
    int hash_profile_id,
    bcm_flexdigest_hash_profile_control_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_validate_hash_profile(unit, hash_profile_id,
                                                    type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flex digest normalization seed profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] option Create option. See BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_xxx
 * \param [inout] seed_profile_id Seed profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_seed_profile_create(
    int unit,
    uint32_t option,
    int *seed_profile_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = &fd_norm->seed_profile;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(seed_profile_id, SHR_E_PARAM);

    if (!FD_SUB_FEATURE_TEST(fd_norm, NORM_SEED_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_profile_create(profile, option, seed_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_norm_seed_profile_create(unit, *seed_profile_id));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Destroy a flex digest normalization seed profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] seed_profile_id Seed profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_seed_profile_destroy(
    int unit,
    int seed_profile_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = &fd_norm->seed_profile;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_norm, NORM_SEED_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_profile_destory(profile, seed_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_norm_seed_profile_destroy(unit, seed_profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest normalization seed profiles.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_seed_profile_destroy_all(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = &fd_norm->seed_profile;
    int id;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_norm, NORM_SEED_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    FD_PROFILE_ID_ITER(profile, id) {
        if (FD_PROFILE_ID_TEST(profile, id)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_profile_destory(profile, id));

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexdigest_norm_seed_profile_destroy(unit, id));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the flex digest normalization seed profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] seed_profile_id Seed profile identifier.
 * \param [in] type The normalization seed profile control type.
 * \param [in] value The normalization seed profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_seed_profile_set(
    int unit,
    int seed_profile_id,
    bcm_flexdigest_norm_seed_control_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_norm_profile_seed_id(unit, seed_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_norm_seed_profile_set(unit, seed_profile_id,
                                                    type, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the flex digest normalization seed profile configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] seed_profile_id Seed profile identifier.
 * \param [in] type The normalization seed profile control type.
 * \param [out] value The normalization seed profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_seed_profile_get(
    int unit,
    int seed_profile_id,
    bcm_flexdigest_norm_seed_control_t type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_norm_profile_seed_id(unit, seed_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_norm_seed_profile_get(unit, seed_profile_id,
                                                    type, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover flexdigest normalization seed profile identifier bitmap.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile The Flex Digest profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_seed_profile_recover(int unit, fd_profile_t *profile)
{
    int i, rv;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    FD_PROFILE_ID_ITER(profile, i) {
        rv = mbcm_ltsw_flexdigest_norm_seed_profile_configured(unit, i);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        FD_PROFILE_ID_ADD(profile, i);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump/Show a flexdigest normalization seed profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] seed_profile_id The Flex Digest profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_seed_profile_dump(int unit, int seed_profile_id)
{
    int i, value;
    char *str[] = BCM_FLEXDIGEST_NORM_SEED_CONTROL_STRINGS;

    SHR_FUNC_ENTER(unit);

    LOG_CLI((BSL_META("\t %d):\n"), seed_profile_id));
    for (i = 0; i < bcmFlexDigestNormSeedControlCount; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_norm_seed_profile_get(unit, seed_profile_id, i, &value));
        LOG_CLI((BSL_META("\t\t%s: 0x%08x\n"), str[i], value));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create a flex digest normalization profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] option Create option. See BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_xxx
 * \param [inout] norm_profile_id Normalization profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_profile_create(
    int unit,
    uint32 option,
    int *norm_profile_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = &fd_norm->profile;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(norm_profile_id, SHR_E_PARAM);

    if (!FD_SUB_FEATURE_TEST(fd_norm, NORM_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_profile_create(profile, option, norm_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_norm_profile_create(unit, *norm_profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a flex digest normalization profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] norm_profile_id Normalization profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_profile_destroy(
    int unit,
    int norm_profile_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = &fd_norm->profile;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_profile_destory(profile, norm_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_norm_profile_destroy(unit, norm_profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest normalization profiles.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_profile_destroy_all(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = &fd_norm->profile;
    int id;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_norm, NORM_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    FD_PROFILE_ID_ITER(profile, id) {
        if (FD_PROFILE_ID_TEST(profile, id)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_profile_destory(profile, id));

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexdigest_norm_profile_destroy(unit, id));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure flex digest normalization profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] norm_profile_id Normalization profile identifier.
 * \param [in] type The normalization profile control type.
 * \param [in] value The normalization profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_profile_set(
    int unit,
    int norm_profile_id,
    bcm_flexdigest_norm_profile_control_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_norm_profile_id(unit, norm_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_norm_profile(unit, norm_profile_id, type, value));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_norm_profile_set(unit, norm_profile_id,
                                               type, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex digest normalization profile configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] norm_profile_id Normalization profile identifier.
 * \param [in] type The normalization profile control type.
 * \param [out] value The normalization profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_profile_get(
    int unit,
    int norm_profile_id,
    bcm_flexdigest_norm_profile_control_t type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_norm_profile_id(unit, norm_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_norm_profile_get(unit, norm_profile_id,
                                               type, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover flexdigest normalization profile identifier bitmap.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile The Flex Digest profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_profile_recover(int unit, fd_profile_t *profile)
{
    int i, rv;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    FD_PROFILE_ID_ITER(profile, i) {
        rv = mbcm_ltsw_flexdigest_norm_profile_configured(unit, i);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        FD_PROFILE_ID_ADD(profile, i);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump/Show a flexdigest normalization profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile The Flex Digest profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_norm_profile_dump(int unit, int profile_id)
{
    int i, value;
    char *str[] = BCM_FLEXDIGEST_NORM_PROFILE_CONTROL_STRINGS;

    SHR_FUNC_ENTER(unit);

    LOG_CLI((BSL_META_U(unit, "\t %d):\n"), profile_id));
    for (i = 0; i < bcmFlexDigestNormProfileControlCount; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_norm_profile_get(unit, profile_id, i, &value));
        LOG_CLI((BSL_META_U(unit, "\t\t%s: %d\n"), str[i], value));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the reference count for a match identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 * \param [in] ref_count The reference count.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_match_id_update_ref_count(
    int unit,
    bcm_flexdigest_match_id_t match_id,
    int ref_count)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id(unit, match_id));

    match_info = &fd_lkup->match_info_pool[match_id];
    match_info->ref_count += ref_count;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find entry group by entry identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_find_by_entry(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_group_t *group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    int i, entry_min, entry_max;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_sf(unit));

    for (i = 0; i < fd_lkup->num_groups; i++) {
        grp = &fd_lkup->groups[i];
        if (grp->flags == FD_GROUP_UNUSED) {
            continue;
        }
        entry_min = grp->entry_id_base;
        entry_max = grp->entry_id_base + grp->entry_id_count - 1;
        if (entry >= entry_min && entry <= entry_max) {
            *group = i;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate resources for flex digest group.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_resource_allocate(
    int unit,
    bcm_flexdigest_group_t group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    void *ptr = NULL;
    shr_ha_sub_id sub_id = 0;
    int rv;

    SHR_FUNC_ENTER(unit);

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    if (grp_res->qualifier_presel_pool == NULL) {
        ha_array_size = fd_lkup->qualifier_presel_count;
        ha_instance_size = sizeof(bcmint_flexdigest_lkup_group_qualifier_presel_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        sub_id = grp->sub_comp_id_base + FD_LKUP_GROUP_SUB_ID_QUALIFIER_PRESEL_POOL;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     "bcmFDGrpQualifierPreselPool",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                SHR_E_MEMORY : SHR_E_NONE);
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     0, ha_instance_size, ha_array_size,
                                     BCMINT_FLEXDIGEST_LKUP_GROUP_QUALIFIER_PRESEL_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        grp_res->qualifier_presel_pool = ptr;
        ptr = NULL;
    }
    if (grp_res->entry_pool == NULL) {
        ha_array_size = grp->entry_id_count;
        ha_instance_size = sizeof(bcmint_flexdigest_lkup_entry_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        sub_id = grp->sub_comp_id_base + FD_LKUP_GROUP_SUB_ID_ENTRY_POOL;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     "bcmFDEntryPool",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                SHR_E_MEMORY : SHR_E_NONE);
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     0, ha_instance_size, ha_array_size,
                                     BCMINT_FLEXDIGEST_LKUP_ENTRY_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        grp_res->entry_pool = ptr;
        ptr = NULL;
    }
    if (grp_res->qualifier_pool == NULL) {
        ha_array_size = grp->qualifier_max_per_entry * grp->entry_id_count;
        ha_instance_size = sizeof(bcmint_flexdigest_lkup_entry_qualifier_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        sub_id = grp->sub_comp_id_base + FD_LKUP_GROUP_SUB_ID_QUALIFIER_POOL;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     "bcmFDQualifierPool",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                SHR_E_MEMORY : SHR_E_NONE);
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     0, ha_instance_size, ha_array_size,
                                     BCMINT_FLEXDIGEST_LKUP_ENTRY_QUALIFIER_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        grp_res->qualifier_pool = ptr;
        ptr = NULL;
    }
    if (grp_res->action_pool == NULL) {
        ha_array_size = grp->action_max_per_entry * grp->entry_id_count;
        ha_instance_size = sizeof(bcmint_flexdigest_lkup_entry_action_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        sub_id = grp->sub_comp_id_base + FD_LKUP_GROUP_SUB_ID_ACTION_POOL;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     "bcmFDActionPool",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                SHR_E_MEMORY : SHR_E_NONE);
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     0, ha_instance_size, ha_array_size,
                                     BCMINT_FLEXDIGEST_LKUP_ENTRY_ACTION_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        grp_res->action_pool = ptr;
        ptr = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize resources for flex digest group.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_resource_init(
    int unit,
    bcm_flexdigest_group_t group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    int i;

    SHR_FUNC_ENTER(unit);

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    if (grp_res->qualifier_presel_pool) {
        ha_array_size = fd_lkup->qualifier_presel_count;
        ha_instance_size = sizeof(bcmint_flexdigest_lkup_group_qualifier_presel_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        sal_memset(grp_res->qualifier_presel_pool, 0, ha_alloc_size);
    }
    if (grp_res->entry_pool) {
        ha_array_size = grp->entry_id_count;
        ha_instance_size = sizeof(bcmint_flexdigest_lkup_entry_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        sal_memset(grp_res->entry_pool, 0, ha_alloc_size);
        for (i = 0; i < grp->entry_id_count; i++) {
            ent = &grp_res->entry_pool[i];
            ent->id = i;
            ent->pri = i;
        }
    }
    if (grp_res->qualifier_pool) {
        ha_array_size = grp->qualifier_max_per_entry * grp->entry_id_count;
        ha_instance_size = sizeof(bcmint_flexdigest_lkup_entry_qualifier_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        sal_memset(grp_res->qualifier_pool, 0, ha_alloc_size);
    }
    if (grp_res->action_pool) {
        ha_array_size = grp->action_max_per_entry * grp->entry_id_count;
        ha_instance_size = sizeof(bcmint_flexdigest_lkup_entry_action_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        sal_memset(grp_res->action_pool, 0, ha_alloc_size);
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free resources for flex digest group.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_resource_free(
    int unit,
    bcm_flexdigest_group_t group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    fd_lkup_group_resource_t *grp_res = NULL;

    SHR_FUNC_ENTER(unit);

    grp_res = &fd_lkup->group_resources[group];

    if (grp_res->qualifier_presel_pool) {
        grp_res->qualifier_presel_pool = NULL;
    }
    if (grp_res->entry_pool) {
        grp_res->entry_pool = NULL;
    }
    if (grp_res->qualifier_pool) {
        grp_res->qualifier_pool = NULL;
    }
    if (grp_res->action_pool) {
        grp_res->action_pool = NULL;
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update flex digest lookup presel by group qset.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_update_presel(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_group_qualifier_presel_t *q = NULL;
    bcm_flexdigest_qset_t presel_qset;
    int i, j;

    SHR_FUNC_ENTER(unit);

    BCM_FLEXDIGEST_QSET_INIT(presel_qset);

    for (i = 0; i < fd_lkup->num_groups; i++) {
        grp = &fd_lkup->groups[i];
        grp_res = &fd_lkup->group_resources[i];
        if (grp->flags == FD_GROUP_UNUSED) {
            continue;
        }
        for (j = 0; j < fd_lkup->qualifier_presel_count; j++) {
            q = &grp_res->qualifier_presel_pool[j];
            if (q->valid) {
                BCM_FLEXDIGEST_QSET_ADD(presel_qset, q->qualifier);
            }
        }
    }

    BCM_FLEXDIGEST_QSET_REMOVE(presel_qset, bcmFlexDigestQualifyMatchId);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_lkup_group_update_presel(unit, presel_qset));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an action pointer from group database.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 * \param [in] base Action base for an entry in group database.
 * \param [in] offset Action offset for an entry in group database..
 *
 * \retval Pointer of the action in group database.
 */
static bcmint_flexdigest_lkup_entry_action_t *
fd_lkup_action_entry_action_get(
    int unit,
    bcm_flexdigest_group_t group,
    int base,
    int offset)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    fd_lkup_group_resource_t *grp_res = NULL;

    if (group >= 0 && group < fd_lkup->num_groups) {
        grp_res = &fd_lkup->group_resources[group];
        return &grp_res->action_pool[base + offset];
    }

    return NULL;
}

/*!
 * \brief Add an action and parameters into a lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] action The flex digest policy table action.
 * \param [in] param0 The action parameter 0.
 * \param [in] param1 The action parameter 1.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_action_add(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_action_t action,
    uint16_t param0,
    uint16_t param1)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcmint_flexdigest_lkup_entry_action_t *act;
    bcm_flexdigest_group_t group;
    int i, entry_id_offset, found, action_base;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_action_params(unit, action, param0, param1));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];
    action_base = entry_id_offset * grp->action_max_per_entry;
    found = 0;
    for (i = 0; i < grp->action_max_per_entry; i++) {
        act = fd_lkup_action_entry_action_get(unit, group, action_base, i);
        SHR_NULL_CHECK(act, SHR_E_INTERNAL);
        if (act->flags == FD_ENTRY_ACTION_UNUSED) {
            continue;
        }
        if (act->action == action) {
            found = 1;
            break;
        }
    }
    if (!found) {
        for (i = 0; i < grp->action_max_per_entry; i++) {
            act = fd_lkup_action_entry_action_get(unit, group, action_base, i);
            SHR_NULL_CHECK(act, SHR_E_INTERNAL);
            if (act->flags == FD_ENTRY_ACTION_UNUSED) {
                found = 1;
                break;
            }
        }
    }
    if (!found) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    act->action = action;
    act->param0 = param0;
    act->param1 = param1;
    act->flags |= FD_ENTRY_ACTION_USED;
    if (ent->flags & FD_ENTRY_INSTALLED) {
        ent->flags |= FD_ENTRY_DIRTY;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an action and parameters from a lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] action The flex digest policy table action.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_action_delete(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_action_t action)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcmint_flexdigest_lkup_entry_action_t *act;
    bcm_flexdigest_group_t group;
    int i, entry_id_offset, action_base;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_action(unit, action));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];
    action_base = entry_id_offset * grp->action_max_per_entry;
    for (i = 0; i < grp->action_max_per_entry; i++) {
        act = fd_lkup_action_entry_action_get(unit, group, action_base, i);
        SHR_NULL_CHECK(act, SHR_E_INTERNAL);
        if (act->flags == FD_ENTRY_ACTION_UNUSED) {
            continue;
        }
        if (act->action == action) {
            act->action = 0;
            act->param0 = 0;
            act->param1 = 0;
            act->flags = FD_ENTRY_ACTION_UNUSED;
            if (ent->flags & FD_ENTRY_INSTALLED) {
                ent->flags |= FD_ENTRY_DIRTY;
            }
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all actions from a lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_action_delete_all(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    bcmint_flexdigest_lkup_entry_action_t *act;
    bcm_flexdigest_group_t group;
    int i, entry_id_offset, action_base;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    grp = &fd_lkup->groups[group];
    entry_id_offset = entry - grp->entry_id_base;
    action_base = entry_id_offset * grp->action_max_per_entry;
    for (i = 0; i < grp->action_max_per_entry; i++) {
        act = fd_lkup_action_entry_action_get(unit, group, action_base, i);
        SHR_NULL_CHECK(act, SHR_E_INTERNAL);
        if (act->flags == FD_ENTRY_ACTION_UNUSED) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_lkup_action_delete(unit, entry, act->action));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an action parameters from a lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] action The flex digest policy table action.
 * \param [out] param0 The action parameter 0.
 * \param [out] param1 The action parameter 1.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_action_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_action_t action,
    uint16_t *param0,
    uint16_t *param1)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    bcmint_flexdigest_lkup_entry_action_t *act;
    bcm_flexdigest_group_t group;
    int i, entry_id_offset, action_base;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(param0, SHR_E_PARAM);
    SHR_NULL_CHECK(param1, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_action(unit, action));

    grp = &fd_lkup->groups[group];
    entry_id_offset = entry - grp->entry_id_base;
    action_base = entry_id_offset * grp->action_max_per_entry;
    for (i = 0; i < grp->action_max_per_entry; i++) {
        act = fd_lkup_action_entry_action_get(unit, group, action_base, i);
        SHR_NULL_CHECK(act, SHR_E_INTERNAL);
        if (act->flags == FD_ENTRY_ACTION_UNUSED) {
            continue;
        }
        if (act->action == action) {
            *param0 = act->param0;
            *param1 = act->param1;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an qualifier pointer from group database.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 * \param [in] base Qualifier base for an entry in group database.
 * \param [in] offset Qualifier offset for an entry in group database..
 *
 * \retval Pointer of the qualifier in group database.
 */
static bcmint_flexdigest_lkup_entry_qualifier_t *
fd_lkup_qualifier_entry_qualifier_get(
    int unit,
    bcm_flexdigest_group_t group,
    int base,
    int offset)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    fd_lkup_group_resource_t *grp_res = NULL;

    if (group >= 0 && group < fd_lkup->num_groups) {
        grp_res = &fd_lkup->group_resources[group];
        return &grp_res->qualifier_pool[base + offset];
    }

    return NULL;
}

/*!
 * \brief Add a qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] qualifier The flex digest qualifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_qualifier_add(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_qualify_t qualifier,
    uint16_t data,
    uint16_t mask)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcmint_flexdigest_lkup_entry_qualifier_t *qual;
    bcm_flexdigest_group_t group;
    int i, entry_id_offset, qualifier_base;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_qualifier_data(unit, group, qualifier, data, mask));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];
    qualifier_base = entry_id_offset * grp->qualifier_max_per_entry;
    for (i = 0; i < grp->qualifier_max_per_entry; i++) {
        qual = fd_lkup_qualifier_entry_qualifier_get(unit, group,
                                                     qualifier_base, i);
        SHR_NULL_CHECK(qual, SHR_E_INTERNAL);
        if (qual->flags == FD_ENTRY_QUAL_UNUSED) {
            continue;
        }
        if (qual->qualifier == qualifier) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }
    for (i = 0; i < grp->qualifier_max_per_entry; i++) {
        qual = fd_lkup_qualifier_entry_qualifier_get(unit, group,
                                                     qualifier_base, i);
        SHR_NULL_CHECK(qual, SHR_E_INTERNAL);
        if (qual->flags == FD_ENTRY_QUAL_UNUSED) {
            qual->qualifier = qualifier;
            qual->data = data;
            qual->mask = mask;
            qual->flags |= FD_ENTRY_QUAL_USED;
            if (ent->flags & FD_ENTRY_INSTALLED) {
                ent->flags |= FD_ENTRY_DIRTY;
            }
            if (qualifier == bcmFlexDigestQualifyMatchId) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (fd_lkup_match_id_update_ref_count(unit, data, 1));
            }
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_RESOURCE);
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Delete a qualifier from a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] qualifier The flex digest qualifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_qualifier_delete(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_qualify_t qualifier)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcmint_flexdigest_lkup_entry_qualifier_t *qual;
    bcm_flexdigest_group_t group;
    int i, entry_id_offset, qualifier_base;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_qualifier(unit, group, qualifier));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];
    qualifier_base = entry_id_offset * grp->qualifier_max_per_entry;
    for (i = 0; i < grp->qualifier_max_per_entry; i++) {
        qual = fd_lkup_qualifier_entry_qualifier_get(unit, group,
                                                     qualifier_base, i);
        SHR_NULL_CHECK(qual, SHR_E_INTERNAL);
        if (qual->flags == FD_ENTRY_QUAL_UNUSED) {
            continue;
        }
        if (qual->qualifier == qualifier) {
            if (qualifier == bcmFlexDigestQualifyMatchId) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (fd_lkup_match_id_update_ref_count(unit, qual->data, -1));
            }
            qual->qualifier = 0;
            qual->data = 0;
            qual->mask = 0;
            qual->flags = FD_ENTRY_QUAL_UNUSED;
            if (ent->flags & FD_ENTRY_INSTALLED) {
                ent->flags |= FD_ENTRY_DIRTY;
            }
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all qualifiers in a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_qualifier_delete_all(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    bcmint_flexdigest_lkup_entry_qualifier_t *qual;
    bcm_flexdigest_group_t group;
    int i, entry_id_offset, qualifier_base;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    grp = &fd_lkup->groups[group];
    entry_id_offset = entry - grp->entry_id_base;
    qualifier_base = entry_id_offset * grp->qualifier_max_per_entry;
    for (i = 0; i < grp->qualifier_max_per_entry; i++) {
        qual = fd_lkup_qualifier_entry_qualifier_get(unit, group,
                                                     qualifier_base, i);
        SHR_NULL_CHECK(qual, SHR_E_INTERNAL);
        if (qual->flags == FD_ENTRY_QUAL_UNUSED) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_lkup_qualifier_delete(unit, entry, qual->qualifier));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for a qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] qualifier The flex digest qualifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_qualifier_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_qualify_t qualifier,
    uint16_t *data,
    uint16_t *mask)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    bcmint_flexdigest_lkup_entry_qualifier_t *qual;
    bcm_flexdigest_group_t group;
    int i, entry_id_offset, qualifier_base;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(mask, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_qualifier(unit, group, qualifier));

    grp = &fd_lkup->groups[group];
    entry_id_offset = entry - grp->entry_id_base;
    qualifier_base = entry_id_offset * grp->qualifier_max_per_entry;
    for (i = 0; i < grp->qualifier_max_per_entry; i++) {
        qual = fd_lkup_qualifier_entry_qualifier_get(unit, group,
                                                     qualifier_base, i);
        SHR_NULL_CHECK(qual, SHR_E_INTERNAL);
        if (qual->flags == FD_ENTRY_QUAL_UNUSED) {
            continue;
        }
        if (qual->qualifier == qualifier) {
            *data = qual->data;
            *mask = qual->mask;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a logical match identifier for flex digest lookup entry key.
 *
 * \param [in] unit Unit Number.
 * \param [out] match_id The logical match identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_match_id_create(
    int unit,
    bcm_flexdigest_match_id_t *match_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id_sf(unit));

    SHR_NULL_CHECK(match_id, SHR_E_PARAM);

    for (i = 0; i < fd_lkup->num_match_ids; i++) {
        match_info = &fd_lkup->match_info_pool[i];
        if (match_info->flags == FD_MATCH_ID_UNUSED) {
            match_info->flags |= FD_MATCH_ID_USED;
            *match_id = i;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_RESOURCE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the reference count of match criteria identifer.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match criteria identifer.
 * \param [out] ref_count The qualified reference count of MatchId.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_match_id_ref_count_get(
    int unit,
    int match_id,
    int *ref_count)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ref_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id(unit, match_id));

    match_info = &fd_lkup->match_info_pool[match_id];

    *ref_count = match_info->ref_count;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get match by match id and offset.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 * \param [in] offset The offset of matching criteria.
 *
 * \retval Pointer of the matching criteria.
 */
static bcmint_flexdigest_lkup_match_t *
fd_lkup_match_id_match_get(
    int unit,
    bcm_flexdigest_match_id_t match_id,
    int offset)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    int match_offset;

    if (offset >= 0 && offset < fd_lkup->match_width) {
        match_offset = match_id * fd_lkup->match_width + offset;
        return &fd_lkup->match_pool[match_offset];
    }

    return NULL;
}

/*!
 * \brief Add a matching criteria into a logical match identifiers.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 * \param [in] match The matching criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_match_add(
    int unit,
    bcm_flexdigest_match_id_t match_id,
    bcm_flexdigest_match_t match)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;
    bcmint_flexdigest_lkup_match_t *mat = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id(unit, match_id));

    match_info = &fd_lkup->match_info_pool[match_id];

    if (match_info->ref_count) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    for (i = 0; i < fd_lkup->match_width; i++) {
        mat = fd_lkup_match_id_match_get(unit, match_id, i);
        SHR_NULL_CHECK(mat, SHR_E_INTERNAL);
        if (mat->flags == FD_MATCH_UNUSED) {
            continue;
        }
        if (mat->match == match) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_validate_lkup_match(unit, match));

    for (i = 0; i < fd_lkup->match_width; i++) {
        mat = fd_lkup_match_id_match_get(unit, match_id, i);
        SHR_NULL_CHECK(mat, SHR_E_INTERNAL);
        if (mat->flags == FD_MATCH_UNUSED) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_validate_lkup_match_conflict(unit, match,
                                                               mat->match));
    }

    for (i = 0; i < fd_lkup->match_width; i++) {
        mat = fd_lkup_match_id_match_get(unit, match_id, i);
        SHR_NULL_CHECK(mat, SHR_E_INTERNAL);
        if (mat->flags == FD_MATCH_UNUSED) {
            mat->flags |= FD_MATCH_USED;
            mat->match = match;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a matching criteria from a logical match identifiers.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 * \param [in] match The matching criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_match_delete(
    int unit,
    bcm_flexdigest_match_id_t match_id,
    bcm_flexdigest_match_t match)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;
    bcmint_flexdigest_lkup_match_t *mat = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id(unit, match_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_validate_lkup_match(unit, match));

    match_info = &fd_lkup->match_info_pool[match_id];

    if (match_info->ref_count) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    for (i = 0; i < fd_lkup->match_width; i++) {
        mat = fd_lkup_match_id_match_get(unit, match_id, i);
        SHR_NULL_CHECK(mat, SHR_E_INTERNAL);
        if (mat->flags == FD_MATCH_UNUSED) {
            continue;
        }
        if (mat->match == match) {
            mat->flags &= ~FD_MATCH_USED;
            mat->match = 0;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the matching criterias from a logical match identifiers.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 * \param [in] array_size The maximum number of matching criteria array.
 * \param [out] match_array The matching criteria array.
 * \param [out] count The number of matching criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_match_get(
    int unit,
    bcm_flexdigest_match_id_t match_id,
    int array_size,
    bcm_flexdigest_match_t *match_array,
    int *count)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_t *mat = NULL;
    int i, array_cnt = 0, array_cnt_tmp = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id(unit, match_id));

    for (i = 0; i < fd_lkup->match_width; i++) {
        mat = fd_lkup_match_id_match_get(unit, match_id, i);
        SHR_NULL_CHECK(mat, SHR_E_INTERNAL);
        if (mat->flags == FD_MATCH_UNUSED) {
            continue;
        }
        array_cnt++;
    }

    if (array_size > 0) {
        if (array_size < array_cnt) {
            array_cnt = array_size;
        }

        for (i = 0; i < fd_lkup->match_width; i++) {
            mat = fd_lkup_match_id_match_get(unit, match_id, i);
            SHR_NULL_CHECK(mat, SHR_E_INTERNAL);
            if (mat->flags == FD_MATCH_UNUSED) {
                continue;
            }
            if (array_cnt_tmp >= array_cnt) {
                break;
            }
            match_array[array_cnt_tmp++] = mat->match;
        }
    }

    if (count) {
        *count = array_cnt;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all matching criterias associated with a logical match identifiers.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_match_delete_all(
    int unit,
    bcm_flexdigest_match_id_t match_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;
    bcmint_flexdigest_lkup_match_t *mat = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id(unit, match_id));

    match_info = &fd_lkup->match_info_pool[match_id];

    if (match_info->ref_count) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    for (i = 0; i < fd_lkup->match_width; i++) {
        mat = fd_lkup_match_id_match_get(unit, match_id, i);
        SHR_NULL_CHECK(mat, SHR_E_INTERNAL);
        if (mat->flags == FD_MATCH_UNUSED) {
            continue;
        }
        mat->flags = FD_MATCH_UNUSED;
        mat->match = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a logical match identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_match_id_destroy(
    int unit,
    bcm_flexdigest_match_id_t match_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id(unit, match_id));

    match_info = &fd_lkup->match_info_pool[match_id];

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_match_delete_all(unit, match_id));

    match_info->flags = FD_MATCH_ID_UNUSED;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all logical match identifiers.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_match_id_destroy_all(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id_sf(unit));

    for (i = 0; i < fd_lkup->num_match_ids; i++) {
        match_info = &fd_lkup->match_info_pool[i];
        if (match_info->flags == FD_MATCH_ID_UNUSED) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_lkup_match_id_destroy(unit, i));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flex digest lookup entry with entry Identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_entry_create_id(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_entry_t entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    int entry_id_offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_used(unit, group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_id(unit, group, entry));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];

    if (ent->flags != FD_ENTRY_UNUSED) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    ent->id = entry_id_offset;
    ent->pri = entry_id_offset;
    ent->flags |= FD_ENTRY_USED;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flex digest lookup entry for a given groups.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 * \param [out] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_entry_create(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_entry_t *entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_used(unit, group));

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    for (i = 0; i < grp->entry_id_count; i++) {
        ent = &grp_res->entry_pool[i];
        if (ent->flags == FD_ENTRY_UNUSED) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_lkup_entry_create_id(unit, group, i + grp->entry_id_base));
            *entry = i + grp->entry_id_base;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_RESOURCE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure a flex digest lookup entry priority.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] pri The entry priority.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_entry_priority_set(
    int unit,
    bcm_flexdigest_entry_t entry,
    int pri)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcm_flexdigest_group_t group;
    int entry_id_offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];

    if (pri < 0 || pri > fd_lkup->entry_prio_max) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ent->pri = pri;

    if (ent->flags & FD_ENTRY_INSTALLED) {
        ent->flags |= FD_ENTRY_DIRTY;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the flex digest lookup entry priority.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] pri The entry priority.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_entry_priority_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    int *pri)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcm_flexdigest_group_t group;
    int entry_id_offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];

    if (pri) {
        *pri = ent->pri;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Install a flex digest lookup entry into hardware.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_entry_install(
    int unit,
    bcm_flexdigest_entry_t entry,
    int update)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcmint_flexdigest_lkup_match_t *mat = NULL;
    bcmint_flexdigest_lkup_entry_qualifier_t *qual = NULL;
    bcmint_flexdigest_lkup_entry_action_t *act = NULL;
    bcm_flexdigest_group_t group;
    int entry_id_offset;
    bcmint_flexdigest_entry_info_t einfo;
    bcmint_flexdigest_entry_info_t einfo_old;
    int i, j, match_id, qualifier_base, action_base;

    SHR_FUNC_ENTER(unit);

    sal_memset(&einfo, 0, sizeof(einfo));
    sal_memset(&einfo_old, 0, sizeof(einfo_old));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];
    qualifier_base = entry_id_offset * grp->qualifier_max_per_entry;
    action_base = entry_id_offset * grp->action_max_per_entry;

    if ((ent->flags & FD_ENTRY_INSTALLED) && !update) {
        SHR_EXIT();
    }

    einfo.id = ent->id;
    einfo.group_prio = grp->pri;

    einfo_old.id = ent->id;
    einfo_old.group_prio = grp->pri;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_lkup_entry_allocate(unit, &einfo));

    /* Use a backup handler to save the old configurations. */
    if (update) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_lkup_entry_allocate(unit, &einfo_old));
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_lkup_entry_lookup(unit, &einfo_old));
    }

    /* Unset all field configurations to default. */
    if (update) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_lkup_entry_lookup(unit, &einfo));
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_lkup_entry_unset(unit, &einfo));
    }

    /* Configure the entry priority. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_lkup_entry_pri_set(unit, &einfo, ent->pri));

    /* Configure the qualifiers and MatchId. */
    for (i = 0; i < grp->qualifier_max_per_entry; i++) {
        qual = fd_lkup_qualifier_entry_qualifier_get(unit, group,
                                                     qualifier_base, i);
        SHR_NULL_CHECK(qual, SHR_E_INTERNAL);
        if (qual->flags == FD_ENTRY_QUAL_UNUSED) {
            continue;
        }
        if (qual->qualifier == bcmFlexDigestQualifyMatchId) {
            match_id = qual->data & qual->mask;
            for (j = 0; j < fd_lkup->match_width; j++) {
                mat = fd_lkup_match_id_match_get(unit, match_id, j);
                SHR_NULL_CHECK(mat, SHR_E_INTERNAL);
                if (mat->flags == FD_MATCH_UNUSED) {
                    continue;
                }
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_flexdigest_lkup_entry_match_add(unit, &einfo,
                                                               mat->match));
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexdigest_lkup_entry_qualifier_add(unit, &einfo,
                                                               qual->qualifier,
                                                               qual->data,
                                                               qual->mask));
        }
    }

    /* Configure the actions. */
    for (i = 0; i < grp->action_max_per_entry; i++) {
        act = fd_lkup_action_entry_action_get(unit, group, action_base, i);
        SHR_NULL_CHECK(act, SHR_E_INTERNAL);
        if (act->flags == FD_ENTRY_ACTION_UNUSED) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_lkup_entry_action_add(unit, &einfo,
                                                        act->action,
                                                        act->param0,
                                                        act->param1));
    }

    if (update) {
        /* Update the configurations to LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_lkup_entry_update(unit, &einfo));

        ent->flags &= ~FD_ENTRY_DIRTY;
        ent->flags |=  FD_ENTRY_INSTALLED;

    } else {
        /* Insert the configurations to LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_lkup_entry_insert(unit, &einfo));
        ent->flags |=  FD_ENTRY_INSTALLED;
    }

    if (update) {
        /* Use old configurations to decrease old profile reference count. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_lkup_entry_mask_profile_restore(unit, &einfo_old));
    }

exit:
    /* Safe free handlers. */
    (void)mbcm_ltsw_flexdigest_lkup_entry_free(unit, &einfo);
    (void)mbcm_ltsw_flexdigest_lkup_entry_free(unit, &einfo_old);
    SHR_FUNC_EXIT();
}



/*!
 * \brief Remove a flex digest lookup entry from hardware.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_entry_remove(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcm_flexdigest_group_t group;
    bcmint_flexdigest_entry_info_t einfo;
    int entry_id_offset;

    SHR_FUNC_ENTER(unit);

    sal_memset(&einfo, 0, sizeof(einfo));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];

    if (!(ent->flags & FD_ENTRY_INSTALLED)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    einfo.id = ent->id;
    einfo.group_prio = grp->pri;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_lkup_entry_allocate(unit, &einfo));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_lkup_entry_lookup(unit, &einfo));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_lkup_entry_mask_profile_restore(unit, &einfo));
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_lkup_entry_delete(unit, &einfo));

    ent->flags &= ~(FD_ENTRY_INSTALLED | FD_ENTRY_DIRTY);

exit:
    (void)mbcm_ltsw_flexdigest_lkup_entry_free(unit, &einfo);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Reinstall a flex digest lookup entry into hardware.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_entry_reinstall(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcm_flexdigest_group_t group;
    int entry_id_offset;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_entry_used(unit, group, entry));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];
    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];

    if (ent->flags & FD_ENTRY_INSTALLED) {
        if (ent->flags & FD_ENTRY_DIRTY) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_lkup_entry_install(unit, entry, 1));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_lkup_entry_install(unit, entry, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a flex digest lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_entry_destroy(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    int entry_id_offset;
    bcm_flexdigest_group_t group;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_sf(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_used(unit, group));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    if (entry < grp->entry_id_base ||
        entry >= (grp->entry_id_base + grp->entry_id_count)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];

    if (ent->flags & FD_ENTRY_INSTALLED) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_lkup_entry_remove(unit, entry));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_qualifier_delete_all(unit, entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_action_delete_all(unit, entry));

    ent->id = entry_id_offset;
    ent->pri = entry_id_offset;
    ent->flags &= ~FD_ENTRY_USED;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest lookup entries in a group.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_entry_destroy_all(
    int unit,
    int group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_used(unit, group));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    for (i = 0; i < grp->entry_id_count; i++) {
        ent = &grp_res->entry_pool[i];
        if (ent->flags == FD_ENTRY_UNUSED) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_lkup_entry_destroy(unit, grp->entry_id_base + ent->id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Assign a priority to a flex digest lookup group
 *
 * \param [in] unit Unit Number.
 * \param [in] pri The group priority.
 * \param [in] group The group identifier.
 * \param [out] pri_out The assigned group priority.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_priority_assign(
    int unit,
    int pri,
    bcm_flexdigest_group_t group,
    int *pri_out)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    int i;
    uint32_t group_pri_bitmap = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pri_out, SHR_E_PARAM);

    for (i = 0; i < fd_lkup->num_groups; i++) {
        grp = &fd_lkup->groups[i];
        if (grp->flags != FD_GROUP_UNUSED) {
            group_pri_bitmap |= 1 << grp->pri;
        }
    }

    if (pri == BCM_FLEXDIGEST_GROUP_PRIO_ANY) {
        for (i = 0; i <= fd_lkup->group_prio_max; i++) {
            if (group_pri_bitmap & (1 << i)) {
                continue;
            }
            *pri_out = i;
            break;
        }
        if (i > fd_lkup->group_prio_max) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }

    } else {
        if (group_pri_bitmap & (1 << pri)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            *pri_out = pri;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flex digest group with a group Id.
 *
 * \param [in] unit Unit Number.
 * \param [in] qset The qualifier set.
 * \param [in] pri The group priority.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_create_id(
    int unit,
    bcm_flexdigest_qset_t qset,
    int pri,
    bcm_flexdigest_group_t group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_group_qualifier_presel_t *q = NULL;
    int i, j;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_id(unit, group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_info(unit, qset, pri));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    if (grp->flags != FD_GROUP_UNUSED) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_priority_assign(unit, pri, group, &pri));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_resource_allocate(unit, group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_resource_init(unit, group));

    for (i = 0, j = 0; i < bcmFlexDigestQualifyCount; i++) {
        if (BCM_FLEXDIGEST_QSET_TEST(qset, i)) {
            for (; j < fd_lkup->qualifier_presel_count;) {
                q = &grp_res->qualifier_presel_pool[j++];
                if (!q->valid) {
                    q->qualifier = i;
                    q->valid = true;
                    break;
                }
            }
        }
    }
    grp->pri = pri;
    grp->flags |= FD_GROUP_USED;

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_update_presel(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flex digest group.
 *
 * \param [in] unit Unit Number.
 * \param [in] qset The qualifier set.
 * \param [in] pri The group priority.
 * \param [out] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_create(
    int unit,
    bcm_flexdigest_qset_t qset,
    int pri,
    bcm_flexdigest_group_t *group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_sf(unit));

    SHR_NULL_CHECK(group, SHR_E_PARAM);

    for (i = 0; i < fd_lkup->num_groups; i++) {
        grp = &fd_lkup->groups[i];
        if (grp->flags == FD_GROUP_UNUSED) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_lkup_group_create_id(unit, qset, pri, i));
            *group = i;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_RESOURCE);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the flex digest group configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 * \param [out] qset The qualifier set.
 * \param [out] pri The group priority.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_get(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_qset_t *qset,
    int *pri)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_group_qualifier_presel_t *q = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_used(unit, group));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    if (qset) {
        BCM_FLEXDIGEST_QSET_INIT(*qset);
        for (i = 0; i < fd_lkup->qualifier_presel_count; i++) {
            q = &grp_res->qualifier_presel_pool[i];
            if (q->valid) {
                BCM_FLEXDIGEST_QSET_ADD(*qset, q->qualifier);
            }
        }
    }
    if (pri) {
        *pri = grp->pri;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a flex digest group.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_destroy(
    int unit,
    bcm_flexdigest_group_t group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_used(unit, group));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    for (i = 0; i < grp->entry_id_count; i++) {
        ent = &grp_res->entry_pool[i];
        if (ent->flags != FD_ENTRY_UNUSED) {
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_resource_free(unit, group));

    grp->pri = 0;
    grp->flags = FD_GROUP_UNUSED;

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_update_presel(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest groups.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_lkup_group_destroy_all(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    int group;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_sf(unit));

    for (group = 0; group < fd_lkup->num_groups; group++) {
        grp = &fd_lkup->groups[group];
        if (grp->flags & FD_GROUP_USED) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_lkup_entry_destroy_all(unit, group));
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_lkup_group_destroy(unit, group));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create a flex digest hashing profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] option Create option. See BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_xxx
 * \param [in/out] hash_profile_id Hashing profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_profile_create(
    int unit,
    uint32 option,
    int *hash_profile_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_hash_db_t *fd_hash = &fd_db->hash;
    fd_profile_t *profile = &fd_hash->profile;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(hash_profile_id, SHR_E_PARAM);

    if (!FD_SUB_FEATURE_TEST(fd_hash, HASH_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_profile_create(profile, option, hash_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_hash_profile_create(unit, *hash_profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a flex digest hashing profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] hash_profile_id Hashing profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_profile_destroy(
    int unit,
    int hash_profile_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_hash_db_t *fd_hash = &fd_db->hash;
    fd_profile_t *profile = &fd_hash->profile;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_hash, HASH_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_profile_destory(profile, hash_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_hash_profile_destroy(unit, hash_profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest hashing profiles.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_profile_destroy_all(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_hash_db_t *fd_hash = &fd_db->hash;
    fd_profile_t *profile = &fd_hash->profile;
    int id;

    SHR_FUNC_ENTER(unit);

    if (!FD_SUB_FEATURE_TEST(fd_hash, HASH_PROFILE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    FD_PROFILE_ID_ITER(profile, id) {
        if (FD_PROFILE_ID_TEST(profile, id)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_profile_destory(profile, id));

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexdigest_hash_profile_destroy(unit, id));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure a flex digest hashing profiles.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Hashing profile identifier.
 * \param [in] type The hashing profile control type.
 * \param [in] value The hashing profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_profile_set(
    int unit,
    int hash_profile_id,
    bcm_flexdigest_hash_profile_control_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_hash_profile_id(unit, hash_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_hash_profile(unit, hash_profile_id, type, value));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_hash_profile_set(unit, hash_profile_id,
                                               type, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a flex digest hashing profiles.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Hashing profile identifier.
 * \param [in] type The hashing profile control type.
 * \param [out] value The hashing profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_profile_get(
    int unit,
    int hash_profile_id,
    bcm_flexdigest_hash_profile_control_t type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_hash_profile_id(unit, hash_profile_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_hash_profile_get(unit, hash_profile_id,
                                               type, value));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover flexdigest hashing profile identifier bitmap.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile The Flex Digest profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_profile_recover(int unit, fd_profile_t *profile)
{
    int i, rv;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    FD_PROFILE_ID_ITER(profile, i) {
        rv = mbcm_ltsw_flexdigest_hash_profile_configured(unit, i);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        FD_PROFILE_ID_ADD(profile, i);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Dump/Show a flexdigest hashing profile configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile The Flex Digest profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_profile_dump(int unit, int profile_id)
{
    int i, value;
    char *str[] = BCM_FLEXDIGEST_HASH_PROFILE_CONTROL_STRINGS;
    char *func_str[] = {"None",
                        "CRC16", "CRC16XOR1", "CRC16XOR2", "CRC16XOR4",
                        "CRC16XOR8", "XOR16", "CRC16CCITT", "CRC32LO",
                        "CRC32HI", "CRC32_ETH_LO", "CRC32_ETH_HI", "KOOPMAN_LO",
                        "KOOPMAN_HI",
                        "Unknown"};

    SHR_FUNC_ENTER(unit);

    LOG_CLI((BSL_META("\t %d):\n"), profile_id));
    for (i = 0;
         i < bcmFlexDigestHashProfileControlBinA0FunctionSelection; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_hash_profile_get(unit, profile_id, i, &value));
        LOG_CLI((BSL_META("\t\t%s: %s\n"),
                 str[i], value ? "Enable" : "Disable"));
    }
    for (i = bcmFlexDigestHashProfileControlBinA0FunctionSelection;
         i < bcmFlexDigestHashProfileControlCount; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_hash_profile_get(unit, profile_id, i, &value));
        LOG_CLI((BSL_META("\t\t%s: %s\n"), str[i], func_str[value]));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Configure salt for a specific hashing bin set.
 *
 * \param [in] unit Unit Number.
 * \param [in] bin_set Hashing bin set.
 * \param [in] array_size The number of hashing bins in a bin set.
 * \param [in] salt_array The hashing salt array.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_salt_set(
    int unit,
    bcm_flexdigest_hash_bin_set_t bin_set,
    int array_size,
    uint16_t *salt_array)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_hash_salt_set(unit, bin_set, array_size, salt_array));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get salt configuration for a specific hashing bin set.
 *
 * \param [in] unit Unit Number.
 * \param [in] bin_set Hashing bin set.
 * \param [in] array_size The maximum number of hashing bins array.
 * \param [out] salt_array The hashing salt array.
 * \param [out] count The number of hashing bins in a bin set.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_salt_get(
    int unit,
    bcm_flexdigest_hash_bin_set_t bin_set,
    int array_size,
    uint16_t *salt_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_hash_salt_get(unit, bin_set, array_size, salt_array, count));

exit:
    SHR_FUNC_EXIT();
}

#if defined(BROADCOM_DEBUG)
/*!
 * \brief Dump/Show the hashing salt configuration.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_hash_salt_dump(int unit)
{
    int rv, i, j;
    char *str[] = BCM_FLEXDIGEST_HASH_BIN_SET_STRINGS;
    int sz[bcmFlexDigestHashBinCount] = {0};
    uint16_t salt[bcmFlexDigestHashBinCount][FD_HASH_BIN_MAX] = {{0}};

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < bcmFlexDigestHashBinCount; i++) {
        rv = fd_hash_salt_get(unit, i, FD_HASH_BIN_MAX, salt[i], &sz[i]);
        if (rv != SHR_E_NONE && rv != SHR_E_UNAVAIL && rv != SHR_E_INTERNAL) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    LOG_CLI((BSL_META("\t\t\n")));
    for (i = 0; i < bcmFlexDigestHashBinCount; i++) {
        LOG_CLI((BSL_META("  %s\n"), str[i]));
    }
    LOG_CLI((BSL_META("\n")));
    for (j = 0; j < FD_HASH_BIN_MAX; j++) {
        LOG_CLI((BSL_META("\t\t")));
        for (i = 0; i < bcmFlexDigestHashBinCount; i++) {
            LOG_CLI((BSL_META("  %08x\n"), salt[i][j]));
        }
        LOG_CLI((BSL_META("\n")));
    }

exit:
    SHR_FUNC_EXIT();
}
#endif

/*!
 * \brief De-initialize the flex digest normalization database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
fd_db_norm_deinit(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = NULL;

    SHR_FUNC_ENTER(unit);

    if (FD_SUB_FEATURE_TEST(fd_norm, NORM_SEED_PROFILE)) {
        profile = &fd_norm->seed_profile;
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_profile_deinit(profile));
    }
    if (FD_SUB_FEATURE_TEST(fd_norm, NORM_PROFILE)) {
        profile = &fd_norm->profile;
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_profile_deinit(profile));
    }
    fd_norm->features = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the flex digest normalization database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_db_norm_init(int unit, int warm)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_profile_t *profile = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_feature_get(unit,
                                          BCMGENE_FLEXDIGEST_F_NORM,
                                          &fd_norm->features));
    if (FD_SUB_FEATURE_TEST(fd_norm, NORM_SEED_PROFILE)) {
        profile = &fd_norm->seed_profile;
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoNormSeedProfileIdCount,
                    &profile->id_count));
        profile->desc = "bcmFlexDigestNormSeedProfile";
        profile->recover = &fd_norm_seed_profile_recover;
        profile->dump = &fd_norm_seed_profile_dump;
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_profile_init(profile));
        if (warm) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_profile_recover(unit, profile));
        }
    }
    if (FD_SUB_FEATURE_TEST(fd_norm, NORM_PROFILE)) {
        profile = &fd_norm->profile;
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoNormProfileIdCount,
                    &profile->id_count));
        profile->desc = "bcmFlexDigestNormProfile";
        profile->recover = &fd_norm_profile_recover;
        profile->dump = &fd_norm_profile_dump;
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_profile_init(profile));
        if (warm) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_profile_recover(unit, profile));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the flex digest lookup database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
fd_db_lkup_deinit(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;

    SHR_FUNC_ENTER(unit);

    if (fd_lkup->group_resources) {
        SHR_FREE(fd_lkup->group_resources);
    }

    sal_memset(fd_lkup, 0, sizeof(*fd_lkup));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover the flex digest lookup database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_db_lkup_recover(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    if (FD_SUB_FEATURE_TEST(fd_lkup, LKUP_GROUP)) {
        for (i = 0; i < fd_lkup->num_groups; i++) {
            grp = &fd_lkup->groups[i];
            if (grp->flags != FD_GROUP_UNUSED) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (fd_lkup_group_resource_allocate(unit, i));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the flex digest lookup database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_db_lkup_init(int unit, int warm)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    void *ptr = NULL;
    bcmint_flexdigest_lkup_group_t *group = NULL;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;
    int i, qualifier_max_per_entry, action_max_per_entry;
    shr_ha_sub_id sub_id = 0;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_feature_get(unit,
                                          BCMGENE_FLEXDIGEST_F_LKUP,
                                          &fd_lkup->features));
    if (FD_SUB_FEATURE_TEST(fd_lkup, LKUP_MATCH_ID)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoNumMatchIds,
                    &fd_lkup->num_match_ids));
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoMatchWidth,
                    &fd_lkup->match_width));
        if (fd_lkup->match_pool == NULL) {
            ha_array_size = fd_lkup->num_match_ids * fd_lkup->match_width;
            ha_instance_size = sizeof(bcmint_flexdigest_lkup_match_t);
            ha_req_size = ha_array_size * ha_instance_size;
            ha_alloc_size = ha_req_size;
            ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                         BCMI_HA_COMP_ID_FLEXDIGEST,
                                         sub_id,
                                         "bcmFDMatchPool",
                                         &ha_alloc_size);
            SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
            SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                    SHR_E_MEMORY : SHR_E_NONE);
            if (!warm) {
                sal_memset(ptr, 0, ha_alloc_size);
            }
            rv = bcmi_ltsw_issu_struct_info_report(unit,
                                         BCMI_HA_COMP_ID_FLEXDIGEST,
                                         sub_id,
                                         0, ha_instance_size, ha_array_size,
                                         BCMINT_FLEXDIGEST_LKUP_MATCH_T_ID);
            if (rv != SHR_E_EXISTS) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            fd_lkup->match_pool = ptr;
            ptr = NULL;
            sub_id++;
        }
        if (fd_lkup->match_info_pool == NULL) {
            ha_array_size = fd_lkup->num_match_ids;
            ha_instance_size = sizeof(bcmint_flexdigest_lkup_match_info_t);
            ha_req_size = ha_array_size * ha_instance_size;
            ha_alloc_size = ha_req_size;
            ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                         BCMI_HA_COMP_ID_FLEXDIGEST,
                                         sub_id,
                                         "bcmFDMatchInfoPool",
                                         &ha_alloc_size);
            SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
            SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                    SHR_E_MEMORY : SHR_E_NONE);
            if (!warm) {
                sal_memset(ptr, 0, ha_alloc_size);
            }
            rv = bcmi_ltsw_issu_struct_info_report(unit,
                                         BCMI_HA_COMP_ID_FLEXDIGEST,
                                         sub_id,
                                         0, ha_instance_size, ha_array_size,
                                         BCMINT_FLEXDIGEST_LKUP_MATCH_INFO_T_ID);
            if (rv != SHR_E_EXISTS) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            fd_lkup->match_info_pool = ptr;
            ptr = NULL;
            sub_id++;
        }
        if (fd_lkup->match_info_pool) {
            for (i = 0; i < fd_lkup->num_match_ids; i++) {
                match_info = &fd_lkup->match_info_pool[i];
                match_info->id = i;
            }
        }
    }
    if (FD_SUB_FEATURE_TEST(fd_lkup, LKUP_GROUP)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoNumGroups,
                    &fd_lkup->num_groups));
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoNumEntries,
                    &fd_lkup->num_entries));
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoGroupPrioMax,
                    &fd_lkup->group_prio_max));
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoEntryPrioMax,
                    &fd_lkup->entry_prio_max));
        fd_lkup->num_entries_per_group = fd_lkup->num_entries / fd_lkup->num_groups;

        ha_array_size = fd_lkup->num_groups;
        ha_instance_size = sizeof(bcmint_flexdigest_lkup_group_t);
        ha_req_size = ha_array_size * ha_instance_size;
        ha_alloc_size = ha_req_size;
        ptr = bcmi_ltsw_ha_mem_alloc(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     "bcmFDGroupInfo",
                                     &ha_alloc_size);
        SHR_NULL_CHECK(ptr, SHR_E_MEMORY);
        SHR_IF_ERR_VERBOSE_EXIT((ha_alloc_size < ha_req_size) ?
                                SHR_E_MEMORY : SHR_E_NONE);
        if (!warm) {
            sal_memset(ptr, 0, ha_alloc_size);
        }
        rv = bcmi_ltsw_issu_struct_info_report(unit,
                                     BCMI_HA_COMP_ID_FLEXDIGEST,
                                     sub_id,
                                     0, ha_instance_size, ha_array_size,
                                     BCMINT_FLEXDIGEST_LKUP_GROUP_T_ID);
        if (rv != SHR_E_EXISTS) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        fd_lkup->groups = ptr;
        ptr = NULL;
        sub_id++;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoQualifierMaxPerEntry,
                    &qualifier_max_per_entry));
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoActionMaxPerEntry,
                    &action_max_per_entry));

        fd_lkup->qualifier_presel_count = qualifier_max_per_entry;
        if (fd_lkup->num_match_ids) {
            fd_lkup->qualifier_presel_count -= 1;
        }
        for (i = 0; i < fd_lkup->num_groups; i++) {
            group = &fd_lkup->groups[i];
            group->id = i;
            group->sub_comp_id_base = i * FD_LKUP_GROUP_SUB_ID_COUNT + sub_id;
            group->qualifier_max_per_entry = qualifier_max_per_entry;
            group->action_max_per_entry = action_max_per_entry;
            group->entry_id_base = i * fd_lkup->num_entries_per_group;
            group->entry_id_count = fd_lkup->num_entries_per_group;
        }

        SHR_ALLOC(fd_lkup->group_resources,
                  fd_lkup->num_groups * sizeof(fd_lkup_group_resource_t),
                  "bcmFlexDigestGroupResource");
        SHR_NULL_CHECK(fd_lkup->group_resources, SHR_E_MEMORY);
        sal_memset(fd_lkup->group_resources, 0,
                   fd_lkup->num_groups * sizeof(fd_lkup_group_resource_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (fd_lkup->group_resources) {
            SHR_FREE(fd_lkup->group_resources);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the flex digest hashing database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
fd_db_hash_deinit(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_hash_db_t *fd_hash = &fd_db->hash;
    fd_profile_t *profile = NULL;

    SHR_FUNC_ENTER(unit);

    if (FD_SUB_FEATURE_TEST(fd_hash, HASH_PROFILE)) {
        profile = &fd_hash->profile;
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_profile_deinit(profile));
    }
    fd_hash->features = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the flex digest hashing database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_db_hash_init(int unit, int warm)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_hash_db_t *fd_hash = &fd_db->hash;
    fd_profile_t *profile = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_feature_get(unit,
                                          BCMGENE_FLEXDIGEST_F_HASH,
                                          &fd_hash->features));
    if (FD_SUB_FEATURE_TEST(fd_hash, HASH_PROFILE)) {
        profile = &fd_hash->profile;
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexdigest_info_get(unit,
                    bcmiFlexDigestInfoHashProfileIdCount,
                    &profile->id_count));
        profile->desc = "bcmFlexDigestHashProfile";
        profile->recover = &fd_hash_profile_recover;
        profile->dump = &fd_hash_profile_dump;
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_profile_init(profile));
        if (warm) {
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_profile_recover(unit, profile));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Recover the flex digest database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
fd_db_recover(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_db_lkup_recover(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the flex digest database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
fd_db_deinit(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];

    SHR_FUNC_ENTER(unit);

    fd_lock(unit);

    fd_db->signature = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_db_norm_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_db_lkup_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_db_hash_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_db_deinit(unit));

exit:
    fd_unlock(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the flex digest database.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_db_init(int unit)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    fd_lock(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_db_init(unit, warm));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_db_norm_init(unit, warm));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_db_lkup_init(unit, warm));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_db_hash_init(unit, warm));

    if (warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_db_recover(unit));
    }

    fd_db->signature = FD_SIGNATURE;

exit:
    if (SHR_FUNC_ERR() && !warm) {
        (void)fd_db_deinit(unit);
    }
    fd_unlock(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the flex digest module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_init(int unit)
{
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the flex digest module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexdigest_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

#if defined(BROADCOM_DEBUG)
/*!
 * \brief Show binding entries of a flex digest match identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The matching criteria identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_match_id_binding_dump(
    int unit,
    bcm_flexdigest_match_id_t match_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    bcmint_flexdigest_lkup_entry_qualifier_t *qual = NULL;
    int i, j, k, first = 1, eid, qualifier_base;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id(unit, match_id));

    LOG_CLI((BSL_META_U(unit, "    Binding entries are: ")));

    for (i = 0; i < fd_lkup->num_groups; i++) {
        grp = &fd_lkup->groups[i];
        grp_res = &fd_lkup->group_resources[i];
        if (grp->flags == FD_GROUP_UNUSED) {
            continue;
        }
        for (j = 0; j < grp->entry_id_count; j++) {
            eid = j + grp->entry_id_base;
            ent = &grp_res->entry_pool[j];
            if (ent->flags == FD_ENTRY_UNUSED) {
                continue;
            }
            qualifier_base = j * grp->qualifier_max_per_entry;
            for (k = 0; k < grp->qualifier_max_per_entry; k++) {
                qual = fd_lkup_qualifier_entry_qualifier_get(unit, i,
                                                             qualifier_base, k);
                SHR_NULL_CHECK(qual, SHR_E_INTERNAL);
                if (qual->flags == FD_ENTRY_QUAL_UNUSED) {
                    continue;
                }
                if (qual->qualifier == bcmFlexDigestQualifyMatchId &&
                    qual->data == match_id) {
                    if (first) {
                        LOG_CLI((BSL_META_U(unit, "%d"), eid));
                        first = 0;
                    } else {
                        LOG_CLI((BSL_META_U(unit, ", %d"), eid));
                    }
                }
            }
        }
    }
    LOG_CLI((BSL_META_U(unit, "\n")));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Show contents of a flex digest match identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The matching criteria identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_match_id_dump(
    int unit,
    bcm_flexdigest_match_id_t match_id)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_match_info_t *match_info = NULL;
    bcmint_flexdigest_lkup_match_t *mat;
    int i, first = 1;
    char *match_str[] = BCM_FLEXDIGEST_MATCH_STRINGS;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_match_id(unit, match_id));

    match_info = &fd_lkup->match_info_pool[match_id];

    if (match_info->flags & FD_MATCH_ID_USED) {
        LOG_CLI((BSL_META_U(unit,
                            "MatchId %d: [Ref.%d]\n"),
                            match_info->id, match_info->ref_count));
        LOG_CLI((BSL_META_U(unit, "    Matches={")));
        for (i = 0; i < fd_lkup->match_width; i++) {
            mat = fd_lkup_match_id_match_get(unit, match_id, i);
            SHR_NULL_CHECK(mat, SHR_E_INTERNAL);
            if (mat->flags == FD_MATCH_UNUSED) {
                continue;
            }
            if (first) {
                LOG_CLI((BSL_META_U(unit, "%s"), match_str[mat->match]));
                first = 0;
            } else {
                LOG_CLI((BSL_META_U(unit, ", %s"), match_str[mat->match]));
            }
        }
        LOG_CLI((BSL_META_U(unit, "}\n")));

        SHR_IF_ERR_VERBOSE_EXIT
            (fd_match_id_binding_dump(unit, match_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Show contents of a flex digest lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_entry_dump(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_entry_t *ent = NULL;
    int entry_id_offset;
    bcm_flexdigest_group_t group;
    bcmint_flexdigest_lkup_entry_qualifier_t *qual;
    bcmint_flexdigest_lkup_entry_action_t *act;
    char *qual_str[] = BCM_FLEXDIGEST_QUALIFY_STRINGS;
    char *action_str[] = BCM_FLEXDIGEST_ACTION_STRINGS;
    int i, qualifier_base, action_base;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lkup_group_find_by_entry(unit, entry, &group));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_used(unit, group));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    if (entry < grp->entry_id_base ||
        entry >= (grp->entry_id_base + grp->entry_id_count)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    entry_id_offset = entry - grp->entry_id_base;
    ent = &grp_res->entry_pool[entry_id_offset];
    qualifier_base = entry_id_offset * grp->qualifier_max_per_entry;
    action_base = entry_id_offset * grp->action_max_per_entry;

    if (ent->flags & FD_ENTRY_USED) {
        LOG_CLI((BSL_META_U(unit,
                            "    EID %d [%d:%d]"), entry, ent->id, ent->pri));
        if (ent->flags & FD_ENTRY_INSTALLED) {
            LOG_CLI((BSL_META_U(unit, " Installed")));
            if (ent->flags & FD_ENTRY_DIRTY) {
                LOG_CLI((BSL_META_U(unit, " Dirty")));
            }
        } else {
            LOG_CLI((BSL_META_U(unit, " Unstalled")));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        LOG_CLI((BSL_META_U(unit, "\t %-20s      Data      Mask\n"), "Qualifier"));
        LOG_CLI((BSL_META_U(unit, "\t--------------------------------------------\n")));
        for (i = 0; i < grp->qualifier_max_per_entry; i++) {
            qual = fd_lkup_qualifier_entry_qualifier_get(unit, group,
                                                         qualifier_base, i);
            SHR_NULL_CHECK(qual, SHR_E_INTERNAL);
            if (qual->flags & FD_ENTRY_QUAL_USED) {
                if (qual->qualifier == bcmFlexDigestQualifyMatchId) {
                    LOG_CLI((BSL_META_U(unit, "\t %-20s      %d\n"),
                                        qual_str[qual->qualifier], qual->data));
                } else {
                    LOG_CLI((BSL_META_U(unit, "\t %-20s      0x%x       0x%x\n"),
                                        qual_str[qual->qualifier],
                                        qual->data, qual->mask));
                }
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        LOG_CLI((BSL_META_U(unit, "\t %-20s      P0        P1\n"), "Action"));
        LOG_CLI((BSL_META_U(unit, "\t--------------------------------------------\n")));
        for (i = 0; i < grp->action_max_per_entry; i++) {
            act = fd_lkup_action_entry_action_get(unit, group, action_base, i);
            SHR_NULL_CHECK(act, SHR_E_INTERNAL);
            if (act->flags & FD_ENTRY_ACTION_USED) {
                LOG_CLI((BSL_META_U(unit, "\t %-20s      %-5d     0x%04x\n"),
                                    action_str[act->action],
                                    act->param0, act->param1));
            }
        }
    } else {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Show contents of a flex digest group.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_group_dump(
    int unit,
    bcm_flexdigest_group_t group)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    bcmint_flexdigest_lkup_group_t *grp = NULL;
    fd_lkup_group_resource_t *grp_res = NULL;
    bcmint_flexdigest_lkup_group_qualifier_presel_t *q = NULL;
    int i, first = 1, eid;
    char *qual_str[] = BCM_FLEXDIGEST_QUALIFY_STRINGS;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_lkup_group_id(unit, group));

    grp = &fd_lkup->groups[group];
    grp_res = &fd_lkup->group_resources[group];

    if (grp->flags & FD_GROUP_USED) {
        LOG_CLI((BSL_META_U(unit,
                            "GID %d:\n"), grp->id));
        LOG_CLI((BSL_META_U(unit, "    Qset={")));
        for (i = 0; i < fd_lkup->qualifier_presel_count; i++) {
            q = &grp_res->qualifier_presel_pool[i];
            if (q->valid) {
                if (first) {
                    LOG_CLI((BSL_META_U(unit, "%s"), qual_str[q->qualifier]));
                    first = 0;
                } else {
                    LOG_CLI((BSL_META_U(unit, ", %s"), qual_str[q->qualifier]));
                }
            }
        }
        LOG_CLI((BSL_META_U(unit, "}\n")));
        LOG_CLI((BSL_META_U(unit, "    Priority: %d\n"), grp->pri));
        LOG_CLI((BSL_META_U(unit, "    Entry range: [%d-%d]\n"),
                            grp->entry_id_base,
                            grp->entry_id_base + grp->entry_id_count - 1));
        for (i = 0; i < grp->entry_id_count; i++) {
            eid = grp->entry_id_base + i;
            rv = fd_entry_dump(unit, eid);
            if (rv != SHR_E_NOT_FOUND) {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Show current software state of the flex digest module.
 *
 * \param [in] unit Unit Number.
 * \param [in] pfx The showing prefix.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
fd_show(
    int unit,
    const char *pfx)
{
    fd_db_t *fd_db = &flexdigest_db[unit];
    fd_norm_db_t *fd_norm = &fd_db->norm;
    fd_lkup_db_t *fd_lkup = &fd_db->lkup;
    fd_hash_db_t *fd_hash = &fd_db->hash;
    int i, rv;

    SHR_FUNC_ENTER(unit);

    if (!fd_db_is_initialized(unit)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s:\tunit %d:\n"), pfx, unit));

    LOG_CLI((BSL_META_U(unit,
                        "Normalization:(0x%08x)\n"), fd_norm->features));
    if (FD_SUB_FEATURE_TEST(fd_norm, NORM_SEED_PROFILE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_profile_dump(unit, &fd_norm->seed_profile));
    }
    if (FD_SUB_FEATURE_TEST(fd_norm, NORM_PROFILE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_profile_dump(unit, &fd_norm->profile));
    }

    LOG_CLI((BSL_META_U(unit,
                        "Lookup:(0x%08x)\n"), fd_lkup->features));
    if (FD_SUB_FEATURE_TEST(fd_norm, LKUP_GROUP)) {
        LOG_CLI((BSL_META_U(unit,
                            "\tNumber of Groups: %d\n"),
                            fd_lkup->num_groups));
        LOG_CLI((BSL_META_U(unit,
                            "\tNumber of Entries: %d\n"),
                            fd_lkup->num_entries));
        LOG_CLI((BSL_META_U(unit,
                            "\tNumber of Entries per Group: %d\n"),
                            fd_lkup->num_entries_per_group));
        LOG_CLI((BSL_META_U(unit,
                            "\tNumber of MatchIds: %d\n"),
                            fd_lkup->num_match_ids));
        LOG_CLI((BSL_META_U(unit,
                            "\tGroup Priority Range: [0-%d]\n"),
                            fd_lkup->group_prio_max));
        LOG_CLI((BSL_META_U(unit,
                            "\tEntry Priority Range: [0-%d]\n"),
                            fd_lkup->entry_prio_max));
        LOG_CLI((BSL_META_U(unit,
                            "\tQualifier Presel Count Max: %d\n"),
                            fd_lkup->qualifier_presel_count));

        for (i = 0; i < fd_lkup->num_match_ids; i++) {
            rv = fd_validate_lkup_match_id(unit, i);
            if (SHR_FAILURE(rv)) {
                continue;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_match_id_dump(unit, i));
        }

        for (i = 0; i < fd_lkup->num_groups; i++) {
            rv = fd_validate_lkup_group_used(unit, i);
            if (SHR_FAILURE(rv)) {
                continue;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (fd_group_dump(unit, i));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "Hashing:(0x%08x)\n"), fd_hash->features));
    if (FD_SUB_FEATURE_TEST(fd_norm, HASH_PROFILE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_profile_dump(unit, &fd_hash->profile));
    }
    if (FD_SUB_FEATURE_TEST(fd_norm, HASH_SALT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_hash_salt_dump(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

#endif

/******************************************************************************
 * Public functions
 */

int
bcmi_ltsw_flexdigest_match_id_ref_count_get(
    int unit,
    int match_id,
    int *ref_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_match_id_ref_count_get(unit, match_id, ref_count));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the flex digest module.
 *
 * Initial flex-digest database data structure and
 * clear the flex-digest related logical tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    if (fd_db_is_initialized(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (fd_db_deinit(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lock_create(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_db_init(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the flex-digest module.
 *
 * Detach flex-digest database data structure.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcm_ltsw_flexdigest_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_db_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_lock_destroy(unit));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create a flex digest normalization seed profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] option Create option. See BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_xxx
 * \param [in/out] seed_profile_id Seed profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_seed_profile_create(
    int unit,
    uint32 option,
    int *seed_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_seed_profile_create(unit, option, seed_profile_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a flex digest normalization seed profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] seed_profile_id Seed profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_seed_profile_destroy(
    int unit,
    int seed_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_seed_profile_destroy(unit, seed_profile_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest normalization seed profiles.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_seed_profile_destroy_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_seed_profile_destroy_all(unit));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure the flex digest normalization seed profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] seed_profile_id Seed profile identifier.
 * \param [in] type The normalization seed profile control type.
 * \param [in] value The normalization seed profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_seed_profile_set(
    int unit,
    int seed_profile_id,
    bcm_flexdigest_norm_seed_control_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_seed_profile_set(unit, seed_profile_id, type, value));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the flex digest normalization seed profile configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] seed_profile_id Seed profile identifier.
 * \param [in] type The normalization seed profile control type.
 * \param [out] value The normalization seed profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_seed_profile_get(
    int unit,
    int seed_profile_id,
    bcm_flexdigest_norm_seed_control_t type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_seed_profile_get(unit, seed_profile_id, type, value));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flex digest normalization profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] option Create option. See BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_xxx
 * \param [in/out] norm_profile_id Normalization profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_profile_create(
    int unit,
    uint32 option,
    int *norm_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_profile_create(unit, option, norm_profile_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a flex digest normalization profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] norm_profile_id Normalization profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_profile_destroy(
    int unit,
    int norm_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_profile_destroy(unit, norm_profile_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest normalization profiles.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_profile_destroy_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_profile_destroy_all(unit));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure flex digest normalization profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] norm_profile_id Normalization profile identifier.
 * \param [in] type The normalization profile control type.
 * \param [in] value The normalization profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_profile_set(
    int unit,
    int norm_profile_id,
    bcm_flexdigest_norm_profile_control_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_profile_set(unit, norm_profile_id, type, value));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex digest normalization profile configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] norm_profile_id Normalization profile identifier.
 * \param [in] type The normalization profile control type.
 * \param [out] value The normalization profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_norm_profile_get(
    int unit,
    int norm_profile_id,
    bcm_flexdigest_norm_profile_control_t type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_norm_profile_get(unit, norm_profile_id, type, value));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Create a flex digest group.
 *
 * \param [in] unit Unit Number.
 * \param [in] qset The qualifier set.
 * \param [in] pri The group priority.
 * \param [out] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_group_create(
    int unit,
    bcm_flexdigest_qset_t qset,
    int pri,
    bcm_flexdigest_group_t *group)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_group_create(unit, qset, pri, group));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flex digest group with a group Id.
 *
 * \param [in] unit Unit Number.
 * \param [in] qset The qualifier set.
 * \param [in] pri The group priority.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_group_create_id(
    int unit,
    bcm_flexdigest_qset_t qset,
    int pri,
    bcm_flexdigest_group_t group)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_group_create_id(unit, qset, pri, group));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the flex digest group configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 * \param [out] qset The qualifier set.
 * \param [out] pri The group priority.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_group_get(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_qset_t *qset,
    int *pri)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_group_get(unit, group, qset, pri));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a flex digest group.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_group_destroy(
    int unit,
    bcm_flexdigest_group_t group)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_group_destroy(unit, group));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest groups.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_group_destroy_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_group_destroy_all(unit));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flex digest lookup entry for a given groups.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 * \param [out] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_create(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_entry_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_entry_create(unit, group, entry));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a flex digest lookup entry with entry Identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_create_id(
    int unit,
    bcm_flexdigest_group_t group,
    bcm_flexdigest_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_entry_create_id(unit, group, entry));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a flex digest lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_destroy(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_entry_destroy(unit, entry));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest lookup entries in a group.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_destroy_all(
    int unit,
    int group)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_entry_destroy_all(unit, group));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure a flex digest lookup entry priority.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] pri The entry priority.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_priority_set(
    int unit,
    bcm_flexdigest_entry_t entry,
    int pri)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_entry_priority_set(unit, entry, pri));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the flex digest lookup entry priority.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] pri The entry priority.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_priority_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    int *pri)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_entry_priority_get(unit, entry, pri));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Install a flex digest lookup entry into hardware.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_install(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_entry_install(unit, entry, 0));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove a flex digest lookup entry from hardware.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_remove(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_entry_remove(unit, entry));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Reinstall a flex digest lookup entry into hardware.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_reinstall(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_entry_reinstall(unit, entry));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a logical match identifier for flex digest lookup entry key.
 *
 * \param [in] unit Unit Number.
 * \param [out] match_id The logical match identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_match_id_create(
    int unit,
    bcm_flexdigest_match_id_t *match_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_match_id_create(unit, match_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a logical match identifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_match_id_destroy(
    int unit,
    bcm_flexdigest_match_id_t match_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_match_id_destroy(unit, match_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all logical match identifiers.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_match_id_destroy_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_match_id_destroy_all(unit));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a matching criteria into a logical match identifiers.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 * \param [in] match The matching criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_match_add(
    int unit,
    bcm_flexdigest_match_id_t match_id,
    bcm_flexdigest_match_t match)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_match_add(unit, match_id, match));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a matching rule from a logical match identifiers.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 * \param [in] match The matching criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_match_delete(
    int unit,
    bcm_flexdigest_match_id_t match_id,
    bcm_flexdigest_match_t match)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_match_delete(unit, match_id, match));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the matching criterias from a logical match identifiers.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 * \param [in] array_size The maximum number of matching criteria array.
 * \param [out] match_array The matching criteria array.
 * \param [out] count The number of matching criteria.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_match_get(
    int unit,
    bcm_flexdigest_match_id_t match_id,
    int array_size,
    bcm_flexdigest_match_t *match_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_match_get(unit, match_id, array_size, match_array, count));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all matching criterias associated with a logical match identifiers.
 *
 * \param [in] unit Unit Number.
 * \param [in] match_id The logical match identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_match_delete_all(
    int unit,
    bcm_flexdigest_match_id_t match_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_match_delete_all(unit, match_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add MatchId qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] match_id The logical match identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_MatchId(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_match_id_t match_id)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyMatchId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = match_id;
    m = 0xffff;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the MatchId value.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] match_id The logical match identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_MatchId_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_match_id_t *match_id)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyMatchId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (match_id) {
        *match_id = d;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add L4Valid qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L4Valid(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL4Valid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for L4Valid qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L4Valid_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL4Valid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Add MyStation1Hit qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_MyStation1Hit(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyMyStation1Hit;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for MyStation1Hit qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_MyStation1Hit_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyMyStation1Hit;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Add TunnelProcessingResults1 qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_TunnelProcessingResults1(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyTunnelProcessingResults1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for TunnelProcessingResults1 qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_TunnelProcessingResults1_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyTunnelProcessingResults1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Add TunnelProcessingResultsRaw qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_TunnelProcessingResultsRaw(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyTunnelProcessingResultsRaw;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for TunnelProcessingResultsRaw qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_TunnelProcessingResultsRaw_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyTunnelProcessingResultsRaw;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Add FlexHve2ResultSet1 qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_FlexHve2ResultSet1(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyFlexHve2ResultSet1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for FlexHve2ResultSet1 qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_FlexHve2ResultSet1_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyFlexHve2ResultSet1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add FixedHve2ResultSet0 qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_FixedHve2ResultSet0(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyFixedHve2ResultSet0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for FixedHve2ResultSet0 qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_FixedHve2ResultSet0_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyFixedHve2ResultSet0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add FlexHve1ResultSet1 qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_FlexHve1ResultSet1(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyFlexHve1ResultSet1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    fd_lock(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for FlexHve1ResultSet1 qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_FlexHve1ResultSet1_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyFlexHve1ResultSet1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add FixedHve1ResultSet0 qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_FixedHve1ResultSet0(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyFixedHve1ResultSet0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for FixedHve1ResultSet0 qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_FixedHve1ResultSet0_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyFixedHve1ResultSet0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add L2IifFlexDigestCtrlIdB qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L2IifFlexDigestCtrlIdB(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdB;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for L2IifFlexDigestCtrlIdB qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L2IifFlexDigestCtrlIdB_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdB;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add L2IifFlexDigestCtrlIdA qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L2IifFlexDigestCtrlIdA(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdA;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for L2IifFlexDigestCtrlIdA qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L2IifFlexDigestCtrlIdA_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdA;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add ProtocolPktOpaqueCtrlId qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_ProtocolPktOpaqueCtrlId(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyProtocolPktOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for ProtocolPktOpaqueCtrlId qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_ProtocolPktOpaqueCtrlId_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyProtocolPktOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add VfiOpaqueCtrlId qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_VfiOpaqueCtrlId(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyVfiOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for VfiOpaqueCtrlId qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_VfiOpaqueCtrlId_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyVfiOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add SvpOpaqueCtrlId qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_SvpOpaqueCtrlId(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifySvpOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for SvpOpaqueCtrlId qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_SvpOpaqueCtrlId_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifySvpOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add L3IifOpaqueCtrlId qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L3IifOpaqueCtrlId(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL3IifOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for L3IifOpaqueCtrlId qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L3IifOpaqueCtrlId_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL3IifOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add L2IifOpaqueCtrlId qualifier into a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] data The matching data for a qualifier.
 * \param [in] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L2IifOpaqueCtrlId(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 data,
    uint8 mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL2IifOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    d = data;
    m = mask;

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_add(unit, entry, q, d, m));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the data/mask configuration for L2IifOpaqueCtrlId qualifier.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [out] data The matching data for a qualifier.
 * \param [out] mask The mask of matching data.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualify_L2IifOpaqueCtrlId_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    uint16_t d, m;
    bcm_flexdigest_qualify_t q = bcmFlexDigestQualifyL2IifOpaqueCtrlId;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_get(unit, entry, q, &d, &m));

    fd_unlock(unit);

    if (SHR_FUNC_ERR()) {
        SHR_EXIT();
    }

    if (data) {
        *data = d;
    }
    if (mask) {
        *mask = m;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Remove match criteria from a flex digest processor entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] qual_id The entry qualifier identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualifier_delete(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_qualify_t qual_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_delete(unit, entry, qual_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all qualifiers from a flex digest entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_qualifier_delete_all(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_qualifier_delete_all(unit, entry));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an action and parameters into a lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] action The flex digest policy table action.
 * \param [in] param0 The action parameter 0.
 * \param [in] param1 The action parameter 1.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_action_add(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_action_t action,
    uint16_t param0,
    uint16_t param1)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_action_add(unit, entry, action, param0, param1));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an action and parameters from a lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] action The flex digest policy table action.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_action_delete(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_action_t action)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_action_delete(unit, entry, action));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all actions from a lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_action_delete_all(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_action_delete_all(unit, entry));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an action parameters from a lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 * \param [in] action The flex digest policy table action.
 * \param [out] param0 The action parameter 0.
 * \param [out] param1 The action parameter 1.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_action_get(
    int unit,
    bcm_flexdigest_entry_t entry,
    bcm_flexdigest_action_t action,
    uint16_t *param0,
    uint16_t *param1)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_lkup_action_get(unit, entry, action, param0, param1));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

#if defined(BROADCOM_DEBUG)
/*!
 * \brief Show current software state of the flex digest module.
 *
 * \param [in] unit Unit Number.
 * \param [in] pfx The showing prefix.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_show(
    int unit,
    const char *pfx)
{
    SHR_FUNC_ENTER(unit);

    fd_lock(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_show(unit, pfx));

exit:
    fd_unlock(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Show contents of a flex digest group.
 *
 * \param [in] unit Unit Number.
 * \param [in] group The group identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_group_dump(
    int unit,
    bcm_flexdigest_group_t group)
{
    SHR_FUNC_ENTER(unit);

    fd_lock(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_group_dump(unit, group));

exit:
    fd_unlock(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Show contents of a flex digest lookup entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] entry The entry identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_entry_dump(
    int unit,
    bcm_flexdigest_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    fd_lock(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_entry_dump(unit, entry));

exit:
    fd_unlock(unit);
    SHR_FUNC_EXIT();
}
#endif

/*!
 * \brief Create a flex digest hashing profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] option Create option. See BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_xxx
 * \param [in/out] hash_profile_id Hashing profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_hash_profile_create(
    int unit,
    uint32 option,
    int *hash_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_hash_profile_create(unit, option, hash_profile_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a flex digest hashing profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] hash_profile_id Hashing profile identifier.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_hash_profile_destroy(
    int unit,
    int hash_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_hash_profile_destroy(unit, hash_profile_id));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy all flex digest hashing profiles.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_hash_profile_destroy_all(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_hash_profile_destroy_all(unit));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure a flex digest hashing profiles.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Hashing profile identifier.
 * \param [in] type The hashing profile control type.
 * \param [in] value The hashing profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_hash_profile_set(
    int unit,
    int hash_profile_id,
    bcm_flexdigest_hash_profile_control_t type,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_hash_profile_set(unit, hash_profile_id, type, value));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a flex digest hashing profiles.
 *
 * \param [in] unit Unit Number.
 * \param [in] profile_id Hashing profile identifier.
 * \param [in] type The hashing profile control type.
 * \param [out] value The hashing profile control value.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_hash_profile_get(
    int unit,
    int hash_profile_id,
    bcm_flexdigest_hash_profile_control_t type,
    int *value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_hash_profile_get(unit, hash_profile_id, type, value));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configure salt for a specific hashing bin set.
 *
 * \param [in] unit Unit Number.
 * \param [in] bin_set Hashing bin set.
 * \param [in] array_size The number of hashing bins in a bin set.
 * \param [in] salt_array The hashing salt array.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_hash_salt_set(
    int unit,
    bcm_flexdigest_hash_bin_set_t bin_set,
    int array_size,
    uint16_t *salt_array)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_hash_salt_set(unit, bin_set, array_size, salt_array));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get salt configuration for a specific hashing bin set.
 *
 * \param [in] unit Unit Number.
 * \param [in] bin_set Hashing bin set.
 * \param [in] array_size The maximum number of hashing bins array.
 * \param [out] salt_array The hashing salt array.
 * \param [out] count The number of hashing bins in a bin set.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_hash_salt_get(
    int unit,
    bcm_flexdigest_hash_bin_set_t bin_set,
    int array_size,
    uint16_t *salt_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (fd_hash_salt_get(unit, bin_set, array_size, salt_array, count));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a device-wide flex digest control value.
 *
 * \param [in] unit Unit number.
 * \param [in] type The flex digest control type.
 * \param [in] arg Argument whose meaning depends on type.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_control_set(
    int unit,
    bcm_flexdigest_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_flexdigest_control_set(unit, type, arg));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a device-wide flex digest control value.
 *
 * \param [in] unit Unit number.
 * \param [in] type The flex digest control type.
 * \param [out] arg Argument whose meaning depends on type.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_flexdigest_control_get(
    int unit,
    bcm_flexdigest_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_functionality(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (fd_validate_initialization(unit));

    fd_lock(unit);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_flexdigest_control_get(unit, type, arg));

    fd_unlock(unit);

exit:
    SHR_FUNC_EXIT();
}

