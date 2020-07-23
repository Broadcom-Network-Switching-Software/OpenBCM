/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for Cypress W229B/W311 Clock generator (PLL) chip.
 * The W229b is a highly integrated frequency timing generator,
 * supplying a variety of common clock sources.
 *
 * The W311 is a more robust version of the W229b which allows for
 * read capability and arbitrary SMB byte, word, or block operations.
 *
 * This driver supports both the W229b (for downward compatibility
 * with older 5605 baseboards) as well as the W311 chip. In order to
 * determine if we are running in W311 mode, we attempt to read the
 * W311 device and vendor Id from register offset 8; if this succeeds,
 * then we assume that we are communicating with a W311.
 *
 * The W229b clock frequency configuration is table driven, the W311
 * uses M and N values to set the clock frequency. In addition, the
 * W311 allows setting of the clock frequency and then later reading
 * it back.
 *
 * On Broadcom BCM95605 baseboards, the W229b clock chip is used to
 * supply the 5605/5606 baseboard with a core clock. On new Broadcom
 * BCM5605 baseboards and 48port (p48) boards, the W311 chip is used
 * instead.
 *
 * In W229b data sheet, the CPU clock is the switch core clock in
 * Table 4. The same holds true for the W311.
 *
 * For W229b, in the ioctl interface to this driver, if the clock
 * value supplied is in between the higher and lower value, we pick
 * the lower value.
 *
 * See the Cypress W229B/W311 Clock chip data sheet(s) for more details.
 */
#include <sal/types.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
/* W229B Command code : Always Zero */
#define W229B_COMM_CODE     0x00000000
#define W229B_N_REGS        7 /* Bytes 0 - 6 */

/*
 * Definitions for Control Byte 4: SEL bits for freq select
 * Return byte represent SEL values with FS override set (SEL selects
 * the operating frequency). Used for both W229/W311 clock chips.
 */
#define W229_SPEED(s4, s3, s2, s1, s0)\
( (s3 << 7) | (s2 << 6) | (s1 << 5)|(s0 << 4)|(1 << 3)|(s4 << 2) )

#define W311_SPEED(s4, s3, s2, s1, s0)\
  ( (s4 << 5) | (s3 << 4) | (s2 << 3)|(s1 << 2)|(s0 << 1)|s0)


/* W229b: Min Freq = 66Mhz, Max = 166Mhz */
#define W229B_MIN_SPEED   66000000
#define W229B_MAX_SPEED  166000000

/* W229b: Default Control/Reserved registers: Bytes0-3 */
#define W229B_R0    0x06
#define W229B_R1    0xff
#define W229B_R2    0xff
#define W229B_R3    0x08
#define W229B_CNT   5  /* 5 byte SMB block write */

/* Constants for conversions */
#define MHZ    1000*1000
#define Megahertz(x) ((x)*MHZ)

/* W311 Definitions */
#define W311_ID_REG 0x08
#define W311_CTRL_REG1 0x01
#define W311_N_REG  0X0d /* N Register */
#define W311_M_REG  0x0e /* M Register (and programmable freq enable)*/
#ifdef	COMPILER_HAS_DOUBLE
#define W311_GEAR_CONSTANT 48.00741
#else
#define W311_GEAR_CONSTANT 48007410
#endif
#define W311_M_LOWSPEED  93 /* 50-129 Mhz */
#define W311_M_HIGHSPEED 45 /* 130-248 Mhz */
#define W311_FS_REG  0x0f /* Reserved reg for latched FS_Sel bits */

/* Byte or Word Command Operationse */
#define W311_COMM_CODE(addr)    (0x80|(addr))

/* Min and Max clock output values */
#define W311_MIN_CLOCK     50
#define W311_MAX_CLOCK     258

/* Low scale Mhz range for LOWSPEED M value, in this range of
 * frequencies, clock output is tunable to 0.5Mhz resolution!
 * So, 126.5 is possible,  however 132.5 is not.
 */
#define W311_LOW_MIN       50
#define W311_LOW_MAX       129

/* Spread spectrum defines for W311 clock chip.
 * Bit definitions for Byte 1 (Control register 1)
 */
#define W311_SS_CLEAR            0x0f
#define W311_SS_TRISTATE         (0x30)
#define W311_SS_ENABLE_NEG5 	 (0x40)
#define W311_SS_ENABLE_5_5	 (0x50)
#define W311_SS_ENABLE_2_5	 (0x60)
#define W311_SS_ENABLE_3_8	 (0x70)

