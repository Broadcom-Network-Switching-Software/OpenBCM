/*! \file bcmcth_ctr_eflex_pool_mgr.c
 *
 * BCMCTH Enhanced Flex Counter pool resource manager routines
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmcth/bcmcth_ctr_eflex_internal.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/* Function to get counter pool list */
static int
bcmcth_ctr_eflex_pool_list_get(int unit,
                               uint32_t pipe_idx,
                               uint32_t pool_idx,
                               ctr_eflex_action_profile_data_t *entry,
                               ctr_eflex_pool_list_t **list)
{
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    int egr_num_instance = 0;
    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));
    device_info = ctrl->ctr_eflex_device_info;
    egr_num_instance = device_info->state_ext.num_egress_instance;
    switch (entry->comp) {
        case CTR_EFLEX:
            *list = entry->ingress ? &ctrl->ing_pool_list[pool_idx][pipe_idx] :
                                     &ctrl->egr_pool_list[pool_idx][pipe_idx];
            break;
        case STATE_EFLEX:
            if (entry->ingress) {
                switch (entry->state_ext.inst) {
                    case ING_POST_LKUP_INST:
                        *list = &ctrl->ing_pool_list[pool_idx][pipe_idx];
                        break;
                    case ING_POST_FWD_INST:
                        *list =
                        &ctrl->state_ext.ing_pool_list_b[pool_idx][pipe_idx];
                        break;
                    default:
                        SHR_ERR_EXIT(SHR_E_PARAM);
                } /* end inner SWITCH */
            } else {
                if (egr_num_instance == 1 ) {
                    *list = &ctrl->egr_pool_list[pool_idx][pipe_idx];
                } else {
                   switch (entry->state_ext.egr_inst) {
                      case EGR_POST_LKUP_INST:
                          *list = &ctrl->egr_pool_list[pool_idx][pipe_idx];
                          break;
                      case EGR_POST_FWD_INST:
                          *list =
                          &ctrl->state_ext.egr_pool_list_b[pool_idx][pipe_idx];
                          break;
                      default:
                          SHR_ERR_EXIT(SHR_E_PARAM);
                   } /* end inner SWITCH */
               }
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    } /* end outer SWITCH */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable/Disable counter pool
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] pool_idx  Pool number.
 * \param [in] pipe_idx  Pipe number.
 * \param [in] state disable, enable, shadow
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_PARAM Illegal pool number.
 */
static int
bcmcth_ctr_eflex_counter_pool_enable(int unit,
                                     ctr_eflex_action_profile_data_t *entry,
                                     uint32_t pool_idx,
                                     uint32_t pipe_idx,
                                     bcmptm_cci_pool_state_t state)
{
    bcmdrd_sid_t ctr_table_sid;
    ctr_eflex_pool_list_t *pool_list = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    uint32_t inst_idx, num_insts;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    /* Get counter pool list */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pool_list_get(unit,
                                        pipe_idx,
                                        pool_idx,
                                        entry,
                                        &pool_list));

    num_insts = ctrl->ctr_eflex_device_info->num_pipe_instance;

    /*
     * For device has no instance concept, the value may be initialized as 0,
     * Adjusted to 1 for unified process.
     */
    if (!num_insts) {
        num_insts = 1;
    }

    for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
        /* Get counter pool sid */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_eflex_pool_sid_get(unit,
                                       entry->ingress,
                                       entry->comp,
                                       inst_idx,
                                       entry->ingress ?
                                       entry->state_ext.inst : entry->state_ext.egr_inst,
                                       pool_idx,
                                       &ctr_table_sid));

        if (state == ENABLE) {
            /*
             * Check if pool is already enabled
             * During ctr_alloc/reserve, pool is enabled first
             * and pool_list bitmap is set later.
             */
            if (SHR_BITNULL_RANGE(pool_list->inuse_bitmap,
                                  0,
                                  pool_list->max_track)) {

                /* Set CCI counter pool state to ENABLE */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_cci_flex_counter_pool_set_state(unit,
                                                            ctr_table_sid,
                                                            ENABLE));
            }
        } else if (state == DISABLE) {
            /*
             * Disable only if all counters are disabled in this pool
             * During ctr_free, pool_list bitmap is cleared first and
             * pool is disabled later.
             */
            if (SHR_BITNULL_RANGE(pool_list->inuse_bitmap,
                                  0,
                                  pool_list->max_track)) {
                /* Set CCI counter pool state to DISABLE */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_cci_flex_counter_pool_set_state(unit,
                                                            ctr_table_sid,
                                                            DISABLE));

            }
        } else if (state == SHADOW) {
            /* Set CCI counter pool state to SHADOW */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cci_flex_counter_pool_set_state(unit,
                                                        ctr_table_sid,
                                                        SHADOW));
        }
    } /* end FOR */

exit:
    SHR_FUNC_EXIT();
}

