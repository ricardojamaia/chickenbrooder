#!/usr/bin/env python3
"""
ESP32 Log Viewer
Real-time log monitoring for ESP32 devices with filtering, colors, and statistics
"""

import socket
import sys
import argparse
import re
from datetime import datetime
from collections import defaultdict
import threading
import time

# ANSI color codes
class Colors:
    # Log levels
    ERROR = '\033[91m'      # Bright Red
    WARN = '\033[93m'       # Bright Yellow
    INFO = '\033[92m'       # Bright Green
    DEBUG = '\033[96m'      # Bright Cyan
    VERBOSE = '\033[97m'    # Bright White
    
    # Components
    COMPONENT = '\033[95m'  # Bright Magenta
    TIMESTAMP = '\033[90m'  # Gray
    
    # UI
    BOLD = '\033[1m'
    RESET = '\033[0m'
    DIM = '\033[2m'

# ESP32 log level mapping
LOG_LEVELS = {
    'E': ('ERROR', Colors.ERROR),
    'W': ('WARN', Colors.WARN),
    'I': ('INFO', Colors.INFO),
    'D': ('DEBUG', Colors.DEBUG),
    'V': ('VERBOSE', Colors.VERBOSE),
}

LEVEL_PRIORITY = {
    'ERROR': 1,
    'WARN': 2,
    'INFO': 3,
    'DEBUG': 4,
    'VERBOSE': 5,
}

class LogStats:
    """Track logging statistics"""
    def __init__(self):
        self.total_messages = 0
        self.by_level = defaultdict(int)
        self.by_component = defaultdict(int)
        self.errors_last_minute = []
        self.start_time = time.time()
        self.lock = threading.Lock()
    
    def add(self, level, component):
        with self.lock:
            self.total_messages += 1
            self.by_level[level] += 1
            self.by_component[component] += 1
            
            if level == 'ERROR':
                self.errors_last_minute.append(time.time())
                # Keep only last minute
                cutoff = time.time() - 60
                self.errors_last_minute = [t for t in self.errors_last_minute if t > cutoff]
    
    def get_summary(self):
        with self.lock:
            runtime = time.time() - self.start_time
            return {
                'total': self.total_messages,
                'runtime': runtime,
                'rate': self.total_messages / runtime if runtime > 0 else 0,
                'by_level': dict(self.by_level),
                'by_component': dict(self.by_component),
                'errors_per_min': len(self.errors_last_minute)
            }

def parse_esp32_log(line):
    """
    Parse ESP32 log format: LEVEL (timestamp) component: message
    Example: I (1234) wifi: STA_CONNECTED
    """
    # ESP32 format: LEVEL (timestamp) component: message
    pattern = r'^([EWDIV])\s*\((\d+)\)\s*([^:]+):\s*(.+)$'
    match = re.match(pattern, line)
    
    if match:
        level_char, timestamp, component, message = match.groups()
        level, _ = LOG_LEVELS.get(level_char, ('INFO', Colors.INFO))
        return {
            'level': level,
            'timestamp': int(timestamp),
            'component': component.strip(),
            'message': message.strip(),
            'raw': line
        }
    
    # Fallback: treat as raw message
    return {
        'level': 'INFO',
        'timestamp': None,
        'component': 'unknown',
        'message': line.strip(),
        'raw': line
    }

