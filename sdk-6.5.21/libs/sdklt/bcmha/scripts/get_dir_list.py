#
# \file get_dir_list.py
#
# This file prepares the list of directories that should be included in the
# DLL.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
import sys
import os
import getopt
import issu_version_list

ISSU_DB_DEF_DIR = '/bcmissu/dll/db/'


def usage():
    print('python get_dir_list.py -v <cur-ver> -s <start-ver> [-d <issu-db-dir]')
    print('Where issu-db-dir is the root directory for the ISSU DB (default ' \
          '$SDK/bcmissu/dll/db).')
    print('The -v and -s flags are mandatory')


def main(argv):
    current_ver = ''
    start_ver = ''
    output_ver = ''
    overwrite_db_root_dir = ''

    try:
        opts, args = getopt.getopt(argv, "hv:s:d:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit(0);
        elif opt == '-v':
            current_ver = arg
        elif opt == '-s':
            start_ver = arg
        elif opt == '-d':
            overwrite_db_root_dir = arg
        else:
            usage()
            sys.exit(0);

    if start_ver == '' or current_ver == '':
        sys.exit(-1)

    try:
        sdk = os.environ['SDK']
    except:
        print("SDK Environment variable must be defined")
        sys.exit(0);

    if overwrite_db_root_dir == '':
        issu_db_dir = sdk + ISSU_DB_DEF_DIR
    else:
        issu_db_dir = overwrite_db_root_dir
    ver_list_c = issu_version_list.issu_version_list(issu_db_dir,
                                                     start_ver,
                                                     current_ver)
    rev_version_list = ver_list_c.get_ver_list()
    for v in rev_version_list:
        if output_ver != '':
            output_ver += ' '
        output_ver += 'ver_' + v
    sys.stdout.write(output_ver + '\n')


if __name__ == "__main__":
    if sys.version_info[0] < 2 or \
       (sys.version_info[0] == 2 and sys.version_info[1] < 7):
        print ('Error: Unsupported python version, min version 2.7.x')
    else:
        main(sys.argv[1:])
