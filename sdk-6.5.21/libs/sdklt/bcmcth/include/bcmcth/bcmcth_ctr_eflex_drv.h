/*! \file bcmcth_ctr_eflex_drv.h
 *
 * BCMCTH CTR_EFLEX Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BCMBD) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_EFLEX_DRV_H
#define BCMCTH_CTR_EFLEX_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/*! Invalid index. */
#define INVALID_IDX 0xff

/*!
 * \brief Get update controls list and field list.
 *
 * \param [in] unit       Unit number.
 * \param [in] ingress    Ingress or Egress direction.
 * \param [out] update_control_list  List of update controls registers.
 * \param [out] num  Number of update controls.
 * \param [out] counter_pool_enable_fid Counter pool enable field ID.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_update_control_get_f)(
    int unit,
    bool ingress,
    bcmdrd_sid_t **update_control_list,
    uint32_t *num,
    bcmdrd_fid_t *counter_pool_enable_fid);

/*!
 * \brief Get counter tables list.
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [out] ctr_table_list  List of counter tables.
 * \param [out] num  Number of counter tables.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_ctr_table_get_f)(int unit,
                                    bool ingress,
                                    bcmdrd_sid_t **ctr_table_list,
                                    uint32_t *num);
/*!
 * \brief Get  device info
 *
 * \param [in] unit    Unit number.
 * \param [out] device_info Device pool and pipe info
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_device_info_get_f)(
    int unit,
    bcmcth_ctr_eflex_device_info_t **device_info);

/*!
 * \brief Set counter action enable reg
 *
 * \param [in] unit    Unit number.
 * \param [in] enable  Set/clear action id bit.
 * \param [in] entry   Counter action id entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_action_reg_set_f)(int unit,
                                     bool enable,
                                     ctr_eflex_action_profile_data_t *entry);

/*!
 * \brief Write counter action table 0 & 1 entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Counter action id entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_action_table_write_f)(
    int unit,
    ctr_eflex_action_profile_data_t *entry);

/*!
 * \brief Set bitp profile to default values
 *
 * \param [in] unit    Unit number.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_bitp_profile_set_f)(int unit);

/*!
 * \brief Write operand profile table entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Operand profile entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_op_profile_write_f)(
    int unit,
    ctr_eflex_op_profile_data_t *entry);

/*!
 * \brief Write range check profile table entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Range check profile entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_range_chk_write_f)(
    int unit,
    ctr_eflex_range_chk_data_t *entry);

/*!
 * \brief Write trigger table entry
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry   Trigger entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_trigger_write_f)(
    int unit,
    ctr_eflex_trigger_data_t *entry);

/*!
 * \brief Read trigger table entry
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry   Trigger entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_trigger_read_f)(
    int unit,
    ctr_eflex_trigger_data_t *entry);

/*!
 * \brief Write group action entry
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry   Group action entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_group_action_write_f)(
    int unit,
    ctr_eflex_group_action_data_t *entry);

/*!
 * \brief Get physical object id
 *
 * \param [in] unit    Unit number.
 * \param [in] select  Ingress/egress.
 * \param [in] data    Value of object enum.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_object_id_get_f)(
    int unit,
    int select,
    uint64_t data);

/*!
 * \brief Get logical object id
 *
 * \param [in] unit    Unit number.
 * \param [in] select  Ingress/egress.
 * \param [in] data    Value of object enum.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_logical_object_id_get_f)(
    int unit,
    int select,
    uint64_t data);

/*!
 * \brief Get object index
 *
 * \param [in] unit    Unit number.
 * \param [in] select  Ingress/egress.
 * \param [in] data    Value of object enum.
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_object_idx_get_f)(
    int unit,
    int select,
    int data);

/*!
 * \brief Get action zone info
 *
 * \param [in] unit    Unit number.
 * \param [in] select  Ingress/egress.
 * \param [in] data    Value of action.
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_action_zone_get_f)(
    int unit,
    int select,
    uint32_t data);

/*!
 * \brief Write error stats entry
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry   Error stats entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_error_stats_write_f)(
    int unit,
    ctr_eflex_error_stats_data_t *entry);

/*!
 * \brief Read error stats entry
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry   Error stats entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_error_stats_read_f)(
    int unit,
    ctr_eflex_error_stats_data_t *entry);

/*!
 * \brief Verify if the pipe number is valid or not.
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress/egress.
 * \param [in] pipe    Pipe number.
 * \param [out] valid  Pipe is valid or not.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_pipe_valid_check_f)(int unit,
                                       bool ingress,
                                       uint32_t pipe,
                                       bool *valid);

/*!
 * \brief Initialize flex counter hitbit bank sid to logical id mapping.
 *
 * \param [in] unit    Unit number.
 * \param [in] warm    Warm or cold boot.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_hitbit_sid_map_init_f)(int unit, bool warm);

/*!
 * \brief De-initialize flex counter hitbit bank sid to logical id mapping.
 *
 * \param [in] unit    Unit number.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_hitbit_sid_map_deinit_f)(int unit);

/*!
 * \brief Set shadow pool registers
 *
 * \param [in] unit    Unit number.
 * \param [in] enable  Set/clear shadow pool registers.
 * \param [in] entry   Counter action id entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_shadow_pool_set_f)(int unit,
                                      bool enable,
                                      ctr_eflex_action_profile_data_t *entry);

/*!
 * \brief Initialize timestamp registers
 *
 * \param [in] unit    Unit number.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_timestamp_init_f)(int unit);

/*!
 * \brief Write object quantization entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Object quantization entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_obj_quant_write_f)(int unit,
                                      ctr_eflex_obj_quant_data_t *entry);

/*!
 * \brief Register OBJ_QUANTIZATION handlers with IMM
 *
 * \param [in] unit     Unit number.
 * \param [out] imm_reg Register handlers with IMM.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_obj_quant_imm_reg_f)(int unit, bool *imm_reg);

/*!
 * \brief Get flex counter object info
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in,out] num_obj Number of flex counter objects.
 * \param [out] obj Array of flex counter object values.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_obj_info_get_f)(int unit,
                                   bool ingress,
                                   uint32_t *num_obj,
                                   uint32_t *obj);

/*!
 * \brief Add hitbit control info to internal software state
 *
 * \param [in] unit    Unit number.
 * \param [in] notify  Group/Non-group action profile notification.
 * \param [in] skip_index Dst/Src group/non-group action profile to be skipped.
 * \param [in] ctrl    Flex control structure.
 * \param [in] entry   HITBIT_CONTROL LT entry.
 * \param [in] grp_action_entry GROUP_ACTION_PROFILE LT entry.
 * \param [out] hitbit_lt_info Internal hitbit lt software state info structure.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_hitbit_ctrl_info_add_f)(
    int unit,
    bool notify,
    uint32_t skip_index,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_group_action_data_t *grp_action_entry,
    ctr_eflex_hitbit_lt_info_t **hitbit_lt_info);

/*!
 * \brief Get destination or source index to be skipped during
 * HITBIT_CONTROL LT update operation.
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Updated HITBIT_CONTROL LT entry.
 * \param [in] cur_entry Existing HITBIT_CONTROL LT entry.
 * \param [out] skip_index Dst/Src group/non-group action profile to be skipped.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_hitbit_ctrl_skip_idx_get_f)(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_hitbit_ctrl_data_t *cur_entry,
    uint32_t *skip_index);

/*!
 * \brief During HITBIT_CONTROL LT lookup operation, determine if corresponding
 * group/non-group action profiles are configured.
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   HITBIT_CONTROL LT entry.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_hitbit_ctrl_lookup_help_f)(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry);

/*!
 * \brief Function to calculate hit index for hash LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table id with hitbit monitoring configured.
 * \param [in] sid Sid of hitbit physical bank.
 * \param [in] hit_context Hit context information.
 * \param [in] tbl_inst Pipe index.
 * \param [in] bkt_num Number of entries in bucket.
 * \param [in] bkt_entry Position of entry in bucket.
 * \param [in] context Flex counter context info.
 * \param [out] entry_idx Calculated hit index.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_ctr_eflex_hash_entry_index_calc_f)(
    int unit,
    bcmltd_sid_t ltid,
    bcmdrd_sid_t sid,
    bcmptm_rm_hash_hit_context_t *hit_context,
    int tbl_inst,
    uint32_t bkt_num,
    uint32_t bkt_entry,
    void *context,
    uint32_t *entry_idx);

/*!
 * \brief CTR EFLEX driver object
 *
 * CTR EFLEX driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH CTR EFLEX module by
 * \ref bcmcth_ctr_eflex_drv_init() from the top layer. BCMCTH CTR EFLEX
 * internally will use this interface to get the corresponding information.
 */
