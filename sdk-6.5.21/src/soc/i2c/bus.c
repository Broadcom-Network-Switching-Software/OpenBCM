/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C driver for the CPU Management Interface Controller (CMIC). This
 * module provides common I2C Bus driver routines for using the the
 * I2C bus controller internal to the CMIC as a bus-master. (I2C
 * bus-master driver interface).
 *
 * The I2C bus is a 2-wire bus originally developed by Philips
 * Semiconductor for bi-directional data I/O between two
 * interconnected integrated circuits (hence the name Inter-IC, IIC or
 * I2C). The I2C bus uses the 2-wires SDA (Serial Data) and SCL
 * (Serial Clock) and each device connected to the bus is addressable
 * through a unique slave or device address while simple master/slave
 * relationships exist at all times. Serial, 8-bit oriented,
 * bi-directional data transfers can be made at up to 400Kbits/sec in
 * fast mode, with 100Kbits/s being the norm. The number of IC's
 * connected to this bus is limited only by a maximum bus capacitance
 * of 400pF. For more information, see: The I2C Bus and How to Use it
 *
 * This driver allows access to I2C slave devices attached to the SOC
 * (Switch on a Chip) device's SCL and SDA pins. The SDA and SCL pins
 * are controlled by the intelligent I2C controller inside the CMIC.
 *
 * Known Issues
 *
 * Currently, slave mode is supported only in HW, as supporting dual
 * mode (master/slave) in software is currently unimplemented.
 *
 * The I2C controller will automatically enter slave transmit mode if
 * it receives it's own Slave address with the read bit
 * set. Similarly, the I2C controller will automatically enter slave
 * receive mode if it receives it's own slave address and the write
 * bit, or the general call address. The implication is that you
 * cannot have two masters with the same slave address on the same bus
 * or using the General call address at this time.
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/cmic.h>
#include <soc/iproc.h>
#include <soc/cm.h>
#include <soc/i2c.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#define KHZ_TO_HZ(n) ((n)*1000)

#ifdef HW_DEBUG
#define SOC_I2C_EVENT_LOGGING 1
#define SOC_I2C_TIME_STAMPING 1
#endif

#ifdef SOC_I2C_EVENT_LOGGING
#define SOC_I2C_MAX_EVENTS       (1024*1024) /* 1MB trace buffer */
static uint8* log_ptr[SOC_MAX_NUM_DEVICES];
static int log_index[SOC_MAX_NUM_DEVICES];
#ifdef SOC_I2C_TIME_STAMPING
static sal_usecs_t* time_ptr[SOC_MAX_NUM_DEVICES];
#endif
#endif

/*
 * Function: soc_i2c_log_event
 *
 * Purpose:  Add the last bus status to the event log.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    status - value of BUS STATUS register.
 *
 * Returns:
 *    none
 *
 * Notes:
 *    This routine is used to capture a trace of bus activity.
 */
void
soc_i2c_log_event(int unit, uint8 status)
{
#ifdef SOC_I2C_EVENT_LOGGING
    int idx = log_index[unit];

    if (log_index[unit] == SOC_I2C_MAX_EVENTS) {
	log_index[unit] = 0;
    }

    log_ptr[unit][idx] = status;

#ifdef SOC_I2C_TIME_STAMPING
    time_ptr[unit][idx] = sal_time_usecs();
#endif

    log_index[unit]++;
#else
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(status);
#endif
}

/*
 * Function: soc_i2c_show_log
 *
 * Purpose:  Display the bus trace event log for the specified unit.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    reverse - if set, the log is show in reverse order.
 *
 * Returns:
 *    none
 *
 * Notes:
 *    This routine is used to capture a trace of bus activity.
 */
void
soc_i2c_show_log(int unit, int reverse)
{
#ifdef SOC_I2C_EVENT_LOGGING
    int i;

    if (reverse) {
	for (i = log_index[unit]; i >= 0; i--) {
#ifdef SOC_I2C_TIME_STAMPING
	    LOG_CLI((BSL_META_U(unit,
                                "%dus: STATUS[%d]: %s (0x%x)\n"),
                     time_ptr[unit][i],
                     i,
                     soc_i2c_status_message((soc_i2c_status_t)log_ptr[unit][i]),
                     (soc_i2c_status_t)log_ptr[unit][i]));
#else
	    LOG_CLI((BSL_META_U(unit,
                                "STATUS[%d]: %s (0x%x)\n"),
                     i,
                     soc_i2c_status_message((soc_i2c_status_t)log_ptr[unit][i]),
                     (soc_i2c_status_t)log_ptr[unit][i]));
#endif
	}
    } else {
	for (i = 0; i < log_index[unit]; i++) {
#ifdef SOC_I2C_TIME_STAMPING
	    LOG_CLI((BSL_META_U(unit,
                                "%dus: STATUS[%d]: %s (0x%x)\n"),
                     time_ptr[unit][i],
                     i,
                     soc_i2c_status_message((soc_i2c_status_t)log_ptr[unit][i]),
                     (soc_i2c_status_t)log_ptr[unit][i]));
#else
	    LOG_CLI((BSL_META_U(unit,
                                "STATUS[%d]: %s (0x%x)\n"),
                     i,
                     soc_i2c_status_message((soc_i2c_status_t)log_ptr[unit][i]),
                     (soc_i2c_status_t)log_ptr[unit][i]));
#endif
	}
    }
#else
    LOG_CLI((BSL_META_U(unit,
                        "NOTICE: SOC_I2C_EVENT_LOGGING not compiled in.\n")));
#endif
}

void
soc_i2c_clear_log(int unit)
{
#ifdef SOC_I2C_EVENT_LOGGING
    log_index[unit] = 0;
#endif
}

/*
 * The CMIC I2C controller register values are clocked off the
 * I2C bus which is slow in comparison to PCI. As a result, we
 * need a few PCI cycles of delay so that the CPU "sees" the
 * correct I2C register value.
 */
#define SLEEP(u) \
{ \
    soc_pci_read(u, CMIC_I2C_SLAVE_ADDR); \
    soc_pci_read(u, CMIC_I2C_SLAVE_ADDR); \
    soc_pci_read(u, CMIC_I2C_SLAVE_ADDR); \
    soc_pci_read(u, CMIC_I2C_SLAVE_ADDR); \
}

/*
 * Get a CMIC I2C register in PCI space.
 * Input address is relative to the base of CMIC registers.
 * Same as soc_pci_read(), with a delay for I2C register
 * access.
 */
uint32
soc_i2c_pci_read(int unit, uint32 addr)
{
    SLEEP(unit);
    return soc_pci_read(unit, addr);
}

/*
 * Set a CMIC I2C register in PCI space.
 * Input address is relative to the base of CMIC registers.
 * Same as soc_pci_write(), with a delay for I2C register
 * access.
 */
int
soc_i2c_pci_write(int unit, uint32 addr, uint32 data)
{
    soc_pci_write(unit, addr, data);
    SLEEP(unit);
    return 0;
}

/*
 * Function: soc_write_i2c_stop_bits
 *
 * Purpose:  Generate stop condition on the I2C Bus.
 *           This also recovers from an I2C bus error.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *
 * Returns:
 *    none
 *
 * Notes:
 *    none
 */
STATIC INLINE void
soc_write_i2c_stop_bits(int unit)
{
    soc_i2c_bus_t	*i2cbus;
    uint32		reg;

    i2cbus = I2CBUS(unit);
    reg = soc_i2c_pci_read(unit, CMIC_I2C_CTRL);
    reg |= (CI2CC_MM_STOP | CI2CC_AACK);
    reg &= ~CI2CC_INT_FLAG;

    /*
     * Clear Intr from any previous phase, and initiate STOP phase.
     * IFLG will NOT set when the STOP phase has completed.
     */
    soc_i2c_pci_write(unit, CMIC_I2C_CTRL, reg);

    /* If in interrupt mode, leave I2C interrupt unmasked (enabled). */
    if ((i2cbus->flags & SOC_I2C_MODE_INTR)) {
#ifdef BCM_ESW_SUPPORT
	soc_intr_enable(unit, IRQ_I2C_INTR);
#endif
    }

}

/*
 * Function: soc_write_i2c_start_bits
 *
 * Purpose: Generate start condition on the I2C bus.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 * Returns:
 *    none
 * Notes:
 *    none
 */
STATIC INLINE void
soc_write_i2c_start_bits(int unit)
{
    soc_i2c_bus_t	*i2cbus;
    uint32		reg;

    i2cbus = I2CBUS(unit);
    reg = soc_i2c_pci_read(unit, CMIC_I2C_CTRL);

    reg |= (CI2CC_MM_START | CI2CC_AACK);
    reg &= ~CI2CC_INT_FLAG;

    /* If in interrupt mode, make sure I2C interrupt is disabled. */
    if ((i2cbus->flags & SOC_I2C_MODE_INTR)) {
#ifdef BCM_ESW_SUPPORT
	soc_intr_disable(unit, IRQ_I2C_INTR);
#endif
    }

    /*
     * Clear Intr from any previous phase, and initiate START or
     * REPEATED_START.
     * IFLG will set when the START/REPEATED_START  phase has completed.
     */
    soc_i2c_pci_write(unit, CMIC_I2C_CTRL, reg);

    /* If in interrupt mode, make sure I2C interrupt is enabled. */
    if ((i2cbus->flags & SOC_I2C_MODE_INTR)) {
#ifdef BCM_ESW_SUPPORT
	soc_intr_enable(unit, IRQ_I2C_INTR);
#endif
    }

}

/*
 * Function: soc_i2c_reset
 *
 * Purpose: Reset I2C Bus controller core logic.
 *
 * Parameters:
 *       unit - StrataSwitch I2C bus controller chip number (PCI device)
 * Returns:
 *       none
 * Notes:
 */
void
soc_i2c_reset(int unit)
{
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    uint32 rval;
#endif

#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        READ_CMIC_I2CM_SMBUS_CONFIGr(unit,&rval);
        soc_reg_field_set(unit, CMIC_I2CM_SMBUS_CONFIGr, &rval, RESETf, 1);
        WRITE_CMIC_I2CM_SMBUS_CONFIGr(unit,rval);
        soc_reg_field_set(unit, CMIC_I2CM_SMBUS_CONFIGr, &rval, RESETf, 0);
        WRITE_CMIC_I2CM_SMBUS_CONFIGr(unit,rval);
    } else
#endif
#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
		if (soc_feature(unit, soc_feature_use_smbus2_for_i2c)) {
            READ_IPROCPERIPH_SMBUS2_SMBUS_CONFIGr(unit,&rval);
            soc_reg_field_set(unit, IPROCPERIPH_SMBUS2_SMBUS_CONFIGr, &rval, RESETf, 1);
            WRITE_IPROCPERIPH_SMBUS2_SMBUS_CONFIGr(unit,rval);
            soc_reg_field_set(unit, IPROCPERIPH_SMBUS2_SMBUS_CONFIGr, &rval, RESETf, 0);
            WRITE_IPROCPERIPH_SMBUS2_SMBUS_CONFIGr(unit,rval);
         } else if (soc_feature(unit, soc_feature_use_smbus0_for_i2c)) {
            READ_IPROCPERIPH_SMBUS0_SMBUS_CONFIGr(unit,&rval);
            soc_reg_field_set(unit, IPROCPERIPH_SMBUS0_SMBUS_CONFIGr, &rval, RESETf, 1);
            WRITE_IPROCPERIPH_SMBUS0_SMBUS_CONFIGr(unit,rval);
            soc_reg_field_set(unit, IPROCPERIPH_SMBUS0_SMBUS_CONFIGr, &rval, RESETf, 0);
            WRITE_IPROCPERIPH_SMBUS0_SMBUS_CONFIGr(unit,rval);
         } else {
            READ_IPROCPERIPH_SMBUS1_SMBUS_CONFIGr(unit,&rval);
            soc_reg_field_set(unit, IPROCPERIPH_SMBUS1_SMBUS_CONFIGr, &rval, RESETf, 1);
            WRITE_IPROCPERIPH_SMBUS1_SMBUS_CONFIGr(unit,rval);
            soc_reg_field_set(unit, IPROCPERIPH_SMBUS1_SMBUS_CONFIGr, &rval, RESETf, 0);
            WRITE_IPROCPERIPH_SMBUS1_SMBUS_CONFIGr(unit,rval);
         }
    } else
