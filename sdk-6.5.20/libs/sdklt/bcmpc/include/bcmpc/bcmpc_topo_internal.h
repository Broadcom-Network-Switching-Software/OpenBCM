/*! \file bcmpc_topo_internal.h
 *
 * BCMPC TOPO Internal Functions.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the Port Control (BCMPC) module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_TOPO_INTERNAL_H
#define BCMPC_TOPO_INTERNAL_H

#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_pm_drv_internal.h>
#include <bcmpc/bcmpc_topo.h>

/*!
 * \brief Port macro instance structure
 *
 * This data structure defines the information for a port macro instance.
 *
 * The base port means the first physical switch port number of the PM.
 * For example, a PM is associated with physical port 2, 3, 4 and 5, the
 * \c base_port should be set to 2.
 *
 * The invalid physical PM port bit map, \c invalid_pbmp, indicates which
 * physical PM ports are not associated with a physical device port.
 * By default all physical PM ports are valid i.e. invalid_pbmp is 0.
 */
typedef struct bcmpc_topo_s {

    /*! PortMacro type ID. */
    int tid;

    /*! Base physical port number. */
    bcmpc_pport_t base_pport;

    /*! PortMacro specific information. */
    pm_info_t*   pm_info;

    /*! Port number of lanes configured on this PortMacro. */
    int port_num_lanes[BCMPC_NUM_PPORTS_PER_PM_MAX];

    /*! Invalid physical PM port bit map. */
    SHR_BITDCLNAME(invalid_pbmp, BCMPC_NUM_PPORTS_PER_PM_MAX);

    /*! Indicates if the PM is enabled in the SKU variant. */
    bool is_active;

} bcmpc_topo_t;

/*!
 * \brief Port Macro Feature.
 *
 * This feature list defined some special handling
 */
typedef enum bcmpc_ft_e {

    /*! Toggle phy signal when enable MAC loopback. */
    BCMPC_FT_TOGGLE_PHY_ON_MAC_LPBK = 0,

    /*! Do full sequences for core initialization. */
    BCMPC_FT_CORE_INIT_FULL,

    /*! Skip the firmware loading during core initialization. */
    BCMPC_FT_CORE_INIT_SKIP_FW_LOAD,

    /*! Only update the lane swap setting during core initialization. */
    BCMPC_FT_CORE_INIT_LANE_MAP_ONLY,

    /*! 1588 Egress timestamp supported. */
    BCMPC_FT_1588_EGRESS_TIMESTAMP,

    /*! RLM supported. */
    BCMPC_FT_RLM,

    /*! Support software state update in BCMPC thread. */
    BCMPC_FT_SW_STATE_UPDATE,

    /*! FDR supported. */
    BCMPC_FT_FDR_SUPPORTED,

    /*! Management ports present in the PM. */
    BCMPC_FT_MGMT_PORT_MACRO,

    /*! Must be the last. */
    BCMPC_FT_COUNT

} bcmpc_ft_t;

/*!
 * \brief Download the PHY firmware
 *
 * PMGR helps download the PHY firmware after reading user confuguration
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id Port macro index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
typedef int
(*bcmpc_topo_firmware_load_f)(int unit, int pm_id);


/*!
 * \brief Port macro type structure
 *
 * This data structure defines the information for a port macro type.
 *
 * The number of lanes (\c num_lanes) and ports (\c num_ports) are in physical
 * view. For example,
 * TSCE has 4 physical lanes and occupies 4 physical ports.
 * QTC/QSGMII has 4 lanes and occupies 16 physical ports.
 *
 * For an aggregated PM, the number of aggregated entities (\c num_aggr) is
 * required.
 * An aggregated PM means the PM has more than one sub-cores, and each core
 * could work independently as several individual units or together as one unit.
 * For example, PM12x10 could either work as 3 independent units (i.e. PM4x10)
 * or aggregating all resources to become one control unit. For this case, we
 * would set \c num_aggr = 3.
 *
 * We suppose that each sub-unit is identical, i.e. the lanes and ports of
 * an aggregated PM will be equally distributed to each sub-unit.
 */
typedef struct bcmpc_topo_type_s {

    /*! PortMacro type name. */
    const char *name;

    /*! The number of physical lanes. */
    int num_lanes;

    /*! The number of physical ports. */
    int num_ports;

    /*! The number of aggregated entities. */
    int num_aggr;

    /*! The number of PLLs. */
    int num_plls;

    /*! The pll index used by TVCO. */
    int tvco_pll_index;

    /*! PM driver. */
    bcmpc_pm_drv_t *pm_drv;

    /*!
     * \brief Chip-specific firmware load function
     *   If pm_firmware_load is NULL, the firmware download would happen at
     *   a early stage before configuration is loaded.
     *   Otherwise, the PMGR driver will load firmware when doing core init.
     */
    bcmpc_topo_firmware_load_f pm_firmware_load;

    /*! Feature list */
    SHR_BITDCLNAME(pm_feature, BCMPC_FT_COUNT);

    /*! Reference clock used by the port macro. */
    phymod_ref_clk_t ref_clk;

} bcmpc_topo_type_t;


