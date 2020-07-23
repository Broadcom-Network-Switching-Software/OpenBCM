#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
#!/usr/bin/env python
import os
import argparse
import re
import collections
from xml.etree.ElementTree import Element, SubElement, Comment, tostring
import xml.etree.ElementTree as ET
from collections import Iterable

info = list()
entry_info = {}
skip_headers = list()

def cmd_examples_main(device_id, input, output):

    device_um_xml = os.environ["SDK"] + "/doc/sand/{}_UM_ShellCmd.xml".format(device_id)

    with open(input, "r") as f:
        info = f.read()

    info = info.split("\nCommand:\'")[1:]

    tree = ET.parse(device_um_xml)
    root = tree.getroot()

    examples_list = list()
    general_examples = list()
    Start_from_first_exec = False
    for tag in root.iter('Examples'):
        for item in tag:
            if re.search(item.get("command"), info[0]) and item.get("command") != '' and Start_from_first_exec == False:
                Start_from_first_exec = True
            if not Start_from_first_exec:
                general_examples.append(item.get("command"))
                continue
            examples_list.append(item.get("command"))

    commands = collections.OrderedDict()
    for example in examples_list:
        if not example.split(" ")[0] in commands.keys():
            commands.setdefault(example.split(" ")[0], list())

    for example in general_examples:
        if not example.split(" ")[0] in commands.keys():
            commands.setdefault(example.split(" ")[0], list())

    example_info = collections.OrderedDict()
    for cmd in commands:
        example_info.setdefault(cmd, list())
        for line in info:
            if re.search("^"+cmd+" " ,line):
                example_info[cmd].append(line)
                commands[cmd].append(line.split('\n')[0].replace("\'", ""))

    example_info.setdefault('general', list())
    commands.setdefault('general', list())
    for example in general_examples:
        if example != "":
            commands['general'].append(example)
            example_info['general'].append("")

    output_file = output + device_id + '_general.html'
    create_html(example_info['general'], output_file, commands['general'])

    for cmd in commands:
        if cmd.lower().strip():
            output_file = output + device_id + "_" + cmd.lower() + '.html'
            create_html(example_info[cmd], output_file, commands[cmd])

def create_html(input, output_file, commands):

    String = """<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>Examples Results</title>
        <style>
              body {font-family: Arial;}
              h1 {font-size: 18pt; font-weight: bold;}
              h2 {font-size: 16pt; font-weight: bold;}
              h3 {font-size: 14pt; font-weight: bold;}
              h4 {font-size: 12pt; font-weight: bold;}
              h5 {font-size: 11pt; font-weight: bold;}
              p {font-size: 10pt;}
              th {font-size: 9pt; font-weight: bold;}
              td {font-size: 9pt;}
        </style>
    </head>
    <body>
        </p><a name="tocLink">
            <h1><font color="CC092F">Table of Contents</font></h1></a>
            <table style="width:30%" border="1" cellpadding="5">
                <tr bgcolor="#D0D0CE">
                    <th>#</th>
                    <th>Commands</th>
                </tr>"""

    for index, command in enumerate(commands):
        String +="""
                <tr>
                    <td>{index}.</td>
                    <td><a href="#{cmd}">{cmd}</a></td>
                </tr>""".format(index=index, cmd=command.strip())
    String +="""
            </table>"""

    for index, command in enumerate(commands):
        result = ('\n'.join(input[index].split('\n')[1:-1]))
        if 'Commands Examples Execution' in result:
            for id, line in enumerate(result.splitlines()):
                if 'Commands Examples Execution' in line:
                    break
            result = '\n'.join(result.split('\n')[0: (id -2)])
        result = result.replace('\n', '&#13;&#10;').replace('\r', '')
        result = re.sub("\033\[[0-9;]+m", "", result)
        String += """
        <h5><a name="{cmd}">{cmd}</a></h5>
        <textarea cols='180' rows='{rows_count}' >{info}</textarea>
""".format(cmd=command.strip(), info=result, rows_count=len(result.split('&#13;&#10;')))

        String += """
    <p><a href="#tocLink">Table of Contents</a></p>
"""

    String += """
    </body>
</html>
"""
    with open(output_file, 'w+') as f:
        f.write(String)

if __name__ == '__main__' :

    parser = argparse.ArgumentParser(description='')

    parser.add_argument('--device_id', default="88690", help=argparse.SUPPRESS)
    parser.add_argument('--input', default=os.environ["SDK"] + "/tools/sand/manual/88690_exec_log.txt", help=argparse.SUPPRESS)
    parser.add_argument('--output', default=os.environ["SDK"]+"/doc/sand/results/", help=argparse.SUPPRESS)

    args = parser.parse_args()

    cmd_examples_main(args.device_id, args.input, args.output)