/* Useful macros */
#define W311_SS_SET_DISABLE(x)       ((x) & W311_SS_CLEAR)
#define W311_SS_SET_ENABLE_NEG5(x)   ((x) | W311_SS_ENABLE_NEG5) /* -0.50% */
#define W311_SS_SET_ENABLE_5_5(x)    ((x) | W311_SS_ENABLE_5_5) /* +/- 0.50% */
#define W311_SS_SET_ENABLE_2_5(x)    ((x) | W311_SS_ENABLE_2_5) /* +/- 0.25% */
#define W311_SS_SET_ENABLE_3_8(x)    ((x) | W311_SS_ENABLE_3_8) /* +/- 0.38% */

/* Decimal point formatting */
#define INT_FRAC_2PT_4(x) (x) / 1000000, ((x) / 10000) % 100

/* Whether we are in W311 mode or W229b mode */
static int is311 = FALSE;

/*
 * Data: w311_freq_tab
 * Purpose: Table 5 in W311 DataSheet.
 *          This table encodes the FS_Sel bits of the W311 clock speeds
 *          found in the pin strapping option which is read-only
 *          via reserved register 15 (0xf)
 * NOTE: This table is used for retrieving hardwired speeds, it does
 *       not need to be sorted by frequency.
 */
#ifdef	COMPILER_HAS_DOUBLE
static struct
w311_freq_tab_s{
    uint8 control;
    double speed; /* In Mhz */
} w311_freq_tab[] = {
    { W311_SPEED(0,0,0,0,0), 200.0},
    { W311_SPEED(0,0,0,0,1), 190.0},
    { W311_SPEED(0,0,0,1,0), 180.0},
    { W311_SPEED(0,0,0,1,1), 170.0},
    { W311_SPEED(0,0,1,0,0), 166.0},
    { W311_SPEED(0,0,1,0,1), 160.0},
    { W311_SPEED(0,0,1,1,0), 150.0},
    { W311_SPEED(0,0,1,1,1), 145.0},
    { W311_SPEED(0,1,0,0,0), 140.0},
    { W311_SPEED(0,1,0,0,1), 136.0},
    { W311_SPEED(0,1,0,1,0), 130.0},
    { W311_SPEED(0,1,0,1,1), 124.0},
    { W311_SPEED(0,1,1,0,0),  66.6},
    { W311_SPEED(0,1,1,0,1), 100.0},
    { W311_SPEED(0,1,1,1,0), 118.0},
    { W311_SPEED(0,1,1,1,1), 133.3},
    { W311_SPEED(1,0,0,0,0),  66.8},
    { W311_SPEED(1,0,0,0,1), 100.2},
    { W311_SPEED(1,0,0,1,0), 115.0},
    { W311_SPEED(1,0,0,1,1), 133.6},
    { W311_SPEED(1,0,1,0,0),  66.8},
    { W311_SPEED(1,0,1,0,1), 100.2},
    { W311_SPEED(1,0,1,1,0), 110.0},
    { W311_SPEED(1,0,1,1,1), 133.6},
    { W311_SPEED(1,1,0,0,0), 105.0},
    { W311_SPEED(1,1,0,0,1),  90.0},
    { W311_SPEED(1,1,0,1,0),  85.0},
    { W311_SPEED(1,1,0,1,1),  78.0},
    { W311_SPEED(1,1,1,0,0),  66.6},
    { W311_SPEED(1,1,1,0,1), 100.0},
    { W311_SPEED(1,1,1,1,0),  75.0},
    { W311_SPEED(1,1,1,1,1), 133.33},
};
#else
static struct
w311_freq_tab_s{
    uint8 control;
    int speed; /* In Hz */
} w311_freq_tab[] = {
    { W311_SPEED(0,0,0,0,0), 200000000}, /* 200.00 MHz */
    { W311_SPEED(0,0,0,0,1), 190000000}, /* 190.00 MHz */
    { W311_SPEED(0,0,0,1,0), 180000000}, /* 180.00 MHz */
    { W311_SPEED(0,0,0,1,1), 170000000}, /* 170.00 MHz */
    { W311_SPEED(0,0,1,0,0), 166000000}, /* 166.00 MHz */
    { W311_SPEED(0,0,1,0,1), 160000000}, /* 160.00 MHz */
    { W311_SPEED(0,0,1,1,0), 150000000}, /* 150.00 MHz */
    { W311_SPEED(0,0,1,1,1), 145000000}, /* 145.00 MHz */
    { W311_SPEED(0,1,0,0,0), 140000000}, /* 140.00 MHz */
    { W311_SPEED(0,1,0,0,1), 136000000}, /* 136.00 MHz */
    { W311_SPEED(0,1,0,1,0), 130000000}, /* 130.00 MHz */
    { W311_SPEED(0,1,0,1,1), 124000000}, /* 124.00 MHz */
    { W311_SPEED(0,1,1,0,0),  66600000}, /*  66.60 MHz */
    { W311_SPEED(0,1,1,0,1), 100000000}, /* 100.00 MHz */
    { W311_SPEED(0,1,1,1,0), 118000000}, /* 118.00 MHz */
    { W311_SPEED(0,1,1,1,1), 133300000}, /* 133.30 MHz */
    { W311_SPEED(1,0,0,0,0),  66800000}, /*  66.80 MHz */
    { W311_SPEED(1,0,0,0,1), 100200000}, /* 100.20 MHz */
    { W311_SPEED(1,0,0,1,0), 115000000}, /* 115.00 MHz */
    { W311_SPEED(1,0,0,1,1), 133600000}, /* 133.60 MHz */
    { W311_SPEED(1,0,1,0,0),  66800000}, /*  66.80 MHz */
    { W311_SPEED(1,0,1,0,1), 100200000}, /* 100.20 MHz */
    { W311_SPEED(1,0,1,1,0), 110000000}, /* 110.00 MHz */
    { W311_SPEED(1,0,1,1,1), 133600000}, /* 133.60 MHz */
    { W311_SPEED(1,1,0,0,0), 105000000}, /* 105.00 MHz */
    { W311_SPEED(1,1,0,0,1),  90000000}, /*  90.00 MHz */
    { W311_SPEED(1,1,0,1,0),  85000000}, /*  85.00 MHz */
    { W311_SPEED(1,1,0,1,1),  78000000}, /*  78.00 MHz */
    { W311_SPEED(1,1,1,0,0),  66600000}, /*  66.60 MHz */
    { W311_SPEED(1,1,1,0,1), 100000000}, /* 100.00 MHz */
    { W311_SPEED(1,1,1,1,0),  75000000}, /*  75.00 MHz */
    { W311_SPEED(1,1,1,1,1), 133330000}, /* 133.33 MHz */
};
#endif
#define N_W311_FREQS   COUNTOF(w311_freq_tab)

