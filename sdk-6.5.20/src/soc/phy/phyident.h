/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phyident.h
 * Purpose:     Basic defines for PHY Model/OUI/Rev for PHY identification. 
 * Note:        This header file is for PHY driver module. Should not include
 *              this header file in SOC or BCM layers.
 */

#ifndef _PHY_IDENT_H
#define _PHY_IDENT_H

/*
 * Each PHY is identified by OUI (Organizationally Unique Identifier),
 * model number, and revision as found in IEEE registers 2 and 3.
 */

#define PHY_BRCM_OUI1   0x001018        /* Broadcom original */
#define PHY_BRCM_OUI1R  0x000818        /* Broadcom reversed */
#define PHY_BRCM_OUI2   0x000af7        /* Broadcom new */
#define PHY_BRCM_OUI3   0x001be9        /* Broadcom new */
#define PHY_BRCM_OUI4   0x00c086        /* Broadcom OUI */
#define PHY_BRCM_OUI5   0x18c086        /* Broadcom OUI */
#define PHY_BRCM_OUI6   0xd40129        /* Broadcom OUI */
#define PHY_BRCM_OUI7   0x000a0c        /* Broadcom OUI */
#define PHY_BRCM_OUI8   0xb02628        /* Broadcom OUI */

#define PHY_ALTIMA_OUI  0x0010a9        /* Altima */

/*
 * Macros to Encode/Decode PHY_ID0/ID1 registers (MDIO register 0x02/0x03)
 */

#define PHY_ID0(oui, model, rev) \
    ((uint16)(_shr_bit_rev_by_byte_word32(oui) >> 6 & 0xffff))

#define PHY_ID1(oui, model, rev) \
    ((uint16)(_shr_bit_rev_by_byte_word32(oui) & 0x3f) << 10 | \
             ((model) & 0x3f) << 4 | \
             ((rev) & 0x0f) << 0)

#define PHY_OUI(id0, id1) \
    _shr_bit_rev_by_byte_word32((uint32)(id0) << 6 | ((id1) >> 10 & 0x3f))

#define PHY_MODEL(id0, id1) ((id1) >> 4 & 0x3f)

#define PHY_OUI_MODEL(oui, model)       (((uint32)(oui) << 8) | (model))

#define PHY_REV(id0, id1)   ((id1) & 0xf)

/*
 * Identifiers of known PHYs
 */

#define PHY_BCM5218_OUI         PHY_BRCM_OUI1R
#define PHY_BCM5218_MODEL       0x1f

#define PHY_BCM5220_OUI         PHY_BRCM_OUI1R          /* & BCM5221 */
#define PHY_BCM5220_MODEL       0x1e

#define PHY_BCM5226_OUI         PHY_BRCM_OUI1R
#define PHY_BCM5226_MODEL       0x1c

#define PHY_BCM5228_OUI         PHY_BRCM_OUI1R
#define PHY_BCM5228_MODEL       0x1d

#define PHY_BCM5238_OUI         PHY_BRCM_OUI1R
#define PHY_BCM5238_MODEL       0x20

#define PHY_BCM5248_OUI         PHY_BRCM_OUI2
#define PHY_BCM5248_MODEL       0x00

#define PHY_BCM5400_OUI         PHY_BRCM_OUI1
#define PHY_BCM5400_MODEL       0x04

#define PHY_BCM5401_OUI         PHY_BRCM_OUI1
#define PHY_BCM5401_MODEL       0x05

#define PHY_BCM5402_OUI         PHY_BRCM_OUI1
#define PHY_BCM5402_MODEL       0x06

#define PHY_BCM5404_OUI         PHY_BRCM_OUI1
#define PHY_BCM5404_MODEL       0x08

#define PHY_BCM5411_OUI         PHY_BRCM_OUI1
#define PHY_BCM5411_MODEL       0x07

#define PHY_BCM5421_OUI         PHY_BRCM_OUI1           /* & 5421S */
#define PHY_BCM5421_MODEL       0x0e

#define PHY_BCM5424_OUI         PHY_BRCM_OUI1           /* & 5234 */
#define PHY_BCM5424_MODEL       0x0a

#define PHY_BCM5461_OUI         PHY_BRCM_OUI1           /* & 5461S */
#define PHY_BCM5461_MODEL       0x0c

#define PHY_BCM5462_OUI         PHY_BRCM_OUI1           /* & 5461D */
#define PHY_BCM5462_MODEL       0x0d

#define PHY_BCM5464_OUI         PHY_BRCM_OUI1           /* & 5464S */
#define PHY_BCM5464_MODEL       0x0b

