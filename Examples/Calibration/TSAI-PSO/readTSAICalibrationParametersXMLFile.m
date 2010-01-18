function [R,T,C, dx, dy,sx, k,f ]=readTSAICalibrationParametersXMLFile(fileName);


if not(exist('fileName','var'))
    [fileName, pathName] = uigetfile('*.xml', 'Pick a input file');
    fileName=[pathName,fileName];
end

valueNames={'xworld','yworld','ximage','yimage'};
xDoc=xmlread(fileName);
calibration=xDoc.getElementsByTagName('calibration');
calibration1 = calibration.item(0);
parameters=calibration1.getElementsByTagName('parameters');


Rpart = parameters.item(0);
Rlist = Rpart.getElementsByTagName('R');
R = zeros(3,3);


for i=1:3
	for j=1:3
		p  = Rlist.item(0).getElementsByTagName(strcat( char('a'-1+i),char('a'-1+j) ));
		R(i,j) = str2double(p.item(0).getFirstChild.getNodeValue);
	end
end


Tpart = parameters.item(0);
Tlist =	Tpart.getElementsByTagName('T');
T = zeros(3,1);
for i=1:3
	p  = Tlist.item(0).getElementsByTagName(strcat('T' , char('w' + i) ));
	T(i) = str2double(p.item(0).getFirstChild.getNodeValue);
end


Clist = parameters.item(0).getElementsByTagName('C');
C = zeros(2,1);
for i=1:2
	p = Clist.item(0).getElementsByTagName(strcat('C' , char('w' + i) ));
	C(i) = str2double(p.item(0).getFirstChild.getNodeValue);
end


dx_ = parameters.item(0).getElementsByTagName('dx');
dx =  str2double(dx_.item(0).getElementsByTagName('value').item(0).getFirstChild.getNodeValue);

dy_ = parameters.item(0).getElementsByTagName('dy');
dy =  str2double(dy_.item(0).getElementsByTagName('value').item(0).getFirstChild.getNodeValue);


sx_ = parameters.item(0).getElementsByTagName('sx');
sx =  str2double(sx_.item(0).getElementsByTagName('value').item(0).getFirstChild.getNodeValue);

k_ = parameters.item(0).getElementsByTagName('k');
k =  str2double(k_.item(0).getElementsByTagName('value').item(0).getFirstChild.getNodeValue);


f_ = parameters.item(0).getElementsByTagName('f');
f = str2double(f_.item(0).getElementsByTagName('value').item(0).getFirstChild.getNodeValue);

end
