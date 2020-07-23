/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _IMB_INTERNAL_H_
#define _IMB_INTERNAL_H_

#include <bcm_int/dnx/port/imb/imb.h>

/**
 * \brief - get imb_type from imb_id
 *
 * \param [in] unit   - chip unit id.
 * \param [in] imb_id - imb instance id.
 * \param [in] type   - imb dispatcher type.
 *
 *
 * \return
 *   int  - err code. see _SHR_E_*
 *
 * \see
 *   * None
 */
int imb_id_type_get(
    int unit,
    int imb_id,
    imb_dispatch_type_t * type);

/**
 * \brief - get imb_type from ethu_id
 *
 * \param [in] unit   - chip unit id.
 * \param [in] ethu_id - ethu id.
 * \param [in] type   - imb dispatcher type.
 *
 *
 * \return
 *   int  - err code. see _SHR_E_*
 *
 * \see
 *   * None
 */
int ethu_id_type_get(
    int unit,
    int ethu_id,
    imb_dispatch_type_t * type);

/**
 * \brief - get imb_type from imb_id
 *
 * \param [in]  unit     - chip unit id.
 * \param [out] nof_imbs - number of imb instances in use
 *
 *
 * \return
 *   int  - err code. see _SHR_E_*
 *
 * \see
 *   * None
 */
int imb_nof_get(
    int unit,
    uint32 *nof_imbs);

#endif /*_IMB_INTERNAL_H_*/
