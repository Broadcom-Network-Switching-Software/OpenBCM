/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for PCIE modules.
 */
#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/i2c.h>

#define PCIE_PAGE_SIZE            4    /* Bytes per page */
#define PCIE_DEVICE_SIZE         (256*PCIE_PAGE_SIZE) /* 256 bytes */
#define PCIE_ACK_RETRY_COUNT      1000  /* Number of retry polls */

/*
 * Function: pcie_ack_poll
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
 *     PCIE_ACK_RETRY_COUNT if the device is not online or responding.
 *
 *
 * Notes:
 *     See also: soc_i2c_ack_poll
 */
STATIC INLINE int
pcie_ack_poll(int unit, i2c_bus_addr_t bus_addr)
{
    return soc_i2c_ack_poll(unit, bus_addr, PCIE_ACK_RETRY_COUNT);
}

/*
 * Function: pcie_read
 *
 * Purpose: Read len bytes of data into buffer, update len with total
 *          amount read.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    addr - PCIE memory address to read from
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
pcie_read(int unit, int devno,
	      uint16 addr, uint8* data, uint32 *len)
{
    int rv = SOC_E_NONE;
    uint8 saddr_r, saddr_w;
    uint16 a0;
    uint32 nbytes = 0;

    /* Valid address, memory and size must be provided */
    if ( ! len || ! data )
	return SOC_E_PARAM;

    I2C_LOCK(unit);

    saddr_r = SOC_I2C_RX_ADDR(soc_i2c_addr(unit, devno));
    saddr_w = SOC_I2C_TX_ADDR(soc_i2c_addr(unit, devno));

    a0 = (uint16) (addr & 0xFFff);

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "pcie_read: addr=0x%x (a0=0x%x) len=%d\n"),
              addr, a0, (int)*len));

    /* First, we put out the address which we would like to
     * read at using the SOC_I2C_TX_ADDR (saddr_w)
     */
    if ( (rv = soc_i2c_start(unit, saddr_w)) < 0) {
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "pcie_read(%d,%d,%x,%p,%d): "
                             "failed to generate start.\n"),
                  unit, devno, addr, (void *)data, *len));
	I2C_UNLOCK(unit);
	return rv;
    }

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "pcie_read(%d,%d,%x,%p,%d): "
                         "to send a0 byte.\n"),
              unit, devno, addr, (void *)data, *len));
    if( (rv = soc_i2c_write_one_byte(unit, ((a0 >> 8) & 0xFF))) < 0) {
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "pcie_read(%d,%d,%x,%p,%d): "
                             "failed to send a0 byte.\n"),
                  unit, devno, addr, (void *)data, *len));

	goto error;
    }

    if( (rv = soc_i2c_write_one_byte(unit, a0 & 0xFF)) < 0) {
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "pcie_read(%d,%d,%04x,%x,%p,%d): "
                             "failed to send a0 byte.\n"),
                  unit, devno, addr, a0, (void *)data, *len));

	goto error;
    }
    /* Now, we have sent the first and second word addresses,
     * we then issue a repeated start condition, followed by
     * the device's read address (note: saddr_r)
     */
    if( (rv = soc_i2c_rep_start(unit, saddr_r)) < 0) {
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "pcie_read(%d,%d,%x,%p,%d): "
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
 * Function: pcie_write
 * Purpose: Write len bytes of data, return the number of bytes written.
 * Uses PAGE mode to write up to 32 bytes at a time between address
 * stages for maximum performance.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    addr - PCIE memory address to write to
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
pcie_write(int unit, int devno,
	      uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 *ptr;
    uint16 a0;
    uint32 b, numpages, cpage, nbytes, tbytes, caddr;
    i2c_bus_addr_t bus_addr;

    /* User must have data to write */
    if ( ! data || len <= 0 )
	return SOC_E_PARAM;

    I2C_LOCK(unit);

    /* Use PAGE mode */
    caddr = addr;
    numpages = len / PCIE_PAGE_SIZE + (len % PCIE_PAGE_SIZE > 0);
    ptr = data;


    tbytes = soc_i2c_device(unit, devno)->tbyte++;

    bus_addr = SOC_I2C_TX_ADDR(soc_i2c_addr(unit, devno));

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "pcie_write: addr=0x%x data=%p len=%d npages=%d\n"),
              caddr, (void *)data, (int)len, numpages));

    /* Loop over every page in buffer .. */
    for(cpage = 0; cpage < numpages; cpage++){
	/* Address not page aligned, D'Oh, can't write a full page. */
	if( (caddr % PCIE_PAGE_SIZE) != 0){
	    nbytes = PCIE_PAGE_SIZE - (caddr % PCIE_PAGE_SIZE);
	    len -= nbytes;
	} else {
	    /* Address is page aligned, calculate bytes to write */
	    if ( len <= PCIE_PAGE_SIZE ) {
	        /* Less than a page to write */
		nbytes = len;
	    } else {
		/* Wammo, full page write */
		nbytes = PCIE_PAGE_SIZE;
		len -= nbytes;
	    }
	}

	/* Construct device address bytes */
	a0 = (uint16) (caddr & 0xFFff);


	/* Generate Start, for Write address */
	if( (rv = soc_i2c_start(unit, bus_addr)) < 0){
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "pcie_write(%d,%d,%x,%d,%d): "
                                 "failed to gen start\n"),
                      unit, devno, caddr, *data, len));
	    I2C_UNLOCK(unit);
	    return rv;
	}
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "pcie_write: unit=%d cpage=%d START on page_addr=0x%x"
                             " nbytes=%d\n"), unit, cpage, ((a0 >> 8) & 0xFF) , nbytes));
        if( (rv = soc_i2c_write_one_byte(unit, ((a0 >> 8) & 0xFF))) < 0) {
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "pcie_write(%d,%d,%x,%p,%d): "
                                 "failed to send a0 byte.\n"),
                      unit, devno, addr, (void *)data, len));

            goto error;
        }

	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "pcie_write: unit=%d cpage=%d START on page_addr=0x%x"
                             " nbytes=%d\n"), unit, cpage, ((a0) & 0xFF) , nbytes));
        if( (rv = soc_i2c_write_one_byte(unit, a0 & 0xFF)) < 0) {
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "pcie_write(%d,%d,%04x,%x,%p,%d): "
                                 "failed to send a0 byte.\n"),
                      unit, devno, addr, a0, (void *)data, len));

            goto error;
        }
	/* Send up to PAGE_SIZE data bytes, wait for ACK */
	for ( b = 0; b < nbytes; b++, ptr++, caddr++ ) {
	    if ( (rv = soc_i2c_write_one_byte(unit, *ptr)) < 0){
		LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "pcie_write(%d,%d,%d,%d,%d): "
                                     "tx data byte error\n"),
                          unit, devno, caddr, (uint32)*ptr, b));
		goto error;
	    }
	    LOG_VERBOSE(BSL_LS_SOC_I2C,
                        (BSL_META_U(unit,
                                    "pcie_write(u=%d,id=%d,page=%d "
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
	rv = pcie_ack_poll(unit, bus_addr);
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "pcie_ack_poll: "
                             "%d address cycles for wr latency.\n"), rv));
	rv = (rv > 0 ? SOC_E_NONE: SOC_E_TIMEOUT);

    }

    I2C_UNLOCK(unit);

    return rv < 0 ? rv : soc_i2c_device(unit, devno)->tbyte - tbytes - 1 ;
}


/*
 * Function: pcie_init
 *
 * Purpose: initialize the PCIE I2C interface.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    data - address of test data
 *    len - size of test data
 */
STATIC int
pcie_init(int unit, int devno, void* data, int len)
{
    soc_i2c_devdesc_set(unit, devno, "PCIE");

    return SOC_E_NONE;
}

/*
 * Function: pcie_ioctl: does miscellaenous tasks.
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
pcie_ioctl(int unit, int devno,
	       int command, void* data, int len)
{
    switch ( command ) {

    default:
	break;
    }
    return SOC_E_NONE;
}

/* PCIE Driver callout */
i2c_driver_t _soc_i2c_pcie_driver = {
    0x0,0x0, /* System assigned bytes */
    PCIE_DEVICE_TYPE,
    pcie_read,
    pcie_write,
    pcie_ioctl,
    pcie_init,
    NULL,
};
