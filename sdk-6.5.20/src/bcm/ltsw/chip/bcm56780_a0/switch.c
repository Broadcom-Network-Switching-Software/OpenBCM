/*! \file switch.c
 *
 * Switch control interfaces for device bcm56780.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcm_int/ltsw/xfs/switch.h>
#include <bcm_int/ltsw/mbcm/switch.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/chip/bcm56780_a0/variant_dispatch.h>

#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmlt/bcmlt.h>

#define BSL_LOG_MODULE BSL_LS_BCM_SWITCH

/******************************************************************************
 * Private functions
 */
static int
ep_recirc_profile_validate(
    int unit,
    bcm_switch_ep_recirc_profile_t *recirc_profile)
{
    uint32_t valid_flags, entry_num;
    int i, id, rv, flex_word_count;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_REDIRECT_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    valid_flags = BCM_SWITCH_EP_RECIRC_PROFILE_REPLACE |
                  BCM_SWITCH_EP_RECIRC_PROFILE_WITH_ID |
                  BCM_SWITCH_EP_RECIRC_DROP_SWITCHED_PACKET;

    if (recirc_profile->flags & (~valid_flags)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid flags(%"PRId32") for EP Recirculate"
                                " profile.\n"),
                                recirc_profile->flags));
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    if ((recirc_profile->flags & BCM_SWITCH_EP_RECIRC_PROFILE_REPLACE) &&
        !(recirc_profile->flags & BCM_SWITCH_EP_RECIRC_PROFILE_WITH_ID)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get number of EP Recirculate profiles. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              MON_REDIRECT_PROFILEs,
                              &entry_num));

    if (recirc_profile->flags & BCM_SWITCH_EP_RECIRC_PROFILE_WITH_ID) {
        id = recirc_profile->recirc_profile_id;
        /* Check profile id. */
        if (id < 0 || id >= entry_num) {
            SHR_ERR_EXIT(SHR_E_BADID);
        }

        lt_entry.fields = field;
        lt_entry.nfields = sizeof(field)/sizeof(field[0]);
        lt_entry.attr = 0;

        /* Get the entry with id. */
        field[0].u.val = recirc_profile->recirc_profile_id;
        rv = bcmi_lt_entry_get(unit, MON_REDIRECT_PROFILEs, &lt_entry,
                               NULL, NULL);
        /* The profile id already exists. */
        if (rv == SHR_E_NONE &&
            !(recirc_profile->flags & BCM_SWITCH_EP_RECIRC_PROFILE_REPLACE)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        } else if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    if (recirc_profile->buffer_priority < 0 ||
        recirc_profile->buffer_priority > 3) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Range of buffer_priority is 0-3.")));
        SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /* =========================================================================
     * CPU_DMA_HEADER_FLEX_WORD
     * =========================================================================
     */
    if (recirc_profile->tocpu_type != bcmSwitchEpRecircPktCopyTypeNoCopy) {

        flex_word_count = 0;
        if (recirc_profile->cpu_dma_cfg.cpu_dma_header_flex_word_bitmap &
            ~(0x3ffff)) {
            LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "MSB is 17 for cpu_dma_header_flex_word_bitmap.")));
        }

        for (i = 0; i < 18; i++) {
            if (recirc_profile->cpu_dma_cfg.cpu_dma_header_flex_word_bitmap &
                (1 << i)) {
                flex_word_count++;
            }
        }

        if (flex_word_count != 14) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Need to have exactly 14 bits selected "
                                    "from cpu_dma_header_flex_word_bitmap.")));
            SHR_ERR_EXIT(BCM_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ep_recirc_profile_id_allocate(
    int unit,
    bcm_switch_ep_recirc_profile_t *recirc_profile)
{
    int i, rv;
    uint32_t entry_num;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_REDIRECT_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    /*
     * Id is specified by the user and validation has been done
     * in ep_recirc_profile_validate
     */
    if (recirc_profile->flags & BCM_SWITCH_EP_RECIRC_PROFILE_WITH_ID) {
        SHR_EXIT();
    }

    /* Get number of EP Recirculate profiles. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              MON_REDIRECT_PROFILEs,
                              &entry_num));

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    /* Get first un-occupied index. */
    for (i = 0; i < entry_num; i++) {
        field[0].u.val = i;
        rv = bcmi_lt_entry_get(unit, MON_REDIRECT_PROFILEs, &lt_entry,
                               NULL, NULL);
        if (rv == SHR_E_NOT_FOUND) {
            recirc_profile->recirc_profile_id = i;
            SHR_EXIT();
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    if (i == entry_num) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a MON_REDIRECT_PROFILE LT entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] recirc_profile The EP Recirculate profile config structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ep_recirc_profile_table_set(
    int unit,
    bcm_switch_ep_recirc_profile_t *recirc_profile)
{
    int i;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_REDIRECT_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {DROPs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {NON_CPU_PORT_COPY_CONTROLs, BCMI_LT_FIELD_F_SET |
                                           BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*3*/ {CPU_COPY_CONTROLs, BCMI_LT_FIELD_F_SET |
                                  BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*4*/ {CELL_TRUNCATE_LENGTHs, BCMI_LT_FIELD_F_SET |
                                      BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*5*/ {RDB_ENQUEUE_PRIORITYs, BCMI_LT_FIELD_F_SET, 0, {0}},

        /*6*/ {NIH_HEADER_STARTs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*7*/ {DESTINATION_TYPEs, BCMI_LT_FIELD_F_SET |
                                  BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*8*/ {DESTINATIONs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*9*/ {NIH_HEADER_FIELDs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_SET |
                                  BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /*10*/ {NIH_HEADER_FIELDs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_SET |
                                   BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /*11*/ {NIH_HEADER_FIELDs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_SET |
                                   BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
        /*12*/ {NIH_HEADER_FIELDs, BCMI_LT_FIELD_F_ARRAY, 3, {0}},
        /*13*/ {NIH_HEADER_DROP_ENABLEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*14*/ {NIH_HEADER_TRACE_EVENTs, BCMI_LT_FIELD_F_SET |
                                         BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*15*/ {CPU_DMA_HEADER_WORDs, BCMI_LT_FIELD_F_ARRAY |
                                      BCMI_LT_FIELD_F_SET |
                                      BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /*16*/ {CPU_DMA_HEADER_WORDs, BCMI_LT_FIELD_F_ARRAY |
                                      BCMI_LT_FIELD_F_SET |
                                      BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /*17*/ {CPU_DMA_HEADER_WORDs, BCMI_LT_FIELD_F_ARRAY |
                                      BCMI_LT_FIELD_F_SET |
                                      BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
        /*18*/ {CPU_DMA_HEADER_WORDs, BCMI_LT_FIELD_F_ARRAY, 3, {0}},
        /*19*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        /*20*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 1, {0}},
        /*21*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 2, {0}},
        /*22*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 3, {0}},
        /*23*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 4, {0}},
        /*24*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 5, {0}},
        /*25*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 6, {0}},
        /*26*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 7, {0}},
        /*27*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 8, {0}},
        /*28*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 9, {0}},
        /*29*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 10, {0}},
        /*30*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 11, {0}},
        /*31*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 12, {0}},
        /*32*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 13, {0}},
        /*33*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 14, {0}},
        /*34*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 15, {0}},
        /*35*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 16, {0}},
        /*36*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 17, {0}},
        /*37*/ {CPU_DMA_HEADER_VERSIONs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    bcm_switch_ep_recirc_cpu_dma_config_t cpu_dma_cfg;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(recirc_profile, SHR_E_PARAM);

    cpu_dma_cfg = recirc_profile->cpu_dma_cfg;

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = recirc_profile->recirc_profile_id;

    if ((recirc_profile->flags) & BCM_SWITCH_EP_RECIRC_DROP_SWITCHED_PACKET) {
        field[1].u.val = 1;
    } else {
        field[1].u.val = 0;
    }

    switch (recirc_profile->redirect_type) {
        case bcmSwitchEpRecircPktCopyTypeNoCopy:
            field[2].u.sym_val = COPY_TO_PORT_DISABLEs;
            break;
        case bcmSwitchEpRecircPktCopyTypeDropped:
            field[2].u.sym_val = COPY_TO_PORT_ON_DROPs;
            break;
        case bcmSwitchEpRecircPktCopyTypeNotDropped:
            field[2].u.sym_val = COPY_TO_PORT_ON_NO_DROPs;
            break;
        case bcmSwitchEpRecircPktCopyTypeAll:
            field[2].u.sym_val = COPY_TO_PORTs;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }

    switch (recirc_profile->tocpu_type) {
        case bcmSwitchEpRecircPktCopyTypeNoCopy:
            field[3].u.sym_val = COPY_TO_CPU_DISABLEs;
            break;
        case bcmSwitchEpRecircPktCopyTypeDropped:
            field[3].u.sym_val = COPY_TO_CPU_ON_DROPs;
            break;
        case bcmSwitchEpRecircPktCopyTypeNotDropped:
            field[3].u.sym_val = COPY_TO_CPU_ON_NO_DROPs;
            break;
        case bcmSwitchEpRecircPktCopyTypeAll:
            field[3].u.sym_val = COPY_TO_CPUs;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }

    switch (recirc_profile->truncate_cell_length) {
        case 0:
            field[4].u.sym_val = DISABLEs;
            break;
        case 1:
            field[4].u.sym_val = ONE_CELLs;
            break;
        case 2:
            field[4].u.sym_val = TWO_CELLSs;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }

    field[5].u.val = recirc_profile->buffer_priority;

    /* NIH_HEADER_START is constant 0x2 in current DNA. */
    field[6].u.val = 2;

    /* =========================================================================
     * NIH_HEADER_FIELD
     * =======================================================================*/
    /* bit-0: 0: carries dest type & dest, else carry opqaue control & obj A */
    field[9].u.val = 0;
    switch (recirc_profile->nih_cfg.destination_type) {
        case bcmSwitchEpRecircNihDestTypeNone:
            field[7].u.sym_val = NO_OPs;
            break;
        case bcmSwitchEpRecircNihDestTypeEgressL2Interface:
            field[7].u.sym_val = L2_OIFs;
            break;
        case bcmSwitchEpRecircNihDestTypeFromOpaqueA:
            field[7].u.sym_val = NO_OPs;
            field[9].u.val = 1;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }

    field[8].u.val = recirc_profile->nih_cfg.destination;

    /* bit-1: 0: carries ingress timestamp, else carries egress timestamp. */
    switch (recirc_profile->nih_cfg.timestamp_select) {
        case bcmSwitchEpRecircTimestampSelectIngress:
            field[10].u.val = 0;
            break;
        case bcmSwitchEpRecircTimestampSelectEgress:
            field[10].u.val = 1;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /* bit-2; 0: carries residence time, else carries opaque obj B & C. */
    switch (recirc_profile->nih_cfg.res_time_field_cfg.res_time_field_select) {
        case bcmSwitchEpRecircResidenceTimeFieldSelectTime:
            field[11].u.val = 0;
            break;
        case bcmSwitchEpRecircResidenceTimeFieldSelectOpaqueObjBC:
            field[11].u.val = 1;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }
    /* bit-3; Reserved. */

    if (recirc_profile->nih_cfg.res_time_field_cfg.nih_drop_code_en == 0) {
        field[13].u.val = 0;
    } else {
        field[13].u.val = 1;
    }

    switch (recirc_profile->nih_cfg.res_time_field_cfg.nih_trace_event_select) {
        case bcmSwitchEpRecircNihTraceEventDisable:
            field[14].u.sym_val = EVENTS_DISABLEs;
            break;
        case bcmSwitchEpRecircNihTraceEvent_0_15:
            field[14].u.sym_val = EVENTS_0_15s;
            break;
        case bcmSwitchEpRecircNihTraceEvent_16_31:
            field[14].u.sym_val = EVENTS_16_31s;
            break;
        case bcmSwitchEpRecircNihTraceEvent_32_47:
            field[14].u.sym_val = EVENTS_32_47s;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }

    /* =========================================================================
     * CPU_DMA_HEADER_WORD
     * =======================================================================*/
    /* bit-0: 0: carries ingress timestamp, else carries egress timestamp. */
    switch (cpu_dma_cfg.timestamp_field_cfg.timestamp_select) {
        case bcmSwitchEpRecircTimestampSelectIngress:
            field[15].u.val = 0;
            break;
        case bcmSwitchEpRecircTimestampSelectEgress:
            field[15].u.val = 1;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }
    /* bit-1; 0: carries residence time, else carries opaque obj B & C. */
    switch (cpu_dma_cfg.res_time_field_select) {
        case bcmSwitchEpRecircResidenceTimeFieldSelectTime:
            field[16].u.val = 0;
            break;
        case bcmSwitchEpRecircResidenceTimeFieldSelectOpaqueObjBC:
            field[16].u.val = 1;
            break;
        default:
            SHR_ERR_EXIT(BCM_E_PARAM);
    }
    /* bit-2; if set, carries opaque obj A. */
    if (cpu_dma_cfg.timestamp_field_cfg.opaque_obj_a_en == 1) {
        field[17].u.val = 1;
    } else {
        field[17].u.val = 0;
    }
    /* bit-3; Reserved. */

    /* =========================================================================
     * CPU_DMA_HEADER_FLEX_WORD
     * =========================================================================
     */
    for (i = 0; i < 18; i++) {
        if (cpu_dma_cfg.cpu_dma_header_flex_word_bitmap & (1 << i)) {
            field[19+i].u.val = 1;
        } else {
            field[19+i].u.val = 0;
        }
        field[19+i].flags |= BCMI_LT_FIELD_F_SET | BCMI_LT_FIELD_F_ELE_VALID;
    }

    /* CPU_DMA_HEADER_VERSION is constant 0x1 in current DNA. */
    field[37].u.val = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_REDIRECT_PROFILEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a mirror instance id to a MON_REDIRECT_PROFILE LT entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] recirc_profile_id The EP Recirculate profile config structure.
 * \param [in] mirror_instance_id Mirror instance id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ep_recirc_profile_table_mirror_instance_set(
    int unit,
    int recirc_profile_id,
    int mirror_instance_id)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_REDIRECT_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {NIH_HEADER_OPAQUE_PROPERTYs, 0, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = recirc_profile_id;
    field[1].flags |= BCMI_LT_FIELD_F_GET;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_REDIRECT_PROFILEs, &lt_entry,
                           NULL, NULL));

    if (field[1].u.val != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "EP Recirculate profile (%d) already set "
                                "mirror instance id (%d).\n"),
                                recirc_profile_id, (int)field[1].u.val));
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    field[1].flags = 0;
    field[1].flags |= BCMI_LT_FIELD_F_SET;
    field[1].u.val = mirror_instance_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_REDIRECT_PROFILEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the mirror instance id from MON_REDIRECT_PROFILE LT entries.
 *
 * \param [in] unit Unit number.
 * \param [out] mirror_instance_id Mirror instance id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ep_recirc_profile_table_mirror_instance_get(
    int unit,
    int *mirror_instance_id)
{
    uint32_t entry_num;
    int i, rv, instance_id_tmp = -1;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_REDIRECT_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {NIH_HEADER_OPAQUE_PROPERTYs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    /* Get number of EP Recirculate profiles. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              MON_REDIRECT_PROFILEs,
                              &entry_num));

    /* Traverse profile entries */
    for (i = 0; i < entry_num; i++) {
        field[0].u.val = i;
        rv = bcmi_lt_entry_get(unit, MON_REDIRECT_PROFILEs, &lt_entry,
                               NULL, NULL);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        if (field[1].u.val != 0) {
            if (instance_id_tmp == -1) {
                instance_id_tmp = field[1].u.val;
            } else {
                if (instance_id_tmp != field[1].u.val) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit,
                                            "EP Recirculate profile mirror "
                                            "instance id conflicts, one is "
                                            "(%d), another is (%d).\n"),
                                            instance_id_tmp,
                                            (int)field[1].u.val));
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
            }
        }
    }

    *mirror_instance_id = instance_id_tmp;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a MON_REDIRECT_PROFILE LT entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] recirc_profile_id The EP Recirculate config profile id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ep_recirc_profile_table_delete(
    int unit,
    int recirc_profile_id)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_REDIRECT_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = recirc_profile_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_delete(unit, MON_REDIRECT_PROFILEs, &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all valid entry ids of MON_REDIRECT_PROFILE LT.
 *
 * If recirc_profile_id_array == NULL, count will be OUT for the number of
 * profiles; if the size cannot fit, it will only copy according to the size.
 *
 * \param [in] unit Unit number.
 * \param [out] recirc_profile_id_array EP Recirculate profile id array.
 * \param [in/out] count Count of the EP Recirculate profiles.
 *
 * \retval BCM_E_NONE on success and error code otherwise.
 */
int
ep_recirc_profile_table_get_all(
    int unit,
    int *recirc_profile_id_array,
    int *count)
{
    uint32_t entry_num;
    int i, rv = 0, index = 0;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_REDIRECT_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(count, SHR_E_PARAM);

    /* Get number of EP Recirculate profiles. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              MON_REDIRECT_PROFILEs,
                              &entry_num));

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    if (recirc_profile_id_array == NULL) {
        *count = 0;
        for (i = 0; i < entry_num; i++) {
            field[0].u.val = i;
            rv = bcmi_lt_entry_get(unit, MON_REDIRECT_PROFILEs, &lt_entry, NULL, NULL);
            if (rv == SHR_E_NOT_FOUND) {
                continue;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            *count = *count + 1;
        }
    } else {
        for (i = 0; i < entry_num; i++) {
            field[0].u.val = i;
            rv = bcmi_lt_entry_get(unit, MON_REDIRECT_PROFILEs, &lt_entry, NULL, NULL);
            if (rv == SHR_E_NOT_FOUND) {
                continue;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            if (index < *count) {
                recirc_profile_id_array[index] = i;
                index++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a MON_REDIRECT_PROFILE LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] recirc_profile_id The EP Recirculate profile id.
 * \param [out] recirc_profile The EP Recirculate profile.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ep_recirc_profile_table_get(
    int unit,
    int recirc_profile_id,
    bcm_switch_ep_recirc_profile_t *recirc_profile)
{
    int i;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_REDIRECT_PROFILE_IDs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {DROPs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*2*/ {NON_CPU_PORT_COPY_CONTROLs, BCMI_LT_FIELD_F_GET |
                                           BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*3*/ {CPU_COPY_CONTROLs, BCMI_LT_FIELD_F_GET |
                                  BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*4*/ {CELL_TRUNCATE_LENGTHs, BCMI_LT_FIELD_F_GET |
                                      BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*5*/ {RDB_ENQUEUE_PRIORITYs, BCMI_LT_FIELD_F_GET, 0, {0}},

        /*6*/ {NIH_HEADER_STARTs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*7*/ {DESTINATION_TYPEs, BCMI_LT_FIELD_F_GET |
                                  BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*8*/ {DESTINATIONs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*9*/ {NIH_HEADER_FIELDs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET |
                                  BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /*10*/ {NIH_HEADER_FIELDs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET |
                                   BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /*11*/ {NIH_HEADER_FIELDs, BCMI_LT_FIELD_F_ARRAY | BCMI_LT_FIELD_F_GET |
                                   BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
        /*12*/ {NIH_HEADER_FIELDs, BCMI_LT_FIELD_F_ARRAY, 3, {0}},
        /*13*/ {NIH_HEADER_DROP_ENABLEs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*14*/ {NIH_HEADER_TRACE_EVENTs, BCMI_LT_FIELD_F_GET |
                                         BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*15*/ {CPU_DMA_HEADER_WORDs, BCMI_LT_FIELD_F_ARRAY |
                                      BCMI_LT_FIELD_F_GET |
                                      BCMI_LT_FIELD_F_ELE_VALID, 0, {0}},
        /*16*/ {CPU_DMA_HEADER_WORDs, BCMI_LT_FIELD_F_ARRAY |
                                      BCMI_LT_FIELD_F_GET |
                                      BCMI_LT_FIELD_F_ELE_VALID, 1, {0}},
        /*17*/ {CPU_DMA_HEADER_WORDs, BCMI_LT_FIELD_F_ARRAY |
                                      BCMI_LT_FIELD_F_GET |
                                      BCMI_LT_FIELD_F_ELE_VALID, 2, {0}},
        /*18*/ {CPU_DMA_HEADER_WORDs, BCMI_LT_FIELD_F_ARRAY, 3, {0}},
        /*19*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 0, {0}},
        /*20*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 1, {0}},
        /*21*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 2, {0}},
        /*22*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 3, {0}},
        /*23*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 4, {0}},
        /*24*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 5, {0}},
        /*25*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 6, {0}},
        /*26*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 7, {0}},
        /*27*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 8, {0}},
        /*28*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 9, {0}},
        /*29*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 10, {0}},
        /*30*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 11, {0}},
        /*31*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 12, {0}},
        /*32*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 13, {0}},
        /*33*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 14, {0}},
        /*34*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 15, {0}},
        /*35*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 16, {0}},
        /*36*/ {CPU_DMA_HEADER_FLEX_WORDs, BCMI_LT_FIELD_F_ARRAY, 17, {0}},
        /*37*/ {CPU_DMA_HEADER_VERSIONs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < 18; i++) {
        field[19+i].flags |= BCMI_LT_FIELD_F_GET | BCMI_LT_FIELD_F_ELE_VALID;
    }

    SHR_NULL_CHECK(recirc_profile, SHR_E_PARAM);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = recirc_profile_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_REDIRECT_PROFILEs, &lt_entry, NULL, NULL));

    if (field[1].u.val == 1) {
        recirc_profile->flags |= BCM_SWITCH_EP_RECIRC_DROP_SWITCHED_PACKET;
    } else {
        recirc_profile->flags = 0;
    }

    if (sal_strcmp(field[2].u.sym_val, COPY_TO_PORT_DISABLEs) == 0) {
        recirc_profile->redirect_type = bcmSwitchEpRecircPktCopyTypeNoCopy;
    } else if (sal_strcmp(field[2].u.sym_val, COPY_TO_PORT_ON_DROPs) == 0) {
        recirc_profile->redirect_type = bcmSwitchEpRecircPktCopyTypeDropped;
    } else if (sal_strcmp(field[2].u.sym_val, COPY_TO_PORT_ON_NO_DROPs) == 0) {
        recirc_profile->redirect_type = bcmSwitchEpRecircPktCopyTypeNotDropped;
    } else if (sal_strcmp(field[2].u.sym_val, COPY_TO_PORTs) == 0) {
        recirc_profile->redirect_type = bcmSwitchEpRecircPktCopyTypeAll;
    }

    if (sal_strcmp(field[3].u.sym_val, COPY_TO_CPU_DISABLEs) == 0) {
        recirc_profile->tocpu_type = bcmSwitchEpRecircPktCopyTypeNoCopy;
    } else if (sal_strcmp(field[3].u.sym_val, COPY_TO_CPU_ON_DROPs) == 0) {
        recirc_profile->tocpu_type = bcmSwitchEpRecircPktCopyTypeDropped;
    } else if (sal_strcmp(field[3].u.sym_val, COPY_TO_CPU_ON_NO_DROPs) == 0) {
        recirc_profile->tocpu_type = bcmSwitchEpRecircPktCopyTypeNotDropped;
    } else if (sal_strcmp(field[3].u.sym_val, COPY_TO_CPUs) == 0) {
        recirc_profile->tocpu_type = bcmSwitchEpRecircPktCopyTypeAll;
    }

    if (sal_strcmp(field[4].u.sym_val, DISABLEs) == 0) {
        recirc_profile->truncate_cell_length = 0;
    } else if (sal_strcmp(field[4].u.sym_val, ONE_CELLs) == 0) {
        recirc_profile->truncate_cell_length = 1;
    } else if (sal_strcmp(field[4].u.sym_val, TWO_CELLSs) == 0) {
        recirc_profile->truncate_cell_length = 2;
    }

    recirc_profile->buffer_priority = field[5].u.val ;

    /* NIH_HEADER_START is constant 0x2 in current DNA. */
    /* field[6].u.val */

    /* =========================================================================
     * NIH_HEADER_FIELD
     * =======================================================================*/
    /* bit-0: 0: carries dest type & dest, else carry opqaue control & obj A */
    if (sal_strcmp(field[7].u.sym_val, NO_OPs) == 0) {
        recirc_profile->nih_cfg.destination_type = bcmSwitchEpRecircNihDestTypeNone;
    } else if (sal_strcmp(field[7].u.sym_val, L2_OIFs) == 0) {
        recirc_profile->nih_cfg.destination_type =
            bcmSwitchEpRecircNihDestTypeEgressL2Interface;
    } else {
       /* warn */
    }

    if (field[9].u.val == 1) {
        recirc_profile->nih_cfg.destination_type = bcmSwitchEpRecircNihDestTypeFromOpaqueA;
    }

    recirc_profile->nih_cfg.destination = field[8].u.val;

    /* bit-1: 0: carries ingress timestamp, else carries egress timestamp. */
    if (field[10].u.val == 0) {
        recirc_profile->nih_cfg.timestamp_select =
            bcmSwitchEpRecircTimestampSelectIngress;
    } else {
        recirc_profile->nih_cfg.timestamp_select =
            bcmSwitchEpRecircTimestampSelectEgress;
    }

    /* bit-2; 0: carries residence time, else carries opaque obj B & C. */
    if (field[11].u.val == 0) {
        recirc_profile->nih_cfg.res_time_field_cfg.res_time_field_select =
            bcmSwitchEpRecircResidenceTimeFieldSelectTime;
    } else {
        recirc_profile->nih_cfg.res_time_field_cfg.res_time_field_select =
            bcmSwitchEpRecircResidenceTimeFieldSelectOpaqueObjBC;
    }

    /* bit-3; Reserved. */

    recirc_profile->nih_cfg.res_time_field_cfg.nih_drop_code_en = field[13].u.val;

    if (sal_strcmp(field[14].u.sym_val, EVENTS_DISABLEs) == 0) {
        recirc_profile->nih_cfg.res_time_field_cfg.nih_trace_event_select =
            bcmSwitchEpRecircNihTraceEventDisable;
    } else if (sal_strcmp(field[14].u.sym_val, EVENTS_0_15s) == 0) {
        recirc_profile->nih_cfg.res_time_field_cfg.nih_trace_event_select =
            bcmSwitchEpRecircNihTraceEvent_0_15;
    } else if (sal_strcmp(field[14].u.sym_val, EVENTS_16_31s) == 0) {
        recirc_profile->nih_cfg.res_time_field_cfg.nih_trace_event_select =
            bcmSwitchEpRecircNihTraceEvent_16_31;
    } else if (sal_strcmp(field[14].u.sym_val, EVENTS_32_47s) == 0) {
        recirc_profile->nih_cfg.res_time_field_cfg.nih_trace_event_select =
            bcmSwitchEpRecircNihTraceEvent_32_47;
    }

    /* =========================================================================
     * CPU_DMA_HEADER_WORD
     * =======================================================================*/
    /* bit-0: 0: carries ingress timestamp, else carries egress timestamp. */
    if (field[15].u.val == 0) {
        recirc_profile->cpu_dma_cfg.timestamp_field_cfg.timestamp_select =
            bcmSwitchEpRecircTimestampSelectIngress;
    } else {
        recirc_profile->cpu_dma_cfg.timestamp_field_cfg.timestamp_select =
            bcmSwitchEpRecircTimestampSelectEgress;
    }

    /* bit-1; 0: carries residence time, else carries opaque obj B & C. */
    if (field[16].u.val == 0) {
        recirc_profile->cpu_dma_cfg.res_time_field_select =
            bcmSwitchEpRecircResidenceTimeFieldSelectTime;
    } else {
        recirc_profile->cpu_dma_cfg.res_time_field_select =
            bcmSwitchEpRecircResidenceTimeFieldSelectOpaqueObjBC;
    }

    /* bit-2; if set, carries opaque obj A. */
    recirc_profile->cpu_dma_cfg.timestamp_field_cfg.opaque_obj_a_en = field[17].u.val;

    /* bit-3; Reserved. */

    /* =========================================================================
     * CPU_DMA_HEADER_FLEX_WORD
     * =========================================================================
     */
    recirc_profile->cpu_dma_cfg.cpu_dma_header_flex_word_bitmap &= 0x00000000;
    for (i = 0; i < 18; i++) {
        if (field[19+i].u.val == 1) {
            recirc_profile->cpu_dma_cfg.cpu_dma_header_flex_word_bitmap |=
                (1 << i);
        }
    }

    /* CPU_DMA_HEADER_VERSION is constant 0x1 in current DNA. */
    /* field[37].u.val  */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a MON_REDIRECT_DROP_EVENT_CONTROL LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] drop_event_ctrl The EP recirculate drop event control structure.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ep_recirc_drop_event_control_table_set(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {REDIRECTs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {THRESHOLDs, BCMI_LT_FIELD_F_SET |
                           BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2*/ {REDIRECT_PROFILEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*3*/ {REDIRECT_PROFILE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    const char *symbol = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(drop_event_ctrl, SHR_E_PARAM);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = drop_event_ctrl->recirc_en;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_xfs_switch_ep_recirc_drop_event_to_symbol
             (unit, drop_event_ctrl->threshold, &symbol));

    field[1].u.sym_val = symbol;
    field[2].u.val = drop_event_ctrl->profile_id_offset;
    field[3].u.val = drop_event_ctrl->profile_id_mask;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_REDIRECT_DROP_EVENT_CONTROLs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a MON_REDIRECT_DROP_EVENT_CONTROL LT entry.
 *
 * \param [in] unit Unit number.
 * \param [out] drop_event_ctrl The EP recirculate drop event control structure.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ep_recirc_drop_event_control_table_get(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl)
{
    int drop_event;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {REDIRECTs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*1*/ {THRESHOLDs, BCMI_LT_FIELD_F_GET |
                           BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*2*/ {REDIRECT_PROFILEs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*3*/ {REDIRECT_PROFILE_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(drop_event_ctrl, SHR_E_PARAM);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_REDIRECT_DROP_EVENT_CONTROLs,
                           &lt_entry, NULL, NULL));

    drop_event_ctrl->recirc_en = field[0].u.val;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_xfs_switch_ep_recirc_symbol_to_drop_event(unit,
                                                          field[1].u.sym_val,
                                                          &drop_event));
    drop_event_ctrl->threshold = drop_event;
    drop_event_ctrl->profile_id_offset = field[2].u.val;
    drop_event_ctrl->profile_id_mask = field[3].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a MON_EGR_REDIRECT_TRACE_EVENT_CONTROL LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event_ctrl The EP recirculate trace event control structure.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ep_recirc_trace_event_control_table_set(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {REDIRECTs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*1*/ {REDIRECT_PROFILEs, BCMI_LT_FIELD_F_SET, 0, {0}},
        /*2*/ {REDIRECT_PROFILE_MASKs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(trace_event_ctrl, SHR_E_PARAM);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    field[0].u.val = trace_event_ctrl->recirc_en;
    field[1].u.val = trace_event_ctrl->profile_id_offset;
    field[2].u.val = trace_event_ctrl->profile_id_mask;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_EGR_REDIRECT_TRACE_EVENT_CONTROLs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a MON_EGR_REDIRECT_TRACE_EVENT_CONTROL LT entry.
 *
 * \param [in] unit Unit number.
 * \param [out] trace_event_ctrl The EP recirculate trace event control structure.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ep_recirc_trace_event_control_table_get(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {REDIRECTs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*1*/ {REDIRECT_PROFILEs, BCMI_LT_FIELD_F_GET, 0, {0}},
        /*2*/ {REDIRECT_PROFILE_MASKs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(trace_event_ctrl, SHR_E_PARAM);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_EGR_REDIRECT_TRACE_EVENT_CONTROLs,
                           &lt_entry, NULL, NULL));

    trace_event_ctrl->recirc_en = field[0].u.val;
    trace_event_ctrl->profile_id_offset = field[1].u.val;
    trace_event_ctrl->profile_id_mask = field[2].u.val;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a MON_EGR_REDIRECT_TRACE_EVENT LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event The trace event.
 * \param [in] enable The enabled status of the trace event.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ep_recirc_trace_event_table_set(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int enable)
{
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_EGR_REDIRECT_TRACE_EVENT_IDs, BCMI_LT_FIELD_F_SET |
                                                 BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*1*/ {REDIRECTs, BCMI_LT_FIELD_F_SET, 0, {0}},
    };
    const char *symbol = NULL;

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_xfs_switch_ep_recirc_trace_event_to_symbol(unit,
                                                           trace_event,
                                                           &symbol));

    field[0].u.sym_val = symbol;
    field[1].u.val = enable;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set(unit, MON_EGR_REDIRECT_TRACE_EVENTs,
                           &lt_entry, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all enabled MON_EGR_REDIRECT_TRACE_EVENT LT trace events.
 *
 * If trace_event_array == NULL, count will be OUT for the number of
 * events; if the size cannot fit, it will only copy according to the size.
 *
 * \param [in] unit Unit number.
 * \param [out] trace_event_array Trace event array.
 * \param [in/out] count Count of the enabled trace events.
 *
 * \retval BCM_E_NONE on success and error code otherwise.
 */
static int
ep_recirc_trace_event_table_get_all(
    int unit,
    bcm_pkt_trace_event_t *trace_event_array,
    int *count)
{
    int i, rv = SHR_E_NONE;
    int num_trace_event, index = 0;
    bcm_pkt_trace_event_t trace_event;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_EGR_REDIRECT_TRACE_EVENT_IDs, BCMI_LT_FIELD_F_SET |
                                                 BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*1*/ {REDIRECTs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    const char *symbol = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    /* Get number of supported trace events. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_xfs_switch_ep_recirc_trace_event_num_get(unit,
                                                         &num_trace_event));

    if (trace_event_array == NULL) {
        *count = 0;
        for (i = 0; i < num_trace_event; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_xfs_switch_ep_recirc_trace_event_symbol_iterate_get
                    (unit, i, &symbol));
            field[0].u.sym_val = symbol;
            rv = bcmi_lt_entry_get(unit, MON_EGR_REDIRECT_TRACE_EVENTs,
                                   &lt_entry, NULL, NULL);
            if (rv == SHR_E_NOT_FOUND) {
                continue;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            if (field[1].u.val == 1) {
                *count = *count + 1;
            }
        }
    } else {
        if (*count < 0 ) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        for (i = 0; i < num_trace_event; i++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_xfs_switch_ep_recirc_trace_event_symbol_iterate_get
                    (unit, i, &symbol));
            field[0].u.sym_val = symbol;
            rv = bcmi_lt_entry_get(unit, MON_EGR_REDIRECT_TRACE_EVENTs,
                                   &lt_entry, NULL, NULL);
            if (rv == SHR_E_NOT_FOUND) {
                continue;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
            if (field[1].u.val == 1) {
                if (index < *count) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_xfs_switch_ep_recirc_symbol_to_trace_event
                            (unit, symbol, &trace_event));
                    trace_event_array[index] = trace_event;
                    index++;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a MON_EGR_REDIRECT_TRACE_EVENT LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] trace_event The trace event.
 * \param [out] enable The enabled status of the trace event.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
ep_recirc_trace_event_table_get(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int *enable)
{
    int rv = SHR_E_NONE;
    bcmi_lt_entry_t lt_entry;
    bcmi_lt_field_t field[] =
    {
        /*0*/ {MON_EGR_REDIRECT_TRACE_EVENT_IDs, BCMI_LT_FIELD_F_SET |
                                                 BCMI_LT_FIELD_F_SYMBOL, 0, {0}},
        /*1*/ {REDIRECTs, BCMI_LT_FIELD_F_GET, 0, {0}},
    };
    const char *symbol = NULL;

    SHR_FUNC_ENTER(unit);

    lt_entry.fields = field;
    lt_entry.nfields = sizeof(field)/sizeof(field[0]);
    lt_entry.attr = 0;

    rv = bcmint_xfs_switch_ep_recirc_trace_event_to_symbol(unit,
                                                           trace_event,
                                                           &symbol);
    if (rv == SHR_E_NOT_FOUND) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Trace event(%d) not supported.\n"), trace_event));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    field[0].u.sym_val = symbol;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_get(unit, MON_EGR_REDIRECT_TRACE_EVENTs,
                           &lt_entry, NULL, NULL));

    *enable = field[1].u.val;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_scp_set(int unit, int port, bcm_switch_control_t type,
                    int arg)
{
    return bcmint_xfs_scp_set(unit, port, type, arg);
}

static int
bcm56780_a0_scp_get(int unit, int port, bcm_switch_control_t type,
                    int *arg)
{
    return bcmint_xfs_scp_get(unit, port, type, arg);
}

static int
bcm56780_a0_sc_set(int unit, bcm_switch_control_t type, int arg)
{
    return bcmint_xfs_sc_set(unit, type, arg);
}

static int
bcm56780_a0_sc_get(int unit, bcm_switch_control_t type, int *arg)
{
    return bcmint_xfs_sc_get(unit, type, arg);
}

static int
bcm56780_a0_spc_add(int unit, uint32_t options,
                    bcm_switch_pkt_protocol_match_t *match,
                    bcm_switch_pkt_control_action_t *action,
                    int priority)
{
    return bcmint_xfs_spc_add(unit, options, match, action, priority);
}

static int
bcm56780_a0_spc_get(int unit,
                    bcm_switch_pkt_protocol_match_t *match,
                    bcm_switch_pkt_control_action_t *action,
                    int *priority)
{
    return bcmint_xfs_spc_get(unit, match, action, priority);
}

static int
bcm56780_a0_spc_delete(int unit, bcm_switch_pkt_protocol_match_t *match)
{
    return bcmint_xfs_spc_delete(unit, match);
}

static int
bcm56780_a0_switch_pkt_integrity_check_add(int unit, uint32 options,
                                           bcm_switch_pkt_integrity_match_t *match,
                                           bcm_switch_pkt_control_action_t *action,
                                           int priority)
{
    return bcmint_xfs_switch_pkt_integrity_check_add(unit, options, match, action,
                                                     priority);
}

static int
bcm56780_a0_switch_pkt_integrity_check_get(int unit,
                                           bcm_switch_pkt_integrity_match_t *match,
                                           bcm_switch_pkt_control_action_t *action,
                                           int *priority)
{
    return bcmint_xfs_switch_pkt_integrity_check_get(unit, match, action, priority);
}

static int
bcm56780_a0_switch_pkt_integrity_check_delete(int unit,
                                              bcm_switch_pkt_integrity_match_t *match)

{
    return bcmint_xfs_switch_pkt_integrity_check_delete(unit, match);
}

static int
bcm56780_a0_switch_drop_event_mon_set(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    return bcmint_xfs_switch_drop_event_mon_set(unit, monitor);
}

static int
bcm56780_a0_switch_drop_event_mon_get(int unit, bcm_switch_drop_event_mon_t *monitor)
{
    return bcmint_xfs_switch_drop_event_mon_get(unit, monitor);
}

static int
bcm56780_a0_switch_trace_event_mon_set(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    return bcmint_xfs_switch_trace_event_mon_set(unit, monitor);
}

static int
bcm56780_a0_switch_trace_event_mon_get(int unit, bcm_switch_trace_event_mon_t *monitor)
{
    return bcmint_xfs_switch_trace_event_mon_get(unit, monitor);
}

static int
bcm56780_a0_switch_drop_event_to_symbol(int unit,
                                        bcm_pkt_drop_event_t drop_event,
                                        const char **table, const char **symbol)
{
    return bcmint_xfs_switch_drop_event_to_symbol(unit, drop_event, table, symbol);
}

static int
bcm56780_a0_switch_trace_event_to_symbol(int unit,
                                         bcm_pkt_trace_event_t trace_event,
                                         const char **table, const char **symbol)
{
    return bcmint_xfs_switch_trace_event_to_symbol(unit, trace_event, table, symbol);
}

static int
bcm56780_a0_switch_ep_recirc_profile_create(
    int unit,
    bcm_switch_ep_recirc_profile_t *recirc_profile,
    int *recirc_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(recirc_profile, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_profile_validate(unit, recirc_profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_profile_id_allocate(unit, recirc_profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_profile_table_set(unit, recirc_profile));

    *recirc_profile_id = recirc_profile->recirc_profile_id;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_profile_destroy(
    int unit,
    int recirc_profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_profile_table_delete(unit, recirc_profile_id));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_profile_get_all(
    int unit,
    int *recirc_profile_id_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_profile_table_get_all(unit, recirc_profile_id_array, count));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_profile_get(
    int unit,
    int recirc_profile_id,
    bcm_switch_ep_recirc_profile_t *recirc_profile)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_profile_table_get(unit, recirc_profile_id, recirc_profile));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_profile_mirror_instance_set(
    int unit,
    int recirc_profile_id,
    int mirror_instance_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_profile_table_mirror_instance_set(unit,
                                                     recirc_profile_id,
                                                     mirror_instance_id));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_profile_mirror_instance_get(
    int unit,
    int *mirror_instance_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_profile_table_mirror_instance_get(unit,
                                                     mirror_instance_id));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_drop_event_control_set(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_drop_event_control_table_set(unit, drop_event_ctrl));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_drop_event_control_get(
    int unit,
    bcm_switch_ep_recirc_drop_event_control_t *drop_event_ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_drop_event_control_table_get(unit, drop_event_ctrl));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_trace_event_control_set(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_trace_event_control_table_set(unit, trace_event_ctrl));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_trace_event_control_get(
    int unit,
    bcm_switch_ep_recirc_trace_event_control_t *trace_event_ctrl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_trace_event_control_table_get(unit, trace_event_ctrl));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_trace_event_enable_set(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_trace_event_table_set(unit, trace_event, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_trace_event_enable_get_all(
    int unit,
    bcm_pkt_trace_event_t *trace_event_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_trace_event_table_get_all(unit, trace_event_array, count));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_ep_recirc_trace_event_enable_get(
    int unit,
    bcm_pkt_trace_event_t trace_event,
    int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ep_recirc_trace_event_table_get(unit, trace_event, enable));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_xfs_switch_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_switch_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_xfs_switch_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

static mbcm_ltsw_sc_drv_t bcm56780_a0_sc_drv = {
    .scp_set = bcm56780_a0_scp_set,
    .scp_get = bcm56780_a0_scp_get,
    .sc_set = bcm56780_a0_sc_set,
    .sc_get = bcm56780_a0_sc_get,
    .spc_add = bcm56780_a0_spc_add,
    .spc_get = bcm56780_a0_spc_get,
    .spc_del = bcm56780_a0_spc_delete,
    .switch_pkt_integrity_check_add = bcm56780_a0_switch_pkt_integrity_check_add,
    .switch_pkt_integrity_check_get = bcm56780_a0_switch_pkt_integrity_check_get,
    .switch_pkt_integrity_check_delete = bcm56780_a0_switch_pkt_integrity_check_delete,
    .switch_drop_event_mon_set = bcm56780_a0_switch_drop_event_mon_set,
    .switch_drop_event_mon_get = bcm56780_a0_switch_drop_event_mon_get,
    .switch_trace_event_mon_set = bcm56780_a0_switch_trace_event_mon_set,
    .switch_trace_event_mon_get = bcm56780_a0_switch_trace_event_mon_get,
    .switch_drop_event_to_symbol = bcm56780_a0_switch_drop_event_to_symbol,
    .switch_trace_event_to_symbol = bcm56780_a0_switch_trace_event_to_symbol,
    .switch_ep_recirc_profile_create = bcm56780_a0_switch_ep_recirc_profile_create,
    .switch_ep_recirc_profile_destroy = bcm56780_a0_switch_ep_recirc_profile_destroy,
    .switch_ep_recirc_profile_get_all = bcm56780_a0_switch_ep_recirc_profile_get_all,
    .switch_ep_recirc_profile_get = bcm56780_a0_switch_ep_recirc_profile_get,
    .switch_ep_recirc_profile_mirror_instance_set = bcm56780_a0_switch_ep_recirc_profile_mirror_instance_set,
    .switch_ep_recirc_profile_mirror_instance_get = bcm56780_a0_switch_ep_recirc_profile_mirror_instance_get,
    .switch_ep_recirc_drop_event_control_set = bcm56780_a0_switch_ep_recirc_drop_event_control_set,
    .switch_ep_recirc_drop_event_control_get = bcm56780_a0_switch_ep_recirc_drop_event_control_get,
    .switch_ep_recirc_trace_event_control_set = bcm56780_a0_switch_ep_recirc_trace_event_control_set,
    .switch_ep_recirc_trace_event_control_get = bcm56780_a0_switch_ep_recirc_trace_event_control_get,
    .switch_ep_recirc_trace_event_enable_set = bcm56780_a0_switch_ep_recirc_trace_event_enable_set,
    .switch_ep_recirc_trace_event_enable_get_all = bcm56780_a0_switch_ep_recirc_trace_event_enable_get_all,
    .switch_ep_recirc_trace_event_enable_get = bcm56780_a0_switch_ep_recirc_trace_event_enable_get,
    .switch_init = bcm56780_a0_switch_init,
    .switch_detach = bcm56780_a0_switch_detach,
};

/******************************************************************************
 * Public functions
 */

int
bcm56780_a0_ltsw_sc_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_sc_drv_set(unit, &bcm56780_a0_sc_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_ltsw_variant_drv_attach(unit,
                                             BCM56780_A0_LTSW_VARIANT_SWITCH));
exit:
    SHR_FUNC_EXIT();
}