/*
 * Data: w229b_freq_tab
 * Purpose: Table 4: Reserved Register Settings (CPU output frequencies and
 *          their SEL bits, sorted by frequency): Byte 4
 */
static struct
w229b_freq_tab_s{
    uint8 control;
    uint32 speed;
} w229b_freq_tab[] = {
    { W229_SPEED(0,1,0,0,0),  66300000}, /* 66.3Mhz   */
    { W229_SPEED(1,1,1,0,0),  66600000}, /* 66.6Mhz   */
    { W229_SPEED(0,1,1,0,0),  66800000}, /* 66.8Mhz   */
    { W229_SPEED(0,0,0,0,0),  75300000}, /* 75.3Mhz   */
    { W229_SPEED(1,1,0,0,1),  78000000}, /* 78Mhz     */
    { W229_SPEED(1,1,0,0,0),  80000000}, /* 80Mhz     */
    { W229_SPEED(0,0,0,0,1),  95000000}, /* 95Mhz     */
    { W229_SPEED(1,1,1,0,1), 100000000}, /* 100Mhz    */
    { W229_SPEED(0,1,1,0,1), 100200000}, /* 100.2Mhz  */
    { W229_SPEED(0,1,0,0,1), 105000000}, /* 105Mhz    */
    { W229_SPEED(0,0,1,0,1), 110000000}, /* 110Mhz    */
    { W229_SPEED(1,0,1,1,1), 114000000}, /* 114Mhz    */
    { W229_SPEED(1,0,1,1,0), 117000000}, /* 117Mhz    */
    { W229_SPEED(1,0,0,1,1), 122000000}, /* 122Mhz    */
    { W229_SPEED(1,0,1,0,1), 122000000}, /* 122Mhz    */
    { W229_SPEED(1,0,1,0,0), 127000000}, /* 127Mhz    */
    { W229_SPEED(0,0,0,1,0), 129000000}, /* 129Mhz    */
    { W229_SPEED(1,1,1,1,0), 133300000}, /* 133.3Mhz  */
    { W229_SPEED(1,1,1,1,1), 133300000}, /* 133.3Mhz  */
    { W229_SPEED(1,1,0,1,1), 133600000}, /* 133.6Mhz  */
    { W229_SPEED(0,1,1,1,0), 133600000}, /* 133.6Mhz  */
    { W229_SPEED(0,1,1,1,1), 133600000}, /* 133.6Mhz  */
    { W229_SPEED(0,1,0,1,0), 138000000}, /* 138Mhz    */
    { W229_SPEED(0,0,1,1,0), 140000000}, /* 140Mhz    */
    { W229_SPEED(0,1,0,1,1), 140000000}, /* 140Mhz    */
    { W229_SPEED(0,0,1,1,1), 144000000}, /* 144Mhz    */
    { W229_SPEED(0,0,0,1,1), 150000000}, /* 150Mhz    */
    { W229_SPEED(0,0,1,0,0), 150000000}, /* 150Mhz    */
    { W229_SPEED(1,0,0,0,0), 157300000}, /* 157.3Mhz  */
    { W229_SPEED(1,0,0,0,1), 160000000}, /* 160Mhz    */
    { W229_SPEED(1,1,0,1,0), 166000000}, /* 166Mhz    */
};
#define N_W229B_FREQS   COUNTOF(w229b_freq_tab)

