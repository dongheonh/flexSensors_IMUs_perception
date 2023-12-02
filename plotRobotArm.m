function plotRobotArm(roll, pitch, yaw, armLength)
    % Define the length of each arm segment

    % Initialize the starting point
    x = 0;
    y = 0;
    z = 0;

    % Initialize figure
    figure(3);

    % Loop through each joint
    for i = 1:3
        % Calculate the orientation of the segment
        Rx = [1 0 0; 0 cosd(roll(i)) -sind(roll(i)); 0 sind(roll(i)) cosd(roll(i))];
        Ry = [cosd(pitch(i)) 0 sind(pitch(i)); 0 1 0; -sind(pitch(i)) 0 cosd(pitch(i))];
        Rz = [cosd(yaw(i)) -sind(yaw(i)) 0; sind(yaw(i)) cosd(yaw(i)) 0; 0 0 1];

        % Calculate the new end point
        endpoint = [x; y; z] + Rx * Ry * Rz * [armLength; 0; 0];

        % Plot the segment
        plot3([x endpoint(1)], [y endpoint(2)], [z endpoint(3)], 'LineWidth', 2);
        hold on
        grid on;
  
        axis equal;
        xlabel('X');
        ylabel('Y');
        zlabel('Z');
        title('3D Robot Arm');
         
        % Update the starting point for the next segment
        x = endpoint(1);
        y = endpoint(2);
        z = endpoint(3);
    end
    hold off
end

