import socket
import json

HOST = ''  # all interfaces
PORT = 5000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind((HOST, PORT))
server.listen(1)
print("Server başladı, gözləyir...")

while True:
    conn, addr = server.accept()
    print("Qoşuldu:", addr)
    with conn:
        buffer = b''
        while True:
            try:
                data = conn.recv(2048)
                if not data:
                    break
                buffer += data
                # assume newline delimited JSON messages
                while b'\n' in buffer:
                    line, buffer = buffer.split(b'\n', 1)
                    try:
                        payload = json.loads(line.decode())
                        print("Məlumat:", payload)
                    except Exception as e:
                        print("JSON parse xətası:", e, " Raw:", line)
            except ConnectionResetError:
                break
    print("Conn closed")
