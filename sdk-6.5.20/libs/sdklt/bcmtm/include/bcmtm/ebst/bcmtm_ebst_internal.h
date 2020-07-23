/*! \file bcmtm_ebst_internal.h
 *
 * APIs, definitions for EBST.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_EBST_DEFS_H
#define BCMTM_EBST_DEFS_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <sal/sal_mutex.h>
#include <sal/sal_msgq.h>
#include <sal/sal_sem.h>
#include <shr/shr_bitop.h>
#include <shr/shr_thread.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmbd/bcmbd_fifodma.h>

/*! Enumerations for EBST scan mode. */
typedef enum
bcmtm_ebst_scan_mode_e {

    /*! EBST type Queue. It supports both unicast and multicast queues. */
    EBST_SCAN_MODE_QUEUE = 0,

    /*! EBST type unicast port service pool. */
    EBST_SCAN_MODE_PORTSP_UC = 1,

    /*! EBST type multicast port service pool. */
    EBST_SCAN_MODE_PORTSP_MC = 2,

    /*! EBST type service pool. */
    EBST_SCAN_MODE_SP = 3,
} bcmtm_ebst_scan_mode_t;

/*! EBST state machine states. */
typedef enum
bcmtm_ebst_enable_mode_e {
    /*! EBST OFF. */
    EBST_ENABLE_MODE_OFF = 0,

    /*! EBST on. */
    EBST_ENABLE_MODE_START = 1,

    /*! EBST stopped. */
    EBST_ENABLE_MODE_STOP = 2,

    /*! EBST FIFO full interrupt raised. */
    EBST_ENABLE_MODE_FULL = 3,
} bcmtm_ebst_enable_mode_t;

/*! EBST port operational states. */
typedef enum
bcmtm_ebst_port_oper_state_e {
    /*! Port valid. */
    EBST_OPER_VALID                 = 0,

    /*! Logical to physical port mapping unavailable. */
    EBST_OPER_PORT_INFO_UNAVAILABLE = 1,

    /*! Queue ID is invalid for the port. */
    EBST_OPER_Q_ID_INVALID          = 2
} bcmtm_ebst_port_oper_state_t;

/*!
 * EBST control user configuration.
 */
typedef struct {

    /*! Scan round configuration in clocks. */
    uint32_t scan_round;

    /*! Scan threshold entries. */
    uint32_t scan_thd;

    /*! Scan mode. */
    bcmtm_ebst_scan_mode_t scan_mode;

    /*! Enable mode. */
    bcmtm_ebst_enable_mode_t enable_mode;
} bcmtm_ebst_control_cfg_t;


/*!
 * \brief timestamp register configuration structure.
 */
typedef struct bcmtm_ebst_ts_cfg_s {
    /*! Timestamp register sid. */
    bcmdrd_sid_t sid;

    /*! Timestamp register fid. */
    bcmdrd_fid_t fid;

    /*! Timestamp register index. */
    int idx;

    /*! Timestamp register value. */
    uint32_t val;
} bcmtm_ebst_ts_cfg_t;

/*!
 * EBST FIFO data.
 */
typedef struct ebst_fifodma_work_s {
    /*! EBST work item. */
    bcmbd_fifodma_work_t ebst_work;

    /*! EBST data. */
    bcmbd_fifodma_data_t ebst_data;

    /*! EBST buffer. */
    uint32_t *buffer;
} bcmtm_ebst_fifodata_t;

/*!
 * EBST data DB
 */
typedef struct {
    /*! EBST timestamp */
    uint64_t timestamp;

    /*! EBST count */
    uint32_t count;

    /*! red drop state */
    uint32_t red_drop_state;

    /*! yellow drop state */
    uint32_t yellow_drop_state;

    /*! green drop state */
    uint32_t green_drop_state;

    /*! Data type. */
    bcmtm_ebst_scan_mode_t type;
} bcmtm_ebst_data_t;

/*!
 * EBST data pointer.
 */
