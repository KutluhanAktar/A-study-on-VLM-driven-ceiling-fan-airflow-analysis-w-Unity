> I developed my portfolio website — kutluhanaktar.com — from scratch and decided to migrate all of my codebase to GitHub to provide a simple and straightforward experimentation or replication method for my proof-of-concept projects. Nonetheless, since I focused on demonstrating my thought process and experiment results thoroughly in the written tutorial format, including but not limited to PCB and 3D model designing steps, I could not provide enough information to fully explain my concepts and instructions via GitHub descriptions only. Thus, I highly recommend inspecting the associated project tutorials and videos on my portfolio website or other platforms (maker communities), i.e., Hackster, Hackaday, and Instructables.

# Original Project Publication Date

**September 21, 2026**

# Description

**Utilizing VLMs to determine dual fan configurations for optimum cooling based on LiDAR-oriented real-time airflow simulations on Unity.**

As consecutive heatwaves were passing through my region at the beginning of Summer, I had been intrigued to develop a research project regarding AI-oriented HVAC simulations to increase real-world cooling efficiency both energy-wise and coverage-wise. Although there were meticulous research papers, even commercial firms providing services to analyze the cooling efficiency of air conditioning units by thorough HVAC simulations, I decided to focus on developing my project around ceiling fans. Even though ceiling fans are not as popular as air conditioning units nowadays, I had noticed there were a plethora of ceiling fan installations in my city, especially in historic shopping districts, bazaars, and government establishments. Thus, I decided to focus my research on simulating ceiling fan-induced airflow and deriving optimal system settings from this simulation for efficient cooling.

Considering I wanted to see the impact of AI-oriented simulations on deriving optimum configurations for a real-world ceiling fan system, I decided to design a dual ceiling fan mechanism from scratch, enabling me to control all experiment parameters while building my ceiling fan airflow simulation program. Though a little untraditional, I decided to base my dual ceiling fan design on a differential bevel gear mechanism to be able to move both fans 180° vertically and 360° horizontally, leading to a lot more complicated but comprehensive simulation-driven airflow analysis and real-world cooling efficiency improvements.

After deciding on the bare bones of the dual ceiling fan mechanism, I needed a method to map the surroundings of operating ceiling fans since a precise airflow simulation must include real-time obstacle updates to analyze airflow path fluctuations triggered by solid surfaces such as walls, open doors, windows, or even moving people. Generally, HVAC simulations for industrial settings utilize highly sensitive 3D LiDARs with SLAM or sensor fusion techniques to generate accurate heat maps. Nonetheless, since I wanted to focus on fan-produced indoor airflow analysis for much simpler settings, I decided to utilize an RPLIDAR A1M8-R6 LiDAR scanner and turn its 2D scan points into a simple 3D obstacle map in my airflow simulation program, enabling fan-produced air displacement routes (trails) to bounce off of their surfaces.

In the spirit of making the ceiling airflow simulation program easily accessible and open-source, I decided to utilize Unity to develop all airflow analysis features, including the obstacle generation from 2D LiDAR scan points, without utilizing any third-party or paid Unity services. Although Unity might seem like an odd choice since it is a cross-platform, performance-focused 3D game engine, I wanted to capitalize on its advanced scene and animation rendering features in order to simulate accurate airflow fluctuations and track fan-produced air displacement routes (trails). In this regard, I was able to obtain animation-based frames individually for each pre-defined ceiling fan configuration (angles, fan strength, etc.), enabling me to conduct airflow analysis by taking instances from a long animation (video) sequence.

To derive optimal ceiling fan configurations from selected airflow simulation frames so as to achieve real-world efficient cooling, I decided to utilize vision–language models (VLMs) running locally. As I wanted to demonstrate the performance of open-source vision–language models without tailoring model output for a specific ceiling fan-installed space, I did not train any models beforehand. In this regard, I was able to see whether off-the-shelf open-weight vision–language models could be deployed to obtain airflow simulation-based ceiling fan configurations to achieve energy-efficient and optimal cooling of a real-world closed environment. I thoroughly discussed my findings in the following tutorial, but I can briefly state that the vision-language models (Qwen) I employed performed well and provided effective fan configurations based on the obstacle map derived from 2D LiDAR scan points.

- Qwen2.5-VL-7B-Instruct
- Qwen3-VL-8B-Instruct

