function [angle, distance]=createmask(masksize, center, rmin, rmax)

cx=center(1);
cy=center(2);

masksize=masksize(1:2);
angle=zeros(masksize);
distance=zeros(masksize);
for x=1:masksize(1)
for y=1:masksize(2)
	r=sqrt((x-cx)^2+(y-cy)^2);
	if r>=rmin && r<=rmax
		angle(x, y)=atan2(y-cy, x-cx);
		distance(x, y)=r;
	end
end
end
