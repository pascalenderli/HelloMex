// File:   hello.h
// Author: Pascal Enderli
// Date:   Zuerich, 06. Februar 2021

#ifndef HELLO_H
#define HELLO_H

// Underlaying class which finally will be accessed from MATLAB
class Hello
{
public:

    // Constructor
    // define a preset number which is used as constant multiplier by the class.
    Hello(int preset);

    // Computes out = factor * preset (see constructor)
    int Compute(int factor);

    // Returned the preset factor defined at construction.
    int GetPreset();

private:
    int preset_;
};

#endif // HELLO_H
