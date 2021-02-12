// File:   HelloMex.cpp
// Author: Pascal Enderli
// Date:   Zuerich, 06.02.2021

#include "mex.hpp"
#include "mexAdapter.hpp"
#include "hello.h"

#include <exception>
#include <memory>
#include <vector>
#include <string>

//*************************************************************************************************
// Compile Time Configuration
//*************************************************************************************************

//-------------------------------------------------------------------------------------------------
// MACRO LOG_LEVEL
//-------------------------------------------------------------------------------------------------
// Defines which messages are logged to the MATLAB Console.
//
// Possible values:
// 0 -> logs LEVEL_FAIL, LEVEL_WARN, LEVEL_INFO
// 1 -> logs LEVEL_FAIL, LEVEL_WARN
// 2 -> logs LEVEL_FAIL
#define LOG_LEVEL 2

//*************************************************************************************************
// Helper Classes
//*************************************************************************************************

//=================================================================================================
// MatlabLogger class
//=================================================================================================
// Logger Usage:
// Use the macro LOG(level, msg) for logging.
//
// Argument level: LEVEL_INFO
//                 LEVEL_WARN
//                 LEVEL_FAIL
//
// Argument msg:   The log message as a std::string
//
// Note: * Exceptions are always logged to MATLLAB (LEVEL_FAIL).
//         Even if LOG_TO_MATLAB is equal to 0.

#define LEVEL_FAIL 2
#define LEVEL_WARN 1
#define LEVEL_INFO 0

class MatlabLogger : public matlab::mex::Function
{
public:

    //---------------------------------------------------------------------------------------------
    // Constructor
    //---------------------------------------------------------------------------------------------
    MatlabLogger() : matlab_ptr_(getEngine()) {}

    //---------------------------------------------------------------------------------------------
    // Log
    //---------------------------------------------------------------------------------------------
    void Log(int level, std::string msg, int line)
    {
        switch( level )
        {
        case LEVEL_INFO: Info(msg, line); break;
        case LEVEL_WARN: Warn(msg, line); break;
        case LEVEL_FAIL: Fail(msg, line); break;
        default: Warn("Unknown log level: " + std::to_string(level), __LINE__);
        }
    }

private:
    //---------------------------------------------------------------------------------------------
    // Fail
    //---------------------------------------------------------------------------------------------
    void Fail(std::string msg, int line)
    {
        matlab_ptr_->feval(u"disp", 0, std::vector<matlab::data::Array>({ factory_.createScalar("[Fail] [Line: " + std::to_string(line) + "] " + msg) }));
    }

    //---------------------------------------------------------------------------------------------
    // Warn
    //---------------------------------------------------------------------------------------------
    void Warn(std::string msg, int line)
    {
        matlab_ptr_->feval(u"disp", 0, std::vector<matlab::data::Array>({ factory_.createScalar("[Warn] [Line: " + std::to_string(line) + "] " + msg) }));
    }

    //---------------------------------------------------------------------------------------------
    // Info
    //---------------------------------------------------------------------------------------------
    void Info(std::string msg, int line)
    {
        matlab_ptr_->feval(u"disp", 0, std::vector<matlab::data::Array>({ factory_.createScalar("[Info] [Line: " + std::to_string(line) + "] " + msg) }));
    }

    std::shared_ptr<matlab::engine::MATLABEngine> matlab_ptr_;
    matlab::data::ArrayFactory factory_;
};

// Define Logger Macro
#define LOG(level, msg) do{ if(level >= LOG_LEVEL) { Log(level, std::string(msg), __LINE__); } }while(0)

//=================================================================================================
// Handle class
//=================================================================================================

// Maps a handle number to an array index and vice versa.
// Use this class in tandem with a variable length array. Handle will keep track at which array
// index which element is. This allows to provide a constant handle identifier to a public interface
// and change their location (array index) in the array as needed.
class Handles : public MatlabLogger
{
public:

    //---------------------------------------------------------------------------------------------
    // Add
    //---------------------------------------------------------------------------------------------
    // Appends a new element at the end of the array and assigns the smallest unused handle
    // identifier.
    size_t Add()
    {
        size_t new_handle = 0;
        for( size_t handle : handles_ )
        {
            if( handle == new_handle )
            {
                ++new_handle;
            }
            else
            {
                break;
            }
        }

        handles_.push_back(new_handle);
        LOG(LEVEL_INFO, "Add new handle: " + std::to_string(new_handle));
        return new_handle;
    }

