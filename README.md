# Network-simulation
This project aims to evaluate the performance of the CSMA/CA (Carrier Sense Multiple Access with Collision Avoidance) protocol in a WiFi Ad-hoc network using the NS-3 simulator.

## 📌 Project Goal
   - Simulate a WiFi Ad-hoc network (2–30 nodes)
   - Disable RTS/CTS mechanism
   - Analyze performance metrics:
      - Throughput
      - Delay
      - Packet Delivery Ratio (PDR)

## 🚀 How to Run
   1. Move to ns-3 directory:
      ```bash
      cd ~/ns-allinone-3.39/ns-3.39
      ```

   2. Create a symbolic link to the project folder:

      ```bash
      ln -s /home/minh/Network-simulation scratch/my-wifi
      ```
    

   (Note: You can replace "my-wifi" with any folder name)

   3. Build the project:
      ```bash
      ./ns3 build  
      ```

   4. Run the simulation:
      ```bash
      ./ns3 run "my-wifi/traffic" 
      ```

   5.  Create automation script
      ### 5.1. Create file
            ```bash
            touch run_all.sh
            ```
      ### 5.2. Open file
            ```bash
            nano run_all.sh
            ```
      ### 5.3. Paste this script
            ```bash
               #!/bin/bash

                  FINAL_CSV="full_results.csv"

                  echo "numNodes,txPackets,rxPackets,delaySumSeconds,rxBytes,throughputKbps,avgDelayMs,lossRatioPercent" > $FINAL_CSV

                  NODE_LIST=($(seq 2 30))

                  DISTANCE=10
                  SIM_TIME=5

                  echo "Starting Simulation..."

                  for n in "${NODE_LIST[@]}"
                  do
                     echo "[STAGE] Running for $n nodes..."

                     ./ns3 run "scratch/my-wifi/traffic --numNodes=$n --distance=$DISTANCE --simTime=$SIM_TIME"

                     if [ -f scratch/my-wifi/summary.csv ]; then
                        tail -n +2 scratch/my-wifi/summary.csv >> $FINAL_CSV
                        rm scratch/my-wifi/summary.csv
                        echo "[SUCCESS] Data for $n nodes collected."
                     else
                        echo "[ERROR] summary.csv not found!"
                     fi

                     echo "-----------------------------------------------"
                     sleep 1
                  done

                  echo "Results saved in $FINAL_CSV"
            ```
      
      6. Run All Experiments
         ### 6.1. Make the script executable
            ```bash
                  chmod +x run_all.sh
            ```
         
         ### 6.2. Execute the script
            ```bash
                  ./run_all.sh
            ```
## ⚠️ Note

   Each simulation must contain ONLY ONE main() function.

   Do NOT place multiple .cc files with main() in the same folder (e.g., scratch/my-wifi/),
   because ns-3 will compile them together into a single program, causing a "multiple definition of main" error.

   ❌ Wrong:
   scratch/my-wifi/
      traffic.cc        (has main)
      wifi_setup.cc     (has main)

   ✅ Correct options:

   Option 1 (Recommended):
   Place only ONE .cc file in the folder:
   scratch/my-wifi/
      traffic.cc

   Option 2:
   Use separate folders for each simulation:
   scratch/traffic/traffic.cc
   scratch/wifi_setup/wifi_setup.cc

   👉 Rule: 1 folder = 1 program = 1 main()

