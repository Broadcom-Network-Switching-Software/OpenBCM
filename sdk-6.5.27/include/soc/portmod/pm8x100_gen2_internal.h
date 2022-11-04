/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _PM8x100_GEN2_INTERNAL_H__H_
#define _PM8x100_GEN2_INTERNAL_H__H_

#ifdef PORTMOD_PM8X100_GEN2_SUPPORT

/*!
 * @brief Flags of available VCOs
 */
#define PM8x100_GEN2_VCO_ETH_41P25G    0x1 /**< Ethernet 41.25G */
#define PM8x100_GEN2_VCO_ETH_51P5625G  0x2 /**< Ethernet 51.5625G */
#define PM8x100_GEN2_VCO_ETH_53P125G   0x4 /**< Ethernet 53.125G */

#define PM8x100_GEN2_VCO_ETH_41P25G_SET(flags) ((flags) |= PM8x100_GEN2_VCO_ETH_41P25G)
#define PM8x100_GEN2_VCO_ETH_51P5625G_SET(flags) ((flags) |= PM8x100_GEN2_VCO_ETH_51P5625G)
#define PM8x100_GEN2_VCO_ETH_53P125G_SET(flags) ((flags) |= PM8x100_GEN2_VCO_ETH_53P125G)

#define PM8x100_GEN2_VCO_ETH_41P25G_CLR(flags) ((flags) &= ~PM8x100_GEN2_VCO_ETH_41P25G)
#define PM8x100_GEN2_VCO_ETH_51P5625G_CLR(flags) ((flags) &= ~PM8x100_GEN2_VCO_ETH_51P5625G)
#define PM8x100_GEN2_VCO_ETH_53P125G_CLR(flags) ((flags) &= ~PM8x100_GEN2_VCO_ETH_53P125G)

#define PM8x100_GEN2_VCO_ETH_41P25G_GET(flags) ((flags) & PM8x100_GEN2_VCO_ETH_41P25G ? 1 : 0)
#define PM8x100_GEN2_VCO_ETH_51P5625G_GET(flags) ((flags) & PM8x100_GEN2_VCO_ETH_51P5625G ? 1 : 0)
#define PM8x100_GEN2_VCO_ETH_53P125G_GET(flags) ((flags) & PM8x100_GEN2_VCO_ETH_53P125G ? 1 : 0)

#define PM8x100_GEN2_SPEED_ID_TABLE_STATUS_NOT_LOADED             0
#define PM8x100_GEN2_SPEED_ID_TABLE_STATUS_IEEE_MODE_LOADED       1
#define PM8x100_GEN2_SPEED_ID_TABLE_STATUS_HG3_MODE_LOADED        2

#endif /* PORTMOD_PM8X100_GEN2_SUPPORT */

#endif /*_PM8x100_GEN2_INTERNAL_H__H_*/
