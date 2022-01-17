#ifndef PARSER_PUSH_H
#define PARSER_PUSH_H

Command* push(long long n) {
    return new PUSH(n);
}

#endif