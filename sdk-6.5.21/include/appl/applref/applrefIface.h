/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef APPLREFIFACE_H
#define APPLREFIFACE_H

/** \struct to invoke a function in applref */
typedef struct ApplRefFunction {
  char *pname;        /**< name of the function */
  char *pbriefdescr;  /**< brief description of the function */
  char *pdescr;       /**< description of the function */
  void *pf;           /**< address of the function */

  /** to keep things symbol, we don't want to implement 
      inferior function call with dummy frame, 
      so a pre-defined dispatch function  is enough to 
      convert char string to correct argument types. */

  int (*dispatch)(void*, int, char *[]); /**< dispatch function */
} ApplRefFunction_t;


#endif
