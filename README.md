# HelloMex
[![View HelloMex on File Exchange](https://www.mathworks.com/matlabcentral/images/matlab-file-exchange.svg)](https://www.mathworks.com/matlabcentral/fileexchange/87292-hellomex)


The repositiory contains a simple example for MATLAB mex-file generation of C++ classes.
The files are documented in detail directly in the code.

Because of the architecture of mex files, two wrappers are needed in order to achieve expected
object behaviour and syntax in MATLAB.

One wrapper is on the C++ side, it overrides the operator() of a class called 'MexFunction'.
This method is executed whenever the mex-file is called out of MATLAB. Within the MexFunction
class a list of Hello (the underlaying C++ class) objects is managed. Each list item
represents the state of a class instance in MATLAB. The MexFunction class acts as a simple
command interpreter which calls the right Hello Method of the respective MATLAB object and
casts the datatypes between MATLAB and C++ before and after the Hello method execution.

The second wrapper is a MATLAB class called 'Hello' to express the analogy to the C++ Hello
class. The MATLAB verson of the Hello class takes care, that the function calls to the mex-file
hold the correct syntax and passes an object specific handle (identifier), such that the C++
MexFunction class always knows which object state it needs to refer to.

# Getting Started
* Run the make_hello_mex.m script to compile the mex file.
* Use `doc Hello` after you compiled the mex-file to get a documentation of the MATLAB class.
* Study the detailed comments inside the files.
* Enjoy and like on Github and File Exchange :-)

# Repository Files:
This repository contains the following files.

## hello.h / hello.c
Underlaying class which finally will be accessed from MATLAB. This is the actual
C++ class of which we want to use the functionality in MATLAB.

The class is initialized with a number called preset.
The value of preset can be requested using the method GetPreset() during the lifetime of the 
object. Using the Method Compute(factor) the preset number is multiplied with the function
argument called factor.

## HelloMex.cpp
Holds the implementation of the C++ Wrapper class 'MexFunction'. On the upper part of the
file is also a compile time configuration concerning the log level of the mex file.
This file translates to the mex-file after compilation.

## Hello.m
The MATLAB wrapper for the mex file. This class manages calls to the mex-file and provides
a userfriendly class interface to interact with the underlined cpp class methods.

## make_hello_mex.m
Script to compile the mex file and rudimentary testing of the code.
