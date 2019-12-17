/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

extern char *kconfig_get(const char *name);
extern int kconfig_get_next(char **name, char **value);
extern int kconfig_set(char *name, char *value);
