#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int64.hpp>

using namespace std::placeholders;

class OutputNode : public rclcpp::Node
{
public:
  OutputNode() : Node("output_node")
  {
    sub_ = this->create_subscription<std_msgs::msg::Int64>(
      "fact_result", 10,
      std::bind(&OutputNode::callback, this, _1));
    RCLCPP_INFO(this->get_logger(), "Output node started");
  }

private:
  rclcpp::Subscription<std_msgs::msg::Int64>::SharedPtr sub_;
  
  void callback(const std_msgs::msg::Int64::SharedPtr msg){
    RCLCPP_INFO(this->get_logger(), "Received factorial result: %ld", msg->data);
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<OutputNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}