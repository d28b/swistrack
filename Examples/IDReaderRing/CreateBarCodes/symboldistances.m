function symboldistances(N, symbollist)
% N: number of chips (8-15)
% symbollist: list with symbols (subset of 0 .. 2^N-1)

% Calculate distances
distances=zeros(length(symbollist), length(symbollist));
for id1=1:length(symbollist)
for id2=id1+1:length(symbollist)
	symbol1=symbollist(id1);
	symbol2=symbollist(id2);
	chips1=dec2bin(symbol1, N)-48;
	chips2=dec2bin(symbol2, N)-48;
	mindist=N;
	for i=1:N
		dist=sum(abs(chips1-chips2));
		if dist<mindist, mindist=dist;end
		chips2=[chips2(2:end), chips2(1)];
	end
	distances(id1, id2)=mindist;
end
end
distances

% Plot distances
figure(2)
imagesc(distances);
axis equal;