    //---------------------------------------------------------------------------------------------
    // GetObjIdx
    //---------------------------------------------------------------------------------------------
    // Given a handle identifier, this method returns the corresponding array index.
    size_t GetObjIdx(size_t handle)
    {
        size_t n_handles = handles_.size();
        for( size_t i = 0; i < n_handles; ++i )
        {
            if( handle == handles_[i] )
            {
                LOG(LEVEL_INFO, "Handle : " + std::to_string(handle) + " maps to index: " + std::to_string(i));
                return i;
            }
        }
        throw std::out_of_range("Unknown handle: " + std::to_string(handle));
    }

    //---------------------------------------------------------------------------------------------
    // Remove
    //---------------------------------------------------------------------------------------------
    // Removes the requested element from the array and returns the array index where it was before
    // its destruction.
    size_t Remove(size_t handle)
    {
        size_t idx = GetObjIdx(handle);
        handles_.erase(handles_.begin() + idx);
        LOG(LEVEL_INFO, "Erased handle : " + std::to_string(handle));
        return idx;
    }

    //---------------------------------------------------------------------------------------------
    // Size
    //---------------------------------------------------------------------------------------------
    // returns how many handles currently are in the array.
    size_t Size()
    {
        size_t n_handles = handles_.size();
        LOG(LEVEL_INFO, "Number of handles : " + std::to_string(n_handles));
        return n_handles;
    }

private:
    std::vector<size_t> handles_;
};

//*************************************************************************************************
// Main Mex Class
//*************************************************************************************************

//=================================================================================================
// MexFunction class
//=================================================================================================
// This class is interfaced to the public (MATLAB) through the mex file.
// The object (MexFunction) exists from the first function call until is cleared from MATLAB.
// The class is the link between MATLAB and the underlaying c++ class which needs to be accessed
// in MATLAB. The MexFunction class holds an array of Hello class instances which is needed to
// manage the multiple instantiated objects in MATLAB. The overloaded operator() acts as a simple
// command handler and calls the underlaying method from the requested object instance.
// Additionally some type casting is done in order to map MATLAB data types to C++ data types and
// vice versa.
class MexFunction : public MatlabLogger
{
public:
    MexFunction() : matlab_ptr_(matlab::mex::Function::getEngine()) {}

    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs)
    {
        try
        {
            if( inputs.size() < 1 )
            {
                throw std::invalid_argument("Command Keyword missing.");
            }

            if( !IsString(inputs, 0) )
            {
                throw std::invalid_argument("Expected a string as input.");
            }
            std::string cmd = inputs[0][0];
            LOG(LEVEL_INFO, "Received command: " + cmd);

            size_t num_outputs = outputs.size();
            size_t num_inputs = inputs.size();
            size_t num_args = num_inputs - 1;

            LOG(LEVEL_INFO, "Number of outputs : " + std::to_string(num_outputs));

            //-------------------------------------------------------------------------------------
            // Constructor Command (Keyword: "New")
            //-------------------------------------------------------------------------------------
            if( cmd == "New" )
            {
                LOG(LEVEL_INFO, "Number of args : " + std::to_string(num_args));
                if( num_args != 1 )
                {
                    throw std::length_error("Expected one argument.");
                }

                if( num_outputs != 1 )
                {
                    throw std::length_error("Output is a scalar. Requested " + std::to_string(num_outputs) + " outputs.");
                }

                if( !IsDouble(inputs, 1) )
                {
                    throw std::invalid_argument("Expected a double as input.");
                }
                double preset = inputs[1][0];

                hello_objects_.push_back(std::unique_ptr<Hello>(new Hello(std::forward<double>(preset))));

                size_t handle = object_handles_.Add();
                LOG(LEVEL_INFO, "Return handle: " + std::to_string(handle));
                outputs[0] = factory_.createScalar<size_t>(handle);
            }

            //---------------------------------------------------------------------------------
            // Static Method GetNumberOfHandles Command (Keyword: "GetNumberOfHandles")
            //---------------------------------------------------------------------------------
            else if( cmd == "GetNumberOfHandles" )
            {
                LOG(LEVEL_INFO, "Number of args : " + std::to_string(num_args));
                if( num_args != 0 )
                {
                    throw std::length_error("Expected no arguments.");
                }

                if( num_outputs != 1 )
                {
                    throw std::length_error("Output is a scalar. Requested " + std::to_string(num_outputs) + " outputs.");
                }

                outputs[0] = factory_.createScalar<size_t>(object_handles_.Size());
            }

            //-------------------------------------------------------------------------------------
            // Object specific methods
            //-------------------------------------------------------------------------------------
            else
            {
                if( !IsInt(inputs, 1) && !IsUInt(inputs, 1) )
                {
                    throw std::invalid_argument("Expected an int as input.");
                }
                size_t handle = inputs[1][0];
                LOG(LEVEL_INFO, "Receive handle: " + std::to_string(handle));
                num_args = num_inputs - 2;
                size_t object_idx = object_handles_.GetObjIdx(handle);
                LOG(LEVEL_INFO, "Number of args : " + std::to_string(num_args));

                //---------------------------------------------------------------------------------
                // Compute Command (Keyword: "Compute")
                //---------------------------------------------------------------------------------
                if( cmd == "Compute" )
                {
                    if( num_args != 1 )
                    {
                        throw std::length_error("Expect 2 arguments.");
                    }

                    if( num_outputs != 1 )
                    {
                        throw std::length_error("Output is a scalar. Requested " + std::to_string(num_outputs) + " outputs.");
                    }

                    if( !IsDouble(inputs, 2) )
                    {
                        throw std::invalid_argument("Expected a double as input.");
                    }
                    double factor = inputs[2][0];
                    double result = hello_objects_[object_idx]->Compute(factor);
                    LOG(LEVEL_INFO, "Return Result: " + std::to_string(result));
                    outputs[0] = factory_.createScalar<double>(result);
                }

                //---------------------------------------------------------------------------------
                // GetPreset Command (Keyword: "GetPreset")
                //---------------------------------------------------------------------------------
                else if( cmd == "GetPreset" )
                {
                    if( num_args != 0 )
                    {
                        throw std::length_error("Expect 1 arguments.");
                    }

                    if( num_outputs != 1 )
                    {
                        throw std::length_error("Output is a scalar. Requested " + std::to_string(num_outputs) + " outputs.");
                    }

                    double result = hello_objects_[object_idx]->GetPreset();
                    LOG(LEVEL_INFO, "Return Result: " + std::to_string(result));
                    outputs[0] = factory_.createScalar<double>(result);
                }

                //---------------------------------------------------------------------------------
                // Delete Command (Keyword: "Delete")
                //---------------------------------------------------------------------------------
                else if( cmd == "Delete" )
                {
                    if( num_args != 0 )
                    {
                        throw std::length_error("Expect 1 argument.");
                    }

                    if( num_outputs != 0 )
                    {
                        throw std::length_error("Output is a scalar. Requested " + std::to_string(num_outputs) + " outputs.");
                    }

                    // delete object and handle
                    size_t object_idx = object_handles_.Remove(handle);
                    hello_objects_.erase(hello_objects_.begin() + object_idx);
                }

                else
                {
                    throw std::invalid_argument("Unknown Command: " + cmd);
                }
            }
        }
        catch( std::exception& e )
        {
            LOG(LEVEL_FAIL, e.what());
        }
    }

