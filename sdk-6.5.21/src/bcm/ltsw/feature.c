/*! \file feature.c
 *
 * LTSW feature inferface.
 * This file contains the LTSW feature infterface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm_int/ltsw/mbcm/feature.h>
#include <bcm_int/ltsw/feature.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/*!
 * \brief feature information.
 */
typedef struct ltsw_feature_info_s {
    /*! Bitmap of features list to be supported. */
    SHR_BITDCLNAME(features, LTSW_FT_COUNT);
} ltsw_feature_info_t;

static ltsw_feature_info_t ltsw_feature_info[BCM_MAX_NUM_UNITS] = {{{ 0 }}};

#define FEAT_INFO(u) (&ltsw_feature_info[u])

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize the features to be supported.
 *
 * \param [in] unit    Unit number.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_feature_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Cleanup feature list. */
    sal_memset(FEAT_INFO(unit), 0, sizeof(ltsw_feature_info_t));

    /* Dispatch for feature init. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_feature_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initialize the supported features.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval None.
 */
void
bcmi_ltsw_feature_detach(int unit)
{
    sal_memset(FEAT_INFO(unit), 0, sizeof(ltsw_feature_info_t));
    return;
}

/*!
 * \brief Enable the specific feature.
 *
 * \param [in] unit Unit number.
 *
 * \retval None.
 */
void
ltsw_feature_enable(int unit, ltsw_feature_t feature)
{
    SHR_BITSET(FEAT_INFO(unit)->features, feature);
    return;
}

/*!
 * \brief Check if the feature is enabled.
 *
 * \param [in] unit Unit number.
 *
 * \retval TRUE  Feature is enabled.
 * \retval FALSE Feature is disabled.
 */
bool
ltsw_feature(int unit, ltsw_feature_t feature)
{
    return SHR_BITGET(FEAT_INFO(unit)->features, feature);
}
