/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * I2C Driver for CXP/QSFP device
 *  Derived from XFP driver, more ioctls supported
 *  Limited to page 0 
 */
#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/i2c.h>
#include <shared/bsl.h>

#define CXP_PAGE_SIZE            1    /* Bytes per page */
#define CXP_DEVICE_SIZE         (256*CXP_PAGE_SIZE) /* 256 bytes */
#define CXP_ACK_RETRY_COUNT      1000  /* Number of retry polls */

#define CXP_TX_LOWER_PAGE     0
#define CXP_TX_UPPER_PAGE_0   1
#define CXP_TX_UPPER_PAGE_1   2
#define CXP_RX_LOWER_PAGE     3
#define CXP_RX_UPPER_PAGE_0   4
#define CXP_RX_UPPER_PAGE_1   5

#define CXP_RX_PAGE_MASK     0x40
#define CXP_UPPER_PAGE_CTRL  0x80

/*
 * Function: cxp_ack_poll
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
 *     CXP_ACK_RETRY_COUNT if the device is not online or responding.
 *
 *
 * Notes:
 *     See also: soc_i2c_ack_poll
 */
STATIC INLINE int
cxp_ack_poll(int unit, i2c_bus_addr_t bus_addr)
{
    return soc_i2c_ack_poll(unit, bus_addr, CXP_ACK_RETRY_COUNT);
}

STATIC int
_cxp_read(int unit, uint8 saddr,
	      uint16 addr, uint8* data, uint32 *len)
{
    int rv = SOC_E_NONE;
    uint8 saddr_r;
    uint32 nbytes = 0;
    uint8 saddr_w, a0;

    /* Valid address, memory and size must be provided */
    if ( ! len || ! data )
	return SOC_E_PARAM;

    I2C_LOCK(unit);

    saddr_r = SOC_I2C_RX_ADDR(saddr);
    saddr_w = SOC_I2C_TX_ADDR(saddr);

    a0 = (uint8) (addr & 0x00ff);

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "cxp_read: addr=0x%x (a0=0x%x) len=%d\n"),
              addr, a0, (int)*len));

    /* First, we put out the address which we would like to
     * read at using the SOC_I2C_TX_ADDR (saddr_w)
     */
    if ( (rv = soc_i2c_start(unit, saddr_w)) < 0) {
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "cxp_read(%d,%x,%x,%p,%d): "
                             "failed to generate start.\n"),
                  unit, saddr_w, addr, (void *)data, *len));
	I2C_UNLOCK(unit);
	return rv;
    }

    if( (rv = soc_i2c_write_one_byte(unit, a0)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "cxp_read(%d,%x,%x,%p,%d): "
                             "failed to send a0 byte.\n"),
                  unit, saddr_w, addr, (void *)data, *len));

	goto error;
    }
    /* Now, we have sent the first and second word addresses,
     * we then issue a repeated start condition, followed by
     * the device's read address (note: saddr_r)
     */
    if( (rv = soc_i2c_rep_start(unit, saddr_r)) < 0) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "cxp_read(%d,%x,%x,%p,%d): "
                             "failed to generate rep start.\n"),
                  unit, saddr_r, addr, (void *)data, *len));
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
 * Function: cxp_read
 *
 * Purpose: Read len bytes of data into buffer, update len with total
 *          amount read.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    addr - CXP memory address to read from
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
cxp_read(int unit, int devno,
	      uint16 addr, uint8* data, uint32 *len)
{
    int rv;
    uint8 saddr;

    saddr = soc_i2c_addr(unit, devno);

    rv = _cxp_read(unit, saddr, addr, data, len);
    if (SOC_SUCCESS(rv)) {
        soc_i2c_device(unit, devno)->rbyte += *len;
    }
    return rv;
}

