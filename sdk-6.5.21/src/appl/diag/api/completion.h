/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        completion.h
 * Purpose:     Completion interface
 */

#ifndef   _COMPLETION_H_
#define   _COMPLETION_H_

typedef enum api_mode_completion_type_e {
    api_mode_completion_none,
    api_mode_completion_api
} api_mode_completion_type_t;

extern int api_mode_completion_initialize(void);
extern int api_mode_completion_uninitialize(void);
extern int api_mode_completion_set(api_mode_completion_type_t ctype);

#endif /* _COMPLETION_H_ */
