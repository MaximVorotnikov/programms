#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "sensor_msgs/PointCloud.h"
#include "math.h"


sensor_msgs::LaserScan rangesnow;
sensor_msgs::LaserScan rangesObrabotan;
ros::Publisher pubranges;
ros::Publisher pubcloude;
ros::Publisher pubcloudeKvad;


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
    point.header.frame_id ="base_scan";
    for(int i = 0; i < rangesnowfunCloude.ranges.size(); i++)
    {
        geometry_msgs::Point32 p;
        float x, y;
        p.x = rangesnowfunCloude.ranges.at(i) * cos(i * rangesnowfunCloude.angle_increment);
        p.y = rangesnowfunCloude.ranges.at(i) * sin(i * rangesnowfunCloude.angle_increment);
        point.points.push_back(p);

        ROS_INFO("\n x = %f \ny = %f\n", point.points.at(i).x, point.points.at(i).y);
    }
    pubcloude.publish(point);
}


void pointCloudeKvadrat(sensor_msgs::LaserScan rangesnowfunCloudeKvad)
{
    rangesnowfunCloudeKvad.range_max = 1;
    sensor_msgs::PointCloud pointKvad;
    pointKvad.header.stamp = ros::Time::now();
    pointKvad.header.frame_id ="base_scan";
    for(int i = 0; i < rangesnowfunCloudeKvad.ranges.size(); i++)
    {
            geometry_msgs::Point32 p;
            float x, y;
            {
            if (i * rangesnowfunCloudeKvad.angle_increment <= M_PI/4 || i * rangesnowfunCloudeKvad.angle_increment >= (M_PI / 4) * 7 && i * rangesnowfunCloudeKvad.angle_increment <= 2 * M_PI)
            {
                p.x = rangesnowfunCloudeKvad.range_max/sqrt(2);
                p.y = rangesnowfunCloudeKvad.ranges.at(i) * sin(i * rangesnowfunCloudeKvad.angle_increment);
                pointKvad.points.push_back(p);                
            }
            if (i * rangesnowfunCloudeKvad.angle_increment > M_PI / 4 && i * rangesnowfunCloudeKvad.angle_increment <= (M_PI / 4) * 3)
            {
                p.y = rangesnowfunCloudeKvad.range_max/sqrt(2);
                p.x = rangesnowfunCloudeKvad.ranges.at(i) * cos(i * rangesnowfunCloudeKvad.angle_increment);
                pointKvad.points.push_back(p);                
            }
            if (i * rangesnowfunCloudeKvad.angle_increment > (M_PI / 4) * 3 && i * rangesnowfunCloudeKvad.angle_increment <= (M_PI / 4) * 5)
            {
                p.x = rangesnowfunCloudeKvad.range_max/-sqrt(2);
                p.y = rangesnowfunCloudeKvad.ranges.at(i) * sin(i * rangesnowfunCloudeKvad.angle_increment);
                pointKvad.points.push_back(p);                
            }
            if (i * rangesnowfunCloudeKvad.angle_increment > (M_PI / 4) * 5 && i * rangesnowfunCloudeKvad.angle_increment < (M_PI / 4) * 7)
            {
                p.y = rangesnowfunCloudeKvad.range_max/-sqrt(2);
                p.x = rangesnowfunCloudeKvad.ranges.at(i) * cos(i * rangesnowfunCloudeKvad.angle_increment);
                pointKvad.points.push_back(p);                
            }
            }
    }
    pubcloudeKvad.publish(pointKvad);

}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "lidar_xy");
    ros::NodeHandle n;
    pubranges = n.advertise<sensor_msgs::LaserScan>("/scan_limited", 1000);
    pubcloude = n.advertise<sensor_msgs::PointCloud>("/point_xy", 1000);
    pubcloudeKvad = n.advertise<sensor_msgs::PointCloud>("/point_xyKvadrat", 1000);
    ros::Subscriber sub = n.subscribe("/scan", 1000, ranges_Cb);

    ros::Rate loop_rate(10);

    while (ros::ok())
    {
        rangesObrabotan = ranges(rangesnow);
        pointCloude(rangesObrabotan);
        pointCloudeKvadrat(rangesObrabotan);
        ros::spinOnce();
        loop_rate.sleep();
    }
    



}