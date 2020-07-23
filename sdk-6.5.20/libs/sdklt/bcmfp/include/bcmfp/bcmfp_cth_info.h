/*! \file bcmfp_cth_info.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP Group info LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_GROUP_INFO_H
#define BCMFP_CTH_GROUP_INFO_H

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmfp/bcmfp_stage_internal.h>

/*! Validate if out index value is higher than
 * custom handler allocated out count.
 * When value becomes equal, throw error.
 */
#define BCMFP_CTH_OUT_COUNT_CHECK(out_index, out_count)    \
   if (out_index >= out_count) {                             \
       return (SHR_E_INTERNAL);                            \
   }

/*!
 * Number of parameters need to be passed by source module which
 * posts BCMFP_INFO_LOOKUP event.
 * 1. Param0 - LTID
 * 2. Param1 - Transaction ID
 * 3. Param2 - Pointer in fields(bcmltd_fields_t)
 * 4. Param3 - Pointer to out fields(bcmltd_fields_t)
 */
#define BCMFP_INFO_EVENT_PARAMS_COUNT 0x4

/*!
 * Maximum number of containers an action gets
 * mapped to
 */
#define BCMFP_MAX_CONTS_PER_ACTION 2

/*!
 * \brief Get presel group related info.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transaction id.
 * \param [in] tbl_id             Logical table id.
 * \param [in] stage              Stage info structure.
 * \param [in] in                 Input fields information from LT.
 * \param [out] out               Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_psg_info_update(int unit,
                        uint32_t trans_id,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out);
/*!
 * \brief Get group related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage         Stage info structure.
 * \param [in] in            Input fields information from LT.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_group_info_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out);
/*!
 * \brief Get action container related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] stage         Stage info structure.
 * \param [in] in            Input fields information from LT.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */

extern int
bcmfp_action_cont_info_lookup(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmfp_stage_t *stage,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out);
/*!
 * \brief Get next action container related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage_id      Stage id.
 * \param [in] in            Input fields information from LT.
 * \param [in] arg           Generic arguements.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_action_cont_info_next_search(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmfp_stage_id_t stage_id,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get first action container info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage_id      Stage id.
 * \param [in] arg           Generic arguements.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */

extern int
bcmfp_action_cont_info_first_search(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmfp_stage_id_t stage_id,
                                    bcmltd_fields_t *out,
                                    const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get group partition related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage         Stage info structure.
 * \param [in] in            Input fields information from LT.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_group_partition_info_update(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t tbl_id,
                           bcmfp_stage_t *stage,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out);
/*!
 * \brief Get pdd and action related info.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transaction id.
 * \param [in] tbl_id             Logical table id.
 * \param [in] stage              Stage info structure.
 * \param [in] in                 Input fields information from LT.
 * \param [out] out               Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_pdd_info_update(int unit,
                        uint32_t trans_id,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out);

/*!
 * \brief Get first group related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage_id      Stage id.
 * \param [in] arg           Generic arguements.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_grp_info_first_search(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get next group related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage_id      Stage id.
 * \param [in] in            Input fields information from LT.
 * \param [in] arg           Generic arguements.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */

extern int
bcmfp_grp_info_next_search(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get first group partition related info.
 *
 * \param [in] unit               Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id             Logical table id.
 * \param [in] stage_id           Stage id.
 * \param [in] arg                Generic arguements.
 * \param [out] out               Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */

extern int
bcmfp_grp_part_info_first_search(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get next group partition related info.
 *
 * \param [in] unit               Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id             Logical table id.
 * \param [in] stage_id           Stage id.
 * \param [in] in                 Input fields information from LT.
 * \param [in] arg                Generic arguements.
 * \param [out] out               Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */

extern int
bcmfp_grp_part_info_next_search(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         bcmfp_stage_id_t stage_id,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get first presel group related info.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transaction id.
 * \param [in] tbl_id             Logical table id.
 * \param [in] stage_id           Stage id.
 * \param [in] arg                Generic arguements.
 * \param [out] out               Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */

extern int
bcmfp_psg_info_first_search(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get next presel group related info.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transaction id.
 * \param [in] tbl_id             Logical table id.
 * \param [in] stage_id           Stage id.
 * \param [in] in                 Input fields information from LT.
 * \param [in] arg                Generic arguements.
 * \param [out] out               Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */

extern int
bcmfp_psg_info_next_search(int unit,
                        uint32_t trans_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg);

/*!
 * \brief Get first pdd related info.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transaction id.
 * \param [in] tbl_id             Logical table id.
 * \param [in] stage_id           Stage id.
 * \param [in] arg                Generic arguements.
 * \param [out] out               Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_pdd_info_first_search(int unit,
                         uint32_t trans_id,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief Get next pdd related info.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transaction id.
 * \param [in] tbl_id             Logical table id.
 * \param [in] stage_id           Stage id.
 * \param [in] in                 Input fields information from LT.
 * \param [in] arg                Generic arguements.
 * \param [out] out               Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_pdd_info_next_search(int unit,
                        uint32_t trans_id,
                        bcmfp_stage_id_t stage_id,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg);

/*!
 * \brief Process BCMFP INFO LT events.
 *
 * \param [in] unit     Logical device id.
 * \param [in] event    Event name.
 * \param [in] ev_data  Event data to used to process
 *                      and populate.
 */
extern void
bcmfp_info_event_cb(int unit,
                    const char *event,
                    uint64_t ev_data);

/*!
 * \brief Get stage related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage         Stage info structure.
 * \param [in] in            Input fields information from LT.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_stage_info_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out);
/*!
 * \brief Get slice related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage         Stage info structure.
 * \param [in] in            Input fields information from LT.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_slice_info_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t tbl_id,
                        bcmfp_stage_t *stage,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out);

/*!
 * \brief Get first stage related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage_id      Stage id.
 * \param [in] arg           Generic arguements.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_stage_info_first_search(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get next stage related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage_id      Stage id.
 * \param [in] in            Input fields information from LT.
 * \param [in] arg           Generic arguements.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */

extern int
bcmfp_stage_info_next_search(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief Get first slice related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage_id      Stage id.
 * \param [in] arg           Generic arguements.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */
extern int
bcmfp_slice_info_first_search(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            bcmfp_stage_id_t stage_id,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg);
/*!
 * \brief Get next slice related info.
 *
 * \param [in] unit          Logical device id.
 * \param [in] op_arg        Operation arguments.
 * \param [in] tbl_id        Logical table id.
 * \param [in] stage_id      Stage id.
 * \param [in] in            Input fields information from LT.
 * \param [in] arg           Generic arguements.
 * \param [out] out          Output fields information from LT.
 *
 * \retval SHR_E_NONE        Success.
 * \retval SHR_E_NOT_FOUND   Info not found.
 */

extern int
bcmfp_slice_info_next_search(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmfp_stage_id_t stage_id,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg);

#endif /* BCMFP_CTH_GROUP_INFO_H */
