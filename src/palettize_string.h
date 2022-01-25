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

inline bool strings_match(char *a, char *b, bool case_sensitive = true) {
    while (*a && *b) {
        if (*a == *b ||
            (!case_sensitive && *a == flip_case(*b))) {
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