def format_timestamp(millis):
    """Convert milliseconds to HH:MM:SS.mmm format"""
    if millis is None:
        return "??:??:??.???"
    
    seconds = millis / 1000
    hours = int(seconds // 3600)
    minutes = int((seconds % 3600) // 60)
    secs = int(seconds % 60)
    ms = int(millis % 1000)
    
    if hours > 0:
        return f"{hours:02d}:{minutes:02d}:{secs:02d}.{ms:03d}"
    else:
        return f"{minutes:02d}:{secs:02d}.{ms:03d}"

def format_log_line(log, use_color=True, show_timestamp=True, show_component=True):
    """Format a log line for display"""
    parts = []
    
    # Timestamp
    if show_timestamp and log['timestamp'] is not None:
        ts = format_timestamp(log['timestamp'])
        if use_color:
            parts.append(f"{Colors.TIMESTAMP}[{ts}]{Colors.RESET}")
        else:
            parts.append(f"[{ts}]")
    
    # Log level
    level = log['level']
    if use_color:
        color = LOG_LEVELS.get(level[0], ('INFO', Colors.INFO))[1]
        parts.append(f"{color}[{level:7s}]{Colors.RESET}")
    else:
        parts.append(f"[{level:7s}]")
    
    # Component
    if show_component and log['component'] != 'unknown':
        if use_color:
            parts.append(f"{Colors.COMPONENT}[{log['component']}]{Colors.RESET}")
        else:
            parts.append(f"[{log['component']}]")
    
    # Message
    parts.append(log['message'])
    
    return ' '.join(parts)

def should_display(log, args):
    """Determine if log should be displayed based on filters"""
    # Level filter
    if args.level:
        min_priority = LEVEL_PRIORITY.get(args.level, 5)
        log_priority = LEVEL_PRIORITY.get(log['level'], 5)
        if log_priority > min_priority:
            return False
    
    # Text filter
    if args.filter and args.filter.lower() not in log['message'].lower():
        return False
    
    # Component filter
    if args.component and log['component'] != args.component:
        return False
    
    # Exclude components
    if args.exclude and log['component'] in args.exclude:
        return False
    
    return True

def print_header(args):
    """Print header with listening info"""
    print(f"{Colors.BOLD}{'='*80}{Colors.RESET}")
    print(f"{Colors.BOLD}ESP32 Log Viewer{Colors.RESET}")
    print(f"Listening on UDP {args.bind}:{args.port}")
    
    if args.level:
        print(f"Min level: {args.level}")
    if args.filter:
        print(f"Filter: '{args.filter}'")
    if args.component:
        print(f"Component: {args.component}")
    if args.exclude:
        print(f"Excluding: {', '.join(args.exclude)}")
    if args.save:
        print(f"Saving to: {args.save}")
    
    print(f"{Colors.BOLD}{'='*80}{Colors.RESET}")
    print()

def print_stats(stats):
    """Print statistics summary"""
    summary = stats.get_summary()
    
    print(f"\n{Colors.BOLD}{'='*80}{Colors.RESET}")
    print(f"{Colors.BOLD}Statistics:{Colors.RESET}")
    print(f"  Total messages: {summary['total']}")
    print(f"  Runtime: {summary['runtime']:.1f}s")
    print(f"  Rate: {summary['rate']:.1f} msg/s")
    print(f"  Errors/min: {summary['errors_per_min']}")
    
    if summary['by_level']:
        print(f"\n{Colors.BOLD}By Level:{Colors.RESET}")
        for level in ['ERROR', 'WARN', 'INFO', 'DEBUG', 'VERBOSE']:
            count = summary['by_level'].get(level, 0)
            if count > 0:
                color = LOG_LEVELS.get(level[0], ('INFO', Colors.INFO))[1]
                print(f"  {color}{level:8s}{Colors.RESET}: {count:6d}")
    
    if summary['by_component']:
        print(f"\n{Colors.BOLD}Top Components:{Colors.RESET}")
        sorted_components = sorted(summary['by_component'].items(), 
                                   key=lambda x: x[1], reverse=True)[:10]
        for component, count in sorted_components:
            print(f"  {component:20s}: {count:6d}")
    
    print(f"{Colors.BOLD}{'='*80}{Colors.RESET}\n")

def stats_thread_func(stats, interval=60):
    """Periodically print statistics"""
    while True:
        time.sleep(interval)
        print_stats(stats)

def main():
    parser = argparse.ArgumentParser(
        description='ESP32 Log Viewer - Real-time log monitoring',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                           # Basic usage
  %(prog)s -p 6666                   # Specific port
  %(prog)s --level WARN              # Only warnings and errors
  %(prog)s --filter "temperature"    # Filter by text
  %(prog)s --component wifi          # Only WiFi logs
  %(prog)s --exclude wifi,phy_init   # Exclude components
  %(prog)s --save logs.txt           # Save to file
  %(prog)s --no-color                # Disable colors
  %(prog)s --stats 30                # Show stats every 30s
        """
    )
    
    # Connection
    parser.add_argument('-p', '--port', type=int, default=6666,
                       help='UDP port to listen on (default: 6666)')
    parser.add_argument('-b', '--bind', type=str, default='0.0.0.0',
                       help='IP address to bind to (default: 0.0.0.0)')
    
    # Filtering
    parser.add_argument('--level', type=str, 
                       choices=['ERROR', 'WARN', 'INFO', 'DEBUG', 'VERBOSE'],
                       help='Minimum log level to display')
    parser.add_argument('--filter', type=str,
                       help='Only show logs containing this text')
    parser.add_argument('--component', type=str,
                       help='Only show logs from this component')
    parser.add_argument('--exclude', type=str,
                       help='Exclude components (comma-separated)')
    
    # Display
    parser.add_argument('--no-color', action='store_true',
                       help='Disable colored output')
    parser.add_argument('--no-timestamp', action='store_true',
                       help='Hide timestamps')
    parser.add_argument('--no-component', action='store_true',
                       help='Hide component names')
    parser.add_argument('--raw', action='store_true',
                       help='Show raw log lines (no parsing)')
    
    # Output
    parser.add_argument('--save', type=str, metavar='FILE',
                       help='Save logs to file')
    parser.add_argument('--stats', type=int, metavar='SECONDS',
                       help='Show statistics every N seconds')
    
    args = parser.parse_args()
    
    # Parse exclude list
    if args.exclude:
        args.exclude = [c.strip() for c in args.exclude.split(',')]
    else:
        args.exclude = []
    
    # Create socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        sock.bind((args.bind, args.port))
    except OSError as e:
        print(f"{Colors.ERROR}Error: Cannot bind to {args.bind}:{args.port}{Colors.RESET}")
        print(f"  {e}")
        print(f"\nTry a different port with -p <port>")
        sys.exit(1)
    
    # Open log file if specified
    log_file = None
    if args.save:
        try:
            log_file = open(args.save, 'a')
            print(f"Saving logs to: {args.save}")
        except IOError as e:
            print(f"{Colors.ERROR}Error: Cannot open log file {args.save}{Colors.RESET}")
            print(f"  {e}")
            sys.exit(1)
    
    # Statistics
    stats = LogStats()
    
    # Start stats thread if requested
    if args.stats:
        stats_thread = threading.Thread(target=stats_thread_func, 
                                        args=(stats, args.stats), 
                                        daemon=True)
        stats_thread.start()
    
    # Print header
    print_header(args)
    
    try:
        while True:
            # Receive data
            data, addr = sock.recvfrom(4096)
            
            try:
                message = data.decode('utf-8').strip()
            except UnicodeDecodeError:
                print(f"{Colors.WARN}[WARNING] Received invalid UTF-8 data from {addr}{Colors.RESET}")
                continue
            
            # Split into lines (buffer might contain multiple log lines)
            lines = message.split('\n')
            
            for line in lines:
                if not line:
                    continue
                
                # Raw mode: just print the line
                if args.raw:
                    print(line)
                    if log_file:
                        log_file.write(f"{datetime.now().isoformat()} {line}\n")
                        log_file.flush()
                    continue
                
                # Parse log line
                log = parse_esp32_log(line)
                
                # Update statistics
                stats.add(log['level'], log['component'])
                
                # Apply filters
                if not should_display(log, args):
                    continue
                
                # Format and display
                formatted = format_log_line(
                    log, 
                    use_color=not args.no_color,
                    show_timestamp=not args.no_timestamp,
                    show_component=not args.no_component
                )
                print(formatted)
                
                # Save to file if specified (without colors)
                if log_file:
                    raw_formatted = format_log_line(
                        log,
                        use_color=False,
                        show_timestamp=True,
                        show_component=True
                    )
                    log_file.write(f"{datetime.now().isoformat()} {raw_formatted}\n")
                    log_file.flush()
    
    except KeyboardInterrupt:
        print(f"\n{Colors.BOLD}{'='*80}{Colors.RESET}")
        print(f"{Colors.BOLD}Stopped by user{Colors.RESET}")
        
        # Final statistics
        if not args.stats:
            print_stats(stats)
    
    finally:
        sock.close()
        if log_file:
            log_file.close()
            print(f"Logs saved to: {args.save}")

if __name__ == '__main__':
    main()