static uint8 w229_regvals[W229B_N_REGS];

/*
 * Create control word for SMB word write operation updating N and M.
 */
STATIC uint16
w311_control_word(uint8 n, uint8 m)
{
    uint16 tmp;
    m |= 0x80; /* Programmable frequency enable */
    tmp = m;
    tmp <<= 8;
    tmp |= n;
    return tmp;
}

STATIC void
w311_ss_print(char* pfx, uint8 cb)
{
    char* modeStr = NULL;
    if( cb == W311_SS_CLEAR)
	modeStr = "(off)";
    else if( cb == (W311_SS_ENABLE_NEG5 | W311_SS_CLEAR))
	modeStr = "enabled (-0.5%)";
    else if( cb == (W311_SS_ENABLE_5_5 | W311_SS_CLEAR))
	modeStr = "enabled (+/-0.5%)";
    else if( cb == (W311_SS_ENABLE_2_5 | W311_SS_CLEAR))
	modeStr = "enabled (+/- 0.25%)";
    else if( cb == (W311_SS_ENABLE_3_8 | W311_SS_CLEAR))
	modeStr = "enabled (+/- 0.38%)";
    else
	modeStr = "unknown";
    LOG_CLI((BSL_META("w311: %s spread spectrum %s (0x%x)\n"), pfx, modeStr, cb));
}

/*
 * Function: w311_ioctl
 *
 * Purpose: Given an input speed (clock frequency), tell W311 to
 *          output that frequency for core switch clocks.
 *
 *          If the user provides a data pointer, we simply query the chip for
 *          the clock speed and return the data in an integer format.
 *
 *          The user provides speed value in opcode, we check that the value
 *          is in range, and if so, we compute M/N and program the
 *          chip. If data is non-null, we simply return the current speed.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    opcode - clock speed value, if less than 1Mhz, multiplied by 1Mhz
 *    data - if not-null, we return the clock speed read from the device.
 *    len - unused
 *
 * Returns:
 *     SOC_E_NONE - no error
 *     SOC_E_TIMEOUT - chip temperature reading unavailable or IO error.
 */
STATIC int
w311_ioctl(int unit, int devno,
	   int opcode, void* data, int len)
{
    int  i, rv = SOC_E_NONE;
    uint8 m,n,saddr = soc_i2c_addr(unit, devno);
#ifdef	COMPILER_HAS_DOUBLE
    double val = 0, speed = 0;
#else
    int val = 0, speed = 0;
#endif
    uint16 nm;

    if(!data)
        return SOC_E_PARAM;

    if( opcode == I2C_PLL_W311_SETSPEED ){

	/* Grab requested value */
#ifdef	COMPILER_HAS_DOUBLE
	speed = *((double*)data);

	/* Convert to MHZ if short form given */
	if ( speed < MHZ  )
	    speed *= MHZ;
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Speed requested = %2.2f\n"), speed));
#else
	speed = *((int*)data);
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Speed requested = %d.%02d\n"), 
                     INT_FRAC_2PT_4(speed)));