#define PHY_BCM5466_OUI         PHY_BRCM_OUI1           /* & 5466S */
#define PHY_BCM5466_MODEL       0x3b

#define PHY_BCM5478_OUI         PHY_BRCM_OUI2
#define PHY_BCM5478_MODEL       0x08

#define PHY_BCM5488_OUI         PHY_BRCM_OUI2
#define PHY_BCM5488_MODEL       0x09

#define PHY_BCM5482_OUI         PHY_BRCM_OUI2           /* & 5482S */
#define PHY_BCM5482_MODEL       0x0b

#define PHY_BCM5481_OUI         PHY_BRCM_OUI2           /* & 5481 */
#define PHY_BCM5481_MODEL       0x0a

#define PHY_BCM54616_OUI        PHY_BRCM_OUI3
#define PHY_BCM54616_MODEL      0x11

#define PHY_BCM54618SE_OUI      PHY_BRCM_OUI3
#define PHY_BCM54618SE_MODEL    0x0D

#define PHY_BCM54618E_OUI       PHY_BRCM_OUI3
#define PHY_BCM54618E_MODEL     0x26

#define PHY_BCM54680_OUI        PHY_BRCM_OUI3
#define PHY_BCM54680_MODEL      0x1c

/* Model 0x17 is being used by 54880E now */
#define PHY_BCM54880E_OUI        PHY_BRCM_OUI3
#define PHY_BCM54880E_MODEL      0x17

/* MSB of rev ID is used to distinguish between BCM54680E and BCM54640E */
#define PHY_BCM54680E_OUI       PHY_BRCM_OUI3
#define PHY_BCM54680E_MODEL     0x27

/* MSB of rev ID is used to distinguish between BCM54682E and BCM54685E */
#define PHY_BCM54682E_OUI       PHY_BRCM_OUI3
#define PHY_BCM54682E_MODEL     0x12

#define PHY_BCM54685_OUI        PHY_BRCM_OUI3
#define PHY_BCM54685_MODEL      0x1d

#define PHY_BCM54880_OUI        PHY_BRCM_OUI3
#define PHY_BCM54880_MODEL      0x1e

#define PHY_BCM54881_OUI        PHY_BRCM_OUI3
#define PHY_BCM54881_MODEL      0x1f

#define PHY_BCM54810_OUI        PHY_BRCM_OUI3
#define PHY_BCM54810_MODEL      0x10

#define PHY_BCM54811_OUI        PHY_BRCM_OUI3
#define PHY_BCM54811_MODEL      0x0c

#define PHY_BCM54684_OUI        PHY_BRCM_OUI2
#define PHY_BCM54684_MODEL      0x2f

#define PHY_BCM54684E_OUI       PHY_BRCM_OUI3
#define PHY_BCM54684E_MODEL     0x0e

#define PHY_BCM52681E_OUI       PHY_BRCM_OUI3
#define PHY_BCM52681E_MODEL     0x0f

/* MSB of rev ID is used to distinguish between BCM54640 and BCM52681E */
#define PHY_BCM54640_OUI        PHY_BRCM_OUI3
#define PHY_BCM54640_MODEL      0x1b

/* begin BCM84728 family */
#define PHY_BCM84707_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84707_MODEL      0x30 
#define PHY_BCM84073_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84073_MODEL      0x30 
#define PHY_BCM84074_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84074_MODEL      0x30 
#define PHY_BCM84728_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84728_MODEL      0x31 
#define PHY_BCM84748_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84748_MODEL      0x31 
#define PHY_BCM84727_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84727_MODEL      0x31 
#define PHY_BCM84747_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84747_MODEL      0x31 
#define PHY_BCM84762_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84762_MODEL      0x32 
#define PHY_BCM84764_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84764_MODEL      0x32 
#define PHY_BCM84042_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84042_MODEL      0x32 
#define PHY_BCM84044_OUI        PHY_BRCM_OUI5 
#define PHY_BCM84044_MODEL      0x32 
/* end BCM84728 family */

#if defined(INCLUDE_MACSEC) || defined(INCLUDE_FCMAP)
#define PHY_BCM54580_OUI        PHY_BRCM_OUI3
#define PHY_BCM54580_MODEL      0x14

#define PHY_BCM54584_OUI        PHY_BRCM_OUI3
#define PHY_BCM54584_MODEL      0x15

#define PHY_BCM54540_OUI        PHY_BRCM_OUI3
#define PHY_BCM54540_MODEL      0x16

#define PHY_BCM54380_OUI        PHY_BRCM_OUI5
#define PHY_BCM54380_MODEL      0x00

