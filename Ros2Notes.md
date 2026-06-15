# ROS2 Complete Notes

## Table of Contents
1. [ROS2 Basics](#ros2-basics)
2. [Workspace Setup](#workspace-setup)
3. [Creating Packages](#creating-packages)
4. [Creating Nodes](#creating-nodes)
5. [Topics (Pub/Sub)](#topics)
6. [Services (Request/Response)](#services)
7. [Parameters](#parameters)
8. [Custom Messages & Services](#custom-messages--services)
9. [Pipeline Architecture](#pipeline-architecture)
10. [User Input Pattern](#user-input-pattern)
11. [Important Terminal Commands](#important-terminal-commands)

---

## ROS2 Basics

### What is ROS2?
ROS2 (Robot Operating System 2) is a middleware platform for building distributed robotic applications. It allows different nodes (programs) to communicate with each other.

### Why ROS2?
- **Modularity**: Break complex robot behavior into small, independent nodes
- **Reusability**: Write once, use across multiple projects
- **Real-time**: Better performance than ROS1
- **Communication**: Nodes can run on different computers and communicate over network
- **Easy testing**: Test individual nodes in isolation

### When to Use?
- When building multi-component robotic systems
- When you need inter-process communication
- When you want flexible, scalable architecture
- When working with multiple programming languages together

### How does it work?
- Nodes (executables) publish/subscribe to topics or call services
- Middleware (DDS) handles all communication
- Graph of nodes creates distributed system

---

## Workspace Setup

### Directory Structure
```
ros2_ws/                  # Workspace (can be any name)
├── src/                  # Source code directory
│   ├── my_cpp_pkg/      # C++ package
│   │   ├── CMakeLists.txt
│   │   ├── package.xml
│   │   ├── include/
│   │   └── src/
│   └── my_py_pkg/       # Python package
│       ├── setup.py
│       ├── setup.cfg
│       ├── package.xml
│       └── my_py_pkg/
├── build/               # Compiled binaries (auto-generated)
├── install/             # Installed files (auto-generated)
└── log/                 # Build logs (auto-generated)
```

### Building Workspace
```bash
# Build everything
colcon build

# Build specific package
colcon build --packages-select my_cpp_pkg

# Build with symlink (Python) - changes take effect immediately
colcon build --symlink-install
```

### Sourcing Setup
```bash
# Source the workspace (run from ros2_ws directory)
source install/setup.bash

# Check if sourced correctly
echo $ROS_PACKAGE_PATH
```

---

## Creating Packages

### What is a Package?
A package is a directory containing code, configuration files, and metadata for a ROS2 application.

### Python Package

**Create:**
```bash
ros2 pkg create --build-type ament_python my_py_pkg
```

**Structure:**
- `setup.py` - Python build configuration
- `setup.cfg` - Setup configuration
- `package.xml` - Package metadata
- `my_py_pkg/` - Actual Python code directory

**package.xml Example:**
```xml
<?xml version="1.0"?>
<package format="3">
  <name>my_py_pkg</name>
  <version>0.0.0</version>
  <description>My Python ROS2 package</description>
  <maintainer email="user@example.com">Your Name</maintainer>
  <license>Apache License 2.0</license>
  
  <depend>rclpy</depend>
  <depend>example_interfaces</depend>
</package>
```

**setup.py Example:**
```python
from setuptools import setup

package_name = 'my_py_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_cmake_core/cmake/package_templates',
         ['package_templates/package.cmake.in']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    author='Your Name',
    author_email='your_email@example.com',
    description='My Python package',
    license='Apache License 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'my_node = my_py_pkg.my_first_node:main',
        ],
    },
)
```

### C++ Package

**Create:**
```bash
ros2 pkg create --build-type ament_cmake my_cpp_pkg --dependencies rclcpp example_interfaces
```

**Structure:**
- `CMakeLists.txt` - Build configuration
- `package.xml` - Package metadata
- `include/my_cpp_pkg/` - Header files
- `src/` - C++ source files

**CMakeLists.txt Example:**
```cmake
cmake_minimum_required(VERSION 3.5)
project(my_cpp_pkg)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(example_interfaces REQUIRED)

add_executable(my_node src/my_first_node.cpp)
ament_target_dependencies(my_node rclcpp example_interfaces)

install(TARGETS
  my_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
```

---

## Creating Nodes

### What is a Node?
A node is an executable program that performs a specific task (e.g., read sensor, control motor, process data).

### Python Node Template

```python
#!/usr/bin/env python3

import rclpy
from rclpy.node import Node

class MyNode(Node):
    def __init__(self):
        super().__init__("node_name")        # Node name
        self.get_logger().info("Node started")
    
    def destroy_node(self):
        super().destroy_node()

def main(args=None):
    rclpy.init(args=args)                   # Initialize ROS2
    node = MyNode()
    rclpy.spin(node)                         # Keep node running
    rclpy.shutdown()                         # Cleanup on exit

if __name__ == "__main__":
    main()
```

### C++ Node Template

```cpp
#include <rclcpp/rclcpp.hpp>

class MyNode : public rclcpp::Node
{
public:
    MyNode() : Node("node_name")             // Node name
    {
        RCLCPP_INFO(this->get_logger(), "Node started");
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);               // Initialize ROS2
    auto node = std::make_shared<MyNode>();
    rclcpp::spin(node);                     // Keep node running
    rclcpp::shutdown();                     // Cleanup on exit
    return 0;
}
```

---

## Topics

### What is a Topic?
Topics are named channels for one-way message passing. Many nodes can publish to a topic, and many can subscribe to it. It's **asynchronous** (fire-and-forget).

### Why Topics?
- **One-to-many**: One publisher, multiple subscribers
- **Anonymous**: Publishers and subscribers don't know each other
- **Asynchronous**: No waiting for response
- **Real-time data**: Sensor readings, status updates

### When to Use Topics?
- Sensor data (camera, lidar, IMU)
- Status updates (battery level, state)
- Continuous data streams
- Broadcasting information

### How Topics Work?
1. Publisher creates a topic and sends messages
2. Subscribers listen to the topic
3. Messages are passed through middleware (DDS)
4. Messages can be queued (history size = 10)

### Python Publisher

```python
from example_interfaces.msg import String

class RobotNewsNode(Node):
    def __init__(self):
        super().__init__("robot_news_station")
        
        # Create publisher
        self.publisher = self.create_publisher(
            String,                    # Message type
            "robot_news",              # Topic name
            10                         # Queue size (history)
        )
        
        # Create timer to publish periodically
        self.timer = self.create_timer(
            0.5,                       # Period in seconds
            self.publish_news          # Callback function
        )
    
    def publish_news(self):
        msg = String()
        msg.data = "Hello from ROS2"
        self.publisher.publish(msg)
        self.get_logger().info(f"Published: {msg.data}")
```

### Python Subscriber

```python
class SmartphoneNode(Node):
    def __init__(self):
        super().__init__("smartphone")
        
        # Create subscription
        self.subscription = self.create_subscription(
            String,                    # Message type
            "robot_news",              # Topic name
            self.listener_callback,    # Callback function
            10                         # Queue size
        )
    
    def listener_callback(self, msg):
        self.get_logger().info(f"Received: {msg.data}")
```

### C++ Publisher

```cpp
#include "example_interfaces/msg/string.hpp"

class RobotNewsNode : public rclcpp::Node
{
public:
    RobotNewsNode() : Node("robot_news_node")
    {
        publisher_ = this->create_publisher<
            example_interfaces::msg::String
        >("robot_news", 10);
        
        timer_ = this->create_wall_timer(
            500ms,
            std::bind(&RobotNewsNode::publishNews, this)
        );
    }

private:
    void publishNews()
    {
        auto msg = example_interfaces::msg::String();
        msg.data = "Hello from ROS2";
        publisher_->publish(msg);
    }
    
    rclcpp::Publisher<
        example_interfaces::msg::String
    >::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};
```

### C++ Subscriber

```cpp
class SmartphoneNode : public rclcpp::Node
{
public:
    SmartphoneNode() : Node("smartphone")
    {
        subscription_ = this->create_subscription<
            example_interfaces::msg::String
        >(
            "robot_news",
            10,
            std::bind(&SmartphoneNode::topic_callback, 
                      this, std::placeholders::_1)
        );
    }

private:
    void topic_callback(const 
        example_interfaces::msg::String & msg) const
    {
        RCLCPP_INFO(this->get_logger(), 
                    "Heard: '%s'", msg.data.c_str());
    }
    
    rclcpp::Subscription<
        example_interfaces::msg::String
    >::SharedPtr subscription_;
};
```

---

## Services

### What is a Service?
Services are **synchronous** request-response mechanisms. A client sends a request and waits for a response. One server, multiple clients.

### Why Services?
- **Request-Response**: Wait for answer before proceeding
- **One-to-many**: One server, many clients
- **Deterministic**: Need guaranteed response
- **Action-based**: Perform specific action and return result

### When to Use Services?
- Adding two numbers
- Starting/stopping tasks
- Enabling/disabling features
- Getting specific information on demand
- **NOT for continuous data** (use topics instead)

### How Services Work?
1. Server advertises a service with a specific name
2. Client sends a request and waits
3. Server processes and sends response
4. Client receives response and continues

### Define Custom Service

**File: `my_interfaces/srv/AddTwoInts.srv`**
```
int64 a
int64 b
---
int64 sum
```
- Lines before `---` are **request** fields
- Lines after `---` are **response** fields

**Or use built-in:** `example_interfaces/srv/AddTwoInts`

### Python Service Server

```python
from example_interfaces.srv import AddTwoInts

class AddTwoIntsServerNode(Node):
    def __init__(self):
        super().__init__("add_two_ints_server")
        
        # Create service
        self.server = self.create_service(
            AddTwoInts,                # Service type
            "add_two_ints",            # Service name
            self.callback_add_two_ints # Callback function
        )
        self.get_logger().info("AddTwoInts service ready")
    
    def callback_add_two_ints(self, 
                              request: AddTwoInts.Request, 
                              response: AddTwoInts.Response):
        self.get_logger().info(
            f"Request: a={request.a}, b={request.b}"
        )
        response.sum = request.a + request.b
        return response
```

### Python Service Client

```python
from example_interfaces.srv import AddTwoInts

class AddTwoIntsClientNode(Node):
    def __init__(self):
        super().__init__("add_two_ints_client")
        
        # Create client
        self.client = self.create_client(
            AddTwoInts,
            "add_two_ints"
        )
        
        # Wait for service to be available
        while not self.client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info("Waiting for service...")
        
        # Send request (one-time)
        self.send_request()
    
    def send_request(self):
        request = AddTwoInts.Request()
        request.a = 5
        request.b = 3
        
        # Send and wait for response
        future = self.client.call_async(request)
        future.add_done_callback(self.response_callback)
    
    def response_callback(self, future):
        response = future.result()
        self.get_logger().info(
            f"Response: sum = {response.sum}"
        )
```

### C++ Service Server

```cpp
#include "example_interfaces/srv/add_two_ints.hpp"

class AddTwoIntsServerNode : public rclcpp::Node
{
public:
    AddTwoIntsServerNode() : Node("add_two_ints_server")
    {
        server_ = this->create_service<
            example_interfaces::srv::AddTwoInts
        >(
            "add_two_ints",
            std::bind(&AddTwoIntsServerNode::callback,
                      this,
                      std::placeholders::_1,
                      std::placeholders::_2)
        );
        RCLCPP_INFO(this->get_logger(), "Service ready");
    }

private:
    void callback(const std::shared_ptr<
                      example_interfaces::srv::AddTwoInts::Request> request,
                  std::shared_ptr<
                      example_interfaces::srv::AddTwoInts::Response> response)
    {
        response->sum = request->a + request->b;
        RCLCPP_INFO(this->get_logger(),
                    "Sum: %ld", response->sum);
    }
    
    rclcpp::Service<
        example_interfaces::srv::AddTwoInts
    >::SharedPtr server_;
};
```

### C++ Service Client

```cpp
#include "example_interfaces/srv/add_two_ints.hpp"

class AddTwoIntsClientNode : public rclcpp::Node
{
public:
    AddTwoIntsClientNode() : Node("add_two_ints_client")
    {
        client_ = this->create_client<
            example_interfaces::srv::AddTwoInts
        >("add_two_ints");
        
        while (!client_->wait_for_service(
            std::chrono::seconds(1))) {
            RCLCPP_INFO(this->get_logger(), 
                        "Waiting for service...");
        }
        
        send_request();
    }

private:
    void send_request()
    {
        auto request = std::make_shared<
            example_interfaces::srv::AddTwoInts::Request>();
        request->a = 5;
        request->b = 3;
        
        auto future = client_->async_send_request(request,
            std::bind(&AddTwoIntsClientNode::response_callback,
                      this,
                      std::placeholders::_1));
    }
    
    void response_callback(rclcpp::Client<
        example_interfaces::srv::AddTwoInts>::SharedFuture future)
    {
        auto response = future.get();
        RCLCPP_INFO(this->get_logger(),
                    "Sum: %ld", response->sum);
    }
    
    rclcpp::Client<
        example_interfaces::srv::AddTwoInts
    >::SharedPtr client_;
};
```

---

## Parameters

### What are Parameters?
Parameters are node configuration values that can be set and modified at runtime without restarting the node. Each node can have its own parameters.

### Why Parameters?
- **Reconfiguration**: Change node behavior without restarting
- **Flexibility**: Different settings for different environments
- **Testing**: Quickly test different configurations
- **Shared settings**: Access from command line or other nodes

### When to Use Parameters?
- Robot speed (max velocity, acceleration)
- Sensor calibration values
- PID controller gains
- Debug flags or logging levels
- Timeout values
- File paths or configurations

### How Parameters Work?
1. Node declares parameter with name, type, and default value
2. Parameter is initialized to default value
3. Can be overridden via command line during startup
4. Can be read at runtime using `get_parameter()`
5. Can be modified at runtime (if parameter callbacks are set up)

### Python Parameters

**Declare and Get Parameter:**
```python
from rclpy.parameter import Parameter
from rcl_interfaces.msg import ParameterType

class ConfigurableNode(Node):
    def __init__(self):
        super().__init__("configurable_node")
        
        # Declare parameter with default value
        self.declare_parameter(
            'max_speed',           # Parameter name
            50.0                   # Default value
        )
        
        # Get parameter value
        max_speed = self.get_parameter(
            'max_speed'
        ).get_parameter_value().double_value
        
        self.get_logger().info(
            f"Max speed set to: {max_speed}"
        )

def main(args=None):
    rclpy.init(args=args)
    node = ConfigurableNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()
```

**Run with Custom Parameter:**
```bash
# Override parameter on command line
ros2 run my_py_pkg configurable_node --ros-args -p max_speed:=100.0

# Set multiple parameters
ros2 run my_py_pkg configurable_node --ros-args -p max_speed:=100.0 -p debug:=true
```

### C++ Parameters

**Declare and Get Parameter:**
```cpp
#include <rclcpp/rclcpp.hpp>

class ConfigurableNode : public rclcpp::Node
{
public:
    ConfigurableNode() : Node("configurable_node")
    {
        // Declare parameter with default value
        this->declare_parameter<double>("max_speed", 50.0);
        
        // Get parameter value
        double max_speed = this->get_parameter(
            "max_speed"
        ).as_double();
        
        RCLCPP_INFO(this->get_logger(),
                    "Max speed set to: %f", max_speed);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ConfigurableNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
```

**Run with Custom Parameter:**
```bash
# Override parameter on command line
ros2 run my_cpp_pkg configurable_node --ros-args -p max_speed:=100.0
```

### Parameter Types

**Python:**
```python
# String
self.declare_parameter('robot_name', 'MyRobot')

# Integer
self.declare_parameter('num_sensors', 5)

# Double/Float
self.declare_parameter('speed', 1.5)

# Boolean
self.declare_parameter('debug_mode', False)

# List/Array
self.declare_parameter('goal_position', [1.0, 2.0, 3.0])
```

**C++:**
```cpp
// String
this->declare_parameter<std::string>("robot_name", "MyRobot");

// Integer
this->declare_parameter<int>("num_sensors", 5);

// Double
this->declare_parameter<double>("speed", 1.5);

// Boolean
this->declare_parameter<bool>("debug_mode", false);

// Vector (list)
this->declare_parameter<std::vector<double>>(
    "goal_position", 
    {1.0, 2.0, 3.0}
);
```

### Parameter Callbacks (Advanced)

**Python - Watch for parameter changes:**
```python
def __init__(self):
    super().__init__("configurable_node")
    
    self.declare_parameter('max_speed', 50.0)
    
    # Add callback for parameter changes
    self.add_on_set_parameters_callback(
        self.parameters_callback
    )

def parameters_callback(self, params):
    for param in params:
        if param.name == 'max_speed':
            self.get_logger().info(
                f"Parameter changed to: {param.value}"
            )
    return SetParametersResult(successful=True)
```

### Terminal Parameter Commands

```bash
# List node parameters
ros2 param list /node_name

# Get parameter value
ros2 param get /node_name param_name

# Set parameter (while running)
ros2 param set /node_name param_name 100.0

# Describe parameters
ros2 param describe /node_name param_name

# Save parameters to file
ros2 param dump /node_name > params.yaml

# Load parameters from file
ros2 run my_pkg my_node --ros-args --params-file params.yaml
```

---

## Custom Messages & Services

### What are Custom Messages & Services?
Custom messages and services allow you to define your own data structures for communication between nodes, beyond the built-in types like `String`, `Int64`, etc.

### When to Use?
- Need multiple fields in one message
- Complex data structures
- Domain-specific data types
- Better code organization and type safety

### Create a Custom Message

**File: `my_robot_interfaces/msg/HardwareStatus.msg`**
```
string hardware_name
int32 status
float64 temperature
bool is_operational
```

**Edit `my_robot_interfaces/CMakeLists.txt`:**
```cmake
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/HardwareStatus.msg"
)
```

**Edit `my_robot_interfaces/package.xml`:**
```xml
<build_depend>rosidl_default_generators</build_depend>
<exec_depend>rosidl_default_runtime</exec_depend>
<member_of_group>rosidl_interface_packages</member_of_group>
```

**Build:**
```bash
colcon build
source install/setup.bash
```

### Create a Custom Service

**File: `my_robot_interfaces/srv/ComputeTwoInts.srv`**
```
int64 a
int64 b
string operation
---
int64 result
```

The structure is:
- **Request fields** (before `---`): `a`, `b`, `operation`
- **Response fields** (after `---`): `result`

**Edit `my_robot_interfaces/CMakeLists.txt`:**
```cmake
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/HardwareStatus.msg"
  "srv/ComputeTwoInts.srv"
)
```

**Build:**
```bash
colcon build
source install/setup.bash
```

### Use Custom Message in Python

**Publisher:**
```python
from my_robot_interfaces.msg import HardwareStatus

class HardwareNode(Node):
    def __init__(self):
        super().__init__("hardware_node")
        self.publisher = self.create_publisher(
            HardwareStatus,
            "hardware_status",
            10
        )
        self.timer = self.create_timer(1.0, self.publish_status)
    
    def publish_status(self):
        msg = HardwareStatus()
        msg.hardware_name = "Motor_1"
        msg.status = 1
        msg.temperature = 45.5
        msg.is_operational = True
        self.publisher.publish(msg)
```

**Subscriber:**
```python
from my_robot_interfaces.msg import HardwareStatus

class MonitorNode(Node):
    def __init__(self):
        super().__init__("monitor_node")
        self.subscription = self.create_subscription(
            HardwareStatus,
            "hardware_status",
            self.status_callback,
            10
        )
    
    def status_callback(self, msg):
        self.get_logger().info(
            f"Hardware: {msg.hardware_name}, "
            f"Temp: {msg.temperature}°C, "
            f"Operational: {msg.is_operational}"
        )
```

### Use Custom Service in Python

**Server:**
```python
from my_robot_interfaces.srv import ComputeTwoInts

class CalcServer(Node):
    def __init__(self):
        super().__init__("calc_server")
        self.service = self.create_service(
            ComputeTwoInts,
            "compute_two_ints",
            self.compute_callback
        )
    
    def compute_callback(self, request, response):
        if request.operation == "add":
            response.result = request.a + request.b
        elif request.operation == "sub":
            response.result = request.a - request.b
        elif request.operation == "mul":
            response.result = request.a * request.b
        elif request.operation == "div":
            response.result = request.a // request.b
        else:
            response.result = 0
        
        self.get_logger().info(
            f"{request.a} {request.operation} {request.b} = {response.result}"
        )
        return response
```

**Client:**
```python
from my_robot_interfaces.srv import ComputeTwoInts

class CalcClient(Node):
    def __init__(self):
        super().__init__("calc_client")
        self.client = self.create_client(
            ComputeTwoInts,
            "compute_two_ints"
        )
        
        while not self.client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info("Waiting for service...")
        
        self.send_request("add", 10, 5)
    
    def send_request(self, operation, a, b):
        request = ComputeTwoInts.Request()
        request.a = a
        request.b = b
        request.operation = operation
        
        future = self.client.call_async(request)
        future.add_done_callback(self.response_callback)
    
    def response_callback(self, future):
        response = future.result()
        self.get_logger().info(f"Result: {response.result}")
```

### Use Custom Service in C++

**Server:**
```cpp
#include "my_robot_interfaces/srv/compute_two_ints.hpp"
using ComputeTwoInts = my_robot_interfaces::srv::ComputeTwoInts;
using std::placeholders::_1;
using std::placeholders::_2;

class CalcServer : public rclcpp::Node
{
public:
    CalcServer() : Node("calc_server")
    {
        service_ = this->create_service<ComputeTwoInts>(
            "compute_two_ints",
            std::bind(&CalcServer::compute_callback, this, _1, _2)
        );
    }

private:
    void compute_callback(
        const std::shared_ptr<ComputeTwoInts::Request> request,
        const std::shared_ptr<ComputeTwoInts::Response> response)
    {
        if (request->operation == "add") {
            response->result = request->a + request->b;
        } else if (request->operation == "sub") {
            response->result = request->a - request->b;
        }
        
        RCLCPP_INFO(this->get_logger(),
                    "%ld %s %ld = %ld",
                    request->a,
                    request->operation.c_str(),
                    request->b,
                    response->result);
    }
    
    rclcpp::Service<ComputeTwoInts>::SharedPtr service_;
};
```

**Update CMakeLists.txt to link custom interface:**
```cmake
find_package(my_robot_interfaces REQUIRED)

add_executable(calc_server src/calc_server.cpp)
ament_target_dependencies(calc_server rclcpp my_robot_interfaces)
```

---

## Pipeline Architecture

### What is Pipeline Architecture?
A pipeline architecture connects multiple nodes in sequence, where data flows from one node to the next. Each node performs a specific task and passes results to the next node.

### Pattern:
```
Input Node → Processing Node 1 → Processing Node 2 → Output Node
```

### Example: Calculator Pipeline

**Node 1: Input Node** - Reads numbers from user input
```cpp
class InputNum : public rclcpp::Node
{
public:
    InputNum() : Node("input_num"), running_(true)
    {
        pub_ = this->create_publisher<std_msgs::msg::Int64>("input_number", 10);
        
        // Thread for non-blocking stdin
        reader_thread_ = std::thread([this]() {
            while (rclcpp::ok() && running_) {
                int64_t n;
                if (!(std::cin >> n)) {
                    running_ = false;
                    break;
                }
                publishNumber(n);
            }
        });
    }

private:
    void publishNumber(int64_t num)
    {
        auto msg = std_msgs::msg::Int64();
        msg.data = num;
        pub_->publish(msg);
    }
    
    rclcpp::Publisher<std_msgs::msg::Int64>::SharedPtr pub_;
    std::thread reader_thread_;
    std::atomic<bool> running_;
};
```

**Node 2: Processing Node** - Subscribes to input, publishes result
```cpp
class FactorialNode : public rclcpp::Node
{
public:
    FactorialNode() : Node("factorial_node")
    {
        sub_ = this->create_subscription<std_msgs::msg::Int64>(
            "input_number", 10,
            std::bind(&FactorialNode::compute_callback, this, _1)
        );
        
        pub_ = this->create_publisher<std_msgs::msg::Int64>("fact_result", 10);
    }

private:
    void compute_callback(const std_msgs::msg::Int64::SharedPtr msg)
    {
        int64_t result = factorial(msg->data);
        auto output = std_msgs::msg::Int64();
        output.data = result;
        pub_->publish(output);
    }
    
    int64_t factorial(int64_t n)
    {
        return (n <= 1) ? 1 : n * factorial(n - 1);
    }
    
    rclcpp::Subscription<std_msgs::msg::Int64>::SharedPtr sub_;
    rclcpp::Publisher<std_msgs::msg::Int64>::SharedPtr pub_;
};
```

**Node 3: Output Node** - Displays final result
```cpp
class OutputNode : public rclcpp::Node
{
public:
    OutputNode() : Node("output_node")
    {
        sub_ = this->create_subscription<std_msgs::msg::Int64>(
            "fact_result", 10,
            std::bind(&OutputNode::callback, this, _1)
        );
    }

private:
    void callback(const std_msgs::msg::Int64::SharedPtr msg)
    {
        RCLCPP_INFO(this->get_logger(),
                    "Factorial result: %ld", msg->data);
    }
    
    rclcpp::Subscription<std_msgs::msg::Int64>::SharedPtr sub_;
};
```

**Run the Pipeline:**
```bash
# Terminal 1: Input node
ros2 run my_cpp_pkg input_num

# Terminal 2: Processing node
ros2 run my_cpp_pkg factorial

# Terminal 3: Output node
ros2 run my_cpp_pkg output_num

# Terminal 4: Monitor (optional)
ros2 topic echo input_number
ros2 topic echo fact_result
```

### Advantages of Pipeline Architecture
- **Modular**: Each node has single responsibility
- **Testable**: Test each node independently
- **Reusable**: Nodes can be used in different pipelines
- **Scalable**: Easy to add/remove nodes
- **Flexible**: Can rearrange pipeline without changing node code

---

## User Input Pattern

### What is User Input Pattern?
This pattern allows a node to read user input from keyboard (stdin) and publish it as ROS2 messages or use it to trigger actions.

### Why Use?
- **Interactive nodes**: Allow user to control behavior at runtime
- **Testing**: Manually send data/commands
- **Debugging**: Quick input without external tools
- **Simple interfaces**: User-friendly control

### C++ Implementation

**Using std::thread for Non-Blocking Input:**
```cpp
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int64.hpp>
#include <thread>
#include <atomic>
#include <iostream>

class InputNode : public rclcpp::Node
{
public:
    InputNode() : Node("input_node"), running_(true)
    {
        pub_ = this->create_publisher<std_msgs::msg::Int64>("user_input", 10);
        
        // Start reader thread (doesn't block ROS2 spin)
        reader_thread_ = std::thread([this]() {
            while (rclcpp::ok() && running_) {
                int64_t value;
                std::cout << "Enter a number: ";
                
                if (!(std::cin >> value)) {
                    running_ = false;
                    break;
                }
                
                publishValue(value);
            }
        });
    }
    
    ~InputNode()
    {
        running_ = false;
        if (reader_thread_.joinable()) {
            reader_thread_.join();
        }
    }

private:
    void publishValue(int64_t value)
    {
        auto msg = std_msgs::msg::Int64();
        msg.data = value;
        pub_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "Published: %ld", value);
    }
    
    rclcpp::Publisher<std_msgs::msg::Int64>::SharedPtr pub_;
    std::thread reader_thread_;
    std::atomic<bool> running_;
};
```

### Python Implementation

**Using threading for Non-Blocking Input:**
```python
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int64
import threading

class InputNode(Node):
    def __init__(self):
        super().__init__("input_node")
        self.pub = self.create_publisher(Int64, "user_input", 10)
        self.running = True
        
        # Start reader thread
        self.reader_thread = threading.Thread(target=self.read_input)
        self.reader_thread.daemon = True
        self.reader_thread.start()
    
    def read_input(self):
        while rclpy.ok() and self.running:
            try:
                value = int(input("Enter a number: "))
                msg = Int64()
                msg.data = value
                self.pub.publish(msg)
                self.get_logger().info(f"Published: {value}")
            except ValueError:
                self.get_logger().warn("Invalid input")
            except EOFError:
                self.running = False

def main(args=None):
    rclpy.init(args=args)
    node = InputNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()
```

### Best Practices

1. **Always use threads** - Don't block the main ROS2 spin
2. **Handle EOF** - Gracefully exit when input stream ends
3. **Validate input** - Check user input is valid
4. **Use atomic variables** - For thread-safe flag management
5. **Clean up threads** - Join threads in destructor

---

## Important Terminal Commands

### Workspace Commands
```bash
# Build workspace
colcon build
colcon build --symlink-install              # Python dev mode

# Source workspace (run from ros2_ws)
source install/setup.bash

# Check workspace
echo $ROS_PACKAGE_PATH
```

### Package Commands
```bash
# Create package
ros2 pkg create --build-type ament_python my_py_pkg
ros2 pkg create --build-type ament_cmake my_cpp_pkg --dependencies rclcpp

# List all packages
ros2 pkg list

# Get package info
ros2 pkg prefix my_py_pkg
```

### Node Commands
```bash
# Run a node
ros2 run my_py_pkg my_node

# Run with arguments
ros2 run my_py_pkg my_node --ros-args -p param_name:=value

# List running nodes
ros2 node list

# Get node info
ros2 node info /node_name
```

### Topic Commands
```bash
# List all topics
ros2 topic list

# See topic message type
ros2 topic list -t

# Echo (listen to) a topic
ros2 topic echo /robot_news

# Show topic info
ros2 topic info /robot_news

# Publish message to topic (test)
ros2 topic pub /robot_news example_interfaces/msg/String "{data: 'hello'}"

# Check topic rate
ros2 topic hz /robot_news

# Get topic bandwidth
ros2 topic bw /robot_news
```

### Service Commands
```bash
# List all services
ros2 service list

# See service type
ros2 service list -t

# Get service info
ros2 service info /add_two_ints

# Call service (test)
ros2 service call /add_two_ints example_interfaces/srv/AddTwoInts "{a: 5, b: 3}"

# Find service by type
ros2 service find example_interfaces/srv/AddTwoInts
```

### Debugging Commands
```bash
# Show ROS graph (nodes and topics)
ros2 graph

# Show detailed node graph
ros2 node list --all

# Check ROS environment
ros2 doctor

# Show ROS distro
ros2 --version

# List messages
ros2 interface list

# Show message definition
ros2 interface show example_interfaces/msg/String
```

### Setup.py Console Scripts
In `setup.py`, add executables:
```python
entry_points={
    'console_scripts': [
        'my_node = my_py_pkg.my_first_node:main',
        'server = my_py_pkg.add_two_ints_server:main',
        'client = my_py_pkg.add_two_ints_client:main',
    ],
},
```

Then run as:
```bash
ros2 run my_py_pkg my_node
ros2 run my_py_pkg server
ros2 run my_py_pkg client
```

---

## Quick Reference

| Feature | Topic | Service |
|---------|-------|---------|
| Type | Asynchronous | Synchronous |
| Communication | One-way (pub/sub) | Request/Response |
| Clients | Many subscribers | Many clients |
| Servers | One publisher | One server |
| Use Case | Continuous data | On-demand action |
| Response | No response | Always response |
| Example | Sensor data | Add two numbers |

---

## Your Project Files

- `my_cpp_pkg/` - C++ nodes and services
- `my_py_pkg/` - Python nodes and services
- Topics: `robot_news`
- Services: `add_two_ints`
- Nodes: `robot_news_station`, `smartphone`, `add_two_ints_server`, `add_two_ints_client`

---

Last Updated: June 2026