typedef struct {
    /*! Base into the data buffer. */
    int base_id;

    /*! Maximum number of entry. */
    int num_entry;

    /*! Maximum ID into the data_buffer. */
    int max_id;

    /*! Next entry to write. */
    int write_ptr[MAX_BUFFER_POOL];
} bcmtm_ebst_entity_map_t;


/*!
 * EBST port configuration.
 */
typedef struct {
    /*! Enable EBST for the port. */
    bool enable;

    /*! Operational status for the configuration. */
    uint8_t opcode;

    /*! Action performed on the port. */
    uint8_t action;

}bcmtm_ebst_port_cfg_t;

/*! EBST queue configuration. */
typedef struct {
    /*! Unicast or non-unicast queue type.*/
    bcmtm_q_type_t q_type;

    /*! Enable monitoring. */
    bool enable;

    /*! Queue entity map for EBST. */
    bcmtm_ebst_entity_map_t q_map;

    /*! Operational status. */
    uint8_t opcode;

    /*! Action (insert/delete/update) to be performed. */
    uint8_t action;
} bcmtm_ebst_q_cfg_t;


/*! EBST Service pool configuration. */
typedef struct {
    /*! Enable monitoring. */
    bool enable;

    /*! Service pool entity map for EBST. */
    bcmtm_ebst_entity_map_t sp_map;

    /*! Operational status. */
    uint8_t opcode;

} bcmtm_ebst_sp_cfg_t;

/*!
 * EBST data info structure for port service_pool.
 */
typedef struct {
    /*! Unicast port service pool details */
    bcmtm_ebst_entity_map_t uc_sp;

    /*! Multicast port service pool details. */
    bcmtm_ebst_entity_map_t mc_sp;
} bcmtm_ebst_portsp_map_t;


/*!
 * EBST port service pool logical table data configuration.
 */
typedef struct {
    /*! EBST port service pool data mapping configuration. */
    bcmtm_ebst_portsp_map_t portsp_map;
} bcmtm_ebst_portsp_cfg_t;


/*!
 * EBST data info structure
 */
typedef struct {
    /*!
     * EBST data buffer and map lock.
     */
    sal_mutex_t lock;

    /*!
     * Per chip queue control structure.
     * Valid only when EBST_SCAN_MODE is QUEUE.
     */
    bcmtm_ebst_entity_map_t *q_map;

    /*!
     * Per {chip port, sp} control structure.
     * Valid only when EBST_SCAN_MODE is not QUEUE.
     */
    bcmtm_ebst_portsp_map_t *portsp_map;

    /*!
     * Per sp control structure.
     * Valid when any queue or portsp ebst is enabled.
     */
    bcmtm_ebst_entity_map_t *sp_map;

} bcmtm_ebst_map_info_t;


/*! Enumerations for EBST Message. */
typedef enum {
    /*! EBST thread exit. */
    EBST_MSG_EXIT_THREAD,

    /*! EBST fifo dma messages. */
    EBST_MSG_FIFODMA,

    /*! EBST FIFO full. */
    EBST_FIFO_FULL
} bcmtm_ebst_msg_t;


/*! EBST notification messages. */
typedef struct bcmtm_ebst_msgq_notif_s {
    /*! Message type. */
    bcmtm_ebst_msg_t msg;

    /*! Buffer pool ID. */
    int buffer_pool;
} bcmtm_ebst_msgq_notif_t;

/*!
 * EBST device info structure
 */
typedef struct {
    /*! Number of buffer pools. */
    uint8_t max_buffer_pool;

    /*! Active scan mode. */
    bcmtm_ebst_scan_mode_t scan_mode;

    /*! Active enable mode. */
    bcmtm_ebst_enable_mode_t enable_mode;

    /*! EBST FIFO data */
    bcmtm_ebst_fifodata_t fifodata[MAX_BUFFER_POOL];

    /*! EBST FIFO sid. */
    bcmdrd_sid_t fifo_sid;

    /*! Channel number for the device. */
    uint16_t chan[MAX_BUFFER_POOL];

    /*! Message queue for callback and thread communication.*/
    sal_msgq_t msgq_intr;

    /*! Thread PID */
    shr_thread_t pid;

    /*! EBST enabled on buffer pools. */
    bool start[MAX_BUFFER_POOL];

    /*! EBST mapping informations. */
    bcmtm_ebst_map_info_t map_info;

    /*! Number of entires in the FIFO DMA. */
    uint32_t num_entries;

    /*! Number of threshold entries in the FIFO DMA. */
    uint32_t thd_entries;

    /*! Monitor enable for Service pool. */
    uint32_t monitor;
} bcmtm_ebst_dev_info_t;


