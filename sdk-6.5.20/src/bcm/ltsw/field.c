/*! \file field.c
 *
 * Field Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <bcm/types.h>
#include <bcm/field.h>
#include <bcm/flexctr.h>
#include <bcmltd/chip/bcmltd_str.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/field_destination.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/policer.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>
/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP
#define BCMINT_FP_ENTRY_MAX_AACL_CLASS_ID_CNT 2

/*!
 * \brief One Field control entry for each SOC device containing Field
 * information for that device.
 */
bcmi_ltsw_field_db_t bcmi_ltsw_field_db[BCM_MAX_NUM_UNITS];

/*
 * \brief bcmint_field_lt_entry_commit
 *
 * Function to perform commit operation for a given LT table.
 *
 * Params:
 *    template     - (IN/OUT) Reference to entry handle.
 *    operation    - (IN)     Commit operation.
 *    priority     - (IN)     Commit priority.
 *    table_name   - (IN)     LT table name.
 *    key_name     - (IN)     LT key name.
 *    key_val      - (IN)     LT key val.
 *    field_name   - (IN)     LT field name.
 *    field_val    - (IN)     field value.
 *    free         - (IN)     If set to 1, free the template.
 *
 * Returns:
 *     BCM_E_NONE      - Success
 *    !BCM_E_NONE      - Failure
 */
int
bcmint_field_lt_entry_commit(int unit,
                             bcmlt_entry_handle_t *template,
                             bcmlt_opcode_t operation,
                             bcmlt_priority_level_t priority,
                             char *table_name,
                             char *key_name,
                             bool key_symbol,
                             uint64_t key_val,
                             char *key_str,
                             char *field_name,
                             uint64_t field_val,
                             uint32_t start_idx,
                             uint32_t num_element,
                             int free_template)
{
    int dunit;
    bcmlt_field_def_t fld_defs;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (*template == BCMLT_INVALID_HDL) {
        /* Entry handle allocate for given template */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_allocate(dunit, table_name, template));
    }

    if (key_name != NULL) {
        /* Add given key to the template. */
        if (key_symbol == TRUE) {
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmlt_entry_field_symbol_add(*template, key_name, key_str));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmlt_entry_field_add(*template, key_name, key_val));
        }
    }

    if (field_name != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, (const char *)table_name,
                                   field_name, &fld_defs));

        if (fld_defs.elements > 1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(*template,
                                                 field_name,
                                                 start_idx,
                                                 &field_val,
                                                 num_element));
        } else {
            /* Add given field value to the given field_name. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(*template, field_name, field_val));
        }
    }

    /* Perform the operation */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmi_lt_entry_commit(unit, *template, operation, priority));

exit:
    if (free_template) {
       (void) bcmlt_entry_free(*template);
       *template = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief To get field module control structure.
 *
 * \param [in] Unit Number.
 * \param [out] fc structure.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcmint_field_control_get(int unit, bcmint_field_control_info_t **fc)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fc, SHR_E_PARAM);
    if (bcmi_ltsw_field_db[unit].lock == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    *fc = &(bcmi_ltsw_field_db[unit].control_db);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To get field module stage structure.
 *
 * \param [in] Unit Number.
 * \param [in] stage id.
 * \param [out] stage structure.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcmint_field_stage_info_get(int unit,
                            bcmi_ltsw_field_stage_t stage_id,
                            bcmint_field_stage_info_t **stage_info)
{
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    *stage_info = &(fp_control->stage_info[stage_id]);

    if ((*stage_info)->tbls_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Create protection mutex for field module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmint_field_lock_create(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_field_db[unit].lock == NULL) {
        bcmi_ltsw_field_db[unit].lock = sal_mutex_create("bcmFieldCntlMutex");
        SHR_NULL_CHECK(bcmi_ltsw_field_db[unit].lock, SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy protection mutex for field module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
bcmint_field_lock_destroy(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_field_db[unit].lock != NULL) {
        sal_mutex_destroy(bcmi_ltsw_field_db[unit].lock);
        bcmi_ltsw_field_db[unit].lock = NULL;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief create sbr profile for default entry supported stage.
 *
 * \param [in] unit Unit Number.
 * \param [in] Control info.
 *
 * \retval SHR_E_NONE Success.
 */
static int
bcmint_field_stage_default_entry_sbr_prof_create(int unit,
                             bcmint_field_control_info_t *control_info)
{
    bcmi_ltsw_field_stage_t stage_id;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t sbr_template = BCMLT_INVALID_HDL;
    bcm_field_action_t action = 0;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    int iter = 0;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);
    for (stage_id = bcmiFieldStageIngress;
         stage_id < bcmiFieldStageCount; stage_id++) {
        stage_info = &(control_info->stage_info[stage_id]);
        if (!((stage_info->flags & BCMINT_FIELD_STAGE_DEFAULT_ENTRY_SUPPORT) &&
             (stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR))) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_allocate(dunit,
               stage_info->tbls_info->sbr_sid, &sbr_template));

        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(sbr_template,
               stage_info->tbls_info->sbr_key_fid,
               (stage_info->strength_id_max) + 1));

        for (action = 0; action < bcmFieldActionCount; action++) {
            if (BCM_FIELD_QSET_TEST(stage_info->sbr_aset, action) == 0) {
                continue;
            }
            if ((bcmFieldActionStatGroup == action) ||
                (bcmFieldActionStatGroupWithoutCounterIndex == action) ||
                (bcmFieldActionPolicerGroup == action) ||
                (bcmFieldActionGpStatGroup == action) ||
                (bcmFieldActionYpStatGroup == action) ||
                (bcmFieldActionRpStatGroup == action)) {
                /* actions not supported in default entry */
                continue;
            }
            lt_map_ref = NULL;
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmint_field_action_map_find(unit, action,
                   stage_info, &lt_map_ref));

            for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(sbr_template,
                        (lt_map_ref->map[iter]).lt_field_name, 1));
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, sbr_template,
                BCMLT_OPCODE_INSERT, BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_free(sbr_template));
    }
exit:
    if (BCMLT_INVALID_HDL != sbr_template) {
        (void) bcmlt_entry_free(sbr_template);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize stage info for field module.
 *
 * \param [in] unit Unit Number.
 * \param [in/out] Control info.
 *
 * \retval SHR_E_NONE Success.
 */
static int
bcmint_field_stage_info_init(int unit,
                             bcmint_field_control_info_t *control_info)
{
    int iter = 0;
    uint8_t found = 0;
    bcmi_field_action_t action = 0;
    bcmi_ltsw_field_stage_t stage_id;
    bcmint_field_stage_info_t *stage_info = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;


    SHR_FUNC_ENTER(unit);

    for (stage_id = bcmiFieldStageIngress;
         stage_id < bcmiFieldStageCount; stage_id++) {

        /* Retreive stage info structure */
        stage_info = &(control_info->stage_info[stage_id]);

        stage_info->stage_id = stage_id;
        stage_info->stage_base_id = (int)stage_id << 24;
        stage_info->flags = 0;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_stage_info_init(unit,
                                             stage_info));
        if (NULL == stage_info->tbls_info) {
            /* Given Stage is not supported by the device. */
            continue;
        }

        if (stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR) {
            uint64_t sbr_min;
            uint64_t sbr_max;

            /* check the range of entry priority */
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmi_lt_field_value_range_get(unit,
                                       stage_info->tbls_info->sbr_sid,
                                       stage_info->tbls_info->sbr_key_fid,
                                       &sbr_min,
                                       &sbr_max));
            stage_info->sbr_max_limit = (uint16_t)sbr_max;
            if (stage_info->flags & BCMINT_FIELD_STAGE_DEFAULT_ENTRY_SUPPORT) {
                /* the strength_id_max+1 will be used for default entry*/
                stage_info->strength_id_max = sbr_max - 2;
            } else {
                stage_info->strength_id_max = sbr_max - 1;
            }
        }

        /* Update all sbr enabled action set in stage structure */
        stage_info->sbr_aset_cnt = 0;
        BCM_FIELD_ASET_INIT(stage_info->sbr_aset);
        for (action = 0; action < bcmiFieldActionCount; action++) {
            lt_map_ref = NULL;
            found = 0;
            bcmint_field_action_map_find(unit, action,
                                         stage_info, &lt_map_ref);
            if (lt_map_ref != NULL) {
                for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
                    if ((lt_map_ref->map[iter]).sbr_enabled == TRUE) {
                        found = 1;
                    }
                }
                if (found) {
                    stage_info->sbr_aset_cnt++;
                    BCM_FIELD_ASET_ADD(stage_info->sbr_aset, action);
                }
            }
        }
        if (stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_SUPPORTED) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_stage_compress_type_init(unit, stage_info));
        }
        if (stage_info->flags & BCMINT_FIELD_STAGE_PROFILE_SUPPORT) {
            /* Initialize profiles */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_ingress_profile_init(unit));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to clear all the stage information.
 */
