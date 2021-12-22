#ifndef PARSER_ADD_H
#define PARSER_ADD_H

Command* add(Command* exp1, Command* exp2) {
    ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK* ADD_NUMBERS = new ADD_TWO_NUMBERS_FROM_STACK_AND_PUSH_RESULT_ON_STACK();
    
    ADD_NUMBERS->pushCommandFront(exp1);
    ADD_NUMBERS->pushCommandFront(exp2);
    return ADD_NUMBERS;
}

#endif