As I wanted to calculate 2D LiDAR scan points, execute the Unity ceiling fan-produced airflow simulation application, and run vision-language models on the same device to achieve fast and reliable simulation-based airflow analysis results, I decided to utilize the Qualcomm Dragonwing IQ-9075 Evaluation Kit (EVK), which is a compact development platform optimized for industrial IoT, Edge AI, and robotics applications involving heavy workloads. Thanks to the IQ-9075's dedicated AI Engine (Hexagon NPU) for on-device AI, I was able to execute operations separately to achieve excellent calculation, simulation, and inference performance, which is still bemusing to me considering the compactness of the EVK :)

To provide a user-friendly interface to select Unity-generated airflow simulation frames and communicate with the provided vision-language models, I developed a simple web application running on Flask, behaving as the intermediary between the Unity airflow simulation application and the local LLM/VLM server — Qualcomm Docker Compose container.

- Octa-core Kryo CPU ➡️ LiDAR scan points and other calculations
- Adreno GPU (with CPU) ➡️ Unity airflow simulation application
- Hexagon NPU ➡️ Vision-language models (VLMs)

Even though I obtained great airflow analysis results from vision-language models, I did not want to create a direct pipeline between the VLM outputs and ceiling fan mechanism controls, considering potential hallucinations and false positives. Furthermore, I wanted to enable the user to access all of the ceiling fan mechanism controls without impediments by not concealing fan configurations behind VLM outputs, including the dual light bulbs I added, since the ceiling fan with an on-device light source is a widely preferred household good. Since I did not want to create a secondary web-based interface for the fan configurations, I decided to develop a BLE-enabled mobile (Android) application, embedding the previously mentioned intermediary (Flask) web application. In this regard, all of the ceiling fan mechanism controls and VLM interactions can be inspected and adjusted on a single mobile application. The mobile application allows the user to:

- ✅ adjust ceiling fan vertical (180°) and horizontal (360°) angles for both fans,
- ✅ adjust ceiling fan strength magnitudes from 1 to 4,
- ✅ control light source states (on or off),
- ✅ observe current vertical angles and horizontal axis alignment states,
- ✅ connect to the intermediary web application to interact with the provided VLMs.

To enable the mobile application to manage ceiling fan mechanism configurations over BLE without putting any additional stress on the compute power of the IQ-9075 EVK, I decided to utilize a separate board, Arduino Nano 33 BLE, to control all mechanical features. I designed a unique 4-layer PCB to build a compact fan control unit based on the Nano 33 BLE, which provides all the required connections for the external power source, stepper motor drivers, DC motor drivers, lights, rotary encoders, optical speed modules, etc., to execute all of the ceiling fan mechanism features noted above.

As a proof-of-concept research project, I did not focus on building a directly installable ceiling fan system ready to cool an enclosed space like a commercially available product. Instead, I focused on developing a VLM-assisted ceiling fan mechanism that showcases intricate circuit connections, wiring, device structure, and a simulation-based airflow analysis pipeline. Thus, I immensely scaled down the intertwined VLM-assisted ceiling fan mechanism I envisioned and highlighted the fan elevation by placing the whole mechanism on an IKEA LACK table, which enabled me to create a highly detailed digital twin of the ceiling fan system in Fusion 360 and integrate this exquisite 3D model into the Unity Editor to develop an utterly accurate Unity airflow simulation application, estimating airflow fluctuations and air displacement routes (trails) based on the fan elevation and the LiDAR-detected obstacles.

# Inspect the project tutorial on:

