/*! \file bcmfp_pdd_internal.h
 *
 * PDD(Policy Data Decoder) data structures and APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_PDD_INTERNAL_H
#define BCMFP_PDD_INTERNAL_H

#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_pdd.h>
#include <bcmfp/generated/bcmfp_ha.h>

/*
 * Maximum number of PDD physical containers a stage
 * supports across all supported devices.
 */
#define BCMFP_PDD_MAX_CONTAINERS (256)

/*
 * Maximum number of PDD sections a stage supported
 * across all supported devices.
 */
#define BCMFP_PDD_MAX_SECTIONS  (4)

/*
 * Maximum number of PDD Words a stage supported
 * across all supported devices.
 */
#define BCMFP_MAX_PDD_WORDS     (8)

/*!
 * \brief Get PDD operational information in a given stage id.
 *  Since pdd profile ids are unique across all pipes, pipe id
 *  is not required in fetching the PDD operational information.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id PDD profile id created in stage_id.
 * \param [out] oinfo PDD  operational nformation.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_oper_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_pdd_oper_info_t **oinfo);
/*!
 * \brief Get PDD actions operational information in a given stage id.
 *  Since pdd profile ids are unique across all pipes, pipe id
 *  is not required in fetching the PDD actions operational information.
 *
 * \param [in]  unit     Logical device id
 * \param [in]  stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in]  pdd_id   PDD profile id created in stage_id.
 * \param [in]  type     PDD operational inforamtion type.
 * \param [out] ainfo    PDD actions operational nformation.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_actions_info_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_pdd_id_t pdd_id,
                           bcmfp_pdd_oper_type_t type,
                           bcmfp_pdd_actions_info_t **ainfo);
/*!
 * \brief Each action in a PDD has offset(s) and width(s) information
 *  in the raw policy data. These offset(s) and width(s) may spread
 *  across multiple parts if PDD is multiwide. This function is to
 *  get the action offset(s) and width(s) information of an action
 *  in a given part_id, pdd_id and stage_id combination.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id PDD profile id created in stage_id.
 * \param [in] part_id First/second/Third Slice assigned to group.
 * \param [in] action_id Qualifier id.
 * \param [in] type PDD operational information type.
 * \param [out] a_offset offset(s) and width(s) information of an action.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_action_offset_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_action_t action_id,
                        bcmfp_pdd_oper_type_t type,
                        bcmfp_action_oper_info_t *a_offset);
/*
 * \brief When pdd is deleted, offset(s) and width(s) information
 *  of all its actions needs to be deleted. This involves releasing
 *  the Type-1 and Type-2 HA element chains of PDD. To know more
 *  about Type-1 and Type-2 HA element chains refer the function header
 *  of bcmfp_pdd_action_offset_info_set.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id PDD id created in stage_id.
 * \param [in] type PDD operational information type.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_action_offset_info_reset(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   bcmfp_pdd_id_t pdd_id,
                                   bcmfp_pdd_oper_type_t type);
/*
 * \brief Upon successfull creation of PDD, offset(s) and
 *  width(s) information of all actions in all parts of
 *  the PDD will be saved in HA area. A single action
 *  may be part of different slices of the PDD(if the PDD
 *  is multi wide). So this function may be called multiple
 *  times for a single action. offset(s) and width(s) of a
 *  single action in all parts are saved in single HA
 *  elements chain.
 *
 *  PDDs actions offset(s) and width(s) information is
 *  stored in two types of HA element chains.
 *
 *  Type-1: Each action has 3 consecutive HA elements in this
 *  chain. First HA element element holds action id. Second HA
 *  element holds head HA block id of another HA element chain
 *  holding offset(s) and width(s) of the action in all parts.
 *  Third HA element holds head HA block offset of HA block id
 *  stored in second HA element.Only one HA element chain of this
 *  type is required per action. Head HA block id and offset comes
 *  from the pdd operational information.Number of HA elements
 *  in this chain is equal to (3 * number of actions present in
 *  the PDD.
 *
 *  Type-2:Each action has one HA element chain of this type.
 *  Head HA blok id and offsets to this chain comes from Type-1
 *  HA element chain. Number of HA elements in this chain depends
 *  on the number of unique offsets needs to be saved per action.
 *  Number of HA element chains is equal to number of actions
 *  present in the PDD.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] part_id First/Second/Third slice of the PDD.
 * \param [in] type PDD operational information type.
 * \param [in] a_offset Actions offset(s) and width(s).
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_action_offset_info_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_pdd_oper_type_t type,
                        bcmfp_action_oper_info_t *a_offset);
/*
 * \brief Save the PDD profile bitmap of a given part in
 *  HA area.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] part_id First/Second/Third slice of the PDD.
 * \param [in] type PDD operational information type.
 * \param [in] pdd_bitmap PDD profile bitmap.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_bitmap_set(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_pdd_id_t pdd_id,
                     bcmfp_part_id_t part_id,
                     bcmfp_pdd_oper_type_t type,
                     uint32_t *pdd_bitmap);
/*
 * \brief Reset the PDD profile bitmap in all parts.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_bitmap_reset(int unit,
                       bcmfp_stage_id_t stage_id,
                       bcmfp_pdd_id_t pdd_id,
                       bcmfp_pdd_oper_type_t type);
/*
 * \brief Get the data size occupied by all actions in PDD. This
 *  includes the physical containers, section offsets and raw data
 *  size.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] part_id First/Second/Third slice of the PDD.
 * \param [in] type PDD operational information type.
 * \param [out] data_width Number of bits occupied by all
 *                         actions enabled in PDD.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_data_width_get(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_pdd_id_t pdd_id,
                     bcmfp_part_id_t part_id,
                     bcmfp_pdd_oper_type_t type,
                     uint32_t *data_width);
/*
 * \brief Get the PDD profile bitmap of a given part in
 *  HA area.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] part_id First/Second/Third slice of the PDD.
 * \param [in] type PDD operational information type.
 * \param [out] pdd_bitmap PDD profile bitmap.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_bitmap_get(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_pdd_id_t pdd_id,
                     bcmfp_part_id_t part_id,
                     bcmfp_pdd_oper_type_t type,
                     uint32_t *pdd_bitmap);
/*
 * \brief Save the PDD sections and their offset information
 *  once te PDD bitmap is created.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] part_id First/Second/Third slice of the PDD.
 * \param [in] type PDD operational information type.
 * \param [in] sinfo PDD sections and their offsets information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_section_info_set(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_pdd_id_t pdd_id,
                           bcmfp_part_id_t part_id,
                           bcmfp_pdd_oper_type_t type,
                           bcmfp_pdd_section_info_t *sinfo);
/*
 * \brief Get the PDD sections and their offset information
 *  from the HA area.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] part_id First/Second/Third slice of the PDD.
 * \param [in] type PDD operational information type.
 * \param [out] sinfo PDD sections and their offsets information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_section_info_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_pdd_id_t pdd_id,
                           bcmfp_part_id_t part_id,
                           bcmfp_pdd_oper_type_t type,
                           bcmfp_pdd_section_info_t *sinfo);
/*
 * \brief Delete the PDD sections and their offset information
 *  from the HA area.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_section_info_reset(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_pdd_id_t pdd_id,
                             bcmfp_pdd_oper_type_t type);
/*
 * \brief Set the the minimum group mode required for the PDD to accomodate all
 * the actions in it.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 * \param [in] pdd_mode group mode.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_group_mode_set(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_pdd_id_t pdd_id,
                         bcmfp_pdd_oper_type_t type,
                         bcmfp_group_mode_t pdd_mode);
/*
 * \brief Get the operational group mode of the PDD. This may not be set if none
 * of the group template using this PDD template is with MODE_AUTO=1
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 * \param [in] pdd_mode group mode.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_group_mode_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_pdd_id_t pdd_id,
                         bcmfp_pdd_oper_type_t type,
                         bcmfp_group_mode_t *pdd_mode);
/*
 * \brief Set the number of parts(1x/2x/3x) a PDD occupies
 *  to exapnd its policy data.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 * \param [in] num_parts 1x/2x/3x
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_num_parts_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_pdd_oper_type_t type,
                        uint8_t num_parts);
/*
 * \brief Get the number of parts(1x/2x/3x) a PDD occupies
 *  to exapnd its policy data.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 * \param [out] num_parts 1x/2x/3x
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_num_parts_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_pdd_oper_type_t type,
                        uint8_t *num_parts);
/*
 * \brief Get the width of the policy using this PDD in given
 *  operational type.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 * \param [out] policy_width Width of the policy using this PDD
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_policy_width_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_pdd_id_t pdd_id,
                           bcmfp_pdd_oper_type_t type,
                           uint32_t *policy_width);
/*
 * \brief Reset the number of parts(1x/2x/3x) a PDD occupies
 *  to zero.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_num_parts_reset(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pdd_id_t pdd_id,
                          bcmfp_pdd_oper_type_t type);
/*
 * \brief Check if bitmap, section offsets, actions offsets,
 *  and num_parts are already generated for the PDD. Otherwide
 *  PDD is yet to be operational.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 * \param [out] oper_status TRUE, if  bitmap, section offsets,
 *              actions offsets and num_parts are already
 *              generated for the PDD. Otherwise FALSE.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_oper_status_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pdd_id_t pdd_id,
                          bcmfp_pdd_oper_type_t type,
                          bool *oper_status);
/*
 * \brief Reset the bitmap, section offsets, actions offsets and
 *  num_parts of a PDD.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_oper_info_reset(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pdd_id_t pdd_id);

/*
 * \brief Reset the bitmap, section offsets, actions offsets and
 *  num_parts of a PDD.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] type PDD operational information type.
 * \param [in] num_parts numnber of PDD parts.
 * \param [in] group_mode group mode required for the PDD.
 * \param [in] policy_width Width of the policy using this PDD.
 * \param [in] pdd_oper_info PDD operational info.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_oper_info_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_pdd_oper_type_t type,
                        uint8_t num_parts,
                        bcmfp_group_mode_t group_mode,
                        uint32_t policy_wdith,
                        bcmfp_pdd_oper_t *pdd_oper_info);
/*
 * \brief Set PDD Config info from stage, pdd LT info
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pdd_id pdd id created in stage_id.
 * \param [in] group mode
 * \param [in] per_group_pdd PDD is per group or per entry.
 * \param [in] per_group_sbr SBR is per group or per entry.
 * \param [in] pdd imm config
 * \param [out] valid pdd data flag
 * \param [out] PDD config info
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_config_info_set(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pdd_id_t pdd_id,
                          bcmfp_group_mode_t group_mode,
                          bool is_default_policy,
                          bool is_pdd_per_group,
                          bool is_sbr_per_group,
                          bcmfp_pdd_config_t *pdd_config,
                          uint8_t *valid_pdd_data,
                          bcmfp_pdd_cfg_t **pdd_cfg_info);
/*
 * \brief Free the memory allocated for PDD Config infoo
 *
 * \param [in] unit Logical device id
 * \param [in] PDD config info
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_pdd_config_info_free(int unit,
                           bcmfp_pdd_cfg_t *pdd_cfg_info);

#endif /* BCMFP_PDD_INTERNAL_H */
