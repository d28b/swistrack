function exportsymbols(N, symbollist, name)

if nargin<3, name='exportsymbols', end

mkdir(name);

for i=1:length(symbollist)
	filename=[name '/' num2str(N) '-' num2str(symbollist(i)) '.eps'];
	disp(['Creating ' filename]);
	drawsymbol(N, symbollist(i), [name ' ' num2str(i)]);
	print(gcf, '-deps', filename);
end
