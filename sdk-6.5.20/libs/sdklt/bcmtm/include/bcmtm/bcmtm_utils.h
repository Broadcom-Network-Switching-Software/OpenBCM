/*! \file bcmtm_utils.h
 *
 * BCMTM utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_UTILS_H
#define BCMTM_UTILS_H

#include <bcmtm/bcmtm_types.h>

/*!
 * \brief Get number of multicast queue for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] num_mc_q Number of multicast queues for logical port.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_num_mcq_get(int unit,
                       bcmtm_lport_t lport,
                       int *num_mcq);

/*!
 * \brief Get number of unicast queue for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] num_ucq Number of multicast queues for logical port.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_num_ucq_get(int unit,
                       bcmtm_lport_t lport,
                       int *num_ucq);

/*!
 * \brief Verify logical port for cpu port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is CPU port.
 * \retval FALSE logical port is not CPU port.
 */
extern int
bcmtm_lport_is_cpu(int unit,
        bcmtm_lport_t lport);

/*!
 * \brief Verify logical port for loopback port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is loopback port.
 * \retval FALSE logical port is not loopback port.
 */
extern int
bcmtm_lport_is_lb(int unit, bcmtm_lport_t lport);

/*!
 * \brief Verify logical port for front panel port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is front panel port.
 * \retval FALSE logical port is not front panel port.
 */
extern int
bcmtm_lport_is_fp(int unit, bcmtm_lport_t lport);

/*!
 * \brief Verify logical port for management port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is management port.
 * \retval FALSE logical port is not management port.
 */
extern int
bcmtm_lport_is_mgmt(int unit, bcmtm_lport_t lport);

/*!
 * \brief Verify logical port for rdb port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE logical port is RDB port.
 * \retval FALSE logical port is not RDB port.
 */
extern int
bcmtm_lport_is_rdb(int unit, bcmtm_lport_t lport);

/*!
 * \brief Get global mmu port number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] mport MMU global port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_mport_get(int unit,
        bcmtm_lport_t lport,
        int *mport);

/*!
 * \brief Get physical port number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] pport Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_pport_get(int unit,
        bcmtm_lport_t lport,
        int *pport);

/*!
 * \brief Get mmu chip port number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] mmu_chip_port MMU chip port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_mchip_port_get(int unit,
        bcmtm_lport_t lport,
        int *mmu_chip_port);

/*!
 * \brief Get maximum speed for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] max_port_speed Maximum port speed for the logical port.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_max_speed_get(int unit,
                          bcmtm_lport_t lport,
                          int *max_port_speed);

/*!
 * \brief Get the pipe number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] pipe Pipe number for the logical port.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_pipe_get(int unit,
        bcmtm_lport_t lport,
        int *pipe);

/*!
 * \brief Get mmu local port number for the logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] mmu_local MMU local port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_mmu_local_port_get(int unit,
        bcmtm_lport_t lport,
        int *mmu_local);


/*!
 * \brief Get multicast queue base number for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] mc_q_base Multicast base queue number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_mcq_base_get(int unit,
        bcmtm_lport_t lport,
        int *mc_q_base);

/*!
 * \brief Get number of unicast queue base number for a logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] uc_q_base Unicast base queue number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_lport_ucq_base_get(int unit,
        bcmtm_lport_t lport,
        int *uc_q_base);

/*!
 * \brief Get the logical port for a given mmu port.
 *
 * \param [in] unit Unit number.
 * \param [in] mport MMU port number.
 * \param [out] lport Logical port ID.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_mport_lport_get(int unit,
                      int mport,
                      int *lport);

/*!
 *
 * \brief Derives logical port number for the given physical port number.
 *
 * \param [in] unit Logical unit number.
 * \param [in] pport Physical port number.
 * \param [out] lport Logical port number.
 *
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_pport_lport_get(int unit,
                      bcmtm_pport_t pport,
                      int *lport);
/*!
 *
 * \brief Derives logical port number for the given TM chip port number.
 *
 * \param [in] unit Logical unit number.
 * \param [in] mchip_port TM chip port number.
 * \param [out] lport Logical port number.
 *
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 * \retval SHR_E_NOT_FOUND Port not configured.
 */
extern int
bcmtm_mchip_port_lport_get(int unit,
                           int mchip_port,
                           int *lport);

/*!
 * \brief  Gets the chip q number given the lport and mmu queue.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] mmu_q TM queue number.
 * \param [out] chip_q Chip queue number.
 */
int
bcmtm_chipq_from_mmuq_get(int unit,
                          uint32_t lport,
                          uint32_t mmu_q,
                          uint32_t *chip_q);

/*!
 * \brief  Gets the non-CPU chip q number given the lport and mmu queue.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [in] mmu_q MMU queue number.
 * \param [out] global_q Global MMU queue number.
 */
int
bcmtm_noncpu_chipq_from_mmuq_get(int unit,
                          uint32_t lport,
                          uint32_t mmu_q,
                          uint32_t *global_q);

/*!
 * \brief  Checks if ITM is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] itm  ITM Number.
 *
 * \retval SHR_E_NONE ITM is valid.
 * \retval SHR_E_FAIL ITM is not valid.
 */

int
bcmtm_itm_valid_get(int unit, int itm);

/*!
 * \brief Port validation for obm function.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Device information not found.
 */
extern int
bcmtm_obm_port_validation(int unit, bcmtm_lport_t lport);


/*!
 * \brief Get the number of unicast queue for non-cpu ports.
 *
 * \param [in] unit Unit number.
 * \param [out] num_ucq Number of unicast queue.
 */
extern int
bcmtm_num_ucq_get(int unit, uint8_t *num_ucq);

/*!
 * \brief Get the number of multicast queue for non-cpu ports.
 *
 * \param [in] unit Unit number.
 * \param [out] num_mcq Number of multicast queue.
 */
extern int
bcmtm_num_mcq_get(int unit, uint8_t *num_mcq);

/*!
 * \brief Check credit loop between EP and MMU.
 *
 * \param [in] unit unit number.
 * \param [in] lport Logical Port Number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PORT Invalid logical port.
 * \retval SHR_E_FAIL Request credit mismatch observed.
 */
extern int
bcmtm_check_ep_mmu_credit(int unit, int lport);

/*!
 * \brief Check credit loop between Port and EP.
 *
 * \param [in] unit unit number.
 * \param [in] lport Logical Port Number.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PORT Invalid logical port.
 * \retval SHR_E_FAIL Request credit mismatch observed of function called for
 *                    internal port.
 */
extern int
bcmtm_check_port_ep_credit(int unit, int lport);

/*!
 * \brief Get OBM threshold value.
 *
 * \param [in] unit unit number.
 * \param [in] lport Logical Port Number.
 * \param [in] thd Threshold type enum.
 * \param [out] val Threshold value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_UNAVAIL Driver is not available.
 * \retval SHR_E_PARAM Invalid threshold type enum.
 */
extern int
bcmtm_obm_thd_get(int unit, uint32_t lport, int thd, uint32_t *val);

/*!
 * \brief Set OBM threshold value.
 *
 * \param [in] unit unit number.
 * \param [in] lport Logical Port Number.
 * \param [in] thd Threshold type enum.
 * \param [in] val Threshold value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_UNAVAIL Driver is not available.
 * \retval SHR_E_PARAM Invalid threshold type enum.
 */
extern int
bcmtm_obm_thd_set(int unit, uint32_t lport, int thd, uint32_t val);

#endif /* BCMTM_UTILS_H */
