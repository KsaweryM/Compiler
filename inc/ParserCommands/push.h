#ifndef PARSER_PUSH_H
#define PARSER_PUSH_H

Command* push(int n) {
    return new PUSH(n);
}

#endif