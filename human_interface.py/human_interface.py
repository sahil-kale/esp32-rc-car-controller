import pygame
import time
from talker import WifiTalkerController
from bike_command_model import BicycleModelTranslator
import argparse

# Constants
STEERING_SCALE = 30.0  # Scale joystick input to desired steering range (e.g., -30 to +30 degrees)
THROTTLE_SCALE = 1.0   # Scale trigger input to desired throttle range (e.g., 0 to 1)
UPDATE_RATE = 1 / 50.0  # 50Hz update rate


def get_controller_input():
    """Reads Xbox controller input and returns normalized steer angle and throttle."""
    pygame.event.pump()
    
    # Left Stick X-axis for steering (-1 to 1)
    steer_input = pygame.joystick.Joystick(0).get_axis(0)
    steer_angle = steer_input * STEERING_SCALE  # Convert to degrees
    
    # Right trigger (accelerator) (0 to 1)
    throttle_input = (pygame.joystick.Joystick(0).get_axis(5) + 1) / 2  # Normalize to 0-1
    throttle = throttle_input * THROTTLE_SCALE
    
    return steer_angle, throttle


def main():
    parser = argparse.ArgumentParser(description="Talker script for the RC Car Controller")
    parser.add_argument("--ip", type=str, default="192.168.4.1")
    parser.add_argument("--port", type=int, default=3383, help="Port number for the RC car controller")
    parser.add_argument("--debug", action="store_true", help="Enable debug mode")
    args = parser.parse_args()

    # Initialize the talker and model
    controller = WifiTalkerController(args.ip, args.port, debug=args.debug)
    bike_model = BicycleModelTranslator(controller)

    # Initialize pygame and the joystick
    pygame.init()
    joystick = pygame.joystick.Joystick(0)
    joystick.init()

    print("Controller initialized. Sending commands at 50Hz...")
    try:
        while True:
            steer_angle, throttle = get_controller_input()
            bike_model.send_command(steer_angle, throttle)
            time.sleep(UPDATE_RATE)  # Maintain 50Hz loop
    except KeyboardInterrupt:
        print("Stopping...")
    finally:
        pygame.quit()


if __name__ == "__main__":
    main()
