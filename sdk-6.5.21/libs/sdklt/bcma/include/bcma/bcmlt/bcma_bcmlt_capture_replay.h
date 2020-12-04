/*! \file bcma_bcmlt_capture_replay.h
 *
 * Functions related to the LT capture and replay in BCMLT module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMLT_CAPTURE_REPLAY_H
#define BCMA_BCMLT_CAPTURE_REPLAY_H

#include <shr/shr_thread.h>

/*! Parameters for LT capture API (\ref bcma_bcmlt_capture_start). */
typedef struct bcma_bcmlt_capture_params_s {

    /*! The file to store the captured raw data (in binary) if not NULL. */
    char *cap_file;

    /*!
     * If not NULL, the captured raw data will be decoded to CLI commands
     * in real-time and saved to the specified file (in text).
     */
    char *rtrp_text_file;

    /*!
     * If set to true, the captured raw data will be decoded to CLI commands
     * in real-time and displayed in console immediately.
     */
    bool rtrp_console_output;

    /*! Skip the read operation in capture if set to true. */
    bool write_only;

} bcma_bcmlt_capture_params_t;

/*!
 * \brief Start the LT capture feature in BCMLT.
 *
 * If this function returns successfully, the LT operations will be captured
 * until \ref bcma_bcmlt_capture_stop is called.
 *
 * \param [in] unit Unit number.
 * \param [in] cparams Capture parameters.
 *
 * \retval SHR_E_NONE LT capture is started successfully.
 * \retval SHR_E_EXISTS Capture is already started for the specified unit.
 * \retval SHR_E_XXX Other errors.
 */
extern int
bcma_bcmlt_capture_start(int unit, bcma_bcmlt_capture_params_t *cparams);

/*!
 * \brief Stop the LT capture feature in BCMLT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE LT capture is stopped successfully.
 * \retval SHR_E_NOT_FOUND LT capture is not started.
 * \retval SHR_E_XXX Other errors.
 */
extern int
bcma_bcmlt_capture_stop(int unit);

/*!
 * \brief Indicate whether the LT capture is started.
 *
 * \param [in] unit Unit number.
 *
 * \return TRUE if the capture is started, otherwise FALSE.
 */
extern bool
bcma_bcmlt_capture_started(int unit);

/*!
 * \brief Get the current capture parameters.
 *
 * \param [in] unit Unit number.
 * \param [out] cparams Return the current capture parameters.
 *
 * \retval SHR_E_NONE The capture parameters is retrieved successfully.
 * \retval SHR_E_NOT_FOUND LT capture is not started.
 * \retval SHR_E_XXX Other errors.
 */
extern int
bcma_bcmlt_capture_params_get(int unit, bcma_bcmlt_capture_params_t *cparams);

/*!
 * Parameters for replaying the LT captured raw data
 * (\ref bcma_bcmlt_replay_thread_start).
 */
typedef struct bcma_bcmlt_replay_params_s {

    /*!
     * The name of log file to be replayed. This file is a captured raw data
     * file saved from \ref bcma_bcmlt_capture_start and
     * \ref bcma_bcmlt_capture_stop.
     */
    char *cap_file;

    /*! The replay will take effect if set to true. */
    bool submit;

    /*!
     * The original capture timing will be referenced in replay
     * if set to true.
     */
    bool timing;

    /*!
     * If not NULL, the captured raw data from \c cap_file will be decoded
     * to CLI commands and saved to the specifed file.
     */
    char *text_file;

    /*!
     * If set to true, the captured raw data from \c cap_file will be decoded
     * to CLI commands and displayed in console.
     */
    bool console_output;

    /*!
     * If set to true, the decoded CLI commands will be in raw staged result.
     * Otherwise the the CLI friendly command will be generated for the replay
     * to run without errors.
     * Note that raw=0 is supported only in dry-run mode (submit=0).
     */
    bool raw;

    /*!
     * Customized replay action callback from BCMLT replay engine.
     * If NULL, the default replay action is to decode the captured raw data
     * to CLI text commands.
     * Note that the \c text_file, \c console_output and \c raw parameters are
     * valid only when \c custom_replay_act is NULL.
     */
    bcmlt_replay_action_f custom_replay_act;

    /*!
     * Customized replay data read callback from BCMLT replay engine.
     * If NULL, the default replay data read callback is to read captured data
     * from \c cap_file.
     */
    bcmlt_replay_cb_t *custom_replay_cb;

} bcma_bcmlt_replay_params_t;

/*!
 * \brief Callback function when the BCMLT replay engine thread is done.
 *
 * \param [in] unit Unit number.
 * \param [in] rv Return value of the BCMLT replay engine.
 */
typedef void (*bcma_bcmlt_replay_done_f)(int unit, int rv);

/*!
 * \brief Start a thread to run the BCMLT replay engine.
 *
 * If this function returns successfully, a separate thread will be started
 * to run the BCMLT engine for the specified unit.
 *
 * \param [in] unit Unit number.
 * \param [in] rparams Replay parameters.
 * \param [in] rp_done_cb If not NULL, the function will be called when the
 *                        replay thread is done.
 *
 * \retval SHR_E_NONE The LT replay engine is started successfully.
 * \retval SHR_E_EXISTS Replay is already started for the specified unit.
 * \retval SHR_E_XXX Other errors.
 */
extern int
bcma_bcmlt_replay_thread_start(int unit, bcma_bcmlt_replay_params_t *rparams,
                               bcma_bcmlt_replay_done_f rp_done_cb);

/*!
 * \brief Stop the BCMLT replay engine thread.
 *
 * Stop the BCMLT replay engine and terminate the replay thread.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE The LT replay engine is stopped successfully.
 * \retval SHR_E_NOT_FOUND LT replay engine thread is not started.
 * \retval SHR_E_XXX Other errors.
 */
extern int
bcma_bcmlt_replay_thread_stop(int unit);

#endif /* BCMA_BCMLT_CAPTURE_REPLAY_H */