#endif
    {
        soc_i2c_pci_write(unit, CMIC_I2C_RESET, 0x000000ff);
        sal_usleep(10000);
    }
}

/*
 * Function: soc_i2c_decode_ctrl
 *
 * Purpose: Pretty print encoding of control register.
 *
 * Parameters:
 *    ctrl - unsigned 8bit value for I2C control register
 *           (CMIC PCIM off=0x128 (CMIC_I2C_CONTROL)
 * Returns:
 *    none
 * Notes:
 *   Pretty-printed output of bit meanings in control register.
 */
STATIC void
soc_i2c_decode_ctrl(uint8 ctrl)
{
    if (ctrl & CI2CC_INT_EN) {
	LOG_CLI((BSL_META(" ie")));
    }
    if (ctrl & CI2CC_BUS_EN) {
	LOG_CLI((BSL_META(" be")));
    }
    if (ctrl & CI2CC_MM_START) {
	LOG_CLI((BSL_META(" sta")));
    }
    if (ctrl & CI2CC_MM_STOP) {
	LOG_CLI((BSL_META(" stp")));
    }
    if (ctrl & CI2CC_INT_FLAG) {
	LOG_CLI((BSL_META(" ip")));
    }
    if (ctrl & CI2CC_AACK) {
	LOG_CLI((BSL_META(" aak")));
    }
    LOG_CLI((BSL_META("\n")));
}

/*
 * Function: soc_i2c_decode_op
 *
 * Purpose: Print CPU requested IO.
 *
 * Parameters:
 *       soc_i2c_op_t opcode -- CPU opcode
 * Returns:
 *       none
 * Notes:
 *       Pretty-printed output of meanings of last CPU initiated IO.
 */
STATIC char*
soc_i2c_decode_op(soc_i2c_op_t opcode)
{
    switch (opcode) {
    case SOC_I2C_IDLE:		return "IDLE";
    case SOC_I2C_START:		return "START";
    case SOC_I2C_REP_START:	return "REP START";
    case SOC_I2C_TX:		return "TX";
    case SOC_I2C_RX:		return "RX";
    case SOC_I2C_STOP:		return "STOP";
    case SOC_I2C_PROBE:		return "PROBE";
    default:			return "?";
    }
}

/*
 * Function: soc_i2c_decode_flags
 *
 * Purpose: Decode driver flags and pretty print output.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    msg - message string to print after device string
 *    flags - I2C Bus driver flags value.
 * Returns:
 *    none
 * Notes:
 *    Pretty-printed output of meanings of current state of driver.
 */
void
soc_i2c_decode_flags(int unit, char *msg, uint32 flags)
{
    LOG_CLI((BSL_META_U(unit,
                        "unit %d i2c: %s:"), unit, msg));
    if (flags == 0) {
        LOG_CLI((BSL_META_U(unit,
                            " OFFLINE")));
    }
    if (flags & SOC_I2C_MODE_PIO) {
        LOG_CLI((BSL_META_U(unit,
                            " PIO")));
    }
    if (flags & SOC_I2C_MODE_INTR) {
        LOG_CLI((BSL_META_U(unit,
                            " INTR")));
    }
    if (flags & SOC_I2C_ATTACHED) {
        LOG_CLI((BSL_META_U(unit,
                            " ATTACHED")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));
}

/*
 * Function: soc_i2c_wait_for_iflg_set
 *
 * Purpose: Read IFLG register, if not set, then busy-wait (spin)
 * until ihe IFLG register has been set, or if the number of retries
 * have been exceeded, a timeout error.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *
 * Returns:
 *    SOC_E_NONE - device ok
 *    SOC_E_TIMEOUT - the last operation (state) timed out.
 *
 * Notes:
 *    This is used for PIO mode I2C operations.
 */
int
soc_i2c_wait_for_iflg_set(int unit)
{
    soc_i2c_bus_t	*i2cbus;
    volatile register uint32 reg;
    uint32 retries;

    i2cbus = I2CBUS(unit);
    retries = i2cbus->pio_retries;

    while (!((reg = soc_i2c_pci_read(unit, CMIC_I2C_CTRL)) &
	     CI2CC_INT_FLAG)
	   && --retries) {
           sal_udelay(1);
	/* SPIN */
    }

    /* Store a statistic for PIO mode */
    i2cbus->iflg_polls = _i2c_abs(((int)i2cbus->pio_retries) - (int) retries);
    return retries > 0 ? SOC_E_NONE : SOC_E_TIMEOUT;
}

/* Forward declaration of static routine to set bus frequency */
static int soc_i2c_set_freq(int unit);

/*
 * Function: soc_i2c_attach
 *
 * Purpose: I2C Bus attach routine, main entry point for I2C startup.
 * Initialize the I2C controller configuration for the specified
 * device. Default is to disable the device, if enable is specified,
 * the default frequency is 100Khz. Flags currently allows selection
 * of Interrupt driven mode, PIO mode, or force configuration.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    flags - bitmap (logical OR) of one or more of the following:
 *                  SOC_I2C_MODE_INTR - interrupt driven mode
 *                  SOC_I2C_MODE_PIO - programmed I/O mode
 *    speed_khz - Requested I2C bus speed, in kilohertz.
 *                Zero or excessively large values use default speed.
 *                < 0 uses currently programmed H/W speed, or default
 *                speed if H/W not currently programmed.
 *
 * Returns:
 *	SOC_E_* if negative
 *	count of found devices if positive or zero
 *
 * Notes: Default is Interrupt mode, if both are selected Interrupt is
 *       chosen.
 */
int
soc_i2c_attach(int unit, uint32 flags, int speed_khz)
{
    uint32		reg;
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);
    if (i2cbus == NULL) {
	i2cbus = sal_alloc(sizeof(*i2cbus), "i2c_bus");
	if (i2cbus == NULL) {
	    return SOC_E_MEMORY;
	}
	I2CBUS_VOID(unit) = i2cbus;
	sal_memset(i2cbus, 0, sizeof(*i2cbus));
    }

    if (bsl_check(bslLayerSoc, bslSourceI2c, bslSeverityNormal, unit)) {
	soc_i2c_decode_flags(unit, "current flags", i2cbus->flags);
	soc_i2c_decode_flags(unit, "new flags", flags);
    }

#ifdef SOC_I2C_EVENT_LOGGING
    if (!log_ptr[unit]) {
	log_ptr[unit] = (uint8*)sal_alloc(SOC_I2C_MAX_EVENTS, "i2c event log");
    }
    log_index[unit] = 0;
#ifdef SOC_I2C_TIME_STAMPING
    if (!time_ptr[unit]) {
	time_ptr[unit] = (sal_usecs_t*)
	    sal_alloc(SOC_I2C_MAX_EVENTS*sizeof(sal_usecs_t),
		      "i2c timestamp event log");
    }
#endif
#endif


    /* If not yet done, create synchronization semaphores/mutex */
    if (i2cbus->i2cMutex == NULL) {
        i2cbus->i2cMutex = sal_mutex_create("I2C Mutex");
	if (i2cbus->i2cMutex == NULL) {
	    return SOC_E_MEMORY;
	}
    }

    if (i2cbus->i2cIntr == NULL) {
        i2cbus->i2cIntr = sal_sem_create("I2C interrupt", sal_sem_BINARY, 0);
	if (i2cbus->i2cIntr == NULL) {
	    return SOC_E_MEMORY;
	}
    }

    /* Set semaphore timeout values */
    if (SAL_BOOT_QUICKTURN) {
	i2cbus->i2cTimeout = I2C_TIMEOUT_QT;
    } else if (SAL_BOOT_PLISIM) {
	i2cbus->i2cTimeout = I2C_TIMEOUT_PLI;
    } else {
	i2cbus->i2cTimeout = I2C_TIMEOUT;
    }

    i2cbus->i2cTimeout = soc_property_get(unit, spn_I2C_TIMEOUT_USEC,
                                          i2cbus->i2cTimeout);

    /* Choose one or the other IO mode, default
     * to interrupt driven
     */
    if ( ((flags & SOC_I2C_MODE_INTR) != 0) ==
	 ((flags & SOC_I2C_MODE_PIO) != 0) ) {
	i2cbus->flags = SOC_I2C_MODE_INTR;
    } else {
        i2cbus->flags = flags & (SOC_I2C_MODE_INTR | SOC_I2C_MODE_PIO);
    }

    /* Number of PIO's (IFLG/ACK) */
    i2cbus->pio_retries = 1000000;
    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "soc_i2c_attach: oldspeed=%d newspeed=%d\n"),
              i2cbus->frequency, KHZ_TO_HZ(speed_khz)));

    /*
     * Use default speed if zero or bad value specified,
     * or if current speed requested with unprogrammed H/W.
     * Leave the H/W speed alone if negative speed requested
     * and the H/W already has non-zero speed programmed.
     * Otherwise keep requested speed.
     */
    if ( ((speed_khz == 0) || (speed_khz > (SOC_IS_XGS3_SWITCH(unit) ? 250 : 2500))) ||
	 ((speed_khz < 0) && (i2cbus->frequency <= 0)) ) {
      if (SOC_IS_XGS_FABRIC(unit) || SOC_IS_XGS3_SWITCH(unit)) {
	    /* 10Gig Ethernet on board seems to cause signal
	     * integrity issue, hence, 100Khz is default on 5670.
	     */
	    speed_khz = CMIC_I2C_SPEED_SLOW_IO;
	} else {
	    speed_khz = CMIC_I2C_SPEED_DEFAULT;
	}
    }
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        /* I2C set to Master Mode through Override Strap */
        READ_CMIC_OVERRIDE_STRAPr(unit, &reg);
        soc_reg_field_set(unit, CMIC_OVERRIDE_STRAPr, &reg,
                ENABLE_OVERRIDE_I2C_MASTER_SLAVE_MODEf, 1);
        soc_reg_field_set(unit, CMIC_OVERRIDE_STRAPr, &reg,
                I2C_MASTER_SLAVE_MODEf, 1);
        WRITE_CMIC_OVERRIDE_STRAPr(unit, reg);

        /* 1) Enable CPU access to I2C controller */
        READ_CMIC_I2CM_SMBUS_CONFIGr(unit,&reg);
        soc_reg_field_set(unit, CMIC_I2CM_SMBUS_CONFIGr, &reg, SMB_ENf, 1);
        WRITE_CMIC_I2CM_SMBUS_CONFIGr(unit,reg);
        /* Write to I2C register after it is enabled */
        if (speed_khz > 0) {
            i2cbus->frequency = KHZ_TO_HZ(speed_khz);
        }
        /* 2) Program the SOC device's (7-bit) slave I2C address.  */
        /* SOC Default Slave Address = SOC_I2C_SLAVE_BASE + PCI_ID */
        

        /* 3) Enable bus, interrupts */

        /* 4) Tell CMIC to enable SOC I2C interrupts
         * only when in interrupt driven IO mode.
         */
        
    } else
