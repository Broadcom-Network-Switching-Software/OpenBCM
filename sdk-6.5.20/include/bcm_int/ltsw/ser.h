/*! \file ser.h
 *
 * ltsw ser module internal header file.
 * Include structure definitions and
 * function declarations used by other modules.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMI_LTSW_SER_H
#define BCMI_LTSW_SER_H

#include <bcm/types.h>
#include <bcm/switch.h>
#include <sal/types.h>

/*!
 * \brief Initialize HSDK SER component.
 *
 * \param [in] unit Unit number.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcmi_ltsw_ser_init(int unit);

/*!
 * \brief Detach HSDK SER component.
 *
 * \param [in] unit Unit number.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcmi_ltsw_ser_detach(int unit);

/*!
 * \brief Enable single bit error reporting of all PTs which SER check type is ECC.
 *
 * \param [in] unit Unit number.
 * \param [in] enable 1: Enable single bit error reporting of all ECC PTs.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcmi_ltsw_ser_single_bit_error_set(int unit, int enable);

/*!
 * \brief To check whether single bit error reporting of all ECC PTs is enabled or not.
 *
 * \param [in] unit Unit number.
 * \param [out] enable 1: single bit error reporting of all ECC PTs is enabled.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcmi_ltsw_ser_single_bit_error_get(int unit, int *enable);

/*!
 * \brief Inject SER error to an appointed PT.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_name Name of the appointed PT.
 * \param [in] inst Instance number of the PT.
 * \param [in] index Index of the PT.
 * \param [in] validation 1: SER error is injected into XOR bank of the PT.
 * \param [in] xor_bank 1: SER error is not be validated during this command.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcmi_ltsw_cmd_ser_inject(int unit, char *pt_name, int inst,
                         int index, int xor_bank, int validation);

/*!
 * \brief Configure SRAM or TCAM SCAN.
 *
 * \param [in] unit Unit number.
 * \param [in] tcam 1: TCAM SCAN, 0: SRAM SCAN.
 * \param [in] rate  Number of entries to scan per interval.
 * \param [in] interval Time between SCAM scans in milliseconds.
 * \param [in] enable Enable or disable SRAM or TCAM SCAN.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcmi_ltsw_cmd_ser_scan_config(int unit, bool tcam, int rate,
                              int internal, int enable);
/*!
 * \brief Get configuration of SRAM or TCAM SCAN.
 *
 * \param [in] unit Unit number.
 * \param [in] tcam 1: TCAM SCAN, 0: SRAM SCAN.
 * \param [out] rate  Number of entries to scan per interval.
 * \param [out] interval Time between SCAM scans in milliseconds.
 * \param [out] enable Enable or disable SRAM or TCAM SCAN.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcmi_ltsw_cmd_ser_scan_get(int unit, bool tcam, int *rate,
                           int *internal, int *enable);

/*!
 * \brief Ser control information.
 */
typedef struct bcmi_ltsw_ser_control_s {

    /* Squash duplicated ser event interval. */
    uint32_t squash_duplicated_interval;

} bcmi_ltsw_ser_control_t;

/*!
 * \brief Set SER control.
 *
 * \param [in] unit Unit number.
 * \param [in] ser_ctrl Ser control.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcmi_ltsw_cmd_ser_control_set(int unit, bcmi_ltsw_ser_control_t *ser_ctrl);

/*!
 * \brief Get SER control.
 *
 * \param [in] unit Unit number.
 * \param [out] ser_ctrl Ser control.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcmi_ltsw_cmd_ser_control_get(int unit, bcmi_ltsw_ser_control_t *ser_ctrl);

/*!
 * \brief Public function to get ser log information.
 *
 * \param [in] unit Unit number.
 * \param [in] id SER log ID.
 * \param [out] info SER log information.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcm_ltsw_switch_ser_log_info_get(int unit, int id,
                                 bcm_switch_ser_log_info_t *info);
/*!
 * \brief Public function to get SER error statistics for specified statistics type.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_type SER error statistics type.
 * \param [out] value Value of the appointed statistics type.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcm_ltsw_switch_ser_error_stat_get(int unit,
                                   bcm_switch_ser_error_stat_type_t stat_type,
                                   uint32 *value);
/*!
 * \brief Public function to clear SER error statistics for specified statistics type.
 *
 * \param [in] unit Unit number.
 * \param [out] stat_type SER error statistics type.
 *
 * \return BCM_E_NONE on success, error code otherwise.
 */
extern int
bcm_ltsw_switch_ser_error_stat_clear(int unit,
                                     bcm_switch_ser_error_stat_type_t stat_type);

#endif /* !BCMI_LTSW_SER_H */
