/*! \file bcmtm_utils_internal.h
 *
 * Internal  utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_UTILS_INTERNAL_H
#define BCMTM_UTILS_INTERNAL_H

#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd_intr.h>
#include <bcmtm/generated/bcmtm_ha.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmpc/bcmpc_tm.h>


/*! Interrupt controls. */
typedef enum
bcmtm_intr_enable_e {
    /*! Interrupt enable. */
    INTR_ENABLE = 0,

    /*! Interrupt disable. */
    INTR_DISABLE = 1,

    /*! Clear the interrupts. */
    INTR_CLEAR = 2
} bcmtm_intr_enable_t;


/*! BCMTM queue types. */
typedef enum
bcmtm_q_type_e {
    /*! Unicast queue. */
    UC_Q = 0,
    /*! Multicast queue. */
    MC_Q,
} bcmtm_q_type_t;

/*! BCMTM port informations. */
typedef
struct bcmtm_port_info_s {
   /*! Logical port. */
   int log_port;
   /*! Physical port. */
   int phy_port;
   /*! Pipe. */
   int pipe;
   /*! IDB port. */
   int idb_port;
   /*! TM port. */
   int mmu_port;
} bcmtm_port_info_t;

/*!
 * BCMTM port mapping informations.
 * This has the information fo logical port, physical port and mmu port
 * mappings for the device.
 */
typedef struct
bcmtm_port_map_info_s {

    /*! Logical port map. */
    bcmtm_lport_map_t *lport_map;

    /*! Physical port map. */
    bcmtm_pport_map_t *pport_map;

    /*! MMU port map. */
    bcmtm_mport_map_t *mport_map;
} bcmtm_port_map_info_t;
/*!
 * \brief validates the logical table field data and key information.
 *
 * Return if the fields and data corresponding to the fields and keys for
 * logical table is right or not.
 *
 * \param [in] unit         Unit number.
 * \param [in] opcode       Opcode for the logical table (insert/delete/update/lookup).
 * \param [in] sid          Logical table sid.
 * \param [in] in           In field list.
 *
 * \retval 1      in field list is correct.
 * \retval 0      in field list is incorrect.
 */
int
bcmtm_lt_field_validate (int unit,
                        bcmlt_opcode_t opcode,
                        bcmlrd_sid_t sid,
                        const bcmltd_fields_t *in);

/*!
 * \brief parses in in list and prepares a data list
 *
 * prepare data list with the in parameters and default parameters
 *
 * \param [in] unit         Unit number.
 * \param [in] in           in field list .
 * \param [in] sid          Logical table sid.
 * \param [in] num_fid      max number of fields for the given logical table
 * \param [out] in_data     data list for all the fields
 */
int
bcmtm_in_field_parser (int unit,
                       const bcmltd_fields_t *in,
                       bcmlrd_sid_t sid,
                       size_t num_fid,
                       uint32_t *in_data);

/*!
 * \brief allocate memory to make the field list
 *
 * verifies if the operation requested is valid for the input list or not.
 *
 * \param [in] unit         Unit number.
 * \param [in] num_fields   number of fields.
 * \param [out] flist       allocated memory for field list
 */
int
bcmtm_field_list_alloc(int unit,
                       size_t num_fields,
                       bcmltd_fields_t *flist);

/*!
 * \brief frees the allocated field list
 *
 * verifies if the operation requested is valid for the input list or not.
 *
 * \param [in]          flist allocated field list .
 */
void
bcmtm_field_list_free(bcmltd_fields_t *flist);

/*!
 * \brief frees the bcmltd_field_t linked list
 *
 * \param [in] head Head of the field linked list.
 */
void
bcmtm_field_linked_list_free(bcmltd_field_t *head);

/*!
 * \brief set the value of a field in the field list.
 *
 * Not applicable to array fields.
 *
 * \param [in] unit  Unit number.
 * \param [in] flist Allocated field list.
 * \param [in] fid   The fid of the field to be set.
 * \param [in] idx   The idx for array based fields.
 * \param [in] val   Value to set.
 *
 * \retval SHR_E_FULL No place to add new element to the field list.
 */
int
bcmtm_field_list_set(int unit,
                     bcmltd_fields_t *flist,
                     bcmlrd_fid_t fid,
                     uint32_t idx,
                     uint64_t val);

/*!
 * \brief get the value of a field in the field list.
 *
 * Not applicable to array fields.
 *
 * \param [in]  unit  Unit number.
 * \param [in]  flist Allocated field list.
 * \param [in]  fid   The fid of the field to retrieve.
 * \param [in] idx   The idx for array based fields.
 * \param [out] val  The field value.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval SHR_E_NOT_FOUND Cannot find the field in the field list.
 */
