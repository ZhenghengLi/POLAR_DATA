#ifndef COOCONV_H
#define COOCONV_H

/*
 * for i of module:                  |      for i of module from back:
 *     00 05 10 15 20                |          20 15 10 05 00
 *     01 06 11 16 21                |          21 16 11 06 01
 *     02 07 12 17 22                |          22 17 12 07 02
 *     03 08 13 18 23                |          23 18 13 08 03
 *     04 09 14 19 24                |          24 19 14 09 04
 *
 * for j of channel:                 |      for j of channel from back:
 *     63 62 61 60 59 58 57 56       |          56 57 58 59 60 61 62 63
 *     55 54 53 52 51 50 49 48       |          48 49 50 51 52 53 54 55
 *     47 46 45 44 43 42 41 40       |          40 41 42 43 44 45 46 47
 *     39 38 37 36 35 34 33 32       |          32 33 34 35 36 37 38 39
 *     31 30 29 28 27 26 25 24       |          24 25 26 27 28 29 30 31
 *     23 22 21 20 19 18 17 16       |          16 17 18 19 20 21 22 23
 *     15 14 13 12 11 10 09 08       |          08 09 10 11 12 13 14 15
 *     07 06 05 04 03 02 01 00       |          00 01 02 03 04 05 06 07
 *
 */

inline int ijtox(int i, int j) {
    return (i / 5) * 8 + (7 - j % 8);
}

inline int ijtoxb(int i, int j) {
    return (4 - i / 5) * 8 + j % 8;
}

inline int ijtoy(int i, int j) {
    return (4 - i % 5) * 8 + j / 8;
}

inline int ijtoyb(int i, int j) {
    return (4 - i % 5) * 8 + j / 8;
}

inline int xytoi(int x, int y) {
    return x / 8 * 5 + (4 - y / 8);
}

inline int xbybtoi(int xb, int yb) {
    return (4 - xb / 8) * 5 + (4 - yb / 8);
}

inline int xytoj(int x, int y) {
    return y % 8 * 8 + (7 - x % 8);
}

inline int xbybtoj(int xb, int yb) {
    return yb % 8 * 8 + xb % 8;
}

inline int jtoc(int j) {
    return 8 * (7 - j / 8) + (7 - j % 8) + 1;
}

inline int jtocb(int j) {
    return 8 * (7 - j / 8) + j % 8 + 1;
}

inline int itoc(int i) {
    return 5 * (i % 5) + i / 5 + 1;
}

inline int itocb(int i) {
    return 5 * (i % 5) + (4 - i / 5) + 1;
}

inline int itox(int i) {
    return i / 5;
}

inline int itoxb(int i) {
    return 4 - i / 5;
}

inline int itoy(int i) {
    return 4 - i % 5;
}

inline int itoyb(int i) {
    return 4 - i % 5;
}

inline int jtox(int j) {
    return 7 - j % 8;
}

inline int jtoxb(int j) {
    return j % 8;
}

inline int jtoy(int j) {
    return j / 8;
}

inline int jtoyb(int j) {
    return j / 8;
}

#endif
