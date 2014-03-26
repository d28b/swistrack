function savePart(filename, pointCount)

f = load(filename);
s = size(f,1);
r = rand(s,1);
fr = pointCount/s;

selection = find(r<fr);

fs = f(selection, :);
size(selection)
save  'cameraX.calibration.part' fs '-ASCII';


end
