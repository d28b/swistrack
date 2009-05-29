function [averageError, maxError, stdError]=computeError(real, newReal)
%function [averageError, maxError, stdError]=computeError(real, newReal)
%
% Input:
% real      Real coordinates of the calibration points, N by 2 matrix
% newReal   Coordinates of the calibration points computed from the image
%           coordinates, N by 2 matrix
%
% Output:
% The errors are computed using the calibration points
% Each error is displayed in the form: [x,y,xy]
% Along x axis, along y axis, 2D norm
%
% averageError  Average error resulting of the calibration 
% maxError      Max error resulting of the calibration
% stdError      Standard deviation of the calibration error
%
% Pierre Roduit, 2009-05-29, pierre.roduit@epfl.ch 

if(size(real,1)~=size(newReal,1))
    error('Real and newReal must possess the same number of points.')
end

if ((size(real,2)~=2)||(size(newReal,2)~=2))
    error('real and newReal must be N by 2 matrices');
end

%Display output format
disp('Output format: [x,y,xy]');

errorValue=abs(real-newReal);
errorValue=[errorValue sqrt(dot(errorValue,errorValue,2))];
averageError=mean(errorValue);
maxError=max(errorValue);
stdError=std(errorValue);