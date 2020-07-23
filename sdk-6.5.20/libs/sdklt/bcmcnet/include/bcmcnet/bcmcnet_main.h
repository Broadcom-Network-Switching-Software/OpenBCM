/*! \file bcmcnet_main.h
 *
 * BCMCNET data structure and APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCNET_MAIN_H
#define BCMCNET_MAIN_H

#include <bcmpkt/bcmpkt_buf.h>
#include <bcmcnet/bcmcnet_types.h>

/*! Maximum number of PDMA devices supported */
#define NUM_PDMA_DEV_MAX        BCMDRD_CONFIG_MAX_UNITS

/*! Maximum number of Rx callbacks */
#define NUM_RX_CB_MAX           8

/*!
 * \brief Device configure structure.
 */
typedef struct bcmcnet_dev_cfg_s {
    /*! Device name */
    char name[DEV_NAME_LEN_MAX];

    /*! Device type string */
    char type_str[DEV_NAME_LEN_MAX];

    /*! Device ID */
    uint32_t dev_id;

    /*! Device mode */
    dev_mode_t mode;

    /*! Number of groups */
    uint32_t nb_grp;

    /*! Bitmap of groups */
    uint32_t bm_grp;

    /*! Rx packet header size */
    uint32_t rx_ph_size;

    /*! Tx packet header size */
    uint32_t tx_ph_size;
} bcmcnet_dev_cfg_t;

/*!
 * \brief Channel configure structure.
 */
typedef struct bcmcnet_chan_cfg_s {
    /*! Channel number */
    int chan;

    /*! Number of descriptors */
    uint32_t nb_desc;

    /*! Rx buffer size */
    uint32_t rx_buf_size;

    /*! Channel control */
    uint32_t chan_ctrl;
    /*! Packet_byte_swap */
#define BCMCNET_PKT_BYTE_SWAP   (1 << 0)
    /*! Non packet_byte_swap */
#define BCMCNET_OTH_BYTE_SWAP   (1 << 1)
    /*! Header_byte_swap */
#define BCMCNET_HDR_BYTE_SWAP   (1 << 2)

    /*! Rx or Tx */
    int dir;
    /*! Rx channel */
#define BCMCNET_RX_CHAN         PDMA_Q_RX
    /*! Tx channel */
#define BCMCNET_TX_CHAN         PDMA_Q_TX
} bcmcnet_chan_cfg_t;

/*!
 * \brief RCPU header structure.
 */
struct bcmcnet_rcpu_hdr {
    /*! Destination MAC address */
    uint8_t dst_mac[6];

    /*! Source MAC address */
    uint8_t src_mac[6];

    /*! VLAN TPID */
    uint16_t vlan_tpid;

    /*! VLAN TCI */
    uint16_t vlan_tci;

    /*! Ethernet type */
    uint16_t eth_type;

    /*! Packet signature */
    uint16_t pkt_sig;

    /*! Operation code */
    uint8_t op_code;

    /*! Flags */
    uint8_t flags;

    /*! Transaction number */
    uint16_t trans_id;

    /*! Packet data length */
    uint16_t data_len;

    /*! Reserved must be 0 */
    uint16_t rsvd0;

    /*! Packet meta data length */
    uint8_t meta_len;

    /*! Transmission queue number */
    uint8_t queue_id;

    /*! Reserved must be 0 */
    uint16_t rsvd1;
};

/*! RCPU Rx operation */
#define RCPU_OPCODE_RX          0x10
/*! RCPU Tx operation */
#define RCPU_OPCODE_TX          0x20

/*! RCPU purge flag */
#define RCPU_FLAG_PURGE         (1 << 0)
/*! RCPU pause flag */
#define RCPU_FLAG_PAUSE         (1 << 1)
/*! RCPU modhdr flag */
#define RCPU_FLAG_MODHDR        (1 << 2)
/*! RCPU bind queue flag */
#define RCPU_FLAG_BIND_QUE      (1 << 3)

/*! Function type for RX packet processing callbacks */
typedef int (*bcmcnet_rx_cb_t)(int unit, int queue, bcmpkt_data_buf_t *buf, void *ck);

/*!
 * \brief Callbacks information structure.
 */
typedef struct bcmcnet_rx_cbs_s {
    /*! Callback */
    bcmcnet_rx_cb_t cb;

    /*! Callback parameter */
    void *ck;
} bcmcnet_rx_cbs_t;

/*!
 * \brief Initialize the core networking subsystem.
 *
 * \param [in] unit Device number.
 * \param [in] dev_cfg Device configure structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_init(int unit, bcmcnet_dev_cfg_t *dev_cfg);

/*!
 * \brief Clean up the core networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_cleanup(int unit);

/*!
 * \brief Configure the core networking subsystem channel.
 *
 * \param [in] unit Device number.
 * \param [in] chan_cfg Channel configure structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_chan_config(int unit, bcmcnet_chan_cfg_t *chan_cfg);

/*!
 * \brief Query the core networking subsystem channel.
 *
 * \param [in] unit Device number.
 * \param [in] chan_cfg Channel configure structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_chan_query(int unit, bcmcnet_chan_cfg_t *chan_cfg);

/*!
 * \brief Start the core networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_start(int unit);

/*!
 * \brief Stop the core networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_stop(int unit);

/*!
 * \brief Suspend the core networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_suspend(int unit);

/*!
 * \brief Resume the core networking subsystem.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_resume(int unit);

/*!
 * \brief Register callback.
 *
 * \param [in] unit Device number.
 * \param [in] cb Callback.
 * \param [in] ck Callback parameter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_rx_cb_register(int unit, bcmcnet_rx_cb_t cb, void *ck);

/*!
 * \brief Unregister callback.
 *
 * \param [in] unit Device number.
 * \param [in] cb Callback.
 * \param [in] ck Callback parameter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_rx_cb_unregister(int unit, bcmcnet_rx_cb_t cb, void *ck);

/*!
 * \brief Sent a Tx packet to the the core networking subsystem.
 *
 * \param [in] unit Device number.
 * \param [in] queue Tx queue.
 * \param [in] buf Tx packet buffer point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_tx(int unit, int queue, bcmpkt_data_buf_t *buf);

/*!
 * \brief Get the core networking subsystem information.
 *
 * \param [in] unit Device number.
 * \param [in] info Device information structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_info_get(int unit, struct bcmcnet_dev_info **info);

/*!
 * \brief Get the kernel networking subsystem stats.
 *
 * \param [in] unit Device number.
 * \param [in] stats Device stats structure point.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_stats_get(int unit, struct bcmcnet_dev_stats **stats);

/*!
 * \brief Reset the kernel networking subsystem stats.
 *
 * \param [in] unit Device number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_stats_reset(int unit);

/*!
 * \brief Dump Rx queue registers.
 *
 * \param [in] unit Device number.
 * \param [in] queue Rx queue.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_rx_queue_reg_dump(int unit, int queue);

/*!
 * \brief Dump Tx queue registers.
 *
 * \param [in] unit Device number.
 * \param [in] queue Tx queue.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_tx_queue_reg_dump(int unit, int queue);

/*!
 * \brief Dump Rx queue DCB ring.
 *
 * \param [in] unit Device number.
 * \param [in] queue Rx queue.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_rx_queue_ring_dump(int unit, int queue);

/*!
 * \brief Dump Tx queue DCB ring.
 *
 * \param [in] unit Device number.
 * \param [in] queue Tx queue.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_XXXX Operation failed.
 */
extern int
bcmcnet_dev_tx_queue_ring_dump(int unit, int queue);

#endif /* BCMCNET_MAIN_H */

