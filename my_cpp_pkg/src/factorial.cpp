#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int64.hpp>

using namespace std::chrono_literals;

class FactorialNode : public rclcpp::Node
{
public:
  FactorialNode()
  : Node("factorial_node")
  {
    sub_ = this->create_subscription<std_msgs::msg::Int64>(
      "input_number", 10,
      std::bind(&FactorialNode::handle_input, this, std::placeholders::_1));
    pub_ = this->create_publisher<std_msgs::msg::Int64>("fact_result", 10);
    RCLCPP_INFO(this->get_logger(), "Factorial node started");
  }

private:
  void handle_input(const std_msgs::msg::Int64::SharedPtr msg)
  {
    int64_t n = msg->data;
    if (n < 0) {
      RCLCPP_WARN(this->get_logger(), "Negative input %ld, publishing 0", n);
      publish_result(0);
      return;
    }

    if (n == 1 || n == 0) {
      publish_result(0);
      return;
    }
    // compute factorial with uint64_t; clamp large inputs to avoid overflow
    uint64_t result = 1;
    for (int64_t i = 1; i <= n && i <= 20; ++i) {
      result *= static_cast<uint64_t>(i);
    }
    if (n > 20) {
      RCLCPP_WARN(this->get_logger(), "Input %ld too large; result may be clamped/overflowed", n);
    }
    publish_result(static_cast<int64_t>(result));
  }

  void publish_result(int64_t val)
  {
    auto out = std_msgs::msg::Int64();
    out.data = val;
    pub_->publish(out);
  }

  rclcpp::Subscription<std_msgs::msg::Int64>::SharedPtr sub_;
  rclcpp::Publisher<std_msgs::msg::Int64>::SharedPtr pub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<FactorialNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}