/*
 * Function: cxp_write
 * Purpose: Write len bytes of data, return the number of bytes written.
 * Uses PAGE mode to write up to 32 bytes at a time between address
 * stages for maximum performance.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    addr - CXP memory address to write to
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
_cxp_write(int unit, uint8 saddr , uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    i2c_bus_addr_t bus_addr;
    uint32 b, numpages, cpage, nbytes, caddr;
    uint8 *ptr, a0;

    /* User must have data to write */
    if ( ! data || len <= 0 )
	return SOC_E_PARAM;

    I2C_LOCK(unit);

    /* Use PAGE mode */
    caddr = addr;
    numpages = len / CXP_PAGE_SIZE + (len % CXP_PAGE_SIZE > 0);
    ptr = data;

    bus_addr = SOC_I2C_TX_ADDR(saddr);

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "cxp_write: addr=0x%x data=%p len=%d npages=%d\n"),
              caddr, (void *)data, (int)len, numpages));

    /* Loop over every page in buffer .. */
    for(cpage = 0; cpage < numpages; cpage++){
	/* Address not page aligned, D'Oh, can't write a full page. */
	if( (caddr % CXP_PAGE_SIZE) != 0){
        /* coverity[dead_error_begin] */
	    nbytes = CXP_PAGE_SIZE - (caddr % CXP_PAGE_SIZE);
	    len -= nbytes;
	} else {
	    /* Address is page aligned, calculate bytes to write */
	    if ( len <= CXP_PAGE_SIZE ) {
	        /* Less than a page to write */
		nbytes = len;
	    } else {
		/* Wammo, full page write */
		nbytes = CXP_PAGE_SIZE;
		len -= nbytes;
	    }
	}

	/* Construct device address bytes */
	a0 = (uint8) (caddr & 0x00ff);

	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "cxp_write: unit=%d cpage=%d START on page_addr=0x%x"
                             " nbytes=%d\n"), unit, cpage, caddr, nbytes));

	/* Generate Start, for Write address */
	if( (rv = soc_i2c_start(unit, bus_addr)) < 0){
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "cxp_write(%d,%x,%x,%d,%d): "
                                 "failed to gen start\n"),
                      unit, bus_addr, caddr, *data, len));
	    I2C_UNLOCK(unit);
	    return rv;
	}

	/* Send LSB (a0), wait for ACK */
	if( (rv = soc_i2c_write_one_byte(unit, a0)) < 0){
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "cxp_write(%d,%x,%x,%d,%d): "
                                 "failed to send a0 byte\n"),
                      unit, bus_addr, caddr, *data, len));
	    goto error;
	}
	/* Send up to PAGE_SIZE data bytes, wait for ACK */
	for ( b = 0; b < nbytes; b++, ptr++, caddr++ ) {
	    if ( (rv = soc_i2c_write_one_byte(unit, *ptr)) < 0){
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "cxp_write(%d,%x,%d,%d,%d): "
                                 "tx data byte error\n"),
                      unit, bus_addr, caddr, (uint32)*ptr, b));
		goto error;
	    }
	    LOG_VERBOSE(BSL_LS_SOC_I2C,
                        (BSL_META_U(unit,
                                    "cxp_write(u=%d,addr=%x,page=%d "
                                    "caddr=%d,data=0x%x,idx=%d)\n"),
                         unit, bus_addr, cpage, caddr, (uint8)*ptr, b));

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
	rv = cxp_ack_poll(unit, bus_addr);
	LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "cxp_ack_poll: "
                             "%d address cycles for wr latency.\n"), rv));
	rv = (rv > 0 ? SOC_E_NONE: SOC_E_TIMEOUT);

    }

    I2C_UNLOCK(unit);
    return (rv);
}

STATIC int
cxp_write(int unit, int devno, 
	      uint16 addr, uint8* data, uint32 len)
{
    uint8 saddr;
    int rv;

    saddr = soc_i2c_addr(unit, devno);

    rv = _cxp_write(unit, saddr, addr, data, len);
    if (SOC_SUCCESS(rv)) {
        soc_i2c_device(unit, devno)->tbyte += len;
    }
    return rv;
}


