/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgs3.h
 * Purpose:     Definitions for XGS3 systems.
 */

#ifndef _BCM_INT_XGS3_H_
#define _BCM_INT_XGS3_H_

/* Color MACROs */

#define _BCM_XGS3_COLOR_GREEN   0
#define _BCM_XGS3_COLOR_YELLOW  3
#define _BCM_XGS3_COLOR_RED     1
#define _BCM_XGS3_COLOR_NONE    0
#define _BCM_XGS3_COLOR_RESERVED   2

#define _BCM_COLOR_ENCODING(unit, color) \
        (SOC_IS_XGS3_SWITCH(unit) ? \
          (((color) == bcmColorGreen) ? _BCM_XGS3_COLOR_GREEN : \
            (((color) == bcmColorYellow) ? _BCM_XGS3_COLOR_YELLOW : \
             (((color) == bcmColorRed) ? _BCM_XGS3_COLOR_RED : \
              _BCM_XGS3_COLOR_NONE))) : \
               _BCM_XGS3_COLOR_NONE)

#define _BCM_COLOR_DECODING(unit, hw_color) \
        (SOC_IS_XGS3_SWITCH(unit) ? \
          ((hw_color) == _BCM_XGS3_COLOR_GREEN) ? bcmColorGreen : \
           ((hw_color) == _BCM_XGS3_COLOR_YELLOW) ? bcmColorYellow : \
            (((hw_color) == _BCM_XGS3_COLOR_RED) ? bcmColorRed : bcmColorGreen) \
            : bcmColorGreen)


#endif  /* !_BCM_INT_XGS3_H_ */
