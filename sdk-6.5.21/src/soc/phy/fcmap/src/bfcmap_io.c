/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bfcmap.h>
#include <bfcmap_int.h>



/*********************************************************************
 * LMI wrapper functions
 */

int 
bfcmap_io_mmi(bfcmap_dev_addr_t dev_addr, int dev_port, 
                   bfcmap_io_op_t op, buint32_t io_addr, 
                   int word_sz, int num_entry, buint32_t *data)
{
    return blmi_io_mmi(dev_addr, dev_port, op, io_addr, 
                   word_sz, num_entry, data);
}

/* MMI I/O function for MMI version 1 */
int 
bfcmap_io_mmi1(bfcmap_dev_addr_t dev_addr, int dev_port, 
                   bfcmap_io_op_t op, buint32_t io_addr, 
                   int word_sz, int num_entry, buint32_t *data)
{
    return blmi_io_mmi1(dev_addr, dev_port, op, io_addr, 
                   word_sz, num_entry, data);
}

/* MMI I/O function for MMI version 1 using clause45 */
int 
bfcmap_io_mmi1_cl45(bfcmap_dev_addr_t dev_addr, int dev_port, 
                     bfcmap_io_op_t op, buint32_t io_addr, 
                     int word_sz, int num_entry, buint32_t *data)
{
    return blmi_io_mmi1_cl45(dev_addr, dev_port, op, io_addr, 
                     word_sz, num_entry, data);
}

/* MDIO access routine. */
int 
bfcmap_io_mdio(bfcmap_dev_addr_t phy_addr, bfcmap_mdio_io_op_t op, 
                    buint32_t io_addr, buint16_t *data)
{
    return blmi_io_mdio(phy_addr, op, io_addr, data);
}


int
bfcmap_dev_mmi_mdio_register(bfcmap_dev_mmi_mdio_rd_f mmi_rd_f,
                              bfcmap_dev_mmi_mdio_wr_f mmi_wr_f)
{
    return blmi_dev_mmi_mdio_register(mmi_rd_f, mmi_wr_f);
}

