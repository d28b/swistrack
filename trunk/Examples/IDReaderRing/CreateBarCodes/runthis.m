% http://en.wikibooks.org/wiki/SwisTrack/Components/IDReaderRing
% http://en.wikibooks.org/wiki/SwisTrack/Examples/IDReaderRing

% Note that the algorithm assumes a perfectly circular barcode,
% and therefore doesn't perform very well under large deformations.
% In addition, even though the algorithm is near-optimal in a well-configured setup,
% a fairly high resolution (and a well-focussed lens) is required to read such codes.
% E.g., 14-chip codes should appear as 20-by-20 pixels at least on the image.

% The number of chips as well as the Hamming distance will greatly influence the number of available codes. 
% There are a number of trade-offs involved, however:
%
%   * The more chips you use, the more codes you'll get.
%   * The bigger the Hamming distance, the fewer codes you'll get.
%   * The more chips you use, the more resolution you need 
%     (i.e. better camera resolution or bigger blobs on the camera image).
%     This is a constraint imposed by the Nyquist criterion.
%     As a rule of thumb, the image should be at least N by N pixels for a code with 0.7N chips.
%   * The bigger the Hamming distance, the more robust the detection/discrimination is going to be.
%
% Hence, you should find a configuration that provides about the number of blobs that you really need.
% One or two times more codes than you need is OK and may serve you as spare codes
% in case you'll need more at some later time, but if your configuration yields 10 or 100 times more codes, 
% you unnecessarily trading off detection quality with number of codes.

%%%%%%%%%%%%%%%%%%%% set these parameters %%%%%%%%%%%%%%%%%%
chips = 13; % number of chips (8-15)
hdist = 4; % minimum (Hamming) distance between two symbols (1-4)
name = 'Test'; % name of the ID set
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

symbollist=symbols(chips, hdist) % (chips, hamming distance)
nID = size(symbollist,2); % number of IDs

%(optional) Check the symbol distances:
%symboldistances(14, symbollist);

% draw all symbols on the same figure
figure;
j = ceil(sqrt(nID));
for i = 1:nID
    subplot(j,j,i);
    drawsymbol(chips, symbollist(i), [name,num2str(i)]);
end


%Draw (and save/export) all symbols:
s = input('Do you want to save these IDs (y/n)?','s');
if s == 'y'
    figure;
    exportsymbols(chips, symbollist, name)
else
    close all %figures
end
    
