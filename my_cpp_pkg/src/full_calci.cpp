#include <memory>
#include <rclcpp/rclcpp.hpp>
#include "my_robot_interfaces/srv/compute_two_ints.hpp"

using ComputeTwoInts = my_robot_interfaces::srv::ComputeTwoInts;
using std::placeholders::_1;
using std::placeholders::_2;

class ComputeTwoIntsServer : public rclcpp::Node
{
public:
  ComputeTwoIntsServer()
  : Node("full_calci")
  {
    service_ = this->create_service<ComputeTwoInts>(
      "compute_two_ints",
      std::bind(&ComputeTwoIntsServer::handle_compute, this, _1, _2));

    RCLCPP_INFO(this->get_logger(), "ComputeTwoInts service is ready.");
  }

private:
  void handle_compute(
    const std::shared_ptr<ComputeTwoInts::Request> request,
    const std::shared_ptr<ComputeTwoInts::Response> response)
  {
    if (request->operation == "add") {
      response->result = request->a + request->b;
    } else if ( request->operation == "sub"){
        response->result = request->a - request->b;
    } else if ( request->operation == "mul"){
        response->result = request->a * request->b;
    } else if ( request->operation == "div"){
        response->result = request->a / request->b;
    }
    else {
      response->result = 0;
    }

    RCLCPP_INFO(
      this->get_logger(),
      "Received a=%ld, b=%ld, operation='%s' -> result=%ld",
      request->a,
      request->b,
      request->operation.c_str(),
      response->result);
  }

  rclcpp::Service<ComputeTwoInts>::SharedPtr service_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ComputeTwoIntsServer>());
  rclcpp::shutdown();
  return 0;
}