#endif

	if((int)speed < Megahertz(W311_MIN_CLOCK) ||
	   (int)speed > Megahertz(W311_MAX_CLOCK)){
	    LOG_ERROR(BSL_LS_SOC_I2C,
                      (BSL_META_U(unit,
                                  "unit %d i2c %s: invalid clock speed %dMhz,"
                                  " valid range %d:%dMHz\n"),
                       unit, soc_i2c_devname(unit,devno),
                       (int)speed / MHZ, W311_MIN_CLOCK, W311_MAX_CLOCK));
	    return SOC_E_PARAM;
	}

	if( (int)speed >= Megahertz(W311_LOW_MIN) &&
	    (int)speed <= Megahertz(W311_LOW_MAX))
	    m = W311_M_LOWSPEED ;
	else
	    m = W311_M_HIGHSPEED ;
	/*
	 * The new frequency will start to load whenever there is an
	 * update to either CPU_FSEL_N[7:0] or CPU_FSEL_M[6:0], therefore,
	 * it is recommended by Cypress that Word or Block write be used
	 * to update both registers in one SMBus transaction.
	 */
	/* FCpu = G * (N+3)/(M+3)
	 * n = ((FCpu * (M+3)) / G) - 3
	 */
#ifdef	COMPILER_HAS_DOUBLE
	speed /= MHZ;
	val = ( ( (speed * (m + 3) ) / W311_GEAR_CONSTANT) - 3.0) + 0.5;
#else
	val = ( ( (speed * (m + 3) ) / W311_GEAR_CONSTANT) - 3);
#endif
	n = (uint8) val;
	nm = w311_control_word(n, m);
#ifdef	COMPILER_HAS_DOUBLE
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "w311: set speed=%2.2f n(fp)=%f (m=%d n=%d) hwval=0x%x\n"),
                     speed, val, m, n, nm));
#else
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "w311: set speed=%d.%02d n(fp)=%d (m=%d n=%d) hwval=0x%x\n"),
                     INT_FRAC_2PT_4(speed), val, m, n, nm));
#endif
	rv = soc_i2c_write_word_data(unit, saddr,
				     W311_COMM_CODE(W311_N_REG), nm);
	return rv;
    }


    if( opcode == I2C_PLL_W311_GETSPEED ){

	/* Read M/N values and compute frequency .. */
	rv = soc_i2c_read_word_data(unit, saddr,
				    W311_COMM_CODE(W311_N_REG), &nm);

	m = (uint8)(nm >> 8 & 0x00ff);
	m &= ~0x80;
	n = (uint8)nm;


	/*
	 * User did not program clock via M/N method, read hardwired
	 * freq from latched FS-Sel pins and do table-lookup for
	 * frequency.
	 */
	if( nm == 0){
	    rv = soc_i2c_read_byte_data(unit, saddr,
					W311_COMM_CODE(W311_FS_REG), &n);
	    n &= ~(0x07);
	    n >>= 3;

	    /* FS0,1,2 */
#ifdef	COMPILER_HAS_DOUBLE
#ifdef CLOCK_DEBUG
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "w311: FS_Sel[0:4]=0x%x f=%2.2f\n"),
                         n,0.0));
#endif
	    *((double*)data) = 0.0;
#else
	    *((int*)data) = 0;
#endif

	    /* Look for table match */
	    for ( i = 0; i < N_W311_FREQS; i++) {
#ifdef CLOCK_DEBUG
		LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "n=0x%x tab=0x%x\n"),
                             n, w311_freq_tab[i].control));
#endif
		if(n == w311_freq_tab[i].control){
#ifdef	COMPILER_HAS_DOUBLE
		    *((double*)data) = w311_freq_tab[i].speed;
#else
		    *((int*)data) = w311_freq_tab[i].speed;
#endif
		}

	    }
	}
	else {
#ifdef	COMPILER_HAS_DOUBLE
#ifdef CLOCK_DEBUG
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "w311: m/n=0x%x m=0x%x n=0x%x, f=%2.2f\n"),
                         nm, m, n,
                         (W311_GEAR_CONSTANT * (( n + 3.0)/(m + 3.0)))));
#endif
	    /* Compute freq from data-sheet */
	    *((double*)data) = (W311_GEAR_CONSTANT * (( n + 3.0)/(m + 3.0)));
