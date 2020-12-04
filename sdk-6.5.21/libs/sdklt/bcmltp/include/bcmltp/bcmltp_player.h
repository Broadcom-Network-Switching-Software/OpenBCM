/*! \file bcmltp_player.h
 *
 * BCMLTP playback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTP_PLAYER_H
#define BCMLTP_PLAYER_H

/*!
 * \brief Play back BCMCFG data.
 *
 * Play back data recorded by BCMCFG readers to logical and physical
 * tables. This may be called multiple times, and will play back the
 * same logical table sequence each time.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
int bcmltp_play(int unit);

#endif /* BCMLTP_PLAYER_H */
