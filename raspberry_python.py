import RPi.GPIO as GPIO
from gpiozero import Servo
import math
import time

# Define GPIO pins
stepper1_pins = [2, 5, 8]
stepper2_pins = [4, 7, 8]
servo_pin = 12

# Robot arm parameters
l1 = 170  # Length of link 1 (mm)
l2 = 211  # Length of link 2 (mm)
offset = 65.5  # Distance from base to the edge of the table

# Initialize GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(stepper1_pins, GPIO.OUT)
GPIO.setup(stepper2_pins, GPIO.OUT)
GPIO.setup(servo_pin, GPIO.OUT)

# Initialize servo
pen = Servo(servo_pin)

# Set initial positions and speeds
Step_speed = 170
rx = 0.06
ry = 0.09
conv = 48
pen_up = 0
pen_down = 1

# Calculate inverse kinematics
def calculate(px, py):
    c2 = (px * px + py * py - l1 * l1 - l2 * l2) / (2 * l1 * l2)
    s2 = math.sqrt(abs(1 - c2 * c2))
    t2 = math.degrees(math.atan2(s2, c2))
    tt2 = t2 / ry
    ttt2 = round(tt2)
    c1 = px * (l1 + l2 * c2) + py * l2 * s2
    s1 = py * (l1 + l2 * c2) - px * l2 * s2
    t1 = math.degrees(math.atan2(s1, c1))
    tt1 = t1 / rx
    ttt1 = round(tt1)
    return ttt1, ttt2

# Run stepper motors
def run_step(pulse1, pulse2, step_speed):
    GPIO.output(stepper1_pins[2], GPIO.HIGH if pulse1 > 0 else GPIO.LOW)
    GPIO.output(stepper2_pins[2], GPIO.HIGH if pulse2 > 0 else GPIO.LOW)
    GPIO.output(stepper1_pins[0], GPIO.HIGH if pulse1 > 0 else GPIO.LOW)
    GPIO.output(stepper2_pins[0], GPIO.HIGH if pulse2 > 0 else GPIO.LOW)

    for _ in range(abs(pulse1)):
        GPIO.output(stepper1_pins[1], GPIO.HIGH)
        time.sleep(1.0 / step_speed)
        GPIO.output(stepper1_pins[1], GPIO.LOW)
        time.sleep(1.0 / step_speed)

    for _ in range(abs(pulse2)):
        GPIO.output(stepper2_pins[1], GPIO.HIGH)
        time.sleep(1.0 / step_speed)
        GPIO.output(stepper2_pins[1], GPIO.LOW)
        time.sleep(1.0 / step_speed)

# Control pen (1 is pen down, 0 is pen up)
def end_effector(mypen):
    pen.value = 0.5 if mypen == 1 else -1

# Draw nought (O)
def nought(center_x, center_y, radius):
    px, py = radius + center_x, center_y
    ttt1, ttt2 = calculate(px, py)
    run_step(ttt1, ttt2, Step_speed)
    end_effector(pen_down)

    for i in range(int(2 * math.pi * 10)):
        px = radius * math.cos(i * 0.1) + center_x
        py = radius * math.sin(i * 0.1) + center_y
        ttt1, ttt2 = calculate(px, py)
        run_step(ttt1, ttt2, Step_speed)
        time.sleep(0.01)

    end_effector(pen_up)
    run_step(0, 0, Step_speed)

# Draw cross (X)
def cross(center_x, center_y, radius):
    # Draw diagonal line
    px = center_x + radius * math.cos(135 * math.pi / 180)
    py = center_y + radius * math.sin(135 * math.pi / 180)
    ttt1, ttt2 = calculate(px, py)
    run_step(ttt1, ttt2, Step_speed)
    end_effector(pen_down)

    for r in range(int(radius), -int(radius), -1):
        px = center_x + r * math.cos(135 * math.pi / 180)
        py = center_y + r * math.sin(135 * math.pi / 180)
        ttt1, ttt2 = calculate(px, py)
        run_step(ttt1, ttt2, Step_speed)
        time.sleep(0.01)

    end_effector(pen_up)

    # Draw anti-diagonal line
    px = center_x - radius * math.cos(135 * math.pi / 180)
    py = center_y + radius * math.sin(135 * math.pi / 180)
    ttt1, ttt2 = calculate(px, py)
    run_step(ttt1, ttt2, Step_speed)
    end_effector(pen_down)

    for r in range(int(radius), -int(radius), -1):
        px = center_x - r * math.cos(135 * math.pi / 180)
        py = center_y + r * math.sin(135 * math.pi / 180)
        ttt1, ttt2 = calculate(px, py)
        run_step(ttt1, ttt2, Step_speed)
        time.sleep(0.01)

    end_effector(pen_up)
    run_step(0, 0, Step_speed)

# Set up enable pin
GPIO.setup(8, GPIO.OUT)
GPIO.output(8, GPIO.HIGH)

try:
    while True:
        # Read command from input
        command = input("Enter command ('x' for cross, 'o' for nought): ")
        if command.lower() == 'x':
            cross(0, 0, 50)
        elif command.lower() == 'o':
            nought(0, 0, 50)
except KeyboardInterrupt:
    GPIO.cleanup()
