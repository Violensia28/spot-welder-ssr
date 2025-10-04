#!/bin/bash
echo "=== Setting up Codespaces for ESP32 ==="

# Install PlatformIO
pip install platformio

# Check installation
pio --version

echo "✅ Setup complete! Run 'pio run' in firmware folder to build."
