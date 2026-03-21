# Network-simualtion



## 🚀 How to Run
   1. Move to ns-3 directory:

   cd ~/ns-allinone-3.39/ns-3.39

   2. Create a symbolic link to the project folder:

   ln -s /home/minh/Network-simulation scratch/my-wifi

   (Note: You can replace "my-wifi" with any folder name)

   3. Build the project:

   ./ns3 build

   4. Run the simulation:

   ./ns3 run "my-wifi/traffic" (cach chay sau khi co phan cua Dao + Dat)
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

