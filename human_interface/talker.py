import socket
import argparse

class WifiTalkerController:
    def __init__(self, ip: str, port: int, debug: bool = False):
        self.ip = ip
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.debug = debug

    def send_binary(self, data: bytes):
        try:
            self.sock.sendto(data, (self.ip, self.port))
            if self.debug:
                print(f"Debug: Sending data to {self.ip}:{self.port}")
        except Exception as e:
            print(f"Error sending data: {e}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Talker script for the RC Car Controller")
    parser.add_argument(
        "--ip",
        type=str,
        default="192.168.4.1",
        help="IP address of the RC car controller",
    )
    parser.add_argument(
        "--port",
        type=int,
        default=3383,
        help="Port number for the RC car controller",
    )

    args = parser.parse_args()

    controller = WifiTalkerController(args.ip, args.port, debug=True)

    while True:
        data = input("Enter data to send (or 'exit' to quit): ")
        controller.send_binary(data.encode())

