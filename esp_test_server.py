# KI generierter Testserver für ESP Comms.
import socket
import struct
import threading
import time
import random
from rich.console import Console
from rich.table import Table
from rich.live import Live

# --- KONFIGURATION ---
UDP_PORT = 9005
TCP_PORT = 9006
IP = "0.0.0.0" 

console = Console()

# Globale Daten für die Live-Anzeige
data_display = {
    "udp_x": 0, "udp_y": 0, "udp_mode": 0, 
    "udp_count_total": 0,
    "udp_pps": 0,  # Packets Per Second
    "tcp_last_msg_sent": "-", "tcp_last_msg_rec": "-", 
    "tcp_conn": "Warte auf Verbindung..."
}

# Hilfsvariable für die PPS-Berechnung
_udp_counter_temp = 0
_counter_lock = threading.Lock()

# Variable für die aktive TCP-Verbindung
active_tcp_conn = None

def generate_table():
    table = Table(title="[bold green]ESP32 Bi-Directional Monitor[/bold green]")
    table.add_column("Kategorie", style="cyan")
    table.add_column("Status / Wert", style="magenta")
    
    # UDP Sektion
    table.add_row("UDP Pakete (Gesamt)", str(data_display["udp_count_total"]))
    pps_color = "green" if data_display["udp_pps"] > 0 else "red"
    table.add_row("UDP Rate (PPS)", f"[{pps_color}]{data_display['udp_pps']} pkts/s[/{pps_color}]")
    table.add_row("Joystick X | Y", f"{data_display['udp_x']} | {data_display['udp_y']}")
    table.add_row("Modus (UDP)", str(data_display["udp_mode"]))
    
    table.add_section()
    
    # TCP Sektion
    table.add_row("TCP Verbindung", data_display["tcp_conn"])
    table.add_row("Empfangen (vom ESP)", f"[yellow]{data_display['tcp_last_msg_rec']}[/yellow]")
    table.add_row("Gesendet (an ESP)", f"[green]{data_display['tcp_last_msg_sent']}[/green]")
    
    return table

def udp_server():
    global _udp_counter_temp
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((IP, UDP_PORT))
    while True:
        try:
            data, addr = sock.recvfrom(1024)
            with _counter_lock:
                data_display["udp_count_total"] += 1
                _udp_counter_temp += 1
            
            if len(data) >= 5:
                x, y, mode = struct.unpack('<HHB', data[:5])
                data_display["udp_x"] = x
                data_display["udp_y"] = y
                data_display["udp_mode"] = mode
        except: pass

def pps_calculator():
    """Berechnet jede Sekunde die Pakete pro Sekunde"""
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
            except: break
        active_tcp_conn = None
        conn.close()
        data_display["tcp_conn"] = "Getrennt / Warte..."

def sender_thread():
    """ Sendet alle 250ms Daten an den ESP32 """
    while True:
        if active_tcp_conn:
            try:
                batt = round(random.uniform(3.0, 6.0), 2)
                lenk = round(random.uniform(0.0, 3.3), 2)
                
                msg_batt = f"BATT:{batt}\n"
                msg_lenk = f"LENK:{lenk}\n"
                
                active_tcp_conn.sendall(msg_batt.encode())
                active_tcp_conn.sendall(msg_lenk.encode())
                
                data_display["tcp_last_msg_sent"] = f"B:{batt}V, L:{lenk}"
            except:
                pass
        time.sleep(0.25)

# Start Threads
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
    console.print("\n[bold red]Server gestoppt.[/bold red]")