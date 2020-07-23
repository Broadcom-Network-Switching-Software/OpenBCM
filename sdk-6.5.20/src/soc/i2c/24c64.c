/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM56xx I2C Device Driver for ATMEL 24LC64 EEPROM
 *
 * The AT24C32/64 provides for 32,768 or 65,536 bits of electrically
 * eraseable programmable read only memory (EEPROM) organized as
 * 4096/8192 bytes. The AT24C32/64 chips are internally organized 
 * as 128/256 pages of 32 bytes each, hence a page addressing mode
 * is available.
 * The top quadrant (1024/2048 bytes) can be write protected. This is
 * where the Board Information Structure (BIS) is stored; it is not
 * subject to any destructive memory integrity tests.
 *
 * See also: AT24C64 Data Sheet
 */
#include <sal/types.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/i2c.h>
#include <soc/iproc.h>
#include <soc/cm.h>
#include <shared/bsl.h>
#define LC2464_WRITE_CYCLE_DELAY   (10*MILLISECOND_USEC) /* 10 ms MAX */
#define LC2464_PAGE_SIZE            32    /* Bytes per page */
#define LC2464_DEVICE_SIZE         (8192) /* AT24C64 */
#define LC2464_DEVICE_RW_SIZE      (((LC2464_DEVICE_SIZE)*3)/4)
#define LC2464_ACK_RETRY_COUNT      1000  /* Number of retry polls */
#define LC2464_NAME_LEN            6





typedef struct eep24c64_s{
    uint16 size;   /* Number of Bytes */
    uint16 type;   /* Reserved */
    char name[LC2464_NAME_LEN];  /* Device name */
    uint16 chksum; /* 16 bit checksum of size, type, and name */
} eep24c64_t;

/*
 * Store EEPROM data at start of prom, make all
 * drivers read/write beyond that. This basically
 * serves as a way of signing the EEPROM with data
 * so that we know the device is good and has been
 * validated.
 */
#define LC24C64_PARAMS_SIZE (sizeof(eep24c64_t))
#define LC24C64_DATA_START   LC24C64_PARAMS_SIZE

/*
 * Function: eep24c64_ack_poll
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
 *     LC24C64_ACK_RETRY_COUNT if the device is not online or responding.
 *
 *
 * Notes:
 *     See also: soc_i2c_ack_poll
 */
STATIC INLINE int
eep24c64_ack_poll(int unit, i2c_bus_addr_t bus_addr)
{
    return soc_i2c_ack_poll(unit, bus_addr, LC2464_ACK_RETRY_COUNT);
}

/*
 * Function: eep24c64_read
 *
 * Purpose: Read len bytes of data into buffer, update len with total
 *          amount read.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    addr - NVRAM memory address to read from
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
eep24c64_read(int unit, int devno,
	      uint16 addr, uint8* data, uint32 *len)
{
    int rv = SOC_E_NONE;
    uint8 saddr_r, saddr_w, a0, a1;
    uint32 nbytes = 0;
#ifdef BCM_CMICM_SUPPORT
    uint8 rx;
    uint32 nread;
    soc_timeout_t to;
#endif
#ifdef BCM_IPROC_SUPPORT
    uint32 rval;
    uint32 i;
#endif

    /* Valid address, memory and size must be provided */
    if ( ! len || ! data )
	return SOC_E_PARAM;

    I2C_LOCK(unit);

    saddr_r = SOC_I2C_RX_ADDR(soc_i2c_addr(unit, devno));
    saddr_w = SOC_I2C_TX_ADDR(soc_i2c_addr(unit, devno));

    a0 = (uint8) (addr & 0x00ff);
    a1 = (uint8) ((addr & 0xff00) >> 8);

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "eep24c64_read: addr=0x%x (a0=0x%x,a1=0x%x) len=%d\n"),
              addr, a0, a1, (int)*len));

