#define IN1 GPIO_PIN_0  // Connect to L298N IN1
#define IN2 GPIO_PIN_1  // Connect to L298N IN2
#define IN3 GPIO_PIN_2  // Connect to L298N IN3
#define IN4 GPIO_PIN_3  // Connect to L298N IN4
#define MOTOR_PORT GPIOA  // Change to correct port


void Stepper_Step(int step);
void Stepper_Move(int steps, int delay);