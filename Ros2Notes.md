# ROS2 Complete Notes

## Table of Contents
1. [ROS2 Basics](#ros2-basics)
2. [Workspace Setup](#workspace-setup)
3. [Creating Packages](#creating-packages)
4. [Creating Nodes](#creating-nodes)
5. [Topics (Pub/Sub)](#topics)
6. [Services (Request/Response)](#services)
7. [Important Terminal Commands](#important-terminal-commands)

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
