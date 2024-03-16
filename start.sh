#!/bin/sh
#cd "$(dirname "$0")"
#SCRIPT_DIR="$(pwd)"
#echo "Script directory: $SCRIPT_DIR"
terminal=$(sudo fgconsole)
sudo chvt 3
cd "$(dirname "$0")"
./mandelbrodt
sudo chvt $terminal