#define PHY_BCM54382_OUI        PHY_BRCM_OUI5
#define PHY_BCM54382_MODEL      0x01  /*Rev = 8*/

#define PHY_BCM54340_OUI        PHY_BRCM_OUI5
#define PHY_BCM54340_MODEL      0x2   /*Rev = 0*/

#define PHY_BCM54385_OUI        PHY_BRCM_OUI5
#define PHY_BCM54385_MODEL      0x1   /*Rev = 0*/


/* 
 * Model 0x17 is being used by 54880E now. BCM54585 
 * doesn't exist. So we need to remove support for this
 * device.
 */
#define PHY_BCM54585_OUI        PHY_BRCM_OUI3
#define PHY_BCM54585_MODEL      0xFF

#define PHY_BCM5927_OUI         PHY_BRCM_OUI3
#define PHY_BCM5927_MODEL       0x39

#define PHY_BCM8729_OUI         PHY_BRCM_OUI1
#define PHY_BCM8729_MODEL       0x3

#define PHY_BCM84756_OUI        PHY_BRCM_OUI5
#define PHY_BCM84756_MODEL      0x27

#define PHY_BCM84749_OUI        PHY_BRCM_OUI5
#define PHY_BCM84749_MODEL      0x30

#define PHY_BCM84729_OUI        PHY_BRCM_OUI5
#define PHY_BCM84729_MODEL      PHY_BCM84728_MODEL

#define PHY_BCM84334_OUI        PHY_BRCM_OUI3
#define PHY_BCM84334_MODEL      0x03

#define PHY_BCM84336_OUI        PHY_BRCM_OUI3
#define PHY_BCM84336_MODEL      0x0a

#define PHY_BCM84333_OUI        PHY_BRCM_OUI3
#define PHY_BCM84333_MODEL      0x0b


#endif /* INCLUDE_MACSEC  || INCLUDE_FCMAP */

#define PHY_BCM5418X_OUI        PHY_BRCM_OUI6
#define PHY_BCM5419X_OUI        PHY_BRCM_OUI6
#define PHY_BCM542XX_OUI        PHY_BRCM_OUI5

#define PHY_BCM54182_OUI        PHY_BRCM_OUI6
#define PHY_BCM54182_MODEL      0x01

#define PHY_BCM54185_OUI        PHY_BRCM_OUI6
#define PHY_BCM54185_MODEL      0x01 /*Rev = 8*/

#define PHY_BCM54180_OUI        PHY_BRCM_OUI6
#define PHY_BCM54180_MODEL      0x0

#define PHY_BCM54140_OUI        PHY_BRCM_OUI6
#define PHY_BCM54140_MODEL      0x0  /*Rev = 8*/

#define PHY_BCM54192_OUI        PHY_BRCM_OUI6
#define PHY_BCM54192_MODEL      0x11

#define PHY_BCM54195_OUI        PHY_BRCM_OUI6
#define PHY_BCM54195_MODEL      0x11 /*Rev = 8*/

#define PHY_BCM54190_OUI        PHY_BRCM_OUI6
#define PHY_BCM54190_MODEL      0x10

#define PHY_BCM54194_OUI        PHY_BRCM_OUI6
#define PHY_BCM54194_MODEL      0x10  /*Rev = 8*/

#define PHY_BCM54210_OUI        PHY_BRCM_OUI5
#define PHY_BCM54210_MODEL      0x19

#define PHY_BCM54220_OUI        PHY_BRCM_OUI5
#define PHY_BCM54220_MODEL      0x18

#define PHY_BCM54240_OUI        PHY_BRCM_OUI5
#define PHY_BCM54240_MODEL      0x06  /*Rev = 1*/

#define PHY_BCM54280_OUI        PHY_BRCM_OUI5
#define PHY_BCM54280_MODEL      0x04  /*Rev = 1*/

#define PHY_BCM54282_OUI        PHY_BRCM_OUI5
#define PHY_BCM54282_MODEL      0x05  /*Rev = 9*/

#define PHY_BCM54285_OUI        PHY_BRCM_OUI5
#define PHY_BCM54285_MODEL      0x05  /*Rev = 1*/

#define PHY_BCM5428X_OUI        PHY_BRCM_OUI5
#define PHY_BCM5428X_MODEL      0x1b  /*Rev = 1*/

#define PHY_BCM54290_OUI        PHY_BRCM_OUI5
#define PHY_BCM54290_MODEL      0x12

#define PHY_BCM54292_OUI        PHY_BRCM_OUI5
#define PHY_BCM54292_MODEL      0x13

