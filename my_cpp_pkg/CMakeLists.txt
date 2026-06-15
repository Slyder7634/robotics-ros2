cmake_minimum_required(VERSION 3.8)
project(my_cpp_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(example_interfaces REQUIRED)

add_executable(input_num src/input_num.cpp)
ament_target_dependencies(input_num rclcpp std_msgs)

add_executable(factorial_node src/factorial.cpp)
ament_target_dependencies(factorial_node rclcpp std_msgs)

add_executable(output_node src/output_num.cpp)
ament_target_dependencies(output_node rclcpp std_msgs)

add_executable(add_two_ints_server src/add_two_ints_server.cpp)
ament_target_dependencies(add_two_ints_server rclcpp example_interfaces)

add_executable(add_two_ints_client src/add_two_ints_client.cpp)
ament_target_dependencies(add_two_ints_client rclcpp example_interfaces)

add_executable(calci_add_node src/calci_add_node.cpp)
ament_target_dependencies(calci_add_node rclcpp example_interfaces)

add_executable(calci_multiply_node src/calci_multiply_node.cpp)
ament_target_dependencies(calci_multiply_node rclcpp example_interfaces)

add_executable(calci_divide_node src/calci_divide_node.cpp)
ament_target_dependencies(calci_divide_node rclcpp example_interfaces)

add_executable(calci_subtract_node src/calci_subtract_node.cpp)
ament_target_dependencies(calci_subtract_node rclcpp example_interfaces)

install(TARGETS
  input_num
  factorial_node
  output_node
  add_two_ints_server
  add_two_ints_client
  calci_add_node
  calci_multiply_node
  calci_divide_node
  calci_subtract_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
