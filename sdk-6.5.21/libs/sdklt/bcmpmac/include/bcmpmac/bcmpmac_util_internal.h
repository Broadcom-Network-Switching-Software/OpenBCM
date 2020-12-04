/*! \file bcmpmac_util_internal.h
 *
 * Utilities of BCM PortMAC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_UTIL_INTERNAL_H
#define BCMPMAC_UTIL_INTERNAL_H

#include <bcmpmac/bcmpmac_acc.h>


/*******************************************************************************
 * Utilities for the PMAC driver.
 */

/*!
 * \brief BCMPMAC port mode map.
 *
 * This structure is used to define the supported port lane map and specify the
 * corresponding PM port mode.
 */
typedef struct bcmpmac_port_mode_map_s {

    /*! Port lane map. */
    uint32_t lane_map[BCMPMAC_NUM_PORTS_PER_PM_MAX];

    /*! Port mode value. */
    uint32_t port_mode;

} bcmpmac_port_mode_map_t;

/*!
 * \brief BCMPMAC function type for the string-based API.
 */
typedef int
(*bcmpmac_drv_op_f)(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t param);

/*!
 * \brief BCMPMAC port mode map.
 *
 * This structure is used to define the operation for the string-based APIs.
 */
typedef struct bcmpmac_drv_op_hdl_s {

    /*! Operation name. */
    char *name;

    /*! Operation handler. */
    bcmpmac_drv_op_f func;

} bcmpmac_drv_op_hdl_t;

/*!
 * \brief Retrieve the PM port mode value for the given port lane maps.
 *
 * Get the PM port mode value \c mval by searching the PM mode mapping \c map
 * for the given port lane maps \c lane_map.
 *
 * \param [in] modes Port mode map.
 * \param [in] mode_num The entry number of \c modes.
 * \param [in] lane_map Port lane maps.
 * \param [out] mval PM port mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM No PM port mode is defined for the given lane maps.
 */
int
bcmpmac_port_mode_get(const bcmpmac_port_mode_map_t *modes, int mode_num,
                      uint32_t *lane_map, uint32_t *mval);

/*!
 * \brief Retrieve the function pointer of the given operation name.
 *
 * Get the function pointer by searching the operation list \c ops for the
 * given operation string \c op_name.
 *
 * \param [in] ops Supported operations.
 * \param [in] op_num The number of the supported operations.
 * \param [in] op_name The operation name to search.
 *
 * \return The function pointer of the given operation name or NULL when not
 *         found.
 */
bcmpmac_drv_op_f
bcmpmac_drv_op_func_get(const bcmpmac_drv_op_hdl_t *ops, int op_num,
                        const char *op_name);

/*******************************************************************************
 * Register access functions.
 */

/*!
 * \brief BCMPMAC read function
 *
 * Read data from the given register
 *
 * \param [in] pm_acc PortMAC access info
 * \param [in] pm_pport The port index in the block
 * \param [in] address Reg/Mem offset
 * \param [in] idx Entry index
 * \param [in] size Data size in byte
 * \param [out] data Data buffer
 *
 * \retval 0 No errors
 */
extern int
bcmpmac_read(bcmpmac_access_t *pm_acc, bcmpmac_pport_t pm_pport,
             uint32_t address, uint32_t idx, size_t size, uint32_t *data);

/*!
 * \brief Write register data
 *
 * Write data to the given register
 *
 * \param [in] pm_acc PortMAC access info
 * \param [in] pm_pport The port index in the block
 * \param [in] address Reg/Mem offset
 * \param [in] idx Entry index
 * \param [in] size Data size in byte
 * \param [in] data Data buffer
 *
 * \retval 0 No errors
 */
extern int
bcmpmac_write(bcmpmac_access_t *pm_acc, bcmpmac_pport_t pm_pport,
              uint32_t address, uint32_t idx, size_t size, uint32_t *data);


/*******************************************************************************
 * Filed operations for little endian word order (normal)
 */

/*!
 * \brief Extract multi-word field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] sbit bit number of first bit of the field to extract
 * \param [in] ebit bit number of last bit of the field to extract
 * \param [out] fbuf buffer where to store extracted field value
 *
 * \return Pointer to extracted field value.
 */
extern uint32_t *
bcmpmac_field_get(const uint32_t *entbuf, int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Assign multi-word field value in multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] sbit bit number of first bit of the field to assign
 * \param [in] ebit bit number of last bit of the field to assign
 * \param [in] fbuf buffer with new field value
 *
 * \return Nothing.
 */
extern void
bcmpmac_field_set(uint32_t *entbuf, int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Extract field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] sbit bit number of first bit of the field to extract
 * \param [in] ebit bit number of last bit of the field to extract
 *
 * \return Extracted field value.
 */
extern uint32_t
bcmpmac_field32_get(const uint32_t *entbuf, int sbit, int ebit);

/*!
 * \brief Assign field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] sbit bit number of first bit of the field to assign
 * \param [in] ebit bit number of last bit of the field to assign
 * \param [in] fval the value of the field to assign
 *
 * \return Nothing.
 */
extern void
bcmpmac_field32_set(uint32_t *entbuf, int sbit, int ebit, uint32_t fval);


/*******************************************************************************
 * Filed operations for big endian word order (some exceptions)
 */

/*!
 * \brief Extract multi-word field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] wsize size of entbuf counted in 32-bit words
 * \param [in] sbit bit number of first bit of the field to extract
 * \param [in] ebit bit number of last bit of the field to extract
 * \param [out] fbuf buffer where to store extracted field value
 *
 * \return Pointer to extracted field value.
 */
extern uint32_t *
bcmpmac_field_be_get(const uint32_t *entbuf, int wsize,
                     int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Assign multi-word field value in multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] wsize size of entbuf counted in 32-bit words
 * \param [in] sbit bit number of first bit of the field to assign
 * \param [in] ebit bit number of last bit of the field to assign
 * \param [in] fbuf buffer with new field value
 *
 * \return Nothing.
 */
extern void
bcmpmac_field_be_set(uint32_t *entbuf, int wsize,
                     int sbit, int ebit, uint32_t *fbuf);

/*!
 * \brief Extract field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] wsize size of entbuf counted in 32-bit words
 * \param [in] sbit bit number of first bit of the field to extract
 * \param [in] ebit bit number of last bit of the field to extract
 *
 * \return Extracted field value.
 */
extern uint32_t
bcmpmac_field32_be_get(const uint32_t *entbuf, int wsize,
                       int sbit, int ebit);

/*!
 * \brief Assign field value from multi-word register/memory.
 *
 * \param [in] entbuf current contents of register/memory (word array)
 * \param [in] wsize size of entbuf counted in 32-bit words
 * \param [in] sbit bit number of first bit of the field to assign
 * \param [in] ebit bit number of last bit of the field to assign
 * \param [in] fval the value of the field to assign
 *
 * \return Nothing.
 */
extern void
bcmpmac_field32_be_set(uint32_t *entbuf, int wsize,
                       int sbit, int ebit, uint32_t fval);

#endif /* BCMPMAC_UTIL_INTERNAL_H */
