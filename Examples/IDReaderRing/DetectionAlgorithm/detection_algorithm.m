% Configuration
img=imread('barcode.bmp');	% Image with the barcode
rmax=6;						% Outer radius of the barcode [pixel]
rmin=2;						% Inner radius of the barcode [pixel]
center=[7.5, 7.5];			% Center of the blob [pixel]

% Convert to mono
img=sum(double(img), 3);

% Create and plot the angle/distance maps
[angle, distance]=createmask(size(img), center, rmin, rmax);

figure(1);
imagesc(angle);
axis square;

figure(2);
imagesc(img-distance*100);
axis square;

% Read the signal along the circle
nw=1;
for x=1:size(img, 1)
for y=1:size(img, 2)
	if distance(x, y)
		signalx(nw)=angle(x, y);
		signaly(nw)=img(x, y);
		nw=nw+1;
	end
end
end

% Plot the signal (polar histogram)
figure(3);
clf;
plot(signalx, signaly, '.k');

hold on
for i=-180:(360/14):180
	plot([i/180*pi, i/180*pi], [180, 255], 'b-');
end

integration=(360/14)/180*pi;
for i=1:360
	sigx=signalx-i/180*pi;
	sel=find(sigx<0);
	sigx(sel)=sigx(sel)+2*pi;
	sel=find(sigx<0);
	sigx(sel)=sigx(sel)+2*pi;
	sel=find(sigx<0);
	sigx(sel)=sigx(sel)+2*pi;

	sel=find(sigx<=integration);
	s(i)=mean(signaly(sel));
end

figure(4);
polar((1:360)/180*pi, s-min(s));