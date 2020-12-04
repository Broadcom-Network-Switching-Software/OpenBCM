/*! \file bcmcth_ctr_eflex_obj_offset.c
 *
 * BCMCTH CTR_EFLEX_OBJ_INFOt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>

#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/*******************************************************************************
 * Public Functions
 */

/* Function to populate flex counter/state OBJ_INFO LT entries */
int
bcmcth_eflex_obj_info_populate(int unit, bool ingress, eflex_comp_t comp)
{
    int rv;
    bcmltd_fields_t obj_info_lt_flds = {0};
    size_t num_fields = 0, f_cnt, count;
    uint32_t num_obj, i;
    uint32_t *obj = NULL;
    bcmlrd_fid_t   *fid_list = NULL;
    bcmcth_ctr_eflex_obj_bit_offset_t *obj_info = NULL;
    const bcmlrd_map_t *map = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_obj_info_lt_info_t *lt_info = NULL;
    bool ent_limit = false;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get LT sid and fid info */
    lt_info = ingress ? &ctrl->ing_obj_info_lt_info :
                        &ctrl->egr_obj_info_lt_info;

    if (lt_info->supported) {
        /* Check if LT is supported for given device */
        rv = bcmlrd_map_get(unit, lt_info->lt_sid, &map);

        if (SHR_SUCCESS(rv) && map) {
            /* Get CTR_XXX_EFLEX_OBJ_INFO IMM LT entry_limit value */
            for (i = 0; i < map->table_attr->attributes; i++) {
                if (map->table_attr->attr[i].key ==
                        BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT) {
                    num_obj = map->table_attr->attr[i].value;
                    ent_limit = true;
                    break;
                }
            }

            /* MAX_OBJ_NUM used to be default value of num_obj */
            if (ent_limit == false) {
                num_obj = MAX_OBJ_NUM;
            }

            /* Initialize array of obj */
            SHR_ALLOC(obj,
                      sizeof(uint32_t) * num_obj,
                      "bcmcthCtrEFlexObjArray");
            SHR_NULL_CHECK(obj, SHR_E_MEMORY);
            sal_memset(obj, 0, sizeof(uint32_t) * num_obj);

            /* Initialize array of obj_info */
            SHR_ALLOC(obj_info,
                      sizeof(bcmcth_ctr_eflex_obj_bit_offset_t) * num_obj,
                      "bcmcthCtrEFlexObjInfoArray");
            SHR_NULL_CHECK(obj_info, SHR_E_MEMORY);
            sal_memset(obj_info,
                       0,
                       sizeof(bcmcth_ctr_eflex_obj_bit_offset_t) * num_obj);

            /* Get number of fields. */
            SHR_IF_ERR_EXIT
                (bcmlrd_field_count_get(unit, lt_info->lt_sid, &num_fields));

            SHR_ALLOC(fid_list, sizeof(bcmlrd_fid_t) * num_fields,
                "bcmcthCtrEFlexObjInfoFields");
            SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
            sal_memset(fid_list, 0, sizeof(bcmlrd_fid_t) * num_fields);

            SHR_IF_ERR_EXIT
                (bcmlrd_field_list_get(unit,
                                       lt_info->lt_sid,
                                       num_fields,
                                       fid_list,
                                       &count));

            obj_info_lt_flds.field = NULL;

            SHR_IF_ERR_EXIT
                (bcmcth_ctr_eflex_fields_alloc(unit,
                                               &obj_info_lt_flds,
                                               num_fields));

            /* Get number of object enums and their values */
            if (comp == CTR_EFLEX) {
                SHR_IF_ERR_EXIT
                    (bcmcth_ctr_eflex_obj_info_get(unit,
                                                   ingress,
                                                   &num_obj,
                                                   obj));
            } else if (comp == STATE_EFLEX) {
                SHR_IF_ERR_EXIT
                    (bcmcth_state_eflex_obj_info_get(unit,
                                                     ingress,
                                                     &num_obj,
                                                     obj));
            }

            /* Get offset values for each object enum */
            for (i = 0; i < num_obj; i++) {
                if (comp == CTR_EFLEX) {
                    if (ingress) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmcth_ctr_ing_eflex_bus_bit_offset_get(
                                unit,
                                i,
                                &obj_info[i]));
                    } else {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmcth_ctr_egr_eflex_bus_bit_offset_get(
                                unit,
                                i,
                                &obj_info[i]));
                    }
                } else if (comp == STATE_EFLEX) {
                    if (ingress) {
                        SHR_IF_ERR_VERBOSE_EXIT
                           (bcmcth_state_eflex_post_lkup_obj_bus_bit_offset_get(
                                unit,
                                i,
                                &obj_info[i]));

                    } else {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmcth_state_eflex_egr_obj_bus_bit_offset_get(
                                unit,
                                i,
                                &obj_info[i]));
                    }
                }
            }

            /* Insert entries in the OBJ_INFO LT */
            for (i = 0; i < num_obj; i++) {
                for (f_cnt = 0; f_cnt < count; f_cnt++) {
                    obj_info_lt_flds.field[f_cnt]->id = fid_list[f_cnt];

                    if (fid_list[f_cnt] == lt_info->obj_fid) {
                        obj_info_lt_flds.field[f_cnt]->data = obj[i];
                    } else if (fid_list[f_cnt] == lt_info->min_bit_fid) {
                        obj_info_lt_flds.field[f_cnt]->data =
                                                        obj_info[i].minbit;
                    } else if (fid_list[f_cnt] == lt_info->max_bit_fid) {
                        obj_info_lt_flds.field[f_cnt]->data =
                                                        obj_info[i].maxbit;
                    } else {
                        obj_info_lt_flds.field[f_cnt]->data = 0;
                    }
                }

                obj_info_lt_flds.count = count;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmimm_entry_insert(unit,
                                         lt_info->lt_sid,
                                         &obj_info_lt_flds));
            }
        } else {
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,"OBJ_INFO LT not supported\n")));
        }
    }

