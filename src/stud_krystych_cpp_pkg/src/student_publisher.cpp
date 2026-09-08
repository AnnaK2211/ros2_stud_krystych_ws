#include "stud_krystych_cpp_pkg/student_publisher.hpp"

constexpr int PUBLISH_INTERVAL_MS = 500;

StudentPublisher::StudentPublisher(): Node("stud_krystych_publisher"), counter_(1)
{
    publisher_ = this->create_publisher<std_msgs::msg::String>("/stud_krystych/message", 10);
    timer_ = this->create_wall_timer(std::chrono::milliseconds(PUBLISH_INTERVAL_MS), 
    std::bind(&StudentPublisher::timer_callback, this));
}

void StudentPublisher::timer_callback()
{
  auto message = std_msgs::msg::String();
  message.data = "Krystych | Message #" + std::to_string(counter_++);
  publisher_->publish(message);
  RCLCPP_INFO(this->get_logger(), "%s", message.data.c_str());
}
