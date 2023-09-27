#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "sensor_msgs/PointCloud.h"
#include "math.h"


sensor_msgs::LaserScan rangesnow;
sensor_msgs::LaserScan rangesObrabotan;
ros::Publisher pubranges;
ros::Publisher pubcloude;


void ranges_Cb(const sensor_msgs::LaserScan::ConstPtr& rangesCb)
{
    rangesnow = *rangesCb;
}

 // for(int i = 0; i * rangesnowfunCloude.angle_increment < 2 * M_PI; i++)
sensor_msgs::LaserScan ranges(sensor_msgs::LaserScan rangesnowfun)
{
    rangesnowfun.header.stamp = ros::Time::now();
    rangesnowfun.header.frame_id = "laser";
    for(int i = 0; i < rangesnowfun.ranges.size(); i++)
    {
        if (rangesnowfun.ranges.at(i) > 1 || rangesnowfun.ranges.at(i) == INFINITY)
        rangesnowfun.ranges.at(i) = 1;
    }
    pubranges.publish(rangesnowfun);
    return rangesnowfun;
}


void pointCloude(sensor_msgs::LaserScan rangesnowfunCloude)
{
    sensor_msgs::PointCloud point;
    point.header.stamp = ros::Time::now();
    point.header.frame_id ="laser";
    for(int i = 0; i < rangesnowfunCloude.ranges.size(); i++)
    {
        geometry_msgs::Point32 p;
        float x, y;
        // point.points.at(i).x = rangesnowfunCloude.ranges.at(i) * cos(i * rangesnowfunCloude.angle_increment * 180/M_PI);
        // p.x = point.points.at(i).x;
        // point.points.at(i).y = rangesnowfunCloude.ranges.at(i) * sin(i * rangesnowfunCloude.angle_increment * 180/M_PI);
        // p.y = point.points.at(i).y;
        // point.points.push_back(p);

        p.x = rangesnowfunCloude.ranges.at(i) * cos(i * rangesnowfunCloude.angle_increment*180/M_PI);
        p.y = rangesnowfunCloude.ranges.at(i) * sin(i * rangesnowfunCloude.angle_increment*180/M_PI);
        point.points.push_back(p);

        ROS_INFO("\n x = %f \ny = %f\n", point.points.at(i).x, point.points.at(i).y);
    }
    pubcloude.publish(point);
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "lidar_xy");
    ros::NodeHandle n;
    pubranges = n.advertise<sensor_msgs::LaserScan>("/scan_limited", 1000);
    pubcloude = n.advertise<sensor_msgs::PointCloud>("/point_xy", 1000);
    ros::Subscriber sub = n.subscribe("/scan", 1000, ranges_Cb);

    ros::Rate loop_rate(10);

    while (ros::ok())
    {
        rangesObrabotan = ranges(rangesnow);
        pointCloude(rangesObrabotan);
        ros::spinOnce();
        loop_rate.sleep();
    }
    



}