/*!
 * \brief BCMTM EBST chip specific port configuration.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] port_cfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in parameters.
 * \retval SHR_E_NOT_FOUND Port not found.
 */
typedef int
(*bcmtm_ebst_port_config_set_t)(int unit,
                                bcmltd_sid_t ltid,
                                bcmtm_lport_t lport,
                                bcmtm_ebst_port_cfg_t *port_cfg);

/*!
 * \brief EBST device specific scan cofigurations.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] ebst_cfg EBST configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Error in parameters.
 */
typedef int
(*bcmtm_ebst_scan_config_set_t)(int unit,
                                bcmltd_sid_t ltid,
                                bcmtm_ebst_control_cfg_t *ebst_cfg);


/*!
 * \brief Parse the EBST data based on the scan mode.
 *
 * \param [in] unit Logical unit number.
 * \param [in] entry Data entry to be parsed.
 * \param [in] scan_mode Scan mode for EBST scan.
 * \param [out] entity EBST data for the entity based on scan mode.
 * \param [out] data EBST data.
 *
 * \retval SHR_E_NONE No error.
 */
typedef int
(*bcmtm_ebst_data_parse_t)(int unit,
                           uint32_t *entry,
                           bcmtm_ebst_scan_mode_t scan_mode,
                           uint32_t *entity,
                           bcmtm_ebst_data_t *data);

/*!
 * \brief EBST fifo full interrupt enable.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] enable Interrupt state.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*bcmtm_ebst_enable_t) (int unit,
                        bcmltd_sid_t ltid,
                        bool enable);

/*!
 * \brief EBST data map allocation.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_MEMORY No memory.
 */
typedef int
(*bcmtm_ebst_map_alloc_t) (int unit);

/*!
 * \brief Update EBST queue configuration.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] q_id Queue ID.
 * \param [in] q_cfg Queue configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_NOT_FOUND Logical port not found.
 */
typedef int
(*bcmtm_ebst_q_config_set_t) (int unit,
                              bcmltd_sid_t ltid,
                              bcmtm_lport_t lport,
                              uint16_t q_id,
                              bcmtm_ebst_q_cfg_t *q_cfg);

/*!
 * \brief Get the index for mapping table for port service pools.
 * This returns the index for port service pool used for populating data table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port ID.
 * \param [in] sp_id Service pool ID.
 * \param [out] map_idx Mapping index for port service pool map.
 *
 * \retval SHR_E_NOT_FOUND Logical port not valid.
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*bcmtm_ebst_portsp_index_get_t) (int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t spid,
                                  uint16_t *map_idx);

/*!
 * \brief Get the index for mapping table for queues.
 *
 * This returns the index for queue types used for populating data table.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port ID.
 * \param [in] q_id Queue ID. (unicast queue ID or multicast queue ID).
 * \param [in] q_type Queue type.
 * \param [out] map_idx Mapping index for queue map.
 */
typedef int
(*bcmtm_ebst_q_map_index_get_t) (int unit,
                                 bcmtm_lport_t lport,
                                 uint16_t q_id,
                                 bcmtm_q_type_t q_type,
                                 uint16_t *map_idx);


