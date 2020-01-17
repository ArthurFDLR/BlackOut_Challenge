import serial
from struct import unpack
from binascii import unhexlify

def decode_float(s):
        return unpack('<f', unhexlify(s))[0]

print(decode_float("C3F54840"))