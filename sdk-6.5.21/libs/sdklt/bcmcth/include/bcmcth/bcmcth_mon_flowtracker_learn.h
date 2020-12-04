/*! \file bcmcth_mon_flowtracker_learn.h
 *
 * BCMCTH Flowtracker learn thread related definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMCTH_MON_FLOWTRACKER_LEARN_H
#define BCMCTH_MON_FLOWTRACKER_LEARN_H
#include <shr/shr_thread.h>
#include <bcmltd/chip/bcmltd_mon_constants.h>
/*!
 * Aggressive polling interval in microseconds used for sleeping
 * when there are unread fifo entries
 */
#define FT_LEARN_THREAD_AGGRESSIVE_INTVL 100

/*! Flowtracker learn thread control structure per pipe */
typedef struct bcmcth_mon_flowtracker_learn_ctrl_pipe_s {
    /*! Thread handle */
    shr_thread_t thread_ctrl;

    /*! Is thread active ? */
    bool is_active;

    /*! Thread sleep interval */
    sal_usecs_t sleep_intvl;
}bcmcth_mon_flowtracker_learn_ctrl_pipe_t;

/*! Flowtracker learn thread control structure per unit */
typedef struct bcmcth_mon_flowtracker_learn_ctrl_s {

    /*! Per pipe information of learn threads */
    bcmcth_mon_flowtracker_learn_ctrl_pipe_t *pipe_info;

    /*! Per pipe information of learn fail threads */
    bcmcth_mon_flowtracker_learn_ctrl_pipe_t *fail_pipe_info;

}bcmcth_mon_flowtracker_learn_ctrl_t;

/*! Flowtracker learn thread debug info structure */
typedef struct bcmcth_mon_flowtracker_learn_ctrl_debug_s {

    /*! Number of pipes */
    int num_pipes;

    /*! Per pipe information */
    bcmcth_mon_flowtracker_learn_ctrl_pipe_t pipe_info[FLOWTRACKER_PIPES_MAX];

}bcmcth_mon_flowtracker_learn_ctrl_debug_t;

/*!
 * \brief Dump the FT learn threads info.
 *
 * \param [in]  unit Unit number.
 * \param [out] info Info structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int bcmcth_mon_ft_learn_thread_info_dump(
                        int unit,
                        bcmcth_mon_flowtracker_learn_ctrl_debug_t *info);

/*!
 * \brief Get the sleep interval of the FT learn thread.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pipe Pipe number.
 * \param [out]  intvl Sleep interval.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_learn_thread_intvl_get(int unit, int pipe, sal_usecs_t *intvl);

/*!
 * \brief Set the sleep interval of the FT learn thread.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pipe Pipe number.
 * \param [in]  intvl Sleep interval.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_learn_thread_intvl_set(int unit, int pipe, sal_usecs_t intvl);

/*!
 * \brief De-initialize the FT learn threads.
 *
 * \param [in]  unit Unit number.
 *
 * \retval None.
 */
extern int bcmcth_mon_ft_learn_threads_deinit(int unit);

/*!
 * \brief Initialize the FT learn threads.
 *
 * \param [in]  unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int bcmcth_mon_ft_learn_threads_init(int unit);
#endif /* BCMCTH_MON_FLOWTRACKER_LEARN_H */
