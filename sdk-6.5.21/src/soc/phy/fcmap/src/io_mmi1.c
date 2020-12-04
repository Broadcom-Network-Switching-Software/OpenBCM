/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bbase_util.h>
#include <mmi_cmn.h>
#include <io_mmi1.h>

#include <shared/bsl.h>

STATIC int
_blmi_mmi1_mdio_read(_mmi_dev_info_t *devInfo, 
                        buint32_t io_addr, buint16_t *data)
{
    if (devInfo->is_cl45) {
        io_addr = BLMI_IO_CL45_ADDRESS(devInfo->cl45_dev, io_addr);
    }
    return _blmi_mmi_rd_f(devInfo->addr, io_addr, data);
}

STATIC int
_blmi_mmi1_mdio_write(_mmi_dev_info_t *devInfo, 
                        buint32_t io_addr, buint16_t data)
{
    if (devInfo->is_cl45) {
        io_addr = BLMI_IO_CL45_ADDRESS(devInfo->cl45_dev, io_addr);
    }
    return _blmi_mmi_wr_f(devInfo->addr, io_addr, data);
}

/*********************************************************
 * Local functions and structures
 */
#define MMI_MIIM_READ(devInfo, r, pd)   \
                    _blmi_mmi1_mdio_read((devInfo), (r), (pd))

#define MMI_MIIM_WRITE(devInfo, r, d)   \
                    _blmi_mmi1_mdio_write((devInfo), (r), (buint16_t) (d))

/* MAX LMI IO wait count in microseconds */
#define BLMI_MMI1_IO_MAX_WAIT      100


STATIC _mmi_dev_info_t*
_blmi_mmi_get_device_info(blmi_dev_addr_t dev_addr)
{
    return _blmi_mmi_cmn_get_device_info(dev_addr);
}

STATIC
int blmi_int_mmi1_reset(blmi_dev_addr_t dev_addr)
{
    buint16_t   rval, rdptr, wrptr;
    int         retry_cnt = BLMI_MMI1_IO_MAX_WAIT, rv = BLMI_E_NONE;
    _mmi_dev_info_t     *devInfo;
    
    devInfo = _blmi_mmi_get_device_info(dev_addr);
    if (devInfo == NULL) {
        BMF_SAL_DBG_PRINTF(
            "Error : Failed to probe device (MDIO adress: 0x%x\n", dev_addr);
        return BLMI_E_INTERNAL;
    }
    
    BMF_SAL_LOCK(devInfo->lock);

    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_CMD, 0x8000);
    while (retry_cnt--) {
        if (MMI_MIIM_READ(devInfo, BLMI_MMI1_REG_PORT_CMD, &rval) < 0) {
            rv = BLMI_E_INIT;
            goto error;
        }
        if ((rval & 0x8000) == 0) {
            break;
        }
    }

    /* 
     * Check the read and write fifo pointers to make sure FIFI is
     * empty after reset.
     */
    if ((MMI_MIIM_READ(devInfo, 
                        BLMI_MMI1_REG_PORT_FIFO_WR_PTR, &wrptr) < 0) ||
        (MMI_MIIM_READ(devInfo, 
                        BLMI_MMI1_REG_PORT_FIFO_RD_PTR, &rdptr) < 0)) {
        rv = BLMI_E_INIT;
        goto error;
    }

    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_INTR, 0x1f00);
    BMF_SAL_UNLOCK(devInfo->lock);
    
    return (wrptr == rdptr) ? BLMI_E_NONE : BLMI_E_INIT;

error:
    BMF_SAL_UNLOCK(devInfo->lock);

    return rv;
}


/*******************************************************
 * MMI functions.
 */