int
bcmint_field_stage_info_clear(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    uint8_t blk_id;
    SHR_FUNC_ENTER(unit);

    /* Dealloc Entry Operational data */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_remove_all(unit, stage_info));
    /* Dealloc Entry Hash block */
    FP_ENTRY_HASH_BLK_ID(unit, stage_info->stage_id, blk_id);
    FP_HA_ROOT_BLK_ID_BMP_CLR(unit, blk_id);
    FP_HA_BLK_ID_DEALLOCATE(unit, blk_id);
    /* Dealloc Entry info */
    FP_ENTRY_INFO_BLK_ID(unit, stage_info->stage_id, blk_id);
    FP_HA_ROOT_BLK_ID_BMP_CLR(unit, blk_id);
    FP_HA_BLK_ID_DEALLOCATE(unit, blk_id);
    stage_info->entry_info = NULL;

    /* Dealloc Presel Operational data */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_oper_remove_all(unit, stage_info));
    /* Dealloc Entry Hash block */
    FP_PRESEL_HASH_BLK_ID(unit, stage_info->stage_id, blk_id);
    FP_HA_ROOT_BLK_ID_BMP_CLR(unit, blk_id);
    FP_HA_BLK_ID_DEALLOCATE(unit, blk_id);
    /* Dealloc Presel Info block */
    FP_PRESEL_INFO_BLK_ID(unit, stage_info->stage_id, blk_id);
    FP_HA_ROOT_BLK_ID_BMP_CLR(unit, blk_id);
    FP_HA_BLK_ID_DEALLOCATE(unit, blk_id);
    stage_info->presel_info = NULL;

    /* Dealloc Group Operational data */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_remove_all(unit, stage_info));
    /* Dealloc Group Hash block */
    FP_GRP_HASH_BLK_ID(unit, stage_info->stage_id, blk_id);
    FP_HA_ROOT_BLK_ID_BMP_CLR(unit, blk_id);
    FP_HA_BLK_ID_DEALLOCATE(unit, blk_id);
    /* Dealloc Group Info block */
    FP_GRP_INFO_BLK_ID(unit, stage_info->stage_id, blk_id);
    FP_HA_ROOT_BLK_ID_BMP_CLR(unit, blk_id);
    FP_HA_BLK_ID_DEALLOCATE(unit, blk_id);
    stage_info->group_info = NULL;

    if (stage_info->flags & BCMINT_FIELD_STAGE_PROFILE_SUPPORT) {
        /* Initialize profiles */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_ingress_profile_deinit(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to clear all the stage information.
 */
static int
bcmint_field_hint_info_clear(
    int unit)
{
    uint8_t blk_id;

    SHR_FUNC_ENTER(unit);

    /* Dealloc all group hints information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_hints_destroy_all(unit));
    /* Dealloc hint list. */
    blk_id = FP_HINT_LIST_BLK_ID(unit);
    FP_HA_ROOT_BLK_ID_BMP_CLR(unit, blk_id);
    FP_HA_BLK_ID_DEALLOCATE(unit, blk_id);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the field control information.
 *
 * This function initializes the field control information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmint_field_control_info_init(int unit)
{
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    sal_memset(fp_control, 0, sizeof(bcmint_field_control_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_control_info_init(unit, fp_control));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_init(unit, fp_control));

    /* create default sbr profile for default entry supported stages */
    if (!(bcmi_warmboot_get(unit))) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmint_field_stage_default_entry_sbr_prof_create(unit,
                fp_control));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the field control information.
 *
 * This function detaches the field control information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmint_field_control_info_detach(int unit)
{
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_hint_cleanup(unit));

    sal_memset(fp_control, 0x0, sizeof(bcmint_field_control_info_t));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the field database.
 *
 * This function initializes the field database.
 *
 * \param [in] unit: Unit number.
 * \param [in] warm: Warmboot state.
 *
 * \retval SHR_E_NONE Initialize the field database successfully.
 * \retval SHR_E_PARAM Invalid parameters.
 */
static int
bcmint_field_db_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_info_init(unit));

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Detach the field database.
 *
 * This function initialize the field database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 */
static int
bcmint_field_db_detach(int unit, bool warm, bool init)
{
    int rv, stage_id;
    bcmint_field_stage_info_t *stage_info;

    SHR_FUNC_ENTER(unit);

    /* Field Stage clear */
    for (stage_id = bcmiFieldStageIngress;
         stage_id < bcmiFieldStageCount; stage_id++) {
        rv = bcmint_field_stage_info_get(unit, stage_id,
                                         &stage_info);
        if (SHR_FAILURE(rv) || stage_info == NULL) {
            continue;
        }

        if ((warm == FALSE) && (init == FALSE)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_destroy_all(unit,
                                                stage_info));
            if (stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_presel_entry_destroy_all(unit,
                                                           stage_info));
            }
        } else {
            if (init == false) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_strength_destroy_all(unit,
                                                       stage_info));
            }
        }
    }

    if ((warm == FALSE) && (init == FALSE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_hint_info_clear(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_ha_cleanup(unit, warm));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_info_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear all the field related Logical Tables.
 *
 * \param [in] unit: Unit Number.
 * \param [in] warm: Warmboot state.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY No Memory Resource.
 */
static int
bcmint_field_clear(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    if (!warm) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_clear(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_field_port_filter_enable_set(int unit, uint32 state)
{
    bcm_port_config_t  port_config; /* Device port config structure.*/
    bcm_pbmp_t port_pbmp;

    SHR_FUNC_ENTER(unit);

    /* Read device port configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));
    BCM_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_ASSIGN(port_pbmp, port_config.all);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_port_filter_enable_set(unit, state,
                                                port_pbmp));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the field module.
 *
 * Detach field database data structure.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcmint_field_detach(int unit, bool warm, bool init)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_db_detach(unit, warm, init));

    /* Clear field Logical tables. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_clear(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the field module.
 *
 * Initial field database data structure and
 * clear the field related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY No Memory Resource.
 */
static int
bcmint_field_init(int unit)
{
    bool warm = bcmi_warmboot_get(unit);
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_detach(unit, warm, TRUE));

    /* Field Database initialization. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_db_init(unit, warm));

    /* Field Operational HA DB initialization. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_ha_init(unit, warm));

    if (!warm) {
        /* Enable Filter on all logical ports */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_port_filter_enable_set(unit, TRUE));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_init(unit));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmint_field_stage_compress_type_init(int unit,
                           bcmint_field_stage_info_t *stage_info)
{
    int dunit = 0;
    uint64_t compress_type = 0x0;
    bcmlt_entry_handle_t fp_compress_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    dunit = bcmi_ltsw_dev_dunit(unit);

    if (stage_info->tbls_info->compress_sid == NULL) {
        SHR_EXIT();
    }
    /* Entry handle allocate for operational mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->compress_sid,
                              &fp_compress_template));

    /* Lookup mode template using handle. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, fp_compress_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Retreive operation mode field from fetched entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(fp_compress_template,
                               stage_info->tbls_info->compress_src_ip4_only_sid,
                               &compress_type));

    if (compress_type == 1) {
        stage_info->flags |= BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY;
    }
    compress_type =0x00;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(fp_compress_template,
                               stage_info->tbls_info->compress_src_ip6_only_sid,
                               &compress_type));
    if (compress_type == 1) {
        stage_info->flags |= BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY;
    }
    compress_type =0x00;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(fp_compress_template,
                               stage_info->tbls_info->compress_dst_ip4_only_sid,
                               &compress_type));
    if (compress_type == 1) {
        stage_info->flags |= BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY;
    }
    compress_type =0x00;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(fp_compress_template,
                               stage_info->tbls_info->compress_dst_ip6_only_sid,
                               &compress_type));
    if (compress_type == 1) {
        stage_info->flags |= BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY;
    }

exit:
    if (BCMLT_INVALID_HDL != fp_compress_template) {
        (void) bcmlt_entry_free(fp_compress_template);
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief check if the given map is supported for IFP or EM stages.
 *
 * \param [in] unit Unit Number.
 *
 * \param  [in] stage_info stage info.
 *
 * \param  [in] map qualifier field map.
 *
 * \retval TRUE if supported else FALSE.
 */
bool
bcmint_field_map_supported(int unit,
                           bcmint_field_stage_info_t *stage_info,
                           bcm_field_qual_map_info_t map)
{
    bool is_supported = FALSE;
    if ((map.em == BCMI_FIELD_STAGE_LTMAP_ALL) ||
        ((((stage_info->stage_id == bcmiFieldStageExactMatch) ||
           (stage_info->stage_id == bcmiFieldStageFlowTracker)) &&
        (map.em == BCMI_FIELD_STAGE_LTMAP_EM_ONLY))) ||
        ((stage_info->stage_id == bcmiFieldStageIngress) &&
        (map.em == BCMI_FIELD_STAGE_LTMAP_IFP_ONLY))) {
        is_supported = TRUE;
    }
    return is_supported;
}

/*
 * Do search for given qualifier and return success or failure.
 *
 * Algorithm:
 *  1. Search provided qualifier in database as indicated by
 *     stage id and presel input
 *  2. If qualifier is not present, return NOT FOUND
 *  3. If qualifier is present in database then return
 *     SUCCESS
 *  4. If qualifier is present in database then
 *     pointer to db if input p2p is not NULL
 */
int
bcmint_field_qual_map_find(int unit,
                           bcmi_field_qualify_t qual,
                           bcmint_field_stage_info_t *stage_info,
                           bool presel,
                           bool group_map,
                           int *map_count,
                           bcm_field_qual_map_t **lt_map)
{
    int start = 0, end = 0;
    int midpoint = 0, result = 0;
    bool found = 0;
    int indx = 0;
    bcm_field_qual_map_t *lt_map_db = NULL;
    bcm_field_qual_map_t *lt_map_db_tmp = NULL;

    SHR_FUNC_ENTER(unit);
    *map_count = 0;
    /* Retreive entry count & db from stage first */
    if (presel) {
        lt_map_db_tmp = lt_map_db = stage_info->presel_qual_lt_map_db;
        end = stage_info->presel_qual_db_count - 1;
    } else if (qual < (int) bcmFieldQualifyCount) {
        if ((group_map) && (stage_info->group_qual_lt_map_db))  {
            lt_map_db_tmp = lt_map_db = stage_info->group_qual_lt_map_db;
            end = stage_info->group_qual_db_count - 1;
        } else  {
            lt_map_db_tmp = lt_map_db = stage_info->qual_lt_map_db;
            end = stage_info->qual_db_count - 1;
        }
    } else {
        lt_map_db_tmp = lt_map_db = stage_info->udf_qual_lt_map_db;
        end = stage_info->udf_qual_db_count - 1;
    }

    /* do binary search over db */
    while (end >= start) {
        midpoint = (end + start) / 2;
        lt_map_db_tmp = lt_map_db + midpoint;

        /* compare current db node with target qualifier */
        if ((int) lt_map_db_tmp->qual < (int) qual) {
            result = -1;
        } else if ((int) lt_map_db_tmp->qual > (int) qual) {
            result = 1;
        } else {
            result = 0;
        }

        /* adjust start/end based on compare results */
        if (result == 1) {
            end = midpoint - 1;
        } else if (result == -1) {
            start = midpoint + 1;
        } else {
            found = 1;
            break;
        }
    }

    /* return results */
    if (found) {
        if (lt_map != NULL) {
            *lt_map = lt_map_db + midpoint;
            for (indx=0; indx < (*lt_map)->num_maps; indx++) {
                if (TRUE == bcmint_field_map_supported(unit, stage_info,
                            (*lt_map)->map[indx])) {
                  (*map_count)++;
                }
            }
            if (*map_count == 0) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "Error: Qualifier is not "
                    " supported by the device.\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
        }
    } else {
        LOG_DEBUG(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "Error: Qualifier is not "
                              " supported by the device.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Private functions
 */
void
bcmint_entry_field_buffer_set(bcm_qual_data_type_t data_type,
                              uint32_t *inp_buf,
                              uint32_t *op_buf,
                              uint32_t src_offset,
                              uint32_t dst_offset,
                              uint32_t width)
{
    void *ibuf_ptr = NULL;
    uint64_t *obuf_ptr = NULL;
    uint32_t src_bit_offset = 0, dst_bit_offset = 0;
    uint32_t part_width = 0;
    uint64_t tmp64 = 0;
    uint64_t mask64 = 0xFFFFFFFFFFFFFFFFULL;
    uint64_t mask = 0, clr_mask = 0;

    obuf_ptr = ((uint64_t *)op_buf + (dst_offset / 64));
    dst_bit_offset = (dst_offset % 64);
    mask = (mask64 >> (64 - width));
    clr_mask = ~(mask << dst_bit_offset);
    switch(data_type) {
    case BCM_QUAL_DATA_TYPE_INT:
    case BCM_QUAL_DATA_TYPE_TRUNK:
    case BCM_QUAL_DATA_TYPE_IF:
    case BCM_QUAL_DATA_TYPE_L4_PORT:
    case BCM_QUAL_DATA_TYPE_VRF:
    case BCM_QUAL_DATA_TYPE_COS:
    case BCM_QUAL_DATA_TYPE_MULTICAST:
    case BCM_QUAL_DATA_TYPE_IF_GROUP:
    case BCM_QUAL_DATA_TYPE_TRILL_NAME:
    case BCM_QUAL_DATA_TYPE_GPORT:
    case BCM_QUAL_DATA_TYPE_PORT:
    case BCM_QUAL_DATA_TYPE_STG:
        ibuf_ptr = ((int *)inp_buf + (src_offset / 32));
        src_bit_offset = (src_offset % 32);
        *obuf_ptr &= clr_mask;
        *obuf_ptr |= (((*(int *)ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_UINT8:
        ibuf_ptr = ((uint8_t *)inp_buf + (src_offset / 8));
        src_bit_offset = (src_offset % 8);
        *obuf_ptr &= clr_mask;
        *obuf_ptr |= (((*(uint8_t *)ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_UINT16:
    case BCM_QUAL_DATA_TYPE_VLAN:
    case BCM_QUAL_DATA_TYPE_VPN:
    case BCM_QUAL_DATA_TYPE_PBMP:
        ibuf_ptr = ((uint16_t *)inp_buf + (src_offset / 16));
        src_bit_offset = (src_offset % 16);
        *obuf_ptr &= clr_mask;
        *obuf_ptr |= (((*(uint16_t *)ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_UINT32:
    case BCM_QUAL_DATA_TYPE_IP:
    case BCM_QUAL_DATA_TYPE_NAT_ID:
    case BCM_QUAL_DATA_TYPE_MPLS_LABEL:
    case BCM_QUAL_DATA_TYPE_LLC:
        ibuf_ptr = ((uint32_t *)inp_buf + (src_offset / 32));
        src_bit_offset = (src_offset % 32);
        *obuf_ptr &= clr_mask;
        *obuf_ptr |= (((*(uint32_t *)ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_UINT64:
    case BCM_QUAL_DATA_TYPE_CLASS:
    case BCM_QUAL_DATA_TYPE_SNAP:
        ibuf_ptr = ((uint64_t *)inp_buf + (src_offset / 64));
        src_bit_offset = (src_offset % 64);
        *obuf_ptr &= clr_mask;
        *obuf_ptr |= (((*(uint64_t *)ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_MAC:
    case BCM_QUAL_DATA_TYPE_IP6:
        if ((dst_offset + width) > 64 && (dst_offset < 64)) {
            /* Field span two uint64 */
            obuf_ptr = (uint64_t *)op_buf;
            part_width = ((dst_offset + width) - 64);
            ibuf_ptr = ((uint8_t *)inp_buf + (src_offset / 8));
            tmp64 = 0;
            bcmi_ltsw_util_generic_to_uint64(&tmp64, ibuf_ptr, part_width);
            mask = (mask64 >> (64 - part_width));
            clr_mask = ~(mask);
            obuf_ptr[1] &= clr_mask;
            obuf_ptr[1] |= (tmp64 & mask);

            width -= part_width;
            src_offset += part_width;
            ibuf_ptr = ((uint8_t *)inp_buf + (src_offset / 8));
            tmp64 = 0;
            bcmi_ltsw_util_generic_to_uint64(&tmp64, ibuf_ptr, width);
            mask = (mask64 >> (64 - width));
            clr_mask = ~(mask << dst_bit_offset);
            obuf_ptr[0] &= clr_mask;
            obuf_ptr[0] |= ((tmp64 & mask) << dst_bit_offset);
        } else {
            /* Field is limited to one uint64 */
            obuf_ptr = ((uint64_t *)op_buf + (dst_offset / 64));
            ibuf_ptr = ((uint8_t *)inp_buf + (src_offset / 8));
            tmp64 = 0;
            bcmi_ltsw_util_generic_to_uint64(&tmp64, ibuf_ptr, width);
            mask = (mask64 >> (64 - width));
            clr_mask = ~(mask << dst_bit_offset);
            *obuf_ptr &= clr_mask;
            *obuf_ptr |= ((tmp64 & mask) << dst_bit_offset);
        }
        break;
    default:
        break;
    }
}

void
bcmint_entry_field_buffer_get(bcm_qual_data_type_t data_type,
                              uint32_t *inp_buf,
                              uint32_t *op_buf,
                              uint32_t src_offset,
                              uint32_t dst_offset,
                              uint32_t width)
{
    uint64_t *ibuf_ptr = NULL;
    void *obuf_ptr = NULL;
    uint32_t dst_bit_offset = 0, src_bit_offset = 0;
    uint32_t part_width = 0;
    uint64_t tmp64 = 0;
    uint64_t mask64 = 0xFFFFFFFFFFFFFFFFULL;
    uint64_t mask = 0;

    ibuf_ptr = ((uint64_t *)inp_buf + (src_offset / 64));
    src_bit_offset = (src_offset % 64);
    mask = (mask64 >> (64 - width));
    switch(data_type) {
    case BCM_QUAL_DATA_TYPE_INT:
    case BCM_QUAL_DATA_TYPE_TRUNK:
    case BCM_QUAL_DATA_TYPE_IF:
    case BCM_QUAL_DATA_TYPE_L4_PORT:
    case BCM_QUAL_DATA_TYPE_VRF:
    case BCM_QUAL_DATA_TYPE_COS:
    case BCM_QUAL_DATA_TYPE_MULTICAST:
    case BCM_QUAL_DATA_TYPE_IF_GROUP:
    case BCM_QUAL_DATA_TYPE_TRILL_NAME:
    case BCM_QUAL_DATA_TYPE_GPORT:
    case BCM_QUAL_DATA_TYPE_PORT:
    case BCM_QUAL_DATA_TYPE_STG:
        obuf_ptr = ((int *)op_buf + (dst_offset / 32));
        dst_bit_offset = (dst_offset % 32);
        *(int *)obuf_ptr |= (((*ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_UINT8:
        obuf_ptr = ((uint8_t *)op_buf + (dst_offset / 8));
        dst_bit_offset = (dst_offset % 8);
        *(uint8_t *)obuf_ptr |= (((*ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_UINT16:
    case BCM_QUAL_DATA_TYPE_VLAN:
    case BCM_QUAL_DATA_TYPE_VPN:
    case BCM_QUAL_DATA_TYPE_PBMP:
        obuf_ptr = ((uint16_t *)op_buf + (dst_offset / 16));
        dst_bit_offset = (dst_offset % 16);
        *(uint16_t *)obuf_ptr |= (((*ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_UINT32:
    case BCM_QUAL_DATA_TYPE_IP:
    case BCM_QUAL_DATA_TYPE_NAT_ID:
    case BCM_QUAL_DATA_TYPE_MPLS_LABEL:
    case BCM_QUAL_DATA_TYPE_LLC:
        obuf_ptr = ((uint32_t *)op_buf + (dst_offset / 32));
        dst_bit_offset = (dst_offset % 32);
        *(uint32_t *)obuf_ptr |= (((*ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_UINT64:
    case BCM_QUAL_DATA_TYPE_CLASS:
    case BCM_QUAL_DATA_TYPE_SNAP:
        obuf_ptr = ((uint64_t *)op_buf + (dst_offset / 64));
        dst_bit_offset = (dst_offset % 64);
        *(uint64_t *)obuf_ptr |= (((*ibuf_ptr >> src_bit_offset) & mask) << dst_bit_offset);
        break;
    case BCM_QUAL_DATA_TYPE_MAC:
    case BCM_QUAL_DATA_TYPE_IP6:
        if ((src_offset + width) > 64 && (src_offset < 64)) {
            /* Field spans two uint64 */
            ibuf_ptr = (uint64_t *)inp_buf;
            obuf_ptr = ((uint8_t *)op_buf + (dst_offset / 8));
            part_width = ((src_offset + width) - 64);
            mask = (mask64 >> (64 - part_width));
            tmp64 = ((ibuf_ptr[1] & mask));
            bcmi_ltsw_util_uint64_to_generic(obuf_ptr, &tmp64, part_width);

            width -= part_width;
            dst_offset += part_width;
            obuf_ptr = ((uint8_t *)op_buf + (dst_offset / 8));
            mask = (mask64 >> (64 - width));
            tmp64 = ((ibuf_ptr[0] >> src_bit_offset) & mask);
            bcmi_ltsw_util_uint64_to_generic(obuf_ptr, &tmp64, width);
        } else {
            /* Field is limited to one uint64 */
            ibuf_ptr = ((uint64_t *)inp_buf + (src_offset / 64));
            obuf_ptr = ((uint8_t *)op_buf + (dst_offset / 8));
            mask = (mask64 >> (64 - width));
            tmp64 = ((*ibuf_ptr >> src_bit_offset) & mask);
            bcmi_ltsw_util_uint64_to_generic(obuf_ptr, &tmp64, width);
        }
        break;
    default:
        break;
    }
}

void
bcmint_entry_field_qual_buffer_set(int unit,
                                   const bcm_field_qual_map_info_t *map,
                                   bcmlt_field_def_t *fld_defs,
                                   bcm_qual_field_t *qual_info,
                                   uint32_t *src_ofst_current,
                                   uint32_t *qual_value_inp_buf,
                                   uint32_t *qual_mask_inp_buf,
                                   uint32_t *qual_value,
                                   uint32_t *qual_mask_value)
{
    int idx = 0;
    uint32_t qual_fixed_ofst = 0;
    uint32_t width = 0, src_offset = 0, dst_offset = 0;

    /* Certain qualifier like SrcIp6High/DstIp6High
     * data is picked from upper 64b though passed value is 128b. In these
     * cases we have to add this fixed offset to pick up input
     * data correctly. */
    if ((qual_info->sdk6_qual_enum == bcmFieldQualifySrcIp6Low) ||
        (qual_info->sdk6_qual_enum == bcmFieldQualifyDstIp6Low)) {
        qual_fixed_ofst = 64;
    }

    if (map->width[0] == 0) {
        width = fld_defs->width;
        if ((fld_defs->dtag == BCMLT_FIELD_DATA_TAG_BOOL)
            && (fld_defs->width == 1)
            && (fld_defs->elements > 1)) {
            for (idx = 0; idx < fld_defs->elements; idx++) {
                 dst_offset = (idx * 64);
                 src_offset = (*src_ofst_current + qual_fixed_ofst);
                 bcmint_entry_field_buffer_set(qual_info->data_type,
                                               qual_value_inp_buf,
                                               qual_value,
                                               src_offset,
                                               dst_offset,
                                               width);
                 bcmint_entry_field_buffer_set(qual_info->data_type,
                                               qual_mask_inp_buf,
                                               qual_mask_value,
                                               src_offset,
                                               dst_offset,
                                               width);
                 *src_ofst_current += width;
            }
        } else {
            dst_offset = 0;
            src_offset = (*src_ofst_current + qual_fixed_ofst);
            bcmint_entry_field_buffer_set(qual_info->data_type,
                                          qual_value_inp_buf,
                                          qual_value,
                                          src_offset,
                                          dst_offset,
                                          width);
            bcmint_entry_field_buffer_set(qual_info->data_type,
                                          qual_mask_inp_buf,
                                          qual_mask_value,
                                          src_offset,
                                          dst_offset,
                                          width);
            *src_ofst_current += width;
        }
    } else {

        /* Set qualifier in o/p as per offset/widths specified in map */
        for (idx = 0; idx < map->num_offsets; idx++) {
            width =  map->width[idx];
            src_offset = (*src_ofst_current + qual_fixed_ofst);
            dst_offset = map->offset[idx];
            bcmint_entry_field_buffer_set(qual_info->data_type,
                                          qual_value_inp_buf,
                                          qual_value,
                                          src_offset,
                                          dst_offset,
                                          width);
            bcmint_entry_field_buffer_set(qual_info->data_type,
                                          qual_mask_inp_buf,
                                          qual_mask_value,
                                          src_offset,
                                          dst_offset,
                                          width);
            *src_ofst_current += width;
        }
    }
}

void
bcmint_entry_field_qual_buffer_get(int unit,
                                   const bcm_field_qual_map_info_t *map,
                                   bcmlt_field_def_t *fld_defs,
                                   bcm_qual_field_t *qual_info,
                                   uint32_t *src_ofst_current,
                                   uint32_t *qual_value_inp_buf,
                                   uint32_t *qual_mask_inp_buf,
                                   uint32_t *qual_value,
                                   uint32_t *qual_mask_value)
{
    int idx = 0;
    uint32_t qual_fixed_ofst = 0;
    uint32_t width = 0, src_offset = 0, dst_offset = 0;

    /* Certain qualifier like SrcIp6High/DstIp6High
     * data is picked from upper 64b though passed value is 128b. In these
     * cases we have to add this fixed offset to pick up input
     * data correctly. */
    if ((qual_info->sdk6_qual_enum == bcmFieldQualifySrcIp6Low) ||
        (qual_info->sdk6_qual_enum == bcmFieldQualifyDstIp6Low)) {
        qual_fixed_ofst = 64;
    }

    if (map->width[0] == 0) {
        width = fld_defs->width;
        if ((fld_defs->dtag == BCMLT_FIELD_DATA_TAG_BOOL)
            && (fld_defs->width == 1)
            && (fld_defs->elements > 1)) {
            for (idx = 0; idx < fld_defs->elements; idx++) {
                src_offset = (idx * 64);
                dst_offset = (*src_ofst_current + qual_fixed_ofst);
                bcmint_entry_field_buffer_get(qual_info->data_type,
                                              qual_value,
                                              qual_value_inp_buf,
                                              src_offset,
                                              dst_offset,
                                              width);
                bcmint_entry_field_buffer_get(qual_info->data_type,
                                              qual_mask_value,
                                              qual_mask_inp_buf,
                                              src_offset,
                                              dst_offset,
                                              width);
                *src_ofst_current += width;
            }
        } else {
            /* Qualifier takes full field width */
            src_offset = 0;
            dst_offset = (*src_ofst_current + qual_fixed_ofst);
            bcmint_entry_field_buffer_get(qual_info->data_type,
                                          qual_value,
                                          qual_value_inp_buf,
                                          src_offset,
                                          dst_offset,
                                          width);
            bcmint_entry_field_buffer_get(qual_info->data_type,
                                          qual_mask_value,
                                          qual_mask_inp_buf,
                                          src_offset,
                                          dst_offset,
                                          width);
            *src_ofst_current += width;
        }
    } else {
        /* Get qualifier in o/p as per offset/widths specified in map */
        for (idx = 0; idx < map->num_offsets; idx++) {
            width =  map->width[idx];
            dst_offset = (*src_ofst_current + qual_fixed_ofst);
            src_offset = map->offset[idx];
            bcmint_entry_field_buffer_get(qual_info->data_type,
                                          qual_value,
                                          qual_value_inp_buf,
                                          src_offset,
                                          dst_offset,
                                          width);
            bcmint_entry_field_buffer_get(qual_info->data_type,
                                          qual_mask_value,
                                          qual_mask_inp_buf,
                                          src_offset,
                                          dst_offset,
                                          width);
            *src_ofst_current += width;
        }
    }
}

void
bcmint_entry_field_qual_buffer_delete(int unit,
                                      const bcm_field_qual_map_info_t *map,
                                      bcmlt_field_def_t *fld_defs,
                                      bcm_qual_field_t *qual_info,
                                      uint32_t *src_ofst_current,
                                      uint32_t *qual_value_inp_buf,
                                      uint32_t *qual_mask_inp_buf,
                                      uint32_t *qual_value,
                                      uint32_t *qual_mask_value)
{
    int idx = 0;
    uint32_t width = 0;
    uint32_t part_width = 0, offset = 0;
    uint64_t mask = 0xFFFFFFFFFFFFFFFFULL;
    uint64_t clr_mask = 0;
    uint64_t *dst_val = NULL;
    uint64_t *dst_mask = NULL;

    if (map->width[0] == 0) {
        width = fld_defs->width;
        dst_val = (uint64_t *)qual_value;
        dst_mask = (uint64_t *)qual_mask_value;
        if (width > 64) {
            part_width = (width - 64);
            clr_mask = ~(mask >> (64 - part_width));
            dst_val[1] &= clr_mask;
            dst_mask[1] &= clr_mask;
            width -= part_width;
            *src_ofst_current += part_width;
        }
        clr_mask = ~(mask >> (64 - width));
        dst_val[0] &= clr_mask;
        dst_mask[0] &= clr_mask;
        *src_ofst_current += width;
    } else {
        /* Set qualifier in o/p as per offset/widths specified in map */
        for (idx = 0; idx < map->num_offsets; idx++) {
            width = map->width[idx];
            offset = map->offset[idx];
            if ((offset < 64) && ((offset + width) > 64)) {
                dst_val = (uint64_t *)qual_value;
                dst_mask = (uint64_t *)qual_mask_value;
                part_width = ((offset + width) - 64);
                clr_mask = ~(mask >> (64 - part_width));
                dst_val[1] &= (clr_mask);
                dst_mask[1] &= (clr_mask);
                width -= part_width;
                *src_ofst_current += part_width;

                clr_mask = (mask >> (64 - width));
                dst_val[0] &= ~(clr_mask << offset);
                dst_mask[0] &= ~(clr_mask << offset);
                *src_ofst_current += width;
            } else {
                dst_val = ((uint64_t *)qual_value + (offset / 64));
                dst_mask = ((uint64_t *)qual_mask_value + (offset / 64));
                clr_mask = (mask >> (64 - width));
                *dst_val &= ~(clr_mask << (offset % 64));
                *dst_mask &= ~(clr_mask << (offset % 64));
                *src_ofst_current += width;
            }
        }
    }
}

/*!
 * \brief validate the max value of data based on width of the qualifier.
 *
 * validate the qual data passed against the max allowed data
 * for the qualifier based on width of the qualifier
 *
 * \param [in] unit Unit Number.
 * \param [in] width Width of the qualifier.
 * \param [in] qual_info Qualfier information
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM provided qualifier data is invalid.
 * \retval SHR_E_UNAVAIL provided qualifier is invalid.
 */
static int
bcmint_field_qualifier_validate(int unit,
                                int width,
                                bcm_qual_field_t *qual_info)
{
    switch(qual_info->data_type) {
        case BCM_QUAL_DATA_TYPE_INT:
        case BCM_QUAL_DATA_TYPE_TRUNK:
        case BCM_QUAL_DATA_TYPE_IF:
        case BCM_QUAL_DATA_TYPE_L4_PORT:
        case BCM_QUAL_DATA_TYPE_VRF:
        case BCM_QUAL_DATA_TYPE_COS:
        case BCM_QUAL_DATA_TYPE_MULTICAST:
        case BCM_QUAL_DATA_TYPE_IF_GROUP:
        case BCM_QUAL_DATA_TYPE_TRILL_NAME:
        case BCM_QUAL_DATA_TYPE_GPORT:
        case BCM_QUAL_DATA_TYPE_PORT:
        case BCM_QUAL_DATA_TYPE_STG:
            if ((qual_info->data_type == BCM_QUAL_DATA_TYPE_PORT) &&
                 BCM_GPORT_IS_SET(*((uint *)qual_info->qual_data))) {
                return SHR_E_NONE;
            }
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if ((qual_info->enum_name == NULL) &&
                    (qual_info->enum_value >
                    (BCMINT_FIELD_MAX_WIDTH_VALUE(width)))) {
                    return SHR_E_PARAM;
                }
            } else {
                if ((*((uint *)qual_info->qual_data)) >
                    (BCMINT_FIELD_MAX_WIDTH_VALUE(width))) {
                    return SHR_E_PARAM;
                }
            }
            break;
        case BCM_QUAL_DATA_TYPE_UINT8:
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if ((qual_info->enum_name == NULL) &&
                    (qual_info->enum_value >
                    (BCMINT_FIELD_MAX_WIDTH_VALUE(width)))) {
                    return SHR_E_PARAM;
                }
            } else {
                if ((*((uint8_t *)qual_info->qual_data)) >
                    (BCMINT_FIELD_MAX_WIDTH_VALUE(width))) {
                    return SHR_E_PARAM;
                }
            }
            break;
        case BCM_QUAL_DATA_TYPE_UINT16:
        case BCM_QUAL_DATA_TYPE_VLAN:
        case BCM_QUAL_DATA_TYPE_VPN:
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if ((qual_info->enum_name == NULL) &&
                    (qual_info->enum_value >
                    (BCMINT_FIELD_MAX_WIDTH_VALUE(width)))) {
                    return SHR_E_PARAM;
                }
            } else {
                if ((*((uint16_t *)qual_info->qual_data)) >
                    (BCMINT_FIELD_MAX_WIDTH_VALUE(width))) {
                    return SHR_E_PARAM;
                }
            }
            break;
        case BCM_QUAL_DATA_TYPE_UINT32:
        case BCM_QUAL_DATA_TYPE_IP:
        case BCM_QUAL_DATA_TYPE_NAT_ID:
        case BCM_QUAL_DATA_TYPE_MPLS_LABEL:
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if ((qual_info->enum_name != NULL) &&
                    (qual_info->enum_value >
                    (BCMINT_FIELD_MAX_WIDTH_VALUE(width)))) {
                    return SHR_E_PARAM;
                }
            } else {
                if ((*((uint32_t *)qual_info->qual_data)) >
                    (BCMINT_FIELD_MAX_WIDTH_VALUE(width))) {
                    return SHR_E_PARAM;
                }
            }
            break;
        case BCM_QUAL_DATA_TYPE_UINT64:
            if ((*((uint *)qual_info->qual_data)) >
                    (BCMINT_FIELD_MAX_WIDTH_VALUE(width))) {
                return SHR_E_PARAM;
            }
            break;
        case BCM_QUAL_DATA_TYPE_CLASS:
            /* There are no qualifiers using these fields
             * returning error so that validation can be
             * when its used
             */
            return (SHR_E_PARAM);
        case BCM_QUAL_DATA_TYPE_MAC:
        case BCM_QUAL_DATA_TYPE_IP6:
        case BCM_QUAL_DATA_TYPE_PBMP:
        case BCM_QUAL_DATA_TYPE_CUSTOM:
        case BCM_QUAL_DATA_TYPE_LLC:
        case BCM_QUAL_DATA_TYPE_SNAP:
            /* The validation is done in the input function itself*/
            break;
        default:
             return (SHR_E_UNAVAIL);
    }
    return (SHR_E_NONE);
}

/*
 * Function: bcmint_field_gport_port_resolve
 *
 * Purpose:
 *     Resolves gport into port, module, trunk.
 *
 * Parameters:
 *     unit     - (IN) BCM device number
 *     gport    - (IN) GPORT to resolve
 *     out_p    - (OUT) port encoded into gport.
 *     out_m    - (OUT) module encoded into gport.
 *     out_t    - (OUT) trunk encoded into gport.
 *
 * Returns:
 *     SHR_E_NONE      - No errors.
 *     SHR_E_XXX       - Otherwise.
 */

int
bcmint_field_gport_port_resolve(int unit, bcm_gport_t gport,
                                int *out_p, int *out_m, int *out_t)
{
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     tgid = BCM_TRUNK_INVALID;
    int             id;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmi_ltsw_port_gport_resolve(unit, gport, &modid, &port, &tgid, &id));

    if (BCM_TRUNK_INVALID != tgid && NULL == out_t) {
        return BCM_E_PORT;
    }

    if (NULL != out_t) {
        *out_t = tgid;
    }
    if (NULL != out_m) {
        *out_m = modid;
    }
    if (NULL != out_p) {
        *out_p = port;
    }

exit:
    SHR_FUNC_EXIT();
}


STATIC int
ltsw_field_qual_int_data_type_resolve(
    int unit,
    bcm_qual_field_t *qual_info,
    int *data,
    int *mask)
{
    SHR_FUNC_ENTER(unit);

    *data = *(int *)qual_info->qual_data;
    *mask = *(int *)qual_info->qual_mask;

    if ((qual_info->data_type == BCM_QUAL_DATA_TYPE_PORT) &&
        BCM_GPORT_IS_SET(*data)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_gport_port_resolve(unit, *data,
                                             data, NULL, NULL));
        if (*data == -1) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_entry_compression_set(int unit,
                                 bcmint_field_stage_info_t *stage_info,
                                 bcmi_field_ha_entry_oper_t *e_oper,
                                 bcm_field_qualify_t qual,
                                 bcmlt_entry_handle_t rule_template)
{
    bcm_field_qset_t qset;

    SHR_FUNC_ENTER(unit);

    if (!(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_SUPPORTED)) {
        SHR_EXIT();
    }

    if ((qual != bcmFieldQualifySrcIp) &&
        (qual != bcmFieldQualifyDstIp) &&
        (qual != bcmFieldQualifySrcIp6) &&
        (qual != bcmFieldQualifyDstIp6)) {
        SHR_EXIT();
    }

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, qual);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_compression_set(unit, stage_info, e_oper,
                                         &qset, 0x0,
                                         rule_template));
    if (e_oper) {
        if (qual == bcmFieldQualifySrcIp) {
            e_oper->entry_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT;
        } else if (qual == bcmFieldQualifySrcIp6) {
            e_oper->entry_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT;
        } else if (qual == bcmFieldQualifyDstIp) {
            e_oper->entry_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT;
        } else {
            e_oper->entry_flags |= BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_entry_compression_del(int unit,
                                 bcmint_field_stage_info_t *stage_info,
                                 bcm_field_qualify_t qual,
                                 bcmlt_entry_handle_t rule_template)
{
    bcm_field_qset_t qset;

    SHR_FUNC_ENTER(unit);

    if ((qual != bcmFieldQualifySrcIp) &&
        (qual != bcmFieldQualifyDstIp) &&
        (qual != bcmFieldQualifySrcIp6) &&
        (qual != bcmFieldQualifyDstIp6)) {
        SHR_EXIT();
    }

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, qual);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_compression_del(unit,
                                         stage_info,
                                         &qset,
                                         rule_template));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_qual_internal_map_set(int unit,
                                bool group_map,
                                uint8_t set_int_val,
                                bcm_field_qualify_t qual,
                                bcmlt_entry_handle_t qual_template_hdl,
                                bcmlt_entry_handle_t qual_template_lkup_hdl,
                                bcmint_field_stage_info_t *stage_info,
                                const bcm_field_qual_map_info_t *map,
                                uint32_t start_idx,
                                uint32_t num_elem)
{
    int rv = 0;
    uint32_t int_value = 0, elem = 0;
    uint64_t *qual_value_inp_buf = NULL;
    uint16_t idx = 0, iter = 0, src_ofst_int = 0;
    char *qual_int_field = NULL, *qual_template_name = NULL;
    bcmlt_field_def_t *fld_defs_ref = NULL, *fld_defs_iter = NULL;
    const bcm_field_qual_internal_map_info_t *lt_map_int_ref = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    /* Set internal map for a qual map if present. */
    if (map->num_internal_maps > 0) {

        /* Retreive qual template name from stage. */
        if (group_map)  {
            qual_template_name = stage_info->tbls_info->group_sid;
        } else {

            qual_template_name = stage_info->tbls_info->rule_sid;
        }
        /* Allocate memory for all qual ltmap fields. */
        BCMINT_FIELD_MEM_ALLOC
            (fld_defs_ref,
                sizeof(bcmlt_field_def_t) * map->num_internal_maps,
                "internal qual map field definitions");
        fld_defs_iter = fld_defs_ref;

        /* Retreive internal map first. */
        lt_map_int_ref = map->internal_map;
        if (lt_map_int_ref != NULL) {

            /* Retreive field definition for provided lt field in map */
            for (iter = 0; iter < map->num_internal_maps; iter++) {

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit, qual_template_name,
                      (lt_map_int_ref[iter]).lt_field_name,
                      &(fld_defs_iter[iter])));

                /* Validate field definitions for valid values */
                if (0 == fld_defs_iter[iter].elements) {
                    /* Not handling fields of array type or symbols for
                     * internal map, can be added later in case of need. */
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
            }

            src_ofst_int = 0;
            fld_defs_iter = fld_defs_ref;
            for (iter = 0; iter < map->num_internal_maps; iter++) {

                BCMINT_FIELD_MEM_ALLOC
                    (qual_value_inp_buf, sizeof(uint64_t) * fld_defs_iter[iter].elements,
                     "qual value");
                BCMINT_FIELD_MEM_ALLOC(qual_int_field,
                        (strlen(lt_map_int_ref[iter].lt_field_name) + 1),
                        "qual internal field");
                sal_strcpy(qual_int_field, lt_map_int_ref[iter].lt_field_name);

                /* Retreive previously set qualifier & mask value */
                if (fld_defs_iter[iter].symbol) {
                    /*Do nothing */
                } else if (fld_defs_iter->depth > 1) {
                    rv = bcmlt_entry_field_array_get(qual_template_lkup_hdl,
                                                     qual_int_field,
                                                     start_idx,
                                                     qual_value_inp_buf,
                                                     num_elem, &elem);
                    if ( SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
                        SHR_IF_ERR_VERBOSE_EXIT(rv);
                    }

                    if (set_int_val) {
                        int_value = ((uintptr_t)(lt_map_int_ref[iter].value));
                    }
                } else {
                    rv = bcmlt_entry_field_get(qual_template_lkup_hdl,
                                               qual_int_field,
                                               qual_value_inp_buf);
                    if ( SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
                        SHR_IF_ERR_VERBOSE_EXIT(rv);
                    }
                    if (set_int_val) {
                        int_value = ((uintptr_t)(lt_map_int_ref[iter].value));
                    }
                }

                if ((0 == lt_map_int_ref[iter].num_offsets) &&
                    (0 == lt_map_int_ref[iter].width[0])) {
                    /* qual takes full field width */
                    SHR_BITCOPY_RANGE((uint32_t *)qual_value_inp_buf, 0,
                      &int_value, 0, fld_defs_iter[iter].width);
                } else {
                    /* Set qual in o/p as per offset & widths specified in map */
                    for (idx = 0; idx < lt_map_int_ref[iter].num_offsets; idx++) {
                        SHR_BITCOPY_RANGE((uint32_t *)qual_value_inp_buf,
                                lt_map_int_ref[iter].offset[idx],
                                &int_value, src_ofst_int,
                                lt_map_int_ref[iter].width[idx]);
                        src_ofst_int +=  lt_map_int_ref[iter].width[idx];
                    }
                }

            if (fld_defs_iter[iter].symbol) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(qual_template_hdl,
                                                  qual_int_field,
                                                  (const char *)lt_map_int_ref[iter].value));
            } else if (fld_defs_iter[iter].depth > 1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(qual_template_hdl,
                                                 qual_int_field,
                                                 start_idx,
                                                 qual_value_inp_buf,
                                                 num_elem));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(qual_template_hdl,
                                               qual_int_field,
                                               *qual_value_inp_buf));
                }
                BCMINT_FIELD_MEM_FREE(qual_value_inp_buf);
                BCMINT_FIELD_MEM_FREE(qual_int_field);
            }
        }
    }

exit:
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    BCMINT_FIELD_MEM_FREE(qual_value_inp_buf);
    BCMINT_FIELD_MEM_FREE(qual_int_field);
    SHR_FUNC_EXIT();
}
/*!
 * \brief bcmint_field_qualifier_set
 *
 * Set qualifier values for provided entry
 * \param [in] unit      - BCM device number.
 * \param [in] entry     - Entry identifier.
 * \param [in] qual_info - Qualfier information.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided entry does not exist in system
 *     SHR_E_UNAVAIL   - provided qualifier is not supported in system
 *     SHR_E_PARAM     - provided qualifier value is not valid
 *     SHR_E_NONE      - Success
 */
int
bcmi_ltsw_field_qualifier_set(int unit,
                              bcm_field_entry_t entry,
                              bcm_qual_field_t *qual_info)
{
    bool found;
    shr_error_t rv = SHR_E_NONE;
    int dunit = 0, idx = 0, iter = 0;
    uint8_t qual_value_inp_buf_size = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    uint32_t num_element = 0, src_ofset_current = 0;
    int map_count = 0, entry_id = 0, data = 0, mask = 0;
    uint16_t lt_map_qual_width = 0, lt_map_field_width = 0;
    bcmlt_entry_handle_t rule_template_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t lookup_rule_template_hdl = BCMLT_INVALID_HDL;
    uint8_t *qual_value_inp_buf = NULL, *qual_mask_inp_buf = NULL;
    uint64_t *qual_value_op_buf = NULL, *qual_mask_op_buf = NULL;
    bcmlt_field_def_t *fld_defs_iter = NULL, *fld_defs_ref = NULL;
    char* rule_template_name = NULL, *rule_template_id_name = NULL;
    char *qual_field = NULL, *lt_sym_str_op = NULL, *qual_mask_field = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bool qual_found = FALSE;
    uint32_t tmp_qual_val_inp_buf = 0;
    uint32_t ref_data[2] = { 0 };
    uint32_t ref_mask[2] = { 0 };
    bcm_field_llc_header_t llc_data;
    bcm_field_llc_header_t llc_mask;
    bcm_field_snap_header_t snap_data;
    bcm_field_snap_header_t snap_mask;
    bcm_field_qset_t presel_qset;
    int presel_grp_id = 0;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    sal_memset(&llc_data, 0, sizeof(bcm_field_llc_header_t));
    sal_memset(&llc_mask, 0, sizeof(bcm_field_llc_header_t));
    sal_memset(&snap_data, 0, sizeof(bcm_field_snap_header_t));
    sal_memset(&snap_mask, 0, sizeof(bcm_field_snap_header_t));

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Find lt_map for provided qualifier from db */
    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_presel_oper_lookup(unit, entry_id,
                                             stage_info,
                                             &presel_oper));
        presel_grp_id = presel_oper->group_id;
        /* Verify with qualifiers in presel grp. */
        if (presel_grp_id != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_presel_qset_get(unit,
                                          stage_info->stage_id,
                                          presel_grp_id,
                                          1,
                                          &presel_qset));
            if (!(BCM_FIELD_QSET_TEST(presel_qset,
                                qual_info->sdk6_qual_enum))) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit, "Error: qualifier not part of"
                  " presel group qset.\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
        }

        rule_template_name = stage_info->tbls_info->presel_entry_sid;
        rule_template_id_name = stage_info->tbls_info->presel_entry_key_fid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_qual_map_find(unit, qual_info->sdk6_qual_enum,
                                              stage_info, 1, 0, &map_count,
                                              &lt_map_ref));
    } else {
        /* check if qual is part of group qset */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_oper_lookup(unit, entry_id,
                                            stage_info,
                                            &entry_oper));
        if (entry_oper->entry_flags &
            BCMINT_FIELD_ENTRY_DEFAULT_ONLY) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }

        /* Retreive group hash entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_oper_lookup(unit,
                                            entry_oper->group_id,
                                            stage_info,
                                            &group_oper));

        FP_HA_GROUP_OPER_QSET_TEST(group_oper,
                                   qual_info->sdk6_qual_enum,
                                   found);
        if (found == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        rule_template_name = stage_info->tbls_info->rule_sid;
        rule_template_id_name = stage_info->tbls_info->rule_key_fid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_qual_map_find(unit, qual_info->sdk6_qual_enum,
                                              stage_info, 0, 0, &map_count,
                                             &lt_map_ref));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qual_value_set(unit, entry, qual_info));

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t) * (lt_map_ref->num_maps),
                                                    "field definitions");
    fld_defs_iter = fld_defs_ref;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if (FALSE == bcmint_field_map_supported(unit, stage_info,
                     (lt_map_ref)->map[iter])) {
            fld_defs_iter++;
            continue;
        }

        if ((map_count == 1) &&
            (lt_map_ref->map[iter].width[0] == 0)) {

            /* No need to check offsets and widths, these values means
             * qualifier is taking full lt field width */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, rule_template_name,
                   (lt_map_ref->map[iter]).lt_field_name, fld_defs_iter));

            /* Validate field definitions for valid values */
            if (0 == fld_defs_iter->elements) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            lt_map_qual_width += fld_defs_iter->width;

        } else {
            lt_map_field_width = 0;

            /* Retreive field definition for provided lt field in map */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, rule_template_name,
                   (lt_map_ref->map[iter]).lt_field_name, fld_defs_iter));

            /* Validate field definitions for valid values */
            if (0 == fld_defs_iter->elements) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            for(idx = 0; idx < (lt_map_ref->map[iter]).num_offsets; idx++) {
                if ((lt_map_ref->map[iter]).width[idx] == 0) {
                    /* Allow any one lt map with width 0, which means
                     * qualifier mapping to full lt field, other cases
                     * user is supposed to update offset & width in db */
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }

                if ((((lt_map_ref->map[iter]).offset[idx]) +
                     ((lt_map_ref->map[iter]).width[idx])) >
                      (fld_defs_iter->width)) {
                    /* No individual offset/width provied should
                     * exceed width of field in which they are going int*/
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);

                }
                lt_map_field_width += (lt_map_ref->map[iter]).width[idx];
            }
            lt_map_qual_width += lt_map_field_width;

            /* Return error if num of elements in field definitions is not
             * valid & req value exceeds max allowed field value */
            if(lt_map_field_width > fld_defs_iter->width) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }
            fld_defs_iter++;
        }
    }

    rv = bcmint_field_qualifier_validate(unit, lt_map_qual_width, qual_info);
    if (SHR_FAILURE(rv)) {
        LOG_DEBUG(BSL_LS_BCM_FP,
          (BSL_META_U(unit, "Error: qualifier data/mask width"
          " exceeds maximum.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Entry handle allocate for rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              rule_template_name,
                              &lookup_rule_template_hdl));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(lookup_rule_template_hdl,
                               rule_template_id_name,
                               entry_id));

    /* Lookup for the rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, lookup_rule_template_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Entry handle allocate for rule Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              rule_template_name,
                              &rule_template_hdl));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(rule_template_hdl,
                               rule_template_id_name,
                               entry_id));

    
    /* Allocate internal buffer mem and copy data */
    switch(qual_info->data_type) {
        case BCM_QUAL_DATA_TYPE_INT:
        case BCM_QUAL_DATA_TYPE_TRUNK:
        case BCM_QUAL_DATA_TYPE_IF:
        case BCM_QUAL_DATA_TYPE_L4_PORT:
        case BCM_QUAL_DATA_TYPE_VRF:
        case BCM_QUAL_DATA_TYPE_COS:
        case BCM_QUAL_DATA_TYPE_MULTICAST:
        case BCM_QUAL_DATA_TYPE_IF_GROUP:
        case BCM_QUAL_DATA_TYPE_TRILL_NAME:
        case BCM_QUAL_DATA_TYPE_GPORT:
        case BCM_QUAL_DATA_TYPE_PORT:
        case BCM_QUAL_DATA_TYPE_STG:
            qual_value_inp_buf_size = sizeof(int);
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "ip buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "ip mask buf");
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_field_qual_int_data_type_resolve(
                                               unit,
                                               qual_info,
                                               &data,
                                               &mask));
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if (qual_info->enum_name != NULL) {
                    BCMINT_FIELD_MEM_ALLOC
                        (lt_sym_str_op,
                        (strlen(qual_info->enum_name) + 1), "lt sym buf");
                     sal_strcpy(lt_sym_str_op, qual_info->enum_name);
                } else {
                    sal_memcpy(qual_value_inp_buf,
                              &qual_info->enum_value, qual_value_inp_buf_size);
                    sal_memcpy(qual_mask_inp_buf,
                              &qual_info->enum_mask, qual_value_inp_buf_size);
                }
            } else {
                sal_memcpy(qual_value_inp_buf, &data,
                           qual_value_inp_buf_size);
                sal_memcpy(qual_mask_inp_buf, &mask,
                           qual_value_inp_buf_size);
            }
            break;

        case BCM_QUAL_DATA_TYPE_UINT8:
            qual_value_inp_buf_size = sizeof(uint8_t);
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "ip buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "ip mask buf");
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if (qual_info->enum_name != NULL) {
                    BCMINT_FIELD_MEM_ALLOC
                        (lt_sym_str_op,
                        (strlen(qual_info->enum_name) + 1), "lt sym buf");
                     sal_strcpy(lt_sym_str_op, qual_info->enum_name);
                } else {
                    sal_memcpy(qual_value_inp_buf,
                              &qual_info->enum_value, qual_value_inp_buf_size);
                    sal_memcpy(qual_mask_inp_buf,
                              &qual_info->enum_mask, qual_value_inp_buf_size);
                }
            } else {
                sal_memcpy(qual_value_inp_buf,
                    (uint8_t *)qual_info->qual_data, qual_value_inp_buf_size);
                sal_memcpy(qual_mask_inp_buf,
                    (uint8_t *)qual_info->qual_mask, qual_value_inp_buf_size);
            }
            break;

        case BCM_QUAL_DATA_TYPE_UINT16:
        case BCM_QUAL_DATA_TYPE_VLAN:
        case BCM_QUAL_DATA_TYPE_VPN:
            qual_value_inp_buf_size = sizeof(uint16_t);
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "ip buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "ip mask buf");
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if (qual_info->enum_name != NULL) {
                    BCMINT_FIELD_MEM_ALLOC
                        (lt_sym_str_op,
                        (strlen(qual_info->enum_name) + 1), "lt sym buf");
                     sal_strcpy(lt_sym_str_op, qual_info->enum_name);
                } else {
                    sal_memcpy(qual_value_inp_buf,
                              &qual_info->enum_value, qual_value_inp_buf_size);
                    sal_memcpy(qual_mask_inp_buf,
                              &qual_info->enum_mask, qual_value_inp_buf_size);
                }
            } else {
                sal_memcpy(qual_value_inp_buf,
                    (uint16_t *)qual_info->qual_data, qual_value_inp_buf_size);
                sal_memcpy(qual_mask_inp_buf,
                    (uint16_t *)qual_info->qual_mask, qual_value_inp_buf_size);
            }
            break;

        case BCM_QUAL_DATA_TYPE_UINT32:
        case BCM_QUAL_DATA_TYPE_IP:
        case BCM_QUAL_DATA_TYPE_NAT_ID:
        case BCM_QUAL_DATA_TYPE_MPLS_LABEL:
            qual_value_inp_buf_size = sizeof(uint32_t);
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "ip buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "ip mask buf");
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if (qual_info->enum_name != NULL) {
                    BCMINT_FIELD_MEM_ALLOC
                        (lt_sym_str_op,
                        (strlen(qual_info->enum_name) + 1), "lt sym buf");
                     sal_strcpy(lt_sym_str_op, qual_info->enum_name);
                } else {
                    sal_memcpy(qual_value_inp_buf,
                              &qual_info->enum_value, qual_value_inp_buf_size);
                    sal_memcpy(qual_mask_inp_buf,
                              &qual_info->enum_mask, qual_value_inp_buf_size);
                }
            } else {
                sal_memcpy(qual_value_inp_buf,
                      (uint32_t *)qual_info->qual_data, qual_value_inp_buf_size);
                sal_memcpy(qual_mask_inp_buf,
                      (uint32_t *)qual_info->qual_mask, qual_value_inp_buf_size);
            }
            tmp_qual_val_inp_buf = *qual_value_inp_buf;
            break;

        case BCM_QUAL_DATA_TYPE_UINT64:
        case BCM_QUAL_DATA_TYPE_CLASS:
            qual_value_inp_buf_size = sizeof(uint64_t);
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "ip buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "ip mask buf");
            sal_memcpy(qual_value_inp_buf,
                (uint64_t *)qual_info->qual_data, qual_value_inp_buf_size);
            sal_memcpy(qual_mask_inp_buf,
                (uint64_t *)qual_info->qual_mask, qual_value_inp_buf_size);
            break;

        case BCM_QUAL_DATA_TYPE_MAC:
            qual_value_inp_buf_size = sizeof(uint8_t) * 6;
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "ip buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "ip mask buf");
            sal_memcpy(qual_value_inp_buf, *(uint8_t **)qual_info->qual_data, qual_value_inp_buf_size);
            sal_memcpy(qual_mask_inp_buf, *(uint8_t **)qual_info->qual_mask, qual_value_inp_buf_size);
            break;

         case BCM_QUAL_DATA_TYPE_IP6:
            qual_value_inp_buf_size = sizeof(uint8_t) * 16;
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "ip buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "ip mask buf");
            sal_memcpy(qual_value_inp_buf, *(uint8_t **)qual_info->qual_data, qual_value_inp_buf_size);
            sal_memcpy(qual_mask_inp_buf, *(uint8_t **)qual_info->qual_mask, qual_value_inp_buf_size);
            break;
        case BCM_QUAL_DATA_TYPE_LLC:
            qual_value_inp_buf_size = sizeof(bcm_field_llc_header_t);
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "Llc buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "Llc mask buf");
            sal_memcpy(&llc_data, (bcm_field_llc_header_t *)qual_info->qual_data, qual_value_inp_buf_size);
            sal_memcpy(&llc_mask, (bcm_field_llc_header_t *)qual_info->qual_mask, qual_value_inp_buf_size);

            ref_data[0] = (llc_data.dsap << 16) | (llc_data.ssap << 8) | llc_data.control;
            ref_mask[0] = (llc_mask.dsap << 16) | (llc_mask.ssap << 8) | llc_mask.control;

            sal_memcpy(qual_value_inp_buf, ref_data, qual_value_inp_buf_size);
            sal_memcpy(qual_mask_inp_buf, ref_mask, qual_value_inp_buf_size);
            break;
        case BCM_QUAL_DATA_TYPE_SNAP:
            qual_value_inp_buf_size = sizeof(bcm_field_snap_header_t);
            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "Snap buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "Snap mask buf");
            sal_memcpy(&snap_data, (bcm_field_snap_header_t *)qual_info->qual_data, qual_value_inp_buf_size);
            sal_memcpy(&snap_mask, (bcm_field_snap_header_t *)qual_info->qual_mask, qual_value_inp_buf_size);

            ref_data[1] = (snap_data.org_code & 0x00ff0000) >> 16;
            ref_data[0] = (snap_data.org_code & 0x0000ffff) << 16 | snap_data.type;

            ref_mask[1] = (snap_mask.org_code & 0x00ff0000) >> 16;
            ref_mask[0] = (snap_mask.org_code & 0x0000ffff) << 16 | snap_mask.type;

            sal_memcpy(qual_value_inp_buf, ref_data, qual_value_inp_buf_size);
            sal_memcpy(qual_mask_inp_buf, ref_mask, qual_value_inp_buf_size);
            break;
        case BCM_QUAL_DATA_TYPE_PBMP:
            qual_value_inp_buf_size = sizeof(bcm_pbmp_t);

            BCMINT_FIELD_MEM_ALLOC
                (qual_value_inp_buf, qual_value_inp_buf_size, "ip buf");
            BCMINT_FIELD_MEM_ALLOC
                (qual_mask_inp_buf, qual_value_inp_buf_size, "ip mask buf");

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_pbmp_index_get(unit,
                                          group_oper->group_id,
                                          stage_info,
                                         (bcm_pbmp_t *)qual_info->qual_data,
                                         (bcm_pbmp_t *)qual_info->qual_mask,
                                          qual_value_inp_buf,
                                          qual_mask_inp_buf));
            break;

        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Total qualifier width cannot be greater than allocated
     * buffer size */
    if (!(qual_info->flags & BCM_FIELD_F_BMP))  {
        if (lt_map_qual_width > (8 * qual_value_inp_buf_size)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit, "Error: Qualifier offset width"
                                " exceeds maximum, check db.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    }

    /*
     * Sequence to set a value in a lt map is:
     *  Go though all lt maps for a qualifier
     *  - get current value set for a lt field
     *  - set new incoming value
     *    - loop through all offsets/width and set in field
     */
    src_ofset_current = 0;
    fld_defs_iter = fld_defs_ref;

    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if (FALSE == bcmint_field_map_supported(unit, stage_info,
                      (lt_map_ref)->map[iter])) {
            continue;
        }

        BCMINT_FIELD_MEM_ALLOC
            (qual_value_op_buf, sizeof(uint64_t) * fld_defs_iter[iter].elements,
             "qualifier value");
        BCMINT_FIELD_MEM_ALLOC
            (qual_mask_op_buf, sizeof(uint64_t) * fld_defs_iter[iter].elements,
             "qualifier mask value");
        BCMINT_FIELD_MEM_ALLOC
            (qual_field, (strlen((lt_map_ref->map[iter]).lt_field_name) + 1),
             "qualifier field");
        BCMINT_FIELD_MEM_ALLOC
          (qual_mask_field, (strlen((lt_map_ref->map[iter]).lt_field_name) + 6),
          "qualifier mask field");

        sal_strcpy(qual_field, (lt_map_ref->map[iter]).lt_field_name);
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_qual_mask_lt_field_get(unit, qual_field, &qual_mask_field));
        num_element = fld_defs_iter[iter].elements;

        if (fld_defs_iter[iter].elements > 1) {

            if (fld_defs_iter[iter].symbol) {
                /* Not handled till now as no such case is present
                 * in db till date */
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);

            } else {

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(lookup_rule_template_hdl,
                                                 qual_field,
                                                 0, qual_value_op_buf,
                                                 num_element, &num_element));
                if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_get(lookup_rule_template_hdl,
                                                 qual_mask_field,
                                                 0, qual_mask_op_buf,
                                                 num_element, &num_element));
                }

                /* Set values in input buffer as per offset & width specified */
                bcmint_entry_field_qual_buffer_set(unit,
                                                 &(lt_map_ref->map[iter]),
                                                 &fld_defs_iter[iter],
                                                 qual_info,
                                                 &src_ofset_current,
                                                  (uint32_t *)qual_value_inp_buf,
                                                  (uint32_t *)qual_mask_inp_buf,
                                                  (uint32_t *)qual_value_op_buf,
                                                  (uint32_t *)qual_mask_op_buf);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(rule_template_hdl,
                                                 qual_field,
                                                 0, qual_value_op_buf,
                                                 num_element));
                if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_add(rule_template_hdl,
                                                 qual_mask_field,
                                                 0, qual_mask_op_buf,
                                                 num_element));
                }
            }

        } else {

            if (fld_defs_iter[iter].symbol) {
                if (lt_sym_str_op != NULL) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_symbol_add(rule_template_hdl,
                                                      qual_field,
                                                      lt_sym_str_op));
                } else {
                    /* if field is of symbol type in sdklt
                     * then we should fill equivalent
                     * symbol string in auto-gen db code */
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
            } else {

                if (lt_sym_str_op != NULL) {
                    /* if fied is not of symbol type in sdklt
                     * then we should not fill symbol string from
                     * auto-gen db code */
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }

                if ((lt_map_ref)->data_type == bcmFieldDataTypeEnumtoLtField) {
                    /* Only one of the mapped LT names are valid. check
                     * the enum value and insert only that LT field.
                     */
                    if (tmp_qual_val_inp_buf == (lt_map_ref->map[iter]).value) {
                        *qual_value_inp_buf = 1;
                        *qual_mask_inp_buf = 1;
                    } else {
                        *qual_value_inp_buf = 0;
                        *qual_mask_inp_buf = 0;
                    }
                    src_ofset_current = 0;
                } else if ((lt_map_ref)->data_type == bcmFieldDataTypeMacrotoLtField) {
                    /* More than one of the mapped LT names are valid. check
                     * the Macro value and get only that LT field.
                     */
                    if (!qual_found) {
                         tmp_qual_val_inp_buf = *qual_value_inp_buf;
                         qual_found = TRUE;
                    }

                    if ((tmp_qual_val_inp_buf & (lt_map_ref->map[iter]).value)) {
                        *qual_value_inp_buf = 1;
                        *qual_mask_inp_buf = 1;
                    } else if (tmp_qual_val_inp_buf == 0) {
                        *qual_value_inp_buf = 0;
                        *qual_mask_inp_buf = 1;
                    } else {
                        *qual_value_inp_buf = 0;
                        *qual_mask_inp_buf = 0;
                    }
                    src_ofset_current = 0;
                }

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(lookup_rule_template_hdl,
                                           qual_field,
                                           qual_value_op_buf));
                if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(lookup_rule_template_hdl,
                                           qual_mask_field,
                                           qual_mask_op_buf));
                }

                /* Set values in input buffer as per offset & width specified */
                bcmint_entry_field_qual_buffer_set(unit,
                                                 &(lt_map_ref->map[iter]),
                                                 &fld_defs_iter[iter],
                                                 qual_info,
                                                 &src_ofset_current,
                                                 (uint32_t *)qual_value_inp_buf,
                                                 (uint32_t *)qual_mask_inp_buf,
                                                 (uint32_t *)qual_value_op_buf,
                                                 (uint32_t *)qual_mask_op_buf);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(rule_template_hdl,
                                           qual_field,
                                           *qual_value_op_buf));

                if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(rule_template_hdl,
                                           qual_mask_field,
                                           *qual_mask_op_buf));
                }
            }
        }

        BCMINT_FIELD_MEM_FREE(qual_field);
        BCMINT_FIELD_MEM_FREE(qual_mask_field);
        BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
        BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
    }

    if ((BCMINT_FIELD_IS_PRESEL_ENTRY(entry) == 0) &&
        (BCMINT_FIELD_IS_COMPRESSION_QUAL(qual_info->sdk6_qual_enum)) &&
        (stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_SUPPORTED) &&
        (group_oper->group_flags & BCMINT_FIELD_GROUP_COMPRESSION_ENABLED)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_entry_compression_set(unit, stage_info,
                                              entry_oper,
                                              qual_info->sdk6_qual_enum,
                                              rule_template_hdl));
    }

    /* Update rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(rule_template_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        /* Set internal maps associated if any. */
        if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_qual_internal_map_set(unit,
                                                    0,
                                                    1,
                                                    qual_info->sdk6_qual_enum,
                                                    rule_template_hdl,
                                                    lookup_rule_template_hdl,
                                                    stage_info,
                                                    &(lt_map_ref->map[iter]),
                                                    0, 0));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, rule_template_hdl,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL));
        }
    }