exit:
    /* Restore obj_info_lt_flds.count */
    obj_info_lt_flds.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &obj_info_lt_flds);
    SHR_FREE(fid_list);
    SHR_FREE(obj);
    SHR_FREE(obj_info);
    SHR_FUNC_EXIT();
}

int
bcmcth_ctr_eflex_imm_obj_info_register(int unit)
{
    bcmimm_lt_cb_t ctr_eflex_lt_cb;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, CTR_EFLEX, &ctrl));

    /* Register IMM handlers only if OBJ_INFO LT is supported*/
    if (ctrl->ing_obj_info_lt_info.supported) {
        /* Register CTR_ING_EFLEX_OBJ_INFO handler */
        sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        ctr_eflex_lt_cb.validate =
            bcmcth_ctr_eflex_read_only_lt_validate_cb;

        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 ctrl->ing_obj_info_lt_info.lt_sid,
                                 &ctr_eflex_lt_cb,
                                 NULL));
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"CTR_ING_EFLEX_OBJ_INFO LT not supported\n")));
    }

    if (ctrl->egr_obj_info_lt_info.supported) {
        /* Register CTR_EGR_EFLEX_OBJ_INFO handler */
        sal_memset(&ctr_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        ctr_eflex_lt_cb.validate =
            bcmcth_ctr_eflex_read_only_lt_validate_cb;

        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 ctrl->egr_obj_info_lt_info.lt_sid,
                                 &ctr_eflex_lt_cb,
                                 NULL));
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"CTR_EGR_EFLEX_OBJ_INFO LT not supported\n")));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_state_eflex_imm_obj_info_register(int unit)
{
    bcmimm_lt_cb_t state_eflex_lt_cb;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, STATE_EFLEX, &ctrl));

    if (ctrl->ing_obj_info_lt_info.supported) {
        /* Register FLEX_STATE_ING_OBJ_INFO handler */
        sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        state_eflex_lt_cb.validate =
            bcmcth_ctr_eflex_read_only_lt_validate_cb;

        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 ctrl->ing_obj_info_lt_info.lt_sid,
                                 &state_eflex_lt_cb,
                                 NULL));
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"FLEX_STATE_ING_OBJ_INFO LT not supported\n")));
    }

    if (ctrl->egr_obj_info_lt_info.supported) {
        /* Register FLEX_STATE_EGR_OBJ_INFO handler */
        sal_memset(&state_eflex_lt_cb, 0, sizeof(bcmimm_lt_cb_t));
        state_eflex_lt_cb.validate =
            bcmcth_ctr_eflex_read_only_lt_validate_cb;

        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 ctrl->egr_obj_info_lt_info.lt_sid,
                                 &state_eflex_lt_cb,
                                 NULL));
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"FLEX_STATE_EGR_OBJ_INFO LT not supported\n")));
    }

exit:
    SHR_FUNC_EXIT();
}
