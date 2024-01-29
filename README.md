# Regular Expressions (Thompson's Construction)

## Description and Implementation

This repository contains an implementation of Thompson's construction algorithm
for regular expressions. The program takes a regular expression as input and
generates a non-deterministic finite automaton (NFA) that accepts the same
language as the regular expression. It parses the regular expression by
interpreting it as a word from an LL-grammer, which can, therefore, be parsed
using a recursive descent implementation.

The NFA can be exported to a dot-notation file, which can be used to generate
a visual representation of the automaton using Graphviz.

The program can also be used to check whether a string is accepted by the
automaton/regular expression.

**Available operations:**

- `exp <expression>`     Read in regular expression
- `dot <filename>`       Export regular expression to dot-notation
- `mat <string>`         Check whether a string is accepted by automaton
- `end`                  Close the program

## How to compile

To compile, first generate a Makefile using `cmake .` (from the base directory).
Then, run `make` to compile the program.

## How to run

This program can operate in two ways: autonomous and interactive.

1. Autonomous: supply the argument `d` and provide a line-seperated list of
   instructions to the standard input, e.g.
   `./RegExp d < file.txt`.
2. Interactive: simply run the program without arguments, e.g. `./RegExp`.

_Interactive mode with only the absolute minimum amount of feedback output
to the console is also possible by providing the `d` flag and not providing
anything to the standard input._