STATIC int
_macsec_mmi_wait_write_ok(_mmi_dev_info_t *devInfo)
{
    int     rv, retry_cnt = BLMI_MMI1_IO_MAX_WAIT;
    buint16_t  rval;

    while (retry_cnt) {
        rv = MMI_MIIM_READ(devInfo, BLMI_MMI1_REG_PORT_STATUS, &rval);
        if (rv < 0) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read \
                            BLMI_MMI1_REG_PORT_CMD register.\n");
            return BLMI_E_FAIL;
        }

        if (rval & BLMI_MMI1_PORT_STATUS_ERROR) {
            BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_write_ok (Status: 0x%x)\n",
                                    rval);
            return BLMI_E_FAIL;
        }

        if ((rval & BLMI_MMI1_PORT_CMD_DOIT) == 0) {
            return BLMI_E_NONE;
        }

        BMF_SAL_USLEEP(1);
        retry_cnt--;
    }

    BMF_SAL_PRINTF("MACSEC_MII_1 Error: Write timeout\n");

    return BLMI_E_FAIL;
}

STATIC int
_macsec_mmi_wait_ready(_mmi_dev_info_t *devInfo)
{
    int     rv, retry_cnt = BLMI_MMI1_IO_MAX_WAIT;
    buint16_t  rval;

    while (retry_cnt) {
        rv = MMI_MIIM_READ(devInfo, BLMI_MMI1_REG_PORT_STATUS, &rval);
        if (rv < 0) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read \
                            BLMI_MMI1_REG_PORT_STATUS register.\n");
            return BLMI_E_FAIL;
        }

        if ((rval & BLMI_MMI1_PORT_STATUS_READY) == 0) {
            goto port_ready;
        }

        BMF_SAL_USLEEP(1);
        retry_cnt--;
    }

    BMF_SAL_PRINTF("MACSEC_MII_1 Error: Not Ready timeout\n");

    return BLMI_E_FAIL;

port_ready:
    retry_cnt = BLMI_MMI1_IO_MAX_WAIT;
    while (retry_cnt) {
        rv = MMI_MIIM_READ(devInfo, BLMI_MMI1_REG_PORT_CMD, &rval);
        if (rv < 0) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read \
                            BLMI_MMI1_REG_PORT_CMD register.\n");
            return BLMI_E_FAIL;
        }

        if ((rval & BLMI_MMI1_PORT_CMD_DOIT) == 0) {
            goto doit_done;
        }

        BMF_SAL_USLEEP(1);
        retry_cnt--;
    }

    BMF_SAL_PRINTF("MACSEC_MII_1 Error: Timeout DO_IT set\n");

    return BLMI_E_FAIL;

doit_done:
    return BLMI_E_NONE;
}

STATIC int 
_macsec_mmi_wait_data_ready(_mmi_dev_info_t *devInfo)
{
    int     rv, retry_cnt = BLMI_MMI1_IO_MAX_WAIT;
    buint16_t  rval;

    while (retry_cnt) {
        rv = MMI_MIIM_READ(devInfo, BLMI_MMI1_REG_PORT_STATUS, &rval);
        if (rv < 0) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read \
                            BLMI_MMI1_REG_PORT_CMD register.\n");
            return BLMI_E_FAIL;
        }

        if (rval & BLMI_MMI1_PORT_STATUS_READY) {
            return BLMI_E_NONE;
        }

        if (rval & BLMI_MMI1_PORT_STATUS_ERROR) {
            BMF_SAL_DBG_PRINTF("Error: Failed to read (Status: 0x%x)\n",
                                    rval);
            return BLMI_E_FAIL;
        }

        BMF_SAL_USLEEP(50);
        retry_cnt--;
    }

    BMF_SAL_PRINTF("Error: READ Failed Status Reg val = %x\n", rval);
    return BLMI_E_FAIL;
}

