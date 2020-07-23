/*! \file cci_internal.h
 *
 * Defines shared across CCI sub-modules
 *
 * This file contains utils, defines shared across CCI sub-modules
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef CCI_INTERNAL_H
#define CCI_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_thread.h>
#include <sal/sal_time.h>
#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmptm/generated/bcmptm_cci_ha.h>

/*!
 * \brief CCI time out in microseconds.
 */
#define CFG_CCI_TIME_OUT      10000000

/*!
 * \brief Message Q time out
 */
#define MSGQ_TIME_OUT      1000000

/*!
 * \brief Retry Count
 */
#define CFG_SER_RETRY_COUNT      3

/*!
 * \brief Max port register group for DMA process
 */
#define CCI_MAX_PORT_REG_GROUP   (20)

/*!
 * \brief MAx Physical port count (words * sizeof word * bits in byte)
 */
#define CCI_MAX_PPORT_COUNT      (BCMDRD_PBMP_WORD_MAX * 4 * 8)

/*!
 * \brief Time Delay in usec
 */
#define CCI_DELAY_TIME(tm)      (sal_time_usecs() - (tm))


/*! BSL_LOG_MODULE used for SHR_FUNC_ENTER, EXIT
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CCI

/*! Maximum number of threads that can be used for counter collection
 */
#define CCI_COL_MAX_THREADS     (1)

/*!  IMM LT names.
 */
#define CTR_CFG_LT_NM               "CTR_CONTROL"
#define CTR_SYNC_STATE_LT_NM        "CTR_EVENT_SYNC_STATE"
#define CTR_SYNC_STATE_CTRL_LT_NM   "CTR_EVENT_SYNC_STATE_CONTROL"


/*!
 * \brief Description CCI  state information
 */

typedef enum {
    /*! System Manager Init  */
    CCI_STATE_INIT = 1,

    /*! System Manager Configure  */
    CCI_STATE_CONFIG,

    /*! System Manager Run  */
    CCI_STATE_RUN,

    /*! System Manager Stop  */
    CCI_STATE_STOP,

    /*! System Manager Shut down */
    CCI_STATE_SHUTDOWN
} cci_state_t;

/*!
 * Counter sync event states.
 */
typedef enum {
    /*! Counter sync is off. */
    OFF = 0,

    /*! Counter sync started.*/
    START = 1,

    /*! Counter sync completed.*/
    COMPLETE = 2

} cci_sync_event_state_t;


/*!
* \brief Thread message Queue commands
*/

typedef enum {
    MSG_CMD_POL_CONFIG_COL_EN = 1,
    MSG_CMD_POL_CONFIG_DMA,
    MSG_CMD_POL_CONFIG_INTERVAL,
    MSG_CMD_POL_CONFIG_PORT,
    MSG_CMD_POL_CONFIG_MULTIPLIER_PORT,
    MSG_CMD_POL_CONFIG_MULTIPLIER_EPIPE,
    MSG_CMD_POL_CONFIG_MULTIPLIER_IPIPE,
    MSG_CMD_POL_CONFIG_MULTIPLIER_TM,
    MSG_CMD_POL_CONFIG_MULTIPLIER_EVICT,
    MSG_CMD_POL_CONFIG_MULTIPLIER_SEC,
    MSG_CMD_EVICTION_THRESHOLD,
    MSG_CMD_FIFODMA_REQ,
    MSG_CMD_POL_PORT_ADD,
    MSG_CMD_POL_PORT_DEL,
    MSG_CMD_POL_STOP,
    MSG_CMD_HIT_BIT_SET,
    MSG_CMD_POL_STAT_SYNC,
    MSG_CMD_CTR_ENTRY_MOVE,
} cci_pol_cmd_t;

/*!
* \brief Description of counter thread message
*/

typedef struct col_thread_msg_s {
    /*! Collection thread command types */
   cci_pol_cmd_t cmd;
    /*! message data */
   uint64_t      data[BCMDRD_PBMP_WORD_MAX];
}  col_thread_msg_t;

/*!
 * \brief Description of the current configuration
 */