int
bcmtm_field_list_get(int unit,
                     bcmltd_fields_t *flist,
                     bcmlrd_fid_t fid,
                     uint32_t idx,
                     uint64_t *val);

/*!
 * \brief returns field value range
 *
 * returns the min and max values supported for a field
 *
 * \param [in] unit         Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Table Field.
 * \param [out] min          min value
 * \param [out] max          max value
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the field definition.
 */
int
bcmtm_field_value_range_get(int unit, bcmlrd_sid_t sid, uint32_t fid,
                            uint64_t *min, uint64_t *max);


/*!
 * \brief Default values for the field.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] fid Table Field.
 * \param [out] def Default field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the field definition.
 */
extern int
bcmtm_field_default_value_get(int unit,
        bcmlrd_sid_t sid,
        uint32_t fid,
        uint64_t *def);

/*!
 * \brief Get the field value from field array for Logical table field ID.
 *
 * \param [in] unit  Unit number.
 * \param [in] fid   Logical table field ID.
 * \param [in] idx   Logical table array field index.
 * \param [in] field_array Field array having field id and field_value.
 * \param [out] fval field value for the logical table field ID.
 *
 * \return SHR_E_NONE No error.
 * \return SHR_E_NOT_FOUND field ID not found in field array.
 */
extern int
bcmtm_fval_get_from_field_array(int unit,
        bcmlrd_fid_t fid,
        uint32_t idx,
        const bcmltd_field_t *field_array,
        uint64_t *fval);

/*!
 * \brief Construct an LT entry in an array format (bcmltd_fields_t) with all
 * the fields populated to default values.
 *
 * All fields (including key fields) that are valid to the given unit will be
 * populated. If the entry requires valid key fields, the user must set the key
 * fields after calling this function. This function does not work for LTs
 * containing any array field.
 *
 * \param [in] unit  Unit number.
 * \param [in] sid   Logical table ID.
 * \param [out] entry bcmltd_fields_t pointer to a allocated field list.
 *
 * \return SHR_E_NONE No error.
 * \return SHR_E_NOT_FOUND Failed to retrieve field ID or default value from LRD.
 * \return SHR_E_MEMORY Failed to allocate the data structure.
 */
extern int
bcmtm_default_entry_array_construct(int unit, bcmlrd_sid_t sid,
                                    bcmltd_fields_t *entry);

/*!
 * \brief Construct an LT entry in a linked list format (bcmltd_field_t) with
 * all the fields populated to default values.
 *
 * All fields (including key fields) that are valid to the given unit will be
 * populated. If the entry requires valid key fields, the user must set the key
 * fields after calling this function. This function does not work for LTs
 * containing any array field.
 *
 * \param [in] unit  Unit number.
 * \param [in] sid   Logical table ID.
 * \param [out] data_fields Address of the bcmltd_field_t pointer.
 *
 * \return SHR_E_NONE No error.
 * \return SHR_E_NOT_FOUND Failed to retrieve field ID or default value from LRD.
 * \return SHR_E_MEMORY Failed to allocate the data structure.
 */
extern int
bcmtm_default_entry_ll_construct(int unit, bcmlrd_sid_t sid,
                                 bcmltd_field_t **data_fields);

/*!
 * \brief Check the size of the passed output_fields in IMM callback.
 *
 * In IMM callback functions, output_fields is used to update read-only LT
 * fields. However, the memory is allocated by IMM and it is not guaranteed to
 * be large enough to hold all the read-only fields in the LT. This check should
 * always be performed against the output_fields to avoid any memory issue.
 *
 * \param [in] unit  Unit number.
 * \param [in] flist Pointer to the output_fields.
 * \param [in] size The desired number of fields.
 *
 * \return SHR_E_NONE No error.
 * \return SHR_E_MEMORY output_fields is too small for the number of fields.
 */
extern int
bcmtm_field_list_size_check(int unit, bcmltd_fields_t *flist, size_t size);

/*!
 * \brief Print drv_info for debug.
 *
 * \param [in] unit  Unit number.
 */
extern void
bcmtm_drv_info_print(int unit);

/*!
 * \brief Get the field width in bits.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] fid Logical table field ID.
 * \param [out] width Width of the field.
 */
extern int
bcmtm_field_width_get(int unit,
                      bcmltd_sid_t ltid,
                      bcmltd_fid_t fid,
                      size_t *width);

