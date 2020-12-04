/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for XFP modules.
 */
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/i2c.h>
#include <shared/bsl.h>
#define XFP_PAGE_SIZE            1    /* Bytes per page */
#define XFP_DEVICE_SIZE         (256*XFP_PAGE_SIZE) /* 256 bytes */
#define XFP_ACK_RETRY_COUNT      1000  /* Number of retry polls */

/*
 * Function: xfp_ack_poll
 *
 * Purpose:  Poll the device to determine if it is ready for IO.
 *           When the chip writes data, it will become unresponsive
 *           for a period of Time Tw (Internal Write Delay). We could
 *           either wait the maximum amount of time for the device to
 *           finish it's write-cycle (10ms), or we can poll the device
 *           for a specified operation (r/w) until it responds with an
 *           ACK. This routine polls the device until it is responsive.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    bus_addr - chip IO (I2C) slave bus address byte(s)
 *
 * Returns:
 *     Number of Poll operations required to contact device, or
 *     XFP_ACK_RETRY_COUNT if the device is not online or responding.
 *
 *
 * Notes:
 *     See also: soc_i2c_ack_poll
 */
STATIC INLINE int
xfp_ack_poll(int unit, i2c_bus_addr_t bus_addr)
{
    return soc_i2c_ack_poll(unit, bus_addr, XFP_ACK_RETRY_COUNT);
}

/*
 * Function: xfp_read
 *
 * Purpose: Read len bytes of data into buffer, update len with total
 *          amount read.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    addr - XFP memory address to read from
 *    data - address of data buffer to read into
 *    len - address containing number of bytes read into data buffer (updated
 *          with number of bytes read on completion).
 *
 * Returns: data bufffer filled in with data from address, number of
 *          bytes read is updated in len field. Status code:
 *
 *          SOC_E_NONE -- no error encounter
 *          SOC_E_TIMEOUT - chip timeout or data error
 *
 * Notes:
 *         Currently uses random address byte read to initiate the read; if
 *         more than one byte of data is requested at the current address, a
 *         sequential read operation is performed.
 */
STATIC int
xfp_read(int unit, int devno,
	      uint16 addr, uint8* data, uint32 *len)
{
    int rv = SOC_E_NONE;
    uint8 saddr_r, saddr_w, a0;
    uint32 nbytes = 0;

    /* Valid address, memory and size must be provided */
    if ( ! len || ! data )
	return SOC_E_PARAM;

    I2C_LOCK(unit);

    saddr_r = SOC_I2C_RX_ADDR(soc_i2c_addr(unit, devno));
    saddr_w = SOC_I2C_TX_ADDR(soc_i2c_addr(unit, devno));

    a0 = (uint8) (addr & 0x00ff);

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "xfp_read: addr=0x%x (a0=0x%x) len=%d\n"),
              addr, a0, (int)*len));

    /* First, we put out the address which we would like to
     * read at using the SOC_I2C_TX_ADDR (saddr_w)
     */
    if ( (rv = soc_i2c_start(unit, saddr_w)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "xfp_read(%d,%d,%x,%p,%d): "
                             "failed to generate start.\n"),
                  unit, devno, addr, (void *)data, *len));
	I2C_UNLOCK(unit);
	return rv;
    }

    if( (rv = soc_i2c_write_one_byte(unit, a0)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "xfp_read(%d,%d,%x,%p,%d): "
                             "failed to send a0 byte.\n"),
                  unit, devno, addr, (void *)data, *len));

	goto error;
    }
    /* Now, we have sent the first and second word addresses,
     * we then issue a repeated start condition, followed by
     * the device's read address (note: saddr_r)
     */
    if( (rv = soc_i2c_rep_start(unit, saddr_r)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "xfp_read(%d,%d,%x,%p,%d): "
                             "failed to generate rep start.\n"),
                  unit, devno, addr, (void *)data, *len));
	goto error;
    }
    nbytes = *len;
    if ( (rv = soc_i2c_read_bytes(unit, data, (int *)&nbytes, 0) ) < 0 ) {
	goto error;
    }
    *len = nbytes;

 error:

    soc_i2c_stop(unit);

    I2C_UNLOCK(unit);
    return rv ;
}

/*
 * Function: xfp_write
 * Purpose: Write len bytes of data, return the number of bytes written.
 * Uses PAGE mode to write up to 32 bytes at a time between address
 * stages for maximum performance.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    addr - XFP memory address to write to
 *    data - address of data buffer to write from
 *    len - number of bytes to write
 *
 * Returns:
 *          SOC_E_NONE -- no error encountered
 *          SOC_E_TIMEOUT - chip timeout or data error
 *
 *
 * Notes:
 *     Uses page mode to write in 32-byte chunks, when an address
 *     which does not begin on a page boundary is provided, the write
 *     operation handles unaligned accesses by breaking up the write
 *     into one write which is not page aligned, and the remainder as
 *     page aligned accesses.
 */