STATIC int 
_blmi_mmi_reg_read(_mmi_dev_info_t *devInfo, buint32_t io_addr,
                      void *data, int word_sz, int burstSz)
{
    int         num_wrd_rd, total_size;
    buint16_t   rval_lo, rval_hi;
    buint32_t   *pdata = (buint32_t *)data;

    if (_macsec_mmi_wait_ready(devInfo) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_ready failed.\n");
        return BLMI_E_FAIL;
    }
    
    /* Write Address */
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_ADDR_LSB, 
                                            (io_addr & 0xffff));
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_ADDR_MSB, 
                                    ((io_addr >> 16) & 0xffff));

    if (burstSz > 1) {
        MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 
                       BLMI_MMI1_PORT_EXT_BURST_LEN(burstSz) | 
                       BLMI_MMI1_PORT_EXT_BURST_EN);
    }

    total_size = burstSz * word_sz * 4;
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_CMD, 
                   (total_size << 3) | BLMI_MMI1_PORT_CMD_DOIT);

    if (_macsec_mmi_wait_data_ready(devInfo) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: LMI Register read data \
                                        failed. Timeout!!\n");
        if (burstSz > 1) {
            MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 0);
        }
        return BLMI_E_TIMEOUT;
    }
  
    if (burstSz > 1) {
        MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 0);
    }

    /* read the data */
    while(burstSz) {
        for (num_wrd_rd = word_sz; num_wrd_rd; num_wrd_rd--) {
            MMI_MIIM_READ(devInfo, BLMI_MMI1_REG_PORT_DATA_LSB, &rval_lo);
            MMI_MIIM_READ(devInfo, BLMI_MMI1_REG_PORT_DATA_MSB, &rval_hi);
            *pdata++  = (rval_hi << 16) | rval_lo;
        }
        burstSz--;
    }

    return BLMI_E_NONE;
}

STATIC int 
_blmi_mmi_reg_write(_mmi_dev_info_t *devInfo, buint32_t io_addr,
                       void *data, int word_sz, int burstSz, int blk_copy)
{
    int         num_wrd_wr, total_size;
    buint16_t   rval_lo, rval_hi;
    buint32_t   *pdata = (buint32_t *)data;

    if (_macsec_mmi_wait_ready(devInfo) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_ready failed.\n");
        return BLMI_E_FAIL;
    }
    
    /* Write Address */
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_ADDR_LSB, 
                                                (io_addr & 0xffff));
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_ADDR_MSB, 
                                        ((io_addr >> 16) & 0xffff));

    if (burstSz > 1) {
        if (blk_copy) {
            MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 
                           BLMI_MMI1_PORT_EXT_BURST_LEN(burstSz) | 
                           BLMI_MMI1_PORT_EXT_BLK_INIT_EN);
            burstSz = 1;
        } else {
            MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 
                           BLMI_MMI1_PORT_EXT_BURST_LEN(burstSz) | 
                           BLMI_MMI1_PORT_EXT_BURST_EN);
        }
    }

    total_size = burstSz * word_sz * 4;

    while(burstSz) {
        for (num_wrd_wr = 0; num_wrd_wr < word_sz; num_wrd_wr++) {
            rval_lo = (buint16_t) (*pdata) & 0xffff;
            rval_hi = (buint16_t) ((*pdata) >> 16) & 0xffff;
            MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_DATA_MSB, rval_hi);
            MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_DATA_LSB, rval_lo);
            pdata ++;
        }
        burstSz--;
    }
   
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_CMD, 
                        (total_size << 3) |
                        BLMI_MMI1_PORT_CMD_WRITE_EN | 
                        BLMI_MMI1_PORT_CMD_DOIT);

    return _macsec_mmi_wait_write_ok(devInfo);
}