#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_eeprom_iproc)) {
        rval = (uint32)saddr_w;
        WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr(unit, rval);
        rval = (uint32)a1;
        WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr(unit, rval);
        rval = (uint32)a0;
        WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr(unit, rval);

        nbytes = *len;
        *len = 0;
        for (i = 0; i < nbytes; i++) {
            rval = (uint32)saddr_r;
            soc_reg_field_set(unit, CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr, &rval,
                              MASTER_WR_STATUSf, 1);
            WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr(unit, rval);
            rval = 0;
            soc_reg_field_set(unit, CHIPCOMMONG_SMBUS0_SMBUS_MASTER_COMMANDr, &rval,
                              SMBUS_PROTOCOLf, SMBUS_BLOCK_PROCESS_CALL);
            soc_reg_field_set(unit, CHIPCOMMONG_SMBUS0_SMBUS_MASTER_COMMANDr, &rval,
                              RD_BYTE_COUNTf, 1);
            WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_COMMANDr(unit, rval);

            rv = iproc_smbus_start_wait(unit);
            if (rv == SOC_E_NONE) {
                READ_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_READr(unit, &rval);
                data[i] = (uint8)(rval & 0xFF);
                *len = *len + 1;
            } else {
                LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "eep24c64_read(%d,%d,%x,%p,%d): "
                                     "failed to read.\n"),
                         unit, devno, addr, (void *)data, *len));
                I2C_UNLOCK(unit);
                return rv;
            }
        }
    } else
#endif /* BCM_IPROC_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm) && !SOC_IS_SAND(unit)) {
       rv = soc_i2c_write_word(unit, soc_i2c_addr(unit, devno), addr);
        nbytes = *len;
        *len = 0;
        if (rv == SOC_E_NONE) {
            for (nread = 0; nread < nbytes; nread++) {
                rv = soc_i2c_read_byte(unit, soc_i2c_addr(unit, devno), &rx);
                if(rv != SOC_E_NONE) {
                    if (nread == 0) {
                        /* Might be the situation that the previous write is 
                         * under it's internal write-cycle. The max write-cycle 
                         * time could reach to 5ms.
                         */
                        soc_timeout_init(&to, 5000, 3); 
                        do {
                            /* coverity[callee_ptr_arith: FALSE] */
                            rv = soc_i2c_read_byte(unit, soc_i2c_addr(unit, devno), &rx);
                            if (rv == SOC_E_NONE) {
                                break;
                            }
                        } while(!(soc_timeout_check(&to)));
                    }
                    if (rv != SOC_E_NONE) {
                        I2C_UNLOCK(unit);
                        return rv;
                    }
                }
                *len = *len + 1;
                data[nread] = rx;
            } 
        }
    } else
#endif /* BCM_CMICM_SUPPORT */
    {
        /* First, we put out the address which we would like to
         * read at using the SOC_I2C_TX_ADDR (saddr_w)
         */
        if ( (rv = soc_i2c_start(unit, saddr_w)) < 0) {
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "eep24c64_read(%d,%d,%x,%p,%d): "
                                 "failed to generate start.\n"),
                      unit, devno, addr, (void *)data, *len));
            I2C_UNLOCK(unit);
            return rv;
        }
        /* Word Protocol: Address MSB */
        if ( (rv = soc_i2c_write_one_byte(unit, a1)) < 0) {
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "eep24c64_read(%d,%d,%x,%p,%d): "
                                 "failed to send a1 byte.\n"),
                      unit, devno, addr, (void *)data, *len));

            goto error;
        }
        /* Address LSB */
        if( (rv = soc_i2c_write_one_byte(unit, a0)) < 0) {
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "eep24c64_read(%d,%d,%x,%p,%d): "
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
                                 "eep24c64_read(%d,%d,%x,%p,%d): "
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
    }

    I2C_UNLOCK(unit);
    return rv ;
}

