
import simulator

bus = simulator.Bus()
cpu = bus.getCPU()
clock = bus.getClock()
memory = bus.getMemory()
registers = cpu.getRegisters()

memory.setData([0x2C, 0x03,
                0x2D, 0x05,0x00,0x00,0x00,
                0x80, 0xEB, 0x01])

for _ in range(1):
    print("----------------------------------------------------------")
    print(f"Clock cycles: {clock.getCycles()+1}")
    cpu.cpuStep()
    registers_values = {simulator.Register.RAX: registers.getReg(simulator.Register.RAX).raw(),
                        simulator.Register.RBX: registers.getReg(simulator.Register.RBX).raw(),
                        simulator.Register.RCX: registers.getReg(simulator.Register.RCX).raw(),
                        simulator.Register.RDX: registers.getReg(simulator.Register.RDX).raw(),
                        simulator.Register.RSP: registers.getReg(simulator.Register.RSP).raw(),
                        simulator.Register.RBP: registers.getReg(simulator.Register.RBP).raw(),
                        simulator.Register.RSI: registers.getReg(simulator.Register.RSI).raw(),
                        simulator.Register.RDI: registers.getReg(simulator.Register.RDI).raw(),
                        simulator.Register.R8: registers.getReg(simulator.Register.R8).raw(),
                        simulator.Register.R9: registers.getReg(simulator.Register.R9).raw(),
                        simulator.Register.R10: registers.getReg(simulator.Register.R10).raw(),
                        simulator.Register.R11: registers.getReg(simulator.Register.R11).raw(),
                        simulator.Register.R12: registers.getReg(simulator.Register.R12).raw(),
                        simulator.Register.R13: registers.getReg(simulator.Register.R13).raw(),
                        simulator.Register.R14: registers.getReg(simulator.Register.R14).raw(),
                        simulator.Register.R15: registers.getReg(simulator.Register.R15).raw(),
                        simulator.Register.RIP: registers.getReg(simulator.Register.RIP).raw()}
    print("Register values:")
    for reg, val in registers_values.items():
        print(f"{reg.name}: {val:#018x}")

    # You can also manipulate memory here if needed

print("Simulation completed.")  