exit:
    if (BCMLT_INVALID_HDL != lookup_rule_template_hdl) {
        (void) bcmlt_entry_free(lookup_rule_template_hdl);
    }
    if (BCMLT_INVALID_HDL != rule_template_hdl) {
        (void) bcmlt_entry_free(rule_template_hdl);
    }
    BCMINT_FIELD_MEM_FREE(qual_field);
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    BCMINT_FIELD_MEM_FREE(lt_sym_str_op);
    BCMINT_FIELD_MEM_FREE(qual_mask_field);
    BCMINT_FIELD_MEM_FREE(qual_mask_inp_buf);
    BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
    BCMINT_FIELD_MEM_FREE(qual_value_inp_buf);
    BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmi_ltsw_field_qualifier_get
 *
 * Get qualifier values for provided entry
 * \param [in] unit      - BCM device number.
 * \param [in] entry     - Entry identifier.
 * \param [in/out] qual_info - Qualfier information.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided entry does not exist in system
 *     SHR_E_UNAVAIL   - provided qualifier is not supported in system
 *     SHR_E_PARAM     - provided qualifier value is not valid.
 *     SHR_E_NONE      - Success
 */
STATIC int
ltsw_field_qualifier_get(int unit,
                         bcm_field_entry_t entry,
                         bcm_qual_field_t *qual_info)
{
    bool found;
    int dunit = 0, iter = 0;
    int map_count = 0, entry_id = 0;
    uint8_t qual_value_inp_buf_size = 0;
    bcmlt_field_def_t *fld_defs_ref = NULL;
    bcmlt_field_def_t *fld_defs_iter = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    uint32_t num_element = 0, src_ofset_current = 0;
    bcmlt_entry_handle_t rule_template_hdl = BCMLT_INVALID_HDL;
    uint64_t *qual_value_op_buf = NULL, *qual_mask_op_buf = NULL;
    uint8_t *qual_value_inp_buf = NULL, *qual_mask_inp_buf = NULL;
    char* rule_template_name = NULL, *rule_template_id_name = NULL;
    char *qual_field = NULL, *lt_sym_str_op = NULL, *qual_mask_field = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    uint32_t tmp_qual_value_inp_buf = 0;
    uint32_t ref_data[2] = { 0 };
    uint32_t ref_mask[2] = { 0 };
    bcm_field_llc_header_t llc_data;
    bcm_field_llc_header_t llc_mask;
    bcm_field_snap_header_t snap_data;
    bcm_field_snap_header_t snap_mask;

    SHR_FUNC_ENTER(unit);

    sal_memset(&llc_data, 0, sizeof(bcm_field_llc_header_t));
    sal_memset(&llc_mask, 0, sizeof(bcm_field_llc_header_t));
    sal_memset(&snap_data, 0, sizeof(bcm_field_snap_header_t));
    sal_memset(&snap_mask, 0, sizeof(bcm_field_snap_header_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qual_info_set(unit, entry, qual_info));

    /* Validate NULL parameters */
    if ((qual_info->flags & BCM_FIELD_F_ENUM) == 0) {
        if (((*(void **)qual_info->qual_data == NULL) ||
            (*(void **)qual_info->qual_mask == NULL)) &&
            (qual_info->enum_name == NULL)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Find lt_map for provided qualifier from db */
    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
        rule_template_name = stage_info->tbls_info->presel_entry_sid;
        rule_template_id_name = stage_info->tbls_info->presel_entry_key_fid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_qual_map_find(unit, qual_info->sdk6_qual_enum,
                                        stage_info, 1, 0, &map_count,
                                        &lt_map_ref));
    } else {
        /* check if qual is part of group qset */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_oper_lookup(unit, entry_id,
                                            stage_info,
                                            &entry_oper));
        /* Retreive group hash entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_oper_lookup(unit,
                                            entry_oper->group_id,
                                            stage_info,
                                            &group_oper));
        FP_HA_GROUP_OPER_QSET_TEST(group_oper,
                                   qual_info->sdk6_qual_enum,
                                   found);
        if (found == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }

        rule_template_name = stage_info->tbls_info->rule_sid;
        rule_template_id_name = stage_info->tbls_info->rule_key_fid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_qual_map_find(unit, qual_info->sdk6_qual_enum,
                                        stage_info, 0, 0, &map_count,
                                        &lt_map_ref));
    }

    /* Check all lt maps width is within max allowed range */
    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t) * lt_map_ref->num_maps,
         "field definitions");
    fld_defs_iter = fld_defs_ref;

    /* Iterate through all lt maps */
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if (FALSE == bcmint_field_map_supported(unit, stage_info,
                     (lt_map_ref)->map[iter])) {
            fld_defs_iter++;
            continue;
        }
        /* Retreive field definition for provided lt field in map */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, rule_template_name,
            (lt_map_ref->map[iter]).lt_field_name, fld_defs_iter));

        if (0 == fld_defs_iter->elements) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        fld_defs_iter++;
    }

    /* Entry handle allocate for rule Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              rule_template_name,
                              &rule_template_hdl));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(rule_template_hdl,
                               rule_template_id_name,
                               entry_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, rule_template_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Allocate internal buffer mem and copy data */
    switch(qual_info->data_type) {
        case BCM_QUAL_DATA_TYPE_INT:
        case BCM_QUAL_DATA_TYPE_TRUNK:
        case BCM_QUAL_DATA_TYPE_IF:
        case BCM_QUAL_DATA_TYPE_L4_PORT:
        case BCM_QUAL_DATA_TYPE_VRF:
        case BCM_QUAL_DATA_TYPE_COS:
        case BCM_QUAL_DATA_TYPE_MULTICAST:
        case BCM_QUAL_DATA_TYPE_IF_GROUP:
        case BCM_QUAL_DATA_TYPE_TRILL_NAME:
        case BCM_QUAL_DATA_TYPE_GPORT:
        case BCM_QUAL_DATA_TYPE_PORT:
        case BCM_QUAL_DATA_TYPE_STG:
            qual_value_inp_buf_size = sizeof(int);
            break;
        case BCM_QUAL_DATA_TYPE_UINT8:
            qual_value_inp_buf_size = sizeof(uint8_t);
            break;
        case BCM_QUAL_DATA_TYPE_UINT16:
        case BCM_QUAL_DATA_TYPE_VLAN:
        case BCM_QUAL_DATA_TYPE_VPN:
            qual_value_inp_buf_size = sizeof(uint16_t);
            break;
        case BCM_QUAL_DATA_TYPE_UINT32:
        case BCM_QUAL_DATA_TYPE_IP:
        case BCM_QUAL_DATA_TYPE_NAT_ID:
        case BCM_QUAL_DATA_TYPE_MPLS_LABEL:
            qual_value_inp_buf_size = sizeof(uint32_t);
            break;
        case BCM_QUAL_DATA_TYPE_UINT64:
        case BCM_QUAL_DATA_TYPE_CLASS:
            qual_value_inp_buf_size = sizeof(uint64_t);
            break;
        case BCM_QUAL_DATA_TYPE_MAC:
            qual_value_inp_buf_size = sizeof(uint8_t) * 6;
            break;
        case BCM_QUAL_DATA_TYPE_IP6:
            qual_value_inp_buf_size = sizeof(uint8_t) * 16;
            break;
        case BCM_QUAL_DATA_TYPE_PBMP:
            qual_value_inp_buf_size = sizeof(bcm_pbmp_t);
            break;
        case BCM_QUAL_DATA_TYPE_LLC:
            qual_value_inp_buf_size = sizeof(bcm_field_llc_header_t);
            break;
        case BCM_QUAL_DATA_TYPE_SNAP:
            qual_value_inp_buf_size = sizeof(bcm_field_snap_header_t);
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Allocate input buffer */
    BCMINT_FIELD_MEM_ALLOC
        (qual_value_inp_buf, qual_value_inp_buf_size, "ip buf");
    BCMINT_FIELD_MEM_ALLOC
        (qual_mask_inp_buf, qual_value_inp_buf_size, "ip mask buf");

    /* Clear the input buffer. */
    if (!(qual_info->flags & BCM_FIELD_F_ENUM)) {
        sal_memset(*(uint32_t **)qual_info->qual_data, 0, qual_value_inp_buf_size);
    }

    /*
     * Sequence to get a value in a lt map is:
     *  Go though all lt maps for a qualifier
     *    - loop through all offsets/width and get field value
     */
    src_ofset_current = 0;
    fld_defs_iter = fld_defs_ref;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {

        if (FALSE == bcmint_field_map_supported(unit, stage_info,
                                        (lt_map_ref)->map[iter])) {
            continue;
        }

        BCMINT_FIELD_MEM_ALLOC
            (qual_value_op_buf, sizeof(uint64_t) * fld_defs_iter[iter].elements,
             "qualifier value");
        BCMINT_FIELD_MEM_ALLOC
            (qual_mask_op_buf, sizeof(uint64_t) * fld_defs_iter[iter].elements,
             "qualifier mask value");
        BCMINT_FIELD_MEM_ALLOC
            (qual_field, (strlen((lt_map_ref->map[iter]).lt_field_name) + 1),
             "qualifier field");
        BCMINT_FIELD_MEM_ALLOC
          (qual_mask_field, (strlen((lt_map_ref->map[iter]).lt_field_name) + 6),
             "qualifier mask field");

        sal_strcpy(qual_field, (lt_map_ref->map[iter]).lt_field_name);
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_qual_mask_lt_field_get(unit, qual_field, &qual_mask_field));
        num_element = fld_defs_iter[iter].elements;

        if (fld_defs_iter[iter].elements > 1) {

            if (fld_defs_iter[iter].symbol) {
                /* Not handled till now as no such case is present
                 *                  * in db till date */
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);

            } else {

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(rule_template_hdl,
                                                 qual_field,
                                                 0, qual_value_op_buf,
                                                 num_element, &num_element));
                if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_get(rule_template_hdl,
                                                 qual_mask_field,
                                                 0, qual_mask_op_buf,
                                                 num_element, &num_element));
                }

                /* Get values in input buffer as per offset & width specified */
                bcmint_entry_field_qual_buffer_get(unit,
                                                 &(lt_map_ref->map[iter]),
                                                 &fld_defs_iter[iter],
                                                  qual_info,
                                                 &src_ofset_current,
                                                 (uint32_t *)qual_value_inp_buf,
                                                 (uint32_t *)qual_mask_inp_buf,
                                                 (uint32_t *)qual_value_op_buf,
                                                 (uint32_t *)qual_mask_op_buf);
            }

        } else {

            if (fld_defs_iter[iter].symbol) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_get(rule_template_hdl,
                                                  qual_field,
                                      (const char **)&lt_sym_str_op));
                if (NULL == lt_sym_str_op) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }
            } else {

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(rule_template_hdl,
                                           qual_field,
                                           qual_value_op_buf));
                if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(rule_template_hdl,
                                           qual_mask_field,
                                           qual_mask_op_buf));
                }
                if ((lt_map_ref)->data_type == bcmFieldDataTypeEnumtoLtField) {

                    /* Only one of the mapped LT names are valid. check
                     * the enum value and get only that LT field.
                     */
                    if (*qual_value_op_buf == 1 && *qual_mask_op_buf == 1) {
                        *qual_value_inp_buf = (lt_map_ref->map[iter]).value;
                        *qual_mask_inp_buf = (lt_map_ref->map[iter]).value;
                        break;
                    } else {
                        BCMINT_FIELD_MEM_FREE(qual_field);
                        BCMINT_FIELD_MEM_FREE(qual_mask_field);
                        BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
                        BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
                        continue;
                    }
                } else if ((lt_map_ref)->data_type == bcmFieldDataTypeMacrotoLtField) {
                    /* More than one of the mapped LT names are valid. check
                     * the Macro value and get only that LT field.
                     */
                    if (*qual_value_op_buf == 1 && *qual_mask_op_buf == 1) {
                        tmp_qual_value_inp_buf |= (lt_map_ref->map[iter]).value;
                    }
                    BCMINT_FIELD_MEM_FREE(qual_field);
                    BCMINT_FIELD_MEM_FREE(qual_mask_field);
                    BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
                    BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
                    continue;
                }

                /* Set values in input buffer as per offset & width specified */
                bcmint_entry_field_qual_buffer_get(unit,
                                                 &(lt_map_ref->map[iter]),
                                                 &fld_defs_iter[iter],
                                                  qual_info,
                                                 &src_ofset_current,
                                                 (uint32_t *)qual_value_inp_buf,
                                                 (uint32_t *)qual_mask_inp_buf,
                                                 (uint32_t *)qual_value_op_buf,
                                                 (uint32_t *)qual_mask_op_buf);
            }
        }

        BCMINT_FIELD_MEM_FREE(qual_field);
        BCMINT_FIELD_MEM_FREE(qual_mask_field);
        BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
        BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
    }

    if ((lt_map_ref)->data_type == bcmFieldDataTypeMacrotoLtField) {
        *qual_value_inp_buf = tmp_qual_value_inp_buf;
        *qual_mask_inp_buf = tmp_qual_value_inp_buf;
    }
    /* Copy data in user provided memory */
    switch(qual_info->data_type) {
        case BCM_QUAL_DATA_TYPE_INT:
        case BCM_QUAL_DATA_TYPE_TRUNK:
        case BCM_QUAL_DATA_TYPE_IF:
        case BCM_QUAL_DATA_TYPE_L4_PORT:
        case BCM_QUAL_DATA_TYPE_VRF:
        case BCM_QUAL_DATA_TYPE_COS:
        case BCM_QUAL_DATA_TYPE_MULTICAST:
        case BCM_QUAL_DATA_TYPE_IF_GROUP:
        case BCM_QUAL_DATA_TYPE_TRILL_NAME:
        case BCM_QUAL_DATA_TYPE_GPORT:
        case BCM_QUAL_DATA_TYPE_PORT:
        case BCM_QUAL_DATA_TYPE_STG:
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if ((NULL == qual_info->enum_name)
                        && (NULL != lt_sym_str_op)) {
                    qual_info->enum_name = lt_sym_str_op;
                } else {
                    sal_memcpy(qual_value_inp_buf,
                              &qual_info->enum_value, qual_value_inp_buf_size);
                    sal_memcpy(qual_mask_inp_buf,
                              &qual_info->enum_mask, qual_value_inp_buf_size);
                }
            } else {
                sal_memcpy(*(int **)qual_info->qual_data,
                        qual_value_inp_buf, qual_value_inp_buf_size);
                sal_memcpy(*(int **)qual_info->qual_mask,
                        qual_mask_inp_buf, qual_value_inp_buf_size);
            }
            break;

        case BCM_QUAL_DATA_TYPE_UINT8:
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if ((NULL == qual_info->enum_name)
                        && (NULL != lt_sym_str_op)) {
                    qual_info->enum_name = lt_sym_str_op;
                } else {
                    sal_memcpy(&qual_info->enum_value,
                            qual_value_inp_buf, qual_value_inp_buf_size);
                    sal_memcpy(&qual_info->enum_mask,
                            qual_mask_inp_buf, qual_value_inp_buf_size);
                }
            } else {
                sal_memcpy(*(uint8_t **)qual_info->qual_data,
                        qual_value_inp_buf, qual_value_inp_buf_size);
                sal_memcpy(*(uint8_t **)qual_info->qual_mask,
                        qual_mask_inp_buf, qual_value_inp_buf_size);
            }
            break;

        case BCM_QUAL_DATA_TYPE_UINT16:
        case BCM_QUAL_DATA_TYPE_VLAN:
        case BCM_QUAL_DATA_TYPE_VPN:
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if ((NULL == qual_info->enum_name)
                        && (NULL != lt_sym_str_op)) {
                    qual_info->enum_name = lt_sym_str_op;
                } else {
                    sal_memcpy(&qual_info->enum_value,
                            qual_value_inp_buf, qual_value_inp_buf_size);
                    sal_memcpy(&qual_info->enum_mask,
                            qual_mask_inp_buf, qual_value_inp_buf_size);
                }
            } else {
                sal_memcpy(*(uint16_t **)qual_info->qual_data,
                        qual_value_inp_buf, qual_value_inp_buf_size);
                sal_memcpy(*(uint16_t **)qual_info->qual_mask,
                        qual_mask_inp_buf, qual_value_inp_buf_size);
            }
            break;

        case BCM_QUAL_DATA_TYPE_UINT32:
        case BCM_QUAL_DATA_TYPE_IP:
        case BCM_QUAL_DATA_TYPE_NAT_ID:
        case BCM_QUAL_DATA_TYPE_MPLS_LABEL:
            if (qual_info->flags & BCM_FIELD_F_ENUM) {
                if ((NULL == qual_info->enum_name)
                        && (NULL != lt_sym_str_op)) {
                    qual_info->enum_name = lt_sym_str_op;
                } else {
                    sal_memcpy(&qual_info->enum_value,
                            qual_value_inp_buf, qual_value_inp_buf_size);
                    sal_memcpy(&qual_info->enum_mask,
                            qual_mask_inp_buf, qual_value_inp_buf_size);
                }
            } else {
                sal_memcpy(*(uint32_t **)qual_info->qual_data,
                        qual_value_inp_buf, qual_value_inp_buf_size);
                sal_memcpy(*(uint32_t **)qual_info->qual_mask,
                        qual_mask_inp_buf, qual_value_inp_buf_size);
            }
            break;

        case BCM_QUAL_DATA_TYPE_UINT64:
        case BCM_QUAL_DATA_TYPE_CLASS:
            sal_memcpy(*(uint64_t **)qual_info->qual_data,
                    qual_value_inp_buf, qual_value_inp_buf_size);
            sal_memcpy(*(uint64_t **)qual_info->qual_mask,
                    qual_mask_inp_buf, qual_value_inp_buf_size);
            break;

        case BCM_QUAL_DATA_TYPE_MAC:
            sal_memcpy(*(uint8_t **)qual_info->qual_data,
                    qual_value_inp_buf, qual_value_inp_buf_size);
            sal_memcpy(*(uint8_t **)qual_info->qual_mask,
                    qual_mask_inp_buf, qual_value_inp_buf_size);
            break;

        case BCM_QUAL_DATA_TYPE_IP6:
            sal_memcpy(*(uint8_t **)qual_info->qual_data,
                    qual_value_inp_buf, qual_value_inp_buf_size);
            sal_memcpy(*(uint8_t **)qual_info->qual_mask,
                    qual_mask_inp_buf, qual_value_inp_buf_size);
            break;

        case BCM_QUAL_DATA_TYPE_PBMP:
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_pbmp_get(unit,
                                          group_oper->group_id,
                                          stage_info,
                                          qual_value_inp_buf,
                                          qual_mask_inp_buf,
                                         *(bcm_pbmp_t **)qual_info->qual_data,
                                         *(bcm_pbmp_t **)qual_info->qual_mask));
            break;

        case BCM_QUAL_DATA_TYPE_LLC:
            sal_memcpy(ref_data, qual_value_inp_buf, qual_value_inp_buf_size);
            sal_memcpy(ref_mask, qual_mask_inp_buf, qual_value_inp_buf_size);

            llc_data.dsap = (ref_data[0] & 0x00ff0000) >> 16;
            llc_data.ssap = (ref_data[0] & 0x0000ff00) >> 8;
            llc_data.control = ref_data[0] & 0x000000ff;

            llc_mask.dsap = (ref_mask[0] & 0x00ff0000) >> 16;
            llc_mask.ssap = (ref_mask[0] & 0x0000ff00) >> 8;
            llc_mask.control = ref_mask[0] & 0x000000ff;

            sal_memcpy(*(bcm_field_llc_header_t **)qual_info->qual_data,
                    &llc_data, qual_value_inp_buf_size);
            sal_memcpy(*(bcm_field_llc_header_t **)qual_info->qual_mask,
                    &llc_mask, qual_value_inp_buf_size);
            break;

        case BCM_QUAL_DATA_TYPE_SNAP:
            sal_memcpy(ref_data, qual_value_inp_buf, qual_value_inp_buf_size);
            sal_memcpy(ref_mask, qual_mask_inp_buf, qual_value_inp_buf_size);

            snap_data.type = ref_data[0] & 0x0000ffff;
            snap_data.org_code = (ref_data[0] & 0xffff0000) >> 16 |
                                  (ref_data[1] & 0x000000ff) << 16;

            snap_mask.type = ref_mask[0] & 0x0000ffff;
            snap_mask.org_code = (ref_mask[0] & 0xffff0000) >> 16 |
                                  (ref_mask[1] & 0x000000ff) << 16;

            sal_memcpy(*(bcm_field_snap_header_t **)qual_info->qual_data,
                    &snap_data, qual_value_inp_buf_size);
            sal_memcpy(*(bcm_field_snap_header_t **)qual_info->qual_mask,
                    &snap_mask, qual_value_inp_buf_size);
            break;

        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_qual_value_get(unit, entry, qual_info));

