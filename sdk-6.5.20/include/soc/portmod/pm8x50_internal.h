/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PM8x50_INTERNAL_H__H_
#define _PM8x50_INTERNAL_H__H_

#ifdef PORTMOD_PM8X50_SUPPORT

/*!
 * @brief Flags of available VCOs
 */
#define PM8x50_VCO_ILKN_20P625G 0x1 /**< ILKN 20.625G */
#define PM8x50_VCO_ETH_20P625G  0x2 /**< Ethernet 20.625G */
#define PM8x50_VCO_ILKN_25G     0x4 /**< ILKN 25G */
#define PM8x50_VCO_ETH_25P781G  0x8 /**< Ethernet 25.781G */
#define PM8x50_VCO_ILKN_25P781G 0x10 /**< ILKN 25.781G */
#define PM8x50_VCO_ETH_26P562G  0x20 /**< Ethernet 26.562G */
#define PM8x50_VCO_ILKN_26P562G 0x40 /**< ILKN 26.562G */
#define PM8x50_VCO_ILKN_27P343G 0x80 /**< ILKN 27.343G */
#define PM8x50_VCO_ILKN_28P125G 0x100 /**< ILKN 28.125G */
#define PM8x50_VCO_ETH_25G      0x200 /**< Ethernet 25G */

#define PM8x50_VCO_ILKN_20P625G_SET(flags) ((flags) |= PM8x50_VCO_ILKN_20P625G)
#define PM8x50_VCO_ETH_20P625G_SET(flags) ((flags) |= PM8x50_VCO_ETH_20P625G)
#define PM8x50_VCO_ILKN_25G_SET(flags) ((flags) |= PM8x50_VCO_ILKN_25G)
#define PM8x50_VCO_ETH_25P781G_SET(flags) ((flags) |= PM8x50_VCO_ETH_25P781G)
#define PM8x50_VCO_ILKN_25P781G_SET(flags) ((flags) |= PM8x50_VCO_ILKN_25P781G)
#define PM8x50_VCO_ETH_26P562G_SET(flags) ((flags) |= PM8x50_VCO_ETH_26P562G)
#define PM8x50_VCO_ILKN_26P562G_SET(flags) ((flags) |= PM8x50_VCO_ILKN_26P562G)
#define PM8x50_VCO_ILKN_27P343G_SET(flags) ((flags) |= PM8x50_VCO_ILKN_27P343G)
#define PM8x50_VCO_ILKN_28P125G_SET(flags) ((flags) |= PM8x50_VCO_ILKN_28P125G)
#define PM8x50_VCO_ETH_25G_SET(flags) ((flags) |= PM8x50_VCO_ETH_25G)

#define PM8x50_VCO_ILKN_20P625G_CLR(flags) ((flags) &= ~PM8x50_VCO_ILKN_20P625G)
#define PM8x50_VCO_ETH_20P625G_CLR(flags) ((flags) &= ~PM8x50_VCO_ETH_20P625G)
#define PM8x50_VCO_ILKN_25G_CLR(flags) ((flags) &= ~PM8x50_VCO_ILKN_25G)
#define PM8x50_VCO_ETH_25P781G_CLR(flags) ((flags) &= ~PM8x50_VCO_ETH_25P781G)
#define PM8x50_VCO_ILKN_25P781G_CLR(flags) ((flags) &= ~PM8x50_VCO_ILKN_25P781G)
#define PM8x50_VCO_ETH_26P562G_CLR(flags) ((flags) &= ~PM8x50_VCO_ETH_26P562G)
#define PM8x50_VCO_ILKN_26P562G_CLR(flags) ((flags) &= ~PM8x50_VCO_ILKN_26P562G)
#define PM8x50_VCO_ILKN_27P343G_CLR(flags) ((flags) &= ~PM8x50_VCO_ILKN_27P343G)
#define PM8x50_VCO_ILKN_28P125G_CLR(flags) ((flags) &= ~PM8x50_VCO_ILKN_28P125G)
#define PM8x50_VCO_ETH_25G_CLR(flags) ((flags) &= ~PM8x50_VCO_ETH_25G)