STATIC int
xfp_write(int unit, int devno,
	      uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 *ptr, a0;
    uint32 b, numpages, cpage, nbytes, tbytes, caddr;
    i2c_bus_addr_t bus_addr;

    /* User must have data to write */
    if ( ! data || len <= 0 )
	return SOC_E_PARAM;

    I2C_LOCK(unit);

    /* Use PAGE mode */
    caddr = addr;
    numpages = len / XFP_PAGE_SIZE + (len % XFP_PAGE_SIZE > 0);
    ptr = data;


    tbytes = soc_i2c_device(unit, devno)->tbyte++;

    bus_addr = SOC_I2C_TX_ADDR(soc_i2c_addr(unit, devno));

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "xfp_write: addr=0x%x data=%p len=%d npages=%d\n"),
              caddr, (void *)data, (int)len, numpages));

    /* Loop over every page in buffer .. */
    for(cpage = 0; cpage < numpages; cpage++){
	/* Address not page aligned, D'Oh, can't write a full page. */
	if( (caddr % XFP_PAGE_SIZE) != 0){
        /* coverity[dead_error_begin] */
	    nbytes = XFP_PAGE_SIZE - (caddr % XFP_PAGE_SIZE);
	    len -= nbytes;
	} else {
	    /* Address is page aligned, calculate bytes to write */
	    if ( len <= XFP_PAGE_SIZE ) {
	        /* Less than a page to write */
		nbytes = len;
	    } else {
		/* Wammo, full page write */
		nbytes = XFP_PAGE_SIZE;
		len -= nbytes;
	    }
	}

	/* Construct device address bytes */
	a0 = (uint8) (caddr & 0x00ff);

	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "xfp_write: unit=%d cpage=%d START on page_addr=0x%x"
                             " nbytes=%d\n"), unit, cpage, caddr, nbytes));

	/* Generate Start, for Write address */
	if( (rv = soc_i2c_start(unit, bus_addr)) < 0){
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "xfp_write(%d,%d,%x,%d,%d): "
                                 "failed to gen start\n"),
                      unit, devno, caddr, *data, len));
	    I2C_UNLOCK(unit);
	    return rv;
	}

	/* Send LSB (a0), wait for ACK */
	if( (rv = soc_i2c_write_one_byte(unit, a0)) < 0){
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "xfp_write(%d,%d,%x,%d,%d): "
                                 "failed to send a0 byte\n"),
                      unit, devno, caddr, *data, len));
	    goto error;
	}
	/* Send up to PAGE_SIZE data bytes, wait for ACK */
	for ( b = 0; b < nbytes; b++, ptr++, caddr++ ) {
	    if ( (rv = soc_i2c_write_one_byte(unit, *ptr)) < 0){
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "xfp_write(%d,%d,%d,%d,%d): "
                                 "tx data byte error\n"),
                      unit, devno, caddr, (uint32)*ptr, b));
		goto error;
	    }
	    LOG_VERBOSE(BSL_LS_SOC_I2C,
                        (BSL_META_U(unit,
                                    "xfp_write(u=%d,id=%d,page=%d "
                                    "caddr=%d,data=0x%x,idx=%d)\n"),
                         unit, devno, cpage, caddr, (uint8)*ptr, b));

	    soc_i2c_device(unit, devno)->tbyte++;
	}

	/* Send STOP, also what we do on an error to free bus.. */
    error:

	soc_i2c_stop(unit);

	/* Acknowledge polling: When the chip enters it's
	 * own internal write-cycle, it falls off the I2C bus
	 * and does not ACK a start/address phase; hence, we
	 * attempt addressing the chip until it responds, at
	 * which point the internal write cycle has finished.
	 */
	rv = xfp_ack_poll(unit, bus_addr);
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "xfp_ack_poll: "
                             "%d address cycles for wr latency.\n"), rv));
	rv = (rv > 0 ? SOC_E_NONE: SOC_E_TIMEOUT);

    }

    I2C_UNLOCK(unit);

    if (rv >= 0) {
        return soc_i2c_device(unit, devno)->tbyte - tbytes - 1 ;
    } else {
        return (rv);
    }
}


/*
 * Function: xfp_init
 *
 * Purpose: initialize the XFP I2C interface.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    data - address of test data
 *    len - size of test data
 */
STATIC int
xfp_init(int unit, int devno, void* data, int len)
{
    soc_i2c_devdesc_set(unit, devno, "XFP");

    return SOC_E_NONE;
}

/*
 * Function: xfp_ioctl: does miscellaenous tasks.
 * Purpose: Support miscellaneous chip options.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    command - IO control operation
 *    data - address of user data
 *    len - size of test data
 *
 * Notes:
 *   Placeholder for write Protect, EEPROM Chip reset - NYI
 */
STATIC int
xfp_ioctl(int unit, int devno,
	       int command, void* data, int len)
{
    switch ( command ) {

    default:
	break;
    }
    return SOC_E_NONE;
}

/* XFP Driver callout */
i2c_driver_t _soc_i2c_xfp_driver = {
    0x0,0x0, /* System assigned bytes */
    XFP_DEVICE_TYPE,
    xfp_read,
    xfp_write,
    xfp_ioctl,
    xfp_init,
    NULL,
};