exit:
    if (BCMLT_INVALID_HDL != rule_template_hdl) {
        (void) bcmlt_entry_free(rule_template_hdl);
    }
    BCMINT_FIELD_MEM_FREE(qual_field);
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    BCMINT_FIELD_MEM_FREE(qual_mask_field);
    BCMINT_FIELD_MEM_FREE(qual_mask_inp_buf);
    BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
    BCMINT_FIELD_MEM_FREE(qual_value_inp_buf);
    BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmi_ltsw_field_qualfier_get
 *
 * Get qualifier values for provided entry
 * \param [in] unit      - BCM device number.
 * \param [in] entry     - Entry identifier.
 * \param [in/out] qual_info - Qualfier information.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided entry does not exist in system
 *     SHR_E_UNAVAIL   - provided qualifier is not supported in system
 *     SHR_E_PARAM     - provided qualifier value is not valid.
 *     SHR_E_NONE      - Success
 */
int
bcmi_ltsw_field_qualifier_get(int unit,
                              bcm_field_entry_t entry,
                              bcm_qual_field_t *qual_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_qualifier_get(unit, entry, qual_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_field_entry_policer_update_all(int unit,
                                         bcm_policer_t policer_id,
                                         uint8_t in_use)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_policer_update_all(unit, policer_id, in_use));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public Functions
 */

/*********************  INIT API's ******************************/

/*!
 * \brief Initialize the field module.
 *
 * Initial field database data structure and
 * clear the field related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_field_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lock_create(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the field module.
 *
 * Detach field database data structure.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcm_ltsw_field_detach(int unit)
{
    int rv;
    bool warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    rv = bcmint_ltsw_field_destination_deinit(unit);
    if (SHR_FAILURE(rv)) {
        FP_UNLOCK(unit);
        SHR_ERR_EXIT(rv);
    }

    rv = (bcmint_field_detach(unit, warm, FALSE));
    FP_UNLOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lock_destroy(unit));

exit:
    SHR_FUNC_EXIT();
}

/*********************  QUALIFIER API's ******************************/

STATIC int
ltsw_field_qual_delete(
    int unit,
    bcm_field_qualify_t qual_id,
    bcmint_field_stage_info_t *stage_info,
    char* rule_template_name,
    bcmlt_entry_handle_t rule_template_hdl,
    bool is_presel)
{
    int iter = 0;
    int map_count = 0;
    char *qual_field = NULL;
    uint32_t num_element = 0;
    bcm_qual_field_t qual_info;
    uint16_t ip_op_buf_size = 0;
    char *qual_mask_field = NULL;
    uint32_t src_ofset_current = 0;
    uint8_t *qual_mask_inp_buf = NULL;
    uint8_t *qual_value_inp_buf = NULL;
    uint64_t *qual_mask_op_buf = NULL;
    uint64_t *qual_value_op_buf = NULL;
    bcmlt_field_def_t *fld_defs_ref = NULL;
    bcmlt_field_def_t *fld_defs_iter = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_qual_map_find(unit, qual_id,
                                    stage_info, is_presel, 0, &map_count,
                                    &lt_map_ref));
    /* Check all lt maps width is within max allowed range */
    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t) * lt_map_ref->num_maps,
         "field definitions");
    fld_defs_iter = fld_defs_ref;
    qual_info.sdk6_qual_enum = qual_id;
    /* Iterate through all lt maps */
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if (FALSE == bcmint_field_map_supported(unit, stage_info,
                      (lt_map_ref)->map[iter])) {
            fld_defs_iter++;
            continue;
        }
        /* Retreive field definition for provided lt field in map */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, rule_template_name,
                (lt_map_ref->map[iter]).lt_field_name, fld_defs_iter));

        if (0 == fld_defs_iter->elements) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        fld_defs_iter++;
    }

    /* Go though all lt maps for a qualifier */
    src_ofset_current = 0;
    fld_defs_iter = fld_defs_ref;
    for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
        if (FALSE == bcmint_field_map_supported(unit, stage_info,
                     (lt_map_ref)->map[iter])) {
            continue;
        }
        ip_op_buf_size = sizeof(uint64_t) * fld_defs_iter[iter].elements;
        BCMINT_FIELD_MEM_ALLOC
            (qual_value_inp_buf, ip_op_buf_size, "qualifier value");
        BCMINT_FIELD_MEM_ALLOC
            (qual_mask_inp_buf, ip_op_buf_size, "qualifier mask value");
        BCMINT_FIELD_MEM_ALLOC
            (qual_value_op_buf, ip_op_buf_size, "qualifier value");
        BCMINT_FIELD_MEM_ALLOC
            (qual_mask_op_buf, ip_op_buf_size, "qualifier mask value");
        BCMINT_FIELD_MEM_ALLOC
            (qual_field, (strlen((lt_map_ref->map[iter]).lt_field_name) + 1),
             "qualifier field");
        BCMINT_FIELD_MEM_ALLOC
          (qual_mask_field, (strlen((lt_map_ref->map[iter]).lt_field_name) + 6),
             "qualifier mask field");

        sal_strcpy(qual_field, (lt_map_ref->map[iter]).lt_field_name);
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_qual_mask_lt_field_get(unit, qual_field, &qual_mask_field));
        num_element = fld_defs_iter[iter].elements;

        if (fld_defs_iter[iter].elements > 1) {
            if (fld_defs_iter[iter].symbol) {
                /* Not handled till now as no such case is present
                 * in db till date */
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            } else {
                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(rule_template_hdl,
                                                 qual_field,
                                                 0, qual_value_op_buf,
                                                 num_element, &num_element));
                if ((is_presel == true) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_get(rule_template_hdl,
                                                 qual_mask_field,
                                                 0, qual_mask_op_buf,
                                                 num_element, &num_element));
                }

                /* Set values in input buffer as per offset & width specified */
                sal_memset(qual_value_inp_buf, 0, ip_op_buf_size);
                sal_memset(qual_mask_inp_buf, 0, ip_op_buf_size);
                bcmint_entry_field_qual_buffer_delete(unit,
                                                      &(lt_map_ref->map[iter]),
                                                      &fld_defs_iter[iter],
                                                      &qual_info,
                                                      &src_ofset_current,
                                                      (uint32_t *)qual_value_inp_buf,
                                                      (uint32_t *)qual_mask_inp_buf,
                                                      (uint32_t *)qual_value_op_buf,
                                                      (uint32_t *)qual_mask_op_buf);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_add(rule_template_hdl,
                                                 qual_field,
                                                 0, qual_value_op_buf,
                                                 num_element));
                if ((is_presel == true) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_add(rule_template_hdl,
                                                 qual_mask_field,
                                                 0, qual_mask_op_buf,
                                                 num_element));
                }
            }
        } else {
            if (fld_defs_iter[iter].symbol) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(rule_template_hdl,
                                                  qual_field,
                                                  fld_defs_iter[iter].sym_def));
            } else {

                /* Retreive previously set qualifier & mask value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(rule_template_hdl,
                                           qual_field,
                                           qual_value_op_buf));
                if ((is_presel == true) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(rule_template_hdl,
                                           qual_mask_field,
                                           qual_mask_op_buf));
                }

                /* Set values in input buffer as per offset & width specified */
                sal_memset(qual_value_inp_buf, 0, ip_op_buf_size);
                sal_memset(qual_mask_inp_buf, 0, ip_op_buf_size);
                bcmint_entry_field_qual_buffer_delete(unit,
                                                      &(lt_map_ref->map[iter]),
                                                      &fld_defs_iter[iter],
                                                      &qual_info,
                                                      &src_ofset_current,
                                                      (uint32_t *)qual_value_inp_buf,
                                                      (uint32_t *)qual_mask_inp_buf,
                                                      (uint32_t *)qual_value_op_buf,
                                                      (uint32_t *)qual_mask_op_buf);

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(rule_template_hdl,
                                           qual_field,
                                           *qual_value_op_buf));
                if ((is_presel == true) ||
                    ((stage_info->stage_id != bcmiFieldStageExactMatch) &&
                    (stage_info->stage_id != bcmiFieldStageFlowTracker))) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(rule_template_hdl,
                                               qual_mask_field,
                                               *qual_mask_op_buf));
                }
            }
        }

        BCMINT_FIELD_MEM_FREE(qual_field);
        BCMINT_FIELD_MEM_FREE(qual_mask_field);
        BCMINT_FIELD_MEM_FREE(qual_mask_inp_buf);
        BCMINT_FIELD_MEM_FREE(qual_value_inp_buf);
        BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
        BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
    }

exit:
    BCMINT_FIELD_MEM_FREE(qual_field);
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    BCMINT_FIELD_MEM_FREE(qual_mask_field);
    BCMINT_FIELD_MEM_FREE(qual_mask_inp_buf);
    BCMINT_FIELD_MEM_FREE(qual_mask_op_buf);
    BCMINT_FIELD_MEM_FREE(qual_value_inp_buf);
    BCMINT_FIELD_MEM_FREE(qual_value_op_buf);

    SHR_FUNC_EXIT();
}

STATIC int
ltsw_field_compression_paired_qual_get(
    int unit,
    bcm_field_entry_t entry,
    uint32_t entry_flags,
    bcm_field_qualify_t qual_id,
    bcm_field_qualify_t *pair_qual
    )
{
    int l4port = 0;
    int l4port_mask = 0;
    bcm_ip_t ip = 0x0;
    bcm_ip_t ip_mask = 0x0;
    bcm_ip6_t ip6;
    bcm_ip6_t ip6_mask;
    bool is_ip6 = 0;
    bool is_ip = 0;
    bcm_field_qualify_t qual = bcmFieldQualifyCount;
    bcm_qual_field_t qual_info;
    bcmint_field_stage_info_t *stage_info;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));
    if ((qual_id == bcmFieldQualifySrcIp) ||
        (qual_id == bcmFieldQualifySrcIp6)) {
        if ((stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY) ||
            (stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY)) {
            if ((!(stage_info->flags &
                 BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY)) &&
                 (stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY)) {
                if (qual_id == bcmFieldQualifySrcIp) {
                    qual = bcmFieldQualifyL4SrcPort;
                }
            } else if ((!(stage_info->flags &
                 BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY)) &&
                 (stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY)) {
                if (qual_id == bcmFieldQualifySrcIp6) {
                    qual = bcmFieldQualifyL4SrcPort;
                }
            }
        } else {
            qual = bcmFieldQualifyL4SrcPort;
        }
    } else if ((qual_id == bcmFieldQualifyDstIp) ||
            (qual_id == bcmFieldQualifyDstIp6)) {
        if ((stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY) ||
            (stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY)) {
            if ((!(stage_info->flags &
                 BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY)) &&
                 (stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY)) {
                if (qual_id == bcmFieldQualifyDstIp) {
                    qual = bcmFieldQualifyL4DstPort;
                }
            } else if ((!(stage_info->flags &
                 BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY)) &&
                 (stage_info->flags &
                  BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY)) {
                if (qual_id == bcmFieldQualifyDstIp6) {
                    qual = bcmFieldQualifyL4DstPort;
                }
            }
        } else {
            qual = bcmFieldQualifyL4DstPort;
        }
    }

    if ((qual == bcmFieldQualifyL4SrcPort) ||
        (qual == bcmFieldQualifyL4DstPort)) {
        int *l4port_t = &l4port;
        int *l4port_t_mask = &l4port_mask;
        sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));

        qual_info.sdk6_qual_enum = qual;
        qual_info.depth = 1;
        qual_info.data_type = BCM_QUAL_DATA_TYPE_L4_PORT;
        qual_info.flags = 0;

        qual_info.qual_data = (void *) &l4port_t;
        qual_info.qual_mask = (void *) &l4port_t_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_qualifier_get(unit, entry, &qual_info));
        if (l4port_mask != 0) {
            *pair_qual = qual;
        }
        SHR_EXIT();
    }

    if (qual_id == bcmFieldQualifyL4SrcPort) {
        if ((stage_info->flags &
                BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY) &&
             (stage_info->flags &
                 BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY)) {
            SHR_EXIT();
        } else {
            if ((stage_info->flags &
                BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY) &&
             (!(stage_info->flags &
                 BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY))) {
                 if (entry_flags & BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT) {
                    qual = bcmFieldQualifySrcIp6;
                    is_ip6 = 1;
                 }
            } else if ((!(stage_info->flags &
                     BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP4_ONLY)) &&
                   (stage_info->flags &
                     BCMINT_FIELD_STAGE_COMPRESSION_SRC_IP6_ONLY)) {
                if (entry_flags & BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT) {
                    qual = bcmFieldQualifySrcIp;
                    is_ip = 1;
                }

            } else {
                if (entry_flags & BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT) {
                    qual = bcmFieldQualifySrcIp;
                    is_ip = 1;
                } else if (entry_flags & BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT) {
                   qual = bcmFieldQualifySrcIp6;
                    is_ip6 = 1;
                } else {
                    SHR_EXIT();
                }
            }
        }

    } else if (qual_id == bcmFieldQualifyL4DstPort) {
        if ((stage_info->flags &
                BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY) &&
             (stage_info->flags &
                 BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY)) {
            SHR_EXIT();
        } else {
            if ((stage_info->flags &
                BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY) &&
             (!(stage_info->flags &
                 BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY))) {
                 if (entry_flags & BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT) {
                    qual = bcmFieldQualifyDstIp6;
                    is_ip6 = 1;
                 }
            } else if ((!(stage_info->flags &
                     BCMINT_FIELD_STAGE_COMPRESSION_DST_IP4_ONLY)) &&
                   (stage_info->flags &
                     BCMINT_FIELD_STAGE_COMPRESSION_DST_IP6_ONLY)) {
                if (entry_flags & BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT) {
                    qual = bcmFieldQualifyDstIp;
                    is_ip = 1;
                }

            } else {
                if (entry_flags & BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT) {
                    qual = bcmFieldQualifyDstIp;
                    is_ip = 1;
                } else if (entry_flags & BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT) {
                   qual = bcmFieldQualifyDstIp6;
                    is_ip6 = 1;
                } else {
                    SHR_EXIT();
                }
            }
        }
    }

    if (is_ip == 1) {
        bcm_ip_t *ip_t = &ip;
        bcm_ip_t *ip_t_mask = &ip_mask;

        sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));
        qual_info.sdk6_qual_enum = qual;
        qual_info.depth = 1;
        qual_info.data_type = BCM_QUAL_DATA_TYPE_IP;
        qual_info.flags = 0;

        qual_info.qual_data = (void *)&ip_t;
        qual_info.qual_mask = (void *)&ip_t_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_qualifier_get(unit, entry, &qual_info));
        if (ip_mask != 0) {
            *pair_qual = qual;
        }
    }

    if (is_ip6 == 1) {
        bcm_ip6_t null_ip6;
        bcm_ip6_t *ip6_t = &ip6;
        bcm_ip6_t *ip6_t_mask = &ip6_mask;

        sal_memset(&null_ip6, 0x0, sizeof(null_ip6));
        sal_memset(&qual_info, 0, sizeof(bcm_qual_field_t));
        qual_info.sdk6_qual_enum = qual;
        qual_info.depth = 1;
        qual_info.data_type = BCM_QUAL_DATA_TYPE_IP6;
        qual_info.flags = 0;
        qual_info.qual_data = (void *) &ip6_t;
        qual_info.qual_mask = (void *) &ip6_t_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_qualifier_get(unit, entry, &qual_info));
        if (sal_memcmp(&ip6_mask, &null_ip6, sizeof(bcm_ip6_t))) {
            *pair_qual = qual;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_qualifier_delete
 *
 * Delete qualifier for provided entry
 * \param [in] unit    - BCM device number.
 * \param [in] entry   - Entry identifier.
 * \param [in] qual_id - Qualfier identifier.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided entry does not exist in system
 *     SHR_E_UNAVAIL   - provided qualifier is not supported in system
 *     SHR_E_NONE      - Success
 */
int
bcm_ltsw_field_qualifier_delete(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_qualify_t qual_id)
{
    int dunit = 0;
    int entry_id = 0;
    bool is_presel = FALSE;
    bcmi_field_ha_entry_oper_t *e_oper = NULL;
    bcmi_field_ha_group_oper_t *g_oper = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t rule_template_hdl = BCMLT_INVALID_HDL;
    char* rule_template_name = NULL, *rule_template_id_name = NULL;
    bcm_field_qualify_t qual_id_to_remove = bcmFieldQualifyCount;
    bcm_field_qset_t presel_qset;
    int presel_grp_id = 0;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;
    bool found = TRUE;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;

    /* Find lt_map for provided qualifier from db */
    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
        rule_template_name = stage_info->tbls_info->presel_entry_sid;
        rule_template_id_name = stage_info->tbls_info->presel_entry_key_fid;
        is_presel = TRUE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_presel_oper_lookup(unit, entry_id,
                                             stage_info,
                                             &presel_oper));
        presel_grp_id = presel_oper->group_id;
        /* Verify with qualifiers in presel grp. */
        if (presel_grp_id != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_presel_qset_get(unit,
                                          stage_info->stage_id,
                                          presel_grp_id,
                                          1,
                                          &presel_qset));
            if (!(BCM_FIELD_QSET_TEST(presel_qset,
                                qual_id))) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit, "Error: qualifier not part of"
                  " presel group qset.\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
            }
        }

    } else {
        /* check if qual is part of group qset */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_oper_lookup(unit, entry_id,
                                            stage_info,
                                            &e_oper));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_oper_lookup(unit, e_oper->group_id,
                                            stage_info,
                                            &g_oper));
        FP_HA_GROUP_OPER_QSET_TEST(g_oper,
                                   qual_id,
                                   found);
        if (found == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
        }
        rule_template_name = stage_info->tbls_info->rule_sid;
        rule_template_id_name = stage_info->tbls_info->rule_key_fid;
        is_presel = FALSE;
    }

    /* Entry handle allocate for rule Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              rule_template_name,
                              &rule_template_hdl));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(rule_template_hdl,
                               rule_template_id_name,
                               entry_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, rule_template_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_qual_delete(unit, qual_id,
                                stage_info,
                                rule_template_name,
                                rule_template_hdl,
                                is_presel));

    if ((BCMINT_FIELD_IS_PRESEL_ENTRY(entry) == 0) &&
        (BCMINT_FIELD_IS_COMPRESSION_QUAL(qual_id)) &&
        (g_oper->group_flags & BCMINT_FIELD_GROUP_COMPRESSION_ENABLED)) {
        uint32_t comp_flags = 0x0;
        bcm_field_qualify_t pair_qual = bcmFieldQualifyCount;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_compression_paired_qual_get(unit, entry,
                                                    e_oper->entry_flags,
                                                    qual_id, &pair_qual));
        if (pair_qual != bcmFieldQualifyCount) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_field_qual_delete(unit, pair_qual,
                                        stage_info,
                                        rule_template_name,
                                        rule_template_hdl,
                                        is_presel));
        }

        if (BCMINT_FIELD_IS_IP_QUAL(qual_id)) {
            qual_id_to_remove = qual_id;
        } else if (BCMINT_FIELD_IS_IP_QUAL(pair_qual)) {
            qual_id_to_remove = pair_qual;
        }
        switch (qual_id_to_remove) {
            case bcmFieldQualifySrcIp:
               comp_flags = BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP_L4PORT;
               break;
            case bcmFieldQualifySrcIp6:
                comp_flags = BCMINT_FIELD_ENTRY_COMP_PAIR_SRC_IP6_L4PORT;
                break;
            case bcmFieldQualifyDstIp:
                comp_flags = BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP_L4PORT;
                break;
            case bcmFieldQualifyDstIp6:
                comp_flags = BCMINT_FIELD_ENTRY_COMP_PAIR_DST_IP6_L4PORT;
                break;
             default:
                comp_flags = 0;
        }

        if ((comp_flags != 0) &&
              (BCMINT_FIELD_IS_IP_QUAL(qual_id_to_remove))) {
            /* Rule template Compression delete */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_field_entry_compression_del(unit, stage_info,
                                                  qual_id_to_remove,
                                                  rule_template_hdl));
            /* clear the compression flags */
            e_oper->entry_flags &= ~comp_flags;
        }
    }

    /* Insert rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, rule_template_hdl,
                                  BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != rule_template_hdl) {
        (void) bcmlt_entry_free(rule_template_hdl);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_qualify_clear
 *
 * Clear all qualifier for provided entry
 * \param [in] unit    - BCM device number.
 * \param [in] entry   - Entry identifier.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided entry does not exist in system
 *     SHR_E_NONE      - Success
 */
int
bcm_ltsw_field_qualify_clear(int unit, bcm_field_entry_t entry)
{
    int dunit = 0;
    int idx = 0, entry_id = 0;
    uint32_t nflds = 0;
    uint32_t num_element = 0;
    uint64_t *qual_value_op_buf = NULL;
    bcmlt_field_def_t *fld_defs = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t rule_template_hdl = BCMLT_INVALID_HDL;
    char* rule_template_name = NULL, *rule_template_id_name = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Validate the entry, if its valid entry, update stage param */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry, &stage_info));

    /* Mask the stage bits from given entry ID */
    entry_id = entry & BCM_FIELD_ID_MASK;
    if (BCMINT_FIELD_IS_PRESEL_ENTRY(entry)) {
        rule_template_name = stage_info->tbls_info->presel_entry_sid;
        rule_template_id_name = stage_info->tbls_info->presel_entry_key_fid;
    } else {
        rule_template_name = stage_info->tbls_info->rule_sid;
        rule_template_id_name = stage_info->tbls_info->rule_key_fid;
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_table_field_defs_get(dunit, rule_template_name, 0, NULL, &nflds));

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs, sizeof(bcmlt_field_def_t) * nflds, "field definitions");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(dunit, rule_template_name, nflds,
                                                        fld_defs, &nflds));

    /* Entry handle allocate for rule Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              rule_template_name,
                              &rule_template_hdl));

    /* Add rule_template_id field to Rule LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(rule_template_hdl,
                               rule_template_id_name,
                               entry_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, rule_template_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    for (idx = 0; idx < nflds; idx++) {
        if (fld_defs[idx].key == FALSE) {
            if (fld_defs[idx].elements > 1) {
                num_element = fld_defs[idx].elements;
                BCMINT_FIELD_MEM_ALLOC
                    (qual_value_op_buf,
                     sizeof(uint64_t) * num_element, "qualifier value");

                if (fld_defs[idx].symbol) {
                    
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_add(rule_template_hdl,
                                                     fld_defs[idx].name,
                                                     0, qual_value_op_buf,
                                                     num_element));
                }
                BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
            } else {
                if (fld_defs[idx].symbol) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_symbol_add(rule_template_hdl,
                                                      fld_defs[idx].name,
                                                      fld_defs[idx].sym_def));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(rule_template_hdl,
                                               fld_defs[idx].name,
                                               fld_defs[idx].def));
                }
            }
        }
    }

    /* Insert rule template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, rule_template_hdl,
                                  BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != rule_template_hdl) {
        (void) bcmlt_entry_free(rule_template_hdl);
    }
    BCMINT_FIELD_MEM_FREE(fld_defs);
    BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/********************* ACTION API's ******************************/
int
bcm_ltsw_field_action_add(int unit,
                          bcm_field_entry_t entry,
                          bcm_field_action_t action,
                          uint32 param0, uint32 param1)
{
    bcm_field_action_params_t params;

    SHR_FUNC_ENTER(unit);

    if ((bcmFieldActionRedirectPbmp == action)
        || (bcmFieldActionEgressPortsAdd == action)
        || (bcmFieldActionEgressMask == action)
        || (bcmFieldActionRedirectBcastPbmp == action)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&params, 0x0, sizeof(params));

    params.param0 = param0;
    params.param1 = param1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_field_action_config_info_add(unit,
                                               entry, action,
                                               &params, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_action_config_info_add(int unit,
                                      bcm_field_entry_t entry,
                                      bcm_field_action_t action,
                                      bcm_field_action_params_t *params,
                                      bcm_field_action_match_config_t *match_config)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_config_info_add(unit,
                                             entry, action,
                                             params, match_config));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_action_get(int unit,
                          bcm_field_entry_t entry,
                          bcm_field_action_t action,
                          uint32 *param0, uint32 *param1)
{
    bcm_field_action_params_t params;

    SHR_FUNC_ENTER(unit);
    sal_memset(&params, 0x0, sizeof(params));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_field_action_config_info_get(unit,
                                               entry, action,
                                               &params, NULL));
    *param0 = params.param0;
    *param1 = params.param1;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_action_config_info_get(int unit,
                                      bcm_field_entry_t entry,
                                      bcm_field_action_t action,
                                      bcm_field_action_params_t *params,
                                      bcm_field_action_match_config_t *match_config)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_config_info_get(unit,
                                             entry, action, NULL,
                                             params, match_config));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_action_delete(int unit,
                             bcm_field_entry_t entry,
                             bcm_field_action_t action,
                             uint32 param0, uint32 param1)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_action_delete(unit,
                                   entry, action,
                                   param0, param1, 0));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_action_remove(int unit,
                             bcm_field_entry_t entry,
                             bcm_field_action_t action)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_action_delete(unit,
                                   entry, action,
                                   0, 0, 1));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_action_remove_all(int unit,
                                 bcm_field_entry_t entry)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_action_delete_all(unit, entry));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
/********************* GROUP API's ******************************/

/*!
 * \brief bcm_ltsw_field_group_config_create
 *
 * Create a group with a mode (single, double, etc.), a port bitmap,
 * group size and a Group ID.
 * \param [in] unit         - BCM device number.
 * \param [in] group_config - Group create attributes namely:
 * \param [in] flags        - Bits indicate which parameters have been
 *                             passed to API and should be used during group
 *                             creation.
 * \param [in] qset        - Field qualifier set
 * \param [in] priority    - Priority within allowable range,
 *                             or BCM_FIELD_GROUP_PRIO_ANY to automatically
 *                             assign a priority; each priority value may be
 *                             used only once
 * \param [in] mode        - Group mode (single, double, triple or Auto-wide)
 * \param [in] ports       - Ports where group is defined
 * \param [out] group      - Requested Group ID. If Group ID is not set,
 *                              then API allocates and returns the created
 *                              Group ID.
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */
int
bcm_ltsw_field_group_config_create(int unit,
                                   bcm_field_group_config_t *group_config)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_config_create(unit, group_config, 0));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_group_remove
 *
 * Remove group with provided id.
 * Remove all the entries associated to the given group
 *
 * \param [in] unit     - BCM device number.
 * \param [in] group_id - Group Identifier.
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NONE      - Success
 */
int
bcm_ltsw_field_group_remove(int unit,
                            bcm_field_group_t group)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_entries_remove(unit, group));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_group_destroy
 *
 * Destroy group with provided id.
 *
 * \param [in] unit     - BCM device number.
 * \param [in] group_id - Group Identifier.
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NONE      - Success
 */
int
bcm_ltsw_field_group_destroy(int unit,
                             bcm_field_group_t group)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_destroy(unit, group));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_presel_set(int unit,
                                bcm_field_group_t group,
                                bcm_field_presel_set_t *presel)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_presel_set(unit, group, presel));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_presel_get(int unit,
                                bcm_field_group_t group,
                                bcm_field_presel_set_t *presel)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_group_presel_get(unit, group, presel));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_set(int unit,
                         bcm_field_group_t group,
                         bcm_field_qset_t qset)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_qset_set(unit, group, qset));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_get(int unit,
                         bcm_field_group_t group,
                         bcm_field_qset_t *qset)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_qset_get(unit, group, qset));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_action_set(int unit,
                                bcm_field_group_t group,
                                bcm_field_aset_t aset)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_aset_set(unit, group, aset));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_action_get(int unit,
                                bcm_field_group_t group,
                                bcm_field_aset_t *aset)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_aset_get(unit, group, aset));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_priority_get(int unit,
                                  bcm_field_group_t group,
                                  int *priority)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_priority_get(unit, group, priority));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_priority_set(int unit,
                                  bcm_field_group_t group,
                                  int prio)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_priority_set(unit, group, prio));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_enable_get(int unit,
                                bcm_field_group_t group,
                                int *enable)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_enable_get(unit, group, enable));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_enable_set(int unit,
                                bcm_field_group_t group,
                                int enable)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_enable_set(unit, group, enable));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_config_validate(
    int unit,
    bcm_field_group_config_t *group_config,
    bcm_field_group_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);


    FP_LOCK(unit);

    group_config->flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
    group_config->mode = *mode;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_config_create(unit, group_config, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_destroy(unit, group_config->group));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_install(
    int unit,
    bcm_field_group_t group)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_enable_set(unit,
                                         group,
                                         1));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_mode_get(
    int unit,
    bcm_field_group_t group,
    bcm_field_group_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_mode_get(unit,
                                     group,
                                     mode));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_oper_mode_get(
    int unit,
    bcm_field_qualify_t stage,
    bcm_field_group_oper_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_mode_get(unit,
                                          stage,
                                          mode));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/********************* ENTRY API's ******************************/

/*
 * Function: bcm_ltsw_field_entry_create
 * Purpose:
 *   Create a blank entry based on a group.
 *   Automatically generate an entry
 *   ID. Create Rule and Policy templates.
 * Parameters:
 *   unit  - BCM device number
 *   group - Field group ID
 *   entry - (OUT) New entry
 * Returns:
 *   BCM_E_INIT        BCM unit not initialized
 *   BCM_E_NOT_FOUND   group not found in unit
 *   BCM_E_PARAM       *entry was NULL
 *   BCM_E_RESOURCE    No unused entries available
 *   BCM_E_XXX         Error from bcm_field_entry_create_id
 * See Also:
 * bcm_field_entry_create_id
 */
int
bcm_ltsw_field_entry_create(int unit,
                            bcm_field_group_t group,
                            bcm_field_entry_t *entry)
{
    bcm_field_entry_config_t entry_config;
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    bcm_field_entry_config_t_init(&entry_config);
    entry_config.group_id = group;
    entry_config.entry_id = *entry;
    entry_config.flags = 0;
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_entry_config_create(unit,
                                  &entry_config));
    *entry = entry_config.entry_id;

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_create_id(int unit,
                              bcm_field_group_t group,
                              bcm_field_entry_t entry)
{
    bcm_field_entry_config_t entry_config;
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    bcm_field_entry_config_t_init(&entry_config);
    entry_config.group_id = group;
    entry_config.entry_id = entry;
    entry_config.flags |= BCM_FIELD_ENTRY_CREATE_WITH_ID;

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_entry_config_create(unit,
                                     &entry_config));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function: bcm_ltsw_field_entry_config_create
 * Purpose:
 *     create an entry based on configuration provided
 * Parameters:
 *     unit  - BCM device number
 *     entry_config - (INOUT) Configuration for entry creation
 * Returns:
 *     BCM_E_SUCCESS    Entry created succesfully
 *     BCM_E_XXX        Error from bcm_field_entry_create_id
 */
