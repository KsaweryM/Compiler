#ifndef PARSER_CHANGE_SING_H
#define PARSER_CHANGE_SING_H

Command* changeSign(Command* exp) {
    CHANGE_SIGN_ON_STACK* CHANGE_SIGN = new CHANGE_SIGN_ON_STACK();
    
    CHANGE_SIGN->pushCommandFront(exp);

    return CHANGE_SIGN;
}

#endif