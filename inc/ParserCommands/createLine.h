#ifndef PARSER_CREATE_LINE_H
#define PARSER_CREATE_LINE_H

void createLine(Command* command) {
    ComplexCommand* complex = new ComplexCommand();

    complex->pushCommandBack(command);
    complex->pushCommandBack(new LOAD_FROM_STACK_TO_REGISTER(VMregister::a));
    complex->pushCommandBack(new PUT());

    complex->execute();

    delete complex;
}

#endif