/*! \file bcmcth_mon_flowtracker_flow_entry.h
 *
 * BCMCTH Flowtracker LT MON_FLOWTRACKER_LEARN_FLOW_ENTRY
 * related definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_FLOWTRACKER_FLOW_ENTRY_H
#define BCMCTH_MON_FLOWTRACKER_FLOW_ENTRY_H

#include <bcmltd/chip/bcmltd_common_enum_ctype.h>

/*!
 * \brief Insert dummy entry into MON_FLOWTRACKER_LEARN_FLOW_ENTRY LT.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe on which flow was learnt.
 * \param [in] mode Exact match index mode of the flow learnt (Single,double,quad)
 * \param [in] em_idx Exact match index of the flow learnt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_learn_flow_entry_dummy_insert(
         int unit,
         int pipe,
         bcmltd_common_flowtracker_hw_learn_exact_match_idx_mode_t_t mode,
         uint32_t em_idx
         );

/*!
 * \brief Delete an entry from MON_FLOWTRACKER_LEARN_FLOW_ENTRY LT.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe on which flow was learnt.
 * \param [in] mode Exact match index mode of the flow learnt (Single,double,quad)
 * \param [in] em_idx Exact match index of the flow learnt.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_mon_ft_learn_flow_entry_delete(
         int unit,
         int pipe,
         bcmltd_common_flowtracker_hw_learn_exact_match_idx_mode_t_t mode,
         uint32_t em_idx
         );
#endif /* BCMCTH_MON_FLOWTRACKER_FLOW_ENTRY_H */
