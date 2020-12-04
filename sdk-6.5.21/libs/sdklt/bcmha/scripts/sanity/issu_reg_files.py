#
# \file issu_reg_files.py
#
# This script contains a class that verifies DRD register files signature
# mismatch.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

import sys
import os
from os import walk
import os.path
import csv

SIGNATURE_FILE_EXT = '.sig'
SIG_FILE = 'reg_sig.db'
CHIP_DIR = 'chip'


class reg_file_sig:
    """Class that compares signature files of two versions and points differences"""

    def __init__(self, base_path):
        self.base_path = base_path

    def comp_versions(self, ver1, ver2):
        dictlist = [dict() for x in range(2)]
        csv_dicts = [dict() for x in range(2)]
        files_dir_list = []
        file_hdl_list = []

        files_dir_list.append(self.base_path + '/ver_' + ver1 + '/' + SIG_FILE)
        files_dir_list.append(self.base_path + '/ver_' + ver2 + '/' + SIG_FILE)

        # Sanity check
        for f in files_dir_list:
            if not os.path.isfile(f):
                return -1

        try:
            for f in files_dir_list:
                fh = open(f, 'r')
                file_hdl_list.append(fh)
        except:
            return -1

        # Read the content of the file into csv dictionary
        idx = 0
        for fh in file_hdl_list:
            csv_dicts[idx] = csv.DictReader(fh, fieldnames = ['dev', 'sig'])
            idx += 1

        for idx in range(0, 2):
            for rec in csv_dicts[idx]:
                dev = rec['dev']
                sig = rec['sig']
                dictlist[idx][dev] = sig

        for dev in dictlist[0]:
            if dictlist[0][dev] != dictlist[1][dev]:
                print('Register file %s changed' % dev)

        # Close the open signature files
        for fh in file_hdl_list:
            fh.close()

        return 0

    # This function saves the signature values from each device register into a
    # single file that is keyed by the device name.
    def save_sig(self, version, fltg_dir):
        dict = {}
        sig_f = self.base_path + '/ver_' + version + '/' + SIG_FILE
        try:
            fh = open(sig_f, 'w')
        except:
            print('Failed to create file %s' % sig_f)
            return

        dev_dv_dir = fltg_dir + '/' + CHIP_DIR
        for (dirpath, dirnames, filenames) in walk(dev_dv_dir):
            for f in filenames:
                if not f.endswith(SIGNATURE_FILE_EXT):
                    continue
                idx = f.find(SIGNATURE_FILE_EXT)
                dev = f[:idx]
                try:
                    sfh = open(dirpath + '/' + f, 'r')
                except:
                    print('failed to open signature file %s' % dirpath + '/' + f)
                    return
                sig = sfh.readline()
                sfh.close()
                dict[dev] = sig

        for dev in sorted (dict.keys()):
            fh.write('%s, %s' % (dev, dict[dev]))
        fh.close()
