/*! \file bcm56780_a0_ina_2_4_13_cosq.c
 *
 * BCM56780_A0 INA_2_4_13 COSQ driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/cosq.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

#define TM_UNICAST_VIEW_PROFILE_ID 0
#define TM_NON_UNICAST_VIEW_PROFILE_ID 1
#define TM_IFP_VIEW_PROFILE_ID 2
#define TM_DST_FP_VIEW_PROFILE_ID 3

/*
 * \brief String map for TM_Q_ASSIGNMENT_INFO_ID_T.
 */
static const
bcmint_cosq_str_map_t tm_q_assignment_info_id_type_map[] = {
    {
        .idx = BCMINT_TM_UNICAST_VIEW,
        .str = TM_UNICAST_VIEWs,
        .mc_cos_mirror = TRUE,
        .profile_id = TM_UNICAST_VIEW_PROFILE_ID
    },
    {
        .idx = BCMINT_TM_NON_UNICAST_VIEW,
        .str = TM_NON_UNICAST_VIEWs,
        .mc_cos_mirror = FALSE,
        .profile_id = TM_NON_UNICAST_VIEW_PROFILE_ID
    },
    {
        .idx = BCMINT_TM_IFP_UNICAST_VIEW,
        .str = TM_IFP_UNICAST_VIEWs,
        .mc_cos_mirror = TRUE,
        .profile_id = TM_IFP_VIEW_PROFILE_ID
    },
    {
        .idx = BCMINT_TM_IFP_NON_UNICAST_VIEW,
        .str = TM_IFP_NON_UNICAST_VIEWs,
        .mc_cos_mirror = FALSE,
        .profile_id = TM_IFP_VIEW_PROFILE_ID
    },
    {
        .idx = BCMINT_TM_DST_FP_UNICAST_VIEW,
        .str = TM_DST_FP_UNICAST_VIEWs,
        .mc_cos_mirror = TRUE,
        .profile_id = TM_DST_FP_VIEW_PROFILE_ID
    },
    {
        .idx = BCMINT_TM_DST_FP_NON_UNICAST_VIEW,
        .str = TM_DST_FP_NON_UNICAST_VIEWs,
        .mc_cos_mirror = FALSE,
        .profile_id = TM_DST_FP_VIEW_PROFILE_ID
    },
    {
        .idx = BCMINT_TM_IFP_UNICAST_HI_VIEW,
        .str = TM_IFP_UNICAST_HI_VIEWs,
        .mc_cos_mirror = TRUE,
        .profile_id = TM_IFP_VIEW_PROFILE_ID,
        .base_index = 256
    },
    {
        .idx = BCMINT_TM_IFP_NON_UNICAST_HI_VIEW,
        .str = TM_IFP_NON_UNICAST_HI_VIEWs,
        .mc_cos_mirror = FALSE,
        .profile_id = TM_IFP_VIEW_PROFILE_ID,
        .base_index = 256
    },
};

/*!
 * \brief cosq string map database.
 */
static const
bcmint_cosq_str_map_db_t str_map_db[] = {
    {
        .hdl = BCMINT_TM_Q_ASSIGNMENT_INFO_ID_T,
        .info = tm_q_assignment_info_id_type_map,
        .num = 8
    },
};

/*!
 * \brief cosq map database.
 */
static const
bcmint_cosq_map_db_t cosq_map_db[] = {
    {
        .str_db = str_map_db,
        .str_num = 1
    }
};

/******************************************************************************
 * Private functions
 */

/*
 * \brief read TM_Q_ASSIGNMENT_INFO.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
bcm56780_a0_ina_2_4_13_ltsw_tm_queue_assignment_info_get(
    int unit,
    const char *info_id,
    bcmint_cosq_assignment_info_t *info)
{
    int i;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/ {TM_Q_ASSIGNMENT_INFO_IDs,    BCMI_LT_FIELD_F_SET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*1*/ {UNICASTs,                    BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2*/ {SWITCHTOCPUs,                BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*3*/ {NUM_TM_Q_ASSIGNMENT_PROFILE_IDs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*4*/ {TM_Q_ASSIGNMENT_PROFILE_IDs, BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL |
                                            BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        /*5*/ {TM_Q_ASSIGNMENT_PROFILE_IDs, BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL |
                                            BCMI_LT_FIELD_F_ARRAY, 1, {0}},
        /*6*/ {TM_Q_ASSIGNMENT_PROFILE_IDs, BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL |
                                            BCMI_LT_FIELD_F_ARRAY, 2, {0}},
        /*7*/ {TM_Q_ASSIGNMENT_PROFILE_IDs, BCMI_LT_FIELD_F_GET |
                                            BCMI_LT_FIELD_F_SYMBOL |
                                            BCMI_LT_FIELD_F_ARRAY, 3, {0}},
    };
    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.sym_val = info_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_Q_ASSIGNMENT_INFOs,
                           &lt_entry, NULL, NULL));

    info->unicast = fields[1].u.sym_val;
    info->switchtocpu = fields[2].u.sym_val;
    info->num_tm_q_assignment_profile_id = fields[3].u.val;
    for (i =0; i < TM_Q_ASSIGNMENT_INFO_MAX_POLICY; i++) {
        info->tm_q_assignment_profile_id[i] = fields[i+4].u.sym_val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize cosq map database.
 *
 * \param [in] unit Unit Number.
 * \param [in|out] map_db cosqmap database pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_ina_2_4_13_ltsw_cosq_map_db_init(int unit,
                                            const bcmint_cosq_map_db_t **map_db)
{
    *map_db = cosq_map_db;

    return SHR_E_NONE;
}

/*!
 * \brief cosq driver function variable for bcm56780_a0 DNA_2_4_2 device.
 */
static mbcm_ltsw_cosq_drv_t bcm56780_a0_ina_2_4_13_ltsw_cosq_drv = {
    .cosq_map_db_init = bcm56780_a0_ina_2_4_13_ltsw_cosq_map_db_init,
    .cosq_queue_assignment_info_get = bcm56780_a0_ina_2_4_13_ltsw_tm_queue_assignment_info_get
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ina_2_4_13_ltsw_cosq_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_drv_set(unit, &bcm56780_a0_ina_2_4_13_ltsw_cosq_drv));

exit:
    SHR_FUNC_EXIT();
}

