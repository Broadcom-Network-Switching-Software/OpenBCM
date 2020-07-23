/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * zm73xx.h    Digital Power Monitor
 *
 * Driver Supports PowerOne zm7304, zm7308, zm7316 & zm7332
 *
 * <http://www.power-one.com>
 *
 * This driver's initial intent is use on the Broadcom MetroCore Chassis boards.
 */

/*
 * The ZM7300 series chips have several banks of registers,
 * user memory, Point Of Load settings, and can monitor the state
 * of the PoLs.
 *
 * The command set is laid out below
 */
#if !defined(_ZM73XX_H_)
#define _ZM73XX_H_

#define ZM73_CMD_WRP    (0x01)  /* 6 write 1 read -- BB mode  */
#define ZM73_CMD_RRP    (0x11)  /* 2 write 2 read -- BB mode  */
#define ZM73_CMD_WRM    (0x02)  /* 2 write 1 read -- BB mode  */
#define ZM73_CMD_RRM    (0x12)  /* 1 write 6 read -- Ext Mode */
#define ZM73_CMD_SRM    (0x34)  /* 2 write 1 read -- BB mode  */
#define ZM73_CMD_RPID   (0x16)  /* 0 write 33 read -- BB mode */
#define ZM73_CMD_LSR    (0x32)  /* 1 write 1 read -- Ext Mode */
#define ZM73_CMD_WSR    (0x05)  /* 2 write 1 read -- BB mode  */
#define ZM73_CMD_RSR    (0x15)  /* 3 write 1 read -- BB mode  */
#define ZM73_CMD_SSR    (0x33)  /* 1 write 1 read -- Ext Mode */
#define ZM73_CMD_LUM    (0x35)  /* 1 write 1 read -- Ext Mode */
#define ZM73_CMD_WUM    (0x36)  /* 2 write 1 read -- BB mode  */
#define ZM73_CMD_RUM    (0x38)  /* 2 write 2 read -- BB mode  */
#define ZM73_CMD_SUM    (0x37)  /* 1 write 1 read -- Ext Mode */
#define ZM73_CMD_RSM    (0x40)  /* 0 write 7 read -- BB mode  */
#define ZM73_CMD_RPM    (0x41)  /* 0 write 17 read -- BB mode */
#define ZM73_CMD_RMDP   (0x45)  /* 1 write 6 read -- Ext Mode */
#define ZM73_CMD_RPSS   (0x44)  /* 0 write 5 read -- Ext Mode */
#define ZM73_CMD_RRT    (0x42)  /* 0 write 5 read -- Ext Mode */
#define ZM73_CMD_ONA    (0x20)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_ONB    (0x21)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_ONC    (0x22)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_OND    (0x23)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_ONS    (0x24)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_OFA    (0x25)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_OFB    (0x26)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_OFC    (0x27)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_OFD    (0x28)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_OFS    (0x29)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_EOFA   (0x2A)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_EOFB   (0x2B)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_EOFC   (0x2C)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_EOFD   (0x2D)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_EOFS   (0x2E)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_MAR    (0x39)  /* 1 write 1 read -- Ext Mode */
#define ZM73_CMD_FON    (0x3A)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_FOF    (0x3B)  /* Normal BYTE_WRITE_DATA     */
#define ZM73_CMD_CRC    (0x2F)  /* 4 write 1 read -- BB mode  */
#define ZM73_CMD_PRGP   (0x31)  /* 4 write 1 read -- BB mode  */

/*
 * DPM Setup Registers
 */

#define ZM73XX_DPM_REG_GD1  (0x00)
#define ZM73XX_DPM_REG_GD2  (0x04)
#define ZM73XX_DPM_REG_GAC  (0x08)
#define ZM73XX_DPM_REG_GBC  (0x09)
#define ZM73XX_DPM_REG_GCC  (0x0A)
#define ZM73XX_DPM_REG_GDC  (0x0B)
#define ZM73XX_DPM_REG_FPC1 (0x0C)
#define ZM73XX_DPM_REG_FPC2 (0x0D)
#define ZM73XX_DPM_REG_EPC  (0x0E)
#define ZM73XX_DPM_REG_IC1  (0x0F)
#define ZM73XX_DPM_REG_IC2  (0x10)
#define ZM73XX_DPM_REG_IBL  (0x11)
#define ZM73XX_DPM_REG_IBH  (0x13)
#define ZM73XX_DPM_REG_ID   (0x15)
#define ZM73XX_DPM_REG_PB1  (0x17)
#define ZM73XX_DPM_REG_PB2  (0x1B)
#define ZM73XX_DPM_REG_PB3  (0x1F)
#define ZM73XX_DPM_REG_PB4  (0x23)
#define ZM73XX_DPM_REG_PMC  (0x27)
#define ZM73XX_DPM_REG_PID  (0x28)
#define ZM73XX_DPM_REG_RTC  (0x80)
#define ZM73XX_DPM_REG_PPS  (0x84)
#define ZM73XX_DPM_REG_EST  (0x88)
#define ZM73XX_DPM_REG_IBV  (0x89)
#define ZM73XX_DPM_REG_STA  (0x8B)
#define ZM73XX_DPM_REG_STB  (0x8C)
#define ZM73XX_DPM_REG_STC  (0x8D)
#define ZM73XX_DPM_REG_STD  (0x8E)
#define ZM73XX_DPM_REG_REL0 (0x8F)
#define ZM73XX_DPM_REG_REL1 (0x90)
#define ZM73XX_DPM_REG_PSS  (0x91)
#define ZM73XX_DPM_REG_DPMS (0x95)
#define ZM73XX_DPM_REG_WP   (0x96)

