/*! \file bcma_testutil_flex.h
 *
 * Flex port operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_FLEX_H
#define BCMA_TESTUTIL_FLEX_H

#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>


/*!
 * \brief Add port into PC_PORT logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] speed Port speed.
 * \param [in] num_lanes Number of lanes.
 * \param [in] max_frame_size Max frame size.
 * \param [in] fec_mode Ptr to fec_mode string.
 * \param [in] speed_vco Ptr to speed_vco string.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_flex_pc_port_add(int unit, uint32_t lport, uint32_t speed,
                                   uint8_t num_lanes, uint32_t max_frame_size,
                                   char *fec_mode, char *speed_vco);

/*!
 * \brief Delete port from PC_PORT logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_flex_pc_port_delete(int unit, uint32_t lport);

/*!
 * \brief Add port into PC_PORT_PHYS_MAP logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] pport Physical port.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_flex_pc_port_phys_map_add(int unit, uint32_t lport,
                                            uint32_t pport);

/*!
 * \brief Delete port from PC_PORT_PHYS_MAP logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_flex_pc_port_phys_map_delete(int unit, uint32_t lport);

/*!
 * \brief Get logical to physical port map from config file.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 * \param [in] array_size Size of l2p_map array.
 * \param [out] l2p_map Ptr to logical to physical port map.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_flex_l2p_map_get(int unit, bcmdrd_pbmp_t pbmp,
                                   uint32_t array_size, uint32_t *l2p_map);

/*!
 * \brief Flex port down/up for all port in port bitmap.
 *
 * \param [in] unit Unit number.
 * \param [in] lgc_pbmp Logical port bitmap for testing.
 * \param [in] phy_pbmp_down Physical port bitmap for down ports (optional).
 * \param [in] phy_pbmp_up   Physical port bitmap for up ports   (optional).
 * \param [in] num_pms Number 1st dimention size of pm_mode_str.
 * \param [in] str_size 2nd dimension size of pm_mode_str.
 * \param [in] pm_mode_str Ptr to PM mode to be flexed.
 * \param [out] lgc_pbmp_down Ptr to logical port bitmap for down ports.
 * \param [out] lgc_pbmp_up Ptr to logical port bitmap for up ports.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_flex_do_flex(int unit, bcmdrd_pbmp_t lgc_pbmp,
                               bcmdrd_pbmp_t phy_pbmp_down,
                               bcmdrd_pbmp_t phy_pbmp_up,
                               uint32_t num_pms, uint32_t str_size,
                               char *pm_mode_str,
                               bcmdrd_pbmp_t *lgc_pbmp_down,
                               bcmdrd_pbmp_t *lgc_pbmp_up);

/*!
 * \brief Add port into PC_PORT logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] speed Port speed.
 * \param [in] num_lanes Number of lanes.
 * \param [in] max_frame_size Max frame size.
 * \param [in] fec_mode Ptr to fec_mode string.
 * \param [in] speed_vco Ptr to speed_vco string.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_flex_pc_port_add_f)(
                int unit, uint32_t lport, uint32_t speed, uint8_t num_lanes,
                uint32_t max_frame_size, char *fec_mode, char *speed_vco);

/*!
 * \brief Delete port from PC_PORT logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_flex_pc_port_delete_f)(
                int unit, uint32_t lport);

/*!
 * \brief Add port into PC_PORT_PHYS_MAP logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 * \param [in] pport Physical port.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_flex_pc_port_phys_map_add_f)(
                int unit, uint32_t lport, uint32_t pport);

/*!
 * \brief Delete port from PC_PORT_PHYS_MAP logical table.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_flex_pc_port_phys_map_delete_f)(
                int unit, uint32_t lport);

/*!
 * \brief Get logical to physical port map from config file.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 * \param [in] array_size Size of l2p_map array.
 * \param [out] l2p_map Ptr to logical to physical port map.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_flex_l2p_map_get_f)(
                int unit, bcmdrd_pbmp_t pbmp, uint32_t array_size,
                uint32_t *l2p_map);

/*!
 * \brief Flex port down/up for all port in port bitmap.
 *
 * \param [in] unit Unit number.
 * \param [in] lgc_pbmp Logical port bitmap for testing.
 * \param [in] phy_pbmp_down Physical port bitmap for down ports (optional).
 * \param [in] phy_pbmp_up   Physical port bitmap for up ports   (optional).
 * \param [in] num_pms Number 1st dimention size of pm_mode_str.
 * \param [in] str_size 2nd dimension size of pm_mode_str.
 * \param [in] pm_mode_str Ptr to PM mode to be flexed.
 * \param [out] lgc_pbmp_down Ptr to logical port bitmap for down ports.
 * \param [out] lgc_pbmp_up Ptr to logical port bitmap for up ports.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_flex_do_flex_f)(
                int unit, bcmdrd_pbmp_t lgc_pbmp,
                bcmdrd_pbmp_t phy_pbmp_down, bcmdrd_pbmp_t phy_pbmp_up,
                uint32_t num_pms, uint32_t str_size, char *pm_mode_str,
                bcmdrd_pbmp_t *lgc_pbmp_down, bcmdrd_pbmp_t *lgc_pbmp_up);

/*! Flex port logical table operation functions. */
typedef struct bcma_testutil_flex_lt_op_s {

    /*! Add port into lt PC_PORT. */
    bcma_testutil_drv_flex_pc_port_add_f flex_pc_port_add;

    /*! Delete port from lt PC_PORT. */
    bcma_testutil_drv_flex_pc_port_delete_f flex_pc_port_delete;

    /*! Add port into lt PC_PORT_PHYS_MAP. */
    bcma_testutil_drv_flex_pc_port_phys_map_add_f flex_pc_port_phys_map_add;

    /*! Delete port from lt PC_PORT_PHYS_MAP. */
    bcma_testutil_drv_flex_pc_port_phys_map_delete_f
        flex_pc_port_phys_map_delete;

    /*! Get logical to physical port map from config file. */
    bcma_testutil_drv_flex_l2p_map_get_f flex_l2p_map_get;

} bcma_testutil_flex_lt_op_t;