/*! BCMTM EBST driver */
typedef struct bcmtm_ebst_drv_s {
    /*! EBST port configuration. */
    bcmtm_ebst_port_config_set_t ebst_port_config_set;

    /*! EBST scan configurations. */
    bcmtm_ebst_scan_config_set_t ebst_scan_config_set;

    /*! EBST port service pool mapping index get. */
    bcmtm_ebst_portsp_index_get_t ebst_portsp_index_get;

    /*! EBST data parsing. */
    bcmtm_ebst_data_parse_t ebst_data_parse;

    /*! EBST enable. */
    bcmtm_ebst_enable_t ebst_enable;

    /*! EBST map allocation. */
    bcmtm_ebst_map_alloc_t ebst_map_alloc;

    /*! EBST queue configurations. */
    bcmtm_ebst_q_config_set_t ebst_q_config_set;

    /*!  EBST queue mapping index get. */
    bcmtm_ebst_q_map_index_get_t ebst_q_map_index_get;
} bcmtm_ebst_drv_t;


/*!
 * \brief EBST port internal updates.
 *
 * Auto updates the operational state for the configured instructions when there
 * is any port flex operations.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 * \param [in] action Port add/delete action.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_ebst_port_internal_update(int unit,
                                bcmtm_lport_t lport,
                                uint8_t action);

/*!
 * \brief EBST port queue internal updates.
 *
 * Auto updates the operational state for the configured instructions when there
 * is any port flex operations.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 * \param [in] action Port add/delete action.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_ebst_port_q_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t action);

/*!
 * \brief EBST interrupt notifications.
 *
 * \param [in] unit Logical unit number.
 * \param [in] intr_param Interrupt parameters.
 */
extern void
bcmtm_ebst_intr_notify(int unit, uint32_t intr_param);

/*! \brief IMM initialization function for EBST
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_ebst_port_init(int unit, int warm);


/*!
 * \brief BCMTM EBST device information.
 *
 * \param [in] unit Logical unit number.
 * \param [out] ebst_dev_info EBST dev information.
 */
extern void
bcmtm_ebst_dev_info_get(int unit, bcmtm_ebst_dev_info_t **ebst_dev_info);

/*!
 * \brief Enabled EBST.
 *
 * Creates the FIFO DMA mapping and EBST thread for processing.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcmtm_ebst_start(int unit);

/*!
 * \brief EBST fifo dma stop.
 *
 * \param [in] unit Logical unit number.
 * \param [in] free Free the allocated resources.
 */
extern void
bcmtm_ebst_fifodma_stop(int unit, bool free);

/*!
 * \brief BCMTM TM_EBST_STATUS logical table updates.
 *
 * \param [in] unit Logical unit number.
 * \param [in] mode EBST working mode.
 * \param [in] ebst_full_state EBST full state. The calling function should pass
 *  the structure with number of buffer pools available in the system.
 */
extern int
bcmtm_ebst_status_update(int unit,
                         bcmtm_ebst_enable_mode_t mode,
                         int *ebst_full_state);

/*! \brief IMM cleanup function for EBST
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
extern int
bcmtm_ebst_cleanup(int unit);

/*!
 * \brief BCMTM EBST initialization.
 *
 * \param [in] unit Logical unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_INIT Error in initialization.
 */
extern int
bcmtm_ebst_init(int unit, bool warm);

/*!
 * \brief TM EBST control configuration imm entry lookup.
 *
 * \param [in] unit Unit number.
 * \param [out] ebst_cfg EBST control configuration.
 */
extern int
bcmtm_ebst_control_imm_lkup(int unit, bcmtm_ebst_control_cfg_t *ebst_cfg);

/*!
 * \brief EBST queue map populate.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Error in parameters.
 * \retval SHR_E_MEMORY No memory.
 */
extern int
bcmtm_ebst_q_map_populate(int unit, bcmltd_sid_t ltid);

/*!
 * \brief EBST port sp map populate.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Error in parameters.
 * \retval SHR_E_MEMORY No memory.
 */
extern int
bcmtm_ebst_portsp_map_populate(int unit, bcmltd_sid_t ltid);

/*!
 * \brief EBST sp map populate.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Error in parameters.
 * \retval SHR_E_MEMORY No memory.
 */
extern int
bcmtm_ebst_sp_map_populate(int unit, bcmltd_sid_t ltid);

/*!
 * \brief Allocates memory for device specific TM EBST driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmtm_ebst_dev_info_alloc(int unit);
#endif /* BCMTM_EBST_DEFS_H */
