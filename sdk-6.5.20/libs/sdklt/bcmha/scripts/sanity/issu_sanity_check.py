#
# \file issu_sanity_check.py
#
# This script checks if an upgrade from one version to another is sane. The
# test doesn't fail but merely produces a report indicating the potential
# issues that might impact proper ISSU operation.
# It is recommend to run this script weekly between any two consecutive
# versions N and N+1 (i.e. 1.3.2 and 1.3.3). However, the script can run
# between any two versions X & Y assuming that the ISSU DB between these
# two versions is current.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

import sys
import os
import csv
import getopt
import yaml  # YAML parser
from collections import OrderedDict  # keep YAML parser output in order
from yaml.resolver import Resolver
import shutil
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/..')
import issu_version_list
import issu_data_struct_valid
import issu_reg_files

DEFAULT_ISSU_DB_DIR = '/bcmissu/dll/db'
FLTG_INPUT_DIR = '/devdb'


def clean_python_default_boolean():
    for ch in "OoYyNn":
        if len(Resolver.yaml_implicit_resolvers[ch]) == 1:
            del Resolver.yaml_implicit_resolvers[ch]
        else:
            Resolver.yaml_implicit_resolvers[ch] = [x for x in
                    Resolver.yaml_implicit_resolvers[ch] if x[0] != 'tag:yaml.org,2002:bool']


def usage():
    print("python issu_sanity_check.py -s <start-ver> -v <version> [-h]")
    print("       [-d <root-db-dir>]")
    print('root-db-dir allows to overwrite the default ISSU DB dir ' \
          '$SDK/bcmissu/dll/db')


# The main function.
# Parse input parameters and invoke the relevant sanity tests.
def main(argv):
    start_ver = None
    to_ver = None
    base_dir = None
    version_list = []

    try:
        opts, args = getopt.getopt(argv, "hs:v:d:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(-2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit(0);
        elif opt == '-v':
            to_ver = arg
        elif opt == '-s':
            start_ver = arg
        elif opt == '-d':
            base_dir = arg
        else:
            usage()
            sys.exit(0);

    if start_ver == None or to_ver == None:
            usage()
            sys.exit(0);

    if base_dir == None:
        try:
            base_dir = os.environ['SDK'] + DEFAULT_ISSU_DB_DIR
        except:
            print('the environment variable SDK must be defined')
            sys.exit(-1)

    # Make sure that our YAML files will be processed correctly
    clean_python_default_boolean()

    # Build the list of all the versions from start_ver to to_ver.
    # The list is sorted in reverse order.
    ver_list_c = issu_version_list.issu_version_list(base_dir, start_ver, to_ver)
    version_list = ver_list_c.get_ver_list(reverse = False)

    # Start with DRD register signature compare
    reg_file_c = issu_reg_files.reg_file_sig(base_dir)
    for idx in range(0, len(version_list) - 1, 1):
        reg_file_c.comp_versions(version_list[idx], version_list[idx + 1])

    # Validate the data structures
    issu_dstruct_c = issu_data_struct_valid.issu_data_validate(base_dir)
    for idx in range(0, len(version_list) - 1, 1):
        issu_dstruct_c.check_versions(version_list[idx], version_list[idx + 1])


if __name__ == "__main__":
    if sys.version_info[0] < 2 or \
       (sys.version_info[0] == 2 and sys.version_info[1] < 7):
        print('Error: Unsupported python version, min version  2.7.x')
    else:
        main(sys.argv[1:])
