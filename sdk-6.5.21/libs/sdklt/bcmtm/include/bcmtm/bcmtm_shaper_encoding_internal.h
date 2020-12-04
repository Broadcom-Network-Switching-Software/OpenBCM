/*! \file bcmtm_shaper_encoding_internal.h
 *
 * TM shaper rate encodings. APIs to converts user readable format to
 * physical table readable format.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_SHAPER_ENCODING_INTERNAL_H
#define BCMTM_SHAPER_ENCODING_INTERNAL_H

#include <bcmtm/sched_shaper/bcmtm_shaper_internal.h>
/*!
 * \brief Shaper user readable format to physical table readable format.
 *
 * \param [in] unit           Unit number.
 * \param [in] lport          Logical port number.
 * \param [in] shaping_mode   Shaping mode (packet_mode/ byte_mode).
 * \param [in] burst_auto     Burst auto update. It defines if burst_auto = 1,
 * then uses bandwidth to get burst size, else uses user defined burst size.
 * \param [out] bucket_encode Bucket encodings(physical table readable format).
 */
extern int
bcmtm_shaper_rate_to_bucket (int unit,
                             bcmtm_lport_t lport,
                             uint32_t shaping_mode,
                             uint8_t burst_auto,
                             bcmtm_shaper_bucket_encode_t *bucket_encode);

/*!
 * \brief Shaper physical table readable format to user readable format.
 *
 * \param [in] unit              Unit number.
 * \param [in] shaping_mode      Shaping mode (packet mode/ byte mode).
 * \param [out] bucket_encode Bucket encodings (physical table readable format).
 */
extern int
bcmtm_shaper_bucket_to_rate(int unit,
                            uint32_t shaping_mode,
                            bcmtm_shaper_bucket_encode_t *bucket_encode);

#endif /* BCMTM_SHAPER_ENCODING_INTERNAL_H */

