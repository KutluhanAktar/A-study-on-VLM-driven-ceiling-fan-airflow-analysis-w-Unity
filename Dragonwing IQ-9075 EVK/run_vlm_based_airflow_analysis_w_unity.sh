#!/bin/bash

# Initiate the Qualcomm LLM/VLM Docker container to run the installed VLM models.
gnome-terminal --title="Qualcomm LLM/VLM Container [Docker]" -- bash -c "
  docker-compose -f docker-compose-qcs9100-ubuntu.yaml up
  exec bash
"

# Initiate the airflow analysis data hub web application running a Flask server, transferring RPLIDAR A1 scan points to the Unity airflow simulator application, and handling VLM output generation on the provided Docker container.
gnome-terminal --title="Data Hub Web Application [Flask]" -- bash -c "
  cd unity_lidar_based_ceiling_fan_airflow_analysis || exit 1;
  source venv/bin/activate    
  python3 app.py
  exec bash
"

# Initiate the Unity airflow simulator application (x86_64) via Box64, utilizing RPLIDAR A1 scan points to simulate air current alterations based on obstacles.
gnome-terminal --title="Unity Airflow Simulator [x86_64]" -- bash -c "
  cd unity_lidar_based_ceiling_fan_airflow_analysis/Unity_Airflow_Simulation_x86_64 || exit 1;
  chmod +x airflow_simulator.x86_64
  DISPLAY=:0 box64 ./airflow_simulator.x86_64 -force-gles32 -screen-fullscreen 0 -screen-width 1280 -screen-height 720 -logfile
  exec bash
"