/*! Function to set counter mode */
static int
bcmcth_ctr_eflex_counter_mode_set(int unit,
                                  ctr_eflex_action_profile_data_t *entry,
                                  ctr_eflex_ctr_mode_data_t *mode_data)
{
    uint32_t index_min = 0, index_max = 0;
    uint32_t cur_pool_idx = 0, num_ctr_per_section;
    ctr_eflex_pool_list_t *pool_list = NULL;
    bcmdrd_sid_t table_sid;
    ctr_eflex_control_t *ctrl = NULL;
    uint32_t inst_idx, num_insts;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mode_data, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    /* Calculate number of sections requested */
    num_ctr_per_section =
        ctrl->ctr_eflex_device_info->num_ctr_section;

    /* Call CCI API to set mode for associated pools */
    cur_pool_idx = mode_data->start_pool_idx;

    num_insts = ctrl->ctr_eflex_device_info->num_pipe_instance;

    /*
     * For device has no instance concept, the value may be initialized as 0,
     * Adjusted to 1 for unified process.
     */
    if (!num_insts) {
        num_insts = 1;
    }

    while (cur_pool_idx <= mode_data->end_pool_idx) {
        /* Get counter pool list */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_list_get(unit,
                                            mode_data->pipe_idx,
                                            cur_pool_idx,
                                            entry,
                                            &pool_list));


        /* Calculate min and max counter index in the pool */
        if (cur_pool_idx == mode_data->start_pool_idx) {
            index_min = mode_data->start_section_idx * num_ctr_per_section;
        } else {
            index_min = 0;
        }

        if (cur_pool_idx == mode_data->end_pool_idx) {
            index_max = (mode_data->end_section_idx * num_ctr_per_section) -1;
        } else {
            index_max = (pool_list->max_track * num_ctr_per_section) - 1;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "%s Entry:\n pipe_idx %d pool_idx %d\n"
                    "index min %d max %d\n"
                    "ctr_mode %d update_mode[0] %d update_mode[1] %d num %d\n"),
                    mode_data->ingress ? "Ingress" : "Egress",
                    mode_data->pipe_idx, cur_pool_idx, index_min, index_max,
                    mode_data->ctr_mode, mode_data->ctr_update_mode[0],
                    mode_data->ctr_update_mode[1],
                    (uint32_t)mode_data->num_update_mode));

        /* Call CCI API to set_mode */
        for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
            /* Get table sid */
            SHR_IF_ERR_EXIT
                (bcmcth_eflex_pool_sid_get(unit,
                                           mode_data->ingress,
                                           entry->comp,
                                           inst_idx,
                                           mode_data->ingress ?
                                           entry->state_ext.inst :
                                             entry->state_ext.egr_inst,
                                           cur_pool_idx,
                                           &table_sid));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmptm_cci_flex_counter_mode_set(unit,
                                                  table_sid,
                                                  index_min,
                                                  index_max,
                                                  mode_data->pipe_idx,
                                                  mode_data->ctr_mode,
                                                  mode_data->ctr_update_mode,
                                                  mode_data->num_update_mode));
        }
        cur_pool_idx++;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_ctr_eflex_pool_mgr_init(int unit,
                               bool ingress,
                               eflex_comp_t comp,
                               uint32_t num_ctr_per_section,
                               uint32_t max_pipe_num,
                               uint32_t max_pool_num,
                               ctr_eflex_pool_list_t p_list[][MAX_PIPES_NUM],
                               uint32_t state_instance
                                )
{
    uint32_t pipe_idx, pool_idx, num_bytes;
    uint32_t num_section, max_ctr_num;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_pool_list_t *pool_list;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Allocate pool mgr */
    for (pipe_idx = 0; pipe_idx < max_pipe_num; pipe_idx++) {
        for (pool_idx = 0; pool_idx < max_pool_num; pool_idx++) {
            pool_list = &p_list[pool_idx][pipe_idx];

            /* Get max number of counters per pool */
            max_ctr_num = ingress ?
                          ctrl->ing_pool_ctr_num[pool_idx] :
                          ctrl->egr_pool_ctr_num[pool_idx];

            if (!ingress  && comp == STATE_EFLEX
                          && state_instance == EGR_POST_FWD_INST ) {
                max_ctr_num =
                          ctrl->state_ext.egr_pool_ctr_num_b[pool_idx];
            }
            /* Calculate number of sections per pool */
            num_section = max_ctr_num / num_ctr_per_section;

            num_bytes = SHR_BITALLOCSIZE(num_section);

            /* Allocate inuse bitmap */
            pool_list->inuse_bitmap = sal_alloc(num_bytes,
                                                "bcmcthCtrEflexPoolListBitmap");
            SHR_NULL_CHECK(pool_list->inuse_bitmap, SHR_E_MEMORY);

            sal_memset(pool_list->inuse_bitmap, 0, num_bytes);

            pool_list->max_track = num_section;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_ctr_eflex_pool_mgr_uninit(int unit,
                                 bool ingress,
                                 eflex_comp_t comp,
                                 uint32_t max_pipe_num,
                                 uint32_t max_pool_num,
                                 ctr_eflex_pool_list_t p_list[][MAX_PIPES_NUM])
{
    uint32_t pipe_idx, pool_idx;
    ctr_eflex_pool_list_t *pool_list;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Free pool mgr */
    for (pipe_idx = 0; pipe_idx < max_pipe_num; pipe_idx++) {
        for (pool_idx = 0; pool_idx < max_pool_num; pool_idx++) {
            pool_list = &p_list[pool_idx][pipe_idx];

            if (pool_list->inuse_bitmap != NULL) {
                sal_free(pool_list->inuse_bitmap);
                pool_list->inuse_bitmap = NULL;
            }
            pool_list->max_track = 0;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to create pool mgr
 *
 * The pool mgr resource is implemented as an array of bitmaps.
 * The number of pools represents the size of the array and is
 * device dependent.
 * Each pool is divided into sections and the number of counters
 * per section is device dependent.
 * The hardware base index is in terms of sections (i.e. section
 * of N counters is the granualarity of the hardware base index
 * Each section is represented as a bit in the bitmap and hence
 * the counters are allocated/reserved/freed in terms of sections.
 */
int
bcmcth_ctr_eflex_pool_mgr_create(int unit, eflex_comp_t comp)
{
    uint32_t max_pipe_num, max_ing_pool_num, num_egr_instance;
    uint32_t max_egr_pool_num, num_ctr_per_section;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    max_pipe_num = device_info->num_pipes;
    max_ing_pool_num = device_info->num_ingress_pools;
    max_egr_pool_num = device_info->num_egress_pools;
    num_ctr_per_section = device_info->num_ctr_section;
    num_egr_instance = device_info->state_ext.num_egress_instance;
    /* Create ingress pool mgr */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pool_mgr_init(unit,
                                        INGRESS,
                                        comp,
                                        num_ctr_per_section,
                                        max_pipe_num,
                                        max_ing_pool_num,
                                        ctrl->ing_pool_list,
                                        0));

    /* Create egress pool mgr */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pool_mgr_init(unit,
                                        EGRESS,
                                        comp,
                                        num_ctr_per_section,
                                        max_pipe_num,
                                        max_egr_pool_num,
                                        ctrl->egr_pool_list,
                                        0));

    /* For flex state, initialize other ingress instance */
    if (comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_init(unit,
                                            INGRESS,
                                            comp,
                                            num_ctr_per_section,
                                            max_pipe_num,
                                            max_ing_pool_num,
                                            ctrl->state_ext.ing_pool_list_b,
                                            0));
        switch (num_egr_instance) {

          case 2:
          /* For flex state, initialize other egress instance */
          SHR_IF_ERR_VERBOSE_EXIT
          (bcmcth_ctr_eflex_pool_mgr_init(unit,
                                          EGRESS,
                                          comp,
                                          num_ctr_per_section,
                                          max_pipe_num,
                                          max_ing_pool_num,
                                          ctrl->state_ext.egr_pool_list_b,
                                          EGR_POST_FWD_INST));
          default :
          break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to destroy pool mgr */
int
bcmcth_ctr_eflex_pool_mgr_destroy(int unit, eflex_comp_t comp)
{
    uint32_t max_pipe_num, max_ing_pool_num, max_egr_pool_num;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    max_pipe_num = device_info->num_pipes;
    max_ing_pool_num = device_info->num_ingress_pools;
    max_egr_pool_num = device_info->num_egress_pools;

    /* Destroy ingress pool mgr */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pool_mgr_uninit(unit,
                                          INGRESS,
                                          comp,
                                          max_pipe_num,
                                          max_ing_pool_num,
                                          ctrl->ing_pool_list));

    /* Destroy egress pool mgr */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pool_mgr_uninit(unit,
                                          EGRESS,
                                          comp,
                                          max_pipe_num,
                                          max_egr_pool_num,
                                          ctrl->egr_pool_list));

    /* For flex state, destroy other ingress instance */
    if (comp == STATE_EFLEX) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_uninit(unit,
                                              INGRESS,
                                              comp,
                                              max_pipe_num,
                                              max_ing_pool_num,
                                              ctrl->state_ext.ing_pool_list_b));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_uninit(unit,
                                              EGRESS,
                                              comp,
                                              max_pipe_num,
                                              max_ing_pool_num,
                                              ctrl->state_ext.egr_pool_list_b));
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to set allocated counter bits in pool list */
static int
bcmcth_ctr_eflex_pool_mgr_list_set(int unit,
                                   uint32_t num_sections,
                                   uint32_t pipe_idx,
                                   ctr_eflex_action_profile_data_t *entry,
                                   ctr_eflex_ctr_mode_data_t *ctr_mode_data)
{
    uint32_t cur_pool_idx;
    uint32_t start_bit = 0, num_bits = 0;
    ctr_eflex_control_t *ctrl = NULL;
    ctr_eflex_pool_list_t *pool_list = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    cur_pool_idx = ctr_mode_data->start_pool_idx;

    while (cur_pool_idx <= ctr_mode_data->end_pool_idx) {
        /* Get counter pool list */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_list_get(unit,
                                            pipe_idx,
                                            cur_pool_idx,
                                            entry,
                                            &pool_list));

        start_bit = 0;

        /* Calculate start bit and number of bits in the pool */
        if (cur_pool_idx == ctr_mode_data->start_pool_idx) {
            start_bit = ctr_mode_data->start_section_idx;
            if (cur_pool_idx == ctr_mode_data->end_pool_idx) {
                num_bits = num_sections;
            } else {
                num_bits = pool_list->max_track - start_bit;
            }
        } else if (cur_pool_idx == ctr_mode_data->end_pool_idx) {
            num_bits = ctr_mode_data->end_section_idx;
        } else {
            num_bits = pool_list->max_track;
        }

        /* Set bits in current pool */
        SHR_BITSET_RANGE(pool_list->inuse_bitmap,
                         start_bit,
                         num_bits);

        cur_pool_idx++;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Helper function to determine start and end pool id and sections */
static int
bcmcth_ctr_eflex_pool_mgr_alloc_helper(int unit,
                                       uint32_t pipe_idx,
                                       ctr_eflex_action_profile_data_t *entry,
                                       ctr_eflex_ctr_mode_data_t *ctr_mode_data)
{
    uint32_t section_count = 0, section_count_rem = 0;
    uint32_t max_pool_num, num_ctr_per_section, num_sections;
    uint32_t start_pool_idx, end_pool_idx = 0, cur_pool_idx;
    uint32_t start_section_idx = 0, end_section_idx = 0, cur_section_idx;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    ctr_eflex_pool_list_t *pool_list = NULL;
    bool start_section_idx_found = false, done = false;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    max_pool_num = entry->ingress ? device_info->num_ingress_pools :
                                    device_info->num_egress_pools;

    /* Calculate number of sections requested */
    num_ctr_per_section = device_info->num_ctr_section;

    num_sections = (entry->num_ctrs % num_ctr_per_section) ?
                   ((entry->num_ctrs/num_ctr_per_section) + 1) :
                   (entry->num_ctrs/num_ctr_per_section);

    start_pool_idx = cur_pool_idx = entry->pool_idx;

    do {
        /* In given pool, determine the start of free sections */
        if (!start_section_idx_found) {
            /* Get counter pool list */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_pool_list_get(unit,
                                                pipe_idx,
                                                cur_pool_idx,
                                                entry,
                                                &pool_list));

            for (cur_section_idx = 0;
                 cur_section_idx < pool_list->max_track;
                 cur_section_idx++) {
                if (!SHR_BITGET(pool_list->inuse_bitmap, cur_section_idx)) {
                    section_count++;
                } else {
                    section_count = 0;
                    continue;
                }

                /*
                 * Requested number of counters are
                 * available in the given pool
                 */
                if (section_count == num_sections) {
                    start_section_idx = cur_section_idx - section_count + 1;
                    end_pool_idx = cur_pool_idx;
                    end_section_idx = start_section_idx + num_sections;
                    done = true;
                    break;
                }
            } /* end FOR */

            if (!done) {
                /*
                 * Here section_count represents the number of
                 * free sections at the end of the pool
                 */
                if (!section_count) {
                    /* There are no free counters in the requested pool */
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                            "No free counters in pipe %d pool %d\n"),
                            pipe_idx, entry->pool_idx));
                    return SHR_E_RESOURCE;
                }

                start_section_idx = pool_list->max_track - section_count;

                start_section_idx_found = true;

                /* calculate remaining number of required sections */
                section_count_rem = num_sections - section_count;
            }
        } /* end IF (start_section_idx_found) */

        /*
         * Determine if remaining number of counters are available
         * in the next consecutive pool(s)
         */
        if (!done) {
            section_count = 0;

            /* Move to the subsequent pool */
            cur_pool_idx++;

            if (cur_pool_idx >= max_pool_num) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,"Max pool reached in pipe %d\n"),
                        pipe_idx));
                return SHR_E_RESOURCE;
            }

            /* Get counter pool list */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_pool_list_get(unit,
                                                pipe_idx,
                                                cur_pool_idx,
                                                entry,
                                                &pool_list));

            /* Check for free sections from the beginning of the pool */
            for (cur_section_idx = 0;
                 cur_section_idx < pool_list->max_track;
                 cur_section_idx++) {
                if (!SHR_BITGET(pool_list->inuse_bitmap, cur_section_idx)) {
                    section_count++;
                } else {
                    break;
                }
            }

            if (section_count >= section_count_rem) {
                end_pool_idx = cur_pool_idx;
                end_section_idx = section_count_rem;
                done = true;
            } else {
                /* Update remaining number of required sections */
                section_count_rem -= section_count;
            }
        }
    } while (!done);

    ctr_mode_data->ingress = entry->ingress;
    ctr_mode_data->start_pool_idx = start_pool_idx;
    ctr_mode_data->end_pool_idx = end_pool_idx;
    ctr_mode_data->start_section_idx = start_section_idx;
    ctr_mode_data->end_section_idx = end_section_idx;
    ctr_mode_data->pipe_idx = pipe_idx;
    ctr_mode_data->ctr_mode = entry->ctr_mode;
    ctr_mode_data->ctr_update_mode[UPDATE_MODE_A_IDX] =
        entry->ctr_update_mode[UPDATE_MODE_A_IDX];
    ctr_mode_data->ctr_update_mode[UPDATE_MODE_B_IDX] =
        entry->ctr_update_mode[UPDATE_MODE_B_IDX];
    ctr_mode_data->num_update_mode = entry->num_update_mode;