#define PHY_BCM54294_OUI        PHY_BRCM_OUI5
#define PHY_BCM54294_MODEL      0x12

#define PHY_BCM54295_OUI        PHY_BRCM_OUI5
#define PHY_BCM54295_MODEL      0x13

#define PHY_BCM54296_OUI        PHY_BRCM_OUI5
#define PHY_BCM54296_MODEL      0x13

#define PHY_BCM54980_OUI        PHY_BRCM_OUI2
#define PHY_BCM54980_MODEL      0x28

#define PHY_BCM54980C_OUI       PHY_BRCM_OUI2
#define PHY_BCM54980C_MODEL     0x29

#define PHY_BCM54980V_OUI       PHY_BRCM_OUI2
#define PHY_BCM54980V_MODEL     0x2a

#define PHY_BCM54980VC_OUI      PHY_BRCM_OUI2
#define PHY_BCM54980VC_MODEL    0x2b

#define PHY_BCM8011_OUI         PHY_BRCM_OUI1R
#define PHY_BCM8011_MODEL       0x09

#define PHY_BCM8703_OUI         PHY_BRCM_OUI1
#define PHY_BCM8703_MODEL       0x03

#define PHY_BCM8704_OUI         PHY_BRCM_OUI1
#define PHY_BCM8704_MODEL       0x03

#define PHY_BCM8705_OUI         PHY_BRCM_OUI1
#define PHY_BCM8705_MODEL       0x03

#define PHY_BCM8706_OUI         PHY_BRCM_OUI1
#define PHY_BCM8706_MODEL       0x03

#define PHY_BCM8727_OUI         PHY_BRCM_OUI1
#define PHY_BCM8727_MODEL       0x03

#define PHY_BCM8747_OUI         PHY_BRCM_OUI1
#define PHY_BCM8747_MODEL       0x03

#define PHY_BCM8072_OUI         PHY_BRCM_OUI1
#define PHY_BCM8072_MODEL       0x03

#define PHY_BCM8073_OUI         PHY_BRCM_OUI1
#define PHY_BCM8073_MODEL       0x03

#define PHY_BCM8074_OUI         PHY_BRCM_OUI1
#define PHY_BCM8074_MODEL       0x03

#define PHY_BCM8040_OUI         PHY_BRCM_OUI1
#define PHY_BCM8040_MODEL       0x3c

#define PHY_BCM8481X_OUI        PHY_BRCM_OUI3
#define PHY_BCM8481X_MODEL      0x00

#define PHY_BCM84812CE_OUI      PHY_BRCM_OUI3
#define PHY_BCM84812CE_MODEL    0x01

#define PHY_BCM8482X_OUI        PHY_BRCM_OUI3
#define PHY_BCM84821_OUI        PHY_BRCM_OUI3
#define PHY_BCM84821_MODEL      0x04

#define PHY_BCM84822_OUI        PHY_BRCM_OUI3
#define PHY_BCM84822_MODEL      0x05

#define PHY_BCM84823_OUI        PHY_BRCM_OUI3
#define PHY_BCM84823_MODEL      0x06

#define PHY_BCM8483X_OUI        PHY_BRCM_OUI3
#define PHY_BCM84833_OUI        PHY_BRCM_OUI3
#define PHY_BCM84833_MODEL      0x0b

#define PHY_BCM84834_OUI        PHY_BRCM_OUI3
#define PHY_BCM84834_MODEL      0x03

#define PHY_BCM84835_OUI        PHY_BRCM_OUI3
#define PHY_BCM84835_MODEL      0x07

#define PHY_BCM84836_OUI        PHY_BRCM_OUI3
#define PHY_BCM84836_MODEL      0x0a

#define PHY_BCM8484X_OUI        PHY_BRCM_OUI5
#define PHY_BCM84844_OUI        PHY_BRCM_OUI5
#define PHY_BCM84844_MODEL      0x0D

#define PHY_BCM84846_OUI        PHY_BRCM_OUI5
#define PHY_BCM84846_MODEL      0x0C

#define PHY_BCM84848_OUI        PHY_BRCM_OUI5
#define PHY_BCM84848_MODEL      0x0F

#define PHY_BCM8485X_OUI        PHY_BRCM_OUI5
#define PHY_BCM84858_OUI        PHY_BRCM_OUI5
#define PHY_BCM84858_MODEL      0x16

#define PHY_BCM84856_OUI        PHY_BRCM_OUI5
#define PHY_BCM84856_MODEL      0x15

#define PHY_BCM8486X_OUI        PHY_BRCM_OUI6
#define PHY_BCM84860_OUI        PHY_BRCM_OUI6
#define PHY_BCM84860_MODEL      0x4

