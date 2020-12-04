/*! \file bcmbd_qspi.h
 *
 * BD QSPI API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_QSPI_H
#define BCMBD_QSPI_H


/*!
 * QSPI configuration flags.
 * \anchor BCMBD_QSPI_MODE_
 */

/*! \{ */

/*! Clock phase. */
#define BCMBD_QSPI_MODE_CPHA (1 << 0)

/*! Clock polarity. */
#define BCMBD_QSPI_MODE_CPOL (1 << 1)

/*! \} */

/*!
 * QSPI transfer flags.
 * \anchor BCMBD_QSPI_TRANS_
 */

/*! \{ */

/*! Transfer begin. */
#define BCMBD_QSPI_TRANS_BEGIN   (1 << 0)

/*! Transfer end. */
#define BCMBD_QSPI_TRANS_END     (1 << 1)

/*! \} */

/*!
 * \brief QSPI configurations.
 */
typedef struct bcmbd_qspi_conf_s {
    /*! Mode bits. /ref BCMBD_QSPI_MODE_ */
    uint32_t mode;

    /*! Maximum frequency in Hz. */
    uint32_t max_hz;

} bcmbd_qspi_conf_t;

/*!
 * Initialize QSPI interface.
 *
 * \param [in] unit Unit number.
 * \param [in] conf Parameters for initialization.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 */
extern int
bcmbd_qspi_init(int unit, bcmbd_qspi_conf_t *conf);

/*!
 * Deinitialize QSPI interface.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_qspi_cleanup(int unit);

/*!
 * \brief Transfer data via QSPI interface.
 *
 * The \c flag is used to enclose the QSPI data into a transaction.
 *
 * The transferred data starts from BCMBD_QSPI_TRANS_BEGIN to
 * BCMBD_QSPI_TRANS_END will be treated as one command.
 *
 * \param [in] unit Unit number.
 * \param [in] flag Transfer flag. \ref BCMBD_QSPI_TRANS_.
 * \param [in] tx_bytes Bytes to transmit.
 * \param [in] tx Transmit buffer.
 * \param [in] rx_bytes Bytes to receive.
 * \param [out] rx Receive buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation timeout.
 */
extern int
bcmbd_qspi_transfer(int unit, uint32_t flag, size_t tx_bytes, uint8_t *tx,
                    size_t rx_bytes, uint8_t *rx);

/*!
 * \brief Transfer one complete command via QSPI interface.
 *
 * This is a wrapper function of \ref bcmbd_qspi_transfer().
 * Each call to this function should always be a complete command.
 *
 * \param [in] unit Unit number.
 * \param [in] tx_bytes Bytes to transmit.
 * \param [in] tx Transmit buffer.
 * \param [in] rx_bytes Bytes to receive.
 * \param [out] rx Receive buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware access returned an error.
 * \retval SHR_E_TIMEOUT Operation timeout.
 */
extern int
bcmbd_qspi_transfer_one(int unit, size_t tx_bytes, uint8_t *tx,
                        size_t rx_bytes, uint8_t *rx);

#endif /* BCMBD_QSPI_H */
