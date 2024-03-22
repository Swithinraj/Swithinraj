#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <cmath>

class HuskyMove {
private:
    ros::NodeHandle nh;
    ros::Subscriber odom_sub;
    ros::Publisher vel_pub;
    double target_x;
    double target_y;
    bool reached;

public:
    HuskyMove() : target_x(10.0), target_y(1.0), reached(false) {
        odom_sub = nh.subscribe("/odometry/filtered", 1, &HuskyMove::odomCallback, this);
        vel_pub = nh.advertise<geometry_msgs::Twist>("/twist_marker_server/cmd_vel", 1);
    }

    void odomCallback(const nav_msgs::Odometry::ConstPtr& msg) {
        double current_x = msg->pose.pose.position.x;
        double current_y = msg->pose.pose.position.y;
        double distance_to_target = std::sqrt(std::pow(target_x - current_x, 2) + std::pow(target_y - current_y, 2));

        ROS_INFO("Distance to target: %.2f meters", distance_to_target);

        if (distance_to_target <= 0.5 && !reached) {
            reached = true;
            ROS_INFO("Target reached!");
            stopRobot();
        } else if (!reached) {
            geometry_msgs::Twist vel_msg;
            vel_msg.linear.x = 0.2; 
            vel_msg.angular.z = std::atan2(target_y - current_y, target_x - current_x);
            vel_pub.publish(vel_msg);
        }
    }

    void stopRobot() {
        geometry_msgs::Twist vel_msg;
        vel_pub.publish(vel_msg);
    }
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "robot_driver");
    HuskyMove driver;
    ros::spin();
    return 0;
}