int
bcm_ltsw_field_entry_config_create(int unit,
                             bcm_field_entry_config_t *entry_config)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_entry_config_create(unit,
                                     entry_config));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function: bcm_ltsw_field_entry_config_get
 * Purpose:
 *     create an entry based on configuration provided
 * Parameters:
 *     unit  - BCM device number
 *     entry_config - (INOUT) Configuration of the requested entry
 * Returns:
 *     BCM_E_SUCCESS    Entry created succesfully
 *     BCM_E_XXX        Error if failed
 */
int
bcm_ltsw_field_entry_config_get(int unit,
                             bcm_field_entry_config_t *entry_config)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_entry_config_get(unit,
                                     entry_config));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*
 * Function: bcm_ltsw_field_entry_destroy
 * Purpose:
 *     Delete entry and associated templates if any
 * Parameters:
 *     unit  - BCM device number
 *     entry - (IN) New entry
 * Returns:
 *     BCM_E_INIT        BCM unit not initialized
 *     BCM_E_NOT_FOUND   group not found in unit
 */
int
bcm_ltsw_field_entry_destroy(int unit,
                             bcm_field_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_destroy(unit, entry, NULL));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function: bcm_ltsw_field_entry_destroy_all
 * Purpose:
 *     Delete all entry present in system
 * Parameters:
 *     unit  - BCM device number
 *     entry - (IN) New entry
 * Returns:
 *     BCM_E_INIT        BCM unit not initialized
 *     BCM_E_NOT_FOUND   group not found in unit
 */
int
bcm_ltsw_field_entry_destroy_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_destroy_all(unit, BCMINT_FIELD_ID_INVALID));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_config_copy(int unit,
                                 bcm_field_entry_t src_entry,
                                 bcm_field_entry_t *dst_entry,
                                 uint32 flags)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_copy(unit, src_entry,
                                 dst_entry, flags));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_copy(
    int unit,
    bcm_field_entry_t src_entry,
    bcm_field_entry_t *dst_entry)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    *dst_entry = 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_copy(unit, src_entry,
                                 dst_entry, 0));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_copy_id(
    int unit,
    bcm_field_entry_t src_entry,
    bcm_field_entry_t dst_entry)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    if (dst_entry == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_copy(unit, src_entry,
                                 &dst_entry, 0));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_install(int unit,
                             bcm_field_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_install(unit,
                                    entry));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_reinstall(int unit,
                               bcm_field_entry_t entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_field_entry_prio_get(int unit,
                              bcm_field_entry_t entry, int *prio)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_prio_get(unit,
                                     entry,
                                     prio));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_prio_set(int unit,
                              bcm_field_entry_t entry, int prio)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_prio_set(unit,
                                     entry,
                                     prio));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/* Install a field entry into large direct tables. */
int
bcm_ltsw_field_entry_install_and_handler_update(int unit,
                                                bcm_field_entry_t entry,
                                                bcm_field_entry_t *large_dt_entry)
{
    return (BCM_E_UNAVAIL);
}

int
bcm_ltsw_field_entry_enable_get(
    int unit,
    bcm_field_entry_t entry,
    int *enable_flag)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_enable_get(unit,
                                       entry,
                                       enable_flag));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_enable_set(
    int unit,
    bcm_field_entry_t entry,
    int enable_flag)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_enable_set(unit,
                                       entry,
                                       ((enable_flag != 0) ? 1 : 0)));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_remove(
    int unit,
    bcm_field_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_enable_set(unit,
                                       entry,
                                       2));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_field_entry_flexctr_attach
 *
 * Description:
 *       Attach the flexctr action and its associated counter index to the
 *       field entry. The counter index should be in the range of maximum
 *       counters associated to the flexctr action.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      entry             - (IN) Field entry id.
 *      flexctr_cfg       - (IN) Reference to Flexctr config structure.
 *
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_field_entry_flexctr_attach(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_flexctr_config_t *flexctr_cfg)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_flexctr_attach(unit, entry,
                                           flexctr_cfg));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_field_entry_flexctr_detach
 *
 * Description:
 *      Detach the flexctr for a given field entry.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      entry             - (IN) Field entry id.
 *      flexctr_cfg       - (IN) Reference to Flexctr config structure.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_field_entry_flexctr_detach(int unit,
                                    bcm_field_entry_t entry,
                                    bcm_field_flexctr_config_t *flexctr_cfg)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_flexctr_detach(unit, entry,
                                           flexctr_cfg));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *       bcm_ltsw_field_entry_policer_attach
 *
 * Description:
 *      Attach a policer to a field entry.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      entry             - (IN) Field entry id.
 *      level             - (IN) Policer level.
 *      policer_id        - (IN) Policer id.
 *
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_field_entry_policer_attach(int unit,
                                    bcm_field_entry_t entry,
                                    int level,
                                    bcm_policer_t policer_id)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_policer_attach(unit,
                                           entry,
                                           level,
                                           policer_id));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_field_entry_policer_detach
 *
 * Description:
 *      Detach a policer from a field entry.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      entry             - (IN) Field entry id.
 *      level             - (IN) Policer level.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_field_entry_policer_detach(int unit,
                                    bcm_field_entry_t entry,
                                    int level)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_policer_detach(unit,
                                           entry,
                                           level));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_field_entry_policer_detach_all
 *
 * Description:
 *      Detach all policers from a field entry.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      entry             - (IN) Field entry id.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_field_entry_policer_detach_all(int unit,
                                    bcm_field_entry_t entry)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_policer_detach_all(unit,
                                               entry));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_field_entry_policer_get
 *
 * Description:
 *      Get the policers attached to a field entry.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      entry             - (IN) Field entry id.
 *      level             - (IN) Policer level.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_field_entry_policer_get(int unit,
                                 bcm_field_entry_t entry,
                                 int level,
                                 bcm_policer_t *policer_id)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_policer_get(unit,
                                        entry,
                                        level,
                                        policer_id));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_field_entry_multi_get(
    int unit,
    bcm_field_group_t group,
    int entry_size,
    bcm_field_entry_t *entry_array,
    int *entry_count)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_multi_get(unit, group,
                                      entry_size,
                                      entry_array,
                                      entry_count));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/************************** PRESEL API's *******************************/

int
bcm_ltsw_field_presel_create_stage_id(int unit,
                                      bcm_field_stage_t stage,
                                      bcm_field_presel_t presel_id)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_create_stage_id(unit,
                                             stage,
                                             presel_id));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_presel_config_create
 *
 * Create a field presel ID with specified stage and group ID.
 * \param [in] unit          - BCM device number.
 * \param [inout] presel_config - Presel create attributes
 * Returns:
 *     BCM_E_PARAM     - Invalid param error
 *     BCM_E_NONE      - Success
 */
int
bcm_ltsw_field_presel_config_create(int unit,
                                   bcm_field_presel_config_t *presel_config)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_config_create(unit, presel_config));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_presel_destroy(int unit,
                              bcm_field_presel_t presel_entry)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_entry_destroy(unit, presel_entry));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/**************************** HINT's API's *******************************/

/*!
 * \brief bcm_ltsw_field_hints_create
 *
 * Create a hint identifier
 * \param [in] unit     - BCM device number.
 * \param [out] hint_id - Hint identifier.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_RESOURCE  - no resource to created hint id
 *     SHR_E_NONE      - Success
 */
int
bcm_ltsw_field_hints_create(int unit,
                            bcm_field_hintid_t *hint_id)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_hints_create(unit, hint_id));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_hints_destroy
 *
 * Destroy a hint identifier
 * \param [in] unit    - BCM device number.
 * \param [in] hint_id - Hint identifier.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided hintid does not exist in system
 *     SHR_E_BUSY      - provided hintid is in use by field group
 *     SHR_E_NONE      - Success
 */
int
bcm_ltsw_field_hints_destroy(int unit,
                             bcm_field_hintid_t hint_id)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_hints_destroy(unit, hint_id));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_hints_add
 *
 * Add hint entry in provided hint identifier.
 * \param [in] unit    - BCM device number.
 * \param [in] hint_id - Hint identifier.
 * \param [in] hint    - Hint entry.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided hintid does not exist in system
 *     SHR_E_BUSY      - provided hintid is in use by field group
 *     SHR_E_NONE      - Success
 */
int
bcm_ltsw_field_hints_add(int unit,
                         bcm_field_hintid_t hint_id,
                         bcm_field_hint_t *hint)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_hints_add(unit, hint_id, hint));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_hints_get
 *
 * Get hint entry from provided hint identifier.
 * \param [in] unit    - BCM device number.
 * \param [in] hint_id - Hint identifier.
 * \param [in/out] hint    - Hint entry.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided hintid does not exist in system
 *     SHR_E_BUSY      - provided hintid is in use by field group
 *     SHR_E_NONE      - Success
 */
int
bcm_ltsw_field_hints_get(int unit,
                         bcm_field_hintid_t hint_id,
                         bcm_field_hint_t *hint)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_hints_get(unit, hint_id, hint));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_hints_delete
 *
 * Delete hint entry from provided hint identifier.
 * \param [in] unit    - BCM device number.
 * \param [in] hint_id - Hint identifier.
 * \param [in] hint    - Hint entry.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided hintid does not exist in system
 *     SHR_E_BUSY      - provided hintid is in use by field group
 *     SHR_E_NONE      - Success
 */
int
bcm_ltsw_field_hints_delete(int unit,
                            bcm_field_hintid_t hint_id,
                            bcm_field_hint_t *hint)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_hints_delete(unit, hint_id, hint));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcm_ltsw_field_hints_delete_all
 *
 * Delete all hint entries attached to provided hint identifier.
 * \param [in] unit    - BCM device number.
 * \param [in] hint_id - Hint identifier.
 *
 * Returns:
 *     SHR_E_INIT      - BCM unit not initialized
 *     SHR_E_NOT_FOUND - provided hintid does not exist in system
 *     SHR_E_BUSY      - provided hintid is in use by field group
 *     SHR_E_NONE      - Success
 */
int
bcm_ltsw_field_hints_delete_all(int unit,
                                bcm_field_hintid_t hint_id)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_hints_delete_all(unit, hint_id));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


/*
 * Function: bcm_ltsw_field_group_ports_get
 *
 * Purpose:
 *    Retrieve bitmap of the ports associated
 *    with a given field group ID
 *
 * Parameters:
 *     unit  - BCM device number
 *     group - Field group
 *     pbmp -  Port bitmap of a given field group
 *
 * Returns:
 *     SHR_E_NOT_FOUND - If given group ID is not found
 *     SHR_E_NONE      - Success
 */
int
bcm_ltsw_field_group_ports_get(
    int unit,
    bcm_field_group_t group,
    bcm_pbmp_t *pbmp)
{
    SHR_FUNC_ENTER(unit);

    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_ports_get(unit, group, pbmp));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_field_group_traverse
 * Purpose:
 *      Traverse all the fp groups in the system, calling a specified
 *      callback for each one
 * Parameters:
 *      unit - (IN) Unit number.
 *      callback - (IN) A pointer to the callback function to call for each fp group
 *      user_data - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      SHR_E_xxx
 * Notes:
 */
int
bcm_ltsw_field_group_traverse(int unit, bcm_field_group_traverse_cb callback,
                             void *user_data)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_traverse(unit, callback, user_data));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_presel_group_config_create(
    int unit,
    bcm_field_presel_group_config_t *group_config)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_EXIT
        (bcmint_field_presel_group_config_create(unit, group_config));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_presel_group_config_get(
    int unit,
    bcm_field_presel_group_config_t *group_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_ltsw_field_presel_group_destroy(
    int unit,
    bcm_field_presel_group_t presel_group)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_EXIT
        (bcmint_field_presel_group_destroy(unit, presel_group));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qset_id_multi_set(
    int unit,
    bcm_field_qualify_t qual,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_EXIT
        (bcmint_field_qset_id_multi_set(unit, qual, num_objects,
                                        objects_list, qset));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qset_id_multi_get(
    int unit,
    bcm_field_qset_t qset,
    bcm_field_qualify_t qual,
    int max,
    int *objects_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_EXIT
        (bcmint_field_qset_id_multi_get(unit, qset, qual, max,
                                        objects_list, actual));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_qset_id_multi_delete(
    int unit,
    bcm_field_qualify_t qual,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_EXIT
        (bcmint_field_qset_id_multi_delete(unit, qual, num_objects,
                                           objects_list,
                                           qset));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
bcmint_field_control_map_find(
        int unit,
        bcm_field_control_t control,
        const bcm_field_control_map_t **lt_map)
{
    int start = 0, end = 0;
    int midpoint = 0, result = 0;
    bool found = 0;
    const bcm_field_control_map_t *lt_map_db = NULL;
    const bcm_field_control_map_t *lt_map_db_tmp = NULL;
    bcmint_field_control_info_t *fp_control = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    lt_map_db_tmp = lt_map_db = fp_control->control_lt_map_db;

    end = fp_control->control_db_count - 1;

    /* do binary search over db */
    while (end >= start) {
        midpoint = (end + start) / 2;
        lt_map_db_tmp = lt_map_db + midpoint;

        /* compare current db node with target action */
        if (lt_map_db_tmp->control < control) {
            result = -1;
        } else if (lt_map_db_tmp->control > control) {
            result = 1;
        } else {
            result = 0;
        }

        /* adjust start/end based on compare results */
        if (result == 1) {
            end = midpoint - 1;
        } else if (result == -1) {
            start = midpoint + 1;
        } else {
            found = 1;
            break;
        }
    }

    /* return results */
    if (found) {
        if (lt_map != NULL) {
            *lt_map = lt_map_db + midpoint;
        }
    } else {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "Error: Control is not "
                   "supported by the device\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_field_control_get(
        int unit,
        bcm_field_control_t control,
        uint32 *state)
{
    bcmlt_entry_handle_t lt_hdl = BCMLT_INVALID_HDL;
    const bcm_field_control_map_t *lt_map_ref = NULL;
    int dunit = 0;
    uint64_t data = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(state, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_map_find(unit, control, &lt_map_ref));

    if (lt_map_ref->map[0].func_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_map_ref->map[0].func_get(unit, state));
        SHR_EXIT();
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* LT handle allocate */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, (lt_map_ref->map[0]).lt_name, &lt_hdl));

    /* LOOKUP the LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, lt_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Get key from the LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(lt_hdl, (lt_map_ref->map[0]).lt_field_name, &data));

    /* Fill the OUT param */
    *state = data;

exit:
    if (BCMLT_INVALID_HDL != lt_hdl) {
        (void) bcmlt_entry_free(lt_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_ltsw_field_control_set(
        int unit,
        bcm_field_control_t control,
        uint32 state)
{
    bcmlt_entry_handle_t lt_hdl = BCMLT_INVALID_HDL;
    const bcm_field_control_map_t *lt_map_ref = NULL;
    bcmlt_field_def_t fld_defs_ref;
    int dunit = 0;
    uint64_t data = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_map_find(unit, control, &lt_map_ref));

    if (lt_map_ref->map[0].func_set) {
        SHR_IF_ERR_VERBOSE_EXIT
            (lt_map_ref->map[0].func_set(unit, state));
        SHR_EXIT();
    }

    sal_memset(&fld_defs_ref, 0, sizeof(bcmlt_field_def_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, (lt_map_ref->map[0]).lt_name,
                               (lt_map_ref->map[0]).lt_field_name, &fld_defs_ref));

    /* Validate field definitions for valid values */
    if (0 == fld_defs_ref.elements) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    data = state;

    if (data > fld_defs_ref.max) {
        LOG_DEBUG(BSL_LS_BCM_FP,
           (BSL_META_U(unit, "Error: Field control param value "
               "exceeds max allowed, check param value passed.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* LT handle allocate */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, (lt_map_ref->map[0]).lt_name, &lt_hdl));

    /* Add key to the template. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(lt_hdl, (lt_map_ref->map[0]).lt_field_name, data));

    /* Commit to the LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, lt_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != lt_hdl) {
        (void) bcmlt_entry_free(lt_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_control_set(
        int unit,
        bcm_field_control_t control,
        uint32 state)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_control_set(unit, control, state));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_control_get(
        int unit,
        bcm_field_control_t control,
        uint32 *state)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_control_get(unit, control, state));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int bcm_ltsw_field_stage_qset_get(
    int unit,
    bcm_field_stage_t stage,
    uint8 presel,
    bcm_field_qset_t *qset)
{
    int start = 0, end = 0;
    bcmi_ltsw_field_stage_t stage_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcm_field_qual_map_t *lt_map_db = NULL;
    bcm_field_qual_map_t *lt_map_db_tmp = NULL;
    int is_qual_supported = 0;
    int indx = 0;
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    switch(stage) {
        case bcmFieldStageIngress:
            stage_id = bcmiFieldStageIngress;
            break;
        case bcmFieldStageEgress:
            stage_id = bcmiFieldStageEgress;
            break;
        case bcmFieldStageLookup:
            stage_id = bcmiFieldStageVlan;
            break;
        case bcmFieldStageIngressExactMatch:
            stage_id = bcmiFieldStageExactMatch;
            break;
        case bcmFieldStageIngressFlowtracker:
            stage_id = bcmiFieldStageFlowTracker;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    if (presel) {
        lt_map_db_tmp = lt_map_db = stage_info->presel_qual_lt_map_db;
        end = stage_info->presel_qual_db_count - 1;
    } else {
        if (stage_info->group_qual_lt_map_db)  {
            lt_map_db_tmp = lt_map_db = stage_info->group_qual_lt_map_db;
            end = stage_info->group_qual_db_count - 1;
        } else  {
            lt_map_db_tmp = lt_map_db = stage_info->qual_lt_map_db;
            end = stage_info->qual_db_count - 1;
        }
    }
    BCM_FIELD_QSET_INIT(*qset);

    for (start=0; start<=end; start++) {
        lt_map_db_tmp = lt_map_db + start;
        is_qual_supported = 0;
        for (indx=0; indx < (lt_map_db_tmp)->num_maps; indx++) {
            if (TRUE == bcmint_field_map_supported(unit, stage_info,
                        (lt_map_db_tmp)->map[indx])) {
                is_qual_supported = 1;
                break;
            }
        }
        if (is_qual_supported) {
            BCM_FIELD_QSET_ADD(*qset, (int)lt_map_db_tmp->qual);
        }
    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();

}

int
bcm_ltsw_field_destination_entry_add(
        int unit,
        uint32   options,
        bcm_field_destination_match_t  *match,
        bcm_field_destination_action_t  *action)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_entry_add(unit, options, match, action));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_source_class_mode_set(
        int unit,
        bcm_field_stage_t stage_id,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_mode_t mode)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_source_class_mode_set(unit, stage_id, pbmp, mode));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_source_class_mode_get(
        int unit,
        bcm_field_stage_t stage_id,
        bcm_pbmp_t pbmp,
        bcm_field_src_class_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_source_class_mode_get(unit, stage_id, pbmp, mode));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_destination_entry_get(
    int unit,
    bcm_field_destination_match_t  *match,
    bcm_field_destination_action_t  *action)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_entry_get(unit, match, action));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_destination_entry_traverse(
    int unit,
    bcm_field_destination_entry_traverse_cb callback,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_entry_traverse(unit,
                                                      callback, user_data));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_destination_entry_delete(
    int unit,
    bcm_field_destination_match_t  *match)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_entry_delete(unit, match));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_destination_stat_attach(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t ctr_id)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_stat_attach(unit, match, ctr_id));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_destination_stat_detach(
    int unit,
    bcm_field_destination_match_t *match)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_stat_detach(unit, match));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_destination_stat_id_get(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t *ctr_id)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_stat_id_get(unit, match, ctr_id));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_destination_flexctr_object_set(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t value)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_flexctr_object_set(unit, match, value));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_destination_flexctr_object_get(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t *value)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_flexctr_object_get(unit, match, value));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_field_destination_mirror_index_get(
    int unit,
    bcm_field_destination_match_t *match,
    int *mirror_index)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_destination_mirror_index_get(unit, match,
                                                        mirror_index));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Group ID is the hint given to the flexctr create API to allocate unique pool.
 * However, for the groups with same priority, the allocated pool should be
 * same. This routine is invoked from flexctr during its create to retrieve
 * the actual hint (group priority), so that, it can allocate same pool for
 * same priority groups or allocate a new one.
 *
 * Note: This function is not protected by FP_LOCK, since it is being called from
 * flexctr module, in-case of multi-process environment, it may cause a deadlock
 * if one process tries to create flexctr and at the same time other tries to
 * set the group priority (group priority invokes flexctr API to update the
 * hint).
 */
int
bcmi_field_flexctr_group_info_get(
    int unit,
    bcm_flexctr_source_t source,
    int group,
    bcmi_field_flexctr_group_info_t *info)
{
    int pipe = 0;
    bcmi_ltsw_field_stage_t stage_id;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;


    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    switch (source) {
        case  bcmFlexctrSourceIfp:
            stage_id = bcmiFieldStageIngress;
            break;
        case  bcmFlexctrSourceVfp:
            stage_id = bcmiFieldStageVlan;
            break;
        case  bcmFlexctrSourceEfp:
            stage_id = bcmiFieldStageEgress;
            break;
        case  bcmFlexctrSourceExactMatch:
            stage_id = bcmiFieldStageExactMatch;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);

    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id,
                                    &stage_info));
    if (!((group > stage_info->stage_base_id) &&
          (group <=
          (((stage_info->stage_base_id) |
          (stage_info->group_info->gid_max))-
           BCMINT_FIELD_GROUP_RESERVE_GID_COUNT)))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Check whether the corresponding group ID exists. */
    if (SHR_BITGET(stage_info->group_info->grpid_bmp.w,
                   (group & BCM_FIELD_ID_MASK)) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit,
                                        (group & BCM_FIELD_ID_MASK),
                                        stage_info,
                                        &group_oper));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit, stage_info, group_oper, &pipe));

    info->pipe = pipe;
    info->priority = group_oper->priority;

    
    if (!(stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR)) {
        if (stage_info->stage_id != bcmiFieldStageExactMatch) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_primary_slice_id_get(unit,
                                                         group,
                                                         &(info->flex_ctr_obj_id)));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_field_entry_hit_index_get(int unit,
                              bcm_field_entry_t entry,
                              uint32_t *hit_index)
{
    int dunit;
    int entry_id = 0;
    uint32_t count = 0;
    uint64_t val64 = 0;
    uint32_t table_id = 0, entry_index = 0;
    const char *pt_name;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));
    if (stage_info->stage_id != bcmiFieldStageExactMatch) {
        SHR_EXIT();
    }

    entry_id = entry & BCM_FIELD_ID_MASK;
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_CONTROLs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(eh,
                                      TABLE_IDs,
                                      stage_info->tbls_info->entry_sid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, TABLE_OP_PT_INFOs, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit,
                                      &entry_template,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->entry_sid,
                                      stage_info->tbls_info->entry_key_fid,
                                      0, entry_id, NULL,
                                      NULL, 0, 0, 0, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_OP_PT_INFOs, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(eh,
                                      TABLE_IDs,
                                      stage_info->tbls_info->entry_sid));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_get(eh, PT_INDEXs, 0, &val64,
                                     1, &count));
    entry_index = val64;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_symbol_get(eh, PT_IDs, 0, &pt_name,
                                            1, &count));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;


    SHR_IF_ERR_EXIT
        (mbcm_ltsw_field_entry_table_id_get(unit,
                                            pt_name,
                                            &table_id,
                                            &entry_index));

    *hit_index = ((table_id << 15) | entry_index);
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/************************************************
 *  Port call back functions
 */

int
bcmi_ltsw_field_flex_port_update(int unit,
                            bcm_pbmp_t detached_pbmp,
                            bcm_pbmp_t attached_pbmp)
{

    bcmint_field_stage_info_t *stage_info = NULL;
    int gid = 0;
    bcmi_field_ha_group_oper_t *grp_oper = NULL;
    int idx = 0;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcm_qual_field_t *qual_info = NULL;
    bcm_pbmp_t *data = NULL;
    bcm_pbmp_t *mask = NULL;
    bool found = FALSE;
    bcm_pbmp_t  valid_full_mask;
    bcm_pbmp_t  old_valid_full_mask;
    bcm_pbmp_t  temp_mask;
    int oper_mode = -1;
    bcm_field_group_t group;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit,
                                     bcmiFieldStageIngress,
                                     &stage_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit,
                                           stage_info,
                                           &oper_mode));

    /* Inports qualifier is not available in global mode */
    if (oper_mode == bcmiFieldOperModeGlobal) {
        SHR_EXIT();
    }

    BCMINT_FIELD_MEM_ALLOC(qual_info, sizeof(bcm_qual_field_t), "bcmFieldInportsQualInfo");
    BCMINT_FIELD_MEM_ALLOC(data, sizeof(bcm_pbmp_t), "bcmFieldInportsPbmpData");
    BCMINT_FIELD_MEM_ALLOC(mask, sizeof(bcm_pbmp_t), "bcmFieldInportsPbmpMask");

    BCM_PBMP_CLEAR(valid_full_mask);

    qual_info->sdk6_qual_enum = bcmFieldQualifyInPorts;
    qual_info->depth = 1;
    qual_info->data_type = BCM_QUAL_DATA_TYPE_PBMP;
    qual_info->flags = 0;

    /* Loop through all groups */
    for (gid = 1;
         gid <= ((stage_info->group_info->gid_max)-
         BCMINT_FIELD_GROUP_RESERVE_GID_COUNT); gid++) {
        if (!SHR_BITGET(stage_info->group_info->grpid_bmp.w, gid)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmint_field_group_oper_lookup(unit,
                                           gid,
                                           stage_info,
                                           &grp_oper));
        found = FALSE;
        FP_HA_GROUP_OPER_QSET_TEST(grp_oper, bcmFieldQualifyInPorts, found);
        if (found == FALSE) {
            continue;
        }

        /* Retrieve valid ports */
        BCMINT_FIELD_STAGE_ENTRY_ID_GET(stage_info->stage_id,
                                        gid,
                                        group);
        SHR_IF_ERR_VERBOSE_EXIT(
             bcmint_field_group_ports_get(unit, group, &valid_full_mask));
        BCM_PBMP_ASSIGN(old_valid_full_mask, valid_full_mask);
        BCM_PBMP_REMOVE(old_valid_full_mask, attached_pbmp);

        FP_HA_ENT_BASE_BLK_GET(unit,
                               stage_info->stage_id,
                               0,
                               entry_base);
        for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
            FP_HA_ENT_BASE_OPER_GET(unit,
                                    stage_info->stage_id,
                                    entry_base,
                                    idx,
                                    entry_oper);
            if (entry_oper == NULL) {
                continue;
            }

            /* Search the list of entries */
            do {
                if (entry_oper->group_id == gid) {
                    BCM_PBMP_CLEAR(*data);
                    BCM_PBMP_CLEAR(*mask);
                    qual_info->qual_data = (void *) &data;
                    qual_info->qual_mask = (void *) &mask;
                    SHR_IF_ERR_VERBOSE_EXIT(
                        bcmi_ltsw_field_qualifier_get(unit,
                                                      entry_oper->entry_id,
                                                      qual_info));

                    /* Check if entry pbmp contains all valid ports */
                    BCM_PBMP_ASSIGN(temp_mask, *mask);
                    BCM_PBMP_AND(temp_mask, old_valid_full_mask);
                    if (BCM_PBMP_EQ(old_valid_full_mask, temp_mask)) {
                        /* Remove detached ports from the entry mask */
                        BCM_PBMP_REMOVE(*mask, detached_pbmp);
                        /* Add newly attached ports to the entry mask */
                        BCM_PBMP_OR(*mask, attached_pbmp);
                        BCM_PBMP_AND(*mask, valid_full_mask);
                        qual_info->qual_data = (void *) data;
                        qual_info->qual_mask = (void *) mask;
                        SHR_IF_ERR_VERBOSE_EXIT(
                            bcmi_ltsw_field_qualifier_set(unit,
                                                          entry_oper->entry_id,
                                                          qual_info));
                    }
                }
                FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
            } while (entry_oper != NULL);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_port_filter_enable_set(unit, 1,
                                                attached_pbmp));
exit:
    if (data != NULL) {
        BCMINT_FIELD_MEM_FREE(data);
    }
    if (mask != NULL) {
        BCMINT_FIELD_MEM_FREE(mask);
    }
    if (qual_info != NULL ) {
        BCMINT_FIELD_MEM_FREE(qual_info);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/************************************************
 *  Compression related internal functions
 */

/*
 * Function to retrieve the Operational Mode Status of compression.
 */
int
bcmint_field_comp_oper_mode_status_get(int unit,
                                  bcmint_field_control_info_t *fp_control,
                                  int *mode)
{
    int dunit = 0;
    uint64_t oper_mode = 0x0;
    bcmlt_entry_handle_t oper_mode_template = BCMLT_INVALID_HDL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fp_control, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mode, SHR_E_INTERNAL);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Entry handle allocate for operational mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              fp_control->comp_tbls_info->oper_mode_sid,
                              &oper_mode_template));

    /* Lookup mode template using handle. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, oper_mode_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Retreive operation mode field from fetched entry. */
    rv = (bcmlt_entry_field_get(oper_mode_template,
                                fp_control->comp_tbls_info->oper_mode_fid,
                                &oper_mode));
    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_ERR_EXIT(rv);
    }

    *mode = oper_mode;

exit:
    if (BCMLT_INVALID_HDL != oper_mode_template) {
        (void) bcmlt_entry_free(oper_mode_template);
    }
    SHR_FUNC_EXIT();
}