typedef struct cci_config_s {
    /*! Enable collection */
    bool col_enable;

    /*! Number of threads */
    uint8_t nthreads;

    /*! Interval time period in microseconds */
    uint32_t interval;

    /*! DMA based collection */
    bool dma;

    /*! Interval multiplier */
    uint8_t multiplier[CCI_CTR_TYPE_NUM];

    /*! Valid port list against pbmp. */
    bcmdrd_pbmp_t valid_pbmp;

    /*! Ports active ports */
    bcmdrd_pbmp_t pbmp;

    /*! Threshold for eviction counter collection. */
    uint32_t eviction_threshold;
} cci_config_t;


/*!
 * \brief In memory table resources
 */
typedef struct config_inmem_s {
    /*! Config Logical Table ID */
    bcmltd_sid_t sid;

    /*! Number of Field ID's */
    size_t num_fid;

    /*! Number of Field count */
    size_t count_fld;

    /*! Field Information  */
    bcmlrd_client_field_info_t *f_info;

   /*! Configuration fields */
    bcmltd_fields_t fields[2];
} config_inmem_t;

/*!
 * \brief Handles of subcompont initialized per unit
 */

typedef void *cci_handle_t;

/*!
 * \brief Description context of CCI
 */

typedef struct cci_context_s {
    /*! Logical device id  */
    int unit;

    /*! Handle for collection */
    cci_handle_t hcol;

    /*! Handle for collection */
    cci_handle_t hcache;

    /*! In memory configuration */
    config_inmem_t config;

    /*! Event state */
    config_inmem_t state;

    /*! Event state control */
    config_inmem_t state_ctrl;

    /*! Sync state */
    cci_sync_event_state_t sync_state[CCI_CTR_TYPE_NUM + 1];

    /*! Sub state */
    int sub_phase;

    /*! Warm boot flag */
    bool warm;

    /*! In memory configuration initialized */
    bool init_config;
} cci_context_t;

/*!
 * Get  cci context.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
cci_handle_t
bcmptm_cci_context_get(int unit);

/*!
 * \brief Get CCI Configuration information
 *
 * \param [in] unit Logical device id
 * \param [out] config, pointer to structure cci_config_t
 *
 * \retval SHR_E_NONE Success
 */
int
bcmptm_cci_config_get(int unit,
                      cci_config_t *config);


/*!
 * \brief Get CCI cache Handle
 *
 * \param [in] unit Logical device id
 * \param [in]con CCI context
 * \param [out] handle of CCI Cache Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_handle_get(int unit,
                            const cci_context_t *con,
                            cci_handle_t *handle);

/*!
 * \brief Enable eviction counter.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] TRUE (enable) , FALSE(Disable).
 * \param [in] TRUE for clt_on_read to be enable.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_ctr_evict_enable(int unit, bcmdrd_sid_t sid,
                            bool enable, bool clr_on_read);

/*!
 * \brief Eviction counter is enabled
 *
 * \param [in] unit Unit number
 * \param [in] symbol ID.
 *
 * \retval TRUE (enabled)
 */

extern bool
bcmptm_cci_ctr_evict_is_enable(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Clr on Read is enabled
 *
 * \param [in] unit Unit number
 * \param [in] symbol ID.
 *
 * \retval TRUE (enabled)
 */

extern bool
bcmptm_cci_ctr_evict_is_clr_on_read(int unit, bcmdrd_sid_t sid);

/*!
 * Configuration resource cleanup function
 *
 * \param [in] unit Unit number
 * \param [in] cci context.
 *
 * \retval None
 */
extern void
bcmptm_cci_imm_config_cleanup(int unit,
                              cci_context_t   *con);

/*!
 * Configuration resource initialization function
 *
 * \param [in] unit Unit number
 * \param [in] cci context.
 * \param [in] imm configuration.
 * \param [in] stage call back function pointer.
 * \param [in] validate call back function pointer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_cci_imm_config_init(int unit,
                           cci_context_t   *con,
                           const char *table_name,
                           config_inmem_t *inmem,
                           bcmimm_lt_stage_cb *stage_cb,
                           bcmimm_lt_validate_cb *validate_cb);

/*
 * Initialize imm
 *
 * \param [in] Unit number.
 * \param [in] cci context.
 *
 * \retval SHR_E_NONE No errors.
 */

extern int
bcmptm_cci_imm_init(int uint,
                    cci_context_t   *con);

/*
 * Insert entires in imm
 *
 * \param [in] Unit number.
 * \param [in] cci context.
 *
 * \retval SHR_E_NONE No errors.
 */

extern int
bcmptm_cci_imm_insert(int uint,
                      cci_context_t   *con);

#endif /* CCI_INTERNAL_H */
