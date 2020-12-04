/*! \file bcmcth_state_eflex_drv.h
 *
 * BCMCTH STATE_EFLEX Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BCMBD) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_STATE_EFLEX_DRV_H
#define BCMCTH_STATE_EFLEX_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/*!
 * \brief Get counter tables list.
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] inst    Ingress instance.
 * \param [out] ctr_table_list  List of counter tables.
 * \param [out] num  Number of counter tables.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_state_eflex_ctr_table_get_f)(int unit,
                                      bool ingress,
                                      state_eflex_inst_t inst,
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
(*bcmcth_state_eflex_device_info_get_f)(
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
(*bcmcth_state_eflex_action_reg_set_f)(int unit,
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
(*bcmcth_state_eflex_action_table_write_f)(
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
(*bcmcth_state_eflex_bitp_profile_set_f)(int unit);

/*!
 * \brief Write operand profile table entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Operand profile entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_state_eflex_op_profile_write_f)(
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
(*bcmcth_state_eflex_range_chk_write_f)(
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
(*bcmcth_state_eflex_trigger_write_f)(
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
(*bcmcth_state_eflex_trigger_read_f)(
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
(*bcmcth_state_eflex_group_action_write_f)(
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
(*bcmcth_state_eflex_object_id_get_f)(
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
(*bcmcth_state_eflex_logical_object_id_get_f)(
    int unit,
    int select,
    uint64_t data);

/*!
 * \brief Write error stats entry
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry   Error stats entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_state_eflex_error_stats_write_f)(
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
(*bcmcth_state_eflex_error_stats_read_f)(
    int unit,
    ctr_eflex_error_stats_data_t *entry);

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
(*bcmcth_state_eflex_shadow_pool_set_f)(int unit,
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
(*bcmcth_state_eflex_timestamp_init_f)(int unit);

/*!
 * \brief Write object quantization entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Object quantization entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_state_eflex_obj_quant_write_f)(int unit,
                                        ctr_eflex_obj_quant_data_t *entry);
/*!
 * \brief Write truth table entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Object quantization entry info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_state_eflex_truth_table_write_f)(int unit,
                                        ctr_eflex_truth_table_data_t *entry);


/*!
 * \brief Get flex state object info
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in,out] num_obj Number of flex state objects.
 * \param [out] obj Array of flex state object values.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmcth_state_eflex_obj_info_get_f)(int unit,
                                     bool ingress,
                                     uint32_t *num_obj,
                                     uint32_t *obj);
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
(*bcmcth_state_eflex_pipe_valid_check_f)(int unit,
                                         bool ingress,
                                         uint32_t pipe,
                                         bool *valid);

/*!
 * \brief STATE EFLEX driver object
 *
 * STATE EFLEX driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH STATE EFLEX module by
 * \ref bcmcth_state_eflex_drv_init() from the top layer. BCMCTH STATE EFLEX
 * internally will use this interface to get the corresponding information.
 */
typedef struct bcmcth_state_eflex_drv_s {
    /*! Get device info */
    bcmcth_state_eflex_device_info_get_f    device_info_get_fn;
    /*! Get counter tables list */
    bcmcth_state_eflex_ctr_table_get_f      ctr_table_get_fn;
    /*! Set counter action enable reg */
    bcmcth_state_eflex_action_reg_set_f     action_reg_set_fn;
    /*! Write counter action table 0 & 1 entry */
    bcmcth_state_eflex_action_table_write_f action_table_write_fn;
    /*! Set bitp profile */
    bcmcth_state_eflex_bitp_profile_set_f   bitp_profile_set_fn;
    /*! Write operand profile table entry */
    bcmcth_state_eflex_op_profile_write_f   op_profile_write_fn;
    /*! Write range check profile table entry */
    bcmcth_state_eflex_range_chk_write_f    range_chk_write_fn;
    /*! Write trigger table entry */
    bcmcth_state_eflex_trigger_write_f      trigger_write_fn;
    /*! Read trigger table entry */
    bcmcth_state_eflex_trigger_read_f       trigger_read_fn;
    /*! Write group action table entry */
    bcmcth_state_eflex_group_action_write_f group_action_write_fn;
    /*! Get object id */
    bcmcth_state_eflex_object_id_get_f      object_id_get_fn;
    /*! Write error stats entry */
    bcmcth_state_eflex_error_stats_write_f  error_stats_write_fn;
    /*! Read error stats entry */
    bcmcth_state_eflex_error_stats_read_f   error_stats_read_fn;
    /*! Get logical object id */
    bcmcth_state_eflex_logical_object_id_get_f logical_object_id_get_fn;
    /*! Set shadow pool registers */
    bcmcth_state_eflex_shadow_pool_set_f    shadow_pool_set_fn;
    /*! Initialize timestamp registers */
    bcmcth_state_eflex_timestamp_init_f     timestamp_init_fn;
    /*! Write object quantization entry */
    bcmcth_state_eflex_obj_quant_write_f    obj_quant_write_fn;
    /*! Write truth table entry */
    bcmcth_state_eflex_truth_table_write_f  truth_table_write_fn;
    /*! Get object info */
    bcmcth_state_eflex_obj_info_get_f       obj_info_get_fn;
    /*! Verify pipe number */
    bcmcth_state_eflex_pipe_valid_check_f     pipe_valid_check_fn;
} bcmcth_state_eflex_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,\
                             _fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_state_eflex_drv_t *_bd##_cth_state_eflex_drv_get(int unit);
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
bcmcth_state_eflex_device_info_get(
    int unit,
    bcmcth_ctr_eflex_device_info_t **device_info);