static int
bcmint_ltsw_field_comp_pipe_get(
        int unit,
        bcmint_field_control_info_t *fp_control,
        bcm_pbmp_t pbmp,
        int *field_pipe)
{
    uint8_t pipe = 0;
    bcm_pbmp_t pc_pbmp;
    bcm_port_config_t port_config;
    int num_pipe = 0, oper_mode = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fp_control, BCM_E_INTERNAL);
    SHR_NULL_CHECK(field_pipe, BCM_E_INTERNAL);

    /* Retrieve port configuration */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));

    /* Retrieve operational mode status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_comp_oper_mode_status_get(unit, fp_control, &oper_mode));

    if (oper_mode == 0) {
        *field_pipe = -1;
        SHR_EXIT();
    }

    /* per-pipe pbm */
    num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);

    for (pipe = 0; pipe < num_pipe; pipe++) {
        BCM_PBMP_ASSIGN(pc_pbmp, port_config.per_pp_pipe[pipe]);
        if (BCM_PBMP_EQ(pbmp, pc_pbmp)) {
            break;
        }
    }

    if (pipe == num_pipe) {
        LOG_DEBUG(BSL_LS_BCM_FP,
           (BSL_META_U(unit, "Error: Unable to find pipe"
               " for provided group port.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Found pipe succesfully, return it to calling api. */
    *field_pipe = pipe;

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_compression_direct_mapped_set(
    int unit,
    bcm_field_compression_map_type_t type,
    bcm_pbmp_t ports,
    uint16 entry_cnt,
    uint16 *val_arr,
    uint16 *map_val_arr)
{
    bcmint_field_control_info_t *fp_control = NULL;
    int pipe = 0;
    char *sid = NULL;
    char *val_fid = NULL;
    char *map_fid = NULL;
    char *pipe_fid = NULL;
    bcmlt_entry_handle_t comp_template = BCMLT_INVALID_HDL;
    int dunit = 0;
    int eid = 0;
    bcmlt_field_def_t *fld_defs_ref = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(val_arr, BCM_E_PARAM);
    SHR_NULL_CHECK(map_val_arr, BCM_E_PARAM);
    FP_LOCK(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t), "field definitions");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    switch (type) {

        case bcmFieldCompressionDirectMapTypeTtl:
            sid = fp_control->comp_tbls_info->direct_mapped_ttl_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_ttl_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_ttl_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTos:
            sid = fp_control->comp_tbls_info->direct_mapped_tos_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tos_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tos_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTcp:
            sid = fp_control->comp_tbls_info->direct_mapped_tcp_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tcp_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tcp_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTtlZero:
            sid = fp_control->comp_tbls_info->direct_mapped_ttl_zero_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_ttl_zero_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_ttl_zero_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTosZero:
            sid = fp_control->comp_tbls_info->direct_mapped_tos_zero_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tos_zero_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tos_zero_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTcpZero:
            sid = fp_control->comp_tbls_info->direct_mapped_tcp_zero_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tcp_zero_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tcp_zero_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeIpTunnelTtl:
            sid = fp_control->comp_tbls_info->direct_mapped_tunnelttl_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tunnelttl_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tunnelttl_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeIpProtocol:
            sid = fp_control->comp_tbls_info->direct_mapped_ipprotocol_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_ipprotocol_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_ipprotocol_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;
        case bcmFieldCompressionDirectMapTypeTtlOne:
            sid = fp_control->comp_tbls_info->direct_mapped_ttl_one_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_ttl_one_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_ttl_one_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTosOne:
            sid = fp_control->comp_tbls_info->direct_mapped_tos_one_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tos_one_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tos_one_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTcpOne:
            sid = fp_control->comp_tbls_info->direct_mapped_tcp_one_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tcp_one_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tcp_one_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Entry handle allocate for Comp Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             sid,
                             &comp_template));

    /* Get pipe to be configured based on ports */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_comp_pipe_get(unit,
                                       fp_control,
                                       ports,
                                       &pipe));
    if (pipe != -1) {

        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(comp_template,
                                  pipe_fid,
                                  pipe));
    }

    for (eid = 0; eid < entry_cnt; eid++) {

        sal_memset(fld_defs_ref, 0,
                   sizeof(bcmlt_field_def_t));
        /* Retreive field definition for LT field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit,
                sid, val_fid, fld_defs_ref));
        if (val_arr[eid] > (1 << fld_defs_ref->width)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(comp_template,
                                  val_fid,
                                  val_arr[eid]));

        sal_memset(fld_defs_ref, 0,
                   sizeof(bcmlt_field_def_t));
        /* Retreive field definition for LT field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit,
                sid, map_fid, fld_defs_ref));
        if (map_val_arr[eid] > (1 << fld_defs_ref->width)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(comp_template,
                                  map_fid,
                                  map_val_arr[eid]));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, comp_template,
                                  BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (comp_template != BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(comp_template));
    }
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_compression_direct_mapped_get(
    int unit,
    bcm_field_compression_map_type_t type,
    bcm_pbmp_t ports,
    uint16 entry_cnt,
    uint16 *val_arr,
    uint16 *map_val_arr)
{
    bcmint_field_control_info_t *fp_control = NULL;
    int pipe = 0;
    char *sid = NULL;
    char *val_fid = NULL;
    char *map_fid = NULL;
    char *pipe_fid = NULL;
    bcmlt_entry_handle_t comp_template = BCMLT_INVALID_HDL;
    int dunit = 0;
    int eid = 0;
    uint64_t mapped_val = 0;
    bcmlt_field_def_t *fld_defs_ref = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(val_arr, BCM_E_PARAM);
    SHR_NULL_CHECK(map_val_arr, BCM_E_PARAM);
    FP_LOCK(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t), "field definitions");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    switch (type) {

        case bcmFieldCompressionDirectMapTypeTtl:
            sid = fp_control->comp_tbls_info->direct_mapped_ttl_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_ttl_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_ttl_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTos:
            sid = fp_control->comp_tbls_info->direct_mapped_tos_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tos_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tos_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTcp:
            sid = fp_control->comp_tbls_info->direct_mapped_tcp_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tcp_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tcp_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTtlZero:
            sid = fp_control->comp_tbls_info->direct_mapped_ttl_zero_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_ttl_zero_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_ttl_zero_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTosZero:
            sid = fp_control->comp_tbls_info->direct_mapped_tos_zero_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tos_zero_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tos_zero_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTcpZero:
            sid = fp_control->comp_tbls_info->direct_mapped_tcp_zero_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tcp_zero_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tcp_zero_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeIpTunnelTtl:
            sid = fp_control->comp_tbls_info->direct_mapped_tunnelttl_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tunnelttl_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tunnelttl_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeIpProtocol:
            sid = fp_control->comp_tbls_info->direct_mapped_ipprotocol_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_ipprotocol_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_ipprotocol_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;
        case bcmFieldCompressionDirectMapTypeTtlOne:
            sid = fp_control->comp_tbls_info->direct_mapped_ttl_one_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_ttl_one_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_ttl_one_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTosOne:
            sid = fp_control->comp_tbls_info->direct_mapped_tos_one_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tos_one_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tos_one_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        case bcmFieldCompressionDirectMapTypeTcpOne:
            sid = fp_control->comp_tbls_info->direct_mapped_tcp_one_sid;
            val_fid = fp_control->comp_tbls_info->direct_val_tcp_one_fid;
            map_fid = fp_control->comp_tbls_info->direct_map_val_tcp_one_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Entry handle allocate for Comp Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             sid,
                             &comp_template));

    /* Get pipe to be configured based on ports */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_comp_pipe_get(unit,
                                       fp_control,
                                       ports,
                                       &pipe));
    if (pipe != -1) {

        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(comp_template,
                                  pipe_fid,
                                  pipe));
    }

    for (eid = 0; eid < entry_cnt; eid++) {

        sal_memset(fld_defs_ref, 0,
                   sizeof(bcmlt_field_def_t));
        /* Retreive field definition for LT field */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit,
                sid, val_fid, fld_defs_ref));
        if (val_arr[eid] > (1 << fld_defs_ref->width)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(comp_template,
                                  val_fid,
                                  val_arr[eid]));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, comp_template,
                                  BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_get(comp_template,
                                  map_fid,
                                  &mapped_val));
        map_val_arr[eid] = mapped_val;
    }

exit:
    if (comp_template != BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(comp_template));
    }
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_compression_index_mapped_set(
    int unit,
    bcm_field_compression_map_type_t type,
    uint16 index,
    bcm_pbmp_t ports,
    uint16 val,
    uint16 map_val)
{
    bcmint_field_control_info_t *fp_control = NULL;
    int pipe = 0;
    char *sid = NULL;
    char *val_fid = NULL;
    char *map_fid = NULL;
    char *pipe_fid = NULL;
    char *index_fid = NULL;
    bcmlt_entry_handle_t comp_template = BCMLT_INVALID_HDL;
    int dunit = 0;
    int oper_mode = -1;
    bcmlt_field_def_t *fld_defs_ref = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t), "field definitions");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    switch (type) {

        case bcmFieldCompressionIndexMapTypeEthertype:
            sid = fp_control->comp_tbls_info->index_mapped_ethertype_sid;
            val_fid = fp_control->comp_tbls_info->index_val_ethertype_fid;
            map_fid = fp_control->comp_tbls_info->index_map_val_ethertype_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            index_fid = fp_control->comp_tbls_info->index_key_ethertype_fid;
            break;

       case bcmFieldCompressionIndexMapTypeIpProtocol:
            sid = fp_control->comp_tbls_info->index_mapped_ipprotocol_sid;
            val_fid = fp_control->comp_tbls_info->index_val_ipprotocol_fid;
            map_fid = fp_control->comp_tbls_info->index_map_val_ipprotocol_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            index_fid = fp_control->comp_tbls_info->index_key_ipprotocol_fid;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Entry handle allocate for Comp Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             sid,
                             &comp_template));

    /* Retrieve operational mode status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_comp_oper_mode_status_get(unit,
                                                fp_control,
                                                &oper_mode));

    /* Get pipe to be configured based on ports */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_comp_pipe_get(unit,
                                       fp_control,
                                       ports,
                                       &pipe));

    if (oper_mode == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(comp_template,
                                  pipe_fid,
                                  pipe));
    }

    sal_memset(fld_defs_ref, 0,
               sizeof(bcmlt_field_def_t));
    /* Retreive field definition for LT field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit,
            sid, index_fid, fld_defs_ref));
    if (index > (1 << fld_defs_ref->width)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(comp_template,
                              index_fid,
                              index));

    sal_memset(fld_defs_ref, 0,
               sizeof(bcmlt_field_def_t));
    /* Retreive field definition for LT field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit,
            sid, val_fid, fld_defs_ref));
    if (val > (1 << fld_defs_ref->width)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(comp_template,
                              val_fid,
                              val));

    sal_memset(fld_defs_ref, 0,
               sizeof(bcmlt_field_def_t));
    /* Retreive field definition for LT field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit,
            sid, map_fid, fld_defs_ref));
    if (map_val > (1 << fld_defs_ref->width)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(comp_template,
                              map_fid,
                              map_val));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, comp_template,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (comp_template != BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(comp_template));
    }
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_compression_index_mapped_get(
    int unit,
    bcm_field_compression_map_type_t type,
    uint16 index,
    bcm_pbmp_t ports,
    uint16 *val,
    uint16 *map_val)
{
    bcmint_field_control_info_t *fp_control = NULL;
    int pipe = 0;
    char *sid = NULL;
    char *val_fid = NULL;
    char *map_fid = NULL;
    char *pipe_fid = NULL;
    char *index_fid = NULL;
    bcmlt_entry_handle_t comp_template = BCMLT_INVALID_HDL;
    int dunit = 0;
    int oper_mode = -1;
    uint64_t value = 0, mapped_value = 0;
    bcmlt_field_def_t *fld_defs_ref = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t), "field definitions");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_control_get(unit, &fp_control));

    switch (type) {

        case bcmFieldCompressionIndexMapTypeEthertype:
            sid = fp_control->comp_tbls_info->index_mapped_ethertype_sid;
            val_fid = fp_control->comp_tbls_info->index_val_ethertype_fid;
            map_fid = fp_control->comp_tbls_info->index_map_val_ethertype_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            index_fid = fp_control->comp_tbls_info->index_key_ethertype_fid;
            break;

       case bcmFieldCompressionIndexMapTypeIpProtocol:
            sid = fp_control->comp_tbls_info->index_mapped_ipprotocol_sid;
            val_fid = fp_control->comp_tbls_info->index_val_ipprotocol_fid;
            map_fid = fp_control->comp_tbls_info->index_map_val_ipprotocol_fid;
            pipe_fid = fp_control->comp_tbls_info->pipe_fid;
            index_fid = fp_control->comp_tbls_info->index_key_ipprotocol_fid;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Entry handle allocate for Comp Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             sid,
                             &comp_template));

    /* Retrieve operational mode status */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_comp_oper_mode_status_get(unit,
                                                fp_control,
                                                &oper_mode));

    /* Get pipe to be configured based on ports */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_field_comp_pipe_get(unit,
                                       fp_control,
                                       ports,
                                       &pipe));

    if (oper_mode == 1) {
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(comp_template,
                                  pipe_fid,
                                  pipe));
    }

    sal_memset(fld_defs_ref, 0,
               sizeof(bcmlt_field_def_t));
    /* Retreive field definition for LT field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit,
            sid, index_fid, fld_defs_ref));
    if (index > (1 << fld_defs_ref->width)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(comp_template,
                              index_fid,
                              index));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, comp_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_get(comp_template,
                              val_fid,
                              &value));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_get(comp_template,
                              map_fid,
                              &mapped_value));
    *map_val = mapped_value;
    *val = value;

exit:
    if (comp_template != BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(comp_template));
    }
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

#if defined(BROADCOM_DEBUG)

/*
 * Function:
 *     bcmint_field_qual_name
 * Purpose:
 *     Translate a Qualifier enum value to a text string.
 * Parameters:
 *     Enum value from bcm_field_qualify_e. (ex.bcmFieldQualifyInPorts)
 * Returns:
 *     Text name of indicated qualifier enum value.
 */
char *
bcmint_field_qual_name(bcm_field_qualify_t qid)
{
    /* Text names of the enumerated qualifier IDs. */
    static char *qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
    static char *_qual_text[bcmiFieldQualifyLastCount - bcmFieldQualifyCount] =
                                                _BCMI_FIELD_QUALIFY_STRINGS;

    if (qid < bcmFieldQualifyCount) {
        return qual_text[qid];
    } else if ((int)qid < bcmiFieldQualifyLastCount) {
        return _qual_text[qid - bcmFieldQualifyCount];
    }
    return "UnknownQualifier";
 }

/*
 * Function:
 *     bcmint_field_action_name
 * Purpose:
 *     Return text name of indicated action enum value.
 */
char *
bcmint_field_action_name(bcm_field_action_t action)
{
    /* Text names of Actions. These are used for debugging output and CLIs.
     * Note that the order needs to match the bcm_field_action_t enum order.
     */
    static char *action_text[] = BCM_FIELD_ACTION_STRINGS;
    assert(COUNTOF(action_text)     == bcmFieldActionCount);

    return (action >= bcmFieldActionCount ? "??" : action_text[action]);
}

/*
 * Function:
 *     bcmint_field_stage_name
 * Purpose:
 *     Translate group pipeline  stage to a text string.
 * Parameters:
 *     stage_fc stage field control structure.
 * Returns:
 *     Text name of indicated stage qualifier enum value.
 */
char *
bcmint_field_stage_name(bcmi_ltsw_field_stage_t stage_id)
{
    static char *stage_text[] = BCMI_FIELD_STAGE_STRINGS;

    if (stage_id >= COUNTOF(stage_text)) {
        return "??";
    }
    return stage_text[stage_id];
}

/*
 * Function:
 *     bcmint_field_group_mode_name
 * Purpose:
 *     Return text name of indicated group mode enum value.
 */
char *
bcmint_field_group_mode_name(bcm_field_group_mode_t mode)
{
    static char *mode_text[bcmFieldGroupModeCount] =
                 BCM_FIELD_GROUP_MODE_STRINGS;

    return (mode >= bcmFieldGroupModeCount ? "??" : mode_text[mode]);
}

static int
bcmint_field_print_fields(
    int unit,
    bcmlt_entry_handle_t *templ,
    bcmlt_field_def_t *fld_defs_ref,
    char *lt_field_name)
{
    uint64_t *qual_value_op_buf = NULL;
    uint32_t num_element = 0;
    uint32_t value= 0;
    char *lt_sym_str_op = NULL;
    uint8_t id = 0;

    SHR_FUNC_ENTER(unit);

    num_element = fld_defs_ref->elements;
    BCMINT_FIELD_MEM_ALLOC
        (qual_value_op_buf,
        (sizeof(uint64_t) * num_element),
        "qualifier value");

    if (num_element > 1) {
        if (fld_defs_ref->symbol) {
            /* Not handled till now as no such case is present
             * in db till date */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else {
            /* Retreive from qualifier value */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(*templ,
                                         lt_field_name,
                                         0, qual_value_op_buf,
                                         num_element,
                                         &num_element));

            for (id = 0; id < num_element; id++) {
                if (fld_defs_ref->width > 32) {
                    value = COMPILER_64_HI(qual_value_op_buf[id]);
                    LOG_CLI((BSL_META_U(unit,
                               "%08x "), value));
                }
                value = COMPILER_64_LO(qual_value_op_buf[id]);
                LOG_CLI((BSL_META_U(unit,
                           "%08x "), value));
            }
        }
    } else {
        if (fld_defs_ref->symbol) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(*templ,
                                          lt_field_name,
                              (const char **)&lt_sym_str_op));
            LOG_CLI((BSL_META_U(unit,
                       "%s "), lt_sym_str_op));
        } else {
            /* Retreive from qualifier value */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(*templ,
                                   lt_field_name,
                                   qual_value_op_buf));
            if (fld_defs_ref->width > 32) {
                value = COMPILER_64_HI(*qual_value_op_buf);
                LOG_CLI((BSL_META_U(unit,
                           "%08x "), value));
            }
            value = COMPILER_64_LO(*qual_value_op_buf);
            LOG_CLI((BSL_META_U(unit,
                       "%08x "), value));
        }
    }
exit:
    BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
    SHR_FUNC_EXIT();

}


static int
bcmint_field_counter_info_get(int unit,
                            bcmint_field_stage_info_t *stage_info,
                            int gid,
                            int pipe,
                            uint32_t *num_counters_total,
                            uint32_t *num_counters_free,
                            uint32_t *num_counters_in_use)
{
    bool counters_present = FALSE;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    int idx = 0;
    bcm_flexctr_source_t source;
    bcm_flexctr_action_t action_info;
    uint32_t stat_counter_id = 0;
    bcmi_ltsw_flexctr_stat_info_t info;

    SHR_FUNC_ENTER(unit);

    switch (stage_info->stage_id) {
        case bcmiFieldStageIngress:
            source = bcmFlexctrSourceIfp;
            break;
        case bcmiFieldStageVlan:
            source = bcmFlexctrSourceVfp;
            break;
        case bcmiFieldStageEgress:
            source = bcmFlexctrSourceEfp;
            break;
        case bcmiFieldStageExactMatch:
            source = bcmFlexctrSourceExactMatch;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                entry_oper);
        if (entry_oper == NULL) {
            continue;
        }

        /* Search the list of entries */
        do {
            if ((entry_oper->group_id == gid) &&
                (entry_oper->flexctr_action_id != 0)) {
                counters_present = TRUE;
                stat_counter_id = entry_oper->flexctr_action_id;
                break;
            }
            FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
        } while (entry_oper != NULL);
        if (counters_present) {
            break;
        }
    }
    if (counters_present) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_flexctr_action_get(unit, stat_counter_id,
                                    &action_info));
        *num_counters_total = action_info.index_num;
        *num_counters_free = 0;
    } else {
        sal_memset(&info, 0x0, sizeof(bcmi_ltsw_flexctr_stat_info_t));
        (void)bcmi_ltsw_flexctr_stat_info_get(unit, source,
                                              pipe, &info);
        *num_counters_total = info.total_entries;
        *num_counters_free  = info.free_entries;
    }
    /* User needs to handle this in application */
    *num_counters_in_use = 0;

