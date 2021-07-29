#include <ros/ros.h>
#include "std_msgs/String.h"
#include <geometry_msgs/Pose.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <tf/transform_broadcaster.h>

#include <ros/package.h>

#include<iostream>
#include<sstream>
#include<fstream>
#include<iomanip>
#include <sys/stat.h>
#include <tuple>


#include <cstdlib> // for exit function

using namespace std;

vector<float> convertStrtoArr(string str)
{

	vector<string> v;
	stringstream ss(str);
	vector<float> arr;
	while (ss.good()) {
		string substr;
		getline(ss, substr, ',');
		arr.push_back(atof (substr.c_str()));
	}
  return arr;
}


bool fileExists(const std::string& filename)
{
	struct stat buf;
	if (stat(filename.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

std::string toString(auto &i) 
{
	std::stringstream ss;
	ss << i; 
	return ss.str();
}


class Localizer
{

  /**
 * This module allows for automatic self-localization in case of navigation crash.
 */

  public:
	 fstream my_file;
	 bool localising;
	 std::string currentPose;
	 float position_array[3];
	 float orientation_array[4];
	 std::string data_path;


	Localizer()
	{

	  std::string dataFileName = "pose-data";
	  std::string package_path = ros::package::getPath("self_localize");
	  data_path = package_path + "/localize-data/" + dataFileName;
	  std::vector<vector<float>> location_array;

	  if (fileExists(data_path) == true)
	  {
		localising = true;
		my_file.open(data_path, ios::in);
		if(my_file.is_open()) 
		{
		  my_file.seekg(-2,ios_base::end);                // go to one spot before the EOF

		  bool keepLooping = true;
		  while(keepLooping) 
		  {
			  char ch;
			  my_file.get(ch);                            // Get current byte's data

			  if((int)my_file.tellg() <= 1) {             // If the data was at or before the 0th byte
				  my_file.seekg(0);                       // The first line is the last line
				  keepLooping = false;                // So stop there
			  }
			  else if(ch == '\n') {                   // If the data was a newline
				  keepLooping = false;                // Stop at the current position.
			  }
			  else {                                  // If the data was neither a newline nor at the 0 byte
				  my_file.seekg(-2,ios_base::cur);        // Move to the front of that data, then to the front of the data before it
			  }
		  }

		  string lastLine;            
		  getline(my_file,lastLine);                      // Read the current line
		  std::istringstream input(lastLine);
		  std::string word;
		  while (input >> word)
			  location_array.push_back(convertStrtoArr(word));


		  for (int j = 0; j < location_array[0].size(); j++)
		  {
			position_array[j] = location_array[0][j];
		  }

		  for (int j = 0; j < location_array[1].size(); j++)
		  {
			orientation_array[j] = location_array[1][j];
		  }
		}
	  }

	  else
	  {
		my_file.open(data_path, ios::out);
	  }
	
	}
	void chatterCallback(const geometry_msgs::Pose& msg)
	{
	  std::string position_array_str = toString(msg.position.x)+","+toString(msg.position.y) +","+toString(msg.position.z);
	  std::string orientation_array_str = toString(msg.orientation.x)+","+toString(msg.orientation.y) +","+toString(msg.orientation.z)+","+toString(msg.orientation.w);
	  my_file  << position_array_str << " " <<orientation_array_str <<endl;
	}

	void endnode(void)
	{
	  cout << "saved data point in:"<< data_path << endl;
	  my_file.close();
	}


};


int main(int argc, char **argv)
{

  ros::init(argc, argv, "listener");

  Localizer obj;
  ros::Time begin;

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;
  if (obj.localising == true)
  {
	cout << "reading from file" << endl;
	ros::Publisher initial_pose_pub = n.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose", 0);
	ros::Rate loop_rate(10);

	while (initial_pose_pub.getNumSubscribers() < 1) 
	{
		cout << "waiting" << endl;
	}

	//for(int i = 0 ; i<11 ; i++)
	//{
	  while (true) 
		{
		  begin = ros::Time::now();
		  if (begin.sec > 0)
		  {
			break;
		  }
		}
		geometry_msgs::PoseWithCovarianceStamped pose_msg;

		pose_msg.header.stamp.sec = begin.sec;
		pose_msg.header.stamp.nsec = begin.nsec;
		pose_msg.header.frame_id = "map";
		pose_msg.pose.pose.position.x = obj.position_array[0];
		pose_msg.pose.pose.position.y = obj.position_array[1];
		pose_msg.pose.pose.position.z = obj.position_array[2];
		pose_msg.pose.covariance[0] = 0.25;
		pose_msg.pose.covariance[6 * 1 + 1] = 0.25;
		pose_msg.pose.covariance[6 * 5 + 5] = 0.06853891945200942;
		pose_msg.pose.pose.orientation.x = obj.orientation_array[0];
		pose_msg.pose.pose.orientation.y = obj.orientation_array[1];
		pose_msg.pose.pose.orientation.z = obj.orientation_array[2];
		pose_msg.pose.pose.orientation.w = obj.orientation_array[3];
	
		initial_pose_pub.publish(pose_msg);
	  //}
  }

  else
  {
	ros::Subscriber sub = n.subscribe("/robot_pose", 1000, &Localizer::chatterCallback, &obj);
	ros::spin();
  }  
  obj.endnode();
  return 0;
}
