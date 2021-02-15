% Hello MATLAB wrapper to an underlying C++ class
%
% This interface is a wrapper for the C++ class 'Hello'.
%
% Examples:
%   obj = Hello(preset)         % <- Constructor
%         obj.delete()          % <- Destructor
%         obj.Compute(factor)   % <- Computes out = preset * factor
%         obj.GetPreset()       % <- Returnes preset
%
% Note:
% The mex file HelloMex.mex64 must be in the MATLAB search path.
% * The class instances are managed by the mex file. each constructed
%   instance is referenced by a unique handle to disambiguate.
% * There exists also a c++ side wrapper called 'HelloMex.cpp'.
%   HelloMex.cpp Emulates class behaviour on the MATLAB side and manages
%   the properties of the object instances.
%
% Author:
%   Pascal Enderli, Zuerich, 13. Februar 2021

classdef Hello < handle
    %======================================================================
    %  Properties
    %======================================================================
    properties (Access = private, Hidden = true)
        handle_ = -1;
    end
    
    %======================================================================
    %  Methods
    %======================================================================
    methods
        %------------------------------------------------------------------
        %  Constructor
        %------------------------------------------------------------------
        function this = Hello(preset)
            % Constructor - Create a new C++ class instance
            % Inputs:
            %   preset   A preset factor for multiplication of two scalars.
            %
            % Outputs:
            %   An instance of a Hello object.
            %
            % Examples:
            %   obj = Hello(preset) % <- Constructor
            %
            % Details:
            %   Arguments passed to the mex when calling:
            %     * Command identifier: "New"
            %     * Command Argument:   preset
            %
            
            assert(this.handle_ == -1, "This object is already constructed.");
            this.handle_ = HelloMex("New", preset);
        end
        
        %------------------------------------------------------------------
        %   Destructor
        %------------------------------------------------------------------
        function delete(this)
            % Destructor - Destroys a class instance
            %
            % Inputs:
            %   None
            %
            % Outputs:
            %   None
            %
            % Examples:
            %   obj.delete() % <- Destructor
            %
            % Details:
            %   Arguments passed to the mex when calling:
            %     * Command identifier:         "Delete"
            %     * Object property identifier: handle (unsigned integer)
            %
            HelloMex("Delete", this.handle_);
            this.handle_ = -1;
        end
        
        function Delete(this)
            % Destructor - Destroys a class instance
            %
            % Details:
            % Same as delete()
            %
            this.delete();
        end
        
        %------------------------------------------------------------------
        %   Compute
        %------------------------------------------------------------------
        function out = Compute(this, factor)
            % Compute - Computes a Multiplication
            %
            % Inputs:
            %   None
            %
            % Outputs:
            %   Computation result (double).
            %
            % Examples:
            %   obj.Compute(factor) % <- Computes out = preset * factor
            %
            % Details:
            %   Arguments passed to the mex when calling:
            %     * Command identifier:         "Compute"
            %     * Object property identifier: handle (unsigned integer)
            %     * Command argument:           factor
            %
            
            out = HelloMex("Compute", this.handle_, factor);
        end
        
        %------------------------------------------------------------------
        %   GetPreset
        %------------------------------------------------------------------
        function out = GetPreset(this)
            % GetPreset - Retruns the preset multiplication factor
            %
            % Inputs:
            %   None
            %
            % Output:
            %   presset Multiplication factor (double).
            %
            % Examples:
            %   obj.GetPreset() % <- Returns the preset
            %
            % Details:
            %   Arguments passed to the mex when calling:
            %     * Command identifier:         "GetPreset"
            %     * Object property identifier: handle (unsigned integer)
            %
            
            out = HelloMex("GetPreset", this.handle_);
        end
        
        %------------------------------------------------------------------
        %   GetHandle
        %------------------------------------------------------------------
        function out = GetHandle(this)
            % GetHandle - Retruns the handle of this object
            %
            % Inputs:
            %   None
            %
            % Outputs:
            %   handle of this object instance (uint64).
            %
            % Examples:
            %   obj.GetHandle() % <- Returns the handle
            %
            % Details:
            %   This Method does not call the mex.
            %
            
            out = this.handle_;
        end
    end
    
    %======================================================================
    %  Static Methods
    %======================================================================
    methods(Static)
        %------------------------------------------------------------------
        %   NumberOfInstances
        %------------------------------------------------------------------
        function out = NumberOfInstances()
            % NumberOfInstances - Retruns the handle of this object
            %
            % Inputs:
            %   None
            %
            % Outputs:
            %   number of unique instantiated objects from this class (uint64).
            %
            % Examples:
            %   Hello.NumberOfInstances()
            %
            out = HelloMex("GetNumberOfHandles");
        end
    end
end