/*
 * Function: eep24c64_write
 * Purpose: Write len bytes of data, return the number of bytes written.
 * Uses PAGE mode to write up to 32 bytes at a time between address
 * stages for maximum performance. See AT24C64 data sheet for more info.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    addr - NVRAM memory address to write to
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
eep24c64_write(int unit, int devno,
	      uint16 addr, uint8* data, uint32 len)
{
    int rv = SOC_E_NONE;
    uint8 *ptr, a0, a1;
    uint32 b, numpages, cpage, nbytes, tbytes, caddr;
    i2c_bus_addr_t bus_addr;
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
    uint32 rval;
#endif

    /* User must have data to write */
    if ( ! data || len <= 0 ) {
        return SOC_E_PARAM;
    }

    I2C_LOCK(unit);

    /* Use PAGE mode */
    caddr = addr;
    numpages = 1 + (((caddr%LC2464_PAGE_SIZE)+len-1)/LC2464_PAGE_SIZE);
    ptr = data;


    tbytes = soc_i2c_device(unit, devno)->tbyte++;

    bus_addr = SOC_I2C_TX_ADDR(soc_i2c_addr(unit, devno));

    LOG_INFO(BSL_LS_SOC_I2C,
             (BSL_META_U(unit,
                         "eep24c64_write: addr=0x%x data=%p len=%d npages=%d\n"),
              caddr, (void *)data, (int)len, numpages));

    /* Loop over every page in buffer .. */
    for(cpage = 0; cpage < numpages; cpage++) {
        if( (caddr % LC2464_PAGE_SIZE) != 0){
            /* Address not page aligned, D'Oh, can't write a full page. */
            nbytes = LC2464_PAGE_SIZE - (caddr % LC2464_PAGE_SIZE);
            nbytes = (len > nbytes) ? nbytes : len;
            len -= nbytes;
        } else {
            /* Address is page aligned, calculate bytes to write */
            if ( len <= LC2464_PAGE_SIZE ) {
                /* Less than a page to write */
                nbytes = len;
            } else {
                /* Wammo, full page write */
                nbytes = LC2464_PAGE_SIZE;
                len -= nbytes;
            }
        }

        /* Construct device address bytes */
        a1 = (uint8) ((caddr & 0xff00) >> 8);
        a0 = (uint8) (caddr & 0x00ff);

        LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "eep24c64_write: unit=%d cpage=%d START on page_addr=0x%x"
                                 " nbytes=%d\n"), unit, cpage, caddr, nbytes));

#ifdef BCM_IPROC_SUPPORT
        if(soc_feature(unit, soc_feature_eeprom_iproc)) {
            /* Write Word is achieved with SMBUS_WRITE_BYTE, with LSB of data instead of the command */
            rval = SOC_I2C_TX_ADDR(soc_i2c_addr(unit, devno));
            WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr(unit, rval);
            rval = a1;
            WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr(unit, rval);
            rval = a0;
            WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr(unit, rval);

            for ( b = 0; b < nbytes; b++, ptr++, caddr++ ) {
                rval = *ptr;
                if(b == (nbytes - 1)) {
                    soc_reg_field_set(unit, CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr,
                                      &rval, MASTER_WR_STATUSf, 1); /* Last Byte */
                }
                WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_DATA_WRITEr(unit, rval);
                soc_i2c_device(unit, devno)->tbyte++;
            }

            rval = 0;
            soc_reg_field_set(unit, CHIPCOMMONG_SMBUS0_SMBUS_MASTER_COMMANDr, &rval, SMBUS_PROTOCOLf, SMBUS_BLOCK_WRITE);
            WRITE_CHIPCOMMONG_SMBUS0_SMBUS_MASTER_COMMANDr(unit, rval);
            rv = iproc_smbus_start_wait(unit);

            if (rv < 0) {
                I2C_UNLOCK(unit);
                return (rv);
            }

            /* EEPROM after write operation requested need a period of time
             * (in term of WriteCycle) to finish the programming process
             * into EEPROM. This driver implement a usleep to wait WriteCycle.
             */
            sal_usleep(5000);
        } else
