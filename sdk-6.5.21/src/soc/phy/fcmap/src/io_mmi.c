/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bbase_util.h>
#include <mmi_cmn.h>
#include <io_mmi.h>

#include <shared/bsl.h>

#define MMI_MIIM_READ(da, r, pd) _blmi_mmi_rd_f((da), (r), (pd))
#define MMI_MIIM_WRITE(da, r, d) _blmi_mmi_wr_f((da), (r), (buint16_t) (d))

/* MAX LMI IO wait count in microseconds */
#define BLMI_MMI_IO_MAX_WAIT      1000

/*******************************************************
 * LMI control structure
 */
int blmi_int_mmi_reset(blmi_dev_addr_t dev_addr)
{
    buint16_t   rval, rdptr, wrptr;
    int         retry_cnt = BLMI_MMI_IO_MAX_WAIT;

    /*
     * Reset the MMI.
     */
    if (MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_CFG, 1) < 0) {
        return BLMI_E_INIT;
    }

    while (retry_cnt--) {
        if (MMI_MIIM_READ(dev_addr, BLMI_MMI_REG_CFG, &rval) < 0) {
            return BLMI_E_INIT;
        }

        if ((rval & 0x1) == 0) {
            break;
        }
    }

    /* 
     * Check the read and write fifo pointers to make sure FIFI is
     * empty after reset.
     */
    if ((MMI_MIIM_READ(dev_addr, 
                        BLMI_MMI_REG_PORT_FIFO_WR_PTR, &wrptr) < 0) ||
        (MMI_MIIM_READ(dev_addr, 
                        BLMI_MMI_REG_PORT_FIFO_RD_PTR, &rdptr) < 0)) {
        return BLMI_E_INIT;
    }

    return (wrptr == rdptr) ? BLMI_E_NONE : BLMI_E_INIT;
}


/*******************************************************
 * MMI functions.
 */
STATIC int
_macsec_mmi_wait_write_ok(blmi_dev_addr_t dev_addr)
{
    int     rv, retry_cnt = BLMI_MMI_IO_MAX_WAIT;
    buint16_t  rval;

    while (retry_cnt) {
        rv = MMI_MIIM_READ(dev_addr, BLMI_MMI_REG_PORT_STATUS, &rval);
        if (rv < 0) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read \
                            BLMI_MMI_REG_PORT_CMD register.\n");
            return BLMI_E_FAIL;
        }

        if (rval & BLMI_MMI_PORT_STATUS_ERROR) {
            BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_write_ok \
                                    (Status: 0x%x)\n",
                                    rval);
            return BLMI_E_FAIL;
        }

        if ((rval & BLMI_MMI_PORT_CMD_DOIT) == 0) {
            return BLMI_E_NONE;
        }

        BMF_SAL_USLEEP(1);
        retry_cnt--;
    }

    BMF_SAL_PRINTF("MACSEC_MII Error: Write timeout\n");

    return BLMI_E_FAIL;
}

STATIC int
_macsec_mmi_wait_ready(blmi_dev_addr_t dev_addr)
{
    int     rv, retry_cnt = BLMI_MMI_IO_MAX_WAIT;
    buint16_t  rval;

    while (retry_cnt) {
        rv = MMI_MIIM_READ(dev_addr, BLMI_MMI_REG_PORT_STATUS, &rval);
        if (rv < 0) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read \
                            BLMI_MMI_REG_PORT_STATUS register.\n");
            return BLMI_E_FAIL;
        }

        if ((rval & BLMI_MMI_PORT_STATUS_READY) == 0) {
            goto port_ready;
        }

        BMF_SAL_USLEEP(1);
        retry_cnt--;
    }

    BMF_SAL_PRINTF("MACSEC_MII Error: Not Ready timeout\n");

    return BLMI_E_FAIL;

port_ready:
    retry_cnt = BLMI_MMI_IO_MAX_WAIT;
    while (retry_cnt) {
        rv = MMI_MIIM_READ(dev_addr, BLMI_MMI_REG_PORT_CMD, &rval);
        if (rv < 0) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read \
                            BLMI_MMI_REG_PORT_CMD register.\n");
            return BLMI_E_FAIL;
        }

        if ((rval & BLMI_MMI_PORT_CMD_DOIT) == 0) {
            goto doit_done;
        }

        BMF_SAL_USLEEP(1);
        retry_cnt--;
    }

    BMF_SAL_PRINTF("MACSEC_MII Error: Timeout DO_IT set\n");

    return BLMI_E_FAIL;

doit_done:
    return BLMI_E_NONE;
}

STATIC int 
_macsec_mmi_wait_data_ready(blmi_dev_addr_t dev_addr)
{
    int     rv, retry_cnt = BLMI_MMI_IO_MAX_WAIT;
    buint16_t  rval;

    while (retry_cnt) {
        rv = MMI_MIIM_READ(dev_addr, BLMI_MMI_REG_PORT_STATUS, &rval);
        if (rv < 0) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read \
                            BLMI_MMI_REG_PORT_CMD register.\n");
            return BLMI_E_FAIL;
        }

        if (rval & BLMI_MMI_PORT_STATUS_READY) {
            return BLMI_E_NONE;
        }

        if (rval & BLMI_MMI_PORT_STATUS_ERROR) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read (Status: 0x%x)\n",
                                    rval);
            return BLMI_E_FAIL;
        }

        BMF_SAL_USLEEP(1);
        retry_cnt--;
    }

    return BLMI_E_FAIL;
}