#endif
#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
        /* 1) Enable CPU access to I2C controller */
		if (soc_feature(unit, soc_feature_use_smbus2_for_i2c)) {
            READ_IPROCPERIPH_SMBUS2_SMBUS_CONFIGr(unit,&reg);
            soc_reg_field_set(unit, IPROCPERIPH_SMBUS2_SMBUS_CONFIGr, &reg, SMB_ENf, 1);
            WRITE_IPROCPERIPH_SMBUS2_SMBUS_CONFIGr(unit,reg);
        } else if (soc_feature(unit, soc_feature_use_smbus0_for_i2c)) {
            READ_IPROCPERIPH_SMBUS0_SMBUS_CONFIGr(unit,&reg);
            soc_reg_field_set(unit, IPROCPERIPH_SMBUS0_SMBUS_CONFIGr, &reg, SMB_ENf, 1);
            WRITE_IPROCPERIPH_SMBUS0_SMBUS_CONFIGr(unit,reg);
        } else {
            READ_IPROCPERIPH_SMBUS1_SMBUS_CONFIGr(unit,&reg);
            soc_reg_field_set(unit, IPROCPERIPH_SMBUS1_SMBUS_CONFIGr, &reg, SMB_ENf, 1);
            WRITE_IPROCPERIPH_SMBUS1_SMBUS_CONFIGr(unit,reg);
        }
        /* Write to I2C register after it is enabled */
        if (speed_khz > 0) {
            i2cbus->frequency = KHZ_TO_HZ(speed_khz);
        }
        /* 2) Program the SOC device's (7-bit) slave I2C address.  */
        /* SOC Default Slave Address = SOC_I2C_SLAVE_BASE + PCI_ID */
        

        /* 3) Enable bus, interrupts */

        /* 4) Tell CMIC to enable SOC I2C interrupts
         * only when in interrupt driven IO mode.
         */
        
    } else
#endif
    {
        /* 1) Enable CPU access to I2C controller */
        reg = soc_i2c_pci_read(unit, CMIC_CONFIG);
        reg |= CC_I2C_EN;
        soc_i2c_pci_write(unit, CMIC_CONFIG, reg);

        /* Write to I2C register after it is enabled */
        if (speed_khz > 0) {
            i2cbus->frequency = KHZ_TO_HZ(speed_khz);
    	soc_i2c_set_freq(unit);
        }

        /* 2) Program the SOC device's (7-bit) slave I2C address.  */
        /* SOC Default Slave Address = SOC_I2C_SLAVE_BASE + PCI_ID */
        i2cbus->master_addr = SOC_I2C_SLAVE_BASE + unit;
        soc_i2c_pci_write(unit, CMIC_I2C_SLAVE_ADDR, i2cbus->master_addr<<1);

        /* 3) Enable bus, interrupts */
        reg = soc_i2c_pci_read(unit, CMIC_I2C_CTRL);
        if (i2cbus->flags & SOC_I2C_MODE_INTR) {
    	reg |= CI2CC_INT_EN;
        }

#ifdef CONFIG_I2C_MIXED_MODE
        /* Allows the controller to enter slave mode when it sees
         * either it's own or the general call address.
         */
        reg |= CI2CC_BUS_EN;
#endif
        soc_i2c_pci_write(unit, CMIC_I2C_CTRL, reg);

#ifdef BCM_ESW_SUPPORT
        /* 4) Tell CMIC to enable SOC I2C interrupts
         * only when in interrupt driven IO mode.
         */
        if (i2cbus->flags & SOC_I2C_MODE_INTR) {
    	soc_intr_enable(unit, IRQ_I2C_INTR);
        } else {
    	soc_intr_disable(unit, IRQ_I2C_INTR);
        }
#endif
    }

    /* Enable smbus0 in iproc */
#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_eeprom_iproc)) {
        /* 1) Enable CPU access to I2C controller */
        READ_CHIPCOMMONG_SMBUS0_SMBUS_CONFIGr(unit, &reg);
        soc_reg_field_set(unit, CHIPCOMMONG_SMBUS0_SMBUS_CONFIGr, &reg, SMB_ENf, 1);
        WRITE_CHIPCOMMONG_SMBUS0_SMBUS_CONFIGr(unit, reg);
    }
#endif /* BCM_IPROC_SUPPORT */

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "unit %d i2c 0x%03x bus: mode %s, speed %dKbps\n"),
                 unit, i2cbus->master_addr,
                 (i2cbus->flags & SOC_I2C_MODE_PIO) ? "PIO" : "INTR",
                 (i2cbus->frequency+500) / 1000 ));

/*
 * Disable general call addresses (for now).
 */
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        
    } else
#endif
#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
        
    } else
#endif
    {
        soc_i2c_pci_write(unit, CMIC_I2C_SLAVE_ADDR,
    		      soc_i2c_pci_read(unit, CMIC_I2C_SLAVE_ADDR)
    		      & ~(0x01));
    }

    i2cbus->flags |= SOC_I2C_ATTACHED;

    /*
     * Probe for I2C devices, update device list for detected devices ...
     */
    if (flags & SOC_I2C_NO_PROBE) {
        return SOC_E_NONE;
    } else {
        return soc_i2c_probe(unit);
    }

}

/*
 * Function: soc_i2c_detach
 *
 * Purpose: I2C detach routine: free resources used by I2C bus driver.
 *
 * Paremeters:
 *    unit - StrataSwitch device number or I2C bus number
 * Returns:
 *    SOC_E_NONE - no error
 * Notes:
 *    none
 */
int
soc_i2c_detach(int unit)
{
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);

    if (i2cbus != NULL) {
        (void)soc_i2c_unload_devices(unit);
        if (i2cbus->i2cIntr) {
            sal_sem_destroy(i2cbus->i2cIntr);
            i2cbus->i2cIntr = 0;
        }
        if (i2cbus->i2cMutex) {
            sal_mutex_destroy(i2cbus->i2cMutex);
            i2cbus->i2cMutex = 0;
        }
        sal_free(i2cbus);
        I2CBUS_VOID(unit) = NULL;
    }

    return SOC_E_NONE;
}

/*
 * Function: soc_i2c_next_bus_phase
 *
 * Purpose: Interrupt clear/next operation continue.  Clear the IFLG
 * to trigger or complete a CPU initiated transaction.  If tx_ack is
 * set, then an acknowledgement will be sent; usually tx_ack is only
 * relevant for receive operations.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    tx_ack - when non-zero, ACKs will be sent, otherwise NACKs will be sent.
 *
 * Returns:
 *    Once the next (implied) phase has been started. Calling code
 *    must call soc_i2c_wait() to get indication of phase completion.
 *
 * Notes:
 *    The nature of the next bus phase depends on the current state
 *    of the I2C controller.
 */
void
soc_i2c_next_bus_phase(int unit, int tx_ack)
{
    volatile register uint32 reg, data=0;
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);

#ifdef SOC_I2C_EVENT_LOGGING
    soc_i2c_log_event(unit, soc_i2c_stat(unit));
#endif

    reg = soc_i2c_pci_read(unit, CMIC_I2C_CTRL);

    /* Will clear the interrupt flag when we start next phase */
    reg &= ~CI2CC_INT_FLAG;

    /* Set remote transmit acknowledgement .. */
    if (tx_ack) {
	reg |= CI2CC_AACK;
    } else {
	reg &= ~CI2CC_AACK;
    }

    /* Set it all in one write */
    soc_i2c_pci_write(unit, CMIC_I2C_CTRL, reg);

    /* More debug nonsense */
    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "soc_i2c_next_bus_phase: (after) "
                         "ctrl=0x%x data=0x%x op=%s(%d) "),
              reg, data,
              soc_i2c_decode_op(i2cbus->opcode),
              i2cbus->opcode));
    if (bsl_check(bslLayerSoc, bslSourceI2c, bslSeverityNormal, unit)) {
	soc_i2c_decode_ctrl(reg);
    }

    /* If in interrupt mode, re-enable the I2C interrupt. */
    if (i2cbus->flags & SOC_I2C_MODE_INTR) {
#ifdef BCM_ESW_SUPPORT
	soc_intr_enable(unit, IRQ_I2C_INTR);
#endif
    }

}

/*
 * Function: soc_i2c_stat
 *
 * Purpose: Return bus status code in enumerated type format.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *
 * Returns:
 *    bus status code as enumerated type definition
 *
 * Notes:
 *    This routine should be used in conjunction with
 *    soc_i2c_status_message
 */
soc_i2c_status_t
soc_i2c_stat(int unit)
{
    uint32 stat;

    stat = soc_i2c_pci_read(unit, CMIC_I2C_STAT);
    stat &= CMIC_I2C_REG_MASK;
    return (soc_i2c_status_t) stat;
}

/*
 * Function: soc_i2c_intr
 *
 * Purpose: ISR for I2C interrupts, we basically unblock the calling
 *          task since clearing of the IFLG invokes a particular
 *          action which is state-machine dependent and requires the
 *          data register be loaded before it is cleared.
 *
 * Parameters:
 *	unit - StrataSwitch device number or I2C bus number
 * Returns:
 *	none
 * Notes:
 *	executed from within interrupt context
 */
