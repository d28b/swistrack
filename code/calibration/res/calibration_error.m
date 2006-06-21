%
% Calibration Error Visualization
%
% Nikolaus Correll, Swarm-Intelligent Systems Group, 2005

C=load('calibration_error.txt');
figure, hold on
plot(C(:,3),C(:,4),'.')
plot(C(:,5),C(:,6),'r.')
legend('Original Points','Calibrated Points');

figure, hold on
%I=imread('calibration.bmp');
%imagesc(I);
for I=1:length(C),
plot3([C(I,1); C(I,1)],[C(I,2) C(I,2)],[0 sqrt(C(I,7))])
end
set(gca,'CameraPosition',[522.765 -1518.22 0.0873639]);

figure, hold on
D=[floor(sqrt(C(:,3).^2+C(:,4).^2)*100)/100 sqrt(C(:,7))];

M=[];
S=[];
M(1)=mean(D(find(D(:,1)<0.05),2));
M(2)=mean(D(find(D(:,1)<0.2 & D(:,1)>0.05),2));
M(3)=mean(D(find(D(:,1)<0.3 & D(:,1)>0.2),2));
M(4)=mean(D(find(D(:,1)<0.4 & D(:,1)>0.3),2));
M(5)=mean(D(find(D(:,1)<0.5 & D(:,1)>0.4),2));
S(1)=std(D(find(D(:,1)<0.05),2));
S(2)=std(D(find(D(:,1)<0.2 & D(:,1)>0.05),2));
S(3)=std(D(find(D(:,1)<0.3 & D(:,1)>0.2),2));
S(4)=std(D(find(D(:,1)<0.4 & D(:,1)>0.3),2));
S(5)=std(D(find(D(:,1)<0.5 & D(:,1)>0.4),2));
errorbar(M,S);