#define PHY_BCM84861_OUI        PHY_BRCM_OUI6
#define PHY_BCM84861_MODEL      0x4

#define PHY_BCM84864_OUI        PHY_BRCM_OUI6
#define PHY_BCM84864_MODEL      0x6

#define PHY_BCM84868_OUI        PHY_BRCM_OUI6
#define PHY_BCM84868_MODEL      0x6

#define PHY_BCM8488X_OUI        PHY_BRCM_OUI6
#define PHY_BCM84888_OUI        PHY_BRCM_OUI6
#define PHY_BCM84888_MODEL      0x14

#define PHY_BCM84884_OUI        PHY_BRCM_OUI6
#define PHY_BCM84884_MODEL      0x14

#define PHY_BCM84888E_OUI       PHY_BRCM_OUI6
#define PHY_BCM84888E_MODEL     0x16

#define PHY_BCM84884E_OUI       PHY_BRCM_OUI6
#define PHY_BCM84884E_MODEL     0x16

#define PHY_BCM84881_OUI        PHY_BRCM_OUI6
#define PHY_BCM84881_MODEL      0x15

#define PHY_BCM84880_OUI        PHY_BRCM_OUI6
#define PHY_BCM84880_MODEL      0x15

#define PHY_BCM84888S_OUI       PHY_BRCM_OUI6
#define PHY_BCM84888S_MODEL     0x17

#define PHY_BCM84885_OUI        PHY_BRCM_OUI6
#define PHY_BCM84885_MODEL      0x17

#define PHY_BCM84886_OUI        PHY_BRCM_OUI6
#define PHY_BCM84886_MODEL      0x17

#define PHY_BCM84887_OUI        PHY_BRCM_OUI6
#define PHY_BCM84887_MODEL      0x14

#define PHY_BCM8750_OUI         PHY_BRCM_OUI3
#define PHY_BCM8750_MODEL       0x39

#define PHY_BCM8752_OUI         PHY_BRCM_OUI3
#define PHY_BCM8752_MODEL       0x39

#define PHY_BCM8754_OUI         PHY_BRCM_OUI3
#define PHY_BCM8754_MODEL       0x3A

#define PHY_BCM84740_OUI        PHY_BRCM_OUI3
#define PHY_BCM84740_MODEL      0x3D

#define PHY_BCM84793_OUI        PHY_BRCM_OUI5
#define PHY_BCM84793_MODEL      0x39

#define PHY_BCM84164_OUI        PHY_BRCM_OUI5
#define PHY_BCM84164_MODEL      0x2E

#define PHY_BCM84758_OUI        PHY_BRCM_OUI5
#define PHY_BCM84758_MODEL      0x2F

#define PHY_BCM84780_OUI        PHY_BRCM_OUI5
#define PHY_BCM84780_MODEL      0x2F

#define PHY_BCM84784_OUI        PHY_BRCM_OUI5
#define PHY_BCM84784_MODEL      0x2F

#define PHY_BCM84328_OUI        PHY_BRCM_OUI5
#define PHY_BCM84328_MODEL      0x10

#define PHY_BCMXGXS1_OUI        PHY_BRCM_OUI1
#define PHY_BCMXGXS1_MODEL      0x3f

#define PHY_BCMXGXS2_OUI        PHY_BRCM_OUI1
#define PHY_BCMXGXS2_MODEL      0x3a

#define PHY_AL101_OUI           PHY_ALTIMA_OUI
#define PHY_AL101_MODEL         0x21

#define PHY_AL101L_OUI          PHY_ALTIMA_OUI
#define PHY_AL101L_MODEL        0x12

#define PHY_AL104_OUI           PHY_ALTIMA_OUI
#define PHY_AL104_MODEL         0x14

#define PHY_BCMXGXS5_OUI        PHY_BRCM_OUI2
#define PHY_BCMXGXS5_MODEL      0x19

#define PHY_BCMXGXS6_OUI        PHY_BRCM_OUI2
#define PHY_BCMXGXS6_MODEL      0x1c

#define PHY_XGXS_HL65_OUI       PHY_BRCM_OUI2
#define PHY_XGXS_HL65_MODEL     PHY_BCMXGXS1_MODEL 

#define PHY_SERDES100FX_OUI     PHY_BRCM_OUI2
#define PHY_SERDES100FX_MODEL   0x18

#define PHY_SERDES65LP_OUI      PHY_BRCM_OUI2
#define PHY_SERDES65LP_MODEL    0x3f