void
soc_i2c_intr(int unit)
{
    soc_i2c_bus_t	*i2cbus;
    soc_i2c_status_t	s;

    i2cbus = I2CBUS(unit);
#ifdef BCM_ESW_SUPPORT
    /* Disable interrupts so caller gets a chance to run... */
    soc_intr_disable(unit, IRQ_I2C_INTR);
#endif
    s = soc_i2c_stat(unit);

    /*
     * Slave Processing: Trap all Slave requests for now, when the
     * CMIC is in bus-slave mode (default power-up state), all of
     * this processing is done by the CMIC I2C controller, when we
     * are in master mode, and another master is accessing us on the
     * bus, we may get confused as we are not acting as a slave.
     *
     * Slave Transmit Mode
     *
     * The I2C controller will enter into Slave mode when it
     * receives it's own Slave Address, and a read bit after
     * a start condition.
     *
     * Slave transmit mode can also be entered directly from a master
     * mode transaction if arbitration is lost in master mode during
     * the transmission of an address and the slave address and read
     * bit are received.
     *
     * The data byte to be transmitted should then be loaded
     * into the DATA register and the IFLG cleared. When the
     * I2C  controller has transmitted the byte and received an
     * acknowledge, the IFLG will be set and the STAT register
     * will contain 0xB8. Once the last byte to be transmitted
     * has been loaded into the DATA register, the AAK bit
     * should be cleared when the IFLG is cleared. After the
     * last byte has been transmitted, the IFLG will be set and
     * the STAT register will contain 0xC8. The I2C will then
     * return to the idle state and the AAK bit must be set to
     * one before slave mode can be entered again.  If no
     * acknowledge is received after transmitting a byte, the
     * IFLG will be set and the STAT register will contact
     * 0xC0. The I2C will then return to the idle state. If the
     * STOP condition is detected after an acknowledge bit, the
     * I2C will return to the idle state.
     */

    if (s == SOC_I2C_SADDR_RX_RD_BIT_RX_ACK_TX ||
	s == SOC_I2C_ARB_LOST_IN_ADDR_PHASE_SADDR_RX_RD_BIT_RX_ACK_TX) {
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "i2c%d: slave transmit mode entered: %s\n"),
                     unit, soc_i2c_status_message(s)));

	/* Terminate remote client: send NACK */
	soc_i2c_next_bus_phase(unit, FALSE);
    }

    /*
     * Slave receive mode; in the slave receive mode, a number of
     * bytes are received from a master transmitter.
     *
     * The I2C controller will enter into slave recieve mode when it
     * receives its own slave address and a write bit (LSB=0) after a
     * START condition. The I2C will then transmit an ACK bit and set
     * the IFLG bit in the CTRL register: the STAT register will then
     * contain the status code 0x60. The I2C will also enter slave
     * receive mode when it receives the general call address 0x00 (if
     * the GCE bit in the ADDR register is set). The status code will
     * then be 0x70.
     *
     * Slave receive mode can also be entered directly from a master
     * mode if arbitration is lost in master mode during the
     * transmission of an address and the slave address and write bit
     * (or the general call address if bit GCE in the ADDR register is
     * set to one) are received. The status code in the STAT register
     * will then be 0x68 if the slave address was received or 0x78 if
     * the General Call address was received. The IFLG bit must be
     * cleared to zero to allow the data transfer to continue.
     *
     * If the AAK bit in the CTRL register is set to one, then after
     * each byte is received, an acknowledge bit (low level on SDA) is
     * transmitted and the IFLG bit is set: the stat register will then
     * contain status code 0x80 (or 0x90 if slave receive mode was
     * entered with the general call address). The received data byte
     * can be read from the DATA register and the IFLG bit must be
     * cleared to allow the transfer to continue. When the STOP
     * condition or a repeated START condition is detected after the
     * acknowledge bit, the the IFLG bit is set and the STAT register
     * will contain status code 0xA0.
     *
     * If the AAK bit is cleared to zero during a transfer, the I2C will
     * transmit a NACK bit (high level on SDA) after the next byte is
     * received, and set the IFLG bit. The STAT register will contain
     * status code 0x88 (or 0x98 if slave receive mode was entered with
     * the general call address). When the IFLG bit has been cleared to
     * zero, the I2C will return to the idle state.
     */
    if (s == SOC_I2C_SADDR_RX_WR_BIT_RX_ACK_TX ||
	s == SOC_I2C_GC_ADDR_RX_ACK_TX ||
	s == SOC_I2C_ARB_LOST_SADDR_RX_WR_BIT_RX_ACK_TX ||
	s == SOC_I2C_ARB_LOST_GC_ADDR_RX_ACK_TX ||
	s == SOC_I2C_DATA_BYTE_RX_AFTER_SADDR_RX_ACK_TX ||
	s == SOC_I2C_DATA_BYTE_RX_AFTER_SADDR_RX_ACK_TX ||
	s == SOC_I2C_DATA_BYTE_RX_AFTER_SADDR_RX_NO_ACK_TX ||
	s == SOC_I2C_DATA_BYTE_RX_AFTER_GC_ADDR_RX_ACK_TX ||
	s == SOC_I2C_DATA_BYTE_RX_AFTER_GC_ADDR_RX_NO_ACK_TX ||
	s == SOC_I2C_STOP_OR_REP_START_COND_RX_IN_SLAVE_MODE) {

	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "i2c%d: slave receive mode entered: %s\n"),
                     unit, soc_i2c_status_message(s)));

	/* Terminate remote client: send NACK */
	soc_i2c_next_bus_phase(unit, FALSE);
    }

    /* Master processing, simply give back semaphore ... */
    if (i2cbus && i2cbus->i2cIntr) {
	sal_sem_give(i2cbus->i2cIntr);
    }
}

/*
 * Function: soc_i2c_wait
 *
 * Purpose: Wait for Interrupt Pending state. This routine is the second
 *          half of soc_intr and in PIO mode, it blocks until the next
 *          state of I2C bus controller is ready for processing.
 *
 *          In interrupt-driven mode, we wait for the interrupt to
 *          occur and to be unblocked from the semaphore relinquished
 *          by the interrupt. In PIO mode, we poll some number of
 *          times, waiting for IFLG to be set.
 *
 * Input:
 *    unit - StrataSwitch device number or I2C bus number
 *
 * Returns:
 *          SOC_E_NONE  - the device was contacted and is ready for the
 *          next I/O state.
 *          SOC_E_TIMEOUT - device is not present or failure.
 *  Notes:
 *          Usually, we will initiate some action (e.g. START, ADDR),
 *          and then we will load a data byte, clear the iflg, and then
 *          soc_i2c_wait for the next state. Finally, we issue STOP
 *          to release the bus.
 */
int
soc_i2c_wait(int unit)
{
    uint32 stat;
    int rv = SOC_E_NONE;
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);

    /* Interrupt Driven IO: Wait for interrupt */
    if (i2cbus->flags & SOC_I2C_MODE_INTR) {
	if (sal_sem_take(i2cbus->i2cIntr, i2cbus->i2cTimeout) != 0) {
	    rv = SOC_E_TIMEOUT;
	}
    } else {
	/* PIO: Poll IFLG=1 */
	rv = soc_i2c_wait_for_iflg_set(unit);
    }

    stat = soc_i2c_pci_read(unit, CMIC_I2C_STAT);
    i2cbus->stat = stat;

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "soc_i2c_wait: current state=0x%x:[%s]\n"),
              stat, soc_i2c_status_message((soc_i2c_status_t)stat)));
    return rv;
}

/*
 * Function: _i2c_start_helper
 *
 * Purpose: Helper routine for soc_i2c_start() and soc_i2c_rep_start().
 *          Issue an I2C start command and the provided bus_addr byte.
 *          The bus_addr byte contains the appropriate read/write bit.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    bus_addr - I2C slave device bus address byte. Contains R/W info.
 *    repeated - Set to generate REP_START, clear to generate START.
 *
 * Returns: SOC_E_NONE if the device was contacted and ready for I/O.
 *          SOC_E_INTERNAL - Unexpected or internal error
 *          SOC_E_TIMEOUT if the device is not present.
 *
 *        If SOC_E_NONE is returned, the I2C bus is in a "suspended"
 *        state, ready to be bumped to the next appropriate state
 *        (read, write, or stop).
 *        Otherwise, returns with the I2C bus in stopped (idle) state.
 *
 * Notes: 10-bit addressing currently not supported by H/W or S/W.
 */
int
_i2c_start_helper(int unit, i2c_bus_addr_t bus_addr, int repeated)
{
    int rv = SOC_E_NONE;
    soc_i2c_bus_t	*i2cbus;
    soc_i2c_status_t s = soc_i2c_stat(unit);
    soc_i2c_status_t correct_next_stat = SOC_I2C_NO_STATUS;

    i2cbus = I2CBUS(unit);
    /*
     * A START should happen only with a status of SOC_I2C_NO_STATUS.
     * A REP_START should happen with a non-idle status.
     */
    if ( (!repeated && (s != SOC_I2C_NO_STATUS)) ||
	 (repeated && (s == SOC_I2C_NO_STATUS)) ) {
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "unit %d i2c 0x%x: %sSTART- BAD STATUS: %s\n"),
                     unit, bus_addr>>1,
                     (repeated ? "REP_" : ""),
                     soc_i2c_status_message(s)));
	rv = SOC_E_INTERNAL;
	goto done;
    }

    soc_write_i2c_start_bits(unit);

    if (repeated) {
        i2cbus->opcode = SOC_I2C_REP_START;
	correct_next_stat = SOC_I2C_REP_START_TX;
    } else {
        i2cbus->opcode = SOC_I2C_START;
	correct_next_stat = SOC_I2C_START_TX;
    }

    if (SOC_E_NONE == soc_i2c_wait(unit)) {

	if ((s = soc_i2c_stat(unit)) == correct_next_stat) {
	    /*
	     * We generated start, now send the slave's bus address byte.
	     * (7-bit address mode only)
	     */
	    soc_i2c_pci_write(unit, CMIC_I2C_DATA, bus_addr);
	    soc_i2c_next_bus_phase(unit, 1);
	} else {
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "unit %d i2c 0x%x: %sSTART unhandled state 0x%x:"
                                 " %s\n"),
                      unit, bus_addr>>1,
                      repeated?"REP_":"",
                      s,
                      soc_i2c_status_message(s)));
	    rv = SOC_E_INTERNAL;
	    goto done;
	}

	/*
	 * Now, wait again for an interrupt. One of four possible
	 * interrupts should occur:
	 *
	 * Ready for IO: (R/W)
	 *    SOC_I2C_ADDR_WR_BIT_TX_ACK_RX - Device ready for writing
	 *    SOC_I2C_ADDR_RD_BIT_TX_ACK_RX - Device ready for reading
	 * No Device :
	 *    SOC_I2C_ADDR_WR_BIT_TX_NO_ACK_RX - No device ready
	 *                                       for write
	 *    SOC_I2C_ADDR_RD_BIT_TX_NO_ACK_RX - No device ready
	 *                                       for read
	 */
	if ((rv = soc_i2c_wait(unit)) == SOC_E_NONE) {
	    s = soc_i2c_stat(unit);
	    if (s == SOC_I2C_ADDR_WR_BIT_TX_ACK_RX) {
	        i2cbus->opcode = SOC_I2C_TX;
		rv = SOC_E_NONE;
		/* Next I2C bus phase is write-ready. */
	    } else if (s == SOC_I2C_ADDR_RD_BIT_TX_ACK_RX) {
	        i2cbus->opcode = SOC_I2C_RX;
		rv = SOC_E_NONE;
		/* Next I2C bus phase is read-ready. */
	    } else if (s == SOC_I2C_ADDR_RD_BIT_TX_NO_ACK_RX ||
		       s == SOC_I2C_ADDR_WR_BIT_TX_NO_ACK_RX) {
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "unit %d i2c 0x%x: no response from device: %s\n"),
                      unit, bus_addr>>1,
                      soc_i2c_status_message(s)));
            rv = SOC_E_TIMEOUT;
	    } else{
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "unit %d i2c 0x%x: BUS_ADDR "
                                 "unhandled state 0x%x:"
                                 " %s\n"),
                      unit, bus_addr>>1,
                      s,
                      soc_i2c_status_message(s)));
            rv = SOC_E_INTERNAL;
	    }
	}
    } else {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "unit %d i2c 0x%x: timeout generating start condition:"
                             " check or reset I2C bus\n"),
                  unit, bus_addr>>1));
        rv = SOC_E_TIMEOUT;
    }

 done:
    if (rv != SOC_E_NONE) {
	/* Very important, if anything went wrong,
	 * we MUST release the bus to return to idle state
	 */
	soc_i2c_stop(unit);
    }

    return rv;
}


/*
 * Function: soc_i2c_start
 *
 * Purpose: Issue an I2C start command and the provided bus_addr byte.
 *          The bus_addr byte contains the appropriate read/write bit.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    bus_addr - I2C slave device bus address byte. Contains R/W info.
 *
 * Returns: SOC_E_NONE if the device was contacted and ready for I/O.
 *          SOC_E_INTERNAL - Unexpected or internal error
 *          SOC_E_TIMEOUT if the device is not present.
 *
 *        If SOC_E_NONE is returned, the I2C bus is in a "suspended"
 *        state. Calling code must then progress to the next
 *        appropriate I2C bus phase.
 *
 *        See also: SOC_I2C_TX_ADDR(), SOC_I2C_RX_ADDR()
 *
 * Notes: 10-bit addressing currently not supported by H/W or S/W.
 */
int
soc_i2c_start(int unit, i2c_bus_addr_t bus_addr)
{
    return _i2c_start_helper(unit, bus_addr, FALSE);
}


/*
 * Function: soc_i2c_rep_start
 *
 * Purpose: Generate a repeated start and the specified bus_addr byte.
 *          The bus_addr byte contains the appropriate read/write bit.
 *          Typically, this is done in the middle of an operation in
 *          order to delimit a new command sequence without releasing
 *          the I2C bus.
 *
 * Parameters:
 *
 *    unit - StrataSwitch device number or I2C bus number
 *    bus_addr - I2C slave device bus address byte. Contains R/W info.
 *
 * Returns:
 *
 *    SOC_E_TIMEOUT - the device cannot be contacted or is offline
 *    SOC_E_INTERNAL - Unexpected or internal error
 *    SOC_E_NONE - no error, device ready for IO.
 *
 * Notes:
 *    If SOC_E_NONE is returned, the I2C bus is in a "suspended"
 *    state. Calling code must then progress to the next
 *    appropriate I2C bus phase.
 *
 * Notes: 10-bit addressing currently not supported by H/W or S/W.
 */
