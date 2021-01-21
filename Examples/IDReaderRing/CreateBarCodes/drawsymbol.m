function drawsymbol(N, symbol, name)

symbolchips=dec2bin(symbol, N)-48;

%figure(3);
%clf;
hold on;

% Outer circle (black)
x=sin((1:360)/180*pi)*6;
y=cos((1:360)/180*pi)*6;
H=patch(x, y, [0, 0, 0]);
set(H, 'LineStyle', 'none');

% Symbol chips
steps=ceil(360/N);
for i=1:N
	for n=0:steps
		angle(n+1)=2*pi/N*(i-n/steps);
	end
	x=sin(angle)*4.3;
	y=cos(angle)*4.3;
	x=[x sin(angle(steps+1:-1:1))*3.3];
	y=[y cos(angle(steps+1:-1:1))*3.3];
	H=patch(x, y, [symbolchips(i), symbolchips(i), symbolchips(i)]*0.75);
	set(H, 'LineStyle', 'none');
end

% Inner circle (white)
x=sin((1:360)/180*pi)*2;
y=cos((1:360)/180*pi)*2;
H=patch(x, y, [1, 1, 1]);
set(H, 'LineStyle', 'none');

% Symbol number
H=text(0, -1, [num2str(N), '-', num2str(symbol)]);
set(H, 'HorizontalAlignment', 'center');
set(H, 'VerticalAlignment', 'middle');
set(H, 'FontSize', 6);

% Symbol set name
H=text(0, 1, name);
set(H, 'HorizontalAlignment', 'center');
set(H, 'VerticalAlignment', 'middle');
set(H, 'FontSize', 6);

% Cross in the middle, plus markings around the circle in the middle
line([0, 0], [2.4, 2.5], 'Color', [0, 0, 0], 'LineWidth', 0.1);
line([0, 0], [-2.4, -2.5], 'Color', [0, 0, 0], 'LineWidth', 0.1);
line([2.4, 2.5], [0, 0], 'Color', [0, 0, 0], 'LineWidth', 0.1);
line([-2.4, -2.5], [0, 0], 'Color', [0, 0, 0], 'LineWidth', 0.1);
line([-0.3, 0.3], [0, 0], 'Color', [0, 0, 0], 'LineWidth', 0.1);
line([0, 0], [-0.3, 0.3], 'Color', [0, 0, 0], 'LineWidth', 0.1);

axis off;
axis equal;
