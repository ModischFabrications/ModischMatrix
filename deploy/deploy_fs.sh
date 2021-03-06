#!/usr/bin/env bash

# exit when any command fails
set -e

echo "[MPC] --- Deploying to target device file system --- "

command -v pio >/dev/null 2>&1 || { echo >&2 "pio can't be found. Add to PATH!"; exit 1; }

echo "[MPC] clearing /data"
rm -rf ./data/
mkdir ./data/

# -- better alternative, but I'm unable to get it running
# echo "[MPC] copying /website to /data, minifying in the process"
# minify -s -o ./data ./website

echo "[MPC] copying /website to /data"
cp -rf ./website/* ./data/
# ---

echo "[MPC] converting to *.gz"
gzip -rf ./data*

pio run --target uploadfs || exit 4

echo "[MPC] --- Finished successfully, don't forget to upload your program --- "
exit 0
