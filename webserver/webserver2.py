import socket
import machine


#HTML to send to browsers
html = """<!DOCTYPE html>
<html>
<head> <title>ESP8266 LED ON/OFF</title> </head>
<center><h2>A simple webserver for turning onboard LED's on and off with Micropython</h2></center>
<center><h3>(for noobs to both the ESP8266 and Micropython)</h3></center>
<form>
LED0:
<button name="LED" value="ON16" type="submit">LED ON</button>
<button name="LED" value="OFF16" type="submit">LED OFF</button><br><br>
LED2:
<button name="LED" value="ON2" type="submit">LED ON</button>
<button name="LED" value="OFF2" type="submit">LED OFF</button>
</form>
</html>
"""

#Setup PINS
LED16 = machine.Pin(16, machine.Pin.OUT)
LED2 = machine.Pin(2, machine.Pin.OUT)

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
    response = html
    conn.send(response)
    conn.close()
