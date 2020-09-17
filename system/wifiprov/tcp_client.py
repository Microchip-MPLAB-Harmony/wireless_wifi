import socket
import sys


HOST = sys.argv[1]
PORT = int(sys.argv[2])
HOMEAP_AUTH = sys.argv[3]
HOMEAP_SSID = sys.argv[4]
HOMEAP_PWD = sys.argv[5]
AP_AUTH = "4"
AP_SSID = "DEMO_AP_SOFTAP"
AP_PWD = "password"

data = "\
{\"mode\" : 0,\"save_config\" : 1,\"countrycode\":\"GEN\",\
\"STA\":{\"ch\":0,\"auto\":1,\"auth\":"+HOMEAP_AUTH+",\"SSID\":"+"\""+str(HOMEAP_SSID)+"\""+",\"PWD\":"+"\""+str(HOMEAP_PWD)+"\""+"},\
\"AP\":{\"ch\":2,\"ssidv\":1,\"auth\":"+AP_AUTH+",\"SSID\":"+"\""+str(AP_SSID)+"\""+",\"PWD\":"+"\""+str(AP_PWD)+"\""+"}}"

# Create a socket (SOCK_STREAM means a TCP socket)
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    # Connect to server and send data
    
    #print("Sent:     {}".format(data))
    sock.connect((HOST, PORT))
    sock.sendall(bytes(data + "\n", "utf-8"))

    # Receive data from the server and shut down
    #received = str(sock.recv(1024), "utf-8")

print("Sent:     {}".format(data))
#print("Received: {}".format(received))
print ("closing socket")
sock.close()
