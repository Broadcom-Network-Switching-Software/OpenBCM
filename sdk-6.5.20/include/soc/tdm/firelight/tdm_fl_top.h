/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM top header for Firelight
 */

#ifndef TDM_FL_PREPROCESSOR_DIRECTIVES_H
#define TDM_FL_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_fl_defines.h>
#else
	#include <soc/tdm/firelight/tdm_fl_defines.h>
#endif

#ifdef _TDM_STANDALONE
	#include <tdm_fl_methods.h>
#else
	#include <soc/tdm/firelight/tdm_fl_methods.h>
#endif


#endif /* TDM_FL_PREPROCESSOR_DIRECTIVES_H */
