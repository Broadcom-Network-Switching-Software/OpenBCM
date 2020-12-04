/*! \file cli_internal.h
 *
 * CLI definitions intended for internal use only.
 *
 * Application code should normally not need to include this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_CLI_INT_H
#define BCMA_CLI_INT_H

#include <sal/sal_internal.h>

/*! Signature for sanity checks. */
#define CLI_SIGNATURE   SAL_SIG_DEF('C', 'L', 'I', '>')

/*! Check that this is a valid CLI object. */
#define BAD_CLI(_cli)   ((_cli)->cli_sig != CLI_SIGNATURE)

/*! Check that this CLI object was dynamically allocated. */
#define BAD_OBJ(_cli)   ((_cli)->dyn_sig != CLI_SIGNATURE)

#endif /* BCMA_CLI_INT_H */