exit:
    SHR_FUNC_EXIT();

}
static int
bcmint_field_meter_info_get(int unit,
                            bcmint_field_stage_info_t *stage_info,
                            int pipe,
                            uint32_t *num_meters_total,
                            uint32_t *num_meters_free)
{
    bcmlt_entry_handle_t meter_info_templ = BCMLT_INVALID_HDL;
    uint64_t total_meters = 0;
    int gid = 0;
    uint32_t num_meters_per_grp = 0;
    uint32_t total_used_meters  = 0;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    if (NULL == stage_info->tbls_info->meter_info_sid) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    /* Allocate and Read meter info template of source entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &meter_info_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->meter_info_sid,
                                      0, 0, 0,
                                      0, 0,
                                      0, 0,
                                      0, 0));

    if (pipe == -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(meter_info_templ,
                                   stage_info->tbls_info->meter_info_num_meters_fid,
                                   &total_meters));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(meter_info_templ,
                                   stage_info->tbls_info->meter_info_num_meters_per_pipe_fid,
                                   &total_meters));
    }

    *num_meters_total = COMPILER_64_LO(total_meters);

    for (gid = 1;
         gid <= ((stage_info->group_info->gid_max)-
         BCMINT_FIELD_GROUP_RESERVE_GID_COUNT); gid++) {
        if (!SHR_BITGET(stage_info->group_info->grpid_bmp.w, gid)) {
            continue;
        }

        if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICER_SUPPORT) ||
            (stage_info->flags & BCMINT_FIELD_STAGE_LEGACY_POLICER_SUPPORT)) {
             SHR_IF_ERR_VERBOSE_EXIT(
                 bcmint_field_group_oper_lookup(unit,
                                            gid,
                                            stage_info,
                                            &group_oper));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_policer_num_meters_in_use_per_pool_get(
                                             unit,
                                             stage_info->stage_id,
                                             pipe,
                                             group_oper->policer_pool_id,
                                             &num_meters_per_grp));
            total_used_meters += num_meters_per_grp;
        }
     }
     *num_meters_free  = *num_meters_total - total_used_meters;

exit:
    if (BCMLT_INVALID_HDL != meter_info_templ) {
        (void) bcmlt_entry_free(meter_info_templ);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_field_flex_counter_info_get(
    int unit,
    bcmi_field_ha_entry_oper_t *entry_oper,
    bcmi_field_ha_group_oper_t *group_oper,
    bcmint_field_stage_info_t *stage_info,
    uint64_t *counter_id,
    uint64_t *gp_counter_id,
    uint64_t *rp_counter_id,
    uint64_t *yp_counter_id)
{
    SHR_FUNC_ENTER(unit);

    if (entry_oper->counter_color == 0) {
        SHR_NULL_CHECK(counter_id, SHR_E_PARAM);
 
        *counter_id = entry_oper->counter_idx;
    } else {
        SHR_NULL_CHECK(gp_counter_id, SHR_E_PARAM);
        SHR_NULL_CHECK(yp_counter_id, SHR_E_PARAM);
        SHR_NULL_CHECK(rp_counter_id, SHR_E_PARAM);

        *gp_counter_id = entry_oper->g_counter_idx;
        *yp_counter_id = entry_oper->y_counter_idx;
        *rp_counter_id = entry_oper->r_counter_idx;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_entry_dump(
    int unit,
    bcm_field_entry_t entry)
{
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    int entry_id = 0;
    bcmlt_entry_handle_t rule_templ = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t grp_info_templ = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_templ = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t grp_info_qual_templ[4] = {BCMLT_INVALID_HDL};
    int ct = 0;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcm_field_qual_map_t *lt_qual_map = NULL;
    bcmlt_field_def_t *fld_defs_ref = NULL;
    bcmlt_field_def_t *fld_defs_iter = NULL;
    bcmlt_field_def_t *fld_defs_qual_info = NULL;
    int iter = 0;
    char *qual_field = NULL;
    uint64_t *qual_value_num_offset_buf = NULL;
    uint64_t *qual_value_offset_buf = NULL;
    uint64_t *qual_value_width_buf = NULL;
    uint32_t num_element = 0;
    int map_count= 0;
    uint8_t id = 0;
    bcm_field_action_t action = 0;
    bcm_field_action_params_t params;
    bcm_field_action_match_config_t match_config;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    uint64_t num_offsets = 0;
    uint8_t part_id = 0;
    uint64_t num_parts = 0;
    uint32_t offset = 0, width = 0;
    uint16_t offset_index = 0, width_index = 0;
    char buf[_SHR_PBMP_FMT_LEN];
    bool lt_offset_needed = 0;
    char *num_offset_field_name = NULL;
    char *offset_field_name = NULL;
    char *width_field_name = NULL;
    char *lt_field_name = NULL;
    int rv = 0;
    bcm_field_qualify_t pair_qual = bcmFieldQualifyCount;
    int class_id = -1;
    bcm_field_qualify_t pair_qual_arr[BCMINT_FP_ENTRY_MAX_AACL_CLASS_ID_CNT] =
         {bcmFieldQualifyCount, bcmFieldQualifyCount};
    bcm_field_qualify_t aacl_qual_arr[BCMINT_FP_ENTRY_MAX_AACL_CLASS_ID_CNT] =
         {bcmFieldQualifyCount, bcmFieldQualifyCount};
    bcm_field_qset_t grp_qset;
    int class_id_arr[BCMINT_FP_ENTRY_MAX_AACL_CLASS_ID_CNT] = {-1, -1};
    uint8 class_id_count = 0;
    uint8 indx = 0;
    uint64_t counter_id = 0;
    uint64_t gp_counter_id = 0;
    uint64_t yp_counter_id = 0;
    uint64_t rp_counter_id = 0;
    uint32_t mask_value = 0;
    bool stat_without_object = FALSE;
    bcmlt_entry_handle_t table_control_templ = BCMLT_INVALID_HDL;
    uint32_t count = 0;
    int dunit = 0;
    uint64_t hw_index = 0;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_entry_get(unit, entry,
                                           &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit, entry,
                                        stage_info,
                                        &entry_oper));

    entry_id = (entry & BCM_FIELD_ID_MASK);

    BCMINT_FIELD_MEM_ALLOC(fld_defs_qual_info,
                           sizeof(bcmlt_field_def_t),
                           "field definitions");

    tbls_info =  stage_info->tbls_info;

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
        dunit = bcmi_ltsw_dev_dunit(unit);

        /* Table control update */
          SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, TABLE_CONTROLs, &table_control_templ));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(table_control_templ,
                                          TABLE_IDs,
                                          tbls_info->entry_sid));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(table_control_templ, TABLE_OP_PT_INFOs, 1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(table_control_templ, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));

        (void)bcmlt_entry_free(table_control_templ);
        table_control_templ = BCMLT_INVALID_HDL;

        /* Entry lookup */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &entry_templ,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->entry_sid,
                                          tbls_info->entry_key_fid,
                                          0,
                                          entry_oper->entry_id,
                                          NULL,
                                          0, 0,
                                          0, 0,
                                          1));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, TABLE_OP_PT_INFOs, &table_control_templ));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(table_control_templ,
                                          TABLE_IDs,
                                          tbls_info->entry_sid));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_commit(table_control_templ, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_get(table_control_templ, PT_INDEXs, 0, &hw_index,
                                         1, &count));

        (void)bcmlt_entry_free(table_control_templ);
        table_control_templ = BCMLT_INVALID_HDL;
    }

    sal_memset(&params, 0 ,sizeof(bcm_field_action_params_t));

    LOG_CLI((BSL_META_U(unit,
                    "EID 0x%08x: eid=0x%x, gid=0x%x,\n"),
                    entry, entry_oper->entry_id,
                    entry_oper->group_id));
    LOG_CLI((BSL_META_U(unit,
               " prio=%#x, flags=%#x, hw_index=%#x, "),
               entry_oper->priority, entry_oper->entry_flags,
               COMPILER_64_LO(hw_index)));

    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED) {
        LOG_CLI((BSL_META_U(unit,
                            "Installed")));
    }
    if (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_DISABLED) {
        LOG_CLI((BSL_META_U(unit,
                            ", Disabled")));
    }

    /* Allocate and Read grp info template of source entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &grp_info_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      tbls_info->group_info_sid,
                                      tbls_info->group_info_key_fid,
                                      0, entry_oper->group_id, NULL,
                                      NULL,
                                      0,
                                      0, 0,
                                      0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(grp_info_templ,
                               tbls_info->group_info_num_parts_fid,
                               &num_parts));

    /* Allocate and Read rule template of source entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &rule_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      tbls_info->rule_sid,
                                      tbls_info->rule_key_fid,
                                      0, entry_id, NULL,
                                      NULL,
                                      0,
                                      0, 0,
                                      0));

    /* Allocate and Read grp info qual template of source entry */
    for (part_id = 1; part_id <= num_parts; part_id++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &grp_info_qual_templ[part_id - 1],
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->group_info_partition_sid,
                                          tbls_info->group_info_partition_key_fid,
                                          0, entry_oper->group_id, NULL,
                                          tbls_info->group_info_common_partition_key_fid,
                                          part_id,
                                          0, 0,
                                          0));
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmint_field_group_oper_lookup(unit,
                                       entry_oper->group_id,
                                       stage_info,
                                       &group_oper));
    BCM_FIELD_QSET_INIT(grp_qset);
    FP_HA_GROUP_OPER_QSET_ARR_GET(unit, group_oper, grp_qset);
    if ((group_oper->group_flags & BCMINT_FIELD_GROUP_COMPRESSION_ENABLED) &&
            (entry_oper->entry_flags & BCMINT_FIELD_ENTRY_INSTALLED)) {
        for (ct = 0; ct < group_oper->qual_cnt; ct++) {
            if (BCMINT_FIELD_IS_PSEUDO_QUAL((int)group_oper->qset_arr[ct])) {
                continue;
            }
            if (BCMINT_FIELD_IS_IP_QUAL(group_oper->qset_arr[ct])) {
                class_id = -1;
                pair_qual = bcmFieldQualifyCount;
                rv = mbcm_ltsw_field_aacl_class_id_get(unit,
                                   group_oper->qset_arr[ct],
                                   stage_info, entry_oper,
                                   &class_id, &pair_qual);
                if (SHR_SUCCESS(rv) && (class_id != -1)) {
                    if (class_id_count <
                            BCMINT_FP_ENTRY_MAX_AACL_CLASS_ID_CNT) {
                        pair_qual_arr[class_id_count] = pair_qual;
                        aacl_qual_arr[class_id_count] =
                                     group_oper->qset_arr[ct];
                        class_id_arr[class_id_count] = class_id;
                        class_id_count++;
                    }
;
                }
            }
        }
    }

    for (ct = 0; ct < group_oper->qual_cnt; ct++) {
        if (BCMINT_FIELD_IS_PSEUDO_QUAL((int)group_oper->qset_arr[ct])) {
            continue;
        }
        lt_offset_needed = 0;
        LOG_CLI((BSL_META_U(unit,
            "\n %s\n  HW info: "),
            bcmint_field_qual_name(group_oper->qset_arr[ct])));

        offset_index = 0;
        width_index = 0;

       /* Find lt map from db for given qualifier in group qset */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_qual_map_find(unit, group_oper->qset_arr[ct],
                                        stage_info, 0, 0, &map_count,
                                        &lt_qual_map));
        sal_memset(fld_defs_qual_info, 0, sizeof(bcmlt_field_def_t));

        BCMINT_FIELD_MEM_ALLOC
            (fld_defs_ref,
            sizeof(bcmlt_field_def_t) * (lt_qual_map->num_maps),
            "field definitions");

        fld_defs_iter = fld_defs_ref;

        for (iter = (lt_qual_map->num_maps - 1); iter >=0 ; iter--) {
            if (FALSE == bcmint_field_map_supported(unit, stage_info,
                (lt_qual_map->map[iter]))) {
                fld_defs_iter++;
                continue;
            }

            if (group_oper->qset_arr[ct] == bcmFieldQualifyGroupClass) {
                lt_offset_needed = 1;
                break;
            }
            lt_field_name = NULL;
            /* Get Field Qualifier name for group template */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                   stage_info, (const bcm_field_qset_t *) &grp_qset,
                   &(lt_qual_map->map[iter]),
                   &lt_field_name));

            /* Retreive field definition for provided lt field in map */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    tbls_info->rule_sid,
                    lt_field_name, fld_defs_iter));
            if (0 == fld_defs_iter->elements) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            for (part_id = 1; part_id <= num_parts; part_id++) {

                BCMINT_FIELD_MEM_ALLOC
                    (num_offset_field_name,
                    (strlen(lt_field_name) + 15),
                    "qualifier field num offset");
                BCMINT_FIELD_MEM_ALLOC
                    (offset_field_name,
                    (strlen(lt_field_name) + 15),
                    "qualifier field offsets");
                BCMINT_FIELD_MEM_ALLOC
                    (width_field_name,
                    (strlen(lt_field_name) + 15),
                    "qualifier field widths");
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_field_qual_info_lt_name_get(
                        unit,
                        lt_field_name,
                        num_offset_field_name,
                        offset_field_name,
                        width_field_name));

                /* Retreive field definition for provided lt field in map */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit,
                        tbls_info->group_info_partition_sid,
                        num_offset_field_name, fld_defs_qual_info));
                if (fld_defs_qual_info->elements > 1) {
                    BCMINT_FIELD_MEM_ALLOC
                        (qual_value_num_offset_buf,
                        sizeof(uint64_t) * (fld_defs_qual_info->elements),
                        "qualifier num offset");

                    num_element = fld_defs_qual_info->elements;

                    /* Retreive from qualifier value */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_array_get(
                                             grp_info_qual_templ[part_id-1],
                                             num_offset_field_name,
                                             0, qual_value_num_offset_buf,
                                             num_element,
                                             &num_element));
                    num_offsets = qual_value_num_offset_buf[0];

                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(grp_info_qual_templ[part_id-1],
                                           num_offset_field_name,
                                           &num_offsets));
                }

                /* Retreive field definition for provided lt field in map */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit,
                        tbls_info->group_info_partition_sid,
                        offset_field_name, fld_defs_qual_info));

                BCMINT_FIELD_MEM_ALLOC
                    (qual_value_offset_buf,
                    sizeof(uint64_t) * (fld_defs_qual_info->elements),
                    "qualifier offset");

                num_element = fld_defs_qual_info->elements;

                /* Retreive from qualifier value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(grp_info_qual_templ[part_id-1],
                                             offset_field_name,
                                             0, qual_value_offset_buf,
                                             num_element,
                                             &num_element));

                /* Retreive field definition for provided lt field in map */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit,
                        tbls_info->group_info_partition_sid,
                        width_field_name, fld_defs_qual_info));

                BCMINT_FIELD_MEM_ALLOC
                    (qual_value_width_buf,
                    sizeof(uint64_t) * (fld_defs_qual_info->elements),
                    "qualifier width");

                num_element = fld_defs_qual_info->elements;

                /* Retreive from qualifier value */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_array_get(grp_info_qual_templ[part_id-1],
                                             width_field_name,
                                             0, qual_value_width_buf,
                                             num_element,
                                             &num_element));

                for (id = 0; id < num_offsets; id++) {
                     offset = COMPILER_64_LO(qual_value_offset_buf[id]);
                     width = COMPILER_64_LO(qual_value_width_buf[id]);
                     LOG_CLI((BSL_META_U(unit,
                               "\n    Part: %d Offset%d: %d Width%d: %d"),
                               (part_id - 1), offset_index,
                               offset, width_index, width));
                     offset_index++;
                     width_index++;
                }
                for (id = 0; id < lt_qual_map->map[iter].num_offsets;
                 id++) {
                    if ((lt_qual_map->map[iter].width[id]) != 0) {
                        lt_offset_needed = 1;
                    }
                }
                BCMINT_FIELD_MEM_FREE(num_offset_field_name);
                BCMINT_FIELD_MEM_FREE(offset_field_name);
                BCMINT_FIELD_MEM_FREE(width_field_name);
                BCMINT_FIELD_MEM_FREE(qual_value_offset_buf);
                BCMINT_FIELD_MEM_FREE(qual_value_num_offset_buf);
                BCMINT_FIELD_MEM_FREE(qual_value_width_buf);
            }
            fld_defs_iter++;
        } /* end of for */

        if (lt_offset_needed) {
            LOG_CLI((BSL_META_U(unit,
                    "\n  LT field info:")));

            fld_defs_iter = fld_defs_ref;
            for (iter = (lt_qual_map->num_maps - 1); iter >=0 ; iter--) {
                if (FALSE == bcmint_field_map_supported(unit, stage_info,
                    (lt_qual_map->map[iter]))) {
                    fld_defs_iter++;
                    continue;
                }

                lt_field_name = NULL;
                /* Get Field Qualifier name for group template */
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                       stage_info, (const bcm_field_qset_t *) &grp_qset,
                       &(lt_qual_map->map[iter]),
                       &lt_field_name));

                /* Retreive field definition for provided lt field in map */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit,
                        tbls_info->rule_sid,
                        lt_field_name, fld_defs_iter));

                if (0 == fld_defs_iter->elements) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }

                for (id = 0; id < lt_qual_map->map[iter].num_offsets;
                     id++) {
                     offset = (lt_qual_map->map[iter].offset[id]);
                     width = (lt_qual_map->map[iter].width[id]);
                     LOG_CLI((BSL_META_U(unit,
                               "\n    Field: %d Offset%d: %d Width%d: %d"),
                               iter, id , offset, id, width));
                }
                fld_defs_iter++;
            } /* end of for */
        }

        LOG_CLI((BSL_META_U(unit,
                "\n     DATA=")));

        fld_defs_iter = fld_defs_ref;
        for (iter = (lt_qual_map->num_maps - 1); iter >=0 ; iter--) {
            if (FALSE == bcmint_field_map_supported(unit, stage_info,
                (lt_qual_map->map[iter]))) {
                fld_defs_iter++;
                continue;
            }
            lt_field_name = NULL;
            /* Get Field Qualifier name for group template */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                    stage_info, (const bcm_field_qset_t *) &grp_qset,
                    &(lt_qual_map->map[iter]),
                    &lt_field_name));

            /* Retreive field definition for provided lt field in map */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    tbls_info->rule_sid,
                    lt_field_name, fld_defs_iter));

            if (0 == fld_defs_iter->elements) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            BCMINT_FIELD_MEM_ALLOC
                (qual_field,
                (strlen(lt_field_name) + 1),
                "qualifier field");

            sal_strcpy(qual_field, lt_field_name);

            /* Print field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_print_fields(unit,
                                         &rule_templ,
                                         fld_defs_iter,
                                         qual_field));
            BCMINT_FIELD_MEM_FREE(qual_field);
            fld_defs_iter++;
        } /* end of for */

        LOG_CLI((BSL_META_U(unit,
                 "\n     MASK=")));

        fld_defs_iter = fld_defs_ref;
        for (iter = (lt_qual_map->num_maps - 1); iter >=0 ; iter--) {
            if (FALSE == bcmint_field_map_supported(unit, stage_info,
                (lt_qual_map->map[iter]))) {
                fld_defs_iter++;
                continue;
            }
            lt_field_name = NULL;
            /* Get Field Qualifier name for group template */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                    stage_info, (const bcm_field_qset_t *) &grp_qset,
                    &(lt_qual_map->map[iter]),
                    &lt_field_name));
            /* Retreive field definition for provided lt field in map */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    tbls_info->rule_sid,
                    lt_field_name, fld_defs_iter));

            if (0 == fld_defs_iter->elements) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }
            if (stage_info->flags & BCMINT_FIELD_STAGE_DEFAULT_ENTRY_SUPPORT) {
                for (indx = 0; indx < fld_defs_iter->elements; indx++) {
                    if (fld_defs_ref->width > 32) {
                        mask_value = ((1 << (fld_defs_iter->width - 32)) - 1);
                        LOG_CLI((BSL_META_U(unit,
                                   "%08x "), (mask_value)));
                        mask_value = 0xFFFFFFFF;
                        LOG_CLI((BSL_META_U(unit,
                                   "%08x "), (mask_value)));
                    } else {
                        mask_value = ((1 << fld_defs_iter->width) - 1);
                        LOG_CLI((BSL_META_U(unit,
                                   "%08x "), (mask_value)));
                    }
                }
            } else {
                BCMINT_FIELD_MEM_ALLOC
                   (qual_field,
                   (strlen(lt_field_name) + 6),
                   "qualifier mask field");

                sal_strcpy(qual_field, lt_field_name);
                sal_strcat(qual_field,"_MASK");
                /* Print field */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_print_fields(unit,
                                             &rule_templ,
                                             fld_defs_iter,
                                             qual_field));
                BCMINT_FIELD_MEM_FREE(qual_field);
            }
            fld_defs_iter++;
        } /* end of for */

        BCMINT_FIELD_MEM_FREE(fld_defs_ref);

        for (indx = 0; indx < class_id_count; indx++) {
            if ((class_id_arr[indx] != -1) &&
                (aacl_qual_arr[indx] == group_oper->qset_arr[ct])) {
                LOG_CLI((BSL_META_U(unit,
                         "\n     AACL Compression Class ID  : %d\n"),
                         class_id_arr[indx]));
            } else if ((class_id_arr[indx] != -1) &&
                       (pair_qual_arr[indx] ==
                          group_oper->qset_arr[ct])) {
                LOG_CLI((BSL_META_U(unit,
                         "\n     AACL Compression Class ID  : %d (paired)\n"),
                         class_id_arr[indx]));
            }
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    /* loop through all actions and see if action is configured in entry*/
    for (ct = 0; ct < group_oper->action_cnt; ct++) {
        action = group_oper->aset_arr[ct];

        if ((bcmFieldActionStatGroup == action) ||
            (bcmFieldActionPolicerGroup == action)) {
            /* will be handled later */
            continue;
        }

        if (bcmFieldActionStatGroupWithoutCounterIndex == action) {
            stat_without_object = TRUE;
        }
        if (bcmint_field_action_config_info_get(unit,
                                            entry,
                                            action, NULL,
                                            &params,
                                            &match_config) != SHR_E_NONE) {
            continue;
        }

        LOG_CLI((BSL_META_U(unit,
                    "         action = ")));
        LOG_CLI((BSL_META("{act=%s, "
                          "param0=%d(%#x), param1=%d(%#x),"
                          " param2=%d(%#x)"),
                          bcmint_field_action_name(action),
                          params.param0, params.param0,
                          params.param1, params.param1,
                          params.param2, params.param2));
        if (BCM_PBMP_NOT_NULL(params.pbmp)) {
            LOG_CLI((BSL_META(", pbmp=%s}\n"),
                              _SHR_PBMP_FMT(params.pbmp, buf)));
        } else {
            LOG_CLI((BSL_META("\n")));
        }
    }

    if (entry_oper->flexctr_action_id) {

        if (stat_without_object) {
            if (stage_info->flags & BCMINT_FIELD_STAGE_DEFAULT_ENTRY_SUPPORT) {
                counter_id = entry;
            } else {
                counter_id = 0;  
            }
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_field_flex_counter_info_get(unit,
                                                  entry_oper,
                                                  group_oper,
                                                  stage_info,
                                                  &counter_id,
                                                  &gp_counter_id,
                                                  &rp_counter_id,
                                                  &yp_counter_id));
        }

        LOG_CLI((BSL_META_U(unit,
                            "         flex_counter = ")));

        if (entry_oper->counter_color == 0) {
            /* Display entry's counter. */
            LOG_CLI((BSL_META_U(unit,
                                "{flexctr_action_id=%d(%#x), counter_id=%d(%#x)}"),
                                entry_oper->flexctr_action_id,
                                entry_oper->flexctr_action_id,
                                COMPILER_64_LO(counter_id),
                                COMPILER_64_LO(counter_id)));
        } else {
            /* Display entry's counter. */
            LOG_CLI((BSL_META_U(unit,
                                "{flexctr_action_id=%d(%#x), gp_counter_id=%d(%#x),"
                                " yp_counter_id=%d(%#x), rp_counter_id=%d(%#x)}"),
                                entry_oper->flexctr_action_id,
                                entry_oper->flexctr_action_id,
                                COMPILER_64_LO(gp_counter_id),
                                COMPILER_64_LO(gp_counter_id),
                                COMPILER_64_LO(yp_counter_id),
                                COMPILER_64_LO(yp_counter_id),
                                COMPILER_64_LO(rp_counter_id),
                                COMPILER_64_LO(rp_counter_id)));
        }
        LOG_CLI((BSL_META_U(unit,
                    "\n")));

    }

    if (entry_oper->policer_id) {

        LOG_CLI((BSL_META_U(unit,
                            "         policer = ")));

        /* Display entry's meter. */
        LOG_CLI((BSL_META_U(unit,
                            "{policer id %d}"), entry_oper->policer_id));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

exit:
    if (BCMLT_INVALID_HDL != rule_templ) {
        (void) bcmlt_entry_free(rule_templ);
    }
    if (BCMLT_INVALID_HDL != grp_info_templ) {
        (void) bcmlt_entry_free(grp_info_templ);
    }
    for (iter = 0; iter < 4; iter++) {
        if (BCMLT_INVALID_HDL != grp_info_qual_templ[iter]) {
            (void) bcmlt_entry_free(grp_info_qual_templ[iter]);
        }
    }
    if (qual_field != NULL) {
        BCMINT_FIELD_MEM_FREE(qual_field);
    }
    if (fld_defs_ref != NULL) {
        BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    }
    if (qual_value_width_buf != NULL) {
        BCMINT_FIELD_MEM_FREE(qual_value_width_buf);
    }
    if (num_offset_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(num_offset_field_name);
    }
    if (offset_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(offset_field_name);
    }
    if (width_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(width_field_name);
    }
    if (qual_value_offset_buf != NULL) {
        BCMINT_FIELD_MEM_FREE(qual_value_offset_buf);
    }
    if (qual_value_num_offset_buf != NULL) {
        BCMINT_FIELD_MEM_FREE(qual_value_num_offset_buf);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static int
bcmint_field_group_info_get(
    int unit,
    bcm_field_group_t group,
    bool entry_present,
    bcmlt_entry_handle_t *grp_info_templ)
{
    int rv;
    int entry = 0;
    bcm_field_entry_config_t entry_config;
    int gid = 0;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bool entry_created = FALSE;

    SHR_FUNC_ENTER(unit);

    /* Validate the group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                           &stage_info));
    gid = (group & BCM_FIELD_ID_MASK);
    tbls_info = stage_info->tbls_info;

    /* Create dummy entry if group has no entries. */
    if (!entry_present) {
        entry = stage_info->entry_info->eid_max;
        for (; entry > 0; entry--) {
            if (SHR_BITGET(stage_info->entry_info->eid_bmp.w, entry)) {
                continue;
            }
            break;
        }

        if (entry == 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }
        entry = stage_info->stage_base_id | entry;

        bcm_field_entry_config_t_init(&entry_config);
        entry_config.group_id = group;
        entry_config.entry_id = entry;
        entry_config.flags |= BCM_FIELD_ENTRY_CREATE_WITH_ID;

        rv = bcmint_field_entry_config_create(unit,
                                         &entry_config);

        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }

        entry_created = TRUE;

        rv = (bcmint_field_entry_install(unit, entry));
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    /* Allocate and Read grp info template of source entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, grp_info_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      tbls_info->group_info_sid,
                                      tbls_info->group_info_key_fid,
                                      0, gid, NULL,
                                      NULL,
                                      0,
                                      0, 0,
                                      0));
exit:
    /* Destroy the dummy entry */
    if (entry_created) {
        rv = bcmint_field_entry_destroy(unit, entry, NULL);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcmint_field_group_dump(
    int unit,
    bcm_field_group_t group,
    bool brief)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    int gid = 0;
    int pipe = 0;
    bcm_field_group_mode_t mode;
    int enable = 0;
    uint64_t action_res_id = -1;
    int ct = 0;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    int oper_mode;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    int idx = 0;
    bcm_field_entry_t eid = 0;
    bcmlt_entry_handle_t grp_info_templ = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t grp_qual_info_templ = BCMLT_INVALID_HDL;
    uint64_t entries_created = 0, entries_tentative = 0;
    uint64_t num_parts = 0;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    uint64_t slice[BCMINT_INFO_MAX] = {0};
    uint64_t num_slice = 0;
    uint32_t num_slices = 0;
    uint8_t id = 0, slice_id = 0;
    uint32_t value = 0;
    bcm_pbmp_t pbmp;
    char buf[_SHR_PBMP_FMT_LEN];
    bool entry_present = FALSE;
    bcm_field_stage_t stage = bcmFieldStageCount;
    bcm_pbmp_t pbmp_slice;
    bcm_field_stage_slice_info_t slice_info;
    uint32_t num_meters = 0;
    uint32_t num_meters_total = 0;
    uint32_t num_meters_free = 0;
    uint32_t num_counters_total = 0;
    uint32_t num_counters_free = 0;
    uint32_t num_counters_in_use = 0;
    bcm_field_group_mode_type_t mode_type = bcmFieldGroupModeTypeAuto;
    bcm_field_group_packet_type_t packet_type = bcmFieldGroupPacketTypeDefault;
    bcm_field_qset_t grp_qset;
    int dunit = 0;
    static char *mode_text[bcmFieldGroupModeTypeCount] = BCM_FIELD_GROUP_MODE_TYPE_STRINGS;
    static char *pkt_text[bcmFieldGroupPacketTypeCount] = BCM_FIELD_GROUP_PACKET_TYPE_STRINGS;

    SHR_FUNC_ENTER(unit);

    /* Validate the group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                           &stage_info));
    gid = (group & BCM_FIELD_ID_MASK);
    tbls_info = stage_info->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit,
                                           stage_info,
                                           &oper_mode));

    if (oper_mode == bcmiFieldOperModePipeUnique) {
        bcmi_field_ha_group_oper_t *group_oper = NULL;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_oper_lookup(unit,
                                        (group & BCM_FIELD_ID_MASK),
                                        stage_info,
                                        &group_oper));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_pipe_get(unit,
                                         stage_info,
                                         group_oper,
                                         &pipe));
    } else {
        pipe = -1;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_mode_get(unit, group,
                                     &mode));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_enable_get(unit, group,
                                       &enable));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_field_group_is_empty(unit, group,
                                   &entry_present));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_info_get(unit, group,
                                   entry_present,
                                   &grp_info_templ));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(grp_info_templ,
                               tbls_info->group_info_num_parts_fid,
                               &num_parts));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(grp_info_templ,
                               tbls_info->group_info_entries_created_fid,
                               &entries_created));

    if (NULL != tbls_info->group_info_entries_tentative_fid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(grp_info_templ,
                                   tbls_info->group_info_entries_tentative_fid,
                                   &entries_tentative));
    }
    if (NULL != tbls_info->group_info_act_res_fid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(grp_info_templ,
                                   tbls_info->group_info_act_res_fid,
                                   &action_res_id));
    }

    if (!entry_present) {
        entries_created = entries_created - 1;
        entries_tentative = entries_tentative + 1;
    }

    LOG_CLI((BSL_META_U(unit,
             "GID 0x%08x: gid=0x%x, instance=%d mode=%s, stage=%s "),
             group, gid, pipe,
             bcmint_field_group_mode_name(mode),
             bcmint_field_stage_name(stage_info->stage_id)));
    if (enable) {
        LOG_CLI((BSL_META_U(unit,"lookup=Enabled, ")));
    } else {
        LOG_CLI((BSL_META_U(unit,"lookup=Disabled, ")));
    }

    LOG_CLI((BSL_META_U(unit,
                        "ActionResId={%d}, "),
                        COMPILER_64_LO(action_res_id)));

    if (stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED) {
        dunit = bcmi_ltsw_dev_dunit(unit);
        BCM_FIELD_QSET_INIT(grp_qset);
        BCMINT_FIELD_QUAL_ARR_TO_QSET(group_oper->qual_cnt,
                                      group_oper->qset_arr,
                                      grp_qset);
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_egr_group_qset_validate(dunit,
                                                     gid,
                                                     stage_info,
                                                     grp_qset,
                                                     &mode_type,
                                                     &packet_type));
        LOG_CLI((BSL_META_U(unit,
                            "\n         Mode_type={%s}, Packet_type={%s},"),
                            mode_text[mode_type],
                            pkt_text[packet_type]));

    }
    BCM_PBMP_CLEAR(pbmp);
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmint_field_group_ports_get(unit, group, &pbmp));

    LOG_CLI((BSL_META_U(unit,
                        "\n         Pbmp={%s}, "),
                        _SHR_PBMP_FMT(pbmp, buf)));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmint_field_group_oper_lookup(unit,
                                       gid,
                                       stage_info,
                                       &group_oper));


    LOG_CLI((BSL_META_U(unit,"\n         qset={")));
    for (ct = 0; ct < group_oper->qual_cnt; ct++) {
        if (BCMINT_FIELD_IS_PSEUDO_QUAL((int)group_oper->qset_arr[ct])) {
            continue;
        }
        if (ct != 0) {
           LOG_CLI((BSL_META_U(unit, ", ")));
        }
        LOG_CLI((BSL_META_U(unit,
            "%s "),  bcmint_field_qual_name(group_oper->qset_arr[ct])));
    }
    LOG_CLI((BSL_META_U(unit,"},")));

    LOG_CLI((BSL_META_U(unit,"\n         aset={")));
    for (ct = 0; ct < group_oper->action_cnt; ct++) {
        if (ct != 0) {
           LOG_CLI((BSL_META_U(unit, ", ")));
        }
        LOG_CLI((BSL_META_U(unit,
            "%s "),  bcmint_field_action_name(group_oper->aset_arr[ct])));
    }
    LOG_CLI((BSL_META_U(unit,"},")));

    LOG_CLI((BSL_META_U(unit,
                    "\n         preselectors={")));
    for (ct = 0; ct < group_oper->presel_cnt; ct++) {
        if (ct != 0) {
           LOG_CLI((BSL_META_U(unit, ", ")));
        }
        LOG_CLI((BSL_META_U(unit, "%d"), group_oper->presel_arr[ct]));
    }
    LOG_CLI((BSL_META_U(unit, "},\n\r")));
    LOG_CLI((BSL_META_U(unit,"         group_priority= %d"),
                        group_oper->priority));
    BCM_PBMP_CLEAR(pbmp_slice);

    switch (stage_info->stage_id) {
       case bcmiFieldStageIngress:
           stage = bcmFieldStageIngress;
           break;
       case bcmiFieldStageEgress:
           stage = bcmFieldStageEgress;
           break;
       case bcmiFieldStageVlan:
           stage = bcmFieldStageLookup;
           break;
       case bcmiFieldStageExactMatch:
       case bcmiFieldStageFlowTracker:
       default:
           break;
    }

    for (id = 1; id <= num_parts; id++) {
        /* Allocate and Read grp part info template of source entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &grp_qual_info_templ,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->group_info_partition_sid,
                                          tbls_info->group_info_partition_key_fid,
                                          0, gid, NULL,
                                          tbls_info->group_info_common_partition_key_fid,
                                          id,
                                          0, 0,
                                          0));
        if (tbls_info->group_info_num_slice_fid != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(grp_qual_info_templ,
                                   tbls_info->group_info_num_slice_fid,
                                   &num_slice));
            num_slices = COMPILER_64_LO(num_slice);

        }
        if (tbls_info->group_info_slice_fid != NULL && (num_slices != 0)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(grp_qual_info_templ,
                                         tbls_info->group_info_slice_fid,
                                         0, slice,
                                         num_slices,
                                         &num_slices));
        }
        if (id == 1) {
            LOG_CLI((BSL_META_U(unit,"\n         slice_primary=")));
        } else if (id == 2) {
            LOG_CLI((BSL_META_U(unit,"\n         slice_secondary=")));
        } else if (id == 3) {
            LOG_CLI((BSL_META_U(unit,"\n         slice_ternary=")));
        } else {
            LOG_CLI((BSL_META_U(unit,"\n         slice_quarternary=")));
        }

        for (slice_id = 0; slice_id < num_slices; slice_id++) {
            if (tbls_info->slice_info_sid != NULL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcm_ltsw_field_stage_slice_info_get(unit,
                                                         stage,
                                                         pbmp_slice,
                                                         slice[slice_id],
                                                         &slice_info));
            }
            value = COMPILER_64_LO(slice[slice_id]);
            LOG_CLI((BSL_META_U(unit," {slice_number=%d,"),
                value));
            if (tbls_info->slice_info_sid != NULL) {
                LOG_CLI((BSL_META_U(unit,
                    " entries_total=%d, entries_free=%d"),
                    slice_info.total_entries,
                    slice_info.free_entries));
            }
            LOG_CLI((BSL_META_U(unit," },")));
        }
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICER_SUPPORT) ||
        (stage_info->flags & BCMINT_FIELD_STAGE_LEGACY_POLICER_SUPPORT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_policer_num_meters_in_use_per_pool_get(
                                         unit,
                                         stage_info->stage_id,
                                         pipe,
                                         group_oper->policer_pool_id,
                                         &num_meters));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_meter_info_get(unit, stage_info,
                                         pipe,
                                         &num_meters_total,
                                         &num_meters_free));
    }

    if (stage_info->flags & BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_counter_info_get(unit, stage_info,
                                         gid, pipe,
                                         &num_counters_total,
                                         &num_counters_free,
                                         &num_counters_in_use));
    }

    LOG_CLI((BSL_META_U(unit,"\n         group_status= ")));
    value = COMPILER_64_LO(entries_created + entries_tentative);
    LOG_CLI((BSL_META(" entries_total=%d,"), value));
    value = COMPILER_64_LO(entries_tentative);
    LOG_CLI((BSL_META(" entries_free=%d,\n"), value));
    LOG_CLI((BSL_META("                        meters_total=%d,"),
         num_meters_total));
    LOG_CLI((BSL_META(" meters_free=%d,"),
         num_meters_free));
    LOG_CLI((BSL_META(" counters_total=%d, counters_free=%d}"),
         num_counters_total, num_counters_free));
    LOG_CLI((BSL_META_U(unit,"\n")));

    if (brief) {
        SHR_EXIT();
    }

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                entry_oper);
        if (entry_oper == NULL) {
            continue;
        }

        /* Search the list of entries */
        do {
            if (entry_oper->group_id == gid) {
                BCMINT_FIELD_STAGE_ENTRY_ID_GET(stage_info->stage_id,
                                                entry_oper->entry_id,
                                                eid);
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcm_ltsw_field_entry_dump(unit, eid));
            }
            FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
        } while (entry_oper != NULL);
    }

exit:
    if (BCMLT_INVALID_HDL != grp_info_templ) {
        (void) bcmlt_entry_free(grp_info_templ);
    }
    if (BCMLT_INVALID_HDL != grp_qual_info_templ) {
        (void) bcmlt_entry_free(grp_qual_info_templ);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_dump(
    int unit,
    bcm_field_group_t group)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_dump(unit, group, 0));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_show(
    int unit,
    const char *pfx)
{
    bcm_field_group_t group;
    int gid = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_ltsw_field_stage_t stage_id;
    bool brief = 0;
    int rv = SHR_E_NONE;
    bcmlt_entry_handle_t stage_info_templ = BCMLT_INVALID_HDL;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    uint32_t tcam_sz = 0;
    uint32_t tcam_slices = 0;
    uint32_t entry_id_max = 0;
    uint32_t entry_base_id = 0;
    uint64_t num_slices = 0;
    uint64_t num_entries = 0;
    bcm_pbmp_t pbmp;
    bcm_field_stage_slice_info_t slice_info;
    uint8_t slice_no = 0;
    bcm_field_stage_t stage = bcmFieldStageCount;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    if (sal_strcmp(pfx, "brief") == 0) {
        brief = 1;
    } else {
        brief = 0;
    }

    BCM_PBMP_CLEAR(pbmp);

    for (stage_id = bcmiFieldStageIngress;
         stage_id < bcmiFieldStageCount; stage_id++) {
         rv = bcmint_field_stage_info_get(unit, stage_id,
                                          &stage_info);
         if (SHR_FAILURE(rv)) {
             continue;
         }
         tbls_info = stage_info->tbls_info;
         tcam_sz = 0;
         tcam_slices = 0;
         num_slices = 0;
         num_entries = 0;

         switch (stage_id) {
            case bcmiFieldStageIngress:
                LOG_CLI((BSL_META_U(unit,
                                  "PIPELINE STAGE INGRESS\n")));
                stage = bcmFieldStageIngress;
                entry_base_id = BCM_FIELD_IFP_ID_BASE;
                break;
            case bcmiFieldStageEgress:
                LOG_CLI((BSL_META_U(unit,
                                  "PIPELINE STAGE EGRESS\n")));
                stage = bcmFieldStageEgress;
                entry_base_id = BCM_FIELD_EFP_ID_BASE;
                break;
            case bcmiFieldStageVlan:
                LOG_CLI((BSL_META_U(unit,
                                  "PIPELINE STAGE LOOKUP\n")));
                stage = bcmFieldStageLookup;
                entry_base_id = BCM_FIELD_VFP_ID_BASE;
                break;
            case bcmiFieldStageExactMatch:
                LOG_CLI((BSL_META_U(unit,
                                  "PIPELINE STAGE EXACT MATCH\n")));
                entry_base_id = BCM_FIELD_EM_ID_BASE;
                break;
            case bcmiFieldStageFlowTracker:
                LOG_CLI((BSL_META_U(unit,
                                  "PIPELINE STAGE FLOW TRACKER\n")));
                entry_base_id = BCM_FIELD_FT_ID_BASE;
                break;
            default:
                break;
        }

        if (NULL != tbls_info->stage_info_sid) {
            /* Allocate and Read stage info template of source entry */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(unit, &stage_info_templ,
                                              BCMLT_OPCODE_LOOKUP,
                                              BCMLT_PRIORITY_NORMAL,
                                              tbls_info->stage_info_sid,
                                              0, 0, 0, 0,0, 0, 0, 0, 0));

            if (NULL != tbls_info->stage_info_num_slices_fid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(stage_info_templ,
                                           tbls_info->stage_info_num_slices_fid,
                                           &num_slices));
                tcam_slices = COMPILER_64_LO(num_slices);
                for (slice_no = 0; slice_no < tcam_slices; slice_no++) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcm_ltsw_field_stage_slice_info_get(unit,
                                                             stage,
                                                             pbmp,
                                                             slice_no,
                                                             &slice_info));
                    tcam_sz += slice_info.total_entries;
                }
            }

            if (NULL != tbls_info->stage_info_num_entries_fid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(stage_info_templ,
                                           tbls_info->stage_info_num_entries_fid,
                                           &num_entries));
                entry_id_max = COMPILER_64_LO(num_entries);
            }

            if (BCMLT_INVALID_HDL != stage_info_templ) {
                (void) bcmlt_entry_free(stage_info_templ);
                stage_info_templ = BCMLT_INVALID_HDL;
            }
        }
        LOG_CLI((BSL_META_U(unit,
           "FP:\t      :tcam_sz=%d(%#x),"),
           tcam_sz, tcam_sz));

        LOG_CLI((BSL_META_U(unit,
            " tcam_slices=%d"), tcam_slices));

        LOG_CLI((BSL_META_U(unit,
            " EntryID range=%d-%d\n"),
             entry_base_id + 1,
             entry_base_id + entry_id_max));
   }

   for (stage_id = bcmiFieldStageIngress;
        stage_id < bcmiFieldStageCount; stage_id++) {
        rv = bcmint_field_stage_info_get(unit, stage_id,
                                         &stage_info);
        if (SHR_FAILURE(rv)) {
            continue;
        }
        for (gid = 1;
             gid <= ((stage_info->group_info->gid_max)-
             BCMINT_FIELD_GROUP_RESERVE_GID_COUNT); gid++) {
            if (!SHR_BITGET(stage_info->group_info->grpid_bmp.w, gid)) {
                continue;
            }
            BCMINT_FIELD_STAGE_ENTRY_ID_GET(stage_info->stage_id,
                                            gid,
                                            group);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_dump(unit, group, brief));
        }

    }
exit:
    if (BCMLT_INVALID_HDL != stage_info_templ) {
        (void) bcmlt_entry_free(stage_info_templ);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcmi_field_group_dump_brief(
    int unit,
    bcm_field_group_t group)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_dump(unit, group, 1));
exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

