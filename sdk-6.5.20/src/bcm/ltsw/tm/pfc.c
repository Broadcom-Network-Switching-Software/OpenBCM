/* \file pfc.c
 *
 * TM PFC Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/mbcm/pfc.h>
#include <bcm_int/ltsw/tm/pfc.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/cosq_int.h>
#include <bcm_int/ltsw/cosq_thd.h>
#include <bcm_int/ltsw/event_mgr.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/obm.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/switch.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

#define MAX_NUM_COS 12
#define PFC_COS_NUM 12
#define PFC_PRIORITY_NUM 8
#define PFC_MMU_QUEUE_NUM 12
#define PFC_RX_PROFILE_NUM_ENTRIES 8
#define PFC_COS_NUM_MASK_OFFSET 12 /* total 12 cos using bits[11:0]*/

#define PFC_PROPERTIES_PROFILE_ID_MIN 0
#define PFC_PROPERTIES_PROFILE_ID_MAX 7
#define PFC_CLASS_PROFILE_ID_MIN 0
#define PFC_CLASS_PROFILE_ID_MAX 7
#define PFC_PRIORITY_GROUP_ID_MAX 7
#define PFC_DEADLOCK_TIMER_MAX 15

typedef struct pfc_deadlock_event_info_s {
    /* Logical port ID. */
    int port;

    /* PFC priority. */
    int pfc_pri;

    /* Queue deadlock recovery state. */
    bcm_cosq_pfc_deadlock_recovery_event_t recovery_state;
} pfc_deadlock_event_info_t;

typedef struct pfc_deadlock_cb_s {
    /* Callback function */
    bcm_cosq_pfc_deadlock_recovery_event_cb_t pfc_deadlock_cb;

    /* user data */
    void * pfc_deadlock_userdata;
} pfc_deadlock_cb_t;

static pfc_deadlock_cb_t *pfc_deadlock_cb[BCM_MAX_NUM_UNITS];

#define PFC_DEADLOCK_CB(u) pfc_deadlock_cb[(u)]

static const int pfc_timer_tick_encoding[3] = {1, 10, 100};

