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




##CERCARE DI MIGLIOARE PERFORMANCE GUI (USARE TEXTBOX INVECE DI LABEL PER MEMORIA? AGGIORNARE SOLO LINEE MODIFICATE?)

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

        self.memory.setDataPartial([
            0x2C, 0x03,
            0x2D, 0x05,0x00,0x00,0x00,
            0x80, 0xEB, 0x01,
            0x81, 0xE9, 0xFF,0xFF,0xFF,0xFF,
            0x83, 0xEA, 0x02
        ], 0)

        self.mem_window_creation()
        self.reg_window_creation()

        self.pipeline_stages = [
            "FETCH",
            "DECODE",
            "OPERAND FETCH",
            "EXECUTE",
            "MEMORY",
            "WRITEBACK"
        ]

        self.pipeline_buffers = [
            "F/D",
            "D/OF",
            "OF/EX",
            "EX/MEM",
            "MEM/WB"
        ]

        self.pipeline_window_creation()

        # Bind dello step CPU
        self.app.bind("<space>", self.on_space_press)

        info = ctk.CTkLabel(self.app,
                            text="Premi SPAZIO per fare uno step della CPU",
                            font=ctk.CTkFont(size=14, weight="bold"))
        info.pack(pady=10)

        #al momento stampiamo 
        

        self.app.mainloop()

    # --- REGISTER WINDOW ---
    def reg_window_creation(self):
        self.reg_window = ctk.CTkToplevel(self.app)
        self.reg_window.title("Register Viewer")
        self.reg_window.geometry("300x800")

        title_reg = ctk.CTkLabel(self.reg_window,
                                 text="Register Viewer",
                                 font=ctk.CTkFont(size=20, weight="bold"))
        title_reg.pack(pady=10)

        self.labels = {}
        for name in REGISTER_NAMES:
            label = ctk.CTkLabel(self.reg_window,
                                 text=f"{name}: 0",
                                 font=ctk.CTkFont(size=16))
            label.pack(pady=5)
            self.labels[name] = label

    # --- MEMORY WINDOW ---
    def mem_window_creation(self):
        self.mem_window = ctk.CTkToplevel(self.app)
        self.mem_window.title("Memory Viewer")
        self.mem_window.geometry("600x500")

        title_mem = ctk.CTkLabel(self.mem_window,
                                 text="Memory Viewer",
                                 font=ctk.CTkFont(size=20, weight="bold"))
        title_mem.pack(pady=10)

        frame = ctk.CTkFrame(self.mem_window)
        frame.pack(fill="both", expand=True, padx=10, pady=10)

        self.mem_text = ctk.CTkTextbox(frame, width=580, height=450, corner_radius=10)
        self.mem_text.pack(fill="both", expand=True)

        self.update_memory_view()

    # --- UPDATE REGISTERS ---
    def update_register_labels(self):
        for name in REGISTER_NAMES:
            enum_val = getattr(simulator, name)
            value = self.registers.getReg(enum_val).raw()
            value_hex = f"0x{value:016X}"
            self.labels[name].configure(text=f"{name}: {value_hex}")

    # --- STEP CPU ---
    def on_space_press(self, event):
        print("STEP CPU")
        self.cpu.cpuStep()
        self.update_register_labels()
        self.update_pipeline_view()
        # (non aggiorniamo la memoria qui: lagga!)

    # --- MEMORY VIEWER UPDATE ---
    def update_memory_view(self):
        self.mem_text.delete("1.0", tk.END)
        mem_data = self.memory.getData()

        lines = []
        for i, byte in enumerate(mem_data):
            lines.append(f"{i:04X}: {byte:02X}")

        self.mem_text.insert(tk.END, "\n".join(lines))

    # --- PIPELINE VIEWER CREATION ---
    def pipeline_window_creation(self):
        self.pipeline_window = ctk.CTkToplevel(self.app)
        self.pipeline_window.title("Pipeline Viewer")
        self.pipeline_window.geometry("700x550")

        title_pipe = ctk.CTkLabel(
            self.pipeline_window,
            text="Pipeline Viewer",
            font=ctk.CTkFont(size=20, weight="bold")
        )
        title_pipe.pack(pady=10)

        main_frame = ctk.CTkFrame(self.pipeline_window)
        main_frame.pack(fill="both", expand=True, padx=20, pady=20)

        self.stage_labels = {}
        self.buffer_labels = {}

        # --- STAGES ---
        stages_frame = ctk.CTkFrame(main_frame)
        stages_frame.pack(side="left", fill="both", expand=True, padx=10)

        stages_title = ctk.CTkLabel(stages_frame, text="Pipeline Stages", font=ctk.CTkFont(size=16, weight="bold"))
        stages_title.pack(pady=5)

        for stage in self.pipeline_stages:
            frame = ctk.CTkFrame(stages_frame)
            frame.pack(fill="x", pady=4)

            label_title = ctk.CTkLabel(frame, text=stage, width=120, anchor="w")
            label_title.pack(side="left", padx=5)

            label_text = ctk.CTkLabel(frame, text="(empty)", anchor="w")
            label_text.pack(side="left", padx=5)

            self.stage_labels[stage] = label_text

        # --- BUFFERS ---
        buffers_frame = ctk.CTkFrame(main_frame)
        buffers_frame.pack(side="right", fill="both", expand=True, padx=10)

        buffers_title = ctk.CTkLabel(buffers_frame, text="Pipeline Buffers", font=ctk.CTkFont(size=16, weight="bold"))
        buffers_title.pack(pady=5)

        for buf in self.pipeline_buffers:
            frame = ctk.CTkFrame(buffers_frame)
            frame.pack(fill="x", pady=4)

            label_title = ctk.CTkLabel(frame, text=buf, width=80, anchor="w")
            label_title.pack(side="left", padx=5)

            label_text = ctk.CTkLabel(frame, text="(empty)", anchor="w")
            label_text.pack(side="left", padx=5)

            self.buffer_labels[buf] = label_text

        # primo update
        self.update_pipeline_view()


    # --- PIPELINE VIEWER UPDATE ---
    def update_pipeline_view(self):
        try:
            pipe = self.cpu.getPipeline()
            stages = [pipe.getFetchStage(),
                      pipe.getDecodeStage(),
                      pipe.getOperandFetchStage(),
                      pipe.getExecuteStage(),
                      pipe.getMemoryStage(),
                      pipe.getWriteBackStage()]
            buffers = [pipe.getFetchDecodeBuffer(),
                       pipe.getDecodeOperandFetchBuffer(),
                       pipe.getOperandFetchExecuteBuffer(),
                       pipe.getExecuteMemoryBuffer(),
                       pipe.getMemoryWriteBackBuffer()]

        except:
            stages = [""] * len(self.pipeline_stages)
            buffers = [""] * len(self.pipeline_buffers)

        # update stages
        for name, stage in zip(self.pipeline_stages, stages):
            status_name = stage.getStatus().name
            instr_id = "(empty)"  # default se vuoto

            if hasattr(stage, "peekInstruction") and not isinstance(stage, simulator.DecodeStage):
                instr = stage.peekInstruction()
                if instr is not None and not stage.isInstructionEmpty(instr):
                    instr_id = instr.getInstructionId()
            elif hasattr(stage, "getInstructionToDecode"):
                instr_info = stage.getInstructionToDecode() 
                if instr_info is not None:
                    instr_id = instr_info.instructionId if instr_info.instructionId != 0 else "(empty)"
            elif hasattr(stage, "getCurrentInstructionInfo"):
                instr_id = ("fetching ...")
            

            value = f"{status_name} --- {instr_id}"
            self.stage_labels[name].configure(text=value)

        # update buffers
        for name,  buf in zip(self.pipeline_buffers, buffers):
            instr_id = "(empty)"  # default se vuoto
            if buf.valid == False:
                pass
            elif hasattr(buf, "peekInstruction"):
                instr = buf.peekInstruction()
                if instr is not None:
                    instr_id = instr.getInstructionId()
            elif hasattr(buf, "instructionInfo"):
                instr_info = buf.instructionInfo
                if instr_info is not None:
                    instr_id = instr_info.instructionId if instr_info.instructionId != "0" else "(empty)"

            self.buffer_labels[name].configure(text=instr_id)

        # next update
        if hasattr(self, "pipeline_window") and self.pipeline_window.winfo_exists():
            self.pipeline_window.after(500, self.update_pipeline_view)




if __name__ == "__main__":
    gui = SimulatorGUI()