STATIC int 
_blmi_mmi_reg_read(_mmi_dev_info_t *devInfo, buint32_t io_addr,
                      void *data, int word_sz, int burstSz)
{
    int         num_wrd_rd, total_size;
    buint16_t   rval_lo, rval_hi;
    blmi_dev_addr_t dev_addr = devInfo->addr;
    buint32_t   *pdata = (buint32_t *)data;

    if (_macsec_mmi_wait_ready(dev_addr) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_ready failed.\n");
        return BLMI_E_FAIL;
    }
    
    /* Write Address */
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_ADDR_LSB, 
                                            (io_addr & 0xffff));
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_ADDR_MSB, 
                                    ((io_addr >> 16) & 0xffff));

    if (burstSz > 1) {
        MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_EXT_CMD, 
                       BLMI_MMI_PORT_EXT_BURST_LEN(burstSz) | 
                       BLMI_MMI_PORT_EXT_BURST_EN);
    }

    total_size = burstSz * word_sz * 4;
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_CMD, 
                   (total_size << 3) | BLMI_MMI_PORT_CMD_DOIT);

    if (_macsec_mmi_wait_data_ready(dev_addr) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: LMI Register read data \
                                        failed. Timeout!!\n");
        if (burstSz > 1) {
            MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_EXT_CMD, 0);
        }
        return BLMI_E_TIMEOUT;
    }
  
    if (burstSz > 1) {
        MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_EXT_CMD, 0);
    }

    /* read the data */
    while(burstSz) {
        for (num_wrd_rd = word_sz; num_wrd_rd; num_wrd_rd--) {
            MMI_MIIM_READ(dev_addr, BLMI_MMI_REG_PORT_DATA_LSB, &rval_lo);
            MMI_MIIM_READ(dev_addr, BLMI_MMI_REG_PORT_DATA_MSB, &rval_hi);
            *pdata++  = (rval_hi << 16) | rval_lo;
        }
        burstSz--;
    }

    return BLMI_E_NONE;
}

STATIC int 
_blmi_mmi_reg_write(_mmi_dev_info_t *devInfo, buint32_t io_addr,
                       void *data, int word_sz, int burstSz)
{
    int         num_wrd_wr, total_size;
    buint16_t   rval_lo, rval_hi;
    blmi_dev_addr_t dev_addr = devInfo->addr;
    buint32_t   *pdata = (buint32_t *)data;

    if (_macsec_mmi_wait_ready(dev_addr) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_ready failed.\n");
        return BLMI_E_FAIL;
    }
    
    total_size = burstSz * word_sz * 4;

    /* Write Address */
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_ADDR_LSB, 
                                                (io_addr & 0xffff));
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_ADDR_MSB, 
                                        ((io_addr >> 16) & 0xffff));

    if (burstSz > 1) {
        MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_EXT_CMD, 
                       BLMI_MMI_PORT_EXT_BURST_LEN(burstSz) | 
                       BLMI_MMI_PORT_EXT_BURST_EN);
    }

    while(burstSz) {
        for (num_wrd_wr = 0; num_wrd_wr < word_sz; num_wrd_wr++) {
            rval_lo = (buint16_t) (*pdata) & 0xffff;
            rval_hi = (buint16_t) ((*pdata) >> 16) & 0xffff;
            MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_DATA_MSB, rval_hi);
            MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_DATA_LSB, rval_lo);
            pdata ++;
        }
        burstSz--;
    }
   
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_CMD, 
                        (total_size << 3) |
                        BLMI_MMI_PORT_CMD_WRITE_EN | 
                        BLMI_MMI_PORT_CMD_DOIT);

    return _macsec_mmi_wait_write_ok(dev_addr);
}