typedef struct bcmcth_ctr_eflex_drv_s {
    /*! Get device info */
    bcmcth_ctr_eflex_device_info_get_f  device_info_get_fn;
    /*! Get counter tables list */
    bcmcth_ctr_eflex_ctr_table_get_f  ctr_table_get_fn;
    /*! Set counter action enable reg */
    bcmcth_ctr_eflex_action_reg_set_f  action_reg_set_fn;
    /*! Write counter action table 0 & 1 entry */
    bcmcth_ctr_eflex_action_table_write_f  action_table_write_fn;
    /*! Set bitp profile */
    bcmcth_ctr_eflex_bitp_profile_set_f  bitp_profile_set_fn;
    /*! Write operand profile table entry */
    bcmcth_ctr_eflex_op_profile_write_f  op_profile_write_fn;
    /*! Write range check profile table entry */
    bcmcth_ctr_eflex_range_chk_write_f  range_chk_write_fn;
    /*! Write trigger table entry */
    bcmcth_ctr_eflex_trigger_write_f  trigger_write_fn;
    /*! Read trigger table entry */
    bcmcth_ctr_eflex_trigger_read_f  trigger_read_fn;
    /*! Write group action table entry */
    bcmcth_ctr_eflex_group_action_write_f  group_action_write_fn;
    /*! Get object id */
    bcmcth_ctr_eflex_object_id_get_f  object_id_get_fn;
    /*! Get index of object */
    bcmcth_ctr_eflex_object_idx_get_f  object_idx_get_fn;
    /*! Get action zone info */
    bcmcth_ctr_eflex_action_zone_get_f  action_zone_get_fn;
    /*! Write error stats entry */
    bcmcth_ctr_eflex_error_stats_write_f  error_stats_write_fn;
    /*! Read error stats entry */
    bcmcth_ctr_eflex_error_stats_read_f  error_stats_read_fn;
    /*! Verify pipe number */
    bcmcth_ctr_eflex_pipe_valid_check_f  pipe_valid_check_fn;
    /*! Get counter update control info */
    bcmcth_ctr_eflex_update_control_get_f  update_control_get_fn;
    /*! Initialize hitbit bank sid to logical id mapping */
    bcmcth_ctr_eflex_hitbit_sid_map_init_f hitbit_sid_map_init_fn;
    /*! De-initialize hitbit bank sid to logical id mapping */
    bcmcth_ctr_eflex_hitbit_sid_map_deinit_f hitbit_sid_map_deinit_fn;
    /*! Get logical object id */
    bcmcth_ctr_eflex_logical_object_id_get_f  logical_object_id_get_fn;
    /*! Set shadow pool registers */
    bcmcth_ctr_eflex_shadow_pool_set_f shadow_pool_set_fn;
    /*! Initialize timestamp registers */
    bcmcth_ctr_eflex_timestamp_init_f timestamp_init_fn;
    /*! Write object quantization entry */
    bcmcth_ctr_eflex_obj_quant_write_f  obj_quant_write_fn;
    /*! Register object quantization handler with IMM */
    bcmcth_ctr_eflex_obj_quant_imm_reg_f  obj_quant_imm_reg_fn;
    /*! Get object info */
    bcmcth_ctr_eflex_obj_info_get_f  obj_info_get_fn;
    /*! Add hitbit control info */
    bcmcth_ctr_eflex_hitbit_ctrl_info_add_f  hitbit_ctrl_info_add_fn;
    /*! Get hitbit control skip get */
    bcmcth_ctr_eflex_hitbit_ctrl_skip_idx_get_f hitbit_ctrl_skip_idx_get_fn;
    /*! Help hitbit control lookup */
    bcmcth_ctr_eflex_hitbit_ctrl_lookup_help_f hitbit_ctrl_lookup_help_fn;
    /*! Calculate hash entry index */
    bcmcth_ctr_eflex_hash_entry_index_calc_f hash_entry_index_calc_fn;
} bcmcth_ctr_eflex_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,\
                             _fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_ctr_eflex_drv_t *_bd##_cth_ctr_eflex_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Get  device info
 *
 * \param [in] unit    Unit number.
 * \param [out] device_info Device pool and pipe info
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_device_info_get(int unit,
                                 bcmcth_ctr_eflex_device_info_t **device_info);