/*!
 * \brief Get the address for device specific driver info.
 *
 * \param [in] unit Unit number.
 * \param [out] drv_info Driver information.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL Driver information not available.
 */
extern int
bcmtm_drv_info_get(int unit,
                   bcmtm_drv_info_t **drv_info);

/*!
 * \brief Get the address for device specific threshold related SW states.
 *
 * \param [in] unit Unit number.
 * \param [out] thd_info Threshold feature information.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL Threshold feature information not available.
 */
extern int
bcmtm_thd_info_get(int unit,
                   bcmtm_thd_info_t **thd_info);

/*!
 * \brief Get the address for OBM max usage port bitmap.
 *
 * \param [in] unit Unit number.
 * \param [out] obm_usage_pbmp OBM usage port bitmap.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNVAIL Information not available.
 */
extern int
bcmtm_obm_usage_pbmp_info_get(int unit, bcmtm_obm_usage_info_t **obm_usage_pbmp);

/*!
 * \brief Allocate HA memory for OBM max usage port bitmap.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNVAIL Information not available.
 */
extern int
bcmtm_obm_usage_pbmp_info_alloc(int unit, bool warm);

/*!
 * \brief Get the current context for flexport.
 *
 * \param [in] unit Unit number.
 *
 * \retval 0 Current context is NOT flexport.
 * \retval 1 Current context is flexport.
 */
extern int
bcmtm_is_flexport(int unit);

/*!
 * \brief BCMTM hardware interrupt enable/disable.
 *
 * \param [in] unit Logical unit number.
 * \param [in] intr_num Interrupt number.
 * \param [in] intr_func Interrupt callback function.
 * \param [in] enable Enable/disable/clear interrupts.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_interrupt_enable(int unit,
                       int intr_num,
                       bcmbd_intr_f intr_func,
                       bcmtm_intr_enable_t enable);

/*!
 * \brief BCMTM instrument interrupt enable/disable.
 *
 * \param [in] unit Logical unit number.
 * \param [in] intr_num Interrupt number.
 * \param [in] intr_func Interrupt callback function.
 * \param [in] enable Enable/disable/clear interrupts.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_mmui_interrupt_enable(int unit,
                            int intr_num,
                            bcmbd_intr_f intr_func,
                            bcmtm_intr_enable_t enable);

/*!
 * \brief Get BCMTM multicast resource free count.
 *
 * \param [in] unit Logical unit number.
 * \param [out] free_resource Free multicast resource (repl list) count.
 *
 * \return SHR_E_NONE No error.
 * \return SHR_E_UNAVAIL Feature not available.
 */
extern int
bcmtm_mc_free_resource_count_get (int unit,
                                  uint64_t *free_resource);

/*!
 * \brief Find if the port is a HiGig 3 port.
 *
 * \param [in]  unit Logical unit number.
 * \param [in]  lport Logical port.
 * \param [out] hg3 Port is higig 3.
 *
 * \return SHR_E_NONE No error.
 */
extern int
bcmtm_port_is_higig3 (int unit, bcmtm_lport_t lport, int *hg3);

/*!
 * \brief TM port level init/config function.
 *
 * Perform all port configuration dependent TM configurations.
 *
 * This function is bound to the port control(PC) driver and will only be invoked by PC during
 * initialization or after a port update.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of ports in the port config.
 * \param [in] old_cfg Old MMU port config.
 * \param [in] new_cfg New MMU port config.
 *
 * \retval none
 * \retval SHR_E_PARAM Hardware table not found.
 * \retval SHR_E_NOT_FOUND  Field infomation not found for SID and FID.
 */

extern int
bcmtm_pc_tm_configure(int unit,
                      size_t num_ports,
                      const bcmpc_mmu_port_cfg_t *old_cfg,
                      const bcmpc_mmu_port_cfg_t *new_cfg);


/*! \brief Get device port map information.
 *
 * \param [in] unit Logical unit number.
 * \param [out] port_map Port mapping.
 */
extern void
bcmtm_port_map_get(int unit, bcmtm_port_map_info_t **port_map);

/*! \brief Set device port map information.
 *
 * \param [in] unit Logical unit number.
 * \param [out] port_map Port mapping.
 */
extern int
bcmtm_port_map_set(int unit, bcmtm_port_map_info_t *port_map);

extern void
bcmtm_pfc_ddr_info_get(int unit, bcmtm_pfc_ddr_info_t **pfc_ddr_info);

extern int
bcmtm_pfc_ddr_info_set(int unit, bcmtm_pfc_ddr_info_t *pfc_ddr_info);
#endif /* BCMTM_UTILS_INTERNAL_H */