int
soc_i2c_rep_start(int unit, i2c_bus_addr_t bus_addr)
{
    return _i2c_start_helper(unit, bus_addr, TRUE);
}


/*
 * Function: soc_i2c_write_one_byte
 *
 * Purpose: Write data to the last addressed slave device.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    data - data byte to transmit
 *
 * Returns:
 *         SOC_E_NONE if the device was contacted and the operation
 *         succeeded.
 *         SOC_E_BUSY if the device timed out or is busy.
 *
 * Notes:
 *       Before this routine can be used, the I2C controller must
 *       be in a write-ready state, i.e. following a START-WRITE_SADDR
 *       phase or a previous write operation.
 */
int
soc_i2c_write_one_byte(int unit, uint8 data)
{
    soc_i2c_bus_t	*i2cbus;

    i2cbus = I2CBUS(unit);
    /* Make sure we're in a write-ready state. */
    if (i2cbus->opcode != SOC_I2C_TX) {
        return SOC_E_INTERNAL;
    }

    soc_i2c_pci_write(unit, CMIC_I2C_DATA, data);
    soc_i2c_next_bus_phase(unit, 1);
    if (SOC_E_TIMEOUT == soc_i2c_wait(unit)) {
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "soc_i2c_write_one_byte: u=%d data=0x%x"
                             " DEVICE TIMEOUT!\n"),
                  unit, data));
	return SOC_E_BUSY;
    }
    return SOC_E_NONE;
}


/*
 * Function: soc_i2c_read_bytes
 *
 * Purpose: Read bytes from the last addressed slave device.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    data - address to place data byte received from slave
 *    len - (in)  number of bytes to read from slave.
 *          (out) number of bytes actually read.
 *    ack_last_byte -
 *             if set, an ACK will automatically be sent by the
 *             controller for the last byte read from the slave.
 *             If not set, a NAK is pulsed when the last byte
 *             has been received. Set when a master would like
 *             to signify that this block read is NOT the last
 *             data to be read from the slave.
 *
 * Returns:
 *         SOC_E_NONE if the device was contacted and the operation
 *         succeeded.
 *         SOC_E_INTERNAL - Unexpected or internal error
 *         SOC_E_TIMEOUT if the device timed out.
 *
 * Notes:
 *       The ack_last_byte field only affects the last byte; this
 *       allows support for the following I2C byte read transactions:
 *
 *       START Addr Rd [ACK] [Data] ACK REPSTART ...
 *       START Addr Rd [ACK] [Data] NAK REPSTART ...
 *
 *       START Addr Rd [ACK] [Data] ACK [DATA] ACK STOP
 *       START Addr Rd [ACK] [Data] ACK [DATA] NAK STOP
 *
 *       We always transmit an ACK when two or more bytes
 *       remain to be read, i.e. the non-last byte(s).
 *
 *       Before this routine can be used, the I2C controller must
 *       be in a read-ready state, i.e. following a START-READ_SADDR
 *       phase or a previous read operation.
 */
int
soc_i2c_read_bytes(int unit, uint8* data, int* len, int ack_last_byte)
{
    int ack;
    uint32 rx, nread, nbytes;
    uint8* ptr;
    soc_i2c_bus_t *i2cbus;
    soc_i2c_status_t s;

    if (!len || (*len <= 0)) {
	return SOC_E_PARAM;
    }

    i2cbus = I2CBUS(unit);
    /* Make sure we're in a read-ready state. */
    if (i2cbus->opcode != SOC_I2C_RX) {
        return SOC_E_INTERNAL;
    }

    nbytes = *len;
    ptr = data;

    /* Read up to len bytes ... */
    *len = 0;
    /*
     * Some sort of start condition and slave address has been sent
     * by the I2C controller (master) and ACK'd by the slave device,
     * or one or more bytes have already been read.
     * The I2C controller is now in a state that will perform byte
     * reads until a stop or repeat start is explicitly initiated.
     * We'll only do the requested number of byte reads here, and
     * leave it up to the calling code to do the stop or start.
     */
    for (nread = 0; nread < nbytes; nread++) {
        /* ACK the byte we're about to read? */
        if (!ack_last_byte) {
	    ack = (nread == nbytes - 1 ? 0 : 1);
	} else {
	    ack = 1;
	}

	/* Initiate the next byte read. */
	soc_i2c_next_bus_phase(unit, ack);

	if (SOC_E_TIMEOUT == soc_i2c_wait(unit)) {
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "soc_i2c_read_bytes: u=%d data=0x%x state=0x%x:[%s]\n"),
                      unit, 0, soc_i2c_stat(unit),
                      soc_i2c_status_message(soc_i2c_stat(unit))));
	    return SOC_E_TIMEOUT;
	}

	/* Store the read data byte, or deal with error condition. */
	if ( ((s=soc_i2c_stat(unit)) == SOC_I2C_DATA_BYTE_RX_ACK_TX) ||
	     (s == SOC_I2C_DATA_BYTE_RX_NO_ACK_TX) ) {

	    i2cbus->opcode = SOC_I2C_RX;
	    rx = soc_i2c_pci_read(unit, CMIC_I2C_DATA);
	    rx &= CMIC_I2C_REG_MASK;

	    *ptr++ = (uint8) rx ;
	    *len = *len + 1;
	} else {
	    return SOC_E_INTERNAL;
	}
    } /* read nbytes bytes */

    return SOC_E_NONE;
}

/*
 * Function: soc_i2c_read_one_byte
 *
 * Purpose: Read one byte from the last addressed slave device,
 *          with ACK/NACK control.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    data - address to place data byte received from slave
 *    ack -  if set, an ACK will automatically be sent by the
 *           controller when the slave sends the data. If not
 *           set, a NAK is pulsed when the byte has been received.
 *           This is set when a master would like to signify
 *           that this is NOT the last byte being read from the
 *           slave.
 *
 * Returns:
 *         SOC_E_NONE if the device was contacted and the operation
 *         succeeded.
 *         SOC_E_INTERNAL - Unexpected or internal error
 *         SOC_E_TIMEOUT if the device timed out.
 *
 * Notes:
 *       Before this routine can be used, the I2C controller must
 *       be in a read-ready state, i.e. following a START-READ_SADDR
 *       phase or a previous read operation.
 */
int
soc_i2c_read_one_byte(int unit, uint8* data, int ack)
{
    int nbytes = 1;

    return soc_i2c_read_bytes(unit, data, &nbytes, ack);
}

/*
 *
 * Function: soc_i2c_read_short
 *
 * Purpose: Read two bytes, interpret as a single short value
 *          (LSbyte then MSbyte), with ACK/NACK control.
 *
 * Algorithm: [Read_LSbyte] [A] [Read_MSbyte] [ack_last_byte]
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    value - 16 bit device-specific data value to read.
 *    ack_last_byte -
 *             if set, an ACK will automatically be sent by the
 *             controller for the second byte read from the slave.
 *             If not set, a NAK is pulsed when the second byte
 *             has been received. Set when a master would like
 *             to signify that this short read is NOT the last
 *             data to be read from the slave.
 *
 * Returns:
 *
 *    SOC_E_TIMEOUT - the device can not be contacted or is offline.
 *    SOC_E_INTERNAL - Unexpected or internal error
 *    SOC_E_NONE - no error, operation succeeded.
 *
 * Notes:
 *    none
 */
int
soc_i2c_read_short(int unit, uint16* val, int ack_last_byte)
{
    uint8 a0, a1;
    uint32 rx;
    soc_i2c_bus_t	*i2cbus;
    soc_i2c_status_t s;

    i2cbus = I2CBUS(unit);
    /* Make sure we're in a read-ready state. */
    if (i2cbus->opcode != SOC_I2C_RX) {
        return SOC_E_INTERNAL;
    }

    a0 = a1 = 0;

    /* Read first byte (LSbyte); always ACK first byte. */
    soc_i2c_next_bus_phase(unit, 1);

    if (SOC_E_TIMEOUT == soc_i2c_wait(unit)) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "soc_i2c_read_short: u=%d data=0x%x state=0x%x:[%s]\n"),
                  unit, 0, soc_i2c_stat(unit),
                  soc_i2c_status_message(soc_i2c_stat(unit))));
	return SOC_E_TIMEOUT;
    }

    /*
     * Read/store the LSB, then read the MSbyte.
     * Else deal with any error condition.
     */
    if ( ((s=soc_i2c_stat(unit)) == SOC_I2C_DATA_BYTE_RX_ACK_TX) ||
	 (s == SOC_I2C_DATA_BYTE_RX_NO_ACK_TX) ) {

	rx = soc_i2c_pci_read(unit, CMIC_I2C_DATA);
	rx &= CMIC_I2C_REG_MASK;

	a0 = (uint8) rx ;

	/* Read next byte (MSbyte), setting ACK/NACK as requested. */
	soc_i2c_next_bus_phase(unit, (ack_last_byte ? 1 : 0));

	/* Wait for next state change */
	if (SOC_E_TIMEOUT == soc_i2c_wait(unit)) {
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "soc_i2c_read_short: u=%d data=0x%x state=0x%x:[%s]\n"),
                      unit, (uint32)a1, soc_i2c_stat(unit),
                      soc_i2c_status_message(soc_i2c_stat(unit))));
	    return SOC_E_TIMEOUT;
	}
    } else {
        return SOC_E_INTERNAL;
    }

    /* Read/store the MSB, or deal with error condition */
    if ( ((s=soc_i2c_stat(unit)) == SOC_I2C_DATA_BYTE_RX_ACK_TX) ||
	 (s == SOC_I2C_DATA_BYTE_RX_NO_ACK_TX) ) {
	rx = soc_i2c_pci_read(unit, CMIC_I2C_DATA);
	rx &= CMIC_I2C_REG_MASK;

	a1 = (uint8) rx ;
    } else {
        return SOC_E_INTERNAL;
    }

    *val= (a1 << 8) | a0;

    return SOC_E_NONE;
}


/*
 * Function: soc_i2c_stop
 *
 * Purpose: Generate stop condition on the I2C bus. This routine is
 *          used to signal the end of a data transfer and releases
 *          the bus according to the I2C protocol.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 * Returns:
 *     SOC_E_NONE - no error, one can always issue stop.
 * Notes:
 *     none
 */
int
soc_i2c_stop(int unit)
{
    I2CBUS(unit)->opcode = SOC_I2C_STOP;
    soc_write_i2c_stop_bits(unit);
    return SOC_E_NONE;
}

/*
 * Function: soc_i2c_ack_poll
 *
 * Purpose: Many devices will require a polling acknowledge cycle to
 *          determine if the device is available for IO. Usually, this
 *          means that a START condition is generated, along with a read
 *          or write form of the slave device address, and then we wait
 *          until the device responds with an ACK. When this occurs, we
 *          issue a STOP, to free the bus, and return since the device
 *          is ready for IO.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    bus_addr - device bus address byte, with r/w bit set for data direction.
 *    max_polls - number of times to attempt the operation.
 *
 * Returns:
 *     Number of Poll operations required to contact device, or
 *     maxpolls if the device is not online or responding.
 *
 * Notes: Typically, we poll a specified IO address with the
 *        read/write bit set to determine if the device is ready
 *        for reading or writing. When we receive an ACK for that
 *        function (Read/Write), the device is ready for IO. The
 *        data direction (or function) is determined by the address
 *        bits (see SOC_I2C_TX_ADDR/SOC_I2C_RX_ADDR) macros.
 */
