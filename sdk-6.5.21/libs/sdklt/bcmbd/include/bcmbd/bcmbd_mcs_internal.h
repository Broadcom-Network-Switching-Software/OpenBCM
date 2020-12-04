/*! \file bcmbd_mcs_internal.h
 *
 * MCS Driver and Messaging data structures and APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_MCS_INTERNAL_H
#define BCMBD_MCS_INTERNAL_H

#include <sal/sal_types.h>

/*!
 * \brief MCS driver structure.
 */
typedef struct mcs_drv_s {
    /*! Get the PCIe address of MCS memory. */
    uint32_t (*mcs_to_pci_addr)(int unit, int uc, uint32_t mcs_addr);
    /*! Reset MCS device. */
    int (*mcs_uc_reset)(int unit, int uc);
    /*! Set configuration to MCS Device. */
    int (*mcs_uc_cfg_set)(int unit, int uc, bcmbd_ukernel_config_t *cfg);
    /*! Get configuration from MCS Device. */
    int (*mcs_uc_cfg_get)(int unit, int uc, bcmbd_ukernel_config_t *cfg);
    /*! Get debug info from MCS Device. */
    int (*mcs_uc_dbg_get)(int unit, int uc, mcs_debug_extn_t *dbg);
    /*! Start MCS Device. */
    int (*mcs_uc_start)(int unit, int uc);
    /*! Check if MCS device is in reset. */
    int (*mcs_uc_is_reset)(int unit, int uc, int *isrst);
    /*! Enable messaging interrupt */
    int (*mcs_intr_enable)(int unit, int uc);
    /*! Disable messaging interrupt */
    int (*mcs_intr_disable)(int unit, int uc);
    /*! Set messaging interrupt */
    int (*mcs_intr_set)(int unit, int uc);
    /*! Reserve resources. */
    int (*mcs_rm_reserve)(int unit, int uc, bcmbd_mcs_res_req_t *req,
                         bcmbd_ukernel_config_t *cfg, uint32_t flags);
    /*! Release resources. */
    int (*mcs_rm_release)(int unit, int uc);
    /*! Get resources. */
    int (*mcs_rm_get_resources)(int unit, int uc, int type,
                                bcmbd_ukernel_config_t *cfg, uint32_t flags);
} mcs_drv_t;

/*!
 * \brief Initialize device-specific MCS driver.
 *
 * Note that the driver object will not be copied, so it must be store
 * as static data by the caller.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Pointer to MCS driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
extern int
bcmbd_mcs_drv_init(int unit, const mcs_drv_t *drv);

/*!
 * \brief MCS messaging interrupt handler.
 *
 * Must be called by the primary software interrupt handler once the
 * hardware interrupt has been dealt with.
 *
 * This function will typically trigger the MCS message processing
 * thread and return.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS number that raised the Software interrupt.
 */
extern void
bcmbd_mcs_isr(int unit, uint32_t uc);

/*!
 * \brief Enable software interrupts from remote processor.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS number to enable Software interrupt from.
 *
 * \retval 0 No errors.
 */
extern int
bcmbd_mcs_intr_enable(int unit, int uc);

/*!
 * \brief Disable software interrupts from remote processor.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS number to disable Software interrupt from.
 *
 * \retval 0 No errors.
 */
extern int
bcmbd_mcs_intr_disable(int unit, int uc);

/*!
 * \brief Raise messaging interrupt on remote processor.
 *
 * \param [in] unit Unit number.
 * \param [in] uc MCS number to raise the Software interrupt to.
 *
 * \retval 0 No errors.
 */
extern int
bcmbd_mcs_intr_set(int unit, int uc);

/*!
 * \brief Initialize MCS instance.
 *
 * \param [in] unit Unit number.
 * \param [in] num_uc Number of microcontrollers for this device.
 * \param [in] msg_areas Array of messaging area for all uCs.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_init(int unit, int num_uc, uint32_t *msg_areas);

/*!
 * \brief Clean up MCS instance.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_mcs_cleanup(int unit);

/*!
 * \brief Console output function for embedded uC CLI.
 *
 * \param [in] ch Character to output.
 */
extern void
bcmbd_mcs_putc(int ch);

/*!
 * \brief MCS Information.
 *
 * uKernel Firmware saves specific values in case of an exception
 * into specific locations from where the SDK gets and displays
 * to assist in debugging
 */
/*! ARM Core's R0-R15 at the time of exception. */
#define MCS_INF_R0_ADDR 0x20
/*! ARM Core's CPSR. This will be non-zero only in case of an exception. */
#define MCS_INF_CPSR_ADDR 0x60
/*! Exception Type. Data_Abort/Prefetch_Abort/Undefined. */
#define MCS_INF_EX_TYPE_ADDR 0x64
/*! ARM Core's Data Fault Status Register. */
#define MCS_INF_DFSR_ADDR 0x68
/*! ARM Core's Data Fault Address Register. */
#define MCS_INF_DFAR_ADDR 0x6c
/*! ARM Core's Instruction Fault Status Register. */
#define MCS_INF_IFSR_ADDR 0x70
/*! ARM Core's Instruction Fault Address Register. */
#define MCS_INF_IFAR_ADDR 0x74

/*!
 * \brief MCS Debug Data.
 *
 * uKernel Firmware maintains some debug data to help in debugging.
 * Though this location can change, a pointer is maintained at a
 * specific location.
 */
/*! Pointer to Firmware's Debug Extension. */
#define MCS_DBG_PTR_ADDR 0x78

/*!
 * \brief MCS Configurability Feature.
 *
 * uKernel Firmware has specific locations into which certain
 * configuration info can be written before starting the uC
 */
/*! Check for firmware "configurable" feature at this address. */
#define MCS_CFG_FEAT_ADDR 0x80
/*! A value of 0xbadc0de1 means that firmware has "configurable" feature. */
#define MCS_CONFIG_SUPPORTED 0xbadc0de1
/*! Configuration version indicates which features are supported. */
#define MCS_CFG_VER_ADDR 0x84
/*! Configured uC number. */
#define MCS_CFG_APP_UC_NUM_ADDR 0x88
/*! Configured HostRAM Base Address MSW. */
#define MCS_CFG_HOSTRAM_BASE_MSW_ADDR 0x8c
/*! Configured HostRAM Base Address LSW. */
#define MCS_CFG_HOSTRAM_BASE_LSW_ADDR 0x90
/*! Configured HostRAM Size. */
#define MCS_CFG_HOSTRAM_SIZE_ADDR 0x94
/*! Configured SRAM Base Address. */
#define MCS_CFG_SRAM_BASE_ADDR 0x98
/*! Configured SRAM Size. */
#define MCS_CFG_SRAM_SIZE_ADDR 0x9c
/*! Configured UNCACHED SRAM Size. */
/* This must not exceed SRAM Size */
#define MCS_CFG_UNCACHED_SRAM_SIZE_ADDR 0xa0
/*! Configured sbusdma channels */
#define MCS_CFG_SBUSDMA_BMP_ADDR 0xa4
/*! Configured pktdma channels */
#define MCS_CFG_PKTDMA_BMP_ADDR 0xa8
/*! Configured schanfifo channels */
#define MCS_CFG_SCHANFIFO_BMP_ADDR 0xac

#endif /* BCMBD_MCS_INTERNAL_H */
