/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

Diagnostic Tests

Notes
-----

Diag tests are not expected to clean up after themselves.
However, each test is expected to initialize the chip properly before
performing its work.  Many of the tests, such as port loopbacks, do this
by calling soc_reset() or by running rc.soc prior to the test.