#endif /* BCM_IPROC_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
        if(soc_feature(unit, soc_feature_cmicm) && !SOC_IS_SAND(unit)) {
            /* Write Word is achieved with SMBUS_WRITE_BYTE, with LSB of data instead of the command */
            rval = SOC_I2C_TX_ADDR(soc_i2c_addr(unit, devno));
            WRITE_CMIC_I2CM_SMBUS_MASTER_DATA_WRITEr(unit, rval);

            rval = a1;
            WRITE_CMIC_I2CM_SMBUS_MASTER_DATA_WRITEr(unit, rval);
            rval = a0;
            WRITE_CMIC_I2CM_SMBUS_MASTER_DATA_WRITEr(unit, rval);

            for ( b = 0; b < nbytes; b++, ptr++, caddr++ ) {
                rval = *ptr;
                if(b == (nbytes-1)) {
                    soc_reg_field_set(unit, CMIC_I2CM_SMBUS_MASTER_DATA_WRITEr, &rval, MASTER_WR_STATUSf, 1); /* Last Byte */
                }
                WRITE_CMIC_I2CM_SMBUS_MASTER_DATA_WRITEr(unit, rval);
                soc_i2c_device(unit, devno)->tbyte++;
            }

            rval = 0;
            soc_reg_field_set(unit, CMIC_I2CM_SMBUS_MASTER_COMMANDr, &rval, SMBUS_PROTOCOLf, SMBUS_BLOCK_WRITE);
            WRITE_CMIC_I2CM_SMBUS_MASTER_COMMANDr(unit,rval);
            rv = smbus_start_wait(unit);

            if (rv < 0) {
                I2C_UNLOCK(unit);
                return (rv);
            }

            /* EEPROM after write operation requested need a period of time 
             * (in term of WriteCycle) to finish the programming process 
             * into EEPROM. This driver implement a usleep to wait WriteCycle. 
             */
            sal_usleep(5000);
        } else
#endif /* BCM_CMICM_SUPPORT */
        {
            /* Generate Start, for Write address */
            if( (rv = soc_i2c_start(unit, bus_addr)) < 0){
                LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "eep24c64_write(%d,%d,%x,%d,%d): "
                                     "failed to gen start\n"),
                          unit, devno, caddr, *data, len));
                I2C_UNLOCK(unit);
                return rv;
            }
            /* Send MSB (a1), wait for ACK */
            if( (rv = soc_i2c_write_one_byte(unit, a1)) < 0){
                LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "eep24c64_write(%d,%d,%x,%d,%d): "
                                     "failed to send a1 byte\n"),
                          unit, devno, caddr, *data, len));
                goto error;
            }
            /* Send LSB (a0), wait for ACK */
            if( (rv = soc_i2c_write_one_byte(unit, a0)) < 0){
                LOG_INFO(BSL_LS_SOC_I2C,
                         (BSL_META_U(unit,
                                     "eep24c64_write(%d,%d,%x,%d,%d): "
                                     "failed to send a0 byte\n"),
                          unit, devno, caddr, *data, len));
                goto error;
            }
            /* Send up to PAGE_SIZE data bytes, wait for ACK */
            for ( b = 0; b < nbytes; b++, ptr++, caddr++ ) {
                if ( (rv = soc_i2c_write_one_byte(unit, *ptr)) < 0){
                    LOG_INFO(BSL_LS_SOC_I2C,
                             (BSL_META_U(unit,
                                         "eep24c64_write(%d,%d,%d,%d,%d): "
                                         "tx data byte error\n"),
                              unit, devno, caddr, (uint32)*ptr, b));
                goto error;
                }
                LOG_VERBOSE(BSL_LS_SOC_I2C,
                            (BSL_META_U(unit,
                                        "eep24c64_write(u=%d,id=%d,page=%d "
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
            rv = eep24c64_ack_poll(unit, bus_addr);
            LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "eep24c64_ack_poll: "
                                 "%d address cycles for wr latency.\n"), rv));
            rv = (rv > 0 ? SOC_E_NONE: SOC_E_TIMEOUT);
        }
    }

    I2C_UNLOCK(unit);

    if (rv >= 0) {
        return soc_i2c_device(unit, devno)->tbyte - tbytes - 1 ;
    } else {
        return (rv);
    }
}

