% File: make_hello_mex.m
% Author: Pascal Enderli
% Date: Zuerich, 06.02.2021

%--------------------------------------------------------------------------
%% Compiling the mex-File
clear all
clc

% select the newest c++ compiler in the matlab console.
mex -setup C++

% Compile mex
% Tested with MATLAB R2019b
mex('HelloMex.cpp', 'hello.cpp');

%--------------------------------------------------------------------------
%% Testing the mex-File

disp("Starting tests...");

obj1 = Hello(2);
assert(2 == obj1.GetPreset(), "[Fail] obj1.GetPreset");
assert((2*5) == obj1.Compute(5), "[Fail] obj1.Compute");

obj2 = Hello(5);
assert(5 == obj2.GetPreset(), "[Fail] obj2.GetPreset");

assert(2 == Hello.NumberOfInstances(),  "[Fail] Expected two instaces. ");
clear('obj1');
assert(1 == Hello.NumberOfInstances(),  "[Fail] Expected one instaces. ");
obj3 = Hello(10);
assert(2 == Hello.NumberOfInstances(),  "[Fail] Expected two instaces. ");
clear('all');
assert(0 == Hello.NumberOfInstances(),  "[Fail] Expected zero instaces. ");

disp("Test was successful");
