/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BFCMAP_IO_H
#define BFCMAP_IO_H
#include <bfcmap_types.h>
#include <blmi_io.h>


/*
 * typedef for address.
 */
typedef blmi_dev_addr_t      bfcmap_dev_addr_t;

/*
 * MACSEC I/O operation type identifier.
 */
typedef blmi_io_op_t bfcmap_io_op_t;
#define BFCMAP_IO_REG_RD  (BLMI_IO_REG_RD)   /* Register Read    */
#define BFCMAP_IO_REG_WR  (BLMI_IO_REG_WR)   /* Register Write   */
#define BFCMAP_IO_TBL_RD  (BLMI_IO_TBL_RD)   /* Table Read       */
#define BFCMAP_IO_TBL_WR  (BLMI_IO_TBL_WR)   /* Table Write      */

/*
 * MACSEC I/O callback prototype.
 */
typedef blmi_dev_io_f bfcmap_dev_io_f;

/*
 * MMI read callback prototype.
 */
typedef blmi_dev_mmi_mdio_rd_f  bfcmap_dev_mmi_mdio_rd_f;

/*
 * MMI write callback prototype.
 */
typedef blmi_dev_mmi_mdio_wr_f  bfcmap_dev_mmi_mdio_wr_f;

/*
 * MMI IO callback registeration.
 */
extern int
bfcmap_dev_mmi_mdio_register(bfcmap_dev_mmi_mdio_rd_f mmi_rd_f,
                              bfcmap_dev_mmi_mdio_wr_f mmi_wr_f);

/*
 * MDIO I/O operation type identifier.
 */
typedef blmi_mdio_io_op_t bfcmap_mdio_io_op_t;
#define BFCMAP_MDIO_IO_REG_RD  (BLMI_MDIO_IO_REG_RD)   /* Register Read    */
#define BFCMAP_MDIO_IO_REG_WR  (BLMI_MDIO_IO_REG_WR)   /* Register Write   */

/* MMI IO routine */
/* MMI I/O function for MMI version 0, i.e. device bcm545XX. */
extern int 
bfcmap_io_mmi(bfcmap_dev_addr_t dev_addr, int dev_port, 
                   bfcmap_io_op_t op, buint32_t io_addr, 
                   int word_sz, int num_entry, buint32_t *data);

/* MMI I/O function for MMI version 1 */
extern int 
bfcmap_io_mmi1(bfcmap_dev_addr_t dev_addr, int dev_port, 
                   bfcmap_io_op_t op, buint32_t io_addr, 
                   int word_sz, int num_entry, buint32_t *data);

/* MMI I/O function for MMI version 1 using clause45 */
extern int 
bfcmap_io_mmi1_cl45(bfcmap_dev_addr_t dev_addr, int dev_port, 
                     bfcmap_io_op_t op, buint32_t io_addr, 
                     int word_sz, int num_entry, buint32_t *data);

/* MDIO access routine. */
extern int 
bfcmap_io_mdio(bfcmap_dev_addr_t phy_addr, bfcmap_mdio_io_op_t op, 
                    buint32_t io_addr, buint16_t *data);


#define BFCMAP_IO_CL45_ADDRESS(cl45_dev, reg)  \
           BLMI_IO_CL45_ADDRESS(cl45_dev, reg) 

#endif /* BFCMAP_IO_H */

