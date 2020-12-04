/*! \file bcmcth_mon_rx_flex.h
 *
 * Packet RX access interface to obtain all profiles.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_RX_FLEX_H
#define BCMCTH_MON_RX_FLEX_H

#include <shr/shr_pb.h>
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Get all profiles for packet RX.
 *
 * Obtain all possible profiles for packet RX in flexible devices
 *
 * \param [in] variant Variant type.
 * \param [in, out] profiles bitmap of valid profiles.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support Reason.
 * \retval SHR_E_INTERNAL Internal issue.
 */
extern int
bcmcth_mon_rx_flex_profiles_get(bcmlrd_variant_t variant, SHR_BITDCL *profiles);

#endif /* BCMCTH_MON_RX_FLEX_H */
