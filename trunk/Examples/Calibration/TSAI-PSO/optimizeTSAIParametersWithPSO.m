%      Runs PSO toolbox for the desired parameters, stores the best parameter set using
%	function WriteTSAICalibrationParametersXMLFile(R,T,C,dx,dy,sx,k1,f,fileName);
%	
%	since just the parameters which will be optimized are passed to the objective 
%	function( computeErrorPSO ), the other paramters are hard coded to this file
%	and to the computeErrorPSO file

% 	number of the parameters to be optimized can be changed, their search interval
%	should be changed in this file
%	required changes should be done in the computerErrorPSO function

function y = optimizeTSAIParametersWithPSO()


%		PSO PARAMETERS
%      P(1) - Epochs between updating display, default = 100. if 0, 
%             no display
%      P(2) - Maximum number of iterations (epochs) to train, default = 2000.
%      P(3) - population size, default = 24
%
%      P(4) - acceleration const 1 (local best influence), default = 2
%      P(5) - acceleration const 2 (global best influence), default = 2
%      P(6) - Initial inertia weight, default = 0.9
%      P(7) - Final inertia weight, default = 0.4
%      P(8) - Epoch when inertial weight at final value, default = 1500
%      P(9)- minimum global error gradient, 
%                 if abs(Gbest(i+1)-Gbest(i)) < gradient over 
%                 certain length of epochs, terminate run, default = 1e-25
%      P(10)- epochs before error gradient criterion terminates run, 
%                 default = 150, if the SSE does not change over 250 epochs
%                               then exit
%      P(11)- error goal, if NaN then unconstrained min or max, default=NaN
%      P(12)- type flag (which kind of PSO to use)
%                 0 = Common PSO w/intertia (default)
%                 1,2 = Trelea types 1,2
%                 3   = Clerc's Constricted PSO, Type 1"
%      P(13)- PSOseed, default=0
%               = 0 for initial positions all random
%               = 1 for initial particles as user input
%


P = zeros(13);
P(1) = 50;
P(2) = 10000;
P(3) = 96;
P(4) = 2;
P(5) = 2;
P(6) = 9;
P(7) = 0.3;
P(8) = 1500;
P(9) = 1e-25;
P(10) = 150;
P(11) = NaN;
P(12) = 0;
P(13) = 0; 

% [optOUT,tr,te]=...
%        PSO(functname,D,mv,VarRange,minmax,PSOparams,plotfcn,PSOseedValue)
% function --> will be a test function
%  D - # of inputs to the function (dimension of problem)
functname = 'computeErrorPSO';
D = 9;



% opional inputs:
mv = [ 0.0001, 0.0001 , 0.005 , 0.01 , 0.01 , 0.001, 0.0001, 0.05, 0.05] ; %max velocities


VarRange =  [ 	-0.2 , 0.2;... %Rx
		3 , 3.3 ; % ry
		1 , 2;... %Rz
		0, 20 ; 0,20;...  % Tx, ty
		1.5 , 3 ;... % f
		0.1, 0.5 ; ... % k1: 0.2102
		491, 544; % cx
		364, 414 ]  % cy


%  

% minmax = 0, funct minimized (default)
minmax = 0;



% first D of the optOUT are the optimum values of the parameters, the last one is the corresponding error
[optOUT,tr,te] = pso_Trelea_vectorized(functname,D,mv,VarRange,minmax, P);



%function WriteTSAICalibrationParametersXMLFile(R,T,C,dx,dy,sx,k1,f,fileName);


rx = optOUT(1);
ry = optOUT(2);
rz = optOUT(3);
R = calculateR2( rx , ry, rz);

T = [optOUT(4), optOUT(5), 1.60];
f = optOUT(6);
k1 = optOUT(7);
C = [optOUT(8) , optOUT(9)];

% fixed parameters
dx = 0.0046;
sx = 1;

WriteTSAICalibrationParametersXMLFile(R,T,C,dx,dx,sx,k1,f, 'parameters');
optOUT

end


function R = calculateR2( rx , ry, rz)

Rz=[cos(rz), -sin(rz), 0; sin(rz), cos(rz), 0; 0, 0, 1];
Ry=[cos(ry), 0, sin(ry); 0, 1, 0; -sin(ry), 0, cos(ry)];
Rx=[1, 0, 0; 0, cos(rx), -sin(rx); 0, sin(rx), cos(rx)];
R=Rz*Ry*Rx;
end