#define PM8x50_VCO_ILKN_20P625G_GET(flags) ((flags) & PM8x50_VCO_ILKN_20P625G ? 1 : 0)
#define PM8x50_VCO_ETH_20P625G_GET(flags) ((flags) & PM8x50_VCO_ETH_20P625G ? 1 : 0)
#define PM8x50_VCO_ILKN_25G_GET(flags) ((flags) & PM8x50_VCO_ILKN_25G ? 1 : 0)
#define PM8x50_VCO_ETH_25P781G_GET(flags) ((flags) & PM8x50_VCO_ETH_25P781G ? 1 : 0)
#define PM8x50_VCO_ILKN_25P781G_GET(flags) ((flags) & PM8x50_VCO_ILKN_25P781G ? 1 : 0)
#define PM8x50_VCO_ETH_26P562G_GET(flags) ((flags) & PM8x50_VCO_ETH_26P562G ? 1 : 0)
#define PM8x50_VCO_ILKN_26P562G_GET(flags) ((flags) & PM8x50_VCO_ILKN_26P562G ? 1 : 0)
#define PM8x50_VCO_ILKN_27P343G_GET(flags) ((flags) & PM8x50_VCO_ILKN_27P343G ? 1 : 0)
#define PM8x50_VCO_ILKN_28P125G_GET(flags) ((flags) & PM8x50_VCO_ILKN_28P125G ? 1 : 0)
#define PM8x50_VCO_ETH_25G_GET(flags) ((flags) & PM8x50_VCO_ETH_25G ? 1 : 0)

/*!
 * @brief Flags for PM8x50 version.
 */
#define PM8X50_PM_VERSION_F_REV0_16NM 0x1 /**< PM8X50, revision 0, 16nm */
#define PM8X50_PM_VERSION_F_FLEXE     0x2 /**< PM8X50, Flex E      16nm */

#define PM8X50_PM_VERSION_F_REV0_16NM_SET(flags) ((flags) |= PM8X50_PM_VERSION_F_REV0_16NM)
#define PM8X50_PM_VERSION_F_REV0_16NM_CLR(flags) ((flags) &= ~PM8X50_PM_VERSION_F_REV0_16NM)
#define PM8X50_PM_VERSION_F_REV0_16NM_GET(flags) ((flags) & PM8X50_PM_VERSION_F_REV0_16NM ? 1 : 0)
#define PM8X50_PM_VERSION_F_FLEXE_SET(flags) ((flags) |= PM8X50_PM_VERSION_F_FLEXE)
#define PM8X50_PM_VERSION_F_FLEXE_CLR(flags) ((flags) &= ~PM8X50_PM_VERSION_F_FLEXE)
#define PM8X50_PM_VERSION_F_FLEXE_GET(flags) ((flags) & PM8X50_PM_VERSION_F_FLEXE ? 1 : 0)

/*!
 * @brief Flags for Ethernet TVCO value.
 */
#define PM8X50_TVCO_SUPPORT_20P625G 0x1 /**< Support TVCO running at 20.625G in Ethernet mode. */
#define PM8X50_TVCO_FLEXE_SUPPORT_26P562G   0x2 /**< Support TVCO running at 26.562G for Flexe config */
#define PM8X50_TVCO_PRIORITIZE_26P562G 0x4 /**< Prioritize 26.562G when assigning TVCO rate. */

#define PM8X50_TVCO_SUPPORT_20P625G_SET(flags) ((flags) |= PM8X50_TVCO_SUPPORT_20P625G)
#define PM8X50_TVCO_SUPPORT_20P625G_CLR(flags) ((flags) &= ~PM8X50_TVCO_SUPPORT_20P625G)
#define PM8X50_TVCO_SUPPORT_20P625G_GET(flags) ((flags) & PM8X50_TVCO_SUPPORT_20P625G ? 1: 0)

#define PM8X50_TVCO_FLEXE_SUPPORT_26P562G_SET(flags) ((flags) |= PM8X50_TVCO_FLEXE_SUPPORT_26P562G)
#define PM8X50_TVCO_FLEXE_SUPPORT_26P562G_CLR(flags) ((flags) &= ~PM8X50_TVCO_FLEXE_SUPPORT_26P562G)
#define PM8X50_TVCO_FLEXE_SUPPORT_26P562G_GET(flags) ((flags) & PM8X50_TVCO_FLEXE_SUPPORT_26P562G ? 1: 0)

#define PM8X50_TVCO_PRIORITIZE_26P562G_SET(flags) ((flags) |= PM8X50_TVCO_PRIORITIZE_26P562G)
#define PM8X50_TVCO_PRIORITIZE_26P562G_CLR(flags) ((flags) &= ~PM8X50_TVCO_PRIORITIZE_26P562G)
#define PM8X50_TVCO_PRIORITIZE_26P562G_GET(flags) ((flags) & PM8X50_TVCO_PRIORITIZE_26P562G ? 1: 0)

/* VCOs bitmap type */
typedef uint16_t pm8x50_vcos_bmp_t;

/* Function gets current TVCO, OVCO and tries to accommodate input VCO requests */
int pm8x50_vcos_request_allocate(int unit,
                                 uint32 flags,
                                 const pm8x50_vcos_bmp_t required_vcos_bmp,
                                 portmod_vco_type_t *current_tvco,
                                 portmod_vco_type_t *current_ovco);

#endif /* PORTMOD_PM8X50_SUPPORT */
 
#endif /*_PM8x50_INTERNAL_H__H_*/
