# SelfLocalization

TurtleBot3 using AMCL node for localization, for which Initial Pose Estimation must be performed before running the Navigation as this process initializes the AMCL parameters that are critical in Navigation. TurtleBot3 has to be correctly located on the map with the LDS sensor data that neatly overlaps the displayed map.

This node aims to automate the localization in situations where the navigation stack had to be relaunched, hence the node requires tracking of the latest location of the robot in the simulation, when the navigation stack is relaunced, the node accesses the latest location and initiates locationlization with it automatically.

To test the node, ensure you have TurtleBot3 setup in your ros workspace, i.e you have the turtlebot3, turtlebot3_simulations packages along with robot_pose_publisher(https://github.com/GT-RAIL/robot_pose_publisher).

follow the TurtleBot3 SLAM tutorial(https://emanual.robotis.com/docs/en/platform/turtlebot3/slam/#run-slam-node) and ensure that the map is saved in the map folder of self_localize as map.yaml,map.pgm

run the test_world  along with the navigation stack launch files in self_localize, check that the map loads successful and initialize the robot, as the initial spawn in gazebo and amcl launch are different, this can be changed in the amcl launch file.

now, ctl+c the navigation stack and relaunch and the robot should localize without you having to do so.
