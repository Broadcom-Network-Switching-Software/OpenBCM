/*! \file bcmfp_oper_internal.h
 *
 * APIs to SET/GET opertaional information saved in HA area.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_OPER_INTERNAL_H
#define BCMFP_OPER_INTERNAL_H

#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_qual_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_trans_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>

/*!
 * Value to be used to caliculate the hash to index different
 * mapping segments in stage operational information.
 */
#define BCMFP_IDP_MAPS_MODULO 0x8

typedef struct bcmfp_sbr_oper_info_s {
    /*
     * SBR template id to entry id(s) mapping. Single SBR
     * template id can be associated to one or more entry ids.
     * This points to a linked list of HA elements in which
     * each HA element holds entry id associated to the SBR
     * template id.
     * When SBR template id is created/updated/deleted, all the
     * mapping entry ids should be updated. In order to avoid the
     * time spent in searching the entry ids associated to the
     * required SBR template id, this mapping is maintained.
     */
    bcmfp_ha_blk_info_t sbr_entry_map;
} bcmfp_sbr_oper_info_t;

/*!
 * \brief Get stage operational information of a given stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [out] stage_oper_info Stage operational information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_info_get(int unit,
                 bcmfp_stage_id_t stage_id,
                 bcmfp_stage_oper_info_t **stage_oper_info);

/*!
 * \brief Get operational mode(Global/PerPipe) of a given stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [out] oper_mode Stage operational mode.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_mode_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t *oper_mode);
/*!
 * \brief Get group operational information in a given stage id.
 *   Since group ids are unique across all pipes, pipe id is not
 *   required in fetching the group operational information.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [out] oinfo Group operational nformation.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_oper_info_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      bcmfp_group_oper_info_t **oinfo);

/*!
 * \brief Get group operational information in a given stage id.
 *   Since group ids are unique across all pipes, pipe id is not
 *   required in fetching the group operational information.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [out] oinfo Group operational nformation.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_oper_info_check_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      bcmfp_group_oper_info_t **oinfo);

/*!
 * \brief Get the reference count array for keygen profile
 *  table entries in a given pipe id and stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id.
 * \param [out] keygen_prof_ref_count pointer to array of reference
 *              counts saved for all entries in keygen profile table.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_keygen_prof_ref_count_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            int pipe_id,
                            bcmfp_ref_count_t **keygen_prof_ref_count);
/*!
 * \brief Get the reference count array for action profile
 *  table entries in a given pipe id and stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id.
 * \param [out] keygen_prof_ref_count pointer to array of reference
 *              counts saved for all entries in action profile table.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_action_prof_ref_count_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            int pipe_id,
                            bcmfp_ref_count_t **action_prof_ref_count);
/*!
 * \brief Get the reference count array for qos action profile
 *  table entries in a given pipe id and stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id.
 * \param [out] keygen_prof_ref_count pointer to array of reference
 *              counts saved for all entries in qos action profile table.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_qos_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             bcmfp_ref_count_t **qos_prof_ref_count);
/*!
 * \brief Get the reference count array for em key attribute profile
 *  table entries in a given pipe id and stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id.
 * \param [out] em_key_attrib_prof_ref_count pointer to array of reference
 *              counts saved for all entries in qos action profile table.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_em_key_attrib_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             bcmfp_ref_count_t **em_key_attrib_prof_ref_count);
/*!
 * \brief Get the reference count array for SBR profile
 *  table entries in a given pipe id and stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id.
 * \param [out] sbr_prof_ref_count pointer to array of reference
 *              counts saved for all entries in qos action profile table.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_sbr_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             bcmfp_ref_count_t **sbr_prof_ref_count);
/*!
 * \brief Get the reference count array for Presel Group profile
 *  table entries in a given pipe id and stage id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id.
 * \param [out] psg_prof_ref_count pointer to array of reference
 *              counts saved for all entries in qos action profile table.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_psg_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             bcmfp_ref_count_t **psg_prof_ref_count);
/*!
 * \brief Get the action resolution id(h/w LTID) for the given
 *  group id and stage id. Since group ids are unique across all
 *  pipes, pipe id is not required in fetching the action resolution id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [out] action_res_id Groups action resolution id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_action_res_id_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint16_t *action_res_id);
/*!
 * \brief Each qualifier in a group has offset(s) and width(s) information
 *   in the final key. Thease offset(s) and width(s) may spread across
 *   multiple parts if group is multiwide. This function is to get the
 *   qualifiers offset(s) and width(s) information of a qualifier in a
 *   given part_id, group_id and stage_id combination.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [in] opinfo Group operational information.
 * \param [in] is_presel presel group_id or main TCAM group_id.
 * \param [in] part_id First/second/Third Slice assigned to group.
 * \param [in] qual_id Qualifier id.
 * \param [out] q_offset offset(s) and width(s) information of a qualifier.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_qual_offset_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        bcmfp_group_oper_info_t *opinfo,
                        bool is_presel,
                        bcmfp_part_id_t part_id,
                        bcmltd_fid_t fid,
                        uint16_t fidx,
                        bcmfp_qual_offset_info_t *q_offset);

/*!
 * \brief Get the flags assigned to a group.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [out] group_flags group flags.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_flags_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      uint32_t *group_flags);
/*!
 * \brief Get the pipe to which group belongs to.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [out] tbl_inst groups pipe id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_tbl_inst_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      int *tbl_inst);
/*!
 * \brief Get the flags assigned to a group.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [in] part_id First/second/Third Slice assigned to group.
 * \param [out] ext_codes Extractor controls.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_ext_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_ext_codes_t **ext_codes);
/*!
 * \brief  Save the group action resolution id to HA space.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 * \param [in] action_res_id Action resolution id to be saved.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_action_res_id_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint16_t action_res_id);
/*!
 * \brief  Save the groups operational data, generated after group
 *  is successfully created, to HA space.
 *
 * \param [in] unit Logical device id.
 * \param [in] fg Group operational information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_oper_info_set(int unit,
                          bcmfp_group_t *fg);
extern int
bcmfp_group_oper_info_create(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id,
                             bcmfp_group_oper_info_t **opinfo);
/*!
 * \brief  Update the groups operational data, generated after group
 *  is successfully updated, to HA space.
 *
 * \param [in] unit Logical device id.
 * \param [in] fg Group operational information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_oper_info_update(int unit,
                             bcmfp_group_t *fg);
/*!
 * \brief  Save the different parameters of the group.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] pipe_id pipe id to which group belongs to.
 * \param [in] group_id group id created in stage_id.
 * \param [in] group_prio group priority.
 * \param [in] group_slice_mode Group slice mode.
 * \param [in] port_pkt_type Incoming port type(HIGIG/Ethernet),
 *             packet type(V4L3/V6L3/L2) to which rules in the
 *             groups applies.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_params_set(int unit,
                       bcmfp_stage_id_t stage_id,
                       int pipe_id,
                       bcmfp_group_id_t group_id,
                       uint32_t group_prio,
                       bcmfp_group_slice_mode_t group_slice_mode,
                       uint8_t port_pkt_type);
/*!
 * \brief Set operational mode(Global/PerPipe) of a given stage id.
 *  This function is being called only during FP component
 *  initialization time. This state information comes from FP_CONFIG
 *  LT. Since field values in FP_CONFIG values are save in IMM HA, FP
 *  need not save it again in HA. So eventually this funtion will be
 *  removed.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [out] oper_mode Stage operational mode.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_mode_set(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t oper_mode);


/*!
 * \brief  Delete the group operational information saved in
 *  HA memory. This happens when group is deleted.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_oper_info_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id);
/*!
 * \brief  Reset the different parameters of the group, which are
 *  saved in HA, to default values. This happens when group is
 *  deleted.
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_group_params_reset(int unit,
                       bcmfp_stage_id_t stage_id,
                       bcmfp_group_id_t group_id);

/*!
 * \brief During FP component initialization, each supported
 *  stage will be allocated one HA block. These HA blocks have
 *  the data layout as below. This function is to intialize the
 *  fields in each data segment depicted in this picture. This
 *  function is called during cold boot.
 *
 *   --------------------------------------
 *  |           HA block header            |
 *  |         (bcmfp_ha_blk_hdr_t)         |
 *   --------------------------------------
 *  |   stage operational information      |
 *  |     (bcmfp_stage_oper_info_t)        |
 *   --------------------------------------
 *  |     keygen profile table entries     |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |     action profile table entries     |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |     qos profile table entries        |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |em key attribute profile table entries|
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |       sbr profile table entries      |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |       psg profile table entries      |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |  group id to entry Id map array      |
 *   --------------------------------------
 *  |  rule id to entry Id map array       |
 *   --------------------------------------
 *  |  policy id to entry Id map array     |
 *   --------------------------------------
 *  |  meter id to entry Id map array      |
 *  |             (optional)               |
 *   --------------------------------------
 *  |  counter id to entry Id map array    |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    sbr id to entry Id map array      |
 *  |             (optional)               |
 *   --------------------------------------
 *  |presel entry id to group Id map array |
 *  |             (optional)               |
 *   --------------------------------------
 *  |presel group id to group Id map array |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    pdd id to group Id map array      |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    sbr id to group Id map array      |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    dpdd id to group Id map array     |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    dpolicy id to group Id map array  |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    dsbr id to group Id map array     |
 *  |             (optional)               |
 *   --------------------------------------
 *  |                                      |
 *  |    group oper info of group id 0     |
 *  |      (bcmfp_group_oper_info_t)       |
 *  |                                      |
 *   --------------------------------------
 *  |    group oper info of group id 1     |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                                      |
 *   --------------------------------------
 *  |   group oper info of group id N-1    |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |        Group oper info of            |
 *  |        presel group id 0             |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |        Group oper info of            |
 *  |        presel group id 1             |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                                      |
 *   --------------------------------------
 *  |        Group oper info of            |
 *  |        presel group id N-1           |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |      PDD oper info of PDD id 0       |
 *  |      (bcmfp_pdd_oper_info_t)         |
 *   --------------------------------------
 *  |      PDD oper info of PDD id 1       |
 *  |      (bcmfp_pdd_oper_info_t)         |
 *   --------------------------------------
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                                      |
 *   --------------------------------------
 *  |    PDD oper info of PDD id N - 1     |
 *  |      (bcmfp_pdd_oper_info_t)         |
 *   --------------------------------------
 *
 * \param [in] unit Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] blk_size size of the HA block id.
 * \param [in] issu_offset Different offsets in HA block.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_info_init_cb(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t blk_size,
                              bcmfp_ha_struct_offset_t *issu_offset);

/*!
 * \brief During FP component initialization, each supported
 *  stage will be allocated one HA block. These HA blocks have
 *  the data layout as below. This function is to intialize the
 *  offset of each data segment depicted in this picture. This
 *  function is called during warm boot.
 *
 *   --------------------------------------
 *  |           HA block header            |
 *  |         (bcmfp_ha_blk_hdr_t)         |
 *   --------------------------------------
 *  |   stage operational information      |
 *  |     (bcmfp_stage_oper_info_t)        |
 *   --------------------------------------
 *  |     keygen profile table entries     |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |     action profile table entries     |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |     qos profile table entries        |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |em key attribute profile table entries|
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |       sbr profile table entries      |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |       psg profile table entries      |
 *  |         reference counts             |
 *  |             (optional)               |
 *   --------------------------------------
 *  |  group id to entry Id map array      |
 *   --------------------------------------
 *  |  rule id to entry Id map array       |
 *   --------------------------------------
 *  |  policy id to entry Id map array     |
 *   --------------------------------------
 *  |  meter id to entry Id map array      |
 *  |             (optional)               |
 *   --------------------------------------
 *  |  counter id to entry Id map array    |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    sbr id to entry Id map array      |
 *  |             (optional)               |
 *   --------------------------------------
 *  |presel entry id to group Id map array |
 *  |             (optional)               |
 *   --------------------------------------
 *  |presel group id to group Id map array |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    pdd id to group Id map array      |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    sbr id to group Id map array      |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    dpdd id to group Id map array     |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    dpolicy id to group Id map array  |
 *  |             (optional)               |
 *   --------------------------------------
 *  |    dsbr id to group Id map array     |
 *  |             (optional)               |
 *   --------------------------------------
 *  |                                      |
 *  |    group oper info of group id 0     |
 *  |      (bcmfp_group_oper_info_t)       |
 *  |                                      |
 *   --------------------------------------
 *  |    group oper info of group id 1     |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                                      |
 *   --------------------------------------
 *  |   group oper info of group id N-1    |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |        Group oper info of            |
 *  |        presel group id 0             |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |        Group oper info of            |
 *  |        presel group id 1             |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                                      |
 *   --------------------------------------
 *  |        Group oper info of            |
 *  |        presel group id N-1           |
 *  |      (bcmfp_group_oper_info_t)       |
 *   --------------------------------------
 *  |      PDD oper info of PDD id 0       |
 *  |      (bcmfp_pdd_oper_info_t)         |
 *   --------------------------------------
 *  |      PDD oper info of PDD id 1       |
 *  |      (bcmfp_pdd_oper_info_t)         |
 *   --------------------------------------
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                 .                    |
 *  |                                      |
 *   --------------------------------------
 *  |    PDD oper info of PDD id N - 1     |
 *  |      (bcmfp_pdd_oper_info_t)         |
 *   --------------------------------------
 *
 * \param [in] unit     Logical device id.
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] blk_id   HA block ID(Either active or backup).
 * \param [in] blk_size size of the HA block id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_stage_oper_info_init_wb(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint16_t blk_id,
                              uint32_t blk_size);

/*!
 * \brief Set the operational information of a presel group that
 *  is generated whule presel group is created for the first time.
 *
 * \param [in] unit Logical device id
 * \param [in] group_cfg Presel group configuration.
 * \param [out] keygen_oper presel group operational information.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_presel_group_oper_info_set(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_group_id_t group_id,
                                 bcmfp_psg_config_t *psg_config,
                                 bcmfp_keygen_oper_t *keygen_oper);
/*!
 * \brief Delete the operational information of a presel group that
 *  is generated while presel group is created for the first time.
 *
 * \param [in] unit Logical device id
 * \param [in] group_id Presel group id.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmfp_presel_group_oper_info_delete(int unit,
                                    bcmfp_stage_id_t stage_id,
                                    bcmfp_group_id_t group_id);
/*!
 * \brief Get group operational information in a given stage id.
 *  Since group ids are unique across all pipes, pipe id is not
 *  required in fetching the group operational information.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id presel group id created in stage_id.
 * \param [out] oinfo presel group operational nformation.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
extern int
bcmfp_presel_group_oper_info_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint8_t presel_group_id,
                                 bcmfp_group_oper_info_t **oinfo);

#endif /* BCMFP_OPER_INTERNAL_H */
