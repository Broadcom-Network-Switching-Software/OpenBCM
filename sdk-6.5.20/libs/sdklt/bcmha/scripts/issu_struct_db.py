#
# \file issu_struct_db.py
#
# This file prepares the information from previous versions required
# to perform the data structure upgrade.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
import sys
import os
import getopt
import issu_db_gen
import issu_patch_insert
import issu_version_list
sys.path.append(os.path.dirname(os.path.abspath(__file__)) + '/sanity')
import issu_reg_files
from filelock import Timeout, FileLock

ISSU_DB_DEF_DIR = '/bcmissu/dll/db/'
FLTG_INPUT_DIR = '/devdb'


def usage():
    print("python issu_struct_db.py -v <cur-ver> [-s <start-ver>] [-d <src-dir]")
    print("where -d <src-dir> allows to define the root of the devdb directory")
    print("the default used is $SDK/devdb")


# The main function.
# This function generates the DB for the ISSU by looking at multiple versions.
# The function creates a list of all available versions from the start to
# the current version. It then introduces each version in the list to the
# global_struct_db class. The introduction starts from the 'start' version
# upward until the current version. During the version feed, the
# global_struct_db class keeps calculating the differences between previous
# versions and current one. Structures that had changed are added to the
# data structure associated with the version input.
def main(argv):
    current_ver = ''
    start_ver = ''
    version_list = []
    rev_version_list = []  # Version list in reverse order
    fltg_root_dir = None

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
            fltg_root_dir = arg
        else:
            usage()
            sys.exit(0);

    if start_ver == '':
        start_ver = current_ver

    try:
        sdk = os.environ['SDK']
    except:
        print("SDK Environment variable must be defined")
        sys.exit(0);

    try:
        issu_db_dir = os.environ['ISSU_DB'] + '/'
    except:
        issu_db_dir = sdk + ISSU_DB_DEF_DIR

    base_dir = issu_db_dir
    ver_list_c = issu_version_list.issu_version_list(base_dir,
                                                     start_ver,
                                                     current_ver)
    rev_version_list = ver_list_c.get_ver_list()
    version_list = ver_list_c.get_ver_list(reverse = False)

    # Build the base dir
    if current_ver == '':
        base_dir += 'current'
    else:
        base_dir += 'ver_' + current_ver
    target_dir = base_dir

    # Lock the DB to enable multiple simultaneous builds. If the lock is taken
    # it means that other build is building the DB. Let it finish first before
    # we can jump to the next phase of compiling the DLL
    lock_file = issu_db_dir + 'issu_db.lock'
    try:
        lock = FileLock(lock_file, 1800)
        lock.acquire()  # wait up to 30 min
    except Timeout:
        print('Failed to acquire ISSU DB lock')
        sys.exit(-1)

    db_class = issu_db_gen.global_struct_db(base_dir, None)
    if not db_class.generate_c_db(current_ver, version_list):
        print('Failed to generate DB for version %s' % current_ver)

    base_dir = issu_db_dir + 'ver_'
    for ver in rev_version_list:
        if ver != current_ver:
            db_class.parse_previous_versions(base_dir, ver)

    # Final generation and shutdown the DB gen class
    db_class.shutdown()

    # Save reg file signatures into the current version DB
    if current_ver != '':
        if fltg_root_dir == None:
            fltg_root_dir = sdk + FLTG_INPUT_DIR
        reg_file_c = issu_reg_files.reg_file_sig(issu_db_dir)
        reg_file_c.save_sig(current_ver, fltg_root_dir)

    # Generate the patch list
    patch_class = issu_patch_insert.issu_patch(issu_db_dir)
    patch_class.read_patch_db(version_list)
    patch_class.create_patch_c_file()
    patch_class.create_patch_makefile()
    lock.release()


if __name__ == "__main__":
    if sys.version_info[0] < 2 or \
       (sys.version_info[0] == 2 and sys.version_info[1] < 7):
        print ('Error: Unsupported python version, min version 2.7.x')
    else:
        main(sys.argv[1:])

