function err = testParameters(pointsFile, parameterFile)



points = load(pointsFile);
image = points(:,1:2);
world = points(:,3:4);


% function [R,T,C, dx, dy,sx, k,f ]=readTSAICalibrationParametersXMLFile(fileName);

[R , T , C , dx , dy, sx , k1, f] = readTSAICalibrationParametersXMLFile(parameterFile);



 newReal=TsaiImage2World(image, R, T, f, k1, C, dx, dy, sx);
[averageError, maxError, stdError]=computeError(world, newReal,true);
err= maxError(3);
averageError
 maxError
 stdError
    % Display Error
    figure;
    hold on;
    plot(world(:,1),world(:,2),'+b');
    plot(newReal(:,1),newReal(:,2),'+r');
    legend('Orig. World Points','Computed World Points','Location','SouthOutside');

	saveas(gcf, strcat(parameterFile, '.png'), 'png');
end

