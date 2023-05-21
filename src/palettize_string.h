#ifndef PALETTIZE_STRING_H
#define PALETTIZE_STRING_H

inline char flip_case(char c) {
    char result = '\0';

    if ('a' <= c && c <= 'z') {
        result = 'A' + (c - 'a');
    } else if ('A' <= c && c <= 'Z') {
        result = 'a' + (c - 'A');
    }

    return result;
}

inline bool istrings_match(char *a, char *b) {
    while (*a && *b) {
        if (*a == *b || *a == flip_case(*b)) {
            a++;
            b++;
        } else {
            break;
        }
    }

    bool result = *a == *b;

    return result;
}

#endif
