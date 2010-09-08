% 	Computes the maximal error with the given parameters, 
%	uses 'calibration points' file in the same directory
% 	In case of changing parameters pf PSO, input parameters
%	of this function and fixed parameters should be handled.

function y = computeErrorPSO(in )

	rx = in(1);
	ry = in(2);
	rz = in(3);
	R = calculateR2( rx , ry, rz);

	T = [in(4), in(5), 1.60];
	f = in(6);
	k1 = in(7);
	C = [in(8) , in(9)];


%VarRange =  [1 , 2;... %Rz
%		0, 10 ; 0,10;...  % Tx, ty
%		0.5, 2 ;... % f
%		0.01, 0.5 ; ... % k1: 0.2102
%		491, 544; % cx
%		364, 414; % cy


	% fixed paramters
	dx = 0.0046;
	sx = 1;


	% chosen points
	filename = 'cameraX.calibration';
	inputFile = load(filename);

	s = size(inputFile,1);

	newReal=TsaiImage2World(inputFile(:,1:2), R, T, f, k1, C, dx,dx, sx);
	err = abs(inputFile(:,3:4) -  newReal);
	y = max( sqrt( sum(err.^2 , 2)));

end


function R = calculateR2( rx , ry, rz)

Rz=[cos(rz), -sin(rz), 0; sin(rz), cos(rz), 0; 0, 0, 1];
Ry=[cos(ry), 0, sin(ry); 0, 1, 0; -sin(ry), 0, cos(ry)];
Rx=[1, 0, 0; 0, cos(rx), -sin(rx); 0, sin(rx), cos(rx)];
R=Rz*Ry*Rx;
end

