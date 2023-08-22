                      
#include <AccelStepper.h>
#include <Servo.h>

// Define stepper motors and servo
AccelStepper stepper1(1, 2, 5, 8);
AccelStepper stepper2(1, 4, 7, 8);
Servo pen;

// Robot arm parameters
float l1 = 170; // Length of link 1 (mm)
float l2 = 211; // Length of link 2 (mm)
float offset = 65.5; // Distance from base to the edge of the table
int ser_pin = 12; // Servo pin

float rx = 0.06;
float ry = 0.09;
int Step_speed = 170;
float c2, s2, c1, s1, px, py, t1, t2;
float tt1 = 0, tt2 = 0;
int ttt1 = 0, ttt2 = 0;
char buffer[4] = {};
String command;
float stride, inipos_x, inipos_y, pos_x, pos_y;
float conv = 48;
float radius;
bool pen_up = 0, pen_down = 1;

// Calculate inverse kinematics
void calculate(float px, float py) {
  c2 = (px * px + py * py - l1 * l1 - l2 * l2) / (2 * l1 * l2);
  s2 = sqrt(abs(1 - c2 * c2));
  t2 = atan2(s2, c2) * 180 / 3.14;
  tt2 = t2 / ry;
  ttt2 = round(tt2);
  c1 = px * (l1 + l2 * c2) + py * l2 * s2;
  s1 = py * (l1 + l2 * c2) - px * l2 * s2;
  t1 = atan2(s1, c1) * 180 / 3.14;
  tt1 = t1 / rx;
  ttt1 = round(tt1);
}

// Run stepper motors
void Run_step(int pulse1, int pulse2, int step_speed) {
  stepper1.moveTo(pulse1);
  stepper1.setSpeed(step_speed);
  stepper2.moveTo(pulse2);
  stepper2.setSpeed(step_speed);
  while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0) {
    stepper1.runSpeedToPosition();
    stepper2.runSpeedToPosition();
  }
}

// Control pen (1 is pen down, 0 is pen up)
void end_effector(bool mypen) {
  if (mypen == 1) {
    pen.write(90);
  }
  if (mypen == 0) {
    pen.write(180);
  }
}

// Draw nought (O)
void nought(float center_x, float center_y, float radius) {
  px = radius + center_x;
  py = center_y;
  calculate(px, py);
  Run_step(ttt1, ttt2, Step_speed);
  end_effector(pen_down);
  for (float i = 0; i <= 2 * 31.4; i++) {
    px = radius * cos(i * 0.1) + center_x;
    py = radius * sin(i * 0.1) + center_y;
    calculate(px, py);
    Run_step(ttt1, ttt2, Step_speed);
    delay(10);
  }
  end_effector(pen_up);
  Run_step(0, 0, Step_speed);
}

// Draw cross (X)
void cross(float center_x, float center_y, float radius) {
  // Draw diagonal line
  px = center_x + radius * cos(135 * 3.14 / 180);
  py = center_y + radius * sin(135 * 3.14 / 180);
  calculate(px, py);
  Run_step(ttt1, ttt2, Step_speed);
  end_effector(pen_down);
  for (float r = radius; r >= -radius; r--) {
    px = center_x + r * cos(135 * 3.14 / 180);
    py = center_y + r * sin(135 * 3.14 / 180);
    calculate(px, py);
    Run_step(ttt1, ttt2, Step_speed);
    delay(10);
  }
  end_effector(pen_up);

  // Draw anti-diagonal line
  px = center_x - radius * cos(135 * 3.14 / 180);
  py = center_y + radius * sin(135 * 3.14 / 180);
  calculate(px, py);
  Run_step(ttt1, ttt2, Step_speed);
  end_effector(pen_down);
  for (float r = radius; r >= -radius; r--) {
    px = center_x - r * cos(135 * 3.14 / 180);
    py = center_y + r * sin(135 * 3.14 / 180);
    calculate(px, py);
    Run_step(ttt1, ttt2, Step_speed);
    delay(10);
  }
  end_effector(pen_up);

  Run_step(0, 0, Step_speed);
}

void setup() {
  Serial.begin(9600);

  // Attach servo
  pen.attach(ser_pin);

  // Set up stepper motors
  stepper1.setMaxSpeed(1000);
  stepper1.setCurrentPosition(0);
  stepper2.setMaxSpeed(1000);
  stepper2.setCurrentPosition(0);

  // Set up enable pin
  pinMode(8, OUTPUT);
}

void loop() {
  // Disable servo and enable pin
  end_effector(pen_up);
  digitalWrite(8, LOW);

  // Read command from serial input
  if (Serial.available() > 0) {
    Serial.readBytes(buffer, 4);
    command = buffer;
    // Calculate position and radius based on input command
    // Draw either 'x' (cross) or 'o' (nought)
    // Delete char and command from buffer
    Serial.flush();
  }
}
