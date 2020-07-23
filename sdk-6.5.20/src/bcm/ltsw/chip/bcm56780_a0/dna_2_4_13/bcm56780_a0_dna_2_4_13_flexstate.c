/*! \file bcm56780_a0_dna_2_4_13_flexstate.c
 *
 * BCM56780_A0 DNA_2_4_13 Flex State driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/flexstate.h>

#include <bcm_int/ltsw/flexstate_int.h>
#include <bcm_int/ltsw/mbcm/flexstate.h>
#include <bcm_int/ltsw/flexstate.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FLEXSTATE

/*!
 * \brief Flex state source database.
 */
static bcmint_flexstate_source_info_t bcm56780_a0_dna_2_4_13_ltsw_flexstate_source_db[] = {
    {
        bcmFlexstateSourceEmFt,
        BCMI_LTSW_FLEXSTATE_STAGE_ING_LKUP,
        DT_EM_FT_ENTRYs,
        BCMINT_FLEXSTATE_ING_LKUP_EMFT_TBL_BIT,
    },
    {
        bcmFlexstateSourceIngPort,
        BCMI_LTSW_FLEXSTATE_STAGE_ING_LKUP,
        ING_L2_IIF_ATTRIBUTES_TABLEs,
        BCMINT_FLEXSTATE_ING_LKUP_ING_L2_IIF_ATTRIBUTES_TBL_BIT,
    },
    {
        bcmiFlexstateSourceEcnState,
        BCMI_LTSW_FLEXSTATE_STAGE_EGR,
        PORTs,
        BCMINT_FLEXSTATE_EGR_ECN_STATE_TBL_BIT,
    },
    /* Always last one. */
    {
        bcmFlexstateSourceCount,
    },
};

/*!
 * \brief Flex state object database.
 */
static bcmint_flexstate_object_info_t bcm56780_a0_dna_2_4_13_ltsw_flexstate_object_db[] = {
    {
        bcmFlexstateObjectConstZero,
        BCMI_LTSW_FLEXSTATE_STAGE_COUNT,
        FlexstateObjectTypeConst,
        USE_0s,
    },
    {
        bcmFlexstateObjectConstOne,
        BCMI_LTSW_FLEXSTATE_STAGE_COUNT,
        FlexstateObjectTypeConst,
        USE_1s,
    },
    {
        bcmFlexstateObjectTriggerInterval,
        BCMI_LTSW_FLEXSTATE_STAGE_COUNT,
        FlexstateObjectTypeTrigger,
        USE_TRIG_INTERVALs,
    },
    {
        bcmFlexstateObjectQuant,
        BCMI_LTSW_FLEXSTATE_STAGE_COUNT,
        FlexstateObjectTypeQuant,
        USE_RANGE_CHK_1s,
    },
    /* bcm56780_a0_dna_2_4_13_FLEX_STATE_POST_LKUP_OBJ_BUS_T */
    {
        bcmFlexstateObjectIngL2Iif,
        BCMI_LTSW_FLEXSTATE_STAGE_ING_LKUP,
        FlexstateObjectTypeMenu,
        L2_IIFs,
    },
    {
        bcmFlexstateObjectIngEmFtHitIndex0,
        BCMI_LTSW_FLEXSTATE_STAGE_ING_LKUP,
        FlexstateObjectTypeMenu,
        EM_FT_HIT_INDEX_0s,
    },
    {
        bcmFlexstateObjectIngEmFtHitIndex1,
        BCMI_LTSW_FLEXSTATE_STAGE_ING_LKUP,
        FlexstateObjectTypeMenu,
        EM_FT_HIT_INDEX_1s,
    },
    /* bcm56780_a0_dna_2_4_13_FLEX_STATE_EGR_OBJ_BUS_T */
    {
        bcmFlexstateObjectEgrResidenceTimeRaw0,
        BCMI_LTSW_FLEXSTATE_STAGE_EGR,
        FlexstateObjectTypeMenu,
        RESIDENCE_TIME_RAW_0s,
    },
    {
        bcmFlexstateObjectEgrResidenceTimeRaw,
        BCMI_LTSW_FLEXSTATE_STAGE_EGR,
        FlexstateObjectTypeMenu,
        RESIDENCE_TIME_RAW_1s,
    },
    {
        bcmFlexstateObjectEgrPipePortQueueNum,
        BCMI_LTSW_FLEXSTATE_STAGE_EGR,
        FlexstateObjectTypeMenu,
        PIPE_PORT_QUEUE_NUMs,
    },
    {
        bcmFlexstateObjectEgrPktLenObj,
        BCMI_LTSW_FLEXSTATE_STAGE_EGR,
        FlexstateObjectTypeMenu,
        PKT_LEN_OBJs,
    },
    /* Always last one. */
    {
        bcmFlexstateObjectCount,
    },
};

/******************************************************************************
 * Private functions
 */

static int
bcm56780_a0_dna_2_4_13_ltsw_flexstate_source_info_get(
    int unit,
    int source,
    bcmint_flexstate_source_info_t **source_info)
{
    bcmint_flexstate_source_info_t *source_db, *si;
    size_t i, count;

    count = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_flexstate_source_db);
    source_db = bcm56780_a0_dna_2_4_13_ltsw_flexstate_source_db;

    if ((source < 0 || source >= bcmFlexstateSourceCount) &&
        (source < bcmiFlexstateSourceStart ||
         source >= bcmiFlexstateSourceEnd)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid flex state source %d\n"),
                   source));

        *source_info = NULL;
        return SHR_E_PARAM;
    }

    for (i = 0; i < count; i++) {
        si = &(source_db[i]);
        if (source == si->source) {
            *source_info = si;
            break;
        }
    }

    /* Not found. */
    if (i >= count) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Flex state source %d not found\n"),
                  source));

        *source_info = NULL;
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

static int
bcm56780_a0_dna_2_4_13_ltsw_flexstate_object_info_get(
    int unit,
    int object,
    bcmint_flexstate_object_info_t **object_info)
{
    bcmint_flexstate_object_info_t *object_db, *oi;
    size_t i, count;

    count = COUNTOF(bcm56780_a0_dna_2_4_13_ltsw_flexstate_object_db);
    object_db = bcm56780_a0_dna_2_4_13_ltsw_flexstate_object_db;

    if ((object < 0) || (object >= bcmFlexstateObjectCount)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid flex state object %d\n"),
                   object));

        *object_info = NULL;
        return SHR_E_PARAM;
    }

    for (i = 0; i < count; i++) {
        oi = &(object_db[i]);
        if (object == oi->object) {
            *object_info = oi;
            break;
        }
    }

    /* Not found. */
    if (i >= count) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Flex state object %d not found\n"),
                  object));

        *object_info = NULL;
        return SHR_E_UNAVAIL;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Flexstate driver function variable for bcm56780_a0_dna_2_4_13 device.
 */
static mbcm_ltsw_flexstate_drv_t bcm56780_a0_dna_2_4_13_ltsw_flexstate_drv = {
    .flexstate_source_info_get = bcm56780_a0_dna_2_4_13_ltsw_flexstate_source_info_get,
    .flexstate_object_info_get = bcm56780_a0_dna_2_4_13_ltsw_flexstate_object_info_get
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_dna_2_4_13_ltsw_flexstate_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexstate_drv_set
             (unit, &bcm56780_a0_dna_2_4_13_ltsw_flexstate_drv));

exit:
    SHR_FUNC_EXIT();
}

