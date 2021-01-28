#!/usr/bin/python3

import signal
import time
import serial
import glob

def hex8(x):
    return '0x{0:0{1}x}'.format(x,2)

def hex16(x):
    return '0x{0:0{1}x}'.format(x,4)


def serial_ports():
    ports = glob.glob('/dev/tty.*')
    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except:
            pass
    return result

class Interface:
    def __init__(self):
        self.clock_speed = 500
        self.clock_running = False
        self.connected = False
        self.serial = None

        signal.signal(signal.SIGINT, self.handle_interrupt)

    def connect(self, port="/dev/tty.usbmodem141101", baudrate=9600):
        print("Attempting to connect to", port, "[", baudrate, "]...", end="")
        try:
            self.serial = serial.Serial(port, baudrate=baudrate, timeout=.1)
            self.connected = True
            print(" success.")
        except:
            print(" failed.")
    
    def _serial_write(self, data):
        if not self.connected:
            print("no serial connection found")
            return

        if not isinstance(data, list):
            data = [data]

        self.serial.write(bytearray(data))
        time.sleep(0.05)

    def _serial_read(self):
        if not self.connected:
            print("no serial connection found")
            return
        b = self.serial.read()
        return int.from_bytes(b, byteorder='little', signed=False)

    def _serial_readall(self):
        res = []
        s = self._serial_read()
        while s:
            res.append(s)
            s = self._serial_read()
        return res

    def _checkok(self):
        ok = self._serial_read()
        if ok != 1:
            raise Exception("Expected code 1, instead got: " + str(ok))

    def handle_interrupt(self, signum, frame):
        if not self.clock_running:
            print("exit")
            if self.connected:
                self.serial.close()
            exit(0)
        self.clock_running = False

    def _rwb_read(self):
        self._serial_write(0xf4)
        self._checkok()
        return self._serial_read()

    def data_bus_read(self):
        self._serial_write(0xf3)
        self._checkok()
        return self._serial_read()
        
    def addr_bus_read(self):
        self._serial_write(0xf2)
        self._checkok()
        addr_lo = self._serial_read()
        addr_hi = self._serial_read()
        return (addr_hi << 8) | addr_lo

    def _rom_read(self, addr):
        self._serial_write(0xf5)
        self._serial_write(addr & 0xff)
        self._serial_write((addr >> 8) & 0xff)
        self._checkok()
        return self._serial_read()

    def rom_read(self, addr, nbytes=1):
        for i in range(nbytes):
            if i%16 == 0:
                print()
                print(hex(addr), end=". ")
        
            print(hex(self._rom_read(addr)), end=" ")
            addr += 1

        print()

    def _rom_write(self, addr, data):
        self._serial_write(0xf6)
        self._serial_write(addr & 0xff)
        self._serial_write((addr >> 8) & 0xff)
        self._serial_write(data)
        self._checkok()

    def rom_write(self, addr, data):
        if not isinstance(data, list):
            data = [data]

        for b in data:
            self._rom_write(addr, b)
            addr += 1

    def _clock_high(self):
        self._serial_write(0xf1)
        self._checkok()

    def _clock_low(self):
        self._serial_write(0xf0)
        self._checkok()
    
    def _clock_sleep(self):
        time.sleep(self.clock_speed / 1000.0)

    def clock_tick(self):
        self._clock_high()
        addr = self.addr_bus_read()
        data = self.data_bus_read()
        rwb = self._rwb_read()
        self._clock_sleep()

        print(hex16(addr), hex8(data), 'r' if rwb else 'W')
        self._clock_low()
        self._clock_sleep()

    def clock_run(self):
        self.clock_running = True
        print("start running clock")
        while self.clock_running:
            self.clock_tick()
        print("\ndone running clock")

    def clock_duration(self, duration=None):
        if duration:
            self.clock_speed = duration
        else:
            print("clock duration =", self.clock_speed)


interface = Interface()
interface.connect()

"""
Examples
--------

read reset vector:
interface.rom_read(0xfffc, 2)

write nop loop:
interface.rom_write(0x8000, [0xea, 0x4c, 0x0, 0x80, 0xea])

run:
interface.clock_run()

Ctrl+C to stop the clock while running. Interrupt is handled properly.
"""

"""

0x8000 LDA #$0      0xa9 0x00
0x8002 PHA          0x48
0x8003 ADC #$1      0x69 0x01
0x8005 JMP $0x8000  0x4c 0x00 0x80

interface.rom_write(0x8000, [0xa9, 0x00, 0x48, 0x69, 0x01, 0x4c, 0x00, 0x80])

0x8000 LDA #$1      0xa9 0x01
0x8002 PHA          0x48
0x8003 PLA          0x68
0x8004 ADC #$1      0x69 0x01
0x8006 PHA          0x48
0x8007 PLA          0x68
0x8008 ADC #$1      0x69 0x01
0x800a PHA          0x48
0x800b PLA          0x68
0x800c JMP $0x800c  0x4c 0x0c 0x80

interface.rom_write(0x8000, \
    [0xa9, 0x01, 0x48, 0x68,\
     0x69, 0x01, 0x48, 0x68,\
     0x69, 0x01, 0x48, 0x68,\
     0x4c, 0x0c, 0x80])

$8000    a9 04     LDA #$04
$8002    85 00     STA $00
$8004    a9 02     LDA #$02
$8006    85 01     STA $01
$8008    20 10 80  JSR $8010
$800b    85 02     STA $02
$800d    4c 20 80  JMP $8020
$8010    a9 00     LDA #$00
$8012    a6 00     LDX $00
$8014    e0 00     CPX #$00
$8016    f0 07     BEQ $801f
$8018    ca        DEX 
$8019    18        CLC 
$801a    65 01     ADC $01
$801c    4c 14 80  JMP $8014
$801f    60        RTS 
$8020    ea        NOP 
$8021    4c 20 80  JMP $8020

a9 04 85 00 a9 02 85 01 20 10 80 85 02 4c 20 80 
a9 00 a6 00 e0 00 f0 07 ca 18 65 01 4c 14 80 60 
ea 4c 20 80 

interface.rom_write(0x8000, [0xa9, 0x04, 0x85, 0x00, 0xa9, 0x02, 0x85, 0x01, 0x20, 0x10, 0x80, 0x85, 0x02, 0x4c, 0x20, 0x80, 0xa9, 0x00, 0xa6, 0x00, 0xe0, 0x00, 0xf0, 0x07, 0xca, 0x18, 0x65, 0x01, 0x4c, 0x14, 0x80, 0x60, 0xea, 0x4c, 0x20, 0x80])
    
"""