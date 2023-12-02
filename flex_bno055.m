% Author: Dong Heon Han
% FREE lab
% 12/02/2023

clear, clc
tic;                        % record time

% check the serial port name in the terminal, type: ls /dev/tty.*
s = serialport("/dev/tty.usbmodem14401",9600);

orient = [];                % ith column is ith angular orientation 
maxIteration = 2*10^2;      % approximately 20 seconds of data collecting
iteration = 0;              % initialize the iteration

armLength = 80;             % each of arms length in mm 
number_IMUs = 3;            % numbers of IMUs

% Roll pitch and yaw vectors
roll  = zeros(number_IMUs,1);
pitch = zeros(number_IMUs,1);
yaw   = zeros(number_IMUs,1);

%% Read the first line to Update the offsets (Calibration)
% Assume that the robot starts from the posture if vertical straight line

data = readline(first_line);

% # of datas in line: # of joints * 4, modify when the # of joints change
first_line = sscanf(data, "%f, %f, %f, %f, %f, %f," + ...
    " %f, %f, %f, %f, %f, %f");
orient_iter_ea = parsedData';

for i = 1: number_IMUs
    roll(i)  = orient_iter_ea(3*(i-1) + 1);
    pitch(i) = orient_iter_ea(3*(i-1) + 2);
    yaw(i)   = orient_iter_ea(3*(i-1) + 3);
end

% data from the flex sensor
roll_flex = flexSensor_volt_angle(orient_iter_ea(length(orient_iter_ea) - 2 ...
    : length(orient_iter_ea)));

% obtain the offsets (from IMUs)
offset_roll  = 90*ones(number_IMUs,1) - roll;
offset_pitch = 90*ones(number_IMUs,1) - pitch;
offset_yaw   = 90*ones(number_IMUs,1) - yaw;

% flex sensor's offsets
offset_flex = 90*ones(number_IMUs,1) - roll_flex;

%% Data Collection
whileFlag = 1;
while whileFlag
    iteration = iteration + 1;

    % Read data from serial port
    data = readline(s);
    parsedData = sscanf(data, "%f, %f, %f, %f, %f, %f, " + ...
        "%f, %f, %f, %f, %f, %f");

    orient_iter_ea = parsedData';

    for i = 1: 3
        roll(i)  = orient_iter_ea(3*(i-1) + 1);
        pitch(i) = orient_iter_ea(3*(i-1) + 2);
        yaw(i)   = orient_iter_ea(3*(i-1) + 3);
    end

    % data from the flex sensor
    roll_flex = orient_iter_ea(length(orient_iter_ea) - 2: ...
        length(orient_iter_ea));

    % Consider offset at every iterations 
    roll  = roll  + offset_roll;
    pitch = pitch + offset_pitch;
    yaw   = yaw   + offset_yaw;

    roll_flex = roll_flex + offset_flex;

    fprintf('roll: %d, yaw: %d, pitch: %d\n', ...
        roll, yaw, pitch) % print roll, yaw, pitch values
    
    % @ figure 1
    figrue(1)
    plotRobotArm(roll, pitch, yaw, armLength) % PLOT robot_IMU_data

    % @ figure 2
    figure(2)
    plotRobotArm(roll_flex, 90, 90, armLength) % PLOT robot_IMU_data

    % (optional) save orientation data - expand the matrix size
    orient = [orient; orient_iter_ea];

    
    % if iteration reach maxIteratioan terminate
    if iteration == maxIteration
        fprintf('done with collecting orientation')
        whileFlag = 0;
    end

end

toc;    % record time