int
soc_i2c_ack_poll(int unit, i2c_bus_addr_t bus_addr, int maxpolls)
{
    int i = maxpolls;

    while (--i > 0 && (soc_i2c_start(unit, bus_addr) < 0)) {
	/* NOP */
    }
    soc_i2c_stop(unit);
    return maxpolls - i;
}

/*
 * Data: soc_i2c_message[]
 * Purpose: Human-readable status messages for the I2C bus controller.
 *
 * Notes: there is a one-one-correspondence between the order of
 *        these message definitions here and the stat register
 *        meaning definitions in i2c.h (do not re-order).
 */
STATIC struct i2c_status_info{
    soc_i2c_status_t status;
    char *msg;
} soc_i2c_message[] = {
    {SOC_I2C_BERR,
     "Bus Error"},
    {SOC_I2C_START_TX,
     "START Condition Transmitted"},
    {SOC_I2C_REP_START_TX,
     "Repeated START Condition Transmitted"},
    {SOC_I2C_ADDR_WR_BIT_TX_ACK_RX,
     "Address and Write Bit Transmitted, ACK Received"},
    {SOC_I2C_ADDR_WR_BIT_TX_NO_ACK_RX,
     "Address and Write Bit Transmitted, NO ACK received"},
    {SOC_I2C_DATA_BYTE_TX_ACK_RX,
     "Data Byte Transmitted, ACK Received"},
    {SOC_I2C_DATA_BYTE_TX_NO_ACK_RX,
     "Data Byte Transmitted, NO ACK Received"},
    {SOC_I2C_ARB_LOST,
     "Arbitration Lost in Address or Data Byte"},
    {SOC_I2C_ADDR_RD_BIT_TX_ACK_RX,
     "Address and Read Bit Transmitted, ACK Received"},
    {SOC_I2C_ADDR_RD_BIT_TX_NO_ACK_RX,
     "Address and Read Bit Transmitted, NO ACK Received"},
    {SOC_I2C_DATA_BYTE_RX_ACK_TX,
     "Data Byte Received, ACK Transmitted"},
    {SOC_I2C_DATA_BYTE_RX_NO_ACK_TX,
     "Data Byte Received, NO ACK Transmitted"},
    {SOC_I2C_SADDR_RX_WR_BIT_RX_ACK_TX,
     "Slave Address and Write Bit Received, ACK Transmitted"},
    {SOC_I2C_ARB_LOST_SADDR_RX_WR_BIT_RX_ACK_TX,
     "Arbitration Lost in Address Phase, Slave Address and Write"
     " Bit Received, ACK Transmitted"},
    {SOC_I2C_GC_ADDR_RX_ACK_TX,
     "General Call Address Received, ACK Transmitted"},
    {SOC_I2C_ARB_LOST_GC_ADDR_RX_ACK_TX,
     "Arbitration Lost in Address Phase, General Call Address "
     "Received, ACK Transmitted"},
    {SOC_I2C_DATA_BYTE_RX_AFTER_SADDR_RX_ACK_TX,
     "Data Byte Received after Slave Address Received, "
     "ACK Transmitted"},
    {SOC_I2C_DATA_BYTE_RX_AFTER_SADDR_RX_NO_ACK_TX,
     "Data Byte Received after Slave Address Received, "
     "NO ACK Transmitted"},
    {SOC_I2C_DATA_BYTE_RX_AFTER_GC_ADDR_RX_ACK_TX,
     "Data Byte Received after General Call Address Received,"
     " ACK Transmitted"},
    {SOC_I2C_DATA_BYTE_RX_AFTER_GC_ADDR_RX_NO_ACK_TX,
     "Data Byte Received after General Call Address Received,"
     " NO ACK Transmitted"},
    {SOC_I2C_STOP_OR_REP_START_COND_RX_IN_SLAVE_MODE,
     "STOP or Repeated START Condition Received in Slave Mode"},
    {SOC_I2C_SADDR_RX_RD_BIT_RX_ACK_TX,
     "Slave Address and Read Bit Received, ACK Transmitted"},
    {SOC_I2C_ARB_LOST_IN_ADDR_PHASE_SADDR_RX_RD_BIT_RX_ACK_TX,
     "Arbitration Lost in Address Phase, Slave Address and Read Bit"
     " Received, ACK Transmitted"},
    {SOC_I2C_SM_DATA_BYTE_TX_ACK_RX,
     "Data Byte Transmitted in Slave Mode, ACK Received"},
    {SOC_I2C_SM_DATA_BYTE_TX_NO_ACK_RX,
     "Data Byte Transmitted in Slave Mode, NO ACK Received"},
    {SOC_I2C_SM_LAST_BYTE_TX_ACK_RX,
     "Last Byte Transmitted in Slave Mode, ACK Received"},
    {SOC_I2C_2ND_ADDR_BYTE_TX_WR_BIT_TX_ACK_RX,
     "Second Address Byte and Write Bit Transmitted, ACK Received"},
    {SOC_I2C_2ND_ADDR_BYTE_TX_WR_BIT_TX_NO_ACK_RX,
     "Second Address Byte and Write Bit Transmitted, NO ACK Received"},
    {SOC_I2C_2ND_ADDR_BYTE_TX_RD_BIT_TX_ACK_RX,
     "Second Address Byte and Read Bit Transmitted, ACK Received"},
    {SOC_I2C_2ND_ADDR_BYTE_TX_RD_BIT_TX_NO_ACK_RX,
     "Second Address Byte and Read Bit Transmitted, NO AC Received"},
    {SOC_I2C_UNDEFINED,"ERROR: Undefined status code!"},
    {SOC_I2C_NO_STATUS,"No relevant status Information (IFLG=0)"}
} ;
#define num_soc_i2c_messages COUNTOF(soc_i2c_message)

/*
 * Function: soc_i2c_status_message
 *
 * Purpose: This routine decodes the current I2C bus status code and
 *          return human-readable bus status for last I2C operation
 *
 * Returns: human readable character string telling what the
 *          current status of the I2C bus is, NULL on invalid
 *          status code.
 *
 * Notes:
 *       See also: soc_i2c_stat (for getting current status value).
 */
char *
soc_i2c_status_message(soc_i2c_status_t status)
{
    int idx = status / 8;

    if (idx >= num_soc_i2c_messages) {
	return NULL;
    }
    if (status < SOC_I2C_BERR || status > SOC_I2C_NUM_STATUS_CODES) {
	return NULL;
    }
    if (status == soc_i2c_message[idx].status) {
	return soc_i2c_message[idx].msg;
    }
    return NULL;
}

/*
 * Data: i2c_freq_tab
 *
 * Purpose: Frequency Table. I2C Bus controller frequencies (k0)
 *          for all values M{0:3},N{0:2}, in_freq=50Mhz,
 *          BUS_DIVIDER = 10
 *
 * Notes:
 *   Algorithm for I2C bus clock frequency.
 *
 *  out_freq =  in_freq / ( (M_Val + 1) *
 *                         pow( 2, N_Val + 1 )) / BUS_DIVIDER;
 */