exit:
    SHR_FUNC_EXIT();
}

/*! Function to allocate counters */
int
bcmcth_ctr_eflex_pool_mgr_ctr_alloc(int unit,
                                    bool warm,
                                    ctr_eflex_action_profile_data_t *entry,
                                    uint32_t *base_idx)
{
    uint32_t num_sections, num_ctr_per_section;
    uint32_t pool_num, pipe_idx;
    ctr_eflex_ctr_mode_data_t ctr_mode_data ={0};
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(base_idx, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    /* Calculate number of sections requested */
    num_ctr_per_section = device_info->num_ctr_section;

    num_sections = (entry->num_ctrs % num_ctr_per_section) ?
                   ((entry->num_ctrs/num_ctr_per_section) + 1) :
                   (entry->num_ctrs/num_ctr_per_section);

    /* Allocate counters for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_alloc_helper(unit,
                                                    pipe_idx,
                                                    entry,
                                                    &ctr_mode_data));

        /* Enable counter pools */
        if (!warm) {
            for (pool_num = ctr_mode_data.start_pool_idx;
                 pool_num <= ctr_mode_data.end_pool_idx;
                 pool_num++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ctr_eflex_counter_pool_enable(unit,
                                                          entry,
                                                          pool_num,
                                                          pipe_idx,
                                                          ENABLE));
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_list_set(unit,
                                                num_sections,
                                                pipe_idx,
                                                entry,
                                                &ctr_mode_data));

        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "%s %s Entry:\n pipe_idx %d num_counters %d\n"
                    "pool_idx start %d end %d\n"
                    "section_idx num %d start %d end %d\n"),
                    entry->ingress ? "Ingress" : "Egress",
                    entry->comp ? "State" : "Ctr",
                    pipe_idx, entry->num_ctrs,
                    ctr_mode_data.start_pool_idx, ctr_mode_data.end_pool_idx,
                    num_sections, ctr_mode_data.start_section_idx,
                    ctr_mode_data.end_section_idx));

        /* Call CCI API to set_mode for associated pools */
        if (!warm) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_counter_mode_set(unit,
                                                   entry,
                                                   &ctr_mode_data));
        }
    }

    *base_idx = ctr_mode_data.start_section_idx;
    entry->start_pool_idx = ctr_mode_data.start_pool_idx;
    entry->end_pool_idx = ctr_mode_data.end_pool_idx;
    entry->start_base_idx = ctr_mode_data.start_section_idx;
    entry->end_base_idx = ctr_mode_data.end_section_idx;

