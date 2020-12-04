/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BLMI_MMI_CMN_H
#define BLMI_MMI_CMN_H

#include <blmi_io.h>
#undef  STATIC
#define STATIC static

/*******************************************************
 * Trace callback
 */
typedef void (*_mmi_trace_callback_t)(
    blmi_dev_addr_t dev_addr,
    blmi_io_op_t op,
    buint32_t io_addr,
    buint32_t *data, 
    int word_sz,
    int num_entry);


/*******************************************************
 * LMI control structure
 */
typedef struct _mmi_dev_info_s {
    blmi_dev_addr_t     addr;
    buser_sal_lock_t    lock;
    int                 used;
    int                 fifo_size;
    int                 is_cl45;
    int                 cl45_dev;
    buint32_t           cap_flags;
} _mmi_dev_info_t;

/*******************************************************
 * Pre-process LMI callback
 */
typedef int (*_mmi_pre_cb_t)(
    _mmi_dev_info_t *dev_info,
    int dev_port,
    blmi_io_op_t op,
    buint32_t *io_addr,
    buint32_t *data, 
    int word_sz,
    int num_entry);

/*******************************************************
 * Post-process LMI callback
 */
typedef int (*_mmi_post_cb_t)(
    _mmi_dev_info_t *dev_info,
    int dev_port,
    blmi_io_op_t op,
    buint32_t *io_addr,
    buint32_t *data, 
    int word_sz,
    int num_entry);


extern blmi_dev_mmi_mdio_rd_f _blmi_mmi_rd_f;
extern blmi_dev_mmi_mdio_wr_f _blmi_mmi_wr_f;

_mmi_dev_info_t*
_blmi_mmi_create_device(blmi_dev_addr_t dev_addr, int cl45);

_mmi_dev_info_t * 
_blmi_mmi_cmn_get_device_info(blmi_dev_addr_t dev_addr);


/*
 * The following flags define the capability of the MMI
 * core.
 */
/* Move register/table entries within the device */
#define BLMI_MMI_CAP_DMA_WITHIN_DEVICE       0x01
/* DMA to/from host to device table/registers */
#define BLMI_MMI_CAP_DMA_TO_FROM_HOST        0x02
/* Write the same entry to incremental addresses */
#define BLMI_MMI_CAP_WRITE_MULTIPLE          0x04


/* Trace functions */
extern _mmi_trace_callback_t  _blmi_trace_cb;
extern unsigned int           _blmi_trace_flags;
#define BLMI_TRACE_REG    0x01
#define BLMI_TRACE_TBL    0x02
int
_blmi_register_trace(unsigned int flags, _mmi_trace_callback_t cb);

int
_blmi_unregister_trace(unsigned int flags);


#define BLMI_TRACE_CALLBACK(dev_info,op,io_addr,data,word_sz,num_entry)\
if (_blmi_trace_flags) {                                               \
  switch (op) {                                                        \
    case BLMI_IO_REG_RD:                                               \
    case BLMI_IO_REG_WR:                                               \
        if (_blmi_trace_flags & BLMI_TRACE_REG) {                      \
            _blmi_trace_cb(                                            \
                (dev_info)->addr,                                      \
                op,                                                    \
                io_addr,                                               \
                data,                                                  \
                word_sz,                                               \
                num_entry);                                            \
        }                                                              \
        break;                                                         \
    case BLMI_IO_TBL_RD:                                               \
    case BLMI_IO_TBL_WR:                                               \
        if (_blmi_trace_flags & BLMI_TRACE_TBL) {                      \
            _blmi_trace_cb(                                            \
                (dev_info)->addr,                                      \
                op,                                                    \
                io_addr,                                               \
                data,                                                  \
                word_sz,                                               \
                num_entry);                                            \
        }                                                              \
        break;                                                         \
  }                                                                    \
}
    

#endif /* BLMI_MMI_CMN_H */

