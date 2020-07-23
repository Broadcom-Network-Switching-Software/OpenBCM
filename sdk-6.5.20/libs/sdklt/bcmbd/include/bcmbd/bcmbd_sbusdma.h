/*! \file bcmbd_sbusdma.h
 *
 * SBUSDMA data structures and APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_SBUSDMA_H
#define BCMBD_SBUSDMA_H

#include <sal/sal_types.h>
#include <bcmdrd_config.h>

/*! Maximum number of SBUSDMA devices supported. */
#define SBUSDMA_DEV_NUM_MAX     BCMDRD_CONFIG_MAX_UNITS

/*! Maximum number of SBUSDMA work queue for asyn access. */
#define SBUSDMA_WORK_QUE_NUM    3

/*!
 * \name Data attributes.
 * \anchor SBUSDMA_DA_xxx
 */

/*! \{ */
/*! Append data buffer to the previous work item's, only used for batch work. */
#define SBUSDMA_DA_APND_BUF     (1 << 0)

/*! Jump to another work item indicated by "jump_item", only used for batch work. */
#define SBUSDMA_DA_JUMP_TO      (1 << 1)

/*! Skip over the current work item, only used for batch work. */
#define SBUSDMA_DA_SKIP_OVER    (1 << 2)
/*! \} */

/*!
 * \name Work flags.
 * \anchor SBUSDMA_WF_xxx
 */

/*! \{ */
/*! Read operation. */
#define SBUSDMA_WF_READ_CMD     (0 << 0)

/*! Write operation. */
#define SBUSDMA_WF_WRITE_CMD    (1 << 0)

/*! Use interrupt mode. */
#define SBUSDMA_WF_INT_MODE     (1 << 1)

/*!
 * Inverse operation, access from the highest entry to the lowest
 * entry.
 */
#define SBUSDMA_WF_DEC_ADDR     (1 << 2)

/*!
 * Single data operation, access all the entries only using the same
 * data of a single entry.
 */
#define SBUSDMA_WF_SGL_DATA     (1 << 3)

/*!
 * Single address operation, only access a single entry using the
 * data of all entries one by one.
 */
#define SBUSDMA_WF_SGL_ADDR     (1 << 4)

/*! Data read will not be copied to Host memory. */
#define SBUSDMA_WF_DUMB_READ    (1 << 5)

/*! Swap words in 64 bit data. */
#define SBUSDMA_WF_64BIT_SWAP   (1 << 6)
/*! \} */

/*!
 * \brief SBUSDMA work data.
 */
typedef struct bcmbd_sbusdma_data_s {
    /*! Start address of entries. */
    uint32_t start_addr;

    /*! Entry width in words. */
    uint32_t data_width;

    /*! Address bits of gap between entries. */
    uint32_t addr_gap;

    /*! Operation code. */
    uint32_t op_code;

    /*! Number of operations. */
    uint32_t op_count;

    /*! DMA buffer physical address. */
    uint64_t buf_paddr;

    /*! Jump to item. */
    int jump_item;

    /*! Data attributes (\ref SBUSDMA_DA_xxx). */
    uint32_t attrs;

    /*! Core clock delay between multiple pending SBUS commands (MOR). */
    uint32_t pend_clocks;

    /*! Max number of commands bursted (MOR), 0 indicates 256, other as it. */
    uint32_t burst_cmds;
} bcmbd_sbusdma_data_t;

/*! Function type for SBUSDMA operation callbacks. */
typedef void (*sbusdma_cb_f)(int unit, void *data);

/*!
 * \brief SBUSDMA work state.
 */
typedef enum bcmbd_sbusdma_state_e {
    /*! Idle work. */
    SBUSDMA_WORK_NULL = 0,

    /*! Initialized light work. */
    SBUSDMA_WORK_INITED = 1,

    /*! Created batch work. */
    SBUSDMA_WORK_CREATED = 2,

    /*! Destroyed batch work. */
    SBUSDMA_WORK_DESTROYED = 3,

    /*! Queued light or batch work. */
    SBUSDMA_WORK_QUEUED = 4,

    /*! Scheduled light or batch work. */
    SBUSDMA_WORK_SCHED = 5,

    /*! Started light or batch work. */
    SBUSDMA_WORK_STARTED = 6,

    /*! Completed work. */
    SBUSDMA_WORK_DONE = 7,

    /*! Timeout work. */
    SBUSDMA_WORK_TIMEOUT = 8,

    /*! Failed work. */
    SBUSDMA_WORK_ERROR = 9,
} bcmbd_sbusdma_state_t;

/*!
 * \brief SBUSDMA work queue number.
 */
typedef enum bcmbd_sbusdma_queue_e {
    /*! Work queue 0. */
    SBUSDMA_WORK_QUEUE_0 = 0,

    /*! Work queue 1. */
    SBUSDMA_WORK_QUEUE_1 = 1,

    /*! Work queue 2. */
    SBUSDMA_WORK_QUEUE_2 = 2,

    /*! Number of work queue. */
    SBUSDMA_WORK_QUEUE_NUMBER = SBUSDMA_WORK_QUE_NUM,
} bcmbd_sbusdma_queue_t;

/*!
 * \brief SBUSDMA work.
 */
typedef struct bcmbd_sbusdma_work_s {
    /*! Work data. */
    bcmbd_sbusdma_data_t *data;

    /*! Work items. */
    int items;

    /*! Pre-config callback. */
    sbusdma_cb_f pc;

    /*! Pre-config callback parameter. */
    void *pc_data;

    /*! Post callback. */
    sbusdma_cb_f cb;

    /*! Post callback parameter. */
    void *cb_data;

    /*! Work queue number for async access. */
    bcmbd_sbusdma_queue_t queue;

    /*! Work link used internally. */
    void *link;

    /*! Work descriptor chain used internally. */
    void *desc;

    /*! Work flags (\ref SBUSDMA_WF_xxx). */
    uint32_t flags;

    /*! Work state. */
    bcmbd_sbusdma_state_t state;
} bcmbd_sbusdma_work_t;

/*!
 * \brief Attach SBUSDMA driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_attach(int unit);

/*!
 * \brief Detach SBUSDMA driver.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_detach(int unit);

/*!
 * \brief Initialize light work.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_light_work_init(int unit, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Create batch work.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_batch_work_create(int unit, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Delete batch work.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_batch_work_delete(int unit, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Execute light or batch work.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_work_execute(int unit, bcmbd_sbusdma_work_t *work);

/*!
 * \brief Add light or batch work to work queue.
 *
 * \param [in] unit Unit number.
 * \param [in] work Work structure point.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_sbusdma_work_add(int unit, bcmbd_sbusdma_work_t *work);

#endif /* BCMBD_SBUSDMA_H */
