##
## This file is part of the APErouter tools stack.
## Copyright (C) 2017 INFN APE Lab.
##
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

from re import match
import json


def compute_shift(bitmask):
    return (bitmask & -bitmask).bit_length() - 1


def print_field(field, val):
    bitmask = int(field["bitmask"], 16)
    shift = compute_shift(bitmask)
    field_val = (val & bitmask) >> shift

    if "values" in field and len(field["values"]) > field_val:
        print "\t\t{}: {} ({})".format(field["name"], field["values"][field_val], field_val)
    else:
        print "\t\t{}: {}".format(field["name"], field_val)


def print_address(root_node):
    keys = sorted(map(int, root_node.keys()))
    for k in keys:
        node = root_node[str(k)]
        print "{}: {}".format(hex(4*k), node["name"])


def print_header(root_node):
    keys = sorted(map(int, root_node.keys()))

    print "#ifndef KARMA_REG_H\n#define KARMA_REG_H\n\n"

    for k in keys:
        node = root_node[str(k)]
        print "#define KARMA_REG_{} {}".format(node["name"], k)

    print "\n#define KARMA_LAST_REG KARMA_REG_{}".format(node["name"])
    print "#define KARMA_NUM_REGS (KARMA_LAST_REG + 1)"
    print "\n#endif"


def print_register(num, node, val, short=False):
    print "{}:\t{}: {}".format(num, node["name"], val)

    if short: return

    if "fields" in node:
        for field in node["fields"]:
            print_field(field, int(val, 16))
        print ""


def parse_register(reg_string):
    r = match("reg\[(\d+)\]=(.+)", reg_string)
    if r:
        num = r.group(1)
        val = r.group(2)
    else:
        print "Error parsing line: " + reg_string
        exit()
    return num, val


def parse_yaml_entry(reg, group_name="", offset=0, substring=""):
    name = group_name + reg["name"].format(substring)
    number = reg["number"] + offset

    d = {"name": name}

    if "fields" in reg:
        d["fields"] = reg["fields"]

    return {number: d}


def parse_yaml(file):
    import yaml

    with open(file, "r") as f:
        data = yaml.load(f)
        data = data["registers"] # drop everything else for now

    parsed = {}

    for elem in data:
        if "group_name" in elem:
            group_size = len(elem["registers"])
            base = elem["start_number"]
            for group_element in elem["group_elements"]:
                for reg in elem["registers"]:
                    parsed.update(parse_yaml_entry(reg, group_name=elem["group_name"], offset=base, substring=group_element))
                base += group_size
        else:
            parsed.update(parse_yaml_entry(elem))

    print(json.dumps(parsed, sort_keys=True, indent=4))
