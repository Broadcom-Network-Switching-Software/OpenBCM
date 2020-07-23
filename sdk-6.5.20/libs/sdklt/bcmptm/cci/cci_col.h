/*! \file cci_col.h
 *
 * Interface functions for Counter Collection
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef CCI_COL_H
#define CCI_COL_H

/******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include "cci_internal.h"
#include "cci_cache.h"

/******************************************************************************
 * Defines
 */

/*!
* \brief Description of the counter collection structure
*/

typedef struct bcmptm_cci_col_s {
    const cci_context_t  *parent; /*!< parent context of parent */
    cci_handle_t hpol;            /*!< hpol handle for poll collection */
    cci_handle_t hevent;          /*!< hcache handle for event collection */
} bcmptm_cci_col_t;


/*!
 * \brief Initialize counter collection
 * \n Must be called every time a new unit is attached
 *
 * \param [in] unit Logical device id
 * \param [in] cci_con  context of cci
 * \param [out] handle of the current Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_init(int unit,
                    const cci_context_t  *cci_con,
                    cci_handle_t *handle);

/*!
 * \brief Clean up counter collection
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_cleanup(int unit,
                    cci_handle_t handle);


/*!
 * \brief Start counter collection
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current collection Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_run(int unit,
                   cci_handle_t handle);

/*!
 * \brief Stop counter collection subcomponent
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_stop(int unit,
                    cci_handle_t handle);

/*!
 * \brief Initialize poll based counter collection resources.
 * \n Must be called every time a new unit is attached
 *
 * \param [in] unit Logical device id
 * \param [in]con_col collection context
 * \param [out] handle of the current Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_poll_init(int unit,
                         const bcmptm_cci_col_t *con_col,
                         cci_handle_t *handle);

/*!
 * \brief Cleanup poll based counter collection resources.
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_poll_cleanup(int unit,
                            cci_handle_t handle);


/*!
 * \brief Start poll based counter collection.
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current poll Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_poll_run(int unit,
                        cci_handle_t handle);

/*!
 * \brief Stop poll based counter collection.
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_poll_stop(int unit,
                         cci_handle_t handle);

/*!
 * \brief Get Top level CCI Handle
 *
 * \param [in] unit Logical device id
 * \param [in]con_col collection context
 * \param [out] handle of CCI  Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_handle_get(int unit,
                      const bcmptm_cci_col_t *ctr_col,
                      cci_context_t **handle);

/*!
 * \brief Send collection configuration change message
 *
 * \param [in] unit Logical device id
 * \param [in] handle Current Instance handler
 * \param [in] cmd Configuration command
 * \param [in] config Configuration data
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_poll_config(int unit,
                           cci_handle_t handle,
                           cci_pol_cmd_t cmd,
                           cci_config_t *config);

/*!
 * \brief Send collection port event change message
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current Instance
 * \param [in] cmd Configuration command
 * \param [in] port Designated port number
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_poll_port_config(int unit,
                                cci_handle_t handle,
                                cci_pol_cmd_t cmd,
                                shr_port_t port);

/*!
 * \brief Get CCI  poll Handle
 *
 * \param [in] unit Logical device id
 * \param [in]con_col collection context
 * \param [out] handle of CCI  poll
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_poll_handle_get(int unit,
                           cci_handle_t hcol,
                           cci_handle_t *hpol);

/*!
 * \brief Send CCI message.
 *
 * \param [in] unit Logical device id
 * \param [in]msg message
 * \param [in] handle of CCI  poll:w
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_col_poll_msg(int unit,
                        col_thread_msg_t *msg,
                        cci_handle_t handle);


#endif /* CCI_COL_H */