exit:
    SHR_FUNC_EXIT();
}

/*! Function to free counters */
int
bcmcth_ctr_eflex_pool_mgr_ctr_free(int unit,
                                   ctr_eflex_action_profile_data_t *entry)
{
    uint32_t num_sections = 0, num_ctr_per_section, pool_num = 0;
    uint32_t start_pool_idx = 0, end_pool_idx = 0, cur_pool_idx = 0;
    uint32_t pipe_idx, num_sections_per_pipe;
    uint32_t start_section_idx = 0, end_section_idx = 0, clr_section_count = 0;
    ctr_eflex_pool_list_t *pool_list = NULL;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    /* Calculate number of sections requested */
    num_ctr_per_section = ctrl->ctr_eflex_device_info->num_ctr_section;

    num_sections_per_pipe = (entry->num_ctrs % num_ctr_per_section) ?
                            ((entry->num_ctrs/num_ctr_per_section) + 1) :
                            (entry->num_ctrs/num_ctr_per_section);

    /* Free counters for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Start from given pool and base index */
        cur_pool_idx = start_pool_idx = entry->pool_idx;
        if (entry->base_idx_auto) {
            start_section_idx = entry->base_idx_oper;
        } else {
            start_section_idx = entry->base_idx;
        }

        /* Restore number of sections for next pipe */
        num_sections = num_sections_per_pipe;

        /* Get counter pool list */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_list_get(unit,
                                            pipe_idx,
                                            cur_pool_idx,
                                            entry,
                                            &pool_list));

        while (num_sections) {
            if (start_section_idx + num_sections > pool_list->max_track) {
                end_section_idx = pool_list->max_track ;
            } else {
                end_section_idx = start_section_idx + num_sections;
            }

            clr_section_count = end_section_idx - start_section_idx;

            /* Clear bits for current pool */
            SHR_BITCLR_RANGE(pool_list->inuse_bitmap,
                             start_section_idx,
                             clr_section_count);

            LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                        "%s %s Entry free:\n pipe_idx %d pool_idx %d\n"
                        "num_sections %d clr count %d\n"),
                        entry->ingress ? "Ingress" : "Egress",
                        entry->comp ? "State" : "Ctr",
                        pipe_idx, cur_pool_idx,
                        num_sections_per_pipe, clr_section_count));

            /* Reached end of current pool */
            if (start_section_idx + num_sections > pool_list->max_track) {
                /* Move to next pool */
                cur_pool_idx++;
                start_section_idx = end_section_idx = 0;

            /* Get counter pool list */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_pool_list_get(unit,
                                                pipe_idx,
                                                cur_pool_idx,
                                                entry,
                                                &pool_list));
            }
            num_sections -= clr_section_count;
        }

        end_pool_idx = cur_pool_idx;

        /* Disable counter pools */
        for (pool_num = start_pool_idx; pool_num <= end_pool_idx; pool_num++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_counter_pool_enable(unit,
                                                      entry,
                                                      pool_num,
                                                      pipe_idx,
                                                      DISABLE));
        }
    } /* end FOR */

