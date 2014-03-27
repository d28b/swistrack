function exportsymbols(N, symbollist, name)

if nargin<3, name='exportsymbols', end

mkdir(name);

% create and write to XML file
filename = [name '/' name '.xml'];
xmlfileID = fopen(filename,'w');
disp(['Creating ' filename]);
fprintf(xmlfileID,'<?xml version="1.0"?>\n');
fprintf(xmlfileID,'  <objectlist>\n');
fprintf(xmlfileID,'    <objects>\n');

for i=1:length(symbollist)
	% save .eps 
    filename=[name '/' num2str(N) '-' num2str(symbollist(i)) '.eps'];
	disp(['Creating ' filename]);
	drawsymbol(N, symbollist(i), [name ' ' num2str(i)]);
	print(gcf, '-deps', filename);

    % write to XML file
    fprintf(xmlfileID,'      <object id="%d">\n',i);
    fprintf(xmlfileID,'        <barcode>\n');
    fprintf(xmlfileID,'          <chips length="%d" symbol="%d" />\n',N,symbollist(i));
    fprintf(xmlfileID,'          <angle>0</angle>\n');
    fprintf(xmlfileID,'        </barcode>\n');
    fprintf(xmlfileID,'      </object>\n');
end

% write to XML file and close it
fprintf(xmlfileID,'    </objects>\n');
fprintf(xmlfileID,'  </objectlist>\n');
fclose(xmlfileID);