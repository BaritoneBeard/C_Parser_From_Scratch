# C Parser

<h2> Compile on Commandline </h2>

To run, compile parser.c and lexical_for_parser.c

``` ./compile.sh ```

if permissions needed, first run:

``` chmod +x compile.sh ```

 then compile.

For completeness sake: the command to compile manually is simple.

``` gcc -o parser parser.c lexical_for_parser.c ```

<h2> Running a program </h2>

once compiled, the parser will take a text file as an input, for example:

``` ./parser demoFunc.txt ```

Currently the only supported aspects of C code are those included as demo files, such as Return or Function calls. Things such as break statements - while parsed - do not execute any code.

Lastly, there are a lot of printed messages. Looking for anything meaningfull in there is nigh-hopeless unless you know what to look for in particular.