/*
 * Function: eep24c64_checksum
 *
 * Purpose: 16 bit incremental checksum; used for checking data
 *          validity of the configuration parameter block.
 * Parameters:
 *        partial - partial checksum, or zero if initial checksum,
 *                  updated on each call.
 *        data - data buffer to checksum
 *        len - size of data buffer to checksum
 *
 * Returns: updated checksum value
 *
 * Notes:
 *       This routine is called incrementally, one or more times,
 *       and each time, the new checksum value is returned. The first
 *       time this routine is called, pass 0 as the initial checksum
 *       value. On succesive calls, pass the last computed checksum
 *       value.
 */
STATIC uint16
eep24c64_chksum(uint16 partial, uint8* data, int len)
{
    uint8* dp;
    uint8 c0, c1;
    int i;

    partial = soc_ntohs(partial);

    c0 = (uint8) partial;
    c1 = (uint8) (partial >> 8);

    for ( i=0, dp = (uint8*)data; i < len; i++, dp++) {
	c0 += *dp;
	c1 += c0;
    }
    partial = (c1 << 8) + c0;
    return soc_htons(partial);
}

/*
 * Function: eep24c64_get_params
 *
 * Purpose: Get EEPROM configuration block from I2C NVRAM.
 *          Used internally by driver, demonstrates how to reliably
 *          store configuration parameters with a checksum.
 *
 * Parameters:
 *    u - StrataSwitch device number or I2C bus number
 *    d - chip device id
 *    c - NVRAM configuration block.
 *
 * Returns:
 *    SOC_E_INTERNAL - checksum bad, contents invalid.
 *    SOC_E_NONE - checksum read and contents valid.
 */
STATIC int
eep24c64_get_params(uint32 u, uint32 d, eep24c64_t* c)
{
    int r = SOC_E_NONE;
    int l = sizeof(eep24c64_t);
    uint16 savesum, sum;

    if ( !c )
	return SOC_E_PARAM;

    if ( (r = eep24c64_read(u, d, 0, (uint8*)c, (uint32 *)&l)) < 0 ) {
        LOG_INFO(BSL_LS_SOC_I2C,
                 (BSL_META_U(u,
                             "eep24c64_get_params: %s\n"), soc_errmsg(r)));
	return r;
    }
    /* Checksum config block */
    savesum = c->chksum;
    sum = eep24c64_chksum(0,  (uint8 *)&c->size, sizeof(uint16) );
    sum = eep24c64_chksum(sum,(uint8 *)&c->type, sizeof(uint16) );
    sum = eep24c64_chksum(sum,(uint8 *)&c->name, LC2464_NAME_LEN);

    if ( sum != savesum ) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(u,
                                "%s: NOTICE: EEPROM contents invalid or bad checksum\n"),
                     soc_i2c_devname(u, d)));
	return SOC_E_INTERNAL;
    }

    return r ;
}

/*
 * Function: eep24c64_set_params
 *
 * Purpose: Set EEPROM configuration block into NVRAM.
 *
 * Parameters:
 *    u - StrataSwitch device number or I2C bus number
 *    d - chip device id
 *    c - NVRAM configuration block.
 *
 * Returns:
 *    SOC_E_TIMEOUT  - write error or chip timeout
 *    SOC_E_NONE - configuration block and checksum updated, written to NVRAM.
 */
STATIC int
eep24c64_set_params(uint32 u, uint32 d, eep24c64_t* c)
{
    int rv = SOC_E_NONE;
    int l = sizeof(eep24c64_t);
    uint16 sum = 0;

    if ( !c )
	return SOC_E_PARAM;

    /* Checksum config block (over length and checksum) */
    sum = eep24c64_chksum(0,  (uint8 *)&c->size, sizeof(uint16) );
    sum = eep24c64_chksum(sum,(uint8 *)&c->type, sizeof(uint16) );
    sum = eep24c64_chksum(sum,(uint8 *)&c->name, LC2464_NAME_LEN);
    c->chksum = sum;
    return (rv = eep24c64_write(u, d, 0, (uint8*)c, l)) == l ? SOC_E_NONE : rv;
}

/*
 * Macro: GET_IO_STAT
 * Purpose: Used to measure I/O performance of NVRAM reads and writes.
 * Parameters:
 *   rtn - C function to call
 *   name - string to display
 *   sz - number of bytes I/O
 */

