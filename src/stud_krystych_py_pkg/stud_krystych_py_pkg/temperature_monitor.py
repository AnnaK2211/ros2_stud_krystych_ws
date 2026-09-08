import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32

PRINT_INTERVAL = 1.0


class TemperatureMonitor(Node):

    def __init__(self):
        super().__init__('stud_krystych_temperature_monitor')

        self.last_celsius = None
        self.last_fahrenheit = None

        self.subscription_celsius = self.create_subscription(Float32, '/stud_krystych/temperature/celsius', self.celsius_callback, 10)
        self.subscription_fahrenheit = self.create_subscription(Float32, '/stud_krystych/temperature/fahrenheit', self.fahrenheit_callback, 10)

        self.timer = self.create_timer(PRINT_INTERVAL, self.print_temperature)

    def celsius_callback(self, msg):
        self.last_celsius = msg.data

    def fahrenheit_callback(self, msg):
        self.last_fahrenheit = msg.data

    def print_temperature(self):
        print('Temperature:')

        if self.last_celsius is not None:
            print(f'Celsius: {self.last_celsius:.1f} °C')
        else:
            print('Celsius: N/A')

        if self.last_fahrenheit is not None:
            print(f'Fahrenheit: {self.last_fahrenheit:.1f} °F')
        else:
            print('Fahrenheit: N/A')

        print()


def main(args=None):
    rclpy.init(args=args)
    node = TemperatureMonitor()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()