STATIC int 
_blmi_mmi_mem_read(_mmi_dev_info_t *devInfo, buint32_t io_addr, 
                      void *data, int word_sz, int burstSz)
{
    int         num_wrd_rd, total_size;
    buint16_t   rval_lo, rval_hi;
    buint32_t   *pdata = (buint32_t*) data;

    if (_macsec_mmi_wait_ready(devInfo) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_ready failed.\n");
        return BLMI_E_FAIL;
    }

    /* Write Address */
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_ADDR_LSB, (io_addr & 0xffff));
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_ADDR_MSB, 
                   ((io_addr >> 16) & 0xffff));

    if (burstSz > 1) {
        MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 
                       BLMI_MMI1_PORT_EXT_BURST_LEN(burstSz) | 
                       BLMI_MMI1_PORT_EXT_BURST_EN);
    }

    total_size = burstSz * word_sz * 4;
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_CMD, 
                            (total_size << 3) |   
                            BLMI_MMI1_PORT_CMD_DOIT | 
                            BLMI_MMI1_PORT_CMD_MEM_EN);

    if (_macsec_mmi_wait_data_ready(devInfo) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: LMI Register read data \
                                failed. Timeout!!\n");
        return BLMI_E_FAIL;
    }
  
    if (burstSz > 1) {
        MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 0);
    }

    while(burstSz) {
        for (num_wrd_rd = word_sz; num_wrd_rd; num_wrd_rd--) {
            MMI_MIIM_READ(devInfo, BLMI_MMI1_REG_PORT_DATA_LSB, &rval_lo);
            MMI_MIIM_READ(devInfo, BLMI_MMI1_REG_PORT_DATA_MSB, &rval_hi);
            *pdata++ = (rval_hi << 16) | rval_lo;
        }
        burstSz--;
    }
    return BLMI_E_NONE;
}

STATIC int 
_blmi_mmi_mem_write(_mmi_dev_info_t *devInfo, buint32_t io_addr, 
                       void *data, int word_sz, int burstSz, int blk_copy)
{
    int         num_wrd_wr, total_size, rv;
    buint16_t   rval_lo, rval_hi;
    buint32_t   *pdata = (buint32_t*) data;

    if (_macsec_mmi_wait_ready(devInfo) != BLMI_E_NONE) {
        BMF_SAL_DBG_PRINTF("Error: _macsec_mmi_wait_ready failed.\n");
        return BLMI_E_FAIL;
    }
    
    /* Write Address */
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_ADDR_LSB, (io_addr & 0xffff));
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_ADDR_MSB, 
                   ((io_addr >> 16) & 0xffff));

    total_size = burstSz * word_sz;

    if (burstSz > 1) {
        if (blk_copy) {
            MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 
                           BLMI_MMI1_PORT_EXT_BURST_LEN(burstSz) | 
                           BLMI_MMI1_PORT_EXT_BLK_INIT_EN);
            burstSz = 1;
        } else {
            MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 
                           BLMI_MMI1_PORT_EXT_BURST_LEN(burstSz) | 
                           BLMI_MMI1_PORT_EXT_BURST_EN);
        }
    }

    while(burstSz) {
        for (num_wrd_wr = 0; num_wrd_wr < word_sz; num_wrd_wr++) {
            rval_lo = (buint16_t) (*pdata) & 0xffff;
            rval_hi = (buint16_t) ((*pdata) >> 16) & 0xffff;
            MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_DATA_MSB, rval_hi);
            MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_DATA_LSB, rval_lo);
            pdata ++;
        }
        burstSz--;
    }
   
    MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_CMD, 
                    (word_sz << 5) | BLMI_MMI1_PORT_CMD_MEM_EN |
                                        BLMI_MMI1_PORT_CMD_WRITE_EN | 
                                        BLMI_MMI1_PORT_CMD_DOIT);

    rv = _macsec_mmi_wait_write_ok(devInfo);

    if (total_size > word_sz) {
        MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_PORT_EXT_CMD, 0);
    }

    return rv;
}

extern int en_mdio_dump;

/*********************************************************
 * IO vector table for MACSec PHYs
 */
