import numpy as np
import ctypes
from ctypes import c_char_p, c_int, c_uint8, byref, \
                   create_string_buffer, addressof

_basepng = ctypes.CDLL('libbasepng/libbasepng.so')
_basepng.decode_png.argtype = c_char_p
_basepng.decode_png.restype = c_int

_basepng.encode_png.argtypes = (c_char_p, c_char_p)
_basepng.encode_png.restype = c_int

def decode_png(input_file_location):
    global _basepng

    input_file_location_bytes = input_file_location.encode('utf-8')
    _basepng.decode_png(input_file_location_bytes)

def encode_png(input_file_location, output_file_location):
    global _basepng

    input_file_location_bytes = input_file_location.encode('utf-8')
    output_file_location_bytes = output_file_location.encode('utf-8')
    _basepng.encode_png(input_file_location_bytes,
                        output_file_location_bytes)

