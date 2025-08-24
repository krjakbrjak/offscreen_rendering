#!/usr/bin/env bash

pkill Xvfb || true

Xvfb :99 -screen 0 1024x768x24 2>/dev/null &
XVFB_PID=$!
export DISPLAY=:99
export QT_OPENGL=desktop

# Trap all common signals to ensure Xvfb is killed
trap "kill $XVFB_PID" EXIT SIGHUP SIGINT SIGTERM SIGQUIT

QT_QPA_PLATFORM=xcb offscreen_sample -o /output/image.png