STATIC int
_blmi_mmi_io_op(_mmi_dev_info_t *dev_info, int dev_port, blmi_io_op_t op,
               buint32_t io_addr, buint32_t *data,
               int word_sz, int num_entry,
               _mmi_pre_cb_t  pre,
               _mmi_post_cb_t post)
{
    int rv = BLMI_E_NONE;

    if (_blmi_mmi1_op_flags & BLMI_OP_FLAG_ACCESS_INHIBIT) {
        return BLMI_E_NONE;
    }


    BMF_SAL_LOCK(dev_info->lock);

    if (pre != NULL) {
        if ((rv = pre(dev_info,dev_port,op,&io_addr,data,word_sz,num_entry))) {
            BMF_SAL_PRINTF("MMI pre failed (op=%d) address = 0x%08x\n",
                            op, io_addr);
            goto done;
        }
    }

    switch (op) {
        case BLMI_IO_REG_RD:
            rv = _blmi_mmi_reg_read(dev_info, io_addr, (void*)data, 
                                      word_sz, num_entry);
        break;
        case BLMI_IO_REG_WR:
            rv = _blmi_mmi_reg_write(dev_info, io_addr, (void*)data,
                                       word_sz, num_entry, 0);
        break;
        case BLMI_IO_TBL_RD:
            rv = _blmi_mmi_mem_read(dev_info, io_addr, (void*)data,
                                      word_sz, num_entry);
        break;
        case BLMI_IO_TBL_WR:
            rv = _blmi_mmi_mem_write(dev_info, io_addr, (void*)data,
                                       word_sz, num_entry, 0);
        break;
    }

    if (rv == BLMI_E_NONE  && post != NULL) {
        rv = post(dev_info,dev_port,op,&io_addr,data,word_sz,num_entry);
    }

done:

    BMF_SAL_UNLOCK(dev_info->lock);

    if (rv != BLMI_E_NONE) {
        BMF_SAL_PRINTF("MMI op failed (op=%d) address = 0x%08x\n", op, io_addr);
    }

    BLMI_TRACE_CALLBACK(dev_info,op,io_addr,data,word_sz,num_entry);

    return rv;
}

STATIC int
_blmi_io_mmi1(blmi_dev_addr_t dev_addr, int dev_port, 
               blmi_io_op_t op, buint32_t io_addr, 
               int word_sz, int num_entry, buint32_t *data,
               _mmi_pre_cb_t  pre,
               _mmi_post_cb_t post)
{
    int total_size, rv;
    _mmi_dev_info_t     *dev_info;
  
    BCOMPILER_COMPILER_SATISFY(dev_port);

    dev_info = _blmi_mmi_get_device_info(dev_addr);
    if (!dev_info) {
        /* Create MMI device */
        dev_info = _blmi_mmi_create_device(dev_addr, 0);
    }

    if (dev_info == NULL) {
        BMF_SAL_PRINTF("Error: Unable to find MMI device addr=%x port=%d\n", dev_addr, dev_port);
        return BLMI_E_INTERNAL;
    }

    total_size = num_entry * word_sz * 4;
    if ((num_entry < 1) || (total_size > dev_info->fifo_size)) {
        return BLMI_E_PARAM;
    }

    rv = _blmi_mmi_io_op(dev_info, dev_port, op, io_addr, data,
                         word_sz, num_entry , pre, post);
    if (rv) {
        blmi_int_mmi1_reset(dev_addr);
    }

    return rv;
}


int
blmi_io_mmi1(blmi_dev_addr_t dev_addr, int dev_port, 
               blmi_io_op_t op, buint32_t io_addr, 
               int word_sz, int num_entry, buint32_t *data)
{
    int rv;
  
    rv = _blmi_io_mmi1(dev_addr, dev_port, 
                       op, io_addr, word_sz, num_entry, data,
                       NULL, NULL );

    return rv;
}


buint32_t _blmi_mmi1_op_flags = 0 ;/* set by blmi_io_mmi1_operational_flag_set*/
int
blmi_io_mmi1_operational_flag_set(buint32_t flags, buint32_t mask)
{
    buint32_t tmp = _blmi_mmi1_op_flags ;

    tmp &= ~(mask);
    tmp |= (flags & mask);

    _blmi_mmi1_op_flags = tmp;

    return BLMI_E_NONE;
}



