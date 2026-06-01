# KI generiertes Dokument! Nur zu Testzwecken
import socket
import struct
import threading
import time
import random
import errno
from rich.console import Console
from rich.table import Table
from rich.live import Live

# --- KONFIGURATION ---
UDP_PORT = 9005
TCP_PORT = 9006
IP = "0.0.0.0" 

console = Console()

data_display = {
    "udp_x": 0, 
    "udp_y": 0, 
    "udp_count_total": 0,
    "udp_pps": 0,
    "tcp_last_msg_sent": "-", 
    "tcp_last_msg_rec": "-", 
    "tcp_conn": "Warte auf Verbindung..."
}

_udp_counter_temp = 0
_counter_lock = threading.Lock()
active_tcp_conn = None

def generate_table():
    table = Table(title="[bold green]ESP32 Control Monitor[/bold green]")
    table.add_column("Kategorie", style="cyan")
    table.add_column("Status / Wert", style="magenta")
    
    table.add_row("UDP Pakete (Gesamt)", str(data_display["udp_count_total"]))
    pps_color = "green" if data_display["udp_pps"] > 0 else "red"
    table.add_row("UDP Rate (PPS)", f"[{pps_color}]{data_display['udp_pps']} pkts/s[/{pps_color}]")
    table.add_row("Joystick X | Y", f"[bold white]{data_display['udp_x']} | {data_display['udp_y']}[/bold white]")
    
    table.add_section()
    
    table.add_row("TCP Verbindung", data_display["tcp_conn"])
    table.add_row("Empfangen (ESP)", f"[yellow]{data_display['tcp_last_msg_rec']}[/yellow]")
    table.add_row("Gesendet (an ESP)", f"[green]{data_display['tcp_last_msg_sent']}[/green]")
    
    return table

def udp_server():
    global _udp_counter_temp
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((IP, UDP_PORT))
    
    while True:
        try:
            data, addr = sock.recvfrom(1024)
            with _counter_lock:
                data_display["udp_count_total"] += 1
                _udp_counter_temp += 1
            
            # 4 Bytes -> <HH (2x uint16)
            if len(data) >= 4:
                x, y = struct.unpack('<HH', data[:4])
                data_display["udp_x"] = x
                data_display["udp_y"] = y
        except OSError as e:
            if e.errno == errno.ECONNRESET:
                continue
        except: 
            pass

def pps_calculator():
    global _udp_counter_temp
    while True:
        time.sleep(1.0)
        with _counter_lock:
            data_display["udp_pps"] = _udp_counter_temp
            _udp_counter_temp = 0

def tcp_handler():
    global active_tcp_conn
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((IP, TCP_PORT))
    sock.listen(1)
    
    while True:
        conn, addr = sock.accept()
        active_tcp_conn = conn
        data_display["tcp_conn"] = f"Verbunden: {addr[0]}"
        while True:
            try:
                data = conn.recv(1024)
                if not data: break
                msg = data.decode('utf-8', errors='ignore').strip()
                if msg: data_display["tcp_last_msg_rec"] = msg
            except: 
                break
        active_tcp_conn = None
        conn.close()
        data_display["tcp_conn"] = "Getrennt / Warte..."

def sender_thread():
    """
    Diese Funktion sendet Daten im Format, das die C++ Funktion 'handleIncomingTCP' 
    erwartet: 'BATT:value\\n' oder 'LENK:value\\n'
    """
    global active_tcp_conn
    while True:
        if active_tcp_conn:
            try:
                # Beispiel: Abwechselnd Batterie und Lenkung senden
                batt = round(random.uniform(3.5, 4.2), 2)
                lenk = round(random.uniform(0.0, 2.4), 2)
                ampere = round(random.uniform(0.1, 2.0), 2)
                abstandHinten = random.randint(100,20000)
                abstandVorne = random.randint(100,20000)

                # Formatierung gemäß C++: startsWith("BATT:") und readStringUntil('\n')
                msg_batt = f"SDATA:BATT:{batt}\n"
                active_tcp_conn.sendall(msg_batt.encode())
                
                #time.sleep(0.1)
                
                msg_lenk = f"SDATA:LENK:{lenk}\n"
                active_tcp_conn.sendall(msg_lenk.encode())
                
                msg_amp = f"SDATA:AMPR:{ampere}\n"
                active_tcp_conn.sendall(msg_amp.encode())

                msg_absH = f"SDATA:ABSH:{abstandHinten}\n"
                active_tcp_conn.sendall(msg_absH.encode())

                msg_absV = f"SDATA:ABSV:{abstandVorne}\n"
                active_tcp_conn.sendall(msg_absV.encode())

                data_display["tcp_last_msg_sent"] = f"B:{batt}V, L:{lenk}, A:{ampere}, AH:{abstandHinten}, AV:{abstandVorne}"
            except:
                active_tcp_conn = None
        time.sleep(0.5)

# Threads starten
threading.Thread(target=udp_server, daemon=True).start()
threading.Thread(target=pps_calculator, daemon=True).start()
threading.Thread(target=tcp_handler, daemon=True).start()
threading.Thread(target=sender_thread, daemon=True).start()

try:
    with Live(generate_table(), refresh_per_second=10) as live:
        while True:
            time.sleep(0.1)
            live.update(generate_table())
except KeyboardInterrupt:
    print("\nServer gestoppt.")