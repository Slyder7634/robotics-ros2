#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int64.hpp>
#include <thread>
#include <atomic>
#include <iostream>

using namespace std::chrono_literals;

class InputNum : public rclcpp::Node
{
public:
  InputNum() : Node("input_num"), running_(true)
  {
    pub_ = this->create_publisher<std_msgs::msg::Int64>("input_number", 10);

    // Thread reads from stdin (non-blocking for rclcpp spin)
    reader_thread_ = std::thread([this]() {
      while (rclcpp::ok() && running_) {
        int64_t n;
        if (!(std::cin >> n)) {
          // end-of-input or invalid -> stop
          running_ = false;
          break;
        }
        publishNumber(n);
      }
    });
  }

  ~InputNum()
  {
    running_ = false;
    if (reader_thread_.joinable()) 
      reader_thread_.join();
  }

private:
  rclcpp::Publisher<std_msgs::msg::Int64>::SharedPtr pub_;
  std::thread reader_thread_;
  std::atomic<bool> running_;

  void publishNumber(int64_t n)
  {
    auto msg = std_msgs::msg::Int64();
    msg.data = n;
    pub_->publish(msg);
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<InputNum>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}