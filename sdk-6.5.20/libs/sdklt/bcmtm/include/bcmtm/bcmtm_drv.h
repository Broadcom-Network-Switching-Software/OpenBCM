/*! \file bcmtm_drv.h
 *
 * BCMTM Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BD) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_DRV_H
#define BCMTM_DRV_H

#include <sal/sal_types.h>
#include <bcmpc/bcmpc_tm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmtm/bcmtm_internal_tdm.h>
#include <bcmtm/mirror_on_drop/bcmtm_mirror_on_drop_info.h>
#include <bcmtm/pfc/bcmtm_pfc_deadlock_internal.h>
#include <bcmtm/generated/bcmtm_mc_ha.h>
#include <bcmcth/bcmcth_tm_drv.h>

/*! High GIG port type 2 */
#define BCMTM_PORT_IS_HG2      (1 << 1)
/*! Management port */
#define BCMTM_PORT_IS_MGMT     (1 << 2)
/*! Oversubscription port. */
#define BCMTM_PORT_IS_OVSB     (1 << 3)
/*! High GIG port type 3. */
#define BCMTM_PORT_IS_HG3      (1 << 4)
/*! Port type ENET */
#define BCMTM_PORT_IS_ENET     (1 << 5)
/*! Port type fabric */
#define BCMTM_PORT_IS_FABRIC   (1 << 6)


/*!
 * \name TM driver functions
 *
 * The function prototypes for \ref bcmtm_drv_t.
 */
typedef void
(*bcmtm_drv_pt_sid_rename_f)(int unit,
                             bcmdrd_sid_t in_sid,
                             int in_inst,
                             bcmdrd_sid_t *out_sid,
                             int *instance);
typedef bcmdrd_sid_t
(*bcmtm_drv_pt_sid_uniq_acc_f)(int unit,
                              bcmdrd_sid_t in_sid,
                              int xpe,
                              int pipe);


typedef int
(*bcmtm_drv_chip_func_f)(int unit, bool warm);


typedef int
(*bcmtm_drv_port_op_f)(int unit, bcmtm_pport_t port);

typedef bcmpc_tm_handler_t *
(*bcmtm_drv_pc_hdl_get_f)(int unit);

typedef int
(*bcmtm_tdm_drv_f)(int unit, bcmtm_tdm_drv_t *drv);

/*!
 * Function pointer for imm initialization.
 */
typedef int
(*bcmtm_drv_imm_init_f)(int unit);


/*! Returns the chip queue number.*/
typedef int
(*bcmtm_drv_chip_q_get_f)(int unit, uint32_t lport,
                            uint32_t mmu_q, uint32_t *chip_q);

/*! Returns the non-CPU chip queue number.*/
typedef int
(*bcmtm_drv_noncpu_chip_q_get_f)(int unit, uint32_t lport,
                            uint32_t mmu_q, uint32_t *global_q);

/*! Returns the num_uc_q value.*/
typedef int
(*bcmtm_drv_num_uc_q_cfg_get)(int unit);

/*! Returns the num_mc_q value.*/
typedef int
(*bcmtm_drv_num_mc_q_cfg_get)(int unit);

/*! Returns logical port from mmu chip port. */
typedef int
(*bcmtm_drv_mchip_port_lport_get_f) (int unit, int mchip_port, int *lport);

/*! Returns if ITM is valid. */
typedef int
(*bcmtm_drv_itm_valid_valid_get)(int unit, int itm);

/*! Returns PFC deadlock handler function pointers.*/
typedef bcmtm_pfc_ddr_drv_t *
(*bcmtm_drv_pfc_ddr_drv_get_f)(int unit);

/*! Returns PFC handler function pointers.*/
typedef bcmtm_pfc_drv_t *
(*bcmtm_drv_pfc_drv_get_f)(int unit);

