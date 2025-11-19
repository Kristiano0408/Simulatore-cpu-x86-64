
import simulator

bus = simulator.Bus()
cpu = bus.getCPU()
clock = bus.getClock()
memory = bus.getMemory()
registers = cpu.getRegisters()

memory.setData([0x2C, 0x03,0x2D, 0x05,0x00,0x00,0x00,0x80, 0xEB, 0x01])

for _ in range(10):
    cpu.cpuStep()
    # You can also manipulate memory here if needed

print("Simulation completed.")  
