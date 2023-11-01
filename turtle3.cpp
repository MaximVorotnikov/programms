#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "math.h"
#include "sensor_msgs/LaserScan.h"

ros::Publisher pub;
sensor_msgs::LaserScan rangesnow;
ros::Publisher pubranges;



void ranges_Cb(const sensor_msgs::LaserScan::ConstPtr& rangesCb)
{
    rangesnow = *rangesCb;
}


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


geometry_msgs::Twist cmd_Vel(sensor_msgs::LaserScan rangesnowCmd_vel, geometry_msgs::Twist velfun)
{
    float rleft, rright, rfront;
    int schl = 0, schr = 0, schf = 0;
    for(int i = 0; i < rangesnowCmd_vel.ranges.size(); i++)
    {
        float r = i * rangesnowCmd_vel.angle_increment;
        ROS_INFO("for_ang = %f", r);
        if (M_PI/9 <= r && r <= M_PI/3) // 20 <= r <= 60
        {
            rleft += rangesnowCmd_vel.ranges.at(i);
            schl++;
        }
        if (5 * (M_PI/3) <= r && r <= 17 * (M_PI/9)) // 300 <= r <= 340
        {
            rright += rangesnowCmd_vel.ranges.at(i);
            schr++;
        }
 //       rfront = rangesnowCmd_vel.ranges.at(0);
        if (0 <= r && r <= M_PI/36 || 71 * (M_PI/36) <= r && r <= 2 * M_PI) // 0 <= r <= 5 or 355 <= r <= 360
        {
            rfront += rangesnowCmd_vel.ranges.at(i);
            schf++; 
        }       
    } 
 //   ROS_INFO("sum ranges= %f,\n schetchik l = %f,\n schetchik f = %f ", rleft, schl, schf);
    ROS_INFO("ang = %f", rangesnowCmd_vel.angle_increment);
    rleft = rleft / schl;
    rright = rright / schr; 
    rfront = rfront / schf;
    velfun.linear.x = rfront - 0.6;
    velfun.angular.z = (1/rright - 1/rleft) * 0.35;
    return velfun;
}


int main(int argc, char **argv)
{

    ros::init(argc, argv, "turtle3");
    ros::NodeHandle n;
    pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
    pubranges = n.advertise<sensor_msgs::LaserScan>("/scan_limited", 1000);
    ros::Subscriber sub = n.subscribe("/scan", 1000, ranges_Cb);
    ros::Rate loop_rate(10);


    while (ros::ok())
    {
    geometry_msgs::Twist vel;
    pub.publish(cmd_Vel(ranges(rangesnow), vel));
    ros::spinOnce();
    loop_rate.sleep();
    }
}



/*
void cmd_Vel(sensor_msgs::LaserScan rangesnowCmd_vel, geometry_msgs::Twist velfun)
{
    velfun.linear.x = 0.1;
    for(int i = 0; i < rangesnowCmd_vel.ranges.size(); i++)
    {
        if (rangesnowCmd_vel.ranges.at(i) < 0.35)
        {
            float r = i * rangesnowCmd_vel.angle_increment;
            if (r == 2 * M_PI && r == 0) // 0
            {
                velfun.linear.x = -0.2;
                velfun.angular.z = 0;
                break;
            } 
            if (r < M_PI/18 && r >= (5/36) * M_PI) // от 5 включая до 10 не включая 
            {
                velfun.linear.x = -0.1;
                velfun.angular.z = 0.3;
                break;
            } 
            if (r < (71/36) * M_PI && r >= (35/18)*M_PI) // от 350 включая до 355  не включая
            {
                velfun.linear.x = -0.1;
                velfun.angular.z = -0.3;
                break;
            }            
            if (r <= M_PI/9 && r >= M_PI/18) // от 10 включая до 20 включая
            {
                velfun.linear.x = 0.1;
                velfun.angular.z = -0.25;
                break;
            }   
             if (r >= 5 * M_PI/36 && r <= 2 * M_PI/9) // от 25 включая до 40 включая
            {
                velfun.linear.x = 0.1;
                velfun.angular.z = -0.15;
                break;
            } 
              if (r >= M_PI/4 && r <= M_PI/3) // от 45 включая до 60 включая
            {
                velfun.linear.x = 0.1;
                velfun.angular.z = -0.1;
                break;
            }            
             if (r >= (5/3) * M_PI && r <= 7 * M_PI/4) // от 300 включая до 315 включая
            {
                velfun.linear.x = 0.1;
                velfun.angular.z = 0.1;
                break;
            }     
            if (r >= (16/9) * M_PI && r <= (67/36) * M_PI) // от 320 включая до 335 включая
            {
                velfun.linear.x = 0.1;
                velfun.angular.z = 0.15; 
                break;
            }
           if (r >= (17/9) * M_PI && r < (35/18)*M_PI) // от 340 включая до 350 не включая
            {
                velfun.linear.x = 0.1;
                velfun.angular.z = 0.25; 
                break;
            }
            if (r >= (7/9)*M_PI && r <= M_PI) // от 140 включая до 180 включая
            {
                velfun.linear.x = 0.1;
                velfun.angular.z = -0.2;  
                break;                     
            }
            if (r > M_PI && r <= (11/9)*M_PI) // от 180 не включая до 220 включая
            {
                velfun.linear.x = 0.1;
                velfun.angular.z = 0.2;   
                break;                    
            }           
        }
    }
    pub.publish(velfun);
}*/