/*! \file bcmcth_l3_bank.h
 *
 * BCMCTH_L3_BANK Custom Handler header file
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_L3_BANK_H
#define BCMCTH_L3_BANK_H

/*******************************************************************************
  Includes
 */
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>
#include <shr/shr_bitop.h>

/*! Maximum number of L3 banks. */
#define L3_BANK_MAX_BIT 32

/*!
 * L3 bank control lt structure
 */
typedef struct bcmcth_l3_bank_s {
    /*! Number of overlay next hops. */
    uint8_t    num_o_nhop;
    /*! Validity of num_o_nhop field. */
    bool       num_o_nhop_valid;
    /*! Next hops banks bitmap. */
    SHR_BITDCLNAME(nhop_bank_bitmap, L3_BANK_MAX_BIT) ;
    /*! Number of overlay egress interface. */
    uint8_t    num_o_eif;
    /*! Validity of num_o_eif field. */
    bool       num_o_eif_valid;
    /*! Eif banks bitmap. */
    SHR_BITDCLNAME(eif_bank_bitmap, L3_BANK_MAX_BIT);
} bcmcth_l3_bank_t;

/*!
 * L3 bank control lt structure
 */
typedef struct bcmcth_l3_bank_control_s {
    /*! Next hops banks bitmap. */
    SHR_BITDCLNAME(nhop_bank_bitmap, L3_BANK_MAX_BIT) ;
    /*! Eif banks bitmap. */
    SHR_BITDCLNAME(eif_bank_bitmap, L3_BANK_MAX_BIT);
} bcmcth_l3_bank_control_t;

/*!
 * \brief L3 bank driver encode.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_bank The l3 bank struct ptr.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
typedef int (*bcmcth_l3_bank_encode_f)(int unit, bcmcth_l3_bank_t *l3_bank);

/*!
 * \brief L3 bank driver decode.
 *
 * \param [in] unit Unit number.
 * \param [in] l3_bank The l3 bank struct ptr.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
typedef int (*bcmcth_l3_bank_decode_f)(int unit, bcmcth_l3_bank_t *l3_bank);


/*!
 * \brief L3 bank driver set.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  l3_bank       L3 bank struct info.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
typedef int (*bcmcth_l3_bank_set_f)(int unit,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmltd_sid_t lt_id,
                                      bcmcth_l3_bank_t l3_bank);

/*!
 * \brief L3 bank driver get.
 *
 * \param [in] unit           Unit number.
 * \param [in] op_arg         The operation arguments.
 * \param [in] lt_id          Logical table id.
 * \param [in/out] l3_bank    pointer of l3_bank.
 * \param [in] ireq           read via ireq PTM API.
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
typedef int (*bcmcth_l3_bank_get_f)(int unit,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmltd_sid_t lt_id,
                                      bcmcth_l3_bank_t *l3_bank,
                                      bool ireq);

/*!
 * \brief bcm cth L3 bank driver structure.
 */
typedef struct bcmcth_l3_bank_drv_s {
    /*! Next hop LT IDs count. */
    int nhop_lt_id_count;

    /*! Next hop LT ID list. */
    bcmltd_sid_t *nhop_lt_id_list;

    /*! EIF LT IDs count. */
    int eif_lt_id_count;

    /*! EIF LT ID list. */
    bcmltd_sid_t *eif_lt_id_list;

    /*! Bank index offset in nhop ID. */
    uint8_t nhop_bank_index_offset;

    /*! Bank index offset in egress inferface ID. */
    uint8_t eif_bank_index_offset;

    /*! Encode the bitmaps based on overlay bank numbers. */
    bcmcth_l3_bank_encode_f  l3_bank_encode;

    /*! Decode the bitmaps based on overlay bank numbers. */
    bcmcth_l3_bank_decode_f  l3_bank_decode;

    /*! Set the hardware driver. */
    bcmcth_l3_bank_set_f  l3_bank_set;

    /*! Get the hardware driver. */
    bcmcth_l3_bank_get_f  l3_bank_get;
} bcmcth_l3_bank_drv_t;


/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern bcmcth_l3_bank_drv_t *_bc##_cth_l3_bank_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Install the device-specific L3 bank driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_l3_bank_drv_init(int unit);

/*!
 * \brief Get the device-specific L3 bank driver.
 *
 * \param [in] unit Unit number.
 * \param [in] drv  l3 bank driver ptr of ptr.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAILABLE Unavailable.
 */
extern int
bcmcth_l3_bank_drv_get(int unit, bcmcth_l3_bank_drv_t **drv);

/*!
 * \brief BCMCTH L3 bank control software init.
 *
 * \param [in] unit Unit number.
 * \param [in] warm warm boot flag.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_l3_bank_control_init(int unit, bool warm);

/*!
 * \brief BCM CTH L3 bank set.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  l3_bank       L3 banck struct info.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_l3_bank_set(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   bcmltd_sid_t lt_id,
                   bcmcth_l3_bank_t l3_bank);

/*!
 * \brief BCM CTH L3 bank get.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        The operation arguments.
 * \param [in]  lt_id         Logical table id.
 * \param [in]  l3_bank       L3 banck struct ptr.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_l3_bank_get(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   bcmltd_sid_t lt_id,
                   bcmcth_l3_bank_t *l3_bank);

/*!
 * \brief Check L3 bank usage.
 *
 * \param [in] unit           Unit Number.
 * \param [in] trans_id       The transaction ID.
 * \param [in] l3_bank        The l3 bank struct ptr.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_BUSY When there is an entry configured in the related LTs.
 */
extern int
bcmcth_l3_bank_usage_check(int unit,
                           uint32_t trans_id,
                           bcmcth_l3_bank_t l3_bank);

/*!
 * \brief Check if L3 next hop ID is overlay based on L3 bank control.
 *
 * \param [in] unit           Unit Number.
 * \param [in] nhop_id        Nexthop ID.
 * \param [in] is_overlay     Indicate it is overlay nhop.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmcth_l3_bank_nhop_id_overlay(int unit,
                               uint16_t nhop_id,
                               bool *is_overlay);

/*!
 * \brief Check if L3 eif ID is overlay based on L3 bank control.
 *
 * \param [in] unit           Unit Number.
 * \param [in] eif_id         Egress interface ID.
 * \param [in] is_overlay     Indicate it is overlay nhop.
 *
 * \return SHR_E_NONE No errors.
 */
extern int
bcmcth_l3_bank_eif_id_overlay(int unit,
                              uint16_t eif_id,
                              bool *is_overlay);

#endif /* BCMCTH_L3_BANK_H */