/*!
 * \brief MMU IMM update function pointer type.
 *
 * This function is called after each port configuration change to allow the
 * MMU component to reconfigure the MMU resources.
 *
 * The configuration arrays are indexed by the physical port number.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of physical ports in configuration arrays.
 * \param [in] delete_update Port delete related updates.
 * \param [in] old_cfg Current port configuration.
 * \param [in] new_cfg New/requested port configuration.
 *
 * \return SHR_E_NONE when no errors, otherwise !SHR_E_NONE.
 */
typedef int
(*bcmpc_mmu_imm_update_f)(int unit, int lport, uint8_t action);

/*!
 * \brief Get Mirror-on-drop information.
 *
 * \param [in] unit Unit number.
 * \param [in] buff_pool Buffer pool index.
 * \param [out] info Mirror-on-drop information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmtm_mirror_on_drop_info_get_f)(
    int unit, int buff_pool, bcmtm_mirror_on_drop_info_t *info);

/*!
 * \brief Chip driver get for the device.
 *
 * \param [in] unit Logical unit number.
 * \param [out] wred_drv WRED driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int (*bcmtm_drv_get_f) (int unit, void *drv);

/*!
 * \brief Update drv_infor for PortMap.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ports Number of ports.
 * \param [in] pc_tm_cfg structure rray for ports.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef void (*bcmtm_drv_portmap_fill_f)(
    int unit, size_t num_ports, const bcmpc_mmu_port_cfg_t *old_cfg,
    const bcmpc_mmu_port_cfg_t *new_cfg);

/*!
 * \brief  Update unicast drop config in IP.

 * @param [in] unit Device number
 * @param [in] pport Physical port number
 * @param [in] lport Logical port number
 * @param [in] add_flag Flag indicating port add or delete sequence.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int(*bcmtm_update_unicast_drop_config_f)(
        int unit, bcmtm_pport_t pport, int lport, bool flag);

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
typedef int(*bcmtm_check_ep_mmu_credit_f)(int unit, int lport);

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
typedef int(*bcmtm_check_port_ep_credit_f)(int unit, int lport);
/*******************************************************************************
* TM driver object
 */

/*!
 * \brief TM driver object
 *
 * TM driver is used to provide the chip specific information.
 *
 * The chip specific information are included,
 * 1. The port block id and PHY address.
 * 2. The driver and access bus of PHY/PMAC.
 * 3. The programming sequence for complex port configuration e.g. port up/down.
 * 4. The linkscan mode.
 *
 * Each device should provide its own instance to BCMTM module by \ref
 * bcmtm_drv_set() from the top layer. BCMTM internally will use this interface
 * to get the corresponding information.
 */