/*!
 * \brief Initalize PM type data structure.
 *
 * \param [in] tinfo Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_topo_type_t_init(bcmpc_topo_type_t *tinfo);


/*!
 * \brief Create a port macro type.
 *
 * \param [in] unit Unit number.
 * \param [in] tid PM type identity to create. The value should be unique.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_type_create(int unit, int tid);


/*!
 * \brief Destroy a port macro type.
 *
 * \param [in] unit Unit number.
 * \param [in] tid PM type identity to destroy.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_type_destroy(int unit, int tid);


/*!
 * \brief Set port macro type information.
 *
 * \param [in] unit Unit number.
 * \param [in] tid PM type identity to set.
 * \param [in] tinfo PM type info.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_type_set(int unit, int tid, bcmpc_topo_type_t *tinfo);


/*!
 * \brief Get port macro type information.
 *
 * \param [in] unit Unit number.
 * \param [in] tid type identity to get
 * \param [out] tinfo type info
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_type_get(int unit, int tid, bcmpc_topo_type_t *tinfo);


/*!
 * \brief Callback function for type traversing.
 *
 * The traversal function to invoke on each created type.
 *
 * \param [in] unit Unit number.
 * \param [in] tid PM type identity.
 * \param [in] tinfo PM type info.
 * \param [in] cookie User data for this traverse function.
 */
typedef int (*bcmpc_topo_type_traverse_cb_f)(int unit, int tid,
                                             bcmpc_topo_type_t *tinfo,
                                             void *cookie);


/*!
 * \brief Traverse created types.
 *
 * Use the provided traversal function and cookie on each type.
 *
 * \param [in] unit Unit number.
 * \param [in] cb_func Callback function to invoke.
 * \param [in] cookie User data for this traverse function.
 */
extern int
bcmpc_topo_type_traverse(int unit, bcmpc_topo_type_traverse_cb_f cb_func,
                         void *cookie);


/*!
 * \brief Initalize PM instance data structure.
 *
 * \param [in] pm_info Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_topo_t_init(bcmpc_topo_t *pm_info);


/*!
 * \brief Create a port macro instance.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Cold/Warm boot.
 * \param [in] pm_id PM identity to create. The value should be unique.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_create(int unit, bool warm, int pm_id);


/*!
 * \brief Destroy a port macro instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity to destroy.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_destroy(int unit, int pm_id);


/*!
 * \brief Set port macro instance information.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity to set.
 * \param [in] pm_info PM info.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_set(int unit, int pm_id, bcmpc_topo_t *pm_info);


/*!
 * \brief Get port macro instance information.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity to get.
 * \param [out] pm_info PM info.
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_get(int unit, int pm_id, bcmpc_topo_t *pm_info);

/*!
 * \brief Update/Add port macro instance information.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity which needs to be updated.
 * \param [in] topo_info PM informtion
 *
 * \return shr_error_t
 */
extern int
bcmpc_topo_update(int unit, int pm_id, bcmpc_topo_t *topo_info);


/*!
 * \brief Callback function for instance traversal.
 *
 * The traversal function to invoke on each created instance.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM identity.
 * \param [in] pm_info PM info.
 * \param [in] cookie User data for this traverse function.
 */
typedef int (*bcmpc_topo_traverse_cb_f)(int unit, int pm_id,
                                        bcmpc_topo_t *pm_info,
                                        void *cookie);


/*!
 * \brief Traverse created instances.
 *
 * Use the provided traversal function and cookie on each instance.
 *
 * \param [in] unit Unit number.
 * \param [in] cb_func Callback function to invoke.
 * \param [in] cookie User data for this traverse function.
 */
extern int
bcmpc_topo_traverse(int unit, bcmpc_topo_traverse_cb_f cb_func,
                    void *cookie);

/*!
 * \brief Create the topology instances for the given block type.
 *
 * This function will create the topology instances for the given block type
 * based on the DRD infroamtion.
 *
 * \param [in] unit Unit number.
 * \param [in] blktype Block type.
 * \param [in] type_id The topology type to specify.
 * \param [in] warm True for warm boot, otherwise cold boot.
 * \param [in] pm_id The starting PM ID.
 * \param [out] inst_cnt The number of created instances.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to create the instances for \c blktype.
 */
extern int
bcmpc_topo_instances_create(int unit, int blktype, int type_id, bool warm,
                            int pm_id, int *inst_cnt);

#endif /* BCMPC_TOPO_INTERNAL_H */
