/*! \file bcm5699x_int.c
 *
 * BCM5699x INT subordinate driver.
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
    [bcmIntMetadataOverlayL3EgrIntf] =              {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)L3_EIF_OVERLAYs},
    [bcmIntMetadataL3EgrIntf] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)L3_EIFs},
    [bcmIntMetadataL3IngIntf] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)L3_IIFs},
    [bcmIntMetadataIngPortId] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)ING_PORTs},
    [bcmIntMetadataEgrPortId] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)EGR_PORTs},
    [bcmIntMetadataVrf] =                           {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)VRFs},
    [bcmIntMetadataVfi] =                           {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)VFIs},
    [bcmIntMetadataDvp] =                           {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)DVPs},
    [bcmIntMetadataSvp] =                           {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)SVPs},
    [bcmIntMetadataTunnelEncapIndex] =              {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)TNL_ENCAP_INDEXs},
    [bcmIntMetadataCng] =                           {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)CNGs},
    [bcmIntMetadataCongestion] =                    {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)CONGESTIONs},
    [bcmIntMetadataQueueId] =                       {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)QUEUE_IDs},
    [bcmIntMetadataProfileId] =                     {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)METADATA_PROFILE_IDs},
    [bcmIntMetadataCutThrough] =                    {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, (char *)CUT_THROUGHs},
    [bcmIntMetadataCngQueueIdProfileIdCutThrough] = {1, bcmintIntMdFieldEntryNarrow, 0, {{0}, {0}}, NULL},
};

static int
bcm5699x_ltsw_int_md_field_info_get(
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
 * \brief Int sub driver functions for bcm5699x.
 */
static mbcm_ltsw_int_drv_t bcm5699x_int_sub_drv = {
    .int_md_field_info_get = bcm5699x_ltsw_int_md_field_info_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_int_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_int_drv_set(unit, &bcm5699x_int_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
#else
int
bcm5699x_int_sub_drv_attach(int unit, void *hdl)
{
    return 0;
}
#endif /* INCLUDE_INT */