#ifdef	COMPILER_HAS_DOUBLE
#define GET_IO_STAT(rtn, sz, name, op) { \
	COMPILER_DOUBLE stime, etime; \
	stime = sal_time_double(); \
	(rtn); \
	etime = sal_time_double(); \
	LOG_VERBOSE(BSL_LS_SOC_COMMON, \
                    (BSL_META("%s: %s took %.2f sec %.2fKB/sec\n"), \
name, op, etime - stime, \
                     sz / (etime - stime) / 1024)); \
}
#else
#define GET_IO_STAT(rtn, sz, name, op) { \
	COMPILER_DOUBLE stime, etime; \
	stime = sal_time_usecs(); \
	(rtn); \
	etime = sal_time_usecs(); \
	LOG_VERBOSE(BSL_LS_SOC_COMMON, \
                    (BSL_META("%s: %s took %u usec %dKB/sec\n"), \
name, op, etime - stime, \
                     sz * (SECOND_USEC / 1024) / (etime - stime))); \
}
#endif	/* COMPILER_HAS_DOUBLE */

/*
 * Function: eep24c64_init
 *
 * Purpose: initialize the ATMEL 24C64 NVRAM chip.  Attempt to read
 *          checksum, if checksum is invalid, attempt to write a pattern
 *          to the first three quadrants of the 64K chip, and then read 
 *          it all back, checking each byte to verify data integrity. When
 *          finished, write the new configuration block to the start
 *          of the chip.
 *
 *          The last quadrant of the NVRAM is write protectable and
 *          stores read-only static information about the platform.
 *
 *          If the checksum is valid, simply display the checksum and
 *          number of bytes tested initially when the chip was probed.
 *
 * Parameters:
 *    unit - StrataSwitch device number or I2C bus number
 *    devno - chip device id
 *    data - address of test data
 *    len - size of test data
 *
 *
 * Notes: When the initial NVRAM is configured, this test destructively
 *       modifies the NVRAM contents. If the NVRAM ever becomes
 *       corrupt, this driver will clear the NVRAM with a new data
 *       pattern. All reads and writes should be performed past the
 *       NVRAM configuration block (LC24C64_DATA_START).
 */
STATIC int
eep24c64_init(int unit, int devno, void* data, int len)
{
    eep24c64_t config;
    uint8 pattern;
    const char *devname;

#ifdef CLEAR_NVRAM_BLOCK
    eep24c64_write(unit, devno, 0, "nvm", 3);
#endif

    devname = soc_i2c_devname(unit, devno);
    soc_i2c_devdesc_set(unit, devno, "Atmel 24C64 Serial EEPROM");

     sal_memset(&config, 0 , sizeof(eep24c64_t));

    
    if ( eep24c64_get_params( unit, devno, &config) < 0 ) {
	int i, j, rlen = len;
	uint8* outbuf = (uint8*)data;
	uint8* inbuf = NULL;

#ifdef RANDOM_PAGE_TEST
	inbuf = (uint8*)sal_alloc(len, "i2c");
	/* Fragmented (page and then some) write .. */
	for(i = 0; i <= LC2464_DEVICE_RW_SIZE/8; i+= len){
	    LOG_INFO(BSL_LS_SOC_I2C,
                     (BSL_META_U(unit,
                                 "eep24c64_init: unit=%d bytes=%d\n"), unit, i));
	    /* Write buffer to EEPROM */
	    eep24c64_write(unit, devno, i, outbuf, len);
	    /* Read back data just written */
	    eep24c64_read(unit, devno, i, inbuf, &rlen);

	    /* Verify each byte */
	    for( j = 0; j < len; j++){
		if ( inbuf[j] != outbuf[j]){
		    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                            "%s: ERROR: EEPROM miscompare "
                                            "off=%d expected=0x%x "
                                            "got 0x%x (addr=%d in page)\n"),
                                 devname,
                                 i+j, outbuf[j], inbuf[j], j));
		}
	    }
	}
	sal_free(inbuf);
