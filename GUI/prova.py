import simulator
import tkinter as tk
import customtkinter as ctk

REGISTER_NAMES = [
    "RAX","RBX","RCX","RDX",
    "RSI","RDI","RSP","RBP",
    "R8","R9","R10","R11",
    "R12","R13","R14","R15",
    "RIP"
]

class SimulatorGUI:
    def __init__(self):
        ctk.set_appearance_mode("dark")
        ctk.set_default_color_theme("dark-blue")

        self.app = ctk.CTk()
        self.app.title("Simulator GUI")
        self.app.geometry("800x600")

        title = ctk.CTkLabel(self.app, text="Simulator Debug GUI", font=ctk.CTkFont(size=22, weight="bold"))
        title.pack(pady=20)

        self.bus = simulator.Bus()
        self.cpu = self.bus.getCPU()
        self.memory = self.bus.getMemory()
        self.registers = self.cpu.getRegisters()

        print(len(self.memory.getData()))

        self.memory.setDataPartial([0x2C, 0x03,
                                    0x2D, 0x05,0x00,0x00,0x00,
                                    0x80, 0xEB, 0x01,
                                    0x81, 0xE9, 0xFF,0xFF,0xFF,0xFF,
                                    0x83, 0xEA, 0x02], 0)
        print(len(self.memory.getData()))

        # Register display
        self.labels = {}
        for name in REGISTER_NAMES:
            label = ctk.CTkLabel(self.app, text=f"{name}: 0", font=ctk.CTkFont(size=16))
            label.pack(pady=5)
            self.labels[name] = label

       


        # Dentro __init__, subito dopo aver creato self.app
        self.mem_window = ctk.CTkToplevel(self.app)
        self.mem_window.title("Memory Viewer (sempre aperta)")
        self.mem_window.geometry("600x500")

        title_mem = ctk.CTkLabel(self.mem_window, text="Memory Viewer", 
                                font=ctk.CTkFont(size=20, weight="bold"))
        title_mem.pack(pady=10)

        # Frame per textbox
        frame = ctk.CTkFrame(self.mem_window)
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        self.mem_text = ctk.CTkTextbox(frame, width=580, height=450, corner_radius=10)
        self.mem_text.pack(fill="both", expand=True)

        # Aggiornamento iniziale
        self.update_memory_view()




         # ➤ Bind per SPACE (step CPU)
        self.app.bind("<space>", self.on_space_press)

        info = ctk.CTkLabel(self.app, text="Premi SPAZIO per fare uno step della CPU",
                            font=ctk.CTkFont(size=14, weight="bold"))
        info.pack(pady=10)


        self.app.mainloop()

    def update_register_labels(self):
        for name in REGISTER_NAMES:
            enum_val = getattr(simulator, name)
            value = self.registers.getReg(enum_val).raw()
            # stampa 64-bit hex
            value_hex = f"0x{value:016X}"
            self.labels[name].configure(text=f"{name}: {value_hex}")


    def on_space_press(self, event):
        print("STEP CPU")
        self.cpu.cpuStep()
        self.update_register_labels()
        self.update_memory_view()

    def update_memory_view(self):
        self.mem_text.delete("1.0", tk.END)
        mem_data = self.memory.getData()
        lines = []

        for i, byte in enumerate(mem_data):
            # Ogni riga mostra l'indice e il byte in esadecimale
            line = f"{i:04X}: {byte:02X}"
            lines.append(line)

        # Inserisco tutte le righe nel Text widget
        self.mem_text.insert(tk.END, "\n".join(lines))

        

if __name__ == "__main__":
    gui = SimulatorGUI()


