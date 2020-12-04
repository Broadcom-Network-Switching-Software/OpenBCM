/*! \file bcmbd_mcs.h
 *
 * MCS data structures and APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_MCS_H
#define BCMBD_MCS_H

#include <sal/sal_types.h>
#include <sal/sal_time.h>

#include <bcmbd/mcs_shared/mcs_msg_common.h>

/*! Debug status string size. */
#define MCS_INFO_STATUS_SIZE 12

/*!
 * \brief Info displayed if MCS is in exception mode.
 */
typedef struct bcmbd_ukernel_info_s {
    /*! uC status : OK/Reset/Fault/Invalid. */
    char status[MCS_INFO_STATUS_SIZE + 1];
    /*! uC FW version. */
    char fw_version[100];
    /*! ARM CPSR register value. */
    unsigned int cpsr;
    /*! Exception type. */
    unsigned int type;
    /*! Data Fault Status Register */
    unsigned int dfsr;
    /*! Data Fault Address Register */
    unsigned int dfar;
    /*! Instruction Fault Status Register */
    unsigned int ifsr;
    /*! Instruction Fault Address Register */
    unsigned int ifar;
    /*! ARM registers at the time of Exception */
    unsigned int armreg[16];
} bcmbd_ukernel_info_t;

/*!
 * \brief uKernel Configuration values.
 *
 * These values are passed to the uKernel by the firmware
 * loader function.
 */
typedef struct bcmbd_ukernel_config_s {
    /*! mHost core to configure */
    int uc;
    /*! HostRAM buffer address */
    void *hostram_addr;
    /*! HostRAM physical address */
    uint64_t hostram_paddr;
    /*! HostRAM Size in bytes */
    uint32_t hostram_size;
    /*! SRAM base address */
    uint32_t sram_base;
    /*! SRAM size in bytes */
    uint32_t sram_size;
    /*! Uncached SRAM size in bytes */
    uint32_t sram_unc_size;
    /*! SBUSDMA channels */
    uint32_t sbusdma_bmp;
    /*! PKTDMA channels */
    uint32_t pktdma_bmp;
    /*! SCHANFIFO channels */
    uint32_t schanfifo_bmp;
} bcmbd_ukernel_config_t;

/*!
 * \brief Resources requested.
 *
 * These values are passed to the MCS Resource manager
 * to check and reserve resources for eApps.
 */
typedef struct bcmbd_mcs_res_req_s {
    /*! App type */
    mcs_app_type_t app;
    /*! HostRAM Size in bytes */
    uint32_t hostram_size;
    /*! SRAM size in bytes */
    uint32_t sram_size;
    /*! Uncached SRAM size in bytes */
    uint32_t sram_unc_size;
    /*! No. of SBUSDMA channels */
    uint32_t num_sbusdma;
    /*! No. of PKTDMA channels */
    uint32_t num_pktdma;
    /*! No. of SCHANFIFO channels */
    uint32_t num_schanfifo;
} bcmbd_mcs_res_req_t;

/*! MCS Notification events */
typedef enum bcmbd_mcs_event_e {
    BCMBD_MCS_EVENT_STARTING = 1, /*!< uC about to shut down. */
    BCMBD_MCS_EVENT_HALTED,       /*!< uC core is halted. */
    BCMBD_MCS_EVENT_NOHALT,       /*!< uC Apps should run without host. */
    BCMBD_MCS_EVENT_EXCEPTION     /*!< uC FW is in exception mode. */
} bcmbd_mcs_event_t;


/*!
 * \brief Console output function for embedded uC CLI.
 *
 * \param [in] ch Character to output.
 */
typedef void (*bcmbd_mcs_putc_f)(int ch);

/*!
 * \brief Callback to application code to notify a uC/messaging event.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] event Notification event.
 * \param [in] user_data User Data for Callback.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
typedef int (bcmbd_mcs_appl_cb_t)(int unit,
                                  int uc,
                                  bcmbd_mcs_event_t event,
                                  void *user_data);

/*!
 * \brief Send uKernel CLI command to the MCS.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] cmd Command string to send to the MCS.
 * \param [in] waitms Time in mS to wait for the MCS to consume
 *             the command before returning.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_con_cmd_send(int unit, int uc, char *cmd, int waitms);

/*!
 * \brief Set the MCS console snooping status.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] enable MCS console snooping status.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_con_mode_set(int unit, int uc, bool enable);

/*!
 * \brief Get the MCS console snooping status.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [out] enabled MCS console snooping status.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_con_mode_get(int unit, int uc, bool *enabled);

/*!
 * \brief Configure output function for MCS console.
 *
 * The MCS driver will use this callback function to display
 * characters from the embedded MCS console output.
 *
 * Note that all MCS consoles share the same function.
 *
 * \param [in] putc_func Output callback function.
 */
extern void
bcmbd_mcs_putc_func_set(bcmbd_mcs_putc_f putc_func);

