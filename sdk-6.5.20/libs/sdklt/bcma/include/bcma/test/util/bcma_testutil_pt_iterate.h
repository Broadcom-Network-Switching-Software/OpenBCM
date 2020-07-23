/*! \file bcma_testutil_pt_iterate.h
 *
 * Physical table iteration utility.
 *
 * It is used to iterate all registers or memories, and invoke
 * user-defined callback functions for each physical table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_PT_ITERATE_H
#define BCMA_TESTUTIL_PT_ITERATE_H

#include <bcmbd/bcmbd.h>

/*! Special name for iteration on all registers */
#define BCMA_TESTUTIL_PT_ALL_REGS "*r"

/*! Special name for iteration on all memories */
#define BCMA_TESTUTIL_PT_ALL_MEMS "*m"

/*! Special value for iteration on all block type */
#define BCMA_TESTUTIL_PT_ALL_BLOCK_TYPE -1

/*! Max entity handler number */
#define BCMA_TESTUTIL_PT_ENTITY_HANDLE_MAX  (4)

/*!
 * \brief The callback that will be executed while a physical table is found.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] user_data User-defined parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_pt_table_handle_f)(int unit, bcmdrd_sid_t sid,
                                               void *user_data);

/*!
 * \brief The callback that will be executed to cleanup initial condition.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_pt_table_cleanup_f)(int unit, bcmdrd_sid_t sid);

/*!
 * \brief The callback that will be executed while one entity of a physical table is found.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Device-specific structure of dynamic address components.
 * \param [in] ovrr_info Device-sepcific override structure, which can be used to override read individual pipes.
 * \param [in] user_data User-defined parameters.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_pt_entity_handle_f)(int unit,
                                                bcmdrd_sid_t sid,
                                                bcmbd_pt_dyn_info_t *dyn_info,
                                                void *ovrr_info,
                                                void *user_data);

/*!
 * \brief The callback that get the next entity index
 *
 * \param [in] unit Unit number.
 * \param [in] user_data User-defined parameters.
 *
 * \return Non-negative value on next entity index, or -1 for ending traverse
 */
typedef int (*bcma_testutil_pt_entity_next_id_f)(int unit, void *user_data);

/*!
 * \brief The parameters for physical table iteration.
 */
typedef struct bcma_testutil_pt_iter_s {
    /*! Unit number. */
    int unit;

    /*! Physical table name. */
    const char* name;

    /*! Block type. */
    int blktype;

    /*! The callback function for table. */
    bcma_testutil_pt_table_handle_f table_handle;

    /*! The callback function for table. */
    bcma_testutil_pt_table_cleanup_f table_cleanup;

    /*! The callback function for each entity. */
    bcma_testutil_pt_entity_handle_f entity_handle[BCMA_TESTUTIL_PT_ENTITY_HANDLE_MAX];

    /*! Whether to traverse each individual pipe in entity */
    bool entity_handle_pipes[BCMA_TESTUTIL_PT_ENTITY_HANDLE_MAX];

    /*! The callback function for the next entity id */
    bcma_testutil_pt_entity_next_id_f entity_next_id;

    /*! User-defined parameters. */
    void *user_data;
} bcma_testutil_pt_iter_t;

/*!
 * \brief Init physical table iteration.
 *
 * \param [in] pt_iter The parameters for iteration.
 *
 * \return None.
 */
extern void
bcma_testutil_pt_iterate_init(bcma_testutil_pt_iter_t *pt_iter);

/*!
 * \brief Physical table iteration.
 *
 * \param [in] pt_iter The parameters for iteration.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_pt_iterate(const bcma_testutil_pt_iter_t *pt_iter);

#endif /* BCMA_TESTUTIL_PT_ITERATE_H */