exit:
    SHR_FUNC_EXIT();
}

/*! Function to reserve counters */
int
bcmcth_ctr_eflex_pool_mgr_ctr_reserve(int unit,
                                      bool warm,
                                      ctr_eflex_action_profile_data_t *entry)
{
    uint32_t num_sections, num_ctr_per_section, pool_num = 0;
    uint32_t start_pool_idx = 0, end_pool_idx = 0, cur_pool_idx = 0;
    uint32_t start_section_idx = 0, end_section_idx = 0, set_section_count = 0;
    uint32_t pipe_idx, max_pool_num, num_sections_per_pipe;
    ctr_eflex_pool_list_t *pool_list = NULL;
    ctr_eflex_ctr_mode_data_t ctr_mode_data;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    sal_memset(&ctr_mode_data, 0, sizeof(ctr_eflex_ctr_mode_data_t));

    max_pool_num = entry->ingress ?
                   ctrl->ctr_eflex_device_info->num_ingress_pools :
                   ctrl->ctr_eflex_device_info->num_egress_pools;

    /* Calculate number of sections requested */
    num_ctr_per_section = ctrl->ctr_eflex_device_info->num_ctr_section;

    num_sections_per_pipe = (entry->num_ctrs % num_ctr_per_section) ?
                            ((entry->num_ctrs/num_ctr_per_section) + 1) :
                            (entry->num_ctrs/num_ctr_per_section);

    /*
     * During warmboot, if BASE_INDEX_AUTO is enabled,
     * then use the previously calculated base index in base_idx_oper
     */
    if (warm && entry->base_idx_auto) {
        entry->base_idx = entry->base_idx_oper;
    }

    /* Reserve counters for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Start from given pool and base index */
        cur_pool_idx = start_pool_idx = entry->pool_idx;
        start_section_idx = entry->base_idx;

        /* Restore number of sections for next pipe */
        num_sections = num_sections_per_pipe;

        /* Get counter pool list */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_list_get(unit,
                                            pipe_idx,
                                            cur_pool_idx,
                                            entry,
                                            &pool_list));

        while (num_sections) {
            if (start_section_idx + num_sections > pool_list->max_track) {
                end_section_idx = pool_list->max_track ;
            } else {
                end_section_idx = start_section_idx + num_sections;
            }

            set_section_count = end_section_idx - start_section_idx;

            /* Reached end of current pool */
            if (start_section_idx + num_sections > pool_list->max_track) {
                /* Move to next pool */
                cur_pool_idx++;

                if (cur_pool_idx >= max_pool_num) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,"Max pool reached in pipe %d\n"),
                            pipe_idx));
                    return SHR_E_RESOURCE;
                }

                start_section_idx = end_section_idx = 0;

                /* Get counter pool list */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ctr_eflex_pool_list_get(unit,
                                                    pipe_idx,
                                                    cur_pool_idx,
                                                    entry,
                                                    &pool_list));
            }
            num_sections -= set_section_count;
        }

        end_pool_idx = cur_pool_idx;

        ctr_mode_data.ingress = entry->ingress;
        ctr_mode_data.start_pool_idx = start_pool_idx;
        ctr_mode_data.end_pool_idx = end_pool_idx;
        ctr_mode_data.start_section_idx = entry->base_idx;
        ctr_mode_data.end_section_idx = end_section_idx;
        ctr_mode_data.pipe_idx = pipe_idx;
        ctr_mode_data.ctr_mode = entry->ctr_mode;
        ctr_mode_data.ctr_update_mode[UPDATE_MODE_A_IDX] =
            entry->ctr_update_mode[UPDATE_MODE_A_IDX];
        ctr_mode_data.ctr_update_mode[UPDATE_MODE_B_IDX] =
            entry->ctr_update_mode[UPDATE_MODE_B_IDX];
        ctr_mode_data.num_update_mode = entry->num_update_mode;

        /* Enable counter pools */
        if (!warm) {
            for (pool_num = start_pool_idx;
                 pool_num <= end_pool_idx;
                 pool_num++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ctr_eflex_counter_pool_enable(unit,
                                                          entry,
                                                          pool_num,
                                                          pipe_idx,
                                                          ENABLE));
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_list_set(unit,
                                                num_sections_per_pipe,
                                                pipe_idx,
                                                entry,
                                                &ctr_mode_data));

        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                    "%s %s Entry:\n pipe_idx %d num_counters %d\n"
                    "pool_idx start %d end %d\n"
                    "section_idx num %d start %d end %d\n"),
                    entry->ingress ? "Ingress" : "Egress",
                    entry->comp ? "State" : "Ctr",
                    pipe_idx, entry->num_ctrs,
                    ctr_mode_data.start_pool_idx, ctr_mode_data.end_pool_idx,
                    num_sections_per_pipe,
                    ctr_mode_data.start_section_idx,
                    ctr_mode_data.end_section_idx));

        /* Call CCI API to set_mode for calculated pools */
        if (!warm) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_counter_mode_set(unit,
                                                   entry,
                                                   &ctr_mode_data));
        }
    } /* end FOR */

    entry->start_pool_idx = ctr_mode_data.start_pool_idx;
    entry->end_pool_idx = ctr_mode_data.end_pool_idx;
    entry->start_base_idx = ctr_mode_data.start_section_idx;
    entry->end_base_idx = ctr_mode_data.end_section_idx;

