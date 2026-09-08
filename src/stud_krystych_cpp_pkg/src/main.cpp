#include "rclcpp/rclcpp.hpp"
#include "stud_krystych_cpp_pkg/student_publisher.hpp"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<StudentPublisher>();

  rclcpp::spin(node);
  rclcpp::shutdown();

  return 0;
}
