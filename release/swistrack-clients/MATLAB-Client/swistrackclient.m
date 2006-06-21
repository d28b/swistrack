% Matlab client for SwisTrack demo
% 
% (c) 2005 Nikolaus Correll and Gregory Mermoud
%
% Requires Instrumentation Toolbox.

host = 'localhost';
port = 3000;


%%% Open and initialize the connection
t = tcpip(host,port);
set(t,'InputBufferSize',1024); % Put the maximum amount of data that you will be receiving here
set(t,'OutputBufferSize',1);
fopen(t);
%warning off instrument:fscanf:unsuccessfulRead
%figure;
hold on;
set(gcf,'DoubleBuffer','On');
%%% Queries data
xstack=[];
ystack=[];



fprintf(t,'%c','N'); % Asks for the number of tracks
N=fscanf(t,'%d',4);  % Reads the number of tracks (4 bytes)
len=N*22;            % Calculates the width of the string that contains all tracks
for I=1:100,
 fprintf(t,'%c','T'); % Asks for trajectory data
 X=fscanf(t,'%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f',len);
 if(length(X)==2*N),
     x=X(1:2:2*N);
     y=X(2:2:2*N);
     xstack=[xstack x];
     ystack=[ystack y];
     plot(xstack',ystack','.')
     drawnow;
 end;    
end;


 
%%% Disconnects from the server
fclose(t);
delete(t);
clear t

