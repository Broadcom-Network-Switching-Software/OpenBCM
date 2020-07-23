/*****************************************************************************************
 *****************************************************************************************
 *                                                                                       *
 *  Revision      :  $Id: eagle_merlin_api_uc_common.h 924 2015-02-24 18:08:11Z eroes $ *
 *                                                                                       *
 *  Description   :  Defines and Enumerations required by Eagle/Merlin APIs              *
 *                                                                                       *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.                                                                 *
 *  No portions of this material may be reproduced in any form without                   *
 *  the written permission of:                                                           *
 *      Broadcom Corporation                                                             *
 *      5300 California Avenue                                                           *
 *      Irvine, CA  92617                                                                *
 *                                                                                       *
 *  All information contained in this document is Broadcom Corporation                   *
 *  company private proprietary, and trade secret.                                       *
 */

/** @file eagle_merlin_api_uc_common.h
 * Defines and Enumerations shared by Eagle & Merlin IP Specific API and Microcode
 */

#ifndef EAGLE_MERLIN_API_UC_COMMON_H
#define EAGLE_MERLIN_API_UC_COMMON_H

/* Add Eagle/Merlin specific items below this */ 

/* Please note that when adding entries here you should update the #defines in the merlin_koi_common.h */

/** OSR_MODES Enum */
enum merlin_koi_osr_mode_enum {
  MERLIN_KOI_OSX1    = 0,
  MERLIN_KOI_OSX2    = 1,
  MERLIN_KOI_OSX3    = 2,
  MERLIN_KOI_OSX3P3  = 3,
  MERLIN_KOI_OSX4    = 4,
  MERLIN_KOI_OSX5    = 5,
  MERLIN_KOI_OSX7P5  = 6,
  MERLIN_KOI_OSX8    = 7,
  MERLIN_KOI_OSX8P25 = 8,
  MERLIN_KOI_OSX10   = 9
};


/** VCO_RATE Enum */
enum merlin_koi_vco_rate_enum {
  MERLIN_KOI_VCO_5P5G = 0,
  MERLIN_KOI_VCO_5P75G,
  MERLIN_KOI_VCO_6G,
  MERLIN_KOI_VCO_6P25G,
  MERLIN_KOI_VCO_6P5G,
  MERLIN_KOI_VCO_6P75G,
  MERLIN_KOI_VCO_7G,
  MERLIN_KOI_VCO_7P25G,
  MERLIN_KOI_VCO_7P5G,
  MERLIN_KOI_VCO_7P75G,
  MERLIN_KOI_VCO_8G,
  MERLIN_KOI_VCO_8P25G,
  MERLIN_KOI_VCO_8P5G,
  MERLIN_KOI_VCO_8P75G,
  MERLIN_KOI_VCO_9G,
  MERLIN_KOI_VCO_9P25G,
  MERLIN_KOI_VCO_9P5G,
  MERLIN_KOI_VCO_9P75G,
  MERLIN_KOI_VCO_10G,
  MERLIN_KOI_VCO_10P25G,
  MERLIN_KOI_VCO_10P5G,
  MERLIN_KOI_VCO_10P75G,
  MERLIN_KOI_VCO_11G,
  MERLIN_KOI_VCO_11P25G,
  MERLIN_KOI_VCO_11P5G,
  MERLIN_KOI_VCO_11P75G,
  MERLIN_KOI_VCO_12G,
  MERLIN_KOI_VCO_12P25G,
  MERLIN_KOI_VCO_12P5G,
  MERLIN_KOI_VCO_12P75G,
  MERLIN_KOI_VCO_13G,
  MERLIN_KOI_VCO_13P25G
};

#endif   
