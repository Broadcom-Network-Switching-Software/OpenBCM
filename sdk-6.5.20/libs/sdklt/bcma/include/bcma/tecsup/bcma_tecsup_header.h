/*! \file bcma_tecsup_header.h
 *
 * Techsupport report utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TECSUP_HEADER_H
#define BCMA_TECSUP_HEADER_H

#include <shr/shr_pb.h>

/*!
 * \brief Display techsupport welcome message.
 *
 * \param[in] unit Unit number.
 * \param[in] pb Print buffer.
 *
 * \return Always 0.
 */
extern int
bcma_tecsup_header(int unit, shr_pb_t *pb);

#endif /* BCMA_TECSUP_HEADER_H */
