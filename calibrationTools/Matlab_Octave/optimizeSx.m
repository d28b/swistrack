function optSx=optimizeSx(image, world, C, dx, dy, sxArray,criterion)
% function optSx=optimizeSx(image, world, C, dx, dy, sxArray,criterion)
%
% Compute the best sx values for the Tsai calibration using
% computeTsaiCalibrationParameters function.
%
% Inputs:
% image     N-by-2 matrix with the image coordinates of the calibration
%           points. The first column contains the X coordinates and second
%           column the Y coordinates. The unit is pixels.
% world     N-by-2 matrix with the world coordinates of the calibration
%           points. The first column contains the X coordinates and second
%           column the Y coordinates.
% C         Lens optical center, in the camera array referential (in pixels)
%           Most of the time, it corresponds to the image center (2D)
% dx        Size, along x, of a camera pixel (look at the camera datasheet)
% dy        Size, along y, of a camera pixel (look at the camera datasheet)
% sxArray   Uncertainty on dx/dy (In fact the real dx used is dx/sx)
%           If you don't know, set this value to 1 (or close to one)
%           As you we want to do the computation for multiple values of sx,
%           you can give a vector of sx values (e.g. 0.8:0.01:1.2)
% criterion Optimization criterion in the form of the 2D vector. First
%           value belongs to [1 2 3]: 1 averageError, 2 maxError, 3
%           stdError. Second value belongs to [1 2 3]: 1 along x, 2 along
%           y, 3 2D norm. e.g. if you enter [1 3] it corresponds to
%           optimizing the averageError in 2D
%
% Outputs:
% optSx     Optimal value of sx miniminizing the error based on the
%           selected criterion.
%
% Pierre Roduit, 2009-05-29, pierre.roduit@epfl.ch 

optSx=0;
tmpError=inf;
for i=1:length(sxArray)
    [R,T,f,k1]= computeTsaiCalibrationParameters(image,world, C , dx, dy, sxArray(i),false);
    newReal=TsaiImage2World(image, R, T, f, k1, C, dx, dy, sxArray(i));
    [averageError, maxError, stdError]=computeError(world, newReal,false);
    matrixError=[averageError;maxError;stdError];
    if (matrixError(criterion(1),criterion(2))<tmpError)
        tmpError=matrixError(criterion(1),criterion(2));
        optSx=sxArray(i);
    end
end