static struct freq_tab_s{
	uint8 m;
	uint8 n;
	uint32 speed;
	char* name;
} i2c_freq_tab[]={
	{ 0,	0,	2500000,	"2.50MHz"},	/* 2.50MHz */
	{ 1,	0,	1250000,	"1.25MHz"},	/* 1.25MHz */
	{ 0,	1,	1250000,	"1.25MHz"},	/* 1.25MHz */
	{ 2,	0,	833333,		"833.33kHz"},	/* 833.33KHz */
	{ 3,	0,	625000,		"625.00kHz"},	/* 625.00KHz */
	{ 1,	1,	625000,		"625.00kHz"},	/* 625.00KHz */
	{ 0,	2,	625000,		"625.00kHz"},	/* 625.00KHz */
	{ 4,	0,	500000,		"500.00kHz"},	/* 500.00KHz */
	{ 5,	0,	416666,		"416.67kHz"},	/* 416.67KHz */
	{ 2,	1,	416666,		"416.67kHz"},	/* 416.67KHz */
	{ 6,	0,	357142,		"357.14kHz"},	/* 357.14KHz */
	{ 7,	0,	312500,		"312.50kHz"},	/* 312.50KHz */
	{ 3,	1,	312500,		"312.50kHz"},	/* 312.50KHz */
	{ 1,	2,	312500,		"312.50kHz"},	/* 312.50KHz */
	{ 0,	3,	312500,		"312.50kHz"},	/* 312.50KHz */
	{ 8,	0,	277777,		"277.78kHz"},	/* 277.78KHz */
	{ 9,	0,	250000,		"250.00kHz"},	/* 250.00KHz */
	{ 4,	1,	250000,		"250.00kHz"},	/* 250.00KHz */
	{ 10,	0,	227272,		"227.27kHz"},	/* 227.27KHz */
	{ 5,	1,	208333,		"208.33kHz"},	/* 208.33KHz */
	{ 2,	2,	208333,		"208.33kHz"},	/* 208.33KHz */
	{ 11,	0,	208333,		"208.33kHz"},	/* 208.33KHz */
	{ 12,	0,	192307,		"192.31kHz"},	/* 192.31KHz */
	{ 6,	1,	178571,		"178.57kHz"},	/* 178.57KHz */
	{ 13,	0,	178571,		"178.57kHz"},	/* 178.57KHz */
	{ 14,	0,	166666,		"166.67kHz"},	/* 166.67KHz */
	{ 7,	1,	156250,		"156.25kHz"},	/* 156.25KHz */
	{ 3,	2,	156250,		"156.25kHz"},	/* 156.25KHz */
	{ 15,	0,	156250,		"156.25kHz"},	/* 156.25KHz */
	{ 1,	3,	156250,		"156.25kHz"},	/* 156.25KHz */
	{ 0,	4,	156250,		"156.25kHz"},	/* 156.25KHz */
	{ 8,	1,	138888,		"138.89kHz"},	/* 138.89KHz */
	{ 9,	1,	125000,		"125.00kHz"},	/* 125.00KHz */
	{ 4,	2,	125000,		"125.00kHz"},	/* 125.00KHz */
	{ 10,	1,	113636,		"113.64kHz"},	/* 113.64KHz */
	{ 5,	2,	104166,		"104.17kHz"},	/* 104.17KHz */
	{ 2,	3,	104166,		"104.17kHz"},	/* 104.17KHz */
	{ 11,	1,	104166,		"104.17kHz"},	/* 104.17KHz */
	{ 12,	1,	96153,		"96.15kHz"},	/* 96.15KHz */
	{ 6,	2,	89285,		"89.28kHz"},	/* 89.28KHz */
	{ 13,	1,	89285,		"89.28kHz"},	/* 89.28KHz */
	{ 14,	1,	83333,		"83.33kHz"},	/* 83.33KHz */
	{ 7,	2,	78125,		"78.12kHz"},	/* 78.12KHz */
	{ 3,	3,	78125,		"78.12kHz"},	/* 78.12KHz */
	{ 15,	1,	78125,		"78.12kHz"},	/* 78.12KHz */
	{ 1,	4,	78125,		"78.12kHz"},	/* 78.12KHz */
	{ 0,	5,	78125,		"78.12kHz"},	/* 78.12KHz */
	{ 8,	2,	69444,		"69.44kHz"},	/* 69.44KHz */
	{ 9,	2,	62500,		"62.50kHz"},	/* 62.50KHz */
	{ 4,	3,	62500,		"62.50kHz"},	/* 62.50KHz */
	{ 10,	2,	56818,		"56.82kHz"},	/* 56.82KHz */
	{ 5,	3,	52083,		"52.08kHz"},	/* 52.08KHz */
	{ 2,	4,	52083,		"52.08kHz"},	/* 52.08KHz */
	{ 11,	2,	52083,		"52.08kHz"},	/* 52.08KHz */
	{ 12,	2,	48076,		"48.08kHz"},	/* 48.08KHz */
	{ 6,	3,	44642,		"44.64kHz"},	/* 44.64KHz */
	{ 13,	2,	44642,		"44.64kHz"},	/* 44.64KHz */
	{ 14,	2,	41666,		"41.67kHz"},	/* 41.67KHz */
	{ 7,	3,	39062,		"39.06kHz"},	/* 39.06KHz */
	{ 3,	4,	39062,		"39.06kHz"},	/* 39.06KHz */
	{ 15,	2,	39062,		"39.06kHz"},	/* 39.06KHz */
	{ 1,	5,	39062,		"39.06kHz"},	/* 39.06KHz */
	{ 0,	6,	39062,		"39.06kHz"},	/* 39.06KHz */
	{ 8,	3,	34722,		"34.72kHz"},	/* 34.72KHz */
	{ 9,	3,	31250,		"31.25kHz"},	/* 31.25KHz */
	{ 4,	4,	31250,		"31.25kHz"},	/* 31.25KHz */
	{ 10,	3,	28409,		"28.41kHz"},	/* 28.41KHz */
	{ 5,	4,	26041,		"26.04kHz"},	/* 26.04KHz */
	{ 2,	5,	26041,		"26.04kHz"},	/* 26.04KHz */
	{ 11,	3,	26041,		"26.04kHz"},	/* 26.04KHz */
	{ 12,	3,	24038,		"24.04kHz"},	/* 24.04KHz */
	{ 6,	4,	22321,		"22.32kHz"},	/* 22.32KHz */
	{ 13,	3,	22321,		"22.32kHz"},	/* 22.32KHz */
	{ 14,	3,	20833,		"20.83kHz"},	/* 20.83KHz */
	{ 7,	4,	19531,		"19.53kHz"},	/* 19.53KHz */
	{ 3,	5,	19531,		"19.53kHz"},	/* 19.53KHz */
	{ 15,	3,	19531,		"19.53kHz"},	/* 19.53KHz */
	{ 1,	6,	19531,		"19.53kHz"},	/* 19.53KHz */
	{ 0,	7,	19531,		"19.53kHz"},	/* 19.53KHz */
	{ 8,	4,	17361,		"17.36kHz"},	/* 17.36KHz */
	{ 9,	4,	15625,		"15.62kHz"},	/* 15.62KHz */
	{ 4,	5,	15625,		"15.62kHz"},	/* 15.62KHz */
	{ 10,	4,	14204,		"14.20kHz"},	/* 14.20KHz */
	{ 5,	5,	13020,		"13.02kHz"},	/* 13.02KHz */
	{ 2,	6,	13020,		"13.02kHz"},	/* 13.02KHz */
	{ 11,	4,	13020,		"13.02kHz"},	/* 13.02KHz */
	{ 12,	4,	12019,		"12.02kHz"},	/* 12.02KHz */
	{ 6,	5,	11160,		"11.16kHz"},	/* 11.16KHz */
	{ 13,	4,	11160,		"11.16kHz"},	/* 11.16KHz */
	{ 14,	4,	10416,		"10.42kHz"},	/* 10.42KHz */
	{ 7,	5,	9765,		"9.77kHz"},	/* 9.77KHz */
	{ 3,	6,	9765,		"9.77kHz"},	/* 9.77KHz */
	{ 15,	4,	9765,		"9.77kHz"},	/* 9.77KHz */
	{ 1,	7,	9765,		"9.77kHz"},	/* 9.77KHz */
	{ 8,	5,	8680,		"8.68kHz"},	/* 8.68KHz */
	{ 9,	5,	7812,		"7.81kHz"},	/* 7.81KHz */
	{ 4,	6,	7812,		"7.81kHz"},	/* 7.81KHz */
	{ 10,	5,	7102,		"7.10kHz"},	/* 7.10KHz */
	{ 5,	6,	6510,		"6.51kHz"},	/* 6.51KHz */
	{ 2,	7,	6510,		"6.51kHz"},	/* 6.51KHz */
	{ 11,	5,	6510,		"6.51kHz"},	/* 6.51KHz */
	{ 12,	5,	6009,		"6.01kHz"},	/* 6.01KHz */
	{ 6,	6,	5580,		"5.58kHz"},	/* 5.58KHz */
	{ 13,	5,	5580,		"5.58kHz"},	/* 5.58KHz */
	{ 14,	5,	5208,		"5.21kHz"},	/* 5.21KHz */
	{ 7,	6,	4882,		"4.88kHz"},	/* 4.88KHz */
	{ 3,	7,	4882,		"4.88kHz"},	/* 4.88KHz */
	{ 15,	5,	4882,		"4.88kHz"},	/* 4.88KHz */
	{ 8,	6,	4340,		"4.34kHz"},	/* 4.34KHz */
	{ 9,	6,	3906,		"3.91kHz"},	/* 3.91KHz */
	{ 4,	7,	3906,		"3.91kHz"},	/* 3.91KHz */
	{ 10,	6,	3551,		"3.55kHz"},	/* 3.55KHz */
	{ 5,	7,	3255,		"3.25kHz"},	/* 3.25KHz */
	{ 11,	6,	3255,		"3.25kHz"},	/* 3.25KHz */
	{ 12,	6,	3004,		"3.00kHz"},	/* 3.00KHz */
	{ 6,	7,	2790,		"2.79kHz"},	/* 2.79KHz */
	{ 13,	6,	2790,		"2.79kHz"},	/* 2.79KHz */
	{ 14,	6,	2604,		"2.60kHz"},	/* 2.60KHz */
	{ 7,	7,	2441,		"2.44kHz"},	/* 2.44KHz */
	{ 15,	6,	2441,		"2.44kHz"},	/* 2.44KHz */
	{ 8,	7,	2170,		"2.17kHz"},	/* 2.17KHz */
	{ 9,	7,	1953,		"1.95kHz"},	/* 1.95KHz */
	{ 10,	7,	1775,		"1.77kHz"},	/* 1.77KHz */
	{ 11,	7,	1627,		"1.63kHz"},	/* 1.63KHz */
	{ 12,	7,	1502,		"1.50kHz"},	/* 1.50KHz */
	{ 13,	7,	1395,		"1.40kHz"},	/* 1.40KHz */
	{ 14,	7,	1302,		"1.30kHz"},	/* 1.30KHz */
	{ 15,	7,	1220,		"1.22kHz"}	/* 1.22KHz */
};
#define freqtab_sz COUNTOF(i2c_freq_tab)

/*
 * Data: i2c_xgs3_freq_tab
 *
 * Purpose: Frequency Table. I2C Bus controller frequencies (k0)
 *          for all values M{0:3},N{0:2}, in_freq=5Mhz,
 *          BUS_DIVIDER = 10
 *
 * Notes:
 *   Algorithm for I2C bus clock frequency.
 *
 *  out_freq =  in_freq / ( (M_Val + 1) *
 *                         pow( 2, N_Val + 1 )) / BUS_DIVIDER;
 */
