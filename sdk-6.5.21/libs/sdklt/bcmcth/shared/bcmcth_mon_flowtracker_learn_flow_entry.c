/*! \file bcmcth_mon_flowtracker_learn_flow_entry.c
 *
 * BCMCTH MON_FLOWTRACKER_LEARN_FLOW_ENTRY LT utilities
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl_enum.h>

#include <bcmltd/id/bcmltd_common_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>

#include <bcmimm/bcmimm_int_comp.h>

#include <bcmcth/generated/flowtracker_ha.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmcth/bcmcth_mon_flowtracker_util.h>
#include <bcmcth/bcmcth_mon_flowtracker_flow_entry.h>

/*******************************************************************************
 * Defines
 */
/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON
static int
bcmcth_mon_flowtracker_learn_flow_entry_fields_count(
    int unit,
    uint32_t *count)
{
    uint32_t depth = 0;
    uint32_t temp_cnt = 0;
    bcmlrd_field_def_t def;
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < MON_FLOWTRACKER_LEARN_FLOW_ENTRYt_FIELD_COUNT; i++) {
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmlrd_table_field_def_get(unit,
                                           MON_FLOWTRACKER_LEARN_FLOW_ENTRYt, i,
                                           &def));

        depth = def.depth;
        if (depth != 0) {
            temp_cnt += depth;
        } else {
            temp_cnt++;
        }
    }

    *count = temp_cnt;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Inserts/delete an entry into/from the MON_FLOWTRACKER_LEARN_FLOW_ENTRY LT
 * op = 0 (insert)
 * op = 1 (delete)
 */
static int
bcmcth_mon_ft_learn_flow_entry_ins_del_internal(
         int unit,
         int pipe,
         bcmltd_common_flowtracker_hw_learn_exact_match_idx_mode_t_t mode,
         uint32_t em_idx,
         int op
         )
{
    uint32_t field_cnt = 0;
    bcmltd_fields_t lt_flds = {0};
    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_learn_flow_entry_fields_count(unit, &field_cnt));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit, field_cnt, &lt_flds));

    lt_flds.count = 3;

    lt_flds.field[0]->id = MON_FLOWTRACKER_LEARN_FLOW_ENTRYt_PIPEf;
    lt_flds.field[0]->data = pipe;
    lt_flds.field[1]->id = MON_FLOWTRACKER_LEARN_FLOW_ENTRYt_MODEf;
    lt_flds.field[1]->data = mode;
    lt_flds.field[2]->id = MON_FLOWTRACKER_LEARN_FLOW_ENTRYt_EXACT_MATCH_INDEXf;
    lt_flds.field[2]->data = em_idx;
    if (op == 0) { /* insert */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmimm_entry_insert(unit, MON_FLOWTRACKER_LEARN_FLOW_ENTRYt,
                                    &lt_flds));
    } else if (op == 1) { /* delete */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmimm_entry_delete(unit, MON_FLOWTRACKER_LEARN_FLOW_ENTRYt,
                                    &lt_flds));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    bcmcth_mon_ft_util_fields_free(unit, field_cnt, &lt_flds);
    SHR_FUNC_EXIT();
}

/******************************************************************************
* Public functions
 */
/*!
 * \brief Insert dummy entry into MON_FLOWTRACKER_LEARN_FLOW_ENTRY LT.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe on which flow was learnt.
 * \param [in] mode Exact match index mode of the flow learnt (Single,double,quad)
 * \param [in] em_idx Exact match index of the flow learnt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_ft_learn_flow_entry_dummy_insert(
         int unit,
         int pipe,
         bcmltd_common_flowtracker_hw_learn_exact_match_idx_mode_t_t mode,
         uint32_t em_idx
         )
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_mon_ft_learn_flow_entry_ins_del_internal(unit, pipe, mode,
                                                        em_idx, 0 /* Insert */));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from MON_FLOWTRACKER_LEARN_FLOW_ENTRY LT.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe on which flow was learnt.
 * \param [in] mode Exact match index mode of the flow learnt (Single,double,quad)
 * \param [in] em_idx Exact match index of the flow learnt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_mon_ft_learn_flow_entry_delete(
         int unit,
         int pipe,
         bcmltd_common_flowtracker_hw_learn_exact_match_idx_mode_t_t mode,
         uint32_t em_idx
         )
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_mon_ft_learn_flow_entry_ins_del_internal(unit, pipe, mode,
                                                        em_idx, 1 /* Delete */));
exit:
    SHR_FUNC_EXIT();
}
