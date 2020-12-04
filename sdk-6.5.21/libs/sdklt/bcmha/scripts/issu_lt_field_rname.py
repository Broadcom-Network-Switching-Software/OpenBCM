#
# \file issu_lt_field_rname.py
#
# This script handles the case of renaming a field within an LT.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

import sys
import os
import shutil
import getopt
import lt_field_rename_report

ISSU_DB_DEF_DIR = '/bcmissu/dll/'


def usage():
    print("python lt_field_rename_report.py -l <lt_name> -f <old_field_name> -n <new_field_name> \
          -v <version> [-d <device> -s <variant>]")


# The main function.
# Parse input parameters, open output file and call the file
# parser.
def main(argv):
    lt_name = ''
    old_fld_name = ''
    new_fld_name = ''
    version = ''
    device = ''
    variant = ''

    try:
        opts, args = getopt.getopt(argv, "hl:f:n:v:d:s:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit(0);
        elif opt == '-l':
            lt_name = arg
        elif opt == '-f':
            old_fld_name = arg
        elif opt == '-n':
            new_fld_name = arg
        elif opt == '-v':
            version = arg
        elif opt == '-d':
            device = arg
        elif opt == '-s':
            variant = arg
        else:
            usage()
            sys.exit(0)

    try:
        sdk = os.environ['SDK']
    except:
        print('The environment variable SDK must be defined')
        sys.exit(-1)

    if lt_name == '' or old_fld_name == '' or new_fld_name == '' or version == '':
        print('Missing parameter')
        usage()
        sys.exit(0);

    try:
        lt_fld_cls = lt_field_rename_report.lt_field_rename(sdk + ISSU_DB_DEF_DIR)
    except:
        print('Failed to create report')
        sys.exit(-1)

    lt_fld_cls.add_field(lt_name, old_fld_name, new_fld_name, version,
                         device, variant)


if __name__ == "__main__":
    if sys.version_info[0] < 2 or \
       (sys.version_info[0] == 2 and sys.version_info[1] < 7):
        print('Error: Unsupported python version, min version  2.7.x')
    else:
        main(sys.argv[1:])
