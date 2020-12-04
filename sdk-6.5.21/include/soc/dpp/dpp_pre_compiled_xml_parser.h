/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#include <sal/types.h>
#include <shared/shrextend/shrextend_error.h>

#ifndef _PRE_COMPILED_XML_PARSER_H
#define _PRE_COMPILED_XML_PARSER_H

shr_error_e pre_compiled_xml_to_buf(
    int unit,
    char *filePath,
    char **buf,
    long int *size_p);

int pre_compiled_xml_file_exists(
    char *filepath);

#endif 