exit:
    SHR_FUNC_EXIT();
}

/*! Function to allocate shadow pool counters & set primary pools to SHADOW */
int
bcmcth_ctr_eflex_pool_mgr_shadow_ctr_alloc(
    int unit,
    bool warm,
    ctr_eflex_action_profile_data_t *entry)
{
    uint32_t num_sections, num_pools_per_pipe, num_ctr_per_section;
    uint32_t pool_num = 0, pipe_idx, max_ctr_num, num_pools = 0, max_pool_num;
    uint32_t cur_pool_idx, start_pool_idx = 0, end_pool_idx;
    ctr_eflex_ctr_mode_data_t ctr_mode_data ={0};
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;
    ctr_eflex_pool_list_t *pool_list = NULL;
    bool start_pool_found = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    max_pool_num = entry->ingress ? device_info->num_ingress_pools :
                                    device_info->num_egress_pools;

    /* Get max number of counters per pool */
    max_ctr_num = entry->ingress ? ctrl->ing_pool_ctr_num[0] :
                                   ctrl->egr_pool_ctr_num[0];

    /* Calculate number of complete pools required */
    num_pools_per_pipe = (entry->num_ctrs % max_ctr_num) ?
                         ((entry->num_ctrs / max_ctr_num) + 1) :
                         (entry->num_ctrs / max_ctr_num);

    /* Calculate number of sections required */
    num_ctr_per_section = device_info->num_ctr_section;
    num_sections = (max_ctr_num / num_ctr_per_section) * num_pools_per_pipe;

    /* Allocate counters for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Start from 1st pool */
        cur_pool_idx = 0;

        /* Restore number of pools for next pipe */
        num_pools = num_pools_per_pipe;

        while (num_pools) {
            if (cur_pool_idx >= max_pool_num) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,"Max pool reached in pipe %d\n"),
                        pipe_idx));
                return SHR_E_RESOURCE;
            }
            /* Get counter pool list */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_pool_list_get(unit,
                                                pipe_idx,
                                                cur_pool_idx,
                                                entry,
                                                &pool_list));

            /* Check if complete pool is empty */
            if (SHR_BITNULL_RANGE(pool_list->inuse_bitmap,
                                  0,
                                  pool_list->max_track)) {
                if (!start_pool_found) {
                    /* Set current pool as start shadow pool id */
                    start_pool_idx = cur_pool_idx;
                    start_pool_found = true;
                }
                num_pools--;
            } else {
                /*
                 * Current pool is not completely empty and
                 * all required shadow pools are not allocated yet
                 * Hence reset start shadow pool id and continue search
                 */
                start_pool_found = false;
                num_pools = num_pools_per_pipe;
            }
            cur_pool_idx++;
        } /* end  WHILE */

        end_pool_idx = cur_pool_idx - 1;

        ctr_mode_data.ingress = entry->ingress;
        ctr_mode_data.start_pool_idx = start_pool_idx;
        ctr_mode_data.end_pool_idx = end_pool_idx;
        ctr_mode_data.start_section_idx = 0;
        ctr_mode_data.end_section_idx = max_ctr_num / num_ctr_per_section;
        ctr_mode_data.pipe_idx = pipe_idx;
        ctr_mode_data.ctr_mode = entry->ctr_mode;
        ctr_mode_data.ctr_update_mode[UPDATE_MODE_A_IDX] =
            entry->ctr_update_mode[UPDATE_MODE_A_IDX];
        ctr_mode_data.ctr_update_mode[UPDATE_MODE_B_IDX] =
            entry->ctr_update_mode[UPDATE_MODE_B_IDX];
        ctr_mode_data.num_update_mode = entry->num_update_mode;

        /* Set shadow pools as in-use */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_mgr_list_set(unit,
                                                num_sections,
                                                pipe_idx,
                                                entry,
                                                &ctr_mode_data));

        /* Set primary pools to SHADOW */
        if (!warm) {
            for (pool_num = entry->start_pool_idx;
                 pool_num <= entry->end_pool_idx;
                 pool_num++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ctr_eflex_counter_pool_enable(unit,
                                                          entry,
                                                          pool_num,
                                                          pipe_idx,
                                                          SHADOW));
            }

            /* Enable shadow counter pools */
            for (pool_num = start_pool_idx;
                 pool_num <= end_pool_idx;
                 pool_num++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ctr_eflex_counter_pool_enable(unit,
                                                          entry,
                                                          pool_num,
                                                          pipe_idx,
                                                          ENABLE));
            }

            /* Call CCI API to set_mode for calculated shadow pools */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_counter_mode_set(unit,
                                                   entry,
                                                   &ctr_mode_data));
        }
    } /* end  FOR */

    entry->start_shdw_pool_idx = ctr_mode_data.start_pool_idx;
    entry->end_shdw_pool_idx = ctr_mode_data.end_pool_idx;

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,"Shadow pool start %d, end %d\n"),
            entry->start_shdw_pool_idx, entry->end_shdw_pool_idx));