typedef struct bcmtm_drv_s {

    /*! BCMTM chip init. */
    bcmtm_drv_chip_func_f dev_init;

    /*! BCMTM device stop. */
    bcmtm_drv_chip_func_f dev_stop;

    /*! BCMTM device clean up. */
    bcmtm_drv_chip_func_f dev_cleanup;

    /*! Configure traffic manager components. */
    bcmpc_mmu_update_f tm_configure;

    /*! BCMTM PC TM imm updates. Update the port based IMM LTs */
    bcmpc_mmu_imm_update_f imm_update;

    /*! Port down operation for Ingress. */
    bcmtm_drv_port_op_f port_ing_down;

    /*! Port up operation for Ingress. */
    bcmtm_drv_port_op_f port_ing_up;

    /*! Port down operation for MMU. */
    bcmtm_drv_port_op_f port_mmu_down;

    /*! Port up operation for MMU. */
    bcmtm_drv_port_op_f port_mmu_up;

    /*! Port down operation for egress. */
    bcmtm_drv_port_op_f port_egr_down;

    /*! Port up operation for egress. */
    bcmtm_drv_port_op_f port_egr_up;

    /*! Port Egress credit reset. */
    bcmtm_drv_port_op_f port_egr_credit_reset;

    /*! Port Egress forward enable. */
    bcmtm_drv_port_op_f port_egr_fwd_enable;

    /*! Port Egress forward disable. */
    bcmtm_drv_port_op_f port_egr_fwd_disable;

    /*! TDM driver */
    bcmtm_tdm_drv_f tdm_drv;

    /*! PT SID rename function. */
    bcmtm_drv_pt_sid_rename_f pt_sid_rename;

    /*! PT SID unique access. */
    bcmtm_drv_pt_sid_uniq_acc_f pt_sid_uniq_acc;

    /*! TM port control handlers. */
    bcmtm_drv_pc_hdl_get_f pc_hdl_get;

    /*! TM imm init function. */
    bcmtm_drv_imm_init_f imm_init;

    /*! Get chip queue number. */
    bcmtm_drv_chip_q_get_f chip_q_get;

    /*! Get logical port number */
    bcmtm_drv_mchip_port_lport_get_f mchip_port_lport_get;

    /*! Get chip queue number. */
    bcmtm_drv_noncpu_chip_q_get_f noncpu_chip_q_get;

    /*! Number of UC queues derived from MC_Q_MODE. */
    bcmtm_drv_num_uc_q_cfg_get num_uc_q_cfg_get;

    /*! Number of MC queues derived from MC_Q_MODE. */
    bcmtm_drv_num_mc_q_cfg_get num_mc_q_cfg_get;

    /*! Check if ITM is valid. */
    bcmtm_drv_itm_valid_valid_get itm_valid_get;

    /*! PFC deadlock handler function pointers. */
    bcmtm_drv_pfc_ddr_drv_get_f pfc_ddr_drv_get;

   /*! MoD driver function pointers. */
    bcmtm_drv_get_f mod_drv_get;

    /*! PFC handler function pointers. */
    bcmtm_drv_pfc_drv_get_f pfc_drv_get;

    /*! Scheduler driver get. */
    bcmtm_drv_get_f scheduler_drv_get;

    /*! Shaper driver get. */
    bcmtm_drv_get_f shaper_drv_get;

    /*! WRED driver get. */
    bcmtm_drv_get_f wred_drv_get;

    /*! OOBFC driver get. */
    bcmtm_drv_get_f oobfc_drv_get;

    /*! OBM driver get. */
    bcmtm_drv_get_f obm_drv_get;

    /*! Cut-through driver get. */
    bcmtm_drv_get_f ct_drv_get;

    /*! Multicast driver get. */
    bcmtm_drv_get_f mc_drv_get;

    /*! BST driver get. */
    bcmtm_drv_get_f bst_drv_get;

    /*! EBST driver get. */
    bcmtm_drv_get_f ebst_drv_get;

    /*! Device RX Queue driver get. */
    bcmtm_drv_get_f rxq_drv_get;

    /*! Port addition. */
    bcmtm_drv_port_op_f tm_port_add;

    /*! Port deletion. */
    bcmtm_drv_port_op_f tm_port_delete;

    /*! Drv_info port map update. */
    bcmtm_drv_portmap_fill_f portmap_update;

    /*! Update unicast drop config in IP. */
    bcmtm_update_unicast_drop_config_f update_unicast_drop_config;

    /*! Check Ep to MMU credit request loop. */
    bcmtm_check_ep_mmu_credit_f check_ep_mmu_credit;

    /*! Check Port to EP credit request loop. */
    bcmtm_check_port_ep_credit_f check_port_ep_credit;
} bcmtm_drv_t;

/*!
 * \brief Set the TM driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv TM driver to set.
 */
extern int
bcmtm_drv_set(int unit, bcmtm_drv_t *drv);

/*!
 * \brief Get the TM driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv TM driver to get.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmtm_drv_get(int unit, bcmtm_drv_t **drv);


/*!
 * \brief Allocates HA memory for device specific driver info.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in allocating driver info.
 */
extern int
bcmtm_drv_info_alloc(int unit, bool warm);

/*!
 * \brief Allocates HA memory for device specific multicast info.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in allocating driver info.
 */