#else
	    *((int*)data) = (W311_GEAR_CONSTANT * (( n + 3)/(m + 3)));
#endif
	}
    }

    if(opcode == I2C_PLL_W311_GETSPREAD){
	uint8 cb = 0;
	rv =soc_i2c_read_byte_data(unit, saddr,
				   W311_COMM_CODE(W311_CTRL_REG1),&cb);

	if (cb == W311_SS_CLEAR)
	    *((int*)data)=0;
	else
	    *((int*)data)=1;
    }

    if(opcode == I2C_PLL_W311_SPREAD){

	uint8 cb = 0;
	int mode = *((int*)data);
	/*
	 * W311 Byte1: Control Register 1
	 */
	if((rv =soc_i2c_read_byte_data(unit, saddr,
				       W311_COMM_CODE(W311_CTRL_REG1),&cb))<0){
	    LOG_ERROR(BSL_LS_SOC_I2C,
                      (BSL_META_U(unit,
                                  "w311: could not read W311 byte1, control reg1!\n")));
	}

	/* Show current mode decoding */
	w311_ss_print("current",cb);

	/* If user didn't input mode, just quit */
	if((mode < I2C_PLL_SPREAD_NONE) || (mode > I2C_PLL_SPREAD_3_8)) {
	    return rv;
        }

	switch(mode){

	case I2C_PLL_SPREAD_NONE:
	    cb = W311_SS_SET_DISABLE(cb);
	    break;
	case I2C_PLL_SPREAD_NEG5:
	    cb = W311_SS_SET_ENABLE_NEG5(cb);
	    break;
	case I2C_PLL_SPREAD_5_5:
	    cb = W311_SS_SET_ENABLE_5_5(cb);
	    break;
	case I2C_PLL_SPREAD_2_5:
	    cb = W311_SS_SET_ENABLE_2_5(cb);
	    break;
	case I2C_PLL_SPREAD_3_8:
	    cb = W311_SS_SET_ENABLE_3_8(cb);
	    break;
        /*
         mode is validated above. So the default case is never used.
	default:
	    LOG_ERROR(BSL_LS_SOC_I2C,
                      (BSL_META_U(unit,
                                  "w311: spread spectrum error: no mode %d\n"), mode));
	    break;
        */
	}

	if((rv =soc_i2c_write_byte_data(unit, saddr,
				       W311_COMM_CODE(W311_CTRL_REG1),cb))<0){
	    LOG_ERROR(BSL_LS_SOC_I2C,
                      (BSL_META_U(unit,
                                  "w311: could not write W311 byte1, control reg1!\n")));
	}

	w311_ss_print("new", cb);
	return rv;
    }

    return rv;
}



/*
 * Function: w229b_ioctl
 *
 * Purpose: Given an input speed (clock frequency), tell W229b to
 *          output that frequency for core switch clocks.
 *
 *          The user provides speed value in opcode, we check that the value
 *          is in range, and if so, we find the desired speed, and program the
 *          clock chip.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    opcode - clock speed value, if less than 1Mhz, multiplied by 1Mhz
 *    data - unused
 *    len - unused
 *
 * Returns:
 *     SOC_E_NONE - no error
 *     SOC_E_TIMEOUT - chip temperature reading unavailable or IO error.
 */
