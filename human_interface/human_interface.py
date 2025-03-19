import pygame
import time
from talker import WifiTalkerController
from bike_command_model import BicycleModelTranslator
import argparse

# Constants
STEERING_SCALE = 0.25  # Scale keyboard input to desired steering range (e.g., -30 to +30 degrees)
STEERING_OFFSET = 0.65
THROTTLE_SCALE = 0.2   # Scale keyboard input to desired throttle range (e.g., 0 to 1)
THROTTLE_OFFSET = 0.5
UPDATE_RATE = 1 / 10.0  # 50Hz update rate

def get_keyboard_input():
    """Reads WASD keyboard input and returns normalized steer angle and throttle."""
    keys = pygame.key.get_pressed()

    steer_angle = STEERING_OFFSET
    throttle = THROTTLE_OFFSET


    
    if keys[pygame.K_a]:
        steer_angle = -STEERING_SCALE + STEERING_OFFSET  # Turn left
    if keys[pygame.K_d]:
        steer_angle = STEERING_SCALE + STEERING_OFFSET  # Turn right
    if keys[pygame.K_w]:
        throttle = THROTTLE_SCALE + THROTTLE_OFFSET  # Accelerate
    if keys[pygame.K_s]:
        throttle = -THROTTLE_SCALE + THROTTLE_OFFSET # Reverse
    
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

    # Initialize pygame
    pygame.init()
    screen = pygame.display.set_mode((400, 300))  # Small window to capture keyboard input
    pygame.display.set_caption("WASD Controller")

    print("WASD control initialized. Sending commands at 50Hz...")
    try:
        while True:
            pygame.event.pump()
            steer_angle, throttle = get_keyboard_input()
            bike_model.send_command(steer_angle, throttle)
            if args.debug:
                print(f"Steer Angle: {steer_angle}, Throttle: {throttle}")
            time.sleep(UPDATE_RATE)  # Maintain 50Hz loop
    except KeyboardInterrupt:
        print("Stopping...")
    finally:
        pygame.quit()


if __name__ == "__main__":
    main()
