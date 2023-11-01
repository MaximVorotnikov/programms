#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/LaserScan.hpp"
#include "sensor_msgs/PointCloud.hpp"
#include "math.hpp"

using namespace std::chrono_literals;
using std::placeholders::_1;

class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("xy")
    {
      subscriber_ = this->create_subscription<sensor_msgs::LaserScan>("/scan", 1, std::bind(&MinimalSubscriber::ranges_Cb, this, _1));
      publisher_ = this->create_publisher<sensor_msgs::PointCloud>("/point_xy", 1);
    //  timer_ = this->create_wall_timer(500ms, std::bind(&MinimalPublisher::publish_message, this));
    }

  private:
//  void subscribe_message(const geometry_msgs::msg::Twist::SharedPtr message) const
//    {
//      RCLCPP_INFO(this->get_logger(), "Recieved - Linear Velocity : '%f', Angular Velocity : '%f'", message->linear.x, message->angular.z);
//   }
    
    void ranges_Cb(const sensor_msgs::LaserScan::SharedPtr& rangesCb) const
    {
    auto rangesnow = sensor_msgs::LaserScan();
    rangesnow = *rangesCb;
////////////

    rangesnow.header.stamp = ros::Time::now();
    rangesnow.header.frame_id = "laser";
    for(int i = 0; i < rangesnow.ranges.size(); i++)
    {
        if (rangesnow.ranges.at(i) > 1 || rangesnow.ranges.at(i) == INFINITY)
        rangesnow.ranges.at(i) = 1;
    }
    auto point = sensor_msgs::PointCloud();
    point.header.stamp = ros::Time::now();
    point.header.frame_id = "laser";
    for(int i = 0; i < rangesnow.ranges.size(); i++)
    {
        geometry_msgs::Point32 p;
        float x, y;
        p.x = rangesnow.ranges.at(i) * cos(i * rangesnow.angle_increment);
        p.y = rangesnow.ranges.at(i) * sin(i * rangesnow.angle_increment);
        point.points.push_back(p);

        RCLCPP_INFO(this->get_logger(), "\n x = %f \ny = %f\n", point.points.at(i).x, point.points.at(i).y);
    }
    publisher_->publish(point);
///////////
    }
    //0rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<sensor_msgs::PointCloud>::SharedPtr publisher_;
    rclcpp::Subscription<sensor_msgs::LaserScan>::SharedPtr subscriber_;
};//конец класса

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin_some(std::make_shared<MinimalPublisher>());
  //rclcpp::shutdown();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using std::placeholders::_1;

class MinimalSubscriber : public rclcpp::Node
{
  public:
    MinimalSubscriber()
    : Node("cpp_topic_subscriber_spiral")
    {
      subscriber_ = this->create_subscription<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 1, std::bind(&MinimalSubscriber::subscribe_message, this, _1));
    }

  private:
    void subscribe_message(const geometry_msgs::msg::Twist::SharedPtr message) const
    {
      RCLCPP_INFO(this->get_logger(), "Recieved - Linear Velocity : '%f', Angular Velocity : '%f'", message->linear.x, message->angular.z);
    }
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscriber_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalSubscriber>());
  rclcpp::shutdown();
  return 0;
}