STATIC int
w229b_ioctl(int unit, int devno,
	   int opcode, void* data, int len)
{
    int i, rv = SOC_E_NONE;
    uint8 saddr = soc_i2c_addr(unit, devno);
    uint32  speed = opcode;
#ifdef	COMPILER_HAS_DOUBLE
    double val = 0;
#else
    int val = 0;
#endif

    /* Set Chip driver access mode first (W311 or W229b mode) */
    if ( data ) {
	if( opcode == I2C_PLL_SETW311 ){
	    is311 = * ((int*)data);
	    return SOC_E_NONE;
	}
    }
    /* Handle W311 differently : it does .5Mhz increments */
    if( is311)
	return w311_ioctl(unit, devno, opcode, data, len);

    /* Convert to MHZ if short form given */
    if ( speed < MHZ  )
	speed *= MHZ;

    if((speed > W229B_MAX_SPEED) || (speed < W229B_MIN_SPEED)){
	LOG_CLI((BSL_META_U(unit,
                            "The following speeds are available on W229b:\n")));

	for(i = 0; i < N_W229B_FREQS; i++){
#ifdef	COMPILER_HAS_DOUBLE
	    val = (double)w229b_freq_tab[i].speed  / (1000.0 * 1000.0) ;
	    LOG_CLI((BSL_META_U(unit,
                                "\t%2.2fMhz\n"), val));
#else
	    val = w229b_freq_tab[i].speed;
	    LOG_CLI((BSL_META_U(unit,
                                "\t%d.%02dMhz\n"), INT_FRAC_2PT_4(val)));
#endif
	}

	return SOC_E_NONE;
    }

    /* Go through table, find speed match ... */
    for(i = 0; i < N_W229B_FREQS; i++) {

	/* Load configuration defaults */
	w229_regvals[0] = W229B_R0;
	w229_regvals[1] = W229B_R1;
	w229_regvals[2] = W229B_R2;
	w229_regvals[3] = W229B_R3;

	/* Load clock speed bytes : write to H/W */
	w229_regvals[4] = w229b_freq_tab[i].control;

	if ( speed <= w229b_freq_tab[i].speed ) {

	    if ( (rv = soc_i2c_block_write(unit, saddr,
					   W229B_COMM_CODE,
					   W229B_CNT, w229_regvals ) ) < 0 ) {

            LOG_ERROR(BSL_LS_SOC_I2C,
                      (BSL_META_U(unit,
                                  "unit %d i2c %s: error on SMB block write: %s\n"),
                       unit, soc_i2c_devname(unit,devno),
                       soc_errmsg(rv)));

		return rv;
	    }
	    soc_i2c_device(unit, devno)->tbyte += 5;
#ifdef	COMPILER_HAS_DOUBLE
	    val = (double)w229b_freq_tab[i].speed  / (1000.0 * 1000.0) ;
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "unit %d i2c %s: set W229B Clock Speed=%.2fMhz"
                                    " (CB=0x%x)\n"),
                         unit, soc_i2c_devname(unit,devno),
                         val, w229_regvals[4]));
#else
	    val = w229b_freq_tab[i].speed;
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "unit %d i2c %s: set W229B Clock Speed=%d.%02dMHz"
                                    " (CB=0x%x)\n"),
                         unit, soc_i2c_devname(unit,devno),
                         INT_FRAC_2PT_4(val), w229_regvals[4]));
#endif
	    break;
	}
    }
    return rv;
}

/*
 * Function: w229b_init
 * Purpose: Initialize the W229b clock chip
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    data - not used
 *    len - not used
 *
 * Returns:
 *     SOC_E_NONE - no failure
 */
STATIC int
w229b_init(int unit, int devno,
	  void* data, int len)
{
    uint8 saddr = soc_i2c_addr(unit, devno);
    uint8 rev;

    if(is311){
	/* 5615 and higher boards should have it ... */
	if((soc_i2c_read_byte_data(unit, saddr, W311_ID_REG, &rev)) < 0){
	    is311 = FALSE;
	    soc_i2c_devdesc_set(unit, devno, "Cypress W229B Clock Chip");
	} else {
	    is311 = TRUE;
	    soc_i2c_devdesc_set(unit, devno, "Cypress W311 Clock Chip");
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "%s: vendor 0x%x revision 0x%x\n"),
                         soc_i2c_devname(unit,devno),
                         rev & 0xf0,		/* Bits 0-3 */
                         rev & 0x0f));		/* Bits 4-7 */
	}
    } else {
	/* In a perfect world, the above detection algorithm would work,
	 * but, however the w229 does not like being read from in a byte
	 * read operation and hangs the I2c bus.
	 */
	soc_i2c_devdesc_set(unit, devno, "Cypress W229B/W311 Clock Chip");
    }
    return SOC_E_NONE;
}

/* NOT USED */
STATIC int
w229b_read(int unit, int devno,
	  uint16 addr, uint8* data, uint32* len)
{
    return SOC_E_NONE;
}

/* NOT USED */
STATIC int
w229b_write(int unit, int devno,
	   uint16 addr, uint8* data, uint32 len)
{
    return SOC_E_NONE;
}

/* W229B/W311 Clock Chip Driver callout */
i2c_driver_t _soc_i2c_w229b_driver = {
    0x0, 0x0, /* System assigned bytes */
    W229B_DEVICE_TYPE,
    w229b_read,
    w229b_write,
    w229b_ioctl,
    w229b_init,
    NULL,
};
