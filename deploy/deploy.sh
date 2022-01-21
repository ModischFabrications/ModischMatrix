#!/usr/bin/env bash

# exit when any command fails
set -e

echo "[MPC] --- Deploying the whole project --- "

command -v pio >/dev/null 2>&1 || { echo >&2 "pio can't be found. Add to PATH!"; exit 1; }

echo "[MPC] Uploading file system"
./deploy/deploy_fs.sh || exit $?

echo "[MPC] Uploading program code and monitoring serial"
pio run --target upload --target monitor || exit 3;

echo "[MPC] --- Finished successfully --- "
exit 0
