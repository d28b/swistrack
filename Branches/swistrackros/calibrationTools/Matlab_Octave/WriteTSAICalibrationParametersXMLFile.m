function WriteTSAICalibrationParametersXMLFile(R,T,C,dx,dy,sx,k1,f,fileName);
% function WriteTSAICalibrationParametersXMLFile(R,T,C,dx,dy,sx,k1,f,fileName);
%
% Writes an XML file for SwisTrack's TSAI calibration component.
% This XML provides the calibration parameters, computed outside SwisTrack. 
% It is based on the TSAI coplanar calibration model.
%
% Input:
% R     Rotation Matrix of the pinhole camera model (3D)
% T     Translation Matrix of the pinhole camera model (3D)
% C     Lens optical center, in the camera array referential (in pixels)
%       Most of the time, it corresponds to the image center (2D)
% dx    Size, along x, of a camera pixel (look at the camera datasheet)
% dy    Size, along y, of a camera pixel (look at the camera datasheet)
% sx    Uncertainty on dx/dy (In fact the real dx used is dx/sx)
%       If you don't know, set this value to 1 (or close to one)
% k1    1st order factor of the radial distorsion
% f     Lens focal
% fileName  Name of the output file name. If no file name is provided,
%           the points are written to "TSAICalibrationParameters.xml".

%
% Pierre Roduit, 2009-05-28, pierre.roduit@epfl.ch

% Check input parameters
if not(exist('fileName','var'))
    fileName='TSAICalibrationParameters.xml';
end

% Write file
writeFile = fopen(fileName, 'w');
fprintf(writeFile, '<?xml version="1.0"?>\r\n<calibration>\r\n\t<parameters>\r\n');

%R
fprintf(writeFile, '\t\t<R>\r\n');
for i=1:3
    for j=1:3
        fprintf(writeFile, ['\t\t\t<',char('a'-1+i),char('a'-1+j) ,'>' num2str(R(i,j)) '</',char('a'-1+i),char('a'-1+j) ,'>\r\n']);
    end
end
fprintf(writeFile, '\t\t</R>\r\n');

%T
fprintf(writeFile, '\t\t<T>\r\n');
for i=1:3    
    fprintf(writeFile, ['\t\t\t<T',char('w'+i) ,'>' num2str(T(i)) '</T',char('w'+i),'>\r\n']);
end
fprintf(writeFile, '\t\t</T>\r\n');

%C
fprintf(writeFile, '\t\t<C>\r\n');
for i=1:2    
    fprintf(writeFile, ['\t\t\t<C',char('w'+i) ,'>' num2str(C(i)) '</C',char('w'+i),'>\r\n']);    
end
fprintf(writeFile, '\t\t</C>\r\n');

%dx
fprintf(writeFile, '\t\t<dx>\r\n');
fprintf(writeFile, ['\t\t\t<value>' num2str(dx) '</value>\r\n']);    
fprintf(writeFile, '\t\t</dx>\r\n');

%dy
fprintf(writeFile, '\t\t<dy>\r\n');
fprintf(writeFile, ['\t\t\t<value>' num2str(dy) '</value>\r\n']);    
fprintf(writeFile, '\t\t</dy>\r\n');

%sx
fprintf(writeFile, '\t\t<sx>\r\n');
fprintf(writeFile, ['\t\t\t<value>' num2str(sx) '</value>\r\n']);    
fprintf(writeFile, '\t\t</sx>\r\n');

%k1
fprintf(writeFile, '\t\t<k>\r\n');
fprintf(writeFile, ['\t\t\t<value>' num2str(k1) '</value>\r\n']);    
fprintf(writeFile, '\t\t</k>\r\n');

%f
fprintf(writeFile, '\t\t<f>\r\n');
fprintf(writeFile, ['\t\t\t<value>' num2str(f) '</value>\r\n']);    
fprintf(writeFile, '\t\t</f>\r\n');

fprintf(writeFile, '\t</parameters>\r\n</calibration>');
fclose(writeFile);
