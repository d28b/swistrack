function WriteSwisTrackCalibrationXMLFile(imagePoints, worldPoints, fileName);
% function WriteSwisTrackCalibrationXMLFile(imagePoints, worldPoints, fileName);
%
% Writes an XML file for SwisTrack's calibration components.
% Such files are independent from the calibration method used and can be used with several calibration components.
%
% Input:
% worldPoints:      N-by-2 matrix with the world coordinates of the calibration
%                   points. The first column contains the X coordinates and second
%                   column the Y coordinates.
% imagePoints:      N-by-2 matrix with the image coordinates of the calibration
%                   points. The first column contains the X coordinates and second
%                   column the Y coordinates. The unit is pixels.
% fileName:         Name of the output file name. If no file name is provided,
%                   the points are written to "calibration_points.xml".
%
% Pierre Roduit, 2008-02-14, pierre.roduit@epfl.ch

% Check input parameters
if not(exist('fileName','var'))
    fileName='calibration_points.xml';
end
if (size(worldPoints, 2) ~= 2) || (size(imagePoints, 2) ~= 2)
    warning('worldPoints and imagePoints must be N-by-2 matrices.');
    return;
end
if size(worldPoints, 1) ~= size(imagePoints, 1)
    warning('worldPoints and imagePoints must have the same number of rows.');
    return;
end

% Write file
writeFile = fopen(fileName, 'w');
fprintf(writeFile, '<?xml version="1.0"?>\r\n<pointlist>\r\n\t<points>\r\n');
for i = 1:size(worldPoints, 1)
    fprintf(writeFile, '\t\t<point>\r\n');
    fprintf(writeFile, ['\t\t\t<xworld>' num2str(worldPoints(i, 1)) '</xworld>\r\n']);
    fprintf(writeFile, ['\t\t\t<yworld>' num2str(worldPoints(i, 2)) '</yworld>\r\n']);
    fprintf(writeFile, ['\t\t\t<ximage>' num2str(imagePoints(i, 1)) '</ximage>\r\n']);
    fprintf(writeFile, ['\t\t\t<yimage>' num2str(imagePoints(i, 2)) '</yimage>\r\n']);
    fprintf(writeFile, '\t\t</point>\r\n');
end
fprintf(writeFile, '\t</points>\r\n</pointlist>');
fclose(writeFile);
