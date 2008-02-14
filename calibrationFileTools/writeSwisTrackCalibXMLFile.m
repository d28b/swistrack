function writeSwisTrackCalibXMLFile(worldPoints,imagePoints,filename);
% function writeSwisTrackCalibXMLFile(worldPoints,imagePoints,filename);
% Function to write the xml file used by swisTrack for the calibration.
%
% Input:
% worldPoints:      Matrix Nx2 of the world coordinates of the calibration
%                   points. First colomn is the X coordinates and second
%                   colomn is the Y coordinates
% imagePoints:      Matrix Nx2 of the image coordinates of the calibration
%                   points. The format is the same as for the worldPoints
% filename:         Name of the output file name. By defaults, it is
%                   calibrationPoints.xml
%
% Pierre Roduit, 14-02-2009, pierre.roduit@epfl.ch

%Parameters tests
if(not(exist('fileName','var')))
    fileName='calibrationPoints.xml';
end
if ((size(worldPoints,2)~=2)||(size(imagePoints,2)~=2))
    warning('worldPoints and imagePoints must be a Nx2 matrix');
    return;
end
if (size(worldPoints,1)~=size(imagePoints,1))
    warning('worldPoints and imagePoints must have the same number of points');
    return;
end

writeFile=fopen(fileName,'w');
fprintf(writeFile,'<?xml version="1.0"?>\r\n<pointlist>\r\n\t<points>\r\n');
for i=1:size(worldPoints,1)
    fprintf(writeFile,'\t\t<point>\r\n');
    fprintf(writeFile,['\t\t\t<xworld>' num2str(worldPoints(i,1)) '</xworld>\r\n']);
    fprintf(writeFile,['\t\t\t<yworld>' num2str(worldPoints(i,2)) '</yworld>\r\n']);
    fprintf(writeFile,['\t\t\t<ximage>' num2str(imagePoints(i,1)) '</ximage>\r\n']);
    fprintf(writeFile,['\t\t\t<yimage>' num2str(imagePoints(i,2)) '</yimage>\r\n']);
    fprintf(writeFile,'\t\t</point>\r\n');
end
fprintf(writeFile,'\t</points>\r\n</pointlist>');
fclose(writeFile);