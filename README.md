# Algorithmic Junks

*My experiments on all kinds of algorithms.*

## The `bin` directory

*The so-called "just-in-time" C compiler*

It contains convenient scripts for running or debugging programs in compiled
languages as if they were interpreted scripts.

Programs named `run*` will compile the program to a temporary executable
file, run it and delete it. `debug*` will run it with gdb for debugging.

Example:

    runc99 my_program.c arg1 arg2 arg3 ...

## Author

Kunshan Wang <wks1986@gmail.com>

