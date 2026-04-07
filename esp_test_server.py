# KI generierter Testserver für ESP Comms.
import socket
import struct
import threading
import time
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
    "udp_x": 0, 
    "udp_y": 0, 
    "udp_mode": 0, 
    "udp_count": 0,
    "udp_last_len": 0,
    "tcp_last_msg": "-", 
    "tcp_conn": "Warte auf Verbindung..."
}

def generate_table():
    table = Table(title="[bold green]ESP32 Remote Monitor[/bold green]")
    table.add_column("Kategorie", style="cyan")
    table.add_column("Status / Wert", style="magenta")
    
    table.add_row("UDP Pakete", str(data_display["udp_count"]))
    table.add_row("Letzte Länge", f"{data_display['udp_last_len']} Bytes")
    table.add_row("Joystick X", str(data_display["udp_x"]))
    table.add_row("Joystick Y", str(data_display["udp_y"]))
    table.add_row("Modus (UDP)", str(data_display["udp_mode"]))
    
    table.add_section()
    
    table.add_row("TCP Verbindung", data_display["tcp_conn"])
    table.add_row("Letzte Nachricht", f"[yellow]{data_display['tcp_last_msg']}[/yellow]")
    
    return table

def udp_server():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((IP, UDP_PORT))
    while True:
        try:
            data, addr = sock.recvfrom(1024)
            data_display["udp_count"] += 1
            data_display["udp_last_len"] = len(data)
            
            # Unpacking 5 Bytes: 2x uint16 (H), 1x uint8 (B)
            if len(data) >= 5:
                x, y, mode = struct.unpack('<HHB', data[:5])
                data_display["udp_x"] = x
                data_display["udp_y"] = y
                data_display["udp_mode"] = mode
        except Exception as e:
            data_display["tcp_last_msg"] = f"UDP Err: {e}"

def tcp_server():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((IP, TCP_PORT))
    sock.listen(1)
    
    while True:
        conn, addr = sock.accept()
        data_display["tcp_conn"] = f"Verbunden: {addr[0]}"
        while True:
            try:
                data = conn.recv(1024)
                if not data: break
                msg = data.decode('utf-8', errors='ignore').strip()
                if msg:
                    data_display["tcp_last_msg"] = msg
            except:
                break
        conn.close()
        data_display["tcp_conn"] = "Getrennt / Warte..."

# Start Threads
threading.Thread(target=udp_server, daemon=True).start()
threading.Thread(target=tcp_server, daemon=True).start()

# Start Live View
try:
    with Live(generate_table(), refresh_per_second=10) as live:
        while True:
            time.sleep(0.1)
            live.update(generate_table())
except KeyboardInterrupt:
    console.print("\n[bold red]Server gestoppt.[/bold red]")