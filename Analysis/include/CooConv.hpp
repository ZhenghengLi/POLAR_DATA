#ifndef COOCONV
#define COOCONV

/*
 * for i of module:
 *     00 05 10 15 20
 *     01 06 11 16 21
 *     02 07 12 17 22
 *     03 08 13 18 23
 *     04 09 14 19 24
 *
 * for j of channel:
 *     56 57 58 59 60 61 62 63
 *     48 49 50 51 52 53 54 55
 *     40 41 42 43 44 45 46 47
 *     32 33 34 35 36 37 38 39
 *     24 25 26 27 28 29 30 31
 *     16 17 18 19 20 21 22 23
 *     08 09 10 11 12 13 14 15
 *     00 01 02 03 04 05 06 07  
 *
 */

inline int ijtox(int i, int j) {
    return (i / 5) * 8 + j % 8;
}

inline int ijtoy(int i, int j) {
    return (4 - i % 5) * 8 + j / 8;
}

inline int xytoi(int x, int y) {
    return x / 8 * 5 + (4 - y / 8);
}

inline int xytoj(int x, int y) {
    return y % 8 * 8 + x % 8;
}

inline int jtoc(int j) {
    return 8 * (7 - j / 8) + j % 8 + 1;
}

inline int itoc(int i) {
    return 5 * (i % 5) + i / 5 + 1;
}

#endif
