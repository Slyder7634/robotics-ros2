#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from example_interfaces.srv import AddTwoInts

class AddTwoIntServerNode(Node):

    def __init__(self):
        self.counter = 0
        super().__init__("add_two_ints_server")
        self.server = self.create_service(AddTwoInts, "add_two_ints", self.callback_add_two_ints)
        self.get_logger().info("AddTwoInts server is ready to receive requests.")

    def callback_add_two_ints(self, request: AddTwoInts.Request, response:AddTwoInts.Response):
        self.counter += 1
        self.get_logger().info(f"Incoming request #{self.counter}: a={request.a}, b={request.b}")
        response.sum = request.a + request.b
        return response
        


def main(args = None):
    rclpy.init(args=args)
    node = AddTwoIntServerNode()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == "__main__":
    main()