#define PHY_SERDESCOMBO_OUI     PHY_BRCM_OUI2
#define PHY_SERDESCOMBO_MODEL   0x15

#define PHY_BCM53314_OUI         PHY_BRCM_OUI2
#define PHY_BCM53314_MODEL       0x12

#define PHY_BCM53324_OUI         PHY_BRCM_OUI3
#define PHY_BCM53324_MODEL       0x12

#define PHY_XGXS_16G_OUI         PHY_BRCM_OUI2
#define PHY_XGXS_16G_MODEL       0x3f

#define PHY_XGXS_TSC_OUI         PHY_BRCM_OUI5
#define PHY_XGXS_TSC_MODEL       0x37

#define PHY_BCM82328_OUI         PHY_BRCM_OUI6 
#define PHY_BCM82328_MODEL       0x21

#define PHY_BCM84318_OUI         PHY_BRCM_OUI5 
#define PHY_BCM84318_MODEL       0x2f

#define PHY_XGXS_VIPER_OUI       PHY_BRCM_OUI2
#define PHY_XGXS_VIPER_MODEL     0x3f

#define PHY_BCM82381_OUI         PHY_BRCM_OUI6 
#define PHY_BCM82381_MODEL       0x23

#define PHY_BCM82764_OUI         PHY_BRCM_OUI6
#define PHY_BCM82764_MODEL       0x29

#define PHY_BCM82864_OUI         PHY_BRCM_OUI6
#define PHY_BCM82864_MODEL       0x2b

#define PHY_BCM82780_OUI         PHY_BRCM_OUI6 
#define PHY_BCM82780_MODEL       0x25

#define PHY_BCM56160GPHY_OUI     PHY_BRCM_OUI6
#define PHY_BCM56160GPHY_MODEL   0x2d

#define PHY_BCM53540_OUI        PHY_BRCM_OUI6
#define PHY_BCM53540_MODEL      0x1d

#define PHY_BCM56275GPHY_OUI     PHY_BRCM_OUI8
#define PHY_BCM56275GPHY_MODEL   0x20

#define PHY_BCM8806X_OUI         PHY_BRCM_OUI6
#define PHY_BCM8806X_MODEL       0x2C

#define PHY_REVISION_CHECK(_pc, _model, _oui, _rev) \
                                (((_pc)->phy_model == (_model)) && \
                                 ((_pc)->phy_oui == (_oui)) && \
                                 ((_pc)->phy_rev == (_rev)))
#define PHY_MODEL_CHECK(_pc, _oui, _model) \
                                (((_pc)->phy_oui == (_oui)) && \
                                 ((_pc)->phy_model == (_model)))
#define PHY_IS_BCM5218(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5218_OUI, \
                                                PHY_BCM5218_MODEL)
#define PHY_IS_BCM5220(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5220_OUI, \
                                                PHY_BCM5220_MODEL)
#define PHY_IS_BCM5226(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5226_OUI, \
                                                PHY_BCM5226_MODEL)
#define PHY_IS_BCM5228(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5228_OUI, \
                                                PHY_BCM5228_MODEL)
#define PHY_IS_BCM5238(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5238_OUI,  \
                                                PHY_BCM5238_MODEL)
#define PHY_IS_BCM5248(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5248_OUI,  \
                                                PHY_BCM5248_MODEL)
#define PHY_IS_BCM5400(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5400_OUI,  \
                                                PHY_BCM5400_MODEL)
#define PHY_IS_BCM5401(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5401_OUI,  \
                                                PHY_BCM5401_MODEL)
#define PHY_IS_BCM5402(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5402_OUI,  \
                                                PHY_BCM5402_MODEL)
#define PHY_IS_BCM5404(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5404_OUI,  \
                                                PHY_BCM5404_MODEL)
#define PHY_IS_BCM5411(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5411_OUI,  \
                                                PHY_BCM5411_MODEL)
#define PHY_IS_BCM5421(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5421_OUI,  \
                                                PHY_BCM5421_MODEL)
#define PHY_IS_BCM5424(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5424_OUI,  \
                                                PHY_BCM5424_MODEL)
#define PHY_IS_BCM5464(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5464_OUI,  \
                                                PHY_BCM5464_MODEL)
#define PHY_IS_BCM5466(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5466_OUI,  \
                                                PHY_BCM5466_MODEL)
#define PHY_IS_BCM5461(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5461_OUI,  \
                                                PHY_BCM5461_MODEL)
#define PHY_IS_BCM5462(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5462_OUI,  \
                                                PHY_BCM5462_MODEL)
