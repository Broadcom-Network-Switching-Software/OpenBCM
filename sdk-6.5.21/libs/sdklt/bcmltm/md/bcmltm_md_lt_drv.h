/*! \file bcmltm_md_lt_drv.h
 *
 * Logical Table Manager - Logical Table Driver Definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_LT_DRV_H
#define BCMLTM_MD_LT_DRV_H


#include <shr/shr_types.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmltm/bcmltm_md_internal.h>


/*!
 * \brief LTM LT Function Vector Driver.
 */
typedef struct bcmltm_md_lt_drv_s {
    /*! Destroy opcode metadata object. */
    void (*op_destroy)(bcmltm_lt_op_md_t *op_md);

    /*! Create INSERT opcode metadata object. */
    int (*op_insert_create)(int unit, bcmlrd_sid_t sid,
                            bcmltm_lt_op_md_t **op_md_ptr);

    /*! Create LOOKUP opcode metadata object. */
    int (*op_lookup_create)(int unit, bcmlrd_sid_t sid,
                            bcmltm_lt_op_md_t **op_md_ptr);

    /*! Create DELETE opcode metadata object. */
    int (*op_delete_create)(int unit, bcmlrd_sid_t sid,
                            bcmltm_lt_op_md_t **op_md_ptr);

    /*! Create UPDATE opcode metadata object. */
    int (*op_update_create)(int unit, bcmlrd_sid_t sid,
                            bcmltm_lt_op_md_t **op_md_ptr);

    /*! Create TRAVERSE opcode metadata object. */
    int (*op_traverse_create)(int unit, bcmlrd_sid_t sid,
                              bcmltm_lt_op_md_t **op_md_ptr);
} bcmltm_md_lt_drv_t;


/*!
 * \brief Get the Index LT driver.
 *
 * Get the driver for Direct Mapped Index logical tables
 * with no field selection.
 * This covers Simple Index and Index With Allocation tables, both as
 * Shared(Overlay) and no-Shared.
 *
 * \param [in] void.
 *
 * \retval Returns pointer to Index LT driver.
 */
extern const bcmltm_md_lt_drv_t *
bcmltm_md_lt_index_drv_get(void);


/*!
 * \brief Get the Index Field Select LT driver.
 *
 * Get the driver for Direct Mapped Index logical tables with
 * field selections.
 * This covers Simple Index and Index With Allocation tables, both as
 * Shared(Overlay) and no-Shared.
 *
 * \param [in] void.
 *
 * \retval Returns pointer to Index Field Select LT driver.
 */
extern const bcmltm_md_lt_drv_t *
bcmltm_md_lt_index_fs_drv_get(void);

/*!
 * \brief Get the Keyed Hash and TCAM LT driver.
 *
 * Get the driver for Direct Mapped Keyed Hash and TCAM logical tables
 * with no field selection.
 *
 * \param [in] void.
 *
 * \retval Returns pointer to Keyed LT driver.
 */
extern const bcmltm_md_lt_drv_t *
bcmltm_md_lt_keyed_drv_get(void);

/*!
 * \brief Get the Keyed Field Select LT driver.
 *
 * Get the driver for Direct Mapped Keyed Hash and TCAM logical tables
 * with field selections.
 *
 * \param [in] void.
 *
 * \retval Returns pointer to Keyed Field Select LT driver.
 */
extern const bcmltm_md_lt_drv_t *
bcmltm_md_lt_keyed_fs_drv_get(void);

/*!
 * \brief Get the Custom Table Handler LT driver.
 *
 * Get the driver for the LTA Custom Table Handler logical tables.
 *
 * \param [in] void.
 *
 * \retval Returns pointer to Custom Table Handler LT driver.
 */
extern const bcmltm_md_lt_drv_t *
bcmltm_md_lta_cth_drv_get(void);


/*!
 * \brief Get the LTM based LT driver.
 *
 * Get the driver for the logical tables with LTM map group,
 * e.g. LT_CONFIG, LT_STATS.
 *
 * \param [in] void.
 *
 * \retval Returns pointer to LTM based LT driver.
 */
extern const bcmltm_md_lt_drv_t *
bcmltm_md_lt_ltm_drv_get(void);


#endif /* BCMLTM_MD_LT_DRV_H */
