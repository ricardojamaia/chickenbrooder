# Dockerfile for Brooder Log Monitor
FROM python:3.13-slim

# Install dependencies for dashboard (curses)
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ncurses-term \
    && rm -rf /var/lib/apt/lists/*

# Create app directory
WORKDIR /app

# Copy logger scripts
COPY brooder_log_monitor.py /app/

# Make scripts executable
RUN chmod +x /app/brooder_log_monitor.py

# Create directory for log files
RUN mkdir -p /app/logs

# Default command (can be overridden)
CMD ["python3", "/app/brooder_log_monitor.py", "-p", "6666", "-b", "0.0.0.0"]