#define NUM_DPM_REGS	0x97


/*
 * Group Status Register Bits
 */
#define ZM73XX_GRP_ST_PT    (0x80)  /* Pre-Warning Temperature Warning */
#define ZM73XX_GRP_ST_PG    (0x40)  /* Power Good Warning */
#define ZM73XX_GRP_ST_TR    (0x20)  /* Tracking Fault */
#define ZM73XX_GRP_ST_OT    (0x10)  /* Temperature Fault */
#define ZM73XX_GRP_ST_OC    (0x08)  /* OverCurrent Fault */
#define ZM73XX_GRP_ST_UV    (0x04)  /* UnderVoltage Fault */
#define ZM73XX_GRP_ST_OV    (0x02)  /* Overvoltage Error */
#define ZM73XX_GRP_ST_PV    (0x01)  /* Phase Voltage Error */


/*
 * POL Setup Registers
 */
#define ZM73XX_POL_REG_PC1	(0x00)
#define ZM73XX_POL_REG_PC2	(0x01)
#define ZM73XX_POL_REG_PC3	(0x02)
#define ZM73XX_POL_REG_TC	(0x03)
#define ZM73XX_POL_REG_INT	(0x04)
#define ZM73XX_POL_REG_DON	(0x05)
#define ZM73XX_POL_REG_DOF	(0x06)
#define ZM73XX_POL_REG_VOS	(0x07)
#define ZM73XX_POL_REG_CLS	(0x08)
#define ZM73XX_POL_REG_DCL	(0x09)
#define ZM73XX_POL_REG_B1	(0x0A)
#define ZM73XX_POL_REG_B2	(0x0B)
#define ZM73XX_POL_REG_B3	(0x0C)
#define ZM73XX_POL_REG_C0L	(0x0D)
#define ZM73XX_POL_REG_C0H	(0x0E)
#define ZM73XX_POL_REG_C1L	(0x0F)
#define ZM73XX_POL_REG_C1H	(0x10)
#define ZM73XX_POL_REG_C2L	(0x11)
#define ZM73XX_POL_REG_C2H	(0x12)
#define ZM73XX_POL_REG_C3L	(0x13)
#define ZM73XX_POL_REG_C3H	(0x14)
#define ZM73XX_POL_REG_RUN	(0x15)
#define ZM73XX_POL_REG_ST	(0x16)
#define ZM73XX_POL_REG_VOM	(0x17)
#define ZM73XX_POL_REG_IOM	(0x18)
#define ZM73XX_POL_REG_TOM	(0x19)
#define ZM73XX_POL_REG_VOML	(0x1C)
#define ZM73XX_POL_REG_VOMH	(0x1D)
#define ZM73XX_POL_REG_CRC0	(0x1E)
#define ZM73XX_POL_REG_CRC1	(0x1F)

#define NUM_POL_REGS		(0x20)

/*
 * Auxillary Device Setup Registers
 */
#define ZM73XX_AUX_REG_EC      (0x00)
#define ZM73XX_AUX_REG_EON     (0x05)
#define ZM73XX_AUX_REG_EOF     (0x06)
#define ZM73XX_AUX_REG_CRC0_x  (0x1E)
#define ZM73XX_AUX_REG_CRC1_x  (0x1F)

/*
 * POL/Aux Setup registers and data
 * Note: Aux does not use all registers
 */
#define ZM73XX_POL_NONE (0x00)
#define ZM73XX_POL_ZY7005 (0x01)
#define ZM73XX_POL_ZY7105 (0x02)
#define ZM73XX_POL_ZY7007 (0x03)
#define ZM73XX_POL_ZY7107 (0x04)
#define ZM73XX_POL_ZY7010 (0x05)
#define ZM73XX_POL_ZY7210 (0x06)
#define ZM73XX_POL_ZY7015 (0x07)
#define ZM73XX_POL_ZY7115 (0x08)
#define ZM73XX_POL_ZY7020 (0x09)
#define ZM73XX_POL_ZY7120 (0x0A)
#define ZM73XX_POL_ZY7030 (0x0B)
#define ZM73XX_POL_ZY7130 (0x0C)
#define ZM73XX_POL_ZY7040 (0x0D)
#define ZM73XX_POL_ZY7140 (0x0E)
#define ZM73XX_POL_ZY7050 (0x0F)
#define ZM73XX_POL_ZY7150 (0x10)
#define ZM73XX_POL_AUX0 (0xF8)
#define ZM73XX_POL_AUX1 (0xF9)
#define ZM73XX_POL_AUX2 (0xFA)
#define ZM73XX_POL_AUX3 (0xFB)

#endif /* _ZM73XX_H_ */