#define PHY_IS_BCM5478(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5478_OUI,  \
                                                PHY_BCM5478_MODEL)
#define PHY_IS_BCM5488(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5488_OUI,  \
                                                PHY_BCM5488_MODEL)
#define PHY_IS_BCM5482(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5482_OUI,  \
                                                PHY_BCM5482_MODEL)
#define PHY_IS_BCM5481(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM5481_OUI,  \
                                                PHY_BCM5481_MODEL)  
#define PHY_IS_BCM54980(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54980_OUI, \
                                                PHY_BCM54980_MODEL)
#define PHY_IS_BCM54980C(_pc)   PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54980C_OUI, \
                                                PHY_BCM54980C_MODEL)
#define PHY_IS_BCM54980V(_pc)   PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54980V_OUI, \
                                                PHY_BCM54980V_MODEL)
#define PHY_IS_BCM54980VC(_pc)  PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54980VC_OUI, \
                                                PHY_BCM54980VC_MODEL)
#define PHY_IS_BCM54684(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54684_OUI,  \
                                                PHY_BCM54684_MODEL)
#define PHY_IS_BCM54682(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54682E_OUI,  \
                                                PHY_BCM54682E_MODEL)
#define PHY_IS_BCM54640(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM54640_OUI,  \
                                                PHY_BCM54640_MODEL)
#define PHY_IS_BCM53314(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM53314_OUI,  \
                                                PHY_BCM53314_MODEL)
#define PHY_IS_BCM53324(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM53324_OUI,  \
                                                PHY_BCM53324_MODEL)
#define PHY_IS_BCM8011(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM8011_OUI,  \
                                                PHY_BCM8011_MODEL)
#define PHY_IS_BCM8703(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM8703_OUI,  \
                                                PHY_BCM8703_MODEL)
#define PHY_IS_BCM8704(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM8704_OUI,  \
                                                PHY_BCM8704_MODEL)
#define PHY_IS_BCM8705(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM8705_OUI,  \
                                                PHY_BCM8705_MODEL)
#define PHY_IS_BCMXGXS1(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCMXGXS1_OUI, \
                                                PHY_BCMXGXS1_MODEL)
#define PHY_IS_BCMXGXS2(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCMXGXS2_OUI, \
                                                PHY_BCMXGXS2_MODEL)
#define PHY_IS_BCMXGXS5(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCMXGXS5_OUI, \
                                                PHY_BCMXGXS5_MODEL)
#define PHY_IS_BCMXGXS6(_pc)    PHY_MODEL_CHECK((_pc), \
                                                PHY_BCMXGXS6_OUI, \
                                                PHY_BCMXGXS6_MODEL)
#ifdef INCLUDE_PHY_522X
#define PHY_IS_BCM5324(_pc)     0
#define PHY_IS_BCM5324A1(_pc)   0
#define PHY_IS_BCM5348(_pc)     0
#define PHY_IS_BCM53242(_pc)    0
#define PHY_IS_BCM53262(_pc)    0
#define PHY_IS_BCM53280(_pc)    0
#define PHY_IS_BCM53280_A0(_pc) 0
#endif
#define PHY_IS_BCM56160GPHY(_pc) PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM56160GPHY_OUI, \
                                                PHY_BCM56160GPHY_MODEL)

#define PHY_IS_BCM8806X(_pc)     PHY_MODEL_CHECK((_pc), \
                                                PHY_BCM8806X_OUI, \
                                                PHY_BCM8806X_MODEL)

#define PHY_IS_SERDES100FX(_pc) PHY_MODEL_CHECK((_pc), \
                                                PHY_SERDES100FX_OUI, \
                                                PHY_SERDES100FX_MODEL)
#define PHY_IS_SERDES65LP(_pc)  PHY_MODEL_CHECK((_pc), \
                                                PHY_SERDES65LP_OUI, \
                                                PHY_SERDES65LP_MODEL)
#define PHY_IS_SERDESCOMBO(_pc) PHY_MODEL_CHECK((_pc), \
                                                PHY_SERDESCOMBO_OUI, \
                                                PHY_SERDESCOMBO_MODEL)
#define PHY_IS_SERDESCOMBO65(_pc) PHY_MODEL_CHECK((_pc), \
                                                PHY_SERDESCOMBO65_OUI, \
                                                PHY_SERDESCOMBO65_MODEL)

