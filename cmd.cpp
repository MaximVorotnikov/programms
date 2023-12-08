#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "sensor_msgs/msg/point_cloud.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include <cmath>
#include "std_msgs/msg/string.hpp"
#include <string>
#include <functional>
#include <iostream>
#include <vector>



using namespace std::chrono_literals;
using std::placeholders::_1;

float minimal_element(std::vector<float> input_vector) // Finding the minimum number in an array
{
        float minr = input_vector[0];
        for(int i = 0; i < input_vector.size(); i++)
        {
            //RCLCPP_INFO(this->get_logger(), "range... = %f\n", input_vector[i]);
            if (input_vector[i] < minr)
            minr = input_vector[i];
        }

        return minr;
}


class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("cmd")
    {
      int default_id = 1;
      this->declare_parameter("id", default_id);
      int id = 1;
      this->get_parameter_or("id", id, default_id);
      //subscriber_pose = this->create_subscription<geometry_msgs::msg::PoseStamped>("/robot" + std::to_string(id) + "/pose", 1, std::bind(&MinimalPublisher::pose_Cb, this, _1));
      subscriber_scan = this->create_subscription<sensor_msgs::msg::LaserScan>("/robot" + std::to_string(id) + "/scan", 1, std::bind(&MinimalPublisher::ranges_Cb, this, _1));
      subscriber_pose = this->create_subscription<geometry_msgs::msg::PoseStamped>("/robot" + std::to_string(id) + "/pose", 1, std::bind(&MinimalPublisher::pose_Cb, this, _1));
      publisher_ = this->create_publisher<geometry_msgs::msg::TwistStamped>("/robot" + std::to_string(id) + "/cmd_vel", 1);
      timer_ = this->create_wall_timer(1000ms, std::bind(&MinimalPublisher::pub_cur_vel, this));
    }

  private:
    void pose_Cb(const geometry_msgs::msg::PoseStamped::SharedPtr poseCb)
    {
        posenow = *poseCb;      
    }


    void ranges_Cb(const sensor_msgs::msg::LaserScan::SharedPtr rangesCb)
    {
        auto rangesnow = sensor_msgs::msg::LaserScan();
        rangesnow = *rangesCb;
        std::vector<float> front_distance;
        std::vector<float> rear_distance;

        rangesnow.header.stamp = rclcpp::Clock().now();
        rangesnow.header.frame_id = "laser";
        for(int i = 0; i < rangesnow.ranges.size(); i++)
        {
            if (rangesnow.ranges.at(i) > 2.5 || std::isinf(rangesnow.ranges.at(i)))
            rangesnow.ranges.at(i) = 2.5;
            //RCLCPP_INFO(this->get_logger(), "rangesnow = %f\n", rangesnow.ranges.at(i));
        }

        float rleft = 0.0; // Расстояние слева
        float rright = 0.0; // Расстояние справа
        float rfront = 0.0; // Расстояние спереди
        int schl = 0, schr = 0, schf = 0; // счетчики
        for(int i = 0; i < rangesnow.ranges.size(); i++)
        {
            float angle_rad = rangesnow.angle_min + i * rangesnow.angle_increment;
            // float angle_rad = i * rangesnow.angle_increment;
            // if (angle_rad > M_PI)
            // {
            //     angle_rad = M_PI - angle_rad;
            // }
            //RCLCPP_INFO(this->get_logger(), "for_lens = %f\n", rangesnow.ranges.at(i));
            //RCLCPP_INFO(this->get_logger(), "for_ang = %f\n", rangesnow.angle_increment);
            bool angle_rad_is_less_than_60_deg_and_bigger_than_20 = (M_PI/180) * 20 <= angle_rad && angle_rad <= (M_PI/180) * 60;
            bool angle_rad_is_less_than_minus20_deg_and_bigger_than_minus60 = (M_PI/180) * (-60) <= angle_rad && angle_rad <= (M_PI/180) * (-20);
            if (angle_rad_is_less_than_60_deg_and_bigger_than_20) // 20 <= angle_rad <= 60
            {
                rleft += rangesnow.ranges.at(i);
                schl++;
            }
            if (angle_rad_is_less_than_minus20_deg_and_bigger_than_minus60) // -60 <= angle_rad <= -20
            {
                rright += rangesnow.ranges.at(i);
                schr++;
            }
    //       rfront = rangesnowCmd_vel.ranges.at(0);
            if ((M_PI/180) * (-10) <= angle_rad && angle_rad <= (M_PI/180) * 10) // -10 <= angle_rad <= 10
            {
                front_distance.push_back(rangesnow.ranges.at(i));
                //rfront = rangesfront.range_min;
                //RCLCPP_INFO(this->get_logger(), "rfront... = %f\n", rangesfront.ranges.at(i));

            } 
            if ((M_PI/180) * (-180) <= angle_rad && angle_rad <= (M_PI/180) * (-170) || (M_PI/180) * 170 <= angle_rad && angle_rad <= (M_PI/180) * 180) // -180 <= angle_rad <= -170 or 170 <= angle_rad <= 180
            {
                rear_distance.push_back(rangesnow.ranges.at(i));
                //rfront = rangesfront.range_min;
                //RCLCPP_INFO(this->get_logger(), "rfront... = %f\n", rangesfront.ranges.at(i));

            }        
        } 
        RCLCPP_INFO(this->get_logger(), "minfront.... = %f\n", front_distance[0]);
        RCLCPP_INFO(this->get_logger(), "minrear.... = %f\n", rear_distance[0]);
        float min_front = minimal_element(front_distance);
        float min_rear = minimal_element(rear_distance);
        rleft = rleft / schl;
        rright = rright / schr; 
        //rfront = rfront / schf;
        geometry_msgs::msg::TwistStamped vel = geometry_msgs::msg::TwistStamped();
        //RCLCPP_INFO(this->get_logger(), "rfront = %f\n schf = %i\n", rfront, schf);
        if (min_rear < 0.4)
        vel.twist.linear.x = -(min_rear - 1);
        else
        vel.twist.linear.x = min_front - 1;


        if (vel.twist.linear.x < 0.3 && vel.twist.linear.x > 0)
        {
        vel.twist.linear.x = 0.3;
        }
        else if (vel.twist.linear.x > -0.3 && vel.twist.linear.x <= 0)
        {
        vel.twist.linear.x = -0.3;
        }
        vel.twist.angular.z = (1/rright - 1/rleft) * 0.9;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        RCLCPP_INFO(this->get_logger(), "\n x = %f \ny = %f\n", posenow.pose.position.x, posenow.pose.position.y);
        int a = 0;
        RCLCPP_INFO(this->get_logger(), "\n i = %i\n", i);
        if (!i)
        {
            poseRemember.pose.position.x = posenow.pose.position.x;
            poseRemember.pose.position.y = posenow.pose.position.y;
            i++;
            RCLCPP_INFO(this->get_logger(), "\n REMEMBERx = %f \nREMEMBERy = %f\n", poseRemember.pose.position.x, poseRemember.pose.position.y);
            return;
        }

        if (i)
        {
            RCLCPP_INFO(this->get_logger(), "\n posenowX = %f \nposeRememberX = %f\n",posenow.pose.position.x , poseRemember.pose.position.x);
            float cx = abs(poseRemember.pose.position.x - posenow.pose.position.x);
            float cy = abs(poseRemember.pose.position.y - posenow.pose.position.y);
            RCLCPP_INFO(this->get_logger(), "\n cx = %f \ncy = %f\n", cx, cy);

            if (abs(poseRemember.pose.position.x - posenow.pose.position.x) < 0.00000001 && abs(poseRemember.pose.position.y - posenow.pose.position.y) < 0.00000001)
            {
                i++;
                a = i;
                RCLCPP_INFO(this->get_logger(), "\n //////////////////////////////////////////////\n");
            }
            else
            {
                RCLCPP_INFO(this->get_logger(), "\n a = %i \n", a);
                i = 0;
                return;
            }
        }
        if (a % 2 == 0 && a != 0)
        {
            vel.twist.linear.x = -2;
        }
        if (a % 3 == 0 && a != 0)
        {
            vel.twist.linear.x = 2;
        }
        if (a % 4 == 0 && a != 0)
        {
            vel.twist.angular.z = 1.5;
        }
        if (a % 5 == 0 && a != 0)
        {
            vel.twist.angular.z = -1.5;
        }




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        curr_vel = vel;
    }


    void pub_cur_vel()
    {
        this->publisher_->publish(this->curr_vel);
    }

 
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr publisher_;
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr subscriber_pose;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscriber_scan;
    geometry_msgs::msg::PoseStamped poseRemember = geometry_msgs::msg::PoseStamped();
    geometry_msgs::msg::TwistStamped curr_vel;
    geometry_msgs::msg::PoseStamped posenow = geometry_msgs::msg::PoseStamped();
    int i = 0;
};//конец класса


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}


