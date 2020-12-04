/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bbase_util.h>
#include <mmi_cmn.h>

#include <shared/bsl.h>

STATIC int 
_mmi_default_read_f(blmi_dev_addr_t dev_addr, 
                    buint32_t mdio_addr, buint16_t *data);

STATIC int 
_mmi_default_write_f(blmi_dev_addr_t dev_addr, 
                     buint32_t mdio_addr, buint16_t data);

/*******************************************************************
 * MDIO callback function pointers.
 */
blmi_dev_mmi_mdio_rd_f _blmi_mmi_rd_f = &_mmi_default_read_f;
blmi_dev_mmi_mdio_wr_f _blmi_mmi_wr_f = &_mmi_default_write_f;

#define MMI_MIIM_READ(da, r, pd) _blmi_mmi_rd_f((da), (r), (pd))
#define MMI_MIIM_WRITE(da, r, d) _blmi_mmi_wr_f((da), (r), (d))

char *blmi_errmsg[] = BLMI_ERRMSG_INIT;

/*
 * Default MMI MDIO Read function, it just returns failure,
 * user should always register a proper MDIO read function
 * before any of the MMI operations can proceed.
 */
STATIC int 
_mmi_default_read_f(blmi_dev_addr_t dev_addr, 
                    buint32_t mdio_addr, buint16_t *data)
{
    BCOMPILER_COMPILER_SATISFY(dev_addr);
    BCOMPILER_COMPILER_SATISFY(mdio_addr);
    BCOMPILER_COMPILER_SATISFY(data);

    BMF_SAL_DBG_PRINTF("Error: No HW access to MDIO read\n");
    return BLMI_E_CONFIG;
}

/*
 * Default MMI MDIO Write function, it just returns failure,
 * user should always register a proper MDIO write function
 * before any of the MMI operations can proceed.
 */
STATIC int 
_mmi_default_write_f(blmi_dev_addr_t dev_addr, 
                     buint32_t mdio_addr, buint16_t data)
{
    BCOMPILER_COMPILER_SATISFY(dev_addr);
    BCOMPILER_COMPILER_SATISFY(mdio_addr);
    BCOMPILER_COMPILER_SATISFY(data);

    BMF_SAL_DBG_PRINTF("Error: MACSEC MMI Write\n");
    return BLMI_E_CONFIG;
}

/*
 * MACSEC MMI Information.
 * This table contains information about each of the MMI ports
 * in the system along with its MDIO address and other information
 * pertaining to MMI.
 */
STATIC _mmi_dev_info_t mmi_devs[BLMI_MAX_UNITS];

typedef struct _mmi_port_data_s {
    blmi_dev_addr_t  dev_addr;
    /*bmacsec_core_t      dev_core;*/
    int                 found;
} _mmi_port_data_t;

typedef struct _mmi_cmn_bus_info_s {
    blmi_dev_addr_t  dev_addr;
    /*bmacsec_core_t      dev_core;*/
    int                 used;
} _mmi_cmn_bus_info_t;

_mmi_dev_info_t*
_blmi_mmi_create_device(blmi_dev_addr_t dev_addr, int cl45)
{
    int                 dev_num, cl45_dev = 30;
    buint16_t           id1, id2;
    buint32_t           id1_offset = 2, id2_offset = 3;
    /*
    buint32_t           cap_flags;
    */

    /* Unable to find dev info, add now. */
    for (dev_num = 0; dev_num < BLMI_MAX_UNITS; dev_num++) {
        if (!mmi_devs[dev_num].used) {

            if (cl45) {
                id1_offset = BLMI_IO_CL45_ADDRESS(cl45_dev, id1_offset);
                id2_offset = BLMI_IO_CL45_ADDRESS(cl45_dev, id2_offset);
            }

            if ((MMI_MIIM_READ(dev_addr, id1_offset, &id1) < 0) ||
                (MMI_MIIM_READ(dev_addr, id2_offset, &id2) < 0)) {
                return NULL;
            }

            mmi_devs[dev_num].fifo_size = 2048;
            mmi_devs[dev_num].addr = dev_addr;
            mmi_devs[dev_num].lock = BMF_SAL_LOCK_CREATE("mmi lock");
            mmi_devs[dev_num].used = 1;
            mmi_devs[dev_num].is_cl45 = cl45 ? 1 : 0;
            mmi_devs[dev_num].cl45_dev = (cl45) ? cl45_dev : 0;

            /*
            mmi_devs[dev_num].cap_flags = cap_flags;
            */
            return &mmi_devs[dev_num];
        }
    }
    return NULL;
}

/*
 * Return MMI info corrsponding to device with address dev_addr.
 * If this is the first time, we are learing about the device,
 * add it to our table.
 */
_mmi_dev_info_t * 
_blmi_mmi_cmn_get_device_info(blmi_dev_addr_t dev_addr)
{
    int dev_num;

    for (dev_num = 0; dev_num < BLMI_MAX_UNITS; dev_num++) {
        if ((mmi_devs[dev_num].used) && (mmi_devs[dev_num].addr == dev_addr)) {
            return &mmi_devs[dev_num];
        }
    }
    return NULL;
}

int
blmi_dev_mmi_mdio_register(blmi_dev_mmi_mdio_rd_f mmi_rd_f,
                           blmi_dev_mmi_mdio_wr_f mmi_wr_f)
{
    _blmi_mmi_rd_f = mmi_rd_f;
    _blmi_mmi_wr_f = mmi_wr_f;
    return BLMI_E_NONE;
}

int 
blmi_io_mdio(blmi_dev_addr_t phy_addr, blmi_mdio_io_op_t op, 
                    buint32_t io_addr, buint16_t *data)
{
    int rv = BLMI_E_FAIL;

    if (!data) {
        return rv;
    }

    switch (op) {
        case BLMI_MDIO_IO_REG_RD:
            rv = MMI_MIIM_READ(phy_addr, io_addr, data);
        break;
        case BLMI_MDIO_IO_REG_WR:
            rv = MMI_MIIM_WRITE(phy_addr, io_addr, *data);
        break;
    }
    return rv;
}

/* Trace functions */
_mmi_trace_callback_t  _blmi_trace_cb = NULL;
unsigned int           _blmi_trace_flags = 0;

int
_blmi_register_trace(unsigned int flags, _mmi_trace_callback_t cb)
{
    _blmi_trace_flags |= flags;
    _blmi_trace_cb     = cb;
    return 0;
}


int
_blmi_unregister_trace(unsigned int flags)
{
    _blmi_trace_flags &= ~(flags);
    return 0;
}