/*!
 * \brief Get counter tables list.
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] inst    Ingress instance.
 * \param [out] ctr_table_list  List of counter tables.
 * \param [out] num  Number of counter tables.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_state_eflex_ctr_table_get(int unit,
                                 bool ingress,
                                 state_eflex_inst_t inst,
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
bcmcth_state_eflex_action_reg_set(int unit,
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
bcmcth_state_eflex_action_table_write(int unit,
                                      ctr_eflex_action_profile_data_t *entry);

/*!
 * \brief Set bitp profile to default values
 *
 * \param [in] unit    Unit number.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_state_eflex_bitp_profile_set(int unit);

/*!
 * \brief Write operand profile table entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Operand profile entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_state_eflex_op_profile_write(int unit,
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
bcmcth_state_eflex_range_chk_write(int unit,
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
bcmcth_state_eflex_trigger_write(int unit,
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
bcmcth_state_eflex_trigger_read(int unit,
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
bcmcth_state_eflex_group_action_write(int unit,
                                      ctr_eflex_group_action_data_t *entry);

/*!
 * \brief Get the STATE EFLEX driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_state_eflex_drv_init(int unit);

/*!
 * \brief Cleanup the STATE EFLEX driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_state_eflex_drv_cleanup(int unit);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The STATE EFLEX custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmcth_state_eflex_imm_init(int unit);

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
bcmcth_state_eflex_obj_id_get(int unit, int select, uint64_t data);

/*!
 * \brief Convert parameter to an logical object id.
 *
 * \param [in] unit Unit number.
 * \param [in] select Ingress or Egress.
 * \param [in] data Value of object.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_state_eflex_logical_obj_id_get(int unit, int select, uint64_t data);

/*!
 * \brief Write error stats entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Error stats entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_state_eflex_error_stats_write(int unit,
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
bcmcth_state_eflex_error_stats_read(int unit,
                                    ctr_eflex_error_stats_data_t *entry);

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
bcmcth_state_eflex_shadow_pool_set(int unit,
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
bcmcth_state_eflex_timestamp_init(int unit);

/*!
 * \brief Write object quantization entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Object quantization entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_state_eflex_obj_quant_write(int unit,
                                   ctr_eflex_obj_quant_data_t *entry);
/*!
 * \brief Write truth table entry
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   truth table entry info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_state_eflex_truth_table_write(int unit,
                                   ctr_eflex_truth_table_data_t *entry);


/*!
 * \brief Get flex state object info
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in,out] num_obj Number of flex state objects.
 * \param [out] obj Array of flex state object values.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_state_eflex_obj_info_get(int unit,
                                bool ingress,
                                uint32_t *num_obj,
                                uint32_t *obj);

/*!
 * \brief Egress flex state obj bus object bit offset get function.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index in obj LT.
 * \param [out] bit_offset Min and max bit offsets.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmcth_state_eflex_egr_obj_bus_bit_offset_get(
    int unit,
    int index,
    bcmcth_ctr_eflex_obj_bit_offset_t *bit_offset);

/*!
 * \brief  Post lookup flex state obj bus object bit offset get function.
 *
 * \param [in] unit Unit number.
 * \param [in] index Index in obj LT.
 * \param [out] bit_offset Min and max bit offsets.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmcth_state_eflex_post_lkup_obj_bus_bit_offset_get(
    int unit,
    int index,
    bcmcth_ctr_eflex_obj_bit_offset_t *bit_offset);

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
bcmcth_state_eflex_pipe_valid_check(int unit,
                                    bool ingress,
                                    uint32_t pipe,
                                    bool *valid);

#endif /* BCMCTH_STATE_EFLEX_DRV_H */
