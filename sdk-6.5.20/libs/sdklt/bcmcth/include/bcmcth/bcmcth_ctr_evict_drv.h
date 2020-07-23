/*! \file bcmcth_ctr_evict_drv.h
 *
 * BCMCTH CTR_EVICT Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BCMBD) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_EVICT_DRV_H
#define BCMCTH_CTR_EVICT_DRV_H

#include <bcmltd/bcmltd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_cci.h>

/*! Invalid pool ID */
#define CTR_INVALID_POOL     (-1)

/*!
 * \brief Counter pool types
 */
typedef enum ctr_pool_e {
    /*! Ingress flex counters */
    CTR_POOL_ING_FLEX = 0,
    /*! Egress flex counters */
    CTR_POOL_EGR_FLEX,
    /*! Egress per Q counter */
    CTR_POOL_EGR_PERQ,
    /*! Egress FP counter */
    CTR_POOL_EGR_FP,
    /*! Number of pool type */
    CTR_POOL_COUNT
} ctr_pool_t;

/*!
 * \brief Counter parameters update types for ctr_control_entry_t
 */
typedef enum ctr_update_e {
    /*! Update the eviction mode */
    CTR_UPDATE_EVICT_MODE,
    /*! Update the eviction THD of bytes */
    CTR_UPDATE_EVICT_THD_BYTES,
    /*! Update the eviction THD of pkts */
    CTR_UPDATE_EVICT_THD_PKTS,
    /*! Update the eviction THD of CTR A */
    CTR_UPDATE_EVICT_THD_CTR_A,
    /*! Update the eviction THD of CTR B */
    CTR_UPDATE_EVICT_THD_CTR_B,
    /*! Update the eviction seed */
    CTR_UPDATE_EVICT_SEED,
    /*! Number of eviction updates */
    CTR_UPDATE_COUNT
} ctr_evict_update_t;

/*! Update flag for eviction mode */
#define CTR_UPDATE_F_E_MODE        (1 << CTR_UPDATE_EVICT_MODE)
/*! Update flag for eviction THD of bytes */
#define CTR_UPDATE_F_E_THD_BYTES   (1 << CTR_UPDATE_EVICT_THD_BYTES)
/*! Update flag for eviction THD of pkts */
#define CTR_UPDATE_F_E_THD_PKTS    (1 << CTR_UPDATE_EVICT_THD_PKTS)
/*! Update flag for eviction seed */
#define CTR_UPDATE_F_E_SEED        (1 << CTR_UPDATE_EVICT_SEED)
/*! Update flag for eviction THD of CTR A */
#define CTR_UPDATE_F_E_THD_CTR_A   (1 << CTR_UPDATE_EVICT_THD_CTR_A)
/*! Update flag for eviction THD of CTR B */
#define CTR_UPDATE_F_E_THD_CTR_B   (1 << CTR_UPDATE_EVICT_THD_CTR_B)

/*! Eviction mode */
typedef enum bcmptm_cci_ctr_evict_mode_e ctr_evict_mode_t;

/*!
 * \brief Counter control entry
 */
typedef struct ctr_control_entry_s {
    /*! Counter pool type */
    ctr_pool_t          pool;
    /*! Pool ID */
    uint32_t            pool_id;
    /*! Eviction mode */
    ctr_evict_mode_t    evict_mode;
    /*! Eviction threshold bytes value */
    uint64_t            evict_thd_bytes;
    /*! Eviction threshold pkts value*/
    uint32_t            evict_thd_pkts;
    /*! Eviction random seed value */
    uint64_t            evict_seed;
    /*! Eviction threshold counter A */
    uint64_t            evict_thd_ctr_a;
    /*! Eviction threshold counter B */
    uint64_t            evict_thd_ctr_b;
    /*! Counter parameters update flags */
    uint32_t            update_flags;
} ctr_control_entry_t;

/*! Function type for eviction initialization */
typedef int (*ctr_evict_init_f)(int unit);

/*!
 * \brief Counter eviction control entry update/configuration.
 *
 * Invoke device specific routine to update parameters accordingly.
 *
 * \param [in] unit Device unit.
 * \param [in] ltid Logical table ID
 * \param [in] trans_id Transaction ID
 * \param [in] entry Configuration structure pointer to ctr_control_entry_t.
 */
typedef int (*ctr_evict_entry_update_f)(int unit,
                                        bcmltd_sid_t ltid,
                                        uint32_t trans_id,
                                        ctr_control_entry_t *entry);
/*!
 * \brief CTR EVICT driver object
 *
 * CTR EVICT driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH CTR EVICT module by
 * \ref bcmcth_ctr_evict_drv_init() from the top layer. BCMCTH CTR EVICT
 * internally will use this interface to get the corresponding information.
 */
typedef struct bcmcth_ctr_evict_drv_s {
    /*! eviction control initialization */
    ctr_evict_init_f            evict_init_fn;

    /*! eviction control entry update/configuration */
    ctr_evict_entry_update_f    evict_entry_update_fn;

} bcmcth_ctr_evict_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,\
                             _fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_ctr_evict_drv_t *_bd##_cth_ctr_evict_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief CTR EVICT control entry update/configuration.
 *
 * Invoke device specific routine to update parameters accordingly.
 *
 * \param [in] unit Device unit.
 * \param [in] ltid Logical table ID
 * \param [in] trans_id Transaction ID
 * \param [in] entry Configuration structure pointer to ctr_control_entry_t.
 */
extern int
bcmcth_ctr_evict_entry_update(int unit,
                              bcmltd_sid_t ltid,
                              uint32_t trans_id,
                              ctr_control_entry_t *entry);

/*!
 * \brief CTR EVICT initialization.
 *
 * Invoke device specific routine to specify memory identifications for eviction
 * eligible counter pools.
 *
 * \param [in] unit Device unit.
 * \param [in] warm Warm boot
 */
int
bcmcth_ctr_evict_dev_init(int unit, bool warm);

/*!
 * \brief Get the CTR EVICT driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_evict_drv_init(int unit);

/*!
 * \brief Cleanup the CTR EVICT driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ctr_evict_drv_cleanup(int unit);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The CTR_EVICT custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmcth_ctr_evict_imm_init(int unit);
#endif /* BCMCTH_CTR_EVICT_DRV_H */
