import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("full_30_nodes.csv")

nodes = data["numNodes"]
throughput = data["throughputKbps"]
delay = data["avgDelayMs"]
loss = data["lossRatioPercent"]
tx = data["txPackets"]
rx = data["rxPackets"]

# Throughput
plt.figure()
plt.plot(nodes, throughput, marker='o')
plt.xlabel("Number of Nodes")
plt.ylabel("Throughput (Kbps)")
plt.title("Throughput vs Number of Nodes")
plt.grid(True)
plt.tight_layout()
plt.savefig("throughput_vs_nodes.png")

# Delay
plt.figure()
plt.plot(nodes, delay, marker='o')
plt.xlabel("Number of Nodes")
plt.ylabel("Average Delay (ms)")
plt.title("Delay vs Number of Nodes")
plt.grid(True)
plt.tight_layout()
plt.savefig("delay_vs_nodes.png")

# Loss
plt.figure()
plt.plot(nodes, loss, marker='o')
plt.xlabel("Number of Nodes")
plt.ylabel("Loss Ratio (%)")
plt.title("Packet Loss vs Number of Nodes")
plt.grid(True)
plt.tight_layout()
plt.savefig("loss_vs_nodes.png")

# TX vs RX packets
plt.figure()
plt.plot(nodes, tx, marker='o', label='txPackets')
plt.plot(nodes, rx, marker='s', label='rxPackets')
plt.xlabel("Number of Nodes")
plt.ylabel("Packets")
plt.title("Transmitted vs Received Packets")
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("tx_rx_packets.png")

plt.show()