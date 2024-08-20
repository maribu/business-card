#!/usr/bin/env python3
# SPDX-License-Identifier: MIT

from bdfparser import Font
from sys import argv, exit
from os.path import splitext, basename


if __name__ == '__main__':
    if len(argv) != 2:
        exit(f"Usage: {argv[0]} <BPF_FILE>")

    font = Font(argv[1])

    # trimming off empty space from the space glyph won't work, so we add that
    # by hand
    fontdata = [0x00, 0x00, 0x00]
    offsets = [0]
    offset = len(fontdata)
    for codepoint in range(0x21, 0x7e):
        bitmap = font.glyph(chr(codepoint)).draw().todata()
        data = []
        for idx in range(len(bitmap[0])):
            val = 0
            for line in range(8):
                if bitmap[line][idx] == '1':
                    val |= (1 << line)
            data.append(val)

        # trim empty space left and right
        while data[0] == 0:
            data = data[1:]
        while data[-1] == 0:
            data = data[:-1]

        # append bitmap data
        fontdata += data
        offsets.append(offset)
        offset += len(data)

    print(f"/* This file is auto generated using {basename(argv[0])} */")
    print("#include <stdint.h>")
    print("#include \"bitmap_fonts.h\"")
    print("")

    print("static const uint8_t bitmap_data[] = {")
    print(f"    {fontdata[0]:#04x},", end='')
    for i in range(1, len(fontdata)):
        print(f" {fontdata[i]:#04x},", end='')
        if ((i + 1) % 8) == 0:
            print("\n   ", end='')
    print("\n};\n")

    print(f"const bitmap_font_t {splitext(argv[1])[0]} = " + "{")
    print(f"    .data_size = {len(fontdata):#06x},")
    print("    .data = bitmap_data,")
    print("    .offsets = {")
    print(f"        {offsets[0]:#06x},", end='')
    for i in range(1, len(offsets)):
        print(f" {offsets[i]:#06x},", end='')
        if ((i + 1) % 8) == 0:
            print("\n       ", end='')
    print("\n    },\n};")