- **[kutluhanaktar.com](https://www.kutluhanaktar.com/projects/A_study_on_VLM_driven_ceiling_fan_airflow_analysis_w_Unity/)**

<img width="512" height="278" alt="model_design_0" src="https://github.com/user-attachments/assets/ba13f575-92b9-4907-9318-b594169cc46e" /><br>

<img width="512" height="278" alt="model_design_3" src="https://github.com/user-attachments/assets/2c14f8fb-eb48-4347-ba68-fb8e6782e8ad" /><br>

<img width="512" height="278" alt="model_design_12_fan_mechanism" src="https://github.com/user-attachments/assets/a0a49781-13c2-44d8-b53e-9af976cfc1d4" /><br>

<img width="512" height="278" alt="model_design_35_fan_mechanism" src="https://github.com/user-attachments/assets/0d370aae-1279-4ba2-ba90-1371f9765c7d" /><br>

<img width="512" height="278" alt="model_design_36_fan_mechanism" src="https://github.com/user-attachments/assets/c5012c94-270e-4401-900d-72347f852a00" /><br>

<img width="384" height="512" alt="assembly_68_fan_mechanism_diff" src="https://github.com/user-attachments/assets/4f8db34f-82ec-4bc6-8cca-4c81f23a1346" /><br>

<img width="512" height="278" alt="unity_project_set_up_11_scale_color_mat" src="https://github.com/user-attachments/assets/d46fc3a0-7d83-4cd5-b866-696f66447937" /><br>

<img width="512" height="278" alt="unity_project_set_up_50_rplidar_a1_link_init_test" src="https://github.com/user-attachments/assets/903ba657-9f63-457f-b173-122c47f9a7bb" /><br>

<img width="512" height="278" alt="unity_project_set_up_53_lidar_trail_tests" src="https://github.com/user-attachments/assets/ec498d54-3841-4536-9ff6-6300fed0352a" /><br>

<img width="512" height="287" alt="iq9_software_setup_42_run_unity_app_box64" src="https://github.com/user-attachments/assets/6340fdd6-55c9-4556-a1bb-bd1955291341" /><br>

<img width="512" height="287" alt="iq9_software_setup_43_run_unity_app_box64" src="https://github.com/user-attachments/assets/82685465-f67c-496b-91ed-b08d7d0ff691" /><br>

<img width="512" height="384" alt="assembly_142_legs" src="https://github.com/user-attachments/assets/951b6eeb-2ea1-4af7-99b8-4fe071466957" /><br>

<img width="512" height="384" alt="device_run_1_test" src="https://github.com/user-attachments/assets/7755ca3f-6363-4d3c-b1d4-8d5bac4d3f1e" /><br>

<img width="512" height="384" alt="device_run_4_test" src="https://github.com/user-attachments/assets/06e1596d-2f1c-4c67-92e6-866268dc0c15" /><br>

<img width="245" height="512" alt="android_app_run_10" src="https://github.com/user-attachments/assets/2a97759b-38bd-4982-8d30-70c12c94807a" /><br>

<img width="245" height="512" alt="android_app_run_11" src="https://github.com/user-attachments/assets/0f6a0a01-796d-4043-b812-f069077ca79d" /><br>

<img width="245" height="512" alt="android_app_run_12" src="https://github.com/user-attachments/assets/87a65ec2-a7d0-488e-a119-06d461f47d33" /><br>

<img width="512" height="287" alt="iq9_run_all_3_first_exp" src="https://github.com/user-attachments/assets/d3557685-c12e-439b-a5d8-7e77eba8b1fc" /><br>

<img width="245" height="512" alt="android_app_run_13" src="https://github.com/user-attachments/assets/66226dfd-0b98-4cc0-b768-399d727815b1" /><br>

<img width="245" height="512" alt="android_app_run_15" src="https://github.com/user-attachments/assets/d6aadf72-478a-432a-a03e-a8931115b425" /><br>

<img width="245" height="512" alt="android_app_run_16" src="https://github.com/user-attachments/assets/17778011-4809-4f8c-87ab-eec377242e70" /><br>

<img width="245" height="512" alt="android_app_run_18" src="https://github.com/user-attachments/assets/c7b7520b-3ebf-4d42-8ca9-9d6941fdd8fc" /><br>

<img width="245" height="512" alt="android_app_run_20" src="https://github.com/user-attachments/assets/389b5668-211e-48fb-8a2d-334031669709" /><br>

<img width="245" height="512" alt="android_app_run_21" src="https://github.com/user-attachments/assets/0d122f17-3e89-45ae-88bb-af3079a4134e" /><br>

<img width="245" height="512" alt="android_app_run_23" src="https://github.com/user-attachments/assets/52172f53-cadf-44c0-b9b0-c78adfef56b4" /><br>

<img width="245" height="512" alt="android_app_run_24" src="https://github.com/user-attachments/assets/9785bd28-f572-4c56-9c8e-f89731986b45" /><br>

<img width="245" height="512" alt="android_app_run_25" src="https://github.com/user-attachments/assets/b596970e-d6a5-4786-84e9-8eadd844e12f" /><br>

<img width="245" height="512" alt="android_app_run_27" src="https://github.com/user-attachments/assets/e5f90fa5-4130-4b8b-9359-a9c031407761" /><br>

<img width="245" height="512" alt="android_app_run_29_cont" src="https://github.com/user-attachments/assets/21bb75cb-01e2-40b9-8317-dae7bff25729" /><br>

<img width="512" height="384" alt="device_run_11_fan_movement" src="https://github.com/user-attachments/assets/a56659fe-3b02-4fc3-9051-a75ab6cb5baa" /><br>

<img width="245" height="512" alt="android_app_run_35_cont" src="https://github.com/user-attachments/assets/09aa44a5-ec62-41f7-af10-2134301866b5" /><br>

<img width="512" height="384" alt="device_run_16_fan_movement" src="https://github.com/user-attachments/assets/59c75158-28b1-48c1-9e8f-b46f2b91e0a0" /><br>

<img width="245" height="512" alt="android_app_run_39_cont" src="https://github.com/user-attachments/assets/478ff0a7-1467-4823-a468-b24791f1e36d" /><br>

<img width="512" height="384" alt="device_run_19_fan_movement" src="https://github.com/user-attachments/assets/22dd3e3d-ccbf-46f1-befa-8a37d5273cde" /><br>

<img width="245" height="512" alt="android_app_run_46_cont" src="https://github.com/user-attachments/assets/02424532-d86c-43f1-8c72-41882fc0199d" /><br>

<img width="512" height="384" alt="device_run_23_fan_movement" src="https://github.com/user-attachments/assets/3e0770b2-1ca0-431f-bdc6-bb331f2ab9cb" /><br>

<img width="245" height="512" alt="android_app_run_51_cont" src="https://github.com/user-attachments/assets/b66cdd7f-d8cf-4d0b-9965-4f1e8c12519d" /><br>

<img width="512" height="384" alt="device_run_29_lights" src="https://github.com/user-attachments/assets/1c0c1f3d-56a4-4bfa-8fd4-0071543a1fc6" /><br>

<img width="245" height="512" alt="android_app_run_54_cont" src="https://github.com/user-attachments/assets/bac5c89e-f10d-4523-ba08-8fc6844357ae" /><br>

<img width="512" height="384" alt="device_run_37_angle_zeroing" src="https://github.com/user-attachments/assets/08a26d12-dc63-40fb-9ccf-eec4c182e540" /><br>

<img width="245" height="512" alt="android_app_run_56_cont" src="https://github.com/user-attachments/assets/572f7903-9bba-47d7-b50f-c60e74c5bc17" /><br>

<img width="512" height="384" alt="device_run_40_scnerio_2_door_open" src="https://github.com/user-attachments/assets/fe2a726e-da0f-4ca2-897a-7ce34894ee59" /><br>

<img width="512" height="287" alt="iq9_run_all_9_second_exp" src="https://github.com/user-attachments/assets/88d4bce7-40f9-49d5-a5a3-3029cade63db" /><br>

<img width="245" height="512" alt="android_app_run_58_scenerio_2" src="https://github.com/user-attachments/assets/4d920cb7-453f-41fd-92f8-ad4118b5e2e6" /><br>

<img width="512" height="384" alt="device_run_41_scnerio_2_door_closed" src="https://github.com/user-attachments/assets/81a9fef0-3973-4e86-8dca-c714a21bed74" /><br>

<img width="512" height="287" alt="iq9_run_all_12_second_exp_door_closed" src="https://github.com/user-attachments/assets/374c03a9-b4f4-47e8-aee0-44b9f7753fdc" /><br>

<img width="245" height="512" alt="android_app_run_61_scenerio_2_8b" src="https://github.com/user-attachments/assets/172abb24-b295-433b-8925-31bb2a14b129" /><br>

<img width="245" height="512" alt="android_app_run_64_scenerio_2_8b" src="https://github.com/user-attachments/assets/1323079e-14f7-4371-9d47-1341e451c129" /><br>

<img width="245" height="512" alt="android_app_run_66_scenerio_2_8b" src="https://github.com/user-attachments/assets/731ca065-3cb7-4dbb-8170-4525d344ab84" /><br>

<img width="512" height="287" alt="iq9_run_all_13_second_exp_recorded" src="https://github.com/user-attachments/assets/43f13b84-8b4f-4797-b535-08bd17f5d4de" /><br>

<img width="512" height="384" alt="device_run_42_scnerio_3_cam_move" src="https://github.com/user-attachments/assets/d20219bc-8d9e-4b38-8fed-74c0a9509c98" /><br>

<img width="512" height="287" alt="iq9_run_all_18_third_exp_camera_pos_change" src="https://github.com/user-attachments/assets/69ae649a-a164-4b76-8998-2c8923d15342" /><br>

<img width="245" height="512" alt="android_app_run_72_scenerio_3_cam_move" src="https://github.com/user-attachments/assets/c1812d3d-4c2f-4488-9a26-bf4bd509cb19" />