static tm_ltsw_pfc_chip_driver_t *tm_pfc_chip_driver_cb[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

/*
 * \brief TM_ING_PORT_PRI_GRPs state check.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_state op_state.
 * \param [in] lport logic port.
 * \param [in] pg priority group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_tm_ing_port_pg_state_check(int unit, const char *op_state,
                                bcm_port_t lport, int pg)
{
    SHR_FUNC_ENTER(unit);

    if (sal_strcasecmp(op_state, VALIDs) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NONE);
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
            (BSL_META_U(unit,
                "port %d pg %d state error: %s\n"),
                lport, pg, op_state));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_PORT_PRI_GRPs get.
 *
 * \param [in]  unit        Unit Number.
 * \param [in]  port        Logical port ID.
 * \param [in]  pg          priority group.
 * \param [out] pfc         pfc state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_tm_ing_port_pg_get(int unit, int lport, int pg, int *pfc,
                        bool check_state)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
         /* 0 */ {PORT_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 1 */ {TM_PRI_GRP_IDs,         BCMI_LT_FIELD_F_SET, 0, {0}},
         /* 2 */ {PFCs,                   BCMI_LT_FIELD_F_GET, 0, {0}},
         /* 3 */ {OPERATIONAL_STATEs,     BCMI_LT_FIELD_F_GET |
                                          BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = pg;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_ING_PORT_PRI_GRPs,
                           &lt_entry, NULL, NULL));

    if (!check_state) {
        if (pfc != NULL) {
            *pfc = fields[2].u.val;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_ing_port_pg_state_check(unit, fields[3].u.sym_val, lport, pg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief TM_ING_PORT_PRI_GRPs setting.
 *
 * \param [in] unit        Unit Number.
 * \param [in] lport       Logical port ID.
 * \param [in] pg          priority group.
 * \param [in] pfc         pfc state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
cosq_tm_ing_port_pg_set(int unit, int lport, int pg, int pfc)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /* 0 */ {PORT_IDs,               BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 1 */ {TM_PRI_GRP_IDs,         BCMI_LT_FIELD_F_SET, 0, {0}},
        /* 2 */ {PFCs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = pg;
    fields[2].u.val = pfc ? true : false;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_ING_PORT_PRI_GRPs, &lt_entry, NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_ing_port_pg_get(unit, lport, pg, NULL, true));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set LT TM_PFC_EGRs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] lport         port id.
 * \param [in] pfc_enable    pfc enable.
 * \param [in] profile_id    profile id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_egr_set(int unit, int lport, int pfc_enable, int profile_id)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_IDs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {BLOCK_PFC_QUEUE_UPDATESs,     0, 0, {0}},
        /*2 */ {TM_PFC_PRI_PROFILE_IDs,       0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;

    if (pfc_enable != -1) {
        fields[1].flags = BCMI_LT_FIELD_F_SET;
        fields[1].u.val = pfc_enable ? false : true;
    }

    if (profile_id != -1) {
        fields[2].flags = BCMI_LT_FIELD_F_SET;
        fields[2].u.val = profile_id;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_PFC_EGRs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get LT TM_PFC_PRI_PROFILEs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] profile_id    profile id.
 * \param [in] prc_pri       prc pri.
 * \param [out] pfc_enable    pfc enable.
 * \param [out] optimized     optimized.
 * \param [out] cos_bmp       cos_bmp.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_pri_profile_get(int unit, int profile_id, int prc_pri,
                    int *pfc_enable, uint32_t *cos_bmp)
{
    int i;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 0, {0}},
        /*1*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 1, {0}},
        /*2*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 2, {0}},
        /*3*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 3, {0}},
        /*4*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 4, {0}},
        /*5*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 5, {0}},
        /*6*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 6, {0}},
        /*7*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 7, {0}},
        /*8*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 8, {0}},
        /*9*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 9, {0}},
        /*10*/ {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 10, {0}},
        /*11*/ {COS_LISTs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET, 11, {0}},
        /*12*/ {TM_PFC_PRI_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*13*/ {PFC_PRIs,               BCMI_LT_FIELD_F_SET, 0, {0}},
        /*14*/ {PFCs,                   BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[12].u.val = profile_id;
    fields[13].u.val = prc_pri;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_PFC_PRI_PROFILEs, &lt_entry, NULL, NULL));

    for (i = 0; i < MAX_NUM_COS; i++) {
        if (fields[i].u.val != 0) {
            *cos_bmp |= 1 << i;
        }
    }

    *pfc_enable = fields[14].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get an entire PFC class (RX) profile.
 *
 * \param [in] unit           Unit Number.
 * \param [in] profile_id     Profile ID.
 * \param [in] max_count      Maximum number of elements in config array.
 * \param [out] config_array  PFC class config array.
 * \param [out] count         actual size of array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_class_config_profile_get(
    int unit,
    int profile_id,
    int max_count,
    bcm_cosq_pfc_class_map_config_t *config_array,
    int *count)
{
    int pri, pfc_enable;
    uint32_t cos_bmp;

    SHR_FUNC_ENTER(unit);

    /* if max_count = 0, config_array has to be NULL */
    if(((max_count == 0) && (NULL != config_array)) ||
       ((max_count != 0) && (NULL == config_array))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if(profile_id < PFC_CLASS_PROFILE_ID_MIN ||
       profile_id > PFC_CLASS_PROFILE_ID_MAX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for(pri = 0; pri < max_count && pri < PFC_PRIORITY_NUM; pri++) {
        cos_bmp = 0;
        pfc_enable = 0;

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (pfc_pri_profile_get(unit, profile_id, pri,
                                 &pfc_enable, &cos_bmp),
                                 SHR_E_NOT_FOUND);

        config_array[pri].pfc_enable = pfc_enable;
        config_array[pri].cos_list_bmp = cos_bmp;
    }

    *count = pri;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set LT TM_PFC_PRI_PROFILEs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] profile_id    profile id.
 * \param [in] prc_pri       prc pri.
 * \param [in] pfc_enable    pfc enable.
 * \param [in] optimized     optimized.
 * \param [in] cos_bmp       cos_bmp.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_pri_profile_set(int unit, int profile_id, int prc_pri,
                    int pfc_enable, uint32_t cos_bmp)
{
    int i;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        /*1*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 1, {0}},
        /*2*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 2, {0}},
        /*3*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 3, {0}},
        /*4*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 4, {0}},
        /*5*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 5, {0}},
        /*6*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 6, {0}},
        /*7*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 7, {0}},
        /*8*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 8, {0}},
        /*9*/  {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 9, {0}},
        /*10*/ {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 10, {0}},
        /*11*/ {COS_LISTs, BCMI_LT_FIELD_F_ARRAY, 11, {0}},
        /*12*/ {TM_PFC_PRI_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*13*/ {PFC_PRIs,               BCMI_LT_FIELD_F_SET, 0, {0}},
        /*14*/ {PFCs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    for (i = 0; i < MAX_NUM_COS; i++) {
        fields[i].flags |= BCMI_LT_FIELD_F_SET |
                           BCMI_LT_FIELD_F_ELE_VALID;
        if (cos_bmp & (1 << i)) {
            fields[i].u.val = 1;
        }
    }

    fields[12].u.val = profile_id;
    fields[13].u.val = prc_pri;
    fields[14].u.val = pfc_enable ? TRUE : FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_PFC_PRI_PROFILEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set LT TM_PFC_PRI_TO_PRI_GRP_MAPs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] profile_id    profile id.
 * \param [in] prc_pri       prc pri.
 * \param [in] pg            pg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_pg_profile_set(int unit, int profile_id, int prc_pri, int pg)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {TM_PFC_PRI_TO_PRI_GRP_MAP_IDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {PFC_PRIs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {TM_PRI_GRP_IDs,                  BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = profile_id;
    fields[1].u.val = prc_pri;
    fields[2].u.val = pg;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_PFC_PRI_TO_PRI_GRP_MAPs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get LT TM_PFC_PRI_TO_PRI_GRP_MAPs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] profile_id    profile id.
 * \param [in] prc_pri       prc pri.
 * \param [out] pg           pg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_pg_profile_get(int unit, int profile_id, int prc_pri, int *pg)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {TM_PFC_PRI_TO_PRI_GRP_MAP_IDs,   BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {PFC_PRIs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {TM_PRI_GRP_IDs,                  BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = profile_id;
    fields[1].u.val = prc_pri;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_PFC_PRI_TO_PRI_GRP_MAPs,
                           &lt_entry, NULL, NULL));

    *pg = fields[2].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set LT TM_PFC_DEADLOCK_RECOVERYs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] lport         local port.
 * \param [in] prc_pri       prc pri.
 * \param [in] recover_enable recover enable.
 * \param [in] detect_timer   detect timer.
 * \param [in] timer_gran     timer gran.
 * \param [in] recover_timer  recover timer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_deadlock_recovery_set(int unit, int lport, int pfc_pri, int recover_enable,
                          int detect_timer, int timer_gran, int recover_timer)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_IDs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {PFC_PRIs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {DEADLOCK_RECOVERYs,           0, 0, {0}},
        /*3 */ {DETECTION_TIMERs,             0, 0, {0}},
        /*4 */ {DETECTION_TIMER_GRANULARITYs, 0, 0, {0}},
        /*5 */ {RECOVERY_TIMERs,              0, 0, {0}},
    };
    const char *sym_val = NULL;
    tm_ltsw_pfc_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_pfc_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_pfc_detection_timer_get, SHR_E_INIT);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = pfc_pri;

    if (recover_enable != -1) {
        fields[2].flags = BCMI_LT_FIELD_F_SET;
        fields[2].u.val = recover_enable ? true : false;
    }

    if (detect_timer != -1) {
        fields[3].flags = BCMI_LT_FIELD_F_SET;
        fields[3].u.val = detect_timer;
    }

    if (timer_gran != -1) {
        fields[4].flags = BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL;
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_pfc_detection_timer_get(unit, &timer_gran, &sym_val, true));

        fields[4].u.sym_val = sym_val;
    }

    if (recover_timer != -1) {
        fields[5].flags = BCMI_LT_FIELD_F_SET;
        fields[5].u.val = recover_timer;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_PFC_DEADLOCK_RECOVERYs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get LT TM_PFC_DEADLOCK_RECOVERYs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] lport         local port.
 * \param [in] prc_pri       prc pri.
 * \param [out] recover_enable recover enable.
 * \param [out] detect_timer   detect timer.
 * \param [out] timer_gran     timer gran.
 * \param [out] recover_timer  recover timer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_deadlock_recovery_get(int unit, int lport, int pfc_pri, int *recover_enable,
                          int *detect_timer, int *timer_gran, int *recover_timer)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_IDs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {PFC_PRIs,                     BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {DEADLOCK_RECOVERYs,           BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3 */ {DETECTION_TIMERs,             BCMI_LT_FIELD_F_GET, 0, {0}},
        /*4 */ {DETECTION_TIMER_GRANULARITYs, BCMI_LT_FIELD_F_GET |
                                              BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*5 */ {RECOVERY_TIMERs,              BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    tm_ltsw_pfc_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_pfc_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_pfc_detection_timer_get, SHR_E_INIT);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = lport;
    fields[1].u.val = pfc_pri;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_PFC_DEADLOCK_RECOVERYs,
                           &lt_entry, NULL, NULL));

    if (recover_enable != NULL) {
        *recover_enable = fields[2].u.val;
    }

    if (detect_timer != NULL) {
        *detect_timer = fields[3].u.val;
    }

    if (timer_gran != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_pfc_detection_timer_get(unit, timer_gran,
                                      &(fields[4].u.sym_val), false));
    }

    if (recover_timer != NULL) {
        *recover_timer = fields[5].u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set LT TM_PFC_DEADLOCK_RECOVERY_STATUSs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] lport         local port.
 * \param [in] prc_pri       prc pri.
 * \param [in] cnt           cnt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_deadlock_recovery_status_set(int unit, int port, int prc_pri, uint32_t cnt)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_IDs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {PFC_PRIs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {RECOVERY_CNTs,                   BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = port;
    fields[1].u.val = prc_pri;
    fields[2].u.val = cnt;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get LT TM_PFC_DEADLOCK_RECOVERY_STATUSs.
 *
 * \param [in] unit          Unit Number.
 * \param [in] lport         local port.
 * \param [in] prc_pri       prc pri.
 * \param [out] recovery     recovery.
 * \param [out] cnt          cnt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_deadlock_recovery_status_get(int unit, int port, int prc_pri,
                                 int *recovery, uint32_t *cnt)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {PORT_IDs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1 */ {PFC_PRIs,                        BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2 */ {RECOVERYs,                       BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3 */ {RECOVERY_CNTs,                   BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    fields[0].u.val = port;
    fields[1].u.val = prc_pri;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSs,
                           &lt_entry, NULL, NULL));

    if (recovery != NULL) {
        *recovery = fields[2].u.val;
    }

    if (cnt != NULL) {
        *cnt = fields[3].u.val;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get pfc deadlock timer gran.
 *
 * \param [in] unit          Unit Number.
 * \param [in] port          local port.
 * \param [in] prc_pri       prc pri.
 * \param [out] timer_gran   timer gran.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_deadlock_timer_gran_get(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    int *timer_gran)
{
    int gran = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(timer_gran, SHR_E_PARAM);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (pfc_deadlock_recovery_get(unit, port, pfc_priority,
                                   NULL, NULL, &gran, NULL),
                                   SHR_E_NOT_FOUND);
    *timer_gran = gran;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief To check the validity of COS lists for all PFC priorities.
 *
 * \param [in] unit          Unit Number.
 * \param [in] config_array  PFC class config array.
 * \param [in] array_len     The array length.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_opt_bmp_validate(int unit, bcm_cosq_pfc_class_map_config_t *config_array,
                     int array_len)
{
    int i, j;
    uint32_t opt_bmp_agg;

    SHR_FUNC_ENTER(unit);

    if (config_array == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (array_len > PFC_PRIORITY_NUM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*
     *   two conditions have to be met
     *   1. for any two optimized PFC priorities, the list of CoS must be either
     *      mutually exclusive or equal.
     *   2. A priority p0 can be PFC optimized if there is no other priority p1
     *      that COS list of p1 is a subset of COS list of p0.
 */
    for (i = 0; i< array_len; i++)   {
        if (!config_array[i].pfc_optimized ||
            !config_array[i].cos_list_bmp) {
            continue;
        }

        for (j = 0; j < array_len; j++) {
            if (j == i || !config_array[j].cos_list_bmp) {
                continue;
            }

            opt_bmp_agg = config_array[i].cos_list_bmp &
                          config_array[j].cos_list_bmp;

            if (config_array[j].pfc_optimized) {
                /*  contition 1 */
                if ((opt_bmp_agg != 0) &&
                    (config_array[i].cos_list_bmp !=
                     config_array[j].cos_list_bmp)) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);

                }
            } else {
                /* condition 2*/
                if (opt_bmp_agg == config_array[j].cos_list_bmp) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);

                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To check if the input Config array is valid.
 *
 * \param [in] unit          Unit Number.
 * \param [in] config_array  PFC class config array.
 * \param [in] array_len     The array length.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pfc_rx_config_validate(int unit,  bcm_cosq_pfc_class_map_config_t *config_array,
                       int array_len)
{
    int i;

    SHR_FUNC_ENTER(unit);

    if (config_array == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < array_len; i++) {
        if (config_array[i].pfc_optimized) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }

        if ((config_array[i].cos_list_bmp >> PFC_COS_NUM_MASK_OFFSET)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pfc_opt_bmp_validate(unit, config_array, PFC_PRIORITY_NUM));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PFC deadlock event callback function.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   event        Event string.
 * \param [in]   notif_info   pfc deadlock event info.
 * \param [in]   user_data    User data.
 */
static void
pfc_deadlock_event_cb(int unit, const char *event, void *notif_info,
                      void *user_data)
{
    pfc_deadlock_event_info_t *deadlock_event = NULL;
    pfc_deadlock_cb_t *deadlock_cb = NULL;

    deadlock_event = (pfc_deadlock_event_info_t *)notif_info;
    if (deadlock_event == NULL) {
        return;
    }

    deadlock_cb = PFC_DEADLOCK_CB(unit);

    if(deadlock_cb->pfc_deadlock_cb) {
        deadlock_cb->pfc_deadlock_cb(unit, deadlock_event->port,
                                     deadlock_event->pfc_pri,
                                     deadlock_event->recovery_state,
                                     deadlock_cb->pfc_deadlock_userdata);
    }
}

/*!
 * \brief The callback function to handle pfc deadlock notification.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   event        Event string.
 * \param [in]   notif_info   pfc deadlock event info.
 * \param [in]   user_data    User data.
 */
static void
pfc_deadlock_notification_cb(int unit, const char *event, void *notif_info,
                             void *user_data)
{
    pfc_deadlock_event_info_t *event_l = (pfc_deadlock_event_info_t *)notif_info;

    if (event_l->recovery_state == -1) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SW update Port %d PFC Priority %d recovery cnt\n"),
                  event_l->port, event_l->pfc_pri));
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "PFC deadlock notification event %s "
                             "Port %d PFC Priority %d state %d\n"),
                  event, event_l->port, event_l->pfc_pri, event_l->recovery_state));

        bcmi_ltsw_event_notify(unit, BCMI_LTSW_EV_PFC_DEADLOCK, notif_info,
                               sizeof(pfc_deadlock_event_info_t));
    }

    return;
}

/*!
 * \brief Parse pfc deadlock event table entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       Table name.
 * \param [in]  eh            LT entry handle.
 * \param [out] notif_info    Pointer to notification info.
 * \param [out] status        Parser callback returned status, NULL for not care.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static void
pfc_deadlock_event_parse(int unit, const char *lt_name,
                         uint32_t entry_hdl, void *notif_info,
                         bcmi_ltsw_event_status_t *status)
{
    bcmlt_entry_handle_t eh = entry_hdl;
    pfc_deadlock_event_info_t *event = (pfc_deadlock_event_info_t *)notif_info;
    union {
        /* scalar data. */
        uint64_t    val;
        /* symbol data. */
        const char  *sym_val;
    } fval;
    int recovery;
    int rv;

    rv = bcmlt_entry_field_get(eh, PORT_IDs, &(fval.val));
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, PORT_IDs, rv));
        return;
    }
    event->port = fval.val;

    rv = bcmlt_entry_field_get(eh, PFC_PRIs, &(fval.val));
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, PFC_PRIs, rv));
        return;
    }
    event->pfc_pri = fval.val;

    rv = bcmlt_entry_field_get(eh, RECOVERYs, &(fval.val));

    if (rv == SHR_E_NOT_FOUND) {
        event->recovery_state = -1;
        return;
    }

    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to parse %s.%s (%d).\n"),
                  lt_name, RECOVERYs, rv));
        return;
    }
    recovery = fval.val;

    if (recovery) {
        event->recovery_state = bcmCosqPfcDeadlockRecoveryEventBegin;
    } else {
        event->recovery_state = bcmCosqPfcDeadlockRecoveryEventEnd;
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Port %d PFC Priority %d Recovery %s\n"),
              event->port, event->pfc_pri, recovery ? "BEGIN" : "END"));

}

/*!
 * \brief Subscribe for updates of TM_PFC_DEADLOCK_RECOVERY_STATUS table.
 *
 * \param [in] unit Unit number.
 * \param [in] enable enable.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Table was not found.
 */
static int
pfc_deadlock_monitor_enable(int unit, bool enable)
{
    SHR_FUNC_ENTER(unit);

    if (enable) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_parser_set(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSs,
                                      pfc_deadlock_event_parse,
                                      sizeof(pfc_deadlock_event_info_t)));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_attrib_set(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSs,
                                      BCMI_LTSW_EVENT_HIGH_PRIORITY));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_subscribe(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSs,
                                     pfc_deadlock_notification_cb, NULL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_table_unsubscribe(unit, TM_PFC_DEADLOCK_RECOVERY_STATUSs,
                                       pfc_deadlock_notification_cb));
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

