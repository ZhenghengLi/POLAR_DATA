#!/usr/bin/env python


# for i of module:
#     00 05 10 15 20
#     01 06 11 16 21
#     02 07 12 17 22
#     03 08 13 18 23
#     04 09 14 19 24
#
# for j of channel:
#     63 62 61 60 59 58 57 56
#     55 54 53 52 51 50 49 48
#     47 46 45 44 43 41 40 39
#     39 40 41 42 43 44 45 46
#     31 32 33 34 35 36 37 38
#     23 22 21 20 19 18 17 16
#     15 14 13 12 11 10 09 08
#     07 06 05 04 03 02 01 00
#


def ijtox(i, j):
    return (i / 5) * 8 + (7 - j % 8)

def ijtoy(i, j):
    return (4 - i % 5) * 8 + j / 8

def xytoi(x, y):
    return x / 8 * 5 + (4 - y / 8)

def xytoj(x, y):
    return y % 8 * 8 + (7 - x % 8)

def jtoc(j):
    return 8 * (7 - j / 8) +  (7 - j % 8) + 1

def itoc(i):
    return 5 * (i % 5) + i / 5 + 1

