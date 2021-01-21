function [imageData,worldData]=readSwisTrackCalibrationXMLFile(fileName);
% function [imageData,worldData]=readSwisTrackCalibrationXMLFile(fileName);
%
% Read a XML file for SwisTrack's calibration components.
% Such files are independent from the calibration method used and can be used with several calibration components.
%
% Input:
% fileName:         Name of the input file name. If no file name is provided,
%                   a GUI opens
%
% Output:
% imageData:        N-by-2 matrix with the image coordinates of the calibration
%                   points. The first column contains the X coordinates and second
%                   column the Y coordinates. The unit is pixels.
% worldData:        N-by-2 matrix with the world coordinates of the calibration
%                   points. The first column contains the X coordinates and second
%                   column the Y coordinates.
% Pierre Roduit, 2009-05-28, pierre.roduit@epfl.ch

%test input
if not(exist('fileName','var'))
    [fileName, pathName] = uigetfile('*.xml', 'Pick a input file');
    fileName=[pathName,fileName];
end

valueNames={'xworld','yworld','ximage','yimage'};
xDoc=xmlread(fileName);
pointList=xDoc.getElementsByTagName('point');
for i=0:pointList.getLength-1
    point=pointList.item(i);    
    for j=1:length(valueNames)
        valueList=point.getElementsByTagName(char(valueNames(j)));
        if (valueList.getLength>1)
            %There are too many tag values
            warning(['There are more than one ',char(valueNames(j)),' tag for the point number ',num2str(i+1),'. Only the first will be taken into account.']);
        end
        if (valueList.getLength>1)
            %The tag value is missing
            error(['There is no ',char(valueNames(j)),' tag for the point number ',num2str(i+1)]);
        end
        value=str2num(valueList.item(0).getFirstChild.getNodeValue);
        if (length(value)==0)
            error(['The tag ',char(valueNames(j)),' of point number ',num2str(i+1),' does not contain a numerical value']);
        end
        data(i+1,j)=value;
    end
end
imageData=data(:,3:4);
worldData=data(:,1:2);