/*
 * Function: cxp_ioctl: does miscellaenous tasks.
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
cxp_ioctl(int unit, int devno,
	       int command, void* data, int wlen)
{
    int offset = 0, page = 0;
    uint32 len = 1;
    int op = 0; /* read */
    uint8 saddr;
    uint8 cxp_dev = 1;

    saddr = soc_i2c_addr(unit, devno);

    _cxp_read(unit, saddr, 128, (uint8*)&cxp_dev, &len);
    switch ( command ) {
    case I2C_CXP_VENDOR_NAME:
        page = 0;
        if (cxp_dev == 0xc) { offset = 148; }
        else if (cxp_dev < 0xc) { offset = 20; }
        else { offset = 152; page = CXP_TX_UPPER_PAGE_0;}
        len = 16;
        break;
    case I2C_CXP_VENDOR_OUI:
        offset = 168;
        page = CXP_TX_UPPER_PAGE_0;
        len = 3;
        break;
    case I2C_CXP_VENDOR_PART_NUM:
        page = 0;
        if (cxp_dev == 0xc) { offset = 162; }
        else if (cxp_dev < 0xc) { offset = 40; }
        else { offset = 171; page = CXP_TX_UPPER_PAGE_0;}
        len = 16;
        break;
    case I2C_CXP_VENDOR_REV_ID:
        offset = 187;
        page = CXP_TX_UPPER_PAGE_0;
        len = 2;
        break;
    case I2C_CXP_VENDOR_SERIAL:
        offset = 189;
        page = CXP_TX_UPPER_PAGE_0;
        len = 16;
        break;
    case I2C_CXP_VENDOR_DATE:
        offset = 205;
        page = CXP_TX_UPPER_PAGE_0;
        len = 8;
        break;
    case I2C_CXP_VENDOR_LOT:
        offset = 213;
        page = CXP_TX_UPPER_PAGE_0;
        len = 10;
        break;
    case I2C_CXP_TX_CAPABILITY:
        offset = 142;
        page = CXP_TX_UPPER_PAGE_0;
        len = 2;
        break;
    case I2C_CXP_RX_CAPABILITY:
        offset = 144 ;
        page = CXP_RX_UPPER_PAGE_0;
        len = 2;
        break;
    case I2C_CXP_RX_STATUS:
        page = CXP_RX_LOWER_PAGE;
        offset = 6;
        len = 1;
        break;
    case I2C_CXP_RX_LOS_STATUS:
        /* returns both LOS and FAULT */
        page = CXP_RX_LOWER_PAGE;
        offset = 7;
        len = 4;
        break;
    case I2C_CXP_RX_CHANNEL_CTRL:
        page = CXP_RX_LOWER_PAGE;
        offset = 52;
        len = 2;
        break;
    case I2C_CXP_RX_OUTPUT_CTRL:
        page = CXP_RX_LOWER_PAGE;
        offset = 54;
        len = 2;
        op = 1;
        break;
    case I2C_CXP_RX_OUTPUT_STATUS:
        page = CXP_RX_LOWER_PAGE;
        offset = 54;
        len = 2;
        break;
    case I2C_CXP_RX_LOS_MASK:
        page = CXP_RX_LOWER_PAGE;
        offset = 95;
        len = 2;
        break;

    case I2C_CXP_TX_STATUS:
        page = CXP_TX_LOWER_PAGE;
        offset = 6;
        len = 1;
        break;
    case I2C_CXP_TX_LOS_STATUS:
        /* returns both LOS and FAULT */
        page = CXP_TX_LOWER_PAGE;
        offset = 7;
        len = 4;
        break;
    case I2C_CXP_TX_CHANNEL_CTRL:
        page = CXP_TX_LOWER_PAGE;
        offset = 52;
        len = 2;
        break;
    case I2C_CXP_TX_OUTPUT_CTRL:
        page = CXP_TX_LOWER_PAGE;
        offset = 54;
        len = 2;
        op = 1;
        break;
    case I2C_CXP_TX_OUTPUT_STATUS:
        page = CXP_TX_LOWER_PAGE;
        offset = 54;
        len = 2;
        break;
    case I2C_CXP_TX_LOS_MASK:
        page = CXP_TX_LOWER_PAGE;
        offset = 95;
        len = 2;
        break;
    default:
	break;
    }
    saddr = soc_i2c_addr(unit, devno);
    if (page >= CXP_RX_LOWER_PAGE) {
        saddr |= CXP_RX_PAGE_MASK;
    }
    if (op) {
        _cxp_write(unit, saddr, offset, (uint8*)data, len);
    } else {
        _cxp_read(unit, saddr, offset, (uint8*)data, &len);
    }
    return SOC_E_NONE;
}

/*
 * Function: cxp_init
 *
 * Purpose: initialize the CXP I2C interface.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    data - address of test data
 *    len - size of test data
 */
STATIC int
cxp_init(int unit, int devno, void* data, int len)
{
    uint8 buffer[256];
    sal_memset(buffer, 0, 256);
    cxp_ioctl(unit, devno, I2C_CXP_VENDOR_NAME, buffer, 256);
    LOG_VERBOSE(BSL_LS_SOC_I2C,
                (BSL_META_U(unit,
                            "\nCXP device: Vendor name: %s"), buffer));
    sal_memset(buffer, 0, 256);
    cxp_ioctl(unit, devno, I2C_CXP_VENDOR_PART_NUM, buffer, 256);
    LOG_VERBOSE(BSL_LS_SOC_I2C,
                (BSL_META_U(unit,
                            "Part: %s detected \n"), buffer));

    soc_i2c_devdesc_set(unit, devno, "Cxp/Sfp/Qsfp Driver");

    return SOC_E_NONE;
}


/* CXP Driver callout */
i2c_driver_t _soc_i2c_cxp_driver = {
    0x0,0x0, /* System assigned bytes */
    CXP_DEVICE_TYPE,
    cxp_read,
    cxp_write,
    cxp_ioctl,
    cxp_init,
    NULL,
};