#endif

	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: testing data integrity, %d bytes\n"),
                     devname, LC2464_DEVICE_RW_SIZE));

	/* Full buffer write in one shot ...*/
	len = LC2464_DEVICE_RW_SIZE;
	outbuf = (uint8*)sal_alloc(len, "i2c");
	if (outbuf == NULL) {
	    return SOC_E_MEMORY;
	}
	inbuf = (uint8*)sal_alloc(len, "i2c");
	if (inbuf == NULL) {
	    sal_free(outbuf);
	    return SOC_E_MEMORY;	
	}

	/* Write known pattern, clobber the parameter block */
	sal_memset(inbuf, 0x0, len);
	pattern = (uint8) sal_time_usecs();
	sal_memset(outbuf,pattern, len);
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: writing %d bytes pattern=0x%x\n"),
                     devname, len, pattern));

	GET_IO_STAT(rlen = eep24c64_write(unit, devno, 0, outbuf, len),
		 LC2464_DEVICE_RW_SIZE, devname, "write");

	if (rlen == LC2464_DEVICE_RW_SIZE )
	    ;	/* write was ok */
	else if (rlen > 0)
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "%s: ERROR: only %d out of %d bytes written!\n"),
                         devname, rlen, len));
	else
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "%s: ERROR: write failed: %s\n"),
                         devname, soc_errmsg(rlen)));

	/* Clear receive buffer */
	sal_memset(inbuf, 0x0, len);
	rlen = len;

	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "%s: reading %d bytes\n"), devname, rlen));

	GET_IO_STAT(eep24c64_read(unit, devno, 0, inbuf, (uint32 *)&rlen),
		    rlen, devname, "read");

	if (rlen == LC2464_DEVICE_RW_SIZE)
	    ;	/* read was ok */
	else if (rlen > 0)
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "%s: ERROR: only %d out of %d bytes read!\n"),
                         devname, rlen, len));
	else
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "%s: ERROR: read failed: %s\n"),
                         devname, soc_errmsg(rlen)));

	i = 0;
	for( j = 0; j < LC2464_DEVICE_RW_SIZE; j++){
	    if ( inbuf[j] != outbuf[j]){
		LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "%s: ERROR: miscompare at offset=%d "
                                        "expected=0x%x got 0x%x (addr=%d in page)\n"),
                             devname, i+j, outbuf[j], inbuf[j], j));
		i = -1;
		break;
	    }
	}

	if(i == 0)
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "%s: test passed (%d bytes verified)\n"),
                         devname, len));

	sal_free(inbuf);
	sal_free(outbuf);

	/* Write configuration block to NVRAM */
	config.size = (uint16)rlen - LC24C64_DATA_START;
	config.type = 0x70;
	sal_memset(config.name, 0x0,LC2464_NAME_LEN);

	if(sal_strlen(soc_i2c_devname(unit,devno)) > LC2464_NAME_LEN ) {
        LOG_WARN(BSL_LS_SOC_I2C,
                 (BSL_META_U(unit,
                             "Device name %s too long, trimming it .... \n"),
                  soc_i2c_devname(unit , devno)));
	sal_memcpy(config.name , soc_i2c_devname(unit, devno), LC2464_NAME_LEN - 1);
	config.name[LC2464_NAME_LEN - 1] = '\0';
	} else {
        sal_strncpy(config.name, soc_i2c_devname(unit,devno), LC2464_NAME_LEN - 1);
        config.name[LC2464_NAME_LEN - 1] = '\0';
 	}
	return eep24c64_set_params(unit, devno, &config);

    }

    return SOC_E_NONE;
}

/*
 * Function: eep24c64_ioctl: does miscellaenous tasks.
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
eep24c64_ioctl(int unit, int devno,
	       int command, void* data, int len)
{
    switch ( command ) {

    default:
	break;
    }
    return SOC_E_NONE;
}


/* AT24C64 64K Serial EEPROM Driver callout */
i2c_driver_t _soc_i2c_eep24c64_driver = {
    0x0,0x0, /* System assigned bytes */
    LC24C64_DEVICE_TYPE,
    eep24c64_read,
    eep24c64_write,
    eep24c64_ioctl,
    eep24c64_init,
    NULL,
};




  




