#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)



{

ball_chaser::DriveToTarget srv;

ROS_INFO_STREAM("Hiya, the robot is now online...");

srv.request.linear_x = lin_x;
srv.request.angular_z = ang_z;


if(!client.call(srv)){
ROS_ERROR("Oh, there seems to be a problem...");
	}

	
}


// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
	int off;
	int spotted_total = 0;
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

/*
for (int i = 0; i + 4 < img.data.size(); i = i + 5)
    {
        if ((img.data[i] == 255) && (img.data[i+1] == 255) && (img.data[i+2] == 255) && (img.data[i + 3] == 255) && (img.data[i + 4] == 255))
    {
            off = (i % (img.width * 5)) / 3;
            spotted_total ++;
        }
    }
*/
//changed granularity to suit 3 times 3 1 dimensional matrix of pixels for 'white'
for (int i = 0; i + 2 < img.data.size(); i = i + 3)
    {
        if ((img.data[i] == 255) && (img.data[i+1] == 255) && (img.data[i+2] == 255))
    {
            off = (i % (img.width * 3)) / 3;
            spotted_total ++;
        }
    }
    
    if (spotted_total == 0)
    {
        drive_robot(0.0, 0.0);
    }
    else
    {
        
        if(off < img.width / 3)
        {
            drive_robot(0.4, 0.15);
        }
        else if(off > img.width * 2 / 3)
        {
            drive_robot(0.4, -0.15);
        }
        else
        {
            drive_robot(0.4, 0);
        }
    }


}



int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}