static
int bcmint_lt_field_value_extract(int unit,
                              const bcm_field_qual_map_info_t *map,
                              uint32_t inp_buf,
                              uint32_t *out_buf)
{
    int ofst_idx = 0;
    uint32_t op = 0;
    uint8_t width = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(out_buf, SHR_E_INTERNAL);

    if (map->width[0] == 0) {
        *out_buf = inp_buf;
        SHR_EXIT();
    }
    for (ofst_idx = 0;
        ofst_idx < map->num_offsets;
        ofst_idx++) {
        op = (((1<< map->width[ofst_idx]) - 1) &
                (inp_buf >> map->offset[ofst_idx]));
        if (ofst_idx > 0) {
            op = (op << width);
        }
        width = (width + map->width[ofst_idx]);
        *out_buf |= op;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();

}

int bcmint_field_presel_qset_get(
    int unit,
    bcmi_ltsw_field_stage_t stage_id,
    int id,
    bool presel_grp_info,
    bcm_field_qset_t *pqset)
{
    bcmlt_entry_handle_t presel_templ = BCMLT_INVALID_HDL;
    bcm_field_qset_t qset;
    int iter = 0;
    char *lt_field_name = NULL;
    bcm_field_qualify_t qual;
    bcmi_field_qualify_t qual_idx;
    bcmlt_field_def_t *fld_defs_ref = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    int rv = SHR_E_NONE;
    const bcm_field_qual_map_t *lt_map_db = NULL;
    uint64_t buf = 0;
    int map_count = 0;
    uint32_t inp_buf = 0, out_buf = 0;
    char *lt_field_name_mask = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t), "field definitions");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));
    tbls_info =  stage_info->tbls_info;

    if (presel_grp_info) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &presel_templ,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->presel_group_sid,
                                          tbls_info->presel_group_key_fid,
                                          0, id, NULL,
                                          NULL,
                                          0,
                                          0, 0,
                                          0));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &presel_templ,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->presel_entry_sid,
                                          tbls_info->presel_entry_key_fid,
                                          0, id, NULL,
                                          NULL,
                                          0,
                                          0, 0,
                                          0));
    }

    BCM_FIELD_QSET_INIT(qset);
    /* Retreive Presel LT map and get the QSET */
    for (iter = 0; iter < stage_info->presel_qual_db_count; iter++) {
         lt_map_db = stage_info->presel_qual_lt_map_db + iter;
         BCM_FIELD_QSET_ADD(qset, lt_map_db->qual);
    }


    BCM_FIELD_QSET_INIT(*pqset);
    for (qual_idx = 0; qual_idx < BCM_FIELD_QUALIFY_MAX; qual_idx++) {
        if (!SHR_BITGET(qset.w, qual_idx)) {
            continue;
        }
        qual = qual_idx;

        if (BCMINT_FIELD_IS_PSEUDO_QUAL((int)qual)) {
            continue;
        }

        /* Find lt map from db for given qualifier in group qset */
        rv = (bcmint_field_qual_map_find(unit, qual_idx,
                                         stage_info, 1, 0, &map_count,
                                         &lt_map_ref));
        if (SHR_FAILURE(rv)){
            continue;
        }

        /* Loop through all LT fields associated to a qualifier */
        for (iter = (lt_map_ref->num_maps - 1); iter >= 0; iter--) {
            if (FALSE == bcmint_field_map_supported(unit, stage_info,
                          (lt_map_ref)->map[iter])) {
                continue;
            }
            /* Init out buffer */
            out_buf = 0;

            sal_memset(fld_defs_ref, 0, sizeof(bcmlt_field_def_t));
            lt_field_name = NULL;
            /* Get Field Qualifier name for group template */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                   stage_info, pqset, &(lt_map_ref->map[iter]), &lt_field_name));

            /* Retreive field definition for LT field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    stage_info->tbls_info->presel_group_sid,
                    lt_field_name, fld_defs_ref));

            /* Validate field definitions for valid values */
            if (!(fld_defs_ref->elements > 0)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            if (presel_grp_info) {
                /* Validate presel group fields */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(presel_templ,
                                           lt_field_name,
                                           &buf));
            } else {
                BCMINT_FIELD_MEM_ALLOC
                    (lt_field_name_mask,
                    (strlen(lt_field_name) + 6),
                    "qualifier field mask");

                sal_strcpy(lt_field_name_mask, lt_field_name);
                sal_strcat(lt_field_name_mask, "_MASK");

                /* Validate presel entry fields */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(presel_templ,
                                           lt_field_name_mask,
                                           &buf));
                BCMINT_FIELD_MEM_FREE(lt_field_name_mask);
            }

            if (COMPILER_64_LO(buf) > 0) {
                inp_buf = COMPILER_64_LO(buf);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_lt_field_value_extract(unit,
                                               &(lt_map_ref->map[iter]),
                                               inp_buf,
                                               &out_buf));
                if (out_buf > 0) {
                    BCM_FIELD_QSET_ADD(*pqset, qual_idx);
                    break;
                }
            }
        }
    }
exit:
    if (fld_defs_ref != NULL) {
        BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    }
    if (BCMLT_INVALID_HDL != presel_templ) {
        (void) bcmlt_entry_free(presel_templ);
    }
    if (lt_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(lt_field_name);
    }
    if (lt_field_name_mask != NULL) {
        BCMINT_FIELD_MEM_FREE(lt_field_name_mask);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
static
int bcmint_field_presel_info_dump(
    int unit,
    bcmi_ltsw_field_stage_t stage_id,
    int presel_id)
{
    int iter = 0;
    char *lt_field_name = NULL;
    char *lt_field_name_mask = NULL;
    bcm_field_qualify_t qual;
    bcmi_field_qualify_t qual_idx;
    bcmlt_field_def_t *fld_defs_ref = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    int map_count = 0;
    bcmlt_entry_handle_t presel_templ = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t psg_info_templ = BCMLT_INVALID_HDL;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    int rv = SHR_E_NONE;
    bcm_field_qset_t presel_qset;
    bcmlt_field_def_t *fld_defs_qual_info = NULL;
    uint64_t num_offsets = 0;
    uint64_t *qual_value_offset_buf = NULL;
    uint64_t *qual_value_width_buf = NULL;
    uint32_t offset =0, width = 0;
    uint32_t offset_index = 0, width_index = 0;
    uint32_t num_element = 0, id = 0;
    bool lt_offset_needed = 0;
    bcm_field_action_params_t params;
    char *num_offset_field_name = NULL;
    char *offset_field_name = NULL;
    char *width_field_name = NULL;
    uint64_t entry_prio = 0;
    int presel_grp_id = 0;
    uint64_t hw_index = 0;
    bcmlt_entry_handle_t table_control_templ = BCMLT_INVALID_HDL;
    uint32_t count = 0;
    int dunit = 0;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    BCMINT_FIELD_MEM_ALLOC(fld_defs_qual_info,
                           sizeof(bcmlt_field_def_t),
                           "field definitions");

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t), "field definitions");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));
    tbls_info =  stage_info->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_presel_oper_lookup(unit, presel_id,
                                         stage_info,
                                         &presel_oper));
    presel_grp_id = presel_oper->group_id;

    if (SHR_FAILURE(rv) || (presel_grp_id == 0)) {
        LOG_CLI((BSL_META_U(unit,"\n")));
        SHR_EXIT();
    }

    /* Fetch presel grp qset from LT */
    rv = (bcmint_field_presel_qset_get(unit,
                                      stage_id,
                                      presel_grp_id,
                                      1,
                                      &presel_qset));
    if (rv == SHR_E_PARAM) {
        LOG_CLI((BSL_META_U(unit,"\n")));
        SHR_EXIT();
    }

    /* Table control update */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_CONTROLs, &table_control_templ));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(table_control_templ,
                                      TABLE_IDs,
                                      tbls_info->presel_entry_sid));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(table_control_templ, TABLE_OP_PT_INFOs, 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(table_control_templ, BCMLT_OPCODE_UPDATE,
                             BCMLT_PRIORITY_NORMAL));

    (void)bcmlt_entry_free(table_control_templ);
    table_control_templ = BCMLT_INVALID_HDL;

    /* Allocate and Read presel template of source entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &presel_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      tbls_info->presel_entry_sid,
                                      tbls_info->presel_entry_key_fid,
                                      0, presel_id, NULL,
                                      NULL,
                                      0,
                                      0, 0,
                                      0));

    if (NULL != tbls_info->presel_entry_prio_fid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(presel_templ,
                                   tbls_info->presel_entry_prio_fid,
                                   &entry_prio));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TABLE_OP_PT_INFOs, &table_control_templ));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(table_control_templ,
                                      TABLE_IDs,
                                      tbls_info->presel_entry_sid));

    rv = (bcmlt_entry_commit(table_control_templ, BCMLT_OPCODE_LOOKUP,
                             BCMLT_PRIORITY_NORMAL));

    /* Till entry is created in main group,
     * hw indexes cannot be retrieved.
     */
    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        (void)bcmlt_entry_free(table_control_templ);
        table_control_templ = BCMLT_INVALID_HDL;
        SHR_EXIT();
    }
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_get(table_control_templ, PT_INDEXs,
                                         0, &hw_index,
                                         1, &count));
    }

    (void)bcmlt_entry_free(table_control_templ);
    table_control_templ = BCMLT_INVALID_HDL;

    LOG_CLI((BSL_META_U(unit,
        "PRESEL ID:  %d, eid=0x%x, priority=%d\n\r"),
        presel_id, (presel_id | BCM_FIELD_QUALIFY_PRESEL),
        COMPILER_64_LO(entry_prio)));

    LOG_CLI((BSL_META_U(unit, "\tStage:%s"),
        bcmint_field_stage_name(stage_id)));

    LOG_CLI((BSL_META_U(unit, "\tHw index:%#x"),
        COMPILER_64_LO(hw_index)));

    /* Allocate and Read grp info qual template of source entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit, &psg_info_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      tbls_info->group_info_presel_sid,
                                      tbls_info->group_info_presel_key_fid,
                                      0, presel_grp_id, NULL,
                                      tbls_info->group_info_common_partition_key_fid,
                                      1,
                                      0, 0,
                                      0));

    LOG_CLI((BSL_META_U(unit,"\n         qset={")));
    for (qual_idx = 0; qual_idx < BCM_FIELD_QUALIFY_MAX; qual_idx++) {
        if (!SHR_BITGET(presel_qset.w, qual_idx)) {
            continue;
        }
        qual = qual_idx;

        if (BCMINT_FIELD_IS_PSEUDO_QUAL((int)qual)) {
            continue;
        }
        LOG_CLI((BSL_META_U(unit,
                "%s, "),  bcmint_field_qual_name(qual)));
    }
    LOG_CLI((BSL_META_U(unit,"},")));

    LOG_CLI((BSL_META_U(unit, "\nQualifiers =")));

    for (qual_idx = 0; qual_idx < BCM_FIELD_QUALIFY_MAX; qual_idx++) {
        if (!SHR_BITGET(presel_qset.w, qual_idx)) {
            continue;
        }
        qual = qual_idx;

        if (BCMINT_FIELD_IS_PSEUDO_QUAL((int)qual)) {
            continue;
        }

        /* Find lt map from db for given qualifier in group qset */
        rv = (bcmint_field_qual_map_find(unit, qual_idx,
                                         stage_info, 1, 0, &map_count,
                                         &lt_map_ref));
        if (SHR_FAILURE(rv)){
            continue;
        }

        sal_memset(fld_defs_qual_info, 0, sizeof(bcmlt_field_def_t));

        LOG_CLI((BSL_META_U(unit,
            "\n %s\n  HW info:"),
            bcmint_field_qual_name(qual)));

        offset_index = 0;
        width_index = 0;
        lt_offset_needed = 0;

        /* Loop through all LT fields associated to a qualifier */
        for (iter = (lt_map_ref->num_maps - 1); iter >= 0; iter--) {
            if (FALSE == bcmint_field_map_supported(unit, stage_info,
                          (lt_map_ref)->map[iter])) {
                continue;
            }

            sal_memset(fld_defs_ref, 0, sizeof(bcmlt_field_def_t));
            lt_field_name = NULL;
            /* Get Field Qualifier name for group template */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                   stage_info, NULL, &(lt_map_ref->map[iter]),
                   &lt_field_name));

            /* Retreive field definition for LT field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    stage_info->tbls_info->presel_group_sid,
                    lt_field_name, fld_defs_ref));

            /* Validate field definitions for valid values */
            if (!(fld_defs_ref->elements > 0)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            BCMINT_FIELD_MEM_ALLOC
                (num_offset_field_name,
                (strlen(lt_field_name) + 15),
                "qualifier field num offset");
            BCMINT_FIELD_MEM_ALLOC
                (offset_field_name,
                (strlen(lt_field_name) + 15),
                "qualifier field offsets");
            BCMINT_FIELD_MEM_ALLOC
                (width_field_name,
                (strlen(lt_field_name) + 15),
                "qualifier field widths");

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_qual_info_lt_name_get(
                    unit,
                    lt_field_name,
                    num_offset_field_name,
                    offset_field_name,
                    width_field_name));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(psg_info_templ,
                                       num_offset_field_name,
                                       &num_offsets));

            /* Retreive field definition for provided lt field in map */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    tbls_info->group_info_presel_sid,
                    offset_field_name, fld_defs_qual_info));

            BCMINT_FIELD_MEM_ALLOC
                (qual_value_offset_buf,
                sizeof(uint64_t) * (fld_defs_qual_info->elements),
                "qualifier offset");

            num_element = fld_defs_qual_info->elements;

            /* Retreive from qualifier value */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(psg_info_templ,
                                         offset_field_name,
                                         0, qual_value_offset_buf,
                                         num_element,
                                         &num_element));

            /* Retreive field definition for provided lt field in map */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    tbls_info->group_info_presel_sid,
                    width_field_name, fld_defs_qual_info));

            BCMINT_FIELD_MEM_ALLOC
                (qual_value_width_buf,
                sizeof(uint64_t) * (fld_defs_qual_info->elements),
                "qualifier width");

            num_element = fld_defs_qual_info->elements;

            /* Retreive from qualifier value */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_get(psg_info_templ,
                                         width_field_name,
                                         0, qual_value_width_buf,
                                         num_element,
                                         &num_element));

            for (id = 0; id < num_offsets; id++) {
                 offset = COMPILER_64_LO(qual_value_offset_buf[id]);
                 width = COMPILER_64_LO(qual_value_width_buf[id]);
                 LOG_CLI((BSL_META_U(unit,
                           "\n    Offset%d: %d Width%d: %d"),
                           offset_index, offset,
                           width_index, width));
                 offset_index++;
                 width_index++;
            }
            for (id = 0; id < lt_map_ref->map[iter].num_offsets;
                 id++) {
                if ((lt_map_ref->map[iter].width[id]) != 0) {
                    lt_offset_needed = 1;
                }
            }
            BCMINT_FIELD_MEM_FREE(num_offset_field_name);
            BCMINT_FIELD_MEM_FREE(offset_field_name);
            BCMINT_FIELD_MEM_FREE(width_field_name);
            BCMINT_FIELD_MEM_FREE(qual_value_offset_buf);
            BCMINT_FIELD_MEM_FREE(qual_value_width_buf);
        }

        if (lt_offset_needed) {
            LOG_CLI((BSL_META_U(unit,
                    "\n  LT field info:")));

            for (iter = (lt_map_ref->num_maps - 1); iter >=0 ; iter--) {
                if (FALSE == bcmint_field_map_supported(unit, stage_info,
                    (lt_map_ref->map[iter]))) {
                    continue;
                }

                sal_memset(fld_defs_ref, 0, sizeof(bcmlt_field_def_t));
                lt_field_name = NULL;

                /* Get Field Qualifier name for group template */
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                       stage_info, NULL, &(lt_map_ref->map[iter]),
                       &lt_field_name));

                /* Retreive field definition for LT field */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit,
                        stage_info->tbls_info->presel_group_sid,
                        lt_field_name, fld_defs_ref));

                /* Validate field definitions for valid values */
                if (!(fld_defs_ref->elements > 0)) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }

                for (id = 0; id < lt_map_ref->map[iter].num_offsets;
                     id++) {
                     offset = (lt_map_ref->map[iter].offset[id]);
                     width = (lt_map_ref->map[iter].width[id]);
                     LOG_CLI((BSL_META_U(unit,
                               "\n    Field: %d Offset%d:"
                               " %d Width%d: %d"),
                               iter, id , offset, id, width));
                }
            } /* end of for */
        }

        LOG_CLI((BSL_META_U(unit,
        "\n     DATA=")));

        /* Loop through all LT fields associated to a qualifier */
        for (iter = (lt_map_ref->num_maps - 1); iter >= 0; iter--) {
            if (FALSE == bcmint_field_map_supported(unit, stage_info,
                          (lt_map_ref)->map[iter])) {
                continue;
            }

            sal_memset(fld_defs_ref, 0, sizeof(bcmlt_field_def_t));
            lt_field_name = NULL;
            /* Get Field Qualifier name for group template */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                   stage_info, NULL, &(lt_map_ref->map[iter]),
                   &lt_field_name));

            /* Retreive field definition for LT field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    stage_info->tbls_info->presel_group_sid,
                    lt_field_name, fld_defs_ref));

            /* Validate field definitions for valid values */
            if (!(fld_defs_ref->elements > 0)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            /* Print field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_print_fields(unit,
                                         &presel_templ,
                                         fld_defs_ref,
                                         lt_field_name));
        }

        LOG_CLI((BSL_META_U(unit,
        "\n     MASK=")));

        /* Loop through all LT fields associated to a qualifier */
        for (iter = (lt_map_ref->num_maps - 1); iter >= 0; iter--) {
            if (FALSE == bcmint_field_map_supported(unit,
                         stage_info,
                         (lt_map_ref)->map[iter])) {
                continue;
            }

            sal_memset(fld_defs_ref, 0, sizeof(bcmlt_field_def_t));
            lt_field_name = NULL;
            /* Get Field Qualifier name for group template */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_lt_qual_name_update(unit, 0,
                   stage_info, NULL, &(lt_map_ref->map[iter]),
                   &lt_field_name));

            /* Retreive field definition for LT field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                    stage_info->tbls_info->presel_group_sid,
                    lt_field_name, fld_defs_ref));

            /* Validate field definitions for valid values */
            if (!(fld_defs_ref->elements > 0)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            BCMINT_FIELD_MEM_ALLOC
                (lt_field_name_mask,
                (strlen(lt_field_name) + 6),
                "qualifier field mask");

            sal_strcpy(lt_field_name_mask, lt_field_name);
            sal_strcat(lt_field_name_mask, "_MASK");

            /* Print field */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_print_fields(unit,
                                         &presel_templ,
                                         fld_defs_ref,
                                         lt_field_name_mask));

            BCMINT_FIELD_MEM_FREE(lt_field_name_mask);
        }
    }

    LOG_CLI((BSL_META_U(unit, "\n")));
    if ((bcmint_field_presel_action_get(unit,
                                       (presel_id | BCM_FIELD_QUALIFY_PRESEL),
                                       bcmFieldActionGroupClassSet,
                                       &params) == SHR_E_NONE) &&
                                       (params.param0 != 0)){

        LOG_CLI((BSL_META("         {act=%s, "
             "param0=%d(%#x), param1=%d(%#x),"
             " param2=%d(%#x)\n"),
             bcmint_field_action_name(bcmFieldActionGroupClassSet),
             params.param0, params.param0,
             params.param1, params.param1,
             params.param2, params.param2));
    }

exit:
    if (fld_defs_ref != NULL) {
        BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    }
    if (fld_defs_qual_info != NULL) {
        BCMINT_FIELD_MEM_FREE(fld_defs_qual_info);
    }
    if (BCMLT_INVALID_HDL != psg_info_templ) {
        (void) bcmlt_entry_free(psg_info_templ);
    }
    if (BCMLT_INVALID_HDL != presel_templ) {
        (void) bcmlt_entry_free(presel_templ);
    }
    if (lt_field_name_mask != NULL) {
        BCMINT_FIELD_MEM_FREE(lt_field_name_mask);
    }
    if (qual_value_width_buf != NULL) {
        BCMINT_FIELD_MEM_FREE(qual_value_width_buf);
    }
    if (qual_value_offset_buf != NULL) {
        BCMINT_FIELD_MEM_FREE(qual_value_offset_buf);
    }
    if (num_offset_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(num_offset_field_name);
    }
    if (offset_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(offset_field_name);
    }
    if (width_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(width_field_name);
    }
    if (lt_field_name != NULL) {
        BCMINT_FIELD_MEM_FREE(lt_field_name);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
int
bcmi_field_presel_dump(
    int unit,
    bcm_field_presel_t presel_id)
{
    int i = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcm_field_presel_t  act_presel_id = 0;
    bcmi_ltsw_field_stage_t stage_id = bcmiFieldStageCount;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    if (presel_id == -1) {
        for (stage_id = bcmiFieldStageIngress;
            stage_id < bcmiFieldStageCount; stage_id++) {
            rv = bcmint_field_stage_info_get(unit, stage_id,
                                          &stage_info);
            if (SHR_FAILURE(rv)) {
                continue;
            }
            for (i = 1; i < BCM_FIELD_PRESEL_SEL_MAX; i++) {
                /* Check whether there exists a valid presel with this ID */
                if (SHR_BITGET(
                    stage_info->presel_info->pid_bmp.w, i)) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_presel_info_dump(unit,
                                                     stage_id,
                                                     i));
                }
            }
        }
    } else {
        /* validate the entry range and retrieve the stage info */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_stage_from_presel_get(unit,
                                                presel_id,
                                                &stage_info));
        act_presel_id = presel_id & BCM_FIELD_ID_MASK;
        /* Verify whether the given ID is in Stage Entry ID's range */
        if ((presel_id > stage_info->stage_base_id) &&
            (presel_id <=
             stage_info->stage_base_id +
             stage_info->presel_info->pid_max)) {
            /* Check whether the corresponding presel entry ID already exists. */
            if (SHR_BITGET(stage_info->presel_info->pid_bmp.w, act_presel_id)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_presel_info_dump(unit,
                                                 stage_info->stage_id,
                                                 act_presel_id));
            }
        } else {
            LOG_CLI((BSL_META_U(unit,
                "Error: Presel entry ID is not in valid range\n")));
        }

    }

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function: bcm_ltsw_field_group_status_get
 *
 * Purpose:
 *     Get the number of used and available entries, counters, and
 *     meters for a field group.
 *
 * Paramters:
 *     unit - BCM device number
 *     group - Field group ID
 *     status - (OUT) Status structure
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit has not been intialized
 *     BCM_E_NOT_FOUND - Group ID not found on unit.
 *     BCM_E_PARAM     - *status is NULL
 *     BCM_E_NONE      - Success
 */

int
bcm_ltsw_field_group_status_get(int unit,
                               bcm_field_group_t group,
                               bcm_field_group_status_t *status)
{
    bcmlt_entry_handle_t grp_info_templ = BCMLT_INVALID_HDL;
    uint64_t entries_created = 0, entries_tentative = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    int gid = 0;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmi_field_ha_blk_info_t *entry_base = NULL;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;
    bool min_val_set = false;
    bool entry_present = FALSE;
    int idx = 0;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    int pipe = -1;
    uint32_t num_meters = 0;
    uint32_t num_meters_total = 0;
    uint32_t num_meters_free = 0;
    uint32_t num_counters_total = 0;
    uint32_t num_counters_free = 0;
    uint32_t num_counters_in_use = 0;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    sal_memset(status, 0x00, sizeof(bcm_field_group_status_t));
    /* Validate the group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                           &stage_info));
    gid = (group & BCM_FIELD_ID_MASK);
    tbls_info = stage_info->tbls_info;

    SHR_NULL_CHECK(tbls_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(tbls_info->group_info_sid, SHR_E_UNAVAIL);

    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit,
                                        gid,
                                        stage_info,
                                        &group_oper));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit, stage_info,
                                     group_oper, &pipe));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_field_group_is_empty(unit, group,
                                   &entry_present));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_info_get(unit, group,
                                   entry_present,
                                   &grp_info_templ));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(grp_info_templ,
                               tbls_info->group_info_entries_created_fid,
                               &entries_created));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(grp_info_templ,
                               tbls_info->group_info_entries_tentative_fid,
                               &entries_tentative));
    if (!entry_present) {
        entries_created = entries_created - 1;
        entries_tentative = entries_tentative + 1;
    }

    status->entry_count = COMPILER_64_LO(entries_created);
    status->entries_free = COMPILER_64_LO(entries_tentative);
    status->entries_total = COMPILER_64_LO(entries_created +
                                           entries_tentative);
    /* travere through entries of the group and find the min and max prio
     * set for the group entries
     */
    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                entry_oper);
        if (entry_oper == NULL) {
            continue;
        }

        do {
            if (entry_oper->group_id == gid) {
                if (status->prio_max < entry_oper->priority) {
                    status->prio_max = entry_oper->priority;
                }

                /*set the min value for first entry of the group*/
                if (!min_val_set) {
                    status->prio_min = entry_oper->priority;
                    min_val_set = true;
                }

                if (status->prio_min > entry_oper->priority) {
                    status->prio_min = entry_oper->priority;
                }
            }
            FP_HA_ENT_OPER_NEXT(unit, entry_oper, entry_oper);
        } while (entry_oper != NULL);
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICER_SUPPORT) ||
        (stage_info->flags & BCMINT_FIELD_STAGE_LEGACY_POLICER_SUPPORT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_policer_num_meters_in_use_per_pool_get(
                                         unit,
                                         stage_info->stage_id,
                                         pipe,
                                         group_oper->policer_pool_id,
                                         &num_meters));
        status->meter_count = num_meters;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_meter_info_get(unit, stage_info,
                                         pipe,
                                         &num_meters_total,
                                         &num_meters_free));
        status->meters_total = num_meters_total;
        status->meters_free = num_meters_free;

    }

    if (stage_info->flags & BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_counter_info_get(unit, stage_info,
                                         gid, pipe,
                                         &num_counters_total,
                                         &num_counters_free,
                                         &num_counters_in_use));
        status->counters_total = num_counters_total;
        status->counters_free = num_counters_free;
        status->counter_count = num_counters_in_use;
    }

exit:
    if (BCMLT_INVALID_HDL != grp_info_templ) {
        (void) bcmlt_entry_free(grp_info_templ);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
#endif

/*
 * Function: bcm_field_stage_slice_count_get
 *
 * Purpose:
 *     Return the number os slices in a given stage
 *
 * Parameters:
 *     unit       - (IN)  BCM device number
 *     stage_id   - (IN)  Stage ID
 *     slice_count- (OUT) number of slices in a given stage
 *
 * Returns:
 *     BCM_E_NONE    - Success
 *     BCM_E_CONFIG  - error in configuration
 *     BCM_E_PARAM   - invalid parameter
 */

int
bcm_ltsw_field_stage_slice_count_get(
    int unit,
    bcm_field_stage_t stage_id,
    int *slice_count) {

    bcmi_ltsw_field_stage_t stage = 0;
    bcmint_field_stage_info_t *stage_info;
    bcmlt_entry_handle_t stage_info_templ = BCMLT_INVALID_HDL;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    uint64_t num_slices = 0;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    if (slice_count == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    *slice_count = 0;

    switch(stage_id) {
        case bcmFieldStageIngress:
            stage = bcmiFieldStageIngress;
            break;
        case bcmFieldStageEgress:
            stage = bcmiFieldStageEgress;
            break;
        case bcmFieldStageLookup:
            stage = bcmiFieldStageVlan;
            break;
        case bcmFieldStageIngressExactMatch:
        case bcmFieldStageIngressFlowtracker:
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage, &stage_info));
    tbls_info = stage_info->tbls_info;
    if (NULL != tbls_info->stage_info_sid) {
            /* Allocate and Read stage info template of source entry */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(unit, &stage_info_templ,
                                              BCMLT_OPCODE_LOOKUP,
                                              BCMLT_PRIORITY_NORMAL,
                                              tbls_info->stage_info_sid,
                                              0, 0, 0, 0, 0, 0, 0, 0, 0));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(stage_info_templ,
                                       tbls_info->stage_info_num_slices_fid,
                                       &num_slices));
            *slice_count = COMPILER_64_LO(num_slices);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    if (BCMLT_INVALID_HDL != stage_info_templ) {
        (void) bcmlt_entry_free(stage_info_templ);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_stage_slice_info_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_pbmp_t pbmp,
    int slice_no,
    bcm_field_stage_slice_info_t *slice_info)
{
    bcmint_field_stage_info_t *stage_info;
    int oper_mode;
    uint8_t pipe = 0;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmlt_entry_handle_t slice_info_templ = BCMLT_INVALID_HDL;
    uint64_t num_entries_in_use = 0;
    uint64_t num_entries_max = 0;
    bcm_port_config_t port_config;
    bcmi_ltsw_field_stage_t stage_id = 0;

    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    switch(stage) {
        case bcmFieldStageIngress:
            stage_id = bcmiFieldStageIngress;
            break;
        case bcmFieldStageEgress:
            stage_id = bcmiFieldStageEgress;
            break;
        case bcmFieldStageLookup:
            stage_id = bcmiFieldStageVlan;
            break;
        case bcmFieldStageIngressExactMatch:
        case bcmFieldStageIngressFlowtracker:
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
    (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit,
                                           stage_info,
                                           &oper_mode));
    tbls_info = stage_info->tbls_info;

    if (tbls_info->slice_info_sid == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (oper_mode == bcmiFieldOperModePipeUnique) {
        if (BCM_PBMP_IS_NULL(pbmp)) {
            pipe = 0;
        } else {
            /* Get FP Pipe */
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_field_group_pipe_get(unit,
                                                stage_info,
                                                pbmp,
                                                &pipe));
        }
        /* Allocate and Read slice info template of source entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &slice_info_templ,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->slice_info_sid,
                                          tbls_info->slice_info_key_fid,
                                          0, slice_no, NULL,
                                          tbls_info->slice_info_pipe_fid,
                                          pipe,
                                          0, 0,
                                          0));
    } else {
        /* Retrieve port configuration */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_config_get(unit, &port_config));
        if ((BCM_PBMP_NEQ(pbmp, port_config.all)) &&
            (BCM_PBMP_NOT_NULL(pbmp))) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        /* Allocate and Read slice info template of source entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &slice_info_templ,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          tbls_info->slice_info_sid,
                                          tbls_info->slice_info_key_fid,
                                          0, slice_no, NULL,
                                          0, 0,
                                          0, 0,
                                          0));

    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(slice_info_templ,
                               tbls_info->slice_info_entry_in_use_fid,
                               &num_entries_in_use));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(slice_info_templ,
                               tbls_info->slice_info_entry_max_fid,
                               &num_entries_max));

    slice_info->total_entries = COMPILER_64_LO(num_entries_max);
    slice_info->free_entries = COMPILER_64_LO(num_entries_max
                               - num_entries_in_use);

exit:
    if (BCMLT_INVALID_HDL != slice_info_templ) {
        (void) bcmlt_entry_free(slice_info_templ);
    }
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();

}

/* Retrieve supported ASET for a given stage */
int
bcm_ltsw_field_stage_aset_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_aset_t *aset)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_aset_get(unit, stage, aset));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_action_cont_info_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_action_t action,
    uint8_t max_ct,
    uint16_t *cont_id_arr,
    uint16_t *cont_size_arr,
    uint8_t *actual_ct,
    uint8_t *action_size)
{
    SHR_FUNC_ENTER(unit);
    FP_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_cont_info_get(
            unit, stage, action,
            max_ct, cont_id_arr,
            cont_size_arr, actual_ct,
            action_size));

exit:
    FP_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_mode_qset_get(
    int unit,
    bcm_field_group_mode_type_t mode_type,
    bcm_field_group_packet_type_t packet_type,
    bcm_field_qset_t* qset)
{
    int key_mode = 0;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, bcmiFieldStageEgress,
                                     &stage_info));
    for (key_mode = 0; key_mode < BCMINT_FIELD_EFP_KEYS; key_mode++) {
        if ((stage_info->efp_key_qual_map[key_mode].mode_type == mode_type) &&
            (stage_info->efp_key_qual_map[key_mode].packet_type == packet_type)) {
            *qset = stage_info->efp_key_qual_map[key_mode].key_qset;
        }
    }

exit:
    SHR_FUNC_EXIT();
}
