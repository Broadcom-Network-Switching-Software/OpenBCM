/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BLMI_IO_H
#define BLMI_IO_H
#include <bbase_types.h>
#include <blmi_err.h>


#define BLMI_MAX_UNITS  BMF_MAX_MT_PORTS

/*
 * typedef for address.
 */
typedef buint32_t      blmi_dev_addr_t;

/*
 * MACSEC I/O operation type identifier.
 */
typedef enum {
	BLMI_IO_REG_RD,   /* Register Read    */
	BLMI_IO_REG_WR,   /* Register Write   */
	BLMI_IO_TBL_RD,   /* Table Read       */
	BLMI_IO_TBL_WR    /* Table Write      */
} blmi_io_op_t;

/*
 * MACSEC I/O callback prototype.
 */
typedef int (*blmi_dev_io_f)(blmi_dev_addr_t devaddr,/* device addr*/
                               int dev_port,        /* dev port index  */
                               blmi_io_op_t op,  /* I/O operation   */  
                               buint32_t io_addr,   /* I/O address     */
                               int word_sz,         /* Word size       */
                               int num_entry,       /* Num entry       */
                               buint32_t *data);    /* Data buffer     */

/*
 * MMI read callback prototype.
 */
typedef int (*blmi_dev_mmi_mdio_rd_f)(blmi_dev_addr_t dev_addr, 
                                    buint32_t miim_reg, buint16_t *data);

/*
 * MMI write callback prototype.
 */
typedef int (*blmi_dev_mmi_mdio_wr_f)(blmi_dev_addr_t dev_addr, 
                                    buint32_t miim_reg, buint16_t data);

/*
 * MMI IO callback registeration.
 */
extern int
blmi_dev_mmi_mdio_register(blmi_dev_mmi_mdio_rd_f mmi_rd_f,
                              blmi_dev_mmi_mdio_wr_f mmi_wr_f);

/*
 * MDIO I/O operation type identifier.
 */
typedef enum {
	BLMI_MDIO_IO_REG_RD,   /* Register Read    */
	BLMI_MDIO_IO_REG_WR    /* Register Write   */
} blmi_mdio_io_op_t;

/* MMI IO routine */
/* MMI I/O function for MMI version 0, i.e. device bcm545XX. */
extern int 
blmi_io_mmi(blmi_dev_addr_t dev_addr, int dev_port, 
                   blmi_io_op_t op, buint32_t io_addr, 
                   int word_sz, int num_entry, buint32_t *data);

/* MMI I/O function for MMI version 1 */
extern buint32_t _blmi_mmi1_op_flags;
typedef enum {
   BLMI_OP_FLAG_ACCESS_INHIBIT  =  (1<<0) /* Block Access */
} blmi_op_flag_t;

extern int
blmi_io_mmi1_operational_flag_set(buint32_t flags, buint32_t mask);

extern int 
blmi_io_mmi1(blmi_dev_addr_t dev_addr, int dev_port, 
                   blmi_io_op_t op, buint32_t io_addr, 
                   int word_sz, int num_entry, buint32_t *data);

/* MMI I/O function for MMI version 1 using clause45 */
extern int 
blmi_io_mmi1_cl45(blmi_dev_addr_t dev_addr, int dev_port, 
                     blmi_io_op_t op, buint32_t io_addr, 
                     int word_sz, int num_entry, buint32_t *data);

/* MMI I/O function for MMI version 1 with quad select support */
extern int 
blmi_io_mmi1_quad(blmi_dev_addr_t dev_addr, int dev_port, 
                     blmi_io_op_t op, buint32_t io_addr, 
                     int word_sz, int num_entry, buint32_t *data);

/* MDIO access routine. */
extern int 
blmi_io_mdio(blmi_dev_addr_t phy_addr, blmi_mdio_io_op_t op, 
                    buint32_t io_addr, buint16_t *data);

#define BLMI_IO_CL45_ADDRESS(cl45_dev, reg)      \
                                (((cl45_dev) << 16) | (reg))

#endif /* BLMI_IO_H */