STATIC int 
_blmi_mmi_mem_read(_mmi_dev_info_t *devInfo, buint32_t io_addr, 
                      void *data, int word_sz, int burstSz)
{
    int         num_wrd_rd, total_size;
    buint16_t   rval_lo, rval_hi;
    blmi_dev_addr_t dev_addr = devInfo->addr;
    buint32_t   *pdata = (buint32_t*) data;

    if (_macsec_mmi_wait_ready(dev_addr) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_ready failed.\n");
        return BLMI_E_FAIL;
    }

    /* Write Address */
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_ADDR_LSB, (io_addr & 0xffff));
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_ADDR_MSB, 
                   ((io_addr >> 16) & 0xffff));

    if (burstSz > 1) {
        MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_EXT_CMD, 
                       BLMI_MMI_PORT_EXT_BURST_LEN(burstSz) | 
                       BLMI_MMI_PORT_EXT_BURST_EN);
    }

    total_size = burstSz * word_sz * 4;
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_CMD, 
                            (total_size << 3) |   
                            BLMI_MMI_PORT_CMD_DOIT | 
                            BLMI_MMI_PORT_CMD_MEM_EN);

    if (_macsec_mmi_wait_data_ready(dev_addr) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: LMI Register read data \
                                failed. Timeout!!\n");
        return BLMI_E_FAIL;
    }
  
    if (burstSz > 1) {
        MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_EXT_CMD, 0);
    }

    while(burstSz) {
        for (num_wrd_rd = word_sz; num_wrd_rd; num_wrd_rd--) {
            MMI_MIIM_READ(dev_addr, BLMI_MMI_REG_PORT_DATA_LSB, &rval_lo);
            MMI_MIIM_READ(dev_addr, BLMI_MMI_REG_PORT_DATA_MSB, &rval_hi);
            *pdata++ = (rval_hi << 16) | rval_lo;
        }
        burstSz--;
    }
    return BLMI_E_NONE;
}

STATIC int 
_blmi_mmi_mem_write(_mmi_dev_info_t *devInfo, buint32_t io_addr, 
                       void *data, int word_sz, int burstSz)
{
    int         num_wrd_wr, total_size;
    buint16_t   rval_lo, rval_hi;
    blmi_dev_addr_t dev_addr = devInfo->addr;
    buint32_t   *pdata = (buint32_t*) data;

    if (_macsec_mmi_wait_ready(dev_addr) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_ready failed.\n");
        return BLMI_E_FAIL;
    }
    
    /* Write Address */
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_ADDR_LSB, (io_addr & 0xffff));
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_ADDR_MSB, 
                   ((io_addr >> 16) & 0xffff));

    if (burstSz > 1) {
        MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_EXT_CMD, 
                       BLMI_MMI_PORT_EXT_BURST_LEN(burstSz) | 
                       BLMI_MMI_PORT_EXT_BURST_EN);
    }

    total_size = burstSz * word_sz * 4;

    while(burstSz) {
        for (num_wrd_wr = 0; num_wrd_wr < word_sz; num_wrd_wr++) {
            rval_lo = (buint16_t) (*pdata) & 0xffff;
            rval_hi = (buint16_t) ((*pdata) >> 16) & 0xffff;
            MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_DATA_MSB, rval_hi);
            MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_DATA_LSB, rval_lo);
            pdata ++;
        }
        burstSz--;
    }
   
    MMI_MIIM_WRITE(dev_addr, BLMI_MMI_REG_PORT_CMD, 
                    (total_size << 3) | BLMI_MMI_PORT_CMD_MEM_EN |
                                        BLMI_MMI_PORT_CMD_WRITE_EN | 
                                        BLMI_MMI_PORT_CMD_DOIT);

    return _macsec_mmi_wait_write_ok(dev_addr);
}

STATIC _mmi_dev_info_t*
_blmi_mmi_get_device_info(blmi_dev_addr_t dev_addr)
{
    return _blmi_mmi_cmn_get_device_info(dev_addr);
}


/*********************************************************
 * IO vector table for MACSec PHYs
 */
int 
blmi_io_mmi(blmi_dev_addr_t dev_addr, int dev_port, 
               blmi_io_op_t op, buint32_t io_addr, 
               int word_sz, int num_entry, buint32_t *data)
{
    int rv = BLMI_E_FAIL, total_size;
    _mmi_dev_info_t     *dev_info;

    BCOMPILER_COMPILER_SATISFY(dev_port);

    dev_info = _blmi_mmi_get_device_info(dev_addr);
    if (!dev_info) {
        /* Create MMI device */
        dev_info = _blmi_mmi_create_device(dev_addr, 0);
    }

    if (dev_info == NULL) {
        BMF_SAL_DBG_PRINTF("Error: Unable to find MMI device addr=%x port=%d\n",
        dev_addr, dev_port);
        return BLMI_E_INTERNAL;
    }

    total_size = num_entry * word_sz * 4;
    if ((num_entry < 1) || (total_size > dev_info->fifo_size)) {
        return BLMI_E_PARAM;
    }

    BMF_SAL_LOCK(dev_info->lock);

    switch (op) {
        case BLMI_IO_REG_RD:
            rv = _blmi_mmi_reg_read(dev_info, io_addr, (void*)data, 
                                      word_sz, num_entry);
        break;
        case BLMI_IO_REG_WR:
            rv = _blmi_mmi_reg_write(dev_info, io_addr, (void*)data,
                                       word_sz, num_entry);
        break;
        case BLMI_IO_TBL_RD:
            rv = _blmi_mmi_mem_read(dev_info, io_addr, (void*)data,
                                      word_sz, num_entry);
        break;
        case BLMI_IO_TBL_WR:
            rv = _blmi_mmi_mem_write(dev_info, io_addr, (void*)data,
                                       word_sz, num_entry);
        break;
    }

    BMF_SAL_UNLOCK(dev_info->lock);

    BLMI_TRACE_CALLBACK(dev_info,op,io_addr,data,word_sz,num_entry);

    return rv;
}