private:

    bool IsDouble(matlab::mex::ArgumentList& inputs, int idx)
    {
        return (inputs[idx].getType() == matlab::data::ArrayType::DOUBLE && inputs[idx].getType() != matlab::data::ArrayType::COMPLEX_DOUBLE);
    }

    bool IsInt(matlab::mex::ArgumentList& inputs, int idx)
    {
        matlab::data::ArrayType type = inputs[idx].getType();
        return ((type == matlab::data::ArrayType::INT8  ||
                 type == matlab::data::ArrayType::INT16 ||
                 type == matlab::data::ArrayType::INT32 ||
                 type == matlab::data::ArrayType::INT64 ));
    }

    bool IsUInt(matlab::mex::ArgumentList& inputs, int idx)
    {
        matlab::data::ArrayType type = inputs[idx].getType();
        return ((type == matlab::data::ArrayType::UINT8  ||
                 type == matlab::data::ArrayType::UINT16 ||
                 type == matlab::data::ArrayType::UINT32 ||
                 type == matlab::data::ArrayType::UINT64 ));
    }

    bool IsString(matlab::mex::ArgumentList& inputs, int idx)
    {
        return (inputs[idx].getType() == matlab::data::ArrayType::MATLAB_STRING && inputs[idx].getType() != matlab::data::ArrayType::CHAR);
    }

    std::shared_ptr<matlab::engine::MATLABEngine> matlab_ptr_;
    matlab::data::ArrayFactory factory_;

    // Object handles and objects arrays have analogue indexing. A unique handle belongs to
    // each object.
    Handles object_handles_;

    // An array of objects of the underlaying wrapped class. Each array item represents one class
    // instance in the MATLAB workspace.
    std::vector<std::unique_ptr<Hello>> hello_objects_;
};
