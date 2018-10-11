import socket
import machine
import machine, neopixel
import time



#HTML to send to browsers
html = """<!DOCTYPE html>
<html>
<head> <title>ESP8266 LED ON/OFF</title> </head>
<center><h2>Webserver to LED's</h2>
<form>
LED16:
<button name="LED" value="ON16" type="submit">LED ON</button>
<button name="LED" value="OFF16" type="submit">LED OFF</button><br><br>
LED2:
<button name="LED" value="ON2" type="submit">LED ON</button>
<button name="LED" value="OFF2" type="submit">LED OFF</button><br><br>
NEO:
<button name="NEO" value="NEO" type="submit">NEO SHOW</button><br><br>
LEDRED:
<button name="LED" value="ONRED" type="submit">LED ON</button>
<button name="LED" value="OFFRED" type="submit">LED OFF</button>
</form>
</html>
"""

#Setup PINS
LED16 = machine.Pin(16, machine.Pin.OUT)
LED2 = machine.Pin(2, machine.Pin.OUT)
LEDRED = machine.Pin(12, machine.Pin.OUT)
np = neopixel.NeoPixel(machine.Pin(4), 9)


def demo(np):
    n = np.n

    # cycle
    for i in range(4 * n):
        for j in range(n):
            np[j] = (0, 0, 0)
        np[i % n] = (255, 255, 255)
        np.write()
        time.sleep_ms(25)

    # bounce
    for i in range(4 * n):
        for j in range(n):
            np[j] = (0, 0, 128)
        if (i // n) % 2 == 0:
            np[i % n] = (0, 0, 0)
        else:
            np[n - 1 - (i % n)] = (0, 0, 0)
        np.write()
        time.sleep_ms(60)

    # fade in/out
    for i in range(0, 4 * 256, 8):
        for j in range(n):
            if (i // 256) % 2 == 0:
                val = i & 0xff
            else:
                val = 255 - (i & 0xff)
            np[j] = (val, 0, 0)
        np.write()

    # clear
    for i in range(n):
        np[i] = (0, 0, 0)
    np.write()


#Setup Socket WebServer
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('', 80))
s.listen(5)
while True:
    conn, addr = s.accept()
    print("Got a connection from %s" % str(addr))
    request = conn.recv(1024)
    print("Content = %s" % str(request))
    request = str(request)
    LEDON16 = request.find('/?LED=ON16')
    LEDOFF16 = request.find('/?LED=OFF16')
    LEDON2 = request.find('/?LED=ON2')
    LEDOFF2 = request.find('/?LED=OFF2')
    LEDONRED = request.find('/?LED=ONRED')
    LEDOFFRED = request.find('/?LED=OFFRED')
    NEO = request.find('/?NEO=NEO')

    #print("Data: " + str(LEDON0))
    #print("Data2: " + str(LEDOFF0))
    if LEDON16 == 6:
        print('TURN LED16 ON')
        LED16.off()
    if LEDOFF16 == 6:
        print('TURN LED16 OFF')
        LED16.on()
    if LEDON2 == 6:
        print('TURN LED2 ON')
        LED2.off()
    if LEDOFF2 == 6:
        print('TURN LED2 OFF')
        LED2.on()
    if LEDONRED == 6:
        print('TURN LEDRED ON')
        LEDRED.on()
    if LEDOFFRED == 6:
        print('TURN LEDRED OFF')
        LEDRED.off()
    if NEO == 6:
        print('TURN NEO ON')
        demo(np)
    response = html
    conn.send(response)
    conn.close()