int
tm_ltsw_pfc_chip_driver_register(int unit, tm_ltsw_pfc_chip_driver_t *drv)
{
    tm_pfc_chip_driver_cb[unit] = drv;

    return SHR_E_NONE;
}

int
tm_ltsw_pfc_chip_driver_deregister(int unit)
{
    tm_pfc_chip_driver_cb[unit] = NULL;

    return SHR_E_NONE;
}

int
tm_ltsw_pfc_chip_driver_get(int unit, tm_ltsw_pfc_chip_driver_t **drv)
{
    *drv = tm_pfc_chip_driver_cb[unit];

    return SHR_E_NONE;
}

int
tm_ltsw_pfc_class_config_profile_get(
    int unit,
    int profile_id,
    int max_count,
    bcm_cosq_pfc_class_map_config_t *config_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pfc_class_config_profile_get(unit, profile_id, max_count,
                                      config_array, count));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_class_config_profile_set(
    int unit,
    int profile_id,
    int count,
    bcm_cosq_pfc_class_map_config_t *config_array)
{

    int pri;

    SHR_FUNC_ENTER(unit);

    if (profile_id < PFC_CLASS_PROFILE_ID_MIN ||
        profile_id > PFC_CLASS_PROFILE_ID_MAX ||
        config_array == NULL ||
        count != PFC_PRIORITY_NUM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pfc_rx_config_validate(unit, config_array, count));

    /* Start from priority 7 and check every priority */
    for (pri = 0; pri < PFC_PRIORITY_NUM; pri++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pfc_pri_profile_set(unit, profile_id, pri,
                                 config_array[pri].pfc_enable,
                                 config_array[pri].cos_list_bmp));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_priority_mapping_profile_get(
    int unit,
    int profile_id,
    int array_max,
    int *pg_array,
    int *array_count)
{
    int pri, pg;

    SHR_FUNC_ENTER(unit);

    if (profile_id < PFC_PROPERTIES_PROFILE_ID_MIN ||
        profile_id > PFC_PROPERTIES_PROFILE_ID_MAX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((0 == array_max && pg_array != NULL) ||
        (0 != array_max && pg_array == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (pri = 0; pri < array_max && pri < PFC_PRIORITY_NUM; pri++) {
        pg = 0;

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (pfc_pg_profile_get(unit, profile_id, pri, &pg), SHR_E_NOT_FOUND);

        pg_array[pri] = pg;
    }

    *array_count = pri;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_priority_mapping_profile_set(
    int unit,
    int profile_id,
    int array_count,
    int *pg_array)
{
    int pri;

    SHR_FUNC_ENTER(unit);

    if (profile_id < PFC_PROPERTIES_PROFILE_ID_MIN ||
        profile_id > PFC_PROPERTIES_PROFILE_ID_MAX ||
        array_count > PFC_PRIORITY_NUM  ||
        array_count <= 0 ||
        pg_array == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (pri = 0; pri < array_count; pri++) {
        if (pg_array[pri] < 0 || pg_array[pri] > PFC_PRIORITY_GROUP_ID_MAX) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (pfc_pg_profile_set(unit, profile_id, pri, pg_array[pri]));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_init(int unit)
{
    uint32_t ha_alloc_size, ha_req_size;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    ha_req_size = sizeof(pfc_deadlock_cb_t);
    ha_alloc_size = ha_req_size;

    pfc_deadlock_cb[unit] =
        (pfc_deadlock_cb_t *)bcmi_ltsw_ha_mem_alloc(
                                unit, BCMI_HA_COMP_ID_COSQ,
                                BCMINT_COSQ_PFC_DEADLOCK_SUB_COMP_ID,
                                "bcmCosqPfcDeadlock",
                                &ha_alloc_size);
    SHR_NULL_CHECK(pfc_deadlock_cb[unit], SHR_E_MEMORY);

    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    if (!warm) {
        pfc_deadlock_cb[unit]->pfc_deadlock_cb = NULL;
        pfc_deadlock_cb[unit]->pfc_deadlock_userdata = NULL;
    } else {
        if (pfc_deadlock_cb[unit]->pfc_deadlock_cb) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_event_register(unit,
                                          BCMI_LTSW_EV_PFC_DEADLOCK,
                                          &pfc_deadlock_event_cb,
                                          NULL));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_event_attrib_set(unit,
                                            BCMI_LTSW_EV_PFC_DEADLOCK,
                                            BCMI_LTSW_EVENT_APPL_CALLBACK));

            SHR_IF_ERR_VERBOSE_EXIT
                (pfc_deadlock_monitor_enable(unit, TRUE));
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (!warm && pfc_deadlock_cb[unit]) {
            (void)bcmi_ltsw_ha_mem_free(unit, pfc_deadlock_cb[unit]);
            pfc_deadlock_cb[unit] = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_detach(int unit)
{
    int warm = bcmi_warmboot_get(unit);
    pfc_deadlock_cb_t *deadlock_cb = NULL;

    SHR_FUNC_ENTER(unit);

    deadlock_cb = PFC_DEADLOCK_CB(unit);

    if (deadlock_cb->pfc_deadlock_cb) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_event_unregister(unit,
                                        BCMI_LTSW_EV_PFC_DEADLOCK,
                                        &pfc_deadlock_event_cb));

        SHR_IF_ERR_VERBOSE_EXIT
            (pfc_deadlock_monitor_enable(unit, FALSE));
    }

    if (!warm) {
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, TM_PFC_EGRs));

        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, TM_PFC_PRI_PROFILEs));

        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, TM_PFC_PRI_TO_PRI_GRP_MAPs));

        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, TM_PFC_DEADLOCK_RECOVERYs));

        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, TM_PFC_DEADLOCK_RECOVERY_CONTROLs));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_rx_config_set(
    int unit,
    int lport,
    int pfc_enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pfc_egr_set(unit, lport, pfc_enable, -1));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_tx_config_set(
    int unit,
    int lport,
    int pfc_enable)
{
    int pg;
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_device_info_get(unit, &device_info));

    for (pg = 0; pg < device_info.num_pg; pg++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_tm_ing_port_pg_set(unit, lport, pg, pfc_enable));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_cosq_port_priority_group_property_set(unit, lport, 0,
            bcmCosqPauseEnable, pfc_enable ? false : true));

    if (bcmi_ltsw_port_is_type(unit, lport,  BCMI_LTSW_PORT_TYPE_CD)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_obm_port_fc_enable_set(unit, lport, pfc_enable ? 1 : 0));
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_port_pg_pfc_set(
    int unit,
    bcm_port_t port,
    int pg,
    int pfc_enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_ing_port_pg_set(unit, port, pg, pfc_enable));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_port_pg_pfc_get(
    int unit,
    bcm_port_t port,
    int pg,
    int *pfc_enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (cosq_tm_ing_port_pg_get(unit, port, pg, pfc_enable, false));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_config_profile_id_set(
    int unit,
    int lport,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    if (profile_id < 0 || profile_id > PFC_CLASS_PROFILE_ID_MAX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pfc_egr_set(unit, lport, -1, profile_id));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_deadlock_recovery_action_set(
    int unit,
    int action)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0*/  {ACTIONs, BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };
    const char *sym_val = NULL;
    tm_ltsw_pfc_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_pfc_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_pfc_recovery_action_get, SHR_E_INIT);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields)/sizeof(fields[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
          (drv->cosq_pfc_recovery_action_get(unit, &action, &sym_val, true));

    fields[0].u.sym_val = sym_val;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, TM_PFC_DEADLOCK_RECOVERY_CONTROLs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_deadlock_recovery_action_get(
    int unit,
    int *action)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t fields[] =
    {
        /*0 */ {ACTIONs, BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
    };
    int rv = SHR_E_NONE;
    tm_ltsw_pfc_chip_driver_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (tm_ltsw_pfc_chip_driver_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_INIT);
    SHR_NULL_CHECK(drv->cosq_pfc_recovery_action_get, SHR_E_INIT);

    lt_entry.fields = fields;
    lt_entry.nfields = sizeof(fields) / sizeof(fields[0]);
    lt_entry.attr = 0;

    rv = bcmi_lt_entry_get(unit, TM_PFC_DEADLOCK_RECOVERY_CONTROLs,
                           &lt_entry, NULL, NULL);

    if (rv == SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (drv->cosq_pfc_recovery_action_get(unit, action,
                                      &(fields[0].u.sym_val), false));
    } else if (rv == SHR_E_NOT_FOUND) {
        *action = 0;
        rv = SHR_E_NONE;
    }

    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_deadlock_control_get(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int *arg)
{
    int lport;
    uint32_t cnt = 0;
    int recover_enable = 0;
    int detect_timer = 0;
    int timer_gran = 0;
    int recover_timer = 0;
    int timer_tick, gran;
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &lport));

    if ((pfc_priority < 0 || pfc_priority >= PFC_PRIORITY_NUM) ||
        (type < 0 || type >= bcmCosqPFCDeadlockControlTypeCount)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (type == bcmCosqPFCDeadlockRecoveryOccurrences) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pfc_deadlock_recovery_status_get(unit, lport, pfc_priority,
                                              NULL, &cnt));
        timer_tick = bcmCosqPFCDeadlockTimerInterval100MiliSecond;
        gran = pfc_timer_tick_encoding[timer_tick];
        *arg = cnt * gran;

        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (pfc_deadlock_recovery_get(unit, port, pfc_priority,
                                   &recover_enable, &detect_timer,
                                   &timer_gran, &recover_timer),
                                   SHR_E_NOT_FOUND);

    switch (type) {
        case bcmCosqPFCDeadlockDetectionAndRecoveryEnable:
            *arg = recover_enable;
            break;
        case bcmCosqPFCDeadlockDetectionTimer:
            SHR_IF_ERR_VERBOSE_EXIT
                (pfc_deadlock_timer_gran_get(unit, lport, pfc_priority,
                                             &timer_tick));

            if(timer_tick < bcmCosqPFCDeadlockTimerInterval1MiliSecond ||
               timer_tick >= bcmCosqPFCDeadlockTimerIntervalCount) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            gran = pfc_timer_tick_encoding[timer_tick];
            *arg = detect_timer * gran;
            break;
        case bcmCosqPFCDeadlockRecoveryTimer:
            timer_tick = bcmCosqPFCDeadlockTimerInterval100MiliSecond;
            gran = pfc_timer_tick_encoding[timer_tick];
            *arg = recover_timer * gran;
            break;
        case bcmCosqPFCDeadlockTimerGranularity:
            *arg = timer_gran;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    LOG_DEBUG(BSL_LOG_MODULE,
        (BSL_META_U(unit,
                    "port=0x%x pfc_priority=%d recover_enable=%d detect_timer=%d"
                    "recover_timer=%d timer_gran=%d\n"),
                    lport, pfc_priority, recover_enable,
                    detect_timer, recover_timer, timer_gran));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_deadlock_control_set(
    int unit,
    bcm_port_t port,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_control_t type,
    int arg)
{
    int lport;
    int timer_tick, timer_gran;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &lport));

    if ((pfc_priority < 0 || pfc_priority >= PFC_PRIORITY_NUM) ||
        (type < 0 || type >= bcmCosqPFCDeadlockControlTypeCount)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    LOG_DEBUG(BSL_LOG_MODULE,
        (BSL_META_U(unit,
                    "port=0x%x pfc_priority=%d type=%d arg=%d\n"),
                    lport, pfc_priority, type, arg));

    switch (type) {
        case bcmCosqPFCDeadlockDetectionAndRecoveryEnable:
            if ((arg != 0) && (arg != 1)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (pfc_deadlock_recovery_set(unit, lport, pfc_priority, arg,
                                           -1, -1, -1));
            break;
        case bcmCosqPFCDeadlockDetectionTimer:
            SHR_IF_ERR_VERBOSE_EXIT
                (pfc_deadlock_timer_gran_get(unit, lport, pfc_priority,
                                             &timer_tick));

            if(timer_tick < bcmCosqPFCDeadlockTimerInterval1MiliSecond ||
               timer_tick >= bcmCosqPFCDeadlockTimerIntervalCount) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            timer_gran = pfc_timer_tick_encoding[timer_tick];
            arg = arg / timer_gran;

            if ((arg < 0) || (arg > PFC_DEADLOCK_TIMER_MAX)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (pfc_deadlock_recovery_set(unit, lport, pfc_priority, -1,
                                           arg, -1, -1));
            break;
        case bcmCosqPFCDeadlockRecoveryTimer:
            timer_tick = bcmCosqPFCDeadlockTimerInterval100MiliSecond;
            timer_gran = pfc_timer_tick_encoding[timer_tick];
            arg = arg / timer_gran;
            if ((arg < 0) || (arg > PFC_DEADLOCK_TIMER_MAX)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (pfc_deadlock_recovery_set(unit, lport, pfc_priority, -1,
                                           -1, -1, arg));
            break;
        case bcmCosqPFCDeadlockTimerGranularity:
            if(arg < bcmCosqPFCDeadlockTimerInterval1MiliSecond ||
               arg >= bcmCosqPFCDeadlockTimerIntervalCount) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (pfc_deadlock_recovery_set(unit, lport, pfc_priority, -1,
                                           -1, arg, -1));
            break;
        case bcmCosqPFCDeadlockRecoveryOccurrences:
            if (arg == 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (pfc_deadlock_recovery_status_set(unit, lport, pfc_priority,
                                                      arg));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            break;
        /*
         * Coverity
         * This is defensive statement.
         */
        /* coverity[dead_error_begin] */
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_deadlock_info_get(
    int unit,
    int pfc_priority,
    bcm_cosq_pfc_deadlock_info_t *pfc_deadlock_info)
{
    int port;
    uint32_t port_type;
    bcm_pbmp_t pbmp;
    int recover_enable = 0;
    int recovery = 0;

    SHR_FUNC_ENTER(unit);

    if (pfc_priority < 0 || pfc_priority >= PFC_PRIORITY_NUM) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    port_type = BCMI_LTSW_PORT_TYPE_PORT | BCMI_LTSW_PORT_TYPE_MGMT;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, port_type, &pbmp));

    BCM_PBMP_ITER(pbmp, port) {
        recover_enable = 0;

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (pfc_deadlock_recovery_get(unit, port, pfc_priority, &recover_enable,
                                       NULL, NULL, NULL), SHR_E_NOT_FOUND);

        if(recover_enable) {
            BCM_PBMP_PORT_ADD(pfc_deadlock_info->enabled_pbmp, port);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (pfc_deadlock_recovery_status_get(unit, port, pfc_priority,
                                              &recovery, NULL));

        if(recovery) {
            BCM_PBMP_PORT_ADD(pfc_deadlock_info->deadlock_pbmp, port);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_deadlock_queue_status_get(
    int unit,
    bcm_gport_t gport,
    uint8_t *deadlock_status)
{
    int pri, local_port, recovery;
    int modid, trunk_id, id;
    uint8_t deadlock_bmp = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(deadlock_status, SHR_E_PARAM);

    if (BCM_GPORT_IS_SET(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_resolve(unit, gport, &modid, &local_port,
                                          &trunk_id, &id));
    } else {
        local_port = gport;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, local_port, &local_port));

    for (pri = 0; pri < PFC_PRIORITY_NUM; pri++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pfc_deadlock_recovery_status_get(unit, local_port, pri,
                                              &recovery, NULL));

        if (recovery) {
            deadlock_bmp |= (1 << pri);
        }
    }

    *deadlock_status = deadlock_bmp;

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_deadlock_recovery_event_register(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata)
{
    pfc_deadlock_cb_t *deadlock_cb = NULL;

    SHR_FUNC_ENTER(unit);

    deadlock_cb = PFC_DEADLOCK_CB(unit);

    deadlock_cb->pfc_deadlock_cb = callback;
    deadlock_cb->pfc_deadlock_userdata = userdata;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_event_register(unit,
                                  BCMI_LTSW_EV_PFC_DEADLOCK,
                                  &pfc_deadlock_event_cb,
                                  NULL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_event_attrib_set(unit,
                                    BCMI_LTSW_EV_PFC_DEADLOCK,
                                    BCMI_LTSW_EVENT_APPL_CALLBACK));

    SHR_IF_ERR_VERBOSE_EXIT
        (pfc_deadlock_monitor_enable(unit, TRUE));

exit:
    SHR_FUNC_EXIT();
}

int
tm_ltsw_pfc_deadlock_recovery_event_unregister(
    int unit,
    bcm_cosq_pfc_deadlock_recovery_event_cb_t callback,
    void *userdata)
{
    pfc_deadlock_cb_t *deadlock_cb = NULL;

    SHR_FUNC_ENTER(unit);

    deadlock_cb = PFC_DEADLOCK_CB(unit);

    deadlock_cb->pfc_deadlock_cb = NULL;
    deadlock_cb->pfc_deadlock_userdata = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_event_unregister(unit,
                                    BCMI_LTSW_EV_PFC_DEADLOCK,
                                    &pfc_deadlock_event_cb));

    SHR_IF_ERR_VERBOSE_EXIT
        (pfc_deadlock_monitor_enable(unit, FALSE));

exit:
    SHR_FUNC_EXIT();
}

