/*! \file bcmbd_m0ssq_internal.h
 *
 * M0SSQ driver internal definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_M0SSQ_INTERNAL_H
#define BCMBD_M0SSQ_INTERNAL_H

#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <shr/shr_thread.h>
#include <bcmbd/bcmbd_m0ssq.h>

/*!
 *\brief Maximun number of supported uC.
 */
#define M0SSQ_MAX_NUM_OF_UC             (4)

/*!
 *\brief M0SSQ memory object.
 */
typedef struct m0ssq_mem_s bcmbd_m0ssq_mem_t;

/*******************************************************************************
 * M0SSQ driver function prototypes
 */

/*!
 * \brief Device-specific M0SSQ driver initialization.
 *
 * This function is used to initialize the chip-specific M0SSQ
 * configuration, which may include both software and hardware
 * operations.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL M0SSQ driver initialization failed.
 */
typedef int
(*m0ssq_init_f)(int unit);

/*!
 * \brief Device-specific M0SSQ driver cleanup.
 *
 * This function is used to cleanup the chip-specific configuration.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to driver cleanup.
 */
typedef int
(*m0ssq_cleanup_f)(int unit);

/*!
 *\brief Start/Stop a uC.
 *
 * This function is to start/stop a uC.
 *
 * \param [in]  unit Unit number.
 * \param [in]  uc uC number.
 * \param [in]  start !0: start a uC.
 *                    0: stop a uC.
 *
 * \retval SHR_E_NONE Start/stop of uC successed.
 * \retval SHR_E_FAIL Start/stop of uC failed .
 */
typedef int
(*m0ssq_uc_start_set_f)(int unit, uint32_t uc, bool start);


/*!
 *\brief Check uC is started or not.
 *
 * This function is to check if a uC is started or not.
 *
 * \param [in]   unit Unit number.
 * \param [in]   uc uC number in M0SSQ.
 * \param [out]  start Indicate uC is started or not.
 *
 * \return SHR_E_NONE if successful.
 */
typedef int
(*m0ssq_uc_start_get_f)(int unit,
                        uint32_t uc,
                        bool *start);

/*******************************************************************************
 * M0SSQ and M0SSQ-MBOX shared structure.
 */
/*!
 * \brief Write data into M0SSQ memory block.
 *
 * \param [in] unit Unit number.
 * \param [in] mem Memory block object, possible memory objects are listed in
 *                 \ref bcmbd_m0ssq_dev_t.
 * \param [in] offset Starting position of writei operation.
 * \param [in] data Data buffer should be word aligned.
 * \param [in] len  Byte length.
 *
 * \retval SHR_E_NONE Write memory successfully.
 * \retval SHR_E_FAIL Write operation failed.
 */
typedef int
(*m0ssq_mem_write32_f) (bcmbd_m0ssq_mem_t *mem,
                        uint32_t offset,
                        const uint32_t *data,
                        uint32_t len);



/*!
 * \brief Read data from M0SSQ memory block.
 *
 * \param [in] unit Unit number.
 * \param [in] mem Memory block object, possible memory objects are listed in
 *                 \ref bcmbd_m0ssq_dev_t.
 * \param [in] offset Starting position of read operation.
 * \param [in] data Data buffer should be word aligned.
 * \param [in] len  Byte length,a mutiple of 4.
 *
 * \retval SHR_E_NONE Read memory successfully.
 * \retval SHR_E_FAIL Read operation failed.
 */
typedef int
(*m0ssq_mem_read32_f) (bcmbd_m0ssq_mem_t *mem,
                       uint32_t offset,
                       uint32_t *data,
                       uint32_t len);


/*!
 * \brief Structure of M0SSQ memory object.
 */
struct m0ssq_mem_s {

    /*! Base address of memory block. */
    int unit;

    /*! Base address of memory block. */
    uint32_t base;

    /*! Size of memory block. */
    uint32_t size;

    /*! Read function of memory block. */
    m0ssq_mem_read32_f read32;

    /*! Write function of memory block. */
    m0ssq_mem_write32_f write32;

};

/*!
 * \brief M0SSQ driver object.
 */
typedef struct bcmbd_m0ssq_drv_s {

    /*! Initialize M0SSQ driver. */
    m0ssq_init_f init;

    /*! Cleanup M0SSQ driver. */
    m0ssq_cleanup_f cleanup;

    /*! Start/Stop uC. */
    m0ssq_uc_start_set_f uc_start_set;

    /*! Get uC is started or not. */
    m0ssq_uc_start_get_f uc_start_get;

} bcmbd_m0ssq_drv_t;


/*!
 * \brief Firmware initial configuration.
 */
typedef struct bcmbd_fwconfig_s {

    /*! chip device id. */
    uint32_t devid;

    /*! chip revision id. */
    uint32_t revid;

    /*! uC number. */
    uint32_t uc;

    /*! Size of bcmbd_fwconfig_t */
    uint32_t len;

    /*! Checksum */
    uint32_t chksum;

    /*! Magic number. */
    uint32_t magic;

} bcmbd_fwconfig_t;

/*!
 *\brief Magic number of bcmbd_fwconfig_t.
 */
#define M0SSQ_FWCFG_MAGIC          (0x4D304657)

/*!
 * \brief M0SSQ internal data structure.
 */