/* PHY drivers */
extern phy_driver_t phy_522xdrv_fe;
extern phy_driver_t phy_5421Sdrv_ge;
extern phy_driver_t phy_5464drv_ge;
extern phy_driver_t phy_5482drv_ge;
extern phy_driver_t phy_54684drv_ge;
extern phy_driver_t phy_54682drv_ge;
extern phy_driver_t phy_54640drv_ge;
extern phy_driver_t phy_54580drv_ge;
extern phy_driver_t phy_54380drv_ge;
extern phy_driver_t phy_542xxdrv_ge;
extern phy_driver_t phy_5401drv_ge;
extern phy_driver_t phy_5402drv_ge;
extern phy_driver_t phy_5404drv_ge;
extern phy_driver_t phy_5411drv_ge;
extern phy_driver_t phy_5424drv_ge;
extern phy_driver_t phy_5690drv_ge;
extern phy_driver_t phy_56xxxdrv_ge;
extern phy_driver_t phy_56xxx_5601x_drv_ge;
extern phy_driver_t phy_53xxxdrv_ge;
extern phy_driver_t phy_8703drv_xe;
extern phy_driver_t phy_8705drv_xe;
extern phy_driver_t phy_8706drv_xe;
extern phy_driver_t phy_8072drv_xe;
extern phy_driver_t phy_8074drv_xe;
extern phy_driver_t phy_drv_fe;
extern phy_driver_t phy_drv_ge;
extern phy_driver_t phy_null;
extern phy_driver_t phy_nocxn;
extern phy_driver_t phy_simul;
extern phy_driver_t phy_serdes100fx_ge;
extern phy_driver_t phy_serdes65lp_ge;
extern phy_driver_t phy_qsgmii65_ge;
extern phy_driver_t phy_serdescombo_ge;
extern phy_driver_t phy_serdescombo_5601x_ge;
extern phy_driver_t phy_serdescombo65_ge;
extern phy_driver_t phy_serdescombo65_5602x_ge;
extern phy_driver_t phy_xgxs1_hg;  
extern phy_driver_t phy_xgxs5_hg;  
extern phy_driver_t phy_xgxs6_hg;  
extern phy_driver_t phy_hl65_hg;
extern phy_driver_t phy_hl65_l0_hg;
extern phy_driver_t phy_hl65_l1_hg;
extern phy_driver_t phy_hl65_l2_hg;
extern phy_driver_t phy_hl65_l3_hg;
extern phy_driver_t phy_xgxs16g_hg;
extern phy_driver_t phy_xgxs16g1l_ge;
extern phy_driver_t phy_8040drv_xe;
extern phy_driver_t phy_8481drv_xe;
extern phy_driver_t phy_8750drv_xe;
extern phy_driver_t phy_54680drv_ge;
extern phy_driver_t phy_54880drv_ge;
extern phy_driver_t phy_copper_sfp_drv;
extern phy_driver_t phy_8729drv_gexe;
extern phy_driver_t phy_wc40_hg;
extern phy_driver_t phy_wcmod_hg;
extern phy_driver_t phy_tscmod_hg;
extern phy_driver_t phy_tsce_drv;
extern phy_driver_t phy_tscf_drv;
extern phy_driver_t phy_qsgmiie_drv;
extern phy_driver_t phy_qtce_drv;
extern phy_driver_t phy_54616drv_ge;
extern phy_driver_t phy_84740drv_xe;
extern phy_driver_t phy_84793drv_ce;
extern phy_driver_t phy_84756drv_xe;
extern phy_driver_t phy_84756drv_fcmap_xe;
extern phy_driver_t phy_84749drv_xe;
extern phy_driver_t phy_84728drv_xe;
extern phy_driver_t phy_84334drv_xe;
extern phy_driver_t phy_84328drv_xe;
extern phy_driver_t phy_82328drv_xe;
extern phy_driver_t phy_viper_drv;
extern phy_driver_t phy82381_drv;
extern phy_driver_t phy82109_drv;
extern phy_driver_t phy82764_drv;
extern phy_driver_t phy82864_drv;
extern phy_driver_t phy_8806xdrv;
#ifdef PORTMOD_SUPPORT
extern phy_driver_t phy_ext_to_int_drv;
#endif
extern phy_driver_t phy82780_drv;
extern phy_driver_t phy_egphy28drv_ge;
extern phy_driver_t phy_egphy16drv_ge;

/* Function:
 *    (*soc_phy_ident_f)
 *
 * Purpose:
 *    call back for phy identification
 *
 * Parameters:
 *    int unit, soc_port_t port,         Unit and port being checked.
 *    soc_phy_table_t *my_entry,         Pointer to calling structure.
 *    uint16 phy_id0, uint16 phy_id1     Values of phy regs read
 *
 * Side effects:  Sets the phy_info structure for this port.
 */

#endif /* _PHY_IDENT_H */
