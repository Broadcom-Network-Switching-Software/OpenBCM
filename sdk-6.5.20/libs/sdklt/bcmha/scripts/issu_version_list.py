#
# \file issu_version_list.py
#
# This file contains a class that builds a list of all the versions starting
# from start version to destination version. The list will be lexicographically
# sorted based on the version number.
# The class also provide a method to retrieve the list in reverse order.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
import sys
import os
from os import walk

class issu_version_list:
    """Class that builds a list of ISSU version directories"""

    # Initializing the class.
    # base_path is the path to the ISSU DB directory
    # start_ver is the version to start from
    # to_ver is the last version
    def __init__(self, base_path, start_ver, to_ver):
        dirs = []
        self.version_list = []

        if to_ver == '' or start_ver == '':
            return

        # Get the list of all the directories
        for (dirpath, dirnames, filenames) in walk(base_path):
            dirs.extend(dirnames)
            break

        for d in dirs:
            if d.find('ver_') != -1:
                self.version_list.append(d[4:])

        # In-place reverse sorting so the highest version is first. We are using
        # proprietary compare function since the standard compare function compares
        # strings by comparing character by character. That will lead to
        # 1.2.9 > 1.2.10 which is not what we want.
        self.version_list.sort(cmp=lambda x,y: self.__my_comp_func(x,y), reverse=True)

        # Delete all the versions that are predecessors to the start version
        if start_ver in self.version_list:
            idx = self.version_list.index(start_ver) + 1
            while idx < len(self.version_list):
                try:
                    self.version_list.pop(idx)
                except:
                    return

        if to_ver in self.version_list:
            while self.version_list[0] != to_ver:
                try:
                    self.version_list.pop(0)
                except:
                    return

    # Compare two segment strings. Each string composed of a number that might
    # be followed by an alphabet letter and another number. For example 2.10A
    # or 2.10A1.
    def __comp_numbers(self, x, y):
        # Since this function can be called recursively we need to terminate for
        # empty strings
        if len(x) == 0 and len(y) == 0:
            return 0
        elif len(x) == 0:
            return -1
        elif len(y) == 0:
            return 1

        if len(x) > len(y):
            cnt = len(y)
            max_cnt = len(x)
        else:
            cnt = len(x)
            max_cnt = len(y)

        # We typically start with digits so find the max length of digits on
        # both string and see if we can compare it to each other.
        no_digit = False
        for i in range(0,cnt):
            if x[i].isalpha() or y[i].isalpha():
                no_digit = True
                break;

        # For all digit strings with equal length (easy case)
        if len(x) == len(y) and (not no_digit):
            if int(x) > int(y):
                return 1
            elif int(x) < int(y):
                return -1
            else:
                return 0

        # The i index encounter a letter. See if that occur in both strings
        if no_digit:
            if x[i].isdigit():
                return 1
            elif y[i].isdigit():
                return -1
        else:   # I searched all the way. See if one string has more digits
            if len(x)-1 > i and x[i+1].isdigit():
                return 1
            elif i < len(y)-1 and y[i+1].isdigit():
                return -1
            else: # Both had the same number of digits but different overall length
                if int(x[:i+1]) > int(y[:i+1]):
                    return 1
                elif int(x[:i+1]) < int(y[:i+1]):
                    return -1

        # start from the index that was not digit
        if no_digit:
            start = i
        else:
            start = i+1
        for i in range(start,max_cnt):
            # If exceeding one of the strings length we done.
            if i >= len(x):
                return -1
            if i >= len(y):
                return 1

            # Compare letters
            if x[i].isalpha() and y[i].isalpha():
                if x[i] > y[i]:
                    return 1
                elif x[i] < y[i]:
                    return -1
            elif x[i].isdigit() and y[i].isdigit():
                # This will be used only when we start to have digits again for
                # example compare 1.2B3 to 1.2B4
                return self__comp_numbers(x[i:], y[i:])
            elif x[i].isdigit():  # Digit is larger then letter
                return 1
            else:
                return -1

        return 0


    # Compare two segments of strings where the idx_xxx is the index of the dot
    # ('.') character (if exist) or has the -1 value if it is the last segment
    # of the compared segments.
    def __compare_segment(self, x, y, idx_x, idx_y):
        if idx_x == -1 and idx_y == -1:
            return self.__comp_numbers(x, y), True
        elif idx_x == -1:
            v = self.__comp_numbers(x, y[:idx_y])
            if v == 0:
                return -1, True
            else:
                return v, True
        elif idx_y == -1:
            v = self__comp_numbers(x[:idx], y)
            if v == 0:
                return 1, True
            else:
                return v, True

        return 0, False

    # This is a compare function of two versions strings (only numbers separated by
    # dots). The function breaks each version string into segments (starting from
    # the left most segment). If equal, continue to the next segment.
    def __my_comp_func(self, x, y):
        idx_x = x.find('.')
        idx_y = y.find('.')
        v, final = self.__compare_segment(x, y, idx_x, idx_y)
        if final:
            return v
        start_x = 0
        start_y = 0
        while idx_x != -1 and idx_y != 0:
            v = self.__comp_numbers(x[start_x:start_x+idx_x], y[start_y:start_y+idx_y])
            if v != 0:
                return v
            start_x += idx_x + 1
            start_y += idx_y + 1
            idx_x = x[start_x:].find('.')
            idx_y = y[start_y:].find('.')
        v, final = self.__compare_segment(x[start_x:], y[start_y:], idx_x, idx_y)

        if not final:
            print('Invalid directory compare')
            return 0
        else:
            return v

    def get_ver_list(self, reverse=True):
        ret_list = []
        if reverse:
            return self.version_list
        for idx in range(len(self.version_list)-1, -1, -1):
            ret_list.append(self.version_list[idx])
        return ret_list