static struct xgs3_freq_tab_s{
	uint8 m;
	uint8 n;
	uint32 speed;
	char* name;
} i2c_xgs3_freq_tab[]={
        { 0,    0,      250000,          "250.00kHz"},  /* 250.00KHz */
        { 1,    0,      125000,          "125.00kHz"},  /* 125.00KHz */
        { 0,    1,      125000,          "125.00kHz"},  /* 125.00KHz */
        { 2,    0,      83333,           "83.33kHz"},   /* 83.33KHz */
        { 3,    0,      62500,           "62.50kHz"},   /* 62.50KHz */
        { 0,    2,      62500,           "62.50kHz"},   /* 62.50KHz */
        { 4,    0,      50000,           "50.00kHz"},   /* 50.00KHz */
        { 5,    0,      41667,           "41.67kHz"},   /* 41.67KHz */
        { 2,    1,      41667,           "41.67kHz"},   /* 41.67KHz */
        { 6,    0,      35714,           "35.71kHz"},   /* 35.71KHz */
        { 7,    0,      31250,           "31.25kHz"},   /* 31.25KHz */
        { 0,    3,      31250,           "31.25kHz"},   /* 31.25KHz */
        { 3,    1,      31250,           "31.25kHz"},   /* 31.25KHz */
        { 1,    2,      31250,           "31.25kHz"},   /* 31.25KHz */
        { 8,    0,      27778,           "27.78kHz"},   /* 27.78KHz */
        { 9,    0,      25000,           "25.00kHz"},   /* 25.00KHz */
        { 4,    1,      25000,           "25.00kHz"},   /* 25.00KHz */
        { 10,   0,      22727,           "22.73kHz"},   /* 22.73KHz */
        { 2,    2,      20833,           "20.83kHz"},   /* 20.83KHz */
        { 5,    1,      20833,           "20.83kHz"},   /* 20.83KHz */
        { 11,   0,      20833,           "20.83kHz"},   /* 20.83KHz */
        { 12,   0,      19231,           "19.23kHz"},   /* 19.23KHz */
        { 13,   0,      17857,           "17.86kHz"},   /* 17.86KHz */
        { 6,    1,      17857,           "17.86kHz"},   /* 17.86KHz */
        { 14,   0,      16667,           "16.67kHz"},   /* 16.67KHz */
        { 15,   0,      15625,           "15.63kHz"},   /* 15.63KHz */
        { 0,    4,      15625,           "15.63kHz"},   /* 15.63KHz */
        { 3,    2,      15625,           "15.63kHz"},   /* 15.63KHz */
        { 7,    1,      15625,           "15.63kHz"},   /* 15.63KHz */
        { 1,    3,      15625,           "15.63kHz"},   /* 15.63KHz */
        { 16,   0,      14706,           "14.71kHz"},   /* 14.71KHz */
        { 17,   0,      13889,           "13.89kHz"},   /* 13.89KHz */
        { 8,    1,      13889,           "13.89kHz"},   /* 13.89KHz */
        { 18,   0,      13158,           "13.16kHz"},   /* 13.16KHz */
        { 19,   0,      12500,           "12.50kHz"},   /* 12.50KHz */
        { 4,    2,      12500,           "12.50kHz"},   /* 12.50KHz */
        { 9,    1,      12500,           "12.50kHz"},   /* 12.50KHz */
        { 20,   0,      11905,           "11.91kHz"},   /* 11.91KHz */
        { 21,   0,      11364,           "11.36kHz"},   /* 11.36KHz */
        { 10,   1,      11364,           "11.36kHz"},   /* 11.36KHz */
        { 22,   0,      10870,           "10.87kHz"},   /* 10.87KHz */
        { 23,   0,      10417,           "10.42kHz"},   /* 10.42KHz */
        { 2,    3,      10417,           "10.42kHz"},   /* 10.42KHz */
        { 5,    2,      10417,           "10.42kHz"},   /* 10.42KHz */
        { 11,   1,      10417,           "10.42kHz"},   /* 10.42KHz */
        { 24,   0,      10000,           "10.00kHz"},   /* 10.00KHz */
        { 25,   0,      9615,            "9.62kHz"},    /* 9.62KHz */
        { 12,   1,      9615,            "9.62kHz"},    /* 9.62KHz */
        { 26,   0,      9259,            "8.93kHz"},    /* 8.93KHz */
        { 27,   0,      8929,            "8.93kHz"},    /* 8.93KHz */
        { 13,   1,      8929,            "8.93kHz"},    /* 8.93KHz */
        { 6,    2,      8929,            "8.93kHz"},    /* 8.93KHz */
        { 28,   0,      8621,            "8.62kHz"},    /* 8.62KHz */
        { 29,   0,      8333,            "8.33kHz"},    /* 8.33KHz */
        { 30,   0,      8065,            "8.07kHz"},    /* 8.07KHz */
        { 31,   0,      7813,            "7.81kHz"},    /* 7.81KHz */
        { 0,    5,      7813,            "7.81kHz"},    /* 7.81KHz */
        { 3,    3,      7813,            "7.81kHz"},    /* 7.81KHz */
        { 7,    2,      7813,            "7.81kHz"},    /* 7.81KHz */
        { 1,    4,      7813,            "7.81kHz"},    /* 7.81KHz */
        { 32,   0,      7576,            "7.58kHz"},    /* 7.58KHz */
        { 33,   0,      7353,            "7.35kHz"},    /* 7.35KHz */
        { 34,   0,      7143,            "7.14kHz"},    /* 7.14KHz */
        { 35,   0,      6944,            "6.94kHz"},    /* 6.94KHz */
        { 8,    2,      6944,            "6.94kHz"},    /* 6.94KHz */
        { 36,   0,      6757,            "6.76kHz"},    /* 6.76KHz */
        { 37,   0,      6579,            "6.58kHz"},    /* 6.58KHz */
        { 38,   0,      6410,            "6.41kHz"},    /* 6.41KHz */
        { 39,   0,      6250,            "6.25kHz"},    /* 6.25KHz */
        { 4,    3,      6250,            "6.25kHz"},    /* 6.25KHz */
        { 9,    2,      6250,            "6.25kHz"},    /* 6.25KHz */
        { 40,   0,      6098,            "6.10kHz"},    /* 6.10KHz */
        { 41,   0,      5952,            "5.95kHz"},    /* 5.95KHz */
        { 42,   0,      5814,            "5.81kHz"},    /* 5.81KHz */
        { 43,   0,      5682,            "5.68kHz"},    /* 5.68KHz */
        { 10,   2,      5682,            "5.68kHz"},    /* 5.68KHz */
        { 44,   0,      5556,            "5.56kHz"},    /* 5.56KHz */
        { 45,   0,      5435,            "5.44kHz"},    /* 5.44KHz */
        { 46,   0,      5319,            "5.32kHz"},    /* 5.32KHz */
        { 47,   0,      5208,            "5.21kHz"},    /* 5.21KHz */
        { 5,    3,      5208,            "5.21kHz"},    /* 5.21KHz */
        { 11,   2,      5208,            "5.21kHz"},    /* 5.21KHz */
        { 2,    4,      5208,            "5.21kHz"},    /* 5.21KHz */
        { 48,   0,      5102,            "5.10kHz"},    /* 5.10KHz */
        { 49,   0,      5000,            "5.00kHz"},    /* 5.00KHz */
        { 50,   0,      4902,            "4.90kHz"},    /* 4.90KHz */
        { 51,   0,      4808,            "4.81kHz"},    /* 4.81KHz */
        { 12,   2,      4808,            "4.81kHz"},    /* 4.81KHz */
        { 52,   0,      4717,            "4.72kHz"},    /* 4.72KHz */
        { 53,   0,      4630,            "4.63kHz"},    /* 4.63KHz */
        { 54,   0,      4545,            "4.55kHz"},    /* 4.55KHz */
        { 55,   0,      4464,            "4.46kHz"},    /* 4.46KHz */
        { 6,    3,      4464,            "4.46kHz"},    /* 4.46KHz */
        { 13,   2,      4464,            "4.46kHz"},    /* 4.46KHz */
        { 56,   0,      4386,            "4.39kHz"},    /* 4.39KHz */
        { 57,   0,      4310,            "4.31kHz"},    /* 4.31KHz */
        { 58,   0,      4237,            "4.24kHz"},    /* 4.24KHz */
        { 59,   0,      4167,            "4.17kHz"},    /* 4.17KHz */
        { 60,   0,      4098,            "4.10kHz"},    /* 4.10KHz */
        { 61,   0,      4032,            "4.03kHz"},    /* 4.03KHz */
        { 62,   0,      3968,            "3.97kHz"},    /* 3.97KHz */
        { 63,   0,      3906,            "3.91kHz"},    /* 3.91KHz */
        { 0,    6,      3906,            "3.91kHz"},    /* 3.91KHz */
        { 3,    4,      3906,            "3.91kHz"},    /* 3.91KHz */
        { 7,    3,      3906,            "3.91kHz"},    /* 3.91KHz */
        { 8,    3,      3472,            "3.47kHz"},    /* 3.47KHz */
        { 4,    4,      3125,            "3.13kHz"},    /* 3.13KHz */
        { 9,    3,      3125,            "3.13kHz"},    /* 3.13KHz */
        { 10,   3,      2841,            "2.84kHz"},    /* 2.84KHz */
        { 2,    5,      2604,            "2.60kHz"},    /* 2.60KHz */
        { 5,    4,      2604,            "2.60kHz"},    /* 2.60KHz */
        { 11,   3,      2604,            "2.60kHz"},    /* 2.60KHz */
        { 12,   3,      2404,            "2.40kHz"},    /* 2.40KHz */
        { 13,   3,      2232,            "2.23kHz"},    /* 2.23KHz */
        { 6,    4,      2232,            "2.23kHz"},    /* 2.23KHz */
        { 0,    7,      1953,            "1.95kHz"},    /* 1.95KHz */
        { 7,    4,      1953,            "1.95kHz"},    /* 1.95KHz */
        { 3,    5,      1953,            "1.95kHz"},    /* 1.95KHz */
        { 8,    4,      1736,            "1.74kHz"},    /* 1.74KHz */
        { 4,    5,      1563,            "1.56kHz"},    /* 1.56KHz */
        { 9,    4,      1563,            "1.56kHz"},    /* 1.56KHz */
        { 10,   4,      1420,            "1.42kHz"},    /* 1.42KHz */
        { 5,    5,      1302,            "1.30kHz"},    /* 1.30KHz */
        { 2,    6,      1302,            "1.30kHz"},    /* 1.30KHz */
        { 11,   4,      1302,            "1.30kHz"},    /* 1.30KHz */
        { 12,   4,      1202,            "1.20kHz"},    /* 1.20KHz */
        { 13,   4,      1116,            "1.11kHz"},    /* 1.11KHz */
        { 6,    5,      1116,            "1.11kHz"}     /* 1.11KHz */
};
#define xgs3_freqtab_sz COUNTOF(i2c_xgs3_freq_tab)

/*
 * Function: soc_i2c_set_freq
 *
 * Purpose:  Set the clock control register on the I2C bus controller.
 *           The CMIC_I2C_STAT register is a read-only register for the
 *           bus status code of the last operation. The CMIC_I2C_CCR
 *           register is at the same offset, but is a write-only
 *           register, hence we must keep track of what M/N values
 *           we are using.
 *
 * Inputs:
 *    unit - StrataSwitch device number or I2C bus number
 *
 * Returns:
 *    SOC_E_PARAM on bad input value, SOC_E_NONE otherwise.
 *
 * Notes:
 *    CMIC_I2C_CCR <X,M{6:3},N{2:0}>
 */
static int
soc_i2c_set_freq(int unit)
{
    soc_i2c_bus_t *i2cbus;
    int i, size;
    uint32 speed;

    i2cbus = I2CBUS(unit);
    speed = i2cbus->frequency;

    if (bsl_check(bslLayerSoc, bslSourceI2c, bslSeverityNormal, unit)) {
	uint32 whole, decimal;
        char* pfx = NULL;

	if (speed >= 1000000) {
	    pfx = "M";
	    whole = speed / 1000000;
	    decimal = ((speed%1000000)+5000)/10000;
	} else if (speed >= 1000) {
	    pfx = "k";
	    whole = speed / 1000;
	    decimal = ((speed%1000)+5)/10;
	} else {
	    pfx = "";
	    whole = speed;
	    decimal = 0;
	}

	if (decimal == 100) {
	    whole++;
	    decimal = 0;
	}

	LOG_CLI((BSL_META_U(unit,
                            "unit %d i2c bus: attempting to set speed=%d.%02d%sHz (%d)\n"),
                 unit, whole, decimal, pfx, speed));
    }

    size = SOC_IS_XGS3_SWITCH(unit) ? xgs3_freqtab_sz : freqtab_sz;

    for (i = 0; i < size; i++) {
	if (speed >= (SOC_IS_XGS3_SWITCH(unit) ?
                      i2c_xgs3_freq_tab[i].speed : i2c_freq_tab[i].speed)) {
	    /* Use lower value (never exceed requested speed) */
	    i2cbus->m_val = SOC_IS_XGS3_SWITCH(unit) ?
                            i2c_xgs3_freq_tab[i].m : i2c_freq_tab[i].m;
	    i2cbus->n_val = SOC_IS_XGS3_SWITCH(unit) ?
                            i2c_xgs3_freq_tab[i].n : i2c_freq_tab[i].n;
	    soc_i2c_pci_write(unit, CMIC_I2C_STAT,
				(i2cbus->m_val << 3) | i2cbus->n_val);
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "unit %d i2c bus: set frequency: "
                                 " just set M=%d N=%d: %s\n"),
                      unit, i2cbus->m_val, i2cbus->n_val,
                      SOC_IS_XGS3_SWITCH(unit) ?
                      i2c_xgs3_freq_tab[i].name : i2c_freq_tab[i].name));
	    i2cbus->frequency = SOC_IS_XGS3_SWITCH(unit) ?
                                i2c_xgs3_freq_tab[i].speed : i2c_freq_tab[i].speed;
	    return SOC_E_NONE;
	}

    }

    /*
     * Here if requested speed is below the H/W's minimum.
     * In this case, use the hardware minimum even though
     * it is greater than the requested speed.
     */
    i--; /* Last value in i2c_freq_table */

    i2cbus->m_val = SOC_IS_XGS3_SWITCH(unit) ?
                    i2c_xgs3_freq_tab[i].m : i2c_freq_tab[i].m;
    i2cbus->n_val = SOC_IS_XGS3_SWITCH(unit) ?
                    i2c_xgs3_freq_tab[i].n : i2c_freq_tab[i].n;
    soc_i2c_pci_write(unit, CMIC_I2C_STAT,
		      (i2cbus->m_val << 3) | i2cbus->n_val);
    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "unit %d i2c bus: set frequency: "
                         " just set M=%d N=%d: %s\n"),
              unit, i2cbus->m_val, i2cbus->n_val,
              SOC_IS_XGS3_SWITCH(unit) ?
              i2c_xgs3_freq_tab[i].name : i2c_freq_tab[i].name));
    i2cbus->frequency = SOC_IS_XGS3_SWITCH(unit) ?
                        i2c_xgs3_freq_tab[i].speed :  i2c_freq_tab[i].speed;

    return SOC_E_NONE;
}

void
soc_i2c_show_speeds(int unit)
{
    int i;

    for (i = 0; i < freqtab_sz; i++) {
	LOG_CLI((BSL_META_U(unit,
                            "unit %d i2c bus: speed %s (CCR M=%d,N=%d) [%d]\n"),
                 unit, SOC_IS_XGS3_SWITCH(unit) ?
                 i2c_xgs3_freq_tab[i].name : i2c_freq_tab[i].name,
                 SOC_IS_XGS3_SWITCH(unit) ?
                 i2c_xgs3_freq_tab[i].m : i2c_freq_tab[i].m,
                 SOC_IS_XGS3_SWITCH(unit) ?
                 i2c_xgs3_freq_tab[i].n : i2c_freq_tab[i].n,
                 SOC_IS_XGS3_SWITCH(unit) ?
                 i2c_xgs3_freq_tab[i].speed : i2c_freq_tab[i].speed));
    }
}

