# Simple Adafruit BNO055 sensor reading example.  Will print the orientation
# and calibration data every second.
#
# Copyright (c) 2015 Adafruit Industries
# Author: Tony DiCola
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
import logging
import sys
import time
import socket
# import sys
import json

from Adafruit_BNO055 import BNO055

def limit(num, minimum, maximum):
    if num < minimum:
        num = minimum
    elif num > maximum:
        num = maximum
    return num


# Create and configure the BNO sensor connection.  Make sure only ONE of the
# below 'bno = ...' lines is uncommented:
# Raspberry Pi configuration with serial UART and RST connected to GPIO 18:
bno = BNO055.BNO055(serial_port='/dev/serial0', rst=18)
# BeagleBone Black configuration with default I2C connection (SCL=P9_19, SDA=P9_20),
# and RST connected to pin P9_12:
#bno = BNO055.BNO055(rst='P9_12')


# Enable verbose debug logging if -v is passed as a parameter.
if len(sys.argv) == 2 and sys.argv[1].lower() == '-v':
    logging.basicConfig(level=logging.DEBUG)

# Initialize the BNO055 and stop if something went wrong.
if not bno.begin():
    raise RuntimeError('Failed to initialize BNO055! Is the sensor connected?')

# Print system status and self test result.
status, self_test, error = bno.get_system_status()
print('System status: {0}'.format(status))
print('Self test result (0x0F is normal): 0x{0:02X}'.format(self_test))
# Print out an error if system status is in error mode.
if status == 0x01:
    print('System error: {0}'.format(error))
    print('See datasheet section 4.3.59 for the meaning.')

# Print BNO055 software revision and other diagnostic data.
sw, bl, accel, mag, gyro = bno.get_revision()
print('Software version:   {0}'.format(sw))
print('Bootloader version: {0}'.format(bl))
print('Accelerometer ID:   0x{0:02X}'.format(accel))
print('Magnetometer ID:    0x{0:02X}'.format(mag))
print('Gyroscope ID:       0x{0:02X}\n'.format(gyro))

print('Reading BNO055 data, press Ctrl-C to quit...')

# Client start
HOST, PORT = "143.215.109.64", 50007
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
# Client end

yawRef = 0
pitchRef = 0
count = 0



while True:
    # Read the Euler angles for heading, roll, pitch (all in degrees).
    yaw, roll, pitch = bno.read_euler()
    # Read the calibration status, 0=uncalibrated and 3=fully calibrated.
    sys, gyro, accel, mag = bno.get_calibration_status()

    if count == 0:
        yawRef = yaw
        pitchRef = pitch
        # loopPN = pitchRef - 60 < -180
        # loopPP = pitchRef + 60 > 180
        # loopYN = yawRef - 85 < -180
        # loopYP = yawRef + 85 > 180
        count = 1

    if yawRef > 180:
        if yaw < 180:
            yaw = 360 + yaw - yawRef
        else:
            yaw = yaw - yawRef
    else:
        if yaw > 180:
            yaw = yaw - 360 - yawRef
        else:
            yaw = yaw - yawRef


    fb = limit(pitch, -45, 45)
    turn = limit(yaw, -70, 70)

    fb = (fb*80)/45
    turn = (turn*60)/70

    if fb >= 0:
        if turn <= 0:
            pwmA = fb + turn
            pwmB = fb
        else:
            pwmA = fb
            # pwmB = limit(fb - turn,-80,80)
            pwmB = fb - turn
    elif turn <= 0:
        pwmA = fb - turn
        pwmB = fb
    else:
        pwmA = fb
        pwmB = fb + turn

    yaw = format(yaw, '3.2f')
    roll = format(roll, '3.2f')
    pitch = format(pitch, '3.2f')

    # Client start
    data = json.dumps({"A": int(pwmA), "B": int(pwmB)})
    s.send(data.encode())
    print(str(s.recv(1000)))
    # Client end

    # yawSize = yaw.size()
    # rollSize = roll.size()
    # pitchSize = pitch.size()

    print('yaw={0} Roll={1} Pitch={2}\n'.format(
          yaw, roll, pitch, ))
    print('pwmA = {0} pwmB={1} pitchRef={2} yawRef={3}\n'.format(
        int(pwmA),int(pwmB), pitchRef, yawRef))
    # Other values you can optionally read:
    # Orientation as a quaternion:
    #x,y,z,w = bno.read_quaterion()
    # Sensor temperature in degrees Celsius:
    #temp_c = bno.read_temp()
    # Magnetometer data (in micro-Teslas):
    #x,y,z = bno.read_magnetometer()
    # Gyroscope data (in degrees per second):
    #x,y,z = bno.read_gyroscope()
    # Accelerometer data (in meters per second squared):
    #x,y,z = bno.read_accelerometer()
    # Linear acceleration data (i.e. acceleration from movement, not gravity--
    # returned in meters per second squared):
    #x,y,z = bno.read_linear_acceleration()
    # Gravity acceleration data (i.e. acceleration just from gravity--returned
    # in meters per second squared):
    #x,y,z = bno.read_gravity()
    # Sleep for a second until the next reading.
    # import socket
    # import sys
    # import json
    #
    # HOST, PORT = "localhost", 50007
    #
    # s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    #
    # s.connect((HOST, PORT))
    # for x in range(100, 200):
    #     # make sure the data is sent in 5 digit length
    #     print("Step 1")
    #     arr1 = x
    #     arr2 = 2 * x
    #     someVar = 300 - x
    #     data = json.dumps({"roll": arr1, "pitch": arr2, "yaw": someVar})
    #     s.send(data.encode())
    #     print("Step 2")
    #     print(str(s.recv(1000)))
    #     print(x)

    time.sleep(1)