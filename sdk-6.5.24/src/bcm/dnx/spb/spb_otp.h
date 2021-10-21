/** \file src/bcm/dnx/spb/spb_otp.h
 *
 * Internal DNX spb otp procedures to be used in spb init
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_SPB_OCBM_SPBOTP_H_INCLUDED
/*
 * {
 */
#define  _BCM_DNX_SPB_OCBM_SPBOTP_H_INCLUDED

/**
 * \brief This file is only used by DNX (JR2 family). Including it by
 * software that is not specific to DNX is an error.
 */
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

shr_error_e dnx_spb_otp_defective_banks_info_read(
    int unit,
    int defective_banks[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_DATA_MAX_SPB_OCB_NOF_RINGS]);

#endif /* _BCM_DNX_SPB_OCBM_SPBOTP_H_INCLUDED */
