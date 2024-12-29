import socket
import logging
from ctypes import POINTER, Structure, c_size_t, c_uint16, c_uint32, c_uint8, c_void_p, cast, memmove, pointer, sizeof
from typing import Generic, List, Optional, TypeVar, Union, Self

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class PackedStruct(Structure):
    _pack_ = 1

class PacketHeader(PackedStruct):
    _fields_ = [
        ('type', c_uint16),
        ('size', c_uint32)
    ]

    def __str__(self) -> str:
        return f"PacketHeader[type={self.type},size={self.size}]"

class NetworkByteVector(PackedStruct):
    _fields_ = [
        ('data_size', c_size_t),
        ('data_array', POINTER(c_uint8))
    ]

    def __init__(
        self, 
        data: Union[List[c_uint8], int, None] = None, 
        extra_data: Optional[List[c_uint8]] = None
    ):
        super().__init__()
        
        if data is None:
            self.data_size = 0
            self.data_array = None
        
        elif isinstance(data, int):
            self.data_size = data
            array_type = type(data[0]) * data if data > 0 else None
            self.data_array = cast(array_type, POINTER(c_uint8)) if array_type else None
        
        elif isinstance(data, list):
            if extra_data:
                combined_data = data + extra_data
                self.data_size = len(combined_data)
                array_type = type(combined_data[0]) * len(combined_data)
                self.data_array = cast(array_type(*combined_data), POINTER(c_uint8))
            else:
                self.data_size = len(data)
                array_type = type(data[0]) * len(data)
                self.data_array = cast(array_type(*data), POINTER(c_uint8))

    @classmethod
    def from_range(cls, begin, end):
        size = end - begin
        data = [begin[i] for i in range(size)]
        return cls(data)

class ReadMemPayload(PackedStruct):
    _fields_ = [
        ('address', c_uint32)
    ]

class ReadMemPayloadRes(PackedStruct):
    _fields_ = [

    ]

from ctypes import Structure, c_uint8, c_uint16, c_uint32, sizeof, create_string_buffer, memmove, pointer
from typing import TypeVar, Optional
import logging

PacketType = c_uint16
PacketSize = c_uint32
byte = c_uint8

class PacketHeader(Structure):
    _pack_ = 1
    _fields_ = [
        ("type", PacketType),
        ("size", PacketSize)
    ]

T = TypeVar('T')

class Packet:
    def __init__(self, header: PacketHeader, data: bytes):
        self.header = header
        self.data = data

    @staticmethod
    def wrap(type: int, struct_data: Structure) -> 'Packet':
        data_size = sizeof(struct_data)
        data_buffer = create_string_buffer(data_size)
        memmove(data_buffer, pointer(struct_data), data_size)
        
        header = PacketHeader(type=type, size=data_size)
        return Packet(header, bytes(data_buffer))

    def serialize(self) -> bytes:
        header_buffer = create_string_buffer(sizeof(PacketHeader))
        memmove(header_buffer, pointer(self.header), sizeof(PacketHeader))
        return bytes(header_buffer) + self.data

    def deserialize(self, struct_class: type[Structure]) -> Optional[Structure]:
        if sizeof(struct_class) != len(self.data):
            logging.error(f"Size mismatch: expected {sizeof(struct_class)}, got {len(self.data)}")
            return None
            
        result = struct_class()
        memmove(pointer(result), self.data, len(self.data))
        return result

def main():
    SERVER_HOST = 'localhost'
    SERVER_PORT = 8088

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            logger.info(f"Connecting to {SERVER_HOST}:{SERVER_PORT}.")
            sock.connect((SERVER_HOST, SERVER_PORT))
            logger.info(f"Connection established!")

            rmPayload = ReadMemPayload()
            rmPayload.address = 0x69

            sock.send(Packet.wrap(1, rmPayload).serialize())
            

        

    except ConnectionRefusedError:
        logger.error(f"Could not connect to {SERVER_HOST}:{SERVER_PORT}")
    except Exception as e:
        logger.error(f"An error occurred: {e}")

if __name__ == "__main__":
    main()