/*! Flex port operation functions for traffic test. */
typedef struct bcma_testutil_flex_traffic_op_s {

    /*! Flex port wrap. */
    bcma_testutil_drv_flex_do_flex_f flex_do_flex;

} bcma_testutil_flex_traffic_op_t;

/*! XFS device flex operation functions on logical table. */
extern bcma_testutil_flex_lt_op_t *bcma_testutil_xfs_flex_lt_op_get(int unit);

/*!
 * \brief Flexport test port flex (down and up) procedure.
 *
 * \param [in] unit Unit number.
 * \param [in] lgc_pbmp Logical port bitmap for testing.
 * \param [in] phy_pbmp_down Physical port bitmap for down ports (optional).
 * \param [in] phy_pbmp_up   Physical port bitmap for up ports   (optional).
 * \param [in] num_pms Number 1st dimention size of pm_mode_str.
 * \param [in] str_size 2nd dimension size of pm_mode_str.
 * \param [in] pm_mode_str Ptr to PM mode to be flexed.
 * \param [out] lgc_pbmp_down Ptr to logical port bitmap for down ports.
 * \param [out] lgc_pbmp_up Ptr to logical port bitmap for up ports.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_flex_do_flex(int unit, bcmdrd_pbmp_t lgc_pbmp,
                 bcmdrd_pbmp_t phy_pbmp_down,
                 bcmdrd_pbmp_t phy_pbmp_up,
                 uint32_t num_pms, uint32_t str_size,
                 char *pm_mode_str,
                 bcmdrd_pbmp_t *lgc_pbmp_down,
                 bcmdrd_pbmp_t *lgc_pbmp_up);

/*! XGS device flex operation functions. */
extern bcma_testutil_flex_traffic_op_t *xgs_flex_traffic_op_get(int unit);

#endif /* BCMA_TESTUTIL_FLEX_H */