exit:
    SHR_FUNC_EXIT();
}

/*! Function to free shadow pool counters & enable primary pools */
int
bcmcth_ctr_eflex_pool_mgr_shadow_ctr_free(
    int unit,
    bool primary_enable,
    ctr_eflex_action_profile_data_t *entry)
{
    uint32_t pool_idx, pipe_idx;
    ctr_eflex_pool_list_t *pool_list = NULL;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Free shadow counters for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        for (pool_idx = entry->start_shdw_pool_idx;
             pool_idx <= entry->end_shdw_pool_idx;
             pool_idx++) {
            /* Get shadow counter pool list */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_pool_list_get(unit,
                                                pipe_idx,
                                                pool_idx,
                                                entry,
                                                &pool_list));

            /* Clear bits for shadow pool */
            SHR_BITCLR_RANGE(pool_list->inuse_bitmap,
                             0,
                             pool_list->max_track);

            /* Disable shadow pool */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ctr_eflex_counter_pool_enable(unit,
                                                      entry,
                                                      pool_idx,
                                                      pipe_idx,
                                                      DISABLE));
        }

        /* Set primary pools to ENABLE */
        if (primary_enable) {
            for (pool_idx = entry->start_pool_idx;
                 pool_idx <= entry->end_pool_idx;
                 pool_idx++) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_ctr_eflex_counter_pool_enable(unit,
                                                          entry,
                                                          pool_idx,
                                                          pipe_idx,
                                                          ENABLE));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to check if given pool is free */
bool
bcmcth_ctr_eflex_pool_mgr_is_pool_free(int unit,
                                       ctr_eflex_action_profile_data_t *entry)
{
    uint32_t pipe_idx;
    ctr_eflex_pool_list_t *pool_list = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Check for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        /* Get counter pool list */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_pool_list_get(unit,
                                            pipe_idx,
                                            entry->pool_idx,
                                            entry,
                                            &pool_list));

        /* If entire pool is not free, return false */
        if (!SHR_BITNULL_RANGE(pool_list->inuse_bitmap,
                               0,
                               pool_list->max_track)) {
            SHR_ERR_EXIT(false);
        }
    }

    /* Pool is free in all applicable pipes */
    SHR_ERR_EXIT(true);

exit:
    if (SHR_FUNC_ERR()) {
        SHR_ERR_EXIT(false);
    }
    SHR_FUNC_EXIT();
}
