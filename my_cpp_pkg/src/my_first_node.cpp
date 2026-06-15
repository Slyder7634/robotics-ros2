// #include "rclcpp/rclcpp.hpp"
// #include <chrono>
// #include <termios.h>
// #include <unistd.h>
// #include <fcntl.h>

// using namespace std::chrono_literals;

// // // Non-blocking Linux terminal input reader
// // bool kbhit() {
// //     struct termios oldt, newt;
// //     int ch;
// //     int oldf;
// //     tcgetattr(STDIN_FILENO, &oldt);
// //     newt = oldt;
// //     newt.c_lflag &= ~(ICANON | ECHO);
// //     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
// //     oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
// //     fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
// //     ch = getchar();
// //     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
// //     fcntl(STDIN_FILENO, F_SETFL, oldf);
// //     if(ch != EOF) {
// //         ungetc(ch, stdin);
// //         return true;
// //     }
// //     return false;
// // }


// class MyNode : public rclcpp::Node{
//     public:
//         MyNode(): rclcpp::Node("my_first_node"){
//             RCLCPP_INFO(this->get_logger(), "Instant Teleop Started! Use W/A/S/D. Position: [%d, %d]", nums_[0], nums_[1]);
//         } 

//         // void check_keyboard(){
//         //     if (kbhit()) {
//         //         char key = getchar();
//         //         bool position_changed = false;

//             //     // Map keys to coordinate adjustments
//             //     if (key == 'w' || key == 'W') {
//             //         nums_[1]++; // Y increase
//             //         position_changed = true;
//             //     } 
//             //     else if (key == 's' || key == 'S') {
//             //         nums_[1]--; // Y decrease
//             //         position_changed = true;
//             //     } 
//             //     else if (key == 'a' || key == 'A') {
//             //         nums_[0]--; // X decrease
//             //         position_changed = true;
//             //     } 
//             //     else if (key == 'd' || key == 'D') {
//             //         nums_[0]++; // X increase
//             //         position_changed = true;
//             //     }
//             //     if (position_changed) {
//             //         RCLCPP_INFO(this->get_logger(), "Current position: (%d, %d)", nums_[0], nums_[1]);
//             //         // std::cout << "{\"x\":" << nums_[0] << ",\"y\":" << nums_[1] << "}" << std::endl;
//             //     }
//             // }
//         }
//     // private:
//     // int nums_[2] = {0, 0};
//     // };

// int main(int argc, char **argv){
//     rclcpp::init(argc, argv);

//     auto node = std::make_shared<MyNode>();

//     while(rclcpp::ok()){
//         node->check_keyboard();
         
//         rclcpp::spin_some(node);

//         std::this_thread::sleep_for(std::chrono::milliseconds(1));

//     }
//     rclcpp::spin(node);
//     rclcpp::shutdown();
//     return 0;
// }