/*!
 * \brief Send a message to another processor.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] msg Message to send.
 * \param [in] timeout Time to wait or zero (no wait).
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_msg_send(int unit, int uc,
                   mcs_msg_data_t *msg, sal_usecs_t timeout);

/*!
 * \brief Wait & Receive a message of a given type from a uC.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] mclass Message class to receive.
 * \param [out] msg Pointer to buffer to receive message.
 * \param [in] timeout Time to wait or zero (no wait).
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_msg_receive(int unit, int uc, uint8_t mclass,
                      mcs_msg_data_t *msg, int timeout);

/*!
 * \brief Send a message & Wait & Receive a reply.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] send Message to send.
 * \param [out] reply Pointer to buffer to receive reply.
 * \param [in] timeout Time to wait or zero (no wait).
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_msg_send_receive(int unit, int uc, mcs_msg_data_t *send,
                           mcs_msg_data_t *reply, sal_usecs_t timeout);

/*!
 * \brief Cancel wait for a message of a given type from a uC.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] mclass Message class.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
int
bcmbd_mcs_msg_receive_cancel(int unit, int uc, uint8_t mclass);

/*!
 * \brief Convert address from MCS address space to PCI address space.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS processor instance.
 * \param [in] mcs_addr Address in MCS address space.
 *
 * \return Address in PCI address space.
 */
extern uint32_t
bcmbd_mcs_to_pci_addr(int unit, int uc, uint32_t mcs_addr);

/*!
 * \brief Reset MCS Microcontroller.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_reset(int unit, int uc);

/*!
 * \brief Set MCS Microcontroller Configurable parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] cfg Configurable parameters for the uKernel.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_cfg_set(int unit, int uc, bcmbd_ukernel_config_t *cfg);

/*!
 * \brief Get MCS Microcontroller Configurable parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [out] cfg Configurable parameters from the uKernel.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_cfg_get(int unit, int uc, bcmbd_ukernel_config_t *cfg);

/*!
 * \brief Get MCS Microcontroller Debug information.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [out] dbg Debug information from the uKernel.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_dbg_get(int unit, int uc, mcs_debug_extn_t *dbg);

/*!
 * \brief Start MCS Microcontroller.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_start(int unit, int uc);

/*!
 * \brief Check reset state of MCS Microcontroller.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [out] isrst Non-zero if in reset state, othwerwise zero.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_is_reset(int unit, int uc, int *isrst);

/*!
 * \brief Start uC messaging common part.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_msg_start(int unit);

/*!
 * \brief Start uC messaging core-specific.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_msg_uc_start(int unit, int uc);

/*!
 * \brief Stop uC messaging core-specific.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_msg_uc_stop(int unit, int uc);

/*!
 * \brief Get MCS microcontroller status.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS processor instance.
 * \param [out] info Microcontroller FW Information.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_uc_status(int unit, int uc, bcmbd_ukernel_info_t *info);

/*!
 * \brief Get number of MCS processor instances.
 *
 * \param [in] unit Unit number.
 *
 * \return Number of MCS processor instances.
 */
extern int
bcmbd_mcs_num_uc(int unit);

/*!
 * \brief Register a callback function for certain uC events.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS processor instance.
 * \param [in] msg_class Message class in case multiple apps runs on one uC.
 * \param [in] cb_fn Callback function.
 * \param [in] cb_data Parameter data for the Callback function.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_appl_cb_register(int unit, int uc, int msg_class,
                           bcmbd_mcs_appl_cb_t *cb_fn, void *cb_data);

/*!
 * \brief reserve resources in the resource manager
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] req Requested resources.
 * \param [out] cfg Reserved resource values.
 * \param [in] flags Flags for reserve resource operation
 *             (\ref BCMBD_MCS_RM_FLAG_xxx).
 *
 * \return BCMBD_E_xxx error code.
 */
extern int
bcmbd_mcs_rm_reserve(int unit, int uc, bcmbd_mcs_res_req_t *req,
                     bcmbd_ukernel_config_t *cfg, uint32_t flags);

/*!
 * \brief release resources reserved by a uC in the resource manager
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 *
 * \return standard SHR_E_xxx error code.
 */
extern int
bcmbd_mcs_rm_release(int unit, int uc);

/*!
 * \brief get resources reserved in resource manager for a given uC
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS Processor instance.
 * \param [in] type Type of requested resource.
 *             (\ref BCMBD_MCS_RM_RESOURCES_xxx).
 * \param [out] cfg Used/Free/All resource values.
 * \param [in] flags Flags for getting resource info.
 *             Only BCMBD_MCS_RM_FLAG_INCLUDE_SELF is used.
 *
 * \return standard SHR_E_xxx error code.
 */
extern int
bcmbd_mcs_rm_get_resources(int unit, int uc, int type,
                           bcmbd_ukernel_config_t *cfg, uint32_t flags);

/*!
 * \name MCS resource type.
 * \anchor BCMBD_MCS_RM_RESOURCES_xxx
 */
/*! \{ */
/*! Resources used by specific uC. */
#define BCMBD_MCS_RM_RESOURCES_USED         1
/*! Free resources available for specific uC. */
#define BCMBD_MCS_RM_RESOURCES_FREE         2
/*! All resources available for specific uC. */
#define BCMBD_MCS_RM_RESOURCES_ALL          3
/*! \} */

/*!
 * \name MCS resource manager flags.
 * \anchor BCMBD_MCS_RM_FLAG_xxx
 */
/*! \{ */
/*! Include resources used by current uC as "Free". */
#define BCMBD_MCS_RM_FLAG_INCLUDE_SELF      0x00000001
/*! Automatically get configuration from app type. */
#define BCMBD_MCS_RM_FLAG_AUTO_CONFIG       0x00000004
/*! Automatically use next available core. */
#define BCMBD_MCS_RM_FLAG_AUTO_CORE         0x00000008
/*! \} */

#endif /* BCMBD_MCS_H */
