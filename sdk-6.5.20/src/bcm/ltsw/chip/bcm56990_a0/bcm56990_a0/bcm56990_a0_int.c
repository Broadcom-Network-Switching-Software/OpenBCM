/*! \file bcm56990_a0_int.c
 *
 * BCM56990_A0 tunnel subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_INT)

#include <bcm_int/ltsw/mbcm/int.h>
#include <bcm_int/ltsw/xgs/int.h>
#include <bcm_int/ltsw/int_int.h>
#include <bcm_int/ltsw/dev.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_INT

/******************************************************************************
 * Private functions
 */

static bcmint_int_md_field_info_t
    int_md_field_info[bcmIntMetadataCount] =
{
    [bcmIntMetadataPadOne] =                        {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)PAD_ONESs},
    [bcmIntMetadataPadZero] =                       {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)PAD_ZEROSs},
    [bcmIntMetadataOpaqueData1] =                   {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)OPAQUE_DATA_0s},
    [bcmIntMetadataOpaqueData2] =                   {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)OPAQUE_DATA_1s},
    [bcmIntMetadataOpaqueData3] =                   {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)OPAQUE_DATA_2s},
    [bcmIntMetadataSwitchId] =                      {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)SWITCH_IDs},
    [bcmIntMetadataResidenceTimeSubSeconds] =       {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)TRANSIT_DELAY_SUBSECONDSs},
    [bcmIntMetadataCosqStat0] =                     {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)TM_STAT_0s},
    [bcmIntMetadataCosqStat1] =                     {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)TM_STAT_1s},
    [bcmIntMetadataIngressTimestampSubSeconds] =    {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)ING_TIMESTAMP_SUB_SECONDSs},
    [bcmIntMetadataEgressTimestampSubSeconds] =     {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)EGR_TIMESTAMP_SUB_SECONDSs},
    [bcmIntMetadataIngressTimestampSecondsLower] =  {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)ING_TIMESTAMP_SECONDS_LOs},
    [bcmIntMetadataEgressTimestampSecondsLower] =   {1, bcmintIntMdFieldEntryWide, 0, {{0}, {0}}, (char *)EGR_TIMESTAMP_SECONDS_LOs},
    [bcmIntMetadataIngressTimestampSecondsUpper] =  {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)ING_TIMESTAMP_SECONDS_HIs},
    [bcmIntMetadataEgressTimestampSecondsUpper] =   {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)EGR_TIMESTAMP_SECONDS_HIs},
    [bcmIntMetadataResidenceTimeSeconds] =          {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)TRANSIT_DELAY_SECONDSs},
    [bcmIntMetadataL3EgrIntf] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)L3_EIFs},
    [bcmIntMetadataL3IngIntf] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)L3_IIFs},
    [bcmIntMetadataIngPortId] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)ING_PORTs},
    [bcmIntMetadataEgrPortId] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)EGR_PORTs},
    [bcmIntMetadataCongestion] =                    {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)CONGESTIONs},
    [bcmIntMetadataQueueId] =                       {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)QUEUE_IDs},
    [bcmIntMetadataProfileId] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)METADATA_PROFILE_IDs},
    [bcmIntMetadataCutThrough] =                    {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)CUT_THROUGHs},
    [bcmIntMetadataQueueIdProfileIdCutThrough] =    {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, NULL},
};

static int
bcm56990_a0_ltsw_int_md_field_info_get(
    int unit,
    bcmint_int_md_field_info_t **field_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(field_info, SHR_E_MEMORY);

    *field_info = int_md_field_info;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Int sub driver functions for bcm56990_a0.
 */
static mbcm_ltsw_int_drv_t bcm56990_a0_int_sub_drv = {
    .int_md_field_info_get = bcm56990_a0_ltsw_int_md_field_info_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_int_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_drv_set(unit, &bcm56990_a0_int_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
#else
int
bcm56990_a0_int_sub_drv_attach(int unit, void *hdl)
{
    return 0;
}
#endif /* INCLUDE_INT */
