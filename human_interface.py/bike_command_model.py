import struct

class BicycleModelTranslator:
    def __init__(self, wifi_talker):
        self.wifi_talker = wifi_talker
        self.struct_code = '<ff' # little-endian, 2 floats

    def send_command(self, steer_angle, throttle):
        packed_data = struct.pack(self.struct_code, steer_angle, throttle)
        self.wifi_talker.send_data(packed_data)