/*!
 * \brief Get counter tables list.
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [out] ctr_table_list  List of counter tables.
 * \param [out] num  Number of counter tables.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_ctr_table_get(int unit,
                               bool ingress,
                               bcmdrd_sid_t **ctr_table_list,
                               uint32_t *num);

/*!
 * \brief Set counter action enable reg
 *
 * \param [in] unit    Unit number.
 * \param [in] enable  Set/clear action id bit.
 * \param [in] entry   Counter action id entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_action_reg_set(int unit,
                                bool enable,
                                ctr_eflex_action_profile_data_t *entry);

/*!
 * \brief Write counter action table 0 & 1 entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Counter action id entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_action_table_write(int unit,
                                    ctr_eflex_action_profile_data_t *entry);

/*!
 * \brief Set bitp profile to default values
 *
 * \param [in] unit    Unit number.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_bitp_profile_set(int unit);

/*!
 * \brief Write operand profile table entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Operand profile entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_op_profile_write(int unit,
                                  ctr_eflex_op_profile_data_t *entry);

/*!
 * \brief Write range check profile table entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Range check profile entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_range_chk_write(int unit,
                                 ctr_eflex_range_chk_data_t *entry);

/*!
 * \brief Write trigger table entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Trigger entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_trigger_write(int unit,
                               ctr_eflex_trigger_data_t *entry);

/*!
 * \brief Read trigger table entry
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry   Trigger entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_trigger_read(int unit,
                              ctr_eflex_trigger_data_t *entry);

/*!
 * \brief Write group action entry
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry   Group action entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_group_action_write(int unit,
                                    ctr_eflex_group_action_data_t *entry);

/*!
 * \brief Get the CTR EFLEX driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_eflex_drv_init(int unit);

/*!
 * \brief Cleanup the CTR EFLEX driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_eflex_drv_cleanup(int unit);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The CTR EFLEX custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmcth_ctr_eflex_imm_init(int unit);

/*!
 * \brief Convert parameter to an object id.
 *
 * \param [in] unit Unit number.
 * \param [in] select Ingress or Egress.
 * \param [in] data Value of object.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_eflex_obj_id_get(int unit, int select, uint64_t data);

/*!
 * \brief Convert parameter to logical object id.
 *
 * \param [in] unit Unit number.
 * \param [in] select Ingress or Egress.
 * \param [in] data Value of object.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_eflex_logical_obj_id_get(int unit, int select, uint64_t data);

/*!
 * \brief Convert object id to index.
 *
 * \param [in] unit Unit number.
 * \param [in] select Ingress or Egress.
 * \param [in] data Object id.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_eflex_obj_idx_get(int unit, int select, int data);

/*!
 * \brief Get action zone info.
 *
 * \param [in] unit Unit number.
 * \param [in] select Ingress or Egress.
 * \param [in] data Value of action.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_eflex_action_zone_get(int unit, int select, uint32_t data);

/*!
 * \brief Write error stats entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Error stats entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_error_stats_write(int unit,
                                   ctr_eflex_error_stats_data_t *entry);

/*!
 * \brief Read error stats entry
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry   Error stats entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_error_stats_read(int unit,
                                  ctr_eflex_error_stats_data_t *entry);

/*!
 * \brief Verify if the pipe number is valid or not.
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress/egress.
 * \param [in] pipe    Pipe number.
 * \param [out] valid  Pipe is valid or not.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_pipe_valid_check(int unit,
                                  bool ingress,
                                  uint32_t pipe,
                                  bool *valid);

/*!
 * \brief Get update controls list and field list.
 *
 * \param [in] unit       Unit number.
 * \param [in] ingress    Ingress or Egress direction.
 * \param [out] update_control_list  List of update controls registers.
 * \param [out] num  Number of update controls.
 * \param [out] counter_pool_enable_fid Counter pool enable field ID.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_update_control_get(
    int unit,
    bool ingress,
    bcmdrd_sid_t **update_control_list,
    uint32_t *num,
    bcmdrd_fid_t *counter_pool_enable_fid);

/*!
 * \brief Initialize flex counter hitbit bank sid to logical id mapping.
 *
 * \param [in] unit    Unit number.
 * \param [in] warm    Warm or cold boot.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_hitbit_sid_map_init(int unit, bool warm);

/*!
 * \brief De-initialize flex counter hitbit bank sid to logical id mapping.
 *
 * \param [in] unit    Unit number.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_hitbit_sid_map_deinit(int unit);

/*!
 * \brief Set shadow pool registers
 *
 * \param [in] unit    Unit number.
 * \param [in] enable  Set/clear shadow pool registers.
 * \param [in] entry   Counter action id entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_shadow_pool_set(int unit,
                                 bool enable,
                                 ctr_eflex_action_profile_data_t *entry);

/*!
 * \brief Initialize timestamp registers
 *
 * \param [in] unit    Unit number.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_timestamp_init(int unit);

/*!
 * \brief Write object quantization entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Object quantization entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_obj_quant_write(int unit,
                                 ctr_eflex_obj_quant_data_t *entry);

/*!
 * \brief Register OBJ_QUANTIZATION handlers with IMM
 *
 * \param [in] unit     Unit number.
 * \param [out] imm_reg Register handlers with IMM.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_obj_quant_imm_reg(int unit, bool *imm_reg);

/*!
 * \brief Get flex counter object info
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in, out] num_obj Number of flex counter objects.
 * \param [out] obj Array of flex counter object values.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_obj_info_get(int unit,
                              bool ingress,
                              uint32_t *num_obj,
                              uint32_t *obj);

/*!
 * \brief Add hitbit control info to internal software state
 *
 * \param [in] unit    Unit number.
 * \param [in] notify  Group/Non-group action profile notification.
 * \param [in] skip_index Dst/Src group/non-group action profile to be skipped.
 * \param [in] ctrl    Flex control structure.
 * \param [in] entry   HITBIT_CONTROL LT entry.
 * \param [in] grp_action_entry GROUP_ACTION_PROFILE LT entry.
 * \param [out] hitbit_lt_info Internal hitbit lt software state info structure.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_hitbit_ctrl_info_add(
    int unit,
    bool notify,
    uint32_t skip_index,
    ctr_eflex_control_t *ctrl,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_group_action_data_t *grp_action_entry,
    ctr_eflex_hitbit_lt_info_t **hitbit_lt_info);

/*!
 * \brief Get destination or source index to be skipped during
 * HITBIT_CONTROL LT update operation.
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Updated HITBIT_CONTROL LT entry.
 * \param [in] cur_entry Existing HITBIT_CONTROL LT entry.
 * \param [out] skip_index Dst/Src group/non-group action profile to be skipped.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_hitbit_ctrl_skip_idx_get(
    int unit,
    ctr_eflex_hitbit_ctrl_data_t *entry,
    ctr_eflex_hitbit_ctrl_data_t *cur_entry,
    uint32_t *skip_index);

/*!
 * \brief During HITBIT_CONTROL LT lookup operation, determine if corresponding
 * group/non-group action profiles are configured.
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   HITBIT_CONTROL LT entry.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_hitbit_ctrl_lookup_help(int unit,
                                         ctr_eflex_hitbit_ctrl_data_t *entry);

/*!
 * \brief Function to calculate hit index for hash LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table id with hitbit monitoring configured.
 * \param [in] sid Sid of hitbit physical bank.
 * \param [in] hit_context Hit context information.
 * \param [in] tbl_inst Pipe index.
 * \param [in] bkt_num Number of entries in bucket.
 * \param [in] bkt_entry Position of entry in bucket.
 * \param [in] context Flex counter context info.
 * \param [out] entry_idx Calculated hit index.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_ctr_eflex_hash_entry_index_calc(
    int unit,
    bcmltd_sid_t ltid,
    bcmdrd_sid_t sid,
    bcmptm_rm_hash_hit_context_t *hit_context,
    int tbl_inst,
    uint32_t bkt_num,
    uint32_t bkt_entry,
    void *context,
    uint32_t *entry_idx);

/*!
 * \brief Egress cmd bus object bit offset get function.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index in obj LT.
 * \param [out] bit_offset Min and max bit offsets.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmcth_ctr_egr_eflex_cmd_bus_lts_bit_offset_get(int unit,
                             int index,
                             bcmcth_ctr_eflex_obj_bit_offset_t *bit_offset);

/*!
 * \brief Egress obj bus object bit offset get function.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index in obj LT.
 * \param [out] bit_offset Min and max bit offsets.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmcth_ctr_egr_eflex_bus_bit_offset_get(int unit,
                             int index,
                             bcmcth_ctr_eflex_obj_bit_offset_t *bit_offset);

/*!
 * \brief Ingress obj bus object bit offset get function.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index in obj LT.
 * \param [out] bit_offset Min and max bit offsets.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmcth_ctr_ing_eflex_bus_bit_offset_get(int unit,
                             int index,
                             bcmcth_ctr_eflex_obj_bit_offset_t *bit_offset);

#endif /* BCMCTH_CTR_EFLEX_DRV_H */