typedef struct bcmbd_m0ssq_dev_s {

    /*! Base address of M0SSQ. */
    uint32_t base_addr;

    /*! Number of uC in M0SSQ. */
    uint32_t num_of_uc;

    /*! Offset of uC swintr number. */
    uint32_t swintr_offset;

    /*! uC swintr handler on host processor side. */
    bcmbd_intr_f uc_swintr_handler[M0SSQ_MAX_NUM_OF_UC];

    /*! Parameter of uC's swintr handler. */
    uint32_t uc_swintr_param[M0SSQ_MAX_NUM_OF_UC];

    /*! uC's software interrupt enable. */
    bool uc_swintr_enable[M0SSQ_MAX_NUM_OF_UC];

    /*! Flag to indicate if there is any interrupt is pending
     *  and awaiting for being handled.
     */
    bool uc_swintr_pending[M0SSQ_MAX_NUM_OF_UC];

    /*! Flag indacates a uC's software interrupt handling is
     *  in process.
     */
    bool uc_swintr_handling[M0SSQ_MAX_NUM_OF_UC];

    /*! uC's software interrupt thread. */
    shr_thread_t uc_swintr_thread_ctrl[M0SSQ_MAX_NUM_OF_UC];

    /*! Memory object for shared SRAM in M0SSQ. */
    bcmbd_m0ssq_mem_t sram;

    /*! Memory object for LED accumulation RAM in M0SSQ. */
    bcmbd_m0ssq_mem_t led_accu_ram;

    /*! Memory object for LED send RAM in M0SSQ. */
    bcmbd_m0ssq_mem_t led_send_ram;

    /*! Memory object for uC's TCM in M0SSQ. */
    bcmbd_m0ssq_mem_t m0_tcm[M0SSQ_MAX_NUM_OF_UC];

    /*! Memory object for firmware initial configuration. */
    bcmbd_m0ssq_mem_t fwconfig_mem[M0SSQ_MAX_NUM_OF_UC];

    /*! Firmware initial configuration. */
    bcmbd_fwconfig_t fwconfig[M0SSQ_MAX_NUM_OF_UC];

} bcmbd_m0ssq_dev_t;

/*******************************************************************************
 * BCMBD internal APIs.
 */

/*!
 * \brief Install device-specific M0SSQ driver.
 *
 * Install device-specific M0SSQ driver into top-level M0SSQ API.
 *
 * Use \c m0ssq_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] m0ssq_drv M0SSQ driver object.
 *
 * \retval 0 No errors.
 */
extern int
bcmbd_m0ssq_drv_init(int unit, const bcmbd_m0ssq_drv_t *m0ssq_drv);

/*!
 * \brief Device-specific initialization.
 *
 * This function is used to initialize the chip-specific
 * configuration, which may include both software and hardware
 * operations.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL M0SSQ driver initialization failed.
 */
extern int
bcmbd_m0ssq_init(int unit);

/*!
 * \brief Device-specific driver cleanup.
 *
 * This function is used to cleanup the chip-specific configuration.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to driver cleanup.
 */
extern int
bcmbd_m0ssq_cleanup(int unit);

/*!
 * \brief Get device-specific data.
 *
 * \param [in] unit Unit number.
 * \param [out] dev Device data of M0SSQ driver.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_m0ssq_dev_get(int unit, bcmbd_m0ssq_dev_t **dev);

/*!
 * \brief M0SSQ software interrupt primary handler.
 *
 * This function is shared by all M0s.
 *
 * \param [in] unit Unit number.
 * \param [in] uc uC number.
 */
extern void
bcmbd_m0ssq_uc_swintr_isr(int unit, uint32_t uc);

/*!
 * \brief Read function for iProc memory block.
 *
 * \param [in] mem Memory block object.
 * \param [in] offset Starting offset of data read within a memory block.
 * \param [out] data Buffer for read data.
 * \param [in] len Byte length of data read.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_m0ssq_iproc_mem_read32(bcmbd_m0ssq_mem_t *mem,
                             uint32_t offset,
                             uint32_t *data, uint32_t len);

/*!
 * \brief Write function for iProc memory block.
 *
 * \param [in] mem Memory block object.
 * \param [in] offset Starting offset of data write within a memory block.
 * \param [in] data Buffer for write data.
 * \param [in] len Byte length of write data.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_m0ssq_iproc_mem_write32(bcmbd_m0ssq_mem_t *mem,
                              uint32_t offset,
                              const uint32_t *data, uint32_t len);


/*!
 * \brief Clear memory block by zeros.
 *
 * \param [in] mem Memory block object.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_m0ssq_mem_clear(bcmbd_m0ssq_mem_t *mem);

/*!
 * \brief Create M0SSQ memory objects for CMICx and CMICx2 device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_m0ssq_mem_create(int unit);

/*!
 * \brief Get CMICx-M0SSQ base driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval Pointer to the base driver.
 */
extern const bcmbd_m0ssq_drv_t *
bcmbd_cmicx_m0ssq_base_drv_get(int unit);

/*!
 * \brief Get CMICx2-M0SSQ base driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval Pointer to the base driver.
 */
extern const bcmbd_m0ssq_drv_t *
bcmbd_cmicx2_m0ssq_base_drv_get(int unit);

/*!
 * \brief Calculate checksum of M0 firmware config.
 *
 * \param [in] fwcfg M0 firmware config.
 * \param [out] pchksum Checksum of M0 firmware config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Wrong parameter.
 */
extern int
bcmbd_m0ssq_fwconfig_chksum(bcmbd_fwconfig_t fwcfg, uint32_t *pchksum);

#endif /* BCMBD_M0SSQ_INTERNAL_H */