/************************************************
 * Quad select pre-process function
 */

#define BLMI_QUAD_MAP_PORT(p)\
    ( ((p) % 4)  + (8 * ((p) / 8)))
#define BLMI_QUAD_SEL(p)\
    ( ((p) / 4) % 2 )
#define BLMI_IOADDR_GET_PORT(ioaddr, shift)\
    (((ioaddr)  >> shift) & 0xf )
#define BLMI_IOADDR_SET_PORT(ioaddr, shift, port)\
     ((ioaddr) &= ~((       0xf) << shift) ) ;\
     ((ioaddr) |= (((port)& 0xf) << shift) )

#define BLMI_TBL_SHIFT  (16)
#define BLMI_REG_SHIFT  (12)

STATIC int
_blmi_io_quad_select(
    _mmi_dev_info_t *devInfo,    
    int dev_port,
    blmi_io_op_t op,
    buint32_t *io_addr,
    buint32_t *data, 
    int word_sz,
    int num_entry) {

    int iport;

    BCOMPILER_COMPILER_SATISFY(data);
    BCOMPILER_COMPILER_SATISFY(word_sz);
    BCOMPILER_COMPILER_SATISFY(num_entry);

    switch (op) {
        case BLMI_IO_REG_RD:
        case BLMI_IO_REG_WR:
            iport = BLMI_IOADDR_GET_PORT(*io_addr, BLMI_REG_SHIFT);
            BLMI_IOADDR_SET_PORT(*io_addr,
                                 BLMI_REG_SHIFT,
                                 BLMI_QUAD_MAP_PORT(iport) ) ;
        break;
        case BLMI_IO_TBL_RD:
        case BLMI_IO_TBL_WR:
            iport = BLMI_IOADDR_GET_PORT(*io_addr, BLMI_TBL_SHIFT);
            BLMI_IOADDR_SET_PORT(*io_addr,
                                 BLMI_TBL_SHIFT,
                                 BLMI_QUAD_MAP_PORT(iport) ) ;
        break;
    }


    return MMI_MIIM_WRITE(devInfo, BLMI_MMI1_REG_QUAD_SEL, 
                          BLMI_QUAD_SEL(dev_port) );
}


int
blmi_io_mmi1_quad(blmi_dev_addr_t dev_addr, int dev_port, 
               blmi_io_op_t op, buint32_t io_addr, 
               int word_sz, int num_entry, buint32_t *data)
{
    int rv;

    rv = _blmi_io_mmi1(dev_addr, dev_port, 
                       op, io_addr, word_sz, num_entry, data,
                       _blmi_io_quad_select, NULL );

    return rv;
}

int 
blmi_io_mmi1_cl45(blmi_dev_addr_t dev_addr, int dev_port, 
                     blmi_io_op_t op, buint32_t io_addr, 
                     int word_sz, int num_entry, buint32_t *data)
{
    int total_size, rv;
    _mmi_dev_info_t     *dev_info;
  
    BCOMPILER_COMPILER_SATISFY(dev_port);

    dev_info = _blmi_mmi_get_device_info(dev_addr);
    if (!dev_info) {
        /* Create MMI device */
        dev_info = _blmi_mmi_create_device(dev_addr, 1);
    }

    if (dev_info == NULL) {
        BMF_SAL_PRINTF("Error: Unable to find MMI device addr=0x%x port=%d\n", dev_addr, dev_port);
        return BLMI_E_INTERNAL;
    }

    total_size = num_entry * word_sz * 4;
    if ((num_entry < 1) || (total_size > dev_info->fifo_size)) {
        return BLMI_E_PARAM;
    }

    rv = _blmi_mmi_io_op(dev_info, dev_port, op, io_addr, data,
                         word_sz, num_entry , NULL, NULL);
    if (rv) {
        blmi_int_mmi1_reset(dev_addr);
    }

    return rv;
}