extern int
bcmtm_mc_dev_info_alloc(int unit, bool warm);

/*!
 * \brief Get the TM multicast device information of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] mc_dev_info Multicast device info.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmtm_mc_dev_info_get(int unit, bcmtm_mc_dev_info_t **mc_dev_info);

/*!
 * \brief Allocates HA memory for device specific threshold related SW states.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in allocating thd info.
 */
extern int
bcmtm_thd_info_alloc(int unit, bool warm);

/*!
 * \brief Allocates HA memory for device specific OBM threshold SW states.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in allocating thd info.
 */
extern int
bcmtm_obm_thd_info_alloc(int unit, bool warm);

extern bcmdrd_sid_t
bcmtm_pt_sid_unique_xpe_pipe(int unit, bcmdrd_sid_t sid, int xpe, int pipe);


/*!
 * \brief Port ingress down.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL function not available for corresponding chip.
 * \retval  Error code.
 */
extern int
bcmtm_port_ing_down(int unit, bcmtm_pport_t port);

/*!
 * \brief Port ingress up.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL function not available for corresponding chip.
 * \retval  Error code.
 */
extern int
bcmtm_port_ing_up(int unit, bcmtm_pport_t port);

/*!
 * \brief Port mmu down.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL function not available for corresponding chip.
 * \retval  Error code.
 */
extern int
bcmtm_port_mmu_down(int unit, bcmtm_pport_t port);

/*!
 * \brief Port mmu up.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL function not available for corresponding chip.
 * \retval  Error code.
 */
extern int
bcmtm_port_mmu_up(int unit, bcmtm_pport_t port);

/*!
 * \brief Port egress down.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL function not available for corresponding chip.
 * \retval  Error code.
 */
extern int
bcmtm_port_egr_down(int unit, bcmtm_pport_t port);

/*!
 * \brief Port egress up.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL function not available for corresponding chip.
 * \retval  Error code.
 */
extern int
bcmtm_port_egr_up(int unit, bcmtm_pport_t port);

/*!
 * \brief Port egress forward disable.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL function not available for corresponding chip.
 * \retval  Error code.
 */
extern int
bcmtm_port_egr_fwd_disable(int unit, bcmtm_pport_t port);

/*!
 * \brief Port egress forward enable.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL function not available for corresponding chip.
 * \retval  Error code.
 */
extern int
bcmtm_port_egr_fwd_enable(int unit, bcmtm_pport_t port);

/*!
 * \brief Port egress credit reset.
 *
 * \param [in] unit Logical unit number.
 * \param [in] port Physical port number.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL function not available for corresponding chip.
 * \retval  Error code.
 */
extern int
bcmtm_port_egr_credit_reset_clr(int unit, bcmtm_pport_t port);

/*!
 * \brief Execute operations of a port configuration operation.
 *
 * This function will handle the TM operations for link up/down.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] op_name Operation name.
 * \param [in] op_param Operation parameter.
 */
int
bcmtm_port_op_exec(int unit, bcmtm_pport_t pport,
                   char *op_name, uint32_t op_param);

/*!
 * \brief TM modules regestering for IMM.
 *
 * \param [in] unit   Unit Number.
 */
extern int
bcmtm_imm_reg(int unit);

/*! \brief BCMTM queue assignment information driver get.
 *
 * \param [in] unit Logical unit number.
 * \param [out] drv queue assignement driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAILABLE Not available.
 */
extern int
bcmtm_q_assignment_info_drv_get(int unit, bcmcth_tm_info_drv_t **drv);

/*! \brief CPU cos queue map driver get.
 *
 * \param [in] unit Logical unit number.
 * \param [out] drv queue assignement driver.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAILABLE Not available.
 */
extern int
bcmtm_cos_q_cpu_map_info_drv_get(int unit, bcmcth_tm_info_drv_t **drv);

#endif /* BCMTM_DRV_H */
