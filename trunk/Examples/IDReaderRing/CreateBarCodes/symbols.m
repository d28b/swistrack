function [symbollist]=symbols(N, mindist)
% N: number of chips (8-15)
% mindist: minimum distance between two symbols (1-4)

% Find possible symbols
symbollist=[];
symbol=0:(2^N-1);
symbolchips=dec2bin(symbol, N)-48;
mask=ones(size(symbol));
while 1
	maskbest=max(mask);
	if maskbest==0, break; end
	selid=find(mask==maskbest);
	cur=selid(floor(rand()*length(selid))+1)-1;
	symbollist(end+1)=cur;
	curchips=dec2bin(cur, N)-48;

	for i=1:N
		diff=abs(symbolchips-repmat(curchips, length(symbol), 1));
		dist=sum(diff, 2);
		selid=find(dist<mindist);
		mask(selid)=0;
		
		selid=find((dist==mindist) & (mask'>0));
		mask(selid)=mask(selid)+1;

		curchips=[curchips(2:end), curchips(1)];
	end
end

% This is the list with the possible symbols
symbollist=sort(symbollist);

% Plot
figure(1)
symbollistchips=dec2bin(symbollist, N)-48;
imagesc(symbollistchips);
axis equal;
